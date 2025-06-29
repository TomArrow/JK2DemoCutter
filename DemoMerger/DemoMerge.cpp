#include "demoCut.h"
#include "DemoReader.h"
#include "DemoReaderLight.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <set>
#include <chrono>
#include <jkaStuff.h>
#include "otherGameStuff.h"
#include "otherGameStuff2.h"

#include "popl.hpp"
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


class ExtraMergeOptions {
public:
	int persistEntitiesMaxDelay = 0; // Try to find "gaps" in entity visibility and repeat the last state of the entity.
	int interpolateEntitiesMaxDelay = 0; // Try to find "gaps" in entity visibility and repeat the last state of the entity.
	int interpolatePlayerStateMaxDelay = 0; // Try to find "gaps" in entity visibility and repeat the last state of the entity.
	bool skipMainPlayerDeadFrames = false; // For reframes: Skip frames that don't contain main player.
	bool visAll = false; // For reframes: Everything is visible, not just combined visibility mask of the different source demos
};


// When we combine demos from multiple angles, inevitably we will encounter combining events from playerstates and entitystates.
// Sadly the two operate rather differently. To not cause myself a major headache, we'll just accumulate and rewrite all player events.
// This means it won't be a perfect merge but honestly I think it's the best I can do.
// What will most likely get lost is the exact timing/duration of the event value being set in entities and since each player can have
// up to 2 events per frame, the order of them might also get lost in my head logic. Because the server doesn't send a snapshot for each
// server frame, an event may happen on one server frame and another on the second server frame and if only the second server frame is sent
// to the client, we might end up with only the player entity having been assigned the event and it having been overwritten on the second server frame.
// Yet the playerstate still has both and looking at the playerstate with 2 new events it might appear as though there should be two entities carrying
// these events yet there aren't.
typedef struct playerEvent_t {
	int				eventValue;
	int				eventParm;
	int64_t			timeFirstSeen;
	qboolean		wasNotExternalEvent; // We can only prove that it wasn't an external event. We can't prove that it was (because when converting to entity, either external or sequence one can be converted)

	// Tracking of where to write the event in output.
	// Tracking when writing to playerstate:
	qboolean		isAssignedPlayerState;
	int				sequenceNum;			// We don't assign sequenceNum if it's assigned as external, to keep stuff tidy and logical (?)
	qboolean		assignedAsExternal;
	// Tracking when writing to entities
	qboolean		isAssignedEntity;
	int				assignedEntityNum;
};

#define PLAYER_MAX_EVENT_COUNT 3

typedef struct playerEventData_t {
	int				eventSequence;	// We actually have to just start at 0 I guess and just do our own numbering, because trying to keep things consistent between playerstates, entitystates, changing angles and appearing/disappearing entities might just be too much...
	playerEvent_t	events[PLAYER_MAX_EVENT_COUNT];
};

playerEventData_t playerEventData[MAX_CLIENTS];


class DemoReaderTrackingWrapper {
public:
	DemoReader reader;
	int packetsUsed = 0;
	std::vector<std::string> commandDupesToFilter;
	SnapshotInfoMapIterator currentSnapIt, nextSnapIt, nullIt, snapshotSafeToClear;
	int snapshotsProcessedSinceClear = 0;
	int snapshotsProcessedSinceSnapshotSafeToClearSet = 0;
};

class DemoReaderLightTrackingWrapper {
public:
	DemoReaderLight reader;
	std::vector<MetaEventItemAbsolute> metaEventDupesToFilter;
	SnapshotTimesIterator currentSnapIt, nextSnapIt, nullIt;
};






#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


// Compare time of existing and new info to decide if the new info should override the old info
// Generally, any time BEFORE current time is always preferred over a time AFTER current time (aka prefer old playerstate/entitystate info over future one)
// as this is consistent with the behavior of dropped frames or similar.
// Otherwise, the time closer to curTime is best.
template<class T>
inline bool IsBetterOrEqualTime(T& curTime, T& oldTime, T& newTime) {
	return
		oldTime == -1 || // At least equal condition.
		newTime == curTime || // At least equal condition.
		newTime == oldTime || // Equal condition.
		newTime <= curTime && oldTime > curTime && newTime != -1 || // New time is before current time. Always prefer past times over future times. Better condition.
		newTime > curTime && oldTime > curTime && newTime <= oldTime || // If BOTH times are in the future, take the closer one. At least equal condition.
		newTime < curTime && oldTime < curTime && newTime >= oldTime; // If both in the past, newer is better. At least equal condition.
}

// Same as above but require it to be actually better, not just equal
template<class T>
inline bool IsBetterTime(T& curTime, T& oldTime, T& newTime) {
	return
		newTime != -1 && oldTime == -1 ||
		newTime == curTime && oldTime != curTime || // Better condition.
		newTime <= curTime && oldTime > curTime && newTime != -1 || // New time is before current time. Always prefer past times over future times. Better condition.
		newTime > curTime && oldTime > curTime && newTime < oldTime || // If BOTH times are in the future, take the closer one. Better condition.
		newTime < curTime && oldTime < curTime && newTime > oldTime; // If both in the past, newer is better. Better condition.
}


qboolean demoMerge( const char* outputName, std::vector<std::string>* inputFiles, std::string* reframeSearchString, const ExtraMergeOptions& opts) {
	fileHandle_t	newHandle = 0;
	char			outputNameNoExt[MAX_OSPATH];
	char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	//char* ext;
	char ext[7]{};
	//char originalExt[7]{};
	demoType_t		demoType;
	qboolean createCompressedOutput;

	//strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);

	demoCutGetDemoType(outputName,ext, outputNameNoExt ,&demoType,&createCompressedOutput);


	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	/*ext = (char*)outputName + strlen(outputName) - 6;
	if (!*ext) {
		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	else {
		// TODO Maybe just check output files too and judge based on them?
		demoType = DM_15;
		ext = ".dm_15";
	}*/

	memset(&demo, 0, sizeof(demo));
	memset(&playerEventData, 0, sizeof(playerEventData));

	std::vector<std::unique_ptr<DemoReaderTrackingWrapper>> demoReaders;
	std::cout << "loading up demos...";
	int startTime = INT_MAX;
	demoReaders.reserve(inputFiles->size()); // This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	qboolean needPrePass = qfalse;
	for (int i = 0; i < inputFiles->size(); i++) {
		std::cout << i<<"...";
		demoReaders.emplace_back(std::make_unique<DemoReaderTrackingWrapper>());
		if (!demoReaders.back()->reader.LoadDemo((*inputFiles)[i].c_str())) {
			std::cerr << "Failed to open " << (*inputFiles)[i] << ". Aborting.\n";
			return qfalse;
		}
		needPrePass = (qboolean)(needPrePass || demoReaders.back()->reader.GetMetaEventCount());
		demoReaders.back()->snapshotSafeToClear = demoReaders.back()->currentSnapIt = demoReaders.back()->nextSnapIt = demoReaders.back()->nullIt = demoReaders.back()->reader.SnapNullIt();
		startTime = std::min(startTime, demoReaders.back()->reader.GetFirstSnapServerTime()); // Find earliest serverTime from all source demos and start there.
	}
	std::vector<MetaEventItemAbsolute> newMetaEvents;
	std::stringstream metaEventString;
	if (needPrePass) {
		std::cout << "... meta events found, doing prepass ...";
		std::vector<std::unique_ptr<DemoReaderLightTrackingWrapper>> pingDemoReaders;
		pingDemoReaders.reserve(inputFiles->size());// This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
		for (int i = 0; i < inputFiles->size(); i++) {
			pingDemoReaders.emplace_back(std::make_unique<DemoReaderLightTrackingWrapper>());
			pingDemoReaders.back()->reader.LoadDemo((*inputFiles)[i].c_str());
			pingDemoReaders.back()->reader.ReadToEnd();
			pingDemoReaders.back()->currentSnapIt = pingDemoReaders.back()->nextSnapIt = pingDemoReaders.back()->nullIt = pingDemoReaders.back()->reader.SnapNullIt();
		}
		int prePassTime = startTime;

		std::vector<metaEventType_t> metaEventsToAdd;


		// Do a mini version of the main loop later on to combine & deduplicate meta events.
		while (1) {
			metaEventsToAdd.clear();
			qboolean allSourceDemosFinished = qtrue;
			int nonSkippedDemoIndex = 0;

			// Assemble the entities etc
			for (int i = 0; i < pingDemoReaders.size(); i++) {
				if (pingDemoReaders[i]->reader.SeekToServerTime(prePassTime)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.

					SnapshotTimesIterator snapInfoHereIterator = pingDemoReaders[i]->nullIt;
					if (pingDemoReaders[i]->nextSnapIt != pingDemoReaders[i]->nullIt) {
						if (*pingDemoReaders[i]->nextSnapIt== prePassTime) {
							snapInfoHereIterator = pingDemoReaders[i]->nextSnapIt;
						}
					}
					else {
						snapInfoHereIterator = pingDemoReaders[i]->reader.GetSnapshotTimeAtServerTimeIterator(prePassTime);
					}
					int serverTimeHere = snapInfoHereIterator == pingDemoReaders[i]->nullIt ? -1 : *snapInfoHereIterator;
					qboolean snapIsInterpolated = qfalse;
					if (serverTimeHere == -1) {
						SnapshotTimesIterator thisDemoLastSnapshotIt = pingDemoReaders[i]->nullIt;
						SnapshotTimesIterator thisDemoNextSnapshotIt = pingDemoReaders[i]->nullIt;

						// Find last
						if (pingDemoReaders[i]->currentSnapIt != pingDemoReaders[i]->nullIt) {
							thisDemoLastSnapshotIt = pingDemoReaders[i]->currentSnapIt;
						}
						else {

							int thisDemoLastServerTime = pingDemoReaders[i]->reader.GetLastServerTimeBeforeServerTime(prePassTime);
							thisDemoLastSnapshotIt = pingDemoReaders[i]->reader.GetSnapshotTimeAtServerTimeIterator(thisDemoLastServerTime);
						}

						// Find next
						if (pingDemoReaders[i]->nextSnapIt != pingDemoReaders[i]->nullIt) {
							thisDemoNextSnapshotIt = pingDemoReaders[i]->nextSnapIt;
						}
						else {

							int thisDemoNextServerTime = pingDemoReaders[i]->reader.GetFirstServerTimeAfterServerTime(prePassTime);
							thisDemoNextSnapshotIt = pingDemoReaders[i]->reader.GetSnapshotTimeAtServerTimeIterator(thisDemoNextServerTime);
						}


						if (thisDemoLastSnapshotIt == pingDemoReaders[i]->nullIt || thisDemoNextSnapshotIt == pingDemoReaders[i]->nullIt) continue;

						snapInfoHereIterator = thisDemoLastSnapshotIt;
						serverTimeHere = *thisDemoLastSnapshotIt;
						// TODO Do actual interpolation instead of just copying last one. Don't copy entities that are in previous but not in next.
						snapIsInterpolated = qtrue;
					}
					else {
						pingDemoReaders[i]->currentSnapIt = snapInfoHereIterator;
					}


					// Get new commands
					std::vector<MetaEventItemAbsolute> newMetaEventsHere = pingDemoReaders[i]->reader.GetNewMetaEventsAtServerTime(prePassTime);
					for (int c = 0; c < newMetaEventsHere.size(); c++) {

						// New handling to avoid dupes
						qboolean isADupe = qfalse;
						for (int sc = 0; sc < pingDemoReaders[i]->metaEventDupesToFilter.size(); sc++) {
							if (pingDemoReaders[i]->metaEventDupesToFilter[sc].type == newMetaEventsHere[c].type) {
								isADupe = qtrue;
								pingDemoReaders[i]->metaEventDupesToFilter.erase(pingDemoReaders[i]->metaEventDupesToFilter.begin() + sc);
								break;
							}
						}

						if (!isADupe) {
							metaEventsToAdd.push_back(newMetaEventsHere[c].type);
							for (int sd = 0; sd < pingDemoReaders.size(); sd++) {
								if (sd != i) {
									pingDemoReaders[sd]->metaEventDupesToFilter.push_back(newMetaEventsHere[c]);
								}
							}
						}
					}

					if (!snapIsInterpolated) { // We already parsed this. If we didn't find a dupe of an older command now, we won't find it later either. (and it might be stuff that was only sent to some of the clients, like team chat)

						pingDemoReaders[i]->metaEventDupesToFilter.clear();
					}


				}
				if (!pingDemoReaders[i]->reader.EndReachedAtServerTime(prePassTime)) {
					allSourceDemosFinished = qfalse;
				}
			}


			for (int nme = 0; nme < metaEventsToAdd.size(); nme++) {
				int64_t newMetaEventDemoTime = prePassTime - startTime;
				if (newMetaEventDemoTime < 0) {
					std::cerr << "Error merging meta events, new demo time < 0:" << newMetaEventDemoTime << "\n";
					continue; // Probably bugged due to demo being around a map change or such
				}
				MetaEventItemAbsolute newItem;
				newItem.timeFromDemoStart = newMetaEventDemoTime;
				newItem.type = metaEventsToAdd[nme];
				newItem.serverTime = prePassTime;
				newItem.serverTimeCorrelated = qtrue;
				newMetaEvents.push_back(newItem);
			}

			int oldTime = prePassTime;
			prePassTime = INT_MAX;
			for (int i = 0; i < pingDemoReaders.size(); i++) {
				if (pingDemoReaders[i]->currentSnapIt != pingDemoReaders[i]->nullIt) {

					pingDemoReaders[i]->nextSnapIt = pingDemoReaders[i]->reader.GetFirstSnapshotTimeAfterSnapshotTimeIterator(pingDemoReaders[i]->currentSnapIt, oldTime);
				}
				else if (pingDemoReaders[i]->nextSnapIt == pingDemoReaders[i]->nullIt || *pingDemoReaders[i]->nextSnapIt <= oldTime) {
					int thisDemoNextServerTime = pingDemoReaders[i]->reader.GetFirstServerTimeAfterServerTime(oldTime);
					pingDemoReaders[i]->nextSnapIt = pingDemoReaders[i]->reader.GetSnapshotTimeAtServerTimeIterator(thisDemoNextServerTime);
				}
				if (pingDemoReaders[i]->nextSnapIt != pingDemoReaders[i]->nullIt) {

					int nextTimeThisDemo = *pingDemoReaders[i]->nextSnapIt;
					prePassTime = std::min(prePassTime, nextTimeThisDemo); // Find nearest serverTime of all the demos.
				}
			}


			if (allSourceDemosFinished || prePassTime == INT_MAX) break;
		}
		std::sort(newMetaEvents.begin(), newMetaEvents.end(), [](MetaEventItemAbsolute a, MetaEventItemAbsolute b)
			{
				return a.timeFromDemoStart < b.timeFromDemoStart;
			}
		);

		pingDemoReaders.clear();

		for (int me = 0; me < newMetaEvents.size(); me++) {
			metaEventString << (me > 0 ? "," : "") << metaEventKeyNames[newMetaEvents[me].type] << newMetaEvents[me].timeFromDemoStart;
		}
	}
	std::cout << "done.";


	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	//int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(demoType);
	for (int i = 0; i < maxAllowedConfigString; i++) {
		tmpConfigString = demoReaders[0]->reader.GetConfigString(i,&tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString,demoType);
		}
	}

	//std::map<int, int> lastSpectatedClientNums; // Need this for later. - do we  really?



	int reframeClientNum = -1; // By default there is no reframing but it can be enabled as an extra option
	// Copy over player config strings
	if (reframeSearchString) {
		std::cout << "\n";
	}
	for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i]->reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
			//int spectatedClient = demoReaders[i]->reader.GetCurrentPlayerState().clientNum;
			//lastSpectatedClientNums[i] = spectatedClient;			
			//if (i >= MAX_CLIENTS) continue; // We don't have names/configstrings for players > 32
			//tmpConfigString = demoReaders[i]->reader.GetPlayerConfigString(spectatedClient,&tmpConfigStringMaxLength);
			//if (strlen(tmpConfigString)) {
				//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS+i, tmpConfigString);
			//}
			if (reframeSearchString) {
				int reframeClientNumHere = demoReaders[i]->reader.getClientNumForDemo(reframeSearchString);
				std::cout << "\n";
				if (reframeClientNumHere != -1 && reframeClientNum != -1 && reframeClientNumHere != reframeClientNum) {
					std::cout << "Reframe clientnum mismatch: " << reframeClientNum << " vs " << reframeClientNumHere << ". Discarding latter.\n";
				}
				if (reframeClientNum == -1 && reframeClientNumHere != -1) {
					reframeClientNum = reframeClientNumHere;
				}
			}
		}
	}
	
	//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_LEVEL_START_TIME, "10000");

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING_MAX];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy, demoType);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially merged using JK2DemoCutter tools.", demoType);

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName), "%s%s", outputNameNoExt, ext);
	while (FS_FileExists(newName)) {
		Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputNameNoExt, dupeIterator, ext);
		dupeIterator++;
	}

	newHandle = FS_FOpenFileWrite(newName,createCompressedOutput ? FILECOMPRESSION_LZMA : FILECOMPRESSION_NONE);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	if (newMetaEvents.size()) {
		// TODO: Save "oto": Original total offset. Throughout all cuts, what's the offset from the original file now?

		rapidjson::Document* jsonMetaDocument = new rapidjson::Document();
		jsonMetaDocument->SetObject();
		std::string meString = metaEventString.str();
		const char* meStringC = meString.c_str();
		jsonMetaDocument->AddMember("me", rapidjson::Value(meStringC, jsonMetaDocument->GetAllocator()).Move(), jsonMetaDocument->GetAllocator());
		jsonMetaDocument->AddMember("wr", "DemoMerger", jsonMetaDocument->GetAllocator());
		jsonMetaDocument->AddMember("fd", true, jsonMetaDocument->GetAllocator()); // fake demo

		for (int i = 0; i < demoReaders.size();i++) {
			demoReaders[i]->reader.copyMetadataTo(jsonMetaDocument,qfalse);
		}
		const char* noteName = jsonGetRealMetadataKeyName(jsonMetaDocument,"note");
		if (noteName && _stricmp(noteName,"note")) {

			rapidjson::Value newNameRapid;
			newNameRapid.SetString("note", strlen("note"));
			std::cout << "Making one of the original notes the new note.\n";
			std::string note = (*jsonMetaDocument)[noteName].GetString();
			const char* noteC = note.c_str();
			jsonMetaDocument->AddMember(newNameRapid, rapidjson::Value(noteC, jsonMetaDocument->GetAllocator()).Move(), jsonMetaDocument->GetAllocator());
		}

		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		jsonMetaDocument->Accept(writer);
		const char* finalJsonMetaString = sb.GetString();
		demoCutWriteEmptyMessageWithMetadata(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType, createCompressedOutput, finalJsonMetaString);
		delete jsonMetaDocument;
		jsonMetaDocument = NULL;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	int time = startTime; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	SnapshotEntities playerEntities;
	SnapshotEntities playerEntitiesOld;
	SnapshotEntities futureEntityStates;
	SnapshotEntities pastEntityStates;
	std::vector<std::string> commandsToAdd;
	std::vector<Event> eventsToAdd;
	playerState_t tmpPS,tmpPS2,tmpPS3, mainPlayerPS, mainPlayerPSOld;
	entityState_t tmpES;// , mainPlayerPSSourceES;
	int currentCommand = 1;
	// Start writing snapshots.
	qboolean isFirstSnapshot = qtrue;
	std::stringstream ss;
	int framesWritten = 0;
	int framesSkippedDeadFrames = 0;
	byte	areamasHere[MAX_MAP_AREA_BYTES];

	while(1){
		futureEntityStates.clear();
		commandsToAdd.clear();
		eventsToAdd.clear();
		qboolean allSourceDemosFinished = qtrue;
		playerEntities.clear();
		int nonSkippedDemoIndex = 0;
		static qboolean entityIsInterpolated[MAX_GENTITIES];
		static int entityServerTime[MAX_GENTITIES];
		Com_Memset(entityIsInterpolated, 0, sizeof(entityIsInterpolated));
		Com_Memset(entityServerTime, 0, sizeof(entityServerTime));
		if (reframeClientNum != -1) {
			Com_Memset(&mainPlayerPS, 0, sizeof(mainPlayerPS));
		}
		Com_Memset(areamasHere, 255, sizeof(areamasHere));
		qboolean mainPlayerPSIsInterpolated = qfalse;
		//qboolean mainPlayerPSIsRealPS = qfalse;
		int mainPlayerServerTime = -1;
		int mainPlayerRealPSPartsServerTime = -1;
		std::set<int> extraPlayerEventEntities[MAX_CLIENTS];
		
		// Clear expired player events and also clear info about extra player event entities while at it
		for (int i = 0; i < MAX_CLIENTS; i++) {

			playerEventData_t* thisPlayerEventData = &playerEventData[i];
			for (int se = 0; se < PLAYER_MAX_EVENT_COUNT; se++) {
				if (thisPlayerEventData->events[se].timeFirstSeen < time-EVENT_VALID_MSEC && thisPlayerEventData->events[se].eventValue) {
					memset(&thisPlayerEventData->events[se], 0, sizeof(thisPlayerEventData->events[se]));
				}
			}
			extraPlayerEventEntities[i].clear();
		}

		// Assemble the entities etc
		for (int i = 0; i < demoReaders.size(); i++) {
			if (demoReaders[i]->reader.SeekToServerTime(time)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
				
				SnapshotInfoMapIterator snapInfoHereIterator = demoReaders[i]->nullIt;
				if (demoReaders[i]->nextSnapIt != demoReaders[i]->nullIt){
					if (demoReaders[i]->nextSnapIt->second->serverTime == time) {
						snapInfoHereIterator = demoReaders[i]->nextSnapIt;
					}
				}
				else {
					snapInfoHereIterator = demoReaders[i]->reader.GetSnapshotInfoAtServerTimeIterator(time);
				}
				SnapshotInfo* snapInfoHere = snapInfoHereIterator == demoReaders[i]->nullIt ? NULL : snapInfoHereIterator->second.get();
				qboolean snapIsInterpolated = qfalse;
				if (!snapInfoHere) {
					SnapshotInfoMapIterator thisDemoLastSnapshotIt = demoReaders[i]->nullIt;
					SnapshotInfoMapIterator thisDemoNextSnapshotIt = demoReaders[i]->nullIt;

					// Find last
					if (demoReaders[i]->currentSnapIt != demoReaders[i]->nullIt) {
						thisDemoLastSnapshotIt = demoReaders[i]->currentSnapIt;
					}
					else {

						int thisDemoLastServerTime = demoReaders[i]->reader.GetLastServerTimeBeforeServerTime(time);
						thisDemoLastSnapshotIt = demoReaders[i]->reader.GetSnapshotInfoAtServerTimeIterator(thisDemoLastServerTime);
					}

					// Find next
					if (demoReaders[i]->nextSnapIt != demoReaders[i]->nullIt) {
						thisDemoNextSnapshotIt = demoReaders[i]->nextSnapIt;
					}
					else {

						int thisDemoNextServerTime = demoReaders[i]->reader.GetFirstServerTimeAfterServerTime(time);
						thisDemoNextSnapshotIt = demoReaders[i]->reader.GetSnapshotInfoAtServerTimeIterator(thisDemoNextServerTime);
					}

					
					if (thisDemoLastSnapshotIt == demoReaders[i]->nullIt || thisDemoNextSnapshotIt == demoReaders[i]->nullIt) continue;

					snapInfoHereIterator = thisDemoLastSnapshotIt;
					snapInfoHere = thisDemoLastSnapshotIt->second.get();
					// TODO Do actual interpolation instead of just copying last one. Don't copy entities that are in previous but not in next.
					snapIsInterpolated = qtrue;
				}
				else {
					demoReaders[i]->packetsUsed++;
					demoReaders[i]->currentSnapIt = snapInfoHereIterator;
				}
				//std::map<int, entityState_t> hereEntities = demoReaders[i].GetCurrentEntities();
				//tmpPS = demoReaders[i].GetCurrentPlayerState();
				tmpPS = snapInfoHere->playerState;

				// All that is visible in the current snap should be visible in final demo
				for (int amb = 0; amb < MAX_MAP_AREA_BYTES; amb++) {
					areamasHere[amb] = areamasHere[amb] & snapInfoHere->areamask[amb];
				}

				if (reframeClientNum != -1) {

					mainPlayerPS.clientNum = reframeClientNum;

					// If we are reframing, this gets its own separate handling.
					// In reframe handling we don't have the whole "mainPlayerPSIsInterpolated" concept.
					// We just track movement/entity data separately from playerstate data, sort of. Or rather, we remember the time of playerstate info (health,armor etc) as well as the time of the entitystate info (position etc)
					// And based on that info we try to get the best of both worlds always.
					if(mainPlayerRealPSPartsServerTime != time){ // Skip any further processsing if we already have a perfect real PS with the right timing.

						if (tmpPS.clientNum == reframeClientNum) { // Get a true PS, nice!

							bool isBetterOrEqualPlayerStateData = IsBetterOrEqualTime(time, mainPlayerRealPSPartsServerTime, snapInfoHere->serverTime);
							bool isBetterOrEqualEntityStateData = IsBetterOrEqualTime(time, mainPlayerServerTime, snapInfoHere->serverTime);
							if (isBetterOrEqualPlayerStateData && isBetterOrEqualEntityStateData) { // Perfect full Playerstate that is better in every way compared to what we already have. Perfection.
								mainPlayerPS = tmpPS;
								mainPlayerServerTime = snapInfoHere->serverTime;
								mainPlayerRealPSPartsServerTime = snapInfoHere->serverTime;
							}
							else {
								if (IsBetterTime(time, mainPlayerRealPSPartsServerTime, snapInfoHere->serverTime)) {
									// Upgrade playerstate data only if it's actually better
									EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS, demoType); // Just update health/armor and stuff like that. Not a godlike solution but eh.
									mainPlayerRealPSPartsServerTime = snapInfoHere->serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
								}
								//if (IsBetterTime(time, mainPlayerServerTime, snapInfoHere->serverTime)) {
									// Upgrade entitystate data only if it's actually better
									// There is no way for this to be true without the playerstate data ALSO being better, is there?
									// Because this is a real full playerstate. If it has more up to date info on the movement/entitystate data,
									// it MUST also have more up to date info on the playerstate data because playerstate data includes entitystate data
									// and there is no circumstance where we would (or should!) end up with more up to date/better playerstate data than entitystate data.
									// Because why the heck would we update health/armor from a playerstate without updating the entitystate related data (movement etc) too?
									// Hence, this condition doesn't have to be here. It would never trigger because the "isBetterOrEqualPlayerStateData && isBetterOrEqualEntityStateData" condition above
									// would already catch it
									//
									// TODO: This explanation kinda sucks and is hard to read.
								//}
							}
							
							/*if (snapInfoHere->serverTime > mainPlayerServerTime) {
								// We have a real PS and its newer than what we already had. Just overwrite.
								mainPlayerPS = tmpPS;
								mainPlayerServerTime = snapInfoHere->serverTime;
								mainPlayerRealPSPartsServerTime = snapInfoHere->serverTime;
							}
							else if (snapInfoHere->serverTime < mainPlayerServerTime && snapInfoHere->serverTime > mainPlayerRealPSPartsServerTime && !mainPlayerPSIsRealPS) {
								// We have a real PS but it's  older than the fake PS (from entities) we already have.
								// Hence, we combine...
								//mainPlayerPS = tmpPS;
								//CG_EntityStateToPlayerState(&mainPlayerPSSourceES, &mainPlayerPS, demoType, qtrue, NULL, qtrue); // enhanceOnly. Aka, we only update stuff we can reasonably reliably get from entityState
								EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS); // Just update health/armor and stuff like that. Not a godlike solution but eh.
								mainPlayerRealPSPartsServerTime = snapInfoHere->serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
								// TODO How will this work together with the already existing PS assembly inside GetLastOrNextPlayer() ?
							}*/
						}
						else {
							// Get creative...
							SnapshotInfoMapIterator usedSnapIt = demoReaders[i]->nullIt;
							SnapshotInfoMapIterator usedPlayerStateSnapIt = demoReaders[i]->nullIt;
							tmpPS2 = demoReaders[i]->reader.GetLastOrNextPlayer(reframeClientNum, time, &usedSnapIt,&usedPlayerStateSnapIt, qtrue, &snapInfoHereIterator, 5000);

							// All that is visible in the main player source snap should be visible in final demo
							if (usedSnapIt != demoReaders[i]->nullIt) {
								for (int amb = 0; amb < MAX_MAP_AREA_BYTES; amb++) {
									areamasHere[amb] = areamasHere[amb] & usedSnapIt->second->areamask[amb];
								}
							}

							bool isBetterOrEqualEntityStateData = usedSnapIt != demoReaders[i]->nullIt && IsBetterOrEqualTime(time, mainPlayerServerTime, usedSnapIt->second->serverTime);
							bool isBetterOrEqualPlayerStateData = usedPlayerStateSnapIt != demoReaders[i]->nullIt && IsBetterOrEqualTime(time, mainPlayerRealPSPartsServerTime, usedPlayerStateSnapIt->second->serverTime);

							bool isBetterEntityStateData = usedSnapIt != demoReaders[i]->nullIt && IsBetterTime(time, mainPlayerServerTime, usedSnapIt->second->serverTime);
							bool isBetterPlayerStateData = usedPlayerStateSnapIt != demoReaders[i]->nullIt && IsBetterTime(time, mainPlayerRealPSPartsServerTime, usedPlayerStateSnapIt->second->serverTime);

							//mainPlayerPSSourceES;
							if (isBetterPlayerStateData && isBetterEntityStateData) { // Uh. Can this actually happen? :thonk: Idk, let's just pretend it can for now.
								mainPlayerPS = tmpPS2;
								mainPlayerServerTime = usedSnapIt->second->serverTime;
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second->serverTime;
							}
							else if (isBetterOrEqualEntityStateData && isBetterOrEqualPlayerStateData && usedSnapIt== usedPlayerStateSnapIt) {
								// Equally good is only acceptable if it's a nice united playerstate and not some frankensstein monstser// Equally good is only acceptable if it's a nice united playerstate and not some frankensstein monstser
								// TODO Tbh this condition/option might be a bit illogical/unnecessary given all the other code. Haven't thought it through fully.
								mainPlayerPS = tmpPS2;
								mainPlayerServerTime = usedSnapIt->second->serverTime;
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second->serverTime;
							}
							else if (isBetterPlayerStateData) {
								// Upgrade playerstate data only if it's actually better
								EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS2, demoType); // Just update health/armor and stuff like that. Not a godlike solution but eh.
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second->serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
							}
							else if (isBetterEntityStateData) {
								// Upgrade entitystate related data if its actually better.
								// This CAN actually happen because we might not have gotten back any playerstate related data at all so we would just end up treating this as normal entitystate data

								// Sanity check.
								assert(usedSnapIt->second->entities.find(reframeClientNum) != usedSnapIt->second->entities.end()); 

								// TODO: Make this actually work somehow?
								// Problem: Depending on the order in which we get playerstate/entitystate data, we either do the "enhanceonly" thing or we do the EnhancePlayerStateWithBaseState thing, and both lead to inconsistent results, so in cases where we quickly alternate between both methods (like with strobe), we end up with weird inconsistent behavior. Sad but true.
								//CG_EntityStateToPlayerState(&usedSnapIt->second.entities[reframeClientNum], &mainPlayerPS, demoType, qtrue, NULL, mainPlayerRealPSPartsServerTime != -1 ?  qtrue : qfalse); // Accessing the entities map like that.. not very efficient? We do the same inside GetLastOrNextPlayer, now we have done it twice. Meh.
								
								// Sad way of doing it but best we got for now for consistency :(
								tmpPS3 = mainPlayerPS;
								CG_EntityStateToPlayerState(&usedSnapIt->second->entities[reframeClientNum], &mainPlayerPS, demoType, qtrue, NULL, qfalse);
								EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS3, demoType); // Just update health/armor and stuff like that. Not a godlike solution but eh.

								mainPlayerServerTime = usedSnapIt->second->serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
							}
						}
					}

					if (reframeClientNum != tmpPS.clientNum) {
						Com_Memset(&tmpES, 0, sizeof(tmpES));
						BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse, demoType, qtrue);
						if (playerEntities.find(tmpPS.clientNum) == playerEntities.end() || (entityIsInterpolated[tmpPS.clientNum] && (!snapIsInterpolated || entityServerTime[tmpPS.clientNum] < snapInfoHere->serverTime))) { // Prioritize entities that are not interpolated
							playerEntities[tmpPS.clientNum] = tmpES;
							entityIsInterpolated[tmpPS.clientNum] = snapIsInterpolated;
							entityServerTime[tmpPS.clientNum] = snapInfoHere->serverTime;
						}
					}
				}
				else {
					// self explanatory.
					if (nonSkippedDemoIndex++ == 0 || (tmpPS.clientNum == mainPlayerPS.clientNum && mainPlayerPSIsInterpolated && (!snapIsInterpolated || snapInfoHere->serverTime > mainPlayerServerTime))) { // TODO MAke this more sophisticated. Allow moving over some non-snapped values from entitystates perhaps to smooth out mainPlayerPS
						// For reference, here's what gets snapped (rounded) in entities:
						// SnapVector( s->pos.trBase );
						// SnapVector( s->apos.trBase );
						mainPlayerPS = tmpPS;
						mainPlayerPSIsInterpolated = snapIsInterpolated;
						mainPlayerServerTime = snapInfoHere->serverTime;
					}
					else if (tmpPS.clientNum != mainPlayerPS.clientNum) {
						Com_Memset(&tmpES, 0, sizeof(tmpES));
						BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse, demoType, qtrue);
						if (playerEntities.find(tmpPS.clientNum) == playerEntities.end() || (entityIsInterpolated[tmpPS.clientNum] && (!snapIsInterpolated || entityServerTime[tmpPS.clientNum] < snapInfoHere->serverTime))) { // Prioritize entities that are not interpolated
							playerEntities[tmpPS.clientNum] = tmpES;
							entityIsInterpolated[tmpPS.clientNum] = snapIsInterpolated;
							entityServerTime[tmpPS.clientNum] = snapInfoHere->serverTime;
						}
					}
				}

				// Copy all entities
				// Entities from other demos will automatically overwrite entities from this demo.
				for (auto it = snapInfoHere->entities.begin(); it != snapInfoHere->entities.end(); it++) {

					// This is for events. We wanna log all the extra entities we have available to write player events to, whether or not we actually keep that entity (in case of entity Num == ps ClientNum)
					// Reason: For each new event we must assign it to a fixed entity, otherwise we'd have glitches and events repeating/interfering
					if (it->first >= MAX_CLIENTS && it->second.eFlags & EF_PLAYER_EVENT) {
						extraPlayerEventEntities[it->second.otherEntityNum].insert(it->first);
					}

					int compareNum = reframeClientNum == -1 ? mainPlayerPS.clientNum : reframeClientNum;
					if (it->first != compareNum) { // Compare against maain player ps or reframe client num. (reframing is optional for DemoMerger)
						if (!(it->second.eFlags & EF_PLAYER_EVENT && it->second.otherEntityNum == compareNum)) { // Don't copy over player event entities if we are spectating that player (all the events belong into the playerstate then)
							if (playerEntities.find(it->first) == playerEntities.end() || (entityIsInterpolated[it->first] && (!snapIsInterpolated || entityServerTime[it->first] < snapInfoHere->serverTime))) { // Prioritize entities that are not interpolated
								playerEntities[it->first] = it->second;
								entityIsInterpolated[it->first] = snapIsInterpolated;
								entityServerTime[it->first] = snapInfoHere->serverTime;
							}
						}
					}
					else if(reframeClientNum == -1 && // Reframe handling is completely separate already, see further above.
						mainPlayerPSIsInterpolated && (!snapIsInterpolated || snapInfoHere->serverTime > mainPlayerServerTime)){
						// Move some entity stuff over to playerState to improve its precision? Kind of experimental.
						// TODO: Make this actually work somehow?
						// Problem: Depending on the order in which we get playerstate/entitystate data, we either do the "enhanceonly" thing or we do the EnhancePlayerStateWithBaseState thing, and both lead to inconsistent results, so in cases where we quickly alternate between both methods (like with strobe), we end up with weird inconsistent behavior. Sad but true.
						//CG_EntityStateToPlayerState(&it->second, &mainPlayerPS, demoType, qtrue, NULL, qtrue);
						tmpPS3 = mainPlayerPS;
						
						// Sad way of doing it but best we got for now for consistency :(
						CG_EntityStateToPlayerState(&it->second, &mainPlayerPS, demoType, qtrue, NULL, qfalse);
						EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS3, demoType);
						mainPlayerServerTime = snapInfoHere->serverTime;

						// This does not reset the isInterpolated status, since it's kind of a "meh" value from an entity. It's just a patchwork kinda.
					}
				}


				// Get new commands
				std::vector<std::string> newCommandsHere = demoReaders[i]->reader.GetNewCommandsAtServerTime(time);
				for (int c = 0; c < newCommandsHere.size(); c++) {
					
					// New handling to avoid dupes
					qboolean isADupe = qfalse;
					for (int sc = 0; sc < demoReaders[i]->commandDupesToFilter.size(); sc++) {
						if (demoReaders[i]->commandDupesToFilter[sc] == newCommandsHere[c]) {
							isADupe = qtrue;
							demoReaders[i]->commandDupesToFilter.erase(demoReaders[i]->commandDupesToFilter.begin()+sc);
							break;
						}
					}

					if (!isADupe) {
						commandsToAdd.push_back(newCommandsHere[c]);
						for (int sd = 0; sd < demoReaders.size(); sd++) {
							if (sd != i) {
								demoReaders[sd]->commandDupesToFilter.push_back(newCommandsHere[c]);
							}
						}
					}


					/*if (i == 0) { // This is the main reference demo. Just add this stuff.
						commandsToAdd.push_back(newCommandsHere[c]);
					}
					else {
						// Check if it's a dupe. Not a perfect solution as it won't properly deal with duplicated chat messages that only exist in a latter demo,
						// but it's best I can come up with.
						qboolean isDupe = qfalse;
						for (int cd = 0; cd < commandsToAdd.size(); cd++) {
							if (commandsToAdd[cd] == newCommandsHere[c]) {
								isDupe = qtrue;
								break;
							}
						}
						if (!isDupe) {
							commandsToAdd.push_back(newCommandsHere[c]);
						}
					}*/
				}

				if (!snapIsInterpolated) { // We already parsed this. If we didn't find a dupe of an older command now, we won't find it later either. (and it might be stuff that was only sent to some of the clients, like team chat)

					demoReaders[i]->commandDupesToFilter.clear();
				}

				// Ok now... redo all events for players do avoid inconsistencies. This is unelegant af but I see no realistic way of doing it properly without making it
				// incredibly complicated, hard to read and possibly even more unelegant in places
				std::vector<Event> newEvents = demoReaders[i]->reader.GetNewEventsAtServerTime(time,EK_ALL);
				for (int e = 0; e < newEvents.size(); e++) {

					//if (!newEvents[e].eventNumber) {
					//	std::cout << "Event number 0 from Demo Reader, weird!\n";
					//}

					// We check whether this event is already registered.
					// Since we are combining multiple demos, each event will (or can) come from multiple places.

					// Is this a player event? We only process player events due to the playerstate/entitystate discrepance. Normal entities are left alone.
					qboolean isMainEntityPlayerEvent = (qboolean)(newEvents[e].theEvent.number >= 0 && newEvents[e].theEvent.number < MAX_CLIENTS && newEvents[e].kind == EK_ENTITY);
					qboolean isExtraEntityPlayerEvent = (qboolean)(newEvents[e].theEvent.number >= MAX_CLIENTS && newEvents[e].kind == EK_ENTITY && newEvents[e].theEvent.eFlags & EF_PLAYER_EVENT);
					if (isMainEntityPlayerEvent || isExtraEntityPlayerEvent || newEvents[e].kind == EK_PS_ARRAY || newEvents[e].kind == EK_PS_EXTERNAL
						) {

						int eventClientNum = isExtraEntityPlayerEvent ? newEvents[e].theEvent.otherEntityNum : newEvents[e].theEvent.number;

						playerEventData_t* thisPlayerEventData = &playerEventData[eventClientNum];
						qboolean eventIsTracked = qfalse;
						int freeSpots = 0;
						int freeSpot = -1;
						int64_t oldestTrackedEventTime = INT64_MAX;

						// Check the events we are tracking for this player
						// We are checking:
						// 1. Whether the event is being tracked
						// 2. We try to find a free spot for a new event. This will either be a free spot or the oldest tracked event.
						for (int se = 0; se < PLAYER_MAX_EVENT_COUNT; se++) {
							if (thisPlayerEventData->events[se].eventValue == newEvents[e].theEvent.event) {
								eventIsTracked = qtrue;
								break;
							}
							if (thisPlayerEventData->events[se].eventValue == 0) {
								freeSpots++;
								freeSpot = se;
							}
							else if (thisPlayerEventData->events[se].timeFirstSeen < oldestTrackedEventTime) {
								oldestTrackedEventTime = thisPlayerEventData->events[se].timeFirstSeen;
								if (freeSpots == 0) {
									freeSpot = se;
								}
							}
						}

						// If the event is already tracked, we don't have to do anything. If it isn't, we have to add it.
						if (!eventIsTracked) {
							memset(&thisPlayerEventData->events[freeSpot],0,sizeof(thisPlayerEventData->events[freeSpot]));
							thisPlayerEventData->events[freeSpot].eventValue = newEvents[e].theEvent.event;
							thisPlayerEventData->events[freeSpot].eventParm = newEvents[e].theEvent.eventParm;
							thisPlayerEventData->events[freeSpot].timeFirstSeen = time;
							thisPlayerEventData->events[freeSpot].wasNotExternalEvent = (qboolean)(isExtraEntityPlayerEvent || newEvents[e].kind == EK_PS_ARRAY); // External event is always written first to entitystate, so if its an extra entity, it was one of the sequence ones for sure.
						}
					}
				}

				if (opts.persistEntitiesMaxDelay) {
					demoReaders[i]->reader.GetFutureEntityStates(time, opts.persistEntitiesMaxDelay, true, &futureEntityStates, &snapInfoHereIterator);
				}

			}
			if (!demoReaders[i]->reader.EndReachedAtServerTime(time)) {
				allSourceDemosFinished = qfalse;
			}
		}


		// Actually rewrite the player events and also assign them if not yet done.
		// Step 1: Clear playerstate (doesn't matter who it is, we always end up clearing it anyway
		mainPlayerPS.externalEvent = 0;
		mainPlayerPS.externalEventParm = 0;
		//mainPlayerPS.externalEventTime = 0; // Not sure what this one even does tbh. Game doesn't seem to use it anywhere and it's not transmitted either
		memset(&mainPlayerPS.events, 0, sizeof(mainPlayerPS.events));
		memset(&mainPlayerPS.eventParms, 0, sizeof(mainPlayerPS.eventParms));

		// Step 2: Clear player entity events and rewrite them.
		for (int i = 0; i < getMAX_CLIENTS(demoType); i++) {

			// First, clear event data in actual entities/playerstate
			if (playerEntities.find(i) != playerEntities.end()) { // Main player entity
				playerEntities[i].event = 0;
				playerEntities[i].eventParm = 0;
			}
			for (auto it = extraPlayerEventEntities[i].begin(); it != extraPlayerEventEntities[i].end(); it++) {
				if (playerEntities.find(*it) != playerEntities.end()) {
					if (*it >= getMAX_CLIENTS(demoType)) {
						playerEntities[*it].eType = getET_EVENTS(demoType);
					}
					playerEntities[*it].event = 0;
					playerEntities[*it].eventParm = 0;
				}
			}

			playerEventData_t* thisPlayerEventData = &playerEventData[i];
			for (int e = 0; e < PLAYER_MAX_EVENT_COUNT; e++) {
				if (thisPlayerEventData->events[e].eventValue) {


					// First, assign the event to playerstate slots and entity slots if not already done.
					if (!thisPlayerEventData->events[e].isAssignedPlayerState) {
						// Not assigned to playerstate slot yet

						// Playerstate assignment
						// Generally speaking we have 3 places to assign something in playerstate: externalEvent and the array of 2 events.
						// Judging by the game code, it doesn't really seem to matter which is where, they get executed just the same.
						// So let's just treat each slot as equal for now. (TODO?)

						// Check where we can put stuff/what is already occupied
						qboolean externalEventIsFree = qtrue;
						qboolean externalEventIsOldest = qfalse;
						//int arraySpotsOccupied = 0;

						for (int se = 0; se < PLAYER_MAX_EVENT_COUNT; se++) {
							
							if (thisPlayerEventData->events[se].eventValue != 0 && thisPlayerEventData->events[se].isAssignedPlayerState) {
								if (thisPlayerEventData->events[se].assignedAsExternal) {
									externalEventIsFree = qfalse;
								}
								else {
									//arraySpotsOccupied++;
								}
							}
						}

						if (externalEventIsFree) {
							// Let's put it in external event
							thisPlayerEventData->events[e].assignedAsExternal = qtrue;
						}
						else  {
							// Let's put it in array
							thisPlayerEventData->events[e].assignedAsExternal = qfalse;
							thisPlayerEventData->events[e].sequenceNum = thisPlayerEventData->eventSequence++;
						}
						thisPlayerEventData->events[e].isAssignedPlayerState = qtrue;
					}

					if (!thisPlayerEventData->events[e].isAssignedEntity) {
						// Not assigned to entity player event slot yet
						// We have to handle this separately from playerstate because while playerstate assignments can always be done blind,
						// for entity slot assignments we actually need to know the entity numbers and they may not be available on every frame.
						
						std::set<int> thisPlayerAvailableSlots;
						thisPlayerAvailableSlots.insert(i);
						thisPlayerAvailableSlots.insert(extraPlayerEventEntities[i].begin(), extraPlayerEventEntities[i].end());


						int64_t oldestTrackedEventTime = INT64_MAX;
						int oldestSlot = -1;
						int oldestSlotEventIndex = -1;

						// Check which slots are available
						for (int se = 0; se < PLAYER_MAX_EVENT_COUNT; se++) {

							if (thisPlayerEventData->events[se].eventValue != 0 && thisPlayerEventData->events[se].isAssignedEntity) {

								if (thisPlayerAvailableSlots.find(thisPlayerEventData->events[se].assignedEntityNum) != thisPlayerAvailableSlots.end()) {

									thisPlayerAvailableSlots.erase(thisPlayerEventData->events[se].assignedEntityNum);
								}

								if (thisPlayerEventData->events[se].timeFirstSeen < oldestTrackedEventTime) {
									oldestTrackedEventTime = thisPlayerEventData->events[se].timeFirstSeen;
									oldestSlotEventIndex = se;
									oldestSlot = thisPlayerEventData->events[se].assignedEntityNum;
								}
							}
						}

						// Yes, this means that if the player isn't visible at all at the moment or if we are in the playerstate perspective, a lot of events get lost. Isn't that intended tho? (TODO?)
						int slotToUse = thisPlayerAvailableSlots.size() == 0 ? oldestSlot : *thisPlayerAvailableSlots.begin();

						if (slotToUse != -1) { 
							thisPlayerEventData->events[e].isAssignedEntity = qtrue;
							thisPlayerEventData->events[e].assignedEntityNum = slotToUse;
						}
					}

					// NOW
					// Actually write the event info.
					//
					//
					int compareNum = reframeClientNum == -1 ? mainPlayerPS.clientNum : reframeClientNum;
					if (compareNum == i) {
						// We write the stuff to the playerstate
						if (thisPlayerEventData->events[e].assignedAsExternal) {
							mainPlayerPS.externalEvent = thisPlayerEventData->events[e].eventValue;
							mainPlayerPS.externalEventParm = thisPlayerEventData->events[e].eventParm;
						}
						else {
							mainPlayerPS.events[thisPlayerEventData->events[e].sequenceNum & (MAX_PS_EVENTS - 1)] = thisPlayerEventData->events[e].eventValue &~ EV_EVENT_BITS;
							mainPlayerPS.eventParms[thisPlayerEventData->events[e].sequenceNum & (MAX_PS_EVENTS - 1)] = thisPlayerEventData->events[e].eventParm;
						}
					}
					else if(thisPlayerEventData->events[e].isAssignedEntity) {
						// We write the stuff to entities (if they are available)
						if (playerEntities.find(thisPlayerEventData->events[e].assignedEntityNum) != playerEntities.end()) {
							entityState_t* thisPlayerEntityRightHere = &playerEntities[thisPlayerEventData->events[e].assignedEntityNum];
							if (thisPlayerEventData->events[e].assignedEntityNum >= getMAX_CLIENTS(demoType)) { // Is ET_PLAYER

								thisPlayerEntityRightHere->eFlags |= EF_PLAYER_EVENT;
								thisPlayerEntityRightHere->otherEntityNum = i;
								thisPlayerEntityRightHere->eType = thisPlayerEventData->events[e].eventValue + getET_EVENTS(demoType);
							}
							thisPlayerEntityRightHere->event = thisPlayerEventData->events[e].eventValue;
							thisPlayerEntityRightHere->eventParm = thisPlayerEventData->events[e].eventParm;
						}
					}

				}
			}
			extraPlayerEventEntities[i].clear();
		}

		if (opts.persistEntitiesMaxDelay) {

			// Interpolate playerstate if desired
			if (opts.interpolatePlayerStateMaxDelay && mainPlayerServerTime < time) {
				auto futurePS = futureEntityStates.find(mainPlayerPS.clientNum);
				if (futurePS != futureEntityStates.end()
					&& (futurePS->second.demoToolsData.serverTime- mainPlayerServerTime) < opts.interpolatePlayerStateMaxDelay
					&& mainPlayerServerTime < futurePS->second.demoToolsData.serverTime
					&& time < futurePS->second.demoToolsData.serverTime
					) {
					entityState_t* futurePsState = &futurePS->second; // You think it's lame we take it from an entityState but we only need pos, angles and velocity and those entitystates are from getfutureentitystates and not snapped. Just makes things easier.
					int oldTime = mainPlayerServerTime;
					int futureTime = futurePS->second.demoToolsData.serverTime;
					int nowTime = time;
					float ratio = ((float)nowTime - (float)oldTime) / ((float)futureTime - (float)oldTime);
					if (futurePsState->pos.trType == TR_LINEAR_STOP) { // Hmm I'm not too confident about this one... feel this will all fall apart on a server without g_smoothclients with a fast switching spectator
						oldTime = mainPlayerPS.commandTime;
						futureTime = futurePS->second.pos.trTime;
						nowTime = oldTime + ratio * (float)(futureTime - oldTime);
						mainPlayerPS.commandTime = nowTime;
					}
					else {
						mainPlayerPS.commandTime = nowTime-oldTime + mainPlayerPS.commandTime; // Ew
					}
					if (oldTime < futureTime) {
						for (int i = 0; i < 3; i++) {
							mainPlayerPS.origin[i] = mainPlayerPS.origin[i] + ratio * (futurePsState->pos.trBase[i] - mainPlayerPS.origin[i]);
							mainPlayerPS.viewangles[i] = LerpAngle(
								mainPlayerPS.viewangles[i], futurePsState->apos.trBase[i], ratio);
							mainPlayerPS.velocity[i] = mainPlayerPS.velocity[i] +
								ratio * (futurePsState->pos.trDelta[i] - mainPlayerPS.velocity[i]);
						}
					}
				}
			}

			// Repeat/interpolate past entities
			for (auto it = pastEntityStates.begin(); it != pastEntityStates.end(); it++) {
				auto futureIt = futureEntityStates.find(it->first);
				if (it->second.demoToolsData.serverTime < time
					&& it->first != mainPlayerPS.clientNum
					&& playerEntities.find(it->first) == playerEntities.end() 
					&& futureIt != futureEntityStates.end()
					&& (futureIt->second.demoToolsData.serverTime- it->second.demoToolsData.serverTime) <= opts.persistEntitiesMaxDelay
					&& futureIt->second.eType == it->second.eType
					//&& (futureIt->second.eType != specializeGameValue<GMAP_ENTITYTYPE,UNSAFE>(ET_PLAYER_GENERAL,demoType) || )
					&& !((futureIt->second.eFlags ^ it->second.eFlags) & getEF_TELEPORTBIT(demoType)) // Hasn't teleported in between
					) {
					int deltaTime = futureIt->second.demoToolsData.serverTime - it->second.demoToolsData.serverTime;

					entityState_t entState = it->second;

					if (entState.demoToolsData.isInterpolated) {
						// Restore non-interpolated trajectories so we are starting from real data.
						entState.pos = entState.demoToolsData.uninterpolatedPos;
						entState.apos = entState.demoToolsData.uninterpolatedAPos;
						entState.demoToolsData.isInterpolated = false;
					}
					else {
						// Save real data for restoring later.
						entState.demoToolsData.uninterpolatedPos = entState.pos;
						entState.demoToolsData.uninterpolatedAPos = entState.apos;
					}

					float distanceFromOld = VectorDistance(futureIt->second.pos.trBase,it->second.pos.trBase);
					float travelSpeedUPS = 1000.0f*distanceFromOld / (float)deltaTime;
					if (travelSpeedUPS < 3000) { // Travel speed 3000 is very implausible. Just a sanity check.

						if (opts.interpolateEntitiesMaxDelay && deltaTime <= opts.interpolateEntitiesMaxDelay && futureIt->second.demoToolsData.serverTime > it->second.demoToolsData.serverTime && futureIt->second.demoToolsData.serverTime > time) {
							
							if (it->second.eType == specializeGameValue<GMAP_ENTITYTYPE, UNSAFE>(ET_PLAYER_GENERAL, demoType)) {
								int oldTime = it->second.demoToolsData.serverTime;
								int futureTime = futureIt->second.demoToolsData.serverTime;
								int nowTime = time;
								float ratio = ((float)nowTime - (float)oldTime) / ((float)futureTime - (float)oldTime);
								if (it->second.pos.trType == TR_LINEAR_STOP) { // Hmm I'm not too confident about this one... feel this will all fall apart on a server without g_smoothclients with a fast switching spectator
									oldTime = it->second.pos.trTime;
									futureTime = futureIt->second.pos.trTime;
									nowTime = oldTime + ratio * (float)(futureTime - oldTime);
									entState.pos.trTime = nowTime;
								}
								if (futureTime > oldTime) {

									entState.demoToolsData.isInterpolated = true;
									for (int i = 0; i < 3; i++) {
										entState.pos.trBase[i] = it->second.pos.trBase[i] + ratio * (futureIt->second.pos.trBase[i] - it->second.pos.trBase[i]);
										entState.apos.trBase[i] = LerpAngle(
											it->second.apos.trBase[i], futureIt->second.apos.trBase[i], ratio);
										entState.pos.trDelta[i] = it->second.pos.trDelta[i] +
											ratio * (futureIt->second.pos.trDelta[i] - it->second.pos.trDelta[i]);
									}
								}

								playerEntities[it->first] = entState;
								entityServerTime[it->first] = it->second.demoToolsData.serverTime;
							}
							else {
								if (specializeGameValue<GMAP_ENTITYTYPE, UNSAFE>(ET_ITEM_GENERAL, demoType) == it->second.eType) {
									entState.demoToolsData.flags |= DTFLAG_JK2MP_ITEMBOUNCE; // TODO is this same for other games?
								}
								retimeEntity(&entState, time, time, demoType);
								playerEntities[it->first] = entState;
								entityServerTime[it->first] = it->second.demoToolsData.serverTime;
							}
						}
						else {
							playerEntities[it->first] = entState;
							entityServerTime[it->first] = entState.demoToolsData.serverTime;
						}
					}
				}
			}
		}

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0]->reader.GetCurrentSnap();

		if (opts.visAll) {
			Com_Memset(demo.cut.Cl.snap.areamask, 0, sizeof(demo.cut.Cl.snap.areamask));
		}
		else {
			Com_Memcpy(demo.cut.Cl.snap.areamask, areamasHere, sizeof(demo.cut.Cl.snap.areamask));
		}
		//Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 

		bool doWriteSnap = !(opts.skipMainPlayerDeadFrames /* && reframeClientNum != -1*/ && mainPlayerServerTime != time);

		if (doWriteSnap) {

			SnapshotEntitiesOrderedPointers ordered = SnapShotEntitiesToOrderedPointers(playerEntities);
			if (isFirstSnapshot) {
				demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &ordered, NULL, NULL, createCompressedOutput);
				isFirstSnapshot = qfalse;
			}
			else {
				SnapshotEntitiesOrderedPointers orderedOld = SnapShotEntitiesToOrderedPointers(playerEntitiesOld);
				demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &ordered, &orderedOld, &mainPlayerPSOld, createCompressedOutput);
			}

			framesWritten++;

			demo.cut.Clc.reliableSequence++;
			demo.cut.Clc.serverMessageSequence++;

			mainPlayerPSOld = mainPlayerPS;
			//playerEntitiesOld.clear();
			//for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
			//	playerEntitiesOld[it->first] = it->second;
			//}
			playerEntitiesOld = playerEntities; // Faster?
		}
		else {
			framesSkippedDeadFrames++;
		}

		int oldTime = time;
		time = INT_MAX;
		for (int i = 0; i < demoReaders.size(); i++) {
			if (demoReaders[i]->currentSnapIt != demoReaders[i]->nullIt) {

				demoReaders[i]->nextSnapIt = demoReaders[i]->reader.GetFirstSnapshotAfterSnapshotIterator(demoReaders[i]->currentSnapIt,oldTime);
			}
			else if(demoReaders[i]->nextSnapIt == demoReaders[i]->nullIt || demoReaders[i]->nextSnapIt->second->serverTime <= oldTime) {
				int thisDemoNextServerTime = demoReaders[i]->reader.GetFirstServerTimeAfterServerTime(oldTime);
				demoReaders[i]->nextSnapIt = demoReaders[i]->reader.GetSnapshotInfoAtServerTimeIterator(thisDemoNextServerTime);
			}
			if (demoReaders[i]->nextSnapIt != demoReaders[i]->nullIt) {

				if (demoReaders[i]->nextSnapIt != demoReaders[i]->currentSnapIt && demoReaders[i]->currentSnapIt != demoReaders[i]->nullIt) {
					demoReaders[i]->snapshotsProcessedSinceClear++;
					if (demoReaders[i]->snapshotSafeToClear != demoReaders[i]->nullIt) {
						demoReaders[i]->snapshotsProcessedSinceSnapshotSafeToClearSet++;
					}
					if (demoReaders[i]->snapshotsProcessedSinceClear == 10000) { // ok mark this one as the "first one to keep" and then wait 5000 more frames.
						demoReaders[i]->snapshotSafeToClear = demoReaders[i]->currentSnapIt;
						demoReaders[i]->snapshotsProcessedSinceSnapshotSafeToClearSet = 0;
					}
					if (demoReaders[i]->snapshotsProcessedSinceSnapshotSafeToClearSet > 5000) { // this way we have the past 5000 frames too.
						demoReaders[i]->reader.ClearSnapshotsBeforeIterator(demoReaders[i]->snapshotSafeToClear); // memory optimization. get rid of old snapshots so we can reframe larger files
						demoReaders[i]->snapshotSafeToClear = demoReaders[i]->nullIt;
						demoReaders[i]->snapshotsProcessedSinceClear = 0;
						demoReaders[i]->snapshotsProcessedSinceSnapshotSafeToClearSet = 0;
					}
				}
				int nextTimeThisDemo = demoReaders[i]->nextSnapIt->second->serverTime;
				time = std::min(time, nextTimeThisDemo); // Find nearest serverTime of all the demos.
			}
		}

		/*if (doWriteSnap) {

			demo.cut.Clc.reliableSequence++;
			demo.cut.Clc.serverMessageSequence++;

			mainPlayerPSOld = mainPlayerPS;
			//playerEntitiesOld.clear();
			//for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
			//	playerEntitiesOld[it->first] = it->second;
			//}
			playerEntitiesOld = playerEntities; // Faster?
		}*/


		// Remember any past versions of entities so we can persist them
		if (opts.persistEntitiesMaxDelay) {
			for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
				if (pastEntityStates.find(it->first) == pastEntityStates.end() || pastEntityStates[it->first].demoToolsData.serverTime < entityServerTime[it->first]){
					entityState_t entState = it->second;
					entState.demoToolsData.serverTime = entityServerTime[it->first];
					pastEntityStates[it->first] = entState;
				}
			}
		}

		if (allSourceDemosFinished || time == INT_MAX) break;
	}

cutcomplete:
	if (newHandle) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}
cuterror:
	FS_FCloseFile(newHandle);

	std::cout << "Total frames written: " << framesWritten << "\n";
	if (opts.skipMainPlayerDeadFrames) {
		std::cout << "Total main player dead frames skipped: " << framesSkippedDeadFrames << "\n";
	}
	for (int i = 0; i < demoReaders.size(); i++) {
		std::cout << "Frames from demo " << i << ": " << demoReaders[i]->packetsUsed << " (" << (demoReaders[i]->packetsUsed*100/framesWritten) << "%)\n";
	}

	return ret;
}

/*void CL_DemoCut_f(void) {
	double startTime, endTime;
	char demoName[MAX_OSPATH];
	if (Cmd_Argc() < 3) {
		Com_Printf("Usage: demoCut start end (in seconds)\n");
		return;
	}
	startTime = atof(Cmd_Argv(1));
	endTime = atof(Cmd_Argv(2));
	if (endTime <= startTime) {
		Com_Printf("invalid range: %.3f >= %.3f\n", startTime, endTime);
		return;
	}
	if (endTime - startTime < 0.05) {
		Com_Printf("invalid range: less than 50 milliseconds is not allowed\n");
		return;
	}
	// convert to msec
	startTime *= 1000;
	endTime *= 1000;
	Com_sprintf(demoName, MAX_OSPATH, mme_demoFileName->string);
	if (demoCut(demoName, startTime, endTime)) {
		Com_Printf("Demo %s got successfully cut\n", mme_demoFileName->string);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", mme_demoFileName->string);
	}
}*/


int main(int argcO, char** argvO) {

	popl::OptionParser op("Allowed options");
	auto h = op.add<popl::Switch>("h", "help", "Show help");
	auto r = op.add<popl::Value<std::string>>("r", "reframe", "Optionally, reframe. Value same as would be with DemoReframer: Search string or clientnum");
	auto p = op.add<popl::Implicit<int>>("p", "persist-entities", "Detect gaps in entity visibility and try to fill them. Millisecond value of maximum gap to fill.", -1);
	auto i = op.add<popl::Implicit<int>>("i", "interpolate-entities", "Interpolate entity positions. Automatically implies -p/--persist-entities.  Millisecond value of maximum gap to fill.", -1);
	auto I = op.add<popl::Implicit<int>>("I", "interpolate-playerstate", "Interpolate playerstate positions. Automatically implies -p/--persist-entities.  Millisecond value of maximum gap to fill.", -1);
	auto s = op.add<popl::Switch>("s", "skip-main-player-deadframes", "For reframing: Skip frames that don't contain the main player, to reduce stutter.");
	auto v = op.add<popl::Switch>("v", "visall", "For reframing: Set visibility mask such that everything is visible (if demos have visual artifacts like level disappearing).");
	op.parse(argcO, argvO);
	auto args = op.non_option_args();


	//if (argc <3) {
	if (args.size() <2) {
		std::cout << "need 2 arguments at least: outputname, demoname1, [demoname2, demoname3,...]";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 1;
	}
	else if (h->is_set()) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds\n";
		std::cout << "third optional argument: myctfreturns, ctfreturns, top10defrag, alldefrag, allkills or allmykills\n";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 0;
	}
	initializeGameInfos();
	char* demoName = NULL;
	const char* outputName = NULL;

	ExtraMergeOptions opts;
	
	opts.persistEntitiesMaxDelay = p->is_set() ? (p->value() < 0 ? EVENT_VALID_MSEC : p->value()) : 0; // If no millisecond value provided, use default of 300. Shrug.
	opts.interpolateEntitiesMaxDelay = i->is_set() ? (i->value() < 0 ? EVENT_VALID_MSEC : i->value()) : 0; // If no millisecond value provided, use default of 300. Shrug.
	opts.interpolatePlayerStateMaxDelay = I->is_set() ? (I->value() < 0 ? EVENT_VALID_MSEC : I->value()) : 0; // If no millisecond value provided, use default of 300. Shrug.
	if ((opts.interpolateEntitiesMaxDelay || opts.interpolatePlayerStateMaxDelay) && !opts.persistEntitiesMaxDelay) {
		opts.persistEntitiesMaxDelay = opts.interpolateEntitiesMaxDelay;
	}
	opts.skipMainPlayerDeadFrames = s->is_set();
	opts.visAll = v->is_set();

	//outputName = argv[1];
	outputName = args[0].c_str();
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName,qtrue);
	//strcpy(outputName, filteredOutputName);
	//delete[] filteredOutputName;

	std::vector<std::string> inputFiles;
	//for (int i = 2; i < argc; i++) {
	for (int i = 1; i < args.size(); i++) {
		inputFiles.emplace_back(args[i]);
	}

	std::string searchString = r->is_set() ? r->value() : "";

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();

	if (demoMerge(filteredOutputName,&inputFiles, (r->is_set() && searchString.size())? &searchString : NULL, opts)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully cut in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get cut or cut with errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif