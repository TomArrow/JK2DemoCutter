#include "demoCut.h"
#include "jkaStuff.h"
#include "otherGameStuff.h"
#include "DemoReaderLight.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>




// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//







qboolean DemoReaderLight::PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap) {
	// if the next frame is a teleport for the playerstate, we
	// can't interpolate during demos
	if (lastSnap && ((snap->ps.eFlags ^ lastSnap->ps.eFlags) & EF_TELEPORT_BIT)) {
		return qtrue;
	}

	// if changing follow mode, don't interpolate
	if (snap->ps.clientNum != lastSnap->ps.clientNum) {
		return qtrue;
	}

	// if changing server restarts, don't interpolate
	if ((snap->snapFlags ^ lastSnap->snapFlags) & SNAPFLAG_SERVERCOUNT) {
		return qtrue;
	}

	return qfalse;
}









#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


entityState_t* DemoReaderLight::findEntity(int number) {
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

		if (thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}


qboolean DemoReaderLight::LoadDemo(const char* sourceDemoFile) {
	char			oldName[MAX_OSPATH];
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	//char* ext; 
	char ext[7];
	//char originalExt[7];
	oldHandle = 0; // TODO if already loaded, gracefully close

	thisDemo.cut.Clc.demoCheckFor103 = qfalse;
	strncpy_s(oldName, sizeof(oldName), sourceDemoFile, strlen(sourceDemoFile) - 6);

	demoCutGetDemoType(sourceDemoFile, ext, &demoType, &isCompressedFile, &thisDemo.cut.Clc.demoCheckFor103);

	maxClientsThisDemo = getMAX_CLIENTS(demoType);

	/*ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	if (!*ext) {
		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		ext = ".dm_15"; 
		thisDemo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext, ".dm_26")) {

		demoType = DM_26;
		ext = ".dm_26";
	}*/
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue,isCompressedFile);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	memset(&playerSeen, 0, sizeof(playerSeen));

	readGamestate = 0;

	demoStartTime = 0;
	demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	demoCurrentTime = 0;
	lastGameStateChange = 0;
	lastGameStateChangeInDemoTime = 0;
	lastKnownTime = 0;
	thisDemo.cut.Clc.lastKnownCommandTime = 0;
	messageOffset = 0;
	lastGottenCommandsTime = 0;
	lastGottenEventsTime = 0;

	for (int i = 0; i < maxClientsThisDemo; i++) {
		pingValues[i].clear();
	}


	//while (oldSize > 0) ReadMessage();

	ReadMessage(); // Will only read pretty much the first message so we have gamestate and are somewhat "initialized" i guess
}

qboolean DemoReaderLight::CloseDemo() {

	FS_FCloseFile(oldHandle);
	return qtrue;
}
qboolean DemoReaderLight::AnySnapshotParsed() {

	return anySnapshotParsed;
}
qboolean DemoReaderLight::EndReached() {

	return endReached;
}
qboolean DemoReaderLight::EndReachedAtTime(float time) {
	SeekToTime(time);
	return (qboolean)(demoCurrentTime < time);
}

qboolean DemoReaderLight::SeekToTime(int time) {
	while (demoCurrentTime < time && !endReached) {
		ReadMessage();
	}
	if (demoCurrentTime < time && endReached) return qfalse;
	return qtrue;
}
void DemoReaderLight::ReadToEnd() {
	while (!endReached) {
		ReadMessage();
	}
}
qboolean DemoReaderLight::SeekToServerTime(int serverTime) {
	while (lastKnownTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReaderLight::SeekToCommandTime(int serverTime) {
	while (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReaderLight::SeekToAnySnapshotIfNotYet() {
	while (!anySnapshotParsed && !endReached) {
		ReadMessage();
	}
	if (!anySnapshotParsed && endReached) return qfalse;
	return qtrue;
}
playerState_t DemoReaderLight::GetCurrentPlayerState() {
	return thisDemo.cut.Cl.snap.ps;
}

void DemoReaderLight::GetPlayersSeen(qboolean* playersSeenA) { // Requires pointer to array with 32 qbooleans
	Com_Memcpy(playersSeenA, playerSeen, sizeof(playerSeen));
}
void DemoReaderLight::GetMedianPingData(int* playerPingData) { // Requires pointer to array with 32 ints

	for (int i = 0; i < maxClientsThisDemo; i++) {
		int countHere = pingValues[i].size();
		if (countHere > 0) {
			sort(pingValues[i].begin(),pingValues[i].end());
			if (countHere % 2) {
				// Median with averaged center values
				playerPingData[i] = (pingValues[i][countHere / 2]+ pingValues[i][countHere / 2-1])/2;
			}
			else {
				// True median
				playerPingData[i] = pingValues[i][countHere / 2]; // Integer division always floors the value. So let's say, count 3: 3/2 = 1.5 => 1. Index 1 is used, which is correct.
			}
		}
		else {
			playerPingData[i] = 0;
		}
	}

}
void DemoReaderLight::GetLowestPingData(int* playerPingData) { // Requires pointer to array with 32 ints

	for (int i = 0; i < maxClientsThisDemo; i++) {
		int countHere = pingValues[i].size();
		if (countHere > 0) {
			int lowestPing = INT_MAX;
			for (int b = 0; b < pingValues[i].size(); b++) {
				if (pingValues[i][b] < lowestPing) lowestPing = pingValues[i][b];
			}
			playerPingData[i] = lowestPing;
		}
		else {
			playerPingData[i] = 0;
		}
	}

}
#define LOCAL_AVERAGE_SAMPLE_COUNT 100
void DemoReaderLight::GetMedianOfLocalAveragesPingData(float* playerPingData) { // Requires pointer to array with 32 ints

	for (int i = 0; i < maxClientsThisDemo; i++) {

		static float localAverageData[LOCAL_AVERAGE_SAMPLE_COUNT];
		std::vector<float> localAverages;
		int s = 0;
		for (; s < pingValues[i].size(); s++) {
			localAverageData[s % LOCAL_AVERAGE_SAMPLE_COUNT] = pingValues[i][s]; // Add sample to pool
			if (s >= LOCAL_AVERAGE_SAMPLE_COUNT) {
				// Run the average. But only once the whole array is filled.
				averageHelper_t averageHelper;
				Com_Memset(&averageHelper, 0, sizeof(averageHelper));
				for (int c = 0; c < LOCAL_AVERAGE_SAMPLE_COUNT; c++) {
					averageHelper.sum += localAverageData[c];
					averageHelper.divisor++;
				}
				localAverages.push_back(averageHelper.sum / averageHelper.divisor);
			}
		}


		int countHere = localAverages.size();
		if (countHere > 0) {
			sort(localAverages.begin(), localAverages.end());
			if (countHere % 2) {
				// Median with averaged center values
				playerPingData[i] = (localAverages[countHere / 2]+ localAverages[countHere / 2-1])/2;
			}
			else {
				// True median
				playerPingData[i] = localAverages[countHere / 2]; // Integer division always floors the value. So let's say, count 3: 3/2 = 1.5 => 1. Index 1 is used, which is correct.
			}
		}
		else {
			playerPingData[i] = 0;
		}
	}

}



void DemoReaderLight::FreePingData() {
	for (int i = 0; i < maxClientsThisDemo; i++) {
		pingValues[i].clear();
	}
}


std::map<int,entityState_t> DemoReaderLight::GetCurrentEntities() {
	std::map<int, entityState_t> retVal;
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
		entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		retVal[thisEntity->number] = *thisEntity;
	}
	return retVal;
}



clSnapshot_t DemoReaderLight::GetCurrentSnap() {
	return thisDemo.cut.Cl.snap;
}

const char* DemoReaderLight::GetPlayerConfigString(int playerNum,int* maxLength) {
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + playerNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReaderLight::GetConfigString(int configStringNum, int* maxLength) {
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}


qboolean DemoReaderLight::ReadMessage() {
	if (endReached) return qfalse;
	qboolean realReadResult = qfalse;
	realReadResult = ReadMessageReal();
	if (!realReadResult) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
}


qboolean DemoReaderLight::ReadMessageReal() {

	//constexpr int svc_mapchange_realCMD = D == DM_26 ? svc_mapchange + 1 : svc_mapchange;
	//constexpr int svc_EOF_realCMD = D == DM_26 ? svc_EOF + 1 : svc_EOF;
	//constexpr int svc_setgame_realCMD = D == DM_26 ? 8 : 100; // A non-JKA demo won't have this, let's just set something ridiculously high

readNext:
	int				buf;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte> oldDataRaw;

	if (isCompressedFile) {
		oldDataRaw.clear();
		MSG_InitRaw(&oldMsg, &oldDataRaw);
	}
	else {
		MSG_Init(&oldMsg, oldData, sizeof(oldData));
	}
	/* Read the sequence number */
	if (FS_Read(&thisDemo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4)
		return qfalse;
	thisDemo.cut.Clc.serverMessageSequence = LittleLong(thisDemo.cut.Clc.serverMessageSequence);
	oldSize -= 4;
	/* Read the message size */
	if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4)
		return qfalse;
	oldMsg.cursize = LittleLong(oldMsg.cursize);
	oldSize -= 4;
	/* Negative size signals end of demo */
	if (oldMsg.cursize < 0)
		return qfalse;
	if (oldMsg.cursize > oldMsg.maxsize)
		return qfalse;
	/* Read the actual message */
	if (FS_Read(&oldMsg, oldHandle) != oldMsg.cursize)
		return qfalse;
	oldSize -= oldMsg.cursize;
	// init the bitstream
	MSG_BeginReading(&oldMsg);
	// Skip the reliable sequence acknowledge number
	MSG_ReadLong(&oldMsg);
	//
	// parse the message
	//
	while (1) {
		byte cmd;
		if (oldMsg.readcount > oldMsg.cursize) {
			Com_Printf("Demo cutter, read past end of server message.\n");
			return qfalse;
		}
		cmd = MSG_ReadByte(&oldMsg);
		cmd = generalizeGameSVCOp(cmd,demoType);
		if (cmd == svc_EOF_general) {
			break;
		}
		//I'm not sure what this does or why it does it...
		// skip all the gamestates until we reach needed
		if (readGamestate < thisDemo.currentNum) {
			//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
			if (cmd == svc_gamestate_general) {
				readGamestate++;
			}
			goto readNext;
		}

		// other commands
		switch (cmd) {
		default:
			Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
			return qfalse;
		case svc_nop_general:
			break;
		case svc_serverCommand_general:
			demoCutParseCommandString(&oldMsg, &thisDemo.cut.Clc);
			break;
		case svc_gamestate_general:
			lastGameStateChange = thisDemo.cut.Cl.snap.serverTime;
			lastGameStateChangeInDemoTime = demoCurrentTime;
			//if (readGamestate > demo.currentNum) {
			//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
			//	goto cutcomplete;
			//}
			if (!demoCutParseGamestate(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, &demoType)) {
				return qfalse;
			}
			//Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
			//newHandle = FS_FOpenFileWrite(newName);
			//if (!newHandle) {
			//	Com_Printf("Failed to open %s for target cutting.\n", newName);
			//	return qfalse;
			//}
			readGamestate++;
			break;
		case svc_snapshot_general:
			if (!demoCutParseSnapshot(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, demoType)) {
				return qfalse;
			}

			if (thisDemo.cut.Cl.lastSnapshotFinishedParsing) {
				for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
					entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

					// See if we can figure out the command time of this entity if it's a player.
					if (thisEntity->number >= 0 && thisEntity->number < maxClientsThisDemo) {
						int commandOrServerTimeHere = 0;
						if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
							commandOrServerTimeHere = thisEntity->pos.trTime;
						}
						else {
							commandOrServerTimeHere = thisDemo.cut.Cl.snap.serverTime; // Just use servertime. Will mean ping 0, but best we can do.
						}
						int pingHere = thisDemo.cut.Cl.snap.serverTime - commandOrServerTimeHere;
						pingValues[thisEntity->number].push_back(pingHere);
					}
				}
				int psPingHere = thisDemo.cut.Cl.snap.serverTime - thisDemo.cut.Cl.snap.ps.commandTime;
				pingValues[thisDemo.cut.Cl.snap.ps.clientNum].push_back(psPingHere);
			}

			anySnapshotParsed = qtrue;

			// Time related stuff
			if (messageOffset++ == 0) {
				// first message in demo. Get servertime offset from here to cut correctly.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}
			if (thisDemo.cut.Cl.snap.serverTime < lastKnownTime && thisDemo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
				demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}
			demoCurrentTime = demoBaseTime + thisDemo.cut.Cl.snap.serverTime - demoStartTime;
			lastKnownTime = thisDemo.cut.Cl.snap.serverTime;

			playerSeen[thisDemo.cut.Cl.snap.ps.clientNum] = qtrue;

			// Fire events
			for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

				entityState_t* thisEs = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

				// Document which players we've seen.
				if (thisEs->number >= 0 && thisEs->number < maxClientsThisDemo) {
					playerSeen[thisEs->number] = qtrue;
				}

				int eventNumber = 0;// GetEvent(thisEs);

				if (eventNumber) {
					/*

					// Handle kills
					if (eventNumber == EV_OBITUARY) {
						int				target = thisEs->otherEntityNum;
						int				attacker = thisEs->otherEntityNum2;
						int				mod = thisEs->eventParm;
						bool			victimIsFlagCarrier = false;
						bool			isSuicide;
						bool			isDoomKill;
						bool			isWorldKill = false;
						bool			isVisible = false;
						if (target < 0 || target >= maxClientsThisDemo) {
							std::cout << "CG_Obituary: target out of range. This should never happen really.";
						}

						if (attacker < 0 || attacker >= maxClientsThisDemo) {
							attacker = ENTITYNUM_WORLD;
							isWorldKill = true;
						}
						entityState_t* targetEntity = findEntity(target);
						if (targetEntity) {
							isVisible = true;
							//if (targetEntity->powerups & (1 << PW_REDFLAG) || targetEntity->powerups & (1 << PW_BLUEFLAG)) {
							//	// If the victim isn't visible, his entity won't be available, thus this won't be set
							//	// But we're trying to find interesting moments, so stuff that's not even visible is not that interesting to us
							//	victimIsFlagCarrier = true;
							//}
						}
						victimIsFlagCarrier = target == lastKnownBlueFlagCarrier || target == lastKnownRedFlagCarrier;

						isSuicide = target == attacker;
						if (isSuicide || !victimIsFlagCarrier || isWorldKill || !isVisible) continue; // Not that interesting.

						isDoomKill = mod == MOD_FALLING;

						bool attackerIsFollowed = thisDemo.cut.Cl.snap.ps.clientNum == attacker;

						entityState_t* attackerEntity = findEntity(attacker);

						// Find extra info for means of death.
						std::stringstream modInfo;
						weapon_t probableKillingWeapon = (weapon_t)weaponFromMOD[mod];

						qboolean needMoreInfo = qtrue;
						if (probableKillingWeapon == WP_NONE) { // means something like doom kill
							if (attackerIsFollowed) {
								probableKillingWeapon = (weapon_t)thisDemo.cut.Cl.snap.ps.weapon;
							}
							else if (attackerEntity) {
								probableKillingWeapon = (weapon_t)attackerEntity->weapon;
							}
						}

						int stringOffset = thisDemo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
						const char* info = thisDemo.cut.Cl.gameState.stringData + stringOffset;
						std::string mapname = Info_ValueForKey(info,sizeof(thisDemo.cut.Cl.gameState.stringData)- stringOffset, "mapname");
						const char* playerInfo;
						std::string playername = "WEIRDATTACKER";
						if (attacker >= 0 && attacker < maxClientsThisDemo) {
							stringOffset = thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + attacker];
							playerInfo = thisDemo.cut.Cl.gameState.stringData + stringOffset;
							playername = Info_ValueForKey(playerInfo, sizeof(thisDemo.cut.Cl.gameState.stringData) - stringOffset, "n");
						}
						stringOffset = thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
						playerInfo = thisDemo.cut.Cl.gameState.stringData + stringOffset;
						std::string victimname = Info_ValueForKey(playerInfo, sizeof(thisDemo.cut.Cl.gameState.stringData) - stringOffset, "n");
					}*/
				}
			}


			break;
		case svc_download_general:
			// read block number
			buf = MSG_ReadShort(&oldMsg);
			if (!buf)	//0 block, read file size
				MSG_ReadLong(&oldMsg);
			// read block size
			buf = MSG_ReadShort(&oldMsg);
			// read the data block
			for (; buf > 0; buf--)
				MSG_ReadByte(&oldMsg);
			break;
		case svc_setgame_general:
			{
				static char	newGameDir[MAX_QPATH];
				int i = 0;
				while (i < MAX_QPATH) {
					int next = MSG_ReadByte(&oldMsg);
					if (next) {
						newGameDir[i] = next;
					}
					else {
						break;
					}
					i++;
				}
				newGameDir[i] = 0;
				// But here we stop, and don't do more. If this goes horribly wrong sometime, you might have to go and actually do something with this.
			}
			break;
		case svc_mapchange_general:
			// nothing to parse.
			break;
		}
	}
	int firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
	qboolean hadConfigStringChanges = qfalse;
	// process any new server commands
	for (; thisDemo.cut.Clc.lastExecutedServerCommand <= thisDemo.cut.Clc.serverCommandSequence; thisDemo.cut.Clc.lastExecutedServerCommand++) {
		char* command = thisDemo.cut.Clc.serverCommands[thisDemo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
		
		Cmd_TokenizeString(command);
		char* cmd = Cmd_Argv(0);
		if (cmd[0]) {
			firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
		}
		if (!strcmp(cmd, "cs")) {
			if (!demoCutConfigstringModified(&thisDemo.cut.Cl,demoType)) {
				return qfalse;
			}
			hadConfigStringChanges = qtrue;
		}
		if (!strcmp(cmd, "print")) {
			//Looking for 
			//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."
			// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.

			/*jp::VecNum vec_num;
			jp::RegexMatch rm;
			std::string printText = Cmd_Argv(1);
#if DEBUG
			std::cout << printText << "\n";
#endif

			size_t count = rm.setRegexObject(&defragRecordFinishRegex)                          //set associated Regex object
				.setSubject(&printText)                         //set subject string
				.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
				.match();

			for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
				const char* info = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
				std::string mapname = Info_ValueForKey(info, "mapname");
				std::string playername = vec_num[matchNum][1];
				int minutes = atoi(vec_num[matchNum][2].c_str());
				std::string secondString = vec_num[matchNum][3];
				float seconds = atof(vec_num[matchNum][3].c_str());
				int milliSeconds = (1000.0f * seconds) + 0.5f;
				int pureMilliseconds = milliSeconds % 1000;
				int pureSeconds = milliSeconds / 1000;

				bool isLogged = vec_num[matchNum][4].length() > 0;
				bool isNumberOne = vec_num[matchNum][6].length() > 0;

				//int totalSeconds = minutes * 60 + seconds;
				int totalMilliSeconds = minutes * 60000 + milliSeconds;

				// Find player
				int playerNumber = -1;
				for (int clientNum = 0; clientNum < maxClientsThisDemo; clientNum++) {

					const char* playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + clientNum];
					std::string playerNameCompare = Info_ValueForKey(playerInfo, "n");
					if (playerNameCompare == playername) {
						playerNumber = clientNum;
					}
				}

				bool wasFollowed = false;
				bool wasVisible = false;
				bool wasVisibleOrFollowed = false;
				if (playerNumber != -1) {
					if (playerFirstFollowed[playerNumber] != -1 && playerFirstFollowed[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasFollowed = true;
					}
					if (playerFirstVisible[playerNumber] != -1 && playerFirstVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasVisible = true;
					}
					if (playerFirstFollowedOrVisible[playerNumber] != -1 && playerFirstFollowedOrVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasVisibleOrFollowed = true;
					}
				}
				int runStart = demoCurrentTime - totalMilliSeconds;
			}*/



		}
	}

	if (hadConfigStringChanges) {
	}

/*#if DEBUG
	if (oldSize == 0) {
		goto cutcomplete;
	}
#endif*/


	return qtrue;
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

/*
int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds";
		std::cin.get();
		return 1;
	}
	initializeGameInfos();

	char* demoName = argv[1];
	float bufferTime = atof(argv[2]);

	highlightSearchMode_t searchMode = SEARCH_INTERESTING;
	if (argc > 3) {
		// Searchmode specified
		char* searchModeText = argv[3];
		if (!_stricmp(searchModeText, "myctfreturns")) {
			searchMode = SEARCH_MY_CTF_RETURNS;
		}
		else if (!_stricmp(searchModeText, "ctfreturns")) {
			searchMode = SEARCH_CTF_RETURNS;
		}
		else if (!_stricmp(searchModeText, "top10defrag")) {
			searchMode = SEARCH_TOP10_DEFRAG;
		}
	}

	if (demoHighlightFind(demoName, bufferTime, "highlightExtractionScript.bat", searchMode)) {
		Com_Printf("Highlights successfully found.\n", demoName);
	}
	else {
		Com_Printf("Finding highlights in demo %s has resulted in errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}*/

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif