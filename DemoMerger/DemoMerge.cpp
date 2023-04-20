#include "demoCut.h"
#include "DemoReader.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <set>
#include <chrono>
#include <jkaStuff.h>
#include "otherGameStuff.h"

#include "popl.hpp"

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


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
	std::vector<std::string,mi_stl_allocator<std::string>> commandDupesToFilter;
	SnapshotInfoMapIterator currentSnapIt, nextSnapIt, nullIt;
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


qboolean demoMerge( const char* outputName, std::vector<std::string,mi_stl_allocator<std::string>>* inputFiles, std::string* reframeSearchString) {
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

	strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);

	demoCutGetDemoType(outputName,ext,&demoType,&createCompressedOutput);
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

	std::vector<DemoReaderTrackingWrapper,mi_stl_allocator<DemoReaderTrackingWrapper>> demoReaders;
	std::cout << "loading up demos...";
	int startTime = INT_MAX;
	demoReaders.reserve(inputFiles->size()); // This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	for (int i = 0; i < inputFiles->size(); i++) {
		std::cout << i<<"...";
		demoReaders.emplace_back();
		demoReaders.back().reader.LoadDemo((*inputFiles)[i].c_str());
		demoReaders.back().currentSnapIt = demoReaders.back().nextSnapIt = demoReaders.back().nullIt = demoReaders.back().reader.SnapNullIt();
		startTime = std::min(startTime, demoReaders.back().reader.GetFirstSnapServerTime()); // Find earliest serverTime from all source demos and start there.
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
		tmpConfigString = demoReaders[0].reader.GetConfigString(i,&tmpConfigStringMaxLength);
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
		if (demoReaders[i].reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
			//int spectatedClient = demoReaders[i].reader.GetCurrentPlayerState().clientNum;
			//lastSpectatedClientNums[i] = spectatedClient;			
			//if (i >= MAX_CLIENTS) continue; // We don't have names/configstrings for players > 32
			//tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(spectatedClient,&tmpConfigStringMaxLength);
			//if (strlen(tmpConfigString)) {
				//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS+i, tmpConfigString);
			//}
			if (reframeSearchString) {
				int reframeClientNumHere = demoReaders[i].reader.getClientNumForDemo(reframeSearchString);
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
	char infoCopy[MAX_INFO_STRING];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
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
		Com_Printf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	int time = startTime; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	std::map<int, entityState_t> playerEntities;
	std::map<int, entityState_t> playerEntitiesOld;
	std::vector<std::string,mi_stl_allocator<std::string>> commandsToAdd;
	std::vector<Event,mi_stl_allocator<Event>> eventsToAdd;
	playerState_t tmpPS,tmpPS2, mainPlayerPS, mainPlayerPSOld;
	entityState_t tmpES;// , mainPlayerPSSourceES;
	int currentCommand = 1;
	// Start writing snapshots.
	qboolean isFirstSnapshot = qtrue;
	std::stringstream ss;
	int framesWritten = 0;
	byte	areamasHere[MAX_MAP_AREA_BYTES];

	while(1){
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
			if (demoReaders[i].reader.SeekToServerTime(time)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
				
				SnapshotInfoMapIterator snapInfoHereIterator = demoReaders[i].nullIt;
				if (demoReaders[i].nextSnapIt != demoReaders[i].nullIt){
					if (demoReaders[i].nextSnapIt->second.serverTime == time) {
						snapInfoHereIterator = demoReaders[i].nextSnapIt;
					}
				}
				else {
					snapInfoHereIterator = demoReaders[i].reader.GetSnapshotInfoAtServerTimeIterator(time);
				}
				SnapshotInfo* snapInfoHere = snapInfoHereIterator == demoReaders[i].nullIt ? NULL : &snapInfoHereIterator->second;
				qboolean snapIsInterpolated = qfalse;
				if (!snapInfoHere) {
					SnapshotInfoMapIterator thisDemoLastSnapshotIt = demoReaders[i].nullIt;
					SnapshotInfoMapIterator thisDemoNextSnapshotIt = demoReaders[i].nullIt;

					// Find last
					if (demoReaders[i].currentSnapIt != demoReaders[i].nullIt) {
						thisDemoLastSnapshotIt = demoReaders[i].currentSnapIt;
					}
					else {

						int thisDemoLastServerTime = demoReaders[i].reader.GetLastServerTimeBeforeServerTime(time);
						thisDemoLastSnapshotIt = demoReaders[i].reader.GetSnapshotInfoAtServerTimeIterator(thisDemoLastServerTime);
					}

					// Find next
					if (demoReaders[i].nextSnapIt != demoReaders[i].nullIt) {
						thisDemoNextSnapshotIt = demoReaders[i].nextSnapIt;
					}
					else {

						int thisDemoNextServerTime = demoReaders[i].reader.GetFirstServerTimeAfterServerTime(time);
						thisDemoNextSnapshotIt = demoReaders[i].reader.GetSnapshotInfoAtServerTimeIterator(thisDemoNextServerTime);
					}

					
					if (thisDemoLastSnapshotIt == demoReaders[i].nullIt || thisDemoNextSnapshotIt == demoReaders[i].nullIt) continue;

					snapInfoHereIterator = thisDemoLastSnapshotIt;
					snapInfoHere = &thisDemoLastSnapshotIt->second;
					// TODO Do actual interpolation instead of just copying last one. Don't copy entities that are in previous but not in next.
					snapIsInterpolated = qtrue;
				}
				else {
					demoReaders[i].packetsUsed++;
					demoReaders[i].currentSnapIt = snapInfoHereIterator;
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
									EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS); // Just update health/armor and stuff like that. Not a godlike solution but eh.
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
							SnapshotInfoMapIterator usedSnapIt = demoReaders[i].nullIt;
							SnapshotInfoMapIterator usedPlayerStateSnapIt = demoReaders[i].nullIt;
							tmpPS2 = demoReaders[i].reader.GetLastOrNextPlayer(reframeClientNum, time, &usedSnapIt,&usedPlayerStateSnapIt, qtrue, &snapInfoHereIterator);

							// All that is visible in the main player source snap should be visible in final demo
							if (usedSnapIt != demoReaders[i].nullIt) {
								for (int amb = 0; amb < MAX_MAP_AREA_BYTES; amb++) {
									areamasHere[amb] = areamasHere[amb] & usedSnapIt->second.areamask[amb];
								}
							}

							bool isBetterOrEqualEntityStateData = usedSnapIt != demoReaders[i].nullIt && IsBetterOrEqualTime(time, mainPlayerServerTime, usedSnapIt->second.serverTime);
							bool isBetterOrEqualPlayerStateData = usedPlayerStateSnapIt != demoReaders[i].nullIt && IsBetterOrEqualTime(time, mainPlayerRealPSPartsServerTime, usedPlayerStateSnapIt->second.serverTime);

							bool isBetterEntityStateData = usedSnapIt != demoReaders[i].nullIt && IsBetterTime(time, mainPlayerServerTime, usedSnapIt->second.serverTime);
							bool isBetterPlayerStateData = usedPlayerStateSnapIt != demoReaders[i].nullIt && IsBetterTime(time, mainPlayerRealPSPartsServerTime, usedPlayerStateSnapIt->second.serverTime);

							//mainPlayerPSSourceES;
							if (isBetterPlayerStateData && isBetterEntityStateData) { // Uh. Can this actually happen? :thonk: Idk, let's just pretend it can for now.
								mainPlayerPS = tmpPS2;
								mainPlayerServerTime = usedSnapIt->second.serverTime;
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second.serverTime;
							}
							else if (isBetterOrEqualEntityStateData && isBetterOrEqualPlayerStateData && usedSnapIt== usedPlayerStateSnapIt) {
								// Equally good is only acceptable if it's a nice united playerstate and not some frankensstein monstser// Equally good is only acceptable if it's a nice united playerstate and not some frankensstein monstser
								// TODO Tbh this condition/option might be a bit illogical/unnecessary given all the other code. Haven't thought it through fully.
								mainPlayerPS = tmpPS2;
								mainPlayerServerTime = usedSnapIt->second.serverTime;
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second.serverTime;
							}
							else if (isBetterPlayerStateData) {
								// Upgrade playerstate data only if it's actually better
								EnhancePlayerStateWithBaseState(&mainPlayerPS, &tmpPS2); // Just update health/armor and stuff like that. Not a godlike solution but eh.
								mainPlayerRealPSPartsServerTime = usedPlayerStateSnapIt->second.serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
							}
							else if (isBetterEntityStateData) {
								// Upgrade entitystate related data if its actually better.
								// This CAN actually happen because we might not have gotten back any playerstate related data at all so we would just end up treating this as normal entitystate data

								// Sanity check.
								assert(usedSnapIt->second.entities.find(reframeClientNum) != usedSnapIt->second.entities.end()); 

								CG_EntityStateToPlayerState(&usedSnapIt->second.entities[reframeClientNum], &mainPlayerPS, demoType, qtrue, NULL, mainPlayerRealPSPartsServerTime != -1 ?  qtrue : qfalse); // Accessing the entities map like that.. not very efficient? We do the same inside GetLastOrNextPlayer, now we have done it twice. Meh.
								mainPlayerServerTime = usedSnapIt->second.serverTime; // If this happens multiple times we try to always get the most up to date version of the PS parts.
							}
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
						CG_EntityStateToPlayerState(&it->second, &mainPlayerPS, demoType, qtrue, NULL, qtrue);
						mainPlayerServerTime = snapInfoHere->serverTime;
						// This does not reset the isInterpolated status, since it's kind of a "meh" value from an entity. It's just a patchwork kinda.
					}
				}


				// Get new commands
				std::vector<std::string,mi_stl_allocator<std::string>> newCommandsHere = demoReaders[i].reader.GetNewCommandsAtServerTime(time);
				for (int c = 0; c < newCommandsHere.size(); c++) {
					
					// New handling to avoid dupes
					qboolean isADupe = qfalse;
					for (int sc = 0; sc < demoReaders[i].commandDupesToFilter.size(); sc++) {
						if (demoReaders[i].commandDupesToFilter[sc] == newCommandsHere[c]) {
							isADupe = qtrue;
							demoReaders[i].commandDupesToFilter.erase(demoReaders[i].commandDupesToFilter.begin()+sc);
							break;
						}
					}

					if (!isADupe) {
						commandsToAdd.push_back(newCommandsHere[c]);
						for (int sd = 0; sd < demoReaders.size(); sd++) {
							if (sd != i) {
								demoReaders[sd].commandDupesToFilter.push_back(newCommandsHere[c]);
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

					demoReaders[i].commandDupesToFilter.clear();
				}

				// Ok now... redo all events for players do avoid inconsistencies. This is unelegant af but I see no realistic way of doing it properly without making it
				// incredibly complicated, hard to read and possibly even more unelegant in places
				std::vector<Event,mi_stl_allocator<Event>> newEvents = demoReaders[i].reader.GetNewEventsAtServerTime(time,EK_ALL);
				for (int e = 0; e < newEvents.size(); e++) {
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

			}
			if (!demoReaders[i].reader.EndReachedAtServerTime(time)) {
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
		

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0].reader.GetCurrentSnap();

		
		Com_Memcpy(demo.cut.Cl.snap.areamask, areamasHere, sizeof(demo.cut.Cl.snap.areamask));
		//Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 

		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, NULL,NULL,createCompressedOutput);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, &playerEntitiesOld, &mainPlayerPSOld,createCompressedOutput);
		}

		framesWritten++;

		int oldTime = time;
		time = INT_MAX;
		for (int i = 0; i < demoReaders.size(); i++) {
			if (demoReaders[i].currentSnapIt != demoReaders[i].nullIt) {

				demoReaders[i].nextSnapIt = demoReaders[i].reader.GetFirstSnapshotAfterSnapshotIterator(demoReaders[i].currentSnapIt,oldTime);
			}
			else if(demoReaders[i].nextSnapIt == demoReaders[i].nullIt || demoReaders[i].nextSnapIt->second.serverTime <= oldTime) {
				int thisDemoNextServerTime = demoReaders[i].reader.GetFirstServerTimeAfterServerTime(oldTime);
				demoReaders[i].nextSnapIt = demoReaders[i].reader.GetSnapshotInfoAtServerTimeIterator(thisDemoNextServerTime);
			}
			if (demoReaders[i].nextSnapIt != demoReaders[i].nullIt) {

				int nextTimeThisDemo = demoReaders[i].nextSnapIt->second.serverTime;
				time = std::min(time, nextTimeThisDemo); // Find nearest serverTime of all the demos.
			}
		}
		demo.cut.Clc.reliableSequence++;
		demo.cut.Clc.serverMessageSequence++;

		mainPlayerPSOld = mainPlayerPS;
		//playerEntitiesOld.clear();
		//for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
		//	playerEntitiesOld[it->first] = it->second;
		//}
		playerEntitiesOld = playerEntities; // Faster?

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
	for (int i = 0; i < demoReaders.size(); i++) {
		std::cout << "Frames from demo " << i << ": " << demoReaders[i].packetsUsed << " (" << (demoReaders[i].packetsUsed*100/framesWritten) << "%)\n";
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

	//outputName = argv[1];
	outputName = args[0].c_str();
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName,qtrue);
	//strcpy(outputName, filteredOutputName);
	//delete[] filteredOutputName;

	std::vector<std::string,mi_stl_allocator<std::string>> inputFiles;
	//for (int i = 2; i < argc; i++) {
	for (int i = 1; i < args.size(); i++) {
		inputFiles.emplace_back(args[i]);
	}

	std::string searchString = r->is_set() ? r->value() : "";

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();

	if (demoMerge(filteredOutputName,&inputFiles, (r->is_set() && searchString.size())? &searchString : NULL)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully cut in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif