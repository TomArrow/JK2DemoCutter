#pragma once
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <demoCut.h>



typedef enum dimensionDataType_t {
	DIM_OWNAGE
};


struct MetaEventItemAbsolute {
	int timeFromDemoStart; // Relative to the demo start
	int serverTime = 0; // correlated
	qboolean serverTimeCorrelated = qfalse; // whether the serverTime value is already set for this one
	metaEventType_t type;
};


class DemoReaderBase {
	void	parseMetaEvents(const char* meString);

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

	
	struct {
		qboolean ownageExtraInfoMetaMarker = qfalse;
		qboolean dimensionInfoConfirmed = qfalse;
		dimensionDataType_t dimensionInfoType = (dimensionDataType_t )(-1);
		int g_entHUDieldsValue = 0;
		int64_t	cutStartOffset = 0;
		int64_t	truncationOffset = 0;
	} extraFieldInfo;

	qboolean wasFirstCommandByte = qfalse;
	qboolean firstCommandByteRead = qfalse;
	rapidjson::Document* jsonSourceFileMetaDocument = NULL;
	std::vector<MetaEventItemAbsolute> metaEvents;
	std::vector<MetaEventItemAbsolute>::iterator metaEventsTrackIterator;
	std::vector<MetaEventItemAbsolute>::iterator gottenMetaEventsIterator;
	int64_t metaHighlight = -1;

	qboolean tryReadMetadata(msg_t* msg);
	void trackMetaEventsTiming();
public:

	DemoReaderBase() {

		metaEventsTrackIterator = metaEvents.begin();
		gottenMetaEventsIterator = metaEvents.begin();
	}

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
	qboolean SeekToServerTime(int serverTime);
	std::vector<MetaEventItemAbsolute> GetNewMetaEventsAtServerTime(int serverTime);
	int GetMetaEventCount();
	qboolean EndReachedAtServerTime(int serverTime);
	qboolean copyMetadataTo(rapidjson::Document* targetDocument, qboolean keepKeys);
};