#include "demoCut.h"
#include "DemoReader.h"
#include "DemoReaderLight.h"
#include "ini.h"
#include <vector>
#include <sstream>
#include <set>
#include <chrono>

// TODO attach amount of dropped frames in filename.

// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//






class SlotManager {
	struct sourceDemoMapping {
		int demoIndex;
		int entityNum;
		qboolean isModel;
	};

	int availableSlots = MAX_GENTITIES - 2;
	std::map<int, sourceDemoMapping> mappings;
	typedef std::map<int, sourceDemoMapping>::iterator mappingIterator;
	vec3_t modelDistanceOrigin{};
public:
	int getPlayerSlot(int demoIndex, int clientNum,qboolean* wasNewPlayer) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == clientNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,clientNum,qfalse };
				availableSlots--;
				if (wasNewPlayer) {
					*wasNewPlayer = qtrue;
				}
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	int getEntitySlot(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = MAX_CLIENTS; i < MAX_GENTITIES-2; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,entityNum,qfalse };
				availableSlots--;
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	void setModelDistanceOrigin(vec_t* originA) {
		VectorCopy(originA, modelDistanceOrigin);
	}
	// These are released as soon as they disappear from the demo. We don't need consistency from frame to frame on them. It's just random models/movers etc., they only need to be visible.
	int getModelSlot(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		// Else, create new mapping in free slot.
		for (int i = MAX_CLIENTS; i < MAX_GENTITIES-2; i++) {
			if (mappings.find(i) == mappings.end()) {
				// Free slot!
				mappings[i] = { demoIndex,entityNum,qtrue };
				availableSlots--;
				return i;
			}
		}
		// Error: No free slot found
		return -1;
	}
	int getNormalPlayerSlotIfExists(int demoIndex, int entityNum) { // Since we're also copying some players as g2 anim ents. Don't wanna end up with a CG_OBITUARY with an attacker num of 50 lol.
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->first >= MAX_CLIENTS) break;
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		return -1;
	}
	int getSlotIfExists(int demoIndex, int entityNum) {
		// Check if mapping already exists
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				return it->first;
			}
		}
		return -1;
	}
	std::vector<int> freeSlots(int demoIndex) { // Returns the erased slots
		std::vector<int> erasedSlots;
		int countErased = 0;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); ) {
			mappingIterator iteratorHere = it;
			it++;
			if (iteratorHere->second.demoIndex == demoIndex) {
				erasedSlots.push_back(iteratorHere->first);
				mappings.erase(iteratorHere);
				availableSlots++;
				countErased++;
			}
		}
		return erasedSlots;
	}
	/*std::vector<int> getModelMappings(int demoIndex) { // Returns the erased slots
		std::vector<int> modelSlots;
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++; ) {
			
			if (it->second.demoIndex == demoIndex && it->second.isModel) {
				modelSlots.push_back(it->first);
			}
		}
		return modelSlots;
	}*/
	int freeSlot(int demoIndex, int entityNum) {
		for (mappingIterator it = mappings.begin(); it != mappings.end(); it++ ) {
			if (it->second.demoIndex == demoIndex && it->second.entityNum == entityNum) {
				mappings.erase(it);
				availableSlots++;
				return 1;
			}
		}
		return 0;
	}
	qboolean isSlotFree(int slotNum) {
		for (mappingIterator it = mappings.begin(); it != mappings.end();it++ ) {
			if (it->first == slotNum) return qfalse;
		}
		return qtrue;
	}
};

demo_t			demo;






















#ifdef RELDEBUG
//#pragma optimize("", off)
#endif














/*class NameMatch {
public:
	std::string matchedName;
	int clientNum;
};

int getClientNumForDemo(std::string* thisPlayer,DemoReader* reader,qboolean printEndLine = qfalse) {
	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	int clientNumHere = -1;
	int pstrlen = thisPlayer->size();
	if (!pstrlen) return clientNumHere;
	if (pstrlen <= 2 && isdigit((*thisPlayer)[0]) && (pstrlen ==1 || isdigit((*thisPlayer)[1]))) {
		// It's a number. ClientNum. Just parse it.
		clientNumHere = atoi(thisPlayer->c_str());
		tmpConfigString = reader->GetPlayerConfigString(clientNumHere, &tmpConfigStringMaxLength);
		std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength, "n");
		std::cout << *thisPlayer << " (interpreted as clientNum) matches '" << nameHere << "' (" << clientNumHere << ")";
	}
	else {
		std::string thisPlayerLower = *thisPlayer;
		std::transform(thisPlayerLower.begin(), thisPlayerLower.end(), thisPlayerLower.begin(), tolowerSignSafe);

		std::vector<NameMatch> colorStrippedMatches;
		std::vector<NameMatch> caseInsensitiveMatches;
		std::vector<NameMatch> matches;

		// Find matching player name
		for (int c = 0; c < MAX_CLIENTS; c++) {
			tmpConfigString = reader->GetPlayerConfigString(c,&tmpConfigStringMaxLength);
			std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength, "n");
			std::string nameHereLower = nameHere;
			std::transform(nameHereLower.begin(), nameHereLower.end(), nameHereLower.begin(), tolowerSignSafe);

			// Make color stripped version
			const char* sourceCStringName = nameHereLower.c_str();
			int stringLen = strlen(sourceCStringName);
			char* cStringName = new char[stringLen + 1];
			strcpy_s(cStringName, stringLen + 1, sourceCStringName);
			Q_StripColorAll(cStringName);
			std::string nameHereLowerColorStripped = cStringName;

			if (strstr(nameHere.c_str(), thisPlayer->c_str())) {
				NameMatch nm;
				nm.clientNum = c;
				nm.matchedName = nameHere;
				matches.push_back(nm);
			}
			if (strstr(nameHereLower.c_str(), thisPlayerLower.c_str())) {
				NameMatch nm;
				nm.clientNum = c;
				nm.matchedName = nameHere;
				caseInsensitiveMatches.push_back(nm);
			}
			if (strstr(nameHereLowerColorStripped.c_str(), thisPlayerLower.c_str())) {
				NameMatch nm;
				nm.clientNum = c;
				nm.matchedName = nameHere;
				colorStrippedMatches.push_back(nm);
			}
		}

		if (matches.size() > 1) {
			std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
			for (int c = 0; c < matches.size(); c++) {
				std::cout << matches[c].matchedName << "(" << matches[c].clientNum << ")" << std::endl;
			}
			std::cout << "Picking first match '" << matches[0].matchedName << "' (" << matches[0].clientNum << ")";
			clientNumHere = matches[0].clientNum;
		}
		else if (matches.size() == 1) {
			std::cout <<"'" << *thisPlayer << "' matches '" << matches[0].matchedName << "' (" << matches[0].clientNum << ")";
			clientNumHere = matches[0].clientNum;
		}
		else {
			// No match. Try case insensitive
			if (caseInsensitiveMatches.size() > 1) {
				std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
				for (int c = 0; c < caseInsensitiveMatches.size(); c++) {
					std::cout << caseInsensitiveMatches[c].matchedName << "(" << caseInsensitiveMatches[c].clientNum << ")" << std::endl;
				}
				std::cout << "Picking first match '" << caseInsensitiveMatches[0].matchedName << "' ("<<caseInsensitiveMatches[0].clientNum<<")";
				clientNumHere = caseInsensitiveMatches[0].clientNum;
			}
			else if (caseInsensitiveMatches.size() == 1) {
				std::cout << "'" << *thisPlayer << "' matches '" << caseInsensitiveMatches[0].matchedName << "' (" << caseInsensitiveMatches[0].clientNum << ")";
				clientNumHere = caseInsensitiveMatches[0].clientNum;
			}
			else {
				//std::cout << "[WARNING] '" << *thisPlayer << "' matches nothing. Discarding.";
				// No match. Try stripped colors 
				if (colorStrippedMatches.size() > 1) {
					std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
					for (int c = 0; c < colorStrippedMatches.size(); c++) {
						std::cout << colorStrippedMatches[c].matchedName << "(" << colorStrippedMatches[c].clientNum << ")" << std::endl;
					}
					std::cout << "Picking first match '" << colorStrippedMatches[0].matchedName << "' (" << colorStrippedMatches[0].clientNum << ")";
					clientNumHere = colorStrippedMatches[0].clientNum;
				}
				else if (colorStrippedMatches.size() == 1) {
					std::cout << "'" << *thisPlayer << "' matches '" << colorStrippedMatches[0].matchedName << "' (" << colorStrippedMatches[0].clientNum << ")";
					clientNumHere = colorStrippedMatches[0].clientNum;
				}
				else {
					std::cout << "[WARNING] '" << *thisPlayer << "' matches nothing. Discarding.";
					// Done.

				}
			}
		}
	}
	if (printEndLine) {
		std::cout << std::endl;
	}
	return clientNumHere;
}*/


qboolean demoCut( const char* outputName, std::vector<DemoSource>* inputFiles) {
	fileHandle_t	newHandle = 0;
	char			outputNameNoExt[MAX_OSPATH];
	char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	//char* ext;
	char ext[7]{};
	//char originalExt[7]{};
	demoType_t		demoType;


	qboolean createCompressedOutput = qfalse;

	demoCutGetDemoType(outputName, ext, outputNameNoExt, &demoType, &createCompressedOutput);

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	/*ext = (char*)outputName + strlen(outputName) - 6;
	if (!*ext) {
		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext, ".dm_15")) { // TODO Provide way to write 1.03 files? Eh who cares.

		demoType = DM_15;
		ext = ".dm_15";
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	else {
		// TODO Maybe just check output files too and judge based on them?
		demoType = DM_15;
		ext = ".dm_15";
	}*/

	memset(&demo, 0, sizeof(demo));

	std::vector<DemoReaderLight> pingDemoReaders;
	std::vector<DemoReaderWrapper> demoReaders;
	demoReaders.reserve(inputFiles->size());// This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	pingDemoReaders.reserve(inputFiles->size());// This is needed because really strange stuff happens when vectors are resized. It calls destructors on objects and iterators inside the object and whatnot. I don't get it but this ought to solve it.
	for (int i = 0; i < inputFiles->size(); i++) {
		demoReaders.emplace_back();
		pingDemoReaders.emplace_back();
		demoReaders.back().reader.LoadDemo((*inputFiles)[i].demoPath.c_str());
		demoReaders.back().sourceInfo = &(*inputFiles)[i];
		demoReaders.back().targetFramesRead = 0;
		pingDemoReaders.back().LoadDemo((*inputFiles)[i].demoPath.c_str());
	}

	demoCutInitClearGamestate(&demo.cut.Clc, &demo.cut.Cl, 1,0,0);

	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	// Copy over configstrings from first demo.
	// Later maybe we can do something more refined and clever.
	// TODO: JKA?
	int maxConfigStrings = getMaxConfigStrings(demoType); // this is NOT a proper integration. We're still relying on hardcoded CS_Players etc. Just consider JKA NOT being possible atm.
	for (int i = 0; i < maxConfigStrings; i++) {
		if (i >= CS_PLAYERS && i < (CS_PLAYERS + MAX_CLIENTS)) continue; // Player stuff will be copied manually.
		tmpConfigString = demoReaders[0].reader.GetConfigString(i, &tmpConfigStringMaxLength);
		if (strlen(tmpConfigString)) {
			demoCutConfigstringModifiedManual(&demo.cut.Cl, i, tmpConfigString, demoType);
		}
	}

	std::map<int, int> lastSpectatedClientNums; // Need this for later.

	//int copiedPlayerIndex = 0;

	SlotManager slotManager;


	// Find correct player numbers and copy configstrings for players
	for (int i = 0; i < demoReaders.size(); i++) {
		if (demoReaders[i].reader.SeekToAnySnapshotIfNotYet()) { // Make sure we actually have a snapshot parsed, just for whatever's sake. Maybe not necessary? Could help with stuff like DARK Homebase demos, since player configstrings are sent a bit later?
			
			// Get ping info by preparsing entire demo in the lighter demo reader
			//float	medianPingsHere[MAX_CLIENTS];
			//float	medianOfLocalAveragePingsHere[MAX_CLIENTS];
			int	lowestPingsHere[MAX_CLIENTS_MAX];
			qboolean	playerExistsAsEntity[MAX_CLIENTS_MAX];
			pingDemoReaders[i].ReadToEnd();
			//pingDemoReaders[i].GetMedianPingData(medianPingsHere);
			//pingDemoReaders[i].GetMedianOfLocalAveragesPingData(medianOfLocalAveragePingsHere);
			pingDemoReaders[i].GetLowestPingData(lowestPingsHere);
			pingDemoReaders[i].GetPlayersSeen(playerExistsAsEntity);
			pingDemoReaders[i].FreePingData();

			std::set<int> clientNumsUsedHere;
			for (int p = 0; p < demoReaders[i].sourceInfo->playersToCopy.size(); p++) {

				std::string* thisPlayer = &demoReaders[i].sourceInfo->playersToCopy[p];
				//int clientNumHere = getClientNumForDemo(thisPlayer,&demoReaders[i].reader);
				int clientNumHere = demoReaders[i].reader.getClientNumForDemo(thisPlayer);
				if (clientNumHere != -1) {
					std::cout << " [median ping:"<< lowestPingsHere[clientNumHere] <<"]" << std::endl;
					demoReaders[i].playersToCopy.push_back({ clientNumHere,(float)lowestPingsHere[clientNumHere],qfalse });
					clientNumsUsedHere.insert(clientNumHere);
				}
				else {
					std::cout << std::endl;
				}
			}
			if (demoReaders[i].sourceInfo->copyAllPlayers || demoReaders[i].sourceInfo->copyRemainingPlayersAsG2AnimEnts) {
				qboolean asAnimEnts = (qboolean)!demoReaders[i].sourceInfo->copyAllPlayers;
				std::cout << "Copying remaining players"<< (asAnimEnts ? " as G2AnimEnts" : "") << std::endl;
				// We'll also be copying over all other players, either as full players or only as G2 Anim Ents
				// they won't have clientinfo etc.
				int maxClientsThisDemo = demoReaders[i].reader.getMaxClients();
				for (int p = 0; p < maxClientsThisDemo; p++) {
					if (clientNumsUsedHere.find(p) != clientNumsUsedHere.end()) continue; // Don't add players twice that are already added.
					int maxLength;
					const char* playerCS = demoReaders[i].reader.GetPlayerConfigString(p,&maxLength);
					const char* playerTeam = Info_ValueForKey(playerCS,maxLength,"t");
					const char* playerName = Info_ValueForKey(playerCS,maxLength,demoReaders[i].reader.isThisMOHAADemo() ? "name" :"n");
					if (strlen(playerTeam) && atoi(playerTeam) < TEAM_SPECTATOR) { // Don't copy non-existent players and don't copy spectators
						if (playerExistsAsEntity[p]) {
							demoReaders[i].playersToCopy.push_back({ p,(float)lowestPingsHere[p],asAnimEnts });
							std::cout << "Copying " << playerName << " (" << p << ") "<< (asAnimEnts?"as G2AnimEnt ":"") << "[ping adjust:" << lowestPingsHere[p] << "]" << std::endl;
						}
						else {
							std::cout << "Skipping copying " << playerName << " (" << p << ") "<< (asAnimEnts ? "as G2AnimEnt " : "") <<"since it never appears in the source demo" << " [ping adjust:" << lowestPingsHere[p] << "]" << std::endl;
						}
					}
				}
			}
			int spectatedClient = demoReaders[i].reader.GetCurrentPlayerState().clientNum;
			lastSpectatedClientNums[i] = spectatedClient;
		}
	}
	pingDemoReaders.clear();
	
	
	demoCutConfigstringModifiedManual(&demo.cut.Cl, getCS_LEVEL_START_TIME(demoType), "10000",demoType);

	// Add "fake demo" server name.
	char infoCopy[MAX_INFO_STRING_MAX];
	infoCopy[0] = 0;
	strcpy_s(infoCopy, isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING, demo.cut.Cl.gameState.stringData+demo.cut.Cl.gameState.stringOffsets[0]);
	Info_SetValueForKey_Big(infoCopy,sizeof(infoCopy), "sv_hostname", "^1^7^1FAKE ^4^7^4DEMO");
	demoCutConfigstringModifiedManual(&demo.cut.Cl, 0, infoCopy, demoType);

	demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_MOTD, "^7This demo was artificially created using JK2DemoCutter tools.", demoType);

	// TODO In general: Generate scoreboard commands with the scores from the playerstates?
	// Note: We will simply use a null state as entity baselines. Not memory efficient but it should do for starters. Don't hav to do anything for that, since we already nulled the whole demo_t struct

	// Create unique output file.
	int dupeIterator = 2;
	Com_sprintf(newName, sizeof(newName),  "%s%s", outputNameNoExt, ext);
	while (FS_FileExists(newName)) {
		Com_sprintf(newName, sizeof(newName), "%s(%d)%s", outputNameNoExt, dupeIterator, ext);
		dupeIterator++;
	}

	newHandle = FS_FOpenFileWrite(newName,createCompressedOutput ? FILECOMPRESSION_LZMA : FILECOMPRESSION_NONE, qfalse);
	if (!newHandle) {
		Com_DPrintf("Failed to open %s for target cutting.\n", newName);
		return qfalse;
	}

	// Write demo header
	demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl, demoType,createCompressedOutput);
	demo.cut.Clc.reliableSequence++;
	demo.cut.Clc.serverMessageSequence++;

	double sourceTime = 0.0f;
	double time = 10000.0f; // You don't want to start at time 0. It causes incomprehensible weirdness. In fact, it crashes most clients if you try to play back the demo.
	double fps = 60.0f;
	std::map<int, entityState_t> targetEntities;
	std::map<int, entityState_t> targetEntitiesOld;
	std::vector<std::string> commandsToAdd;
	std::vector<Event> eventsToAdd;
	playerState_t tmpPS, mainPlayerPS, mainPlayerPSOld;
	entityState_t tmpES;
	int currentCommand = 1;
	// Start writing snapshots.
	qboolean isFirstSnapshot = qtrue;
	std::stringstream ss;


	//std::map<int, float> blockedEntities; // Entities that are blocked because they had an event in the last EVENT_VALID_MSEC timeframe. The value describes the time they started to be blocked.
	float blockedEntities[MAX_GENTITIES]; // Entities that are blocked because they had an event in the last EVENT_VALID_MSEC timeframe. The value describes the time they started to be blocked.
	Com_Memset(blockedEntities, 0,sizeof(blockedEntities));

	while(1){
		commandsToAdd.clear();
		eventsToAdd.clear();
		qboolean allSourceDemosFinished = qtrue;
		targetEntities.clear();

		int thisFrameIndex = 0;

		//copiedPlayerIndex = 0;
		for (int i = 0; i < demoReaders.size(); i++) {
			if (sourceTime>= (demoReaders[i].sourceInfo->delay+ demoReaders[i].sourceInfo->showStart) && // Don't start showing stuff from this demo until it starts (controlled by delay & showStart)
				sourceTime<= (demoReaders[i].sourceInfo->delay+ demoReaders[i].sourceInfo->showEnd) && // Don't show stuff from this demo if we're past showEnd
				demoReaders[i].reader.SeekToTime(sourceTime-demoReaders[i].sourceInfo->delay)) { // Make sure we actually have a snapshot parsed, otherwise we can't get the info about the currently spectated player.


				demoType_t sourceDemoType = demoReaders[i].reader.getDemoType();
				int maxClientsThisDemo = demoReaders[i].reader.getMaxClients();
				for (int c = 0; c < demoReaders[i].playersToCopy.size(); c++) {

					int clientNumHere = demoReaders[i].playersToCopy[c].clientNum;
					//int medianPlayerPingHere = demoReaders[i].playersToCopy[c].medianPing;
					double pingCompensationHere = demoReaders[i].playersToCopy[c].pingCompensation;
					qboolean asG2AnimEnt = demoReaders[i].playersToCopy[c].asG2AnimEnt;

					int targetClientNum = -1;
					if (asG2AnimEnt) {
						targetClientNum = slotManager.getEntitySlot(i, clientNumHere);
					}
					else {

						qboolean isNewPlayer = qfalse;
						targetClientNum = slotManager.getPlayerSlot(i, clientNumHere, &isNewPlayer);
						if (isNewPlayer) {

							tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(clientNumHere, &tmpConfigStringMaxLength);

							if (strlen(tmpConfigString)) { // Would be pretty weird if this wasn't the case tho tbh.
								//demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + (copiedPlayerIndex++), tmpConfigString);
								commandsToAdd.push_back(makeConfigStringCommand(CS_PLAYERS + targetClientNum, tmpConfigString));
								demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + targetClientNum, tmpConfigString, demoType);
							}
						}
					}

					bool thisClientIsTargetPlayerState = !asG2AnimEnt && sourceTime >= (demoReaders[i].sourceInfo->delay + demoReaders[i].sourceInfo->playerStateStart) && // Don't use this demo for playerstate unless we're past playerStateStart
						sourceTime <= (demoReaders[i].sourceInfo->delay + demoReaders[i].sourceInfo->playerStateEnd) && // Don't use this demo for playerstate if we're past playerStateEnd
						thisFrameIndex++ == 0;
					
					//std::map<int, entityState_t> hereEntities = demoReaders[i].reader.GetCurrentEntities();
					//tmpPS = demoReaders[i].GetCurrentPlayerState();
					//tmpPS = demoReaders[i].reader.GetInterpolatedPlayerState(sourceTime+demoReaders[i].sourceInfo->delay);
					SnapshotInfo* oldSnap = NULL;
					SnapshotInfo* newSnap = NULL;
					//tmpPS = demoReaders[i].reader.GetInterpolatedPlayer(clientNumHere, sourceTime - demoReaders[i].sourceInfo->delay,&oldSnap,&newSnap,(qboolean)(targetClientNum == 0));
					float translatedTime = 0.0f;
					tmpPS = demoReaders[i].reader.GetInterpolatedPlayer(clientNumHere, sourceTime - demoReaders[i].sourceInfo->delay- pingCompensationHere,&oldSnap,&newSnap,(qboolean)thisClientIsTargetPlayerState,&translatedTime);
					//int originalPlayerstateClientNum = tmpPS.clientNum;
					
					demoReaders[i].reader.convertPSTo(&tmpPS,demoType);


					// Check if oldSnap contains ET_BODY of this client and if so, copy it.
					// We have to do this here because we are using commandtime as the basis for timing, but normal 
					// serverTime for entities and such.
					// Hence, there would be a desync normally.
					// However TODO: This might still be a problem with many clients/bodies? Actually maybe we should just handle this stuff manually or sth?
					// Idk.
					if (oldSnap && !asG2AnimEnt) {

						// Translate translated server time into actual server time (instead of commandtime)
						float realTranslatedTime = translatedTime - oldSnap->playerCommandOrServerTimes[clientNumHere] + oldSnap->serverTime;

						for (auto it = oldSnap->entities.begin(); it != oldSnap->entities.end(); it++) {
							// Players are already handled otherwhere
							//if (it->first >= MAX_CLIENTS) {
							if (it->first >= maxClientsThisDemo) {
								// Is this a corpse?
								if (generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(it->second.eType,sourceDemoType) == ET_BODY_GENERAL && it->second.clientNum == clientNumHere) {
									// Check if we are tracking this player
									if (targetClientNum != -1) {
										int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
										if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
											entityState_t tmpEntity = it->second;
											tmpEntity.eType = ET_BODY_JK2;
											tmpEntity.clientNum = targetClientNum;
											tmpEntity.number = targetEntitySlot;
											retimeEntity(&tmpEntity, realTranslatedTime, time);
											//tmpEntity.pos.trTime = time;
											if (EV_BODY_QUEUE_COPY_GENERAL == (generalizeGameValue<GMAP_EVENTS, UNSAFE>(tmpEntity.event,sourceDemoType) & ~EV_EVENT_BITS)) {
												tmpEntity.eventParm = convertGameValue<GMAP_EVENTS, UNSAFE>(tmpEntity.event & ~EV_EVENT_BITS,sourceDemoType, demoType);
												tmpEntity.eventParm = targetClientNum;
											}
											targetEntities[targetEntitySlot] = tmpEntity;
										}
									}
								}
							}
						}
					}


					//tmpPS.clientNum = copiedPlayerIndex;
					tmpPS.clientNum = targetClientNum;
					tmpPS.commandTime = time;

					// Process any changes of the spectated player in the original demo by just updating our configstring and setting teleport bit.
					/*if (lastSpectatedClientNums[i] != originalPlayerstateClientNum) {
						tmpConfigString = demoReaders[i].reader.GetPlayerConfigString(originalPlayerstateClientNum);
						if (strlen(tmpConfigString)) {
							demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + i, tmpConfigString);
						}
						ss.str(std::string());
						ss << "cs " << (CS_PLAYERS + i) << " " << tmpConfigString;
						commandsToAdd.push_back(ss.str());
						tmpPS.eFlags |= getEF_TELEPORTBIT(demoType);
					}
					lastSpectatedClientNums[i]= originalPlayerstateClientNum;*/

					// self explanatory.
					//if (copiedPlayerIndex == 0) {
					//if (targetClientNum == 0) {
					if (thisClientIsTargetPlayerState) {
						mainPlayerPS = tmpPS;
					}
					else {
						Com_Memset(&tmpES, 0, sizeof(tmpES));
						BG_PlayerStateToEntityState(&tmpPS, &tmpES, qfalse, demoType, qtrue); // DM_15 because BG_PlayerStateToEntityState only needs it for output eType and that's DM_15 for us. If that changes in the future.. we'll see. I'll probably accidentally forget about it and get bugs, gg.
						if (asG2AnimEnt) {
							// Can't be ET_PLAYER. Has to be ET_GRAPPLE (G2 anim ent)
							tmpES.eType = ET_GRAPPLE_JK2;
							// We don't have a player model. So instead get a ModelIndex for this playermodel
							{	// TODO It's kinda wasteful to do this on every frame. Maybe figure out way to do it only when the model changes.
								int maxLength;
								//const char* playerInfo = demoReaders[i].reader.GetConfigString(CS_PLAYERS + clientNumHere, &maxLength);
								const char* playerInfo = demoReaders[i].reader.GetPlayerConfigString(clientNumHere, &maxLength);
								std::string thisModel = Info_ValueForKey(playerInfo, maxLength, "model");

								std::transform(thisModel.begin(), thisModel.end(), thisModel.begin(), tolowerSignSafe);
								size_t firstSlash = thisModel.find_first_of("/");

								bool variantExists = false;
								std::string baseModel = "";
								std::string variant = "";
								if (firstSlash == std::string::npos) {
									// No variant. Just the model name
									baseModel = thisModel;
									//baseModelCString = thisModel->c_str();
								}
								else {
									baseModel = thisModel.substr(0, firstSlash);
									//baseModelCString = thisModel->c_str();
									if (firstSlash < (thisModel.size() - 1)) {
										variantExists = true;
										variant = thisModel.substr(firstSlash + 1, thisModel.size() - firstSlash - 1);
										//variantCString = variant.c_str();
									}
								}

								tmpES.modelindex = G_ModelIndex(va("models/players/%s/model.glm", baseModel.c_str()), &demo.cut.Cl, &commandsToAdd, demoType);
							}
						}
						//playerEntities[copiedPlayerIndex] = tmpES;
						targetEntities[targetClientNum] = tmpES;
					}
					//copiedPlayerIndex++;
				}

				// Get various related entities
				int maxLengthTmp;
				double thisTimeInServerTime;
				std::map<int, entityState_t> sourceEntitiesAtTime = demoReaders[i].reader.GetEntitiesAtTime(sourceTime - demoReaders[i].sourceInfo->delay,&thisTimeInServerTime);
				
				for (auto it = sourceEntitiesAtTime.begin(); it != sourceEntitiesAtTime.end(); it++) {
					
					int generalizedEntityType = generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(it->second.eType,sourceDemoType);




					// EFFECT_EXPLOSION_TRIPMINE (EV_PLAY_EFFECT)
					// EV_GENERAL_SOUND te->s.trickedentindex = ent->s.number;
					if (generalizedEntityType == ET_GENERAL_GENERAL && generalizeGameValue<GMAP_WEAPONS,SAFE>(it->second.weapon,sourceDemoType) == WP_TRIP_MINE_GENERAL && (it->second.eFlags & getEF_MISSILE_STICK(sourceDemoType))) { // TODO Make this work with JKA?
						// It's a trip mine.

						// Ignore this: (might be based on mistake)
						// TODO actually this detection is not reliable because the game reuses entities without clearing them.
						// Hmmm....
						// e->inuse = qtrue;
						// e->classname = "noclass";
						// e->s.number = e - g_entities;
						// e->r.ownerNum = ENTITYNUM_NONE;
						// e->s.modelGhoul2 = 0; //assume not
						// This is all it does

						int ownerSlot = slotManager.getSlotIfExists(i, it->second.genericenemyindex - 1024); // TODO Should we copy this for g2animents?
						if (it->second.pos.trType == TR_GRAVITY) {
							// It's flying/in the air.
							// Only for entities we are tracking, otherwise
							// mines will come flying out of nowhere
							
						}
						else if (it->second.pos.trType == TR_STATIONARY) {
							// It's sticking. Let it exist then. Might be used for boosts or sth
							if (ownerSlot == -1) {
								ownerSlot = 0; // Just give it to person 0. Is wrong I guess but won't matter unless it's a kill.
								// Will be the responsibility of the tool user to include the owner of the mine otherwise.
								// Actually it won't matter anyway because EV_OBITUARY is not dependent on that, if anything at all is.
							}
						}

						if (ownerSlot != -1) {
							int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
							if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
								entityState_t tmpEntity = it->second;
								tmpEntity.eFlags &= ~getEF_MISSILE_STICK(sourceDemoType);
								tmpEntity.eFlags |= EF_MISSILE_STICK;
								tmpEntity.eType = ET_GENERAL_JK2;
								tmpEntity.weapon = WP_TRIP_MINE_JK2;
								tmpEntity.genericenemyindex = ownerSlot >= MAX_CLIENTS? 0: ownerSlot + 1024;
								tmpEntity.number = targetEntitySlot;
								remapConfigStrings(&tmpEntity,&demo.cut.Cl,&demoReaders[i].reader,&commandsToAdd,qtrue,qfalse,demoType);
								retimeEntity(&tmpEntity, thisTimeInServerTime,time);
								/*if (EV_GENERAL_SOUND == (tmpEntity.event & ~EV_EVENT_BITS)) {
									//tmpEntity.eventParm = targetPlayerSlot;
								}*/
								targetEntities[targetEntitySlot] = tmpEntity;
							}
						}

					}
					
					// Sentry
					//else if (it->second.eType == ET_GENERAL && it->second.modelindex && !strcmp(demoReaders[i].reader.GetConfigString(CS_MODELS+ it->second.modelindex,&maxLengthTmp), "models/items/psgun.glm")) {
					else if (generalizedEntityType == ET_GENERAL_GENERAL && it->second.modelindex && !strcmp(demoReaders[i].reader.GetModelConfigString(it->second.modelindex,&maxLengthTmp), "models/items/psgun.glm")) {
						// It's a sentry


						int ownerSlot = slotManager.getNormalPlayerSlotIfExists(i, it->second.owner);
						if (ownerSlot == -1) {
							ownerSlot = 0; 
							// It's a sentry. We want it either way. For now.
						}

						if (ownerSlot != -1) {
							int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
							if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
								entityState_t tmpEntity = it->second;
								tmpEntity.eType = ET_GENERAL_JK2;
								tmpEntity.owner = ownerSlot;
								tmpEntity.number = targetEntitySlot;
								remapConfigStrings(&tmpEntity,&demo.cut.Cl,&demoReaders[i].reader,&commandsToAdd,qtrue,qfalse, demoType);
								retimeEntity(&tmpEntity, thisTimeInServerTime,time);
								/*if (EV_GENERAL_SOUND == (tmpEntity.event & ~EV_EVENT_BITS)) {
									//tmpEntity.eventParm = targetPlayerSlot;
								}*/
								targetEntities[targetEntitySlot] = tmpEntity;
							}
						}

					}
					// Flag
					//else if (it->second.eType == ET_ITEM && it->second.modelindex && bg_itemlist[it->second.modelindex].giType == IT_TEAM && bg_itemlist[it->second.modelindex].giTag >= PW_REDFLAG && bg_itemlist[it->second.modelindex].giTag <= PW_NEUTRALFLAG ) {
					else if (generalizedEntityType == ET_ITEM_GENERAL && it->second.modelindex && generalizeGameValue<GMAP_ITEMLIST,SAFE>(it->second.modelindex,demoReaders[i].reader.getDemoType()) >= ITEMLIST_TEAM_CTF_REDFLAG_GENERAL&& generalizeGameValue<GMAP_ITEMLIST,SAFE>(it->second.modelindex, demoReaders[i].reader.getDemoType()) <= ITEMLIST_TEAM_CTF_NEUTRALFLAG_GENERAL) {
						
						int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
						if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
							entityState_t tmpEntity = it->second;
							tmpEntity.eType = ET_ITEM_JK2;
							tmpEntity.modelindex = convertGameValue<GMAP_ITEMLIST,UNSAFE>(it->second.modelindex, sourceDemoType, demoType);
							tmpEntity.number = targetEntitySlot;
							//remapConfigStrings(&tmpEntity,&demo.cut.Cl,&demoReaders[i].reader,&commandsToAdd,qfalse,qfalse);
							retimeEntity(&tmpEntity, thisTimeInServerTime,time);
							targetEntities[targetEntitySlot] = tmpEntity;
						}

					}
					// Force shield
					else if (generalizedEntityType == ET_SPECIAL_GENERAL && it->second.modelindex == HI_SHIELD) { // TODO: Make work with JKA?
						
						int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
						if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
							entityState_t tmpEntity = it->second;
							tmpEntity.eType = ET_SPECIAL_JK2;
							tmpEntity.number = targetEntitySlot;
							int mappedOwner = slotManager.getNormalPlayerSlotIfExists(i, it->second.owner);
							tmpEntity.owner = mappedOwner == -1 ? 0 : mappedOwner; // We want force shields always no matter who made them. For now.
							remapConfigStrings(&tmpEntity,&demo.cut.Cl,&demoReaders[i].reader,&commandsToAdd,qfalse,qfalse, demoType);
							retimeEntity(&tmpEntity, thisTimeInServerTime,time);
							targetEntities[targetEntitySlot] = tmpEntity;
						}

					}
					// Shots, rockets etc
					// We can't really tell who they belong to. Just copy them.
					else if (generalizedEntityType == ET_MISSILE_GENERAL) {
						
						int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
						if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
							entityState_t tmpEntity = it->second;
							tmpEntity.eType = ET_MISSILE_JK2;
							tmpEntity.weapon = convertGameValue<GMAP_WEAPONS, SAFE>(tmpEntity.weapon, sourceDemoType, demoType);
							tmpEntity.number = targetEntitySlot;
							remapConfigStrings(&tmpEntity,&demo.cut.Cl,&demoReaders[i].reader,&commandsToAdd,qfalse,qfalse, demoType);
							retimeEntity(&tmpEntity, thisTimeInServerTime,time);
							targetEntities[targetEntitySlot] = tmpEntity;
						}

					}
					// Single player NPCs
					else if (generalizedEntityType == ET_PLAYER_GENERAL && sourceDemoType == DM_14) {
						
						int targetEntitySlot = slotManager.getEntitySlot(i, it->first);
						if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
							entityState_t tmpEntity = it->second;
							tmpEntity.eType = ET_GRAPPLE_JK2;
							remapConfigStrings(&tmpEntity, &demo.cut.Cl, &demoReaders[i].reader, &commandsToAdd, qtrue, qfalse, demoType);
							tmpEntity.modelindex = tmpEntity.modelindex ? tmpEntity.modelindex : G_ModelIndex(va("models/players/stormtrooper/model.glm"), &demo.cut.Cl, &commandsToAdd, demoType); // By unmodded default these don't contain a modelIndex as that is sent directly between server and client in SP.
							tmpEntity.number = targetEntitySlot;
							tmpEntity.legsAnim = convertGameValue<GMAP_ANIMATIONS,UNSAFE>(tmpEntity.legsAnim,sourceDemoType, demoType);
							tmpEntity.torsoAnim = convertGameValue<GMAP_ANIMATIONS,UNSAFE>(tmpEntity.torsoAnim,sourceDemoType, demoType);
							tmpEntity.weapon = convertGameValue<GMAP_WEAPONS,SAFE>(tmpEntity.weapon,sourceDemoType, demoType);
							tmpEntity.event = convertGameValue<GMAP_EVENTS, UNSAFE>(tmpEntity.event,sourceDemoType, demoType);
							tmpEntity.saberMove = convertGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(tmpEntity.boltInfo,sourceDemoType, demoType);
							tmpEntity.boltInfo = -1;
							retimeEntity(&tmpEntity, thisTimeInServerTime,time);
							targetEntities[targetEntitySlot] = tmpEntity;
						}

					}

					
					// Players are already handled otherwhere
					if (it->first >= maxClientsThisDemo) {
						/* Now handling this up there
						// Is this a corpse?
						if (it->second.eType == ET_BODY) {
							// Check if we are tracking this player
							int targetPlayerSlot = slotManager.getSlotIfExists(i,it->second.clientNum);
							if (targetPlayerSlot != -1) {
								int targetEntitySlot = slotManager.getEntitySlot(i,it->first);
								if (targetEntitySlot != -1) { // (otherwise we've ran out of slots)
									entityState_t tmpEntity = it->second;
									tmpEntity.clientNum = targetPlayerSlot;
									tmpEntity.number = targetEntitySlot;
									tmpEntity.pos.trTime = time;
									if (EV_BODY_QUEUE_COPY == (tmpEntity.event & ~EV_EVENT_BITS)) {
										tmpEntity.eventParm = targetPlayerSlot;
									}
									targetEntities[targetEntitySlot] = tmpEntity;
								}
							}
						}*/
					}
				}

				// Get new commands
				std::vector<std::string> newCommandsHere = demoReaders[i].reader.GetNewCommands(sourceTime - demoReaders[i].sourceInfo->delay);
				for (int c = 0; c < newCommandsHere.size(); c++) {

					Cmd_TokenizeString(newCommandsHere[c].c_str());
					char* cmd = Cmd_Argv(0);
					if (!strcmp(cmd, "print") || !strcmp(cmd, "chat")/* || !strcmp(cmd, "cp")*/) {
						if (!strstr(newCommandsHere[c].c_str(), "!respos") && !strstr(newCommandsHere[c].c_str(), "!savepos")) { // TODO Make this case insensitive for absolute protection!
							commandsToAdd.push_back(newCommandsHere[c]);
						}
					}
					if (!strcmp(cmd, "kg2")/* || !strcmp(cmd, "cp")*/) {
						char commandString[10]; // highest possible num is 4 digits. kg2 is 3 digits. space is 1 digit. NUL takes 1 digit. Let's make it a round 10 for comfort.
						commandString[0] = 0;
						int num = atoi(Cmd_Argv(1));
						int targetNum = slotManager.getSlotIfExists(i, num);
						if (targetNum != -1) {
							Com_sprintf(commandString, sizeof(commandString), "kg2 %d", targetNum);
							commandsToAdd.push_back(commandString);
						}
					}
				}

				// Get new events and remember them.
				if (demoReaders[i].sourceInfo->delay < 0 && demoReaders[i].targetFramesRead == 0) {
					// For demos with a negative delay (part of start cut off), discard events that happened longer than 2 frames ago.
					demoReaders[i].reader.GetNewEvents(sourceTime - demoReaders[i].sourceInfo->delay - 2.0 * 1000.0 / fps);
					// Just ignore the return value.
				}
				std::vector<Event> newEventsHere = demoReaders[i].reader.GetNewEvents(sourceTime - demoReaders[i].sourceInfo->delay);
				std::map<int, entityState_t> entitiesHere;
				qboolean entitiesAlreadyRead = qfalse; // Slight optimization really, nthing more.
				for (int c = 0; c < newEventsHere.size(); c++) {
					Event* thisEvent = &newEventsHere[c];
					int eventNumber = thisEvent->eventNumber; // This is currently all specialized for jk2 output! Maybe generalize some day... 
					qboolean addThisEvent = qfalse;
					if (eventNumber == EV_PLAYER_TELEPORT_IN_JK2 || eventNumber == EV_PLAYER_TELEPORT_OUT_JK2) {
						//if (thisEvent->theEvent.clientNum == originalPlayerstateClientNum) {
						//if (std::find(demoReaders[i].playersToCopy.begin(), demoReaders[i].playersToCopy.end(), thisEvent->theEvent.clientNum) != demoReaders[i].playersToCopy.end()) {
						//	thisEvent->theEvent.clientNum = i;
						//	addThisEvent = qtrue;
						//}
						int mappedClientNum = slotManager.getSlotIfExists(i, thisEvent->theEvent.clientNum);
						if (mappedClientNum != -1) {
							thisEvent->theEvent.clientNum = mappedClientNum; // We wanna show it for even g2animents. The clientnum isnt really used for anything anyway so we just preserve it and give it the g2animent number
							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_OBITUARY_JK2) {

						//int				target = ent->otherEntityNum;
						//int				attacker = ent->otherEntityNum2;

						// Check if we are tracking these players.
						int target = slotManager.getNormalPlayerSlotIfExists(i, thisEvent->theEvent.otherEntityNum);
						int attacker = slotManager.getNormalPlayerSlotIfExists(i, thisEvent->theEvent.otherEntityNum2); // This will not track kills of g2 anim ents. Intended?
						if (target != -1 && attacker != -1) {
							thisEvent->theEvent.otherEntityNum = target;
							thisEvent->theEvent.otherEntityNum2 = attacker;

							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_CTFMESSAGE_JK2) {

						//int				target = ent->otherEntityNum;
						//int				attacker = ent->otherEntityNum2;

						// Check if we are tracking these players.
						int mappedPlayer = slotManager.getNormalPlayerSlotIfExists(i, thisEvent->theEvent.trickedentindex);
						if (mappedPlayer) {
							thisEvent->theEvent.trickedentindex = mappedPlayer;

							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_PLAY_EFFECT_JK2) {

						addThisEvent = qtrue;
					}
					else if (eventNumber == EV_GENERAL_SOUND_JK2 && thisEvent->theEvent.eType > ET_EVENTS_JK2) { // Only copy event entities. Not players with the event or sth.

						// Are we tracking the entity that generated this sound? Actually: This doesnt work bc it only remembers that information if it was a player.
						if (thisEvent->theEvent.eFlags == EF_SOUNDTRACKER) {
							// This was a player, so we know which player
							// So only copy this if we are tracking this player.
							int soundSourceEntity = slotManager.getSlotIfExists(i, thisEvent->theEvent.trickedentindex); // TODO Siwtch to getNormalPlayerSlotIfExists?
							if (soundSourceEntity != -1) {
								thisEvent->theEvent.trickedentindex = soundSourceEntity;
								remapConfigStrings(&thisEvent->theEvent, &demo.cut.Cl, &demoReaders[i].reader, &commandsToAdd, qfalse, qfalse, demoType);
								addThisEvent = qtrue;
							}
						}
						else {
							// Not a player. could be a sentry. Who knows.
							// We cannot know if we need it, so let's just take it.
							remapConfigStrings(&thisEvent->theEvent, &demo.cut.Cl, &demoReaders[i].reader, &commandsToAdd, qfalse, qfalse, demoType);
							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_SHIELD_HIT_JK2) {

						// Check if we are tracking this player.
						int target = slotManager.getSlotIfExists(i, thisEvent->theEvent.otherEntityNum);
						if (target != -1) {
							// This will not work on g2 anim ents BUT it doesnt throw an error. And we can't just put it on another entity.
							// But some demo player (jomme?) could possibly unlock this for g2 anim ents, so we preserve it.
							thisEvent->theEvent.otherEntityNum = target; 

							addThisEvent = qtrue;
						}
					}
					else if (eventNumber == EV_SABER_HIT_JK2 || eventNumber == EV_SABER_BLOCK_JK2) { // How to get rid of hits with no corresponding player copied? Check distance?

						if (!entitiesAlreadyRead) entitiesHere = demoReaders[i].reader.GetEntitiesAtPreciseTime(thisEvent->demoTime,qtrue);

						// I think default max hitbox is 30x30x64? So middle to farthest point should be vector 15,15,30 length. but i think middle isnt centered?
						// Is it -24 and then 40 on the top? So let's say max diagonal is distance of 15,15,40. aka sqrt(15*15+15*15+40*40) = 45.27692569068708313286904083492
						// Let's double it and make it a round 100. Add max saber length *2 for good measure.
						constexpr float maxDistance = 100.0f + 2.0f * (float)SABER_LENGTH_MAX;

						/*if (eventNumber == EV_SABER_HIT && thisEvent->theEvent.eventParm == 0) {
							// Not a player hit
							// This could be a player hitting the floor or sth I think?
							// Let's allow it and widen the allowed distance by saber length *2.
							maxDistance += 2.0f*(float)SABER_LENGTH_MAX;
						}
						if (eventNumber == EV_SABER_HIT && thisEvent->theEvent.eventParm >1) {
							// I think this is dismemberment stuff
							maxDistance += 2.0f*(float)SABER_LENGTH_MAX;
						}
						if (eventNumber == EV_SABER_BLOCK) {
							// This is a saber block.
							maxDistance += 2.0f * (float)SABER_LENGTH_MAX;
						}*/

						// Check if there's any tracked player entity nearby, aka whether it makes sense to copy this over.
						for (auto entitIt = entitiesHere.begin(); entitIt != entitiesHere.end(); entitIt++) {
							if (entitIt->first < 0 || entitIt->first >= maxClientsThisDemo) break;
							int target = slotManager.getSlotIfExists(i, entitIt->second.number);
							vec3_t distanceVec;
							VectorSubtract(entitIt->second.pos.trBase, thisEvent->theEvent.pos.trBase, distanceVec);
							if (VectorLength(distanceVec) <= maxDistance) {
								addThisEvent = qtrue;
							}
						}

					}
					/*if (eventNumber == EV_BODY_QUEUE_COPY) { // This is actually part of the ET_BODY I'm copying over anyway?

						// Check if we are tracking this player.
						int target = slotManager.getSlotIfExists(i, thisEvent->theEvent.eventParm);
						if (target != -1) {
							thisEvent->theEvent.eventParm = target;

							addThisEvent = qtrue;
						}
					}*/
					if (addThisEvent) eventsToAdd.push_back(*thisEvent);
				}

				demoReaders[i].targetFramesRead++;
			}
			/*else {
				// We reached the end of this demo.
				slotManager.freeSlots(i);
			}
			if (!demoReaders[i].reader.EndReachedAtTime(sourceTime - demoReaders[i].sourceInfo->delay)) {
				allSourceDemosFinished = qfalse;
			}*/
			qboolean endReached, showEndReached;
			if ((endReached= demoReaders[i].reader.EndReachedAtTime(sourceTime - demoReaders[i].sourceInfo->delay))
					|| (showEndReached = (qboolean)(sourceTime > (demoReaders[i].sourceInfo->delay + demoReaders[i].sourceInfo->showEnd)))) {
				if (!demoReaders[i].sourceInfo->isFinished) {
					std::cout << "Demo " << i << " has ended at " << sourceTime << " (" << demoReaders[i].reader.getCurrentDemoTime() << "). Demo had a delay of " << demoReaders[i].sourceInfo->delay << ". endReached: " << endReached << ", showEndReached: " << showEndReached << std::endl;
					// We reached the end of this demo.
					std::vector<int> erasedSlots = slotManager.freeSlots(i);
					for (int sl = 0; sl < erasedSlots.size(); sl++) {
						int erasedSlot = erasedSlots[sl];
						if (erasedSlot >= 0 && erasedSlot < MAX_CLIENTS) {
							commandsToAdd.push_back(makeConfigStringCommand(CS_PLAYERS + erasedSlot, ""));
							demoCutConfigstringModifiedManual(&demo.cut.Cl, CS_PLAYERS + erasedSlot, "", demoType);
							commandsToAdd.push_back(va("kg2 %d", erasedSlot)); // Send kg2 for every erased slot.
						}
					}
					demoReaders[i].sourceInfo->isFinished = qtrue;
				}
			}
			else {
				allSourceDemosFinished = qfalse;
			}
		}

		// Find empty places in entities and add events.
		for (int i = 0; i < eventsToAdd.size(); i++) {
			for (int e = MAX_CLIENTS; e < MAX_GENTITIES-2; e++) { // Can I use full MAX_GENTITIES amount? Isn't MAX_GENTITIES -1 reserved for world and stuff like that?
				if (targetEntities.find(e) == targetEntities.end() && (blockedEntities[e]+EVENT_VALID_MSEC+100.0f) < time && slotManager.isSlotFree(e)) { // Events won't get recognized if they happen on the same entity in a short timeframe. Give it EVENT_VALID_MSEC so the client resets it, and add 100 on top for good measure.
					// Let's add it!
					eventsToAdd[i].theEvent.number = e;
					targetEntities[e] = eventsToAdd[i].theEvent;
					blockedEntities[e] = time;
					break;
				}
			}
		}

		demo.cut.Cl.snap.serverTime = time;
		demo.cut.Cl.snap.ps = mainPlayerPS;

		clSnapshot_t mainPlayerSnapshot = demoReaders[0].reader.GetCurrentSnap();
		//Com_Memcpy(demo.cut.Cl.snap.areamask, mainPlayerSnapshot.areamask,sizeof(demo.cut.Cl.snap.areamask));// We might wanna do something smarter someday but for now this will do. 
		Com_Memset(demo.cut.Cl.snap.areamask,0,sizeof(demo.cut.Cl.snap.areamask));

		if (isFirstSnapshot) {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl, demoType, &targetEntities, NULL,NULL,createCompressedOutput);
			isFirstSnapshot = qfalse;
		}
		else {
			demoCutWriteDeltaSnapshotManual(&commandsToAdd, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl, demoType, &targetEntities, &targetEntitiesOld, &mainPlayerPSOld,createCompressedOutput);
		}

		time += 1000.0 / fps;
		sourceTime += 1000.0 / fps;
		demo.cut.Clc.reliableSequence++;
		demo.cut.Clc.serverMessageSequence++;

		mainPlayerPSOld = mainPlayerPS;
		targetEntitiesOld.clear();
		for (auto it = targetEntities.begin(); it != targetEntities.end(); it++) {
			targetEntitiesOld[it->first] = it->second;
		}

		if (allSourceDemosFinished) break;
	}

cutcomplete:
	if (newHandle) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}
cuterror:
	FS_FCloseFile(newHandle);
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
	if (argc <3) {
		std::cout << "need 2 arguments: outputname, scriptname";
		return 1;
	}
	initializeGameInfos();
	char* scriptName = NULL;
	char* outputName = NULL;

	outputName = argv[1];
	char* filteredOutputName = new char[strlen(outputName) + 1];
	sanitizeFilename(outputName, filteredOutputName,qtrue);
	strcpy(outputName, filteredOutputName);
	delete[] filteredOutputName;

	// Read the script
	scriptName = argv[2];
	//std::vector<std::string> inputFiles;
	mINI::INIFile file(scriptName);
	mINI::INIStructure ini;
	file.read(ini);

	std::vector<DemoSource> demoSources;

	for (auto const& it : ini)
	{
		DemoSource demoSource;
		auto const& section = it.first;
		auto const& collection = it.second;
		demoSource.sourceName = section;
		std::cout << "[" << section << "]" << std::endl;
		//std::cout << "test: " << collection.get("test")<< strlen(collection.get("test").c_str()) << std::endl;
		std::cout << "path: " << collection.get("path") << std::endl;

		demoSource.copyRemainingPlayersAsG2AnimEnts = (qboolean)atoi(collection.get("restAsAnimEnts").c_str());
		std::cout << "restAsAnimEnts: " << demoSource.copyRemainingPlayersAsG2AnimEnts << std::endl;
		
		demoSource.copyAllPlayers = (qboolean)atoi(collection.get("allPlayers").c_str());
		std::cout << "allPlayers: " << demoSource.copyAllPlayers << std::endl;

		demoSource.copySubmodels = (qboolean)atoi(collection.get("submodels").c_str());
		std::cout << "submodels: " << demoSource.copySubmodels << std::endl;

		demoSource.copyModels = (qboolean)atoi(collection.get("models").c_str());
		std::cout << "models: " << demoSource.copyModels << std::endl;

		float delay = atof(collection.get("delay").c_str());
		std::cout << "delay: " << delay <<" ms" << std::endl;
		demoSource.delay = delay;

		std::string showStart = collection.get("start");
		if (showStart.size() > 0) {
			float start = atof(showStart.c_str());
			std::cout << "showStart: " << start << " ms" << std::endl;
			demoSource.showStart = start;
		}

		std::string showEnd = collection.get("end");
		if (showEnd.size() > 0) {
			float end = atof(showEnd.c_str());
			std::cout << "showEnd: " << end << " ms" << std::endl;
			demoSource.showEnd = end;
		}

		std::string playerStateStart = collection.get("playerStateStart");
		if (playerStateStart.size() > 0) {
			float start = atof(playerStateStart.c_str());
			std::cout << "playerStateStart: " << start << " ms" << std::endl;
			demoSource.playerStateStart = start;
		}

		std::string playerStateEnd = collection.get("playerStateEnd");
		if (playerStateEnd.size() > 0) {
			float end = atof(playerStateEnd.c_str());
			std::cout << "playerStateEnd: " << end << " ms" << std::endl;
			demoSource.playerStateEnd = end;
		}


		demoSource.demoPath = collection.get("path");

		std::vector<std::string> players = splitString(collection.get("players"),",");

		for (int i = 0; i < players.size(); i++) {
			std::cout << "player: " << players[i] << std::endl;
			if (players[i].size() > 0) {
				demoSource.playersToCopy.push_back(players[i]);
			}
		}

		if (demoSource.playersToCopy.size() > 0) {
			demoSources.push_back(demoSource);
		}
		else {
			std::cout << section << "was not added: No players specified." << std::endl;
		}
		std::cout << std::endl;
		/*for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << std::endl;
		}*/
	}



	/*
	for (int i = 2; i < argc; i++) {
		inputFiles.emplace_back(argv[i]);
	}
	*/
	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	if (demoCut(outputName,&demoSources)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Demo %s got successfully combined in %.5f seconds \n", scriptName, seconds);
	}
	else {
		Com_DPrintf("Demo %s has failed to get combined or combined with errors\n", scriptName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif