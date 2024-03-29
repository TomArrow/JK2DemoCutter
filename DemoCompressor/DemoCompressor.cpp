#include "demoCut.h"


#ifdef _WIN32
#include <windows.h>
#include "fileapi.h"
#include "Handleapi.h"
#endif
#include <chrono>

#ifdef MSG_READBITS_TRANSCODE
extern msg_t* transcodeTargetMsg;
#endif




// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;








#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


qboolean demoCompress(const char* sourceDemoFile, const char* outputName) {
	fileHandle_t	oldHandle = 0;
	fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte>	oldDataRaw;
	msg_t			newMsg;
	byte			newData[MAX_MSGLEN];
	std::vector<byte>	newDataRaw;
	int				oldSize;
	char			oldName[MAX_OSPATH];
	char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	//char* ext;
	char			ext[7]{};
	char			originalExt[7]{};
	demoType_t		demoType;
	int				demoStartTime = 0;
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int				demoCurrentTime = 0;

	int				lastKnownTime = 0;

	int				mapRestartCounter = 0;
	qboolean		isCompressedFile = qfalse;
	qboolean		createCompressedOutput = qtrue;
	bool SEHExceptionCaught = false;


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
	//strncpy_s(oldName, sizeof(oldName), sourceDemoFile, strlen(sourceDemoFile) - 6);
	//strncpy_s(ext, sizeof(ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalExt, sizeof(originalExt), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(sourceDemoFile,ext, oldName,&demoType,&isCompressedFile, &demo.cut.Clc);


	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	strcpy_s(originalExt, sizeof(originalExt), ext);

	/*char specialTypeChar = ext[3];
	ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	createCompressedOutput = (qboolean) !isCompressedFile;

	/*demo.cut.Clc.demoCheckFor103 = qfalse;
	if (!*ext) {
		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		strncpy_s(ext, sizeof(ext), ".dm_15", 6); 
		demo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}*/
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, originalExt), &oldHandle, qtrue, isCompressedFile);
	if (!oldHandle) {
		Com_DPrintf("Failed to open %s for compressing.\n", oldName);
		return qfalse;
	}

	int messageOffset = 0;

	if (demoType >= DM3_MOHAA_PROT_6 && demoType <= DM3_MOHAA_PROT_15) { // TODO handle this smarter in some central place, ideally in demoCutGetDemoType
		if (createCompressedOutput) {
			strcpy_s(ext, sizeof(ext), ".dm3_c");
		}
		else {
			strcpy_s(ext, sizeof(ext), ".dm3");
		}
	}
	else {
		if (createCompressedOutput) {
			ext[3] = 'c';
		}
		else {
			ext[3] = '_';
		}
	}


	// Open new file
	{int dupeIterator = 0;
	while (!dupeIterator || FS_FileExists(newName)) {
		if (!dupeIterator) {
			if (outputName) {
				Com_sprintf(newName, sizeof(newName), "%s%s", outputName, ext);
			}
			else {
				Com_sprintf(newName, sizeof(newName), "%s_c%s", oldName, ext);
			}
		}
		else {
			if (outputName) {
				Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputName, 1 + dupeIterator, ext);
			}
			else {
				Com_sprintf(newName, sizeof(newName), "%s_c(%d)%s", oldName, 1 + dupeIterator, ext);
			}
		}
		dupeIterator++;
	}}

	newHandle = FS_FOpenFileWrite(newName,createCompressedOutput ? FILECOMPRESSION_LZMA : FILECOMPRESSION_NONE, qfalse);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}
	



	//	Com_SetLoadingMsg("Cutting the demo...");
	while (oldSize > 0) {
	//cutcontinue:
		if (isCompressedFile) {
			oldDataRaw.clear();
			MSG_InitRaw(&oldMsg, &oldDataRaw); // Input message
		}
		else {
			MSG_Init(&oldMsg, oldData, sizeof(oldData)); // Input message
		}

		Com_Memset(&newMsg, 0, sizeof(newMsg)); // Is this necessary? idk, wanted to be safe.

		if (createCompressedOutput) { // Ironically if the output is compressed we do not compress the message to get a better compression ratio later
			newDataRaw.clear();
			MSG_InitRaw(&newMsg, &newDataRaw); // Output message
		}
		else {
			MSG_Init(&newMsg, newData,sizeof(newData)); // Output message
		}
		
		MSG_Bitstream(&newMsg);

		transcodeTargetMsg = &newMsg;

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
		if (oldMsg.raw){
			oldMsg.dataRaw->resize(oldMsg.cursize);
			if (FS_Read(oldMsg.dataRaw->data(), oldMsg.cursize, oldHandle) != oldMsg.cursize){
				goto cuterror;}
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
			cmd = generalizeGameSVCOp(cmd, demoType);
			if (cmd == svc_EOF_general) {
				break;
			}
			// skip all the gamestates until we reach needed
			//if (readGamestate < demo.currentNum) {
			//	//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
			//	if (cmd == svc_gamestate) {
			//		readGamestate++;
			//	}
			//	goto cutcontinue;
			//}
			// other commands
			switch (cmd) {
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
				//if (readGamestate > demo.currentNum) {
				//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n"); // We dont like this. Unless its not currently cutting anyway.
				//	goto cutcomplete;
				//}
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, &demoType, (qboolean)(readGamestate == 0), SEHExceptionCaught)) {
					goto cuterror;
				}
				
				readGamestate++;
				break;
			case svc_snapshot_general:
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType, SEHExceptionCaught, malformedMessageCaught)) {
					goto cuterror;
				}
				/*if (messageOffset++ == 0) {
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
				}*/
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
			//if (!strcmp(cmd, "map_restart")) {
			//	mapRestartCounter++;
			//}
		}
		
		int sequenceToWrite = LittleLong(demo.cut.Clc.serverMessageSequence);
		int cursizeToWrite = LittleLong(newMsg.cursize);
		FS_Write(&sequenceToWrite, 4, newHandle);
		FS_Write(&cursizeToWrite, 4, newHandle);
		if (newMsg.raw) {
			FS_Write(newMsg.dataRaw->data(), newMsg.dataRaw->size(), newHandle);
		}
		else {
			FS_Write(newMsg.data, newMsg.cursize, newHandle);
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
	mbstowcs(newNameWide, newName, MAX_OSPATH);
	mbstowcs(oldNameWide, va("%s%s", oldName, originalExt), MAX_OSPATH);
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
	if (argc < 2) {
		std::cout << "need 1 arguments at least: demoname, outputfile(optional)";
		return 1;
	}
	initializeGameInfos();
	char* demoName = NULL;
	char* outputName = NULL;
	if (argc == 2) {
		demoName = argv[1];
	}
	else if (argc == 3) {
		demoName = argv[1];
		outputName = argv[2];
		char* filteredOutputName = new char[strlen(outputName) + 1];
		sanitizeFilename(outputName, filteredOutputName);
		strcpy(outputName, filteredOutputName);
		delete[] filteredOutputName;
	}

	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	if (demoCompress(demoName, outputName)) {
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()/1000000.0f;
		Com_Printf("Demo %s got successfully compressed in %.5f seconds \n", demoName, seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get compressed or compressed with errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif