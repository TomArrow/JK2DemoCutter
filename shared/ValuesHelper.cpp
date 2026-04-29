#include "ValuesHelper.h"


// TODO tinfo

void ValuesHelper::checkTime(int serverTime) {
	if (serverTime < oldServerTime) {
		reset();
	}
	else if (serverTime != oldServerTime) {
		memset(esp, 0, sizeof(esp));
		espClientMask = 0;
	}
	oldServerTime = serverTime;
}

void ValuesHelper::reset() {

	memset(clientSeen, 0, sizeof(clientSeen));
	memset(esp, 0, sizeof(esp));
	espClientMask = 0;
	memset(residual, 0, sizeof(residual));
	memset(haveKeyFrame, 0, sizeof(haveKeyFrame));
	memset(isDead, 0, sizeof(isDead));
	memset(pickupInfo, 0, sizeof(pickupInfo));
	oldServerTime = -1;
}
void ValuesHelper::reset(int entNum) {
	if (entNum < 0 || entNum >= maxclients) {
		memset(&pickupInfo[entNum], 0, sizeof(pickupInfo[entNum]));
		return;
	}
	haveKeyFrame[entNum] = 0;
	clientSeen[entNum] = 0;
	memset(&residual[entNum], 0, sizeof(residual[entNum]));
}
void ValuesHelper::haveValue(int clientNum, int which, int value) {
	espClientMask |= (1<<clientNum);
	esp[clientNum].clientNum = clientNum;
	esp[clientNum].entityExtraValuesBitmask |= (1 << which);
	esp[clientNum].entityExtraValues[which] = value;
	residual[clientNum].val[which] = value;
	residual[clientNum].time[which] = oldServerTime;
}
std::vector<espDataPoint_t> ValuesHelper::getDataPoints() {
	std::vector<espDataPoint_t> retVal;
	if (!espClientMask) {
		return retVal;
	}
	for (int i = 0; i < maxclients; i++) {
		if (espClientMask & (1 << i)) {
			retVal.push_back(esp[i]);
		}
	}
	return retVal;
}
void ValuesHelper::noticePlayerState(int eventnum, playerState_t* ps, int serverTime) {
	checkTime(serverTime);
	haveKeyFrame[ps->clientNum] = (1 << ENTITYEXTRA_HEALTH) | (1 << ENTITYEXTRA_ARMOR) | (1 << ENTITYEXTRA_HASMEDPACK);
	residual[ps->clientNum].val[ENTITYEXTRA_HEALTH] = ps->stats[STAT_HEALTH];
	residual[ps->clientNum].val[ENTITYEXTRA_ARMOR] = ps->stats[STAT_ARMOR];
	residual[ps->clientNum].val[ENTITYEXTRA_HASMEDPACK] = ps->stats[STAT_HOLDABLE_ITEMS] & (1 << HI_MEDPAC);
	residual[ps->clientNum].time[ENTITYEXTRA_HEALTH] = serverTime;
	residual[ps->clientNum].time[ENTITYEXTRA_ARMOR] = serverTime;
	residual[ps->clientNum].time[ENTITYEXTRA_HASMEDPACK] = serverTime;
	clientSeen[ps->clientNum] = serverTime;
}
void ValuesHelper::noticeTeamInfo(int serverTime, demoType_t demoType) {
	int		i;
	int		client;

	checkTime(serverTime);

	int numSortedTeamPlayers = atoi(Cmd_Argv(1));

	for (i = 0; i < numSortedTeamPlayers; i++) {
		client = atoi(Cmd_Argv(i * 6 + 2));

		//sortedTeamPlayers[i] = client;

		//int location = atoi(Cmd_Argv(i * 6 + 3));
		int health = atoi(Cmd_Argv(i * 6 + 4));
		int armor = atoi(Cmd_Argv(i * 6 + 5));
		//int curWeapon = atoi(Cmd_Argv(i * 6 + 6));
		//int powerups = atoi(Cmd_Argv(i * 6 + 7));

		if (health <= 0) {
			isDead[client] = true;
			haveKeyFrame[client] = 0;
			haveValue(client, ENTITYEXTRA_ALIVESTATUS, 0);
			continue;
		}

		haveKeyFrame[client] |= (1 << ENTITYEXTRA_HEALTH) | (1 << ENTITYEXTRA_ARMOR);
		haveValue(client, ENTITYEXTRA_HEALTH, health);
		haveValue(client, ENTITYEXTRA_ARMOR, armor);
	}
}
void ValuesHelper::noticeEntity(int entityNum, entityState_t* ent, int serverTime, demoType_t demoType) {
	checkTime(serverTime);
	if (entityNum < 0 || entityNum >= maxclients) {
		int entityType = generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(ent->eType,demoType);
		if (entityType == ET_ITEM_GENERAL) {
			int itemlistItem = generalizeGameValue<GMAP_ITEMLIST,SAFE>(ent->modelindex,demoType);
			pickupInfo[entityNum].item = itemlistItem;
			if (itemlistItem == ITEMLIST_ITEM_SHIELD_SM_INSTANT_GENERAL) {
				pickupInfo[entityNum].shieldgain = 25;
			}
			else if (itemlistItem == ITEMLIST_ITEM_SHIELD_LRG_INSTANT_GENERAL) {
				pickupInfo[entityNum].shieldgain = 100;
			}
			else if (itemlistItem == ITEMLIST_ITEM_MEDPAK_INSTANT_GENERAL) {
				pickupInfo[entityNum].healthgain = 25;
			}
			else if (itemlistItem == ITEMLIST_ITEM_MEDPAC_GENERAL) { // just so it doesn't get reset
			}
			else {
				reset(entityNum);
			}
		}
		else {
			reset(entityNum);
		}
		return;
	}
	if (serverTime - clientSeen[entityNum] > EVENT_VALID_MSEC) {
		reset(entityNum);
		haveValue(entityNum, ENTITYEXTRA_VALIDSTATUS, 0);
	}
	int tunneled = 0;
	if (ent->demoToolsData.entityExtraValuesBitmask) {
		if (ent->demoToolsData.entityExtraValuesBitmask & (1 << ENTITYEXTRA_HEALTH)) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_HEALTH);
			tunneled |= (1 << ENTITYEXTRA_HEALTH);
			residual[entityNum].val[ENTITYEXTRA_HEALTH] = ent->demoToolsData.entityExtraValues[ENTITYEXTRA_HEALTH];
			residual[entityNum].time[ENTITYEXTRA_HEALTH] = serverTime;
		}
		if (ent->demoToolsData.entityExtraValuesBitmask & (1 << ENTITYEXTRA_ARMOR)) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_ARMOR);
			tunneled |= (1 << ENTITYEXTRA_ARMOR);
			residual[entityNum].val[ENTITYEXTRA_ARMOR] = ent->demoToolsData.entityExtraValues[ENTITYEXTRA_ARMOR];
			residual[entityNum].time[ENTITYEXTRA_ARMOR] = serverTime;
		}
		if (ent->demoToolsData.entityExtraValuesBitmask & (1 << ENTITYEXTRA_HASMEDPACK)) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_HASMEDPACK);
			tunneled |= (1 << ENTITYEXTRA_HASMEDPACK);
			residual[entityNum].val[ENTITYEXTRA_HASMEDPACK] = ent->demoToolsData.entityExtraValues[ENTITYEXTRA_HASMEDPACK];
			residual[entityNum].time[ENTITYEXTRA_HASMEDPACK] = serverTime;
		}
		// don't call haveValue here because tunneled values don't need to be ESP'd. we just use them as base values if needed.
	}
	int efdead = (demoType < DM_25 || demoType > DM_26_XBOX) ? EF_DEAD : EF_DEAD_JKA;
	if (isDead[entityNum] && !(ent->eFlags & efdead)) { // just respawned, i guess.
		isDead[entityNum] = false;
		if (!(tunneled & (1 << ENTITYEXTRA_HEALTH))) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_HEALTH);
			haveValue(entityNum, ENTITYEXTRA_HEALTH, 125);
		}
		if (!(tunneled & (1 << ENTITYEXTRA_ARMOR))) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_ARMOR);
			haveValue(entityNum, ENTITYEXTRA_ARMOR, 25);
		}
		if (!(tunneled & (1 << ENTITYEXTRA_HASMEDPACK))) {
			haveKeyFrame[entityNum] |= (1 << ENTITYEXTRA_HASMEDPACK);
			haveValue(entityNum, ENTITYEXTRA_HASMEDPACK, 0);
		}
	}
	clientSeen[entityNum] = serverTime;
}
void ValuesHelper::noticeEvent(int eventnum, entityState_t* eventEnt, int serverTime, demoType_t demoType) {
	checkTime(serverTime);
	if (eventnum == EV_OBITUARY_GENERAL) {
		int target = eventEnt->otherEntityNum;
		if (target >= 0 && target < maxclients) {
			isDead[target] = true;
			haveKeyFrame[target] = 0;
			haveValue(target, ENTITYEXTRA_ALIVESTATUS, 0);
		}
	} else if (eventnum == EV_SHIELD_HIT_GENERAL) {
		int target = eventEnt->otherEntityNum;
		if (target >= 0 && target < maxclients && (haveKeyFrame[target] & (1 << ENTITYEXTRA_ARMOR))) {
			int damage = eventEnt->time2;
			int shield = getBaseShield(target,serverTime);
			shield -= damage;
			if (shield < 0) {
				shield = 0;
			}
			haveValue(target, ENTITYEXTRA_ARMOR, shield);
		}
	} else if (eventnum == EV_PAIN_GENERAL) {
		int target = (eventEnt->eFlags & EF_PLAYER_EVENT) ? eventEnt->otherEntityNum : eventEnt->number;
		if (target >= 0 && target < maxclients) {
			int health = eventEnt->eventParm;
			haveKeyFrame[target] |= (1 << ENTITYEXTRA_HEALTH);
			haveValue(target, ENTITYEXTRA_HEALTH, health);
		}
	} else if ( eventnum == EV_ITEM_PICKUP_GENERAL ) {
		int pickedup = eventEnt->eventParm;
		int target = (eventEnt->eFlags & EF_PLAYER_EVENT) ? eventEnt->otherEntityNum : eventEnt->number;
		if (target >= 0 && target < maxclients) {
			if (pickupInfo[pickedup].item == ITEMLIST_ITEM_MEDPAC_GENERAL) {
				haveValue(target, ENTITYEXTRA_HASMEDPACK, 1);
			}
			if (pickupInfo[pickedup].shieldgain && (haveKeyFrame[target] & (1 << ENTITYEXTRA_ARMOR))) {
				int newshield = getBaseShield(target, serverTime);
				newshield += pickupInfo[pickedup].shieldgain;
				if (newshield > 200) {
					newshield = 200;
				}
				if (pickupInfo[pickedup].shieldgain == 25 && newshield > 100) {
					newshield = 100;
				}
				haveValue(target, ENTITYEXTRA_ARMOR, newshield);
			}
			if (pickupInfo[pickedup].healthgain && (haveKeyFrame[target] & (1 << ENTITYEXTRA_HEALTH))) {
				int newhealth = getBaseHealth(target, serverTime);
				newhealth += pickupInfo[pickedup].healthgain;
				if (newhealth > 200) {
					newhealth = 200;
				}
				if (pickupInfo[pickedup].healthgain == 25 && newhealth > 100) {
					newhealth = 100;
				}
				haveValue(target, ENTITYEXTRA_HEALTH, newhealth);
			}
		}
	}
	else if (eventnum == EV_USE_ITEM0_GENERAL + HI_MEDPAC) {
		int target = (eventEnt->eFlags & EF_PLAYER_EVENT) ? eventEnt->otherEntityNum : eventEnt->number;
		//if (haveKeyFrame[target] & (1 << ENTITYEXTRA_HEALTH)) {
			//int newhealth = getBaseHealth(target, serverTime);
			//newhealth += 100;
			//if (newhealth > 100) {
			//	newhealth = 100;
			//}
			//haveValue(target, ENTITYEXTRA_HEALTH, newhealth);
			haveKeyFrame[target] |= (1 << ENTITYEXTRA_HEALTH);
			haveValue(target, ENTITYEXTRA_HEALTH, 100); // it's 100. what else would it be?
			haveValue(target, ENTITYEXTRA_HASMEDPACK, 0); // defo don't have one now. unless we stood on one? oh well
		//}
	}
}

int ValuesHelper::decayValue(int value, int serverTime, int referenceTime) {
	if (value <= 100) {
		return value;
	}
	int delta = (serverTime / 1000) - (referenceTime / 1000);
	return std::max(100, value - delta);
}

int ValuesHelper::getBaseShield(int clientNum, int serverTime) {
	// bad approximation thats count down once per second or rather once the 000 mark in milliseconds is crossed
	// technically it's not *that* bad, but it's based on a residual time value in game, but it's internal
	// and we don't really know it for a given client.
	// if we had a playerstate we could infer it from a reference situation but let's not overcomplicate
	return decayValue(residual[clientNum].val[ENTITYEXTRA_ARMOR], serverTime, residual[clientNum].time[ENTITYEXTRA_ARMOR]);
}

int ValuesHelper::getBaseHealth(int clientNum, int serverTime) {
	// bad approximation thats count down once per second or rather once the 000 mark in milliseconds is crossed
	// technically it's not *that* bad, but it's based on a residual time value in game, but it's internal
	// and we don't really know it for a given client.
	// if we had a playerstate we could infer it from a reference situation but let's not overcomplicate
	return decayValue(residual[clientNum].val[ENTITYEXTRA_HEALTH], serverTime, residual[clientNum].time[ENTITYEXTRA_HEALTH]);
}
