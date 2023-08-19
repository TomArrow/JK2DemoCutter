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
{ PSF(fov), 0, netFieldType_t::regular },
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
{ PSF(fov), 0, netFieldType_t::regular },
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

	// No client attacker
	{"was burned to a crisp",{qfalse,qfalse,{},MOD_LAVA_MOH}}, // Could also be MOD_SLIME
	{"cratered",{qfalse,qfalse,{},MOD_FALLING_MOH}},
	{"blew up",{qfalse,qfalse,{},MOD_EXPLOSION_MOH}}, // Could also be MOD_GRENADE
	{"caught a rocket",{qfalse,qfalse,{},MOD_ROCKET_MOH}},
	{"was shot in the",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"was shot",{qfalse,qfalse,{},MOD_BULLET_MOH}}, // Could also be MOD_FAST_BULLET
	{"died",{qfalse,qfalse,{},MOD_NONE_MOH}},

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


};

static const tsl::htrie_map<char, int> mohKillLocationArray = {
	{"head",0},
	{"helmet",1},
	{"neck",2},
	{"upper torso",3},
	{"middle torso",4},
	{"lower torso",5},
	{"pelvis",6},
	{"upper right arm",7},
	{"upper left arm",8},
	{"upper right leg",9},
	{"upper left leg",10},
	{"lower right arm",11},
	{"lower left arm",12},
	{"lower right leg",13},
	{"lower left leg",14},
	{"right hand",15},
	{"left hand",16},
	{"right foot",17},
	{"left foot",18},
};




#endif