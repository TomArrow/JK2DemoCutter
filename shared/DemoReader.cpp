#include "demoCut.h"
#include "anims.h"
#include "jkaStuff.h"
#include "otherGameStuff.h"
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
int DemoReader::getMaxClients() {
	return maxClientsThisDemo;
}

int DemoReader::getClientNumForDemo(std::string* playerSearchString, qboolean printEndLine) {
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
		for (int c = 0; c < maxClientsThisDemo; c++) {
			tmpConfigString = reader->GetPlayerConfigString(c, &tmpConfigStringMaxLength);
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
	if (lastSnap && ((snap->ps.eFlags ^ lastSnap->ps.eFlags) & EF_TELEPORT_BIT)) {
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


	strncpy_s(oldName, sizeof(oldName), sourceDemoFile, strlen(sourceDemoFile) - 6);

	demoCutGetDemoType(sourceDemoFile, ext,&demoType,&isCompressedFile,&thisDemo.cut.Clc.demoCheckFor103);

	maxClientsThisDemo = getMAX_CLIENTS(demoType);
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
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}
	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&thisDemo, 0, sizeof(thisDemo));
	memset(basePlayerStates, 0, sizeof(basePlayerStates));
	memset(&oldPS, 0, sizeof(oldPS));
	memset(lastGottenEventsTime, 0, sizeof(lastGottenEventsTime));
	memset(lastGottenEventsServerTime, 0, sizeof(lastGottenEventsServerTime));

	readGamestate = 0;

	demoStartTime = 0;
	demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	demoCurrentTime = 0;
	lastGameStateChange = 0;
	lastGameStateChangeInDemoTime = 0;
	lastKnownTime = 0;
	thisDemo.cut.Clc.lastKnownCommandTime = 0;
	thisDemo.cut.Clc.firstSnapServerTime = -1;
	lastKnownCommandOrServerTimes.clear();
	messageOffset = 0;
	lastGottenCommandsTime = 0;
	lastGottenCommandsServerTime = 0;
	endReached = qfalse;

	snapshotInfos.clear();


	//while (oldSize > 0) ReadMessage();

	ReadMessage(); // Will only read pretty much the first message so we have gamestate and are somewhat "initialized" i guess
}

qboolean DemoReader::CloseDemo() {

	FS_FCloseFile(oldHandle);
	return qtrue;
}
qboolean DemoReader::AnySnapshotParsed() {

	return anySnapshotParsed;
}
qboolean DemoReader::EndReached() {

	return endReached;
}
qboolean DemoReader::EndReachedAtTime(double time) {
	SeekToTime(time);
	return (qboolean)(demoCurrentTime < time);
}
qboolean DemoReader::EndReachedAtServerTime(int serverTime) {
	SeekToServerTime(serverTime);
	return (qboolean)(lastKnownTime < serverTime);
}
int DemoReader::getCurrentDemoTime() {
	return demoCurrentTime;
}

qboolean DemoReader::SeekToTime(double time) {
	while (demoCurrentTime < time && !endReached) {
		ReadMessage();
	}
	if (demoCurrentTime < time && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToServerTime(int serverTime) {
	while (lastKnownTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime < serverTime && endReached) return qfalse;
	return qtrue;
}
int DemoReader::GetFirstServerTimeAfterServerTime(int serverTime) {
	while (lastKnownTime <= serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownTime <= serverTime && endReached) return -1;

	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime > serverTime) return it->second.serverTime;
	}
	return -1; // Shouldn't happen really but whatever
}
int DemoReader::GetLastServerTimeBeforeServerTime(int serverTime) {

	if (!SeekToServerTime(serverTime)) return -1;

	int lastServerTime = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime >= serverTime) return lastServerTime;
		lastServerTime = it->second.serverTime;
	}
	return -1; // Shouldn't happen really but whatever
}
SnapshotInfo* DemoReader::GetSnapshotInfoAtServerTime(int serverTime) {
	if (SeekToServerTime(serverTime)) {
		for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
			if (it->second.serverTime == serverTime) return &it->second;
		}
		return NULL; // Can happen if that particular time's snapshot is missing from the demo.
	}
	else {
		return NULL;
	}

}
qboolean DemoReader::SeekToCommandTime(int serverTime) {
	while (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && !endReached) {
		ReadMessage();
	}
	if (thisDemo.cut.Clc.lastKnownCommandTime < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToPlayerCommandOrServerTime(int clientNum,int serverTime) {
	while (lastKnownCommandOrServerTimes[clientNum] < serverTime && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] < serverTime && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToPlayerInPacket(int clientNum) {
	while (lastKnownCommandOrServerTimes[clientNum] <=0 && !endReached) {
		ReadMessage();
	}
	if (lastKnownCommandOrServerTimes[clientNum] <= 0 && endReached) return qfalse;
	return qtrue;
}
qboolean DemoReader::SeekToAnySnapshotIfNotYet() {
	while (!anySnapshotParsed && !endReached) {
		ReadMessage();
	}
	if (!anySnapshotParsed && endReached) return qfalse;
	return qtrue;
}
playerState_t DemoReader::GetCurrentPlayerState() {
	return thisDemo.cut.Cl.snap.ps;
}

playerState_t DemoReader::GetPlayerFromSnapshot(int clientNum, int snapNum, qboolean detailedPS) {
	SnapshotInfo* snap = &snapshotInfos[snapNum];
	if (snap->playerState.clientNum == clientNum) {
		return snap->playerState;
	}
	else {
		playerState_t retVal;
		Com_Memset(&retVal, 0, sizeof(playerState_t));

		int baseSnap = -1;
		if(detailedPS){ // Try to restore stuff like health/armor, pers_spawn_count etc. Use for main player in demo.
			int pastSnap = -1;
			int futureSnap = -1;
			// Search if there is a past and/or future playerstate.
			// TODO What about future ones not read yet? It might happen minutes later or whatever...
			SeekToServerTime(snap->serverTime+ PLAYERSTATE_FUTURE_SEEK);
			for (auto snapIt = snapshotInfos.begin(); snapIt != snapshotInfos.end(); snapIt++) {
				if (snapIt->first < snapNum && snapIt->second.playerState.clientNum == clientNum) {
					pastSnap = snapIt->first;
				}
				if (snapIt->first > snapNum && snapIt->second.playerState.clientNum == clientNum) {
					futureSnap = snapIt->first;
				}
			}
			baseSnap = pastSnap != -1 ? pastSnap : futureSnap;
		}

		entityState_t* thisEntity = &snap->entities[clientNum];
		// Need to convert the entity.
		CG_EntityStateToPlayerState(thisEntity, &retVal,demoType,qtrue,baseSnap != -1 ? &snapshotInfos[baseSnap].playerState : &basePlayerStates[clientNum]);

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
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		//if (it->second.serverTime <= time) {
		
		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.
		
		if (firstPacketWithPlayerInIt == -1) firstPacketWithPlayerInIt = it->first;
		
		if (it->second.playerCommandOrServerTimes[clientNum] <= time) {
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
			//return GetPlayerFromSnapshot(clientNum,&snapshotInfos[firstPacketWithPlayerInIt]);
			return GetPlayerFromSnapshot(clientNum,firstPacketWithPlayerInIt, detailedPS);
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
			return GetPlayerFromSnapshot(clientNum, lastMessageWithEntity[clientNum], detailedPS);
		}
	}

	// Okay now we want to locate the first snap with a different commandtime than lastPastSnap and then interpolate between the two.
	int firstNextSnap = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {

		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.

		if (it->second.playerCommandOrServerTimes[clientNum] > lastPastSnapCommandTime) {
			firstNextSnap = it->first;
			break;
		}
	}

	if (oldSnap) *oldSnap = &snapshotInfos[lastPastSnap];
	if (newSnap) *newSnap = &snapshotInfos[firstNextSnap];

	// Okay now we know the messageNum of before and after. Let's interpolate! How exciting!
	InterpolatePlayer(clientNum,time, &snapshotInfos[lastPastSnap], &snapshotInfos[firstNextSnap], &retVal, detailedPS);

	return retVal;
	
}

// It's like a cheaper version of GetInterpolatedPlayer. We don't interpolate anything. We just return the last player state or the next one if no last one exists.
// It ignores command times and is based purely on server time.
// arguments oldSnap and newSnap do nothing atm.
playerState_t DemoReader::GetLastOrNextPlayer(int clientNum, int serverTime, SnapshotInfo** oldSnap, SnapshotInfo** newSnap, qboolean detailedPS) {
	playerState_t retVal;
	Com_Memset(&retVal, 0, sizeof(playerState_t));

	if (oldSnap) *oldSnap = NULL;
	if (newSnap) *newSnap = NULL;

	SeekToAnySnapshotIfNotYet();
	SeekToServerTime(serverTime);

	SeekToPlayerInPacket(clientNum);

	if (endReached && !anySnapshotParsed) return retVal; // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	int lastPastSnap = -1;
	int lastPastSnapServerTime = -1;
	int firstPacketWithPlayerInIt = -1;
	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		
		if (it->second.playerCommandOrServerTimes.find(clientNum) == it->second.playerCommandOrServerTimes.end()) continue; // This snapshot doesn't have this player. Don't access the player's number in the map or the map will generate a useless value.
		
		if (firstPacketWithPlayerInIt == -1) firstPacketWithPlayerInIt = it->first;
		
		if (it->second.serverTime <= serverTime) {
			lastPastSnap = it->first;
			lastPastSnapServerTime = it->second.serverTime;
		}
	}
	if (lastPastSnap == -1) { // Might be beginning of the demo, nothing in the past yet. Let's just take the first packet we have with the player in it
		if (firstPacketWithPlayerInIt == -1) {
			// Uhm. Ok. Maybe handle this better at some later time but for now we just return that empty playerState.
			// We should probably keep seeking then or sth.
			return retVal;
		}
		else {
			return GetPlayerFromSnapshot(clientNum,firstPacketWithPlayerInIt, detailedPS);
			
		}
	}
	else {
		return GetPlayerFromSnapshot(clientNum, lastPastSnap, detailedPS);
	}

	
}

void DemoReader::mapAnimsToDM15(playerState_t* ps) {
	if (demoType == DM_14) {

		ps->torsoAnim = convertGameValue<GMAP_ANIMATIONS>(ps->torsoAnim, demoType, DM_16);
		ps->legsAnim = convertGameValue<GMAP_ANIMATIONS>(ps->legsAnim, demoType, DM_16);
		ps->weapon = convertGameValue<GMAP_WEAPONS>(ps->weapon,demoType,DM_15);
	} else if (demoType == DM_26 || demoType == DM_25) {

		//ps->torsoAnim = jkaAnimMapping[ps->torsoAnim];
		ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->torsoAnim, demoType, DM_16);
		if (ps->torsoFlip) ps->torsoAnim |= ANIM_TOGGLEBIT; // Generalize togglebit someday?
		//ps->legsAnim = jkaAnimMapping[ps->legsAnim];
		ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->legsAnim, demoType, DM_16);
		if (ps->legsFlip) ps->legsAnim |= ANIM_TOGGLEBIT;// Generalize togglebit someday?
		//ps->weapon = jkaWeaponMap[ps->weapon];
		ps->weapon = specializedGameValueMapUnsafe<GMAP_WEAPONS>(ps->weapon,demoType,DM_15);
	} else if (demoType == DM_68) { // TODO Allow other ones too? But idk if anims changed

		//ps->torsoAnim = MapQ3AnimToJK2(ps->torsoAnim);
		ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->torsoAnim, demoType, DM_16);
		//ps->legsAnim = MapQ3AnimToJK2(ps->legsAnim);
		ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->legsAnim, demoType, DM_16);
		//ps->weapon = q3WeaponMap[ps->weapon];
		ps->weapon = specializedGameValueMapUnsafe<GMAP_WEAPONS>(ps->weapon, demoType, DM_15);;
		ps->genericEnemyIndex = -1; // Don't draw seeker drone pls.
	}
	if (demoType == DM_14 || demoType == DM_16 || demoType == DM_26 || demoType == DM_25 || demoType == DM_68) { // TODO: Do all this more elegeantly? Please?

		//ps->torsoAnim = animMappingTable_1_04_to_1_02[ps->torsoAnim];
		//ps->legsAnim = animMappingTable_1_04_to_1_02[ps->legsAnim];
		//ps->torsoAnim = MV_MapAnimation102(ps->torsoAnim);
		ps->torsoAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->torsoAnim, DM_16, DM_15);
		//ps->legsAnim = MV_MapAnimation102(ps->legsAnim);
		ps->legsAnim = specializedGameValueMapUnsafe<GMAP_ANIMATIONS>(ps->legsAnim, DM_16, DM_15);
	}

	if (demoType < DM_15 || demoType > DM_16) {
		ps->events[0] = convertGameValue<GMAP_EVENTS>(ps->events[0], demoType, DM_15);
		ps->events[1] = convertGameValue<GMAP_EVENTS>(ps->events[1], demoType, DM_15);
		ps->externalEvent = convertGameValue<GMAP_EVENTS>(ps->externalEvent, demoType, DM_15);
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


void DemoReader::InterpolatePlayerState(float time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS) {
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

void DemoReader::InterpolatePlayer(int clientNum, float time,SnapshotInfo* from, SnapshotInfo* to, playerState_t* outPS, qboolean detailedPS) {
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
		inPsConverted = GetPlayerFromSnapshot(clientNum,from->snapNum, detailedPS);
		curps = &inPsConverted;
		currentTime = curps->commandTime;
		//currentTime = from->serverTime;
		currentServerTime = from->serverTime;
		if (to) {
			outPsConverted = GetPlayerFromSnapshot(clientNum, to->snapNum, detailedPS);
			nextps = &outPsConverted;
			nextTime = nextps->commandTime;
			//nextTime = to->serverTime; // Testing. See if we can find the perfect way to do it ... hmm
			nextServerTime = to->serverTime;
		}
	}
	if (!to) {
		return;
	}
	
	nextPsTeleport = (qboolean)( to->snapFlagServerCount || ((curps->eFlags ^ nextps->eFlags) & EF_TELEPORT_BIT));
	
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





std::map<int,entityState_t> DemoReader::GetCurrentEntities() {
	std::map<int, entityState_t> retVal;
	for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
		entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		retVal[thisEntity->number] = *thisEntity;
	}
	return retVal;
}

std::map<int,entityState_t> DemoReader::GetEntitiesAtTime(double time, double * translatedTime = NULL) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time);

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

	
	return snapshotInfos[lastPastSnap].entities;
}

std::map<int,entityState_t> DemoReader::GetEntitiesAtPreciseTime(int time, qboolean includingPS) { // Can't use currentEntities one really because we might have seeked past the current time already for some interpolation reasons

	SeekToAnySnapshotIfNotYet();
	SeekToTime(time+0.5);

	// Now let's translate time into server time
	time = time - demoBaseTime + demoStartTime;

	if (endReached && !anySnapshotParsed) return std::map<int, entityState_t>(); // Nothing to do really lol.

	// Ok now we are sure we have at least one snapshot. Good.
	// Now we wanna make sure we have a snapshot in the future with a different commandtime than the one before "time".

	for (auto it = snapshotInfos.begin(); it != snapshotInfos.end(); it++) {
		if (it->second.serverTime == time) {
			if (includingPS) {

				std::map<int, entityState_t> retVal = it->second.entities;
				entityState_t psEnt;
				Com_Memset(&psEnt, 0, sizeof(psEnt));
				BG_PlayerStateToEntityState(&it->second.playerState, &psEnt,qfalse,qtrue);
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
std::vector<Event> DemoReader::GetNewEvents(double time, eventKind_t kind) {
	std::vector<Event> retVal;
	SeekToTime(time);
	for (int i = 0; i < readEvents.size(); i++) {
		if (readEvents[i].demoTime <= time && readEvents[i].demoTime > lastGottenEventsTime[kind] && (kind == EK_ALL || readEvents[i].kind == kind)) {
#ifdef DEBUG
			readEvents[i].countGiven++;
#endif
			retVal.push_back(readEvents[i]);
		}
	}
	lastGottenEventsTime[kind] = time;
	return retVal;
}

std::vector<Event> DemoReader::GetNewEventsAtServerTime(int serverTime, eventKind_t kind) {
	std::vector<Event> retVal;
	SeekToServerTime(serverTime);
	for (int i = 0; i < readEvents.size(); i++) {
		if (readEvents[i].serverTime <= serverTime && readEvents[i].serverTime > lastGottenEventsServerTime[kind] && (kind == EK_ALL || readEvents[i].kind == kind)) {
#ifdef DEBUG
			readEvents[i].countGiven++;
#endif
			retVal.push_back(readEvents[i]);
		}
	}
	lastGottenEventsServerTime[kind] = serverTime;
	return retVal;
}

int DemoReader::GetFirstSnapServerTime() {
	SeekToAnySnapshotIfNotYet();
	return thisDemo.cut.Clc.firstSnapServerTime;
}

clSnapshot_t DemoReader::GetCurrentSnap() {
	return thisDemo.cut.Cl.snap;
}

const char* DemoReader::GetPlayerConfigString(int playerNum,int* maxLength) {
	int configStringBaseIndex = getCS_PLAYERS(demoType); // (demoType == DM_26 || demoType == DM_25) ? CS_PLAYERS_JKA : CS_PLAYERS;
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + playerNum];
	if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
	return thisDemo.cut.Cl.gameState.stringData + offset;
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
const char* DemoReader::GetConfigString(int configStringNum, int* maxLength) {
	int offset = thisDemo.cut.Cl.gameState.stringOffsets[configStringNum];
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
		basePlayerStates[i].stats[STAT_MAX_HEALTH] = 100;
	}
}

qboolean DemoReader::ReadMessage() {
	if (endReached) return qfalse;
	qboolean realReadResult = qfalse;
	realReadResult = ReadMessageReal();
	if (!realReadResult) {
		endReached = qtrue;
		return qfalse;
	}
	return qtrue;
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

	if (isCompressedFile) {
		oldDataRaw.clear();
		MSG_InitRaw(&oldMsg, &oldDataRaw);
	}
	else {
		MSG_Init(&oldMsg, oldData, sizeof(oldData));
	}
	/* Read the sequence number */
	if (FS_Read(&thisDemo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4) {
		Com_Printf("[NOTE] Demo cutter, reading sequence number failed.\n");
		return qfalse;
	}
	thisDemo.cut.Clc.serverMessageSequence = LittleLong(thisDemo.cut.Clc.serverMessageSequence);
	oldSize -= 4;
	/* Read the message size */
	if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4) {
		Com_Printf("[NOTE] Demo cutter, reading message size failed.\n");
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
		Com_Printf("[NOTE] Demo cutter, message size above maxsize.\n");
		return qfalse;
	}
	/* Read the actual message */
	if (FS_Read(&oldMsg, oldHandle) != oldMsg.cursize) {
		Com_Printf("[NOTE] Demo cutter, reading actual message failed.\n");
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
		byte cmd;
		if (oldMsg.readcount > oldMsg.cursize) {
			Com_Printf("Demo cutter, read past end of server message.\n");
			return qfalse;
		}
		cmd = MSG_ReadByte(&oldMsg);
		cmd = generalizeGameSVCOp(cmd,demoType);
		if (cmd == svc_EOF_general) {
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

		// other commands
		switch (cmd) {
		default:
			Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
			return qfalse;
		case svc_nop_general:
			break;
		case svc_serverCommand_general:
			demoCutParseCommandString(&oldMsg, &thisDemo.cut.Clc);
			break;
		case svc_gamestate_general:
			lastGameStateChange = thisDemo.cut.Cl.snap.serverTime;
			lastGameStateChangeInDemoTime = demoCurrentTime;
			//if (readGamestate > demo.currentNum) {
			//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
			//	goto cutcomplete;
			//}
			if (!demoCutParseGamestate(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, &demoType)) {
				Com_Printf("[NOTE] Demo cutter, parsing gamestate failed.\n");
				return qfalse;
			}
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
			if (!demoCutParseSnapshot(&oldMsg, &thisDemo.cut.Clc, &thisDemo.cut.Cl, demoType,qtrue)) {
				Com_Printf("[NOTE] Demo cutter, parsing snapshot failed.\n");
				return qfalse;
			}

			if (thisDemo.cut.Cl.lastSnapshotFinishedParsing) { // it can return true if the last snapshot was invalid too. but then we dont do this.
				SnapshotInfo snapshotInfo;
				snapshotInfo.serverTime = thisDemo.cut.Cl.snap.serverTime;
				for (int pe = thisDemo.cut.Cl.snap.parseEntitiesNum; pe < thisDemo.cut.Cl.snap.parseEntitiesNum + thisDemo.cut.Cl.snap.numEntities; pe++) {
					entityState_t* thisEntity = &thisDemo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					snapshotInfo.entities[thisEntity->number] = *thisEntity;

					lastMessageWithEntity[thisEntity->number] = thisDemo.cut.Cl.snap.messageNum;

					// See if we can figure out the command time of this entity if it's a player.
					if (thisEntity->number >= 0 && thisEntity->number < maxClientsThisDemo) {
						if (thisEntity->pos.trType == TR_LINEAR_STOP) { // I think this is true when g_smoothclients is true in which case commandtime is saved in trTime
							snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = thisEntity->pos.trTime;
						}
						else {
							snapshotInfo.playerCommandOrServerTimes[thisEntity->number] = lastKnownCommandOrServerTimes[thisEntity->number] = thisDemo.cut.Cl.snap.serverTime; // Otherwise just use servertime. Lame but oh well. Maybe we could do sth better where we try to detect changes in values or such if we truly need to.
						}
					}
				}
				snapshotInfo.snapNum = thisDemo.cut.Cl.snap.messageNum;
				snapshotInfo.playerState = thisDemo.cut.Cl.snap.ps;
				snapshotInfo.playerStateTeleport = PlayerStateIsTeleport(&thisDemo.cut.Cl.oldSnap, &thisDemo.cut.Cl.snap);
				snapshotInfo.snapFlagServerCount = (qboolean)((thisDemo.cut.Cl.oldSnap.snapFlags ^ thisDemo.cut.Cl.snap.snapFlags) & SNAPFLAG_SERVERCOUNT);
				snapshotInfo.playerCommandOrServerTimes[thisDemo.cut.Cl.snap.ps.clientNum] = lastKnownCommandOrServerTimes[thisDemo.cut.Cl.snap.ps.clientNum] = thisDemo.cut.Cl.snap.ps.commandTime;
				snapshotInfos[thisDemo.cut.Cl.snap.messageNum] = snapshotInfo;
				anySnapshotParsed = qtrue;// Fix? Well this makes more sense in any case.
			}


			// Time related stuff
			if (messageOffset++ == 0) {
				// first message in demo. Get servertime offset from here to cut correctly.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}
			if (thisDemo.cut.Cl.snap.serverTime < lastKnownTime && thisDemo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
				demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
				demoStartTime = thisDemo.cut.Cl.snap.serverTime;
			}
			demoCurrentTime = demoBaseTime + thisDemo.cut.Cl.snap.serverTime - demoStartTime;
			lastKnownTime = thisDemo.cut.Cl.snap.serverTime;


			// Fire playerstate events
			if (thisDemo.cut.Cl.snap.ps.externalEvent && thisDemo.cut.Cl.snap.ps.externalEvent != oldPS.externalEvent) {
				
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
					thisEvent.eventNumber = convertGameValue<GMAP_EVENTS>(thisEvent.eventNumber,DM_14,DM_15);
					thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS>(thisEvent.theEvent.event, DM_14, DM_15);
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
				if (i >= oldPS.eventSequence
					// or the server told us to play another event instead of a predicted event we already issued
					// or something the server told us changed our prediction causing a different event
					|| (i > oldPS.eventSequence - MAX_PS_EVENTS && thisDemo.cut.Cl.snap.ps.events[i & (MAX_PS_EVENTS - 1)] != oldPS.events[i & (MAX_PS_EVENTS - 1)])) {


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
						thisEvent.eventNumber = convertGameValue<GMAP_EVENTS>(thisEvent.eventNumber, DM_14, DM_15);
						thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS>(thisEvent.theEvent.event, DM_14, DM_15);
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
						thisEvent.eventNumber = convertGameValue<GMAP_EVENTS>(thisEvent.eventNumber, DM_14, DM_15);
						if (thisEvent.theEvent.eType > getET_EVENTS(demoType)) {
							thisEvent.theEvent.eType = jkaEventToJk2Map[thisEvent.theEvent.eType- getET_EVENTS(demoType)] + ET_EVENTS; // I just changed this, but should I? Hmm
						}
						else {
							thisEvent.theEvent.event = convertGameValue<GMAP_EVENTS>(thisEvent.theEvent.event, DM_14, DM_15); 
						}
					}
					else if (demoType == DM_26 || demoType == DM_25) { // Map events for JKA demos. Dunno if I'm doing it quite right. We'll see I guess.
						thisEvent.eventNumber = jkaEventToJk2Map[thisEvent.eventNumber];
						if (thisEvent.theEvent.eType > ET_EVENTS_JKA) {
							thisEvent.theEvent.eType = jkaEventToJk2Map[thisEvent.theEvent.eType-ET_EVENTS_JKA] + ET_EVENTS; // I just changed this, but should I? Hmm
						}
						else {
							thisEvent.theEvent.event = MapJKAEventJK2(thisEvent.theEvent.event);
						}
					}
					else if (demoType == DM_68) {
						thisEvent.eventNumber = q3dm68EventToJk2Map[thisEvent.eventNumber];
						if (thisEvent.theEvent.eType > ET_EVENTS_Q3) {
							thisEvent.theEvent.eType = q3dm68EventToJk2Map[thisEvent.theEvent.eType - ET_EVENTS_Q3] + ET_EVENTS;
						}
						else {
							thisEvent.theEvent.event = MapQ3DM68EventJK2(thisEvent.theEvent.event);
						}
					}
					readEvents.push_back(thisEvent);
					
				}
			}


			oldPS = thisDemo.cut.Cl.snap.ps;

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
		if (cmd[0]) {
			firstServerCommand = thisDemo.cut.Clc.lastExecutedServerCommand;
		}
		else {
			continue;
		}

		Command readCommand;
		readCommand.command = command;
		readCommand.demoTime = demoCurrentTime;
		readCommand.serverTime = thisDemo.cut.Cl.snap.serverTime;
		readCommands.push_back(readCommand);

		if (!strcmp(cmd, "cs")) {
			if (!demoCutConfigstringModified(&thisDemo.cut.Cl,demoType)) {
				Com_Printf("[NOTE] Demo cutter, configstring parsing failed.\n");
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


void remapConfigStrings(entityState_t* tmpEntity, clientActive_t* clCut, DemoReader* reader, std::vector<std::string>* commandsToAdd, qboolean doModelIndex, qboolean doModelIndex2,demoType_t demoType) {
	int eventHere = generalizeGameValue<GMAP_EVENTS>(tmpEntity->event & ~EV_EVENT_BITS,demoType);
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

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif