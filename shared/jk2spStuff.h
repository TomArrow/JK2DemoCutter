
#ifndef __JK2SPSTUFF_H__
#define __JK2SPSTUFF_H__

#include "anims.h"
#include "demoCut.h"



// be fragmented into multiple packets


#define EF_MISSILE_STICK_JK2SP		0x00000040	// missiles that stick to the wall.

static const int jk2spEntityTypeToGeneral[] {
	ET_GENERAL_GENERAL,
	ET_PLAYER_GENERAL,
	ET_ITEM_GENERAL,
	ET_MISSILE_GENERAL,
	ET_MOVER_GENERAL,
	ET_BEAM_GENERAL,
	ET_PORTAL_GENERAL,
	ET_SPEAKER_GENERAL,
	ET_PUSH_TRIGGER_GENERAL,
	ET_TELEPORT_TRIGGER_GENERAL,
	ET_INVISIBLE_GENERAL,
	ET_THINKER_GENERAL,
	ET_CLOUD_GENERAL, // dumb

	ET_EVENTS_GENERAL				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
};

typedef enum {
	ET_GENERAL_JK2SP,
	ET_PLAYER_JK2SP,
	ET_ITEM_JK2SP,
	ET_MISSILE_JK2SP,
	ET_MOVER_JK2SP,
	ET_BEAM_JK2SP,
	ET_PORTAL_JK2SP,
	ET_SPEAKER_JK2SP,
	ET_PUSH_TRIGGER_JK2SP,
	ET_TELEPORT_TRIGGER_JK2SP,
	ET_INVISIBLE_JK2SP,
	ET_THINKER_JK2SP,
	ET_CLOUD_JK2SP, // dumb

	ET_EVENTS_JK2SP				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityType_jk2sp_t;

#define MAX_CONFIGSTRINGS_JK2SP 1024
#define	MAX_CHARSKINS_JK2SP		64		// character skins
#define MAX_LIGHT_STYLES_JK2SP		64
#define	MAX_CLIENTS_JK2SP			1 // 128		// absolute limit
#define MAX_LOCATIONS_JK2SP		64
#define	MAX_MODELS_JK2SP			256		// these are sent over the net as 8 bits
#define	MAX_SOUNDS_JK2SP			256		// so they cannot be blindly increased

#define MAX_FORCES_JK2SP			96
#define	MAX_SUBMODELS_JK2SP		512		// nine bits

#define MAX_FX_JK2SP				128
#define MAX_WORLD_FX_JK2SP		4

#define	CS_MUSIC_JK2SP			2
#define	CS_MESSAGE_JK2SP			3		// from the map worldspawn's message field
#define	CS_ITEMS_JK2SP			4		// string of 0's and 1's that tell which items are present
#define CS_AMBIENT_SET_JK2SP		5		// ambient set information for the player

#define	CS_MODELS_JK2SP			10
#define	CS_SOUNDS_JK2SP			(CS_MODELS_JK2SP+MAX_MODELS_JK2SP)
#define CS_FORCES_JK2SP			(CS_SOUNDS_JK2SP+MAX_SOUNDS_JK2SP)
#define CS_PLAYERS_JK2SP			(CS_FORCES_JK2SP+MAX_FORCES_JK2SP)
#define	CS_LIGHT_STYLES_JK2SP		(CS_PLAYERS_JK2SP+MAX_CLIENTS_JK2SP)
#define CS_EFFECTS_JK2SP			(CS_LIGHT_STYLES_JK2SP + (MAX_LIGHT_STYLES_JK2SP*3))
/*
Ghoul2 Insert Start
*/
#define CS_CHARSKINS_JK2SP 		(CS_EFFECTS_JK2SP + MAX_FX_JK2SP)
/*
Ghoul2 Insert End
*/
#define CS_DYNAMIC_MUSIC_STATE_JK2SP	(CS_CHARSKINS_JK2SP + MAX_CHARSKINS_JK2SP)
#define CS_WORLD_FX_JK2SP				(CS_DYNAMIC_MUSIC_STATE_JK2SP + 1)	
#define CS_MAX_JK2SP					(CS_WORLD_FX_JK2SP + MAX_WORLD_FX_JK2SP)




// This is the bg_itemlist weirdo stuff implementation idk.
typedef enum
{
	ITM_NONE,

	ITM_SABER_PICKUP,
	ITM_BRYAR_PISTOL_PICKUP,
	ITM_BLASTER_PICKUP,
	ITM_DISRUPTOR_PICKUP,
	ITM_BOWCASTER_PICKUP,
	ITM_REPEATER_PICKUP,
	ITM_DEMP2_PICKUP,
	ITM_FLECHETTE_PICKUP,
	ITM_ROCKET_LAUNCHER_PICKUP,
	ITM_THERMAL_DET_PICKUP,
	ITM_TRIP_MINE_PICKUP,
	ITM_DET_PACK_PICKUP,
	ITM_STUN_BATON_PICKUP,

	ITM_BOT_LASER_PICKUP,
	ITM_EMPLACED_GUN_PICKUP,
	ITM_TURRET_PICKUP,
	ITM_MELEE,
	ITM_ATST_MAIN_PICKUP,
	ITM_ATST_SIDE_PICKUP,
	ITM_TIE_FIGHTER_PICKUP,
	ITM_RAPID_FIRE_CONC_PICKUP,
	ITM_BLASTER_PISTOL_PICKUP,

	ITM_AMMO_FORCE_PICKUP,
	ITM_AMMO_BLASTER_PICKUP,
	ITM_AMMO_POWERCELL_PICKUP,
	ITM_AMMO_METAL_BOLTS_PICKUP,
	ITM_AMMO_ROCKETS_PICKUP,
	ITM_AMMO_EMPLACED_PICKUP,
	ITM_AMMO_THERMAL_PICKUP,
	ITM_AMMO_TRIPMINE_PICKUP,
	ITM_AMMO_DETPACK_PICKUP,

	ITM_FORCE_HEAL_PICKUP,
	ITM_FORCE_LEVITATION_PICKUP,
	ITM_FORCE_SPEED_PICKUP,
	ITM_FORCE_PUSH_PICKUP,
	ITM_FORCE_PULL_PICKUP,
	ITM_FORCE_TELEPATHY_PICKUP,
	ITM_FORCE_GRIP_PICKUP,
	ITM_FORCE_LIGHTNING_PICKUP,
	ITM_FORCE_SABERTHROW_PICKUP,

	ITM_BATTERY_PICKUP,
	ITM_SEEKER_PICKUP,
	ITM_SHIELD_PICKUP,
	ITM_BACTA_PICKUP,
	ITM_DATAPAD_PICKUP,
	ITM_BINOCULARS_PICKUP,
	ITM_SENTRY_GUN_PICKUP,
	ITM_LA_GOGGLES_PICKUP,

	ITM_MEDPAK_PICKUP,
	ITM_SHIELD_SM_PICKUP,
	ITM_SHIELD_LRG_PICKUP,
	ITM_GOODIE_KEY_PICKUP,
	ITM_SECURITY_KEY_PICKUP,

	ITM_NUM_ITEMS
};

static const int jk2spItemListToGeneral[] = {
	ITEMLIST_NONE_GENERAL,

	ITEMLIST_WEAPON_SABER_GENERAL,
	ITEMLIST_WEAPON_BRYAR_PISTOL_GENERAL,
	ITEMLIST_WEAPON_BLASTER_GENERAL,
	ITEMLIST_WEAPON_DISRUPTOR_GENERAL,
	ITEMLIST_WEAPON_BOWCASTER_GENERAL,
	ITEMLIST_WEAPON_REPEATER_GENERAL,
	ITEMLIST_WEAPON_DEMP2_GENERAL,
	ITEMLIST_WEAPON_FLECHETTE_GENERAL,
	ITEMLIST_WEAPON_ROCKET_LAUNCHER_GENERAL,
	ITEMLIST_WEAPON_THERMAL_GENERAL,
	ITEMLIST_WEAPON_TRIP_MINE_GENERAL,
	ITEMLIST_WEAPON_DET_PACK_GENERAL,
	ITEMLIST_WEAPON_STUN_BATON_GENERAL,

	// Not sure about the changes, are they the same?
	ITEMLIST_WEAPON_BOTWELDER_GENERAL,
	ITEMLIST_WEAPON_EMPLACED_GENERAL, //ITEMLIST_WEAPON_EMPLACED_GUN_GENERAL
	ITEMLIST_WEAPON_TURRETWP_GENERAL,//ITEMLIST_WEAPON_TURRET_GENERAL,
	ITEMLIST_WEAPON_MELEE_GENERAL,
	ITEMLIST_WEAPON_ATST_MAIN_GENERAL,
	ITEMLIST_WEAPON_ATST_SIDE_GENERAL,
	ITEMLIST_WEAPON_TIE_FIGHTER_GENERAL,
	ITEMLIST_WEAPON_RAPID_CONCUSSION_GENERAL,
	ITEMLIST_WEAPON_BLASTER_PISTOL_GENERAL,

	ITEMLIST_AMMO_FORCE_GENERAL,
	ITEMLIST_AMMO_BLASTER_GENERAL,
	ITEMLIST_AMMO_POWERCELL_GENERAL,
	ITEMLIST_AMMO_METALLIC_BOLTS_GENERAL,
	ITEMLIST_AMMO_ROCKETS_GENERAL,
	ITEMLIST_AMMO_EMPLACED_GENERAL,
	ITEMLIST_AMMO_THERMAL_GENERAL,
	ITEMLIST_AMMO_TRIPMINE_GENERAL,
	ITEMLIST_AMMO_DETPACK_GENERAL,

	ITEMLIST_HOLOCRON_FORCE_HEAL_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_LEVITATION_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_SPEED_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_PUSH_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_PULL_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_TELEPATHY_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_GRIP_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_LIGHTNING_GENERAL,
	ITEMLIST_HOLOCRON_FORCE_SABERTHROW_GENERAL,

	ITEMLIST_ITEM_BATTERY_GENERAL,//ITEMLIST_ITEM_BATTERY_GENERAL,
	ITEMLIST_ITEM_SEEKER_GENERAL,
	ITEMLIST_ITEM_ENVIRO_GENERAL,
	ITEMLIST_ITEM_MEDPAC_GENERAL,//ITEMLIST_ITEM_BACTA_GENERAL,
	ITEMLIST_ITEM_DATAPAD_GENERAL,
	ITEMLIST_ITEM_BINOCULARS_GENERAL,
	ITEMLIST_ITEM_SENTRY_GUN_GENERAL,
	ITEMLIST_ITEM_BINOCULARS_GENERAL, //ITEMLIST_ITEM_LA_GOGGLES_GENERAL,

	ITEMLIST_ITEM_MEDPAK_INSTANT_GENERAL,
	ITEMLIST_ITEM_SHIELD_SM_INSTANT_GENERAL,
	ITEMLIST_ITEM_SHIELD_LRG_INSTANT_GENERAL,
	ITEMLIST_ITEM_GOODIE_KEY_GENERAL,
	ITEMLIST_ITEM_SECURITY_KEY_GENERAL,

	ITEMLIST_NUM_TOTAL_GENERAL
};


static const int jk2spModToGeneral[]{ // Not sure about these mappings...
	MOD_UNKNOWN_GENERAL,

	// weapons
	MOD_SABER_GENERAL,
	MOD_BRYAR_PISTOL_GENERAL,//MOD_BRYAR_GENERAL,
	MOD_BRYAR_PISTOL_ALT_GENERAL,//MOD_BRYAR_ALT_GENERAL,
	MOD_BLASTER_GENERAL,
	MOD_BLASTER_ALT_GENERAL,
	MOD_DISRUPTOR_GENERAL,
	MOD_SNIPER_GENERAL,
	MOD_BOWCASTER_GENERAL,
	MOD_BOWCASTER_ALT_GENERAL,
	MOD_REPEATER_GENERAL,
	MOD_REPEATER_ALT_GENERAL,
	MOD_DEMP2_GENERAL,
	MOD_DEMP2_ALT_GENERAL,
	MOD_FLECHETTE_GENERAL,
	MOD_FLECHETTE_ALT_SPLASH_GENERAL,//MOD_FLECHETTE_ALT_GENERAL,
	MOD_ROCKET_GENERAL,
	MOD_ROCKET_HOMING_GENERAL,//MOD_ROCKET_ALT_GENERAL,
	MOD_THERMAL_GENERAL,
	MOD_THERMAL_SPLASH_GENERAL,//MOD_THERMAL_ALT_GENERAL,
	MOD_DET_PACK_SPLASH_GENERAL,//MOD_DETPACK_GENERAL,
	MOD_TRIP_MINE_SPLASH_GENERAL,//MOD_LASERTRIP_GENERAL,
	MOD_TIMED_MINE_SPLASH_GENERAL,//MOD_LASERTRIP_ALT_GENERAL,
	MOD_MELEE_GENERAL,
	MOD_SEEKER_GENERAL,
	MOD_FORCE_GRIP_GENERAL,
	MOD_EMPLACED_GENERAL,

	// world / generic
	MOD_ELECTROCUTE_GENERAL,
	MOD_EXPLOSIVE_GENERAL,
	MOD_EXPLOSIVE_SPLASH_GENERAL,
	MOD_KNOCKOUT_GENERAL,
	MOD_ENERGY_GENERAL,
	MOD_ENERGY_SPLASH_GENERAL,
	MOD_WATER_GENERAL,
	MOD_SLIME_GENERAL,
	MOD_LAVA_GENERAL,
	MOD_CRUSH_GENERAL,
	MOD_IMPACT_GENERAL,
	MOD_FALLING_GENERAL,
	MOD_SUICIDE_GENERAL,
	MOD_TRIGGER_HURT_GENERAL,

	MOD_MAX_GENERAL,

};


static const int jk2spWeaponToGeneral[]{
	WP_NONE_GENERAL,

	// Player weapons
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
	WP_STUN_BATON_GENERAL,

	//NOTE: player can only have up to 16 weapons_GENERAL, anything after that is enemy only

	WP_MELEE_GENERAL,			// Any ol' melee attack

	// NPC enemy weapons

	WP_EMPLACED_GUN_GENERAL,

	WP_BOT_LASER_GENERAL,		// Probe droid	- Laser blast

	WP_TURRET_GENERAL,			// turret guns 

	WP_ATST_MAIN_GENERAL,
	WP_ATST_SIDE_GENERAL,

	WP_TIE_FIGHTER_GENERAL,

	WP_RAPID_FIRE_CONC_GENERAL,

	WP_BLASTER_PISTOL_GENERAL,	// apparently some enemy only version of the blaster

//	WP_CHAOTICA_GUARD_GUN_GENERAL,	//			- B/W version of scav rifle for Chaotica's guards.
//	WP_BOT_ROCKET_GENERAL,		// Hunter Seeker - Rocket projectile
//	WP_KLINGON_BLADE_GENERAL,
//	WP_IMPERIAL_BLADE_GENERAL,

//	WP_DESPERADO_GENERAL,			// special holo-weapon
//	WP_PALADIN_GENERAL,			// special holo-weapon

	//# #eol
	WP_NUM_WEAPONS_GENERAL
};

static const int jk2spEventsToGeneral[]{
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

	EV_JUMP_GENERAL,
	EV_ROLL_GENERAL,
	EV_WATER_TOUCH_GENERAL,	// foot touches
	EV_WATER_LEAVE_GENERAL,	// foot leaves
	EV_WATER_UNDER_GENERAL,	// head touches
	EV_WATER_CLEAR_GENERAL,	// head leaves
	EV_WATER_GURP1_GENERAL,	// need air 1
	EV_WATER_GURP2_GENERAL,	// need air 2
	EV_WATER_DROWN_GENERAL,	// drowned

	EV_ITEM_PICKUP_GENERAL,

	EV_NOAMMO_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_FIRE_WEAPON_GENERAL,
	EV_ALT_FIRE_GENERAL,
	EV_POWERUP_SEEKER_FIRE_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	EV_USE_GENERAL,

	EV_REPLICATOR_GENERAL,

	EV_BATTERIES_CHARGED_GENERAL,

	EV_GRENADE_BOUNCE_GENERAL,		// eventParm will be the soundindex
	EV_MISSILE_STICK_GENERAL,		// eventParm will be the soundindex

	EV_BMODEL_SOUND_GENERAL,
	EV_GENERAL_SOUND_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,		// no attenuation

	EV_ENTITY_FORCE_GENERAL,
	EV_AREA_FORCE_GENERAL,
	EV_GLOBAL_FORCE_GENERAL,
	EV_FORCE_STOP_GENERAL,

	EV_PLAY_EFFECT_GENERAL,
	EV_PLAY_MUZZLE_EFFECT_GENERAL,

	EV_TARGET_BEAM_DRAW_GENERAL,

	EV_DISRUPTOR_MAIN_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_SHOT_GENERAL,
	EV_DISRUPTOR_SNIPER_MISS_GENERAL,

	EV_DEMP2_ALT_IMPACT_GENERAL,

	EV_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,

	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,

	EV_DISINTEGRATION_GENERAL,

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

	EV_USE_ITEM_GENERAL,

	EV_USE_INV_BINOCULARS_GENERAL,
	EV_USE_INV_BACTA_GENERAL,
	EV_USE_INV_SEEKER_GENERAL,
	EV_USE_INV_LIGHTAMP_GOGGLES_GENERAL,
	EV_USE_INV_SENTRY_GENERAL,

	EV_USE_FORCE_GENERAL,

	EV_DRUGGED_GENERAL,		// hit by an interrogator

	EV_DEBUG_LINE_GENERAL,
};





const netField_t	entityStateFields_jk2sp[] =
{
{ NETF(eType), 8 },
{ NETF(eFlags), 32 },

{ NETF(pos.trType), 8 },
{ NETF(pos.trTime), 32 },
{ NETF(pos.trDuration), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },

{ NETF(apos.trType), 8 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },

{ NETF(time), 32 },
{ NETF(time2), 32 },

{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },

{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(origin2[2]), 0 },

{ NETF(angles[0]), 0 },
{ NETF(angles[1]), 0 },
{ NETF(angles[2]), 0 },

{ NETF(angles2[0]), 0 },
{ NETF(angles2[1]), 0 },
{ NETF(angles2[2]), 0 },

{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(groundEntityNum), GENTITYNUM_BITS },

{ NETF(constantLight), 32 },
{ NETF(loopSound), 16 },
{ NETF(modelindex), 9 },	//0 to 511
{ NETF(modelindex2), 8 },
{ NETF(modelindex3), 8 },
{ NETF(clientNum), 32 },
{ NETF(frame), 16 },

{ NETF(solid), 24 },

{ NETF(event), 10 },
{ NETF(eventParm), 16 },

{ NETF(powerups), 16 },
{ NETF(weapon), 8 },
{ NETF(legsAnim), 16 },
{ NETF(legsAnimTimer), 8 },
{ NETF(torsoAnim), 16 },
{ NETF(torsoAnimTimer), 8 },
{ NETF(scale), 8 },

{ NETF(saberInFlight), 4 },
{ NETF(saberActive), 4 },
{ NETF(vehicleModel), 32 },
/*
Ghoul2 Insert Start
*/
{ NETF(modelScale[0]), 0 },
{ NETF(modelScale[1]), 0 },
{ NETF(modelScale[2]), 0 },
{ NETF(radius), 16 },
{ NETF(boltInfo), 32 },
//{ NETF(ghoul2), 32 },


//loadsavecrash
//#if _DEBUG
//
//{ NETF(mins[0]), 0 },
//{ NETF(mins[1]), 0 },
//{ NETF(mins[2]), 0 },
//{ NETF(maxs[0]), 0 },
//{ NETF(maxs[1]), 0 },
//{ NETF(maxs[2]), 0 }
//#endif


};


static const netField_t	playerStateFields_jk2sp[] =
{
{ PSF(commandTime), 32 },
{ PSF(pm_type), 8 },
{ PSF(bobCycle), 8 },
{ PSF(pm_flags), 17 },
{ PSF(pm_time), -16 },
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
{ PSF(gravity), 16 },
{ PSF(leanofs), -8 },
{ PSF(friction), 16 },
{ PSF(speed), 16 },
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
//{ PSF(animationTimer), 16 },
{ PSF(legsAnim), 16 },
{ PSF(torsoAnim), 16 },
{ PSF(movementDir), 4 },
{ PSF(eFlags), 32 },
{ PSF(eventSequence), 16 },
{ PSF(events[0]), 8 },
{ PSF(events[1]), 8 },
{ PSF(eventParms[0]), -9 },
{ PSF(eventParms[1]), -9 },
{ PSF(externalEvent), 8 },
{ PSF(externalEventParm), 8 },
{ PSF(clientNum), 32 },
{ PSF(weapon), 5 },
{ PSF(weaponstate),	  4 },
{ PSF(batteryCharge),	16 },
{ PSF(viewangles[0]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[2]), 0 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), -8 },
{ PSF(damageCount), 8 },
{ PSF(saberColor), 8 },
{ PSF(saberActive), 8 },
{ PSF(saberLength), 32 },
{ PSF(saberLengthMax), 32 },
{ PSF(forcePowersActive), 32},
{ PSF(saberInFlight), 8 },
{ PSF(vehicleModel), 32 },
{ PSF(viewEntity), 32 },
{ PSF(serverViewOrg[0]), 0 },
{ PSF(serverViewOrg[1]), 0 },
{ PSF(serverViewOrg[2]), 0 },
};

#endif