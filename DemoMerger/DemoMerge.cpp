#include "demoCut.h"
#include "DemoReader.h"
#include "anims.h"
#include <vector>
#include <sstream>
#include <set>
#include <chrono>

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
	std::vector<std::string> commandDupesToFilter;
};


qboolean demoCutConfigstringModifiedManual(clientActive_t* clCut, int configStringNum, const char* value) {
	char* old;
	const char* s;
	int			i, index;
	const char* dup;
	gameState_t	oldGs;
	int			len;
	index = configStringNum;
	if (index < 0 || index >= MAX_CONFIGSTRINGS) {
		Com_Printf("demoCutConfigstringModified: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = value;
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
			Com_Printf("MAX_GAMESTATE_CHARS exceeded on %d:%s", configStringNum,value);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}

qboolean demoCutConfigstringModified(clientActive_t* clCut) {
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
			Com_Printf("MAX_GAMESTATE_CHARS exceeded on %d:%s", index, s);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}

void demoCutWriteDemoHeader(fileHandle_t f, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
	byte			bufData[MAX_MSGLEN];
	msg_t			buf;
	int				i;
	int				len;
	entityState_t* ent;
	entityState_t	nullstate;
	char* s;
	// write out the gamestate message
	MSG_Init(&buf, bufData, sizeof(bufData));
	MSG_Bitstream(&buf);
	// NOTE, MRE: all server->client messages now acknowledge
	MSG_WriteLong(&buf, clcCut->reliableSequence);
	MSG_WriteByte(&buf, svc_gamestate);
	MSG_WriteLong(&buf, clcCut->serverCommandSequence);
	// configstrings
	for (i = 0; i < MAX_CONFIGSTRINGS; i++) {
		if (!clCut->gameState.stringOffsets[i]) {
			continue;
		}
		s = clCut->gameState.stringData + clCut->gameState.stringOffsets[i];
		MSG_WriteByte(&buf, svc_configstring);
		MSG_WriteShort(&buf, i);
		MSG_WriteBigString(&buf, s);
	}
	// baselines
	Com_Memset(&nullstate, 0, sizeof(nullstate));
	for (i = 0; i < MAX_GENTITIES; i++) {
		ent = &clCut->entityBaselines[i];
		if (!ent->number) {
			continue;
		}
		MSG_WriteByte(&buf, svc_baseline);
		MSG_WriteDeltaEntity(&buf, &nullstate, ent, qtrue, (qboolean)(demoType == DM_15));
	}
	MSG_WriteByte(&buf, svc_EOF);
	// finished writing the gamestate stuff
	// write the client num
	MSG_WriteLong(&buf, clcCut->clientNum);
	// write the checksum feed
	MSG_WriteLong(&buf, clcCut->checksumFeed);
	// finished writing the client packet
	MSG_WriteByte(&buf, svc_EOF);
	// write it to the demo file
	len = LittleLong(clcCut->serverMessageSequence - 1);
	FS_Write(&len, 4, f);
	len = LittleLong(buf.cursize);
	FS_Write(&len, 4, f);
	FS_Write(buf.data, buf.cursize, f);
}

static void demoCutEmitPacketEntities(clSnapshot_t* from, clSnapshot_t* to, msg_t* msg, clientActive_t* clCut, demoType_t demoType) {
	entityState_t* oldent, * newent;
	int oldindex, newindex;
	int oldnum, newnum;
	int from_num_entities;
	// generate the delta update
	if (!from) {
		from_num_entities = 0;
	}
	else {
		from_num_entities = from->numEntities;
	}
	newent = NULL;
	oldent = NULL;
	newindex = 0;
	oldindex = 0;
	while (newindex < to->numEntities || oldindex < from_num_entities) {
		if (newindex >= to->numEntities) {
			newnum = 9999;
		}
		else {
			newent = &clCut->parseEntities[(to->parseEntitiesNum + newindex) & (MAX_PARSE_ENTITIES - 1)];
			newnum = newent->number;
		}
		if (oldindex >= from_num_entities) {
			oldnum = 9999;
		}
		else {
			oldent = &clCut->parseEntities[(from->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			oldnum = oldent->number;
		}
		if (newnum == oldnum) {
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, (qboolean)(demoType == DM_15));
			oldindex++;
			newindex++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue, (qboolean)(demoType == DM_15));
			newindex++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, (qboolean)(demoType == DM_15));
			oldindex++;
			continue;
		}
	}
	MSG_WriteBits(msg, (MAX_GENTITIES - 1), GENTITYNUM_BITS);	// end of packetentities
}

std::map<int, entityState_t> emptyEntities;

static void demoCutEmitPacketEntitiesManual( msg_t* msg, clientActive_t* clCut, demoType_t demoType, std::map<int, entityState_t>* entities, std::map<int, entityState_t>* fromEntities) {
	entityState_t* oldent, * newent;
	int oldindex, newindex;
	int oldnum, newnum;
	int from_num_entities;
	// generate the delta update
	//if (!from) {
	if (!fromEntities) {
		from_num_entities = 0;
		fromEntities = &emptyEntities;
	}
	else {
		//from_num_entities = from->numEntities;
		from_num_entities = fromEntities->size();
	}
	newent = NULL;
	oldent = NULL;
	newindex = 0;
	auto newIterator = entities->begin();
	oldindex = 0;
	auto oldIterator = fromEntities->begin();
	while (newindex < entities->size() || oldindex < from_num_entities) {
		if (newindex >= entities->size()) {
			newnum = 9999;
		}
		else {
			//newent = &clCut->parseEntities[(to->parseEntitiesNum + newindex) & (MAX_PARSE_ENTITIES - 1)];
			//newnum = newent->number;
			newnum = newIterator->first;
			newent = &newIterator->second;
		}
		if (oldindex >= from_num_entities) {
			oldnum = 9999;
		}
		else {
			//oldent = &clCut->parseEntities[(from->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			//oldnum = oldent->number;
			oldent = &oldIterator->second;
			oldnum = oldIterator->first;
		}
		if (newnum == oldnum) {
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, (qboolean)(demoType == DM_15));
			oldindex++;
			oldIterator++;
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue, (qboolean)(demoType == DM_15));
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, (qboolean)(demoType == DM_15));
			oldindex++;
			oldIterator++;
			continue;
		}
	}
	MSG_WriteBits(msg, (MAX_GENTITIES - 1), GENTITYNUM_BITS);	// end of packetentities
}

void demoCutWriteDemoMessage(msg_t* msg, fileHandle_t f, clientConnection_t* clcCut) {
	int len;
	// write the packet sequence
	len = LittleLong(clcCut->serverMessageSequence);
	FS_Write(&len, 4, f);
	// skip the packet sequencing information
	len = LittleLong(msg->cursize);
	FS_Write(&len, 4, f);
	FS_Write(msg->data, msg->cursize, f);
}

void demoCutWriteDeltaSnapshot(int firstServerCommand, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
	msg_t			msgImpl, * msg = &msgImpl;
	byte			msgData[MAX_MSGLEN];
	clSnapshot_t* frame, * oldframe;
	int				lastframe = 0;
	int				snapFlags;
	MSG_Init(msg, msgData, sizeof(msgData));
	MSG_Bitstream(msg);
	MSG_WriteLong(msg, clcCut->reliableSequence);
	// copy over any commands
	for (int serverCommand = firstServerCommand; serverCommand <= clcCut->serverCommandSequence; serverCommand++) {
		char* command = clcCut->serverCommands[serverCommand & (MAX_RELIABLE_COMMANDS - 1)];
		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, serverCommand/* + serverCommandOffset*/);
		MSG_WriteString(msg, command);
	}
	// this is the snapshot we are creating
	frame = &clCut->snap;
	if (clCut->snap.messageNum > 0 && !forceNonDelta) {
		lastframe = 1;
		oldframe = &clCut->snapshots[(clCut->snap.messageNum - 1) & PACKET_MASK]; // 1 frame previous
		if (!oldframe->valid) {
			// not yet set
			lastframe = 0;
			oldframe = NULL;
		}
	}
	else {
		lastframe = 0;
		oldframe = NULL;
	}
	MSG_WriteByte(msg, svc_snapshot);
	// send over the current server time so the client can drift
	// its view of time to try to match
	MSG_WriteLong(msg, frame->serverTime);
	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (oldframe) {
		MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, &frame->ps, (qboolean)(demoType == DM_15));
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps, (qboolean)(demoType == DM_15));
	}
	// delta encode the entities
	demoCutEmitPacketEntities(oldframe, frame, msg, clCut, demoType);
	MSG_WriteByte(msg, svc_EOF);
	demoCutWriteDemoMessage(msg, f, clcCut);
}
//void demoCutWriteDeltaSnapshotManual(int firstServerCommand, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType,std::map<int,entityState_t>* entities,std::map<int,entityState_t>* fromEntities,playerState_t* fromPS) {
void demoCutWriteDeltaSnapshotManual(std::vector<std::string>* newCommands, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType,std::map<int,entityState_t>* entities,std::map<int,entityState_t>* fromEntities,playerState_t* fromPS) {
	msg_t			msgImpl, * msg = &msgImpl;
	byte			msgData[MAX_MSGLEN];
	qboolean		doDelta = qfalse;
	clSnapshot_t* frame;// , * oldframe;
	int				lastframe = 0;
	int				snapFlags;
	MSG_Init(msg, msgData, sizeof(msgData));
	MSG_Bitstream(msg);
	MSG_WriteLong(msg, clcCut->reliableSequence);
	// copy over any commands
	/*for (int serverCommand = firstServerCommand; serverCommand <= clcCut->serverCommandSequence; serverCommand++) {
		char* command = clcCut->serverCommands[serverCommand & (MAX_RELIABLE_COMMANDS - 1)];
		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, serverCommand)// + serverCommandOffset);
		MSG_WriteString(msg, command);
	}*/
	for (int i = 0; i<newCommands->size(); i++) {
		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, ++clcCut->serverCommandSequence/* + serverCommandOffset*/);
		MSG_WriteString(msg, (*newCommands)[i].c_str());
	}
	// this is the snapshot we are creating
	frame = &clCut->snap;
	if (fromEntities && fromPS && /*clCut->snap.messageNum > 0 &&*/ !forceNonDelta) {
		lastframe = 1;
		//lastframe = clcCut->reliableSequence-1;
		doDelta = qtrue;
		/*oldframe = &clCut->snapshots[(clCut->snap.messageNum - 1) & PACKET_MASK]; // 1 frame previous
		if (!oldframe->valid) {
			// not yet set
			lastframe = 0;
			oldframe = NULL;
		}*/
	}
	else {
		lastframe = 0;
		//oldframe = NULL;
	}
	MSG_WriteByte(msg, svc_snapshot);
	// send over the current server time so the client can drift
	// its view of time to try to match
	MSG_WriteLong(msg, frame->serverTime);
	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (doDelta) {
		MSG_WriteDeltaPlayerstate(msg, fromPS, &frame->ps, (qboolean)(demoType == DM_15));
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps, (qboolean)(demoType == DM_15));
	}
	// delta encode the entities
	demoCutEmitPacketEntitiesManual(msg, clCut, demoType, entities, fromEntities);
	MSG_WriteByte(msg, svc_EOF);
	demoCutWriteDemoMessage(msg, f, clcCut);
}

void demoCutParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType) {
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

qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
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
	MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps, demoType,qfalse);

	// JKA-specific
	if (demoType == DM_26 && newSnap.ps.m_iVehicleNum)
		MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->vps : NULL, &newSnap.vps, demoType, qtrue);


	// read packet entities
	demoCutParsePacketEntities(msg, oldSnap, &newSnap, clCut, demoType);
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
	return qtrue;
}


qboolean demoCutInitClearGamestate(clientConnection_t* clcCut, clientActive_t* clCut, int serverCommandSequence, int clientNum, int checksumFeed) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd;
	char* s;
	clcCut->connectPacketCount = 0;
	Com_Memset(clCut, 0, sizeof(*clCut));
	clcCut->serverCommandSequence = serverCommandSequence;
	clCut->gameState.dataCount = 1;
	clcCut->clientNum = clientNum;
	clcCut->checksumFeed = checksumFeed;
	return qtrue;
}

void demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
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


std::string makeConfigStringCommand(int index, std::string value) {
	std::stringstream ss;
	ss << "cs " << index << " " << value;
	return ss.str();
}

/*
================
G_FindConfigstringIndex

================
*/
int G_FindConfigstringIndex(char* name, int start, int max, qboolean create, clientActive_t* clCut, std::vector<std::string>* commandsToAdd) {
	int		i;
	//char	s[MAX_STRING_CHARS];
	char* s;

	if (!name || !name[0]) {
		return 0;
	}

	for (i = 1; i < max; i++) {
		//trap_GetConfigstring(start + i, s, sizeof(s));
		int stringOffset = clCut->gameState.stringOffsets[start + i];
		s = clCut->gameState.stringData + stringOffset;//sizeof(demo.cut.Cl.gameState.stringData) - stringOffset

		if (!s[0]) {
			break;
		}
		if (!strcmp(s, name)) {
			return i;
		}
	}

	if (!create) {
		return 0;
	}

	if (i == max) {
		Com_Printf("G_FindConfigstringIndex: overflow");
		return 0;
		//G_Error("G_FindConfigstringIndex: overflow");
	}

	//trap_SetConfigstring(start + i, name);
	demoCutConfigstringModifiedManual(clCut, start + i, name);
	commandsToAdd->push_back(makeConfigStringCommand(start + i, name));

	return i;
}

int G_SoundIndex(char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd) {
	return G_FindConfigstringIndex(name, CS_SOUNDS, MAX_SOUNDS, qtrue, clCut, commandsToAdd);
}
int G_ModelIndex(char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd) {
	return G_FindConfigstringIndex(name, CS_MODELS, MAX_MODELS, qtrue, clCut, commandsToAdd);
}
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
	char* ext;
	demoType_t		demoType;

	strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), outputName, strlen(outputName) - 6);
	ext = (char*)outputName + strlen(outputName) - 6;
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
	}

	memset(&demo, 0, sizeof(demo));
	memset(&playerEventData, 0, sizeof(playerEventData));

	std::vector<DemoReaderTrackingWrapper> demoReaders;
	std::cout << "loading up demos...";
	int startTime = INT_MAX;
	for (int i = 0; i < inputFiles->size(); i++) {
		std::cout << i<<"...";
		demoReaders.emplace_back();
		demoReaders.back().reader.LoadDemo((*inputFiles)[i].c_str());
		startTime = std::min(startTime, demoReaders.back().reader.GetFirstSnapServerTime()); // Find earliest serverTime from all source demos and start there.
	}
	std::cout << "done.";

	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	for (int i = 0; i < MAX_CONFIGSTRINGS; i++) {
		tmpConfigString = demoReaders[0].reader.GetConfigString(i,&tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString);
		}
	}

	std::map<int, int> lastSpectatedClientNums; // Need this for later.

	// Copy over player config strings
	for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i].reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.
			int spectatedClient = demoReaders[i].reader.GetCurrentPlayerState().clientNum;
			lastSpectatedClientNums[i] = spectatedClient;			
			//if (i >= MAX_CLIENTS) continue; // We don't have names/configstrings for players > 32
			//tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(spectatedClient,&tmpConfigStringMaxLength);
			//if (strlen(tmpConfigString)) {
				//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS+i, tmpConfigString);
			//}
		}
	}
	
	//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_LEVEL_START_TIME, "10000");

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially merged using JK2DemoCutter tools.");

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName), "%s%s", outputNameNoExt, ext);
	while (FS_FileExists(newName)) {
		Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputNameNoExt, dupeIterator, ext);
		dupeIterator++;
	}

	newHandle = FS_FOpenFileWrite(newName);
	if (!newHandle) {
		Com_Printf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	int time = startTime; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
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
	int framesWritten = 0;
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
		qboolean mainPlayerPSIsInterpolated = qfalse;
		int mainPlayerServerTime = 0;
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
				
				SnapshotInfo* snapInfoHere = demoReaders[i].reader.GetSnapshotInfoAtServerTime(time);
				qboolean snapIsInterpolated = qfalse;
				if (!snapInfoHere) {
					int thisDemoLastServerTime = demoReaders[i].reader.GetLastServerTimeBeforeServerTime(time);
					int thisDemoNextServerTime = demoReaders[i].reader.GetFirstServerTimeAfterServerTime(time);
					if (thisDemoLastServerTime == -1 || thisDemoNextServerTime == -1)continue;

					snapInfoHere = demoReaders[i].reader.GetSnapshotInfoAtServerTime(thisDemoLastServerTime);
					// TODO Do actual interpolation instead of just copying last one. Don't copy entities that are in previous but not in next.
					snapIsInterpolated = qtrue;
				}
				else {
					demoReaders[i].packetsUsed++;
				}
				//std::map<int, entityState_t> hereEntities = demoReaders[i].GetCurrentEntities();
				//tmpPS = demoReaders[i].GetCurrentPlayerState();
				tmpPS = snapInfoHere->playerState;


				// self explanatory.
				if (nonSkippedDemoIndex++ == 0 || (tmpPS.clientNum == mainPlayerPS.clientNum && mainPlayerPSIsInterpolated && (!snapIsInterpolated || snapInfoHere->serverTime > mainPlayerServerTime))) { // TODO MAke this more sophisticated. Allow moving over some non-snapped values from entitystates perhaps to smooth out mainPlayerPS
					// For reference, here's what gets snapped (rounded) in entities:
					// SnapVector( s->pos.trBase );
					// SnapVector( s->apos.trBase );
					mainPlayerPS = tmpPS;
					mainPlayerPSIsInterpolated = snapIsInterpolated;
					mainPlayerServerTime = snapInfoHere->serverTime;
				}
				else if(tmpPS.clientNum != mainPlayerPS.clientNum) {
					BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse, qtrue);
					if (playerEntities.find(tmpPS.clientNum) == playerEntities.end() || (entityIsInterpolated[tmpPS.clientNum] && (!snapIsInterpolated || entityServerTime[tmpPS.clientNum] < snapInfoHere->serverTime))) { // Prioritize entities that are not interpolated
						playerEntities[tmpPS.clientNum] = tmpES;
						entityIsInterpolated[tmpPS.clientNum] = snapIsInterpolated;
						entityServerTime[tmpPS.clientNum] = snapInfoHere->serverTime;
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

					if (it->first != mainPlayerPS.clientNum) {
						if (!(it->second.eFlags & EF_PLAYER_EVENT && it->second.otherEntityNum == mainPlayerPS.clientNum)) { // Don't copy over player event entities if we are spectating that player (all the events belong into the playerstate then)
							if (playerEntities.find(it->first) == playerEntities.end() || (entityIsInterpolated[it->first] && (!snapIsInterpolated || entityServerTime[it->first] < snapInfoHere->serverTime))) { // Prioritize entities that are not interpolated
								playerEntities[it->first] = it->second;
								entityIsInterpolated[it->first] = snapIsInterpolated;
								entityServerTime[it->first] = snapInfoHere->serverTime;
							}
						}
					}
					else if(mainPlayerPSIsInterpolated && (!snapIsInterpolated || snapInfoHere->serverTime > mainPlayerServerTime)){
						// Move some entity stuff over to playerState to improve its precision? Kind of experimental.
						CG_EntityStateToPlayerState(&it->second, &mainPlayerPS, demoType, qtrue, NULL, qtrue);
						mainPlayerServerTime = snapInfoHere->serverTime;
						// This does not reset the isInterpolated status, since it's kind of a "meh" value from an entity. It's just a patchwork kinda.
					}
				}


				// Get new commands
				std::vector<std::string> newCommandsHere = demoReaders[i].reader.GetNewCommandsAtServerTime(time);
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
				std::vector<Event> newEvents = demoReaders[i].reader.GetNewEventsAtServerTime(time,EK_ALL);
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
		for (int i = 0; i < MAX_CLIENTS; i++) {

			// First, clear event data in actual entities/playerstate
			if (playerEntities.find(i) != playerEntities.end()) { // Main player entity
				playerEntities[i].event = 0;
				playerEntities[i].eventParm = 0;
			}
			for (auto it = extraPlayerEventEntities[i].begin(); it != extraPlayerEventEntities[i].end(); it++) {
				if (playerEntities.find(*it) != playerEntities.end()) {
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
					if (mainPlayerPS.clientNum == i) {
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
							playerEntities[thisPlayerEventData->events[e].assignedEntityNum].event = thisPlayerEventData->events[e].eventValue;
							playerEntities[thisPlayerEventData->events[e].assignedEntityNum].eventParm = thisPlayerEventData->events[e].eventParm;
						}
					}

				}
			}
			extraPlayerEventEntities[i].clear();
		}
		

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0].reader.GetCurrentSnap();
		Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 

		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, NULL,NULL);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &playerEntities, &playerEntitiesOld, &mainPlayerPSOld);
		}

		framesWritten++;

		int oldTime = time;
		time = INT_MAX;
		for (int i = 0; i < demoReaders.size(); i++) {
			int nextTimeThisDemo = demoReaders[i].reader.GetFirstServerTimeAfterServerTime(oldTime);
			if (nextTimeThisDemo != -1) {
				time = std::min(time, nextTimeThisDemo); // Find nearest serverTime of all the demos.
			}
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


int main(int argc, char** argv) {
	if (argc <3) {
		std::cout << "need 2 arguments at least: outputname, demoname1, [demoname2, demoname3,...]";
		return 1;
	}
	char* demoName = NULL;
	char* outputName = NULL;

	outputName = argv[1];
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName);
	//strcpy(outputName, filteredOutputName);
	//delete[] filteredOutputName;

	std::vector<std::string> inputFiles;
	for (int i = 2; i < argc; i++) {
		inputFiles.emplace_back(argv[i]);
	}

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();

	if (demoCut(filteredOutputName,&inputFiles)) {
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