#pragma once
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>
#include <DemoReaderBase.h>




class DemoReaderLight : public DemoReaderBase {


	std::map<int, int> lastEvent;

	std::vector<int> pingValues[MAX_CLIENTS_MAX];
	qboolean		playerSeen[MAX_CLIENTS_MAX];


	float			lastGottenCommandsTime = 0;
	float			lastGottenEventsTime = 0;



	//clSnapshot_t lastSnap;

	int messageOffset = 0;

	//qboolean ConfigstringModified(clientActive_t* clCut);
	//void ParseCommandString(msg_t* msg, clientConnection_t* clcCut);
	//qboolean ParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	//void ParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType);
	//qboolean ParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType);
	//void ParseRMG(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut);


	qboolean PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap);
	entityState_t* findEntity(int number);

	// Obsolete:
	//qboolean demoRead(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, highlightSearchMode_t searchMode);

	//qboolean ReadMessage();

	qboolean ReadMessageReal() override;
	qboolean SeekToServerTime(int serverTime);


public:

	qboolean LoadDemo(const char* sourceDemoFile);
	demoType_t getDemoType();
	bool isThisMOHAADemo();
	qboolean SeekToTime(int time);
	
	qboolean EndReachedAtTime(float time);

	void ReadToEnd();
	void GetMedianPingData(int* playerPingData); // Requires pointer to array with 32 ints
	void GetLowestPingData(int* playerPingData); // Requires pointer to array with 32 ints
	void GetMedianOfLocalAveragesPingData(float* playerPingData); // Requires pointer to array with 32 ints
	void GetPlayersSeen(qboolean* playersSeenA); // Requires pointer to array with 32 qbooleans
	void FreePingData();
};