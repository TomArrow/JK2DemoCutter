#include "demoCut.h"
#include "DemoReader.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <chrono>
#include <jkaStuff.h>
#include <otherGameStuff.h>

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


class DemoReaderTrackingWrapper {
public:
	DemoReader reader;
	int packetsUsed = 0;
};







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

	strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);

	demoCutGetDemoType(outputName,ext,&demoType, &createCompressedOutput);
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
	char infoCopy[MAX_INFO_STRING];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
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
		Com_Printf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	std::string playerSearchStdString = playerSearchString;
	int reframeClientNum = demoReader->reader.getClientNumForDemo(&playerSearchStdString);

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
			if (demoReader->reader.SeekToServerTime(time)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
				
				SnapshotInfo* snapInfoHere = demoReader->reader.GetSnapshotInfoAtServerTime(time);
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

					mainPlayerPS = demoReader->reader.GetLastOrNextPlayer(reframeClientNum, time, NULL, NULL, qtrue,snapInfoHere);
					/*BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse);
					if (playerEntities.find(tmpPS.clientNum) == playerEntities.end() || entityIsInterpolated[tmpPS.clientNum]) { // Prioritize entities that are not interpolated
						playerEntities[tmpPS.clientNum] = tmpES;
						entityIsInterpolated[tmpPS.clientNum] = snapIsInterpolated;
					}*/
				}



				// Copy all entities
				// Entities from other demos will automatically overwrite entities from this demo.
				for (auto it = snapInfoHere->entities.begin(); it != snapInfoHere->entities.end(); it++) {
					if (it->first != mainPlayerPS.clientNum) {
						//if (playerEntities.find(it->first) == playerEntities.end() || entityIsInterpolated[it->first]) { // Prioritize entities that are not interpolated
							playerEntities[it->first] = it->second;
						//	entityIsInterpolated[it->first] = snapIsInterpolated;
						//}
					}
				}

				if (tmpPS.clientNum != reframeClientNum) {
					Com_Memset(&tmpES, 0, sizeof(tmpES));
					BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse,demoType, qtrue);
					playerEntities[tmpPS.clientNum] = tmpES;
				}


				// Get new commands
				std::vector<std::string> newCommandsHere = demoReader->reader.GetNewCommandsAtServerTime(time);
				for (int c = 0; c < newCommandsHere.size(); c++) {

					//if (i == 0) { // This is the main reference demo. Just add this stuff.
						commandsToAdd.push_back(newCommandsHere[c]);
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

			}
			if (!demoReader->reader.EndReachedAtServerTime(time)) {
				allSourceDemosFinished = qfalse;
			}
		//}


		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReader->reader.GetCurrentSnap();
		if (visAll) {
			Com_Memset(demo.cut.Cl.snap.areamask, 0, sizeof(demo.cut.Cl.snap.areamask));
		}
		else {
			Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask, sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do.  TODO: Actually in some older demos this results in hall of mirrors effect hmm
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
		int nextTimeThisDemo = demoReader->reader.GetFirstServerTimeAfterServerTime(oldTime);
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
		Com_Printf("Demo %s has failed to get reframed or reframed with errors\n", demoName);
	}
	delete[] filteredOutputName;
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif