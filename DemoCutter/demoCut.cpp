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
			parsedTime.skips = atoi(skipNumber.c_str());
		}

		if (number1.size() != 0 && number2.size() == 0 && number3.size() == 0) { // Just a simple number
			parsedTime.time = atoi(number1.c_str());
			parsedTime.success = qtrue;
		}
		else if (number2.size() != 0) { // MM:SS.xxx format
			parsedTime.time = atoi(number2.c_str())*1000;
			if (number1.size() != 0) parsedTime.time += atoi(number1.c_str())*60*1000;
			if (number3.size() != 0) {
				int64_t rawValue = atoi(number3.c_str());
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





//qboolean demoCut(const char* sourceDemoFile, demoTime_t startTime, demoTime_t endTime, const char* outputName, const char* jsonMetaData, bool noForcedMeta) {
qboolean demoCut(const char* sourceDemoFile, demoTime_t startTime, demoTime_t endTime, const char* outputName, const std::vector<std::string>* metaDataStrings, bool noForcedMeta, const char* reframeString) {
	fileHandle_t	oldHandle = 0;
	fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte>	oldDataRaw;
	int				oldSize;
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
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int				demoCurrentTime = 0;

	int				lastKnownTime = 0;
	int				lastValidSnap = -1;

	int				mapRestartCounter = 0;
	bool			SEHExceptionCaught = false;
	//mvprotocol_t	protocol;

	qboolean demoCutStartsAtZero = (qboolean)(startTime.type == DEMOTIME && startTime.time == 0 && startTime.skips == 0);

	int64_t originalFileAbsoluteCutOffset = 0;

	rapidjson::Document* jsonMetaDocument = NULL;
	rapidjson::Document* jsonPreviousMetaDocument = NULL;

	/*if (jsonMetaData) {
		jsonMetaDocument = new rapidjson::Document();
		if (jsonMetaDocument->Parse(jsonMetaData).HasParseError() || !jsonMetaDocument->IsObject()) {
			std::cout << "-m/--meta metadata: Unable to parse as JSON.\n";
			return qfalse;
		}
		if (jsonMetaDocument->HasMember("oco")) {
			originalFileAbsoluteCutOffset = (*jsonMetaDocument)["oco"].GetInt64();
		}
	}*/
	if (metaDataStrings->size() > 1) {
		jsonMetaDocument = new rapidjson::Document();
		jsonMetaDocument->SetObject();
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
				if (!jsonMetaDocument->HasMember(it->name)) {
					jsonMetaDocument->AddMember(it->name, it->value, jsonMetaDocument->GetAllocator()); // This is move semantics, it will invalidate the original value but thats ok (?)
				}
				else {
					std::cout << "Metadata member '" << it->name.GetString() << "' provided more than once. Error.\n";
					return qfalse;
				}
			}
		}
	}
	else if (metaDataStrings->size() == 1) {
		jsonMetaDocument = new rapidjson::Document();
		if (jsonMetaDocument->Parse((*metaDataStrings)[0].c_str()).HasParseError() || !jsonMetaDocument->IsObject()) {
			std::cout << "-m/--meta metadata: Unable to parse as JSON.\n";
			return qfalse;
		}
	}


	// Since not in MME:
	/*if (!play) {
		Com_Printf("Demo cutting is allowed in mme mode only.\n");
		return qfalse;
	}*/
	//startTime += play->startTime;
	//endTime += play->startTime;
	

	//protocol = MV_GetCurrentProtocol();
	//if (protocol == PROTOCOL_UNDEF)
	//	ext = ".dm_16";
	//else
	//	ext = va(".dm_%i", protocol);
	//ext = Cvar_FindVar("mme_demoExt")->string;
	demo.cut.Clc.demoCheckFor103 = qfalse;
	//strncpy_s(oldName, sizeof(oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	//ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	//strncpy_s(ext, sizeof(ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalExt, sizeof(originalExt), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	qboolean isCompressedFile = qfalse;
	qboolean createCompressedOutput = qfalse;


	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(sourceDemoFile,ext,oldName,&demoType,&isCompressedFile,&demo.cut.Clc);
	/*
	char specialTypeChar = ext[3];
	ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	createCompressedOutput = isCompressedFile;


	/*if (!*ext) {
		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}
	else if (!_stricmp(ext,".dm_15")) {

		demoType = DM_15;
		strncpy_s(ext, sizeof(ext), ".dm_15", 6);
		demo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(ext,".dm_16")) {

		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}*/

	fileCompressionScheme_t compressionSchemeUsed = FILECOMPRESSION_NONE;

	const char* oldPath = va("%s%s", oldName, ext);
	oldSize = FS_FOpenFileRead(oldPath, &oldHandle, qtrue, isCompressedFile,&compressionSchemeUsed,qtrue);
	if (!oldHandle) {
		Com_DPrintf("Failed to open %s for cutting.\n", oldName);
		return qfalse;
	}

	int messageOffset = 0;


	if (createCompressedOutput) {
		ext[3] = 'c';
	}

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	qboolean wasFirstCommandByte = qfalse;
	qboolean firstCommandByteRead = qfalse;

	qboolean originalCutOffsetRead = qfalse;

	//	Com_SetLoadingMsg("Cutting the demo...");
	while (oldSize > 0) {
	cutcontinue:
		if (isCompressedFile) {
			oldDataRaw.clear();
			MSG_InitRaw(&oldMsg, &oldDataRaw); // Input message
		}
		else {
			MSG_Init(&oldMsg, oldData, sizeof(oldData)); // Input message
		}
		/* Read the sequence number */
		if (FS_Read(&demo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4)
			goto cuterror;
		demo.cut.Clc.serverMessageSequence = LittleLong(demo.cut.Clc.serverMessageSequence);
		oldSize -= 4;
		/* Read the message size */
		if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4)
			goto cuterror;
		oldMsg.cursize = LittleLong(oldMsg.cursize);
		oldSize -= 4;
		/* Negative size signals end of demo */
		if (oldMsg.cursize < 0)
			break;
		if (oldMsg.cursize > oldMsg.maxsize)
			goto cuterror;
		/* Read the actual message */
		if (oldMsg.raw) {
			oldMsg.dataRaw->resize(oldMsg.cursize);
			if (FS_Read(oldMsg.dataRaw->data(), oldMsg.cursize, oldHandle) != oldMsg.cursize) {
				goto cuterror;
			}
		}
		else {
			if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize)
				goto cuterror;
		}
		oldSize -= oldMsg.cursize;
		// init the bitstream
		MSG_BeginReading(&oldMsg);
		// Skip the reliable sequence acknowledge number
		MSG_ReadLong(&oldMsg);
		//
		// parse the message
		//
		while (1) {
			bool malformedMessageCaught = false;
			byte cmd;
			if (oldMsg.readcount > oldMsg.cursize) {
				Com_DPrintf("Demo cutter, read past end of server message.\n");
				goto cuterror;
			}
			cmd = MSG_ReadByte(&oldMsg);
			wasFirstCommandByte = (qboolean)!firstCommandByteRead;
			firstCommandByteRead = qtrue;
			cmd = generalizeGameSVCOp(cmd,demoType);
			if (cmd == svc_EOF_general) {
				// TODO Check for svc_extension/svc_voip (ioq3/wolfcamql)
				if (wasFirstCommandByte) {
					// check for hidden meta content
					const char* maybeMeta = demoCutReadPossibleMetadata(&oldMsg, demoType);
					if (maybeMeta) {

						jsonPreviousMetaDocument = new rapidjson::Document();
						if (jsonPreviousMetaDocument->Parse(maybeMeta).HasParseError() || !jsonPreviousMetaDocument->IsObject()) {
							// We won't quit demo cutting over this. It's whatever. We don't wanna make a demo unusable just because it contains bad
							// metadata. Kinda goes against the spirit. This is a different approach from above with the main metadata, where an error in that
							// will quit the process. Because the user can after all just adjust and fix the commandline.
							std::cout << "Old demo appears to contain metadata, but wasn't able to parse it. Discarding.\n";
							break;
						}

						// Copy any old values to the new meta unless they already exist.
						if (!jsonMetaDocument) {
							jsonMetaDocument = new rapidjson::Document();
							jsonMetaDocument->SetObject();
						}

						if (jsonPreviousMetaDocument->HasMember("oco") && !originalCutOffsetRead) {
							originalFileAbsoluteCutOffset += (*jsonPreviousMetaDocument)["oco"].GetInt64();
							originalCutOffsetRead = qtrue;
						}

						for (rapidjson::Value::MemberIterator it = jsonPreviousMetaDocument->MemberBegin(); it != jsonPreviousMetaDocument->MemberEnd(); it++) {
							
							const char* newName = NULL;
							
							if (demoCutStartsAtZero || it->name == "of" || it->name == "oco" || it->name == "odm" || it->name == "oip" || it->name == "ost") {
								newName = it->name.GetString();
							} else {
								// We add "_" before the name because a lot of metadata can potentially stop being meaningful once we do a cut to the file with existing metadata,
								// since metadata can contain stuff relative to positioning of events inside a demo and metadata can be completely custom.
								// We thus indicate the "generation" of the metadata by the amount of underscores before it.
								// But if the demo cut is starting at 0, we can keep the original name.
								// Also, metadata we KNOW to be ok to copy we keep (like original filename)
								newName = va("_%s", it->name.GetString());
							}

							if (!jsonMetaDocument->HasMember(newName)) {

								rapidjson::Value newNameRapid;
								newNameRapid.SetString(newName,strlen(newName));
								std::cout << "Metadata member '" << it->name.GetString() << "' from original demo copied to new demo as " << newName << ".\n";
								jsonMetaDocument->AddMember(newNameRapid, it->value, jsonMetaDocument->GetAllocator()); // This is move semantics, it will invalidate the original value but thats ok (?)
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
			if (readGamestate < demo.currentNum) {
				//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
				if (cmd == svc_gamestate_general) {
					readGamestate++;
				}
				goto cutcontinue;
			}
			// other commands
			switch (cmd) {
			case svc_centerprint_general:
			case svc_locprint_general:
			case svc_cgameMessage_general:
				if (isMOHAADemo) {
					demoCutParseMOHAASVC(&oldMsg, demoType, cmd, SEHExceptionCaught);
					break;
				}
			default:
				Com_DPrintf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cuterror;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				if (!demoCutParseCommandString(&oldMsg, &demo.cut.Clc, demoType, SEHExceptionCaught)) {
					goto cuterror;
				}
				break;
			case svc_gamestate_general:
				//if (readGamestate > demo.currentNum && demoCurrentTime >= startTime) {
				if (readGamestate > demo.currentNum && startTime.isReached(demoCurrentTime, demo.cut.Cl.snap.serverTime, atoi(demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[getCS_LEVEL_START_TIME(demoType)]), (qboolean)(demo.cut.Cl.snap.ps.pm_type == PM_SPINTERMISSION), demoCurrentTime - demo.lastPMTChange, mapRestartCounter, NULL)) {
					//Com_DPrintf("Warning: unexpected new gamestate, finishing cutting.\n"); // We dont like this. Unless its not currently cutting anyway.
					//goto cutcomplete;// Actually, who cares. Let's keep the map changes in the cut demo too. Shrug.
					newGameStateAfterDemoCutBegun = qtrue;
				} 
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl,&demoType, (qboolean)(readGamestate == 0), SEHExceptionCaught)) {
					goto cuterror;
				}
				// Only open if none opened yet.
				if (!newHandle) {
					{int dupeIterator = 0;
					while (!dupeIterator || FS_FileExists(newName)) {
						if (!dupeIterator) {
							if (outputName) {
								Com_sprintf(newName, sizeof(newName), "%s%s", outputName, ext);
							}
							else {
								Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
							}
						}
						else {
							if (outputName) {
								Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputName, 1 + dupeIterator, ext);
							}
							else {
								Com_sprintf(newName, sizeof(newName), "%s_cut(%d)%s", oldName, 1 + dupeIterator, ext);
							}
						}
						dupeIterator++;
					}}
					newHandle = FS_FOpenFileWrite(newName, compressionSchemeUsed, qfalse); // Maintain the compression scheme of the original file
					if (!newHandle) {
						Com_DPrintf("Failed to open %s for target cutting.\n", newName);
						return qfalse;
					}
				}
				readGamestate++;
				break;
			case svc_snapshot_general:
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType, SEHExceptionCaught, malformedMessageCaught)) {
					goto cuterror;
				}
				if (messageOffset++ == 0) {
					// first message in demo. Get servertime offset from here to cut correctly.
					demoStartTime = demo.cut.Cl.snap.serverTime;
					//startTime += demo.cut.Cl.snap.serverTime;
					//endTime += demo.cut.Cl.snap.serverTime;
				}
				if (demo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
					demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}
				demoCurrentTime = demoBaseTime + demo.cut.Cl.snap.serverTime - demoStartTime;
				lastKnownTime = demo.cut.Cl.snap.serverTime;
				if (demo.lastPMT != demo.cut.Cl.snap.ps.pm_type) {
					demo.lastPMTChange = demoCurrentTime;
					demo.lastPMT = demo.cut.Cl.snap.ps.pm_type;
				}
				break;
			case svc_download_general:
				// read block number
				buf = MSG_ReadShort(&oldMsg);
				if (!buf)	//0 block, read file size
					MSG_ReadLong(&oldMsg);
				// read block size
				buf = MSG_ReadShort(&oldMsg);
				// read the data block
				for (; buf > 0; buf--)
					MSG_ReadByte(&oldMsg);
				break;
			case svc_setgame_general:
				{
					static char	newGameDir[MAX_QPATH];
					int i = 0;
					while (i < MAX_QPATH) {
						int next = MSG_ReadByte(&oldMsg);
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
			if (!strcmp(cmd, "cs")) {
				if (!demoCutConfigstringModified(&demo.cut.Cl,demoType)) {
					goto cuterror;
				}
			}
			if (!strcmp(cmd, "map_restart")) {
				mapRestartCounter++;
			}
		}

		int64_t cutStartOffset = 0;

		//if (demoCurrentTime > endTime) {
		if (endTime.isSurpassed(demoCurrentTime, demo.cut.Cl.snap.serverTime, atoi(demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[getCS_LEVEL_START_TIME(demoType)]), (qboolean)(demo.cut.Cl.snap.ps.pm_type == PM_SPINTERMISSION), demoCurrentTime - demo.lastPMTChange,mapRestartCounter)) {
			goto cutcomplete;
		}
		else if (framesSaved > 0) {
			/* this msg is in range, write it */
			if (framesSaved > Q_max(10, demo.cut.Cl.snap.messageNum - demo.cut.Cl.snap.deltaNum) || newGameStateAfterDemoCutBegun) { // Hmm did I do this? I don't recall... NEW: newGameStateAfterDemoCutBegun: If there is a new gamestate, may as well just start just dumping the messages now.
				demoCutWriteDemoMessage(&oldMsg, newHandle, &demo.cut.Clc);
			}
			else {
				demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl,demoType, createCompressedOutput);
			}
			framesSaved++;
		}
		//else if (demo.cut.Cl.snap.serverTime >= startTime) {
		//else if (demoCurrentTime >= startTime) {
		else if (demo.cut.Cl.newSnapshots && startTime.isReached(demoCurrentTime, demo.cut.Cl.snap.serverTime, atoi(demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[getCS_LEVEL_START_TIME(demoType)]), (qboolean)(demo.cut.Cl.snap.ps.pm_type == PM_SPINTERMISSION), demoCurrentTime - demo.lastPMTChange,mapRestartCounter,&cutStartOffset)) {
			if (!jsonMetaDocument && !noForcedMeta) {
				jsonMetaDocument = new rapidjson::Document();
				jsonMetaDocument->SetObject();
			}
			if (jsonMetaDocument) {
				// TODO: Save "oto": Original total offset. Throughout all cuts, what's the offset from the original file now?

				if (!jsonMetaDocument->HasMember("of")) { // original filename
					std::string oldPathStr(oldPath);
					std::string oldBasename = oldPathStr.substr(oldPathStr.find_last_of("/\\") + 1);
					jsonMetaDocument->AddMember("of", rapidjson::Value(oldBasename.c_str(), jsonMetaDocument->GetAllocator()).Move(),jsonMetaDocument->GetAllocator());
				}
				if (!jsonMetaDocument->HasMember("odm")) { // original date modified (unix)
					std::filesystem::file_time_type filetime = std::filesystem::last_write_time(oldPath);
					//time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));
					time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime - std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));
					jsonMetaDocument->AddMember("odm", oldDemoDateModified, jsonMetaDocument->GetAllocator());
				}
				if (!jsonMetaDocument->HasMember("cso")) { // cut start offset? to detect imperfect timing and adjust for it? Easier than to feed it from the outside.
					jsonMetaDocument->AddMember("cso", cutStartOffset, jsonMetaDocument->GetAllocator());
				}
				else {
					(*jsonMetaDocument)["cso"] = cutStartOffset;
					std::cout << "Overriding old 'cso' (cut start offset) metadata value with '"<<cutStartOffset << "'\n";
				}

				originalFileAbsoluteCutOffset += demoCurrentTime;
				if (!jsonMetaDocument->HasMember("oco")) { // original cut offset. aka absolute offset from start of the once ancestral original demo before any cutting was done. this value is read back by subsequent cutting into originalFileAbsoluteCutOffset and then added to the new offset.
					jsonMetaDocument->AddMember("oco", originalFileAbsoluteCutOffset, jsonMetaDocument->GetAllocator());
				}
				else {
					(*jsonMetaDocument)["oco"] = originalFileAbsoluteCutOffset;
					std::cout << "Overriding old 'oco' (original cut offset) metadata value with '"<< originalFileAbsoluteCutOffset << "'\n";
				}


				if (!jsonMetaDocument->HasMember("wr")) {
					jsonMetaDocument->AddMember("wr", "DemoCutter", jsonMetaDocument->GetAllocator());
				}
				else {
					(*jsonMetaDocument)["wr"] = "DemoCutter";
					std::cout << "Overriding old 'wr' (writer) metadata value with 'DemoCutter'\n";
				}
				rapidjson::StringBuffer sb;
				rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
				jsonMetaDocument->Accept(writer);
				const char* finalJsonMetaString = sb.GetString();
				demoCutWriteEmptyMessageWithMetadata(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType, createCompressedOutput,finalJsonMetaString);
				delete jsonMetaDocument;
				if (jsonPreviousMetaDocument) {
					delete jsonPreviousMetaDocument;
				}
			}
			demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl,demoType,createCompressedOutput);
			demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl,demoType,createCompressedOutput);
			// copy rest
			framesSaved = 1;
		}
	}
cutcomplete:
	if (newHandle) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}
cuterror:
	//remove previosly converted demo from the same cut
	if (newHandle) {
		/*memset(newName, 0, sizeof(newName));
		if (demo.currentNum > 0) {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s.%d_cut.mme", oldName, demo.currentNum);
		}
		else {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s_cut.mme", oldName);
		}
		if (FS_FileExists(newName))
			FS_FileErase(newName);*/
	}
	FS_FCloseFile(oldHandle);
	FS_FCloseFile(newHandle);

#ifdef _WIN32
	// On Windows we now change the Date modified to that of the original file.
	// TODO Implement for other OSes?
	wchar_t newNameWide[MAX_OSPATH];
	wchar_t oldNameWide[MAX_OSPATH];
	mbstowcs(newNameWide,newName,MAX_OSPATH);
	mbstowcs(oldNameWide, va("%s%s", oldName, ext),MAX_OSPATH); 
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
	if (reframeString) {

		std::cout << "Reframing after cutting... \n";

		char			reframedFilename[MAX_OSPATH];

		int baseFilenameLength = strlen(newName) - strlen(ext);
		strncpy_s(reframedFilename,newName, baseFilenameLength); // Get filename without extension.
		reframedFilename[baseFilenameLength] = 0; // Null-terminate it.
		strncat_s(reframedFilename, "_reframe", sizeof("_reframe")-1); // TODO uh am i using this correctly?
		strncat_s(reframedFilename, ext, strlen(ext)); // TODO uh am i using this correctly?

		for (int i = 0; i < 2; i++) {


			std::stringstream cmdLine;
			//cmdLine << "\"";
#ifndef WIN32
			if (i == 0) cmdLine << "./"; // First try calling reframer .exe in same directory.
#endif
			cmdLine << "DemoReframer \"" << newName << "\" \"" << reframedFilename << "\" \"" << reframeString << "\"";
			std::cout << "trying: " << cmdLine.str() << "\n";
			int returnValue = system(cmdLine.str().c_str());
			if (!returnValue) {
				// Success
				std::cout << "Reframe seems successful.\n";
				break;
			}
			else {
				if (i == 0) {

					std::cout << "Reframe apparently unsuccessful. Trying global executable path next.\n";
				}
				else {
					std::cout << "Reframe apparently failed, sorry.\n";
				}
			}
#ifdef WIN32
			break; // On linux, we try ./DemoReframer and then just DemoReframer to always prefer same folder. On Windows that's default behavior anyway so no need to try twice
#endif
		}
	}


	return ret;
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
	auto r = op.add<popl::Value<std::string>>("r", "reframe", "Optionally, reframe by calling reframer using system() call on the output file. Value same as would be with DemoReframer: Search string or clientnum");
	op.parse(argcO, argvO);
	auto args = op.non_option_args();


	//if (argc < 4) {
	if (args.size() < 3) {
		std::cout << "need 3 arguments at least: demoname, outputfile(optional), start and endtime";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 1;
	}
	else if (h->is_set()) {
		std::cout << "need 3 arguments at least: demoname, outputfile(optional), start and endtime\n";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 0;
	}
	initializeGameInfos();
	const char* demoName = NULL;
	const char* outputName = NULL;
	//float startTime = 0;
	//float endTime = 0;
	demoTime_t startTime;
	demoTime_t endTime;
	bool mustDeleteOutputName = false;
	//if (argc == 4) {
	if (args.size() == 3) {
		//demoName = argv[1];
		demoName = args[0].c_str();
		//startTime = atof(argv[2]);
		//startTime = timeParse(argv[2]);
		startTime = timeParse(args[1].c_str());
		//endTime = atof(argv[3]);
		//endTime = timeParse(argv[3]);
		endTime = timeParse(args[2].c_str());
	}
	//else if(argc == 5) {
	else if(args.size() == 4) {
		//demoName = argv[1];
		demoName = args[0].c_str();
		//outputName = argv[2];
		outputName = args[1].c_str();
		char* filteredOutputName = new char[strlen(outputName)+1];
		sanitizeFilename(outputName, filteredOutputName);
		outputName = filteredOutputName;
		mustDeleteOutputName = true;
		//strcpy(outputName, filteredOutputName);
		//startTime = atof(argv[3]);
		//startTime = timeParse(argv[3]);
		startTime = timeParse(args[2].c_str());
		//endTime = atof(argv[4]);
		//endTime = timeParse(argv[4]);
		endTime = timeParse(args[3].c_str());
	}

	std::vector<std::string> metaDataStrings;
	for (int i = 0; i < m->count(); i++) {
		metaDataStrings.push_back(m->value(i));
	}
	//std::string metaData = m->is_set() ? m->value() : "";

	std::string reframeString = r->is_set() ? r->value() : "";
	const char* reframeStringC = r->is_set() ? reframeString.c_str() : NULL;

	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	//if (demoCut(demoName, startTime, endTime, outputName, (m->is_set() && metaData.size()) ? metaData.c_str() : NULL, n->is_set())) {
	if (demoCut(demoName, startTime, endTime, outputName, &metaDataStrings, n->is_set(), reframeStringC)) {
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