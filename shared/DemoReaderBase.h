#pragma once
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>




class DemoReaderBase {
protected:

	std::string originalDemoPath = "";

	bool	isMOHAADemo = false;
	int		maxClientsThisDemo = MAX_CLIENTS;

	qboolean		anySnapshotParsed = qfalse;
	qboolean		endReached = qfalse;

	int lastKnownRedFlagCarrier = -1;
	int lastKnownBlueFlagCarrier = -1;
	demo_t			thisDemo;

	// Info during reading
	demoType_t		demoType;
	qboolean		isCompressedFile;
	fileHandle_t	oldHandle = 0;
	int				oldSize;
	int				readGamestate = 0;

	int64_t			demoStartTime = 0;
	int64_t			demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int64_t			demoCurrentTime = 0;
	int				lastGameStateChange = 0;
	int64_t			lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;
	int				lastKnownInOrderTime = 0;
	int				maxSequenceNum = -9999;
public:


	qboolean ReadMessage();
	virtual qboolean ReadMessageReal() = 0;

	qboolean SeekToCommandTime(int serverTime);
	qboolean SeekToAnySnapshotIfNotYet();
	qboolean CloseDemo();
	playerState_t GetCurrentPlayerState();	
	std::map<int, entityState_t> GetCurrentEntities(); 
	clSnapshot_t GetCurrentSnap();
	const char* GetConfigString(int configStringNum, int* maxLength);
	const char* GetPlayerConfigString(int playerNum, int* maxLength);
	qboolean AnySnapshotParsed();
	qboolean EndReached();
	int64_t getCurrentDemoTime();
};