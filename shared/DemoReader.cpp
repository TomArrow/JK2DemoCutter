#include "demoCut.h"
#include "anims.h"
#include "jkaStuff.h"
#include "otherGameStuff.h"
#include "otherGameStuff2.h"
#include "DemoReader.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>



// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demoType_t DemoReader::getDemoType() {
	return demoType;
}
qboolean DemoReader::containsDimensionData() {
	return extraFieldInfo.dimensionInfoConfirmed;
}
int64_t DemoReader::getCutStartOffset(qboolean withTruncationOffset) {
	return extraFieldInfo.cutStartOffset + (withTruncationOffset ? extraFieldInfo.truncationOffset : 0);
}
bool DemoReader::isThisMOHAADemo() {
	return demoTypeIsMOHAA(demoType);
}
int DemoReader::getMaxClients() {
	return maxClientsThisDemo;
}

int DemoReader::getClientNumForDemo(std::string* playerSearchString, qboolean printEndLine) {
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	std::string* thisPlayer = playerSearchString;
	DemoReader* reader = this;
	const char* tmpConfigString;
	int tmpConfigStringMaxLength;
	int clientNumHere = -1;
	int pstrlen = thisPlayer->size();
	if (!pstrlen) return clientNumHere;
	if (pstrlen <= 2 && isdigit((*thisPlayer)[0]) && (pstrlen == 1 || isdigit((*thisPlayer)[1]))) {
		// It's a number. ClientNum. Just parse it.
		clientNumHere = atoi(thisPlayer->c_str());
		tmpConfigString = reader->GetPlayerConfigString(clientNumHere, &tmpConfigStringMaxLength);
		std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength, isMOHAADemo?"name":"n");
		std::cout << *thisPlayer << " (interpreted as clientNum) matches '" << nameHere << "' (" << clientNumHere << ")";
	}
	else {
		std::string thisPlayerLower = *thisPlayer;
		std::transform(thisPlayerLower.begin(), thisPlayerLower.end(), thisPlayerLower.begin(), tolowerSignSafe);

		std::vector<NameMatch> colorStrippedMatches;
		std::vector<NameMatch> caseInsensitiveMatches;
		std::vector<NameMatch> matches;

		// Find matching player name
		for (int c = 0; c < maxClientsThisDemo; c++) {
			tmpConfigString = reader->GetPlayerConfigString(c, &tmpConfigStringMaxLength);
			std::string nameHere = Info_ValueForKey(tmpConfigString, tmpConfigStringMaxLength,isMOHAADemo?"name": "n");
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
			std::cout << "'" << *thisPlayer << "' matches '" << matches[0].matchedName << "' (" << matches[0].clientNum << ")";
			clientNumHere = matches[0].clientNum;
		}
		else {
			// No match. Try case insensitive
			if (caseInsensitiveMatches.size() > 1) {
				std::cout << "Too many matches for player name '" << *thisPlayer << "': " << std::endl;
				for (int c = 0; c < caseInsensitiveMatches.size(); c++) {
					std::cout << caseInsensitiveMatches[c].matchedName << "(" << caseInsensitiveMatches[c].clientNum << ")" << std::endl;
				}
				std::cout << "Picking first match '" << caseInsensitiveMatches[0].matchedName << "' (" << caseInsensitiveMatches[0].clientNum << ")";
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
}




qboolean DemoReader::PlayerStateIsTeleport(clSnapshot_t* lastSnap, clSnapshot_t* snap) {
	// if the next frame is a teleport for the playerstate, we
	// can't interpolate during demos
	if (lastSnap && ((snap->ps.eFlags ^ lastSnap->ps.eFlags) & getEF_TELEPORTBIT(demoType))) {
		return qtrue;
	}

	// if changing follow mode, don't interpolate
	if (snap->ps.clientNum != lastSnap->ps.clientNum) {
		return qtrue;
	}

	// if changing server restarts, don't interpolate
	if ((snap->snapFlags ^ lastSnap->snapFlags) & SNAPFLAG_SERVERCOUNT) {
		return qtrue;
	}

	return qfalse;
}







#ifdef RELDEBUG
//#pragma optimize("", off)
#endif

int DemoReader::GetEvent(entityState_t* es) {
	//if (lastEvent.find(es->number) == lastEvent.end()) { // this might not be necessary because its auto-initialized to 0 anyway (std::map behavior)
	//	lastEvent[es->number] = 0;
	//}

	// check for event-only entities
	/*if (es->eType > ET_EVENTS) {
		if (cent->previousEvent) {
			return;	// already fired
		}
		// if this is a player event set the entity number of the client entity number
		if (es->eFlags & EF_PLAYER_EVENT) {
			es->number = es->otherEntityNum;
		}

		cent->previousEvent = 1;

		es->event = es->eType - ET_EVENTS;
	}
	else {
		// check for events riding with another entity
		if (es->event == cent->previousEvent) {
			return;
		}
		cent->previousEvent = es->event;
		if ((es->event & ~EV_EVENT_BITS) == 0) {
			return;
		}
	}*/

	if (lastEventTime[es->number] < demoCurrentTime - EVENT_VALID_MSEC) {
		lastEvent[es->number] = 0;
	}

	int eventNumberRaw = es->eType > getET_EVENTS(demoType) ? es->eType - getET_EVENTS(demoType) : es->event;
	int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

	lastEventTime[es->number] = demoCurrentTime;
	if (eventNumberRaw == lastEvent[es->number]) {
		return 0;
	}

	lastEvent[es->number] = eventNumberRaw;
	return eventNumber;

}

entityState_t* DemoReader::findEntity(int number) {
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

		if (thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}


qboolean DemoReader::LoadDemo(const char* sourceDemoFile) {
	char			oldName[MAX_OSPATH];
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	//char* ext;
	char ext[7];
	//char originalExt[7];
	oldHandle = 0; // TODO if already loaded, gracefully close
	
	originalDemoPath = sourceDemoFile;

	//strncpy_s(oldName, sizeof(oldName), sourceDemoFile, strlen(sourceDemoFile) - 6);

	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	demoCutGetDemoType(sourceDemoFile, ext, oldName ,&demoType,&isCompressedFile,&thisDemo.cut.Clc);


	isMOHAADemo = isThisMOHAADemo();

	maxClientsThisDemo = getMAX_CLIENTS(demoType);

	wasFirstCommandByte = qfalse;
	firstCommandByteRead = qfalse;

	/*ext = (char*)sourceDemoFile + strlen(sourceDemoFile) - 6;
	if (!*ext) {
		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		ext = ".dm_15"; 
		thisDemo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		ext = ".dm_16";
	}
	else if (!_stricmp(ext, ".dm_26")) {

		demoType = DM_26;
		ext = ".dm_26";
	}*/
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue,isCompressedFile);
	if (!oldHandle) {
		Com_DPrintf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	memset(basePlayerStates, 0, sizeof(basePlayerStates));
	memset(&oldPS, 0, sizeof(oldPS));
	memset(lastGottenEventsTime, 0, sizeof(lastGottenEventsTime));
	memset(lastGottenEventsServerTime, 0, sizeof(lastGottenEventsServerTime));

	// MOHAA
	if(isMOHAADemo){
		Com_Memset(mohaaPlayerWeapon, 0, sizeof(mohaaPlayerWeapon));
		Com_Memset(mohaaPlayerWeaponModelIndex, 0, sizeof(mohaaPlayerWeaponModelIndex));
		for (int i = 0; i < maxClientsThisDemo; i++) {
			mohaaPlayerWeaponModelIndex[i] = -1;
		}
		Com_Memset(mohaaPlayerWeaponModelIndexThisFrame, 0, sizeof(mohaaPlayerWeaponModelIndexThisFrame));
	}


	for (int i = 0; i < MAX_CLIENTS_MAX; i++) {
		firstPacketWithPlayer[i] = -1;
		lastKnownPacketWithPlayer[i] = -1;
		firstPacketWithPlayerState[i] = -1;
		lastKnownPacketWithPlayerState[i] = -1;
	}

	Com_Memset(&extraFieldInfo, 0, sizeof(extraFieldInfo));
	extraFieldInfo.dimensionInfoType = (dimensionDataType_t)(-1);
	extraFieldInfo.truncationOffset = demoCutGetDemoNameTruncationOffset(sourceDemoFile);

	readGamestate = 0;

	demoStartTime = 0;
	demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	demoCurrentTime = 0;
	lastGameStateChange = 0;
	lastGameStateChangeInDemoTime = 0;
	lastKnownTime = 0;
	lastKnownInOrderTime = 0; 
	maxSequenceNum = -9999;
	thisDemo.cut.Clc.lastKnownCommandTime = 0;
	thisDemo.cut.Clc.firstSnapServerTime = -1;
	lastKnownCommandOrServerTimes.clear();
	messageOffset = 0;
	lastGottenCommandsTime = 0;
	lastGottenCommandsServerTime = 0;
	endReached = qfalse;
	jsonSourceFileMetaDocument = NULL;

	snapshotInfos.clear();


	//while (oldSize > 0) ReadMessage();

	while (readGamestate == 0 && !endReached) {
		ReadMessage(); // Will only read pretty much the first message so we have gamestate and are somewhat "initialized" i guess
	}
	return qtrue;
}

qboolean DemoReader::EndReachedAtTime(double time) {
	SeekToTime(time);
	return (qboolean)(demoCurrentTime < time);
}

qboolean DemoReader::SeekToTime(double time) {
	while (demoCurrentTime < time && !endReached) {
		ReadMessage();
	}
	if (demoCurrentTime < time && endReached) return qfalse;
	return qtrue;
}



qboolean DemoReader::SeekToPlayerCommandOrServerTime(int clientNum, int serverTime) {
	while (lastKnownCommandOrServerTimes[clientNum] < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToPlayerInPacket(int clientNum) {
	while (lastKnownCommandOrServerTimes[clientNum] <= 0 && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] <= 0 && endReached) return qfalse;
	return qtrue;
}

int DemoReader::GetFirstServerTimeAfterServerTime(int serverTime) {
	while (lastKnownTime <= serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime <= serverTime && endReached) return -1;

	if (demoBaseTime) { // Continuity not guaranteed. Do slow linear search

		for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
			if (it->second.serverTime > serverTime) return it->second.serverTime;
		}
	}
	else {
		// Do fast binary search
		auto upper = std::upper_bound(snapshotInfos.begin(),snapshotInfos.end(),serverTime, snapshotInfosServerTimeGreaterPredicate); // Should be first one that has greater server time.
		if (upper != snapshotInfos.end() /* && upper->second.serverTime > serverTime*/) {
			return upper->second.serverTime;
		}
	}
	return -1; // Shouldn't happen really but whatever
}
SnapshotInfoMapIterator DemoReader::GetFirstSnapshotAfterSnapshotIterator(SnapshotInfoMapIterator oldSnapInfoIt) {
	int serverTime = oldSnapInfoIt->second.serverTime;
	while (lastKnownTime <= serverTime && !endReached) {
		ReadMessage();
	}
	return ++oldSnapInfoIt;
}
SnapshotInfoMapIterator DemoReader::GetFirstSnapshotAfterSnapshotIterator(SnapshotInfoMapIterator oldSnapInfoIt, int pastServerTime) {
	while (lastKnownTime <= pastServerTime && !endReached) {
		ReadMessage();
	}
	while (oldSnapInfoIt != snapshotInfos.end() && oldSnapInfoIt->second.serverTime <= pastServerTime) {

		oldSnapInfoIt++;
	}
	return oldSnapInfoIt;
}
SnapshotInfoMapIterator DemoReader::SnapNullIt() {
	return snapshotInfos.end();
}
int DemoReader::GetLastServerTimeBeforeServerTime(int serverTime) {

	if (!SeekToServerTime(serverTime)) return -1;

	if (demoBaseTime) { // Continuity not guaranteed. Do slow linear search
		int lastServerTime = -1;
		for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
			if (it->second.serverTime >= serverTime) return lastServerTime;
			lastServerTime = it->second.serverTime;
		}
	}
	else {
		// Do fast binary search
		auto lower = std::lower_bound(snapshotInfos.begin(), snapshotInfos.end(), serverTime, snapshotInfosServerTimeSmallerPredicate); // Should be first one that has greater server time.
		if (lower != snapshotInfos.end() && lower != snapshotInfos.begin()) {
			return (--lower)->second.serverTime; // ? untested, but should work? 
		}
	}
	return -1; // Shouldn't happen really but whatever
}

SnapshotInfoMapIterator DemoReader::GetSnapshotInfoAtOrBeforeDemoTimeIterator(float demoTime, qboolean includeAfterIfFirst) {
	if (SeekToTime(demoTime) || (demoTime < 0.0f && SeekToAnySnapshotIfNotYet() && includeAfterIfFirst)) {
		auto lower = std::lower_bound(snapshotInfos.begin(), snapshotInfos.end(), demoTime, snapshotInfosDemoTimeSmallerPredicate);
		if (lower != snapshotInfos.end()) {
			if (lower->second.demoTime == demoTime) {
				return lower;
			}
			else if (lower != snapshotInfos.begin()) {
				lower--;
				return lower;
			}
			else if(includeAfterIfFirst){
				return lower;
			}
		}
		return snapshotInfos.end(); // Can happen if that particular time's snapshot is missing from the demo.
	}
	else {
		return snapshotInfos.end();
	}

}
SnapshotInfoMapIterator DemoReader::GetSnapshotInfoAtServerTimeIterator(int serverTime) {
	if (SeekToServerTime(serverTime)) {
		if (demoBaseTime) { // demoBaseTime should be 0 for any regular demo BUT some demos might include a serverTime reset, in which case demoBaseTime is increased. At this point there is no more serverTime regularity guaranteed hence we cannot use binary search and have to do a slow linear search
			for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
				if (it->second.serverTime == serverTime) return it;
			}
		}
		else { // Binary search
			auto lower = std::lower_bound(snapshotInfos.begin(),snapshotInfos.end(),serverTime, snapshotInfosServerTimeSmallerPredicate);
			if (lower != snapshotInfos.end() && lower->second.serverTime == serverTime) return lower;
		}
		return snapshotInfos.end(); // Can happen if that particular time's snapshot is missing from the demo.
	}
	else {
		return snapshotInfos.end();
	}

}
SnapshotInfo* DemoReader::GetSnapshotInfoAtServerTime(int serverTime) {
	SnapshotInfoMapIterator it = GetSnapshotInfoAtServerTimeIterator(serverTime);
	return it!= snapshotInfos.end() ? &it->second : NULL;
}
SnapshotInfo* DemoReader::GetSnapshotInfoAtOrBeforeDemoTime(float demoTime, qboolean includeAfterIfFirst) {
	SnapshotInfoMapIterator it = GetSnapshotInfoAtOrBeforeDemoTimeIterator(demoTime, includeAfterIfFirst);
	return it!= snapshotInfos.end() ? &it->second : NULL;
}

// Don't call if you're not sure if the snapnum exists.
// playerStateSourceSnap is in case we are creating a playerstate from an entity and we are searching backwards and forwards for a playerState to fill in some info (health, armor)
// So that will return (if desired) the source snap used for that playerstate fill in info
playerState_t DemoReader::GetPlayerFromSnapshot(int clientNum, int snapNum, SnapshotInfoMapIterator* playerStateSourceSnap, qboolean detailedPS) {
	SnapshotInfoMapIterator snapIt = snapshotInfos.find(snapNum);
	return GetPlayerFromSnapshot(clientNum, snapIt, playerStateSourceSnap, detailedPS);
}

// playerStateSourceSnap is in case we are creating a playerstate from an entity and we are searching backwards and forwards for a playerState to fill in some info (health, armor)
// So that will return (if desired) the source snap used for that playerstate fill in info or if we get the playerstate directly from the current snap, it just has the current snap.
playerState_t DemoReader::GetPlayerFromSnapshot(int clientNum, SnapshotInfoMapIterator snapInfoIterator, SnapshotInfoMapIterator* playerStateSourceSnap, qboolean detailedPS) {
	SnapshotInfo* snap = &snapInfoIterator->second;
	if (playerStateSourceSnap) *playerStateSourceSnap = snapshotInfos.end();
	if (snap->playerState.clientNum == clientNum) {
		if (playerStateSourceSnap) *playerStateSourceSnap = snapInfoIterator;
		return snap->playerState;
	}
	else {
		playerState_t retVal;
		Com_Memset(&retVal, 0, sizeof(playerState_t));

		const auto nullSnapIt = snapshotInfos.end();
		auto baseSnapIt = nullSnapIt;
		//int baseSnap = -1;
		if(detailedPS){ // Try to restore stuff like health/armor, pers_spawn_count etc. Use for main player in demo.

			auto pastSnapIt = nullSnapIt;
			auto futureSnapIt = nullSnapIt;
			//int pastSnap = -1;
			//int futureSnap = -1;
			// Search if there is a past and/or future playerstate.
			// TODO What about future ones not read yet? It might happen minutes later or whatever...
			SeekToServerTime(snap->serverTime+ PLAYERSTATE_FUTURE_SEEK);
			int firstPacketWithPlayerStateOfThisPlayer = firstPacketWithPlayerState[clientNum];
			int lastKnownPacketWithPlayerStateOfThisPlayer = lastKnownPacketWithPlayerState[clientNum];
			if (firstPacketWithPlayerStateOfThisPlayer != -1) { // We don't have  any playerstate of this player at all. Don't bother trying to find the last and next, what for? Waste of resources.
				
				// Linear search forwards and backwaards from current time (find current time via fast binary search)

				// Past:
				// Get the current snapshot quickly. TODO Maybe pass iterator straight in here for even faster?
				//auto itStart = std::lower_bound(snapshotInfos.begin(), snapshotInfos.end(), firstPacketWithPlayerStateOfThisPlayer < snapNum ? snapNum : firstPacketWithPlayerStateOfThisPlayer, snapshotInfosSnapNumPredicate);
				auto itStart = snapInfoIterator;
				if (itStart != snapshotInfos.end()) { // Just to be safe.

					auto it = itStart;

					if (lastKnownPacketWithPlayerStateOfThisPlayer < it->second.snapNum) {
						it = snapshotInfos.find(lastKnownPacketWithPlayerStateOfThisPlayer);
					}

					if (firstPacketWithPlayerStateOfThisPlayer < itStart->first) {

						//while (pastSnap == -1 && it->first >= firstPacketWithPlayerStateOfThisPlayer) { // To explain the second condition: If we know that the FIRST playerstate ever of this player was snap 2356, there is no need to search snaps 0 to 2355.
						while (pastSnapIt == nullSnapIt && it->first >= firstPacketWithPlayerStateOfThisPlayer) { // To explain the second condition: If we know that the FIRST playerstate ever of this player was snap 2356, there is no need to search snaps 0 to 2355.
							if (it->first < itStart->first && it->second.playerState.clientNum == clientNum) {
								pastSnapIt = it;
								break;
							}
							if (it == snapshotInfos.begin()) {
								break;
							}
							it--;
						}

					}

					// Future:
					it = itStart;

					//while (futureSnap == -1 && it->first <= lastKnownPacketWithPlayerStateOfThisPlayer) { // To explain the second condition: If we know that the LAST KNOWN playerstate ever of this player was snap 2356, there is no need to search snaps 2357+.
					while (futureSnapIt == nullSnapIt && it->first <= lastKnownPacketWithPlayerStateOfThisPlayer) { // To explain the second condition: If we know that the LAST KNOWN playerstate ever of this player was snap 2356, there is no need to search snaps 2357+.
						if (it->first > itStart->first && it->second.playerState.clientNum == clientNum) {
							futureSnapIt = it;
							break;
						}
						it++;
						if (it == snapshotInfos.end()) {
							break;
						}
					}

				}
				
				/*if (demoBaseTime) { // Linearity not guaranteed, do linear search. (TODO This is prolly pointless anyway because DemoReader likely doesn't deal well with resetted serverTimes etc)

					for (auto snapIt = snapshotInfos.begin(); snapIt != snapshotInfos.end(); snapIt++) {
						if (snapIt->first < snapNum && snapIt->second.playerState.clientNum == clientNum) {
							pastSnap = snapIt->first;
						}
						if (snapIt->first > snapNum && snapIt->second.playerState.clientNum == clientNum) {
							futureSnap = snapIt->first;
							break;
						}
					}
				}
				else {
					// Linear search forwards and backwaards from current time (find current time via fast binary search)

					// Past:
					// Get the current snapshot quickly. TODO Maybe pass iterator straight in here for even faster?
					auto itStart = std::lower_bound(snapshotInfos.begin(), snapshotInfos.end(), firstPacketWithPlayerStateOfThisPlayer < snapNum ? snapNum : firstPacketWithPlayerStateOfThisPlayer, snapshotInfosSnapNumPredicate);
					if (itStart != snapshotInfos.end()) { // Just to be safe.

						auto it = itStart;

						if (firstPacketWithPlayerStateOfThisPlayer < snapNum) {

							while (pastSnap == -1 && it->first >= firstPacketWithPlayerStateOfThisPlayer) { // To explain the second condition: If we know that the FIRST playerstate ever of this player was snap 2356, there is no need to search snaps 0 to 2355.
								if (it->first < snapNum && it->second.playerState.clientNum == clientNum) {
									pastSnap = it->first;
									break;
								}
								if (it == snapshotInfos.begin()) {
									break;
								}
								it--;
							}

						}

						// Future:
						it = itStart;
					
						while (futureSnap == -1) { // To explain the second condition: If we know that the FIRST playerstate ever of this player was snap 2356, there is no need to search snaps 0 to 2355.
							if (it->first > snapNum && it->second.playerState.clientNum == clientNum) {
								futureSnap = it->first;
								break;
							}
							it++;
							if (it == snapshotInfos.end()) {
								break;
							}
						}

					}
				}*/
			}
			baseSnapIt = pastSnapIt != nullSnapIt ? pastSnapIt : futureSnapIt;
			//baseSnap = pastSnap != -1 ? pastSnap : futureSnap;
		}

		entityState_t* thisEntity = &snap->entities[clientNum];
		// Need to convert the entity.
		if (baseSnapIt != nullSnapIt && playerStateSourceSnap) *playerStateSourceSnap = baseSnapIt;
		CG_EntityStateToPlayerState(thisEntity, &retVal, demoType, qtrue, baseSnapIt != nullSnapIt ? &baseSnapIt->second.playerState : &basePlayerStates[clientNum]);

		if (isMOHAADemo) {

			int normalizedPSFlags = MOH_CPT_NormalizePlayerStateFlags(snap->playerState.net_pm_flags,demoType);
			//if ((normalizedPSFlags & PMF_CAMERA_VIEW_MOH) && (normalizedPSFlags & PMF_SPECTATING_MOH) && VectorDistance(thisEntity->pos.trBase, snap->playerState.origin) < 0.01f) {
			if ((normalizedPSFlags & PMF_CAMERA_VIEW_MOH) && (normalizedPSFlags & PMF_SPECTATING_MOH) && (demoType == DM3_MOHAA_PROT_15 ? VectorDistance(thisEntity->pos.trBase, snap->playerState.origin) < 0.5f : VectorSame(thisEntity->pos.trBase, snap->playerState.origin))) { // In expansions, probably due to netcode changes, we cannot check for equality of the vectors. We must compare for them being very close instead.
				
				// We are likely following/spectating this player, and can thus infer some extra info. 
				// For example we can copy over the fov which helps with sniper zoom stuff and such
				retVal.fovMOHAA = snap->playerState.fovMOHAA;
				if (retVal.fovMOHAA <= 60) { // Sniper zoom
					retVal.stats[STAT_INZOOM_MOH] = retVal.fovMOHAA;
					retVal.stats[STAT_CROSSHAIR_MOH] = qfalse;
				}


				/*if (VectorSame(thisEntity->apos.trBase, snap->playerState.viewangles)) { // Can't compare angles. For some reason they don't stay same for spectating.
					std::cout << "Cool";
				}*/
			}

			if (thisEntity->number == snap->playerState.stats[STAT_INFOCLIENT_MOH]) { // We are getting the health info for this player from a playerstate.

				retVal.stats[STAT_HEALTH_MOH] = snap->playerState.stats[STAT_INFOCLIENT_HEALTH_MOH];
			}

			if (snap->mohaaPlayerWeapon[clientNum] != -1) {
				int mohaaWeaponNum = specializeGameValue<GMAP_WEAPONS, UNSAFE>(snap->mohaaPlayerWeapon[clientNum], demoType);
				const mohWeaponInfo_t* weaponInfo = &mohWeaponsInfo[mohaaWeaponNum];
				const char* csWeaponName = weaponInfo->csName;
				retVal.stats[STAT_EQUIPPED_WEAPON_MOH] = weaponInfo->weaponClassBitmask;
				retVal.stats[STAT_WEAPONS_MOH] = weaponInfo->weaponClassBitmask;
				if (*csWeaponName) { // Check for empty string

					for (int i = CS_WEAPONS_MOH; i < CS_SOUNDTRACK_MOH; i++) { // from 1748 to 1881 but I think 1881 is too high of a limit actually but whatever, won't hurt.
						const char* compareWeaponName = this->GetConfigString(i, NULL);
						if (!_stricmp(csWeaponName, compareWeaponName)) {
							retVal.activeItems[1] = i- CS_WEAPONS_MOH;
							vmAnimMoh_e viewModelAnim = VM_ANIM_IDLE_MOH;
							if (demoType == DM3_MOHAA_PROT_6) {
								/*// Don't have it done for spearhead etc stuff
								float reloadWeight = 0;
								float fireWeight = 0;
								for (int f = 0; f < 16; f++) {
									if (thisEntity->frameInfo[f].index == 150) {
										reloadWeight = std::max(0.001f, thisEntity->frameInfo[f].weight);
									} else if (thisEntity->frameInfo[f].index == 157) {
										fireWeight = std::max(0.001f, thisEntity->frameInfo[f].weight);
									}
								}
								if (fireWeight > reloadWeight) {
									viewModelAnim = VM_ANIM_FIRE_MOH;
								}
								else if (reloadWeight > fireWeight) {
									viewModelAnim = VM_ANIM_RELOAD_MOH;
								}*/
								float minWeight = -9999.9f;
								bool nonIdleTypeFound = false;
								for (int f = 0; f < 16; f++) {
									vmAnimMoh_e animThisFrame = playerAnimToVMAnimMap[thisEntity->frameInfo[f].index];
									if (animThisFrame != VM_ANIM_IDLE_MOH) {
										bool useThis = false;
										if (animThisFrame == VM_ANIM_IDLE_0_MOH || animThisFrame == VM_ANIM_IDLE_1_MOH || animThisFrame == VM_ANIM_IDLE_2_MOH) {
											if (!nonIdleTypeFound && thisEntity->frameInfo[f].weight > minWeight) {
												useThis = true;
											}
										}
										else {
											if (!nonIdleTypeFound || thisEntity->frameInfo[f].weight > minWeight) {
												useThis = true;
											}
											nonIdleTypeFound = true;
										}
										if (useThis) {
											minWeight = thisEntity->frameInfo[f].weight;
											viewModelAnim = playerAnimToVMAnimMap[thisEntity->frameInfo[f].index];
										}
									}
								}
								
							}
							retVal.iNetViewModelAnim = demoType == DM3_MOHAA_PROT_6 ? viewModelAnim -1 : viewModelAnim;
							//retVal.iNetViewModelAnim = demoType == DM3_MOHAA_PROT_6 ? VM_ANIM_FIRE_MOH-1 : VM_ANIM_FIRE_MOH;
							break;
						}
					}
				}
			}
		}

		//CG_EntityStateToPlayerState(thisEntity, &retVal,demoType,qtrue,baseSnap != -1 ? &snapshotInfos[baseSnap].playerState : &basePlayerStates[clientNum]);

		if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
			retVal.commandTime = thisEntity->pos.trTime;
		}
		else {
			retVal.commandTime = snap->serverTime; // Otherwise just use servertime. Lame but oh well. Maybe we could do sth better where we try to detect changes in values or such if we truly need to.
		}

		return retVal;
	}
}

playerState_t DemoReader::GetInterpolatedPlayer(int clientNum, double time, SnapshotInfo** oldSnap, SnapshotInfo** newSnap, qboolean detailedPS, float* translatedTime) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));

	if (oldSnap) *oldSnap = NULL;
	if (newSnap) *newSnap = NULL;

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (translatedTime) {
		*translatedTime = time;
	}

	SeekToPlayerCommandOrServerTime(clientNum,time);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	int lastPastSnap = -1;
	int lastPastSnapCommandTime = -1;
	int firstPacketWithPlayerInIt = -1;
	SnapshotInfoMapIterator lastPastSnapIt;
	SnapshotInfoMapIterator firstPacketWithPlayerInItIt;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		//if (it->second.serverTime <= time) {
		
		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.
		
		if (firstPacketWithPlayerInIt == -1) { 
			firstPacketWithPlayerInItIt = it;
			firstPacketWithPlayerInIt = it->first; 
		}
		
		if (it->second.playerCommandOrServerTimes[clientNum] <= time) {
			lastPastSnapIt = it;
			lastPastSnap = it->first;
			//lastPastSnapCommandTime = it->second.playerState.commandTime;
			lastPastSnapCommandTime = it->second.playerCommandOrServerTimes[clientNum];
		}
	}
	if (lastPastSnap == -1) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have with the player in it
		if (firstPacketWithPlayerInIt == -1) {
			// Uhm. Ok. Maybe handle this better at some later time but for now we just return that empty playerState.
			// We should probably keep seeking then or sth.
			return retVal;
		}
		else {
			//if (oldSnap) *oldSnap = &firstPacketWithPlayerInItIt->second;
			//if (newSnap) *newSnap = &firstPacketWithPlayerInItIt->second;
			//return GetPlayerFromSnapshot(clientNum,&snapshotInfos[firstPacketWithPlayerInIt]);
			return GetPlayerFromSnapshot(clientNum,firstPacketWithPlayerInIt,NULL, detailedPS);
			/*SnapshotInfo* thatSnapshot = &snapshotInfos[firstPacketWithPlayerInIt];
			if (thatSnapshot->playerState.clientNum == clientNum) {
				return thatSnapshot->playerState;
			}
			else {
				// Need to convert the entity.
				CG_EntityStateToPlayerState(&thatSnapshot->entities[clientNum],&retVal);
				return retVal;
			}*/
		}
	}

	// Ok now we wanna make sure we have at least one snap after the last one before "time" that has a different commandTime so we have something to interpolate.
	while (lastPastSnapCommandTime >= lastKnownCommandOrServerTimes[clientNum] && !endReached) {
		ReadMessage();
	}

	// already at end, nothing we can interpolate.
	// Just return the last state we have about this player.
	// TODO actually: give way to return info about whether demo is over and such. So we dont end up with stuck entities in one place, tehy should disappear instead.
	// Although... in case of short visibility interruptions it might actually be better to keep returning interpolated values? (but is that related?)
	if (lastPastSnapCommandTime >= lastKnownCommandOrServerTimes[clientNum] && endReached) {
		if (lastMessageWithEntity.find(clientNum) == lastMessageWithEntity.end()) {
			// We never knew anything about this player at all.
			return retVal;
		}
		else {

			if (oldSnap) *oldSnap = &snapshotInfos[lastMessageWithEntity[clientNum]];
			if (newSnap) *newSnap = &snapshotInfos[lastMessageWithEntity[clientNum]];
			//return GetPlayerFromSnapshot(clientNum, &snapshotInfos[lastMessageWithEntity[clientNum]]);
			return GetPlayerFromSnapshot(clientNum, lastMessageWithEntity[clientNum],NULL,detailedPS);
		}
	}

	// Okay now we want to locate the first snap with a different commandtime than lastPastSnap and then interpolate between the two.
	int firstNextSnap = -1;
	SnapshotInfoMapIterator firstNextSnapIt = lastPastSnapIt;
	//for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
	for (auto it = lastPastSnapIt; it != snapshotInfos.end(); it++) { // change: start directly from last snap. why start from begin()? Or does the iterator become invalid? it shouldn't..

		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.

		if (it->second.playerCommandOrServerTimes[clientNum] > lastPastSnapCommandTime) {
			firstNextSnap = it->first;
			firstNextSnapIt = it;
			break;
		}
	}

	if (oldSnap) *oldSnap = &lastPastSnapIt->second;//&snapshotInfos[lastPastSnap];
	if (newSnap) *newSnap = firstNextSnap == -1 ? NULL : &firstNextSnapIt->second;//&snapshotInfos[firstNextSnap];

	// Okay now we know the messageNum of before and after. Let's interpolate! How exciting!
	//InterpolatePlayer(clientNum,time, &snapshotInfos[lastPastSnap], &snapshotInfos[firstNextSnap], &retVal, detailedPS);
	InterpolatePlayer(clientNum,time, &lastPastSnapIt->second, firstNextSnap == -1 ? NULL : &firstNextSnapIt->second, &retVal, detailedPS);
	InterpolatePlayer(clientNum,time, &lastPastSnapIt->second, firstNextSnap == -1 ? NULL : &firstNextSnapIt->second, &retVal, detailedPS);

	return retVal;
	
}

// It's like a cheaper version of GetInterpolatedPlayer. We don't interpolate anything. We just return the last player state or the next one if no last one exists.
// It ignores command times and is based purely on server time.
// arguments oldSnap and newSnap do nothing atm.
// Function name seems a misnomer btw. It doesn't really search into the future? Oh wait it does, nvm.
playerState_t DemoReader::GetLastOrNextPlayer(int clientNum, int serverTime, SnapshotInfoMapIterator* usedSourceSnap, SnapshotInfoMapIterator* usedSourcePlayerStateSnap, qboolean detailedPS, const SnapshotInfoMapIterator* referenceSnapIt) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));

	if (usedSourceSnap) *usedSourceSnap = snapshotInfos.end();
	if (usedSourcePlayerStateSnap) *usedSourcePlayerStateSnap = snapshotInfos.end();

	SeekToAnySnapshotIfNotYet();
	SeekToServerTime(serverTime);

	SeekToPlayerInPacket(clientNum);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	//  Avoid searching around forever if we already know the player is in this snapshot
	//if (referenceSnap && referenceSnap->playerCommandOrServerTimes.find(clientNum) != referenceSnap->playerCommandOrServerTimes.end()) {
	SnapshotInfo* referenceSnap =&(*referenceSnapIt)->second;
	if (referenceSnap && referenceSnap->hasPlayer[clientNum]) {
		if (usedSourceSnap) *usedSourceSnap = *referenceSnapIt;
		return GetPlayerFromSnapshot(clientNum, referenceSnap->snapNum, usedSourcePlayerStateSnap, detailedPS);
	}

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	SnapshotInfoMapIterator lastPastSnap = snapshotInfos.end();
	int lastPastSnapServerTime = -1;
	int firstPacketWithPlayerInIt = firstPacketWithPlayer[clientNum];
	int lastKnownPacketWithPlayerInIt = lastKnownPacketWithPlayer[clientNum];
	/*int firstPacketWithPlayerInIt = -1;*/
	if (demoBaseTime) { // There has been a servertime reset. Values are not continuous. Gotta do the slow algorithm. Tbh everything's probably fucked at this point anyway but whatever.

		for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {

			//if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.
			if (!it->second.hasPlayer[clientNum]) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.

			//if (firstPacketWithPlayerInIt == -1) firstPacketWithPlayerInIt = it->first;

			if (it->second.serverTime <= serverTime) {
				lastPastSnap = it;
				lastPastSnapServerTime = it->second.serverTime;
			}
		}
	}
	else {
		// K let's do this a quicker way. We search in reverse from requested current time which we find via binary search.
		auto it = *referenceSnapIt;
		if (lastKnownPacketWithPlayerInIt < it->second.snapNum) {
			// If it's been very long since we've seen the player, don't backtrack too much, just start directly from there.
			// This might be a bit slower for short demos but should be faster for very long demos hopefully.
			it = snapshotInfos.find(lastKnownPacketWithPlayerInIt); 
		}
		if (it != snapshotInfos.end()) {
			while (lastPastSnap == snapshotInfos.end()) {
				if (it->second.serverTime <= serverTime && it->second.hasPlayer[clientNum]) {
					lastPastSnap = it;
					lastPastSnapServerTime = it->second.serverTime;
					break;
				}
				if (it == snapshotInfos.begin()) {
					break;
				}
				it--;
			}
		}

	}
	if (lastPastSnap == snapshotInfos.end()) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have with the player in it
		if (firstPacketWithPlayerInIt == -1) {
			// Uhm. Ok. Maybe handle this better at some later time but for now we just return that empty playerState.
			// We should probably keep seeking then or sth.
			return retVal;
		}
		else {
			SnapshotInfoMapIterator firstPacketIt = snapshotInfos.find(firstPacketWithPlayerInIt);
			if (usedSourceSnap) *usedSourceSnap = firstPacketIt;
			return GetPlayerFromSnapshot(clientNum, firstPacketIt, usedSourcePlayerStateSnap, detailedPS);
			
		}
	}
	else {
		if (usedSourceSnap) *usedSourceSnap = lastPastSnap;
		return GetPlayerFromSnapshot(clientNum, lastPastSnap, usedSourcePlayerStateSnap, detailedPS);
	}

	
}

std::map<int, entityState_t> DemoReader::GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, const SnapshotInfoMapIterator* referenceSnapIt) {
	std::map<int, entityState_t> retVal;

	int searchEndTime = serverTime + maxTimeIntoFuture;

	SeekToAnySnapshotIfNotYet();
	SeekToServerTime(serverTime+ searchEndTime);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.


	SnapshotInfoMapIterator lastPastSnap = snapshotInfos.end();

	auto it = *referenceSnapIt;
	while (it != snapshotInfos.end() && it->second.serverTime <= searchEndTime) {
		if (it->second.serverTime >= serverTime) {
			for (auto itEnt = it->second.entities.begin(); itEnt != it->second.entities.end(); itEnt++) {
				if (retVal.find(itEnt->first) == retVal.end()) {
					entityState_t entState = itEnt->second;
					entState.demoToolsData.serverTime = it->second.serverTime;
					retVal[itEnt->first] = entState;
				}
			}
			if (includePlayerStates && retVal.find(it->second.playerState.clientNum) == retVal.end()) {
				entityState_t entState{};
				BG_PlayerStateToEntityState(&it->second.playerState, &entState,qfalse,demoType,qtrue,qfalse);
				entState.demoToolsData.serverTime = it->second.serverTime;
				retVal[it->second.playerState.clientNum] = entState;
			}
		}
		it++;
	}

	return retVal;
	
}

void DemoReader::GetFutureEntityStates(int serverTime, int maxTimeIntoFuture, bool includePlayerStates, std::map<int, entityState_t>* mapToEnhance, const SnapshotInfoMapIterator* referenceSnapIt) {

	int searchEndTime = serverTime + maxTimeIntoFuture;

	SeekToAnySnapshotIfNotYet();
	SeekToServerTime(serverTime+ searchEndTime);

	if (endReached && !anySnapshotParsed) return; // Nothing to do really lol.

	SnapshotInfoMapIterator lastPastSnap = snapshotInfos.end();

	auto it = *referenceSnapIt;
	while (it != snapshotInfos.end() && it->second.serverTime <= searchEndTime) {
		if (it->second.serverTime >= serverTime) {
			for (auto itEnt = it->second.entities.begin(); itEnt != it->second.entities.end(); itEnt++) {
				if (mapToEnhance->find(itEnt->first) == mapToEnhance->end() || (*mapToEnhance)[itEnt->first].demoToolsData.serverTime > it->second.serverTime) { // Put it in if we either don't have it yet, or if our version of the entity is less into the future.
					entityState_t entState = itEnt->second;
					entState.demoToolsData.serverTime = it->second.serverTime;
					(*mapToEnhance)[itEnt->first] = entState;
				}
			}

			if (includePlayerStates && mapToEnhance->find(it->second.playerState.clientNum) == mapToEnhance->end()) {
				entityState_t entState{};
				BG_PlayerStateToEntityState(&it->second.playerState, &entState, qfalse, demoType, qtrue, qfalse);
				entState.demoToolsData.serverTime = it->second.serverTime;
				(*mapToEnhance)[it->second.playerState.clientNum] = entState;
			}
		}
		it++;
	}
	
}
void DemoReader::convertPSTo(playerState_t* ps, demoType_t targetDemoType) { // dont really bother using this unless ur intended output is DM_15 or DM_16. It's a mess.

	if (targetDemoType == demoType)
	{
		return; // Nothing to do.
	}

	if (targetDemoType <= DM_16 && targetDemoType >= DM_15) {
		if (demoType == DM_14) {

			ps->saberMove = convertGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(ps->saberMove, demoType, DM_16);
			ps->torsoAnim = convertGameValue<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, demoType, DM_16);
			ps->legsAnim = convertGameValue<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, demoType, DM_16);
			ps->weapon = convertGameValue<GMAP_WEAPONS, UNSAFE>(ps->weapon, demoType, DM_15);
			ps->genericEnemyIndex = -1; // Don't draw seeker drone pls.
		}
		else if (demoType == DM_26 || demoType == DM_25) {

			//ps->torsoAnim = jkaAnimMapping[ps->torsoAnim];
			ps->saberMove = specializedGameValueMapUnsafe<GMAP_LIGHTSABERMOVE, UNSAFE>(ps->saberMove, demoType, targetDemoType);
			ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, demoType, DM_16);
			if (ps->torsoFlip) ps->torsoAnim |= ANIM_TOGGLEBIT; // Generalize togglebit someday?
			//ps->legsAnim = jkaAnimMapping[ps->legsAnim];
			ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, demoType, DM_16);
			if (ps->legsFlip) ps->legsAnim |= ANIM_TOGGLEBIT;// Generalize togglebit someday?
			//ps->weapon = jkaWeaponMap[ps->weapon];
			ps->weapon = specializedGameValueMapUnsafe<GMAP_WEAPONS, UNSAFE>(ps->weapon, demoType, targetDemoType);
		}
		else if (demoType == DM_68) { // TODO Allow other ones too? But idk if anims changed

		 //ps->torsoAnim = MapQ3AnimToJK2(ps->torsoAnim);
			ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, demoType, DM_16);
			//ps->legsAnim = MapQ3AnimToJK2(ps->legsAnim);
			ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, demoType, DM_16);
			//ps->weapon = q3WeaponMap[ps->weapon];
			ps->weapon = specializedGameValueMapUnsafe<GMAP_WEAPONS, UNSAFE>(ps->weapon, demoType, targetDemoType);
			ps->genericEnemyIndex = -1; // Don't draw seeker drone pls.
		}
		if (targetDemoType == DM_15 &&( demoType == DM_14 || demoType == DM_16 || demoType == DM_26 || demoType == DM_25 || demoType == DM_68)) { // TODO: Do all this more elegeantly? Please?

			//ps->torsoAnim = animMappingTable_1_04_to_1_02[ps->torsoAnim];
			//ps->legsAnim = animMappingTable_1_04_to_1_02[ps->legsAnim];
			//ps->torsoAnim = MV_MapAnimation102(ps->torsoAnim);
			ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, DM_16, DM_15);
			//ps->legsAnim = MV_MapAnimation102(ps->legsAnim);
			ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, DM_16, DM_15);
		}
		else if (demoType == DM_15 && (targetDemoType == DM_16 || targetDemoType == DM_15_1_03)) {
			ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, DM_15, DM_16);
			ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, DM_15, DM_16);
		}

		if (demoType < DM_15 || demoType > DM_16) {
			ps->events[0] = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->events[0], demoType, targetDemoType);
			ps->events[1] = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->events[1], demoType, targetDemoType);
			ps->externalEvent = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->externalEvent, demoType, targetDemoType);
		}
	}
	else {
		// Generic handling if output isn't DM_15 to DM_16 (not really supported... dont do it or dont cry xd)
		ps->saberMove = convertGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(ps->saberMove, demoType, targetDemoType);
		ps->torsoAnim = convertGameValue<GMAP_ANIMATIONS, UNSAFE>(ps->torsoAnim, demoType, targetDemoType);
		ps->legsAnim = convertGameValue<GMAP_ANIMATIONS, UNSAFE>(ps->legsAnim, demoType, targetDemoType);
		ps->weapon = convertGameValue<GMAP_WEAPONS, UNSAFE>(ps->weapon, demoType, targetDemoType);
		ps->events[0] = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->events[0], demoType, targetDemoType);
		ps->events[1] = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->events[1], demoType, targetDemoType);
		ps->externalEvent = convertGameValue<GMAP_EVENTS, UNSAFE>(ps->externalEvent, demoType, targetDemoType);
	}

}

playerState_t DemoReader::GetInterpolatedPlayerState(double time) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));
	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;
	SeekToCommandTime(time);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".
	
	int lastPastSnap = -1;
	int lastPastSnapCommandTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		//if (it->second.serverTime <= time) {
		if (it->second.playerState.commandTime <= time) {
			lastPastSnap = it->first;
			lastPastSnapCommandTime = it->second.playerState.commandTime;
		}
	}

	if (lastPastSnap == -1) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have.
		retVal = snapshotInfos.begin()->second.playerState;
		return retVal;
	}

	// Ok now we wanna make sure we have at least one snap after the last one before "time" that has a different commandTime so we have something to interpolate.
	while (lastPastSnapCommandTime >= thisDemo.cut.Clc.lastKnownCommandTime && !endReached) {
		ReadMessage();
	}

	// already at end, nothing we can interpolate.
	if (lastPastSnapCommandTime >= thisDemo.cut.Clc.lastKnownCommandTime && endReached) {
		retVal = thisDemo.cut.Cl.snap.ps;
		return retVal;
	}

	// Okay now we want to locate the first snap with a different commandtime than lastPastSnap and then interpolate between the two.
	int firstNextSnap =-1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.playerState.commandTime > lastPastSnapCommandTime) {
			firstNextSnap = it->first;
			break;
		}
	}

	// Okay now we know the messageNum of before and after. Let's interpolate! How exciting!
	InterpolatePlayerState(time, &snapshotInfos[lastPastSnap], &snapshotInfos[firstNextSnap], &retVal);

	return retVal;
}


void DemoReader::InterpolatePlayerState(double time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS) {
	float			f;
	int				i;
	playerState_t* out;
	//snapshot_t* prev, * next;
	playerState_t* curps = NULL, * nextps = NULL;
	//qboolean		nextPsTeleport = qfalse;
	qboolean		nextPsTeleport = to->playerStateTeleport;
	int currentTime = 0, nextTime = 0, currentServerTime = 0, nextServerTime = 0;

	out = outPS;
	*out = from->playerState;
	//prev = cg.snap;
	//next = cg.nextSnap;

	if (1) {
		//timedPlayerState_t* tps, * nexttps;
		//CG_ComputeCommandSmoothPlayerstates(&tps, &nexttps, &nextPsTeleport);
		curps = &from->playerState;
		currentTime = curps->commandTime;
		//currentTime = from->serverTime;
		currentServerTime = from->serverTime;
		if (to) {
			nextps = &to->playerState;
			nextTime = nextps->commandTime;
			//nextTime = to->serverTime; // Testing. See if we can find the perfect way to do it ... hmm
			nextServerTime = to->serverTime;
		}
	}
	if (!to) {
		return;
	}/*
#ifdef _DEBUG
	if (!(currentTime <= cg.time + (double)cg.timeFraction && nextTime >= cg.time + (double)cg.timeFraction)) {
		Com_Printf("WARNING: Couldn't locate slots with correct time\n");
	}
	if (nextTime - currentTime < 0) {
		Com_Printf("WARNING: nexttps->time - tps->time < 0 (%d, %d)\n", nextTime, currentTime);
	}
#endif
	if (currentTime != nextTime) {
		f = ((double)cg.timeFraction + (cg.time - currentTime)) / (nextTime - currentTime);
#ifdef _DEBUG
		if (f > 1) {
			Com_Printf("EXTRAPOLATING (f=%f)\n", f);
		}
		else if (f < 0) {
			Com_Printf("GOING BACKWARDS (f=%f)\n", f);
		}
#endif
	}
	else {
		f = 0;
	}
	*/
	f = ((double)time - (double)currentTime) / ((double)nextTime - (double)currentTime);
	*out = *curps;
	//out->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];


	// if the next frame is a teleport, we can't lerp to it
	if (nextPsTeleport) { // TODO make that actually matter
		return;
	}

	/*if ( !next || next->serverTime <= prev->serverTime ) {
		return;
	}*/

	i = nextps->bobCycle;
	if (i < curps->bobCycle) {
		i += 256;		// handle wraparound
	}
	out->bobCycle = curps->bobCycle + f * (i - curps->bobCycle);

	for (i = 0; i < 3; i++) {
		out->origin[i] = curps->origin[i] + f * (nextps->origin[i] - curps->origin[i]);
		//if (!grabAngles) {
			out->viewangles[i] = LerpAngle(
				curps->viewangles[i], nextps->viewangles[i], f);
		//}
		out->velocity[i] = curps->velocity[i] +
			f * (nextps->velocity[i] - curps->velocity[i]);
	}

	// requires commandSmooth 2, since it needs entity state history of the mover
	/*if (cg_commandSmooth.integer > 1) {
		// adjust for the movement of the groundentity
		// step 1: remove the delta introduced by cur->next origin interpolation
		int curTime = currentServerTime + (int)(f * (nextServerTime - currentServerTime));
		float curTimeFraction = (f * (nextServerTime - currentServerTime));
		curTimeFraction -= (long)curTimeFraction;
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, curTime, curTimeFraction, currentServerTime, 0, out->origin);
		// step 2: mover state should now be what it was at currentServerTime, now redo calculation of mover effect to cg.time
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, currentServerTime, 0, cg.time, cg.timeFraction, out->origin);
	}*/ // TODO Fix this?

	//curps->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];
	//BG_PlayerStateToEntityState(curps, &cg_entities[curps->clientNum].currentState, qfalse);
	//BG_PlayerStateToEntityState(nextps, &cg_entities[nextps->clientNum].nextState, qfalse);
	//cg.playerInterpolation = f;
	
	/*if (cg.timeFraction >= cg.nextSnap->serverTime - cg.time) {
		cg.physicsTime = cg.nextSnap->serverTime;
	}
	else {
		cg.physicsTime = cg.snap->serverTime;
	}*/ // TODO Relevant?


	// TODO: What about this? not in jamme
	//cg.predictedTimeFrac = f * (next->ps.commandTime - prev->ps.commandTime);
	//cg.predictedTimeFrac = f * (nextps->commandTime - curps->commandTime);
}

void DemoReader::InterpolatePlayer(int clientNum, double time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS, qboolean detailedPS) {
	float			f;
	int				i;
	playerState_t* out;
	//snapshot_t* prev, * next;
	playerState_t* curps = NULL, * nextps = NULL;
	//qboolean		nextPsTeleport = qfalse;
	//qboolean		nextPsTeleport = to->playerStateTeleport;
	qboolean		nextPsTeleport = qfalse;
	int currentTime = 0, nextTime = 0, currentServerTime = 0, nextServerTime = 0;

	out = outPS;
	*out = from->playerState;
	//prev = cg.snap;
	//next = cg.nextSnap;

	playerState_t inPsConverted, outPsConverted;

	if (1) {


		//inPsConverted = GetPlayerFromSnapshot(clientNum,from);
		inPsConverted = GetPlayerFromSnapshot(clientNum,from->snapNum,NULL, detailedPS);
		curps = &inPsConverted;
		currentTime = curps->commandTime;
		//currentTime = from->serverTime;
		currentServerTime = from->serverTime;
		if (to) {
			outPsConverted = GetPlayerFromSnapshot(clientNum, to->snapNum,NULL, detailedPS);
			nextps = &outPsConverted;
			nextTime = nextps->commandTime;
			//nextTime = to->serverTime; // Testing. See if we can find the perfect way to do it ... hmm
			nextServerTime = to->serverTime;
		}
	}
	if (!to) {
		return;
	}
	
	nextPsTeleport = (qboolean)( to->snapFlagServerCount || ((curps->eFlags ^ nextps->eFlags) & getEF_TELEPORTBIT(demoType)));
	
	/*
#ifdef _DEBUG
	if (!(currentTime <= cg.time + (double)cg.timeFraction && nextTime >= cg.time + (double)cg.timeFraction)) {
		Com_Printf("WARNING: Couldn't locate slots with correct time\n");
	}
	if (nextTime - currentTime < 0) {
		Com_Printf("WARNING: nexttps->time - tps->time < 0 (%d, %d)\n", nextTime, currentTime);
	}
#endif
	if (currentTime != nextTime) {
		f = ((double)cg.timeFraction + (cg.time - currentTime)) / (nextTime - currentTime);
#ifdef _DEBUG
		if (f > 1) {
			Com_Printf("EXTRAPOLATING (f=%f)\n", f);
		}
		else if (f < 0) {
			Com_Printf("GOING BACKWARDS (f=%f)\n", f);
		}
#endif
	}
	else {
		f = 0;
	}
	*/
	f = ((double)time - (double)currentTime) / ((double)nextTime - (double)currentTime);
	*out = *curps;
	//out->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];


	// if the next frame is a teleport, we can't lerp to it
	if (nextPsTeleport) { // TODO make that actually matter
		return;
	}

	/*if ( !next || next->serverTime <= prev->serverTime ) {
		return;
	}*/

	i = nextps->bobCycle;
	if (i < curps->bobCycle) {
		i += 256;		// handle wraparound
	}
	out->bobCycle = curps->bobCycle + f * (i - curps->bobCycle);

	for (i = 0; i < 3; i++) {
		out->origin[i] = curps->origin[i] + f * (nextps->origin[i] - curps->origin[i]);
		//if (!grabAngles) {
			out->viewangles[i] = LerpAngle(
				curps->viewangles[i], nextps->viewangles[i], f);
		//}
		out->velocity[i] = curps->velocity[i] +
			f * (nextps->velocity[i] - curps->velocity[i]);
	}

	// requires commandSmooth 2, since it needs entity state history of the mover
	/*if (cg_commandSmooth.integer > 1) {
		// adjust for the movement of the groundentity
		// step 1: remove the delta introduced by cur->next origin interpolation
		int curTime = currentServerTime + (int)(f * (nextServerTime - currentServerTime));
		float curTimeFraction = (f * (nextServerTime - currentServerTime));
		curTimeFraction -= (long)curTimeFraction;
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, curTime, curTimeFraction, currentServerTime, 0, out->origin);
		// step 2: mover state should now be what it was at currentServerTime, now redo calculation of mover effect to cg.time
		CG_AdjustInterpolatedPositionForMover(out->origin,
			curps->groundEntityNum, currentServerTime, 0, cg.time, cg.timeFraction, out->origin);
	}*/ // TODO Fix this?

	//curps->stats[STAT_HEALTH] = cg.snap->ps.stats[STAT_HEALTH];
	//BG_PlayerStateToEntityState(curps, &cg_entities[curps->clientNum].currentState, qfalse);
	//BG_PlayerStateToEntityState(nextps, &cg_entities[nextps->clientNum].nextState, qfalse);
	//cg.playerInterpolation = f;
	
	/*if (cg.timeFraction >= cg.nextSnap->serverTime - cg.time) {
		cg.physicsTime = cg.nextSnap->serverTime;
	}
	else {
		cg.physicsTime = cg.snap->serverTime;
	}*/ // TODO Relevant?


	// TODO: What about this? not in jamme
	//cg.predictedTimeFrac = f * (next->ps.commandTime - prev->ps.commandTime);
	//cg.predictedTimeFrac = f * (nextps->commandTime - curps->commandTime);
}





std::map<int,entityState_t> DemoReader::GetEntitiesAtTime(double time, double * translatedTime, int* sourceSnapNum) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

	if (sourceSnapNum) {
		*sourceSnapNum = -1;
	}

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (translatedTime) {
		*translatedTime = time;
	}

	if (endReached && !anySnapshotParsed) return std::map<int, entityState_t>(); // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	int lastPastSnap = -1;
	int lastPastSnapServerTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime <= time) {
			lastPastSnap = it->first;
			lastPastSnapServerTime = it->second.serverTime;
		}
	}
	
	if (sourceSnapNum) {
		*sourceSnapNum = lastPastSnap;
	}

	return snapshotInfos[lastPastSnap].entities;
}

std::map<int,entityState_t> DemoReader::GetEntitiesAtPreciseTime(int time, qboolean includingPS, int* sourceSnapNum) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time+0.5);

	if (sourceSnapNum) {
		*sourceSnapNum = -1;
	}

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (endReached && !anySnapshotParsed) return std::map<int, entityState_t>(); // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime == time) {
			if (sourceSnapNum) {
				*sourceSnapNum = it->first;
			}
			if (includingPS) {

				std::map<int, entityState_t> retVal = it->second.entities;
				entityState_t psEnt;
				Com_Memset(&psEnt, 0, sizeof(psEnt));
				BG_PlayerStateToEntityState(&it->second.playerState, &psEnt,qfalse,demoType,qtrue);
				retVal[it->second.playerState.clientNum] = psEnt;
				return retVal;
			}
			else {
				return it->second.entities;
			}
		}
	}
	return std::map<int, entityState_t>();
}

qboolean DemoReader::purgeSnapsBefore(int snapNum) { 

	int countErase = 0;
	SnapshotInfoMapIterator itEnd = snapshotInfos.find(snapNum);
	SnapshotInfoMapIterator itStart = snapshotInfos.begin();
	if (itEnd != snapshotInfos.end() && itEnd != itStart) {
		snapshotInfos.erase(itStart, itEnd);
		return qtrue;
	}
	return qfalse;
}

std::vector<std::string> DemoReader::GetNewCommands(double time) {
	std::vector<std::string> retVal;
	SeekToTime(time);
	for (int i = 0; i < readCommands.size(); i++) {
		if (readCommands[i].demoTime <= time && readCommands[i].demoTime > lastGottenCommandsTime) {
			retVal.push_back(readCommands[i].command);
		}
	}
	lastGottenCommandsTime = time;
	return retVal;
}
// Don't combine this with GetNewCommands, they keep track in different ways. Pick one or the other.
std::vector<std::string> DemoReader::GetNewCommandsAtServerTime(int serverTime) {
	std::vector<std::string> retVal;
	SeekToServerTime(serverTime);
	for (int i = 0; i < readCommands.size(); i++) {
		if (readCommands[i].serverTime <= serverTime && readCommands[i].serverTime > lastGottenCommandsServerTime) {
			retVal.push_back(readCommands[i].command);
		}
	}
	lastGottenCommandsServerTime = serverTime;
	return retVal;
}

inline bool EventTimeGreaterPredicate(const double& time,const Event& it) {
	return time < it.demoTime;
}

std::vector<Event> DemoReader::GetNewEvents(double time, eventKind_t kind) {
	std::vector<Event> retVal;
	SeekToTime(time);
	auto startIt = readEvents.begin();

	// Fast forward a bit using binary search
	startIt = std::upper_bound(readEvents.begin(),readEvents.end(),lastGottenEventsTime[kind], EventTimeGreaterPredicate);
		
	for (auto it = startIt; it != readEvents.end(); it++) {
		if (it->demoTime <= time /* && it->demoTime > lastGottenEventsTime[kind]*/ && (kind == EK_ALL || it->kind == kind)) {
#ifdef DEBUG
			it->countGiven++;
#endif
			retVal.push_back(*it);
		}
	}
	lastGottenEventsTime[kind] = time;
	return retVal;
}

inline bool EventServerTimeGreaterPredicate(const int& serverTime, const Event& it) {
	return serverTime < it.serverTime;
}

std::vector<Event> DemoReader::GetNewEventsAtServerTime(int serverTime, eventKind_t kind) {
	std::vector<Event> retVal;
	SeekToServerTime(serverTime);

	auto startIt = readEvents.begin();

	// Fast forward a bit using binary search if there was no server time reset (bc continuity in binary search is needed)
	if (!demoBaseTime) {
		startIt = std::upper_bound(readEvents.begin(), readEvents.end(), lastGottenEventsServerTime[kind], EventServerTimeGreaterPredicate);
	}

	for (auto it = startIt; it != readEvents.end(); it++) {
		if (it->serverTime <= serverTime /* && it->serverTime > lastGottenEventsServerTime[kind]*/ && (kind == EK_ALL || it->kind == kind)) {
#ifdef DEBUG
			it->countGiven++;
#endif
			retVal.push_back(*it);
		}
	}
	lastGottenEventsServerTime[kind] = serverTime;
	return retVal;
}

int DemoReader::GetFirstSnapServerTime() {
	SeekToAnySnapshotIfNotYet();
	return thisDemo.cut.Clc.firstSnapServerTime;
}


const char* DemoReader::GetSoundConfigString(int soundNum,int* maxLength) {
	int configStringBaseIndex = getCS_SOUNDS(demoType);// (demoType == DM_26 || demoType == DM_25) ? CS_SOUNDS_JKA : CS_SOUNDS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + soundNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}
const char* DemoReader::GetModelConfigString(int modelNum,int* maxLength) {
	int configStringBaseIndex = getCS_MODELS(demoType); //(demoType == DM_26 || demoType == DM_25) ? CS_MODELS_JKA : CS_MODELS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + modelNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
}


void DemoReader::generateBasePlayerStates() { // TODO expand this to be time-relevant by also reading tinfo and filling health and armor and such
	int maxLength;
	for (int i = 0; i < maxClientsThisDemo; i++) {
		const char* playerCS = GetPlayerConfigString(i,&maxLength);
		int team = atoi(Info_ValueForKey(playerCS, maxLength, "t"));
		basePlayerStates[i].persistant[PERS_TEAM] = team;
		basePlayerStates[i].stats[STAT_HEALTH] = 100;
		basePlayerStates[i].stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH] = 100;
	}
}
void DemoReader::updateConfigStringRelatedInfo() { // TODO expand this to be time-relevant by also reading tinfo and filling health and armor and such
	int maxLength;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[CS_SYSTEMINFO];
	maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	const char* systemInfo = thisDemo.cut.Cl.gameState.stringData + offset;
	int g_entHUDFields = atoi(Info_ValueForKey(systemInfo, maxLength, "g_entHUDFields"));
	extraFieldInfo.g_entHUDieldsValue = g_entHUDFields;
}


int DemoReader::getDemoRecorderClientNum() {
	SeekToAnySnapshotIfNotYet();
	return demo.cut.Clc.clientNum;
}

qboolean DemoReader::ReadMessageReal() {

	//constexpr int svc_mapchange_realCMD = D == DM_26 ? svc_mapchange+1: svc_mapchange;
	//constexpr int svc_EOF_realCMD = D == DM_26 ? svc_EOF+1 : svc_EOF;
	//constexpr int svc_setgame_realCMD = D == DM_26 ? 8 : 100; // A non-JKA demo won't have this, let's just set something ridiculously high


readNext:
	int				buf;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte>			oldDataRaw;
	bool			SEHExceptionCaught = false;

	if (isCompressedFile) {
		oldDataRaw.clear();
		MSG_InitRaw(&oldMsg, &oldDataRaw);
	}
	else {
		MSG_Init(&oldMsg, oldData, sizeof(oldData));
	}
	/* Read the sequence number */
	if (FS_Read(&thisDemo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4) {
		Com_DPrintf("[NOTE] Demo cutter, reading sequence number failed.\n");
		return qfalse;
	}
	thisDemo.cut.Clc.serverMessageSequence = LittleLong(thisDemo.cut.Clc.serverMessageSequence);
	maxSequenceNum = std::max(maxSequenceNum, demo.cut.Clc.serverMessageSequence);
	oldSize -= 4;
	/* Read the message size */
	if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4) {
		Com_DPrintf("[NOTE] Demo cutter, reading message size failed.\n");
		return qfalse;
	}
	oldMsg.cursize = LittleLong(oldMsg.cursize);
	oldSize -= 4;
	/* Negative size signals end of demo */
	if (oldMsg.cursize < 0) {
		Com_Printf("[NOTE] Demo cutter, message cursize < 0.\n");
		return qfalse;
	}
	if (oldMsg.cursize > oldMsg.maxsize) {
		Com_DPrintf("[NOTE] Demo cutter, message size above maxsize.\n");
		return qfalse;
	}
	/* Read the actual message */
	if (FS_Read(&oldMsg, oldHandle) != oldMsg.cursize) {
		Com_DPrintf("[NOTE] Demo cutter, reading actual message failed.\n");
		return qfalse;
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
			Com_DPrintf("Demo reader, read past end of server message.\n");
			return qfalse;
		}
		cmd = MSG_ReadByte(&oldMsg);
		wasFirstCommandByte = (qboolean)!firstCommandByteRead;
		firstCommandByteRead = qtrue;
		cmd = generalizeGameSVCOp(cmd,demoType);
		if (cmd == svc_EOF_general) {
			// TODO Check for svc_extension/svc_voip (ioq3/wolfcamql)
			if (wasFirstCommandByte) {
				// check for hidden meta content
				tryReadMetadata(&oldMsg);
			}
			break;
		}
		//I'm not sure what this does or why it does it...
		// skip all the gamestates until we reach needed
		if (readGamestate < thisDemo.currentNum) {
			//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
			if (cmd == svc_gamestate_general) {
				readGamestate++;
			}
			goto readNext;
		}


		SnapshotInfo* thisSnapshotInfo = NULL;

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
			return qfalse;
		case svc_nop_general:
			break;
		case svc_serverCommand_general:
			if (!demoCutParseCommandString(&oldMsg, &thisDemo.cut.Clc, demoType, SEHExceptionCaught)) {
				return qfalse;
			}
			break;
		case svc_gamestate_general:
			lastGameStateChange = thisDemo.cut.Cl.snap.serverTime;
			lastGameStateChangeInDemoTime = demoCurrentTime;
			//if (readGamestate > demo.currentNum) {
			//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
			//	goto cutcomplete;
			//}
			if (!demoCutParseGamestate(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, &demoType, (qboolean)(readGamestate==0), SEHExceptionCaught)) {
				Com_DPrintf("[NOTE] Demo cutter, parsing gamestate failed.\n");
				return qfalse;
			}
			updateConfigStringRelatedInfo();
			generateBasePlayerStates();
			//Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
			//newHandle = FS_FOpenFileWrite(newName);
			//if (!newHandle) {
			//	Com_Printf("Failed to open %s for target cutting.\n", newName);
			//	return qfalse;
			//}
			readGamestate++;
			break;
		case svc_snapshot_general:
			if (!demoCutParseSnapshot(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, demoType, SEHExceptionCaught,malformedMessageCaught,qtrue)) {
				Com_DPrintf("[NOTE] Demo cutter, parsing snapshot failed.\n");
				return qfalse;
			}

			{
				int possibleDimensionNum = (thisDemo.cut.Cl.snap.ps.fd.forcePowersActive >> 20) & 31;
				if (possibleDimensionNum) {
					if (!extraFieldInfo.dimensionInfoConfirmed && extraFieldInfo.ownageExtraInfoMetaMarker) {
						extraFieldInfo.dimensionInfoConfirmed = qtrue;
						extraFieldInfo.dimensionInfoType = DIM_OWNAGE;
						std::cout << "Demo appears to contain Ownage-City style dimension data (first seen in playerstate)! Report a bug if this is a mistake.\n";
					}
					if (extraFieldInfo.dimensionInfoConfirmed && extraFieldInfo.dimensionInfoType == DIM_OWNAGE) {
						thisDemo.cut.Cl.snap.ps.demoToolsData.detectedDimension = possibleDimensionNum;
						thisDemo.cut.Cl.snap.ps.demoToolsData.globalDimension = possibleDimensionNum == (((int)-999) & 31);
					}
				}
			}

			for (int i = 0; i < maxClientsThisDemo; i++) {
				mohaaPlayerWeaponModelIndexThisFrame[i] = -1;
			}

			thisSnapshotInfo = NULL;
			if (thisDemo.cut.Cl.lastSnapshotFinishedParsing) { // it can return true if the last snapshot was invalid too. but then we dont do this.
				SnapshotInfo snapshotInfo;
				snapshotInfo.serverTime = thisDemo.cut.Cl.snap.serverTime;
				for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
					entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

					if (extraFieldInfo.g_entHUDieldsValue) {
						const int g_entHUDFieldsExtraFields = (1 << ENTITYEXTRA_HEALTH) | (1 << ENTITYEXTRA_ARMOR) | (1 << ENTITYEXTRA_FORCE) | (1 << ENTITYEXTRA_SABERDRAWANIMLEVEL) | (1 << ENTITYEXTRA_CURRENTWEAPONAMMO) | (1 << ENTITYEXTRA_TRIPMINEAMMO) | (1 << ENTITYEXTRA_HASSENTRY) | (1 << ENTITYEXTRA_HASMEDPACK) | (1 << ENTITYEXTRA_HASFORCEFIELD) | (1 << ENTITYEXTRA_HASSEEKER);
						/*
							// trickedentindex3: armor (8 bits), health (8 bits)
							ent->client->ps.fd.forceMindtrickTargetIndex3 = ent->s.trickedentindex3 = ((MIN(127,MAX(-128,ent->client->ps.stats[STAT_HEALTH])) & 0xff) << 8) | (MIN(127, MAX(-128, ent->client->ps.stats[STAT_ARMOR])) & 0xff);
							// trickedentindex4: force power (7 bits), current weapon ammo (7 bits), saberdrawanimlevel (2 bits)
							ent->client->ps.fd.forceMindtrickTargetIndex4 = ent->s.trickedentindex4 = (ent->client->ps.fd.saberDrawAnimLevel & 3) << 14 | ((MAX(0, MIN(127, ent->client->ps.ammo[weaponData[ent->client->ps.weapon].ammoIndex])) & 127) << 7) | (MAX(0, MIN(127, ent->client->ps.fd.forcePower)) & 127);
							// generic1: seeker, forcefield, bacta, sentry in inventory (1 bit each), mine count (4 bits)
							ent->client->ps.generic1 = ent->s.generic1 = ((MAX(0, MIN(15, ent->client->ps.ammo[weaponData[WP_TRIP_MINE].ammoIndex])) & 15) << 4) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SENTRY_GUN))) << 3) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_MEDPAC))) << 2) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SHIELD))) << 1) | (!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SEEKER)));
						*/
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HEALTH] = (signed char)((thisEntity->trickedentindex3>>8) & 0xff);
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_ARMOR] = (signed char)((thisEntity->trickedentindex3) & 0xff);
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_FORCE] = thisEntity->trickedentindex4 & 127;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_SABERDRAWANIMLEVEL] = (thisEntity->trickedentindex4 >> 14) & 3;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_CURRENTWEAPONAMMO] = (thisEntity->trickedentindex4>>7) & 127;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_TRIPMINEAMMO] = (thisEntity->generic1 >>4) & 15;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HASSENTRY] = (thisEntity->generic1 >>3) & 1;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HASMEDPACK] = (thisEntity->generic1 >>2) & 1;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HASFORCEFIELD] = (thisEntity->generic1 >>1) & 1;
						thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HASSEEKER] = (thisEntity->generic1) & 1;
						thisEntity->demoToolsData.entityExtraValuesBitmask = g_entHUDFieldsExtraFields;
					}

					int possibleDimensionNum = (thisEntity->forcePowersActive >> 20) & 31;
					if (possibleDimensionNum) {
						if (!extraFieldInfo.dimensionInfoConfirmed && extraFieldInfo.ownageExtraInfoMetaMarker) {
							extraFieldInfo.dimensionInfoConfirmed = qtrue;
							extraFieldInfo.dimensionInfoType = DIM_OWNAGE;
							std::cout << "Demo appears to contain Ownage-City style dimension data (first seen in entity)! Report a bug if this is a mistake.\n";
						}
						if (extraFieldInfo.dimensionInfoConfirmed && extraFieldInfo.dimensionInfoType == DIM_OWNAGE) {
							thisEntity->demoToolsData.detectedDimension = possibleDimensionNum;
							thisEntity->demoToolsData.globalDimension = possibleDimensionNum == (((int)-999) & 31);
						}
					}
					thisEntity->demoToolsData.globalEvent = false;
					if (extraFieldInfo.dimensionInfoConfirmed && extraFieldInfo.dimensionInfoType == DIM_OWNAGE) {
						if (thisEntity->number >= maxClientsThisDemo && thisEntity->eType > getET_EVENTS(demoType)) {
							int generalEvent = generalizeGameValue<GMAP_EVENTS, UNSAFE>(thisEntity->eType - getET_EVENTS(demoType), demoType);
							thisEntity->demoToolsData.globalEvent =
								generalEvent == EV_BODY_QUEUE_COPY_GENERAL
								|| generalEvent == EV_CLIENTJOIN_GENERAL
								|| generalEvent == EV_DESTROY_WEAPON_MODEL_GENERAL
								//|| generalEvent == EV_GLOBAL_SOUND_GENERAL
								//|| generalEvent == EV_GLOBAL_TEAM_SOUND_GENERAL
								//|| generalEvent == EV_MUTE_SOUND_GENERAL
								|| generalEvent == EV_CTFMESSAGE_GENERAL
								|| generalEvent == EV_SAGA_OBJECTIVECOMPLETE_GENERAL
								|| generalEvent == EV_SAGA_ROUNDOVER_GENERAL
								|| generalEvent == EV_SCREENSHAKE_GENERAL
								|| generalEvent == EV_DESTROY_GHOUL2_INSTANCE_GENERAL
								|| generalEvent == EV_SET_FREE_SABER_GENERAL
								|| generalEvent == EV_SET_FORCE_DISABLE_GENERAL
								|| generalEvent == EV_GIVE_NEW_RANK_GENERAL
								|| generalEvent == EV_TESTLINE_GENERAL
								//|| generalEvent == EV_SCOREPLUM_GENERAL
								|| generalEvent == EV_OBITUARY_GENERAL;
						}
					}

					snapshotInfo.entities[thisEntity->number] = *thisEntity;

					lastMessageWithEntity[thisEntity->number] = thisDemo.cut.Cl.snap.messageNum;

					// See if we can figure out the command time of this entity if it's a player.
					if (thisEntity->number >= 0 && thisEntity->number < maxClientsThisDemo) {
						if (firstPacketWithPlayer[thisEntity->number] == -1) firstPacketWithPlayer[thisEntity->number] = thisDemo.cut.Cl.snap.messageNum;
						lastKnownPacketWithPlayer[thisEntity->number] = thisDemo.cut.Cl.snap.messageNum;
						if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
							snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = thisEntity->pos.trTime;
						}
						else {
							snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = thisDemo.cut.Cl.snap.serverTime; // Otherwise just use servertime. Lame but oh well. Maybe we could do sth better where we try to detect changes in values or such if we truly need to.
						}
						snapshotInfo.hasPlayer[thisEntity->number] = qtrue;
					}
					else if (isMOHAADemo && thisEntity->parent > 0 && thisEntity->parent < maxClientsThisDemo && generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(thisEntity->eType, demoType) == ET_ITEM_GENERAL && (thisEntity->eFlags & EF_SENTIENT_MOH)/* && && (thisEs->eFlags & EF_WEAPON_MOH)*/) { // Don't ask me why weapons have EF_SENTIENT and not EF_WEAPON...

					 //static int tagNumUsage[1000]{};
					 //tagNumUsage[thisEs->tag_num]++;
					 // Magic tag numbers. Seem to be the ones for main held weapon. 22 and 55. Used by far the most.
					 // There is others that happen to be used sometimes like 14 for snipers. I'm guessing that means they're strapped onto someone's back or sth because the snipers can also have 22/55
					 // General observed usage (with sample count as example): 
					 // 
					 // 3 (47), 5(400), 6(191), 14 (11555), 22 (347807), 23 (11), 29 (6024), 40 (1243), 52 (90908)
						if (thisEntity->tag_num == 52 || thisEntity->tag_num == 22) {

							int configStringBaseIndex = getCS_MODELS(demoType); //(demoType == DM_26 || demoType == DM_25) ? CS_MODELS_JKA : CS_MODELS;
							if (mohaaPlayerWeaponModelIndexThisFrame[thisEntity->parent] != -1) {
								int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + mohaaPlayerWeaponModelIndexThisFrame[thisEntity->parent]];
								char* modelName2 = thisDemo.cut.Cl.gameState.stringData + offset;
								offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + thisEntity->modelindex];
								char* modelName = thisDemo.cut.Cl.gameState.stringData + offset;
								std::cerr << "Weird. MOHAA weapon for player found twice. Duplicate? " << modelName << " vs. " << modelName2 << "\n";
							}
							if (thisEntity->modelindex != mohaaPlayerWeaponModelIndex[thisEntity->parent]) {
								// Model changed. Update weapons array.

								// Detecting weapons players are carrying
								int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + thisEntity->modelindex];
								//if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
								std::string weaponModel = thisDemo.cut.Cl.gameState.stringData + offset;
								auto foundModel = mohaaWeaponModelMap.find(weaponModel);
								if (foundModel != mohaaWeaponModelMap.end()) {
									mohaaPlayerWeapon[thisEntity->parent] = generalizeGameValue<GMAP_WEAPONS, UNSAFE>(foundModel->second, demoType);
								}
								else {
									mohaaPlayerWeapon[thisEntity->parent] = WP_NONE_GENERAL;
								}

							}
							mohaaPlayerWeaponModelIndexThisFrame[thisEntity->parent] = thisEntity->modelindex;
							mohaaPlayerWeaponModelIndex[thisEntity->parent] = thisEntity->modelindex;
						}
					}
				}
				Com_Memcpy(snapshotInfo.areamask, thisDemo.cut.Cl.snap.areamask, sizeof(thisDemo.cut.Cl.snap.areamask));
				Com_Memcpy(snapshotInfo.mohaaPlayerWeapon, mohaaPlayerWeapon, sizeof(mohaaPlayerWeapon));
				snapshotInfo.snapNum = thisDemo.cut.Cl.snap.messageNum;
				snapshotInfo.playerState = thisDemo.cut.Cl.snap.ps;
				snapshotInfo.playerStateTeleport = PlayerStateIsTeleport(&thisDemo.cut.Cl.oldSnap, &thisDemo.cut.Cl.snap);
				snapshotInfo.snapFlagServerCount = (qboolean)((thisDemo.cut.Cl.oldSnap.snapFlags ^ thisDemo.cut.Cl.snap.snapFlags) & SNAPFLAG_SERVERCOUNT);
				snapshotInfo.playerCommandOrServerTimes[thisDemo.cut.Cl.snap.ps.clientNum] = lastKnownCommandOrServerTimes[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.ps.commandTime;
				if (firstPacketWithPlayer[thisDemo.cut.Cl.snap.ps.clientNum] == -1) firstPacketWithPlayer[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.messageNum;
				lastKnownPacketWithPlayer[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.messageNum;
				if (firstPacketWithPlayerState[thisDemo.cut.Cl.snap.ps.clientNum] == -1) firstPacketWithPlayerState[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.messageNum;
				lastKnownPacketWithPlayerState[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.messageNum;
				snapshotInfo.hasPlayer[thisDemo.cut.Cl.snap.ps.clientNum] = qtrue;
				thisSnapshotInfo = &(snapshotInfos[thisDemo.cut.Cl.snap.messageNum] = snapshotInfo);
				anySnapshotParsed = qtrue;// Fix? Well this makes more sense in any case.
			}


			// Time related stuff
			if (messageOffset++ == 0) {
				// first message in demo. Get servertime offset from here to cut correctly.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}
			if (thisDemo.cut.Clc.serverMessageSequence == maxSequenceNum) { // See demoHighlightFind.cpp for detailed commentary
				if (thisDemo.cut.Cl.snap.serverTime < lastKnownInOrderTime) {

					if (thisDemo.cut.Cl.snap.serverTime > 10000) {
						// This is a non-critical warning, mostly for debugging. It used to be more dangerous.
						std::cerr << "thisDemo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Clc.serverMessageSequence == maxSequenceNum but demo.cut.Cl.snap.serverTime > 10000;  delta " << (lastKnownTime - demo.cut.Cl.snap.serverTime) << ", demoCurrentTime " << demoCurrentTime << ", demoBaseTime " << demoBaseTime << ", demoStartTime " << demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", lastKnownTime " << lastKnownTime << " (" << originalDemoPath << ")\n";
					}

					demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
					demoStartTime = thisDemo.cut.Cl.snap.serverTime;
				}
				lastKnownInOrderTime = thisDemo.cut.Cl.snap.serverTime;
			}
			/*if (thisDemo.cut.Cl.snap.serverTime < lastKnownTime  && thisDemo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
				if (demo.cut.Cl.snap.serverTime > 10000) {
					std::cerr << "demo.cut.Cl.snap.serverTime < lastKnownTime but demo.cut.Cl.snap.serverTime > 10000; demoCurrentTime " << demoCurrentTime << ", demoBaseTime " << demoBaseTime << ", demoStartTime " << demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", lastKnownTime " << lastKnownTime << " (" << DPrintFLocation << ")\n";
				}
				demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}*/
			demoCurrentTime = demoBaseTime + thisDemo.cut.Cl.snap.serverTime - demoStartTime;
			lastKnownTime = thisDemo.cut.Cl.snap.serverTime;

			if (thisSnapshotInfo) {
				thisSnapshotInfo->demoTime = demoCurrentTime;
			}

			trackMetaEventsTiming();

			// Fire playerstate events
			if (thisDemo.cut.Cl.snap.ps.externalEvent && thisDemo.cut.Cl.snap.ps.externalEvent != oldPS[thisDemo.cut.Cl.snap.ps.clientNum].externalEvent) {
				
				Event thisEvent;
#ifdef DEBUG
				thisEvent.countGiven = 0;
#endif
				thisEvent.demoTime = demoCurrentTime;
				thisEvent.serverTime = thisDemo.cut.Cl.snap.serverTime;
				thisEvent.kind = EK_PS_EXTERNAL;
				
				memset(&thisEvent.theEvent,0,sizeof(thisEvent.theEvent));

				thisEvent.theEvent.number = thisDemo.cut.Cl.snap.ps.clientNum;
				thisEvent.theEvent.event = thisDemo.cut.Cl.snap.ps.externalEvent;
				thisEvent.theEvent.eventParm = thisDemo.cut.Cl.snap.ps.externalEventParm;

				thisEvent.eventNumber = thisEvent.theEvent.event & ~EV_EVENT_BITS;

				if (demoType == DM_14) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
					thisEvent.eventNumber = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.eventNumber,DM_14,DM_15);
					thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.theEvent.event, DM_14, DM_15);
				}else if (demoType == DM_26 || demoType == DM_25) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
					thisEvent.eventNumber = jkaEventToJk2Map[thisEvent.eventNumber];
					thisEvent.theEvent.event = MapJKAEventJK2(thisEvent.theEvent.event);
				} else if (demoType == DM_68) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
					thisEvent.eventNumber = q3dm68EventToJk2Map[thisEvent.eventNumber];
					thisEvent.theEvent.event = MapQ3DM68EventJK2(thisEvent.theEvent.event);
				}
				readEvents.push_back(thisEvent);

			}

			// Fire eventSequence events
			for (int i = thisDemo.cut.Cl.snap.ps.eventSequence - MAX_PS_EVENTS; i < thisDemo.cut.Cl.snap.ps.eventSequence; i++) {
				// if we have a new predictable event
				if (i >= oldPS[thisDemo.cut.Cl.snap.ps.clientNum].eventSequence
					// or the server told us to play another event instead of a predicted event we already issued
					// or something the server told us changed our prediction causing a different event
					|| (i > oldPS[thisDemo.cut.Cl.snap.ps.clientNum].eventSequence - MAX_PS_EVENTS && thisDemo.cut.Cl.snap.ps.events[i & (MAX_PS_EVENTS - 1)] != oldPS[thisDemo.cut.Cl.snap.ps.clientNum].events[i & (MAX_PS_EVENTS - 1)])) {


					Event thisEvent;
#ifdef DEBUG
					thisEvent.countGiven = 0;
#endif
					thisEvent.demoTime = demoCurrentTime;
					thisEvent.serverTime = thisDemo.cut.Cl.snap.serverTime;
					thisEvent.kind = EK_PS_ARRAY;

					memset(&thisEvent.theEvent, 0, sizeof(thisEvent.theEvent));

					thisEvent.theEvent.number = thisDemo.cut.Cl.snap.ps.clientNum;
					thisEvent.theEvent.event = thisDemo.cut.Cl.snap.ps.events[i & (MAX_PS_EVENTS - 1)] | ((i & 3) << 8); // I hope this is right, using i there....
					thisEvent.theEvent.eventParm = thisDemo.cut.Cl.snap.ps.eventParms[i & (MAX_PS_EVENTS - 1)];

					thisEvent.eventNumber = thisEvent.theEvent.event & ~EV_EVENT_BITS;
					if (demoType == DM_14) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.eventNumber, DM_14, DM_15);
						thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.theEvent.event, DM_14, DM_15);
					}
					else if (demoType == DM_26 || demoType == DM_25) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = jkaEventToJk2Map[thisEvent.eventNumber];
						thisEvent.theEvent.event = MapJKAEventJK2(thisEvent.theEvent.event);
					}
					else if (demoType == DM_68) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = q3dm68EventToJk2Map[thisEvent.eventNumber];
						thisEvent.theEvent.event = MapQ3DM68EventJK2(thisEvent.theEvent.event);
					}
					readEvents.push_back(thisEvent);


				}
			}

			// Fire entity events
			for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {

				entityState_t* thisEs = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
				int eventNumber = GetEvent(thisEs);

				if (eventNumber) {

					Event thisEvent;
#ifdef DEBUG
					thisEvent.countGiven = 0;
#endif
					thisEvent.kind = EK_ENTITY;
					thisEvent.demoTime = demoCurrentTime;
					thisEvent.serverTime = thisDemo.cut.Cl.snap.serverTime;
					thisEvent.theEvent = *thisEs;
					thisEvent.eventNumber = eventNumber;
					if (demoType == DM_14) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.eventNumber, DM_14, DM_15);
						if (thisEvent.theEvent.eType > getET_EVENTS(demoType)) {
							thisEvent.theEvent.eType = jkaEventToJk2Map[thisEvent.theEvent.eType- getET_EVENTS(demoType)] + ET_EVENTS_JK2; // I just changed this, but should I? Hmm
						}
						else {
							thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS, UNSAFE>(thisEvent.theEvent.event, DM_14, DM_15);
						}
					}
					else if (demoType == DM_26 || demoType == DM_25) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = jkaEventToJk2Map[thisEvent.eventNumber];
						if (thisEvent.theEvent.eType > ET_EVENTS_JKA) {
							thisEvent.theEvent.eType = jkaEventToJk2Map[thisEvent.theEvent.eType-ET_EVENTS_JKA] + ET_EVENTS_JK2; // I just changed this, but should I? Hmm
						}
						else {
							thisEvent.theEvent.event = MapJKAEventJK2(thisEvent.theEvent.event);
						}
					}
					else if (demoType == DM_68) {
						thisEvent.eventNumber = q3dm68EventToJk2Map[thisEvent.eventNumber];
						if (thisEvent.theEvent.eType > ET_EVENTS_Q3) {
							thisEvent.theEvent.eType = q3dm68EventToJk2Map[thisEvent.theEvent.eType - ET_EVENTS_Q3] + ET_EVENTS_JK2;
						}
						else {
							thisEvent.theEvent.event = MapQ3DM68EventJK2(thisEvent.theEvent.event);
						}
					}
					readEvents.push_back(thisEvent);
					
				}
			}


			oldPS[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.ps;

			// Find out which players are visible / followed
			// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
			/*lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
			for (int p = 0; p < maxClientsThisDemo; p++) {
				// Go through parseenttities of last snap to see if client is in it
				bool clientIsInSnapshot = false;
				bool clientVisibleOrFollowed = false;
				for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
					entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					if (thisEntity->number == p) {
						clientIsInSnapshot = true;

						if (thisEntity->powerups & (1 << PW_REDFLAG)) {
							lastKnownRedFlagCarrier = thisEntity->number;
						}
						else if (thisEntity->powerups & (1 << PW_BLUEFLAG)) {
							lastKnownBlueFlagCarrier = thisEntity->number;
						}
					}
				}
				if (clientIsInSnapshot) {
					clientVisibleOrFollowed = true;
					if (playerFirstVisible[p] == -1) {
						playerFirstVisible[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstVisible[p] = -1;
				}
				if (thisDemo.cut.Cl.snap.ps.clientNum == p) {
					clientVisibleOrFollowed = true;
					if (playerFirstFollowed[p] == -1) {
						playerFirstFollowed[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstFollowed[p] = -1;
				}
				if (clientVisibleOrFollowed) {
					if (playerFirstFollowedOrVisible[p] == -1) {
						playerFirstFollowedOrVisible[p] = thisDemo.cut.Cl.snap.serverTime;
					}
				}
				else {
					playerFirstFollowedOrVisible[p] = -1;
				}
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
	int firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
	qboolean hadConfigStringChanges = qfalse;
	// process any new server commands
	for (; thisDemo.cut.Clc.lastExecutedServerCommand <= thisDemo.cut.Clc.serverCommandSequence; thisDemo.cut.Clc.lastExecutedServerCommand++) {
		char* command = thisDemo.cut.Clc.serverCommands[thisDemo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
		
		Cmd_TokenizeString(command);
		char* cmd = Cmd_Argv(0);
		//if (cmd[0] && !firstServerCommand) {
		//	firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
		//}
		//else {
		if(!cmd){
			continue;
		}

		Command readCommand;
		readCommand.command = command;
		readCommand.demoTime = demoCurrentTime;
		readCommand.serverTime = thisDemo.cut.Cl.snap.serverTime;
		readCommands.push_back(readCommand);

		if (!strcmp(cmd, "cs")) {
			if (!demoCutConfigstringModified(&thisDemo.cut.Cl,demoType)) {
				Com_DPrintf("[NOTE] Demo cutter, configstring parsing failed.\n");
				return qfalse;
			}
			hadConfigStringChanges = qtrue;
		}
		if (!strcmp(cmd, "print")) {
			//Looking for 
			//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."
			// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.

			/*jp::VecNum vec_num;
			jp::RegexMatch rm;
			std::string printText = Cmd_Argv(1);
#if DEBUG
			std::cout << printText << "\n";
#endif

			size_t count = rm.setRegexObject(&defragRecordFinishRegex)                          //set associated Regex object
				.setSubject(&printText)                         //set subject string
				.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
				.match();

			for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
				const char* info = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
				std::string mapname = Info_ValueForKey(info, "mapname");
				std::string playername = vec_num[matchNum][1];
				int minutes = atoi(vec_num[matchNum][2].c_str());
				std::string secondString = vec_num[matchNum][3];
				float seconds = atof(vec_num[matchNum][3].c_str());
				int milliSeconds = (1000.0f * seconds) + 0.5f;
				int pureMilliseconds = milliSeconds % 1000;
				int pureSeconds = milliSeconds / 1000;

				bool isLogged = vec_num[matchNum][4].length() > 0;
				bool isNumberOne = vec_num[matchNum][6].length() > 0;

				//int totalSeconds = minutes * 60 + seconds;
				int totalMilliSeconds = minutes * 60000 + milliSeconds;

				// Find player
				int playerNumber = -1;
				for (int clientNum = 0; clientNum < maxClientsThisDemo; clientNum++) {

					const char* playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS + clientNum];
					std::string playerNameCompare = Info_ValueForKey(playerInfo, "n");
					if (playerNameCompare == playername) {
						playerNumber = clientNum;
					}
				}

				bool wasFollowed = false;
				bool wasVisible = false;
				bool wasVisibleOrFollowed = false;
				if (playerNumber != -1) {
					if (playerFirstFollowed[playerNumber] != -1 && playerFirstFollowed[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasFollowed = true;
					}
					if (playerFirstVisible[playerNumber] != -1 && playerFirstVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasVisible = true;
					}
					if (playerFirstFollowedOrVisible[playerNumber] != -1 && playerFirstFollowedOrVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
						wasVisibleOrFollowed = true;
					}
				}
				int runStart = demoCurrentTime - totalMilliSeconds;
			}*/



		}
	}

	if (hadConfigStringChanges) {
		generateBasePlayerStates();
		updateConfigStringRelatedInfo();
	}

/*#if DEBUG
	if (oldSize == 0) {
		goto cutcomplete;
	}
#endif*/
	return qtrue;
	
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

/*
int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds";
		std::cin.get();
		return 1;
	}
	initializeGameInfos();

	char* demoName = argv[1];
	float bufferTime = atof(argv[2]);

	highlightSearchMode_t searchMode = SEARCH_INTERESTING;
	if (argc > 3) {
		// Searchmode specified
		char* searchModeText = argv[3];
		if (!_stricmp(searchModeText, "myctfreturns")) {
			searchMode = SEARCH_MY_CTF_RETURNS;
		}
		else if (!_stricmp(searchModeText, "ctfreturns")) {
			searchMode = SEARCH_CTF_RETURNS;
		}
		else if (!_stricmp(searchModeText, "top10defrag")) {
			searchMode = SEARCH_TOP10_DEFRAG;
		}
	}

	if (demoHighlightFind(demoName, bufferTime, "highlightExtractionScript.bat", searchMode)) {
		Com_Printf("Highlights successfully found.\n", demoName);
	}
	else {
		Com_Printf("Finding highlights in demo %s has resulted in errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}*/


void remapConfigStrings(int eventNumber, entityState_t* tmpEntity, clientActive_t* clCut, DemoReader* reader, std::vector<std::string>* commandsToAdd, qboolean doModelIndex, qboolean doModelIndex2,demoType_t demoType) {
	int eventHere = generalizeGameValue<GMAP_EVENTS, UNSAFE>(eventNumber,demoType);
	int maxLength = 0;
	if (eventHere == EV_GENERAL_SOUND_GENERAL) {
		int soundIndex = tmpEntity->eventParm;
		const char* soundName = reader->GetSoundConfigString(soundIndex, &maxLength);
		int newSoundIndex = G_SoundIndex((char*)soundName, clCut, commandsToAdd, demoType);
		tmpEntity->eventParm = newSoundIndex;
	}
	if (doModelIndex && tmpEntity->modelindex) {
		const char* modelName = reader->GetModelConfigString(tmpEntity->modelindex, &maxLength);
		int newModelIndex = G_ModelIndex((char*)modelName, clCut, commandsToAdd, demoType);
		tmpEntity->modelindex = newModelIndex;
	}
	if (doModelIndex2 && tmpEntity->modelindex2) {
		const char* modelName = reader->GetModelConfigString(tmpEntity->modelindex2, &maxLength);
		int newModelIndex = G_ModelIndex((char*)modelName, clCut, commandsToAdd, demoType);
		tmpEntity->modelindex2 = newModelIndex;
	}
}

void remapConfigStrings(entityState_t* tmpEntity, clientActive_t* clCut, DemoReader* reader, std::vector<std::string>* commandsToAdd, qboolean doModelIndex, qboolean doModelIndex2, demoType_t demoType) {
	/*int eventHere = generalizeGameValue<GMAP_EVENTS, UNSAFE>(tmpEntity->event & ~EV_EVENT_BITS,demoType);
	int maxLength = 0;
	if (eventHere == EV_GENERAL_SOUND_GENERAL) {
		int soundIndex = tmpEntity->eventParm;
		const char* soundName = reader->GetSoundConfigString(soundIndex, &maxLength);
		int newSoundIndex = G_SoundIndex((char*)soundName, clCut, commandsToAdd, demoType);
		tmpEntity->eventParm = newSoundIndex;
	}
	if (doModelIndex && tmpEntity->modelindex) {
		const char* modelName = reader->GetModelConfigString(tmpEntity->modelindex, &maxLength);
		int newModelIndex = G_ModelIndex((char*)modelName, clCut, commandsToAdd, demoType);
		tmpEntity->modelindex = newModelIndex;
	}
	if (doModelIndex2 && tmpEntity->modelindex2) {
		const char* modelName = reader->GetModelConfigString(tmpEntity->modelindex2, &maxLength);
		int newModelIndex = G_ModelIndex((char*)modelName, clCut, commandsToAdd, demoType);
		tmpEntity->modelindex2 = newModelIndex;
	}*/
	remapConfigStrings(tmpEntity->event & ~EV_EVENT_BITS, tmpEntity, clCut, reader, commandsToAdd, doModelIndex, doModelIndex2, demoType);
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif