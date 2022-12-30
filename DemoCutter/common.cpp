#include "demoCut.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <jkaStuff.h>
#include "otherGameStuff.h"
#include <stateFields.h>
#include <sstream>

// Code is 99%-100% from jomme, from various files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//


static	int			cmd_argc;
static	char* cmd_argv[MAX_STRING_TOKENS];		// points into cmd_tokenized
static	char		cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];	// will have 0 bytes inserted



vec_t VectorLength(const vec3_t v) {
	return (vec_t)sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
vec_t VectorLength2(const vec2_t v) {
	return (vec_t)sqrtf(v[0] * v[0] + v[1] * v[1]);
}

vec_t VectorNormalize(vec3_t v) {
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);

	if (length) {
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}


int		Cmd_Argc(void) {
	return cmd_argc;
}
char* Cmd_Argv(int arg) {
	if ((unsigned)arg >= cmd_argc) {
		return "";
	}
	return cmd_argv[arg];
}

void Q_strncpyz(char* dest, int destCapacity, const char* src, int destsize) {
	// bk001129 - also NULL dest
	if (!dest) {
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL dest");
	}
	if (!src) {
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL src");
	}
	if (destsize < 1) {
		Com_Error(ERR_FATAL, "Q_strncpyz: destsize < 1");
	}

	//strncpy(dest, src, destsize - 1);
	strncpy_s(dest,destCapacity, src, destsize - 1);
	dest[destsize - 1] = 0;
}

void QDECL Com_Printf(const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	//vsprintf(msg, fmt, argptr);
	vsprintf_s(msg,sizeof(msg), fmt, argptr);
	va_end(argptr);

	std::cout << msg;
}
void QDECL Com_DPrintf(const char* fmt, ...) {
#ifdef DEBUG
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	//vsprintf(msg, fmt, argptr);
	vsprintf_s(msg,sizeof(msg), fmt, argptr);
	va_end(argptr);

	std::cout << msg;
#endif
}
void QDECL Com_Error(int ignore, const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	//vsprintf(msg, fmt, argptr);
	vsprintf_s(msg,sizeof(msg), fmt, argptr);
	va_end(argptr);

	std::cout << msg;
}
char* Cmd_ArgsFrom(int arg) {
	static	char		cmd_args[BIG_INFO_STRING];
	int		i;

	cmd_args[0] = 0;
	if (arg < 0)
		arg = 0;
	for (i = arg; i < cmd_argc; i++) {
		//strcat(cmd_args, cmd_argv[i]);
		strcat_s(cmd_args,sizeof(cmd_args), cmd_argv[i]);
		if (i != cmd_argc - 1) {
			//strcat(cmd_args, " ");
			strcat_s(cmd_args,sizeof(cmd_args), " ");
		}
	}

	return cmd_args;
}


void Com_Memcpy(void* dest, const void* src, const size_t count)
{
	memcpy(dest, src, count);
}
void Com_Memset(void* dest, const int val, const size_t count)
{
	memset(dest, val, count);
}



int Q_vsnprintf(char* str,int capacity, size_t size, const char* format, va_list ap) {
	int retval;
	//retval = _vsnprintf(str, size, format, ap);
	retval = _vsnprintf_s(str,capacity, size, format, ap);
	if (retval < 0 || retval == size) {
		// Microsoft doesn't adhere to the C99 standard of vsnprintf,
		// which states that the return value must be the number of
		// bytes written if the output string had sufficient length.
		//
		// Obviously we cannot determine that value from Microsoft's
		// implementation, so we have no choice but to return size.
		str[size - 1] = '\0';
		return size;
	}
	return retval;
}
void QDECL Com_sprintf(char* dest,int size, const char* fmt, ...) {
	int		len;
	va_list		argptr;

	va_start(argptr, fmt);
	len = Q_vsnprintf(dest,size, size, fmt, argptr);
	va_end(argptr);
	if (len >= size) {
		Com_Printf("Com_sprintf: overflow of %i in %i\n", len, size);
#ifdef	_DEBUG
		//__asm {
		//	int 3;
		//}
#endif
	}
}

void Cmd_TokenizeString(const char* text_in) {
	const char* text;
	char* textOut;

	// clear previous args
	cmd_argc = 0;

	if (!text_in) {
		return;
	}

	text = text_in;
	textOut = cmd_tokenized;

	while (1) {
		if (cmd_argc == MAX_STRING_TOKENS) {
			return;			// this is usually something malicious
		}

		while (1) {
			// skip whitespace
			while (*text && *text <= ' ') {
				text++;
			}
			if (!*text) {
				return;			// all tokens parsed
			}

			// skip // comments
			if (text[0] == '/' && text[1] == '/') {
				return;			// all tokens parsed
			}

			// skip /* */ comments
			if (text[0] == '/' && text[1] == '*') {
				while (*text && (text[0] != '*' || text[1] != '/')) {
					text++;
				}
				if (!*text) {
					return;		// all tokens parsed
				}
				text += 2;
			}
			else {
				break;			// we are ready to parse a token
			}
		}

		// handle quoted strings
		if (*text == '"') {
			cmd_argv[cmd_argc] = textOut;
			cmd_argc++;
			text++;
			while (*text && *text != '"') {
				/*if (*text == '\\' && text[1] == '"') {
					text++; // Allow double quotes inside double quotes if they are escaped
				}*/ // This causes issues on some demos... sad.
				*textOut++ = *text++;
			}
			*textOut++ = 0;
			if (!*text) {
				return;		// all tokens parsed
			}
			text++;
			continue;
		}

		// regular token
		cmd_argv[cmd_argc] = textOut;
		cmd_argc++;

		// skip until whitespace, quote, or command
		while (*text > ' ') {
			if (text[0] == '"') {
				break;
			}

			if (text[0] == '/' && text[1] == '/') {
				break;
			}

			// skip /* */ comments
			if (text[0] == '/' && text[1] == '*') {
				break;
			}

			/*if (*text == '\\' && text[1] == '"') {
				text++; // Allow double quotes as string content if they are escaped (let's hope this does not interefere with other stuff)
			}*/ // This causes issues on some demos... sad.

			*textOut++ = *text++;
		}

		*textOut++ = 0;

		if (!*text) {
			return;		// all tokens parsed
		}
	}

}

#define VABUFFERSIZE 32000
char* QDECL va(const char* format, ...) {
	va_list		argptr;
	static char		string[10][VABUFFERSIZE];	// in case va is called by nested functions
	static int		index = 0;
	char* buf;

	buf = string[index % 10];
	index++;

	va_start(argptr, format);
	//vsprintf(buf, format, argptr);
	vsprintf_s(buf, VABUFFERSIZE, format, argptr);
	va_end(argptr);

	return buf;
}

void sanitizeFilename(const char* input, char* output,qboolean allowExtension) {

	char* lastDot = NULL;
	const char* inputStart = input;
	while (*input) {
		if (*input == '.' && input != inputStart) { // Even tho we allow extensions (dots), we don't allow the dot at the start of the filename.
			lastDot = output;
		}
		if ((*input == 32) // Don't allow ! exclamation mark. Linux doesn't like that.
			|| (*input >= 34 && *input < 42)
			|| (*input >= 43 && *input < 46)
			|| (*input >= 48 && *input < 58)
			|| (*input >= 59 && *input < 60)
			|| (*input == 61)
			|| (*input >= 64 && *input < 92)
			|| (*input >= 93 && *input < 124)
			|| (*input >= 125 && *input < 127)
			) {
			*output++ = *input;
		}
		else if (*input == '|') {
			*output++ = 'I';
		}
		else {
			*output++ = '-';
		}
		input++;
	}
	*output = 0;
	
	if (allowExtension && lastDot) {
		*lastDot = '.';
	}
}

/*void sanitizeFilename(std::string input, std::stringstream output) {

	while (*input) {
		if ((*input >= 32 && *input < 47)
			|| (*input >= 48 && *input < 60)
			|| (*input == 61)
			|| (*input >= 64 && *input < 92)
			|| (*input >= 93 && *input < 124)
			|| (*input >= 125 && *input < 127)
			) {
			*output++ = *input;
		}
		else if (*input == '|') {
			*output++ = 'I';
		}
		input++;
	}
	*output = 0;
}*/


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey(char* s, const char* key) {
	char* start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char* o;

	if (strlen(s) >= MAX_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_RemoveKey: oversize infostring");
	}

	if (strchr(key, '\\')) {
		return;
	}

	while (1) {
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\') {
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s) {
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp(key, pkey)) {
			memmove(start, s, strlen(s) + 1); // remove this part
			return;
		}

		if (!*s)
			return;
	}

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big(char* s, const char* key) {
	char* start;
	char	pkey[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	char* o;

	if (strlen(s) >= BIG_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_RemoveKey_Big: oversize infostring");
	}

	if (strchr(key, '\\')) {
		return;
	}

	while (1) {
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\') {
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s) {
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp(key, pkey)) {
			memmove(start, s, strlen(s) + 1); // remove this part
			return;
		}

		if (!*s)
			return;
	}

}


/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char* Info_ValueForKey(const char* s,int maxLength, const char* key) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
											// work without stomping on each other
	static	int	valueindex = 0;
	char* o;

	if (!s || !key) {
		return "";
	}

	//if (strlen(s) >= BIG_INFO_STRING) {
	if (strnlen_s(s,maxLength) >= BIG_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_ValueForKey: oversize infostring");
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1) {
		o = pkey;
		while (*s != '\\') {
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s) {
			*o++ = *s++;
		}
		*o = 0;

		if (!_stricmp(key, pkey))
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}


/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
qboolean Info_SetValueForKey(char* s,int capacity, const char* key, const char* value) {
	char	newi[MAX_INFO_STRING];

	if (strlen(s) >= MAX_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_SetValueForKey: oversize infostring");
	}

	if (strchr(key, '\\') || strchr(value, '\\')) {
		Com_Printf("Can't use keys or values with a \\\n");
		return qfalse;
	}

	if (strchr(key, ';') || strchr(value, ';')) {
		Com_Printf("Can't use keys or values with a semicolon\n");
		return qfalse;
	}

	if (strchr(key, '\"') || strchr(value, '\"')) {
		Com_Printf("Can't use keys or values with a \"\n");
		return qfalse;
	}

	Info_RemoveKey(s, key);

	if (!strlen(value))
		return qfalse;

	Com_sprintf(newi, sizeof(newi),  "\\%s\\%s", key, value);

	// q3infoboom exploit
	if (strlen(newi) + strlen(s) >= MAX_INFO_STRING) {
		Com_Printf("Info string length exceeded\n");
		return qfalse;
	}

	//strcat(newi, s);
	strcat_s(newi,sizeof(newi), s);
	//strcpy(s, newi);
	strcpy_s(s,capacity, newi);
	return qtrue;
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big(char* s, int capacity, const char* key, const char* value) {
	char	newi[BIG_INFO_STRING];

	if (strlen(s) >= BIG_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_SetValueForKey: oversize infostring");
	}

	if (strchr(key, '\\') || strchr(value, '\\')) {
		Com_Printf("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr(key, ';') || strchr(value, ';')) {
		Com_Printf("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr(key, '\"') || strchr(value, '\"')) {
		Com_Printf("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big(s, key);

	if (!strlen(value))
		return;

	Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

	// q3infoboom exploit
	if (strlen(newi) + strlen(s) >= BIG_INFO_STRING) {
		Com_Printf("BIG Info string length exceeded\n");
		return;
	}

	//strcat(s, newi);
	strcat_s(s, capacity, newi);
}




// MOD-weapon mapping array.
int weaponFromMOD[MOD_MAX] =
{
	WP_NONE,				//MOD_UNKNOWN,
	WP_STUN_BATON,			//MOD_STUN_BATON,
	WP_NONE,				//MOD_MELEE,
	WP_SABER,				//MOD_SABER,
	WP_BRYAR_PISTOL,		//MOD_BRYAR_PISTOL,
	WP_BRYAR_PISTOL,		//MOD_BRYAR_PISTOL_ALT,
	WP_BLASTER,				//MOD_BLASTER,
	WP_DISRUPTOR,			//MOD_DISRUPTOR,
	WP_DISRUPTOR,			//MOD_DISRUPTOR_SPLASH,
	WP_DISRUPTOR,			//MOD_DISRUPTOR_SNIPER,
	WP_BOWCASTER,			//MOD_BOWCASTER,
	WP_REPEATER,			//MOD_REPEATER,
	WP_REPEATER,			//MOD_REPEATER_ALT,
	WP_REPEATER,			//MOD_REPEATER_ALT_SPLASH,
	WP_DEMP2,				//MOD_DEMP2,
	WP_DEMP2,				//MOD_DEMP2_ALT,
	WP_FLECHETTE,			//MOD_FLECHETTE,
	WP_FLECHETTE,			//MOD_FLECHETTE_ALT_SPLASH,
	WP_ROCKET_LAUNCHER,		//MOD_ROCKET,
	WP_ROCKET_LAUNCHER,		//MOD_ROCKET_SPLASH,
	WP_ROCKET_LAUNCHER,		//MOD_ROCKET_HOMING,
	WP_ROCKET_LAUNCHER,		//MOD_ROCKET_HOMING_SPLASH,
	WP_THERMAL,				//MOD_THERMAL,
	WP_THERMAL,				//MOD_THERMAL_SPLASH,
	WP_TRIP_MINE,			//MOD_TRIP_MINE_SPLASH,
	WP_TRIP_MINE,			//MOD_TIMED_MINE_SPLASH,
	WP_DET_PACK,			//MOD_DET_PACK_SPLASH,
	WP_NONE,				//MOD_FORCE_DARK,
	WP_NONE,				//MOD_SENTRY,
	WP_NONE,				//MOD_WATER,
	WP_NONE,				//MOD_SLIME,
	WP_NONE,				//MOD_LAVA,
	WP_NONE,				//MOD_CRUSH,
	WP_NONE,				//MOD_TELEFRAG,
	WP_NONE,				//MOD_FALLING,
	WP_NONE,				//MOD_SUICIDE,
	WP_NONE,				//MOD_TARGET_LASER,
	WP_NONE,				//MOD_TRIGGER_HURT,
};

typedef enum {
	Q_BR,
	Q_R,
	Q_TR,
	Q_T,
	Q_TL,
	Q_L,
	Q_BL,
	Q_B,
	Q_NUM_QUADS
} saberQuadrant_t;

typedef enum {
	BLK_NO,
	BLK_TIGHT,		// Block only attacks and shots around the saber itself, a bbox of around 12x12x12
	BLK_WIDE		// Block all attacks in an area around the player in a rough arc of 180 degrees
} saberBlockType_t;

#define SETANIM_TORSO 1
#define SETANIM_LEGS  2
#define SETANIM_BOTH  SETANIM_TORSO|SETANIM_LEGS//3

#define SETANIM_FLAG_NORMAL		0//Only set if timer is 0
#define SETANIM_FLAG_OVERRIDE	1//Override previous
#define SETANIM_FLAG_HOLD		2//Set the new timer
#define SETANIM_FLAG_RESTART	4//Allow restarting the anim if playing the same one (weapon fires)
#define SETANIM_FLAG_HOLDLESS	8//Set the new timer

// Silly, but I'm replacing these macros so they are shorter!
#define AFLAG_IDLE	(SETANIM_FLAG_NORMAL)
#define AFLAG_ACTIVE (/*SETANIM_FLAG_OVERRIDE | */SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS)
#define AFLAG_WAIT (SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS)
#define AFLAG_FINISH (SETANIM_FLAG_HOLD)

saberMoveData_t	saberMoveData[LS_MOVE_MAX] = {//							NB:randomized
	// name			anim				startQ	endQ	setanimflag		blend,	blocking	chain_idle		chain_attack	trailLen
	{"None",		BOTH_STAND1,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_NO,		LS_NONE,		LS_NONE,		0	},	// LS_NONE		= 0,

	// General movements with saber
	{"Ready",		BOTH_STAND2,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_WIDE,	LS_READY,		LS_S_R2L,		0	},	// LS_READY,
	{"Draw",		BOTH_STAND1TO2,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY,		LS_S_R2L,		0	},	// LS_DRAW,
	{"Putaway",		BOTH_STAND2TO1,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY,		LS_S_R2L,		0	},	// LS_PUTAWAY,

	// Attacks
	//UL2LR
	{"TL2BR Att",	BOTH_A1_TL_BR,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TL2BR,		LS_R_TL2BR,		200	},	// LS_A_TL2BR
	//SLASH LEFT
	{"L2R Att",		BOTH_A1__L__R,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_L2R,		LS_R_L2R,		200 },	// LS_A_L2R
	//LL2UR
	{"BL2TR Att",	BOTH_A1_BL_TR,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_TIGHT,	LS_R_BL2TR,		LS_R_BL2TR,		200	},	// LS_A_BL2TR
	//LR2UL
	{"BR2TL Att",	BOTH_A1_BR_TL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_BR2TL,		LS_R_BR2TL,		200	},	// LS_A_BR2TL
	//SLASH RIGHT
	{"R2L Att",		BOTH_A1__R__L,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_R2L,		LS_R_R2L,		200 },// LS_A_R2L
	//UR2LL
	{"TR2BL Att",	BOTH_A1_TR_BL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TR2BL,		LS_R_TR2BL,		200	},	// LS_A_TR2BL
	//SLASH DOWN
	{"T2B Att",		BOTH_A1_T__B_,		Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_T2B,		LS_R_T2B,		200	},	// LS_A_T2B
	//special attacks
	{"Back Stab",	BOTH_A2_STABBACK1,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_A_BACKSTAB
	{"Back Att",	BOTH_ATTACK_BACK,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_A_BACK
	{"CR Back Att",	BOTH_CROUCHATTACKBACK1,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_A_BACK_CR
	{"Lunge Att",	BOTH_LUNGE2_B__T_,	Q_B,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_A_LUNGE
	{"Jump Att",	BOTH_FORCELEAP2_T__B_,Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_A_JUMP_T__B_
	{"Flip Stab",	BOTH_JUMPFLIPSTABDOWN,Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_T1_T___R,	200	},	// LS_A_FLIP_STAB
	{"Flip Slash",	BOTH_JUMPFLIPSLASHDOWN1,Q_L,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY,		LS_T1__R_T_,	200	},	// LS_A_FLIP_SLASH

	//starts
	{"TL2BR St",	BOTH_S1_S1_TL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TL2BR,		LS_A_TL2BR,		200	},	// LS_S_TL2BR
	{"L2R St",		BOTH_S1_S1__L,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_L2R,		LS_A_L2R,		200	},	// LS_S_L2R
	{"BL2TR St",	BOTH_S1_S1_BL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BL2TR,		LS_A_BL2TR,		200	},	// LS_S_BL2TR
	{"BR2TL St",	BOTH_S1_S1_BR,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BR2TL,		LS_A_BR2TL,		200	},	// LS_S_BR2TL
	{"R2L St",		BOTH_S1_S1__R,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_R2L,		LS_A_R2L,		200	},	// LS_S_R2L
	{"TR2BL St",	BOTH_S1_S1_TR,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TR2BL,		LS_A_TR2BL,		200	},	// LS_S_TR2BL
	{"T2B St",		BOTH_S1_S1_T_,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_T2B,		LS_A_T2B,		200	},	// LS_S_T2B

	//returns
	{"TL2BR Ret",	BOTH_R1_BR_S1,		Q_BR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_TL2BR
	{"L2R Ret",		BOTH_R1__R_S1,		Q_R,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_L2R
	{"BL2TR Ret",	BOTH_R1_TR_S1,		Q_TR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_BL2TR
	{"BR2TL Ret",	BOTH_R1_TL_S1,		Q_TL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_BR2TL
	{"R2L Ret",		BOTH_R1__L_S1,		Q_L,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_R2L
	{"TR2BL Ret",	BOTH_R1_BL_S1,		Q_BL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_TR2BL
	{"T2B Ret",		BOTH_R1_B__S1,		Q_B,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY,		LS_READY,		200	},	// LS_R_T2B

	//Transitions
	{"BR2R Trans",	BOTH_T1_BR__R,		Q_BR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast arc bottom right to right
	{"BR2TR Trans",	BOTH_T1_BR_TR,		Q_BR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	{"BR2T Trans",	BOTH_T1_BR_T_,		Q_BR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	{"BR2TL Trans",	BOTH_T1_BR_TL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast weak spin bottom right to top left
	{"BR2L Trans",	BOTH_T1_BR__L,		Q_BR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast weak spin bottom right to left
	{"BR2BL Trans",	BOTH_T1_BR_BL,		Q_BR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast weak spin bottom right to bottom left
	{"R2BR Trans",	BOTH_T1__R_BR,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	{"R2TR Trans",	BOTH_T1__R_TR,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast arc right to top right
	{"R2T Trans",	BOTH_T1__R_T_,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast ar right to top				(use: BOTH_T1_T___R)
	{"R2TL Trans",	BOTH_T1__R_TL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast arc right to top left
	{"R2L Trans",	BOTH_T1__R__L,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast weak spin right to left
	{"R2BL Trans",	BOTH_T1__R_BL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast weak spin right to bottom left
	{"TR2BR Trans",	BOTH_T1_TR_BR,		Q_TR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast arc top right to bottom right
	{"TR2R Trans",	BOTH_T1_TR__R,		Q_TR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	{"TR2T Trans",	BOTH_T1_TR_T_,		Q_TR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	{"TR2TL Trans",	BOTH_T1_TR_TL,		Q_TR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast arc top right to top left
	{"TR2L Trans",	BOTH_T1_TR__L,		Q_TR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast arc top right to left
	{"TR2BL Trans",	BOTH_T1_TR_BL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast weak spin top right to bottom left
	{"T2BR Trans",	BOTH_T1_T__BR,		Q_T,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast arc top to bottom right
	{"T2R Trans",	BOTH_T1_T___R,		Q_T,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast arc top to right
	{"T2TR Trans",	BOTH_T1_T__TR,		Q_T,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast arc top to top right
	{"T2TL Trans",	BOTH_T1_T__TL,		Q_T,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast arc top to top left
	{"T2L Trans",	BOTH_T1_T___L,		Q_T,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast arc top to left
	{"T2BL Trans",	BOTH_T1_T__BL,		Q_T,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast arc top to bottom left
	{"TL2BR Trans",	BOTH_T1_TL_BR,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast weak spin top left to bottom right
	{"TL2R Trans",	BOTH_T1_TL__R,		Q_TL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	{"TL2TR Trans",	BOTH_T1_TL_TR,		Q_TL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	{"TL2T Trans",	BOTH_T1_TL_T_,		Q_TL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	{"TL2L Trans",	BOTH_T1_TL__L,		Q_TL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	{"TL2BL Trans",	BOTH_T1_TL_BL,		Q_TL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast arc top left to bottom left
	{"L2BR Trans",	BOTH_T1__L_BR,		Q_L,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast weak spin left to bottom right
	{"L2R Trans",	BOTH_T1__L__R,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast weak spin left to right
	{"L2TR Trans",	BOTH_T1__L_TR,		Q_L,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	{"L2T Trans",	BOTH_T1__L_T_,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast arc left to top				(use: BOTH_T1_T___L)
	{"L2TL Trans",	BOTH_T1__L_TL,		Q_L,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast arc left to top left
	{"L2BL Trans",	BOTH_T1__L_BL,		Q_L,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	{"BL2BR Trans",	BOTH_T1_BL_BR,		Q_BL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	//# Fast weak spin bottom left to bottom right
	{"BL2R Trans",	BOTH_T1_BL__R,		Q_BL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_A_R2L,		150	},	//# Fast weak spin bottom left to right
	{"BL2TR Trans",	BOTH_T1_BL_TR,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	//# Fast weak spin bottom left to top right
	{"BL2T Trans",	BOTH_T1_BL_T_,		Q_BL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_A_T2B,		150	},	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	{"BL2TL Trans",	BOTH_T1_BL_TL,		Q_BL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	{"BL2L Trans",	BOTH_T1_BL__L,		Q_BL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_A_L2R,		150	},	//# Fast arc bottom left to left

	//Bounces
	{"Bounce BR",	BOTH_B1_BR___,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_T1_BR_TR,	150	},
	{"Bounce R",	BOTH_B1__R___,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_T1__R__L,	150	},
	{"Bounce TR",	BOTH_B1_TR___,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_T1_TR_TL,	150	},
	{"Bounce T",	BOTH_B1_T____,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_T1_T__BL,	150	},
	{"Bounce TL",	BOTH_B1_TL___,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_T1_TL_TR,	150	},
	{"Bounce L",	BOTH_B1__L___,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_T1__L__R,	150	},
	{"Bounce BL",	BOTH_B1_BL___,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_T1_BL_TR,	150	},

	//Deflected attacks (like bounces, but slide off enemy saber, not straight back)
	{"Deflect BR",	BOTH_D1_BR___,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR,		LS_T1_BR_TR,	150	},
	{"Deflect R",	BOTH_D1__R___,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R,		LS_T1__R__L,	150	},
	{"Deflect TR",	BOTH_D1_TR___,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_T1_TR_TL,	150	},
	{"Deflect T",	BOTH_B1_T____,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_T1_T__BL,	150	},
	{"Deflect TL",	BOTH_D1_TL___,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL,		LS_T1_TL_TR,	150	},
	{"Deflect L",	BOTH_D1__L___,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L,		LS_T1__L__R,	150	},
	{"Deflect BL",	BOTH_D1_BL___,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL,		LS_T1_BL_TR,	150	},
	{"Deflect B",	BOTH_D1_B____,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR,		LS_T1_T__BL,	150	},

	//Reflected attacks
	{"Reflected BR",BOTH_V1_BR_S1,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_BR
	{"Reflected R",	BOTH_V1__R_S1,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1__R
	{"Reflected TR",BOTH_V1_TR_S1,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_TR
	{"Reflected T",	BOTH_V1_T__S1,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_T_
	{"Reflected TL",BOTH_V1_TL_S1,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_TL
	{"Reflected L",	BOTH_V1__L_S1,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1__L
	{"Reflected BL",BOTH_V1_BL_S1,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_BL
	{"Reflected B",	BOTH_V1_B__S1,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY,		LS_READY,	150	},//	LS_V1_B_

	// Broken parries
	{"BParry Top",	BOTH_H1_S1_T_,		Q_T,	Q_B,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_UP,
	{"BParry UR",	BOTH_H1_S1_TR,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_UR,
	{"BParry UL",	BOTH_H1_S1_TL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_UL,
	{"BParry LR",	BOTH_H1_S1_BL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_LR,
	{"BParry Bot",	BOTH_H1_S1_B_,		Q_B,	Q_T,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_LL
	{"BParry LL",	BOTH_H1_S1_BR,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY,		LS_READY,		150	},	// LS_PARRY_LL

	// Knockaways
	{"Knock Top",	BOTH_K1_S1_T_,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_T1_T__BR,		150	},	// LS_PARRY_UP,
	{"Knock UR",	BOTH_K1_S1_TR,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_T1_TR__R,		150	},	// LS_PARRY_UR,
	{"Knock UL",	BOTH_K1_S1_TL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL,		LS_T1_TL__L,		150	},	// LS_PARRY_UL,
	{"Knock LR",	BOTH_K1_S1_BL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR,		LS_T1_BL_TL,		150	},	// LS_PARRY_LR,
	{"Knock LL",	BOTH_K1_S1_BR,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL,		LS_T1_BR_TR,		150	},	// LS_PARRY_LL

	// Parry
	{"Parry Top",	BOTH_P1_S1_T_,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_A_T2B,		150	},	// LS_PARRY_UP,
	{"Parry UR",	BOTH_P1_S1_TR,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_A_TR2BL,		150	},	// LS_PARRY_UR,
	{"Parry UL",	BOTH_P1_S1_TL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL,		LS_A_TL2BR,		150	},	// LS_PARRY_UL,
	{"Parry LR",	BOTH_P1_S1_BL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR,		LS_A_BR2TL,		150	},	// LS_PARRY_LR,
	{"Parry LL",	BOTH_P1_S1_BR,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL,		LS_A_BL2TR,		150	},	// LS_PARRY_LL

	// Reflecting a missile
	{"Reflect Top",	BOTH_P1_S1_T_,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_A_T2B,		300	},	// LS_PARRY_UP,
	{"Reflect UR",	BOTH_P1_S1_TL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL,		LS_A_TL2BR,		300	},	// LS_PARRY_UR,
	{"Reflect UL",	BOTH_P1_S1_TR,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR,		LS_A_TR2BL,		300	},	// LS_PARRY_UL,
	{"Reflect LR",	BOTH_P1_S1_BR,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL,		LS_A_BL2TR,		300	},	// LS_PARRY_LR
	{"Reflect LL",	BOTH_P1_S1_BL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR,		LS_A_BR2TL,		300	},	// LS_PARRY_LL,
};

std::map <int, std::string>  saberMoveNames
{
	// Invalid, or saber not armed
	  std::make_pair(LS_NONE, "_WEIRD"),

	  // General movements with saber
	  std::make_pair(LS_READY, "_IDLE"),
	  std::make_pair(LS_DRAW, "_DRAW"),
	  std::make_pair(LS_PUTAWAY, "_PUTAWAY"),

	  // Attacks
	  std::make_pair(LS_A_TL2BR, ""),//4
	  std::make_pair(LS_A_L2R, ""),
	  std::make_pair(LS_A_BL2TR, ""),
	  std::make_pair(LS_A_BR2TL, ""),
	  std::make_pair(LS_A_R2L, ""),
	  std::make_pair(LS_A_TR2BL, ""),
	  std::make_pair(LS_A_T2B, ""),
	  std::make_pair(LS_A_BACKSTAB, "_BLUBS"),
	  std::make_pair(LS_A_BACK, "_BS"),
	  std::make_pair(LS_A_BACK_CR, "_DBS"),
	  std::make_pair(LS_A_LUNGE, "_UPCUT"),
	  std::make_pair(LS_A_JUMP_T__B_, "_DFA"),
	  std::make_pair(LS_A_FLIP_STAB, "_YDFA"),
	  std::make_pair(LS_A_FLIP_SLASH, "_YDFA"),

	  //starts
	  std::make_pair(LS_S_TL2BR, ""),//26
	  std::make_pair(LS_S_L2R, ""),
	  std::make_pair(LS_S_BL2TR, ""),//# Start of attack chaining to SLASH LR2UL
	  std::make_pair(LS_S_BR2TL, ""),//# Start of attack chaining to SLASH LR2UL
	  std::make_pair(LS_S_R2L, ""),
	  std::make_pair(LS_S_TR2BL, ""),
	  std::make_pair(LS_S_T2B, ""),

	  //returns
	  std::make_pair(LS_R_TL2BR, ""),//33
	  std::make_pair(LS_R_L2R, ""),
	  std::make_pair(LS_R_BL2TR, ""),
	  std::make_pair(LS_R_BR2TL, ""),
	  std::make_pair(LS_R_R2L, ""),
	  std::make_pair(LS_R_TR2BL, ""),
	  std::make_pair(LS_R_T2B, ""),

	  //transitions
	  std::make_pair(LS_T1_BR__R, ""),//40
	  std::make_pair(LS_T1_BR_TR, ""),
	  std::make_pair(LS_T1_BR_T_, ""),
	  std::make_pair(LS_T1_BR_TL, ""),
	  std::make_pair(LS_T1_BR__L, ""),
	  std::make_pair(LS_T1_BR_BL, ""),
	  std::make_pair(LS_T1__R_BR, ""),//46
	  std::make_pair(LS_T1__R_TR, ""),
	  std::make_pair(LS_T1__R_T_, ""),
	  std::make_pair(LS_T1__R_TL, ""),
	  std::make_pair(LS_T1__R__L, ""),
	  std::make_pair(LS_T1__R_BL, ""),
	  std::make_pair(LS_T1_TR_BR, ""),//52
	  std::make_pair(LS_T1_TR__R, ""),
	  std::make_pair(LS_T1_TR_T_, ""),
	  std::make_pair(LS_T1_TR_TL, ""),
	  std::make_pair(LS_T1_TR__L, ""),
	  std::make_pair(LS_T1_TR_BL, ""),
	  std::make_pair(LS_T1_T__BR, ""),//58
	  std::make_pair(LS_T1_T___R, ""),
	  std::make_pair(LS_T1_T__TR, ""),
	  std::make_pair(LS_T1_T__TL, ""),
	  std::make_pair(LS_T1_T___L, ""),
	  std::make_pair(LS_T1_T__BL, ""),
	  std::make_pair(LS_T1_TL_BR, ""),//64
	  std::make_pair(LS_T1_TL__R, ""),
	  std::make_pair(LS_T1_TL_TR, ""),
	  std::make_pair(LS_T1_TL_T_, ""),
	  std::make_pair(LS_T1_TL__L, ""),
	  std::make_pair(LS_T1_TL_BL, ""),
	  std::make_pair(LS_T1__L_BR, ""),//70
	  std::make_pair(LS_T1__L__R, ""),
	  std::make_pair(LS_T1__L_TR, ""),
	  std::make_pair(LS_T1__L_T_, ""),
	  std::make_pair(LS_T1__L_TL, ""),
	  std::make_pair(LS_T1__L_BL, ""),
	  std::make_pair(LS_T1_BL_BR, ""),//76
	  std::make_pair(LS_T1_BL__R, ""),
	  std::make_pair(LS_T1_BL_TR, ""),
	  std::make_pair(LS_T1_BL_T_, ""),
	  std::make_pair(LS_T1_BL_TL, ""),
	  std::make_pair(LS_T1_BL__L, ""),

	  //Bounces
	  std::make_pair(LS_B1_BR, "_BOUNCE"),
	  std::make_pair(LS_B1__R, "_BOUNCE"),
	  std::make_pair(LS_B1_TR, "_BOUNCE"),
	  std::make_pair(LS_B1_T_, "_BOUNCE"),
	  std::make_pair(LS_B1_TL, "_BOUNCE"),
	  std::make_pair(LS_B1__L, "_BOUNCE"),
	  std::make_pair(LS_B1_BL, "_BOUNCE"),

	  //Deflected attacks
	  std::make_pair(LS_D1_BR, "_DEFLECT"),
	  std::make_pair(LS_D1__R, "_DEFLECT"),
	  std::make_pair(LS_D1_TR, "_DEFLECT"),
	  std::make_pair(LS_D1_T_, "_DEFLECT"),
	  std::make_pair(LS_D1_TL, "_DEFLECT"),
	  std::make_pair(LS_D1__L, "_DEFLECT"),
	  std::make_pair(LS_D1_BL, "_DEFLECT"),
	  std::make_pair(LS_D1_B_, "_DEFLECT"),

	  //Reflected attacks
	  std::make_pair(LS_V1_BR, "_REFLECT"),
	  std::make_pair(LS_V1__R, "_REFLECT"),
	  std::make_pair(LS_V1_TR, "_REFLECT"),
	  std::make_pair(LS_V1_T_, "_REFLECT"),
	  std::make_pair(LS_V1_TL, "_REFLECT"),
	  std::make_pair(LS_V1__L, "_REFLECT"),
	  std::make_pair(LS_V1_BL, "_REFLECT"),
	  std::make_pair(LS_V1_B_, "_REFLECT"),

	  // Broken parries
	  std::make_pair(LS_H1_T_, "_BPARRY"),//
	  std::make_pair(LS_H1_TR, "_BPARRY"),
	  std::make_pair(LS_H1_TL, "_BPARRY"),
	  std::make_pair(LS_H1_BR, "_BPARRY"),
	  std::make_pair(LS_H1_B_, "_BPARRY"),
	  std::make_pair(LS_H1_BL, "_BPARRY"),

	  // Knockaways
	  std::make_pair(LS_K1_T_, "_KNOCKAWAY"),//
	  std::make_pair(LS_K1_TR, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_TL, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_BR, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_BL, "_KNOCKAWAY"),

	  // Parries
	  std::make_pair(LS_PARRY_UP, "_PARRY"),//
	  std::make_pair(LS_PARRY_UR, "_PARRY"),
	  std::make_pair(LS_PARRY_UL, "_PARRY"),
	  std::make_pair(LS_PARRY_LR, "_PARRY"),
	  std::make_pair(LS_PARRY_LL, "_PARRY"),

	  // Projectile Reflections
	  std::make_pair(LS_REFLECT_UP, "_PREFLECT"),//
	  std::make_pair(LS_REFLECT_UR, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_UL, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_LR, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_LL, "_PREFLECT"),

	  //std::make_pair(LS_MOVE_MAX, "")//
};

std::map <int, std::string>  saberStyleNames
{
	std::make_pair(FORCE_LEVEL_0, "_UNKST"),
	std::make_pair(FORCE_LEVEL_1, "_BLU"),
	std::make_pair(FORCE_LEVEL_2, "_YEL"),
	std::make_pair(FORCE_LEVEL_3, "_RED"),
	std::make_pair(NUM_FORCE_POWER_LEVELS, "")
};


void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, qboolean snap,qboolean writeCommandTime, qboolean clientSideStyleEventConversion) {
	int		i;

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = ET_INVISIBLE;
	}
	else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) {
		s->eType = ET_INVISIBLE;
	}
	else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;

	if (writeCommandTime) {
		s->pos.trType = TR_LINEAR_STOP;
		s->pos.trTime = ps->commandTime;
		// set maximum extra polation time
		s->pos.trDuration = 50; // 1000 / sv_fps (default = 20) (dunno why its needed really but do this to be consistent with original implementation to avoid jumps in demo when using cg_smoothclients)
	}
	else {
		s->pos.trType = TR_INTERPOLATE;
	}

	VectorCopy(ps->origin, s->pos.trBase);

	if (snap) {
		SnapVector(s->pos.trBase);
	}
	// set the trDelta for flag direction
	VectorCopy(ps->velocity, s->pos.trDelta);

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);
	if (snap) {
		SnapVector(s->apos.trBase);
	}

	s->trickedentindex = ps->fd.forceMindtrickTargetIndex;
	s->trickedentindex2 = ps->fd.forceMindtrickTargetIndex2;
	s->trickedentindex3 = ps->fd.forceMindtrickTargetIndex3;
	s->trickedentindex4 = ps->fd.forceMindtrickTargetIndex4;

	s->forceFrame = ps->saberLockFrame;

	s->emplacedOwner = ps->electrifyTime;

	s->speed = ps->speed;

	s->genericenemyindex = ps->genericEnemyIndex;

	s->activeForcePass = ps->activeForcePass;

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;

	s->saberInFlight = ps->saberInFlight;
	s->saberEntityNum = ps->saberEntityNum;
	s->saberMove = ps->saberMove;
	s->forcePowersActive = ps->fd.forcePowersActive;

	if (ps->duelInProgress)
	{
		s->bolt1 = 1;
	}
	else
	{
		s->bolt1 = 0;
	}

	if (ps->dualBlade)
	{
		s->bolt2 = 1;
	}
	else
	{
		s->bolt2 = 0;
	}

	s->otherEntityNum2 = ps->emplacedIndex;

	s->shouldtarget = ps->saberHolstered; //reuse bool in entitystate for players differently
	s->teamowner = ps->usingATST;

	if (ps->genericEnemyIndex != -1)
	{
		s->eFlags |= EF_SEEKERDRONE;
	}

	if (ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	}
	else {
		s->eFlags &= ~EF_DEAD;
	}

	if (ps->externalEvent) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	}
	else if (clientSideStyleEventConversion) {
		int		entityEventSequenceReal, seq;

		entityEventSequenceReal = ps->entityEventSequence - 1;
		if (entityEventSequenceReal < ps->eventSequence - MAX_PS_EVENTS) {
			entityEventSequenceReal = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = entityEventSequenceReal & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq] | ((entityEventSequenceReal & 3) << 8);
		s->eventParm = ps->eventParms[seq];
	}
	else if (ps->entityEventSequence < ps->eventSequence) {
		int		seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq] | ((ps->entityEventSequence & 3) << 8);
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequence++;
	}


	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (ps->powerups[i]) {
			s->powerups |= 1 << i;
		}
	}

	s->loopSound = ps->loopSound;
	s->generic1 = ps->generic1;

	//NOT INCLUDED IN ENTITYSTATETOPLAYERSTATE:
	s->modelindex2 = ps->weaponstate;
	s->constantLight = ps->weaponChargeTime;

	VectorCopy(ps->lastHitLoc, s->origin2);

	s->isJediMaster = ps->isJediMaster;

	s->time2 = ps->holocronBits;

	s->fireflag = ps->fd.saberAnimLevel;
}

// baseState is my modification. It will use values from the base Snap that an entity just doesn't have. PERS_SPAWN_COUNT and precise health/armor
void CG_EntityStateToPlayerState(entityState_t* s, playerState_t* ps, demoType_t demoType, qboolean allValues, playerState_t* baseState, qboolean enhanceOnly) {
	int		i;

	if (!enhanceOnly) {
		ps->clientNum = s->number;
	}

	if (s->pos.trType == TR_LINEAR_STOP) {
		ps->commandTime = s->pos.trTime;
	}

	if(!enhanceOnly || VectorDistance(s->pos.trBase, ps->origin) > 2.0f){ // If we are enhancing only: Entity values here are snapped (rounded) so they will reduce precision UNLESS we are moving sufficiently fast to justify using them anyway
		VectorCopy(s->pos.trBase, ps->origin);
	}

	VectorCopy(s->pos.trDelta, ps->velocity); // Velocity is not specially rounded for entities. (from what I can tell)

	if (!enhanceOnly || VectorDistance(s->apos.trBase, ps->viewangles) > 2.0f) {  // If we are enhancing only: Entity values here are snapped (rounded) so they will reduce precision UNLESS we are moving sufficiently fast to justify using them anyway
		VectorCopy(s->apos.trBase, ps->viewangles);
	}

	ps->fd.forceMindtrickTargetIndex = s->trickedentindex;
	ps->fd.forceMindtrickTargetIndex2 = s->trickedentindex2;
	ps->fd.forceMindtrickTargetIndex3 = s->trickedentindex3;
	ps->fd.forceMindtrickTargetIndex4 = s->trickedentindex4;

	ps->saberLockFrame = s->forceFrame;

	ps->electrifyTime = s->emplacedOwner;

	ps->speed = s->speed;

	ps->genericEnemyIndex = s->genericenemyindex;

	ps->activeForcePass = s->activeForcePass;

	ps->movementDir = s->angles2[YAW];
	ps->legsAnim = s->legsAnim;
	ps->torsoAnim = s->torsoAnim;
	ps->clientNum = s->clientNum;

	ps->eFlags = s->eFlags;

	ps->saberInFlight = s->saberInFlight;
	ps->saberEntityNum = s->saberEntityNum;
	ps->saberMove = s->saberMove;
	ps->fd.forcePowersActive = s->forcePowersActive;

	if (s->bolt1)
	{
		ps->duelInProgress = qtrue;
	}
	else
	{
		ps->duelInProgress = qfalse;
	}

	if (s->bolt2)
	{
		ps->dualBlade = qtrue;
	}
	else
	{
		ps->dualBlade = qfalse;
	}

	ps->emplacedIndex = s->otherEntityNum2;

	ps->saberHolstered = s->shouldtarget; //reuse bool in entitystate for players differently
	ps->usingATST = (qboolean)s->teamowner;

	/*
	if (ps->genericEnemyIndex != -1)
	{
		s->eFlags |= EF_SEEKERDRONE;
	}
	*/
	if(!enhanceOnly){
		ps->genericEnemyIndex = -1; //no real option for this

		//The client has no knowledge of health levels (except for the client entity)
		if (s->eFlags & EF_DEAD)
		{
			ps->stats[STAT_HEALTH] = 0;
		}
		else
		{
			ps->stats[STAT_HEALTH] = 100;
		}

		// TODO fix this. For example we want the EV_SABER_ATTACK event to work.
		// Dirty quick try: Just set it as "externalEvent"
		ps->externalEvent = s->event;
		ps->externalEventParm = s->eventParm;
		/*
		if ( ps->externalEvent ) {
			s->event = ps->externalEvent;
			s->eventParm = ps->externalEventParm;
		} else if ( ps->entityEventSequence < ps->eventSequence ) {
			int		seq;

			if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
				ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
			}
			seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
			s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
			s->eventParm = ps->eventParms[ seq ];
			ps->entityEventSequence++;
		}
		*/
		//Eh.

	}

	ps->weapon = s->weapon;
	ps->groundEntityNum = s->groundEntityNum;

	if (!enhanceOnly) { // Quick explainer: If we are enhancing a playerState, we don't want to ever reduce the precision of any of its values, so we only copy over stuff that is actually precisely known from the entityState.

		for (i = 0; i < MAX_POWERUPS; i++) {
			if (s->powerups & (1 << i))
			{
				ps->powerups[i] = 30;
			}
			else
			{
				ps->powerups[i] = 0;
			}
		}
	}

	ps->loopSound = s->loopSound;
	ps->generic1 = s->generic1;

	if (allValues) { // Wasn't included in original ToPlayerstate for some reason
		ps->weaponstate = s->modelindex2;
		ps->weaponChargeTime = s->constantLight;

		VectorCopy(s->origin2,ps->lastHitLoc);

		ps->isJediMaster = s->isJediMaster;

		ps->holocronBits = s->time2;

		if (!enhanceOnly) { // This stuff is kinda guesswork so... eh. Not use that if we are enhancing
			if (s->weapon < WP_NUM_WEAPONS) {

				ps->stats[STAT_WEAPONS] |= 1 << s->weapon; // To make weapon select wheel look correct
			}

			if (s->weapon == WP_SABER && s->fireflag == 0) {
				// Server disabled sending fireflag saber style. We must deduce from animations (ugh)
				byte probability;
				ps->fd.saberDrawAnimLevel = ps->fd.saberAnimLevel = getLikelyStanceFromTorsoAnim(s->torsoAnim, demoType, &probability);
			}
			else {
				ps->fd.saberDrawAnimLevel = ps->fd.saberAnimLevel = s->fireflag;
			}

			// This is my own stuff: (restoring viewheight)
			if (s->eFlags & EF_DEAD) {
				ps->viewheight = DEAD_VIEWHEIGHT;
			}
			else if (
				(s->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE || (s->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_15
				|| (s->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALKBACK || (s->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALK
			
				) {
				ps->viewheight = CROUCH_VIEWHEIGHT;
			}
			else {
				ps->viewheight = DEFAULT_VIEWHEIGHT;
			}
		}

	}

	if (baseState) {
		//ps->persistant[PERS_SPAWN_COUNT] = baseState->persistant[PERS_SPAWN_COUNT];
		Com_Memcpy(ps->persistant, baseState->persistant, sizeof(ps->persistant));
		if (ps->stats[STAT_HEALTH] > 0 && baseState->stats[STAT_HEALTH] > 0) { // don't copy positive health into a dead entity
			//Com_Memcpy(ps->stats,baseState->stats,sizeof(ps->stats));
			ps->fd.forcePower = baseState->fd.forcePower;
			ps->stats[STAT_HEALTH] = baseState->stats[STAT_HEALTH];
			ps->stats[STAT_ARMOR] = baseState->stats[STAT_ARMOR];
			ps->stats[STAT_MAX_HEALTH] = baseState->stats[STAT_MAX_HEALTH];
		}
	}
}



/*
===============
LerpAngle

===============
*/
float LerpAngle(float from, float to, float frac) {
	float	a;

	if (to - from > 180) {
		to -= 360;
	}
	if (to - from < -180) {
		to += 360;
	}
	a = from + frac * (to - from);

	return a;
}









std::vector<std::string> splitString(std::string input, std::string separator, bool trim, bool allowEmpty) {
	std::vector<std::string> retVal;
	int position = 0;
	size_t foundPos = 0;
	size_t lastFoundPos = -1;

	while (std::string::npos != (foundPos = input.find(separator, lastFoundPos + 1))) {
		std::string newString = input.substr(lastFoundPos + 1, foundPos - lastFoundPos - 1);
		if (trim) {
			size_t tmpLocation = newString.find_last_not_of(" ");
			if (tmpLocation != std::string::npos && tmpLocation < (newString.size() - 1)) {
				newString.erase(tmpLocation + 1);
			}
			newString.erase(0, std::max((int)newString.find_first_not_of(" "), 0));
		}
		if (allowEmpty || newString.size() > 0)
			retVal.push_back(newString);
		lastFoundPos = foundPos;
	}
	if (lastFoundPos != input.size() - 1) {
		std::string newString = input.substr(lastFoundPos + 1, input.size() - lastFoundPos - 1);
		if (trim) {
			size_t tmpLocation = newString.find_last_not_of(" ");
			if (tmpLocation != std::string::npos && tmpLocation < (newString.size() - 1)) {
				newString.erase(tmpLocation + 1);
			}
			newString.erase(0, std::max((int)newString.find_first_not_of(" "), 0));
		}
		if (allowEmpty || newString.size() > 0)
			retVal.push_back(newString);
	}
	return retVal;
}


char tolowerSignSafe(char in) {
	return (char)tolower((unsigned char)in);
}




/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory(const trajectory_t* tr, int atTime, vec3_t result) {
	float		deltaTime;
	float		phase;

	switch (tr->trType) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorCopy(tr->trBase, result);
		break;
	case TR_LINEAR:
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		break;
	case TR_SINE:
		deltaTime = (atTime - tr->trTime) / (float)tr->trDuration;
		phase = sin(deltaTime * M_PI * 2);
		VectorMA(tr->trBase, phase, tr->trDelta, result);
		break;
	case TR_LINEAR_STOP:
		if (atTime > tr->trTime + tr->trDuration) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		if (deltaTime < 0) {
			deltaTime = 0;
		}
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		break;
	case TR_GRAVITY:
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	default:
#ifdef QAGAME
		Com_Error(ERR_DROP, "BG_EvaluateTrajectory: [GAME SIDE] unknown trType: %i", tr->trType);
#else
		Com_Error(ERR_DROP, "BG_EvaluateTrajectory: [CLIENTGAME SIDE] unknown trType: %i", tr->trType);
#endif
		break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta(const trajectory_t* tr, int atTime, vec3_t result) {
	float	deltaTime;
	float	phase;

	switch (tr->trType) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear(result);
		break;
	case TR_LINEAR:
		VectorCopy(tr->trDelta, result);
		break;
	case TR_SINE:
		deltaTime = (atTime - tr->trTime) / (float)tr->trDuration;
		phase = cos(deltaTime * M_PI * 2);	// derivative of sin = cos
		phase *= 0.5;
		VectorScale(tr->trDelta, phase, result);
		break;
	case TR_LINEAR_STOP:
		if (atTime > tr->trTime + tr->trDuration) {
			VectorClear(result);
			return;
		}
		VectorCopy(tr->trDelta, result);
		break;
	case TR_GRAVITY:
		deltaTime = (atTime - tr->trTime) * 0.001;	// milliseconds to seconds
		VectorCopy(tr->trDelta, result);
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error(ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime);
		break;
	}
}



int getLikelyStanceFromTorsoAnim(int torsoAnim,demoType_t demoType, byte* probability) {

	torsoAnim &= ~ANIM_TOGGLEBIT;

	constexpr int countDm15AnimMappings = sizeof(animStanceMappings15) / sizeof(animStanceMapping_t);
	constexpr int countDm16AnimMappings = sizeof(animStanceMappings16) / sizeof(animStanceMapping_t);

	if (probability) *probability = 0;

	animStanceMapping_t* mappingHere;
	if (demoType == DM_15) {
		if (torsoAnim >= countDm15AnimMappings) {
			std::cout << "torsoAnim "<< torsoAnim << " higher than max for DM_15 ("<< countDm15AnimMappings <<")\n";
			return 0;
		}
		else {
			mappingHere = &animStanceMappings15[torsoAnim];
		}
	}
	else {
		if (torsoAnim >= countDm16AnimMappings) {
			std::cout << "torsoAnim " << torsoAnim << " higher than max for DM_15 (" << countDm15AnimMappings << ")\n";
			return 0;
		}
		else {
			mappingHere = &animStanceMappings16[torsoAnim];
		}
	}

	if (!mappingHere->totalCount) {
		return 0; // No idea then.
	}

	byte highestProbability = 0;
	int highestProbabilityStance = 0;
	for (int i = 0; i < POSSIBLE_SABER_STANCE_VALUES; i++) {
		if (mappingHere->probability[i] > highestProbability) {
			highestProbability = mappingHere->probability[i];
			highestProbabilityStance = i;
		}
	}

	if (probability) {
		// Ok I know this looks strange, but the thought is simple:
		// Even if 100% of our samples indicate a particular stance,
		// if our sample size for that torsoAnim is under 100, the probability is not that great.
		// So we take the sqrt of the sample count times 10. Meaning 100 samples = 100% probability still.
		// But under that, it declines. Then we use whichever probability value ends up smaller.
		// For sample sizes over 100, effectively the actual probability is used.

		byte actualProbability = (byte)std::min((int)highestProbability,(int)(10.0*sqrt(mappingHere->totalCount)));
		*probability = actualProbability;
	}
	return highestProbabilityStance;

}



float VectorDistance(vec3_t v1, vec3_t v2)
{
	vec3_t dir;

	VectorSubtract(v2, v1, dir);
	return VectorLength(dir);
} 



#define Q_COLOR_ESCAPE	'^'
// you MUST have the last bit on here about colour strings being less than 7 or taiwanese strings register as colour!!!!
//#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE && *((p)+1) <= '7' && *((p)+1) >= '0' )
// Correct version of the above for Q_StripColor
#define Q_IsColorStringExt(p)	((p) && *(p) == Q_COLOR_ESCAPE && *((p)+1) && isdigit(*((p)+1))) // ^[0-9]

// from eternaljk2mv:
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) <= '7' && *((p)+1) >= '0' )
#define Q_IsColorString_1_02(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE ) // 1.02 ColorStrings
#define Q_IsColorString_Extended(p) Q_IsColorString_1_02(p)

#define Q_IsColorStringNT(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE && *((p)+1) <= 0x7F && *((p)+1) >= 0x00 )
#define ColorIndexNT(c)			( (c) & 127 )

#define Q_IsColorStringHex(p) (Q_IsColorStringHexY((p))) || (Q_IsColorStringHexy((p))) || (Q_IsColorStringHexX((p))) || (Q_IsColorStringHexx((p)) )
#define Q_IsColorStringHexY(p) ((p)+8) && (p) && *(p)=='Y' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4)) && Q_IsHex((p+5)) && Q_IsHex((p+6)) && Q_IsHex((p+7)) && Q_IsHex((p+8))
#define Q_IsColorStringHexy(p) ((p)+4) && (p) && *(p)=='y' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4))
#define Q_IsColorStringHexX(p) ((p)+6) && (p) && *(p)=='X' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3)) && Q_IsHex((p+4)) && Q_IsHex((p+5)) && Q_IsHex((p+6))
#define Q_IsColorStringHexx(p) ((p)+3) && (p) && *(p)=='x' && Q_IsHex((p+1)) && Q_IsHex((p+2)) && Q_IsHex((p+3))

#define Q_IsHex(p) ((p) && ((*(p) >= '0' && *(p) <= '9') || (*(p) >= 'a' && *(p) <= 'f') || (*(p) >= 'A' && *(p) <= 'F')))


qboolean Q_parseColorHex(const char* p, float* color, int* skipCount) {
	char c = *p++;
	int i;
	int val;

	qboolean doWrite = qtrue;
	if (!color || !(color + 3)) {
		doWrite = qfalse;
	}

	*skipCount = 0; // We update it only if successful. If not successful, we want the string to be parsed normally.

	int countToParse = 8;
	qboolean halfPrecision = qfalse;
	if (c == 'Y') {
		countToParse = 8;
	}
	else if (c == 'y') {
		countToParse = 4;
		halfPrecision = qtrue;
	}
	else if (c == 'X') {
		countToParse = 6;
		if (doWrite) color[3] = 1.0f; // Z and z don't contain alpha.
	}
	else if (c == 'x') {
		countToParse = 3;
		if (doWrite) color[3] = 1.0f;
		halfPrecision = qtrue;
	}

	int presumableSkipCount = countToParse + 1; // skip count will be set to this if successful.

	for (i = 0; i < countToParse; i++) {
		int readHex;
		c = p[i];
		if (c >= '0' && c <= '9') {
			readHex = c - '0';
		}
		else if (c >= 'a' && c <= 'f') {
			readHex = 0xa + c - 'a';
		}
		else if (c >= 'A' && c <= 'F') {
			readHex = 0xa + c - 'A';
		}
		else {
			if (color) {
				color[0] = color[1] = color[2] = color[3] = 1.0f;
			}
			return qfalse;
		}
		if (doWrite) {

			if (halfPrecision) { // Single digit per value.
				val = readHex;
				color[i] = val * (1 / 15.0f);
			}
			else {
				if (i & 1) {
					val |= readHex;
					color[i >> 1] = val * (1 / 255.0f);
				}
				else {
					val = readHex << 4;
				}
			}
		}

	}

	*skipCount = presumableSkipCount;
	return qtrue;

}

std::string Q_StripColorAll(std::string string) {
	const char* sourceCString = string.c_str();
	int stringLen = strlen(sourceCString);
	char* cString = new char[stringLen + 1];
	strcpy_s(cString, stringLen + 1, sourceCString);
	Q_StripColorAll(cString);
	std::string colorStripped = cString;
	return colorStripped;
}

void Q_StripColorAll(char* text) {
	char* read;
	char* write;

	read = write = text;
	while (*read) {
		if (Q_IsColorStringHex(read + 1)) {
			int skipCount = 0;
			Q_parseColorHex(read + 1, 0, &skipCount);
			read += 1 + skipCount;
		}
		else if (Q_IsColorStringExt(read)) {
			read += 2;
		}
		else if (Q_IsColorStringNT(read)) {
			read += 2;
		}
		else {
			// Avoid writing the same data over itself
			if (write != read) {
				*write = *read;
			}
			write++;
			read++;
		}
	}
	if (write < read) {
		// Add trailing NUL byte if string has shortened
		*write = '\0';
	}
}





/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

/*QUAKED misc_shield_floor_unit (1 0 0) (-16 -16 0) (16 16 40)
#MODELNAME="/models/items/a_shield_converter.md3"
Gives shield energy when used.

"count" - max charge value (default 50)
"chargerate" - rechage 1 point every this many milliseconds (default 3000)
*/

gitem_t	bg_itemlist[] =
{
	{
		NULL,				// classname	
		NULL,				// pickup_sound
		{	NULL,			// world_model[0]
			NULL,			// world_model[1]
			0, 0} ,			// world_model[2],[3]
		NULL,				// view_model
		/* icon */		NULL,		// icon
		/* pickup */	//NULL,		// pickup_name
				0,					// quantity
				(itemType_t)0,					// giType (IT_*)
				0,					// giTag
				/* precache */ "",			// precaches
				/* sounds */ ""				// sounds
					},	// leave index 0 alone

					//
					// Pickups
					//

				/*QUAKED item_shield_sm_instant (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
				Instant shield pickup, restores 25
				*/
					{
						"item_shield_sm_instant",
						"sound/player/pickupshield.wav",
						{ "models/map_objects/mp/psd_sm.md3",
						0, 0, 0},
						/* view */		NULL,
						/* icon */		"gfx/mp/small_shield",
						/* pickup *///	"Shield Small",
								25,
								IT_ARMOR,
								1, //special for shield - max on pickup is maxhealth*tag, thus small shield goes up to 100 shield
								/* precache */ "",
								/* sounds */ ""
									},

	/*QUAKED item_shield_lrg_instant (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Instant shield pickup, restores 100
	*/
		{
			"item_shield_lrg_instant",
			"sound/player/pickupshield.wav",
			{ "models/map_objects/mp/psd.md3",
			0, 0, 0},
			/* view */		NULL,
			/* icon */		"gfx/mp/large_shield",
			/* pickup *///	"Shield Large",
					100,
					IT_ARMOR,
					2, //special for shield - max on pickup is maxhealth*tag, thus large shield goes up to 200 shield
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_medpak_instant (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Instant medpack pickup, heals 25
	*/
		{
			"item_medpak_instant",
			"sound/player/pickuphealth.wav",
			{ "models/map_objects/mp/medpac.md3",
			0, 0, 0 },
			/* view */		NULL,
			/* icon */		"gfx/hud/i_icon_medkit",
			/* pickup *///	"Medpack",
					25,
					IT_HEALTH,
					0,
					/* precache */ "",
					/* sounds */ ""
						},


	//
	// ITEMS
	//

/*QUAKED item_seeker (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
30 seconds of seeker drone
*/
	{
		"item_seeker",
		"sound/weapons/w_pkup.wav",
		{ "models/items/remote.md3",
		0, 0, 0} ,
		/* view */		NULL,
		/* icon */		"gfx/hud/i_icon_seeker",
		/* pickup *///	"Seeker Drone",
				120,
				IT_HOLDABLE,
				HI_SEEKER,
				/* precache */ "",
				/* sounds */ ""
					},

	/*QUAKED item_shield (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
	Portable shield
	*/
		{
			"item_shield",
			"sound/weapons/w_pkup.wav",
			{ "models/map_objects/mp/shield.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/i_icon_shieldwall",
			/* pickup *///	"Forcefield",
					120,
					IT_HOLDABLE,
					HI_SHIELD,
					/* precache */ "",
					/* sounds */ "sound/weapons/detpack/stick.wav sound/movers/doors/forcefield_on.wav sound/movers/doors/forcefield_off.wav sound/movers/doors/forcefield_lp.wav sound/effects/bumpfield.wav",
						},

						/*QUAKED item_medpac (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
						Bacta canister pickup, heals 25 on use
						*/
							{
								"item_medpac",	//should be item_bacta
								"sound/weapons/w_pkup.wav",
								{ "models/map_objects/mp/bacta.md3",
								0, 0, 0} ,
								/* view */		NULL,
								/* icon */		"gfx/hud/i_icon_bacta",
								/* pickup *///	"Bacta Canister",
										25,
										IT_HOLDABLE,
										HI_MEDPAC,
										/* precache */ "",
										/* sounds */ ""
											},

	/*QUAKED item_datapad (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
	Do not place this.
	*/
		{
			"item_datapad",
			"sound/weapons/w_pkup.wav",
			{ "models/items/datapad.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		NULL,
			/* pickup *///	"Datapad",
					1,
					IT_HOLDABLE,
					HI_DATAPAD,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_binoculars (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
	These will be standard equipment on the player - DO NOT PLACE
	*/
		{
			"item_binoculars",
			"sound/weapons/w_pkup.wav",
			{ "models/items/binoculars.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/i_icon_zoom",
			/* pickup *///	"Binoculars",
					60,
					IT_HOLDABLE,
					HI_BINOCULARS,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_sentry_gun (.3 .3 1) (-8 -8 -0) (8 8 16) suspended
	Sentry gun inventory pickup.
	*/
		{
			"item_sentry_gun",
			"sound/weapons/w_pkup.wav",
			{ "models/items/psgun.glm",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/i_icon_sentrygun",
			/* pickup *///	"Sentry Gun",
					120,
					IT_HOLDABLE,
					HI_SENTRY_GUN,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_force_enlighten_light (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Adds one rank to all Force powers temporarily. Only light jedi can use.
	*/
		{
			"item_force_enlighten_light",
			"sound/player/enlightenment.wav",
			{ "models/map_objects/mp/jedi_enlightenment.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/mpi_jlight",
			/* pickup *///	"Light Force Enlightenment",
					25,
					IT_POWERUP,
					PW_FORCE_ENLIGHTENED_LIGHT,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_force_enlighten_dark (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Adds one rank to all Force powers temporarily. Only dark jedi can use.
	*/
		{
			"item_force_enlighten_dark",
			"sound/player/enlightenment.wav",
			{ "models/map_objects/mp/dk_enlightenment.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/mpi_dklight",
			/* pickup *///	"Dark Force Enlightenment",
					25,
					IT_POWERUP,
					PW_FORCE_ENLIGHTENED_DARK,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_force_boon (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Unlimited Force Pool for a short time.
	*/
		{
			"item_force_boon",
			"sound/player/boon.wav",
			{ "models/map_objects/mp/force_boon.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/mpi_fboon",
			/* pickup *///	"Force Boon",
					25,
					IT_POWERUP,
					PW_FORCE_BOON,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED item_ysalimari (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	A small lizard carried on the player, which prevents the possessor from using any Force power.  However, he is unaffected by any Force power.
	*/
		{
			"item_ysalimari",
			"sound/player/ysalimari.wav",
			{ "models/map_objects/mp/ysalimari.md3",
			0, 0, 0} ,
			/* view */		NULL,
			/* icon */		"gfx/hud/mpi_ysamari",
			/* pickup *///	"Ysalamiri",
					25,
					IT_POWERUP,
					PW_YSALAMIRI,
					/* precache */ "",
					/* sounds */ ""
						},

	//
	// WEAPONS 
	//

/*QUAKED weapon_stun_baton (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
Don't place this
*/
	{
		"weapon_stun_baton",
		"sound/weapons/w_pkup.wav",
		{ "models/weapons2/stun_baton/baton_w.glm",
		0, 0, 0},
		/* view */		"models/weapons2/stun_baton/baton.md3",
		/* icon */		"gfx/hud/w_icon_stunbaton",
		/* pickup *///	"Stun Baton",
				100,
				IT_WEAPON,
				WP_STUN_BATON,
				/* precache */ "",
				/* sounds */ ""
					},

	/*QUAKED weapon_saber (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Don't place this
	*/
		{
			"weapon_saber",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/saber/saber_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/saber/saber_w.md3",
			/* icon */		"gfx/hud/w_icon_lightsaber",
			/* pickup *///	"Lightsaber",
					100,
					IT_WEAPON,
					WP_SABER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_bryar_pistol (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Don't place this
	*/
		{
			"weapon_bryar_pistol",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/briar_pistol/briar_pistol_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/briar_pistol/briar_pistol.md3",
			/* icon */		"gfx/hud/w_icon_rifle",
			/* pickup *///	"Bryar Pistol",
					100,
					IT_WEAPON,
					WP_BRYAR_PISTOL,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_blaster (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_blaster",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/blaster_r/blaster_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/blaster_r/blaster.md3",
			/* icon */		"gfx/hud/w_icon_blaster",
			/* pickup *///	"E11 Blaster Rifle",
					100,
					IT_WEAPON,
					WP_BLASTER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_disruptor (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_disruptor",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/disruptor/disruptor_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/disruptor/disruptor.md3",
			/* icon */		"gfx/hud/w_icon_disruptor",
			/* pickup *///	"Tenloss Disruptor Rifle",
					100,
					IT_WEAPON,
					WP_DISRUPTOR,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_bowcaster (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_bowcaster",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/bowcaster/bowcaster_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/bowcaster/bowcaster.md3",
			/* icon */		"gfx/hud/w_icon_bowcaster",
			/* pickup *///	"Wookiee Bowcaster",
					100,
					IT_WEAPON,
					WP_BOWCASTER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_repeater (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_repeater",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/heavy_repeater/heavy_repeater_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/heavy_repeater/heavy_repeater.md3",
			/* icon */		"gfx/hud/w_icon_repeater",
			/* pickup *///	"Imperial Heavy Repeater",
					100,
					IT_WEAPON,
					WP_REPEATER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_demp2 (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	NOTENOTE This weapon is not yet complete.  Don't place it.
	*/
		{
			"weapon_demp2",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/demp2/demp2_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/demp2/demp2.md3",
			/* icon */		"gfx/hud/w_icon_demp2",
			/* pickup *///	"DEMP2",
					100,
					IT_WEAPON,
					WP_DEMP2,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_flechette (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_flechette",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/golan_arms/golan_arms_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/golan_arms/golan_arms.md3",
			/* icon */		"gfx/hud/w_icon_flechette",
			/* pickup *///	"Golan Arms Flechette",
					100,
					IT_WEAPON,
					WP_FLECHETTE,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_rocket_launcher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_rocket_launcher",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/merr_sonn/merr_sonn_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/merr_sonn/merr_sonn.md3",
			/* icon */		"gfx/hud/w_icon_merrsonn",
			/* pickup *///	"Merr-Sonn Missile System",
					3,
					IT_WEAPON,
					WP_ROCKET_LAUNCHER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_thermal (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"ammo_thermal",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/thermal/thermal_pu.md3",
			"models/weapons2/thermal/thermal_w.glm", 0, 0},
			/* view */		"models/weapons2/thermal/thermal.md3",
			/* icon */		"gfx/hud/w_icon_thermal",
			/* pickup *///	"Thermal Detonators",
					4,
					IT_AMMO,
					AMMO_THERMAL,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_tripmine (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"ammo_tripmine",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/laser_trap/laser_trap_pu.md3",
			"models/weapons2/laser_trap/laser_trap_w.glm", 0, 0},
			/* view */		"models/weapons2/laser_trap/laser_trap.md3",
			/* icon */		"gfx/hud/w_icon_tripmine",
			/* pickup *///	"Trip Mines",
					3,
					IT_AMMO,
					AMMO_TRIPMINE,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_detpack (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"ammo_detpack",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/detpack/det_pack_pu.md3", "models/weapons2/detpack/det_pack_proj.glm", "models/weapons2/detpack/det_pack_w.glm", 0},
			/* view */		"models/weapons2/detpack/det_pack.md3",
			/* icon */		"gfx/hud/w_icon_detpack",
			/* pickup *///	"Det Packs",
					3,
					IT_AMMO,
					AMMO_DETPACK,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_thermal (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_thermal",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/thermal/thermal_w.glm", "models/weapons2/thermal/thermal_pu.md3",
			0, 0 },
			/* view */		"models/weapons2/thermal/thermal.md3",
			/* icon */		"gfx/hud/w_icon_thermal",
			/* pickup *///	"Thermal Detonator",
					4,
					IT_WEAPON,
					WP_THERMAL,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_trip_mine (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_trip_mine",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/laser_trap/laser_trap_w.glm", "models/weapons2/laser_trap/laser_trap_pu.md3",
			0, 0},
			/* view */		"models/weapons2/laser_trap/laser_trap.md3",
			/* icon */		"gfx/hud/w_icon_tripmine",
			/* pickup *///	"Trip Mine",
					3,
					IT_WEAPON,
					WP_TRIP_MINE,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_det_pack (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_det_pack",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/detpack/det_pack_proj.glm", "models/weapons2/detpack/det_pack_pu.md3", "models/weapons2/detpack/det_pack_w.glm", 0},
			/* view */		"models/weapons2/detpack/det_pack.md3",
			/* icon */		"gfx/hud/w_icon_detpack",
			/* pickup *///	"Det Pack",
					3,
					IT_WEAPON,
					WP_DET_PACK,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED weapon_emplaced (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	*/
		{
			"weapon_emplaced",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/blaster_r/blaster_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/blaster_r/blaster.md3",
			/* icon */		"gfx/hud/w_icon_blaster",
			/* pickup *///	"Emplaced Gun",
					50,
					IT_WEAPON,
					WP_EMPLACED_GUN,
					/* precache */ "",
					/* sounds */ ""
						},


	//NOTE: This is to keep things from messing up because the turret weapon type isn't real
		{
			"weapon_turretwp",
			"sound/weapons/w_pkup.wav",
			{ "models/weapons2/blaster_r/blaster_w.glm",
			0, 0, 0},
			/* view */		"models/weapons2/blaster_r/blaster.md3",
			/* icon */		"gfx/hud/w_icon_blaster",
			/* pickup *///	"Turret Gun",
					50,
					IT_WEAPON,
					WP_TURRET,
					/* precache */ "",
					/* sounds */ ""
						},

	//
	// AMMO ITEMS
	//

/*QUAKED ammo_force (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
Don't place this
*/
	{
		"ammo_force",
		"sound/player/pickupenergy.wav",
		{ "models/items/energy_cell.md3",
		0, 0, 0},
		/* view */		NULL,
		/* icon */		"gfx/hud/w_icon_blaster",
		/* pickup *///	"Force??",
				100,
				IT_AMMO,
				AMMO_FORCE,
				/* precache */ "",
				/* sounds */ ""
					},

	/*QUAKED ammo_blaster (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo for the Bryar and Blaster pistols.
	*/
		{
			"ammo_blaster",
			"sound/player/pickupenergy.wav",
			{ "models/items/energy_cell.md3",
			0, 0, 0},
			/* view */		NULL,
			/* icon */		"gfx/hud/i_icon_battery",
			/* pickup *///	"Blaster Pack",
					100,
					IT_AMMO,
					AMMO_BLASTER,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_powercell (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo for Tenloss Disruptor, Wookie Bowcaster, and the Destructive Electro Magnetic Pulse (demp2 ) guns
	*/
		{
			"ammo_powercell",
			"sound/player/pickupenergy.wav",
			{ "models/items/power_cell.md3",
			0, 0, 0},
			/* view */		NULL,
			/* icon */		"gfx/mp/ammo_power_cell",
			/* pickup *///	"Power Cell",
					100,
					IT_AMMO,
					AMMO_POWERCELL,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_metallic_bolts (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo for Imperial Heavy Repeater and the Golan Arms Flechette
	*/
		{
			"ammo_metallic_bolts",
			"sound/player/pickupenergy.wav",
			{ "models/items/metallic_bolts.md3",
			0, 0, 0},
			/* view */		NULL,
			/* icon */		"gfx/mp/ammo_metallic_bolts",
			/* pickup *///	"Metallic Bolts",
					100,
					IT_AMMO,
					AMMO_METAL_BOLTS,
					/* precache */ "",
					/* sounds */ ""
						},

	/*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo for Merr-Sonn portable missile launcher
	*/
		{
			"ammo_rockets",
			"sound/player/pickupenergy.wav",
			{ "models/items/rockets.md3",
			0, 0, 0},
			/* view */		NULL,
			/* icon */		"gfx/mp/ammo_rockets",
			/* pickup *///	"Rockets",
					3,
					IT_AMMO,
					AMMO_ROCKETS,
					/* precache */ "",
					/* sounds */ ""
						},


	//
	// POWERUP ITEMS
	//
/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
	{
		"team_CTF_redflag",
		NULL,
		{ "models/flags/r_flag.md3",
		"models/flags/r_flag_ysal.md3", 0, 0 },
		/* view */		NULL,
		/* icon */		"gfx/hud/mpi_rflag",
		/* pickup *///	"Red Flag",
				0,
				IT_TEAM,
				PW_REDFLAG,
				/* precache */ "",
				/* sounds */ ""
					},

	/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
	Only in CTF games
	*/
		{
			"team_CTF_blueflag",
			NULL,
			{ "models/flags/b_flag.md3",
			"models/flags/b_flag_ysal.md3", 0, 0 },
			/* view */		NULL,
			/* icon */		"gfx/hud/mpi_bflag",
			/* pickup *///	"Blue Flag",
					0,
					IT_TEAM,
					PW_BLUEFLAG,
					/* precache */ "",
					/* sounds */ ""
						},

	//
	// PERSISTANT POWERUP ITEMS
	//

	/*QUAKED team_CTF_neutralflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in One Flag CTF games
*/
	{
		"team_CTF_neutralflag",
		NULL,
		{ "models/flags/n_flag.md3",
		0, 0, 0 },
		/* view */		NULL,
		/* icon */		"icons/iconf_neutral1",
		/* pickup *///	"Neutral Flag",
				0,
				IT_TEAM,
				PW_NEUTRALFLAG,
				/* precache */ "",
				/* sounds */ ""
					},

					{
						"item_redcube",
						"sound/player/pickupenergy.wav",
						{ "models/powerups/orb/r_orb.md3",
						0, 0, 0 },
						/* view */		NULL,
						/* icon */		"icons/iconh_rorb",
						/* pickup *///	"Red Cube",
								0,
								IT_TEAM,
								0,
								/* precache */ "",
								/* sounds */ ""
									},

									{
										"item_bluecube",
										"sound/player/pickupenergy.wav",
										{ "models/powerups/orb/b_orb.md3",
										0, 0, 0 },
										/* view */		NULL,
										/* icon */		"icons/iconh_borb",
										/* pickup *///	"Blue Cube",
												0,
												IT_TEAM,
												0,
												/* precache */ "",
												/* sounds */ ""
													},

	// end of list marker
	{NULL}
};

int		bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;



int forceMasteryPoints[NUM_FORCE_MASTERY_LEVELS] =
{
	0,		// FORCE_MASTERY_UNINITIATED,
	5,		// FORCE_MASTERY_INITIATE,
	10,		// FORCE_MASTERY_PADAWAN,
	20,		// FORCE_MASTERY_JEDI,
	30,		// FORCE_MASTERY_JEDI_GUARDIAN,
	50,		// FORCE_MASTERY_JEDI_ADEPT,
	75,		// FORCE_MASTERY_JEDI_KNIGHT,
	100		// FORCE_MASTERY_JEDI_MASTER,
};

int bgForcePowerCost[NUM_FORCE_POWERS][NUM_FORCE_POWER_LEVELS] = //0 == neutral
{
	{	0,	2,	4,	6	},	// Heal			// FP_HEAL
	{	0,	0,	2,	6	},	// Jump			//FP_LEVITATION,//hold/duration
	{	0,	2,	4,	6	},	// Speed		//FP_SPEED,//duration
	{	0,	1,	3,	6	},	// Push			//FP_PUSH,//hold/duration
	{	0,	1,	3,	6	},	// Pull			//FP_PULL,//hold/duration
	{	0,	4,	6,	8	},	// Mind Trick	//FP_TELEPATHY,//instant
	{	0,	1,	3,	6	},	// Grip			//FP_GRIP,//hold/duration
	{	0,	2,	5,	8	},	// Lightning	//FP_LIGHTNING,//hold/duration
	{	0,	4,	6,	8	},	// Dark Rage	//FP_RAGE,//duration
	{	0,	2,	5,	8	},	// Protection	//FP_PROTECT,//duration
	{	0,	1,	3,	6	},	// Absorb		//FP_ABSORB,//duration
	{	0,	1,	3,	6	},	// Team Heal	//FP_TEAM_HEAL,//instant
	{	0,	1,	3,	6	},	// Team Force	//FP_TEAM_FORCE,//instant
	{	0,	2,	4,	6	},	// Drain		//FP_DRAIN,//hold/duration
	{	0,	2,	5,	8	},	// Sight		//FP_SEE,//duration
	{	0,	1,	5,	8	},	// Saber Attack	//FP_SABERATTACK,
	{	0,	1,	5,	8	},	// Saber Defend	//FP_SABERDEFEND,
	{	0,	4,	6,	8	}	// Saber Throw	//FP_SABERTHROW,
	//NUM_FORCE_POWERS
};

float forceJumpHeight[NUM_FORCE_POWER_LEVELS] =
{
	32,//normal jump (+stepheight+crouchdiff = 66)
	96,//(+stepheight+crouchdiff = 130)
	192,//(+stepheight+crouchdiff = 226)
	384//(+stepheight+crouchdiff = 418)
};

float forceJumpStrength[NUM_FORCE_POWER_LEVELS] =
{
	JUMP_VELOCITY,//normal jump
	420,
	590,
	840
};



qboolean BG_SaberInSpecialAttack(int anim)
{
	switch (anim & ~ANIM_TOGGLEBIT)
	{
	case BOTH_A2_STABBACK1:
	case BOTH_ATTACK_BACK:
	case BOTH_CROUCHATTACKBACK1:
	case BOTH_BUTTERFLY_LEFT:
	case BOTH_BUTTERFLY_RIGHT:
	case BOTH_FJSS_TR_BL:
	case BOTH_FJSS_TL_BR:
	case BOTH_LUNGE2_B__T_:
	case BOTH_FORCELEAP2_T__B_:
	case BOTH_JUMPFLIPSLASHDOWN1://#
	case BOTH_JUMPFLIPSTABDOWN://#
		return qtrue;
	}
	return qfalse;
}

void vectoangles(const vec3_t value1, vec3_t angles) {
	float	forward;
	float	yaw, pitch;

	if (value1[1] == 0 && value1[0] == 0) {
		yaw = 0;
		if (value1[2] > 0) {
			pitch = 90;
		}
		else {
			pitch = 270;
		}
	}
	else {
		if (value1[0]) {
			yaw = (atan2(value1[1], value1[0]) * 180 / M_PI);
		}
		else if (value1[1] > 0) {
			yaw = 90;
		}
		else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}

		forward = sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = (atan2(value1[2], forward) * 180 / M_PI);
		if (pitch < 0) {
			pitch += 360;
		}
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}


void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up) {
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * (M_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}
	if (up)
	{
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}

/*
=================
AngleSubtract

Always returns a value from -180 to 180
=================
*/
float	AngleSubtract(float a1, float a2) {
	float	a;

	a = a1 - a2;
	assert(fabs(a) < 3600);
	while (a > 180) {
		a -= 360;
	}
	while (a < -180) {
		a += 360;
	}
	return a;
}


void AnglesSubtract(vec3_t v1, vec3_t v2, vec3_t v3) {
	v3[0] = AngleSubtract(v1[0], v2[0]);
	v3[1] = AngleSubtract(v1[1], v2[1]);
	v3[2] = AngleSubtract(v1[2], v2[2]);
}

qboolean BG_SaberInAttack(int move)
{
	if (move >= LS_A_TL2BR && move <= LS_A_T2B)
	{
		return qtrue;
	}
	switch (move)
	{
	case LS_A_BACK:
	case LS_A_BACK_CR:
	case LS_A_BACKSTAB:
	case LS_A_LUNGE:
	case LS_A_JUMP_T__B_:
	case LS_A_FLIP_STAB:
	case LS_A_FLIP_SLASH:
		return qtrue;
		break;
	}
	return qfalse;
}

// TODO Handle JKA?
qboolean PM_InSaberAnim(int anim,demoType_t demoType)
{
	if (demoType == DM_16 && (anim & ~ANIM_TOGGLEBIT) >= BOTH_A1_T__B_ && (anim & ~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR
		|| demoType == DM_15 && (anim & ~ANIM_TOGGLEBIT) >= BOTH_A1_T__B__15 && (anim & ~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR_15
		)
	{
		return qtrue;
	}
	return qfalse;
}


qboolean PM_SaberInBrokenParry(int move, demoType_t demoType)
{
	if (move >= LS_V1_BR && move <= LS_V1_B_ && demoType != DM_15) // This should be actually be check for 1.02 specifically TODO
	{
		return qtrue;
	}
	if (move >= LS_H1_T_ && move <= LS_H1_BL)
	{
		return qtrue;
	}
	return qfalse;
}


















































qboolean CL_ServerVersionIs103(const char* versionstr) {
	return strstr(versionstr, "v1.03") ? qtrue : qfalse;
}

#include "zlib/zlib.h"
void demoCutParseRMG(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut) {
	int i;
	clcCut->rmgHeightMapSize = (unsigned short)MSG_ReadShort(msg);
	if (clcCut->rmgHeightMapSize == 0) {
		return;
	}
	z_stream zdata;
	int flatDataSize;
	unsigned char heightmap1[15000];
	// height map
	if (MSG_ReadBits(msg, 1)) {
		memset(&zdata, 0, sizeof(z_stream));
		inflateInit(&zdata/*, Z_SYNC_FLUSH*/);
		MSG_ReadData(msg, heightmap1, clcCut->rmgHeightMapSize);
		zdata.next_in = heightmap1;
		zdata.avail_in = clcCut->rmgHeightMapSize;
		zdata.next_out = (unsigned char*)clcCut->rmgHeightMap;
		zdata.avail_out = MAX_HEIGHTMAP_SIZE;
		inflate(&zdata, Z_SYNC_FLUSH);
		clcCut->rmgHeightMapSize = zdata.total_out;
		inflateEnd(&zdata);
	}
	else {
		MSG_ReadData(msg, (unsigned char*)clcCut->rmgHeightMap, clcCut->rmgHeightMapSize);
	}
	// Flatten map
	flatDataSize = MSG_ReadShort(msg);
	if (MSG_ReadBits(msg, 1)) {
		// Read the flatten map
		memset(&zdata, 0, sizeof(z_stream));
		inflateInit(&zdata/*, Z_SYNC_FLUSH*/);
		MSG_ReadData(msg, heightmap1, flatDataSize);
		zdata.next_in = heightmap1;
		zdata.avail_in = clcCut->rmgHeightMapSize;
		zdata.next_out = (unsigned char*)clcCut->rmgFlattenMap;
		zdata.avail_out = MAX_HEIGHTMAP_SIZE;
		inflate(&zdata, Z_SYNC_FLUSH);
		inflateEnd(&zdata);
	}
	else {
		MSG_ReadData(msg, (unsigned char*)clcCut->rmgFlattenMap, flatDataSize);
	}
	// Seed
	clcCut->rmgSeed = MSG_ReadLong(msg);
	// Automap symbols
	clcCut->rmgAutomapSymbolCount = (unsigned short)MSG_ReadShort(msg);
	for (i = 0; i < clcCut->rmgAutomapSymbolCount; i++) {
		clcCut->rmgAutomapSymbols[i].mType = (int)MSG_ReadByte(msg);
		clcCut->rmgAutomapSymbols[i].mSide = (int)MSG_ReadByte(msg);
		clcCut->rmgAutomapSymbols[i].mOrigin[0] = (float)MSG_ReadLong(msg);
		clcCut->rmgAutomapSymbols[i].mOrigin[1] = (float)MSG_ReadLong(msg);
	}
}


//
//
// Shared demo parsing functions
//
qboolean demoCutParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t* demoType) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd;
	char* s;

	//int svc_EOF_realCMD = *demoType == DM_26 ? svc_EOF + 1 : svc_EOF;
	//int maxAllowedConfigString = *demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(*demoType);

	clcCut->connectPacketCount = 0;
	Com_Memset(clCut, 0, sizeof(*clCut));
	clcCut->serverCommandSequence = MSG_ReadLong(msg);
	clCut->gameState.dataCount = 1;
	while (1) {
		cmd = MSG_ReadByte(msg);
		cmd = generalizeGameSVCOp(cmd,*demoType);
		if (cmd == svc_EOF_general) {
			break;
		}
		if (cmd == svc_configstring_general) {
			int len, start;
			start = msg->readcount;
			i = MSG_ReadShort(msg);
			if (i < 0 || i >= maxAllowedConfigString) {
				Com_Printf("configstring > MAX_CONFIGSTRINGS");
				return qfalse;
			}
			s = MSG_ReadBigString(msg);
			len = strlen(s);
			if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
				Com_Printf("MAX_GAMESTATE_CHARS exceeded");
				return qfalse;
			}

			if (clcCut->demoCheckFor103 && i == CS_SERVERINFO && *demoType == DM_15) {
				//This is the big serverinfo string containing the value of the "version" cvar of the server.
				//If we are about to play a demo, we can use this information to ascertain whether this demo was recorded on
				//a 1.03 server.
				if (CL_ServerVersionIs103(Info_ValueForKey(s, BIG_INFO_STRING, "version"))) {
					//A 1.03 demo - set the proper game version internally so parsing snapshots etc won't fail
					*demoType = DM_15_1_03;
				}

				clcCut->demoCheckFor103 = qfalse; //No need to check this again while playing the demo.
			}

			// append it to the gameState string buffer
			clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
			Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, s, len + 1);
			clCut->gameState.dataCount += len + 1;
		}
		else if (cmd == svc_baseline_general) {
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
			if (newnum < 0 || newnum >= MAX_GENTITIES) {
				Com_Printf("Baseline number out of range: %i", newnum);
				return qfalse;
			}
			Com_Memset(&nullstate, 0, sizeof(nullstate));
			es = &clCut->entityBaselines[newnum];
			MSG_ReadDeltaEntity(msg, &nullstate, es, newnum, *demoType);
		}
		else {
			Com_Printf("demoCutParseGameState: bad command byte");
			return qfalse;
		}
	}
	clcCut->clientNum = MSG_ReadLong(msg);
	clcCut->checksumFeed = MSG_ReadLong(msg);

	// RMG stuff (JKA specific)
	if (*demoType == DM_26 || *demoType == DM_25) {
		demoCutParseRMG(msg, clcCut, clCut);
	}

	return qtrue;
}


void demoCutParsePacketEntities(msg_t* msg, clSnapshot_t* oldSnap, clSnapshot_t* newSnap, clientActive_t* clCut, demoType_t demoType) {
	/* The beast that is entity parsing */
	int			newnum;
	entityState_t* oldstate, * newstate;
	int			oldindex = 0;
	int			oldnum;
	newSnap->parseEntitiesNum = clCut->parseEntitiesNum;
	newSnap->numEntities = 0;
	newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
	while (1) {
		// read the entity index number
		if (oldSnap && oldindex < oldSnap->numEntities) {
			oldstate = &clCut->parseEntities[(oldSnap->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			oldnum = oldstate->number;
		}
		else {
			oldstate = 0;
			oldnum = 99999;
		}
		newstate = &clCut->parseEntities[clCut->parseEntitiesNum];
		if (!oldstate && (newnum == (MAX_GENTITIES - 1))) {
			break;
		}
		else if (oldnum < newnum) {
			*newstate = *oldstate;
			oldindex++;
		}
		else if (oldnum == newnum) {
			oldindex++;
			MSG_ReadDeltaEntity(msg, oldstate, newstate, newnum, demoType);
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		else if (oldnum > newnum) {
			MSG_ReadDeltaEntity(msg, &clCut->entityBaselines[newnum], newstate, newnum, demoType);
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
		}
		if (newstate->number == MAX_GENTITIES - 1)
			continue;
		clCut->parseEntitiesNum++;
		clCut->parseEntitiesNum &= (MAX_PARSE_ENTITIES - 1);
		newSnap->numEntities++;
	}
}

void demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut) {
	int index;
	int seq = MSG_ReadLong(msg);
	char* s = MSG_ReadString(msg);
	if (clcCut->serverCommandSequence >= seq) {
		return;
	}
	clcCut->serverCommandSequence = seq;
	index = seq & (MAX_RELIABLE_COMMANDS - 1);
	Q_strncpyz(clcCut->serverCommands[index], MAX_STRING_CHARS, s, sizeof(clcCut->serverCommands[index]));
}

qboolean demoCutConfigstringModified(clientActive_t* clCut, demoType_t demoType) {
	char* old, * s;
	int			i, index;
	char* dup;
	gameState_t	oldGs;
	int			len;
	index = atoi(Cmd_Argv(1));
	//int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(demoType);
	if (index < 0 || index >= maxAllowedConfigString) {
		Com_Printf("demoCutConfigstringModified: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = Cmd_ArgsFrom(2);
	old = clCut->gameState.stringData + clCut->gameState.stringOffsets[index];
	if (!strcmp(old, s)) {
		return qtrue; // unchanged
	}
	// build the new gameState_t
	oldGs = clCut->gameState;
	Com_Memset(&clCut->gameState, 0, sizeof(clCut->gameState));
	// leave the first 0 for uninitialized strings
	clCut->gameState.dataCount = 1;
	for (i = 0; i < maxAllowedConfigString; i++) {
		if (i == index) {
			dup = s;
		}
		else {
			dup = oldGs.stringData + oldGs.stringOffsets[i];
		}
		if (!dup[0]) {
			continue; // leave with the default empty string
		}
		len = strlen(dup);
		if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
			Com_Printf("MAX_GAMESTATE_CHARS exceeded on %d:%s", index, s);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}

qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean writeOldSnap) {
	int len;
	clSnapshot_t* oldSnap;
	clSnapshot_t newSnap;
	int deltaNum;
	int oldMessageNum;
	int i, packetNum;
	Com_Memset(&newSnap, 0, sizeof(newSnap));
	clCut->lastSnapshotFinishedParsing = qfalse;
	newSnap.serverCommandNum = clcCut->serverCommandSequence;
	newSnap.serverTime = MSG_ReadLong(msg);
	//cl_paused->modified = qfalse;
	newSnap.messageNum = clcCut->serverMessageSequence;
	deltaNum = MSG_ReadByte(msg);
	newSnap.snapFlags = MSG_ReadByte(msg);
	if (!deltaNum) {
		newSnap.deltaNum = -1;
		newSnap.valid = qtrue;		// uncompressed frame
		oldSnap = NULL;
	}
	else {
		newSnap.deltaNum = newSnap.messageNum - deltaNum;
		int lastOneNum = newSnap.deltaNum;
		oldSnap = &clCut->snapshots[newSnap.deltaNum & PACKET_MASK];

		// Try work with it anyway?
		if (!oldSnap->valid && clcCut->lastValidSnapSet) {
			// should never happen
			oldSnap = &clCut->snapshots[clcCut->lastValidSnap & PACKET_MASK];
			lastOneNum = clcCut->lastValidSnap;
			Com_Printf("Delta from invalid frame (not supposed to happen!). Trying to use last valid snap.\n");
		}

		if (!oldSnap->valid) {
			// should never happen
			Com_Printf("Delta from invalid frame (not supposed to happen!).\n");
		}
		//else if (oldSnap->messageNum != newSnap.deltaNum) {
		else if (oldSnap->messageNum != lastOneNum) {
			// The frame that the server did the delta from
			// is too old, so we can't reconstruct it properly.
			Com_Printf("Delta frame too old.\n");
		}
		else if (clCut->parseEntitiesNum - oldSnap->parseEntitiesNum > MAX_PARSE_ENTITIES - 128) {
			Com_DPrintf("Delta parseEntitiesNum too old.\n");
		}
		else {
			newSnap.valid = qtrue;	// valid delta parse
		}
	}
	// read areamask
	len = MSG_ReadByte(msg);
	//if (len > sizeof(newSnap.areamask)) {
	//	Com_Printf("demoCutParseSnapshot: Invalid size %d for areamask", len);
	//	return qfalse;
	//}
	MSG_ReadData(msg, &newSnap.areamask, len);
	// read playerinfo
	MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps, demoType, qfalse);

	// JKA-specific
	if ((demoType == DM_26 || demoType == DM_25) && newSnap.ps.m_iVehicleNum)
		MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->vps : NULL, &newSnap.vps, demoType, qtrue);

	// read packet entities
	demoCutParsePacketEntities(msg, oldSnap, &newSnap, clCut, demoType);
	// if not valid, dump the entire thing now that it has
	// been properly read
	if (!newSnap.valid) {
		return qtrue;
	}
	clcCut->lastValidSnap = newSnap.messageNum;
	clcCut->lastValidSnapSet = qtrue;

	// clear the valid flags of any snapshots between the last
	// received and this one, so if there was a dropped packet
	// it won't look like something valid to delta from next
	// time we wrap around in the buffer
	oldMessageNum = clCut->snap.messageNum + 1;
	if (newSnap.messageNum - oldMessageNum >= PACKET_BACKUP) {
		oldMessageNum = newSnap.messageNum - (PACKET_BACKUP - 1);
	}
	for (; oldMessageNum < newSnap.messageNum; oldMessageNum++) {
		clCut->snapshots[oldMessageNum & PACKET_MASK].valid = qfalse;
	}
	// copy to the current good spot
	if (writeOldSnap) {
		clCut->oldSnap = clCut->snap; // TODO OPTIONAL
	}
	clCut->snap = newSnap;
	clCut->snap.ping = 999;
	// calculate ping time
	for (i = 0; i < PACKET_BACKUP; i++) {
		packetNum = (clcCut->netchan.outgoingSequence - 1 - i) & PACKET_MASK;
		if (clCut->snap.ps.commandTime >= clCut->outPackets[packetNum].p_serverTime) {
			//clCut->snap.ping = cls.realtime - clCut->outPackets[packetNum].p_realtime;
			// Dont see how this is needed?
			break;
		}
	}
	// save the frame off in the backup array for later delta comparisons
	clCut->snapshots[clCut->snap.messageNum & PACKET_MASK] = clCut->snap;
	clCut->newSnapshots = qtrue;
	clCut->lastSnapshotFinishedParsing = qtrue;

	if (clcCut->firstSnapServerTime == -1) {
		clcCut->firstSnapServerTime = clCut->snap.serverTime;
	}

	clcCut->lastKnownCommandTime = clCut->snap.ps.commandTime;


	return qtrue;
}

void demoCutEmitPacketEntities(clSnapshot_t* from, clSnapshot_t* to, msg_t* msg, clientActive_t* clCut, demoType_t demoType) {
	entityState_t* oldent, * newent;
	int oldindex, newindex;
	int oldnum, newnum;
	int from_num_entities;
	// generate the delta update
	if (!from) {
		from_num_entities = 0;
	}
	else {
		from_num_entities = from->numEntities;
	}
	newent = NULL;
	oldent = NULL;
	newindex = 0;
	oldindex = 0;
	while (newindex < to->numEntities || oldindex < from_num_entities) {
		if (newindex >= to->numEntities) {
			newnum = 9999;
		}
		else {
			newent = &clCut->parseEntities[(to->parseEntitiesNum + newindex) & (MAX_PARSE_ENTITIES - 1)];
			newnum = newent->number;
		}
		if (oldindex >= from_num_entities) {
			oldnum = 9999;
		}
		else {
			oldent = &clCut->parseEntities[(from->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			oldnum = oldent->number;
		}
		if (newnum == oldnum) {
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, demoType);
			oldindex++;
			newindex++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue, demoType);
			newindex++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, demoType);
			oldindex++;
			continue;
		}
	}
	MSG_WriteBits(msg, (MAX_GENTITIES - 1), GENTITYNUM_BITS);	// end of packetentities
}

void demoCutWriteDemoMessage(msg_t* msg, fileHandle_t f, clientConnection_t* clcCut) {
	int len;
	// write the packet sequence
	len = LittleLong(clcCut->serverMessageSequence);
	FS_Write(&len, 4, f);
	// skip the packet sequencing information
	len = LittleLong(msg->cursize);
	FS_Write(&len, 4, f);
	if (msg->raw) {
		FS_Write(msg->dataRaw->data(), msg->dataRaw->size(), f);
	}
	else {
		FS_Write(msg->data, msg->cursize, f);
	}
}

void demoCutWriteDemoHeader(fileHandle_t f, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw) {
	byte			bufData[MAX_MSGLEN];
	std::vector<byte>			bufDataRaw;
	msg_t			buf;
	int				i;
	int				len;
	entityState_t* ent;
	entityState_t	nullstate;
	char* s;

	//int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(demoType);

	// write out the gamestate message
	if (raw) {
		bufDataRaw.clear();
		MSG_InitRaw(&buf, &bufDataRaw);
	}
	else {
		MSG_Init(&buf, bufData, sizeof(bufData));
	}
	MSG_Bitstream(&buf);
	// NOTE, MRE: all server->client messages now acknowledge
	MSG_WriteLong(&buf, clcCut->reliableSequence);
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_gamestate_general,demoType));
	MSG_WriteLong(&buf, clcCut->serverCommandSequence);
	// configstrings
	for (i = 0; i < maxAllowedConfigString; i++) {
		if (!clCut->gameState.stringOffsets[i]) {
			continue;
		}
		s = clCut->gameState.stringData + clCut->gameState.stringOffsets[i];
		MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_configstring_general, demoType));
		MSG_WriteShort(&buf, i);
		MSG_WriteBigString(&buf, s);
	}
	// baselines
	Com_Memset(&nullstate, 0, sizeof(nullstate));
	for (i = 0; i < MAX_GENTITIES; i++) {
		ent = &clCut->entityBaselines[i];
		if (!ent->number) {
			continue;
		}
		MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_baseline_general, demoType));
		MSG_WriteDeltaEntity(&buf, &nullstate, ent, qtrue, demoType);
	}
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_EOF_general, demoType));
	// finished writing the gamestate stuff
	// write the client num
	MSG_WriteLong(&buf, clcCut->clientNum);
	// write the checksum feed
	MSG_WriteLong(&buf, clcCut->checksumFeed);

	if (demoType == DM_26 || demoType == DM_25) {
		// RMG stuff
		if (clcCut->rmgHeightMapSize) {
			// Height map
			MSG_WriteShort(&buf, (unsigned short)clcCut->rmgHeightMapSize);
			MSG_WriteBits(&buf, 0, 1);
			MSG_WriteData(&buf, clcCut->rmgHeightMap, clcCut->rmgHeightMapSize);
			// Flatten map
			MSG_WriteShort(&buf, (unsigned short)clcCut->rmgHeightMapSize);
			MSG_WriteBits(&buf, 0, 1);
			MSG_WriteData(&buf, clcCut->rmgFlattenMap, clcCut->rmgHeightMapSize);
			// Seed
			MSG_WriteLong(&buf, clcCut->rmgSeed);
			// Automap symbols
			MSG_WriteShort(&buf, (unsigned short)clcCut->rmgAutomapSymbolCount);
			for (i = 0; i < clcCut->rmgAutomapSymbolCount; i++) {
				MSG_WriteByte(&buf, (unsigned char)clcCut->rmgAutomapSymbols[i].mType);
				MSG_WriteByte(&buf, (unsigned char)clcCut->rmgAutomapSymbols[i].mSide);
				MSG_WriteLong(&buf, (long)clcCut->rmgAutomapSymbols[i].mOrigin[0]);
				MSG_WriteLong(&buf, (long)clcCut->rmgAutomapSymbols[i].mOrigin[1]);
			}
		}
		else {
			MSG_WriteShort(&buf, 0);
		}
	}

	// finished writing the client packet
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_EOF_general, demoType));
	// write it to the demo file
	len = LittleLong(clcCut->serverMessageSequence - 1);
	FS_Write(&len, 4, f);
	len = LittleLong(buf.cursize);
	FS_Write(&len, 4, f);

	if (buf.raw) {
		FS_Write(buf.dataRaw->data(), buf.dataRaw->size(), f);
	}
	else {
		FS_Write(buf.data, buf.cursize, f);
	}
}

void demoCutWriteDeltaSnapshot(int firstServerCommand, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw) {
	msg_t			msgImpl, * msg = &msgImpl;
	byte			msgData[MAX_MSGLEN];
	std::vector<byte>			msgDataRaw;
	clSnapshot_t* frame, * oldframe;
	int				lastframe = 0;
	int				snapFlags;
	if (raw) {
		msgDataRaw.clear();
		MSG_InitRaw(msg, &msgDataRaw);
	}
	else {
		MSG_Init(msg, msgData, sizeof(msgData));
	}
	MSG_Bitstream(msg);
	MSG_WriteLong(msg, clcCut->reliableSequence);
	// copy over any commands
	for (int serverCommand = firstServerCommand; serverCommand <= clcCut->serverCommandSequence; serverCommand++) {
		char* command = clcCut->serverCommands[serverCommand & (MAX_RELIABLE_COMMANDS - 1)];
		MSG_WriteByte(msg, specializeGeneralSVCOp(svc_serverCommand_general,demoType));
		MSG_WriteLong(msg, serverCommand/* + serverCommandOffset*/);
		MSG_WriteString(msg, command);
	}
	// this is the snapshot we are creating
	frame = &clCut->snap;
	if (clCut->snap.messageNum > 0 && !forceNonDelta) {
		lastframe = 1;
		oldframe = &clCut->snapshots[(clCut->snap.messageNum - 1) & PACKET_MASK]; // 1 frame previous
		if (!oldframe->valid) {
			// not yet set
			lastframe = 0;
			oldframe = NULL;
		}
	}
	else {
		lastframe = 0;
		oldframe = NULL;
	}
	MSG_WriteByte(msg, specializeGeneralSVCOp(svc_snapshot_general, demoType));
	// send over the current server time so the client can drift
	// its view of time to try to match
	MSG_WriteLong(msg, frame->serverTime);
	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (oldframe) {
		MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, &frame->ps,qfalse, demoType);
		if (demoType == DM_26 || demoType == DM_25) {
			if (frame->ps.m_iVehicleNum) {
				//then write the vehicle's playerstate too
				if (!oldframe->ps.m_iVehicleNum) {
					//if last frame didn't have vehicle, then the old vps isn't gonna delta
					//properly (because our vps on the client could be anything)
					MSG_WriteDeltaPlayerstate(msg, NULL, &frame->vps, qtrue, demoType);
				}
				else {
					MSG_WriteDeltaPlayerstate(msg, &oldframe->vps, &frame->vps, qtrue, demoType);
				}
			}
		}
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps,qfalse, demoType);
		if (demoType == DM_26 || demoType == DM_25) {
			if (frame->ps.m_iVehicleNum) {
				//then write the vehicle's playerstate too
				MSG_WriteDeltaPlayerstate(msg, NULL, &frame->vps, qtrue,demoType);
			}
		}
	}
	// delta encode the entities
	demoCutEmitPacketEntities(oldframe, frame, msg, clCut, demoType);
	MSG_WriteByte(msg, specializeGeneralSVCOp(svc_EOF_general, demoType));
	demoCutWriteDemoMessage(msg, f, clcCut);
}

qboolean demoCutConfigstringModifiedManual(clientActive_t* clCut, int configStringNum, const char* value, demoType_t demoType) { 
	char* old;
	const char* s;
	int			i, index;
	const char* dup;
	gameState_t	oldGs;
	int			len;
	index = configStringNum;

	//int maxAllowedConfigString = demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(demoType);

	if (index < 0 || index >= maxAllowedConfigString) {
		Com_Printf("demoCutConfigstringModifiedManual: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = value;
	old = clCut->gameState.stringData + clCut->gameState.stringOffsets[index];
	if (!strcmp(old, s)) {
		return qtrue; // unchanged
	}
	// build the new gameState_t
	oldGs = clCut->gameState;
	Com_Memset(&clCut->gameState, 0, sizeof(clCut->gameState));
	// leave the first 0 for uninitialized strings
	clCut->gameState.dataCount = 1;
	for (i = 0; i < maxAllowedConfigString; i++) {
		if (i == index) {
			dup = s;
		}
		else {
			dup = oldGs.stringData + oldGs.stringOffsets[i];
		}
		if (!dup[0]) {
			continue; // leave with the default empty string
		}
		len = strlen(dup);
		if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
			Com_Printf("MAX_GAMESTATE_CHARS exceeded on %d:%s", configStringNum, value);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}





static std::map<int, entityState_t> emptyEntities;

void demoCutEmitPacketEntitiesManual(msg_t* msg, clientActive_t* clCut, demoType_t demoType, std::map<int, entityState_t>* entities, std::map<int, entityState_t>* fromEntities) {
	entityState_t* oldent, * newent;
	int oldindex, newindex;
	int oldnum, newnum;
	int from_num_entities;
	// generate the delta update
	//if (!from) {
	if (!fromEntities) {
		from_num_entities = 0;
		fromEntities = &emptyEntities;
	}
	else {
		//from_num_entities = from->numEntities;
		from_num_entities = fromEntities->size();
	}
	newent = NULL;
	oldent = NULL;
	newindex = 0;
	auto newIterator = entities->begin();
	oldindex = 0;
	auto oldIterator = fromEntities->begin();
	while (newindex < entities->size() || oldindex < from_num_entities) {
		if (newindex >= entities->size()) {
			newnum = 9999;
		}
		else {
			//newent = &clCut->parseEntities[(to->parseEntitiesNum + newindex) & (MAX_PARSE_ENTITIES - 1)];
			//newnum = newent->number;
			newnum = newIterator->first;
			newent = &newIterator->second;
		}
		if (oldindex >= from_num_entities) {
			oldnum = 9999;
		}
		else {
			//oldent = &clCut->parseEntities[(from->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			//oldnum = oldent->number;
			oldent = &oldIterator->second;
			oldnum = oldIterator->first;
		}
		if (newnum == oldnum) {
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, demoType);
			oldindex++;
			oldIterator++;
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue,demoType);
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, demoType);
			oldindex++;
			oldIterator++;
			continue;
		}
	}
	MSG_WriteBits(msg, (MAX_GENTITIES - 1), GENTITYNUM_BITS);	// end of packetentities
}

qboolean demoCutInitClearGamestate(clientConnection_t* clcCut, clientActive_t* clCut, int serverCommandSequence, int clientNum, int checksumFeed) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd;
	char* s;
	clcCut->connectPacketCount = 0;
	Com_Memset(clCut, 0, sizeof(*clCut));
	clcCut->serverCommandSequence = serverCommandSequence;
	clCut->gameState.dataCount = 1;
	clcCut->clientNum = clientNum;
	clcCut->checksumFeed = checksumFeed;
	return qtrue;
}


//void demoCutWriteDeltaSnapshotManual(int firstServerCommand, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType,std::map<int,entityState_t>* entities,std::map<int,entityState_t>* fromEntities,playerState_t* fromPS) {
// TODO: Make this work for JKA? No idea. Whatever.
void demoCutWriteDeltaSnapshotManual(std::vector<std::string>* newCommands, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, std::map<int, entityState_t>* entities, std::map<int, entityState_t>* fromEntities, playerState_t* fromPS,qboolean raw) {
	msg_t			msgImpl, * msg = &msgImpl;
	byte			msgData[MAX_MSGLEN];
	std::vector<byte>			msgDataRaw;
	qboolean		doDelta = qfalse;
	clSnapshot_t* frame;// , * oldframe;
	int				lastframe = 0;
	int				snapFlags;
	if (raw) {
		msgDataRaw.clear();
		MSG_InitRaw(msg, &msgDataRaw);
	}else{
		MSG_Init(msg, msgData, sizeof(msgData));
	}
	MSG_Bitstream(msg);
	MSG_WriteLong(msg, clcCut->reliableSequence);
	// copy over any commands
	/*for (int serverCommand = firstServerCommand; serverCommand <= clcCut->serverCommandSequence; serverCommand++) {
		char* command = clcCut->serverCommands[serverCommand & (MAX_RELIABLE_COMMANDS - 1)];
		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, serverCommand)// + serverCommandOffset);
		MSG_WriteString(msg, command);
	}*/
	for (int i = 0; i < newCommands->size(); i++) {
		MSG_WriteByte(msg, specializeGeneralSVCOp(svc_serverCommand_general,demoType));
		MSG_WriteLong(msg, ++clcCut->serverCommandSequence/* + serverCommandOffset*/);
		MSG_WriteString(msg, (*newCommands)[i].c_str());
	}
	// this is the snapshot we are creating
	frame = &clCut->snap;
	if (fromEntities && fromPS && /*clCut->snap.messageNum > 0 &&*/ !forceNonDelta) {
		lastframe = 1;
		//lastframe = clcCut->reliableSequence-1;
		doDelta = qtrue;
		/*oldframe = &clCut->snapshots[(clCut->snap.messageNum - 1) & PACKET_MASK]; // 1 frame previous
		if (!oldframe->valid) {
			// not yet set
			lastframe = 0;
			oldframe = NULL;
		}*/
	}
	else {
		lastframe = 0;
		//oldframe = NULL;
	}
	MSG_WriteByte(msg, specializeGeneralSVCOp(svc_snapshot_general, demoType));
	// send over the current server time so the client can drift
	// its view of time to try to match
	MSG_WriteLong(msg, frame->serverTime);
	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (doDelta) {
		MSG_WriteDeltaPlayerstate(msg, fromPS, &frame->ps,qfalse, demoType);
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps,qfalse,demoType );
	}
	// delta encode the entities
	demoCutEmitPacketEntitiesManual(msg, clCut, demoType, entities, fromEntities);
	MSG_WriteByte(msg, specializeGeneralSVCOp(svc_EOF_general, demoType));
	demoCutWriteDemoMessage(msg, f, clcCut);
}

std::string makeConfigStringCommand(int index, std::string value) {
	std::stringstream ss;
	ss << "cs " << index << " " << value;
	return ss.str();
}

/*
================
G_FindConfigstringIndex

================
*/
int G_FindConfigstringIndex(char* name, int start, int max, qboolean create, clientActive_t* clCut, std::vector<std::string>* commandsToAdd,demoType_t demoType) {
	int		i;
	//char	s[MAX_STRING_CHARS];
	char* s;

	if (!name || !name[0]) {
		return 0;
	}

	for (i = 1; i < max; i++) {
		//trap_GetConfigstring(start + i, s, sizeof(s));
		int stringOffset = clCut->gameState.stringOffsets[start + i];
		s = clCut->gameState.stringData + stringOffset;//sizeof(demo.cut.Cl.gameState.stringData) - stringOffset

		if (!s[0]) {
			break;
		}
		if (!strcmp(s, name)) {
			return i;
		}
	}

	if (!create) {
		return 0;
	}

	if (i == max) {
		Com_Printf("G_FindConfigstringIndex: overflow");
		return 0;
		//G_Error("G_FindConfigstringIndex: overflow");
	}

	//trap_SetConfigstring(start + i, name);
	demoCutConfigstringModifiedManual(clCut, start + i, name,demoType);
	commandsToAdd->push_back(makeConfigStringCommand(start + i, name));

	return i;
}


int G_SoundIndex(char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, CS_SOUNDS, MAX_SOUNDS, qtrue, clCut, commandsToAdd, demoType);
}
int G_ModelIndex(char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, CS_MODELS, MAX_MODELS, qtrue, clCut, commandsToAdd, demoType);
}



void retimeEntity(entityState_t* entity, double newServerTime, double newDemoTime) {
	vec3_t newPos;
	BG_EvaluateTrajectory(&entity->pos, newServerTime, newPos);
	VectorCopy(newPos, entity->pos.trBase);
	BG_EvaluateTrajectoryDelta(&entity->pos, newServerTime, newPos);
	VectorCopy(newPos, entity->pos.trDelta);
	BG_EvaluateTrajectory(&entity->apos, newServerTime, newPos);
	VectorCopy(newPos, entity->apos.trBase);
	BG_EvaluateTrajectoryDelta(&entity->apos, newServerTime, newPos);
	VectorCopy(newPos, entity->apos.trDelta);
	entity->pos.trTime = newDemoTime;
	entity->apos.trTime = newDemoTime;
}





qboolean demoCutGetDemoType(const char* demoFile, char extOutput[7], demoType_t* demoType, qboolean* isCompressed, qboolean* checkFor103) {

	char normalizedExt[7]{};
	strncpy_s(extOutput, 7, (char*)demoFile + strlen(demoFile) - 6, 6);
	strncpy_s(normalizedExt, sizeof(normalizedExt), (char*)demoFile + strlen(demoFile) - 6, 6);
	//strncpy_s(originalExtOutput, 7, (char*)demoFile + strlen(demoFile) - 6, 6);

	normalizedExt[0] = '.';

	char specialTypeChar = normalizedExt[3];
	normalizedExt[3] = '_';

	*isCompressed = qfalse;
	if (specialTypeChar == 'c') {
		*isCompressed = qtrue;
	}

	if (checkFor103) {
		*checkFor103 = qfalse;
	}

	if (!*normalizedExt) { // This cant really happen anyway can it?
		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (checkFor103) {
			*checkFor103 = qtrue;
		}
		return qfalse;
	}
	else if (!_stricmp(normalizedExt, ".dm_15")) {

		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (checkFor103) {
			*checkFor103 = qtrue;
		}
	}
	else if (!_stricmp(normalizedExt, ".dm_16")) {

		*demoType = DM_16;
		//strncpy_s(normalizedExt, 7, ".dm_16", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_25")) {

		*demoType = DM_25;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_26")) {

		*demoType = DM_26;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_66")) {

		*demoType = DM_66;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_67")) {

		*demoType = DM_67;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_68")) {

		*demoType = DM_68;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	} else {

		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (checkFor103) {
			*checkFor103 = qtrue;
		}
		return qfalse;
	}

	return qtrue;
}

























static gameInfo_t gameInfos[] = {
	{ // First is treated as default.
		DM_15,
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_mapchange_general,
			svc_EOF_general
		},
		{
			{},
			{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral)/sizeof(jk2EntityEventsToGeneral[0])}
		},
		{
			entityStateFields15,
			sizeof(entityStateFields15) / sizeof(entityStateFields15[0]),
			playerStateFields15,
			sizeof(playerStateFields15) / sizeof(playerStateFields15[0]),
		}
	},{
		DM_15_1_03,
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_mapchange_general,
			svc_EOF_general
		},
		{
			{},
			{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral) / sizeof(jk2EntityEventsToGeneral[0])}
		},
		{
			entityStateFields,
			sizeof(entityStateFields) / sizeof(entityStateFields[0]),
			playerStateFields,
			sizeof(playerStateFields) / sizeof(playerStateFields[0]),
		}
	},{
		DM_16,
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_mapchange_general,
			svc_EOF_general
		},
		{
			{},
			{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral) / sizeof(jk2EntityEventsToGeneral[0])}
		},
		{
			entityStateFields,
			sizeof(entityStateFields) / sizeof(entityStateFields[0]),
			playerStateFields,
			sizeof(playerStateFields) / sizeof(playerStateFields[0]),
		}
	},{
		DM_25, // Ok this is experimental. Idk if DM_25 is the same as DM_26 in all these aspects... trust this less.
		{
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
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
			{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])}
		},
		{
			entityStateFieldsJKA,
			sizeof(entityStateFieldsJKA) / sizeof(entityStateFieldsJKA[0]),
			playerStateFieldsJKA,
			sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]),
			qtrue // The playerstate stuff is actually more convoluted in jka, need special handling that cant be represented purely in terms of this table
		},
		MAX_CONFIGSTRINGS_JKA,
		{CS_MODELS_JKA,CS_SOUNDS_JKA,CS_PLAYERS_JKA,ET_EVENTS_JKA},
	},{
		DM_26,
		{
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
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
			{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])}
		},
		{
			entityStateFieldsJKA,
			sizeof(entityStateFieldsJKA) / sizeof(entityStateFieldsJKA[0]),
			playerStateFieldsJKA,
			sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]),
			qtrue // The playerstate stuff is actually more convoluted in jka, need special handling that cant be represented purely in terms of this table
		},
		MAX_CONFIGSTRINGS_JKA,
		{CS_MODELS_JKA,CS_SOUNDS_JKA,CS_PLAYERS_JKA,ET_EVENTS_JKA},
	},{
		DM_26_XBOX,
		{
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
			svc_newpeer_general,				//jsw//inform current clients about new player
			svc_removepeer_general,				//jsw//inform current clients about dying player
			svc_xbInfo_general,					//jsw//update client with current server xbOnlineInfo
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
			{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])}
		},
		{},
		MAX_CONFIGSTRINGS_JKA
	},{
		DM_66,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map)/sizeof(q3dm68EventToJk2Map[0])}},
			{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])}
		},
		{
			entityStateFieldsQ3DM68,
			sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),
			playerStateFieldsQ3DM68,
			sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3},
		64
	},{
		DM_67,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map) / sizeof(q3dm68EventToJk2Map[0])}},
			{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])}
		},
		{
			entityStateFieldsQ3DM68,
			sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),
			playerStateFieldsQ3DM68,
			sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3},
		64
	},{
		DM_68,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general
		},
		{
			{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map) / sizeof(q3dm68EventToJk2Map[0])}},
			{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])}
		},
		{
			entityStateFieldsQ3DM68,
			sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),
			playerStateFieldsQ3DM68,
			sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3},
		64
	},
};


// Multi game related
specializedGameMappingsContainer_t specializedMappings[DEMOTYPE_COUNT][DEMOTYPE_COUNT]{};
gameInfo_t* gameInfosMapped[DEMOTYPE_COUNT]{}; // Is auto-filled by democutter tools.
static qboolean gameInfosInitialized = qfalse;

void initializeGameInfos() {

	if (!gameInfosInitialized) {
		constexpr int countGameMappings = sizeof(gameInfos) / sizeof(gameInfo_t);
		for (int i = 0; i < countGameMappings; i++) {
			gameInfosMapped[gameInfos[i].demoType] = &gameInfos[i];
			// Fill reverse svc lookup table
			for (int gameOp = svc_ops_general_count - 1; gameOp >= 0; gameOp--) { // We do this in reverse so that svc_bad always ends up 0 in the reverse lookup. Prolly irrelevant because it's ... not supposed to be really used, but its neater.
				svc_ops_e_general generalOp = gameInfos[i].opsToGeneral[gameOp];
				gameInfos[i].generalToOps[generalOp] = gameOp;
			}
			// Set some defaults.
			if (!gameInfos[i].maxConfigstrings) gameInfos[i].maxConfigstrings = MAX_CONFIGSTRINGS;
			if (!gameInfos[i].constants.cs_models) gameInfos[i].constants.cs_models = CS_MODELS;
			if (!gameInfos[i].constants.cs_sounds) gameInfos[i].constants.cs_sounds = CS_SOUNDS;
			if (!gameInfos[i].constants.cs_players) gameInfos[i].constants.cs_players = CS_PLAYERS;
			if (!gameInfos[i].constants.cs_players) gameInfos[i].constants.et_events = ET_EVENTS;
			if (!gameInfos[i].maxClients) gameInfos[i].maxClients = MAX_CLIENTS;

			// We create the reverse event lookup table unless the programmer has specifically decided to provide one (no idea why anyone would do that but oh well).
			if (!gameInfos[i].mappings.eventMappingsReverse.data) {
				gameInfos[i].mappings.eventMappingsReverse.data = new const int[EV_ENTITY_EVENT_COUNT_GENERAL] {};
				for (int gameEvent = gameInfos[i].mappings.eventMappingsReverse.count - 1; gameEvent >= 0; gameEvent--) { // We do this in reverse so that svc_bad always ends up 0 in the reverse lookup. Prolly irrelevant because it's ... not supposed to be really used, but its neater.
					entity_event_t generalEvent = gameInfos[i].mappings.eventMappings.data[gameEvent];
					((int*)gameInfos[i].mappings.eventMappingsReverse.data)[generalEvent] = gameEvent; // We cast the const int to int. We wanna be able to change it here to give it the correct values. It's just not supposed to be modified elsewhere
				}
			}

			// Check for specialized mappings and point to them.
			for (int s = 0; s < MAX_SPECIALIZED_MAPPINGS; s++) {
				if (gameInfos[i].mappings.specializedEventMappings[s].mapping.data) { // Will be a null ptr if it wasnt initialized/added
					for (int td = 0; td < MAX_MAPPING_TARGET_DEMOTYPES; td++) {
						if (gameInfos[i].mappings.specializedEventMappings[s].targetDemoTypes[td]) {
							specializedMappings[gameInfos[i].demoType][gameInfos[i].mappings.specializedEventMappings[s].targetDemoTypes[td]].eventMapping = gameInfos[i].mappings.specializedEventMappings[s].mapping;
						}
					}
				}
			}
		}
		gameInfosInitialized = qtrue;
	}
}
