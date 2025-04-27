#include "demoCut.h"
#include "DemoReader.h"
#include "DemoReaderLight.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <chrono>
#include <set>

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;

/*
struct SourcePlayerInfo {
	int clientNum;
	int medianPing;
	qboolean asG2AnimEnt;
};*/



#ifdef RELDEBUG
//#pragma optimize("", off)
#endif

qboolean demoCut( const char* outputName, std::vector<std::string>* inputFiles) {
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

	//strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);

	qboolean createCompressedOutput = qfalse;

	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(outputName, ext, outputNameNoExt, &demoType, &createCompressedOutput);


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


	std::vector<std::unique_ptr<DemoReaderWrapper>> demoReaders; 
	std::vector<DemoReaderLight> pingDemoReaders;
	std::cout << "loading up demos...";
	demoReaders.reserve(inputFiles->size());// This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	pingDemoReaders.reserve(inputFiles->size());// This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	for (int i = 0; i < inputFiles->size(); i++) {
		std::cout << i<<"...";
		demoReaders.emplace_back(std::make_unique<DemoReaderWrapper>());
		demoReaders.back()->reader.LoadDemo((*inputFiles)[i].c_str());
		demoReaders.back()->sourceInfo = new DemoSource(); // yeah i guess we'll leak it. screw it, who cares. its a fake demosource entry thingie to track delay.
		pingDemoReaders.emplace_back();
		pingDemoReaders.back().LoadDemo((*inputFiles)[i].c_str());
	}
	std::cout << "done.";

	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	int maxConfigStrings = getMaxConfigStrings(demoType); // This does NOT guarantee proper integration. We still rely on hardcoded CS_PLAYERS etc. Just consider anything but JK2 NOT working here.
	for (int i = 0; i < maxConfigStrings; i++) {
		if (i >= CS_PLAYERS && i < (CS_PLAYERS + MAX_CLIENTS)) continue; // Player stuff will be copied manually.
		tmpConfigString = demoReaders[0]->reader.GetConfigString(i,&tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString,demoType);
		}
	}

	std::map<int, int> lastSpectatedClientNums; // Need this for later.

	SlotManager slotManager(demoReaders.size()*2); // reserve count of source demos * 2 (keep free ones cuz saberentitynum)

	// Copy over player config strings
	for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i]->reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
			
			// We simply track ping for all players here. "playersToCopy" doesn't mean the same thing it does for DemoCombine, it's just an array we use to track pings.
			int	lowestPingsHere[MAX_CLIENTS_MAX];
			qboolean	playerExistsAsEntity[MAX_CLIENTS_MAX];
			pingDemoReaders[i].ReadToEnd();
			pingDemoReaders[i].GetLowestPingData(lowestPingsHere);
			pingDemoReaders[i].GetPlayersSeen(playerExistsAsEntity);
			pingDemoReaders[i].FreePingData();
			for (int p = 0; p < demoReaders[i]->reader.getMaxClients(); p++) {
				const char* playerCString = demoReaders[i]->reader.GetPlayerConfigString(p, NULL);
				if (*playerCString) {
					std::cout << playerCString << " [median ping:" << lowestPingsHere[p] << "]" << std::endl;
				}
				demoReaders[i]->playersToCopy.push_back({ p,(float)lowestPingsHere[p],qfalse }); // we just use this array here to track pings, nothing else (unlike DemoCombiner)
			}
			
			int64_t cutStartOffset = demoReaders[i]->reader.getCutStartOffset(qtrue);
			demoReaders[i]->sourceInfo->delay = cutStartOffset;
			int spectatedClient = demoReaders[i]->reader.GetCurrentPlayerState().clientNum;
			lastSpectatedClientNums[i] = spectatedClient;			
			if (i >= MAX_CLIENTS) continue; // We don't have names/configstrings for players > 32
			tmpConfigString = demoReaders[i]->reader.GetPlayerConfigString(spectatedClient,&tmpConfigStringMaxLength);
			if (strlen(tmpConfigString)) {
				demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS+i, tmpConfigString, demoType);
			}
		}
	}
	
	pingDemoReaders.clear();

	demoCutConfigstringModifiedManual(&demo.cut.Cl, getCS_LEVEL_START_TIME(demoType), "10000", demoType);

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING_MAX];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy, demoType);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially created using JK2DemoCutter tools.", demoType);

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName), "%s%s", outputNameNoExt, ext);
	while (FS_FileExists(newName)) {
		Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputNameNoExt, dupeIterator, ext);
		dupeIterator++;
	}

	newHandle = FS_FOpenFileWrite(newName, createCompressedOutput ? FILECOMPRESSION_LZMA : FILECOMPRESSION_NONE, qfalse);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	double sourceTime = 0.0f;
	double time = 10000.0f; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	float fps = 60.0f;
	std::map<int, entityState_t> playerEntities;
	std::map<int, entityState_t> playerEntitiesOld;
	std::vector<std::string> commandsToAdd;
	std::vector<Event> eventsToAdd;
	playerState_t tmpPS, mainPlayerPS, mainPlayerPSOld;
	entityState_t tmpES;
	int currentCommand = 1;
	// Start writing snapshots.
	qboolean isFirstSnapshot = qtrue;
	std::stringstream ss;
	while(1){
		commandsToAdd.clear();
		eventsToAdd.clear();
		qboolean allSourceDemosFinished = qtrue;
		playerEntities.clear();
		for (int i = 0; i < demoReaders.size(); i++) {
			if (demoReaders[i]->reader.SeekToTime(sourceTime - demoReaders[i]->sourceInfo->delay))
			{ // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
				

				int smallestSnapNumUsed = INT_MAX;

				demoType_t sourceDemoType = demoReaders[i]->reader.getDemoType();
				//std::map<int, entityState_t> hereEntities = demoReaders[i]->reader.GetCurrentEntities();
				//tmpPS = demoReaders[i].GetCurrentPlayerState();
				//tmpPS = demoReaders[i]->reader.GetInterpolatedPlayerState(sourceTime); 
				SnapshotInfo* snapInfo = demoReaders[i]->reader.GetSnapshotInfoAtOrBeforeDemoTime(sourceTime - demoReaders[i]->sourceInfo->delay, qtrue); // Find out which player to show at this time
				SnapshotInfo* realSourceSnap = NULL;
				tmpPS = demoReaders[i]->reader.GetInterpolatedPlayer(snapInfo->playerState.clientNum, sourceTime - demoReaders[i]->sourceInfo->delay - demoReaders[i]->playersToCopy[snapInfo->playerState.clientNum].pingCompensation, &realSourceSnap); // Interpolate him. TODO: Make more efficient.
				std::map<int, entityState_t> hereEntities = snapInfo->entities;

				smallestSnapNumUsed = std::min(smallestSnapNumUsed, snapInfo->snapNum);
				if (realSourceSnap) {
					smallestSnapNumUsed = std::min(smallestSnapNumUsed, realSourceSnap->snapNum);
				}

				demoReaders[i]->reader.convertPSTo(&tmpPS,demoType);

				int originalPlayerstateClientNum = tmpPS.clientNum;
				tmpPS.clientNum = i;
				tmpPS.commandTime = time;

				// Process any changes of the spectated player in the original demo by just updating our configstring and setting teleport bit.
				if (lastSpectatedClientNums[i] != originalPlayerstateClientNum && i<MAX_CLIENTS) {
					tmpConfigString = demoReaders[i]->reader.GetPlayerConfigString(originalPlayerstateClientNum, &tmpConfigStringMaxLength);
					if (strlen(tmpConfigString)) {
						demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + i, tmpConfigString, demoType);
					}
					ss.str(std::string());
					ss << "cs " << (CS_PLAYERS + i) << " " << tmpConfigString;
					commandsToAdd.push_back(ss.str());
					tmpPS.eFlags |= getEF_TELEPORTBIT(demoType);
				}
				lastSpectatedClientNums[i]= originalPlayerstateClientNum;

				tmpPS.saberEntityNum = inputFiles->size() + i; // so cgame doesnt confuse sabers with players/animents

				// self explanatory.
				if (i == 0) {
					mainPlayerPS = tmpPS;
				}
				else {
					BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse, sourceDemoType, qtrue);
					if (i >= MAX_CLIENTS) {
						// Can't be ET_PLAYER. Has to be ET_GRAPPLE (G2 anim ent)
						tmpES.eType = ET_GRAPPLE_JK2;
						// We don't have a player model. So instead get a ModelIndex for this playermodel
						{
							int maxLength;
							//const char* playerInfo = demoReaders[i].GetConfigString(CS_PLAYERS+ originalPlayerstateClientNum,&maxLength);
							const char* playerInfo = demoReaders[i]->reader.GetPlayerConfigString(originalPlayerstateClientNum,&maxLength);
							std::string thisModel = Info_ValueForKey(playerInfo,maxLength,"model");

							std::transform(thisModel.begin(), thisModel.end(), thisModel.begin(), tolowerSignSafe);
							size_t firstSlash = thisModel.find_first_of("/");

							bool variantExists = false;
							std::string baseModel = "";
							std::string variant = "";
							if (firstSlash == std::string::npos) {
								// No variant. Just the model name
								baseModel = thisModel;
								//baseModelCString = thisModel->c_str();
							}
							else {
								baseModel = thisModel.substr(0, firstSlash);
								//baseModelCString = thisModel->c_str();
								if (firstSlash < (thisModel.size() - 1)) {
									variantExists = true;
									variant = thisModel.substr(firstSlash + 1, thisModel.size() - firstSlash - 1);
									//variantCString = variant.c_str();
								}
							}

							tmpES.modelindex = G_ModelIndex(va("models/players/%s/model.glm", baseModel.c_str()), &demo.cut.Cl, &commandsToAdd, demoType);
						}
						/*tmpES.number += 32; // This was just for debugging.
						tmpES.clientNum += 32;
						playerEntities[i+32]= tmpES;*/
					}
					playerEntities[i]= tmpES;
				}


				double thisTimeInServerTime;
				int entitiesSourceSnapNum = INT_MAX;
				std::map<int, entityState_t> sourceEntitiesAtTime = demoReaders[i]->reader.GetEntitiesAtTime(sourceTime - demoReaders[i]->sourceInfo->delay, &thisTimeInServerTime,&entitiesSourceSnapNum);

				smallestSnapNumUsed = std::min(smallestSnapNumUsed, entitiesSourceSnapNum);

				for (auto it = sourceEntitiesAtTime.begin(); it != sourceEntitiesAtTime.end(); it++) {

					int generalizedEntityType = generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(it->second.eType, sourceDemoType);


					// Shots, rockets etc
					// We can't really tell who they belong to. Just copy them.
					if (generalizedEntityType == ET_MISSILE_GENERAL) {

						int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
						if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
							entityState_t tmpEntity = it->second;
							tmpEntity.eType = ET_MISSILE_JK2;
							tmpEntity.weapon = convertGameValue<GMAP_WEAPONS, SAFE>(tmpEntity.weapon, sourceDemoType, demoType);
							tmpEntity.number = targetEntitySlot;
							remapConfigStrings(&tmpEntity, &demo.cut.Cl, &demoReaders[i]->reader, &commandsToAdd, qfalse, qfalse, demoType);
							retimeEntity(&tmpEntity, thisTimeInServerTime, time);
							playerEntities[targetEntitySlot] = tmpEntity;
						}
					}
				}

				// Get new commands
				std::vector<std::string> newCommandsHere = demoReaders[i]->reader.GetNewCommands(sourceTime - demoReaders[i]->sourceInfo->delay);
				for (int c = 0; c < newCommandsHere.size(); c++) {

					Cmd_TokenizeString(newCommandsHere[c].c_str());
					char* cmd = Cmd_Argv(0);
					if (!strcmp(cmd, "print") || !strcmp(cmd, "chat")/* || !strcmp(cmd, "cp")*/) {
						if (!strstr(newCommandsHere[c].c_str(),"!respos") && !strstr(newCommandsHere[c].c_str(), "!savepos")) { // TODO Make this case insensitive for absolute protection!
							commandsToAdd.push_back(newCommandsHere[c]);
						}
					}
				}

				// Get new events and remember them.
				std::vector<Event> newEventsHere = demoReaders[i]->reader.GetNewEvents(sourceTime - demoReaders[i]->sourceInfo->delay);
				for (int c = 0; c < newEventsHere.size(); c++) {
					Event* thisEvent = &newEventsHere[c];
					int eventNumber = thisEvent->eventNumber;
					qboolean addThisEvent = qfalse;
					if (eventNumber == EV_PLAYER_TELEPORT_IN_JK2 || eventNumber == EV_PLAYER_TELEPORT_OUT_JK2) {
						if (thisEvent->theEvent.clientNum == originalPlayerstateClientNum) {
							thisEvent->theEvent.clientNum = i;
							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_OBITUARY_JK2) {
						addThisEvent = qtrue; // TODO Fix this, this is nonsensical here really...just for testing
					}
					else if (eventNumber == EV_MISSILE_HIT_JK2 || eventNumber == EV_MISSILE_MISS_JK2 || eventNumber == EV_MISSILE_MISS_METAL_JK2) {
						addThisEvent = qtrue; // for q3 rocket jumpeeee
						thisEvent->theEvent.weapon = convertGameValue<GMAP_WEAPONS, SAFE>(thisEvent->theEvent.weapon, sourceDemoType, demoType);
					}
					if (addThisEvent) eventsToAdd.push_back(*thisEvent);
				}

				demoReaders[i]->reader.purgeSnapsBefore(smallestSnapNumUsed);
			}

			if (!demoReaders[i]->reader.EndReachedAtTime(sourceTime - demoReaders[i]->sourceInfo->delay)) {
				allSourceDemosFinished = qfalse;
			}

		}

		// Find empty places in entities and add events.
		for (int i = 0; i < eventsToAdd.size(); i++) {
			for (int e = MAX_CLIENTS; e < MAX_GENTITIES-2; e++) { // Can I use full MAX_GENTITIES amount? Isn't MAX_GENTITIES -1 reserved for world and stuff like that?
				if (playerEntities.find(e) == playerEntities.end()) {
					// Let's add it!
					eventsToAdd[i].theEvent.number = e;
					playerEntities[e] = eventsToAdd[i].theEvent;
					break;
				}
			}
		}

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0]->reader.GetCurrentSnap();
		Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 

		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, NULL,NULL, createCompressedOutput);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, &playerEntitiesOld, &mainPlayerPSOld, createCompressedOutput);
		}

		time += 1000.0 / fps;
		sourceTime += 1000.0 / fps;
		demo.cut.Clc.reliableSequence++;
		demo.cut.Clc.serverMessageSequence++;

		mainPlayerPSOld = mainPlayerPS;
		playerEntitiesOld.clear();
		for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
			playerEntitiesOld[it->first] = it->second;
		}

		if (allSourceDemosFinished) break;
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


int main(int argc, char** argv) {
	if (argc <3) {
		std::cout << "need 2 arguments at least: outputname, demoname1, [demoname2, demoname3,...]";
		return 1;
	}
	initializeGameInfos();
	char* demoName = NULL;
	char* outputName = NULL;

	outputName = argv[1];
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName,qtrue);
	strcpy(outputName, filteredOutputName);
	delete[] filteredOutputName;

	std::vector<std::string> inputFiles;
	for (int i = 2; i < argc; i++) {
		inputFiles.emplace_back(argv[i]);
	}
	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	if (demoCut(outputName,&inputFiles)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully defrag combined in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get defrag combined or defrag combined with errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif