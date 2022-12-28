#pragma once
#include "demoCut.h"



// JK 2

static netField_t	entityStateFields[] =
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(event), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ NETF(angles2[1]), 0 },
{ NETF(eType), 8 },
{ NETF(torsoAnim), 16 },		// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ NETF(forceFrame), 16 }, //if you have over 65536 frames, then this will explode. Of course if you have that many things then lots of things will probably explode.
{ NETF(eventParm), 8 },
{ NETF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(eFlags), 32 },
{ NETF(bolt1), 8 },
{ NETF(bolt2), GENTITYNUM_BITS },
{ NETF(trickedentindex), 16 }, //See note in PSF
{ NETF(trickedentindex2), 16 },
{ NETF(trickedentindex3), 16 },
{ NETF(trickedentindex4), 16 },
{ NETF(speed), 0 },
{ NETF(fireflag), 2 },
{ NETF(genericenemyindex), 32 }, //Do not change to GENTITYNUM_BITS, used as a time offset for seeker
{ NETF(activeForcePass), 6 },
{ NETF(emplacedOwner), 32 }, //As above, also used as a time value (for electricity render time)
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(weapon), 8 },
{ NETF(clientNum), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDuration), 32 },
{ NETF(apos.trType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(solid), 24 },
{ NETF(owner), GENTITYNUM_BITS },
{ NETF(teamowner), 8 },
{ NETF(shouldtarget), 1 },
{ NETF(powerups), 16 },
{ NETF(modelGhoul2), 5 },
{ NETF(g2radius), 8 },
{ NETF(modelindex), -8 },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(loopSound), 8 },
{ NETF(generic1), 8 },
{ NETF(origin2[2]), 0 },
{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(modelindex2), 8 },
{ NETF(angles[0]), 0 },
{ NETF(time), 32 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },
{ NETF(time2), 32 },
{ NETF(angles[2]), 0 },
{ NETF(angles2[0]), 0 },
{ NETF(angles2[2]), 0 },
{ NETF(constantLight), 32 },
{ NETF(frame), 16 },
{ NETF(saberInFlight), 1 },
{ NETF(saberEntityNum), GENTITYNUM_BITS },
{ NETF(saberMove), 8 },
{ NETF(forcePowersActive), 32 },
{ NETF(isJediMaster), 1 }
};

//for 1.02 demos with 15 protocol
//-{ NETF(modelGhoul2), 5 },
//+{ NETF(modelGhoul2), 4 },
static netField_t	entityStateFields15[] =
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(event), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ NETF(angles2[1]), 0 },
{ NETF(eType), 8 },
{ NETF(torsoAnim), 16 },		// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ NETF(forceFrame), 16 }, //if you have over 65536 frames, then this will explode. Of course if you have that many things then lots of things will probably explode.
{ NETF(eventParm), 8 },
{ NETF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(eFlags), 32 },
{ NETF(bolt1), 8 },
{ NETF(bolt2), GENTITYNUM_BITS },
{ NETF(trickedentindex), 16 }, //See note in PSF
{ NETF(trickedentindex2), 16 },
{ NETF(trickedentindex3), 16 },
{ NETF(trickedentindex4), 16 },
{ NETF(speed), 0 },
{ NETF(fireflag), 2 },
{ NETF(genericenemyindex), 32 }, //Do not change to GENTITYNUM_BITS, used as a time offset for seeker
{ NETF(activeForcePass), 6 },
{ NETF(emplacedOwner), 32 }, //As above, also used as a time value (for electricity render time)
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(weapon), 8 },
{ NETF(clientNum), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDuration), 32 },
{ NETF(apos.trType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(solid), 24 },
{ NETF(owner), GENTITYNUM_BITS },
{ NETF(teamowner), 8 },
{ NETF(shouldtarget), 1 },
{ NETF(powerups), 16 },
{ NETF(modelGhoul2), 4 },
{ NETF(g2radius), 8 },
{ NETF(modelindex), -8 },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(loopSound), 8 },
{ NETF(generic1), 8 },
{ NETF(origin2[2]), 0 },
{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(modelindex2), 8 },
{ NETF(angles[0]), 0 },
{ NETF(time), 32 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },
{ NETF(time2), 32 },
{ NETF(angles[2]), 0 },
{ NETF(angles2[0]), 0 },
{ NETF(angles2[2]), 0 },
{ NETF(constantLight), 32 },
{ NETF(frame), 16 },
{ NETF(saberInFlight), 1 },
{ NETF(saberEntityNum), GENTITYNUM_BITS },
{ NETF(saberMove), 8 },
{ NETF(forcePowersActive), 32 },
{ NETF(isJediMaster), 1 }
};


/*
============================================================================

plyer_state_t communication

============================================================================
*/

static netField_t	playerStateFields[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
{ PSF(weaponChargeSubtractTime), 32 }, //? really need 32 bits??
{ PSF(origin[2]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(pm_time), -16 },
{ PSF(eventSequence), 16 },
{ PSF(torsoAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(torsoTimer), 16 },
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(legsTimer), 16 },
{ PSF(movementDir), 4 },
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(pm_flags), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(weaponstate), 4 },
{ PSF(eFlags), 32 },
{ PSF(externalEvent), 10 },
{ PSF(gravity), 16 },
{ PSF(speed), 16 },
{ PSF(basespeed), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(externalEventParm), 8 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), 8 },
{ PSF(damageCount), 8 },
{ PSF(damageType), 2 },
{ PSF(generic1), 8 },
{ PSF(pm_type), 8 },
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(eventParms[0]), -16 },
{ PSF(eventParms[1]), 8 },
{ PSF(clientNum), 8 },
{ PSF(weapon), 5 },
{ PSF(viewangles[2]), 0 },
{ PSF(jumppad_ent), 10 },
{ PSF(loopSound), 16 },

// NOTENOTE Are all of these necessary?
{ PSF(zoomMode), 2 },
{ PSF(zoomTime), 32 },
{ PSF(zoomLocked), 1 },
{ PSF(zoomFov), 8 },

{ PSF(fd.forcePowersActive), 32 },
{ PSF(fd.forceMindtrickTargetIndex), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex2), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex3), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex4), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceJumpZStart), 0 },
{ PSF(fd.forcePowerSelected), 8 },
{ PSF(fd.forcePowersKnown), 32 },
{ PSF(fd.forcePower), 8 },
{ PSF(fd.forceSide), 2 }, //so we know if we should apply greyed out shaders to dark/light force enlightenment
{ PSF(fd.sentryDeployed), 1 },
{ PSF(fd.forcePowerLevel[FP_LEVITATION]), 2 }, //unfortunately we need this for fall damage calculation (client needs to know the distance for the fall noise)
{ PSF(fd.forcePowerLevel[FP_SEE]), 2 }, //needed for knowing when to display players through walls
{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(activeForcePass), 6 },
{ PSF(hasDetPackPlanted), 1 },
{ PSF(emplacedIndex), GENTITYNUM_BITS },
{ PSF(fd.forceRageRecoveryTime), 32 },
{ PSF(rocketLockIndex), 8 }, //should never exceed MAX_CLIENTS
{ PSF(rocketLockTime), 32 },
{ PSF(rocketTargetTime), 32 },
{ PSF(holocronBits), 32 },
{ PSF(isJediMaster), 1 },
{ PSF(forceRestricted), 1 },
{ PSF(trueJedi), 1 },
{ PSF(trueNonJedi), 1 },
{ PSF(fallingToDeath), 32 },
{ PSF(electrifyTime), 32 },

{ PSF(fd.forcePowerDebounce[FP_LEVITATION]), 32 },

{ PSF(saberMove), 32 }, //This value sometimes exceeds the max LS_ value and gets set to a crazy amount, so it needs 32 bits
{ PSF(saberActive), 1 },
{ PSF(saberInFlight), 1 },
{ PSF(saberBlocked), 8 },
{ PSF(saberEntityNum), GENTITYNUM_BITS }, //Also used for channel tracker storage, but should never exceed entity number
{ PSF(saberCanThrow), 1 },
{ PSF(forceHandExtend), 8 },
{ PSF(forceDodgeAnim), 16 },
{ PSF(fd.saberAnimLevel), 2 },
{ PSF(fd.saberDrawAnimLevel), 2 },
{ PSF(saberAttackChainCount), 4 },
{ PSF(saberHolstered), 1 },
{ PSF(usingATST), 1 },
{ PSF(atstAltFire), 1 },

{ PSF(duelIndex), GENTITYNUM_BITS },
{ PSF(duelTime), 32 },
{ PSF(duelInProgress), 1 },

{ PSF(saberLockTime), 32 },
{ PSF(saberLockEnemy), GENTITYNUM_BITS },
{ PSF(saberLockFrame), 16 },
{ PSF(saberLockAdvance), 1 },

{ PSF(inAirAnim), 1 }, //just transmit it for the sake of knowing right when on the client to play a land anim, it's only 1 bit
{ PSF(dualBlade), 1 },

{ PSF(lastHitLoc[2]), 0 },
{ PSF(lastHitLoc[0]), 0 },
{ PSF(lastHitLoc[1]), 0 } //currently only used so client knows to orient disruptor disintegration.. seems a bit much for just that though.
};

//for 1.02 demos with 15 protocol
//-{ PSF(forceRestricted), 1 },
//-{ PSF(trueJedi), 1 },
//-{ PSF(trueNonJedi), 1 },
static netField_t	playerStateFields15[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(weaponChargeTime), 32 }, //? really need 32 bits??
{ PSF(weaponChargeSubtractTime), 32 }, //? really need 32 bits??
{ PSF(origin[2]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(pm_time), -16 },
{ PSF(eventSequence), 16 },
{ PSF(torsoAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(torsoTimer), 16 },
{ PSF(legsAnim), 16 },			// Maximum number of animation sequences is 2048.  Top bit is reserved for the togglebit
{ PSF(legsTimer), 16 },
{ PSF(movementDir), 4 },
{ PSF(events[0]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(events[1]), 10 },			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)
{ PSF(pm_flags), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(weaponstate), 4 },
{ PSF(eFlags), 32 },
{ PSF(externalEvent), 10 },
{ PSF(gravity), 16 },
{ PSF(speed), 16 },
{ PSF(basespeed), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(externalEventParm), 8 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), 8 },
{ PSF(damageCount), 8 },
{ PSF(damageType), 2 },
{ PSF(generic1), 8 },
{ PSF(pm_type), 8 },
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(eventParms[0]), -16 },
{ PSF(eventParms[1]), 8 },
{ PSF(clientNum), 8 },
{ PSF(weapon), 5 },
{ PSF(viewangles[2]), 0 },
{ PSF(jumppad_ent), 10 },
{ PSF(loopSound), 16 },

// NOTENOTE Are all of these necessary?
{ PSF(zoomMode), 2 },
{ PSF(zoomTime), 32 },
{ PSF(zoomLocked), 1 },
{ PSF(zoomFov), 8 },

{ PSF(fd.forcePowersActive), 32 },
{ PSF(fd.forceMindtrickTargetIndex), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex2), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex3), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceMindtrickTargetIndex4), 16 }, //NOTE: Not just an index, used as a (1 << val) bitflag for up to 16 clients
{ PSF(fd.forceJumpZStart), 0 },
{ PSF(fd.forcePowerSelected), 8 },
{ PSF(fd.forcePowersKnown), 32 },
{ PSF(fd.forcePower), 8 },
{ PSF(fd.forceSide), 2 }, //so we know if we should apply greyed out shaders to dark/light force enlightenment
{ PSF(fd.sentryDeployed), 1 },
{ PSF(fd.forcePowerLevel[FP_LEVITATION]), 2 }, //unfortunately we need this for fall damage calculation (client needs to know the distance for the fall noise)
{ PSF(fd.forcePowerLevel[FP_SEE]), 2 }, //needed for knowing when to display players through walls
{ PSF(genericEnemyIndex), 32 }, //NOTE: This isn't just an index all the time, it's often used as a time value, and thus needs 32 bits
{ PSF(activeForcePass), 6 },
{ PSF(hasDetPackPlanted), 1 },
{ PSF(emplacedIndex), GENTITYNUM_BITS },
{ PSF(fd.forceRageRecoveryTime), 32 },
{ PSF(rocketLockIndex), 8 }, //should never exceed MAX_CLIENTS
{ PSF(rocketLockTime), 32 },
{ PSF(rocketTargetTime), 32 },
{ PSF(holocronBits), 32 },
{ PSF(isJediMaster), 1 },
{ PSF(fallingToDeath), 32 },
{ PSF(electrifyTime), 32 },

{ PSF(fd.forcePowerDebounce[FP_LEVITATION]), 32 },

{ PSF(saberMove), 32 }, //This value sometimes exceeds the max LS_ value and gets set to a crazy amount, so it needs 32 bits
{ PSF(saberActive), 1 },
{ PSF(saberInFlight), 1 },
{ PSF(saberBlocked), 8 },
{ PSF(saberEntityNum), GENTITYNUM_BITS }, //Also used for channel tracker storage, but should never exceed entity number
{ PSF(saberCanThrow), 1 },
{ PSF(forceHandExtend), 8 },
{ PSF(forceDodgeAnim), 16 },
{ PSF(fd.saberAnimLevel), 2 },
{ PSF(fd.saberDrawAnimLevel), 2 },
{ PSF(saberAttackChainCount), 4 },
{ PSF(saberHolstered), 1 },
{ PSF(usingATST), 1 },
{ PSF(atstAltFire), 1 },

{ PSF(duelIndex), GENTITYNUM_BITS },
{ PSF(duelTime), 32 },
{ PSF(duelInProgress), 1 },

{ PSF(saberLockTime), 32 },
{ PSF(saberLockEnemy), GENTITYNUM_BITS },
{ PSF(saberLockFrame), 16 },
{ PSF(saberLockAdvance), 1 },

{ PSF(inAirAnim), 1 }, //just transmit it for the sake of knowing right when on the client to play a land anim, it's only 1 bit
{ PSF(dualBlade), 1 },

{ PSF(lastHitLoc[2]), 0 },
{ PSF(lastHitLoc[0]), 0 },
{ PSF(lastHitLoc[1]), 0 } //currently only used so client knows to orient disruptor disintegration.. seems a bit much for just that though.
};








// Q3

static netField_t	entityStateFieldsQ3DM26[] =
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(event), 10 },
{ NETF(angles2[1]), 0 },
{ NETF(eType), 8 },
{ NETF(torsoAnim), 8 },
{ NETF(eventParm), 8 },
{ NETF(legsAnim), 8 },
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(eFlags), 19 },
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(weapon), 8 },
{ NETF(clientNum), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDuration), 32 },
{ NETF(apos.trType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(solid), 24 },
{ NETF(powerups), 16 },
{ NETF(modelindex), 8 },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(loopSound), 8 },
{ NETF(generic1), 8 },
{ NETF(origin2[2]), 0 },
{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(modelindex2), 8 },
{ NETF(angles[0]), 0 },
{ NETF(time), 32 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },
{ NETF(time2), 32 },
{ NETF(angles[2]), 0 },
{ NETF(angles2[0]), 0 },
{ NETF(angles2[2]), 0 },
{ NETF(constantLight), 32 },
{ NETF(frame), 16 }
};

static netField_t	playerStateFieldsQ3DM26[] =
{
{ PSF(commandTime), 32 },
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(legsTimer), 8 },
{ PSF(pm_time), -16 },
{ PSF(eventSequence), 16 },
{ PSF(torsoAnim), 8 },
{ PSF(movementDir), 4 },
{ PSF(events[0]), 8 },
{ PSF(legsAnim), 8 },
{ PSF(events[1]), 8 },
{ PSF(pm_flags), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(weaponstate), 4 },
{ PSF(eFlags), 16 },
{ PSF(externalEvent), 10 },
{ PSF(gravity), 16 },
{ PSF(speed), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(externalEventParm), 8 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), 8 },
{ PSF(damageCount), 8 },
{ PSF(generic1), 8 },
{ PSF(pm_type), 8 },
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(torsoTimer), 12 },
{ PSF(eventParms[0]), 8 },
{ PSF(eventParms[1]), 8 },
{ PSF(clientNum), 8 },
{ PSF(weapon), 5 },
{ PSF(viewangles[2]), 0 },
{ PSF(grapplePoint[0]), 0 },
{ PSF(grapplePoint[1]), 0 },
{ PSF(grapplePoint[2]), 0 },
{ PSF(jumppad_ent), 10 },
{ PSF(loopSound), 16 }
};