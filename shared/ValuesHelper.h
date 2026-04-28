#pragma once
#ifndef VALUESHELPER_H
#define VALUESHELPER_H

#include <demoCut.h>
#include <jkaStuff.h>

typedef struct pickupInfo_s {
	int shieldgain;
	int healthgain;
	int item;
}pickupInfo_t;

typedef struct {
	int	val[ENTITYEXTRA_COUNT];
	int	time[ENTITYEXTRA_COUNT];
} residualEsp_t;

class ValuesHelper {
	int				maxclients = MAX_CLIENTS_MAX;
	int				oldServerTime = -1;
	espDataPoint_t	esp[MAX_CLIENTS_MAX] = { 0 };
	int				espClientMask = 0;
	int				haveKeyFrame[MAX_CLIENTS_MAX] = { 0 }; // bitmask for entityExtraValues_t
	bool			isDead[MAX_CLIENTS_MAX] = { false };
	residualEsp_t	residual[MAX_CLIENTS_MAX] = { 0 };
	int				clientSeen[MAX_CLIENTS_MAX] = { 0 };
	pickupInfo_t	pickupInfo[MAX_GENTITIES] = { 0 };

	void			checkTime(int serverTime);
	void			haveValue(int clientNum, int which, int value);
	int				getBaseShield(int clientNum, int serverTime);
	int				getBaseHealth(int clientNum, int serverTime);
public:
	void reset();
	void reset(int entNum);

	ValuesHelper(int maxclientsA) {
		maxclients = maxclientsA;
		reset();
	}
	void			noticeEvent(int eventnum, entityState_t* eventEnt, int serverTime, demoType_t demoType);
	void			noticePlayerState(int eventnum, playerState_t* ps, int serverTime);
	void			noticeEntity(int entityNum, entityState_t* ent, int serverTime, demoType_t demoType);
	static int		decayValue(int value, int serverTime, int referenceTime);
	std::vector<espDataPoint_t> getDataPoints();
};

#endif