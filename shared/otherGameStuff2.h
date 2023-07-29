#ifndef __OTHERGAMESTUFF2_H__
#define __OTHERGAMESTUFF2_H__

#include "anims.h"
#include "demoCut.h"

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



#endif