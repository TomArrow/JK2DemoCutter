#pragma once
// Code is 99%-100% from jomme, from various header files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//


#ifndef DEMOCUT_H
#define DEMOCUT_H

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>


#include "anims.h"
#include "animsStanceMappings.h"

#include "include/rapidjson/document.h"

#include "tsl/htrie_map.h"

#include "unordered_dense.h"

extern int GlobalDebugOutputFlags;
extern int64_t GlobalDebugDemoTime;
extern const char* DPrintFLocation;

#ifdef _WIN32
#define WINDOWS // For popl library
#endif

extern std::string errorInfo;

#define NOMINMAX

#ifdef _NOTRYCATCH
#define __TRY if(1)
#define __EXCEPT if(0)
#elif defined(_MSC_VER)
#include <windows.h>
#undef SEARCH_ALL
#undef near
#include <excpt.h>
#include <dbghelp.h>

typedef struct exceptionCodeName_t {
	DWORD64 code;
	char* name;
};
#define EXCEPTNAME(name) {name,#name}
static const exceptionCodeName_t codeNames[] = {
	EXCEPTNAME(STILL_ACTIVE), EXCEPTNAME(EXCEPTION_ACCESS_VIOLATION), EXCEPTNAME(EXCEPTION_DATATYPE_MISALIGNMENT), EXCEPTNAME(EXCEPTION_BREAKPOINT), EXCEPTNAME(EXCEPTION_SINGLE_STEP), EXCEPTNAME(EXCEPTION_ARRAY_BOUNDS_EXCEEDED), EXCEPTNAME(EXCEPTION_FLT_DENORMAL_OPERAND), EXCEPTNAME(EXCEPTION_FLT_DIVIDE_BY_ZERO), EXCEPTNAME(EXCEPTION_FLT_INEXACT_RESULT), EXCEPTNAME(EXCEPTION_FLT_INVALID_OPERATION), EXCEPTNAME(EXCEPTION_FLT_OVERFLOW), EXCEPTNAME(EXCEPTION_FLT_STACK_CHECK), EXCEPTNAME(EXCEPTION_FLT_UNDERFLOW), EXCEPTNAME(EXCEPTION_INT_DIVIDE_BY_ZERO), EXCEPTNAME(EXCEPTION_INT_OVERFLOW), EXCEPTNAME(EXCEPTION_PRIV_INSTRUCTION), EXCEPTNAME(EXCEPTION_IN_PAGE_ERROR), EXCEPTNAME(EXCEPTION_ILLEGAL_INSTRUCTION), EXCEPTNAME(EXCEPTION_NONCONTINUABLE_EXCEPTION), EXCEPTNAME(EXCEPTION_STACK_OVERFLOW), EXCEPTNAME(EXCEPTION_INVALID_DISPOSITION), EXCEPTNAME(EXCEPTION_GUARD_PAGE), EXCEPTNAME(EXCEPTION_INVALID_HANDLE),
#ifdef STATUS_POSSIBLE_DEADLOCK
	EXCEPTNAME(EXCEPTION_POSSIBLE_DEADLOCK), 
#endif	
	EXCEPTNAME(CONTROL_C_EXIT)
};

inline  std::string getFunctionName(void* addr)
{
	std::stringstream ss;
	HANDLE hProcess = GetCurrentProcess();

	char searchPath[MAX_PATH+3];
	searchPath[0] ='.';
	searchPath[1] =';';
	searchPath[2] = 0;
	if (GetModuleFileNameA(NULL, searchPath + 2, MAX_PATH)) {
		// Remove filename itself
		int index = 2;
		int lastSlashPos = 1;
		while (searchPath[index] != 0) {
			if (searchPath[index] == '\\' || searchPath[index] == '/') {
				lastSlashPos = index;
			} 
			index++;
		}
		searchPath[lastSlashPos + 1] = 0; // Cut it off here.
	}
	else {

		searchPath[2] = 0;
	}

	

	SymInitialize(hProcess, searchPath, TRUE);

	IMAGEHLP_LINE line;
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

	char buf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	DWORD64 displacement = 0;
	if (SymFromAddr(hProcess, (DWORD64)addr, &displacement, symbol))
	{
		ss << symbol->Name;
		char functionName[MAX_SYM_NAME];
		if (UnDecorateSymbolName(symbol->Name, functionName, MAX_SYM_NAME, UNDNAME_COMPLETE))
		{
			ss << "," << functionName;
		}
	}

	DWORD displacementLine = 0;
	if (SymGetLineFromAddr(hProcess, (DWORD64)addr, &displacementLine, &line))
	{
		ss << ",file:" << line.FileName << "," << "line:" << line.LineNumber;
	}


	SymCleanup(hProcess);
	return ss.str();
}

int inline seh_filter_func(_EXCEPTION_POINTERS* exceptInfo) {
	std::stringstream ss;
	ss << errorInfo << std::hex << "addr:" << exceptInfo->ExceptionRecord->ExceptionAddress << ",code:" << exceptInfo->ExceptionRecord->ExceptionCode;
	for (int i = 0; i < sizeof(codeNames) / sizeof(codeNames[0]); i++) {
		if (codeNames[i].code == exceptInfo->ExceptionRecord->ExceptionCode) {
			ss << " (" << codeNames[i].name << ")";
			break;
		}
	}
	ss << std::hex << ",flags:" << exceptInfo->ExceptionRecord->ExceptionFlags << ",info:" << exceptInfo->ExceptionRecord->ExceptionInformation << ",record:" << exceptInfo->ExceptionRecord->ExceptionRecord << ",params:" << exceptInfo->ExceptionRecord->NumberParameters;
#ifdef _AMD64_
	ss << std::hex << ",rip:" << exceptInfo->ContextRecord->Rip << ",name:" << getFunctionName((void*)exceptInfo->ContextRecord->Rip);
#else
	ss << std::hex << ",eip:" << exceptInfo->ContextRecord->Eip << ",name:" << getFunctionName((void*)exceptInfo->ContextRecord->Eip);
#endif    
	ss << ",INFOEND";
	errorInfo = ss.str();
	return EXCEPTION_EXECUTE_HANDLER;
}
#define __TRY __try
#define __EXCEPT __except(seh_filter_func(GetExceptionInformation()))
#else
#define __TRY try
#define __EXCEPT catch(...)
#endif


class malformed_message_exception : public std::runtime_error {
public:
	malformed_message_exception(const char* const message) throw() : std::runtime_error(message) {
	}
};

/*
struct parsedArguments_t {
	std::vector<std::string> mainArgs;
	std::map<std::string, std::string> extraArgs;
};

inline parsedArguments_t parseArguments(char** argv, int argc) {
	parsedArguments_t retVal;

	for (int i = 0; i < argc; i++) {
		char* thisArg = argv[i];
		int thisArgLength = strlen(thisArg);

	}
}*/

#ifdef WINDOWS
#define atoll(S) _atoi64(S)
#endif




typedef enum globalDebugOutputType_t {
	DEBUG_CONFIGSTRING,
	DEBUG_COMMANDS,
	DEBUG_NETANALYSIS1,
	DEBUG_HIDDENUSERCMD
};




// OpenMOHAA
#define NUM_BONE_CONTROLLERS 5
#define MAX_FRAMEINFOS			16
typedef struct frameInfo_s {
	int index;
	float time;
	float weight;
} frameInfoMOHAA_t;

// OpenMOHAA end










enum demoType_t {
	DEMOTYPE_NONE,
	DM3_MOHAA_PROT_6, // MOHAA "MIN"
	//DM3_MOHAA_PROT_7, // MOHAA
	//DM3_MOHAA_PROT_8, // MOHAA
	DM3_MOHAA_PROT_15, // MOHTA "MIN"
	//DM3_MOHAA_PROT_16, // MOHTA
	//DM3_MOHAA_PROT_17, // MOHTA
	DM_14, // My JK2 Single Player demo work-in-progress
	DM_15,
	DM_15_1_03,
	DM_16,
	DM_25, // JKA 1.00 - not currently implemented
	DM_26, // JKA 1.01 - only rudimentary implementation... not true support
	DM_26_XBOX, // Not really in use atm but whatever, for completeness. Probably can't even record demos with it.
	DM_66, // Q3 v1.29 - v1.30 - not currently implemented
	DM_67, // Q3 v1.31 - not currently implemented
	DM_68, // Q3 v1.32 - not currently implemented
	DM_73, // Quake Live
	DM_90, // Quake Live 
	DM_91, // Quake Live 
	DEMOTYPE_COUNT
};



#define NULL 0

#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef struct _iobuf
{
	void* _Placeholder;
} FILE;
#endif

// this will need adjusting if demo tools should work on big endian
#define LittleLong
#define LittleShort
#define LittleFloat

#define MAX_CLIENTS			32
#define MAX_CLIENTS_MAX		64 // Highest of any game

#define MAX_TEAMS 64 // This is some bs from ui_local.h lol. I don't think it's valid but it's best I got

#define	MAX_QPATH			64		// max length of a quake game pathname
#define	MAX_OSPATH			256
#define	PATH_SEP '\\'

//#define	MAX_FILE_HANDLES	64
#define	MAX_FILE_HANDLES	1024

#define	MAX_MSGLEN_JK2SP				(3*16384)		// max length of a message, which may
#define	MAX_MSGLEN		MAX_MSGLEN_JK2SP		//16384	 (make it long enough for any game)		// max length of a message, which may
#define	MAX_MSGLEN_RAW			MAX_MSGLEN*10	// max length of a message, which may
// be fragmented into multiple packets

//#define	CMD_BACKUP			64	
#define	CMD_BACKUP			4096	
#define	CMD_MASK			(CMD_BACKUP - 1)

//#define	PACKET_BACKUP	32	// number of old messages that must be kept on client and
#define	PACKET_BACKUP	2048	// number of old messages that must be kept on client and
							// server for delta comrpession and ping estimation
#define	PACKET_BACKUP_MIN	32	// we can expect this much from any client ( i hope ? )
#define	PACKET_MASK		(PACKET_BACKUP-1)

#define	MAX_MAP_AREA_BYTES		32		// bit vector of area visibility


#define	GENTITYNUM_BITS		10		// don't need to send any more
#define	MAX_GENTITIES		(1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define	ENTITYNUM_NONE		(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD		(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)

//#define	MAX_PARSE_ENTITIES	2048
#define	MAX_PARSE_ENTITIES	131072
#define	MAX_PARSE_ENTITIES_MIN 2048 // we can expect this much from any client. though quake 2 is only 1024 but we dont support it rn and may never so w/e

#define	MAX_PS_EVENTS			2

// bit field limits
#define	MAX_STATS				32 //16 is for jk and all others. 32 is mohaa
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16
#define	MAX_WEAPONS				16		



#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))



#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_CHARS_MAX 2048	// MOHAA...
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token
#define	MAX_INFO_STRING		1024
#define	MAX_INFO_STRING_MAX		1350 // MOHAA...
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		  8192
#define	BIG_INFO_VALUE		8192


#define	MAX_CONFIGSTRINGS	1400

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can
#define	MAX_MODELS			256	
#define	MAX_SOUNDS			256	

#define	CS_MUSIC				2
#define	CS_MESSAGE				3		// from the map worldspawn's message field
#define	CS_MOTD					4		// g_motd string for server message of the day
#define	CS_WARMUP				5		// server time when the match will be restarted
#define	CS_SCORES1				6
#define	CS_SCORES2				7
#define CS_VOTE_TIME			8
#define CS_VOTE_STRING			9
#define	CS_VOTE_YES				10
#define	CS_VOTE_NO				11

#define CS_TEAMVOTE_TIME		12
#define CS_TEAMVOTE_STRING		14

#define	CS_TEAMVOTE_YES			16
#define	CS_TEAMVOTE_NO			18

#define	CS_GAME_VERSION			20
#define	CS_LEVEL_START_TIME		21		// so the timer only shows the current level
#define	CS_INTERMISSION			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS			23		// string indicating flag status in CTF
#define CS_SHADERSTATE			24
#define CS_BOTINFO				25

#define CS_MVSDK				26		// CS for mvsdk specific configuration

#define	CS_ITEMS				27		// string of 0's and 1's that tell which items are present

#define CS_CLIENT_JEDIMASTER	28		// current jedi master
#define CS_CLIENT_DUELWINNER	29		// current duel round winner - needed for printing at top of scoreboard
#define CS_CLIENT_DUELISTS		30		// client numbers for both current duelists. Needed for a number of client-side things.

#define	CS_MODELS				32
#define	CS_SOUNDS				(CS_MODELS+MAX_MODELS)
#define	CS_PLAYERS				(CS_SOUNDS+MAX_SOUNDS)

/*
Ghoul2 Insert Start
*/
#define CS_CHARSKINS_JK2			(CS_PLAYERS+MAX_CLIENTS)
/*
Ghoul2 Insert End
*/
#define	MAX_CHARSKINS_JK2		64		// character skins
#define MAX_LOCATIONS_JK2		64
#define MAX_FX_JK2				64		// max effects strings, I'm hoping that 64 will be plenty
#define MAX_LIGHT_STYLES_JK2		64
#define MAX_STRING_PACKAGES_JK2	30
#define CS_LOCATIONS_JK2			(CS_CHARSKINS_JK2+MAX_CHARSKINS_JK2)
#define CS_PARTICLES_JK2			(CS_LOCATIONS_JK2+MAX_LOCATIONS_JK2)
#define CS_EFFECTS_JK2				(CS_PARTICLES_JK2+MAX_LOCATIONS_JK2)
#define	CS_LIGHT_STYLES_JK2			(CS_EFFECTS_JK2 + MAX_FX_JK2)
#define CS_STRING_PACKAGES_JK2		(CS_LIGHT_STYLES_JK2 + (MAX_LIGHT_STYLES_JK2*3))

#define CS_MAX_JK2					(CS_STRING_PACKAGES_JK2+MAX_STRING_PACKAGES_JK2)

#if (CS_MAX_JK2) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX_JK2) > MAX_CONFIGSTRINGS
#endif

#define CS_GRAPPLESKIN_MAX_OFFSET 0
#define MAX_GRAPPLESKINS 64

//#define	MAX_GAMESTATE_CHARS	16000
#define	MAX_GAMESTATE_CHARS	41952 // Mohaa is hungry...




#define	DEFAULT_GRAVITY		800
#define	GIB_HEALTH			-40
#define ARMOR_PROTECTION		0.50 // Shields only stop 50% of armor-piercing dmg
#define ARMOR_REDUCTION_FACTOR	0.50 // Certain damage doesn't take off armor as efficiently

#define	JUMP_VELOCITY		225//270

#define	MAX_ITEMS			256

#define	RANK_TIED_FLAG		0x4000

#define	ITEM_RADIUS			15		// item sizes are needed for client side pickup detection

#define	SCORE_NOT_PRESENT	-9999	// for the CS_SCORES[12] when only one player is present

#define	VOTE_TIME			30000	// 30 seconds before vote times out

#define DEFAULT_MINS_2		-24
#define DEFAULT_MAXS_2		40
#define CROUCH_MAXS_2		16
#define	STANDARD_VIEWHEIGHT_OFFSET -4

#define	MINS_Z				-24
#define	DEFAULT_VIEWHEIGHT	(DEFAULT_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//26
#define CROUCH_VIEWHEIGHT	(CROUCH_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//12
#define	DEAD_VIEWHEIGHT		-16




typedef enum
{
	SABER_RED,
	SABER_ORANGE,
	SABER_YELLOW,
	SABER_GREEN,
	SABER_BLUE,
	SABER_PURPLE,
	//[RGBSaber]
	SABER_RGB,
	SABER_FLAME1,
	SABER_ELEC1,
	SABER_FLAME2,
	SABER_ELEC2,
	SABER_BLACK,
	//[/RGBSaber]
	NUM_SABER_COLORS

} saber_colors_t;

#define SABER_LENGTH_MAX	40


// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT		2048		// Maximum number of animation sequences is 2048 (0-2047).  12th bit is the toggle


// pmove->pm_flags
#define	PMF_DUCKED			1
#define	PMF_JUMP_HELD		2
#define PMF_ROLLING			4
#define	PMF_BACKWARDS_JUMP	8		// go into backwards land
#define	PMF_BACKWARDS_RUN	16		// coast down to backwards run
#define	PMF_TIME_LAND		32		// pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		// pm_time is an air-accelerate only time
#define	PMF_TIME_WATERJUMP	256		// pm_time is waterjump
#define	PMF_RESPAWNED		512		// clear after attack and jump buttons come up
#define	PMF_USE_ITEM_HELD	1024
#define PMF_UPDATE_ANIM		2048	// The server updated the animation, the pmove should set the ghoul2 anim to match.
#define PMF_FOLLOW			4096	// spectate following another player
#define PMF_SCOREBOARD		8192	// spectate as a scoreboard

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)

#define	MAXTOUCH	32


typedef enum {
	PM_NORMAL,		// can accelerate and turn
	PM_FLOAT,		// float with no gravity in general direction of velocity (intended for gripping)
	PM_NOCLIP,		// noclip movement
	PM_SPECTATOR,	// still run into walls
	PM_DEAD,		// no acceleration or turning, but free falling
	PM_FREEZE,		// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
	PM_SPINTERMISSION	// no movement or status bar
} pmtype_t;
// player_state->stats[] indexes
// NOTE: may not have more than 16
//typedef enum {
//	STAT_HEALTH,
//	STAT_HOLDABLE_ITEM,
//	STAT_HOLDABLE_ITEMS,
//	STAT_PERSISTANT_POWERUP,
//	STAT_WEAPONS,					// 16 bit fields
//	STAT_ARMOR,
//	STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
//	STAT_CLIENTS_READY,				// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
//	STAT_MAX_HEALTH,				// health / armor limit, changable by handicap
//	STAT_DASHTIME,
//	STAT_LASTJUMPSPEED,
//	STAT_RACEMODE,
//	STAT_ONLYBHOP,
//	STAT_MOVEMENTSTYLE,
//	STAT_JUMPTIME,
//	STAT_WJTIME
//} statIndex_t;

// player_state->stats[] indexes
// NOTE: may not have more than 16
typedef enum {
	STAT_HEALTH,
	STAT_HOLDABLE_ITEM,
	STAT_HOLDABLE_ITEMS,
	STAT_PERSISTANT_POWERUP,
	STAT_WEAPONS,					// 16 bit fields
	STAT_ARMOR,
	STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY,				// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_MAX_HEALTH,				// health / armor limit, changable by handicap
	STAT_BOUNCEPOWER,//STAT_DASHTIME,  // 9 bits for current bounce power (up to 500ms), 7 bits for regenerate timer (100 ms timeout to regen 10ms)
	STAT_LASTJUMPSPEED,//STAT_LASTJUMPSPEED, 
	STAT_RACEMODE,
	STAT_MSECRESTRICT,//STAT_ONLYBHOP, // 0= no restriction. otherwise msec value thats required for movement. in racemode its the msec value of racestyle. otherwise accepted physicsfps value
	STAT_MOVEMENTSTYLE,
	STAT_RUNFLAGS,//STAT_JUMPTIME, 
	STAT_PLACEHOLDER4,//STAT_WJTIME // unused rn
	STAT_CHARGEJUMPDATA = STAT_BOUNCEPOWER, // used in chargejumpmode ( reuses STAT_BOUNCEPOWER ). we just set some flags here for example
} statIndex_t;


typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;


#define	GIB_HEALTH			-40


#define Q_min(x,y) ((x)<(y)?(x):(y))
#define Q_max(x,y) ((x)>(y)?(x):(y))
#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorLerp( f, s, e, r ) ((r)[0]=(s)[0]+(f)*((e)[0]-(s)[0]),\
  (r)[1]=(s)[1]+(f)*((e)[1]-(s)[1]),\
  (r)[2]=(s)[2]+(f)*((e)[2]-(s)[2])) 

#define VectorSame(a,b)			((b)[0]==(a)[0] && (b)[1]==(a)[1] && (b)[2]==(a)[2])


#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])

#define	SnapVector(v) {v[0]=((int)(v[0]));v[1]=((int)(v[1]));v[2]=((int)(v[2]));}


#define Vector2Subtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1])


// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

typedef unsigned char 		byte;

typedef enum { qfalse, qtrue }	qboolean;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];


typedef int veci_t;
typedef veci_t veci2_t[2];
typedef veci_t veci3_t[3];
typedef veci_t veci4_t[4];
typedef veci_t veci5_t[5];


#define Square(x) ((x)*(x))

vec_t VectorLength(const vec3_t v);
vec_t VectorLength2(const vec2_t v);
vec_t VectorNormalize(vec3_t v); 
vec_t Vector2Normalize(vec2_t v); 
vec_t VectorNormalize2(const vec3_t v, vec3_t out);
void VectorInverse(vec3_t v);
vec_t VectorLengthSquared(const vec3_t v);
vec_t Vector2LengthSquared(const vec2_t v);
vec_t Distance(const vec3_t p1, const vec3_t p2);
vec_t DistanceSquared(const vec3_t p1, const vec3_t p2);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);

#ifdef _MSC_VER 
int __builtin_ctzll(unsigned long long input);
#endif


// past location encoding to get a ROUGH idea of the path a capper took.
#define LOCATIONSPIRAL_GRIDSIZE 500 // distance between points on the grid. we encode the closest point to ourselves
#define LOCATIONSPIRAL_GRIDSIZE_MINDISTANCE (LOCATIONSPIRAL_GRIDSIZE/2) // distance between points on the grid. we encode the closest point to ourselves
static constexpr float LOCATIONSPIRAL_GRIDSIZE_MINDISTANCE_SQUARED = (float)LOCATIONSPIRAL_GRIDSIZE_MINDISTANCE * (float)LOCATIONSPIRAL_GRIDSIZE_MINDISTANCE; // if we stand right at the transition between two locations and move back and forth, that shouldn't trigger a new location or we spam it.
static constexpr float LOCATIONSPIRAL_GRID_MULTIPLIER = 1.0f/(float)LOCATIONSPIRAL_GRIDSIZE; //multiply by this instead of dividing. ezier
typedef struct spiralLocation_t {
	int		encodedLocation;
	int64_t	demoTime;
	vec2_t	actualLocation;
	veci2_t	roughLocation;
};
extern const char base91BasicAlphabet_[91]; // base91
extern const uint8_t base91DecAlphabet_[256]; // base91
int encode2dspiral(int x, int y);
void decode2dspiral(int index, int* x, int* y);
qboolean encodeNewLocation(vec_t* origin, spiralLocation_t* oldLocation, spiralLocation_t* newLocation);
// END past location encoding to get a ROUGH idea of the path a capper took.


inline float AngleDelta(float angle1, float angle2);
inline float AngleNormalize180(float angle);
inline float AngleNormalize360(float angle);


typedef union {
	float f;
	int i;
	unsigned int ui;
} floatint_t;

inline int VectorCompare(const vec3_t v1, const vec3_t v2) {
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2]) {
		return 0;
	}
	return 1;
}

typedef struct {
	float		normal[3];
	float		dist;
} dplane_t;


// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;
#define MAX_G2_COLLISIONS 16

/*
Ghoul2 Insert Start
*/
typedef struct
{
	float		mDistance;
	int			mEntityNum;
	int			mModelIndex;
	int			mPolyIndex;
	int			mSurfaceIndex;
	vec3_t		mCollisionPosition;
	vec3_t		mCollisionNormal;
	int			mFlags;
	int			mMaterial;
	int			mLocation;
	float		mBarycentricI; // two barycentic coodinates for the hit point
	float		mBarycentricJ; // K = 1-I-J
} CollisionRecord_t;

// a trace is returned when a box is swept through the world
typedef struct {
	qboolean	allsolid;	// if true, plane is not valid
	qboolean	startsolid;	// if true, the initial point was in a solid area
	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface hit
	int			entityNum;	// entity the contacted sirface is a part of
/*
Ghoul2 Insert Start
*/
	CollisionRecord_t G2CollisionMap[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit
/*
Ghoul2 Insert End
*/
} trace_t;

typedef int		clipHandle_t;

typedef enum {
	AXIS_SIDE,
	AXIS_FORWARD,
	AXIS_UP,
	AXIS_ROLL,
	AXIS_YAW,
	AXIS_PITCH,
	MAX_JOYSTICK_AXIS
} joystickAxis_t;

//#define MAX_CONFIGSTRINGS_MAX 1700 // Highest I found anywhere (JKA). We gotta be able to hold it all.
#define MAX_CONFIGSTRINGS_MAX 2736 // Highest I found anywhere (MOHAA). We gotta be able to hold it all.

typedef struct {
	int			stringOffsets[MAX_CONFIGSTRINGS_MAX];
	char		stringData[MAX_GAMESTATE_CHARS];
	int			dataCount;
} gameState_t;


typedef struct {
	int		p_cmdNumber;		// cl.cmdNumber when packet was sent
	int		p_serverTime;		// usercmd->serverTime when packet was sent
	int		p_realtime;			// cls.realtime when packet was sent
} outPacket_t;

typedef enum
{
	FP_FIRST = 0,//marker
	FP_HEAL = 0,//instant
	FP_LEVITATION,//hold/duration
	FP_SPEED,//duration
	FP_PUSH,//hold/duration
	FP_PULL,//hold/duration
	FP_TELEPATHY,//instant
	FP_GRIP,//hold/duration
	FP_LIGHTNING,//hold/duration
	FP_RAGE,//duration
	FP_PROTECT,
	FP_ABSORB,
	FP_TEAM_HEAL,
	FP_TEAM_FORCE,
	FP_DRAIN,
	FP_SEE,
	FP_SABERATTACK,
	FP_SABERDEFEND,
	FP_SABERTHROW,
	NUM_FORCE_POWERS
} forcePowers_t;

typedef enum {
	TRACK_CHANNEL_NONE = 50,
	TRACK_CHANNEL_1,
	TRACK_CHANNEL_2,
	TRACK_CHANNEL_3,
	TRACK_CHANNEL_4,
	TRACK_CHANNEL_5,
	NUM_TRACK_CHANNELS
} trackchan_t;

#define TRACK_CHANNEL_MAX (NUM_TRACK_CHANNELS-50)

typedef struct forcedata_s {
	//NOT_FULLY_NETWORKED//int			forcePowerDebounce[NUM_FORCE_POWERS];	//for effects that must have an interval
	int			forcePowerDebounce_FP_LEVITATION_;	//for effects that must have an interval
	int			forcePowersKnown;
	int			forcePowersActive;
	int			forcePowerSelected;
	//NOT_NETWORKED//int			forceButtonNeedRelease;
	//NOT_NETWORKED//int			forcePowerDuration[NUM_FORCE_POWERS];
	int			forcePower;
	//NOT_NETWORKED//int			forcePowerMax;
	//NOT_NETWORKED//int			forcePowerRegenDebounceTime;
	//NOT_FULLY_NETWORKED//int			forcePowerLevel[NUM_FORCE_POWERS];		//so we know the max forceJump power you have
	int			forcePowerLevel_FP_SEE_;
	int			forcePowerLevel_FP_LEVITATION_;
	//NOT_NETWORKED//int			forcePowerBaseLevel[NUM_FORCE_POWERS];
	//NOT_NETWORKED//int			forceUsingAdded;
	float		forceJumpZStart;					//So when you land, you don't get hurt as much
	//NOT_NETWORKED//float		forceJumpCharge;					//you're current forceJump charge-up level, increases the longer you hold the force jump button down
	//NOT_NETWORKED//int			forceJumpSound;
	//NOT_NETWORKED//int			forceJumpAddTime;
	//NOT_NETWORKED//int			forceGripEntityNum;					//what entity I'm gripping
	//NOT_NETWORKED//int			forceGripDamageDebounceTime;		//debounce for grip damage
	//NOT_NETWORKED//float		forceGripBeingGripped;				//if > level.time then client is in someone's grip
	int			forceGripCripple;					//if != 0 then make it so this client can't move quickly (he's being gripped) 
	//NOT_NETWORKED//int			forceGripUseTime;					//can't use if > level.time
	//NOT_NETWORKED//float		forceGripSoundTime;
	//NOT_NETWORKED//float		forceGripStarted;					//level.time when the grip was activated
	//NOT_NETWORKED//float		forceSpeedSmash;
	//NOT_NETWORKED//float		forceSpeedDoDamage;
	//NOT_NETWORKED//int			forceSpeedHitIndex;					//if we hit another player and got hurt, hurt them too
	//NOT_NETWORKED//int			forceHealTime;
	//NOT_NETWORKED//int			forceHealAmount;

	//This hurts me somewhat to do, but there's no other real way to allow completely "dynamic" mindtricking.
	int			forceMindtrickTargetIndex; //0-15
	int			forceMindtrickTargetIndex2; //16-32
	int			forceMindtrickTargetIndex3; //33-48
	int			forceMindtrickTargetIndex4; //49-64

	int			forceRageRecoveryTime;
	//NOT_NETWORKED//int			forceDrainEntNum;
	//NOT_NETWORKED//float		forceDrainTime;

	//NOT_NETWORKED//int			forceDoInit;

	int			forceSide;
	//NOT_NETWORKED//int			forceRank;

	//NOT_NETWORKED//int			forceDeactivateAll;

	//NOT_NETWORKED//int			killSoundEntIndex[TRACK_CHANNEL_MAX]; //this goes here so it doesn't get wiped over respawn

	qboolean	sentryDeployed;

	int			saberAnimLevel;
	int			saberDrawAnimLevel;

	//NOT_NETWORKED//int			suicides;

	//NOT_NETWORKED//int			privateDuelTime;

	// not networked but used in pmove:
	float		forceSpeedSmash;
	float		forceJumpCharge;
	int			forcePowerDuration_FP_LEVITATION_;
} forcedata_t;


// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
// NOTE: may not have more than 16
typedef enum {
	PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,						// total points damage inflicted so damage beeps can sound on change
	PERS_RANK,						// player rank or team rank
	PERS_TEAM,						// player team
	PERS_SPAWN_COUNT,				// incremented every respawn
	PERS_PLAYEREVENTS,				// 16 bits that can be flipped for events
	PERS_ATTACKER,					// clientnum of last damage inflicter
	PERS_ATTACKEE_ARMOR,			// health/armor of last person we attacked
	PERS_KILLED,					// count of the number of times you died
	// player awards tracking
	PERS_IMPRESSIVE_COUNT,			// two railgun hits in a row
	PERS_EXCELLENT_COUNT,			// two successive kills in a short amount of time
	PERS_DEFEND_COUNT,				// defend awards
	PERS_ASSIST_COUNT,				// assist awards
	PERS_GAUNTLET_FRAG_COUNT,		// kills with the guantlet
	PERS_CAPTURES					// captures
} persEnum_t;



// Jk2 SP stuff

#define MAX_INVENTORY			15		// See INV_MAX
// JK2 SP stuff end




// xkan, 1/10/2003 - adapted from original SP
typedef enum
{
	AISTATE_RELAXED,
	AISTATE_QUERY,
	AISTATE_ALERT,
	AISTATE_COMBAT,

	MAX_AISTATES
} aistateEnum_t;


typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;


typedef enum entityExtraValues_t {
	ENTITYEXTRA_HEALTH,
	ENTITYEXTRA_ARMOR,
	ENTITYEXTRA_FORCE,
	ENTITYEXTRA_SABERDRAWANIMLEVEL,
	ENTITYEXTRA_CURRENTWEAPONAMMO,
	ENTITYEXTRA_TRIPMINEAMMO,
	ENTITYEXTRA_HASSENTRY,
	ENTITYEXTRA_HASMEDPACK,
	ENTITYEXTRA_HASFORCEFIELD,
	ENTITYEXTRA_HASSEEKER,
	ENTITYEXTRA_COUNT
};


typedef struct entityMeta_s {
	vec3_t	modelScale;
	const char* skin;
	byte saberColor;
} entityMeta_t;

typedef enum demoToolsEntityFlags_s {
	DTFLAG_MINMAXSET = (1<<0),
	DTFLAG_ISLIMB = (1<<1), // used for jk2sp bouncing related stuff
	DTFLAG_JK2MP_ITEMBOUNCE = (1<<2),
}demoToolsEntityFlags_s;

typedef struct demoToolsEntityData_t{
	int entityExtraValuesBitmask;
	short entityExtraValues[ENTITYEXTRA_COUNT];
	int detectedDimension;
	bool globalDimension;
	bool globalEvent;
	int serverTime; // Server time of this entity state. To be used where necessary/useful.
	bool isInterpolated;
	trajectory_t uninterpolatedPos;
	trajectory_t uninterpolatedAPos;
	entityMeta_t entityMeta; // for combiner. modelscale etc. to know when to change entcs
	vec3_t mins, maxs;
	int flags; // demoToolsEntityFlags_s
};



typedef struct playerState_s {
	// Shared between games:

	int			commandTime;	// cmd->serverTime of last executed command
	vec3_t		origin;
	vec3_t		viewangles;		// for fixed views
	vec3_t		velocity;
	int			speed;
	int			delta_angles[3];	// add to command angles to get view direction
									// changed by spawns, rotating objects, and teleporters
	int			viewheight;
	int			groundEntityNum;// ENTITYNUM_NONE = in air
	int			gravity;
	int			clientNum;		// ranges from 0 to MAX_CLIENTS-1
	int			pm_type;
	int			bobCycle;		// for view bobbing and footstep generation
	int			pm_time;



	// JK2/jk3:
	int			pm_flags;		// ducked, jump_held, etc

	int			weaponTime;
	int			weaponChargeTime;
	int			weaponChargeSubtractTime;
	int			basespeed; //used in prediction to know base server g_speed value when modifying speed between updates

	//NOT_NETWORKED//int			useTime;


	int			legsTimer;		// don't change low priority animations until this runs out
	int			legsAnim;		// mask off ANIM_TOGGLEBIT

	int			torsoTimer;		// don't change low priority animations until this runs out
	int			torsoAnim;		// mask off ANIM_TOGGLEBIT

	int			movementDir;	// a number 0 to 7 that represents the reletive angle
								// of movement to the view angle (axial and diagonals)
								// when at rest, the value will remain unchanged
								// used to twist the legs during strafing

	vec3_t		grapplePoint;	// Q3 specific: location of grapple to pull towards if PMF_GRAPPLE_PULL 

	int			eFlags;			// copied to entityState_t->eFlags

	int			eventSequence;	// pmove generated events
	int			events[MAX_PS_EVENTS];
	int			eventParms[MAX_PS_EVENTS];

	int			externalEvent;	// events set on player from another source
	int			externalEventParm;
	int			externalEventTime;

	int			weapon;			// copied to entityState_t->weapon
	int			weaponstate;


	// damage feedback
	int			damageEvent;	// when it changes, latch the other parms
	int			damageYaw;
	int			damagePitch;
	int			damageCount;
	int			damageType;

	//NOT_NETWORKED//int			painTime;		// used for both game and client side to process the pain twitch - NOT sent across the network
	//NOT_NETWORKED//int			painDirection;	// NOT sent across the network
	//NOT_NETWORKED//float		yawAngle;		// NOT sent across the network
	//NOT_NETWORKED//qboolean	yawing;			// NOT sent across the network
	//NOT_NETWORKED//float		pitchAngle;		// NOT sent across the network
	//NOT_NETWORKED//qboolean	pitching;		// NOT sent across the network

	int			stats[MAX_STATS];
	int			persistant[MAX_PERSISTANT];	// stats that aren't cleared on death
	int			powerups[MAX_POWERUPS];	// level.time that the powerup runs out
	int			ammo[MAX_WEAPONS];



	int			generic1;
	int			loopSound;
	int			jumppad_ent;	// jumppad entity hit this frame

	// not communicated over the net at all
	//NOT_NETWORKED//int			ping;			// server to game info for scoreboard
	//NOT_NETWORKED//int			pmove_framecount;	// FIXME: don't transmit over the network
	//NOT_NETWORKED//int			jumppad_frame;
	int			entityEventSequenceNOTNETWORKED;

	//NOT_NETWORKED//int			lastOnGround;	//last time you were on the ground

	qboolean	saberInFlight;
	qboolean	saberActive;

	int			saberMove;
	//NOT_NETWORKED//int			saberBlocking;
	int			saberBlocked;

	int			saberLockTime;
	int			saberLockEnemy;
	int			saberLockFrame; //since we don't actually have the ability to get the current anim frame
	//NOT_NETWORKED//int			saberLockHits; //every x number of buttons hits, allow one push forward in a saber lock (server only)
	qboolean	saberLockAdvance; //do an advance (sent across net as 1 bit)

	int			saberEntityNum;
	//NOT_NETWORKED//float		saberEntityDist;
	//NOT_NETWORKED//int			saberEntityState;
	//NOT_NETWORKED//int			saberThrowDelay;
	qboolean	saberCanThrow;
	//NOT_NETWORKED//int			saberDidThrowTime;
	//NOT_NETWORKED//int			saberDamageDebounceTime;
	//NOT_NETWORKED//int			saberHitWallSoundDebounceTime;
	//NOT_NETWORKED//int			saberEventFlags;

	int			rocketLockIndex;
	//NOT_NETWORKED//float		rocketLastValidTime;
	float		rocketLockTime;
	float		rocketTargetTime;

	int			emplacedIndex;
	//NOT_NETWORKED//float		emplacedTime;

	qboolean	isJediMaster;
	qboolean	forceRestricted;
	qboolean	trueJedi;
	qboolean	trueNonJedi;
	//NOT_NETWORKED//int			saberIndex;

	int			genericEnemyIndex;
	//NOT_NETWORKED//float		droneFireTime;
	//NOT_NETWORKED//float		droneExistTime;

	int			activeForcePass;

	qboolean	hasDetPackPlanted; //better than taking up an eFlag isn't it?

	//NOT_NETWORKED//float		holocronsCarried[NUM_FORCE_POWERS];
	//NOT_NETWORKED//int			holocronCantTouch;
	//NOT_NETWORKED//float		holocronCantTouchTime; //for keeping track of the last holocron that just popped out of me (if any)
	int			holocronBits;

	//NOT_NETWORKED//int			legsAnimExecute;
	//NOT_NETWORKED//int			torsoAnimExecute;
	//NOT_NETWORKED//int			fullAnimExecute;

	int			electrifyTime;

	//NOT_NETWORKED//int			saberAttackSequence;
	//NOT_NETWORKED//int			saberIdleWound;
	//NOT_NETWORKED//int			saberAttackWound;
	//NOT_NETWORKED//int			saberBlockTime;

	//NOT_NETWORKED//int			otherKiller;
	//NOT_NETWORKED//int			otherKillerTime;
	//NOT_NETWORKED//int			otherKillerDebounceTime;

	forcedata_t	fd;
	//NOT_NETWORKED//qboolean	forceJumpFlip;
	int			forceHandExtend;
	//NOT_NETWORKED//int			forceHandExtendTime;

	//NOT_NETWORKED//int			forceRageDrainTime;

	int			forceDodgeAnim;
	//NOT_NETWORKED//qboolean	quickerGetup;

	//NOT_NETWORKED//int			groundTime;		// time when first left ground

	//NOT_NETWORKED//int			footstepTime;

	//NOT_NETWORKED//int			otherSoundTime;
	//NOT_NETWORKED//float		otherSoundLen;

	//NOT_NETWORKED//int			forceGripMoveInterval;
	//NOT_NETWORKED//int			forceGripChangeMovetype;

	//NOT_NETWORKED//int			forceKickFlip;

	int			duelIndex;
	int			duelTime;
	qboolean	duelInProgress;

	int			saberAttackChainCount;

	qboolean	saberHolstered;

	qboolean	usingATST;
	qboolean	atstAltFire;
	//NOT_NETWORKED//int			holdMoveTime;

	//NOT_NETWORKED//int			forceAllowDeactivateTime;

	// zoom key
	int			zoomMode;		// 0 - not zoomed, 1 - disruptor weapon
	int			zoomTime;
	qboolean	zoomLocked;
	float		zoomFov;
	//NOT_NETWORKED//int			zoomLockTime;

	int			fallingToDeath;

	//NOT_NETWORKED//int			useDelay;

	qboolean	inAirAnim;

	qboolean	dualBlade;

	vec3_t		lastHitLoc;


	// Shared among games:

	int fov;
	float friction;
	
	union {
		
		struct { // 208 bytes
			// JKA Specific:
			vec3_t		moveDir; //NOT sent over the net - nor should it be.
			float		speedJKA;
			int			slopeRecalcTime; //this is NOT sent across the net and is maintained seperately on game and cgame in pmove code.
			qboolean	legsFlip; //set to opposite when the same anim needs restarting, sent over in only 1 bit. Cleaner and makes porting easier than having that god forsaken ANIM_TOGGLEBIT.
			qboolean	torsoFlip;
			int			eFlags2;		// copied to entityState_t->eFlags2, EF2_??? used much less frequently
			int			saberLockHitCheckTime; //so we don't allow more than 1 push per server frame
			int			saberLockHitIncrementTime; //so we don't add a hit per attack button press more than once per server frame
			int			saberHolsteredJKA;
			int			heldByClient; //can only be a client index - this client should be holding onto my arm using IK stuff.
			int			ragAttach; //attach to ent while ragging
			int			iModelScale;
			int			brokenLimbs;
			//for looking at an entity's origin (NPCs and players)
			qboolean	hasLookTarget;
			int			lookTarget;
			int			customRGBA[4];
			int			standheight;
			int			crouchheight;
			//If non-0, this is the index of the vehicle a player/NPC is riding.
			int			m_iVehicleNum;
			//lovely hack for keeping vehicle orientation in sync with prediction
			vec3_t		vehOrientation;
			qboolean	vehBoarding;
			int			vehSurfaces;
			//vehicle turnaround stuff (need this in ps so it doesn't jerk too much in prediction)
			int			vehTurnaroundIndex;
			int			vehTurnaroundTime;
			//vehicle has weapons linked
			qboolean	vehWeaponsLinked;
			//when hyperspacing, you just go forward really fast for HYPERSPACE_TIME
			int			hyperSpaceTime;
			vec3_t		hyperSpaceAngles;
			//hacking when > time
			int			hackingTime;
			//actual hack amount - only for the proper percentage display when
			//drawing progress bar (is there a less bandwidth-eating way to do
			//this without a lot of hassle?)
			int			hackingBaseTime;
			//keeps track of jetpack fuel
			int			jetpackFuel;
			//keeps track of cloak fuel
			int			cloakFuel;
			//rww - spare values specifically for use by mod authors.
			//See psf_overrides.txt if you want to increase the send
			//amount of any of these above 1 bit.
			int			userInt1;
			int			userInt2;
			int			userInt3;
			float		userFloat1;
			float		userFloat2;
			float		userFloat3;
			vec3_t		userVec1;
			vec3_t		userVec2;
#ifdef _ONEBIT_COMBO
			int			deltaOneBits;
			int			deltaNumBits;
#endif
		};

		struct {

			union {
				struct { // 132 bytes
					// RTCW (according to uber demo tools):

					// for weapons that don't fire immediately when 'fire' is hit (grenades, venom, ...)
					int weaponDelay;
					// for delayed grenade throwing. this is set to a #define for grenade
					// lifetime when the attack button goes down, then when attack is released
					// this is the amount of time left before the grenade goes off
					// (or if it gets to 0 while in player's hand, it explodes)
					int grenadeTimeLeft;
					float leanf;             // amount of 'lean' when player is looking around corner
					int weapons[2];          // 64 bits for weapons held
					int weapAnim;            // mask off ANIM_TOGGLEBIT
					vec3_t mins, maxs;
					float crouchMaxZ;
					float crouchViewHeight, standViewHeight, deadViewHeight;
					float runSpeedScale, sprintSpeedScale, crouchSpeedScale; // variable movement speed
					int viewlocked;          // view locking for mg42
					int viewlocked_entNum;
					// need this to fix friction problems with slow zombies whereby
					// the friction prevents them from accelerating to their full potential
					int aiChar;              // AI character id is used for weapon association
					int teamNum;
					int gunfx;
					int sprintTime;
					int aimSpreadScale;      // 0-255 increases with angular movement
					int onFireStart;         // burning effect is required for view blending effect
					int classWeaponTime;
					int serverCursorHint;    // what type of cursor hint the server is dictating
					int serverCursorHintVal; // a value (0-255) associated with the above
					int curWeapHeat;         // for the currently selected weapon
					int aiState;
					//int ammoclip[64]; // Not needed for net comms
					//int holdable[16]; // Not needed for net comms
					
					// ET
					int nextWeapon;
					int identifyClient; // NERVE - SMF
					int identifyClientHealth;
					aistateEnum_t aiStateET;          // xkan, 1/10/2003
				};

				struct { // 160 bytes
					
					// Quake Live (according to uber demo tools):
					int doubleJumped; // qboolean 
					int jumpTime;
					int weaponPrimary;
					int crouchTime;
					int crouchSlideTime;
					int location;
					int forwardmove;
					int rightmove;
					int upmove;

					// JK2 SP stuff
					int			leanofs;
					int			batteryCharge;
					saber_colors_t	saberColor;
					float		saberLength;
					float		saberLengthMax;
					int			forcePowersActive;	//prediction needs to know this
					int			vehicleModel;	// For overriding your playermodel with a drivable vehicle
					int			viewEntity;		// For overriding player movement controls and vieworg
					vec3_t		serverViewOrg;
					int			inventory[MAX_INVENTORY];
				};
			};
		};
		
		


		struct { // 140 bytes (not counting the stats-like fields)
			// OpenMOHAA:
			int			net_pm_flags;	// ducked, jump_held, etc
			int			feetfalling;
			float		fovMOHAA;

			float		fLeanAngle;
			int			iNetViewModelAnim;
			int			iViewModelAnimChanged;

			int			current_music_mood;
			int			fallback_music_mood;
			float		music_volume;
			float		music_volume_fade_time;
			int			reverb_type;
			float		reverb_level;
			float		blend[4];
			float		camera_origin[3];
			float		camera_angles[3];
			float		camera_time;
			float		camera_offset[3];
			float		camera_posofs[3];
			int			camera_flags;
			float		damage_angles[3];
			// Team Assault
			int			radarInfo;
			qboolean	voted;

			// stats-like fields:
			int			activeItems[8]; // MOHAA
			int			ammo_name_index[16]; // MOHAA
			int			ammo_amount[16]; // MOHAA
			int			max_ammo_amount[16]; // MOHAA
		};
	};

	// non-networked but needed for pmove: (but maybe not? maybe hide behind a #define someday)
	int			pmove_framecount;	// FIXME: don't transmit over the network
	int			forceHandExtendTime;
	int			saberAttackSequence;
	int			forceJumpFlip;
	int			groundTime;

	// Demo Tools specific values, only used internally
	// Not exactly a super elegant way of doing it but eh,
	// we don't have the problem of needing these structs to be in sync between game,cgame,engine etc.
	demoToolsEntityData_t demoToolsData;

} playerState_t;

typedef struct server_sound_s { // MOHAA
	vec3_t origin;
	int entity_number;
	int channel;
	short int sound_index;
	float volume;
	float min_dist;
	float maxDist;
	float pitch;
	qboolean stop_flag;
	qboolean streamed;
} server_sound_t;

typedef enum snapIssues_s {
	SNAPISSUE_DELTAFROMINVALID_TRYING_LAST = (1<<0),
	SNAPISSUE_DELTAFROMINVALID = (1 << 1),
	SNAPISSUE_DELTAFRAMETOOOLD = (1 << 2),
	SNAPISSUE_DELTAPARSEENTITIESNUMTOOOLD = (1 << 3),
	SNAPISSUE_DELTAFROMSNAPWITHISSUES = (1 << 4),

} snapIssues_t;

typedef struct {
	qboolean		valid;			// cleared if delta parsing was invalid
	int				snapFlags;		// rate delayed and dropped commands

	int				snapIssues;		// from snapIssues_t enum. to see if parsing this was smooth

	int				serverTime;		// server time the message is valid for (in msec)
	int				serverTimeResidual; // MOHAA

	int				messageNum;		// copied from netchan->incoming_sequence
	int				deltaNum;		// messageNum the delta is from
	int				ping;			// time from when cmdNum-1 was sent to time packet was reeceived
	byte			areamask[MAX_MAP_AREA_BYTES];		// portalarea visibility bits
	int				areamaskLen;

	int				cmdNum;			// the next cmdNum the server is expecting
	playerState_t	ps;						// complete information about the current player at this time
	playerState_t	vps; //vehicle I'm riding's playerstate (if applicable) -rww

	int				numEntities;			// all of the entities that need to be presented
	int				parseEntitiesNum;		// at the time of this snapshot

	qboolean		isNonIssueSnap;			// confirmed to be delta-able from

	qboolean		snapSkipped;			// for DemoOptimizer. to not delta from skipped snaps

	int				serverCommandNum;		// execute all commands up to this before
											// making the snapshot current

	int				number_of_sounds; // MOHAA
	server_sound_t	sounds[64]; // MOHAA
} clSnapshot_t;

typedef struct usercmd_s {
	int				serverTime;
	int				angles[3];
	int 			buttons;
	byte			weapon;           // weapon 
	byte			forcesel;
	byte			invensel;
	byte			generic_cmd;
	signed char	forwardmove, rightmove, upmove;
} usercmd_t;



usercmd_t CG_DirToCmd(int moveDir);


// Be extra(!!) careful here because of the unions.
// TODO: This was done kinda manually after the fact. Try and automate this union making stuff somehow.
typedef struct entityState_s {

	// shared in both MOHAA and other games
	int		number;			// entity index
	int		eType;			// entityType_t
	int		eFlags;
	trajectory_t	pos;	// for calculating position
	trajectory_t	apos;	// for calculating angles
	int		modelindex;
	int		constantLight;	// r + (g<<8) + (b<<16) + (intensity<<24)
	int		solid;			// for client side prediction, trap_linkentity sets this properly
	vec3_t	origin;
	vec3_t	origin2;
	int		loopSound;		// constantly loop this sound
	int		groundEntityNum;	// -1 = in air
	int		clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	

	union { 

		// The extra fields of MOHAA alone are as big as the extra fields of many other games combined. So we unionize MOHAA extra fields and extra fields of other games. Since an entitystate can only belong to one game at a time
		// Let's just hope this doesn't come back to bite us in the ass. But saving a lot of memory this way seems like an alluring idea.
		
		// Non-MOHAA games:
		struct { // 396 bytes (2023-08-08), slightly below the MOHAA struct.
			
			// JK2:
			int		time;
			int		time2;

			vec3_t	angles;
			vec3_t	angles2;

			//rww - these were originally because we shared g2 info client and server side. Now they
			//just get used as generic values everywhere.
			int		bolt1;
			int		bolt2;

			//rww - this is necessary for determining player visibility during a jedi mindtrick
			int		trickedentindex; //0-15
			int		trickedentindex2; //16-32
			int		trickedentindex3; //33-48
			int		trickedentindex4; //49-64

			float	speed;

			int		fireflag;

			int		genericenemyindex;

			int		activeForcePass;

			int		emplacedOwner;

			int		otherEntityNum;	// shotgun sources, etc
			int		otherEntityNum2;



			int		modelGhoul2;
			int		g2radius;
			int		modelindex2;
			int		frame;

			qboolean	saberInFlight;
			int			saberEntityNum;
			int			saberMove;
			int			forcePowersActive;

			qboolean	isJediMaster;


			int		event;			// impulse events -- muzzle flashes, footsteps, etc
			int		eventParm;

			// so crosshair knows what it's looking at
			int			owner;
			int			teamowner;
			qboolean	shouldtarget;

			// for players
			int		powerups;		// bit flags
			int		weapon;			// determines weapon and flash model, etc
			int		legsAnim;		// mask off ANIM_TOGGLEBIT
			int		torsoAnim;		// mask off ANIM_TOGGLEBIT

			int		forceFrame;		//if non-zero, force the anim frame

			int		generic1;


			union {

				// Shared between JKA and other games:
				int			health;

				// JKA
				struct {
					// JKA-specific:
					int		eFlags2;		// EF2_??? used much less frequently
					qboolean	loopIsSoundset; //qtrue if the loopSound index is actually a soundset index
					int		soundSetIndex;
					int			saberHolstered;//sent in only only 2 bits - should be 0, 1 or 2
					qboolean	isPortalEnt; //this needs to be seperate for all entities I guess, which is why I couldn't reuse another value.
					qboolean	legsFlip; //set to opposite when the same anim needs restarting, sent over in only 1 bit. Cleaner and makes porting easier than having that god forsaken ANIM_TOGGLEBIT.
					qboolean	torsoFlip;
					int		heldByClient; //can only be a client index - this client should be holding onto my arm using IK stuff.
					int		ragAttach; //attach to ent while ragging
					int		iModelScale; //rww - transfer a percentage of the normal scale in a single int instead of 3 x-y-z scale values
					int		brokenLimbs;
					int		boltToPlayer; //set to index of a real client+1 to bolt the ent to that client. Must be a real client, NOT an NPC.
					//for looking at an entity's origin (NPCs and players)
					qboolean	hasLookTarget;
					int			lookTarget;
					int			customRGBA[4];
					//I didn't want to do this, but I.. have no choice. However, we aren't setting this for all ents or anything,
					//only ones we want health knowledge about on cgame (like siege objective breakables) -rww
					
					int			maxhealth; //so I know how to draw the stupid health bar
					//NPC-SPECIFIC FIELDS
					//------------------------------------------------------------
					int		npcSaber1;
					int		npcSaber2;
					//index values for each type of sound, gets the folder the sounds
					//are in. I wish there were a better way to do this,
					int		csSounds_Std;
					int		csSounds_Combat;
					int		csSounds_Extra;
					int		csSounds_Jedi;
					int		surfacesOn; //a bitflag of corresponding surfaces from a lookup table. These surfaces will be forced on.
					int		surfacesOff; //same as above, but forced off instead.
					//Allow up to 4 PCJ lookup values to be stored here.
					//The resolve to configstrings which contain the name of the
					//desired bone.
					int		boneIndex1;
					int		boneIndex2;
					int		boneIndex3;
					int		boneIndex4;
					//packed with x, y, z orientations for bone angles
					int		boneOrient;
					//I.. feel bad for doing this, but NPCs really just need to
					//be able to control this sort of thing from the server sometimes.
					//At least it's at the end so this stuff is never going to get sent
					//over for anything that isn't an NPC.
					vec3_t	boneAngles1; //angles of boneIndex1
					vec3_t	boneAngles2; //angles of boneIndex2
					vec3_t	boneAngles3; //angles of boneIndex3
					vec3_t	boneAngles4; //angles of boneIndex4
					int		NPC_class; //we need to see what it is on the client for a few effects.
					//If non-0, this is the index of the vehicle a player/NPC is riding.
					int		m_iVehicleNum;
					//rww - spare values specifically for use by mod authors.
					//See netf_overrides.txt if you want to increase the send
					//amount of any of these above 1 bit.
					int			userInt1;
					int			userInt2;
					int			userInt3;
					float		userFloat1;
					float		userFloat2;
					float		userFloat3;
					vec3_t		userVec1;
					vec3_t		userVec2;
				};

				// Non JKA
				struct {
					// RTCW (according to uber demo tools)
					int dl_intensity;  // used for coronas 
					int eventSequence; // pmove generated events 
					int events[4];
					int eventParms[4];
					int density;       // for particle effects 
					// to pass along additional information for damage effects for players 
					// also used for cursorhints for non-player entities 
					int dmgFlags;
					int onFireStart;
					int onFireEnd;
					int aiChar;
					int teamNum;
					int effect1Time;
					int effect2Time;
					int effect3Time;
					int aiState;
					int animMovetype;  // clients can't derive movetype of other clients for anim scripting system 

					// ET
					int nextWeapon;


					// Quake Live (according to uber demo tools)
					int pos_gravity;  // part of idEntityStateBase::pos trajectory
					int apos_gravity; // part of idEntityStateBase::apos trajectory 
					int jumpTime;
					int doubleJumped; // qboolean 
					//int health; // Already exists through jka
					int armor;
					int location;


					// JK2 SP Stuff
					int		modelindex3;
					int		legsAnimTimer;	// don't change low priority animations on legs until this runs out
					int		torsoAnimTimer;	// don't change low priority animations on torso until this runs out
					int		scale;			//Scale players
					qboolean	saberActive;
					int		vehicleModel;	// For overriding your playermodel with a drivable vehicle
					vec3_t	modelScale;		// used to scale models in any axis
					//Ghoul2 stuff for jk2sp:
					int		radius;			// used for culling all the ghoul models attached to this ent NOTE - this is automatically scaled by Ghoul2 if/when you scale the model. This is a 100% size value
					int		boltInfo;		// info used for bolting entities to Ghoul2 models - NOT used for bolting ghoul2 models to themselves, more for stuff like bolting effects to ghoul2 models

				};
			};
			

		}; 




		// OpenMOHAA
		struct { // This struct alone is 412 bytes
			//vec3_t	netorigin;// Replacing this in netfields with the trajectory one
			//vec3_t	netangles; // Replacing this in netfields with the trajectory one
			float	loopSoundVolume;
			float	loopSoundMinDist;
			float	loopSoundMaxDist;
			float	loopSoundPitch;
			int		loopSoundFlags;
			int		parent;
			int		tag_num;
			qboolean	attach_use_angles;
			vec3_t		attach_offset;
			int		beam_entnum;
			int		usageIndex;
			int		skinNum;
			int		wasframe;
			frameInfoMOHAA_t frameInfo[MAX_FRAMEINFOS];
			float	actionWeight;
			int		bone_tag[NUM_BONE_CONTROLLERS];
			vec3_t	bone_angles[NUM_BONE_CONTROLLERS];
			byte	surfaces[32];
			float	alpha;
			int		renderfx;
			float	shader_data[2];
			float	shader_time;
			vec3_t	eyeVector;
			float	scaleMOHAA;
		};
	};


	// Demo Tools specific values, only used internally
	// Not exactly a super elegant way of doing it but eh,
	// we don't have the problem of needing these structs to be in sync between game,cgame,engine etc.
	demoToolsEntityData_t demoToolsData;
} entityState_t;

typedef struct {
	int			timeoutcount;		// it requres several frames in a timeout condition
									// to disconnect, preventing debugging breaks from
									// causing immediate disconnects on continue
	clSnapshot_t	snap;			// latest received from server
	clSnapshot_t	oldSnap;		// latest received from server

	int			serverTime;			// may be paused during play
	int			oldServerTime;		// to prevent time from flowing bakcwards
	int			oldFrameServerTime;	// to check tournament restarts
	int			serverTimeDelta;	// cl.serverTime = cls.realtime + cl.serverTimeDelta
									// this value changes as net lag varies

	float		serverFrameTime;	// MOHAA

	qboolean	extrapolatedSnapshot;	// set if any cgame frame has been forced to extrapolate
									// cleared when CL_AdjustTimeDelta looks at it
	qboolean	newSnapshots;		// set on parse of any valid packet
	qboolean	lastSnapshotFinishedParsing; // set during any parsesnapshot to qtrue or qfalse (different from newsnapshots which is set only to qtrue and then remains that)

	gameState_t	gameState;			// configstrings
	char		mapname[MAX_QPATH];	// extracted from CS_SERVERINFO

	int			parseEntitiesNum;	// index (not anded off) into cl_parse_entities[]

	int			mouseDx[2], mouseDy[2];	// added to by mouse events
	int			mouseIndex;
	int			joystickAxis[MAX_JOYSTICK_AXIS];	// set by joystick events

	// cgame communicates a few values to the client system
	int			cgameUserCmdValue;	// current weapon to add to usercmd_t
	vec3_t		cgameViewAngleForce;
	int			cgameViewAngleForceTime;
	float		cgameTurnExtentAdd;
	float		cgameTurnExtentSub;
	int			cgameTurnExtentTime;
	float		cgameSensitivity;

	int			cgameForceSelection;
	int			cgameInvenSelection;

	qboolean	gcmdSendValue;
	byte		gcmdValue;

	float		lastViewYaw;

	// cmds[cmdNumber] is the predicted command, [cmdNumber-1] is the last
	// properly generated command
	usercmd_t	cmds[CMD_BACKUP];	// each mesage will send several old cmds
	int			cmdNumber;			// incremented each frame, because multiple
									// frames may need to be packed into a single packet

	outPacket_t	outPackets[PACKET_BACKUP];	// information about each packet we have sent out

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  It is cleared to 0 upon entering each level.
	// the server sends a delta each frame which is added to the locally
	// tracked view angles to account for standing on rotating objects,
	// and teleport direction changes
	vec3_t		viewangles;

	int			serverId;			// included in each client message so the server
												// can tell if it is for a prior map_restart
	// big stuff at end of structure so most offsets are 15 bits or less

	entityState_t	entityBaselines[MAX_GENTITIES];	// for delta compression when not in previous frame

	entityState_t	parseEntities[MAX_PARSE_ENTITIES];

	clSnapshot_t	snapshots[PACKET_BACKUP];


	//char* mSharedMemory;
} clientActive_t;









#define	QDECL	__cdecl
#define	MAXPRINTMSG	4096

typedef int		fileHandle_t;

#define	MAX_RELIABLE_COMMANDS	512//128			// max string commands buffered for restransmit. heightened for MOH which likes to dump a lot of configstrings and stuff.

typedef enum {
	NA_BOT,
	NA_BAD,					// an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
} netadrtype_t;

typedef enum {
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct {
	netadrtype_t	type;

	byte	ip[4];
	byte	ipx[10];

	unsigned short	port;
} netadr_t;

typedef struct {
	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	netadr_t	remoteAddress;
	int			qport;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
} netchan_t;


#define MAX_HEIGHTMAP_SIZE	16000

typedef struct
{
	int			mType;
	int			mSide;
	vec3_t		mOrigin;

} rmAutomapSymbol_t;

#define	MAX_AUTOMAP_SYMBOLS	512

typedef struct {

	int			clientNum;
	int			lastPacketSentTime;			// for retransmits during connection
	int			lastPacketTime;				// for timeouts

	netadr_t	serverAddress;
	int			connectTime;				// for connection retransmits
	int			connectPacketCount;			// for display on connection dialog
	char		serverMessage[MAX_STRING_TOKENS];	// for display on connection dialog

	int			challenge;					// from the server to use for connecting
	int			checksumFeed;				// from the server for checksum calculations

	// these are our reliable messages that go to the server
	int			reliableSequence;
	int			reliableAcknowledge;		// the last one the server has executed
	char		reliableCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS_MAX];

	// server message (unreliable) and command (reliable) sequence
	// numbers are NOT cleared at level changes, but continue to
	// increase as long as the connection is valid

	// message sequence is used by both the network layer and the
	// delta compression layer
	int			serverMessageSequence;

	// reliable messages received from server
	int			serverCommandSequence;
	int			lastPreExecutedServerCommand;		// last server command grabbed or executed with CL_GetServerCommand
	int			lastExecutedServerCommand;		// last server command grabbed or executed with CL_GetServerCommand
	char		serverCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS_MAX];

	// file transfer from server
	fileHandle_t download;
	char		downloadTempName[MAX_OSPATH];
	char		downloadName[MAX_OSPATH];
	int			downloadNumber;
	int			downloadBlock;	// block we are waiting for
	int			downloadCount;	// how many bytes we got
	int			downloadSize;	// how many bytes we got
	char		downloadList[MAX_INFO_STRING]; // list of paks we need to download
	qboolean	downloadRestart;	// if true, we need to do another FS_Restart because we downloaded a pak

	// demo information
	char		demoName[MAX_QPATH];
	qboolean	spDemoRecording;
	qboolean	demorecording;
	qboolean	demoplaying;
	qboolean	demowaiting;	// don't record until a non-delta message is received
	qboolean	firstDemoFrameSkipped;
	fileHandle_t	demofile;
	qboolean	newDemoPlayer;
	qboolean	demoCheckFor103;
	qboolean	demoCheckProtocol;

	int			lastKnownCommandTime; // For DemoReaderLight

	// For DemoReader:
	int			firstSnapServerTime;

	// For Cutter and generally
	qboolean	lastValidSnapSet;
	int			lastValidSnap;
	 
	//qboolean	lastNonIssueSnapSet;
	//int			lastNonIssueSnap;

	int			timeDemoFrames;		// counter of rendered frames
	int			timeDemoStart;		// cls.realtime before first frame
	int			timeDemoBaseTime;	// each frame will be at this time + frameNum * 50

	//rwwRMG - added: (JKA specific)
	int					rmgSeed;
	int					rmgHeightMapSize;
	unsigned char		rmgHeightMap[MAX_HEIGHTMAP_SIZE];
	unsigned char		rmgFlattenMap[MAX_HEIGHTMAP_SIZE];
	rmAutomapSymbol_t	rmgAutomapSymbols[MAX_AUTOMAP_SYMBOLS];
	int					rmgAutomapSymbolCount;

	// big stuff at end of structure so most offsets are 15 bits or less
	netchan_t	netchan;

	float		aviDemoRemain;		// Used for accurate fps recording
} clientConnection_t;


typedef struct {
	qboolean	allowoverflow;	// if false, do a Com_Error
	qboolean	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	qboolean	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte* data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes

	qboolean	raw;			// raw, everything saved as integers
	std::vector<byte>*	dataRaw;
} msg_t;

void MSG_Init(msg_t* buf, byte* data, int length);
void MSG_InitRaw(msg_t* buf, std::vector<byte>* dataRaw);
void MSG_InitOOB(msg_t* buf, byte* data, int length);
void MSG_Clear(msg_t* buf);
void MSG_WriteData(msg_t* buf, const void* data, int length);
void MSG_Bitstream(msg_t* buf);
void MSG_PrintFieldErrors(demoType_t demoType);

struct usercmd_s;
struct entityState_s;
struct playerState_s;

void MSG_WriteBits(msg_t* msg, int value, int bits);

void MSG_WriteChar(msg_t* sb, int c);
void MSG_WriteByte(msg_t* sb, int c);
void MSG_WriteShort(msg_t* sb, int c);
void MSG_WriteSShort(msg_t* sb, int c);
void MSG_WriteLong(msg_t* sb, int c);
void MSG_WriteFloat(msg_t* sb, float f);
void MSG_WriteString(msg_t* sb, const char* s, demoType_t demoType);
void MSG_WriteBigString(msg_t* sb, const char* s, demoType_t demoType);
void MSG_WriteAngle16(msg_t* sb, float f);

void	MSG_BeginReading(msg_t* sb);
void	MSG_BeginReadingOOB(msg_t* sb);

int		MSG_ReadBits(msg_t* msg, int bits);

int		MSG_ReadChar(msg_t* sb);
int		MSG_ReadByte(msg_t* sb);
int		MSG_ReadShort(msg_t* sb);
int		MSG_ReadSShort(msg_t* sb);
int		MSG_ReadLong(msg_t* sb);
float	MSG_ReadFloat(msg_t* sb);
char* MSG_ReadString(msg_t* sb, demoType_t demoType, qboolean forceNonScrambled=qfalse);
char* MSG_ReadBigString(msg_t* sb, demoType_t demoType);
char* MSG_ReadStringLine(msg_t* sb);
float	MSG_ReadAngle16(msg_t* sb);
void	MSG_ReadData(msg_t* sb, void* buffer, int size);
int		MSG_LookaheadByte(msg_t* msg);


void MSG_WriteDeltaUsercmd(msg_t* msg, struct usercmd_s* from, struct usercmd_s* to);
void MSG_ReadDeltaUsercmd(msg_t* msg, struct usercmd_s* from, struct usercmd_s* to);

void MSG_WriteDeltaUsercmdKey(msg_t* msg, int key, usercmd_t* from, usercmd_t* to);
void MSG_ReadDeltaUsercmdKey(msg_t* msg, int key, usercmd_t* from, usercmd_t* to);

void MSG_WriteDeltaEntity(msg_t* msg, struct entityState_s* from, struct entityState_s* to
	, qboolean force, demoType_t demoType, float frameTime); // frameTime is for MOHAA
void MSG_ReadDeltaEntity(msg_t* msg, entityState_t* from, entityState_t* to,
	int number, demoType_t demoType, float frameTime); // frameTime is for MOHAA

void MSG_WriteDeltaPlayerstate(msg_t* msg, struct playerState_s* from, struct playerState_s* to, qboolean isVehiclePS, demoType_t demoType, float frameTime); // frametime is for MOHAA
void MSG_ReadDeltaPlayerstate(msg_t* msg, struct playerState_s* from, struct playerState_s* to, demoType_t demoType, qboolean isVehiclePS, playerState_t* currentPS=NULL);


void MSG_ReportChangeVectors_f(void);

// MOHAA
void MSG_GetNullEntityState(entityState_t* nullState);
unsigned short MSG_ReadEntityNum(msg_t* sb, demoType_t demoType);
float MSG_ReadServerFrameTime(msg_t* msg, demoType_t demoType, clientActive_t* clCut, qboolean forceConfigStringMethod);
void MSG_ReadSounds(msg_t* msg, server_sound_t* sounds, int* snapshot_number_of_sounds);
void MSG_WriteSounds(msg_t* msg, server_sound_t* sounds, int snapshot_number_of_sounds);
float MSG_ReadCoord(msg_t* msg);
void MSG_ReadDir(msg_t* msg, vec3_t dir);
void CL_ParseCGMessageMOHAA(msg_t* msg, demoType_t demoType);

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

#define NYT HMAX					/* NYT = Not Yet Transmitted */
#define INTERNAL_NODE (HMAX+1)

typedef struct nodetype {
	struct	nodetype* left, * right, * parent; /* tree structure */
	struct	nodetype* next, * prev; /* doubly-linked list */
	struct	nodetype** head; /* highest ranked node in block */
	int		weight;
	int		symbol;
} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct {
	int			blocNode;
	int			blocPtrs;

	node_t* tree;
	node_t* lhead;
	node_t* ltail;
	node_t* loc[HMAX + 1];
	node_t** freelist;

	node_t		nodeList[768];
	node_t* nodePtrs[768];
} huff_t;

typedef struct {
	huff_t		compressor;
	huff_t		decompressor;
} huffman_t;

void	Huff_Compress(msg_t* buf, int offset);
void	Huff_Decompress(msg_t* buf, int offset);
void	Huff_Init(huffman_t* huff);
void	Huff_addRef(huff_t* huff, byte ch);
int		Huff_Receive(node_t* node, int* ch, byte* fin);
void	Huff_transmit(huff_t* huff, int ch, byte* fout);
void	Huff_offsetReceive(node_t* node, int* ch, byte* fin, int* offset);
void	Huff_offsetTransmit(huff_t* huff, int ch, byte* fout, int* offset);
void	Huff_putBit(int bit, byte* fout, int* offset);
int		Huff_getBit(byte* fout, int* offset);

// don't use if you don't know what you're doing.
int		Huff_getBloc(void);
void	Huff_setBloc(int _bloc);

void Com_Memset(void* dest, const int val, const size_t count);
void Com_Memcpy(void* dest, const void* src, const size_t count);
void 		QDECL Com_Printf(const char* fmt, ...);
void 		QDECL Com_DPrintf(const char* fmt, ...);
void 		QDECL Com_Error(int code, const char* fmt, ...);

typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

void	Q_strncpyz(char* dest, int destCapacity, const char* src, int destsize);
char* Q_stristr(const char* str, char* charset);
char* QDECL va(const char* format, ...);

int		Cmd_Argc(void);
char* Cmd_Argv(int arg);

char* Cmd_ArgsFrom(int arg);







// VIDEO STUFF
#define COLOR_EXT_AMOUNT 16 // can be safely raised only to 32
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )
#define ColorIndex_Extended(c) ( ( (c) - '0' ) & (COLOR_EXT_AMOUNT - 1) ) // compatible with 1.02, 'a' & 15 = 1

#define Q_IsColorStringHex(p) ((Q_IsColorStringHexY((p))) || (Q_IsColorStringHexy((p))) || (Q_IsColorStringHexX((p))) || (Q_IsColorStringHexx((p)) ))
#define Q_IsColorStringHexY(p) (p) && *(p)=='Y' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4)) && Q_IsHex((p+5)) && Q_IsHex((p+6)) && Q_IsHex((p+7)) && Q_IsHex((p+8))
#define Q_IsColorStringHexy(p) (p) && *(p)=='y' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4))
#define Q_IsColorStringHexX(p) (p) && *(p)=='X' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4)) && Q_IsHex((p+5)) && Q_IsHex((p+6))
#define Q_IsColorStringHexx(p) (p) && *(p)=='x' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3))

#define Q_IsHex(p) ((p) && ((*(p) >= '0' && *(p) <= '9') || (*(p) >= 'a' && *(p) <= 'f') || (*(p) >= 'A' && *(p) <= 'F')))

qboolean Q_parseColorHex(const char* p, float* color, int* skipCount);
extern const vec4_t	g_color_table[COLOR_EXT_AMOUNT];
extern vec4_t	g_color_table_nt[128];










enum fileCompressionScheme_t {
	FILECOMPRESSION_NONE, // Normal default file handling
	FILECOMPRESSION_RAW, // The special compressed file format but without actually using any compression
	FILECOMPRESSION_LZMA // The special compressed file format with LZMA compression
};

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

int		FS_Seek(fileHandle_t f, int64_t offset, int origin);
int64_t		FS_FTell(fileHandle_t f);
int64_t		FS_Write(const void* buffer, int64_t len, fileHandle_t f, qboolean ignoreCompression=qfalse);
//fileHandle_t	FS_FOpenFileWrite(const char* qpath, qboolean quiet = qfalse, fileCompressionScheme_t compression= FILECOMPRESSION_NONE); // Compressedtype has an int at the start corresponding to fileCompressionScheme_t
fileHandle_t	FS_FOpenFileWrite(const char* qpath, fileCompressionScheme_t compression, qboolean quiet = qfalse); // Compressedtype has an int at the start corresponding to fileCompressionScheme_t
int64_t		FS_Read(msg_t* msg, fileHandle_t f, qboolean ignoreCompression = qfalse);
int64_t		FS_Read(void* buffer, int64_t len, fileHandle_t f, qboolean ignoreCompression = qfalse);
qboolean FS_FileExists(const char* file);
qboolean FS_FileErase(const char* file);
void	FS_FCloseFile(fileHandle_t f);
//int		FS_FOpenFileRead(const char* qpath, fileHandle_t* file, qboolean uniqueFILE, qboolean compressedType = qfalse, fileCompressionScheme_t* compressionUsed=NULL);
int64_t	FS_FOpenFileRead(const char* qpath, fileHandle_t* file, qboolean uniqueFILE, qboolean compressedType, fileCompressionScheme_t* compressionUsed=NULL, qboolean nonExclusiveRead=qfalse); 

void	QDECL Com_sprintf(char* dest,int size, const char* fmt, ...);

void	Cmd_TokenizeString(const char* text);

/*enum svc_ops_e {
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring,			// [short] [string] only in gamestate messages
	svc_baseline,				// only in gamestate messages
	svc_serverCommand,			// [string] to be executed by client game module
	svc_download,				// [short] size [size bytes]
	svc_snapshot,
	svc_mapchange,

	svc_EOF
};*/
enum svc_ops_e_general {
	svc_bad_general,
	svc_nop_general,
	svc_gamestate_general,
	svc_configstring_general,			// [short] [string] only in gamestate messages
	svc_baseline_general,				// only in gamestate messages
	svc_serverCommand_general,			// [string] to be executed by client game module
	svc_download_general,				// [short] size [size bytes]
	svc_snapshot_general,
	svc_setgame_general,
	svc_mapchange_general,

	// JKA Xbox specific: We ignore these for now.
	svc_newpeer_general,				//jsw//inform current clients about new player
	svc_removepeer_general,				//jsw//inform current clients about dying player
	svc_xbInfo_general,					//jsw//update client with current server xbOnlineInfo

	// Quake Live
	svc_extension_general,
	svc_voip_general,

	// MOHAA
	svc_centerprint_general,
	svc_locprint_general,
	svc_cgameMessage_general,

	svc_EOF_general,
	svc_ops_general_count
};

char* Info_ValueForKey(const char* s,int maxLength, const char* key);
qboolean Info_HasKey(const char* s, int maxLength, const char* key);
char* Info_ValueForKey_Exists(const char* s, int maxLength, const char* key, qboolean* exists);
void Info_RemoveKey(char* s, const char* key, bool isMOHAADemo);
void Info_RemoveKey_Big(char* s, const char* key);
qboolean Info_SetValueForKey(char* s, int capacity, const char* key, const char* value, bool isMOHAADemo);
void Info_SetValueForKey_Big(char* s, int capacity, const char* key, const char* value);
int Info_ApplyOverrides(const char* overrides, char* data, int maxLengthOverrides, int maxLengthData);


// entityState_t->event values
// entity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

#define	EVENT_VALID_MSEC	300

typedef enum {
	GT_FFA_GENERAL,				// free for all
	GT_HOLOCRON_GENERAL,		// holocron ffa
	GT_JEDIMASTER_GENERAL,		// jedi master
	GT_TOURNAMENT_GENERAL,		// one on one tournament
	GT_SINGLE_PLAYER_GENERAL,	// single player ffa


	GT_POWERDUEL_GENERAL, // jka
	GT_RACE_GENERAL, // ql racing
	GT_HM_GENERAL, // cpma? this seems to be an improved duel mode

	//-- team games go after this --

	GT_TEAM_GENERAL,			// team deathmatch
	GT_SAGA_SIEGE_GENERAL,			// saga/siege
	GT_CTF_GENERAL,				// capture the flag
	GT_CTY_GENERAL,
	GT_1FCTF_GENERAL,//q3
	GT_OBELISK_GENERAL,//q3
	GT_HARVESTER_GENERAL, //q3
	GT_TEAM_ROUNDS_GENERAL, //moh
	GT_OBJECTIVE_GENERAL, //moh
	GT_TOW_GENERAL, //moh
	GT_LIBERATION_GENERAL, //moh
	GT_CA_GENERAL, // ql idk
	GT_FT_GENERAL, // ql freezetag
	GT_DOMINATION_GENERAL, // ql
	GT_CTFS_GENERAL, // ql attack and defend
	GT_REDROVER_GENERAL, // ql - is this really a team game by typical definition? meh i guess so
	GT_NTF_GENERAL, // cpma not team fortress
	GT_TWO_VS_TWO_GENERAL, // cpma?
	GT_CBTDM_GENERAL, // idk cod?	

	GT_MAX_GAME_TYPE_GENERAL
} gametype_general_t;


typedef struct enumName_s {
	int	value; // wasteful but i only do this to make sure I don't mistype stuff
	const char* name;
} enumName_t;

#define ENUMENTRY(a) { a , #a }

typedef enum {
	PM_NORMAL_GENERAL,		// can accelerate and turn
	PM_CLIMBWALL_GENERAL,   // MOHAA
	PM_JETPACK_GENERAL,		// special jetpack movement (JKA)
	PM_FLOAT_GENERAL,		// float with no gravity in general direction of velocity (intended for gripping) (JK2/JKA)
	PM_NOCLIP_GENERAL,		// noclip movement
	PM_SPECTATOR_GENERAL,	// still run into walls
	PM_DEAD_GENERAL,		// no acceleration or turning, but free falling
	PM_FREEZE_GENERAL,		// stuck in place with no control
	PM_INTERMISSION_GENERAL,	// no movement or status bar
	PM_SPINTERMISSION_GENERAL,	// no movement or status bar
	PM_TYPES_COUNT
} pmtypeGeneral_t;

static const int jk2pmTypeToGeneral[]{
	PM_NORMAL_GENERAL,		// can accelerate and turn
	PM_FLOAT_GENERAL,		// float with no gravity in general direction of velocity (intended for gripping)
	PM_NOCLIP_GENERAL,		// noclip movement
	PM_SPECTATOR_GENERAL,	// still run into walls
	PM_DEAD_GENERAL,		// no acceleration or turning, but free falling
	PM_FREEZE_GENERAL,		// stuck in place with no control
	PM_INTERMISSION_GENERAL,	// no movement or status bar
	PM_SPINTERMISSION_GENERAL	// no movement or status bar
};

//
// entityState_t->eType
//
typedef enum {
	ET_GENERAL_GENERAL,
	ET_PLAYER_GENERAL,
	ET_ITEM_GENERAL,
	ET_MISSILE_GENERAL,
	ET_SPECIAL_GENERAL,
	ET_HOLOCRON_GENERAL,
	ET_MOVER_GENERAL,
	ET_BEAM_GENERAL,
	ET_PORTAL_GENERAL,
	ET_SPEAKER_GENERAL,
	ET_PUSH_TRIGGER_GENERAL,
	ET_TELEPORT_TRIGGER_GENERAL,
	ET_INVISIBLE_GENERAL,
	ET_GRAPPLE_GENERAL,
	ET_TEAM_GENERAL,
	ET_BODY_GENERAL,

	// JKA
	ET_NPC_GENERAL,
	ET_TERRAIN_GENERAL,
	ET_FX_GENERAL,

	// JK2SP
	ET_THINKER_GENERAL,
	ET_CLOUD_GENERAL,

	// MOHAA
	ET_MODELANIM_SKEL_GENERAL,
	ET_MODELANIM_GENERAL,
	ET_VEHICLE_GENERAL,
	ET_MULTIBEAM_GENERAL,
	ET_EVENT_ONLY_GENERAL,
	ET_RAIN_GENERAL,
	ET_LEAF_GENERAL,
	ET_DECAL_GENERAL,
	ET_EMITTER_GENERAL,
	ET_ROPE_GENERAL,
	ET_EXEC_COMMANDS_GENERAL,

	ET_EVENTS_GENERAL
} entityType_general_t;

extern enumName_t entityTypesGeneralNames[ET_EVENTS_GENERAL];

static const int jk2GametypeToGeneral[] {
	GT_FFA_GENERAL,				// free for all
	GT_HOLOCRON_GENERAL,		// holocron ffa
	GT_JEDIMASTER_GENERAL,		// jedi master
	GT_TOURNAMENT_GENERAL,		// one on one tournament
	GT_SINGLE_PLAYER_GENERAL,	// single player ffa

	//-- team games go after this --

	GT_TEAM_GENERAL,			// team deathmatch
	GT_SAGA_SIEGE_GENERAL,			// saga
	GT_CTF_GENERAL,				// capture the flag
	GT_CTY_GENERAL,
	GT_MAX_GAME_TYPE_GENERAL
};
static const int jk2EntityTypeToGeneral[] {
	ET_GENERAL_GENERAL,
	ET_PLAYER_GENERAL,
	ET_ITEM_GENERAL,
	ET_MISSILE_GENERAL,
	ET_SPECIAL_GENERAL,				// rww - force fields
	ET_HOLOCRON_GENERAL,			// rww - holocron icon displays
	ET_MOVER_GENERAL,
	ET_BEAM_GENERAL,
	ET_PORTAL_GENERAL,
	ET_SPEAKER_GENERAL,
	ET_PUSH_TRIGGER_GENERAL,
	ET_TELEPORT_TRIGGER_GENERAL,
	ET_INVISIBLE_GENERAL,
	ET_GRAPPLE_GENERAL,				// grapple hooked on wall
	ET_TEAM_GENERAL,
	ET_BODY_GENERAL,

	ET_EVENTS_GENERAL				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
};

typedef enum {
	ET_GENERAL_JK2,
	ET_PLAYER_JK2,
	ET_ITEM_JK2,
	ET_MISSILE_JK2,
	ET_SPECIAL_JK2,				// rww - force fields
	ET_HOLOCRON_JK2,			// rww - holocron icon displays
	ET_MOVER_JK2,
	ET_BEAM_JK2,
	ET_PORTAL_JK2,
	ET_SPEAKER_JK2,
	ET_PUSH_TRIGGER_JK2,
	ET_TELEPORT_TRIGGER_JK2,
	ET_INVISIBLE_JK2,
	ET_GRAPPLE_JK2,				// grapple hooked on wall
	ET_TEAM_JK2,
	ET_BODY_JK2,

	ET_EVENTS_JK2				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityType_jk2_t;


typedef enum {
	EV_NONE_JK2,

	EV_CLIENTJOIN_JK2,

	EV_FOOTSTEP_JK2,
	EV_FOOTSTEP_METAL_JK2,
	EV_FOOTSPLASH_JK2,
	EV_FOOTWADE_JK2,
	EV_SWIM_JK2,

	EV_STEP_4_JK2,
	EV_STEP_8_JK2,
	EV_STEP_12_JK2,
	EV_STEP_16_JK2,

	EV_FALL_JK2,

	EV_JUMP_PAD_JK2,			// boing sound at origin_JK2, jump sound on player

	EV_PRIVATE_DUEL_JK2,

	EV_JUMP_JK2,
	EV_ROLL_JK2,
	EV_WATER_TOUCH_JK2,	// foot touches
	EV_WATER_LEAVE_JK2,	// foot leaves
	EV_WATER_UNDER_JK2,	// head touches
	EV_WATER_CLEAR_JK2,	// head leaves

	EV_ITEM_PICKUP_JK2,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_JK2,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO_JK2,
	EV_CHANGE_WEAPON_JK2,
	EV_FIRE_WEAPON_JK2,
	EV_ALT_FIRE_JK2,
	EV_SABER_ATTACK_JK2,
	EV_SABER_HIT_JK2,
	EV_SABER_BLOCK_JK2,
	EV_SABER_UNHOLSTER_JK2,
	EV_BECOME_JEDIMASTER_JK2,
	EV_DISRUPTOR_MAIN_SHOT_JK2,
	EV_DISRUPTOR_SNIPER_SHOT_JK2,
	EV_DISRUPTOR_SNIPER_MISS_JK2,
	EV_DISRUPTOR_HIT_JK2,
	EV_DISRUPTOR_ZOOMSOUND_JK2,

	EV_PREDEFSOUND_JK2,

	EV_TEAM_POWER_JK2,

	EV_SCREENSHAKE_JK2,

	EV_USE_JK2,			// +Use key

	EV_USE_ITEM0_JK2,
	EV_USE_ITEM1_JK2,
	EV_USE_ITEM2_JK2,
	EV_USE_ITEM3_JK2,
	EV_USE_ITEM4_JK2,
	EV_USE_ITEM5_JK2,
	EV_USE_ITEM6_JK2,
	EV_USE_ITEM7_JK2,
	EV_USE_ITEM8_JK2,
	EV_USE_ITEM9_JK2,
	EV_USE_ITEM10_JK2,
	EV_USE_ITEM11_JK2,
	EV_USE_ITEM12_JK2,
	EV_USE_ITEM13_JK2,
	EV_USE_ITEM14_JK2,
	EV_USE_ITEM15_JK2,

	EV_ITEMUSEFAIL_JK2,

	EV_ITEM_RESPAWN_JK2,
	EV_ITEM_POP_JK2,
	EV_PLAYER_TELEPORT_IN_JK2,
	EV_PLAYER_TELEPORT_OUT_JK2,

	EV_GRENADE_BOUNCE_JK2,		// eventParm will be the soundindex
	EV_MISSILE_STICK_JK2,		// eventParm will be the soundindex

	EV_PLAY_EFFECT_JK2,
	EV_PLAY_EFFECT_ID_JK2,

	EV_MUTE_SOUND_JK2,
	EV_GENERAL_SOUND_JK2,
	EV_GLOBAL_SOUND_JK2,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_JK2,
	EV_ENTITY_SOUND_JK2,

	EV_PLAY_ROFF_JK2,

	EV_GLASS_SHATTER_JK2,
	EV_DEBRIS_JK2,

	EV_MISSILE_HIT_JK2,
	EV_MISSILE_MISS_JK2,
	EV_MISSILE_MISS_METAL_JK2,
	EV_BULLET_JK2,				// otherEntity is the shooter

	EV_PAIN_JK2,
	EV_DEATH1_JK2,
	EV_DEATH2_JK2,
	EV_DEATH3_JK2,
	EV_OBITUARY_JK2,

	EV_POWERUP_QUAD_JK2,
	EV_POWERUP_BATTLESUIT_JK2,
	//EV_POWERUP_REGEN_JK2,

	EV_FORCE_DRAINED_JK2,

	EV_GIB_PLAYER_JK2,			// gib a previously living player
	EV_SCOREPLUM_JK2,			// score plum

	EV_CTFMESSAGE_JK2,

	EV_SAGA_ROUNDOVER_JK2,
	EV_SAGA_OBJECTIVECOMPLETE_JK2,

	EV_DESTROY_GHOUL2_INSTANCE_JK2,

	EV_DESTROY_WEAPON_MODEL_JK2,

	EV_GIVE_NEW_RANK_JK2,
	EV_SET_FREE_SABER_JK2,
	EV_SET_FORCE_DISABLE_JK2,

	EV_WEAPON_CHARGE_JK2,
	EV_WEAPON_CHARGE_ALT_JK2,

	EV_SHIELD_HIT_JK2,

	EV_DEBUG_LINE_JK2,
	EV_TESTLINE_JK2,
	EV_STOPLOOPINGSOUND_JK2,
	EV_STARTLOOPINGSOUND_JK2,
	EV_TAUNT_JK2,
	EV_TAUNT_YES_JK2,
	EV_TAUNT_NO_JK2,
	EV_TAUNT_FOLLOWME_JK2,
	EV_TAUNT_GETFLAG_JK2,
	EV_TAUNT_GUARDBASE_JK2,
	EV_TAUNT_PATROL_JK2,

	EV_BODY_QUEUE_COPY_JK2,
} entity_event_jk2_t;

typedef enum {
	EV_NONE_GENERAL,

	EV_CLIENTJOIN_GENERAL,

	EV_FOOTSTEP_GENERAL,
	EV_FOOTSTEP_METAL_GENERAL,
	EV_FOOTSPLASH_GENERAL,
	EV_FOOTWADE_GENERAL,
	EV_SWIM_GENERAL,

	EV_STEP_4_GENERAL,
	EV_STEP_8_GENERAL,
	EV_STEP_12_GENERAL,
	EV_STEP_16_GENERAL,

	EV_FALL_GENERAL,

	EV_JUMP_PAD_GENERAL,			// boing sound at origin_GENERAL, jump sound on player

	EV_PRIVATE_DUEL_GENERAL,

	EV_JUMP_GENERAL,
	EV_ROLL_GENERAL,
	EV_WATER_TOUCH_GENERAL,	// foot touches
	EV_WATER_LEAVE_GENERAL,	// foot leaves
	EV_WATER_UNDER_GENERAL,	// head touches
	EV_WATER_CLEAR_GENERAL,	// head leaves

	EV_ITEM_PICKUP_GENERAL,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_GENERAL,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_FIRE_WEAPON_GENERAL,
	EV_ALT_FIRE_GENERAL,
	EV_SABER_ATTACK_GENERAL,
	EV_SABER_HIT_GENERAL,
	EV_SABER_BLOCK_GENERAL,
	EV_SABER_UNHOLSTER_GENERAL,
	EV_BECOME_JEDIMASTER_GENERAL,
	EV_DISRUPTOR_MAIN_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_MISS_GENERAL,
	EV_DISRUPTOR_HIT_GENERAL,
	EV_DISRUPTOR_ZOOMSOUND_GENERAL,

	EV_PREDEFSOUND_GENERAL,

	EV_TEAM_POWER_GENERAL,

	EV_SCREENSHAKE_GENERAL,

	EV_USE_GENERAL,			// +Use key

	EV_USE_ITEM0_GENERAL,
	EV_USE_ITEM1_GENERAL,
	EV_USE_ITEM2_GENERAL,
	EV_USE_ITEM3_GENERAL,
	EV_USE_ITEM4_GENERAL,
	EV_USE_ITEM5_GENERAL,
	EV_USE_ITEM6_GENERAL,
	EV_USE_ITEM7_GENERAL,
	EV_USE_ITEM8_GENERAL,
	EV_USE_ITEM9_GENERAL,
	EV_USE_ITEM10_GENERAL,
	EV_USE_ITEM11_GENERAL,
	EV_USE_ITEM12_GENERAL,
	EV_USE_ITEM13_GENERAL,
	EV_USE_ITEM14_GENERAL,
	EV_USE_ITEM15_GENERAL,

	EV_ITEMUSEFAIL_GENERAL,

	EV_ITEM_RESPAWN_GENERAL,
	EV_ITEM_POP_GENERAL,
	EV_PLAYER_TELEPORT_IN_GENERAL,
	EV_PLAYER_TELEPORT_OUT_GENERAL,

	EV_GRENADE_BOUNCE_GENERAL,		// eventParm will be the soundindex
	EV_MISSILE_STICK_GENERAL,		// eventParm will be the soundindex

	EV_PLAY_EFFECT_GENERAL,
	EV_PLAY_EFFECT_ID_GENERAL,

	EV_MUTE_SOUND_GENERAL,
	EV_GENERAL_SOUND_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_GENERAL,
	EV_ENTITY_SOUND_GENERAL,

	EV_PLAY_ROFF_GENERAL,

	EV_GLASS_SHATTER_GENERAL,
	EV_DEBRIS_GENERAL,

	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,
	EV_MISSILE_MISS_METAL_GENERAL,
	EV_BULLET_GENERAL,				// otherEntity is the shooter

	EV_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,
	EV_OBITUARY_GENERAL,

	EV_POWERUP_QUAD_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	//EV_POWERUP_REGEN_GENERAL,

	EV_FORCE_DRAINED_GENERAL,

	EV_GIB_PLAYER_GENERAL,			// gib a previously living player
	EV_SCOREPLUM_GENERAL,			// score plum

	EV_CTFMESSAGE_GENERAL,

	EV_SAGA_ROUNDOVER_GENERAL,
	EV_SAGA_OBJECTIVECOMPLETE_GENERAL,

	EV_DESTROY_GHOUL2_INSTANCE_GENERAL,

	EV_DESTROY_WEAPON_MODEL_GENERAL,

	EV_GIVE_NEW_RANK_GENERAL,
	EV_SET_FREE_SABER_GENERAL,
	EV_SET_FORCE_DISABLE_GENERAL,

	EV_WEAPON_CHARGE_GENERAL,
	EV_WEAPON_CHARGE_ALT_GENERAL,

	EV_SHIELD_HIT_GENERAL,

	EV_DEBUG_LINE_GENERAL,
	EV_TESTLINE_GENERAL,
	EV_STOPLOOPINGSOUND_GENERAL,
	EV_STARTLOOPINGSOUND_GENERAL,
	EV_TAUNT_GENERAL,
	EV_TAUNT_YES_GENERAL,
	EV_TAUNT_NO_GENERAL,
	EV_TAUNT_FOLLOWME_GENERAL,
	EV_TAUNT_GETFLAG_GENERAL,
	EV_TAUNT_GUARDBASE_GENERAL,
	EV_TAUNT_PATROL_GENERAL,

	EV_BODY_QUEUE_COPY_GENERAL,


	// JKA specific
	EV_GHOUL2_MARK_GENERAL,
	EV_GLOBAL_DUEL_GENERAL,
	EV_VEH_FIRE_GENERAL,
	EV_SABER_CLASHFLARE_GENERAL,
	EV_LOCALTIMER_GENERAL,
	EV_PLAY_PORTAL_EFFECT_ID_GENERAL,
	EV_PLAYDOORSOUND_GENERAL,
	EV_PLAYDOORLOOPSOUND_GENERAL,
	EV_BMODEL_SOUND_GENERAL,
	EV_VOICECMD_SOUND_GENERAL,
	EV_MISC_MODEL_EXP_GENERAL,
	EV_CONC_ALT_IMPACT_GENERAL,
	EV_BODYFADE_GENERAL,
	EV_SIEGE_ROUNDOVER_GENERAL,
	EV_SIEGE_OBJECTIVECOMPLETE_GENERAL,
	//rww - Begin NPC sound events
	EV_ANGER1_GENERAL,
	EV_ANGER2_GENERAL,
	EV_ANGER3_GENERAL,
	EV_VICTORY1_GENERAL,
	EV_VICTORY2_GENERAL,
	EV_VICTORY3_GENERAL,
	EV_CONFUSE1_GENERAL,
	EV_CONFUSE2_GENERAL,
	EV_CONFUSE3_GENERAL,
	EV_PUSHED1_GENERAL,
	EV_PUSHED2_GENERAL,
	EV_PUSHED3_GENERAL,
	EV_CHOKE1_GENERAL,
	EV_CHOKE2_GENERAL,
	EV_CHOKE3_GENERAL,
	EV_FFWARN_GENERAL,
	EV_FFTURN_GENERAL,
	//extra sounds for ST
	EV_CHASE1_GENERAL,
	EV_CHASE2_GENERAL,
	EV_CHASE3_GENERAL,
	EV_COVER1_GENERAL,
	EV_COVER2_GENERAL,
	EV_COVER3_GENERAL,
	EV_COVER4_GENERAL,
	EV_COVER5_GENERAL,
	EV_DETECTED1_GENERAL,
	EV_DETECTED2_GENERAL,
	EV_DETECTED3_GENERAL,
	EV_DETECTED4_GENERAL,
	EV_DETECTED5_GENERAL,
	EV_LOST1_GENERAL,
	EV_OUTFLANK1_GENERAL,
	EV_OUTFLANK2_GENERAL,
	EV_ESCAPING1_GENERAL,
	EV_ESCAPING2_GENERAL,
	EV_ESCAPING3_GENERAL,
	EV_GIVEUP1_GENERAL,
	EV_GIVEUP2_GENERAL,
	EV_GIVEUP3_GENERAL,
	EV_GIVEUP4_GENERAL,
	EV_LOOK1_GENERAL,
	EV_LOOK2_GENERAL,
	EV_SIGHT1_GENERAL,
	EV_SIGHT2_GENERAL,
	EV_SIGHT3_GENERAL,
	EV_SOUND1_GENERAL,
	EV_SOUND2_GENERAL,
	EV_SOUND3_GENERAL,
	EV_SUSPICIOUS1_GENERAL,
	EV_SUSPICIOUS2_GENERAL,
	EV_SUSPICIOUS3_GENERAL,
	EV_SUSPICIOUS4_GENERAL,
	EV_SUSPICIOUS5_GENERAL,
	//extra sounds for Jedi
	EV_COMBAT1_GENERAL,
	EV_COMBAT2_GENERAL,
	EV_COMBAT3_GENERAL,
	EV_JDETECTED1_GENERAL,
	EV_JDETECTED2_GENERAL,
	EV_JDETECTED3_GENERAL,
	EV_TAUNT1_GENERAL,
	EV_TAUNT2_GENERAL,
	EV_TAUNT3_GENERAL,
	EV_JCHASE1_GENERAL,
	EV_JCHASE2_GENERAL,
	EV_JCHASE3_GENERAL,
	EV_JLOST1_GENERAL,
	EV_JLOST2_GENERAL,
	EV_JLOST3_GENERAL,
	EV_DEFLECT1_GENERAL,
	EV_DEFLECT2_GENERAL,
	EV_DEFLECT3_GENERAL,
	EV_GLOAT1_GENERAL,
	EV_GLOAT2_GENERAL,
	EV_GLOAT3_GENERAL,
	EV_PUSHFAIL_GENERAL,
	EV_SIEGESPEC_GENERAL,

	// Q3 specific
	EV_FALL_SHORT_GENERAL,
	EV_FALL_MEDIUM_GENERAL,
	EV_FALL_FAR_GENERAL,
	EV_BULLET_HIT_FLESH_GENERAL,
	EV_BULLET_HIT_WALL_GENERAL,
	EV_RAILTRAIL_GENERAL,
	EV_SHOTGUN_GENERAL,
	EV_POWERUP_REGEN_GENERAL,
	EV_PROXIMITY_MINE_STICK_GENERAL,
	EV_PROXIMITY_MINE_TRIGGER_GENERAL,
	EV_KAMIKAZE_GENERAL,
	EV_OBELISKEXPLODE_GENERAL,
	EV_OBELISKPAIN_GENERAL,
	EV_INVUL_IMPACT_GENERAL,
	EV_JUICED_GENERAL,
	EV_LIGHTNINGBOLT_GENERAL,

	// QL Specific
	EV_DROP_WEAPON_GENERAL,
	EV_DROWN_GENERAL,
	EV_POWERUP_ARMOR_REGEN_GENERAL,
	EV_FOOTSTEP_SNOW_GENERAL,
	EV_FOOTSTEP_WOOD_GENERAL,
	EV_ITEM_PICKUP_SPEC_GENERAL,
	EV_OVERTIME_GENERAL,
	EV_GAMEOVER_GENERAL,
	EV_THAW_PLAYER_GENERAL,
	EV_THAW_TICK_GENERAL,
	EV_HEADSHOT_GENERAL,
	EV_POI_GENERAL,
	EV_RACE_START_GENERAL,
	EV_RACE_CHECKPOINT_GENERAL,
	EV_RACE_END_GENERAL,
	EV_DAMAGEPLUM_GENERAL,
	EV_AWARD_GENERAL,
	EV_INFECTED_GENERAL,
	EV_NEW_HIGH_SCORE_GENERAL,
	EV_STEP_20_GENERAL,
	EV_STEP_24_GENERAL,

	// ET specific:
	EV_FOOTSTEP_GRASS_GENERAL,
	EV_FOOTSTEP_GRAVEL_GENERAL,
	EV_FOOTSTEP_ROOF_GENERAL,
	EV_FOOTSTEP_CARPET_GENERAL,
	EV_FALL_NDIE_GENERAL,
	EV_FALL_DMG_10_GENERAL,
	EV_FALL_DMG_15_GENERAL,
	EV_FALL_DMG_25_GENERAL,
	EV_FALL_DMG_50_GENERAL,
	EV_ITEM_PICKUP_QUIET_GENERAL,   // (SA) same_GENERAL, but don't play the default pickup sound as it was specified in the ent
	EV_WEAPONSWITCHED_GENERAL,
	EV_EMPTYCLIP_GENERAL,
	EV_FILL_CLIP_GENERAL,
	EV_MG42_FIXED_GENERAL, // JPW NERVE
	EV_WEAP_OVERHEAT_GENERAL,
	EV_CHANGE_WEAPON_2_GENERAL,
	EV_FIRE_WEAPONB_GENERAL,
	EV_FIRE_WEAPON_LASTSHOT_GENERAL,
	EV_NOFIRE_UNDERWATER_GENERAL,
	EV_FIRE_WEAPON_MG42_GENERAL,
	EV_FIRE_WEAPON_MOUNTEDMG42_GENERAL,
	EV_GENERAL_SOUND_VOLUME_GENERAL,
	EV_GLOBAL_CLIENT_SOUND_GENERAL, // DHM - Nerve :: no attenuation_GENERAL, only plays for specified client
	EV_FX_SOUND_GENERAL,
	EV_VENOM_GENERAL,
	EV_LOSE_HAT_GENERAL,            //----(SA)
	EV_CROUCH_PAIN_GENERAL,
	EV_STOPSTREAMINGSOUND_GENERAL, // JPW NERVE swiped from sherman
	EV_SMOKE_GENERAL,
	EV_SPARKS_GENERAL,
	EV_SPARKS_ELECTRIC_GENERAL,
	EV_EXPLODE_GENERAL,     // func_explosive
	EV_RUBBLE_GENERAL,
	EV_EFFECT_GENERAL,      // target_effect
	EV_MORTAREFX_GENERAL,   // mortar firing
	EV_SPINUP_GENERAL,  // JPW NERVE panzerfaust preamble
	EV_SNOW_ON_GENERAL,
	EV_SNOW_OFF_GENERAL,
	EV_MISSILE_MISS_SMALL_GENERAL,
	EV_MISSILE_MISS_LARGE_GENERAL,
	EV_MORTAR_IMPACT_GENERAL,
	EV_MORTAR_MISS_GENERAL,
	EV_SPIT_HIT_GENERAL,
	EV_SPIT_MISS_GENERAL,
	EV_SHARD_GENERAL,
	EV_JUNK_GENERAL,
	EV_EMITTER_GENERAL, //----(SA)	added // generic particle emitter that uses client-side particle scripts
	EV_OILPARTICLES_GENERAL,
	EV_OILSLICK_GENERAL,
	EV_OILSLICKREMOVE_GENERAL,
	EV_MG42EFX_GENERAL,
	EV_FLAKGUN1_GENERAL,
	EV_FLAKGUN2_GENERAL,
	EV_FLAKGUN3_GENERAL,
	EV_FLAKGUN4_GENERAL,
	EV_EXERT1_GENERAL,
	EV_EXERT2_GENERAL,
	EV_EXERT3_GENERAL,
	EV_SNOWFLURRY_GENERAL,
	EV_CONCUSSIVE_GENERAL,
	EV_DUST_GENERAL,
	EV_RUMBLE_EFX_GENERAL,
	EV_GUNSPARKS_GENERAL,
	EV_FLAMETHROWER_EFFECT_GENERAL,
	EV_POPUP_GENERAL,
	EV_POPUPBOOK_GENERAL,
	EV_GIVEPAGE_GENERAL,
	EV_MG42BULLET_HIT_FLESH_GENERAL,    // Arnout: these two send the seed as well
	EV_MG42BULLET_HIT_WALL_GENERAL,
	EV_SHAKE_GENERAL,
	EV_DISGUISE_SOUND_GENERAL,
	EV_BUILDDECAYED_SOUND_GENERAL,
	EV_FIRE_WEAPON_AAGUN_GENERAL,
	EV_ALERT_SPEAKER_GENERAL,
	EV_POPUPMESSAGE_GENERAL,
	EV_ARTYMESSAGE_GENERAL,
	EV_AIRSTRIKEMESSAGE_GENERAL,
	EV_MEDIC_CALL_GENERAL,

	// JK2SP specific:
	EV_WATER_GURP1_GENERAL,	// need air 1
	EV_WATER_GURP2_GENERAL,	// need air 2
	EV_WATER_DROWN_GENERAL,	// drowned
	EV_POWERUP_SEEKER_FIRE_GENERAL,
	EV_REPLICATOR_GENERAL,
	EV_BATTERIES_CHARGED_GENERAL,
	EV_ENTITY_FORCE_GENERAL,
	EV_AREA_FORCE_GENERAL,
	EV_GLOBAL_FORCE_GENERAL,
	EV_FORCE_STOP_GENERAL,
	EV_PLAY_MUZZLE_EFFECT_GENERAL,
	EV_TARGET_BEAM_DRAW_GENERAL,
	EV_DEMP2_ALT_IMPACT_GENERAL,
	EV_DISINTEGRATION_GENERAL,
	EV_USE_ITEM_GENERAL,
	EV_USE_INV_BINOCULARS_GENERAL,
	EV_USE_INV_BACTA_GENERAL,
	EV_USE_INV_SEEKER_GENERAL,
	EV_USE_INV_LIGHTAMP_GOGGLES_GENERAL,
	EV_USE_INV_SENTRY_GENERAL,
	EV_USE_FORCE_GENERAL,
	EV_DRUGGED_GENERAL,		// hit by an interrogator


	EV_ENTITY_EVENT_COUNT_GENERAL

} entity_event_t;			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)

static const int jk2EntityEventsToGeneral[] = {
	EV_NONE_GENERAL,

	EV_CLIENTJOIN_GENERAL,

	EV_FOOTSTEP_GENERAL,
	EV_FOOTSTEP_METAL_GENERAL,
	EV_FOOTSPLASH_GENERAL,
	EV_FOOTWADE_GENERAL,
	EV_SWIM_GENERAL,

	EV_STEP_4_GENERAL,
	EV_STEP_8_GENERAL,
	EV_STEP_12_GENERAL,
	EV_STEP_16_GENERAL,

	EV_FALL_GENERAL,

	EV_JUMP_PAD_GENERAL,			// boing sound at origin_GENERAL, jump sound on player

	EV_PRIVATE_DUEL_GENERAL,

	EV_JUMP_GENERAL,
	EV_ROLL_GENERAL,
	EV_WATER_TOUCH_GENERAL,	// foot touches
	EV_WATER_LEAVE_GENERAL,	// foot leaves
	EV_WATER_UNDER_GENERAL,	// head touches
	EV_WATER_CLEAR_GENERAL,	// head leaves

	EV_ITEM_PICKUP_GENERAL,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_GENERAL,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_FIRE_WEAPON_GENERAL,
	EV_ALT_FIRE_GENERAL,
	EV_SABER_ATTACK_GENERAL,
	EV_SABER_HIT_GENERAL,
	EV_SABER_BLOCK_GENERAL,
	EV_SABER_UNHOLSTER_GENERAL,
	EV_BECOME_JEDIMASTER_GENERAL,
	EV_DISRUPTOR_MAIN_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_MISS_GENERAL,
	EV_DISRUPTOR_HIT_GENERAL,
	EV_DISRUPTOR_ZOOMSOUND_GENERAL,

	EV_PREDEFSOUND_GENERAL,

	EV_TEAM_POWER_GENERAL,

	EV_SCREENSHAKE_GENERAL,

	EV_USE_GENERAL,			// +Use key

	EV_USE_ITEM0_GENERAL,
	EV_USE_ITEM1_GENERAL,
	EV_USE_ITEM2_GENERAL,
	EV_USE_ITEM3_GENERAL,
	EV_USE_ITEM4_GENERAL,
	EV_USE_ITEM5_GENERAL,
	EV_USE_ITEM6_GENERAL,
	EV_USE_ITEM7_GENERAL,
	EV_USE_ITEM8_GENERAL,
	EV_USE_ITEM9_GENERAL,
	EV_USE_ITEM10_GENERAL,
	EV_USE_ITEM11_GENERAL,
	EV_USE_ITEM12_GENERAL,
	EV_USE_ITEM13_GENERAL,
	EV_USE_ITEM14_GENERAL,
	EV_USE_ITEM15_GENERAL,

	EV_ITEMUSEFAIL_GENERAL,

	EV_ITEM_RESPAWN_GENERAL,
	EV_ITEM_POP_GENERAL,
	EV_PLAYER_TELEPORT_IN_GENERAL,
	EV_PLAYER_TELEPORT_OUT_GENERAL,

	EV_GRENADE_BOUNCE_GENERAL,		// eventParm will be the soundindex
	EV_MISSILE_STICK_GENERAL,		// eventParm will be the soundindex

	EV_PLAY_EFFECT_GENERAL,
	EV_PLAY_EFFECT_ID_GENERAL,

	EV_MUTE_SOUND_GENERAL,
	EV_GENERAL_SOUND_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_GENERAL,
	EV_ENTITY_SOUND_GENERAL,

	EV_PLAY_ROFF_GENERAL,

	EV_GLASS_SHATTER_GENERAL,
	EV_DEBRIS_GENERAL,

	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,
	EV_MISSILE_MISS_METAL_GENERAL,
	EV_BULLET_GENERAL,				// otherEntity is the shooter

	EV_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,
	EV_OBITUARY_GENERAL,

	EV_POWERUP_QUAD_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	//EV_POWERUP_REGEN_GENERAL,

	EV_FORCE_DRAINED_GENERAL,

	EV_GIB_PLAYER_GENERAL,			// gib a previously living player
	EV_SCOREPLUM_GENERAL,			// score plum

	EV_CTFMESSAGE_GENERAL,

	EV_SAGA_ROUNDOVER_GENERAL,
	EV_SAGA_OBJECTIVECOMPLETE_GENERAL,

	EV_DESTROY_GHOUL2_INSTANCE_GENERAL,

	EV_DESTROY_WEAPON_MODEL_GENERAL,

	EV_GIVE_NEW_RANK_GENERAL,
	EV_SET_FREE_SABER_GENERAL,
	EV_SET_FORCE_DISABLE_GENERAL,

	EV_WEAPON_CHARGE_GENERAL,
	EV_WEAPON_CHARGE_ALT_GENERAL,

	EV_SHIELD_HIT_GENERAL,

	EV_DEBUG_LINE_GENERAL,
	EV_TESTLINE_GENERAL,
	EV_STOPLOOPINGSOUND_GENERAL,
	EV_STARTLOOPINGSOUND_GENERAL,
	EV_TAUNT_GENERAL,
	EV_TAUNT_YES_GENERAL,
	EV_TAUNT_NO_GENERAL,
	EV_TAUNT_FOLLOWME_GENERAL,
	EV_TAUNT_GETFLAG_GENERAL,
	EV_TAUNT_GUARDBASE_GENERAL,
	EV_TAUNT_PATROL_GENERAL,

	EV_BODY_QUEUE_COPY_GENERAL,
};

// entityState_t->eFlags
#define	EF_DEAD				0x00000001		// don't draw a foe marker over players with EF_DEAD
#define	EF_BOUNCE_SHRAPNEL	0x00000002		// special shrapnel flag
#define	EF_TELEPORT_BIT		0x00000004		// toggled every time the origin abruptly changes

//doesn't do anything
#define	EF_AWARD_EXCELLENT	0x00000008		// draw an excellent sprite

#define EF_PLAYER_EVENT		0x00000010
#define	EF_BOUNCE			0x00000010		// for missiles

#define	EF_BOUNCE_HALF		0x00000020		// for missiles

//doesn't do anything
#define	EF_AWARD_GAUNTLET	0x00000040		// draw a gauntlet sprite

#define	EF_NODRAW			0x00000080		// may have an event, but no model (unspawned items)
#define	EF_FIRING			0x00000100		// for lightning gun
#define EF_ALT_FIRING		0x00000200		// for alt-fires, mostly for lightning guns though
#define	EF_MOVER_STOP		0x00000400		// will push otherwise

//doesn't do anything
#define EF_AWARD_CAP		0x00000800		// draw the capture sprite

#define	EF_TALK				0x00001000		// draw a talk balloon
#define	EF_CONNECTION		0x00002000		// draw a connection trouble sprite
#define	EF_VOTED			0x00004000		// already cast a vote

//next 4 don't actually do anything
#define	EF_AWARD_IMPRESSIVE	0x00008000		// draw an impressive sprite
#define	EF_AWARD_DEFEND		0x00010000		// draw a defend sprite
#define	EF_AWARD_ASSIST		0x00020000		// draw a assist sprite
#define EF_AWARD_DENIED		0x00040000		// denied

#define EF_TEAMVOTED		0x00080000		// already cast a team vote
#define EF_SEEKERDRONE		0x00100000		// show seeker drone floating around head
#define EF_MISSILE_STICK	0x00200000		// missiles that stick to the wall.
#define EF_ITEMPLACEHOLDER	0x00400000		// item effect
#define EF_SOUNDTRACKER		0x00800000		// sound position needs to be updated in relation to another entity
#define EF_DROPPEDWEAPON	0x01000000		// it's a dropped weapon
#define EF_DISINTEGRATION	0x02000000		// being disintegrated by the disruptor
#define EF_INVULNERABLE		0x04000000		// just spawned in or whatever, so is protected

typedef enum {
	PW_NONE,

	PW_QUAD,
	PW_BATTLESUIT,
	PW_HASTE,
	//PW_INVIS, //rww - removed
	//PW_REGEN, //rww - removed
	//PW_FLIGHT, //rww - removed

	PW_REDFLAG,
	PW_BLUEFLAG,
	PW_NEUTRALFLAG,

	PW_SHIELDHIT,

	//PW_SCOUT, //rww - removed
	//PW_GUARD, //rww - removed
	//PW_DOUBLER, //rww - removed
	//PW_AMMOREGEN, //rww - removed
	PW_SPEEDBURST,
	PW_DISINT_4,
	PW_SPEED,
	PW_FORCE_LIGHTNING,
	PW_FORCE_ENLIGHTENED_LIGHT,
	PW_FORCE_ENLIGHTENED_DARK,
	PW_FORCE_BOON,
	PW_YSALAMIRI,

	PW_NUM_POWERUPS

} powerup_t;

// means of death
typedef enum {
	MOD_UNKNOWN_GENERAL,
	MOD_STUN_BATON_GENERAL,
	MOD_MELEE_GENERAL,
	MOD_SABER_GENERAL,
	MOD_BRYAR_PISTOL_GENERAL,
	MOD_BRYAR_PISTOL_ALT_GENERAL,
	MOD_BLASTER_GENERAL,
	MOD_DISRUPTOR_GENERAL,
	MOD_DISRUPTOR_SPLASH_GENERAL,
	MOD_DISRUPTOR_SNIPER_GENERAL,
	MOD_BOWCASTER_GENERAL,
	MOD_REPEATER_GENERAL,
	MOD_REPEATER_ALT_GENERAL,
	MOD_REPEATER_ALT_SPLASH_GENERAL,
	MOD_DEMP2_GENERAL,
	MOD_DEMP2_ALT_GENERAL,
	MOD_FLECHETTE_GENERAL,
	MOD_FLECHETTE_ALT_SPLASH_GENERAL,
	MOD_ROCKET_GENERAL,
	MOD_ROCKET_SPLASH_GENERAL,
	MOD_ROCKET_HOMING_GENERAL,
	MOD_ROCKET_HOMING_SPLASH_GENERAL,
	MOD_THERMAL_GENERAL,
	MOD_THERMAL_SPLASH_GENERAL,
	MOD_TRIP_MINE_SPLASH_GENERAL,
	MOD_TIMED_MINE_SPLASH_GENERAL,
	MOD_DET_PACK_SPLASH_GENERAL,
	MOD_FORCE_DARK_GENERAL,
	MOD_SENTRY_GENERAL,
	MOD_WATER_GENERAL,
	MOD_SLIME_GENERAL,
	MOD_LAVA_GENERAL,
	MOD_CRUSH_GENERAL,
	MOD_TELEFRAG_GENERAL,
	MOD_FALLING_GENERAL,
	MOD_SUICIDE_GENERAL,
	MOD_TARGET_LASER_GENERAL,
	MOD_TRIGGER_HURT_GENERAL,

	// JK3
	MOD_TURBLAST_GENERAL,
	MOD_VEHICLE_GENERAL,
	MOD_CONC_GENERAL,
	MOD_CONC_ALT_GENERAL,
	MOD_COLLISION_GENERAL,
	MOD_VEH_EXPLOSION_GENERAL,
	MOD_TEAM_CHANGE_GENERAL,

	//q3
	MOD_SHOTGUN_GENERAL,
	MOD_GAUNTLET_GENERAL,
	MOD_MACHINEGUN_GENERAL,
	MOD_GRENADELAUNCHER_GENERAL,
	MOD_GRENADE_SPLASH_GENERAL,
	MOD_PLASMA_GENERAL,
	MOD_PLASMA_SPLASH_GENERAL,
	MOD_RAILGUN_GENERAL,
	MOD_LIGHTNING_GENERAL,
	MOD_BFG_GENERAL,
	MOD_BFG_SPLASH_GENERAL,
//#ifdef MISSIONPACK
	MOD_NAIL_GENERAL,
	MOD_CHAINGUN_GENERAL,
	MOD_PROXIMITY_MINE_GENERAL,
	MOD_KAMIKAZE_GENERAL,
	MOD_JUICED_GENERAL,
//#endif
	MOD_GRAPPLE_GENERAL,

	//quake live:
	MOD_SWITCH_TEAMS_GENERAL,  // 29
	MOD_THAW_GENERAL,
	MOD_LIGHTNING_DISCHARGE_GENERAL,  // demo?
	MOD_HMG_GENERAL,
	MOD_RAILGUN_HEADSHOT_GENERAL,

	//jk2sp:
	MOD_BLASTER_ALT_GENERAL,
	MOD_SNIPER_GENERAL, // this is when energy crackle kills yu?!?
	MOD_BOWCASTER_ALT_GENERAL,
	MOD_SEEKER_GENERAL,
	MOD_FORCE_GRIP_GENERAL,
	MOD_EMPLACED_GENERAL,
	MOD_ELECTROCUTE_GENERAL,
	MOD_EXPLOSIVE_GENERAL,
	MOD_EXPLOSIVE_SPLASH_GENERAL,
	MOD_KNOCKOUT_GENERAL,
	MOD_ENERGY_GENERAL,
	MOD_ENERGY_SPLASH_GENERAL,
	MOD_IMPACT_GENERAL,
	
	// MOHAA:
	MOD_CRUSH_EVERY_FRAME_GENERAL, 
	MOD_LAST_SELF_INFLICTED_GENERAL,
	MOD_EXPLOSION_GENERAL,
	MOD_EXPLODEWALL_GENERAL,
	MOD_ELECTRIC_GENERAL,
	MOD_ELECTRICWATER_GENERAL,
	MOD_THROWNOBJECT_GENERAL,
	MOD_GRENADE_GENERAL,
	MOD_BEAM_GENERAL,
	MOD_BULLET_GENERAL,
	MOD_FAST_BULLET_GENERAL,
	MOD_FIRE_GENERAL,
	MOD_FLASHBANG_GENERAL,
	MOD_ON_FIRE_GENERAL,
	MOD_GIB_GENERAL,
	MOD_IMPALE_GENERAL,
	MOD_BASH_GENERAL,
	MOD_LANDMINE_GENERAL, // Breakthrough/SH? Not sure if actually used.

	//MBII
	MOD_CHANGEDCLASSES_GENERAL,
	MOD_WENTSPECTATOR_GENERAL,
	MOD_CHANGEDTEAMS_GENERAL,
	MOD_TOOMANYTKS_GENERAL,
	MOD_EWEBEXPLOSION_GENERAL,
	MOD_SPACE_GENERAL,
	MOD_ENV_LIGHTNING_GENERAL,
	MOD_TRIGGER_HURT_ELECTRICAL_GENERAL,
	MOD_TRIGGER_HURT_FLAME_GENERAL,
	MOD_TRIGGER_HURT_POISON_GENERAL,
	MOD_WATERELECTRICAL_GENERAL,
	MOD_FORCE_LIGHTNING_GENERAL,
	MOD_FORCE_DESTRUCTION_GENERAL,
	MOD_FORCE_DEADLY_SIGHT_GENERAL,
	MOD_BRYAR_OLD_GENERAL,
	MOD_BRYAR_OLD_ALT_GENERAL,
	MOD_BOWCASTER_CHARGED_GENERAL,
	MOD_SEEKERDRONE_GENERAL,
	MOD_LASER_GENERAL,
	MOD_FLAMETHROWER_GENERAL,
	MOD_ICETHROWER_GENERAL,
	MOD_ION_BLAST_GENERAL,
	MOD_SBD_CANNON_GENERAL,
	MOD_REALCONC_GENERAL,
	MOD_REALCONC_ALT_GENERAL,
	MOD_REALCONC_SPLASH_GENERAL,
	MOD_T21_GENERAL,
	MOD_T21ALT_GENERAL,
	MOD_MICRO_THERMAL_GENERAL,
	MOD_REAL_THERMAL_GENERAL,
	MOD_PULSENADE_GENERAL,
	MOD_LAUNCHED_PULSENADE_GENERAL,
	MOD_DEP_PACK_SPLASH_GENERAL,
	MOD_CRYOBAN_BLAST_GENERAL,
	MOD_FIRE_BLAST_GENERAL,
	MOD_FIRE_BLAST_BURN_GENERAL,
	MOD_SONIC_BLAST_GENERAL,
	MOD_TRACKINGDART_GENERAL,
	MOD_POISONDART_GENERAL,
	MOD_POISON_GENERAL,
	MOD_ACID_GENERAL,
	MOD_MELEE_KICK_GENERAL,
	MOD_MELEE_KATA_GENERAL,
	MOD_ASSA_GENERAL,
	MOD_SABER_THROW_GENERAL,
	MOD_SABER_HPDRAIN_GENERAL,
	MOD_SHOCKWAVE_GENERAL,

	MOD_MAX_GENERAL,
} meansOfDeath_t;

typedef enum {
	MOD_UNKNOWN_JK2,
	MOD_STUN_BATON_JK2,
	MOD_MELEE_JK2,
	MOD_SABER_JK2,
	MOD_BRYAR_PISTOL_JK2,
	MOD_BRYAR_PISTOL_ALT_JK2,
	MOD_BLASTER_JK2,
	MOD_DISRUPTOR_JK2,
	MOD_DISRUPTOR_SPLASH_JK2,
	MOD_DISRUPTOR_SNIPER_JK2,
	MOD_BOWCASTER_JK2,
	MOD_REPEATER_JK2,
	MOD_REPEATER_ALT_JK2,
	MOD_REPEATER_ALT_SPLASH_JK2,
	MOD_DEMP2_JK2,
	MOD_DEMP2_ALT_JK2,
	MOD_FLECHETTE_JK2,
	MOD_FLECHETTE_ALT_SPLASH_JK2,
	MOD_ROCKET_JK2,
	MOD_ROCKET_SPLASH_JK2,
	MOD_ROCKET_HOMING_JK2,
	MOD_ROCKET_HOMING_SPLASH_JK2,
	MOD_THERMAL_JK2,
	MOD_THERMAL_SPLASH_JK2,
	MOD_TRIP_MINE_SPLASH_JK2,
	MOD_TIMED_MINE_SPLASH_JK2,
	MOD_DET_PACK_SPLASH_JK2,
	MOD_FORCE_DARK_JK2,
	MOD_SENTRY_JK2,
	MOD_WATER_JK2,
	MOD_SLIME_JK2,
	MOD_LAVA_JK2,
	MOD_CRUSH_JK2,
	MOD_TELEFRAG_JK2,
	MOD_FALLING_JK2,
	MOD_SUICIDE_JK2,
	MOD_TARGET_LASER_JK2,
	MOD_TRIGGER_HURT_JK2,
	MOD_MAX_JK2
} meansOfDeath_jk2_t;

static const int jk2modToGeneralMap[] =  {
	MOD_UNKNOWN_GENERAL,
	MOD_STUN_BATON_GENERAL,
	MOD_MELEE_GENERAL,
	MOD_SABER_GENERAL,
	MOD_BRYAR_PISTOL_GENERAL,
	MOD_BRYAR_PISTOL_ALT_GENERAL,
	MOD_BLASTER_GENERAL,
	MOD_DISRUPTOR_GENERAL,
	MOD_DISRUPTOR_SPLASH_GENERAL,
	MOD_DISRUPTOR_SNIPER_GENERAL,
	MOD_BOWCASTER_GENERAL,
	MOD_REPEATER_GENERAL,
	MOD_REPEATER_ALT_GENERAL,
	MOD_REPEATER_ALT_SPLASH_GENERAL,
	MOD_DEMP2_GENERAL,
	MOD_DEMP2_ALT_GENERAL,
	MOD_FLECHETTE_GENERAL,
	MOD_FLECHETTE_ALT_SPLASH_GENERAL,
	MOD_ROCKET_GENERAL,
	MOD_ROCKET_SPLASH_GENERAL,
	MOD_ROCKET_HOMING_GENERAL,
	MOD_ROCKET_HOMING_SPLASH_GENERAL,
	MOD_THERMAL_GENERAL,
	MOD_THERMAL_SPLASH_GENERAL,
	MOD_TRIP_MINE_SPLASH_GENERAL,
	MOD_TIMED_MINE_SPLASH_GENERAL,
	MOD_DET_PACK_SPLASH_GENERAL,
	MOD_FORCE_DARK_GENERAL,
	MOD_SENTRY_GENERAL,
	MOD_WATER_GENERAL,
	MOD_SLIME_GENERAL,
	MOD_LAVA_GENERAL,
	MOD_CRUSH_GENERAL,
	MOD_TELEFRAG_GENERAL,
	MOD_FALLING_GENERAL,
	MOD_SUICIDE_GENERAL,
	MOD_TARGET_LASER_GENERAL,
	MOD_TRIGGER_HURT_GENERAL,
	MOD_MAX_GENERAL
};

typedef enum {
	WP_NONE_GENERAL,
	WP_STUN_BATON_GENERAL,
	WP_SABER_GENERAL,
	WP_BRYAR_PISTOL_GENERAL,
	WP_BLASTER_GENERAL,
	WP_DISRUPTOR_GENERAL,
	WP_BOWCASTER_GENERAL,
	WP_REPEATER_GENERAL,
	WP_DEMP2_GENERAL,
	WP_FLECHETTE_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,
	WP_THERMAL_GENERAL,
	WP_TRIP_MINE_GENERAL,
	WP_DET_PACK_GENERAL,
	WP_EMPLACED_GUN_GENERAL,
	WP_TURRET_GENERAL,
	//jk3
	WP_MELEE_GENERAL,
	WP_CONCUSSION_GENERAL,
	WP_BRYAR_OLD_GENERAL,
	//q3
	WP_GAUNTLET_GENERAL,
	WP_MACHINEGUN_GENERAL,
	WP_SHOTGUN_GENERAL,
	WP_GRENADE_LAUNCHER_GENERAL,
	WP_LIGHTNING_GENERAL,
	WP_RAILGUN_GENERAL,
	WP_PLASMAGUN_GENERAL,
	WP_BFG_GENERAL,
	WP_GRAPPLING_HOOK_GENERAL,
	WP_NAILGUN_GENERAL,
	WP_PROX_LAUNCHER_GENERAL,
	WP_CHAINGUN_GENERAL,

	// ql
	WP_HEAVY_MACHINEGUN_GENERAL,

	// Jk2sp
	WP_BOT_LASER_GENERAL,		// Probe droid	- Laser blast
	WP_ATST_MAIN_GENERAL,
	WP_ATST_SIDE_GENERAL,
	WP_TIE_FIGHTER_GENERAL,
	WP_RAPID_FIRE_CONC_GENERAL,
	WP_BLASTER_PISTOL_GENERAL,	// apparently some enemy only version of the blaster

	// MOH (moh doesn't REALLY have a WP_... array, we base it on the WPREFIX_ array)
	WP_PAPERS_GENERAL,
	WP_COLT45_GENERAL,
	WP_P38_GENERAL,
	WP_HISTANDARD_GENERAL,
	WP_GARAND_GENERAL,
	WP_KAR98_GENERAL,
	WP_KAR98SNIPER_GENERAL,
	WP_SPRINGFIELD_GENERAL,
	WP_THOMPSON_GENERAL,
	WP_MP40_GENERAL,
	WP_BAR_GENERAL,
	WP_MP44_GENERAL,
	WP_FRAGGRENADE_GENERAL,
	WP_STIELHANDGRANATE_GENERAL,
	WP_BAZOOKA_GENERAL,
	WP_PANZERSCHRECK_GENERAL, 
	WP_UNARMED_GENERAL,
	WP_MG42_PORTABLE_GENERAL,
	WP_WEBLEY_GENERAL,
	WP_NAGANTREV_GENERAL,
	WP_BERETTA_GENERAL,
	WP_ENFIELD_GENERAL,
	WP_SVT_GENERAL,
	WP_MOSIN_GENERAL,
	WP_G43_GENERAL,
	WP_ENFIELDL42A_GENERAL,
	WP_CARCANO_GENERAL,
	WP_DELISLE_GENERAL,
	WP_STEN_GENERAL,
	WP_PPSH_GENERAL,
	WP_MOSCHETTO_GENERAL,
	WP_FG42_GENERAL,
	WP_VICKERS_GENERAL,
	WP_BREDA_GENERAL,
	WP_F1_GRENADE_GENERAL,
	WP_MILLS_GRENADE_GENERAL,
	WP_NEBELHANDGRANATE_GENERAL,
	WP_M18_SMOKE_GRENADE_GENERAL,
	WP_RDG1_SMOKE_GRENADE_GENERAL,
	WP_BOMBA_GENERAL,
	WP_BOMBA_BREDA_GENERAL,
	WP_MINE_GENERAL,
	WP_MINE_DETECTOR_GENERAL,
	WP_MINE_DETECTOR_AXIS_GENERAL,
	WP_DETONATOR_GENERAL,
	WP_KAR98_MORTAR_GENERAL,
	WP_PIAT_GENERAL,

	WP_NUM_WEAPONS_GENERAL,
} weapon_t;





typedef enum {
	WP_NONE_JK2,

	WP_STUN_BATON_JK2,
	WP_SABER_JK2,				 // NOTE: lots of code assumes this is the first weapon (... which is crap) so be careful -Ste.
	WP_BRYAR_PISTOL_JK2,
	WP_BLASTER_JK2,
	WP_DISRUPTOR_JK2,
	WP_BOWCASTER_JK2,
	WP_REPEATER_JK2,
	WP_DEMP2_JK2,
	WP_FLECHETTE_JK2,
	WP_ROCKET_LAUNCHER_JK2,
	WP_THERMAL_JK2,
	WP_TRIP_MINE_JK2,
	WP_DET_PACK_JK2,
	WP_EMPLACED_GUN_JK2,
	WP_TURRET_JK2,

	//	WP_GAUNTLET,
	//	WP_MACHINEGUN,			// Bryar
	//	WP_SHOTGUN,				// Blaster
	//	WP_GRENADE_LAUNCHER,	// Thermal
	//	WP_LIGHTNING,			// 
	//	WP_RAILGUN,				// 
	//	WP_GRAPPLING_HOOK,

	WP_NUM_WEAPONS_JK2
} weapon_jk2_t;




typedef struct weaponData_s
{
	//	char	classname[32];		// Spawning name

	int		ammoIndex;			// Index to proper ammo slot
	int		ammoLow;			// Count when ammo is low

	int		energyPerShot;		// Amount of energy used per shot
	int		fireTime;			// Amount of time between firings
	int		range;				// Range of weapon

	int		altEnergyPerShot;	// Amount of energy used for alt-fire
	int		altFireTime;		// Amount of time between alt-firings
	int		altRange;			// Range of alt-fire

	int		chargeSubTime;		// ms interval for subtracting ammo during charge
	int		altChargeSubTime;	// above for secondary

	int		chargeSub;			// amount to subtract during charge on each interval
	int		altChargeSub;		// above for secondary

	int		maxCharge;			// stop subtracting once charged for this many ms
	int		altMaxCharge;		// above for secondary
} weaponData_t;

extern weaponData_t weaponData_1_02[WP_NUM_WEAPONS_JK2];



static const int jk2WeaponsToGeneral[]= {
	WP_NONE_GENERAL,

	WP_STUN_BATON_GENERAL,
	WP_SABER_GENERAL,				 // NOTE: lots of code assumes this is the first weapon (... which is crap) so be careful -Ste.
	WP_BRYAR_PISTOL_GENERAL,
	WP_BLASTER_GENERAL,
	WP_DISRUPTOR_GENERAL,
	WP_BOWCASTER_GENERAL,
	WP_REPEATER_GENERAL,
	WP_DEMP2_GENERAL,
	WP_FLECHETTE_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,
	WP_THERMAL_GENERAL,
	WP_TRIP_MINE_GENERAL,
	WP_DET_PACK_GENERAL,
	WP_EMPLACED_GUN_GENERAL,
	WP_TURRET_GENERAL,

	//	WP_GAUNTLET,
	//	WP_MACHINEGUN,			// Bryar
	//	WP_SHOTGUN,				// Blaster
	//	WP_GRENADE_LAUNCHER,	// Thermal
	//	WP_LIGHTNING,			// 
	//	WP_RAILGUN,				// 
	//	WP_GRAPPLING_HOOK,

	WP_NUM_WEAPONS_GENERAL
};

extern int weaponFromMOD_JK2[MOD_MAX_JK2];
extern int weaponFromMOD_GENERAL[MOD_MAX_GENERAL];

enum
{
	FORCE_LEVEL_0,
	FORCE_LEVEL_1,
	FORCE_LEVEL_2,
	FORCE_LEVEL_3,
	NUM_FORCE_POWER_LEVELS
};

// This is JKA stuff actually but the constants havent changed from JK2 in terms of values so its ok
typedef enum
{
	SS_NONE = 0,
	SS_FAST,
	SS_MEDIUM,
	SS_STRONG,
	SS_DESANN,
	SS_TAVION,
	SS_DUAL,
	SS_STAFF,
	SS_NUM_SABER_STYLES
} saber_styles_t;

typedef enum {
	//totally invalid
	LS_INVALID_GENERAL = -1,
	// Invalid, or saber not armed
	LS_NONE_GENERAL = 0,

	// General movements with saber
	LS_READY_GENERAL,
	LS_DRAW_GENERAL,
	LS_PUTAWAY_GENERAL,

	// Attacks
	LS_A_TL2BR_GENERAL,//4
	LS_A_L2R_GENERAL,
	LS_A_BL2TR_GENERAL,
	LS_A_BR2TL_GENERAL,
	LS_A_R2L_GENERAL,
	LS_A_TR2BL_GENERAL,
	LS_A_T2B_GENERAL,
	LS_A_BACKSTAB_GENERAL,
	LS_A_BACK_GENERAL,
	LS_A_BACK_CR_GENERAL,
	LS_ROLL_STAB_GENERAL,
	LS_A_LUNGE_GENERAL,
	LS_A_JUMP_T__B__GENERAL,
	LS_A_FLIP_STAB_GENERAL,
	LS_A_FLIP_SLASH_GENERAL,
	LS_JUMPATTACK_DUAL_GENERAL,
	LS_JUMPATTACK_ARIAL_LEFT_GENERAL,
	LS_JUMPATTACK_ARIAL_RIGHT_GENERAL,
	LS_JUMPATTACK_CART_LEFT_GENERAL,
	LS_JUMPATTACK_CART_RIGHT_GENERAL,
	LS_JUMPATTACK_STAFF_LEFT_GENERAL,
	LS_JUMPATTACK_STAFF_RIGHT_GENERAL,
	LS_BUTTERFLY_LEFT_GENERAL,
	LS_BUTTERFLY_RIGHT_GENERAL,
	LS_A_BACKFLIP_ATK_GENERAL,
	LS_SPINATTACK_DUAL_GENERAL,
	LS_SPINATTACK_GENERAL,
	LS_LEAP_ATTACK_GENERAL,
	LS_SWOOP_ATTACK_RIGHT_GENERAL,
	LS_SWOOP_ATTACK_LEFT_GENERAL,
	LS_TAUNTAUN_ATTACK_RIGHT_GENERAL,
	LS_TAUNTAUN_ATTACK_LEFT_GENERAL,
	LS_KICK_F_GENERAL,
	LS_KICK_B_GENERAL,
	LS_KICK_R_GENERAL,
	LS_KICK_L_GENERAL,
	LS_KICK_S_GENERAL,
	LS_KICK_BF_GENERAL,
	LS_KICK_RL_GENERAL,
	LS_KICK_F_AIR_GENERAL,
	LS_KICK_B_AIR_GENERAL,
	LS_KICK_R_AIR_GENERAL,
	LS_KICK_L_AIR_GENERAL,
	LS_STABDOWN_GENERAL,
	LS_STABDOWN_STAFF_GENERAL,
	LS_STABDOWN_DUAL_GENERAL,
	LS_DUAL_SPIN_PROTECT_GENERAL,
	LS_STAFF_SOULCAL_GENERAL,
	LS_A1_SPECIAL_GENERAL,
	LS_A2_SPECIAL_GENERAL,
	LS_A3_SPECIAL_GENERAL,
	LS_UPSIDE_DOWN_ATTACK_GENERAL,
	LS_PULL_ATTACK_STAB_GENERAL,
	LS_PULL_ATTACK_SWING_GENERAL,
	LS_SPINATTACK_ALORA_GENERAL,
	LS_DUAL_FB_GENERAL,
	LS_DUAL_LR_GENERAL,
	LS_HILT_BASH_GENERAL,

	//starts
	LS_S_TL2BR_GENERAL,//26
	LS_S_L2R_GENERAL,
	LS_S_BL2TR_GENERAL,//# Start of attack chaining to SLASH LR2UL
	LS_S_BR2TL_GENERAL,//# Start of attack chaining to SLASH LR2UL
	LS_S_R2L_GENERAL,
	LS_S_TR2BL_GENERAL,
	LS_S_T2B_GENERAL,

	//returns
	LS_R_TL2BR_GENERAL,//33
	LS_R_L2R_GENERAL,
	LS_R_BL2TR_GENERAL,
	LS_R_BR2TL_GENERAL,
	LS_R_R2L_GENERAL,
	LS_R_TR2BL_GENERAL,
	LS_R_T2B_GENERAL,

	//transitions
	LS_T1_BR__R_GENERAL,//40
	LS_T1_BR_TR_GENERAL,
	LS_T1_BR_T__GENERAL,
	LS_T1_BR_TL_GENERAL,
	LS_T1_BR__L_GENERAL,
	LS_T1_BR_BL_GENERAL,
	LS_T1__R_BR_GENERAL,//46
	LS_T1__R_TR_GENERAL,
	LS_T1__R_T__GENERAL,
	LS_T1__R_TL_GENERAL,
	LS_T1__R__L_GENERAL,
	LS_T1__R_BL_GENERAL,
	LS_T1_TR_BR_GENERAL,//52
	LS_T1_TR__R_GENERAL,
	LS_T1_TR_T__GENERAL,
	LS_T1_TR_TL_GENERAL,
	LS_T1_TR__L_GENERAL,
	LS_T1_TR_BL_GENERAL,
	LS_T1_T__BR_GENERAL,//58
	LS_T1_T___R_GENERAL,
	LS_T1_T__TR_GENERAL,
	LS_T1_T__TL_GENERAL,
	LS_T1_T___L_GENERAL,
	LS_T1_T__BL_GENERAL,
	LS_T1_TL_BR_GENERAL,//64
	LS_T1_TL__R_GENERAL,
	LS_T1_TL_TR_GENERAL,
	LS_T1_TL_T__GENERAL,
	LS_T1_TL__L_GENERAL,
	LS_T1_TL_BL_GENERAL,
	LS_T1__L_BR_GENERAL,//70
	LS_T1__L__R_GENERAL,
	LS_T1__L_TR_GENERAL,
	LS_T1__L_T__GENERAL,
	LS_T1__L_TL_GENERAL,
	LS_T1__L_BL_GENERAL,
	LS_T1_BL_BR_GENERAL,//76
	LS_T1_BL__R_GENERAL,
	LS_T1_BL_TR_GENERAL,
	LS_T1_BL_T__GENERAL,
	LS_T1_BL_TL_GENERAL,
	LS_T1_BL__L_GENERAL,

	//Bounces
	LS_B1_BR_GENERAL,
	LS_B1__R_GENERAL,
	LS_B1_TR_GENERAL,
	LS_B1_T__GENERAL,
	LS_B1_TL_GENERAL,
	LS_B1__L_GENERAL,
	LS_B1_BL_GENERAL,

	//Deflected attacks
	LS_D1_BR_GENERAL,
	LS_D1__R_GENERAL,
	LS_D1_TR_GENERAL,
	LS_D1_T__GENERAL,
	LS_D1_TL_GENERAL,
	LS_D1__L_GENERAL,
	LS_D1_BL_GENERAL,
	LS_D1_B__GENERAL,

	//Reflected attacks
	LS_V1_BR_GENERAL,
	LS_V1__R_GENERAL,
	LS_V1_TR_GENERAL,
	LS_V1_T__GENERAL,
	LS_V1_TL_GENERAL,
	LS_V1__L_GENERAL,
	LS_V1_BL_GENERAL,
	LS_V1_B__GENERAL,

	// Broken parries
	LS_H1_T__GENERAL,//
	LS_H1_TR_GENERAL,
	LS_H1_TL_GENERAL,
	LS_H1_BR_GENERAL,
	LS_H1_B__GENERAL,
	LS_H1_BL_GENERAL,

	// Knockaways
	LS_K1_T__GENERAL,//
	LS_K1_TR_GENERAL,
	LS_K1_TL_GENERAL,
	LS_K1_BR_GENERAL,
	LS_K1_BL_GENERAL,

	// Parries
	LS_PARRY_UP_GENERAL,//
	LS_PARRY_UR_GENERAL,
	LS_PARRY_UL_GENERAL,
	LS_PARRY_LR_GENERAL,
	LS_PARRY_LL_GENERAL,

	// Projectile Reflections
	LS_REFLECT_UP_GENERAL,//
	LS_REFLECT_UR_GENERAL,
	LS_REFLECT_UL_GENERAL,
	LS_REFLECT_LR_GENERAL,
	LS_REFLECT_LL_GENERAL,

	LS_MOVE_MAX_GENERAL//
} saberMoveName_t;


typedef enum {
	LS_INVALID = -1,
	// Invalid_GENERAL, or saber not armed
	LS_NONE_JK2 = 0,

	// General movements with saber
	LS_READY_JK2,
	LS_DRAW_JK2,
	LS_PUTAWAY_JK2,

	// Attacks
	LS_A_TL2BR_JK2,//4
	LS_A_L2R_JK2,
	LS_A_BL2TR_JK2,
	LS_A_BR2TL_JK2,
	LS_A_R2L_JK2,
	LS_A_TR2BL_JK2,
	LS_A_T2B_JK2,
	LS_A_BACKSTAB_JK2,
	LS_A_BACK_JK2,
	LS_A_BACK_CR_JK2,
	LS_A_LUNGE_JK2,
	LS_A_JUMP_T__B__JK2,
	LS_A_FLIP_STAB_JK2,
	LS_A_FLIP_SLASH_JK2,

	//starts
	LS_S_TL2BR_JK2,//26
	LS_S_L2R_JK2,
	LS_S_BL2TR_JK2,//# Start of attack chaining to SLASH LR2UL
	LS_S_BR2TL_JK2,//# Start of attack chaining to SLASH LR2UL
	LS_S_R2L_JK2,
	LS_S_TR2BL_JK2,
	LS_S_T2B_JK2,

	//returns
	LS_R_TL2BR_JK2,//33
	LS_R_L2R_JK2,
	LS_R_BL2TR_JK2,
	LS_R_BR2TL_JK2,
	LS_R_R2L_JK2,
	LS_R_TR2BL_JK2,
	LS_R_T2B_JK2,

	//transitions
	LS_T1_BR__R_JK2,//40
	LS_T1_BR_TR_JK2,
	LS_T1_BR_T__JK2,
	LS_T1_BR_TL_JK2,
	LS_T1_BR__L_JK2,
	LS_T1_BR_BL_JK2,
	LS_T1__R_BR_JK2,//46
	LS_T1__R_TR_JK2,
	LS_T1__R_T__JK2,
	LS_T1__R_TL_JK2,
	LS_T1__R__L_JK2,
	LS_T1__R_BL_JK2,
	LS_T1_TR_BR_JK2,//52
	LS_T1_TR__R_JK2,
	LS_T1_TR_T__JK2,
	LS_T1_TR_TL_JK2,
	LS_T1_TR__L_JK2,
	LS_T1_TR_BL_JK2,
	LS_T1_T__BR_JK2,//58
	LS_T1_T___R_JK2,
	LS_T1_T__TR_JK2,
	LS_T1_T__TL_JK2,
	LS_T1_T___L_JK2,
	LS_T1_T__BL_JK2,
	LS_T1_TL_BR_JK2,//64
	LS_T1_TL__R_JK2,
	LS_T1_TL_TR_JK2,
	LS_T1_TL_T__JK2,
	LS_T1_TL__L_JK2,
	LS_T1_TL_BL_JK2,
	LS_T1__L_BR_JK2,//70
	LS_T1__L__R_JK2,
	LS_T1__L_TR_JK2,
	LS_T1__L_T__JK2,
	LS_T1__L_TL_JK2,
	LS_T1__L_BL_JK2,
	LS_T1_BL_BR_JK2,//76
	LS_T1_BL__R_JK2,
	LS_T1_BL_TR_JK2,
	LS_T1_BL_T__JK2,
	LS_T1_BL_TL_JK2,
	LS_T1_BL__L_JK2,

	//Bounces
	LS_B1_BR_JK2,
	LS_B1__R_JK2,
	LS_B1_TR_JK2,
	LS_B1_T__JK2,
	LS_B1_TL_JK2,
	LS_B1__L_JK2,
	LS_B1_BL_JK2,

	//Deflected attacks
	LS_D1_BR_JK2,
	LS_D1__R_JK2,
	LS_D1_TR_JK2,
	LS_D1_T__JK2,
	LS_D1_TL_JK2,
	LS_D1__L_JK2,
	LS_D1_BL_JK2,
	LS_D1_B__JK2,

	//Reflected attacks
	LS_V1_BR_JK2,
	LS_V1__R_JK2,
	LS_V1_TR_JK2,
	LS_V1_T__JK2,
	LS_V1_TL_JK2,
	LS_V1__L_JK2,
	LS_V1_BL_JK2,
	LS_V1_B__JK2,

	// Broken parries
	LS_H1_T__JK2,//
	LS_H1_TR_JK2,
	LS_H1_TL_JK2,
	LS_H1_BR_JK2,
	LS_H1_B__JK2,
	LS_H1_BL_JK2,

	// Knockaways
	LS_K1_T__JK2,//
	LS_K1_TR_JK2,
	LS_K1_TL_JK2,
	LS_K1_BR_JK2,
	LS_K1_BL_JK2,

	// Parries
	LS_PARRY_UP_JK2,//
	LS_PARRY_UR_JK2,
	LS_PARRY_UL_JK2,
	LS_PARRY_LR_JK2,
	LS_PARRY_LL_JK2,

	// Projectile Reflections
	LS_REFLECT_UP_JK2,//
	LS_REFLECT_UR_JK2,
	LS_REFLECT_UL_JK2,
	LS_REFLECT_LR_JK2,
	LS_REFLECT_LL_JK2,

	LS_MOVE_MAX_JK2
} saberMoveName_jk2_t;

static const int saberMoveJK2ToGeneral[] {
	LS_INVALID_GENERAL,

	// Invalid_GENERAL, or saber not armed
	LS_NONE_GENERAL,

	// General movements with saber
	LS_READY_GENERAL,
	LS_DRAW_GENERAL,
	LS_PUTAWAY_GENERAL,

	// Attacks
	LS_A_TL2BR_GENERAL,//4
	LS_A_L2R_GENERAL,
	LS_A_BL2TR_GENERAL,
	LS_A_BR2TL_GENERAL,
	LS_A_R2L_GENERAL,
	LS_A_TR2BL_GENERAL,
	LS_A_T2B_GENERAL,
	LS_A_BACKSTAB_GENERAL,
	LS_A_BACK_GENERAL,
	LS_A_BACK_CR_GENERAL,
	LS_A_LUNGE_GENERAL,
	LS_A_JUMP_T__B__GENERAL,
	LS_A_FLIP_STAB_GENERAL,
	LS_A_FLIP_SLASH_GENERAL,

	//starts
	LS_S_TL2BR_GENERAL,//26
	LS_S_L2R_GENERAL,
	LS_S_BL2TR_GENERAL,//# Start of attack chaining to SLASH LR2UL
	LS_S_BR2TL_GENERAL,//# Start of attack chaining to SLASH LR2UL
	LS_S_R2L_GENERAL,
	LS_S_TR2BL_GENERAL,
	LS_S_T2B_GENERAL,

	//returns
	LS_R_TL2BR_GENERAL,//33
	LS_R_L2R_GENERAL,
	LS_R_BL2TR_GENERAL,
	LS_R_BR2TL_GENERAL,
	LS_R_R2L_GENERAL,
	LS_R_TR2BL_GENERAL,
	LS_R_T2B_GENERAL,

	//transitions
	LS_T1_BR__R_GENERAL,//40
	LS_T1_BR_TR_GENERAL,
	LS_T1_BR_T__GENERAL,
	LS_T1_BR_TL_GENERAL,
	LS_T1_BR__L_GENERAL,
	LS_T1_BR_BL_GENERAL,
	LS_T1__R_BR_GENERAL,//46
	LS_T1__R_TR_GENERAL,
	LS_T1__R_T__GENERAL,
	LS_T1__R_TL_GENERAL,
	LS_T1__R__L_GENERAL,
	LS_T1__R_BL_GENERAL,
	LS_T1_TR_BR_GENERAL,//52
	LS_T1_TR__R_GENERAL,
	LS_T1_TR_T__GENERAL,
	LS_T1_TR_TL_GENERAL,
	LS_T1_TR__L_GENERAL,
	LS_T1_TR_BL_GENERAL,
	LS_T1_T__BR_GENERAL,//58
	LS_T1_T___R_GENERAL,
	LS_T1_T__TR_GENERAL,
	LS_T1_T__TL_GENERAL,
	LS_T1_T___L_GENERAL,
	LS_T1_T__BL_GENERAL,
	LS_T1_TL_BR_GENERAL,//64
	LS_T1_TL__R_GENERAL,
	LS_T1_TL_TR_GENERAL,
	LS_T1_TL_T__GENERAL,
	LS_T1_TL__L_GENERAL,
	LS_T1_TL_BL_GENERAL,
	LS_T1__L_BR_GENERAL,//70
	LS_T1__L__R_GENERAL,
	LS_T1__L_TR_GENERAL,
	LS_T1__L_T__GENERAL,
	LS_T1__L_TL_GENERAL,
	LS_T1__L_BL_GENERAL,
	LS_T1_BL_BR_GENERAL,//76
	LS_T1_BL__R_GENERAL,
	LS_T1_BL_TR_GENERAL,
	LS_T1_BL_T__GENERAL,
	LS_T1_BL_TL_GENERAL,
	LS_T1_BL__L_GENERAL,

	//Bounces
	LS_B1_BR_GENERAL,
	LS_B1__R_GENERAL,
	LS_B1_TR_GENERAL,
	LS_B1_T__GENERAL,
	LS_B1_TL_GENERAL,
	LS_B1__L_GENERAL,
	LS_B1_BL_GENERAL,

	//Deflected attacks
	LS_D1_BR_GENERAL,
	LS_D1__R_GENERAL,
	LS_D1_TR_GENERAL,
	LS_D1_T__GENERAL,
	LS_D1_TL_GENERAL,
	LS_D1__L_GENERAL,
	LS_D1_BL_GENERAL,
	LS_D1_B__GENERAL,

	//Reflected attacks
	LS_V1_BR_GENERAL,
	LS_V1__R_GENERAL,
	LS_V1_TR_GENERAL,
	LS_V1_T__GENERAL,
	LS_V1_TL_GENERAL,
	LS_V1__L_GENERAL,
	LS_V1_BL_GENERAL,
	LS_V1_B__GENERAL,

	// Broken parries
	LS_H1_T__GENERAL,//
	LS_H1_TR_GENERAL,
	LS_H1_TL_GENERAL,
	LS_H1_BR_GENERAL,
	LS_H1_B__GENERAL,
	LS_H1_BL_GENERAL,

	// Knockaways
	LS_K1_T__GENERAL,//
	LS_K1_TR_GENERAL,
	LS_K1_TL_GENERAL,
	LS_K1_BR_GENERAL,
	LS_K1_BL_GENERAL,

	// Parries
	LS_PARRY_UP_GENERAL,//
	LS_PARRY_UR_GENERAL,
	LS_PARRY_UL_GENERAL,
	LS_PARRY_LR_GENERAL,
	LS_PARRY_LL_GENERAL,

	// Projectile Reflections
	LS_REFLECT_UP_GENERAL,//
	LS_REFLECT_UR_GENERAL,
	LS_REFLECT_UL_GENERAL,
	LS_REFLECT_LR_GENERAL,
	LS_REFLECT_LL_GENERAL,

	LS_MOVE_MAX_GENERAL//
};


typedef std::tuple<demoType_t, int, int> demoTypeFieldOffset; // demoType, field offset, ispsfield
extern ankerl::unordered_dense::map< demoTypeFieldOffset, size_t, ankerl::unordered_dense::hash<demoTypeFieldOffset>> readOverflows;

extern ankerl::unordered_dense::map <int, std::string, ankerl::unordered_dense::hash<int>>  saberMoveNames_general;
extern ankerl::unordered_dense::map <int, std::string, ankerl::unordered_dense::hash<int>>  saberStyleNames;
extern ankerl::unordered_dense::map <std::string, int, ankerl::unordered_dense::hash<std::string>>  mohaaWeaponModelMap;

typedef enum
{
	HL_NONE = 0,
	HL_FOOT_RT,
	HL_FOOT_LT,
	HL_LEG_RT,
	HL_LEG_LT,
	HL_WAIST,
	HL_BACK_RT,
	HL_BACK_LT,
	HL_BACK,
	HL_CHEST_RT,
	HL_CHEST_LT,
	HL_CHEST,
	HL_ARM_RT,
	HL_ARM_LT,
	HL_HAND_RT,
	HL_HAND_LT,
	HL_HEAD,
	HL_GENERIC1,
	HL_GENERIC2,
	HL_GENERIC3,
	HL_GENERIC4,
	HL_GENERIC5,
	HL_GENERIC6,
	HL_MAX
} HL_t;

typedef enum {
	G2_MODELPART_HEAD = 10,
	G2_MODELPART_WAIST,
	G2_MODELPART_LARM,
	G2_MODELPART_RARM,
	G2_MODELPART_RHAND,
	G2_MODELPART_LLEG,
	G2_MODELPART_RLEG
} g2ModelParts_t;

#define G2_MODEL_PART	50

#define	SOLID_BMODEL	0xffffff


typedef struct
{
	char* name;
	int animToUseGeneral; // If you ever use this, fix the animations/generalize them
	int	startQuad;
	int	endQuad;
	unsigned animSetFlags;
	int blendTime;
	int blocking;
	saberMoveName_t chain_idle;			// What move to call if the attack button is not pressed at the end of this anim
	saberMoveName_t chain_attack;		// What move to call if the attack button (and nothing else) is pressed
	int trailLength;
} saberMoveData_t;

typedef struct
{
	char* name;
	int animToUseDM16;
	int	startQuad;
	int	endQuad;
	unsigned animSetFlags;
	int blendTime;
	int blocking;
	saberMoveName_jk2_t chain_idle;			// What move to call if the attack button is not pressed at the end of this anim
	saberMoveName_jk2_t chain_attack;		// What move to call if the attack button (and nothing else) is pressed
	int trailLength;
} saberMoveData_jk2_t;

extern saberMoveData_t	saberMoveData_general[LS_MOVE_MAX_GENERAL];
extern saberMoveData_jk2_t	saberMoveData_jk2[LS_MOVE_MAX_JK2];


typedef enum saberMoveType_t { // roughly the 3 classes of saber moves for purposes of combo detection. note we dont exclude yellow dfa from being counted as part of a combo. same about any that jka would potentially exclude in bg_saber
	LST_IDLE,
	LST_ATTACK,
	LST_TRANS
};

inline saberMoveType_t classifySaberMove(saberMoveName_t saberMove) {
	if (saberMove <= LS_PUTAWAY_GENERAL) {
		return LST_IDLE;
	}
	else if (saberMove <= LS_HILT_BASH_GENERAL) {
		return LST_ATTACK;
	}
	else {
		return LST_TRANS;
	}
}

void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, qboolean snap, demoType_t demoType, qboolean writeCommandTime = qtrue, qboolean clientSideStyleEventConversion=qfalse);
void CG_EntityStateToPlayerState(entityState_t* s, playerState_t* ps, demoType_t demoType, qboolean allValues=qtrue, playerState_t* baseState=NULL, qboolean enhanceOnly=qfalse);
void EnhancePlayerStateWithBaseState(playerState_t* ps, playerState_t* baseState, demoType_t demoType);

float LerpAngle(float from, float to, float frac);


#define	SNAPFLAG_RATE_DELAYED	1
#define	SNAPFLAG_NOT_ACTIVE		2	// snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT	4	// toggled every map_restart so transitions can be detected



#define DEMO_MAX_INDEX	36000 // Controls how much of the demo positions get precached (after that performance is terrible). Default was 3600
#define DEMO_PLAY_CMDS  256
#define DEMO_SNAPS  32
#define DEMOCONVERTFRAMES 16

#define DEMO_CLIENT_UPDATE 0x1
#define DEMO_CLIENT_ACTIVE 0x2

/*typedef struct {
	int			offsets[MAX_CONFIGSTRINGS];
	int			used;
	char		data[MAX_GAMESTATE_CHARS];
} demoString_t;
typedef struct {
	int			serverTime;
	playerState_t clients[MAX_CLIENTS];
	byte		clientData[MAX_CLIENTS];
	entityState_t entities[MAX_GENTITIES];
	byte		entityData[MAX_GENTITIES];
	int			commandUsed;
	char		commandData[2048 * MAX_CLIENTS];
	byte		areaUsed;
	byte		areamask[MAX_MAP_AREA_BYTES];
	demoString_t string;
} demoFrame_t;

#define FRAME_BUF_SIZE 20
typedef struct {
	fileHandle_t		fileHandle;
	char				fileName[MAX_QPATH];
	int					fileSize, filePos;
	int					totalFrames;
	int					startTime, endTime;			//serverTime from first snapshot 

	qboolean			lastFrame;

	int					frameNumber;

	char				commandData[256 * 1024];
	int					commandFree;
	int					commandStart[DEMO_PLAY_CMDS];
	int					commandCount;
	int					clientNum;
	demoFrame_t			storageFrame[FRAME_BUF_SIZE];
	demoFrame_t* frame, * nextFrame;
	qboolean			nextValid;
	struct {
		int				pos, time, frame;
	} fileIndex[DEMO_MAX_INDEX];
	int					fileIndexCount;
} demoPlay_t;*/

typedef struct {
	int						/*nextNum,*/ currentNum;
	/*struct {
		demoPlay_t* handle;
		int					snapCount;
		int					messageCount;
		int					oldDelay;
		int					oldTime;
		int					oldFrameNumber;
		int					serverTime;
	} play;
	qboolean				del;*/
	struct {
		clientConnection_t	Clc;
		clientActive_t		Cl;
	} cut;
	int lastPMT;
	int64_t lastPMTChange;
} demo_t;


extern demo_t demo;





void sanitizeFilename(const char* input, char* output, qboolean allowExtension=qfalse);
qboolean getModelAndSkinFromSkinPath(const char* skinPath, char* outModel, int outModelMaxLen, char* outSkin, int outSkinMaxLen);

std::vector<std::string> splitString(std::string input, std::string separator, bool trim = true, bool allowEmpty = false);



char tolowerSignSafe(char in);


void BG_EvaluateTrajectory(const trajectory_t* tr, int atTime, vec3_t result);
void BG_EvaluateTrajectoryDelta(const trajectory_t* tr, int atTime, vec3_t result);


int getLikelyStanceFromTorsoAnim(int torsoAnim, demoType_t demoType, byte* probability);


float VectorDistance(vec3_t v1, vec3_t v2);
float Vector2Distance(vec2_t v1, vec2_t v2);
float Vector2DistanceSquared(vec2_t v1, vec2_t v2);



typedef enum //# ammo_e
{
	AMMO_NONE,
	AMMO_FORCE,		// AMMO_PHASER
	AMMO_BLASTER,	// AMMO_STARFLEET,
	AMMO_POWERCELL,	// AMMO_ALIEN,
	AMMO_METAL_BOLTS,
	AMMO_ROCKETS,
	AMMO_EMPLACED,
	AMMO_THERMAL,
	AMMO_TRIPMINE,
	AMMO_DETPACK,
	AMMO_MAX
} ammo_t;
typedef enum {
	HI_NONE,

	HI_SEEKER,
	HI_SHIELD,
	HI_MEDPAC,
	HI_DATAPAD,
	HI_BINOCULARS,
	HI_SENTRY_GUN,

	HI_NUM_HOLDABLE
} holdable_t;

// gitem_t->type
typedef enum {
	IT_BAD,
	IT_WEAPON,				// EFX: rotate + upscale + minlight
	IT_AMMO,				// EFX: rotate
	IT_ARMOR,				// EFX: rotate + minlight
	IT_HEALTH,				// EFX: static external sphere + rotating internal
	IT_POWERUP,				// instant on, timer based
							// EFX: rotate + external ring that rotates
							IT_HOLDABLE,			// single use, holdable item
													// EFX: rotate + bob
													IT_PERSISTANT_POWERUP,
													IT_TEAM
} itemType_t;

#define MAX_ITEM_MODELS 4

typedef struct gitem_s {
	char* classname;	// spawning name
	char* pickup_sound;
	char* world_model[MAX_ITEM_MODELS];
	char* view_model;

	char* icon;
	//	char		*pickup_name;	// for printing on pickup

	int			quantity;		// for ammo how much, or duration of powerup
	itemType_t  giType;			// IT_* flags

	int			giTag;

	char* precaches;		// string of all models and images this item will use
	char* sounds;		// string of all sounds this item will use
} gitem_t;



//extern	gitem_t	bg_itemlist[];
//extern	int		bg_numItems;


typedef enum {
	ITEMLIST_NONE_GENERAL,
	ITEMLIST_ITEM_SHIELD_SM_INSTANT_GENERAL,
	ITEMLIST_ITEM_SHIELD_LRG_INSTANT_GENERAL,
	ITEMLIST_ITEM_MEDPAK_INSTANT_GENERAL,
	ITEMLIST_ITEM_SEEKER_GENERAL,
	ITEMLIST_ITEM_SHIELD_GENERAL,
	ITEMLIST_ITEM_MEDPAC_GENERAL,
	ITEMLIST_ITEM_DATAPAD_GENERAL,
	ITEMLIST_ITEM_BINOCULARS_GENERAL,
	ITEMLIST_ITEM_SENTRY_GUN_GENERAL,
	ITEMLIST_ITEM_FORCE_ENLIGHTEN_LIGHT_GENERAL,
	ITEMLIST_ITEM_FORCE_ENLIGHTEN_DARK_GENERAL,
	ITEMLIST_ITEM_FORCE_BOON_GENERAL,
	ITEMLIST_ITEM_YSALIMARI_GENERAL,
	ITEMLIST_WEAPON_STUN_BATON_GENERAL,
	ITEMLIST_WEAPON_SABER_GENERAL,
	ITEMLIST_WEAPON_BRYAR_PISTOL_GENERAL,
	ITEMLIST_WEAPON_BLASTER_GENERAL,
	ITEMLIST_WEAPON_DISRUPTOR_GENERAL,
	ITEMLIST_WEAPON_BOWCASTER_GENERAL,
	ITEMLIST_WEAPON_REPEATER_GENERAL,
	ITEMLIST_WEAPON_DEMP2_GENERAL,
	ITEMLIST_WEAPON_FLECHETTE_GENERAL,
	ITEMLIST_WEAPON_ROCKET_LAUNCHER_GENERAL,
	ITEMLIST_AMMO_THERMAL_GENERAL,
	ITEMLIST_AMMO_TRIPMINE_GENERAL,
	ITEMLIST_AMMO_DETPACK_GENERAL,
	ITEMLIST_WEAPON_THERMAL_GENERAL,
	ITEMLIST_WEAPON_TRIP_MINE_GENERAL,
	ITEMLIST_WEAPON_DET_PACK_GENERAL,
	ITEMLIST_WEAPON_EMPLACED_GENERAL,
	ITEMLIST_WEAPON_TURRETWP_GENERAL,
	ITEMLIST_AMMO_FORCE_GENERAL,
	ITEMLIST_AMMO_BLASTER_GENERAL,
	ITEMLIST_AMMO_POWERCELL_GENERAL,
	ITEMLIST_AMMO_METALLIC_BOLTS_GENERAL,
	ITEMLIST_AMMO_ROCKETS_GENERAL,
	ITEMLIST_TEAM_CTF_REDFLAG_GENERAL,
	ITEMLIST_TEAM_CTF_BLUEFLAG_GENERAL,
	ITEMLIST_TEAM_CTF_NEUTRALFLAG_GENERAL,
	ITEMLIST_ITEM_REDCUBE_GENERAL,
	ITEMLIST_ITEM_BLUECUBE_GENERAL,
	ITEMLIST_ITEM_MEDPAC_BIG_GENERAL,
	ITEMLIST_ITEM_JETPACK_GENERAL,
	ITEMLIST_ITEM_HEALTHDISP_GENERAL,
	ITEMLIST_ITEM_AMMODISP_GENERAL,
	ITEMLIST_ITEM_EWEB_HOLDABLE_GENERAL,
	ITEMLIST_ITEM_CLOAK_GENERAL,
	ITEMLIST_WEAPON_MELEE_GENERAL,
	ITEMLIST_WEAPON_BLASTER_PISTOL_GENERAL,
	ITEMLIST_WEAPON_CONCUSSION_RIFLE_GENERAL,
	ITEMLIST_AMMO_ALL_GENERAL,
	ITEMLIST_ITEM_ARMOR_SHARD_GENERAL,
	ITEMLIST_ITEM_ARMOR_COMBAT_GENERAL,
	ITEMLIST_ITEM_ARMOR_BODY_GENERAL,
	ITEMLIST_ITEM_ARMOR_JACKET_GENERAL, // Quake Live
	ITEMLIST_ITEM_HEALTH_SMALL_GENERAL,
	ITEMLIST_ITEM_HEALTH_GENERAL,
	ITEMLIST_ITEM_HEALTH_LARGE_GENERAL,
	ITEMLIST_ITEM_HEALTH_MEGA_GENERAL,
	ITEMLIST_WEAPON_GAUNTLET_GENERAL,
	ITEMLIST_WEAPON_SHOTGUN_GENERAL,
	ITEMLIST_WEAPON_MACHINEGUN_GENERAL,
	ITEMLIST_WEAPON_GRENADELAUNCHER_GENERAL,
	ITEMLIST_WEAPON_ROCKETLAUNCHER_GENERAL,
	ITEMLIST_WEAPON_LIGHTNING_GENERAL,
	ITEMLIST_WEAPON_RAILGUN_GENERAL,
	ITEMLIST_WEAPON_PLASMAGUN_GENERAL,
	ITEMLIST_WEAPON_BFG_GENERAL,
	ITEMLIST_WEAPON_GRAPPLINGHOOK_GENERAL,
	ITEMLIST_AMMO_SHELLS_GENERAL,
	ITEMLIST_AMMO_BULLETS_GENERAL,
	ITEMLIST_AMMO_GRENADES_GENERAL,
	ITEMLIST_AMMO_CELLS_GENERAL,
	ITEMLIST_AMMO_LIGHTNING_GENERAL,
	ITEMLIST_AMMO_SLUGS_GENERAL,
	ITEMLIST_AMMO_BFG_GENERAL,
	ITEMLIST_HOLDABLE_TELEPORTER_GENERAL,
	ITEMLIST_HOLDABLE_MEDKIT_GENERAL,
	ITEMLIST_ITEM_QUAD_GENERAL,
	ITEMLIST_ITEM_ENVIRO_GENERAL,
	ITEMLIST_ITEM_HASTE_GENERAL,
	ITEMLIST_ITEM_INVIS_GENERAL,
	ITEMLIST_ITEM_REGEN_GENERAL,
	ITEMLIST_ITEM_FLIGHT_GENERAL,
	ITEMLIST_HOLDABLE_KAMIKAZE_GENERAL,
	ITEMLIST_HOLDABLE_PORTAL_GENERAL,
	ITEMLIST_HOLDABLE_INVULNERABILITY_GENERAL,
	ITEMLIST_AMMO_NAILS_GENERAL,
	ITEMLIST_AMMO_MINES_GENERAL,
	ITEMLIST_AMMO_BELT_GENERAL,
	ITEMLIST_ITEM_SCOUT_GENERAL,
	ITEMLIST_ITEM_GUARD_GENERAL,
	ITEMLIST_ITEM_DOUBLER_GENERAL,
	ITEMLIST_ITEM_ARMORREGEN_GENERAL, // Quake Live
	ITEMLIST_ITEM_AMMOREGEN_GENERAL,
	ITEMLIST_WEAPON_NAILGUN_GENERAL,
	ITEMLIST_WEAPON_PROX_LAUNCHER_GENERAL,
	ITEMLIST_WEAPON_CHAINGUN_GENERAL,
	ITEMLIST_ITEM_BATTERY_GENERAL,

	// Quake Live:
	ITEMLIST_ITEM_SPAWNARMOR_GENERAL,
	ITEMLIST_WEAP_HMG_GENERAL,
	ITEMLIST_AMMO_HMG_GENERAL,
	ITEMLIST_AMMO_PACK_GENERAL,
	ITEMLIST_ITEM_KEY_SILVER_GENERAL,
	ITEMLIST_ITEM_KEY_GOLD_GENERAL,
	ITEMLIST_ITEM_KEY_MASTER_GENERAL,

	// JK2 SP:
	ITEMLIST_WEAPON_ATST_MAIN_GENERAL,
	ITEMLIST_WEAPON_ATST_SIDE_GENERAL,
	ITEMLIST_WEAPON_TIE_FIGHTER_GENERAL,
	ITEMLIST_WEAPON_RAPID_CONCUSSION_GENERAL,
	ITEMLIST_WEAPON_BOTWELDER_GENERAL,
	ITEMLIST_ITEM_GOODIE_KEY_GENERAL,
	ITEMLIST_ITEM_SECURITY_KEY_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_HEAL_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_LEVITATION_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_SPEED_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_PUSH_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_PULL_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_TELEPATHY_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_GRIP_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_LIGHTNING_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_SABERTHROW_GENERAL,
	ITEMLIST_AMMO_EMPLACED_GENERAL,
	
	ITEMLIST_NUM_TOTAL_GENERAL,
} itemList_t;


static const int jk2ItemListToGeneral[] = {
	ITEMLIST_NONE_GENERAL,
	ITEMLIST_ITEM_SHIELD_SM_INSTANT_GENERAL,
	ITEMLIST_ITEM_SHIELD_LRG_INSTANT_GENERAL,
	ITEMLIST_ITEM_MEDPAK_INSTANT_GENERAL,
	ITEMLIST_ITEM_SEEKER_GENERAL,
	ITEMLIST_ITEM_SHIELD_GENERAL,
	ITEMLIST_ITEM_MEDPAC_GENERAL,
	ITEMLIST_ITEM_DATAPAD_GENERAL,
	ITEMLIST_ITEM_BINOCULARS_GENERAL,
	ITEMLIST_ITEM_SENTRY_GUN_GENERAL,
	ITEMLIST_ITEM_FORCE_ENLIGHTEN_LIGHT_GENERAL,
	ITEMLIST_ITEM_FORCE_ENLIGHTEN_DARK_GENERAL,
	ITEMLIST_ITEM_FORCE_BOON_GENERAL,
	ITEMLIST_ITEM_YSALIMARI_GENERAL,
	ITEMLIST_WEAPON_STUN_BATON_GENERAL,
	ITEMLIST_WEAPON_SABER_GENERAL,
	ITEMLIST_WEAPON_BRYAR_PISTOL_GENERAL,
	ITEMLIST_WEAPON_BLASTER_GENERAL,
	ITEMLIST_WEAPON_DISRUPTOR_GENERAL,
	ITEMLIST_WEAPON_BOWCASTER_GENERAL,
	ITEMLIST_WEAPON_REPEATER_GENERAL,
	ITEMLIST_WEAPON_DEMP2_GENERAL,
	ITEMLIST_WEAPON_FLECHETTE_GENERAL,
	ITEMLIST_WEAPON_ROCKET_LAUNCHER_GENERAL,
	ITEMLIST_AMMO_THERMAL_GENERAL,
	ITEMLIST_AMMO_TRIPMINE_GENERAL,
	ITEMLIST_AMMO_DETPACK_GENERAL,
	ITEMLIST_WEAPON_THERMAL_GENERAL,
	ITEMLIST_WEAPON_TRIP_MINE_GENERAL,
	ITEMLIST_WEAPON_DET_PACK_GENERAL,
	ITEMLIST_WEAPON_EMPLACED_GENERAL,
	ITEMLIST_WEAPON_TURRETWP_GENERAL,
	ITEMLIST_AMMO_FORCE_GENERAL,
	ITEMLIST_AMMO_BLASTER_GENERAL,
	ITEMLIST_AMMO_POWERCELL_GENERAL,
	ITEMLIST_AMMO_METALLIC_BOLTS_GENERAL,
	ITEMLIST_AMMO_ROCKETS_GENERAL,
	ITEMLIST_TEAM_CTF_REDFLAG_GENERAL,
	ITEMLIST_TEAM_CTF_BLUEFLAG_GENERAL,
	ITEMLIST_TEAM_CTF_NEUTRALFLAG_GENERAL,
	ITEMLIST_ITEM_REDCUBE_GENERAL,
	ITEMLIST_ITEM_BLUECUBE_GENERAL,
	ITEMLIST_NUM_TOTAL_GENERAL,
};





typedef enum {
	CTFMESSAGE_FRAGGED_FLAG_CARRIER,
	CTFMESSAGE_FLAG_RETURNED,
	CTFMESSAGE_PLAYER_RETURNED_FLAG,
	CTFMESSAGE_PLAYER_CAPTURED_FLAG,
	CTFMESSAGE_PLAYER_GOT_FLAG
} ctfMsg_t;


std::string Q_StripColorAll(std::string string);
void Q_StripColorAll(char* text);


typedef struct {
	double sum;
	double divisor;
} averageHelper_t;




typedef enum {
	FORCE_MASTERY_UNINITIATED,
	FORCE_MASTERY_INITIATE,
	FORCE_MASTERY_PADAWAN,
	FORCE_MASTERY_JEDI,
	FORCE_MASTERY_JEDI_GUARDIAN,
	FORCE_MASTERY_JEDI_ADEPT,
	FORCE_MASTERY_JEDI_KNIGHT,
	FORCE_MASTERY_JEDI_MASTER,
	NUM_FORCE_MASTERY_LEVELS
} forceMastery_t;
extern char* forceMasteryLevels[NUM_FORCE_MASTERY_LEVELS];
extern int forceMasteryPoints[NUM_FORCE_MASTERY_LEVELS];

extern int bgForcePowerCost[NUM_FORCE_POWERS][NUM_FORCE_POWER_LEVELS];

extern const float forceJumpHeight[NUM_FORCE_POWER_LEVELS];
extern const float forceJumpStrength[NUM_FORCE_POWER_LEVELS];




template<qboolean animIsGeneral>
qboolean inline isBackflip(int anim, demoType_t demoType) {

	if constexpr (!animIsGeneral) {
		anim = generalizeGameValue<GMAP_ANIMATIONS>(anim,demoType);
	}
	int animRaw = anim & ~ANIM_TOGGLEBIT;
	//return (qboolean)(
	//	(demoType == DM_15 && animRaw >= BOTH_FLIP_BACK1_15 && animRaw <= BOTH_FLIP_BACK3_15)
	//	|| (demoType == DM_16 && animRaw >= BOTH_FLIP_BACK1 && animRaw <= BOTH_FLIP_BACK3) // TODO JKA?
	//	);
	return (qboolean)(
		(animRaw >= BOTH_FLIP_BACK1_GENERAL && animRaw <= BOTH_FLIP_BACK3_GENERAL)
		);
}
template<qboolean animIsGeneral>
qboolean inline isYellowDFA(int anim, demoType_t demoType) {

	if constexpr (!animIsGeneral) {
		anim = generalizeGameValue<GMAP_ANIMATIONS>(anim, demoType);
	}
	int animRaw = anim & ~ANIM_TOGGLEBIT;
	//return (qboolean)(
	//	(demoType == DM_15 && animRaw >= BOTH_JUMPFLIPSLASHDOWN1_15 && animRaw <= BOTH_JUMPFLIPSTABDOWN_15)
	//	|| (demoType == DM_16 && animRaw >= BOTH_JUMPFLIPSLASHDOWN1 && animRaw <= BOTH_JUMPFLIPSTABDOWN) // TODO JKA?
	//	);
	return (qboolean)(
		(animRaw >= BOTH_JUMPFLIPSLASHDOWN1_GENERAL && animRaw <= BOTH_JUMPFLIPSTABDOWN_GENERAL)
		);
}
template<qboolean animIsGeneral>
qboolean inline isDFA(int anim, demoType_t demoType) {

	if constexpr (!animIsGeneral) {
		anim = generalizeGameValue<GMAP_ANIMATIONS>(anim, demoType);
	}
	int animRaw = anim & ~ANIM_TOGGLEBIT;
	//return (qboolean)(
	//	(demoType == DM_15 && animRaw == BOTH_FORCELEAP2_T__B__15)
	//	|| (demoType == DM_16 && animRaw == BOTH_FORCELEAP2_T__B_) // TODO JKA?
	//	);
	return (qboolean)(
		(animRaw == BOTH_FORCELEAP2_T__B__GENERAL)
		);
}
template<qboolean animIsGeneral>
qboolean inline BG_SaberInSpecialAttack(int anim, demoType_t demoType)
{
	if constexpr (!animIsGeneral) {
		anim = generalizeGameValue<GMAP_ANIMATIONS>(anim, demoType);
	}
	switch (anim & ~ANIM_TOGGLEBIT)
	{
	case BOTH_A2_STABBACK1_GENERAL:
	case BOTH_ATTACK_BACK_GENERAL:
	case BOTH_CROUCHATTACKBACK1_GENERAL:
	case BOTH_ROLL_STAB_GENERAL:
	case BOTH_BUTTERFLY_LEFT_GENERAL:
	case BOTH_BUTTERFLY_RIGHT_GENERAL:
	case BOTH_BUTTERFLY_FL1_GENERAL:
	case BOTH_BUTTERFLY_FR1_GENERAL:
	case BOTH_FJSS_TR_BL_GENERAL:
	case BOTH_FJSS_TL_BR_GENERAL:
	case BOTH_LUNGE2_B__T__GENERAL:
	case BOTH_FORCELEAP2_T__B__GENERAL:
	case BOTH_JUMPFLIPSLASHDOWN1_GENERAL://#
	case BOTH_JUMPFLIPSTABDOWN_GENERAL://#
	case BOTH_JUMPATTACK6_GENERAL:
	case BOTH_JUMPATTACK7_GENERAL:
	case BOTH_SPINATTACK6_GENERAL:
	case BOTH_SPINATTACK7_GENERAL:
	case BOTH_FORCELONGLEAP_ATTACK_GENERAL:
	case BOTH_VS_ATR_S_GENERAL:
	case BOTH_VS_ATL_S_GENERAL:
	case BOTH_VT_ATR_S_GENERAL:
	case BOTH_VT_ATL_S_GENERAL:
	case BOTH_A7_KICK_F_GENERAL:
	case BOTH_A7_KICK_B_GENERAL:
	case BOTH_A7_KICK_R_GENERAL:
	case BOTH_A7_KICK_L_GENERAL:
	case BOTH_A7_KICK_S_GENERAL:
	case BOTH_A7_KICK_BF_GENERAL:
	case BOTH_A7_KICK_RL_GENERAL:
	case BOTH_A7_KICK_F_AIR_GENERAL:
	case BOTH_A7_KICK_B_AIR_GENERAL:
	case BOTH_A7_KICK_R_AIR_GENERAL:
	case BOTH_A7_KICK_L_AIR_GENERAL:
	case BOTH_STABDOWN_GENERAL:
	case BOTH_STABDOWN_STAFF_GENERAL:
	case BOTH_STABDOWN_DUAL_GENERAL:
	case BOTH_A6_SABERPROTECT_GENERAL:
	case BOTH_A7_SOULCAL_GENERAL:
	case BOTH_A1_SPECIAL_GENERAL:
	case BOTH_A2_SPECIAL_GENERAL:
	case BOTH_A3_SPECIAL_GENERAL:
	case BOTH_FLIP_ATTACK7_GENERAL:
	case BOTH_PULL_IMPALE_STAB_GENERAL:
	case BOTH_PULL_IMPALE_SWING_GENERAL:
	case BOTH_ALORA_SPIN_SLASH_GENERAL:
	case BOTH_A6_FB_GENERAL:
	case BOTH_A6_LR_GENERAL:
	case BOTH_A7_HILT_GENERAL:
		return qtrue;
	}
	return qfalse;
}

typedef enum netFieldType_e { // OpenMOHAA
	regular,
	angle,
	animTime,
	animWeight,
	scale,
	alpha,
	coord,
	// This field was introduced in TA.
	coordExtra,
	velocity,
	// not sure what is this, but it's only present in the Mac build (since AA)
	simple
} netFieldType_t;

typedef struct {
	const char* name;
	size_t		offset;
	int		bits;		// 0 = float
	netFieldType_t type; // OpenMOHAA only.
} netField_t;

// using the stringizing operator to save typing...
#ifdef _MSC_VER 
#define	NETF(x) #x,offsetof(entityState_t,x) // Ok you'll think this is completely braindead, but hear me out... MSVC will not allow the offsetof magic in constexpr unless you use THEIR macro. DONT ASK ME WHY
#else
#define	NETF(x) #x,(size_t)&((entityState_t*)0)->x
#endif 

// using the stringizing operator to save typing...
#ifdef _MSC_VER 
#define	PSF(x) #x,offsetof(playerState_t,x) // Ok you'll think this is completely braindead, but hear me out... MSVC will not allow the offsetof magic in constexpr unless you use THEIR macro. DONT ASK ME WHY
#else
#define	PSF(x) #x,(size_t)&((playerState_t*)0)->x
#endif 

static const vec3_t	vec3_origin = { 0,0,0 };
#define NUMVERTEXNORMALS	162
void ByteToDir(int b, vec3_t dir);
void vectoangles(const vec3_t value1, vec3_t angles);

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
float vectoyaw(const vec3_t vec);

void AnglesSubtract(vec3_t v1, vec3_t v2, vec3_t v3); 
float	AngleSubtract(float a1, float a2);

void Matrix4x4Transform(vec4_t vec, float matrix[16], vec4_t out);
void Matrix4x4TransformOut3(vec4_t vec, float matrix[16], vec3_t out);

typedef enum {
	WEAPON_READY,
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_FIRING,
	WEAPON_CHARGING,
	WEAPON_CHARGING_ALT,
	WEAPON_IDLE, //lowered		// NOTENOTE Added with saber
} weaponstate_t;



// Calculate deviation from perfect strafe, if applicable
// Based on CG_StrafeHelper from EternalJk2mv
template <class T>
float calculateStrafeDeviation(T* state, qboolean* isApplicable) { // Handles entitystate and playerstate

	vec3_t	viewAngles;
	vec3_t	velocity;
	int		movementDir;
	int		groundEntityNum;
	float	baseSpeed;
	if constexpr (std::is_same<T, playerState_t>::value) {
		movementDir = ((playerState_t*)state)->movementDir;
		groundEntityNum = ((playerState_t*)state)->groundEntityNum;
		baseSpeed = ((playerState_t*)state)->speed;
		VectorCopy(((playerState_t*)state)->viewangles, viewAngles);
		VectorCopy(((playerState_t*)state)->velocity, velocity);
	}
	else if constexpr (std::is_same<T, entityState_t>::value) {
		movementDir = ((entityState_t*)state)->angles2[YAW];
		groundEntityNum = ((entityState_t*)state)->groundEntityNum;
		baseSpeed = ((entityState_t*)state)->speed;
		VectorCopy(((entityState_t*)state)->apos.trBase, viewAngles);
		VectorCopy(((entityState_t*)state)->pos.trDelta, velocity);
	}
	else {
		if (isApplicable) *isApplicable = qfalse;
		return INFINITY;
	}

	float pmAccel = 10.0f, pmAirAccel = 1.0f, pmFriction = 6.0f, frametime, optimalDeltaAngle;

	const float currentSpeed = (float)sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]);

	usercmd_t cmd = { 0 };

	switch (movementDir) {
	case 0: // W
		cmd.forwardmove = 1; break;
	case 1: // WA
		cmd.forwardmove = 1; cmd.rightmove = -1; break;
	case 2: // A
		cmd.rightmove = -1;	break;
	case 3: // AS
		cmd.rightmove = -1;	cmd.forwardmove = -1; break;
	case 4: // S
		cmd.forwardmove = -1; break;
	case 5: // SD
		cmd.forwardmove = -1; cmd.rightmove = 1; break;
	case 6: // D
		cmd.rightmove = 1; break;
	case 7: // DW
		cmd.rightmove = 1; cmd.forwardmove = 1;	break;
	default:
		break;
	}

	qboolean onGround = (qboolean)(groundEntityNum == ENTITYNUM_WORLD); //sadly predictedPlayerState makes it jerky so need to use cg.snap groundentityNum, and check for cg.snap earlier

	if (currentSpeed < (baseSpeed - 1)) { // Dunno why
		if (isApplicable) *isApplicable = qfalse;
		return INFINITY;
	}

	frametime = 0.001f;// / 142.0f; // Not sure what else I can do with a demo... TODO? 0.001 prevents invalid nan(ind) coming from acos when onground sometimes?
	/*if (cg_strafeHelper_FPS.value < 1)
		frametime = ((float)cg.frametime * 0.001f);
	else if (cg_strafeHelper_FPS.value > 1000) // invalid
		frametime = 1;
	else frametime = 1 / cg_strafeHelper_FPS.value;*/

	if (onGround)//On ground
		optimalDeltaAngle = acos((double)((baseSpeed - (pmAccel * baseSpeed * frametime)) / (currentSpeed * (1 - pmFriction * (frametime))))) * (180.0f / M_PI) - 45.0f;
	else
		optimalDeltaAngle = acos((double)((baseSpeed - (pmAirAccel * baseSpeed * frametime)) / currentSpeed)) * (180.0f / M_PI) - 45.0f;

	if (isnan(optimalDeltaAngle)) { // It happens sometimes I guess...
		if (isApplicable) *isApplicable = qfalse;
		return INFINITY;
	}

	if (optimalDeltaAngle < 0 || optimalDeltaAngle > 360)
		optimalDeltaAngle = 0;

	vec3_t velocityAngle;
	velocity[2] = 0;
	vectoangles(velocity, velocityAngle); //We have the offset from our Velocity angle that we should be aiming at, so now we need to get our velocity angle.
	
	//float cg_strafeHelperOffset = 75; // dunno why or what huh
	float cg_strafeHelperOffset = 0; // dunno why or what huh

	float diff = INFINITY;
	qboolean anyActive = qfalse;
	float smallestAngleDiff = INFINITY;
	if (cmd.forwardmove > 0 && cmd.rightmove < 0) {
		diff = (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f)); // WA
		smallestAngleDiff = std::min(smallestAngleDiff,abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW]+ diff)));
		anyActive = qtrue;
	}
	if (cmd.forwardmove > 0 && cmd.rightmove > 0) {
		diff = (-optimalDeltaAngle - (cg_strafeHelperOffset * 0.01f)); // WD
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
		anyActive = qtrue;
	}
	if (cmd.forwardmove == 0 && cmd.rightmove < 0) {
		anyActive = qtrue;
		// Forwards
		diff = -(45.0f - (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // A
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
		// Backwards
		diff = (225.0f - (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // A
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
	}
	if (cmd.forwardmove == 0 && cmd.rightmove > 0) {
		anyActive = qtrue;
		// Forwards
		diff = (45.0f - (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // D
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
		// Backwards
		diff = (135.0f + (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // D
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
	}

	if (cmd.forwardmove > 0 && cmd.rightmove == 0) {
		anyActive = qtrue;
		// Variation 1
		diff = (45.0f + (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // W
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
		// Variation 2
		diff = (-45.0f - (optimalDeltaAngle + (cg_strafeHelperOffset * 0.01f))); // W
		smallestAngleDiff = std::min(smallestAngleDiff, abs(AngleSubtract(viewAngles[YAW], velocityAngle[YAW] + diff)));
	}
	
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 1); //WA
	//DrawStrafeLine(velocityAngle, , (qboolean)(, 7); //WD
	//DrawStrafeLine(velocityAngle, , (qboolean)(cmd.forwardmove == 0 && cmd.rightmove < 0), 2); //A
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 6); //D
	// Rear stuff? idk									
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 9); //A
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 10); //D
	//W only
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 0); //W
	//DrawStrafeLine(velocityAngle, , (qboolean)(), 0); //W

	// DrawStrafeLine(vec3_t velocity, float diff, qboolean active, int moveDir) 

	if (!anyActive) {
		if (isApplicable) *isApplicable = qfalse;
		return INFINITY;
	}
	else {
		if (isApplicable) *isApplicable = qtrue;
		return smallestAngleDiff;
	}

	//int cg_strafeHelperPrecision = 256; // Dunno why or what or whatever
	//int sensitivity = cg_strafeHelperPrecision;

	//vec3_t angs, forward, delta, line,start;

	//VectorCopy(cg.refdef.vieworg, start);

	//VectorCopy(velocityAngle, angs);
	//angs[YAW] += diff;
	//AngleVectors(angs, forward, NULL, NULL);
	//VectorScale(forward, sensitivity, delta); // line length

	//line[0] = delta[0] + start[0];
	//line[1] = delta[1] + start[1];
	//line[2] = start[2];
	

}



template<qboolean saberMoveIsGeneralized>
qboolean inline PM_SaberInBrokenParry(int move, demoType_t demoType)
{
	if constexpr (!saberMoveIsGeneralized) {

		move = generalizeGameValue<GMAP_LIGHTSABERMOVE>(move, demoType);
	}
	if (move >= LS_V1_BR_GENERAL && move <= LS_V1_B__GENERAL && demoType != DM_15) // This should be actually be check for 1.02 specifically TODO
	{
		return qtrue;
	}
	if (move >= LS_H1_T__GENERAL && move <= LS_H1_BL_GENERAL)
	{
		return qtrue;
	}
	return qfalse;
}

template<qboolean animIsGeneral>
qboolean inline PM_InSaberAnim(int anim, demoType_t demoType)
{
	if constexpr (!animIsGeneral) {
		anim = generalizeGameValue<GMAP_ANIMATIONS>(anim, demoType);
	}
	//if (demoType == DM_16 && (anim & ~ANIM_TOGGLEBIT) >= BOTH_A1_T__B_ && (anim & ~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR
	//	|| demoType == DM_15 && (anim & ~ANIM_TOGGLEBIT) >= BOTH_A1_T__B__15 && (anim & ~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR_15
	//	)
	if ((anim & ~ANIM_TOGGLEBIT) >= BOTH_A1_T__B__GENERAL && (anim & ~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR_GENERAL)
	{
		return qtrue;
	}
	return qfalse;
}

template<qboolean saberMoveIsGeneralized>
qboolean inline BG_SaberInAttack(int move, demoType_t demoType)
{
	if constexpr (!saberMoveIsGeneralized) {
		move = generalizeGameValue<GMAP_LIGHTSABERMOVE>(move, demoType);
	}
	if (move >= LS_A_TL2BR_GENERAL && move <= LS_A_T2B_GENERAL)
	{
		return qtrue;
	}
	switch (move)
	{
	case LS_A_BACK_GENERAL:
	case LS_A_BACK_CR_GENERAL:
	case LS_A_BACKSTAB_GENERAL:
	case LS_ROLL_STAB_GENERAL:
	case LS_A_LUNGE_GENERAL:
	case LS_A_JUMP_T__B__GENERAL:
	case LS_A_FLIP_STAB_GENERAL:
	case LS_A_FLIP_SLASH_GENERAL:
	case LS_JUMPATTACK_DUAL_GENERAL:
	case LS_JUMPATTACK_ARIAL_LEFT_GENERAL:
	case LS_JUMPATTACK_ARIAL_RIGHT_GENERAL:
	case LS_JUMPATTACK_CART_LEFT_GENERAL:
	case LS_JUMPATTACK_CART_RIGHT_GENERAL:
	case LS_JUMPATTACK_STAFF_LEFT_GENERAL:
	case LS_JUMPATTACK_STAFF_RIGHT_GENERAL:
	case LS_BUTTERFLY_LEFT_GENERAL:
	case LS_BUTTERFLY_RIGHT_GENERAL:
	case LS_A_BACKFLIP_ATK_GENERAL:
	case LS_SPINATTACK_DUAL_GENERAL:
	case LS_SPINATTACK_GENERAL:
	case LS_LEAP_ATTACK_GENERAL:
	case LS_SWOOP_ATTACK_RIGHT_GENERAL:
	case LS_SWOOP_ATTACK_LEFT_GENERAL:
	case LS_TAUNTAUN_ATTACK_RIGHT_GENERAL:
	case LS_TAUNTAUN_ATTACK_LEFT_GENERAL:
	case LS_KICK_F_GENERAL:
	case LS_KICK_B_GENERAL:
	case LS_KICK_R_GENERAL:
	case LS_KICK_L_GENERAL:
	case LS_KICK_S_GENERAL:
	case LS_KICK_BF_GENERAL:
	case LS_KICK_RL_GENERAL:
	case LS_KICK_F_AIR_GENERAL:
	case LS_KICK_B_AIR_GENERAL:
	case LS_KICK_R_AIR_GENERAL:
	case LS_KICK_L_AIR_GENERAL:
	case LS_STABDOWN_GENERAL:
	case LS_STABDOWN_STAFF_GENERAL:
	case LS_STABDOWN_DUAL_GENERAL:
	case LS_DUAL_SPIN_PROTECT_GENERAL:
	case LS_STAFF_SOULCAL_GENERAL:
	case LS_A1_SPECIAL_GENERAL:
	case LS_A2_SPECIAL_GENERAL:
	case LS_A3_SPECIAL_GENERAL:
	case LS_UPSIDE_DOWN_ATTACK_GENERAL:
	case LS_PULL_ATTACK_STAB_GENERAL:
	case LS_PULL_ATTACK_SWING_GENERAL:
	case LS_SPINATTACK_ALORA_GENERAL:
	case LS_DUAL_FB_GENERAL:
	case LS_DUAL_LR_GENERAL:
	case LS_HILT_BASH_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}





template<class T>
qboolean WP_SaberCanBlock_Simple(T* state, demoType_t demoType) // TODO MAke support JKA properly? Or too much work...
{

	int saberMove, torsoAnim, saberHolstered, usingATST, weapon, weaponstate, saberInFlight;
	if constexpr (std::is_same<T, playerState_t>::value) {
		saberMove = ((playerState_t*)state)->saberMove;
		torsoAnim = ((playerState_t*)state)->torsoAnim;
		saberHolstered = ((playerState_t*)state)->saberHolstered;
		usingATST = ((playerState_t*)state)->usingATST;
		weapon = ((playerState_t*)state)->weapon;
		weaponstate = ((playerState_t*)state)->weaponstate;
		saberInFlight = ((playerState_t*)state)->saberInFlight;
	}
	else if constexpr (std::is_same<T, entityState_t>::value) {
		saberMove = ((entityState_t*)state)->saberMove;
		torsoAnim = ((entityState_t*)state)->torsoAnim;
		saberHolstered = ((entityState_t*)state)->shouldtarget;
		usingATST = ((entityState_t*)state)->teamowner;
		weapon = ((entityState_t*)state)->weapon;
		weaponstate = ((entityState_t*)state)->modelindex2;
		saberInFlight = ((entityState_t*)state)->saberInFlight;
	}
	else {
		return qfalse;
	}

	saberMove = generalizeGameValue<GMAP_LIGHTSABERMOVE,UNSAFE>(saberMove, demoType);
	torsoAnim = generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(torsoAnim, demoType);

	if (BG_SaberInAttack<qtrue>(saberMove, demoType))
	{
		return qfalse;
	}

	if (PM_InSaberAnim<qtrue>(torsoAnim, demoType) &&
		saberMove != LS_READY_GENERAL && saberMove != LS_NONE_GENERAL)
	{
		if (saberMove < LS_PARRY_UP_GENERAL || saberMove > LS_REFLECT_LL_GENERAL)
		{
			return qfalse;
		}
	}

	if (demoType != DM_15 && PM_SaberInBrokenParry<qtrue>(saberMove,demoType)) // Hmm. This is supposed to only be for 1.02. Idk how it will handle 1.03 TODO
	{
		return qfalse;
	}

	if (saberHolstered)
	{
		return qfalse;
	}

	if (usingATST)
	{
		return qfalse;
	}

	if (generalizeGameValue<GMAP_WEAPONS, UNSAFE>( weapon,demoType) != WP_SABER_GENERAL)
	{
		return qfalse;
	}

	if (weaponstate == WEAPON_RAISING)
	{
		return qfalse;
	}

	if (saberInFlight)
	{
		return qfalse;
	}

	return qtrue;
}






typedef ankerl::unordered_dense::segmented_set<std::string, ankerl::unordered_dense::hash<std::string>> InternedStringTable;

extern InternedStringTable internedStrings;


/*
template<int bits>
class EzBitmask {
	byte data[(bits / 8) + 1] = { 0 };
public:
	inline const bool operator [](size_t bit) {
		return (data[(bit >> 3)] & (1 << (bit & 7)));
	}
	inline void setbit(size_t bit) {
		data[(bit >> 3)] |= (1 << (bit & 7));
	}
	inline void clearbit(size_t bit) {
		data[(bit >> 3)] &= ~(1 << (bit & 7));
	}
};*/

enum class EzBitmaskMemoryType {
	Stack,
	Heap,
};

#define EZBM_DATASIZE ((bits + 7) / 8)
template<size_t bits, EzBitmaskMemoryType memtype>
class EzBitmask {
	byte _dataHeap[memtype == EzBitmaskMemoryType::Stack ? EZBM_DATASIZE : 1] = { 0 }; // meeh it has to be size 1 at least xd
	byte* _data = _dataHeap;
public:
	EzBitmask() {
		if constexpr (memtype == EzBitmaskMemoryType::Heap) {
			_data = new byte[EZBM_DATASIZE]{ 0 };
		}
	}
	~EzBitmask() {
		if constexpr (memtype == EzBitmaskMemoryType::Heap) {
			if (_data) {
				delete[] _data;
				_data = NULL;
			}
		}
	}
	const bool operator [](size_t bit) {
		return (_data[(bit >> 3)] & (1 << (bit & 7)));
	}
	void setbit(size_t bit) {
		_data[(bit >> 3)] |= (1 << (bit & 7));
	}
	void clearbit(size_t bit) {
		_data[(bit >> 3)] &= ~(1 << (bit & 7));
	}
	const byte* data() {
		return _data;
	}
	const byte* release() {
		if constexpr (memtype == EzBitmaskMemoryType::Heap) {
			byte* retVal = _data;
			_data = NULL;
			return retVal;
		}
		else {
			return _data;
		}
	}
	const size_t getDataSize() {
		return EZBM_DATASIZE;
	}
	void clear(size_t count = 0) {
		if constexpr (memtype == EzBitmaskMemoryType::Heap) {
			if (_data) {
				memset(_data, 0, count ? std::min(EZBM_DATASIZE, count) : EZBM_DATASIZE);
			}
		}
		else {
			memset(_data, 0, count ? std::min(EZBM_DATASIZE, count) : EZBM_DATASIZE);
		}
	}
	void fill(size_t count = 0) {
		if constexpr (memtype == EzBitmaskMemoryType::Heap) {
			if (_data) {
				memset(_data, 255, count ? std::min(EZBM_DATASIZE, count) : EZBM_DATASIZE);
			}
		}
		else {
			memset(_data, 255, count ? std::min(EZBM_DATASIZE, count) : EZBM_DATASIZE);
		}
	}
};








typedef ankerl::unordered_dense::map<uint16_t, entityState_t, ankerl::unordered_dense::hash<uint16_t>> SnapshotEntities;
//typedef std::map<uint16_t, entityState_t> SnapshotEntitiesOrdered;
typedef std::map<uint16_t, entityState_t*> SnapshotEntitiesOrderedPointers;
typedef ankerl::unordered_dense::map<uint16_t, int, ankerl::unordered_dense::hash<uint16_t>> EntityIntegerMap;


static inline SnapshotEntitiesOrderedPointers SnapShotEntitiesToOrderedPointers(SnapshotEntities& ents) {
	SnapshotEntitiesOrderedPointers ptrs;
	// it might be slightly faster to use an std::vector with std::pair and then run a single std::sort, however
	// std::map guarantees order so semantically it is better.
	for (auto it = ents.begin(); it != ents.end(); it++) {
		ptrs.emplace(it->first, &it->second);
	}
	return ptrs;
}

// Shared demo parsing functions
class SnapshotInfo {
public:
	int snapNum;
	SnapshotEntities entities;
	EntityIntegerMap playerCommandOrServerTimes;
	bool hasPlayer[MAX_CLIENTS_MAX]{};
	byte mohaaPlayerWeapon[MAX_CLIENTS_MAX]{};
	playerState_t playerState;
	int serverTime;
	int64_t demoTime;
	byte areamask[MAX_MAP_AREA_BYTES];
	bool playerStateTeleport;
	bool snapFlagServerCount; // Used for considering teleports for non-playerstate clients
};






//qboolean demoCutParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t* demoType);
qboolean demoCutParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t* demoType, qboolean isDemoHeader, bool& SEHExceptionCaught);
void demoCutParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType);
//qboolean demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut);
#ifdef MSG_READBITS_TRANSCODE
qboolean demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut, demoType_t demoType, bool& SEHExceptionCaught, qboolean* wasNewCommand = NULL);
#else
qboolean demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut, demoType_t demoType, bool& SEHExceptionCaught);
#endif
qboolean demoCutConfigstringModified(clientActive_t* clCut, demoType_t demoType);
//qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean writeOldSnap = qfalse);
qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, bool& SEHExceptionCaught, bool& malformedMessage, qboolean writeOldSnap = qfalse);
qboolean demoCutParseMOHAASVC(msg_t* msg, demoType_t demoType, byte cmd, bool& SEHExceptionCaught);

void demoCutEmitPacketEntities(clSnapshot_t* from, clSnapshot_t* to, msg_t* msg, clientActive_t* clCut, demoType_t demoType);
void demoCutWriteDemoMessage(msg_t* msg, fileHandle_t f, clientConnection_t* clcCut);
void demoCutWriteEmptyMessageMetadataPart(msg_t* msg, demoType_t demoType, const char* metaData);
void demoCutWriteEmptyMessageWithMetadata(fileHandle_t f, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw, const char* metaData);
const char* demoCutReadPossibleMetadata(msg_t* msg, demoType_t demoType);
qboolean demoCutReadPossibleHiddenUserCMDs(msg_t* msg, demoType_t demoType, bool& SEHExceptionCaught);

void demoCutWriteDemoHeader(fileHandle_t f, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw);
void demoCutWriteDeltaSnapshot(int firstServerCommand, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw);
void demoCutWriteDeltaSnapshotActual(msg_t* msg, clSnapshot_t* frame, clSnapshot_t* oldframe, clientActive_t* clCut, demoType_t demoType, qboolean replicateAreaMaskSize=qfalse);
qboolean demoCutConfigstringModifiedManual(clientActive_t* clCut, int configStringNum, const char* value, demoType_t demoType);
void demoCutEmitPacketEntitiesManual(msg_t* msg, clientActive_t* clCut, demoType_t demoType, SnapshotEntitiesOrderedPointers* entities, SnapshotEntitiesOrderedPointers* fromEntities);
qboolean demoCutInitClearGamestate(clientConnection_t* clcCut, clientActive_t* clCut, int serverCommandSequence, int clientNum, int checksumFeed);
void demoCutWriteDeltaSnapshotManual(std::vector<std::string>* newCommands, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, SnapshotEntitiesOrderedPointers* entities, SnapshotEntitiesOrderedPointers* fromEntities, playerState_t* fromPS,qboolean raw);

const char* jsonGetRealMetadataKeyName(rapidjson::Document* doc, const char* searchName);
std::string printRapidJsonValue(rapidjson::Value* value);

std::string makeConfigStringCommand(int index, std::string value);
std::string makeEntityConfigStringCommand(int index, std::string value);
int G_FindConfigstringIndex(const char* name, int start, int max, qboolean create, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);
int G_SoundIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);
int G_EffectIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);
int G_ModelIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);
int G_GrappleSkinIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);
int G_ModelIndex_NoAdd(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType);

#if USE_CMODEL
class CModel;
void retimeEntity(entityState_t* entity, double newServerTime, double newDemoTime, demoType_t demoType, CModel* cm = NULL);
#else
void retimeEntity(entityState_t* entity, double newServerTime, double newDemoTime, demoType_t demoType);
#endif
qboolean demoCutGetDemoType(const char* demoFile, char extOutput[7], char outputNameNoExt[MAX_OSPATH], demoType_t* demoType, qboolean* isCompressed, clientConnection_t* clcCut = NULL);
int64_t demoCutGetDemoNameTruncationOffset(const char* demoName);

int Q_irand(int value1, int value2, qboolean useDefault, int defaultValue);

int strNumLen(const char* s);
int atoiWhileNumber(const char* s);
int atoiWhileNumber(const char** s);




typedef enum {
	UNSAFE,
	SAFE
} arrayAccessType_t;

extern void initializeGameInfos();

template <class T>
struct wannabeArray_t {
	T* data;
	int count;
	int offset; // Like when we need to map from something that could be -1. Otherwise not possible.
	//inline T& operator[](const size_t& index)
	//{
	//	return data[index+offset];
	//}
	template<arrayAccessType_t A>
	inline T& get(const size_t& index)
	{
		if constexpr (A == SAFE) { // Somtimes entity values might be used for random stuff and have weird values. For those we call safe.

			if ((index + offset) >= count || (index + offset) < 0) {
				return NULL;
			}
			else {
				return data[index + offset];
			}
		}
		else {
			return data[index + offset];
		}
	}
};


struct gameNetFieldInfo_t {
	wannabeArray_t<const netField_t> entityStateFields;
	wannabeArray_t<const netField_t> playerStateFields;
	//const netField_t* entityStateFields;
	//int					entityStateFieldsNum;
	//const netField_t* playerStateFields;
	//int					playerStateFieldsNum;
	qboolean			playerStateFieldsRequireSpecialHandling;
};

struct gameConstantsInfo_t {
	int cs_models;
	int cs_sounds;
	int cs_players;
	int	et_events;
	int ef_missile_stick;
	int anim_togglebit;
	int cs_level_start_time;
	int ef_teleportbit;
	int cs_max;
	int cs_effects;
};

#define MAX_SPECIALIZED_MAPPINGS 5	// If this ever isnt enough, just increase it.
#define MAX_MAPPING_TARGET_DEMOTYPES 5	// If this ever isnt enough, just increase it.

struct specializedGameMapping_t {
	demoType_t targetDemoTypes[MAX_MAPPING_TARGET_DEMOTYPES];
	wannabeArray_t<const int> mapping;
};
struct specializedGameMappingsContainer_t {
	wannabeArray_t<const int> mapping;
};

struct gameInfoMapping_t {
	specializedGameMapping_t	specializedMappings[MAX_SPECIALIZED_MAPPINGS];
	wannabeArray_t<const int>	mapping;
	wannabeArray_t<const int>	reversedMapping; // auto-filled
};

enum gameMappingType_t { // When changing this, also update gameMappingTypeGeneralArrayLength array
	GMAP_EVENTS,
	GMAP_WEAPONS,
	GMAP_MEANSOFDEATH,
	GMAP_LIGHTSABERMOVE,
	GMAP_ITEMLIST,
	GMAP_ANIMATIONS,
	GMAP_ENTITYTYPE,
	GMAP_GAMETYPE,
	GMAP_PLAYERMOVETYPE,
	GAMEMAPPINGTYPES_COUNT
};

static const int gameMappingTypeGeneralArrayLength[GAMEMAPPINGTYPES_COUNT] {
	EV_ENTITY_EVENT_COUNT_GENERAL+1, //GMAP_EVENTS,
	WP_NUM_WEAPONS_GENERAL+1,//GMAP_WEAPONS,
	MOD_MAX_GENERAL+1, //GMAP_MEANSOFDEATH,
	LS_MOVE_MAX_GENERAL+2, //GMAP_LIGHTSABERMOVE // Is +2 because it has the value -1 as well. This also needs to have an offset applied!
	ITEMLIST_NUM_TOTAL_GENERAL+1,
	MAX_ANIMATIONS_GENERAL+1,
	ET_EVENTS_GENERAL+1,
	GT_MAX_GAME_TYPE_GENERAL+1,
	PM_TYPES_COUNT+1,
}; // + 1 because we wanna map the _MAX values too. Let's not cause writing to random memory locations and crash :)

static const int gameMappingTypeGeneralValueOffset[GAMEMAPPINGTYPES_COUNT] {
	NULL, //GMAP_EVENTS,
	NULL,//GMAP_WEAPONS,
	NULL, //GMAP_MEANSOFDEATH,
	1, //GMAP_LIGHTSABERMOVE // Offset 1 because the real smallest value is -1 in this enum. We can't index an array with -1!
	0, // GMAAP_ITEMLIST
	0, // GMAP_ANIMATIONS
	0, // GMAP_ENTITYTYPE
	0, // GMAP_GAMETYPE
	0, // GMAP_PLAYERMOVETYPE
}; // + 1 because we wanna map the _MAX values too. Let's not cause writing to random memory locations and crash :)

//struct gameMappings_t {
	//gameInfoMapping_t	
	//specializedGameMapping_t				specializedEventMappings[MAX_SPECIALIZED_MAPPINGS];	// Mapping to a target demo type directly
	//wannabeArray_t<const entity_event_t>	eventMappings;				// Mapping to general
	//specializedGameMapping_t				specializedWeaponMappings[MAX_SPECIALIZED_MAPPINGS];	// Mapping to a target demo type directly
	//wannabeArray_t<const weapon_t>			weaponMappings;				// Mapping to general

	//wannabeArray_t<const int>				eventMappingsReverse;		// Auto-generated. From general events to specific of this game
	//wannabeArray_t<const int>				weaponMappingsReverse;		// Auto-generated. From general events to specific of this game
//};

// Multi game support related
struct gameInfo_t {
	demoType_t						demoType;
	svc_ops_e_general				opsToGeneral[svc_ops_general_count];
	//gameMappings_t					mappings;
	gameInfoMapping_t				mappings[GAMEMAPPINGTYPES_COUNT];
	gameNetFieldInfo_t				netFieldInfo;
	int								maxConfigstrings;
	gameConstantsInfo_t				constants;
	int								maxClients;

	// Is auto-filled by democutter tools.
	int generalToOps[svc_ops_general_count]; 
	qboolean						isDefault;
};

extern gameInfo_t* gameInfosMapped[DEMOTYPE_COUNT];
extern specializedGameMappingsContainer_t specializedMappings[GAMEMAPPINGTYPES_COUNT][DEMOTYPE_COUNT][DEMOTYPE_COUNT];

//inline svc_ops_e_general generalizeGameSVCOp(int gameOp, demoType_t demoType);
//inline int specializeGeneralSVCOp(svc_ops_e_general generalOp, demoType_t demoType);
//inline qboolean getEntityStateFields(netField_t** fields, int* fieldCount, demoType_t demoType);
//inline qboolean getPlayerStateFields(netField_t** fields, int* fieldCount, demoType_t demoType);
//inline qboolean playerStateRequiresSpecialHandling(demoType_t demoType);

inline svc_ops_e_general generalizeGameSVCOp(int gameOp, demoType_t demoType) {
	return gameInfosMapped[demoType]->opsToGeneral[gameOp];
}

inline int specializeGeneralSVCOp(svc_ops_e_general generalOp, demoType_t demoType) {
	return gameInfosMapped[demoType]->generalToOps[generalOp];
}

inline qboolean getEntityStateFields(const netField_t** fields, int* fieldCount, demoType_t demoType) {
	*fields = gameInfosMapped[demoType]->netFieldInfo.entityStateFields.data;
	*fieldCount = gameInfosMapped[demoType]->netFieldInfo.entityStateFields.count;
	return qtrue;
}
inline qboolean getPlayerStateFields(const netField_t** fields, int* fieldCount, demoType_t demoType) {
	*fields = gameInfosMapped[demoType]->netFieldInfo.playerStateFields.data;
	*fieldCount = gameInfosMapped[demoType]->netFieldInfo.playerStateFields.count;
	return qtrue;
}
inline qboolean playerStateRequiresSpecialHandling(demoType_t demoType) {
	return gameInfosMapped[demoType]->netFieldInfo.playerStateFieldsRequireSpecialHandling;
}
inline int getMaxConfigStrings(demoType_t demoType) {
	return gameInfosMapped[demoType]->maxConfigstrings;
}
inline int getCS_PLAYERS(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_players;;
}
inline int getCS_MODELS(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_models;
}
inline int getCS_SOUNDS(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_sounds;
}
inline int getCS_EFFECTS(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_effects;
}
inline int getCS_LEVEL_START_TIME(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_level_start_time;
}
inline int getET_EVENTS(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.et_events;
}
inline int getEF_MISSILE_STICK(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.ef_missile_stick;
}
inline int getANIM_TOGGLEBIT(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.anim_togglebit;
}
inline int getEF_TELEPORTBIT(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.ef_teleportbit;
}
inline int getCS_MAX(demoType_t demoType) {
	return gameInfosMapped[demoType]->constants.cs_max;
}
inline int getMAX_CLIENTS(demoType_t demoType) {
	return gameInfosMapped[demoType]->maxClients;
}



// Use this in special cases where you know you have created that specialized mapping, otherwise this will crash the software.
template<gameMappingType_t T, arrayAccessType_t A>
inline int specializedGameValueMapUnsafe(int value, demoType_t sourceDemoType, demoType_t targetDemoType) {
	if constexpr (T == GMAP_EVENTS) {
		return specializedMappings[T][sourceDemoType][targetDemoType].mapping.get<A>(value & ~EV_EVENT_BITS) | ((value)&EV_EVENT_BITS);
	}
	else if constexpr (T == GMAP_ANIMATIONS) {
		int& sourceAnimToggleBit = gameInfosMapped[sourceDemoType]->constants.anim_togglebit;
		int& targetAnimToggleBit = gameInfosMapped[targetDemoType]->constants.anim_togglebit;
		return specializedMappings[T][sourceDemoType][targetDemoType].mapping.get<A>(value & ~sourceAnimToggleBit) | ((value & sourceAnimToggleBit) * targetAnimToggleBit / sourceAnimToggleBit);
	}
	else if constexpr (T == GMAP_ENTITYTYPE) {
		throw std::exception("GMAP_ENTITYTYPE specialized map not implemented");
		return 0;
	}
	else {

		return specializedMappings[T][sourceDemoType][targetDemoType].mapping.get<A>(value);
	}
}



// Tries a specialized mapping (if exists) or falls back to a more standard approach
template<gameMappingType_t T, arrayAccessType_t A>
inline int convertGameValue(int value, demoType_t sourceDemoType, demoType_t targetDemoType) {
	if (specializedMappings[T][sourceDemoType][targetDemoType].mapping.data) {
		return specializedGameValueMapUnsafe<T,A>(value,sourceDemoType,targetDemoType);
	}
	else {
		// Use specialized to general mapping of source demotype first, then use reverse general to specialized mapping of target demo type
		if constexpr (T == GMAP_EVENTS) {

			int generalValue = gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value & ~EV_EVENT_BITS);
			return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(generalValue) | ((value)&EV_EVENT_BITS);
		}
		else  if constexpr (T == GMAP_ENTITYTYPE) {
			if (value >= gameInfosMapped[sourceDemoType]->constants.et_events) {
				int generalEvent = gameInfosMapped[sourceDemoType]->mappings[GMAP_EVENTS].mapping.get<A>(value - gameInfosMapped[sourceDemoType]->constants.et_events);
				return gameInfosMapped[targetDemoType]->constants.et_events + gameInfosMapped[targetDemoType]->mappings[GMAP_EVENTS].reversedMapping.get<A>(generalEvent);
			}
			else {
				int generalValue = gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value);
				return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(generalValue);
			}
		}
		else  if constexpr (T == GMAP_ANIMATIONS) {

			int& sourceAnimToggleBit = gameInfosMapped[sourceDemoType]->constants.anim_togglebit;
			int& targetAnimToggleBit = gameInfosMapped[targetDemoType]->constants.anim_togglebit;
			int generalValue = gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value & ~sourceAnimToggleBit);
			return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(generalValue) | ((value&sourceAnimToggleBit)*targetAnimToggleBit/sourceAnimToggleBit);
		}
		else  {

			int generalValue = gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value);
			return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(generalValue);
		}
	}
}

//#define MapQ3AnimToJK2(anim) (q3AnimMapping[(anim)&~ANIM_TOGGLEBIT_Q3] | (((anim)&ANIM_TOGGLEBIT_Q3)*ANIM_TOGGLEBIT/ANIM_TOGGLEBIT_Q3))


// TODO Make safe variant for stuff like .weapon that could be random stuff
template<gameMappingType_t T, arrayAccessType_t A>
inline int generalizeGameValue(int value, demoType_t sourceDemoType) {
	if constexpr (T == GMAP_EVENTS) {
		return gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value & ~EV_EVENT_BITS) | ((value)&EV_EVENT_BITS);
	}
	else  if constexpr (T == GMAP_ENTITYTYPE) {
		if (value >= gameInfosMapped[sourceDemoType]->constants.et_events) {
			return ET_EVENTS_GENERAL + gameInfosMapped[sourceDemoType]->mappings[GMAP_EVENTS].mapping.get<A>(value - gameInfosMapped[sourceDemoType]->constants.et_events);
		}
		else {
			return gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value);
		}
	}
	else if constexpr (T == GMAP_ANIMATIONS) {
		int& sourceAnimToggleBit = gameInfosMapped[sourceDemoType]->constants.anim_togglebit;
		return gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value & ~sourceAnimToggleBit) | ((value&sourceAnimToggleBit)*ANIM_TOGGLEBIT/sourceAnimToggleBit);
	}
	else  {
		return gameInfosMapped[sourceDemoType]->mappings[T].mapping.get<A>(value);
	}
}

template<gameMappingType_t T, arrayAccessType_t A>
inline int specializeGameValue(int value, demoType_t targetDemoType) {
	if constexpr (T == GMAP_EVENTS) {
		return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>((value & ~EV_EVENT_BITS)) | ((value)&EV_EVENT_BITS);
	}
	else  if constexpr (T == GMAP_ENTITYTYPE) {
		if (value >= ET_EVENTS_GENERAL) {
			return gameInfosMapped[targetDemoType]->constants.et_events + gameInfosMapped[targetDemoType]->mappings[GMAP_EVENTS].reversedMapping.get<A>(value - ET_EVENTS_GENERAL);
		}
		else {
			return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(value);
		}
	}
	else  if constexpr (T == GMAP_ANIMATIONS) {
		int& targetAnimToggleBit = gameInfosMapped[targetDemoType]->constants.anim_togglebit;
		return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>((value & ~ANIM_TOGGLEBIT)) | ((value& ANIM_TOGGLEBIT)*targetAnimToggleBit/ANIM_TOGGLEBIT);
	}
	else  {
		return gameInfosMapped[targetDemoType]->mappings[T].reversedMapping.get<A>(value);
	}
}

inline bool isCrouchAnim(int legsAnim, demoType_t demoType) {
	int generalizedLegsAnim = generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(legsAnim, demoType);

	return (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_GENERAL //|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_15
		|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALKBACK_GENERAL || (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALK_GENERAL;
}
inline bool isGeneralizedCrouchAnim(animNumberGeneral_t generalizedLegsAnim, demoType_t demoType) {

	return (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_GENERAL //|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_15
		|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALKBACK_GENERAL || (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALK_GENERAL;
}







inline bool demoTypeIsMOHAA(demoType_t demoType) {
	return demoType == DM3_MOHAA_PROT_15 || demoType == DM3_MOHAA_PROT_6;
}


int getMOHTeam(entityState_t* s);

#define SHIELD_HALFTHICKNESS		4
void IntegerToBoundingBox(int num, int forceFieldTime2, vec3_t mins, vec3_t maxs, demoType_t demoType);


/*
template<gameMappingType_t T>
inline int getGameValueMaxCount(int value, demoType_t sourceDemoType) {
	return gameInfosMapped[sourceDemoType]->mappings[T].mapping.count-1; // Since these arrays attach a _COUNT element at the end, the size is actually 1 bigger than the number we want here.
}*/

/*

// Tries a specialized mapping (if exists) or falls back to a more standard approach
inline int convertWeapon(int weapon, demoType_t sourceDemoType, demoType_t targetDemoType) {
	if (specializedMappings[sourceDemoType][targetDemoType].weaponMapping.data) {
		return specializedMappings[sourceDemoType][targetDemoType].weaponMapping.data[weapon];
	}
	else {
		// Use specialized to general mapping of source demotype first, then use reverse general to specialized mapping of target demo type
		weapon_t generalValue = gameInfosMapped[sourceDemoType]->mappings[GMAP_WEAPONS].mapping.data[weapon];
		return gameInfosMapped[targetDemoType]->mappings[GMAP_WEAPONS].mappingReverse.data[generalValue];
	}
}

inline int generalizeWeapon(int weapon, demoType_t sourceDemoType) {
	return gameInfosMapped[sourceDemoType]->mappings[GMAP_WEAPONS].mapping.data[weapon];
}

// Use this in special cases where you know you have created that specialized mapping, otherwise this will crash the software.
inline int specializedWeaponMapUnsafe(int weapon, demoType_t sourceDemoType, demoType_t targetDemoType) {
	return specializedMappings[sourceDemoType][targetDemoType].weaponMapping.data[weapon];
}*/


entityState_t* parseMOHAADeathMessage(tsl::htrie_map<char, int>* playerMapClientNumMap, char* message);
entityState_t* parseMOHAAPrintDeathMsgFromTokenized(tsl::htrie_map<char, int>* playerMapClientNumMap);

typedef enum chatType_t
{
	CHATTYPE_NONE,
	CHATTYPE_PRIVATE,
	CHATTYPE_TEAM,
	CHATTYPE_PUBLIC
};
class parsedChatMessage_t
{
public:
	bool isValid = false;
	std::string playerName = "";
	int playerNum = -1;
	chatType_t type = CHATTYPE_PUBLIC;
	std::string message = "";
	bool commandComesFromJKWatcher = false;
};

bool	isNumber(const char* str);
int		getBodyPartFromHitLoc(vec3_t entpos, vec3_t entangles, int hitLoc, vec3_t roughHitLoc);
#define MV_VersionMagic_g2ModelParts( limbType, demoType ) ( demoType != DM_15 ? limbType : (limbType >= G2_MODELPART_RHAND ? limbType-1 : limbType) )
#define VersionMagic_SPtoMPTrType( trType ) ( trType > TR_LINEAR_STOP ? trType-1 : trType) 

static const char privateChatBegin[]{ "[" };
static const char teamChatBegin[]{ "(" };
static const char privateChatSeparator[]{ "^7]: ^6" };
static const char teamChatSeparator[]{ "^7): ^5" };
static const char publicChatSeparator[]{ "^7: ^2" };

template<int max_clients>
inline parsedChatMessage_t ParseChatMessage(clientActive_t* clCut, demoType_t demoType, const char* nameChatSegmentA, const char* extraArgument = NULL)
{
	parsedChatMessage_t retVal;

	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	int sentNumber = -1;

	if (extraArgument) // This is jka only i think, sending the client num as an extra
	{
		if (!isNumber(extraArgument))
		{
			std::cerr << "Chat message parsing, received extra argument that is not a number: " << (extraArgument ? extraArgument : "") << " (" << nameChatSegmentA << ")\n";
		}
		else
		{
			sentNumber = atoi(extraArgument);
		}
	}

	const char* nameChatSegment = nameChatSegmentA;
	chatType_t detectedChatType = CHATTYPE_PUBLIC;
	if (strlen(nameChatSegment) < (sizeof(privateChatBegin) - 1))
	{
		return retVal; // Idk
	}

	const char* separator;
	if (!_strnicmp(nameChatSegment, privateChatBegin, sizeof(privateChatBegin) - 1))
	{
		detectedChatType = CHATTYPE_PRIVATE;
		nameChatSegment = nameChatSegment += sizeof(privateChatBegin) - 1;
		separator = privateChatSeparator;
	}
	if (!_strnicmp(nameChatSegment, teamChatBegin, sizeof(teamChatBegin) - 1))
	{
		detectedChatType = CHATTYPE_TEAM;
		nameChatSegment = nameChatSegment += sizeof(teamChatBegin) - 1;
		separator = teamChatSeparator;
	}
	else
	{
		separator = publicChatSeparator;
	}

	int separatorLength = strlen(separator);

	//string[] nameChatSplits = nameChatSegment.Split(new string[]{ separator }, StringSplitOptions.None);
	std::vector<std::string> nameChatSplits = splitString(nameChatSegment, separator, false, true);

	std::vector<int> possiblePlayers;
	if (nameChatSplits.size() > 2)
	{
		// WTf. Someone is messing around and having a complete meme name or meme message consisting of the separator sequence.
		// Let's TRY find out who it is.
		std::stringstream sb;
		for (int i = 0; i < nameChatSplits.size() - 1; i++)
		{
			if (i != 0)
			{
				sb << separator;
			}
			sb << nameChatSplits[i];
			std::string possibleNameStr = sb.str();
			const char* possibleName = possibleNameStr.c_str();
			/*foreach(PlayerInfo playerInfo in infoPool.playerInfo)
			{
				if (playerInfo.infoValid && playerInfo.name == possibleName)
				{
					possiblePlayers.Add(playerInfo.clientNum);
				}
			}*/
			for (int clientNum = 0; clientNum < max_clients; clientNum++) {

				int stringOffset = clCut->gameState.stringOffsets[CS_PLAYERS_here + clientNum];
				const char* playerInfo = clCut->gameState.stringData + stringOffset;

				if (!*playerInfo) continue;

				const char* playerNameCompare = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");

				if (!_stricmp(playerNameCompare, possibleName)) {
					possiblePlayers.push_back(clientNum);
				}
			}
		}
	}
	else
	{
		for (int clientNum = 0; clientNum < max_clients; clientNum++) {

			int stringOffset = clCut->gameState.stringOffsets[CS_PLAYERS_here + clientNum];
			const char* playerInfo = clCut->gameState.stringData + stringOffset;

			if (!*playerInfo) continue;

			const char* playerNameCompare = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");

			if (!_stricmp(playerNameCompare, nameChatSplits[0].c_str())) {
				possiblePlayers.push_back(clientNum);
			}
		}
		/*foreach(PlayerInfo playerInfo in infoPool.playerInfo)
		{
			if (playerInfo.infoValid && playerInfo.name == nameChatSplits[0])
			{
				possiblePlayers.Add(playerInfo.clientNum);
			}
		}*/
	}

	int playerNum = -1;

	if (possiblePlayers.size() == 0)
	{
		if (sentNumber == -1)
		{
			std::cerr << "Could not identify sender of (t)chat message. Zero matches: " << nameChatSegmentA << "\n";
			return retVal;
		}
		else
		{
			std::cerr << "Could not identify sender of (t)chat message (" << nameChatSegmentA << "), zero matches, but extra argument number " << sentNumber << " helped.\n";
			playerNum = sentNumber;
		}
	}
	else if (possiblePlayers.size() > 1)
	{
		if (sentNumber == -1)
		{
			std::cerr << "Could not reliably identify sender of (t)chat message. More than 1 match: " << nameChatSegmentA << ".\n";
			return retVal;
		}
		else
		{
			int confirmedNumber = -1;
			for (int i = 0; i < possiblePlayers.size(); i++)
			{
				if (possiblePlayers[i] == sentNumber)
				{
					confirmedNumber = sentNumber;
				}
			}
			if (confirmedNumber == -1)
			{
				std::cerr << "Could not reliably identify sender of (t)chat message. More than 1 match: " << nameChatSegmentA << " and extra argument number " << sentNumber << " matched none.\n";
				return retVal;
			}
			else
			{
				std::cerr << "Could not reliably identify sender of (t)chat message (" << nameChatSegmentA << "), but extra argument number " << sentNumber << " cleared it up.\n";
				playerNum = confirmedNumber;
			}
		}
	}
	else
	{
		playerNum = possiblePlayers[0];
	}

	int stringOffset = clCut->gameState.stringOffsets[CS_PLAYERS_here + playerNum];
	const char* playerInfo = clCut->gameState.stringData + stringOffset;

	if (!*playerInfo) return retVal;

	const char* playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");

	if (!*playerName)
	{
		// Shouldn't really happen here in demo tools but let's keep it for safety.
		std::cerr << "Received message from player whose name is now null, wtf.";
		return retVal;
	}

	const char* messageRaw = nameChatSegment += strlen(playerName) + separatorLength;
	//int[] ourClientNums = serverWindow.getJKWatcherClientNums();
	const char jkwatcherprivprefix[]{ "   " };
	bool commandComesFromJKWatcher = /*ourClientNums.Contains(playerNum) || */(!_strnicmp(messageRaw, jkwatcherprivprefix, sizeof(jkwatcherprivprefix) - 1) && detectedChatType == CHATTYPE_PRIVATE);

	// return new ParsedChatMessage(){ message = messageRaw.Trim(), playerName = playerName, playerNum = playerNum, type = detectedChatType, commandComesFromJKWatcher = commandComesFromJKWatcher };
	retVal.isValid = true;
	retVal.commandComesFromJKWatcher = commandComesFromJKWatcher;
	retVal.message = messageRaw;
	retVal.playerName = playerName;
	retVal.playerNum = playerNum;
	retVal.type = detectedChatType;
	return retVal;
}








// Meta event related
typedef enum metaEventType_t {
	METAEVENT_TEAMCAPTURE,
	METAEVENT_ENEMYTEAMCAPTURE,
	METAEVENT_CAPTURE,
	METAEVENT_RETURN,
	METAEVENT_KILL,
	METAEVENT_DEATH,
	METAEVENT_JUMP,
	METAEVENT_SABERHIT, // any kind of saber hit, regardless of who is hit or who is ttacking
	METAEVENT_SABERBLOCK, // any saber block, no matter by who or to who
	METAEVENT_EFFECT, // effect event of any sort
	METAEVENT_LAUGH, // effect event of any sort
	//METAEVENT_HIGHLIGHT, // highlight of the demo. not really used, internal for democutter tools for merging and such
	METAEVENT_FLAGGRAB, // effect event of any sort
	METAEVENT_COUNT
};

extern const char* metaEventKeyNames[METAEVENT_COUNT];
struct MetaEventItem {
	int relativeTime; // Relative to the kill
	metaEventType_t type;
};





// Freedomdefrag
#define MAX_COURSE_COUNT 200
#define COURSENAME_MAX_LEN 100
#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 10
#define PASSWORD_MIN_LEN 6
#define PASSWORD_MAX_LEN 50 
#define MAX_NETNAME_FREEDOMDEFRAG			64		// was 36
typedef struct raceStyle_s {
	byte movementStyle; // jk2. maybe some day pjk2 => STAT_MOVEMENTSTYLE
	short msec; // -1 if toggle, -2 if float (ignore float for now, its cringe anyway)
	signed char jumpLevel; // 0=no force, -1 = ysal, -2 = ?, 4=jumpcharge?
	short variant; // when we have map variants (invis walls and such). 0 =default (ignore for now)
	short runFlags; // flags from runFlags_t => STAT_RUNFLAGS
} raceStyle_t;
typedef enum mainLeaderboardType_s {
	LB_MAIN,
	LB_NOJUMPBUG, // main fps but nojumpbug
	LB_CUSTOM, // other fps, segmented, etc
	LB_SEGMENTED, // main fps with or without jumpbug, segmented
	LB_CHEAT, // strafebot, tas
	LB_TYPES_COUNT
} mainLeaderboardType_t;
typedef enum pbFlags_s { // bit flags
	PB_FIRSTRUN_SPECIFICSTYLE = 1,
	PB_NEWPB_SPECIFICSTYLE = 2,
	PB_LB = 4, // Leaderboards sum up various style ranges. e.g. main LB allows 125 and 142 fps runs etc. so pb in ultra specific style is not same as lb pb
} pbFlags_t;
class defragRunMeta_t {
public:
	bool		isSet = false;
	int64_t		demoTime = 0;
	int64_t		lastGameStateChangeInDemoTime = 0;
	int			playerNum = -1;
	bool		wasFollowed = false;
	bool		wasVisible = false;
	bool		wasVisibleOrFollowed = false;
	int			serverTime = 0;
	int			demoRecorderClientNum;
	std::string	serverName;
	std::string mapName;
	std::string playerName;
	double		averageStrafeDeviation = 0.0f;
	bool		averageStrafeDeviationSet = false;
};
typedef class finishedRunInfo_t {
public:
	int			runId = 0;
	int			clientNum = 0;
	int			userId = 0;
	int			milliseconds = 0;
	int			levelTimeStart = 0;
	int			levelTimeEnd = 0;
	int			endCommandTime = 0;
	int			startLessTime = 0;
	int			endLessTime = 0;
	int			warningFlags = 0;
	float		topspeed = 0;
	float		average = 0; // excluding dropped time (due
	float		distance = 0;
	float		distanceXY = 0;
	raceStyle_t raceStyle = { 0 };
	int			savePosCount = 0;
	int			resposCount = 0;
	int			lostMsecCount = 0;
	int			lostPacketCount = 0;
	int			placeHolder1 = 0;
	int			placeHolder2 = 0;
	int			placeHolder3 = 0;
	int			placeHolder4 = 0;
	int			millisecondsSegmentedTotal = 0;
	float		rollSpeed = 0;
	int			rollTakeoffClientSpeed = 0;
	float		startTriggerSpeed = 0;
	int			pbStatus = 0; // see pbFlags_t
	int			rankLB = 0;
	char		coursename[COURSENAME_MAX_LEN + 1] = { 0 };
	char		subcoursename[COURSENAME_MAX_LEN + 1] = { 0 };
	char		username[USERNAME_MAX_LEN + 1] = { 0 };
	int			unixTimeStampShifted = 0;
	int			unixTimeStampShiftedBillionCount = 0;
	char		netname[MAX_NETNAME_FREEDOMDEFRAG] = { 0 };
	mainLeaderboardType_t lbType = LB_MAIN;
	char		tempDemoName[MAX_QPATH] = { 0 };
	char		fpsString[255 + 1] = {0};
	int			checksumBsp = 0;
	int			checksumPak = 0;

	// demotools
	std::string	stylename;
	std::string	playerName;
	defragRunMeta_t	meta;
};
// Freedomdefrag end
















#include "whereami.h" // cringe but cba adding it to every project just to get a stupid path. thanks for the neat library tho


#endif


