#include "demoCut.h"
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

	SnapshotInfo snapshotInfo;
	snapshotInfo.serverTime = clCut->snap.serverTime;
	for (int pe = clCut->snap.parseEntitiesNum; pe < clCut->snap.parseEntitiesNum + clCut->snap.numEntities; pe++) {
		entityState_t* thisEntity = &clCut->parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		snapshotInfo.entities[thisEntity->number] = *thisEntity;
	}
	snapshotInfo.playerState = clCut->snap.ps;
	snapshotInfo.playerStateTeleport = PlayerStateIsTeleport(&lastSnap,&clCut->snap);
	snapshotInfos[clCut->snap.messageNum] = snapshotInfo;

	lastKnownCommandTime = clCut->snap.ps.commandTime;

	lastSnap = clCut->snap;

	return qtrue;
}

qboolean DemoReader::ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
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

void DemoReader::ParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
	int index;
	int seq = MSG_ReadLong(msg);
	char* s = MSG_ReadString(msg);
	if (clcCut->serverCommandSequence >= seq) {
		return;
	}
	clcCut->serverCommandSequence = seq;
	index = seq & (MAX_RELIABLE_COMMANDS - 1);
	Q_strncpyz(clcCut->serverCommands[index], s, sizeof(clcCut->serverCommands[index]));
}
#ifdef RELDEBUG
//#pragma optimize("", off)
#endif

int DemoReader::GetEvent(entityState_t* es) {
	if (lastEvent.find(es->number) == lastEvent.end()) {
		lastEvent[es->number] = 0;
	}

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

	int eventNumberRaw = es->eType > ET_EVENTS ? es->eType - ET_EVENTS : es->event;
	int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

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
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	memset(&lastSnap, 0, sizeof(lastSnap));

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

qboolean DemoReader::SeekToTime(int time) {
	while (demoCurrentTime < time && !endReached) {
		ReadMessage();
	}
	if (demoCurrentTime < time && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToCommandTime(int serverTime) {
	while (lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandTime < serverTime && endReached) return qfalse;
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
		return snapshotInfos.begin()->second.playerState;
	}

	// Ok now we wanna make sure we have at least one snap after the last one before "time" that has a different commandTime so we have something to interpolate.
	while (lastPastSnapCommandTime >= lastKnownCommandTime && !endReached) {
		ReadMessage();
	}

	// already at end, nothing we can interpolate.
	if (lastPastSnapCommandTime >= lastKnownCommandTime && endReached) {
		return thisDemo.cut.Cl.snap.ps;
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





std::map<int,entityState_t> DemoReader::GetCurrentEntities() {
	std::map<int, entityState_t> retVal;
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
		entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		retVal[thisEntity->number] = *thisEntity;
	}
	return retVal;
}

std::vector<std::string> DemoReader::GetNewCommands(float time) {
	std::vector<std::string> retVal;
	SeekToTime(time);
	for (int i = 0; i < readCommands.size(); i++) {
		if (readCommands[i].demoTime < time && readCommands[i].demoTime >= lastGottenCommandsTime) {
			retVal.push_back(readCommands[i].command);
		}
	}
	lastGottenCommandsTime = time;
	return retVal;
}
std::vector<Event> DemoReader::GetNewEvents(float time) {
	std::vector<Event> retVal;
	SeekToTime(time);
	for (int i = 0; i < readEvents.size(); i++) {
		if (readEvents[i].demoTime < time && readEvents[i].demoTime >= lastGottenEventsTime) {
			retVal.push_back(readEvents[i]);
		}
	}
	lastGottenEventsTime = time;
	return retVal;
}

clSnapshot_t DemoReader::GetCurrentSnap() {
	return thisDemo.cut.Cl.snap;
}

const char* DemoReader::GetPlayerConfigString(int playerNum) {
	return thisDemo.cut.Cl.gameState.stringData + thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + playerNum];
}
const char* DemoReader::GetConfigString(int configStringNum) {
	return thisDemo.cut.Cl.gameState.stringData + thisDemo.cut.Cl.gameState.stringOffsets[ configStringNum];
}

qboolean DemoReader::ReadMessage() {
	if (endReached) return qfalse;
	if (!ReadMessageReal()) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
}
qboolean DemoReader::ReadMessageReal() {

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


			// Fire events
			for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

				entityState_t* thisEs = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
				int eventNumber = GetEvent(thisEs);

				if (eventNumber) {

					Event thisEvent;
					thisEvent.demoTime = demoCurrentTime;
					thisEvent.theEvent = *thisEs;
					thisEvent.eventNumber = eventNumber;
					readEvents.push_back(thisEvent);

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
							/*if (targetEntity->powerups & (1 << PW_REDFLAG) || targetEntity->powerups & (1 << PW_BLUEFLAG)) {
								// If the victim isn't visible, his entity won't be available, thus this won't be set
								// But we're trying to find interesting moments, so stuff that's not even visible is not that interesting to us
								victimIsFlagCarrier = true;
							}*/
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

						const char* info = thisDemo.cut.Cl.gameState.stringData + thisDemo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
						std::string mapname = Info_ValueForKey(info, "mapname");
						const char* playerInfo = thisDemo.cut.Cl.gameState.stringData + thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + attacker];
						std::string playername = Info_ValueForKey(playerInfo, "n");
						playerInfo = thisDemo.cut.Cl.gameState.stringData + thisDemo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
						std::string victimname = Info_ValueForKey(playerInfo, "n");
					}
				}
			}


			// Find out which players are visible / followed
			// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
			lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
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
	// process any new server commands
	for (; thisDemo.cut.Clc.lastExecutedServerCommand <= thisDemo.cut.Clc.serverCommandSequence; thisDemo.cut.Clc.lastExecutedServerCommand++) {
		char* command = thisDemo.cut.Clc.serverCommands[thisDemo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
		Command readCommand;
		readCommand.command = command;
		readCommand.demoTime = demoCurrentTime;
		readCommands.push_back(readCommand);
		Cmd_TokenizeString(command);
		char* cmd = Cmd_Argv(0);
		if (cmd[0]) {
			firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
		}
		if (!strcmp(cmd, "cs")) {
			if (!ConfigstringModified(&thisDemo.cut.Cl)) {
				return qfalse;
			}
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