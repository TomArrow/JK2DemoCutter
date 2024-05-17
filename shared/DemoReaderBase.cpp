#include "demoCut.h"
#include "jkaStuff.h"
#include "otherGameStuff.h"
#include "otherGameStuff2.h"
#include "DemoReaderLight.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>




// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//



clSnapshot_t DemoReaderBase::GetCurrentSnap() {
	return thisDemo.cut.Cl.snap;
}

const char* DemoReaderBase::GetPlayerConfigString(int playerNum, int* maxLength) {
	int configStringBaseIndex = getCS_PLAYERS(demoType); // (demoType == DM_26 || demoType == DM_25) ? CS_PLAYERS_JKA : CS_PLAYERS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + playerNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReaderBase::GetConfigString(int configStringNum, int* maxLength) {
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}


std::map<int, entityState_t> DemoReaderBase::GetCurrentEntities() {
	std::map<int, entityState_t> retVal;
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
		entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		retVal[thisEntity->number] = *thisEntity;
}
	return retVal;
}



qboolean DemoReaderBase::CloseDemo() {

	FS_FCloseFile(oldHandle);
	return qtrue;
}
qboolean DemoReaderBase::AnySnapshotParsed() {

	return anySnapshotParsed;
}
qboolean DemoReaderBase::EndReached() {

	return endReached;
}


qboolean DemoReaderBase::ReadMessage() {
	if (endReached) return qfalse;
	qboolean realReadResult = qfalse;
	realReadResult = ReadMessageReal();
	if (!realReadResult) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
}


qboolean DemoReaderBase::SeekToCommandTime(int serverTime) {
	while (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && endReached) return qfalse;
	return qtrue;
}


playerState_t DemoReaderBase::GetCurrentPlayerState() {
	return thisDemo.cut.Cl.snap.ps;
}
qboolean DemoReaderBase::SeekToAnySnapshotIfNotYet() {
	while (!anySnapshotParsed && !endReached) {
		ReadMessage();
	}
	if (!anySnapshotParsed && endReached) return qfalse;
	return qtrue;
}



int64_t DemoReaderBase::getCurrentDemoTime() {
	return demoCurrentTime;
}



#ifdef RELDEBUG
//#pragma optimize("", on)
#endif