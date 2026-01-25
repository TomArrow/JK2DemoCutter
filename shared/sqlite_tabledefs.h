#pragma once

#define NULLSTR3 NULL
#define NULLSTR2 NULLSTR3
#define NULLSTR NULLSTR2

#define UNIQUEGAME_ABSTRACT_TEXT()\
		FIELDSFUNC(mapName,mapName, "TEXT NOT NULL")\
		FIELDSFUNC(gameName,gameName, "TEXT NOT NULL")\
		FIELDSFUNC(version,version, "TEXT NOT NULL")\
		FIELDSFUNC(gameDate,gameDate, "TEXT NOT NULL")\
		FIELDSFUNC(sv_referencedPaks,sv_referencedPaks, "TEXT NOT NULL")\
		FIELDSFUNC(Location,Location, "TEXT NOT NULL")

#define UNIQUEGAME_ABSTRACT()\
		FIELDSFUNC(demoType,_demoType,"INTEGER NOT NULL")\
		FIELDSFUNC(protocol,protocol,"INTEGER NOT NULL")\
		FIELDSFUNC(g_truejedi,g_truejedi,"INTEGER NOT NULL")\
		FIELDSFUNC(g_gametype,g_gametype,"INTEGER NOT NULL")\
		FIELDSFUNC(sv_maxclients,sv_maxclients,"INTEGER NOT NULL")\
		FIELDSFUNC(g_forcepowerdisable,g_forcepowerdisable,"INTEGER NOT NULL")\
		FIELDSFUNC(g_weapondisable,g_weapondisable,"INTEGER NOT NULL")\
		FIELDSFUNC(g_duelweapondisable,g_duelweapondisable,"INTEGER NOT NULL")\
		FIELDSFUNC(g_maxforcerank,g_maxforcerank,"INTEGER NOT NULL")\
		FIELDSFUNC(sv_cheats,sv_cheats,"INTEGER NOT NULL")\
		FIELDSFUNC(serverId,serverId,"INTEGER NOT NULL")\
		FIELDSFUNC_LAST(sv_gameStartUnixTime,sv_gameStartUnixTime,"INTEGER NOT NULL")

// not sure about fs_game
// these are fields that can change during a game, or are reasonably unlikely to be guaranteed not to change
#define UNIQUEGAME_CONCRETE_TEXT()\
		FIELDSFUNC(serverName,serverName, "TEXT NOT NULL")\
		FIELDSFUNC(motd,motd, "TEXT NOT NULL")\
		FIELDSFUNC(message,message, "TEXT NOT NULL")\
		FIELDSFUNC(fs_game,fs_game, "TEXT NOT NULL")\
		FIELDSFUNC(lastMatchStatistics,_lastMatchStatistics, "TEXT NOT NULL")\
		FIELDSFUNC(lastScores,_lastScores, "TEXT NOT NULL")\
		FIELDSFUNC(CSHash,CSHash, "TEXT NOT NULL")

#define UNIQUEGAME_CONCRETE()\
		FIELDSFUNC(intermissionStatus,_intermissionStatus,"INTEGER NOT NULL")\
		FIELDSFUNC(levelStartTime,levelStartTime,"INTEGER NOT NULL")\
		FIELDSFUNC(sv_fps,sv_fps,"INTEGER NOT NULL")\
		FIELDSFUNC(g_needpass,g_needpass,"INTEGER NOT NULL")\
		FIELDSFUNC(g_forceregentime,g_forceregentime,"INTEGER NOT NULL")\
		FIELDSFUNC(g_blockchance,g_blockchance,"INTEGER NOT NULL")\
		FIELDSFUNC(g_blockchanceb,g_blockchanceb,"INTEGER NOT NULL")\
		FIELDSFUNC(g_blockchancer,g_blockchancer,"INTEGER NOT NULL")\
		FIELDSFUNC(g_blockchancey,g_blockchancey,"INTEGER NOT NULL")\
		FIELDSFUNC(g_blockfactor,g_blockfactor,"INTEGER NOT NULL")\
		FIELDSFUNC(g_disablerandomparries,g_disablerandomparries,"INTEGER NOT NULL")\
		FIELDSFUNC_LAST(g_saberUpdateRate,g_saberUpdateRate,"INTEGER NOT NULL")