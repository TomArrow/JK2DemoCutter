#include "demoCut.h"


// Code is 99%-100% from jomme, from various files.
// Most of it is likely still the same as in the original Jedi Knight source code releases
//


static	int			cmd_argc;
static	char* cmd_argv[MAX_STRING_TOKENS];		// points into cmd_tokenized
static	char		cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];	// will have 0 bytes inserted




int		Cmd_Argc(void) {
	return cmd_argc;
}
char* Cmd_Argv(int arg) {
	if ((unsigned)arg >= cmd_argc) {
		return "";
	}
	return cmd_argv[arg];
}

void Q_strncpyz(char* dest, const char* src, int destsize) {
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

	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = 0;
}

void QDECL Com_Printf(const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	std::cout << msg;
}
void QDECL Com_DPrintf(const char* fmt, ...) {
#ifdef DEBUG
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	std::cout << msg;
#endif
}
void QDECL Com_Error(int ignore, const char* fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
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
		strcat(cmd_args, cmd_argv[i]);
		if (i != cmd_argc - 1) {
			strcat(cmd_args, " ");
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



int Q_vsnprintf(char* str, size_t size, const char* format, va_list ap) {
	int retval;
	retval = _vsnprintf(str, size, format, ap);
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
void QDECL Com_sprintf(char* dest, int size, const char* fmt, ...) {
	int		len;
	va_list		argptr;

	va_start(argptr, fmt);
	len = Q_vsnprintf(dest, size, fmt, argptr);
	va_end(argptr);
	if (len >= size) {
		Com_Printf("Com_sprintf: overflow of %i in %i\n", len, size);
#ifdef	_DEBUG
		__asm {
			int 3;
		}
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

char* QDECL va(const char* format, ...) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char* buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	return buf;
}

void sanitizeFilename(const char* input, char* output) {

	while (*input) {
		if ((*input >= 32 && *input < 42)
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
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char* Info_ValueForKey(const char* s, const char* key) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
											// work without stomping on each other
	static	int	valueindex = 0;
	char* o;

	if (!s || !key) {
		return "";
	}

	if (strlen(s) >= BIG_INFO_STRING) {
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
	std::make_pair(FORCE_LEVEL_0, "_BLU"),
	std::make_pair(FORCE_LEVEL_1, "_BLU"),
	std::make_pair(FORCE_LEVEL_2, "_YEL"),
	std::make_pair(FORCE_LEVEL_3, "_RED"),
	std::make_pair(NUM_FORCE_POWER_LEVELS, "")
};


void BG_PlayerStateToEntityState(playerState_t* ps, entityState_t* s, qboolean snap) {
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

	s->pos.trType = TR_INTERPOLATE;
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