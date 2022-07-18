#include "demoCut.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>
#include "sqlite3.h"
#include <set>

#include <iso646.h>
#include "picosha3.h"

#define DEBUGSTATSDB


typedef jpcre2::select<char> jp;
//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

//std::map<int,int> playerFirstVisible;
//std::map<int,int> playerFirstFollowed;
//std::map<int,int> playerFirstFollowedOrVisible;
//std::map<int,int> lastEvent;
struct LastSaberMoveInfo {
	int lastSaberMove;
	int lastSaberMoveChange;
	float speed;
};

struct TeamInfo {
	int playerCount;
	int score;
	//int flagHoldOrigin; // Did the current flag holder pick up the flag from base or from a dropped place? This will simply be the old flag status.
};
struct CapperKillsInfo {
	int lastKillDemoTime; // Will reset if the last kill time is before the flag hold.
	int kills;
	int rets;
};
LastSaberMoveInfo playerLastSaberMove[MAX_CLIENTS];
int playerFirstVisible[MAX_CLIENTS];
int playerFirstFollowed[MAX_CLIENTS];
int playerFirstFollowedOrVisible[MAX_CLIENTS];
int recentFlagHoldTimes[MAX_CLIENTS];
CapperKillsInfo recentKillsDuringFlagHold[MAX_CLIENTS];
int playerTeams[MAX_CLIENTS];
TeamInfo teamInfo[MAX_TEAMS];
int lastEvent[MAX_GENTITIES];
int lastEventTime[MAX_GENTITIES];
std::map<int,std::string> lastPlayerModel;
int lastKnownRedFlagCarrier = -1;
int lastKnownBlueFlagCarrier = -1;


struct cgs{
	int redflag, blueflag, yellowflag;
	int redFlagLastChange, blueFlagLastChange, yellowflagLastChange;
	int redFlagLastPickupOrigin, blueFlagLastPickupOrigin, yellowflagLastPickupOrigin;
	int redFlagLastChangeToTaken, blueFlagLastChangeToTaken, yellowflagLastChangeToTaken;
} cgs;


#ifdef DEBUGSTATSDB
typedef std::tuple<int, int, int, int, int,int> animStanceKey; // demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance
std::map< animStanceKey, int> animStanceCounts;
#endif


#define SQLBIND(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
#define SQLBIND_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
#define SQLBIND_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)

#define NEARBY_PLAYER_MAX_DISTANCE 1000.0f
struct NearbyPlayer {
	int clientNum;
	float distance;
};

class Kill {
public:
	int time;
	int targetClientNum;
	meansOfDeath_t mod;
	bool isSuicide;
	bool isRet;
	bool isDoom;
	bool isExplosion;
	bool isVisible;
	bool isFollowed;
	int timeSinceSaberMoveChange;
	float speedatSaberMoveChange;
	float victimMaxSpeedPastSecond;
	std::string hashSourceString;
	std::string hash;
	std::string attackerName;
	std::string victimName;
	std::string modInfoString;
	std::vector< NearbyPlayer> nearbyPlayers;
};

struct SpreeInfo {
	int totalTime;
	int lastKillTime;
	int countKills = 0;
	int countRets = 0;
	int countDooms = 0;
	int countExplosions = 0;
	int countThirdPersons = 0; // Not followed ones.
	float maxVictimSpeed;
};

// For detecting killstreaks
// Killer is the key, kill info is the value
std::map<int, std::vector<Kill>> kills;
std::map<int, int> timeCheckedForKillStreaks;

#define KILLSTREAK_MIN_KILLS 3
#define KILLSTREAK_MAX_INTERVAL 3000
#define OLDER_SPEEDS_STORE_LIMIT 2000 // Any speeds older than 2000ms are removed.
#define MAX_ASSUMED_SERVER_FPS 200
#define MAX_NEEDED_PAST_SPEED_SAMPLES (OLDER_SPEEDS_STORE_LIMIT*MAX_ASSUMED_SERVER_FPS/1000)


// For calculating top/average speed of past second.
// Top level key is clientNum
// Lower level key is time
// Value is speed.
//std::map<int, std::map<int, float>> speeds;
//std::map<int, float> speeds[MAX_CLIENTS];
struct Speed {
	int time;
	float speed;
};
std::vector<Speed> speeds[MAX_CLIENTS];
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

qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType,int* lastValidSnap) {
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
		int lastOneNum = newSnap.deltaNum;
		oldSnap = &clCut->snapshots[newSnap.deltaNum & PACKET_MASK];

		// Try work with it anyway?
		if (!oldSnap->valid && *lastValidSnap != -1) {
			// should never happen
			oldSnap = &clCut->snapshots[*lastValidSnap & PACKET_MASK]; 
			lastOneNum = *lastValidSnap;
			Com_Printf("Delta from invalid frame (not supposed to happen!). Trying to use last valid snap.\n");
		}

		if (!oldSnap->valid) {
			// should never happen
			Com_Printf("Delta from invalid frame (not supposed to happen!).\n");
		}
		//else if (oldSnap->messageNum != newSnap.deltaNum) {
		else if (oldSnap->messageNum != lastOneNum) {
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
	*lastValidSnap = newSnap.messageNum;

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
	Q_strncpyz(clcCut->serverCommands[index], MAX_STRING_CHARS, s, sizeof(clcCut->serverCommands[index]));
}
#ifdef RELDEBUG
//#pragma optimize("", off)
#endif

int demoCutGetEvent(entityState_t* es,int demoCurrentTime) {
	//if (lastEvent.find(es->number) == lastEvent.end()) {
	//	lastEvent[es->number] = 0;
	//} // Not really necessary is it? That's what it will be by default?

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

	// if ( cent->snapShotTime < cg.time - EVENT_VALID_MSEC ) {
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

entityState_t* findEntity(int number) {
	for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

		if (demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}


float getMaxSpeedForClientinTimeFrame(int clientNum, int fromTime, int toTime) {
	float maxSpeed = -1.0f;
	/*for (auto it = speeds[clientNum].begin(); it != speeds[clientNum].end(); it++) {
		if (it->first >= fromTime && it->first <= toTime && it->second > maxSpeed) {
			maxSpeed = it->second;
		}
	}*/
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && speeds[clientNum][i].speed > maxSpeed) {
			maxSpeed = speeds[clientNum][i].speed;
		}
	}
	return maxSpeed;
}


void setPlayerAndTeamData(clientActive_t* clCut) {
	int stringOffset;
	memset(teamInfo,0,sizeof(teamInfo));
	for (int i = 0; i < MAX_CLIENTS; i++) {

		stringOffset = clCut->gameState.stringOffsets[CS_PLAYERS + i];
		const char* playerInfo = clCut->gameState.stringData + stringOffset;
		const char* playerTeam = Info_ValueForKey(playerInfo, sizeof(clCut->gameState.stringData) - stringOffset, "t");
		if (strlen(playerTeam)) {
			int team = atoi(playerTeam);
			playerTeams[i] = team;
			teamInfo[team].playerCount++;
		}
	}
	stringOffset = clCut->gameState.stringOffsets[CS_SCORES1];
	teamInfo[TEAM_RED].score = atoi(clCut->gameState.stringData + stringOffset);
	stringOffset = clCut->gameState.stringOffsets[CS_SCORES2];
	teamInfo[TEAM_BLUE].score = atoi(clCut->gameState.stringData + stringOffset);
}


void CheckForNameChanges(clientActive_t* clCut,sqlite3* killDb,sqlite3_stmt* insertPlayerModelStatement,sqlite3_stmt* updatePlayerModelCountStatement) {


	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");

	static std::vector<std::string> modelsToAdd;
	modelsToAdd.clear();
	for (int i = 0; i < MAX_CLIENTS; i++) {

		stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + i];
		const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string modelName = Info_ValueForKey(victimInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "model");
		if (modelName != lastPlayerModel[i]) {
			lastPlayerModel[i] = modelName;
			modelsToAdd.push_back(modelName);
		}
	}

	for (int i = 0; i < modelsToAdd.size(); i++) {
		std::string* thisModel = &modelsToAdd[i];
		std::transform(thisModel->begin(), thisModel->end(), thisModel->begin(), tolowerSignSafe);
		size_t firstSlash = thisModel->find_first_of("/");

		bool variantExists = false;
		std::string baseModel = "";
		std::string variant = "";
		if (firstSlash == std::string::npos) { 
			// No variant. Just the model name
			baseModel = *thisModel;
			//baseModelCString = thisModel->c_str();
		}
		else {
			baseModel = thisModel->substr(0,firstSlash);
			//baseModelCString = thisModel->c_str();
			if (firstSlash < (thisModel->size() - 1)) {
				variantExists = true;
				variant = thisModel->substr(firstSlash + 1, thisModel->size()-firstSlash-1);
				//variantCString = variant.c_str();
			}
		}
		SQLBIND_TEXT(insertPlayerModelStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(updatePlayerModelCountStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(insertPlayerModelStatement, "@baseModel", baseModel.c_str());
		SQLBIND_TEXT(updatePlayerModelCountStatement, "@baseModel", baseModel.c_str());
		if (variantExists) {
			SQLBIND_TEXT(insertPlayerModelStatement, "@variant", variant.c_str());
			SQLBIND_TEXT(updatePlayerModelCountStatement, "@variant", variant.c_str());
		}

		int queryResult = sqlite3_step(insertPlayerModelStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting player model into database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(insertPlayerModelStatement);
		queryResult = sqlite3_step(updatePlayerModelCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error updating player model count in database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(updatePlayerModelCountStatement);
	}

}


void CheckSaveKillstreak(SpreeInfo* spreeInfo,int clientNumAttacker, std::vector<Kill>* killsOfThisSpree,std::vector<int>* victims,std::vector<std::string>* killHashes,std::string allKillsHashString, int demoCurrentTime, std::ofstream* outputBatHandle, int bufferTime,int lastGameStateChangeInDemoTime, const char* sourceDemoFile,sqlite3_stmt* insertSpreeStatement,sqlite3* killDb,std::string oldBasename,std::string oldPath,time_t oldDemoDateModified) {

	if (spreeInfo->countKills >= KILLSTREAK_MIN_KILLS) {
		int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
		const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
		std::string playername = "WEIRDATTACKER";
		if (clientNumAttacker >= 0 && clientNumAttacker < MAX_CLIENTS) {

			stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + clientNumAttacker];
			const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "n");
		}
		//playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
		//std::string victimname = Info_ValueForKey(playerInfo, "n");

		std::stringstream victimsSS;
		std::stringstream victimsNumsSS;
		/*for (int i = 0; i < victims->size(); i++) {
			stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + (*victims)[i]];
			const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			victimsSS << (*victims)[i] << ": " << Info_ValueForKey(victimInfo, sizeof(demo.cut.Cl.gameState.stringData)-stringOffset, "n") << " ("<< (*killsOfThisSpree)[i].modInfoString<<")" << "\n";
			victimsNumsSS << (i==0? "" :",") << (*victims)[i];
		}*/
		int lastKillTime = 0;
		std::set<int> nearbyPlayers;
		for (int i = 0; i < killsOfThisSpree->size(); i++) {
			//stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + (*victims)[i]];
			//const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			victimsSS << (*killsOfThisSpree)[i].targetClientNum/*<< (*victims)[i]*/ << ": " << (*killsOfThisSpree)[i].victimName << " ("<< (*killsOfThisSpree)[i].modInfoString<<", +"<< (i>0? ((*killsOfThisSpree)[i].time-lastKillTime) :0)<<")" << "\n";
			lastKillTime = (*killsOfThisSpree)[i].time;
			victimsNumsSS << (i==0? "" :",") << (*victims)[i];
			for (int n = 0; n < (*killsOfThisSpree)[i].nearbyPlayers.size(); n++) {
				nearbyPlayers.insert((*killsOfThisSpree)[i].nearbyPlayers[n].clientNum);
			}
		}
		std::string victimsString = victimsSS.str();
		std::string victimsNumsString = victimsNumsSS.str();

		// Remove any nearby players that are victims or that are the attacker. (they dont count as nearby players)
		for (int i = 0; i < killsOfThisSpree->size(); i++) {
			nearbyPlayers.erase((*killsOfThisSpree)[i].targetClientNum);
		}
		nearbyPlayers.erase(clientNumAttacker);


		std::stringstream nearbyPlayersSS;
		int nearbyPlayerscount = 0;
		for (auto it = nearbyPlayers.begin(); it != nearbyPlayers.end(); it++) {
			nearbyPlayersSS << (nearbyPlayerscount++ == 0 ? "" : ",") << *it;
		}
		std::string nearbyPlayersString = nearbyPlayersSS.str();


		std::stringstream killHashesSS;
		for (int i = 0; i < killHashes->size(); i++) {
			killHashesSS << (i == 0 ? "" : "\n") << (*killHashes)[i];
		}
		std::string killHashesString = killHashesSS.str();

		//hashss << playername << "_" << victimname << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
		auto sha3_512 = picosha3::get_sha3_generator<224>();
		std::string hash_hex_string = sha3_512.get_hex_string(allKillsHashString);
		std::string shorthash = hash_hex_string.substr(0, 3);


		int maxSpeedAttacker = getMaxSpeedForClientinTimeFrame(clientNumAttacker, spreeInfo->lastKillTime - spreeInfo->totalTime - 1000, spreeInfo->lastKillTime);
		int maxSpeedVictims = spreeInfo->maxVictimSpeed;


		// Log the kill.
		SQLBIND_TEXT(insertSpreeStatement, "@hash", hash_hex_string.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@shorthash", shorthash.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@killerName", playername.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@victimNames", victimsString.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@killHashes", killHashesString.c_str());
		SQLBIND(insertSpreeStatement, int, "@killerClientNum", clientNumAttacker);
		SQLBIND_TEXT(insertSpreeStatement, "@victimClientNums", victimsNumsString.c_str());
		SQLBIND(insertSpreeStatement, int, "@countKills", spreeInfo->countKills);
		SQLBIND(insertSpreeStatement, int, "@countRets", spreeInfo->countRets);
		SQLBIND(insertSpreeStatement, int, "@countDooms", spreeInfo->countDooms);
		SQLBIND(insertSpreeStatement, int, "@countExplosions", spreeInfo->countExplosions);
		SQLBIND(insertSpreeStatement, int, "@countThirdPersons", spreeInfo->countThirdPersons);

		SQLBIND_TEXT(insertSpreeStatement, "@nearbyPlayers", nearbyPlayers.size() > 0 ? nearbyPlayersString.c_str() : NULL);
		SQLBIND(insertSpreeStatement, int, "@nearbyPlayerCount", nearbyPlayers.size());

		SQLBIND(insertSpreeStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
		SQLBIND(insertSpreeStatement, int, "@maxSpeedAttacker", maxSpeedAttacker);
		SQLBIND(insertSpreeStatement, int, "@maxSpeedTargets", spreeInfo->maxVictimSpeed);
		SQLBIND_TEXT(insertSpreeStatement, "@demoName", oldBasename.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@demoPath", oldPath.c_str());
		SQLBIND(insertSpreeStatement, int, "@demoTime", spreeInfo->lastKillTime - spreeInfo->totalTime);
		SQLBIND(insertSpreeStatement, int, "@duration", spreeInfo->totalTime);
		SQLBIND(insertSpreeStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
		SQLBIND(insertSpreeStatement, int, "@demoDateTime", oldDemoDateModified);

		int queryResult = sqlite3_step(insertSpreeStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting killing spree into database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(insertSpreeStatement);


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
		ss << "___" << maxSpeedAttacker <<"_"<< maxSpeedVictims << "ups" << (spreeInfo->countThirdPersons ? va("___thirdperson%d", spreeInfo->countThirdPersons) : "") << "___" << clientNumAttacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? "_tr" : "") << "_" << shorthash;

		std::string targetFilename = ss.str();
		char* targetFilenameFiltered = new char[targetFilename.length() + 1];
		sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


		(*outputBatHandle) << "\nrem hash: " << hash_hex_string;
		(*outputBatHandle) << "\nrem demoCurrentTime: " << demoCurrentTime;
		(*outputBatHandle) << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
		delete[] targetFilenameFiltered;
		//std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "\n";
		std::cout << ss.str() << "\n";

		//timeCheckedForKillStreaks[clientNumAttacker] = spreeInfo->lastKillTime;
	}

}

qboolean demoHighlightFind(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, const char* outputBatFileDefrag, highlightSearchMode_t searchMode) {
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
	int				lastValidSnap = -1;

	std::ofstream outputBatHandle;
	std::ofstream outputBatHandleDefrag;

	outputBatHandle.open(outputBatFile, std::ios_base::app); // append instead of overwrite
	outputBatHandleDefrag.open(outputBatFileDefrag, std::ios_base::app); // append instead of overwrite


	Com_Memset(playerFirstVisible,0,sizeof(playerFirstVisible));
	Com_Memset(playerFirstFollowed,0,sizeof(playerFirstFollowed));
	Com_Memset(playerFirstFollowedOrVisible,0,sizeof(playerFirstFollowedOrVisible));
	Com_Memset(lastEvent,0,sizeof(lastEvent));
	Com_Memset(lastEventTime,0,sizeof(lastEventTime));
	Com_Memset(playerLastSaberMove,0,sizeof(playerLastSaberMove));
	Com_Memset(recentFlagHoldTimes,0,sizeof(recentFlagHoldTimes));
	Com_Memset(recentKillsDuringFlagHold,0,sizeof(recentKillsDuringFlagHold));
	Com_Memset(playerTeams,0,sizeof(playerTeams));
	Com_Memset(teamInfo,0,sizeof(teamInfo));
	Com_Memset(&cgs,0,sizeof(cgs));


	sqlite3* killDb;
	sqlite3_open("killDatabase.db",&killDb);
	/*sqlite3_exec(killDb, "CREATE TABLE kills ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"victimName	TEXT NOT NULL,"
		"killerClientNum	INTEGER NOT NULL,"
		"victimClientNum	INTEGER NOT NULL,"
		"isReturn	BOOLEAN NOT NULL,"
		"isDoomKill	BOOLEAN NOT NULL,"
		"isExplosion	BOOLEAN NOT NULL,"
		"isSuicide	BOOLEAN NOT NULL,"
		"targetIsVisible	BOOLEAN NOT NULL,"
		"attackerIsVisible	BOOLEAN NOT NULL,"
		"isFollowed	BOOLEAN NOT NULL,"
		"meansOfDeath	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"maxSpeedAttacker	REAL,"
		"maxSpeedTarget	REAL,"
		"meansOfDeathString	TEXT NOT NULL,"
		"probableKillingWeapon	INTEGER NOT NULL,"
		"positionX	REAL,"
		"positionY	REAL,"
		"positionZ	REAL,"
		"demoName TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL,"
		"PRIMARY KEY(hash)"
		"); ",
		NULL,NULL,NULL);*/
	sqlite3_exec(killDb, "CREATE TABLE kills ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"victimName	TEXT NOT NULL,"
		"killerTeam	INTEGER,"
		"victimTeam	INTEGER NOT NULL,"
		"redScore INTEGER,"
		"blueScore INTEGER,"
		"otherFlagStatus INTEGER,"
		"redPlayerCount INTEGER,"
		"bluePlayerCount INTEGER,"
		"sumPlayerCount INTEGER,"
		"killerClientNum	INTEGER NOT NULL,"
		"victimClientNum	INTEGER NOT NULL,"
		"isDoomKill	BOOLEAN NOT NULL,"
		"isExplosion	BOOLEAN NOT NULL,"
		"isSuicide	BOOLEAN NOT NULL,"
		"meansOfDeath	INTEGER NOT NULL,"
		"positionX	REAL,"
		"positionY	REAL,"
		"positionZ	REAL,"
		"PRIMARY KEY(hash)"
		"); ",
		NULL, NULL, NULL);

	sqlite3_exec(killDb, "CREATE TABLE killAngles ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"killerIsFlagCarrier	BOOLEAN NOT NULL,"
		"isReturn	BOOLEAN NOT NULL,"
		"victimCapperKills INTEGER,"
		"victimCapperRets INTEGER,"
		"victimCapperWasFollowedOrVisible	BOOLEAN,"
		"victimFlagPickupSource	INTEGER,"
		"victimFlagHoldTime	INTEGER,"
		"targetIsVisible	BOOLEAN NOT NULL,"
		"targetIsFollowed	BOOLEAN NOT NULL,"
		"targetIsFollowedOrVisible	BOOLEAN NOT NULL,"
		"isSuicide	BOOLEAN NOT NULL,"
		"attackerIsVisible	BOOLEAN NOT NULL,"
		"attackerIsFollowed	BOOLEAN NOT NULL,"
		"attackerIsFollowedOrVisible	BOOLEAN NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"maxSpeedAttacker	REAL,"
		"maxSpeedTarget	REAL,"
		"currentSpeedAttacker	REAL,"
		"currentSpeedTarget	REAL,"
		"lastSaberMoveChangeSpeed	REAL,"
		"timeSinceLastSaberMoveChange INTEGER,"
		"meansOfDeathString	TEXT NOT NULL,"
		"nearbyPlayers	TEXT,"
		"nearbyPlayerCount	INTEGER NOT NULL,"
		"probableKillingWeapon	INTEGER NOT NULL,"
		"directionX	REAL,"
		"directionY	REAL,"
		"directionZ	REAL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE captures ("
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"flagHoldTime	INTEGER NOT NULL,"
		"flagPickupSource	INTEGER NOT NULL,"
		"capperName	TEXT NOT NULL,"
		"capperClientNum INTEGER NOT NULL,"
		"capperIsVisible	BOOLEAN NOT NULL,"
		"capperIsFollowed	BOOLEAN NOT NULL,"
		"capperIsFollowedOrVisible	BOOLEAN NOT NULL,"
		"capperWasVisible	BOOLEAN NOT NULL,"
		"capperWasFollowed	BOOLEAN NOT NULL,"
		"capperWasFollowedOrVisible	BOOLEAN NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"flagTeam	INTEGER NOT NULL,"
		"capperKills INTEGER NOT NULL,"
		"capperRets INTEGER NOT NULL,"
		"redScore INTEGER,"
		"blueScore INTEGER,"
		"redPlayerCount INTEGER,"
		"bluePlayerCount INTEGER,"
		"sumPlayerCount INTEGER,"
		"maxSpeedCapper	REAL,"
		"nearbyPlayers	TEXT,"
		"nearbyPlayerCount	INTEGER NOT NULL,"
		"nearbyEnemies	TEXT,"
		"nearbyEnemyCount	INTEGER NOT NULL,"
		"directionX	REAL,"
		"directionY	REAL,"
		"directionZ	REAL,"
		"positionX	REAL,"
		"positionY	REAL,"
		"positionZ	REAL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE defragRuns ("
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"readableTime TEXT NOT NULL,"
		"totalMilliseconds	INTEGER,"
		"playerName	TEXT NOT NULL,"
		"isTop10	BOOLEAN NOT NULL,"
		"isNumber1	BOOLEAN NOT NULL,"
		"wasVisible	BOOLEAN NOT NULL,"
		"wasFollowed	BOOLEAN NOT NULL,"
		"wasFollowedOrVisible	BOOLEAN NOT NULL,"
		"runnerClientNum	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE killSprees ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"victimNames	TEXT NOT NULL,"
		"killHashes	TEXT NOT NULL,"
		"killerClientNum	INTEGER NOT NULL,"
		"victimClientNums	TEXT NOT NULL,"
		"countKills	INTEGER NOT NULL,"
		"countRets	INTEGER NOT NULL,"
		"countDooms	INTEGER NOT NULL,"
		"countExplosions	INTEGER NOT NULL,"
		"countThirdPersons	INTEGER NOT NULL,"
		"nearbyPlayers	TEXT,"
		"nearbyPlayerCount	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"maxSpeedAttacker	REAL,"
		"maxSpeedTargets	REAL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"duration INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"//,"
		//"PRIMARY KEY(hash)"
		"); ",
		NULL,NULL,NULL);
	sqlite3_exec(killDb, "CREATE TABLE playerModels (" 
		"map	TEXT NOT NULL,"
		"baseModel	TEXT NOT NULL,"
		"variant	TEXT,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(map,baseModel,variant)"
		"); ",
		NULL,NULL,NULL);
	
	/*char* preparedStatementText = "INSERT INTO kills"
		"(hash, shorthash, map, killerName, victimName, killerClientNum, victimClientNum, isReturn, isDoomKill, isExplosion, isSuicide, targetIsVisible,attackerIsVisible,"
		"isFollowed, meansOfDeath, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTarget, meansOfDeathString, probableKillingWeapon, positionX, "
		"positionY, positionZ,demoName,demoTime, serverTime, demoDateTime)"
		"VALUES "
		"(@hash, @shorthash, @map, @killerName, @victimName, @killerClientNum, @victimClientNum, @isReturn, @isDoomKill, @isExplosion, @isSuicide, @targetIsVisible,@attackerIsVisible,"
		"@isFollowed, @meansOfDeath, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTarget, @meansOfDeathString, @probableKillingWeapon, @positionX,"
		"@positionY, @positionZ,@demoName,@demoTime, @serverTime, @demoDateTime);";*/
	char* preparedStatementText = "INSERT INTO kills"
		"(hash,shorthash,map,serverName,killerName,victimName,killerTeam,victimTeam,redScore,blueScore,otherFlagStatus,redPlayerCount,bluePlayerCount,sumPlayerCount,killerClientNum,victimClientNum,isDoomKill,isExplosion,isSuicide,meansOfDeath,positionX,positionY,positionZ)"
		"VALUES "
		"(@hash,@shorthash,@map,@serverName,@killerName,@victimName,@killerTeam,@victimTeam,@redScore,@blueScore,@otherFlagStatus,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@killerClientNum,@victimClientNum,@isDoomKill,@isExplosion,@isSuicide,@meansOfDeath,@positionX,@positionY,@positionZ);";
	sqlite3_stmt* insertStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText, strlen(preparedStatementText) + 1, &insertStatement, NULL);
	preparedStatementText = "INSERT INTO killAngles"
		"(hash,shorthash,killerIsFlagCarrier,isReturn,victimCapperKills,victimCapperRets,victimCapperWasFollowedOrVisible,victimFlagPickupSource,victimFlagHoldTime,targetIsVisible,targetIsFollowed,targetIsFollowedOrVisible,attackerIsVisible,attackerIsFollowed,demoRecorderClientnum,maxSpeedAttacker,maxSpeedTarget,currentSpeedAttacker,currentSpeedTarget,meansOfDeathString,probableKillingWeapon,demoName,demoPath,demoTime,serverTime,demoDateTime,lastSaberMoveChangeSpeed,timeSinceLastSaberMoveChange,nearbyPlayers,nearbyPlayerCount,directionX,directionY,directionZ,map,isSuicide,attackerIsFollowedOrVisible)"
		"VALUES "
		"(@hash,@shorthash,@killerIsFlagCarrier,@isReturn,@victimCapperKills,@victimCapperRets,@victimCapperWasFollowedOrVisible,@victimFlagPickupSource,@victimFlagHoldTime,@targetIsVisible,@targetIsFollowed,@targetIsFollowedOrVisible,@attackerIsVisible,@attackerIsFollowed,@demoRecorderClientnum,@maxSpeedAttacker,@maxSpeedTarget,@currentSpeedAttacker,@currentSpeedTarget,@meansOfDeathString,@probableKillingWeapon,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime,@lastSaberMoveChangeSpeed,@timeSinceLastSaberMoveChange,@nearbyPlayers,@nearbyPlayerCount,@directionX,@directionY,@directionZ,@map,@isSuicide,@attackerIsFollowedOrVisible);";
	sqlite3_stmt* insertAngleStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertAngleStatement,NULL);
	preparedStatementText = "INSERT INTO captures"
		"(map,serverName,flagHoldTime,flagPickupSource,capperName,capperClientNum,capperIsVisible,capperIsFollowed,capperIsFollowedOrVisible,capperWasVisible,capperWasFollowed,capperWasFollowedOrVisible,demoRecorderClientnum,flagTeam,capperKills,capperRets,redScore,blueScore,redPlayerCount,bluePlayerCount,sumPlayerCount,maxSpeedCapper,nearbyPlayers,nearbyPlayerCount,nearbyEnemies,nearbyEnemyCount,directionX,directionY,directionZ,positionX,positionY,positionZ,demoName,demoPath,demoTime,serverTime,demoDateTime)"
		"VALUES "
		"(@map,@serverName,@flagHoldTime,@flagPickupSource,@capperName,@capperClientNum,@capperIsVisible,@capperIsFollowed,@capperIsFollowedOrVisible,@capperWasVisible,@capperWasFollowed,@capperWasFollowedOrVisible,@demoRecorderClientnum,@flagTeam,@capperKills,@capperRets,@redScore,@blueScore,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@maxSpeedCapper,@nearbyPlayers,@nearbyPlayerCount,@nearbyEnemies,@nearbyEnemyCount,@directionX,@directionY,@directionZ,@positionX,@positionY,@positionZ,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime);";
	sqlite3_stmt* insertCaptureStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertCaptureStatement,NULL);
	preparedStatementText = "INSERT INTO defragRuns"
		"(map,serverName,readableTime,totalMilliseconds,playerName,demoRecorderClientnum,runnerClientNum,isTop10,isNumber1,wasVisible,wasFollowed,wasFollowedOrVisible,demoName,demoPath,demoTime,serverTime,demoDateTime)"
		"VALUES "
		"(@map,@serverName,@readableTime,@totalMilliseconds,@playerName,@demoRecorderClientnum,@runnerClientNum,@isTop10,@isNumber1,@wasVisible,@wasFollowed,@wasFollowedOrVisible,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime);";
	sqlite3_stmt* insertDefragRunStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertDefragRunStatement,NULL);
	preparedStatementText = "INSERT INTO killSprees "
		"( hash, shorthash, map,killerName, victimNames ,killHashes, killerClientNum, victimClientNums, countKills, countRets, countDooms, countExplosions,"
		" countThirdPersons, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTargets,demoName,demoPath,demoTime,duration,serverTime,demoDateTime,nearbyPlayers,nearbyPlayerCount)"
		" VALUES "
		"( @hash, @shorthash, @map, @killerName, @victimNames ,@killHashes, @killerClientNum, @victimClientNums, @countKills, @countRets, @countDooms, @countExplosions,"
		" @countThirdPersons, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTargets,@demoName,@demoPath,@demoTime,@duration,@serverTime,@demoDateTime,@nearbyPlayers,@nearbyPlayerCount)";
	sqlite3_stmt* insertSpreeStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertSpreeStatement,NULL);
	preparedStatementText = "INSERT OR IGNORE INTO playerModels (map,baseModel,variant,countFound) VALUES (@map,@baseModel,@variant, 0);";
	sqlite3_stmt* insertPlayerModelStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertPlayerModelStatement,NULL);
	preparedStatementText = "UPDATE playerModels SET countFound = countFound + 1 WHERE map=@map AND baseModel=@baseModel AND variant=@variant;";
	sqlite3_stmt* updatePlayerModelCountStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&updatePlayerModelCountStatement,NULL);

	sqlite3_exec(killDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);


#ifdef DEBUGSTATSDB
	sqlite3* debugStatsDb;
	sqlite3_open("debugStatsDb.db", &debugStatsDb);

	sqlite3_exec(debugStatsDb, "CREATE TABLE animStances ("
		"demoVersion INTEGER NOT NULL,"
		"saberHolstered BOOLEAN NOT NULL,"
		"torsoAnim INTEGER NOT NULL,"
		"legsAnim INTEGER NOT NULL,"
		"saberMove INTEGER NOT NULL,"
		"stance INTEGER NOT NULL,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance)"
		"); ",
		NULL, NULL, NULL);
	preparedStatementText = "INSERT OR IGNORE INTO animStances (demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance,countFound) VALUES (@demoVersion,@saberHolstered,@torsoAnim,@legsAnim,@saberMove,@stance, 0);";
	sqlite3_stmt* insertAnimStanceStatement;
	sqlite3_prepare_v2(debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &insertAnimStanceStatement, NULL);
	preparedStatementText = "UPDATE animStances SET countFound = countFound + @countFound WHERE demoVersion=@demoVersion AND saberHolstered=@saberHolstered AND torsoAnim=@torsoAnim AND @legsAnim=@legsAnim AND saberMove=@saberMove AND stance=@stance;";
	sqlite3_stmt* updateAnimStanceCountStatement;
	sqlite3_prepare_v2(debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &updateAnimStanceCountStatement, NULL);
	sqlite3_exec(debugStatsDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
#endif



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

	std::string oldPath = va("%s%s", oldName, ext);
	std::string oldBasename = oldPath.substr(oldPath.find_last_of("/\\") + 1);

	// turn oldpath absolute
	std::filesystem::path tmpFSPath = oldPath;
	oldPath = std::filesystem::absolute(tmpFSPath).string();

	std::filesystem::file_time_type filetime = std::filesystem::last_write_time(va("%s%s", oldName, ext));
	time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));

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

				CheckForNameChanges(&demo.cut.Cl,killDb,insertPlayerModelStatement, updatePlayerModelCountStatement);
				setPlayerAndTeamData(&demo.cut.Cl);
				//Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
				//newHandle = FS_FOpenFileWrite(newName);
				//if (!newHandle) {
				//	Com_Printf("Failed to open %s for target cutting.\n", newName);
				//	return qfalse;
				//}
				readGamestate++;
				break;
			case svc_snapshot:
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType,&lastValidSnap)) {
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


				// Record speeds and check sabermove changes
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					if (thisEs->number >= 0 && thisEs->number < MAX_CLIENTS && !(thisEs->eFlags & EF_DEAD)) { // Don't count speeds of dead bodies. They get boosts from dying.
						//speeds[thisEs->number][demoCurrentTime] = VectorLength(thisEs->pos.trDelta);
						float speed = VectorLength(thisEs->pos.trDelta);
						speeds[thisEs->number].push_back({ demoCurrentTime,speed });

						// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
						if (playerLastSaberMove[thisEs->number].lastSaberMove != thisEs->saberMove) {
							playerLastSaberMove[thisEs->number].lastSaberMoveChange = demoCurrentTime;
							playerLastSaberMove[thisEs->number].lastSaberMove= thisEs->saberMove;
							playerLastSaberMove[thisEs->number].speed= speed;
						}
					}
				}
				if (demo.cut.Cl.snap.ps.pm_type != PM_DEAD && demo.cut.Cl.snap.ps.stats[STAT_HEALTH] > 0) {
					//speeds[demo.cut.Cl.snap.ps.clientNum][demoCurrentTime] = VectorLength(demo.cut.Cl.snap.ps.velocity);
					float speed = VectorLength(demo.cut.Cl.snap.ps.velocity);
					speeds[demo.cut.Cl.snap.ps.clientNum].push_back({ demoCurrentTime,speed });

					// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
					if (playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove != demo.cut.Cl.snap.ps.saberMove) {
						playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMoveChange = demoCurrentTime;
						playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove = demo.cut.Cl.snap.ps.saberMove;
						playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].speed = speed;
					}
				}

#ifdef DEBUGSTATSDB
				if(demo.cut.Cl.snap.ps.weapon==WP_SABER){ // TODO Maybe add saber on/off here too? Because saber off might have same anim always?
					
					animStanceKey keyHere = { demoType,demo.cut.Cl.snap.ps.saberHolstered,demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT,demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT,demo.cut.Cl.snap.ps.saberMove,demo.cut.Cl.snap.ps.fd.saberAnimLevel };  // torsoAnim,legsAnim,saberMove,stance
					animStanceCounts[keyHere]++;
					
				}
#endif

				// Fire events
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					int eventNumber = demoCutGetEvent(thisEs,demoCurrentTime);
					if (eventNumber) {
						

						// Handle kills
						if (eventNumber == EV_OBITUARY) {
							int				target = thisEs->otherEntityNum;
							int				attacker = thisEs->otherEntityNum2;
							int				mod = thisEs->eventParm;
							bool			victimIsFlagCarrier = false;
							bool			attackerIsFlagCarrier = false;
							bool			isSuicide;
							bool			isDoomKill;
							bool			isWorldKill = false;
							bool			targetIsVisible = false;
							bool			attackerIsVisibleOrFollowed = false;
							bool			targetIsVisibleOrFollowed = false;
							bool			attackerIsVisible = false;
							if (target < 0 || target >= MAX_CLIENTS) {
								std::cout << "CG_Obituary: target out of range. This should never happen really.";
							}

							if (attacker < 0 || attacker >= MAX_CLIENTS) {
								attacker = ENTITYNUM_WORLD;
								isWorldKill = true;
							}
							entityState_t* targetEntity = findEntity(target);
							if (targetEntity) {
								targetIsVisible = true;
								/*if (targetEntity->powerups & (1 << PW_REDFLAG) || targetEntity->powerups & (1 << PW_BLUEFLAG)) {
									// If the victim isn't visible, his entity won't be available, thus this won't be set
									// But we're trying to find interesting moments, so stuff that's not even visible is not that interesting to us
									victimIsFlagCarrier = true;
								}*/
							}
							entityState_t* attackerEntity = findEntity(attacker);
							if (attackerEntity) {
								attackerIsVisible = true;
							}
							victimIsFlagCarrier = target == lastKnownBlueFlagCarrier || target == lastKnownRedFlagCarrier;
							attackerIsFlagCarrier = attacker == lastKnownBlueFlagCarrier || attacker == lastKnownRedFlagCarrier;

							int victimFlagTeam = victimIsFlagCarrier ? (target == lastKnownBlueFlagCarrier ? TEAM_BLUE:TEAM_RED) : -1;
							int attackerFlagTeam = attackerIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? TEAM_BLUE:TEAM_RED) : -1;

							int victimCarrierLastPickupOrigin = victimIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? cgs.blueFlagLastPickupOrigin : cgs.redFlagLastPickupOrigin) : -1;

							// Track how many people a flag carrier kills during his hold.
							if (!isWorldKill && attackerIsFlagCarrier) {
								if (recentKillsDuringFlagHold[attacker].lastKillDemoTime < demoCurrentTime-recentFlagHoldTimes[attacker]) {
									// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
									Com_Memset(&recentKillsDuringFlagHold[attacker],0,sizeof(CapperKillsInfo));
								}
								if (victimIsFlagCarrier) {
									recentKillsDuringFlagHold[attacker].rets++;
								}
								recentKillsDuringFlagHold[attacker].kills++;
								recentKillsDuringFlagHold[attacker].lastKillDemoTime = demoCurrentTime;
							}
							int victimFlagCarrierKillCount = -1;
							int victimFlagCarrierRetCount = -1;
							if (victimIsFlagCarrier) {
								// See if he had any kills/rets as carrier. (did he fight valiantly before he died?)
								if (recentKillsDuringFlagHold[target].lastKillDemoTime < demoCurrentTime - recentFlagHoldTimes[target]) {
									// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
									Com_Memset(&recentKillsDuringFlagHold[target], 0, sizeof(CapperKillsInfo));
								}
								victimFlagCarrierKillCount = recentKillsDuringFlagHold[target].kills;
								victimFlagCarrierRetCount = recentKillsDuringFlagHold[target].rets;
							}
							// TODO What if lastKnownBlueFlagCarrier or lastKnownRedFlagCarrier is remembered and another kill is counted wrongly as ret even though he is no longer holding a flag?


							// If victim was capper, see if his entire run from start to unfortunate end was captured.
							bool capperWasFollowed = false;
							bool capperWasVisible = false;
							bool capperWasVisibleOrFollowed = false;
							if (victimIsFlagCarrier) {

								int victimFlagHoldTime = recentFlagHoldTimes[target];
								if (playerFirstFollowed[target] != -1 && playerFirstFollowed[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasFollowed = true;
								}
								if (playerFirstVisible[target] != -1 && playerFirstVisible[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasVisible = true;
								}
								if (playerFirstFollowedOrVisible[target] != -1 && playerFirstFollowedOrVisible[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasVisibleOrFollowed = true;
								}
							}


							isSuicide = target == attacker;
							isDoomKill = mod == MOD_FALLING;
							bool targetIsFollowed = demo.cut.Cl.snap.ps.clientNum == target;
							bool attackerIsFollowed = demo.cut.Cl.snap.ps.clientNum == attacker;
							attackerIsVisibleOrFollowed = attackerIsFollowed || attackerIsVisible;
							targetIsVisibleOrFollowed = targetIsFollowed || targetIsVisible;

							//targetIsVisible = targetIsVisible && attackerIsVisibleOrFollowed; // Make sure both attacker and victim are visible. Some servers send info

							float maxSpeedTargetFloat = getMaxSpeedForClientinTimeFrame(target, demoCurrentTime - 1000, demoCurrentTime);
							int maxSpeedTarget = maxSpeedTargetFloat;
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
							//thisKill.isVisible = targetIsVisible;
							thisKill.isVisible = targetIsVisibleOrFollowed;
							thisKill.isFollowed = attackerIsFollowed;
							thisKill.victimMaxSpeedPastSecond = maxSpeedTargetFloat;
							thisKill.timeSinceSaberMoveChange = isWorldKill ? -1 : (demoCurrentTime-playerLastSaberMove[attacker].lastSaberMoveChange);
							thisKill.speedatSaberMoveChange = isWorldKill ? -1 : (playerLastSaberMove[attacker].speed);


							// This is the place that had all the continues originally.
							
							//entityState_t* attackerEntity = findEntity(attacker);

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
											else {
												// It's a special attack.
												// Save speed at which it was executed.
												modInfo << "_" << (int)thisKill.speedatSaberMoveChange << "u";
											}
										}
										else {

											if (attackerEntity) {
												modInfo << saberMoveNames[attackerEntity->saberMove];
												if (!modInfo.str().size()) {
													// THIS IS INVALID!
													int saberStance = attackerEntity->fireflag;
													if (saberStance == 0) {
														// Server removed this info to prevent cheating I guess...
														// Let's make a guess.
														byte stanceProbability = 0; // 0-100
														int likelyStance = getLikelyStanceFromTorsoAnim(attackerEntity->torsoAnim, demoType, &stanceProbability);
														if (stanceProbability == 100) {
															modInfo << saberStyleNames[likelyStance] << "_G";
														}
														else {
															modInfo << saberStyleNames[likelyStance] << "_G" << (int)stanceProbability;
														}
													}
													else {

														modInfo << saberStyleNames[saberStance];
													}
													
												}
												else {
													// It's a special attack.
													// Save speed at which it was executed.
													modInfo <<"_" << (int)thisKill.speedatSaberMoveChange<<"u";
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
							
							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "mapname");
							std::string serverName = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "sv_hostname");
							std::string playername = "WEIRDATTACKER";
							const char* playerInfo;
							if (attacker >= 0 && attacker < MAX_CLIENTS) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + attacker];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "n");
							}
							offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + target];
							playerInfo = demo.cut.Cl.gameState.stringData + offset;
							std::string victimname = Info_ValueForKey(playerInfo,sizeof(demo.cut.Cl.gameState.stringData)- offset, "n");



							int startTime = demoCurrentTime - bufferTime;
							int endTime = demoCurrentTime + bufferTime;
							int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
							bool isTruncated = false;
							if (earliestPossibleStart > startTime) {
								startTime = earliestPossibleStart;
								isTruncated = true;
							}

							float maxSpeedAttackerFloat = getMaxSpeedForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);
							int maxSpeedAttacker = maxSpeedAttackerFloat;


							// Create hash.
							std::stringstream hashss;
							hashss << playername << "_" << victimname << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
							auto sha3_512 = picosha3::get_sha3_generator<224>();
							std::string hash_hex_string = sha3_512.get_hex_string(hashss.str());
							std::string shorthash = hash_hex_string.substr(0,3);

							// Ok now with knowing the hash stuff, save into kills list
							thisKill.hashSourceString = hashss.str();
							thisKill.hash = hash_hex_string;
							thisKill.modInfoString = modInfo.str();
							thisKill.attackerName = playername;
							thisKill.victimName = victimname;


							// Find nearby players.
							std::stringstream nearbyPlayersSS;
							int nearbyPlayersCount = 0;
							for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
								entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
								if (thisEntitySub->number >= 0 && thisEntitySub->number < MAX_CLIENTS && thisEntitySub->number != attacker && thisEntitySub->number != target) {
									float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, thisEs->pos.trBase);
									if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
										nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " ("<<(int)nearbyPlayerDistance<<")";
										thisKill.nearbyPlayers.push_back({ thisEntitySub->number,nearbyPlayerDistance });
									}
								}
							}
							if (demo.cut.Cl.snap.ps.clientNum != target && demo.cut.Cl.snap.ps.clientNum != attacker) {
								float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, thisEs->pos.trBase);
								if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
									nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
									thisKill.nearbyPlayers.push_back({ demo.cut.Cl.snap.ps.clientNum,nearbyPlayerDistance });
								}
							}
							std::string nearbyPlayersString = nearbyPlayersSS.str();


							kills[attacker].push_back(thisKill);


							std::stringstream logModStringSS;
							logModStringSS << (victimIsFlagCarrier ? "RET" : "MOD") << modInfo.str();
							std::string logModString = logModStringSS.str();
							const char* modString = logModString.c_str();

							// Log the kill.
							SQLBIND_TEXT(insertStatement, "@hash", hash_hex_string.c_str());
							SQLBIND_TEXT(insertStatement, "@shorthash", shorthash.c_str());
							SQLBIND_TEXT(insertStatement, "@map", mapname.c_str());
							SQLBIND_TEXT(insertStatement, "@serverName", serverName.c_str());
							SQLBIND_TEXT(insertStatement, "@killerName", playername.c_str());
							SQLBIND_TEXT(insertStatement, "@victimName", victimname.c_str());
							if (isWorldKill) {
								SQLBIND_NULL(insertStatement, "@killerTeam");
							}
							else {
								SQLBIND(insertStatement, int, "@killerTeam", playerTeams[attacker]);
							}
							SQLBIND(insertStatement, int, "@victimTeam", playerTeams[target]);
							SQLBIND(insertStatement, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND(insertStatement, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							if (victimIsFlagCarrier) {
								SQLBIND(insertStatement, int, "@otherFlagStatus",playerTeams[target] == TEAM_BLUE ? cgs.redflag : cgs.blueflag);
							}
							else {
								SQLBIND_NULL(insertStatement, "@otherFlagStatus");
							}
							SQLBIND(insertStatement, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertStatement, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND(insertStatement, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount +  teamInfo[TEAM_BLUE].playerCount + teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertStatement, int, "@killerClientNum", attacker);
							SQLBIND(insertStatement, int, "@victimClientNum", target);
							SQLBIND(insertStatement, int, "@isDoomKill", isDoomKill);
							SQLBIND(insertStatement, int, "@isExplosion", thisKill.isExplosion);
							SQLBIND(insertStatement, int, "@isSuicide", isSuicide);
							SQLBIND(insertStatement, int, "@meansOfDeath", mod);
							SQLBIND(insertStatement, double, "@positionX", thisEs->pos.trBase[0]);
							SQLBIND(insertStatement, double, "@positionY", thisEs->pos.trBase[1]);
							SQLBIND(insertStatement, double, "@positionZ", thisEs->pos.trBase[2]);

							int queryResult = sqlite3_step(insertStatement);
							if (queryResult != SQLITE_DONE) {
								std::cout << "Error inserting kill into database: " << sqlite3_errmsg(killDb) << "\n";
							}
							sqlite3_reset(insertStatement);


							SQLBIND_TEXT(insertAngleStatement, "@hash", hash_hex_string.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@shorthash", shorthash.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@map", mapname.c_str());
							SQLBIND(insertAngleStatement, int, "@isReturn", victimIsFlagCarrier);
							SQLBIND(insertAngleStatement, int, "@killerIsFlagCarrier", attackerIsFlagCarrier);
							if (victimIsFlagCarrier) {
								SQLBIND(insertAngleStatement, int, "@victimFlagHoldTime", recentFlagHoldTimes[target]);
								//SQLBIND(insertAngleStatement, int, "@flagPickupSource", teamInfo[victimFlagTeam].flagHoldOrigin);
								SQLBIND(insertAngleStatement, int, "@victimFlagPickupSource", victimCarrierLastPickupOrigin);
								SQLBIND(insertAngleStatement, int, "@victimCapperKills", victimFlagCarrierKillCount);
								SQLBIND(insertAngleStatement, int, "@victimCapperRets", victimFlagCarrierRetCount);
								SQLBIND(insertAngleStatement, int, "@victimCapperWasFollowedOrVisible", capperWasVisibleOrFollowed);
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@victimFlagHoldTime");
								SQLBIND_NULL(insertAngleStatement, "@victimFlagPickupSource");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperKills");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperRets");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperWasFollowedOrVisible");
							}
							SQLBIND(insertAngleStatement, int, "@targetIsVisible", targetIsVisible);
							SQLBIND(insertAngleStatement, int, "@targetIsFollowed", targetIsFollowed);
							SQLBIND(insertAngleStatement, int, "@targetIsFollowedOrVisible", targetIsVisibleOrFollowed);
							SQLBIND(insertAngleStatement, int, "@isSuicide", isSuicide);
							SQLBIND(insertAngleStatement, int, "@attackerIsVisible", attackerIsVisible);
							SQLBIND(insertAngleStatement, int, "@attackerIsFollowed", attackerIsFollowed);
							SQLBIND(insertAngleStatement, int, "@attackerIsFollowedOrVisible", attackerIsVisibleOrFollowed);
							SQLBIND(insertAngleStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
							SQLBIND(insertAngleStatement, double, "@maxSpeedAttacker", maxSpeedAttackerFloat >= 0 ? maxSpeedAttackerFloat : NULL);
							SQLBIND(insertAngleStatement, double, "@maxSpeedTarget", maxSpeedTargetFloat >= 0 ? maxSpeedTargetFloat : NULL);
							SQLBIND(insertAngleStatement, double, "@lastSaberMoveChangeSpeed", thisKill.speedatSaberMoveChange >= 0 ? thisKill.speedatSaberMoveChange : NULL);
							SQLBIND(insertAngleStatement, int, "@timeSinceLastSaberMoveChange", thisKill.timeSinceSaberMoveChange >= 0 ? thisKill.timeSinceSaberMoveChange : NULL);
							SQLBIND_TEXT(insertAngleStatement, "@meansOfDeathString", modString);
							SQLBIND_TEXT(insertAngleStatement, "@nearbyPlayers", thisKill.nearbyPlayers.size() > 0? nearbyPlayersString.c_str():NULL);
							SQLBIND(insertAngleStatement, int, "@nearbyPlayerCount", thisKill.nearbyPlayers.size());
							SQLBIND(insertAngleStatement, int, "@probableKillingWeapon", probableKillingWeapon);
							if (attackerIsFollowed) {
								SQLBIND(insertAngleStatement, double, "@directionX", demo.cut.Cl.snap.ps.velocity[0]);
								SQLBIND(insertAngleStatement, double, "@directionY", demo.cut.Cl.snap.ps.velocity[1]);
								SQLBIND(insertAngleStatement, double, "@directionZ", demo.cut.Cl.snap.ps.velocity[2]);
								SQLBIND(insertAngleStatement, double, "@currentSpeedAttacker", VectorLength(demo.cut.Cl.snap.ps.velocity));
							} else if(attackerEntity){
								SQLBIND(insertAngleStatement, double, "@directionX", attackerEntity->pos.trDelta[0]);
								SQLBIND(insertAngleStatement, double, "@directionY", attackerEntity->pos.trDelta[1]);
								SQLBIND(insertAngleStatement, double, "@directionZ", attackerEntity->pos.trDelta[2]);
								SQLBIND(insertAngleStatement, double, "@currentSpeedAttacker", VectorLength(attackerEntity->pos.trDelta));
							} else {
								SQLBIND_NULL(insertAngleStatement,  "@directionX");
								SQLBIND_NULL(insertAngleStatement,  "@directionY");
								SQLBIND_NULL(insertAngleStatement,  "@directionZ");
								SQLBIND_NULL(insertAngleStatement,  "@currentSpeedAttacker");
							}
							if (targetIsFollowed) {
								SQLBIND(insertAngleStatement, double, "@currentSpeedTarget", VectorLength(demo.cut.Cl.snap.ps.velocity));
							}
							else if (targetEntity) {
								SQLBIND(insertAngleStatement, double, "@currentSpeedTarget", VectorLength(targetEntity->pos.trDelta));
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@currentSpeedTarget");
							}
							SQLBIND_TEXT(insertAngleStatement, "@demoName", oldBasename.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@demoPath", oldPath.c_str());
							SQLBIND(insertAngleStatement, int, "@demoTime", demoCurrentTime);
							SQLBIND(insertAngleStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND(insertAngleStatement, int, "@demoDateTime", oldDemoDateModified);

							queryResult = sqlite3_step(insertAngleStatement);
							if (queryResult != SQLITE_DONE) {
								std::cout<< "Error inserting kill angle into database: " << sqlite3_errmsg(killDb) <<"\n";
							}
							sqlite3_reset(insertAngleStatement);



							//if (isSuicide || !victimIsFlagCarrier || isWorldKill || !targetIsVisible) continue; // Not that interesting.
							if (isSuicide || !victimIsFlagCarrier || isWorldKill || !targetIsVisibleOrFollowed) continue; // Not that interesting.
							// If it's not a doom kill, it's not that interesting unless we specifically are searching for our own returns or searching for everything
							if (!isDoomKill && searchMode != SEARCH_ALL && searchMode != SEARCH_MY_CTF_RETURNS && searchMode != SEARCH_CTF_RETURNS) continue;
							if (!attackerIsFollowed && searchMode == SEARCH_MY_CTF_RETURNS) continue; // We are searching for our own kills.
							if (!attackerIsVisibleOrFollowed) continue; // Attacker isn't visible at all. Meh. Leave it.


							std::stringstream ss;
							ss << mapname << std::setfill('0') << "___RET" << modInfo.str() << "___" << playername << "___" << victimname<<"___" << maxSpeedAttacker<<"_" << maxSpeedTarget <<"ups" << (attackerIsFollowed ? "" : "___thirdperson") << "_" << attacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? "_tr" : "")<<"_"<<shorthash;

							std::string targetFilename = ss.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


							outputBatHandle << "\nrem demoCurrentTime: " << demoCurrentTime;
							outputBatHandle << "\nrem hash: " << hash_hex_string;
							outputBatHandle << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
							delete[] targetFilenameFiltered;
							std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "___" << maxSpeedAttacker << "_" << maxSpeedTarget << "ups" << "\n";

						}
					
						else if (eventNumber == EV_CTFMESSAGE && thisEs->eventParm == CTFMESSAGE_PLAYER_GOT_FLAG) {
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;
							// A bit pointless tbh because we reset it to -1 anyway before checking entities. 
							// Let me rethink this some day TODO
							if (flagTeam == TEAM_RED) {
								lastKnownRedFlagCarrier = playerNum;
							}else if (flagTeam == TEAM_BLUE) {
								lastKnownBlueFlagCarrier = playerNum;
							}
						}
						else if (eventNumber == EV_CTFMESSAGE && thisEs->eventParm == CTFMESSAGE_PLAYER_CAPTURED_FLAG) {
							//Capture.
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;

							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "mapname");
							std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "sv_hostname");
							std::string playername = "WEIRDONAME";
							const char* playerInfo;
							if (playerNum >= 0 && playerNum < MAX_CLIENTS) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + playerNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "n");
							}

							bool playerIsVisible = false;
							entityState_t* playerEntity = findEntity(playerNum);
							if (playerEntity) {
								playerIsVisible = true;
							}
							bool playerIsFollowed = demo.cut.Cl.snap.ps.clientNum == playerNum;
							bool playerIsVisibleOrFollowed = playerIsFollowed || playerIsVisible;

							int flagHoldTime = recentFlagHoldTimes[playerNum];

							bool wasFollowed = false;
							bool wasVisible = false;
							bool wasVisibleOrFollowed = false;
							if (playerNum != -1) {

								if (playerFirstFollowed[playerNum] != -1 && playerFirstFollowed[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasFollowed = true;
								}
								if (playerFirstVisible[playerNum] != -1 && playerFirstVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisible = true;
								}
								if (playerFirstFollowedOrVisible[playerNum] != -1 && playerFirstFollowedOrVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisibleOrFollowed = true;
								}
							}

							float maxSpeedCapperFloat = getMaxSpeedForClientinTimeFrame(playerNum, demoCurrentTime - 1000, demoCurrentTime);

							// See if he had any kills/rets as carrier. (did he fight valiantly before he died?)
							if (recentKillsDuringFlagHold[playerNum].lastKillDemoTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentKillsDuringFlagHold[playerNum], 0, sizeof(CapperKillsInfo));
							}
							int flagCarrierKillCount = recentKillsDuringFlagHold[playerNum].kills;
							int flagCarrierRetCount = recentKillsDuringFlagHold[playerNum].rets;

							vec3_t currentPos;
							vec3_t currentDir;

							// Find nearby players.
							std::stringstream nearbyPlayersSS;
							std::vector<int> nearbyPlayers;
							std::vector<int> nearbyPlayersDistances;
							int nearbyPlayersCount = 0;
							if (playerIsVisibleOrFollowed) {
								if (playerEntity) {
									VectorCopy(playerEntity->pos.trBase,currentPos); // This is also useful in general.
									VectorCopy(playerEntity->pos.trDelta, currentDir); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPos);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDir);
								}
								for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
									entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
									if (thisEntitySub->number >= 0 && thisEntitySub->number < MAX_CLIENTS && thisEntitySub->number != playerNum) {
										float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, currentPos);
										if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
											nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " (" << (int)nearbyPlayerDistance << ")";
											nearbyPlayers.push_back(thisEntitySub->number);
											nearbyPlayersDistances.push_back(nearbyPlayerDistance);
										}
									}
								}
								if (demo.cut.Cl.snap.ps.clientNum != playerNum) {
									float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, currentPos);
									if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
										nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
										nearbyPlayers.push_back(demo.cut.Cl.snap.ps.clientNum);
										nearbyPlayersDistances.push_back(nearbyPlayerDistance);
									}
								}
							}
							std::string nearbyPlayersString = nearbyPlayersSS.str();

							// Find nearby enemies
							std::stringstream nearbyEnemiesSS;
							int nearbyEnemiescount = 0;
							for (int near = 0; near < nearbyPlayers.size(); near++) {
								int nearbyPlayerHere = nearbyPlayers[near];
								if (playerTeams[nearbyPlayerHere] != playerTeams[playerNum]) {
									nearbyEnemiesSS << (nearbyEnemiescount++ == 0 ? "" : ",") << nearbyPlayerHere << " (" << (int)nearbyPlayersDistances[near] << ")";
								}
							}
							std::string nearbyEnemiesString = nearbyEnemiesSS.str();

							int victimCarrierLastPickupOrigin = -1;// victimIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? cgs.blueFlagLastPickupOrigin : cgs.redFlagLastPickupOrigin) : -1;
							switch (flagTeam) {
								case TEAM_FREE: // Is this correct?
									victimCarrierLastPickupOrigin = cgs.yellowflagLastPickupOrigin;
									break;
								case TEAM_RED:
									victimCarrierLastPickupOrigin = cgs.redFlagLastPickupOrigin;
									break;
								case TEAM_BLUE:
									victimCarrierLastPickupOrigin = cgs.blueFlagLastPickupOrigin;
									break;
							}

							SQLBIND_TEXT(insertCaptureStatement, "@map", mapname.c_str());
							SQLBIND_TEXT(insertCaptureStatement, "@serverName", serverName.c_str());
							SQLBIND(insertCaptureStatement, int, "@flagHoldTime", flagHoldTime);
							//SQLBIND(insertCaptureStatement, int, "@flagPickupSource", teamInfo[flagTeam].flagHoldOrigin);
							SQLBIND(insertCaptureStatement, int, "@flagPickupSource", victimCarrierLastPickupOrigin);
							SQLBIND_TEXT(insertCaptureStatement, "@capperName", playername.c_str());
							SQLBIND(insertCaptureStatement, int, "@capperClientNum", playerNum);
							SQLBIND(insertCaptureStatement, int, "@capperIsVisible", playerIsVisible);
							SQLBIND(insertCaptureStatement, int, "@capperIsFollowed", playerIsFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperIsFollowedOrVisible", playerIsVisibleOrFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperWasVisible", wasVisible);
							SQLBIND(insertCaptureStatement, int, "@capperWasFollowed", wasFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperWasFollowedOrVisible", wasVisibleOrFollowed);
							SQLBIND(insertCaptureStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
							SQLBIND(insertCaptureStatement, int, "@flagTeam", flagTeam);
							SQLBIND(insertCaptureStatement, int, "@capperKills", flagCarrierKillCount);
							SQLBIND(insertCaptureStatement, int, "@capperRets", flagCarrierRetCount);
							SQLBIND(insertCaptureStatement, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND(insertCaptureStatement, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							SQLBIND(insertCaptureStatement, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertCaptureStatement, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND(insertCaptureStatement, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount + teamInfo[TEAM_BLUE].playerCount+teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertCaptureStatement, double, "@maxSpeedCapper", maxSpeedCapperFloat);
							SQLBIND_TEXT(insertCaptureStatement, "@nearbyPlayers", nearbyPlayersString.c_str());
							SQLBIND(insertCaptureStatement, int, "@nearbyPlayerCount", nearbyPlayersCount);
							SQLBIND_TEXT(insertCaptureStatement, "@nearbyEnemies", nearbyEnemiesString.c_str());
							SQLBIND(insertCaptureStatement, int, "@nearbyEnemyCount", nearbyEnemiescount);
							if (playerIsVisibleOrFollowed) {
								SQLBIND(insertCaptureStatement, double, "@positionX", currentPos[0]);
								SQLBIND(insertCaptureStatement, double, "@positionY", currentPos[1]);
								SQLBIND(insertCaptureStatement, double, "@positionZ", currentPos[2]);
								SQLBIND(insertCaptureStatement, double, "@directionX", currentDir[0]);
								SQLBIND(insertCaptureStatement, double, "@directionY", currentDir[1]);
								SQLBIND(insertCaptureStatement, double, "@directionZ", currentDir[2]);
							}
							else {
								SQLBIND_NULL(insertCaptureStatement,  "@positionX");
								SQLBIND_NULL(insertCaptureStatement,  "@positionY");
								SQLBIND_NULL(insertCaptureStatement,  "@positionZ");
								SQLBIND_NULL(insertCaptureStatement,  "@directionX");
								SQLBIND_NULL(insertCaptureStatement,  "@directionY");
								SQLBIND_NULL(insertCaptureStatement,  "@directionZ");
							}
							SQLBIND_TEXT(insertCaptureStatement, "@demoName", oldBasename.c_str());
							SQLBIND_TEXT(insertCaptureStatement, "@demoPath", oldPath.c_str());
							SQLBIND(insertCaptureStatement, int, "@demoTime", demoCurrentTime);
							SQLBIND(insertCaptureStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND(insertCaptureStatement, int, "@demoDateTime", oldDemoDateModified);

							int queryResult = sqlite3_step(insertCaptureStatement);
							if (queryResult != SQLITE_DONE) {
								std::cout << "Error inserting capture into database: " << sqlite3_errmsg(killDb) << "\n";
							}
							sqlite3_reset(insertCaptureStatement);

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
					static std::vector<Kill> killsOfThisSpree;
					static std::vector<int> victims;
					static std::vector<std::string> hashes;
					victims.reserve(10);
					hashes.reserve(10);
					killsOfThisSpree.reserve(10);
					victims.clear();
					hashes.clear();
					killsOfThisSpree.clear();

					int clientNumAttacker = clientIt->first;
					SpreeInfo spreeInfo;
					Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
					std::stringstream allKillsHashSS;

					if (clientIt->second.size() == 0 || (clientIt->second.back().time + KILLSTREAK_MAX_INTERVAL) >= demoCurrentTime) continue; // Might still have an unfinished one here!

					int lastKillTime = 0;

					for (int i = 0; i < clientIt->second.size(); i++) {
						Kill* thisKill = &clientIt->second[i];

						// Whether it was or wasn't a killstreak, there are no killstreaks up to this point, so earlier than current kill time can be safely cleaned up.
						timeCheckedForKillStreaks[clientNumAttacker] = thisKill->time - 1;

						if (thisKill->time <= timeCheckedForKillStreaks[clientNumAttacker]) continue; // This one's already been registered.
						if (thisKill->isSuicide || !thisKill->isVisible) continue; // Uninteresting.


						// Starting or continuing kill spree?
						if (spreeInfo.countKills == 0 || thisKill->time <= (spreeInfo.lastKillTime + KILLSTREAK_MAX_INTERVAL)) {
							
							victims.push_back(thisKill->targetClientNum);
							hashes.push_back(thisKill->hash);
							killsOfThisSpree.push_back(*thisKill);
							if (thisKill->isDoom) spreeInfo.countDooms++;
							if (thisKill->isRet) spreeInfo.countRets++;
							if (thisKill->isExplosion) spreeInfo.countExplosions++;
							if (!thisKill->isFollowed) spreeInfo.countThirdPersons++;
							spreeInfo.totalTime += spreeInfo.countKills == 0? 0: (thisKill->time - spreeInfo.lastKillTime);
							spreeInfo.countKills++;
							spreeInfo.lastKillTime = thisKill->time;
							spreeInfo.maxVictimSpeed = std::max(spreeInfo.maxVictimSpeed,thisKill->victimMaxSpeedPastSecond);
							allKillsHashSS << thisKill->hashSourceString;
						}
						else {
							// This kill is not part of a killspree. Reset.
							// But first, check if this concludes an existing killspree that we can now save.
							CheckSaveKillstreak(&spreeInfo, clientNumAttacker,&killsOfThisSpree,&victims, &hashes, allKillsHashSS.str(),demoCurrentTime,&outputBatHandle,bufferTime,lastGameStateChangeInDemoTime,sourceDemoFile, insertSpreeStatement, killDb, oldBasename,oldPath, oldDemoDateModified);

							// Reset.
							Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
							victims.clear();
							hashes.clear();
							killsOfThisSpree.clear();
							allKillsHashSS.str(std::string());
						}
					}
					CheckSaveKillstreak(&spreeInfo, clientNumAttacker,&killsOfThisSpree,&victims, &hashes, allKillsHashSS.str(), demoCurrentTime, &outputBatHandle, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile, insertSpreeStatement,killDb, oldBasename,oldPath, oldDemoDateModified);
				
					// Clean up old kills that no longer have to be stored
					// We can clear the entire thing since earlier we made sure that we aren't in the middle of an ongoing killstreak
					// So anything that's in there now was already checked for being part of a killstreak.
					// So we can get rid of it all.
					clientIt->second.clear();
				
					// Clean up old speeds
					// We do need to keep speeds of at least OLDER_SPEEDS_STORE_LIMIT because we might have to check past speeds for future kills that aren't logged yet.
					// So let's find the last speed we're erasing
					/*std::map<int, float>::iterator lastToRemove;
					int countToRemove = 0;
					for (auto speedsIt = speeds[clientNumAttacker].begin(); speedsIt != speeds[clientNumAttacker].end(); speedsIt++) {
						if ((demoCurrentTime - speedsIt->first) > OLDER_SPEEDS_STORE_LIMIT) {
							lastToRemove = speedsIt;
							countToRemove++;
						}
					}
					if (countToRemove) {
						speeds[clientNumAttacker].erase(speeds[clientNumAttacker].begin(), lastToRemove);
					}*/
					int countToRemove = 0;
					for (int s = 0; s < speeds[clientNumAttacker].size();s++) {
						if ((demoCurrentTime - speeds[clientNumAttacker][s].time) > OLDER_SPEEDS_STORE_LIMIT) {
							countToRemove++;
						}
					}
					if (countToRemove) {
						speeds[clientNumAttacker].erase(speeds[clientNumAttacker].begin(), speeds[clientNumAttacker].begin()+ countToRemove);
					}
				}


				// Find out which players are visible / followed
				// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
				lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
				for (int p = 0; p < MAX_CLIENTS; p++) {
					// Go through parseenttities of last snap to see if client is in it
					bool clientIsInSnapshot = false;
					bool clientVisibleOrFollowed = false;
					// TODO This is kind of messy uuuh... fix it?!
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum+demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						if (thisEntity->number == p) {
							clientIsInSnapshot = true;

							if (thisEntity->powerups & (1 << PW_REDFLAG)) {
								lastKnownRedFlagCarrier = thisEntity->number;
								recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime - cgs.redFlagLastChangeToTaken;
							}
							else if (thisEntity->powerups & (1 << PW_BLUEFLAG)) {
								lastKnownBlueFlagCarrier = thisEntity->number;
								recentFlagHoldTimes[lastKnownBlueFlagCarrier] = demoCurrentTime - cgs.blueFlagLastChangeToTaken;
							}
						}
					}
					if (demo.cut.Cl.snap.ps.powerups[PW_REDFLAG]) {

						lastKnownRedFlagCarrier = demo.cut.Cl.snap.ps.clientNum;
						recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime - cgs.redFlagLastChangeToTaken;
					}else if (demo.cut.Cl.snap.ps.powerups[PW_BLUEFLAG]) {

						lastKnownBlueFlagCarrier = demo.cut.Cl.snap.ps.clientNum;
						recentFlagHoldTimes[lastKnownBlueFlagCarrier] = demoCurrentTime - cgs.blueFlagLastChangeToTaken;
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

		bool hadConfigStringCommands = false;
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

				int index = atoi(Cmd_Argv(1));
				if (index == CS_FLAGSTATUS) {
					char* str = Cmd_Argv(2);

					int redflagTmp, blueflagTmp, yellowflagTmp;
					// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
					if (strlen(str) >= 2) {
						redflagTmp = str[0] - '0';
						blueflagTmp = str[1] - '0';
					}
					else { // This is some weird bug/imperfection in the code. Sometimes it just sends cs 23 0 for whatever reason. Seems to happen at end of games.
						redflagTmp = 0;
						blueflagTmp = 0;
					}
					if (strlen(str) >= 3) { // Too lazy to do other way lol.
						yellowflagTmp = str[2] - '0';
					}
					else {
						yellowflagTmp = 0;
					}

					if (cgs.redflag != redflagTmp) {
						/* // Actually, we can't do this here bc lastknownredflagcarrier might not be actually known.
						if (redflagTmp == 1) { // Was just picked up. Set flag hold time to 0.
							recentFlagHoldTimes[lastKnownRedFlagCarrier] = 0;
						}
						if (cgs.redflag == 1) { // Was taken before. So just count the flag hold time.
							recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime-cgs.redFlagLastChange;
						}*/
						cgs.redFlagLastChange = demoCurrentTime;
						if (redflagTmp == 1) {
							cgs.redFlagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_RED].flagHoldOrigin = cgs.redflag;
							cgs.redFlagLastPickupOrigin = cgs.redflag;
						}
					}
					if (cgs.blueflag != blueflagTmp) {
						cgs.blueFlagLastChange = demoCurrentTime;
						if (blueflagTmp == 1) {
							cgs.blueFlagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_BLUE].flagHoldOrigin = cgs.blueflag;
							cgs.blueFlagLastPickupOrigin = cgs.blueflag;
						}
					}
					if (cgs.yellowflag != yellowflagTmp) {
						cgs.yellowflagLastChange = demoCurrentTime;
						if (yellowflagTmp == 1) {
							cgs.yellowflagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_FREE].flagHoldOrigin = cgs.yellowflag; // Not sure if this is correct?
							cgs.yellowflagLastPickupOrigin = cgs.yellowflag; // Not sure if this is correct?
						}
					}
					cgs.redflag = redflagTmp;
					cgs.blueflag = blueflagTmp;
					cgs.yellowflag = yellowflagTmp;
					/*if (cgs.isCTFMod && cgs.CTF3ModeActive)
						demo.cgs.yellowflag = str[2] - '0';
					else
						demo.cgs.yellowflag = 0;*/
				}

				hadConfigStringCommands = true;
			}
			if (!strcmp(cmd, "print")) {
				//Looking for 
				//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."

				// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.
				

				jp::VecNum vec_num;
				jp::RegexMatch rm;
				std::string printText = Cmd_Argv(1);
#if DEBUG
				//std::cout << printText << "\n";
#endif

				size_t count = rm.setRegexObject(&defragRecordFinishRegex)                          //set associated Regex object
					.setSubject(&printText)                         //set subject string
					.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
					.match();
				
				for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
					int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
					const char * info = demo.cut.Cl.gameState.stringData + stringOffset;
					std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)- stringOffset, "mapname");
					std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");
					std::string playername = vec_num[matchNum][1];
					int minutes = atoi(vec_num[matchNum][2].c_str());
					std::string secondString = vec_num[matchNum][3];
					float seconds = atof(vec_num[matchNum][3].c_str());
					int milliSeconds = (1000.0f* seconds)+0.5f;
					int pureMilliseconds = milliSeconds % 1000;
					int pureSeconds = milliSeconds / 1000;

					bool isLogged = vec_num[matchNum][4].length() > 0;
					bool isNumberOne = vec_num[matchNum][6].length() > 0;

					//int totalSeconds = minutes * 60 + seconds;
					int totalMilliSeconds = minutes * 60000 + milliSeconds;
					
					// Find player
					int playerNumber = -1;
					for (int clientNum = 0; clientNum < MAX_CLIENTS; clientNum++) {

						int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + clientNum];
						const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
						std::string playerNameCompare = Info_ValueForKey(playerInfo,sizeof(demo.cut.Cl.gameState.stringData)- stringOffset, "n");
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

					std::stringstream formattedTime;
					formattedTime << std::setfill('0') << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds;
					std::string formattedTimeString = formattedTime.str();

					SQLBIND_TEXT(insertDefragRunStatement, "@map", mapname.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@serverName", serverName.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@readableTime", formattedTimeString.c_str());
					SQLBIND(insertDefragRunStatement, int, "@totalMilliseconds", totalMilliSeconds);
					SQLBIND_TEXT(insertDefragRunStatement, "@playerName", playername.c_str());
					SQLBIND(insertDefragRunStatement, int, "@isTop10", isLogged);
					SQLBIND(insertDefragRunStatement, int, "@isNumber1", isNumberOne);
					SQLBIND_TEXT(insertDefragRunStatement, "@demoName", oldBasename.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@demoPath", oldPath.c_str());
					SQLBIND(insertDefragRunStatement, int, "@demoTime", demoCurrentTime);
					SQLBIND(insertDefragRunStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
					SQLBIND(insertDefragRunStatement, int, "@demoDateTime", oldDemoDateModified);
					SQLBIND(insertDefragRunStatement, int, "@wasVisible", wasVisible);
					SQLBIND(insertDefragRunStatement, int, "@wasFollowed", wasFollowed);
					SQLBIND(insertDefragRunStatement, int, "@wasFollowedOrVisible", wasVisibleOrFollowed);
					SQLBIND(insertDefragRunStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
					SQLBIND(insertDefragRunStatement, int, "@runnerClientNum", playerNumber);

					int queryResult = sqlite3_step(insertDefragRunStatement);
					if (queryResult != SQLITE_DONE) {
						std::cout << "Error inserting defrag run into database: " << sqlite3_errmsg(killDb) << "\n";
					}
					sqlite3_reset(insertDefragRunStatement);

					//if (searchMode != SEARCH_INTERESTING && searchMode != SEARCH_ALL && searchMode != SEARCH_TOP10_DEFRAG) continue;
					//if (!isNumberOne && (searchMode != SEARCH_TOP10_DEFRAG || !isLogged)) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.
					if (!isNumberOne && (/*searchMode != SEARCH_TOP10_DEFRAG || */!isLogged)) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.
					

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

					outputBatHandleDefrag << "\nrem demoCurrentTime: "<< demoCurrentTime;
					outputBatHandleDefrag << "\n"<< (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \""<<sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
					delete[] targetFilenameFiltered;
					std::cout << mapname << " " << playerNumber << " " << playername << " " << minutes << ":" << secondString << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << wasFollowed << " visible:" << wasVisible << " visibleOrFollowed:" << wasVisibleOrFollowed << "\n";
				}

				
				
			}
		}

		if (hadConfigStringCommands) {
			CheckForNameChanges(&demo.cut.Cl, killDb, insertPlayerModelStatement, updatePlayerModelCountStatement);
			setPlayerAndTeamData(&demo.cut.Cl);
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


#ifdef DEBUGSTATSDB
	for (auto it = animStanceCounts.begin(); it != animStanceCounts.end(); it++) {

		/*SQLBIND(insertAnimStanceStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(insertAnimStanceStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(insertAnimStanceStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(insertAnimStanceStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(insertAnimStanceStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);
		SQLBIND(updateAnimStanceCountStatement, int, "@countFound", );
		SQLBIND(updateAnimStanceCountStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(updateAnimStanceCountStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(updateAnimStanceCountStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(updateAnimStanceCountStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(updateAnimStanceCountStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);*/
		SQLBIND(insertAnimStanceStatement, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@saberHolstered", std::get<1>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@torsoAnim", std::get<2>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@legsAnim", std::get<3>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@saberMove", std::get<4>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@stance", std::get<5>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@countFound", it->second);
		SQLBIND(updateAnimStanceCountStatement, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@saberHolstered", std::get<1>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@torsoAnim", std::get<2>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@legsAnim", std::get<3>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@saberMove", std::get<4>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@stance", std::get<5>(it->first));

		int queryResult = sqlite3_step(insertAnimStanceStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting anim stance into database: " << sqlite3_errmsg(debugStatsDb) << "\n";
		}
		sqlite3_reset(insertAnimStanceStatement);

		queryResult = sqlite3_step(updateAnimStanceCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error updating anim stance count in database: " << sqlite3_errmsg(debugStatsDb) << "\n";
		}
		sqlite3_reset(updateAnimStanceCountStatement);
	}



	sqlite3_exec(debugStatsDb, "COMMIT;", NULL, NULL, NULL);

	sqlite3_finalize(insertAnimStanceStatement);
	sqlite3_finalize(updateAnimStanceCountStatement);
	sqlite3_close(debugStatsDb);
#endif



	sqlite3_exec(killDb, "COMMIT;", NULL, NULL, NULL);
	sqlite3_finalize(insertDefragRunStatement);
	sqlite3_finalize(insertCaptureStatement);
	sqlite3_finalize(insertSpreeStatement);
	sqlite3_finalize(insertStatement);
	sqlite3_finalize(insertAngleStatement);
	sqlite3_finalize(insertPlayerModelStatement);
	sqlite3_finalize(updatePlayerModelCountStatement);
	sqlite3_close(killDb);

	FS_FCloseFile(oldHandle);
	//FS_FCloseFile(newHandle);

	std::cout << "\ndone." << "\n\n";


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

	Com_Printf("Looking at %s.\n", demoName);
	if (demoHighlightFind(demoName, bufferTime,"highlightExtractionScript.bat","highlightExtractionScriptDefrag.bat", searchMode)) {
		Com_Printf("Highlights successfully found.\n");
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