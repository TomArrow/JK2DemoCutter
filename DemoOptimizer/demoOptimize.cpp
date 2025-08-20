#include "demoCut.h"


#ifdef _WIN32
#include <windows.h>
#include "fileapi.h"
#include "Handleapi.h"
#endif
#include <chrono>
#include "popl.hpp"
#include <sstream>

#ifdef MSG_READBITS_TRANSCODE
extern msg_t* transcodeTargetMsg;
extern msg_t* deltaTargetMsg;
#endif
#include <set>


#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"

// TODO Make work with dm3?


extern bool debugWrite;
extern bool debugRead;
extern std::stringstream ssDebugRead;
extern std::stringstream ssDebugWrite;

class ExtraOptimizeOptions {
public:
	int minMsec = 0;
	bool commandTimeSlash = false;
	bool bruteForce = false;
	int	debugMessage = -1;
};

inline size_t streamsize(std::ostream* stream) {
	size_t oldPointer = stream->tellp();
	stream->seekp(0, std::ios::end);
	size_t end = stream->tellp();
	stream->seekp(oldPointer, std::ios::beg);
	return end;
}

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;

#define MAX_MSEC 500

qboolean optimizeCommands = qtrue;
qboolean optimizeSnaps = qtrue;
int		 optimizeSnapsSafety = 10000; // make a non-delta every now and then to protect against corruption. e.g. every 10000 packets. at 100 fps thats like 100 seconds



bool canUseOldSnap(clSnapshot_t* newSnap, clSnapshot_t* oldSnap, int nonIssueSnap) {
	bool canUse = qtrue;
	canUse = canUse && !((oldSnap->snapFlags ^ newSnap->snapFlags) & SNAPFLAG_SERVERCOUNT); // make sure theyre from the same map
	canUse = canUse && newSnap->messageNum > oldSnap->messageNum; // make sure we're in order. yea ik... shit happens
	canUse = canUse && newSnap->serverTime > oldSnap->serverTime; // make sure we're in order. yea ik... shit happens
	canUse = canUse && oldSnap->messageNum == nonIssueSnap; // make sure it's the right snap and not gone
	canUse = canUse && oldSnap->valid && !oldSnap->snapIssues; // should be already covered but let's be safe
	canUse = canUse && (newSnap->messageNum - oldSnap->messageNum < PACKET_BACKUP_MIN); // don't delta from extremely old frames (was 100 but make it 32 so vanilla clients dont struggle)
	canUse = canUse && (newSnap->serverTime - oldSnap->serverTime < 1000); // don't delta from extremely old frames
	canUse = canUse && !(demo.cut.Cl.parseEntitiesNum - oldSnap->parseEntitiesNum > MAX_PARSE_ENTITIES_MIN - 128); // good old safety check from parsesnapshot
	return canUse;
}


#ifdef RELDEBUG
//#pragma optimize("", off)
#endif


qboolean demoCompress(const char* sourceDemoFile, const char* outputName, double* fileRatio, const ExtraOptimizeOptions& opts) {
	fileHandle_t	oldHandle = 0;
	fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte>	oldDataRaw;
	msg_t			newMsg;
	msg_t			newMsgRemember;
	byte			newData[MAX_MSGLEN];
	std::vector<byte>	newDataRaw;
	int64_t				oldSizeFull;
	int64_t				oldSize;
	int64_t				oldSizeMessageStart;
	int64_t				messageStartFilePos = 0;
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
	int				nonIssueSnap;
	qboolean		nonIssueSnapSet = qfalse;
	int64_t			deltaSnapcount = 0;
	qboolean		currentMessageWritten = qtrue;
	bool doingSnapsManipulation = opts.minMsec || opts.commandTimeSlash;

	rapidjson::Document* jsonPreviousMetaDocument = NULL;
	rapidjson::Document* jsonMetaDocument = NULL;
	if (doingSnapsManipulation) {

		jsonMetaDocument = new rapidjson::Document();
		jsonMetaDocument->SetObject();
		jsonMetaDocument->AddMember("opt_snapsmanip",1,jsonMetaDocument->GetAllocator());
		//(*jsonMetaDocument)["opt_snapsmanip"] = 1;
		if (opts.commandTimeSlash) {
			//(*jsonMetaDocument)["opt_snapsmanip_cts"] = 1;
			jsonMetaDocument->AddMember("opt_snapsmanip_cts", 1, jsonMetaDocument->GetAllocator());
		}
		if (opts.minMsec) {
			//(*jsonMetaDocument)["opt_snapsmanip_minmsec"] = opts.minMsec;
			jsonMetaDocument->AddMember("opt_snapsmanip_minmsec", opts.minMsec, jsonMetaDocument->GetAllocator());
		}
	}

	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(sourceDemoFile, ext, oldName, &demoType, &isCompressedFile, &demo.cut.Clc);

	if (isCompressedFile) {
		std::cerr << "Optimizing compressed files is not currently supported\n"; // todo make this work. im worried about the dataraw vector. not sure if i have to  clear it when doing the remember reset
		return qfalse;
	}


	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	strcpy_s(originalExt, sizeof(originalExt), ext);

	/*char specialTypeChar = ext[3];
	ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	createCompressedOutput = (qboolean)isCompressedFile;

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
	oldSizeFull = oldSizeMessageStart = oldSize = FS_FOpenFileRead(va("%s%s", oldName, originalExt), &oldHandle, qtrue, isCompressedFile);
	if (!oldHandle) {
		Com_DPrintf("Failed to open %s for optimizing.\n", oldName);
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
				Com_sprintf(newName, sizeof(newName), "%s_opt%s", oldName, ext);
			}
		}
		else {
			if (outputName) {
				Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputName, 1 + dupeIterator, ext);
			}
			else {
				Com_sprintf(newName, sizeof(newName), "%s_opt(%d)%s", oldName, 1 + dupeIterator, ext);
			}
		}
		dupeIterator++;
	}}

	newHandle = FS_FOpenFileWrite(newName, createCompressedOutput ? FILECOMPRESSION_LZMA : FILECOMPRESSION_NONE, qfalse);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	int psCommandTime = INT_MIN;
	int lastWrittenServerTime = INT_MIN;
	std::set<int> serverCommandsToFlush;
	std::set<int> newServerCommands;


	qboolean wasFirstCommandByte = qfalse;
	qboolean firstCommandByteRead = qfalse;

	//	Com_SetLoadingMsg("Cutting the demo...");
	uint64_t skippableCmds = (1 << svc_serverCommand_general) | (1 << svc_EOF_general) | (1 << svc_snapshot_general);
	uint64_t unskippableCmds = ~skippableCmds; // if any of these were received, we can't skip the message. to avoid weird situations we didn't expect.
	while (oldSize > 0) {
	cutcontinue:
		uint64_t cmds = 0;
		newServerCommands.clear();
		bool skipMessage = false;
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
			MSG_Init(&newMsg, newData, sizeof(newData)); // Output message
		}

		MSG_Bitstream(&newMsg);

		transcodeTargetMsg = &newMsg;

		messageStartFilePos = FS_FTell(oldHandle);
		oldSizeMessageStart = oldSize;

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

		currentMessageWritten = qfalse;

		oldSize -= (int64_t)oldMsg.cursize;
		// init the bitstream
		MSG_BeginReading(&oldMsg);
		// Skip the reliable sequence acknowledge number
		MSG_ReadLong(&oldMsg);


		for (auto it = serverCommandsToFlush.begin(); it != serverCommandsToFlush.end(); it++) {
			char* command = demo.cut.Clc.serverCommands[*it & (MAX_RELIABLE_COMMANDS - 1)];
			MSG_WriteByte(&newMsg,specializeGeneralSVCOp(svc_serverCommand_general,demoType));
			MSG_WriteLong(&newMsg,*it);
			MSG_WriteString(&newMsg,command,demoType);
		}

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
			newMsgRemember = newMsg;
			cmd = MSG_ReadByte(&oldMsg);
			wasFirstCommandByte = (qboolean)!firstCommandByteRead;
			firstCommandByteRead = qtrue;
			cmd = generalizeGameSVCOp(cmd, demoType);
			if (cmd >= 0 && cmd < 63) {
				cmds |= (1 << cmd);
			}
			else {
				cmds |= (1 << 63);
			}
			if (cmd == svc_EOF_general) {
				int minSize = 0;
				if (wasFirstCommandByte && jsonMetaDocument) { // if we are adding metadata, we must parse the old one. otherwise meh.
					msg_t eofOldMsgRemember = oldMsg;
					transcodeTargetMsg = NULL;
					const char* maybeMeta = demoCutReadPossibleMetadata(&oldMsg, demoType);
					transcodeTargetMsg = &newMsg;
					
					// check for hidden meta content
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

						for (rapidjson::Value::MemberIterator it = jsonPreviousMetaDocument->MemberBegin(); it != jsonPreviousMetaDocument->MemberEnd(); it++) {

							const char* newName = NULL;

							if (_strnicmp(it->name.GetString(),"opt_snapsmanip",sizeof("opt_snapsmanip")-1)) { // if it isnt one of our "registered" keywords, copy it over
								newName = it->name.GetString();
							}
							else {
								// We add "_" before the name because a lot of metadata can potentially stop being meaningful once we do a cut to the file with existing metadata,
								// since metadata can contain stuff relative to positioning of events inside a demo and metadata can be completely custom.
								// We thus indicate the "generation" of the metadata by the amount of underscores before it.
								// But if the demo cut is starting at 0, we can keep the original name.
								// Also, metadata we KNOW to be ok to copy we keep (like original filename)
								newName = va("_%s", it->name.GetString());
							}

							if (!jsonMetaDocument->HasMember(newName)) {

								rapidjson::Value newNameRapid(newName, jsonMetaDocument->GetAllocator());
								//newNameRapid.SetString(newName,strlen(newName));
								std::cout << "Metadata member '" << it->name.GetString() << "' from original demo copied to new demo as " << newName << ".\n";
								jsonMetaDocument->AddMember(newNameRapid, it->value, jsonMetaDocument->GetAllocator()); // This is move semantics, it will invalidate the original value but thats ok (?)
							}
							else {
								std::cout << "Metadata member '" << it->name.GetString() << "' from original demo overridden by new metadata. Discarding.\n";
							}
						}
					}

					if (maybeMeta) {
						// if this was metadata, dump the modified one now.
						rapidjson::StringBuffer sb;
						rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
						jsonMetaDocument->Accept(writer);
						const char* finalJsonMetaString = sb.GetString();
						newMsg = newMsgRemember;
						demoCutWriteEmptyMessageMetadataPart(&newMsg, demoType, finalJsonMetaString);
						minSize = newMsg.cursize;
						delete jsonMetaDocument;
						jsonMetaDocument = NULL;
						if (jsonPreviousMetaDocument) {
							delete jsonPreviousMetaDocument;
						}
					}
				}

				// copy over the rest for a perfect copy
				//(oldMsg.bit >> 3) + 1 == oldMsg.cursize
				//(oldMsg.bit >> 3) == oldMsg.cursize-1
				//oldMsg.bit == (oldMsg.cursize-1) << 8
				int targetBit = (oldMsg.cursize << 3)-1;
				while (oldMsg.bit < targetBit) {
					MSG_ReadBits(&oldMsg, 1);
				}
				// ok we have 1 bit left to do.
				if (oldMsg.bit == targetBit && !createCompressedOutput) {
					msg_t remember = newMsg;
					MSG_ReadBits(&oldMsg, 1);
					newMsg = remember; // remember the old state or we increase the size. we just want the data array to be identical even if that data is irrelevant
				}
				
				if (newMsg.cursize < minSize) { // that's in case we did the metadata. it needs a minsize. :/
					newMsg.cursize = minSize;
				}

				break;
			}

			switch (cmd) {
			default:
				Com_DPrintf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cuterror;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				if (!optimizeCommands) {
					if (!demoCutParseCommandString(&oldMsg, &demo.cut.Clc, demoType, SEHExceptionCaught)) {
						goto cuterror;
					}
				}
				else {
					qboolean wasNew = qfalse;
					if (!demoCutParseCommandString(&oldMsg, &demo.cut.Clc, demoType, SEHExceptionCaught,&wasNew)) {
						newMsg = newMsgRemember;
						goto cuterror; // TODO handle more gracefully? just go next msg?
					}
					if (!wasNew && optimizeCommands // is a dupe, already have it.
						|| serverCommandsToFlush.size() > 0 && serverCommandsToFlush.find(demo.cut.Clc.serverCommandSequence) != serverCommandsToFlush.end() // already have it from flushing
						) { 
						newMsg = newMsgRemember;
					}
					else {
						newServerCommands.insert(demo.cut.Clc.serverCommandSequence);
					}
				}
				break;
			case svc_gamestate_general:
				//if (readGamestate > demo.currentNum && demoCurrentTime >= startTime) {
				//if (readGamestate > demo.currentNum) {
				//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n"); // We dont like this. Unless its not currently cutting anyway.
				//	goto cutcomplete;
				//}
				nonIssueSnapSet = qfalse; // gamestate will invalidate these old ones to delta from. meh we need to actually test this tho.
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, &demoType, (qboolean)(readGamestate == 0), SEHExceptionCaught)) {
					goto cuterror;
				}

				readGamestate++;
				break;
			case svc_snapshot_general:

				if (opts.debugMessage != -1 && opts.debugMessage == demo.cut.Clc.serverMessageSequence) {
					debugRead = true;
				}
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType, SEHExceptionCaught, malformedMessageCaught)) {
					newMsg = newMsgRemember;
					debugRead = false;
					goto cuterror;
				}
				debugRead = false;

				if(opts.minMsec || opts.commandTimeSlash){
					int serverTimeDelta = demo.cut.Cl.snap.serverTime - lastWrittenServerTime;

					if (opts.minMsec && serverTimeDelta < opts.minMsec && demo.cut.Cl.snap.serverTime > lastWrittenServerTime) {
						// skip this message
						skipMessage = true;
					}
					else if (opts.commandTimeSlash && serverTimeDelta < MAX_MSEC && demo.cut.Cl.snap.serverTime > lastWrittenServerTime && demo.cut.Cl.snap.ps.commandTime == psCommandTime) {
						// skip this message
						skipMessage = true;
					}
					psCommandTime = demo.cut.Cl.snap.ps.commandTime;
				}

				// check if we can optimize this one (turn it into a highly efficient delta)
				if ((optimizeSnaps || doingSnapsManipulation) && demo.cut.Cl.snap.valid && !demo.cut.Cl.snap.snapIssues && !(demo.cut.Cl.snap.snapFlags & SNAPFLAG_NOT_ACTIVE)) {
					if (optimizeSnaps && optimizeSnapsSafety && deltaSnapcount >= optimizeSnapsSafety) {
						newMsg = newMsgRemember;
						demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, NULL, &demo.cut.Cl, demoType);
						deltaSnapcount = 0;
					}
					else if (nonIssueSnapSet && optimizeSnaps) {

						int oldbit = newMsg.bit;
						if (opts.bruteForce) {
							int smallestbit = INT_MAX;
							int smallestBitNum = INT_MIN;
							for (int tryNum = nonIssueSnap; tryNum > demo.cut.Cl.snap.messageNum-PACKET_BACKUP_MIN; tryNum--) {
								clSnapshot_t* oldSnap = &demo.cut.Cl.snapshots[tryNum & PACKET_MASK];
								bool canUse = oldSnap->isNonIssueSnap;
								canUse = canUse && canUseOldSnap(&demo.cut.Cl.snap, oldSnap, tryNum);
								if (canUse) {
									newMsg = newMsgRemember;
									if (opts.debugMessage != -1 && opts.debugMessage == demo.cut.Cl.snap.messageNum && demo.cut.Cl.snap.deltaNum == tryNum) {
										debugWrite = true;
									}
									demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, oldSnap, &demo.cut.Cl, demoType, qtrue);
									debugWrite = false;
									if (tryNum == demo.cut.Cl.snap.deltaNum && oldbit != newMsg.bit) {
										std::cerr << "Brute force testing original deltanum but size doesn't match, wtf. Old bit: " << oldbit << ", newbit rewritten old: " << newMsg.bit << ", oldDelta: " << demo.cut.Cl.snap.deltaNum << ", newold: " << oldSnap->messageNum << ", curSnap: " << demo.cut.Cl.snap.messageNum << "\n";
									}
									if (newMsg.bit < smallestbit) {
										smallestbit = newMsg.bit;
										smallestBitNum = tryNum;
									}
								}
							}
							if (smallestBitNum != INT_MIN) {
								clSnapshot_t* oldSnap = &demo.cut.Cl.snapshots[smallestBitNum & PACKET_MASK];
								newMsg = newMsgRemember;
								demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, oldSnap, &demo.cut.Cl, demoType, qtrue); 
								if (smallestbit != newMsg.bit) {
									std::cerr << "Brute force searched for most efficient source snap, but actual application did not match predicted size. Old bit: " << oldbit << ", predicted newbit: " << smallestbit << ", actual newbit: " << newMsg.bit << ", oldDelta: " << demo.cut.Cl.snap.deltaNum << ", newold: " << smallestBitNum << ", curSnap: " << demo.cut.Cl.snap.messageNum << "\n";
								}
								if (smallestbit > oldbit) {
									std::cerr << "Brute-force searched most efficient snapshot, but new snapshot bigger than old one, wtf. Old bit: " << oldbit << ", newbit: " << smallestbit << ", oldDelta: " << demo.cut.Cl.snap.deltaNum << ", newold : " << smallestBitNum << ", curSnap: " << demo.cut.Cl.snap.messageNum << "\n";
								}
							}
						}
						else {

							clSnapshot_t* oldSnap = &demo.cut.Cl.snapshots[nonIssueSnap & PACKET_MASK];

							// ok we have an old and a new frame BUT let's make sure the old is still valid and not phased out from being very old or having some other issues
							bool canUse = canUseOldSnap(&demo.cut.Cl.snap, oldSnap, nonIssueSnap);

							/*canUse = canUse && !((oldSnap->snapFlags ^ demo.cut.Cl.snap.snapFlags) & SNAPFLAG_SERVERCOUNT); // make sure theyre from the same map
							canUse = canUse && demo.cut.Cl.snap.messageNum > oldSnap->messageNum; // make sure we're in order. yea ik... shit happens
							canUse = canUse && demo.cut.Cl.snap.serverTime > oldSnap->serverTime; // make sure we're in order. yea ik... shit happens
							canUse = canUse && oldSnap->messageNum == nonIssueSnap; // make sure it's the right snap and not gone
							canUse = canUse && oldSnap->valid && !oldSnap->snapIssues; // should be already covered but let's be safe
							canUse = canUse && (demo.cut.Cl.snap.messageNum - oldSnap->messageNum < PACKET_BACKUP_MIN); // don't delta from extremely old frames (was 100 but make it 32 so vanilla clients dont struggle)
							canUse = canUse && (demo.cut.Cl.snap.serverTime - oldSnap->serverTime < 1000); // don't delta from extremely old frames
							canUse = canUse && !(demo.cut.Cl.parseEntitiesNum - oldSnap->parseEntitiesNum > MAX_PARSE_ENTITIES_MIN - 128); // good old safety check from parsesnapshot
							*/

							if (canUse) {
								newMsg = newMsgRemember;
								demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, oldSnap, &demo.cut.Cl, demoType, qtrue);
								int newbit = newMsg.bit;
								if (oldbit < newbit) {
									// ok it didnt reduce the size. revert.
									oldSnap = &demo.cut.Cl.snapshots[demo.cut.Cl.snap.deltaNum & PACKET_MASK];
									canUse = oldSnap->isNonIssueSnap;
									canUse = canUse && canUseOldSnap(&demo.cut.Cl.snap, oldSnap, demo.cut.Cl.snap.deltaNum);
									if (canUse) {
										newMsg = newMsgRemember;

										if (opts.debugMessage != -1 && opts.debugMessage == demo.cut.Cl.snap.messageNum) {
											debugWrite = true;
										}
										demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, oldSnap, &demo.cut.Cl, demoType, qtrue);
										debugWrite = false;
										if (oldbit != newMsg.bit) {
											std::cerr << "New snapshot bigger than old one, so rewrote original but size doesn't match, wtf. Old bit: " << oldbit << ", newbit: " << newbit << ", newbit rewritten old: " << newMsg.bit << ", oldDelta: " << demo.cut.Cl.snap.deltaNum << ", newold: " << oldSnap->messageNum << ", curSnap: " << demo.cut.Cl.snap.messageNum << "\n";
										}
									}
								}
								deltaSnapcount++;
							}
						}

					}
					else if (doingSnapsManipulation) {
						// we might have skipped a frame and don't have anything to delta from. we need to write a non-delta. sorry.
						newMsg = newMsgRemember;
						demoCutWriteDeltaSnapshotActual(&newMsg, &demo.cut.Cl.snap, NULL, &demo.cut.Cl, demoType);
						deltaSnapcount = 0;
					}
					else {
						if (demo.cut.Cl.snap.deltaNum >= 0) {
							deltaSnapcount++;
						}
						else {
							deltaSnapcount = 0;
						}
					}
					if (!skipMessage) {
						nonIssueSnap = demo.cut.Cl.snap.messageNum;
						demo.cut.Cl.snap.isNonIssueSnap = demo.cut.Cl.snapshots[demo.cut.Cl.snap.messageNum & PACKET_MASK].isNonIssueSnap = qtrue;
						nonIssueSnapSet = qtrue;
					}
				}
				else {
					if (demo.cut.Cl.snap.deltaNum >= 0) {
						deltaSnapcount++;
					}
					else {
						deltaSnapcount = 0;
					}
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
				if (!demoCutConfigstringModified(&demo.cut.Cl, demoType)) {
					goto cuterror;
				}
			}
			//if (!strcmp(cmd, "map_restart")) {
			//	mapRestartCounter++;
			//}
		}

		if (jsonMetaDocument) {
			// if the original demo had no metadata, but we want to save it, dump it now before the first message.
			rapidjson::StringBuffer sb;
			rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
			jsonMetaDocument->Accept(writer);
			const char* finalJsonMetaString = sb.GetString();
			demoCutWriteEmptyMessageWithMetadata(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType, createCompressedOutput, finalJsonMetaString);
			delete jsonMetaDocument;
			jsonMetaDocument = NULL;
			if (jsonPreviousMetaDocument) {
				delete jsonPreviousMetaDocument;
			}
		}

		if (skipMessage && !(cmds & unskippableCmds)) {

			for (auto it = newServerCommands.begin(); it != newServerCommands.end(); it++) {
				serverCommandsToFlush.insert(*it);
			}
			goto cutcontinue;
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
		serverCommandsToFlush.clear();
		lastWrittenServerTime = demo.cut.Cl.snap.serverTime;
		currentMessageWritten = qtrue;
	}
cutcomplete:
	if (newHandle && !(oldSize > 0 && oldHandle)) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}
cuterror:
	if (oldSize > 0 && oldHandle) {
		std::cerr << "Found data at the end of the demo after parsing. Copying over from start of message.\n";
		static byte buffer[1024*1024*8]; // 8 MB buffer?
		//byte buffer[16]; // 8 MB buffer?
		// we errored or something. copy over the rest of the data.
		if (!FS_Seek(oldHandle, messageStartFilePos, FS_SEEK_SET)) {
			oldSize = oldSizeMessageStart;
			while (oldSize > 0) {
				int64_t toRead = std::min(oldSize, (int64_t)sizeof(buffer));
				int64_t countRead = FS_Read(buffer, toRead, oldHandle);
				int64_t countWritten = 0;
				while (countRead > 0) {
					int64_t countWrittenHere = FS_Write(buffer + countWritten,countRead,newHandle);
					countWritten += countWrittenHere;
					countRead -= countWrittenHere;
				}
				oldSize -= countWritten;
			}
		}
		else {
			std::cerr << "Failed copying over remaining data at the end of the demo, seek failed\n";
		}

	}

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
	int64_t newSize = FS_FTell(newHandle);
	*fileRatio = (double)newSize / (double)oldSizeFull;
	std::cout << va("Demo Optimizer: File ratio %.2f%%: From ",(float)(100.0* (*fileRatio))) << oldSizeFull << " to " << newSize << "\n";
	FS_FCloseFile(oldHandle);
	FS_FCloseFile(newHandle);

	if (streamsize(&ssDebugWrite)) {
		std::ofstream outputHandle;
		outputHandle.open("optimizeDebugWrite.log", std::ios_base::app); // append instead of overwrite
		outputHandle << ssDebugWrite.rdbuf();
		outputHandle.close();
	}
	if (streamsize(&ssDebugRead)) {
		std::ofstream outputHandle;
		outputHandle.open("optimizeDebugRead.log", std::ios_base::app); // append instead of overwrite
		outputHandle << ssDebugRead.rdbuf();
		outputHandle.close();
	}
	MSG_PrintFieldErrors(demoType);

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



int main(int argcO, char** argvO) {

	popl::OptionParser op("Allowed options");
	auto h = op.add<popl::Switch>("h", "help", "Show help");
	auto f = op.add<popl::Switch>("f", "fail-if-no-reduction", "Fail if no filesize reduction was achieved.");
	auto c = op.add<popl::Switch>("c", "commandtime-optimization", "Smoothing: Remove frames in which the followed/main player's command time didn't update.");
	auto s = op.add<popl::Implicit<int>>("s", "snaps-limit", "Smoothing: Limit snapshot rate. Discard snapshots arriving faster than the specified value.", 50);
	auto b = op.add<popl::Switch>("b", "brute-force", "Brute force: Try all possible source snaps to find the most efficient one. Slow.");
	auto D = op.add<popl::Implicit<int>>("D", "debug-message", "Write detailed information about a particular message when reading/writing (values and bit counts).", -1);

	op.parse(argcO, argvO);
	auto args = op.non_option_args();

	ExtraOptimizeOptions opts;
	opts.commandTimeSlash = c->is_set();
	opts.bruteForce = b->is_set();
	opts.minMsec = !s->is_set() ? 0 : (1000 / s->value());
	opts.debugMessage = !D->is_set() ? -1 : D->value();

	if (args.size() < 1) {
		std::cout << "need 1 arguments at least: demoname, outputfile(optional)";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		std::cin.get();
		return 1;
	}
	else if (h->is_set()) {
		std::cout << "need 1 arguments at least: demoname, outputfile(optional)";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 0;
	}
	initializeGameInfos();
	const char* demoName = NULL;
	const char* outputName = NULL;
	bool deleteOutputName = false;
	if (args.size() == 1) {
		demoName = args[0].c_str();
	}
	else if (args.size() == 2) {
		demoName = args[0].c_str();
		outputName = args[1].c_str();
		char* filteredOutputName = new char[strlen(outputName) + 1];
		sanitizeFilename(outputName, filteredOutputName);
		outputName = filteredOutputName;
		//strcpy(outputName, filteredOutputName);
		//delete[] filteredOutputName;
		deleteOutputName = true;
	}

	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	double fileRatio = 100000;
	if (demoCompress(demoName, outputName,&fileRatio,opts)) {
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000000.0f;
		if (fileRatio >= 1.0 && f->is_set()) {
			Com_DPrintf("Demo %s got successfully optimized in %.5f seconds BUT -f/--fail-if-no-reduction was specified and no filesize reduction was achieved (%.4f%%). Failing.\n", demoName, seconds, (float)(100.0f * (fileRatio)));
			return 3;
		} else{
			Com_Printf("Demo %s got successfully optimized in %.5f seconds (%.4f%%)\n", demoName, seconds, (float)(100.0f * (fileRatio)));
			return 0;
		}
	}
	else {
		Com_DPrintf("Demo %s has failed to get compressed or compressed with errors\n", demoName);
		return 2;
	}
	if (deleteOutputName) {
		delete[] outputName;
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif