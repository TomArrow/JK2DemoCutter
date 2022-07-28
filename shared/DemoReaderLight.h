#pragma once
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>




class DemoReaderLight {

	std::map<int, int> lastEvent;

	std::vector<int> pingValues[MAX_CLIENTS];
	qboolean		playerSeen[MAX_CLIENTS];

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

	float			lastGottenCommandsTime = 0;
	float			lastGottenEventsTime = 0;

	qboolean		anySnapshotParsed = qfalse;
	qboolean		endReached = qfalse;


	clSnapshot_t lastSnap;

	int messageOffset = 0;

	qboolean ConfigstringModified(clientActive_t* clCut);
	void ParseCommandString(msg_t* msg, clientConnection_t* clcCut);
	qboolean ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	void ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType);
	qboolean ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	void ParseRMG(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut);


	qboolean PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap);
	entityState_t* findEntity(int number);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	qboolean ReadMessage();

	template <demoType_t D>
	qboolean ReadMessageReal();
	qboolean SeekToServerTime(int serverTime);


public:


	qboolean SeekToTime(int time);
	qboolean SeekToCommandTime(int serverTime);
	qboolean SeekToPlayerInPacket(int clientNum); // Seek until we get a packet with this player
	qboolean SeekToPlayerCommandOrServerTime(int clientNum, int serverTime); // The reason this is called "OR servertime" is because player entities aren't guaranteed to have commandtime in them, they only have it if g_smoothclients is true.
	qboolean SeekToAnySnapshotIfNotYet();
	qboolean LoadDemo(const char* sourceDemoFile);
	qboolean CloseDemo();
	playerState_t GetCurrentPlayerState();
	std::map<int, entityState_t> GetCurrentEntities();
	clSnapshot_t GetCurrentSnap();
	const char* GetConfigString(int configStringNum, int* maxLength);
	const char* GetPlayerConfigString(int playerNum, int* maxLength);
	qboolean AnySnapshotParsed();
	qboolean EndReached();
	qboolean EndReachedAtTime(float time);

	void ReadToEnd();
	void GetMedianPingData(int* playerPingData); // Requires pointer to array with 32 ints
	void GetPlayersSeen(qboolean* playersSeenA); // Requires pointer to array with 32 qbooleans
	void FreePingData();
};