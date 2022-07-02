#include "demoCut.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>

typedef jpcre2::select<char> jp;
//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

std::map<int,int> playerFirstVisible;
std::map<int,int> playerFirstFollowed;
std::map<int,int> playerFirstFollowedOrVisible;
std::map<int,int> lastEvent;
int lastKnownRedFlagCarrier = -1;
int lastKnownBlueFlagCarrier = -1;

struct Kill {
	int time;
	int targetClientNum;
	meansOfDeath_t mod;
	bool isSuicide;
	bool isRet;
	bool isDoom;
	bool isExplosion;
	bool isVisible;
	bool isFollowed;
};

struct SpreeInfo {
	int totalTime;
	int lastKillTime;
	int countKills = 0;
	int countRets = 0;
	int countDooms = 0;
	int countExplosions = 0;
	int countThirdPersons = 0; // Not followed ones.
};

// For detecting killstreaks
// Killer is the key, kill info is the value
std::map<int, std::vector<Kill>> kills;
std::map<int, int> lastKillStreak;

#define KILLSTREAK_MIN_KILLS 3
#define KILLSTREAK_MAX_INTERVAL 3000



// For calculating top/average speed of past second.
// Top level key is clientNum
// Lower level key is time
// Value is speed.
std::map<int, std::map<int, float>> speeds;
// TODO For future? Have angle saved too. See at what angle the kill comes. Big angle between capper and chaser at high speed is more impressive?


enum highlightSearchMode_t {
	SEARCH_ALL,
	SEARCH_INTERESTING,
	SEARCH_MY_CTF_RETURNS,
	SEARCH_CTF_RETURNS,
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
};


// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


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
		MSG_WriteDeltaEntity(&buf, &nullstate, ent, qtrue,(qboolean)(demoType == DM_15));
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
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue,(qboolean)(demoType == DM_15));
			oldindex++;
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
	demoCutParsePacketEntities(msg, oldSnap, &newSnap, clCut,demoType);
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

void demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
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

int demoCutGetEvent(entityState_t* es) {
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

entityState_t* findEntity(int number) {
	for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

		if (demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}


void CheckSaveKillstreak(SpreeInfo* spreeInfo,int clientNumAttacker,std::vector<int>* victims, int demoCurrentTime, std::ofstream* outputBatHandle, int bufferTime,int lastGameStateChangeInDemoTime, const char* sourceDemoFile) {

	if (spreeInfo->countKills > KILLSTREAK_MIN_KILLS) {
		const char* info = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
		std::string mapname = Info_ValueForKey(info, "mapname");
		const char* playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + clientNumAttacker];
		std::string playername = Info_ValueForKey(playerInfo, "n");
		//playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
		//std::string victimname = Info_ValueForKey(playerInfo, "n");


		int startTime = spreeInfo->lastKillTime-spreeInfo->totalTime - bufferTime;
		int endTime = spreeInfo->lastKillTime + bufferTime;
		int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
		bool isTruncated = false;
		if (earliestPossibleStart > startTime) {
			startTime = earliestPossibleStart;
			isTruncated = true;
		}


		std::stringstream ss;
		ss << mapname << std::setfill('0') << "___KILLSPREE" << spreeInfo->countKills << (spreeInfo->countRets ? va("R%d", spreeInfo->countRets) : "") << (spreeInfo->countDooms ? va("D%d", spreeInfo->countDooms) : "") << (spreeInfo->countExplosions ? va("E%d", spreeInfo->countExplosions) : "") << "___" << playername << "__";
		for (int i = 0; i < victims->size(); i++) {
			ss << "_" << (*victims)[i];
		}
		ss << (spreeInfo->countThirdPersons ? va("___thirdperson%d", spreeInfo->countThirdPersons) : "") << "___" << clientNumAttacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? "_tr" : "");;

		std::string targetFilename = ss.str();
		char* targetFilenameFiltered = new char[targetFilename.length() + 1];
		sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


		(*outputBatHandle) << "\nrem demoCurrentTime: " << demoCurrentTime;
		(*outputBatHandle) << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
		delete[] targetFilenameFiltered;
		//std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "\n";
		std::cout << ss.str() << "\n";

		lastKillStreak[clientNumAttacker] = spreeInfo->lastKillTime;
	}

}


qboolean demoHighlightFind(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode) {
	fileHandle_t	oldHandle = 0;
	//fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	int				oldSize;
	char			oldName[MAX_OSPATH];
	//char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	char* ext;
	demoType_t		demoType;
	int				demoStartTime = 0;
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int				demoCurrentTime = 0;
	int				lastGameStateChange = 0;
	int				lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;

	std::ofstream outputBatHandle;

	outputBatHandle.open(outputBatFile, std::ios_base::app); // append instead of overwrite

	//mvprotocol_t	protocol;

	// Since not in MME:
	/*if (!play) {
		Com_Printf("Demo cutting is allowed in mme mode only.\n");
		return qfalse;
	}*/
	//startTime += play->startTime;
	//endTime += play->startTime;
	

	//protocol = MV_GetCurrentProtocol();
	//if (protocol == PROTOCOL_UNDEF)
	//	ext = ".dm_16";
	//else
	//	ext = va(".dm_%i", protocol);
	//ext = Cvar_FindVar("mme_demoExt")->string;
	strncpy_s(oldName, sizeof(oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	if (!*ext) {
		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext,".dm_15")) {

		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext,".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));

	int messageOffset = 0;

	//	Com_SetLoadingMsg("Cutting the demo...");
	while (oldSize > 0) {
	cutcontinue:
		MSG_Init(&oldMsg, oldData, sizeof(oldData));
		/* Read the sequence number */
		if (FS_Read(&demo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4)
			goto cuterror;
		demo.cut.Clc.serverMessageSequence = LittleLong(demo.cut.Clc.serverMessageSequence);
		oldSize -= 4;
		/* Read the message size */
		if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4)
			goto cuterror;
		oldMsg.cursize = LittleLong(oldMsg.cursize);
		oldSize -= 4;
		/* Negative size signals end of demo */
		if (oldMsg.cursize < 0)
			break;
		if (oldMsg.cursize > oldMsg.maxsize)
			goto cuterror;
		/* Read the actual message */
		if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize)
			goto cuterror;
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
				goto cuterror;
			}
			cmd = MSG_ReadByte(&oldMsg);
			if (cmd == svc_EOF) {
				break;
			}
			// skip all the gamestates until we reach needed
			if (readGamestate < demo.currentNum) {
				//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
				if (cmd == svc_gamestate) {
					readGamestate++;
				}
				goto cutcontinue;
			}
			// other commands
			switch (cmd) {
			default:
				Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cuterror;
			case svc_nop:
				break;
			case svc_serverCommand:
				demoCutParseCommandString(&oldMsg, &demo.cut.Clc);
				break;
			case svc_gamestate:
				lastGameStateChange = demo.cut.Cl.snap.serverTime;
				lastGameStateChangeInDemoTime = demoCurrentTime;
				//if (readGamestate > demo.currentNum) {
				//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
				//	goto cutcomplete;
				//}
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl,demoType)) {
					goto cuterror;
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
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType)) {
					goto cuterror;
				}

				// Time related stuff
				if (messageOffset++ == 0) {
					// first message in demo. Get servertime offset from here to cut correctly.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}
				if (demo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
					demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}
				demoCurrentTime = demoBaseTime + demo.cut.Cl.snap.serverTime - demoStartTime;
				lastKnownTime = demo.cut.Cl.snap.serverTime;


				// Fire events
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					int eventNumber = demoCutGetEvent(thisEs);
					if (eventNumber) {
						

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
							isDoomKill = mod == MOD_FALLING;
							bool attackerIsFollowed = demo.cut.Cl.snap.ps.clientNum == attacker;

							Kill thisKill;
							thisKill.isRet = victimIsFlagCarrier;
							thisKill.isSuicide = isSuicide;
							thisKill.isDoom = isDoomKill;
							thisKill.isExplosion = (mod == MOD_FLECHETTE ||
								mod == MOD_FLECHETTE_ALT_SPLASH ||
								mod == MOD_ROCKET ||
								mod == MOD_ROCKET_SPLASH ||
								mod == MOD_ROCKET_HOMING ||
								mod == MOD_ROCKET_HOMING_SPLASH ||
								mod == MOD_THERMAL ||
								mod == MOD_THERMAL_SPLASH ||
								mod == MOD_TRIP_MINE_SPLASH ||
								mod == MOD_TIMED_MINE_SPLASH ||
								mod == MOD_DET_PACK_SPLASH);
							thisKill.mod = (meansOfDeath_t)mod;
							thisKill.targetClientNum = target;
							thisKill.time = demoCurrentTime;
							thisKill.isVisible = isVisible;
							thisKill.isFollowed = attackerIsFollowed;

							kills[attacker].push_back(thisKill);

							if (isSuicide || !victimIsFlagCarrier || isWorldKill || !isVisible) continue; // Not that interesting.
							
							// If it's not a doom kill, it's not that interesting unless we specifically are searching for our own returns or searching for everything
							if (!isDoomKill && searchMode != SEARCH_ALL && searchMode != SEARCH_MY_CTF_RETURNS && searchMode != SEARCH_CTF_RETURNS) continue;


							if (!attackerIsFollowed && searchMode == SEARCH_MY_CTF_RETURNS) continue; // We are searching for our own kills.
							
							entityState_t* attackerEntity = findEntity(attacker);

							// Find extra info for means of death.
							std::stringstream modInfo;
							weapon_t probableKillingWeapon = (weapon_t)weaponFromMOD[mod];
							
							qboolean needMoreInfo = qtrue;
							if (probableKillingWeapon == WP_NONE) { // means something like doom kill
								switch (mod) {
									case MOD_UNKNOWN:
										modInfo << "_WEIRD";
										break;
									case MOD_FORCE_DARK:
										modInfo << "_FORCE";
										needMoreInfo = qfalse;
										break;
									case MOD_SENTRY:
										modInfo << "_SENTRY";
										needMoreInfo = qfalse;
										break;
									case MOD_WATER:
										modInfo << "_DROWN";
										break;
									case MOD_SLIME:
										modInfo << "_SLIME";
										break;
									case MOD_LAVA:
										modInfo << "_LAVA";
										break;
									case MOD_CRUSH:
										modInfo << "_CRUSH";
										break;
									case MOD_TELEFRAG:
										modInfo << "_TELE";
										break;
									case MOD_FALLING:
										modInfo << "_DOOM";
										break;
									case MOD_SUICIDE:
										modInfo << "_SUIC";
										break;
									case MOD_TARGET_LASER:
										modInfo << "_LASER";
										break;
									case MOD_TRIGGER_HURT:
										modInfo << "_TRIG";
										break;
								}
								if(needMoreInfo)
									modInfo << "_~";
								if (attackerIsFollowed) {
									probableKillingWeapon = (weapon_t)demo.cut.Cl.snap.ps.weapon;
								}
								else if (attackerEntity) {
									probableKillingWeapon = (weapon_t)attackerEntity->weapon;
								}
							}
							if (needMoreInfo) {
								switch (probableKillingWeapon) {
									case WP_SABER:
										if (attackerIsFollowed) {

											modInfo << saberMoveNames[demo.cut.Cl.snap.ps.saberMove];
											if (!modInfo.str().size()) {
												modInfo << saberStyleNames[demo.cut.Cl.snap.ps.fd.saberDrawAnimLevel];
											}
										}
										else {

											if (attackerEntity) {
												modInfo << saberMoveNames[attackerEntity->saberMove];
												if (!modInfo.str().size()) {
													modInfo << saberStyleNames[attackerEntity->fireflag];
												}
											}
										}
										break;
									case WP_STUN_BATON:
										modInfo << "_STUN";
										break;
									case WP_BRYAR_PISTOL:
										modInfo << "_BRYAR";
										break;
									case WP_BLASTER:
										modInfo << "_BLASTER";
										break;
									case WP_DISRUPTOR:
										modInfo << "_DISRUPTOR";
										break;
									case WP_BOWCASTER:
										modInfo << "_BOWCAST";
										break;
									case WP_REPEATER:
										modInfo << "_REPEATER";
										break;
									case WP_DEMP2:
										modInfo << "_DEMP2";
										break;
									case WP_FLECHETTE:
										modInfo << "_FLECH";
										break;
									case WP_ROCKET_LAUNCHER:
										modInfo << "_ROCKET";
										break;
									case WP_THERMAL:
										modInfo << "_THERMAL";
										break;
									case WP_TRIP_MINE:
										modInfo << "_MINE";
										break;
									case WP_DET_PACK:
										modInfo << "_DTPCK";
										break;
									case WP_EMPLACED_GUN:
										modInfo << "_EMPLACED";
										break;
									case WP_TURRET:
										modInfo << "_TURRET";
										break;

									default:
										break;
								}
							}

							const char* info = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							std::string mapname = Info_ValueForKey(info, "mapname");
							const char* playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + attacker];
							std::string playername = Info_ValueForKey(playerInfo, "n");
							playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
							std::string victimname = Info_ValueForKey(playerInfo, "n");


							int startTime = demoCurrentTime - bufferTime;
							int endTime = demoCurrentTime + bufferTime;
							int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
							bool isTruncated = false;
							if (earliestPossibleStart > startTime) {
								startTime = earliestPossibleStart;
								isTruncated = true;
							}


							std::stringstream ss;
							ss << mapname << std::setfill('0') << "___RET" << modInfo.str() << "___" << playername << "___" << victimname << (attackerIsFollowed ? "" : "___thirdperson") << "_" << attacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? "_tr" : "");;

							std::string targetFilename = ss.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


							outputBatHandle << "\nrem demoCurrentTime: " << demoCurrentTime;
							outputBatHandle << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
							delete[] targetFilenameFiltered;
							std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "\n";

						}
					}
				}


				// Check for killstreaks.
				// Anything happening within KILLSTREAK_MAX_INTERVAL milliseconds of last kill counts towards it.
				// Baseline here is 3 seconds, since that is the limit for kill messages to be displayed as one in stacked kill messages.
				// TODO Also count stuff that maybe isn't within 3 secs but pretty darn close?
				// KILLSTREAK_MIN_KILLS 3
				// KILLSTREAK_MAX_INTERVAL 3000
				// TODO Let it count chains of killstreaks. If there's already a valid killstreak, let there be a single longer gap there.
				for (auto clientIt = kills.begin(); clientIt != kills.end(); clientIt++) {
					int clientNumAttacker = clientIt->first;
					SpreeInfo spreeInfo;
					Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
					std::vector<int> victims;

					if ((clientIt->second.back().time + KILLSTREAK_MAX_INTERVAL) >= demoCurrentTime) continue; // Might still have an unfinished one here!

					for (int i = 0; i < clientIt->second.size(); i++) {
						Kill* thisKill = &clientIt->second[i];
						if (thisKill->time <= lastKillStreak[clientNumAttacker]) continue; // This one's already been registered.
						if (thisKill->isSuicide || !thisKill->isVisible) continue; // Uninteresting.

						// Starting or continuing kill spree?
						if (spreeInfo.countKills == 0 || thisKill->time <= (spreeInfo.lastKillTime + KILLSTREAK_MAX_INTERVAL)) {
							
							victims.push_back(thisKill->targetClientNum);
							if (thisKill->isDoom) spreeInfo.countDooms++;
							if (thisKill->isRet) spreeInfo.countRets++;
							if (thisKill->isExplosion) spreeInfo.countExplosions++;
							if (!thisKill->isFollowed) spreeInfo.countThirdPersons++;
							spreeInfo.totalTime += spreeInfo.countKills == 0? 0: (thisKill->time - spreeInfo.lastKillTime);
							spreeInfo.countKills++;
							spreeInfo.lastKillTime = thisKill->time;
						}
						else {
							// This kill is not part of a killspree. Reset.
							// But first, check if this concludes an existing killspree that we can now save.
							CheckSaveKillstreak(&spreeInfo, clientNumAttacker, &victims,demoCurrentTime,&outputBatHandle,bufferTime,lastGameStateChangeInDemoTime,sourceDemoFile);

							// Reset.
							Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
							victims.clear();
						}
					}
					CheckSaveKillstreak(&spreeInfo, clientNumAttacker, &victims, demoCurrentTime, &outputBatHandle, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile);
				}


				// Find out which players are visible / followed
				// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
				lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
				for (int p = 0; p < MAX_CLIENTS; p++) {
					// Go through parseenttities of last snap to see if client is in it
					bool clientIsInSnapshot = false;
					bool clientVisibleOrFollowed = false;
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum+demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
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
							playerFirstVisible[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						playerFirstVisible[p] = -1;
					}
					if (demo.cut.Cl.snap.ps.clientNum == p) {
						clientVisibleOrFollowed = true;
						if (playerFirstFollowed[p] == -1) {
							playerFirstFollowed[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						playerFirstFollowed[p] = -1;
					}
					if (clientVisibleOrFollowed) {
						if (playerFirstFollowedOrVisible[p] == -1) {
							playerFirstFollowedOrVisible[p] = demo.cut.Cl.snap.serverTime;
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
		int firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
		// process any new server commands
		for (; demo.cut.Clc.lastExecutedServerCommand <= demo.cut.Clc.serverCommandSequence; demo.cut.Clc.lastExecutedServerCommand++) {
			char* command = demo.cut.Clc.serverCommands[demo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
			Cmd_TokenizeString(command);
			char* cmd = Cmd_Argv(0);
			if (cmd[0]) {
				firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
			}
			if (!strcmp(cmd, "cs")) {
				if (!demoCutConfigstringModified(&demo.cut.Cl)) {
					goto cuterror;
				}
			}
			if (!strcmp(cmd, "print")) {
				//Looking for 
				//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."

				// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.
				
				if (searchMode != SEARCH_INTERESTING && searchMode != SEARCH_ALL && searchMode != SEARCH_TOP10_DEFRAG) continue;

				jp::VecNum vec_num;
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
					const char * info = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
					std::string mapname = Info_ValueForKey(info, "mapname");
					std::string playername = vec_num[matchNum][1];
					int minutes = atoi(vec_num[matchNum][2].c_str());
					std::string secondString = vec_num[matchNum][3];
					float seconds = atof(vec_num[matchNum][3].c_str());
					int milliSeconds = (1000.0f* seconds)+0.5f;
					int pureMilliseconds = milliSeconds % 1000;
					int pureSeconds = milliSeconds / 1000;

					bool isLogged = vec_num[matchNum][4].length() > 0;
					bool isNumberOne = vec_num[matchNum][6].length() > 0;

					if (!isNumberOne && (searchMode != SEARCH_TOP10_DEFRAG || !isLogged)) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.

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
					int startTime = runStart - bufferTime;
					int endTime = demoCurrentTime + bufferTime;
					int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
					bool isTruncated = false;
					if (earliestPossibleStart > startTime) {
						startTime = earliestPossibleStart;
						isTruncated = true;
					}
					//startTime = std::max(lastGameStateChangeInDemoTime+1, startTime); // We can't start before 0 or before the last gamestate change. +1 to be safe, not sure if necessary.
					

					std::stringstream ss;
					ss << mapname << std::setfill('0') << "___" << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << "___" << playername << (isNumberOne ? "" : "___top10") << (isLogged ? "" : "___unlogged") << (wasFollowed ? "" : (wasVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" <<  playerNumber << "_" <<  demo.cut.Clc.clientNum << (isTruncated?"_tr":"");

					std::string targetFilename = ss.str();
					char* targetFilenameFiltered = new char[targetFilename.length()+1];
					sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

					outputBatHandle << "\nrem demoCurrentTime: "<< demoCurrentTime;
					outputBatHandle << "\n"<< (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \""<<sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
					delete[] targetFilenameFiltered;
					std::cout << mapname << " " << playerNumber << " " << playername << " " << minutes << ":" << secondString << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << wasFollowed << " visible:" << wasVisible << " visibleOrFollowed:" << wasVisibleOrFollowed << "\n";
				}

				
				
			}
		}

#if DEBUG
		if (oldSize == 0) {
			goto cutcomplete;
		}
#endif
		/*if (demo.cut.Cl.snap.serverTime > endTime) {
			goto cutcomplete;
		}
		else if (framesSaved > 0) {
			// this msg is in range, write it 
			if (framesSaved > Q_max(10, demo.cut.Cl.snap.messageNum - demo.cut.Cl.snap.deltaNum)) {
				demoCutWriteDemoMessage(&oldMsg, newHandle, &demo.cut.Clc);
			}
			else {
				demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl,demoType);
			}
			framesSaved++;
		}
		else if (demo.cut.Cl.snap.serverTime >= startTime) {
			demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl,demoType);
			demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl,demoType);
			// copy rest
			framesSaved = 1;
		}*/
	}
cutcomplete:
	ret = qtrue;
	/*if (newHandle) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}*/
cuterror:
	//remove previosly converted demo from the same cut
	/*if (newHandle) {
		memset(newName, 0, sizeof(newName));
		if (demo.currentNum > 0) {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s.%d_cut.mme", oldName, demo.currentNum);
		}
		else {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s_cut.mme", oldName);
		}
		if (FS_FileExists(newName))
			FS_FileErase(newName);
	}*/
	FS_FCloseFile(oldHandle);
	//FS_FCloseFile(newHandle);
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
		}else if (!_stricmp(searchModeText, "ctfreturns")) {
			searchMode = SEARCH_CTF_RETURNS;
		}else if (!_stricmp(searchModeText, "top10defrag")) {
			searchMode = SEARCH_TOP10_DEFRAG;
		}
	}

	if (demoHighlightFind(demoName, bufferTime,"highlightExtractionScript.bat", searchMode)) {
		Com_Printf("Highlights successfully found.\n", demoName);
	}
	else {
		Com_Printf("Finding highlights in demo %s has resulted in errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif