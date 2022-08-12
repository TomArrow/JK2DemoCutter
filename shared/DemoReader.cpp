#include "demoCut.h"
#include "anims.h"
#include "jkaStuff.h"
#include "DemoReader.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>




// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//



qboolean DemoReader::ConfigstringModified(clientActive_t* clCut) {
	char* old, * s;
	int			i, index;
	char* dup;
	gameState_t	oldGs;
	int			len;
	index = atoi(Cmd_Argv(1));
	int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	if (index < 0 || index >= maxAllowedConfigString) {
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
	for (i = 0; i < maxAllowedConfigString; i++) {
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



void DemoReader::ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType) {
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
			MSG_ReadDeltaEntity(msg, oldstate, newstate, newnum, demoType);
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		else if (oldnum > newnum) {
			MSG_ReadDeltaEntity(msg, &clCut->entityBaselines[newnum], newstate, newnum, demoType);
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		if (newstate->number == MAX_GENTITIES - 1)
			continue;
		clCut->parseEntitiesNum++;
		clCut->parseEntitiesNum &= (MAX_PARSE_ENTITIES - 1);
		newSnap->numEntities++;
	}
}

qboolean DemoReader::PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap) {
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


qboolean DemoReader::ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
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
	MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps,demoType,qfalse);

	// JKA-specific
	if (demoType == DM_26 && newSnap.ps.m_iVehicleNum)
		MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->vps : NULL, &newSnap.vps,demoType, qtrue);


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

	if (firstSnapServerTime == -1) {
		firstSnapServerTime = clCut->snap.serverTime;
	}

	SnapshotInfo snapshotInfo;
	snapshotInfo.serverTime = clCut->snap.serverTime;
	for (int pe = clCut->snap.parseEntitiesNum; pe < clCut->snap.parseEntitiesNum + clCut->snap.numEntities; pe++) {
		entityState_t* thisEntity = &clCut->parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		snapshotInfo.entities[thisEntity->number] = *thisEntity;

		lastMessageWithEntity[thisEntity->number] = clCut->snap.messageNum;

		// See if we can figure out the command time of this entity if it's a player.
		if (thisEntity->number >= 0 && thisEntity->number < MAX_CLIENTS) {
			if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
				snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = thisEntity->pos.trTime;
			}
			else {
				snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = clCut->snap.serverTime; // Otherwise just use servertime. Lame but oh well. Maybe we could do sth better where we try to detect changes in values or such if we truly need to.
			}
		}
	}
	snapshotInfo.snapNum = clCut->snap.messageNum;
	snapshotInfo.playerState = clCut->snap.ps;
	snapshotInfo.playerStateTeleport = PlayerStateIsTeleport(&lastSnap,&clCut->snap);
	snapshotInfo.snapFlagServerCount = (qboolean)((lastSnap.snapFlags ^ clCut->snap.snapFlags) & SNAPFLAG_SERVERCOUNT);
	snapshotInfo.playerCommandOrServerTimes[clCut->snap.ps.clientNum] = lastKnownCommandOrServerTimes[clCut->snap.ps.clientNum] = clCut->snap.ps.commandTime;
	snapshotInfos[clCut->snap.messageNum] = snapshotInfo;

	lastKnownCommandTime = clCut->snap.ps.commandTime;

	lastSnap = clCut->snap;

	return qtrue;
}

#include "zlib/zlib.h"
void DemoReader::ParseRMG(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut) {
	int i;
	clcCut->rmgHeightMapSize = (unsigned short)MSG_ReadShort(msg);
	if (clcCut->rmgHeightMapSize == 0) {
		return;
	}
	z_stream zdata;
	int flatDataSize;
	unsigned char heightmap1[15000];
	// height map
	if (MSG_ReadBits(msg, 1)) {
		memset(&zdata, 0, sizeof(z_stream));
		inflateInit(&zdata/*, Z_SYNC_FLUSH*/);
		MSG_ReadData(msg, heightmap1, clcCut->rmgHeightMapSize);
		zdata.next_in = heightmap1;
		zdata.avail_in = clcCut->rmgHeightMapSize;
		zdata.next_out = (unsigned char*)clcCut->rmgHeightMap;
		zdata.avail_out = MAX_HEIGHTMAP_SIZE;
		inflate(&zdata, Z_SYNC_FLUSH);
		clcCut->rmgHeightMapSize = zdata.total_out;
		inflateEnd(&zdata);
	}
	else {
		MSG_ReadData(msg, (unsigned char*)clcCut->rmgHeightMap, clcCut->rmgHeightMapSize);
	}
	// Flatten map
	flatDataSize = MSG_ReadShort(msg);
	if (MSG_ReadBits(msg, 1)) {
		// Read the flatten map
		memset(&zdata, 0, sizeof(z_stream));
		inflateInit(&zdata/*, Z_SYNC_FLUSH*/);
		MSG_ReadData(msg, heightmap1, flatDataSize);
		zdata.next_in = heightmap1;
		zdata.avail_in = clcCut->rmgHeightMapSize;
		zdata.next_out = (unsigned char*)clcCut->rmgFlattenMap;
		zdata.avail_out = MAX_HEIGHTMAP_SIZE;
		inflate(&zdata, Z_SYNC_FLUSH);
		inflateEnd(&zdata);
	}
	else {
		MSG_ReadData(msg, (unsigned char*)clcCut->rmgFlattenMap, flatDataSize);
	}
	// Seed
	clcCut->rmgSeed = MSG_ReadLong(msg);
	// Automap symbols
	clcCut->rmgAutomapSymbolCount = (unsigned short)MSG_ReadShort(msg);
	for (i = 0; i < clcCut->rmgAutomapSymbolCount; i++) {
		clcCut->rmgAutomapSymbols[i].mType = (int)MSG_ReadByte(msg);
		clcCut->rmgAutomapSymbols[i].mSide = (int)MSG_ReadByte(msg);
		clcCut->rmgAutomapSymbols[i].mOrigin[0] = (float)MSG_ReadLong(msg);
		clcCut->rmgAutomapSymbols[i].mOrigin[1] = (float)MSG_ReadLong(msg);
	}
}


qboolean DemoReader::ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd;
	char* s;

	int svc_EOF_realCMD = demoType == DM_26 ? svc_EOF + 1 : svc_EOF;
	int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;

	clcCut->connectPacketCount = 0;
	Com_Memset(clCut, 0, sizeof(*clCut));
	clcCut->serverCommandSequence = MSG_ReadLong(msg);
	clCut->gameState.dataCount = 1;
	while (1) {
		cmd = MSG_ReadByte(msg);
		//if (cmd == svc_EOF) {
		if (cmd == svc_EOF_realCMD) {
			break;
		}
		if (cmd == svc_configstring) {
			int len, start;
			start = msg->readcount;
			i = MSG_ReadShort(msg);
			if (i < 0 || i >= maxAllowedConfigString) {
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
			MSG_ReadDeltaEntity(msg, &nullstate, es, newnum, demoType);
		}
		else {
			Com_Printf("demoCutParseGameState: bad command byte");
			return qfalse;
		}
	}
	clcCut->clientNum = MSG_ReadLong(msg);
	clcCut->checksumFeed = MSG_ReadLong(msg);

	// RMG stuff (JKA specific)
	if (demoType == DM_26) {
		ParseRMG(msg, clcCut, clCut);
	}

	return qtrue;
}

void DemoReader::ParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
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

int DemoReader::GetEvent(entityState_t* es) {
	//if (lastEvent.find(es->number) == lastEvent.end()) { // this might not be necessary because its auto-initialized to 0 anyway (std::map behavior)
	//	lastEvent[es->number] = 0;
	//}

	// check for event-only entities
	/*if (es->eType > ET_EVENTS) {
		if (cent->previousEvent) {
			return;	// already fired
		}
		// if this is a player event set the entity number of the client entity number
		if (es->eFlags & EF_PLAYER_EVENT) {
			es->number = es->otherEntityNum;
		}

		cent->previousEvent = 1;

		es->event = es->eType - ET_EVENTS;
	}
	else {
		// check for events riding with another entity
		if (es->event == cent->previousEvent) {
			return;
		}
		cent->previousEvent = es->event;
		if ((es->event & ~EV_EVENT_BITS) == 0) {
			return;
		}
	}*/

	if (lastEventTime[es->number] < demoCurrentTime - EVENT_VALID_MSEC) {
		lastEvent[es->number] = 0;
	}

	int eventNumberRaw = es->eType > ET_EVENTS ? es->eType - ET_EVENTS : es->event;
	int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

	lastEventTime[es->number] = demoCurrentTime;
	if (eventNumberRaw == lastEvent[es->number]) {
		return 0;
	}

	lastEvent[es->number] = eventNumberRaw;
	return eventNumber;

}

entityState_t* DemoReader::findEntity(int number) {
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

		if (thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}


qboolean DemoReader::LoadDemo(const char* sourceDemoFile) {
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
	else if (!_stricmp(ext, ".dm_26")) {

		demoType = DM_26;
		ext = ".dm_26";
	}
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	memset(&lastSnap, 0, sizeof(lastSnap));
	memset(basePlayerStates, 0, sizeof(basePlayerStates));

	readGamestate = 0;

	demoStartTime = 0;
	demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	demoCurrentTime = 0;
	lastGameStateChange = 0;
	lastGameStateChangeInDemoTime = 0;
	lastKnownTime = 0;
	lastKnownCommandTime = 0;
	firstSnapServerTime = -1;
	lastKnownCommandOrServerTimes.clear();
	messageOffset = 0;
	lastGottenCommandsTime = 0;
	lastGottenCommandsServerTime = 0;
	lastGottenEventsTime = 0;
	endReached = qfalse;

	snapshotInfos.clear();


	//while (oldSize > 0) ReadMessage();

	ReadMessage(); // Will only read pretty much the first message so we have gamestate and are somewhat "initialized" i guess
}

qboolean DemoReader::CloseDemo() {

	FS_FCloseFile(oldHandle);
	return qtrue;
}
qboolean DemoReader::AnySnapshotParsed() {

	return anySnapshotParsed;
}
qboolean DemoReader::EndReached() {

	return endReached;
}
qboolean DemoReader::EndReachedAtTime(float time) {
	SeekToTime(time);
	return (qboolean)(demoCurrentTime < time);
}
int DemoReader::getCurrentDemoTime() {
	return demoCurrentTime;
}

qboolean DemoReader::SeekToTime(float time) {
	while (demoCurrentTime < time && !endReached) {
		ReadMessage();
	}
	if (demoCurrentTime < time && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToServerTime(int serverTime) {
	while (lastKnownTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime < serverTime && endReached) return qfalse;
	return qtrue;
}
int DemoReader::GetFirstServerTimeAfterServerTime(int serverTime) {
	while (lastKnownTime <= serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime <= serverTime && endReached) return -1;

	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime > serverTime) return it->second.serverTime;
	}
	return -1; // Shouldn't happen really but whatever
}
int DemoReader::GetLastServerTimeBeforeServerTime(int serverTime) {

	if (!SeekToServerTime(serverTime)) return -1;

	int lastServerTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime >= serverTime) return lastServerTime;
		lastServerTime = it->second.serverTime;
	}
	return -1; // Shouldn't happen really but whatever
}
SnapshotInfo* DemoReader::GetSnapshotInfoAtServerTime(int serverTime) {
	if (SeekToServerTime(serverTime)) {
		for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
			if (it->second.serverTime == serverTime) return &it->second;
		}
		return NULL; // Can happen if that particular time's snapshot is missing from the demo.
	}
	else {
		return NULL;
	}

}
qboolean DemoReader::SeekToCommandTime(int serverTime) {
	while (lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandTime < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToPlayerCommandOrServerTime(int clientNum,int serverTime) {
	while (lastKnownCommandOrServerTimes[clientNum] < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToPlayerInPacket(int clientNum) {
	while (lastKnownCommandOrServerTimes[clientNum] <=0 && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] <= 0 && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToAnySnapshotIfNotYet() {
	while (!anySnapshotParsed && !endReached) {
		ReadMessage();
	}
	if (!anySnapshotParsed && endReached) return qfalse;
	return qtrue;
}
playerState_t DemoReader::GetCurrentPlayerState() {
	return thisDemo.cut.Cl.snap.ps;
}

playerState_t DemoReader::GetPlayerFromSnapshot(int clientNum, int snapNum, qboolean detailedPS) {
	SnapshotInfo* snap = &snapshotInfos[snapNum];
	if (snap->playerState.clientNum == clientNum) {
		return snap->playerState;
	}
	else {
		playerState_t retVal;
		Com_Memset(&retVal, 0, sizeof(playerState_t));

		int baseSnap = -1;
		if(detailedPS){ // Try to restore stuff like health/armor, pers_spawn_count etc. Use for main player in demo.
			int pastSnap = -1;
			int futureSnap = -1;
			// Search if there is a past and/or future playerstate.
			// TODO What about future ones not read yet? It might happen minutes later or whatever...
			SeekToServerTime(snap->serverTime+ PLAYERSTATE_FUTURE_SEEK);
			for (auto snapIt = snapshotInfos.begin(); snapIt != snapshotInfos.end(); snapIt++) {
				if (snapIt->first < snapNum && snapIt->second.playerState.clientNum == clientNum) {
					pastSnap = snapIt->first;
				}
				if (snapIt->first > snapNum && snapIt->second.playerState.clientNum == clientNum) {
					futureSnap = snapIt->first;
				}
			}
			baseSnap = pastSnap != -1 ? pastSnap : futureSnap;
		}

		entityState_t* thisEntity = &snap->entities[clientNum];
		// Need to convert the entity.
		CG_EntityStateToPlayerState(thisEntity, &retVal,demoType,qtrue,baseSnap != -1 ? &snapshotInfos[baseSnap].playerState : &basePlayerStates[clientNum]);

		if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
			retVal.commandTime = thisEntity->pos.trTime;
		}
		else {
			retVal.commandTime = snap->serverTime; // Otherwise just use servertime. Lame but oh well. Maybe we could do sth better where we try to detect changes in values or such if we truly need to.
		}

		return retVal;
	}
}

playerState_t DemoReader::GetInterpolatedPlayer(int clientNum, float time, SnapshotInfo** oldSnap, SnapshotInfo** newSnap, qboolean detailedPS, float* translatedTime) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));

	if (oldSnap) *oldSnap = NULL;
	if (newSnap) *newSnap = NULL;

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (translatedTime) {
		*translatedTime = time;
	}

	SeekToPlayerCommandOrServerTime(clientNum,time);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	int lastPastSnap = -1;
	int lastPastSnapCommandTime = -1;
	int firstPacketWithPlayerInIt = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		//if (it->second.serverTime <= time) {
		
		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.
		
		if (firstPacketWithPlayerInIt == -1) firstPacketWithPlayerInIt = it->first;
		
		if (it->second.playerCommandOrServerTimes[clientNum] <= time) {
			lastPastSnap = it->first;
			//lastPastSnapCommandTime = it->second.playerState.commandTime;
			lastPastSnapCommandTime = it->second.playerCommandOrServerTimes[clientNum];
		}
	}
	if (lastPastSnap == -1) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have with the player in it
		if (firstPacketWithPlayerInIt == -1) {
			// Uhm. Ok. Maybe handle this better at some later time but for now we just return that empty playerState.
			// We should probably keep seeking then or sth.
			return retVal;
		}
		else {
			//return GetPlayerFromSnapshot(clientNum,&snapshotInfos[firstPacketWithPlayerInIt]);
			return GetPlayerFromSnapshot(clientNum,firstPacketWithPlayerInIt, detailedPS);
			/*SnapshotInfo* thatSnapshot = &snapshotInfos[firstPacketWithPlayerInIt];
			if (thatSnapshot->playerState.clientNum == clientNum) {
				return thatSnapshot->playerState;
			}
			else {
				// Need to convert the entity.
				CG_EntityStateToPlayerState(&thatSnapshot->entities[clientNum],&retVal);
				return retVal;
			}*/
		}
	}

	// Ok now we wanna make sure we have at least one snap after the last one before "time" that has a different commandTime so we have something to interpolate.
	while (lastPastSnapCommandTime >= lastKnownCommandOrServerTimes[clientNum] && !endReached) {
		ReadMessage();
	}

	// already at end, nothing we can interpolate.
	// Just return the last state we have about this player.
	// TODO actually: give way to return info about whether demo is over and such. So we dont end up with stuck entities in one place, tehy should disappear instead.
	// Although... in case of short visibility interruptions it might actually be better to keep returning interpolated values? (but is that related?)
	if (lastPastSnapCommandTime >= lastKnownCommandOrServerTimes[clientNum] && endReached) {
		if (lastMessageWithEntity.find(clientNum) == lastMessageWithEntity.end()) {
			// We never knew anything about this player at all.
			return retVal;
		}
		else {

			if (oldSnap) *oldSnap = &snapshotInfos[lastMessageWithEntity[clientNum]];
			if (newSnap) *newSnap = &snapshotInfos[lastMessageWithEntity[clientNum]];
			//return GetPlayerFromSnapshot(clientNum, &snapshotInfos[lastMessageWithEntity[clientNum]]);
			return GetPlayerFromSnapshot(clientNum, lastMessageWithEntity[clientNum], detailedPS);
		}
	}

	// Okay now we want to locate the first snap with a different commandtime than lastPastSnap and then interpolate between the two.
	int firstNextSnap = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {

		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.

		if (it->second.playerCommandOrServerTimes[clientNum] > lastPastSnapCommandTime) {
			firstNextSnap = it->first;
			break;
		}
	}

	if (oldSnap) *oldSnap = &snapshotInfos[lastPastSnap];
	if (newSnap) *newSnap = &snapshotInfos[firstNextSnap];

	// Okay now we know the messageNum of before and after. Let's interpolate! How exciting!
	InterpolatePlayer(clientNum,time, &snapshotInfos[lastPastSnap], &snapshotInfos[firstNextSnap], &retVal, detailedPS);

	return retVal;
	
}

void DemoReader::mapAnimsToDM15(playerState_t* ps) {
	if (demoType == DM_26) {

		ps->torsoAnim = jkaAnimMapping[ps->torsoAnim];
		if (ps->torsoFlip) ps->torsoAnim |= ANIM_TOGGLEBIT;
		ps->legsAnim = jkaAnimMapping[ps->legsAnim];
		if (ps->legsFlip) ps->legsAnim |= ANIM_TOGGLEBIT;
		ps->weapon = jkaWeaponMap[ps->weapon];
	}
	if (demoType == DM_16 || demoType == DM_26) {

		//ps->torsoAnim = animMappingTable_1_04_to_1_02[ps->torsoAnim];
		//ps->legsAnim = animMappingTable_1_04_to_1_02[ps->legsAnim];
		ps->torsoAnim = MV_MapAnimation102(ps->torsoAnim);
		ps->legsAnim = MV_MapAnimation102(ps->legsAnim);
	}
}

playerState_t DemoReader::GetInterpolatedPlayerState(float time) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));
	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;
	SeekToCommandTime(time);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".
	
	int lastPastSnap = -1;
	int lastPastSnapCommandTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		//if (it->second.serverTime <= time) {
		if (it->second.playerState.commandTime <= time) {
			lastPastSnap = it->first;
			lastPastSnapCommandTime = it->second.playerState.commandTime;
		}
	}

	if (lastPastSnap == -1) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have.
		retVal = snapshotInfos.begin()->second.playerState;
		mapAnimsToDM15(&retVal);
		return retVal;
	}

	// Ok now we wanna make sure we have at least one snap after the last one before "time" that has a different commandTime so we have something to interpolate.
	while (lastPastSnapCommandTime >= lastKnownCommandTime && !endReached) {
		ReadMessage();
	}

	// already at end, nothing we can interpolate.
	if (lastPastSnapCommandTime >= lastKnownCommandTime && endReached) {
		retVal = thisDemo.cut.Cl.snap.ps;
		mapAnimsToDM15(&retVal);
		return retVal;
	}

	// Okay now we want to locate the first snap with a different commandtime than lastPastSnap and then interpolate between the two.
	int firstNextSnap =-1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.playerState.commandTime > lastPastSnapCommandTime) {
			firstNextSnap = it->first;
			break;
		}
	}

	// Okay now we know the messageNum of before and after. Let's interpolate! How exciting!
	InterpolatePlayerState(time, &snapshotInfos[lastPastSnap], &snapshotInfos[firstNextSnap], &retVal);

	mapAnimsToDM15(&retVal);
	return retVal;
}


void DemoReader::InterpolatePlayerState(float time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS) {
	float			f;
	int				i;
	playerState_t* out;
	//snapshot_t* prev, * next;
	playerState_t* curps = NULL, * nextps = NULL;
	//qboolean		nextPsTeleport = qfalse;
	qboolean		nextPsTeleport = to->playerStateTeleport;
	int currentTime = 0, nextTime = 0, currentServerTime = 0, nextServerTime = 0;

	out = outPS;
	*out = from->playerState;
	//prev = cg.snap;
	//next = cg.nextSnap;

	if (1) {
		//timedPlayerState_t* tps, * nexttps;
		//CG_ComputeCommandSmoothPlayerstates(&tps, &nexttps, &nextPsTeleport);
		curps = &from->playerState;
		currentTime = curps->commandTime;
		//currentTime = from->serverTime;
		currentServerTime = from->serverTime;
		if (to) {
			nextps = &to->playerState;
			nextTime = nextps->commandTime;
			//nextTime = to->serverTime; // Testing. See if we can find the perfect way to do it ... hmm
			nextServerTime = to->serverTime;
		}
	}
	if (!to) {
		return;
	}/*
#ifdef _DEBUG
	if (!(currentTime <= cg.time + (double)cg.timeFraction && nextTime >= cg.time + (double)cg.timeFraction)) {
		Com_Printf("WARNING: Couldn't locate slots with correct time\n");
	}
	if (nextTime - currentTime < 0) {
		Com_Printf("WARNING: nexttps->time - tps->time < 0 (%d, %d)\n", nextTime, currentTime);
	}
#endif
	if (currentTime != nextTime) {
		f = ((double)cg.timeFraction + (cg.time - currentTime)) / (nextTime - currentTime);
#ifdef _DEBUG
		if (f > 1) {
			Com_Printf("EXTRAPOLATING (f=%f)\n", f);
		}
		else if (f < 0) {
			Com_Printf("GOING BACKWARDS (f=%f)\n", f);
		}
#endif
	}
	else {
		f = 0;
	}
	*/
	f = ((double)time - (double)currentTime) / ((double)nextTime - (double)currentTime);
	*out = *curps;
	//out->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];


	// if the next frame is a teleport, we can't lerp to it
	if (nextPsTeleport) { // TODO make that actually matter
		return;
	}

	/*if ( !next || next->serverTime <= prev->serverTime ) {
		return;
	}*/

	i = nextps->bobCycle;
	if (i < curps->bobCycle) {
		i += 256;		// handle wraparound
	}
	out->bobCycle = curps->bobCycle + f * (i - curps->bobCycle);

	for (i = 0; i < 3; i++) {
		out->origin[i] = curps->origin[i] + f * (nextps->origin[i] - curps->origin[i]);
		//if (!grabAngles) {
			out->viewangles[i] = LerpAngle(
				curps->viewangles[i], nextps->viewangles[i], f);
		//}
		out->velocity[i] = curps->velocity[i] +
			f * (nextps->velocity[i] - curps->velocity[i]);
	}

	// requires commandSmooth 2, since it needs entity state history of the mover
	/*if (cg_commandSmooth.integer > 1) {
		// adjust for the movement of the groundentity
		// step 1: remove the delta introduced by cur->next origin interpolation
		int curTime = currentServerTime + (int)(f * (nextServerTime - currentServerTime));
		float curTimeFraction = (f * (nextServerTime - currentServerTime));
		curTimeFraction -= (long)curTimeFraction;
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, curTime, curTimeFraction, currentServerTime, 0, out->origin);
		// step 2: mover state should now be what it was at currentServerTime, now redo calculation of mover effect to cg.time
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, currentServerTime, 0, cg.time, cg.timeFraction, out->origin);
	}*/ // TODO Fix this?

	//curps->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];
	//BG_PlayerStateToEntityState(curps, &cg_entities[curps->clientNum].currentState, qfalse);
	//BG_PlayerStateToEntityState(nextps, &cg_entities[nextps->clientNum].nextState, qfalse);
	//cg.playerInterpolation = f;
	
	/*if (cg.timeFraction >= cg.nextSnap->serverTime - cg.time) {
		cg.physicsTime = cg.nextSnap->serverTime;
	}
	else {
		cg.physicsTime = cg.snap->serverTime;
	}*/ // TODO Relevant?


	// TODO: What about this? not in jamme
	//cg.predictedTimeFrac = f * (next->ps.commandTime - prev->ps.commandTime);
	//cg.predictedTimeFrac = f * (nextps->commandTime - curps->commandTime);
}

void DemoReader::InterpolatePlayer(int clientNum, float time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS, qboolean detailedPS) {
	float			f;
	int				i;
	playerState_t* out;
	//snapshot_t* prev, * next;
	playerState_t* curps = NULL, * nextps = NULL;
	//qboolean		nextPsTeleport = qfalse;
	//qboolean		nextPsTeleport = to->playerStateTeleport;
	qboolean		nextPsTeleport = qfalse;
	int currentTime = 0, nextTime = 0, currentServerTime = 0, nextServerTime = 0;

	out = outPS;
	*out = from->playerState;
	//prev = cg.snap;
	//next = cg.nextSnap;

	playerState_t inPsConverted, outPsConverted;

	if (1) {


		//inPsConverted = GetPlayerFromSnapshot(clientNum,from);
		inPsConverted = GetPlayerFromSnapshot(clientNum,from->snapNum, detailedPS);
		curps = &inPsConverted;
		currentTime = curps->commandTime;
		//currentTime = from->serverTime;
		currentServerTime = from->serverTime;
		if (to) {
			outPsConverted = GetPlayerFromSnapshot(clientNum, to->snapNum, detailedPS);
			nextps = &outPsConverted;
			nextTime = nextps->commandTime;
			//nextTime = to->serverTime; // Testing. See if we can find the perfect way to do it ... hmm
			nextServerTime = to->serverTime;
		}
	}
	if (!to) {
		return;
	}
	
	nextPsTeleport = (qboolean)( to->snapFlagServerCount || ((curps->eFlags ^ nextps->eFlags) & EF_TELEPORT_BIT));
	
	/*
#ifdef _DEBUG
	if (!(currentTime <= cg.time + (double)cg.timeFraction && nextTime >= cg.time + (double)cg.timeFraction)) {
		Com_Printf("WARNING: Couldn't locate slots with correct time\n");
	}
	if (nextTime - currentTime < 0) {
		Com_Printf("WARNING: nexttps->time - tps->time < 0 (%d, %d)\n", nextTime, currentTime);
	}
#endif
	if (currentTime != nextTime) {
		f = ((double)cg.timeFraction + (cg.time - currentTime)) / (nextTime - currentTime);
#ifdef _DEBUG
		if (f > 1) {
			Com_Printf("EXTRAPOLATING (f=%f)\n", f);
		}
		else if (f < 0) {
			Com_Printf("GOING BACKWARDS (f=%f)\n", f);
		}
#endif
	}
	else {
		f = 0;
	}
	*/
	f = ((double)time - (double)currentTime) / ((double)nextTime - (double)currentTime);
	*out = *curps;
	//out->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];


	// if the next frame is a teleport, we can't lerp to it
	if (nextPsTeleport) { // TODO make that actually matter
		return;
	}

	/*if ( !next || next->serverTime <= prev->serverTime ) {
		return;
	}*/

	i = nextps->bobCycle;
	if (i < curps->bobCycle) {
		i += 256;		// handle wraparound
	}
	out->bobCycle = curps->bobCycle + f * (i - curps->bobCycle);

	for (i = 0; i < 3; i++) {
		out->origin[i] = curps->origin[i] + f * (nextps->origin[i] - curps->origin[i]);
		//if (!grabAngles) {
			out->viewangles[i] = LerpAngle(
				curps->viewangles[i], nextps->viewangles[i], f);
		//}
		out->velocity[i] = curps->velocity[i] +
			f * (nextps->velocity[i] - curps->velocity[i]);
	}

	// requires commandSmooth 2, since it needs entity state history of the mover
	/*if (cg_commandSmooth.integer > 1) {
		// adjust for the movement of the groundentity
		// step 1: remove the delta introduced by cur->next origin interpolation
		int curTime = currentServerTime + (int)(f * (nextServerTime - currentServerTime));
		float curTimeFraction = (f * (nextServerTime - currentServerTime));
		curTimeFraction -= (long)curTimeFraction;
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, curTime, curTimeFraction, currentServerTime, 0, out->origin);
		// step 2: mover state should now be what it was at currentServerTime, now redo calculation of mover effect to cg.time
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, currentServerTime, 0, cg.time, cg.timeFraction, out->origin);
	}*/ // TODO Fix this?

	//curps->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];
	//BG_PlayerStateToEntityState(curps, &cg_entities[curps->clientNum].currentState, qfalse);
	//BG_PlayerStateToEntityState(nextps, &cg_entities[nextps->clientNum].nextState, qfalse);
	//cg.playerInterpolation = f;
	
	/*if (cg.timeFraction >= cg.nextSnap->serverTime - cg.time) {
		cg.physicsTime = cg.nextSnap->serverTime;
	}
	else {
		cg.physicsTime = cg.snap->serverTime;
	}*/ // TODO Relevant?


	// TODO: What about this? not in jamme
	//cg.predictedTimeFrac = f * (next->ps.commandTime - prev->ps.commandTime);
	//cg.predictedTimeFrac = f * (nextps->commandTime - curps->commandTime);
}





std::map<int,entityState_t> DemoReader::GetCurrentEntities() {
	std::map<int, entityState_t> retVal;
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
		entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		retVal[thisEntity->number] = *thisEntity;
	}
	return retVal;
}

std::map<int,entityState_t> DemoReader::GetEntitiesAtTime(float time, float *translatedTime) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (translatedTime) {
		*translatedTime = time;
	}

	if (endReached && !anySnapshotParsed) return std::map<int, entityState_t>(); // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	int lastPastSnap = -1;
	int lastPastSnapServerTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime <= time) {
			lastPastSnap = it->first;
			lastPastSnapServerTime = it->second.serverTime;
		}
	}

	
	return snapshotInfos[lastPastSnap].entities;
}

std::map<int,entityState_t> DemoReader::GetEntitiesAtPreciseTime(int time, qboolean includingPS) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (endReached && !anySnapshotParsed) return std::map<int, entityState_t>(); // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime == time) {
			if (includingPS) {

				std::map<int, entityState_t> retVal = it->second.entities;
				entityState_t psEnt;
				Com_Memset(&psEnt, 0, sizeof(psEnt));
				BG_PlayerStateToEntityState(&it->second.playerState, &psEnt,qfalse);
				retVal[it->second.playerState.clientNum] = psEnt;
				return retVal;
			}
			else {
				return it->second.entities;
			}
		}
	}
	return std::map<int, entityState_t>();
}

std::vector<std::string> DemoReader::GetNewCommands(float time) {
	std::vector<std::string> retVal;
	SeekToTime(time);
	for (int i = 0; i < readCommands.size(); i++) {
		if (readCommands[i].demoTime <= time && readCommands[i].demoTime > lastGottenCommandsTime) {
			retVal.push_back(readCommands[i].command);
		}
	}
	lastGottenCommandsTime = time;
	return retVal;
}
// Don't combine this with GetNewCommands, they keep track in different ways. Pick one or the other.
std::vector<std::string> DemoReader::GetNewCommandsAtServerTime(float serverTime) {
	std::vector<std::string> retVal;
	SeekToServerTime(serverTime);
	for (int i = 0; i < readCommands.size(); i++) {
		if (readCommands[i].serverTime <= serverTime && readCommands[i].serverTime > lastGottenCommandsServerTime) {
			retVal.push_back(readCommands[i].command);
		}
	}
	lastGottenCommandsServerTime = serverTime;
	return retVal;
}
std::vector<Event> DemoReader::GetNewEvents(float time) {
	std::vector<Event> retVal;
	SeekToTime(time);
	for (int i = 0; i < readEvents.size(); i++) {
		if (readEvents[i].demoTime <= time && readEvents[i].demoTime > lastGottenEventsTime) {
#ifdef DEBUG
			readEvents[i].countGiven++;
#endif
			retVal.push_back(readEvents[i]);
		}
	}
	lastGottenEventsTime = time;
	return retVal;
}

int DemoReader::GetFirstSnapServerTime() {
	SeekToAnySnapshotIfNotYet();
	return firstSnapServerTime;
}

clSnapshot_t DemoReader::GetCurrentSnap() {
	return thisDemo.cut.Cl.snap;
}

const char* DemoReader::GetPlayerConfigString(int playerNum,int* maxLength) {
	int configStringBaseIndex = demoType == DM_26 ? CS_PLAYERS_JKA : CS_PLAYERS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + playerNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReader::GetSoundConfigString(int soundNum,int* maxLength) {
	int configStringBaseIndex = demoType == DM_26 ? CS_SOUNDS_JKA : CS_SOUNDS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + soundNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReader::GetModelConfigString(int modelNum,int* maxLength) {
	int configStringBaseIndex = demoType == DM_26 ? CS_MODELS_JKA : CS_MODELS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + modelNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReader::GetConfigString(int configStringNum, int* maxLength) {
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}

void DemoReader::generateBasePlayerStates() { // TODO expand this to be time-relevant by also reading tinfo and filling health and armor and such
	int maxLength;
	for (int i = 0; i < MAX_CLIENTS; i++) {
		const char* playerCS = GetPlayerConfigString(i,&maxLength);
		int team = atoi(Info_ValueForKey(playerCS, maxLength, "t"));
		basePlayerStates[i].persistant[PERS_TEAM] = team;
		basePlayerStates[i].stats[STAT_HEALTH] = 100;
		basePlayerStates[i].stats[STAT_MAX_HEALTH] = 100;
	}
}

qboolean DemoReader::ReadMessage() {
	if (endReached) return qfalse;
	qboolean realReadResult = qfalse;
	if (demoType == DM_26) {
		realReadResult = ReadMessageReal<DM_26>();
	}
	else {
		realReadResult = ReadMessageReal<DM_15>();
	}
	if (!realReadResult) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
}

template <demoType_t D>
qboolean DemoReader::ReadMessageReal() {

	constexpr int svc_mapchange_realCMD = D == DM_26 ? svc_mapchange+1: svc_mapchange;
	constexpr int svc_EOF_realCMD = D == DM_26 ? svc_EOF+1 : svc_EOF;
	constexpr int svc_setgame_realCMD = D == DM_26 ? 8 : 100; // A non-JKA demo won't have this, let's just set something ridiculously high


readNext:
	int				buf;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];

	MSG_Init(&oldMsg, oldData, sizeof(oldData));
	/* Read the sequence number */
	if (FS_Read(&thisDemo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4) {
		Com_Printf("[NOTE] Demo cutter, reading sequence number failed.\n");
		return qfalse;
	}
	thisDemo.cut.Clc.serverMessageSequence = LittleLong(thisDemo.cut.Clc.serverMessageSequence);
	oldSize -= 4;
	/* Read the message size */
	if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4) {
		Com_Printf("[NOTE] Demo cutter, reading message size failed.\n");
		return qfalse;
	}
	oldMsg.cursize = LittleLong(oldMsg.cursize);
	oldSize -= 4;
	/* Negative size signals end of demo */
	if (oldMsg.cursize < 0) {
		Com_Printf("[NOTE] Demo cutter, message cursize < 0.\n");
		return qfalse;
	}
	if (oldMsg.cursize > oldMsg.maxsize) {
		Com_Printf("[NOTE] Demo cutter, message size above maxsize.\n");
		return qfalse;
	}
	/* Read the actual message */
	if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize) {
		Com_Printf("[NOTE] Demo cutter, reading actual message failed.\n");
		return qfalse;
	}
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
		if (cmd == svc_EOF_realCMD) {
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
				Com_Printf("[NOTE] Demo cutter, parsing gamestate failed.\n");
				return qfalse;
			}
			generateBasePlayerStates();
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
				Com_Printf("[NOTE] Demo cutter, parsing snapshot failed.\n");
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


			// Fire events
			for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

				entityState_t* thisEs = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
				int eventNumber = GetEvent(thisEs);

				if (eventNumber) {

					Event thisEvent;
#ifdef DEBUG
					thisEvent.countGiven = 0;
#endif
					thisEvent.demoTime = demoCurrentTime;
					thisEvent.theEvent = *thisEs;
					thisEvent.eventNumber = eventNumber;
					if (demoType == DM_26) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = jkaEventToJk2Map[thisEvent.eventNumber];
						if (thisEvent.theEvent.eType > ET_EVENTS_JKA) {
							thisEvent.theEvent.eType = jkaEventToJk2Map[thisEvent.theEvent.eType-ET_EVENTS_JKA] + ET_EVENTS_JKA;
						}
						else {
							thisEvent.theEvent.event = MapJKAEventJK2(thisEvent.theEvent.event);
						}
					}
					readEvents.push_back(thisEvent);
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


			// Find out which players are visible / followed
			// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
			/*lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
			for (int p = 0; p < MAX_CLIENTS; p++) {
				// Go through parseenttities of last snap to see if client is in it
				bool clientIsInSnapshot = false;
				bool clientVisibleOrFollowed = false;
				for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
					entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					if (thisEntity->number == p) {
						clientIsInSnapshot = true;

						if (thisEntity->powerups & (1 << PW_REDFLAG)) {
							lastKnownRedFlagCarrier = thisEntity->number;
						}
						else if (thisEntity->powerups & (1 << PW_BLUEFLAG)) {
							lastKnownBlueFlagCarrier = thisEntity->number;
						}
					}
				}
				if (clientIsInSnapshot) {
					clientVisibleOrFollowed = true;
					if (playerFirstVisible[p] == -1) {
						playerFirstVisible[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstVisible[p] = -1;
				}
				if (thisDemo.cut.Cl.snap.ps.clientNum == p) {
					clientVisibleOrFollowed = true;
					if (playerFirstFollowed[p] == -1) {
						playerFirstFollowed[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstFollowed[p] = -1;
				}
				if (clientVisibleOrFollowed) {
					if (playerFirstFollowedOrVisible[p] == -1) {
						playerFirstFollowedOrVisible[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstFollowedOrVisible[p] = -1;
				}
			}*/

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
		case svc_setgame_realCMD:
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
		case svc_mapchange_realCMD:
			// nothing to parse.
			break;
		}
	}
	int firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
	qboolean hadConfigStringChanges = qfalse;
	// process any new server commands
	for (; thisDemo.cut.Clc.lastExecutedServerCommand <= thisDemo.cut.Clc.serverCommandSequence; thisDemo.cut.Clc.lastExecutedServerCommand++) {
		char* command = thisDemo.cut.Clc.serverCommands[thisDemo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
		Command readCommand;
		readCommand.command = command;
		readCommand.demoTime = demoCurrentTime;
		readCommand.serverTime = thisDemo.cut.Cl.snap.serverTime;
		readCommands.push_back(readCommand);
		Cmd_TokenizeString(command);
		char* cmd = Cmd_Argv(0);
		if (cmd[0]) {
			firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
		}
		if (!strcmp(cmd, "cs")) {
			if (!ConfigstringModified(&thisDemo.cut.Cl)) {
				Com_Printf("[NOTE] Demo cutter, configstring parsing failed.\n");
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
		generateBasePlayerStates();
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