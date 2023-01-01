// This is some stuff for my idea of also allowing JKA demos as source for generating DM15/DM16 demos.
// Very early stage tho really, just a bit of an idea atm, not a serious effort.


#ifndef __JKASTUFF_H__
#define __JKASTUFF_H__

#include "anims.h"
#include "demoCut.h"



// entityState_t->eFlags
#define	EF_G2ANIMATING_JKA			(1<<0)		//perform g2 bone anims based on torsoAnim and legsAnim, works for ET_GENERAL -rww
#define	EF_DEAD_JKA					(1<<1)		// don't draw a foe marker over players with EF_DEAD
//#define	EF_BOUNCE_SHRAPNEL		(1<<2)		// special shrapnel flag
//do not use eflags for server-only things, it wastes bandwidth -rww
#define EF_RADAROBJECT_JKA			(1<<2)		// display on team radar

#define	EF_TELEPORT_BIT_JKA			(1<<3)		// toggled every time the origin abruptly changes

#define	EF_SHADER_ANIM_JKA			(1<<4)		// Animating shader (by s.frame)

#define EF_PLAYER_EVENT_JKA			(1<<5)
//#define	EF_BOUNCE				(1<<5)		// for missiles
//#define	EF_BOUNCE_HALF			(1<<6)		// for missiles
//these aren't even referenced in bg or client code and do not need to be eFlags, so I
//am using these flags for rag stuff -rww

#define EF_RAG_JKA					(1<<6)		//ragdoll him even if he's alive


#define EF_PERMANENT_JKA			(1<<7)		// rww - I am claiming this. (for permanent entities)

#define	EF_NODRAW_JKA				(1<<8)		// may have an event, but no model (unspawned items)
#define	EF_FIRING_JKA				(1<<9)		// for lightning gun
#define EF_ALT_FIRING_JKA			(1<<10)		// for alt-fires, mostly for lightning guns though
#define	EF_JETPACK_ACTIVE_JKA		(1<<11)		//jetpack is activated

#define EF_NOT_USED_1_JKA			(1<<12)		// not used

#define	EF_TALK_JKA					(1<<13)		// draw a talk balloon
#define	EF_CONNECTION_JKA			(1<<14)		// draw a connection trouble sprite
#define	EF_NOT_USED_6_JKA			(1<<15)		// not used

#define	EF_NOT_USED_2_JKA			(1<<16)		// not used
#define	EF_NOT_USED_3_JKA			(1<<17)		// not used
#define	EF_NOT_USED_4_JKA			(1<<18)		// not used

#define	EF_BODYPUSH_JKA				(1<<19)		//rww - claiming this for fullbody push effect

#define	EF_DOUBLE_AMMO_JKA			(1<<20)		// Hacky way to get around ammo max
#define EF_SEEKERDRONE_JKA			(1<<21)		// show seeker drone floating around head
#define EF_MISSILE_STICK_JKA		(1<<22)		// missiles that stick to the wall.
#define EF_ITEMPLACEHOLDER_JKA		(1<<23)		// item effect
#define EF_SOUNDTRACKER_JKA			(1<<24)		// sound position needs to be updated in relation to another entity
#define EF_DROPPEDWEAPON_JKA		(1<<25)		// it's a dropped weapon
#define EF_DISINTEGRATION_JKA		(1<<26)		// being disintegrated by the disruptor
#define EF_INVULNERABLE_JKA			(1<<27)		// just spawned in or whatever, so is protected

#define EF_CLIENTSMOOTH_JKA			(1<<28)		// standard lerporigin smooth override on client

#define EF_JETPACK_JKA				(1<<29)		//rww - wearing a jetpack
#define EF_JETPACK_FLAMING_JKA		(1<<30)		//rww - jetpack fire effect

#define	EF_NOT_USED_5_JKA			(1<<31)		// not used

//These new EF2_??? flags were added for NPCs, they really should not be used often.
//NOTE: we only allow 10 of these!
#define	EF2_HELD_BY_MONSTER_JKA		(1<<0)		// Being held by something, like a Rancor or a Wampa
#define	EF2_USE_ALT_ANIM_JKA		(1<<1)		// For certain special runs/stands for creatures like the Rancor and Wampa whose runs/stands are conditional
#define	EF2_ALERTED_JKA				(1<<2)		// For certain special anims, for Rancor: means you've had an enemy, so use the more alert stand
#define	EF2_GENERIC_NPC_FLAG_JKA	(1<<3)		// So far, used for Rancor...
#define	EF2_FLYING_JKA				(1<<4)		// Flying FIXME: only used on NPCs doesn't *really* have to be passed over, does it?
#define	EF2_HYPERSPACE_JKA			(1<<5)		// Used to both start the hyperspace effect on the predicted client and to let the vehicle know it can now jump into hyperspace (after turning to face the proper angle)
#define	EF2_BRACKET_ENTITY_JKA		(1<<6)		// Draw as bracketed
#define	EF2_SHIP_DEATH_JKA			(1<<7)		// "died in ship" mode
#define	EF2_NOT_USED_1_JKA			(1<<8)		// not used












#define _OPTIMIZED_VEHICLE_NETWORKING
#define BASE_COMPAT


static int jkaModToGeneralMap[]{
	MOD_UNKNOWN_GENERAL,
	MOD_STUN_BATON_GENERAL,
	MOD_MELEE_GENERAL,
	MOD_SABER_GENERAL,
	MOD_BRYAR_PISTOL_GENERAL,
	MOD_BRYAR_PISTOL_ALT_GENERAL,
	MOD_BLASTER_GENERAL,
	MOD_TURBLAST_GENERAL,
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
	MOD_VEHICLE_GENERAL,
	MOD_CONC_GENERAL,
	MOD_CONC_ALT_GENERAL,
	MOD_FORCE_DARK_GENERAL,
	MOD_SENTRY_GENERAL,
	MOD_WATER_GENERAL,
	MOD_SLIME_GENERAL,
	MOD_LAVA_GENERAL,
	MOD_CRUSH_GENERAL,
	MOD_TELEFRAG_GENERAL,
	MOD_FALLING_GENERAL,
	//MOD_COLLISION_GENERAL,		// OpenJK removed this in commit b319c52fd4ed1e4fb5dae4e468a1a791091b63a5
	//MOD_VEH_EXPLOSION_GENERAL,	// Sad.
	MOD_SUICIDE_GENERAL,
	MOD_TARGET_LASER_GENERAL,
	MOD_TRIGGER_HURT_GENERAL,
	MOD_TEAM_CHANGE_GENERAL,
	//AURELIO: when/if you put this back in_GENERAL, remember to make a case for it in all the other places where
	//mod's are checked. Also_GENERAL, it probably isn't the most elegant solution for what you want - just add
	//a frag back to the player after you call the player_die (and keep a local of his pre-death score to
	//make sure he actually lost points_GENERAL, there may be cases where you don't lose points on changing teams
	//or suiciding_GENERAL, and so you would actually be giving him a point) -Rich
	// I put it back in for now_GENERAL, if it becomes a problem we'll work around it later (it shouldn't though)...
	MOD_MAX_GENERAL
};

static int jkaWeaponToJK2Map[] {
	WP_NONE_JK2,

	WP_STUN_BATON_JK2,
	WP_NONE_JK2,//WP_MELEE_JK2,
	WP_SABER_JK2,
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
	WP_NONE_JK2,//WP_CONCUSSION_JK2,
	WP_NONE_JK2,//WP_BRYAR_OLD_JK2,
	WP_EMPLACED_GUN_JK2,
	WP_TURRET_JK2,

	//	WP_GAUNTLET_JK2,
	//	WP_MACHINEGUN_JK2,			// Bryar
	//	WP_SHOTGUN_JK2,				// Blaster
	//	WP_GRENADE_LAUNCHER_JK2,	// Thermal
	//	WP_LIGHTNING_JK2,			// 
	//	WP_RAILGUN_JK2,				// 
	//	WP_GRAPPLING_HOOK_JK2,

	WP_NUM_WEAPONS_JK2
};
static int jkaWeaponToGeneralMap[] {
	WP_NONE_GENERAL,

	WP_STUN_BATON_GENERAL,
	WP_MELEE_GENERAL,
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
	WP_CONCUSSION_GENERAL,
	WP_BRYAR_OLD_GENERAL,
	WP_EMPLACED_GUN_GENERAL,
	WP_TURRET_GENERAL,

	//	WP_GAUNTLET_GENERAL,
	//	WP_MACHINEGUN_GENERAL,			// Bryar
	//	WP_SHOTGUN_GENERAL,				// Blaster
	//	WP_GRENADE_LAUNCHER_GENERAL,	// Thermal
	//	WP_LIGHTNING_GENERAL,			// 
	//	WP_RAILGUN_GENERAL,				// 
	//	WP_GRAPPLING_HOOK_GENERAL,

	WP_NUM_WEAPONS_GENERAL
};

#define	MAX_WEAPONS_JKA				19

const static int jkaEventToGeneralMap[] = {
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

	EV_GHOUL2_MARK_GENERAL,			//create a projectile impact mark on something with a client-side g2 instance.

	EV_GLOBAL_DUEL_GENERAL,
	EV_PRIVATE_DUEL_GENERAL,

	EV_JUMP_GENERAL,
	EV_ROLL_GENERAL,
	EV_WATER_TOUCH_GENERAL,	// foot touches
	EV_WATER_LEAVE_GENERAL,	// foot leaves
	EV_WATER_UNDER_GENERAL,	// head touches
	EV_WATER_CLEAR_GENERAL,	// head leaves

	EV_ITEM_PICKUP_GENERAL,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_GENERAL,	// powerup / team sounds are broadcast to everyone

	EV_VEH_FIRE_GENERAL,

	EV_NOAMMO_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_FIRE_WEAPON_GENERAL,
	EV_ALT_FIRE_GENERAL,
	EV_SABER_ATTACK_GENERAL,
	EV_SABER_HIT_GENERAL,
	EV_SABER_BLOCK_GENERAL,
	EV_SABER_CLASHFLARE_GENERAL,
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

	EV_LOCALTIMER_GENERAL,

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
	EV_PLAY_PORTAL_EFFECT_ID_GENERAL,

	EV_PLAYDOORSOUND_GENERAL,
	EV_PLAYDOORLOOPSOUND_GENERAL,
	EV_BMODEL_SOUND_GENERAL,

	EV_MUTE_SOUND_GENERAL,
	EV_VOICECMD_SOUND_GENERAL,
	EV_GENERAL_SOUND_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_GENERAL,
	EV_ENTITY_SOUND_GENERAL,

	EV_PLAY_ROFF_GENERAL,

	EV_GLASS_SHATTER_GENERAL,
	EV_DEBRIS_GENERAL,
	EV_MISC_MODEL_EXP_GENERAL,

	EV_CONC_ALT_IMPACT_GENERAL,

	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,
	EV_MISSILE_MISS_METAL_GENERAL,
	EV_BULLET_GENERAL,				// otherEntity is the shooter

	EV_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,
	EV_OBITUARY_GENERAL,

#ifdef BASE_COMPAT
	EV_POWERUP_QUAD_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	//EV_POWERUP_REGEN_GENERAL,
#endif

	EV_FORCE_DRAINED_GENERAL,

	EV_GIB_PLAYER_GENERAL,			// gib a previously living player
	EV_SCOREPLUM_GENERAL,			// score plum

	EV_CTFMESSAGE_GENERAL,

	EV_BODYFADE_GENERAL,

	EV_SIEGE_ROUNDOVER_GENERAL,
	EV_SIEGE_OBJECTIVECOMPLETE_GENERAL,

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

	//rww - Begin NPC sound events
	EV_ANGER1_GENERAL,	//Say when acquire an enemy when didn't have one before
	EV_ANGER2_GENERAL,
	EV_ANGER3_GENERAL,

	EV_VICTORY1_GENERAL,	//Say when killed an enemy
	EV_VICTORY2_GENERAL,
	EV_VICTORY3_GENERAL,

	EV_CONFUSE1_GENERAL,	//Say when confused
	EV_CONFUSE2_GENERAL,
	EV_CONFUSE3_GENERAL,

	EV_PUSHED1_GENERAL,		//Say when pushed
	EV_PUSHED2_GENERAL,
	EV_PUSHED3_GENERAL,

	EV_CHOKE1_GENERAL,		//Say when choking
	EV_CHOKE2_GENERAL,
	EV_CHOKE3_GENERAL,

	EV_FFWARN_GENERAL,		//ffire founds
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
};

static const int jkaEventToJk2Map [] ={
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

	EV_NONE_JK2,//EV_GHOUL2_MARK_JK2,			//create a projectile impact mark on something with a client-side g2 instance.

	EV_NONE_JK2,//EV_GLOBAL_DUEL_JK2,
	EV_PRIVATE_DUEL_JK2,

	EV_JUMP_JK2,
	EV_ROLL_JK2,
	EV_WATER_TOUCH_JK2,	// foot touches
	EV_WATER_LEAVE_JK2,	// foot leaves
	EV_WATER_UNDER_JK2,	// head touches
	EV_WATER_CLEAR_JK2,	// head leaves

	EV_ITEM_PICKUP_JK2,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP_JK2,	// powerup / team sounds are broadcast to everyone

	EV_NONE_JK2,//EV_VEH_FIRE_JK2,

	EV_NOAMMO_JK2,
	EV_CHANGE_WEAPON_JK2,
	EV_FIRE_WEAPON_JK2,
	EV_ALT_FIRE_JK2,
	EV_SABER_ATTACK_JK2,
	EV_SABER_HIT_JK2,
	EV_SABER_BLOCK_JK2,
	EV_NONE_JK2,//EV_SABER_CLASHFLARE_JK2,
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

	EV_NONE_JK2,//EV_LOCALTIMER_JK2,

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
	EV_NONE_JK2,//EV_PLAY_PORTAL_EFFECT_ID_JK2,

	EV_NONE_JK2,//EV_PLAYDOORSOUND_JK2,
	EV_NONE_JK2,//EV_PLAYDOORLOOPSOUND_JK2,
	EV_NONE_JK2,//EV_BMODEL_SOUND_JK2,

	EV_MUTE_SOUND_JK2,
	EV_NONE_JK2,//EV_VOICECMD_SOUND_JK2,
	EV_GENERAL_SOUND_JK2,
	EV_GLOBAL_SOUND_JK2,		// no attenuation
	EV_GLOBAL_TEAM_SOUND_JK2,
	EV_ENTITY_SOUND_JK2,

	EV_PLAY_ROFF_JK2,

	EV_GLASS_SHATTER_JK2,
	EV_DEBRIS_JK2,
	EV_NONE_JK2,//EV_MISC_MODEL_EXP_JK2,

	EV_NONE_JK2,//EV_CONC_ALT_IMPACT_JK2,

	EV_MISSILE_HIT_JK2,
	EV_MISSILE_MISS_JK2,
	EV_MISSILE_MISS_METAL_JK2,
	EV_BULLET_JK2,				// otherEntity is the shooter

	EV_PAIN_JK2,
	EV_DEATH1_JK2,
	EV_DEATH2_JK2,
	EV_DEATH3_JK2,
	EV_OBITUARY_JK2,

#ifdef BASE_COMPAT
	EV_POWERUP_QUAD_JK2,
	EV_POWERUP_BATTLESUIT_JK2,
#endif // BASE_COMPAT

	EV_FORCE_DRAINED_JK2,

	EV_GIB_PLAYER_JK2,			// gib a previously living player
	EV_SCOREPLUM_JK2,			// score plum

	EV_CTFMESSAGE_JK2,

	EV_NONE_JK2,//EV_BODYFADE_JK2,

	EV_NONE_JK2,//EV_SIEGE_ROUNDOVER_JK2,
	EV_NONE_JK2,//EV_SIEGE_OBJECTIVECOMPLETE_JK2,

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

	//rww - Begin NPC sound events
	EV_NONE_JK2,//EV_ANGER1_JK2,	//Say when acquire an enemy when didn't have one before
	EV_NONE_JK2,//EV_ANGER2_JK2,
	EV_NONE_JK2,//EV_ANGER3_JK2,

	EV_NONE_JK2,//EV_VICTORY1_JK2,	//Say when killed an enemy
	EV_NONE_JK2,//EV_VICTORY2_JK2,
	EV_NONE_JK2,//EV_VICTORY3_JK2,

	EV_NONE_JK2,//EV_CONFUSE1_JK2,	//Say when confused
	EV_NONE_JK2,//EV_CONFUSE2_JK2,
	EV_NONE_JK2,//EV_CONFUSE3_JK2,

	EV_NONE_JK2,//EV_PUSHED1_JK2,		//Say when pushed
	EV_NONE_JK2,//EV_PUSHED2_JK2,
	EV_NONE_JK2,//EV_PUSHED3_JK2,

	EV_NONE_JK2,//EV_CHOKE1_JK2,		//Say when choking
	EV_NONE_JK2,//EV_CHOKE2_JK2,
	EV_NONE_JK2,//EV_CHOKE3_JK2,

	EV_NONE_JK2,//EV_FFWARN_JK2,		//ffire founds
	EV_NONE_JK2,//EV_FFTURN_JK2,
	//extra sounds for ST
	EV_NONE_JK2,//EV_CHASE1_JK2,
	EV_NONE_JK2,//EV_CHASE2_JK2,
	EV_NONE_JK2,//EV_CHASE3_JK2,
	EV_NONE_JK2,//EV_COVER1_JK2,
	EV_NONE_JK2,//EV_COVER2_JK2,
	EV_NONE_JK2,//EV_COVER3_JK2,
	EV_NONE_JK2,//EV_COVER4_JK2,
	EV_NONE_JK2,//EV_COVER5_JK2,
	EV_NONE_JK2,//EV_DETECTED1_JK2,
	EV_NONE_JK2,//EV_DETECTED2_JK2,
	EV_NONE_JK2,//EV_DETECTED3_JK2,
	EV_NONE_JK2,//EV_DETECTED4_JK2,
	EV_NONE_JK2,//EV_DETECTED5_JK2,
	EV_NONE_JK2,//EV_LOST1_JK2,
	EV_NONE_JK2,//EV_OUTFLANK1_JK2,
	EV_NONE_JK2,//EV_OUTFLANK2_JK2,
	EV_NONE_JK2,//EV_ESCAPING1_JK2,
	EV_NONE_JK2,//EV_ESCAPING2_JK2,
	EV_NONE_JK2,//EV_ESCAPING3_JK2,
	EV_NONE_JK2,//EV_GIVEUP1_JK2,
	EV_NONE_JK2,//EV_GIVEUP2_JK2,
	EV_NONE_JK2,//EV_GIVEUP3_JK2,
	EV_NONE_JK2,//EV_GIVEUP4_JK2,
	EV_NONE_JK2,//EV_LOOK1_JK2,
	EV_NONE_JK2,//EV_LOOK2_JK2,
	EV_NONE_JK2,//EV_SIGHT1_JK2,
	EV_NONE_JK2,//EV_SIGHT2_JK2,
	EV_NONE_JK2,//EV_SIGHT3_JK2,
	EV_NONE_JK2,//EV_SOUND1_JK2,
	EV_NONE_JK2,//EV_SOUND2_JK2,
	EV_NONE_JK2,//EV_SOUND3_JK2,
	EV_NONE_JK2,//EV_SUSPICIOUS1_JK2,
	EV_NONE_JK2,//EV_SUSPICIOUS2_JK2,
	EV_NONE_JK2,//EV_SUSPICIOUS3_JK2,
	EV_NONE_JK2,//EV_SUSPICIOUS4_JK2,
	EV_NONE_JK2,//EV_SUSPICIOUS5_JK2,
	//extra sounds for Jedi
	EV_NONE_JK2,//EV_COMBAT1_JK2,
	EV_NONE_JK2,//EV_COMBAT2_JK2,
	EV_NONE_JK2,//EV_COMBAT3_JK2,
	EV_NONE_JK2,//EV_JDETECTED1_JK2,
	EV_NONE_JK2,//EV_JDETECTED2_JK2,
	EV_NONE_JK2,//EV_JDETECTED3_JK2,
	EV_NONE_JK2,//EV_TAUNT1_JK2,
	EV_NONE_JK2,//EV_TAUNT2_JK2,
	EV_NONE_JK2,//EV_TAUNT3_JK2,
	EV_NONE_JK2,//EV_JCHASE1_JK2,
	EV_NONE_JK2,//EV_JCHASE2_JK2,
	EV_NONE_JK2,//EV_JCHASE3_JK2,
	EV_NONE_JK2,//EV_JLOST1_JK2,
	EV_NONE_JK2,//EV_JLOST2_JK2,
	EV_NONE_JK2,//EV_JLOST3_JK2,
	EV_NONE_JK2,//EV_DEFLECT1_JK2,
	EV_NONE_JK2,//EV_DEFLECT2_JK2,
	EV_NONE_JK2,//EV_DEFLECT3_JK2,
	EV_NONE_JK2,//EV_GLOAT1_JK2,
	EV_NONE_JK2,//EV_GLOAT2_JK2,
	EV_NONE_JK2,//EV_GLOAT3_JK2,
	EV_NONE_JK2,//EV_PUSHFAIL_JK2,

	EV_NONE_JK2,//EV_SIEGESPEC_JK2,

};


#define MapJKAEventJK2(ev) (jkaEventToJk2Map[(ev)&~EV_EVENT_BITS] | ((ev)&EV_EVENT_BITS))


typedef enum {
	ET_GENERAL_JKA,
	ET_PLAYER_JKA,
	ET_ITEM_JKA,
	ET_MISSILE_JKA,
	ET_SPECIAL_JKA,				// rww - force fields
	ET_HOLOCRON_JKA,			// rww - holocron icon displays
	ET_MOVER_JKA,
	ET_BEAM_JKA,
	ET_PORTAL_JKA,
	ET_SPEAKER_JKA,
	ET_PUSH_TRIGGER_JKA,
	ET_TELEPORT_TRIGGER_JKA,
	ET_INVISIBLE_JKA,
	ET_NPC_JKA,					// ghoul2 player-like entity
	ET_TEAM_JKA,
	ET_BODY_JKA,
	ET_TERRAIN_JKA,
	ET_FX_JKA,

	ET_EVENTS_JKA				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityTypeJKA_t;




const int lsMoveJKAToGeneral[] {
	//totally invalid
	LS_INVALID_GENERAL,
	// Invalid, or saber not armed
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
};

const int lsMoveJKAToJK2[] {
	//totally invalid
	LS_NONE_JK2,//LS_INVALID_JK2,
	// Invalid, or saber not armed
	LS_NONE_JK2,

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
	LS_NONE_JK2,//LS_ROLL_STAB_JK2,
	LS_A_LUNGE_JK2,
	LS_A_JUMP_T__B__JK2,
	LS_A_FLIP_STAB_JK2,
	LS_A_FLIP_SLASH_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_DUAL_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_ARIAL_LEFT_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_ARIAL_RIGHT_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_CART_LEFT_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_CART_RIGHT_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_STAFF_LEFT_JK2,
	LS_NONE_JK2,//LS_JUMPATTACK_STAFF_RIGHT_JK2,
	LS_NONE_JK2,//LS_BUTTERFLY_LEFT_JK2,
	LS_NONE_JK2,//LS_BUTTERFLY_RIGHT_JK2,
	LS_NONE_JK2,//LS_A_BACKFLIP_ATK_JK2,
	LS_NONE_JK2,//LS_SPINATTACK_DUAL_JK2,
	LS_NONE_JK2,//LS_SPINATTACK_JK2,
	LS_NONE_JK2,//LS_LEAP_ATTACK_JK2,
	LS_NONE_JK2,//LS_SWOOP_ATTACK_RIGHT_JK2,
	LS_NONE_JK2,//LS_SWOOP_ATTACK_LEFT_JK2,
	LS_NONE_JK2,//LS_TAUNTAUN_ATTACK_RIGHT_JK2,
	LS_NONE_JK2,//LS_TAUNTAUN_ATTACK_LEFT_JK2,
	LS_NONE_JK2,//LS_KICK_F_JK2,
	LS_NONE_JK2,//LS_KICK_B_JK2,
	LS_NONE_JK2,//LS_KICK_R_JK2,
	LS_NONE_JK2,//LS_KICK_L_JK2,
	LS_NONE_JK2,//LS_KICK_S_JK2,
	LS_NONE_JK2,//LS_KICK_BF_JK2,
	LS_NONE_JK2,//LS_KICK_RL_JK2,
	LS_NONE_JK2,//LS_KICK_F_AIR_JK2,
	LS_NONE_JK2,//LS_KICK_B_AIR_JK2,
	LS_NONE_JK2,//LS_KICK_R_AIR_JK2,
	LS_NONE_JK2,//LS_KICK_L_AIR_JK2,
	LS_NONE_JK2,//LS_STABDOWN_JK2,
	LS_NONE_JK2,//LS_STABDOWN_STAFF_JK2,
	LS_NONE_JK2,//LS_STABDOWN_DUAL_JK2,
	LS_NONE_JK2,//LS_DUAL_SPIN_PROTECT_JK2,
	LS_NONE_JK2,//LS_STAFF_SOULCAL_JK2,
	LS_NONE_JK2,//LS_A1_SPECIAL_JK2,
	LS_NONE_JK2,//LS_A2_SPECIAL_JK2,
	LS_NONE_JK2,//LS_A3_SPECIAL_JK2,
	LS_NONE_JK2,//LS_UPSIDE_DOWN_ATTACK_JK2,
	LS_NONE_JK2,//LS_PULL_ATTACK_STAB_JK2,
	LS_NONE_JK2,//LS_PULL_ATTACK_SWING_JK2,
	LS_NONE_JK2,//LS_SPINATTACK_ALORA_JK2,
	LS_NONE_JK2,//LS_DUAL_FB_JK2,
	LS_NONE_JK2,//LS_DUAL_LR_JK2,
	LS_NONE_JK2,//LS_HILT_BASH_JK2,

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

	LS_MOVE_MAX_JK2//
};














// Turned from enum to array, thus reusing the Jedi Outcast animations where they exist. EZ PZ. (idk if this actually works)
static animNumber_t jkaAnimMapping[] = //# animNumber_e
{
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	(animNumber_t)0,//FACE_TALK0,			//# silent
	FACE_TALK1,			//# quiet
	FACE_TALK2,			//# semi-quiet
	FACE_TALK3,			//# semi-loud
	FACE_TALK4,			//# loud
	FACE_ALERT,				//# 
	FACE_SMILE,				//# 
	FACE_FROWN,				//# 
	FACE_DEAD,				//# 

	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1,		//# First Death anim
	BOTH_DEATH2,			//# Second Death anim
	BOTH_DEATH3,			//# Third Death anim
	BOTH_DEATH4,			//# Fourth Death anim
	BOTH_DEATH5,			//# Fifth Death anim
	BOTH_DEATH6,			//# Sixth Death anim
	BOTH_DEATH7,			//# Seventh Death anim
	BOTH_DEATH8,			//# 
	BOTH_DEATH9,			//# 
	BOTH_DEATH10,			//# 
	BOTH_DEATH11,			//#
	BOTH_DEATH12,			//# 
	BOTH_DEATH13,			//# 
	BOTH_DEATH14,			//# 
	BOTH_DEATH15,			//# 
	BOTH_DEATH16,			//# 
	BOTH_DEATH17,			//# 
	BOTH_DEATH18,			//# 
	BOTH_DEATH19,			//# 
	BOTH_DEATH20,			//# 
	BOTH_DEATH21,			//# 
	BOTH_DEATH22,			//# 
	BOTH_DEATH23,			//# 
	BOTH_DEATH24,			//# 
	BOTH_DEATH25,			//# 

	BOTH_DEATHFORWARD1,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE,		//# Idle while close to death
	BOTH_LYINGDEATH1,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL,		//# Death anim from a roll
	BOTH_DEATH_FLIP,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1,				//# First Death finished pose
	BOTH_DEAD2,				//# Second Death finished pose
	BOTH_DEAD3,				//# Third Death finished pose
	BOTH_DEAD4,				//# Fourth Death finished pose
	BOTH_DEAD5,				//# Fifth Death finished pose
	BOTH_DEAD6,				//# Sixth Death finished pose
	BOTH_DEAD7,				//# Seventh Death finished pose
	BOTH_DEAD8,				//# 
	BOTH_DEAD9,				//# 
	BOTH_DEAD10,			//# 
	BOTH_DEAD11,			//#
	BOTH_DEAD12,			//# 
	BOTH_DEAD13,			//# 
	BOTH_DEAD14,			//# 
	BOTH_DEAD15,			//# 
	BOTH_DEAD16,			//# 
	BOTH_DEAD17,			//# 
	BOTH_DEAD18,			//# 
	BOTH_DEAD19,			//# 
	BOTH_DEAD20,			//# 
	BOTH_DEAD21,			//# 
	BOTH_DEAD22,			//# 
	BOTH_DEAD23,			//# 
	BOTH_DEAD24,			//# 
	BOTH_DEAD25,			//# 
	BOTH_DEADFORWARD1,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2,		//# React to being shot from Second Death finished pose
	BOTH_DISMEMBER_HEAD1,	//#
	BOTH_DISMEMBER_TORSO1,	//#
	BOTH_DISMEMBER_LLEG,	//#
	BOTH_DISMEMBER_RLEG,	//#
	BOTH_DISMEMBER_RARM,	//#
	BOTH_DISMEMBER_LARM,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1,				//# First take pain anim
	BOTH_PAIN2,				//# Second take pain anim
	BOTH_PAIN3,				//# Third take pain anim
	BOTH_PAIN4,				//# Fourth take pain anim
	BOTH_PAIN5,				//# Fifth take pain anim - from behind
	BOTH_PAIN6,				//# Sixth take pain anim - from behind
	BOTH_PAIN7,				//# Seventh take pain anim - from behind
	BOTH_PAIN8,				//# Eigth take pain anim - from behind
	BOTH_PAIN9,				//# 
	BOTH_PAIN10,			//# 
	BOTH_PAIN11,			//# 
	BOTH_PAIN12,			//# 
	BOTH_PAIN13,			//# 
	BOTH_PAIN14,			//# 
	BOTH_PAIN15,			//# 
	BOTH_PAIN16,			//# 
	BOTH_PAIN17,			//# 
	BOTH_PAIN18,			//# 

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1,			//# Attack with stun baton
	BOTH_ATTACK2,			//# Attack with one-handed pistol
	BOTH_ATTACK3,			//# Attack with blaster rifle
	BOTH_ATTACK4,			//# Attack with disruptor
	BOTH_ATTACK5,			//# Another Rancor Attack
	BOTH_ATTACK6,			//# Yet Another Rancor Attack
	BOTH_ATTACK7,			//# Yet Another Rancor Attack
	BOTH_ATTACK10,			//# Attack with thermal det
	BOTH_ATTACK11,			//# "Attack" with tripmine and detpack
	BOTH_MELEE1,			//# First melee attack
	BOTH_MELEE2,			//# Second melee attack
	BOTH_THERMAL_READY,		//# pull back with thermal
	BOTH_THERMAL_THROW,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 1
	BOTH_A1_T__B_,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR,	//# Fast arc right to top right
	BOTH_T1__R_TL,	//# Fast arc right to top left
	BOTH_T1__R__L,	//# Fast weak spin right to left
	BOTH_T1__R_BL,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL,	//# Fast arc top right to top left
	BOTH_T1_TR__L,	//# Fast arc top right to left
	BOTH_T1_TR_BL,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR,	//# Fast arc top to bottom right
	BOTH_T1_T___R,	//# Fast arc top to right
	BOTH_T1_T__TR,	//# Fast arc top to top right
	BOTH_T1_T__TL,	//# Fast arc top to top left
	BOTH_T1_T___L,	//# Fast arc top to left
	BOTH_T1_T__BL,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R,	//# Fast weak spin left to right
	BOTH_T1__L_TL,	//# Fast arc left to top left
	BOTH_T1_BL_BR,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T_,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T_,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T_,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T_,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
	BOTH_B1_BR___,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R___,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR___,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T____,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL___,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L___,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR___,	//# Deflection toward BR
	BOTH_D1__R___,	//# Deflection toward R
	BOTH_D1_TR___,	//# Deflection toward TR
	BOTH_D1_TL___,	//# Deflection toward TL
	BOTH_D1__L___,	//# Deflection toward L
	BOTH_D1_BL___,	//# Deflection toward BL
	BOTH_D1_B____,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B_,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR,	//# Fast arc right to top right
	BOTH_T2__R_TL,	//# Fast arc right to top left
	BOTH_T2__R__L,	//# Fast weak spin right to left
	BOTH_T2__R_BL,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL,	//# Fast arc top right to top left
	BOTH_T2_TR__L,	//# Fast arc top right to left
	BOTH_T2_TR_BL,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR,	//# Fast arc top to bottom right
	BOTH_T2_T___R,	//# Fast arc top to right
	BOTH_T2_T__TR,	//# Fast arc top to top right
	BOTH_T2_T__TL,	//# Fast arc top to top left
	BOTH_T2_T___L,	//# Fast arc top to left
	BOTH_T2_T__BL,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R,	//# Fast weak spin left to right
	BOTH_T2__L_TL,	//# Fast arc left to top left
	BOTH_T2_BL_BR,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T_,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T_,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T_,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T_,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
	BOTH_B2_BR___,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R___,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR___,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T____,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL___,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L___,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR___,	//# Deflection toward BR
	BOTH_D2__R___,	//# Deflection toward R
	BOTH_D2_TR___,	//# Deflection toward TR
	BOTH_D2_TL___,	//# Deflection toward TL
	BOTH_D2__L___,	//# Deflection toward L
	BOTH_D2_BL___,	//# Deflection toward BL
	BOTH_D2_B____,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B_,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR,	//# Fast arc right to top right
	BOTH_T3__R_TL,	//# Fast arc right to top left
	BOTH_T3__R__L,	//# Fast weak spin right to left
	BOTH_T3__R_BL,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL,	//# Fast arc top right to top left
	BOTH_T3_TR__L,	//# Fast arc top right to left
	BOTH_T3_TR_BL,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR,	//# Fast arc top to bottom right
	BOTH_T3_T___R,	//# Fast arc top to right
	BOTH_T3_T__TR,	//# Fast arc top to top right
	BOTH_T3_T__TL,	//# Fast arc top to top left
	BOTH_T3_T___L,	//# Fast arc top to left
	BOTH_T3_T__BL,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R,	//# Fast weak spin left to right
	BOTH_T3__L_TL,	//# Fast arc left to top left
	BOTH_T3_BL_BR,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T_,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T_,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T_,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T_,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
	BOTH_B3_BR___,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R___,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR___,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T____,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL___,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L___,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR___,	//# Deflection toward BR
	BOTH_D3__R___,	//# Deflection toward R
	BOTH_D3_TR___,	//# Deflection toward TR
	BOTH_D3_TL___,	//# Deflection toward TL
	BOTH_D3__L___,	//# Deflection toward L
	BOTH_D3_BL___,	//# Deflection toward BL
	BOTH_D3_B____,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B_,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR,	//# Fast arc right to top right
	BOTH_T4__R_TL,	//# Fast arc right to top left
	BOTH_T4__R__L,	//# Fast weak spin right to left
	BOTH_T4__R_BL,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL,	//# Fast arc top right to top left
	BOTH_T4_TR__L,	//# Fast arc top right to left
	BOTH_T4_TR_BL,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR,	//# Fast arc top to bottom right
	BOTH_T4_T___R,	//# Fast arc top to right
	BOTH_T4_T__TR,	//# Fast arc top to top right
	BOTH_T4_T__TL,	//# Fast arc top to top left
	BOTH_T4_T___L,	//# Fast arc top to left
	BOTH_T4_T__BL,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R,	//# Fast weak spin left to right
	BOTH_T4__L_TL,	//# Fast arc left to top left
	BOTH_T4_BL_BR,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T_,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T_,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T_,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T_,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
	BOTH_B4_BR___,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R___,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR___,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T____,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL___,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L___,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR___,	//# Deflection toward BR
	BOTH_D4__R___,	//# Deflection toward R
	BOTH_D4_TR___,	//# Deflection toward TR
	BOTH_D4_TL___,	//# Deflection toward TL
	BOTH_D4__L___,	//# Deflection toward L
	BOTH_D4_BL___,	//# Deflection toward BL
	BOTH_D4_B____,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B_,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR,	//# Fast arc right to top right
	BOTH_T5__R_TL,	//# Fast arc right to top left
	BOTH_T5__R__L,	//# Fast weak spin right to left
	BOTH_T5__R_BL,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL,	//# Fast arc top right to top left
	BOTH_T5_TR__L,	//# Fast arc top right to left
	BOTH_T5_TR_BL,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR,	//# Fast arc top to bottom right
	BOTH_T5_T___R,	//# Fast arc top to right
	BOTH_T5_T__TR,	//# Fast arc top to top right
	BOTH_T5_T__TL,	//# Fast arc top to top left
	BOTH_T5_T___L,	//# Fast arc top to left
	BOTH_T5_T__BL,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R,	//# Fast weak spin left to right
	BOTH_T5__L_TL,	//# Fast arc left to top left
	BOTH_T5_BL_BR,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T_,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T_,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T_,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T_,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
	BOTH_B5_BR___,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R___,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR___,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T____,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL___,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L___,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR___,	//# Deflection toward BR
	BOTH_D5__R___,	//# Deflection toward R
	BOTH_D5_TR___,	//# Deflection toward TR
	BOTH_D5_TL___,	//# Deflection toward TL
	BOTH_D5__L___,	//# Deflection toward L
	BOTH_D5_BL___,	//# Deflection toward BL
	BOTH_D5_B____,	//# Deflection toward B
	//Saber attack anims - power level 6
		(animNumber_t)0,//BOTH_A6_T__B_,	//# Fast weak vertical attack top to bottom
		(animNumber_t)0,//BOTH_A6__L__R,	//# Fast weak horizontal attack left to right
		(animNumber_t)0,//BOTH_A6__R__L,	//# Fast weak horizontal attack right to left
		(animNumber_t)0,//BOTH_A6_TL_BR,	//# Fast weak diagonal attack top left to botom right
		(animNumber_t)0,//BOTH_A6_BR_TL,	//# Fast weak diagonal attack top left to botom right
		(animNumber_t)0,//BOTH_A6_BL_TR,	//# Fast weak diagonal attack bottom left to top right
		(animNumber_t)0,//BOTH_A6_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
		(animNumber_t)0,//BOTH_T6_BR__R,	//# Fast arc bottom right to right
		(animNumber_t)0,//BOTH_T6_BR_TL,	//# Fast weak spin bottom right to top left
		(animNumber_t)0,//BOTH_T6_BR__L,	//# Fast weak spin bottom right to left
		(animNumber_t)0,//BOTH_T6_BR_BL,	//# Fast weak spin bottom right to bottom left
		(animNumber_t)0,//BOTH_T6__R_TR,	//# Fast arc right to top right
		(animNumber_t)0,//BOTH_T6__R_TL,	//# Fast arc right to top left
		(animNumber_t)0,//BOTH_T6__R__L,	//# Fast weak spin right to left
		(animNumber_t)0,//BOTH_T6__R_BL,	//# Fast weak spin right to bottom left
		(animNumber_t)0,//BOTH_T6_TR_BR,	//# Fast arc top right to bottom right
		(animNumber_t)0,//BOTH_T6_TR_TL,	//# Fast arc top right to top left
		(animNumber_t)0,//BOTH_T6_TR__L,	//# Fast arc top right to left
		(animNumber_t)0,//BOTH_T6_TR_BL,	//# Fast weak spin top right to bottom left
		(animNumber_t)0,//BOTH_T6_T__BR,	//# Fast arc top to bottom right
		(animNumber_t)0,//BOTH_T6_T___R,	//# Fast arc top to right
		(animNumber_t)0,//BOTH_T6_T__TR,	//# Fast arc top to top right
		(animNumber_t)0,//BOTH_T6_T__TL,	//# Fast arc top to top left
		(animNumber_t)0,//BOTH_T6_T___L,	//# Fast arc top to left
		(animNumber_t)0,//BOTH_T6_T__BL,	//# Fast arc top to bottom left
		(animNumber_t)0,//BOTH_T6_TL_BR,	//# Fast weak spin top left to bottom right
		(animNumber_t)0,//BOTH_T6_TL_BL,	//# Fast arc top left to bottom left
		(animNumber_t)0,//BOTH_T6__L_BR,	//# Fast weak spin left to bottom right
		(animNumber_t)0,//BOTH_T6__L__R,	//# Fast weak spin left to right
		(animNumber_t)0,//BOTH_T6__L_TL,	//# Fast arc left to top left
		(animNumber_t)0,//BOTH_T6_BL_BR,	//# Fast weak spin bottom left to bottom right
		(animNumber_t)0,//BOTH_T6_BL__R,	//# Fast weak spin bottom left to right
		(animNumber_t)0,//BOTH_T6_BL_TR,	//# Fast weak spin bottom left to top right
		(animNumber_t)0,//BOTH_T6_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
		(animNumber_t)0,//BOTH_T6_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T6_TR_BR)
		(animNumber_t)0,//BOTH_T6_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T6_T__BR)
		(animNumber_t)0,//BOTH_T6__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T6_BR__R)
		(animNumber_t)0,//BOTH_T6__R_T_,	//# Fast ar right to top				(use: BOTH_T6_T___R)
		(animNumber_t)0,//BOTH_T6_TR__R,	//# Fast arc top right to right			(use: BOTH_T6__R_TR)
		(animNumber_t)0,//BOTH_T6_TR_T_,	//# Fast arc top right to top				(use: BOTH_T6_T__TR)
		(animNumber_t)0,//BOTH_T6_TL__R,	//# Fast arc top left to right			(use: BOTH_T6__R_TL)
		(animNumber_t)0,//BOTH_T6_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T6_TR_TL)
		(animNumber_t)0,//BOTH_T6_TL_T_,	//# Fast arc top left to top				(use: BOTH_T6_T__TL)
		(animNumber_t)0,//BOTH_T6_TL__L,	//# Fast arc top left to left				(use: BOTH_T6__L_TL)
		(animNumber_t)0,//BOTH_T6__L_TR,	//# Fast arc left to top right			(use: BOTH_T6_TR__L)
		(animNumber_t)0,//BOTH_T6__L_T_,	//# Fast arc left to top				(use: BOTH_T6_T___L)
		(animNumber_t)0,//BOTH_T6__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T6_BL__L)
		(animNumber_t)0,//BOTH_T6_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T6_T__BL)
		(animNumber_t)0,//BOTH_T6_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T6_TL_BL)
	//Saber Attack Start Transitions
		(animNumber_t)0,//BOTH_S6_S6_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
		(animNumber_t)0,//BOTH_S6_S6_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
		(animNumber_t)0,//BOTH_R6_B__S6,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6__L_S6,	//# Fast plain transition from left-to-right Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6__R_S6,	//# Fast plain transition from right-to-left Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6_TL_S6,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6_BR_S6,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6_BL_S6,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
		(animNumber_t)0,//BOTH_R6_TR_S6,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
		(animNumber_t)0,//BOTH_B6_BR___,	//# Bounce-back if attack from BR is blocked
		(animNumber_t)0,//BOTH_B6__R___,	//# Bounce-back if attack from R is blocked
		(animNumber_t)0,//BOTH_B6_TR___,	//# Bounce-back if attack from TR is blocked
		(animNumber_t)0,//BOTH_B6_T____,	//# Bounce-back if attack from T is blocked
		(animNumber_t)0,//BOTH_B6_TL___,	//# Bounce-back if attack from TL is blocked
		(animNumber_t)0,//BOTH_B6__L___,	//# Bounce-back if attack from L is blocked
		(animNumber_t)0,//BOTH_B6_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
		(animNumber_t)0,//BOTH_D6_BR___,	//# Deflection toward BR
		(animNumber_t)0,//BOTH_D6__R___,	//# Deflection toward R
		(animNumber_t)0,//BOTH_D6_TR___,	//# Deflection toward TR
		(animNumber_t)0,//BOTH_D6_TL___,	//# Deflection toward TL
		(animNumber_t)0,//BOTH_D6__L___,	//# Deflection toward L
		(animNumber_t)0,//BOTH_D6_BL___,	//# Deflection toward BL
		(animNumber_t)0,//BOTH_D6_B____,	//# Deflection toward B
	//Saber attack anims - power level 7
		(animNumber_t)0,//BOTH_A7_T__B_,	//# Fast weak vertical attack top to bottom
		(animNumber_t)0,//BOTH_A7__L__R,	//# Fast weak horizontal attack left to right
		(animNumber_t)0,//BOTH_A7__R__L,	//# Fast weak horizontal attack right to left
		(animNumber_t)0,//BOTH_A7_TL_BR,	//# Fast weak diagonal attack top left to botom right
		(animNumber_t)0,//BOTH_A7_BR_TL,	//# Fast weak diagonal attack top left to botom right
		(animNumber_t)0,//BOTH_A7_BL_TR,	//# Fast weak diagonal attack bottom left to top right
		(animNumber_t)0,//BOTH_A7_TR_BL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
		(animNumber_t)0,//BOTH_T7_BR__R,	//# Fast arc bottom right to right
		(animNumber_t)0,//BOTH_T7_BR_TL,	//# Fast weak spin bottom right to top left
		(animNumber_t)0,//BOTH_T7_BR__L,	//# Fast weak spin bottom right to left
		(animNumber_t)0,//BOTH_T7_BR_BL,	//# Fast weak spin bottom right to bottom left
		(animNumber_t)0,//BOTH_T7__R_TR,	//# Fast arc right to top right
		(animNumber_t)0,//BOTH_T7__R_TL,	//# Fast arc right to top left
		(animNumber_t)0,//BOTH_T7__R__L,	//# Fast weak spin right to left
		(animNumber_t)0,//BOTH_T7__R_BL,	//# Fast weak spin right to bottom left
		(animNumber_t)0,//BOTH_T7_TR_BR,	//# Fast arc top right to bottom right
		(animNumber_t)0,//BOTH_T7_TR_TL,	//# Fast arc top right to top left
		(animNumber_t)0,//BOTH_T7_TR__L,	//# Fast arc top right to left
		(animNumber_t)0,//BOTH_T7_TR_BL,	//# Fast weak spin top right to bottom left
		(animNumber_t)0,//BOTH_T7_T__BR,	//# Fast arc top to bottom right
			(animNumber_t)0,//BOTH_T7_T___R,	//# Fast arc top to right
			(animNumber_t)0,//BOTH_T7_T__TR,	//# Fast arc top to top right
			(animNumber_t)0,//BOTH_T7_T__TL,	//# Fast arc top to top left
			(animNumber_t)0,//BOTH_T7_T___L,	//# Fast arc top to left
			(animNumber_t)0,//BOTH_T7_T__BL,	//# Fast arc top to bottom left
			(animNumber_t)0,//BOTH_T7_TL_BR,	//# Fast weak spin top left to bottom right
			(animNumber_t)0,//BOTH_T7_TL_BL,	//# Fast arc top left to bottom left
			(animNumber_t)0,//BOTH_T7__L_BR,	//# Fast weak spin left to bottom right
			(animNumber_t)0,//BOTH_T7__L__R,	//# Fast weak spin left to right
			(animNumber_t)0,//BOTH_T7__L_TL,	//# Fast arc left to top left
			(animNumber_t)0,//BOTH_T7_BL_BR,	//# Fast weak spin bottom left to bottom right
			(animNumber_t)0,//BOTH_T7_BL__R,	//# Fast weak spin bottom left to right
			(animNumber_t)0,//BOTH_T7_BL_TR,	//# Fast weak spin bottom left to top right
			(animNumber_t)0,//BOTH_T7_BL__L,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
			(animNumber_t)0,//BOTH_T7_BR_TR,	//# Fast arc bottom right to top right		(use: BOTH_T7_TR_BR)
			(animNumber_t)0,//BOTH_T7_BR_T_,	//# Fast arc bottom right to top			(use: BOTH_T7_T__BR)
			(animNumber_t)0,//BOTH_T7__R_BR,	//# Fast arc right to bottom right			(use: BOTH_T7_BR__R)
			(animNumber_t)0,//BOTH_T7__R_T_,	//# Fast ar right to top				(use: BOTH_T7_T___R)
			(animNumber_t)0,//BOTH_T7_TR__R,	//# Fast arc top right to right			(use: BOTH_T7__R_TR)
			(animNumber_t)0,//BOTH_T7_TR_T_,	//# Fast arc top right to top				(use: BOTH_T7_T__TR)
			(animNumber_t)0,//BOTH_T7_TL__R,	//# Fast arc top left to right			(use: BOTH_T7__R_TL)
			(animNumber_t)0,//BOTH_T7_TL_TR,	//# Fast arc top left to top right			(use: BOTH_T7_TR_TL)
			(animNumber_t)0,//BOTH_T7_TL_T_,	//# Fast arc top left to top				(use: BOTH_T7_T__TL)
			(animNumber_t)0,//BOTH_T7_TL__L,	//# Fast arc top left to left				(use: BOTH_T7__L_TL)
			(animNumber_t)0,//BOTH_T7__L_TR,	//# Fast arc left to top right			(use: BOTH_T7_TR__L)
			(animNumber_t)0,//BOTH_T7__L_T_,	//# Fast arc left to top				(use: BOTH_T7_T___L)
			(animNumber_t)0,//BOTH_T7__L_BL,	//# Fast arc left to bottom left			(use: BOTH_T7_BL__L)
			(animNumber_t)0,//BOTH_T7_BL_T_,	//# Fast arc bottom left to top			(use: BOTH_T7_T__BL)
			(animNumber_t)0,//BOTH_T7_BL_TL,	//# Fast arc bottom left to top left		(use: BOTH_T7_TL_BL)
	//Saber Attack Start Transitions
			(animNumber_t)0,//BOTH_S7_S7_T_,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7__L,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7__R,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7_TL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7_BR,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7_BL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
			(animNumber_t)0,//BOTH_S7_S7_TR,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
			(animNumber_t)0,//BOTH_R7_B__S7,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7__L_S7,	//# Fast plain transition from left-to-right Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7__R_S7,	//# Fast plain transition from right-to-left Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7_TL_S7,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7_BR_S7,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7_BL_S7,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
			(animNumber_t)0,//BOTH_R7_TR_S7,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack, played backwards)
			(animNumber_t)0,//BOTH_B7_BR___,	//# Bounce-back if attack from BR is blocked
			(animNumber_t)0,//BOTH_B7__R___,	//# Bounce-back if attack from R is blocked
			(animNumber_t)0,//BOTH_B7_TR___,	//# Bounce-back if attack from TR is blocked
			(animNumber_t)0,//BOTH_B7_T____,	//# Bounce-back if attack from T is blocked
			(animNumber_t)0,//BOTH_B7_TL___,	//# Bounce-back if attack from TL is blocked
			(animNumber_t)0,//BOTH_B7__L___,	//# Bounce-back if attack from L is blocked
			(animNumber_t)0,//BOTH_B7_BL___,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
			(animNumber_t)0,//BOTH_D7_BR___,	//# Deflection toward BR
			(animNumber_t)0,//BOTH_D7__R___,	//# Deflection toward R
			(animNumber_t)0,//BOTH_D7_TR___,	//# Deflection toward TR
			(animNumber_t)0,//BOTH_D7_TL___,	//# Deflection toward TL
			(animNumber_t)0,//BOTH_D7__L___,	//# Deflection toward L
			(animNumber_t)0,//BOTH_D7_BL___,	//# Deflection toward BL
			(animNumber_t)0,//BOTH_D7_B____,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T_,	//# Block shot/saber top
	BOTH_P1_S1_TR,	//# Block shot/saber top right
	BOTH_P1_S1_TL,	//# Block shot/saber top left
	BOTH_P1_S1_BL,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T_,	//# knockaway saber top
	BOTH_K1_S1_TR,	//# knockaway saber top right
	BOTH_K1_S1_TL,	//# knockaway saber top left
	BOTH_K1_S1_BL,	//# knockaway saber bottom left
	BOTH_K1_S1_B_,	//# knockaway saber bottom
	BOTH_K1_S1_BR,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1,	//# BR attack knocked away
	BOTH_V1__R_S1,	//# R attack knocked away
	BOTH_V1_TR_S1,	//# TR attack knocked away
	BOTH_V1_T__S1,	//# T attack knocked away
	BOTH_V1_TL_S1,	//# TL attack knocked away
	BOTH_V1__L_S1,	//# L attack knocked away
	BOTH_V1_BL_S1,	//# BL attack knocked away
	BOTH_V1_B__S1,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T_,	//# saber knocked down from top parry
	BOTH_H1_S1_TR,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B_,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR,	//# saber knocked up-left from BR parry
	//Dual Saber parry anims
			(animNumber_t)0,//BOTH_P6_S6_T_,	//# Block shot/saber top
			(animNumber_t)0,//BOTH_P6_S6_TR,	//# Block shot/saber top right
			(animNumber_t)0,//BOTH_P6_S6_TL,	//# Block shot/saber top left
			(animNumber_t)0,//BOTH_P6_S6_BL,	//# Block shot/saber bottom left
			(animNumber_t)0,//BOTH_P6_S6_BR,	//# Block shot/saber bottom right
	//Dual Saber knockaway
			(animNumber_t)0,//BOTH_K6_S6_T_,	//# knockaway saber top
			(animNumber_t)0,//BOTH_K6_S6_TR,	//# knockaway saber top right
				(animNumber_t)0,//BOTH_K6_S6_TL,	//# knockaway saber top left
				(animNumber_t)0,//BOTH_K6_S6_BL,	//# knockaway saber bottom left
				(animNumber_t)0,//BOTH_K6_S6_B_,	//# knockaway saber bottom
				(animNumber_t)0,//BOTH_K6_S6_BR,	//# knockaway saber bottom right
	//Dual Saber attack knocked away
				(animNumber_t)0,//BOTH_V6_BR_S6,	//# BR attack knocked away
				(animNumber_t)0,//BOTH_V6__R_S6,	//# R attack knocked away
				(animNumber_t)0,//BOTH_V6_TR_S6,	//# TR attack knocked away
				(animNumber_t)0,//BOTH_V6_T__S6,	//# T attack knocked away
				(animNumber_t)0,//BOTH_V6_TL_S6,	//# TL attack knocked away
				(animNumber_t)0,//BOTH_V6__L_S6,	//# L attack knocked away
				(animNumber_t)0,//BOTH_V6_BL_S6,	//# BL attack knocked away
				(animNumber_t)0,//BOTH_V6_B__S6,	//# B attack knocked away
	//Dual Saber parry broken
				(animNumber_t)0,//BOTH_H6_S6_T_,	//# saber knocked down from top parry
				(animNumber_t)0,//BOTH_H6_S6_TR,	//# saber knocked down-left from TR parry
				(animNumber_t)0,//BOTH_H6_S6_TL,	//# saber knocked down-right from TL parry
				(animNumber_t)0,//BOTH_H6_S6_BL,	//# saber knocked up-right from BL parry
				(animNumber_t)0,//BOTH_H6_S6_B_,	//# saber knocked up over head from ready?
				(animNumber_t)0,//BOTH_H6_S6_BR,	//# saber knocked up-left from BR parry
	//SaberStaff parry anims
				(animNumber_t)0,//BOTH_P7_S7_T_,	//# Block shot/saber top
				(animNumber_t)0,//BOTH_P7_S7_TR,	//# Block shot/saber top right
				(animNumber_t)0,//BOTH_P7_S7_TL,	//# Block shot/saber top left
				(animNumber_t)0,//BOTH_P7_S7_BL,	//# Block shot/saber bottom left
				(animNumber_t)0,//BOTH_P7_S7_BR,	//# Block shot/saber bottom right
	//SaberStaff knockaway
				(animNumber_t)0,//BOTH_K7_S7_T_,	//# knockaway saber top
				(animNumber_t)0,//BOTH_K7_S7_TR,	//# knockaway saber top right
				(animNumber_t)0,//BOTH_K7_S7_TL,	//# knockaway saber top left
				(animNumber_t)0,//BOTH_K7_S7_BL,	//# knockaway saber bottom left
				(animNumber_t)0,//BOTH_K7_S7_B_,	//# knockaway saber bottom
				(animNumber_t)0,//BOTH_K7_S7_BR,	//# knockaway saber bottom right
	//SaberStaff attack knocked away
				(animNumber_t)0,//BOTH_V7_BR_S7,	//# BR attack knocked away
				(animNumber_t)0,//BOTH_V7__R_S7,	//# R attack knocked away
				(animNumber_t)0,//BOTH_V7_TR_S7,	//# TR attack knocked away
				(animNumber_t)0,//BOTH_V7_T__S7,	//# T attack knocked away
				(animNumber_t)0,//BOTH_V7_TL_S7,	//# TL attack knocked away
				(animNumber_t)0,//BOTH_V7__L_S7,	//# L attack knocked away
				(animNumber_t)0,//BOTH_V7_BL_S7,	//# BL attack knocked away
				(animNumber_t)0,//BOTH_V7_B__S7,	//# B attack knocked away
	//SaberStaff parry broken
				(animNumber_t)0,//BOTH_H7_S7_T_,	//# saber knocked down from top parry
				(animNumber_t)0,//BOTH_H7_S7_TR,	//# saber knocked down-left from TR parry
				(animNumber_t)0,//BOTH_H7_S7_TL,	//# saber knocked down-right from TL parry
				(animNumber_t)0,//BOTH_H7_S7_BL,	//# saber knocked up-right from BL parry
				(animNumber_t)0,//BOTH_H7_S7_B_,	//# saber knocked up over head from ready?
				(animNumber_t)0,//BOTH_H7_S7_BR,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	//* #sep BOTH_ SABER LOCKED ANIMS
	//BOTH_(DL, S, ST)_(DL, S, ST)_(T, S)_(L, B, SB)_1(_W, _L)
//===Single locks==================================================================
//SINGLE vs. DUAL
	//side locks - I'm using a single and they're using dual
				(animNumber_t)0,//BOTH_LK_S_DL_S_B_1_L,	//normal break I lost
				(animNumber_t)0,//BOTH_LK_S_DL_S_B_1_W,	//normal break I won
				(animNumber_t)0,//BOTH_LK_S_DL_S_L_1,		//lock if I'm using single vs. a dual
				(animNumber_t)0,//BOTH_LK_S_DL_S_SB_1_L,	//super break I lost
				(animNumber_t)0,//BOTH_LK_S_DL_S_SB_1_W,	//super break I won
	//top locks
				(animNumber_t)0,//BOTH_LK_S_DL_T_B_1_L,	//normal break I lost
		(animNumber_t)0,//BOTH_LK_S_DL_T_B_1_W,	//normal break I won
		(animNumber_t)0,//BOTH_LK_S_DL_T_L_1,		//lock if I'm using single vs. a dual
		(animNumber_t)0,//BOTH_LK_S_DL_T_SB_1_L,	//super break I lost
		(animNumber_t)0,//BOTH_LK_S_DL_T_SB_1_W,	//super break I won
//SINGLE vs. STAFF
	//side locks
		(animNumber_t)0,//BOTH_LK_S_ST_S_B_1_L,	//normal break I lost
		(animNumber_t)0,//BOTH_LK_S_ST_S_B_1_W,	//normal break I won
		(animNumber_t)0,//BOTH_LK_S_ST_S_L_1,		//lock if I'm using single vs. a staff
		(animNumber_t)0,//BOTH_LK_S_ST_S_SB_1_L,	//super break I lost
		(animNumber_t)0,//BOTH_LK_S_ST_S_SB_1_W,	//super break I won
	//top locks
		(animNumber_t)0,//BOTH_LK_S_ST_T_B_1_L,	//normal break I lost
		(animNumber_t)0,//BOTH_LK_S_ST_T_B_1_W,	//normal break I won
		(animNumber_t)0,//BOTH_LK_S_ST_T_L_1,		//lock if I'm using single vs. a staff
		(animNumber_t)0,//BOTH_LK_S_ST_T_SB_1_L,	//super break I lost
		(animNumber_t)0,//BOTH_LK_S_ST_T_SB_1_W,	//super break I won
//SINGLE vs. SINGLE
	//side locks
		(animNumber_t)0,//BOTH_LK_S_S_S_B_1_L,	//normal break I lost
		(animNumber_t)0,//BOTH_LK_S_S_S_B_1_W,	//normal break I won
		(animNumber_t)0,//BOTH_LK_S_S_S_L_1,		//lock if I'm using single vs. a single and I initiated
		(animNumber_t)0,//BOTH_LK_S_S_S_SB_1_L,	//super break I lost
		(animNumber_t)0,//BOTH_LK_S_S_S_SB_1_W,	//super break I won
	//top locks
		(animNumber_t)0,//BOTH_LK_S_S_T_B_1_L,	//normal break I lost
		(animNumber_t)0,//BOTH_LK_S_S_T_B_1_W,	//normal break I won
		(animNumber_t)0,//BOTH_LK_S_S_T_L_1,		//lock if I'm using single vs. a single and I initiated
		(animNumber_t)0,//BOTH_LK_S_S_T_SB_1_L,	//super break I lost
				(animNumber_t)0,//BOTH_LK_S_S_T_SB_1_W,	//super break I won
//===Dual Saber locks==================================================================
//DUAL vs. DUAL	
	//side locks
				(animNumber_t)0,//BOTH_LK_DL_DL_S_B_1_L,	//normal break I lost
				(animNumber_t)0,//BOTH_LK_DL_DL_S_B_1_W,	//normal break I won
				(animNumber_t)0,//BOTH_LK_DL_DL_S_L_1,	//lock if I'm using dual vs. dual and I initiated
				(animNumber_t)0,//BOTH_LK_DL_DL_S_SB_1_L,	//super break I lost
				(animNumber_t)0,//BOTH_LK_DL_DL_S_SB_1_W,	//super break I won
	//top locks
					(animNumber_t)0,//BOTH_LK_DL_DL_T_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_DL_DL_T_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_DL_DL_T_L_1,	//lock if I'm using dual vs. dual and I initiated
					(animNumber_t)0,//BOTH_LK_DL_DL_T_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_DL_DL_T_SB_1_W,	//super break I won
//DUAL vs. STAFF
	//side locks
					(animNumber_t)0,//BOTH_LK_DL_ST_S_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_DL_ST_S_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_DL_ST_S_L_1,	//lock if I'm using dual vs. a staff
					(animNumber_t)0,//BOTH_LK_DL_ST_S_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_DL_ST_S_SB_1_W,	//super break I won
	//top locks
					(animNumber_t)0,//BOTH_LK_DL_ST_T_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_DL_ST_T_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_DL_ST_T_L_1,	//lock if I'm using dual vs. a staff
					(animNumber_t)0,//BOTH_LK_DL_ST_T_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_DL_ST_T_SB_1_W,	//super break I won
//DUAL vs. SINGLE
	//side locks
					(animNumber_t)0,//BOTH_LK_DL_S_S_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_DL_S_S_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_DL_S_S_L_1,		//lock if I'm using dual vs. a single
					(animNumber_t)0,//BOTH_LK_DL_S_S_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_DL_S_S_SB_1_W,	//super break I won
	//top locks
					(animNumber_t)0,//BOTH_LK_DL_S_T_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_DL_S_T_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_DL_S_T_L_1,		//lock if I'm using dual vs. a single
					(animNumber_t)0,//BOTH_LK_DL_S_T_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_DL_S_T_SB_1_W,	//super break I won
//===Saber Staff locks==================================================================
//STAFF vs. DUAL
	//side locks
					(animNumber_t)0,//BOTH_LK_ST_DL_S_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_ST_DL_S_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_ST_DL_S_L_1,	//lock if I'm using staff vs. dual
					(animNumber_t)0,//BOTH_LK_ST_DL_S_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_ST_DL_S_SB_1_W,	//super break I won
	//top locks
					(animNumber_t)0,//BOTH_LK_ST_DL_T_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_ST_DL_T_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_ST_DL_T_L_1,	//lock if I'm using staff vs. dual
					(animNumber_t)0,//BOTH_LK_ST_DL_T_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_ST_DL_T_SB_1_W,	//super break I won
//STAFF vs. STAFF
	//side locks
					(animNumber_t)0,//BOTH_LK_ST_ST_S_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_ST_ST_S_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_ST_ST_S_L_1,	//lock if I'm using staff vs. a staff and I initiated
					(animNumber_t)0,//BOTH_LK_ST_ST_S_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_ST_ST_S_SB_1_W,	//super break I won
	//top locks
					(animNumber_t)0,//BOTH_LK_ST_ST_T_B_1_L,	//normal break I lost
					(animNumber_t)0,//BOTH_LK_ST_ST_T_B_1_W,	//normal break I won
					(animNumber_t)0,//BOTH_LK_ST_ST_T_L_1,	//lock if I'm using staff vs. a staff and I initiated
					(animNumber_t)0,//BOTH_LK_ST_ST_T_SB_1_L,	//super break I lost
					(animNumber_t)0,//BOTH_LK_ST_ST_T_SB_1_W,	//super break I won
//STAFF vs. SINGLE
	//side locks
					(animNumber_t)0,//BOTH_LK_ST_S_S_B_1_L,	//normal break I lost
			(animNumber_t)0,//BOTH_LK_ST_S_S_B_1_W,	//normal break I won
			(animNumber_t)0,//BOTH_LK_ST_S_S_L_1,		//lock if I'm using staff vs. a single
			(animNumber_t)0,//BOTH_LK_ST_S_S_SB_1_L,	//super break I lost
			(animNumber_t)0,//BOTH_LK_ST_S_S_SB_1_W,	//super break I won
	//top locks
			(animNumber_t)0,//BOTH_LK_ST_S_T_B_1_L,	//normal break I lost
			(animNumber_t)0,//BOTH_LK_ST_S_T_B_1_W,	//normal break I won
			(animNumber_t)0,//BOTH_LK_ST_S_T_L_1,		//lock if I'm using staff vs. a single
			(animNumber_t)0,//BOTH_LK_ST_S_T_SB_1_L,	//super break I lost
			(animNumber_t)0,//BOTH_LK_ST_S_T_SB_1_W,	//super break I won
//Special cases for same saber style vs. each other (won't fit in nice 5-anim size lists above)
(animNumber_t)0,//BOTH_LK_S_S_S_L_2,		//lock if I'm using single vs. a single and other intitiated
(animNumber_t)0,//BOTH_LK_S_S_T_L_2,		//lock if I'm using single vs. a single and other initiated
(animNumber_t)0,//BOTH_LK_DL_DL_S_L_2,	//lock if I'm using dual vs. dual and other initiated
(animNumber_t)0,//BOTH_LK_DL_DL_T_L_2,	//lock if I'm using dual vs. dual and other initiated
(animNumber_t)0,//BOTH_LK_ST_ST_S_L_2,	//lock if I'm using staff vs. a staff and other initiated
(animNumber_t)0,//BOTH_LK_ST_ST_T_L_2,	//lock if I'm using staff vs. a staff and other initiated
//===End Saber locks==================================================================
	//old locks
	BOTH_BF2RETURN,	//#
	BOTH_BF2BREAK,	//#
	BOTH_BF2LOCK,	//#
	BOTH_BF1RETURN,	//#
	BOTH_BF1BREAK,	//#
	BOTH_BF1LOCK,	//#
	BOTH_CWCIRCLE_R2__R_S1,	//#
	BOTH_CCWCIRCLE_R2__L_S1,	//#
	BOTH_CWCIRCLE_A2__L__R,	//#
	BOTH_CCWCIRCLE_A2__R__L,	//#
	BOTH_CWCIRCLEBREAK,	//#
	BOTH_CCWCIRCLEBREAK,	//#
	BOTH_CWCIRCLELOCK,	//#
	BOTH_CCWCIRCLELOCK,	//#
	//other saber anims
	//* #sep BOTH_ SABER MISC ANIMS
	BOTH_SABERFAST_STANCE,
	BOTH_SABERSLOW_STANCE,
					(animNumber_t)0,//BOTH_SABERDUAL_STANCE,
						(animNumber_t)0,//BOTH_SABERSTAFF_STANCE,
	BOTH_A2_STABBACK1,		//# Stab saber backward
	BOTH_ATTACK_BACK,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1,//#
	BOTH_JUMPFLIPSTABDOWN,//#
	BOTH_FORCELEAP2_T__B_,//#
	BOTH_LUNGE2_B__T_,//#
	BOTH_CROUCHATTACKBACK1,//#
	//New specials for JKA:
						(animNumber_t)0,//BOTH_JUMPATTACK6,//#
						(animNumber_t)0,//BOTH_JUMPATTACK7,//#
						(animNumber_t)0,//BOTH_SPINATTACK6,//#
						(animNumber_t)0,//BOTH_SPINATTACK7,//#
						(animNumber_t)0,//BOTH_S1_S6,//#	From stand1 to saberdual stance - turning on your dual sabers
						(animNumber_t)0,//BOTH_S6_S1,//#	From dualstaff stance to stand1 - turning off your dual sabers
						(animNumber_t)0,//BOTH_S1_S7,//#	From stand1 to saberstaff stance - turning on your saberstaff
						(animNumber_t)0,//BOTH_S7_S1,//#	From saberstaff stance to stand1 - turning off your saberstaff
						(animNumber_t)0,//BOTH_FORCELONGLEAP_START,
						(animNumber_t)0,//BOTH_FORCELONGLEAP_ATTACK,
						(animNumber_t)0,//BOTH_FORCELONGLEAP_LAND,
						(animNumber_t)0,//BOTH_FORCEWALLRUNFLIP_START,
						(animNumber_t)0,//BOTH_FORCEWALLRUNFLIP_END,
						(animNumber_t)0,//BOTH_FORCEWALLRUNFLIP_ALT,
						(animNumber_t)0,//BOTH_FORCEWALLREBOUND_FORWARD,
						(animNumber_t)0,//BOTH_FORCEWALLREBOUND_LEFT,
						(animNumber_t)0,//BOTH_FORCEWALLREBOUND_BACK,
						(animNumber_t)0,//BOTH_FORCEWALLREBOUND_RIGHT,
						(animNumber_t)0,//BOTH_FORCEWALLHOLD_FORWARD,
						(animNumber_t)0,//BOTH_FORCEWALLHOLD_LEFT,
						(animNumber_t)0,//BOTH_FORCEWALLHOLD_BACK,
						(animNumber_t)0,//BOTH_FORCEWALLHOLD_RIGHT,
						(animNumber_t)0,//BOTH_FORCEWALLRELEASE_FORWARD,
						(animNumber_t)0,//BOTH_FORCEWALLRELEASE_LEFT,
						(animNumber_t)0,//BOTH_FORCEWALLRELEASE_BACK,
						(animNumber_t)0,//BOTH_FORCEWALLRELEASE_RIGHT,
						(animNumber_t)0,//BOTH_A7_KICK_F,
						(animNumber_t)0,//BOTH_A7_KICK_B,
						(animNumber_t)0,//BOTH_A7_KICK_R,
						(animNumber_t)0,//BOTH_A7_KICK_L,
						(animNumber_t)0,//BOTH_A7_KICK_S,
						(animNumber_t)0,//BOTH_A7_KICK_BF,
						(animNumber_t)0,//BOTH_A7_KICK_BF_STOP,
						(animNumber_t)0,//BOTH_A7_KICK_RL,
						(animNumber_t)0,//BOTH_A7_KICK_F_AIR,
						(animNumber_t)0,//BOTH_A7_KICK_B_AIR,
						(animNumber_t)0,//BOTH_A7_KICK_R_AIR,
						(animNumber_t)0,//BOTH_A7_KICK_L_AIR,
						(animNumber_t)0,//BOTH_FLIP_ATTACK7,
						(animNumber_t)0,//BOTH_FLIP_HOLD7,
						(animNumber_t)0,//BOTH_FLIP_LAND,
						(animNumber_t)0,//BOTH_PULL_IMPALE_STAB,
						(animNumber_t)0,//BOTH_PULL_IMPALE_SWING,
						(animNumber_t)0,//BOTH_PULLED_INAIR_B,
						(animNumber_t)0,//BOTH_PULLED_INAIR_F,
						(animNumber_t)0,//BOTH_STABDOWN,
						(animNumber_t)0,//BOTH_STABDOWN_STAFF,
						(animNumber_t)0,//BOTH_STABDOWN_DUAL,
						(animNumber_t)0,//BOTH_A6_SABERPROTECT,
						(animNumber_t)0,//BOTH_A7_SOULCAL,
						(animNumber_t)0,//BOTH_A1_SPECIAL,
						(animNumber_t)0,//BOTH_A2_SPECIAL,
						(animNumber_t)0,//BOTH_A3_SPECIAL,
				(animNumber_t)0,//BOTH_ROLL_STAB,

	//# #sep BOTH_ STANDING
	BOTH_STAND1,			//# Standing idle, no weapon, hands down
	BOTH_STAND1IDLE1,		//# Random standing idle
	BOTH_STAND2,			//# Standing idle with a saber
	BOTH_STAND2IDLE1,		//# Random standing idle
	BOTH_STAND2IDLE2,		//# Random standing idle
	BOTH_STAND3,			//# Standing idle with 2-handed weapon
	BOTH_STAND3IDLE1,		//# Random standing idle
	BOTH_STAND4,			//# hands clasp behind back
	BOTH_STAND5,			//# standing idle, no weapon, hand down, back straight
	BOTH_STAND5IDLE1,		//# Random standing idle
	BOTH_STAND6,			//# one handed, gun at side, relaxed stand
	BOTH_STAND8,			//# both hands on hips (male)
	BOTH_STAND1TO2,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2,			//# Transition from stand4 to stand2
	BOTH_STAND4TOATTACK2,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP2,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3,		//# transition from stand 5 to sit 3
	BOTH_STAND1TOSTAND5,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1,	//# Transition from stand5 to stand1
	BOTH_STAND5TOAIM,		//# Transition of Kye aiming his gun at Desann (cin #9) 
	BOTH_STAND5STARTLEDLOOKLEFT,	//# Kyle turning to watch the bridge drop (cin #9) 
	BOTH_STARTLEDLOOKLEFTTOSTAND5,	//# Kyle returning to stand 5 from watching the bridge drop (cin #9) 
	BOTH_STAND5TOSTAND8,	//# Transition from stand5 to stand8
	BOTH_STAND7TOSTAND8,	//# Tavion putting hands on back of chair (cin #11)
	BOTH_STAND8TOSTAND5,	//# Transition from stand8 to stand5
		(animNumber_t)0,//BOTH_STAND9,			//# Kyle's standing idle, no weapon, hands down
		(animNumber_t)0,//BOTH_STAND9IDLE1,		//# Kyle's random standing idle
	BOTH_STAND5SHIFTWEIGHT,	//# Weightshift from stand5 to side and back to stand5
	BOTH_STAND5SHIFTWEIGHTSTART,	//# From stand5 to side
	BOTH_STAND5SHIFTWEIGHTSTOP,		//# From side to stand5
	BOTH_STAND5TURNLEFTSTART,		//# Start turning left from stand5
	BOTH_STAND5TURNLEFTSTOP,		//# Stop turning left from stand5
	BOTH_STAND5TURNRIGHTSTART,		//# Start turning right from stand5
	BOTH_STAND5TURNRIGHTSTOP,		//# Stop turning right from stand5
	BOTH_STAND5LOOK180LEFTSTART,	//# Start looking over left shoulder (cin #17)
	BOTH_STAND5LOOK180LEFTSTOP,	//# Stop looking over left shoulder (cin #17)

	BOTH_CONSOLE1START,		//# typing at a console
	BOTH_CONSOLE1,			//# typing at a console
	BOTH_CONSOLE1STOP,		//# typing at a console
	BOTH_CONSOLE2START,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTART,	//# lean in to type at console while holding comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTOP,	//# lean away after typing at console while holding comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1,		//# Cradling weapon and standing
	BOTH_GESTURE1,			//# Generic gesture, non-specific
	BOTH_GESTURE2,			//# Generic gesture, non-specific
	BOTH_WALK1TALKCOMM1,	//# Talking into coom link while walking
	BOTH_TALK1,				//# Generic talk anim
	BOTH_TALK2,				//# Generic talk anim
	BOTH_TALKCOMM1START,	//# Start talking into a comm link
	BOTH_TALKCOMM1,			//# Talking into a comm link
	BOTH_TALKCOMM1STOP,		//# Stop talking into a comm link
	BOTH_TALKGESTURE1,		//# Generic talk anim

	BOTH_HEADTILTLSTART,		//# Head tilt to left
	BOTH_HEADTILTLSTOP,			//# Head tilt to left
	BOTH_HEADTILTRSTART,		//# Head tilt to right
	BOTH_HEADTILTRSTOP,			//# Head tilt to right
	BOTH_HEADNOD,				//# Head shake YES
	BOTH_HEADSHAKE,				//# Head shake NO
	BOTH_SIT2HEADTILTLSTART,	//# Head tilt to left from seated position 2
	BOTH_SIT2HEADTILTLSTOP,		//# Head tilt to left from seated position 2

	BOTH_REACH1START,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP,		//# Monmothma reaching for crystal

	BOTH_COME_ON1,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_SABERKILLER1,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HUGGER1,			//# Kyle hugging Jan (cin #29)
	BOTH_HUGGERSTOP1,		//# Kyle stop hugging Jan but don't let her go (cin #29)
	BOTH_HUGGEE1,			//# Jan being hugged (cin #29)
	BOTH_HUGGEESTOP1,		//# Jan stop being hugged but don't let go (cin #29)

	BOTH_SABERTHROW1START,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP,		//# Kyle throwing his light saber (cin #32)

	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1,				//# Normal chair sit.
	BOTH_SIT2,				//# Lotus position.
	BOTH_SIT3,				//# Sitting in tired position, elbows on knees

	BOTH_SIT2TOSTAND5,		//# Transition from sit 2 to stand 5
	BOTH_STAND5TOSIT2,		//# Transition from stand 5 to sit 2
	BOTH_SIT2TOSIT4,		//# Trans from sit2 to sit4 (cin #12) Luke leaning back from lotus position.
	BOTH_SIT3TOSTAND5,		//# transition from sit 3 to stand 5

	BOTH_CROUCH1,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE,		//# Crouching idle
	BOTH_CROUCH1WALK,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK,	//# Walking while crouched
	BOTH_UNCROUCH1,			//# Transition from crouch to standing
	BOTH_CROUCH2TOSTAND1,	//# going from crouch2 to stand1
	BOTH_CROUCH3,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_CROUCH4,			//# Slower version of crouch1 for cinematics
	BOTH_UNCROUCH4,			//# Slower version of uncrouch1 for cinematics

	BOTH_GUNSIT1,			//# sitting on an emplaced gun.

	// Swoop Vehicle animations.
	//* #sep BOTH_ SWOOP ANIMS
		(animNumber_t)0,//BOTH_VS_MOUNT_L,			//# Mount from left		
		(animNumber_t)0,//BOTH_VS_DISMOUNT_L,			//# Dismount to left		
		(animNumber_t)0,//BOTH_VS_MOUNT_R,			//# Mount from  right (symmetry)		
		(animNumber_t)0,//BOTH_VS_DISMOUNT_R,			//# DISMOUNT TO  RIGHT (SYMMETRY)		

		(animNumber_t)0,//BOTH_VS_MOUNTJUMP_L,		//#
		(animNumber_t)0,//BOTH_VS_MOUNTTHROW,			//# Land on an occupied vehicle & throw off current pilot
		(animNumber_t)0,//BOTH_VS_MOUNTTHROW_L,		//# Land on an occupied vehicle & throw off current pilot
		(animNumber_t)0,//BOTH_VS_MOUNTTHROW_R,		//# Land on an occupied vehicle & throw off current pilot
		(animNumber_t)0,//BOTH_VS_MOUNTTHROWEE,		//# Current pilot getting thrown off by another guy

		(animNumber_t)0,//BOTH_VS_LOOKLEFT,			//# Turn & Look behind and to the left (no weapon)		
		(animNumber_t)0,//BOTH_VS_LOOKRIGHT,			//# Turn & Look behind and to the right (no weapon)		

			(animNumber_t)0,//BOTH_VS_TURBO,				//# Hit The Turbo Button

		(animNumber_t)0,//BOTH_VS_REV,				//# Player looks back as swoop reverses		

		(animNumber_t)0,//BOTH_VS_AIR,				//# Player stands up when swoop is airborn		
		(animNumber_t)0,//BOTH_VS_AIR_G,				//# "" with Gun
		(animNumber_t)0,//BOTH_VS_AIR_SL,				//# "" with Saber Left
		(animNumber_t)0,//BOTH_VS_AIR_SR,				//# "" with Saber Right

		(animNumber_t)0,//BOTH_VS_LAND,				//# Player bounces down when swoop lands		
		(animNumber_t)0,//BOTH_VS_LAND_G,				//#  "" with Gun
		(animNumber_t)0,//BOTH_VS_LAND_SL,			//#  "" with Saber Left
		(animNumber_t)0,//BOTH_VS_LAND_SR,			//#  "" with Saber Right

		(animNumber_t)0,//BOTH_VS_IDLE,				//# Sit
		(animNumber_t)0,//BOTH_VS_IDLE_G,				//# Sit (gun)
		(animNumber_t)0,//BOTH_VS_IDLE_SL,			//# Sit (saber left)		
		(animNumber_t)0,//BOTH_VS_IDLE_SR,			//# Sit (saber right)		

		(animNumber_t)0,//BOTH_VS_LEANL,				//# Lean left
		(animNumber_t)0,//BOTH_VS_LEANL_G,			//# Lean left (gun)		
		(animNumber_t)0,//BOTH_VS_LEANL_SL,			//# Lean left (saber left)		
		(animNumber_t)0,//BOTH_VS_LEANL_SR,			//# Lean left (saber right)		

		(animNumber_t)0,//BOTH_VS_LEANR,				//# Lean right		
		(animNumber_t)0,//BOTH_VS_LEANR_G,			//# Lean right (gun)		
		(animNumber_t)0,//BOTH_VS_LEANR_SL,			//# Lean right (saber left)		
		(animNumber_t)0,//BOTH_VS_LEANR_SR,			//# Lean right (saber right)		

		(animNumber_t)0,//BOTH_VS_ATL_S,				//# Attack left with saber		
		(animNumber_t)0,//BOTH_VS_ATR_S,				//# Attack right with saber		
		(animNumber_t)0,//BOTH_VS_ATR_TO_L_S,			//# Attack toss saber from right to left hand
		(animNumber_t)0,//BOTH_VS_ATL_TO_R_S,			//# Attack toss saber from left to right hand
		(animNumber_t)0,//BOTH_VS_ATR_G,				//# Attack right with gun (90)		
		(animNumber_t)0,//BOTH_VS_ATL_G,				//# Attack left with gun (90)		
		(animNumber_t)0,//BOTH_VS_ATF_G,				//# Attack forward with gun		

		(animNumber_t)0,//BOTH_VS_PAIN1,				//# Pain

	// Added 12/04/02 by Aurelio.
	//* #sep BOTH_ TAUNTAUN ANIMS
		(animNumber_t)0,//BOTH_VT_MOUNT_L,		//# Mount from left	
		(animNumber_t)0,//BOTH_VT_MOUNT_R,		//# Mount from right
		(animNumber_t)0,//BOTH_VT_MOUNT_B,		//# Mount from air, behind
		(animNumber_t)0,//BOTH_VT_DISMOUNT,		//# Dismount for tauntaun
		(animNumber_t)0,//BOTH_VT_DISMOUNT_L,		//# Dismount to tauntauns left	
		(animNumber_t)0,//BOTH_VT_DISMOUNT_R,		//# Dismount to tauntauns right (symmetry)	

		(animNumber_t)0,//BOTH_VT_WALK_FWD,		//# Walk forward	
		(animNumber_t)0,//BOTH_VT_WALK_REV,		//# Walk backward	
		(animNumber_t)0,//BOTH_VT_WALK_FWD_L,		//# walk lean left
		(animNumber_t)0,//BOTH_VT_WALK_FWD_R,		//# Walk lean right
		(animNumber_t)0,//BOTH_VT_RUN_FWD,		//# Run forward	
		(animNumber_t)0,//BOTH_VT_RUN_REV,		//# Look backwards while running (not weapon specific)	
		(animNumber_t)0,//BOTH_VT_RUN_FWD_L,		//# Run lean left
		(animNumber_t)0,//BOTH_VT_RUN_FWD_R,		//# Run lean right

		(animNumber_t)0,//BOTH_VT_SLIDEF,			//# Tauntaun slides forward with abrupt stop	
		(animNumber_t)0,//BOTH_VT_AIR,			//# Tauntaun jump	
		(animNumber_t)0,//BOTH_VT_ATB,			//# Tauntaun tail swipe	
		(animNumber_t)0,//BOTH_VT_PAIN1,			//# Pain	
		(animNumber_t)0,//BOTH_VT_DEATH1,			//# Die	
		(animNumber_t)0,//BOTH_VT_STAND,			//# Stand still and breath	
		(animNumber_t)0,//BOTH_VT_BUCK,			//# Tauntaun bucking loop animation	

		(animNumber_t)0,//BOTH_VT_LAND,			//# Player bounces down when tauntaun lands	
		(animNumber_t)0,//BOTH_VT_TURBO,			//# Hit The Turbo Button
		(animNumber_t)0,//BOTH_VT_IDLE_SL,		//# Sit (saber left)		
		(animNumber_t)0,//BOTH_VT_IDLE_SR,		//# Sit (saber right)		

		(animNumber_t)0,//BOTH_VT_IDLE,			//# Sit with no weapon selected	
		(animNumber_t)0,//BOTH_VT_IDLE1,			//# Sit with no weapon selected	
		(animNumber_t)0,//BOTH_VT_IDLE_S,			//# Sit with saber selected	
		(animNumber_t)0,//BOTH_VT_IDLE_G,			//# Sit with gun selected	
		(animNumber_t)0,//BOTH_VT_IDLE_T,			//# Sit with thermal grenade selected

		(animNumber_t)0,//BOTH_VT_ATL_S,			//# Attack left with saber	
		(animNumber_t)0,//BOTH_VT_ATR_S,			//# Attack right with saber	
		(animNumber_t)0,//BOTH_VT_ATR_TO_L_S,		//# Attack toss saber from right to left hand
		(animNumber_t)0,//BOTH_VT_ATL_TO_R_S,		//# Attack toss saber from left to right hand
		(animNumber_t)0,//BOTH_VT_ATR_G,			//# Attack right with gun (90)	
		(animNumber_t)0,//BOTH_VT_ATL_G,			//# Attack left with gun (90)	
		(animNumber_t)0,//BOTH_VT_ATF_G,			//# Attack forward with gun	


	// Added 2/26/02 by Aurelio.
	//* #sep BOTH_ FIGHTER ANIMS
			(animNumber_t)0,//BOTH_GEARS_OPEN,
			(animNumber_t)0,//BOTH_GEARS_CLOSE,
			(animNumber_t)0,//BOTH_WINGS_OPEN,
			(animNumber_t)0,//BOTH_WINGS_CLOSE,

	BOTH_DEATH14_UNGRIP,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_KNEES1,			//# Tavion on her knees
	BOTH_KNEES2,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1,			//# Transition of KNEES2 to KNEES1

	//# #sep BOTH_ MOVING
	BOTH_WALK1,				//# Normal walk
	BOTH_WALK2,				//# Normal walk
			(animNumber_t)0,//BOTH_WALK_STAFF,		//# Walk with saberstaff turned on
			(animNumber_t)0,//BOTH_WALKBACK_STAFF,	//# Walk backwards with saberstaff turned on
			(animNumber_t)0,//BOTH_WALK_DUAL,			//# Walk with dual turned on
			(animNumber_t)0,//BOTH_WALKBACK_DUAL,		//# Walk backwards with dual turned on
	BOTH_WALK5,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7,				//# Fast walk
	BOTH_RUN1,				//# Full run
	BOTH_RUN1START,			//# Start into full run1
	BOTH_RUN1STOP,			//# Stop from full run1
	BOTH_RUN2,				//# Full run
			(animNumber_t)0,//BOTH_RUN1TORUN2,		//# Wampa run anim transition
			(animNumber_t)0,//BOTH_RUN2TORUN1,		//# Wampa run anim transition
			(animNumber_t)0,//BOTH_RUN4,				//# Jawa Run
			(animNumber_t)0,//BOTH_RUN_STAFF,			//# Run with saberstaff turned on
			(animNumber_t)0,//BOTH_RUNBACK_STAFF,		//# Run backwards with saberstaff turned on
			(animNumber_t)0,//BOTH_RUN_DUAL,			//# Run with dual turned on
			(animNumber_t)0,//BOTH_RUNBACK_DUAL,		//# Run backwards with dual turned on
	BOTH_STRAFE_LEFT1,		//# Sidestep left, should loop
	BOTH_STRAFE_RIGHT1,		//# Sidestep right, should loop
	BOTH_RUNSTRAFE_LEFT1,	//# Sidestep left, should loop
	BOTH_RUNSTRAFE_RIGHT1,	//# Sidestep right, should loop
	BOTH_TURN_LEFT1,		//# Turn left, should loop
	BOTH_TURN_RIGHT1,		//# Turn right, should loop
	BOTH_TURNSTAND1,		//# Turn from STAND1 position
	BOTH_TURNSTAND2,		//# Turn from STAND2 position
	BOTH_TURNSTAND3,		//# Turn from STAND3 position
	BOTH_TURNSTAND4,		//# Turn from STAND4 position
	BOTH_TURNSTAND5,		//# Turn from STAND5 position
	BOTH_TURNCROUCH1,		//# Turn from CROUCH1 position

	BOTH_WALKBACK1,			//# Walk1 backwards
	BOTH_WALKBACK2,			//# Walk2 backwards
	BOTH_RUNBACK1,			//# Run1 backwards
	BOTH_RUNBACK2,			//# Run1 backwards

	//# #sep BOTH_ JUMPING
	BOTH_JUMP1,				//# Jump - wind-up and leave ground
	BOTH_INAIR1,			//# In air loop (from jump)
	BOTH_LAND1,				//# Landing (from in air loop)
	BOTH_LAND2,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1,		//# In air loop (from jump back)
	BOTH_LANDBACK1,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1,		//# In air loop (from jump left)
	BOTH_LANDLEFT1,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1,		//# In air loop (from jump)
	BOTH_FORCELAND1,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F,			//# Flip forward
	BOTH_FLIP_B,			//# Flip backwards
	BOTH_FLIP_L,			//# Flip left
	BOTH_FLIP_R,			//# Flip right

	BOTH_ROLL_F,			//# Roll forward
	BOTH_ROLL_B,			//# Roll backward
	BOTH_ROLL_L,			//# Roll left
	BOTH_ROLL_R,			//# Roll right

	BOTH_HOP_F,				//# quickstep forward
	BOTH_HOP_B,				//# quickstep backwards
	BOTH_HOP_L,				//# quickstep left
	BOTH_HOP_R,				//# quickstep right

	BOTH_DODGE_FL,			//# lean-dodge forward left
	BOTH_DODGE_FR,			//# lean-dodge forward right
	BOTH_DODGE_BL,			//# lean-dodge backwards left
	BOTH_DODGE_BR,			//# lean-dodge backwards right
	BOTH_DODGE_L,			//# lean-dodge left
	BOTH_DODGE_R,			//# lean-dodge right
		(animNumber_t)0,//BOTH_DODGE_HOLD_FL,			//# lean-dodge pose forward left
		(animNumber_t)0,//BOTH_DODGE_HOLD_FR,			//# lean-dodge pose forward right
		(animNumber_t)0,//BOTH_DODGE_HOLD_BL,			//# lean-dodge pose backwards left
		(animNumber_t)0,//BOTH_DODGE_HOLD_BR,			//# lean-dodge pose backwards right
		(animNumber_t)0,//BOTH_DODGE_HOLD_L,			//# lean-dodge pose left
		(animNumber_t)0,//BOTH_DODGE_HOLD_R,			//# lean-dodge pose right

	//MP taunt anims
	BOTH_ENGAGETAUNT,
				(animNumber_t)0,//BOTH_BOW,
				(animNumber_t)0,//BOTH_MEDITATE,
				(animNumber_t)0,//BOTH_MEDITATE_END,
				(animNumber_t)0,//BOTH_SHOWOFF_FAST,
		(animNumber_t)0,//BOTH_SHOWOFF_MEDIUM,
		(animNumber_t)0,//BOTH_SHOWOFF_STRONG,
		(animNumber_t)0,//BOTH_SHOWOFF_DUAL,
		(animNumber_t)0,//BOTH_SHOWOFF_STAFF,
		(animNumber_t)0,//BOTH_VICTORY_FAST,
		(animNumber_t)0,//BOTH_VICTORY_MEDIUM,
		(animNumber_t)0,//BOTH_VICTORY_STRONG,
				(animNumber_t)0,//BOTH_VICTORY_DUAL,
				(animNumber_t)0,//BOTH_VICTORY_STAFF,
	//other saber/acro anims
	BOTH_ARIAL_LEFT,		//# 
	BOTH_ARIAL_RIGHT,		//# 
	BOTH_CARTWHEEL_LEFT,	//# 
	BOTH_CARTWHEEL_RIGHT,	//# 
	BOTH_FLIP_LEFT,			//# 
	BOTH_FLIP_BACK1,		//# 
	BOTH_FLIP_BACK2,		//# 
	BOTH_FLIP_BACK3,		//# 
	BOTH_BUTTERFLY_LEFT,	//# 
	BOTH_BUTTERFLY_RIGHT,	//# 
	BOTH_WALL_RUN_RIGHT,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP,//#
	BOTH_WALL_RUN_RIGHT_STOP,//# 
	BOTH_WALL_RUN_LEFT,		//# 
	BOTH_WALL_RUN_LEFT_FLIP,//#
	BOTH_WALL_RUN_LEFT_STOP,//# 
	BOTH_WALL_FLIP_RIGHT,	//# 
	BOTH_WALL_FLIP_LEFT,	//# 
	BOTH_KNOCKDOWN1,		//# knocked backwards
	BOTH_KNOCKDOWN2,		//# knocked backwards hard
	BOTH_KNOCKDOWN3,		//#	knocked forwards
	BOTH_KNOCKDOWN4,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5,		//# dupe of 3 - will be removed
	BOTH_GETUP1,			//#
	BOTH_GETUP2,			//#
	BOTH_GETUP3,			//#
	BOTH_GETUP4,			//#
	BOTH_GETUP5,			//#
	BOTH_GETUP_CROUCH_F1,	//#
	BOTH_GETUP_CROUCH_B1,	//#
	BOTH_FORCE_GETUP_F1,	//#
	BOTH_FORCE_GETUP_F2,	//#
	BOTH_FORCE_GETUP_B1,	//#
	BOTH_FORCE_GETUP_B2,	//#
	BOTH_FORCE_GETUP_B3,	//#
	BOTH_FORCE_GETUP_B4,	//#
	BOTH_FORCE_GETUP_B5,	//#
	BOTH_FORCE_GETUP_B6,	//#
		(animNumber_t)0,//BOTH_GETUP_BROLL_B,	//#
		(animNumber_t)0,//BOTH_GETUP_BROLL_F,	//#
		(animNumber_t)0,//BOTH_GETUP_BROLL_L,	//#
		(animNumber_t)0,//BOTH_GETUP_BROLL_R,	//#
		(animNumber_t)0,//BOTH_GETUP_FROLL_B,	//#
		(animNumber_t)0,//BOTH_GETUP_FROLL_F,	//#
		(animNumber_t)0,//BOTH_GETUP_FROLL_L,	//#
		(animNumber_t)0,//BOTH_GETUP_FROLL_R,	//#
	BOTH_WALL_FLIP_BACK1,	//#
	BOTH_WALL_FLIP_BACK2,	//#
	BOTH_SPIN1,				//#
	BOTH_CEILING_CLING,		//# clinging to ceiling
	BOTH_CEILING_DROP,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR,		//# jump spin slash bl to tr
	BOTH_RIGHTHANDCHOPPEDOFF,//#
	BOTH_DEFLECTSLASH__R__L_FIN,//#
	BOTH_BASHED1,//#
	BOTH_ARIAL_F1,//#
	BOTH_BUTTERFLY_FR1,//#
	BOTH_BUTTERFLY_FL1,//#

	//NEW SABER/JEDI/FORCE ANIMS
		(animNumber_t)0,//BOTH_BACK_FLIP_UP,	//# back flip up Bonus Animation!!!!	
		(animNumber_t)0,//BOTH_LOSE_SABER,	//# player losing saber (pulled from hand by force pull 4 - Kyle?)
		(animNumber_t)0,//BOTH_STAFF_TAUNT,	//# taunt saberstaff			
		(animNumber_t)0,//BOTH_DUAL_TAUNT,		//# taunt dual
		(animNumber_t)0,//BOTH_A6_FB,				//# dual attack front/back		
		(animNumber_t)0,//BOTH_A6_LR,				//# dual attack left/right
		(animNumber_t)0,//BOTH_A7_HILT,			//# saber knock (alt + stand still)
	//Alora			
		(animNumber_t)0,//BOTH_ALORA_SPIN,		//#jump spin attack	death ballet	
		(animNumber_t)0,//BOTH_ALORA_FLIP_1,		//# gymnast move 1		
		(animNumber_t)0,//BOTH_ALORA_FLIP_2,		//# gymnast move 2		
		(animNumber_t)0,//BOTH_ALORA_FLIP_3,		//# gymnast move3		
		(animNumber_t)0,//BOTH_ALORA_FLIP_B,		//# gymnast move back		
		(animNumber_t)0,//BOTH_ALORA_SPIN_THROW,	//# dual saber throw		
			(animNumber_t)0,//BOTH_ALORA_SPIN_SLASH,	//# spin slash	special bonus animation!! :)	
			(animNumber_t)0,//BOTH_ALORA_TAUNT,		//# special taunt
	//Rosh (Kothos battle)			
			(animNumber_t)0,//BOTH_ROSH_PAIN,	//# hurt animation (exhausted)		
			(animNumber_t)0,//BOTH_ROSH_HEAL,	//# healed/rejuvenated		
	//Tavion			
		(animNumber_t)0,//BOTH_TAVION_SCEPTERGROUND, //# stabbing ground with sith sword shoots electricity everywhere
		(animNumber_t)0,//BOTH_TAVION_SWORDPOWER,//# Tavion doing the He-Man(tm) thing
		(animNumber_t)0,//BOTH_SCEPTER_START,	//#Point scepter and attack start
					(animNumber_t)0,//BOTH_SCEPTER_HOLD,	//#Point scepter and attack hold
					(animNumber_t)0,//BOTH_SCEPTER_STOP,	//#Point scepter and attack stop
	//Kyle Boss			
					(animNumber_t)0,//BOTH_KYLE_GRAB,		//# grab
			(animNumber_t)0,//BOTH_KYLE_MISS,		//# miss
			(animNumber_t)0,//BOTH_KYLE_PA_1,		//# hold 1
			(animNumber_t)0,//BOTH_PLAYER_PA_1,	//# player getting held 1
			(animNumber_t)0,//BOTH_KYLE_PA_2,		//# hold 2
			(animNumber_t)0,//BOTH_PLAYER_PA_2,	//# player getting held 2
			(animNumber_t)0,//BOTH_PLAYER_PA_FLY,	//# player getting knocked back from punch at end of hold 1
			(animNumber_t)0,//BOTH_KYLE_PA_3,		//# hold 3
					(animNumber_t)0,//BOTH_PLAYER_PA_3,	//# player getting held 3
					(animNumber_t)0,//BOTH_PLAYER_PA_3_FLY,//# player getting thrown at end of hold 3
	//Rancor
		(animNumber_t)0,//BOTH_BUCK_RIDER,	//# Rancor bucks when someone is on him
	//WAMPA Grabbing enemy
		(animNumber_t)0,//BOTH_HOLD_START,	//#
		(animNumber_t)0,//BOTH_HOLD_MISS,	//#
		(animNumber_t)0,//BOTH_HOLD_IDLE,	//#
		(animNumber_t)0,//BOTH_HOLD_END,	//#
		(animNumber_t)0,//BOTH_HOLD_ATTACK,	//#
		(animNumber_t)0,//BOTH_HOLD_SNIFF,	//# Sniff the guy you're holding
		(animNumber_t)0,//BOTH_HOLD_DROP,		//# just drop 'em
	//BEING GRABBED BY WAMPA
		(animNumber_t)0,//BOTH_GRABBED,	//#
		(animNumber_t)0,//BOTH_RELEASED,	//#
		(animNumber_t)0,//BOTH_HANG_IDLE,	//#
		(animNumber_t)0,//BOTH_HANG_ATTACK,	//#
		(animNumber_t)0,//BOTH_HANG_PAIN,	//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE,		//#	Just sitting on the ladder

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_SHIELDED,		//# For sentry droid, shields in

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1,		//# Swimming Idle 1
	BOTH_SWIMFORWARD,		//# Swim forward loop
		(animNumber_t)0,//BOTH_SWIMBACKWARD,		//# Swim backward loop

	//# #sep BOTH_ LYING
	BOTH_SLEEP1,			//# laying on back-rknee up-rhand on torso
	BOTH_SLEEP6START,		//# Kyle leaning back to sleep (cin 20)
	BOTH_SLEEP6STOP,		//# Kyle waking up and shaking his head (cin 21)
	BOTH_SLEEP1GETUP,		//# alarmed and getting up out of sleep1 pose to stand
	BOTH_SLEEP1GETUP2,		//# 

	BOTH_CHOKE1START,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD,	//# loop of tavion in force grip choke
	BOTH_CHOKE1,			//# tavion in force grip choke

	BOTH_CHOKE2,			//# tavion recovering from force grip choke
	BOTH_CHOKE3,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1,			//# Wakes up

	BOTH_TURNON,			//# Protocol Droid wakes up
	BOTH_TURNOFF,			//# Protocol Droid shuts off

	BOTH_BUTTON1,			//# Single button push with right hand
	BOTH_BUTTON2,			//# Single button push with left finger
	BOTH_BUTTON_HOLD,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	//# #sep BOTH_ FORCE ANIMS
	BOTH_RESISTPUSH,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH,			//# Use off-hand to do force power.
	BOTH_FORCEPULL,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_START,	//# Use off-hand to do lightning - start
	BOTH_FORCELIGHTNING_HOLD,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK,		//# Healing meditation gesture
	BOTH_SABERPULL,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP3,		//# force-gripping (right hand)
	BOTH_FORCEGRIP3THROW,	//# throwing while force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE,//# Use off-hand to do grip - release
	BOTH_TOSS1,				//# throwing to left after force gripping
	BOTH_TOSS2,				//# throwing to right after force gripping
	//NEW force anims for JKA:
				(animNumber_t)0,//BOTH_FORCE_RAGE,
				(animNumber_t)0,//BOTH_FORCE_2HANDEDLIGHTNING,
				(animNumber_t)0,//BOTH_FORCE_2HANDEDLIGHTNING_START,
				(animNumber_t)0,//BOTH_FORCE_2HANDEDLIGHTNING_HOLD,
			(animNumber_t)0,//BOTH_FORCE_2HANDEDLIGHTNING_RELEASE,
			(animNumber_t)0,//BOTH_FORCE_DRAIN,
			(animNumber_t)0,//BOTH_FORCE_DRAIN_START,
			(animNumber_t)0,//BOTH_FORCE_DRAIN_HOLD,
						(animNumber_t)0,//BOTH_FORCE_DRAIN_RELEASE,
						(animNumber_t)0,//BOTH_FORCE_DRAIN_GRAB_START,
						(animNumber_t)0,//BOTH_FORCE_DRAIN_GRAB_HOLD,
						(animNumber_t)0,//BOTH_FORCE_DRAIN_GRAB_END,
				(animNumber_t)0,//BOTH_FORCE_DRAIN_GRABBED,
				(animNumber_t)0,//BOTH_FORCE_ABSORB,
				(animNumber_t)0,//BOTH_FORCE_ABSORB_START,
				(animNumber_t)0,//BOTH_FORCE_ABSORB_END,
				(animNumber_t)0,//BOTH_FORCE_PROTECT,
				(animNumber_t)0,//BOTH_FORCE_PROTECT_FAST,

						(animNumber_t)0,//BOTH_WIND,

			(animNumber_t)0,//BOTH_STAND_TO_KNEEL,
			(animNumber_t)0,//BOTH_KNEEL_TO_STAND,

			(animNumber_t)0,//BOTH_TUSKENATTACK1,
			(animNumber_t)0,//BOTH_TUSKENATTACK2,
			(animNumber_t)0,//BOTH_TUSKENATTACK3,
			(animNumber_t)0,//BOTH_TUSKENLUNGE1,
			(animNumber_t)0,//BOTH_TUSKENTAUNT1,

			(animNumber_t)0,//BOTH_COWER1_START,		//# cower start
			(animNumber_t)0,//BOTH_COWER1,			//# cower loop
			(animNumber_t)0,//BOTH_COWER1_STOP,		//# cower stop
			(animNumber_t)0,//BOTH_SONICPAIN_START,
			(animNumber_t)0,//BOTH_SONICPAIN_HOLD,
			(animNumber_t)0,//BOTH_SONICPAIN_END,

	//new anim slots per Jarrod's request
		(animNumber_t)0,//BOTH_STAND10,
		(animNumber_t)0,//BOTH_STAND10_TALK1,
		(animNumber_t)0,//BOTH_STAND10_TALK2,
		(animNumber_t)0,//BOTH_STAND10TOSTAND1,

		(animNumber_t)0,//BOTH_STAND1_TALK1,
		(animNumber_t)0,//BOTH_STAND1_TALK2,
		(animNumber_t)0,//BOTH_STAND1_TALK3,

		(animNumber_t)0,//BOTH_SIT4,
		(animNumber_t)0,//BOTH_SIT5,
		(animNumber_t)0,//BOTH_SIT5_TALK1,
			(animNumber_t)0,//BOTH_SIT5_TALK2,
		(animNumber_t)0,//BOTH_SIT5_TALK3,

		(animNumber_t)0,//BOTH_SIT6,
	BOTH_SIT7,

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1,		//# Put weapon away
	TORSO_DROPWEAP4,		//# Put weapon away
	TORSO_RAISEWEAP1,		//# Draw Weapon
	TORSO_RAISEWEAP4,		//# Draw Weapon
	TORSO_WEAPONREADY1,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY10,	//# Ready to fire thermal det
	TORSO_WEAPONIDLE2,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4,		//# Holding sniper rifle
	TORSO_WEAPONIDLE10,		//# Holding thermal det

	//# #sep TORSO_ MISC
	TORSO_SURRENDER_START,	//# arms up
	TORSO_SURRENDER_STOP,	//# arms back down

	TORSO_CHOKING1,			//# TEMP

		(animNumber_t)0,//TORSO_HANDSIGNAL1,
		(animNumber_t)0,//TORSO_HANDSIGNAL2,
		(animNumber_t)0,//TORSO_HANDSIGNAL3,
		(animNumber_t)0,//TORSO_HANDSIGNAL4,
		(animNumber_t)0,//TORSO_HANDSIGNAL5,


	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_TURN1,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1,		//# Lean left
	LEGS_LEAN_RIGHT1,		//# Lean Right
	LEGS_CHOKING1,			//# TEMP
	LEGS_LEFTUP1,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1,
	LEGS_S1_LUP2,
	LEGS_S1_LUP3,
	LEGS_S1_LUP4,
	LEGS_S1_LUP5,
	LEGS_S1_RUP1,
	LEGS_S1_RUP2,
	LEGS_S1_RUP3,
	LEGS_S1_RUP4,
	LEGS_S1_RUP5,
	LEGS_S3_LUP1,
	LEGS_S3_LUP2,
	LEGS_S3_LUP3,
	LEGS_S3_LUP4,
	LEGS_S3_LUP5,
	LEGS_S3_RUP1,
	LEGS_S3_RUP2,
	LEGS_S3_RUP3,
	LEGS_S3_RUP4,
	LEGS_S3_RUP5,
	LEGS_S4_LUP1,
	LEGS_S4_LUP2,
	LEGS_S4_LUP3,
	LEGS_S4_LUP4,
	LEGS_S4_LUP5,
	LEGS_S4_RUP1,
	LEGS_S4_RUP2,
	LEGS_S4_RUP3,
	LEGS_S4_RUP4,
	LEGS_S4_RUP5,
	LEGS_S5_LUP1,
	LEGS_S5_LUP2,
	LEGS_S5_LUP3,
	LEGS_S5_LUP4,
	LEGS_S5_LUP5,
	LEGS_S5_RUP1,
	LEGS_S5_RUP2,
	LEGS_S5_RUP3,
	LEGS_S5_RUP4,
	LEGS_S5_RUP5,
			(animNumber_t)0,//LEGS_S6_LUP1,
			(animNumber_t)0,//LEGS_S6_LUP2,
			(animNumber_t)0,//LEGS_S6_LUP3,
			(animNumber_t)0,//LEGS_S6_LUP4,
				(animNumber_t)0,//LEGS_S6_LUP5,
			(animNumber_t)0,//LEGS_S6_RUP1,
			(animNumber_t)0,//LEGS_S6_RUP2,
			(animNumber_t)0,//LEGS_S6_RUP3,
		(animNumber_t)0,//LEGS_S6_RUP4,
		(animNumber_t)0,//LEGS_S6_RUP5,
		(animNumber_t)0,//LEGS_S7_LUP1,
		(animNumber_t)0,//LEGS_S7_LUP2,
		(animNumber_t)0,//LEGS_S7_LUP3,
		(animNumber_t)0,//LEGS_S7_LUP4,
		(animNumber_t)0,//LEGS_S7_LUP5,
		(animNumber_t)0,//LEGS_S7_RUP1,
		(animNumber_t)0,//LEGS_S7_RUP2,
		(animNumber_t)0,//LEGS_S7_RUP3,
		(animNumber_t)0,//LEGS_S7_RUP4,
		(animNumber_t)0,//LEGS_S7_RUP5,

	//New anim as per Jarrod's request
		(animNumber_t)0,//LEGS_TURN180,


	//--------------------------------
	//JA+ new animations
	//--------------------------------

		(animNumber_t)0,//BOTH_KISSEE,
		(animNumber_t)0,//BOTH_KISSER,
	BOTH_KISSER1STOP,

			(animNumber_t)0,//BOTH_LEDGE_GRAB,
			(animNumber_t)0,//BOTH_LEDGE_HOLD,
			(animNumber_t)0,//BOTH_LEDGE_LEFT,
			(animNumber_t)0,//BOTH_LEDGE_RIGHT,
			(animNumber_t)0,//BOTH_LEDGE_MERCPULL,

		(animNumber_t)0,//BOTH_MELEE_BACKKICK,
		(animNumber_t)0,//BOTH_MELEE_SPINKICK,
		(animNumber_t)0,//TORSO_MAND_FLAME,

		(animNumber_t)0,//BOTH_STAND1TO2_NEW,
		(animNumber_t)0,//BOTH_STAND2TO1_NEW,
		(animNumber_t)0,//BOTH_S1_S7_NEW,
		(animNumber_t)0,//BOTH_S7_S1_NEW,
		(animNumber_t)0,//BOTH_S6_S1_LARM,
		(animNumber_t)0,//BOTH_S1_S6_LARM,
		(animNumber_t)0,//BOTH_S6_S1_2ARMS,
		(animNumber_t)0,//BOTH_S1_S6_2ARMS,

		(animNumber_t)0,//BOTH_BACK_FALLING,
		(animNumber_t)0,//BOTH_BACK_FALLING_GETUP,
		(animNumber_t)0,//BOTH_BACK_FALLING_GETUP_SLOW,

		(animNumber_t)0,//BOTH_JUMP_BACKKICK_SPIN,

		(animNumber_t)0,//BOTH_JUMP_BACKFLIP_ATCK,
		(animNumber_t)0,//BOTH_JUMP_BACKFLIP_ATCKEE,
		(animNumber_t)0,//BOTH_JUMP_BACKFLIP_ATCKEE_FALL,

		(animNumber_t)0,//BOTH_NEW_STABER,
		(animNumber_t)0,//BOTH_NEW_STABEE,

		(animNumber_t)0,//BOTH_FLIP_STAB,

		(animNumber_t)0,//BOTH_JUMP_BACKFLIP_ATCK_MISSED,


	//--------------------------------
	//JA++ new animations
	//--------------------------------
		(animNumber_t)0,//BOTH_WORM,


	//======================================================
	//cinematic anims
	//======================================================
	//# #sep BOTH_ CINEMATIC-ONLY
		(animNumber_t)0,//BOTH_CIN_1,		//# Level specific cinematic 1
		(animNumber_t)0,//BOTH_CIN_2,		//# Level specific cinematic 2
		(animNumber_t)0,//BOTH_CIN_3,		//# Level specific cinematic 3
		(animNumber_t)0,//BOTH_CIN_4,		//# Level specific cinematic 4
		(animNumber_t)0,//BOTH_CIN_5,		//# Level specific cinematic 5
		(animNumber_t)0,//BOTH_CIN_6,		//# Level specific cinematic 6
		(animNumber_t)0,//BOTH_CIN_7,		//# Level specific cinematic 7
		(animNumber_t)0,//BOTH_CIN_8,		//# Level specific cinematic 8
		(animNumber_t)0,//BOTH_CIN_9,		//# Level specific cinematic 9
		(animNumber_t)0,//BOTH_CIN_10,		//# Level specific cinematic 10
		(animNumber_t)0,//BOTH_CIN_11,		//# Level specific cinematic 11
		(animNumber_t)0,//BOTH_CIN_12,		//# Level specific cinematic 12
		(animNumber_t)0,//BOTH_CIN_13,		//# Level specific cinematic 13
		(animNumber_t)0,//BOTH_CIN_14,		//# Level specific cinematic 14
		(animNumber_t)0,//BOTH_CIN_15,		//# Level specific cinematic 15
		(animNumber_t)0,//BOTH_CIN_16,		//# Level specific cinematic 16
		(animNumber_t)0,//BOTH_CIN_17,		//# Level specific cinematic 17
		(animNumber_t)0,//BOTH_CIN_18,		//# Level specific cinematic 18
		(animNumber_t)0,//BOTH_CIN_19,		//# Level specific cinematic 19
		(animNumber_t)0,//BOTH_CIN_20,		//# Level specific cinematic 20
				(animNumber_t)0,//BOTH_CIN_21,		//# Level specific cinematic 21
				(animNumber_t)0,//BOTH_CIN_22,		//# Level specific cinematic 22
				(animNumber_t)0,//BOTH_CIN_23,		//# Level specific cinematic 23
				(animNumber_t)0,//BOTH_CIN_24,		//# Level specific cinematic 24
					(animNumber_t)0,//BOTH_CIN_25,		//# Level specific cinematic 25
				(animNumber_t)0,//BOTH_CIN_26,		//# Level specific cinematic 
				(animNumber_t)0,//BOTH_CIN_27,		//# Level specific cinematic 
				(animNumber_t)0,//BOTH_CIN_28,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_29,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_30,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_31,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_32,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_33,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_34,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_35,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_36,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_37,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_38,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_39,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_40,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_41,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_42,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_43,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_44,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_45,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_46,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_47,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_48,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_49,		//# Level specific cinematic 
			(animNumber_t)0,//BOTH_CIN_50,		//# Level specific cinematic

	//# #eol
	MAX_ANIMATIONS,
	MAX_TOTALANIMATIONS,
};



#define	MAX_CLIENTS_JKA			32		// absolute limit
#define MAX_TERRAINS_JKA		1//32 //rwwRMG: inserted
#define MAX_LOCATIONS_JKA		64


// these are also in be_aas_def.h - argh (rjr)
#define	MAX_MODELS_JKA			512		// these are sent over the net as -12 bits
#define	MAX_SOUNDS_JKA			256		// so they cannot be blindly increased
#define MAX_ICONS_JKA			64		// max registered icons you can have per map 
#define MAX_FX_JKA				64		// max effects strings, I'm hoping that 64 will be plenty

#define MAX_SUB_BSP_JKA			32 //rwwRMG - added

/*
Ghoul2 Insert Start
*/
#define	MAX_G2BONES_JKA		64		//rww - changed from MAX_CHARSKINS to MAX_G2BONES. value still equal.
/*
Ghoul2 Insert End
*/

#define MAX_AMBIENT_SETS_JKA		256 //rww - ambient soundsets must be sent over in config strings.

#define	MAX_CONFIGSTRINGS_JKA	1700 //this is getting pretty high. Try not to raise it anymore than it already is.



#define	CS_MUSIC_JKA				2
#define	CS_MESSAGE_JKA				3		// from the map worldspawn's message field
#define	CS_MOTD_JKA					4		// g_motd string for server message of the day
#define	CS_WARMUP_JKA				5		// server time when the match will be restarted
#define	CS_SCORES1_JKA				6
#define	CS_SCORES2_JKA				7
#define CS_VOTE_TIME_JKA			8
#define CS_VOTE_STRING_JKA			9
#define	CS_VOTE_YES_JKA				10
#define	CS_VOTE_NO_JKA				11

#define CS_TEAMVOTE_TIME_JKA		12
#define CS_TEAMVOTE_STRING_JKA		14
#define	CS_TEAMVOTE_YES_JKA			16
#define	CS_TEAMVOTE_NO_JKA			18

#define	CS_GAME_VERSION_JKA			20
#define	CS_LEVEL_START_TIME_JKA		21		// so the timer only shows the current level
#define	CS_INTERMISSION_JKA			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS_JKA			23		// string indicating flag status in CTF
#define CS_SHADERSTATE_JKA			24
#define CS_BOTINFO_JKA				25

#define	CS_ITEMS_JKA				27		// string of 0's and 1's that tell which items are present

#define CS_CLIENT_JEDIMASTER_JKA	28		// current jedi master
#define CS_CLIENT_DUELWINNER_JKA	29		// current duel round winner - needed for printing at top of scoreboard
#define CS_CLIENT_DUELISTS_JKA		30		// client numbers for both current duelists. Needed for a number of client-side things.
#define CS_CLIENT_DUELHEALTHS_JKA	31		// nmckenzie: DUEL_HEALTH.  Hopefully adding this cs is safe and good?
#define CS_GLOBAL_AMBIENT_SET_JKA	32

#define CS_AMBIENT_SET_JKA			37

#define CS_SIEGE_STATE_JKA			(CS_AMBIENT_SET_JKA+MAX_AMBIENT_SETS_JKA)
#define CS_SIEGE_OBJECTIVES_JKA		(CS_SIEGE_STATE_JKA+1)
#define CS_SIEGE_TIMEOVERRIDE_JKA	(CS_SIEGE_OBJECTIVES_JKA+1)
#define CS_SIEGE_WINTEAM_JKA		(CS_SIEGE_TIMEOVERRIDE_JKA+1)
#define CS_SIEGE_ICONS_JKA			(CS_SIEGE_WINTEAM_JKA+1)

#define	CS_MODELS_JKA				(CS_SIEGE_ICONS_JKA+1)
#define	CS_SKYBOXORG_JKA			(CS_MODELS_JKA+MAX_MODELS_JKA)		//rww - skybox info
#define	CS_SOUNDS_JKA				(CS_SKYBOXORG_JKA+1)
#define CS_ICONS_JKA				(CS_SOUNDS_JKA+MAX_SOUNDS_JKA)
#define	CS_PLAYERS_JKA				(CS_ICONS_JKA+MAX_ICONS_JKA)
/*
Ghoul2 Insert Start
*/
#define CS_G2BONES_JKA				(CS_PLAYERS_JKA+MAX_CLIENTS_JKA)
//rww - used to be CS_CHARSKINS, but I have eliminated the need for that.
/*
Ghoul2 Insert End
*/
#define CS_LOCATIONS_JKA			(CS_G2BONES_JKA+MAX_G2BONES_JKA)
#define CS_PARTICLES_JKA			(CS_LOCATIONS_JKA+MAX_LOCATIONS_JKA) 
#define CS_EFFECTS_JKA				(CS_PARTICLES_JKA+MAX_LOCATIONS_JKA)
#define	CS_LIGHT_STYLES_JKA			(CS_EFFECTS_JKA + MAX_FX_JKA)

//rwwRMG - added:
#define CS_TERRAINS_JKA				(CS_LIGHT_STYLES_JKA + (MAX_LIGHT_STYLES_JKA*3))
#define CS_BSP_MODELS_JKA			(CS_TERRAINS_JKA + MAX_TERRAINS_JKA)

#define CS_MAX_JKA					(CS_BSP_MODELS_JKA + MAX_SUB_BSP_JKA)

#if (CS_MAX_JKA) > MAX_CONFIGSTRINGS_JKA
#error overflow: (CS_MAX_JKA) > MAX_CONFIGSTRINGS_JKA
#endif





//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#ifdef _OPTIMIZED_VEHICLE_NETWORKING
//Instead of sending 2 full playerStates for the pilot and the vehicle, send a smaller,
//specialized pilot playerState and vehicle playerState.  Also removes some vehicle
//fields from the normal playerState -mcg
//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================

static netField_t	playerStateFieldsJKA[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[1]), 0 },
{ PSF(origin[0]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(weaponTime), -16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(speedJKA), 0 }, //sadly, the vehicles require negative speed values, so..
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(delta_angles[0]), 16 },
{ PSF(torsoAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(eFlags), 32 },
{ PSF(fd.forcePower), 8 },
{ PSF(eventSequence), 16 },
{ PSF(torsoTimer), 16 },
{ PSF(legsTimer), 16 },
{ PSF(viewheight), -8 },
{ PSF(fd.saberAnimLevel), 4 },
{ PSF(rocketLockIndex), GENTITYNUM_BITS },
{ PSF(fd.saberDrawAnimLevel), 4 },
{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(customRGBA[0]), 8 }, //0-255
{ PSF(movementDir), 4 },
{ PSF(saberEntityNum), GENTITYNUM_BITS }, //Also used for channel tracker storage, but should never exceed entity number
{ PSF(customRGBA[3]), 8 }, //0-255
{ PSF(weaponstate), 4 },
{ PSF(saberMove), 32 }, //This value sometimes exceeds the max LS_ value and gets set to a crazy amount, so it needs 32 bits
{ PSF(standheight), 10 },
{ PSF(crouchheight), 10 },
{ PSF(basespeed), -16 },
{ PSF(pm_flags), 16 },
{ PSF(jetpackFuel), 8 },
{ PSF(cloakFuel), 8 },
{ PSF(pm_time), -16 },
{ PSF(customRGBA[1]), 8 }, //0-255
{ PSF(clientNum), GENTITYNUM_BITS },
{ PSF(duelIndex), GENTITYNUM_BITS },
{ PSF(customRGBA[2]), 8 }, //0-255
{ PSF(gravity), 16 },
{ PSF(weapon), 8 },
{ PSF(delta_angles[2]), 16 },
{ PSF(saberCanThrow), 1 },
{ PSF(viewangles[2]), 0 },
{ PSF(fd.forcePowersKnown), 32 },
{ PSF(fd.forcePowerLevel[FP_LEVITATION]), 2 }, //unfortunately we need this for fall damage calculation (client needs to know the distance for the fall noise)
{ PSF(fd.forcePowerDebounce[FP_LEVITATION]), 32 },
{ PSF(fd.forcePowerSelected), 8 },
{ PSF(torsoFlip), 1 },
{ PSF(externalEvent), 10 },
{ PSF(damageYaw), 8 },
{ PSF(damageCount), 8 },
{ PSF(inAirAnim), 1 }, //just transmit it for the sake of knowing right when on the client to play a land anim, it's only 1 bit
{ PSF(eventParms[1]), 8 },
{ PSF(fd.forceSide), 2 }, //so we know if we should apply greyed out shaders to dark/light force enlightenment
{ PSF(saberAttackChainCount), 4 },
{ PSF(pm_type), 8 },
{ PSF(externalEventParm), 8 },
{ PSF(eventParms[0]), -16 },
{ PSF(lookTarget), GENTITYNUM_BITS },
//{ PSF(vehOrientation[0]), 0 },
{ PSF(weaponChargeSubtractTime), 32 }, //? really need 32 bits??
//{ PSF(vehOrientation[1]), 0 },
//{ PSF(moveDir[1]), 0 },
//{ PSF(moveDir[0]), 0 },
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
//{ PSF(vehOrientation[2]), 0 },
{ PSF(legsFlip), 1 },
{ PSF(damageEvent), 8 },
//{ PSF(moveDir[2]), 0 },
{ PSF(rocketTargetTime), 32 },
{ PSF(activeForcePass), 6 },
{ PSF(electrifyTime), 32 },
{ PSF(fd.forceJumpZStart), 0 },
{ PSF(loopSound), 16 }, //rwwFIXMEFIXME: max sounds is 256, doesn't this only need to be 8?
{ PSF(hasLookTarget), 1 },
{ PSF(saberBlocked), 8 },
{ PSF(damageType), 2 },
{ PSF(rocketLockTime), 32 },
{ PSF(forceHandExtend), 8 },
{ PSF(saberHolsteredJKA), 2 },
{ PSF(fd.forcePowersActive), 32 },
{ PSF(damagePitch), 8 },
{ PSF(m_iVehicleNum), GENTITYNUM_BITS }, // 10 bits fits all possible entity nums (2^10 = 1024). - AReis
//{ PSF(vehTurnaroundTime), 32 },//only used by vehicle?
{ PSF(generic1), 8 },
{ PSF(jumppad_ent), 10 },
{ PSF(hasDetPackPlanted), 1 },
{ PSF(saberInFlight), 1 },
{ PSF(forceDodgeAnim), 16 },
{ PSF(zoomMode), 2 }, // NOTENOTE Are all of these necessary?
{ PSF(hackingTime), 32 },
{ PSF(zoomTime), 32 },	// NOTENOTE Are all of these necessary?
{ PSF(brokenLimbs), 8 }, //up to 8 limbs at once (not that that many are used)
{ PSF(zoomLocked), 1 },	// NOTENOTE Are all of these necessary?
{ PSF(zoomFov), 0 },	// NOTENOTE Are all of these necessary?
{ PSF(fd.forceRageRecoveryTime), 32 },
{ PSF(fallingToDeath), 32 },
{ PSF(fd.forceMindtrickTargetIndex), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex2), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
//{ PSF(vehWeaponsLinked), 1 },//only used by vehicle?
{ PSF(lastHitLoc[2]), 0 },
//{ PSF(hyperSpaceTime), 32 },//only used by vehicle?
{ PSF(fd.forceMindtrickTargetIndex3), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(lastHitLoc[0]), 0 },
{ PSF(eFlags2), 10 },
{ PSF(fd.forceMindtrickTargetIndex4), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
//{ PSF(hyperSpaceAngles[1]), 0 },//only used by vehicle?
{ PSF(lastHitLoc[1]), 0 }, //currently only used so client knows to orient disruptor disintegration.. seems a bit much for just that though.
//{ PSF(vehBoarding), 1 }, //only used by vehicle? not like the normal boarding value, this is a simple "1 or 0" value
{ PSF(fd.sentryDeployed), 1 },
{ PSF(saberLockTime), 32 },
{ PSF(saberLockFrame), 16 },
//{ PSF(vehTurnaroundIndex), GENTITYNUM_BITS },//only used by vehicle?
//{ PSF(vehSurfaces), 16 }, //only used by vehicle? allow up to 16 surfaces in the flag I guess
{ PSF(fd.forcePowerLevel[FP_SEE]), 2 }, //needed for knowing when to display players through walls
{ PSF(saberLockEnemy), GENTITYNUM_BITS },
{ PSF(fd.forceGripCripple), 1 }, //should only be 0 or 1 ever
{ PSF(emplacedIndex), GENTITYNUM_BITS },
{ PSF(holocronBits), 32 },
{ PSF(isJediMaster), 1 },
{ PSF(forceRestricted), 1 },
{ PSF(trueJedi), 1 },
{ PSF(trueNonJedi), 1 },
{ PSF(duelTime), 32 },
{ PSF(duelInProgress), 1 },
{ PSF(saberLockAdvance), 1 },
{ PSF(heldByClient), 6 },
{ PSF(ragAttach), GENTITYNUM_BITS },
{ PSF(iModelScale), 10 }, //0-1024 (guess it's gotta be increased if we want larger allowable scale.. but 1024% is pretty big)
{ PSF(hackingBaseTime), 16 }, //up to 65536ms, over 10 seconds would just be silly anyway
//{ PSF(hyperSpaceAngles[0]), 0 },//only used by vehicle?
//{ PSF(hyperSpaceAngles[2]), 0 },//only used by vehicle?

//rww - for use by mod authors only
{ PSF(userInt1), 1 },
{ PSF(userInt2), 1 },
{ PSF(userInt3), 1 },
{ PSF(userFloat1), 1 },
{ PSF(userFloat2), 1 },
{ PSF(userFloat3), 1 },
{ PSF(userVec1[0]), 1 },
{ PSF(userVec1[1]), 1 },
{ PSF(userVec1[2]), 1 },
{ PSF(userVec2[0]), 1 },
{ PSF(userVec2[1]), 1 },
{ PSF(userVec2[2]), 1 }
};

static  netField_t	pilotPlayerStateFieldsJKA[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[1]), 0 },
{ PSF(origin[0]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(delta_angles[0]), 16 },
{ PSF(eFlags), 32 },
{ PSF(eventSequence), 16 },
{ PSF(rocketLockIndex), GENTITYNUM_BITS },
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(weaponstate), 4 },
{ PSF(pm_flags), 16 },
{ PSF(pm_time), -16 },
{ PSF(clientNum), GENTITYNUM_BITS },
{ PSF(weapon), 8 },
{ PSF(delta_angles[2]), 16 },
{ PSF(viewangles[2]), 0 },
{ PSF(externalEvent), 10 },
{ PSF(eventParms[1]), 8 },
{ PSF(pm_type), 8 },
{ PSF(externalEventParm), 8 },
{ PSF(eventParms[0]), -16 },
{ PSF(weaponChargeSubtractTime), 32 }, //? really need 32 bits??
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
{ PSF(rocketTargetTime), 32 },
{ PSF(fd.forceJumpZStart), 0 },
{ PSF(rocketLockTime), 32 },
{ PSF(m_iVehicleNum), GENTITYNUM_BITS }, // 10 bits fits all possible entity nums (2^10 = 1024). - AReis
{ PSF(generic1), 8 },//used by passengers
{ PSF(eFlags2), 10 },

//===THESE SHOULD NOT BE CHANGING OFTEN====================================================================
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(torsoAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(torsoTimer), 16 },
{ PSF(legsTimer), 16 },
{ PSF(jetpackFuel), 8 },
{ PSF(cloakFuel), 8 },
{ PSF(saberCanThrow), 1 },
{ PSF(fd.forcePowerDebounce[FP_LEVITATION]), 32 },
{ PSF(torsoFlip), 1 },
{ PSF(legsFlip), 1 },
{ PSF(fd.forcePowersActive), 32 },
{ PSF(hasDetPackPlanted), 1 },
{ PSF(fd.forceRageRecoveryTime), 32 },
{ PSF(saberInFlight), 1 },
{ PSF(fd.forceMindtrickTargetIndex), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex2), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex3), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex4), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.sentryDeployed), 1 },
{ PSF(fd.forcePowerLevel[FP_SEE]), 2 }, //needed for knowing when to display players through walls
{ PSF(holocronBits), 32 },
{ PSF(fd.forcePower), 8 },

//===THE REST OF THESE SHOULD NOT BE RELEVANT, BUT, FOR SAFETY, INCLUDE THEM ANYWAY, JUST AT THE BOTTOM===============================================================
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(speedJKA), 0 }, //sadly, the vehicles require negative speed values, so..
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(viewheight), -8 },
{ PSF(fd.saberAnimLevel), 4 },
{ PSF(fd.saberDrawAnimLevel), 4 },
{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(customRGBA[0]), 8 }, //0-255
{ PSF(movementDir), 4 },
{ PSF(saberEntityNum), GENTITYNUM_BITS }, //Also used for channel tracker storage, but should never exceed entity number
{ PSF(customRGBA[3]), 8 }, //0-255
{ PSF(saberMove), 32 }, //This value sometimes exceeds the max LS_ value and gets set to a crazy amount, so it needs 32 bits
{ PSF(standheight), 10 },
{ PSF(crouchheight), 10 },
{ PSF(basespeed), -16 },
{ PSF(customRGBA[1]), 8 }, //0-255
{ PSF(duelIndex), GENTITYNUM_BITS },
{ PSF(customRGBA[2]), 8 }, //0-255
{ PSF(gravity), 16 },
{ PSF(fd.forcePowersKnown), 32 },
{ PSF(fd.forcePowerLevel[FP_LEVITATION]), 2 }, //unfortunately we need this for fall damage calculation (client needs to know the distance for the fall noise)
{ PSF(fd.forcePowerSelected), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damageCount), 8 },
{ PSF(inAirAnim), 1 }, //just transmit it for the sake of knowing right when on the client to play a land anim, it's only 1 bit
{ PSF(fd.forceSide), 2 }, //so we know if we should apply greyed out shaders to dark/light force enlightenment
{ PSF(saberAttackChainCount), 4 },
{ PSF(lookTarget), GENTITYNUM_BITS },
{ PSF(moveDir[1]), 0 },
{ PSF(moveDir[0]), 0 },
{ PSF(damageEvent), 8 },
{ PSF(moveDir[2]), 0 },
{ PSF(activeForcePass), 6 },
{ PSF(electrifyTime), 32 },
{ PSF(damageType), 2 },
{ PSF(loopSound), 16 }, //rwwFIXMEFIXME: max sounds is 256, doesn't this only need to be 8?
{ PSF(hasLookTarget), 1 },
{ PSF(saberBlocked), 8 },
{ PSF(forceHandExtend), 8 },
{ PSF(saberHolsteredJKA), 2 },
{ PSF(damagePitch), 8 },
{ PSF(jumppad_ent), 10 },
{ PSF(forceDodgeAnim), 16 },
{ PSF(zoomMode), 2 }, // NOTENOTE Are all of these necessary?
{ PSF(hackingTime), 32 },
{ PSF(zoomTime), 32 },	// NOTENOTE Are all of these necessary?
{ PSF(brokenLimbs), 8 }, //up to 8 limbs at once (not that that many are used)
{ PSF(zoomLocked), 1 },	// NOTENOTE Are all of these necessary?
{ PSF(zoomFov), 0 },	// NOTENOTE Are all of these necessary?
{ PSF(fallingToDeath), 32 },
{ PSF(lastHitLoc[2]), 0 },
{ PSF(lastHitLoc[0]), 0 },
{ PSF(lastHitLoc[1]), 0 }, //currently only used so client knows to orient disruptor disintegration.. seems a bit much for just that though.
{ PSF(saberLockTime), 32 },
{ PSF(saberLockFrame), 16 },
{ PSF(saberLockEnemy), GENTITYNUM_BITS },
{ PSF(fd.forceGripCripple), 1 }, //should only be 0 or 1 ever
{ PSF(emplacedIndex), GENTITYNUM_BITS },
{ PSF(isJediMaster), 1 },
{ PSF(forceRestricted), 1 },
{ PSF(trueJedi), 1 },
{ PSF(trueNonJedi), 1 },
{ PSF(duelTime), 32 },
{ PSF(duelInProgress), 1 },
{ PSF(saberLockAdvance), 1 },
{ PSF(heldByClient), 6 },
{ PSF(ragAttach), GENTITYNUM_BITS },
{ PSF(iModelScale), 10 }, //0-1024 (guess it's gotta be increased if we want larger allowable scale.. but 1024% is pretty big)
{ PSF(hackingBaseTime), 16 }, //up to 65536ms, over 10 seconds would just be silly anyway
//===NEVER SEND THESE, ONLY USED BY VEHICLES==============================================================

//{ PSF(vehOrientation[0]), 0 },
//{ PSF(vehOrientation[1]), 0 },
//{ PSF(vehOrientation[2]), 0 },
//{ PSF(vehTurnaroundTime), 32 },//only used by vehicle?
//{ PSF(vehWeaponsLinked), 1 },//only used by vehicle?
//{ PSF(hyperSpaceTime), 32 },//only used by vehicle?
//{ PSF(vehTurnaroundIndex), GENTITYNUM_BITS },//only used by vehicle?
//{ PSF(vehSurfaces), 16 }, //only used by vehicle? allow up to 16 surfaces in the flag I guess
//{ PSF(vehBoarding), 1 }, //only used by vehicle? not like the normal boarding value, this is a simple "1 or 0" value
//{ PSF(hyperSpaceAngles[1]), 0 },//only used by vehicle?
//{ PSF(hyperSpaceAngles[0]), 0 },//only used by vehicle?
//{ PSF(hyperSpaceAngles[2]), 0 },//only used by vehicle?

//rww - for use by mod authors only
{ PSF(userInt1), 1 },
{ PSF(userInt2), 1 },
{ PSF(userInt3), 1 },
{ PSF(userFloat1), 1 },
{ PSF(userFloat2), 1 },
{ PSF(userFloat3), 1 },
{ PSF(userVec1[0]), 1 },
{ PSF(userVec1[1]), 1 },
{ PSF(userVec1[2]), 1 },
{ PSF(userVec2[0]), 1 },
{ PSF(userVec2[1]), 1 },
{ PSF(userVec2[2]), 1 }
};

static netField_t	vehPlayerStateFieldsJKA[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[1]), 0 },
{ PSF(origin[0]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(speedJKA), 0 }, //sadly, the vehicles require negative speed values, so..
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(delta_angles[0]), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(eFlags), 32 },
{ PSF(eventSequence), 16 },
{ PSF(legsTimer), 16 },
{ PSF(rocketLockIndex), GENTITYNUM_BITS },
//{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
//{ PSF(customRGBA[0]), 8 }, //0-255
//{ PSF(movementDir), 4 },
//{ PSF(customRGBA[3]), 8 }, //0-255
{ PSF(weaponstate), 4 },
//{ PSF(basespeed), -16 },
{ PSF(pm_flags), 16 },
{ PSF(pm_time), -16 },
//{ PSF(customRGBA[1]), 8 }, //0-255
{ PSF(clientNum), GENTITYNUM_BITS },
//{ PSF(duelIndex), GENTITYNUM_BITS },
//{ PSF(customRGBA[2]), 8 }, //0-255
{ PSF(gravity), 16 },
{ PSF(weapon), 8 },
{ PSF(delta_angles[2]), 16 },
{ PSF(viewangles[2]), 0 },
{ PSF(externalEvent), 10 },
{ PSF(eventParms[1]), 8 },
{ PSF(pm_type), 8 },
{ PSF(externalEventParm), 8 },
{ PSF(eventParms[0]), -16 },
{ PSF(vehOrientation[0]), 0 },
{ PSF(vehOrientation[1]), 0 },
{ PSF(moveDir[1]), 0 },
{ PSF(moveDir[0]), 0 },
{ PSF(vehOrientation[2]), 0 },
{ PSF(moveDir[2]), 0 },
{ PSF(rocketTargetTime), 32 },
//{ PSF(activeForcePass), 6 },//actually, you only need to know this for other vehicles, not your own
{ PSF(electrifyTime), 32 },
//{ PSF(fd.forceJumpZStart), 0 },//set on rider by vehicle, but not used by vehicle
{ PSF(loopSound), 16 }, //rwwFIXMEFIXME: max sounds is 256, doesn't this only need to be 8?
{ PSF(rocketLockTime), 32 },
{ PSF(m_iVehicleNum), GENTITYNUM_BITS }, // 10 bits fits all possible entity nums (2^10 = 1024). - AReis
{ PSF(vehTurnaroundTime), 32 },
//{ PSF(generic1), 8 },//used by passengers of vehicles, but not vehicles themselves
{ PSF(hackingTime), 32 },
{ PSF(brokenLimbs), 8 }, //up to 8 limbs at once (not that that many are used)
{ PSF(vehWeaponsLinked), 1 },
{ PSF(hyperSpaceTime), 32 },
{ PSF(eFlags2), 10 },
{ PSF(hyperSpaceAngles[1]), 0 },
{ PSF(vehBoarding), 1 }, //not like the normal boarding value, this is a simple "1 or 0" value
{ PSF(vehTurnaroundIndex), GENTITYNUM_BITS },
{ PSF(vehSurfaces), 16 }, //allow up to 16 surfaces in the flag I guess
{ PSF(hyperSpaceAngles[0]), 0 },
{ PSF(hyperSpaceAngles[2]), 0 },

//rww - for use by mod authors only
{ PSF(userInt1), 1 },
{ PSF(userInt2), 1 },
{ PSF(userInt3), 1 },
{ PSF(userFloat1), 1 },
{ PSF(userFloat2), 1 },
{ PSF(userFloat3), 1 },
{ PSF(userVec1[0]), 1 },
{ PSF(userVec1[1]), 1 },
{ PSF(userVec1[2]), 1 },
{ PSF(userVec2[0]), 1 },
{ PSF(userVec2[1]), 1 },
{ PSF(userVec2[2]), 1 }
};

//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#else//_OPTIMIZED_VEHICLE_NETWORKING
//The unoptimized way, throw *all* the vehicle stuff into the playerState along with everything else... :(
//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================

static  netField_t	playerStateFieldsJKA[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[1]), 0 },
{ PSF(origin[0]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(weaponTime), -16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(speedJKA), 0 }, //sadly, the vehicles require negative speed values, so..
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(delta_angles[0]), 16 },
{ PSF(torsoAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(eFlags), 32 },
{ PSF(fd.forcePower), 8 },
{ PSF(eventSequence), 16 },
{ PSF(torsoTimer), 16 },
{ PSF(legsTimer), 16 },
{ PSF(viewheight), -8 },
{ PSF(fd.saberAnimLevel), 4 },
{ PSF(rocketLockIndex), GENTITYNUM_BITS },
{ PSF(fd.saberDrawAnimLevel), 4 },
{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(customRGBA[0]), 8 }, //0-255
{ PSF(movementDir), 4 },
{ PSF(saberEntityNum), GENTITYNUM_BITS }, //Also used for channel tracker storage, but should never exceed entity number
{ PSF(customRGBA[3]), 8 }, //0-255
{ PSF(weaponstate), 4 },
{ PSF(saberMove), 32 }, //This value sometimes exceeds the max LS_ value and gets set to a crazy amount, so it needs 32 bits
{ PSF(standheight), 10 },
{ PSF(crouchheight), 10 },
{ PSF(basespeed), -16 },
{ PSF(pm_flags), 16 },
{ PSF(jetpackFuel), 8 },
{ PSF(cloakFuel), 8 },
{ PSF(pm_time), -16 },
{ PSF(customRGBA[1]), 8 }, //0-255
{ PSF(clientNum), GENTITYNUM_BITS },
{ PSF(duelIndex), GENTITYNUM_BITS },
{ PSF(customRGBA[2]), 8 }, //0-255
{ PSF(gravity), 16 },
{ PSF(weapon), 8 },
{ PSF(delta_angles[2]), 16 },
{ PSF(saberCanThrow), 1 },
{ PSF(viewangles[2]), 0 },
{ PSF(fd.forcePowersKnown), 32 },
{ PSF(fd.forcePowerLevel[FP_LEVITATION]), 2 }, //unfortunately we need this for fall damage calculation (client needs to know the distance for the fall noise)
{ PSF(fd.forcePowerDebounce[FP_LEVITATION]), 32 },
{ PSF(fd.forcePowerSelected), 8 },
{ PSF(torsoFlip), 1 },
{ PSF(externalEvent), 10 },
{ PSF(damageYaw), 8 },
{ PSF(damageCount), 8 },
{ PSF(inAirAnim), 1 }, //just transmit it for the sake of knowing right when on the client to play a land anim, it's only 1 bit
{ PSF(eventParms[1]), 8 },
{ PSF(fd.forceSide), 2 }, //so we know if we should apply greyed out shaders to dark/light force enlightenment
{ PSF(saberAttackChainCount), 4 },
{ PSF(pm_type), 8 },
{ PSF(externalEventParm), 8 },
{ PSF(eventParms[0]), -16 },
{ PSF(lookTarget), GENTITYNUM_BITS },
{ PSF(vehOrientation[0]), 0 },
{ PSF(weaponChargeSubtractTime), 32 }, //? really need 32 bits??
{ PSF(vehOrientation[1]), 0 },
{ PSF(moveDir[1]), 0 },
{ PSF(moveDir[0]), 0 },
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
{ PSF(vehOrientation[2]), 0 },
{ PSF(legsFlip), 1 },
{ PSF(damageEvent), 8 },
{ PSF(moveDir[2]), 0 },
{ PSF(rocketTargetTime), 32 },
{ PSF(activeForcePass), 6 },
{ PSF(electrifyTime), 32 },
{ PSF(fd.forceJumpZStart), 0 },
{ PSF(loopSound), 16 }, //rwwFIXMEFIXME: max sounds is 256, doesn't this only need to be 8?
{ PSF(hasLookTarget), 1 },
{ PSF(saberBlocked), 8 },
{ PSF(damageType), 2 },
{ PSF(rocketLockTime), 32 },
{ PSF(forceHandExtend), 8 },
{ PSF(saberHolsteredJKA), 2 },
{ PSF(fd.forcePowersActive), 32 },
{ PSF(damagePitch), 8 },
{ PSF(m_iVehicleNum), GENTITYNUM_BITS }, // 10 bits fits all possible entity nums (2^10 = 1024). - AReis
{ PSF(vehTurnaroundTime), 32 },
{ PSF(generic1), 8 },
{ PSF(jumppad_ent), 10 },
{ PSF(hasDetPackPlanted), 1 },
{ PSF(saberInFlight), 1 },
{ PSF(forceDodgeAnim), 16 },
{ PSF(zoomMode), 2 }, // NOTENOTE Are all of these necessary?
{ PSF(hackingTime), 32 },
{ PSF(zoomTime), 32 },	// NOTENOTE Are all of these necessary?
{ PSF(brokenLimbs), 8 }, //up to 8 limbs at once (not that that many are used)
{ PSF(zoomLocked), 1 },	// NOTENOTE Are all of these necessary?
{ PSF(zoomFov), 0 },	// NOTENOTE Are all of these necessary?
{ PSF(fd.forceRageRecoveryTime), 32 },
{ PSF(fallingToDeath), 32 },
{ PSF(fd.forceMindtrickTargetIndex), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex2), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(vehWeaponsLinked), 1 },
{ PSF(lastHitLoc[2]), 0 },
{ PSF(hyperSpaceTime), 32 },
{ PSF(fd.forceMindtrickTargetIndex3), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(lastHitLoc[0]), 0 },
{ PSF(eFlags2), 10 },
{ PSF(fd.forceMindtrickTargetIndex4), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(hyperSpaceAngles[1]), 0 },
{ PSF(lastHitLoc[1]), 0 }, //currently only used so client knows to orient disruptor disintegration.. seems a bit much for just that though.
{ PSF(vehBoarding), 1 }, //not like the normal boarding value, this is a simple "1 or 0" value
{ PSF(fd.sentryDeployed), 1 },
{ PSF(saberLockTime), 32 },
{ PSF(saberLockFrame), 16 },
{ PSF(vehTurnaroundIndex), GENTITYNUM_BITS },
{ PSF(vehSurfaces), 16 }, //allow up to 16 surfaces in the flag I guess
{ PSF(fd.forcePowerLevel[FP_SEE]), 2 }, //needed for knowing when to display players through walls
{ PSF(saberLockEnemy), GENTITYNUM_BITS },
{ PSF(fd.forceGripCripple), 1 }, //should only be 0 or 1 ever
{ PSF(emplacedIndex), GENTITYNUM_BITS },
{ PSF(holocronBits), 32 },
{ PSF(isJediMaster), 1 },
{ PSF(forceRestricted), 1 },
{ PSF(trueJedi), 1 },
{ PSF(trueNonJedi), 1 },
{ PSF(duelTime), 32 },
{ PSF(duelInProgress), 1 },
{ PSF(saberLockAdvance), 1 },
{ PSF(heldByClient), 6 },
{ PSF(ragAttach), GENTITYNUM_BITS },
{ PSF(iModelScale), 10 }, //0-1024 (guess it's gotta be increased if we want larger allowable scale.. but 1024% is pretty big)
{ PSF(hackingBaseTime), 16 }, //up to 65536ms, over 10 seconds would just be silly anyway
{ PSF(hyperSpaceAngles[0]), 0 },
{ PSF(hyperSpaceAngles[2]), 0 },

//rww - for use by mod authors only
{ PSF(userInt1), 1 },
{ PSF(userInt2), 1 },
{ PSF(userInt3), 1 },
{ PSF(userFloat1), 1 },
{ PSF(userFloat2), 1 },
{ PSF(userFloat3), 1 },
{ PSF(userVec1[0]), 1 },
{ PSF(userVec1[1]), 1 },
{ PSF(userVec1[2]), 1 },
{ PSF(userVec2[0]), 1 },
{ PSF(userVec2[1]), 1 },
{ PSF(userVec2[2]), 1 }
};

//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#endif//_OPTIMIZED_VEHICLE_NETWORKING
//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================




static  netField_t	entityStateFieldsJKA[] = // Apply to both DM_25 and DM_26 from what I can tell
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(eType), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
// does this need to be 8 bits?
{ NETF(weapon), 8 },
{ NETF(apos.trType), 8 },
// changed from 12 to 16
{ NETF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
// suspicious
{ NETF(torsoAnim), 16 },		// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
// large use beyond GENTITYNUM_BITS - should use generic1 insead
{ NETF(genericenemyindex), 32 }, //Do not change to GENTITYNUM_BITS, used as a time offset for seeker
{ NETF(eFlags), 32 },
{ NETF(pos.trDuration), 32 },
// might be able to reduce
{ NETF(teamowner), 8 },
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(angles[2]), 0 },
{ NETF(angles[0]), 0 },
{ NETF(solid), 24 },
// flag states barely used - could be moved elsewhere
{ NETF(fireflag), 2 },
{ NETF(event), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
// used mostly for players and npcs - appears to be static / never changing
{ NETF(customRGBA[3]), 8 }, //0-255
// used mostly for players and npcs - appears to be static / never changing
{ NETF(customRGBA[0]), 8 }, //0-255
// only used in fx system (which rick did) and chunks
{ NETF(speed), 0 },
// why are npc's clientnum's that big?
{ NETF(clientNum), GENTITYNUM_BITS }, //with npc's clientnum can be > MAX_CLIENTS so use entnum bits now instead.
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trTime), 32 },
// used mostly for players and npcs - appears to be static / never changing
{ NETF(customRGBA[1]), 8 }, //0-255
// used mostly for players and npcs - appears to be static / never changing
{ NETF(customRGBA[2]), 8 }, //0-255
// multiple meanings
{ NETF(saberEntityNum), GENTITYNUM_BITS },
// could probably just eliminate and assume a big number
{ NETF(g2radius), 8 },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
// used all over the place
{ NETF(owner), GENTITYNUM_BITS },
{ NETF(modelindex2), 8 },
// why was this changed from 0 to 8 ?
{ NETF(eventParm), 8 },
// unknown about size?
{ NETF(saberMove), 8 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(boneAngles1[1]), 0 },
// why raised from 8 to -16?
{ NETF(modelindex), -16 },
// barely used, could probably be replaced
{ NETF(emplacedOwner), 32 }, //As above, also used as a time value (for electricity render time)
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[2]), 0 },
// shouldn't these be better off as flags?  otherwise, they may consume more bits this way
{ NETF(torsoFlip), 1 },
{ NETF(angles2[1]), 0 },
// used mostly in saber and npc
{ NETF(lookTarget), GENTITYNUM_BITS },
{ NETF(origin2[2]), 0 },
// randomly used, not sure why this was used instead of svc_noclient
//	if (cent->currentState.modelGhoul2 == 127)
//	{ //not ready to be drawn or initialized..
//		return;
//	}
{ NETF(modelGhoul2), 8 },
{ NETF(loopSound), 8 },
{ NETF(origin2[0]), 0 },
// multiple purpose bit flag
{ NETF(shouldtarget), 1 },
// widely used, does not appear that they have to be 16 bits
{ NETF(trickedentindex), 16 }, //See note in PSF
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(origin2[1]), 0 },
{ NETF(time2), 32 },
{ NETF(legsFlip), 1 },
// fully used
{ NETF(bolt2), GENTITYNUM_BITS },
{ NETF(constantLight), 32 },
{ NETF(time), 32 },
// why doesn't lookTarget just indicate this?
{ NETF(hasLookTarget), 1 },
{ NETF(boneAngles1[2]), 0 },
// used for both force pass and an emplaced gun - gun is just a flag indicator
{ NETF(activeForcePass), 6 },
// used to indicate health
{ NETF(health), 10 }, //if something's health exceeds 1024, then.. too bad!
// appears to have multiple means, could be eliminated by indicating a sound set differently
{ NETF(loopIsSoundset), 1 },
{ NETF(saberHolstered), 2 },
//NPC-SPECIFIC:
// both are used for NPCs sabers, though limited
{ NETF(npcSaber1), 9 },
{ NETF(maxhealth), 10 },
{ NETF(trickedentindex2), 16 },
// appear to only be 18 powers?
{ NETF(forcePowersActive), 32 },
// used, doesn't appear to be flexible
{ NETF(iModelScale), 10 }, //0-1024 (guess it's gotta be increased if we want larger allowable scale.. but 1024% is pretty big)
// full bits used
{ NETF(powerups), 16 },
// can this be reduced?
{ NETF(soundSetIndex), 8 }, //rww - if MAX_AMBIENT_SETS is changed from 256, REMEMBER TO CHANGE THIS
// looks like this can be reduced to 4? (ship parts = 4, people parts = 2)
{ NETF(brokenLimbs), 8 }, //up to 8 limbs at once (not that that many are used)
{ NETF(csSounds_Std), 8 }, //soundindex must be 8 unless max sounds is changed
// used extensively
{ NETF(saberInFlight), 1 },
{ NETF(angles2[0]), 0 },
{ NETF(frame), 16 },
{ NETF(angles2[2]), 0 },
// why not use torsoAnim and set a flag to do the same thing as forceFrame (saberLockFrame)
{ NETF(forceFrame), 16 }, //if you have over 65536 frames, then this will explode. Of course if you have that many things then lots of things will probably explode.
{ NETF(generic1), 8 },
// do we really need 4 indexes?
{ NETF(boneIndex1), 6 }, //up to 64 bones can be accessed by this indexing method
// only 54 classes, could cut down 2 bits
{ NETF(NPC_class), 8 },
{ NETF(apos.trDuration), 32 },
// there appears to be only 2 different version of parms passed - a flag would better be suited
{ NETF(boneOrient), 9 }, //3 bits per orientation dir
// this looks to be a single bit flag
{ NETF(bolt1), 8 },
{ NETF(trickedentindex3), 16 },
// in use for vehicles
{ NETF(m_iVehicleNum), GENTITYNUM_BITS }, // 10 bits fits all possible entity nums (2^10 = 1024). - AReis
{ NETF(trickedentindex4), 16 },
// but why is there an opposite state of surfaces field?
{ NETF(surfacesOff), 32 },
{ NETF(eFlags2), 10 },
// should be bit field
{ NETF(isJediMaster), 1 },
// should be bit field
{ NETF(isPortalEnt), 1 },
// possible multiple definitions
{ NETF(heldByClient), 6 },
// this does not appear to be used in any production or non-cheat fashion - REMOVE
{ NETF(ragAttach), GENTITYNUM_BITS },
// used only in one spot for seige
{ NETF(boltToPlayer), 6 },
{ NETF(npcSaber2), 9 },
{ NETF(csSounds_Combat), 8 },
{ NETF(csSounds_Extra), 8 },
{ NETF(csSounds_Jedi), 8 },
// used only for surfaces on NPCs
{ NETF(surfacesOn), 32 }, //allow up to 32 surfaces in the bitflag
{ NETF(boneIndex2), 6 },
{ NETF(boneIndex3), 6 },
{ NETF(boneIndex4), 6 },
{ NETF(boneAngles1[0]), 0 },
{ NETF(boneAngles2[0]), 0 },
{ NETF(boneAngles2[1]), 0 },
{ NETF(boneAngles2[2]), 0 },
{ NETF(boneAngles3[0]), 0 },
{ NETF(boneAngles3[1]), 0 },
{ NETF(boneAngles3[2]), 0 },
{ NETF(boneAngles4[0]), 0 },
{ NETF(boneAngles4[1]), 0 },
{ NETF(boneAngles4[2]), 0 },

//rww - for use by mod authors only
// These don't exist on Xbox btw:
{ NETF(userInt1), 1 },
{ NETF(userInt2), 1 },
{ NETF(userInt3), 1 },
{ NETF(userFloat1), 1 },
{ NETF(userFloat2), 1 },
{ NETF(userFloat3), 1 },
{ NETF(userVec1[0]), 1 },
{ NETF(userVec1[1]), 1 },
{ NETF(userVec1[2]), 1 },
{ NETF(userVec2[0]), 1 },
{ NETF(userVec2[1]), 1 },
{ NETF(userVec2[2]), 1 }
};





#endif