#pragma once
#ifndef DEMOREADER_H
#define DEMOREADER_H

#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>
#if USE_PMOVE
#include "CModel.h"
#include "PMove.h"
#endif

#include <DemoReaderBase.h>

#define PLAYERSTATE_FUTURE_SEEK 60000

typedef jpcre2::select<char> jp;

enum highlightSearchMode_t {
	SEARCH_ALL,
	SEARCH_INTERESTING,
	SEARCH_MY_CTF_RETURNS,
	SEARCH_CTF_RETURNS,
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
};


class PlayerInfo {
public:
	int				team = TEAM_FREE;
	int				color1 = 0;
	std::string		model = "";
	std::string		skin = "";
	qboolean		skinExists = qfalse;
	qboolean		modelNeededTeamAdjustment = qfalse;
};


class Command {
public:
	int64_t demoTime;
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


template<class T, size_t sample_count>
class RollingAverage {
	T samples[sample_count];
	T sum = 0;
	int64_t amountAdded = 0;
public:
	const size_t sample_count = sample_count;
	inline void add(T num) {
		size_t currentIndex = amountAdded % sample_count;
		if (amountAdded >= sample_count) {
			sum -= samples[currentIndex];
		}
		sum += num;
		samples[currentIndex] = num;
		amountAdded++;
	}
	inline T getAverage() {
		if (amountAdded >= sample_count) {
			return sum / (T)sample_count;
		}
		else {
			return sum / (T)amountAdded;
		}
	}
	inline bool averageValid() {
		return amountAdded >= sample_count;
	}
	inline void clear() {
		// No need to clear samples[] since we never rely on it being nulled.
		// The only read operations from it happen after all indexes of the array have already been written to.
		sum = 0;
		amountAdded = 0;
	}
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

typedef std::map<int, std::unique_ptr<SnapshotInfo>> SnapshotInfoMap;
typedef SnapshotInfoMap::iterator SnapshotInfoMapIterator;
typedef SnapshotInfoMap::reverse_iterator SnapshotInfoMapIteratorReverse;

inline bool snapshotInfosServerTimeSmallerPredicate(const std::pair<const int, std::unique_ptr<SnapshotInfo>>& it, const int& serverTime) {
	return it.second->serverTime < serverTime;
};
inline bool snapshotInfosDemoTimeSmallerPredicate(const std::pair<const int, std::unique_ptr<SnapshotInfo>>& it, const float& demoTime) {
	return it.second->demoTime < demoTime;
};
inline bool snapshotInfosServerTimeGreaterPredicate(const int& serverTime, const std::pair<const int, std::unique_ptr<SnapshotInfo>>& it) {
	return serverTime < it.second->serverTime;
};
inline bool snapshotInfosSnapNumPredicate(const std::pair<const int, std::unique_ptr<SnapshotInfo>>& it, const int& snapNum) {
	return it.second->snapNum < snapNum;
};

#define COMMANDTIMEOFFSETAVERAGESAMPLECOUNT 100

class DemoReader : public DemoReaderBase {




	struct {
		gametype_general_t	gameTypeGeneral = GT_FFA_GENERAL;
	} game;

	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
	jp::Regex defragRecordFinishRegex;
	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

	playerState_t basePlayerStates[MAX_CLIENTS_MAX]; // Contains stuff like team and max health and other stuff that otherwise messes up rendering.

	//std::map<int, int> playerFirstVisible;
	//std::map<int, int> playerFirstFollowed;
	//std::map<int, int> playerFirstFollowedOrVisible;
	//std::map<int, int> lastEventTime;
	int64_t lastEventTime[MAX_GENTITIES]{};
	//std::map<int, int> lastEvent;
	int lastEvent[MAX_GENTITIES]{};


	int firstPacketWithPlayer[MAX_CLIENTS_MAX];
	int lastKnownPacketWithPlayer[MAX_CLIENTS_MAX];
	int firstPacketWithPlayerState[MAX_CLIENTS_MAX];
	int lastKnownPacketWithPlayerState[MAX_CLIENTS_MAX];
	
	SnapshotInfo* pastSnapshotQuickLookup[COMMANDTIMEOFFSETAVERAGESAMPLECOUNT]{};
	RollingAverage<double, COMMANDTIMEOFFSETAVERAGESAMPLECOUNT> rollingAverageCommandTimeOffsets[MAX_CLIENTS_MAX]; // Offsets of command time to server time per player, so we can smooth things out a bit.

	int mohaaPlayerWeaponModelIndexThisFrame[MAX_CLIENTS_MAX];
	int mohaaPlayerWeaponModelIndex[MAX_CLIENTS_MAX];
	byte mohaaPlayerWeapon[MAX_CLIENTS_MAX];


	SnapshotInfoMap snapshotInfos;
	std::vector<Command> readCommands;
	std::vector<Event> readEvents;

	int					lastKnownCommandOrServerTimes[MAX_CLIENTS_MAX]; // For each clientnum
	int					lastMessageWithEntity[MAX_GENTITIES]; 
	//std::map<int,int>	lastMessageWithEntity; 

	playerState_t	oldPS[MAX_CLIENTS_MAX];

	double			lastGottenCommandsTime = 0;
	int				lastGottenCommandsServerTime = 0;
	double			lastGottenEventsTime[EK_COUNT] = { 0 };
	int				lastGottenEventsServerTime[EK_COUNT] = { 0 };



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
	void InterpolatePlayer(int clientNum, double time, SnapshotInfoMapIterator* from, SnapshotInfoMapIterator* to, playerState_t* outPS, qboolean detailedPS = qfalse);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	//qboolean ReadMessage();

	qboolean ReadMessageReal() override;
	playerState_t GetPlayerFromSnapshot(int clientNum, int snapNum, SnapshotInfoMapIterator* playerStateSourceSnap = NULL, qboolean detailedPS = qfalse);
	playerState_t GetPlayerFromSnapshot(int clientNum, SnapshotInfoMapIterator snapInfoIterator, SnapshotInfoMapIterator* playerStateSourceSnap = NULL, qboolean detailedPS = qfalse);

	void updateConfigStringRelatedInfo();

	template<typename T, std::size_t ARRAY_LEN>
	int getPastPlayerAngles(int clientNum, SnapshotInfoMapIterator snapInfoIterator,std::array<T, ARRAY_LEN>& times, std::array<T, ARRAY_LEN>& pitch, std::array<T, ARRAY_LEN>& yaw, qboolean fill);

	void generateBasePlayerStates();
#if USE_PMOVE
	CModel* cm = NULL;
	PMove* pm = NULL;
#endif
public:
#if USE_PMOVE
	static const bgEntity_t* getEnt(int entNum, void* reference) {
		SnapshotInfo* snapInfo = (SnapshotInfo*)reference;
		//if (snapInfo->playerState.clientNum == entNum) {// TODO
		//	return NULL;
		//}
		if (snapInfo->entities.find(entNum) != snapInfo->entities.end()) { // this is kinda ugly :/ oh well. shouldnt be important most of the time
			return (const bgEntity_t*)&snapInfo->entities[entNum];
		}
		return NULL; // TODO
	}

	// you are responsible for destroying cmodel yourself. DemoReader won't do it for you.
	void setCM(CModel* cmodel) {
		cm = cmodel;
		if (cm && !pm) {
			pm = new PMove(demoType, cm, getEnt);
		}
		else if(pm) {
			pm->SetCModel(cm);
		}
	}
#endif

	qboolean LoadDemo(const char* sourceDemoFile);
	void convertPSTo(playerState_t* ps, demoType_t targetDemoType);

	DemoReader::DemoReader() : defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi") {
	};

#ifdef DEBUG
	~DemoReader() {
#if USE_PMOVE
		if (pm) {
			delete pm;
		}
#endif
		std::cout << "DemoReader being destroyed.";
	}
#endif

	int getMaxClients();

	int64_t getCutStartOffset(qboolean withTruncationOffset = qtrue);
	qboolean containsDimensionData();
	demoType_t getDemoType();
	inline gametype_general_t getGametypeGeneral() {
		return game.gameTypeGeneral;
	}
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
	qboolean SeekToPlayerInPacket(int clientNum, int maxMsgNum=0); // Seek until we get a packet with this player
	qboolean SeekToPlayerCommandOrServerTime(int clientNum, int serverTime); // The reason this is called "OR servertime" is because player entities aren't guaranteed to have commandtime in them, they only have it if g_smoothclients is true.
	void	ClearSnapshotsBeforeIterator(SnapshotInfoMapIterator snapInfoIt);

	// tmpPS is for DM_14 to get saber color
	PlayerInfo GetPlayerInfo(int clientnum, playerState_t* tmpPS = NULL);

	playerState_t GetInterpolatedPlayerState(double time);
	playerState_t GetLastOrNextPlayer(int clientNum, int serverTime, SnapshotInfoMapIterator* usedSourceSnap=NULL,SnapshotInfoMapIterator* usedSourcePlayerStateSnap=NULL, qboolean detailedPS = qfalse, const SnapshotInfoMapIterator* referenceSnap = NULL, int futureSeekLimit=0);
	SnapshotEntities GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, const SnapshotInfoMapIterator* referenceSnap = NULL);
	void GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, SnapshotEntities* mapToEnhance, const SnapshotInfoMapIterator* referenceSnap = NULL); // Same as the other overload but enhances an existing map if item with lower serverTime is found.
	playerState_t GetInterpolatedPlayer(int clientNum, double time, SnapshotInfo** oldSnap=NULL, SnapshotInfo** newSnap=NULL, qboolean detailedPS = qfalse, float* translatedTime=NULL);
	entityState_t GetInterpolatedNPC(int entityNum, double time, /*SnapshotInfo** oldSnap = NULL, SnapshotInfo** newSnap = NULL, */float* translatedTime = NULL);

	SnapshotEntities GetEntitiesAtTime(double time, double * translatedTime = NULL, int* sourceSnapNum = NULL);
	SnapshotEntities GetEntitiesAtPreciseTime(int time, qboolean includingPS, int* sourceSnapNum = NULL);
	std::vector<std::string> GetNewCommands(double time);
	std::vector<std::string> GetNewCommandsAtServerTime(int serverTime);
	std::vector<Event> GetNewEvents(double time, eventKind_t kind=EK_ENTITY);
	std::vector<Event> GetNewEventsAtServerTime(int serverTime, eventKind_t kind=EK_ENTITY);
	
	const char* GetModelConfigString(int modelNum, int* maxLength);
	const char* GetSoundConfigString(int soundNum, int* maxLength);
	qboolean EndReachedAtTime(double time);
	int getDemoRecorderClientNum();
	qboolean	purgeSnapsBefore(int snapNum);
	qboolean	purgeSnapsBefore(int snapNum, int keepExtra);
};





void remapConfigStrings(int eventNumber,entityState_t* tmpEntity, clientActive_t* clCut, DemoReader* reader, std::vector<std::string>* commandsToAdd, qboolean doModelIndex, qboolean doModelIndex2, demoType_t demoType);
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
	size_t framesSincePurge=0;
};






class SlotManager {

	const int maxAvailableSlots = MAX_GENTITIES - 2;
	int availableSlots = maxAvailableSlots;
	int lowestPossibleEntityNum = MAX_CLIENTS;
	vec3_t modelDistanceOrigin{};
public:
	typedef enum sourceDemoMappingType {
		SDMT_NONE,
		SDMT_G2,
		SDMT_BODYPART
	};
	typedef struct mappingMeta {
		int	bodyPartClientNumPlus1;
		int	bodyPart;
	} mappingMeta;
	struct sourceDemoMapping {
		int demoIndex;
		int entityNum;
		qboolean isModel;
		sourceDemoMappingType type;
		mappingMeta meta;
	};
private:
	//std::map<int, sourceDemoMapping> mappings;
	typedef ankerl::unordered_dense::map<uint16_t, sourceDemoMapping, ankerl::unordered_dense::hash<uint16_t>> MappingMap;
	MappingMap mappings;
	//bool occupied[MAX_GENTITIES] = {false};
	typedef MappingMap::iterator mappingIterator;
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
	int getEntitySlot(int demoIndex, int entityNum, sourceDemoMappingType type = SDMT_NONE) {
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
				mappings[i] = { demoIndex,entityNum,qfalse,type };
				availableSlots--;
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	sourceDemoMapping* getSlotMappingData(int slotNum) {
		if (mappings.find(slotNum) == mappings.end()) {
			return NULL;
		}
		else {
			return &mappings[slotNum];
		}
	}
	void setModelDistanceOrigin(vec_t* originA) {
		VectorCopy(originA, modelDistanceOrigin);
	}
	// These are released as soon as they disappear from the demo. We don't need consistency from frame to frame on them. It's just random models/movers etc., they only need to be visible.
	// TODO actually do this?
	int getModelSlot(int demoIndex, int entityNum, sourceDemoMappingType type = SDMT_NONE) {
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
				mappings[i] = { demoIndex,entityNum,qtrue,type };
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
	std::vector<std::tuple<int,sourceDemoMapping>> freeSlots(int demoIndex) { // Returns the erased slots
		std::vector<std::tuple<int, sourceDemoMapping>> erasedSlots;
		int countErased = 0;
		restart:
		for (mappingIterator it = mappings.begin(); it != mappings.end(); ) {
			mappingIterator iteratorHere = it;
			it++;
			if (iteratorHere->second.demoIndex == demoIndex) {
				erasedSlots.push_back(std::tuple(iteratorHere->first,iteratorHere->second));
				mappings.erase(iteratorHere);
				availableSlots++;
				countErased++;
				goto restart; // since we're using ankerl's dense map now, iterators become invalidated.
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