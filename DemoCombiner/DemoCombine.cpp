#include "demoCut.h"
#include "DemoReader.h"
#include "ini.h"
#include <vector>
#include <sstream>

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//



class DemoSource {
public:
	std::string sourceName;
	std::string demoPath;
	std::vector<std::string> playersToCopy;
};

class DemoReaderWrapper {
public:
	DemoReader reader;
	std::vector<int> playersToCopy;
	DemoSource* sourceInfo;
};


class SlotManager {
	struct sourceDemoMapping {
		int demoIndex;
		int entityNum;
	};

	std::map<int, sourceDemoMapping> mappings;
	typedef std::map<int, sourceDemoMapping>::iterator mappingIterator;
public:
	int getPlayerSlot(int demoIndex, int clientNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == clientNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,clientNum };
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	int getEntitySlot(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = MAX_CLIENTS; i < MAX_GENTITIES-1; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,entityNum };
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	int getSlotIfExists(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		return -1;
	}
	int freeSlots(int demoIndex) {
		int countErased = 0;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); ) {
			mappingIterator iteratorHere = it;
			it++;
			if (iteratorHere->second.demoIndex == demoIndex) {
				mappings.erase(iteratorHere);
				countErased++;
			}
		}
		return countErased;
	}
	int freeSlot(int demoIndex, int entityNum) {
		int countErased = 0;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); ) {
			mappingIterator iteratorHere = it;
			it++;
			if (iteratorHere->second.demoIndex == demoIndex && iteratorHere->second.entityNum == entityNum) {
				mappings.erase(iteratorHere);
				countErased++;
			}
		}
		return countErased;
	}
};

demo_t			demo;


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
	MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps, (qboolean)(demoType == DM_15));
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

qboolean demoCutParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType) {
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
#ifdef RELDEBUG
//#pragma optimize("", off)
#endif



class NameMatch {
public:
	std::string matchedName;
	int clientNum;
};

int getClientNumForDemo(std::string* thisPlayer,DemoReader* reader) {
	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	int clientNumHere = -1;
	int pstrlen = thisPlayer->size();
	if (!pstrlen) return clientNumHere;
	if (pstrlen <= 2 && isdigit((*thisPlayer)[0]) && (pstrlen ==1 || isdigit((*thisPlayer)[1]))) {
		// It's a number. ClientNum. Just parse it.
		clientNumHere = atoi(thisPlayer->c_str());
		tmpConfigString = reader->GetPlayerConfigString(clientNumHere, &tmpConfigStringMaxLength);
		std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength, "n");
		std::cout << *thisPlayer << " (interpreted as clientNum) matches '" << nameHere << "' (" << clientNumHere << ")" << std::endl;
	}
	else {
		std::string thisPlayerLower = *thisPlayer;
		std::transform(thisPlayerLower.begin(), thisPlayerLower.end(), thisPlayerLower.begin(), tolowerSignSafe);

		std::vector<NameMatch> caseInsensitiveMatches;
		std::vector<NameMatch> matches;

		// Find matching player name
		for (int c = 0; c < MAX_CLIENTS; c++) {
			tmpConfigString = reader->GetPlayerConfigString(c,&tmpConfigStringMaxLength);
			std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength, "n");
			std::string nameHereLower = nameHere;
			std::transform(nameHereLower.begin(), nameHereLower.end(), nameHereLower.begin(), tolowerSignSafe);

			if (strstr(nameHere.c_str(), thisPlayer->c_str())) {
				NameMatch nm;
				nm.clientNum = c;
				nm.matchedName = nameHere;
				matches.push_back(nm);
			}
			if (strstr(nameHereLower.c_str(), thisPlayerLower.c_str())) {
				NameMatch nm;
				nm.clientNum = c;
				nm.matchedName = nameHere;
				caseInsensitiveMatches.push_back(nm);
			}
		}

		if (matches.size() > 1) {
			std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
			for (int c = 0; c < matches.size(); c++) {
				std::cout << matches[c].matchedName << "(" << matches[c].clientNum << ")" << std::endl;
			}
			std::cout << "Picking first match '" << matches[0].matchedName << "' (" << matches[0].clientNum << ")" << std::endl;
			clientNumHere = matches[0].clientNum;
		}
		else if (matches.size() == 1) {
			std::cout <<"'" << *thisPlayer << "' matches '" << matches[0].matchedName << "' (" << matches[0].clientNum << ")" << std::endl;
			clientNumHere = matches[0].clientNum;
		}
		else {
			// No match. Try case insensitive
			if (caseInsensitiveMatches.size() > 1) {
				std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
				for (int c = 0; c < caseInsensitiveMatches.size(); c++) {
					std::cout << caseInsensitiveMatches[c].matchedName << "(" << caseInsensitiveMatches[c].clientNum << ")" << std::endl;
				}
				std::cout << "Picking first match '" << caseInsensitiveMatches[0].matchedName << "' ("<<caseInsensitiveMatches[0].clientNum<<")" << std::endl;
				clientNumHere = caseInsensitiveMatches[0].clientNum;
			}
			else if (caseInsensitiveMatches.size() == 1) {
				std::cout << "'" << *thisPlayer << "' matches '" << caseInsensitiveMatches[0].matchedName << "' (" << caseInsensitiveMatches[0].clientNum << ")" << std::endl;
				clientNumHere = caseInsensitiveMatches[0].clientNum;
			}
			else {
				std::cout << "[WARNING] '" << *thisPlayer << "' matches nothing. Discarding." << std::endl;
				// No match. Try case insensitive
			}
		}
	}
	return clientNumHere;
}


qboolean demoCut( const char* outputName, std::vector<DemoSource>* inputFiles) {
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

	std::vector<DemoReaderWrapper> demoReaders;
	for (int i = 0; i < inputFiles->size(); i++) {
		demoReaders.emplace_back();
		demoReaders.back().reader.LoadDemo((*inputFiles)[i].demoPath.c_str());
		demoReaders.back().sourceInfo = &(*inputFiles)[i];
	}

	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	for (int i = 0; i < MAX_CONFIGSTRINGS; i++) {
		if (i >= CS_PLAYERS && i < (CS_PLAYERS + MAX_CLIENTS)) continue; // Player stuff will be copied manually.
		tmpConfigString = demoReaders[0].reader.GetConfigString(i, &tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString);
		}
	}

	std::map<int, int> lastSpectatedClientNums; // Need this for later.

	//int copiedPlayerIndex = 0;

	SlotManager slotManager;


	// Find correct player numbers and copy configstrings for players
	for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i].reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, just for whatever's sake. Maybe not necessary? Could help with stuff like DARK Homebase demos, since player configstrings are sent a bit later?
			
			for (int p = 0; p < demoReaders[i].sourceInfo->playersToCopy.size(); p++) {

				std::string* thisPlayer = &demoReaders[i].sourceInfo->playersToCopy[p];
				int clientNumHere = getClientNumForDemo(thisPlayer,&demoReaders[i].reader);
				if (clientNumHere != -1) {
					demoReaders[i].playersToCopy.push_back(clientNumHere);
					int targetClientNum = slotManager.getPlayerSlot(i,clientNumHere);
					tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(clientNumHere,&tmpConfigStringMaxLength);

					if (strlen(tmpConfigString)) { // Would be pretty weird if this wasn't the case tho tbh.
						//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + (copiedPlayerIndex++), tmpConfigString);
						demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + targetClientNum, tmpConfigString);
					}
				}
			}
			int spectatedClient = demoReaders[i].reader.GetCurrentPlayerState().clientNum;
			lastSpectatedClientNums[i] = spectatedClient;
		}
	}
	
	
	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_LEVEL_START_TIME, "10000");

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially created using JK2DemoCutter tools.");

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName),  "%s%s", outputNameNoExt, ext);
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

	float sourceTime = 0.0f;
	float time = 10000.0f; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	float fps = 60.0f;
	std::map<int, entityState_t> targetEntities;
	std::map<int, entityState_t> targetEntitiesOld;
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
		targetEntities.clear();

		//copiedPlayerIndex = 0;
		for (int i = 0; i < demoReaders.size(); i++) {
			if (demoReaders[i].reader.SeekToTime(sourceTime)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.

				for (int c = 0; c < demoReaders[i].playersToCopy.size(); c++) {

					int clientNumHere = demoReaders[i].playersToCopy[c];

					int targetClientNum = slotManager.getPlayerSlot(i, clientNumHere);

					//std::map<int, entityState_t> hereEntities = demoReaders[i].reader.GetCurrentEntities();
					//tmpPS = demoReaders[i].GetCurrentPlayerState();
					//tmpPS = demoReaders[i].reader.GetInterpolatedPlayerState(sourceTime);
					SnapshotInfo* oldSnap = NULL;
					SnapshotInfo* newSnap = NULL;
					tmpPS = demoReaders[i].reader.GetInterpolatedPlayer(clientNumHere, sourceTime,&oldSnap,&newSnap,(qboolean)(targetClientNum == 0));
					//int originalPlayerstateClientNum = tmpPS.clientNum;



					// Check if oldSnap contains ET_BODY of this client and if so, copy it.
					// We have to do this here because we are using commandtime as the basis for timing, but normal 
					// serverTime for entities and such.
					// Hence, there would be a desync normally.
					// However TODO: This might still be a problem with many clients/bodies? Actually maybe we should just handle this stuff manually or sth?
					// Idk.
					if (oldSnap) {
						for (auto it = oldSnap->entities.begin(); it != oldSnap->entities.end(); it++) {
							// Players are already handled otherwhere
							if (it->first >= MAX_CLIENTS) {
								// Is this a corpse?
								if (it->second.eType == ET_BODY && it->second.clientNum == clientNumHere) {
									// Check if we are tracking this player
									if (targetClientNum != -1) {
										int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
										if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
											entityState_t tmpEntity = it->second;
											tmpEntity.clientNum = targetClientNum;
											tmpEntity.number = targetEntitySlot;
											tmpEntity.pos.trTime = time;
											if (EV_BODY_QUEUE_COPY == (tmpEntity.event & ~EV_EVENT_BITS)) {
												tmpEntity.eventParm = targetClientNum;
											}
											targetEntities[targetEntitySlot] = tmpEntity;
										}
									}
								}
							}
						}
					}


					//tmpPS.clientNum = copiedPlayerIndex;
					tmpPS.clientNum = targetClientNum;
					tmpPS.commandTime = time;

					// Process any changes of the spectated player in the original demo by just updating our configstring and setting teleport bit.
					/*if (lastSpectatedClientNums[i] != originalPlayerstateClientNum) {
						tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(originalPlayerstateClientNum);
						if (strlen(tmpConfigString)) {
							demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + i, tmpConfigString);
						}
						ss.str(std::string());
						ss << "cs " << (CS_PLAYERS + i) << " " << tmpConfigString;
						commandsToAdd.push_back(ss.str());
						tmpPS.eFlags |= EF_TELEPORT_BIT;
					}
					lastSpectatedClientNums[i]= originalPlayerstateClientNum;*/

					// self explanatory.
					//if (copiedPlayerIndex == 0) {
					if (targetClientNum == 0) {
						mainPlayerPS = tmpPS;
					}
					else {
						Com_Memset(&tmpES, 0, sizeof(tmpES));
						BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse);
						//playerEntities[copiedPlayerIndex] = tmpES;
						targetEntities[targetClientNum] = tmpES;
					}
					//copiedPlayerIndex++;
				}

				// Get various related entities
				std::map<int, entityState_t> sourceEntitiesAtTime = demoReaders[i].reader.GetEntitiesAtTime(sourceTime);
				for (auto it = sourceEntitiesAtTime.begin(); it != sourceEntitiesAtTime.end(); it++) {
					// Players are already handled otherwhere
					if (it->first >= MAX_CLIENTS) {
						/* Now handling this up there
						// Is this a corpse?
						if (it->second.eType == ET_BODY) {
							// Check if we are tracking this player
							int targetPlayerSlot = slotManager.getSlotIfExists(i,it->second.clientNum);
							if (targetPlayerSlot != -1) {
								int targetEntitySlot = slotManager.getEntitySlot(i,it->first);
								if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
									entityState_t tmpEntity = it->second;
									tmpEntity.clientNum = targetPlayerSlot;
									tmpEntity.number = targetEntitySlot;
									tmpEntity.pos.trTime = time;
									if (EV_BODY_QUEUE_COPY == (tmpEntity.event & ~EV_EVENT_BITS)) {
										tmpEntity.eventParm = targetPlayerSlot;
									}
									targetEntities[targetEntitySlot] = tmpEntity;
								}
							}
						}*/
					}
				}

				// Get new commands
				std::vector<std::string> newCommandsHere = demoReaders[i].reader.GetNewCommands(sourceTime);
				for (int c = 0; c < newCommandsHere.size(); c++) {

					Cmd_TokenizeString(newCommandsHere[c].c_str());
					char* cmd = Cmd_Argv(0);
					if (!strcmp(cmd, "print") || !strcmp(cmd, "chat")/* || !strcmp(cmd, "cp")*/) {
						if (!strstr(newCommandsHere[c].c_str(), "!respos") && !strstr(newCommandsHere[c].c_str(), "!savepos")) { // TODO Make this case insensitive for absolute protection!
							commandsToAdd.push_back(newCommandsHere[c]);
						}
					}
				}

				// Get new events and remember them.
				std::vector<Event> newEventsHere = demoReaders[i].reader.GetNewEvents(sourceTime);
				std::map<int, entityState_t> entitiesHere;
				qboolean entitiesAlreadyRead = qfalse; // Slight optimization really, nthing more.
				for (int c = 0; c < newEventsHere.size(); c++) {
					Event* thisEvent = &newEventsHere[c];
					int eventNumber = thisEvent->eventNumber;
					qboolean addThisEvent = qfalse;
					if (eventNumber == EV_PLAYER_TELEPORT_IN || eventNumber == EV_PLAYER_TELEPORT_OUT) {
						//if (thisEvent->theEvent.clientNum == originalPlayerstateClientNum) {
						if (std::find(demoReaders[i].playersToCopy.begin(), demoReaders[i].playersToCopy.end(), thisEvent->theEvent.clientNum) != demoReaders[i].playersToCopy.end()) {
							thisEvent->theEvent.clientNum = i;
							addThisEvent = qtrue;
						}
					}
					if (eventNumber == EV_OBITUARY) {

						//int				target = ent->otherEntityNum;
						//int				attacker = ent->otherEntityNum2;

						// Check if we are tracking these players.
						int target = slotManager.getSlotIfExists(i, thisEvent->theEvent.otherEntityNum);
						int attacker = slotManager.getSlotIfExists(i, thisEvent->theEvent.otherEntityNum2);
						if (target != -1 && attacker != -1) {
							thisEvent->theEvent.otherEntityNum = target;
							thisEvent->theEvent.otherEntityNum2 = attacker;

							addThisEvent = qtrue;
						}
					}
					if (eventNumber == EV_SHIELD_HIT) {

						// Check if we are tracking this player.
						int target = slotManager.getSlotIfExists(i, thisEvent->theEvent.otherEntityNum);
						if (target != -1) {
							thisEvent->theEvent.otherEntityNum = target;

							addThisEvent = qtrue;
						}
					}
					if (eventNumber == EV_SABER_HIT || eventNumber == EV_SABER_BLOCK) { // How to get rid of hits with no corresponding player copied? Check distance?

						if (!entitiesAlreadyRead) entitiesHere = demoReaders[i].reader.GetEntitiesAtPreciseTime(thisEvent->demoTime,qtrue);

						// I think default max hitbox is 30x30x64? So middle to farthest point should be vector 15,15,30 length. but i think middle isnt centered?
						// Is it -24 and then 40 on the top? So let's say max diagonal is distance of 15,15,40. aka sqrt(15*15+15*15+40*40) = 45.27692569068708313286904083492
						// Let's double it and make it a round 100. Add max saber length *2 for good measure.
						constexpr float maxDistance = 100.0f + 2.0f * (float)SABER_LENGTH_MAX;

						/*if (eventNumber == EV_SABER_HIT && thisEvent->theEvent.eventParm == 0) {
							// Not a player hit
							// This could be a player hitting the floor or sth I think?
							// Let's allow it and widen the allowed distance by saber length *2.
							maxDistance += 2.0f*(float)SABER_LENGTH_MAX;
						}
						if (eventNumber == EV_SABER_HIT && thisEvent->theEvent.eventParm >1) {
							// I think this is dismemberment stuff
							maxDistance += 2.0f*(float)SABER_LENGTH_MAX;
						}
						if (eventNumber == EV_SABER_BLOCK) {
							// This is a saber block.
							maxDistance += 2.0f * (float)SABER_LENGTH_MAX;
						}*/

						// Check if there's any tracked player entity nearby, aka whether it makes sense to copy this over.
						for (auto entitIt = entitiesHere.begin(); entitIt != entitiesHere.end(); entitIt++) {
							if (entitIt->first < 0 || entitIt->first >= MAX_CLIENTS) break;
							int target = slotManager.getSlotIfExists(i, entitIt->second.number);
							vec3_t distanceVec;
							VectorSubtract(entitIt->second.pos.trBase, thisEvent->theEvent.pos.trBase, distanceVec);
							if (VectorLength(distanceVec) <= maxDistance) {
								addThisEvent = qtrue;
							}
						}

					}
					/*if (eventNumber == EV_BODY_QUEUE_COPY) { // This is actually part of the ET_BODY I'm copying over anyway?

						// Check if we are tracking this player.
						int target = slotManager.getSlotIfExists(i, thisEvent->theEvent.eventParm);
						if (target != -1) {
							thisEvent->theEvent.eventParm = target;

							addThisEvent = qtrue;
						}
					}*/
					if (addThisEvent) eventsToAdd.push_back(*thisEvent);
				}
			}
			else {
				// We reached the end of this demo.
				slotManager.freeSlots(i);
			}
			if (!demoReaders[i].reader.EndReachedAtTime(sourceTime)) {
				allSourceDemosFinished = qfalse;
			}
		}

		// Find empty places in entities and add events.
		for (int i = 0; i < eventsToAdd.size(); i++) {
			for (int e = MAX_CLIENTS; e < MAX_GENTITIES-1; e++) { // Can I use full MAX_GENTITIES amount? Isn't MAX_GENTITIES -1 reserved for world and stuff like that?
				if (targetEntities.find(e) == targetEntities.end()) {
					// Let's add it!
					eventsToAdd[i].theEvent.number = e;
					targetEntities[e] = eventsToAdd[i].theEvent;
					break;
				}
			}
		}

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0].reader.GetCurrentSnap();
		//Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 
		Com_Memset(demo.cut.Cl.snap.areamask,0,sizeof(demo.cut.Cl.snap.areamask));

		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &targetEntities, NULL,NULL);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &targetEntities, &targetEntitiesOld, &mainPlayerPSOld);
		}

		time += 1000.0f / fps;
		sourceTime += 1000.0f / fps;
		demo.cut.Clc.reliableSequence++;
		demo.cut.Clc.serverMessageSequence++;

		mainPlayerPSOld = mainPlayerPS;
		targetEntitiesOld.clear();
		for (auto it = targetEntities.begin(); it != targetEntities.end(); it++) {
			targetEntitiesOld[it->first] = it->second;
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
		std::cout << "need 2 arguments: outputname, scriptname";
		return 1;
	}
	char* scriptName = NULL;
	char* outputName = NULL;

	outputName = argv[1];
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName);
	strcpy(outputName, filteredOutputName);
	delete[] filteredOutputName;

	// Read the script
	scriptName = argv[2];
	//std::vector<std::string> inputFiles;
	mINI::INIFile file(scriptName);
	mINI::INIStructure ini;
	file.read(ini);

	std::vector<DemoSource> demoSources;

	for (auto const& it : ini)
	{
		DemoSource demoSource;
		auto const& section = it.first;
		auto const& collection = it.second;
		demoSource.sourceName = section;
		std::cout << "[" << section << "]" << std::endl;
		//std::cout << "test: " << collection.get("test")<< strlen(collection.get("test").c_str()) << std::endl;
		std::cout << "path: " << collection.get("path") << std::endl;
		demoSource.demoPath = collection.get("path");

		std::vector<std::string> players = splitString(collection.get("players"),",");

		for (int i = 0; i < players.size(); i++) {
			std::cout << "player: " << players[i] << std::endl;
			if (players[i].size() > 0) {
				demoSource.playersToCopy.push_back(players[i]);
			}
		}

		if (demoSource.playersToCopy.size() > 0) {
			demoSources.push_back(demoSource);
		}
		else {
			std::cout << section << "was not added: No players specified." << std::endl;
		}
		std::cout << std::endl;
		/*for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << std::endl;
		}*/
	}



	/*
	for (int i = 2; i < argc; i++) {
		inputFiles.emplace_back(argv[i]);
	}
	*/
	if (demoCut(outputName,&demoSources)) {
		Com_Printf("Demo %s got successfully cut\n", scriptName);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", scriptName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif