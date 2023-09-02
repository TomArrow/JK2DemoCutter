#include "demoCut.h"
#include "DemoReader.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <chrono>
#include <jkaStuff.h>
#include <otherGameStuff.h>
#include <otherGameStuff2.h>

#include "tsl/htrie_map.h"

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


class DemoReaderTrackingWrapper {
public:
	DemoReader reader;
	int packetsUsed = 0;
};



tsl::htrie_map<char, int> playerNamesToClientNums;
qboolean clientNameIsDuplicate[MAX_CLIENTS_MAX];



void updatePlayerDemoStats(DemoReader* reader) {

	bool isMOHAADemo = reader->isThisMOHAADemo();

	if (!isMOHAADemo) return;

	int max_clients = reader->getMaxClients();

	if (isMOHAADemo) {
		playerNamesToClientNums.clear();
		Com_Memset(clientNameIsDuplicate, 0, sizeof(clientNameIsDuplicate));
	}

	int maxLength = 0;
	int CS_PLAYERS_here = getCS_PLAYERS(reader->getDemoType());
	for (int i = 0; i < max_clients; i++) {
		const char* playerInfo = reader->GetConfigString(CS_PLAYERS_here + i, &maxLength);
		std::string playerName = Info_ValueForKey(playerInfo, maxLength, isMOHAADemo ? "name" : "n");

		if (isMOHAADemo && *playerInfo) { // Don't insert non existing players.
			auto existingName = playerNamesToClientNums.find(playerName.c_str());
			if (existingName != playerNamesToClientNums.end()) {
				int otherClientNum = existingName.value();
				clientNameIsDuplicate[otherClientNum] = qtrue;
				clientNameIsDuplicate[i] = qtrue;
				// Can't do this in reframer atm TODO
				//if (playerLastSeen[i] > playerLastSeen[otherClientNum]) {
				//	// Prioritize this player because he was seen more recently (duplicates could be lagged out connections). SHITTY solution, especially since it's only triggered during CS change.
				//	// TODO FIX THIS omg
				//	playerNamesToClientNums[playerName.c_str()] = i;
				//}
			}
			else {
				playerNamesToClientNums[playerName.c_str()] = i; // The lowest client number with that name will be accepted... kinda dumb but idk.
			}
		}
	}
}


#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


qboolean demoReframe( const char* demoName,const char* outputName, const char* playerSearchString, qboolean visAll) {
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
	qboolean		createCompressedOutput;

	// TODO Deal with extensions better, kind of annoying, also what about 1.03 detection?

	//strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);

#if MOHAAANGLEDEBUG
	float tmpBig=0, tmpSmall=0;
	float tmpBigPS=0, tmpSmallPS=0;
#endif

	demoCutGetDemoType(outputName,ext, outputNameNoExt ,&demoType, &createCompressedOutput);

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

	//std::vector<DemoReaderTrackingWrapper> demoReaders;
	std::cout << "loading up demo...";
	DemoReaderTrackingWrapper* demoReader = new DemoReaderTrackingWrapper();
	int startTime = INT_MAX;
	demoReader->reader.LoadDemo(demoName);
	startTime = std::min(startTime, demoReader->reader.GetFirstSnapServerTime()); // Find start time.
	std::cout << "done.";

	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	//int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(demoType);
	for (int i = 0; i < maxAllowedConfigString; i++) {
		tmpConfigString = demoReader->reader.GetConfigString(i,&tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString,demoType);
		}
	}

	//std::map<int, int> lastSpectatedClientNums; // Need this for later.

	// Copy over player config strings
	/*for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i].reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
			int spectatedClient = demoReaders[i].reader.GetCurrentPlayerState().clientNum;
			lastSpectatedClientNums[i] = spectatedClient;			
			//if (i >= MAX_CLIENTS) continue; // We don't have names/configstrings for players > 32
			//tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(spectatedClient,&tmpConfigStringMaxLength);
			//if (strlen(tmpConfigString)) {
				//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS+i, tmpConfigString);
			//}
		}
	}*/
	
	//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_LEVEL_START_TIME, "10000");

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING_MAX];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy,demoType);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially reframed using JK2DemoCutter tools.", demoType);

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName), "%s%s", outputNameNoExt, ext);
	while (FS_FileExists(newName)) {
		Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputNameNoExt, dupeIterator, ext);
		dupeIterator++;
	}

	newHandle = FS_FOpenFileWrite(newName,createCompressedOutput ? FILECOMPRESSION_LZMA :FILECOMPRESSION_NONE);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	std::string playerSearchStdString = playerSearchString;
	int reframeClientNum = demoReader->reader.getClientNumForDemo(&playerSearchStdString);

	if (reframeClientNum == -1) {
		Com_DPrintf("Failed to find matching player.\n");
		return qfalse;
	}

	// Write demo header
	if (isMOHAADemo) {
		demo.cut.Clc.clientNum = reframeClientNum;

	}

	int CS_PLAYERS_here = getCS_PLAYERS(demoReader->reader.getDemoType());


	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	updatePlayerDemoStats(&demoReader->reader);

	int time = startTime; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	std::map<int, entityState_t> playerEntities;
	std::map<int, entityState_t> playerEntitiesOld;
	std::vector<std::string> commandsToAdd;
	std::vector<Event> eventsToAdd;
	playerState_t tmpPS, mainPlayerPS, mainPlayerPSOld;
	mainPlayerPS.clientNum = -1;
	entityState_t tmpES;
	int currentCommand = 1;
	// Start writing snapshots.
	qboolean isFirstSnapshot = qtrue;
	std::stringstream ss;
	int framesWritten = 0;
	SnapshotInfoMapIterator nullIt = demoReader->reader.SnapNullIt();
	SnapshotInfoMapIterator nextSnapInfoHereIt = nullIt;

	int maxClientsHere = demoReader->reader.getMaxClients();

	struct {
		int lastViewModelAnim = -1;
		int lastEquippedWeapon = -1;
		int viewModelAnimChanged = 0;
	} mohaaState;

	while(1){
		commandsToAdd.clear();
		eventsToAdd.clear();
		qboolean allSourceDemosFinished = qtrue;
		playerEntities.clear();
		int nonSkippedDemoIndex = 0;
		//static qboolean entityIsInterpolated[MAX_GENTITIES];
		//Com_Memset(entityIsInterpolated, 0, sizeof(entityIsInterpolated));
		//qboolean mainPlayerPSIsInterpolated = qfalse;
		//for (int i = 0; i < demoReaders.size(); i++) {
		SnapshotInfoMapIterator sourceSnap = nullIt; // snap that actually contains the player so we can use its areamask.
		SnapshotInfoMapIterator snapInfoHereIt = nullIt;
			if (demoReader->reader.SeekToServerTime(time)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
				
				sourceSnap = snapInfoHereIt = nextSnapInfoHereIt != nullIt ? nextSnapInfoHereIt : demoReader->reader.GetSnapshotInfoAtServerTimeIterator(time);
				SnapshotInfo* snapInfoHere = &snapInfoHereIt->second;
				//qboolean snapIsInterpolated = qfalse;
				//if (!snapInfoHere) {
				//	int thisDemoLastServerTime = demoReader.reader.GetLastServerTimeBeforeServerTime(time);
				//	int thisDemoNextServerTime = demoReader.reader.GetFirstServerTimeAfterServerTime(time);
				//	if (thisDemoLastServerTime == -1 || thisDemoNextServerTime == -1)continue;

				//	snapInfoHere = demoReader.reader.GetSnapshotInfoAtServerTime(thisDemoLastServerTime);
					// TODO Do actual interpolation instead of just copying last one. Don't copy entities that are in previous but not in next.
				//	snapIsInterpolated = qtrue;
				//}
				//else {
					demoReader->packetsUsed++;
				//}
				//std::map<int, entityState_t> hereEntities = demoReaders[i].GetCurrentEntities();
				//tmpPS = demoReaders[i].GetCurrentPlayerState();
				tmpPS = snapInfoHere->playerState;


				// self explanatory.
				//if (nonSkippedDemoIndex++ == 0 || (tmpPS.clientNum == mainPlayerPS.clientNum && mainPlayerPSIsInterpolated && !snapIsInterpolated)) { // TODO MAke this more sophisticated. Allow moving over some non-snapped values from entitystates perhaps to smooth out mainPlayerPS
				// If intermission (basically scoreboard): Don't reframe. Just show scoreboard.
				if (tmpPS.clientNum == reframeClientNum || (tmpPS.pm_type == PM_INTERMISSION/* && tmpPS.clientNum == demoReader->reader.getDemoRecorderClientNum()*/)) { // TODO MAke this more sophisticated. Allow moving over some non-snapped values from entitystates perhaps to smooth out mainPlayerPS
					// For reference, here's what gets snapped (rounded) in entities:
					// SnapVector( s->pos.trBase );
					// SnapVector( s->apos.trBase );
					mainPlayerPS = tmpPS;
					//mainPlayerPSIsInterpolated = snapIsInterpolated;
				}
				else if(tmpPS.clientNum != mainPlayerPS.clientNum) {

					mainPlayerPS = demoReader->reader.GetLastOrNextPlayer(reframeClientNum, time, &sourceSnap,NULL, qtrue,&snapInfoHereIt);
					/*BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse);
					if (playerEntities.find(tmpPS.clientNum) == playerEntities.end() || entityIsInterpolated[tmpPS.clientNum]) { // Prioritize entities that are not interpolated
						playerEntities[tmpPS.clientNum] = tmpES;
						entityIsInterpolated[tmpPS.clientNum] = snapIsInterpolated;
					}*/
				}

				mainPlayerPS.clientNum = reframeClientNum;

				if (isMOHAADemo) {

					if (mainPlayerPS.iNetViewModelAnim != mohaaState.lastViewModelAnim || mainPlayerPS.stats[STAT_EQUIPPED_WEAPON_MOH] != mohaaState.lastEquippedWeapon) {
						mohaaState.viewModelAnimChanged = (mohaaState.viewModelAnimChanged+1)&3;
					}
					mainPlayerPS.iViewModelAnimChanged = mohaaState.viewModelAnimChanged;
				}

				//
				// Note:
				// MOHAA doesn't seem to send proper view angles for entities
				// But it sends bone_angles and for bone_angles[0] (head) the equation is something like:
				// bone_angle[0] = view_angle[0] -  0.90f * view_angle[0] * 0.70f
				// So bone_angle/0.37f roughly gives us the proper view angle? Not sure though, have to verify.
				//
				//

				// Copy all entities
				// Entities from other demos will automatically overwrite entities from this demo.
				for (auto it = snapInfoHere->entities.begin(); it != snapInfoHere->entities.end(); it++) {
					//if (it->first != mainPlayerPS.clientNum) {
					if (it->first != reframeClientNum || isMOHAADemo) {
						//if (playerEntities.find(it->first) == playerEntities.end() || entityIsInterpolated[it->first]) { // Prioritize entities that are not interpolated

						//if (isMOHAADemo && it->first >= 0 && it->first < maxClientsHere) {
						//	
						//	if(VectorDistance(snapInfoHere->playerState.origin,it->second.))
						//}

#if MOHAAANGLEDEBUG
						if (it->second.number < 64) {
							if (it->second.bone_angles[0][0]/0.37f < tmpSmall) {
								tmpSmall = it->second.bone_angles[0][0] / 0.37f;
							}
							if (it->second.bone_angles[0][0] / 0.37f > tmpBig) {
								tmpBig = it->second.bone_angles[0][0] / 0.37f;
							}
							//std::cout << it->second.apos.trBase[0] << "," << it->second.apos.trBase[1] << "," << it->second.apos.trBase[2] << "\n";
							//std::cout << it->second.number << ": " << it->second.bone_angles[0][0] << "," << it->second.bone_angles[0][1] << "," << it->second.bone_angles[0][2] << "\n";
							
						}
#endif
							//if (isMOHAADemo && it->second.parent == reframeClientNum) {
							//	continue;
							//}
							playerEntities[it->first] = it->second;
						//	entityIsInterpolated[it->first] = snapIsInterpolated;
						//}
					}
				}
#if MOHAAANGLEDEBUG
				if (tmpPS.viewangles[0] < tmpSmallPS) {
					tmpSmallPS = tmpPS.viewangles[0];
				}
				if (tmpPS.viewangles[0] > tmpBigPS) {
					tmpBigPS = tmpPS.viewangles[0];
				}
				std::cout << tmpSmall << "," << tmpBig << ","<< tmpSmallPS << "," << tmpBigPS << "\n";
#endif

				if (tmpPS.clientNum != reframeClientNum) {
					Com_Memset(&tmpES, 0, sizeof(tmpES));
					BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse,demoType, qtrue);
					playerEntities[tmpPS.clientNum] = tmpES;
				}


				// Get new commands
				bool anyConfigStringcommands = false;
				std::vector<std::string> newCommandsHere = demoReader->reader.GetNewCommandsAtServerTime(time);
				for (int c = 0; c < newCommandsHere.size(); c++) {

					bool skipMessage = false;
					if (isMOHAADemo) { // For MOHAA we have to add "You killed XXX" messages and also remove them in case they were part of the original demo (so we only get them in the right angle).

						const char* command = newCommandsHere[c].c_str();
						Cmd_TokenizeString(command);

						char* cmd = Cmd_Argv(0);

						if (!strcmp(cmd, "print")) {
							
							static const char youKilledPrefix[] = "\x03You killed"; // "Has matado a ti mismo" / "Has matado a &&&"
							static const int youKilledPrefixLength = sizeof(youKilledPrefix)-1;
							static char youKilledPrefixCompare[youKilledPrefixLength+1];

							strncpy_s(youKilledPrefixCompare, Cmd_Argv(1), youKilledPrefixLength);
							youKilledPrefixCompare[youKilledPrefixLength] = 0;

							if (!_stricmp(youKilledPrefix, youKilledPrefixCompare)) {
								skipMessage = true; // It's a "You killed XXX" message. Dump it. We're recreating these from scratch.
							}
							else {

								// If it's not, check if it's a death message.
								entityState_t* deathEvent = parseMOHAADeathMessage(&playerNamesToClientNums, Cmd_Argv(1));
								if (deathEvent) {
									int				target = deathEvent->otherEntityNum;
									int				attacker = deathEvent->otherEntityNum2;
									if (attacker == reframeClientNum) {
										if (target != reframeClientNum) {

											int maxLength = 0;
											const char* playerInfo = demoReader->reader.GetConfigString(CS_PLAYERS_here + target, &maxLength);
											const char* playerName = Info_ValueForKey(playerInfo, maxLength, isMOHAADemo ? "name" : "n");
											std::string newMsg = va("print \"\x03You killed %s\n\"", playerName);
											commandsToAdd.push_back(newMsg);
										}
										else {

											std::string newMsg = va("print \"\x03You killed yourself\n\""); 
											commandsToAdd.push_back(newMsg);
										}
									}

									delete deathEvent;
								}
							}

						}
						else if (!strcmp(cmd, "cs")) {
							anyConfigStringcommands = true;
						}
					}

					//if (i == 0) { // This is the main reference demo. Just add this stuff.
						if(!skipMessage) commandsToAdd.push_back(newCommandsHere[c]);
					/* }
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

				if (anyConfigStringcommands) {
					updatePlayerDemoStats(&demoReader->reader); // TODO Also do this if gamestate. Somehow.
				}

			}
			if (!demoReader->reader.EndReachedAtServerTime(time)) {
				allSourceDemosFinished = qfalse;
			}
		//}


		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		if (visAll || sourceSnap == nullIt) {
			Com_Memset(demo.cut.Cl.snap.areamask, 0, sizeof(demo.cut.Cl.snap.areamask));
		}
		else {
			byte	areamasHere[MAX_MAP_AREA_BYTES];
			// We combine tmpPS and source snap playerstate
			for (int amb = 0; amb < MAX_MAP_AREA_BYTES; amb++) {
				areamasHere[amb] = sourceSnap->second.areamask[amb] & snapInfoHereIt->second.areamask[amb];
			}
			Com_Memcpy(demo.cut.Cl.snap.areamask, areamasHere, sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do.  TODO: Actually in some older demos this results in hall of mirrors effect hmm
		}
		
		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, NULL,NULL,createCompressedOutput);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, &playerEntitiesOld, &mainPlayerPSOld, createCompressedOutput);
		}

		framesWritten++;

		int oldTime = time;
		time = INT_MAX;
		//int nextTimeThisDemo = demoReader->reader.GetFirstServerTimeAfterServerTime(oldTime);
		int nextTimeThisDemo = -1;
		nextSnapInfoHereIt = demoReader->reader.GetFirstSnapshotAfterSnapshotIterator(snapInfoHereIt);
		if (nextSnapInfoHereIt != nullIt) {
			nextTimeThisDemo = nextSnapInfoHereIt->second.serverTime;
		}
		if (nextTimeThisDemo != -1) {
			time = std::min(time, nextTimeThisDemo); // Find nearest serverTime of all the demos.
		}
		demo.cut.Clc.reliableSequence++;
		demo.cut.Clc.serverMessageSequence++;

		mainPlayerPSOld = mainPlayerPS;
		playerEntitiesOld.clear();
		for (auto it = playerEntities.begin(); it != playerEntities.end(); it++) {
			playerEntitiesOld[it->first] = it->second;
		}

		if (allSourceDemosFinished || time == INT_MAX) {
			break;
		}
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
	std::cout << "Frames from demo " << ": " << demoReader->packetsUsed << " (" << (demoReader->packetsUsed*100/framesWritten) << "%)\n";

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
	if (argc <4) {
		std::cout << "need 3 arguments at least: demoname, outputname, player to follow (search string or clientnum), [optional: visall for demos that result in graphical artifacts]";
		return 1;
	}
	initializeGameInfos();
	char* demoName = NULL;
	char* outputName = NULL;
	char* playerNameSearchString = NULL;

	demoName = argv[1];
	outputName = argv[2];
	playerNameSearchString = argv[3];
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName,qtrue);
	
	qboolean visAll = qfalse;
	if (argc == 5 && !stricmp(argv[4],"visall")) {
		visAll = qtrue;
	}

	std::cout << sizeof(DemoReaderTrackingWrapper);

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();

	if (demoReframe(demoName, filteredOutputName, playerNameSearchString, visAll)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully reframed in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get reframed or reframed with errors\n", demoName);
	}
	delete[] filteredOutputName;
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif