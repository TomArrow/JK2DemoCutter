#pragma once
#ifndef DEMOREADER_H
#define DEMOREADER_H

#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>

#define PLAYERSTATE_FUTURE_SEEK 60000

typedef jpcre2::select<char> jp;

enum highlightSearchMode_t {
	SEARCH_ALL,
	SEARCH_INTERESTING,
	SEARCH_MY_CTF_RETURNS,
	SEARCH_CTF_RETURNS,
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
};


class Command {
public:
	int demoTime;
	int serverTime;
	std::string command;
};


enum eventKind_t {
	EK_ENTITY,
	EK_PS_EXTERNAL,
	EK_PS_ARRAY,
	EK_ALL,
	EK_COUNT
};


class Event {
public:
#ifdef DEBUG
	int countGiven;
#endif
	int64_t demoTime;
	int serverTime;
	entityState_t theEvent;
	eventKind_t kind;
	int eventNumber;
};

class NameMatch {
public:
	std::string matchedName;
	int clientNum;
};

typedef std::map<int, SnapshotInfo> SnapshotInfoMap;
typedef SnapshotInfoMap::iterator SnapshotInfoMapIterator;

typedef enum dimensionDataType_t {
	DIM_OWNAGE
};

class DemoReader {

	std::string originalDemoPath = "";

	bool	isMOHAADemo = false;
	int		maxClientsThisDemo = MAX_CLIENTS;

	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
	jp::Regex defragRecordFinishRegex;
	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

	playerState_t basePlayerStates[MAX_CLIENTS_MAX]; // Contains stuff like team and max health and other stuff that otherwise messes up rendering.

	//std::map<int, int> playerFirstVisible;
	//std::map<int, int> playerFirstFollowed;
	//std::map<int, int> playerFirstFollowedOrVisible;
	//std::map<int, int> lastEventTime;
	int lastEventTime[MAX_GENTITIES]{};
	//std::map<int, int> lastEvent;
	int lastEvent[MAX_GENTITIES]{};


	int firstPacketWithPlayer[MAX_CLIENTS_MAX];
	int lastKnownPacketWithPlayer[MAX_CLIENTS_MAX];
	int firstPacketWithPlayerState[MAX_CLIENTS_MAX];
	int lastKnownPacketWithPlayerState[MAX_CLIENTS_MAX];

	int mohaaPlayerWeaponModelIndexThisFrame[MAX_CLIENTS_MAX];
	int mohaaPlayerWeaponModelIndex[MAX_CLIENTS_MAX];
	int mohaaPlayerWeapon[MAX_CLIENTS_MAX];

	struct {
		qboolean ownageExtraInfoMetaMarker = qfalse;
		qboolean dimensionInfoConfirmed = qfalse;
		dimensionDataType_t dimensionInfoType = (dimensionDataType_t )(-1);
		int g_entHUDieldsValue = 0;
		int64_t	cutStartOffset = 0;
		int64_t	truncationOffset = 0;
	} extraFieldInfo;

	SnapshotInfoMap snapshotInfos;
	std::vector<Command> readCommands;
	std::vector<Event> readEvents;

	int lastKnownRedFlagCarrier = -1;
	int lastKnownBlueFlagCarrier = -1;
	demo_t			thisDemo;

	// Info during reading
	demoType_t		demoType;
	qboolean		isCompressedFile;
	fileHandle_t	oldHandle = 0;
	int				oldSize;
	int				readGamestate = 0;

	int				demoStartTime = 0;
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int64_t			demoCurrentTime = 0;
	int				lastGameStateChange = 0;
	int				lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;
	int				lastKnownInOrderTime = 0;
	int				maxSequenceNum = -9999;
	std::map<int,int>	lastKnownCommandOrServerTimes; // For each clientnum
	std::map<int,int>	lastMessageWithEntity; 
	qboolean wasFirstCommandByte = qfalse;
	qboolean firstCommandByteRead = qfalse;
	rapidjson::Document* jsonSourceFileMetaDocument = NULL;

	playerState_t	oldPS[MAX_CLIENTS_MAX];

	double			lastGottenCommandsTime = 0;
	int				lastGottenCommandsServerTime = 0;
	double			lastGottenEventsTime[EK_COUNT] = { 0 };
	int				lastGottenEventsServerTime[EK_COUNT] = { 0 };

	qboolean		anySnapshotParsed = qfalse;
	qboolean		endReached = qfalse;


	//clSnapshot_t lastSnap;

	int messageOffset = 0;

	//qboolean ConfigstringModified(clientActive_t* clCut);
	int GetEvent(entityState_t* es);
	//void ParseCommandString(msg_t* msg, clientConnection_t* clcCut);
	//qboolean ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	//void ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType);
	qboolean ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	//void ParseRMG(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut);

	qboolean PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap);
	entityState_t* findEntity(int number);

	void InterpolatePlayerState(double time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS);
	void InterpolatePlayer(int clientNum, double time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS, qboolean detailedPS = qfalse);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	qboolean ReadMessage();

	qboolean ReadMessageReal();
	playerState_t GetPlayerFromSnapshot(int clientNum, int snapNum, SnapshotInfoMapIterator* playerStateSourceSnap = NULL, qboolean detailedPS = qfalse);
	playerState_t GetPlayerFromSnapshot(int clientNum, SnapshotInfoMapIterator snapInfoIterator, SnapshotInfoMapIterator* playerStateSourceSnap = NULL, qboolean detailedPS = qfalse);

	void updateConfigStringRelatedInfo();

	void generateBasePlayerStates();

public:

	void convertPSTo(playerState_t* ps, demoType_t targetDemoType);

	DemoReader::DemoReader() : defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi") {
	};

#ifdef DEBUG
	~DemoReader() {
		std::cout << "DemoReader being destroyed.";
	}
#endif

	int getMaxClients();

	int64_t getCutStartOffset(qboolean withTruncationOffset = qtrue);
	qboolean containsDimensionData();
	demoType_t getDemoType();
	bool isThisMOHAADemo();
	int getClientNumForDemo(std::string* playerSearchString, qboolean printEndLine = qfalse);
	int GetFirstSnapServerTime();
	qboolean SeekToTime(double time);
	int GetFirstServerTimeAfterServerTime(int serverTime);
	SnapshotInfoMapIterator SnapNullIt();
	SnapshotInfoMapIterator GetFirstSnapshotAfterSnapshotIterator(SnapshotInfoMapIterator oldSnapInfoIt);
	SnapshotInfoMapIterator GetFirstSnapshotAfterSnapshotIterator(SnapshotInfoMapIterator oldSnapInfoIt, int pastServerTime);
	int GetLastServerTimeBeforeServerTime(int serverTime);
	SnapshotInfo* GetSnapshotInfoAtOrBeforeDemoTime(float demoTime, qboolean includeAfterIfFirst = qtrue);
	SnapshotInfoMapIterator GetSnapshotInfoAtOrBeforeDemoTimeIterator(float demoTime, qboolean includeAfterIfFirst = qtrue);
	SnapshotInfo* GetSnapshotInfoAtServerTime(int serverTime);
	SnapshotInfoMapIterator GetSnapshotInfoAtServerTimeIterator(int serverTime);
	qboolean SeekToServerTime(int serverTime);
	qboolean SeekToCommandTime(int serverTime);
	qboolean SeekToPlayerInPacket(int clientNum); // Seek until we get a packet with this player
	qboolean SeekToPlayerCommandOrServerTime(int clientNum, int serverTime); // The reason this is called "OR servertime" is because player entities aren't guaranteed to have commandtime in them, they only have it if g_smoothclients is true.
	qboolean SeekToAnySnapshotIfNotYet();
	qboolean LoadDemo(const char* sourceDemoFile);
	qboolean CloseDemo();
	playerState_t GetCurrentPlayerState();
	playerState_t GetInterpolatedPlayerState(double time);
	playerState_t GetLastOrNextPlayer(int clientNum, int serverTime, SnapshotInfoMapIterator* usedSourceSnap=NULL,SnapshotInfoMapIterator* usedSourcePlayerStateSnap=NULL, qboolean detailedPS = qfalse, const SnapshotInfoMapIterator* referenceSnap = NULL);
	std::map<int, entityState_t> GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, const SnapshotInfoMapIterator* referenceSnap = NULL);
	void GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, std::map<int, entityState_t>* mapToEnhance, const SnapshotInfoMapIterator* referenceSnap = NULL); // Same as the other overload but enhances an existing map if item with lower serverTime is found.
	playerState_t GetInterpolatedPlayer(int clientNum, double time, SnapshotInfo** oldSnap=NULL, SnapshotInfo** newSnap=NULL, qboolean detailedPS = qfalse, float* translatedTime=NULL);
	std::map<int, entityState_t> DemoReader::GetCurrentEntities();
	std::map<int, entityState_t> DemoReader::GetEntitiesAtTime(double time, double * translatedTime);
	std::map<int, entityState_t> DemoReader::GetEntitiesAtPreciseTime(int time, qboolean includingPS);
	std::vector<std::string> DemoReader::GetNewCommands(double time);
	std::vector<std::string> DemoReader::GetNewCommandsAtServerTime(int serverTime);
	std::vector<Event> DemoReader::GetNewEvents(double time, eventKind_t kind=EK_ENTITY);
	std::vector<Event> DemoReader::GetNewEventsAtServerTime(int serverTime, eventKind_t kind=EK_ENTITY);
	clSnapshot_t GetCurrentSnap();
	const char* GetConfigString(int configStringNum, int* maxLength);
	const char* GetPlayerConfigString(int playerNum, int* maxLength);
	const char* GetModelConfigString(int modelNum, int* maxLength);
	const char* GetSoundConfigString(int soundNum, int* maxLength);
	qboolean AnySnapshotParsed();
	qboolean EndReached();
	qboolean EndReachedAtTime(double time);
	qboolean EndReachedAtServerTime(int serverTime);
	int getCurrentDemoTime();
	int getDemoRecorderClientNum();
};





void remapConfigStrings(entityState_t* tmpEntity, clientActive_t* clCut, DemoReader* reader, std::vector<std::string>* commandsToAdd, qboolean doModelIndex, qboolean doModelIndex2, demoType_t demoType);


class DemoSource {
public:
	std::string sourceName;
	std::string demoPath;
	std::vector<std::string> playersToCopy;
	double delay = 0.0f; // Absolute delay of the demo time against the target demo time
	double showStart = 0.0f; // In demo time
	double showEnd = std::numeric_limits<float>::infinity(); // In demo time
	double playerStateStart = 0.0f; // When to start considering this demo for playerState. Players will still be copied as entities otherwise.
	double playerStateEnd = std::numeric_limits<float>::infinity(); // When to stop considering this demo for playerState. Players will still be copied as entities otherwise.
	qboolean copyAllPlayers = qfalse;
	qboolean copyRemainingPlayersAsG2AnimEnts = qfalse;
	qboolean isFinished = qfalse;
	qboolean copySubmodels = qfalse;
	qboolean copyModels = qfalse;
};

struct SourcePlayerInfo {
	int clientNum;
	double pingCompensation; // in ms
	qboolean asG2AnimEnt;
};

class DemoReaderWrapper {
public:
	DemoReader reader;
	std::vector<SourcePlayerInfo> playersToCopy;
	DemoSource* sourceInfo;
	int targetFramesRead;
};






class SlotManager {
	struct sourceDemoMapping {
		int demoIndex;
		int entityNum;
		qboolean isModel;
	};

	const int maxAvailableSlots = MAX_GENTITIES - 2;
	int availableSlots = maxAvailableSlots;
	int lowestPossibleEntityNum = MAX_CLIENTS;
	std::map<int, sourceDemoMapping> mappings;
	typedef std::map<int, sourceDemoMapping>::iterator mappingIterator;
	vec3_t modelDistanceOrigin{};
public:
	SlotManager() {

	}
	SlotManager(int reservedEntities) {
		availableSlots = maxAvailableSlots - reservedEntities;
		lowestPossibleEntityNum = reservedEntities;
	}
	int getPlayerSlot(int demoIndex, int clientNum, qboolean* wasNewPlayer) {
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
				mappings[i] = { demoIndex,clientNum,qfalse };
				availableSlots--;
				if (wasNewPlayer) {
					*wasNewPlayer = qtrue;
				}
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
		for (int i = lowestPossibleEntityNum; i < MAX_GENTITIES - 2; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,entityNum,qfalse };
				availableSlots--;
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	void setModelDistanceOrigin(vec_t* originA) {
		VectorCopy(originA, modelDistanceOrigin);
	}
	// These are released as soon as they disappear from the demo. We don't need consistency from frame to frame on them. It's just random models/movers etc., they only need to be visible.
	int getModelSlot(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = lowestPossibleEntityNum; i < MAX_GENTITIES - 2; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,entityNum,qtrue };
				availableSlots--;
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	int getNormalPlayerSlotIfExists(int demoIndex, int entityNum) { // Since we're also copying some players as g2 anim ents. Don't wanna end up with a CG_OBITUARY with an attacker num of 50 lol.
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->first >= MAX_CLIENTS) break;
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
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
	std::vector<int> freeSlots(int demoIndex) { // Returns the erased slots
		std::vector<int> erasedSlots;
		int countErased = 0;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); ) {
			mappingIterator iteratorHere = it;
			it++;
			if (iteratorHere->second.demoIndex == demoIndex) {
				erasedSlots.push_back(iteratorHere->first);
				mappings.erase(iteratorHere);
				availableSlots++;
				countErased++;
			}
		}
		return erasedSlots;
	}
	/*std::vector<int> getModelMappings(int demoIndex) { // Returns the erased slots
		std::vector<int> modelSlots;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++; ) {

			if (it->second.demoIndex == demoIndex && it->second.isModel) {
				modelSlots.push_back(it->first);
			}
		}
		return modelSlots;
	}*/
	int freeSlot(int demoIndex, int entityNum) {
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				mappings.erase(it);
				availableSlots++;
				return 1;
			}
		}
		return 0;
	}
	qboolean isSlotFree(int slotNum) {
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->first == slotNum) return qfalse;
		}
		return qtrue;
	}
};


#endif