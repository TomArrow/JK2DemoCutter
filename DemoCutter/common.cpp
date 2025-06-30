#include "demoCut.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <jkaStuff.h>
#include "otherGameStuff.h"
#include "otherGameStuff2.h"
#include "jk2spStuff.h"
#include <stateFields.h>
#include <sstream>
#include "CModel.h"

#include "include/rapidjson/document.h"


constexpr char* postEOFMetadataMarker = "HIDDENMETA";
constexpr char* postEOFUcmdMarker = "HIDDENUCMD";

// Code is 99%-100% from jomme, from various files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//

InternedStringTable internedStrings;


int GlobalDebugOutputFlags = 0;
int64_t GlobalDebugDemoTime = 0;



std::string errorInfo = "";

static	int			cmd_argc;
static	char* cmd_argv[MAX_STRING_TOKENS];		// points into cmd_tokenized
static	char		cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];	// will have 0 bytes inserted


void VectorInverse(vec3_t v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}
vec_t VectorLength(const vec3_t v) {
	return (vec_t)sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
vec_t VectorLength2(const vec2_t v) {
	return (vec_t)sqrtf(v[0] * v[0] + v[1] * v[1]);
}
vec_t VectorLengthSquared(const vec3_t v) {
	return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vec_t Distance(const vec3_t p1, const vec3_t p2) {
	vec3_t	v;

	VectorSubtract(p2, p1, v);
	return VectorLength(v);
}

vec_t DistanceSquared(const vec3_t p1, const vec3_t p2) {
	vec3_t	v;

	VectorSubtract(p2, p1, v);
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
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
vec_t VectorNormalize2(const vec3_t v, vec3_t out) {
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrtf(length);

	if (length)
	{
#ifndef Q3_VM // bk0101022 - FPE related
		//	  assert( ((Q_fabs(v[0])!=0.0f) || (Q_fabs(v[1])!=0.0f) || (Q_fabs(v[2])!=0.0f)) );
#endif
		ilength = 1 / length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
		out[2] = v[2] * ilength;
	}
	else {
#ifndef Q3_VM // bk0101022 - FPE related
		//	  assert( ((Q_fabs(v[0])==0.0f) && (Q_fabs(v[1])==0.0f) && (Q_fabs(v[2])==0.0f)) );
#endif
		VectorClear(out);
	}

	return length;

}

void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross) {
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
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

const char* DPrintFLocation = NULL;

// Just using this for error output now. :P :) XD
void QDECL Com_DPrintf(const char* fmt, ...) {
//#ifdef DEBUG
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	//vsprintf(msg, fmt, argptr);
	vsprintf_s(msg,sizeof(msg), fmt, argptr);
	va_end(argptr);

	if (DPrintFLocation) {
		std::cerr << DPrintFLocation << ":" << msg;
	}
	else {
		std::cerr << msg;
	}
//#endif
}
void QDECL Com_Error(int ignore, const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	//vsprintf(msg, fmt, argptr);
	vsprintf_s(msg,sizeof(msg), fmt, argptr);
	va_end(argptr);

	if (DPrintFLocation) {
		std::cerr << DPrintFLocation << ":" << msg;
	}
	else {
		std::cerr << msg;
	}
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
		Com_DPrintf("Com_sprintf: overflow of %i in %i\n", len, size);
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


qboolean getModelAndSkinFromSkinPath(const char* skinPath, char* outModel, int outModelMaxLen, char* outSkin, int outSkinMaxLen) {
	if (strncmp(skinPath, "models/players", sizeof("models/players") - 1)) {
		return qfalse;
	}
	skinPath += sizeof("models/players") - 1;
	while (*skinPath == '/') {
		skinPath++;
	}
	while (*skinPath && *skinPath != '/' && outModelMaxLen > 1) {
		*outModel = tolower(*skinPath);
		skinPath++;
		outModel++;
		outModelMaxLen--;
	}
	*outModel = 0;
	while (*skinPath == '/') {
		skinPath++;
	}
	if (strncmp(skinPath, "model_", sizeof("model_") - 1)) {
		return qfalse;
	}
	skinPath += sizeof("model_") - 1;
	while (*skinPath && *skinPath != '.' && outSkinMaxLen > 1) {
		*outSkin = tolower(*skinPath);
		skinPath++;
		outSkin++;
		outSkinMaxLen--;
	}
	*outSkin = 0;
	return qtrue;
}

void sanitizeFilename(const char* input, char* output,qboolean allowExtension) {

	char* lastDot = NULL;
	const char* inputStart = input;
	while (*input) {
		if (*input == '.' && input != inputStart) { // Even tho we allow extensions (dots), we don't allow the dot at the start of the filename.
			lastDot = output;
		}
		if ((*input == 32) // Don't allow ! exclamation mark. Linux doesn't like that.
			|| (*input >= 35 && *input < 42)
			|| (*input >= 43 && *input < 46)
			|| (*input >= 48 && *input < 58)
			|| (*input >= 59 && *input < 60)
			|| (*input == 61)
			|| (*input >= 64 && *input < 92)
			|| (*input >= 93 && *input < 96) // Don't allow `. Linux doesn't like that either, at least not in shell scripts.
			|| (*input >= 97 && *input < 124)
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
void Info_RemoveKey(char* s, const char* key, bool isMOHAADemo) {
	char* start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char* o;

	if (strlen(s) >= (isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING)) {
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
Info_ApplyOverrides

Apply values from one info string to another and override the other ones values.
FIXME: overflow check?
===============
*/
int Info_ApplyOverrides(const char* overrides, char* data, int maxLengthOverrides, int maxLengthData) {
	char	pkey[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	//static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
											// work without stomping on each other
	//static	int	valueindex = 0;
	char* o;
	const char* s = overrides;
	int		overridesApplied = 0;

	if (!overrides || !data) {
		return 0;
	}

	//if (strlen(s) >= BIG_INFO_STRING) {
	if (strnlen_s(overrides, maxLengthOverrides) >= BIG_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_ApplyOverrides: oversize overrides");
	}
	if (strnlen_s(data, maxLengthData) >= BIG_INFO_STRING) {
		Com_Error(ERR_DROP, "Info_ApplyOverrides: oversize data");
	}

	//valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1) {
		o = pkey;
		while (*s != '\\') {
			if (!*s)
				return overridesApplied;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;//value[valueindex];

		while (*s != '\\' && *s) {
			*o++ = *s++;
		}
		*o = 0;

		Info_SetValueForKey_Big(data, maxLengthData, pkey, value);
		//if (!_stricmp(key, pkey))
		//	return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return overridesApplied;
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
qboolean Info_SetValueForKey(char* s, int capacity, const char* key, const char* value, bool isMOHAADemo) {
	char	newi[MAX_INFO_STRING_MAX];

	if (strlen(s) >= (isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING)) {
		Com_Error(ERR_DROP, "Info_SetValueForKey: oversize infostring");
	}

	if (strchr(key, '\\') || strchr(value, '\\')) {
		Com_DPrintf("Can't use keys or values with a \\\n");
		return qfalse;
	}

	if (strchr(key, ';') || strchr(value, ';')) {
		Com_DPrintf("Can't use keys or values with a semicolon\n");
		return qfalse;
	}

	if (strchr(key, '\"') || strchr(value, '\"')) {
		Com_DPrintf("Can't use keys or values with a \"\n");
		return qfalse;
	}

	Info_RemoveKey(s, key, isMOHAADemo);

	if (!strlen(value))
		return qfalse;

	Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

	// q3infoboom exploit
	if (strlen(newi) + strlen(s) >= (isMOHAADemo ? MAX_INFO_STRING_MAX : MAX_INFO_STRING)) {
		Com_DPrintf("Info string length exceeded\n");
		return qfalse;
	}

	// TA: lil safety measure for userinfo strings as they dont always start with \ unfortunately
	if (*s != '\\' && *s != '\0'){
		strcat_s(newi, sizeof(newi), "\\");
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
		Com_DPrintf("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr(key, ';') || strchr(value, ';')) {
		Com_DPrintf("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr(key, '\"') || strchr(value, '\"')) {
		Com_DPrintf("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big(s, key);

	if (!strlen(value))
		return;

	Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

	// q3infoboom exploit
	if (strlen(newi) + strlen(s) >= BIG_INFO_STRING) {
		Com_DPrintf("BIG Info string length exceeded\n");
		return;
	}

	//strcat(s, newi);
	strcat_s(s, capacity, newi);
}

enumName_t entityTypesGeneralNames[ET_EVENTS_GENERAL] = {

	ENUMENTRY(ET_GENERAL_GENERAL),
	ENUMENTRY(ET_PLAYER_GENERAL),
	ENUMENTRY(ET_ITEM_GENERAL),
	ENUMENTRY(ET_MISSILE_GENERAL),
	ENUMENTRY(ET_SPECIAL_GENERAL),
	ENUMENTRY(ET_HOLOCRON_GENERAL),
	ENUMENTRY(ET_MOVER_GENERAL),
	ENUMENTRY(ET_BEAM_GENERAL),
	ENUMENTRY(ET_PORTAL_GENERAL),
	ENUMENTRY(ET_SPEAKER_GENERAL),
	ENUMENTRY(ET_PUSH_TRIGGER_GENERAL),
	ENUMENTRY(ET_TELEPORT_TRIGGER_GENERAL),
	ENUMENTRY(ET_INVISIBLE_GENERAL),
	ENUMENTRY(ET_GRAPPLE_GENERAL),
	ENUMENTRY(ET_TEAM_GENERAL),
	ENUMENTRY(ET_BODY_GENERAL),

	// JKA
	ENUMENTRY(ET_NPC_GENERAL),
	ENUMENTRY(ET_TERRAIN_GENERAL),
	ENUMENTRY(ET_FX_GENERAL),

	// JK2SP
	ENUMENTRY(ET_THINKER_GENERAL),
	ENUMENTRY(ET_CLOUD_GENERAL),

	// MOHAA
	ENUMENTRY(ET_MODELANIM_SKEL_GENERAL),
	ENUMENTRY(ET_MODELANIM_GENERAL),
	ENUMENTRY(ET_VEHICLE_GENERAL),
	ENUMENTRY(ET_MULTIBEAM_GENERAL),
	ENUMENTRY(ET_EVENT_ONLY_GENERAL),
	ENUMENTRY(ET_RAIN_GENERAL),
	ENUMENTRY(ET_LEAF_GENERAL),
	ENUMENTRY(ET_DECAL_GENERAL),
	ENUMENTRY(ET_EMITTER_GENERAL),
	ENUMENTRY(ET_ROPE_GENERAL),
	ENUMENTRY(ET_EXEC_COMMANDS_GENERAL),
};



// MOD-weapon mapping array.
int weaponFromMOD_JK2[MOD_MAX_JK2] =
{
	WP_NONE_JK2,				//MOD_UNKNOWN_JK2,
	WP_STUN_BATON_JK2,			//MOD_STUN_BATON_JK2,
	WP_NONE_JK2,				//MOD_MELEE_JK2,
	WP_SABER_JK2,				//MOD_SABER_JK2,
	WP_BRYAR_PISTOL_JK2,		//MOD_BRYAR_PISTOL_JK2,
	WP_BRYAR_PISTOL_JK2,		//MOD_BRYAR_PISTOL_ALT_JK2,
	WP_BLASTER_JK2,				//MOD_BLASTER_JK2,
	WP_DISRUPTOR_JK2,			//MOD_DISRUPTOR_JK2,
	WP_DISRUPTOR_JK2,			//MOD_DISRUPTOR_SPLASH_JK2,
	WP_DISRUPTOR_JK2,			//MOD_DISRUPTOR_SNIPER_JK2,
	WP_BOWCASTER_JK2,			//MOD_BOWCASTER_JK2,
	WP_REPEATER_JK2,			//MOD_REPEATER_JK2,
	WP_REPEATER_JK2,			//MOD_REPEATER_ALT_JK2,
	WP_REPEATER_JK2,			//MOD_REPEATER_ALT_SPLASH_JK2,
	WP_DEMP2_JK2,				//MOD_DEMP2_JK2,
	WP_DEMP2_JK2,				//MOD_DEMP2_ALT_JK2,
	WP_FLECHETTE_JK2,			//MOD_FLECHETTE_JK2,
	WP_FLECHETTE_JK2,			//MOD_FLECHETTE_ALT_SPLASH_JK2,
	WP_ROCKET_LAUNCHER_JK2,		//MOD_ROCKET_JK2,
	WP_ROCKET_LAUNCHER_JK2,		//MOD_ROCKET_SPLASH_JK2,
	WP_ROCKET_LAUNCHER_JK2,		//MOD_ROCKET_HOMING_JK2,
	WP_ROCKET_LAUNCHER_JK2,		//MOD_ROCKET_HOMING_SPLASH_JK2,
	WP_THERMAL_JK2,				//MOD_THERMAL_JK2,
	WP_THERMAL_JK2,				//MOD_THERMAL_SPLASH_JK2,
	WP_TRIP_MINE_JK2,			//MOD_TRIP_MINE_SPLASH_JK2,
	WP_TRIP_MINE_JK2,			//MOD_TIMED_MINE_SPLASH_JK2,
	WP_DET_PACK_JK2,			//MOD_DET_PACK_SPLASH_JK2,
	WP_NONE_JK2,				//MOD_FORCE_DARK_JK2,
	WP_NONE_JK2,				//MOD_SENTRY_JK2,
	WP_NONE_JK2,				//MOD_WATER_JK2,
	WP_NONE_JK2,				//MOD_SLIME_JK2,
	WP_NONE_JK2,				//MOD_LAVA_JK2,
	WP_NONE_JK2,				//MOD_CRUSH_JK2,
	WP_NONE_JK2,				//MOD_TELEFRAG_JK2,
	WP_NONE_JK2,				//MOD_FALLING_JK2,
	WP_NONE_JK2,				//MOD_SUICIDE_JK2,
	WP_NONE_JK2,				//MOD_TARGET_LASER_JK2,
	WP_NONE_JK2,				//MOD_TRIGGER_HURT_JK2,
};

// MOD-weapon mapping array.
int weaponFromMOD_GENERAL[MOD_MAX_GENERAL] =
{
	WP_NONE_GENERAL,				//MOD_UNKNOWN_GENERAL,
	WP_STUN_BATON_GENERAL,			//MOD_STUN_BATON_GENERAL,
	WP_NONE_GENERAL,				//MOD_MELEE_GENERAL,
	WP_SABER_GENERAL,				//MOD_SABER_GENERAL,
	WP_BRYAR_PISTOL_GENERAL,		//MOD_BRYAR_PISTOL_GENERAL,
	WP_BRYAR_PISTOL_GENERAL,		//MOD_BRYAR_PISTOL_ALT_GENERAL,
	WP_BLASTER_GENERAL,				//MOD_BLASTER_GENERAL,
	WP_DISRUPTOR_GENERAL,			//MOD_DISRUPTOR_GENERAL,
	WP_DISRUPTOR_GENERAL,			//MOD_DISRUPTOR_SPLASH_GENERAL,
	WP_DISRUPTOR_GENERAL,			//MOD_DISRUPTOR_SNIPER_GENERAL,
	WP_BOWCASTER_GENERAL,			//MOD_BOWCASTER_GENERAL,
	WP_REPEATER_GENERAL,			//MOD_REPEATER_GENERAL,
	WP_REPEATER_GENERAL,			//MOD_REPEATER_ALT_GENERAL,
	WP_REPEATER_GENERAL,			//MOD_REPEATER_ALT_SPLASH_GENERAL,
	WP_DEMP2_GENERAL,				//MOD_DEMP2_GENERAL,
	WP_DEMP2_GENERAL,				//MOD_DEMP2_ALT_GENERAL,
	WP_FLECHETTE_GENERAL,			//MOD_FLECHETTE_GENERAL,
	WP_FLECHETTE_GENERAL,			//MOD_FLECHETTE_ALT_SPLASH_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,		//MOD_ROCKET_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,		//MOD_ROCKET_SPLASH_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,		//MOD_ROCKET_HOMING_GENERAL,
	WP_ROCKET_LAUNCHER_GENERAL,		//MOD_ROCKET_HOMING_SPLASH_GENERAL,
	WP_THERMAL_GENERAL,				//MOD_THERMAL_GENERAL,
	WP_THERMAL_GENERAL,				//MOD_THERMAL_SPLASH_GENERAL,
	WP_TRIP_MINE_GENERAL,			//MOD_TRIP_MINE_SPLASH_GENERAL,
	WP_TRIP_MINE_GENERAL,			//MOD_TIMED_MINE_SPLASH_GENERAL,
	WP_DET_PACK_GENERAL,			//MOD_DET_PACK_SPLASH_GENERAL,
	WP_NONE_GENERAL,				//MOD_FORCE_DARK_GENERAL,
	WP_NONE_GENERAL,				//MOD_SENTRY_GENERAL,
	WP_NONE_GENERAL,				//MOD_WATER_GENERAL,
	WP_NONE_GENERAL,				//MOD_SLIME_GENERAL,
	WP_NONE_GENERAL,				//MOD_LAVA_GENERAL,
	WP_NONE_GENERAL,				//MOD_CRUSH_GENERAL,
	WP_NONE_GENERAL,				//MOD_TELEFRAG_GENERAL,
	WP_NONE_GENERAL,				//MOD_FALLING_GENERAL,
	WP_NONE_GENERAL,				//MOD_SUICIDE_GENERAL,
	WP_NONE_GENERAL,				//MOD_TARGET_LASER_GENERAL,
	WP_NONE_GENERAL,				//MOD_TRIGGER_HURT_GENERAL,
	// JK3
	WP_NONE_GENERAL,//MOD_TURBLAST_GENERAL,
	WP_NONE_GENERAL,//MOD_VEHICLE_GENERAL,
	WP_CONCUSSION_GENERAL,//MOD_CONC_GENERAL,
	WP_CONCUSSION_GENERAL,//MOD_CONC_ALT_GENERAL,
	WP_NONE_GENERAL,//MOD_COLLISION_GENERAL,
	WP_NONE_GENERAL,//MOD_VEH_EXPLOSION_GENERAL,
	WP_NONE_GENERAL,//MOD_TEAM_CHANGE_GENERAL,

	//q3
	WP_SHOTGUN_GENERAL,//MOD_SHOTGUN_GENERAL,
	WP_GAUNTLET_GENERAL,//MOD_GAUNTLET_GENERAL,
	WP_MACHINEGUN_GENERAL,//MOD_MACHINEGUN_GENERAL,
	WP_GRENADE_LAUNCHER_GENERAL,//MOD_GRENADE_GENERAL,
	WP_GRENADE_LAUNCHER_GENERAL,//MOD_GRENADE_SPLASH_GENERAL,
	WP_PLASMAGUN_GENERAL,//MOD_PLASMA_GENERAL,
	WP_PLASMAGUN_GENERAL,//MOD_PLASMA_SPLASH_GENERAL,
	WP_RAILGUN_GENERAL,//MOD_RAILGUN_GENERAL,
	WP_LIGHTNING_GENERAL,//MOD_LIGHTNING_GENERAL,
	WP_BFG_GENERAL,//MOD_BFG_GENERAL,
	WP_BFG_GENERAL,//MOD_BFG_SPLASH_GENERAL,
	// 
	//#ifdef MISSIONPACK
	WP_NAILGUN_GENERAL,//MOD_NAIL_GENERAL,
	WP_CHAINGUN_GENERAL,//MOD_CHAINGUN_GENERAL,
	WP_PROX_LAUNCHER_GENERAL, //MOD_PROXIMITY_MINE_GENERAL,
	WP_NONE_GENERAL,//MOD_KAMIKAZE_GENERAL,
	WP_NONE_GENERAL,//MOD_JUICED_GENERAL,
	//#endif

	WP_GRAPPLING_HOOK_GENERAL,//MOD_GRAPPLE_GENERAL,
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

//comment from jka: FIXME: add the alternate anims for each style?
// TODO: Fix the animations here? But really doesnt matter does it? we dont use them. But generalize that stuff all someday.
saberMoveData_t	saberMoveData_general[LS_MOVE_MAX_GENERAL] = {//							NB:randomized
	// name			anim(do all styles?)startQ	endQ	setanimflag		blend,	blocking	chain_idle		chain_attack	trailLen
	{"None",		BOTH_STAND1_GENERAL,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_NO,		LS_NONE_GENERAL,		LS_NONE_GENERAL,		0	},	// LS_NONE_GENERAL		= 0,

	// General movements with saber
	{"Ready",		BOTH_STAND2_GENERAL,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_WIDE,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_READY_GENERAL,
	{"Draw",		BOTH_STAND1TO2_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_DRAW_GENERAL,
	{"Putaway",		BOTH_STAND2TO1_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_PUTAWAY_GENERAL,

	// Attacks
	//UL2LR
	{"TL2BR Att",	BOTH_A1_TL_BR_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TL2BR_GENERAL,		LS_R_TL2BR_GENERAL,		200	},	// LS_A_TL2BR_GENERAL
	//SLASH LEFT
	{"L2R Att",		BOTH_A1__L__R_GENERAL,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_L2R_GENERAL,		LS_R_L2R_GENERAL,		200 },	// LS_A_L2R_GENERAL
	//LL2UR
	{"BL2TR Att",	BOTH_A1_BL_TR_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_TIGHT,	LS_R_BL2TR_GENERAL,		LS_R_BL2TR_GENERAL,		200	},	// LS_A_BL2TR_GENERAL
	//LR2UL
	{"BR2TL Att",	BOTH_A1_BR_TL_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_BR2TL_GENERAL,		LS_R_BR2TL_GENERAL,		200	},	// LS_A_BR2TL_GENERAL
	//SLASH RIGHT
	{"R2L Att",		BOTH_A1__R__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_R2L_GENERAL,		LS_R_R2L_GENERAL,		200 },// LS_A_R2L_GENERAL
	//UR2LL
	{"TR2BL Att",	BOTH_A1_TR_BL_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TR2BL_GENERAL,		LS_R_TR2BL_GENERAL,		200	},	// LS_A_TR2BL_GENERAL
	//SLASH DOWN
	{"T2B Att",		BOTH_A1_T__B__GENERAL,		Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_T2B_GENERAL,		LS_R_T2B_GENERAL,		200	},	// LS_A_T2B_GENERAL
	//special attacks
	{"Back Stab",	BOTH_A2_STABBACK1_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACKSTAB_GENERAL
	{"Back Att",	BOTH_ATTACK_BACK_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACK_GENERAL
	{"CR Back Att",	BOTH_CROUCHATTACKBACK1_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACK_CR_GENERAL
	{"RollStab_GENERAL",BOTH_ROLL_STAB_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200},	// LS_ROLL_STAB_GENERAL
	{"Lunge Att",	BOTH_LUNGE2_B__T__GENERAL,	Q_B,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_LUNGE_GENERAL
	{"Jump Att",	BOTH_FORCELEAP2_T__B__GENERAL,Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_JUMP_T__B__GENERAL
	{"Flip Stab",	BOTH_JUMPFLIPSTABDOWN_GENERAL,Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_T___R_GENERAL,	200	},	// LS_A_FLIP_STAB_GENERAL
	{"Flip Slash",	BOTH_JUMPFLIPSLASHDOWN1_GENERAL,Q_L,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__R_T__GENERAL,	200	},	// LS_A_FLIP_SLASH_GENERAL
	{"DualJump Atk",BOTH_JUMPATTACK6_GENERAL,	Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_BL_TR_GENERAL,	200	},	// LS_JUMPATTACK_DUAL_GENERAL

	{"DualJumpAtkL_A",BOTH_ARIAL_LEFT_GENERAL,	Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_A_TL2BR_GENERAL,		200	},	// LS_JUMPATTACK_ARIAL_LEFT_GENERAL
	{"DualJumpAtkR_A",BOTH_ARIAL_RIGHT_GENERAL,	Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_A_TR2BL_GENERAL,		200	},	// LS_JUMPATTACK_ARIAL_RIGHT_GENERAL

	{"DualJumpAtkL_A",BOTH_CARTWHEEL_LEFT_GENERAL,	Q_R,Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_TL_BR_GENERAL,	200	},	// LS_JUMPATTACK_CART_LEFT_GENERAL
	{"DualJumpAtkR_A",BOTH_CARTWHEEL_RIGHT_GENERAL,	Q_R,Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_TR_BL_GENERAL,	200	},	// LS_JUMPATTACK_CART_RIGHT_GENERAL

	{"DualJumpAtkLStaff_GENERAL", BOTH_BUTTERFLY_FL1_GENERAL,Q_R,Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__L__R_GENERAL,	200	},	// LS_JUMPATTACK_STAFF_LEFT_GENERAL
	{"DualJumpAtkRStaff", BOTH_BUTTERFLY_FR1_GENERAL,Q_R,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__R__L_GENERAL,	200	},	// LS_JUMPATTACK_STAFF_RIGHT_GENERAL

	{"ButterflyLeft", BOTH_BUTTERFLY_LEFT_GENERAL,Q_R,Q_L,		AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__L__R_GENERAL,	200	},	// LS_BUTTERFLY_LEFT_GENERAL
	{"ButterflyRight", BOTH_BUTTERFLY_RIGHT_GENERAL,Q_R,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__R__L_GENERAL,	200	},	// LS_BUTTERFLY_RIGHT_GENERAL

	{"BkFlip Atk",	BOTH_JUMPATTACK7_GENERAL,	Q_B,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_T___R_GENERAL,	200	},	// LS_A_BACKFLIP_ATK_GENERAL
	{"DualSpinAtk_GENERAL",BOTH_SPINATTACK6_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_SPINATTACK_DUAL_GENERAL
	{"StfSpinAtk",	BOTH_SPINATTACK7_GENERAL,	Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_SPINATTACK_GENERAL
	{"LngLeapAtk",	BOTH_FORCELONGLEAP_ATTACK_GENERAL,Q_R,Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_LEAP_ATTACK_GENERAL
	{"SwoopAtkR",	BOTH_VS_ATR_S_GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_SWOOP_ATTACK_RIGHT_GENERAL
	{"SwoopAtkL",	BOTH_VS_ATL_S_GENERAL,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_SWOOP_ATTACK_LEFT_GENERAL
	{"TauntaunAtkR",BOTH_VT_ATR_S_GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_TAUNTAUN_ATTACK_RIGHT_GENERAL
	{"TauntaunAtkL",BOTH_VT_ATL_S_GENERAL,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_TAUNTAUN_ATTACK_LEFT_GENERAL
	{"StfKickFwd",	BOTH_A7_KICK_F_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_F_GENERAL
	{"StfKickBack",	BOTH_A7_KICK_B_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_B_GENERAL
	{"StfKickRight",BOTH_A7_KICK_R_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_R_GENERAL
	{"StfKickLeft",	BOTH_A7_KICK_L_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_L_GENERAL
	{"StfKickSpin",	BOTH_A7_KICK_S_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_S_GENERAL
	{"StfKickBkFwd",BOTH_A7_KICK_BF_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_BF_GENERAL
	{"StfKickSplit",BOTH_A7_KICK_RL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_RL_GENERAL
	{"StfKickFwdAir",BOTH_A7_KICK_F_AIR_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_F_AIR_GENERAL
	{"StfKickBackAir",BOTH_A7_KICK_B_AIR_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_B_AIR_GENERAL
	{"StfKickRightAir",BOTH_A7_KICK_R_AIR_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_R_AIR_GENERAL
	{"StfKickLeftAir",BOTH_A7_KICK_L_AIR_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_KICK_L_AIR_GENERAL
	{"StabDown",	BOTH_STABDOWN_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_STABDOWN_GENERAL
	{"StabDownStf",	BOTH_STABDOWN_STAFF_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_STABDOWN_STAFF_GENERAL
	{"StabDownDual",BOTH_STABDOWN_DUAL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		200	},	// LS_STABDOWN_DUAL_GENERAL
	{"dualspinprot_GENERAL",BOTH_A6_SABERPROTECT_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		500	},	// LS_DUAL_SPIN_PROTECT_GENERAL
	{"StfSoulCal",	BOTH_A7_SOULCAL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		500	},	// LS_STAFF_SOULCAL_GENERAL
	{"specialfast",	BOTH_A1_SPECIAL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		2000},	// LS_A1_SPECIAL_GENERAL
	{"specialmed",	BOTH_A2_SPECIAL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		2000},	// LS_A2_SPECIAL_GENERAL
	{"specialstr_GENERAL",	BOTH_A3_SPECIAL_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		2000},	// LS_A3_SPECIAL_GENERAL
	{"upsidedwnatk",BOTH_FLIP_ATTACK7_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200},	// LS_UPSIDE_DOWN_ATTACK_GENERAL
	{"pullatkstab",	BOTH_PULL_IMPALE_STAB_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200},	// LS_PULL_ATTACK_STAB_GENERAL
	{"pullatkswing",BOTH_PULL_IMPALE_SWING_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200},	// LS_PULL_ATTACK_SWING_GENERAL
	{"AloraSpinAtk",BOTH_ALORA_SPIN_SLASH_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_SPINATTACK_ALORA_GENERAL
	{"Dual FB Atk",	BOTH_A6_FB_GENERAL,			Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_DUAL_FB_GENERAL
	{"Dual LR Atk",	BOTH_A6_LR_GENERAL,			Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200 },	// LS_DUAL_LR_GENERAL
	{"StfHiltBash",	BOTH_A7_HILT_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_HILT_BASH_GENERAL

	//starts
	{"TL2BR St",	BOTH_S1_S1_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TL2BR_GENERAL,		LS_A_TL2BR_GENERAL,		200	},	// LS_S_TL2BR_GENERAL
	{"L2R St",		BOTH_S1_S1__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_L2R_GENERAL,		LS_A_L2R_GENERAL,		200	},	// LS_S_L2R_GENERAL
	{"BL2TR St",	BOTH_S1_S1_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BL2TR_GENERAL,		LS_A_BL2TR_GENERAL,		200	},	// LS_S_BL2TR_GENERAL
	{"BR2TL St",	BOTH_S1_S1_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BR2TL_GENERAL,		LS_A_BR2TL_GENERAL,		200	},	// LS_S_BR2TL_GENERAL
	{"R2L St",		BOTH_S1_S1__R_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_R2L_GENERAL,		LS_A_R2L_GENERAL,		200	},	// LS_S_R2L_GENERAL
	{"TR2BL St",	BOTH_S1_S1_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TR2BL_GENERAL,		LS_A_TR2BL_GENERAL,		200	},	// LS_S_TR2BL_GENERAL
	{"T2B St",		BOTH_S1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_T2B_GENERAL,		LS_A_T2B_GENERAL,		200	},	// LS_S_T2B_GENERAL

	//returns
	{"TL2BR Ret",	BOTH_R1_BR_S1_GENERAL,		Q_BR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_TL2BR_GENERAL
	{"L2R Ret",		BOTH_R1__R_S1_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_L2R_GENERAL
	{"BL2TR Ret",	BOTH_R1_TR_S1_GENERAL,		Q_TR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_BL2TR_GENERAL
	{"BR2TL Ret",	BOTH_R1_TL_S1_GENERAL,		Q_TL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_BR2TL_GENERAL
	{"R2L Ret",		BOTH_R1__L_S1_GENERAL,		Q_L,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_R2L_GENERAL
	{"TR2BL Ret",	BOTH_R1_BL_S1_GENERAL,		Q_BL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_TR2BL_GENERAL
	{"T2B Ret",		BOTH_R1_B__S1_GENERAL,		Q_B,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_T2B_GENERAL

	//Transitions
	{"BR2R Trans",	BOTH_T1_BR__R_GENERAL,		Q_BR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc bottom right to right
	{"BR2TR Trans",	BOTH_T1_BR_TR_GENERAL,		Q_BR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR_GENERAL)
	{"BR2T Trans",	BOTH_T1_BR_T__GENERAL,		Q_BR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR_GENERAL)
	{"BR2TL Trans",	BOTH_T1_BR_TL_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast weak spin bottom right to top left
	{"BR2L Trans",	BOTH_T1_BR__L_GENERAL,		Q_BR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast weak spin bottom right to left
	{"BR2BL Trans",	BOTH_T1_BR_BL_GENERAL,		Q_BR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin bottom right to bottom left
	{"R2BR Trans",	BOTH_T1__R_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R_GENERAL)
	{"R2TR Trans",	BOTH_T1__R_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc right to top right
	{"R2T Trans",	BOTH_T1__R_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast ar right to top				(use: BOTH_T1_T___R_GENERAL)
	{"R2TL Trans",	BOTH_T1__R_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc right to top left
	{"R2L Trans",	BOTH_T1__R__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast weak spin right to left
	{"R2BL Trans",	BOTH_T1__R_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin right to bottom left
	{"TR2BR Trans",	BOTH_T1_TR_BR_GENERAL,		Q_TR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc top right to bottom right
	{"TR2R Trans",	BOTH_T1_TR__R_GENERAL,		Q_TR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top right to right			(use: BOTH_T1__R_TR_GENERAL)
	{"TR2T Trans",	BOTH_T1_TR_T__GENERAL,		Q_TR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc top right to top				(use: BOTH_T1_T__TR_GENERAL)
	{"TR2TL Trans",	BOTH_T1_TR_TL_GENERAL,		Q_TR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc top right to top left
	{"TR2L Trans",	BOTH_T1_TR__L_GENERAL,		Q_TR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top right to left
	{"TR2BL Trans",	BOTH_T1_TR_BL_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin top right to bottom left
	{"T2BR Trans",	BOTH_T1_T__BR_GENERAL,		Q_T,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc top to bottom right
	{"T2R Trans",	BOTH_T1_T___R_GENERAL,		Q_T,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top to right
	{"T2TR Trans",	BOTH_T1_T__TR_GENERAL,		Q_T,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc top to top right
	{"T2TL Trans",	BOTH_T1_T__TL_GENERAL,		Q_T,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc top to top left
	{"T2L Trans",	BOTH_T1_T___L_GENERAL,		Q_T,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top to left
	{"T2BL Trans",	BOTH_T1_T__BL_GENERAL,		Q_T,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc top to bottom left
	{"TL2BR Trans",	BOTH_T1_TL_BR_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin top left to bottom right
	{"TL2R Trans",	BOTH_T1_TL__R_GENERAL,		Q_TL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top left to right			(use: BOTH_T1__R_TL_GENERAL)
	{"TL2TR Trans",	BOTH_T1_TL_TR_GENERAL,		Q_TL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL_GENERAL)
	{"TL2T Trans",	BOTH_T1_TL_T__GENERAL,		Q_TL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc top left to top				(use: BOTH_T1_T__TL_GENERAL)
	{"TL2L Trans",	BOTH_T1_TL__L_GENERAL,		Q_TL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top left to left				(use: BOTH_T1__L_TL_GENERAL)
	{"TL2BL Trans",	BOTH_T1_TL_BL_GENERAL,		Q_TL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc top left to bottom left
	{"L2BR Trans",	BOTH_T1__L_BR_GENERAL,		Q_L,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin left to bottom right
	{"L2R Trans",	BOTH_T1__L__R_GENERAL,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast weak spin left to right
	{"L2TR Trans",	BOTH_T1__L_TR_GENERAL,		Q_L,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc left to top right			(use: BOTH_T1_TR__L_GENERAL)
	{"L2T Trans",	BOTH_T1__L_T__GENERAL,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc left to top				(use: BOTH_T1_T___L_GENERAL)
	{"L2TL Trans",	BOTH_T1__L_TL_GENERAL,		Q_L,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc left to top left
	{"L2BL Trans",	BOTH_T1__L_BL_GENERAL,		Q_L,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L_GENERAL)
	{"BL2BR Trans",	BOTH_T1_BL_BR_GENERAL,		Q_BL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin bottom left to bottom right
	{"BL2R Trans",	BOTH_T1_BL__R_GENERAL,		Q_BL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast weak spin bottom left to right
	{"BL2TR Trans",	BOTH_T1_BL_TR_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast weak spin bottom left to top right
	{"BL2T Trans",	BOTH_T1_BL_T__GENERAL,		Q_BL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL_GENERAL)
	{"BL2TL Trans",	BOTH_T1_BL_TL_GENERAL,		Q_BL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL_GENERAL)
	{"BL2L Trans",	BOTH_T1_BL__L_GENERAL,		Q_BL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc bottom left to left

	//Bounces
	{"Bounce BR",	BOTH_B1_BR____GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_T1_BR_TR_GENERAL,	150	},
	{"Bounce R",	BOTH_B1__R____GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_T1__R__L_GENERAL,	150	},
	{"Bounce TR",	BOTH_B1_TR____GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_TR_TL_GENERAL,	150	},
	{"Bounce T",	BOTH_B1_T_____GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},
	{"Bounce TL",	BOTH_B1_TL____GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_T1_TL_TR_GENERAL,	150	},
	{"Bounce L",	BOTH_B1__L____GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_T1__L__R_GENERAL,	150	},
	{"Bounce BL",	BOTH_B1_BL____GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_T1_BL_TR_GENERAL,	150	},

	//Deflected attacks (like bounces, but slide off enemy saber, not straight back)
	{"Deflect BR",	BOTH_D1_BR____GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_T1_BR_TR_GENERAL,	150	},
	{"Deflect R",	BOTH_D1__R____GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_T1__R__L_GENERAL,	150	},
	{"Deflect TR",	BOTH_D1_TR____GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_TR_TL_GENERAL,	150	},
	{"Deflect T",	BOTH_B1_T_____GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},
	{"Deflect TL",	BOTH_D1_TL____GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_T1_TL_TR_GENERAL,	150	},
	{"Deflect L",	BOTH_D1__L____GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_T1__L__R_GENERAL,	150	},
	{"Deflect BL",	BOTH_D1_BL____GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_T1_BL_TR_GENERAL,	150	},
	{"Deflect B",	BOTH_D1_B_____GENERAL,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},

	//Reflected attacks
	{"Reflected BR",BOTH_V1_BR_S1_GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_BR_GENERAL
	{"Reflected R",	BOTH_V1__R_S1_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1__R_GENERAL
	{"Reflected TR",BOTH_V1_TR_S1_GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_TR_GENERAL
	{"Reflected T",	BOTH_V1_T__S1_GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_T__GENERAL
	{"Reflected TL",BOTH_V1_TL_S1_GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_TL_GENERAL
	{"Reflected L",	BOTH_V1__L_S1_GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1__L_GENERAL
	{"Reflected BL",BOTH_V1_BL_S1_GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_BL_GENERAL
	{"Reflected B",	BOTH_V1_B__S1_GENERAL,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_B__GENERAL

	// Broken parries
	{"BParry Top",	BOTH_H1_S1_T__GENERAL,		Q_T,	Q_B,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
	{"BParry UR",	BOTH_H1_S1_TR_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
	{"BParry UL",	BOTH_H1_S1_TL_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
	{"BParry LR",	BOTH_H1_S1_BR_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	{"BParry Bot",	BOTH_H1_S1_B__GENERAL,		Q_B,	Q_T,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LR_GENERAL
	{"BParry LL",	BOTH_H1_S1_BL_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
	//{"BParry LR",	BOTH_H1_S1_BL_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	//{"BParry Bot",	BOTH_H1_S1_B__GENERAL,		Q_B,	Q_T,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
	//{"BParry LL",	BOTH_H1_S1_BR_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LL_GENERAL

	// Knockaways
	{"Knock Top",	BOTH_K1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_T1_T__BR_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
	{"Knock UR",	BOTH_K1_S1_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_T1_TR__R_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
	{"Knock UL",	BOTH_K1_S1_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_T1_TL__L_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
	{"Knock LR",	BOTH_K1_S1_BR_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_T1_BL_TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	{"Knock LL",	BOTH_K1_S1_BL_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_T1_BR_TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
	//{"Knock LR",	BOTH_K1_S1_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_T1_BL_TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	//{"Knock LL",	BOTH_K1_S1_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_T1_BR_TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL

	// Parry
	{"Parry Top",	BOTH_P1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
	{"Parry UR",	BOTH_P1_S1_TR_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
	{"Parry UL",	BOTH_P1_S1_TL_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
	{"Parry LR",	BOTH_P1_S1_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	{"Parry LL",	BOTH_P1_S1_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
	//{"Parry LR",	BOTH_P1_S1_BL_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
	//{"Parry LL",	BOTH_P1_S1_BR_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL

	// Reflecting a missile
	{"Reflect Top",	BOTH_P1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		300	},	// LS_PARRY_UP_GENERAL,
	{"Reflect UR",	BOTH_P1_S1_TL_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		300	},	// LS_PARRY_UR_GENERAL,
	{"Reflect UL",	BOTH_P1_S1_TR_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		300	},	// LS_PARRY_UL_GENERAL,
	{"Reflect LR",	BOTH_P1_S1_BR_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		300	},	// LS_PARRY_LR_GENERAL
	{"Reflect LL",	BOTH_P1_S1_BL_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		300	},	// LS_PA_GENERALRRY_LL,
};


saberMoveData_jk2_t	saberMoveData_jk2[LS_MOVE_MAX_JK2] = {//							NB:randomized
	// name			anim				startQ	endQ	setanimflag		blend,	blocking	chain_idle		chain_attack	trailLen
	{"None",		BOTH_STAND1_DM16,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_NO,		LS_NONE_JK2,		LS_NONE_JK2,		0	},	// LS_NONE_JK2		= 0,

	// General movements with saber
	{"Ready",		BOTH_STAND2_DM16,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_WIDE,	LS_READY_JK2,		LS_S_R2L_JK2,		0	},	// LS_READY_JK2,
	{"Draw",		BOTH_STAND1TO2_DM16,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_JK2,		LS_S_R2L_JK2,		0	},	// LS_DRAW_JK2,
	{"Putaway",		BOTH_STAND2TO1_DM16,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_JK2,		LS_S_R2L_JK2,		0	},	// LS_PUTAWAY_JK2,

	// Attacks
	//UL2LR
	{"TL2BR Att",	BOTH_A1_TL_BR_DM16,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TL2BR_JK2,		LS_R_TL2BR_JK2,		200	},	// LS_A_TL2BR_JK2
	//SLASH LEFT
	{"L2R Att",		BOTH_A1__L__R_DM16,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_L2R_JK2,		LS_R_L2R_JK2,		200 },	// LS_A_L2R_JK2
	//LL2UR
	{"BL2TR Att",	BOTH_A1_BL_TR_DM16,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_TIGHT,	LS_R_BL2TR_JK2,		LS_R_BL2TR_JK2,		200	},	// LS_A_BL2TR_JK2
	//LR2UL
	{"BR2TL Att",	BOTH_A1_BR_TL_DM16,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_BR2TL_JK2,		LS_R_BR2TL_JK2,		200	},	// LS_A_BR2TL_JK2
	//SLASH RIGHT
	{"R2L Att",		BOTH_A1__R__L_DM16,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_R2L_JK2,		LS_R_R2L_JK2,		200 },// LS_A_R2L_JK2
	//UR2LL
	{"TR2BL Att",	BOTH_A1_TR_BL_DM16,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TR2BL_JK2,		LS_R_TR2BL_JK2,		200	},	// LS_A_TR2BL_JK2
	//SLASH DOWN
	{"T2B Att",		BOTH_A1_T__B__DM16,		Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_T2B_JK2,		LS_R_T2B_JK2,		200	},	// LS_A_T2B_JK2
	//special attacks
	{"Back Stab",	BOTH_A2_STABBACK1_DM16,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_A_BACKSTAB_JK2
	{"Back Att",	BOTH_ATTACK_BACK_DM16,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_A_BACK_JK2
	{"CR Back Att",	BOTH_CROUCHATTACKBACK1_DM16,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_A_BACK_CR_JK2
	{"Lunge Att",	BOTH_LUNGE2_B__T__DM16,	Q_B,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_A_LUNGE_JK2
	{"Jump Att",	BOTH_FORCELEAP2_T__B__DM16,Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_A_JUMP_T__B__JK2
	{"Flip Stab",	BOTH_JUMPFLIPSTABDOWN_DM16,Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_T1_T___R_JK2,	200	},	// LS_A_FLIP_STAB_JK2
	{"Flip Slash",	BOTH_JUMPFLIPSLASHDOWN1_DM16,Q_L,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_T1__R_T__JK2,	200	},	// LS_A_FLIP_SLASH_JK2

	//starts
	{"TL2BR St",	BOTH_S1_S1_TL_DM16,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TL2BR_JK2,		LS_A_TL2BR_JK2,		200	},	// LS_S_TL2BR_JK2
	{"L2R St",		BOTH_S1_S1__L_DM16,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_L2R_JK2,		LS_A_L2R_JK2,		200	},	// LS_S_L2R_JK2
	{"BL2TR St",	BOTH_S1_S1_BL_DM16,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BL2TR_JK2,		LS_A_BL2TR_JK2,		200	},	// LS_S_BL2TR_JK2
	{"BR2TL St",	BOTH_S1_S1_BR_DM16,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BR2TL_JK2,		LS_A_BR2TL_JK2,		200	},	// LS_S_BR2TL_JK2
	{"R2L St",		BOTH_S1_S1__R_DM16,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_R2L_JK2,		LS_A_R2L_JK2,		200	},	// LS_S_R2L_JK2
	{"TR2BL St",	BOTH_S1_S1_TR_DM16,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TR2BL_JK2,		LS_A_TR2BL_JK2,		200	},	// LS_S_TR2BL_JK2
	{"T2B St",		BOTH_S1_S1_T__DM16,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_T2B_JK2,		LS_A_T2B_JK2,		200	},	// LS_S_T2B_JK2

	//returns
	{"TL2BR Ret",	BOTH_R1_BR_S1_DM16,		Q_BR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_TL2BR_JK2
	{"L2R Ret",		BOTH_R1__R_S1_DM16,		Q_R,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_L2R_JK2
	{"BL2TR Ret",	BOTH_R1_TR_S1_DM16,		Q_TR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_BL2TR_JK2
	{"BR2TL Ret",	BOTH_R1_TL_S1_DM16,		Q_TL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_BR2TL_JK2
	{"R2L Ret",		BOTH_R1__L_S1_DM16,		Q_L,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_R2L_JK2
	{"TR2BL Ret",	BOTH_R1_BL_S1_DM16,		Q_BL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_TR2BL_JK2
	{"T2B Ret",		BOTH_R1_B__S1_DM16,		Q_B,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_JK2,		LS_READY_JK2,		200	},	// LS_R_T2B_JK2

	//Transitions
	{"BR2R Trans",	BOTH_T1_BR__R_DM16,		Q_BR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast arc bottom right to right
	{"BR2TR Trans",	BOTH_T1_BR_TR_DM16,		Q_BR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR_DM16)
	{"BR2T Trans",	BOTH_T1_BR_T__DM16,		Q_BR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR_DM16)
	{"BR2TL Trans",	BOTH_T1_BR_TL_DM16,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast weak spin bottom right to top left
	{"BR2L Trans",	BOTH_T1_BR__L_DM16,		Q_BR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast weak spin bottom right to left
	{"BR2BL Trans",	BOTH_T1_BR_BL_DM16,		Q_BR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast weak spin bottom right to bottom left
	{"R2BR Trans",	BOTH_T1__R_BR_DM16,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R_DM16)
	{"R2TR Trans",	BOTH_T1__R_TR_DM16,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast arc right to top right
	{"R2T Trans",	BOTH_T1__R_T__DM16,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast ar right to top				(use: BOTH_T1_T___R_DM16)
	{"R2TL Trans",	BOTH_T1__R_TL_DM16,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast arc right to top left
	{"R2L Trans",	BOTH_T1__R__L_DM16,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast weak spin right to left
	{"R2BL Trans",	BOTH_T1__R_BL_DM16,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast weak spin right to bottom left
	{"TR2BR Trans",	BOTH_T1_TR_BR_DM16,		Q_TR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast arc top right to bottom right
	{"TR2R Trans",	BOTH_T1_TR__R_DM16,		Q_TR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast arc top right to right			(use: BOTH_T1__R_TR_DM16)
	{"TR2T Trans",	BOTH_T1_TR_T__DM16,		Q_TR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast arc top right to top				(use: BOTH_T1_T__TR_DM16)
	{"TR2TL Trans",	BOTH_T1_TR_TL_DM16,		Q_TR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast arc top right to top left
	{"TR2L Trans",	BOTH_T1_TR__L_DM16,		Q_TR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast arc top right to left
	{"TR2BL Trans",	BOTH_T1_TR_BL_DM16,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast weak spin top right to bottom left
	{"T2BR Trans",	BOTH_T1_T__BR_DM16,		Q_T,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast arc top to bottom right
	{"T2R Trans",	BOTH_T1_T___R_DM16,		Q_T,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast arc top to right
	{"T2TR Trans",	BOTH_T1_T__TR_DM16,		Q_T,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast arc top to top right
	{"T2TL Trans",	BOTH_T1_T__TL_DM16,		Q_T,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast arc top to top left
	{"T2L Trans",	BOTH_T1_T___L_DM16,		Q_T,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast arc top to left
	{"T2BL Trans",	BOTH_T1_T__BL_DM16,		Q_T,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast arc top to bottom left
	{"TL2BR Trans",	BOTH_T1_TL_BR_DM16,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast weak spin top left to bottom right
	{"TL2R Trans",	BOTH_T1_TL__R_DM16,		Q_TL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast arc top left to right			(use: BOTH_T1__R_TL_DM16)
	{"TL2TR Trans",	BOTH_T1_TL_TR_DM16,		Q_TL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL_DM16)
	{"TL2T Trans",	BOTH_T1_TL_T__DM16,		Q_TL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast arc top left to top				(use: BOTH_T1_T__TL_DM16)
	{"TL2L Trans",	BOTH_T1_TL__L_DM16,		Q_TL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast arc top left to left				(use: BOTH_T1__L_TL_DM16)
	{"TL2BL Trans",	BOTH_T1_TL_BL_DM16,		Q_TL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast arc top left to bottom left
	{"L2BR Trans",	BOTH_T1__L_BR_DM16,		Q_L,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast weak spin left to bottom right
	{"L2R Trans",	BOTH_T1__L__R_DM16,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast weak spin left to right
	{"L2TR Trans",	BOTH_T1__L_TR_DM16,		Q_L,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast arc left to top right			(use: BOTH_T1_TR__L_DM16)
	{"L2T Trans",	BOTH_T1__L_T__DM16,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast arc left to top				(use: BOTH_T1_T___L_DM16)
	{"L2TL Trans",	BOTH_T1__L_TL_DM16,		Q_L,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast arc left to top left
	{"L2BL Trans",	BOTH_T1__L_BL_DM16,		Q_L,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L_DM16)
	{"BL2BR Trans",	BOTH_T1_BL_BR_DM16,		Q_BL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	//# Fast weak spin bottom left to bottom right
	{"BL2R Trans",	BOTH_T1_BL__R_DM16,		Q_BL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_A_R2L_JK2,		150	},	//# Fast weak spin bottom left to right
	{"BL2TR Trans",	BOTH_T1_BL_TR_DM16,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	//# Fast weak spin bottom left to top right
	{"BL2T Trans",	BOTH_T1_BL_T__DM16,		Q_BL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL_DM16)
	{"BL2TL Trans",	BOTH_T1_BL_TL_DM16,		Q_BL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL_DM16)
	{"BL2L Trans",	BOTH_T1_BL__L_DM16,		Q_BL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_A_L2R_JK2,		150	},	//# Fast arc bottom left to left

	//Bounces
	{"Bounce BR",	BOTH_B1_BR____DM16,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_T1_BR_TR_JK2,	150	},
	{"Bounce R",	BOTH_B1__R____DM16,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_T1__R__L_JK2,	150	},
	{"Bounce TR",	BOTH_B1_TR____DM16,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_T1_TR_TL_JK2,	150	},
	{"Bounce T",	BOTH_B1_T_____DM16,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_T1_T__BL_JK2,	150	},
	{"Bounce TL",	BOTH_B1_TL____DM16,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_T1_TL_TR_JK2,	150	},
	{"Bounce L",	BOTH_B1__L____DM16,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_T1__L__R_JK2,	150	},
	{"Bounce BL",	BOTH_B1_BL____DM16,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_T1_BL_TR_JK2,	150	},

	//Deflected attacks (like bounces, but slide off enemy saber, not straight back)
	{"Deflect BR",	BOTH_D1_BR____DM16,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_JK2,		LS_T1_BR_TR_JK2,	150	},
	{"Deflect R",	BOTH_D1__R____DM16,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_JK2,		LS_T1__R__L_JK2,	150	},
	{"Deflect TR",	BOTH_D1_TR____DM16,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_T1_TR_TL_JK2,	150	},
	{"Deflect T",	BOTH_B1_T_____DM16,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_T1_T__BL_JK2,	150	},
	{"Deflect TL",	BOTH_D1_TL____DM16,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_JK2,		LS_T1_TL_TR_JK2,	150	},
	{"Deflect L",	BOTH_D1__L____DM16,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_JK2,		LS_T1__L__R_JK2,	150	},
	{"Deflect BL",	BOTH_D1_BL____DM16,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_JK2,		LS_T1_BL_TR_JK2,	150	},
	{"Deflect B",	BOTH_D1_B_____DM16,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_JK2,		LS_T1_T__BL_JK2,	150	},

	//Reflected attacks
	{"Reflected BR",BOTH_V1_BR_S1_DM16,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_BR_JK2
	{"Reflected R",	BOTH_V1__R_S1_DM16,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1__R_JK2
	{"Reflected TR",BOTH_V1_TR_S1_DM16,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_TR_JK2
	{"Reflected T",	BOTH_V1_T__S1_DM16,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_T__JK2
	{"Reflected TL",BOTH_V1_TL_S1_DM16,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_TL_JK2
	{"Reflected L",	BOTH_V1__L_S1_DM16,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1__L_JK2
	{"Reflected BL",BOTH_V1_BL_S1_DM16,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_BL_JK2
	{"Reflected B",	BOTH_V1_B__S1_DM16,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_JK2,		LS_READY_JK2,	150	},//	LS_V1_B__JK2

	// Broken parries
	{"BParry Top",	BOTH_H1_S1_T__DM16,		Q_T,	Q_B,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_UP_JK2,
	{"BParry UR",	BOTH_H1_S1_TR_DM16,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_UR_JK2,
	{"BParry UL",	BOTH_H1_S1_TL_DM16,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_UL_JK2,
	{"BParry LR",	BOTH_H1_S1_BL_DM16,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_LR_JK2,
	{"BParry Bot",	BOTH_H1_S1_B__DM16,		Q_B,	Q_T,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_LL_JK2
	{"BParry LL",	BOTH_H1_S1_BR_DM16,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_JK2,		LS_READY_JK2,		150	},	// LS_PARRY_LL_JK2

	// Knockaways
	{"Knock Top",	BOTH_K1_S1_T__DM16,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_T1_T__BR_JK2,		150	},	// LS_PARRY_UP_JK2,
	{"Knock UR",	BOTH_K1_S1_TR_DM16,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_T1_TR__R_JK2,		150	},	// LS_PARRY_UR_JK2,
	{"Knock UL",	BOTH_K1_S1_TL_DM16,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_JK2,		LS_T1_TL__L_JK2,		150	},	// LS_PARRY_UL_JK2,
	{"Knock LR",	BOTH_K1_S1_BL_DM16,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_JK2,		LS_T1_BL_TL_JK2,		150	},	// LS_PARRY_LR_JK2,
	{"Knock LL",	BOTH_K1_S1_BR_DM16,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_JK2,		LS_T1_BR_TR_JK2,		150	},	// LS_PARRY_LL_JK2

	// Parry
	{"Parry Top",	BOTH_P1_S1_T__DM16,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		150	},	// LS_PARRY_UP_JK2,
	{"Parry UR",	BOTH_P1_S1_TR_DM16,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		150	},	// LS_PARRY_UR_JK2,
	{"Parry UL",	BOTH_P1_S1_TL_DM16,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		150	},	// LS_PARRY_UL_JK2,
	{"Parry LR",	BOTH_P1_S1_BL_DM16,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		150	},	// LS_PARRY_LR_JK2,
	{"Parry LL",	BOTH_P1_S1_BR_DM16,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		150	},	// LS_PARRY_LL_JK2

	// Reflecting a missile
	{"Reflect Top",	BOTH_P1_S1_T__DM16,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_A_T2B_JK2,		300	},	// LS_PARRY_UP_JK2,
	{"Reflect UR",	BOTH_P1_S1_TL_DM16,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_JK2,		LS_A_TL2BR_JK2,		300	},	// LS_PARRY_UR_JK2,
	{"Reflect UL",	BOTH_P1_S1_TR_DM16,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_JK2,		LS_A_TR2BL_JK2,		300	},	// LS_PARRY_UL_JK2,
	{"Reflect LR",	BOTH_P1_S1_BR_DM16,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_JK2,		LS_A_BL2TR_JK2,		300	},	// LS_PARRY_LR_JK2
	{"Reflect LL",	BOTH_P1_S1_BL_DM16,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_JK2,		LS_A_BR2TL_JK2,		300	},	// LS_PARRY_LL_JK2,
};

ankerl::unordered_dense::map <int, std::string, ankerl::unordered_dense::hash<int>>  saberMoveNames_general
{
		// Invalid, or saber not armed
	  std::make_pair(LS_INVALID_GENERAL, "_INVALID"),
	  std::make_pair(LS_NONE_GENERAL, "_WEIRD"),

	  // General movements with saber
	  std::make_pair(LS_READY_GENERAL, "_IDLE"),
	  std::make_pair(LS_DRAW_GENERAL, "_DRAW"),
	  std::make_pair(LS_PUTAWAY_GENERAL, "_PUTAWAY"),

	  // Attacks
	  std::make_pair(LS_A_TL2BR_GENERAL, ""),//4
	  std::make_pair(LS_A_L2R_GENERAL, ""),
	  std::make_pair(LS_A_BL2TR_GENERAL, ""),
	  std::make_pair(LS_A_BR2TL_GENERAL, ""),
	  std::make_pair(LS_A_R2L_GENERAL, ""),
	  std::make_pair(LS_A_TR2BL_GENERAL, ""),
	  std::make_pair(LS_A_T2B_GENERAL, ""),
	  std::make_pair(LS_A_BACKSTAB_GENERAL, "_BLUBS"),
	  std::make_pair(LS_A_BACK_GENERAL, "_BS"),
	  std::make_pair(LS_A_BACK_CR_GENERAL, "_DBS"),

	  std::make_pair(LS_ROLL_STAB_GENERAL, "_ROLLSTAB"), // JKA

	  std::make_pair(LS_A_LUNGE_GENERAL, "_UPCUT"),
	  std::make_pair(LS_A_JUMP_T__B__GENERAL, "_DFA"),
	  std::make_pair(LS_A_FLIP_STAB_GENERAL, "_YDFA"),
	  std::make_pair(LS_A_FLIP_SLASH_GENERAL, "_YDFA"),

	  // JKA
		std::make_pair(LS_JUMPATTACK_DUAL_GENERAL,"_BUTTERFLYDUAL"), // Flip forward attack
		std::make_pair(LS_JUMPATTACK_ARIAL_LEFT_GENERAL,"_CARTWHEEL"),
		std::make_pair(LS_JUMPATTACK_ARIAL_RIGHT_GENERAL,"_CARTWHEEL"),
		std::make_pair(LS_JUMPATTACK_CART_LEFT_GENERAL,"_CARTWHEEL"),
		std::make_pair(LS_JUMPATTACK_CART_RIGHT_GENERAL,"_CARTWHEEL"),
		std::make_pair(LS_JUMPATTACK_STAFF_LEFT_GENERAL,"_BUTTERFLYSTAFF"), // Official butterfly but sabermoveData calls it dual jump attack staff(?!)
		std::make_pair(LS_JUMPATTACK_STAFF_RIGHT_GENERAL,"_BUTTERFLYSTAFF"),
		std::make_pair(LS_BUTTERFLY_LEFT_GENERAL,"_BUTTERFLYSTAFF2"), // Not the official butterfly but actually named butterfly.. wtf
		std::make_pair(LS_BUTTERFLY_RIGHT_GENERAL,"_BUTTERFLYSTAFF2"),
		std::make_pair(LS_A_BACKFLIP_ATK_GENERAL,"_BFATK"),
		std::make_pair(LS_SPINATTACK_DUAL_GENERAL,"_KATADUAL"), // Dual spin attack
		std::make_pair(LS_SPINATTACK_GENERAL,"_KATASTAFF2"), // Saber staff twirl
		std::make_pair(LS_LEAP_ATTACK_GENERAL,"_LONGLEAP"), // idk wtf this is
		std::make_pair(LS_SWOOP_ATTACK_RIGHT_GENERAL,"_SWOOP"), // Idk if this is an actual attack. The animation is a guy sitting and swooping.. ?!?
		std::make_pair(LS_SWOOP_ATTACK_LEFT_GENERAL,"_SWOOP"),	// Ooh. It might be if sitting on an animal or in a vehicle otherwise? Oh are they called "swoop bikes" ?
		std::make_pair(LS_TAUNTAUN_ATTACK_RIGHT_GENERAL,"_TAUNTAUN"), // thes are also sitting... hmm. sitting on a tauntaun? 
		std::make_pair(LS_TAUNTAUN_ATTACK_LEFT_GENERAL,"_TAUNTAUN"),
		std::make_pair(LS_KICK_F_GENERAL,"_KICKFRONT"),
		std::make_pair(LS_KICK_B_GENERAL,"_KICKBACK"),
		std::make_pair(LS_KICK_R_GENERAL,"_KICKSIDE"), // what difference does it make...
		std::make_pair(LS_KICK_L_GENERAL,"_KICKSIDE"),
		std::make_pair(LS_KICK_S_GENERAL,"_KICKSPIN"), // I havent investigated this too deeply. Idk how to do it
		std::make_pair(LS_KICK_BF_GENERAL,"_KICKFRONTBACK"),
		std::make_pair(LS_KICK_RL_GENERAL,"_KICKBOTHSIDES"),
		std::make_pair(LS_KICK_F_AIR_GENERAL,"_KICKFRONTAIR"),
		std::make_pair(LS_KICK_B_AIR_GENERAL,"_KICKBACKAIR"),
		std::make_pair(LS_KICK_R_AIR_GENERAL,"_KICKSIDEAIR"),
		std::make_pair(LS_KICK_L_AIR_GENERAL,"_KICKSIDEAIR"),
		std::make_pair(LS_STABDOWN_GENERAL,"_STABGROUND"),
		std::make_pair(LS_STABDOWN_STAFF_GENERAL,"_STABGROUNDSTAFF"),
		std::make_pair(LS_STABDOWN_DUAL_GENERAL,"_STABGROUNDDUAL"),
		std::make_pair(LS_DUAL_SPIN_PROTECT_GENERAL,"_DUALBARRIER"),	// Dual saber barrier (spinning sabers)
		std::make_pair(LS_STAFF_SOULCAL_GENERAL,"_KATASTAFF"),
		std::make_pair(LS_A1_SPECIAL_GENERAL,"_KATABLUE"), // Fast attack kata
		std::make_pair(LS_A2_SPECIAL_GENERAL,"_KATAYEL"),
		std::make_pair(LS_A3_SPECIAL_GENERAL,"_KATARED"),
		std::make_pair(LS_UPSIDE_DOWN_ATTACK_GENERAL,"_FLIPATK"), // Can't find info on this. Animation looks like a vampire hanging upside down and wiggling a saber downwards
		std::make_pair(LS_PULL_ATTACK_STAB_GENERAL,"_PULLSTAB"),	// Can't find info on this either. 
		std::make_pair(LS_PULL_ATTACK_SWING_GENERAL,"_PULLSWING"),	// Some kind of animation that pulls someone in and stabs? Idk if its actually usable or how...
		std::make_pair(LS_SPINATTACK_ALORA_GENERAL,"_ALORA"), // "Alora Spin slash"? No info on it either idk. Might just all be single player stuff
		std::make_pair(LS_DUAL_FB_GENERAL,"_DUALSTABFB"), // Dual stab front back
		std::make_pair(LS_DUAL_LR_GENERAL,"_DUALSTABLR"), // dual stab left right
		std::make_pair(LS_HILT_BASH_GENERAL,"_HILTBASH"), // Staff handle bashed into face (like darth maul i guess?)
		// JKA end

	  //starts
	  std::make_pair(LS_S_TL2BR_GENERAL, ""),//26
	  std::make_pair(LS_S_L2R_GENERAL, ""),
	  std::make_pair(LS_S_BL2TR_GENERAL, ""),//# Start of attack chaining to SLASH LR2UL
	  std::make_pair(LS_S_BR2TL_GENERAL, ""),//# Start of attack chaining to SLASH LR2UL
	  std::make_pair(LS_S_R2L_GENERAL, ""),
	  std::make_pair(LS_S_TR2BL_GENERAL, ""),
	  std::make_pair(LS_S_T2B_GENERAL, ""),

	  //returns
	  std::make_pair(LS_R_TL2BR_GENERAL, ""),//33
	  std::make_pair(LS_R_L2R_GENERAL, ""),
	  std::make_pair(LS_R_BL2TR_GENERAL, ""),
	  std::make_pair(LS_R_BR2TL_GENERAL, ""),
	  std::make_pair(LS_R_R2L_GENERAL, ""),
	  std::make_pair(LS_R_TR2BL_GENERAL, ""),
	  std::make_pair(LS_R_T2B_GENERAL, ""),

	  //transitions
	  std::make_pair(LS_T1_BR__R_GENERAL, ""),//40
	  std::make_pair(LS_T1_BR_TR_GENERAL, ""),
	  std::make_pair(LS_T1_BR_T__GENERAL, ""),
	  std::make_pair(LS_T1_BR_TL_GENERAL, ""),
	  std::make_pair(LS_T1_BR__L_GENERAL, ""),
	  std::make_pair(LS_T1_BR_BL_GENERAL, ""),
	  std::make_pair(LS_T1__R_BR_GENERAL, ""),//46
	  std::make_pair(LS_T1__R_TR_GENERAL, ""),
	  std::make_pair(LS_T1__R_T__GENERAL, ""),
	  std::make_pair(LS_T1__R_TL_GENERAL, ""),
	  std::make_pair(LS_T1__R__L_GENERAL, ""),
	  std::make_pair(LS_T1__R_BL_GENERAL, ""),
	  std::make_pair(LS_T1_TR_BR_GENERAL, ""),//52
	  std::make_pair(LS_T1_TR__R_GENERAL, ""),
	  std::make_pair(LS_T1_TR_T__GENERAL, ""),
	  std::make_pair(LS_T1_TR_TL_GENERAL, ""),
	  std::make_pair(LS_T1_TR__L_GENERAL, ""),
	  std::make_pair(LS_T1_TR_BL_GENERAL, ""),
	  std::make_pair(LS_T1_T__BR_GENERAL, ""),//58
	  std::make_pair(LS_T1_T___R_GENERAL, ""),
	  std::make_pair(LS_T1_T__TR_GENERAL, ""),
	  std::make_pair(LS_T1_T__TL_GENERAL, ""),
	  std::make_pair(LS_T1_T___L_GENERAL, ""),
	  std::make_pair(LS_T1_T__BL_GENERAL, ""),
	  std::make_pair(LS_T1_TL_BR_GENERAL, ""),//64
	  std::make_pair(LS_T1_TL__R_GENERAL, ""),
	  std::make_pair(LS_T1_TL_TR_GENERAL, ""),
	  std::make_pair(LS_T1_TL_T__GENERAL, ""),
	  std::make_pair(LS_T1_TL__L_GENERAL, ""),
	  std::make_pair(LS_T1_TL_BL_GENERAL, ""),
	  std::make_pair(LS_T1__L_BR_GENERAL, ""),//70
	  std::make_pair(LS_T1__L__R_GENERAL, ""),
	  std::make_pair(LS_T1__L_TR_GENERAL, ""),
	  std::make_pair(LS_T1__L_T__GENERAL, ""),
	  std::make_pair(LS_T1__L_TL_GENERAL, ""),
	  std::make_pair(LS_T1__L_BL_GENERAL, ""),
	  std::make_pair(LS_T1_BL_BR_GENERAL, ""),//76
	  std::make_pair(LS_T1_BL__R_GENERAL, ""),
	  std::make_pair(LS_T1_BL_TR_GENERAL, ""),
	  std::make_pair(LS_T1_BL_T__GENERAL, ""),
	  std::make_pair(LS_T1_BL_TL_GENERAL, ""),
	  std::make_pair(LS_T1_BL__L_GENERAL, ""),

	  //Bounces
	  std::make_pair(LS_B1_BR_GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1__R_GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1_TR_GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1_T__GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1_TL_GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1__L_GENERAL, "_BOUNCE"),
	  std::make_pair(LS_B1_BL_GENERAL, "_BOUNCE"),

	  //Deflected attacks
	  std::make_pair(LS_D1_BR_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1__R_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1_TR_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1_T__GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1_TL_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1__L_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1_BL_GENERAL, "_DEFLECT"),
	  std::make_pair(LS_D1_B__GENERAL, "_DEFLECT"),

	  //Reflected attacks
	  std::make_pair(LS_V1_BR_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1__R_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1_TR_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1_T__GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1_TL_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1__L_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1_BL_GENERAL, "_REFLECT"),
	  std::make_pair(LS_V1_B__GENERAL, "_REFLECT"),

	  // Broken parries
	  std::make_pair(LS_H1_T__GENERAL, "_BPARRY"),//
	  std::make_pair(LS_H1_TR_GENERAL, "_BPARRY"),
	  std::make_pair(LS_H1_TL_GENERAL, "_BPARRY"),
	  std::make_pair(LS_H1_BR_GENERAL, "_BPARRY"),
	  std::make_pair(LS_H1_B__GENERAL, "_BPARRY"),
	  std::make_pair(LS_H1_BL_GENERAL, "_BPARRY"),

	  // Knockaways
	  std::make_pair(LS_K1_T__GENERAL, "_KNOCKAWAY"),//
	  std::make_pair(LS_K1_TR_GENERAL, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_TL_GENERAL, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_BR_GENERAL, "_KNOCKAWAY"),
	  std::make_pair(LS_K1_BL_GENERAL, "_KNOCKAWAY"),

	  // Parries
	  std::make_pair(LS_PARRY_UP_GENERAL, "_PARRY"),//
	  std::make_pair(LS_PARRY_UR_GENERAL, "_PARRY"),
	  std::make_pair(LS_PARRY_UL_GENERAL, "_PARRY"),
	  std::make_pair(LS_PARRY_LR_GENERAL, "_PARRY"),
	  std::make_pair(LS_PARRY_LL_GENERAL, "_PARRY"),

	  // Projectile Reflections
	  std::make_pair(LS_REFLECT_UP_GENERAL, "_PREFLECT"),//
	  std::make_pair(LS_REFLECT_UR_GENERAL, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_UL_GENERAL, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_LR_GENERAL, "_PREFLECT"),
	  std::make_pair(LS_REFLECT_LL_GENERAL, "_PREFLECT"),

	  //std::make_pair(LS_MOVE_MAX, "")//
};

ankerl::unordered_dense::map <int, std::string, ankerl::unordered_dense::hash<int>>  saberStyleNames
{
	std::make_pair(FORCE_LEVEL_0, "_UNKST"),
	std::make_pair(FORCE_LEVEL_1, "_BLU"),
	std::make_pair(FORCE_LEVEL_2, "_YEL"),
	std::make_pair(FORCE_LEVEL_3, "_RED"),

	// New in jka: but the values of the former ones have stayed the same so we dont have to worry its ok.
	std::make_pair(SS_DESANN, "_DESANN"), // Is there a better name?
	std::make_pair(SS_TAVION, "_TAVION"), // Is there a better name?
	std::make_pair(SS_DUAL, "_DUAL"),
	std::make_pair(SS_STAFF, "_STAFF"),
	std::make_pair(SS_NUM_SABER_STYLES, ""),
	//std::make_pair(NUM_FORCE_POWER_LEVELS, "")
};


ankerl::unordered_dense::map <std::string, int, ankerl::unordered_dense::hash<std::string>>  mohaaWeaponModelMap
{
	std::make_pair("NONE",WPREFIX_NONE), // ?
	std::make_pair("models/items/papers.tik",WPREFIX_PAPERS), // ????
	std::make_pair("models/items/papers2.tik",WPREFIX_PAPERS), // ????
	std::make_pair("models/weapons/colt45.tik",WPREFIX_COLT45),
	std::make_pair("models/weapons/p38.tik",WPREFIX_P38),
	std::make_pair("models/weapons/silencedpistol.tik",WPREFIX_HISTANDARD),
	std::make_pair("models/weapons/m1_garand.tik",WPREFIX_GARAND),
	std::make_pair("models/weapons/kar98.tik",WPREFIX_KAR98),
	std::make_pair("models/weapons/kar98sniper.tik",WPREFIX_KAR98SNIPER),
	std::make_pair("models/weapons/springfield.tik",WPREFIX_SPRINGFIELD),
	std::make_pair("models/weapons/thompsonsmg.tik",WPREFIX_THOMPSON),
	std::make_pair("models/weapons/mp40.tik",WPREFIX_MP40),
	std::make_pair("models/weapons/bar.tik",WPREFIX_BAR),
	std::make_pair("models/weapons/mp44.tik",WPREFIX_MP44),
	std::make_pair("models/weapons/m2frag_grenade.tik",WPREFIX_FRAGGRENADE),
	std::make_pair("models/weapons/steilhandgranate.tik",WPREFIX_STIELHANDGRANATE),
	std::make_pair("models/weapons/bazooka.tik",WPREFIX_BAZOOKA),
	std::make_pair("models/weapons/panzerschreck.tik",WPREFIX_PANZERSCHRECK),
	std::make_pair("models/weapons/shotgun.tik",WPREFIX_SHOTGUN),
	std::make_pair("UNARMED",WPREFIX_UNARMED), // ??
	//
	// Team Assault and Team Tactics weapons
	std::make_pair("models/weapons/mg42carryable.tik",WPREFIX_MG42_PORTABLE),
	std::make_pair("models/weapons/webley_revolver.tik",WPREFIX_WEBLEY),
	std::make_pair("models/weapons/nagant_revolver.tik",WPREFIX_NAGANTREV),
	std::make_pair("models/weapons/mosin_nagant_rifle.tik",-1), // ???
	std::make_pair("models/weapons/It_W_Beretta.tik",WPREFIX_BERETTA),
	std::make_pair("models/weapons/enfield.tik",WPREFIX_ENFIELD),
	std::make_pair("models/weapons/svt_rifle.tik",WPREFIX_SVT),
	std::make_pair("models/weapons/mosin_nagant_rifle.tik",WPREFIX_MOSIN),
	std::make_pair("models/weapons/Mosin_Nagant_Rifle_lite.tik",WPREFIX_MOSIN), //? is that a real thing?
	std::make_pair("models/weapons/g43.tik",WPREFIX_G43),
	std::make_pair("models/weapons/Uk_W_L42A1.tik",WPREFIX_ENFIELDL42A),
	std::make_pair("models/weapons/It_W_Carcano.tik",WPREFIX_CARCANO),
	std::make_pair("models/weapons/delisle.tik",WPREFIX_DELISLE),
	std::make_pair("models/weapons/sten.tik",WPREFIX_STEN),
	std::make_pair("models/weapons/ppsh_smg.tik",WPREFIX_PPSH),
	std::make_pair("models/weapons/It_W_Moschetto.tik",WPREFIX_MOSCHETTO),
	std::make_pair("FG42",WPREFIX_FG42), // ?
	std::make_pair("models/weapons/Uk_W_Vickers.tik",WPREFIX_VICKERS),
	std::make_pair("models/weapons/It_W_Breda.tik",WPREFIX_BREDA),
	std::make_pair("models/weapons/Russian_F1_grenade.tik",WPREFIX_F1_GRENADE),
	std::make_pair("models/weapons/mills_grenade.tik",WPREFIX_MILLS_GRENADE),
	std::make_pair("models/weapons/nebelhandgranate.tik",WPREFIX_NEBELHANDGRANATE),
	std::make_pair("models/weapons/M18_smoke_grenade.tik",WPREFIX_M18_SMOKE_GRENADE),
	std::make_pair("models/weapons/RDG-1_Smoke_grenade.tik",WPREFIX_RDG1_SMOKE_GRENADE),
	std::make_pair("models/weapons/It_W_Bomba.tik",WPREFIX_BOMBA),
	std::make_pair("models/weapons/It_W_BombaBreda.tik",WPREFIX_BOMBA_BREDA),
	std::make_pair("models/weapons/Landmine.tik",WPREFIX_MINE),
	std::make_pair("models/weapons/US_W_MineDetector.tik",WPREFIX_MINE_DETECTOR),
	std::make_pair("models/weapons/Gr_W_MineDetector.tik",WPREFIX_MINE_DETECTOR_AXIS),
	std::make_pair("DETONATOR",WPREFIX_DETONATOR), // ?
	std::make_pair("models/weapons/kar98_mortar.tik",WPREFIX_KAR98_MORTAR),
	std::make_pair("models/weapons/Uk_W_Piat.tik",WPREFIX_PIAT)
};


void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, qboolean snap, demoType_t demoType, qboolean writeCommandTime, qboolean clientSideStyleEventConversion) {
	int		i;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = specializeGameValue<GMAP_ENTITYTYPE,UNSAFE>(ET_INVISIBLE_GENERAL,demoType);
	}
	else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) { // Hmm should I be really doing this?
		s->eType = specializeGameValue<GMAP_ENTITYTYPE, UNSAFE>(ET_INVISIBLE_GENERAL, demoType);;
	}
	else {
		s->eType = specializeGameValue<GMAP_ENTITYTYPE, UNSAFE>(ET_PLAYER_GENERAL, demoType);;
	}

	if (isMOHAADemo) {
		s->parent = ENTITYNUM_NONE;
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

	s->legsFlip = ps->legsFlip; // JK3
	s->torsoFlip = ps->torsoFlip; // JK3

	s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	s->eFlags2 = ps->eFlags2; // JK3

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
	s->saberHolstered = ps->saberHolstered; // JK3

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

		entityEventSequenceReal = ps->entityEventSequenceNOTNETWORKED - 1;
		if (entityEventSequenceReal < ps->eventSequence - MAX_PS_EVENTS) {
			entityEventSequenceReal = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = entityEventSequenceReal & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq] | ((entityEventSequenceReal & 3) << 8);
		s->eventParm = ps->eventParms[seq];
	}
	else if (ps->entityEventSequenceNOTNETWORKED < ps->eventSequence) {
		int		seq;

		if (ps->entityEventSequenceNOTNETWORKED < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequenceNOTNETWORKED = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequenceNOTNETWORKED & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq] | ((ps->entityEventSequenceNOTNETWORKED & 3) << 8);
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequenceNOTNETWORKED++;
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

	// JK3
	s->heldByClient = ps->heldByClient;
	s->ragAttach = ps->ragAttach;

	s->iModelScale = ps->iModelScale;

	s->brokenLimbs = ps->brokenLimbs;

	s->hasLookTarget = ps->hasLookTarget;
	s->lookTarget = ps->lookTarget;

	s->customRGBA[0] = ps->customRGBA[0];
	s->customRGBA[1] = ps->customRGBA[1];
	s->customRGBA[2] = ps->customRGBA[2];
	s->customRGBA[3] = ps->customRGBA[3];

	s->m_iVehicleNum = ps->m_iVehicleNum;

	s->demoToolsData = ps->demoToolsData;

	if (demoType == DM_14) {
		s->modelindex3 = ps->saberColor;
	}
}

int getMOHTeam(entityState_t* s) {
	return (s->eFlags & EF_ANY_TEAM_MOH) ? ((s->eFlags & EF_AXIS_MOH) ? TEAM_AXIS_MOH : TEAM_ALLIES_MOH) : TEAM_FREEFORALL_MOH;
}

// baseState is my modification. It will use values from the base Snap that an entity just doesn't have. PERS_SPAWN_COUNT and precise health/armor
void CG_EntityStateToPlayerState(entityState_t* s, playerState_t* ps, demoType_t demoType, qboolean allValues, playerState_t* baseState, qboolean enhanceOnly) {
	int		i;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

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

	if (isMOHAADemo) {
		if (s->solid) {
			vec3_t mins, maxs;
			IntegerToBoundingBox(s->solid, mins, maxs);
			switch ((int)maxs[2]) { // Very bad way of doing this tbh. idk. different places of the code say different values. sigh
			case 94:
			case MAXS_Z_MOH:
				if (maxs[2] == 94.0f && mins[2] == 54.0f) { // Don't ask it's probably wrong anyway
					ps->viewheight = CROUCH_RUN_VIEWHEIGHT_MOH;
				}
				else {
					ps->viewheight = DEFAULT_VIEWHEIGHT_MOH;
				}
				break;
			case 54:// But supposedly prone AND ducked in one place of the code? idk
			case CROUCH_MAXS_Z_MOH:
				ps->viewheight = CROUCH_VIEWHEIGHT_MOH;
				break;
			case 60:
			case 53:
				ps->viewheight = CROUCH_RUN_VIEWHEIGHT_MOH;
				break;
			case 52:
				ps->viewheight = JUMP_START_VIEWHEIGHT_MOH;
				break;
			case 20:
				ps->viewheight = PRONE_VIEWHEIGHT_MOH;
				break;
			default:
				ps->viewheight = DEFAULT_VIEWHEIGHT_MOH;
				break;
			}
		}
		else {
			ps->viewheight = DEFAULT_VIEWHEIGHT_MOH; // Idk, sometimes solid isn't set for whatever reason.
		}
		
		ps->viewangles[0] = s->bone_angles[0][0] / 0.37f;
		//ps->viewangles[2] = s->bone_angles[0][2] / -0.4f;
		//ps->fLeanAngle = ps->viewangles[2];
		ps->fLeanAngle = s->bone_angles[0][2] / -0.4f;
		ps->fovMOHAA = 80;
		ps->stats[STAT_CROSSHAIR_MOH] = qtrue;
		ps->stats[STAT_TEAM_MOH] = getMOHTeam(s);
		VectorCopy(ps->origin, ps->camera_origin);
		VectorCopy(ps->viewangles, ps->camera_angles);
	}


	ps->legsFlip = s->legsFlip;// JK3
	ps->torsoFlip = s->torsoFlip;// JK3


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

	if (demoType == DM_15 || demoType == DM_15_1_03 || demoType == DM_16) {
		ps->saberHolstered = s->shouldtarget; //reuse bool in entitystate for players differently
	}
	else if(demoType == DM_25 || demoType == DM_26 || demoType == DM_26_XBOX){ // JK3 things.
		ps->saberHolstered = (qboolean)s->saberHolstered;
	}
	ps->usingATST = (qboolean)s->teamowner;

	/*
	if (ps->genericEnemyIndex != -1)
	{
		s->eFlags |= EF_SEEKERDRONE;
	}
	*/
	if (!enhanceOnly || (s->eFlags & EF_DEAD) > 0 != (ps->stats[STAT_HEALTH] <= 0)) {
		//The client has no knowledge of health levels (except for the client entity)
		if (s->eFlags & EF_DEAD)
		{
			ps->stats[STAT_HEALTH] = 0;
			ps->pm_type = PM_DEAD;
		}
		else
		{
			ps->stats[STAT_HEALTH] = 100;
			if (ps->pm_type == PM_DEAD) { // Correct this if needed.
				ps->pm_type = PM_NORMAL;
			}
		}
	}

	if(!enhanceOnly){
		ps->genericEnemyIndex = -1; //no real option for this


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
			if (s->weapon < specializeGameValue<GMAP_WEAPONS, UNSAFE>( WP_NUM_WEAPONS_GENERAL,demoType)) {

				ps->stats[STAT_WEAPONS] |= 1 << s->weapon; // To make weapon select wheel look correct
			}

			if (generalizeGameValue<GMAP_WEAPONS, UNSAFE>( s->weapon,demoType) == WP_SABER_GENERAL && s->fireflag == 0) {
				// Server disabled sending fireflag saber style. We must deduce from animations (ugh)
				byte probability;
				ps->fd.saberDrawAnimLevel = ps->fd.saberAnimLevel = getLikelyStanceFromTorsoAnim(s->torsoAnim, demoType, &probability);
			}
			else {
				ps->fd.saberDrawAnimLevel = ps->fd.saberAnimLevel = s->fireflag;
			}

			if(!isMOHAADemo){
				int generalizedLegsAnim = generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(s->legsAnim,demoType);

				// This is my own stuff: (restoring viewheight)
				if (s->eFlags & EF_DEAD) {
					ps->viewheight = DEAD_VIEWHEIGHT;
				}
				else if (
					(generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_GENERAL //|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1IDLE_15
					|| (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALKBACK_GENERAL || (generalizedLegsAnim & ~ANIM_TOGGLEBIT) == BOTH_CROUCH1WALK_GENERAL
			
					) {
					ps->viewheight = CROUCH_VIEWHEIGHT;
				}
				else {
					ps->viewheight = DEFAULT_VIEWHEIGHT;
				}
			}
		}

		// JK3 (I havent checked if these are actually networked!)
		ps->heldByClient = s->heldByClient;
		ps->ragAttach = s->ragAttach;

		ps->iModelScale = s->iModelScale;

		ps->brokenLimbs = s->brokenLimbs;

		ps->hasLookTarget = s->hasLookTarget;
		ps->lookTarget = s->lookTarget;

		ps->customRGBA[0] = s->customRGBA[0];
		ps->customRGBA[1] = s->customRGBA[1];
		ps->customRGBA[2] = s->customRGBA[2];
		ps->customRGBA[3] = s->customRGBA[3];

		ps->m_iVehicleNum = s->m_iVehicleNum;

	}

	if (baseState) {
		//ps->persistant[PERS_SPAWN_COUNT] = baseState->persistant[PERS_SPAWN_COUNT];
		Com_Memcpy(ps->persistant, baseState->persistant, sizeof(ps->persistant));
		Com_Memcpy(ps->ammo, baseState->ammo, sizeof(ps->ammo));
		ps->damageEvent = baseState->damageEvent;
		ps->damageCount = baseState->damageCount;
		ps->damageYaw = baseState->damageYaw;
		ps->damagePitch = baseState->damagePitch;
		if (s->eFlags & EF_DEAD) {
			// Something to fix view angles? idk
		}
		ps->stats[STAT_HOLDABLE_ITEMS] = baseState->stats[STAT_HOLDABLE_ITEMS];
		ps->stats[STAT_HOLDABLE_ITEM] = baseState->stats[STAT_HOLDABLE_ITEM];
		ps->stats[STAT_WEAPONS] = baseState->stats[STAT_WEAPONS];
		ps->stats[STAT_DEAD_YAW] = baseState->stats[STAT_DEAD_YAW];
		ps->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH] = baseState->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH];
		if (ps->stats[STAT_HEALTH] > 0 && baseState->stats[STAT_HEALTH] > 0) { // don't copy positive health into a dead entity
			//Com_Memcpy(ps->stats,baseState->stats,sizeof(ps->stats));
			ps->fd.forcePower = baseState->fd.forcePower;
			ps->stats[STAT_HEALTH] = baseState->stats[STAT_HEALTH];
			ps->stats[STAT_ARMOR] = baseState->stats[STAT_ARMOR];
		}
	}

	ps->demoToolsData = s->demoToolsData;

	
	/*
		// trickedentindex3: armor (8 bits), health (8 bits)
		ent->client->ps.fd.forceMindtrickTargetIndex3 = ent->s.trickedentindex3 = ((MIN(127,MAX(-128,ent->client->ps.stats[STAT_HEALTH])) & 0xff) << 8) | (MIN(127, MAX(-128, ent->client->ps.stats[STAT_ARMOR])) & 0xff);
		// trickedentindex4: force power (7 bits), current weapon ammo (7 bits), saberdrawanimlevel (2 bits)
		ent->client->ps.fd.forceMindtrickTargetIndex4 = ent->s.trickedentindex4 = (ent->client->ps.fd.saberDrawAnimLevel & 3) << 14 | ((MAX(0, MIN(127, ent->client->ps.ammo[weaponData[ent->client->ps.weapon].ammoIndex])) & 127) << 7) | (MAX(0, MIN(127, ent->client->ps.fd.forcePower)) & 127);
		// generic1: seeker, forcefield, bacta, sentry in inventory (1 bit each), mine count (4 bits)
		ent->client->ps.generic1 = ent->s.generic1 = ((MAX(0, MIN(15, ent->client->ps.ammo[weaponData[WP_TRIP_MINE].ammoIndex])) & 15) << 4) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SENTRY_GUN))) << 3) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_MEDPAC))) << 2) | ((!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SHIELD))) << 1) | (!!(ent->client->ps.stats[STAT_HOLDABLE_ITEMS] & (1 << HI_SEEKER)));
	*/
	/*thisEntity->demoToolsData.entityExtraValues[ENTITYEXTRA_HEALTH] = (signed char)((thisEntity->trickedentindex3>>8) & 0xff);
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
	*/
	bool hasAnyInventory = false;
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_HEALTH)) {
		ps->stats[STAT_HEALTH] = s->demoToolsData.entityExtraValues[ENTITYEXTRA_HEALTH];
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_ARMOR)) {
		ps->stats[STAT_ARMOR] = s->demoToolsData.entityExtraValues[ENTITYEXTRA_ARMOR];
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_FORCE)) {
		ps->fd.forcePower = s->demoToolsData.entityExtraValues[ENTITYEXTRA_FORCE];
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_SABERDRAWANIMLEVEL)) {
		ps->fd.saberDrawAnimLevel = s->demoToolsData.entityExtraValues[ENTITYEXTRA_SABERDRAWANIMLEVEL];
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_CURRENTWEAPONAMMO) && s->weapon < WP_NUM_WEAPONS_JK2 && s->weapon >= 0) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->ammo[weaponData_1_02[s->weapon].ammoIndex] = s->demoToolsData.entityExtraValues[ENTITYEXTRA_CURRENTWEAPONAMMO];
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1 << ENTITYEXTRA_TRIPMINEAMMO) && s->demoToolsData.entityExtraValues[ENTITYEXTRA_TRIPMINEAMMO]) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->ammo[weaponData_1_02[WP_TRIP_MINE_JK2].ammoIndex] = s->demoToolsData.entityExtraValues[ENTITYEXTRA_TRIPMINEAMMO];
		ps->stats[STAT_WEAPONS] |= (1 << WP_TRIP_MINE_JK2);
		hasAnyInventory = true;
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_HASSENTRY) && s->demoToolsData.entityExtraValues[ENTITYEXTRA_HASSENTRY]) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_SENTRY_GUN);
		hasAnyInventory = true;
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_HASMEDPACK) && s->demoToolsData.entityExtraValues[ENTITYEXTRA_HASMEDPACK]) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->stats[STAT_HOLDABLE_ITEMS] |= (1<< HI_MEDPAC);
		hasAnyInventory = true;
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_HASFORCEFIELD) && s->demoToolsData.entityExtraValues[ENTITYEXTRA_HASFORCEFIELD]) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_SHIELD);
		hasAnyInventory = true;
	}
	if (s->demoToolsData.entityExtraValuesBitmask & (1<< ENTITYEXTRA_HASSEEKER) && s->demoToolsData.entityExtraValues[ENTITYEXTRA_HASSEEKER]) {
		// TODO Make this work for other stuff than jk2 1.02?
		ps->stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_SEEKER);
		hasAnyInventory = true;
	}
	if (hasAnyInventory) {

		ps->stats[STAT_HOLDABLE_ITEM] = 7;// stupid lol, setting it to datapad. its just used for inventory selection stuff and we need it to draw inventory on screen.
	}


	if (demoType == DM_14) {
		ps->saberColor = (saber_colors_t)s->modelindex3;
	}
}

void EnhancePlayerStateWithBaseState(playerState_t* ps, playerState_t* baseState, demoType_t demoType) {
	int		i;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	Com_Memcpy(ps->persistant, baseState->persistant, sizeof(ps->persistant));
	Com_Memcpy(ps->ammo, baseState->ammo, sizeof(ps->ammo));
	ps->damageEvent = baseState->damageEvent;
	ps->damageCount = baseState->damageCount;
	ps->damageYaw = baseState->damageYaw;
	ps->damagePitch = baseState->damagePitch;
	if (ps->stats[STAT_HEALTH] <= 0) {
		// Something to fix view angles? idk
	}
	ps->stats[STAT_HOLDABLE_ITEMS] = baseState->stats[STAT_HOLDABLE_ITEMS];
	ps->stats[STAT_HOLDABLE_ITEM] = baseState->stats[STAT_HOLDABLE_ITEM];
	ps->stats[STAT_WEAPONS] = baseState->stats[STAT_WEAPONS];
	ps->stats[STAT_DEAD_YAW] = baseState->stats[STAT_DEAD_YAW];
	ps->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH] = baseState->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH];
	if (ps->stats[STAT_HEALTH] > 0 && baseState->stats[STAT_HEALTH] > 0) { // don't copy positive health into a dead entity
		//Com_Memcpy(ps->stats,baseState->stats,sizeof(ps->stats));
		ps->fd.forcePower = baseState->fd.forcePower;
		ps->stats[STAT_HEALTH] = baseState->stats[STAT_HEALTH];
		ps->stats[STAT_ARMOR] = baseState->stats[STAT_ARMOR];
		ps->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH] = baseState->stats[isMOHAADemo ? STAT_MAXHEALTH_MOH : STAT_MAX_HEALTH];
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
			newString.erase(0, (std::max)((int)newString.find_first_not_of(" "), 0));
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
			newString.erase(0, (std::max)((int)newString.find_first_not_of(" "), 0));
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

	torsoAnim &= ~getANIM_TOGGLEBIT(demoType);

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
			std::cout << "torsoAnim " << torsoAnim << " higher than max for DM_15_1_03/DM_16 (" << countDm15AnimMappings << ")\n";
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

		byte actualProbability = (byte)(std::min)((int)highestProbability,(int)(10.0*sqrt(mappingHere->totalCount)));
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

float Vector2Distance(vec2_t v1, vec2_t v2)
{
	vec2_t dir;

	Vector2Subtract(v2, v1, dir);
	return VectorLength2(dir);
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

#if 0
gitem_t	bg_itemlist_UNUSED[] =
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
				WP_STUN_BATON_JK2,
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
					WP_SABER_JK2,
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
					WP_BRYAR_PISTOL_JK2,
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
					WP_BLASTER_JK2,
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
					WP_DISRUPTOR_JK2,
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
					WP_BOWCASTER_JK2,
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
					WP_REPEATER_JK2,
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
					WP_DEMP2_JK2,
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
					WP_FLECHETTE_JK2,
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
					WP_ROCKET_LAUNCHER_JK2,
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
					WP_THERMAL_JK2,
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
					WP_TRIP_MINE_JK2,
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
					WP_DET_PACK_JK2,
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
					WP_EMPLACED_GUN_JK2,
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
					WP_TURRET_JK2,
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
#endif


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

const float forceJumpHeight[NUM_FORCE_POWER_LEVELS] =
{
	32,//normal jump (+stepheight+crouchdiff = 66)
	96,//(+stepheight+crouchdiff = 130)
	192,//(+stepheight+crouchdiff = 226)
	384//(+stepheight+crouchdiff = 418)
};

const float forceJumpStrength[NUM_FORCE_POWER_LEVELS] =
{
	JUMP_VELOCITY,//normal jump
	420,
	590,
	840
};

weaponData_t weaponData_1_02[WP_NUM_WEAPONS_JK2] =
{
	{	// WP_NONE
//		"No Weapon",			//	char	classname[32];		// Spawning name
		AMMO_NONE,				//	int		ammoIndex;			// Index to proper ammo slot
		0,						//	int		ammoLow;			// Count when ammo is low
		0,						//	int		energyPerShot;		// Amount of energy used per shot
		0,						//	int		fireTime;			// Amount of time between firings
		0,						//	int		range;				// Range of weapon
		0,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		0,						//	int		altFireTime;		// Amount of time between alt-firings
		0,						//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_STUN_BATON
//		"Stun Baton",			//	char	classname[32];		// Spawning name
		AMMO_NONE,				//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		0,						//	int		energyPerShot;		// Amount of energy used per shot
		400,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		0,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		400,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_SABER,
//		"Lightsaber",			//	char	classname[32];		// Spawning name
		AMMO_NONE,				//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		0,						//	int		energyPerShot;		// Amount of energy used per shot
		100,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		0,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		100,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_BRYAR_PISTOL,
//		"Bryar Pistol",			//	char	classname[32];		// Spawning name
		AMMO_BLASTER,			//	int		ammoIndex;			// Index to proper ammo slot
		15,						//	int		ammoLow;			// Count when ammo is low
		2,						//	int		energyPerShot;		// Amount of energy used per shot
		400,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		2,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		400,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		200,					//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		1,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		1500					//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_BLASTER
//		"E11 Blaster Rifle",	//	char	classname[32];		// Spawning name
		AMMO_BLASTER,			//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		2,						//	int		energyPerShot;		// Amount of energy used per shot
		350,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		3,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		150,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_DISRUPTOR
//		"Tenloss Disruptor Rifle",//	char	classname[32];		// Spawning name
		AMMO_POWERCELL,			//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		5,						//	int		energyPerShot;		// Amount of energy used per shot
		600,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		6,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		1300,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		200,					//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		3,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		1700					//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_BOWCASTER
//		"Wookiee Bowcaster",		//	char	classname[32];		// Spawning name
		AMMO_POWERCELL,			//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		5,						//	int		energyPerShot;		// Amount of energy used per shot
		1000,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		5,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		750,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		400,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,					//	int		altChargeSubTime;	// above for secondary
		5,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		1700,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0					//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_REPEATER
//		"Imperial Heavy Repeater",//	char	classname[32];		// Spawning name
		AMMO_METAL_BOLTS,		//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		1,						//	int		energyPerShot;		// Amount of energy used per shot
		100,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		8,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		800,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_DEMP2
//		"DEMP2",				//	char	classname[32];		// Spawning name
		AMMO_POWERCELL,			//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		8,						//	int		energyPerShot;		// Amount of energy used per shot
		500,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		6,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		900,						//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		250,					//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		3,						//	int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		2100					//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_FLECHETTE
//		"Golan Arms Flechette",	//	char	classname[32];		// Spawning name
		AMMO_METAL_BOLTS,		//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		10,						//	int		energyPerShot;		// Amount of energy used per shot
		700,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		15,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		800,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_ROCKET_LAUNCHER
//		"Merr-Sonn Missile System",	//	char	classname[32];		// Spawning name
		AMMO_ROCKETS,			//	int		ammoIndex;			// Index to proper ammo slot
		5,						//	int		ammoLow;			// Count when ammo is low
		1,						//	int		energyPerShot;		// Amount of energy used per shot
		900,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		2,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		1200,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_THERMAL
//		"Thermal Detonator",	//	char	classname[32];		// Spawning name
		AMMO_THERMAL,				//	int		ammoIndex;			// Index to proper ammo slot
		0,						//	int		ammoLow;			// Count when ammo is low
		1,						//	int		energyPerShot;		// Amount of energy used per shot
		800,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		1,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		400,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_TRIP_MINE
//		"Trip Mine",			//	char	classname[32];		// Spawning name
		AMMO_TRIPMINE,				//	int		ammoIndex;			// Index to proper ammo slot
		0,						//	int		ammoLow;			// Count when ammo is low
		1,						//	int		energyPerShot;		// Amount of energy used per shot
		800,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		1,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		400,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_DET_PACK
//		"Det Pack",				//	char	classname[32];		// Spawning name
		AMMO_DETPACK,				//	int		ammoIndex;			// Index to proper ammo slot
		0,						//	int		ammoLow;			// Count when ammo is low
		1,						//	int		energyPerShot;		// Amount of energy used per shot
		800,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		0,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		400,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	},
	{	// WP_EMPLCACED_GUN
//		"Emplaced Gun",			//	char	classname[32];		// Spawning name
		/*AMMO_BLASTER*/0,			//	int		ammoIndex;			// Index to proper ammo slot
		/*5*/0,						//	int		ammoLow;			// Count when ammo is low
		/*2*/0,						//	int		energyPerShot;		// Amount of energy used per shot
		100,					//	int		fireTime;			// Amount of time between firings
		8192,					//	int		range;				// Range of weapon
		/*3*/0,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
		100,					//	int		altFireTime;		// Amount of time between alt-firings
		8192,					//	int		altRange;			// Range of alt-fire
		0,						//	int		chargeSubTime;		// ms interval for subtracting ammo during charge
		0,						//	int		altChargeSubTime;	// above for secondary
		0,						//	int		chargeSub;			// amount to subtract during charge on each interval
		0,						//int		altChargeSub;		// above for secondary
		0,						//	int		maxCharge;			// stop subtracting once charged for this many ms
		0						//	int		altMaxCharge;		// above for secondary
	}
};



static vec3_t	bytedirs[NUMVERTEXNORMALS] =
{
{-0.525731f, 0.000000f, 0.850651f}, {-0.442863f, 0.238856f, 0.864188f},
{-0.295242f, 0.000000f, 0.955423f}, {-0.309017f, 0.500000f, 0.809017f},
{-0.162460f, 0.262866f, 0.951056f}, {0.000000f, 0.000000f, 1.000000f},
{0.000000f, 0.850651f, 0.525731f}, {-0.147621f, 0.716567f, 0.681718f},
{0.147621f, 0.716567f, 0.681718f}, {0.000000f, 0.525731f, 0.850651f},
{0.309017f, 0.500000f, 0.809017f}, {0.525731f, 0.000000f, 0.850651f},
{0.295242f, 0.000000f, 0.955423f}, {0.442863f, 0.238856f, 0.864188f},
{0.162460f, 0.262866f, 0.951056f}, {-0.681718f, 0.147621f, 0.716567f},
{-0.809017f, 0.309017f, 0.500000f},{-0.587785f, 0.425325f, 0.688191f},
{-0.850651f, 0.525731f, 0.000000f},{-0.864188f, 0.442863f, 0.238856f},
{-0.716567f, 0.681718f, 0.147621f},{-0.688191f, 0.587785f, 0.425325f},
{-0.500000f, 0.809017f, 0.309017f}, {-0.238856f, 0.864188f, 0.442863f},
{-0.425325f, 0.688191f, 0.587785f}, {-0.716567f, 0.681718f, -0.147621f},
{-0.500000f, 0.809017f, -0.309017f}, {-0.525731f, 0.850651f, 0.000000f},
{0.000000f, 0.850651f, -0.525731f}, {-0.238856f, 0.864188f, -0.442863f},
{0.000000f, 0.955423f, -0.295242f}, {-0.262866f, 0.951056f, -0.162460f},
{0.000000f, 1.000000f, 0.000000f}, {0.000000f, 0.955423f, 0.295242f},
{-0.262866f, 0.951056f, 0.162460f}, {0.238856f, 0.864188f, 0.442863f},
{0.262866f, 0.951056f, 0.162460f}, {0.500000f, 0.809017f, 0.309017f},
{0.238856f, 0.864188f, -0.442863f},{0.262866f, 0.951056f, -0.162460f},
{0.500000f, 0.809017f, -0.309017f},{0.850651f, 0.525731f, 0.000000f},
{0.716567f, 0.681718f, 0.147621f}, {0.716567f, 0.681718f, -0.147621f},
{0.525731f, 0.850651f, 0.000000f}, {0.425325f, 0.688191f, 0.587785f},
{0.864188f, 0.442863f, 0.238856f}, {0.688191f, 0.587785f, 0.425325f},
{0.809017f, 0.309017f, 0.500000f}, {0.681718f, 0.147621f, 0.716567f},
{0.587785f, 0.425325f, 0.688191f}, {0.955423f, 0.295242f, 0.000000f},
{1.000000f, 0.000000f, 0.000000f}, {0.951056f, 0.162460f, 0.262866f},
{0.850651f, -0.525731f, 0.000000f},{0.955423f, -0.295242f, 0.000000f},
{0.864188f, -0.442863f, 0.238856f}, {0.951056f, -0.162460f, 0.262866f},
{0.809017f, -0.309017f, 0.500000f}, {0.681718f, -0.147621f, 0.716567f},
{0.850651f, 0.000000f, 0.525731f}, {0.864188f, 0.442863f, -0.238856f},
{0.809017f, 0.309017f, -0.500000f}, {0.951056f, 0.162460f, -0.262866f},
{0.525731f, 0.000000f, -0.850651f}, {0.681718f, 0.147621f, -0.716567f},
{0.681718f, -0.147621f, -0.716567f},{0.850651f, 0.000000f, -0.525731f},
{0.809017f, -0.309017f, -0.500000f}, {0.864188f, -0.442863f, -0.238856f},
{0.951056f, -0.162460f, -0.262866f}, {0.147621f, 0.716567f, -0.681718f},
{0.309017f, 0.500000f, -0.809017f}, {0.425325f, 0.688191f, -0.587785f},
{0.442863f, 0.238856f, -0.864188f}, {0.587785f, 0.425325f, -0.688191f},
{0.688191f, 0.587785f, -0.425325f}, {-0.147621f, 0.716567f, -0.681718f},
{-0.309017f, 0.500000f, -0.809017f}, {0.000000f, 0.525731f, -0.850651f},
{-0.525731f, 0.000000f, -0.850651f}, {-0.442863f, 0.238856f, -0.864188f},
{-0.295242f, 0.000000f, -0.955423f}, {-0.162460f, 0.262866f, -0.951056f},
{0.000000f, 0.000000f, -1.000000f}, {0.295242f, 0.000000f, -0.955423f},
{0.162460f, 0.262866f, -0.951056f}, {-0.442863f, -0.238856f, -0.864188f},
{-0.309017f, -0.500000f, -0.809017f}, {-0.162460f, -0.262866f, -0.951056f},
{0.000000f, -0.850651f, -0.525731f}, {-0.147621f, -0.716567f, -0.681718f},
{0.147621f, -0.716567f, -0.681718f}, {0.000000f, -0.525731f, -0.850651f},
{0.309017f, -0.500000f, -0.809017f}, {0.442863f, -0.238856f, -0.864188f},
{0.162460f, -0.262866f, -0.951056f}, {0.238856f, -0.864188f, -0.442863f},
{0.500000f, -0.809017f, -0.309017f}, {0.425325f, -0.688191f, -0.587785f},
{0.716567f, -0.681718f, -0.147621f}, {0.688191f, -0.587785f, -0.425325f},
{0.587785f, -0.425325f, -0.688191f}, {0.000000f, -0.955423f, -0.295242f},
{0.000000f, -1.000000f, 0.000000f}, {0.262866f, -0.951056f, -0.162460f},
{0.000000f, -0.850651f, 0.525731f}, {0.000000f, -0.955423f, 0.295242f},
{0.238856f, -0.864188f, 0.442863f}, {0.262866f, -0.951056f, 0.162460f},
{0.500000f, -0.809017f, 0.309017f}, {0.716567f, -0.681718f, 0.147621f},
{0.525731f, -0.850651f, 0.000000f}, {-0.238856f, -0.864188f, -0.442863f},
{-0.500000f, -0.809017f, -0.309017f}, {-0.262866f, -0.951056f, -0.162460f},
{-0.850651f, -0.525731f, 0.000000f}, {-0.716567f, -0.681718f, -0.147621f},
{-0.716567f, -0.681718f, 0.147621f}, {-0.525731f, -0.850651f, 0.000000f},
{-0.500000f, -0.809017f, 0.309017f}, {-0.238856f, -0.864188f, 0.442863f},
{-0.262866f, -0.951056f, 0.162460f}, {-0.864188f, -0.442863f, 0.238856f},
{-0.809017f, -0.309017f, 0.500000f}, {-0.688191f, -0.587785f, 0.425325f},
{-0.681718f, -0.147621f, 0.716567f}, {-0.442863f, -0.238856f, 0.864188f},
{-0.587785f, -0.425325f, 0.688191f}, {-0.309017f, -0.500000f, 0.809017f},
{-0.147621f, -0.716567f, 0.681718f}, {-0.425325f, -0.688191f, 0.587785f},
{-0.162460f, -0.262866f, 0.951056f}, {0.442863f, -0.238856f, 0.864188f},
{0.162460f, -0.262866f, 0.951056f}, {0.309017f, -0.500000f, 0.809017f},
{0.147621f, -0.716567f, 0.681718f}, {0.000000f, -0.525731f, 0.850651f},
{0.425325f, -0.688191f, 0.587785f}, {0.587785f, -0.425325f, 0.688191f},
{0.688191f, -0.587785f, 0.425325f}, {-0.955423f, 0.295242f, 0.000000f},
{-0.951056f, 0.162460f, 0.262866f}, {-1.000000f, 0.000000f, 0.000000f},
{-0.850651f, 0.000000f, 0.525731f}, {-0.955423f, -0.295242f, 0.000000f},
{-0.951056f, -0.162460f, 0.262866f}, {-0.864188f, 0.442863f, -0.238856f},
{-0.951056f, 0.162460f, -0.262866f}, {-0.809017f, 0.309017f, -0.500000f},
{-0.864188f, -0.442863f, -0.238856f}, {-0.951056f, -0.162460f, -0.262866f},
{-0.809017f, -0.309017f, -0.500000f}, {-0.681718f, 0.147621f, -0.716567f},
{-0.681718f, -0.147621f, -0.716567f}, {-0.850651f, 0.000000f, -0.525731f},
{-0.688191f, 0.587785f, -0.425325f}, {-0.587785f, 0.425325f, -0.688191f},
{-0.425325f, 0.688191f, -0.587785f}, {-0.425325f, -0.688191f, -0.587785f},
{-0.587785f, -0.425325f, -0.688191f}, {-0.688191f, -0.587785f, -0.425325f}
};

void ByteToDir(int b, vec3_t dir) {
	if (b < 0 || b >= NUMVERTEXNORMALS) {
		VectorCopy(vec3_origin, dir);
		return;
	}
	VectorCopy(bytedirs[b], dir);
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
float vectoyaw( const vec3_t vec ) {
	float	yaw;
	
	if (vec[YAW] == 0 && vec[PITCH] == 0) {
		yaw = 0;
	} else {
		if (vec[PITCH]) {
			yaw = ( atan2( vec[YAW], vec[PITCH]) * 180 / M_PI );
		} else if (vec[YAW] > 0) {
			yaw = 90;
		} else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}
	}

	return yaw;
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

	// Improved variant. Same results for most values but it's more correct for extremely high values (and more performant as well I guess)
	// The reason I do this: Some demos end up having (or being read as having) nonsensically high  float values.
	// This results in the old code entering an endless loop because subtracting 360 no longer does  anything  to float  values that are that  high.
	a = fmodf(a, 360.0f); 
	if (a > 180) {
		a -= 360;
	}
	if (a < -180) {
		a += 360;
	}
#if 0
	assert(fabs(a) < 3600);
	while (a > 180) {
		a -= 360;
	}
	while (a < -180) {
		a += 360;
	}
#endif
	return a;
}


void AnglesSubtract(vec3_t v1, vec3_t v2, vec3_t v3) {
	v3[0] = AngleSubtract(v1[0], v2[0]);
	v3[1] = AngleSubtract(v1[1], v2[1]);
	v3[2] = AngleSubtract(v1[2], v2[2]);
}




















































// MOHAA: 
// TODO: Do something with these values so they don't get lost. (Important?)
void CL_ParseLocationprint(msg_t* msg, demoType_t demoType) {
	int x, y;
	char* string;

	x = MSG_ReadShort(msg);
	y = MSG_ReadShort(msg);
	//string = MSG_ReadScrambledString(msg);
	string = MSG_ReadString(msg, demoType);

	//UI_UpdateLocationPrint(x, y, string, 1.0);
}

void CL_ParseCenterprint(msg_t* msg, demoType_t demoType) {
	char* string;

	//string = MSG_ReadScrambledString(msg);
	string = MSG_ReadString(msg, demoType);

	// FIXME
	//UI_UpdateCenterPrint(string, 1.0);
}
#define MAX_IMPACTS              64
void CG_ParseCGMessage_ver_15(msg_t* msg, demoType_t demoType)
{
	int    i;
	int    iType;
	int    iLarge;
	int    iInfo;
	int    iCount;
	char* szTmp;
	char* s;
	vec3_t vStart, vEnd, vTmp;
	vec3_t vEndArray[MAX_IMPACTS];
	float  alpha;

	qboolean bMoreCGameMessages = qtrue;
	while (bMoreCGameMessages) {
		iType = MSG_ReadBits(msg,6);

		switch (iType) {
		case 1:
		case 2:
		case 5:
			if (iType == 1) {
				vTmp[0] = MSG_ReadCoord(msg);
				vTmp[1] = MSG_ReadCoord(msg);
				vTmp[2] = MSG_ReadCoord(msg);
			}
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);

			if (iType != 1) {
				vTmp[0] = vStart[0];
				vTmp[1] = vStart[1];
				vTmp[2] = vStart[2];
			}

			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,2);
			if (MSG_ReadBits(msg,1)) {
				int iAlpha = MSG_ReadBits(msg,10);
				alpha = (float)iAlpha / 512.0;
				if (alpha < 0.002f) {
					alpha = 0.002f;
				}
			}
			else {
				alpha = 1.0f;
			}

			if (iType == 1) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue, alpha);
			}
			else if (iType == 2) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue, alpha);
			}
			else {
				//CG_MakeBubbleTrail(vStart, vEndArray[0], iLarge, alpha);
			}

			break;
		case 3:
		case 4:
			if (iType == 3) {
				vTmp[0] = MSG_ReadCoord(msg);
				vTmp[1] = MSG_ReadCoord(msg);
				vTmp[2] = MSG_ReadCoord(msg);
				iInfo = MSG_ReadBits(msg,6);
			}
			else {
				iInfo = 0;
			}

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,2);
			if (MSG_ReadBits(msg,1)) {
				int iAlpha = MSG_ReadBits(msg,10);
				alpha = (float)iAlpha / 512.0;
				if (alpha < 0.002f) {
					alpha = 0.002f;
				}
			}
			else {
				alpha = 1.0f;
			}

			iCount = MSG_ReadBits(msg,6);
			for (i = 0; i < iCount; ++i) {
				vEndArray[i][0] = MSG_ReadCoord(msg);
				vEndArray[i][1] = MSG_ReadCoord(msg);
				vEndArray[i][2] = MSG_ReadCoord(msg);
			}

			if (iCount) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, iCount, iLarge, iInfo, qtrue, alpha);
			}
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			MSG_ReadDir(msg,vEnd);
			iLarge = MSG_ReadBits(msg,2);

			/*switch (iType) {
			case 6:
				if (wall_impact_count < MAX_IMPACTS) {
					VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
					VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
					wall_impact_large[wall_impact_count] = iLarge;
					wall_impact_type[wall_impact_count] = -1;
					wall_impact_count++;
				}
				break;
			case 7:
				if (wall_impact_count < MAX_IMPACTS) {
					VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
					VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
					wall_impact_large[wall_impact_count] = iLarge;
					wall_impact_type[wall_impact_count] = 6;
					wall_impact_count++;
				}
				break;
			case 8:
				if (flesh_impact_count < MAX_IMPACTS) {
					// negative
					VectorNegate(vEnd, vEnd);
					VectorCopy(vStart, flesh_impact_pos[flesh_impact_count]);
					VectorCopy(vEnd, flesh_impact_norm[flesh_impact_count]);
					flesh_impact_large[flesh_impact_count] = iLarge;
					flesh_impact_count++;
				}
				break;
			case 9:
				if (flesh_impact_count < MAX_IMPACTS) {
					// negative
					VectorNegate(vEnd, vEnd);
					VectorCopy(vStart, flesh_impact_pos[flesh_impact_count]);
					VectorCopy(vEnd, flesh_impact_norm[flesh_impact_count]);
					flesh_impact_large[flesh_impact_count] = iLarge;
					flesh_impact_count++;
				}
				break;
			case 10:
				if (wall_impact_count < MAX_IMPACTS) {
					VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
					VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
					wall_impact_large[wall_impact_count] = iLarge;
					wall_impact_type[wall_impact_count] = 2;
					wall_impact_count++;
				}
				break;
			case 11:
				if (wall_impact_count < MAX_IMPACTS) {
					VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
					VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
					wall_impact_large[wall_impact_count] = iLarge;
					wall_impact_type[wall_impact_count] = 4;
					wall_impact_count++;
				}
				break;
			default:
				continue;
			}*/
			break;

		case 12:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEnd[0] = MSG_ReadCoord(msg);
			vEnd[1] = MSG_ReadCoord(msg);
			vEnd[2] = MSG_ReadCoord(msg);
			//CG_MeleeImpact(vStart, vEnd);
			break;
		case 13:
		case 14:
		case 15:
		case 16:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			//CG_MakeExplosionEffect(vStart, iType);
			break;
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			MSG_ReadDir(msg,vEnd);

			//sfxManager.MakeEffect_Normal(iType + SFX_EXP_GREN_PUDDLE, vStart, vEnd);
			break;

		case 26:
		case 27:
		{
			//str    sEffect;
			char   cTmp[8];
			vec3_t axis[3];

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadByte(msg);
			// get the integer as string
			snprintf(cTmp, sizeof(cTmp), "%d", iLarge);

			if (iType == 26) {
				//sEffect = "models/fx/crates/debris_";
			}
			else {
				//sEffect = "models/fx/windows/debris_";
			}

			//sEffect += cTmp;
			//sEffect += ".tik";

			VectorSet(axis[0], 0, 0, 1);
			VectorSet(axis[1], 0, 1, 0);
			VectorSet(axis[2], 1, 0, 0);

			//cgi.R_SpawnEffectModel(sEffect.c_str(), vStart, axis);
		}
		break;

		case 28:
			vTmp[0] = MSG_ReadCoord(msg);
			vTmp[1] = MSG_ReadCoord(msg);
			vTmp[2] = MSG_ReadCoord(msg);
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,2);
			if (MSG_ReadBits(msg,1)) {
				int iAlpha = MSG_ReadBits(msg,10);
				alpha = (float)iAlpha / 512.0;
				if (alpha < 0.002f) {
					alpha = 0.002f;
				}
			}
			else {
				alpha = 1.0f;
			}

			//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qtrue, qtrue, alpha);
			break;

		case 29:
			memset(vTmp, 0, sizeof(vTmp));
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);
			if (MSG_ReadBits(msg,1)) {
				int iAlpha = MSG_ReadBits(msg,10);
				alpha = (float)iAlpha / 512.0;
				if (alpha < 0.002f) {
					alpha = 0.002f;
				}
			}
			else {
				alpha = 1.0f;
			}

			//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue, alpha);
			break;

		case 30:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadString(msg, demoType,qtrue);//strcpy(cgi.HudDrawElements[iInfo].shaderName, MSG_ReadString(msg));
			//cgi.HudDrawElements[iInfo].string[0] = 0;
			//cgi.HudDrawElements[iInfo].pFont = NULL;
			//cgi.HudDrawElements[iInfo].fontName[0] = 0;
			// set the shader
			//CG_HudDrawShader(iInfo);
			break;

		case 31:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadBits(msg, 2);//cgi.HudDrawElements[iInfo].iHorizontalAlign = MSG_ReadBits(msg,2);
			MSG_ReadBits(msg, 2);//cgi.HudDrawElements[iInfo].iVerticalAlign = MSG_ReadBits(msg,2);
			break;

		case 32:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iX = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iY = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iWidth = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iHeight = MSG_ReadShort(msg);
			break;

		case 33:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadBits(msg, 1);//cgi.HudDrawElements[iInfo].bVirtualScreen = MSG_ReadBits(msg,1);
			break;

		case 34:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[0] = MSG_ReadByte(msg) / 255.0;
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[1] = MSG_ReadByte(msg) / 255.0;
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[2] = MSG_ReadByte(msg) / 255.0;
			break;

		case 35:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[3] = MSG_ReadByte(msg) / 255.0;
			break;

		case 36:
			iInfo = MSG_ReadByte(msg);
			//cgi.HudDrawElements[iInfo].hShader = 0;
			s = MSG_ReadString(msg,demoType, qtrue);//strcpy(cgi.HudDrawElements[iInfo].string, MSG_ReadString(msg));
			break;

		case 37:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadString(msg, demoType, qtrue);//strcpy(cgi.HudDrawElements[iInfo].fontName, MSG_ReadString(msg));
			//cgi.HudDrawElements[iInfo].hShader = 0;
			//cgi.HudDrawElements[iInfo].shaderName[0] = 0;
			// load the font
			//CG_HudDrawFont(iInfo);
			break;

		case 38:
		case 39:
		{
			int iOldEnt;

			//iOldEnt = current_entity_number;
			//current_entity_number = cg.snap->ps.clientNum;
			//if (iType == 36) {
			//	commandManager.PlaySound("dm_kill_notify", NULL, CHAN_LOCAL, 2.0, -1, -1, 1);
			//}
			//else {
			//	commandManager.PlaySound("dm_hit_notify", NULL, CHAN_LOCAL, 2.0, -1, -1, 1);
			//}

			//current_entity_number = iOldEnt;
		}
		break;

		case 40:
		{
			int iOldEnt;

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);
			iInfo = MSG_ReadBits(msg,6);
			szTmp = MSG_ReadString(msg,demoType, qtrue);

			//iOldEnt = current_entity_number;

			//if (iLarge) {
			//	current_entity_number = iInfo;
			//
			//	commandManager.PlaySound(szTmp, vStart, CHAN_LOCAL, -1, -1, -1, 0);
			//}
			//else {
			//	current_entity_number = cg.snap->ps.clientNum;
			//
			//	commandManager.PlaySound(szTmp, vStart, CHAN_AUTO, -1, -1, -1, 1);
			//}
		
			//current_entity_number = iOldEnt;
		}
		break;
		case 41:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEnd[0] = MSG_ReadCoord(msg);
			vEnd[1] = MSG_ReadCoord(msg);
			vEnd[2] = MSG_ReadCoord(msg);
			MSG_ReadByte(msg);
			MSG_ReadByte(msg);
			VectorSubtract(vEnd, vStart, vTmp);

			// FIXME: unimplemented
			// ?? can't figure out what is this
			break;
		default:
			//cgi.Error(ERR_DROP, "CG_ParseCGMessage: Unknown CGM message type");
			break;
		}

		bMoreCGameMessages = (qboolean)MSG_ReadBits(msg,1);
	}
}

void CG_ParseCGMessage_ver_6(msg_t* msg, demoType_t demoType)
{
	int    i;
	int    iType;
	int    iLarge;
	int    iInfo;
	int    iCount;
	char* szTmp;
	vec3_t vStart, vEnd, vTmp;
	vec3_t vEndArray[MAX_IMPACTS];

	qboolean bMoreCGameMessages = qtrue;
	while (bMoreCGameMessages) {
		iType = MSG_ReadBits(msg,6);

		switch (iType) {
		case 1:
		case 2:
		case 5:
			if (iType == 1) {
				vTmp[0] = MSG_ReadCoord(msg);
				vTmp[1] = MSG_ReadCoord(msg);
				vTmp[2] = MSG_ReadCoord(msg);
			}
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);

			if (iType != 1) {
				vTmp[0] = vStart[0];
				vTmp[1] = vStart[1];
				vTmp[2] = vStart[2];
			}

			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);

			if (iType == 1) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue);
			}
			else if (iType == 2) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue);
			}
			else {
				//CG_MakeBubbleTrail(vStart, vEndArray[0], iLarge);
			}

			break;
		case 3:
		case 4:
			if (iType == 3) {
				vTmp[0] = MSG_ReadCoord(msg);
				vTmp[1] = MSG_ReadCoord(msg);
				vTmp[2] = MSG_ReadCoord(msg);
				iInfo = MSG_ReadBits(msg,6);
			}
			else {
				iInfo = 0;
			}

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);
			iCount = MSG_ReadBits(msg,6);
			for (i = 0; i < iCount; ++i) {
				vEndArray[i][0] = MSG_ReadCoord(msg);
				vEndArray[i][1] = MSG_ReadCoord(msg);
				vEndArray[i][2] = MSG_ReadCoord(msg);
			}

			if (iCount) {
				//CG_MakeBulletTracer(vTmp, vStart, vEndArray, iCount, iLarge, iInfo, qtrue);
			}
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			MSG_ReadDir(msg,vEnd);
			iLarge = MSG_ReadBits(msg,1);

			switch (iType) {
			case 6:
				//if (wall_impact_count < MAX_IMPACTS) {
				//	VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
				//	VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
				//	wall_impact_large[wall_impact_count] = iLarge;
				//	wall_impact_type[wall_impact_count] = 0;
				//	wall_impact_count++;
				//}
				break;
			case 7:
				//if (flesh_impact_count < MAX_IMPACTS) {
					// negative
				//	VectorNegate(vEnd, vEnd);
				//	VectorCopy(vStart, flesh_impact_pos[flesh_impact_count]);
				//	VectorCopy(vEnd, flesh_impact_norm[flesh_impact_count]);
				//	flesh_impact_large[flesh_impact_count] = iLarge;
				//	flesh_impact_count++;
				//}
				break;
			case 8:
				//if (flesh_impact_count < MAX_IMPACTS) {
					// negative
				//	VectorNegate(vEnd, vEnd);
				//	VectorCopy(vStart, flesh_impact_pos[flesh_impact_count]);
				//	VectorCopy(vEnd, flesh_impact_norm[flesh_impact_count]);
				//	flesh_impact_large[flesh_impact_count] = iLarge;
				//	flesh_impact_count++;
				//}
				break;
			case 9:
				//if (wall_impact_count < MAX_IMPACTS) {
				//	VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
				//	VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
				//	wall_impact_large[wall_impact_count] = iLarge;
				//	wall_impact_type[wall_impact_count] = (iLarge != 0) + 2;
				//	wall_impact_count++;
				//}
				break;
			case 10:
				//if (wall_impact_count < MAX_IMPACTS) {
				//	VectorCopy(vStart, wall_impact_pos[wall_impact_count]);
				//	VectorCopy(vEnd, wall_impact_norm[wall_impact_count]);
				//	wall_impact_large[wall_impact_count] = iLarge;
				//	wall_impact_type[wall_impact_count] = (iLarge != 0) + 4;
				//	wall_impact_count++;
				//}
				break;
			default:
				continue;
			}
			break;

		case 11:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEnd[0] = MSG_ReadCoord(msg);
			vEnd[1] = MSG_ReadCoord(msg);
			vEnd[2] = MSG_ReadCoord(msg);
			//CG_MeleeImpact(vStart, vEnd);
			break;
		case 12:
		case 13:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			//CG_MakeExplosionEffect(vStart, iType);
			break;
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			MSG_ReadDir(msg,vEnd);

			//sfxManager.MakeEffect_Normal(iType + SFX_EXP_GREN_PUDDLE, vStart, vEnd);
			break;

		case 23:
		case 24:
		{
			//str    sEffect;
			char   cTmp[8];
			vec3_t axis[3];

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadByte(msg);
			// get the integer as string
			snprintf(cTmp, sizeof(cTmp), "%d", iLarge);

			if (iType == 23) {
				//sEffect = "models/fx/crates/debris_";
			}
			else {
				//sEffect = "models/fx/windows/debris_";
			}

			//sEffect += cTmp;
			//sEffect += ".tik";

			VectorSet(axis[0], 0, 0, 1);
			VectorSet(axis[1], 0, 1, 0);
			VectorSet(axis[2], 1, 0, 0);

			//cgi.R_SpawnEffectModel(sEffect.c_str(), vStart, axis);
		}
		break;

		case 25:
			vTmp[0] = MSG_ReadCoord(msg);
			vTmp[1] = MSG_ReadCoord(msg);
			vTmp[2] = MSG_ReadCoord(msg);
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);

			//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qtrue, qtrue);
			break;

		case 26:
			memset(vTmp, 0, sizeof(vTmp));
			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			vEndArray[0][0] = MSG_ReadCoord(msg);
			vEndArray[0][1] = MSG_ReadCoord(msg);
			vEndArray[0][2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);

			//CG_MakeBulletTracer(vTmp, vStart, vEndArray, 1, iLarge, qfalse, qtrue);
			break;

		case 27:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadString(msg,demoType);//strcpy(cgi.HudDrawElements[iInfo].shaderName, MSG_ReadString());
			//cgi.HudDrawElements[iInfo].string[0] = 0;
			//cgi.HudDrawElements[iInfo].pFont = NULL;
			//cgi.HudDrawElements[iInfo].fontName[0] = 0;
			// set the shader
			//CG_HudDrawShader(iInfo);
			break;

		case 28:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadBits(msg, 2); //cgi.HudDrawElements[iInfo].iHorizontalAlign = MSG_ReadBits(msg,2);
			MSG_ReadBits(msg, 2);  // cgi.HudDrawElements[iInfo].iVerticalAlign = MSG_ReadBits(msg,2);
			break;

		case 29:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iX = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iY = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iWidth = MSG_ReadShort(msg);
			MSG_ReadShort(msg);//cgi.HudDrawElements[iInfo].iHeight = MSG_ReadShort(msg);
			break;

		case 30:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadBits(msg,1);//cgi.HudDrawElements[iInfo].bVirtualScreen = MSG_ReadBits(1);
			break;

		case 31:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[0] = MSG_ReadByte() / 255.0;
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[1] = MSG_ReadByte() / 255.0;
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[2] = MSG_ReadByte() / 255.0;
			break;

		case 32:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadByte(msg);//cgi.HudDrawElements[iInfo].vColor[3] = MSG_ReadByte() / 255.0;
			break;

		case 33:
			iInfo = MSG_ReadByte(msg);
			//cgi.HudDrawElements[iInfo].hShader = 0;
			MSG_ReadString(msg,demoType);//strcpy(cgi.HudDrawElements[iInfo].string, MSG_ReadString());
			break;

		case 34:
			iInfo = MSG_ReadByte(msg);
			MSG_ReadString(msg,demoType);//strcpy(cgi.HudDrawElements[iInfo].fontName, MSG_ReadString());
			//cgi.HudDrawElements[iInfo].hShader = 0;
			//cgi.HudDrawElements[iInfo].shaderName[0] = 0;
			// load the font
			//CG_HudDrawFont(iInfo);
			break;

		case 35:
		case 36:
		{
			//int iOldEnt;

			//iOldEnt = current_entity_number;
			//current_entity_number = cg.snap->ps.clientNum;
			//if (iType == 36) {
			//	commandManager.PlaySound("dm_kill_notify", NULL, CHAN_LOCAL, 2.0, -1, -1, 1);
			//}
			//else {
			//	commandManager.PlaySound("dm_hit_notify", NULL, CHAN_LOCAL, 2.0, -1, -1, 1);
			//}

			//current_entity_number = iOldEnt;
		}
		break;

		case 37:
		{
			int iOldEnt;

			vStart[0] = MSG_ReadCoord(msg);
			vStart[1] = MSG_ReadCoord(msg);
			vStart[2] = MSG_ReadCoord(msg);
			iLarge = MSG_ReadBits(msg,1);
			iInfo = MSG_ReadBits(msg,6);
			szTmp = MSG_ReadString(msg,demoType);

			//iOldEnt = current_entity_number;

			//if (iLarge) {
			//	current_entity_number = iInfo;
			//
			//	commandManager.PlaySound(szTmp, vStart, CHAN_LOCAL, -1, -1, -1, 0);
			//}
			//else {
			//	current_entity_number = cg.snap->ps.clientNum;
			//
			//	commandManager.PlaySound(szTmp, vStart, CHAN_AUTO, -1, -1, -1, 1);
			//}

			//current_entity_number = iOldEnt;
		}
		break;
		default:
			//cgi.Error(ERR_DROP, "CG_ParseCGMessage: Unknown CGM message type");
			break;
		}

		bMoreCGameMessages = (qboolean)MSG_ReadBits(msg,1);
	}
}

void CL_ParseCGMessageMOHAA(msg_t* msg, demoType_t demoType) {
	//cl_currentMSG = msg;
	//cge->CG_ParseCGMessage();
	if (demoType == DM3_MOHAA_PROT_15) {
		CG_ParseCGMessage_ver_15(msg, demoType);
	}
	else {
		CG_ParseCGMessage_ver_6(msg, demoType);
	}

}

qboolean demoCutParseMOHAASVCReal(msg_t* msg, demoType_t demoType, byte cmd) {
	switch (cmd) {

	case svc_centerprint_general:
		CL_ParseCenterprint(msg, demoType);
		break;
	case svc_locprint_general:
		CL_ParseLocationprint(msg, demoType);
		break;
	case svc_cgameMessage_general:
		CL_ParseCGMessageMOHAA(msg, demoType);
		break;
	}
	return qtrue;
}
qboolean demoCutParseMOHAASVC(msg_t* msg, demoType_t demoType, byte cmd, bool& SEHExceptionCaught) {
	__TRY{
		return demoCutParseMOHAASVCReal(msg,demoType,cmd);
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
}
//
//
// MOHAA end
//
//


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
static inline qboolean demoCutParseGamestateReal(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t* demoType, qboolean isDemoHeader) {
	int				i;
	entityState_t* es;
	int				newnum;
	entityState_t	nullstate;
	int				cmd,ocmd;
	char* s;

	bool isMOHAADemo = demoTypeIsMOHAA(*demoType);

	//int svc_EOF_realCMD = *demoType == DM_26 ? svc_EOF + 1 : svc_EOF;
	//int maxAllowedConfigString = *demoType == DM_26 ? MAX_CONFIGSTRINGS_JKA : MAX_CONFIGSTRINGS;
	int maxAllowedConfigString = getMaxConfigStrings(*demoType);

	clcCut->connectPacketCount = 0;

	if (!isMOHAADemo) {
		Com_Memset(clCut, 0, sizeof(*clCut));
	}
	else {
		// CRINGE
		// I'm doing something wrong I think
		// MOHAA demos just keep referencing old snaps, idk why. So I need to keep that info
		// until I figure out wtf is going on.
		Com_Memset(&clCut->gameState, 0, sizeof(clCut->gameState));
	}

	clcCut->serverCommandSequence = MSG_ReadLong(msg);
	clCut->gameState.dataCount = 1;
	while (1) {
		ocmd = MSG_ReadByte(msg);
		cmd = generalizeGameSVCOp(ocmd,*demoType);
		if (cmd == svc_EOF_general) {
			break;
		}
		if (cmd == svc_configstring_general) {
			int len, start;
			qboolean oobOld;
			int bitOld, readCountOld;
			start = msg->readcount;
			i = MSG_ReadShort(msg);
			if (i < 0 || i >= maxAllowedConfigString) {
				Com_DPrintf("configstring > MAX_CONFIGSTRINGS");
				return qfalse;
			}
			if (i == 0 && *demoType == DM3_MOHAA_PROT_6) {
				oobOld = msg->oob;
				bitOld = msg->bit;
				readCountOld = msg->readcount;
			}
configStringReadRetry:
			s = MSG_ReadBigString(msg, *demoType);
			len = strlen(s);
			if (len + 1 + clCut->gameState.dataCount > MAX_GAMESTATE_CHARS) {
				Com_DPrintf("MAX_GAMESTATE_CHARS exceeded");
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
			if (clcCut->demoCheckProtocol && i == CS_SERVERINFO && *demoType == DM3_MOHAA_PROT_6) {
				// Medal of Honor .dm3 file format. Check protocol version because the demo file ending is same for all.
				// This is produced by stock MOH with unlocked demo functionality. OpenMOHAA has different naming.
				char* protocolString = Info_ValueForKey(s, BIG_INFO_STRING, "protocol");
				if (*protocolString == 0) {
					// This might be spearhead/breakthrough demo, in which case all we get here is garbled nonsense and obviously no value for protocol, since string reading/writing uses a funny lookup table.
					// All we can try is set directly to protocol15/16/17 and retry string reading and hope it works.
					msg->oob = oobOld;
					msg->bit = bitOld;
					msg->readcount = readCountOld;
					*demoType = DM3_MOHAA_PROT_15;
					goto configStringReadRetry;
				}
				int protocol = atoi(protocolString);
				switch (protocol) { // I guess this ends up being pointless because protocol 15-17 uses a different string reading thingie and we get a garbled string here.
					case 0:
						throw std::logic_error("MOH demo without protocol in CS_SERVERINFO");
					case 6:
					case 7:
					case 8:
						*demoType = DM3_MOHAA_PROT_6;
						break;
					case 15:
					case 16:
					case 17:
						*demoType = DM3_MOHAA_PROT_15;
						break;
					default:
						throw std::logic_error(va("MOH demo with unknown protocol %s", protocolString));
						break;
				}

				clcCut->demoCheckProtocol = qfalse; //No need to check this again while playing the demo.
			}

			if (GlobalDebugOutputFlags & (1 << DEBUG_CONFIGSTRING)) {
				std::cerr << "CONFIGSTRING DEBUG (GAMESTATE): serverTime "<< clCut->snap.serverTime << ", index " << i << ": " << s << "\n";
			}

			// append it to the gameState string buffer
			clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
			Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, s, len + 1);
			clCut->gameState.dataCount += len + 1;
		}
		else if (cmd == svc_baseline_general) {
			if (isMOHAADemo) {
				newnum = MSG_ReadEntityNum(msg, *demoType);
			}
			else {
				newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
			}
			if (newnum < 0 || newnum >= MAX_GENTITIES) {
				Com_DPrintf("Baseline number out of range: %i", newnum);
				return qfalse;
			}
			if (isMOHAADemo) {
				MSG_GetNullEntityState(&nullstate);
			}
			else {
				Com_Memset(&nullstate, 0, sizeof(nullstate));
			}
			es = &clCut->entityBaselines[newnum];
			// TODO Does this make sense with serverFrameTime? OpenMOHAA just uses 0, but the actual engine uses the reference to serverFrameTime. idk. it probably just ends up being 0 for the first gamestate.
			MSG_ReadDeltaEntity(msg, &nullstate, es, newnum, *demoType, clCut->serverFrameTime);
		}
		else {
			Com_DPrintf("demoCutParseGameState: bad command byte %d (%d)", cmd, ocmd);
			return qfalse;
		}
	}
	clcCut->clientNum = MSG_ReadLong(msg);
	clcCut->checksumFeed = MSG_ReadLong(msg);

	if (isMOHAADemo) {
		// Important note for DM3_MOHAA_PROT_15:
		// If this is a real gamestate, serverFrameTime will yield a reasonable value.
		// If however this is a gamestate written as a demo header in CL_Record_f,
		// it will be missing this value and we get nonsense data here.
		// Probably an oversight of the developers as they never intended to have demo recording
		// really stable or functional. 
		// Hence we pass the fourth argument "forceConfigStringMethod" if this is a demo header,
		// but read normally if it's not (then it's a normal gamestate with that value intact).
		//clCut->serverFrameTime = MSG_ReadServerFrameTime(msg, *demoType, clCut, isDemoHeader);

		// Actually, I changed my mind. We just force a read here even if it may not be valid, and catch the possible "read past end of message" style event.
		qboolean oldOOB = msg->oob;
		int oldBit = msg->bit;
		int oldReadCount = msg->readcount;
		clCut->serverFrameTime = MSG_ReadServerFrameTime(msg, *demoType, clCut, qfalse);
		if (isDemoHeader && msg->readcount > msg->cursize) {
			// Ok this is confirmed one of the old school "missing server frame time" demo headers as the stock game writes them.
			msg->oob = oldOOB;
			msg->bit = oldBit;
			msg->readcount = oldReadCount;
			clCut->serverFrameTime = MSG_ReadServerFrameTime(msg, *demoType, clCut, qtrue); // Get the correct value from sv_fps instead
		}
	}

	// RMG stuff (JKA specific)
	if (*demoType == DM_26 || *demoType == DM_25) {
		demoCutParseRMG(msg, clcCut, clCut);
	}

	return qtrue;
}
qboolean demoCutParseGamestate(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t* demoType, qboolean isDemoHeader, bool& SEHExceptionCaught) {
	__TRY{
		return demoCutParseGamestateReal(msg,clcCut,clCut,demoType,isDemoHeader);
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
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
	if (demoType == DM3_MOHAA_PROT_15) {
		newnum = (unsigned short)(newnum - 1) % MAX_GENTITIES; // Idk why tf but that's how it is apparently.
	}
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
		newstate = &clCut->parseEntities[clCut->parseEntitiesNum & (MAX_PARSE_ENTITIES - 1)];
		if (!oldstate && (newnum == (MAX_GENTITIES - 1))) {
			break;
		}
		else if (oldnum < newnum) {
			*newstate = *oldstate;
			oldindex++;
		}
		else if (oldnum == newnum) {
			oldindex++;
			MSG_ReadDeltaEntity(msg, oldstate, newstate, newnum, demoType, clCut->serverFrameTime); // serverFrameTime stuff is MOHAA
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
			if (demoType == DM3_MOHAA_PROT_15) {
				newnum = (unsigned short)(newnum - 1) % MAX_GENTITIES; // Idk why tf but that's how it is apparently.
			}
		}
		else if (oldnum > newnum) {
			MSG_ReadDeltaEntity(msg, &clCut->entityBaselines[newnum], newstate, newnum, demoType, clCut->serverFrameTime); // serverFrameTime stuff is MOHAA
			newnum = MSG_ReadBits(msg, GENTITYNUM_BITS);
			if (demoType == DM3_MOHAA_PROT_15) {
				newnum = (unsigned short)(newnum - 1) % MAX_GENTITIES; // Idk why tf but that's how it is apparently.
			}
		}
		if (newstate->number == MAX_GENTITIES - 1)
			continue;
		clCut->parseEntitiesNum++;
		//clCut->parseEntitiesNum &= (MAX_PARSE_ENTITIES - 1);
		newSnap->numEntities++;
	}
}

static inline void demoCutParseCommandStringReal(msg_t* msg, clientConnection_t* clcCut, demoType_t demoType) {
	int index;
	int seq = MSG_ReadLong(msg);
	char* s = MSG_ReadString(msg, demoType);
	if (clcCut->serverCommandSequence >= seq) {
		return;
	}
	clcCut->serverCommandSequence = seq;
	index = seq & (MAX_RELIABLE_COMMANDS - 1);
	Q_strncpyz(clcCut->serverCommands[index], MAX_STRING_CHARS_MAX, s, sizeof(clcCut->serverCommands[index]));
	if (GlobalDebugOutputFlags & (1 << DEBUG_COMMANDS)) {
		std::cerr << "COMMAND DEBUG: demotime " << GlobalDebugDemoTime << ": cmd " << seq << ": " << s << "\n";
	}
}
qboolean demoCutParseCommandString(msg_t* msg, clientConnection_t* clcCut, demoType_t demoType, bool& SEHExceptionCaught) {
	__TRY{
		demoCutParseCommandStringReal(msg,clcCut, demoType);
		return qtrue;
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
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
		Com_DPrintf("demoCutConfigstringModified: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = Cmd_ArgsFrom(2);
	old = clCut->gameState.stringData + clCut->gameState.stringOffsets[index];
	if (!strcmp(old, s)) {
		return qtrue; // unchanged
	}

	if (GlobalDebugOutputFlags & (1 << DEBUG_CONFIGSTRING)) {
		std::cerr << "CONFIGSTRING DEBUG (CONFIGSTRINGMODIFIED): serverTime " << clCut->snap.serverTime << ", index " << index << ": " << s << "\n";
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
			Com_DPrintf("MAX_GAMESTATE_CHARS exceeded on %d:%s", index, s);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}

static inline qboolean demoCutParseSnapshotReal(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean writeOldSnap, bool& malformedMessage) {
	int len;
	clSnapshot_t* oldSnap;
	clSnapshot_t newSnap;
	int deltaNum;
	int oldMessageNum;
	int i, packetNum;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	Com_Memset(&newSnap, 0, sizeof(newSnap));
	clCut->lastSnapshotFinishedParsing = qfalse;
	newSnap.serverCommandNum = clcCut->serverCommandSequence;
	newSnap.serverTime = MSG_ReadLong(msg);
	if (isMOHAADemo) {
		newSnap.serverTimeResidual = MSG_ReadByte(msg);
	}
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
			Com_DPrintf("Delta from invalid frame (not supposed to happen!). Trying to use last valid snap.\n");
		}

		if (!oldSnap->valid) {
			// should never happen
			Com_DPrintf("Delta from invalid frame (not supposed to happen!).\n");
		}
		//else if (oldSnap->messageNum != newSnap.deltaNum) {
		else if (oldSnap->messageNum != lastOneNum) {
			// The frame that the server did the delta from
			// is too old, so we can't reconstruct it properly.
			Com_DPrintf("Delta frame too old.\n");
		}
		else if (clCut->parseEntitiesNum - oldSnap->parseEntitiesNum > MAX_PARSE_ENTITIES - 128) { // is this reliable? assumes max of 128 entities in packet?
			Com_DPrintf("Delta parseEntitiesNum too old.\n");
		}
		else {
			newSnap.valid = qtrue;	// valid delta parse
		}
	}
	try {
		// read areamask
		len = MSG_ReadByte(msg);
		//if (len > sizeof(newSnap.areamask)) {
		//	Com_Printf("demoCutParseSnapshot: Invalid size %d for areamask", len);
		//	return qfalse;
		//}
		MSG_ReadData(msg, &newSnap.areamask, len);
		// read playerinfo
		MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->ps : NULL, &newSnap.ps, demoType, qfalse, &clCut->snap.ps);

		// JKA-specific
		if ((demoType == DM_26 || demoType == DM_25) && newSnap.ps.m_iVehicleNum)
			MSG_ReadDeltaPlayerstate(msg, oldSnap ? &oldSnap->vps : NULL, &newSnap.vps, demoType, qtrue, &clCut->snap.vps);

		// read packet entities
		demoCutParsePacketEntities(msg, oldSnap, &newSnap, clCut, demoType);

		if (isMOHAADemo) {
			MSG_ReadSounds(msg, newSnap.sounds, &newSnap.number_of_sounds);
		}

	}
	catch (malformed_message_exception e) {
		std::cout << "Malformed message caught. Trying to skip. (" << e.what() << ")\n";
		malformedMessage = true;
		return qfalse;
	}
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
qboolean demoCutParseSnapshot(msg_t* msg, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, bool& SEHExceptionCaught, bool& malformedMessage, qboolean writeOldSnap) {
	__TRY{
		return demoCutParseSnapshotReal(msg,clcCut,clCut,demoType,writeOldSnap, malformedMessage);
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
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
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, demoType, clCut->serverFrameTime);
			oldindex++;
			newindex++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue, demoType, clCut->serverFrameTime);
			newindex++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, demoType, clCut->serverFrameTime);
			oldindex++;
			continue;
		}
	}
	MSG_WriteBits(msg, demoType == DM3_MOHAA_PROT_15 ? (MAX_GENTITIES % MAX_GENTITIES) : (MAX_GENTITIES - 1), GENTITYNUM_BITS);	// end of packetentities
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

constexpr int strlenConstExpr(const char* txt) {
	int count = 0;
	while (*txt != 0) {
		count++;
		txt++;
	}
	return count;
}

void demoCutWriteEmptyMessageWithMetadata(fileHandle_t f, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, qboolean raw, const char* metaData) {
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
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_EOF_general,demoType));

	// Normal demo readers will quit here. For all intents and purposes this demo message is over. But we're gonna put the metadata here now. Since it comes after svc_EOF, nobody will ever be bothered by it 
	// but we can read it if we want to.
	constexpr int metaMarkerLength = strlenConstExpr(postEOFMetadataMarker);
	// This is how the demo huffman operates. Worst case a byte can take almost 2 bytes to save, from what I understand. When reading past the end, we need to detect if we SHOULD read past the end.
	// For each byte we need to read, thus, the message length must be at least 2 bytes longer still. Hence at the end we will artificially set the message length to be minimum that long.
	// We will only read x amount of bytes (where x is the length of the meta marker) and see if the meta marker is present. If it is, we then proceeed to read a bigstring.
	// This same thing is technically not true for the custom compressed types (as their size is always the real size of the data) but we'll just leave it like this to be universal and simple.
	constexpr int maxBytePerByteSaved = 2; 
	constexpr int metaMarkerPresenceMinimumByteLengthExtra = metaMarkerLength * maxBytePerByteSaved;

	const int requiredCursize = buf.cursize + metaMarkerPresenceMinimumByteLengthExtra; // We'll just set it to this value at the end if it ends up smaller.

	for (int i = 0; i < metaMarkerLength; i++) {
		MSG_WriteByte(&buf,postEOFMetadataMarker[i]);
	}
	MSG_WriteBigString(&buf, metaData, demoType == DM3_MOHAA_PROT_15 ? DM3_MOHAA_PROT_6 : demoType); // For Spearheard/Breakthrough we might need scrambling, but at loading time we don't even know if it's one of those yet, so it would make it impossible to decide.


	// finished writing the client packet
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_EOF_general, demoType)); // Done. Not really needed but whatever.

	if (buf.cursize < requiredCursize) {
		buf.cursize = requiredCursize;
	}

	// write it to the demo file
	len = LittleLong(clcCut->serverMessageSequence - 2);
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

const char* demoCutReadPossibleMetadata(msg_t* msg, demoType_t demoType) {

	// Normal demo readers will quit here. For all intents and purposes this demo message is over. But we're gonna put the metadata here now. Since it comes after svc_EOF, nobody will ever be bothered by it 
	// but we can read it if we want to.
	constexpr int metaMarkerLength = strlenConstExpr(postEOFMetadataMarker);
	// This is how the demo huffman operates. Worst case a byte can take almost 2 bytes to save, from what I understand. When reading past the end, we need to detect if we SHOULD read past the end.
	// For each byte we need to read, thus, the message length must be at least 2 bytes longer still. Hence at the end we will artificially set the message length to be minimum that long.
	// We will only read x amount of bytes (where x is the length of the meta marker) and see if the meta marker is present. If it is, we then proceeed to read a bigstring.
	// This same thing is technically not true for the custom compressed types (as their size is always the real size of the data) but we'll just leave it like this to be universal and simple.
	constexpr int maxBytePerByteSaved = 2; 
	constexpr int metaMarkerPresenceMinimumByteLengthExtra = metaMarkerLength * maxBytePerByteSaved;

	const int requiredCursize = msg->readcount + metaMarkerPresenceMinimumByteLengthExtra; // We'll just set it to this value at the end if it ends up smaller.

	if (msg->cursize < requiredCursize) {
		return NULL;
	}

	int oldbit = msg->bit;
	int oldreadcount = msg->readcount;

	for (int i = 0; i < metaMarkerLength; i++) {
		if (msg->cursize < msg->readcount + maxBytePerByteSaved)
		{
			msg->bit = oldbit;
			msg->readcount = oldreadcount;
			return NULL;
		}
		if (MSG_ReadByte(msg) != postEOFMetadataMarker[i]) {
			msg->bit = oldbit;
			msg->readcount = oldreadcount;
			return NULL;
		}
	}
	return MSG_ReadBigString(msg, demoType);
}

static qboolean demoCutReadPossibleHiddenUserCMDsReal(msg_t* msg, demoType_t demoType) {

	usercmd_t	nullcmd;

	// Normal demo readers will quit here. For all intents and purposes this demo message is over. But we're gonna put the metadata here now. Since it comes after svc_EOF, nobody will ever be bothered by it 
	// but we can read it if we want to.
	constexpr int metaMarkerLength = strlenConstExpr(postEOFUcmdMarker);
	// This is how the demo huffman operates. Worst case a byte can take almost 2 bytes to save, from what I understand. When reading past the end, we need to detect if we SHOULD read past the end.
	// For each byte we need to read, thus, the message length must be at least 2 bytes longer still. Hence at the end we will artificially set the message length to be minimum that long.
	// We will only read x amount of bytes (where x is the length of the meta marker) and see if the meta marker is present. If it is, we then proceeed to read a bigstring.
	// This same thing is technically not true for the custom compressed types (as their size is always the real size of the data) but we'll just leave it like this to be universal and simple.
	constexpr int maxBytePerByteSaved = 2; 
	constexpr int metaMarkerPresenceMinimumByteLengthExtra = metaMarkerLength * maxBytePerByteSaved;

	const int requiredCursize = msg->readcount + metaMarkerPresenceMinimumByteLengthExtra; // We'll just set it to this value at the end if it ends up smaller.

	if (msg->cursize < requiredCursize) {
		return qfalse;
	}

	int oldbit = msg->bit;
	int oldreadcount = msg->readcount;

	for (int i = 0; i < metaMarkerLength; i++) {
		if (msg->cursize < msg->readcount + maxBytePerByteSaved)
		{
			msg->bit = oldbit;
			msg->readcount = oldreadcount;
			return qfalse;
		}
		if (MSG_ReadByte(msg) != postEOFUcmdMarker[i]) {
			msg->bit = oldbit;
			msg->readcount = oldreadcount;
			return qfalse;
		}
	}

	Com_Memset(&nullcmd, 0, sizeof(nullcmd));

	int version = MSG_ReadBits(msg, 16);
	if (GlobalDebugOutputFlags & (1 << DEBUG_HIDDENUSERCMD)) {
		std::cerr << "Found hidden usermessage marker, version " << version << "\n";
	}
	usercmd_t cmd, oldcmd;
	oldcmd = nullcmd;
	while (MSG_ReadBits(msg,1)) {
		int clientNum = MSG_ReadByte(msg);
		if (GlobalDebugOutputFlags & (1 << DEBUG_HIDDENUSERCMD)) {
			std::cerr << "New usermessage PRE; readcount " << msg->readcount << ", clientnum "<< clientNum <<"\n";
		}
		int serverTimeOffset = MSG_ReadBits(msg, 1) ? MSG_ReadBits(msg, -16) : 0;
		int ping = MSG_ReadBits(msg, 1) ? MSG_ReadBits(msg, -16) : 0;
		int droppedPackets = MSG_ReadBits(msg, 1) ? MSG_ReadByte(msg) : 0;
		if (GlobalDebugOutputFlags & (1 << DEBUG_HIDDENUSERCMD)) {
			std::cerr << "New usermessage; readcount " << msg->readcount << "; servertimeoffset  " << serverTimeOffset << ", ping " << ping << ", droppedPackets " << droppedPackets << "\n";
		}

		if (version == 1) {
			oldcmd = nullcmd; // first prototype did this on every new msg but its wasteful..
		}
		int index = 0;
		while (MSG_ReadBits(msg, 1)) {
			MSG_ReadDeltaUsercmdKey(msg, 0, &oldcmd, &cmd);
			oldcmd = cmd;
			if (GlobalDebugOutputFlags & (1 << DEBUG_HIDDENUSERCMD)) {
				std::cerr << "New usercmd; readcount " << msg->readcount << "; serverTime  " << cmd.serverTime << ", angles[0] " << (int)cmd.angles[0]
					<< ", angles[1] " << (int)cmd.angles[1]
					<< ", angles[2] " << (int)cmd.angles[2]
					<< ", buttons " << (int)cmd.buttons
					<< ", weapon " << (int)cmd.weapon
					<< ", forcesel " << (int)cmd.forcesel
					<< ", invensel " << (int)cmd.invensel
					<< ", generic_cmd " << (int)cmd.generic_cmd
					<< ", forwardmove " << (int)cmd.forwardmove
					<< ", rightmove " << (int)cmd.rightmove
					<< ", upmove " << (int)cmd.upmove
					<< "\n";
				if (index == 0) {
					std::cerr << "Servertime offset solved: "<< cmd.serverTime << " + " << serverTimeOffset << "  =  " << (cmd.serverTime+serverTimeOffset) << "\n";
				}
			}
			index++;
		}
	}

	return qtrue;
}

qboolean demoCutReadPossibleHiddenUserCMDs(msg_t* msg, demoType_t demoType, bool& SEHExceptionCaught) {
	__TRY{
		return demoCutReadPossibleHiddenUserCMDsReal(msg,demoType);
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
}

// In cut demos, metadata gets added ___ at the start, but we may still need the value.
const char* jsonGetRealMetadataKeyName(rapidjson::Document* doc, const char* searchName) {
	if (doc->HasMember(searchName)) {
		return searchName;
	}
	else {
		for (rapidjson::Value::MemberIterator it = doc->MemberBegin(); it != doc->MemberEnd(); it++) {

			const char* nameHere = it->name.GetString();

			if (nameHere) {
				while (*nameHere == '_') {
					nameHere++; 
				}
				if (*nameHere && !stricmp(searchName,nameHere)) {
					std::cout << "Found metadata member " << searchName << " as " << it->name.GetString() << "\n";
					return it->name.GetString();
				}
			}

		}
	}
	return NULL;
}



std::string printRapidJsonValue(rapidjson::Value* value) {
	if (value->IsString()) {
		return value->GetString();
	}
	else {
		std::stringstream ss;
		if (value->IsDouble()) {
			ss << value->GetDouble();
		}
		else if (value->IsBool()) {
			ss << value->GetBool();
		}
		else if (value->IsInt64()) {
			ss << value->GetInt64();
		}
		else {
			ss << "unhandled json type";
		}
		return ss.str();
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

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

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
		MSG_WriteBigString(&buf, s, demoType);
	}
	// baselines
	if (isMOHAADemo) {
		MSG_GetNullEntityState(&nullstate);
	}
	else {
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}
	for (i = 0; i < MAX_GENTITIES; i++) {
		ent = &clCut->entityBaselines[i];
		if (!ent->number) {
			continue;
		}
		MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_baseline_general, demoType));
		MSG_WriteDeltaEntity(&buf, &nullstate, ent, qtrue, demoType, clCut->serverFrameTime); // Is this correct? The serverFrameTime thing?
	}
	MSG_WriteByte(&buf, specializeGeneralSVCOp(svc_EOF_general, demoType));
	// finished writing the gamestate stuff
	// write the client num
	MSG_WriteLong(&buf, clcCut->clientNum);
	// write the checksum feed
	MSG_WriteLong(&buf, clcCut->checksumFeed);

	if (demoType == DM3_MOHAA_PROT_15) {
		// This is a break with the stock game,
		// but the stock game's behavior is actually faulty,
		// it writes invalid gamestate missing this part,
		// causing either read past end of message when trying to read it,
		// or leading to "invalid command byte" style errors if we DON'T read it
		// despite it being there.
		// Just do the right thing.
		MSG_WriteFloat(&buf,clCut->serverFrameTime);
	}

	// This is supposed to be part of gamestate
	// but it doesn't seem like the normal game writes this for the demo header either
	// I guess just stay compatible here and do it the "wrong" way to not lead to a
	// confusing amalgamation of demo formats?
	//if (demoType == DM3_MOHAA_PROT_15) { 
	//	MSG_WriteFloat(&buf, clCut->serverFrameTime);
	//}

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

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

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
		if (serverCommand < clcCut->serverCommandSequence - (MAX_RELIABLE_COMMANDS - 1)) {
			int lowestPossible = clcCut->serverCommandSequence - (MAX_RELIABLE_COMMANDS - 1);
			if (serverCommand != 0) {
				std::cout << "WARNING: demoCutWriteDeltaSnapshot: serverCommand number " << serverCommand << " is lower than the buffer remembers. Skipping to " << lowestPossible << "\n";
			}
			else {
				std::cout << "NOTE: demoCutWriteDeltaSnapshot: serverCommand number " << serverCommand << " is lower than the buffer remembers. Probably due to being start of demo. Skipping to " << lowestPossible << "\n";
			}
			serverCommand = lowestPossible;
		}
		char* command = clcCut->serverCommands[serverCommand & (MAX_RELIABLE_COMMANDS - 1)];
		if (*command) { // firstServerCommand can be 0 at the start, so we have to loop through a lot of empty entries first. No use writing them.
			MSG_WriteByte(msg, specializeGeneralSVCOp(svc_serverCommand_general, demoType));
			MSG_WriteLong(msg, serverCommand/* + serverCommandOffset*/);
			MSG_WriteString(msg, command, demoType);
		}
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

	if (isMOHAADemo) {
		if (frame->serverTimeResidual > 254) // TODO Is this handled well here/elsewhere? idk
			MSG_WriteByte(msg, 255);
		else MSG_WriteByte(msg, frame->serverTimeResidual);
	}

	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (oldframe) {
		MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, &frame->ps,qfalse, demoType, clCut->serverFrameTime);
		if (demoType == DM_26 || demoType == DM_25) {
			if (frame->ps.m_iVehicleNum) {
				//then write the vehicle's playerstate too
				if (!oldframe->ps.m_iVehicleNum) {
					//if last frame didn't have vehicle, then the old vps isn't gonna delta
					//properly (because our vps on the client could be anything)
					MSG_WriteDeltaPlayerstate(msg, NULL, &frame->vps, qtrue, demoType, clCut->serverFrameTime);
				}
				else {
					MSG_WriteDeltaPlayerstate(msg, &oldframe->vps, &frame->vps, qtrue, demoType, clCut->serverFrameTime);
				}
			}
		}
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps,qfalse, demoType, clCut->serverFrameTime);
		if (demoType == DM_26 || demoType == DM_25) {
			if (frame->ps.m_iVehicleNum) {
				//then write the vehicle's playerstate too
				MSG_WriteDeltaPlayerstate(msg, NULL, &frame->vps, qtrue,demoType, clCut->serverFrameTime);
			}
		}
	}
	// delta encode the entities
	demoCutEmitPacketEntities(oldframe, frame, msg, clCut, demoType);

	if (isMOHAADemo) {
		MSG_WriteSounds(msg, frame->sounds, frame->number_of_sounds);

		// TODO Maybe do this? Or naw? Not that important for now I guess
		/*if (client->centerprint) {
			if (client->locprint) {
				MSG_WriteSVC(msg, svc_locprint);
				MSG_WriteShort(msg, client->XOffset);
				MSG_WriteShort(msg, client->YOffset);
				MSG_WriteScrambledString(msg, client->centerprint);
			}
			else {
				MSG_WriteSVC(msg, svc_centerprint);
				MSG_WriteScrambledString(msg, client->centerprint);
			}
		}*/
	}

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
		Com_DPrintf("demoCutConfigstringModifiedManual: bad index %i", index);
		return qtrue;
	}
	// get everything after "cs <num>"
	s = value;
	old = clCut->gameState.stringData + clCut->gameState.stringOffsets[index];
	if (!strcmp(old, s)) {
		return qtrue; // unchanged
	}

	if (GlobalDebugOutputFlags & (1 << DEBUG_CONFIGSTRING)) {
		std::cerr << "CONFIGSTRING DEBUG (CONFIGSTRINGMODIFIEDMANUAL): serverTime " << clCut->snap.serverTime << ", index " << configStringNum << ": " << value << "\n";
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
			Com_DPrintf("MAX_GAMESTATE_CHARS exceeded on %d:%s", configStringNum, value);
			return qfalse;
		}
		// append it to the gameState string buffer
		clCut->gameState.stringOffsets[i] = clCut->gameState.dataCount;
		Com_Memcpy(clCut->gameState.stringData + clCut->gameState.dataCount, dup, len + 1);
		clCut->gameState.dataCount += len + 1;
	}
	return qtrue;
}



static SnapshotEntitiesOrderedPointers emptyEntities;

void demoCutEmitPacketEntitiesManual(msg_t* msg, clientActive_t* clCut, demoType_t demoType, SnapshotEntitiesOrderedPointers* entities, SnapshotEntitiesOrderedPointers* fromEntities) {
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
			newent = newIterator->second;
		}
		if (oldindex >= from_num_entities) {
			oldnum = 9999;
		}
		else {
			//oldent = &clCut->parseEntities[(from->parseEntitiesNum + oldindex) & (MAX_PARSE_ENTITIES - 1)];
			//oldnum = oldent->number;
			oldent = oldIterator->second;
			oldnum = oldIterator->first;
		}
		if (newnum == oldnum) {
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse, demoType, clCut->serverFrameTime);
			oldindex++;
			oldIterator++;
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum < oldnum) {
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &clCut->entityBaselines[newnum], newent, qtrue,demoType, clCut->serverFrameTime);
			newindex++;
			newIterator++;
			continue;
		}
		if (newnum > oldnum) {
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue, demoType, clCut->serverFrameTime);
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
void demoCutWriteDeltaSnapshotManual(std::vector<std::string>* newCommands, fileHandle_t f, qboolean forceNonDelta, clientConnection_t* clcCut, clientActive_t* clCut, demoType_t demoType, SnapshotEntitiesOrderedPointers* entities, SnapshotEntitiesOrderedPointers* fromEntities, playerState_t* fromPS,qboolean raw) {
	msg_t			msgImpl, * msg = &msgImpl;
	byte			msgData[MAX_MSGLEN];
	std::vector<byte>			msgDataRaw;
	qboolean		doDelta = qfalse;
	clSnapshot_t* frame;// , * oldframe;
	int				lastframe = 0;
	int				snapFlags;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

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
		MSG_WriteString(msg, (*newCommands)[i].c_str(), demoType);
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

	if (isMOHAADemo) {
		if (frame->serverTimeResidual > 254) // TODO Is this handled well here/elsewhere? idk
			MSG_WriteByte(msg, 255);
		else MSG_WriteByte(msg, frame->serverTimeResidual);
	}

	// what we are delta'ing from
	MSG_WriteByte(msg, lastframe);
	snapFlags = frame->snapFlags;
	MSG_WriteByte(msg, snapFlags);
	// send over the areabits
	MSG_WriteByte(msg, sizeof(frame->areamask));
	MSG_WriteData(msg, frame->areamask, sizeof(frame->areamask));
	// delta encode the playerstate
	if (doDelta) {
		MSG_WriteDeltaPlayerstate(msg, fromPS, &frame->ps,qfalse, demoType, clCut->serverFrameTime);
	}
	else {
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps,qfalse,demoType, clCut->serverFrameTime);
	}
	// delta encode the entities
	demoCutEmitPacketEntitiesManual(msg, clCut, demoType, entities, fromEntities);

	if (isMOHAADemo) {
		MSG_WriteSounds(msg, frame->sounds, frame->number_of_sounds);

		// TODO Maybe do this? Or naw? Not that important for now I guess
		/*if (client->centerprint) {
			if (client->locprint) {
				MSG_WriteSVC(msg, svc_locprint);
				MSG_WriteShort(msg, client->XOffset);
				MSG_WriteShort(msg, client->YOffset);
				MSG_WriteScrambledString(msg, client->centerprint);
			}
			else {
				MSG_WriteSVC(msg, svc_centerprint);
				MSG_WriteScrambledString(msg, client->centerprint);
			}
		}*/
	}

	MSG_WriteByte(msg, specializeGeneralSVCOp(svc_EOF_general, demoType));
	demoCutWriteDemoMessage(msg, f, clcCut);
}

std::string makeConfigStringCommand(int index, std::string value) {
	std::stringstream ss;
	ss << "cs " << index << " " << value;
	return ss.str();
}
std::string makeEntityConfigStringCommand(int index, std::string value) {
	std::stringstream ss;
	ss << "entcs " << index << " " << value;
	return ss.str();
}

/*
================
G_FindConfigstringIndex

================
*/
int G_FindConfigstringIndex(const char* name, int start, int max, qboolean create, clientActive_t* clCut, std::vector<std::string>* commandsToAdd,demoType_t demoType) {
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
		Com_DPrintf("G_FindConfigstringIndex: overflow");
		return 0;
		//G_Error("G_FindConfigstringIndex: overflow");
	}

	//trap_SetConfigstring(start + i, name);
	demoCutConfigstringModifiedManual(clCut, start + i, name,demoType);
	commandsToAdd->push_back(makeConfigStringCommand(start + i, name));

	return i;
}


int G_SoundIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, getCS_SOUNDS(demoType), MAX_SOUNDS, qtrue, clCut, commandsToAdd, demoType);
}
int G_ModelIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, getCS_MODELS(demoType), MAX_MODELS, qtrue, clCut, commandsToAdd, demoType);
}
int G_GrappleSkinIndex(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, getCS_MAX(demoType)+ CS_GRAPPLESKIN_MAX_OFFSET, MAX_GRAPPLESKINS, qtrue, clCut, commandsToAdd, demoType);
}
int G_ModelIndex_NoAdd(const char* name, clientActive_t* clCut, std::vector<std::string>* commandsToAdd, demoType_t demoType) {
	return G_FindConfigstringIndex(name, getCS_MODELS(demoType), MAX_MODELS, qfalse, clCut, commandsToAdd, demoType);
}



/*
=================
AngleNormalize360

returns angle normalized to the range [0 <= angle < 360]
=================
*/
inline float AngleNormalize360(float angle) {
	return (360.0 / 65536) * ((int)(angle * (65536 / 360.0)) & 65535);
}

/*
=================
AngleNormalize180

returns angle normalized to the range [-180 < angle <= 180]
=================
*/
inline float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}

/*
=================
AngleDelta

returns the normalized delta from angle1 to angle2
=================
*/
inline float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}

#define KEY_W       0
#define KEY_WA      1
#define KEY_A       2
#define KEY_AS      3
#define KEY_S       4
#define KEY_SD      5
#define KEY_D       6
#define KEY_DW      7
usercmd_t CG_DirToCmd(int moveDir){
	usercmd_t outCmd = { 0 };
	switch(moveDir){
		case KEY_W:
			outCmd.forwardmove = 127;
			outCmd.rightmove = 0;
			break;
		case KEY_WA:
			outCmd.forwardmove = 127;
			outCmd.rightmove = -127;
			break;
		case KEY_A:
			outCmd.forwardmove = 0;
			outCmd.rightmove = -127;
			break;
		case KEY_AS:
			outCmd.forwardmove = -127;
			outCmd.rightmove = -127;
			break;
		case KEY_S:
			outCmd.forwardmove = -127;
			outCmd.rightmove = 0;
			break;
		case KEY_SD:
			outCmd.forwardmove = -127;
			outCmd.rightmove = 127;
			break;
		case KEY_D:
			outCmd.forwardmove = 0;
			outCmd.rightmove = 127;
			break;
		case KEY_DW:
			outCmd.forwardmove = 127;
			outCmd.rightmove = 127;
			break;
		default:
			break;
	}
	return outCmd;
}



/*
================
G_SetOrigin

Sets the pos trajectory for a fixed position
================
*/
void G_SetOrigin(entityState_t* entity, vec3_t origin) {
	VectorCopy(origin, entity->pos.trBase);
	entity->pos.trType = TR_STATIONARY;
	entity->pos.trTime = 0;
	entity->pos.trDuration = 0;
	VectorClear(entity->pos.trDelta);
}


static void bounceEntity(entityState_t* entity, double newServerTime, demoType_t demoType, trace_t* trace) {
	vec3_t	velocity;
	float	dot;
	int		hitTime;
	int		EF_BOUNCE_SHRAPNEL_REAL = EF_BOUNCE_SHRAPNEL;

	switch (demoType) {
	case DM_14:
		EF_BOUNCE_SHRAPNEL_REAL = EF_BOUNCE_SHRAPNEL_JK2SP;
		break;
	}

	// reflect the velocity on the trace plane
	hitTime = entity->pos.trTime + (newServerTime - entity->pos.trTime) * trace->fraction;
	BG_EvaluateTrajectoryDelta(&entity->pos, hitTime, velocity);
	dot = DotProduct(velocity, trace->plane.normal);
	VectorMA(velocity, -2 * dot, trace->plane.normal, entity->pos.trDelta);

	if (entity->eFlags & EF_BOUNCE_SHRAPNEL_REAL && (demoType < DM_66 || demoType > DM_68)) // q3 doesnt have this. others may not either? idk might need better condition, but lazy rn
	{
		VectorScale(entity->pos.trDelta, 0.25f, entity->pos.trDelta);
		entity->pos.trType = TR_GRAVITY;

		// check for stop
		if (trace->plane.normal[2] > 0.7 && entity->pos.trDelta[2] < 40) //this can happen even on very slightly sloped walls, so changed it from > 0 to > 0.7
		{
			G_SetOrigin(entity, trace->endpos);
			//ent->nextthink = level.time + 100;
			return;
		}
	}
	else if (entity->eFlags & EF_BOUNCE_HALF  )
	{
		if (demoType == DM_14) {
			VectorScale(entity->pos.trDelta, 0.5, entity->pos.trDelta);

			// check for stop
			if (trace->plane.normal[2] > 0.7 && entity->pos.trDelta[2] < 40) //this can happen even on very slightly sloped walls, so changed it from > 0 to > 0.7
			{
				if (generalizeGameValue<GMAP_WEAPONS,UNSAFE>(entity->weapon,demoType) == WP_THERMAL_GENERAL)
				{//roll when you "stop"
					entity->pos.trType = TR_INTERPOLATE;
				}
				//else
				{
					G_SetOrigin(entity, trace->endpos);
					//ent->nextthink = level.time + 500;
					return;
				}
			}

			//if (generalizeGameValue<GMAP_WEAPONS, UNSAFE>(entity->weapon, demoType) == WP_THERMAL_GENERAL)
			//{//roll when you "stop"
			//
			//	ent->has_bounced = qtrue;
			//}
		}
		else {
			VectorScale(entity->pos.trDelta, 0.65, entity->pos.trDelta);
			// check for stop
			if (trace->plane.normal[2] > 0.2 && VectorLength(entity->pos.trDelta) < 40)
			{
				G_SetOrigin(entity, trace->endpos);
				return;
			}
		}

	}
	else if(entity->demoToolsData.flags & DTFLAG_JK2MP_ITEMBOUNCE){

		VectorScale(entity->pos.trDelta, 0.5, entity->pos.trDelta);
		// check for stop
		if (trace->plane.normal[2] > 0 && entity->pos.trDelta[2] < 40) {
			trace->endpos[2] += 1.0;	// make sure it is off ground
			SnapVector(trace->endpos);
			G_SetOrigin(entity, trace->endpos);
			entity->groundEntityNum = trace->entityNum;
			return;
		}
		G_SetOrigin(entity, trace->endpos);
		return;
	}
	else {

		VectorClear(entity->pos.trDelta);
		G_SetOrigin(entity, trace->endpos);
		return;
	}

	if (demoType != DM_14) {
		VectorAdd(entity->pos.trBase, trace->plane.normal, entity->pos.trBase);
		entity->pos.trTime = newServerTime;
		entity->apos.trTime = newServerTime;
		return;
	}

	// NEW--It would seem that we want to set our trBase to the trace endpos
	//	and set the trTime to the actual time of impact....
	VectorAdd(trace->endpos, trace->plane.normal, entity->pos.trBase);
	if (hitTime >= newServerTime)
	{//trace fraction must have been 1
		entity->pos.trTime = newServerTime - 10;
	}
	else
	{
		entity->pos.trTime = hitTime - 10; // this is kinda dumb hacking, but it pushes the missile away from the impact plane a bit
	}

	//VectorCopy(trace->plane.normal, ent->pos1); // ??

	//if (ententity->weapon != WP_SABER
	//	&& ententity->weapon != WP_THERMAL
	//	&& ent->e_clThinkFunc != clThinkF_CG_Limb
	//	&& ent->e_ThinkFunc != thinkF_LimbThink)
	//{//not a saber, bouncing thermal or limb
	//	//now you can damage the guy you came from
	//	ent->owner = NULL;
	//}

	if (( entity->demoToolsData.flags & DTFLAG_ISLIMB) && entity->pos.trType == TR_STATIONARY) // for limbs
	{//stopped, stop spinning
		//lay flat
		//pitch
		//VectorCopy(ent->currentAngles, entity->apos.trBase);
		vec3_t	flatAngles;
		if (entity->angles2[0] == -1)
		{//any pitch is okay
			flatAngles[0] = entity->apos.trBase[0];
		}
		else
		{//lay flat
			//if (ent->owner
			//	&& ent->owner->client
			//	&& ent->owner->client->NPC_class == CLASS_PROTOCOL
			//	&& ent->count == BOTH_DISMEMBER_TORSO1)
			//{
			//	if (ent->currentAngles[0] > 0 || ent->currentAngles[0] < -180)
			//	{
			//		flatAngles[0] = -90;
			//	}
			//	else
			//	{
			//		flatAngles[0] = 90;
			//	}
			//}
			//else
			{
				if (entity->apos.trBase[0] > 90 || entity->apos.trBase[0] < -90)
				{
					flatAngles[0] = 180;
				}
				else
				{
					flatAngles[0] = 0;
				}
			}
		}
		//yaw
		flatAngles[1] = entity->apos.trBase[1];
		//roll
		if (entity->angles2[2] == -1)
		{//any roll is okay
			flatAngles[2] = entity->apos.trBase[2];
		}
		else
		{
			if (entity->apos.trBase[2] > 90 || entity->apos.trBase[2] < -90)
			{
				flatAngles[2] = 180;
			}
			else
			{
				flatAngles[2] = 0;
			}
		}
		VectorSubtract(flatAngles, entity->apos.trBase, entity->apos.trDelta);
		for (int i = 0; i < 3; i++)
		{
			entity->apos.trDelta[i] = AngleNormalize180(entity->apos.trDelta[i]);
		}
		entity->apos.trTime = newServerTime;
		entity->apos.trDuration = 1000;
		entity->apos.trType = TR_LINEAR_STOP;
		//VectorClear( entity->apos.trDelta );
	}
	else {
		entity->apos.trTime = newServerTime;
	}

}

#define UPDATETRTIME(trajectory,newtime) if((trajectory).trType == TR_LINEAR_STOP){ (trajectory).trDuration = std::max(0,(int)(newtime)-(trajectory).trTime); }; (trajectory).trTime=(newtime);

void retimeEntityStep(entityState_t* entity, double newServerTime, demoType_t demoType, CModel* cm) {

	vec3_t newPos;
	// angles
	BG_EvaluateTrajectory(&entity->apos, newServerTime, newPos);
	VectorCopy(newPos, entity->apos.trBase);
	// angle delta
	BG_EvaluateTrajectoryDelta(&entity->apos, newServerTime, newPos);
	VectorCopy(newPos, entity->apos.trDelta);

	// pos
	BG_EvaluateTrajectory(&entity->pos, newServerTime, newPos);
#if USE_CMODEL
	if (cm && entity->pos.trType > TR_INTERPOLATE) {
		trace_t trace;
		int mask;
		vec3_t mins, maxs;
		// make sure we dont fall into floors.

		if (entity->demoToolsData.flags & DTFLAG_MINMAXSET) {
			VectorCopy(entity->demoToolsData.mins, mins);
			VectorCopy(entity->demoToolsData.maxs, maxs);
		}
		else {
			IntegerToBoundingBox(entity->solid, mins, maxs);
		}

		// trace a line from the previous position to the current position
		//if (ent->clipmask) {
		//	mask = ent->clipmask;
		//}
		//else 
		{
			mask = MASK_PLAYERSOLID & ~CONTENTS_BODY;//MASK_SOLID;
		}
		cm->CM_BoxTrace(&trace, entity->pos.trBase, newPos, mins, maxs, 0, mask, qfalse); // for now we dont trace against any entities. might do later. just make sure it doesnt go into floor or wall
		//JP_Trace(&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin,
		//	ent->r.ownerNum, mask);


		if (trace.fraction < 1.0f) {

			VectorCopy(trace.endpos, entity->pos.trBase);

			//VectorClear(entity->pos.trDelta);
			//VectorClear(entity->apos.trDelta);
			//entity->pos.trType = TR_STATIONARY;
			//entity->apos.trType = TR_STATIONARY;
		}
		else {

			VectorCopy(newPos, entity->pos.trBase);
			BG_EvaluateTrajectoryDelta(&entity->pos, newServerTime, newPos);
			VectorCopy(newPos, entity->pos.trDelta);

			UPDATETRTIME(entity->pos, newServerTime);
			UPDATETRTIME(entity->apos, newServerTime);
			return;
		}


		// bounce
		bounceEntity(entity,newServerTime, demoType, &trace);
	}
	else
#endif
	{
		// pos delta
		VectorCopy(newPos, entity->pos.trBase);
		BG_EvaluateTrajectoryDelta(&entity->pos, newServerTime, newPos);
		VectorCopy(newPos, entity->pos.trDelta);
		UPDATETRTIME(entity->pos, newServerTime);
		UPDATETRTIME(entity->apos, newServerTime);
	}


}

#if USE_CMODEL
void retimeEntity(entityState_t* entity, double newServerTime, double newDemoTime, demoType_t demoType, CModel* cm) {
#else
void retimeEntity(entityState_t* entity, double newServerTime, double newDemoTime, demoType_t demoType) {
	CModel* cm = NULL;
#endif

	if (entity->solid != SOLID_BMODEL){//entity->pos.trType == TR_GRAVITY) {

		double currentTime = std::max(entity->pos.trTime, entity->apos.trTime); // they can be different. i think if we start at the lower one it could cause weirdness.. so start at a common max
		double baseStep,nextTime;


		while (currentTime < newServerTime) {
			baseStep = (newServerTime - currentTime) > 1000 ? (newServerTime - currentTime - 1000) : 100; // in case of some weirdness, skip up to 1 second before here.
			nextTime = ((entity->pos.trType > TR_INTERPOLATE || entity->apos.trType > TR_INTERPOLATE) && cm) ? std::min((double)newServerTime, currentTime + baseStep) : newServerTime;

			// we need to go in steps because bouncing uses a trace to see how far the entity can get when tracing to the evaluated trajectory
			// however the gravity pull doesn't rersult in a linear line movement, rather a curve, so the "impact" point changes as the delta in time moves forward,
			// leading to stutttering in bounce situations

			retimeEntityStep(entity, nextTime, demoType, cm);
			currentTime = nextTime;
		}
	
		entity->pos.trTime += newDemoTime - newServerTime;
		entity->apos.trTime += newDemoTime - newServerTime;
	}
	else {
		// oldschool method
		// the above is way cooler and fancier but its not good for some things like bmodels
#if 0
		// wait... is this even needed? we can just update trTime can't we?
		vec3_t newPos;
		BG_EvaluateTrajectory(&entity->pos, newServerTime, newPos);
		VectorCopy(newPos, entity->pos.trBase);
		BG_EvaluateTrajectoryDelta(&entity->pos, newServerTime, newPos);
		VectorCopy(newPos, entity->pos.trDelta);
		BG_EvaluateTrajectory(&entity->apos, newServerTime, newPos);
		VectorCopy(newPos, entity->apos.trBase);
		BG_EvaluateTrajectoryDelta(&entity->apos, newServerTime, newPos);
		VectorCopy(newPos, entity->apos.trDelta);

		UPDATETRTIME(entity->pos, newServerTime);
		UPDATETRTIME(entity->apos, newServerTime);
		entity->pos.trTime = newDemoTime;
		entity->apos.trTime = newDemoTime;
#else
		// right. this just gets evaluated in the client anyway. why bruteforce it..
		entity->pos.trTime += newDemoTime - newServerTime;
		entity->apos.trTime += newDemoTime - newServerTime;
#endif


	}
}










//rww - convience function..
int Q_irand(int value1, int value2, qboolean useDefault, int defaultValue)
{
	int r;

	//#ifdef DEBUG
	//	// find bad calls: rand\s*\([^,]+,\s*([^,\s]+)\s*,[^,]+,\s*\1\s*\)
	//	if (defaultValue >= value2 || defaultValue < value1) {
	//		Com_Printf("Q_irand(%d,%d,%d,%d), bad call", value1, value2, useDefault, defaultValue);
	//	}
	//#endif
	if (useDefault) {
		return defaultValue;
	}

	r = rand() % value2;
	r += value1;

	return r;
}









int strNumLen(const char* s) {
	if (!s) return 0;
	int len = 0;
	while (*s >= '0' && *s <= '9' || *s == '-') {
		len++;
		s++;
	}
	return len;
}
int atoiWhileNumber(const char* s) {
	static char numString[20];
	if (!s) return 0;
	int len = 0;
	while (*s >= '0' && *s <= '9' || *s == '-') {
		numString[len] = *s;
		len++;
		s++;
	}
	numString[len] = 0;
	return atoi(numString);
}
int atoiWhileNumber(const char** s) {
	static char numString[20];
	if (!s || !*s) return 0;
	int len = 0;
	while (**s >= '0' && **s <= '9' || **s == '-') {
		numString[len] = **s;
		len++;
		(*s)++;
	}
	numString[len] = 0;
	return atoi(numString);
}





/*
=================
IntegerToBoundingBox (MOHAA)
=================
*/
#define BBOX_XBITS 9
#define BBOX_YBITS 8
#define BBOX_ZBOTTOMBITS 5
#define BBOX_ZTOPBITS 9
#define BBOX_MAX_X ( 1 << BBOX_XBITS )
#define BBOX_MAX_Y ( 1 << BBOX_YBITS )
#define BBOX_MAX_BOTTOM_Z ( 1 << ( BBOX_ZBOTTOMBITS - 1 ) )
#define BBOX_REALMAX_BOTTOM_Z ( 1 << BBOX_ZBOTTOMBITS )
#define BBOX_MAX_TOP_Z ( 1 << BBOX_ZTOPBITS )
void IntegerToBoundingBox(int num, vec3_t mins, vec3_t maxs)
{
	int x, y, zd, zu;

	x = num & (BBOX_MAX_X - 1);
	y = (num >> (BBOX_XBITS)) & (BBOX_MAX_Y - 1);
	zd = (num >> (BBOX_XBITS + BBOX_YBITS)) & (BBOX_REALMAX_BOTTOM_Z - 1);
	zd -= BBOX_MAX_BOTTOM_Z;
	zu = (num >> (BBOX_XBITS + BBOX_YBITS + BBOX_ZBOTTOMBITS)) & (BBOX_MAX_TOP_Z - 1);

	mins[0] = -x;
	mins[1] = -y;
	mins[2] = zd;

	maxs[0] = x;
	maxs[1] = y;
	maxs[2] = zu;
}

#ifdef DEBUG
#define DEBUG_MOHAA_STRINGMATCH
//#define DEBUG_MOHAA_STRINGMATCH_FULL
#endif


template<class T>
T* mohaaMatchString(const tsl::htrie_map<char,T>* stringMap, char** message) {
	if (!**message) {
		return NULL; // Not a death message
	}
	else {
		//char partialString[MAX_NAME_LENGTH_MOHAA];
		//char possibleStringCompare[MAX_NAME_LENGTH_MOHAA];
		char partialString[50];
		char possibleStringCompare[50]; // Those spanish strings are hella long
		
		int matchCount = 99999;

		int consecutiveCharCount = 0;
		char* msgTmp = *message;
		int msgLen = strlen(*message);
		while (true) {
			if ((*msgTmp == ' ' || *msgTmp == '\'' || *msgTmp == '\n') && consecutiveCharCount < MAX_NAME_LENGTH_MOHAA - 1) {
				// If we are at a space or single quote, we need to just increment a single place
				// because in theory it might be a space at the end of the time that we want to match
				consecutiveCharCount++;
				msgTmp++;
			}
			else {
				while (*msgTmp && *msgTmp != ' ' && *msgTmp != '\'' && *msgTmp != '\n' && consecutiveCharCount < MAX_NAME_LENGTH_MOHAA - 1) {
					consecutiveCharCount++;
					msgTmp++;
				}
			}
			strncpy_s(partialString, *message, consecutiveCharCount);
			auto matchedStrings = stringMap->equal_prefix_range(partialString);
			matchCount = 0;
			auto realMatchIt = matchedStrings.first;
			for (auto it = matchedStrings.first; it != matchedStrings.second; ++it) {
				auto matchedStringHere = it.key();
				if (matchedStringHere.size() <= msgLen) {

					strncpy_s(possibleStringCompare, *message, matchedStringHere.size());
					if (!_stricmp(possibleStringCompare, matchedStringHere.c_str())) {

						realMatchIt = it;
						matchCount++;
					}
				}
			}

			if (matchCount == 0) {
#ifdef DEBUG_MOHAA_STRINGMATCH
				std::cerr << "STRING MATCH FAIL, NO MATCH: "<< *message << "\n";
#endif
				return NULL;
			}
			else if (matchCount == 1) {
#ifdef DEBUG_MOHAA_STRINGMATCH
#ifdef DEBUG_MOHAA_STRINGMATCH_FULL
				std::cout << "STRING MATCH:" << matchedNames.first.key() << "\n";
#endif
#endif
				*message += realMatchIt.key().size(); // move the passed pointer forward so we skip perfectly past the matched name.
				return (T*)&realMatchIt.value();
			}
			else if (*msgTmp && consecutiveCharCount < MAX_NAME_LENGTH_MOHAA-1){
				// repeat the loop
			}
			else {
#ifdef DEBUG_MOHAA_STRINGMATCH
				std::cerr << "STRING MATCH FAIL, 2 MANY MATCH\n";
#endif
				return NULL;
			}
		}

	}
}


entityState_t* parseMOHAADeathMessage(tsl::htrie_map<char,int>* playerMapClientNumMap, char* message) {
	if (*message != 4 && *message != 5) {
		return NULL; // Not a death message
	}
	else {
		static entityState_t tmpEs;

		Com_Memset(&tmpEs,0,sizeof(tmpEs));

		tmpEs.event = EV_OBITUARY_GENERAL;
		tmpEs.time = -1;

		char* oMsg = message;
		message++;
		int* matchedPlayer1 = mohaaMatchString(playerMapClientNumMap,&message);
		if (matchedPlayer1 == NULL) return NULL;

		tmpEs.otherEntityNum = *matchedPlayer1; // target

		message++; // Empty space

		mohMeansOfDeath_t* mohMod = mohaaMatchString(&mohMeansOfDeathArray,&message);
		if (mohMod == NULL) return NULL;

		tmpEs.eventParm = mohMod->meansOfDeath;
		tmpEs.weapon = mohMod->weaponClass; // Little special thing.
		tmpEs.time2 = mohMod->wasZoomed;

		if (mohMod->isSelfKill) { 
			tmpEs.otherEntityNum2 = *matchedPlayer1; // attacker
		}
		else if (!mohMod->attackerExistsAndIsClient) {
			tmpEs.otherEntityNum2 = ENTITYNUM_WORLD; // attacker
		}

		if (!mohMod->attackerExistsAndIsClient || mohMod->isSelfKill) {
			goto mohParseDeathMsgReturnEntityState;
		}

		message++; // Empty space

		int* matchedPlayer2 = mohaaMatchString(playerMapClientNumMap, &message);
		if (matchedPlayer2 == NULL) return NULL;

		tmpEs.otherEntityNum2 = *matchedPlayer2; // attacker

		char stringCompare[MAX_NAME_LENGTH_MOHAA];
		int msgLen = strlen(message);
		int s2Count = 0;
		bool s2Match = false;
		for (int i = 0; i < MOH_MAX_KILLMSG_S2_VARIATIONS; i++) {

			if (mohMod->s2[i]) {
				s2Count++;
				int lenCompare = strlen(mohMod->s2[i]);
				if (lenCompare <= msgLen) {

					strncpy_s(stringCompare, message, lenCompare);
					if (!_stricmp(stringCompare, mohMod->s2[i])) {

						s2Match = true;
						message += lenCompare;
						msgLen -= lenCompare;
						break;
					}
				}
			}
		}

		if (s2Count && !s2Match) {
			return NULL;
		}

		/* I changed this. the "in the" is now part of the message. Needed due to translations.
		int killLocIntroLength = sizeof(" in the ") - 1;
		if (killLocIntroLength > msgLen) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}

		strncpy_s(stringCompare, message, killLocIntroLength);
		if (_stricmp(stringCompare, " in the ")) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}

		message += killLocIntroLength;

		//int matchedPlayer2 = mohaaMatchPlayerName(playerMapClientNumMap, &message);
		int* matchedKillLocation = mohaaMatchString(&mohKillLocationArray, &message);
		if (matchedKillLocation == NULL) {
			return NULL; // No match for the location but there SHOULD have been a match.
		}*/

		if (!*message) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}
		message++; // Empty space
		if (!*message) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}

		int* matchedKillLocation = mohaaMatchString(&mohKillLocationArray, &message);
		if (matchedKillLocation == NULL) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}

		tmpEs.time = *matchedKillLocation; // Ugly but whatever.

mohParseDeathMsgReturnEntityState:
		entityState_t* retVal = new entityState_t{};
		*retVal = tmpEs;
		return retVal;
	}
}

entityState_t* parseMOHAAPrintDeathMsgFromTokenized(tsl::htrie_map<char,int>* playerMapClientNumMap) {
	if (Cmd_Argc() < 6) {
		return NULL; // Not a proper printdeathmsg
	}
	else {
		static entityState_t tmpEs;

		Com_Memset(&tmpEs,0,sizeof(tmpEs));

		tmpEs.event = EV_OBITUARY_GENERAL;
		tmpEs.time = -1;

		auto player1Match = playerMapClientNumMap->find(Cmd_Argv(4));
		if (player1Match == playerMapClientNumMap->end()) {
			std::cout << "STRING MATCH FAIL, NO MATCH: " << Cmd_Argv(4) << "\n";
			return NULL;
		}
		int matchedPlayer1 = player1Match.value();

		tmpEs.otherEntityNum = matchedPlayer1; // target

		auto s1Match = mohMeansOfDeathArray.find(Cmd_Argv(1));
		if (s1Match == mohMeansOfDeathArray.end()) {
			std::cout << "STRING MATCH FAIL, NO MATCH: " << Cmd_Argv(1) << "\n";
			return NULL;
		}
		mohMeansOfDeath_t mohMod = s1Match.value();

		tmpEs.eventParm = mohMod.meansOfDeath;
		tmpEs.weapon = mohMod.weaponClass; // Little special thing.
		tmpEs.time2 = mohMod.wasZoomed;

		if (mohMod.isSelfKill) { 
			tmpEs.otherEntityNum2 = matchedPlayer1; // attacker
		}
		else if (!mohMod.attackerExistsAndIsClient) {
			tmpEs.otherEntityNum2 = ENTITYNUM_WORLD; // attacker
		}

		if (!mohMod.attackerExistsAndIsClient || mohMod.isSelfKill) {
			goto mohParsePrintDeathMsgReturnEntityState;
		}

		{
			auto player2Match = playerMapClientNumMap->find(Cmd_Argv(3));
			if (player2Match == playerMapClientNumMap->end()) {
				std::cout << "STRING MATCH FAIL, NO MATCH: " << Cmd_Argv(3) << "\n";
				return NULL;
			}
			int matchedPlayer2 = player2Match.value();

			tmpEs.otherEntityNum2 = matchedPlayer2; // attacker
		}

		const char* s2 = Cmd_Argv(2);
		bool s2Match = false;
		int s2Count = 0;
		for (int i = 0; i < MOH_MAX_KILLMSG_S2_VARIATIONS; i++) {

			if (mohMod.s2[i]) {
				s2Count++;
				if (!_stricmp(s2, mohMod.s2[i])) {

					s2Match = true;
					break;
				}
			}
		}

		if (s2Count && !s2Match) {
			std::cout << "STRING MATCH FAIL, NO MATCH: " << Cmd_Argv(2) << "\n";
			return NULL;
		}

		// Kill location doesn't exist in expansions. :(
		/*
		int* matchedKillLocation = mohaaMatchString(&mohKillLocationArray, &message);
		if (matchedKillLocation == NULL) {
			goto mohParseDeathMsgReturnEntityState; // No kill location ig.
		}*/

		tmpEs.time = -1; // Ugly but whatever.

mohParsePrintDeathMsgReturnEntityState:
		entityState_t* retVal = new entityState_t{};
		*retVal = tmpEs;
		return retVal;
	}
}

static float RadiusFromBounds(const vec3_t mins, const vec3_t maxs) {
	int		i;
	vec3_t	corner;
	float	a, b;

	for (i = 0; i < 3; i++) {
		a = fabsf(mins[i]);
		b = fabsf(maxs[i]);
		corner[i] = a > b ? a : b;
	}

	return VectorLength(corner);
}
static void GetAbsMinsMaxs(qboolean isBModel, vec3_t origin, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t absmin, vec3_t absmax) {

	// set the abs box
	if (isBModel && (angles[0] || angles[1] || angles[2])) {
		// expand for rotation
		float		max;
		int			i;

		max = RadiusFromBounds(mins, maxs);
		for (i = 0; i < 3; i++) {
			absmin[i] = origin[i] - max;
			absmax[i] = origin[i] + max;
		}
	}
	else {
		// normal
		VectorAdd(origin, mins, absmin);
		VectorAdd(origin, maxs, absmax);
	}

	// because movement is clipped an epsilon away from an actual edge,
	// we must fully check even when bounding boxes don't quite touch
	absmin[0] -= 1;
	absmin[1] -= 1;
	absmin[2] -= 1;
	absmax[0] += 1;
	absmax[1] += 1;
	absmax[2] += 1;
}

int G_GetHitLocation(entityState_t* target, vec3_t ppoint)
{
	vec3_t			point, point_dir;
	vec3_t			forward, right, up;
	vec3_t			tangles, tcenter;
	float			tradius;
	float			udot, fdot, rdot;
	int				Vertical, Forward, Lateral;
	int				HitLoc;
	vec3_t			mins, maxs, absmin, absmax;

	//get target forward, right and up
	//if (target->client)
	{//ignore player's pitch and roll
		VectorSet(tangles, 0, target->apos.trBase[YAW], 0);
	}

	AngleVectors(tangles, forward, right, up);

	//get center of target
	IntegerToBoundingBox(target->solid,mins,maxs);
	GetAbsMinsMaxs(qboolean(target->solid== SOLID_BMODEL),target->pos.trBase,target->apos.trBase,mins,maxs,absmin,absmax);
	VectorAdd(absmin, absmax, tcenter);
	VectorScale(tcenter, 0.5, tcenter);

	//get radius width of target
	tradius = (fabs(maxs[0]) + fabs(maxs[1]) + fabs(mins[0]) + fabs(mins[1])) / 4;

	//get impact point
	if (ppoint && !VectorCompare(ppoint, vec3_origin))
	{
		VectorCopy(ppoint, point);
	}
	else
	{
		return HL_NONE;
	}

	/*	
	// Get radius width of target.
	tradius = (fabs(target->r.maxs[0]) + fabs(target->r.maxs[1]) + fabs(target->r.mins[0]) + fabs(target->r.mins[1]))/4;

	//get impact dir
		if(pdir && !VectorCompare(pdir, vec3_origin))
		{
			VectorCopy(pdir, dir);
		}
		else
		{
			return;
		}

	//put point at controlled distance from center
		VectorSubtract(point, tcenter, tempvec);
		tempvec[2] = 0;
		hdist = VectorLength(tempvec);

		VectorMA(point, hdist - tradius, dir, point);
		//now a point on the surface of a cylinder with a radius of tradius
	*/
	VectorSubtract(point, tcenter, point_dir);
	VectorNormalize(point_dir);

	//Get bottom to top (Vertical) position index
	udot = DotProduct(up, point_dir);
	if (udot > .800)
		Vertical = 4;
	else if (udot > .400)
		Vertical = 3;
	else if (udot > -.333)
		Vertical = 2;
	else if (udot > -.666)
		Vertical = 1;
	else
		Vertical = 0;

	//Get back to front (Forward) position index
	fdot = DotProduct(forward, point_dir);
	if (fdot > .666)
		Forward = 4;
	else if (fdot > .333)
		Forward = 3;
	else if (fdot > -.333)
		Forward = 2;
	else if (fdot > -.666)
		Forward = 1;
	else
		Forward = 0;

	//Get left to right (Lateral) position index
	rdot = DotProduct(right, point_dir);
	if (rdot > .666)
		Lateral = 4;
	else if (rdot > .333)
		Lateral = 3;
	else if (rdot > -.333)
		Lateral = 2;
	else if (rdot > -.666)
		Lateral = 1;
	else
		Lateral = 0;

	HitLoc = Vertical * 25 + Forward * 5 + Lateral;

	if (HitLoc <= 10)
	{//feet
		if (rdot > 0)
		{
			return HL_FOOT_RT;
		}
		else
		{
			return HL_FOOT_LT;
		}
	}
	else if (HitLoc <= 50)
	{//legs
		if (rdot > 0)
		{
			return HL_LEG_RT;
		}
		else
		{
			return HL_LEG_LT;
		}
	}
	else if (HitLoc == 56 || HitLoc == 60 || HitLoc == 61 || HitLoc == 65 || HitLoc == 66 || HitLoc == 70)
	{//hands
		if (rdot > 0)
		{
			return HL_HAND_RT;
		}
		else
		{
			return HL_HAND_LT;
		}
	}
	else if (HitLoc == 83 || HitLoc == 87 || HitLoc == 88 || HitLoc == 92 || HitLoc == 93 || HitLoc == 97)
	{//arms
		if (rdot > 0)
		{
			return HL_ARM_RT;
		}
		else
		{
			return HL_ARM_LT;
		}
	}
	else if ((HitLoc >= 107 && HitLoc <= 109) ||
		(HitLoc >= 112 && HitLoc <= 114) ||
		(HitLoc >= 117 && HitLoc <= 119))
	{//head
		return HL_HEAD;
	}
	else
	{
		if (udot < 0.3)
		{
			return HL_WAIST;
		}
		else if (fdot < 0)
		{
			if (rdot > 0.4)
			{
				return HL_BACK_RT;
			}
			else if (rdot < -0.4)
			{
				return HL_BACK_LT;
			}
			else
			{
				return HL_BACK;
			}
		}
		else
		{
			if (rdot > 0.3)
			{
				return HL_CHEST_RT;
			}
			else if (rdot < -0.3)
			{
				return HL_CHEST_LT;
			}
			else
			{
				return HL_CHEST;
			}
		}
	}
	return HL_NONE;
}


int G_GetHitQuad(vec3_t entpos, vec3_t entangles, vec3_t hitloc)
{
	vec3_t diff, fwdangles = { 0,0,0 }, right;
	vec3_t clEye;
	float rightdot;
	float zdiff;
	int hitLoc = -1;

	//if (self->client)
	//{
	//	VectorCopy(self->client->ps.origin, clEye);
	//	clEye[2] += self->client->ps.viewheight;
	//}
	//else
	{
		VectorCopy(entpos, clEye);
		clEye[2] += 16;
	}

	VectorSubtract(hitloc, clEye, diff);
	diff[2] = 0;
	VectorNormalize(diff);

	//if (self->client)
	//{
	//	fwdangles[1] = self->client->ps.viewangles[1];
	//}
	//else
	{
		fwdangles[1] = entangles[1];
	}
	// Ultimately we might care if the shot was ahead or behind, but for now, just quadrant is fine.
	AngleVectors(fwdangles, NULL, right, NULL);

	rightdot = DotProduct(right, diff);
	zdiff = hitloc[2] - clEye[2];

	if (zdiff > 0)
	{
		if (rightdot > 0.3)
		{
			hitLoc = G2_MODELPART_RARM;
		}
		else if (rightdot < -0.3)
		{
			hitLoc = G2_MODELPART_LARM;
		}
		else
		{
			hitLoc = G2_MODELPART_HEAD;
		}
	}
	else if (zdiff > -20)
	{
		if (rightdot > 0.1)
		{
			hitLoc = G2_MODELPART_RARM;
		}
		else if (rightdot < -0.1)
		{
			hitLoc = G2_MODELPART_LARM;
		}
		else
		{
			hitLoc = G2_MODELPART_HEAD;
		}
	}
	else
	{
		if (rightdot >= 0)
		{
			hitLoc = G2_MODELPART_RLEG;
		}
		else
		{
			hitLoc = G2_MODELPART_LLEG;
		}
	}

	return hitLoc;
}

int	getBodyPartFromHitLoc(vec3_t entpos,vec3_t entangles, int hitLoc, vec3_t roughHitLoc) {
	int hitLocUse;
	switch (hitLoc)
	{
	case HL_FOOT_RT:
	case HL_LEG_RT:
		hitLocUse = G2_MODELPART_RLEG;
		break;
	case HL_FOOT_LT:
	case HL_LEG_LT:
		hitLocUse = G2_MODELPART_LLEG;
		break;

	case HL_WAIST:
		hitLocUse = G2_MODELPART_WAIST;
		break;
		/*
	case HL_BACK_RT:
	case HL_BACK_LT:
	case HL_BACK:
	case HL_CHEST_RT:
	case HL_CHEST_LT:
	case HL_CHEST:
		break;
		*/
	case HL_ARM_RT:
		hitLocUse = G2_MODELPART_RARM;
		break;
	case HL_HAND_RT:
		hitLocUse = G2_MODELPART_RHAND;
		break;
	case HL_ARM_LT:
	case HL_HAND_LT:
		hitLocUse = G2_MODELPART_LARM;
		break;
	case HL_HEAD:
		hitLocUse = G2_MODELPART_HEAD;
		break;
	default:
		hitLocUse = G_GetHitQuad(entpos, entangles, roughHitLoc);
		break;
	}
	return hitLocUse;
}



bool isNumber(const char* str)
{
	if (!str || !*str) return false;
	while(*str)
	{
		if (!(*str >= '0' && *str <= '9'))
		{
			return false;
		}
		str++;
	}
	return true;
}





int64_t demoCutGetDemoNameTruncationOffset(const char* demoName) {
	static char numberBuffer[100];
	int numberBufferIndex = 0;

	while (*demoName) {
		if (!_strnicmp(demoName,"_tr",3)) {
			demoName += 3;
			numberBufferIndex = 0;
			while (*demoName >= '0' && *demoName <= '9' && numberBufferIndex < (sizeof(numberBuffer)-1)) {
				numberBuffer[numberBufferIndex++] = *demoName;
				demoName++;
			}
			if (numberBufferIndex > 0) {
				numberBuffer[numberBufferIndex] = '\0';
				return atoi(numberBuffer);
			}
			
		}
		demoName++;
	}
	return 0L;
}


qboolean demoCutGetDemoType(const char* demoFile, char extOutput[7], char outputNameNoExt[MAX_OSPATH], demoType_t* demoType, qboolean* isCompressed, clientConnection_t* clcCut) {

	char normalizedExt[7]{};

	int lastChar = strlen(demoFile)-1;


	int lastDot = -1;
	for (int i = lastChar; i >= 0; i--) {
		if (demoFile[i] == '.') {
			lastDot = i;
			break;
		}
	}

	if (lastDot == -1) {
		if (lastChar < 6) {
			goto demoCutGetTypeReturnDefaultFaulted2;
		}
		else {
			goto demoCutGetTypeReturnDefaultFaulted;
		}
	}
	else if (lastDot == 0) {
		goto demoCutGetTypeReturnDefaultFaulted2;
	}
	else if (lastDot == lastChar) {
		goto demoCutGetTypeReturnDefaultFaulted2;
	}

	if (outputNameNoExt) {
		//strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), demoFile, strlen(demoFile) - 6);
		//strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), demoFile, lastDot);
		strncpy_s(outputNameNoExt, MAX_OSPATH, demoFile, lastDot);
	}
	if (extOutput) {
		//strncpy_s(extOutput, 7, (char*)demoFile + strlen(demoFile) - 6, 6);
		strncpy_s(extOutput, 7, (char*)demoFile + lastDot, lastChar-lastDot+1);
	}

	if (strlen(extOutput) < 4) { // Because need to change [3] to create compressed versions... i know its shit. TODO.
		goto demoCutGetTypeReturnDefaultFaulted2;
	}

	bool isStandardDemoNameFormat = extOutput[1] == 'd' && extOutput[2] == 'm' && (extOutput[3] == '_' || extOutput[3] == 'c') && extOutput[4] >= '0' && extOutput[4] <= '9' && extOutput[5] >= '0' && extOutput[5] <= '9';

	//strncpy_s(normalizedExt, sizeof(normalizedExt), (char*)demoFile + strlen(demoFile) - 6, 6);
	strncpy_s(normalizedExt, sizeof(normalizedExt), (char*)demoFile + lastDot, lastChar - lastDot + 1);
	//strncpy_s(originalExtOutput, 7, (char*)demoFile + strlen(demoFile) - 6, 6);

	*isCompressed = qfalse;
	if (isStandardDemoNameFormat) {
		normalizedExt[0] = '.';

		char specialTypeChar = normalizedExt[3];
		normalizedExt[3] = '_';

		if (specialTypeChar == 'c') {
			*isCompressed = qtrue;
		}
	}
	else {
		// Special handling further down.
	}

	if (clcCut) {
		clcCut->demoCheckFor103 = qfalse;
	}

	if (!*normalizedExt) { // This cant really happen anyway can it?
		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (clcCut) {
			clcCut->demoCheckFor103 = qtrue;
		}
		return qfalse;
	}

	// Medal of Honor
	else if (!_stricmp(normalizedExt, ".dm3")) {

		*demoType = DM3_MOHAA_PROT_6;
		if (clcCut) {
			clcCut->demoCheckProtocol = qtrue;
		}
	}
	else if (!_stricmp(normalizedExt, ".dm3_c")) {

		*demoType = DM3_MOHAA_PROT_6;
		if (clcCut) {
			clcCut->demoCheckProtocol = qtrue;
		}
		*isCompressed = qtrue;
	}

	else if (!_stricmp(normalizedExt, ".dm_14")) {

		*demoType = DM_14;
		//strncpy_s(normalizedExt, 7, ".dm_14", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_15")) {

		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (clcCut) {
			clcCut->demoCheckFor103 = qtrue;
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
	}
	else if (!_stricmp(normalizedExt, ".dm_73")) {

		*demoType = DM_73;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_90")) {

		*demoType = DM_90;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	}
	else if (!_stricmp(normalizedExt, ".dm_91")) {

		*demoType = DM_91;
		//strncpy_s(normalizedExt,7, ".dm_26", 6);
	} else {

		*demoType = DM_15;
		//strncpy_s(normalizedExt, 7, ".dm_15", 6);
		if (clcCut) {
			clcCut->demoCheckFor103 = qtrue;
		}
		return qfalse;
	}

	return qtrue;

demoCutGetTypeReturnDefaultFaulted:
	if (outputNameNoExt) {
		strncpy_s(outputNameNoExt, sizeof(outputNameNoExt), demoFile, strlen(demoFile) - 6);
	}
	if (extOutput) {
		strncpy_s(extOutput, 7, (char*)demoFile + strlen(demoFile) - 6, 6);
	}
demoCutGetTypeReturnDefaultFaulted2:
	*demoType = DM_15;
	//strncpy_s(normalizedExt, 7, ".dm_15", 6);
	if (clcCut) {
		clcCut->demoCheckFor103 = qtrue;
	}
	return qfalse;
}

























static gameInfo_t gameInfos[] = {
	{ // MOHAA
		DM3_MOHAA_PROT_6, 
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_centerprint_general,
			svc_locprint_general,
			svc_cgameMessage_general,
			svc_EOF_general
		},
		{
			{ // MOHAA is very different from classic Q3 engine. A lot of these don't make sense for MOHAA so they're not really fixed up.
				{},
				{qlEventToGeneralMap,sizeof(qlEventToGeneralMap) / sizeof(qlEventToGeneralMap[0])},
			},{
				{},
				{mohWeaponsToGeneral,sizeof(mohWeaponsToGeneral) / sizeof(mohWeaponsToGeneral[0])}, // MOH doesn't REALLY have a weapons enum and doesn't network weapon index either. Just basing it on the WPREFIX_ enum.
			},{
				{},
				{mohaaModToGeneralMap,sizeof(mohaaModToGeneralMap) / sizeof(mohaaModToGeneralMap[0])},
			},
			{{},{lsMoveNOSABERToGeneral,sizeof(lsMoveNOSABERToGeneral) / sizeof(lsMoveNOSABERToGeneral[0])}},
			{{},{qldm91ItemListToGeneral,sizeof(qldm91ItemListToGeneral) / sizeof(qldm91ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},qlAnimToDM16,sizeof(qlAnimToDM16) / sizeof(qlAnimToDM16[0])}},
				{qlAnimsToGeneral,sizeof(qlAnimsToGeneral) / sizeof(qlAnimsToGeneral[0])},
			},
			{{},{mohaaEntityTypeToGeneral,sizeof(mohaaEntityTypeToGeneral) / sizeof(mohaaEntityTypeToGeneral[0])}},
			{{},{mohaaGametypeToGeneral,sizeof(mohaaGametypeToGeneral) / sizeof(mohaaGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsMOHAA_ver_6,sizeof(entityStateFieldsMOHAA_ver_6) / sizeof(entityStateFieldsMOHAA_ver_6[0]),},
			{playerStateFieldsMOHAA_ver_6,sizeof(playerStateFieldsMOHAA_ver_6) / sizeof(playerStateFieldsMOHAA_ver_6[0]),}
		},
		MAX_CONFIGSTRINGS_MOH,
		{CS_MODELS_MOH,CS_SOUNDS_MOH,CS_PLAYERS_MOH,ET_EVENTS_MOH,-1,ANIM_TOGGLEBIT_MOH,CS_LEVEL_START_TIME_MOH_OLD, EF_TELEPORT_BIT,CS_SOUNDTRACK_MOH + 1},
		MAX_CLIENTS_MOH
	},
	{ // MOHAA expansions
		DM3_MOHAA_PROT_15, 
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_centerprint_general,
			svc_locprint_general,
			svc_cgameMessage_general,
			svc_EOF_general
		},
		{
			{ // MOHAA is very different from classic Q3 engine. A lot of these don't make sense for MOHAA so they're not really fixed up.
				{},
				{qlEventToGeneralMap,sizeof(qlEventToGeneralMap) / sizeof(qlEventToGeneralMap[0])},
			},{
				{},
				{mohWeaponsToGeneral,sizeof(mohWeaponsToGeneral) / sizeof(mohWeaponsToGeneral[0])}, // MOH doesn't REALLY have a weapons enum and doesn't network weapon index either. Just basing it on the WPREFIX_ enum.
			},{
				{},
				{mohaaModToGeneralMap,sizeof(mohaaModToGeneralMap) / sizeof(mohaaModToGeneralMap[0])},
			},
			{{},{lsMoveNOSABERToGeneral,sizeof(lsMoveNOSABERToGeneral) / sizeof(lsMoveNOSABERToGeneral[0])}},
			{{},{qldm91ItemListToGeneral,sizeof(qldm91ItemListToGeneral) / sizeof(qldm91ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},qlAnimToDM16,sizeof(qlAnimToDM16) / sizeof(qlAnimToDM16[0])}},
				{qlAnimsToGeneral,sizeof(qlAnimsToGeneral) / sizeof(qlAnimsToGeneral[0])},
			},
			{{},{mohaaEntityTypeToGeneral,sizeof(mohaaEntityTypeToGeneral) / sizeof(mohaaEntityTypeToGeneral[0])}},
			{{},{mohaaGametypeToGeneral,sizeof(mohaaGametypeToGeneral) / sizeof(mohaaGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsMOHAA_ver_15,sizeof(entityStateFieldsMOHAA_ver_15) / sizeof(entityStateFieldsMOHAA_ver_15[0]),},
			{playerStateFieldsMOHAA_ver_15,sizeof(playerStateFieldsMOHAA_ver_15) / sizeof(playerStateFieldsMOHAA_ver_15[0]),}
		},
		MAX_CONFIGSTRINGS_MOH,
		{CS_MODELS_MOH,CS_SOUNDS_MOH,CS_PLAYERS_MOH,ET_EVENTS_MOH,-1,ANIM_TOGGLEBIT_MOH,CS_LEVEL_START_TIME_MOH, EF_TELEPORT_BIT,CS_SOUNDTRACK_MOH+1}, // not sure about the value for CS_MAX here. CS_MAX itself is broken in openmohaa but no server i know ever used one above 1880
		MAX_CLIENTS_MOH
	},
	{
		DM_14,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
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
			{
				{},
				{jk2spEventsToGeneral,sizeof(jk2spEventsToGeneral) / sizeof(jk2spEventsToGeneral[0])},
			},{
				{},
				{jk2spWeaponToGeneral,sizeof(jk2spWeaponToGeneral) / sizeof(jk2spWeaponToGeneral[0])},
			},{
				{},
				{jk2spModToGeneral,sizeof(jk2spModToGeneral) / sizeof(jk2spModToGeneral[0])},
			},
			{{},{saberMoveJK2ToGeneral + 1,(sizeof(saberMoveJK2ToGeneral) / sizeof(saberMoveJK2ToGeneral[0])) - 1}}, // Same exact thing EXCEPT it has no LS_INVALID, so move 1 forward
			{{},{jk2spItemListToGeneral,sizeof(jk2spItemListToGeneral) / sizeof(jk2spItemListToGeneral[0])}},
			{ // Single player anims are identical to DM16 anims
				{{{DM_15},animMappingTable_1_04_to_1_02,sizeof(animMappingTable_1_04_to_1_02) / sizeof(animMappingTable_1_04_to_1_02[0])}}, // Here we don't subtract 1 becasue the animmappingtable has no entry for MAX_TOTALANIMS anyway
				{jk2dm16AnimsToGeneral,(sizeof(jk2dm16AnimsToGeneral) / sizeof(jk2dm16AnimsToGeneral[0])) - 1}, //  BUT are mising MAX_TOTALANIMS after MAX_ANIMS. So we subtract 1.
			},
			{{},{jk2spEntityTypeToGeneral,sizeof(jk2spEntityTypeToGeneral) / sizeof(jk2spEntityTypeToGeneral[0])}},
			{{},{jk2spGametypeToGeneral,sizeof(jk2spGametypeToGeneral) / sizeof(jk2spGametypeToGeneral[0])}},
		},
		{
			{entityStateFields_jk2sp,sizeof(entityStateFields_jk2sp) / sizeof(entityStateFields_jk2sp[0]),},
			{playerStateFields_jk2sp,sizeof(playerStateFields_jk2sp) / sizeof(playerStateFields_jk2sp[0]),}
		},
		MAX_CONFIGSTRINGS_JK2SP,//MAX_CONFIGSTRINGS,
		{CS_MODELS_JK2SP,CS_SOUNDS_JK2SP,CS_PLAYERS_JK2SP,ET_EVENTS_JK2SP,EF_MISSILE_STICK_JK2SP,ANIM_TOGGLEBIT,CS_LEVEL_START_TIME, EF_TELEPORT_BIT, CS_MAX_JK2SP},
		1 // Just the main player
	},
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
			{{},{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral) / sizeof(jk2EntityEventsToGeneral[0])}},
			{{},{jk2WeaponsToGeneral,sizeof(jk2WeaponsToGeneral) / sizeof(jk2WeaponsToGeneral[0])}},
			{{},{jk2modToGeneralMap,sizeof(jk2modToGeneralMap) / sizeof(jk2modToGeneralMap[0])}},
			{{},{saberMoveJK2ToGeneral,sizeof(saberMoveJK2ToGeneral) / sizeof(saberMoveJK2ToGeneral[0]),1}},
			{{},{jk2ItemListToGeneral,sizeof(jk2ItemListToGeneral) / sizeof(jk2ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},animMappingTable_1_02_to_1_04,sizeof(animMappingTable_1_02_to_1_04) / sizeof(animMappingTable_1_02_to_1_04[0])}},
				{jk2dm15AnimsToGeneral,sizeof(jk2dm15AnimsToGeneral) / sizeof(jk2dm15AnimsToGeneral[0])},
			},
			{{},{jk2EntityTypeToGeneral,sizeof(jk2EntityTypeToGeneral) / sizeof(jk2EntityTypeToGeneral[0])}},
			{{},{jk2GametypeToGeneral,sizeof(jk2GametypeToGeneral) / sizeof(jk2GametypeToGeneral[0])}},
		},
		{
			{entityStateFields15,sizeof(entityStateFields15) / sizeof(entityStateFields15[0]),},
			{playerStateFields15,sizeof(playerStateFields15) / sizeof(playerStateFields15[0]),}
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
			{{},{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral) / sizeof(jk2EntityEventsToGeneral[0])}},
			{{},{jk2WeaponsToGeneral,sizeof(jk2WeaponsToGeneral) / sizeof(jk2WeaponsToGeneral[0])}},
			{{},{jk2modToGeneralMap,sizeof(jk2modToGeneralMap) / sizeof(jk2modToGeneralMap[0])}},
			{{},{saberMoveJK2ToGeneral,sizeof(saberMoveJK2ToGeneral) / sizeof(saberMoveJK2ToGeneral[0]),1}},
			{{},{jk2ItemListToGeneral,sizeof(jk2ItemListToGeneral) / sizeof(jk2ItemListToGeneral[0])}},
			{
				{{{DM_15},animMappingTable_1_04_to_1_02,sizeof(animMappingTable_1_04_to_1_02) / sizeof(animMappingTable_1_04_to_1_02[0])}},
				{jk2dm16AnimsToGeneral,sizeof(jk2dm16AnimsToGeneral) / sizeof(jk2dm16AnimsToGeneral[0])},
			},
			{{},{jk2EntityTypeToGeneral,sizeof(jk2EntityTypeToGeneral) / sizeof(jk2EntityTypeToGeneral[0])}},
			{{},{jk2GametypeToGeneral,sizeof(jk2GametypeToGeneral) / sizeof(jk2GametypeToGeneral[0])}},
		},
		{
			{entityStateFields,	sizeof(entityStateFields) / sizeof(entityStateFields[0]),},
			{playerStateFields,	sizeof(playerStateFields) / sizeof(playerStateFields[0]),}
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
			{{},{jk2EntityEventsToGeneral,sizeof(jk2EntityEventsToGeneral) / sizeof(jk2EntityEventsToGeneral[0])}},
			{{},{jk2WeaponsToGeneral,sizeof(jk2WeaponsToGeneral) / sizeof(jk2WeaponsToGeneral[0])}},
			{{},{jk2modToGeneralMap,sizeof(jk2modToGeneralMap) / sizeof(jk2modToGeneralMap[0])}},
			{{},{saberMoveJK2ToGeneral,sizeof(saberMoveJK2ToGeneral) / sizeof(saberMoveJK2ToGeneral[0]),1}},
			{{},{jk2ItemListToGeneral,sizeof(jk2ItemListToGeneral) / sizeof(jk2ItemListToGeneral[0])}},
			{
				{{{DM_15},animMappingTable_1_04_to_1_02,sizeof(animMappingTable_1_04_to_1_02) / sizeof(animMappingTable_1_04_to_1_02[0])}},
				{jk2dm16AnimsToGeneral,sizeof(jk2dm16AnimsToGeneral) / sizeof(jk2dm16AnimsToGeneral[0])},
			},
			{{},{jk2EntityTypeToGeneral,sizeof(jk2EntityTypeToGeneral) / sizeof(jk2EntityTypeToGeneral[0])}},
			{{},{jk2GametypeToGeneral,sizeof(jk2GametypeToGeneral) / sizeof(jk2GametypeToGeneral[0])}},
		},
		{
			{entityStateFields,	sizeof(entityStateFields) / sizeof(entityStateFields[0]),},
			{playerStateFields,	sizeof(playerStateFields) / sizeof(playerStateFields[0]),}
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
			{
				{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
				{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},jkaWeaponToJK2Map,sizeof(jkaWeaponToJK2Map) / sizeof(jkaWeaponToJK2Map[0])}},
				{jkaWeaponToGeneralMap,sizeof(jkaWeaponToGeneralMap) / sizeof(jkaWeaponToGeneralMap[0])},
			},{
				{},
				{jkaModToGeneralMap,sizeof(jkaModToGeneralMap) / sizeof(jkaModToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},lsMoveJKAToJK2,sizeof(lsMoveJKAToJK2) / sizeof(lsMoveJKAToJK2[0]),1}}, // last parameter offset 1 because the values start at -1
				{lsMoveJKAToGeneral,sizeof(lsMoveJKAToGeneral) / sizeof(lsMoveJKAToGeneral[0]),1},
			},
			{{},{jkaItemListToGeneral,sizeof(jkaItemListToGeneral) / sizeof(jkaItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},jkaAnimToDM16Mapping,sizeof(jkaAnimToDM16Mapping) / sizeof(jkaAnimToDM16Mapping[0])}},
				{jkaAnimToGeneral,sizeof(jkaAnimToGeneral) / sizeof(jkaAnimToGeneral[0])},
			},
			{{},{jkaEntityTypeToGeneral,sizeof(jkaEntityTypeToGeneral) / sizeof(jkaEntityTypeToGeneral[0])}},
			{{},{jkaGametypeToGeneral,sizeof(jkaGametypeToGeneral) / sizeof(jkaGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsJKA,sizeof(entityStateFieldsJKA) / sizeof(entityStateFieldsJKA[0]),},
			{playerStateFieldsJKA,sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]),},
			qtrue // The playerstate stuff is actually more convoluted in jka, need special handling that cant be represented purely in terms of this table
		},
		MAX_CONFIGSTRINGS_JKA,
		{CS_MODELS_JKA,CS_SOUNDS_JKA,CS_PLAYERS_JKA,ET_EVENTS_JKA,EF_MISSILE_STICK_JKA,ANIM_TOGGLEBIT,CS_LEVEL_START_TIME_JKA, EF_TELEPORT_BIT_JKA, CS_MAX_JKA},
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
			{
				{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
				{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},jkaWeaponToJK2Map,sizeof(jkaWeaponToJK2Map) / sizeof(jkaWeaponToJK2Map[0])}},
				{jkaWeaponToGeneralMap,sizeof(jkaWeaponToGeneralMap) / sizeof(jkaWeaponToGeneralMap[0])},
			},{
				{},
				{jkaModToGeneralMap,sizeof(jkaModToGeneralMap) / sizeof(jkaModToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},lsMoveJKAToJK2,sizeof(lsMoveJKAToJK2) / sizeof(lsMoveJKAToJK2[0]),1}}, // last parameter offset 1 because the values start at -1
				{lsMoveJKAToGeneral,sizeof(lsMoveJKAToGeneral) / sizeof(lsMoveJKAToGeneral[0]),1},
			},
			{{},{jkaItemListToGeneral,sizeof(jkaItemListToGeneral) / sizeof(jkaItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},jkaAnimToDM16Mapping,sizeof(jkaAnimToDM16Mapping) / sizeof(jkaAnimToDM16Mapping[0])}},
				{jkaAnimToGeneral,sizeof(jkaAnimToGeneral) / sizeof(jkaAnimToGeneral[0])},
			},
			{{},{jkaEntityTypeToGeneral,sizeof(jkaEntityTypeToGeneral) / sizeof(jkaEntityTypeToGeneral[0])}},
			{{},{jkaGametypeToGeneral,sizeof(jkaGametypeToGeneral) / sizeof(jkaGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsJKA,sizeof(entityStateFieldsJKA) / sizeof(entityStateFieldsJKA[0]),},
			{playerStateFieldsJKA,sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]),},
			qtrue // The playerstate stuff is actually more convoluted in jka, need special handling that cant be represented purely in terms of this table
		},
		MAX_CONFIGSTRINGS_JKA,
		{CS_MODELS_JKA,CS_SOUNDS_JKA,CS_PLAYERS_JKA,ET_EVENTS_JKA,EF_MISSILE_STICK_JKA,ANIM_TOGGLEBIT,CS_LEVEL_START_TIME_JKA, EF_TELEPORT_BIT_JKA, CS_MAX_JKA},
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
			{
				{{{DM_15,DM_15_1_03,DM_16},jkaEventToJk2Map,sizeof(jkaEventToJk2Map) / sizeof(jkaEventToJk2Map[0])}},
				{jkaEventToGeneralMap,sizeof(jkaEventToGeneralMap) / sizeof(jkaEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},jkaWeaponToJK2Map,sizeof(jkaWeaponToJK2Map) / sizeof(jkaWeaponToJK2Map[0])}},
				{jkaWeaponToGeneralMap,sizeof(jkaWeaponToGeneralMap) / sizeof(jkaWeaponToGeneralMap[0])},
			},{
				{},
				{jkaModToGeneralMap,sizeof(jkaModToGeneralMap) / sizeof(jkaModToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},lsMoveJKAToJK2,sizeof(lsMoveJKAToJK2) / sizeof(lsMoveJKAToJK2[0]),1}}, // last parameter offset 1 because the values start at -1
				{lsMoveJKAToGeneral,sizeof(lsMoveJKAToGeneral) / sizeof(lsMoveJKAToGeneral[0]),1},
			},
			{{},{jkaItemListToGeneral,sizeof(jkaItemListToGeneral) / sizeof(jkaItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},jkaAnimToDM16Mapping,sizeof(jkaAnimToDM16Mapping) / sizeof(jkaAnimToDM16Mapping[0])}},
				{jkaAnimToGeneral,sizeof(jkaAnimToGeneral) / sizeof(jkaAnimToGeneral[0])},
			},
			{{},{jkaEntityTypeToGeneral,sizeof(jkaEntityTypeToGeneral) / sizeof(jkaEntityTypeToGeneral[0])}},
			{{},{jkaGametypeToGeneral,sizeof(jkaGametypeToGeneral) / sizeof(jkaGametypeToGeneral[0])}},
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
			{
				{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map) / sizeof(q3dm68EventToJk2Map[0])}},
				{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},q3WeaponToJK2Map,sizeof(q3WeaponToJK2Map) / sizeof(q3WeaponToJK2Map[0])}},
				{q3WeaponToGeneralMap,sizeof(q3WeaponToGeneralMap) / sizeof(q3WeaponToGeneralMap[0])},
			},{
				{},
				{q3ModToGeneralMap,sizeof(q3ModToGeneralMap) / sizeof(q3ModToGeneralMap[0])},
			},
			{{},{lsMoveQ3ToGeneral,sizeof(lsMoveQ3ToGeneral) / sizeof(lsMoveQ3ToGeneral[0])}},
			{{},{q3ItemListToGeneral,sizeof(q3ItemListToGeneral) / sizeof(q3ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},q3AnimToDM16,sizeof(q3AnimToDM16) / sizeof(q3AnimToDM16[0])}},
				{q3AnimsToGeneral,sizeof(q3AnimsToGeneral) / sizeof(q3AnimsToGeneral[0])},
			},
			{{},{q3EntityTypeToGeneral,sizeof(q3EntityTypeToGeneral) / sizeof(q3EntityTypeToGeneral[0])}},
			{{},{q3GameTypeToGeneral,sizeof(q3GameTypeToGeneral) / sizeof(q3GameTypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQ3DM68,sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),},
			{playerStateFieldsQ3DM68,sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),}
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3,-1,ANIM_TOGGLEBIT_Q3, CS_LEVEL_START_TIME_Q3,EF_TELEPORT_BIT_Q3,CS_MAX_Q3},
		MAX_CLIENTS_Q3
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
			{
				{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map) / sizeof(q3dm68EventToJk2Map[0])}},
				{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},q3WeaponToJK2Map,sizeof(q3WeaponToJK2Map) / sizeof(q3WeaponToJK2Map[0])}},
				{q3WeaponToGeneralMap,sizeof(q3WeaponToGeneralMap) / sizeof(q3WeaponToGeneralMap[0])},
			},{
				{},
				{q3ModToGeneralMap,sizeof(q3ModToGeneralMap) / sizeof(q3ModToGeneralMap[0])},
			},
			{{},{lsMoveQ3ToGeneral,sizeof(lsMoveQ3ToGeneral) / sizeof(lsMoveQ3ToGeneral[0])}},
			{{},{q3ItemListToGeneral,sizeof(q3ItemListToGeneral) / sizeof(q3ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},q3AnimToDM16,sizeof(q3AnimToDM16) / sizeof(q3AnimToDM16[0])}},
				{q3AnimsToGeneral,sizeof(q3AnimsToGeneral) / sizeof(q3AnimsToGeneral[0])},
			},
			{{},{q3EntityTypeToGeneral,sizeof(q3EntityTypeToGeneral) / sizeof(q3EntityTypeToGeneral[0])}},
			{{},{q3GameTypeToGeneral,sizeof(q3GameTypeToGeneral) / sizeof(q3GameTypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQ3DM68,sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),},
			{playerStateFieldsQ3DM68,sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),}
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3,-1,ANIM_TOGGLEBIT_Q3, CS_LEVEL_START_TIME_Q3,EF_TELEPORT_BIT_Q3,CS_MAX_Q3},
		MAX_CLIENTS_Q3
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
			{
				{{{DM_15,DM_15_1_03,DM_16},q3dm68EventToJk2Map,sizeof(q3dm68EventToJk2Map) / sizeof(q3dm68EventToJk2Map[0])}},
				{q3dm68EventToGeneralMap,sizeof(q3dm68EventToGeneralMap) / sizeof(q3dm68EventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},q3WeaponToJK2Map,sizeof(q3WeaponToJK2Map) / sizeof(q3WeaponToJK2Map[0])}},
				{q3WeaponToGeneralMap,sizeof(q3WeaponToGeneralMap) / sizeof(q3WeaponToGeneralMap[0])},
			},{
				{},
				{q3ModToGeneralMap,sizeof(q3ModToGeneralMap) / sizeof(q3ModToGeneralMap[0])},
			},
			{{},{lsMoveQ3ToGeneral,sizeof(lsMoveQ3ToGeneral) / sizeof(lsMoveQ3ToGeneral[0])}},
			{{},{q3ItemListToGeneral,sizeof(q3ItemListToGeneral) / sizeof(q3ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},q3AnimToDM16,sizeof(q3AnimToDM16) / sizeof(q3AnimToDM16[0])}},
				{q3AnimsToGeneral,sizeof(q3AnimsToGeneral) / sizeof(q3AnimsToGeneral[0])},
			},
			{{},{q3EntityTypeToGeneral,sizeof(q3EntityTypeToGeneral) / sizeof(q3EntityTypeToGeneral[0])}},
			{{},{q3GameTypeToGeneral,sizeof(q3GameTypeToGeneral) / sizeof(q3GameTypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQ3DM68,sizeof(entityStateFieldsQ3DM68) / sizeof(entityStateFieldsQ3DM68[0]),},
			{playerStateFieldsQ3DM68,sizeof(playerStateFieldsQ3DM68) / sizeof(playerStateFieldsQ3DM68[0]),}
		},
		MAX_CONFIGSTRINGS_Q3,
		{CS_MODELS_Q3,CS_SOUNDS_Q3,CS_PLAYERS_Q3,ET_EVENTS_Q3,-1,ANIM_TOGGLEBIT_Q3, CS_LEVEL_START_TIME_Q3,EF_TELEPORT_BIT_Q3,CS_MAX_Q3},
		MAX_CLIENTS_Q3
	},{ // Quake live based on QL
		DM_73,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general,
			// svc_extension follows a svc_EOF, followed by another svc_* ...
			//  this keeps legacy clients compatible.
			svc_extension_general,
			svc_voip_general,     // not wrapped in USE_VOIP, so this value is reserved.
		},
		{
			{
				{},
				{qlEventToGeneralMap,sizeof(qlEventToGeneralMap) / sizeof(qlEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},qlWeaponToJK2Map,sizeof(qlWeaponToJK2Map) / sizeof(qlWeaponToJK2Map[0])}},
				{qlWeaponToGeneralMap,sizeof(qlWeaponToGeneralMap) / sizeof(qlWeaponToGeneralMap[0])},
			},{
				{},
				{qlModToGeneralMap,sizeof(qlModToGeneralMap) / sizeof(qlModToGeneralMap[0])},
			},
			{{},{lsMoveNOSABERToGeneral,sizeof(lsMoveNOSABERToGeneral) / sizeof(lsMoveNOSABERToGeneral[0])}},
			{{},{qldm73ItemListToGeneral,sizeof(qldm73ItemListToGeneral) / sizeof(qldm73ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},qlAnimToDM16,sizeof(qlAnimToDM16) / sizeof(qlAnimToDM16[0])}},
				{qlAnimsToGeneral,sizeof(qlAnimsToGeneral) / sizeof(qlAnimsToGeneral[0])},
			},
			{{},{qlEntityTypeToGeneral,sizeof(qlEntityTypeToGeneral) / sizeof(qlEntityTypeToGeneral[0])}},
			{{},{qlGametypeToGeneral,sizeof(qlGametypeToGeneral) / sizeof(qlGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQL73,sizeof(entityStateFieldsQL73) / sizeof(entityStateFieldsQL73[0]),},
			{playerStateFieldsQLDM73,sizeof(playerStateFieldsQLDM73) / sizeof(playerStateFieldsQLDM73[0]),}
		},
		MAX_CONFIGSTRINGS_QL,
		{CS_MODELS_QL,CS_SOUNDS_QL,CS_PLAYERS_QL,ET_EVENTS_QL,-1,ANIM_TOGGLEBIT_QL,CS_LEVEL_START_TIME_QL},
		MAX_CLIENTS_QL
	},{ // Quake live based on QL
		DM_90,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general,
			// svc_extension follows a svc_EOF, followed by another svc_* ...
			//  this keeps legacy clients compatible.
			svc_extension_general,
			svc_voip_general,     // not wrapped in USE_VOIP, so this value is reserved.
		},
		{
			{
				{},
				{qlEventToGeneralMap,sizeof(qlEventToGeneralMap) / sizeof(qlEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},qlWeaponToJK2Map,sizeof(qlWeaponToJK2Map) / sizeof(qlWeaponToJK2Map[0])}},
				{qlWeaponToGeneralMap,sizeof(qlWeaponToGeneralMap) / sizeof(qlWeaponToGeneralMap[0])},
			},{
				{},
				{qlModToGeneralMap,sizeof(qlModToGeneralMap) / sizeof(qlModToGeneralMap[0])},
			},
			{{},{lsMoveNOSABERToGeneral,sizeof(lsMoveNOSABERToGeneral) / sizeof(lsMoveNOSABERToGeneral[0])}},
			{{},{qldm90ItemListToGeneral,sizeof(qldm90ItemListToGeneral) / sizeof(qldm90ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},qlAnimToDM16,sizeof(qlAnimToDM16) / sizeof(qlAnimToDM16[0])}},
				{qlAnimsToGeneral,sizeof(qlAnimsToGeneral) / sizeof(qlAnimsToGeneral[0])},
			},
			{{},{qlEntityTypeToGeneral,sizeof(qlEntityTypeToGeneral) / sizeof(qlEntityTypeToGeneral[0])}},
			{{},{qlGametypeToGeneral,sizeof(qlGametypeToGeneral) / sizeof(qlGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQLDM90,sizeof(entityStateFieldsQLDM90) / sizeof(entityStateFieldsQLDM90[0]),},
			{PlayerStateFieldsQLDM90,sizeof(PlayerStateFieldsQLDM90) / sizeof(PlayerStateFieldsQLDM90[0]),}
		},
		MAX_CONFIGSTRINGS_QL,
		{CS_MODELS_QL,CS_SOUNDS_QL,CS_PLAYERS_QL,ET_EVENTS_QL,-1,ANIM_TOGGLEBIT_QL,CS_LEVEL_START_TIME_QL},
		MAX_CLIENTS_QL
	},{ // Quake live based on QL
		DM_91,  // Yes, I added this entry. This doesn't actually mean that this is implemented ok? It's just in case I implement it in the future.
		{
			svc_bad_general,
			svc_nop_general,
			svc_gamestate_general,
			svc_configstring_general,			// [short] [string] only in gamestate messages
			svc_baseline_general,				// only in gamestate messages
			svc_serverCommand_general,			// [string] to be executed by client game module
			svc_download_general,				// [short] size [size bytes]
			svc_snapshot_general,
			svc_EOF_general,
			// svc_extension follows a svc_EOF, followed by another svc_* ...
			//  this keeps legacy clients compatible.
			svc_extension_general,
			svc_voip_general,     // not wrapped in USE_VOIP, so this value is reserved.
		},
		{
			{
				{},
				{qlEventToGeneralMap,sizeof(qlEventToGeneralMap) / sizeof(qlEventToGeneralMap[0])},
			},{
				{{{DM_15,DM_15_1_03,DM_16},qlWeaponToJK2Map,sizeof(qlWeaponToJK2Map) / sizeof(qlWeaponToJK2Map[0])}},
				{qlWeaponToGeneralMap,sizeof(qlWeaponToGeneralMap) / sizeof(qlWeaponToGeneralMap[0])},
			},{
				{},
				{qlModToGeneralMap,sizeof(qlModToGeneralMap) / sizeof(qlModToGeneralMap[0])},
			},
			{{},{lsMoveNOSABERToGeneral,sizeof(lsMoveNOSABERToGeneral) / sizeof(lsMoveNOSABERToGeneral[0])}},
			{{},{qldm91ItemListToGeneral,sizeof(qldm91ItemListToGeneral) / sizeof(qldm91ItemListToGeneral[0])}},
			{
				{{{DM_15_1_03,DM_16},qlAnimToDM16,sizeof(qlAnimToDM16) / sizeof(qlAnimToDM16[0])}},
				{qlAnimsToGeneral,sizeof(qlAnimsToGeneral) / sizeof(qlAnimsToGeneral[0])},
			},
			{{},{qlEntityTypeToGeneral,sizeof(qlEntityTypeToGeneral) / sizeof(qlEntityTypeToGeneral[0])}},
			{{},{qlGametypeToGeneral,sizeof(qlGametypeToGeneral) / sizeof(qlGametypeToGeneral[0])}},
		},
		{
			{entityStateFieldsQLDM91,sizeof(entityStateFieldsQLDM91) / sizeof(entityStateFieldsQLDM91[0]),},
			{PlayerStateFieldsQLDM91,sizeof(PlayerStateFieldsQLDM91) / sizeof(PlayerStateFieldsQLDM91[0]),}
		},
		MAX_CONFIGSTRINGS_QL,
		{CS_MODELS_QL,CS_SOUNDS_QL,CS_PLAYERS_QL,ET_EVENTS_QL,-1,ANIM_TOGGLEBIT_QL,CS_LEVEL_START_TIME_QL},
		MAX_CLIENTS_QL
	},
};


// Multi game related
specializedGameMappingsContainer_t specializedMappings[GAMEMAPPINGTYPES_COUNT][DEMOTYPE_COUNT][DEMOTYPE_COUNT]{};
gameInfo_t* gameInfosMapped[DEMOTYPE_COUNT]{}; // Is auto-filled by democutter tools.
static qboolean gameInfosInitialized = qfalse;

void initializeGameInfos() {

	constexpr arrayAccessType_t A = UNSAFE;

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
			if (!gameInfos[i].constants.et_events) gameInfos[i].constants.et_events = ET_EVENTS_JK2;
			if (!gameInfos[i].constants.ef_missile_stick) gameInfos[i].constants.ef_missile_stick = EF_MISSILE_STICK;
			if (gameInfos[i].constants.ef_missile_stick == -1) gameInfos[i].constants.ef_missile_stick = NULL; // Q3 for example doesn't have mixxile stick at all.
			if (!gameInfos[i].constants.anim_togglebit) gameInfos[i].constants.anim_togglebit = ANIM_TOGGLEBIT;
			if (!gameInfos[i].constants.cs_level_start_time) gameInfos[i].constants.cs_level_start_time = CS_LEVEL_START_TIME;
			if (!gameInfos[i].constants.ef_teleportbit) gameInfos[i].constants.ef_teleportbit = EF_TELEPORT_BIT;
			if (!gameInfos[i].constants.cs_max) gameInfos[i].constants.cs_max = CS_MAX_JK2;
			if (!gameInfos[i].maxClients) gameInfos[i].maxClients = MAX_CLIENTS;

			// BTw for dynamically created arrays here: We don't care about destroying them. They are created one time at startup and that's it. When program closes they disappear too.

			for (int gmapType = 0; gmapType < GAMEMAPPINGTYPES_COUNT; gmapType++) {
				if (!gameInfos[i].mappings[gmapType].reversedMapping.data) {
					gameInfos[i].mappings[gmapType].reversedMapping = { new const int[gameMappingTypeGeneralArrayLength[gmapType]] {}, gameMappingTypeGeneralArrayLength[gmapType], gameMappingTypeGeneralValueOffset[gmapType] }; // we add +1 because we wanna map the Max_ thing too and not cause writing to some random memory location, damaging it and causing crashes
					for (int gameValue = gameInfos[i].mappings[gmapType].mapping.count - 1 - gameInfos[i].mappings[gmapType].mapping.offset; gameValue >= 0- gameInfos[i].mappings[gmapType].mapping.offset; gameValue--) { // We do this in reverse so that svc_bad always ends up 0 in the reverse lookup. Prolly irrelevant because it's ... not supposed to be really used, but its neater.
						int generalValue = gameInfos[i].mappings[gmapType].mapping.get<A>(gameValue);
						*(int*)&gameInfos[i].mappings[gmapType].reversedMapping.get<A>(generalValue) = gameValue; // We cast the const int to int. We wanna be able to change it here to give it the correct values. It's just not supposed to be modified elsewhere
					}
				}

				for (int s = 0; s < MAX_SPECIALIZED_MAPPINGS; s++) {
					if (gameInfos[i].mappings[gmapType].specializedMappings[s].mapping.data) { // Will be a null ptr if it wasnt initialized/added
						for (int td = 0; td < MAX_MAPPING_TARGET_DEMOTYPES; td++) {
							if (gameInfos[i].mappings[gmapType].specializedMappings[s].targetDemoTypes[td]) {
								specializedMappings[gmapType][gameInfos[i].demoType][gameInfos[i].mappings[gmapType].specializedMappings[s].targetDemoTypes[td]].mapping = gameInfos[i].mappings[gmapType].specializedMappings[s].mapping;
							}
						}
					}
				}
			}
			/*// We create the reverse event lookup table unless the programmer has specifically decided to provide one (no idea why anyone would do that but oh well).
			if (!gameInfos[i].mappings[GMAP_EVENTS].reversedMapping.data) {
				gameInfos[i].mappings[GMAP_EVENTS].reversedMapping = { new const int[EV_ENTITY_EVENT_COUNT_GENERAL] {}, EV_ENTITY_EVENT_COUNT_GENERAL };
				for (int gameEvent = gameInfos[i].mappings[GMAP_EVENTS].mapping.count - 1; gameEvent >= 0; gameEvent--) { // We do this in reverse so that svc_bad always ends up 0 in the reverse lookup. Prolly irrelevant because it's ... not supposed to be really used, but its neater.
					entity_event_t generalEvent = (entity_event_t)gameInfos[i].mappings[GMAP_EVENTS].mapping.data[gameEvent];
					((int*)gameInfos[i].mappings[GMAP_EVENTS].reversedMapping.data)[generalEvent] = gameEvent; // We cast the const int to int. We wanna be able to change it here to give it the correct values. It's just not supposed to be modified elsewhere
				}
			}
			
			// We create the reverse weapon lookup table unless the programmer has specifically decided to provide one (no idea why anyone would do that but oh well).
			if (!gameInfos[i].mappings[GMAP_WEAPONS].reversedMapping.data) {
				gameInfos[i].mappings[GMAP_WEAPONS].reversedMapping = { new const int[WP_NUM_WEAPONS_GENERAL] {}_JK2, WP_NUM_WEAPONS_GENERAL };
				for (int gameWeapon = gameInfos[i].mappings[GMAP_WEAPONS].mapping.count - 1; gameWeapon >= 0; gameWeapon--) { // We do this in reverse so that svc_bad always ends up 0 in the reverse lookup. Prolly irrelevant because it's ... not supposed to be really used, but its neater.
					weapon_t generalWeapon = (weapon_t)gameInfos[i].mappings[GMAP_WEAPONS].mapping.data[gameWeapon];
					((int*)gameInfos[i].mappings[GMAP_WEAPONS].reversedMapping.data)[generalWeapon] = gameWeapon; // We cast the const int to int. We wanna be able to change it here to give it the correct values. It's just not supposed to be modified elsewhere
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
				if (gameInfos[i].mappings.specializedWeaponMappings[s].mapping.data) { // Will be a null ptr if it wasnt initialized/added
					for (int td = 0; td < MAX_MAPPING_TARGET_DEMOTYPES; td++) {
						if (gameInfos[i].mappings.specializedWeaponMappings[s].targetDemoTypes[td]) {
							specializedMappings[gameInfos[i].demoType][gameInfos[i].mappings.specializedWeaponMappings[s].targetDemoTypes[td]].weaponMapping = gameInfos[i].mappings.specializedWeaponMappings[s].mapping;
						}
					}
				}
			}*/
		}
		gameInfosInitialized = qtrue;
	}
}














const char* metaEventKeyNames[METAEVENT_COUNT] = {
	"tc",
	"ec",
	"c",
	"r",
	"k",
	"d",
	"j",
	"sh",
	"sb",
	"ef",
	"l",
	//"hl" // not really used, internal for democutter toools
};


#include <whereami.c> // cringe but cba adding it to every project just to get a stupid path. thanks for the neat library tho

