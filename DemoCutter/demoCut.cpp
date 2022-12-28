#include "demoCut.h"

#define PCRE2_STATIC
#include "jpcre2.hpp"

#ifdef _WIN32
#include <windows.h>
#include "fileapi.h"
#include "Handleapi.h"
#endif

#include <chrono>




// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

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
	bool isReached(int64_t demoTime, int64_t serverTime, int64_t levelStartTime, qboolean isPause, int64_t pauseDuration, int mapRestartCounter) {
		int64_t referenceTime = 0;
		switch (type) {
			case DEMOTIME:
				return time <= demoTime;
			case GAMETIME:
				if(!isPause){
					return mapRestartCounter >= skips && time <= (serverTime - levelStartTime);
				}
				else {
					// We can't really know
					return mapRestartCounter >= skips && time <= (serverTime - levelStartTime - pauseDuration);
				}
			case GAMETIME_PAUSE: 
				// This is a bit fucky and logically very bad tbh. But we know the cases in which it's gonna be used so we can try and make it work in that way. Ideally just don't use this, ever.
				// Two pauses right after each other could have identical times in them. We have to kinda just ignore that here because what else can we really do?
				if (isPause) {
					return time <= (serverTime - levelStartTime);
				}
				else {
					// We can't really tell...
					return qfalse;
				}
			case SERVERTIME:
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





qboolean demoCut(const char* sourceDemoFile, demoTime_t startTime, demoTime_t endTime, const char* outputName) {
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
	char			ext[7]{};
	//char			originalExt[7]{};
	demoType_t		demoType;
	int				demoStartTime = 0;
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int				demoCurrentTime = 0;

	int				lastKnownTime = 0;
	int				lastValidSnap = -1;

	int				mapRestartCounter = 0;

	//mvprotocol_t	protocol;

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
	strncpy_s(oldName, sizeof(oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	//ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	strncpy_s(ext, sizeof(ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalExt, sizeof(originalExt), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	qboolean isCompressedFile = qfalse;
	qboolean createCompressedOutput = qfalse;


	demoCutGetDemoType(sourceDemoFile,ext,&demoType,&isCompressedFile,&demo.cut.Clc.demoCheckFor103);
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

	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue, isCompressedFile,&compressionSchemeUsed);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for cutting.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));

	int messageOffset = 0;


	if (createCompressedOutput) {
		ext[3] = 'c';
	}

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
			byte cmd;
			if (oldMsg.readcount > oldMsg.cursize) {
				Com_Printf("Demo cutter, read past end of server message.\n");
				goto cuterror;
			}
			cmd = MSG_ReadByte(&oldMsg);
			cmd = generalizeGameSVCOp(cmd,demoType);
			if (cmd == svc_EOF_general) {
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
			default:
				Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cuterror;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				demoCutParseCommandString(&oldMsg, &demo.cut.Clc);
				break;
			case svc_gamestate_general:
				//if (readGamestate > demo.currentNum && demoCurrentTime >= startTime) {
				if (readGamestate > demo.currentNum && startTime.isReached(demoCurrentTime,demo.cut.Cl.snap.serverTime,atoi(demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[CS_LEVEL_START_TIME]),(qboolean)(demo.cut.Cl.snap.ps.pm_type==PM_SPINTERMISSION), demoCurrentTime-demo.lastPMTChange,mapRestartCounter)) {
					Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n"); // We dont like this. Unless its not currently cutting anyway.
					goto cutcomplete;
				}
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl,&demoType)) {
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
						Com_Printf("Failed to open %s for target cutting.\n", newName);
						return qfalse;
					}
				}
				readGamestate++;
				break;
			case svc_snapshot_general:
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType)) {
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
			if (cmd[0]) {
				firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
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
		//if (demoCurrentTime > endTime) {
		if (endTime.isSurpassed(demoCurrentTime, demo.cut.Cl.snap.serverTime, atoi(demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_LEVEL_START_TIME]), (qboolean)(demo.cut.Cl.snap.ps.pm_type == PM_SPINTERMISSION), demoCurrentTime - demo.lastPMTChange,mapRestartCounter)) {
			goto cutcomplete;
		}
		else if (framesSaved > 0) {
			/* this msg is in range, write it */
			if (framesSaved > Q_max(10, demo.cut.Cl.snap.messageNum - demo.cut.Cl.snap.deltaNum)) { // Hmm did I do this? I don't recall...
				demoCutWriteDemoMessage(&oldMsg, newHandle, &demo.cut.Clc);
			}
			else {
				demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl,demoType, createCompressedOutput);
			}
			framesSaved++;
		}
		//else if (demo.cut.Cl.snap.serverTime >= startTime) {
		//else if (demoCurrentTime >= startTime) {
		else if (demo.cut.Cl.newSnapshots && startTime.isReached(demoCurrentTime, demo.cut.Cl.snap.serverTime, atoi(demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_LEVEL_START_TIME]), (qboolean)(demo.cut.Cl.snap.ps.pm_type == PM_SPINTERMISSION), demoCurrentTime - demo.lastPMTChange,mapRestartCounter)) {
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


int main(int argc, char** argv) {
	if (argc < 4) {
		std::cout << "need 3 arguments at least: demoname, outputfile(optional), start and endtime";
		return 1;
	}
	char* demoName = NULL;
	char* outputName = NULL;
	//float startTime = 0;
	//float endTime = 0;
	demoTime_t startTime;
	demoTime_t endTime;
	if (argc == 4) {
		demoName = argv[1];
		//startTime = atof(argv[2]);
		startTime = timeParse(argv[2]);
		//endTime = atof(argv[3]);
		endTime = timeParse(argv[3]);
	}
	else if(argc == 5) {
		demoName = argv[1];
		outputName = argv[2];
		char* filteredOutputName = new char[strlen(outputName)+1];
		sanitizeFilename(outputName, filteredOutputName);
		strcpy(outputName, filteredOutputName);
		delete[] filteredOutputName;
		//startTime = atof(argv[3]);
		startTime = timeParse(argv[3]);
		//endTime = atof(argv[4]);
		endTime = timeParse(argv[4]);
	}
	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	if (demoCut(demoName, startTime, endTime, outputName)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully cut in %.5f seconds\n", demoName,seconds);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif