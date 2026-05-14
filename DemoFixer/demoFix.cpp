#include "demoCut.h"

#define PCRE2_STATIC
#include "jpcre2.hpp"

#ifdef _WIN32
#include <windows.h>
#include "fileapi.h"
#include "Handleapi.h"
#endif

#include <chrono>
#include <include/popl.hpp>

#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include <filesystem>


// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

#ifdef GetObject
#undef GetObject // Messes with rapidjson
#endif

demo_t			demo;




typedef jpcre2::select<char> jp;

enum demoTimeType_t {
	DEMOTIME,
	GAMETIME,
	GAMETIME_PAUSE,
	SERVERTIME
};

class demoTime_t {
public:
	qboolean success;
	int64_t time = 0; // In milliseconds
	demoTimeType_t type = DEMOTIME;
	int skips = 0;
	/*int64_t compare(int64_t demoTime, int64_t serverTime, int64_t levelStartTime, qboolean isPause, int64_t pauseDuration) { // >1 if this demoTime_t is bigger than the arguments. 0 if identical. >1 if smaller. 
		int64_t referenceTime = 0;
		switch (type) {
			case DEMOTIME:
				return time - demoTime;
			case GAMETIME:
				if(!isPause){
					return time - (serverTime - levelStartTime);
				}
				else {
					return time - (serverTime - levelStartTime - pauseDuration);
				}
			case GAMETIME_PAUSE: 
				// This is a bit fucky and logically very bad tbh. But we know the cases in which it's gonna be used so we can try and make it work in that way. Ideally just don't use this, ever.
				// Two pauses right after each other could have identical times in them. We have to kinda just ignore that here because what else can we really do?
				if (isPause) {
					return time - (serverTime - levelStartTime);
				}
				else {
					// We can't really tell...
					return -1;
				}
			case SERVERTIME:
				return time - serverTime;
		}
		return NULL;
	}*/
	bool isReached(int64_t demoTime, int64_t serverTime, int64_t levelStartTime, qboolean isPause, int64_t pauseDuration, int mapRestartCounter, int64_t* offset) {
		int64_t referenceTime = 0;
		switch (type) {
			case DEMOTIME:
				if (offset) *offset = demoTime - time;
				return time <= demoTime;
			case GAMETIME:
				if(!isPause){
					if (offset) *offset = (serverTime - levelStartTime) - time;
					return mapRestartCounter >= skips && time <= (serverTime - levelStartTime);
				}
				else {
					// We can't really know
					if (offset) *offset = (serverTime - levelStartTime - pauseDuration) - time;
					return mapRestartCounter >= skips && time <= (serverTime - levelStartTime - pauseDuration);
				}
			case GAMETIME_PAUSE: 
				// This is a bit fucky and logically very bad tbh. But we know the cases in which it's gonna be used so we can try and make it work in that way. Ideally just don't use this, ever.
				// Two pauses right after each other could have identical times in them. We have to kinda just ignore that here because what else can we really do?
				if (isPause) {
					if (offset) *offset = (serverTime - levelStartTime) - time;
					return time <= (serverTime - levelStartTime);
				}
				else {
					// We can't really tell...
					if (offset) *offset = 0;
					return qfalse;
				}
			case SERVERTIME:
				if (offset) *offset = serverTime - time;
				return time <= serverTime;
		}
		return NULL;
	}
	bool isSurpassed(int64_t demoTime, int64_t serverTime, int64_t levelStartTime, qboolean isPause, int64_t pauseDuration, int mapRestartCounter) {
		int64_t referenceTime = 0;
		switch (type) {
			case DEMOTIME:
				return time < demoTime;
			case GAMETIME:
				if(!isPause){
					return  mapRestartCounter >= skips && time < (serverTime - levelStartTime);
				}
				else {
					// We can't really know
					return  mapRestartCounter >= skips && time < (serverTime - levelStartTime - pauseDuration);
				}
			case GAMETIME_PAUSE: 
				// This is a bit fucky and logically very bad tbh. But we know the cases in which it's gonna be used so we can try and make it work in that way. Ideally just don't use this, ever.
				// Two pauses right after each other could have identical times in them. We have to kinda just ignore that here because what else can we really do?
				if (isPause) {
					return time < (serverTime - levelStartTime);
				}
				else {
					// We can't really tell...
					return qfalse;
				}
			case SERVERTIME:
				return time < serverTime;
		}
		return NULL;
	}
};

jp::Regex timeParser(R"raw(\s*(?<type>gp?|s|d)?(?<number1>\d+)\s*(?::(?<number2>\d+))?\s*(?:.(?<number3>\d+))?\s*(skip(?<skipNumber>\d+))?\s*)raw", "mSi");

demoTime_t timeParse(std::string timeText) {
	jp::VecNas vec_nas;
	jp::RegexMatch rm;

	demoTime_t parsedTime;
	parsedTime.skips = 0;
	Com_Memset(&parsedTime, 0,sizeof(parsedTime));

	size_t count = rm.setRegexObject(&timeParser)                          //set associated Regex object
		.setSubject(&timeText)                         //set subject string
		//.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
		.setNamedSubstringVector(&vec_nas)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_nas.size(); matchNum++) { // really its just going to be 1 but whatever
		if (matchNum > 0) break; // Regex can behave a bit weird sometimes.
		std::string type = vec_nas[matchNum]["type"];
		std::string number1 = vec_nas[matchNum]["number1"];
		std::string number2 = vec_nas[matchNum]["number2"];
		std::string number3 = vec_nas[matchNum]["number3"];
		std::string skipNumber = vec_nas[matchNum]["skipNumber"];
		
		if (type == "d") parsedTime.type = DEMOTIME;
		else if (type == "g") parsedTime.type = GAMETIME;
		else if (type == "gp") parsedTime.type = GAMETIME_PAUSE;
		else if (type == "s") parsedTime.type = SERVERTIME;
		else parsedTime.type = DEMOTIME;

		if (parsedTime.type == GAMETIME && skipNumber.size() != 0 ) { // Skip a number of games for gametime.
			parsedTime.skips = atoll(skipNumber.c_str());
		}

		if (number1.size() != 0 && number2.size() == 0 && number3.size() == 0) { // Just a simple number
			parsedTime.time = atoll(number1.c_str());
			parsedTime.success = qtrue;
		}
		else if (number2.size() != 0) { // MM:SS.xxx format
			parsedTime.time = atoll(number2.c_str())*1000;
			if (number1.size() != 0) parsedTime.time += atoll(number1.c_str())*60*1000;
			if (number3.size() != 0) {
				int64_t rawValue = atoll(number3.c_str());
				int64_t decimalCompensate = 3-number3.size();
				if (decimalCompensate > 0) {
					for (int i = 0; i < decimalCompensate; i++) {
						rawValue *= 10;
					}
				} else if (decimalCompensate < 0) {
					for (int i = 0; i > decimalCompensate; i--) {
						rawValue /= 10;
					}
				}
				parsedTime.time += rawValue*pow((int64_t)10, decimalCompensate);
			}
			parsedTime.success = qtrue;
		}
	}

	return parsedTime;
}



















#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


class StateVars {
public:
	fileHandle_t	oldHandle = 0;
	fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	//std::vector<byte>	oldDataRaw;
	int64_t			oldSize;
	char			oldName[MAX_OSPATH];
	char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	qboolean		newGameStateAfterDemoCutBegun = qfalse;
	char			ext[7]{};
	//char			originalExt[7]{};
	demoType_t		demoType;
	int				demoStartTime = 0;
	int64_t			demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int64_t			demoCurrentTime = 0;

	int				lastKnownTime = 0;
	int				lastKnownInOrderTime = 0;
	int				lastValidSnap = -1;

	int				mapRestartCounter = 0;
	bool			SEHExceptionCaught = false;
	int				psGeneralPMType = 0;
	int maxSequenceNum;
	qboolean demoCutStartsAtZero;
	int64_t originalFileAbsoluteCutOffset = 0;

	rapidjson::Document* jsonMetaDocument = NULL;
	rapidjson::Document* jsonPreviousMetaDocument = NULL;

	qboolean wasFirstCommandByte = qfalse;
	qboolean firstCommandByteRead = qfalse;

	qboolean originalCutOffsetRead = qfalse;
	int64_t fileOffset = 0;
	qboolean jsonWritten = qfalse;
};


int64_t FS_Read_FromBufferedFile(void* buffer, int64_t len, fileHandle_t f, byte* bufferSrc, int64_t oldSize, int64_t* fileOffset) {
	int64_t read = len;
	if (read > oldSize) {
		read = oldSize;
	}
	if (read < 0) {
		read = 0;
	}
	memcpy(buffer,bufferSrc+*fileOffset,read);
	*fileOffset += read;
	return read;

}

// TODO: WARNING: newSnap.serverTime < cl.oldFrameServerTime

//qboolean demoCut(const char* sourceDemoFile, demoTime_t startTime, demoTime_t endTime, const char* outputName, const char* jsonMetaData, bool noForcedMeta) {
qboolean demoFix(const char* sourceDemoFile, const char* outputName, const std::vector<std::string>* metaDataStrings, bool noForcedMeta, const char* reframeString) {
	
	//mvprotocol_t	protocol;

	StateVars* state = new StateVars();

	state->demoCutStartsAtZero = qtrue;


	/*if (jsonMetaData) {
		state->jsonMetaDocument = new rapidjson::Document();
		if (state->jsonMetaDocument->Parse(jsonMetaData).HasParseError() || !state->jsonMetaDocument->IsObject()) {
			std::cout << "-m/--meta metadata: Unable to parse as JSON.\n";
			return qfalse;
		}
		if (state->jsonMetaDocument->HasMember("oco")) {
			state->originalFileAbsoluteCutOffset = (*state->jsonMetaDocument)["oco"].GetInt64();
		}
	}*/
	if (metaDataStrings->size() > 1) {
		state->jsonMetaDocument = new rapidjson::Document();
		state->jsonMetaDocument->SetObject();
		for (int i = 0; i < metaDataStrings->size(); i++) {

			rapidjson::Document* tmpMetaDoc = new rapidjson::Document();
			if (tmpMetaDoc->Parse((*metaDataStrings)[i].c_str()).HasParseError() || !tmpMetaDoc->IsObject()) {
				std::cout << "-m/--meta metadata: Unable to parse as JSON.\n";
				return qfalse;
			}
			for (rapidjson::Value::MemberIterator it = tmpMetaDoc->MemberBegin(); it != tmpMetaDoc->MemberEnd(); it++) {
				if (it->name.GetString()[0] == '_') {
					std::cout << "Metadata member name '" << it->name.GetString() << "' is invalid. Names starting with underscore are reserved for old metadata in re-cut demos.\n";
					return qfalse;
				}
				if (!state->jsonMetaDocument->HasMember(it->name)) {
					state->jsonMetaDocument->AddMember(it->name, it->value, state->jsonMetaDocument->GetAllocator()); // This is move semantics, it will invalidate the original value but thats ok (?)
				}
				else {
					std::cout << "Metadata member '" << it->name.GetString() << "' provided more than once. Error.\n";
					return qfalse;
				}
			}
		}
	}
	else if (metaDataStrings->size() == 1) {
		state->jsonMetaDocument = new rapidjson::Document();
		if (state->jsonMetaDocument->Parse((*metaDataStrings)[0].c_str()).HasParseError() || !state->jsonMetaDocument->IsObject()) {
			std::cout << "-m/--meta metadata: Unable to parse as JSON.\n";
			return qfalse;
		}
	}

	if (!state->jsonMetaDocument) {

		state->jsonMetaDocument = new rapidjson::Document();
		state->jsonMetaDocument->SetObject();
	}

	state->jsonMetaDocument->AddMember("fixeddemo", 1, state->jsonMetaDocument->GetAllocator());

	// Since not in MME:
	/*if (!play) {
		Com_Printf("Demo cutting is allowed in mme mode only.\n");
		return qfalse;
	}*/
	//startTime += play->startTime;
	//endTime += play->startTime;
	

	//protocol = MV_GetCurrentProtocol();
	//if (protocol == PROTOCOL_UNDEF)
	//	state->ext = ".dm_16";
	//else
	//	state->ext = va(".dm_%i", protocol);
	//state->ext = Cvar_FindVar("mme_demostate->Ext")->string;
	demo.cut.Clc.demoCheckFor103 = qfalse;
	//strncpy_s(state->oldName, sizeof(state->oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	//state->ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	//strncpy_s(state->ext, sizeof(state->ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalstate->Ext, sizeof(originalstate->Ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	qboolean isCompressedFile = qfalse;
	qboolean createCompressedOutput = qfalse;


	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(sourceDemoFile, state->ext, state->oldName,&state->demoType,&isCompressedFile,&demo.cut.Clc);
	/*
	char specialTypeChar = state->ext[3];
	state->ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	createCompressedOutput = isCompressedFile;

	if (isCompressedFile) {
		return qfalse;
	}

	/*if (!*state->ext) {
		state->demoType = DM_16;
		strncpy_s(state->ext, sizeof(state->ext), ".dm_16", 6);
	}
	else if (!_stricmp(state->ext,".dm_15")) {

		state->demoType = DM_15;
		strncpy_s(state->ext, sizeof(state->ext), ".dm_15", 6);
		demo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(state->ext,".dm_16")) {

		state->demoType = DM_16;
		strncpy_s(state->ext, sizeof(state->ext), ".dm_16", 6);
	}*/

	fileCompressionScheme_t compressionSchemeUsed = FILECOMPRESSION_NONE;

	const char* oldPath = va("%s%s", state->oldName, state->ext);
	state->oldSize = FS_FOpenFileRead(oldPath, &state->oldHandle, qtrue, isCompressedFile,&compressionSchemeUsed,qtrue);
	if (!state->oldHandle) {
		Com_DPrintf("Failed to open %s for cutting.\n", state->oldName);
		return qfalse;
	}

	int messageOffset = 0;

	// read in the entire file so we can start reading at random offsets
	byte* fileBuffer = new byte[state->oldSize];
	FS_Read(fileBuffer, state->oldSize, state->oldHandle);

#define FS_Read(buffer,len,handle) FS_Read_FromBufferedFile(buffer,len,handle,fileBuffer,state->oldSize,&state->fileOffset)

	if (createCompressedOutput) {
		state->ext[3] = 'c';
	}

	bool isMOHAADemo = demoTypeIsMOHAA(state->demoType);

	//	Com_SetLoadingMsg("Cutting the demo...");

	state->maxSequenceNum = -9999;

	demo_t* demoBackup = new demo_t(); 
	StateVars* stateBackup = new StateVars();

	qboolean recovering = qfalse;
	while (state->oldSize > 0) {
		*demoBackup = demo;
		*stateBackup = *state;
		recovering = qfalse;
		Com_Printf(".");
		goto cutcontinue;
	cutreset:
		// message had some kind of bug. reset and go again.
		demo = *demoBackup;
		*state = *stateBackup;
		if (state->oldSize <= 0) {
			goto cuterror;
		}
		state->fileOffset++; // just go one byte further and try again (cringe i know)
		stateBackup->fileOffset++;
		state->oldSize--;
		stateBackup->oldSize--;
		Com_Printf("X");
		recovering = qtrue;
		state->framesSaved = 0;
		stateBackup->framesSaved = 0;
	cutcontinue:
		//if (isCompressedFile) {
		//	state->oldDataRaw.clear();
		//	MSG_InitRaw(&state->oldMsg, &state->oldDataRaw); // Input message
		//}
		//else 
		{
			MSG_Init(&state->oldMsg, state->oldData, sizeof(state->oldData)); // Input message
		}
		/* Read the sequence number */
		if (FS_Read(&demo.cut.Clc.serverMessageSequence, 4, state->oldHandle) != 4)
			goto cuterror;
		demo.cut.Clc.serverMessageSequence = LittleLong(demo.cut.Clc.serverMessageSequence);
		state->maxSequenceNum = std::max(state->maxSequenceNum, demo.cut.Clc.serverMessageSequence);
		state->oldSize -= 4;
		/* Read the message size */
		if (FS_Read(&state->oldMsg.cursize, 4, state->oldHandle) != 4)
			goto cuterror;
		state->oldMsg.cursize = LittleLong(state->oldMsg.cursize);
		state->oldSize -= 4;
		/* Negative size signals end of demo */
		if (state->oldMsg.cursize < 0)
			goto cutreset;
		if (state->oldMsg.cursize > state->oldMsg.maxsize)
			goto cutreset;
		/* Read the actual message */
		//if (state->oldMsg.raw) {
		//	state->oldMsg.dataRaw->resize(state->oldMsg.cursize);
		//	if (FS_Read(state->oldMsg.dataRaw->data(), state->oldMsg.cursize, state->oldHandle) != state->oldMsg.cursize) {
		//		goto cuterror;
		//	}
		//}
		//else 
		{
			if (FS_Read(state->oldMsg.data, state->oldMsg.cursize, state->oldHandle) != state->oldMsg.cursize)
				goto cutreset;
		}
		state->oldSize -= state->oldMsg.cursize;
		// init the bitstream
		MSG_BeginReading(&state->oldMsg);
		// Skip the reliable sequence acknowledge number
		MSG_ReadLong(&state->oldMsg);
		//
		// parse the message
		//
		while (1) {
			bool malformedMessageCaught = false;
			byte cmd;
			if (state->oldMsg.readcount > state->oldMsg.cursize) {
				if (!recovering) {
					Com_DPrintf("Demo cutter, read past end of server message.\n");
				}
				goto cutreset;
			}
			cmd = MSG_ReadByte(&state->oldMsg);
			state->wasFirstCommandByte = (qboolean)!state->firstCommandByteRead;
			state->firstCommandByteRead = qtrue;
			cmd = generalizeGameSVCOp(cmd,state->demoType);
			if (cmd == svc_EOF_general) {
				int testlookahead = MSG_LookaheadByte(&state->oldMsg);
				// TODO Check for svc_state->extension/svc_voip (ioq3/wolfcamql)
				if (state->wasFirstCommandByte) {
					// check for hidden meta content
					const char* maybeMeta = demoCutReadPossibleMetadata(&state->oldMsg, state->demoType);
					if (maybeMeta) {

						state->jsonPreviousMetaDocument = new rapidjson::Document();
						if (state->jsonPreviousMetaDocument->Parse(maybeMeta).HasParseError() || !state->jsonPreviousMetaDocument->IsObject()) {
							// We won't quit demo cutting over this. It's whatever. We don't wanna make a demo unusable just because it contains bad
							// metadata. Kinda goes against the spirit. This is a different approach from above with the main metadata, where an error in that
							// will quit the process. Because the user can after all just adjust and fix the commandline.
							std::cout << "Old demo appears to contain metadata, but wasn't able to parse it. Discarding.\n";
							break;
						}

						// Copy any old values to the new meta unless they already exist.
						if (!state->jsonMetaDocument) {
							state->jsonMetaDocument = new rapidjson::Document();
							state->jsonMetaDocument->SetObject();
						}

						if (state->jsonPreviousMetaDocument->HasMember("oco") && !state->originalCutOffsetRead) {
							state->originalFileAbsoluteCutOffset += (*state->jsonPreviousMetaDocument)["oco"].GetInt64();
							state->originalCutOffsetRead = qtrue;
						}

						for (rapidjson::Value::MemberIterator it = state->jsonPreviousMetaDocument->MemberBegin(); it != state->jsonPreviousMetaDocument->MemberEnd(); it++) {
							
							const char* newName = NULL;
							
							if (state->demoCutStartsAtZero || it->name == "of" || it->name == "oco" || it->name == "odm" || it->name == "oip" || it->name == "ost") {
								newName = it->name.GetString();
							} else {
								// We add "_" before the name because a lot of metadata can potentially stop being meaningful once we do a cut to the file with existing metadata,
								// since metadata can contain stuff relative to positioning of events inside a demo and metadata can be completely custom.
								// We thus indicate the "generation" of the metadata by the amount of underscores before it.
								// But if the demo cut is starting at 0, we can keep the original name.
								// Also, metadata we KNOW to be ok to copy we keep (like original filename)
								newName = va("_%s", it->name.GetString());
							}

							if (!state->jsonMetaDocument->HasMember(newName)) {

								rapidjson::Value newNameRapid(newName, state->jsonMetaDocument->GetAllocator());
								//newNameRapid.SetString(newName,strlen(newName));
								std::cout << "Metadata member '" << it->name.GetString() << "' from original demo copied to new demo as " << newName << ".\n";
								state->jsonMetaDocument->AddMember(newNameRapid, it->value, state->jsonMetaDocument->GetAllocator()); // This is move semantics, it will invalidate the original value but thats ok (?)
							}
							else {
								std::cout << "Metadata member '" << it->name.GetString() << "' from original demo overridden by new metadata. Discarding.\n";
							}
						}
					}
				}
				break;
			}
			// skip all the gamestates until we reach needed
			if (state->readGamestate < demo.currentNum) {
				//if (state->readGamestate < (demo.nstate->extNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
				if (cmd == svc_gamestate_general) {
					state->readGamestate++;
				}
				goto cutcontinue;
			}
			// other commands
			switch (cmd) {
			case svc_centerprint_general:
			case svc_locprint_general:
			case svc_cgameMessage_general:
				if (isMOHAADemo) {
					demoCutParseMOHAASVC(&state->oldMsg, state->demoType, cmd, state->SEHExceptionCaught);
					break;
				}
			default:
				Com_DPrintf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cutreset;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				if (!demoCutParseCommandString(&state->oldMsg, &demo.cut.Clc, state->demoType, state->SEHExceptionCaught)) {
					goto cutreset;
				}
				break;
			case svc_gamestate_general:
				//if (state->readGamestate > demo.currentNum && state->demoCurrentTime >= startTime) {
				if (state->readGamestate > demo.currentNum) {
					//Com_DPrintf("Warning: unexpected new gamestate, finishing cutting.\n"); // We dont like this. Unless its not currently cutting anyway.
					//goto cutcomplete;// Actually, who cares. Let's keep the map changes in the cut demo too. Shrug.
					state->newGameStateAfterDemoCutBegun = qtrue;
				} 
				if (!demoCutParseGamestate(&state->oldMsg, &demo.cut.Clc, &demo.cut.Cl,&state->demoType, (qboolean)(state->readGamestate == 0), state->SEHExceptionCaught)) {
					goto cutreset;
				}
				//if(state->readGamestate > 0 ) state->mapRestartCounter++; // we might still get snaps with the old serverTime. so do it at the serverTime reset. idk maybe find a better way in the future but otherwise it makes cutting precisely in later games difficult.
				// Only open if none opened yet.
				if (!state->newHandle) {
					{int dupeIterator = 0;
					while (!dupeIterator || FS_FileExists(state->newName)) {
						if (!dupeIterator) {
							if (outputName) {
								Com_sprintf(state->newName, sizeof(state->newName), "%s%s", outputName, state->ext);
							}
							else {
								Com_sprintf(state->newName, sizeof(state->newName), "%s_fix%s", state->oldName, state->ext);
							}
						}
						else {
							if (outputName) {
								Com_sprintf(state->newName, sizeof(state->newName), "%s(%d)%s", outputName, 1 + dupeIterator, state->ext);
							}
							else {
								Com_sprintf(state->newName, sizeof(state->newName), "%s_fix(%d)%s", state->oldName, 1 + dupeIterator, state->ext);
							}
						}
						dupeIterator++;
					}}
					state->newHandle = FS_FOpenFileWrite(state->newName, compressionSchemeUsed, qfalse); // Maintain the compression scheme of the original file
					if (!state->newHandle) {
						Com_DPrintf("Failed to open %s for target cutting.\n", state->newName);
						return qfalse;
					}
				}
				state->readGamestate++;
				break;
			case svc_snapshot_general:
				if (!demoCutParseSnapshot(&state->oldMsg, &demo.cut.Clc, &demo.cut.Cl, state->demoType, state->SEHExceptionCaught, malformedMessageCaught)) {
					goto cutreset;
				}
				if (demo.cut.Cl.snap.snapIssues) {
					state->framesSaved = std::min(state->framesSaved,1); // force writing non delta again
				}
				state->psGeneralPMType = generalizeGameValue<GMAP_PLAYERMOVETYPE, SAFE>(demo.cut.Cl.snap.ps.pm_type,state->demoType);
				if (messageOffset++ == 0) {
					// first message in demo. Get servertime offset from here to cut correctly.
					state->demoStartTime = demo.cut.Cl.snap.serverTime;
					//startTime += demo.cut.Cl.snap.serverTime;
					//endTime += demo.cut.Cl.snap.serverTime;
				}
				if (demo.cut.Clc.serverMessageSequence == state->maxSequenceNum) { // See demoHighlightFind.cpp for detailed commentary
					if (demo.cut.Cl.snap.serverTime < state->lastKnownInOrderTime) {

						if (demo.cut.Cl.snap.serverTime > 10000) {
							// This is a non-critical warning, mostly for debugging. It used to be more dangerous.
							std::cerr << "demo.cut.Cl.snap.serverTime < state->lastKnownTime && demo.cut.Clc.serverMessageSequence == state->maxSequenceNum but demo.cut.Cl.snap.serverTime > 10000;  delta " << (state->lastKnownTime - demo.cut.Cl.snap.serverTime) << ", state->demoCurrentTime " << state->demoCurrentTime << ", state->demoBaseTime " << state->demoBaseTime << ", state->demoStartTime " << state->demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", state->lastKnownTime " << state->lastKnownTime << " (" << sourceDemoFile << ")\n";
						}

						state->demoBaseTime = state->demoCurrentTime; // Remember fixed offset into demo time.
						state->demoStartTime = demo.cut.Cl.snap.serverTime;
						state->mapRestartCounter++;
						state->framesSaved = 0;
					}
					state->lastKnownInOrderTime = demo.cut.Cl.snap.serverTime;
				}
				state->demoCurrentTime = state->demoBaseTime + demo.cut.Cl.snap.serverTime - state->demoStartTime;
				state->lastKnownTime = demo.cut.Cl.snap.serverTime;
				if (demo.lastPMT != demo.cut.Cl.snap.ps.pm_type) {
					demo.lastPMTChange = state->demoCurrentTime;
					demo.lastPMT = demo.cut.Cl.snap.ps.pm_type;
				}
				break;
			case svc_download_general:
				// read block number
				state->buf = MSG_ReadShort(&state->oldMsg);
				if (!state->buf)	//0 block, read file size
					MSG_ReadLong(&state->oldMsg);
				// read block size
				state->buf = MSG_ReadShort(&state->oldMsg);
				// read the data block
				for (; state->buf > 0; state->buf--)
					MSG_ReadByte(&state->oldMsg);
				break;
			case svc_setgame_general:
				{
					static char	newGameDir[MAX_QPATH];
					int i = 0;
					while (i < MAX_QPATH) {
						int next = MSG_ReadByte(&state->oldMsg);
						if (next) {
							newGameDir[i] = next;
						}
						else {
							break;
						}
						i++;
					}
					newGameDir[i] = 0;
					// But here we stop, and don't do more. If this goes horribly wrong sometime, you might have to go and actually do something with this.
				}
				break;
			case svc_mapchange_general:
				// nothing to parse.
				break;
			}
		}
		int firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
		// process any new server commands
		for (; demo.cut.Clc.lastExecutedServerCommand <= demo.cut.Clc.serverCommandSequence; demo.cut.Clc.lastExecutedServerCommand++) {
			char* command = demo.cut.Clc.serverCommands[demo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
			Cmd_TokenizeString(command);
			char* cmd = Cmd_Argv(0);
			//if (cmd[0] && !firstServerCommand) {
			//	firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
			//}
			if (!cmd[0]) {
				continue;
			}
			if (!strcmp(cmd, "bcs0") || !strcmp(cmd, "bcs1") || !strcmp(cmd, "bcs2")) {
				char* test = demoCutHandleBigConfigString(cmd, 0);
				if (test) {
					//demoErrorFlags |= DERR_ATYPICALBUTLEGAL; // already doing this further down...
					//demoErrors << "Not an error: Demo uses bcs0/bcs1/bcs2\n";
					//std::cerr << "Not an error: Demo uses bcs0/bcs1/bcs2 (" << DPrintFLocation << ")\n";
					command = test;
					Cmd_TokenizeString(command);
				}
			}
			if (!strcmp(cmd, "cs")) {
				if (!demoCutConfigstringModified(&demo.cut.Cl,state->demoType)) {
					goto cutreset;
				}
			}
			else if (!strcmp(cmd, "map_restart")) {
				state->mapRestartCounter++;
			}
		}

		int64_t cutStartOffset = 0;

		//if (state->demoCurrentTime > endTime) {
		if (state->framesSaved > 0) {
			/* this msg is in range, write it */
			if (state->framesSaved > Q_max(10, demo.cut.Cl.snap.messageNum - demo.cut.Cl.snap.deltaNum) || state->newGameStateAfterDemoCutBegun) { // Hmm did I do this? I don't recall... NEW: state->newGameStateAfterDemoCutBegun: If there is a new gamestate, may as well just start just dumping the messages now.
				demoCutWriteDemoMessage(&state->oldMsg, state->newHandle, &demo.cut.Clc);
			}
			else {
				demoCutWriteDeltaSnapshot(firstServerCommand, state->newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl,state->demoType, createCompressedOutput);
			}
			state->framesSaved++;
		}
		//else if (demo.cut.Cl.snap.serverTime >= startTime) {
		//else if (state->demoCurrentTime >= startTime) {
		else if(demo.cut.Cl.newSnapshots){
			if (!state->jsonMetaDocument && !noForcedMeta && !state->jsonWritten) {
				state->jsonMetaDocument = new rapidjson::Document();
				state->jsonMetaDocument->SetObject();
			}
			if (state->jsonMetaDocument && !state->jsonWritten) {
				// TODO: Save "oto": Original total offset. Throughout all cuts, what's the offset from the original file now?

				if (!state->jsonMetaDocument->HasMember("of")) { // original filename
					std::string oldPathStr(oldPath);
					std::string oldBasename = oldPathStr.substr(oldPathStr.find_last_of("/\\") + 1);
					state->jsonMetaDocument->AddMember("of", rapidjson::Value(oldBasename.c_str(), state->jsonMetaDocument->GetAllocator()).Move(),state->jsonMetaDocument->GetAllocator());
				}
				if (!state->jsonMetaDocument->HasMember("odm")) { // original date modified (unix)
					std::filesystem::file_time_type filetime = std::filesystem::last_write_time(oldPath);
					//time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));
					time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime - std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));
					state->jsonMetaDocument->AddMember("odm", oldDemoDateModified, state->jsonMetaDocument->GetAllocator());
				}
				if (!state->jsonMetaDocument->HasMember("cso")) { // cut start offset? to detect imperfect timing and adjust for it? Easier than to feed it from the outside.
					state->jsonMetaDocument->AddMember("cso", cutStartOffset, state->jsonMetaDocument->GetAllocator());
				}
				else {
					(*state->jsonMetaDocument)["cso"] = cutStartOffset;
					std::cout << "Overriding old 'cso' (cut start offset) metadata value with '"<<cutStartOffset << "'\n";
				}

				state->originalFileAbsoluteCutOffset += state->demoCurrentTime;
				if (!state->jsonMetaDocument->HasMember("oco")) { // original cut offset. aka absolute offset from start of the once ancestral original demo before any cutting was done. this value is read back by subsequent cutting into state->originalFileAbsoluteCutOffset and then added to the new offset.
					state->jsonMetaDocument->AddMember("oco", state->originalFileAbsoluteCutOffset, state->jsonMetaDocument->GetAllocator());
				}
				else {
					(*state->jsonMetaDocument)["oco"] = state->originalFileAbsoluteCutOffset;
					std::cout << "Overriding old 'oco' (original cut offset) metadata value with '"<< state->originalFileAbsoluteCutOffset << "'\n";
				}


				if (!state->jsonMetaDocument->HasMember("wr")) {
					state->jsonMetaDocument->AddMember("wr", "DemoFixer", state->jsonMetaDocument->GetAllocator());
				}
				else {
					(*state->jsonMetaDocument)["wr"] = "DemoCutter";
					std::cout << "Overriding old 'wr' (writer) metadata value with 'DemoCutter'\n";
				}
				rapidjson::StringBuffer sb;
				rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
				state->jsonMetaDocument->Accept(writer);
				const char* finalJsonMetaString = sb.GetString();
				demoCutWriteEmptyMessageWithMetadata(state->newHandle, &demo.cut.Clc, &demo.cut.Cl, state->demoType, createCompressedOutput,finalJsonMetaString);
				state->jsonWritten = qtrue;
				delete state->jsonMetaDocument;
				state->jsonMetaDocument = NULL;
				if (state->jsonPreviousMetaDocument) {
					delete state->jsonPreviousMetaDocument;
					state->jsonPreviousMetaDocument = NULL;
				}
			}
			demoCutWriteDemoHeader(state->newHandle, &demo.cut.Clc, &demo.cut.Cl,state->demoType,createCompressedOutput);
			demoCutWriteDeltaSnapshot(firstServerCommand, state->newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl,state->demoType,createCompressedOutput);
			// copy rest
			state->framesSaved = 1;
		}
	}
cutcomplete:
	if (state->newHandle) {
		state->buf = -1;
		FS_Write(&state->buf, 4, state->newHandle);
		FS_Write(&state->buf, 4, state->newHandle);
		state->ret = qtrue;
	}
cuterror:
	//remove previosly converted demo from the same cut
	if (state->newHandle) {
		/*memset(newName, 0, sizeof(newName));
		if (demo.currentNum > 0) {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s.%d_cut.mme", state->oldName, demo.currentNum);
		}
		else {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s_cut.mme", state->oldName);
		}
		if (FS_FileExists(newName))
			FS_FileErase(newName);*/
	}
	FS_FCloseFile(state->oldHandle);
	FS_FCloseFile(state->newHandle);

#ifdef _WIN32
	// On Windows we now change the Date modified to that of the original file.
	// TODO Implement for other OSes?
	wchar_t newNameWide[MAX_OSPATH];
	wchar_t oldNameWide[MAX_OSPATH];
	mbstowcs(newNameWide, state->newName,MAX_OSPATH);
	mbstowcs(oldNameWide, va("%s%s", state->oldName, state->ext),MAX_OSPATH); 
	HANDLE hFile = CreateFile(newNameWide, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hFileOld = CreateFile(oldNameWide, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE && hFileOld != INVALID_HANDLE_VALUE) // INVALID_FILE_HANDLE / INVALID_HANDLE_VALUE ? 
	{
		FILETIME filetime;
		//SYSTEMTIME systemfile;

		//GetSystemTime(&systemfile);
		//SystemTimeToFileTime(&systemfile, &filetime);
		GetFileTime(hFileOld, NULL, NULL, &filetime);
		SetFileTime(hFile, NULL, NULL, &filetime);
		CloseHandle(hFileOld);
		CloseHandle(hFile);
	}
#endif

	//


	return state->ret;
}

/*void CL_DemoCut_f(void) {
	double startTime, endTime;
	char demoName[MAX_OSPATH];
	if (Cmd_Argc() < 3) {
		Com_Printf("Usage: demoCut start end (in seconds)\n");
		return;
	}
	startTime = atof(Cmd_Argv(1));
	endTime = atof(Cmd_Argv(2));
	if (endTime <= startTime) {
		Com_Printf("invalid range: %.3f >= %.3f\n", startTime, endTime);
		return;
	}
	if (endTime - startTime < 0.05) {
		Com_Printf("invalid range: less than 50 milliseconds is not allowed\n");
		return;
	}
	// convert to msec 
	startTime *= 1000;
	endTime *= 1000;
	Com_sprintf(demoName, MAX_OSPATH, mme_demoFileName->string);
	if (demoCut(demoName, startTime, endTime)) {
		Com_Printf("Demo %s got successfully cut\n", mme_demoFileName->string);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", mme_demoFileName->string);
	}
}*/


int main(int argcO, char** argvO) {

	popl::OptionParser op("Allowed options");
	auto h = op.add<popl::Switch>("h", "help", "Show help");
	auto m = op.add<popl::Value<std::string>>("m", "meta", "Optionally, add {}-enclosed JSON data that will be attached past the end of an empty first message in the demo.");
	auto n = op.add<popl::Switch>("n", "no-forced-meta", "Don't write any metadata at all if neither --meta is supplied nor metadata found in original demofile. By default a 'of' key is added containing the original demo filename. This is overridden by an 'of' value existing already in the demo to be cut.");
	op.parse(argcO, argvO);
	auto args = op.non_option_args();


	//if (argc < 4) {
	if (args.size() < 1) {
		std::cout << "need 1 arguments at least: demoname, outputfile(optional)";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 1;
	}
	else if (h->is_set()) {
		std::cout << "need 3 arguments at least: demoname, outputfile(optional)\n";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 0;
	}
	initializeGameInfos();
	const char* demoName = NULL;
	const char* outputName = NULL;
	bool mustDeleteOutputName = false;
	//if (argc == 4) {
	if (args.size() == 1) {
		//demoName = argv[1];
		demoName = args[0].c_str();
	}
	//else if(argc == 5) {
	else if(args.size() == 2) {
		//demoName = argv[1];
		demoName = args[0].c_str();
		//outputName = argv[2];
		outputName = args[1].c_str();
		char* filteredOutputName = new char[strlen(outputName)+1];
		sanitizeFilename(outputName, filteredOutputName);
		outputName = filteredOutputName;
		mustDeleteOutputName = true;
	}

	std::vector<std::string> metaDataStrings;
	for (int i = 0; i < m->count(); i++) {
		metaDataStrings.push_back(m->value(i));
	}
	//std::string metaData = m->is_set() ? m->value() : "";

	std::string reframeString = "";
	const char* reframeStringC = NULL;

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	//if (demoCut(demoName, startTime, endTime, outputName, (m->is_set() && metaData.size()) ? metaData.c_str() : NULL, n->is_set())) {
	if (demoFix(demoName,outputName, &metaDataStrings, n->is_set(), reframeStringC)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully cut in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get cut or cut with errors\n", demoName);
	}
	if (mustDeleteOutputName) {
		delete[] outputName;
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif