#ifndef __OTHERGAMESTUFF2_H__
#define __OTHERGAMESTUFF2_H__

#include "anims.h"
#include "demoCut.h"
#include "tsl/htrie_map.h"
#include <unordered_map>

#define ANIM_BITS 10

static const netField_t	playerStateFieldsET[] =
{
	{ PSF(commandTime), 32 },
	{ PSF(pm_type), 8 },
	{ PSF(bobCycle), 8 },
	{ PSF(pm_flags), 16 },
	{ PSF(pm_time), -16 },
	{ PSF(origin[0]), 0 },
	{ PSF(origin[1]), 0 },
	{ PSF(origin[2]), 0 },
	{ PSF(velocity[0]), 0 },
	{ PSF(velocity[1]), 0 },
	{ PSF(velocity[2]), 0 },
	{ PSF(weaponTime), -16 },
	{ PSF(weaponDelay), -16 },
	{ PSF(grenadeTimeLeft), -16 },
	{ PSF(gravity), 16 },
	{ PSF(leanf), 0 },
	{ PSF(speed), 16 },
	{ PSF(delta_angles[0]), 16 },
	{ PSF(delta_angles[1]), 16 },
	{ PSF(delta_angles[2]), 16 },
	{ PSF(groundEntityNum), GENTITYNUM_BITS },
	{ PSF(legsTimer), 16 },
	{ PSF(torsoTimer), 16 },
	{ PSF(legsAnim), ANIM_BITS },
	{ PSF(torsoAnim), ANIM_BITS },
	{ PSF(movementDir), 8 },
	{ PSF(eFlags), 24 },
	{ PSF(eventSequence), 8 },
	{ PSF(events[0]), 8 },
	{ PSF(events[1]), 8 },
	{ PSF(events[2]), 8 },
	{ PSF(events[3]), 8 },
	{ PSF(eventParms[0]), 8 },
	{ PSF(eventParms[1]), 8 },
	{ PSF(eventParms[2]), 8 },
	{ PSF(eventParms[3]), 8 },
	{ PSF(clientNum), 8 },
	{ PSF(weapons[0]), 32 },
	{ PSF(weapons[1]), 32 },
	{ PSF(weapon), 7 }, // (SA) yup, even more
	{ PSF(weaponstate), 4 },
	{ PSF(weapAnim), 10 },
	{ PSF(viewangles[0]), 0 },
	{ PSF(viewangles[1]), 0 },
	{ PSF(viewangles[2]), 0 },
	{ PSF(viewheight), -8 },
	{ PSF(damageEvent), 8 },
	{ PSF(damageYaw), 8 },
	{ PSF(damagePitch), 8 },
	{ PSF(damageCount), 8 },
	{ PSF(mins[0]), 0 },
	{ PSF(mins[1]), 0 },
	{ PSF(mins[2]), 0 },
	{ PSF(maxs[0]), 0 },
	{ PSF(maxs[1]), 0 },
	{ PSF(maxs[2]), 0 },
	{ PSF(crouchMaxZ), 0 },
	{ PSF(crouchViewHeight), 0 },
	{ PSF(standViewHeight), 0 },
	{ PSF(deadViewHeight), 0 },
	{ PSF(runSpeedScale), 0 },
	{ PSF(sprintSpeedScale), 0 },
	{ PSF(crouchSpeedScale), 0 },
	{ PSF(friction), 0 },
	{ PSF(viewlocked), 8 },
	{ PSF(viewlocked_entNum), 16 },
	{ PSF(nextWeapon), 8 },
	{ PSF(teamNum), 8 },
	//{ PSF(gunfx), 8},
	{ PSF(onFireStart), 32},
	{ PSF(curWeapHeat), 8 },
	{ PSF(aimSpreadScale), 8},
	{ PSF(serverCursorHint), 8}, //----(SA)	added
	{ PSF(serverCursorHintVal), 8}, //----(SA)	added
	{ PSF(classWeaponTime), 32}, // JPW NERVE
	{ PSF(identifyClient), 8 },
	{ PSF(identifyClientHealth), 8 },
	{ PSF(aiStateET), 2},
};

static const netField_t	entityStateFieldsET[] =
{
	{ NETF(eType), 8 },
	{ NETF(eFlags), 24 },
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
	{ NETF(loopSound), 8 },
	{ NETF(constantLight), 32 },
	{ NETF(dl_intensity), 32 }, //----(SA)	longer now to carry the corona colors
	{ NETF(modelindex), 9 },
	{ NETF(modelindex2), 9 },
	{ NETF(frame), 16 },
	{ NETF(clientNum), 8 },
	{ NETF(solid), 24 },
	{ NETF(event), 10 },
	{ NETF(eventParm), 8 },
	{ NETF(eventSequence), 8 }, // warning: need to modify cg_event.c at "// check the sequencial list" if you change this
	{ NETF(events[0]), 8 },
	{ NETF(events[1]), 8 },
	{ NETF(events[2]), 8 },
	{ NETF(events[3]), 8 },
	{ NETF(eventParms[0]), 8 },
	{ NETF(eventParms[1]), 8 },
	{ NETF(eventParms[2]), 8 },
	{ NETF(eventParms[3]), 8 },
	{ NETF(powerups), 16 },
	{ NETF(weapon), 8 },
	{ NETF(legsAnim), ANIM_BITS },
	{ NETF(torsoAnim), ANIM_BITS },
	{ NETF(density), 10},
	{ NETF(dmgFlags), 32 },   //----(SA)	additional info flags for damage
	{ NETF(onFireStart), 32},
	{ NETF(onFireEnd), 32},
	{ NETF(nextWeapon), 8},
	{ NETF(teamNum), 8},
	{ NETF(effect1Time), 32},
	{ NETF(effect2Time), 32},
	{ NETF(effect3Time), 32},
	{ NETF(animMovetype), 4},
	{ NETF(aiState), 2},
};

static const int etAnimsToGeneral[]{ // based on wolfcamql
	BOTH_DEATH1_GENERAL,
	BOTH_DEAD1_GENERAL,
	BOTH_DEAD1_WATER_GENERAL,
	BOTH_DEATH2_GENERAL,
	BOTH_DEAD2_GENERAL,
	BOTH_DEAD2_WATER_GENERAL,
	BOTH_DEATH3_GENERAL,
	BOTH_DEAD3_GENERAL,
	BOTH_DEAD3_WATER_GENERAL,

	BOTH_CLIMB_GENERAL,
/*10*/ BOTH_CLIMB_DOWN_GENERAL,
	BOTH_CLIMB_DISMOUNT_GENERAL,

	BOTH_SALUTE_GENERAL,

	BOTH_PAIN1_GENERAL,     // head
	BOTH_PAIN2_GENERAL,     // chest
	BOTH_PAIN3_GENERAL,     // groin
	BOTH_PAIN4_GENERAL,     // right shoulder
	BOTH_PAIN5_GENERAL,     // left shoulder
	BOTH_PAIN6_GENERAL,     // right knee
	BOTH_PAIN7_GENERAL,     // left knee
/*20*/ BOTH_PAIN8_GENERAL,      // dazed

	BOTH_GRAB_GRENADE_GENERAL,

	BOTH_ATTACK1_GENERAL,
	BOTH_ATTACK2_GENERAL,
	BOTH_ATTACK3_GENERAL,
	BOTH_ATTACK4_GENERAL,
	BOTH_ATTACK5_GENERAL,

	BOTH_EXTRA1_GENERAL,
	BOTH_EXTRA2_GENERAL,
	BOTH_EXTRA3_GENERAL,
/*30*/ BOTH_EXTRA4_GENERAL,
	BOTH_EXTRA5_GENERAL,
	BOTH_EXTRA6_GENERAL,
	BOTH_EXTRA7_GENERAL,
	BOTH_EXTRA8_GENERAL,
	BOTH_EXTRA9_GENERAL,
	BOTH_EXTRA10_GENERAL,
	BOTH_EXTRA11_GENERAL,
	BOTH_EXTRA12_GENERAL,
	BOTH_EXTRA13_GENERAL,
/*40*/ BOTH_EXTRA14_GENERAL,
	BOTH_EXTRA15_GENERAL,
	BOTH_EXTRA16_GENERAL,
	BOTH_EXTRA17_GENERAL,
	BOTH_EXTRA18_GENERAL,
	BOTH_EXTRA19_GENERAL,
	BOTH_EXTRA20_GENERAL,

	TORSO_GESTURE_GENERAL,
	TORSO_GESTURE2_GENERAL,
	TORSO_GESTURE3_GENERAL,
/*50*/ TORSO_GESTURE4_GENERAL,

	TORSO_DROP_GENERAL,

	TORSO_RAISE_GENERAL,    // (low)
	TORSO_ATTACK_GENERAL,
	TORSO_STAND_GENERAL,
	TORSO_STAND_ALT1_GENERAL,
	TORSO_STAND_ALT2_GENERAL,
	TORSO_READY_GENERAL,
	TORSO_RELAX_GENERAL,

	TORSO_RAISE2_GENERAL,   // (high)
/*60*/ TORSO_ATTACK2_GENERAL,
	TORSO_STAND2_GENERAL,
	TORSO_STAND2_ALT1_GENERAL,
	TORSO_STAND2_ALT2_GENERAL,
	TORSO_READY2_GENERAL,
	TORSO_RELAX2_GENERAL,

	TORSO_RAISE3_GENERAL,   // (pistol)
	TORSO_ATTACK3_GENERAL,
	TORSO_STAND3_GENERAL,
	TORSO_STAND3_ALT1_GENERAL,
/*70*/ TORSO_STAND3_ALT2_GENERAL,
	TORSO_READY3_GENERAL,
	TORSO_RELAX3_GENERAL,

	TORSO_RAISE4_GENERAL,   // (shoulder)
	TORSO_ATTACK4_GENERAL,
	TORSO_STAND4_GENERAL,
	TORSO_STAND4_ALT1_GENERAL,
	TORSO_STAND4_ALT2_GENERAL,
	TORSO_READY4_GENERAL,
	TORSO_RELAX4_GENERAL,

/*80*/ TORSO_RAISE5_GENERAL,    // (throw)
	TORSO_ATTACK5_GENERAL,
	TORSO_ATTACK5B_GENERAL,
	TORSO_STAND5_GENERAL,
	TORSO_STAND5_ALT1_GENERAL,
	TORSO_STAND5_ALT2_GENERAL,
	TORSO_READY5_GENERAL,
	TORSO_RELAX5_GENERAL,

	TORSO_RELOAD1_GENERAL,  // (low)
	TORSO_RELOAD2_GENERAL,  // (high)
/*90*/ TORSO_RELOAD3_GENERAL,   // (pistol)
	TORSO_RELOAD4_GENERAL,  // (shoulder)

	TORSO_MG42_GENERAL,     // firing tripod mounted weapon animation

	TORSO_MOVE_GENERAL,     // torso anim to play while moving and not firing (swinging arms type thing)
	TORSO_MOVE_ALT_GENERAL,

	TORSO_EXTRA_GENERAL,
	TORSO_EXTRA2_GENERAL,
	TORSO_EXTRA3_GENERAL,
	TORSO_EXTRA4_GENERAL,
	TORSO_EXTRA5_GENERAL,
/*100*/ TORSO_EXTRA6_GENERAL,
	TORSO_EXTRA7_GENERAL,
	TORSO_EXTRA8_GENERAL,
	TORSO_EXTRA9_GENERAL,
	TORSO_EXTRA10_GENERAL,

	LEGS_WALKCR_GENERAL,
	LEGS_WALKCR_BACK_GENERAL,
	LEGS_WALK_GENERAL,
	LEGS_RUN_GENERAL,
	LEGS_BACK_GENERAL,
/*110*/ LEGS_SWIM_GENERAL,
	LEGS_SWIM_IDLE_GENERAL,

	LEGS_JUMP_GENERAL,
	LEGS_JUMPB_GENERAL,
	LEGS_LAND_GENERAL,

	LEGS_IDLE_GENERAL,
	LEGS_IDLE_ALT_GENERAL, // LEGS_IDLE2
	LEGS_IDLECR_GENERAL,

	LEGS_TURN_GENERAL,

	LEGS_BOOT_GENERAL,      // kicking animation

/*120*/ LEGS_EXTRA1_GENERAL,
	LEGS_EXTRA2_GENERAL,
	LEGS_EXTRA3_GENERAL,
	LEGS_EXTRA4_GENERAL,
	LEGS_EXTRA5_GENERAL,
	LEGS_EXTRA6_GENERAL,
	LEGS_EXTRA7_GENERAL,
	LEGS_EXTRA8_GENERAL,
	LEGS_EXTRA9_GENERAL,
	LEGS_EXTRA10_GENERAL,

/*130*/ MAX_ANIMATIONS_GENERAL
};

const static int etEventToGeneralMap[] = {
	EV_NONE_GENERAL,
	EV_FOOTSTEP_GENERAL,
	EV_FOOTSTEP_METAL_GENERAL,
	EV_FOOTSTEP_WOOD_GENERAL,
	EV_FOOTSTEP_GRASS_GENERAL,
	EV_FOOTSTEP_GRAVEL_GENERAL,
	EV_FOOTSTEP_ROOF_GENERAL,
	EV_FOOTSTEP_SNOW_GENERAL,
	EV_FOOTSTEP_CARPET_GENERAL,
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
	EV_FALL_NDIE_GENERAL,
	EV_FALL_DMG_10_GENERAL,
	EV_FALL_DMG_15_GENERAL,
	EV_FALL_DMG_25_GENERAL,
	EV_FALL_DMG_50_GENERAL,
	EV_JUMP_GENERAL,
	EV_WATER_TOUCH_GENERAL, // foot touches
	EV_WATER_LEAVE_GENERAL, // foot leaves
	EV_WATER_UNDER_GENERAL, // head touches
	EV_WATER_CLEAR_GENERAL, // head leaves
	EV_ITEM_PICKUP_GENERAL,         // normal item pickups are predictable
	EV_ITEM_PICKUP_QUIET_GENERAL,   // (SA) same_GENERAL, but don't play the default pickup sound as it was specified in the ent
	EV_GLOBAL_ITEM_PICKUP_GENERAL,  // powerup / team sounds are broadcast to everyone
	EV_NOAMMO_GENERAL,
	EV_WEAPONSWITCHED_GENERAL,
	EV_EMPTYCLIP_GENERAL,
	EV_FILL_CLIP_GENERAL,
	EV_MG42_FIXED_GENERAL, // JPW NERVE
	EV_WEAP_OVERHEAT_GENERAL,
	EV_CHANGE_WEAPON_GENERAL,
	EV_CHANGE_WEAPON_2_GENERAL,
	EV_FIRE_WEAPON_GENERAL,
	EV_FIRE_WEAPONB_GENERAL,
	EV_FIRE_WEAPON_LASTSHOT_GENERAL,
	EV_NOFIRE_UNDERWATER_GENERAL,
	EV_FIRE_WEAPON_MG42_GENERAL,
	EV_FIRE_WEAPON_MOUNTEDMG42_GENERAL,
	EV_ITEM_RESPAWN_GENERAL,
	EV_ITEM_POP_GENERAL,
	EV_PLAYER_TELEPORT_IN_GENERAL,
	EV_PLAYER_TELEPORT_OUT_GENERAL,
	EV_GRENADE_BOUNCE_GENERAL,      // eventParm will be the soundindex
	EV_GENERAL_SOUND_GENERAL,
	EV_GENERAL_SOUND_VOLUME_GENERAL,
	EV_GLOBAL_SOUND_GENERAL,        // no attenuation
	EV_GLOBAL_CLIENT_SOUND_GENERAL, // DHM - Nerve :: no attenuation_GENERAL, only plays for specified client
	EV_GLOBAL_TEAM_SOUND_GENERAL,   // no attenuation_GENERAL, team only
	EV_FX_SOUND_GENERAL,
	EV_BULLET_HIT_FLESH_GENERAL,
	EV_BULLET_HIT_WALL_GENERAL,
	EV_MISSILE_HIT_GENERAL,
	EV_MISSILE_MISS_GENERAL,
	EV_RAILTRAIL_GENERAL,
	EV_VENOM_GENERAL,
	EV_BULLET_GENERAL,              // otherEntity is the shooter
	EV_LOSE_HAT_GENERAL,            //----(SA)
	EV_PAIN_GENERAL,
	EV_CROUCH_PAIN_GENERAL,
	EV_DEATH1_GENERAL,
	EV_DEATH2_GENERAL,
	EV_DEATH3_GENERAL,
	EV_OBITUARY_GENERAL,
	EV_STOPSTREAMINGSOUND_GENERAL, // JPW NERVE swiped from sherman
	EV_POWERUP_QUAD_GENERAL,
	EV_POWERUP_BATTLESUIT_GENERAL,
	EV_POWERUP_REGEN_GENERAL,
	EV_GIB_PLAYER_GENERAL,          // gib a previously living player
	EV_DEBUG_LINE_GENERAL,
	EV_STOPLOOPINGSOUND_GENERAL,
	EV_TAUNT_GENERAL,
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
	EV_DEBRIS_GENERAL,
	EV_ALERT_SPEAKER_GENERAL,
	EV_POPUPMESSAGE_GENERAL,
	EV_ARTYMESSAGE_GENERAL,
	EV_AIRSTRIKEMESSAGE_GENERAL,
	EV_MEDIC_CALL_GENERAL,
	EV_ENTITY_EVENT_COUNT_GENERAL,//EV_MAX_EVENTS_GENERAL   // just added as an 'endcap'
};





















// OpenMOHAA

typedef enum {
	TEAM_NONE_MOH,
	TEAM_SPECTATOR_MOH,
	TEAM_FREEFORALL_MOH,
	TEAM_ALLIES_MOH,
	TEAM_AXIS_MOH
} teamtypeMOH_t;

typedef enum {
	GT_SINGLE_PLAYER_MOH,	// single player
	GT_FFA_MOH,				// free for all
	GT_TEAM_MOH,			// team deathmatch
	GT_TEAM_ROUNDS_MOH,
	GT_OBJECTIVE_MOH,
	// Team Assault game mode
	GT_TOW_MOH,
	// Team Tactics game mode
	GT_LIBERATION_MOH,
	GT_MAX_GAME_TYPE_MOH
} gametypeMOH_t;



#define EF_ALLIES_MOH			0x00000080		// su44: this player is in allies team
#define EF_AXIS_MOH				0x00000100		// su44: this player is in axis team
#define EF_ANY_TEAM_MOH			(EF_ALLIES_MOH | EF_AXIS_MOH)

// entity subclass
#define EF_ENTITY_MOH				(1<<0)
#define EF_ANIMATE_MOH				(1<<1)
#define EF_SENTIENT_MOH				(1<<2)
#define EF_PLAYER_MOH				(1<<3)
#define EF_ACTOR_MOH				(1<<4)
#define EF_ITEM_MOH					(1<<5)
#define EF_INVENTORYITEM_MOH		(1<<6)
#define EF_WEAPON_MOH				(1<<7)
#define EF_PROJECTILE_MOH			(1<<8)
#define EF_DOOR_MOH					(1<<9)
#define EF_CAMERA_MOH				(1<<10)
#define EF_VEHICLE_MOH				(1<<11)
#define EF_VEHICLETANK_MOH			(1<<12)
#define EF_VEHICLETURRET_MOH		(1<<13)
#define EF_TURRET_MOH				(1<<14)
#define EF_PATHNODE_MOH				(1<<15)
#define EF_WAYPOINT_MOH				(1<<16)
#define EF_TEMPWAYPOINT_MOH			(1<<17)
#define EF_VEHICLEPOINT_MOH			(1<<18)
#define EF_SPLINEPATH_MOH			(1<<19)
#define EF_CRATEOBJECT_MOH			(1<<20)
#define EF_BOT_MOH					(1<<21)



#define	MAX_ACTIVEITEMS			8
#define	MAX_AMMO				16
#define	MAX_AMMO_AMOUNT			16
#define MAX_MAX_AMMO_AMOUNT		16

#define	MAX_CONFIGSTRINGS_MOH	2736

#define	ANIM_TOGGLEBIT_MOH		(1<<9)

#define	MAX_CLIENTS_MOH			64		// absolute limit
#define MAX_LOCATIONS_MOH		64
#define	MAX_MODELS_MOH			1024		// these are sent over the net as 8 bits
#define	MAX_SOUNDS_MOH			512		// so they cannot be blindly increased
#define MAX_OBJECTIVES_MOH		20
#define MAX_LIGHTSTYLES_MOH		32
#define MAX_WEAPONS_MOH			48

#define	CS_MESSAGE_MOH				2		// from the map worldspawn's message field
#define	CS_SAVENAME_MOH				3		// current save
#define	CS_MOTD_MOH					4		// g_motd string for server message of the day
#define	CS_WARMUP_MOH				5		// server time when the match will be restarted

// 2 values were removed in team tactics
#define	CS_MUSIC_MOH				6		// MUSIC_NewSoundtrack(cs)
#define CS_FOGINFO_MOH				7		// cg.farplane_cull cg.farplane_distance cg.farplane_color[3]
#define CS_SKYINFO_MOH				8		// cg.sky_alpha cg.sky_portal

#define	CS_GAME_VERSION_MOH			9
#define	CS_LEVEL_START_TIME_MOH		10		// so the timer only shows the current level cgs.levelStartTime

#define CS_CURRENT_OBJECTIVE_MOH	11

#define CS_RAIN_DENSITY_MOH			12		// cg.rain
#define CS_RAIN_SPEED_MOH			13
#define CS_RAIN_SPEEDVARY_MOH		14
#define CS_RAIN_SLANT_MOH			15
#define CS_RAIN_LENGTH_MOH			16
#define CS_RAIN_MINDIST_MOH			17
#define CS_RAIN_WIDTH_MOH			18
#define CS_RAIN_SHADER_MOH			19
#define CS_RAIN_NUMSHADERS_MOH		20

//
// Team tactics
//
#define CS_VOTE_TIME_MOH			21
#define CS_VOTE_STRING_MOH			22
#define CS_VOTE_YES_MOH				23
#define CS_VOTE_NO_MOH				24
#define CS_VOTE_UNDECIDED_MOH		25
//

#define CS_MATCHEND_MOH				26		// cgs.matchEndTime

#define	CS_MODELS_MOH				32
#define CS_OBJECTIVES_MOH			(CS_MODELS_MOH+MAX_MODELS_MOH) // 1056
#define	CS_SOUNDS_MOH				(CS_OBJECTIVES_MOH+MAX_OBJECTIVES_MOH) // 1076

#define CS_IMAGES_MOH				(CS_SOUNDS_MOH+MAX_SOUNDS_MOH) // 1588
#define MAX_IMAGES_MOH				64

#define CS_LIGHTSTYLES_MOH			(CS_IMAGES_MOH+MAX_IMAGES_MOH) //1652
#define CS_PLAYERS_MOH				(CS_LIGHTSTYLES_MOH+MAX_LIGHTSTYLES_MOH) // 1684

#define CS_WEAPONS_MOH				(CS_PLAYERS_MOH+MAX_CLIENTS_MOH) // su44 was here
#define CS_TEAMS_MOH				1892
#define CS_GENERAL_STRINGS_MOH		1893
#define CS_SPECTATORS_MOH			1894
#define CS_ALLIES_MOH				1895
#define CS_AXIS_MOH					1896
#define CS_SOUNDTRACK_MOH			1881

#define CS_TEAMINFO_MOH				1

#define CS_MAX_MOH					(CS_PARTICLES_MOH+MAX_LOCATIONS_MOH) // ? Why is CS_PARTICLES not defined in OpenMOHAA? Weird. I'll just carry this weirdness over.
#if (CS_MAX_MOH) > MAX_CONFIGSTRINGS_MOH
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

// These _OLD values are for protocol 6,7,8, above ones are for protocol 15,16,17
#define	CS_MUSIC_MOH_OLD				8		// MUSIC_NewSoundtrack(cs)
#define CS_FOGINFO_MOH_OLD				9		// cg.farplane_cull cg.farplane_distance cg.farplane_color[3]
#define CS_SKYINFO_MOH_OLD				10		// cg.sky_alpha cg.sky_portal

#define	CS_GAME_VERSION_MOH_OLD			11
#define	CS_LEVEL_START_TIME_MOH_OLD		12		// so the timer only shows the current level cgs.levelStartTime

#define CS_CURRENT_OBJECTIVE_MOH_OLD	13

#define CS_RAIN_DENSITY_MOH_OLD			14		// cg.rain
#define CS_RAIN_SPEED_MOH_OLD			15
#define CS_RAIN_SPEEDVARY_MOH_OLD		16
#define CS_RAIN_SLANT_MOH_OLD			17
#define CS_RAIN_LENGTH_MOH_OLD			18
#define CS_RAIN_MINDIST_MOH_OLD			19
#define CS_RAIN_WIDTH_MOH_OLD			20
#define CS_RAIN_SHADER_MOH_OLD			21
#define CS_RAIN_NUMSHADERS_MOH_OLD		22


#define	MINS_Z_MOH				0
#define  MINS_X_MOH               -15
#define  MINS_Y_MOH               -15
#define  MAXS_X_MOH               15
#define  MAXS_Y_MOH               15

#define	MINS_Z_MOH				   0		// IneQuation: bounding box and viewheights to match MoHAA
#define  MAXS_Z_MOH               96

#define DEAD_MINS_Z_MOH				32
#define CROUCH_MAXS_Z_MOH			49
#define DEFAULT_VIEWHEIGHT_MOH		82
#define CROUCH_RUN_VIEWHEIGHT_MOH	64
#define JUMP_START_VIEWHEIGHT_MOH	52
#define CROUCH_VIEWHEIGHT_MOH		48
#define PRONE_VIEWHEIGHT_MOH		16
#define DEAD_VIEWHEIGHT_MOH			8


// moh pm_flags
#define	PMF_DUCKED_MOH				(1<<0)
#define	PMF_VIEW_PRONE_MOH			(1<<1)
//
// 2 Flags removed in team tactics
//
#define PMF_SPECTATING_MOH			(1<<2)
#define	PMF_RESPAWNED_MOH			(1<<3)
#define	PMF_NO_PREDICTION_MOH		(1<<4)
#define	PMF_FROZEN_MOH				(1<<5)
#define	PMF_INTERMISSION_MOH		(1<<6)
#define PMF_SPECTATE_FOLLOW_MOH		(1<<7)
#define	PMF_CAMERA_VIEW_MOH			(1<<7)		// use camera view instead of ps view
#define	PMF_NO_MOVE_MOH				(1<<8)
#define PMF_VIEW_DUCK_RUN_MOH		(1<<9)
#define	PMF_VIEW_JUMP_START_MOH		(1<<10)
#define	PMF_LEVELEXIT_MOH			(1<<11)
#define	PMF_NO_GRAVITY_MOH			(1<<12)
#define	PMF_NO_HUD_MOH				(1<<13)
#define	PMF_UNKNOWN_MOH				(1<<14)
#define	PMF_NO_LEAN_MOH				(1<<15)


static inline uint32_t CPT_NormalizePlayerStateFlags_ver_6(uint32_t flags) {
	uint32_t normalizedFlags = 0;

	// Convert AA PlayerMove flags to SH/BT flags
	normalizedFlags |= flags & (1 << 0);
	for (size_t i = 2; i < 32; ++i)
	{
		if (flags & (1 << (i + 2))) {
			normalizedFlags |= (1 << i);
		}
	}

	// So that flags are normalized across modules
	return normalizedFlags;
}

static inline uint32_t CPT_DenormalizePlayerStateFlags_ver_6(uint32_t flags) {
	uint32_t normalizedFlags = 0;

	// Convert AA PlayerMove flags to SH/BT flags
	normalizedFlags |= flags & (1 << 0);
	for (size_t i = 2; i < 32; ++i)
	{
		if (flags & (1 << i)) {
			normalizedFlags |= (1 << (i + 2));
		}
	}

	// So that flags are normalized across modules
	return normalizedFlags;
}


static inline uint32_t CPT_NormalizePlayerStateFlags_ver_15(uint32_t flags) {
	return flags;
}

static inline uint32_t CPT_DenormalizePlayerStateFlags_ver_15(uint32_t flags) {
	return flags;
}

static inline  uint32_t MOH_CPT_NormalizePlayerStateFlags(uint32_t flags, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_6)
	{
		return CPT_NormalizePlayerStateFlags_ver_6(flags);
	}
	else
	{
		return CPT_NormalizePlayerStateFlags_ver_15(flags);
	}
}


#define MOH_WEAPON_CLASS_PISTOL			(1<<0)
#define MOH_WEAPON_CLASS_RIFLE			(1<<1)
#define MOH_WEAPON_CLASS_SMG			(1<<2)
#define MOH_WEAPON_CLASS_MG				(1<<3)
#define MOH_WEAPON_CLASS_GRENADE		(1<<4)
#define MOH_WEAPON_CLASS_HEAVY			(1<<5)
#define MOH_WEAPON_CLASS_CANNON			(1<<6)
#define MOH_WEAPON_CLASS_ITEM			(1<<7)
#define MOH_WEAPON_CLASS_ITEM1			(1<<8)
#define MOH_WEAPON_CLASS_ITEM2			(1<<9)
#define MOH_WEAPON_CLASS_ITEM3			(1<<10)
#define MOH_WEAPON_CLASS_ITEM4			(1<<11)
#define MOH_WEAPON_CLASS_ANY_ITEM		(MOH_WEAPON_CLASS_ITEM|MOH_WEAPON_CLASS_ITEM1|MOH_WEAPON_CLASS_ITEM2|MOH_WEAPON_CLASS_ITEM3|MOH_WEAPON_CLASS_ITEM4)

#define MOH_WEAPON_CLASS_PRIMARY		(!(MOH_WEAPON_CLASS_PISTOL|MOH_WEAPON_CLASS_GRENADE))
#define MOH_WEAPON_CLASS_SECONDARY		(MOH_WEAPON_CLASS_PISTOL|MOH_WEAPON_CLASS_GRENADE)
#define MOH_WEAPON_CLASS_THROWABLE		(MOH_WEAPON_CLASS_GRENADE|MOH_WEAPON_CLASS_ITEM)
#define MOH_WEAPON_CLASS_MISC			(MOH_WEAPON_CLASS_CANNON|MOH_WEAPON_CLASS_ITEM|MOH_WEAPON_CLASS_ITEM1|MOH_WEAPON_CLASS_ITEM2|MOH_WEAPON_CLASS_ITEM3|MOH_WEAPON_CLASS_ITEM4)
#define MOH_WEAPON_CLASS_ITEMINDEX		(MOH_WEAPON_CLASS_ITEM1|MOH_WEAPON_CLASS_ITEM2|MOH_WEAPON_CLASS_ITEM3|MOH_WEAPON_CLASS_ITEM4)


enum vmAnimMoh_e {
	VM_ANIM_INVALID_MOH = -1, // I added this for convenience. Doesn't exist in MOH
	VM_ANIM_DISABLED_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_SECONDARY_MOH,
	VM_ANIM_RECHAMBER_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RELOAD_SINGLE_MOH,
	VM_ANIM_RELOAD_END_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_LADDERSTEP_MOH,
	VM_ANIM_IDLE_0_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_MAX, // I added this for convenience. Doesn't exist in MOH
};



enum animPrefix_e {
	WPREFIX_NONE,
	WPREFIX_PAPERS,
	WPREFIX_COLT45,
	WPREFIX_P38,
	WPREFIX_HISTANDARD,
	WPREFIX_GARAND,
	WPREFIX_KAR98,
	WPREFIX_KAR98SNIPER,
	WPREFIX_SPRINGFIELD,
	WPREFIX_THOMPSON,
	WPREFIX_MP40,
	WPREFIX_BAR,
	WPREFIX_MP44,
	WPREFIX_FRAGGRENADE,
	WPREFIX_STIELHANDGRANATE,
	WPREFIX_BAZOOKA,
	WPREFIX_PANZERSCHRECK,
	WPREFIX_SHOTGUN,
	WPREFIX_UNARMED,
	//
	// Team Assault and Team Tactics weapons
	WPREFIX_MG42_PORTABLE,
	WPREFIX_WEBLEY,
	WPREFIX_NAGANTREV,
	WPREFIX_BERETTA,
	WPREFIX_ENFIELD,
	WPREFIX_SVT,
	WPREFIX_MOSIN,
	WPREFIX_G43,
	WPREFIX_ENFIELDL42A,
	WPREFIX_CARCANO,
	WPREFIX_DELISLE,
	WPREFIX_STEN,
	WPREFIX_PPSH,
	WPREFIX_MOSCHETTO,
	WPREFIX_FG42,
	WPREFIX_VICKERS,
	WPREFIX_BREDA,
	WPREFIX_F1_GRENADE,
	WPREFIX_MILLS_GRENADE,
	WPREFIX_NEBELHANDGRANATE,
	WPREFIX_M18_SMOKE_GRENADE,
	WPREFIX_RDG1_SMOKE_GRENADE,
	WPREFIX_BOMBA,
	WPREFIX_BOMBA_BREDA,
	WPREFIX_MINE,
	WPREFIX_MINE_DETECTOR,
	WPREFIX_MINE_DETECTOR_AXIS,
	WPREFIX_DETONATOR,
	WPREFIX_KAR98_MORTAR,
	WPREFIX_PIAT
};

static const int mohWeaponsToGeneral[] = {
	WP_NONE_GENERAL,
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
	WP_SHOTGUN_GENERAL,
	WP_UNARMED_GENERAL,
	//
	// Team Assault and Team Tactics weapons
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
	WP_PIAT_GENERAL
};

typedef struct mohWeaponInfo_t {
	const char* csName;
	const char* animPrefixName;
	int weaponClassBitmask;
};

static const mohWeaponInfo_t mohWeaponsInfo[] = {
	{"","",0},//WP_NONE_GENERAL,
	{"Papers","papers",MOH_WEAPON_CLASS_ANY_ITEM },//WP_PAPERS_GENERAL,
	{"Colt 45","colt45",MOH_WEAPON_CLASS_PISTOL },//WP_COLT45_GENERAL,
	{"Walther P38","p38",MOH_WEAPON_CLASS_PISTOL },//WP_P38_GENERAL,
	{"Hi-Standard Silenced","histandard",MOH_WEAPON_CLASS_PISTOL },//WP_HISTANDARD_GENERAL,
	{"M1 Garand","garand",MOH_WEAPON_CLASS_RIFLE },//WP_GARAND_GENERAL,
	{"Mauser KAR 98K","kar98",MOH_WEAPON_CLASS_RIFLE },//WP_KAR98_GENERAL,
	{"KAR98 - Sniper","kar98sniper",MOH_WEAPON_CLASS_RIFLE },//WP_KAR98SNIPER_GENERAL,
	{"Springfield '03 Sniper","springfield",MOH_WEAPON_CLASS_RIFLE },//WP_SPRINGFIELD_GENERAL,
	{"Thompson","thompson",MOH_WEAPON_CLASS_SMG },//WP_THOMPSON_GENERAL,
	{"MP40","mp40",MOH_WEAPON_CLASS_SMG },//WP_MP40_GENERAL,
	{"BAR","bar",MOH_WEAPON_CLASS_MG },//WP_BAR_GENERAL,
	{"StG 44","mp44",MOH_WEAPON_CLASS_MG },//WP_MP44_GENERAL,
	{"Frag Grenade","fraggrenade",MOH_WEAPON_CLASS_GRENADE },//WP_FRAGGRENADE_GENERAL,
	{"Stielhandgranate","stielhandgranate",MOH_WEAPON_CLASS_GRENADE },//WP_STIELHANDGRANATE_GENERAL,
	{"Bazooka","bazooka",MOH_WEAPON_CLASS_HEAVY },//WP_BAZOOKA_GENERAL,
	{"Panzerschreck","panzerschreck",MOH_WEAPON_CLASS_HEAVY },//WP_PANZERSCHRECK_GENERAL,
	{"Shotgun","shotgun",MOH_WEAPON_CLASS_HEAVY },//WP_SHOTGUN_GENERAL,
	{"","unarmed",0},//WP_UNARMED_GENERAL,
	//
	// Team Assault and Team Tactics weapons
	{"Packed MG42 Turret","mg42portable",MOH_WEAPON_CLASS_ANY_ITEM },//WP_MG42_PORTABLE_GENERAL,
	{"Webley Revolver","webley",MOH_WEAPON_CLASS_PISTOL },//WP_WEBLEY_GENERAL,
	{"Nagant Revolver","nagantrev",MOH_WEAPON_CLASS_PISTOL },//WP_NAGANTREV_GENERAL,
	{"Beretta","beretta",MOH_WEAPON_CLASS_PISTOL },//WP_BERETTA_GENERAL,
	{"Lee-Enfield","enfield",MOH_WEAPON_CLASS_RIFLE },//WP_ENFIELD_GENERAL,
	{"SVT 40","svt",MOH_WEAPON_CLASS_RIFLE },//WP_SVT_GENERAL,
	{"Mosin Nagant Rifle","mosin",MOH_WEAPON_CLASS_RIFLE },//WP_MOSIN_GENERAL,
	{"G 43","g43",MOH_WEAPON_CLASS_RIFLE },//WP_G43_GENERAL,
	{"Enfield L42A1","enfieldl42a1",MOH_WEAPON_CLASS_RIFLE },//WP_ENFIELDL42A_GENERAL,
	{"Carcano","carcano",MOH_WEAPON_CLASS_RIFLE },//WP_CARCANO_GENERAL,
	{"DeLisle","delisle",MOH_WEAPON_CLASS_RIFLE },//WP_DELISLE_GENERAL,
	{"Sten Mark II","sten",MOH_WEAPON_CLASS_SMG },//WP_STEN_GENERAL,
	{"PPSH SMG","ppsh",MOH_WEAPON_CLASS_SMG },//WP_PPSH_GENERAL,
	{"Moschetto","moschetto",MOH_WEAPON_CLASS_SMG },//WP_MOSCHETTO_GENERAL,
	{"FG 42","fg42",MOH_WEAPON_CLASS_MG },//WP_FG42_GENERAL,
	{"Vickers-Berthier","vickers",MOH_WEAPON_CLASS_MG },//WP_VICKERS_GENERAL,
	{"Breda","breda",MOH_WEAPON_CLASS_MG },//WP_BREDA_GENERAL,
	{"F1 Grenade","f1grenade",MOH_WEAPON_CLASS_GRENADE },//WP_F1_GRENADE_GENERAL,
	{"Mills Grenade","millsgrenade",MOH_WEAPON_CLASS_GRENADE },//WP_MILLS_GRENADE_GENERAL,
	{"Nebelhandgranate","nebelhandgranate",MOH_WEAPON_CLASS_GRENADE },//WP_NEBELHANDGRANATE_GENERAL,
	{"M18 Smoke Grenade","m18smokegrenade",MOH_WEAPON_CLASS_GRENADE },//WP_M18_SMOKE_GRENADE_GENERAL,
	{"RDG-1 Smoke Grenade","rdg1smokegrenade",MOH_WEAPON_CLASS_GRENADE },//WP_RDG1_SMOKE_GRENADE_GENERAL,
	{"Bomba A Mano","bomba",MOH_WEAPON_CLASS_GRENADE },//WP_BOMBA_GENERAL,
	{"Bomba A Mano Breda","bombabreda",MOH_WEAPON_CLASS_GRENADE },//WP_BOMBA_BREDA_GENERAL,
	{"LandmineAllies","mine",MOH_WEAPON_CLASS_GRENADE },//WP_MINE_GENERAL, - "LandmineAxis" has the same WPrefix ... damn
	{"Minedetector","minedetector",MOH_WEAPON_CLASS_GRENADE },//WP_MINE_DETECTOR_GENERAL,
	{"Minensuchgerat","minedetectoraxis",MOH_WEAPON_CLASS_GRENADE },//WP_MINE_DETECTOR_AXIS_GENERAL,
	{"","detonator",0},//WP_DETONATOR_GENERAL, (dunno what their configstrings are)
	{"","kar98mortar",0},//WP_KAR98_MORTAR_GENERAL, (dunno what their configstrings are)
	{"PIAT","PIAT",MOH_WEAPON_CLASS_HEAVY },//WP_PIAT_GENERAL
};



typedef enum {
	STAT_HEALTH_MOH,
	STAT_MAXHEALTH_MOH,
	STAT_WEAPONS_MOH,
	STAT_EQUIPPED_WEAPON_MOH,
	STAT_AMMO_MOH,
	STAT_MAXAMMO_MOH,
	STAT_CLIPAMMO_MOH,
	STAT_MAXCLIPAMMO_MOH,
	STAT_INZOOM_MOH,
	STAT_CROSSHAIR_MOH,
	STAT_LAST_PAIN_MOH,
	STAT_UNUSED_2_MOH,
	STAT_BOSSHEALTH_MOH,
	STAT_CINEMATIC_MOH,
	STAT_ADDFADE_MOH,
	STAT_LETTERBOX_MOH,
	STAT_COMPASSNORTH_MOH,
	STAT_OBJECTIVELEFT_MOH,
	STAT_OBJECTIVERIGHT_MOH,
	STAT_OBJECTIVECENTER_MOH,
	STAT_TEAM_MOH,
	STAT_KILLS_MOH,
	STAT_DEATHS_MOH,
	STAT_UNUSED_3_MOH,
	STAT_UNUSED_4_MOH,
	STAT_HIGHEST_SCORE_MOH,
	STAT_ATTACKERCLIENT_MOH,
	STAT_INFOCLIENT_MOH,
	STAT_INFOCLIENT_HEALTH_MOH,
	STAT_DAMAGEDIR_MOH,
	STAT_LAST_STAT_MOH
} playerstatMOHAA_t;


typedef enum {
	ET_MODELANIM_SKEL_MOH,
	ET_MODELANIM_MOH,
	ET_VEHICLE_MOH,
	ET_PLAYER_MOH,
	ET_ITEM_MOH,
	ET_GENERAL_MOH,
	ET_MISSILE_MOH,
	ET_MOVER_MOH,
	ET_BEAM_MOH,
	ET_MULTIBEAM_MOH,
	ET_PORTAL_MOH,
	ET_EVENT_ONLY_MOH,
	ET_RAIN_MOH,
	ET_LEAF_MOH,
	ET_SPEAKER_MOH,
	ET_PUSH_TRIGGER_MOH,
	ET_TELEPORT_TRIGGER_MOH,
	ET_DECAL_MOH,
	ET_EMITTER_MOH,
	ET_ROPE_MOH,
	ET_EVENTS_MOH,
	ET_EXEC_COMMANDS_MOH
} entityTypeMOH_t;



static const netField_t	entityStateFieldsMOHAA_ver_15[] =
{
{ NETF(pos.trBase[0]), 0, netFieldType_t::coord },//{ NETF(netorigin[0]), 0, netFieldType_t::coord },
{ NETF(pos.trBase[1]), 0, netFieldType_t::coord },//{ NETF(netorigin[1]), 0, netFieldType_t::coord },
{ NETF(apos.trBase[1]), 12, netFieldType_t::angle },//{ NETF(netangles[1]), 12, netFieldType_t::angle },
{ NETF(frameInfo[0].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[1].time), 15, netFieldType_t::animTime },
{ NETF(bone_angles[0][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[1][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][0]), -13, netFieldType_t::angle },
{ NETF(pos.trBase[2]), 0, netFieldType_t::coord },//{ NETF(netorigin[2]), 0, netFieldType_t::coord },
{ NETF(frameInfo[0].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[1].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[2].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[3].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[0].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[1].index), 12, netFieldType_t::regular },
{ NETF(actionWeight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[2].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[3].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[2].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[3].index), 12, netFieldType_t::regular },
{ NETF(eType), 8, netFieldType_t::regular },
{ NETF(modelindex), 16, netFieldType_t::regular },
{ NETF(parent), 16, netFieldType_t::regular },
{ NETF(constantLight), 32, netFieldType_t::regular },
{ NETF(renderfx), 32, netFieldType_t::regular },
{ NETF(bone_tag[0]), -8, netFieldType_t::regular },
{ NETF(bone_tag[1]), -8, netFieldType_t::regular },
{ NETF(bone_tag[2]), -8, netFieldType_t::regular },
{ NETF(bone_tag[3]), -8, netFieldType_t::regular },
{ NETF(bone_tag[4]), -8, netFieldType_t::regular },
{ NETF(scaleMOHAA), 10, netFieldType_t::scale },
{ NETF(alpha), 8, netFieldType_t::alpha },
{ NETF(usageIndex), 16, netFieldType_t::regular },
{ NETF(eFlags), 16, netFieldType_t::regular },
{ NETF(solid), 32, netFieldType_t::regular },
{ NETF(apos.trBase[2]), 12, netFieldType_t::angle },//{ NETF(netangles[2]), 12, netFieldType_t::angle },
{ NETF(apos.trBase[0]), 12, netFieldType_t::angle },//{ NETF(netangles[0]), 12, netFieldType_t::angle },
{ NETF(tag_num), 10, netFieldType_t::regular },
{ NETF(bone_angles[1][2]), -13, netFieldType_t::angle },
{ NETF(attach_use_angles), 1, netFieldType_t::regular },
{ NETF(origin2[1]), 0, netFieldType_t::coord },
{ NETF(origin2[0]), 0, netFieldType_t::coord },
{ NETF(origin2[2]), 0, netFieldType_t::coord },
{ NETF(bone_angles[0][2]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][2]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][2]), -13, netFieldType_t::angle },
{ NETF(surfaces[0]), 8, netFieldType_t::regular },
{ NETF(surfaces[1]), 8, netFieldType_t::regular },
{ NETF(surfaces[2]), 8, netFieldType_t::regular },
{ NETF(surfaces[3]), 8, netFieldType_t::regular },
{ NETF(bone_angles[0][1]), -13, netFieldType_t::angle },
{ NETF(surfaces[4]), 8, netFieldType_t::regular },
{ NETF(surfaces[5]), 8, netFieldType_t::regular },
{ NETF(pos.trTime), 32, netFieldType_t::regular },
{ NETF(pos.trDelta[0]), 0, netFieldType_t::velocity },
{ NETF(pos.trDelta[1]), 0, netFieldType_t::velocity },
{ NETF(pos.trDelta[2]), 0, netFieldType_t::velocity },
{ NETF(loopSound), 16, netFieldType_t::regular },
{ NETF(loopSoundVolume), 0, netFieldType_t::regular },
{ NETF(loopSoundMinDist), 0, netFieldType_t::regular },
{ NETF(loopSoundMaxDist), 0, netFieldType_t::regular },
{ NETF(loopSoundPitch), 0, netFieldType_t::regular },
{ NETF(loopSoundFlags), 8, netFieldType_t::regular },
{ NETF(attach_offset[0]), 0, netFieldType_t::regular },
{ NETF(attach_offset[1]), 0, netFieldType_t::regular },
{ NETF(attach_offset[2]), 0, netFieldType_t::regular },
{ NETF(beam_entnum), 16, netFieldType_t::regular },
{ NETF(skinNum), 16, netFieldType_t::regular },
{ NETF(wasframe), 10, netFieldType_t::regular },
{ NETF(frameInfo[4].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[5].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[6].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[7].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[8].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[9].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[10].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[11].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[12].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[13].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[14].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[15].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[4].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[5].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[6].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[7].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[8].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[9].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[10].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[11].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[12].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[13].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[14].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[15].time), 15, netFieldType_t::animTime },
{ NETF(frameInfo[4].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[5].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[6].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[7].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[8].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[9].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[10].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[11].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[12].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[13].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[14].weight), 8, netFieldType_t::animWeight },
{ NETF(frameInfo[15].weight), 8, netFieldType_t::animWeight },
{ NETF(bone_angles[1][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][2]), -13, netFieldType_t::angle },
{ NETF(clientNum), 8, netFieldType_t::regular },
{ NETF(groundEntityNum), GENTITYNUM_BITS, netFieldType_t::regular },
{ NETF(shader_data[0]), 0, netFieldType_t::regular },
{ NETF(shader_data[1]), 0, netFieldType_t::regular },
{ NETF(shader_time), 0, netFieldType_t::regular },
{ NETF(eyeVector[0]), 0, netFieldType_t::regular },
{ NETF(eyeVector[1]), 0, netFieldType_t::regular },
{ NETF(eyeVector[2]), 0, netFieldType_t::regular },
{ NETF(surfaces[6]), 8, netFieldType_t::regular },
{ NETF(surfaces[7]), 8, netFieldType_t::regular },
{ NETF(surfaces[8]), 8, netFieldType_t::regular },
{ NETF(surfaces[9]), 8, netFieldType_t::regular },
{ NETF(surfaces[10]), 8, netFieldType_t::regular },
{ NETF(surfaces[11]), 8, netFieldType_t::regular },
{ NETF(surfaces[12]), 8, netFieldType_t::regular },
{ NETF(surfaces[13]), 8, netFieldType_t::regular },
{ NETF(surfaces[14]), 8, netFieldType_t::regular },
{ NETF(surfaces[15]), 8, netFieldType_t::regular },
{ NETF(surfaces[16]), 8, netFieldType_t::regular },
{ NETF(surfaces[17]), 8, netFieldType_t::regular },
{ NETF(surfaces[18]), 8, netFieldType_t::regular },
{ NETF(surfaces[19]), 8, netFieldType_t::regular },
{ NETF(surfaces[20]), 8, netFieldType_t::regular },
{ NETF(surfaces[21]), 8, netFieldType_t::regular },
{ NETF(surfaces[22]), 8, netFieldType_t::regular },
{ NETF(surfaces[23]), 8, netFieldType_t::regular },
{ NETF(surfaces[24]), 8, netFieldType_t::regular },
{ NETF(surfaces[25]), 8, netFieldType_t::regular },
{ NETF(surfaces[26]), 8, netFieldType_t::regular },
{ NETF(surfaces[27]), 8, netFieldType_t::regular },
{ NETF(surfaces[28]), 8, netFieldType_t::regular },
{ NETF(surfaces[29]), 8, netFieldType_t::regular },
{ NETF(surfaces[30]), 8, netFieldType_t::regular },
{ NETF(surfaces[31]), 8, netFieldType_t::regular }
};

static const netField_t	entityStateFieldsMOHAA_ver_6[] =
{
{ NETF(pos.trBase[0]), 0, netFieldType_t::coord },
{ NETF(pos.trBase[1]), 0, netFieldType_t::coord },
{ NETF(apos.trBase[1]), 12, netFieldType_t::angle },
{ NETF(frameInfo[0].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[1].time), 0, netFieldType_t::animTime },
{ NETF(bone_angles[0][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[1][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][0]), -13, netFieldType_t::angle },
{ NETF(pos.trBase[2]), 0, netFieldType_t::coord },
{ NETF(frameInfo[0].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[1].weight), 0, netFieldType_t::animWeight},
{ NETF(frameInfo[2].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[3].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[0].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[1].index), 12, netFieldType_t::regular },
{ NETF(actionWeight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[2].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[3].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[2].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[3].index), 12, netFieldType_t::regular },
{ NETF(eType), 8, netFieldType_t::regular },
{ NETF(modelindex), 16, netFieldType_t::regular },
{ NETF(parent), 16, netFieldType_t::regular },
{ NETF(constantLight), 32, netFieldType_t::regular },
{ NETF(renderfx), 32, netFieldType_t::regular },
{ NETF(bone_tag[0]), -8, netFieldType_t::regular },
{ NETF(bone_tag[1]), -8, netFieldType_t::regular },
{ NETF(bone_tag[2]), -8, netFieldType_t::regular },
{ NETF(bone_tag[3]), -8, netFieldType_t::regular },
{ NETF(bone_tag[4]), -8, netFieldType_t::regular },
{ NETF(scaleMOHAA), 0, netFieldType_t::scale },
{ NETF(alpha), 0, netFieldType_t::alpha },
{ NETF(usageIndex), 16, netFieldType_t::regular },
{ NETF(eFlags), 16, netFieldType_t::regular },
{ NETF(solid), 32, netFieldType_t::regular },
{ NETF(apos.trBase[2]), 12, netFieldType_t::angle },
{ NETF(apos.trBase[0]), 12, netFieldType_t::angle },
{ NETF(tag_num), 10, netFieldType_t::regular },
{ NETF(bone_angles[1][2]), -13, netFieldType_t::angle },
{ NETF(attach_use_angles), 1, netFieldType_t::regular },
{ NETF(origin2[1]), 0, netFieldType_t::coord },
{ NETF(origin2[0]), 0, netFieldType_t::coord },
{ NETF(origin2[2]), 0, netFieldType_t::coord },
{ NETF(bone_angles[0][2]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][2]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][2]), -13, netFieldType_t::angle },
{ NETF(surfaces[0]), 8, netFieldType_t::regular },
{ NETF(surfaces[1]), 8, netFieldType_t::regular },
{ NETF(surfaces[2]), 8, netFieldType_t::regular },
{ NETF(surfaces[3]), 8, netFieldType_t::regular },
{ NETF(bone_angles[0][1]), -13, netFieldType_t::angle },
{ NETF(surfaces[4]), 8, netFieldType_t::regular },
{ NETF(surfaces[5]), 8, netFieldType_t::regular },
{ NETF(pos.trTime), 32, netFieldType_t::regular },
{ NETF(pos.trDelta[0]), 0, netFieldType_t::velocity },
{ NETF(pos.trDelta[1]), 0, netFieldType_t::velocity },
{ NETF(pos.trDelta[2]), 0, netFieldType_t::velocity },
{ NETF(loopSound), 16, netFieldType_t::regular },
{ NETF(loopSoundVolume), 0, netFieldType_t::regular },
{ NETF(loopSoundMinDist), 0, netFieldType_t::regular },
{ NETF(loopSoundMaxDist), 0, netFieldType_t::regular },
{ NETF(loopSoundPitch), 0, netFieldType_t::regular },
{ NETF(loopSoundFlags), 8, netFieldType_t::regular },
{ NETF(attach_offset[0]), 0, netFieldType_t::regular },
{ NETF(attach_offset[1]), 0, netFieldType_t::regular },
{ NETF(attach_offset[2]), 0, netFieldType_t::regular },
{ NETF(beam_entnum), 16, netFieldType_t::regular },
{ NETF(skinNum), 16, netFieldType_t::regular },
{ NETF(wasframe), 10, netFieldType_t::regular },
{ NETF(frameInfo[4].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[5].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[6].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[7].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[8].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[9].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[10].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[11].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[12].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[13].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[14].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[15].index), 12, netFieldType_t::regular },
{ NETF(frameInfo[4].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[5].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[6].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[7].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[8].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[9].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[10].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[11].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[12].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[13].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[14].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[15].time), 0, netFieldType_t::animTime },
{ NETF(frameInfo[4].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[5].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[6].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[7].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[8].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[9].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[10].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[11].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[12].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[13].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[14].weight), 0, netFieldType_t::animWeight },
{ NETF(frameInfo[15].weight), 0, netFieldType_t::animWeight },
{ NETF(bone_angles[1][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[2][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[3][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][0]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][1]), -13, netFieldType_t::angle },
{ NETF(bone_angles[4][2]), -13, netFieldType_t::angle },
{ NETF(clientNum), 8, netFieldType_t::regular },
{ NETF(groundEntityNum), GENTITYNUM_BITS, netFieldType_t::regular },
{ NETF(shader_data[0]), 0, netFieldType_t::regular },
{ NETF(shader_data[1]), 0, netFieldType_t::regular },
{ NETF(shader_time), 0, netFieldType_t::regular },
{ NETF(eyeVector[0]), 0, netFieldType_t::regular },
{ NETF(eyeVector[1]), 0, netFieldType_t::regular },
{ NETF(eyeVector[2]), 0, netFieldType_t::regular },
{ NETF(surfaces[6]), 8, netFieldType_t::regular },
{ NETF(surfaces[7]), 8, netFieldType_t::regular },
{ NETF(surfaces[8]), 8, netFieldType_t::regular },
{ NETF(surfaces[9]), 8, netFieldType_t::regular },
{ NETF(surfaces[10]), 8, netFieldType_t::regular },
{ NETF(surfaces[11]), 8, netFieldType_t::regular },
{ NETF(surfaces[12]), 8, netFieldType_t::regular },
{ NETF(surfaces[13]), 8, netFieldType_t::regular },
{ NETF(surfaces[14]), 8, netFieldType_t::regular },
{ NETF(surfaces[15]), 8, netFieldType_t::regular },
{ NETF(surfaces[16]), 8, netFieldType_t::regular },
{ NETF(surfaces[17]), 8, netFieldType_t::regular },
{ NETF(surfaces[18]), 8, netFieldType_t::regular },
{ NETF(surfaces[19]), 8, netFieldType_t::regular },
{ NETF(surfaces[20]), 8, netFieldType_t::regular },
{ NETF(surfaces[21]), 8, netFieldType_t::regular },
{ NETF(surfaces[22]), 8, netFieldType_t::regular },
{ NETF(surfaces[23]), 8, netFieldType_t::regular },
{ NETF(surfaces[24]), 8, netFieldType_t::regular },
{ NETF(surfaces[25]), 8, netFieldType_t::regular },
{ NETF(surfaces[26]), 8, netFieldType_t::regular },
{ NETF(surfaces[27]), 8, netFieldType_t::regular },
{ NETF(surfaces[28]), 8, netFieldType_t::regular },
{ NETF(surfaces[29]), 8, netFieldType_t::regular },
{ NETF(surfaces[30]), 8, netFieldType_t::regular },
{ NETF(surfaces[31]), 8, netFieldType_t::regular }
};

static const netField_t	playerStateFieldsMOHAA_ver_15[] =
{
{ PSF(commandTime), 32, netFieldType_t::regular },
{ PSF(origin[0]), 0, netFieldType_t::coordExtra },
{ PSF(origin[1]), 0, netFieldType_t::coordExtra },
{ PSF(viewangles[1]), 0, netFieldType_t::regular },
{ PSF(velocity[1]), 0, netFieldType_t::velocity },
{ PSF(velocity[0]), 0, netFieldType_t::velocity },
{ PSF(viewangles[0]), 0, netFieldType_t::regular },
{ PSF(origin[2]), 0, netFieldType_t::coordExtra },
{ PSF(velocity[2]), 0, netFieldType_t::velocity },
{ PSF(iViewModelAnimChanged), 2, netFieldType_t::regular },
{ PSF(damage_angles[0]), -13, netFieldType_t::angle },
{ PSF(damage_angles[1]), -13, netFieldType_t::angle },
{ PSF(damage_angles[2]), -13, netFieldType_t::angle },
{ PSF(speed), 16, netFieldType_t::regular },
{ PSF(delta_angles[1]), 16, netFieldType_t::regular },
{ PSF(viewheight), -8, netFieldType_t::regular },
{ PSF(groundEntityNum), GENTITYNUM_BITS, netFieldType_t::regular },
{ PSF(delta_angles[0]), 16, netFieldType_t::regular },
{ PSF(iNetViewModelAnim), 4, netFieldType_t::regular },
{ PSF(fovMOHAA), 0, netFieldType_t::regular },
{ PSF(current_music_mood), 8, netFieldType_t::regular },
{ PSF(gravity), 16, netFieldType_t::regular },
{ PSF(fallback_music_mood), 8, netFieldType_t::regular },
{ PSF(music_volume), 0, netFieldType_t::regular },
{ PSF(net_pm_flags), 16, netFieldType_t::regular },
{ PSF(clientNum), 8, netFieldType_t::regular },
{ PSF(fLeanAngle), 0, netFieldType_t::regular },
{ PSF(blend[3]), 0, netFieldType_t::regular },
{ PSF(blend[0]), 0, netFieldType_t::regular },
{ PSF(pm_type), 8, netFieldType_t::regular },
{ PSF(feetfalling), 8, netFieldType_t::regular },
{ PSF(radarInfo), 26, netFieldType_t::regular },
{ PSF(camera_angles[0]), 16, netFieldType_t::angle },
{ PSF(camera_angles[1]), 16, netFieldType_t::angle },
{ PSF(camera_angles[2]), 16, netFieldType_t::angle },
{ PSF(camera_origin[0]), 0, netFieldType_t::coordExtra },
{ PSF(camera_origin[1]), 0, netFieldType_t::coordExtra },
{ PSF(camera_origin[2]), 0, netFieldType_t::coordExtra },
{ PSF(camera_posofs[0]), 0, netFieldType_t::coordExtra },
{ PSF(camera_posofs[2]), 0, netFieldType_t::coordExtra },
{ PSF(camera_time), 0, netFieldType_t::regular },
{ PSF(voted), 1, netFieldType_t::regular },
{ PSF(bobCycle), 8, netFieldType_t::regular },
{ PSF(delta_angles[2]), 16, netFieldType_t::regular },
{ PSF(viewangles[2]), 0, netFieldType_t::regular },
{ PSF(music_volume_fade_time), 0, netFieldType_t::regular },
{ PSF(reverb_type), 6, netFieldType_t::regular },
{ PSF(reverb_level), 0, netFieldType_t::regular },
{ PSF(blend[1]), 0, netFieldType_t::regular },
{ PSF(blend[2]), 0, netFieldType_t::regular },
{ PSF(camera_offset[0]), 0, netFieldType_t::regular },
{ PSF(camera_offset[1]), 0, netFieldType_t::regular },
{ PSF(camera_offset[2]), 0, netFieldType_t::regular },
{ PSF(camera_posofs[1]), 0, netFieldType_t::coordExtra },
{ PSF(camera_flags), 16, netFieldType_t::regular }
};

const static netField_t	playerStateFieldsMOHAA_ver_6[] =
{
{ PSF(commandTime), 32, netFieldType_t::regular },
{ PSF(origin[0]), 0, netFieldType_t::coord },
{ PSF(origin[1]), 0, netFieldType_t::coord },
{ PSF(viewangles[1]), 0, netFieldType_t::regular },
{ PSF(velocity[1]), 0, netFieldType_t::velocity },
{ PSF(velocity[0]), 0, netFieldType_t::velocity },
{ PSF(viewangles[0]), 0, netFieldType_t::regular },
{ PSF(pm_time), -16, netFieldType_t::regular },
//{ PSF(weaponTime), -16, netFieldType_t::regular },
{ PSF(origin[2]), 0, netFieldType_t::coord },
{ PSF(velocity[2]), 0, netFieldType_t::velocity },
{ PSF(iViewModelAnimChanged), 2, netFieldType_t::regular },
{ PSF(damage_angles[0]), -13, netFieldType_t::angle },
{ PSF(damage_angles[1]), -13, netFieldType_t::angle },
{ PSF(damage_angles[2]), -13, netFieldType_t::angle },
{ PSF(speed), 16, netFieldType_t::regular },
{ PSF(delta_angles[1]), 16, netFieldType_t::regular },
{ PSF(viewheight), -8, netFieldType_t::regular },
{ PSF(groundEntityNum), GENTITYNUM_BITS, netFieldType_t::regular },
{ PSF(delta_angles[0]), 16, netFieldType_t::regular },
{ PSF(iNetViewModelAnim), 4, netFieldType_t::regular },
{ PSF(fovMOHAA), 0, netFieldType_t::regular },
{ PSF(current_music_mood), 8, netFieldType_t::regular },
{ PSF(gravity), 16, netFieldType_t::regular },
{ PSF(fallback_music_mood), 8, netFieldType_t::regular },
{ PSF(music_volume), 0, netFieldType_t::regular },
{ PSF(net_pm_flags), 16, netFieldType_t::regular },
{ PSF(clientNum), 8, netFieldType_t::regular },
{ PSF(fLeanAngle), 0, netFieldType_t::regular },
{ PSF(blend[3]), 0, netFieldType_t::regular },
{ PSF(blend[0]), 0, netFieldType_t::regular },
{ PSF(pm_type), 8, netFieldType_t::regular },
{ PSF(feetfalling), 8, netFieldType_t::regular },
{ PSF(camera_angles[0]), 16, netFieldType_t::angle },
{ PSF(camera_angles[1]), 16, netFieldType_t::angle },
{ PSF(camera_angles[2]), 16, netFieldType_t::angle },
{ PSF(camera_origin[0]), 0, netFieldType_t::coord },
{ PSF(camera_origin[1]), 0, netFieldType_t::coord },
{ PSF(camera_origin[2]), 0, netFieldType_t::coord },
{ PSF(camera_posofs[0]), 0, netFieldType_t::coord },
{ PSF(camera_posofs[2]), 0, netFieldType_t::coord },
{ PSF(camera_time), 0, netFieldType_t::regular },
{ PSF(bobCycle), 8, netFieldType_t::regular },
{ PSF(delta_angles[2]), 16, netFieldType_t::regular },
{ PSF(viewangles[2]), 0, netFieldType_t::regular },
{ PSF(music_volume_fade_time), 0, netFieldType_t::regular },
{ PSF(reverb_type), 6, netFieldType_t::regular },
{ PSF(reverb_level), 0, netFieldType_t::regular },
{ PSF(blend[1]), 0, netFieldType_t::regular },
{ PSF(blend[2]), 0, netFieldType_t::regular },
{ PSF(camera_offset[0]), 0, netFieldType_t::regular },
{ PSF(camera_offset[1]), 0, netFieldType_t::regular },
{ PSF(camera_offset[2]), 0, netFieldType_t::regular },
{ PSF(camera_posofs[1]), 0, netFieldType_t::coord },
{ PSF(camera_flags), 16, netFieldType_t::regular }
};

// means of death
// su44: changed to MoHAA's
typedef enum {
	MOD_NONE_MOH,
	MOD_SUICIDE_MOH,
	MOD_CRUSH_MOH,
	MOD_CRUSH_EVERY_FRAME_MOH,
	MOD_TELEFRAG_MOH,
	MOD_LAVA_MOH,
	MOD_SLIME_MOH,
	MOD_FALLING_MOH,
	MOD_LAST_SELF_INFLICTED_MOH,
	MOD_EXPLOSION_MOH,
	MOD_EXPLODEWALL_MOH,
	MOD_ELECTRIC_MOH,
	MOD_ELECTRICWATER_MOH,
	MOD_THROWNOBJECT_MOH,
	MOD_GRENADE_MOH,
	MOD_BEAM_MOH,
	MOD_ROCKET_MOH,
	MOD_IMPACT_MOH,
	MOD_BULLET_MOH,
	MOD_FAST_BULLET_MOH,
	MOD_VEHICLE_MOH,
	MOD_FIRE_MOH,
	MOD_FLASHBANG_MOH,
	MOD_ON_FIRE_MOH,
	MOD_GIB_MOH,
	MOD_IMPALE_MOH,
	MOD_BASH_MOH,
	MOD_SHOTGUN_MOH,
	MOD_SOMETHING_MOH,
	MOD_LANDMINE_MOH,
	MOD_TOTAL_NUMBER_MOH,

} meansOfDeathMOH_t;

/*
typedef struct mohParsedObituary_t {
	qboolean isObituary;
	int attacker;
	int target;
	meansOfDeathMOH_t meansOfDeath;

};*/

static int mohaaModToGeneralMap[]{ // based on wolfcamql
	MOD_UNKNOWN_GENERAL,//MOD_NONE_GENERAL,
	MOD_SUICIDE_GENERAL,
	MOD_CRUSH_GENERAL,
	MOD_CRUSH_EVERY_FRAME_GENERAL,
	MOD_TELEFRAG_GENERAL,
	MOD_LAVA_GENERAL,
	MOD_SLIME_GENERAL,
	MOD_FALLING_GENERAL,
	MOD_LAST_SELF_INFLICTED_GENERAL,
	MOD_EXPLOSION_GENERAL,
	MOD_EXPLODEWALL_GENERAL,
	MOD_ELECTRIC_GENERAL,
	MOD_ELECTRICWATER_GENERAL,
	MOD_THROWNOBJECT_GENERAL,
	MOD_GRENADE_GENERAL,
	MOD_BEAM_GENERAL,
	MOD_ROCKET_GENERAL,
	MOD_IMPACT_GENERAL,
	MOD_BULLET_GENERAL,
	MOD_FAST_BULLET_GENERAL,
	MOD_VEHICLE_GENERAL,
	MOD_FIRE_GENERAL,
	MOD_FLASHBANG_GENERAL,
	MOD_ON_FIRE_GENERAL,
	MOD_GIB_GENERAL,
	MOD_IMPALE_GENERAL,
	MOD_BASH_GENERAL,
	MOD_SHOTGUN_GENERAL,
	MOD_UNKNOWN_GENERAL,// Not sure.
	MOD_LANDMINE_GENERAL,
	MOD_MAX_GENERAL,//MOD_TOTAL_NUMBER_GENERAL,
};

static const int mohaaEntityTypeToGeneral[]{ // based on wolfcamql
	ET_MODELANIM_SKEL_GENERAL,
	ET_MODELANIM_GENERAL,
	ET_VEHICLE_GENERAL,
	ET_PLAYER_GENERAL,
	ET_ITEM_GENERAL,
	ET_GENERAL_GENERAL,
	ET_MISSILE_GENERAL,
	ET_MOVER_GENERAL,
	ET_BEAM_GENERAL,
	ET_MULTIBEAM_GENERAL,
	ET_PORTAL_GENERAL,
	ET_EVENT_ONLY_GENERAL,
	ET_RAIN_GENERAL,
	ET_LEAF_GENERAL,
	ET_SPEAKER_GENERAL,
	ET_PUSH_TRIGGER_GENERAL,
	ET_TELEPORT_TRIGGER_GENERAL,
	ET_DECAL_GENERAL,
	ET_EMITTER_GENERAL,
	ET_ROPE_GENERAL,
	ET_EVENTS_GENERAL,
	ET_EXEC_COMMANDS_GENERAL
};





#define MOH_MAX_KILLMSG_S2_VARIATIONS 2
typedef struct mohMeansOfDeath_t {
	qboolean attackerExistsAndIsClient;
	qboolean isSelfKill;
	char* s2[MOH_MAX_KILLMSG_S2_VARIATIONS]; // 2 means maximum 2 variations. Increase if more needed.
	meansOfDeathMOH_t meansOfDeath;
	int weaponClass;
	qboolean wasZoomed;
};



#define	MAX_NAME_LENGTH_MOHAA		32		// max length of a client name
static const tsl::htrie_map<char, mohMeansOfDeath_t> mohMeansOfDeathArray = {
	// Client attacker
	{"was crushes by",{qtrue,qfalse,{},MOD_CRUSH_MOH}},
	{"was telefragged by",{qtrue,qfalse,{},MOD_TELEFRAG_MOH}},
	{"was pushed over the edge by",{qtrue,qfalse,{},MOD_FALLING_MOH}},
	{"was blown away by",{qtrue,qfalse,{},MOD_EXPLOSION_MOH}},
	{"tripped on",{qtrue,qfalse,{"'s grenade"},MOD_GRENADE_MOH}}, // OpenMOHAA misspells as "'s' grenade"
	{"is picking",{qtrue,qfalse,{"'s shrapnel out of his teeth"},MOD_GRENADE_MOH}}, // OpenMOHAA misspells as "'s' shrapnel out of his teeth"
	{"took",{qtrue,qfalse,{"'s rocket right in the kisser","'s rocket in the face"},MOD_ROCKET_MOH}},
	{"was knocked out by",{qtrue,qfalse,{},MOD_IMPACT_MOH}},
	{"was shot by",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH}},
	{"was gunned down by",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_PISTOL}},
	{"was sniped by",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_RIFLE,qtrue}},
	{"was rifled by",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_RIFLE}},
	{"was perforated by",{qtrue,qfalse,{"'s SMG"},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_SMG}}, // OpenMOHAA misspells as "'s' SMG"
	{"was machine-gunned by",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_MG}},
	{"was run over by",{qtrue,qfalse,{},MOD_VEHICLE_MOH}},
	{"was burned up by",{qtrue,qfalse,{},MOD_FIRE_MOH}}, // Can also be MOD_LAVA, MOD_SLIME, MOD_ON_FIRE
	{"was impaled by",{qtrue,qfalse,{},MOD_IMPALE_MOH}},
	{"was bashed by",{qtrue,qfalse,{},MOD_BASH_MOH}},
	{"was clubbed by",{qtrue,qfalse,{},MOD_BASH_MOH}},
	{"was hunted down by",{qtrue,qfalse,{},MOD_SHOTGUN_MOH}},
	{"was pumped full of buckshot by",{qtrue,qfalse,{},MOD_SHOTGUN_MOH}},
	{"was killed by",{qtrue,qfalse,{},MOD_NONE_MOH}},
	{"stepped on",{qtrue,qfalse,{"'s landmine"},MOD_LANDMINE_MOH}}, // Breakthrough/maybe SH

	// No client attacker
	{"was burned to a crisp",{qfalse,qfalse,{},MOD_LAVA_MOH}}, // Could also be MOD_SLIME
	{"was melted to nothing",{qfalse,qfalse,{},MOD_SLIME_MOH}}, // Breakthrough
	{"cratered",{qfalse,qfalse,{},MOD_FALLING_MOH}},
	{"blew up",{qfalse,qfalse,{},MOD_EXPLOSION_MOH}}, // Could also be MOD_GRENADE
	{"caught a rocket",{qfalse,qfalse,{},MOD_ROCKET_MOH}},
	{"was shot in the",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"was shot",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"died",{qfalse,qfalse,{},MOD_NONE_MOH}},
	{"stepped on a land mine",{qfalse,qfalse,{},MOD_LANDMINE_MOH}}, // Breakthrough/maybe SH

	// Self kill
	{"took himself out of commision",{qtrue,qtrue,{},MOD_SUICIDE_MOH}},
	//{"was burned to a crisp",{qtrue,qtrue,"",MOD_LAVA_MOH}}, // Could also be MOD_SLIME // Can't tell this apart from no client attacker
	//{"cratered",{qtrue,qtrue,"",MOD_FALLING_MOH}},  // Can't tell this apart from no client attacker
	{"blew himself up",{qtrue,qtrue,{},MOD_EXPLOSION_MOH}},
	{"played catch with himself",{qtrue,qtrue,{},MOD_GRENADE_MOH}},
	{"tripped on his own grenade",{qtrue,qtrue,{},MOD_GRENADE_MOH}},
	{"rocketed himself",{qtrue,qtrue,{},MOD_ROCKET_MOH}},
	{"shot himself",{qtrue,qtrue,{},MOD_BULLET_MOH}},  // Could also be MOD_FAST_BULLET
	{"shot himself in the",{qtrue,qtrue,{},MOD_BULLET_MOH}},  // Could also be MOD_FAST_BULLET
	//{"died",{qtrue,qtrue,"",MOD_NONE_MOH}},  // Could also be MOD_FAST_BULLET  // Can't tell this apart from no client attacker
	{"was hoist on his own pitard",{qtrue,qtrue,{},MOD_LANDMINE_MOH}}, // Breakthrough/maybe SH
	
	// Spanish versions: (yeah I know, cancer)
	// Client attacker
	{"fue aplastado por",{qtrue,qfalse,{},MOD_CRUSH_MOH}},
	{"fue hecho trizas por",{qtrue,qfalse,{},MOD_TELEFRAG_MOH}},
	{"fue empotrado en la pared por",{qtrue,qfalse,{},MOD_FALLING_MOH}},
	{"fue sorprendido por",{qtrue,qfalse,{},MOD_EXPLOSION_MOH}},
	{"Tropezó sobre",{qtrue,qfalse,{" con una granada"},MOD_GRENADE_MOH}}, // OpenMOHAA misspells as "'s' grenade"
	{"es cogido",{qtrue,qfalse,{" quitándole los dientes"},MOD_GRENADE_MOH}}, // OpenMOHAA misspells as "'s' shrapnel out of his teeth"
	{"ha sido alcanzado por",{qtrue,qfalse,{" con un misilazo en la boca"," con un bazookazo en la cara"},MOD_ROCKET_MOH}},
	{"fue puesto fuera de combate por",{qtrue,qfalse,{},MOD_IMPACT_MOH}},
	//{"fue disparado por",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH}}, // sadly same as the rifle specific one in spanish.
	{"fue fusilado por",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_PISTOL}},
	{"fue abatido con un rifle de francotirador por",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_RIFLE,qtrue}},
	{"fue disparado por",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_RIFLE}},
	{"fue acribillado por",{qtrue,qfalse,{" con un SMG"},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_SMG}}, // OpenMOHAA misspells as "'s' SMG"
	{"fue cosido a balazos por",{qtrue,qfalse,{},MOD_FAST_BULLET_MOH,MOH_WEAPON_CLASS_MG}},
	{"fue atropellado por",{qtrue,qfalse,{},MOD_VEHICLE_MOH}},
	{"fue abrasado por",{qtrue,qfalse,{},MOD_FIRE_MOH}}, // Can also be MOD_LAVA, MOD_SLIME, MOD_ON_FIRE
	{"fue empalado por",{qtrue,qfalse,{},MOD_IMPALE_MOH}},
	{"fue fuertemente golpeado por",{qtrue,qfalse,{},MOD_BASH_MOH}},
	{"fue aporreado por",{qtrue,qfalse,{},MOD_BASH_MOH}},
	{"fue perseguido y cazado por",{qtrue,qfalse,{},MOD_SHOTGUN_MOH}},
	{"fue perdigoneado por",{qtrue,qfalse,{},MOD_SHOTGUN_MOH}},
	{"fue asesinado por",{qtrue,qfalse,{},MOD_NONE_MOH}},

	// No client attacker
	{"fue achicharrado por",{qfalse,qfalse,{},MOD_LAVA_MOH}}, // Could also be MOD_SLIME
	{"cráterizo",{qfalse,qfalse,{},MOD_FALLING_MOH}},
	{"explotó",{qfalse,qfalse,{},MOD_EXPLOSION_MOH}}, // Could also be MOD_GRENADE
	{"coges cohetes",{qfalse,qfalse,{},MOD_ROCKET_MOH}},
	{"fue disparado en el/la",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"fue disparado",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"murio",{qfalse,qfalse,{},MOD_NONE_MOH}},

	// Self kill
	{"se mató",{qtrue,qtrue,{},MOD_SUICIDE_MOH}},
	//{"was burned to a crisp",{qtrue,qtrue,"",MOD_LAVA_MOH}}, // Could also be MOD_SLIME // Can't tell this apart from no client attacker
	//{"cratered",{qtrue,qtrue,"",MOD_FALLING_MOH}},  // Can't tell this apart from no client attacker
	{"te explotaste tu mismo",{qtrue,qtrue,{},MOD_EXPLOSION_MOH}},
	{"Te eliminaste tu mismo",{qtrue,qtrue,{},MOD_GRENADE_MOH}},
	{"Tropezó encima de su propia granada",{qtrue,qtrue,{},MOD_GRENADE_MOH}},
	{"Te lanzaste un bazookazo",{qtrue,qtrue,{},MOD_ROCKET_MOH}},
	{"Se disparó el mismo",{qtrue,qtrue,{},MOD_BULLET_MOH}},  // Could also be MOD_FAST_BULLET
	{"Se disparó él mismo",{qtrue,qtrue,{},MOD_BULLET_MOH}},  // Could also be MOD_FAST_BULLET
	//{"died",{qtrue,qtrue,"",MOD_NONE_MOH}},  // Could also be MOD_FAST_BULLET  // Can't tell this apart from no client attacker


};

static const tsl::htrie_map<char, int> mohKillLocationArray = {
	{"in the head",0},
	{"in the helmet",1},
	{"in the neck",2},
	{"in the upper torso",3},
	{"in the middle torso",4},
	{"in the lower torso",5},
	{"in the pelvis",6},
	{"in the upper right arm",7},
	{"in the upper left arm",8},
	{"in the upper right leg",9},
	{"in the upper left leg",10},
	{"in the lower right arm",11},
	{"in the lower left arm",12},
	{"in the lower right leg",13},
	{"in the lower left leg",14},
	{"in the right hand",15},
	{"in the left hand",16},
	{"in the right foot",17},
	{"in the left foot",18},

	// Spanish
	{"en la cabeza",0},
	{"en el casco",1},
	{"en el cuello",2},
	{"en el pecho",3},
	{"en el estómago",4},
	{"en la tripa",5},
	{"en la cadera",6},
	{"en el brazo derecho",7},
	{"en el brazo izquierdo",8},
	{"en el muslo derecho",9},
	{"en el muslo izquierdo",10},
	{"en el antebrazo derecho",11},
	{"en el antebrazo izquierdo",12},
	{"en la pierna derecha",13},
	{"en la pierna izquierda",14},
	{"en la mano derecha",15},
	{"en la mano izquierda",16},
	{"en el pie derecho",17},
	{"en el pie izquierdo",18},
};




static constexpr const char* playerAnimNames[] = {
	"americanselectionidle1",
	"americanselectionidle2",
	"americanselectionidle3",
	"bazooka_crouch_fire",
	"bazooka_crouch_idle",
	"bazooka_crouch_lower",
	"bazooka_crouch_raise",
	"bazooka_crouch_run_back",
	"bazooka_crouch_run_fire",
	"bazooka_crouch_run_fwd",
	"bazooka_crouch_run_left",
	"bazooka_crouch_run_right",
	"bazooka_crouch_walk_back",
	"bazooka_crouch_walk_fwd",
	"bazooka_crouch_walk_left",
	"bazooka_crouch_walk_right",
	"bazooka_fall",
	"bazooka_jump_run_takeoff",
	"bazooka_jump_takeoff",
	"bazooka_land",
	"bazooka_land_hard",
	"bazooka_pain",
	"bazooka_pain_ducked",
	"bazooka_reload",
	"bazooka_run_into_wall",
	"bazooka_stand_fire",
	"bazooka_stand_idle",
	"bazooka_stand_lower",
	"bazooka_stand_raise",
	"bazooka_stand_run_back",
	"bazooka_stand_run_fire",
	"bazooka_stand_run_fwd",
	"bazooka_stand_run_left",
	"bazooka_stand_run_right",
	"bazooka_stand_walk_back",
	"bazooka_stand_walk_fire",
	"bazooka_stand_walk_fwd",
	"bazooka_stand_walk_left",
	"bazooka_stand_walk_right",
	"death1",
	"death2",
	"death3",
	"death_back1",
	"death_back2",
	"death_back3",
	"death_chest",
	"death_choke",
	"death_collapse",
	"death_crotch",
	"death_crouch",
	"death_explosion_back",
	"death_explosion_front",
	"death_explosion_large",
	"death_explosion_left",
	"death_explosion_right",
	"death_explosion_small",
	"death_fall_back",
	"death_fall_to_knees",
	"death_frontchoke",
	"death_frontcrouch",
	"death_head_flyforward",
	"death_headpistol",
	"death_knockedup",
	"death_left",
	"death_prone",
	"death_right",
	"death_run01",
	"death_run02",
	"death_run03",
	"death_shoot",
	"death_twist",
	"germanselectionidle1",
	"germanselectionidle2",
	"germanselectionidle3",
	"grenade_crouch_charge",
	"grenade_crouch_charge_secondary",
	"grenade_crouch_fire",
	"grenade_crouch_fire_secondary",
	"grenade_crouch_idle",
	"grenade_crouch_lower",
	"grenade_crouch_raise",
	"grenade_crouch_run_back",
	"grenade_crouch_run_fwd",
	"grenade_crouch_run_left",
	"grenade_crouch_run_right",
	"grenade_crouch_walk_back",
	"grenade_crouch_walk_fwd",
	"grenade_crouch_walk_left",
	"grenade_crouch_walk_right",
	"grenade_fall",
	"grenade_jump_run_takeoff",
	"grenade_jump_takeoff",
	"grenade_land",
	"grenade_land_hard",
	"grenade_pain",
	"grenade_pain_ducked",
	"grenade_reload",
	"grenade_run_into_wall",
	"grenade_stand_charge",
	"grenade_stand_charge_secondary",
	"grenade_stand_fire",
	"grenade_stand_fire_secondary",
	"grenade_stand_idle1",
	"grenade_stand_idle2",
	"grenade_stand_lower",
	"grenade_stand_raise",
	"grenade_stand_run_back",
	"grenade_stand_run_fwd",
	"grenade_stand_run_left",
	"grenade_stand_run_right",
	"grenade_stand_walk_back",
	"grenade_stand_walk_fwd",
	"grenade_stand_walk_left",
	"grenade_stand_walk_right",
	"histandard_reload",
	"idle",
	"kar98_crouch_rechamber",
	"kar98_reload",
	"kar98_stand_rechamber",
	"ladder_down_left",
	"ladder_down_right",
	"ladder_getoff_bottom",
	"ladder_getoff_top",
	"ladder_geton_bottom",
	"ladder_geton_top",
	"ladder_idle_left",
	"ladder_idle_right",
	"ladder_slide",
	"ladder_up_left",
	"ladder_up_right",
	"mg_crouch_fire",
	"mg_crouch_idle",
	"mg_crouch_lower",
	"mg_crouch_raise",
	"mg_crouch_run_back",
	"mg_crouch_run_fire",
	"mg_crouch_run_fwd",
	"mg_crouch_run_left",
	"mg_crouch_run_right",
	"mg_crouch_walk_back",
	"mg_crouch_walk_fwd",
	"mg_crouch_walk_left",
	"mg_crouch_walk_right",
	"mg_fall",
	"mg_jump_run_takeoff",
	"mg_jump_takeoff",
	"mg_land",
	"mg_land_hard",
	"mg_pain",
	"mg_pain_ducked",
	"mg_reload",
	"mg_run_into_wall",
	"mg_stand_fire",
	"mg_stand_idle",
	"mg_stand_lower",
	"mg_stand_raise",
	"mg_stand_run_back",
	"mg_stand_run_fire",
	"mg_stand_run_fwd",
	"mg_stand_run_left",
	"mg_stand_run_right",
	"mg_stand_walk_back",
	"mg_stand_walk_fire",
	"mg_stand_walk_fwd",
	"mg_stand_walk_left",
	"mg_stand_walk_right",
	"mp40_crouch_fire",
	"mp40_crouch_idle",
	"mp40_crouch_lower",
	"mp40_crouch_raise",
	"mp40_crouch_run_back",
	"mp40_crouch_run_fire",
	"mp40_crouch_run_fwd",
	"mp40_crouch_run_left",
	"mp40_crouch_run_right",
	"mp40_crouch_walk_back",
	"mp40_crouch_walk_fwd",
	"mp40_crouch_walk_left",
	"mp40_crouch_walk_right",
	"mp40_fall",
	"mp40_jump_run_takeoff",
	"mp40_jump_takeoff",
	"mp40_land",
	"mp40_land_hard",
	"mp40_pain",
	"mp40_pain_ducked",
	"mp40_reload",
	"mp40_run_into_wall",
	"mp40_stand_fire",
	"mp40_stand_idle",
	"mp40_stand_lower",
	"mp40_stand_raise",
	"mp40_stand_run_back",
	"mp40_stand_run_fire",
	"mp40_stand_run_fwd",
	"mp40_stand_run_left",
	"mp40_stand_run_right",
	"mp40_stand_walk_back",
	"mp40_stand_walk_fire",
	"mp40_stand_walk_fwd",
	"mp40_stand_walk_left",
	"mp40_stand_walk_right",
	"mp44_crouch_fire",
	"mp44_crouch_idle",
	"mp44_crouch_lower",
	"mp44_crouch_raise",
	"mp44_crouch_run_back",
	"mp44_crouch_run_fire",
	"mp44_crouch_run_fwd",
	"mp44_crouch_run_left",
	"mp44_crouch_run_right",
	"mp44_crouch_walk_back",
	"mp44_crouch_walk_fwd",
	"mp44_crouch_walk_left",
	"mp44_crouch_walk_right",
	"mp44_fall",
	"mp44_jump_run_takeoff",
	"mp44_jump_takeoff",
	"mp44_land",
	"mp44_land_hard",
	"mp44_pain",
	"mp44_pain_ducked",
	"mp44_reload",
	"mp44_run_into_wall",
	"mp44_stand_fire",
	"mp44_stand_idle",
	"mp44_stand_lower",
	"mp44_stand_raise",
	"mp44_stand_run_back",
	"mp44_stand_run_fire",
	"mp44_stand_run_fwd",
	"mp44_stand_run_left",
	"mp44_stand_run_right",
	"mp44_stand_walk_back",
	"mp44_stand_walk_fire",
	"mp44_stand_walk_fwd",
	"mp44_stand_walk_left",
	"mp44_stand_walk_right",
	"p38_reload",
	"panzerschreck_reload",
	"pistol_butt1",
	"pistol_butt2",
	"pistol_crouch_fire",
	"pistol_crouch_idle",
	"pistol_crouch_lower",
	"pistol_crouch_raise",
	"pistol_crouch_run_back",
	"pistol_crouch_run_fire",
	"pistol_crouch_run_fwd",
	"pistol_crouch_run_left",
	"pistol_crouch_run_right",
	"pistol_crouch_walk_back",
	"pistol_crouch_walk_fwd",
	"pistol_crouch_walk_left",
	"pistol_crouch_walk_right",
	"pistol_fall",
	"pistol_jump_run_takeoff",
	"pistol_jump_takeoff",
	"pistol_land",
	"pistol_land_hard",
	"pistol_pain",
	"pistol_pain_ducked",
	"pistol_reload",
	"pistol_run_into_wall",
	"pistol_silenced_crouch_fire",
	"pistol_silenced_crouch_run_fire",
	"pistol_silenced_stand_fire",
	"pistol_silenced_stand_run_fire",
	"pistol_stand_fire",
	"pistol_stand_idle1",
	"pistol_stand_idle2",
	"pistol_stand_lower",
	"pistol_stand_raise",
	"pistol_stand_run_back",
	"pistol_stand_run_fire",
	"pistol_stand_run_fwd",
	"pistol_stand_run_left",
	"pistol_stand_run_right",
	"pistol_stand_walk_back",
	"pistol_stand_walk_fwd",
	"pistol_stand_walk_left",
	"pistol_stand_walk_right",
	"rifle_crouch_fire",
	"rifle_crouch_idle",
	"rifle_crouch_lower",
	"rifle_crouch_raise",
	"rifle_crouch_run_back",
	"rifle_crouch_run_fire",
	"rifle_crouch_run_fwd",
	"rifle_crouch_run_left",
	"rifle_crouch_run_right",
	"rifle_crouch_walk_back",
	"rifle_crouch_walk_fwd",
	"rifle_crouch_walk_left",
	"rifle_crouch_walk_right",
	"rifle_fall",
	"rifle_jump_run_takeoff",
	"rifle_jump_takeoff",
	"rifle_land",
	"rifle_land_hard",
	"rifle_pain",
	"rifle_pain_ducked",
	"rifle_pain_kneestodeath",
	"rifle_reload",
	"rifle_run_into_wall",
	"rifle_stand_fire",
	"rifle_stand_idle",
	"rifle_stand_lower",
	"rifle_stand_raise",
	"rifle_stand_run_back",
	"rifle_stand_run_fire",
	"rifle_stand_run_fwd",
	"rifle_stand_run_left",
	"rifle_stand_run_right",
	"rifle_stand_walk_back",
	"rifle_stand_walk_fire",
	"rifle_stand_walk_fwd",
	"rifle_stand_walk_left",
	"rifle_stand_walk_right",
	"shotgun_crouch_fire",
	"shotgun_crouch_idle",
	"shotgun_crouch_lower",
	"shotgun_crouch_raise",
	"shotgun_crouch_run_back",
	"shotgun_crouch_run_fire",
	"shotgun_crouch_run_fwd",
	"shotgun_crouch_run_left",
	"shotgun_crouch_run_right",
	"shotgun_crouch_walk_back",
	"shotgun_crouch_walk_fwd",
	"shotgun_crouch_walk_left",
	"shotgun_crouch_walk_right",
	"shotgun_fall",
	"shotgun_jump_run_takeoff",
	"shotgun_jump_takeoff",
	"shotgun_land",
	"shotgun_land_hard",
	"shotgun_pain",
	"shotgun_pain_ducked",
	"shotgun_reload_end",
	"shotgun_reload_loop",
	"shotgun_reload_start",
	"shotgun_run_into_wall",
	"shotgun_stand_fire",
	"shotgun_stand_idle",
	"shotgun_stand_lower",
	"shotgun_stand_raise",
	"shotgun_stand_run_back",
	"shotgun_stand_run_fire",
	"shotgun_stand_run_fwd",
	"shotgun_stand_run_left",
	"shotgun_stand_run_right",
	"shotgun_stand_walk_back",
	"shotgun_stand_walk_fire",
	"shotgun_stand_walk_fwd",
	"shotgun_stand_walk_left",
	"shotgun_stand_walk_right",
	"show_papers",
	"smg_crouch_fire",
	"smg_crouch_idle",
	"smg_crouch_lower",
	"smg_crouch_raise",
	"smg_crouch_run_back",
	"smg_crouch_run_fire",
	"smg_crouch_run_fwd",
	"smg_crouch_run_left",
	"smg_crouch_run_right",
	"smg_crouch_walk_back",
	"smg_crouch_walk_fwd",
	"smg_crouch_walk_left",
	"smg_crouch_walk_right",
	"smg_fall",
	"smg_jump_run_takeoff",
	"smg_jump_takeoff",
	"smg_land",
	"smg_land_hard",
	"smg_pain",
	"smg_pain_ducked",
	"smg_reload",
	"smg_run_into_wall",
	"smg_stand_fire",
	"smg_stand_idle",
	"smg_stand_lower",
	"smg_stand_raise",
	"smg_stand_run_back",
	"smg_stand_run_fire",
	"smg_stand_run_fwd",
	"smg_stand_run_left",
	"smg_stand_run_right",
	"smg_stand_walk_back",
	"smg_stand_walk_fire",
	"smg_stand_walk_fwd",
	"smg_stand_walk_left",
	"smg_stand_walk_right",
	"springfield_crouch_rechamber",
	"springfield_reload_end",
	"springfield_reload_loop",
	"springfield_reload_start",
	"springfield_stand_rechamber",
	"steilhandgranate_crouch_charge",
	"steilhandgranate_crouch_charge_secondary",
	"steilhandgranate_crouch_fire",
	"steilhandgranate_crouch_fire_secondary",
	"steilhandgranate_crouch_idle",
	"steilhandgranate_crouch_lower",
	"steilhandgranate_crouch_raise",
	"steilhandgranate_crouch_run_back",
	"steilhandgranate_crouch_run_fwd",
	"steilhandgranate_crouch_run_left",
	"steilhandgranate_crouch_run_right",
	"steilhandgranate_crouch_walk_back",
	"steilhandgranate_crouch_walk_fwd",
	"steilhandgranate_crouch_walk_left",
	"steilhandgranate_crouch_walk_right",
	"steilhandgranate_fall",
	"steilhandgranate_jump_run_takeoff",
	"steilhandgranate_jump_takeoff",
	"steilhandgranate_land",
	"steilhandgranate_land_hard",
	"steilhandgranate_pain",
	"steilhandgranate_pain_ducked",
	"steilhandgranate_reload",
	"steilhandgranate_run_into_wall",
	"steilhandgranate_stand_charge",
	"steilhandgranate_stand_charge_secondary",
	"steilhandgranate_stand_fire",
	"steilhandgranate_stand_fire_secondary",
	"steilhandgranate_stand_idle1",
	"steilhandgranate_stand_idle2",
	"steilhandgranate_stand_lower",
	"steilhandgranate_stand_raise",
	"steilhandgranate_stand_run_back",
	"steilhandgranate_stand_run_fwd",
	"steilhandgranate_stand_run_left",
	"steilhandgranate_stand_run_right",
	"steilhandgranate_stand_walk_back",
	"steilhandgranate_stand_walk_fwd",
	"steilhandgranate_stand_walk_left",
	"steilhandgranate_stand_walk_right",
	"turret_idle1",
	"turret_idle2",
	"unarmed_crouch_idle",
	"unarmed_crouch_run_back",
	"unarmed_crouch_run_fwd",
	"unarmed_crouch_run_left",
	"unarmed_crouch_run_right",
	"unarmed_crouch_walk_back",
	"unarmed_crouch_walk_fwd",
	"unarmed_crouch_walk_left",
	"unarmed_crouch_walk_right",
	"unarmed_fall",
	"unarmed_jump_run_takeoff",
	"unarmed_jump_takeoff",
	"unarmed_land",
	"unarmed_land_hard",
	"unarmed_run_into_wall",
	"unarmed_stand_idle",
	"unarmed_stand_run_back",
	"unarmed_stand_run_fwd",
	"unarmed_stand_run_left",
	"unarmed_stand_run_right",
	"unarmed_stand_walk_back",
	"unarmed_stand_walk_fwd",
	"unarmed_stand_walk_left",
	"unarmed_stand_walk_right",
	"vehicle_idle",
};

static constexpr const vmAnimMoh_e playerAnimToVMAnimMap[] = {
	VM_ANIM_IDLE_0_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_0_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_SECONDARY_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_SECONDARY_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RECHAMBER_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RECHAMBER_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RELOAD_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_END_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_RECHAMBER_MOH,
	VM_ANIM_RELOAD_END_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_RECHAMBER_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_SECONDARY_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_RELOAD_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_CHARGE_MOH,
	VM_ANIM_FIRE_MOH,
	VM_ANIM_FIRE_SECONDARY_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_PUTAWAY_MOH,
	VM_ANIM_PULLOUT_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_1_MOH,
	VM_ANIM_IDLE_2_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
		VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH,
	VM_ANIM_IDLE_MOH
};



/*
static inline constexpr int tolowerConstExpr(int val ) {
	constexpr int loweringOffset = 'a' - 'A';
	if (val >= 'A' && val <= 'Z') {
		return val + loweringOffset;
	}
	return val;
}
static inline constexpr bool strisameConstExpr(const char* a, const char* b) {
	bool equal = true;
	while (equal) {
		equal = equal && tolowerConstExpr(*a) == tolowerConstExpr(*b);
		if (*a == 0 || *b == 0) break;
		a++;
		b++;
	}
	return equal;
}


// Sadly MSVC can't handle this.
// It works if I only use the first two ifs. The moment I add the rest in, it fails.
// If I comment out the first two and try another single one, then it works as well.
// Ridiculous. I guess it's just taking too long to compute for its taste.
// 
// Later note: This solution wasn't good anyway. Oh well.
static inline constexpr vmAnimMoh_e parseAnimName(const char* animName) {
	if (strisameConstExpr(animName, "fire")) {
		return VM_ANIM_FIRE_MOH;
	} else if (strisameConstExpr(animName, "fire_secondary")) {
		return VM_ANIM_FIRE_SECONDARY_MOH;
	} else if (strisameConstExpr(animName, "idle")) {
		return VM_ANIM_IDLE_MOH;
	} else if (strisameConstExpr(animName, "charge")) {
		return VM_ANIM_CHARGE_MOH;
	} else if (strisameConstExpr(animName, "rechamber")) {
		return VM_ANIM_RECHAMBER_MOH;
	} else if (strisameConstExpr(animName, "reload")) {
		return VM_ANIM_RELOAD_MOH;
	} else if (strisameConstExpr(animName, "reload_single")) { // Don't think this really exists in that anim table. Oh well
		return VM_ANIM_RELOAD_SINGLE_MOH;
	} else if (strisameConstExpr(animName, "reload_end")) {
		return VM_ANIM_RELOAD_END_MOH;
	} else if (strisameConstExpr(animName, "pullout")) {// Don't think this really exists in that anim table. Oh well
		return VM_ANIM_PULLOUT_MOH;
	} else if (strisameConstExpr(animName, "putaway")) {// Don't think this really exists in that anim table. Oh well
		return VM_ANIM_PUTAWAY_MOH;
	} else if (strisameConstExpr(animName, "ladderstep")) {// Don't think this really exists in that anim table. Oh well
		return VM_ANIM_LADDERSTEP_MOH;
	}
	//} else if (!stricmp(animName, "idle0")) {
		//return VM_ANIM_IDLE_0_MOH;
	//} else if (!stricmp(animName, "idle1")) {
		//return VM_ANIM_IDLE_0_MOH;
	//} else if (!stricmp(animName, "idle2")) {
		//return VM_ANIM_IDLE_0_MOH;
	//}
	return VM_ANIM_INVALID_MOH;
}


static constexpr auto playerAnimToVMAnim{ []() constexpr {
	constexpr int size = sizeof(playerAnimNames) / sizeof(playerAnimNames[0]);
	std::array<vmAnimMoh_e, size> retVal{};

	for (int i = 0; i < size; i++) {
		int stringIndex = 0;
		const char* animName = playerAnimNames[i];
		vmAnimMoh_e foundAnim = VM_ANIM_INVALID_MOH;
		//bool lastWasUnderScore = false;
		while (true) { // Find last underscore
			if (animName[stringIndex] == 0) break; // End of line
			foundAnim = parseAnimName(animName + stringIndex);
			if (foundAnim != VM_ANIM_INVALID_MOH) break;
			stringIndex++;
		}
		if (foundAnim == VM_ANIM_INVALID_MOH) foundAnim = VM_ANIM_IDLE_MOH; 
		retVal.data()[i] = foundAnim;
	}

	return retVal;
}() };*/

#endif