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

qboolean DemoReaderBase::tryReadMetadata(msg_t* msg) {
	const char* maybeMeta = demoCutReadPossibleMetadata(msg, demoType);
	if (maybeMeta) {

		jsonSourceFileMetaDocument = new rapidjson::Document();
		if (jsonSourceFileMetaDocument->Parse(maybeMeta).HasParseError() || !jsonSourceFileMetaDocument->IsObject()) {
			// We won't quit demo cutting over this. It's whatever. We don't wanna make a demo unusable just because it contains bad
			// metadata. Kinda goes against the spirit. This is a different approach from above with the main metadata, where an error in that
			// will quit the process. Because the user can after all just adjust and fix the commandline.
			std::cout << "Old demo appears to contain metadata, but wasn't able to parse it. Discarding.\n";
			delete jsonSourceFileMetaDocument;
			jsonSourceFileMetaDocument = NULL;
			return qfalse;
		}

		const char* realExtraFieldsMetaname = jsonGetRealMetadataKeyName(jsonSourceFileMetaDocument, "extraFields");
		if (realExtraFieldsMetaname) {
			const char* extraFields = (*jsonSourceFileMetaDocument)[realExtraFieldsMetaname].GetString();
			if (extraFields) {
				if (!stricmp(extraFields, "ownage")) {
					extraFieldInfo.ownageExtraInfoMetaMarker = qtrue;
				}
			}
		}
		if (jsonSourceFileMetaDocument->HasMember("cso")) { // cut start offset? to detect imperfect timing and adjust for it? Easier than to feed it from the outside.
			extraFieldInfo.cutStartOffset = (*jsonSourceFileMetaDocument)["cso"].GetInt64();
		}
		if (jsonSourceFileMetaDocument->HasMember("trim")) { // cut start offset? to detect imperfect timing and adjust for it? Easier than to feed it from the outside.
			extraFieldInfo.truncationOffset = (*jsonSourceFileMetaDocument)["trim"].GetInt64();
		}
		if (jsonSourceFileMetaDocument->HasMember("hl")) { // cut start offset? to detect imperfect timing and adjust for it? Easier than to feed it from the outside.
			metaHighlight = (*jsonSourceFileMetaDocument)["hl"].GetInt64()- extraFieldInfo.cutStartOffset;
		}
		if (jsonSourceFileMetaDocument->HasMember("me")) {
			const char* metaEvents = (*jsonSourceFileMetaDocument)["me"].GetString();
			parseMetaEvents(metaEvents);
		}

		if (metaEvents.size()) {
			std::sort(metaEvents.begin(), metaEvents.end(), [](MetaEventItemAbsolute a, MetaEventItemAbsolute b)
				{
					return a.timeFromDemoStart < b.timeFromDemoStart;
				}
			);
		}

		return qtrue;

	}
}
void	DemoReaderBase::parseMetaEvents(const char* meString) {
	const char* text = meString;
	if (!meString || !*meString) {
		return;
	}
	while (*text) {
		for (int i = 0; i < METAEVENT_COUNT; i++) {
			int KeyLen = strlen(metaEventKeyNames[i]);
			if (!_strnicmp(text, metaEventKeyNames[i], KeyLen)) {
				text += KeyLen;
				MetaEventItemAbsolute item;
				item.type = (metaEventType_t)i;
				item.timeFromDemoStart = atoiWhileNumber(&text) - extraFieldInfo.cutStartOffset;
				metaEvents.push_back(item);
				break;
			}
		}
		if(*text) text++;
	}

}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif