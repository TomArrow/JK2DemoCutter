#include "demoCut.h"
#include "DemoReaderLight.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>




// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//



qboolean DemoReaderLight::ConfigstringModified(clientActive_t* clCut) {
	char* old, * s;
	int			i, index;
	char* dup;
	gameState_t	oldGs;
	int			len;
	index = atoi(Cmd_Argv(1));
	if (index < 0 || index >= MAX_CONFIGSTRINGS) {
		Com_Printf("demoCutConfigstringModified: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = Cmd_ArgsFrom(2);
	old = clCut->gameState.stringData + clCut->gameState.stringOffsets[index];
	if (!strcmp(old, s)) {
		return qtrue; // unchanged
	}
	// build the new gameState_t
	oldGs = clCut->gameState;
	Com_Memset(&clCut->gameState, 0, sizeof(clCut->gameState));
	// leave the first 0 for uninitialized strings
	clCut->gameState.dataCount = 1;
	for (i = 0; i < MAX_CONFIGSTRINGS; i++) {
		if (i == index) {
			dup = s;
		}
		else {
			dup = oldGs.stringData + oldGs.stringOffsets[i];
		}
		if (!dup[0]) {
			continue; // leave with the default empty string
		}
		len = strlen(dup);
		if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
			Com_Printf("MAX_GAMESTATE_CHARS exceeded");
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}



void DemoReaderLight::ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType) {
	/* The beast that is entity parsing */
	int			newnum;
	entityState_t* oldstate, * newstate;
	int			oldindex = 0;
	int			oldnum;
	newSnap->parseEntitiesNum = clCut->parseEntitiesNum;
	newSnap->numEntities = 0;
	newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
	while (1) {
		// read the entity index number
		if (oldSnap && oldindex < oldSnap->numEntities) {
			oldstate = &clCut->parseEntities[(oldSnap->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			oldnum = oldstate->number;
		}
		else {
			oldstate = 0;
			oldnum = 99999;
		}
		newstate = &clCut->parseEntities[clCut->parseEntitiesNum];
		if (!oldstate && (newnum == (MAX_GENTITIES - 1))) {
			break;
		}
		else if (oldnum < newnum) {
			*newstate = *oldstate;
			oldindex++;
		}
		else if (oldnum == newnum) {
			oldindex++;
			MSG_ReadDeltaEntity(msg, oldstate, newstate, newnum, (qboolean)(demoType == DM_15));
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		else if (oldnum > newnum) {
			MSG_ReadDeltaEntity(msg, &clCut->entityBaselines[newnum], newstate, newnum, (qboolean)(demoType == DM_15));
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		if (newstate->number == MAX_GENTITIES - 1)
			continue;
		clCut->parseEntitiesNum++;
		clCut->parseEntitiesNum &= (MAX_PARSE_ENTITIES - 1);
		newSnap->numEntities++;
	}
}

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


qboolean DemoReaderLight::ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
	int len;
	clSnapshot_t* oldSnap;
	clSnapshot_t newSnap;
	int deltaNum;
	int oldMessageNum;
	int i, packetNum;
	Com_Memset(&newSnap, 0, sizeof(newSnap));
	newSnap.serverCommandNum = clcCut->serverCommandSequence;
	newSnap.serverTime = MSG_ReadLong(msg);
	//cl_paused->modified = qfalse;
	newSnap.messageNum = clcCut->serverMessageSequence;
	deltaNum = MSG_ReadByte(msg);
	newSnap.snapFlags = MSG_ReadByte(msg);
	if (!deltaNum) {
		newSnap.deltaNum = -1;
		newSnap.valid = qtrue;		// uncompressed frame
		oldSnap = NULL;
	}
	else {
		newSnap.deltaNum = newSnap.messageNum - deltaNum;
		oldSnap = &clCut->snapshots[newSnap.deltaNum & PACKET_MASK];
		if (!oldSnap->valid) {
			// should never happen
			Com_Printf("Delta from invalid frame (not supposed to happen!).\n");
		}
		else if (oldSnap->messageNum != newSnap.deltaNum) {
			// The frame that the server did the delta from
			// is too old, so we can't reconstruct it properly.
			Com_Printf("Delta frame too old.\n");
		}
		else if (clCut->parseEntitiesNum - oldSnap->parseEntitiesNum > MAX_PARSE_ENTITIES - 128) {
			Com_DPrintf("Delta parseEntitiesNum too old.\n");
		}
		else {
			newSnap.valid = qtrue;	// valid delta parse
		}
	}
	// read areamask
	len = MSG_ReadByte(msg);
	//if (len > sizeof(newSnap.areamask)) {
	//	Com_Printf("demoCutParseSnapshot: Invalid size %d for areamask", len);
	//	return qfalse;
	//}
	MSG_ReadData(msg, &newSnap.areamask, len);
	// read playerinfo
	MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps, (qboolean)(demoType == DM_15));
	// read packet entities
	ParsePacketEntities(msg, oldSnap, &newSnap, clCut, demoType);
	// if not valid, dump the entire thing now that it has
	// been properly read
	if (!newSnap.valid) {
		return qtrue;
	}
	// clear the valid flags of any snapshots between the last
	// received and this one, so if there was a dropped packet
	// it won't look like something valid to delta from next
	// time we wrap around in the buffer
	oldMessageNum = clCut->snap.messageNum + 1;
	if (newSnap.messageNum - oldMessageNum >= PACKET_BACKUP) {
		oldMessageNum = newSnap.messageNum - (PACKET_BACKUP - 1);
	}
	for (; oldMessageNum < newSnap.messageNum; oldMessageNum++) {
		clCut->snapshots[oldMessageNum & PACKET_MASK].valid = qfalse;
	}
	// copy to the current good spot
	clCut->snap = newSnap;
	clCut->snap.ping = 999;
	// calculate ping time
	for (i = 0; i < PACKET_BACKUP; i++) {
		packetNum = (clcCut->netchan.outgoingSequence - 1 - i) & PACKET_MASK;
		if (clCut->snap.ps.commandTime >= clCut->outPackets[packetNum].p_serverTime) {
			//clCut->snap.ping = cls.realtime - clCut->outPackets[packetNum].p_realtime;
			// Dont see how this is needed?
			break;
		}
	}
	// save the frame off in the backup array for later delta comparisons
	clCut->snapshots[clCut->snap.messageNum & PACKET_MASK] = clCut->snap;
	clCut->newSnapshots = qtrue;

	for (int pe = clCut->snap.parseEntitiesNum; pe < clCut->snap.parseEntitiesNum + clCut->snap.numEntities; pe++) {
		entityState_t* thisEntity = &clCut->parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

		// See if we can figure out the command time of this entity if it's a player.
		if (thisEntity->number >= 0 && thisEntity->number < MAX_CLIENTS) {
			int commandOrServerTimeHere = 0;
			if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
				commandOrServerTimeHere = thisEntity->pos.trTime;
			}
			else {
				commandOrServerTimeHere = clCut->snap.serverTime; // Just use servertime. Will mean ping 0, but best we can do.
			}
			int pingHere = clCut->snap.serverTime - commandOrServerTimeHere;
			pingValues[thisEntity->number].push_back(pingHere);
		}
	}
	int psPingHere = clCut->snap.serverTime - clCut->snap.ps.commandTime;
	pingValues[clCut->snap.ps.clientNum].push_back(psPingHere);

	lastKnownCommandTime = clCut->snap.ps.commandTime;

	lastSnap = clCut->snap;

	return qtrue;
}

qboolean DemoReaderLight::ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd;
	char* s;
	clcCut->connectPacketCount = 0;
	Com_Memset(clCut, 0, sizeof(*clCut));
	clcCut->serverCommandSequence = MSG_ReadLong(msg);
	clCut->gameState.dataCount = 1;
	while (1) {
		cmd = MSG_ReadByte(msg);
		if (cmd == svc_EOF) {
			break;
		}
		if (cmd == svc_configstring) {
			int len, start;
			start = msg->readcount;
			i = MSG_ReadShort(msg);
			if (i < 0 || i >= MAX_CONFIGSTRINGS) {
				Com_Printf("configstring > MAX_CONFIGSTRINGS");
				return qfalse;
			}
			s = MSG_ReadBigString(msg);
			len = strlen(s);
			if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
				Com_Printf("MAX_GAMESTATE_CHARS exceeded");
				return qfalse;
			}
			// append it to the gameState string buffer
			clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
			Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, s, len + 1);
			clCut->gameState.dataCount += len + 1;
		}
		else if (cmd == svc_baseline) {
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
			if (newnum < 0 || newnum >= MAX_GENTITIES) {
				Com_Printf("Baseline number out of range: %i", newnum);
				return qfalse;
			}
			Com_Memset(&nullstate, 0, sizeof(nullstate));
			es = &clCut->entityBaselines[newnum];
			MSG_ReadDeltaEntity(msg, &nullstate, es, newnum, (qboolean)(demoType == DM_15));
		}
		else {
			Com_Printf("demoCutParseGameState: bad command byte");
			return qfalse;
		}
	}
	clcCut->clientNum = MSG_ReadLong(msg);
	clcCut->checksumFeed = MSG_ReadLong(msg);
	return qtrue;
}

void DemoReaderLight::ParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
	int index;
	int seq = MSG_ReadLong(msg);
	char* s = MSG_ReadString(msg);
	if (clcCut->serverCommandSequence >= seq) {
		return;
	}
	clcCut->serverCommandSequence = seq;
	index = seq & (MAX_RELIABLE_COMMANDS - 1);
	Q_strncpyz(clcCut->serverCommands[index], MAX_STRING_CHARS, s, sizeof(clcCut->serverCommands[index]));
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
	char* ext;
	oldHandle = 0; // TODO if already loaded, gracefully close

	strncpy_s(oldName, sizeof(oldName), sourceDemoFile, strlen(sourceDemoFile) - 6);
	ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	if (!*ext) {
		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	memset(&lastSnap, 0, sizeof(lastSnap));
	memset(&playerSeen, 0, sizeof(playerSeen));

	readGamestate = 0;

	demoStartTime = 0;
	demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	demoCurrentTime = 0;
	lastGameStateChange = 0;
	lastGameStateChangeInDemoTime = 0;
	lastKnownTime = 0;
	lastKnownCommandTime = 0;
	messageOffset = 0;
	lastGottenCommandsTime = 0;
	lastGottenEventsTime = 0;

	for (int i = 0; i < MAX_CLIENTS; i++) {
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
	while (lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandTime < serverTime && endReached) return qfalse;
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

	for (int i = 0; i < MAX_CLIENTS; i++) {
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

void DemoReaderLight::FreePingData() {
	for (int i = 0; i < MAX_CLIENTS; i++) {
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
	if (!ReadMessageReal()) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
}
qboolean DemoReaderLight::ReadMessageReal() {

readNext:
	int				buf;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];

	MSG_Init(&oldMsg, oldData, sizeof(oldData));
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
	if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize)
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
		if (cmd == svc_EOF) {
			break;
		}
		//I'm not sure what this does or why it does it...
		// skip all the gamestates until we reach needed
		if (readGamestate < thisDemo.currentNum) {
			//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
			if (cmd == svc_gamestate) {
				readGamestate++;
			}
			goto readNext;
		}

		// other commands
		switch (cmd) {
		default:
			Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
			return qfalse;
		case svc_nop:
			break;
		case svc_serverCommand:
			ParseCommandString(&oldMsg, &thisDemo.cut.Clc);
			break;
		case svc_gamestate:
			lastGameStateChange = thisDemo.cut.Cl.snap.serverTime;
			lastGameStateChangeInDemoTime = demoCurrentTime;
			//if (readGamestate > demo.currentNum) {
			//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
			//	goto cutcomplete;
			//}
			if (!ParseGamestate(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, demoType)) {
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
		case svc_snapshot:
			if (!ParseSnapshot(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, demoType)) {
				return qfalse;
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
				if (thisEs->number >= 0 && thisEs->number < MAX_CLIENTS) {
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
						if (target < 0 || target >= MAX_CLIENTS) {
							std::cout << "CG_Obituary: target out of range. This should never happen really.";
						}

						if (attacker < 0 || attacker >= MAX_CLIENTS) {
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
						if (attacker >= 0 && attacker < MAX_CLIENTS) {
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
		case svc_download:
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
		case svc_mapchange:
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
			if (!ConfigstringModified(&thisDemo.cut.Cl)) {
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
				for (int clientNum = 0; clientNum < MAX_CLIENTS; clientNum++) {

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