#pragma once
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>


typedef jpcre2::select<char> jp;

enum highlightSearchMode_t {
	SEARCH_ALL,
	SEARCH_INTERESTING,
	SEARCH_MY_CTF_RETURNS,
	SEARCH_CTF_RETURNS,
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
};

class SnapshotInfo {
public:
	std::map<int, entityState_t> entities;
	std::map<int, int> playerCommandOrServerTimes;
	playerState_t playerState;
	int serverTime;
	qboolean playerStateTeleport;
};
class Command {
public:
	int demoTime;
	std::string command;
};

class Event {
public:
	int demoTime;
	entityState_t theEvent;
	int eventNumber;
};



class DemoReader {

	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
	jp::Regex defragRecordFinishRegex;
	//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

	std::map<int, int> playerFirstVisible;
	std::map<int, int> playerFirstFollowed;
	std::map<int, int> playerFirstFollowedOrVisible;
	std::map<int, int> lastEvent;

	std::map<int, SnapshotInfo> snapshotInfos;
	std::vector<Command> readCommands;
	std::vector<Event> readEvents;

	int lastKnownRedFlagCarrier = -1;
	int lastKnownBlueFlagCarrier = -1;
	demo_t			thisDemo;

	// Info during reading
	demoType_t		demoType;
	fileHandle_t	oldHandle = 0;
	int				oldSize;
	int				readGamestate = 0;

	int				demoStartTime = 0;
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int				demoCurrentTime = 0;
	int				lastGameStateChange = 0;
	int				lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;
	int				lastKnownCommandTime = 0;
	std::map<int,int>	lastKnownCommandOrServerTimes; // For each clientnum
	std::map<int,int>	lastMessageWithEntity; 

	float			lastGottenCommandsTime = 0;
	float			lastGottenEventsTime = 0;

	qboolean		anySnapshotParsed = qfalse;
	qboolean		endReached = qfalse;


	clSnapshot_t lastSnap;

	int messageOffset = 0;

	qboolean ConfigstringModified(clientActive_t* clCut);
	int GetEvent(entityState_t* es);
	void ParseCommandString(msg_t* msg, clientConnection_t* clcCut);
	qboolean ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	void ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType);
	qboolean ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	qboolean PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap);
	entityState_t* findEntity(int number);

	void InterpolatePlayerState(float time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS);
	void InterpolatePlayer(int clientNum, float time, SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	qboolean ReadMessage();
	qboolean ReadMessageReal();
	playerState_t GetPlayerFromSnapshot(int clientNum, SnapshotInfo* snap);

public:

	DemoReader::DemoReader() : defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi") {
	};

	qboolean SeekToTime(int time);
	qboolean SeekToCommandTime(int serverTime);
	qboolean SeekToPlayerInPacket(int clientNum); // Seek until we get a packet with this player
	qboolean SeekToPlayerCommandOrServerTime(int clientNum, int serverTime); // The reason this is called "OR servertime" is because player entities aren't guaranteed to have commandtime in them, they only have it if g_smoothclients is true.
	qboolean SeekToAnySnapshotIfNotYet();
	qboolean LoadDemo(const char* sourceDemoFile);
	qboolean CloseDemo();
	playerState_t GetCurrentPlayerState();
	playerState_t GetInterpolatedPlayerState(float time);
	playerState_t GetInterpolatedPlayer(int clientNum, float time);
	std::map<int, entityState_t> DemoReader::GetCurrentEntities();
	std::vector<std::string> DemoReader::GetNewCommands(float time);
	std::vector<Event> DemoReader::GetNewEvents(float time);
	clSnapshot_t GetCurrentSnap();
	const char* GetConfigString(int configStringNum);
	const char* GetPlayerConfigString(int playerNum);
	qboolean AnySnapshotParsed();
	qboolean EndReached();
};