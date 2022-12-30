// This is some stuff for my idea of also allowing other game demos as source for generating DM15/DM16 demos.
// Very early stage tho really, just a bit of an idea atm, not a serious effort.


#ifndef __OTHERGAMESTUFF_H__
#define __OTHERGAMESTUFF_H__

#include "anims.h"
#include "demoCut.h"


#define	ANIM_TOGGLEBIT_Q3		128


#define	MAX_CLIENTS_Q3			64		// absolute limit
#define MAX_LOCATIONS_Q3		64
#define	MAX_MODELS_Q3			256		// these are sent over the net as 8 bits
#define	MAX_SOUNDS_Q3			256		// so they cannot be blindly increased

#define	CS_MUSIC_Q3				2
#define	CS_MESSAGE_Q3				3		// from the map worldspawn's message field
#define	CS_MOTD_Q3					4		// g_motd string for server message of the day
#define	CS_WARMUP_Q3				5		// server time when the match will be restarted
#define	CS_SCORES1_Q3				6
#define	CS_SCORES2_Q3				7
#define CS_VOTE_TIME_Q3			8
#define CS_VOTE_STRING_Q3			9
#define	CS_VOTE_YES_Q3				10
#define	CS_VOTE_NO_Q3				11

#define CS_TEAMVOTE_TIME_Q3		12
#define CS_TEAMVOTE_STRING_Q3		14
#define	CS_TEAMVOTE_YES_Q3			16
#define	CS_TEAMVOTE_NO_Q3			18

#define	CS_GAME_VERSION_Q3			20
#define	CS_LEVEL_START_TIME_Q3		21		// so the timer only shows the current level
#define	CS_INTERMISSION_Q3			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS_Q3			23		// string indicating flag status in CTF
#define CS_SHADERSTATE_Q3			24
#define CS_BOTINFO_Q3				25

#define	CS_ITEMS_Q3				27		// string of 0's and 1's that tell which items are present

#define	CS_MODELS_Q3				32
#define	CS_SOUNDS_Q3				(CS_MODELS_Q3+MAX_MODELS_Q3)
#define	CS_PLAYERS_Q3				(CS_SOUNDS_Q3+MAX_SOUNDS_Q3)
#define CS_LOCATIONS_Q3				(CS_PLAYERS_Q3+MAX_CLIENTS_Q3)
#define CS_PARTICLES_Q3				(CS_LOCATIONS_Q3+MAX_LOCATIONS_Q3) 

#define CS_MAX_Q3					(CS_PARTICLES_Q3+MAX_LOCATIONS_Q3)


#define	MAX_CONFIGSTRINGS_Q3	1024



#define	EF_DEAD_Q3				0x00000001		// don't draw a foe marker over players with EF_DEAD
#define EF_TICKING_Q3			0x00000002		// used to make players play the prox mine ticking sound
#define	EF_TELEPORT_BIT_Q3		0x00000004		// toggled every time the origin abruptly changes
#define	EF_AWARD_EXCELLENT_Q3	0x00000008		// draw an excellent sprite
#define EF_PLAYER_EVENT_Q3		0x00000010
#define	EF_BOUNCE_Q3			0x00000010		// for missiles
#define	EF_BOUNCE_HALF_Q3		0x00000020		// for missiles
#define	EF_AWARD_GAUNTLET_Q3	0x00000040		// draw a gauntlet sprite
#define	EF_NODRAW_Q3			0x00000080		// may have an event, but no model (unspawned items)
#define	EF_FIRING_Q3			0x00000100		// for lightning gun
#define	EF_KAMIKAZE_Q3			0x00000200
#define	EF_MOVER_STOP_Q3		0x00000400		// will push otherwise
#define EF_AWARD_CAP_Q3			0x00000800		// draw the capture sprite
#define	EF_TALK_Q3				0x00001000		// draw a talk balloon
#define	EF_CONNECTION_Q3		0x00002000		// draw a connection trouble sprite
#define	EF_VOTED_Q3				0x00004000		// already cast a vote
#define	EF_AWARD_IMPRESSIVE_Q3	0x00008000		// draw an impressive sprite
#define	EF_AWARD_DEFEND_Q3		0x00010000		// draw a defend sprite
#define	EF_AWARD_ASSIST_Q3		0x00020000		// draw a assist sprite
#define EF_AWARD_DENIED_Q3		0x00040000		// denied
#define EF_TEAMVOTED_Q3			0x00080000		// already cast a team vote







// TODO Idk what to do about the mission pack stuff...

static weapon_t q3WeaponMap[]{
	WP_NONE,

	WP_STUN_BATON,//WP_GAUNTLET,
	WP_BLASTER,//WP_MACHINEGUN,
	WP_FLECHETTE,//WP_SHOTGUN,
	WP_DEMP2,//WP_GRENADE_LAUNCHER,
	WP_ROCKET_LAUNCHER,
	WP_REPEATER,//WP_LIGHTNING,
	WP_DISRUPTOR,//WP_RAILGUN,
	WP_REPEATER,//WP_PLASMAGUN,
	WP_BOWCASTER,//WP_BFG,
	WP_NONE,//WP_GRAPPLING_HOOK,
//#ifdef MISSIONPACK
	WP_NONE,//WP_NAILGUN,
	WP_NONE,//WP_PROX_LAUNCHER,
	WP_NONE,//WP_CHAINGUN,
//#endif

	WP_NUM_WEAPONS
};

const static entity_event_t q3dm68EventToGeneralMap[] = {
	EV_NONE_GENERAL,

	EV_FOOTSTEP_GENERAL,
	EV_FOOTSTEP_METAL_GENERAL,
	EV_FOOTSPLASH_GENERAL,
	EV_FOOTWADE_GENERAL,
	EV_SWIM_GENERAL,

	EV_STEP_4_GENERAL,
	EV_STEP_8_GENERAL,
	EV_STEP_12_GENERAL,
	EV_STEP_16_GENERAL,

	EV_FALL_SHORT_GENERAL,
	EV_FALL_MEDIUM_GENERAL,
	EV_FALL_FAR_GENERAL,

	EV_JUMP_PAD_GENERAL,			// boing sound at origin_GENERAL, jump sound on player

	EV_JUMP_GENERAL,
	EV_WATER_TOUCH_GENERAL,	// foot touches
	EV_WATER_LEAVE_GENERAL,	// foot leaves
	EV_WATER_UNDER_GENERAL,	// head touches
	EV_WATER_CLEAR_GENERAL,	// head leaves

	EV_ITEM_PICKUP_GENERAL,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_GENERAL,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_FIRE_WEAPON_GENERAL,

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

	EV_ITEM_RESPAWN_GENERAL,
	EV_ITEM_POP_GENERAL,
	EV_PLAYER_TELEPORT_IN_GENERAL,
	EV_PLAYER_TELEPORT_OUT_GENERAL,

	EV_GRENADE_BOUNCE_GENERAL,		// eventParm will be the soundindex

	EV_GENERAL_SOUND_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_GENERAL,

	EV_BULLET_HIT_FLESH_GENERAL,
	EV_BULLET_HIT_WALL_GENERAL,

	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,
	EV_MISSILE_MISS_METAL_GENERAL,
	EV_RAILTRAIL_GENERAL,
	EV_SHOTGUN_GENERAL,
	EV_BULLET_GENERAL,				// otherEntity is the shooter

	EV_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,
	EV_OBITUARY_GENERAL,

	EV_POWERUP_QUAD_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	EV_POWERUP_REGEN_GENERAL,

	EV_GIB_PLAYER_GENERAL,			// gib a previously living player
	EV_SCOREPLUM_GENERAL,			// score plum

//#ifdef MISSIONPACK
	EV_PROXIMITY_MINE_STICK_GENERAL,
	EV_PROXIMITY_MINE_TRIGGER_GENERAL,
	EV_KAMIKAZE_GENERAL,			// kamikaze explodes
	EV_OBELISKEXPLODE_GENERAL,		// obelisk explodes
	EV_OBELISKPAIN_GENERAL,			// obelisk is in pain
	EV_INVUL_IMPACT_GENERAL,		// invulnerability sphere impact
	EV_JUICED_GENERAL,				// invulnerability juiced effect
	EV_LIGHTNINGBOLT_GENERAL,		// lightning bolt bounced of invulnerability sphere
//#endif

	EV_DEBUG_LINE_GENERAL,
	EV_STOPLOOPINGSOUND_GENERAL,
	EV_TAUNT_GENERAL,
	EV_TAUNT_YES_GENERAL,
	EV_TAUNT_NO_GENERAL,
	EV_TAUNT_FOLLOWME_GENERAL,
	EV_TAUNT_GETFLAG_GENERAL,
	EV_TAUNT_GUARDBASE_GENERAL,
	EV_TAUNT_PATROL_GENERAL
};

static const int q3dm68EventToJk2Map[] = {
	EV_NONE_JK2,

	EV_FOOTSTEP_JK2,
	EV_FOOTSTEP_METAL_JK2,
	EV_FOOTSPLASH_JK2,
	EV_FOOTWADE_JK2,
	EV_SWIM_JK2,

	EV_STEP_4_JK2,
	EV_STEP_8_JK2,
	EV_STEP_12_JK2,
	EV_STEP_16_JK2,

	EV_FALL_JK2,// EV_FALL_SHORT_JK2,
	EV_FALL_JK2,//EV_FALL_MEDIUM_JK2,
	EV_FALL_JK2,//EV_FALL_FAR_JK2,

	EV_JUMP_PAD_JK2,			// boing sound at origin_JK2, jump sound on player

	EV_JUMP_JK2,
	EV_WATER_TOUCH_JK2,	// foot touches
	EV_WATER_LEAVE_JK2,	// foot leaves
	EV_WATER_UNDER_JK2,	// head touches
	EV_WATER_CLEAR_JK2,	// head leaves

	EV_ITEM_PICKUP_JK2,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_JK2,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO_JK2,
	EV_CHANGE_WEAPON_JK2,
	EV_FIRE_WEAPON_JK2,

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

	EV_ITEM_RESPAWN_JK2,
	EV_ITEM_POP_JK2,
	EV_PLAYER_TELEPORT_IN_JK2,
	EV_PLAYER_TELEPORT_OUT_JK2,

	EV_GRENADE_BOUNCE_JK2,		// eventParm will be the soundindex

	EV_GENERAL_SOUND_JK2,
	EV_GLOBAL_SOUND_JK2,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_JK2,

	EV_NONE_JK2,//EV_BULLET_HIT_FLESH_JK2,
	EV_NONE_JK2,//EV_BULLET_HIT_WALL_JK2,

	EV_MISSILE_HIT_JK2,
	EV_MISSILE_MISS_JK2,
	EV_MISSILE_MISS_METAL_JK2,
	EV_NONE_JK2,//EV_RAILTRAIL_JK2,
	EV_NONE_JK2,//EV_SHOTGUN_JK2,
	EV_BULLET_JK2,				// otherEntity is the shooter

	EV_PAIN_JK2,
	EV_DEATH1_JK2,
	EV_DEATH2_JK2,
	EV_DEATH3_JK2,
	EV_OBITUARY_JK2,

	EV_POWERUP_QUAD_JK2,
	EV_POWERUP_BATTLESUIT_JK2,
	EV_NONE_JK2,//EV_POWERUP_REGEN_JK2,

	EV_GIB_PLAYER_JK2,			// gib a previously living player
	EV_SCOREPLUM_JK2,			// score plum

//#ifdef MISSIONPACK
	EV_NONE_JK2,//EV_PROXIMITY_MINE_STICK_JK2,
	EV_NONE_JK2,//EV_PROXIMITY_MINE_TRIGGER_JK2,
	EV_NONE_JK2,//EV_KAMIKAZE_JK2,			// kamikaze explodes
	EV_NONE_JK2,//EV_OBELISKEXPLODE_JK2,		// obelisk explodes
	EV_NONE_JK2,//EV_OBELISKPAIN_JK2,			// obelisk is in pain
	EV_NONE_JK2,//EV_INVUL_IMPACT_JK2,		// invulnerability sphere impact
	EV_NONE_JK2,//EV_JUICED_JK2,				// invulnerability juiced effect
	EV_NONE_JK2,//EV_LIGHTNINGBOLT_JK2,		// lightning bolt bounced of invulnerability sphere
//#endif

	EV_DEBUG_LINE_JK2,
	EV_STOPLOOPINGSOUND_JK2,
	EV_TAUNT_JK2,
	EV_TAUNT_YES_JK2,
	EV_TAUNT_NO_JK2,
	EV_TAUNT_FOLLOWME_JK2,
	EV_TAUNT_GETFLAG_JK2,
	EV_TAUNT_GUARDBASE_JK2,
	EV_TAUNT_PATROL_JK2

};


#define MapQ3DM68EventJK2(ev) (q3dm68EventToJk2Map[(ev)&~EV_EVENT_BITS] | ((ev)&EV_EVENT_BITS))


typedef enum {
	ET_GENERAL_Q3,
	ET_PLAYER_Q3,
	ET_ITEM_Q3,
	ET_MISSILE_Q3,
	ET_MOVER_Q3,
	ET_BEAM_Q3,
	ET_PORTAL_Q3,
	ET_SPEAKER_Q3,
	ET_PUSH_TRIGGER_Q3,
	ET_TELEPORT_TRIGGER_Q3,
	ET_INVISIBLE_Q3,
	ET_GRAPPLE_Q3,				// grapple hooked on wall
	ET_TEAM_Q3,

	ET_EVENTS_Q3
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityTypeQ3DM68_t;


// animations
static animNumber_t q3AnimMapping[] = //# animNumber_e
{
	BOTH_DEATH1,
	BOTH_DEAD1,
	BOTH_DEATH2,
	BOTH_DEAD2,
	BOTH_DEATH3,
	BOTH_DEAD3,

	BOTH_ENGAGETAUNT, //TORSO_GESTURE,

	BOTH_ATTACK1,//TORSO_ATTACK,
	BOTH_ATTACK2,//TORSO_ATTACK2,

	TORSO_DROPWEAP1,//TORSO_DROP,
	TORSO_RAISEWEAP1,//TORSO_RAISE,

	BOTH_STAND3,//TORSO_STAND,
	BOTH_STAND3,//TORSO_STAND2,

	BOTH_CROUCH1WALK,//LEGS_WALKCR,
	BOTH_WALK1,//LEGS_WALK,
	BOTH_RUN1,//LEGS_RUN,
	BOTH_RUNBACK1,//LEGS_BACK,
	BOTH_SWIMFORWARD,//LEGS_SWIM,

	BOTH_JUMP1,//LEGS_JUMP,
	BOTH_LAND1,//LEGS_LAND,

	BOTH_JUMPBACK1,//LEGS_JUMPB,
	BOTH_LANDBACK1,//LEGS_LANDB,

	BOTH_STAND1,//LEGS_IDLE,
	BOTH_CROUCH1IDLE,//LEGS_IDLECR,

	LEGS_TURN1,//LEGS_TURN,

	(animNumber_t)0,//TORSO_GETFLAG,
	(animNumber_t)0,//TORSO_GUARDBASE,
	(animNumber_t)0,//TORSO_PATROL,
	(animNumber_t)0,//TORSO_FOLLOWME,
	(animNumber_t)0,//TORSO_AFFIRMATIVE,
	(animNumber_t)0,//TORSO_NEGATIVE,

	MAX_ANIMATIONS,

	BOTH_CROUCH1WALKBACK,//LEGS_BACKCR,
	BOTH_WALKBACK1,//LEGS_BACKWALK,
	(animNumber_t)0,//FLAG_RUN,
	(animNumber_t)0,//FLAG_STAND,
	(animNumber_t)0,//FLAG_STAND2RUN,

	MAX_TOTALANIMATIONS
};


#define MapQ3AnimToJK2(anim) (q3AnimMapping[(anim)&~ANIM_TOGGLEBIT_Q3] | (((anim)&ANIM_TOGGLEBIT_Q3)*ANIM_TOGGLEBIT/ANIM_TOGGLEBIT_Q3))


#endif