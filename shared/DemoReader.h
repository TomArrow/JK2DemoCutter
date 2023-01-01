#pragma once
#ifndef DEMOREADER_H
#define DEMOREADER_H

#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>


#define PLAYERSTATE_FUTURE_SEEK 10000

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



class DemoReader {

	int		maxClientsThisDemo = MAX_CLIENTS;

	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
	jp::Regex defragRecordFinishRegex;
	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

	playerState_t basePlayerStates[MAX_CLIENTS_MAX]; // Contains stuff like team and max health and other stuff that otherwise messes up rendering.

	std::map<int, int> playerFirstVisible;
	std::map<int, int> playerFirstFollowed;
	std::map<int, int> playerFirstFollowedOrVisible;
	std::map<int, int> lastEventTime;
	std::map<int, int> lastEvent;

	std::map<int, SnapshotInfo> snapshotInfos;
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
	std::map<int,int>	lastKnownCommandOrServerTimes; // For each clientnum
	std::map<int,int>	lastMessageWithEntity; 

	playerState_t	oldPS;

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

	void InterpolatePlayerState(float time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS);
	void InterpolatePlayer(int clientNum, float time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS, qboolean detailedPS = qfalse);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	qboolean ReadMessage();

	qboolean ReadMessageReal();
	playerState_t GetPlayerFromSnapshot(int clientNum, int snapNum, qboolean detailedPS = qfalse);

	void mapAnimsToDM15(playerState_t* ps);


	void generateBasePlayerStates();

public:

	DemoReader::DemoReader() : defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi") {
	};
	
	int getMaxClients();

	demoType_t getDemoType();
	int getClientNumForDemo(std::string* playerSearchString, qboolean printEndLine = qfalse);
	int GetFirstSnapServerTime();
	qboolean SeekToTime(double time);
	int GetFirstServerTimeAfterServerTime(int serverTime);
	int GetLastServerTimeBeforeServerTime(int serverTime);
	SnapshotInfo* GetSnapshotInfoAtServerTime(int serverTime);
	qboolean SeekToServerTime(int serverTime);
	qboolean SeekToCommandTime(int serverTime);
	qboolean SeekToPlayerInPacket(int clientNum); // Seek until we get a packet with this player
	qboolean SeekToPlayerCommandOrServerTime(int clientNum, int serverTime); // The reason this is called "OR servertime" is because player entities aren't guaranteed to have commandtime in them, they only have it if g_smoothclients is true.
	qboolean SeekToAnySnapshotIfNotYet();
	qboolean LoadDemo(const char* sourceDemoFile);
	qboolean CloseDemo();
	playerState_t GetCurrentPlayerState();
	playerState_t GetInterpolatedPlayerState(double time);
	playerState_t GetLastOrNextPlayer(int clientNum, int serverTime, SnapshotInfo** oldSnap=NULL, SnapshotInfo** newSnap=NULL, qboolean detailedPS = qfalse);
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

#endif