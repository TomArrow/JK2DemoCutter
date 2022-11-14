#include "demoCut.h"
#define _USE_MATH_DEFINES
#include <math.h>

// Code is 99%-100% from jomme, from various files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//


static	int			cmd_argc;
static	char* cmd_argv[MAX_STRING_TOKENS];		// points into cmd_tokenized
static	char		cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];	// will have 0 bytes inserted



vec_t VectorLength(const vec3_t v) {
	return (vec_t)sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
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
	static char		string[2][VABUFFERSIZE];	// in case va is called by nested functions
	static int		index = 0;
	char* buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	//vsprintf(buf, format, argptr);
	vsprintf_s(buf, VABUFFERSIZE, format, argptr);
	va_end(argptr);

	return buf;
}

void sanitizeFilename(const char* input, char* output) {

	while (*input) {
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


void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, qboolean snap,qboolean writeCommandTime) {
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

