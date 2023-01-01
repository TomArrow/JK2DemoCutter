// Straight 1:1 Copy from Jomme

#ifndef __ANIMS_H__
#define __ANIMS_H__
// playerAnimations

#define BOOT_ANIMS 16

typedef enum //# animNumber_e
{

	BOTH_1CRUFTFORGIL_GENERAL = 0,		//# G2 cannot have a reverse anim at beginning of file
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	FACE_TALK0_GENERAL,			//# silent
	FACE_TALK1_GENERAL,			//# quiet
	FACE_TALK2_GENERAL,			//# semi-quiet
	FACE_TALK3_GENERAL,			//# semi-loud
	FACE_TALK4_GENERAL,			//# loud
	FACE_ALERT_GENERAL,				//# 
	FACE_SMILE_GENERAL,				//# 
	FACE_FROWN_GENERAL,				//# 
	FACE_DEAD_GENERAL,				//# 

	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1_GENERAL,		//# First Death anim
	BOTH_DEATH2_GENERAL,			//# Second Death anim
	BOTH_DEATH3_GENERAL,			//# Third Death anim
	BOTH_DEATH4_GENERAL,			//# Fourth Death anim
	BOTH_DEATH5_GENERAL,			//# Fifth Death anim
	BOTH_DEATH6_GENERAL,			//# Sixth Death anim
	BOTH_DEATH7_GENERAL,			//# Seventh Death anim
	BOTH_DEATH8_GENERAL,			//# 
	BOTH_DEATH9_GENERAL,			//# 
	BOTH_DEATH10_GENERAL,			//# 
	BOTH_DEATH11_GENERAL,			//#
	BOTH_DEATH12_GENERAL,			//# 
	BOTH_DEATH13_GENERAL,			//# 
	BOTH_DEATH14_GENERAL,			//# 
	BOTH_DEATH15_GENERAL,			//# 
	BOTH_DEATH16_GENERAL,			//# 
	BOTH_DEATH17_GENERAL,			//# 
	BOTH_DEATH18_GENERAL,			//# 
	BOTH_DEATH19_GENERAL,			//# 
	BOTH_DEATH20_GENERAL,			//# 
	BOTH_DEATH21_GENERAL,			//# 
	BOTH_DEATH22_GENERAL,			//# 
	BOTH_DEATH23_GENERAL,			//# 
	BOTH_DEATH24_GENERAL,			//# 
	BOTH_DEATH25_GENERAL,			//# 

	BOTH_DEATHFORWARD1_GENERAL,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2_GENERAL,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3_GENERAL,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1_GENERAL,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2_GENERAL,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE_GENERAL,		//# Idle while close to death
	BOTH_LYINGDEATH1_GENERAL,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1_GENERAL,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1_GENERAL,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR_GENERAL,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND_GENERAL,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL_GENERAL,		//# Death anim from a roll
	BOTH_DEATH_FLIP_GENERAL,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R_GENERAL,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L_GENERAL,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180_GENERAL,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP_GENERAL,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN_GENERAL,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN_GENERAL,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP_GENERAL,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED_GENERAL,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1_GENERAL,				//# First Death finished pose
	BOTH_DEAD2_GENERAL,				//# Second Death finished pose
	BOTH_DEAD3_GENERAL,				//# Third Death finished pose
	BOTH_DEAD4_GENERAL,				//# Fourth Death finished pose
	BOTH_DEAD5_GENERAL,				//# Fifth Death finished pose
	BOTH_DEAD6_GENERAL,				//# Sixth Death finished pose
	BOTH_DEAD7_GENERAL,				//# Seventh Death finished pose
	BOTH_DEAD8_GENERAL,				//# 
	BOTH_DEAD9_GENERAL,				//# 
	BOTH_DEAD10_GENERAL,			//# 
	BOTH_DEAD11_GENERAL,			//#
	BOTH_DEAD12_GENERAL,			//# 
	BOTH_DEAD13_GENERAL,			//# 
	BOTH_DEAD14_GENERAL,			//# 
	BOTH_DEAD15_GENERAL,			//# 
	BOTH_DEAD16_GENERAL,			//# 
	BOTH_DEAD17_GENERAL,			//# 
	BOTH_DEAD18_GENERAL,			//# 
	BOTH_DEAD19_GENERAL,			//# 
	BOTH_DEAD20_GENERAL,			//# 
	BOTH_DEAD21_GENERAL,			//# 
	BOTH_DEAD22_GENERAL,			//# 
	BOTH_DEAD23_GENERAL,			//# 
	BOTH_DEAD24_GENERAL,			//# 
	BOTH_DEAD25_GENERAL,			//# 
	BOTH_DEADFORWARD1_GENERAL,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2_GENERAL,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_GENERAL,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2_GENERAL,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1_GENERAL,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_GENERAL,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND_GENERAL,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1_GENERAL,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2_GENERAL,		//# React to being shot from Second Death finished pose
	BOTH_DEADFLOP3_GENERAL,		//# React to being shot from Third Death finished pose
	BOTH_DEADFLOP4_GENERAL,		//# React to being shot from Fourth Death finished pose
	BOTH_DEADFLOP5_GENERAL,		//# React to being shot from Fifth Death finished pose 
	BOTH_DEADFORWARD1_FLOP_GENERAL,		//# React to being shot First thrown forward death finished pose
	BOTH_DEADFORWARD2_FLOP_GENERAL,		//# React to being shot Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_FLOP_GENERAL,	//# React to being shot First thrown backward death finished pose
	BOTH_DEADBACKWARD2_FLOP_GENERAL,	//# React to being shot Second thrown backward death finished pose
	BOTH_LYINGDEAD1_FLOP_GENERAL,		//# React to being shot Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_FLOP_GENERAL,		//# React to being shot Stumble forward death finished pose
	BOTH_FALLDEAD1_FLOP_GENERAL,	//# React to being shot Fall forward and splat death finished pose
	BOTH_DISMEMBER_HEAD1_GENERAL,	//#
	BOTH_DISMEMBER_TORSO1_GENERAL,	//#
	BOTH_DISMEMBER_LLEG_GENERAL,	//#
	BOTH_DISMEMBER_RLEG_GENERAL,	//#
	BOTH_DISMEMBER_RARM_GENERAL,	//#
	BOTH_DISMEMBER_LARM_GENERAL,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1_GENERAL,				//# First take pain anim
	BOTH_PAIN2_GENERAL,				//# Second take pain anim
	BOTH_PAIN3_GENERAL,				//# Third take pain anim
	BOTH_PAIN4_GENERAL,				//# Fourth take pain anim
	BOTH_PAIN5_GENERAL,				//# Fifth take pain anim - from behind
	BOTH_PAIN6_GENERAL,				//# Sixth take pain anim - from behind
	BOTH_PAIN7_GENERAL,				//# Seventh take pain anim - from behind
	BOTH_PAIN8_GENERAL,				//# Eigth take pain anim - from behind
	BOTH_PAIN9_GENERAL,				//# 
	BOTH_PAIN10_GENERAL,			//# 
	BOTH_PAIN11_GENERAL,			//# 
	BOTH_PAIN12_GENERAL,			//# 
	BOTH_PAIN13_GENERAL,			//# 
	BOTH_PAIN14_GENERAL,			//# 
	BOTH_PAIN15_GENERAL,			//# 
	BOTH_PAIN16_GENERAL,			//# 
	BOTH_PAIN17_GENERAL,			//# 
	BOTH_PAIN18_GENERAL,			//# 
	BOTH_PAIN19_GENERAL,			//# 
	BOTH_PAIN20_GENERAL,			//# GETTING SHOCKED

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1_GENERAL,			//# Attack with stun baton
	BOTH_ATTACK2_GENERAL,			//# Attack with one-handed pistol
	BOTH_ATTACK2IDLE1_GENERAL,		//# Idle with one-handed pistol
	BOTH_ATTACK3_GENERAL,			//# Attack with blaster rifle
	BOTH_ATTACK4_GENERAL,			//# Attack with disruptor
	BOTH_ATTACK5_GENERAL,			//# JK2: Attack with bow caster_GENERAL, JKA: Another Rancor Attack
	BOTH_ATTACK6_GENERAL,			//# JK2: Attack with ???_GENERAL, JKA: Yet Another Rancor Attack
	BOTH_ATTACK7_GENERAL,			//# JK2: Attack with ???_GENERAL, JKA: Yet Another Rancor Attack
	BOTH_ATTACK8_GENERAL,			//# Attack with ???
	BOTH_ATTACK9_GENERAL,			//# Attack with rocket launcher
	BOTH_ATTACK10_GENERAL,			//# Attack with thermal det
	BOTH_ATTACK11_GENERAL,			//# JK2: Attack with laser trap_GENERAL, JKA: "Attack" with tripmine and detpack
	BOTH_ATTACK12_GENERAL,			//# Attack with detpack
	BOTH_MELEE1_GENERAL,			//# First melee attack
	BOTH_MELEE2_GENERAL,			//# Second melee attack
	BOTH_MELEE3_GENERAL,			//# Third melee attack
	BOTH_MELEE4_GENERAL,			//# Fourth melee attack
	BOTH_MELEE5_GENERAL,			//# Fifth melee attack
	BOTH_MELEE6_GENERAL,			//# Sixth melee attack
	BOTH_THERMAL_READY_GENERAL,		//# pull back with thermal
	BOTH_THERMAL_THROW_GENERAL,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 1
	BOTH_A1_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T1__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T1__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T1__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T1_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T1_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T1_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T1_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T1_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T1_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T1_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T1__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T1_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B1_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR____GENERAL,	//# Deflection toward BR
	BOTH_D1__R____GENERAL,	//# Deflection toward R
	BOTH_D1_TR____GENERAL,	//# Deflection toward TR
	BOTH_D1_TL____GENERAL,	//# Deflection toward TL
	BOTH_D1__L____GENERAL,	//# Deflection toward L
	BOTH_D1_BL____GENERAL,	//# Deflection toward BL
	BOTH_D1_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T2__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T2__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T2__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T2_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T2_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T2_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T2_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T2_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T2_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T2_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T2__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T2_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B2_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR____GENERAL,	//# Deflection toward BR
	BOTH_D2__R____GENERAL,	//# Deflection toward R
	BOTH_D2_TR____GENERAL,	//# Deflection toward TR
	BOTH_D2_TL____GENERAL,	//# Deflection toward TL
	BOTH_D2__L____GENERAL,	//# Deflection toward L
	BOTH_D2_BL____GENERAL,	//# Deflection toward BL
	BOTH_D2_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T3__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T3__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T3__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T3_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T3_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T3_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T3_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T3_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T3_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T3_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T3__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T3_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B3_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR____GENERAL,	//# Deflection toward BR
	BOTH_D3__R____GENERAL,	//# Deflection toward R
	BOTH_D3_TR____GENERAL,	//# Deflection toward TR
	BOTH_D3_TL____GENERAL,	//# Deflection toward TL
	BOTH_D3__L____GENERAL,	//# Deflection toward L
	BOTH_D3_BL____GENERAL,	//# Deflection toward BL
	BOTH_D3_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T4__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T4__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T4__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T4_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T4_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T4_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T4_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T4_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T4_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T4_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T4__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T4_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B4_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR____GENERAL,	//# Deflection toward BR
	BOTH_D4__R____GENERAL,	//# Deflection toward R
	BOTH_D4_TR____GENERAL,	//# Deflection toward TR
	BOTH_D4_TL____GENERAL,	//# Deflection toward TL
	BOTH_D4__L____GENERAL,	//# Deflection toward L
	BOTH_D4_BL____GENERAL,	//# Deflection toward BL
	BOTH_D4_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T5__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T5__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T5__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T5_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T5_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T5_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T5_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T5_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T5_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T5_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T5__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T5_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B5_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR____GENERAL,	//# Deflection toward BR
	BOTH_D5__R____GENERAL,	//# Deflection toward R
	BOTH_D5_TR____GENERAL,	//# Deflection toward TR
	BOTH_D5_TL____GENERAL,	//# Deflection toward TL
	BOTH_D5__L____GENERAL,	//# Deflection toward L
	BOTH_D5_BL____GENERAL,	//# Deflection toward BL
	BOTH_D5_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 6
	BOTH_A6_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A6__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A6__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A6_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A6_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A6_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A6_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T6_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T6_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T6_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T6_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T6__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T6__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T6__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T6__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T6_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T6_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T6_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T6_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T6_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T6_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T6_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T6_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T6_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T6_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T6_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T6_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T6__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T6__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T6__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T6_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T6_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T6_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T6_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T6_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T6_TR_BR)
	BOTH_T6_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T6_T__BR)
	BOTH_T6__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T6_BR__R)
	BOTH_T6__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T6_T___R)
	BOTH_T6_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T6__R_TR)
	BOTH_T6_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T6_T__TR)
	BOTH_T6_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T6__R_TL)
	BOTH_T6_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T6_TR_TL)
	BOTH_T6_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T6_T__TL)
	BOTH_T6_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T6__L_TL)
	BOTH_T6__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T6_TR__L)
	BOTH_T6__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T6_T___L)
	BOTH_T6__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T6_BL__L)
	BOTH_T6_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T6_T__BL)
	BOTH_T6_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T6_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S6_S6_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S6_S6__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S6_S6__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S6_S6_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S6_S6_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S6_S6_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S6_S6_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R6_B__S6_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R6__L_S6_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R6__R_S6_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R6_TL_S6_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R6_BR_S6_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R6_BL_S6_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R6_TR_S6_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B6_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B6__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B6_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B6_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B6_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B6__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B6_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D6_BR____GENERAL,	//# Deflection toward BR
	BOTH_D6__R____GENERAL,	//# Deflection toward R
	BOTH_D6_TR____GENERAL,	//# Deflection toward TR
	BOTH_D6_TL____GENERAL,	//# Deflection toward TL
	BOTH_D6__L____GENERAL,	//# Deflection toward L
	BOTH_D6_BL____GENERAL,	//# Deflection toward BL
	BOTH_D6_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 7
	BOTH_A7_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A7__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A7__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A7_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A7_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A7_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A7_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T7_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T7_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T7_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T7_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T7__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T7__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T7__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T7__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T7_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T7_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T7_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T7_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T7_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T7_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T7_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T7_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T7_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T7_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T7_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T7_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T7__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T7__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T7__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T7_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T7_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T7_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T7_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T7_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T7_TR_BR)
	BOTH_T7_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T7_T__BR)
	BOTH_T7__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T7_BR__R)
	BOTH_T7__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T7_T___R)
	BOTH_T7_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T7__R_TR)
	BOTH_T7_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T7_T__TR)
	BOTH_T7_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T7__R_TL)
	BOTH_T7_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T7_TR_TL)
	BOTH_T7_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T7_T__TL)
	BOTH_T7_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T7__L_TL)
	BOTH_T7__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T7_TR__L)
	BOTH_T7__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T7_T___L)
	BOTH_T7__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T7_BL__L)
	BOTH_T7_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T7_T__BL)
	BOTH_T7_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T7_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S7_S7_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S7_S7__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S7_S7__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S7_S7_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S7_S7_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S7_S7_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S7_S7_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R7_B__S7_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R7__L_S7_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R7__R_S7_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R7_TL_S7_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R7_BR_S7_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R7_BL_S7_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R7_TR_S7_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B7_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B7__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B7_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B7_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B7_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B7__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B7_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D7_BR____GENERAL,	//# Deflection toward BR
	BOTH_D7__R____GENERAL,	//# Deflection toward R
	BOTH_D7_TR____GENERAL,	//# Deflection toward TR
	BOTH_D7_TL____GENERAL,	//# Deflection toward TL
	BOTH_D7__L____GENERAL,	//# Deflection toward L
	BOTH_D7_BL____GENERAL,	//# Deflection toward BL
	BOTH_D7_B_____GENERAL,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T__GENERAL,	//# Block shot/saber top
	BOTH_P1_S1_TR_GENERAL,	//# Block shot/saber top right
	BOTH_P1_S1_TL_GENERAL,	//# Block shot/saber top left
	BOTH_P1_S1_BL_GENERAL,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR_GENERAL,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T__GENERAL,	//# knockaway saber top
	BOTH_K1_S1_TR_GENERAL,	//# knockaway saber top right
	BOTH_K1_S1_TL_GENERAL,	//# knockaway saber top left
	BOTH_K1_S1_BL_GENERAL,	//# knockaway saber bottom left
	BOTH_K1_S1_B__GENERAL,	//# knockaway saber bottom
	BOTH_K1_S1_BR_GENERAL,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1_GENERAL,	//# BR attack knocked away
	BOTH_V1__R_S1_GENERAL,	//# R attack knocked away
	BOTH_V1_TR_S1_GENERAL,	//# TR attack knocked away
	BOTH_V1_T__S1_GENERAL,	//# T attack knocked away
	BOTH_V1_TL_S1_GENERAL,	//# TL attack knocked away
	BOTH_V1__L_S1_GENERAL,	//# L attack knocked away
	BOTH_V1_BL_S1_GENERAL,	//# BL attack knocked away
	BOTH_V1_B__S1_GENERAL,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T__GENERAL,	//# saber knocked down from top parry
	BOTH_H1_S1_TR_GENERAL,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL_GENERAL,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL_GENERAL,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B__GENERAL,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR_GENERAL,	//# saber knocked up-left from BR parry
	//Dual Saber parry anims
	BOTH_P6_S6_T__GENERAL,	//# Block shot/saber top
	BOTH_P6_S6_TR_GENERAL,	//# Block shot/saber top right
	BOTH_P6_S6_TL_GENERAL,	//# Block shot/saber top left
	BOTH_P6_S6_BL_GENERAL,	//# Block shot/saber bottom left
	BOTH_P6_S6_BR_GENERAL,	//# Block shot/saber bottom right
	//Dual Saber knockaway
	BOTH_K6_S6_T__GENERAL,	//# knockaway saber top
	BOTH_K6_S6_TR_GENERAL,	//# knockaway saber top right
	BOTH_K6_S6_TL_GENERAL,	//# knockaway saber top left
	BOTH_K6_S6_BL_GENERAL,	//# knockaway saber bottom left
	BOTH_K6_S6_B__GENERAL,	//# knockaway saber bottom
	BOTH_K6_S6_BR_GENERAL,	//# knockaway saber bottom right
	//Dual Saber attack knocked away
	BOTH_V6_BR_S6_GENERAL,	//# BR attack knocked away
	BOTH_V6__R_S6_GENERAL,	//# R attack knocked away
	BOTH_V6_TR_S6_GENERAL,	//# TR attack knocked away
	BOTH_V6_T__S6_GENERAL,	//# T attack knocked away
	BOTH_V6_TL_S6_GENERAL,	//# TL attack knocked away
	BOTH_V6__L_S6_GENERAL,	//# L attack knocked away
	BOTH_V6_BL_S6_GENERAL,	//# BL attack knocked away
	BOTH_V6_B__S6_GENERAL,	//# B attack knocked away
	//Dual Saber parry broken
	BOTH_H6_S6_T__GENERAL,	//# saber knocked down from top parry
	BOTH_H6_S6_TR_GENERAL,	//# saber knocked down-left from TR parry
	BOTH_H6_S6_TL_GENERAL,	//# saber knocked down-right from TL parry
	BOTH_H6_S6_BL_GENERAL,	//# saber knocked up-right from BL parry
	BOTH_H6_S6_B__GENERAL,	//# saber knocked up over head from ready?
	BOTH_H6_S6_BR_GENERAL,	//# saber knocked up-left from BR parry
	//SaberStaff parry anims
	BOTH_P7_S7_T__GENERAL,	//# Block shot/saber top
	BOTH_P7_S7_TR_GENERAL,	//# Block shot/saber top right
	BOTH_P7_S7_TL_GENERAL,	//# Block shot/saber top left
	BOTH_P7_S7_BL_GENERAL,	//# Block shot/saber bottom left
	BOTH_P7_S7_BR_GENERAL,	//# Block shot/saber bottom right
	//SaberStaff knockaway
	BOTH_K7_S7_T__GENERAL,	//# knockaway saber top
	BOTH_K7_S7_TR_GENERAL,	//# knockaway saber top right
	BOTH_K7_S7_TL_GENERAL,	//# knockaway saber top left
	BOTH_K7_S7_BL_GENERAL,	//# knockaway saber bottom left
	BOTH_K7_S7_B__GENERAL,	//# knockaway saber bottom
	BOTH_K7_S7_BR_GENERAL,	//# knockaway saber bottom right
	//SaberStaff attack knocked away
	BOTH_V7_BR_S7_GENERAL,	//# BR attack knocked away
	BOTH_V7__R_S7_GENERAL,	//# R attack knocked away
	BOTH_V7_TR_S7_GENERAL,	//# TR attack knocked away
	BOTH_V7_T__S7_GENERAL,	//# T attack knocked away
	BOTH_V7_TL_S7_GENERAL,	//# TL attack knocked away
	BOTH_V7__L_S7_GENERAL,	//# L attack knocked away
	BOTH_V7_BL_S7_GENERAL,	//# BL attack knocked away
	BOTH_V7_B__S7_GENERAL,	//# B attack knocked away
	//SaberStaff parry broken
	BOTH_H7_S7_T__GENERAL,	//# saber knocked down from top parry
	BOTH_H7_S7_TR_GENERAL,	//# saber knocked down-left from TR parry
	BOTH_H7_S7_TL_GENERAL,	//# saber knocked down-right from TL parry
	BOTH_H7_S7_BL_GENERAL,	//# saber knocked up-right from BL parry
	BOTH_H7_S7_B__GENERAL,	//# saber knocked up over head from ready?
	BOTH_H7_S7_BR_GENERAL,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	//* #sep BOTH_ SABER LOCKED ANIMS
	//BOTH_(DL_GENERAL, S_GENERAL, ST)_(DL_GENERAL, S_GENERAL, ST)_(T_GENERAL, S)_(L_GENERAL, B_GENERAL, SB)_1(_W_GENERAL, _L)
//===Single locks==================================================================
//SINGLE vs. DUAL
	//side locks - I'm using a single and they're using dual
	BOTH_LK_S_DL_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_DL_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_DL_S_L_1_GENERAL,		//lock if I'm using single vs. a dual
	BOTH_LK_S_DL_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_DL_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_S_DL_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_DL_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_DL_T_L_1_GENERAL,		//lock if I'm using single vs. a dual
	BOTH_LK_S_DL_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_DL_T_SB_1_W_GENERAL,	//super break I won
//SINGLE vs. STAFF
	//side locks
	BOTH_LK_S_ST_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_ST_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_ST_S_L_1_GENERAL,		//lock if I'm using single vs. a staff
	BOTH_LK_S_ST_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_ST_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_S_ST_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_ST_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_ST_T_L_1_GENERAL,		//lock if I'm using single vs. a staff
	BOTH_LK_S_ST_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_ST_T_SB_1_W_GENERAL,	//super break I won
//SINGLE vs. SINGLE
	//side locks
	BOTH_LK_S_S_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_S_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_S_S_L_1_GENERAL,		//lock if I'm using single vs. a single and I initiated
	BOTH_LK_S_S_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_S_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_S_S_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_S_S_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_S_S_T_L_1_GENERAL,		//lock if I'm using single vs. a single and I initiated
	BOTH_LK_S_S_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_S_S_T_SB_1_W_GENERAL,	//super break I won
//===Dual Saber locks==================================================================
//DUAL vs. DUAL	
	//side locks
	BOTH_LK_DL_DL_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_DL_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_DL_S_L_1_GENERAL,	//lock if I'm using dual vs. dual and I initiated
	BOTH_LK_DL_DL_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_DL_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_DL_DL_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_DL_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_DL_T_L_1_GENERAL,	//lock if I'm using dual vs. dual and I initiated
	BOTH_LK_DL_DL_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_DL_T_SB_1_W_GENERAL,	//super break I won
//DUAL vs. STAFF
	//side locks
	BOTH_LK_DL_ST_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_ST_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_ST_S_L_1_GENERAL,	//lock if I'm using dual vs. a staff
	BOTH_LK_DL_ST_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_ST_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_DL_ST_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_ST_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_ST_T_L_1_GENERAL,	//lock if I'm using dual vs. a staff
	BOTH_LK_DL_ST_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_ST_T_SB_1_W_GENERAL,	//super break I won
//DUAL vs. SINGLE
	//side locks
	BOTH_LK_DL_S_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_S_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_S_S_L_1_GENERAL,		//lock if I'm using dual vs. a single
	BOTH_LK_DL_S_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_S_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_DL_S_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_DL_S_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_DL_S_T_L_1_GENERAL,		//lock if I'm using dual vs. a single
	BOTH_LK_DL_S_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_DL_S_T_SB_1_W_GENERAL,	//super break I won
//===Saber Staff locks==================================================================
//STAFF vs. DUAL
	//side locks
	BOTH_LK_ST_DL_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_DL_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_DL_S_L_1_GENERAL,	//lock if I'm using staff vs. dual
	BOTH_LK_ST_DL_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_DL_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_ST_DL_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_DL_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_DL_T_L_1_GENERAL,	//lock if I'm using staff vs. dual
	BOTH_LK_ST_DL_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_DL_T_SB_1_W_GENERAL,	//super break I won
//STAFF vs. STAFF
	//side locks
	BOTH_LK_ST_ST_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_ST_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_ST_S_L_1_GENERAL,	//lock if I'm using staff vs. a staff and I initiated
	BOTH_LK_ST_ST_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_ST_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_ST_ST_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_ST_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_ST_T_L_1_GENERAL,	//lock if I'm using staff vs. a staff and I initiated
	BOTH_LK_ST_ST_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_ST_T_SB_1_W_GENERAL,	//super break I won
//STAFF vs. SINGLE
	//side locks
	BOTH_LK_ST_S_S_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_S_S_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_S_S_L_1_GENERAL,		//lock if I'm using staff vs. a single
	BOTH_LK_ST_S_S_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_S_S_SB_1_W_GENERAL,	//super break I won
	//top locks
	BOTH_LK_ST_S_T_B_1_L_GENERAL,	//normal break I lost
	BOTH_LK_ST_S_T_B_1_W_GENERAL,	//normal break I won
	BOTH_LK_ST_S_T_L_1_GENERAL,		//lock if I'm using staff vs. a single
	BOTH_LK_ST_S_T_SB_1_L_GENERAL,	//super break I lost
	BOTH_LK_ST_S_T_SB_1_W_GENERAL,	//super break I won
//Special cases for same saber style vs. each other (won't fit in nice 5-anim size lists above)
BOTH_LK_S_S_S_L_2_GENERAL,		//lock if I'm using single vs. a single and other intitiated
BOTH_LK_S_S_T_L_2_GENERAL,		//lock if I'm using single vs. a single and other initiated
BOTH_LK_DL_DL_S_L_2_GENERAL,	//lock if I'm using dual vs. dual and other initiated
BOTH_LK_DL_DL_T_L_2_GENERAL,	//lock if I'm using dual vs. dual and other initiated
BOTH_LK_ST_ST_S_L_2_GENERAL,	//lock if I'm using staff vs. a staff and other initiated
BOTH_LK_ST_ST_T_L_2_GENERAL,	//lock if I'm using staff vs. a staff and other initiated
//===End Saber locks==================================================================
	//old locks
	BOTH_BF2RETURN_GENERAL,	//#
	BOTH_BF2BREAK_GENERAL,	//#
	BOTH_BF2LOCK_GENERAL,	//#
	BOTH_BF1RETURN_GENERAL,	//#
	BOTH_BF1BREAK_GENERAL,	//#
	BOTH_BF1LOCK_GENERAL,	//#
	BOTH_CWCIRCLE_R2__R_S1_GENERAL,	//#
	BOTH_CCWCIRCLE_R2__L_S1_GENERAL,	//#
	BOTH_CWCIRCLE_A2__L__R_GENERAL,	//#
	BOTH_CCWCIRCLE_A2__R__L_GENERAL,	//#
	BOTH_CWCIRCLEBREAK_GENERAL,	//#
	BOTH_CCWCIRCLEBREAK_GENERAL,	//#
	BOTH_CWCIRCLELOCK_GENERAL,	//#
	BOTH_CCWCIRCLELOCK_GENERAL,	//#
	//other saber anims
	//* #sep BOTH_ SABER MISC ANIMS
	BOTH_SABERFAST_STANCE_GENERAL,
	BOTH_SABERSLOW_STANCE_GENERAL,
	BOTH_SABERDUAL_STANCE_GENERAL,
	BOTH_SABERSTAFF_STANCE_GENERAL,
	BOTH_A2_STABBACK1_GENERAL,		//# Stab saber backward
	BOTH_ATTACK_BACK_GENERAL,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1_GENERAL,//#
	BOTH_JUMPFLIPSTABDOWN_GENERAL,//#
	BOTH_FORCELEAP2_T__B__GENERAL,//#
	BOTH_LUNGE2_B__T__GENERAL,//#
	BOTH_CROUCHATTACKBACK1_GENERAL,//#
	//New specials for JKA:
	BOTH_JUMPATTACK6_GENERAL,//#
	BOTH_JUMPATTACK7_GENERAL,//#
	BOTH_SPINATTACK6_GENERAL,//#
	BOTH_SPINATTACK7_GENERAL,//#
	BOTH_S1_S6_GENERAL,//#	From stand1 to saberdual stance - turning on your dual sabers
	BOTH_S6_S1_GENERAL,//#	From dualstaff stance to stand1 - turning off your dual sabers
	BOTH_S1_S7_GENERAL,//#	From stand1 to saberstaff stance - turning on your saberstaff
	BOTH_S7_S1_GENERAL,//#	From saberstaff stance to stand1 - turning off your saberstaff
	BOTH_FORCELONGLEAP_START_GENERAL,
	BOTH_FORCELONGLEAP_ATTACK_GENERAL,
	BOTH_FORCELONGLEAP_LAND_GENERAL,
	BOTH_FORCEWALLRUNFLIP_START_GENERAL,
	BOTH_FORCEWALLRUNFLIP_END_GENERAL,
	BOTH_FORCEWALLRUNFLIP_ALT_GENERAL,
	BOTH_FORCEWALLREBOUND_FORWARD_GENERAL,
	BOTH_FORCEWALLREBOUND_LEFT_GENERAL,
	BOTH_FORCEWALLREBOUND_BACK_GENERAL,
	BOTH_FORCEWALLREBOUND_RIGHT_GENERAL,
	BOTH_FORCEWALLHOLD_FORWARD_GENERAL,
	BOTH_FORCEWALLHOLD_LEFT_GENERAL,
	BOTH_FORCEWALLHOLD_BACK_GENERAL,
	BOTH_FORCEWALLHOLD_RIGHT_GENERAL,
	BOTH_FORCEWALLRELEASE_FORWARD_GENERAL,
	BOTH_FORCEWALLRELEASE_LEFT_GENERAL,
	BOTH_FORCEWALLRELEASE_BACK_GENERAL,
	BOTH_FORCEWALLRELEASE_RIGHT_GENERAL,
	BOTH_A7_KICK_F_GENERAL,
	BOTH_A7_KICK_B_GENERAL,
	BOTH_A7_KICK_R_GENERAL,
	BOTH_A7_KICK_L_GENERAL,
	BOTH_A7_KICK_S_GENERAL,
	BOTH_A7_KICK_BF_GENERAL,
	BOTH_A7_KICK_BF_STOP_GENERAL,
	BOTH_A7_KICK_RL_GENERAL,
	BOTH_A7_KICK_F_AIR_GENERAL,
	BOTH_A7_KICK_B_AIR_GENERAL,
	BOTH_A7_KICK_R_AIR_GENERAL,
	BOTH_A7_KICK_L_AIR_GENERAL,
	BOTH_FLIP_ATTACK7_GENERAL,
	BOTH_FLIP_HOLD7_GENERAL,
	BOTH_FLIP_LAND_GENERAL,
	BOTH_PULL_IMPALE_STAB_GENERAL,
	BOTH_PULL_IMPALE_SWING_GENERAL,
	BOTH_PULLED_INAIR_B_GENERAL,
	BOTH_PULLED_INAIR_F_GENERAL,
	BOTH_STABDOWN_GENERAL,
	BOTH_STABDOWN_STAFF_GENERAL,
	BOTH_STABDOWN_DUAL_GENERAL,
	BOTH_A6_SABERPROTECT_GENERAL,
	BOTH_A7_SOULCAL_GENERAL,
	BOTH_A1_SPECIAL_GENERAL,
	BOTH_A2_SPECIAL_GENERAL,
	BOTH_A3_SPECIAL_GENERAL,
	BOTH_ROLL_STAB_GENERAL,

	//# #sep BOTH_ STANDING
	BOTH_STAND1_GENERAL,			//# Standing idle_GENERAL, no weapon_GENERAL, hands down
	BOTH_STAND1_RANDOM1_GENERAL,	//# Random standing idle
	BOTH_STAND1_RANDOM2_GENERAL,	//# Random standing idle
	BOTH_STAND1IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND2_GENERAL,			//# Standing idle with a saber
	BOTH_STAND2_RANDOM1_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM2_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM3_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM4_GENERAL,	//# Random standing idle
	BOTH_STAND2IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND2IDLE2_GENERAL,		//# Random standing idle
	BOTH_STAND3_GENERAL,			//# DM15: Standing hands behind back_GENERAL, at ease_GENERAL, etc._GENERAL, Rest: Standing idle with 2-handed weapon
	BOTH_STAND3IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND4_GENERAL,			//# hands clasp behind back
	BOTH_STAND4IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND5_GENERAL,			//# standing idle_GENERAL, no weapon_GENERAL, hand down_GENERAL, back straight
	BOTH_STAND5IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND6_GENERAL,			//# one handed_GENERAL, gun at side_GENERAL, relaxed stand
	BOTH_STAND7_GENERAL,			//# both hands on hips (female)
	BOTH_STAND8_GENERAL,			//# both hands on hips (male)
	BOTH_STAND1TO3_GENERAL,			//# Transition from stand1 to stand3
	BOTH_STAND3TO1_GENERAL,			//# Transition from stand3 to stand1
	BOTH_STAND1TO2_GENERAL,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1_GENERAL,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4_GENERAL,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2_GENERAL,			//# Transition from stand4 to stand2
	BOTH_STANDTOWALK1_GENERAL,		//# Transition from stand1 to walk1
	BOTH_STAND4TOATTACK2_GENERAL,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP1_GENERAL,			//# standing up and stumbling
	BOTH_STANDUP2_GENERAL,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3_GENERAL,		//# transition from stand 5 to sit 3
	BOTH_STAND1_REELO_GENERAL,		//# Reelo in his stand1 position (cin #18)
	BOTH_STAND5_REELO_GENERAL,		//# Reelo in his stand5 position (cin #18)
	BOTH_STAND1TOSTAND5_GENERAL,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1_GENERAL,	//# Transition from stand5 to stand1
	BOTH_STAND5TOAIM_GENERAL,		//# Transition of Kye aiming his gun at Desann (cin #9) 
	BOTH_STAND5STARTLEDLOOKLEFT_GENERAL,	//# Kyle turning to watch the bridge drop (cin #9) 
	BOTH_STARTLEDLOOKLEFTTOSTAND5_GENERAL,	//# Kyle returning to stand 5 from watching the bridge drop (cin #9) 
	BOTH_STAND5TOSTAND8_GENERAL,	//# Transition from stand5 to stand8
	BOTH_STAND7TOSTAND8_GENERAL,	//# Tavion putting hands on back of chair (cin #11)
	BOTH_STAND8TOSTAND5_GENERAL,	//# Transition from stand8 to stand5
	BOTH_STAND9_GENERAL,			//# Kyle's standing idle_GENERAL, no weapon_GENERAL, hands down
	BOTH_STAND9IDLE1_GENERAL,		//# Kyle's random standing idle
	BOTH_STAND5SHIFTWEIGHT_GENERAL,	//# Weightshift from stand5 to side and back to stand5
	BOTH_STAND5SHIFTWEIGHTSTART_GENERAL,	//# From stand5 to side
	BOTH_STAND5SHIFTWEIGHTSTOP_GENERAL,		//# From side to stand5
	BOTH_STAND5TURNLEFTSTART_GENERAL,		//# Start turning left from stand5
	BOTH_STAND5TURNLEFTSTOP_GENERAL,		//# Stop turning left from stand5
	BOTH_STAND5TURNRIGHTSTART_GENERAL,		//# Start turning right from stand5
	BOTH_STAND5TURNRIGHTSTOP_GENERAL,		//# Stop turning right from stand5
	BOTH_STAND5LOOK180LEFTSTART_GENERAL,	//# Start looking over left shoulder (cin #17)
	BOTH_STAND5LOOK180LEFTSTOP_GENERAL,	//# Stop looking over left shoulder (cin #17)

	BOTH_CONSOLE1START_GENERAL,		//# typing at a console
	BOTH_CONSOLE1_GENERAL,			//# typing at a console
	BOTH_CONSOLE1STOP_GENERAL,		//# typing at a console
	BOTH_CONSOLE2START_GENERAL,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2_GENERAL,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP_GENERAL,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTART_GENERAL,	//# lean in to type at console while holding comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTOP_GENERAL,	//# lean away after typing at console while holding comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1_GENERAL,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1_GENERAL,		//# Cradling weapon and standing
	BOTH_ALERT1_GENERAL,			//# Startled by something while on guard
	BOTH_GESTURE1_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE2_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE3_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_WALK1TALKCOMM1_GENERAL,	//# Talking into coom link while walking
	BOTH_TALK1_GENERAL,				//# Generic talk anim
	BOTH_TALK2_GENERAL,				//# Generic talk anim
	BOTH_TALKCOMM1START_GENERAL,	//# Start talking into a comm link
	BOTH_TALKCOMM1_GENERAL,			//# Talking into a comm link
	BOTH_TALKCOMM1STOP_GENERAL,		//# Stop talking into a comm link
	BOTH_TALKGESTURE1_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE2_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE3_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE4START_GENERAL,	//# Beginning talk anim 4
	BOTH_TALKGESTURE4_GENERAL,		//# Talk gesture 4
	BOTH_TALKGESTURE4STOP_GENERAL,	//# Ending talk anim 4
	BOTH_TALKGESTURE5START_GENERAL,	//# Start hand on chin
	BOTH_TALKGESTURE5_GENERAL,		//# Hand on chin
	BOTH_TALKGESTURE5STOP_GENERAL,	//# Stop hand on chin
	BOTH_TALKGESTURE6START_GENERAL,	//# Starting Motions to self
	BOTH_TALKGESTURE6_GENERAL,		//# Pointing at self
	BOTH_TALKGESTURE6STOP_GENERAL,	//# Ending Motions to self
	BOTH_TALKGESTURE7START_GENERAL,	//# Start touches Kyle on shoulder
	BOTH_TALKGESTURE7_GENERAL,		//# Hold touches Kyle on shoulder
	BOTH_TALKGESTURE7STOP_GENERAL,	//# Ending touches Kyle on shoulder
	BOTH_TALKGESTURE8START_GENERAL,	//# Lando's chin hold 
	BOTH_TALKGESTURE8_GENERAL,			//# Lando's chin hold 
	BOTH_TALKGESTURE8STOP_GENERAL,	//# Lando's chin hold 
	BOTH_TALKGESTURE9_GENERAL,		//# Same as gesture 2 but with the right hand
	BOTH_TALKGESTURE10_GENERAL,		//# Shoulder shrug
	BOTH_TALKGESTURE11START_GENERAL,	//# Arms folded across chest
	BOTH_TALKGESTURE11STOP_GENERAL,	//# Arms folded across chest
	BOTH_TALKGESTURE12_GENERAL,		//# Tavion taunting Kyle
	BOTH_TALKGESTURE13START_GENERAL,	//# Luke warning Kyle
	BOTH_TALKGESTURE13_GENERAL,			//# Luke warning Kyle
	BOTH_TALKGESTURE13STOP_GENERAL,	//# Luke warning Kyle
	BOTH_TALKGESTURE14_GENERAL,			//# Luke gesturing to Kyle
	BOTH_TALKGESTURE15START_GENERAL,	//# Desann taunting Kyle
	BOTH_TALKGESTURE15_GENERAL,			//# Desann taunting Kyle
	BOTH_TALKGESTURE15STOP_GENERAL,		//# Desann taunting Kyle
	BOTH_TALKGESTURE16_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE17_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE18_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE19START_GENERAL,	//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE19STOP_GENERAL,		//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE20START_GENERAL,	//# Kyle lifting his arm "Join us" (cin #34)
	BOTH_TALKGESTURE21_GENERAL,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE22_GENERAL,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE23_GENERAL,			//# generic talk gesture from stand3
	BOTH_PAUSE1START_GENERAL,			//# Luke pauses to warn Kyle (cin #24) start
	BOTH_PAUSE1STOP_GENERAL,			//# Luke pauses to warn Kyle (cin #24) stop


	BOTH_HEADTILTLSTART_GENERAL,		//# Head tilt to left
	BOTH_HEADTILTLSTOP_GENERAL,			//# Head tilt to left
	BOTH_HEADTILTRSTART_GENERAL,		//# Head tilt to right
	BOTH_HEADTILTRSTOP_GENERAL,			//# Head tilt to right
	BOTH_HEADNOD_GENERAL,				//# Head shake YES
	BOTH_HEADSHAKE_GENERAL,				//# Head shake NO
	BOTH_HEADSHAKE1_REELO_GENERAL,		//# Head shake NO for Reelo
	BOTH_SITHEADTILTLSTART_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTLSTOP_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTRSTART_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADTILTRSTOP_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADNOD_GENERAL,			//# Head shake YES from seated position
	BOTH_SITHEADSHAKE_GENERAL,			//# Head shake NO from seated position
	BOTH_SIT2HEADTILTLSTART_GENERAL,	//# Head tilt to left from seated position 2
	BOTH_SIT2HEADTILTLSTOP_GENERAL,		//# Head tilt to left from seated position 2

	BOTH_REACH1START_GENERAL,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP_GENERAL,		//# Monmothma reaching for crystal

	BOTH_EXAMINE1START_GENERAL,		//# Start Mon Mothma examining crystal 
	BOTH_EXAMINE1_GENERAL,			//# Mon Mothma examining crystal 
	BOTH_EXAMINE1STOP_GENERAL,		//# Stop Mon Mothma examining crystal 
	BOTH_EXAMINE2START_GENERAL,	//# Start Kyle tossing crystal
	BOTH_EXAMINE2_GENERAL,			//# Hold Kyle tossing crystal
	BOTH_EXAMINE2STOP_GENERAL,		//# End Kyle tossing crystal
	BOTH_EXAMINE3START_GENERAL,	//# Start Lando looking around corner
	BOTH_EXAMINE3_GENERAL,			//# Hold Lando looking around corner
	BOTH_EXAMINE3STOP_GENERAL,		//# End Lando looking around corner

	BOTH_THROW1START_GENERAL,		//# Kyle thrown to the right
	BOTH_THROW1_GENERAL,			//# Kyle thrown to the right
	BOTH_THROW1STOP_GENERAL,		//# Kyle thrown to the right
	BOTH_THROW2START_GENERAL,		//# Kyle thrown to the left
	BOTH_THROW2_GENERAL,			//# Kyle thrown to the left
	BOTH_THROW3_GENERAL,			//# Kyle thrown backwards in cin #9
	BOTH_LEANLEFT2START_GENERAL,	//# Start leaning left in chair
	BOTH_LEANLEFT2STOP_GENERAL,		//# Stop leaning left in chair
	BOTH_LEANRIGHT3START_GENERAL,	//# Start Lando leaning on wall
	BOTH_LEANRIGHT3_GENERAL,			//# Lando leaning on wall
	BOTH_LEANRIGHT3STOP_GENERAL,		//# Stop Lando leaning on wall

	BOTH_FORCEFOUNTAIN1_START_GENERAL,	//# Kyle being lifted into the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_MIDDLE_GENERAL,	//# Kyle changing to looping position in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_LOOP_GENERAL,	//# Kyle being spun in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_STOP_GENERAL,	//# Kyle being set down out of the Force Fountain (cin #10)
	BOTH_THUMBING1_GENERAL,				//# Lando gesturing with thumb over his shoulder (cin #19)
	BOTH_COME_ON1_GENERAL,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1_GENERAL,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END_GENERAL,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1_GENERAL,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1_GENERAL,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_PUNCHER1_GENERAL,				//# Jan punching Kyle in the shoulder (cin #37)
	BOTH_CONSTRAINER1HOLD_GENERAL,		//# Static pose of starting Tavion constraining Jan (cin #9)
	BOTH_CONSTRAINEE1HOLD_GENERAL,		//# Static pose of starting Jan being constrained by Tavion (cin #9)
	BOTH_CONSTRAINER1STAND_GENERAL,		//# Tavion constraining Jan in a stand pose (cin #9)
	BOTH_CONSTRAINEE1STAND_GENERAL,		//# Jan being constrained in a stand pose (cin #9)
	BOTH_CONSTRAINER1WALK_GENERAL,		//# Tavion shoving jan forward (cin #9)
	BOTH_CONSTRAINEE1WALK_GENERAL,		//# Jan being shoved forward by Tavion (cin #9)
	BOTH_CONSTRAINER1LOOP_GENERAL,		//# Tavion walking with Jan in a loop (cin #9)
	BOTH_CONSTRAINEE1LOOP_GENERAL,		//# Jan walking with Tavion in a loop (cin #9)
	BOTH_SABERKILLER1_GENERAL,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1_GENERAL,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HANDSHAKER1START_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKER1LOOP_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKEE1START_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_HANDSHAKEE1LOOP_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_LAUGH1START_GENERAL,			//# Reelo leaning forward before laughing (cin #18)
	BOTH_LAUGH1STOP_GENERAL,			//# Reelo laughing (cin #18)
	BOTH_ESCAPEPOD_LEAVE1_GENERAL,	//# Kyle leaving escape pod (cin #33)
	BOTH_ESCAPEPOD_LEAVE2_GENERAL,	//# Jan leaving escape pod (cin #33)
	BOTH_HUGGER1_GENERAL,			//# Kyle hugging Jan (cin #29)
	BOTH_HUGGERSTOP1_GENERAL,		//# Kyle stop hugging Jan but don't let her go (cin #29)
	BOTH_HUGGERSTOP2_GENERAL,		//# Kyle let go of Jan and step back (cin #29)
	BOTH_HUGGEE1_GENERAL,			//# Jan being hugged (cin #29)
	BOTH_HUGGEESTOP1_GENERAL,		//# Jan stop being hugged but don't let go (cin #29)
	BOTH_HUGGEESTOP2_GENERAL,		//# Jan released from hug (cin #29)
	BOTH_KISSER1_GENERAL,			//# Temp until the Kiss anim gets split up
	BOTH_KISSER1START1_GENERAL,		//# Kyle start kissing Jan
	BOTH_KISSER1START2_GENERAL,		//# Kyle start kissing Jan
	BOTH_KISSER1LOOP_GENERAL,		//# Kyle loop kissing Jan
	BOTH_KISSER1STOP_GENERAL,		//# Temp until the Kiss anim gets split up
	BOTH_KISSER1STOP1_GENERAL,		//# Kyle stop kissing but don't let go
	BOTH_KISSER1STOP2_GENERAL,		//# Kyle step back from Jan
	BOTH_KISSEE1_GENERAL,			//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1START1_GENERAL,		//# Jan start being kissed
	BOTH_KISSEE1START2_GENERAL,		//# Jan start being kissed
	BOTH_KISSEE1LOOP_GENERAL,		//# Jan loop being kissed
	BOTH_KISSEE1STOP_GENERAL,		//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1STOP1_GENERAL,		//# Jan stop being kissed but don't let go
	BOTH_KISSEE1STOP2_GENERAL,		//# Jan wait for Kyle to step back
	BOTH_BARTENDER_IDLE1_GENERAL,	//# Bartender idle in cin #15
	BOTH_BARTENDER_THROW1_GENERAL,	//# Bartender throws glass in cin #15
	BOTH_BARTENDER_COWERSTART_GENERAL,	//# Start of Bartender raising both hands up in surrender (cin #16)
	BOTH_BARTENDER_COWERLOOP_GENERAL,	//# Loop of Bartender waving both hands in surrender (cin #16)
	BOTH_BARTENDER_COWER_GENERAL,		//# Single frame of Bartender waving both hands in surrender (cin #16)
	BOTH_THREATEN1_START_GENERAL,	//# First frame of Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_THREATEN1_GENERAL,			//# Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_RADIO_ONOFF_GENERAL,		//# Mech Galak turning on his suit radio (cin #32)
	BOTH_TRIUMPHANT1START_GENERAL,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STARTGESTURE_GENERAL,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STOP_GENERAL,	//# Mech Galak lowering his arms in victory (cin #32)

	BOTH_SABERTHROW1START_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START_GENERAL,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP_GENERAL,		//# Kyle throwing his light saber (cin #32)

	BOTH_COVERUP1_LOOP_GENERAL,		//# animation of getting in line of friendly fire
	BOTH_COVERUP1_START_GENERAL,	//# transitions from stand to coverup1_loop
	BOTH_COVERUP1_END_GENERAL,		//# transitions from coverup1_loop to stand

	BOTH_INJURED4_GENERAL,			//# Injured pose 4
	BOTH_INJURED4TO5_GENERAL,		//# Transition from INJURED4 to INJURED5
	BOTH_INJURED5_GENERAL,			//# Injured pose 5
	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1STAND_GENERAL,			//# Stand up from First sitting anim
	BOTH_SIT1_GENERAL,				//# Normal chair sit.
	BOTH_SIT2_GENERAL,				//# Lotus position.
	BOTH_SIT3_GENERAL,				//# Sitting in tired position_GENERAL, elbows on knees

	BOTH_SIT2TO3_GENERAL,			//# Trans from sit2 to sit3?
	BOTH_SIT2TOSTAND5_GENERAL,		//# Transition from sit 2 to stand 5
	BOTH_STAND5TOSIT2_GENERAL,		//# Transition from stand 5 to sit 2
	BOTH_SIT2TOSIT4_GENERAL,		//# Trans from sit2 to sit4 (cin #12) Luke leaning back from lotus position.
	BOTH_SIT3TO1_GENERAL,			//# Trans from sit3 to sit1?
	BOTH_SIT3TO2_GENERAL,			//# Trans from sit3 to sit2?
	BOTH_SIT3TOSTAND5_GENERAL,		//# transition from sit 3 to stand 5

	BOTH_SIT4TO5_GENERAL,			//# Trans from sit4 to sit5
	BOTH_SIT4TO6_GENERAL,			//# Trans from sit4 to sit6
	BOTH_SIT5TO4_GENERAL,			//# Trans from sit5 to sit4
	BOTH_SIT5TO6_GENERAL,			//# Trans from sit5 to sit6
	BOTH_SIT6TO4_GENERAL,			//# Trans from sit6 to sit4
	BOTH_SIT6TO5_GENERAL,			//# Trans from sit6 to sit5
	//BOTH_SIT7_GENERAL,				//# sitting with arms over knees_GENERAL, no weapon
	BOTH_SIT7TOSTAND1_GENERAL,		//# getting up from sit7 into stand1
	BOTH_CROUCH1_GENERAL,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE_GENERAL,		//# Crouching idle
	BOTH_CROUCH1WALK_GENERAL,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK_GENERAL,	//# Walking while crouched
	BOTH_UNCROUCH1_GENERAL,			//# Transition from crouch to standing
	BOTH_CROUCH2IDLE_GENERAL,		//# crouch and resting on back righ heel_GENERAL, no weapon
	BOTH_CROUCH2TOSTAND1_GENERAL,	//# going from crouch2 to stand1
	BOTH_CROUCH3_GENERAL,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3_GENERAL,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_CROUCH4_GENERAL,			//# Slower version of crouch1 for cinematics
	BOTH_UNCROUCH4_GENERAL,			//# Slower version of uncrouch1 for cinematics
	BOTH_GET_UP1_GENERAL,			//# Get up from the ground_GENERAL, face down
	BOTH_GET_UP2_GENERAL,			//# Get up from the ground_GENERAL, face up

	//BOTH_COCKPIT_CONSOLE1_GENERAL,		//# work console1 while sitting in a cockpit.
	//BOTH_COCKPIT_CONSOLE2_GENERAL,		//# work console2 while sitting in a cockpit.
	BOTH_COCKPIT_SIT_GENERAL,			//# sit in a cockpit.

	BOTH_GUNSIT1_GENERAL,			//# sitting on an emplaced gun.

	// Swoop Vehicle animations.
	//* #sep BOTH_ SWOOP ANIMS
	BOTH_VS_MOUNT_L_GENERAL,			//# Mount from left		
	BOTH_VS_DISMOUNT_L_GENERAL,			//# Dismount to left		
	BOTH_VS_MOUNT_R_GENERAL,			//# Mount from  right (symmetry)		
	BOTH_VS_DISMOUNT_R_GENERAL,			//# DISMOUNT TO  RIGHT (SYMMETRY)		

	BOTH_VS_MOUNTJUMP_L_GENERAL,		//#
	BOTH_VS_MOUNTTHROW_GENERAL,			//# Land on an occupied vehicle & throw off current pilot
	BOTH_VS_MOUNTTHROW_L_GENERAL,		//# Land on an occupied vehicle & throw off current pilot
	BOTH_VS_MOUNTTHROW_R_GENERAL,		//# Land on an occupied vehicle & throw off current pilot
	BOTH_VS_MOUNTTHROWEE_GENERAL,		//# Current pilot getting thrown off by another guy

	BOTH_VS_LOOKLEFT_GENERAL,			//# Turn & Look behind and to the left (no weapon)		
	BOTH_VS_LOOKRIGHT_GENERAL,			//# Turn & Look behind and to the right (no weapon)		

	BOTH_VS_TURBO_GENERAL,				//# Hit The Turbo Button

	BOTH_VS_REV_GENERAL,				//# Player looks back as swoop reverses		

	BOTH_VS_AIR_GENERAL,				//# Player stands up when swoop is airborn		
	BOTH_VS_AIR_G_GENERAL,				//# "" with Gun
	BOTH_VS_AIR_SL_GENERAL,				//# "" with Saber Left
	BOTH_VS_AIR_SR_GENERAL,				//# "" with Saber Right

	BOTH_VS_LAND_GENERAL,				//# Player bounces down when swoop lands		
	BOTH_VS_LAND_G_GENERAL,				//#  "" with Gun
	BOTH_VS_LAND_SL_GENERAL,			//#  "" with Saber Left
	BOTH_VS_LAND_SR_GENERAL,			//#  "" with Saber Right

	BOTH_VS_IDLE_GENERAL,				//# Sit
	BOTH_VS_IDLE_G_GENERAL,				//# Sit (gun)
	BOTH_VS_IDLE_SL_GENERAL,			//# Sit (saber left)		
	BOTH_VS_IDLE_SR_GENERAL,			//# Sit (saber right)		

	BOTH_VS_LEANL_GENERAL,				//# Lean left
	BOTH_VS_LEANL_G_GENERAL,			//# Lean left (gun)		
	BOTH_VS_LEANL_SL_GENERAL,			//# Lean left (saber left)		
	BOTH_VS_LEANL_SR_GENERAL,			//# Lean left (saber right)		

	BOTH_VS_LEANR_GENERAL,				//# Lean right		
	BOTH_VS_LEANR_G_GENERAL,			//# Lean right (gun)		
	BOTH_VS_LEANR_SL_GENERAL,			//# Lean right (saber left)		
	BOTH_VS_LEANR_SR_GENERAL,			//# Lean right (saber right)		

	BOTH_VS_ATL_S_GENERAL,				//# Attack left with saber		
	BOTH_VS_ATR_S_GENERAL,				//# Attack right with saber		
	BOTH_VS_ATR_TO_L_S_GENERAL,			//# Attack toss saber from right to left hand
	BOTH_VS_ATL_TO_R_S_GENERAL,			//# Attack toss saber from left to right hand
	BOTH_VS_ATR_G_GENERAL,				//# Attack right with gun (90)		
	BOTH_VS_ATL_G_GENERAL,				//# Attack left with gun (90)		
	BOTH_VS_ATF_G_GENERAL,				//# Attack forward with gun		

	BOTH_VS_PAIN1_GENERAL,				//# Pain

	// Added 12/04/02 by Aurelio.
	//* #sep BOTH_ TAUNTAUN ANIMS
	BOTH_VT_MOUNT_L_GENERAL,		//# Mount from left	
	BOTH_VT_MOUNT_R_GENERAL,		//# Mount from right
	BOTH_VT_MOUNT_B_GENERAL,		//# Mount from air_GENERAL, behind
	BOTH_VT_DISMOUNT_GENERAL,		//# Dismount for tauntaun
	BOTH_VT_DISMOUNT_L_GENERAL,		//# Dismount to tauntauns left	
	BOTH_VT_DISMOUNT_R_GENERAL,		//# Dismount to tauntauns right (symmetry)	

	BOTH_VT_WALK_FWD_GENERAL,		//# Walk forward	
	BOTH_VT_WALK_REV_GENERAL,		//# Walk backward	
	BOTH_VT_WALK_FWD_L_GENERAL,		//# walk lean left
	BOTH_VT_WALK_FWD_R_GENERAL,		//# Walk lean right
	BOTH_VT_RUN_FWD_GENERAL,		//# Run forward	
	BOTH_VT_RUN_REV_GENERAL,		//# Look backwards while running (not weapon specific)	
	BOTH_VT_RUN_FWD_L_GENERAL,		//# Run lean left
	BOTH_VT_RUN_FWD_R_GENERAL,		//# Run lean right

	BOTH_VT_SLIDEF_GENERAL,			//# Tauntaun slides forward with abrupt stop	
	BOTH_VT_AIR_GENERAL,			//# Tauntaun jump	
	BOTH_VT_ATB_GENERAL,			//# Tauntaun tail swipe	
	BOTH_VT_PAIN1_GENERAL,			//# Pain	
	BOTH_VT_DEATH1_GENERAL,			//# Die	
	BOTH_VT_STAND_GENERAL,			//# Stand still and breath	
	BOTH_VT_BUCK_GENERAL,			//# Tauntaun bucking loop animation	

	BOTH_VT_LAND_GENERAL,			//# Player bounces down when tauntaun lands	
	BOTH_VT_TURBO_GENERAL,			//# Hit The Turbo Button
	BOTH_VT_IDLE_SL_GENERAL,		//# Sit (saber left)		
	BOTH_VT_IDLE_SR_GENERAL,		//# Sit (saber right)		

	BOTH_VT_IDLE_GENERAL,			//# Sit with no weapon selected	
	BOTH_VT_IDLE1_GENERAL,			//# Sit with no weapon selected	
	BOTH_VT_IDLE_S_GENERAL,			//# Sit with saber selected	
	BOTH_VT_IDLE_G_GENERAL,			//# Sit with gun selected	
	BOTH_VT_IDLE_T_GENERAL,			//# Sit with thermal grenade selected

	BOTH_VT_ATL_S_GENERAL,			//# Attack left with saber	
	BOTH_VT_ATR_S_GENERAL,			//# Attack right with saber	
	BOTH_VT_ATR_TO_L_S_GENERAL,		//# Attack toss saber from right to left hand
	BOTH_VT_ATL_TO_R_S_GENERAL,		//# Attack toss saber from left to right hand
	BOTH_VT_ATR_G_GENERAL,			//# Attack right with gun (90)	
	BOTH_VT_ATL_G_GENERAL,			//# Attack left with gun (90)	
	BOTH_VT_ATF_G_GENERAL,			//# Attack forward with gun	


	// Added 2/26/02 by Aurelio.
	//* #sep BOTH_ FIGHTER ANIMS
	BOTH_GEARS_OPEN_GENERAL,
	BOTH_GEARS_CLOSE_GENERAL,
	BOTH_WINGS_OPEN_GENERAL,
	BOTH_WINGS_CLOSE_GENERAL,

	BOTH_DEATH14_UNGRIP_GENERAL,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP_GENERAL,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_KNEES1_GENERAL,			//# Tavion on her knees
	BOTH_KNEES2_GENERAL,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1_GENERAL,			//# Transition of KNEES2 to KNEES1
	BOTH_STRUGGLE1START_GENERAL,	//# Kyle struggling under crate
	BOTH_STRUGGLE1_GENERAL,			//# Kyle struggling under crate
	BOTH_STRUGGLE1STOP_GENERAL,		//# Kyle struggling under crate
	BOTH_RUMMAGE1START_GENERAL,	//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1_GENERAL,			//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1STOP_GENERAL,		//# Kyle rummaging for crystal (cin 2)
	BOTH_HOLDGLASS1_GENERAL,		//# Bartender holds glass (cin# 14)
	BOTH_SLIDEGLASS1_GENERAL,		//# Bartender slides glass (cin# 14)
	BOTH_SLAMSABERDOWN_GENERAL,		//# Kyle slamming his saber on the bar top (cin# 14)

	//# #sep BOTH_ MOVING
	BOTH_WALK1_GENERAL,				//# Normal walk
	BOTH_WALK2_GENERAL,				//# Normal walk
	BOTH_WALK3_GENERAL,				//# Goes with stand3
	BOTH_WALK4_GENERAL,				//# Walk cycle goes to a stand4
	BOTH_WALK_STAFF_GENERAL,		//# Walk with saberstaff turned on
	BOTH_WALKBACK_STAFF_GENERAL,	//# Walk backwards with saberstaff turned on
	BOTH_WALK_DUAL_GENERAL,			//# Walk with dual turned on
	BOTH_WALKBACK_DUAL_GENERAL,		//# Walk backwards with dual turned on
	BOTH_WALK5_GENERAL,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6_GENERAL,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7_GENERAL,				//# Fast walk
	BOTH_WALK8_GENERAL,				//# Normal walk with hands behind back (Luke in cin#12)
	BOTH_WALK9_GENERAL,				//# Lando walk (cin #17)
	BOTH_WALK10_GENERAL,			//# Lando walk (cin #17)
	BOTH_WALKTORUN1_GENERAL,		//# transition from walk to run
	BOTH_RUN1_GENERAL,				//# Full run
	BOTH_RUN1START_GENERAL,			//# Start into full run1
	BOTH_RUN1STOP_GENERAL,			//# Stop from full run1
	BOTH_RUN2_GENERAL,				//# Full run
	BOTH_RUNINJURED1_GENERAL,		//# Run with injured left leg
	BOTH_RUN1TORUN2_GENERAL,		//# Wampa run anim transition
	BOTH_RUN2TORUN1_GENERAL,		//# Wampa run anim transition
	BOTH_RUN4_GENERAL,				//# Jawa Run
	BOTH_RUN_STAFF_GENERAL,			//# Run with saberstaff turned on
	BOTH_RUNBACK_STAFF_GENERAL,		//# Run backwards with saberstaff turned on
	BOTH_RUN_DUAL_GENERAL,			//# Run with dual turned on
	BOTH_RUNBACK_DUAL_GENERAL,		//# Run backwards with dual turned on
	BOTH_STRAFE_LEFT1_GENERAL,		//# Sidestep left_GENERAL, should loop
	BOTH_STRAFE_RIGHT1_GENERAL,		//# Sidestep right_GENERAL, should loop
	BOTH_RUNSTRAFE_LEFT1_GENERAL,	//# Sidestep left_GENERAL, should loop
	BOTH_RUNSTRAFE_RIGHT1_GENERAL,	//# Sidestep right_GENERAL, should loop
	BOTH_TURN_LEFT1_GENERAL,		//# Turn left_GENERAL, should loop
	BOTH_TURN_RIGHT1_GENERAL,		//# Turn right_GENERAL, should loop
	BOTH_TURNSTAND1_GENERAL,		//# Turn from STAND1 position
	BOTH_TURNSTAND2_GENERAL,		//# Turn from STAND2 position
	BOTH_TURNSTAND3_GENERAL,		//# Turn from STAND3 position
	BOTH_TURNSTAND4_GENERAL,		//# Turn from STAND4 position
	BOTH_TURNSTAND5_GENERAL,		//# Turn from STAND5 position
	BOTH_TURNCROUCH1_GENERAL,		//# Turn from CROUCH1 position
	BOTH_RUNAWAY1_GENERAL,			//# Running scared
	BOTH_SWIM1_GENERAL,				//# Swimming

	BOTH_WALKBACK1_GENERAL,			//# Walk1 backwards
	BOTH_WALKBACK2_GENERAL,			//# Walk2 backwards
	BOTH_RUNBACK1_GENERAL,			//# Run1 backwards
	BOTH_RUNBACK2_GENERAL,			//# Run1 backwards

	//# #sep BOTH_ JUMPING
	BOTH_JUMP1_GENERAL,				//# Jump - wind-up and leave ground
	BOTH_INAIR1_GENERAL,			//# In air loop (from jump)
	BOTH_LAND1_GENERAL,				//# Landing (from in air loop)
	BOTH_LAND2_GENERAL,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1_GENERAL,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1_GENERAL,		//# In air loop (from jump back)
	BOTH_LANDBACK1_GENERAL,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1_GENERAL,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1_GENERAL,		//# In air loop (from jump left)
	BOTH_LANDLEFT1_GENERAL,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1_GENERAL,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1_GENERAL,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1_GENERAL,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1_GENERAL,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1_GENERAL,		//# In air loop (from jump)
	BOTH_FORCELAND1_GENERAL,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1_GENERAL,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1_GENERAL,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1_GENERAL,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1_GENERAL,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1_GENERAL,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1_GENERAL,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1_GENERAL,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1_GENERAL,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1_GENERAL,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F_GENERAL,			//# Flip forward
	BOTH_FLIP_B_GENERAL,			//# Flip backwards
	BOTH_FLIP_L_GENERAL,			//# Flip left
	BOTH_FLIP_R_GENERAL,			//# Flip right

	BOTH_ROLL_F_GENERAL,			//# Roll forward
	BOTH_ROLL_B_GENERAL,			//# Roll backward
	BOTH_ROLL_L_GENERAL,			//# Roll left
	BOTH_ROLL_R_GENERAL,			//# Roll right
	BOTH_ROLL_FR_GENERAL,			//# Roll forward right
	BOTH_ROLL_FL_GENERAL,			//# Roll forward left
	BOTH_ROLL_BR_GENERAL,			//# Roll back right
	BOTH_ROLL_BL_GENERAL,			//# Roll back left

	BOTH_HOP_F_GENERAL,				//# quickstep forward
	BOTH_HOP_B_GENERAL,				//# quickstep backwards
	BOTH_HOP_L_GENERAL,				//# quickstep left
	BOTH_HOP_R_GENERAL,				//# quickstep right

	BOTH_DODGE_FL_GENERAL,			//# lean-dodge forward left
	BOTH_DODGE_FR_GENERAL,			//# lean-dodge forward right
	BOTH_DODGE_BL_GENERAL,			//# lean-dodge backwards left
	BOTH_DODGE_BR_GENERAL,			//# lean-dodge backwards right
	BOTH_DODGE_L_GENERAL,			//# lean-dodge left
	BOTH_DODGE_R_GENERAL,			//# lean-dodge right

	BOTH_DIVE1_GENERAL,				//# Dive!
	BOTH_DODGE_HOLD_FL_GENERAL,			//# lean-dodge pose forward left
	BOTH_DODGE_HOLD_FR_GENERAL,			//# lean-dodge pose forward right
	BOTH_DODGE_HOLD_BL_GENERAL,			//# lean-dodge pose backwards left
	BOTH_DODGE_HOLD_BR_GENERAL,			//# lean-dodge pose backwards right
	BOTH_DODGE_HOLD_L_GENERAL,			//# lean-dodge pose left
	BOTH_DODGE_HOLD_R_GENERAL,			//# lean-dodge pose right

	//MP taunt anims
	BOTH_ENGAGETAUNT_GENERAL,
	BOTH_BOW_GENERAL,
	BOTH_MEDITATE_GENERAL,
	BOTH_MEDITATE_END_GENERAL,
	BOTH_SHOWOFF_FAST_GENERAL,
	BOTH_SHOWOFF_MEDIUM_GENERAL,
	BOTH_SHOWOFF_STRONG_GENERAL,
	BOTH_SHOWOFF_DUAL_GENERAL,
	BOTH_SHOWOFF_STAFF_GENERAL,
	BOTH_VICTORY_FAST_GENERAL,
	BOTH_VICTORY_MEDIUM_GENERAL,
	BOTH_VICTORY_STRONG_GENERAL,
	BOTH_VICTORY_DUAL_GENERAL,
	BOTH_VICTORY_STAFF_GENERAL,
	//other saber/acro anims
	BOTH_ARIAL_LEFT_GENERAL,		//# 
	BOTH_ARIAL_RIGHT_GENERAL,		//# 
	BOTH_CARTWHEEL_LEFT_GENERAL,	//# 
	BOTH_CARTWHEEL_RIGHT_GENERAL,	//# 
	BOTH_FLIP_LEFT_GENERAL,			//# 
	BOTH_FLIP_BACK1_GENERAL,		//# 
	BOTH_FLIP_BACK2_GENERAL,		//# 
	BOTH_FLIP_BACK3_GENERAL,		//# 
	BOTH_BUTTERFLY_LEFT_GENERAL,	//# 
	BOTH_BUTTERFLY_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_RIGHT_STOP_GENERAL,//# 
	BOTH_WALL_RUN_LEFT_GENERAL,		//# 
	BOTH_WALL_RUN_LEFT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_LEFT_STOP_GENERAL,//# 
	BOTH_WALL_FLIP_RIGHT_GENERAL,	//# 
	BOTH_WALL_FLIP_LEFT_GENERAL,	//# 
	BOTH_WALL_FLIP_FWD_GENERAL,		//#
	BOTH_KNOCKDOWN1_GENERAL,		//# knocked backwards
	BOTH_KNOCKDOWN2_GENERAL,		//# knocked backwards hard
	BOTH_KNOCKDOWN3_GENERAL,		//#	knocked forwards
	BOTH_KNOCKDOWN4_GENERAL,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5_GENERAL,		//# dupe of 3 - will be removed
	BOTH_GETUP1_GENERAL,			//#
	BOTH_GETUP2_GENERAL,			//#
	BOTH_GETUP3_GENERAL,			//#
	BOTH_GETUP4_GENERAL,			//#
	BOTH_GETUP5_GENERAL,			//#
	BOTH_GETUP_CROUCH_F1_GENERAL,	//#
	BOTH_GETUP_CROUCH_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_F1_GENERAL,	//#
	BOTH_FORCE_GETUP_F2_GENERAL,	//#
	BOTH_FORCE_GETUP_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_B2_GENERAL,	//#
	BOTH_FORCE_GETUP_B3_GENERAL,	//#
	BOTH_FORCE_GETUP_B4_GENERAL,	//#
	BOTH_FORCE_GETUP_B5_GENERAL,	//#
	BOTH_FORCE_GETUP_B6_GENERAL,	//#
	BOTH_GETUP_BROLL_B_GENERAL,	//#
	BOTH_GETUP_BROLL_F_GENERAL,	//#
	BOTH_GETUP_BROLL_L_GENERAL,	//#
	BOTH_GETUP_BROLL_R_GENERAL,	//#
	BOTH_GETUP_FROLL_B_GENERAL,	//#
	BOTH_GETUP_FROLL_F_GENERAL,	//#
	BOTH_GETUP_FROLL_L_GENERAL,	//#
	BOTH_GETUP_FROLL_R_GENERAL,	//#
	BOTH_WALL_FLIP_BACK1_GENERAL,	//#
	BOTH_WALL_FLIP_BACK2_GENERAL,	//#
	BOTH_SPIN1_GENERAL,				//#
	BOTH_CEILING_CLING_GENERAL,		//# clinging to ceiling
	BOTH_CEILING_DROP_GENERAL,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL_GENERAL,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR_GENERAL,		//# jump spin slash bl to tr
	BOTH_DEATHFROMBACKSLASH_GENERAL,//#
	BOTH_RIGHTHANDCHOPPEDOFF_GENERAL,//#
	BOTH_DEFLECTSLASH__R__L_FIN_GENERAL,//#
	BOTH_BASHED1_GENERAL,//#
	BOTH_ARIAL_F1_GENERAL,//#
	BOTH_BUTTERFLY_FR1_GENERAL,//#
	BOTH_BUTTERFLY_FL1_GENERAL,//#
	BOTH_POSE1_GENERAL,//#
	BOTH_POSE2_GENERAL,//#
	BOTH_POSE3_GENERAL,//#
	BOTH_POSE4_GENERAL,//#

	//NEW SABER/JEDI/FORCE ANIMS
	BOTH_BACK_FLIP_UP_GENERAL,	//# back flip up Bonus Animation!!!!	
	BOTH_LOSE_SABER_GENERAL,	//# player losing saber (pulled from hand by force pull 4 - Kyle?)
	BOTH_STAFF_TAUNT_GENERAL,	//# taunt saberstaff			
	BOTH_DUAL_TAUNT_GENERAL,		//# taunt dual
	BOTH_A6_FB_GENERAL,				//# dual attack front/back		
	BOTH_A6_LR_GENERAL,				//# dual attack left/right
	BOTH_A7_HILT_GENERAL,			//# saber knock (alt + stand still)
	//Alora			
	BOTH_ALORA_SPIN_GENERAL,		//#jump spin attack	death ballet	
	BOTH_ALORA_FLIP_1_GENERAL,		//# gymnast move 1		
	BOTH_ALORA_FLIP_2_GENERAL,		//# gymnast move 2		
	BOTH_ALORA_FLIP_3_GENERAL,		//# gymnast move3		
	BOTH_ALORA_FLIP_B_GENERAL,		//# gymnast move back		
	BOTH_ALORA_SPIN_THROW_GENERAL,	//# dual saber throw		
	BOTH_ALORA_SPIN_SLASH_GENERAL,	//# spin slash	special bonus animation!! :)	
	BOTH_ALORA_TAUNT_GENERAL,		//# special taunt
	//Rosh (Kothos battle)			
	BOTH_ROSH_PAIN_GENERAL,	//# hurt animation (exhausted)		
	BOTH_ROSH_HEAL_GENERAL,	//# healed/rejuvenated		
	//Tavion			
	BOTH_TAVION_SCEPTERGROUND_GENERAL, //# stabbing ground with sith sword shoots electricity everywhere
	BOTH_TAVION_SWORDPOWER_GENERAL,//# Tavion doing the He-Man(tm) thing
	BOTH_SCEPTER_START_GENERAL,	//#Point scepter and attack start
	BOTH_SCEPTER_HOLD_GENERAL,	//#Point scepter and attack hold
	BOTH_SCEPTER_STOP_GENERAL,	//#Point scepter and attack stop
	//Kyle Boss			
	BOTH_KYLE_GRAB_GENERAL,		//# grab
	BOTH_KYLE_MISS_GENERAL,		//# miss
	BOTH_KYLE_PA_1_GENERAL,		//# hold 1
	BOTH_PLAYER_PA_1_GENERAL,	//# player getting held 1
	BOTH_KYLE_PA_2_GENERAL,		//# hold 2
	BOTH_PLAYER_PA_2_GENERAL,	//# player getting held 2
	BOTH_PLAYER_PA_FLY_GENERAL,	//# player getting knocked back from punch at end of hold 1
	BOTH_KYLE_PA_3_GENERAL,		//# hold 3
	BOTH_PLAYER_PA_3_GENERAL,	//# player getting held 3
	BOTH_PLAYER_PA_3_FLY_GENERAL,//# player getting thrown at end of hold 3
	//Rancor
	BOTH_BUCK_RIDER_GENERAL,	//# Rancor bucks when someone is on him
	//WAMPA Grabbing enemy
	BOTH_HOLD_START_GENERAL,	//#
	BOTH_HOLD_MISS_GENERAL,	//#
	BOTH_HOLD_IDLE_GENERAL,	//#
	BOTH_HOLD_END_GENERAL,	//#
	BOTH_HOLD_ATTACK_GENERAL,	//#
	BOTH_HOLD_SNIFF_GENERAL,	//# Sniff the guy you're holding
	BOTH_HOLD_DROP_GENERAL,		//# just drop 'em
	//BEING GRABBED BY WAMPA
	BOTH_GRABBED_GENERAL,	//#
	BOTH_RELEASED_GENERAL,	//#
	BOTH_HANG_IDLE_GENERAL,	//#
	BOTH_HANG_ATTACK_GENERAL,	//#
	BOTH_HANG_PAIN_GENERAL,	//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1_GENERAL,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1_GENERAL,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1_GENERAL,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE_GENERAL,		//#	Just sitting on the ladder
	BOTH_ONLADDER_BOT1_GENERAL,		//# Getting on the ladder at the bottom
	BOTH_OFFLADDER_BOT1_GENERAL,	//# Getting off the ladder at the bottom
	BOTH_ONLADDER_TOP1_GENERAL,		//# Getting on the ladder at the top
	BOTH_OFFLADDER_TOP1_GENERAL,	//# Getting off the ladder at the top
	BOTH_LIFT1_GENERAL,				//# Lifting someone/thing over their shoulder
	BOTH_STEP1_GENERAL,				//# telsia checking out lake cinematic9.2
	BOTH_HITWALL1_GENERAL,			//# cin.18_GENERAL, Kenn hit by borg into wall 56 units away
	BOTH_AMBUSHLAND1_GENERAL,		//# landing from fall on victim
	BOTH_BIRTH1_GENERAL,			//# birth from jumping through walls

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_IDLE1_GENERAL,			//# Flying Idle 1
	BOTH_FLY_IDLE2_GENERAL,			//# Flying Idle 2
	BOTH_FLY_SHIELDED_GENERAL,		//# For sentry droid_GENERAL, shields in


	//# #sep BOTH_ FLYING MOVING
	BOTH_FLY_START1_GENERAL,		//# Start flying
	BOTH_FLY_STOP1_GENERAL,			//# Stop flying
	BOTH_FLY_LOOP1_GENERAL,			//# Normal flying_GENERAL, should loop
	BOTH_FLOAT1_GENERAL,			//# Crew floating through space 1
	BOTH_FLOAT2_GENERAL,			//# Crew floating through space 2
	BOTH_FLOATCONSOLE1_GENERAL,		//# Crew floating and working on console

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1_GENERAL,		//# Swimming Idle 1
	BOTH_SWIMFORWARDSTART_GENERAL,	//# Swim forward start
	BOTH_SWIMFORWARD_GENERAL,		//# Swim forward loop
	BOTH_SWIMFORWARDSTOP_GENERAL,	//# Swim forward end
	BOTH_SWIMBACKWARDSTART_GENERAL,	//# Swim backward start
	BOTH_SWIMBACKWARD_GENERAL,		//# Swim backward loop
	BOTH_SWIMBACKWARDSTOP_GENERAL,	//# Swim backward end

	//# #sep BOTH_ LYING
	BOTH_LIE_DOWN1_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face down
	BOTH_LIE_DOWN2_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face up
	BOTH_LIE_DOWN3_GENERAL,			//# reaction to local disnode being destroyed
	BOTH_PAIN2WRITHE1_GENERAL,		//# Transition from upright position to writhing on ground anim
	BOTH_PRONE2RLEG_GENERAL,		//# Lying on ground reach to grab right leg
	BOTH_PRONE2LLEG_GENERAL,		//# Lying on ground reach to grab left leg
	BOTH_WRITHING1_GENERAL,			//# Lying on ground on back writhing in pain
	BOTH_WRITHING1RLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding right leg
	BOTH_WRITHING1LLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding left leg
	BOTH_WRITHING2_GENERAL,			//# Lying on ground on front writhing in pain
	BOTH_INJURED1_GENERAL,			//# Lying down_GENERAL, against wall - can also be sleeping against wall
	BOTH_INJURED2_GENERAL,			//# Injured pose 2
	BOTH_INJURED3_GENERAL,			//# Injured pose 3
	BOTH_INJURED6_GENERAL,			//# Injured pose 6
	BOTH_INJURED6ATTACKSTART_GENERAL,	//# Start attack while in injured 6 pose 
	BOTH_INJURED6ATTACKSTOP_GENERAL,	//# End attack while in injured 6 pose
	BOTH_INJURED6COMBADGE_GENERAL,	//# Hit combadge while in injured 6 pose
	BOTH_INJURED6POINT_GENERAL,		//# Chang points to door while in injured state
	BOTH_INJUREDTOSTAND1_GENERAL,	//# Runinjured to stand1

	BOTH_PROPUP1_GENERAL,			//# Kyle getting up from having been knocked down (cin #9 end)
	BOTH_CRAWLBACK1_GENERAL,		//# Lying on back_GENERAL, crawling backwards with elbows
	BOTH_SITWALL1_GENERAL,			//# Sitting against a wall
	BOTH_SLEEP1_GENERAL,			//# laying on back-rknee up-rhand on torso
	BOTH_SLEEP2_GENERAL,			//# on floor-back against wall-arms crossed
	BOTH_SLEEP3_GENERAL,			//# Sleeping in a chair
	BOTH_SLEEP4_GENERAL,			//# Sleeping slumped over table
	BOTH_SLEEP5_GENERAL,			//# Laying on side sleeping on flat sufrace
	BOTH_SLEEP6START_GENERAL,		//# Kyle leaning back to sleep (cin 20)
	BOTH_SLEEP6STOP_GENERAL,		//# Kyle waking up and shaking his head (cin 21)
	BOTH_SLEEP1GETUP_GENERAL,		//# alarmed and getting up out of sleep1 pose to stand
	BOTH_SLEEP1GETUP2_GENERAL,		//# 
	BOTH_SLEEP2GETUP_GENERAL,		//# alarmed and getting up out of sleep2 pose to stand
	BOTH_SLEEP3GETUP_GENERAL,		//# alarmed and getting up out of sleep3 pose to stand
	BOTH_SLEEP3DEATH_GENERAL,		//# death in chair_GENERAL, from sleep3 idle
	BOTH_SLEEP3DEAD_GENERAL,		//# death in chair_GENERAL, from sleep3 idle

	BOTH_SLEEP_IDLE1_GENERAL,		//# rub face and nose while asleep from sleep pose 1
	BOTH_SLEEP_IDLE2_GENERAL,		//# shift position while asleep - stays in sleep2
	BOTH_SLEEP_IDLE3_GENERAL,		//# Idle anim from sleep pose 3
	BOTH_SLEEP_IDLE4_GENERAL,		//# Idle anim from sleep pose 4
	BOTH_SLEEP1_NOSE_GENERAL,		//# Scratch nose from SLEEP1 pose
	BOTH_SLEEP2_SHIFT_GENERAL,		//# Shift in sleep from SLEEP2 pose
	BOTH_RESTRAINED1_GENERAL,		//# Telsia tied to medical table
	BOTH_RESTRAINED1POINT_GENERAL,	//# Telsia tied to medical table pointing at Munro
	BOTH_LIFTED1_GENERAL,			//# Fits with BOTH_LIFT1_GENERAL, lifted on shoulder
	BOTH_CARRIED1_GENERAL,			//# Fits with TORSO_CARRY1_GENERAL, carried over shoulder
	BOTH_CARRIED2_GENERAL,			//# Laying over object

	BOTH_CHOKE1START_GENERAL,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD_GENERAL,	//# loop of tavion in force grip choke
	BOTH_CHOKE1_GENERAL,			//# tavion in force grip choke

	BOTH_CHOKE2_GENERAL,			//# tavion recovering from force grip choke
	BOTH_CHOKE3_GENERAL,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1_GENERAL,			//# Wakes up

	BOTH_TURNON_GENERAL,			//# Protocol Droid wakes up
	BOTH_TURNOFF_GENERAL,			//# Protocol Droid shuts off

	BOTH_BUTTON1_GENERAL,			//# Single button push with right hand
	BOTH_BUTTON2_GENERAL,			//# Single button push with left finger
	BOTH_BUTTON_HOLD_GENERAL,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE_GENERAL,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	//# #sep BOTH_ FORCE ANIMS
	BOTH_RESISTPUSH_GENERAL,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1_GENERAL,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2_GENERAL,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING_GENERAL,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_START_GENERAL,	//# Use off-hand to do lightning - start
	BOTH_FORCELIGHTNING_HOLD_GENERAL,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE_GENERAL,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START_GENERAL,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP_GENERAL,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK_GENERAL,		//# Healing meditation gesture
	BOTH_SABERPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1_GENERAL,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP2_GENERAL,		//# force-gripping (?)
	BOTH_FORCEGRIP3_GENERAL,		//# force-gripping (right hand)
	BOTH_FORCEGRIP3THROW_GENERAL,	//# throwing while force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD_GENERAL,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE_GENERAL,//# Use off-hand to do grip - release
	BOTH_TOSS1_GENERAL,				//# throwing to left after force gripping
	BOTH_TOSS2_GENERAL,				//# throwing to right after force gripping
	//NEW force anims for JKA:
	BOTH_FORCE_RAGE_GENERAL,
	BOTH_FORCE_2HANDEDLIGHTNING_GENERAL,
	BOTH_FORCE_2HANDEDLIGHTNING_START_GENERAL,
	BOTH_FORCE_2HANDEDLIGHTNING_HOLD_GENERAL,
	BOTH_FORCE_2HANDEDLIGHTNING_RELEASE_GENERAL,
	BOTH_FORCE_DRAIN_GENERAL,
	BOTH_FORCE_DRAIN_START_GENERAL,
	BOTH_FORCE_DRAIN_HOLD_GENERAL,
	BOTH_FORCE_DRAIN_RELEASE_GENERAL,
	BOTH_FORCE_DRAIN_GRAB_START_GENERAL,
	BOTH_FORCE_DRAIN_GRAB_HOLD_GENERAL,
	BOTH_FORCE_DRAIN_GRAB_END_GENERAL,
	BOTH_FORCE_DRAIN_GRABBED_GENERAL,
	BOTH_FORCE_ABSORB_GENERAL,
	BOTH_FORCE_ABSORB_START_GENERAL,
	BOTH_FORCE_ABSORB_END_GENERAL,
	BOTH_FORCE_PROTECT_GENERAL,
	BOTH_FORCE_PROTECT_FAST_GENERAL,


	BOTH_COCKPIT_TALKR1START_GENERAL,		//# turn head from straight forward to looking full right
	BOTH_COCKPIT_TALKR1STARTTOMID_GENERAL,	//# from TALKR1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKR1MIDTOSTART_GENERAL,	//# from looking at hologram to TALKR1START (cin #1)
	BOTH_COCKPIT_TALKR1STOP_GENERAL,		//# return head to straight forward from BOTH_COCKPIT_TALKR1
	BOTH_COCKPIT_TALKR1STOPTOMID_GENERAL,	//# from TALKR1STOP to TALKR1MID
	BOTH_COCKPIT_TALKR1MIDTOSTOP_GENERAL,	//# from looking at hologram to TALKR1STOP (cin #1)
	BOTH_COCKPIT_TALKR1_GENERAL,			//# talk to right side

	BOTH_COCKPIT_TALKL1START_GENERAL,		//# turn head from straight forward to looking full left
	BOTH_COCKPIT_TALKL1STARTTOMID_GENERAL,	//# from TALKL1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKL1MIDTOSTART_GENERAL,	//# from looking at hologram to TALKL1START (cin #1)
	BOTH_COCKPIT_TALKL1STOP_GENERAL,		//# return head to straight forward from BOTH_COCKPIT_TALKL1
	BOTH_COCKPIT_TALKL1STOPTOMID_GENERAL,	//# from TALKL1STOP to TALKL1MID
	BOTH_COCKPIT_TALKL1MIDTOSTOP_GENERAL,	//# from looking at hologram to TALKL1STOP (cin #1)
	BOTH_COCKPIT_TALKL1_GENERAL,			//# talk to left side

	BOTH_COCKPIT_CONSOLE1_GENERAL,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_GENERAL,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_PARTIAL_GENERAL,	//# last part of console2 anim (cin #1) used by Jan	



	BOTH_COCKPIT_HEADNOD_GENERAL,			//# nod head yes while sitting
	BOTH_COCKPIT_HEADSHAKE_GENERAL,			//# shake head no while sitting





	BOTH_COCKPIT_HEADTILTLSTART_GENERAL,	//# start tilt head left while sitting
	BOTH_COCKPIT_HEADTILTLSTOP_GENERAL,		//# stop tilt head left while sitting
	BOTH_COCKPIT_HEADTILTRSTART_GENERAL,	//# start tilt head right while sitting
	BOTH_COCKPIT_HEADTILTRSTOP_GENERAL,		//# stop tilt head right while sitting


	BOTH_COCKPIT_TALKGESTURE7START_GENERAL,		//# Lando's supporting hand to Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE7STOP_GENERAL,		//# Lando's supporting hand away from Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE8START_GENERAL,		//# Hand to Lando's chin (cin #21)
	BOTH_COCKPIT_TALKGESTURE8STOP_GENERAL,		//# hand away from Lando's chin *cin #21)
	BOTH_COCKPIT_TALKGESTURE11START_GENERAL,	//# 
	BOTH_COCKPIT_TALKGESTURE11STOP_GENERAL,		//# 

	BOTH_COCKPIT_SLEEP6START_GENERAL,		//# 
	BOTH_COCKPIT_SLEEP6STOP_GENERAL,		//# 

	BOTH_WIND_GENERAL,

	BOTH_STAND_TO_KNEEL_GENERAL,
	BOTH_KNEEL_TO_STAND_GENERAL,

	BOTH_TUSKENATTACK1_GENERAL,
	BOTH_TUSKENATTACK2_GENERAL,
	BOTH_TUSKENATTACK3_GENERAL,
	BOTH_TUSKENLUNGE1_GENERAL,
	BOTH_TUSKENTAUNT1_GENERAL,

	BOTH_COWER1_START_GENERAL,		//# cower start
	BOTH_COWER1_GENERAL,			//# cower loop
	BOTH_COWER1_STOP_GENERAL,		//# cower stop
	BOTH_SONICPAIN_START_GENERAL,
	BOTH_SONICPAIN_HOLD_GENERAL,
	BOTH_SONICPAIN_END_GENERAL,

	//new anim slots per Jarrod's request
	BOTH_STAND10_GENERAL,
	BOTH_STAND10_TALK1_GENERAL,
	BOTH_STAND10_TALK2_GENERAL,
	BOTH_STAND10TOSTAND1_GENERAL,

	BOTH_STAND1_TALK1_GENERAL,
	BOTH_STAND1_TALK2_GENERAL,
	BOTH_STAND1_TALK3_GENERAL,

	BOTH_SIT4_GENERAL,
	BOTH_SIT5_GENERAL,
	BOTH_SIT5_TALK1_GENERAL,
	BOTH_SIT5_TALK2_GENERAL,
	BOTH_SIT5_TALK3_GENERAL,

	BOTH_SIT6_GENERAL,
	BOTH_SIT7_GENERAL,

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP2_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP3_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP4_GENERAL,		//# Put weapon away
	TORSO_RAISEWEAP1_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP2_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP3_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP4_GENERAL,		//# Draw Weapon
	TORSO_WEAPONREADY1_GENERAL,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2_GENERAL,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3_GENERAL,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4_GENERAL,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY5_GENERAL,		//# Ready to fire bowcaster
	TORSO_WEAPONREADY6_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY7_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY8_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY9_GENERAL,		//# Ready to fire rocket launcher
	TORSO_WEAPONREADY10_GENERAL,	//# Ready to fire thermal det
	TORSO_WEAPONREADY11_GENERAL,	//# Ready to fire laser trap
	TORSO_WEAPONREADY12_GENERAL,	//# Ready to fire detpack
	TORSO_WEAPONIDLE1_GENERAL,		//# Holding stun baton
	TORSO_WEAPONIDLE2_GENERAL,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3_GENERAL,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4_GENERAL,		//# Holding sniper rifle
	TORSO_WEAPONIDLE5_GENERAL,		//# Holding bowcaster
	TORSO_WEAPONIDLE6_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE7_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE8_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE9_GENERAL,		//# Holding rocket launcher
	TORSO_WEAPONIDLE10_GENERAL,		//# Holding thermal det
	TORSO_WEAPONIDLE11_GENERAL,		//# Holding laser trap
	TORSO_WEAPONIDLE12_GENERAL,		//# Holding detpack

	//# #sep TORSO_ USING NON-WEAPON OBJECTS
	//# #sep TORSO_ MISC
	TORSO_TALKR1START_GENERAL,		//# begin turning head for BOTH_TORSO_TALKR
	TORSO_TALKR1HOLD_GENERAL,		//# non-looping version of talk to right side
	TORSO_TALKR1STOP_GENERAL,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKR1_GENERAL,			//# talk to right side
	TORSO_TALKL1START_GENERAL,		//# begin turning head for BOTH_TORSO_TALKL
	TORSO_TALKL1HOLD_GENERAL,		//# non-looping version of talk to left side
	TORSO_TALKL1STOP_GENERAL,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKL1_GENERAL,			//# talk to left side
	TORSO_LOOKL1_GENERAL,			//# looking left
	TORSO_LOOKR1_GENERAL,			//# looking right
	TORSO_LOOKR2START_GENERAL,		//# turn not so far as TALKR1
	TORSO_LOOKR2STOP_GENERAL,		//# turn not so far as TALKR1
	TORSO_LOOKR2_GENERAL,			//# looking right - not so far as LOOKR1
	TORSO_LOOKL2START_GENERAL,		//# turn not so far as TALKL1
	TORSO_LOOKL2STOP_GENERAL,		//# turn not so far as TALKL1
	TORSO_LOOKL2_GENERAL,			//# looking right - not so far as LOOKL1
	TORSO_HANDGESTURE1_GENERAL,		//# gestures to left one hand
	TORSO_HANDGESTURE2_GENERAL,		//# gestures to right one hand
	TORSO_HANDGESTURE3_GENERAL,		//# gestures to the left both hands
	TORSO_HANDGESTURE4_GENERAL,		//# gestures to the right both hands

	TORSO_HANDEXTEND1_GENERAL,		//# doctor reaching for hypospray in scav5
	TORSO_HANDRETRACT1_GENERAL,		//# doctor taking hypospray from player in scav5

	TORSO_DROPHELMET1_GENERAL,		//# Drop the helmet to the waist
	TORSO_RAISEHELMET1_GENERAL,		//# Bring the helmet to the head
	TORSO_REACHHELMET1_GENERAL,		//# reaching for helmet off of 60 tall cabinet
	TORSO_GRABLBACKL_GENERAL,		//# reach to lower back with left hand
	TORSO_GRABUBACKL_GENERAL,		//# reach to upper back with left hand
	TORSO_GRABLBACKR_GENERAL,		//# reach to lower back with right hand
	TORSO_GRABUBACKR_GENERAL,		//# reach to upper back with right hand

	TORSO_SURRENDER_START_GENERAL,	//# arms up
	TORSO_SURRENDER_STOP_GENERAL,	//# arms back down

	TORSO_CHOKING1_GENERAL,			//# TEMP

	TORSO_HANDSIGNAL1_GENERAL,
	TORSO_HANDSIGNAL2_GENERAL,
	TORSO_HANDSIGNAL3_GENERAL,
	TORSO_HANDSIGNAL4_GENERAL,
	TORSO_HANDSIGNAL5_GENERAL,

	// Q3
	TORSO_GESTURE_GENERAL,
	TORSO_ATTACK_GENERAL,
	TORSO_ATTACK2_GENERAL,
	TORSO_DROP_GENERAL,
	TORSO_RAISE_GENERAL,
	TORSO_STAND_GENERAL,
	TORSO_STAND2_GENERAL,
	TORSO_GETFLAG_GENERAL,
	TORSO_GUARDBASE_GENERAL,
	TORSO_PATROL_GENERAL,
	TORSO_FOLLOWME_GENERAL,
	TORSO_AFFIRMATIVE_GENERAL,
	TORSO_NEGATIVE_GENERAL,
	// Q3 end

	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_WALKBACK1_GENERAL,			//# Walk1 backwards
	LEGS_WALKBACK2_GENERAL,			//# Walk2 backwards
	LEGS_RUNBACK1_GENERAL,			//# Run1 backwards
	LEGS_RUNBACK2_GENERAL,			//# Run2 backwards
	LEGS_TURN1_GENERAL,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2_GENERAL,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1_GENERAL,		//# Lean left
	LEGS_LEAN_RIGHT1_GENERAL,		//# Lean Right
	LEGS_KNEELDOWN1_GENERAL,		//# Get down on one knee?
	LEGS_KNEELUP1_GENERAL,			//# Get up from one knee?
	LEGS_CRLEAN_LEFT1_GENERAL,		//# Crouch Lean left
	LEGS_CRLEAN_RIGHT1_GENERAL,		//# Crouch Lean Right
	LEGS_CHOKING1_GENERAL,			//# TEMP
	LEGS_LEFTUP1_GENERAL,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2_GENERAL,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3_GENERAL,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4_GENERAL,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5_GENERAL,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1_GENERAL,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2_GENERAL,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3_GENERAL,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4_GENERAL,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5_GENERAL,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1_GENERAL,
	LEGS_S1_LUP2_GENERAL,
	LEGS_S1_LUP3_GENERAL,
	LEGS_S1_LUP4_GENERAL,
	LEGS_S1_LUP5_GENERAL,
	LEGS_S1_RUP1_GENERAL,
	LEGS_S1_RUP2_GENERAL,
	LEGS_S1_RUP3_GENERAL,
	LEGS_S1_RUP4_GENERAL,
	LEGS_S1_RUP5_GENERAL,
	LEGS_S3_LUP1_GENERAL,
	LEGS_S3_LUP2_GENERAL,
	LEGS_S3_LUP3_GENERAL,
	LEGS_S3_LUP4_GENERAL,
	LEGS_S3_LUP5_GENERAL,
	LEGS_S3_RUP1_GENERAL,
	LEGS_S3_RUP2_GENERAL,
	LEGS_S3_RUP3_GENERAL,
	LEGS_S3_RUP4_GENERAL,
	LEGS_S3_RUP5_GENERAL,
	LEGS_S4_LUP1_GENERAL,
	LEGS_S4_LUP2_GENERAL,
	LEGS_S4_LUP3_GENERAL,
	LEGS_S4_LUP4_GENERAL,
	LEGS_S4_LUP5_GENERAL,
	LEGS_S4_RUP1_GENERAL,
	LEGS_S4_RUP2_GENERAL,
	LEGS_S4_RUP3_GENERAL,
	LEGS_S4_RUP4_GENERAL,
	LEGS_S4_RUP5_GENERAL,
	LEGS_S5_LUP1_GENERAL,
	LEGS_S5_LUP2_GENERAL,
	LEGS_S5_LUP3_GENERAL,
	LEGS_S5_LUP4_GENERAL,
	LEGS_S5_LUP5_GENERAL,
	LEGS_S5_RUP1_GENERAL,
	LEGS_S5_RUP2_GENERAL,
	LEGS_S5_RUP3_GENERAL,
	LEGS_S5_RUP4_GENERAL,
	LEGS_S5_RUP5_GENERAL,
	LEGS_S6_LUP1_GENERAL,
	LEGS_S6_LUP2_GENERAL,
	LEGS_S6_LUP3_GENERAL,
	LEGS_S6_LUP4_GENERAL,
	LEGS_S6_LUP5_GENERAL,
	LEGS_S6_RUP1_GENERAL,
	LEGS_S6_RUP2_GENERAL,
	LEGS_S6_RUP3_GENERAL,
	LEGS_S6_RUP4_GENERAL,
	LEGS_S6_RUP5_GENERAL,
	LEGS_S7_LUP1_GENERAL,
	LEGS_S7_LUP2_GENERAL,
	LEGS_S7_LUP3_GENERAL,
	LEGS_S7_LUP4_GENERAL,
	LEGS_S7_LUP5_GENERAL,
	LEGS_S7_RUP1_GENERAL,
	LEGS_S7_RUP2_GENERAL,
	LEGS_S7_RUP3_GENERAL,
	LEGS_S7_RUP4_GENERAL,
	LEGS_S7_RUP5_GENERAL,

	//New anim as per Jarrod's request
	LEGS_TURN180_GENERAL,

	// Q3
	LEGS_WALKCR_GENERAL,
	LEGS_WALK_GENERAL,
	LEGS_RUN_GENERAL,
	LEGS_BACK_GENERAL,
	LEGS_SWIM_GENERAL,

	LEGS_JUMP_GENERAL,
	LEGS_LAND_GENERAL,

	LEGS_JUMPB_GENERAL,
	LEGS_LANDB_GENERAL,

	LEGS_IDLE_GENERAL,
	LEGS_IDLECR_GENERAL,

	LEGS_TURN_GENERAL,
	LEGS_BACKCR_GENERAL,
	LEGS_BACKWALK_GENERAL,
	// Q3 end


	//======================================================
	//cinematic anims
	//======================================================
	//# #sep BOTH_ CINEMATIC-ONLY
	BOTH_CIN_1_GENERAL,		//# Level specific cinematic 1
	BOTH_CIN_2_GENERAL,		//# Level specific cinematic 2
	BOTH_CIN_3_GENERAL,		//# Level specific cinematic 3
	BOTH_CIN_4_GENERAL,		//# Level specific cinematic 4
	BOTH_CIN_5_GENERAL,		//# Level specific cinematic 5
	BOTH_CIN_6_GENERAL,		//# Level specific cinematic 6
	BOTH_CIN_7_GENERAL,		//# Level specific cinematic 7
	BOTH_CIN_8_GENERAL,		//# Level specific cinematic 8
	BOTH_CIN_9_GENERAL,		//# Level specific cinematic 9
	BOTH_CIN_10_GENERAL,		//# Level specific cinematic 10
	BOTH_CIN_11_GENERAL,		//# Level specific cinematic 11
	BOTH_CIN_12_GENERAL,		//# Level specific cinematic 12
	BOTH_CIN_13_GENERAL,		//# Level specific cinematic 13
	BOTH_CIN_14_GENERAL,		//# Level specific cinematic 14
	BOTH_CIN_15_GENERAL,		//# Level specific cinematic 15
	BOTH_CIN_16_GENERAL,		//# Level specific cinematic 16
	BOTH_CIN_17_GENERAL,		//# Level specific cinematic 17
	BOTH_CIN_18_GENERAL,		//# Level specific cinematic 18
	BOTH_CIN_19_GENERAL,		//# Level specific cinematic 19
	BOTH_CIN_20_GENERAL,		//# Level specific cinematic 20
	BOTH_CIN_21_GENERAL,		//# Level specific cinematic 21
	BOTH_CIN_22_GENERAL,		//# Level specific cinematic 22
	BOTH_CIN_23_GENERAL,		//# Level specific cinematic 23
	BOTH_CIN_24_GENERAL,		//# Level specific cinematic 24
	BOTH_CIN_25_GENERAL,		//# Level specific cinematic 25
	BOTH_CIN_26_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_27_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_28_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_29_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_30_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_31_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_32_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_33_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_34_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_35_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_36_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_37_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_38_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_39_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_40_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_41_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_42_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_43_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_44_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_45_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_46_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_47_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_48_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_49_GENERAL,		//# Level specific cinematic 
	BOTH_CIN_50_GENERAL,		//# Level specific cinematic

	//Boot
	BOOT_BLOCK_DIAG_LEFT_GENERAL,
	BOOT_BLOCK_DIAG_RIGHT_GENERAL,
	BOOT_UNUSED3_GENERAL,
	BOOT_UNUSED4_GENERAL,
	BOOT_UNUSED5_GENERAL,
	BOOT_UNUSED6_GENERAL,
	BOOT_UNUSED7_GENERAL,
	BOOT_UNUSED8_GENERAL,
	BOOT_UNUSED9_GENERAL,
	BOOT_UNUSED10_GENERAL,
	BOOT_UNUSED11_GENERAL,
	BOOT_UNUSED12_GENERAL,
	BOOT_UNUSED13_GENERAL,
	BOOT_UNUSED14_GENERAL,
	BOOT_UNUSED15_GENERAL,
	BOOT_UNUSED16_GENERAL,
	//

	// Q3
	FLAG_RUN_GENERAL,
	FLAG_STAND_GENERAL,
	FLAG_STAND2RUN_GENERAL,
	// Q3 End

	//# #eol
	MAX_ANIMATIONS_GENERAL,
	MAX_TOTALANIMATIONS_GENERAL,
} animNumberGeneral_t;





const static int jk2dm16AnimsToGeneral[] //# animNumber_e
{
	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	BOTH_1CRUFTFORGIL_GENERAL,		//# G2 cannot have a reverse anim at beginning of file
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1_GENERAL,		//# First Death anim
	BOTH_DEATH2_GENERAL,			//# Second Death anim
	BOTH_DEATH3_GENERAL,			//# Third Death anim
	BOTH_DEATH4_GENERAL,			//# Fourth Death anim
	BOTH_DEATH5_GENERAL,			//# Fifth Death anim
	BOTH_DEATH6_GENERAL,			//# Sixth Death anim
	BOTH_DEATH7_GENERAL,			//# Seventh Death anim
	BOTH_DEATH8_GENERAL,			//# 
	BOTH_DEATH9_GENERAL,			//# 
	BOTH_DEATH10_GENERAL,			//# 
	BOTH_DEATH11_GENERAL,			//#
	BOTH_DEATH12_GENERAL,			//# 
	BOTH_DEATH13_GENERAL,			//# 
	BOTH_DEATH14_GENERAL,			//# 
	BOTH_DEATH15_GENERAL,			//# 
	BOTH_DEATH16_GENERAL,			//# 
	BOTH_DEATH17_GENERAL,			//# 
	BOTH_DEATH18_GENERAL,			//# 
	BOTH_DEATH19_GENERAL,			//# 
	BOTH_DEATH20_GENERAL,			//# 
	BOTH_DEATH21_GENERAL,			//# 
	BOTH_DEATH22_GENERAL,			//# 
	BOTH_DEATH23_GENERAL,			//# 
	BOTH_DEATH24_GENERAL,			//# 
	BOTH_DEATH25_GENERAL,			//# 

	BOTH_DEATHFORWARD1_GENERAL,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2_GENERAL,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3_GENERAL,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1_GENERAL,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2_GENERAL,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE_GENERAL,		//# Idle while close to death
	BOTH_LYINGDEATH1_GENERAL,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1_GENERAL,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1_GENERAL,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR_GENERAL,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND_GENERAL,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL_GENERAL,		//# Death anim from a roll
	BOTH_DEATH_FLIP_GENERAL,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R_GENERAL,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L_GENERAL,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180_GENERAL,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP_GENERAL,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN_GENERAL,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN_GENERAL,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP_GENERAL,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED_GENERAL,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1_GENERAL,				//# First Death finished pose
	BOTH_DEAD2_GENERAL,				//# Second Death finished pose
	BOTH_DEAD3_GENERAL,				//# Third Death finished pose
	BOTH_DEAD4_GENERAL,				//# Fourth Death finished pose
	BOTH_DEAD5_GENERAL,				//# Fifth Death finished pose
	BOTH_DEAD6_GENERAL,				//# Sixth Death finished pose
	BOTH_DEAD7_GENERAL,				//# Seventh Death finished pose
	BOTH_DEAD8_GENERAL,				//# 
	BOTH_DEAD9_GENERAL,				//# 
	BOTH_DEAD10_GENERAL,			//# 
	BOTH_DEAD11_GENERAL,			//#
	BOTH_DEAD12_GENERAL,			//# 
	BOTH_DEAD13_GENERAL,			//# 
	BOTH_DEAD14_GENERAL,			//# 
	BOTH_DEAD15_GENERAL,			//# 
	BOTH_DEAD16_GENERAL,			//# 
	BOTH_DEAD17_GENERAL,			//# 
	BOTH_DEAD18_GENERAL,			//# 
	BOTH_DEAD19_GENERAL,			//# 
	BOTH_DEAD20_GENERAL,			//# 
	BOTH_DEAD21_GENERAL,			//# 
	BOTH_DEAD22_GENERAL,			//# 
	BOTH_DEAD23_GENERAL,			//# 
	BOTH_DEAD24_GENERAL,			//# 
	BOTH_DEAD25_GENERAL,			//# 
	BOTH_DEADFORWARD1_GENERAL,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2_GENERAL,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_GENERAL,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2_GENERAL,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1_GENERAL,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_GENERAL,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND_GENERAL,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1_GENERAL,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2_GENERAL,		//# React to being shot from Second Death finished pose
	BOTH_DEADFLOP3_GENERAL,		//# React to being shot from Third Death finished pose
	BOTH_DEADFLOP4_GENERAL,		//# React to being shot from Fourth Death finished pose
	BOTH_DEADFLOP5_GENERAL,		//# React to being shot from Fifth Death finished pose 
	BOTH_DEADFORWARD1_FLOP_GENERAL,		//# React to being shot First thrown forward death finished pose
	BOTH_DEADFORWARD2_FLOP_GENERAL,		//# React to being shot Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_FLOP_GENERAL,	//# React to being shot First thrown backward death finished pose
	BOTH_DEADBACKWARD2_FLOP_GENERAL,	//# React to being shot Second thrown backward death finished pose
	BOTH_LYINGDEAD1_FLOP_GENERAL,		//# React to being shot Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_FLOP_GENERAL,		//# React to being shot Stumble forward death finished pose
	BOTH_FALLDEAD1_FLOP_GENERAL,	//# React to being shot Fall forward and splat death finished pose
	BOTH_DISMEMBER_HEAD1_GENERAL,	//#
	BOTH_DISMEMBER_TORSO1_GENERAL,	//#
	BOTH_DISMEMBER_LLEG_GENERAL,	//#
	BOTH_DISMEMBER_RLEG_GENERAL,	//#
	BOTH_DISMEMBER_RARM_GENERAL,	//#
	BOTH_DISMEMBER_LARM_GENERAL,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1_GENERAL,				//# First take pain anim
	BOTH_PAIN2_GENERAL,				//# Second take pain anim
	BOTH_PAIN3_GENERAL,				//# Third take pain anim
	BOTH_PAIN4_GENERAL,				//# Fourth take pain anim
	BOTH_PAIN5_GENERAL,				//# Fifth take pain anim - from behind
	BOTH_PAIN6_GENERAL,				//# Sixth take pain anim - from behind
	BOTH_PAIN7_GENERAL,				//# Seventh take pain anim - from behind
	BOTH_PAIN8_GENERAL,				//# Eigth take pain anim - from behind
	BOTH_PAIN9_GENERAL,				//# 
	BOTH_PAIN10_GENERAL,			//# 
	BOTH_PAIN11_GENERAL,			//# 
	BOTH_PAIN12_GENERAL,			//# 
	BOTH_PAIN13_GENERAL,			//# 
	BOTH_PAIN14_GENERAL,			//# 
	BOTH_PAIN15_GENERAL,			//# 
	BOTH_PAIN16_GENERAL,			//# 
	BOTH_PAIN17_GENERAL,			//# 
	BOTH_PAIN18_GENERAL,			//# 
	BOTH_PAIN19_GENERAL,			//# 
	BOTH_PAIN20_GENERAL,			//# GETTING SHOCKED

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1_GENERAL,			//# Attack with stun baton
	BOTH_ATTACK2_GENERAL,			//# Attack with one-handed pistol
	BOTH_ATTACK2IDLE1_GENERAL,		//# Idle with one-handed pistol
	BOTH_ATTACK3_GENERAL,			//# Attack with blaster rifle
	BOTH_ATTACK4_GENERAL,			//# Attack with disruptor
	BOTH_ATTACK5_GENERAL,			//# Attack with bow caster
	BOTH_ATTACK6_GENERAL,			//# Attack with ???
	BOTH_ATTACK7_GENERAL,			//# Attack with ???
	BOTH_ATTACK8_GENERAL,			//# Attack with ???
	BOTH_ATTACK9_GENERAL,			//# Attack with rocket launcher
	BOTH_ATTACK10_GENERAL,			//# Attack with thermal det
	BOTH_ATTACK11_GENERAL,			//# Attack with laser trap
	BOTH_ATTACK12_GENERAL,			//# Attack with detpack
	BOTH_MELEE1_GENERAL,			//# First melee attack
	BOTH_MELEE2_GENERAL,			//# Second melee attack
	BOTH_MELEE3_GENERAL,			//# Third melee attack
	BOTH_MELEE4_GENERAL,			//# Fourth melee attack
	BOTH_MELEE5_GENERAL,			//# Fifth melee attack
	BOTH_MELEE6_GENERAL,			//# Sixth melee attack
	BOTH_THERMAL_READY_GENERAL,		//# pull back with thermal
	BOTH_THERMAL_THROW_GENERAL,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 2
	BOTH_A1_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T1__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T1__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T1__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T1_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T1_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T1_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T1_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T1_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T1_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T1_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T1__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T1_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B1_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR____GENERAL,	//# Deflection toward BR
	BOTH_D1__R____GENERAL,	//# Deflection toward R
	BOTH_D1_TR____GENERAL,	//# Deflection toward TR
	BOTH_D1_TL____GENERAL,	//# Deflection toward TL
	BOTH_D1__L____GENERAL,	//# Deflection toward L
	BOTH_D1_BL____GENERAL,	//# Deflection toward BL
	BOTH_D1_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T2__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T2__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T2__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T2_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T2_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T2_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T2_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T2_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T2_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T2_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T2__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T2_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B2_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR____GENERAL,	//# Deflection toward BR
	BOTH_D2__R____GENERAL,	//# Deflection toward R
	BOTH_D2_TR____GENERAL,	//# Deflection toward TR
	BOTH_D2_TL____GENERAL,	//# Deflection toward TL
	BOTH_D2__L____GENERAL,	//# Deflection toward L
	BOTH_D2_BL____GENERAL,	//# Deflection toward BL
	BOTH_D2_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T3__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T3__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T3__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T3_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T3_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T3_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T3_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T3_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T3_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T3_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T3__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T3_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B3_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR____GENERAL,	//# Deflection toward BR
	BOTH_D3__R____GENERAL,	//# Deflection toward R
	BOTH_D3_TR____GENERAL,	//# Deflection toward TR
	BOTH_D3_TL____GENERAL,	//# Deflection toward TL
	BOTH_D3__L____GENERAL,	//# Deflection toward L
	BOTH_D3_BL____GENERAL,	//# Deflection toward BL
	BOTH_D3_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T4__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T4__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T4__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T4_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T4_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T4_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T4_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T4_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T4_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T4_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T4__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T4_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B4_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR____GENERAL,	//# Deflection toward BR
	BOTH_D4__R____GENERAL,	//# Deflection toward R
	BOTH_D4_TR____GENERAL,	//# Deflection toward TR
	BOTH_D4_TL____GENERAL,	//# Deflection toward TL
	BOTH_D4__L____GENERAL,	//# Deflection toward L
	BOTH_D4_BL____GENERAL,	//# Deflection toward BL
	BOTH_D4_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T5__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T5__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T5__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T5_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T5_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T5_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T5_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T5_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T5_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T5_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T5__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T5_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B5_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR____GENERAL,	//# Deflection toward BR
	BOTH_D5__R____GENERAL,	//# Deflection toward R
	BOTH_D5_TR____GENERAL,	//# Deflection toward TR
	BOTH_D5_TL____GENERAL,	//# Deflection toward TL
	BOTH_D5__L____GENERAL,	//# Deflection toward L
	BOTH_D5_BL____GENERAL,	//# Deflection toward BL
	BOTH_D5_B_____GENERAL,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T__GENERAL,	//# Block shot/saber top
	BOTH_P1_S1_TR_GENERAL,	//# Block shot/saber top right
	BOTH_P1_S1_TL_GENERAL,	//# Block shot/saber top left
	BOTH_P1_S1_BL_GENERAL,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR_GENERAL,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T__GENERAL,	//# knockaway saber top
	BOTH_K1_S1_TR_GENERAL,	//# knockaway saber top right
	BOTH_K1_S1_TL_GENERAL,	//# knockaway saber top left
	BOTH_K1_S1_BL_GENERAL,	//# knockaway saber bottom left
	BOTH_K1_S1_B__GENERAL,	//# knockaway saber bottom
	BOTH_K1_S1_BR_GENERAL,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1_GENERAL,	//# BR attack knocked away
	BOTH_V1__R_S1_GENERAL,	//# R attack knocked away
	BOTH_V1_TR_S1_GENERAL,	//# TR attack knocked away
	BOTH_V1_T__S1_GENERAL,	//# T attack knocked away
	BOTH_V1_TL_S1_GENERAL,	//# TL attack knocked away
	BOTH_V1__L_S1_GENERAL,	//# L attack knocked away
	BOTH_V1_BL_S1_GENERAL,	//# BL attack knocked away
	BOTH_V1_B__S1_GENERAL,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T__GENERAL,	//# saber knocked down from top parry
	BOTH_H1_S1_TR_GENERAL,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL_GENERAL,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL_GENERAL,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B__GENERAL,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR_GENERAL,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	BOTH_BF2RETURN_GENERAL,	//#
	BOTH_BF2BREAK_GENERAL,	//#
	BOTH_BF2LOCK_GENERAL,	//#
	BOTH_BF1RETURN_GENERAL,	//#
	BOTH_BF1BREAK_GENERAL,	//#
	BOTH_BF1LOCK_GENERAL,	//#
	BOTH_CWCIRCLE_R2__R_S1_GENERAL,	//#
	BOTH_CCWCIRCLE_R2__L_S1_GENERAL,	//#
	BOTH_CWCIRCLE_A2__L__R_GENERAL,	//#
	BOTH_CCWCIRCLE_A2__R__L_GENERAL,	//#
	BOTH_CWCIRCLEBREAK_GENERAL,	//#
	BOTH_CCWCIRCLEBREAK_GENERAL,	//#
	BOTH_CWCIRCLELOCK_GENERAL,	//#
	BOTH_CCWCIRCLELOCK_GENERAL,	//#

	BOTH_SABERFAST_STANCE_GENERAL,
	BOTH_SABERSLOW_STANCE_GENERAL,
	BOTH_A2_STABBACK1_GENERAL,		//# Stab saber backward
	BOTH_ATTACK_BACK_GENERAL,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1_GENERAL,//#
	BOTH_JUMPFLIPSTABDOWN_GENERAL,//#
	BOTH_FORCELEAP2_T__B__GENERAL,//#
	BOTH_LUNGE2_B__T__GENERAL,//#
	BOTH_CROUCHATTACKBACK1_GENERAL,//#

	//# #sep BOTH_ STANDING
	BOTH_STAND1_GENERAL,			//# Standing idle_GENERAL, no weapon_GENERAL, hands down
	BOTH_STAND1IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND2_GENERAL,			//# Standing idle with a saber
	BOTH_STAND2IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND2IDLE2_GENERAL,		//# Random standing idle
	BOTH_STAND3_GENERAL,			//# Standing idle with 2-handed weapon
	BOTH_STAND3IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND4_GENERAL,			//# hands clasp behind back
	BOTH_STAND4IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND5_GENERAL,			//# standing idle_GENERAL, no weapon_GENERAL, hand down_GENERAL, back straight
	BOTH_STAND5IDLE1_GENERAL,		//# Random standing idle
	BOTH_STAND6_GENERAL,			//# one handed_GENERAL, gun at side_GENERAL, relaxed stand
	BOTH_STAND7_GENERAL,			//# both hands on hips (female)
	BOTH_STAND8_GENERAL,			//# both hands on hips (male)
	BOTH_STAND1TO3_GENERAL,			//# Transition from stand1 to stand3
	BOTH_STAND3TO1_GENERAL,			//# Transition from stand3 to stand1
	BOTH_STAND1TO2_GENERAL,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1_GENERAL,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4_GENERAL,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2_GENERAL,			//# Transition from stand4 to stand2
	BOTH_STANDTOWALK1_GENERAL,		//# Transition from stand1 to walk1
	BOTH_STAND4TOATTACK2_GENERAL,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP1_GENERAL,			//# standing up and stumbling
	BOTH_STANDUP2_GENERAL,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3_GENERAL,		//# transition from stand 5 to sit 3
	BOTH_STAND1_REELO_GENERAL,		//# Reelo in his stand1 position (cin #18)
	BOTH_STAND5_REELO_GENERAL,		//# Reelo in his stand5 position (cin #18)
	BOTH_STAND1TOSTAND5_GENERAL,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1_GENERAL,	//# Transition from stand5 to stand1
	BOTH_STAND5TOAIM_GENERAL,		//# Transition of Kye aiming his gun at Desann (cin #9) 
	BOTH_STAND5STARTLEDLOOKLEFT_GENERAL,	//# Kyle turning to watch the bridge drop (cin #9) 
	BOTH_STARTLEDLOOKLEFTTOSTAND5_GENERAL,	//# Kyle returning to stand 5 from watching the bridge drop (cin #9) 
	BOTH_STAND5TOSTAND8_GENERAL,	//# Transition from stand5 to stand8
	BOTH_STAND7TOSTAND8_GENERAL,	//# Tavion putting hands on back of chair (cin #11)
	BOTH_STAND8TOSTAND5_GENERAL,	//# Transition from stand8 to stand5
	BOTH_STAND5SHIFTWEIGHT_GENERAL,	//# Weightshift from stand5 to side and back to stand5
	BOTH_STAND5SHIFTWEIGHTSTART_GENERAL,	//# From stand5 to side
	BOTH_STAND5SHIFTWEIGHTSTOP_GENERAL,		//# From side to stand5
	BOTH_STAND5TURNLEFTSTART_GENERAL,		//# Start turning left from stand5
	BOTH_STAND5TURNLEFTSTOP_GENERAL,		//# Stop turning left from stand5
	BOTH_STAND5TURNRIGHTSTART_GENERAL,		//# Start turning right from stand5
	BOTH_STAND5TURNRIGHTSTOP_GENERAL,		//# Stop turning right from stand5
	BOTH_STAND5LOOK180LEFTSTART_GENERAL,	//# Start looking over left shoulder (cin #17)
	BOTH_STAND5LOOK180LEFTSTOP_GENERAL,	//# Stop looking over left shoulder (cin #17)

	BOTH_CONSOLE1START_GENERAL,		//# typing at a console
	BOTH_CONSOLE1_GENERAL,			//# typing at a console
	BOTH_CONSOLE1STOP_GENERAL,		//# typing at a console
	BOTH_CONSOLE2START_GENERAL,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2_GENERAL,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP_GENERAL,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTART_GENERAL,	//# lean in to type at console while holding comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTOP_GENERAL,	//# lean away after typing at console while holding comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1_GENERAL,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1_GENERAL,		//# Cradling weapon and standing
	BOTH_ALERT1_GENERAL,			//# Startled by something while on guard
	BOTH_GESTURE1_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE2_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE3_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_WALK1TALKCOMM1_GENERAL,	//# Talking into coom link while walking
	BOTH_TALK1_GENERAL,				//# Generic talk anim
	BOTH_TALK2_GENERAL,				//# Generic talk anim
	BOTH_TALKCOMM1START_GENERAL,	//# Start talking into a comm link
	BOTH_TALKCOMM1_GENERAL,			//# Talking into a comm link
	BOTH_TALKCOMM1STOP_GENERAL,		//# Stop talking into a comm link
	BOTH_TALKGESTURE1_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE2_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE3_GENERAL,		//# Generic talk anim
	BOTH_TALKGESTURE4START_GENERAL,	//# Beginning talk anim 4
	BOTH_TALKGESTURE4_GENERAL,		//# Talk gesture 4
	BOTH_TALKGESTURE4STOP_GENERAL,	//# Ending talk anim 4
	BOTH_TALKGESTURE5START_GENERAL,	//# Start hand on chin
	BOTH_TALKGESTURE5_GENERAL,		//# Hand on chin
	BOTH_TALKGESTURE5STOP_GENERAL,	//# Stop hand on chin
	BOTH_TALKGESTURE6START_GENERAL,	//# Starting Motions to self
	BOTH_TALKGESTURE6_GENERAL,		//# Pointing at self
	BOTH_TALKGESTURE6STOP_GENERAL,	//# Ending Motions to self
	BOTH_TALKGESTURE7START_GENERAL,	//# Start touches Kyle on shoulder
	BOTH_TALKGESTURE7_GENERAL,		//# Hold touches Kyle on shoulder
	BOTH_TALKGESTURE7STOP_GENERAL,	//# Ending touches Kyle on shoulder
	BOTH_TALKGESTURE8START_GENERAL,	//# Lando's chin hold 
	BOTH_TALKGESTURE8_GENERAL,			//# Lando's chin hold 
	BOTH_TALKGESTURE8STOP_GENERAL,	//# Lando's chin hold 
	BOTH_TALKGESTURE9_GENERAL,		//# Same as gesture 2 but with the right hand
	BOTH_TALKGESTURE10_GENERAL,		//# Shoulder shrug
	BOTH_TALKGESTURE11START_GENERAL,	//# Arms folded across chest
	BOTH_TALKGESTURE11STOP_GENERAL,	//# Arms folded across chest
	BOTH_TALKGESTURE12_GENERAL,		//# Tavion taunting Kyle
	BOTH_TALKGESTURE13START_GENERAL,	//# Luke warning Kyle
	BOTH_TALKGESTURE13_GENERAL,			//# Luke warning Kyle
	BOTH_TALKGESTURE13STOP_GENERAL,	//# Luke warning Kyle
	BOTH_TALKGESTURE14_GENERAL,			//# Luke gesturing to Kyle
	BOTH_TALKGESTURE15START_GENERAL,	//# Desann taunting Kyle
	BOTH_TALKGESTURE15_GENERAL,			//# Desann taunting Kyle
	BOTH_TALKGESTURE15STOP_GENERAL,		//# Desann taunting Kyle
	BOTH_TALKGESTURE16_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE17_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE18_GENERAL,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE19START_GENERAL,	//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE19STOP_GENERAL,		//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE20START_GENERAL,	//# Kyle lifting his arm "Join us" (cin #34)
	BOTH_TALKGESTURE21_GENERAL,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE22_GENERAL,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE23_GENERAL,			//# generic talk gesture from stand3
	BOTH_PAUSE1START_GENERAL,			//# Luke pauses to warn Kyle (cin #24) start
	BOTH_PAUSE1STOP_GENERAL,			//# Luke pauses to warn Kyle (cin #24) stop

	BOTH_HEADTILTLSTART_GENERAL,		//# Head tilt to left
	BOTH_HEADTILTLSTOP_GENERAL,			//# Head tilt to left
	BOTH_HEADTILTRSTART_GENERAL,		//# Head tilt to right
	BOTH_HEADTILTRSTOP_GENERAL,			//# Head tilt to right
	BOTH_HEADNOD_GENERAL,				//# Head shake YES
	BOTH_HEADSHAKE_GENERAL,				//# Head shake NO
	BOTH_HEADSHAKE1_REELO_GENERAL,		//# Head shake NO for Reelo
	BOTH_SITHEADTILTLSTART_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTLSTOP_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTRSTART_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADTILTRSTOP_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADNOD_GENERAL,			//# Head shake YES from seated position
	BOTH_SITHEADSHAKE_GENERAL,			//# Head shake NO from seated position
	BOTH_SIT2HEADTILTLSTART_GENERAL,	//# Head tilt to left from seated position 2
	BOTH_SIT2HEADTILTLSTOP_GENERAL,		//# Head tilt to left from seated position 2
 
	BOTH_REACH1START_GENERAL,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP_GENERAL,		//# Monmothma reaching for crystal

	BOTH_EXAMINE1START_GENERAL,		//# Start Mon Mothma examining crystal 
	BOTH_EXAMINE1_GENERAL,			//# Mon Mothma examining crystal 
	BOTH_EXAMINE1STOP_GENERAL,		//# Stop Mon Mothma examining crystal 
	BOTH_EXAMINE2START_GENERAL,	//# Start Kyle tossing crystal
	BOTH_EXAMINE2_GENERAL,			//# Hold Kyle tossing crystal
	BOTH_EXAMINE2STOP_GENERAL,		//# End Kyle tossing crystal
	BOTH_EXAMINE3START_GENERAL,	//# Start Lando looking around corner
	BOTH_EXAMINE3_GENERAL,			//# Hold Lando looking around corner
	BOTH_EXAMINE3STOP_GENERAL,		//# End Lando looking around corner

	BOTH_LEANLEFT2START_GENERAL,	//# Start leaning left in chair
	BOTH_LEANLEFT2STOP_GENERAL,		//# Stop leaning left in chair
	BOTH_LEANRIGHT3START_GENERAL,	//# Start Lando leaning on wall
	BOTH_LEANRIGHT3_GENERAL,			//# Lando leaning on wall
	BOTH_LEANRIGHT3STOP_GENERAL,		//# Stop Lando leaning on wall

	BOTH_FORCEFOUNTAIN1_START_GENERAL,	//# Kyle being lifted into the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_MIDDLE_GENERAL,	//# Kyle changing to looping position in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_LOOP_GENERAL,	//# Kyle being spun in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_STOP_GENERAL,	//# Kyle being set down out of the Force Fountain (cin #10)
	BOTH_THUMBING1_GENERAL,				//# Lando gesturing with thumb over his shoulder (cin #19)
	BOTH_COME_ON1_GENERAL,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1_GENERAL,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END_GENERAL,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1_GENERAL,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1_GENERAL,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_PUNCHER1_GENERAL,				//# Jan punching Kyle in the shoulder (cin #37)
	BOTH_CONSTRAINER1HOLD_GENERAL,		//# Static pose of starting Tavion constraining Jan (cin #9)
	BOTH_CONSTRAINEE1HOLD_GENERAL,		//# Static pose of starting Jan being constrained by Tavion (cin #9)
	BOTH_CONSTRAINER1STAND_GENERAL,		//# Tavion constraining Jan in a stand pose (cin #9)
	BOTH_CONSTRAINEE1STAND_GENERAL,		//# Jan being constrained in a stand pose (cin #9)
	BOTH_CONSTRAINER1WALK_GENERAL,		//# Tavion shoving jan forward (cin #9)
	BOTH_CONSTRAINEE1WALK_GENERAL,		//# Jan being shoved forward by Tavion (cin #9)
	BOTH_CONSTRAINER1LOOP_GENERAL,		//# Tavion walking with Jan in a loop (cin #9)
	BOTH_CONSTRAINEE1LOOP_GENERAL,		//# Jan walking with Tavion in a loop (cin #9)
	BOTH_SABERKILLER1_GENERAL,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1_GENERAL,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HANDSHAKER1START_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKER1LOOP_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKEE1START_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_HANDSHAKEE1LOOP_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_LAUGH1START_GENERAL,			//# Reelo leaning forward before laughing (cin #18)
	BOTH_LAUGH1STOP_GENERAL,			//# Reelo laughing (cin #18)
	BOTH_ESCAPEPOD_LEAVE1_GENERAL,	//# Kyle leaving escape pod (cin #33)
	BOTH_ESCAPEPOD_LEAVE2_GENERAL,	//# Jan leaving escape pod (cin #33)
	BOTH_HUGGER1_GENERAL,			//# Kyle hugging Jan (cin #29)
	BOTH_HUGGERSTOP1_GENERAL,		//# Kyle stop hugging Jan but don't let her go (cin #29)
	BOTH_HUGGERSTOP2_GENERAL,		//# Kyle let go of Jan and step back (cin #29)
	BOTH_HUGGEE1_GENERAL,			//# Jan being hugged (cin #29)
	BOTH_HUGGEESTOP1_GENERAL,		//# Jan stop being hugged but don't let go (cin #29)
	BOTH_HUGGEESTOP2_GENERAL,		//# Jan released from hug (cin #29)
	BOTH_KISSER1_GENERAL,			//# Temp until the Kiss anim gets split up
	BOTH_KISSER1START1_GENERAL,		//# Kyle start kissing Jan
	BOTH_KISSER1START2_GENERAL,		//# Kyle start kissing Jan
	BOTH_KISSER1LOOP_GENERAL,		//# Kyle loop kissing Jan
	BOTH_KISSER1STOP_GENERAL,		//# Temp until the Kiss anim gets split up
	BOTH_KISSER1STOP1_GENERAL,		//# Kyle stop kissing but don't let go
	BOTH_KISSER1STOP2_GENERAL,		//# Kyle step back from Jan
	BOTH_KISSEE1_GENERAL,			//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1START1_GENERAL,		//# Jan start being kissed
	BOTH_KISSEE1START2_GENERAL,		//# Jan start being kissed
	BOTH_KISSEE1LOOP_GENERAL,		//# Jan loop being kissed
	BOTH_KISSEE1STOP_GENERAL,		//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1STOP1_GENERAL,		//# Jan stop being kissed but don't let go
	BOTH_KISSEE1STOP2_GENERAL,		//# Jan wait for Kyle to step back
	BOTH_BARTENDER_IDLE1_GENERAL,	//# Bartender idle in cin #15
	BOTH_BARTENDER_THROW1_GENERAL,	//# Bartender throws glass in cin #15
	BOTH_BARTENDER_COWERSTART_GENERAL,	//# Start of Bartender raising both hands up in surrender (cin #16)
	BOTH_BARTENDER_COWERLOOP_GENERAL,	//# Loop of Bartender waving both hands in surrender (cin #16)
	BOTH_BARTENDER_COWER_GENERAL,		//# Single frame of Bartender waving both hands in surrender (cin #16)
	BOTH_THREATEN1_START_GENERAL,	//# First frame of Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_THREATEN1_GENERAL,			//# Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_RADIO_ONOFF_GENERAL,		//# Mech Galak turning on his suit radio (cin #32)
	BOTH_TRIUMPHANT1START_GENERAL,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STARTGESTURE_GENERAL,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STOP_GENERAL,	//# Mech Galak lowering his arms in victory (cin #32)

	BOTH_SABERTHROW1START_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START_GENERAL,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP_GENERAL,		//# Kyle throwing his light saber (cin #32)

	BOTH_COVERUP1_LOOP_GENERAL,		//# animation of getting in line of friendly fire
	BOTH_COVERUP1_START_GENERAL,	//# transitions from stand to coverup1_loop
	BOTH_COVERUP1_END_GENERAL,		//# transitions from coverup1_loop to stand

	BOTH_INJURED4_GENERAL,			//# Injured pose 4
	BOTH_INJURED4TO5_GENERAL,		//# Transition from INJURED4 to INJURED5
	BOTH_INJURED5_GENERAL,			//# Injured pose 5

	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1STAND_GENERAL,			//# Stand up from First sitting anim
	BOTH_SIT1_GENERAL,				//# Normal chair sit.
	BOTH_SIT2_GENERAL,				//# Lotus position.
	BOTH_SIT3_GENERAL,				//# Sitting in tired position_GENERAL, elbows on knees

	BOTH_SIT2TO3_GENERAL,			//# Trans from sit2 to sit3?
	BOTH_SIT2TOSTAND5_GENERAL,		//# Transition from sit 2 to stand 5
	BOTH_STAND5TOSIT2_GENERAL,		//# Transition from stand 5 to sit 2
	BOTH_SIT2TOSIT4_GENERAL,		//# Trans from sit2 to sit4 (cin #12) Luke leaning back from lotus position.
	BOTH_SIT3TO1_GENERAL,			//# Trans from sit3 to sit1?
	BOTH_SIT3TO2_GENERAL,			//# Trans from sit3 to sit2?
	BOTH_SIT3TOSTAND5_GENERAL,		//# transition from sit 3 to stand 5

	BOTH_SIT4TO5_GENERAL,			//# Trans from sit4 to sit5
	BOTH_SIT4TO6_GENERAL,			//# Trans from sit4 to sit6
	BOTH_SIT5TO4_GENERAL,			//# Trans from sit5 to sit4
	BOTH_SIT5TO6_GENERAL,			//# Trans from sit5 to sit6
	BOTH_SIT6TO4_GENERAL,			//# Trans from sit6 to sit4
	BOTH_SIT6TO5_GENERAL,			//# Trans from sit6 to sit5
	BOTH_SIT7_GENERAL,				//# sitting with arms over knees_GENERAL, no weapon
	BOTH_SIT7TOSTAND1_GENERAL,		//# getting up from sit7 into stand1

	BOTH_CROUCH1_GENERAL,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE_GENERAL,		//# Crouching idle
	BOTH_CROUCH1WALK_GENERAL,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK_GENERAL,	//# Walking while crouched
	BOTH_UNCROUCH1_GENERAL,			//# Transition from crouch to standing
	BOTH_CROUCH2IDLE_GENERAL,		//# crouch and resting on back righ heel_GENERAL, no weapon
	BOTH_CROUCH2TOSTAND1_GENERAL,	//# going from crouch2 to stand1
	BOTH_CROUCH3_GENERAL,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3_GENERAL,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_CROUCH4_GENERAL,			//# Slower version of crouch1 for cinematics
	BOTH_UNCROUCH4_GENERAL,			//# Slower version of uncrouch1 for cinematics
	BOTH_GET_UP1_GENERAL,			//# Get up from the ground_GENERAL, face down
	BOTH_GET_UP2_GENERAL,			//# Get up from the ground_GENERAL, face up

	BOTH_COCKPIT_SIT_GENERAL,			//# sit in a cockpit.

	BOTH_GUNSIT1_GENERAL,			//# sitting on an emplaced gun.

	BOTH_DEATH14_UNGRIP_GENERAL,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP_GENERAL,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_KNEES1_GENERAL,			//# Tavion on her knees
	BOTH_KNEES2_GENERAL,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1_GENERAL,			//# Transition of KNEES2 to KNEES1
	BOTH_RUMMAGE1START_GENERAL,	//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1_GENERAL,			//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1STOP_GENERAL,		//# Kyle rummaging for crystal (cin 2)
	BOTH_HOLDGLASS1_GENERAL,		//# Bartender holds glass (cin# 14)
	BOTH_SLIDEGLASS1_GENERAL,		//# Bartender slides glass (cin# 14)
	BOTH_SLAMSABERDOWN_GENERAL,		//# Kyle slamming his saber on the bar top (cin# 14)

	//# #sep BOTH_ MOVING
	BOTH_WALK1_GENERAL,				//# Normal walk
	BOTH_WALK2_GENERAL,				//# Normal walk
	BOTH_WALK3_GENERAL,				//# Goes with stand3
	BOTH_WALK4_GENERAL,				//# Walk cycle goes to a stand4
	BOTH_WALK5_GENERAL,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6_GENERAL,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7_GENERAL,				//# Fast walk
	BOTH_WALK8_GENERAL,				//# Normal walk with hands behind back (Luke in cin#12)
	BOTH_WALK9_GENERAL,				//# Lando walk (cin #17)
	BOTH_WALK10_GENERAL,			//# Lando walk (cin #17)
	BOTH_WALKTORUN1_GENERAL,		//# transition from walk to run
	BOTH_RUN1_GENERAL,				//# Full run
	BOTH_RUN1START_GENERAL,			//# Start into full run1
	BOTH_RUN1STOP_GENERAL,			//# Stop from full run1
	BOTH_RUN2_GENERAL,				//# Full run
	BOTH_RUNINJURED1_GENERAL,		//# Run with injured left leg
	BOTH_STRAFE_LEFT1_GENERAL,		//# Sidestep left_GENERAL, should loop
	BOTH_STRAFE_RIGHT1_GENERAL,		//# Sidestep right_GENERAL, should loop
	BOTH_RUNSTRAFE_LEFT1_GENERAL,	//# Sidestep left_GENERAL, should loop
	BOTH_RUNSTRAFE_RIGHT1_GENERAL,	//# Sidestep right_GENERAL, should loop
	BOTH_TURN_LEFT1_GENERAL,		//# Turn left_GENERAL, should loop
	BOTH_TURN_RIGHT1_GENERAL,		//# Turn right_GENERAL, should loop
	BOTH_TURNSTAND1_GENERAL,		//# Turn from STAND1 position
	BOTH_TURNSTAND2_GENERAL,		//# Turn from STAND2 position
	BOTH_TURNSTAND3_GENERAL,		//# Turn from STAND3 position
	BOTH_TURNSTAND4_GENERAL,		//# Turn from STAND4 position
	BOTH_TURNSTAND5_GENERAL,		//# Turn from STAND5 position
	BOTH_TURNCROUCH1_GENERAL,		//# Turn from CROUCH1 position
	BOTH_RUNAWAY1_GENERAL,			//# Running scared
	BOTH_SWIM1_GENERAL,				//# Swimming

	BOTH_WALKBACK1_GENERAL,			//# Walk1 backwards
	BOTH_WALKBACK2_GENERAL,			//# Walk2 backwards
	BOTH_RUNBACK1_GENERAL,			//# Run1 backwards
	BOTH_RUNBACK2_GENERAL,			//# Run1 backwards
	
	//# #sep BOTH_ JUMPING
	BOTH_JUMP1_GENERAL,				//# Jump - wind-up and leave ground
	BOTH_INAIR1_GENERAL,			//# In air loop (from jump)
	BOTH_LAND1_GENERAL,				//# Landing (from in air loop)
	BOTH_LAND2_GENERAL,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1_GENERAL,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1_GENERAL,		//# In air loop (from jump back)
	BOTH_LANDBACK1_GENERAL,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1_GENERAL,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1_GENERAL,		//# In air loop (from jump left)
	BOTH_LANDLEFT1_GENERAL,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1_GENERAL,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1_GENERAL,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1_GENERAL,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1_GENERAL,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1_GENERAL,		//# In air loop (from jump)
	BOTH_FORCELAND1_GENERAL,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1_GENERAL,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1_GENERAL,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1_GENERAL,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1_GENERAL,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1_GENERAL,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1_GENERAL,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1_GENERAL,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1_GENERAL,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1_GENERAL,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F_GENERAL,			//# Flip forward
	BOTH_FLIP_B_GENERAL,			//# Flip backwards
	BOTH_FLIP_L_GENERAL,			//# Flip left
	BOTH_FLIP_R_GENERAL,			//# Flip right

	BOTH_ROLL_F_GENERAL,			//# Roll forward
	BOTH_ROLL_B_GENERAL,			//# Roll backward
	BOTH_ROLL_L_GENERAL,			//# Roll left
	BOTH_ROLL_R_GENERAL,			//# Roll right
	BOTH_ROLL_FR_GENERAL,			//# Roll forward right
	BOTH_ROLL_FL_GENERAL,			//# Roll forward left
	BOTH_ROLL_BR_GENERAL,			//# Roll back right
	BOTH_ROLL_BL_GENERAL,			//# Roll back left

	BOTH_HOP_F_GENERAL,				//# quickstep forward
	BOTH_HOP_B_GENERAL,				//# quickstep backwards
	BOTH_HOP_L_GENERAL,				//# quickstep left
	BOTH_HOP_R_GENERAL,				//# quickstep right

	BOTH_DODGE_FL_GENERAL,			//# lean-dodge forward left
	BOTH_DODGE_FR_GENERAL,			//# lean-dodge forward right
	BOTH_DODGE_BL_GENERAL,			//# lean-dodge backwards left
	BOTH_DODGE_BR_GENERAL,			//# lean-dodge backwards right
	BOTH_DODGE_L_GENERAL,			//# lean-dodge left
	BOTH_DODGE_R_GENERAL,			//# lean-dodge right

	BOTH_DIVE1_GENERAL,				//# Dive!

	BOTH_ENGAGETAUNT_GENERAL,
	BOTH_ARIAL_LEFT_GENERAL,		//# 
	BOTH_ARIAL_RIGHT_GENERAL,		//# 
	BOTH_CARTWHEEL_LEFT_GENERAL,	//# 
	BOTH_CARTWHEEL_RIGHT_GENERAL,	//# 
	BOTH_FLIP_LEFT_GENERAL,			//# 
	BOTH_FLIP_BACK1_GENERAL,		//# 
	BOTH_FLIP_BACK2_GENERAL,		//# 
	BOTH_FLIP_BACK3_GENERAL,		//# 
	BOTH_BUTTERFLY_LEFT_GENERAL,	//# 
	BOTH_BUTTERFLY_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_RIGHT_STOP_GENERAL,//# 
	BOTH_WALL_RUN_LEFT_GENERAL,		//# 
	BOTH_WALL_RUN_LEFT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_LEFT_STOP_GENERAL,//# 
	BOTH_WALL_FLIP_RIGHT_GENERAL,	//# 
	BOTH_WALL_FLIP_LEFT_GENERAL,	//# 
	BOTH_WALL_FLIP_FWD_GENERAL,		//#
	BOTH_KNOCKDOWN1_GENERAL,		//# knocked backwards
	BOTH_KNOCKDOWN2_GENERAL,		//# knocked backwards hard
	BOTH_KNOCKDOWN3_GENERAL,		//#	knocked forwards
	BOTH_KNOCKDOWN4_GENERAL,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5_GENERAL,		//# dupe of 3 - will be removed
	BOTH_GETUP1_GENERAL,			//#
	BOTH_GETUP2_GENERAL,			//#
	BOTH_GETUP3_GENERAL,			//#
	BOTH_GETUP4_GENERAL,			//#
	BOTH_GETUP5_GENERAL,			//#
	BOTH_GETUP_CROUCH_F1_GENERAL,	//#
	BOTH_GETUP_CROUCH_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_F1_GENERAL,	//#
	BOTH_FORCE_GETUP_F2_GENERAL,	//#
	BOTH_FORCE_GETUP_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_B2_GENERAL,	//#
	BOTH_FORCE_GETUP_B3_GENERAL,	//#
	BOTH_FORCE_GETUP_B4_GENERAL,	//#
	BOTH_FORCE_GETUP_B5_GENERAL,	//#
	BOTH_FORCE_GETUP_B6_GENERAL,	//#
	BOTH_WALL_FLIP_BACK1_GENERAL,	//#
	BOTH_WALL_FLIP_BACK2_GENERAL,	//#
	BOTH_SPIN1_GENERAL,				//#
	BOTH_CEILING_CLING_GENERAL,		//# clinging to ceiling
	BOTH_CEILING_DROP_GENERAL,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL_GENERAL,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR_GENERAL,		//# jump spin slash bl to tr
	BOTH_DEATHFROMBACKSLASH_GENERAL,//#
	BOTH_RIGHTHANDCHOPPEDOFF_GENERAL,//#
	BOTH_DEFLECTSLASH__R__L_FIN_GENERAL,//#
	BOTH_BASHED1_GENERAL,//#
	BOTH_ARIAL_F1_GENERAL,//#
	BOTH_BUTTERFLY_FR1_GENERAL,//#
	BOTH_BUTTERFLY_FL1_GENERAL,//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1_GENERAL,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1_GENERAL,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1_GENERAL,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE_GENERAL,		//#	Just sitting on the ladder
	BOTH_ONLADDER_BOT1_GENERAL,		//# Getting on the ladder at the bottom
	BOTH_OFFLADDER_BOT1_GENERAL,	//# Getting off the ladder at the bottom
	BOTH_ONLADDER_TOP1_GENERAL,		//# Getting on the ladder at the top
	BOTH_OFFLADDER_TOP1_GENERAL,	//# Getting off the ladder at the top
	BOTH_LIFT1_GENERAL,				//# Lifting someone/thing over their shoulder
	BOTH_STEP1_GENERAL,				//# telsia checking out lake cinematic9.2
	BOTH_HITWALL1_GENERAL,			//# cin.18_GENERAL, Kenn hit by borg into wall 56 units away
	BOTH_AMBUSHLAND1_GENERAL,		//# landing from fall on victim
	BOTH_BIRTH1_GENERAL,			//# birth from jumping through walls

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_IDLE1_GENERAL,			//# Flying Idle 1
	BOTH_FLY_IDLE2_GENERAL,			//# Flying Idle 2
	BOTH_FLY_SHIELDED_GENERAL,		//# For sentry droid_GENERAL, shields in


	//# #sep BOTH_ FLYING MOVING
	BOTH_FLY_START1_GENERAL,		//# Start flying
	BOTH_FLY_STOP1_GENERAL,			//# Stop flying
	BOTH_FLY_LOOP1_GENERAL,			//# Normal flying_GENERAL, should loop
	BOTH_FLOAT1_GENERAL,			//# Crew floating through space 1
	BOTH_FLOAT2_GENERAL,			//# Crew floating through space 2
	BOTH_FLOATCONSOLE1_GENERAL,		//# Crew floating and working on console

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1_GENERAL,		//# Swimming Idle 1
	BOTH_SWIMFORWARD_GENERAL,		//# Swim forward loop

	//# #sep BOTH_ LYING
	BOTH_LIE_DOWN1_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face down
	BOTH_LIE_DOWN2_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face up
	BOTH_LIE_DOWN3_GENERAL,			//# reaction to local disnode being destroyed
	BOTH_PAIN2WRITHE1_GENERAL,		//# Transition from upright position to writhing on ground anim
	BOTH_PRONE2RLEG_GENERAL,		//# Lying on ground reach to grab right leg
	BOTH_PRONE2LLEG_GENERAL,		//# Lying on ground reach to grab left leg
	BOTH_WRITHING1_GENERAL,			//# Lying on ground on back writhing in pain
	BOTH_WRITHING1RLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding right leg
	BOTH_WRITHING1LLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding left leg
	BOTH_WRITHING2_GENERAL,			//# Lying on ground on front writhing in pain
	BOTH_INJURED1_GENERAL,			//# Lying down_GENERAL, against wall - can also be sleeping against wall
	BOTH_INJURED2_GENERAL,			//# Injured pose 2
	BOTH_INJURED3_GENERAL,			//# Injured pose 3
	BOTH_INJURED6_GENERAL,			//# Injured pose 6
	BOTH_INJURED6ATTACKSTART_GENERAL,	//# Start attack while in injured 6 pose 
	BOTH_INJURED6ATTACKSTOP_GENERAL,	//# End attack while in injured 6 pose
	BOTH_INJURED6COMBADGE_GENERAL,	//# Hit combadge while in injured 6 pose
	BOTH_INJURED6POINT_GENERAL,		//# Chang points to door while in injured state
	BOTH_INJUREDTOSTAND1_GENERAL,	//# Runinjured to stand1

	BOTH_PROPUP1_GENERAL,			//# Kyle getting up from having been knocked down (cin #9 end)
	BOTH_CRAWLBACK1_GENERAL,		//# Lying on back_GENERAL, crawling backwards with elbows
	BOTH_SITWALL1_GENERAL,			//# Sitting against a wall
	BOTH_SLEEP1_GENERAL,			//# laying on back-rknee up-rhand on torso
	BOTH_SLEEP2_GENERAL,			//# on floor-back against wall-arms crossed
	BOTH_SLEEP3_GENERAL,			//# Sleeping in a chair
	BOTH_SLEEP4_GENERAL,			//# Sleeping slumped over table
	BOTH_SLEEP5_GENERAL,			//# Laying on side sleeping on flat sufrace
	BOTH_SLEEP6START_GENERAL,		//# Kyle leaning back to sleep (cin 20)
	BOTH_SLEEP6STOP_GENERAL,		//# Kyle waking up and shaking his head (cin 21)
	BOTH_SLEEP1GETUP_GENERAL,		//# alarmed and getting up out of sleep1 pose to stand
	BOTH_SLEEP1GETUP2_GENERAL,		//# 
	BOTH_SLEEP2GETUP_GENERAL,		//# alarmed and getting up out of sleep2 pose to stand
	BOTH_SLEEP3GETUP_GENERAL,		//# alarmed and getting up out of sleep3 pose to stand
	BOTH_SLEEP3DEATH_GENERAL,		//# death in chair_GENERAL, from sleep3 idle
	BOTH_SLEEP3DEAD_GENERAL,		//# death in chair_GENERAL, from sleep3 idle

	BOTH_SLEEP_IDLE1_GENERAL,		//# rub face and nose while asleep from sleep pose 1
	BOTH_SLEEP_IDLE2_GENERAL,		//# shift position while asleep - stays in sleep2
	BOTH_SLEEP_IDLE3_GENERAL,		//# Idle anim from sleep pose 3
	BOTH_SLEEP_IDLE4_GENERAL,		//# Idle anim from sleep pose 4
	BOTH_SLEEP1_NOSE_GENERAL,		//# Scratch nose from SLEEP1 pose
	BOTH_SLEEP2_SHIFT_GENERAL,		//# Shift in sleep from SLEEP2 pose
	BOTH_RESTRAINED1_GENERAL,		//# Telsia tied to medical table
	BOTH_RESTRAINED1POINT_GENERAL,	//# Telsia tied to medical table pointing at Munro
	BOTH_LIFTED1_GENERAL,			//# Fits with BOTH_LIFT1_GENERAL, lifted on shoulder
	BOTH_CARRIED1_GENERAL,			//# Fits with TORSO_CARRY1_GENERAL, carried over shoulder
	BOTH_CARRIED2_GENERAL,			//# Laying over object

	BOTH_CHOKE1START_GENERAL,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD_GENERAL,	//# loop of tavion in force grip choke
	BOTH_CHOKE1_GENERAL,			//# tavion in force grip choke

	BOTH_CHOKE2_GENERAL,			//# tavion recovering from force grip choke
	BOTH_CHOKE3_GENERAL,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1_GENERAL,			//# Wakes up

	BOTH_TURNON_GENERAL,			//# Protocol Droid wakes up
	BOTH_TURNOFF_GENERAL,			//# Protocol Droid shuts off

	BOTH_BUTTON1_GENERAL,			//# Single button push with right hand
	BOTH_BUTTON2_GENERAL,			//# Single button push with left finger
	BOTH_BUTTON_HOLD_GENERAL,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE_GENERAL,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	BOTH_RESISTPUSH_GENERAL,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1_GENERAL,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2_GENERAL,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING_GENERAL,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_START_GENERAL,	//# Use off-hand to do lightning - start
	BOTH_FORCELIGHTNING_HOLD_GENERAL,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE_GENERAL,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START_GENERAL,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP_GENERAL,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK_GENERAL,		//# Healing meditation gesture
	BOTH_SABERPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1_GENERAL,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP3_GENERAL,		//# force-gripping (right hand)
	BOTH_FORCEGRIP3THROW_GENERAL,	//# throwing while force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD_GENERAL,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE_GENERAL,//# Use off-hand to do grip - release
	BOTH_TOSS1_GENERAL,				//# throwing to left after force gripping
	BOTH_TOSS2_GENERAL,				//# throwing to right after force gripping

	BOTH_COCKPIT_TALKR1START_GENERAL,		//# turn head from straight forward to looking full right
	BOTH_COCKPIT_TALKR1STARTTOMID_GENERAL,	//# from TALKR1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKR1MIDTOSTART_GENERAL,	//# from looking at hologram to TALKR1START (cin #1)
	BOTH_COCKPIT_TALKR1STOP_GENERAL,		//# return head to straight forward from BOTH_COCKPIT_TALKR1
	BOTH_COCKPIT_TALKR1STOPTOMID_GENERAL,	//# from TALKR1STOP to TALKR1MID
	BOTH_COCKPIT_TALKR1MIDTOSTOP_GENERAL,	//# from looking at hologram to TALKR1STOP (cin #1)
	BOTH_COCKPIT_TALKR1_GENERAL,			//# talk to right side

	BOTH_COCKPIT_TALKL1START_GENERAL,		//# turn head from straight forward to looking full left
	BOTH_COCKPIT_TALKL1STARTTOMID_GENERAL,	//# from TALKL1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKL1MIDTOSTART_GENERAL,	//# from looking at hologram to TALKL1START (cin #1)
	BOTH_COCKPIT_TALKL1STOP_GENERAL,		//# return head to straight forward from BOTH_COCKPIT_TALKL1
	BOTH_COCKPIT_TALKL1STOPTOMID_GENERAL,	//# from TALKL1STOP to TALKL1MID
	BOTH_COCKPIT_TALKL1MIDTOSTOP_GENERAL,	//# from looking at hologram to TALKL1STOP (cin #1)
	BOTH_COCKPIT_TALKL1_GENERAL,			//# talk to left side

	BOTH_COCKPIT_CONSOLE1_GENERAL,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_GENERAL,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_PARTIAL_GENERAL,	//# last part of console2 anim (cin #1) used by Jan	

	BOTH_COCKPIT_HEADNOD_GENERAL,			//# nod head yes while sitting
	BOTH_COCKPIT_HEADSHAKE_GENERAL,			//# shake head no while sitting

	BOTH_COCKPIT_HEADTILTLSTART_GENERAL,	//# start tilt head left while sitting
	BOTH_COCKPIT_HEADTILTLSTOP_GENERAL,		//# stop tilt head left while sitting
	BOTH_COCKPIT_HEADTILTRSTART_GENERAL,	//# start tilt head right while sitting
	BOTH_COCKPIT_HEADTILTRSTOP_GENERAL,		//# stop tilt head right while sitting

	BOTH_COCKPIT_TALKGESTURE7START_GENERAL,		//# Lando's supporting hand to Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE7STOP_GENERAL,		//# Lando's supporting hand away from Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE8START_GENERAL,		//# Hand to Lando's chin (cin #21)
	BOTH_COCKPIT_TALKGESTURE8STOP_GENERAL,		//# hand away from Lando's chin *cin #21)
	BOTH_COCKPIT_TALKGESTURE11START_GENERAL,	//# 
	BOTH_COCKPIT_TALKGESTURE11STOP_GENERAL,		//# 

	BOTH_COCKPIT_SLEEP6START_GENERAL,		//# 
	BOTH_COCKPIT_SLEEP6STOP_GENERAL,		//# 

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP2_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP3_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP4_GENERAL,		//# Put weapon away
	TORSO_RAISEWEAP1_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP2_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP3_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP4_GENERAL,		//# Draw Weapon
	TORSO_WEAPONREADY1_GENERAL,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2_GENERAL,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3_GENERAL,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4_GENERAL,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY5_GENERAL,		//# Ready to fire bowcaster
	TORSO_WEAPONREADY6_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY7_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY8_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY9_GENERAL,		//# Ready to fire rocket launcher
	TORSO_WEAPONREADY10_GENERAL,	//# Ready to fire thermal det
	TORSO_WEAPONREADY11_GENERAL,	//# Ready to fire laser trap
	TORSO_WEAPONREADY12_GENERAL,	//# Ready to fire detpack
	TORSO_WEAPONIDLE1_GENERAL,		//# Holding stun baton
	TORSO_WEAPONIDLE2_GENERAL,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3_GENERAL,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4_GENERAL,		//# Holding sniper rifle
	TORSO_WEAPONIDLE5_GENERAL,		//# Holding bowcaster
	TORSO_WEAPONIDLE6_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE7_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE8_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE9_GENERAL,		//# Holding rocket launcher
	TORSO_WEAPONIDLE10_GENERAL,		//# Holding thermal det
	TORSO_WEAPONIDLE11_GENERAL,		//# Holding laser trap
	TORSO_WEAPONIDLE12_GENERAL,		//# Holding detpack

	//# #sep TORSO_ MISC
	TORSO_HANDGESTURE1_GENERAL,		//# gestures to left one hand
	TORSO_HANDGESTURE2_GENERAL,		//# gestures to right one hand
	TORSO_HANDGESTURE3_GENERAL,		//# gestures to the left both hands
	TORSO_HANDGESTURE4_GENERAL,		//# gestures to the right both hands

	TORSO_HANDEXTEND1_GENERAL,		//# doctor reaching for hypospray in scav5
	TORSO_HANDRETRACT1_GENERAL,		//# doctor taking hypospray from player in scav5

	TORSO_DROPHELMET1_GENERAL,		//# Drop the helmet to the waist
	TORSO_RAISEHELMET1_GENERAL,		//# Bring the helmet to the head
	TORSO_REACHHELMET1_GENERAL,		//# reaching for helmet off of 60 tall cabinet
	TORSO_GRABLBACKL_GENERAL,		//# reach to lower back with left hand
	TORSO_GRABUBACKL_GENERAL,		//# reach to upper back with left hand
	TORSO_GRABLBACKR_GENERAL,		//# reach to lower back with right hand
	TORSO_GRABUBACKR_GENERAL,		//# reach to upper back with right hand

	TORSO_SURRENDER_START_GENERAL,	//# arms up
	TORSO_SURRENDER_STOP_GENERAL,	//# arms back down

	TORSO_CHOKING1_GENERAL,			//# TEMP


	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_WALKBACK1_GENERAL,			//# Walk1 backwards
	LEGS_WALKBACK2_GENERAL,			//# Walk2 backwards
	LEGS_RUNBACK1_GENERAL,			//# Run1 backwards
	LEGS_RUNBACK2_GENERAL,			//# Run2 backwards
	LEGS_TURN1_GENERAL,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2_GENERAL,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1_GENERAL,		//# Lean left
	LEGS_LEAN_RIGHT1_GENERAL,		//# Lean Right
	LEGS_KNEELDOWN1_GENERAL,		//# Get down on one knee?
	LEGS_KNEELUP1_GENERAL,			//# Get up from one knee?
	LEGS_CRLEAN_LEFT1_GENERAL,		//# Crouch Lean left
	LEGS_CRLEAN_RIGHT1_GENERAL,		//# Crouch Lean Right
	LEGS_CHOKING1_GENERAL,			//# TEMP
	LEGS_LEFTUP1_GENERAL,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2_GENERAL,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3_GENERAL,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4_GENERAL,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5_GENERAL,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1_GENERAL,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2_GENERAL,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3_GENERAL,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4_GENERAL,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5_GENERAL,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1_GENERAL,
	LEGS_S1_LUP2_GENERAL,
	LEGS_S1_LUP3_GENERAL,
	LEGS_S1_LUP4_GENERAL,
	LEGS_S1_LUP5_GENERAL,
	LEGS_S1_RUP1_GENERAL,
	LEGS_S1_RUP2_GENERAL,
	LEGS_S1_RUP3_GENERAL,
	LEGS_S1_RUP4_GENERAL,
	LEGS_S1_RUP5_GENERAL,
	LEGS_S3_LUP1_GENERAL,
	LEGS_S3_LUP2_GENERAL,
	LEGS_S3_LUP3_GENERAL,
	LEGS_S3_LUP4_GENERAL,
	LEGS_S3_LUP5_GENERAL,
	LEGS_S3_RUP1_GENERAL,
	LEGS_S3_RUP2_GENERAL,
	LEGS_S3_RUP3_GENERAL,
	LEGS_S3_RUP4_GENERAL,
	LEGS_S3_RUP5_GENERAL,
	LEGS_S4_LUP1_GENERAL,
	LEGS_S4_LUP2_GENERAL,
	LEGS_S4_LUP3_GENERAL,
	LEGS_S4_LUP4_GENERAL,
	LEGS_S4_LUP5_GENERAL,
	LEGS_S4_RUP1_GENERAL,
	LEGS_S4_RUP2_GENERAL,
	LEGS_S4_RUP3_GENERAL,
	LEGS_S4_RUP4_GENERAL,
	LEGS_S4_RUP5_GENERAL,
	LEGS_S5_LUP1_GENERAL,
	LEGS_S5_LUP2_GENERAL,
	LEGS_S5_LUP3_GENERAL,
	LEGS_S5_LUP4_GENERAL,
	LEGS_S5_LUP5_GENERAL,
	LEGS_S5_RUP1_GENERAL,
	LEGS_S5_RUP2_GENERAL,
	LEGS_S5_RUP3_GENERAL,
	LEGS_S5_RUP4_GENERAL,
	LEGS_S5_RUP5_GENERAL,
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	FACE_TALK1_GENERAL,			//# quiet
	FACE_TALK2_GENERAL,			//# semi-quiet
	FACE_TALK3_GENERAL,			//# semi-loud
	FACE_TALK4_GENERAL,			//# loud
	FACE_ALERT_GENERAL,				//# 
	FACE_SMILE_GENERAL,				//# 
	FACE_FROWN_GENERAL,				//# 
	FACE_DEAD_GENERAL,				//# 

	//# #eol
	MAX_ANIMATIONS_GENERAL,
	MAX_TOTALANIMATIONS_GENERAL
};



typedef enum //# animNumber_e
{
	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	BOTH_1CRUFTFORGIL_DM16,		//# G2 cannot have a reverse anim at beginning of file
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1_DM16,		//# First Death anim
	BOTH_DEATH2_DM16,			//# Second Death anim
	BOTH_DEATH3_DM16,			//# Third Death anim
	BOTH_DEATH4_DM16,			//# Fourth Death anim
	BOTH_DEATH5_DM16,			//# Fifth Death anim
	BOTH_DEATH6_DM16,			//# Sixth Death anim
	BOTH_DEATH7_DM16,			//# Seventh Death anim
	BOTH_DEATH8_DM16,			//# 
	BOTH_DEATH9_DM16,			//# 
	BOTH_DEATH10_DM16,			//# 
	BOTH_DEATH11_DM16,			//#
	BOTH_DEATH12_DM16,			//# 
	BOTH_DEATH13_DM16,			//# 
	BOTH_DEATH14_DM16,			//# 
	BOTH_DEATH15_DM16,			//# 
	BOTH_DEATH16_DM16,			//# 
	BOTH_DEATH17_DM16,			//# 
	BOTH_DEATH18_DM16,			//# 
	BOTH_DEATH19_DM16,			//# 
	BOTH_DEATH20_DM16,			//# 
	BOTH_DEATH21_DM16,			//# 
	BOTH_DEATH22_DM16,			//# 
	BOTH_DEATH23_DM16,			//# 
	BOTH_DEATH24_DM16,			//# 
	BOTH_DEATH25_DM16,			//# 

	BOTH_DEATHFORWARD1_DM16,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2_DM16,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3_DM16,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1_DM16,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2_DM16,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE_DM16,		//# Idle while close to death
	BOTH_LYINGDEATH1_DM16,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1_DM16,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1_DM16,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR_DM16,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND_DM16,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL_DM16,		//# Death anim from a roll
	BOTH_DEATH_FLIP_DM16,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R_DM16,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L_DM16,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180_DM16,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP_DM16,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN_DM16,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN_DM16,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP_DM16,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED_DM16,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1_DM16,				//# First Death finished pose
	BOTH_DEAD2_DM16,				//# Second Death finished pose
	BOTH_DEAD3_DM16,				//# Third Death finished pose
	BOTH_DEAD4_DM16,				//# Fourth Death finished pose
	BOTH_DEAD5_DM16,				//# Fifth Death finished pose
	BOTH_DEAD6_DM16,				//# Sixth Death finished pose
	BOTH_DEAD7_DM16,				//# Seventh Death finished pose
	BOTH_DEAD8_DM16,				//# 
	BOTH_DEAD9_DM16,				//# 
	BOTH_DEAD10_DM16,			//# 
	BOTH_DEAD11_DM16,			//#
	BOTH_DEAD12_DM16,			//# 
	BOTH_DEAD13_DM16,			//# 
	BOTH_DEAD14_DM16,			//# 
	BOTH_DEAD15_DM16,			//# 
	BOTH_DEAD16_DM16,			//# 
	BOTH_DEAD17_DM16,			//# 
	BOTH_DEAD18_DM16,			//# 
	BOTH_DEAD19_DM16,			//# 
	BOTH_DEAD20_DM16,			//# 
	BOTH_DEAD21_DM16,			//# 
	BOTH_DEAD22_DM16,			//# 
	BOTH_DEAD23_DM16,			//# 
	BOTH_DEAD24_DM16,			//# 
	BOTH_DEAD25_DM16,			//# 
	BOTH_DEADFORWARD1_DM16,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2_DM16,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_DM16,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2_DM16,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1_DM16,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_DM16,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND_DM16,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1_DM16,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2_DM16,		//# React to being shot from Second Death finished pose
	BOTH_DEADFLOP3_DM16,		//# React to being shot from Third Death finished pose
	BOTH_DEADFLOP4_DM16,		//# React to being shot from Fourth Death finished pose
	BOTH_DEADFLOP5_DM16,		//# React to being shot from Fifth Death finished pose 
	BOTH_DEADFORWARD1_FLOP_DM16,		//# React to being shot First thrown forward death finished pose
	BOTH_DEADFORWARD2_FLOP_DM16,		//# React to being shot Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_FLOP_DM16,	//# React to being shot First thrown backward death finished pose
	BOTH_DEADBACKWARD2_FLOP_DM16,	//# React to being shot Second thrown backward death finished pose
	BOTH_LYINGDEAD1_FLOP_DM16,		//# React to being shot Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_FLOP_DM16,		//# React to being shot Stumble forward death finished pose
	BOTH_FALLDEAD1_FLOP_DM16,	//# React to being shot Fall forward and splat death finished pose
	BOTH_DISMEMBER_HEAD1_DM16,	//#
	BOTH_DISMEMBER_TORSO1_DM16,	//#
	BOTH_DISMEMBER_LLEG_DM16,	//#
	BOTH_DISMEMBER_RLEG_DM16,	//#
	BOTH_DISMEMBER_RARM_DM16,	//#
	BOTH_DISMEMBER_LARM_DM16,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1_DM16,				//# First take pain anim
	BOTH_PAIN2_DM16,				//# Second take pain anim
	BOTH_PAIN3_DM16,				//# Third take pain anim
	BOTH_PAIN4_DM16,				//# Fourth take pain anim
	BOTH_PAIN5_DM16,				//# Fifth take pain anim - from behind
	BOTH_PAIN6_DM16,				//# Sixth take pain anim - from behind
	BOTH_PAIN7_DM16,				//# Seventh take pain anim - from behind
	BOTH_PAIN8_DM16,				//# Eigth take pain anim - from behind
	BOTH_PAIN9_DM16,				//# 
	BOTH_PAIN10_DM16,			//# 
	BOTH_PAIN11_DM16,			//# 
	BOTH_PAIN12_DM16,			//# 
	BOTH_PAIN13_DM16,			//# 
	BOTH_PAIN14_DM16,			//# 
	BOTH_PAIN15_DM16,			//# 
	BOTH_PAIN16_DM16,			//# 
	BOTH_PAIN17_DM16,			//# 
	BOTH_PAIN18_DM16,			//# 
	BOTH_PAIN19_DM16,			//# 
	BOTH_PAIN20_DM16,			//# GETTING SHOCKED

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1_DM16,			//# Attack with stun baton
	BOTH_ATTACK2_DM16,			//# Attack with one-handed pistol
	BOTH_ATTACK2IDLE1_DM16,		//# Idle with one-handed pistol
	BOTH_ATTACK3_DM16,			//# Attack with blaster rifle
	BOTH_ATTACK4_DM16,			//# Attack with disruptor
	BOTH_ATTACK5_DM16,			//# Attack with bow caster
	BOTH_ATTACK6_DM16,			//# Attack with ???
	BOTH_ATTACK7_DM16,			//# Attack with ???
	BOTH_ATTACK8_DM16,			//# Attack with ???
	BOTH_ATTACK9_DM16,			//# Attack with rocket launcher
	BOTH_ATTACK10_DM16,			//# Attack with thermal det
	BOTH_ATTACK11_DM16,			//# Attack with laser trap
	BOTH_ATTACK12_DM16,			//# Attack with detpack
	BOTH_MELEE1_DM16,			//# First melee attack
	BOTH_MELEE2_DM16,			//# Second melee attack
	BOTH_MELEE3_DM16,			//# Third melee attack
	BOTH_MELEE4_DM16,			//# Fourth melee attack
	BOTH_MELEE5_DM16,			//# Fifth melee attack
	BOTH_MELEE6_DM16,			//# Sixth melee attack
	BOTH_THERMAL_READY_DM16,		//# pull back with thermal
	BOTH_THERMAL_THROW_DM16,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 2
	BOTH_A1_T__B__DM16,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R_DM16,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L_DM16,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR_DM16,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL_DM16,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R_DM16,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL_DM16,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L_DM16,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL_DM16,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR_DM16,	//# Fast arc right to top right
	BOTH_T1__R_TL_DM16,	//# Fast arc right to top left
	BOTH_T1__R__L_DM16,	//# Fast weak spin right to left
	BOTH_T1__R_BL_DM16,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR_DM16,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL_DM16,	//# Fast arc top right to top left
	BOTH_T1_TR__L_DM16,	//# Fast arc top right to left
	BOTH_T1_TR_BL_DM16,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR_DM16,	//# Fast arc top to bottom right
	BOTH_T1_T___R_DM16,	//# Fast arc top to right
	BOTH_T1_T__TR_DM16,	//# Fast arc top to top right
	BOTH_T1_T__TL_DM16,	//# Fast arc top to top left
	BOTH_T1_T___L_DM16,	//# Fast arc top to left
	BOTH_T1_T__BL_DM16,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR_DM16,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL_DM16,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR_DM16,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R_DM16,	//# Fast weak spin left to right
	BOTH_T1__L_TL_DM16,	//# Fast arc left to top left
	BOTH_T1_BL_BR_DM16,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R_DM16,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR_DM16,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L_DM16,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR_DM16,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T__DM16,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR_DM16,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T__DM16,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R_DM16,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T__DM16,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R_DM16,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR_DM16,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T__DM16,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L_DM16,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR_DM16,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T__DM16,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL_DM16,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T__DM16,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL_DM16,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T__DM16,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L_DM16,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R_DM16,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL_DM16,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR_DM16,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL_DM16,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR_DM16,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1_DM16,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1_DM16,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1_DM16,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1_DM16,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1_DM16,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1_DM16,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1_DM16,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM16, played backwards)
	BOTH_B1_BR____DM16,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R____DM16,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR____DM16,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T_____DM16,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL____DM16,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L____DM16,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL____DM16,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR____DM16,	//# Deflection toward BR
	BOTH_D1__R____DM16,	//# Deflection toward R
	BOTH_D1_TR____DM16,	//# Deflection toward TR
	BOTH_D1_TL____DM16,	//# Deflection toward TL
	BOTH_D1__L____DM16,	//# Deflection toward L
	BOTH_D1_BL____DM16,	//# Deflection toward BL
	BOTH_D1_B_____DM16,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B__DM16,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R_DM16,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L_DM16,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR_DM16,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL_DM16,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R_DM16,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL_DM16,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L_DM16,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL_DM16,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR_DM16,	//# Fast arc right to top right
	BOTH_T2__R_TL_DM16,	//# Fast arc right to top left
	BOTH_T2__R__L_DM16,	//# Fast weak spin right to left
	BOTH_T2__R_BL_DM16,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR_DM16,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL_DM16,	//# Fast arc top right to top left
	BOTH_T2_TR__L_DM16,	//# Fast arc top right to left
	BOTH_T2_TR_BL_DM16,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR_DM16,	//# Fast arc top to bottom right
	BOTH_T2_T___R_DM16,	//# Fast arc top to right
	BOTH_T2_T__TR_DM16,	//# Fast arc top to top right
	BOTH_T2_T__TL_DM16,	//# Fast arc top to top left
	BOTH_T2_T___L_DM16,	//# Fast arc top to left
	BOTH_T2_T__BL_DM16,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR_DM16,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL_DM16,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR_DM16,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R_DM16,	//# Fast weak spin left to right
	BOTH_T2__L_TL_DM16,	//# Fast arc left to top left
	BOTH_T2_BL_BR_DM16,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R_DM16,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR_DM16,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L_DM16,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR_DM16,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T__DM16,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR_DM16,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T__DM16,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R_DM16,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T__DM16,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R_DM16,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR_DM16,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T__DM16,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L_DM16,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR_DM16,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T__DM16,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL_DM16,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T__DM16,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL_DM16,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T__DM16,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L_DM16,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R_DM16,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL_DM16,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR_DM16,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL_DM16,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR_DM16,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1_DM16,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1_DM16,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1_DM16,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1_DM16,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1_DM16,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1_DM16,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1_DM16,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM16, played backwards)
	BOTH_B2_BR____DM16,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R____DM16,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR____DM16,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T_____DM16,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL____DM16,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L____DM16,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL____DM16,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR____DM16,	//# Deflection toward BR
	BOTH_D2__R____DM16,	//# Deflection toward R
	BOTH_D2_TR____DM16,	//# Deflection toward TR
	BOTH_D2_TL____DM16,	//# Deflection toward TL
	BOTH_D2__L____DM16,	//# Deflection toward L
	BOTH_D2_BL____DM16,	//# Deflection toward BL
	BOTH_D2_B_____DM16,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B__DM16,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R_DM16,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L_DM16,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR_DM16,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL_DM16,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R_DM16,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL_DM16,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L_DM16,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL_DM16,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR_DM16,	//# Fast arc right to top right
	BOTH_T3__R_TL_DM16,	//# Fast arc right to top left
	BOTH_T3__R__L_DM16,	//# Fast weak spin right to left
	BOTH_T3__R_BL_DM16,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR_DM16,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL_DM16,	//# Fast arc top right to top left
	BOTH_T3_TR__L_DM16,	//# Fast arc top right to left
	BOTH_T3_TR_BL_DM16,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR_DM16,	//# Fast arc top to bottom right
	BOTH_T3_T___R_DM16,	//# Fast arc top to right
	BOTH_T3_T__TR_DM16,	//# Fast arc top to top right
	BOTH_T3_T__TL_DM16,	//# Fast arc top to top left
	BOTH_T3_T___L_DM16,	//# Fast arc top to left
	BOTH_T3_T__BL_DM16,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR_DM16,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL_DM16,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR_DM16,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R_DM16,	//# Fast weak spin left to right
	BOTH_T3__L_TL_DM16,	//# Fast arc left to top left
	BOTH_T3_BL_BR_DM16,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R_DM16,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR_DM16,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L_DM16,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR_DM16,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T__DM16,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR_DM16,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T__DM16,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R_DM16,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T__DM16,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R_DM16,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR_DM16,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T__DM16,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L_DM16,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR_DM16,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T__DM16,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL_DM16,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T__DM16,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL_DM16,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T__DM16,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L_DM16,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R_DM16,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL_DM16,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR_DM16,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL_DM16,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR_DM16,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1_DM16,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1_DM16,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1_DM16,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1_DM16,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1_DM16,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1_DM16,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1_DM16,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM16, played backwards)
	BOTH_B3_BR____DM16,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R____DM16,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR____DM16,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T_____DM16,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL____DM16,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L____DM16,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL____DM16,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR____DM16,	//# Deflection toward BR
	BOTH_D3__R____DM16,	//# Deflection toward R
	BOTH_D3_TR____DM16,	//# Deflection toward TR
	BOTH_D3_TL____DM16,	//# Deflection toward TL
	BOTH_D3__L____DM16,	//# Deflection toward L
	BOTH_D3_BL____DM16,	//# Deflection toward BL
	BOTH_D3_B_____DM16,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B__DM16,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R_DM16,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L_DM16,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR_DM16,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL_DM16,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R_DM16,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL_DM16,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L_DM16,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL_DM16,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR_DM16,	//# Fast arc right to top right
	BOTH_T4__R_TL_DM16,	//# Fast arc right to top left
	BOTH_T4__R__L_DM16,	//# Fast weak spin right to left
	BOTH_T4__R_BL_DM16,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR_DM16,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL_DM16,	//# Fast arc top right to top left
	BOTH_T4_TR__L_DM16,	//# Fast arc top right to left
	BOTH_T4_TR_BL_DM16,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR_DM16,	//# Fast arc top to bottom right
	BOTH_T4_T___R_DM16,	//# Fast arc top to right
	BOTH_T4_T__TR_DM16,	//# Fast arc top to top right
	BOTH_T4_T__TL_DM16,	//# Fast arc top to top left
	BOTH_T4_T___L_DM16,	//# Fast arc top to left
	BOTH_T4_T__BL_DM16,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR_DM16,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL_DM16,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR_DM16,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R_DM16,	//# Fast weak spin left to right
	BOTH_T4__L_TL_DM16,	//# Fast arc left to top left
	BOTH_T4_BL_BR_DM16,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R_DM16,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR_DM16,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L_DM16,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR_DM16,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T__DM16,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR_DM16,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T__DM16,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R_DM16,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T__DM16,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R_DM16,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR_DM16,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T__DM16,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L_DM16,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR_DM16,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T__DM16,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL_DM16,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T__DM16,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL_DM16,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T__DM16,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L_DM16,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R_DM16,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL_DM16,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR_DM16,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL_DM16,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR_DM16,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1_DM16,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1_DM16,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1_DM16,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1_DM16,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1_DM16,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1_DM16,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1_DM16,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM16, played backwards)
	BOTH_B4_BR____DM16,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R____DM16,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR____DM16,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T_____DM16,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL____DM16,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L____DM16,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL____DM16,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR____DM16,	//# Deflection toward BR
	BOTH_D4__R____DM16,	//# Deflection toward R
	BOTH_D4_TR____DM16,	//# Deflection toward TR
	BOTH_D4_TL____DM16,	//# Deflection toward TL
	BOTH_D4__L____DM16,	//# Deflection toward L
	BOTH_D4_BL____DM16,	//# Deflection toward BL
	BOTH_D4_B_____DM16,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B__DM16,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R_DM16,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L_DM16,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL_DM16,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR_DM16,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL_DM16,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R_DM16,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL_DM16,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L_DM16,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL_DM16,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR_DM16,	//# Fast arc right to top right
	BOTH_T5__R_TL_DM16,	//# Fast arc right to top left
	BOTH_T5__R__L_DM16,	//# Fast weak spin right to left
	BOTH_T5__R_BL_DM16,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR_DM16,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL_DM16,	//# Fast arc top right to top left
	BOTH_T5_TR__L_DM16,	//# Fast arc top right to left
	BOTH_T5_TR_BL_DM16,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR_DM16,	//# Fast arc top to bottom right
	BOTH_T5_T___R_DM16,	//# Fast arc top to right
	BOTH_T5_T__TR_DM16,	//# Fast arc top to top right
	BOTH_T5_T__TL_DM16,	//# Fast arc top to top left
	BOTH_T5_T___L_DM16,	//# Fast arc top to left
	BOTH_T5_T__BL_DM16,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR_DM16,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL_DM16,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR_DM16,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R_DM16,	//# Fast weak spin left to right
	BOTH_T5__L_TL_DM16,	//# Fast arc left to top left
	BOTH_T5_BL_BR_DM16,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R_DM16,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR_DM16,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L_DM16,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR_DM16,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T__DM16,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR_DM16,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T__DM16,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R_DM16,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T__DM16,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R_DM16,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR_DM16,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T__DM16,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L_DM16,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR_DM16,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T__DM16,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL_DM16,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T__DM16,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL_DM16,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T__DM16,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L_DM16,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R_DM16,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL_DM16,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR_DM16,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL_DM16,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR_DM16,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1_DM16,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1_DM16,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1_DM16,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1_DM16,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1_DM16,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1_DM16,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1_DM16,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM16, played backwards)
	BOTH_B5_BR____DM16,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R____DM16,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR____DM16,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T_____DM16,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL____DM16,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L____DM16,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL____DM16,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR____DM16,	//# Deflection toward BR
	BOTH_D5__R____DM16,	//# Deflection toward R
	BOTH_D5_TR____DM16,	//# Deflection toward TR
	BOTH_D5_TL____DM16,	//# Deflection toward TL
	BOTH_D5__L____DM16,	//# Deflection toward L
	BOTH_D5_BL____DM16,	//# Deflection toward BL
	BOTH_D5_B_____DM16,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T__DM16,	//# Block shot/saber top
	BOTH_P1_S1_TR_DM16,	//# Block shot/saber top right
	BOTH_P1_S1_TL_DM16,	//# Block shot/saber top left
	BOTH_P1_S1_BL_DM16,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR_DM16,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T__DM16,	//# knockaway saber top
	BOTH_K1_S1_TR_DM16,	//# knockaway saber top right
	BOTH_K1_S1_TL_DM16,	//# knockaway saber top left
	BOTH_K1_S1_BL_DM16,	//# knockaway saber bottom left
	BOTH_K1_S1_B__DM16,	//# knockaway saber bottom
	BOTH_K1_S1_BR_DM16,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1_DM16,	//# BR attack knocked away
	BOTH_V1__R_S1_DM16,	//# R attack knocked away
	BOTH_V1_TR_S1_DM16,	//# TR attack knocked away
	BOTH_V1_T__S1_DM16,	//# T attack knocked away
	BOTH_V1_TL_S1_DM16,	//# TL attack knocked away
	BOTH_V1__L_S1_DM16,	//# L attack knocked away
	BOTH_V1_BL_S1_DM16,	//# BL attack knocked away
	BOTH_V1_B__S1_DM16,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T__DM16,	//# saber knocked down from top parry
	BOTH_H1_S1_TR_DM16,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL_DM16,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL_DM16,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B__DM16,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR_DM16,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	BOTH_BF2RETURN_DM16,	//#
	BOTH_BF2BREAK_DM16,	//#
	BOTH_BF2LOCK_DM16,	//#
	BOTH_BF1RETURN_DM16,	//#
	BOTH_BF1BREAK_DM16,	//#
	BOTH_BF1LOCK_DM16,	//#
	BOTH_CWCIRCLE_R2__R_S1_DM16,	//#
	BOTH_CCWCIRCLE_R2__L_S1_DM16,	//#
	BOTH_CWCIRCLE_A2__L__R_DM16,	//#
	BOTH_CCWCIRCLE_A2__R__L_DM16,	//#
	BOTH_CWCIRCLEBREAK_DM16,	//#
	BOTH_CCWCIRCLEBREAK_DM16,	//#
	BOTH_CWCIRCLELOCK_DM16,	//#
	BOTH_CCWCIRCLELOCK_DM16,	//#

	BOTH_SABERFAST_STANCE_DM16,
	BOTH_SABERSLOW_STANCE_DM16,
	BOTH_A2_STABBACK1_DM16,		//# Stab saber backward
	BOTH_ATTACK_BACK_DM16,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1_DM16,//#
	BOTH_JUMPFLIPSTABDOWN_DM16,//#
	BOTH_FORCELEAP2_T__B__DM16,//#
	BOTH_LUNGE2_B__T__DM16,//#
	BOTH_CROUCHATTACKBACK1_DM16,//#

	//# #sep BOTH_ STANDING
	BOTH_STAND1_DM16,			//# Standing idle_DM16, no weapon_DM16, hands down
	BOTH_STAND1IDLE1_DM16,		//# Random standing idle
	BOTH_STAND2_DM16,			//# Standing idle with a saber
	BOTH_STAND2IDLE1_DM16,		//# Random standing idle
	BOTH_STAND2IDLE2_DM16,		//# Random standing idle
	BOTH_STAND3_DM16,			//# Standing idle with 2-handed weapon
	BOTH_STAND3IDLE1_DM16,		//# Random standing idle
	BOTH_STAND4_DM16,			//# hands clasp behind back
	BOTH_STAND4IDLE1_DM16,		//# Random standing idle
	BOTH_STAND5_DM16,			//# standing idle_DM16, no weapon_DM16, hand down_DM16, back straight
	BOTH_STAND5IDLE1_DM16,		//# Random standing idle
	BOTH_STAND6_DM16,			//# one handed_DM16, gun at side_DM16, relaxed stand
	BOTH_STAND7_DM16,			//# both hands on hips (female)
	BOTH_STAND8_DM16,			//# both hands on hips (male)
	BOTH_STAND1TO3_DM16,			//# Transition from stand1 to stand3
	BOTH_STAND3TO1_DM16,			//# Transition from stand3 to stand1
	BOTH_STAND1TO2_DM16,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1_DM16,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4_DM16,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2_DM16,			//# Transition from stand4 to stand2
	BOTH_STANDTOWALK1_DM16,		//# Transition from stand1 to walk1
	BOTH_STAND4TOATTACK2_DM16,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP1_DM16,			//# standing up and stumbling
	BOTH_STANDUP2_DM16,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3_DM16,		//# transition from stand 5 to sit 3
	BOTH_STAND1_REELO_DM16,		//# Reelo in his stand1 position (cin #18)
	BOTH_STAND5_REELO_DM16,		//# Reelo in his stand5 position (cin #18)
	BOTH_STAND1TOSTAND5_DM16,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1_DM16,	//# Transition from stand5 to stand1
	BOTH_STAND5TOAIM_DM16,		//# Transition of Kye aiming his gun at Desann (cin #9) 
	BOTH_STAND5STARTLEDLOOKLEFT_DM16,	//# Kyle turning to watch the bridge drop (cin #9) 
	BOTH_STARTLEDLOOKLEFTTOSTAND5_DM16,	//# Kyle returning to stand 5 from watching the bridge drop (cin #9) 
	BOTH_STAND5TOSTAND8_DM16,	//# Transition from stand5 to stand8
	BOTH_STAND7TOSTAND8_DM16,	//# Tavion putting hands on back of chair (cin #11)
	BOTH_STAND8TOSTAND5_DM16,	//# Transition from stand8 to stand5
	BOTH_STAND5SHIFTWEIGHT_DM16,	//# Weightshift from stand5 to side and back to stand5
	BOTH_STAND5SHIFTWEIGHTSTART_DM16,	//# From stand5 to side
	BOTH_STAND5SHIFTWEIGHTSTOP_DM16,		//# From side to stand5
	BOTH_STAND5TURNLEFTSTART_DM16,		//# Start turning left from stand5
	BOTH_STAND5TURNLEFTSTOP_DM16,		//# Stop turning left from stand5
	BOTH_STAND5TURNRIGHTSTART_DM16,		//# Start turning right from stand5
	BOTH_STAND5TURNRIGHTSTOP_DM16,		//# Stop turning right from stand5
	BOTH_STAND5LOOK180LEFTSTART_DM16,	//# Start looking over left shoulder (cin #17)
	BOTH_STAND5LOOK180LEFTSTOP_DM16,	//# Stop looking over left shoulder (cin #17)

	BOTH_CONSOLE1START_DM16,		//# typing at a console
	BOTH_CONSOLE1_DM16,			//# typing at a console
	BOTH_CONSOLE1STOP_DM16,		//# typing at a console
	BOTH_CONSOLE2START_DM16,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2_DM16,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP_DM16,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTART_DM16,	//# lean in to type at console while holding comm link in hand (cin #5) 
	BOTH_CONSOLE2HOLDCOMSTOP_DM16,	//# lean away after typing at console while holding comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1_DM16,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1_DM16,		//# Cradling weapon and standing
	BOTH_ALERT1_DM16,			//# Startled by something while on guard
	BOTH_GESTURE1_DM16,			//# Generic gesture_DM16, non-specific
	BOTH_GESTURE2_DM16,			//# Generic gesture_DM16, non-specific
	BOTH_GESTURE3_DM16,			//# Generic gesture_DM16, non-specific
	BOTH_WALK1TALKCOMM1_DM16,	//# Talking into coom link while walking
	BOTH_TALK1_DM16,				//# Generic talk anim
	BOTH_TALK2_DM16,				//# Generic talk anim
	BOTH_TALKCOMM1START_DM16,	//# Start talking into a comm link
	BOTH_TALKCOMM1_DM16,			//# Talking into a comm link
	BOTH_TALKCOMM1STOP_DM16,		//# Stop talking into a comm link
	BOTH_TALKGESTURE1_DM16,		//# Generic talk anim
	BOTH_TALKGESTURE2_DM16,		//# Generic talk anim
	BOTH_TALKGESTURE3_DM16,		//# Generic talk anim
	BOTH_TALKGESTURE4START_DM16,	//# Beginning talk anim 4
	BOTH_TALKGESTURE4_DM16,		//# Talk gesture 4
	BOTH_TALKGESTURE4STOP_DM16,	//# Ending talk anim 4
	BOTH_TALKGESTURE5START_DM16,	//# Start hand on chin
	BOTH_TALKGESTURE5_DM16,		//# Hand on chin
	BOTH_TALKGESTURE5STOP_DM16,	//# Stop hand on chin
	BOTH_TALKGESTURE6START_DM16,	//# Starting Motions to self
	BOTH_TALKGESTURE6_DM16,		//# Pointing at self
	BOTH_TALKGESTURE6STOP_DM16,	//# Ending Motions to self
	BOTH_TALKGESTURE7START_DM16,	//# Start touches Kyle on shoulder
	BOTH_TALKGESTURE7_DM16,		//# Hold touches Kyle on shoulder
	BOTH_TALKGESTURE7STOP_DM16,	//# Ending touches Kyle on shoulder
	BOTH_TALKGESTURE8START_DM16,	//# Lando's chin hold 
	BOTH_TALKGESTURE8_DM16,			//# Lando's chin hold 
	BOTH_TALKGESTURE8STOP_DM16,	//# Lando's chin hold 
	BOTH_TALKGESTURE9_DM16,		//# Same as gesture 2 but with the right hand
	BOTH_TALKGESTURE10_DM16,		//# Shoulder shrug
	BOTH_TALKGESTURE11START_DM16,	//# Arms folded across chest
	BOTH_TALKGESTURE11STOP_DM16,	//# Arms folded across chest
	BOTH_TALKGESTURE12_DM16,		//# Tavion taunting Kyle
	BOTH_TALKGESTURE13START_DM16,	//# Luke warning Kyle
	BOTH_TALKGESTURE13_DM16,			//# Luke warning Kyle
	BOTH_TALKGESTURE13STOP_DM16,	//# Luke warning Kyle
	BOTH_TALKGESTURE14_DM16,			//# Luke gesturing to Kyle
	BOTH_TALKGESTURE15START_DM16,	//# Desann taunting Kyle
	BOTH_TALKGESTURE15_DM16,			//# Desann taunting Kyle
	BOTH_TALKGESTURE15STOP_DM16,		//# Desann taunting Kyle
	BOTH_TALKGESTURE16_DM16,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE17_DM16,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE18_DM16,			//# Bartender gesture cin #15
	BOTH_TALKGESTURE19START_DM16,	//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE19STOP_DM16,		//# Desann lifting his arm "Join me" (cin #34)
	BOTH_TALKGESTURE20START_DM16,	//# Kyle lifting his arm "Join us" (cin #34)
	BOTH_TALKGESTURE21_DM16,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE22_DM16,			//# generic talk gesture from stand3
	BOTH_TALKGESTURE23_DM16,			//# generic talk gesture from stand3
	BOTH_PAUSE1START_DM16,			//# Luke pauses to warn Kyle (cin #24) start
	BOTH_PAUSE1STOP_DM16,			//# Luke pauses to warn Kyle (cin #24) stop

	BOTH_HEADTILTLSTART_DM16,		//# Head tilt to left
	BOTH_HEADTILTLSTOP_DM16,			//# Head tilt to left
	BOTH_HEADTILTRSTART_DM16,		//# Head tilt to right
	BOTH_HEADTILTRSTOP_DM16,			//# Head tilt to right
	BOTH_HEADNOD_DM16,				//# Head shake YES
	BOTH_HEADSHAKE_DM16,				//# Head shake NO
	BOTH_HEADSHAKE1_REELO_DM16,		//# Head shake NO for Reelo
	BOTH_SITHEADTILTLSTART_DM16,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTLSTOP_DM16,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTRSTART_DM16,		//# Head tilt to right from seated position
	BOTH_SITHEADTILTRSTOP_DM16,		//# Head tilt to right from seated position
	BOTH_SITHEADNOD_DM16,			//# Head shake YES from seated position
	BOTH_SITHEADSHAKE_DM16,			//# Head shake NO from seated position
	BOTH_SIT2HEADTILTLSTART_DM16,	//# Head tilt to left from seated position 2
	BOTH_SIT2HEADTILTLSTOP_DM16,		//# Head tilt to left from seated position 2
 
	BOTH_REACH1START_DM16,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP_DM16,		//# Monmothma reaching for crystal

	BOTH_EXAMINE1START_DM16,		//# Start Mon Mothma examining crystal 
	BOTH_EXAMINE1_DM16,			//# Mon Mothma examining crystal 
	BOTH_EXAMINE1STOP_DM16,		//# Stop Mon Mothma examining crystal 
	BOTH_EXAMINE2START_DM16,	//# Start Kyle tossing crystal
	BOTH_EXAMINE2_DM16,			//# Hold Kyle tossing crystal
	BOTH_EXAMINE2STOP_DM16,		//# End Kyle tossing crystal
	BOTH_EXAMINE3START_DM16,	//# Start Lando looking around corner
	BOTH_EXAMINE3_DM16,			//# Hold Lando looking around corner
	BOTH_EXAMINE3STOP_DM16,		//# End Lando looking around corner

	BOTH_LEANLEFT2START_DM16,	//# Start leaning left in chair
	BOTH_LEANLEFT2STOP_DM16,		//# Stop leaning left in chair
	BOTH_LEANRIGHT3START_DM16,	//# Start Lando leaning on wall
	BOTH_LEANRIGHT3_DM16,			//# Lando leaning on wall
	BOTH_LEANRIGHT3STOP_DM16,		//# Stop Lando leaning on wall

	BOTH_FORCEFOUNTAIN1_START_DM16,	//# Kyle being lifted into the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_MIDDLE_DM16,	//# Kyle changing to looping position in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_LOOP_DM16,	//# Kyle being spun in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_STOP_DM16,	//# Kyle being set down out of the Force Fountain (cin #10)
	BOTH_THUMBING1_DM16,				//# Lando gesturing with thumb over his shoulder (cin #19)
	BOTH_COME_ON1_DM16,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1_DM16,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END_DM16,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1_DM16,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1_DM16,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_PUNCHER1_DM16,				//# Jan punching Kyle in the shoulder (cin #37)
	BOTH_CONSTRAINER1HOLD_DM16,		//# Static pose of starting Tavion constraining Jan (cin #9)
	BOTH_CONSTRAINEE1HOLD_DM16,		//# Static pose of starting Jan being constrained by Tavion (cin #9)
	BOTH_CONSTRAINER1STAND_DM16,		//# Tavion constraining Jan in a stand pose (cin #9)
	BOTH_CONSTRAINEE1STAND_DM16,		//# Jan being constrained in a stand pose (cin #9)
	BOTH_CONSTRAINER1WALK_DM16,		//# Tavion shoving jan forward (cin #9)
	BOTH_CONSTRAINEE1WALK_DM16,		//# Jan being shoved forward by Tavion (cin #9)
	BOTH_CONSTRAINER1LOOP_DM16,		//# Tavion walking with Jan in a loop (cin #9)
	BOTH_CONSTRAINEE1LOOP_DM16,		//# Jan walking with Tavion in a loop (cin #9)
	BOTH_SABERKILLER1_DM16,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1_DM16,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HANDSHAKER1START_DM16,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKER1LOOP_DM16,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKEE1START_DM16,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_HANDSHAKEE1LOOP_DM16,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_LAUGH1START_DM16,			//# Reelo leaning forward before laughing (cin #18)
	BOTH_LAUGH1STOP_DM16,			//# Reelo laughing (cin #18)
	BOTH_ESCAPEPOD_LEAVE1_DM16,	//# Kyle leaving escape pod (cin #33)
	BOTH_ESCAPEPOD_LEAVE2_DM16,	//# Jan leaving escape pod (cin #33)
	BOTH_HUGGER1_DM16,			//# Kyle hugging Jan (cin #29)
	BOTH_HUGGERSTOP1_DM16,		//# Kyle stop hugging Jan but don't let her go (cin #29)
	BOTH_HUGGERSTOP2_DM16,		//# Kyle let go of Jan and step back (cin #29)
	BOTH_HUGGEE1_DM16,			//# Jan being hugged (cin #29)
	BOTH_HUGGEESTOP1_DM16,		//# Jan stop being hugged but don't let go (cin #29)
	BOTH_HUGGEESTOP2_DM16,		//# Jan released from hug (cin #29)
	BOTH_KISSER1_DM16,			//# Temp until the Kiss anim gets split up
	BOTH_KISSER1START1_DM16,		//# Kyle start kissing Jan
	BOTH_KISSER1START2_DM16,		//# Kyle start kissing Jan
	BOTH_KISSER1LOOP_DM16,		//# Kyle loop kissing Jan
	BOTH_KISSER1STOP_DM16,		//# Temp until the Kiss anim gets split up
	BOTH_KISSER1STOP1_DM16,		//# Kyle stop kissing but don't let go
	BOTH_KISSER1STOP2_DM16,		//# Kyle step back from Jan
	BOTH_KISSEE1_DM16,			//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1START1_DM16,		//# Jan start being kissed
	BOTH_KISSEE1START2_DM16,		//# Jan start being kissed
	BOTH_KISSEE1LOOP_DM16,		//# Jan loop being kissed
	BOTH_KISSEE1STOP_DM16,		//# Temp until the Kiss anim gets split up
	BOTH_KISSEE1STOP1_DM16,		//# Jan stop being kissed but don't let go
	BOTH_KISSEE1STOP2_DM16,		//# Jan wait for Kyle to step back
	BOTH_BARTENDER_IDLE1_DM16,	//# Bartender idle in cin #15
	BOTH_BARTENDER_THROW1_DM16,	//# Bartender throws glass in cin #15
	BOTH_BARTENDER_COWERSTART_DM16,	//# Start of Bartender raising both hands up in surrender (cin #16)
	BOTH_BARTENDER_COWERLOOP_DM16,	//# Loop of Bartender waving both hands in surrender (cin #16)
	BOTH_BARTENDER_COWER_DM16,		//# Single frame of Bartender waving both hands in surrender (cin #16)
	BOTH_THREATEN1_START_DM16,	//# First frame of Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_THREATEN1_DM16,			//# Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_RADIO_ONOFF_DM16,		//# Mech Galak turning on his suit radio (cin #32)
	BOTH_TRIUMPHANT1START_DM16,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STARTGESTURE_DM16,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STOP_DM16,	//# Mech Galak lowering his arms in victory (cin #32)

	BOTH_SABERTHROW1START_DM16,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP_DM16,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START_DM16,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP_DM16,		//# Kyle throwing his light saber (cin #32)

	BOTH_COVERUP1_LOOP_DM16,		//# animation of getting in line of friendly fire
	BOTH_COVERUP1_START_DM16,	//# transitions from stand to coverup1_loop
	BOTH_COVERUP1_END_DM16,		//# transitions from coverup1_loop to stand

	BOTH_INJURED4_DM16,			//# Injured pose 4
	BOTH_INJURED4TO5_DM16,		//# Transition from INJURED4 to INJURED5
	BOTH_INJURED5_DM16,			//# Injured pose 5

	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1STAND_DM16,			//# Stand up from First sitting anim
	BOTH_SIT1_DM16,				//# Normal chair sit.
	BOTH_SIT2_DM16,				//# Lotus position.
	BOTH_SIT3_DM16,				//# Sitting in tired position_DM16, elbows on knees

	BOTH_SIT2TO3_DM16,			//# Trans from sit2 to sit3?
	BOTH_SIT2TOSTAND5_DM16,		//# Transition from sit 2 to stand 5
	BOTH_STAND5TOSIT2_DM16,		//# Transition from stand 5 to sit 2
	BOTH_SIT2TOSIT4_DM16,		//# Trans from sit2 to sit4 (cin #12) Luke leaning back from lotus position.
	BOTH_SIT3TO1_DM16,			//# Trans from sit3 to sit1?
	BOTH_SIT3TO2_DM16,			//# Trans from sit3 to sit2?
	BOTH_SIT3TOSTAND5_DM16,		//# transition from sit 3 to stand 5

	BOTH_SIT4TO5_DM16,			//# Trans from sit4 to sit5
	BOTH_SIT4TO6_DM16,			//# Trans from sit4 to sit6
	BOTH_SIT5TO4_DM16,			//# Trans from sit5 to sit4
	BOTH_SIT5TO6_DM16,			//# Trans from sit5 to sit6
	BOTH_SIT6TO4_DM16,			//# Trans from sit6 to sit4
	BOTH_SIT6TO5_DM16,			//# Trans from sit6 to sit5
	BOTH_SIT7_DM16,				//# sitting with arms over knees_DM16, no weapon
	BOTH_SIT7TOSTAND1_DM16,		//# getting up from sit7 into stand1

	BOTH_CROUCH1_DM16,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE_DM16,		//# Crouching idle
	BOTH_CROUCH1WALK_DM16,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK_DM16,	//# Walking while crouched
	BOTH_UNCROUCH1_DM16,			//# Transition from crouch to standing
	BOTH_CROUCH2IDLE_DM16,		//# crouch and resting on back righ heel_DM16, no weapon
	BOTH_CROUCH2TOSTAND1_DM16,	//# going from crouch2 to stand1
	BOTH_CROUCH3_DM16,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3_DM16,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_CROUCH4_DM16,			//# Slower version of crouch1 for cinematics
	BOTH_UNCROUCH4_DM16,			//# Slower version of uncrouch1 for cinematics
	BOTH_GET_UP1_DM16,			//# Get up from the ground_DM16, face down
	BOTH_GET_UP2_DM16,			//# Get up from the ground_DM16, face up

	BOTH_COCKPIT_SIT_DM16,			//# sit in a cockpit.

	BOTH_GUNSIT1_DM16,			//# sitting on an emplaced gun.

	BOTH_DEATH14_UNGRIP_DM16,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP_DM16,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_KNEES1_DM16,			//# Tavion on her knees
	BOTH_KNEES2_DM16,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1_DM16,			//# Transition of KNEES2 to KNEES1
	BOTH_RUMMAGE1START_DM16,	//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1_DM16,			//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1STOP_DM16,		//# Kyle rummaging for crystal (cin 2)
	BOTH_HOLDGLASS1_DM16,		//# Bartender holds glass (cin# 14)
	BOTH_SLIDEGLASS1_DM16,		//# Bartender slides glass (cin# 14)
	BOTH_SLAMSABERDOWN_DM16,		//# Kyle slamming his saber on the bar top (cin# 14)

	//# #sep BOTH_ MOVING
	BOTH_WALK1_DM16,				//# Normal walk
	BOTH_WALK2_DM16,				//# Normal walk
	BOTH_WALK3_DM16,				//# Goes with stand3
	BOTH_WALK4_DM16,				//# Walk cycle goes to a stand4
	BOTH_WALK5_DM16,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6_DM16,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7_DM16,				//# Fast walk
	BOTH_WALK8_DM16,				//# Normal walk with hands behind back (Luke in cin#12)
	BOTH_WALK9_DM16,				//# Lando walk (cin #17)
	BOTH_WALK10_DM16,			//# Lando walk (cin #17)
	BOTH_WALKTORUN1_DM16,		//# transition from walk to run
	BOTH_RUN1_DM16,				//# Full run
	BOTH_RUN1START_DM16,			//# Start into full run1
	BOTH_RUN1STOP_DM16,			//# Stop from full run1
	BOTH_RUN2_DM16,				//# Full run
	BOTH_RUNINJURED1_DM16,		//# Run with injured left leg
	BOTH_STRAFE_LEFT1_DM16,		//# Sidestep left_DM16, should loop
	BOTH_STRAFE_RIGHT1_DM16,		//# Sidestep right_DM16, should loop
	BOTH_RUNSTRAFE_LEFT1_DM16,	//# Sidestep left_DM16, should loop
	BOTH_RUNSTRAFE_RIGHT1_DM16,	//# Sidestep right_DM16, should loop
	BOTH_TURN_LEFT1_DM16,		//# Turn left_DM16, should loop
	BOTH_TURN_RIGHT1_DM16,		//# Turn right_DM16, should loop
	BOTH_TURNSTAND1_DM16,		//# Turn from STAND1 position
	BOTH_TURNSTAND2_DM16,		//# Turn from STAND2 position
	BOTH_TURNSTAND3_DM16,		//# Turn from STAND3 position
	BOTH_TURNSTAND4_DM16,		//# Turn from STAND4 position
	BOTH_TURNSTAND5_DM16,		//# Turn from STAND5 position
	BOTH_TURNCROUCH1_DM16,		//# Turn from CROUCH1 position
	BOTH_RUNAWAY1_DM16,			//# Running scared
	BOTH_SWIM1_DM16,				//# Swimming

	BOTH_WALKBACK1_DM16,			//# Walk1 backwards
	BOTH_WALKBACK2_DM16,			//# Walk2 backwards
	BOTH_RUNBACK1_DM16,			//# Run1 backwards
	BOTH_RUNBACK2_DM16,			//# Run1 backwards
	
	//# #sep BOTH_ JUMPING
	BOTH_JUMP1_DM16,				//# Jump - wind-up and leave ground
	BOTH_INAIR1_DM16,			//# In air loop (from jump)
	BOTH_LAND1_DM16,				//# Landing (from in air loop)
	BOTH_LAND2_DM16,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1_DM16,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1_DM16,		//# In air loop (from jump back)
	BOTH_LANDBACK1_DM16,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1_DM16,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1_DM16,		//# In air loop (from jump left)
	BOTH_LANDLEFT1_DM16,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1_DM16,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1_DM16,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1_DM16,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1_DM16,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1_DM16,		//# In air loop (from jump)
	BOTH_FORCELAND1_DM16,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1_DM16,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1_DM16,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1_DM16,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1_DM16,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1_DM16,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1_DM16,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1_DM16,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1_DM16,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1_DM16,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F_DM16,			//# Flip forward
	BOTH_FLIP_B_DM16,			//# Flip backwards
	BOTH_FLIP_L_DM16,			//# Flip left
	BOTH_FLIP_R_DM16,			//# Flip right

	BOTH_ROLL_F_DM16,			//# Roll forward
	BOTH_ROLL_B_DM16,			//# Roll backward
	BOTH_ROLL_L_DM16,			//# Roll left
	BOTH_ROLL_R_DM16,			//# Roll right
	BOTH_ROLL_FR_DM16,			//# Roll forward right
	BOTH_ROLL_FL_DM16,			//# Roll forward left
	BOTH_ROLL_BR_DM16,			//# Roll back right
	BOTH_ROLL_BL_DM16,			//# Roll back left

	BOTH_HOP_F_DM16,				//# quickstep forward
	BOTH_HOP_B_DM16,				//# quickstep backwards
	BOTH_HOP_L_DM16,				//# quickstep left
	BOTH_HOP_R_DM16,				//# quickstep right

	BOTH_DODGE_FL_DM16,			//# lean-dodge forward left
	BOTH_DODGE_FR_DM16,			//# lean-dodge forward right
	BOTH_DODGE_BL_DM16,			//# lean-dodge backwards left
	BOTH_DODGE_BR_DM16,			//# lean-dodge backwards right
	BOTH_DODGE_L_DM16,			//# lean-dodge left
	BOTH_DODGE_R_DM16,			//# lean-dodge right

	BOTH_DIVE1_DM16,				//# Dive!

	BOTH_ENGAGETAUNT_DM16,
	BOTH_ARIAL_LEFT_DM16,		//# 
	BOTH_ARIAL_RIGHT_DM16,		//# 
	BOTH_CARTWHEEL_LEFT_DM16,	//# 
	BOTH_CARTWHEEL_RIGHT_DM16,	//# 
	BOTH_FLIP_LEFT_DM16,			//# 
	BOTH_FLIP_BACK1_DM16,		//# 
	BOTH_FLIP_BACK2_DM16,		//# 
	BOTH_FLIP_BACK3_DM16,		//# 
	BOTH_BUTTERFLY_LEFT_DM16,	//# 
	BOTH_BUTTERFLY_RIGHT_DM16,	//# 
	BOTH_WALL_RUN_RIGHT_DM16,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP_DM16,//#
	BOTH_WALL_RUN_RIGHT_STOP_DM16,//# 
	BOTH_WALL_RUN_LEFT_DM16,		//# 
	BOTH_WALL_RUN_LEFT_FLIP_DM16,//#
	BOTH_WALL_RUN_LEFT_STOP_DM16,//# 
	BOTH_WALL_FLIP_RIGHT_DM16,	//# 
	BOTH_WALL_FLIP_LEFT_DM16,	//# 
	BOTH_WALL_FLIP_FWD_DM16,		//#
	BOTH_KNOCKDOWN1_DM16,		//# knocked backwards
	BOTH_KNOCKDOWN2_DM16,		//# knocked backwards hard
	BOTH_KNOCKDOWN3_DM16,		//#	knocked forwards
	BOTH_KNOCKDOWN4_DM16,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5_DM16,		//# dupe of 3 - will be removed
	BOTH_GETUP1_DM16,			//#
	BOTH_GETUP2_DM16,			//#
	BOTH_GETUP3_DM16,			//#
	BOTH_GETUP4_DM16,			//#
	BOTH_GETUP5_DM16,			//#
	BOTH_GETUP_CROUCH_F1_DM16,	//#
	BOTH_GETUP_CROUCH_B1_DM16,	//#
	BOTH_FORCE_GETUP_F1_DM16,	//#
	BOTH_FORCE_GETUP_F2_DM16,	//#
	BOTH_FORCE_GETUP_B1_DM16,	//#
	BOTH_FORCE_GETUP_B2_DM16,	//#
	BOTH_FORCE_GETUP_B3_DM16,	//#
	BOTH_FORCE_GETUP_B4_DM16,	//#
	BOTH_FORCE_GETUP_B5_DM16,	//#
	BOTH_FORCE_GETUP_B6_DM16,	//#
	BOTH_WALL_FLIP_BACK1_DM16,	//#
	BOTH_WALL_FLIP_BACK2_DM16,	//#
	BOTH_SPIN1_DM16,				//#
	BOTH_CEILING_CLING_DM16,		//# clinging to ceiling
	BOTH_CEILING_DROP_DM16,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL_DM16,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR_DM16,		//# jump spin slash bl to tr
	BOTH_DEATHFROMBACKSLASH_DM16,//#
	BOTH_RIGHTHANDCHOPPEDOFF_DM16,//#
	BOTH_DEFLECTSLASH__R__L_FIN_DM16,//#
	BOTH_BASHED1_DM16,//#
	BOTH_ARIAL_F1_DM16,//#
	BOTH_BUTTERFLY_FR1_DM16,//#
	BOTH_BUTTERFLY_FL1_DM16,//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1_DM16,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1_DM16,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1_DM16,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE_DM16,		//#	Just sitting on the ladder
	BOTH_ONLADDER_BOT1_DM16,		//# Getting on the ladder at the bottom
	BOTH_OFFLADDER_BOT1_DM16,	//# Getting off the ladder at the bottom
	BOTH_ONLADDER_TOP1_DM16,		//# Getting on the ladder at the top
	BOTH_OFFLADDER_TOP1_DM16,	//# Getting off the ladder at the top
	BOTH_LIFT1_DM16,				//# Lifting someone/thing over their shoulder
	BOTH_STEP1_DM16,				//# telsia checking out lake cinematic9.2
	BOTH_HITWALL1_DM16,			//# cin.18_DM16, Kenn hit by borg into wall 56 units away
	BOTH_AMBUSHLAND1_DM16,		//# landing from fall on victim
	BOTH_BIRTH1_DM16,			//# birth from jumping through walls

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_IDLE1_DM16,			//# Flying Idle 1
	BOTH_FLY_IDLE2_DM16,			//# Flying Idle 2
	BOTH_FLY_SHIELDED_DM16,		//# For sentry droid_DM16, shields in


	//# #sep BOTH_ FLYING MOVING
	BOTH_FLY_START1_DM16,		//# Start flying
	BOTH_FLY_STOP1_DM16,			//# Stop flying
	BOTH_FLY_LOOP1_DM16,			//# Normal flying_DM16, should loop
	BOTH_FLOAT1_DM16,			//# Crew floating through space 1
	BOTH_FLOAT2_DM16,			//# Crew floating through space 2
	BOTH_FLOATCONSOLE1_DM16,		//# Crew floating and working on console

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1_DM16,		//# Swimming Idle 1
	BOTH_SWIMFORWARD_DM16,		//# Swim forward loop

	//# #sep BOTH_ LYING
	BOTH_LIE_DOWN1_DM16,			//# From a stand position_DM16, get down on ground_DM16, face down
	BOTH_LIE_DOWN2_DM16,			//# From a stand position_DM16, get down on ground_DM16, face up
	BOTH_LIE_DOWN3_DM16,			//# reaction to local disnode being destroyed
	BOTH_PAIN2WRITHE1_DM16,		//# Transition from upright position to writhing on ground anim
	BOTH_PRONE2RLEG_DM16,		//# Lying on ground reach to grab right leg
	BOTH_PRONE2LLEG_DM16,		//# Lying on ground reach to grab left leg
	BOTH_WRITHING1_DM16,			//# Lying on ground on back writhing in pain
	BOTH_WRITHING1RLEG_DM16,		//# Lying on ground writhing in pain_DM16, holding right leg
	BOTH_WRITHING1LLEG_DM16,		//# Lying on ground writhing in pain_DM16, holding left leg
	BOTH_WRITHING2_DM16,			//# Lying on ground on front writhing in pain
	BOTH_INJURED1_DM16,			//# Lying down_DM16, against wall - can also be sleeping against wall
	BOTH_INJURED2_DM16,			//# Injured pose 2
	BOTH_INJURED3_DM16,			//# Injured pose 3
	BOTH_INJURED6_DM16,			//# Injured pose 6
	BOTH_INJURED6ATTACKSTART_DM16,	//# Start attack while in injured 6 pose 
	BOTH_INJURED6ATTACKSTOP_DM16,	//# End attack while in injured 6 pose
	BOTH_INJURED6COMBADGE_DM16,	//# Hit combadge while in injured 6 pose
	BOTH_INJURED6POINT_DM16,		//# Chang points to door while in injured state
	BOTH_INJUREDTOSTAND1_DM16,	//# Runinjured to stand1

	BOTH_PROPUP1_DM16,			//# Kyle getting up from having been knocked down (cin #9 end)
	BOTH_CRAWLBACK1_DM16,		//# Lying on back_DM16, crawling backwards with elbows
	BOTH_SITWALL1_DM16,			//# Sitting against a wall
	BOTH_SLEEP1_DM16,			//# laying on back-rknee up-rhand on torso
	BOTH_SLEEP2_DM16,			//# on floor-back against wall-arms crossed
	BOTH_SLEEP3_DM16,			//# Sleeping in a chair
	BOTH_SLEEP4_DM16,			//# Sleeping slumped over table
	BOTH_SLEEP5_DM16,			//# Laying on side sleeping on flat sufrace
	BOTH_SLEEP6START_DM16,		//# Kyle leaning back to sleep (cin 20)
	BOTH_SLEEP6STOP_DM16,		//# Kyle waking up and shaking his head (cin 21)
	BOTH_SLEEP1GETUP_DM16,		//# alarmed and getting up out of sleep1 pose to stand
	BOTH_SLEEP1GETUP2_DM16,		//# 
	BOTH_SLEEP2GETUP_DM16,		//# alarmed and getting up out of sleep2 pose to stand
	BOTH_SLEEP3GETUP_DM16,		//# alarmed and getting up out of sleep3 pose to stand
	BOTH_SLEEP3DEATH_DM16,		//# death in chair_DM16, from sleep3 idle
	BOTH_SLEEP3DEAD_DM16,		//# death in chair_DM16, from sleep3 idle

	BOTH_SLEEP_IDLE1_DM16,		//# rub face and nose while asleep from sleep pose 1
	BOTH_SLEEP_IDLE2_DM16,		//# shift position while asleep - stays in sleep2
	BOTH_SLEEP_IDLE3_DM16,		//# Idle anim from sleep pose 3
	BOTH_SLEEP_IDLE4_DM16,		//# Idle anim from sleep pose 4
	BOTH_SLEEP1_NOSE_DM16,		//# Scratch nose from SLEEP1 pose
	BOTH_SLEEP2_SHIFT_DM16,		//# Shift in sleep from SLEEP2 pose
	BOTH_RESTRAINED1_DM16,		//# Telsia tied to medical table
	BOTH_RESTRAINED1POINT_DM16,	//# Telsia tied to medical table pointing at Munro
	BOTH_LIFTED1_DM16,			//# Fits with BOTH_LIFT1_DM16, lifted on shoulder
	BOTH_CARRIED1_DM16,			//# Fits with TORSO_CARRY1_DM16, carried over shoulder
	BOTH_CARRIED2_DM16,			//# Laying over object

	BOTH_CHOKE1START_DM16,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD_DM16,	//# loop of tavion in force grip choke
	BOTH_CHOKE1_DM16,			//# tavion in force grip choke

	BOTH_CHOKE2_DM16,			//# tavion recovering from force grip choke
	BOTH_CHOKE3_DM16,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1_DM16,			//# Wakes up

	BOTH_TURNON_DM16,			//# Protocol Droid wakes up
	BOTH_TURNOFF_DM16,			//# Protocol Droid shuts off

	BOTH_BUTTON1_DM16,			//# Single button push with right hand
	BOTH_BUTTON2_DM16,			//# Single button push with left finger
	BOTH_BUTTON_HOLD_DM16,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE_DM16,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	BOTH_RESISTPUSH_DM16,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH_DM16,			//# Use off-hand to do force power.
	BOTH_FORCEPULL_DM16,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1_DM16,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2_DM16,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING_DM16,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_START_DM16,	//# Use off-hand to do lightning - start
	BOTH_FORCELIGHTNING_HOLD_DM16,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE_DM16,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START_DM16,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP_DM16,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK_DM16,		//# Healing meditation gesture
	BOTH_SABERPULL_DM16,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1_DM16,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP3_DM16,		//# force-gripping (right hand)
	BOTH_FORCEGRIP3THROW_DM16,	//# throwing while force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD_DM16,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE_DM16,//# Use off-hand to do grip - release
	BOTH_TOSS1_DM16,				//# throwing to left after force gripping
	BOTH_TOSS2_DM16,				//# throwing to right after force gripping

	BOTH_COCKPIT_TALKR1START_DM16,		//# turn head from straight forward to looking full right
	BOTH_COCKPIT_TALKR1STARTTOMID_DM16,	//# from TALKR1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKR1MIDTOSTART_DM16,	//# from looking at hologram to TALKR1START (cin #1)
	BOTH_COCKPIT_TALKR1STOP_DM16,		//# return head to straight forward from BOTH_COCKPIT_TALKR1
	BOTH_COCKPIT_TALKR1STOPTOMID_DM16,	//# from TALKR1STOP to TALKR1MID
	BOTH_COCKPIT_TALKR1MIDTOSTOP_DM16,	//# from looking at hologram to TALKR1STOP (cin #1)
	BOTH_COCKPIT_TALKR1_DM16,			//# talk to right side

	BOTH_COCKPIT_TALKL1START_DM16,		//# turn head from straight forward to looking full left
	BOTH_COCKPIT_TALKL1STARTTOMID_DM16,	//# from TALKL1START to looking at hologram (cin #1)
	BOTH_COCKPIT_TALKL1MIDTOSTART_DM16,	//# from looking at hologram to TALKL1START (cin #1)
	BOTH_COCKPIT_TALKL1STOP_DM16,		//# return head to straight forward from BOTH_COCKPIT_TALKL1
	BOTH_COCKPIT_TALKL1STOPTOMID_DM16,	//# from TALKL1STOP to TALKL1MID
	BOTH_COCKPIT_TALKL1MIDTOSTOP_DM16,	//# from looking at hologram to TALKL1STOP (cin #1)
	BOTH_COCKPIT_TALKL1_DM16,			//# talk to left side

	BOTH_COCKPIT_CONSOLE1_DM16,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_DM16,			//# type at controls
	BOTH_COCKPIT_CONSOLE2_PARTIAL_DM16,	//# last part of console2 anim (cin #1) used by Jan	

	BOTH_COCKPIT_HEADNOD_DM16,			//# nod head yes while sitting
	BOTH_COCKPIT_HEADSHAKE_DM16,			//# shake head no while sitting

	BOTH_COCKPIT_HEADTILTLSTART_DM16,	//# start tilt head left while sitting
	BOTH_COCKPIT_HEADTILTLSTOP_DM16,		//# stop tilt head left while sitting
	BOTH_COCKPIT_HEADTILTRSTART_DM16,	//# start tilt head right while sitting
	BOTH_COCKPIT_HEADTILTRSTOP_DM16,		//# stop tilt head right while sitting

	BOTH_COCKPIT_TALKGESTURE7START_DM16,		//# Lando's supporting hand to Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE7STOP_DM16,		//# Lando's supporting hand away from Kyle (cin #21)
	BOTH_COCKPIT_TALKGESTURE8START_DM16,		//# Hand to Lando's chin (cin #21)
	BOTH_COCKPIT_TALKGESTURE8STOP_DM16,		//# hand away from Lando's chin *cin #21)
	BOTH_COCKPIT_TALKGESTURE11START_DM16,	//# 
	BOTH_COCKPIT_TALKGESTURE11STOP_DM16,		//# 

	BOTH_COCKPIT_SLEEP6START_DM16,		//# 
	BOTH_COCKPIT_SLEEP6STOP_DM16,		//# 

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1_DM16,		//# Put weapon away
	TORSO_DROPWEAP2_DM16,		//# Put weapon away
	TORSO_DROPWEAP3_DM16,		//# Put weapon away
	TORSO_DROPWEAP4_DM16,		//# Put weapon away
	TORSO_RAISEWEAP1_DM16,		//# Draw Weapon
	TORSO_RAISEWEAP2_DM16,		//# Draw Weapon
	TORSO_RAISEWEAP3_DM16,		//# Draw Weapon
	TORSO_RAISEWEAP4_DM16,		//# Draw Weapon
	TORSO_WEAPONREADY1_DM16,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2_DM16,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3_DM16,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4_DM16,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY5_DM16,		//# Ready to fire bowcaster
	TORSO_WEAPONREADY6_DM16,		//# Ready to fire ???
	TORSO_WEAPONREADY7_DM16,		//# Ready to fire ???
	TORSO_WEAPONREADY8_DM16,		//# Ready to fire ???
	TORSO_WEAPONREADY9_DM16,		//# Ready to fire rocket launcher
	TORSO_WEAPONREADY10_DM16,	//# Ready to fire thermal det
	TORSO_WEAPONREADY11_DM16,	//# Ready to fire laser trap
	TORSO_WEAPONREADY12_DM16,	//# Ready to fire detpack
	TORSO_WEAPONIDLE1_DM16,		//# Holding stun baton
	TORSO_WEAPONIDLE2_DM16,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3_DM16,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4_DM16,		//# Holding sniper rifle
	TORSO_WEAPONIDLE5_DM16,		//# Holding bowcaster
	TORSO_WEAPONIDLE6_DM16,		//# Holding ???
	TORSO_WEAPONIDLE7_DM16,		//# Holding ???
	TORSO_WEAPONIDLE8_DM16,		//# Holding ???
	TORSO_WEAPONIDLE9_DM16,		//# Holding rocket launcher
	TORSO_WEAPONIDLE10_DM16,		//# Holding thermal det
	TORSO_WEAPONIDLE11_DM16,		//# Holding laser trap
	TORSO_WEAPONIDLE12_DM16,		//# Holding detpack

	//# #sep TORSO_ MISC
	TORSO_HANDGESTURE1_DM16,		//# gestures to left one hand
	TORSO_HANDGESTURE2_DM16,		//# gestures to right one hand
	TORSO_HANDGESTURE3_DM16,		//# gestures to the left both hands
	TORSO_HANDGESTURE4_DM16,		//# gestures to the right both hands

	TORSO_HANDEXTEND1_DM16,		//# doctor reaching for hypospray in scav5
	TORSO_HANDRETRACT1_DM16,		//# doctor taking hypospray from player in scav5

	TORSO_DROPHELMET1_DM16,		//# Drop the helmet to the waist
	TORSO_RAISEHELMET1_DM16,		//# Bring the helmet to the head
	TORSO_REACHHELMET1_DM16,		//# reaching for helmet off of 60 tall cabinet
	TORSO_GRABLBACKL_DM16,		//# reach to lower back with left hand
	TORSO_GRABUBACKL_DM16,		//# reach to upper back with left hand
	TORSO_GRABLBACKR_DM16,		//# reach to lower back with right hand
	TORSO_GRABUBACKR_DM16,		//# reach to upper back with right hand

	TORSO_SURRENDER_START_DM16,	//# arms up
	TORSO_SURRENDER_STOP_DM16,	//# arms back down

	TORSO_CHOKING1_DM16,			//# TEMP


	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_WALKBACK1_DM16,			//# Walk1 backwards
	LEGS_WALKBACK2_DM16,			//# Walk2 backwards
	LEGS_RUNBACK1_DM16,			//# Run1 backwards
	LEGS_RUNBACK2_DM16,			//# Run2 backwards
	LEGS_TURN1_DM16,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2_DM16,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1_DM16,		//# Lean left
	LEGS_LEAN_RIGHT1_DM16,		//# Lean Right
	LEGS_KNEELDOWN1_DM16,		//# Get down on one knee?
	LEGS_KNEELUP1_DM16,			//# Get up from one knee?
	LEGS_CRLEAN_LEFT1_DM16,		//# Crouch Lean left
	LEGS_CRLEAN_RIGHT1_DM16,		//# Crouch Lean Right
	LEGS_CHOKING1_DM16,			//# TEMP
	LEGS_LEFTUP1_DM16,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2_DM16,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3_DM16,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4_DM16,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5_DM16,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1_DM16,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2_DM16,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3_DM16,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4_DM16,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5_DM16,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1_DM16,
	LEGS_S1_LUP2_DM16,
	LEGS_S1_LUP3_DM16,
	LEGS_S1_LUP4_DM16,
	LEGS_S1_LUP5_DM16,
	LEGS_S1_RUP1_DM16,
	LEGS_S1_RUP2_DM16,
	LEGS_S1_RUP3_DM16,
	LEGS_S1_RUP4_DM16,
	LEGS_S1_RUP5_DM16,
	LEGS_S3_LUP1_DM16,
	LEGS_S3_LUP2_DM16,
	LEGS_S3_LUP3_DM16,
	LEGS_S3_LUP4_DM16,
	LEGS_S3_LUP5_DM16,
	LEGS_S3_RUP1_DM16,
	LEGS_S3_RUP2_DM16,
	LEGS_S3_RUP3_DM16,
	LEGS_S3_RUP4_DM16,
	LEGS_S3_RUP5_DM16,
	LEGS_S4_LUP1_DM16,
	LEGS_S4_LUP2_DM16,
	LEGS_S4_LUP3_DM16,
	LEGS_S4_LUP4_DM16,
	LEGS_S4_LUP5_DM16,
	LEGS_S4_RUP1_DM16,
	LEGS_S4_RUP2_DM16,
	LEGS_S4_RUP3_DM16,
	LEGS_S4_RUP4_DM16,
	LEGS_S4_RUP5_DM16,
	LEGS_S5_LUP1_DM16,
	LEGS_S5_LUP2_DM16,
	LEGS_S5_LUP3_DM16,
	LEGS_S5_LUP4_DM16,
	LEGS_S5_LUP5_DM16,
	LEGS_S5_RUP1_DM16,
	LEGS_S5_RUP2_DM16,
	LEGS_S5_RUP3_DM16,
	LEGS_S5_RUP4_DM16,
	LEGS_S5_RUP5_DM16,
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	FACE_TALK1_DM16,			//# quiet
	FACE_TALK2_DM16,			//# semi-quiet
	FACE_TALK3_DM16,			//# semi-loud
	FACE_TALK4_DM16,			//# loud
	FACE_ALERT_DM16,				//# 
	FACE_SMILE_DM16,				//# 
	FACE_FROWN_DM16,				//# 
	FACE_DEAD_DM16,				//# 

	//# #eol
	MAX_ANIMATIONS_DM16,
	MAX_TOTALANIMATIONS_DM16
} animNumber16_t;

//#define SABER_ANIM_GROUP_SIZE (BOTH_A2_T__B_ - BOTH_A1_T__B_)


const static int jk2dm15AnimsToGeneral[]  //# animNumber_e
{
	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1_GENERAL,		//# First Death anim
	BOTH_DEATH2_GENERAL,			//# Second Death anim
	BOTH_DEATH3_GENERAL,			//# Third Death anim
	BOTH_DEATH4_GENERAL,			//# Fourth Death anim
	BOTH_DEATH5_GENERAL,			//# Fifth Death anim
	BOTH_DEATH6_GENERAL,			//# Sixth Death anim
	BOTH_DEATH7_GENERAL,			//# Seventh Death anim
	BOTH_DEATH8_GENERAL,			//# 
	BOTH_DEATH9_GENERAL,			//# 
	BOTH_DEATH10_GENERAL,			//# 
	BOTH_DEATH11_GENERAL,			//#
	BOTH_DEATH12_GENERAL,			//# 
	BOTH_DEATH13_GENERAL,			//# 
	BOTH_DEATH14_GENERAL,			//# 
	BOTH_DEATH14_UNGRIP_GENERAL,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP_GENERAL,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_DEATH15_GENERAL,			//# 
	BOTH_DEATH16_GENERAL,			//# 
	BOTH_DEATH17_GENERAL,			//# 
	BOTH_DEATH18_GENERAL,			//# 
	BOTH_DEATH19_GENERAL,			//# 

	BOTH_DEATHFORWARD1_GENERAL,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2_GENERAL,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3_GENERAL,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1_GENERAL,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2_GENERAL,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE_GENERAL,		//# Idle while close to death
	BOTH_LYINGDEATH1_GENERAL,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1_GENERAL,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1_GENERAL,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR_GENERAL,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND_GENERAL,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL_GENERAL,		//# Death anim from a roll
	BOTH_DEATH_FLIP_GENERAL,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R_GENERAL,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L_GENERAL,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180_GENERAL,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP_GENERAL,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN_GENERAL,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN_GENERAL,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP_GENERAL,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED_GENERAL,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1_GENERAL,				//# First Death finished pose
	BOTH_DEAD2_GENERAL,				//# Second Death finished pose
	BOTH_DEAD3_GENERAL,				//# Third Death finished pose
	BOTH_DEAD4_GENERAL,				//# Fourth Death finished pose
	BOTH_DEAD5_GENERAL,				//# Fifth Death finished pose
	BOTH_DEAD6_GENERAL,				//# Sixth Death finished pose
	BOTH_DEAD7_GENERAL,				//# Seventh Death finished pose
	BOTH_DEAD8_GENERAL,				//# 
	BOTH_DEAD9_GENERAL,				//# 
	BOTH_DEAD10_GENERAL,			//# 
	BOTH_DEAD11_GENERAL,			//#
	BOTH_DEAD12_GENERAL,			//# 
	BOTH_DEAD13_GENERAL,			//# 
	BOTH_DEAD14_GENERAL,			//# 
	BOTH_DEAD15_GENERAL,			//# 
	BOTH_DEAD16_GENERAL,			//# 
	BOTH_DEAD17_GENERAL,			//# 
	BOTH_DEAD18_GENERAL,			//# 
	BOTH_DEAD19_GENERAL,			//# 
	BOTH_DEADFORWARD1_GENERAL,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2_GENERAL,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_GENERAL,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2_GENERAL,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1_GENERAL,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_GENERAL,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND_GENERAL,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1_GENERAL,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2_GENERAL,		//# React to being shot from Second Death finished pose
	BOTH_DEADFLOP3_GENERAL,		//# React to being shot from Third Death finished pose
	BOTH_DEADFLOP4_GENERAL,		//# React to being shot from Fourth Death finished pose
	BOTH_DEADFLOP5_GENERAL,		//# React to being shot from Fifth Death finished pose 
	BOTH_DEADFORWARD1_FLOP_GENERAL,		//# React to being shot First thrown forward death finished pose
	BOTH_DEADFORWARD2_FLOP_GENERAL,		//# React to being shot Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_FLOP_GENERAL,	//# React to being shot First thrown backward death finished pose
	BOTH_DEADBACKWARD2_FLOP_GENERAL,	//# React to being shot Second thrown backward death finished pose
	BOTH_LYINGDEAD1_FLOP_GENERAL,		//# React to being shot Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_FLOP_GENERAL,		//# React to being shot Stumble forward death finished pose
	BOTH_FALLDEAD1_FLOP_GENERAL,	//# React to being shot Fall forward and splat death finished pose
	BOTH_DISMEMBER_HEAD1_GENERAL,	//#
	BOTH_DISMEMBER_TORSO1_GENERAL,	//#
	BOTH_DISMEMBER_LLEG_GENERAL,	//#
	BOTH_DISMEMBER_RLEG_GENERAL,	//#
	BOTH_DISMEMBER_RARM_GENERAL,	//#
	BOTH_DISMEMBER_LARM_GENERAL,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1_GENERAL,				//# First take pain anim
	BOTH_PAIN2_GENERAL,				//# Second take pain anim
	BOTH_PAIN3_GENERAL,				//# Third take pain anim
	BOTH_PAIN4_GENERAL,				//# Fourth take pain anim
	BOTH_PAIN5_GENERAL,				//# Fifth take pain anim - from behind
	BOTH_PAIN6_GENERAL,				//# Sixth take pain anim - from behind
	BOTH_PAIN7_GENERAL,				//# Seventh take pain anim - from behind
	BOTH_PAIN8_GENERAL,				//# Eigth take pain anim - from behind
	BOTH_PAIN9_GENERAL,				//# 
	BOTH_PAIN10_GENERAL,			//# 
	BOTH_PAIN11_GENERAL,			//# 
	BOTH_PAIN12_GENERAL,			//# 
	BOTH_PAIN13_GENERAL,			//# 
	BOTH_PAIN14_GENERAL,			//# 
	BOTH_PAIN15_GENERAL,			//# 
	BOTH_PAIN16_GENERAL,			//# 
	BOTH_PAIN17_GENERAL,			//# 
	BOTH_PAIN18_GENERAL,			//# 
	BOTH_PAIN19_GENERAL,			//# 
	BOTH_PAIN20_GENERAL,			//# GETTING SHOCKED

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1_GENERAL,			//# Attack with stun baton
	BOTH_ATTACK2_GENERAL,			//# Attack with one-handed pistol
	BOTH_ATTACK3_GENERAL,			//# Attack with blaster rifle
	BOTH_ATTACK4_GENERAL,			//# Attack with disruptor
	BOTH_ATTACK5_GENERAL,			//# Attack with bow caster
	BOTH_ATTACK6_GENERAL,			//# Attack with ???
	BOTH_ATTACK7_GENERAL,			//# Attack with ???
	BOTH_ATTACK8_GENERAL,			//# Attack with ???
	BOTH_ATTACK9_GENERAL,			//# Attack with rocket launcher
	BOTH_ATTACK10_GENERAL,			//# Attack with thermal det
	BOTH_ATTACK11_GENERAL,			//# Attack with laser trap
	BOTH_ATTACK12_GENERAL,			//# Attack with detpack
	BOTH_MELEE1_GENERAL,			//# First melee attack
	BOTH_MELEE2_GENERAL,			//# Second melee attack
	BOTH_MELEE3_GENERAL,			//# Third melee attack
	BOTH_MELEE4_GENERAL,			//# Fourth melee attack
	BOTH_MELEE5_GENERAL,			//# Fifth melee attack
	BOTH_MELEE6_GENERAL,			//# Sixth melee attack
	BOTH_THERMAL_READY_GENERAL,		//# pull back with thermal
	BOTH_THERMAL_THROW_GENERAL,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 2
	BOTH_A1_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T1__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T1__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T1__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T1_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T1_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T1_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T1_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T1_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T1_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T1_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T1__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T1_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B1_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR____GENERAL,	//# Deflection toward BR
	BOTH_D1__R____GENERAL,	//# Deflection toward R
	BOTH_D1_TR____GENERAL,	//# Deflection toward TR
	BOTH_D1_TL____GENERAL,	//# Deflection toward TL
	BOTH_D1__L____GENERAL,	//# Deflection toward L
	BOTH_D1_BL____GENERAL,	//# Deflection toward BL
	BOTH_D1_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T2__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T2__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T2__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T2_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T2_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T2_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T2_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T2_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T2_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T2_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T2__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T2_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B2_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR____GENERAL,	//# Deflection toward BR
	BOTH_D2__R____GENERAL,	//# Deflection toward R
	BOTH_D2_TR____GENERAL,	//# Deflection toward TR
	BOTH_D2_TL____GENERAL,	//# Deflection toward TL
	BOTH_D2__L____GENERAL,	//# Deflection toward L
	BOTH_D2_BL____GENERAL,	//# Deflection toward BL
	BOTH_D2_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T3__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T3__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T3__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T3_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T3_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T3_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T3_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T3_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T3_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T3_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T3__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T3_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B3_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR____GENERAL,	//# Deflection toward BR
	BOTH_D3__R____GENERAL,	//# Deflection toward R
	BOTH_D3_TR____GENERAL,	//# Deflection toward TR
	BOTH_D3_TL____GENERAL,	//# Deflection toward TL
	BOTH_D3__L____GENERAL,	//# Deflection toward L
	BOTH_D3_BL____GENERAL,	//# Deflection toward BL
	BOTH_D3_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T4__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T4__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T4__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T4_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T4_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T4_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T4_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T4_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T4_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T4_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T4__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T4_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B4_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR____GENERAL,	//# Deflection toward BR
	BOTH_D4__R____GENERAL,	//# Deflection toward R
	BOTH_D4_TR____GENERAL,	//# Deflection toward TR
	BOTH_D4_TL____GENERAL,	//# Deflection toward TL
	BOTH_D4__L____GENERAL,	//# Deflection toward L
	BOTH_D4_BL____GENERAL,	//# Deflection toward BL
	BOTH_D4_B_____GENERAL,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B__GENERAL,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R_GENERAL,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L_GENERAL,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL_GENERAL,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR_GENERAL,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL_GENERAL,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R_GENERAL,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL_GENERAL,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L_GENERAL,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL_GENERAL,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR_GENERAL,	//# Fast arc right to top right
	BOTH_T5__R_TL_GENERAL,	//# Fast arc right to top left
	BOTH_T5__R__L_GENERAL,	//# Fast weak spin right to left
	BOTH_T5__R_BL_GENERAL,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR_GENERAL,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL_GENERAL,	//# Fast arc top right to top left
	BOTH_T5_TR__L_GENERAL,	//# Fast arc top right to left
	BOTH_T5_TR_BL_GENERAL,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR_GENERAL,	//# Fast arc top to bottom right
	BOTH_T5_T___R_GENERAL,	//# Fast arc top to right
	BOTH_T5_T__TR_GENERAL,	//# Fast arc top to top right
	BOTH_T5_T__TL_GENERAL,	//# Fast arc top to top left
	BOTH_T5_T___L_GENERAL,	//# Fast arc top to left
	BOTH_T5_T__BL_GENERAL,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR_GENERAL,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL_GENERAL,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR_GENERAL,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R_GENERAL,	//# Fast weak spin left to right
	BOTH_T5__L_TL_GENERAL,	//# Fast arc left to top left
	BOTH_T5_BL_BR_GENERAL,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R_GENERAL,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR_GENERAL,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L_GENERAL,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR_GENERAL,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T__GENERAL,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR_GENERAL,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T__GENERAL,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R_GENERAL,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T__GENERAL,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R_GENERAL,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR_GENERAL,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T__GENERAL,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L_GENERAL,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR_GENERAL,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T__GENERAL,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL_GENERAL,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T__GENERAL,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL_GENERAL,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T__GENERAL,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L_GENERAL,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R_GENERAL,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL_GENERAL,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR_GENERAL,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL_GENERAL,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR_GENERAL,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1_GENERAL,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1_GENERAL,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1_GENERAL,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1_GENERAL,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1_GENERAL,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1_GENERAL,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1_GENERAL,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_GENERAL, played backwards)
	BOTH_B5_BR____GENERAL,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R____GENERAL,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR____GENERAL,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T_____GENERAL,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL____GENERAL,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L____GENERAL,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL____GENERAL,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR____GENERAL,	//# Deflection toward BR
	BOTH_D5__R____GENERAL,	//# Deflection toward R
	BOTH_D5_TR____GENERAL,	//# Deflection toward TR
	BOTH_D5_TL____GENERAL,	//# Deflection toward TL
	BOTH_D5__L____GENERAL,	//# Deflection toward L
	BOTH_D5_BL____GENERAL,	//# Deflection toward BL
	BOTH_D5_B_____GENERAL,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T__GENERAL,	//# Block shot/saber top
	BOTH_P1_S1_TR_GENERAL,	//# Block shot/saber top right
	BOTH_P1_S1_TL_GENERAL,	//# Block shot/saber top left
	BOTH_P1_S1_BL_GENERAL,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR_GENERAL,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T__GENERAL,	//# knockaway saber top
	BOTH_K1_S1_TR_GENERAL,	//# knockaway saber top right
	BOTH_K1_S1_TL_GENERAL,	//# knockaway saber top left
	BOTH_K1_S1_BL_GENERAL,	//# knockaway saber bottom left
	BOTH_K1_S1_B__GENERAL,	//# knockaway saber bottom
	BOTH_K1_S1_BR_GENERAL,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1_GENERAL,	//# BR attack knocked away
	BOTH_V1__R_S1_GENERAL,	//# R attack knocked away
	BOTH_V1_TR_S1_GENERAL,	//# TR attack knocked away
	BOTH_V1_T__S1_GENERAL,	//# T attack knocked away
	BOTH_V1_TL_S1_GENERAL,	//# TL attack knocked away
	BOTH_V1__L_S1_GENERAL,	//# L attack knocked away
	BOTH_V1_BL_S1_GENERAL,	//# BL attack knocked away
	BOTH_V1_B__S1_GENERAL,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T__GENERAL,	//# saber knocked down from top parry
	BOTH_H1_S1_TR_GENERAL,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL_GENERAL,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL_GENERAL,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B__GENERAL,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR_GENERAL,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	BOTH_BF2RETURN_GENERAL,	//#
	BOTH_BF2BREAK_GENERAL,	//#
	BOTH_BF2LOCK_GENERAL,	//#
	BOTH_BF1RETURN_GENERAL,	//#
	BOTH_BF1BREAK_GENERAL,	//#
	BOTH_BF1LOCK_GENERAL,	//#
	BOTH_CWCIRCLE_R2__R_S1_GENERAL,	//#
	BOTH_CCWCIRCLE_R2__L_S1_GENERAL,	//#
	BOTH_CWCIRCLE_A2__L__R_GENERAL,	//#
	BOTH_CCWCIRCLE_A2__R__L_GENERAL,	//#
	BOTH_CWCIRCLEBREAK_GENERAL,	//#
	BOTH_CCWCIRCLEBREAK_GENERAL,	//#
	BOTH_CWCIRCLELOCK_GENERAL,	//#
	BOTH_CCWCIRCLELOCK_GENERAL,	//#


	//# #sep BOTH_ STANDING
	BOTH_STAND1_GENERAL,			//# Standing idle_GENERAL, no weapon_GENERAL, hands down
	BOTH_STAND1_RANDOM1_GENERAL,	//# Random standing idle
	BOTH_STAND1_RANDOM2_GENERAL,	//# Random standing idle
	BOTH_STAND2_GENERAL,			//# Standing idle with a weapon
	BOTH_STAND2_RANDOM1_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM2_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM3_GENERAL,	//# Random standing idle
	BOTH_STAND2_RANDOM4_GENERAL,	//# Random standing idle
	BOTH_STAND3_GENERAL,			//# Standing hands behind back_GENERAL, at ease_GENERAL, etc.
	BOTH_STAND4_GENERAL,			//# hands clasp behind back
	BOTH_STAND5_GENERAL,			//# standing idle_GENERAL, no weapon_GENERAL, hand down_GENERAL, back straight
	BOTH_STAND6_GENERAL,			//# one handed_GENERAL, gun at side_GENERAL, relaxed stand
	BOTH_STAND7_GENERAL,			//# both hands on hips (female)
	BOTH_STAND8_GENERAL,			//# both hands on hips (male)
	BOTH_STAND1TO3_GENERAL,			//# Transition from stand1 to stand3
	BOTH_STAND3TO1_GENERAL,			//# Transition from stand3 to stand1
	BOTH_STAND1TO2_GENERAL,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1_GENERAL,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4_GENERAL,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2_GENERAL,			//# Transition from stand4 to stand2
	BOTH_STANDTOWALK1_GENERAL,		//# Transition from stand1 to walk1
	BOTH_STAND4TOATTACK2_GENERAL,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP1_GENERAL,			//# standing up and stumbling
	BOTH_STANDUP2_GENERAL,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3_GENERAL,		//# transition from stand 5 to sit 3
	BOTH_STAND1_REELO_GENERAL,		//# Reelo in his stand1 position (cin #18)
	BOTH_STAND5_REELO_GENERAL,		//# Reelo in his stand5 position (cin #18)
	BOTH_STAND1TOSTAND5_GENERAL,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1_GENERAL,	//# Transition from stand5 to stand1
	BOTH_STAND5TOSTAND8_GENERAL,	//# Transition from stand5 to stand8
	BOTH_STAND8TOSTAND5_GENERAL,	//# Transition from stand8 to stand5

	BOTH_CONSOLE1START_GENERAL,		//# typing at a console
	BOTH_CONSOLE1_GENERAL,			//# typing at a console
	BOTH_CONSOLE1STOP_GENERAL,		//# typing at a console
	BOTH_CONSOLE2START_GENERAL,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2_GENERAL,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP_GENERAL,		//# typing at a console with comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1_GENERAL,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1_GENERAL,		//# Cradling weapon and standing
	BOTH_ALERT1_GENERAL,			//# Startled by something while on guard
	BOTH_GESTURE1_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE2_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_GESTURE3_GENERAL,			//# Generic gesture_GENERAL, non-specific
	BOTH_PAUSE1START_GENERAL,			//# Luke pauses to warn Kyle (cin #24) start
	BOTH_PAUSE1STOP_GENERAL,			//# Luke pauses to warn Kyle (cin #24) stop

	BOTH_HEADTILTLSTART_GENERAL,		//# Head tilt to left
	BOTH_HEADTILTLSTOP_GENERAL,			//# Head tilt to left
	BOTH_HEADTILTRSTART_GENERAL,		//# Head tilt to right
	BOTH_HEADTILTRSTOP_GENERAL,			//# Head tilt to right
	BOTH_HEADNOD_GENERAL,				//# Head shake YES
	BOTH_HEADSHAKE_GENERAL,				//# Head shake NO
	BOTH_HEADSHAKE1_REELO_GENERAL,		//# Head shake NO for Reelo
	BOTH_SITHEADTILTLSTART_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTLSTOP_GENERAL,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTRSTART_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADTILTRSTOP_GENERAL,		//# Head tilt to right from seated position
	BOTH_SITHEADNOD_GENERAL,			//# Head shake YES from seated position
	BOTH_SITHEADSHAKE_GENERAL,			//# Head shake NO from seated position

	BOTH_REACH1START_GENERAL,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP_GENERAL,		//# Monmothma reaching for crystal

	BOTH_EXAMINE1START_GENERAL,		//# Start Mon Mothma examining crystal 
	BOTH_EXAMINE1_GENERAL,			//# Mon Mothma examining crystal 
	BOTH_EXAMINE1STOP_GENERAL,		//# Stop Mon Mothma examining crystal 
	BOTH_EXAMINE2START_GENERAL,	//# Start Kyle tossing crystal
	BOTH_EXAMINE2_GENERAL,			//# Hold Kyle tossing crystal
	BOTH_EXAMINE2STOP_GENERAL,		//# End Kyle tossing crystal
	BOTH_EXAMINE3START_GENERAL,	//# Start Lando looking around corner
	BOTH_EXAMINE3_GENERAL,			//# Hold Lando looking around corner
	BOTH_EXAMINE3STOP_GENERAL,		//# End Lando looking around corner

	BOTH_THROW1START_GENERAL,		//# Kyle thrown to the right
	BOTH_THROW1_GENERAL,			//# Kyle thrown to the right
	BOTH_THROW1STOP_GENERAL,		//# Kyle thrown to the right
	BOTH_THROW2START_GENERAL,		//# Kyle thrown to the left
	BOTH_THROW2_GENERAL,			//# Kyle thrown to the left
	BOTH_THROW3_GENERAL,			//# Kyle thrown backwards in cin #9

	BOTH_LEANLEFT2START_GENERAL,	//# Start leaning left in chair
	BOTH_LEANLEFT2STOP_GENERAL,		//# Stop leaning left in chair
	BOTH_LEANRIGHT3START_GENERAL,	//# Start Lando leaning on wall
	BOTH_LEANRIGHT3_GENERAL,			//# Lando leaning on wall
	BOTH_LEANRIGHT3STOP_GENERAL,		//# Stop Lando leaning on wall

	BOTH_FORCEFOUNTAIN1_START_GENERAL,	//# Kyle being lifted into the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_MIDDLE_GENERAL,	//# Kyle changing to looping position in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_LOOP_GENERAL,	//# Kyle being spun in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_STOP_GENERAL,	//# Kyle being set down out of the Force Fountain (cin #10)
	BOTH_THUMBING1_GENERAL,				//# Lando gesturing with thumb over his shoulder (cin #19)
	BOTH_COME_ON1_GENERAL,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1_GENERAL,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END_GENERAL,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1_GENERAL,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1_GENERAL,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_PUNCHER1_GENERAL,				//# Jan punching Kyle in the shoulder (cin #37)
	BOTH_CONSTRAINER1HOLD_GENERAL,		//# Static pose of starting Tavion constraining Jan (cin #9)
	BOTH_CONSTRAINEE1HOLD_GENERAL,		//# Static pose of starting Jan being constrained by Tavion (cin #9)
	BOTH_CONSTRAINER1STAND_GENERAL,		//# Tavion constraining Jan in a stand pose (cin #9)
	BOTH_CONSTRAINEE1STAND_GENERAL,		//# Jan being constrained in a stand pose (cin #9)
	BOTH_CONSTRAINER1WALK_GENERAL,		//# Tavion shoving jan forward (cin #9)
	BOTH_CONSTRAINEE1WALK_GENERAL,		//# Jan being shoved forward by Tavion (cin #9)
	BOTH_CONSTRAINER1LOOP_GENERAL,		//# Tavion walking with Jan in a loop (cin #9)
	BOTH_CONSTRAINEE1LOOP_GENERAL,		//# Jan walking with Tavion in a loop (cin #9)
	BOTH_SABERKILLER1_GENERAL,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1_GENERAL,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HANDSHAKER1START_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKER1LOOP_GENERAL,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKEE1START_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_HANDSHAKEE1LOOP_GENERAL,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_LAUGH1START_GENERAL,			//# Reelo leaning forward before laughing (cin #18)
	BOTH_LAUGH1STOP_GENERAL,			//# Reelo laughing (cin #18)
	BOTH_ESCAPEPOD_LEAVE1_GENERAL,	//# Kyle leaving escape pod (cin #33)
	BOTH_ESCAPEPOD_LEAVE2_GENERAL,	//# Jan leaving escape pod (cin #33)
	BOTH_BARTENDER_IDLE1_GENERAL,	//# Bartender idle in cin #15
	BOTH_BARTENDER_THROW1_GENERAL,	//# Bartender throws glass in cin #15
	BOTH_BARTENDER_COWERSTART_GENERAL,	//# Start of Bartender raising both hands up in surrender (cin #16)
	BOTH_BARTENDER_COWERLOOP_GENERAL,	//# Loop of Bartender waving both hands in surrender (cin #16)
	BOTH_BARTENDER_COWER_GENERAL,		//# Single frame of Bartender waving both hands in surrender (cin #16)
	BOTH_THREATEN1_START_GENERAL,	//# First frame of Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_THREATEN1_GENERAL,			//# Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_RADIO_ONOFF_GENERAL,		//# Mech Galak turning on his suit radio (cin #32)
	BOTH_TRIUMPHANT1START_GENERAL,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STOP_GENERAL,	//# Mech Galak lowering his arms in victory (cin #32)

	BOTH_SABERTHROW1START_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP_GENERAL,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START_GENERAL,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP_GENERAL,		//# Kyle throwing his light saber (cin #32)

	BOTH_COVERUP1_LOOP_GENERAL,		//# animation of getting in line of friendly fire
	BOTH_COVERUP1_START_GENERAL,	//# transitions from stand to coverup1_loop
	BOTH_COVERUP1_END_GENERAL,		//# transitions from coverup1_loop to stand

	BOTH_INJURED4_GENERAL,			//# Injured pose 4
	BOTH_INJURED4TO5_GENERAL,		//# Transition from INJURED4 to INJURED5
	BOTH_INJURED5_GENERAL,			//# Injured pose 5

	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1STAND_GENERAL,			//# Stand up from First sitting anim
	BOTH_SIT1_GENERAL,				//# Normal chair sit.
	BOTH_SIT2_GENERAL,				//# Lotus position.
	BOTH_SIT3_GENERAL,				//# Sitting in tired position_GENERAL, elbows on knees

	BOTH_SIT2TO3_GENERAL,			//# Trans from sit2 to sit3?
	BOTH_SIT2TOSTAND5_GENERAL,		//# Transition from sit 2 to stand 5
	BOTH_SIT3TO1_GENERAL,			//# Trans from sit3 to sit1?
	BOTH_SIT3TO2_GENERAL,			//# Trans from sit3 to sit2?
	BOTH_SIT3TOSTAND5_GENERAL,		//# transition from sit 3 to stand 5

	BOTH_SIT4TO5_GENERAL,			//# Trans from sit4 to sit5
	BOTH_SIT4TO6_GENERAL,			//# Trans from sit4 to sit6
	BOTH_SIT5TO4_GENERAL,			//# Trans from sit5 to sit4
	BOTH_SIT5TO6_GENERAL,			//# Trans from sit5 to sit6
	BOTH_SIT6TO4_GENERAL,			//# Trans from sit6 to sit4
	BOTH_SIT6TO5_GENERAL,			//# Trans from sit6 to sit5
	BOTH_SIT7_GENERAL,				//# sitting with arms over knees_GENERAL, no weapon
	BOTH_SIT7TOSTAND1_GENERAL,		//# getting up from sit7 into stand1

	BOTH_CROUCH1_GENERAL,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE_GENERAL,		//# Crouching idle
	BOTH_CROUCH1WALK_GENERAL,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK_GENERAL,	//# Walking while crouched
	BOTH_UNCROUCH1_GENERAL,			//# Transition from crouch to standing
	BOTH_CROUCH2IDLE_GENERAL,		//# crouch and resting on back righ heel_GENERAL, no weapon
	BOTH_CROUCH2TOSTAND1_GENERAL,	//# going from crouch2 to stand1
	BOTH_CROUCH3_GENERAL,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3_GENERAL,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_GET_UP1_GENERAL,			//# Get up from the ground_GENERAL, face down
	BOTH_GET_UP2_GENERAL,			//# Get up from the ground_GENERAL, face up

	BOTH_COCKPIT_CONSOLE1_GENERAL,		//# work console1 while sitting in a cockpit.
	BOTH_COCKPIT_CONSOLE2_GENERAL,		//# work console2 while sitting in a cockpit.
	BOTH_COCKPIT_SIT_GENERAL,			//# sit in a cockpit.

	BOTH_GUNSIT1_GENERAL,			//# sitting on an emplaced gun.

	BOTH_KNEES1_GENERAL,			//# Tavion on her knees
	BOTH_KNEES2_GENERAL,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1_GENERAL,			//# Transition of KNEES2 to KNEES1
	BOTH_STRUGGLE1START_GENERAL,	//# Kyle struggling under crate
	BOTH_STRUGGLE1_GENERAL,			//# Kyle struggling under crate
	BOTH_STRUGGLE1STOP_GENERAL,		//# Kyle struggling under crate
	BOTH_RUMMAGE1START_GENERAL,	//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1_GENERAL,			//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1STOP_GENERAL,		//# Kyle rummaging for crystal (cin 2)
	BOTH_HOLDGLASS1_GENERAL,		//# Bartender holds glass (cin# 14)
	BOTH_SLIDEGLASS1_GENERAL,		//# Bartender slides glass (cin# 14)
	BOTH_SLAMSABERDOWN_GENERAL,		//# Kyle slamming his saber on the bar top (cin# 14)

	//# #sep BOTH_ MOVING
	BOTH_WALK1_GENERAL,				//# Normal walk
	BOTH_WALK2_GENERAL,				//# Normal walk
	BOTH_WALK3_GENERAL,				//# Goes with stand3
	BOTH_WALK4_GENERAL,				//# Walk cycle goes to a stand4
	BOTH_WALK5_GENERAL,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6_GENERAL,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7_GENERAL,				//# Fast walk
	BOTH_WALKTORUN1_GENERAL,		//# transition from walk to run
	BOTH_RUN1_GENERAL,				//# Full run
	BOTH_RUN1START_GENERAL,			//# Start into full run1
	BOTH_RUN1STOP_GENERAL,			//# Stop from full run1
	BOTH_RUN2_GENERAL,				//# Full run
	BOTH_RUNINJURED1_GENERAL,		//# Run with injured left leg
	BOTH_STRAFE_LEFT1_GENERAL,		//# Sidestep left_GENERAL, should loop
	BOTH_STRAFE_RIGHT1_GENERAL,		//# Sidestep right_GENERAL, should loop
	BOTH_RUNSTRAFE_LEFT1_GENERAL,	//# Sidestep left_GENERAL, should loop
	BOTH_RUNSTRAFE_RIGHT1_GENERAL,	//# Sidestep right_GENERAL, should loop
	BOTH_TURN_LEFT1_GENERAL,		//# Turn left_GENERAL, should loop
	BOTH_TURN_RIGHT1_GENERAL,		//# Turn right_GENERAL, should loop
	BOTH_TURNSTAND2_GENERAL,		//# Turn from STAND2 position
	BOTH_TURNSTAND3_GENERAL,		//# Turn from STAND3 position
	BOTH_TURNSTAND4_GENERAL,		//# Turn from STAND4 position
	BOTH_TURNSTAND5_GENERAL,		//# Turn from STAND5 position
	BOTH_RUNAWAY1_GENERAL,			//# Running scared
	BOTH_SWIM1_GENERAL,				//# Swimming

	BOTH_WALKBACK1_GENERAL,			//# Walk1 backwards
	BOTH_WALKBACK2_GENERAL,			//# Walk2 backwards
	BOTH_RUNBACK1_GENERAL,			//# Run1 backwards
	BOTH_RUNBACK2_GENERAL,			//# Run1 backwards

	//# #sep BOTH_ JUMPING
	BOTH_JUMP1_GENERAL,				//# Jump - wind-up and leave ground
	BOTH_INAIR1_GENERAL,			//# In air loop (from jump)
	BOTH_LAND1_GENERAL,				//# Landing (from in air loop)
	BOTH_LAND2_GENERAL,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1_GENERAL,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1_GENERAL,		//# In air loop (from jump back)
	BOTH_LANDBACK1_GENERAL,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1_GENERAL,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1_GENERAL,		//# In air loop (from jump left)
	BOTH_LANDLEFT1_GENERAL,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1_GENERAL,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1_GENERAL,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1_GENERAL,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1_GENERAL,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1_GENERAL,		//# In air loop (from jump)
	BOTH_FORCELAND1_GENERAL,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1_GENERAL,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1_GENERAL,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1_GENERAL,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1_GENERAL,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1_GENERAL,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1_GENERAL,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1_GENERAL,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1_GENERAL,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1_GENERAL,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F_GENERAL,			//# Flip forward
	BOTH_FLIP_B_GENERAL,			//# Flip backwards
	BOTH_FLIP_L_GENERAL,			//# Flip left
	BOTH_FLIP_R_GENERAL,			//# Flip right

	BOTH_ROLL_F_GENERAL,			//# Roll forward
	BOTH_ROLL_B_GENERAL,			//# Roll backward
	BOTH_ROLL_L_GENERAL,			//# Roll left
	BOTH_ROLL_R_GENERAL,			//# Roll right
	BOTH_ROLL_FR_GENERAL,			//# Roll forward right
	BOTH_ROLL_FL_GENERAL,			//# Roll forward left
	BOTH_ROLL_BR_GENERAL,			//# Roll back right
	BOTH_ROLL_BL_GENERAL,			//# Roll back left

	BOTH_HOP_F_GENERAL,				//# quickstep forward
	BOTH_HOP_B_GENERAL,				//# quickstep backwards
	BOTH_HOP_L_GENERAL,				//# quickstep left
	BOTH_HOP_R_GENERAL,				//# quickstep right

	BOTH_DODGE_FL_GENERAL,			//# lean-dodge forward left
	BOTH_DODGE_FR_GENERAL,			//# lean-dodge forward right
	BOTH_DODGE_BL_GENERAL,			//# lean-dodge backwards left
	BOTH_DODGE_BR_GENERAL,			//# lean-dodge backwards right
	BOTH_DODGE_L_GENERAL,			//# lean-dodge left
	BOTH_DODGE_R_GENERAL,			//# lean-dodge right

	BOTH_DIVE1_GENERAL,				//# Dive!

	BOTH_SABERFAST_STANCE_GENERAL,
	BOTH_SABERSLOW_STANCE_GENERAL,
	BOTH_ENGAGETAUNT_GENERAL,
	BOTH_A2_STABBACK1_GENERAL,		//# Stab saber backward
	BOTH_ATTACK_BACK_GENERAL,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1_GENERAL,//#
	BOTH_JUMPFLIPSTABDOWN_GENERAL,//#
	BOTH_FORCELEAP2_T__B__GENERAL,//#
	BOTH_LUNGE2_B__T__GENERAL,//#
	BOTH_CROUCHATTACKBACK1_GENERAL,//#
	BOTH_ARIAL_LEFT_GENERAL,		//# 
	BOTH_ARIAL_RIGHT_GENERAL,		//# 
	BOTH_CARTWHEEL_LEFT_GENERAL,	//# 
	BOTH_CARTWHEEL_RIGHT_GENERAL,	//# 
	BOTH_FLIP_LEFT_GENERAL,			//# 
	BOTH_FLIP_BACK1_GENERAL,		//# 
	BOTH_FLIP_BACK2_GENERAL,		//# 
	BOTH_FLIP_BACK3_GENERAL,		//# 
	BOTH_BUTTERFLY_LEFT_GENERAL,	//# 
	BOTH_BUTTERFLY_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_GENERAL,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_RIGHT_STOP_GENERAL,//# 
	BOTH_WALL_RUN_LEFT_GENERAL,		//# 
	BOTH_WALL_RUN_LEFT_FLIP_GENERAL,//#
	BOTH_WALL_RUN_LEFT_STOP_GENERAL,//# 
	BOTH_WALL_FLIP_RIGHT_GENERAL,	//# 
	BOTH_WALL_FLIP_LEFT_GENERAL,	//# 
	BOTH_WALL_FLIP_FWD_GENERAL,		//#
	BOTH_KNOCKDOWN1_GENERAL,		//# knocked backwards
	BOTH_KNOCKDOWN2_GENERAL,		//# knocked backwards hard
	BOTH_KNOCKDOWN3_GENERAL,		//#	knocked forwards
	BOTH_KNOCKDOWN4_GENERAL,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5_GENERAL,		//# dupe of 3 - will be removed
	BOTH_GETUP1_GENERAL,			//#
	BOTH_GETUP2_GENERAL,			//#
	BOTH_GETUP3_GENERAL,			//#
	BOTH_GETUP4_GENERAL,			//#
	BOTH_GETUP5_GENERAL,			//#
	BOTH_GETUP_CROUCH_F1_GENERAL,	//#
	BOTH_GETUP_CROUCH_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_F1_GENERAL,	//#
	BOTH_FORCE_GETUP_F2_GENERAL,	//#
	BOTH_FORCE_GETUP_B1_GENERAL,	//#
	BOTH_FORCE_GETUP_B2_GENERAL,	//#
	BOTH_FORCE_GETUP_B3_GENERAL,	//#
	BOTH_FORCE_GETUP_B4_GENERAL,	//#
	BOTH_FORCE_GETUP_B5_GENERAL,	//#
	BOTH_FORCE_GETUP_B6_GENERAL,	//#
	BOTH_WALL_FLIP_BACK1_GENERAL,	//#
	BOTH_WALL_FLIP_BACK2_GENERAL,	//#
	BOTH_SPIN1_GENERAL,				//#
	BOTH_CEILING_CLING_GENERAL,		//# clinging to ceiling
	BOTH_CEILING_DROP_GENERAL,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL_GENERAL,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR_GENERAL,		//# jump spin slash bl to tr
	BOTH_DEATHFROMBACKSLASH_GENERAL,//#
	BOTH_RIGHTHANDCHOPPEDOFF_GENERAL,//#
	BOTH_DEFLECTSLASH__R__L_FIN_GENERAL,//#
	BOTH_BASHED1_GENERAL,//#
	BOTH_ARIAL_F1_GENERAL,//#
	BOTH_BUTTERFLY_FR1_GENERAL,//#
	BOTH_BUTTERFLY_FL1_GENERAL,//#
	BOTH_POSE1_GENERAL,//#
	BOTH_POSE2_GENERAL,//#
	BOTH_POSE3_GENERAL,//#
	BOTH_POSE4_GENERAL,//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1_GENERAL,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1_GENERAL,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1_GENERAL,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE_GENERAL,		//#	Just sitting on the ladder
	BOTH_ONLADDER_BOT1_GENERAL,		//# Getting on the ladder at the bottom
	BOTH_OFFLADDER_BOT1_GENERAL,	//# Getting off the ladder at the bottom
	BOTH_ONLADDER_TOP1_GENERAL,		//# Getting on the ladder at the top
	BOTH_OFFLADDER_TOP1_GENERAL,	//# Getting off the ladder at the top
	BOTH_LIFT1_GENERAL,				//# Lifting someone/thing over their shoulder
	BOTH_STEP1_GENERAL,				//# telsia checking out lake cinematic9.2
	BOTH_HITWALL1_GENERAL,			//# cin.18_GENERAL, Kenn hit by borg into wall 56 units away
	BOTH_AMBUSHLAND1_GENERAL,		//# landing from fall on victim
	BOTH_BIRTH1_GENERAL,			//# birth from jumping through walls

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_IDLE1_GENERAL,			//# Flying Idle 1
	BOTH_FLY_IDLE2_GENERAL,			//# Flying Idle 2
	BOTH_FLY_SHIELDED_GENERAL,		//# For sentry droid_GENERAL, shields in


	//# #sep BOTH_ FLYING MOVING
	BOTH_FLY_START1_GENERAL,		//# Start flying
	BOTH_FLY_STOP1_GENERAL,			//# Stop flying
	BOTH_FLY_LOOP1_GENERAL,			//# Normal flying_GENERAL, should loop
	BOTH_FLOAT1_GENERAL,			//# Crew floating through space 1
	BOTH_FLOAT2_GENERAL,			//# Crew floating through space 2
	BOTH_FLOATCONSOLE1_GENERAL,		//# Crew floating and working on console

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1_GENERAL,		//# Swimming Idle 1
	BOTH_SWIMFORWARDSTART_GENERAL,	//# Swim forward start
	BOTH_SWIMFORWARD_GENERAL,		//# Swim forward loop
	BOTH_SWIMFORWARDSTOP_GENERAL,	//# Swim forward end
	BOTH_SWIMBACKWARDSTART_GENERAL,	//# Swim backward start
	BOTH_SWIMBACKWARD_GENERAL,		//# Swim backward loop
	BOTH_SWIMBACKWARDSTOP_GENERAL,	//# Swim backward end

	//# #sep BOTH_ LYING
	BOTH_LIE_DOWN1_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face down
	BOTH_LIE_DOWN2_GENERAL,			//# From a stand position_GENERAL, get down on ground_GENERAL, face up
	BOTH_LIE_DOWN3_GENERAL,			//# reaction to local disnode being destroyed
	BOTH_PAIN2WRITHE1_GENERAL,		//# Transition from upright position to writhing on ground anim
	BOTH_PRONE2RLEG_GENERAL,		//# Lying on ground reach to grab right leg
	BOTH_PRONE2LLEG_GENERAL,		//# Lying on ground reach to grab left leg
	BOTH_WRITHING1_GENERAL,			//# Lying on ground on back writhing in pain
	BOTH_WRITHING1RLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding right leg
	BOTH_WRITHING1LLEG_GENERAL,		//# Lying on ground writhing in pain_GENERAL, holding left leg
	BOTH_WRITHING2_GENERAL,			//# Lying on ground on front writhing in pain
	BOTH_INJURED1_GENERAL,			//# Lying down_GENERAL, against wall - can also be sleeping against wall
	BOTH_INJURED2_GENERAL,			//# Injured pose 2
	BOTH_INJURED3_GENERAL,			//# Injured pose 3
	BOTH_INJURED6_GENERAL,			//# Injured pose 6
	BOTH_INJURED6ATTACKSTART_GENERAL,	//# Start attack while in injured 6 pose 
	BOTH_INJURED6ATTACKSTOP_GENERAL,	//# End attack while in injured 6 pose
	BOTH_INJURED6COMBADGE_GENERAL,	//# Hit combadge while in injured 6 pose
	BOTH_INJURED6POINT_GENERAL,		//# Chang points to door while in injured state
	BOTH_INJUREDTOSTAND1_GENERAL,	//# Runinjured to stand1

	BOTH_PROPUP1_GENERAL,			//# Kyle getting up from having been knocked down (cin #9 end)
	BOTH_CRAWLBACK1_GENERAL,		//# Lying on back_GENERAL, crawling backwards with elbows
	BOTH_SITWALL1_GENERAL,			//# Sitting against a wall
	BOTH_RESTRAINED1_GENERAL,		//# Telsia tied to medical table
	BOTH_RESTRAINED1POINT_GENERAL,	//# Telsia tied to medical table pointing at Munro
	BOTH_LIFTED1_GENERAL,			//# Fits with BOTH_LIFT1_GENERAL, lifted on shoulder
	BOTH_CARRIED1_GENERAL,			//# Fits with TORSO_CARRY1_GENERAL, carried over shoulder
	BOTH_CARRIED2_GENERAL,			//# Laying over object

	BOTH_CHOKE1START_GENERAL,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD_GENERAL,	//# loop of tavion in force grip choke
	BOTH_CHOKE1_GENERAL,			//# tavion in force grip choke

	BOTH_CHOKE2_GENERAL,			//# tavion recovering from force grip choke
	BOTH_CHOKE3_GENERAL,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1_GENERAL,			//# Wakes up

	BOTH_TURNON_GENERAL,			//# Protocol Droid wakes up
	BOTH_TURNOFF_GENERAL,			//# Protocol Droid shuts off

	BOTH_BUTTON1_GENERAL,			//# Single button push with right hand
	BOTH_BUTTON2_GENERAL,			//# Single button push with left finger
	BOTH_BUTTON_HOLD_GENERAL,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE_GENERAL,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	BOTH_RESISTPUSH_GENERAL,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1_GENERAL,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2_GENERAL,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING_GENERAL,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_HOLD_GENERAL,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE_GENERAL,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START_GENERAL,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP_GENERAL,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK_GENERAL,		//# Healing meditation gesture
	BOTH_SABERPULL_GENERAL,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1_GENERAL,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP2_GENERAL,		//# force-gripping (?)
	BOTH_FORCEGRIP3_GENERAL,		//# force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD_GENERAL,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE_GENERAL,//# Use off-hand to do grip - release
	BOTH_TOSS1_GENERAL,				//# throwing to left after force gripping
	BOTH_TOSS2_GENERAL,				//# throwing to right after force gripping

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP2_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP3_GENERAL,		//# Put weapon away
	TORSO_DROPWEAP4_GENERAL,		//# Put weapon away
	TORSO_RAISEWEAP1_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP2_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP3_GENERAL,		//# Draw Weapon
	TORSO_RAISEWEAP4_GENERAL,		//# Draw Weapon
	TORSO_WEAPONREADY1_GENERAL,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2_GENERAL,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3_GENERAL,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4_GENERAL,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY5_GENERAL,		//# Ready to fire bowcaster
	TORSO_WEAPONREADY6_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY7_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY8_GENERAL,		//# Ready to fire ???
	TORSO_WEAPONREADY9_GENERAL,		//# Ready to fire rocket launcher
	TORSO_WEAPONREADY10_GENERAL,	//# Ready to fire thermal det
	TORSO_WEAPONREADY11_GENERAL,	//# Ready to fire laser trap
	TORSO_WEAPONREADY12_GENERAL,	//# Ready to fire detpack
	TORSO_WEAPONIDLE1_GENERAL,		//# Holding stun baton
	TORSO_WEAPONIDLE2_GENERAL,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3_GENERAL,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4_GENERAL,		//# Holding sniper rifle
	TORSO_WEAPONIDLE5_GENERAL,		//# Holding bowcaster
	TORSO_WEAPONIDLE6_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE7_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE8_GENERAL,		//# Holding ???
	TORSO_WEAPONIDLE9_GENERAL,		//# Holding rocket launcher
	TORSO_WEAPONIDLE10_GENERAL,		//# Holding thermal det
	TORSO_WEAPONIDLE11_GENERAL,		//# Holding laser trap
	TORSO_WEAPONIDLE12_GENERAL,		//# Holding detpack

	//# #sep TORSO_ USING NON-WEAPON OBJECTS

	//# #sep TORSO_ MISC
	TORSO_TALKR1START_GENERAL,		//# begin turning head for BOTH_TORSO_TALKR
	TORSO_TALKR1HOLD_GENERAL,		//# non-looping version of talk to right side
	TORSO_TALKR1STOP_GENERAL,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKR1_GENERAL,			//# talk to right side
	TORSO_TALKL1START_GENERAL,		//# begin turning head for BOTH_TORSO_TALKL
	TORSO_TALKL1HOLD_GENERAL,		//# non-looping version of talk to left side
	TORSO_TALKL1STOP_GENERAL,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKL1_GENERAL,			//# talk to left side
	TORSO_LOOKL1_GENERAL,			//# looking left
	TORSO_LOOKR1_GENERAL,			//# looking right
	TORSO_LOOKR2START_GENERAL,		//# turn not so far as TALKR1
	TORSO_LOOKR2STOP_GENERAL,		//# turn not so far as TALKR1
	TORSO_LOOKR2_GENERAL,			//# looking right - not so far as LOOKR1
	TORSO_LOOKL2START_GENERAL,		//# turn not so far as TALKL1
	TORSO_LOOKL2STOP_GENERAL,		//# turn not so far as TALKL1
	TORSO_LOOKL2_GENERAL,			//# looking right - not so far as LOOKL1
	TORSO_HANDGESTURE1_GENERAL,		//# gestures to left one hand
	TORSO_HANDGESTURE2_GENERAL,		//# gestures to right one hand
	TORSO_HANDGESTURE3_GENERAL,		//# gestures to the left both hands
	TORSO_HANDGESTURE4_GENERAL,		//# gestures to the right both hands

	TORSO_HANDEXTEND1_GENERAL,		//# doctor reaching for hypospray in scav5
	TORSO_HANDRETRACT1_GENERAL,		//# doctor taking hypospray from player in scav5

	TORSO_DROPHELMET1_GENERAL,		//# Drop the helmet to the waist
	TORSO_RAISEHELMET1_GENERAL,		//# Bring the helmet to the head
	TORSO_REACHHELMET1_GENERAL,		//# reaching for helmet off of 60 tall cabinet
	TORSO_GRABLBACKL_GENERAL,		//# reach to lower back with left hand
	TORSO_GRABUBACKL_GENERAL,		//# reach to upper back with left hand
	TORSO_GRABLBACKR_GENERAL,		//# reach to lower back with right hand
	TORSO_GRABUBACKR_GENERAL,		//# reach to upper back with right hand

	TORSO_SURRENDER_START_GENERAL,	//# arms up
	TORSO_SURRENDER_STOP_GENERAL,	//# arms back down

	TORSO_CHOKING1_GENERAL,			//# TEMP


	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_WALKBACK1_GENERAL,			//# Walk1 backwards
	LEGS_WALKBACK2_GENERAL,			//# Walk2 backwards
	LEGS_RUNBACK1_GENERAL,			//# Run1 backwards
	LEGS_RUNBACK2_GENERAL,			//# Run2 backwards
	LEGS_TURN1_GENERAL,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2_GENERAL,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1_GENERAL,		//# Lean left
	LEGS_LEAN_RIGHT1_GENERAL,		//# Lean Right
	LEGS_KNEELDOWN1_GENERAL,		//# Get down on one knee?
	LEGS_KNEELUP1_GENERAL,			//# Get up from one knee?
	LEGS_CRLEAN_LEFT1_GENERAL,		//# Crouch Lean left
	LEGS_CRLEAN_RIGHT1_GENERAL,		//# Crouch Lean Right
	LEGS_CHOKING1_GENERAL,			//# TEMP
	LEGS_LEFTUP1_GENERAL,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2_GENERAL,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3_GENERAL,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4_GENERAL,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5_GENERAL,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1_GENERAL,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2_GENERAL,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3_GENERAL,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4_GENERAL,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5_GENERAL,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1_GENERAL,
	LEGS_S1_LUP2_GENERAL,
	LEGS_S1_LUP3_GENERAL,
	LEGS_S1_LUP4_GENERAL,
	LEGS_S1_LUP5_GENERAL,
	LEGS_S1_RUP1_GENERAL,
	LEGS_S1_RUP2_GENERAL,
	LEGS_S1_RUP3_GENERAL,
	LEGS_S1_RUP4_GENERAL,
	LEGS_S1_RUP5_GENERAL,
	LEGS_S3_LUP1_GENERAL,
	LEGS_S3_LUP2_GENERAL,
	LEGS_S3_LUP3_GENERAL,
	LEGS_S3_LUP4_GENERAL,
	LEGS_S3_LUP5_GENERAL,
	LEGS_S3_RUP1_GENERAL,
	LEGS_S3_RUP2_GENERAL,
	LEGS_S3_RUP3_GENERAL,
	LEGS_S3_RUP4_GENERAL,
	LEGS_S3_RUP5_GENERAL,
	LEGS_S4_LUP1_GENERAL,
	LEGS_S4_LUP2_GENERAL,
	LEGS_S4_LUP3_GENERAL,
	LEGS_S4_LUP4_GENERAL,
	LEGS_S4_LUP5_GENERAL,
	LEGS_S4_RUP1_GENERAL,
	LEGS_S4_RUP2_GENERAL,
	LEGS_S4_RUP3_GENERAL,
	LEGS_S4_RUP4_GENERAL,
	LEGS_S4_RUP5_GENERAL,
	LEGS_S5_LUP1_GENERAL,
	LEGS_S5_LUP2_GENERAL,
	LEGS_S5_LUP3_GENERAL,
	LEGS_S5_LUP4_GENERAL,
	LEGS_S5_LUP5_GENERAL,
	LEGS_S5_RUP1_GENERAL,
	LEGS_S5_RUP2_GENERAL,
	LEGS_S5_RUP3_GENERAL,
	LEGS_S5_RUP4_GENERAL,
	LEGS_S5_RUP5_GENERAL,
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	FACE_TALK1_GENERAL,			//# quiet
	FACE_TALK2_GENERAL,			//# semi-quiet
	FACE_TALK3_GENERAL,			//# semi-loud
	FACE_TALK4_GENERAL,			//# loud
	FACE_ALERT_GENERAL,				//# 
	FACE_SMILE_GENERAL,				//# 
	FACE_FROWN_GENERAL,				//# 
	FACE_DEAD_GENERAL,				//# 

	//Boot
	BOOT_BLOCK_DIAG_LEFT_GENERAL,
	BOOT_BLOCK_DIAG_RIGHT_GENERAL,
	BOOT_UNUSED3_GENERAL,
	BOOT_UNUSED4_GENERAL,
	BOOT_UNUSED5_GENERAL,
	BOOT_UNUSED6_GENERAL,
	BOOT_UNUSED7_GENERAL,
	BOOT_UNUSED8_GENERAL,
	BOOT_UNUSED9_GENERAL,
	BOOT_UNUSED10_GENERAL,
	BOOT_UNUSED11_GENERAL,
	BOOT_UNUSED12_GENERAL,
	BOOT_UNUSED13_GENERAL,
	BOOT_UNUSED14_GENERAL,
	BOOT_UNUSED15_GENERAL,
	BOOT_UNUSED16_GENERAL,
	//

	//# #eol
	MAX_ANIMATIONS_GENERAL,//_15 = FACE_DEAD_15 + 1 + BOOT_ANIMS_GENERAL,
	MAX_TOTALANIMATIONS_GENERAL,//_15
};
 
//for 1.02 demos
typedef enum //# animNumber_e
{
	//=================================================
	//ANIMS IN WHICH UPPER AND LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep BOTH_ DEATHS
	BOTH_DEATH1_DM15 = 0,		//# First Death anim
	BOTH_DEATH2_DM15,			//# Second Death anim
	BOTH_DEATH3_DM15,			//# Third Death anim
	BOTH_DEATH4_DM15,			//# Fourth Death anim
	BOTH_DEATH5_DM15,			//# Fifth Death anim
	BOTH_DEATH6_DM15,			//# Sixth Death anim
	BOTH_DEATH7_DM15,			//# Seventh Death anim
	BOTH_DEATH8_DM15,			//# 
	BOTH_DEATH9_DM15,			//# 
	BOTH_DEATH10_DM15,			//# 
	BOTH_DEATH11_DM15,			//#
	BOTH_DEATH12_DM15,			//# 
	BOTH_DEATH13_DM15,			//# 
	BOTH_DEATH14_DM15,			//# 
	BOTH_DEATH14_UNGRIP_DM15,	//# Desann's end death (cin #35)
	BOTH_DEATH14_SITUP_DM15,		//# Tavion sitting up after having been thrown (cin #23)
	BOTH_DEATH15_DM15,			//# 
	BOTH_DEATH16_DM15,			//# 
	BOTH_DEATH17_DM15,			//# 
	BOTH_DEATH18_DM15,			//# 
	BOTH_DEATH19_DM15,			//# 

	BOTH_DEATHFORWARD1_DM15,		//# First Death in which they get thrown forward
	BOTH_DEATHFORWARD2_DM15,		//# Second Death in which they get thrown forward
	BOTH_DEATHFORWARD3_DM15,		//# Tavion's falling in cin# 23
	BOTH_DEATHBACKWARD1_DM15,	//# First Death in which they get thrown backward
	BOTH_DEATHBACKWARD2_DM15,	//# Second Death in which they get thrown backward

	BOTH_DEATH1IDLE_DM15,		//# Idle while close to death
	BOTH_LYINGDEATH1_DM15,		//# Death to play when killed lying down
	BOTH_STUMBLEDEATH1_DM15,		//# Stumble forward and fall face first death
	BOTH_FALLDEATH1_DM15,		//# Fall forward off a high cliff and splat death - start
	BOTH_FALLDEATH1INAIR_DM15,	//# Fall forward off a high cliff and splat death - loop
	BOTH_FALLDEATH1LAND_DM15,	//# Fall forward off a high cliff and splat death - hit bottom
	BOTH_DEATH_ROLL_DM15,		//# Death anim from a roll
	BOTH_DEATH_FLIP_DM15,		//# Death anim from a flip
	BOTH_DEATH_SPIN_90_R_DM15,	//# Death anim when facing 90 degrees right
	BOTH_DEATH_SPIN_90_L_DM15,	//# Death anim when facing 90 degrees left
	BOTH_DEATH_SPIN_180_DM15,	//# Death anim when facing backwards
	BOTH_DEATH_LYING_UP_DM15,	//# Death anim when lying on back
	BOTH_DEATH_LYING_DN_DM15,	//# Death anim when lying on front
	BOTH_DEATH_FALLING_DN_DM15,	//# Death anim when falling on face
	BOTH_DEATH_FALLING_UP_DM15,	//# Death anim when falling on back
	BOTH_DEATH_CROUCHED_DM15,	//# Death anim when crouched
	//# #sep BOTH_ DEAD POSES # Should be last frame of corresponding previous anims
	BOTH_DEAD1_DM15,				//# First Death finished pose
	BOTH_DEAD2_DM15,				//# Second Death finished pose
	BOTH_DEAD3_DM15,				//# Third Death finished pose
	BOTH_DEAD4_DM15,				//# Fourth Death finished pose
	BOTH_DEAD5_DM15,				//# Fifth Death finished pose
	BOTH_DEAD6_DM15,				//# Sixth Death finished pose
	BOTH_DEAD7_DM15,				//# Seventh Death finished pose
	BOTH_DEAD8_DM15,				//# 
	BOTH_DEAD9_DM15,				//# 
	BOTH_DEAD10_DM15,			//# 
	BOTH_DEAD11_DM15,			//#
	BOTH_DEAD12_DM15,			//# 
	BOTH_DEAD13_DM15,			//# 
	BOTH_DEAD14_DM15,			//# 
	BOTH_DEAD15_DM15,			//# 
	BOTH_DEAD16_DM15,			//# 
	BOTH_DEAD17_DM15,			//# 
	BOTH_DEAD18_DM15,			//# 
	BOTH_DEAD19_DM15,			//# 
	BOTH_DEADFORWARD1_DM15,		//# First thrown forward death finished pose
	BOTH_DEADFORWARD2_DM15,		//# Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_DM15,		//# First thrown backward death finished pose
	BOTH_DEADBACKWARD2_DM15,		//# Second thrown backward death finished pose
	BOTH_LYINGDEAD1_DM15,		//# Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_DM15,		//# Stumble forward death finished pose
	BOTH_FALLDEAD1LAND_DM15,		//# Fall forward and splat death finished pose
	//# #sep BOTH_ DEAD TWITCH/FLOP # React to being shot from death poses
	BOTH_DEADFLOP1_DM15,		//# React to being shot from First Death finished pose
	BOTH_DEADFLOP2_DM15,		//# React to being shot from Second Death finished pose
	BOTH_DEADFLOP3_DM15,		//# React to being shot from Third Death finished pose
	BOTH_DEADFLOP4_DM15,		//# React to being shot from Fourth Death finished pose
	BOTH_DEADFLOP5_DM15,		//# React to being shot from Fifth Death finished pose 
	BOTH_DEADFORWARD1_FLOP_DM15,		//# React to being shot First thrown forward death finished pose
	BOTH_DEADFORWARD2_FLOP_DM15,		//# React to being shot Second thrown forward death finished pose
	BOTH_DEADBACKWARD1_FLOP_DM15,	//# React to being shot First thrown backward death finished pose
	BOTH_DEADBACKWARD2_FLOP_DM15,	//# React to being shot Second thrown backward death finished pose
	BOTH_LYINGDEAD1_FLOP_DM15,		//# React to being shot Killed lying down death finished pose
	BOTH_STUMBLEDEAD1_FLOP_DM15,		//# React to being shot Stumble forward death finished pose
	BOTH_FALLDEAD1_FLOP_DM15,	//# React to being shot Fall forward and splat death finished pose
	BOTH_DISMEMBER_HEAD1_DM15,	//#
	BOTH_DISMEMBER_TORSO1_DM15,	//#
	BOTH_DISMEMBER_LLEG_DM15,	//#
	BOTH_DISMEMBER_RLEG_DM15,	//#
	BOTH_DISMEMBER_RARM_DM15,	//#
	BOTH_DISMEMBER_LARM_DM15,	//#
	//# #sep BOTH_ PAINS
	BOTH_PAIN1_DM15,				//# First take pain anim
	BOTH_PAIN2_DM15,				//# Second take pain anim
	BOTH_PAIN3_DM15,				//# Third take pain anim
	BOTH_PAIN4_DM15,				//# Fourth take pain anim
	BOTH_PAIN5_DM15,				//# Fifth take pain anim - from behind
	BOTH_PAIN6_DM15,				//# Sixth take pain anim - from behind
	BOTH_PAIN7_DM15,				//# Seventh take pain anim - from behind
	BOTH_PAIN8_DM15,				//# Eigth take pain anim - from behind
	BOTH_PAIN9_DM15,				//# 
	BOTH_PAIN10_DM15,			//# 
	BOTH_PAIN11_DM15,			//# 
	BOTH_PAIN12_DM15,			//# 
	BOTH_PAIN13_DM15,			//# 
	BOTH_PAIN14_DM15,			//# 
	BOTH_PAIN15_DM15,			//# 
	BOTH_PAIN16_DM15,			//# 
	BOTH_PAIN17_DM15,			//# 
	BOTH_PAIN18_DM15,			//# 
	BOTH_PAIN19_DM15,			//# 
	BOTH_PAIN20_DM15,			//# GETTING SHOCKED

	//# #sep BOTH_ ATTACKS
	BOTH_ATTACK1_DM15,			//# Attack with stun baton
	BOTH_ATTACK2_DM15,			//# Attack with one-handed pistol
	BOTH_ATTACK3_DM15,			//# Attack with blaster rifle
	BOTH_ATTACK4_DM15,			//# Attack with disruptor
	BOTH_ATTACK5_DM15,			//# Attack with bow caster
	BOTH_ATTACK6_DM15,			//# Attack with ???
	BOTH_ATTACK7_DM15,			//# Attack with ???
	BOTH_ATTACK8_DM15,			//# Attack with ???
	BOTH_ATTACK9_DM15,			//# Attack with rocket launcher
	BOTH_ATTACK10_DM15,			//# Attack with thermal det
	BOTH_ATTACK11_DM15,			//# Attack with laser trap
	BOTH_ATTACK12_DM15,			//# Attack with detpack
	BOTH_MELEE1_DM15,			//# First melee attack
	BOTH_MELEE2_DM15,			//# Second melee attack
	BOTH_MELEE3_DM15,			//# Third melee attack
	BOTH_MELEE4_DM15,			//# Fourth melee attack
	BOTH_MELEE5_DM15,			//# Fifth melee attack
	BOTH_MELEE6_DM15,			//# Sixth melee attack
	BOTH_THERMAL_READY_DM15,		//# pull back with thermal
	BOTH_THERMAL_THROW_DM15,		//# throw thermal
	//* #sep BOTH_ SABER ANIMS
	//Saber attack anims - power level 2
	BOTH_A1_T__B__DM15,	//# Fast weak vertical attack top to bottom
	BOTH_A1__L__R_DM15,	//# Fast weak horizontal attack left to right
	BOTH_A1__R__L_DM15,	//# Fast weak horizontal attack right to left
	BOTH_A1_TL_BR_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BR_TL_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A1_BL_TR_DM15,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A1_TR_BL_DM15,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T1_BR__R_DM15,	//# Fast arc bottom right to right
	BOTH_T1_BR_TL_DM15,	//# Fast weak spin bottom right to top left
	BOTH_T1_BR__L_DM15,	//# Fast weak spin bottom right to left
	BOTH_T1_BR_BL_DM15,	//# Fast weak spin bottom right to bottom left
	BOTH_T1__R_TR_DM15,	//# Fast arc right to top right
	BOTH_T1__R_TL_DM15,	//# Fast arc right to top left
	BOTH_T1__R__L_DM15,	//# Fast weak spin right to left
	BOTH_T1__R_BL_DM15,	//# Fast weak spin right to bottom left
	BOTH_T1_TR_BR_DM15,	//# Fast arc top right to bottom right
	BOTH_T1_TR_TL_DM15,	//# Fast arc top right to top left
	BOTH_T1_TR__L_DM15,	//# Fast arc top right to left
	BOTH_T1_TR_BL_DM15,	//# Fast weak spin top right to bottom left
	BOTH_T1_T__BR_DM15,	//# Fast arc top to bottom right
	BOTH_T1_T___R_DM15,	//# Fast arc top to right
	BOTH_T1_T__TR_DM15,	//# Fast arc top to top right
	BOTH_T1_T__TL_DM15,	//# Fast arc top to top left
	BOTH_T1_T___L_DM15,	//# Fast arc top to left
	BOTH_T1_T__BL_DM15,	//# Fast arc top to bottom left
	BOTH_T1_TL_BR_DM15,	//# Fast weak spin top left to bottom right
	BOTH_T1_TL_BL_DM15,	//# Fast arc top left to bottom left
	BOTH_T1__L_BR_DM15,	//# Fast weak spin left to bottom right
	BOTH_T1__L__R_DM15,	//# Fast weak spin left to right
	BOTH_T1__L_TL_DM15,	//# Fast arc left to top left
	BOTH_T1_BL_BR_DM15,	//# Fast weak spin bottom left to bottom right
	BOTH_T1_BL__R_DM15,	//# Fast weak spin bottom left to right
	BOTH_T1_BL_TR_DM15,	//# Fast weak spin bottom left to top right
	BOTH_T1_BL__L_DM15,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T1_BR_TR_DM15,	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR)
	BOTH_T1_BR_T__DM15,	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR)
	BOTH_T1__R_BR_DM15,	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R)
	BOTH_T1__R_T__DM15,	//# Fast ar right to top				(use: BOTH_T1_T___R)
	BOTH_T1_TR__R_DM15,	//# Fast arc top right to right			(use: BOTH_T1__R_TR)
	BOTH_T1_TR_T__DM15,	//# Fast arc top right to top				(use: BOTH_T1_T__TR)
	BOTH_T1_TL__R_DM15,	//# Fast arc top left to right			(use: BOTH_T1__R_TL)
	BOTH_T1_TL_TR_DM15,	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL)
	BOTH_T1_TL_T__DM15,	//# Fast arc top left to top				(use: BOTH_T1_T__TL)
	BOTH_T1_TL__L_DM15,	//# Fast arc top left to left				(use: BOTH_T1__L_TL)
	BOTH_T1__L_TR_DM15,	//# Fast arc left to top right			(use: BOTH_T1_TR__L)
	BOTH_T1__L_T__DM15,	//# Fast arc left to top				(use: BOTH_T1_T___L)
	BOTH_T1__L_BL_DM15,	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L)
	BOTH_T1_BL_T__DM15,	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL)
	BOTH_T1_BL_TL_DM15,	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S1_S1_T__DM15,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S1_S1__L_DM15,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S1_S1__R_DM15,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S1_S1_TL_DM15,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S1_S1_BR_DM15,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S1_S1_BL_DM15,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S1_S1_TR_DM15,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R1_B__S1_DM15,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R1__L_S1_DM15,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R1__R_S1_DM15,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R1_TL_S1_DM15,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R1_BR_S1_DM15,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R1_BL_S1_DM15,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R1_TR_S1_DM15,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM15, played backwards)
	BOTH_B1_BR____DM15,	//# Bounce-back if attack from BR is blocked
	BOTH_B1__R____DM15,	//# Bounce-back if attack from R is blocked
	BOTH_B1_TR____DM15,	//# Bounce-back if attack from TR is blocked
	BOTH_B1_T_____DM15,	//# Bounce-back if attack from T is blocked
	BOTH_B1_TL____DM15,	//# Bounce-back if attack from TL is blocked
	BOTH_B1__L____DM15,	//# Bounce-back if attack from L is blocked
	BOTH_B1_BL____DM15,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D1_BR____DM15,	//# Deflection toward BR
	BOTH_D1__R____DM15,	//# Deflection toward R
	BOTH_D1_TR____DM15,	//# Deflection toward TR
	BOTH_D1_TL____DM15,	//# Deflection toward TL
	BOTH_D1__L____DM15,	//# Deflection toward L
	BOTH_D1_BL____DM15,	//# Deflection toward BL
	BOTH_D1_B_____DM15,	//# Deflection toward B
	//Saber attack anims - power level 2
	BOTH_A2_T__B__DM15,	//# Fast weak vertical attack top to bottom
	BOTH_A2__L__R_DM15,	//# Fast weak horizontal attack left to right
	BOTH_A2__R__L_DM15,	//# Fast weak horizontal attack right to left
	BOTH_A2_TL_BR_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BR_TL_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A2_BL_TR_DM15,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A2_TR_BL_DM15,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T2_BR__R_DM15,	//# Fast arc bottom right to right
	BOTH_T2_BR_TL_DM15,	//# Fast weak spin bottom right to top left
	BOTH_T2_BR__L_DM15,	//# Fast weak spin bottom right to left
	BOTH_T2_BR_BL_DM15,	//# Fast weak spin bottom right to bottom left
	BOTH_T2__R_TR_DM15,	//# Fast arc right to top right
	BOTH_T2__R_TL_DM15,	//# Fast arc right to top left
	BOTH_T2__R__L_DM15,	//# Fast weak spin right to left
	BOTH_T2__R_BL_DM15,	//# Fast weak spin right to bottom left
	BOTH_T2_TR_BR_DM15,	//# Fast arc top right to bottom right
	BOTH_T2_TR_TL_DM15,	//# Fast arc top right to top left
	BOTH_T2_TR__L_DM15,	//# Fast arc top right to left
	BOTH_T2_TR_BL_DM15,	//# Fast weak spin top right to bottom left
	BOTH_T2_T__BR_DM15,	//# Fast arc top to bottom right
	BOTH_T2_T___R_DM15,	//# Fast arc top to right
	BOTH_T2_T__TR_DM15,	//# Fast arc top to top right
	BOTH_T2_T__TL_DM15,	//# Fast arc top to top left
	BOTH_T2_T___L_DM15,	//# Fast arc top to left
	BOTH_T2_T__BL_DM15,	//# Fast arc top to bottom left
	BOTH_T2_TL_BR_DM15,	//# Fast weak spin top left to bottom right
	BOTH_T2_TL_BL_DM15,	//# Fast arc top left to bottom left
	BOTH_T2__L_BR_DM15,	//# Fast weak spin left to bottom right
	BOTH_T2__L__R_DM15,	//# Fast weak spin left to right
	BOTH_T2__L_TL_DM15,	//# Fast arc left to top left
	BOTH_T2_BL_BR_DM15,	//# Fast weak spin bottom left to bottom right
	BOTH_T2_BL__R_DM15,	//# Fast weak spin bottom left to right
	BOTH_T2_BL_TR_DM15,	//# Fast weak spin bottom left to top right
	BOTH_T2_BL__L_DM15,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T2_BR_TR_DM15,	//# Fast arc bottom right to top right		(use: BOTH_T2_TR_BR)
	BOTH_T2_BR_T__DM15,	//# Fast arc bottom right to top			(use: BOTH_T2_T__BR)
	BOTH_T2__R_BR_DM15,	//# Fast arc right to bottom right			(use: BOTH_T2_BR__R)
	BOTH_T2__R_T__DM15,	//# Fast ar right to top				(use: BOTH_T2_T___R)
	BOTH_T2_TR__R_DM15,	//# Fast arc top right to right			(use: BOTH_T2__R_TR)
	BOTH_T2_TR_T__DM15,	//# Fast arc top right to top				(use: BOTH_T2_T__TR)
	BOTH_T2_TL__R_DM15,	//# Fast arc top left to right			(use: BOTH_T2__R_TL)
	BOTH_T2_TL_TR_DM15,	//# Fast arc top left to top right			(use: BOTH_T2_TR_TL)
	BOTH_T2_TL_T__DM15,	//# Fast arc top left to top				(use: BOTH_T2_T__TL)
	BOTH_T2_TL__L_DM15,	//# Fast arc top left to left				(use: BOTH_T2__L_TL)
	BOTH_T2__L_TR_DM15,	//# Fast arc left to top right			(use: BOTH_T2_TR__L)
	BOTH_T2__L_T__DM15,	//# Fast arc left to top				(use: BOTH_T2_T___L)
	BOTH_T2__L_BL_DM15,	//# Fast arc left to bottom left			(use: BOTH_T2_BL__L)
	BOTH_T2_BL_T__DM15,	//# Fast arc bottom left to top			(use: BOTH_T2_T__BL)
	BOTH_T2_BL_TL_DM15,	//# Fast arc bottom left to top left		(use: BOTH_T2_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S2_S1_T__DM15,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S2_S1__L_DM15,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S2_S1__R_DM15,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S2_S1_TL_DM15,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S2_S1_BR_DM15,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S2_S1_BL_DM15,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S2_S1_TR_DM15,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R2_B__S1_DM15,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R2__L_S1_DM15,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R2__R_S1_DM15,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R2_TL_S1_DM15,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R2_BR_S1_DM15,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R2_BL_S1_DM15,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R2_TR_S1_DM15,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM15, played backwards)
	BOTH_B2_BR____DM15,	//# Bounce-back if attack from BR is blocked
	BOTH_B2__R____DM15,	//# Bounce-back if attack from R is blocked
	BOTH_B2_TR____DM15,	//# Bounce-back if attack from TR is blocked
	BOTH_B2_T_____DM15,	//# Bounce-back if attack from T is blocked
	BOTH_B2_TL____DM15,	//# Bounce-back if attack from TL is blocked
	BOTH_B2__L____DM15,	//# Bounce-back if attack from L is blocked
	BOTH_B2_BL____DM15,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D2_BR____DM15,	//# Deflection toward BR
	BOTH_D2__R____DM15,	//# Deflection toward R
	BOTH_D2_TR____DM15,	//# Deflection toward TR
	BOTH_D2_TL____DM15,	//# Deflection toward TL
	BOTH_D2__L____DM15,	//# Deflection toward L
	BOTH_D2_BL____DM15,	//# Deflection toward BL
	BOTH_D2_B_____DM15,	//# Deflection toward B
	//Saber attack anims - power level 3
	BOTH_A3_T__B__DM15,	//# Fast weak vertical attack top to bottom
	BOTH_A3__L__R_DM15,	//# Fast weak horizontal attack left to right
	BOTH_A3__R__L_DM15,	//# Fast weak horizontal attack right to left
	BOTH_A3_TL_BR_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BR_TL_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A3_BL_TR_DM15,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A3_TR_BL_DM15,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T3_BR__R_DM15,	//# Fast arc bottom right to right
	BOTH_T3_BR_TL_DM15,	//# Fast weak spin bottom right to top left
	BOTH_T3_BR__L_DM15,	//# Fast weak spin bottom right to left
	BOTH_T3_BR_BL_DM15,	//# Fast weak spin bottom right to bottom left
	BOTH_T3__R_TR_DM15,	//# Fast arc right to top right
	BOTH_T3__R_TL_DM15,	//# Fast arc right to top left
	BOTH_T3__R__L_DM15,	//# Fast weak spin right to left
	BOTH_T3__R_BL_DM15,	//# Fast weak spin right to bottom left
	BOTH_T3_TR_BR_DM15,	//# Fast arc top right to bottom right
	BOTH_T3_TR_TL_DM15,	//# Fast arc top right to top left
	BOTH_T3_TR__L_DM15,	//# Fast arc top right to left
	BOTH_T3_TR_BL_DM15,	//# Fast weak spin top right to bottom left
	BOTH_T3_T__BR_DM15,	//# Fast arc top to bottom right
	BOTH_T3_T___R_DM15,	//# Fast arc top to right
	BOTH_T3_T__TR_DM15,	//# Fast arc top to top right
	BOTH_T3_T__TL_DM15,	//# Fast arc top to top left
	BOTH_T3_T___L_DM15,	//# Fast arc top to left
	BOTH_T3_T__BL_DM15,	//# Fast arc top to bottom left
	BOTH_T3_TL_BR_DM15,	//# Fast weak spin top left to bottom right
	BOTH_T3_TL_BL_DM15,	//# Fast arc top left to bottom left
	BOTH_T3__L_BR_DM15,	//# Fast weak spin left to bottom right
	BOTH_T3__L__R_DM15,	//# Fast weak spin left to right
	BOTH_T3__L_TL_DM15,	//# Fast arc left to top left
	BOTH_T3_BL_BR_DM15,	//# Fast weak spin bottom left to bottom right
	BOTH_T3_BL__R_DM15,	//# Fast weak spin bottom left to right
	BOTH_T3_BL_TR_DM15,	//# Fast weak spin bottom left to top right
	BOTH_T3_BL__L_DM15,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T3_BR_TR_DM15,	//# Fast arc bottom right to top right		(use: BOTH_T3_TR_BR)
	BOTH_T3_BR_T__DM15,	//# Fast arc bottom right to top			(use: BOTH_T3_T__BR)
	BOTH_T3__R_BR_DM15,	//# Fast arc right to bottom right			(use: BOTH_T3_BR__R)
	BOTH_T3__R_T__DM15,	//# Fast ar right to top				(use: BOTH_T3_T___R)
	BOTH_T3_TR__R_DM15,	//# Fast arc top right to right			(use: BOTH_T3__R_TR)
	BOTH_T3_TR_T__DM15,	//# Fast arc top right to top				(use: BOTH_T3_T__TR)
	BOTH_T3_TL__R_DM15,	//# Fast arc top left to right			(use: BOTH_T3__R_TL)
	BOTH_T3_TL_TR_DM15,	//# Fast arc top left to top right			(use: BOTH_T3_TR_TL)
	BOTH_T3_TL_T__DM15,	//# Fast arc top left to top				(use: BOTH_T3_T__TL)
	BOTH_T3_TL__L_DM15,	//# Fast arc top left to left				(use: BOTH_T3__L_TL)
	BOTH_T3__L_TR_DM15,	//# Fast arc left to top right			(use: BOTH_T3_TR__L)
	BOTH_T3__L_T__DM15,	//# Fast arc left to top				(use: BOTH_T3_T___L)
	BOTH_T3__L_BL_DM15,	//# Fast arc left to bottom left			(use: BOTH_T3_BL__L)
	BOTH_T3_BL_T__DM15,	//# Fast arc bottom left to top			(use: BOTH_T3_T__BL)
	BOTH_T3_BL_TL_DM15,	//# Fast arc bottom left to top left		(use: BOTH_T3_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S3_S1_T__DM15,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S3_S1__L_DM15,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S3_S1__R_DM15,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S3_S1_TL_DM15,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S3_S1_BR_DM15,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S3_S1_BL_DM15,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S3_S1_TR_DM15,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R3_B__S1_DM15,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R3__L_S1_DM15,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R3__R_S1_DM15,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R3_TL_S1_DM15,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R3_BR_S1_DM15,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R3_BL_S1_DM15,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R3_TR_S1_DM15,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM15, played backwards)
	BOTH_B3_BR____DM15,	//# Bounce-back if attack from BR is blocked
	BOTH_B3__R____DM15,	//# Bounce-back if attack from R is blocked
	BOTH_B3_TR____DM15,	//# Bounce-back if attack from TR is blocked
	BOTH_B3_T_____DM15,	//# Bounce-back if attack from T is blocked
	BOTH_B3_TL____DM15,	//# Bounce-back if attack from TL is blocked
	BOTH_B3__L____DM15,	//# Bounce-back if attack from L is blocked
	BOTH_B3_BL____DM15,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D3_BR____DM15,	//# Deflection toward BR
	BOTH_D3__R____DM15,	//# Deflection toward R
	BOTH_D3_TR____DM15,	//# Deflection toward TR
	BOTH_D3_TL____DM15,	//# Deflection toward TL
	BOTH_D3__L____DM15,	//# Deflection toward L
	BOTH_D3_BL____DM15,	//# Deflection toward BL
	BOTH_D3_B_____DM15,	//# Deflection toward B
	//Saber attack anims - power level 4  - Desann's
	BOTH_A4_T__B__DM15,	//# Fast weak vertical attack top to bottom
	BOTH_A4__L__R_DM15,	//# Fast weak horizontal attack left to right
	BOTH_A4__R__L_DM15,	//# Fast weak horizontal attack right to left
	BOTH_A4_TL_BR_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BR_TL_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A4_BL_TR_DM15,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A4_TR_BL_DM15,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T4_BR__R_DM15,	//# Fast arc bottom right to right
	BOTH_T4_BR_TL_DM15,	//# Fast weak spin bottom right to top left
	BOTH_T4_BR__L_DM15,	//# Fast weak spin bottom right to left
	BOTH_T4_BR_BL_DM15,	//# Fast weak spin bottom right to bottom left
	BOTH_T4__R_TR_DM15,	//# Fast arc right to top right
	BOTH_T4__R_TL_DM15,	//# Fast arc right to top left
	BOTH_T4__R__L_DM15,	//# Fast weak spin right to left
	BOTH_T4__R_BL_DM15,	//# Fast weak spin right to bottom left
	BOTH_T4_TR_BR_DM15,	//# Fast arc top right to bottom right
	BOTH_T4_TR_TL_DM15,	//# Fast arc top right to top left
	BOTH_T4_TR__L_DM15,	//# Fast arc top right to left
	BOTH_T4_TR_BL_DM15,	//# Fast weak spin top right to bottom left
	BOTH_T4_T__BR_DM15,	//# Fast arc top to bottom right
	BOTH_T4_T___R_DM15,	//# Fast arc top to right
	BOTH_T4_T__TR_DM15,	//# Fast arc top to top right
	BOTH_T4_T__TL_DM15,	//# Fast arc top to top left
	BOTH_T4_T___L_DM15,	//# Fast arc top to left
	BOTH_T4_T__BL_DM15,	//# Fast arc top to bottom left
	BOTH_T4_TL_BR_DM15,	//# Fast weak spin top left to bottom right
	BOTH_T4_TL_BL_DM15,	//# Fast arc top left to bottom left
	BOTH_T4__L_BR_DM15,	//# Fast weak spin left to bottom right
	BOTH_T4__L__R_DM15,	//# Fast weak spin left to right
	BOTH_T4__L_TL_DM15,	//# Fast arc left to top left
	BOTH_T4_BL_BR_DM15,	//# Fast weak spin bottom left to bottom right
	BOTH_T4_BL__R_DM15,	//# Fast weak spin bottom left to right
	BOTH_T4_BL_TR_DM15,	//# Fast weak spin bottom left to top right
	BOTH_T4_BL__L_DM15,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T4_BR_TR_DM15,	//# Fast arc bottom right to top right		(use: BOTH_T4_TR_BR)
	BOTH_T4_BR_T__DM15,	//# Fast arc bottom right to top			(use: BOTH_T4_T__BR)
	BOTH_T4__R_BR_DM15,	//# Fast arc right to bottom right			(use: BOTH_T4_BR__R)
	BOTH_T4__R_T__DM15,	//# Fast ar right to top				(use: BOTH_T4_T___R)
	BOTH_T4_TR__R_DM15,	//# Fast arc top right to right			(use: BOTH_T4__R_TR)
	BOTH_T4_TR_T__DM15,	//# Fast arc top right to top				(use: BOTH_T4_T__TR)
	BOTH_T4_TL__R_DM15,	//# Fast arc top left to right			(use: BOTH_T4__R_TL)
	BOTH_T4_TL_TR_DM15,	//# Fast arc top left to top right			(use: BOTH_T4_TR_TL)
	BOTH_T4_TL_T__DM15,	//# Fast arc top left to top				(use: BOTH_T4_T__TL)
	BOTH_T4_TL__L_DM15,	//# Fast arc top left to left				(use: BOTH_T4__L_TL)
	BOTH_T4__L_TR_DM15,	//# Fast arc left to top right			(use: BOTH_T4_TR__L)
	BOTH_T4__L_T__DM15,	//# Fast arc left to top				(use: BOTH_T4_T___L)
	BOTH_T4__L_BL_DM15,	//# Fast arc left to bottom left			(use: BOTH_T4_BL__L)
	BOTH_T4_BL_T__DM15,	//# Fast arc bottom left to top			(use: BOTH_T4_T__BL)
	BOTH_T4_BL_TL_DM15,	//# Fast arc bottom left to top left		(use: BOTH_T4_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S4_S1_T__DM15,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S4_S1__L_DM15,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S4_S1__R_DM15,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S4_S1_TL_DM15,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S4_S1_BR_DM15,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S4_S1_BL_DM15,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S4_S1_TR_DM15,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R4_B__S1_DM15,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R4__L_S1_DM15,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R4__R_S1_DM15,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R4_TL_S1_DM15,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R4_BR_S1_DM15,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R4_BL_S1_DM15,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R4_TR_S1_DM15,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM15, played backwards)
	BOTH_B4_BR____DM15,	//# Bounce-back if attack from BR is blocked
	BOTH_B4__R____DM15,	//# Bounce-back if attack from R is blocked
	BOTH_B4_TR____DM15,	//# Bounce-back if attack from TR is blocked
	BOTH_B4_T_____DM15,	//# Bounce-back if attack from T is blocked
	BOTH_B4_TL____DM15,	//# Bounce-back if attack from TL is blocked
	BOTH_B4__L____DM15,	//# Bounce-back if attack from L is blocked
	BOTH_B4_BL____DM15,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D4_BR____DM15,	//# Deflection toward BR
	BOTH_D4__R____DM15,	//# Deflection toward R
	BOTH_D4_TR____DM15,	//# Deflection toward TR
	BOTH_D4_TL____DM15,	//# Deflection toward TL
	BOTH_D4__L____DM15,	//# Deflection toward L
	BOTH_D4_BL____DM15,	//# Deflection toward BL
	BOTH_D4_B_____DM15,	//# Deflection toward B
	//Saber attack anims - power level 5  - Tavion's
	BOTH_A5_T__B__DM15,	//# Fast weak vertical attack top to bottom
	BOTH_A5__L__R_DM15,	//# Fast weak horizontal attack left to right
	BOTH_A5__R__L_DM15,	//# Fast weak horizontal attack right to left
	BOTH_A5_TL_BR_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BR_TL_DM15,	//# Fast weak diagonal attack top left to botom right
	BOTH_A5_BL_TR_DM15,	//# Fast weak diagonal attack bottom left to top right
	BOTH_A5_TR_BL_DM15,	//# Fast weak diagonal attack bottom left to right
	//Saber Arc and Spin Transitions
	BOTH_T5_BR__R_DM15,	//# Fast arc bottom right to right
	BOTH_T5_BR_TL_DM15,	//# Fast weak spin bottom right to top left
	BOTH_T5_BR__L_DM15,	//# Fast weak spin bottom right to left
	BOTH_T5_BR_BL_DM15,	//# Fast weak spin bottom right to bottom left
	BOTH_T5__R_TR_DM15,	//# Fast arc right to top right
	BOTH_T5__R_TL_DM15,	//# Fast arc right to top left
	BOTH_T5__R__L_DM15,	//# Fast weak spin right to left
	BOTH_T5__R_BL_DM15,	//# Fast weak spin right to bottom left
	BOTH_T5_TR_BR_DM15,	//# Fast arc top right to bottom right
	BOTH_T5_TR_TL_DM15,	//# Fast arc top right to top left
	BOTH_T5_TR__L_DM15,	//# Fast arc top right to left
	BOTH_T5_TR_BL_DM15,	//# Fast weak spin top right to bottom left
	BOTH_T5_T__BR_DM15,	//# Fast arc top to bottom right
	BOTH_T5_T___R_DM15,	//# Fast arc top to right
	BOTH_T5_T__TR_DM15,	//# Fast arc top to top right
	BOTH_T5_T__TL_DM15,	//# Fast arc top to top left
	BOTH_T5_T___L_DM15,	//# Fast arc top to left
	BOTH_T5_T__BL_DM15,	//# Fast arc top to bottom left
	BOTH_T5_TL_BR_DM15,	//# Fast weak spin top left to bottom right
	BOTH_T5_TL_BL_DM15,	//# Fast arc top left to bottom left
	BOTH_T5__L_BR_DM15,	//# Fast weak spin left to bottom right
	BOTH_T5__L__R_DM15,	//# Fast weak spin left to right
	BOTH_T5__L_TL_DM15,	//# Fast arc left to top left
	BOTH_T5_BL_BR_DM15,	//# Fast weak spin bottom left to bottom right
	BOTH_T5_BL__R_DM15,	//# Fast weak spin bottom left to right
	BOTH_T5_BL_TR_DM15,	//# Fast weak spin bottom left to top right
	BOTH_T5_BL__L_DM15,	//# Fast arc bottom left to left
	//Saber Arc Transitions that use existing animations played backwards
	BOTH_T5_BR_TR_DM15,	//# Fast arc bottom right to top right		(use: BOTH_T5_TR_BR)
	BOTH_T5_BR_T__DM15,	//# Fast arc bottom right to top			(use: BOTH_T5_T__BR)
	BOTH_T5__R_BR_DM15,	//# Fast arc right to bottom right			(use: BOTH_T5_BR__R)
	BOTH_T5__R_T__DM15,	//# Fast ar right to top				(use: BOTH_T5_T___R)
	BOTH_T5_TR__R_DM15,	//# Fast arc top right to right			(use: BOTH_T5__R_TR)
	BOTH_T5_TR_T__DM15,	//# Fast arc top right to top				(use: BOTH_T5_T__TR)
	BOTH_T5_TL__R_DM15,	//# Fast arc top left to right			(use: BOTH_T5__R_TL)
	BOTH_T5_TL_TR_DM15,	//# Fast arc top left to top right			(use: BOTH_T5_TR_TL)
	BOTH_T5_TL_T__DM15,	//# Fast arc top left to top				(use: BOTH_T5_T__TL)
	BOTH_T5_TL__L_DM15,	//# Fast arc top left to left				(use: BOTH_T5__L_TL)
	BOTH_T5__L_TR_DM15,	//# Fast arc left to top right			(use: BOTH_T5_TR__L)
	BOTH_T5__L_T__DM15,	//# Fast arc left to top				(use: BOTH_T5_T___L)
	BOTH_T5__L_BL_DM15,	//# Fast arc left to bottom left			(use: BOTH_T5_BL__L)
	BOTH_T5_BL_T__DM15,	//# Fast arc bottom left to top			(use: BOTH_T5_T__BL)
	BOTH_T5_BL_TL_DM15,	//# Fast arc bottom left to top left		(use: BOTH_T5_TL_BL)
	//Saber Attack Start Transitions
	BOTH_S5_S1_T__DM15,	//# Fast plain transition from stance1 to top-to-bottom Fast weak attack
	BOTH_S5_S1__L_DM15,	//# Fast plain transition from stance1 to left-to-right Fast weak attack
	BOTH_S5_S1__R_DM15,	//# Fast plain transition from stance1 to right-to-left Fast weak attack
	BOTH_S5_S1_TL_DM15,	//# Fast plain transition from stance1 to top-left-to-bottom-right Fast weak attack
	BOTH_S5_S1_BR_DM15,	//# Fast plain transition from stance1 to bottom-right-to-top-left Fast weak attack
	BOTH_S5_S1_BL_DM15,	//# Fast plain transition from stance1 to bottom-left-to-top-right Fast weak attack
	BOTH_S5_S1_TR_DM15,	//# Fast plain transition from stance1 to top-right-to-bottom-left Fast weak attack
	//Saber Attack Return Transitions
	BOTH_R5_B__S1_DM15,	//# Fast plain transition from top-to-bottom Fast weak attack to stance1
	BOTH_R5__L_S1_DM15,	//# Fast plain transition from left-to-right Fast weak attack to stance1
	BOTH_R5__R_S1_DM15,	//# Fast plain transition from right-to-left Fast weak attack to stance1
	BOTH_R5_TL_S1_DM15,	//# Fast plain transition from top-left-to-bottom-right Fast weak attack to stance1
	BOTH_R5_BR_S1_DM15,	//# Fast plain transition from bottom-right-to-top-left Fast weak attack to stance1
	BOTH_R5_BL_S1_DM15,	//# Fast plain transition from bottom-left-to-top-right Fast weak attack to stance1
	BOTH_R5_TR_S1_DM15,	//# Fast plain transition from top-right-to-bottom-left Fast weak attack
	//Saber Attack Bounces (first 4 frames of an attack_DM15, played backwards)
	BOTH_B5_BR____DM15,	//# Bounce-back if attack from BR is blocked
	BOTH_B5__R____DM15,	//# Bounce-back if attack from R is blocked
	BOTH_B5_TR____DM15,	//# Bounce-back if attack from TR is blocked
	BOTH_B5_T_____DM15,	//# Bounce-back if attack from T is blocked
	BOTH_B5_TL____DM15,	//# Bounce-back if attack from TL is blocked
	BOTH_B5__L____DM15,	//# Bounce-back if attack from L is blocked
	BOTH_B5_BL____DM15,	//# Bounce-back if attack from BL is blocked
	//Saber Attack Deflections (last 4 frames of an attack)
	BOTH_D5_BR____DM15,	//# Deflection toward BR
	BOTH_D5__R____DM15,	//# Deflection toward R
	BOTH_D5_TR____DM15,	//# Deflection toward TR
	BOTH_D5_TL____DM15,	//# Deflection toward TL
	BOTH_D5__L____DM15,	//# Deflection toward L
	BOTH_D5_BL____DM15,	//# Deflection toward BL
	BOTH_D5_B_____DM15,	//# Deflection toward B
	//Saber parry anims
	BOTH_P1_S1_T__DM15,	//# Block shot/saber top
	BOTH_P1_S1_TR_DM15,	//# Block shot/saber top right
	BOTH_P1_S1_TL_DM15,	//# Block shot/saber top left
	BOTH_P1_S1_BL_DM15,	//# Block shot/saber bottom left
	BOTH_P1_S1_BR_DM15,	//# Block shot/saber bottom right
	//Saber knockaway
	BOTH_K1_S1_T__DM15,	//# knockaway saber top
	BOTH_K1_S1_TR_DM15,	//# knockaway saber top right
	BOTH_K1_S1_TL_DM15,	//# knockaway saber top left
	BOTH_K1_S1_BL_DM15,	//# knockaway saber bottom left
	BOTH_K1_S1_B__DM15,	//# knockaway saber bottom
	BOTH_K1_S1_BR_DM15,	//# knockaway saber bottom right
	//Saber attack knocked away
	BOTH_V1_BR_S1_DM15,	//# BR attack knocked away
	BOTH_V1__R_S1_DM15,	//# R attack knocked away
	BOTH_V1_TR_S1_DM15,	//# TR attack knocked away
	BOTH_V1_T__S1_DM15,	//# T attack knocked away
	BOTH_V1_TL_S1_DM15,	//# TL attack knocked away
	BOTH_V1__L_S1_DM15,	//# L attack knocked away
	BOTH_V1_BL_S1_DM15,	//# BL attack knocked away
	BOTH_V1_B__S1_DM15,	//# B attack knocked away
	//Saber parry broken
	BOTH_H1_S1_T__DM15,	//# saber knocked down from top parry
	BOTH_H1_S1_TR_DM15,	//# saber knocked down-left from TR parry
	BOTH_H1_S1_TL_DM15,	//# saber knocked down-right from TL parry
	BOTH_H1_S1_BL_DM15,	//# saber knocked up-right from BL parry
	BOTH_H1_S1_B__DM15,	//# saber knocked up over head from ready?
	BOTH_H1_S1_BR_DM15,	//# saber knocked up-left from BR parry
	//Sabers locked anims
	BOTH_BF2RETURN_DM15,	//#
	BOTH_BF2BREAK_DM15,	//#
	BOTH_BF2LOCK_DM15,	//#
	BOTH_BF1RETURN_DM15,	//#
	BOTH_BF1BREAK_DM15,	//#
	BOTH_BF1LOCK_DM15,	//#
	BOTH_CWCIRCLE_R2__R_S1_DM15,	//#
	BOTH_CCWCIRCLE_R2__L_S1_DM15,	//#
	BOTH_CWCIRCLE_A2__L__R_DM15,	//#
	BOTH_CCWCIRCLE_A2__R__L_DM15,	//#
	BOTH_CWCIRCLEBREAK_DM15,	//#
	BOTH_CCWCIRCLEBREAK_DM15,	//#
	BOTH_CWCIRCLELOCK_DM15,	//#
	BOTH_CCWCIRCLELOCK_DM15,	//#


	//# #sep BOTH_ STANDING
	BOTH_STAND1_DM15,			//# Standing idle_DM15, no weapon_DM15, hands down
	BOTH_STAND1_RANDOM1_DM15,	//# Random standing idle
	BOTH_STAND1_RANDOM2_DM15,	//# Random standing idle
	BOTH_STAND2_DM15,			//# Standing idle with a weapon
	BOTH_STAND2_RANDOM1_DM15,	//# Random standing idle
	BOTH_STAND2_RANDOM2_DM15,	//# Random standing idle
	BOTH_STAND2_RANDOM3_DM15,	//# Random standing idle
	BOTH_STAND2_RANDOM4_DM15,	//# Random standing idle
	BOTH_STAND3_DM15,			//# Standing hands behind back_DM15, at ease_DM15, etc.
	BOTH_STAND4_DM15,			//# hands clasp behind back
	BOTH_STAND5_DM15,			//# standing idle_DM15, no weapon_DM15, hand down_DM15, back straight
	BOTH_STAND6_DM15,			//# one handed_DM15, gun at side_DM15, relaxed stand
	BOTH_STAND7_DM15,			//# both hands on hips (female)
	BOTH_STAND8_DM15,			//# both hands on hips (male)
	BOTH_STAND1TO3_DM15,			//# Transition from stand1 to stand3
	BOTH_STAND3TO1_DM15,			//# Transition from stand3 to stand1
	BOTH_STAND1TO2_DM15,			//# Transition from stand1 to stand2
	BOTH_STAND2TO1_DM15,			//# Transition from stand2 to stand1
	BOTH_STAND2TO4_DM15,			//# Transition from stand2 to stand4
	BOTH_STAND4TO2_DM15,			//# Transition from stand4 to stand2
	BOTH_STANDTOWALK1_DM15,		//# Transition from stand1 to walk1
	BOTH_STAND4TOATTACK2_DM15,	//# relaxed stand to 1-handed pistol ready
	BOTH_STANDUP1_DM15,			//# standing up and stumbling
	BOTH_STANDUP2_DM15,			//# Luke standing up from his meditation platform (cin # 37)
	BOTH_STAND5TOSIT3_DM15,		//# transition from stand 5 to sit 3
	BOTH_STAND1_REELO_DM15,		//# Reelo in his stand1 position (cin #18)
	BOTH_STAND5_REELO_DM15,		//# Reelo in his stand5 position (cin #18)
	BOTH_STAND1TOSTAND5_DM15,	//# Transition from stand1 to stand5
	BOTH_STAND5TOSTAND1_DM15,	//# Transition from stand5 to stand1
	BOTH_STAND5TOSTAND8_DM15,	//# Transition from stand5 to stand8
	BOTH_STAND8TOSTAND5_DM15,	//# Transition from stand8 to stand5

	BOTH_CONSOLE1START_DM15,		//# typing at a console
	BOTH_CONSOLE1_DM15,			//# typing at a console
	BOTH_CONSOLE1STOP_DM15,		//# typing at a console
	BOTH_CONSOLE2START_DM15,		//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2_DM15,			//# typing at a console with comm link in hand (cin #5) 
	BOTH_CONSOLE2STOP_DM15,		//# typing at a console with comm link in hand (cin #5) 

	BOTH_GUARD_LOOKAROUND1_DM15,	//# Cradling weapon and looking around
	BOTH_GUARD_IDLE1_DM15,		//# Cradling weapon and standing
	BOTH_ALERT1_DM15,			//# Startled by something while on guard
	BOTH_GESTURE1_DM15,			//# Generic gesture_DM15, non-specific
	BOTH_GESTURE2_DM15,			//# Generic gesture_DM15, non-specific
	BOTH_GESTURE3_DM15,			//# Generic gesture_DM15, non-specific
	BOTH_PAUSE1START_DM15,			//# Luke pauses to warn Kyle (cin #24) start
	BOTH_PAUSE1STOP_DM15,			//# Luke pauses to warn Kyle (cin #24) stop

	BOTH_HEADTILTLSTART_DM15,		//# Head tilt to left
	BOTH_HEADTILTLSTOP_DM15,			//# Head tilt to left
	BOTH_HEADTILTRSTART_DM15,		//# Head tilt to right
	BOTH_HEADTILTRSTOP_DM15,			//# Head tilt to right
	BOTH_HEADNOD_DM15,				//# Head shake YES
	BOTH_HEADSHAKE_DM15,				//# Head shake NO
	BOTH_HEADSHAKE1_REELO_DM15,		//# Head shake NO for Reelo
	BOTH_SITHEADTILTLSTART_DM15,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTLSTOP_DM15,		//# Head tilt to left from seated position
	BOTH_SITHEADTILTRSTART_DM15,		//# Head tilt to right from seated position
	BOTH_SITHEADTILTRSTOP_DM15,		//# Head tilt to right from seated position
	BOTH_SITHEADNOD_DM15,			//# Head shake YES from seated position
	BOTH_SITHEADSHAKE_DM15,			//# Head shake NO from seated position
 
	BOTH_REACH1START_DM15,		//# Monmothma reaching for crystal
	BOTH_REACH1STOP_DM15,		//# Monmothma reaching for crystal

	BOTH_EXAMINE1START_DM15,		//# Start Mon Mothma examining crystal 
	BOTH_EXAMINE1_DM15,			//# Mon Mothma examining crystal 
	BOTH_EXAMINE1STOP_DM15,		//# Stop Mon Mothma examining crystal 
	BOTH_EXAMINE2START_DM15,	//# Start Kyle tossing crystal
	BOTH_EXAMINE2_DM15,			//# Hold Kyle tossing crystal
	BOTH_EXAMINE2STOP_DM15,		//# End Kyle tossing crystal
	BOTH_EXAMINE3START_DM15,	//# Start Lando looking around corner
	BOTH_EXAMINE3_DM15,			//# Hold Lando looking around corner
	BOTH_EXAMINE3STOP_DM15,		//# End Lando looking around corner

	BOTH_THROW1START_DM15,		//# Kyle thrown to the right
	BOTH_THROW1_DM15,			//# Kyle thrown to the right
	BOTH_THROW1STOP_DM15,		//# Kyle thrown to the right
	BOTH_THROW2START_DM15,		//# Kyle thrown to the left
	BOTH_THROW2_DM15,			//# Kyle thrown to the left
	BOTH_THROW3_DM15,			//# Kyle thrown backwards in cin #9

	BOTH_LEANLEFT2START_DM15,	//# Start leaning left in chair
	BOTH_LEANLEFT2STOP_DM15,		//# Stop leaning left in chair
	BOTH_LEANRIGHT3START_DM15,	//# Start Lando leaning on wall
	BOTH_LEANRIGHT3_DM15,			//# Lando leaning on wall
	BOTH_LEANRIGHT3STOP_DM15,		//# Stop Lando leaning on wall

	BOTH_FORCEFOUNTAIN1_START_DM15,	//# Kyle being lifted into the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_MIDDLE_DM15,	//# Kyle changing to looping position in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_LOOP_DM15,	//# Kyle being spun in the Force Fountain (cin #10)
	BOTH_FORCEFOUNTAIN1_STOP_DM15,	//# Kyle being set down out of the Force Fountain (cin #10)
	BOTH_THUMBING1_DM15,				//# Lando gesturing with thumb over his shoulder (cin #19)
	BOTH_COME_ON1_DM15,				//# Jan gesturing to Kyle (cin #32a)
	BOTH_STEADYSELF1_DM15,			//# Jan trying to keep footing (cin #32a)
	BOTH_STEADYSELF1END_DM15,		//# Return hands to side from STEADSELF1 Kyle (cin#5)
	BOTH_SILENCEGESTURE1_DM15,		//# Luke silencing Kyle with a raised hand (cin #37)
	BOTH_REACHFORSABER1_DM15,		//# Luke holding hand out for Kyle's saber (cin #37)
	BOTH_PUNCHER1_DM15,				//# Jan punching Kyle in the shoulder (cin #37)
	BOTH_CONSTRAINER1HOLD_DM15,		//# Static pose of starting Tavion constraining Jan (cin #9)
	BOTH_CONSTRAINEE1HOLD_DM15,		//# Static pose of starting Jan being constrained by Tavion (cin #9)
	BOTH_CONSTRAINER1STAND_DM15,		//# Tavion constraining Jan in a stand pose (cin #9)
	BOTH_CONSTRAINEE1STAND_DM15,		//# Jan being constrained in a stand pose (cin #9)
	BOTH_CONSTRAINER1WALK_DM15,		//# Tavion shoving jan forward (cin #9)
	BOTH_CONSTRAINEE1WALK_DM15,		//# Jan being shoved forward by Tavion (cin #9)
	BOTH_CONSTRAINER1LOOP_DM15,		//# Tavion walking with Jan in a loop (cin #9)
	BOTH_CONSTRAINEE1LOOP_DM15,		//# Jan walking with Tavion in a loop (cin #9)
	BOTH_SABERKILLER1_DM15,			//# Tavion about to strike Jan with saber (cin #9)
	BOTH_SABERKILLEE1_DM15,			//# Jan about to be struck by Tavion with saber (cin #9)
	BOTH_HANDSHAKER1START_DM15,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKER1LOOP_DM15,		//# Luke shaking Kyle's hand (cin #37)
	BOTH_HANDSHAKEE1START_DM15,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_HANDSHAKEE1LOOP_DM15,		//# Kyle shaking Luke's hand (cin #37)
	BOTH_LAUGH1START_DM15,			//# Reelo leaning forward before laughing (cin #18)
	BOTH_LAUGH1STOP_DM15,			//# Reelo laughing (cin #18)
	BOTH_ESCAPEPOD_LEAVE1_DM15,	//# Kyle leaving escape pod (cin #33)
	BOTH_ESCAPEPOD_LEAVE2_DM15,	//# Jan leaving escape pod (cin #33)
	BOTH_BARTENDER_IDLE1_DM15,	//# Bartender idle in cin #15
	BOTH_BARTENDER_THROW1_DM15,	//# Bartender throws glass in cin #15
	BOTH_BARTENDER_COWERSTART_DM15,	//# Start of Bartender raising both hands up in surrender (cin #16)
	BOTH_BARTENDER_COWERLOOP_DM15,	//# Loop of Bartender waving both hands in surrender (cin #16)
	BOTH_BARTENDER_COWER_DM15,		//# Single frame of Bartender waving both hands in surrender (cin #16)
	BOTH_THREATEN1_START_DM15,	//# First frame of Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_THREATEN1_DM15,			//# Kyle threatening Bartender with lightsaber (cin #16)
	BOTH_RADIO_ONOFF_DM15,		//# Mech Galak turning on his suit radio (cin #32)
	BOTH_TRIUMPHANT1START_DM15,	//# Mech Galak raising his arms in victory (cin #32)
	BOTH_TRIUMPHANT1STOP_DM15,	//# Mech Galak lowering his arms in victory (cin #32)

	BOTH_SABERTHROW1START_DM15,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW1STOP_DM15,		//# Desann throwing his light saber (cin #26)
	BOTH_SABERTHROW2START_DM15,		//# Kyle throwing his light saber (cin #32)
	BOTH_SABERTHROW2STOP_DM15,		//# Kyle throwing his light saber (cin #32)

	BOTH_COVERUP1_LOOP_DM15,		//# animation of getting in line of friendly fire
	BOTH_COVERUP1_START_DM15,	//# transitions from stand to coverup1_loop
	BOTH_COVERUP1_END_DM15,		//# transitions from coverup1_loop to stand

	BOTH_INJURED4_DM15,			//# Injured pose 4
	BOTH_INJURED4TO5_DM15,		//# Transition from INJURED4 to INJURED5
	BOTH_INJURED5_DM15,			//# Injured pose 5

	//# #sep BOTH_ SITTING/CROUCHING
	BOTH_SIT1STAND_DM15,			//# Stand up from First sitting anim
	BOTH_SIT1_DM15,				//# Normal chair sit.
	BOTH_SIT2_DM15,				//# Lotus position.
	BOTH_SIT3_DM15,				//# Sitting in tired position_DM15, elbows on knees

	BOTH_SIT2TO3_DM15,			//# Trans from sit2 to sit3?
	BOTH_SIT2TOSTAND5_DM15,		//# Transition from sit 2 to stand 5
	BOTH_SIT3TO1_DM15,			//# Trans from sit3 to sit1?
	BOTH_SIT3TO2_DM15,			//# Trans from sit3 to sit2?
	BOTH_SIT3TOSTAND5_DM15,		//# transition from sit 3 to stand 5

	BOTH_SIT4TO5_DM15,			//# Trans from sit4 to sit5
	BOTH_SIT4TO6_DM15,			//# Trans from sit4 to sit6
	BOTH_SIT5TO4_DM15,			//# Trans from sit5 to sit4
	BOTH_SIT5TO6_DM15,			//# Trans from sit5 to sit6
	BOTH_SIT6TO4_DM15,			//# Trans from sit6 to sit4
	BOTH_SIT6TO5_DM15,			//# Trans from sit6 to sit5
	BOTH_SIT7_DM15,				//# sitting with arms over knees_DM15, no weapon
	BOTH_SIT7TOSTAND1_DM15,		//# getting up from sit7 into stand1

	BOTH_CROUCH1_DM15,			//# Transition from standing to crouch
	BOTH_CROUCH1IDLE_DM15,		//# Crouching idle
	BOTH_CROUCH1WALK_DM15,		//# Walking while crouched
	BOTH_CROUCH1WALKBACK_DM15,	//# Walking while crouched
	BOTH_UNCROUCH1_DM15,			//# Transition from crouch to standing
	BOTH_CROUCH2IDLE_DM15,		//# crouch and resting on back righ heel_DM15, no weapon
	BOTH_CROUCH2TOSTAND1_DM15,	//# going from crouch2 to stand1
	BOTH_CROUCH3_DM15,			//# Desann crouching down to Kyle (cin 9)
	BOTH_UNCROUCH3_DM15,			//# Desann uncrouching down to Kyle (cin 9)
	BOTH_GET_UP1_DM15,			//# Get up from the ground_DM15, face down
	BOTH_GET_UP2_DM15,			//# Get up from the ground_DM15, face up

	BOTH_COCKPIT_CONSOLE1_DM15,		//# work console1 while sitting in a cockpit.
	BOTH_COCKPIT_CONSOLE2_DM15,		//# work console2 while sitting in a cockpit.
	BOTH_COCKPIT_SIT_DM15,			//# sit in a cockpit.

	BOTH_GUNSIT1_DM15,			//# sitting on an emplaced gun.

	BOTH_KNEES1_DM15,			//# Tavion on her knees
	BOTH_KNEES2_DM15,			//# Tavion on her knees looking down
	BOTH_KNEES2TO1_DM15,			//# Transition of KNEES2 to KNEES1
	BOTH_STRUGGLE1START_DM15,	//# Kyle struggling under crate
	BOTH_STRUGGLE1_DM15,			//# Kyle struggling under crate
	BOTH_STRUGGLE1STOP_DM15,		//# Kyle struggling under crate
	BOTH_RUMMAGE1START_DM15,	//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1_DM15,			//# Kyle rummaging for crystal (cin 2)
	BOTH_RUMMAGE1STOP_DM15,		//# Kyle rummaging for crystal (cin 2)
	BOTH_HOLDGLASS1_DM15,		//# Bartender holds glass (cin# 14)
	BOTH_SLIDEGLASS1_DM15,		//# Bartender slides glass (cin# 14)
	BOTH_SLAMSABERDOWN_DM15,		//# Kyle slamming his saber on the bar top (cin# 14)

	//# #sep BOTH_ MOVING
	BOTH_WALK1_DM15,				//# Normal walk
	BOTH_WALK2_DM15,				//# Normal walk
	BOTH_WALK3_DM15,				//# Goes with stand3
	BOTH_WALK4_DM15,				//# Walk cycle goes to a stand4
	BOTH_WALK5_DM15,				//# Tavion taunting Kyle (cin 22)
	BOTH_WALK6_DM15,				//# Slow walk for Luke (cin 12)
	BOTH_WALK7_DM15,				//# Fast walk
	BOTH_WALKTORUN1_DM15,		//# transition from walk to run
	BOTH_RUN1_DM15,				//# Full run
	BOTH_RUN1START_DM15,			//# Start into full run1
	BOTH_RUN1STOP_DM15,			//# Stop from full run1
	BOTH_RUN2_DM15,				//# Full run
	BOTH_RUNINJURED1_DM15,		//# Run with injured left leg
	BOTH_STRAFE_LEFT1_DM15,		//# Sidestep left_DM15, should loop
	BOTH_STRAFE_RIGHT1_DM15,		//# Sidestep right_DM15, should loop
	BOTH_RUNSTRAFE_LEFT1_DM15,	//# Sidestep left_DM15, should loop
	BOTH_RUNSTRAFE_RIGHT1_DM15,	//# Sidestep right_DM15, should loop
	BOTH_TURN_LEFT1_DM15,		//# Turn left_DM15, should loop
	BOTH_TURN_RIGHT1_DM15,		//# Turn right_DM15, should loop
	BOTH_TURNSTAND2_DM15,		//# Turn from STAND2 position
	BOTH_TURNSTAND3_DM15,		//# Turn from STAND3 position
	BOTH_TURNSTAND4_DM15,		//# Turn from STAND4 position
	BOTH_TURNSTAND5_DM15,		//# Turn from STAND5 position
	BOTH_RUNAWAY1_DM15,			//# Running scared
	BOTH_SWIM1_DM15,				//# Swimming

	BOTH_WALKBACK1_DM15,			//# Walk1 backwards
	BOTH_WALKBACK2_DM15,			//# Walk2 backwards
	BOTH_RUNBACK1_DM15,			//# Run1 backwards
	BOTH_RUNBACK2_DM15,			//# Run1 backwards

	//# #sep BOTH_ JUMPING
	BOTH_JUMP1_DM15,				//# Jump - wind-up and leave ground
	BOTH_INAIR1_DM15,			//# In air loop (from jump)
	BOTH_LAND1_DM15,				//# Landing (from in air loop)
	BOTH_LAND2_DM15,				//# Landing Hard (from a great height)

	BOTH_JUMPBACK1_DM15,			//# Jump backwards - wind-up and leave ground
	BOTH_INAIRBACK1_DM15,		//# In air loop (from jump back)
	BOTH_LANDBACK1_DM15,			//# Landing backwards(from in air loop)

	BOTH_JUMPLEFT1_DM15,			//# Jump left - wind-up and leave ground
	BOTH_INAIRLEFT1_DM15,		//# In air loop (from jump left)
	BOTH_LANDLEFT1_DM15,			//# Landing left(from in air loop)

	BOTH_JUMPRIGHT1_DM15,		//# Jump right - wind-up and leave ground
	BOTH_INAIRRIGHT1_DM15,		//# In air loop (from jump right)
	BOTH_LANDRIGHT1_DM15,		//# Landing right(from in air loop)

	BOTH_FORCEJUMP1_DM15,		//# Jump - wind-up and leave ground
	BOTH_FORCEINAIR1_DM15,		//# In air loop (from jump)
	BOTH_FORCELAND1_DM15,		//# Landing (from in air loop)

	BOTH_FORCEJUMPBACK1_DM15,	//# Jump backwards - wind-up and leave ground
	BOTH_FORCEINAIRBACK1_DM15,	//# In air loop (from jump back)
	BOTH_FORCELANDBACK1_DM15,	//# Landing backwards(from in air loop)

	BOTH_FORCEJUMPLEFT1_DM15,	//# Jump left - wind-up and leave ground
	BOTH_FORCEINAIRLEFT1_DM15,	//# In air loop (from jump left)
	BOTH_FORCELANDLEFT1_DM15,	//# Landing left(from in air loop)

	BOTH_FORCEJUMPRIGHT1_DM15,	//# Jump right - wind-up and leave ground
	BOTH_FORCEINAIRRIGHT1_DM15,	//# In air loop (from jump right)
	BOTH_FORCELANDRIGHT1_DM15,	//# Landing right(from in air loop)
	//# #sep BOTH_ ACROBATICS
	BOTH_FLIP_F_DM15,			//# Flip forward
	BOTH_FLIP_B_DM15,			//# Flip backwards
	BOTH_FLIP_L_DM15,			//# Flip left
	BOTH_FLIP_R_DM15,			//# Flip right

	BOTH_ROLL_F_DM15,			//# Roll forward
	BOTH_ROLL_B_DM15,			//# Roll backward
	BOTH_ROLL_L_DM15,			//# Roll left
	BOTH_ROLL_R_DM15,			//# Roll right
	BOTH_ROLL_FR_DM15,			//# Roll forward right
	BOTH_ROLL_FL_DM15,			//# Roll forward left
	BOTH_ROLL_BR_DM15,			//# Roll back right
	BOTH_ROLL_BL_DM15,			//# Roll back left

	BOTH_HOP_F_DM15,				//# quickstep forward
	BOTH_HOP_B_DM15,				//# quickstep backwards
	BOTH_HOP_L_DM15,				//# quickstep left
	BOTH_HOP_R_DM15,				//# quickstep right

	BOTH_DODGE_FL_DM15,			//# lean-dodge forward left
	BOTH_DODGE_FR_DM15,			//# lean-dodge forward right
	BOTH_DODGE_BL_DM15,			//# lean-dodge backwards left
	BOTH_DODGE_BR_DM15,			//# lean-dodge backwards right
	BOTH_DODGE_L_DM15,			//# lean-dodge left
	BOTH_DODGE_R_DM15,			//# lean-dodge right

	BOTH_DIVE1_DM15,				//# Dive!

	BOTH_SABERFAST_STANCE_DM15,
	BOTH_SABERSLOW_STANCE_DM15,
	BOTH_ENGAGETAUNT_DM15,
	BOTH_A2_STABBACK1_DM15,		//# Stab saber backward
	BOTH_ATTACK_BACK_DM15,		//# Swing around backwards and attack
	BOTH_JUMPFLIPSLASHDOWN1_DM15,//#
	BOTH_JUMPFLIPSTABDOWN_DM15,//#
	BOTH_FORCELEAP2_T__B__DM15,//#
	BOTH_LUNGE2_B__T__DM15,//#
	BOTH_CROUCHATTACKBACK1_DM15,//#
	BOTH_ARIAL_LEFT_DM15,		//# 
	BOTH_ARIAL_RIGHT_DM15,		//# 
	BOTH_CARTWHEEL_LEFT_DM15,	//# 
	BOTH_CARTWHEEL_RIGHT_DM15,	//# 
	BOTH_FLIP_LEFT_DM15,			//# 
	BOTH_FLIP_BACK1_DM15,		//# 
	BOTH_FLIP_BACK2_DM15,		//# 
	BOTH_FLIP_BACK3_DM15,		//# 
	BOTH_BUTTERFLY_LEFT_DM15,	//# 
	BOTH_BUTTERFLY_RIGHT_DM15,	//# 
	BOTH_WALL_RUN_RIGHT_DM15,	//# 
	BOTH_WALL_RUN_RIGHT_FLIP_DM15,//#
	BOTH_WALL_RUN_RIGHT_STOP_DM15,//# 
	BOTH_WALL_RUN_LEFT_DM15,		//# 
	BOTH_WALL_RUN_LEFT_FLIP_DM15,//#
	BOTH_WALL_RUN_LEFT_STOP_DM15,//# 
	BOTH_WALL_FLIP_RIGHT_DM15,	//# 
	BOTH_WALL_FLIP_LEFT_DM15,	//# 
	BOTH_WALL_FLIP_FWD_DM15,		//#
	BOTH_KNOCKDOWN1_DM15,		//# knocked backwards
	BOTH_KNOCKDOWN2_DM15,		//# knocked backwards hard
	BOTH_KNOCKDOWN3_DM15,		//#	knocked forwards
	BOTH_KNOCKDOWN4_DM15,		//# knocked backwards from crouch
	BOTH_KNOCKDOWN5_DM15,		//# dupe of 3 - will be removed
	BOTH_GETUP1_DM15,			//#
	BOTH_GETUP2_DM15,			//#
	BOTH_GETUP3_DM15,			//#
	BOTH_GETUP4_DM15,			//#
	BOTH_GETUP5_DM15,			//#
	BOTH_GETUP_CROUCH_F1_DM15,	//#
	BOTH_GETUP_CROUCH_B1_DM15,	//#
	BOTH_FORCE_GETUP_F1_DM15,	//#
	BOTH_FORCE_GETUP_F2_DM15,	//#
	BOTH_FORCE_GETUP_B1_DM15,	//#
	BOTH_FORCE_GETUP_B2_DM15,	//#
	BOTH_FORCE_GETUP_B3_DM15,	//#
	BOTH_FORCE_GETUP_B4_DM15,	//#
	BOTH_FORCE_GETUP_B5_DM15,	//#
	BOTH_FORCE_GETUP_B6_DM15,	//#
	BOTH_WALL_FLIP_BACK1_DM15,	//#
	BOTH_WALL_FLIP_BACK2_DM15,	//#
	BOTH_SPIN1_DM15,				//#
	BOTH_CEILING_CLING_DM15,		//# clinging to ceiling
	BOTH_CEILING_DROP_DM15,		//# dropping from ceiling cling

	//TESTING
	BOTH_FJSS_TR_BL_DM15,		//# jump spin slash tr to bl
	BOTH_FJSS_TL_BR_DM15,		//# jump spin slash bl to tr
	BOTH_DEATHFROMBACKSLASH_DM15,//#
	BOTH_RIGHTHANDCHOPPEDOFF_DM15,//#
	BOTH_DEFLECTSLASH__R__L_FIN_DM15,//#
	BOTH_BASHED1_DM15,//#
	BOTH_ARIAL_F1_DM15,//#
	BOTH_BUTTERFLY_FR1_DM15,//#
	BOTH_BUTTERFLY_FL1_DM15,//#
	BOTH_POSE1_DM15,//#
	BOTH_POSE2_DM15,//#
	BOTH_POSE3_DM15,//#
	BOTH_POSE4_DM15,//#

	//# #sep BOTH_ MISC MOVEMENT
	BOTH_HIT1_DM15,				//# Kyle hit by crate in cin #9
	BOTH_LADDER_UP1_DM15,		//# Climbing up a ladder with rungs at 16 unit intervals
	BOTH_LADDER_DWN1_DM15,		//# Climbing down a ladder with rungs at 16 unit intervals
	BOTH_LADDER_IDLE_DM15,		//#	Just sitting on the ladder
	BOTH_ONLADDER_BOT1_DM15,		//# Getting on the ladder at the bottom
	BOTH_OFFLADDER_BOT1_DM15,	//# Getting off the ladder at the bottom
	BOTH_ONLADDER_TOP1_DM15,		//# Getting on the ladder at the top
	BOTH_OFFLADDER_TOP1_DM15,	//# Getting off the ladder at the top
	BOTH_LIFT1_DM15,				//# Lifting someone/thing over their shoulder
	BOTH_STEP1_DM15,				//# telsia checking out lake cinematic9.2
	BOTH_HITWALL1_DM15,			//# cin.18_DM15, Kenn hit by borg into wall 56 units away
	BOTH_AMBUSHLAND1_DM15,		//# landing from fall on victim
	BOTH_BIRTH1_DM15,			//# birth from jumping through walls

	//# #sep BOTH_ FLYING IDLE
	BOTH_FLY_IDLE1_DM15,			//# Flying Idle 1
	BOTH_FLY_IDLE2_DM15,			//# Flying Idle 2
	BOTH_FLY_SHIELDED_DM15,		//# For sentry droid_DM15, shields in


	//# #sep BOTH_ FLYING MOVING
	BOTH_FLY_START1_DM15,		//# Start flying
	BOTH_FLY_STOP1_DM15,			//# Stop flying
	BOTH_FLY_LOOP1_DM15,			//# Normal flying_DM15, should loop
	BOTH_FLOAT1_DM15,			//# Crew floating through space 1
	BOTH_FLOAT2_DM15,			//# Crew floating through space 2
	BOTH_FLOATCONSOLE1_DM15,		//# Crew floating and working on console

	//# #sep BOTH_ SWIMMING
	BOTH_SWIM_IDLE1_DM15,		//# Swimming Idle 1
	BOTH_SWIMFORWARDSTART_DM15,	//# Swim forward start
	BOTH_SWIMFORWARD_DM15,		//# Swim forward loop
	BOTH_SWIMFORWARDSTOP_DM15,	//# Swim forward end
	BOTH_SWIMBACKWARDSTART_DM15,	//# Swim backward start
	BOTH_SWIMBACKWARD_DM15,		//# Swim backward loop
	BOTH_SWIMBACKWARDSTOP_DM15,	//# Swim backward end

	//# #sep BOTH_ LYING
	BOTH_LIE_DOWN1_DM15,			//# From a stand position_DM15, get down on ground_DM15, face down
	BOTH_LIE_DOWN2_DM15,			//# From a stand position_DM15, get down on ground_DM15, face up
	BOTH_LIE_DOWN3_DM15,			//# reaction to local disnode being destroyed
	BOTH_PAIN2WRITHE1_DM15,		//# Transition from upright position to writhing on ground anim
	BOTH_PRONE2RLEG_DM15,		//# Lying on ground reach to grab right leg
	BOTH_PRONE2LLEG_DM15,		//# Lying on ground reach to grab left leg
	BOTH_WRITHING1_DM15,			//# Lying on ground on back writhing in pain
	BOTH_WRITHING1RLEG_DM15,		//# Lying on ground writhing in pain_DM15, holding right leg
	BOTH_WRITHING1LLEG_DM15,		//# Lying on ground writhing in pain_DM15, holding left leg
	BOTH_WRITHING2_DM15,			//# Lying on ground on front writhing in pain
	BOTH_INJURED1_DM15,			//# Lying down_DM15, against wall - can also be sleeping against wall
	BOTH_INJURED2_DM15,			//# Injured pose 2
	BOTH_INJURED3_DM15,			//# Injured pose 3
	BOTH_INJURED6_DM15,			//# Injured pose 6
	BOTH_INJURED6ATTACKSTART_DM15,	//# Start attack while in injured 6 pose 
	BOTH_INJURED6ATTACKSTOP_DM15,	//# End attack while in injured 6 pose
	BOTH_INJURED6COMBADGE_DM15,	//# Hit combadge while in injured 6 pose
	BOTH_INJURED6POINT_DM15,		//# Chang points to door while in injured state
	BOTH_INJUREDTOSTAND1_DM15,	//# Runinjured to stand1

	BOTH_PROPUP1_DM15,			//# Kyle getting up from having been knocked down (cin #9 end)
	BOTH_CRAWLBACK1_DM15,		//# Lying on back_DM15, crawling backwards with elbows
	BOTH_SITWALL1_DM15,			//# Sitting against a wall
	BOTH_RESTRAINED1_DM15,		//# Telsia tied to medical table
	BOTH_RESTRAINED1POINT_DM15,	//# Telsia tied to medical table pointing at Munro
	BOTH_LIFTED1_DM15,			//# Fits with BOTH_LIFT1_DM15, lifted on shoulder
	BOTH_CARRIED1_DM15,			//# Fits with TORSO_CARRY1_DM15, carried over shoulder
	BOTH_CARRIED2_DM15,			//# Laying over object

	BOTH_CHOKE1START_DM15,		//# tavion in force grip choke
	BOTH_CHOKE1STARTHOLD_DM15,	//# loop of tavion in force grip choke
	BOTH_CHOKE1_DM15,			//# tavion in force grip choke

	BOTH_CHOKE2_DM15,			//# tavion recovering from force grip choke
	BOTH_CHOKE3_DM15,			//# left-handed choke (for people still holding a weapon)

	//# #sep BOTH_ HUNTER-SEEKER BOT-SPECIFIC
	BOTH_POWERUP1_DM15,			//# Wakes up

	BOTH_TURNON_DM15,			//# Protocol Droid wakes up
	BOTH_TURNOFF_DM15,			//# Protocol Droid shuts off

	BOTH_BUTTON1_DM15,			//# Single button push with right hand
	BOTH_BUTTON2_DM15,			//# Single button push with left finger
	BOTH_BUTTON_HOLD_DM15,		//# Single button hold with left hand
	BOTH_BUTTON_RELEASE_DM15,	//# Single button release with left hand

	//# JEDI-SPECIFIC
	BOTH_RESISTPUSH_DM15,		//# plant yourself to resist force push/pulls.
	BOTH_FORCEPUSH_DM15,			//# Use off-hand to do force power.
	BOTH_FORCEPULL_DM15,			//# Use off-hand to do force power.
	BOTH_MINDTRICK1_DM15,			//# Use off-hand to do mind trick
	BOTH_MINDTRICK2_DM15,			//# Use off-hand to do distraction
	BOTH_FORCELIGHTNING_DM15,		//# Use off-hand to do lightning
	BOTH_FORCELIGHTNING_HOLD_DM15,	//# Use off-hand to do lightning - hold
	BOTH_FORCELIGHTNING_RELEASE_DM15,//# Use off-hand to do lightning - release
	BOTH_FORCEHEAL_START_DM15,		//# Healing meditation pose start
	BOTH_FORCEHEAL_STOP_DM15,		//# Healing meditation pose end
	BOTH_FORCEHEAL_QUICK_DM15,		//# Healing meditation gesture
	BOTH_SABERPULL_DM15,			//# Use off-hand to do force power.
	BOTH_FORCEGRIP1_DM15,		//# force-gripping (no anim?)
	BOTH_FORCEGRIP2_DM15,		//# force-gripping (?)
	BOTH_FORCEGRIP3_DM15,		//# force-gripping (right hand)
	BOTH_FORCEGRIP_HOLD_DM15,	//# Use off-hand to do grip - hold
	BOTH_FORCEGRIP_RELEASE_DM15,//# Use off-hand to do grip - release
	BOTH_TOSS1_DM15,				//# throwing to left after force gripping
	BOTH_TOSS2_DM15,				//# throwing to right after force gripping

	//=================================================
	//ANIMS IN WHICH ONLY THE UPPER OBJECTS ARE IN MD3
	//=================================================
	//# #sep TORSO_ WEAPON-RELATED
	TORSO_DROPWEAP1_DM15,		//# Put weapon away
	TORSO_DROPWEAP2_DM15,		//# Put weapon away
	TORSO_DROPWEAP3_DM15,		//# Put weapon away
	TORSO_DROPWEAP4_DM15,		//# Put weapon away
	TORSO_RAISEWEAP1_DM15,		//# Draw Weapon
	TORSO_RAISEWEAP2_DM15,		//# Draw Weapon
	TORSO_RAISEWEAP3_DM15,		//# Draw Weapon
	TORSO_RAISEWEAP4_DM15,		//# Draw Weapon
	TORSO_WEAPONREADY1_DM15,		//# Ready to fire stun baton
	TORSO_WEAPONREADY2_DM15,		//# Ready to fire one-handed blaster pistol
	TORSO_WEAPONREADY3_DM15,		//# Ready to fire blaster rifle
	TORSO_WEAPONREADY4_DM15,		//# Ready to fire sniper rifle
	TORSO_WEAPONREADY5_DM15,		//# Ready to fire bowcaster
	TORSO_WEAPONREADY6_DM15,		//# Ready to fire ???
	TORSO_WEAPONREADY7_DM15,		//# Ready to fire ???
	TORSO_WEAPONREADY8_DM15,		//# Ready to fire ???
	TORSO_WEAPONREADY9_DM15,		//# Ready to fire rocket launcher
	TORSO_WEAPONREADY10_DM15,	//# Ready to fire thermal det
	TORSO_WEAPONREADY11_DM15,	//# Ready to fire laser trap
	TORSO_WEAPONREADY12_DM15,	//# Ready to fire detpack
	TORSO_WEAPONIDLE1_DM15,		//# Holding stun baton
	TORSO_WEAPONIDLE2_DM15,		//# Holding one-handed blaster
	TORSO_WEAPONIDLE3_DM15,		//# Holding blaster rifle
	TORSO_WEAPONIDLE4_DM15,		//# Holding sniper rifle
	TORSO_WEAPONIDLE5_DM15,		//# Holding bowcaster
	TORSO_WEAPONIDLE6_DM15,		//# Holding ???
	TORSO_WEAPONIDLE7_DM15,		//# Holding ???
	TORSO_WEAPONIDLE8_DM15,		//# Holding ???
	TORSO_WEAPONIDLE9_DM15,		//# Holding rocket launcher
	TORSO_WEAPONIDLE10_DM15,		//# Holding thermal det
	TORSO_WEAPONIDLE11_DM15,		//# Holding laser trap
	TORSO_WEAPONIDLE12_DM15,		//# Holding detpack

	//# #sep TORSO_ USING NON-WEAPON OBJECTS

	//# #sep TORSO_ MISC
	TORSO_TALKR1START_DM15,		//# begin turning head for BOTH_TORSO_TALKR
	TORSO_TALKR1HOLD_DM15,		//# non-looping version of talk to right side
	TORSO_TALKR1STOP_DM15,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKR1_DM15,			//# talk to right side
	TORSO_TALKL1START_DM15,		//# begin turning head for BOTH_TORSO_TALKL
	TORSO_TALKL1HOLD_DM15,		//# non-looping version of talk to left side
	TORSO_TALKL1STOP_DM15,		//# return head to straight forward from BOTH_TORSO_TALKL
	TORSO_TALKL1_DM15,			//# talk to left side
	TORSO_LOOKL1_DM15,			//# looking left
	TORSO_LOOKR1_DM15,			//# looking right
	TORSO_LOOKR2START_DM15,		//# turn not so far as TALKR1
	TORSO_LOOKR2STOP_DM15,		//# turn not so far as TALKR1
	TORSO_LOOKR2_DM15,			//# looking right - not so far as LOOKR1
	TORSO_LOOKL2START_DM15,		//# turn not so far as TALKL1
	TORSO_LOOKL2STOP_DM15,		//# turn not so far as TALKL1
	TORSO_LOOKL2_DM15,			//# looking right - not so far as LOOKL1
	TORSO_HANDGESTURE1_DM15,		//# gestures to left one hand
	TORSO_HANDGESTURE2_DM15,		//# gestures to right one hand
	TORSO_HANDGESTURE3_DM15,		//# gestures to the left both hands
	TORSO_HANDGESTURE4_DM15,		//# gestures to the right both hands

	TORSO_HANDEXTEND1_DM15,		//# doctor reaching for hypospray in scav5
	TORSO_HANDRETRACT1_DM15,		//# doctor taking hypospray from player in scav5

	TORSO_DROPHELMET1_DM15,		//# Drop the helmet to the waist
	TORSO_RAISEHELMET1_DM15,		//# Bring the helmet to the head
	TORSO_REACHHELMET1_DM15,		//# reaching for helmet off of 60 tall cabinet
	TORSO_GRABLBACKL_DM15,		//# reach to lower back with left hand
	TORSO_GRABUBACKL_DM15,		//# reach to upper back with left hand
	TORSO_GRABLBACKR_DM15,		//# reach to lower back with right hand
	TORSO_GRABUBACKR_DM15,		//# reach to upper back with right hand

	TORSO_SURRENDER_START_DM15,	//# arms up
	TORSO_SURRENDER_STOP_DM15,	//# arms back down

	TORSO_CHOKING1_DM15,			//# TEMP


	//=================================================
	//ANIMS IN WHICH ONLY THE LOWER OBJECTS ARE IN MD3
	//=================================================
	//# #sep Legs-only anims
	LEGS_WALKBACK1_DM15,			//# Walk1 backwards
	LEGS_WALKBACK2_DM15,			//# Walk2 backwards
	LEGS_RUNBACK1_DM15,			//# Run1 backwards
	LEGS_RUNBACK2_DM15,			//# Run2 backwards
	LEGS_TURN1_DM15,				//# What legs do when you turn your lower body to match your upper body facing
	LEGS_TURN2_DM15,				//# Leg turning from stand2
	LEGS_LEAN_LEFT1_DM15,		//# Lean left
	LEGS_LEAN_RIGHT1_DM15,		//# Lean Right
	LEGS_KNEELDOWN1_DM15,		//# Get down on one knee?
	LEGS_KNEELUP1_DM15,			//# Get up from one knee?
	LEGS_CRLEAN_LEFT1_DM15,		//# Crouch Lean left
	LEGS_CRLEAN_RIGHT1_DM15,		//# Crouch Lean Right
	LEGS_CHOKING1_DM15,			//# TEMP
	LEGS_LEFTUP1_DM15,			//# On a slope with left foot 4 higher than right
	LEGS_LEFTUP2_DM15,			//# On a slope with left foot 8 higher than right
	LEGS_LEFTUP3_DM15,			//# On a slope with left foot 12 higher than right
	LEGS_LEFTUP4_DM15,			//# On a slope with left foot 16 higher than right
	LEGS_LEFTUP5_DM15,			//# On a slope with left foot 20 higher than right
	LEGS_RIGHTUP1_DM15,			//# On a slope with RIGHT foot 4 higher than left
	LEGS_RIGHTUP2_DM15,			//# On a slope with RIGHT foot 8 higher than left
	LEGS_RIGHTUP3_DM15,			//# On a slope with RIGHT foot 12 higher than left
	LEGS_RIGHTUP4_DM15,			//# On a slope with RIGHT foot 16 higher than left
	LEGS_RIGHTUP5_DM15,			//# On a slope with RIGHT foot 20 higher than left
	LEGS_S1_LUP1_DM15,
	LEGS_S1_LUP2_DM15,
	LEGS_S1_LUP3_DM15,
	LEGS_S1_LUP4_DM15,
	LEGS_S1_LUP5_DM15,
	LEGS_S1_RUP1_DM15,
	LEGS_S1_RUP2_DM15,
	LEGS_S1_RUP3_DM15,
	LEGS_S1_RUP4_DM15,
	LEGS_S1_RUP5_DM15,
	LEGS_S3_LUP1_DM15,
	LEGS_S3_LUP2_DM15,
	LEGS_S3_LUP3_DM15,
	LEGS_S3_LUP4_DM15,
	LEGS_S3_LUP5_DM15,
	LEGS_S3_RUP1_DM15,
	LEGS_S3_RUP2_DM15,
	LEGS_S3_RUP3_DM15,
	LEGS_S3_RUP4_DM15,
	LEGS_S3_RUP5_DM15,
	LEGS_S4_LUP1_DM15,
	LEGS_S4_LUP2_DM15,
	LEGS_S4_LUP3_DM15,
	LEGS_S4_LUP4_DM15,
	LEGS_S4_LUP5_DM15,
	LEGS_S4_RUP1_DM15,
	LEGS_S4_RUP2_DM15,
	LEGS_S4_RUP3_DM15,
	LEGS_S4_RUP4_DM15,
	LEGS_S4_RUP5_DM15,
	LEGS_S5_LUP1_DM15,
	LEGS_S5_LUP2_DM15,
	LEGS_S5_LUP3_DM15,
	LEGS_S5_LUP4_DM15,
	LEGS_S5_LUP5_DM15,
	LEGS_S5_RUP1_DM15,
	LEGS_S5_RUP2_DM15,
	LEGS_S5_RUP3_DM15,
	LEGS_S5_RUP4_DM15,
	LEGS_S5_RUP5_DM15,
	//=================================================
	//HEAD ANIMS
	//=================================================
	//# #sep Head-only anims
	FACE_TALK1_DM15,			//# quiet
	FACE_TALK2_DM15,			//# semi-quiet
	FACE_TALK3_DM15,			//# semi-loud
	FACE_TALK4_DM15,			//# loud
	FACE_ALERT_DM15,				//# 
	FACE_SMILE_DM15,				//# 
	FACE_FROWN_DM15,				//# 
	FACE_DEAD_DM15,				//# 

	//Boot
	BOOT_BLOCK_DIAG_LEFT_DM15,
	BOOT_BLOCK_DIAG_RIGHT_DM15,
	BOOT_UNUSED3_DM15,
	BOOT_UNUSED4_DM15,
	BOOT_UNUSED5_DM15,
	BOOT_UNUSED6_DM15,
	BOOT_UNUSED7_DM15,
	BOOT_UNUSED8_DM15,
	BOOT_UNUSED9_DM15,
	BOOT_UNUSED10_DM15,
	BOOT_UNUSED11_DM15,
	BOOT_UNUSED12_DM15,
	BOOT_UNUSED13_DM15,
	BOOT_UNUSED14_DM15,
	BOOT_UNUSED15_DM15,
	BOOT_UNUSED16_DM15,
	//

	//# #eol
	MAX_ANIMATIONS_DM15,// = FACE_DEAD_15 + 1 + BOOT_ANIMS,
	MAX_TOTALANIMATIONS_DM15
} animNumber15_t;

extern const int animMappingTable_1_04_to_1_02[MAX_TOTALANIMATIONS_DM16];

extern const int animMappingTable_1_02_to_1_04[MAX_TOTALANIMATIONS_DM15];

/* Macros */
//#define MV_MapAnimation104(anim) (animMappingTable_1_02_to_1_04[(anim)&~ANIM_TOGGLEBIT] | ((anim)&ANIM_TOGGLEBIT))
//#define MV_MapAnimation102(anim) (animMappingTable_1_04_to_1_02[(anim)&~ANIM_TOGGLEBIT] | ((anim)&ANIM_TOGGLEBIT))


#endif// #ifndef __ANIMS_H__

