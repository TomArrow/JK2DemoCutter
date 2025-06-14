#include "PMove.h"
#include "CModel.h"
#include "demoCut.h"

// TODO: togglebits for _GENERAL will need some higher value prolly
// TODO make sure all the _GENERAL stuff is converted going into pmove

#if CLIENTSIDEONLY
#define NONETWORK_FORCEPOWERLEVEL(ps,fp) (((ps)->fd.forcePowersKnown & (1<<(fp))) ? FORCE_LEVEL_3 : FORCE_LEVEL_0)
#else
#define NONETWORK_FORCEPOWERLEVEL(ps,fp) ((ps)->fd.forcePowerLevel[(fp)])
#endif

//const int animOverrideNums[] = {
//	BOTH_FORCEWALLREBOUND_BACK_GENERAL,
//	BOTH_FORCEWALLREBOUND_FORWARD_GENERAL,
//	BOTH_FORCEWALLREBOUND_LEFT_GENERAL,
//	BOTH_FORCEWALLREBOUND_RIGHT_GENERAL,
//
//	BOTH_FORCEWALLHOLD_BACK_GENERAL,
//	BOTH_FORCEWALLHOLD_FORWARD_GENERAL,
//	BOTH_FORCEWALLHOLD_LEFT_GENERAL,
//	BOTH_FORCEWALLHOLD_RIGHT_GENERAL,
//
//	BOTH_FORCEWALLRELEASE_BACK_GENERAL,
//	BOTH_FORCEWALLRELEASE_FORWARD_GENERAL,
//	BOTH_FORCEWALLRELEASE_LEFT_GENERAL,
//	BOTH_FORCEWALLRELEASE_RIGHT_GENERAL,
//};

void Sys_SnapVector(vec3_t vec) {
	// non-asm version. hope its correct?
	vec[0] = (float)(int)(roundf(vec[0]) + 0.5f);
	vec[1] = (float)(int)(roundf(vec[1]) + 0.5f);
	vec[2] = (float)(int)(roundf(vec[2]) + 0.5f);
}

//
//#define ENUM2STRING(arg)   {#arg,arg}
//typedef struct stringID_table_s
//{
//	char* name;
//	int		id;
//} stringID_table_t;

//int  GetIDForString(stringID_table_t* table, const char* string);
//const char* GetStringForID(stringID_table_t* table, int id);

int		 Q_rand(int* seed) {
	*seed = (69069 * *seed + 1);
	return *seed;
}

float	 Q_random(int* seed) {
	return (Q_rand(seed) & 0xffff) / (float)0x10000;
}

//const int animOverrideCount = sizeof(animOverrideNums) / sizeof(animOverrideNums[0]);
//stringID_table_t animTable[MAX_ANIMATIONS_GENERAL + 1]; 




int WeaponReadyAnim[WP_NUM_WEAPONS_GENERAL] =
{
	TORSO_DROPWEAP1_GENERAL,//WP_NONE_GENERAL,

	TORSO_WEAPONREADY3_GENERAL,//WP_STUN_BATON_GENERAL,
	BOTH_STAND2_GENERAL,//WP_SABER_GENERAL,
	TORSO_WEAPONREADY2_GENERAL,//WP_BRYAR_PISTOL_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//WP_BLASTER_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY4_GENERAL,//WP_DISRUPTOR_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY5_GENERAL,//WP_BOWCASTER_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY6_GENERAL,//WP_REPEATER_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY7_GENERAL,//WP_DEMP2_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY8_GENERAL,//WP_FLECHETTE_GENERAL,
	TORSO_WEAPONREADY3_GENERAL,//TORSO_WEAPONREADY9_GENERAL,//WP_ROCKET_LAUNCHER_GENERAL,
	TORSO_WEAPONREADY10_GENERAL,//WP_THERMAL_GENERAL,
	TORSO_WEAPONREADY10_GENERAL,//TORSO_WEAPONREADY11_GENERAL,//WP_TRIP_MINE_GENERAL,
	TORSO_WEAPONREADY10_GENERAL,//TORSO_WEAPONREADY12_GENERAL,//WP_DET_PACK_GENERAL,

	//NOT VALID (e.g. should never really be used):
	BOTH_STAND1_GENERAL,//WP_EMPLACED_GUN_GENERAL,
	TORSO_WEAPONREADY1_GENERAL//WP_TURRET_GENERAL,
};

int WeaponAttackAnim[WP_NUM_WEAPONS_GENERAL] =
{
	BOTH_ATTACK1_GENERAL,//WP_NONE_GENERAL, //(shouldn't happen)

	BOTH_ATTACK3_GENERAL,//WP_STUN_BATON_GENERAL,
	BOTH_STAND2_GENERAL,//WP_SABER_GENERAL, //(has its own handling)
	BOTH_ATTACK2_GENERAL,//WP_BRYAR_PISTOL_GENERAL,
	BOTH_ATTACK3_GENERAL,//WP_BLASTER_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK4_GENERAL,//WP_DISRUPTOR_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK5_GENERAL,//WP_BOWCASTER_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK6_GENERAL,//WP_REPEATER_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK7_GENERAL,//WP_DEMP2_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK8_GENERAL,//WP_FLECHETTE_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK9_GENERAL,//WP_ROCKET_LAUNCHER_GENERAL,
	BOTH_THERMAL_THROW_GENERAL,//WP_THERMAL_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK11_GENERAL,//WP_TRIP_MINE_GENERAL,
	BOTH_ATTACK3_GENERAL,//BOTH_ATTACK12_GENERAL,//WP_DET_PACK_GENERAL,

	//NOT VALID (e.g. should never really be used):
	BOTH_STAND1_GENERAL,//WP_EMPLACED_GUN_GENERAL,
	BOTH_ATTACK1_GENERAL//WP_TURRET_GENERAL,
};





//class PMove {
	














// fp16












// this is a simplified version of https://github.com/Maratyszcza/FP16 without any intrinsics or whatever
// also have to change uint16_t to short since qvm gets really angry at unsigned 16 bit ints
// we're gonna use this for some encoding of float values in usercmd_t for strafebot (strafefactor)

static float fp32_from_bits(uint32_t w) {
	union {
		uint32_t as_bits;
		float as_value;
	} fp32;
	fp32.as_bits = w;
	return fp32.as_value;
}

static uint32_t fp32_to_bits(float f) {
	union {
		float as_value;
		uint32_t as_bits;
	} fp32;
	fp32.as_value = f;
	return fp32.as_bits;
}


// this function is actually from https://github.com/gagern/gnulib
// since qvm doesnt really have __builtin_clz or such
static int count_leading_zeros_32(unsigned int x)
{
	/* http://graphics.stanford.edu/~seander/bithacks.html */
	static const char de_Bruijn_lookup[32] = {
	  31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,
	  23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0
	};

	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return de_Bruijn_lookup[((x * 0x07c4acddU) & 0xffffffffU) >> 27];
}

/*
 * Convert a 16-bit floating-point number in IEEE half-precision format, in bit representation, to
 * a 32-bit floating-point number in IEEE single-precision format, in bit representation.
 *
 * @note The implementation doesn't use any floating-point operations.
 */
uint32_t fp16_ieee_to_fp32_bits(uint32_t h) {
	/*
	 * Extend the half-precision floating-point number to 32 bits and shift to the upper part of the 32-bit word:
	 *      +---+-----+------------+-------------------+
	 *      | S |EEEEE|MM MMMM MMMM|0000 0000 0000 0000|
	 *      +---+-----+------------+-------------------+
	 * Bits  31  26-30    16-25            0-15
	 *
	 * S - sign bit, E - bits of the biased exponent, M - bits of the mantissa, 0 - zero bits.
	 */
	const uint32_t w = (uint32_t)h << 16;
	/*
	 * Extract the sign of the input number into the high bit of the 32-bit word:
	 *
	 *      +---+----------------------------------+
	 *      | S |0000000 00000000 00000000 00000000|
	 *      +---+----------------------------------+
	 * Bits  31                 0-31
	 */
	const uint32_t sign = w & UINT32_C(0x80000000);
	/*
	 * Extract mantissa and biased exponent of the input number into the bits 0-30 of the 32-bit word:
	 *
	 *      +---+-----+------------+-------------------+
	 *      | 0 |EEEEE|MM MMMM MMMM|0000 0000 0000 0000|
	 *      +---+-----+------------+-------------------+
	 * Bits  30  27-31     17-26            0-16
	 */
	const uint32_t nonsign = w & UINT32_C(0x7FFFFFFF);
	/*
	 * Iff half-precision number has exponent of 15, the addition overflows it into bit 31,
	 * and the subsequent shift turns the high 9 bits into 1. Thus
	 *   inf_nan_mask ==
	 *                   0x7F800000 if the half-precision number had exponent of 15 (i.e. was NaN or infinity)
	 *                   0x00000000 otherwise
	 */
	const int32_t inf_nan_mask = ((int32_t)(nonsign + 0x04000000) >> 8) & INT32_C(0x7F800000);
	/*
	 * Iff nonsign is 0, it overflows into 0xFFFFFFFF, turning bit 31 into 1. Otherwise, bit 31 remains 0.
	 * The signed shift right by 31 broadcasts bit 31 into all bits of the zero_mask. Thus
	 *   zero_mask ==
	 *                0xFFFFFFFF if the half-precision number was zero (+0.0h or -0.0h)
	 *                0x00000000 otherwise
	 */
	const int32_t zero_mask = (int32_t)(nonsign - 1) >> 31;

	/*
	 * Renorm shift is the number of bits to shift mantissa left to make the half-precision number normalized.
	 * If the initial number is normalized, some of its high 6 bits (sign == 0 and 5-bit exponent) equals one.
	 * In this case renorm_shift == 0. If the number is denormalize, renorm_shift > 0. Note that if we shift
	 * denormalized nonsign by renorm_shift, the unit bit of mantissa will shift into exponent, turning the
	 * biased exponent into 1, and making mantissa normalized (i.e. without leading 1).
	 */
	uint32_t renorm_shift = count_leading_zeros_32(nonsign);
	renorm_shift = renorm_shift > 5 ? renorm_shift - 5 : 0;

	/*
	 * 1. Shift nonsign left by renorm_shift to normalize it (if the input was denormal)
	 * 2. Shift nonsign right by 3 so the exponent (5 bits originally) becomes an 8-bit field and 10-bit mantissa
	 *    shifts into the 10 high bits of the 23-bit mantissa of IEEE single-precision number.
	 * 3. Add 0x70 to the exponent (starting at bit 23) to compensate the different in exponent bias
	 *    (0x7F for single-precision number less 0xF for half-precision number).
	 * 4. Subtract renorm_shift from the exponent (starting at bit 23) to account for renormalization. As renorm_shift
	 *    is less than 0x70, this can be combined with step 3.
	 * 5. Binary OR with inf_nan_mask to turn the exponent into 0xFF if the input was NaN or infinity.
	 * 6. Binary ANDNOT with zero_mask to turn the mantissa and exponent into zero if the input was zero.
	 * 7. Combine with the sign of the input number.
	 */
	return sign | ((((nonsign << renorm_shift >> 3) + ((0x70 - renorm_shift) << 23)) | inf_nan_mask) & ~zero_mask);
}

/*
 * Convert a 16-bit floating-point number in IEEE half-precision format, in bit representation, to
 * a 32-bit floating-point number in IEEE single-precision format.
 *
 * @note The implementation relies on IEEE-like (no assumption about rounding mode and no operations on denormals)
 * floating-point operations and bitcasts between integer and floating-point variables.
 */
float fp16_ieee_to_fp32_value(short hShort) {

	const uint32_t h = SHORT2USHORT(hShort);
	/*
	 * Extend the half-precision floating-point number to 32 bits and shift to the upper part of the 32-bit word:
	 *      +---+-----+------------+-------------------+
	 *      | S |EEEEE|MM MMMM MMMM|0000 0000 0000 0000|
	 *      +---+-----+------------+-------------------+
	 * Bits  31  26-30    16-25            0-15
	 *
	 * S - sign bit, E - bits of the biased exponent, M - bits of the mantissa, 0 - zero bits.
	 */
	const uint32_t w = (uint32_t)h << 16;
	/*
	 * Extract the sign of the input number into the high bit of the 32-bit word:
	 *
	 *      +---+----------------------------------+
	 *      | S |0000000 00000000 00000000 00000000|
	 *      +---+----------------------------------+
	 * Bits  31                 0-31
	 */
	const uint32_t sign = w & UINT32_C(0x80000000);
	/*
	 * Extract mantissa and biased exponent of the input number into the high bits of the 32-bit word:
	 *
	 *      +-----+------------+---------------------+
	 *      |EEEEE|MM MMMM MMMM|0 0000 0000 0000 0000|
	 *      +-----+------------+---------------------+
	 * Bits  27-31    17-26            0-16
	 */
	const uint32_t two_w = w + w;

	/*
	 * Shift mantissa and exponent into bits 23-28 and bits 13-22 so they become mantissa and exponent
	 * of a single-precision floating-point number:
	 *
	 *       S|Exponent |          Mantissa
	 *      +-+---+-----+------------+----------------+
	 *      |0|000|EEEEE|MM MMMM MMMM|0 0000 0000 0000|
	 *      +-+---+-----+------------+----------------+
	 * Bits   | 23-31   |           0-22
	 *
	 * Next, there are some adjustments to the exponent:
	 * - The exponent needs to be corrected by the difference in exponent bias between single-precision and half-precision
	 *   formats (0x7F - 0xF = 0x70)
	 * - Inf and NaN values in the inputs should become Inf and NaN values after conversion to the single-precision number.
	 *   Therefore, if the biased exponent of the half-precision input was 0x1F (max possible value), the biased exponent
	 *   of the single-precision output must be 0xFF (max possible value). We do this correction in two steps:
	 *   - First, we adjust the exponent by (0xFF - 0x1F) = 0xE0 (see exp_offset below) rather than by 0x70 suggested
	 *     by the difference in the exponent bias (see above).
	 *   - Then we multiply the single-precision result of exponent adjustment by 2**(-112) to reverse the effect of
	 *     exponent adjustment by 0xE0 less the necessary exponent adjustment by 0x70 due to difference in exponent bias.
	 *     The floating-point multiplication hardware would ensure than Inf and NaN would retain their value on at least
	 *     partially IEEE754-compliant implementations.
	 *
	 * Note that the above operations do not handle denormal inputs (where biased exponent == 0). However, they also do not
	 * operate on denormal inputs, and do not produce denormal results.
	 */
	const uint32_t exp_offset = UINT32_C(0xE0) << 23;

	const float exp_scale = fp32_from_bits(UINT32_C(0x7800000));
	const float normalized_value = fp32_from_bits((two_w >> 4) + exp_offset) * exp_scale;

	/*
	 * Convert denormalized half-precision inputs into single-precision results (always normalized).
	 * Zero inputs are also handled here.
	 *
	 * In a denormalized number the biased exponent is zero, and mantissa has on-zero bits.
	 * First, we shift mantissa into bits 0-9 of the 32-bit word.
	 *
	 *                  zeros           |  mantissa
	 *      +---------------------------+------------+
	 *      |0000 0000 0000 0000 0000 00|MM MMMM MMMM|
	 *      +---------------------------+------------+
	 * Bits             10-31                0-9
	 *
	 * Now, remember that denormalized half-precision numbers are represented as:
	 *    FP16 = mantissa * 2**(-24).
	 * The trick is to construct a normalized single-precision number with the same mantissa and thehalf-precision input
	 * and with an exponent which would scale the corresponding mantissa bits to 2**(-24).
	 * A normalized single-precision floating-point number is represented as:
	 *    FP32 = (1 + mantissa * 2**(-23)) * 2**(exponent - 127)
	 * Therefore, when the biased exponent is 126, a unit change in the mantissa of the input denormalized half-precision
	 * number causes a change of the constructud single-precision number by 2**(-24), i.e. the same ammount.
	 *
	 * The last step is to adjust the bias of the constructed single-precision number. When the input half-precision number
	 * is zero, the constructed single-precision number has the value of
	 *    FP32 = 1 * 2**(126 - 127) = 2**(-1) = 0.5
	 * Therefore, we need to subtract 0.5 from the constructed single-precision number to get the numerical equivalent of
	 * the input half-precision number.
	 */
	const uint32_t magic_mask = UINT32_C(126) << 23;
	const float magic_bias = 0.5f;
	const float denormalized_value = fp32_from_bits((two_w >> 17) | magic_mask) - magic_bias;

	/*
	 * - Choose either results of conversion of input as a normalized number, or as a denormalized number, depending on the
	 *   input exponent. The variable two_w contains input exponent in bits 27-31, therefore if its smaller than 2**27, the
	 *   input is either a denormal number, or zero.
	 * - Combine the result of conversion of exponent and mantissa with the sign of the input number.
	 */
	const uint32_t denormalized_cutoff = UINT32_C(1) << 27;
	const uint32_t result = sign |
		(two_w < denormalized_cutoff ? fp32_to_bits(denormalized_value) : fp32_to_bits(normalized_value));
	return fp32_from_bits(result);

}

/*
 * Convert a 32-bit floating-point number in IEEE single-precision format to a 16-bit floating-point number in
 * IEEE half-precision format, in bit representation.
 *
 * @note The implementation relies on IEEE-like (no assumption about rounding mode and no operations on denormals)
 * floating-point operations and bitcasts between integer and floating-point variables.
 */
short fp16_ieee_from_fp32_value(float f) {
	uint32_t bits, exp_bits, mantissa_bits, nonsign;
	uint32_t result;
	const float scale_to_inf = fp32_from_bits(UINT32_C(0x77800000));
	const float scale_to_zero = fp32_from_bits(UINT32_C(0x08800000));

	const float saturated_f = fabsf(f) * scale_to_inf;

	float base = saturated_f * scale_to_zero;

	const uint32_t w = fp32_to_bits(f);
	const uint32_t shl1_w = w + w;
	const uint32_t sign = w & UINT32_C(0x80000000);
	uint32_t bias = shl1_w & UINT32_C(0xFF000000);
	if (bias < UINT32_C(0x71000000)) {
		bias = UINT32_C(0x71000000);
	}

	base = fp32_from_bits((bias >> 1) + UINT32_C(0x07800000)) + base;

	// declarations moved to start of fucntion for qvm
	bits = fp32_to_bits(base);
	exp_bits = (bits >> 13) & UINT32_C(0x00007C00);
	mantissa_bits = bits & UINT32_C(0x00000FFF);
	nonsign = exp_bits + mantissa_bits;
	result = (sign >> 16) | (shl1_w > UINT32_C(0xFF000000) ? UINT32_C(0x7E00) : nonsign); // changed UINT16_C to UINT32_C. prolly irrelevant but whatever. qvm doesnt like 16 bit unsigned ints :(

	return USHORT2SHORT(result);
}




























// bg_defrag_global.c


void DF_AntiLoop_NewAngle(antiLoopState_t* antiLoopState, vec3_t oldVelocity, vec3_t velocity, float baseSpeed, qboolean inRace) {
	//float xyVel = VectorLength(velocity); //XYSPEED(velocity);
	float xyVel = XYSPEED(velocity); // should prolly be vectorlength but now ppl already did runs like this so i dont wanna mess wiht it (cuz ppl in quajk can crawl up big steep slopes and move dowm with essentially < baseSpeed but high vertical speed but oh well
	if (xyVel < baseSpeed/* && !inRace*/) {
		antiLoopState->yawAngleChangeSinceBaseSpeed = 0;
	}
	else {
		vec3_t velNorm, oldVelNorm;
		vec3_t angles, anglesOld;
		float diff;
		VectorCopy(oldVelocity, oldVelNorm);
		VectorCopy(velocity, velNorm);
		VectorNormalize(oldVelNorm);
		VectorNormalize(velNorm);
		vectoangles(velNorm, angles);
		vectoangles(oldVelNorm, anglesOld);
		diff = AngleSubtract(angles[YAW], anglesOld[YAW]);
		antiLoopState->yawAngleChangeSinceBaseSpeed += fabsf(diff);
	}
}


qboolean MovementStyleHasQuake2Ramps(int moveStyle) {
	if (moveStyle == MV_DREAM || moveStyle == MV_QUAJK || moveStyle == MV_SICKO || moveStyle == MV_PINBALL) {
		return qtrue;
	}
	return qfalse;
}
qboolean MovementStyleHasVQ3OnlyJumppads(int moveStyle) {
	if (moveStyle == MV_DREAM || moveStyle == MV_QUAJK || moveStyle == MV_SICKO || moveStyle == MV_PJK2) {
		return qfalse;
	}
	return qtrue;
}
qboolean MovementStyleHasCPMOnlyJumppads(int moveStyle) {
	if (moveStyle == MV_DREAM || moveStyle == MV_QUAJK || moveStyle == MV_SICKO || moveStyle == MV_PJK2) {
		return qtrue;
	}
	return qfalse;
}
qboolean MovementIsQuake3Based(int moveStyle) {
	if (moveStyle == MV_DREAM || moveStyle == MV_QUAJK || moveStyle == MV_SICKO) {
		return qtrue;
	}
	return qfalse;
}
float MovementOverbounceFactor(int moveStyle, playerState_t* ps, usercmd_t* ucmd) {
	if (moveStyle == MV_BOUNCE) {
		if ((ps->stats[STAT_BOUNCEPOWER] & BOUNCEPOWER_POWERMASK) && (ucmd->buttons & BUTTON_BOUNCEPOWER)) {
			return 2.0f;
		}
		return 1.3f;
	}
	else if (moveStyle == MV_PINBALL) {
		return 2.1f;
	}
	//else if (moveStyle == MV_DREAM) {
	//	return 1.1f;
	//}
	return 1.001f; // OVERCLIP define
}













// bg_panimate.c




#pragma region "bg_panimate"





// BG_PAnimate.c


/*
==============================================================================
BEGIN: Animation utility functions (sequence checking)
==============================================================================
*/
//Called regardless of pm validity:
qboolean  PMove::BG_InSpecialJump( int anim, int runFlags )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_WALL_RUN_RIGHT_GENERAL:
	case BOTH_WALL_RUN_RIGHT_FLIP_GENERAL:
	case BOTH_WALL_RUN_LEFT_GENERAL:
	case BOTH_WALL_RUN_LEFT_FLIP_GENERAL:
	case BOTH_WALL_FLIP_RIGHT_GENERAL:
	case BOTH_WALL_FLIP_LEFT_GENERAL:
	case BOTH_FLIP_BACK1_GENERAL:
	case BOTH_FLIP_BACK2_GENERAL:
	case BOTH_FLIP_BACK3_GENERAL:
	case BOTH_WALL_FLIP_BACK1_GENERAL:
	case BOTH_BUTTERFLY_LEFT_GENERAL:
	case BOTH_BUTTERFLY_RIGHT_GENERAL:
		return qtrue;
	case BOTH_ARIAL_LEFT_GENERAL: // jka cartwheel
	case BOTH_ARIAL_RIGHT_GENERAL: // jka cartwheel
		if (runFlags & RFL_CLIMBTECH) {
			return qtrue;
		}
	}
	if ((runFlags & RFL_CLIMBTECH))
	{
		if ( BG_InReboundJump( anim ) )
		{
			return qtrue;
		}
		if ( BG_InReboundHold( anim ) )
		{
			return qtrue;
		}
		if ( BG_InReboundRelease( anim ) )
		{
			return qtrue;
		}
	}
	return qfalse;
}

qboolean  PMove::BG_InSaberStandAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_SABERFAST_STANCE_GENERAL:
	case BOTH_STAND2_GENERAL:
	case BOTH_SABERSLOW_STANCE_GENERAL:
		return qtrue;
	default:
		return qfalse;
	}
}

// JKA wallgrab
qboolean  PMove::BG_InReboundJump(int anim)
{
	switch (anim & ~ANIM_TOGGLEBIT) // TA: just fix this? (anim_togglebit)
	{
	case BOTH_FORCEWALLREBOUND_FORWARD_GENERAL:
	case BOTH_FORCEWALLREBOUND_LEFT_GENERAL:
	case BOTH_FORCEWALLREBOUND_BACK_GENERAL:
	case BOTH_FORCEWALLREBOUND_RIGHT_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}
// JKA wallgrab
qboolean  PMove::BG_InReboundHold(int anim)
{
	switch (anim & ~ANIM_TOGGLEBIT) // TA: just fix this? (anim_togglebit)
	{
	case BOTH_FORCEWALLHOLD_FORWARD_GENERAL:
	case BOTH_FORCEWALLHOLD_LEFT_GENERAL:
	case BOTH_FORCEWALLHOLD_BACK_GENERAL:
	case BOTH_FORCEWALLHOLD_RIGHT_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}
// JKA wallgrab
qboolean  PMove::BG_InReboundRelease(int anim)
{
	switch (anim & ~ANIM_TOGGLEBIT) // TA: just fix this? (anim_togglebit)
	{
	case BOTH_FORCEWALLRELEASE_FORWARD_GENERAL:
	case BOTH_FORCEWALLRELEASE_LEFT_GENERAL:
	case BOTH_FORCEWALLRELEASE_BACK_GENERAL:
	case BOTH_FORCEWALLRELEASE_RIGHT_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_InBackFlip(int anim)
{
	switch (anim)
	{
	case BOTH_FLIP_BACK1_GENERAL:
	case BOTH_FLIP_BACK2_GENERAL:
	case BOTH_FLIP_BACK3_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_DirectFlippingAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_FLIP_F_GENERAL:			//# Flip forward
	case BOTH_FLIP_B_GENERAL:			//# Flip backwards
	case BOTH_FLIP_L_GENERAL:			//# Flip left
	case BOTH_FLIP_R_GENERAL:			//# Flip right
		return qtrue;
		break;
	}

	return qfalse;
}

qboolean  PMove::BG_SaberInAttack( int move )
{
	if ( move >= LS_A_TL2BR_GENERAL && move <= LS_A_T2B_GENERAL )
	{
		return qtrue;
	}
	switch ( move )
	{
	case LS_A_BACK_GENERAL:
	case LS_A_BACK_CR_GENERAL:
	case LS_A_BACKSTAB_GENERAL:
	case LS_A_LUNGE_GENERAL:
	case LS_A_JUMP_T__B__GENERAL:
	case LS_A_FLIP_STAB_GENERAL:
	case LS_A_FLIP_SLASH_GENERAL:
	case LS_JUMPATTACK_ARIAL_LEFT_GENERAL: // jka cartwheel
	case LS_JUMPATTACK_ARIAL_RIGHT_GENERAL: // jka cartwheel
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_SaberInSpecial( int move )
{
	switch( move )
	{
	case LS_A_BACK_GENERAL:
	case LS_A_BACK_CR_GENERAL:
	case LS_A_BACKSTAB_GENERAL:
	case LS_A_LUNGE_GENERAL:
	case LS_A_JUMP_T__B__GENERAL:
	case LS_A_FLIP_STAB_GENERAL:
	case LS_A_FLIP_SLASH_GENERAL:
	case LS_JUMPATTACK_ARIAL_LEFT_GENERAL: // jka cartwheel
	case LS_JUMPATTACK_ARIAL_RIGHT_GENERAL: // jka cartwheel
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::BG_SaberInIdle( int move )
{
	switch ( move )
	{
	case LS_NONE_GENERAL:
	case LS_READY_GENERAL:
	case LS_DRAW_GENERAL:
	case LS_PUTAWAY_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}


qboolean  PMove::BG_FlippingAnim( int anim )
{
	switch ( (jk2gameplay == VERSION_1_02 ? anim : anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_FLIP_F_GENERAL:			//# Flip forward
	case BOTH_FLIP_B_GENERAL:			//# Flip backwards
	case BOTH_FLIP_L_GENERAL:			//# Flip left
	case BOTH_FLIP_R_GENERAL:			//# Flip right
	case BOTH_WALL_RUN_RIGHT_FLIP_GENERAL:
	case BOTH_WALL_RUN_LEFT_FLIP_GENERAL:
	case BOTH_WALL_FLIP_RIGHT_GENERAL:
	case BOTH_WALL_FLIP_LEFT_GENERAL:
	case BOTH_FLIP_BACK1_GENERAL:
	case BOTH_FLIP_BACK2_GENERAL:
	case BOTH_FLIP_BACK3_GENERAL:
	case BOTH_WALL_FLIP_BACK1_GENERAL:
	//Not really flips, but...
	case BOTH_WALL_RUN_RIGHT_GENERAL:
	case BOTH_WALL_RUN_LEFT_GENERAL:
	case BOTH_WALL_RUN_RIGHT_STOP_GENERAL:
	case BOTH_WALL_RUN_LEFT_STOP_GENERAL:
	case BOTH_BUTTERFLY_LEFT_GENERAL:
	case BOTH_BUTTERFLY_RIGHT_GENERAL:
	//
	case BOTH_ARIAL_LEFT_GENERAL:
	case BOTH_ARIAL_RIGHT_GENERAL:
	case BOTH_ARIAL_F1_GENERAL:
	case BOTH_CARTWHEEL_LEFT_GENERAL:
	case BOTH_CARTWHEEL_RIGHT_GENERAL:
	case BOTH_JUMPFLIPSLASHDOWN1_GENERAL:
	case BOTH_JUMPFLIPSTABDOWN_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_SpinningSaberAnim( int anim )
{
	switch ( (jk2gameplay == VERSION_1_02 ? anim : anim&~ANIM_TOGGLEBIT) )
	{
	//level 1 - FIXME: level 1 will have *no* spins
	case BOTH_T1_BR_BL_GENERAL:
	case BOTH_T1__R__L_GENERAL:
	case BOTH_T1__R_BL_GENERAL:
	case BOTH_T1_TR_BL_GENERAL:
	case BOTH_T1_BR_TL_GENERAL:
	case BOTH_T1_BR__L_GENERAL:
	case BOTH_T1_TL_BR_GENERAL:
	case BOTH_T1__L_BR_GENERAL:
	case BOTH_T1__L__R_GENERAL:
	case BOTH_T1_BL_BR_GENERAL:
	case BOTH_T1_BL__R_GENERAL:
	case BOTH_T1_BL_TR_GENERAL:
	//level 2
	case BOTH_T2_BR__L_GENERAL:
	case BOTH_T2_BR_BL_GENERAL:
	case BOTH_T2__R_BL_GENERAL:
	case BOTH_T2__L_BR_GENERAL:
	case BOTH_T2_BL_BR_GENERAL:
	case BOTH_T2_BL__R_GENERAL:
	//level 3
	case BOTH_T3_BR__L_GENERAL:
	case BOTH_T3_BR_BL_GENERAL:
	case BOTH_T3__R_BL_GENERAL:
	case BOTH_T3__L_BR_GENERAL:
	case BOTH_T3_BL_BR_GENERAL:
	case BOTH_T3_BL__R_GENERAL:
	//level 4
	case BOTH_T4_BR__L_GENERAL:
	case BOTH_T4_BR_BL_GENERAL:
	case BOTH_T4__R_BL_GENERAL:
	case BOTH_T4__L_BR_GENERAL:
	case BOTH_T4_BL_BR_GENERAL:
	case BOTH_T4_BL__R_GENERAL:
	//level 5
	case BOTH_T5_BR_BL_GENERAL:
	case BOTH_T5__R__L_GENERAL:
	case BOTH_T5__R_BL_GENERAL:
	case BOTH_T5_TR_BL_GENERAL:
	case BOTH_T5_BR_TL_GENERAL:
	case BOTH_T5_BR__L_GENERAL:
	case BOTH_T5_TL_BR_GENERAL:
	case BOTH_T5__L_BR_GENERAL:
	case BOTH_T5__L__R_GENERAL:
	case BOTH_T5_BL_BR_GENERAL:
	case BOTH_T5_BL__R_GENERAL:
	case BOTH_T5_BL_TR_GENERAL:
	//special
	//case BOTH_A2_STABBACK1_GENERAL:
	case BOTH_ATTACK_BACK_GENERAL:
	case BOTH_CROUCHATTACKBACK1_GENERAL:
	case BOTH_BUTTERFLY_LEFT_GENERAL:
	case BOTH_BUTTERFLY_RIGHT_GENERAL:
	case BOTH_FJSS_TR_BL_GENERAL:
	case BOTH_FJSS_TL_BR_GENERAL:
	case BOTH_JUMPFLIPSLASHDOWN1_GENERAL:
	case BOTH_JUMPFLIPSTABDOWN_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_SaberInSpecialAttack( int anim )
{
	switch ( anim&~ANIM_TOGGLEBIT )
	{
	case BOTH_A2_STABBACK1_GENERAL:
	case BOTH_ATTACK_BACK_GENERAL:
	case BOTH_CROUCHATTACKBACK1_GENERAL:
	case BOTH_BUTTERFLY_LEFT_GENERAL:
	case BOTH_BUTTERFLY_RIGHT_GENERAL:
	case BOTH_FJSS_TR_BL_GENERAL:
	case BOTH_FJSS_TL_BR_GENERAL:
	case BOTH_LUNGE2_B__T__GENERAL:
	case BOTH_FORCELEAP2_T__B__GENERAL:
	case BOTH_JUMPFLIPSLASHDOWN1_GENERAL://#
	case BOTH_JUMPFLIPSTABDOWN_GENERAL://#
		return qtrue;
	}
	return qfalse;
}

int  PMove::BG_BrokenParryForAttack( int move )
{
	//Our attack was knocked away by a knockaway parry
	//FIXME: need actual anims for this
	//FIXME: need to know which side of the saber was hit!  For now, we presume the saber gets knocked away from the center
	switch ( saberMoveData_general[move].startQuad )
	{
	case Q_B:
		return LS_V1_B__GENERAL;
		break;
	case Q_BR:
		return LS_V1_BR_GENERAL;
		break;
	case Q_R:
		return LS_V1__R_GENERAL;
		break;
	case Q_TR:
		return LS_V1_TR_GENERAL;
		break;
	case Q_T:
		return LS_V1_T__GENERAL;
		break;
	case Q_TL:
		return LS_V1_TL_GENERAL;
		break;
	case Q_L:
		return LS_V1__L_GENERAL;
		break;
	case Q_BL:
		return LS_V1_BL_GENERAL;
		break;
	}
	return LS_NONE_GENERAL;
}

int  PMove::BG_BrokenParryForParry( int move, qboolean raceMode )
{
	//FIXME: need actual anims for this
	//FIXME: need to know which side of the saber was hit!  For now, we presume the saber gets knocked away from the center
	switch ( move )
	{
	case LS_PARRY_UP_GENERAL:
		//Hmm... since we don't know what dir the hit came from, randomly pick knock down or knock back
		if ( Q_irand( 0, 1 + pml.randomAdd, raceMode, 0 ) )
		{
			return LS_H1_B__GENERAL;
		}
		else
		{
			return LS_H1_T__GENERAL;
		}
		break;
	case LS_PARRY_UR_GENERAL:
		return LS_H1_TR_GENERAL;
		break;
	case LS_PARRY_UL_GENERAL:
		return LS_H1_TL_GENERAL;
		break;
	case LS_PARRY_LR_GENERAL:
		return LS_H1_BR_GENERAL;
		break;
	case LS_PARRY_LL_GENERAL:
		return LS_H1_BL_GENERAL;
		break;
	case LS_READY_GENERAL:
		return LS_H1_B__GENERAL;//???
		break;
	}
	return LS_NONE_GENERAL;
}

int  PMove::BG_KnockawayForParry( int move )
{
	//FIXME: need actual anims for this
	//FIXME: need to know which side of the saber was hit!  For now, we presume the saber gets knocked away from the center
	switch ( move )
	{
	case BLOCKED_TOP://LS_PARRY_UP_GENERAL:
		return LS_K1_T__GENERAL;//push up
		break;
	case BLOCKED_UPPER_RIGHT://LS_PARRY_UR_GENERAL:
	default://case LS_READY_GENERAL:
		return LS_K1_TR_GENERAL;//push up, slightly to right
		break;
	case BLOCKED_UPPER_LEFT://LS_PARRY_UL_GENERAL:
		return LS_K1_TL_GENERAL;//push up and to left
		break;
	case BLOCKED_LOWER_RIGHT://LS_PARRY_LR_GENERAL:
		return LS_K1_BR_GENERAL;//push down and to left
		break;
	case BLOCKED_LOWER_LEFT://LS_PARRY_LL_GENERAL:
		return LS_K1_BL_GENERAL;//push down and to right
		break;
	}
	//return LS_NONE_GENERAL;
}

qboolean  PMove::BG_InRoll( playerState_t *ps, int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_ROLL_F_GENERAL:
	case BOTH_ROLL_B_GENERAL:
	case BOTH_ROLL_R_GENERAL:
	case BOTH_ROLL_L_GENERAL:
		if ( ps->legsTimer > 0 )
		{
			return qtrue;
		}
		break;
	}
	return qfalse;
}

qboolean  PMove::BG_InDeathAnim( int anim )
{
	switch((anim&~ANIM_TOGGLEBIT))
	{
	case BOTH_DIVE1_GENERAL:
	case BOTH_DEATHBACKWARD1_GENERAL:
	case BOTH_DEATHBACKWARD2_GENERAL:
	case BOTH_DEATHFORWARD1_GENERAL:
	case BOTH_DEATHFORWARD2_GENERAL:
	case BOTH_DEATH1_GENERAL:
	case BOTH_DEATH2_GENERAL:
	case BOTH_DEATH3_GENERAL:
	case BOTH_DEATH4_GENERAL:
	case BOTH_DEATH5_GENERAL:
	case BOTH_DEATH6_GENERAL:
	case BOTH_DEATH7_GENERAL:

	case BOTH_DEATH1IDLE_GENERAL:
	case BOTH_LYINGDEATH1_GENERAL:
	case BOTH_STUMBLEDEATH1_GENERAL:
	case BOTH_FALLDEATH1_GENERAL:
	case BOTH_FALLDEATH1INAIR_GENERAL:
	case BOTH_FALLDEATH1LAND_GENERAL:
		return qtrue;
		break;
	default:
		return qfalse;
		break;
	}
}

//Called only where pm is valid (not all require pm, but some do):
int  PMove::PM_SaberBounceForAttack( int move )
{
	switch ( saberMoveData_general[move].startQuad )
	{
	case Q_B:
	case Q_BR:
		return LS_B1_BR_GENERAL;
		break;
	case Q_R:
		return LS_B1__R_GENERAL;
		break;
	case Q_TR:
		return LS_B1_TR_GENERAL;
		break;
	case Q_T:
		return LS_B1_T__GENERAL;
		break;
	case Q_TL:
		return LS_B1_TL_GENERAL;
		break;
	case Q_L:
		return LS_B1__L_GENERAL;
		break;
	case Q_BL:
		return LS_B1_BL_GENERAL;
		break;
	}
	return LS_NONE_GENERAL;
}

int  PMove::PM_SaberDeflectionForQuad( int quad )
{
	switch ( quad )
	{
	case Q_B:
		return LS_D1_B__GENERAL;
		break;
	case Q_BR:
		return LS_D1_BR_GENERAL;
		break;
	case Q_R:
		return LS_D1__R_GENERAL;
		break;
	case Q_TR:
		return LS_D1_TR_GENERAL;
		break;
	case Q_T:
		return LS_D1_T__GENERAL;
		break;
	case Q_TL:
		return LS_D1_TL_GENERAL;
		break;
	case Q_L:
		return LS_D1__L_GENERAL;
		break;
	case Q_BL:
		return LS_D1_BL_GENERAL;
		break;
	}
	return LS_NONE_GENERAL;
}

qboolean  PMove::PM_SaberInDeflect( int move )
{
	if ( move >= LS_D1_BR_GENERAL && move <= LS_D1_B__GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInParry( int move )
{
	if ( move >= LS_PARRY_UP_GENERAL && move <= LS_PARRY_LL_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInKnockaway( int move )
{
	if ( move >= LS_K1_T__GENERAL && move <= LS_K1_BL_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInReflect( int move )
{
	if ( move >= LS_REFLECT_UP_GENERAL && move <= LS_REFLECT_LL_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInStart( int move )
{
	if ( move >= LS_S_TL2BR_GENERAL && move <= LS_S_T2B_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInReturn( int move )
{
	if ( move >= LS_R_TL2BR_GENERAL && move <= LS_R_TL2BR_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_InSaberAnim( int anim )
{
	if ( (anim&~ANIM_TOGGLEBIT) >= BOTH_A1_T__B__GENERAL && (anim&~ANIM_TOGGLEBIT) <= BOTH_H1_S1_BR_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_InKnockDown( playerState_t *ps )
{
	switch ( (ps->legsAnim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_KNOCKDOWN1_GENERAL:
	case BOTH_KNOCKDOWN2_GENERAL:
	case BOTH_KNOCKDOWN3_GENERAL:
	case BOTH_KNOCKDOWN4_GENERAL:
	case BOTH_KNOCKDOWN5_GENERAL:
		return qtrue;
		break;
	case BOTH_GETUP1_GENERAL:
	case BOTH_GETUP2_GENERAL:
	case BOTH_GETUP3_GENERAL:
	case BOTH_GETUP4_GENERAL:
	case BOTH_GETUP5_GENERAL:
	case BOTH_FORCE_GETUP_F1_GENERAL:
	case BOTH_FORCE_GETUP_F2_GENERAL:
	case BOTH_FORCE_GETUP_B1_GENERAL:
	case BOTH_FORCE_GETUP_B2_GENERAL:
	case BOTH_FORCE_GETUP_B3_GENERAL:
	case BOTH_FORCE_GETUP_B4_GENERAL:
	case BOTH_FORCE_GETUP_B5_GENERAL:
		if ( ps->legsTimer )
		{
			return qtrue;
		}
		break;
	}
	return qfalse;
}

//Called only where pm is valid (not all require pm, but some do):
/*qboolean PM_InCartwheel(int anim)
{
	switch (anim)
	{
	case BOTH_ARIAL_LEFT_GENERAL:
	case BOTH_ARIAL_RIGHT_GENERAL:
	case BOTH_ARIAL_F1_GENERAL:
	case BOTH_CARTWHEEL_LEFT_GENERAL:
	case BOTH_CARTWHEEL_RIGHT_GENERAL:
		return qtrue;
		break;
	}
	return qfalse;
}*/

qboolean  PMove::PM_PainAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
		case BOTH_PAIN1_GENERAL:				//# First take pain anim
		case BOTH_PAIN2_GENERAL:				//# Second take pain anim
		case BOTH_PAIN3_GENERAL:				//# Third take pain anim
		case BOTH_PAIN4_GENERAL:				//# Fourth take pain anim
		case BOTH_PAIN5_GENERAL:				//# Fifth take pain anim - from behind
		case BOTH_PAIN6_GENERAL:				//# Sixth take pain anim - from behind
		case BOTH_PAIN7_GENERAL:				//# Seventh take pain anim - from behind
		case BOTH_PAIN8_GENERAL:				//# Eigth take pain anim - from behind
		case BOTH_PAIN9_GENERAL:				//# 
		case BOTH_PAIN10_GENERAL:			//# 
		case BOTH_PAIN11_GENERAL:			//# 
		case BOTH_PAIN12_GENERAL:			//# 
		case BOTH_PAIN13_GENERAL:			//# 
		case BOTH_PAIN14_GENERAL:			//# 
		case BOTH_PAIN15_GENERAL:			//# 
		case BOTH_PAIN16_GENERAL:			//# 
		case BOTH_PAIN17_GENERAL:			//# 
		case BOTH_PAIN18_GENERAL:			//# 
		case BOTH_PAIN19_GENERAL:			//# 
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::PM_JumpingAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
		case BOTH_JUMP1_GENERAL:				//# Jump - wind-up and leave ground
		case BOTH_INAIR1_GENERAL:			//# In air loop (from jump)
		case BOTH_LAND1_GENERAL:				//# Landing (from in air loop)
		case BOTH_LAND2_GENERAL:				//# Landing Hard (from a great height)
		case BOTH_JUMPBACK1_GENERAL:			//# Jump backwards - wind-up and leave ground
		case BOTH_INAIRBACK1_GENERAL:		//# In air loop (from jump back)
		case BOTH_LANDBACK1_GENERAL:			//# Landing backwards(from in air loop)
		case BOTH_JUMPLEFT1_GENERAL:			//# Jump left - wind-up and leave ground
		case BOTH_INAIRLEFT1_GENERAL:		//# In air loop (from jump left)
		case BOTH_LANDLEFT1_GENERAL:			//# Landing left(from in air loop)
		case BOTH_JUMPRIGHT1_GENERAL:		//# Jump right - wind-up and leave ground
		case BOTH_INAIRRIGHT1_GENERAL:		//# In air loop (from jump right)
		case BOTH_LANDRIGHT1_GENERAL:		//# Landing right(from in air loop)
		case BOTH_FORCEJUMP1_GENERAL:				//# Jump - wind-up and leave ground
		case BOTH_FORCEINAIR1_GENERAL:			//# In air loop (from jump)
		case BOTH_FORCELAND1_GENERAL:				//# Landing (from in air loop)
		case BOTH_FORCEJUMPBACK1_GENERAL:			//# Jump backwards - wind-up and leave ground
		case BOTH_FORCEINAIRBACK1_GENERAL:		//# In air loop (from jump back)
		case BOTH_FORCELANDBACK1_GENERAL:			//# Landing backwards(from in air loop)
		case BOTH_FORCEJUMPLEFT1_GENERAL:			//# Jump left - wind-up and leave ground
		case BOTH_FORCEINAIRLEFT1_GENERAL:		//# In air loop (from jump left)
		case BOTH_FORCELANDLEFT1_GENERAL:			//# Landing left(from in air loop)
		case BOTH_FORCEJUMPRIGHT1_GENERAL:		//# Jump right - wind-up and leave ground
		case BOTH_FORCEINAIRRIGHT1_GENERAL:		//# In air loop (from jump right)
		case BOTH_FORCELANDRIGHT1_GENERAL:		//# Landing right(from in air loop)
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::PM_LandingAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
		case BOTH_LAND1_GENERAL:				//# Landing (from in air loop)
		case BOTH_LAND2_GENERAL:				//# Landing Hard (from a great height)
		case BOTH_LANDBACK1_GENERAL:			//# Landing backwards(from in air loop)
		case BOTH_LANDLEFT1_GENERAL:			//# Landing left(from in air loop)
		case BOTH_LANDRIGHT1_GENERAL:		//# Landing right(from in air loop)
		case BOTH_FORCELAND1_GENERAL:		//# Landing (from in air loop)
		case BOTH_FORCELANDBACK1_GENERAL:	//# Landing backwards(from in air loop)
		case BOTH_FORCELANDLEFT1_GENERAL:	//# Landing left(from in air loop)
		case BOTH_FORCELANDRIGHT1_GENERAL:	//# Landing right(from in air loop)
		return qtrue;
		break;
	}
	return qfalse;
}

qboolean  PMove::PM_SpinningAnim( int anim )
{
	/*
	switch ( anim )
	{
	//FIXME: list any other spinning anims
	default:
		break;
	}
	*/
	return BG_SpinningSaberAnim( anim );
}

qboolean  PMove::PM_InOnGroundAnim ( int anim )
{
	switch( anim&~ANIM_TOGGLEBIT )
	{
	case BOTH_DEAD1_GENERAL:
	case BOTH_DEAD2_GENERAL:
	case BOTH_DEAD3_GENERAL:
	case BOTH_DEAD4_GENERAL:
	case BOTH_DEAD5_GENERAL:
	case BOTH_DEADFORWARD1_GENERAL:
	case BOTH_DEADBACKWARD1_GENERAL:
	case BOTH_DEADFORWARD2_GENERAL:
	case BOTH_DEADBACKWARD2_GENERAL:
	case BOTH_LYINGDEATH1_GENERAL:
	case BOTH_LYINGDEAD1_GENERAL:
	case BOTH_PAIN2WRITHE1_GENERAL:		//# Transition from upright position to writhing on ground anim
	case BOTH_WRITHING1_GENERAL:			//# Lying on ground writhing in pain
	case BOTH_WRITHING1RLEG_GENERAL:		//# Lying on ground writhing in pain: holding right leg
	case BOTH_WRITHING1LLEG_GENERAL:		//# Lying on ground writhing in pain: holding left leg
	case BOTH_WRITHING2_GENERAL:			//# Lying on stomache writhing in pain
	case BOTH_INJURED1_GENERAL:			//# Lying down: against wall - can also be sleeping
	case BOTH_CRAWLBACK1_GENERAL:			//# Lying on back: crawling backwards with elbows
	case BOTH_INJURED2_GENERAL:			//# Injured pose 2
	case BOTH_INJURED3_GENERAL:			//# Injured pose 3
	case BOTH_INJURED6_GENERAL:			//# Injured pose 6
	case BOTH_INJURED6ATTACKSTART_GENERAL:	//# Start attack while in injured 6 pose 
	case BOTH_INJURED6ATTACKSTOP_GENERAL:	//# End attack while in injured 6 pose
	case BOTH_INJURED6COMBADGE_GENERAL:	//# Hit combadge while in injured 6 pose
	case BOTH_INJURED6POINT_GENERAL:		//# Chang points to door while in injured state
	case BOTH_KNOCKDOWN1_GENERAL:		//# 
	case BOTH_KNOCKDOWN2_GENERAL:		//# 
		return qtrue;
		break;
	}

	return qfalse;
}

qboolean  PMove::PM_InRollComplete( playerState_t *ps, int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_ROLL_F_GENERAL:
	case BOTH_ROLL_B_GENERAL:
	case BOTH_ROLL_R_GENERAL:
	case BOTH_ROLL_L_GENERAL:
		if ( ps->legsTimer < 1 )
		{
			return qtrue;
		}
		break;
	}
	return qfalse;
}

int  PMove::PM_AnimLength( int index, animNumberGeneral_t anim )
{
	if (anim >= MAX_ANIMATIONS_GENERAL)
	{
		return -1;
	}
	return pm->animations[anim].numFrames * abs(pm->animations[anim].frameLerp);
}

//void  PMove::PM_DebugLegsAnim(int anim)
//{
//	int oldAnim = (pm->ps->legsAnim & ~ANIM_TOGGLEBIT);
//	int newAnim = (anim & ~ANIM_TOGGLEBIT);
//
//	if (oldAnim < MAX_TOTALANIMATIONS_GENERAL && oldAnim >= BOTH_DEATH1_GENERAL &&
//		newAnim < MAX_TOTALANIMATIONS_GENERAL && newAnim >= BOTH_DEATH1_GENERAL)
//	{
//		Com_Printf("OLD: %s\n", animTable[oldAnim].name);
//		Com_Printf("NEW: %s\n", animTable[newAnim].name);
//	}
//}
/*
==============================================================================
END: Animation utility functions (sequence checking)
==============================================================================
*/

/*
======================
BG_ParseAnimationFile

Read a configuration file containing animation coutns and rates
models/players/visor/animation.cfg, etc

======================
*/
//char		BGPAFtext[40000];
//qboolean	BGPAFtextLoaded = qfalse;
animation_t	bgGlobalAnimations[MAX_TOTALANIMATIONS_GENERAL] = {
{ 0 , 2 , -1 , 35 , 35 , 0 , 0 } , // BOTH_1CRUFTFORGIL
{ 8 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_TALK0
{ 17268 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_TALK1
{ 17270 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_TALK2
{ 17272 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_TALK3
{ 17274 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_TALK4
{ 17260 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_ALERT
{ 17266 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_SMILE
{ 17264 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_FROWN
{ 17262 , 2 , -1 , 1000 , 1000 , 0 , 0 } , // FACE_DEAD
{ 3625 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH1
{ 4511 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH2
{ 4798 , 68 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH3
{ 4866 , 39 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEATH4
{ 4905 , 41 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEATH5
{ 4946 , 53 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH6
{ 4999 , 68 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH7
{ 5067 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH8
{ 5106 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH9
{ 3666 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH10
{ 3727 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH11
{ 3788 , 53 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH12
{ 3841 , 55 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH13
{ 3896 , 50 , -1 , 25 , 25 , 0 , 0 } , // BOTH_DEATH14
{ 4092 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH15
{ 4153 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH16
{ 4194 , 125 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH17
{ 4319 , 96 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH18
{ 4415 , 96 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH19
{ 4540 , 64 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH20
{ 4604 , 64 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH21
{ 4668 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH22
{ 4699 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH23
{ 4730 , 34 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH24
{ 4764 , 34 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH25
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEATHFORWARD1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEATHFORWARD2
{ 5238 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATHFORWARD3
{ 5167 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATHBACKWARD1
{ 5208 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATHBACKWARD2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEATH1IDLE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LYINGDEATH1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STUMBLEDEATH1
{ 6036 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FALLDEATH1
{ 6062 , 17 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FALLDEATH1INAIR
{ 6079 , 32 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FALLDEATH1LAND
{ 4866 , 39 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEATH_ROLL
{ 4866 , 39 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEATH_FLIP
{ 4949 , 50 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_SPIN_90_R
{ 3790 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_SPIN_90_L
{ 3792 , 49 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_SPIN_180
{ 3614 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_LYING_UP
{ 3603 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_LYING_DN
{ 3711 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_FALLING_DN
{ 4518 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_FALLING_UP
{ 4232 , 85 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH_CROUCHED
{ 3665 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD1
{ 4539 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD2
{ 4865 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD3
{ 4904 , 1 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEAD4
{ 4945 , 1 , -1 , 34 , 34 , 0 , 0 } , // BOTH_DEAD5
{ 4998 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD6
{ 5066 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD7
{ 5105 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD8
{ 5166 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD9
{ 3726 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD10
{ 3787 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD11
{ 3840 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD12
{ 3895 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD13
{ 3945 , 1 , -1 , 25 , 25 , 0 , 0 } , // BOTH_DEAD14
{ 4152 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD15
{ 4193 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD16
{ 4318 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD17
{ 4414 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD18
{ 4510 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD19
{ 4603 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD20
{ 4667 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD21
{ 4698 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD22
{ 4729 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD23
{ 4763 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD24
{ 4797 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEAD25
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFORWARD1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFORWARD2
{ 5207 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEADBACKWARD1
{ 5237 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEADBACKWARD2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LYINGDEAD1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STUMBLEDEAD1
{ 6110 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FALLDEAD1LAND
{ 3603 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEADFLOP1
{ 3614 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEADFLOP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFLOP3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFLOP4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFLOP5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFORWARD1_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADFORWARD2_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADBACKWARD1_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEADBACKWARD2_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LYINGDEAD1_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STUMBLEDEAD1_FLOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FALLDEAD1_FLOP
{ 5330 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_HEAD1
{ 5340 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_TORSO1
{ 5334 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_LLEG
{ 5338 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_RLEG
{ 5336 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_RARM
{ 5332 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DISMEMBER_LARM
{ 9344 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN1
{ 9401 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PAIN2
{ 9420 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PAIN3
{ 9439 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN4
{ 9445 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN5
{ 9451 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN6
{ 9457 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN7
{ 9462 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN8
{ 9468 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN9
{ 9350 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN10
{ 9356 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN11
{ 9362 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN12
{ 9367 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN13
{ 9372 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN14
{ 9378 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN15
{ 9383 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN16
{ 9389 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN17
{ 10482 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN18
{ 9395 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN19
{ 4194 , 10 , 0 , 50 , 50 , 0 , 0 } , // BOTH_PAIN20
{ 86 , 28 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ATTACK1
{ 532 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ATTACK2IDLE1
{ 538 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK3
{ 544 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK4
{ 270 , 36 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ATTACK5
{ 306 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK6
{ 328 , 90 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ATTACK7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ATTACK8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ATTACK9
{ 512 , 20 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ATTACK10
{ 1561 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK11
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ATTACK12
{ 9285 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MELEE1
{ 9293 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MELEE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_MELEE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_MELEE4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_MELEE5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_MELEE6
{ 512 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THERMAL_READY
{ 516 , 12 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THERMAL_THROW
{ 26 , 8 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A1_T__B_
{ 34 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A1__L__R
{ 40 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A1__R__L
{ 14 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A1_TL_BR
{ 8 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A1_BR_TL
{ 2 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A1_BL_TR
{ 20 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A1_TR_BL
{ 13389 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR__R
{ 13346 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR_TL
{ 13376 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR__L
{ 13333 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR_BL
{ 13524 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__R_TR
{ 13514 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__R_TL
{ 13535 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__R__L
{ 13501 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__R_BL
{ 13359 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_TR_BR
{ 13413 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_TR_TL
{ 13440 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TR__L
{ 13424 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TR_BL
{ 13366 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_T__BR
{ 13528 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_T___R
{ 13436 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_T__TR
{ 13460 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_T__TL
{ 13464 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_T___L
{ 13450 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_T__BL
{ 13400 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TL_BR
{ 13393 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TL_BL
{ 13475 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__L_BR
{ 13488 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__L__R
{ 13420 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1__L_TL
{ 13294 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BL_BR
{ 13320 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BL__R
{ 13307 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BL_TR
{ 13471 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_BL__L
{ 13359 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR_TR
{ 13366 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_BR_T_
{ 13389 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1__R_BR
{ 13528 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__R_T_
{ 13524 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_TR__R
{ 13436 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TR_T_
{ 13514 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_TL__R
{ 13413 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TL_TR
{ 13460 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_TL_T_
{ 13420 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1_TL__L
{ 13440 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1__L_TR
{ 13464 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1__L_T_
{ 13471 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T1__L_BL
{ 13450 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_BL_T_
{ 13393 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T1_BL_TL
{ 10813 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1_T_
{ 10819 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1__L
{ 10823 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1__R
{ 10798 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1_TL
{ 10792 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1_BR
{ 10787 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1_BL
{ 10806 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S1_TR
{ 9579 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R1_B__S1
{ 9599 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R1__L_S1
{ 9605 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R1__R_S1
{ 9584 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R1_TL_S1
{ 9573 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R1_BR_S1
{ 9567 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R1_BL_S1
{ 9591 , 8 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R1_TR_S1
{ 8 , 1 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1_BR___
{ 40 , 2 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1__R___
{ 20 , 1 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1_TR___
{ 26 , 2 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1_T____
{ 14 , 1 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1_TL___
{ 34 , 2 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1__L___
{ 2 , 1 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B1_BL___
{ 19 , 1 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D1_BR___
{ 38 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D1__R___
{ 7 , 1 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D1_TR___
{ 13 , 1 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D1_TL___
{ 45 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D1__L___
{ 25 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D1_BL___
{ 32 , 2 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D1_B____
{ 155 , 11 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2_T__B_
{ 166 , 17 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2__L__R
{ 183 , 16 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2__R__L
{ 117 , 19 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2_TL_BR
{ 63 , 13 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2_BR_TL
{ 47 , 16 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2_BL_TR
{ 136 , 19 , -1 , 20 , 20 , 0 , 0 } , // BOTH_A2_TR_BL
{ 13676 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BR__R
{ 13634 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BR_TL
{ 13656 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BR__L
{ 13614 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BR_BL
{ 13818 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__R_TR
{ 13809 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__R_TL
{ 13831 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__R__L
{ 13789 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__R_BL
{ 13716 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TR_BR
{ 13723 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TR_TL
{ 13736 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TR__L
{ 13704 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TR_BL
{ 13647 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T__BR
{ 13824 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T___R
{ 13730 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T__TR
{ 13698 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T__TL
{ 13770 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T___L
{ 13580 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_T__BL
{ 13687 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TL_BR
{ 13682 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TL_BL
{ 13745 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__L_BR
{ 13777 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__L__R
{ 13765 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__L_TL
{ 13548 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BL_BR
{ 13594 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BL__R
{ 13569 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BL_TR
{ 13589 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BL__L
{ 13716 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_BR_TR
{ 13647 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BR_T_
{ 13676 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2__R_BR
{ 13824 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__R_T_
{ 13818 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_TR__R
{ 13730 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TR_T_
{ 13809 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_TL__R
{ 13723 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_TL_TR
{ 13698 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_TL_T_
{ 13765 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_TL__L
{ 13736 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2__L_TR
{ 13770 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2__L_T_
{ 13589 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2__L_BL
{ 13580 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T2_BL_T_
{ 13682 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T2_BL_TL
{ 10904 , 13 , -1 , 20 , 20 , 0 , 0 } , // BOTH_S2_S1_T_
{ 10917 , 22 , -1 , 12 , 12 , 0 , 0 } , // BOTH_S2_S1__L
{ 10939 , 19 , -1 , 14 , 14 , 0 , 0 } , // BOTH_S2_S1__R
{ 10862 , 33 , -1 , 8 , 8 , 0 , 0 } , // BOTH_S2_S1_TL
{ 10851 , 11 , -1 , 23 , 23 , 0 , 0 } , // BOTH_S2_S1_BR
{ 10829 , 22 , -1 , 12 , 12 , 0 , 0 } , // BOTH_S2_S1_BL
{ 10895 , 9 , -1 , 28 , 28 , 0 , 0 } , // BOTH_S2_S1_TR
{ 9660 , 62 , -1 , 13 , 13 , 0 , 0 } , // BOTH_R2_B__S1
{ 9765 , 13 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2__L_S1
{ 9778 , 15 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2__R_S1
{ 9722 , 20 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2_TL_S1
{ 9633 , 27 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2_BR_S1
{ 9614 , 19 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2_BL_S1
{ 9742 , 23 , -1 , 25 , 25 , 0 , 0 } , // BOTH_R2_TR_S1
{ 63 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2_BR___
{ 183 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2__R___
{ 136 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2_TR___
{ 155 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2_T____
{ 117 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2_TL___
{ 166 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2__L___
{ 47 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B2_BL___
{ 132 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2_BR___
{ 179 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2__R___
{ 59 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2_TR___
{ 72 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2_TL___
{ 195 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2__L___
{ 151 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2_BL___
{ 162 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D2_B____
{ 246 , 27 , -1 , 25 , 25 , 0 , 0 } , // BOTH_A3_T__B_
{ 273 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3__L__R
{ 286 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3__R__L
{ 223 , 14 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3_TL_BR
{ 211 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3_BR_TL
{ 199 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3_BL_TR
{ 237 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A3_TR_BL
{ 13971 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BR__R
{ 13929 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BR_TL
{ 13951 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BR__L
{ 13909 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BR_BL
{ 14113 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__R_TR
{ 14104 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__R_TL
{ 14126 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__R__L
{ 14084 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__R_BL
{ 14011 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TR_BR
{ 14018 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TR_TL
{ 14031 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TR__L
{ 13999 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TR_BL
{ 13942 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T__BR
{ 14119 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T___R
{ 14025 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T__TR
{ 13993 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T__TL
{ 14065 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T___L
{ 13875 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_T__BL
{ 13982 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TL_BR
{ 13977 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TL_BL
{ 14040 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__L_BR
{ 14072 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__L__R
{ 14060 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__L_TL
{ 13843 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BL_BR
{ 13889 , 20 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BL__R
{ 13864 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BL_TR
{ 13884 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BL__L
{ 14011 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_BR_TR
{ 13942 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BR_T_
{ 13971 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3__R_BR
{ 14119 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__R_T_
{ 14113 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_TR__R
{ 14025 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TR_T_
{ 14104 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_TL__R
{ 14018 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_TL_TR
{ 13993 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_TL_T_
{ 14060 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_TL__L
{ 14031 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3__L_TR
{ 14065 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3__L_T_
{ 13884 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3__L_BL
{ 13875 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T3_BL_T_
{ 13977 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T3_BL_TL
{ 11036 , 45 , -1 , 17 , 17 , 0 , 0 } , // BOTH_S3_S1_T_
{ 11081 , 26 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1__L
{ 11107 , 23 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1__R
{ 10998 , 23 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1_TL
{ 10980 , 18 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1_BR
{ 10958 , 22 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1_BL
{ 11021 , 15 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S3_S1_TR
{ 9863 , 49 , -1 , 17 , 17 , 0 , 0 } , // BOTH_R3_B__S1
{ 9993 , 39 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3__L_S1
{ 10032 , 29 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3__R_S1
{ 9912 , 44 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3_TL_S1
{ 9830 , 33 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3_BR_S1
{ 9793 , 37 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3_BL_S1
{ 9956 , 37 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R3_TR_S1
{ 211 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3_BR___
{ 286 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3__R___
{ 237 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3_TR___
{ 246 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3_T____
{ 223 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3_TL___
{ 273 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3__L___
{ 199 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B3_BL___
{ 231 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3_BR___
{ 282 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3__R___
{ 207 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3_TR___
{ 219 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3_TL___
{ 291 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3__L___
{ 241 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D3_BL___
{ 269 , 4 , -1 , 25 , 25 , 0 , 0 } , // BOTH_D3_B____
{ 331 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A4_T__B_
{ 338 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4__L__R
{ 346 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4__R__L
{ 313 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4_TL_BR
{ 304 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4_BR_TL
{ 295 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4_BL_TR
{ 322 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A4_TR_BL
{ 14237 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BR__R
{ 14202 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BR_TL
{ 14224 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4_BR__L
{ 14189 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4_BR_BL
{ 14379 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__R_TR
{ 14370 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__R_TL
{ 14392 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__R__L
{ 14357 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4__R_BL
{ 14277 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TR_BR
{ 14284 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TR_TL
{ 14297 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TR__L
{ 14265 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TR_BL
{ 14215 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T__BR
{ 14385 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T___R
{ 14291 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T__TR
{ 14259 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T__TL
{ 14324 , 21 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T___L
{ 14162 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_T__BL
{ 14248 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TL_BR
{ 14243 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TL_BL
{ 14306 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4__L_BR
{ 14345 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__L__R
{ 14319 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__L_TL
{ 14138 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4_BL_BR
{ 14176 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T4_BL__R
{ 14151 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BL_TR
{ 14171 , 5 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BL__L
{ 14277 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_BR_TR
{ 14215 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BR_T_
{ 14237 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4__R_BR
{ 14385 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__R_T_
{ 14379 , 6 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_TR__R
{ 14291 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TR_T_
{ 14370 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_TL__R
{ 14284 , 7 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_TL_TR
{ 14259 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_TL_T_
{ 14319 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_TL__L
{ 14297 , 9 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4__L_TR
{ 14324 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4__L_T_
{ 14171 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4__L_BL
{ 14162 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_T4_BL_T_
{ 14243 , 5 , -1 , -34 , 34 , 0 , 0 } , // BOTH_T4_BL_TL
{ 11164 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_S4_S1_T_
{ 11176 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1__L
{ 11187 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1__R
{ 11145 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1_TL
{ 11139 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1_BR
{ 11130 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1_BL
{ 11154 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S4_S1_TR
{ 10085 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R4_B__S1
{ 10124 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4__L_S1
{ 10135 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4__R_S1
{ 10098 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4_TL_S1
{ 10075 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4_BR_S1
{ 10061 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4_BL_S1
{ 10113 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R4_TR_S1
{ 304 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4_BR___
{ 346 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4__R___
{ 322 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4_TR___
{ 331 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4_T____
{ 313 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4_TL___
{ 338 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4__L___
{ 295 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B4_BL___
{ 318 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4_BR___
{ 342 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4__R___
{ 300 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4_TR___
{ 309 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4_TL___
{ 351 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4__L___
{ 327 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D4_BL___
{ 334 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D4_B____
{ 390 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5_T__B_
{ 401 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5__L__R
{ 408 , 7 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5__R__L
{ 376 , 6 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5_TL_BR
{ 364 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5_BR_TL
{ 355 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5_BL_TR
{ 382 , 8 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A5_TR_BL
{ 14499 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR__R
{ 14456 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR_TL
{ 14486 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR__L
{ 14443 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR_BL
{ 14634 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__R_TR
{ 14624 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__R_TL
{ 14645 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__R__L
{ 14611 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__R_BL
{ 14469 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_TR_BR
{ 14523 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_TR_TL
{ 14550 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TR__L
{ 14534 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TR_BL
{ 14476 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_T__BR
{ 14638 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_T___R
{ 14546 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_T__TR
{ 14570 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_T__TL
{ 14574 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_T___L
{ 14560 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_T__BL
{ 14510 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TL_BR
{ 14503 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TL_BL
{ 14585 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__L_BR
{ 14598 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__L__R
{ 14530 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5__L_TL
{ 14404 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BL_BR
{ 14430 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BL__R
{ 14417 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BL_TR
{ 14581 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_BL__L
{ 14469 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR_TR
{ 14476 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_BR_T_
{ 14499 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5__R_BR
{ 14638 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__R_T_
{ 14634 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_TR__R
{ 14546 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TR_T_
{ 14624 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_TL__R
{ 14523 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TL_TR
{ 14570 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_TL_T_
{ 14530 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5_TL__L
{ 14550 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5__L_TR
{ 14574 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5__L_T_
{ 14581 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T5__L_BL
{ 14560 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_BL_T_
{ 14503 , 7 , -1 , -50 , 50 , 0 , 0 } , // BOTH_T5_BL_TL
{ 11228 , 7 , -1 , 15 , 15 , 0 , 0 } , // BOTH_S5_S1_T_
{ 11235 , 8 , -1 , 13 , 13 , 0 , 0 } , // BOTH_S5_S1__L
{ 11243 , 11 , -1 , 10 , 10 , 0 , 0 } , // BOTH_S5_S1__R
{ 11210 , 9 , -1 , 12 , 12 , 0 , 0 } , // BOTH_S5_S1_TL
{ 11206 , 4 , -1 , 25 , 25 , 0 , 0 } , // BOTH_S5_S1_BR
{ 11199 , 7 , -1 , 15 , 15 , 0 , 0 } , // BOTH_S5_S1_BL
{ 11219 , 9 , -1 , 12 , 12 , 0 , 0 } , // BOTH_S5_S1_TR
{ 10174 , 9 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5_B__S1
{ 10209 , 11 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5__L_S1
{ 10220 , 14 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5__R_S1
{ 10183 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5_TL_S1
{ 10161 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5_BR_S1
{ 10147 , 14 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5_BL_S1
{ 10196 , 13 , -1 , 34 , 34 , 0 , 0 } , // BOTH_R5_TR_S1
{ 364 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5_BR___
{ 408 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5__R___
{ 382 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5_TR___
{ 390 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5_T____
{ 376 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5_TL___
{ 401 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5__L___
{ 355 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B5_BL___
{ 378 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5_BR___
{ 404 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5__R___
{ 360 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5_TR___
{ 372 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5_TL___
{ 411 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5__L___
{ 386 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5_BL___
{ 397 , 4 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D5_B____
{ 745 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_T__B_
{ 752 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6__L__R
{ 759 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6__R__L
{ 730 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_TL_BR
{ 600 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_BR_TL
{ 593 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_BL_TR
{ 738 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_TR_BL
{ 17697 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR__R
{ 17657 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR_TL
{ 17684 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR__L
{ 17644 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR_BL
{ 17964 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R_TR
{ 17951 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R_TL
{ 17976 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R__L
{ 17930 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R_BL
{ 17772 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR_BR
{ 17776 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR_TL
{ 17797 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR__L
{ 17760 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR_BL
{ 17830 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T__BR
{ 17863 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T___R
{ 17850 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T__TR
{ 17837 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T__TL
{ 17854 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T___L
{ 17817 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_T__BL
{ 17706 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL_BR
{ 17701 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL_BL
{ 17874 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L_BR
{ 17917 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L__R
{ 17887 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L_TL
{ 17581 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL_BR
{ 17631 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL__R
{ 17601 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL_TR
{ 17627 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL__L
{ 17670 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR_TR
{ 17676 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BR_T_
{ 17943 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R_BR
{ 17968 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__R_T_
{ 17810 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR__R
{ 17789 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TR_T_
{ 17747 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL__R
{ 17719 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL_TR
{ 17732 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL_T_
{ 17741 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_TL__L
{ 17891 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L_TR
{ 17904 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L_T_
{ 17870 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6__L_BL
{ 17614 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL_T_
{ 17594 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T6_BL_TL
{ 12981 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6_T_
{ 12987 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6__L
{ 12993 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6__R
{ 12969 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6_TL
{ 12962 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6_BR
{ 12956 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6_BL
{ 12975 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S6_S6_TR
{ 11786 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6_B__S6
{ 11810 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6__L_S6
{ 11818 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6__R_S6
{ 11794 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6_TL_S6
{ 11779 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6_BR_S6
{ 11771 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6_BL_S6
{ 11802 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R6_TR_S6
{ 600 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6_BR___
{ 759 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6__R___
{ 738 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6_TR___
{ 745 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6_T____
{ 730 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6_TL___
{ 752 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6__L___
{ 593 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B6_BL___
{ 735 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6_BR___
{ 756 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6__R___
{ 597 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6_TR___
{ 604 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6_TL___
{ 763 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6__L___
{ 742 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6_BL___
{ 749 , 3 , -1 , 34 , 34 , 0 , 0 } , // BOTH_D6_B____
{ 1136 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_T__B_
{ 1143 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7__L__R
{ 1150 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7__R__L
{ 1122 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_TL_BR
{ 773 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_BR_TL
{ 766 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_BL_TR
{ 1129 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_TR_BL
{ 18091 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR__R
{ 18054 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR_TL
{ 18078 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR__L
{ 18041 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR_BL
{ 18318 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R_TR
{ 18309 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R_TL
{ 18329 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R__L
{ 18292 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R_BL
{ 18155 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR_BR
{ 18159 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR_TL
{ 18175 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR__L
{ 18146 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR_BL
{ 18196 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T__BR
{ 18227 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T___R
{ 18213 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T__TR
{ 18209 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T__TL
{ 18222 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T___L
{ 18191 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_T__BL
{ 18098 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL_BR
{ 18094 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL_BL
{ 18246 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L_BR
{ 18279 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L__R
{ 18259 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L_TL
{ 17989 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL_BR
{ 18028 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL__R
{ 18006 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL_TR
{ 18024 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL__L
{ 18067 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR_TR
{ 18070 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BR_T_
{ 18305 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R_BR
{ 18321 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__R_T_
{ 18188 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR__R
{ 18166 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TR_T_
{ 18133 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL__R
{ 18113 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL_TR
{ 18126 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL_T_
{ 18129 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_TL__L
{ 18262 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L_TR
{ 18275 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L_T_
{ 18242 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7__L_BL
{ 18019 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL_T_
{ 18002 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_T7_BL_TL
{ 13029 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7_T_
{ 13038 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7__L
{ 13044 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7__R
{ 13017 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7_TL
{ 13011 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7_BR
{ 13005 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7_BL
{ 13023 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S7_S7_TR
{ 11840 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7_B__S7
{ 11862 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7__L_S7
{ 11870 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7__R_S7
{ 11846 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7_TL_S7
{ 11832 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7_BR_S7
{ 11824 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7_BL_S7
{ 11854 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_R7_TR_S7
{ 773 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7_BR___
{ 1150 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7__R___
{ 1129 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7_TR___
{ 1136 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7_T____
{ 1122 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7_TL___
{ 1143 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7__L___
{ 766 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_B7_BL___
{ 1126 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7_BR___
{ 1147 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7__R___
{ 770 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7_TR___
{ 777 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7_TL___
{ 1153 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7__L___
{ 1133 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7_BL___
{ 1140 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_D7_B____
{ 9342 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P1_S1_T_
{ 9340 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P1_S1_TR
{ 9338 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P1_S1_TL
{ 9334 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P1_S1_BL
{ 9336 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P1_S1_BR
{ 8433 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_T_
{ 8419 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_TR
{ 8406 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_TL
{ 8371 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_BL
{ 8391 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_B_
{ 8381 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K1_S1_BR
{ 8394 , 12 , -1 , 100 , 100 , 0 , 0 } , // BOTH_V1_BR_S1
{ 7736 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V1__R_S1
{ 8423 , 10 , -1 , 100 , 100 , 0 , 0 } , // BOTH_V1_TR_S1
{ 8437 , 9 , -1 , 100 , 100 , 0 , 0 } , // BOTH_V1_T__S1
{ 8409 , 10 , -1 , 100 , 100 , 0 , 0 } , // BOTH_V1_TL_S1
{ 193 , 6 , -1 , 40 , 40 , 0 , 0 } , // BOTH_V1__L_S1
{ 8376 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_V1_BL_S1
{ 1443 , 10 , -1 , 67 , 67 , 0 , 0 } , // BOTH_V1_B__S1
{ 7780 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_T_
{ 7756 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_TR
{ 7732 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_TL
{ 7660 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_BL
{ 7708 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_B_
{ 7684 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H1_S1_BR
{ 10419 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P6_S6_T_
{ 10417 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P6_S6_TR
{ 10415 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P6_S6_TL
{ 10411 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P6_S6_BL
{ 10413 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P6_S6_BR
{ 8274 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_T_
{ 8259 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_TR
{ 8251 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_TL
{ 8221 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_BL
{ 8240 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_B_
{ 8232 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K6_S6_BR
{ 18840 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_BR_S6
{ 18900 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6__R_S6
{ 18870 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_TR_S6
{ 18880 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_T__S6
{ 18860 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_TL_S6
{ 18890 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6__L_S6
{ 18830 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_BL_S6
{ 18850 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V6_B__S6
{ 7507 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_T_
{ 7499 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_TR
{ 7491 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_TL
{ 7467 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_BL
{ 7483 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_B_
{ 7475 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H6_S6_BR
{ 10429 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P7_S7_T_
{ 10427 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P7_S7_TR
{ 10425 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P7_S7_TL
{ 10421 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P7_S7_BL
{ 10423 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_P7_S7_BR
{ 8353 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_T_
{ 8339 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_TR
{ 8325 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_TL
{ 8286 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_BL
{ 8314 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_B_
{ 8300 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_K7_S7_BR
{ 18923 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_BR_S7
{ 18996 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7__R_S7
{ 18960 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_TR_S7
{ 18973 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_T__S7
{ 18948 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_TL_S7
{ 18984 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7__L_S7
{ 18910 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_BL_S7
{ 18935 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_V7_B__S7
{ 7577 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_T_
{ 7565 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_TR
{ 7553 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_TL
{ 7517 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_BL
{ 7541 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_B_
{ 7529 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_H7_S7_BR
{ 9920 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_DL_S_B_1_L
{ 9926 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_DL_S_B_1_W
{ 9932 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_S_L_1
{ 9963 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_S_SB_1_L
{ 9973 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_S_SB_1_W
{ 10004 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_DL_T_B_1_L
{ 10010 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_DL_T_B_1_W
{ 10016 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_T_L_1
{ 10047 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_T_SB_1_L
{ 10072 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_DL_T_SB_1_W
{ 10103 , 9 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_ST_S_B_1_L
{ 10112 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_ST_S_B_1_W
{ 10118 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_S_L_1
{ 10149 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_S_SB_1_L
{ 10160 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_S_SB_1_W
{ 10176 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_ST_T_B_1_L
{ 10182 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_ST_T_B_1_W
{ 10188 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_T_L_1
{ 10219 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_T_SB_1_L
{ 10225 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_ST_T_SB_1_W
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_S_S_B_1_L
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_S_S_B_1_W
{ 3090 , 31 , -1 , 34 , 34 , 0 , 0 } , // BOTH_LK_S_S_S_L_1
{ 10241 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_S_S_SB_1_L
{ 9973 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_S_S_SB_1_W
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_S_T_B_1_L
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_S_S_T_B_1_W
{ 1807 , 45 , -1 , 34 , 34 , 0 , 0 } , // BOTH_LK_S_S_T_L_1
{ 10252 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_S_T_SB_1_L
{ 10225 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_S_S_T_SB_1_W
{ 9376 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_DL_S_B_1_L
{ 9382 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_DL_S_B_1_W
{ 9388 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_S_L_1
{ 9091 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_S_SB_1_L
{ 9187 , 38 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_S_SB_1_W
{ 9461 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_DL_T_B_1_L
{ 9467 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_DL_T_B_1_W
{ 9473 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_T_L_1
{ 9115 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_T_SB_1_L
{ 9231 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_DL_T_SB_1_W
{ 9131 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_ST_S_B_1_L
{ 9137 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_ST_S_B_1_W
{ 9143 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_ST_S_L_1
{ 9174 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_ST_S_SB_1_L
{ 9187 , 38 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_ST_S_SB_1_W
{ 9225 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_ST_T_B_1_L
{ 9231 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_ST_T_B_1_W
{ 9237 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_ST_T_L_1
{ 9268 , 71 , -1 , 35 , 35 , 0 , 0 } , // BOTH_LK_DL_ST_T_SB_1_L
{ 9339 , 37 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_ST_T_SB_1_W
{ 9376 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_S_S_B_1_L
{ 9382 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_S_S_B_1_W
{ 9388 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_S_L_1
{ 9419 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_S_SB_1_L
{ 9430 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_S_SB_1_W
{ 9461 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_S_T_B_1_L
{ 9467 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_DL_S_T_B_1_W
{ 9473 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_T_L_1
{ 9504 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_T_SB_1_L
{ 9517 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_S_T_SB_1_W
{ 9553 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_DL_S_B_1_L
{ 9559 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_DL_S_B_1_W
{ 9565 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_DL_S_L_1
{ 9596 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_DL_S_SB_1_L
{ 9617 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_DL_S_SB_1_W
{ 9627 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_DL_T_B_1_L
{ 9633 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_DL_T_B_1_W
{ 9639 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_DL_T_L_1
{ 9670 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_DL_T_SB_1_L
{ 9689 , 33 , -1 , 35 , 35 , 0 , 0 } , // BOTH_LK_ST_DL_T_SB_1_W
{ 9779 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_ST_S_B_1_L
{ 9722 , 9 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_ST_S_B_1_W
{ 9565 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_S_L_1
{ 9731 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_S_SB_1_L
{ 9829 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_S_SB_1_W
{ 9860 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_ST_T_B_1_L
{ 9866 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_ST_T_B_1_W
{ 9742 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_T_L_1
{ 9773 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_T_SB_1_L
{ 9914 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_T_SB_1_W
{ 9779 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_S_S_B_1_L
{ 9785 , 9 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_S_S_B_1_W
{ 9794 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_S_L_1
{ 9825 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_S_SB_1_L
{ 9829 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_S_SB_1_W
{ 9860 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_S_T_B_1_L
{ 9866 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LK_ST_S_T_B_1_W
{ 9872 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_T_L_1
{ 9903 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_T_SB_1_L
{ 9914 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_S_T_SB_1_W
{ 2250 , 31 , -1 , 34 , 34 , 0 , 0 } , // BOTH_LK_S_S_S_L_2
{ 1702 , 45 , -1 , 34 , 34 , 0 , 0 } , // BOTH_LK_S_S_T_L_2
{ 9388 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_S_L_2
{ 9473 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_DL_DL_T_L_2
{ 9565 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_S_L_2
{ 9742 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LK_ST_ST_T_L_2
{ 1100 , 12 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF2RETURN
{ 1017 , 38 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF2BREAK
{ 1055 , 45 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF2LOCK
{ 995 , 22 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF1RETURN
{ 922 , 28 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF1BREAK
{ 950 , 45 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BF1LOCK
{ 3576 , 27 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CWCIRCLE_R2__R_S1
{ 1499 , 27 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CCWCIRCLE_R2__L_S1
{ 3561 , 15 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CWCIRCLE_A2__L__R
{ 1484 , 15 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CCWCIRCLE_A2__R__L
{ 3520 , 10 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CWCIRCLEBREAK
{ 1443 , 10 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CCWCIRCLEBREAK
{ 3530 , 31 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CWCIRCLELOCK
{ 1453 , 31 , -1 , 34 , 34 , 0 , 0 } , // BOTH_CCWCIRCLELOCK
{ 11254 , 2 , -1 , 17 , 17 , 0 , 0 } , // BOTH_SABERFAST_STANCE
{ 11325 , 2 , -1 , 17 , 17 , 0 , 0 } , // BOTH_SABERSLOW_STANCE
{ 13050 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERDUAL_STANCE
{ 13125 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERSTAFF_STANCE
{ 76 , 41 , -1 , 34 , 34 , 0 , 0 } , // BOTH_A2_STABBACK1
{ 550 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ATTACK_BACK
{ 8253 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMPFLIPSLASHDOWN1
{ 8293 , 59 , -1 , 34 , 34 , 0 , 0 } , // BOTH_JUMPFLIPSTABDOWN
{ 6904 , 45 , -1 , 34 , 34 , 0 , 0 } , // BOTH_FORCELEAP2_T__B_
{ 9261 , 24 , -1 , 34 , 34 , 0 , 0 } , // BOTH_LUNGE2_B__T_
{ 3479 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CROUCHATTACKBACK1
{ 7959 , 42 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMPATTACK6
{ 8001 , 18 , -1 , 100 , 100 , 0 , 0 } , // BOTH_JUMPATTACK7
{ 14358 , 41 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SPINATTACK6
{ 14399 , 46 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SPINATTACK7
{ 12495 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S6
{ 12945 , 11 , -1 , 112 , 112 , 0 , 0 } , // BOTH_S6_S1
{ 12506 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_S1_S7
{ 12999 , 6 , -1 , 200 , 200 , 0 , 0 } , // BOTH_S7_S1
{ 5932 , 53 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELONGLEAP_START
{ 5893 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELONGLEAP_ATTACK
{ 5914 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELONGLEAP_LAND
{ 6096 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRUNFLIP_START
{ 6077 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRUNFLIP_END
{ 6058 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRUNFLIP_ALT
{ 6022 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLREBOUND_FORWARD
{ 6034 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLREBOUND_LEFT
{ 6010 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLREBOUND_BACK
{ 6046 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLREBOUND_RIGHT
{ 6027 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLHOLD_FORWARD
{ 6039 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLHOLD_LEFT
{ 6015 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLHOLD_BACK
{ 6051 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLHOLD_RIGHT
{ 6028 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRELEASE_FORWARD
{ 6040 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRELEASE_LEFT
{ 6016 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRELEASE_BACK
{ 6052 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEWALLRELEASE_RIGHT
{ 907 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_F
{ 809 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_B
{ 965 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_R
{ 936 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_L
{ 1008 , 34 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_S
{ 823 , 48 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_BF
{ 871 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_BF_STOP
{ 979 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_RL
{ 921 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_F_AIR
{ 892 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_B_AIR
{ 993 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_R_AIR
{ 950 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_KICK_L_AIR
{ 5335 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_ATTACK7
{ 5429 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_HOLD7
{ 5486 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_LAND
{ 11042 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PULL_IMPALE_STAB
{ 11073 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PULL_IMPALE_SWING
{ 11012 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PULLED_INAIR_B
{ 11027 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PULLED_INAIR_F
{ 14445 , 35 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STABDOWN
{ 14515 , 35 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STABDOWN_STAFF
{ 14480 , 35 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STABDOWN_DUAL
{ 649 , 81 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_SABERPROTECT
{ 1042 , 80 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_SOULCAL
{ 30 , 57 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A1_SPECIAL
{ 149 , 56 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A2_SPECIAL
{ 352 , 49 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A3_SPECIAL
{ 12225 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ROLL_STAB
{ 12257 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND1_RANDOM1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND1_RANDOM2
{ 12259 , 150 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1IDLE1
{ 12446 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND2_RANDOM1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND2_RANDOM2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND2_RANDOM3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND2_RANDOM4
{ 12448 , 151 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND2IDLE1
{ 12599 , 75 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND2IDLE2
{ 12674 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND3
{ 15298 , 151 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND3IDLE1
{ 12676 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND4IDLE1
{ 12684 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND5
{ 12686 , 150 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5IDLE1
{ 9285 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND6
{ 14894 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND7
{ 13029 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND1TO3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND3TO1
{ 12409 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1TO2
{ 12409 , 16 , -1 , -67 , 67 , 0 , 0 } , // BOTH_STAND2TO1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND2TO4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STAND4TO2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STANDTOWALK1
{ 12678 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND4TOATTACK2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STANDUP1
{ 13107 , 65 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STANDUP2
{ 12928 , 101 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5TOSIT3
{ 12444 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_STAND1_REELO
{ 13089 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5_REELO
{ 12425 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1TOSTAND5
{ 12425 , 19 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND5TOSTAND1
{ 12907 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5TOAIM
{ 12887 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5STARTLEDLOOKLEFT
{ 13172 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STARTLEDLOOKLEFTTOSTAND5
{ 13029 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5TOSTAND8
{ 13091 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND7TOSTAND8
{ 13029 , 40 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND8TOSTAND5
{ 15878 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND9
{ 15880 , 150 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND9IDLE1
{ 12852 , 35 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5SHIFTWEIGHT
{ 12852 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5SHIFTWEIGHTSTART
{ 12865 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5SHIFTWEIGHTSTOP
{ 13069 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5TURNLEFTSTART
{ 13069 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND5TURNLEFTSTOP
{ 13079 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5TURNRIGHTSTART
{ 13079 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND5TURNRIGHTSTOP
{ 12836 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND5LOOK180LEFTSTART
{ 12836 , 16 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND5LOOK180LEFTSTOP
{ 3068 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSOLE1START
{ 2988 , 80 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CONSOLE1
{ 3087 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSOLE1STOP
{ 3384 , 3 , -1 , -200 , 200 , 0 , 0 } , // BOTH_CONSOLE2START
{ 3384 , 3 , -1 , -200 , 200 , 0 , 0 } , // BOTH_CONSOLE2
{ 3384 , 3 , -1 , -200 , 200 , 0 , 0 } , // BOTH_CONSOLE2STOP
{ 3167 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSOLE2HOLDCOMSTART
{ 3167 , 10 , -1 , -50 , 50 , 0 , 0 } , // BOTH_CONSOLE2HOLDCOMSTOP
{ 2425 , 91 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GUARD_LOOKAROUND1
{ 3384 , 3 , -1 , -200 , 200 , 0 , 0 } , // BOTH_GUARD_IDLE1
{ 0 , 86 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALERT1
{ 3388 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GESTURE1
{ 305 , 44 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GESTURE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_GESTURE3
{ 16708 , 28 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK1TALKCOMM1
{ 14658 , 59 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALK1
{ 370 , 179 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TALK2
{ 14717 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKCOMM1START
{ 14733 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TALKCOMM1
{ 14733 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKCOMM1STOP
{ 14745 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE1
{ 15751 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE2
{ 15941 , 48 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE3
{ 16150 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE4START
{ 15989 , 161 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE4
{ 16171 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE4STOP
{ 16192 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE5START
{ 16192 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE5
{ 16192 , 25 , -1 , -50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE5STOP
{ 16217 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE6START
{ 16258 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE6
{ 16258 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE6STOP
{ 16303 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE7START
{ 16271 , 32 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE7
{ 16303 , 29 , -1 , -50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE7STOP
{ 16332 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE8START
{ 16376 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE8
{ 16332 , 45 , -1 , -50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE8STOP
{ 16377 , 43 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE9
{ 14806 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE10
{ 14845 , 33 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE11START
{ 14878 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE11STOP
{ 14894 , 169 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE12
{ 15183 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE13START
{ 15063 , 120 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE13
{ 15202 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE13STOP
{ 15222 , 59 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE14
{ 15391 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE15START
{ 15281 , 110 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE15
{ 15406 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE15STOP
{ 15416 , 54 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE16
{ 15470 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE17
{ 15521 , 161 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE18
{ 15682 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE19START
{ 15727 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE19STOP
{ 15790 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE20START
{ 15821 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE21
{ 15840 , 33 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE22
{ 15873 , 68 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TALKGESTURE23
{ 3388 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PAUSE1START
{ 3388 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PAUSE1STOP
{ 3388 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HEADTILTLSTART
{ 3384 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_HEADTILTLSTOP
{ 3384 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_HEADTILTRSTART
{ 3384 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_HEADTILTRSTOP
{ 3384 , 3 , -1 , -100 , 100 , 0 , 0 } , // BOTH_HEADNOD
{ 7883 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HEADSHAKE
{ 7923 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HEADSHAKE1_REELO
{ 11769 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADTILTLSTART
{ 11779 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADTILTLSTOP
{ 11788 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADTILTRSTART
{ 11798 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADTILTRSTOP
{ 11698 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADNOD
{ 11729 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SITHEADSHAKE
{ 11577 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT2HEADTILTLSTART
{ 11577 , 5 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SIT2HEADTILTLSTOP
{ 10234 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_REACH1START
{ 10259 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_REACH1STOP
{ 5654 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE1START
{ 5493 , 161 , 0 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE1
{ 5675 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE1STOP
{ 5916 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE2START
{ 5706 , 210 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE2
{ 5937 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE2STOP
{ 5997 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE3START
{ 5953 , 44 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE3
{ 6017 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_EXAMINE3STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW1START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW1STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW2START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_THROW3
{ 9163 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LEANLEFT2START
{ 9185 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LEANLEFT2STOP
{ 9202 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LEANRIGHT3START
{ 9232 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_LEANRIGHT3
{ 9232 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LEANRIGHT3STOP
{ 6488 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEFOUNTAIN1_START
{ 6462 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEFOUNTAIN1_MIDDLE
{ 6431 , 31 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FORCEFOUNTAIN1_LOOP
{ 6519 , 116 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEFOUNTAIN1_STOP
{ 16456 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_THUMBING1
{ 2952 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COME_ON1
{ 13187 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_STEADYSELF1
{ 13187 , 21 , -1 , -34 , 34 , 0 , 0 } , // BOTH_STEADYSELF1END
{ 11544 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SILENCEGESTURE1
{ 10284 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_REACHFORSABER1
{ 9526 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PUNCHER1
{ 3312 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINER1HOLD
{ 3221 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINEE1HOLD
{ 3312 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINER1STAND
{ 3221 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINEE1STAND
{ 3343 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINER1WALK
{ 3252 , 41 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINEE1WALK
{ 3293 , 19 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINER1LOOP
{ 3202 , 19 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CONSTRAINEE1LOOP
{ 11287 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERKILLER1
{ 11256 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERKILLEE1
{ 7841 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANDSHAKER1START
{ 7828 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANDSHAKER1LOOP
{ 7817 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANDSHAKEE1START
{ 7804 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANDSHAKEE1LOOP
{ 9054 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LAUGH1START
{ 9072 , 91 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LAUGH1STOP
{ 5425 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ESCAPEPOD_LEAVE1
{ 5464 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ESCAPEPOD_LEAVE2
{ 8178 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGER1
{ 8199 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGERSTOP1
{ 8215 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGERSTOP2
{ 8128 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGEE1
{ 8149 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGEESTOP1
{ 8165 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HUGGEESTOP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_KISSER1
{ 8655 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSER1START1
{ 8674 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSER1START2
{ 8604 , 51 , 0 , 50 , 50 , 0 , 0 } , // BOTH_KISSER1LOOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_KISSER1STOP
{ 8700 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSER1STOP1
{ 8731 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSER1STOP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_KISSEE1
{ 8497 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSEE1START1
{ 8516 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSEE1START2
{ 8446 , 51 , 0 , 50 , 50 , 0 , 0 } , // BOTH_KISSEE1LOOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_KISSEE1STOP
{ 8542 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSEE1STOP1
{ 8573 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KISSEE1STOP2
{ 636 , 201 , 0 , 50 , 50 , 0 , 0 } , // BOTH_BARTENDER_IDLE1
{ 837 , 61 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BARTENDER_THROW1
{ 619 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BARTENDER_COWERSTART
{ 579 , 40 , 0 , 50 , 50 , 0 , 0 } , // BOTH_BARTENDER_COWERLOOP
{ 579 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BARTENDER_COWER
{ 16420 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_THREATEN1_START
{ 16420 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_THREATEN1
{ 532 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RADIO_ONOFF
{ 598 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TRIUMPHANT1START
{ 607 , 30 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TRIUMPHANT1STARTGESTURE
{ 598 , 9 , -1 , -50 , 50 , 0 , 0 } , // BOTH_TRIUMPHANT1STOP
{ 11327 , 53 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERTHROW1START
{ 11380 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERTHROW1STOP
{ 11409 , 96 , -1 , 25 , 25 , 0 , 0 } , // BOTH_SABERTHROW2START
{ 11505 , 39 , -1 , 25 , 25 , 0 , 0 } , // BOTH_SABERTHROW2STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COVERUP1_LOOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COVERUP1_START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COVERUP1_END
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED4TO5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT1STAND
{ 11573 , 2 , 0 , 17 , 17 , 0 , 0 } , // BOTH_SIT1
{ 11575 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SIT2
{ 11659 , 2 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SIT3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT2TO3
{ 11603 , 56 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT2TOSTAND5
{ 11603 , 56 , -1 , -50 , 50 , 0 , 0 } , // BOTH_STAND5TOSIT2
{ 11582 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT2TOSIT4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT3TO1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT3TO2
{ 11661 , 37 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT3TOSTAND5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT4TO5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT4TO6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT5TO4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT5TO6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT6TO4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT6TO5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SIT7TOSTAND1
{ 3384 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH1
{ 3388 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH1IDLE
{ 3390 , 24 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH1WALK
{ 3414 , 20 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH1WALKBACK
{ 3384 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_UNCROUCH1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CROUCH2IDLE
{ 37 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH2TOSTAND1
{ 3434 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CROUCH3
{ 3434 , 45 , -1 , -50 , 50 , 0 , 0 } , // BOTH_UNCROUCH3
{ 3384 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CROUCH4
{ 3384 , 4 , -1 , -100 , 100 , 0 , 0 } , // BOTH_UNCROUCH4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_GET_UP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_GET_UP2
{ 1745 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_SIT
{ 7657 , 3 , -1 , 17 , 17 , 0 , 0 } , // BOTH_GUNSIT1
{ 19802 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNT_L
{ 19453 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_DISMOUNT_L
{ 19828 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNT_R
{ 19479 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_DISMOUNT_R
{ 19634 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNTJUMP_L
{ 19658 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNTTHROW
{ 19718 , 42 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNTTHROW_L
{ 19760 , 42 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNTTHROW_R
{ 19689 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_MOUNTTHROWEE
{ 19630 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LOOKLEFT
{ 19632 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LOOKRIGHT
{ 19856 , 23 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_TURBO
{ 19854 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_REV
{ 19382 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_AIR
{ 19384 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_AIR_G
{ 19386 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_AIR_SL
{ 19388 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_AIR_SR
{ 19570 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LAND
{ 19581 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LAND_G
{ 19592 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LAND_SL
{ 19603 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LAND_SR
{ 19505 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_IDLE
{ 19536 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_IDLE_G
{ 19566 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_IDLE_SL
{ 19568 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_IDLE_SR
{ 19614 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANL
{ 19616 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANL_G
{ 19618 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANL_SL
{ 19620 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANL_SR
{ 19622 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANR
{ 19624 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANR_G
{ 19626 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANR_SL
{ 19628 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_LEANR_SR
{ 19404 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATL_S
{ 19432 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATR_S
{ 19446 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATR_TO_L_S
{ 19418 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATL_TO_R_S
{ 19425 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATR_G
{ 19397 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATL_G
{ 19390 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VS_ATF_G
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VS_PAIN1
{ 20099 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_MOUNT_L
{ 20139 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_MOUNT_R
{ 20072 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_MOUNT_B
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_DISMOUNT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_DISMOUNT_L
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_DISMOUNT_R
{ 20312 , 32 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_WALK_FWD
{ 20406 , 32 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_WALK_REV
{ 20344 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_WALK_FWD_L
{ 20375 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_WALK_FWD_R
{ 20179 , 39 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_RUN_FWD
{ 3419 , 32 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_RUN_REV
{ 20218 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_RUN_FWD_L
{ 20240 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_RUN_FWD_R
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_SLIDEF
{ 19879 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_AIR
{ 2623 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATB
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_PAIN1
{ 2262 , 72 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_DEATH1
{ 20262 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_STAND
{ 19962 , 38 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_BUCK
{ 154 , 11 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_LAND
{ 20298 , 14 , 0 , 50 , 50 , 0 , 0 } , // BOTH_VT_TURBO
{ 20038 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE_SL
{ 20068 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE_SR
{ 20262 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE
{ 20000 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_VT_IDLE_S
{ 20036 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE_G
{ 20070 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_IDLE_T
{ 19895 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATL_S
{ 19932 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATR_S
{ 19951 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATR_TO_L_S
{ 19914 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATL_TO_R_S
{ 19925 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATR_G
{ 19888 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATL_G
{ 19881 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VT_ATF_G
{ 3 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GEARS_OPEN
{ 3 , 17 , -1 , -50 , 50 , 0 , 0 } , // BOTH_GEARS_CLOSE
{ 20 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WINGS_OPEN
{ 20 , 21 , -1 , -50 , 50 , 0 , 0 } , // BOTH_WINGS_CLOSE
{ 3986 , 106 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH14_UNGRIP
{ 3946 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEATH14_SITUP
{ 8762 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNEES1
{ 8802 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNEES2
{ 8842 , 39 , -1 , 100 , 100 , 0 , 0 } , // BOTH_KNEES2TO1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STRUGGLE1START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STRUGGLE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STRUGGLE1STOP
{ 10650 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUMMAGE1START
{ 10575 , 75 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUMMAGE1
{ 10680 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUMMAGE1STOP
{ 8066 , 2 , 0 , 34 , 34 , 0 , 0 } , // BOTH_HOLDGLASS1
{ 12156 , 76 , -1 , 34 , 34 , 0 , 0 } , // BOTH_SLIDEGLASS1
{ 11807 , 71 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLAMSABERDOWN
{ 16653 , 26 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK1
{ 16736 , 24 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WALK3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WALK4
{ 20739 , 14 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK_STAFF
{ 20739 , 12 , 0 , -50 , 50 , 0 , 0 } , // BOTH_WALKBACK_STAFF
{ 20725 , 14 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK_DUAL
{ 20696 , 29 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALKBACK_DUAL
{ 16760 , 80 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALK5
{ 16840 , 51 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK6
{ 16891 , 20 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK7
{ 16911 , 28 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK8
{ 16939 , 28 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK9
{ 16679 , 29 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALK10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WALKTORUN1
{ 10699 , 26 , 0 , 25 , 25 , 0 , 0 } , // BOTH_RUN1
{ 4 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUN1START
{ 11 , 23 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUN1STOP
{ 10725 , 26 , 0 , 25 , 25 , 0 , 0 } , // BOTH_RUN2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_RUNINJURED1
{ 533 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUN1TORUN2
{ 558 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RUN2TORUN1
{ 12357 , 10 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUN4
{ 12439 , 14 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUN_STAFF
{ 12377 , 10 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUNBACK_STAFF
{ 12413 , 26 , 0 , 25 , 25 , 0 , 0 } , // BOTH_RUN_DUAL
{ 12413 , 14 , 0 , -50 , 50 , 0 , 0 } , // BOTH_RUNBACK_DUAL
{ 13208 , 13 , 0 , 100 , 100 , 0 , 0 } , // BOTH_STRAFE_LEFT1
{ 13221 , 13 , 0 , 100 , 100 , 0 , 0 } , // BOTH_STRAFE_RIGHT1
{ 10761 , 13 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUNSTRAFE_LEFT1
{ 10774 , 13 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUNSTRAFE_RIGHT1
{ 130 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TURN_LEFT1
{ 94 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TURN_RIGHT1
{ 16593 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNSTAND1
{ 16605 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNSTAND2
{ 16617 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNSTAND3
{ 16629 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNSTAND4
{ 16641 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNSTAND5
{ 16581 , 12 , 0 , 50 , 50 , 0 , 0 } , // BOTH_TURNCROUCH1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_RUNAWAY1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SWIM1
{ 16967 , 29 , 0 , 50 , 50 , 0 , 0 } , // BOTH_WALKBACK1
{ 16967 , 15 , 0 , 100 , 100 , 0 , 0 } , // BOTH_WALKBACK2
{ 10751 , 10 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUNBACK1
{ 10751 , 10 , 0 , 50 , 50 , 0 , 0 } , // BOTH_RUNBACK2
{ 8236 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMP1
{ 8228 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_INAIR1
{ 9011 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LAND1
{ 9016 , 23 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LAND2
{ 8244 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMPBACK1
{ 8230 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_INAIRBACK1
{ 9039 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LANDBACK1
{ 8352 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMPLEFT1
{ 8232 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_INAIRLEFT1
{ 9045 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LANDLEFT1
{ 8361 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_JUMPRIGHT1
{ 8234 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_INAIRRIGHT1
{ 9049 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LANDRIGHT1
{ 6767 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEJUMP1
{ 6759 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEINAIR1
{ 6858 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELAND1
{ 6784 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEJUMPBACK1
{ 6761 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEINAIRBACK1
{ 6872 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELANDBACK1
{ 6802 , 28 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEJUMPLEFT1
{ 6763 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEINAIRLEFT1
{ 6882 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELANDLEFT1
{ 6830 , 28 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEJUMPRIGHT1
{ 6765 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEINAIRRIGHT1
{ 6893 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELANDRIGHT1
{ 6366 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_F
{ 6303 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_B
{ 6377 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_L
{ 6415 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_R
{ 10490 , 30 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ROLL_F
{ 10452 , 38 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ROLL_B
{ 10520 , 27 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ROLL_L
{ 10547 , 28 , -1 , 34 , 34 , 0 , 0 } , // BOTH_ROLL_R
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ROLL_FR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ROLL_FL
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ROLL_BR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ROLL_BL
{ 8081 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOP_F
{ 8068 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOP_B
{ 8095 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOP_L
{ 8111 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOP_R
{ 5362 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_FL
{ 5372 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_FR
{ 5342 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_BL
{ 5352 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_BR
{ 5382 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_L
{ 5392 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_R
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DIVE1
{ 4906 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_FL
{ 4916 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_FR
{ 4886 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_BL
{ 4896 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_BR
{ 4926 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_L
{ 4936 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DODGE_HOLD_R
{ 5402 , 23 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ENGAGETAUNT
{ 1864 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BOW
{ 10310 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MEDITATE
{ 10341 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MEDITATE_END
{ 13433 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SHOWOFF_FAST
{ 13448 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SHOWOFF_MEDIUM
{ 13497 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SHOWOFF_STRONG
{ 13414 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SHOWOFF_DUAL
{ 13468 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SHOWOFF_STAFF
{ 19089 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VICTORY_FAST
{ 19140 , 66 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VICTORY_MEDIUM
{ 19309 , 73 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VICTORY_STRONG
{ 19008 , 81 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VICTORY_DUAL
{ 19206 , 103 , -1 , 50 , 50 , 0 , 0 } , // BOTH_VICTORY_STAFF
{ 460 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ARIAL_LEFT
{ 486 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ARIAL_RIGHT
{ 1406 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CARTWHEEL_LEFT
{ 1425 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CARTWHEEL_RIGHT
{ 6393 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_LEFT
{ 6314 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_BACK1
{ 6329 , 17 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_BACK2
{ 6346 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FLIP_BACK3
{ 1224 , 46 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BUTTERFLY_LEFT
{ 1270 , 56 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BUTTERFLY_RIGHT
{ 17137 , 37 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_RUN_RIGHT
{ 17079 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_WALL_RUN_RIGHT_FLIP
{ 17158 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_RUN_RIGHT_STOP
{ 17100 , 37 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_RUN_LEFT
{ 17054 , 21 , -1 , 34 , 34 , 0 , 0 } , // BOTH_WALL_RUN_LEFT_FLIP
{ 17121 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_RUN_LEFT_STOP
{ 17075 , 25 , -1 , 34 , 34 , 0 , 0 } , // BOTH_WALL_FLIP_RIGHT
{ 17050 , 25 , -1 , 34 , 34 , 0 , 0 } , // BOTH_WALL_FLIP_LEFT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WALL_FLIP_FWD
{ 8881 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNOCKDOWN1
{ 8906 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNOCKDOWN2
{ 8936 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNOCKDOWN3
{ 8966 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNOCKDOWN4
{ 8981 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KNOCKDOWN5
{ 7460 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP1
{ 7491 , 33 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP2
{ 7524 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP3
{ 7550 , 50 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP4
{ 7524 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP5
{ 7631 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_CROUCH_F1
{ 7600 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_CROUCH_B1
{ 7244 , 37 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_F1
{ 7281 , 49 , -1 , 34 , 34 , 0 , 0 } , // BOTH_FORCE_GETUP_F2
{ 6993 , 39 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B1
{ 7032 , 44 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B2
{ 7076 , 35 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B3
{ 7111 , 28 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B4
{ 7139 , 51 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B5
{ 7190 , 54 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FORCE_GETUP_B6
{ 7035 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_BROLL_B
{ 7061 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_BROLL_F
{ 7087 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_BROLL_L
{ 7113 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_BROLL_R
{ 7196 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_FROLL_B
{ 7223 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_FROLL_F
{ 7250 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_FROLL_L
{ 7276 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GETUP_FROLL_R
{ 16996 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_FLIP_BACK1
{ 17023 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_WALL_FLIP_BACK2
{ 12232 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SPIN1
{ 6773 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CEILING_CLING
{ 6774 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CEILING_DROP
{ 6207 , 96 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FJSS_TR_BL
{ 6111 , 96 , -1 , 25 , 25 , 0 , 0 } , // BOTH_FJSS_TL_BR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEATHFROMBACKSLASH
{ 10321 , 131 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RIGHTHANDCHOPPEDOFF
{ 5267 , 63 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DEFLECTSLASH__R__L_FIN
{ 898 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BASHED1
{ 415 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ARIAL_F1
{ 1168 , 56 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BUTTERFLY_FR1
{ 1112 , 56 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BUTTERFLY_FL1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_POSE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_POSE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_POSE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_POSE4
{ 1624 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BACK_FLIP_UP
{ 10258 , 28 , -1 , 50 , 50 , 0 , 0 } , // BOTH_LOSE_SABER
{ 14550 , 101 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAFF_TAUNT
{ 4944 , 101 , -1 , 50 , 50 , 0 , 0 } , // BOTH_DUAL_TAUNT
{ 607 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_FB
{ 628 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A6_LR
{ 780 , 29 , -1 , 50 , 50 , 0 , 0 } , // BOTH_A7_HILT
{ 1241 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_SPIN
{ 1157 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_FLIP_1
{ 1179 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_FLIP_2
{ 1199 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_FLIP_3
{ 1219 , 22 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_FLIP_B
{ 1292 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_SPIN_THROW
{ 1249 , 43 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_SPIN_SLASH
{ 1343 , 101 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ALORA_TAUNT
{ 12274 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ROSH_PAIN
{ 12243 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_ROSH_HEAL
{ 18429 , 55 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TAVION_SCEPTERGROUND
{ 18484 , 81 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TAVION_SWORDPOWER
{ 13384 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SCEPTER_START
{ 13344 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SCEPTER_HOLD
{ 13404 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SCEPTER_STOP
{ 8613 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KYLE_GRAB
{ 8626 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_KYLE_MISS
{ 8640 , 121 , -1 , 35 , 35 , 0 , 0 } , // BOTH_KYLE_PA_1
{ 10561 , 86 , -1 , 35 , 35 , 0 , 0 } , // BOTH_PLAYER_PA_1
{ 8761 , 121 , -1 , 35 , 35 , 0 , 0 } , // BOTH_KYLE_PA_2
{ 10647 , 197 , -1 , 35 , 35 , 0 , 0 } , // BOTH_PLAYER_PA_2
{ 11010 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PLAYER_PA_FLY
{ 8882 , 166 , -1 , 35 , 35 , 0 , 0 } , // BOTH_KYLE_PA_3
{ 10844 , 145 , -1 , 35 , 35 , 0 , 0 } , // BOTH_PLAYER_PA_3
{ 10989 , 21 , -1 , 35 , 35 , 0 , 0 } , // BOTH_PLAYER_PA_3_FLY
{ 761 , 41 , -1 , 34 , 34 , 0 , 0 } , // BOTH_BUCK_RIDER
{ 423 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_START
{ 367 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_MISS
{ 603 , 40 , 0 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_IDLE
{ 349 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_END
{ 71 , 46 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_ATTACK
{ 2542 , 81 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_SNIFF
{ 2516 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HOLD_DROP
{ 7302 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_GRABBED
{ 11944 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RELEASED
{ 7617 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANG_IDLE
{ 7589 , 28 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANG_ATTACK
{ 7619 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HANG_PAIN
{ 8000 , 66 , -1 , 50 , 50 , 0 , 0 } , // BOTH_HIT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LADDER_UP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LADDER_DWN1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LADDER_IDLE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ONLADDER_BOT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_OFFLADDER_BOT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_ONLADDER_TOP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_OFFLADDER_TOP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LIFT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_STEP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_HITWALL1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_AMBUSHLAND1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_BIRTH1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLY_IDLE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLY_IDLE2
{ 4 , 6 , -1 , -100 , 100 , 0 , 0 } , // BOTH_FLY_SHIELDED
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLY_START1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLY_STOP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLY_LOOP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLOAT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLOAT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FLOATCONSOLE1
{ 13264 , 30 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SWIM_IDLE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SWIMFORWARDSTART
{ 13234 , 30 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SWIMFORWARD
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SWIMFORWARDSTOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SWIMBACKWARDSTART
{ 691 , 71 , 0 , 34 , 34 , 0 , 0 } , // BOTH_SWIMBACKWARD
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SWIMBACKWARDSTOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LIE_DOWN1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LIE_DOWN2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LIE_DOWN3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PAIN2WRITHE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PRONE2RLEG
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_PRONE2LLEG
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WRITHING1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WRITHING1RLEG
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WRITHING1LLEG
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_WRITHING2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED6ATTACKSTART
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED6ATTACKSTOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED6COMBADGE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJURED6POINT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_INJUREDTOSTAND1
{ 9499 , 27 , -1 , 50 , 50 , 0 , 0 } , // BOTH_PROPUP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CRAWLBACK1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SITWALL1
{ 11878 , 1 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLEEP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP5
{ 11967 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLEEP6START
{ 12007 , 149 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLEEP6STOP
{ 11878 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLEEP1GETUP
{ 11918 , 49 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SLEEP1GETUP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP2GETUP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP3GETUP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP3DEATH
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP3DEAD
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP_IDLE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP_IDLE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP_IDLE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP_IDLE4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP1_NOSE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SLEEP2_SHIFT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_RESTRAINED1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_RESTRAINED1POINT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_LIFTED1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CARRIED1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CARRIED2
{ 1595 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CHOKE1START
{ 1614 , 1 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CHOKE1STARTHOLD
{ 1526 , 69 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CHOKE1
{ 1615 , 60 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CHOKE2
{ 1675 , 70 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CHOKE3
{ 4 , 6 , -1 , 100 , 100 , 0 , 0 } , // BOTH_POWERUP1
{ 561 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TURNON
{ 549 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TURNOFF
{ 1326 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_BUTTON1
{ 1377 , 29 , -1 , 25 , 25 , 0 , 0 } , // BOTH_BUTTON2
{ 1377 , 14 , -1 , 25 , 25 , 0 , 0 } , // BOTH_BUTTON_HOLD
{ 1391 , 15 , -1 , 25 , 25 , 0 , 0 } , // BOTH_BUTTON_RELEASE
{ 10300 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_RESISTPUSH
{ 6980 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEPUSH
{ 6968 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEPULL
{ 9306 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MINDTRICK1
{ 9320 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_MINDTRICK2
{ 6949 , 19 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELIGHTNING
{ 6949 , 3 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELIGHTNING_START
{ 6952 , 7 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FORCELIGHTNING_HOLD
{ 6960 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCELIGHTNING_RELEASE
{ 6726 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEHEAL_START
{ 6746 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEHEAL_STOP
{ 6710 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEHEAL_QUICK
{ 11318 , 7 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SABERPULL
{ 6635 , 13 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEGRIP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_FORCEGRIP2
{ 6648 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEGRIP3
{ 6679 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEGRIP3THROW
{ 6635 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEGRIP_HOLD
{ 6640 , 8 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCEGRIP_RELEASE
{ 16492 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TOSS1
{ 16522 , 59 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TOSS2
{ 6708 , 57 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_RAGE
{ 6123 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_2HANDEDLIGHTNING
{ 6123 , 5 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_2HANDEDLIGHTNING_START
{ 6128 , 15 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_2HANDEDLIGHTNING_HOLD
{ 6143 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_2HANDEDLIGHTNING_RELEASE
{ 6178 , 52 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN
{ 6178 , 6 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_START
{ 6184 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_HOLD
{ 6204 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_RELEASE
{ 6278 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_GRAB_START
{ 6263 , 15 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_GRAB_HOLD
{ 6245 , 18 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_GRAB_END
{ 6230 , 15 , 0 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_DRAIN_GRABBED
{ 6154 , 24 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_ABSORB
{ 6154 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_ABSORB_START
{ 6166 , 12 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_ABSORB_END
{ 6630 , 47 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_PROTECT
{ 6677 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_FORCE_PROTECT_FAST
{ 2933 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKR1START
{ 2943 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKR1STARTTOMID
{ 2938 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKR1MIDTOSTART
{ 2943 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKR1STOP
{ 2933 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKR1STOPTOMID
{ 2948 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKR1MIDTOSTOP
{ 2833 , 100 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKR1
{ 2814 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKL1START
{ 2824 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKL1STARTTOMID
{ 2819 , 5 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKL1MIDTOSTART
{ 2824 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKL1STOP
{ 2814 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKL1STOPTOMID
{ 2829 , 4 , -1 , 100 , 100 , 0 , 0 } , // BOTH_COCKPIT_TALKL1MIDTOSTOP
{ 2714 , 100 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKL1
{ 1745 , 259 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_CONSOLE1
{ 2004 , 259 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_CONSOLE2
{ 2204 , 59 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_CONSOLE2_PARTIAL
{ 2263 , 31 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADNOD
{ 2294 , 39 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADSHAKE
{ 2333 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADTILTLSTART
{ 2343 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADTILTLSTOP
{ 2352 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADTILTRSTART
{ 2362 , 9 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_HEADTILTRSTOP
{ 2629 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE7START
{ 2629 , 40 , -1 , -50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE7STOP
{ 2669 , 45 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE8START
{ 2669 , 45 , -1 , -50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE8STOP
{ 2580 , 33 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE11START
{ 2613 , 16 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COCKPIT_TALKGESTURE11STOP
{ 2371 , 35 , -1 , 17 , 17 , 0 , 0 } , // BOTH_COCKPIT_SLEEP6START
{ 2406 , 174 , -1 , 17 , 17 , 0 , 0 } , // BOTH_COCKPIT_SLEEP6STOP
{ 20973 , 10 , -1 , 200 , 200 , 0 , 0 } , // BOTH_WIND
{ 2944 , 4 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND_TO_KNEEL
{ 2944 , 4 , -1 , -50 , 50 , 0 , 0 } , // BOTH_KNEEL_TO_STAND
{ 18726 , 15 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TUSKENATTACK1
{ 18741 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TUSKENATTACK2
{ 18755 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TUSKENATTACK3
{ 18769 , 14 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TUSKENLUNGE1
{ 18783 , 47 , -1 , 50 , 50 , 0 , 0 } , // BOTH_TUSKENTAUNT1
{ 2884 , 11 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COWER1_START
{ 2792 , 92 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COWER1
{ 2895 , 49 , -1 , 50 , 50 , 0 , 0 } , // BOTH_COWER1_STOP
{ 14323 , 10 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SONICPAIN_START
{ 14258 , 65 , 0 , 50 , 50 , 0 , 0 } , // BOTH_SONICPAIN_HOLD
{ 14193 , 65 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SONICPAIN_END
{ 14653 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND10
{ 14718 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND10_TALK1
{ 14758 , 35 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND10_TALK2
{ 14693 , 25 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND10TOSTAND1
{ 14978 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1_TALK1
{ 15008 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1_TALK2
{ 15038 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_STAND1_TALK3
{ 13672 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT4
{ 13712 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT5
{ 13752 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT5_TALK1
{ 13772 , 33 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT5_TALK2
{ 13805 , 30 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT5_TALK3
{ 13835 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT6
{ 13875 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_SIT7
{ 17174 , 4 , -1 , 50 , 50 , 0 , 0 } , // TORSO_DROPWEAP1
{ 694 , 15 , -1 , 34 , 34 , 0 , 0 } , // TORSO_DROPWEAP2
{ 709 , 16 , -1 , 34 , 34 , 0 , 0 } , // TORSO_DROPWEAP3
{ 17178 , 4 , -1 , 50 , 50 , 0 , 0 } , // TORSO_DROPWEAP4
{ 17182 , 5 , -1 , 50 , 50 , 0 , 0 } , // TORSO_RAISEWEAP1
{ 736 , 61 , -1 , 34 , 34 , 0 , 0 } , // TORSO_RAISEWEAP2
{ 797 , 10 , -1 , 34 , 34 , 0 , 0 } , // TORSO_RAISEWEAP3
{ 17187 , 5 , -1 , 50 , 50 , 0 , 0 } , // TORSO_RAISEWEAP4
{ 86 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONREADY1
{ 12683 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONREADY2
{ 539 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONREADY3
{ 549 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONREADY4
{ 305 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONREADY5
{ 327 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONREADY6
{ 417 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONREADY7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONREADY8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONREADY9
{ 527 , 1 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONREADY10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONREADY11
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONREADY12
{ 113 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONIDLE1
{ 12683 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONIDLE2
{ 12674 , 2 , 0 , 17 , 17 , 0 , 0 } , // TORSO_WEAPONIDLE3
{ 544 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONIDLE4
{ 270 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONIDLE5
{ 306 , 1 , -1 , 50 , 50 , 0 , 0 } , // TORSO_WEAPONIDLE6
{ 328 , 1 , -1 , 34 , 34 , 0 , 0 } , // TORSO_WEAPONIDLE7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONIDLE8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONIDLE9
{ 512 , 1 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONIDLE10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONIDLE11
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_WEAPONIDLE12
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKR1START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKR1HOLD
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKR1STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKR1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKL1START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKL1HOLD
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKL1STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_TALKL1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKL1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKR1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKR2START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKR2STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKR2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKL2START
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKL2STOP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_LOOKL2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDGESTURE1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDGESTURE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDGESTURE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDGESTURE4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDEXTEND1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDRETRACT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_DROPHELMET1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISEHELMET1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_REACHHELMET1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GRABLBACKL
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GRABUBACKL
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GRABLBACKR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GRABUBACKR
{ 17192 , 8 , -1 , 50 , 50 , 0 , 0 } , // TORSO_SURRENDER_START
{ 17192 , 8 , -1 , -50 , 50 , 0 , 0 } , // TORSO_SURRENDER_STOP
{ 4260 , 57 , 0 , 50 , 50 , 0 , 0 } , // TORSO_CHOKING1
{ 20991 , 56 , -1 , 50 , 50 , 0 , 0 } , // TORSO_HANDSIGNAL1
{ 21047 , 73 , -1 , 50 , 50 , 0 , 0 } , // TORSO_HANDSIGNAL2
{ 21120 , 61 , -1 , 50 , 50 , 0 , 0 } , // TORSO_HANDSIGNAL3
{ 21181 , 36 , -1 , 50 , 50 , 0 , 0 } , // TORSO_HANDSIGNAL4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_HANDSIGNAL5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GESTURE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_DROP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GETFLAG
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GUARDBASE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_PATROL
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_FOLLOWME
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_AFFIRMATIVE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_NEGATIVE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEAD1_WATER
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEAD2_WATER
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_DEAD3_WATER
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CLIMB
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CLIMB_DOWN
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_CLIMB_DISMOUNT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_SALUTE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_GRAB_GRENADE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA9
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA11
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA12
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA13
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA14
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA15
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA16
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA17
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA18
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA19
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOTH_EXTRA20
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GESTURE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GESTURE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_GESTURE4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND_ALT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND_ALT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_READY
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELAX
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISE2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND2_ALT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND2_ALT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_READY2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELAX2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISE3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND3_ALT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND3_ALT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_READY3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELAX3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISE4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND4_ALT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND4_ALT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_READY4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELAX4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RAISE5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_ATTACK5B
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND5_ALT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_STAND5_ALT2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_READY5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELAX5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELOAD1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELOAD2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELOAD3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_RELOAD4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_MG42
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_MOVE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_MOVE_ALT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA9
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // TORSO_EXTRA10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_WALKCR_BACK
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_SWIM_IDLE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_IDLE_ALT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_BOOT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA9
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_EXTRA10
{ 4 , 39 , 0 , -50 , 50 , 0 , 0 } , // LEGS_WALKBACK1
{ 4 , 39 , 0 , -50 , 50 , 0 , 0 } , // LEGS_WALKBACK2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_RUNBACK1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_RUNBACK2
{ 524 , 19 , -1 , 50 , 50 , 0 , 0 } , // LEGS_TURN1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_TURN2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_LEAN_LEFT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_LEAN_RIGHT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_KNEELDOWN1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_KNEELUP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_CRLEAN_LEFT1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_CRLEAN_RIGHT1
{ 8236 , 1 , 0 , 50 , 50 , 0 , 0 } , // LEGS_CHOKING1
{ 17200 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_LEFTUP1
{ 17202 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_LEFTUP2
{ 17204 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_LEFTUP3
{ 17206 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_LEFTUP4
{ 17208 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_LEFTUP5
{ 17210 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_RIGHTUP1
{ 17212 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_RIGHTUP2
{ 17214 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_RIGHTUP3
{ 17216 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_RIGHTUP4
{ 17218 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_RIGHTUP5
{ 17220 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_LUP1
{ 17222 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_LUP2
{ 17224 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_LUP3
{ 17226 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_LUP4
{ 17228 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_LUP5
{ 17230 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_RUP1
{ 17232 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_RUP2
{ 17234 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_RUP3
{ 17236 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_RUP4
{ 17238 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S1_RUP5
{ 17240 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_LUP1
{ 17242 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_LUP2
{ 17244 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_LUP3
{ 17246 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_LUP4
{ 17248 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_LUP5
{ 17250 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_RUP1
{ 17252 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_RUP2
{ 17254 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_RUP3
{ 17256 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_RUP4
{ 17258 , 2 , -1 , 17 , 17 , 0 , 0 } , // LEGS_S3_RUP5
{ 21295 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_LUP1
{ 21297 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_LUP2
{ 21299 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_LUP3
{ 21301 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_LUP4
{ 21303 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_LUP5
{ 21305 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_RUP1
{ 21307 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_RUP2
{ 21309 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_RUP3
{ 21311 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_RUP4
{ 21313 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S4_RUP5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_LUP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_LUP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_LUP3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_LUP4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_LUP5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_RUP1
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_RUP2
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_RUP3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_RUP4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_S5_RUP5
{ 21315 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_LUP1
{ 21317 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_LUP2
{ 21319 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_LUP3
{ 21321 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_LUP4
{ 21323 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_LUP5
{ 21325 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_RUP1
{ 21327 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_RUP2
{ 21329 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_RUP3
{ 21331 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_RUP4
{ 21333 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S6_RUP5
{ 21335 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_LUP1
{ 21337 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_LUP2
{ 21339 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_LUP3
{ 21341 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_LUP4
{ 21343 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_LUP5
{ 21345 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_RUP1
{ 21347 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_RUP2
{ 21349 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_RUP3
{ 21351 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_RUP4
{ 21353 , 2 , -1 , 50 , 50 , 0 , 0 } , // LEGS_S7_RUP5
{ 21355 , 19 , -1 , 50 , 50 , 0 , 0 } , // LEGS_TURN180
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_WALKCR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_WALK
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_RUN
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_BACK
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_SWIM
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_JUMP
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_LAND
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_JUMPB
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_LANDB
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_IDLE
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_IDLECR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_TURN
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_BACKCR
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // LEGS_BACKWALK
{ 18 , 24 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CIN_1
{ 262 , 40 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CIN_2
{ 302 , 81 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_3
{ 383 , 40 , 0 , 50 , 50 , 0 , 0 } , // BOTH_CIN_4
{ 423 , 91 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_5
{ 514 , 86 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_6
{ 600 , 116 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_7
{ 716 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_8
{ 752 , 20 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_9
{ 42 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_10
{ 82 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_11
{ 103 , 40 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_12
{ 143 , 21 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_13
{ 164 , 26 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_14
{ 190 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_15
{ 226 , 36 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_16
{ 262 , 28 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_17
{ 290 , 51 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_18
{ 341 , 71 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_19
{ 421 , 46 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_20
{ 467 , 46 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_21
{ 513 , 46 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_22
{ 559 , 130 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_23
{ 689 , 56 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_24
{ 745 , 71 , -1 , 35 , 35 , 0 , 0 } , // BOTH_CIN_25
{ 8722 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_26
{ 8724 , 797 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_27
{ 9521 , 502 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_28
{ 10023 , 594 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_29
{ 11264 , 671 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_30
{ 11935 , 671 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_31
{ 12606 , 321 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_32
{ 12927 , 380 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_33
{ 13307 , 380 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_34
{ 13687 , 380 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_35
{ 14067 , 320 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_36
{ 14387 , 320 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_37
{ 14707 , 320 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_38
{ 15027 , 691 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_39
{ 16163 , 691 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_40
{ 16854 , 691 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_41
{ 17545 , 691 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_42
{ 18236 , 610 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_43
{ 18846 , 610 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_44
{ 19456 , 610 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_45
{ 20066 , 610 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_46
{ 20676 , 701 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_47
{ 21377 , 80 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_48
{ 21457 , 80 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_49
{ 22542 , 2 , -1 , 50 , 50 , 0 , 0 } , // BOTH_CIN_50
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_BLOCK_DIAG_LEFT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_BLOCK_DIAG_RIGHT
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED3
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED4
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED5
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED6
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED7
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED8
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED9
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED10
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED11
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED12
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED13
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED14
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED15
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // BOOT_UNUSED16
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // FLAG_RUN
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // FLAG_STAND
{ 0 , 0 , -1 , 100 , 100 , 0 , 0 } , // FLAG_STAND2RUN
};

//#define CONVENIENT_ANIMATION_FILE_DEBUG_THING

#ifdef CONVENIENT_ANIMATION_FILE_DEBUG_THING
void SpewDebugStuffToFile()
{
	fileHandle_t f;
	int i = 0;

	FS_FOpenFile("file_of_debug_stuff_MP.txt", &f, FS_WRITE);

	if (!f)
	{
		return;
	}

	BGPAFtext[0] = 0;

	while (i < MAX_ANIMATIONS)
	{
		strcat(BGPAFtext, va("%i %i\n", i, bgGlobalAnimations[i].frameLerp));
		i++;
	}

	FS_Write(BGPAFtext, strlen(BGPAFtext), f);
	FS_FCloseFile(f);
}
#endif

/*
void BG_AnimationOverrides() {
	int i, animNum;
	float fps;

	for(i=0;i< animOverrideCount;i++)
	{
		animNum = animOverrideNums[i];
		// Dirty JKA wallgrab hack
		// TODO Only do this if:
		// cgame: tommyternal server && g_defrag
		// game: g_defrag
		// TODO Wallrun too somehow
		switch (animNum) {
			case BOTH_FORCEWALLREBOUND_BACK_GENERAL:
			case BOTH_FORCEWALLREBOUND_FORWARD_GENERAL:
			case BOTH_FORCEWALLREBOUND_LEFT_GENERAL:
			case BOTH_FORCEWALLREBOUND_RIGHT_GENERAL: // use slowed down BOTH_UNCROUCH1_GENERAL anim instead (ditch last frame to make the total length correct with the 1/4 fps)
				bgGlobalAnimations[animNum].firstFrame = 3384;
				bgGlobalAnimations[animNum].numFrames = 3;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = -5;
				break;
			case BOTH_FORCEWALLHOLD_BACK_GENERAL:
			case BOTH_FORCEWALLHOLD_FORWARD_GENERAL:
			case BOTH_FORCEWALLHOLD_LEFT_GENERAL:
			case BOTH_FORCEWALLHOLD_RIGHT_GENERAL: // use first frame of BOTH_CROUCH1IDLE_GENERAL
				bgGlobalAnimations[animNum].firstFrame = 3388;
				bgGlobalAnimations[animNum].numFrames = 1;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = 20;
				break;
			case BOTH_FORCEWALLRELEASE_BACK_GENERAL:
			case BOTH_FORCEWALLRELEASE_FORWARD_GENERAL:
			case BOTH_FORCEWALLRELEASE_LEFT_GENERAL:
			case BOTH_FORCEWALLRELEASE_RIGHT_GENERAL: // use slowed down BOTH_UNCROUCH1_GENERAL anim instead (ditch last frame to make the total length correct with the 1/2 fps)
				bgGlobalAnimations[animNum].firstFrame = 3384;
				bgGlobalAnimations[animNum].numFrames = 3;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = -10;
				break;
		}

		if (fps < 0)
		{//backwards
			bgGlobalAnimations[animNum].frameLerp = floor(1000.0f / fps);
		}
		else
		{
			bgGlobalAnimations[animNum].frameLerp = ceil(1000.0f / fps);
		}

		bgGlobalAnimations[animNum].initialLerp = ceil(1000.0f / fabs(fps));
	}
}*/
/* TODO bring it back later without needing file parsing
qboolean  PMove::BG_ParseAnimationFile(const char *filename) 
{
	char		*text_p;
	int			len;
	int			i;
	char		*token;
	float		fps;

	fileHandle_t	f;
	int				animNum;


	// load the file
	if (!BGPAFtextLoaded)
	{ //rww - We are always using the same animation config now. So only load it once.
		len = FS_FOpenFile( filename, &f, FS_READ );
		if ( len <= 0 ) 
		{
			return qfalse;
		}
		if ( len >= (int)sizeof( BGPAFtext ) - 1 ) 
		{
			//Com_Printf( "File %s too long\n", filename );
			return qfalse;
		}

		FS_Read( BGPAFtext, len, f );
		BGPAFtext[len] = 0;
		FS_FCloseFile( f );
	}
	else
	{
		return qtrue;
	}

	// parse the text
	text_p = BGPAFtext;

	//FIXME: have some way of playing anims backwards... negative numFrames?

	//initialize anim array so that from 0 to MAX_ANIMATIONS, set default values of 0 1 0 100
	for(i = 0; i < MAX_ANIMATIONS_GENERAL; i++)
	{
		bgGlobalAnimations[i].firstFrame = 0;
		bgGlobalAnimations[i].numFrames = 0;
		bgGlobalAnimations[i].loopFrames = -1;
		bgGlobalAnimations[i].frameLerp = 100;
		bgGlobalAnimations[i].initialLerp = 100;
	}

	// read information for each frame
	while(1) 
	{
		token = COM_Parse( (const char **)(&text_p) );

		if ( !token || !token[0]) 
		{
			break;
		}

		animNum = GetIDForString(animTable, token);
		if(animNum == -1)
		{
//#ifndef FINAL_BUILD
#ifdef _DEBUG
		if(developer){
			Com_Printf("WARNING: Unknown token %s in %s\n", token, filename);
		}
#endif
			continue;
		}

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		bgGlobalAnimations[animNum].firstFrame = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		bgGlobalAnimations[animNum].numFrames = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		bgGlobalAnimations[animNum].loopFrames = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		fps = atof( token );
		if ( fps == 0 ) 
		{
			fps = 1;//Don't allow divide by zero error
		}

// TODO... cant do it as simply as this because it only gets loaded once?
//		if (
//#ifdef JK2_GAME
//			g_defrag.integer
//#elif JK2_CGAME
//			
//#endif
//			) 
		{
			switch (animNum) {
			case BOTH_FORCEWALLREBOUND_BACK_GENERAL:
			case BOTH_FORCEWALLREBOUND_FORWARD_GENERAL:
			case BOTH_FORCEWALLREBOUND_LEFT_GENERAL:
			case BOTH_FORCEWALLREBOUND_RIGHT_GENERAL: // use slowed down BOTH_UNCROUCH1_GENERAL anim instead (ditch last frame to make the total length correct with the 1/4 fps)
				bgGlobalAnimations[animNum].firstFrame = 3384;
				bgGlobalAnimations[animNum].numFrames = 3;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = -5;
				break;
			case BOTH_FORCEWALLHOLD_BACK_GENERAL:
			case BOTH_FORCEWALLHOLD_FORWARD_GENERAL:
			case BOTH_FORCEWALLHOLD_LEFT_GENERAL:
			case BOTH_FORCEWALLHOLD_RIGHT_GENERAL: // use first frame of BOTH_CROUCH1IDLE_GENERAL
				bgGlobalAnimations[animNum].firstFrame = 3388;
				bgGlobalAnimations[animNum].numFrames = 1;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = 20;
				break;
			case BOTH_FORCEWALLRELEASE_BACK_GENERAL:
			case BOTH_FORCEWALLRELEASE_FORWARD_GENERAL:
			case BOTH_FORCEWALLRELEASE_LEFT_GENERAL:
			case BOTH_FORCEWALLRELEASE_RIGHT_GENERAL: // use slowed down BOTH_UNCROUCH1_GENERAL anim instead (ditch last frame to make the total length correct with the 1/2 fps)
				bgGlobalAnimations[animNum].firstFrame = 3384;
				bgGlobalAnimations[animNum].numFrames = 3;
				bgGlobalAnimations[animNum].loopFrames = -1;
				fps = -10;
				break;
			}
		}
		

		if ( fps < 0 )
		{//backwards
			bgGlobalAnimations[animNum].frameLerp = floor(1000.0f / fps);
		}
		else
		{
			bgGlobalAnimations[animNum].frameLerp = ceil(1000.0f / fps);
		}

		bgGlobalAnimations[animNum].initialLerp = ceil(1000.0f / fabs(fps));
	}

	//BG_AnimationOverrides(); // just to be safe but we should already be using only available anims (ones in animation.cfg). others cant be used because the array will never loop over them and thus we get assertion error on debug build

#ifdef _DEBUG
	if(developer){
	//Check the array, and print the ones that have nothing in them.
	for (i = 0; i < MAX_ANIMATIONS_GENERAL; i++)
	{
		if (animTable[i].name != NULL)		// This animation reference exists.
		{
			if (bgGlobalAnimations[i].firstFrame <= 0 && bgGlobalAnimations[i].numFrames <= 0)
			{	// This is an empty animation reference.
				Com_Printf("***ANIMTABLE reference #%d (%s) is empty!\n", i, animTable[i].name);
			}
		}
	}
	}
#endif // _DEBUG

#ifdef CONVENIENT_ANIMATION_FILE_DEBUG_THING
	SpewDebugStuffToFile();
#endif
	BGPAFtextLoaded = qtrue;
	return qtrue;
}

*/

/*
===================
LEGS Animations
Base animation for overall body
===================
*/
 void  PMove::PM_StartLegsAnim( int anim ) {
	if ( pm->ps->pm_type >= PM_DEAD ) {
		return;
	}
	if ( pm->ps->legsTimer > 0 ) {
		return;		// a high priority animation is running
	}

	if (pm->ps->usingATST)
	{ //animation is handled mostly client-side with only a few exceptions
		return;
	}

	/*
	if (BG_InSaberStandAnim(anim) && pm->ps->weapon == WP_SABER_GENERAL && pm->ps->dualBlade)
	{ //a bit of a hack, but dualblade is cheat-only anyway
		anim = BOTH_STAND1_GENERAL;
	}
	*/

	pm->ps->legsAnim = ( ( pm->ps->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
		| anim;

	if ( pm->debugLevel ) {
		Com_Printf("%d:  StartLegsAnim %d, on client#%d\n", pm->cmd.serverTime, anim, pm->ps->clientNum);
	}
}

void  PMove::PM_ContinueLegsAnim( int anim ) {
	if ( ( pm->ps->legsAnim & ~ANIM_TOGGLEBIT ) == anim ) {
		return;
	}
	if ( pm->ps->legsTimer > 0 ) {
		return;		// a high priority animation is running
	}

	PM_StartLegsAnim( anim );
}

void  PMove::PM_ForceLegsAnim( int anim) {
	if (BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) &&
		pm->ps->legsTimer > 0 &&
		!BG_InSpecialJump(anim, pm->modParms.runFlags))
	{
		return;
	}

	if (BG_InRoll(pm->ps, pm->ps->legsAnim) &&
		pm->ps->legsTimer > 0 &&
		!BG_InRoll(pm->ps, anim))
	{
		return;
	}

	pm->ps->legsTimer = 0;
	PM_StartLegsAnim( anim );
}



/*
===================
TORSO Animations
Override animations for upper body
===================
*/
void  PMove::PM_StartTorsoAnim( int anim ) {
	if ( pm->ps->pm_type >= PM_DEAD ) {
		return;
	}

	if (pm->ps->usingATST)
	{ //animation is handled mostly client-side with only a few exceptions
		return;
	}

	/*
	if (BG_InSaberStandAnim(anim) && pm->ps->weapon == WP_SABER_GENERAL && pm->ps->dualBlade)
	{ //a bit of a hack, but dualblade is cheat-only anyway
		anim = BOTH_STAND1_GENERAL;
	}
	*/

	pm->ps->torsoAnim = ( ( pm->ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
		| anim;
}


/*
-------------------------
PM_SetLegsAnimTimer
-------------------------
*/

void  PMove::PM_SetLegsAnimTimer(int time )
{
	pm->ps->legsTimer = time;

	if (pm->ps->legsTimer < 0 && time != -1 )
	{//Cap timer to 0 if was counting down, but let it be -1 if that was intentional.  NOTENOTE Yeah this seems dumb, but it mirrors SP.
		pm->ps->legsTimer = 0;
	}
}

/*
-------------------------
PM_SetTorsoAnimTimer
-------------------------
*/

void  PMove::PM_SetTorsoAnimTimer(int time )
{
	pm->ps->torsoTimer = time;

	if (pm->ps->torsoTimer < 0 && time != -1 )
	{//Cap timer to 0 if was counting down, but let it be -1 if that was intentional.  NOTENOTE Yeah this seems dumb, but it mirrors SP.
		pm->ps->torsoTimer = 0;
	}
}

void  PMove::BG_SaberStartTransAnim( int saberAnimLevel, int anim, float *animSpeed )
{
	if ( ( (anim&~ANIM_TOGGLEBIT) >= BOTH_T1_BR__R_GENERAL && 
		(anim&~ANIM_TOGGLEBIT) <= BOTH_T1_BL_TL_GENERAL ) ||
		( (anim&~ANIM_TOGGLEBIT) >= BOTH_T2_BR__R_GENERAL && 
		(anim&~ANIM_TOGGLEBIT) <= BOTH_T2_BL_TL_GENERAL ) ||
		( (anim&~ANIM_TOGGLEBIT) >= BOTH_T3_BR__R_GENERAL && 
		(anim&~ANIM_TOGGLEBIT) <= BOTH_T3_BL_TL_GENERAL ) )
	{
		if ( saberAnimLevel == FORCE_LEVEL_1 )
		{
			*animSpeed *= 1.5;
		}
		else if ( saberAnimLevel == FORCE_LEVEL_3 )
		{
			*animSpeed *= 0.75;
		}
	}
}

/*
-------------------------
PM_SetAnimFinal
-------------------------
*/
void  PMove::PM_SetAnimFinal(int setAnimParts,int anim,int setAnimFlags,
					 int blendTime)		// default blendTime=350
{
	animation_t *animations = pm->animations;

	float editAnimSpeed = (jk2gameplay == VERSION_1_02 ? 0 : 1);

	if (!animations)
	{
		return;
	}

	//NOTE: Setting blendTime here breaks actual blending..
	blendTime = 0;

	BG_SaberStartTransAnim(pm->ps->fd.saberAnimLevel, anim, &editAnimSpeed);

	// Set torso anim
	if (setAnimParts & SETANIM_TORSO)
	{
		// Don't reset if it's already running the anim
		if( !(setAnimFlags & SETANIM_FLAG_RESTART) && (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT ) == anim )
		{
			goto setAnimLegs;
		}
		// or if a more important anim is running
		if( !(setAnimFlags & SETANIM_FLAG_OVERRIDE) && ((pm->ps->torsoTimer > 0)||(pm->ps->torsoTimer == -1)) )
		{	
			goto setAnimLegs;
		}

		PM_StartTorsoAnim( anim );

		if (setAnimFlags & SETANIM_FLAG_HOLD)
		{
			if (setAnimFlags & SETANIM_FLAG_HOLDLESS)
			{	// Make sure to only wait in full 1/20 sec server frame intervals.
				int dur;
				int speedDif;
				
				if ( jk2gameplay == VERSION_1_02 )
				{
					dur = (animations[anim].numFrames ) * abs(animations[anim].frameLerp);
					//dur = ((int)(dur/50.0)) * 50 / timeScaleMod;
					dur -= blendTime+abs(animations[anim].frameLerp)*2;
				}
				else
				{
					dur = (animations[anim].numFrames-1) * abs(animations[anim].frameLerp);
					speedDif = dur - (dur * editAnimSpeed);
					dur += speedDif;
				}
				if (dur > 1)
				{
					pm->ps->torsoTimer = dur-1;
				}
				else
				{
					pm->ps->torsoTimer = abs(animations[anim].frameLerp);
				}
			}
			else
			{
				pm->ps->torsoTimer = ((animations[anim].numFrames ) * abs(animations[anim].frameLerp));
			}

			if (pm->ps->fd.forcePowersActive & (1 << FP_RAGE))
			{
				pm->ps->torsoTimer /= 1.7;
			}

			if (editAnimSpeed && jk2gameplay == VERSION_1_02)
			{
				pm->ps->torsoTimer /= editAnimSpeed;
			}
		}
	}

setAnimLegs:
	// Set legs anim
	if (setAnimParts & SETANIM_LEGS)
	{
		// Don't reset if it's already running the anim
		if( !(setAnimFlags & SETANIM_FLAG_RESTART) && (pm->ps->legsAnim & ~ANIM_TOGGLEBIT ) == anim )
		{
			goto setAnimDone;
		}
		// or if a more important anim is running
		if( !(setAnimFlags & SETANIM_FLAG_OVERRIDE) && ((pm->ps->legsTimer > 0)||(pm->ps->legsTimer == -1)) )
		{	
			goto setAnimDone;
		}

		PM_StartLegsAnim(anim);

		if (setAnimFlags & SETANIM_FLAG_HOLD)
		{
			if (setAnimFlags & SETANIM_FLAG_HOLDLESS)
			{	// Make sure to only wait in full 1/20 sec server frame intervals.
				int dur;
				int speedDif;
				
				if ( jk2gameplay == VERSION_1_02 )
				{
					dur = (animations[anim].numFrames -1) * abs(animations[anim].frameLerp);
					//dur = ((int)(dur/50.0)) * 50 / timeScaleMod;
					dur -= blendTime+abs(animations[anim].frameLerp)*2;
				}
				else
				{
					dur = (animations[anim].numFrames-1) * abs(animations[anim].frameLerp);
					speedDif = dur - (dur * editAnimSpeed);
					dur += speedDif;
				}
				if (dur > 1)
				{
					pm->ps->legsTimer = dur-1;
				}
				else
				{
					pm->ps->legsTimer = abs(animations[anim].frameLerp);
				}
			}
			else
			{
				pm->ps->legsTimer = ((animations[anim].numFrames ) * abs(animations[anim].frameLerp));
			}

			if (pm->ps->fd.forcePowersActive & (1 << FP_RAGE))
			{
				pm->ps->legsTimer /= 1.3;
			}
			else if (pm->ps->fd.forcePowersActive & (1 << FP_SPEED))
			{
				pm->ps->legsTimer /= 1.7;
			}
		}
	}

setAnimDone:
	return;
}



// Imported from single-player, this function is mainly intended to make porting from SP easier.
void  PMove::PM_SetAnim(int setAnimParts,int anim,int setAnimFlags, int blendTime)
{	
#if JK2_GAME // server might use stuff we don't have in cgame, that's ok.
	assert(	bgGlobalAnimations[anim].firstFrame != 0 || 
			bgGlobalAnimations[anim].numFrames != 0);
#endif

	if (BG_InSpecialJump(anim, pm->modParms.runFlags))
	{
		setAnimFlags |= SETANIM_FLAG_RESTART;
	}

	if (BG_InRoll(pm->ps, pm->ps->legsAnim))
	{ //never interrupt a roll
		return;
	}

	if (setAnimFlags&SETANIM_FLAG_OVERRIDE)
	{
		if (setAnimParts & SETANIM_TORSO)
		{
			if( (setAnimFlags & SETANIM_FLAG_RESTART) || (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT ) != anim )
			{
				PM_SetTorsoAnimTimer(0);
			}
		}
		if (setAnimParts & SETANIM_LEGS)
		{
			if( (setAnimFlags & SETANIM_FLAG_RESTART) || (pm->ps->legsAnim & ~ANIM_TOGGLEBIT ) != anim )
			{
				PM_SetLegsAnimTimer(0);
			}
		}
	}

	PM_SetAnimFinal(setAnimParts, anim, setAnimFlags, blendTime);
}







#pragma endregion "bg_panimate"











// bg_saber.c
#pragma region "bg_saber"







int  PMove::PM_irand_timesync(int val1, int val2,qboolean useDefault, int defaultValue)
{
	int i;

	if (useDefault) {
//#ifdef DEBUG
//		// find bad calls: PM_irand_timesync\s*\([^,]+,\s*([^,\s]+)\s*,[^,]+,\s*\1\s*\)
//		if (defaultValue >= val2 || defaultValue < val1) {
//			Com_Printf("PM_irand_timesync(%d,%d,%d,%d), bad call", val1, val2, useDefault, defaultValue);
//		}
//#endif
		return defaultValue;
	}

	i = (val1-1) + (Q_random( &pml.seed )*(val2 - val1)) + 1;
	if (i < val1)
	{
		i = val1;
	}
	if (i > val2)
	{
		i = val2;
	}

	return i;
}

int  PMove::BG_GetMovePhysics(playerState_t* ps)
{
	if (!ps)
		return MV_JK2;
#if JK2_GAME
	if (ps->stats[STAT_RACEMODE])
		return (ps->stats[STAT_MOVEMENTSTYLE]);
	//else if ((g_movementStyle.integer >= MV_SIEGE && g_movementStyle.integer <= MV_WSW) || g_movementStyle.integer == MV_SP)
	//	return (g_movementStyle.integer);
	//else if (g_movementStyle.integer < MV_SIEGE)
	//	return 0;
	//else if (g_movementStyle.integer >= MV_NUMSTYLES)
	//	return MV_JK2;
#elif JK2_CGAME
	if (cgs.isJK2Pro) {
		return ps->stats[STAT_MOVEMENTSTYLE];
	}
	if (cgs.isTommyTernal && ps->stats[STAT_RACEMODE]) {
		if (!ps) return MV_JK2; // not sure why this is needed. from japro.
		return ps->stats[STAT_MOVEMENTSTYLE];
	}
	//if (cgs.gametype == GT_SIEGE)
	//	return MV_SIEGE;
#endif
	return MV_JK2; // this can happen when we die in racemode too!
}

void  PMove::BG_ForcePowerDrain( playerState_t *ps, forcePowers_t forcePower, int overrideAmt )
{
	//take away the power
	int	drain = overrideAmt;
	int moveStyle = BG_GetMovePhysics(ps);

	/*
	if (ps->powerups[PW_FORCE_BOON])
	{
		return;
	}
	*/
	//No longer grant infinite force with boon.

	if ( !drain )
	{
#if CLIENTSIDEONLY // we don't get any more info than this on the clientside (aka from demos) really.. and i dont wanna add useless array entries just to make this compile, eating up RAM
		switch (forcePower) {
		default:
			drain = 0;
			break;
		case FP_SEE:
			drain = forcePowerNeeded[ps->fd.forcePowerLevel_FP_SEE_][forcePower];
			break;
		case FP_LEVITATION:
			drain = forcePowerNeeded[ps->fd.forcePowerLevel_FP_LEVITATION_][forcePower];
			break;
		}
#else
		drain = forcePowerNeeded[ps->fd.forcePowerLevel[forcePower]][forcePower];
#endif
	}
	if ( !drain )
	{
		return;
	}

	if (forcePower == FP_LEVITATION && moveStyle != MV_CHARGEJUMP)
	{ //special case
		int jumpDrain = 0;

		if (ps->velocity[2] > 250)
		{
			jumpDrain = 20;
		}
		else if (ps->velocity[2] > 200)
		{
			jumpDrain = 16;
		}
		else if (ps->velocity[2] > 150)
		{
			jumpDrain = 12;
		}
		else if (ps->velocity[2] > 100)
		{
			jumpDrain = 8;
		}
		else if (ps->velocity[2] > 50)
		{
			jumpDrain = 6;
		}
		else if (ps->velocity[2] > 0)
		{
			jumpDrain = 4;
		}

		if (jumpDrain)
		{
			jumpDrain /= ps->fd.forcePowerLevel_FP_LEVITATION_;
		}

		ps->fd.forcePower -= jumpDrain;
		if ( ps->fd.forcePower < 0 )
		{
			ps->fd.forcePower = 0;
		}

		return;
	}

	ps->fd.forcePower -= drain;
	if ( ps->fd.forcePower < 0 )
	{
		ps->fd.forcePower = 0;
	}
}

qboolean  PMove::BG_EnoughForcePowerForMove(int cost)
{
	if (pm->ps->fd.forcePower < cost)
	{
		PM_AddEvent(EV_NOAMMO_GENERAL);
		return qfalse;
	}

	return qtrue;
}

// Silly, but I'm replacing these macros so they are shorter!
#define AFLAG_IDLE	(SETANIM_FLAG_NORMAL)
#define AFLAG_ACTIVE (/*SETANIM_FLAG_OVERRIDE | */SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS)
#define AFLAG_WAIT (SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS)
#define AFLAG_FINISH (SETANIM_FLAG_HOLD)
//
//saberMoveData_t	saberMoveData_general[LS_MOVE_MAX_GENERAL] = {//							NB:randomized
//	// name			anim				startQ	endQ	setanimflag		blend,	blocking	chain_idle		chain_attack	trailLen
//	{"None",		BOTH_STAND1_GENERAL,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_NO,		LS_NONE_GENERAL,		LS_NONE_GENERAL,		0	},	// LS_NONE_GENERAL		= 0,
//
//	// General movements with saber
//	{"Ready",		BOTH_STAND2_GENERAL,		Q_R,	Q_R,	AFLAG_IDLE,		350,	BLK_WIDE,	LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_READY_GENERAL,
//	{"Draw",		BOTH_STAND1TO2_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_DRAW_GENERAL,
//	{"Putaway",		BOTH_STAND2TO1_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	350,	BLK_NO,		LS_READY_GENERAL,		LS_S_R2L_GENERAL,		0	},	// LS_PUTAWAY_GENERAL,
//
//	// Attacks
//	//UL2LR
//	{"TL2BR Att",	BOTH_A1_TL_BR_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TL2BR_GENERAL,		LS_R_TL2BR_GENERAL,		200	},	// LS_A_TL2BR_GENERAL
//	//SLASH LEFT
//	{"L2R Att",		BOTH_A1__L__R_GENERAL,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_L2R_GENERAL,		LS_R_L2R_GENERAL,		200 },	// LS_A_L2R_GENERAL
//	//LL2UR
//	{"BL2TR Att",	BOTH_A1_BL_TR_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_TIGHT,	LS_R_BL2TR_GENERAL,		LS_R_BL2TR_GENERAL,		200	},	// LS_A_BL2TR_GENERAL
//	//LR2UL
//	{"BR2TL Att",	BOTH_A1_BR_TL_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_BR2TL_GENERAL,		LS_R_BR2TL_GENERAL,		200	},	// LS_A_BR2TL_GENERAL
//	//SLASH RIGHT
//	{"R2L Att",		BOTH_A1__R__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_R2L_GENERAL,		LS_R_R2L_GENERAL,		200 },// LS_A_R2L_GENERAL
//	//UR2LL
//	{"TR2BL Att",	BOTH_A1_TR_BL_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_TR2BL_GENERAL,		LS_R_TR2BL_GENERAL,		200	},	// LS_A_TR2BL_GENERAL
//	//SLASH DOWN
//	{"T2B Att",		BOTH_A1_T__B__GENERAL,		Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_R_T2B_GENERAL,		LS_R_T2B_GENERAL,		200	},	// LS_A_T2B_GENERAL
//	//special attacks
//	{"Back Stab",	BOTH_A2_STABBACK1_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACKSTAB_GENERAL
//	{"Back Att",	BOTH_ATTACK_BACK_GENERAL,	Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACK_GENERAL
//	{"CR Back Att",	BOTH_CROUCHATTACKBACK1_GENERAL,Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_BACK_CR_GENERAL
//	{"Lunge Att",	BOTH_LUNGE2_B__T__GENERAL,	Q_B,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_LUNGE_GENERAL
//	{"Jump Att",	BOTH_FORCELEAP2_T__B__GENERAL,Q_T,	Q_B,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_A_JUMP_T__B__GENERAL
//	{"Flip Stab",	BOTH_JUMPFLIPSTABDOWN_GENERAL,Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1_T___R_GENERAL,	200	},	// LS_A_FLIP_STAB_GENERAL
//	{"Flip Slash",	BOTH_JUMPFLIPSLASHDOWN1_GENERAL,Q_L,Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_T1__R_T__GENERAL,	200	},	// LS_A_FLIP_SLASH_GENERAL
//
//	//starts
//	{"TL2BR St",	BOTH_S1_S1_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TL2BR_GENERAL,		LS_A_TL2BR_GENERAL,		200	},	// LS_S_TL2BR_GENERAL
//	{"L2R St",		BOTH_S1_S1__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_L2R_GENERAL,		LS_A_L2R_GENERAL,		200	},	// LS_S_L2R_GENERAL
//	{"BL2TR St",	BOTH_S1_S1_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BL2TR_GENERAL,		LS_A_BL2TR_GENERAL,		200	},	// LS_S_BL2TR_GENERAL
//	{"BR2TL St",	BOTH_S1_S1_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_BR2TL_GENERAL,		LS_A_BR2TL_GENERAL,		200	},	// LS_S_BR2TL_GENERAL
//	{"R2L St",		BOTH_S1_S1__R_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_R2L_GENERAL,		LS_A_R2L_GENERAL,		200	},	// LS_S_R2L_GENERAL
//	{"TR2BL St",	BOTH_S1_S1_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_TR2BL_GENERAL,		LS_A_TR2BL_GENERAL,		200	},	// LS_S_TR2BL_GENERAL
//	{"T2B St",		BOTH_S1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_A_T2B_GENERAL,		LS_A_T2B_GENERAL,		200	},	// LS_S_T2B_GENERAL
//	
//	//returns
//	{"TL2BR Ret",	BOTH_R1_BR_S1_GENERAL,		Q_BR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_TL2BR_GENERAL
//	{"L2R Ret",		BOTH_R1__R_S1_GENERAL,		Q_R,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_L2R_GENERAL
//	{"BL2TR Ret",	BOTH_R1_TR_S1_GENERAL,		Q_TR,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_BL2TR_GENERAL
//	{"BR2TL Ret",	BOTH_R1_TL_S1_GENERAL,		Q_TL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_BR2TL_GENERAL
//	{"R2L Ret",		BOTH_R1__L_S1_GENERAL,		Q_L,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_R2L_GENERAL
//	{"TR2BL Ret",	BOTH_R1_BL_S1_GENERAL,		Q_BL,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_TR2BL_GENERAL
//	{"T2B Ret",		BOTH_R1_B__S1_GENERAL,		Q_B,	Q_R,	AFLAG_FINISH,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_READY_GENERAL,		200	},	// LS_R_T2B_GENERAL
//
//	//Transitions
//	{"BR2R Trans",	BOTH_T1_BR__R_GENERAL,		Q_BR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc bottom right to right
//	{"BR2TR Trans",	BOTH_T1_BR_TR_GENERAL,		Q_BR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc bottom right to top right		(use: BOTH_T1_TR_BR_GENERAL)
//	{"BR2T Trans",	BOTH_T1_BR_T__GENERAL,		Q_BR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc bottom right to top			(use: BOTH_T1_T__BR_GENERAL)
//	{"BR2TL Trans",	BOTH_T1_BR_TL_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast weak spin bottom right to top left
//	{"BR2L Trans",	BOTH_T1_BR__L_GENERAL,		Q_BR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast weak spin bottom right to left
//	{"BR2BL Trans",	BOTH_T1_BR_BL_GENERAL,		Q_BR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin bottom right to bottom left
//	{"R2BR Trans",	BOTH_T1__R_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc right to bottom right			(use: BOTH_T1_BR__R_GENERAL)
//	{"R2TR Trans",	BOTH_T1__R_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc right to top right
//	{"R2T Trans",	BOTH_T1__R_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast ar right to top				(use: BOTH_T1_T___R_GENERAL)
//	{"R2TL Trans",	BOTH_T1__R_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc right to top left
//	{"R2L Trans",	BOTH_T1__R__L_GENERAL,		Q_R,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast weak spin right to left
//	{"R2BL Trans",	BOTH_T1__R_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin right to bottom left
//	{"TR2BR Trans",	BOTH_T1_TR_BR_GENERAL,		Q_TR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc top right to bottom right
//	{"TR2R Trans",	BOTH_T1_TR__R_GENERAL,		Q_TR,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top right to right			(use: BOTH_T1__R_TR_GENERAL)
//	{"TR2T Trans",	BOTH_T1_TR_T__GENERAL,		Q_TR,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc top right to top				(use: BOTH_T1_T__TR_GENERAL)
//	{"TR2TL Trans",	BOTH_T1_TR_TL_GENERAL,		Q_TR,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc top right to top left
//	{"TR2L Trans",	BOTH_T1_TR__L_GENERAL,		Q_TR,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top right to left
//	{"TR2BL Trans",	BOTH_T1_TR_BL_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast weak spin top right to bottom left
//	{"T2BR Trans",	BOTH_T1_T__BR_GENERAL,		Q_T,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast arc top to bottom right
//	{"T2R Trans",	BOTH_T1_T___R_GENERAL,		Q_T,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top to right
//	{"T2TR Trans",	BOTH_T1_T__TR_GENERAL,		Q_T,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc top to top right
//	{"T2TL Trans",	BOTH_T1_T__TL_GENERAL,		Q_T,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc top to top left
//	{"T2L Trans",	BOTH_T1_T___L_GENERAL,		Q_T,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top to left
//	{"T2BL Trans",	BOTH_T1_T__BL_GENERAL,		Q_T,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc top to bottom left
//	{"TL2BR Trans",	BOTH_T1_TL_BR_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin top left to bottom right
//	{"TL2R Trans",	BOTH_T1_TL__R_GENERAL,		Q_TL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast arc top left to right			(use: BOTH_T1__R_TL_GENERAL)
//	{"TL2TR Trans",	BOTH_T1_TL_TR_GENERAL,		Q_TL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc top left to top right			(use: BOTH_T1_TR_TL_GENERAL)
//	{"TL2T Trans",	BOTH_T1_TL_T__GENERAL,		Q_TL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc top left to top				(use: BOTH_T1_T__TL_GENERAL)
//	{"TL2L Trans",	BOTH_T1_TL__L_GENERAL,		Q_TL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc top left to left				(use: BOTH_T1__L_TL_GENERAL)
//	{"TL2BL Trans",	BOTH_T1_TL_BL_GENERAL,		Q_TL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc top left to bottom left
//	{"L2BR Trans",	BOTH_T1__L_BR_GENERAL,		Q_L,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin left to bottom right
//	{"L2R Trans",	BOTH_T1__L__R_GENERAL,		Q_L,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast weak spin left to right
//	{"L2TR Trans",	BOTH_T1__L_TR_GENERAL,		Q_L,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast arc left to top right			(use: BOTH_T1_TR__L_GENERAL)
//	{"L2T Trans",	BOTH_T1__L_T__GENERAL,		Q_L,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc left to top				(use: BOTH_T1_T___L_GENERAL)
//	{"L2TL Trans",	BOTH_T1__L_TL_GENERAL,		Q_L,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc left to top left
//	{"L2BL Trans",	BOTH_T1__L_BL_GENERAL,		Q_L,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	//# Fast arc left to bottom left			(use: BOTH_T1_BL__L_GENERAL)
//	{"BL2BR Trans",	BOTH_T1_BL_BR_GENERAL,		Q_BL,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	//# Fast weak spin bottom left to bottom right
//	{"BL2R Trans",	BOTH_T1_BL__R_GENERAL,		Q_BL,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_A_R2L_GENERAL,		150	},	//# Fast weak spin bottom left to right
//	{"BL2TR Trans",	BOTH_T1_BL_TR_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	//# Fast weak spin bottom left to top right
//	{"BL2T Trans",	BOTH_T1_BL_T__GENERAL,		Q_BL,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	//# Fast arc bottom left to top			(use: BOTH_T1_T__BL_GENERAL)
//	{"BL2TL Trans",	BOTH_T1_BL_TL_GENERAL,		Q_BL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	//# Fast arc bottom left to top left		(use: BOTH_T1_TL_BL_GENERAL)
//	{"BL2L Trans",	BOTH_T1_BL__L_GENERAL,		Q_BL,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_A_L2R_GENERAL,		150	},	//# Fast arc bottom left to left
//
//	//Bounces
//	{"Bounce BR",	BOTH_B1_BR____GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_T1_BR_TR_GENERAL,	150	},	
//	{"Bounce R",	BOTH_B1__R____GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_T1__R__L_GENERAL,	150	},	
//	{"Bounce TR",	BOTH_B1_TR____GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_TR_TL_GENERAL,	150	},	
//	{"Bounce T",	BOTH_B1_T_____GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},	
//	{"Bounce TL",	BOTH_B1_TL____GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_T1_TL_TR_GENERAL,	150	},	
//	{"Bounce L",	BOTH_B1__L____GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_T1__L__R_GENERAL,	150	},	
//	{"Bounce BL",	BOTH_B1_BL____GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_T1_BL_TR_GENERAL,	150	},	
//
//	//Deflected attacks (like bounces, but slide off enemy saber, not straight back)
//	{"Deflect BR",	BOTH_D1_BR____GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TL2BR_GENERAL,		LS_T1_BR_TR_GENERAL,	150	},	
//	{"Deflect R",	BOTH_D1__R____GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_L2R_GENERAL,		LS_T1__R__L_GENERAL,	150	},	
//	{"Deflect TR",	BOTH_D1_TR____GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_TR_TL_GENERAL,	150	},	
//	{"Deflect T",	BOTH_B1_T_____GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},	
//	{"Deflect TL",	BOTH_D1_TL____GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BR2TL_GENERAL,		LS_T1_TL_TR_GENERAL,	150	},	
//	{"Deflect L",	BOTH_D1__L____GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_R2L_GENERAL,		LS_T1__L__R_GENERAL,	150	},	
//	{"Deflect BL",	BOTH_D1_BL____GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_TR2BL_GENERAL,		LS_T1_BL_TR_GENERAL,	150	},	
//	{"Deflect B",	BOTH_D1_B_____GENERAL,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_R_BL2TR_GENERAL,		LS_T1_T__BL_GENERAL,	150	},	
//
//	//Reflected attacks
//	{"Reflected BR",BOTH_V1_BR_S1_GENERAL,		Q_BR,	Q_BR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_BR_GENERAL
//	{"Reflected R",	BOTH_V1__R_S1_GENERAL,		Q_R,	Q_R,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1__R_GENERAL
//	{"Reflected TR",BOTH_V1_TR_S1_GENERAL,		Q_TR,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_TR_GENERAL
//	{"Reflected T",	BOTH_V1_T__S1_GENERAL,		Q_T,	Q_T,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_T__GENERAL
//	{"Reflected TL",BOTH_V1_TL_S1_GENERAL,		Q_TL,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_TL_GENERAL
//	{"Reflected L",	BOTH_V1__L_S1_GENERAL,		Q_L,	Q_L,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1__L_GENERAL
//	{"Reflected BL",BOTH_V1_BL_S1_GENERAL,		Q_BL,	Q_BL,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_BL_GENERAL
//	{"Reflected B",	BOTH_V1_B__S1_GENERAL,		Q_B,	Q_B,	AFLAG_ACTIVE,	100,	BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,	150	},//	LS_V1_B__GENERAL
//
//	// Broken parries
//	{"BParry Top",	BOTH_H1_S1_T__GENERAL,		Q_T,	Q_B,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
//	{"BParry UR",	BOTH_H1_S1_TR_GENERAL,		Q_TR,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
//	{"BParry UL",	BOTH_H1_S1_TL_GENERAL,		Q_TL,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
//	{"BParry LR",	BOTH_H1_S1_BL_GENERAL,		Q_BL,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
//	{"BParry Bot",	BOTH_H1_S1_B__GENERAL,		Q_B,	Q_T,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
//	{"BParry LL",	BOTH_H1_S1_BR_GENERAL,		Q_BR,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_NO,	LS_READY_GENERAL,		LS_READY_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
//
//	// Knockaways
//	{"Knock Top",	BOTH_K1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_T1_T__BR_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
//	{"Knock UR",	BOTH_K1_S1_TR_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_T1_TR__R_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
//	{"Knock UL",	BOTH_K1_S1_TL_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_T1_TL__L_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
//	{"Knock LR",	BOTH_K1_S1_BL_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_T1_BL_TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
//	{"Knock LL",	BOTH_K1_S1_BR_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_T1_BR_TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
//
//	// Parry
//	{"Parry Top",	BOTH_P1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		150	},	// LS_PARRY_UP_GENERAL,
//	{"Parry UR",	BOTH_P1_S1_TR_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		150	},	// LS_PARRY_UR_GENERAL,
//	{"Parry UL",	BOTH_P1_S1_TL_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		150	},	// LS_PARRY_UL_GENERAL,
//	{"Parry LR",	BOTH_P1_S1_BL_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		150	},	// LS_PARRY_LR_GENERAL,
//	{"Parry LL",	BOTH_P1_S1_BR_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		150	},	// LS_PARRY_LL_GENERAL
//
//	// Reflecting a missile
//	{"Reflect Top",	BOTH_P1_S1_T__GENERAL,		Q_R,	Q_T,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_T2B_GENERAL,		300	},	// LS_PARRY_UP_GENERAL,
//	{"Reflect UR",	BOTH_P1_S1_TL_GENERAL,		Q_R,	Q_TR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BR2TL_GENERAL,		LS_A_TL2BR_GENERAL,		300	},	// LS_PARRY_UR_GENERAL,
//	{"Reflect UL",	BOTH_P1_S1_TR_GENERAL,		Q_R,	Q_TL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_BL2TR_GENERAL,		LS_A_TR2BL_GENERAL,		300	},	// LS_PARRY_UL_GENERAL,
//	{"Reflect LR",	BOTH_P1_S1_BR_GENERAL,		Q_R,	Q_BL,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TR2BL_GENERAL,		LS_A_BL2TR_GENERAL,		300	},	// LS_PARRY_LR_GENERAL
//	{"Reflect LL",	BOTH_P1_S1_BL_GENERAL,		Q_R,	Q_BR,	AFLAG_ACTIVE,	50,		BLK_WIDE,	LS_R_TL2BR_GENERAL,		LS_A_BR2TL_GENERAL,		300	},	// LS_PARRY_LL_GENERAL,
//
//	// JKA cartwheels
//	{ "DualJumpAtkL_A",BOTH_ARIAL_LEFT_GENERAL,	Q_R,	Q_TL,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_A_TL2BR_GENERAL,		200 },	// LS_JUMPATTACK_ARIAL_LEFT_GENERAL
//	{ "DualJumpAtkR_A",BOTH_ARIAL_RIGHT_GENERAL,Q_R,	Q_TR,	AFLAG_ACTIVE,	100,	BLK_TIGHT,	LS_READY_GENERAL,		LS_A_TR2BL_GENERAL,		200 },	// LS_JUMPATTACK_ARIAL_RIGHT_GENERAL
//};


int transitionMove[Q_NUM_QUADS][Q_NUM_QUADS] = 
{
	{
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1_BR__R_GENERAL,//40
		LS_T1_BR_TR_GENERAL,
		LS_T1_BR_T__GENERAL,
		LS_T1_BR_TL_GENERAL,
		LS_T1_BR__L_GENERAL,
		LS_T1_BR_BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1__R_BR_GENERAL,//46
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1__R_TR_GENERAL,
		LS_T1__R_T__GENERAL,
		LS_T1__R_TL_GENERAL,
		LS_T1__R__L_GENERAL,
		LS_T1__R_BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1_TR_BR_GENERAL,//52
		LS_T1_TR__R_GENERAL,
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1_TR_T__GENERAL,
		LS_T1_TR_TL_GENERAL,
		LS_T1_TR__L_GENERAL,
		LS_T1_TR_BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1_T__BR_GENERAL,//58
		LS_T1_T___R_GENERAL,
		LS_T1_T__TR_GENERAL,
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1_T__TL_GENERAL,
		LS_T1_T___L_GENERAL,
		LS_T1_T__BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1_TL_BR_GENERAL,//64
		LS_T1_TL__R_GENERAL,
		LS_T1_TL_TR_GENERAL,
		LS_T1_TL_T__GENERAL,
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1_TL__L_GENERAL,
		LS_T1_TL_BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1__L_BR_GENERAL,//70
		LS_T1__L__R_GENERAL,
		LS_T1__L_TR_GENERAL,
		LS_T1__L_T__GENERAL,
		LS_T1__L_TL_GENERAL,
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_T1__L_BL_GENERAL,
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1_BL_BR_GENERAL,//76
		LS_T1_BL__R_GENERAL,
		LS_T1_BL_TR_GENERAL,
		LS_T1_BL_T__GENERAL,
		LS_T1_BL_TL_GENERAL,
		LS_T1_BL__L_GENERAL,
		LS_NONE_GENERAL,	//Can't transition to same pos!
		LS_NONE_GENERAL,	//No transitions to bottom, and no anims start there, so shouldn't need any
	},
	{
		LS_T1_BL_BR_GENERAL,//NOTE: there are no transitions from bottom, so re-use the bottom right transitions
		LS_T1_BR__R_GENERAL,
		LS_T1_BR_TR_GENERAL,
		LS_T1_BR_T__GENERAL,
		LS_T1_BR_TL_GENERAL,
		LS_T1_BR__L_GENERAL,
		LS_T1_BR_BL_GENERAL,
		LS_NONE_GENERAL		//No transitions to bottom, and no anims start there, so shouldn't need any
	}
};

saberMoveName_t  PMove::PM_AttackMoveForQuad( int quad )
{
	switch ( quad )
	{
	case Q_B:
	case Q_BR:
		return LS_A_BR2TL_GENERAL;
		break;
	case Q_R:
		return LS_A_R2L_GENERAL;
		break;
	case Q_TR:
		return LS_A_TR2BL_GENERAL;
		break;
	case Q_T:
		return LS_A_T2B_GENERAL;
		break;
	case Q_TL:
		return LS_A_TL2BR_GENERAL;
		break;
	case Q_L:
		return LS_A_L2R_GENERAL;
		break;
	case Q_BL:
		return LS_A_BL2TR_GENERAL;
		break;
	}
	return LS_NONE_GENERAL;
}


int  PMove::PM_SaberAnimTransitionAnim( int curmove, int newmove )
{
	int retmove = newmove;
	if (curmove == LS_READY_GENERAL || !pm->modParms.raceMode && pm->mineSwitchFix &&BG_SaberInIdle(curmove)) // in racemode we stay on vanilla behavior always. prolly irrelevant but eh
	{//just standing there
		switch ( newmove )
		{
		case LS_A_TL2BR_GENERAL:
		case LS_A_L2R_GENERAL:
		case LS_A_BL2TR_GENERAL:
		case LS_A_BR2TL_GENERAL:
		case LS_A_R2L_GENERAL:
		case LS_A_TR2BL_GENERAL:
		case LS_A_T2B_GENERAL:
			//transition is the start
			retmove = LS_S_TL2BR_GENERAL + (newmove-LS_A_TL2BR_GENERAL);
			break;
		}
	}
	else
	{
		switch ( newmove )
		{
		//transitioning to ready pose
		case LS_READY_GENERAL:
			switch ( curmove )
			{
			//transitioning from an attack
			case LS_A_TL2BR_GENERAL:
			case LS_A_L2R_GENERAL:
			case LS_A_BL2TR_GENERAL:
			case LS_A_BR2TL_GENERAL:
			case LS_A_R2L_GENERAL:
			case LS_A_TR2BL_GENERAL:
			case LS_A_T2B_GENERAL:
				//transition is the return
				retmove = LS_R_TL2BR_GENERAL + (newmove-LS_A_TL2BR_GENERAL);
				break;
			}
			break;
		//transitioning to an attack
		case LS_A_TL2BR_GENERAL:
		case LS_A_L2R_GENERAL:
		case LS_A_BL2TR_GENERAL:
		case LS_A_BR2TL_GENERAL:
		case LS_A_R2L_GENERAL:
		case LS_A_TR2BL_GENERAL:
		case LS_A_T2B_GENERAL:
			if ( newmove == curmove && jk2gameplay != VERSION_1_02 )
			{
				//going into an attack
				if ( PM_SaberKataDone( curmove, newmove ) )
				{//done with this kata, must return to ready before attack again
					retmove = LS_R_TL2BR_GENERAL + (newmove-LS_A_TL2BR_GENERAL);
				}
				else
				{//okay to chain to another attack
					retmove = transitionMove[saberMoveData_general[curmove].endQuad][saberMoveData_general[newmove].startQuad];
				}
			}
			else if ( saberMoveData_general[curmove].endQuad == saberMoveData_general[newmove].startQuad )
			{//new move starts from same quadrant
				retmove = newmove;
			}
			else
			{
				switch ( curmove )
				{
				//transitioning from an attack
				case LS_D1_BR_GENERAL:
				case LS_D1__R_GENERAL:
				case LS_D1_TR_GENERAL:
				case LS_D1_T__GENERAL:
				case LS_D1_TL_GENERAL:
				case LS_D1__L_GENERAL:
				case LS_D1_BL_GENERAL:
				case LS_D1_B__GENERAL:
					if ( jk2gameplay == VERSION_1_02 ) break;
				case LS_A_TL2BR_GENERAL:
				case LS_A_L2R_GENERAL:
				case LS_A_BL2TR_GENERAL:
				case LS_A_BR2TL_GENERAL:
				case LS_A_R2L_GENERAL:
				case LS_A_TR2BL_GENERAL:
				case LS_A_T2B_GENERAL:
					retmove = transitionMove[saberMoveData_general[curmove].endQuad][saberMoveData_general[newmove].startQuad];
					break;
				//transitioning from a return
				case LS_K1_T__GENERAL:
				case LS_K1_TR_GENERAL:
				case LS_K1_TL_GENERAL:
				case LS_K1_BR_GENERAL:
				case LS_K1_BL_GENERAL:
				case LS_V1_BR_GENERAL:
				case LS_V1__R_GENERAL:
				case LS_V1_TR_GENERAL:
				case LS_V1_T__GENERAL:
				case LS_V1_TL_GENERAL:
				case LS_V1__L_GENERAL:
				case LS_V1_BL_GENERAL:
				case LS_V1_B__GENERAL:
				case LS_H1_T__GENERAL:
				case LS_H1_TR_GENERAL:
				case LS_H1_TL_GENERAL:
				case LS_H1_BR_GENERAL:
				case LS_H1_BL_GENERAL:
					if ( jk2gameplay == VERSION_1_02 ) break;
				case LS_R_TL2BR_GENERAL:
				case LS_R_L2R_GENERAL:
				case LS_R_BL2TR_GENERAL:
				case LS_R_BR2TL_GENERAL:
				case LS_R_R2L_GENERAL:
				case LS_R_TR2BL_GENERAL:
				case LS_R_T2B_GENERAL:
				//transitioning from a bounce
				/*
				case LS_BOUNCE_UL2LL_GENERAL:
				case LS_BOUNCE_LL2UL_GENERAL:
				case LS_BOUNCE_L2LL_GENERAL:
				case LS_BOUNCE_L2UL_GENERAL:
				case LS_BOUNCE_UR2LR_GENERAL:
				case LS_BOUNCE_LR2UR_GENERAL:
				case LS_BOUNCE_R2LR_GENERAL:
				case LS_BOUNCE_R2UR_GENERAL:
				case LS_BOUNCE_TOP_GENERAL:
				case LS_OVER_UR2UL_GENERAL:
				case LS_OVER_UL2UR_GENERAL:
				case LS_BOUNCE_UR_GENERAL:
				case LS_BOUNCE_UL_GENERAL:
				case LS_BOUNCE_LR_GENERAL:
				case LS_BOUNCE_LL_GENERAL:
				*/
				//transitioning from a parry/reflection/knockaway/broken parry
				case LS_PARRY_UP_GENERAL:
				case LS_PARRY_UR_GENERAL:
				case LS_PARRY_UL_GENERAL:
				case LS_PARRY_LR_GENERAL:
				case LS_PARRY_LL_GENERAL:
				case LS_REFLECT_UP_GENERAL:
				case LS_REFLECT_UR_GENERAL:
				case LS_REFLECT_UL_GENERAL:
				case LS_REFLECT_LR_GENERAL:
				case LS_REFLECT_LL_GENERAL:
					retmove = transitionMove[saberMoveData_general[curmove].endQuad][saberMoveData_general[newmove].startQuad];
					break;
				//NB: transitioning from transitions is fine
				}
			}
			break;
		//transitioning to any other anim is not supported
		}
	}

	if ( retmove == LS_NONE_GENERAL )
	{
		return newmove;
	}

	return retmove;
}

int  PMove::PM_SaberMoveQuadrantForMovement( usercmd_t *ucmd )
{
	if ( ucmd->rightmove > 0 )
	{//moving right
		if ( ucmd->forwardmove > 0 )
		{//forward right = TL2BR slash
			return Q_TL;
		}
		else if ( ucmd->forwardmove < 0 )
		{//backward right = BL2TR uppercut
			return Q_BL;
		}
		else
		{//just right is a left slice
			return Q_L;
		}
	}
	else if ( ucmd->rightmove < 0 )
	{//moving left
		if ( ucmd->forwardmove > 0 )
		{//forward left = TR2BL slash
			return Q_TR;
		}
		else if ( ucmd->forwardmove < 0 )
		{//backward left = BR2TL uppercut
			return Q_BR;
		}
		else
		{//just left is a right slice
			return Q_R;
		}
	}
	else
	{//not moving left or right
		if ( ucmd->forwardmove > 0 )
		{//forward= T2B slash
			return Q_T;
		}
		else if ( ucmd->forwardmove < 0 )
		{//backward= T2B slash	//or B2T uppercut?
			return Q_T;
		}
		else
		{//Not moving at all
			return Q_R;
		}
	}
}

//===================================================================
qboolean  PMove::PM_SaberInBounce( int move )
{
	if ( move >= LS_B1_BR_GENERAL && move <= LS_B1_BL_GENERAL )
	{
		return qtrue;
	}
	if ( move >= LS_D1_BR_GENERAL && move <= LS_D1_BL_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberInTransition( int move )
{
	if ( move >= LS_T1_BR__R_GENERAL && move <= LS_T1_BL__L_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}

int saberMoveTransitionAngle[Q_NUM_QUADS][Q_NUM_QUADS] = 
{
	{
		0,//Q_BR,Q_BR,
		45,//Q_BR,Q_R,
		90,//Q_BR,Q_TR,
		135,//Q_BR,Q_T,
		180,//Q_BR,Q_TL,
		215,//Q_BR,Q_L,
		270,//Q_BR,Q_BL,
		45,//Q_BR,Q_B,
	},
	{
		45,//Q_R,Q_BR,
		0,//Q_R,Q_R,
		45,//Q_R,Q_TR,
		90,//Q_R,Q_T,
		135,//Q_R,Q_TL,
		180,//Q_R,Q_L,
		215,//Q_R,Q_BL,
		90,//Q_R,Q_B,
	},
	{
		90,//Q_TR,Q_BR,
		45,//Q_TR,Q_R,
		0,//Q_TR,Q_TR,
		45,//Q_TR,Q_T,
		90,//Q_TR,Q_TL,
		135,//Q_TR,Q_L,
		180,//Q_TR,Q_BL,
		135,//Q_TR,Q_B,
	},
	{
		135,//Q_T,Q_BR,
		90,//Q_T,Q_R,
		45,//Q_T,Q_TR,
		0,//Q_T,Q_T,
		45,//Q_T,Q_TL,
		90,//Q_T,Q_L,
		135,//Q_T,Q_BL,
		180,//Q_T,Q_B,
	},
	{
		180,//Q_TL,Q_BR,
		135,//Q_TL,Q_R,
		90,//Q_TL,Q_TR,
		45,//Q_TL,Q_T,
		0,//Q_TL,Q_TL,
		45,//Q_TL,Q_L,
		90,//Q_TL,Q_BL,
		135,//Q_TL,Q_B,
	},
	{
		215,//Q_L,Q_BR,
		180,//Q_L,Q_R,
		135,//Q_L,Q_TR,
		90,//Q_L,Q_T,
		45,//Q_L,Q_TL,
		0,//Q_L,Q_L,
		45,//Q_L,Q_BL,
		90,//Q_L,Q_B,
	},
	{
		270,//Q_BL,Q_BR,
		215,//Q_BL,Q_R,
		180,//Q_BL,Q_TR,
		135,//Q_BL,Q_T,
		90,//Q_BL,Q_TL,
		45,//Q_BL,Q_L,
		0,//Q_BL,Q_BL,
		45,//Q_BL,Q_B,
	},
	{
		45,//Q_B,Q_BR,
		90,//Q_B,Q_R,
		135,//Q_B,Q_TR,
		180,//Q_B,Q_T,
		135,//Q_B,Q_TL,
		90,//Q_B,Q_L,
		45,//Q_B,Q_BL,
		0//Q_B,Q_B,
	}
};

int  PMove::PM_SaberAttackChainAngle( int move1, int move2 )
{
	if ( move1 == -1 || move2 == -1 )
	{
		return -1;
	}
	return saberMoveTransitionAngle[saberMoveData_general[move1].endQuad][saberMoveData_general[move2].startQuad];
}

qboolean  PMove::PM_SaberKataDone_1_02( void )
{
	if ( (pm->ps->fd.saberAnimLevel >= FORCE_LEVEL_3 && pm->ps->saberAttackChainCount > PM_irand_timesync( 0, 1 + pml.randomAdd, pm->modParms.raceMode, 0 )) ||
		( pm->ps->fd.saberAnimLevel == FORCE_LEVEL_2 && pm->ps->saberAttackChainCount > PM_irand_timesync( 2, 5 + pml.randomAdd, pm->modParms.raceMode, 4) ) )
	{
		return qtrue;
	}
	return qfalse;
}

qboolean  PMove::PM_SaberKataDone(int curmove, int newmove)
{
	if ( pm->ps->fd.saberAnimLevel == FORCE_LEVEL_3 )
	{
		if ( curmove == LS_NONE_GENERAL || newmove == LS_NONE_GENERAL )
		{
			if ( pm->ps->fd.saberAnimLevel >= FORCE_LEVEL_3 && pm->ps->saberAttackChainCount > PM_irand_timesync( 0, 1 + pml.randomAdd, pm->modParms.raceMode, 0) )
			{
				return qtrue;
			}
		}
		else if ( pm->ps->saberAttackChainCount > PM_irand_timesync( 2, 3 + pml.randomAdd, pm->modParms.raceMode, 2) )
		{
			return qtrue;
		}
		else if ( pm->ps->saberAttackChainCount > 0 )
		{
			int chainAngle = PM_SaberAttackChainAngle( curmove, newmove );
			if ( chainAngle < 135 || chainAngle > 215 )
			{//if trying to chain to a move that doesn't continue the momentum
				return qtrue;
			}
			else if ( chainAngle == 180 )
			{//continues the momentum perfectly, allow it to chain 66% of the time
				if ( pm->ps->saberAttackChainCount > 1 )
				{
					return qtrue;
				}
			}
			else
			{//would continue the movement somewhat, 50% chance of continuing
				if ( pm->ps->saberAttackChainCount > 2 )
				{
					return qtrue;
				}
			}
		}
	}
	else 
	{//Perhaps have chainAngle influence fast and medium chains as well? For now, just do level 3.
		if (newmove == LS_A_TL2BR_GENERAL ||
			newmove == LS_A_L2R_GENERAL ||
			newmove == LS_A_BL2TR_GENERAL ||
			newmove == LS_A_BR2TL_GENERAL ||
			newmove == LS_A_R2L_GENERAL ||
			newmove == LS_A_TR2BL_GENERAL )
		{ //lower chaining tolerance for spinning saber anims
			int chainTolerance;

			if (pm->ps->fd.saberAnimLevel == FORCE_LEVEL_1)
			{
				chainTolerance = 5;
			}
			else
			{
				chainTolerance = 3;
			}

			if (pm->ps->saberAttackChainCount >= chainTolerance && PM_irand_timesync(1, pm->ps->saberAttackChainCount + pml.randomAdd, pm->modParms.raceMode, pm->ps->saberAttackChainCount-1) > chainTolerance)
			{
				return qtrue;
			}
		}
		if ( pm->ps->fd.saberAnimLevel == FORCE_LEVEL_2 && pm->ps->saberAttackChainCount > PM_irand_timesync( 2, 5 + pml.randomAdd, pm->modParms.raceMode, 4) )
		{
			return qtrue;
		}
	}
	return qfalse;
}

void  PMove::PM_SetAnimFrame( playerState_t *gent, int frame, qboolean torso, qboolean legs )
{
	gent->saberLockFrame = frame;
}

void  PMove::PM_SaberLockBreak( playerState_t *genemy, qboolean victory )
{
	int	winAnim = BOTH_STAND1_GENERAL;
	// int loseAnim = BOTH_STAND1_GENERAL;
	qboolean punishLoser = qfalse;

	switch ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_BF2LOCK_GENERAL:
		pm->ps->saberMove = LS_A_T2B_GENERAL;
		winAnim = BOTH_A3_T__B__GENERAL;
		if ( !victory )
		{//no-one won
			genemy->saberMove = LS_A_T2B_GENERAL;
			// loseAnim = winAnim;
		}
		else
		{
			punishLoser = qtrue;
		}
		break;
	case BOTH_BF1LOCK_GENERAL:
		pm->ps->saberMove = LS_K1_T__GENERAL;
		winAnim = BOTH_K1_S1_T__GENERAL;
		if ( !victory )
		{//no-one won
			genemy->saberMove = LS_K1_T__GENERAL;
			// loseAnim = winAnim;
		}
		else
		{
			punishLoser = qtrue;
		}
		break;
	case BOTH_CWCIRCLELOCK_GENERAL:
		winAnim = BOTH_CWCIRCLEBREAK_GENERAL;
		if ( !victory )
		{//no-one won
			// loseAnim = winAnim;
		}
		else
		{
			genemy->saberMove = LS_H1_BL_GENERAL;
			genemy->saberBlocked = BLOCKED_PARRY_BROKEN;
			punishLoser = qtrue;
		}
		break;
	case BOTH_CCWCIRCLELOCK_GENERAL:
		winAnim = BOTH_CCWCIRCLEBREAK_GENERAL;
		if ( !victory )
		{//no-one won
			// loseAnim = winAnim;
		}
		else
		{
			genemy->saberMove = LS_H1_BR_GENERAL;
			genemy->saberBlocked = BLOCKED_PARRY_BROKEN;
			punishLoser = qtrue;
		}
		break;
	}
	PM_SetAnim( SETANIM_BOTH, winAnim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, -1 );

	if (punishLoser)
	{ //someone lost the lock, so punish them by knocking them down
		vec3_t oppDir;

		int strength = 8;

		VectorSubtract(genemy->origin, pm->ps->origin, oppDir);
		VectorNormalize(oppDir);

		genemy->forceHandExtend = HANDEXTEND_KNOCKDOWN;
		genemy->forceHandExtendTime = pm->cmd.serverTime + 1100;
		genemy->forceDodgeAnim = 0; //this toggles between 1 and 0, when it's 1 we should play the get up anim

		//genemy->otherKiller = pm->ps->clientNum;
		//genemy->otherKillerTime = pm->cmd.serverTime + 5000;
		//genemy->otherKillerDebounceTime = pm->cmd.serverTime + 100;

		genemy->velocity[0] = oppDir[0]*(strength*40);
		genemy->velocity[1] = oppDir[1]*(strength*40);
		genemy->velocity[2] = 100;

		pm->checkDuelLoss = genemy->clientNum+1;
	}
	else
	{ //If no one lost, then shove each player away from the other
		vec3_t oppDir;

		int strength = 4;

		VectorSubtract(genemy->origin, pm->ps->origin, oppDir);
		VectorNormalize(oppDir);
		genemy->velocity[0] = oppDir[0]*(strength*40);
		genemy->velocity[1] = oppDir[1]*(strength*40);
		genemy->velocity[2] = 150;

		VectorSubtract(pm->ps->origin, genemy->origin, oppDir);
		VectorNormalize(oppDir);
		pm->ps->velocity[0] = oppDir[0]*(strength*40);
		pm->ps->velocity[1] = oppDir[1]*(strength*40);
		pm->ps->velocity[2] = 150;

		genemy->forceHandExtend = HANDEXTEND_WEAPONREADY;
	}

	pm->ps->weaponTime = 0;
	genemy->weaponTime = 0;

	pm->ps->saberLockTime = genemy->saberLockTime = 0;
	pm->ps->saberLockFrame = genemy->saberLockFrame = 0;
	pm->ps->saberLockEnemy = genemy->saberLockEnemy = 0;

	pm->ps->forceHandExtend = HANDEXTEND_WEAPONREADY;

	PM_AddEvent( EV_JUMP_GENERAL );
	if ( !victory )
	{//no-one won
		BG_AddPredictableEventToPlayerstate(EV_JUMP_GENERAL, 0, genemy);
	}
	else
	{
		if ( (jk2gameplay == VERSION_1_02 ? Q_irand( 0, 1 + pml.randomAdd, pm->modParms.raceMode, 0) : PM_irand_timesync( 0, 1 + pml.randomAdd, pm->modParms.raceMode, 0)) )
		{
			BG_AddPredictableEventToPlayerstate(EV_JUMP_GENERAL, PM_irand_timesync( 0, 75 + pml.randomAdd, pm->modParms.raceMode, 74), genemy); // what is this eventparm? doesnt seem used for anything?
		}
	}
}

void  PMove::PM_SaberLocked( void )
{
	int	remaining = 0;

	playerState_t *genemy = pm->bgClients[pm->ps->saberLockEnemy];
	if ( !genemy )
	{
		return;
	}
	if ( ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF2LOCK_GENERAL ||
			(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF1LOCK_GENERAL ||
			(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CWCIRCLELOCK_GENERAL ||
			(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CCWCIRCLELOCK_GENERAL )
		&& ( (genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF2LOCK_GENERAL ||
			(genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF1LOCK_GENERAL ||
			(genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CWCIRCLELOCK_GENERAL ||
			(genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CCWCIRCLELOCK_GENERAL )
		)
	{
		float dist = 0;

		pm->ps->torsoTimer = 0;
		pm->ps->weaponTime = 0;
		genemy->torsoTimer = 0;
		genemy->weaponTime = 0;

		dist = DistanceSquared(pm->ps->origin,genemy->origin);
		if ( dist < 64 || dist > 6400 )
		{//between 8 and 80 from each other
			PM_SaberLockBreak( genemy, qfalse );
			return;
		}
		if ( (pm->cmd.buttons & BUTTON_ATTACK) || pm->ps->saberLockAdvance )
		{//holding attack
			animation_t *anim;

			if (pm->ps->saberLockAdvance)
			{//tapping
				animation_t *anim;
				float		currentFrame;
				int			curFrame;
				int			strength = 1;

				pm->ps->saberLockAdvance = qfalse;

				anim = &pm->animations[pm->ps->torsoAnim&~ANIM_TOGGLEBIT];
	
				currentFrame = pm->ps->saberLockFrame;

				strength = NONETWORK_FORCEPOWERLEVEL(pm->ps, FP_SABERATTACK) + 1;

				if ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CCWCIRCLELOCK_GENERAL ||
					(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF2LOCK_GENERAL )
				{
					curFrame = floor( currentFrame )-strength;
					//drop my frame one
					if ( curFrame <= anim->firstFrame )
					{//I won!  Break out
						PM_SaberLockBreak( genemy, qtrue );
						return;
					}
					else
					{
						PM_SetAnimFrame( pm->ps, curFrame, qtrue, qtrue );
						remaining = curFrame-anim->firstFrame;
					}
				}
				else
				{
					curFrame = ceil( currentFrame )+strength;
					//advance my frame one
					if ( curFrame >= anim->firstFrame+anim->numFrames )
					{//I won!  Break out
						PM_SaberLockBreak( genemy, qtrue );
						return;
					}
					else
					{
						PM_SetAnimFrame( pm->ps, curFrame, qtrue, qtrue );
						remaining = anim->firstFrame+anim->numFrames-curFrame;
					}
				}
				if ( !(jk2gameplay == VERSION_1_02 ? Q_irand( 0, 2 + pml.randomAdd, pm->modParms.raceMode, 1) : PM_irand_timesync( 0, 2 + pml.randomAdd, pm->modParms.raceMode, 1)) )
				{
					PM_AddEvent( EV_JUMP_GENERAL );
				}
			}
			else
			{
				return;
			}

			anim = &pm->animations[(genemy->torsoAnim&~ANIM_TOGGLEBIT)];

			if ( (genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CWCIRCLELOCK_GENERAL ||
				(genemy->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF1LOCK_GENERAL )
			{
				if ( !(jk2gameplay == VERSION_1_02 ? Q_irand( 0, 2 + pml.randomAdd, pm->modParms.raceMode, 1) : PM_irand_timesync( 0, 2 + pml.randomAdd, pm->modParms.raceMode, 1)) )
				{
					BG_AddPredictableEventToPlayerstate(EV_PAIN_GENERAL, floor((float)80/100*100.0f), genemy);
				}
				PM_SetAnimFrame( genemy, anim->firstFrame+remaining, qtrue, qtrue );
			}
			else
			{
				PM_SetAnimFrame( genemy, anim->firstFrame+anim->numFrames-remaining, qtrue, qtrue );
			}
		}
	}
	else
	{//something broke us out of it
		PM_SaberLockBreak( genemy, qfalse );
	}
}

qboolean  PMove::PM_SaberInBrokenParry( int move )
{
	if ( move >= LS_V1_BR_GENERAL && move <= LS_V1_B__GENERAL && jk2gameplay != VERSION_1_02)
	{
		return qtrue;
	}
	if ( move >= LS_H1_T__GENERAL && move <= LS_H1_BL_GENERAL )
	{
		return qtrue;
	}
	return qfalse;
}


int  PMove::PM_BrokenParryForParry( int move )
{
	switch ( move )
	{
	case LS_PARRY_UP_GENERAL:
		return LS_H1_T__GENERAL;
		break;
	case LS_PARRY_UR_GENERAL:
		return LS_H1_TR_GENERAL;
		break;
	case LS_PARRY_UL_GENERAL:
		return LS_H1_TL_GENERAL;
		break;
	case LS_PARRY_LR_GENERAL:
		return LS_H1_BL_GENERAL;
		break;
	case LS_PARRY_LL_GENERAL:
		return LS_H1_BR_GENERAL;
		break;
	case LS_READY_GENERAL:
		return LS_H1_B__GENERAL;
		break;
	}
	return LS_NONE_GENERAL;
}

#define BACK_STAB_DISTANCE 128

qboolean  PMove::PM_CanBackstab(void)
{
	trace_t tr;
	vec3_t flatAng;
	vec3_t fwd, back;
	vec3_t trmins = {-15, -15, -8};
	vec3_t trmaxs = {15, 15, 8};

	VectorCopy(pm->ps->viewangles, flatAng);
	flatAng[PITCH] = 0;

	AngleVectors(flatAng, fwd, 0, 0);

	back[0] = pm->ps->origin[0] - fwd[0]*BACK_STAB_DISTANCE;
	back[1] = pm->ps->origin[1] - fwd[1]*BACK_STAB_DISTANCE;
	back[2] = pm->ps->origin[2] - fwd[2]*BACK_STAB_DISTANCE;

#if CLIENTSIDEONLY
	PM_Trace(&tr, pm->ps->origin, trmins, trmaxs, back, pm->ps->clientNum, MASK_PLAYERSOLID);
#else
	pm->rawtrace(&tr, pm->ps->origin, trmins, trmaxs, back, pm->ps->clientNum, MASK_PLAYERSOLID);
#endif

	if (tr.fraction != 1.0 && tr.entityNum >= 0 && tr.entityNum < MAX_CLIENTS)
	{ //We don't have real entity access here so we can't do an indepth check. But if it's a client and it's behind us, I guess that's reason enough to stab backward
		return qtrue;
	}

	return qfalse;
}

saberMoveName_t  PMove::PM_SaberFlipOverAttackMove(trace_t *tr)
{
	vec3_t fwdAngles, jumpFwd;
	float zDiff = 0;
	playerState_t *psData;

	VectorCopy( pm->ps->viewangles, fwdAngles );
	fwdAngles[PITCH] = fwdAngles[ROLL] = 0;
	AngleVectors( fwdAngles, jumpFwd, NULL, NULL );
	VectorScale( jumpFwd, 50, pm->ps->velocity );
	pm->ps->velocity[2] = 400;

	psData = pm->bgClients[tr->entityNum];

	//go higher for enemies higher than you, lower for those lower than you
	if (psData)
	{
		zDiff = psData->origin[2] - pm->ps->origin[2];
	}
	else
	{
		zDiff = 0;
	}
	pm->ps->velocity[2] += (zDiff)*1.5f;

	//clamp to decent-looking values
	if ( zDiff <= 0 && pm->ps->velocity[2] < 200 )
	{//if we're on same level, don't let me jump so low, I clip into the ground
		pm->ps->velocity[2] = 200;
	}
	else if ( pm->ps->velocity[2] < 100 )
	{
		pm->ps->velocity[2] = 100;
	}
	else if ( pm->ps->velocity[2] > 400 )
	{
		pm->ps->velocity[2] = 400;
	}

	PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height

	PM_AddEvent( EV_JUMP_GENERAL );
#if !CLIENTSIDEONLY
	pm->ps->fd.forceJumpSound = 1;
#endif
	pm->cmd.upmove = 0;

	if ( ((jk2gameplay == VERSION_1_02 && !pm->unlockRandom) ? Q_irand( 0, 1, pm->modParms.raceMode, 0) : PM_irand_timesync( 0, 1 + pml.randomAdd, pm->modParms.raceMode, 0)) ) // if we unlock random make sure we are synced. normally just 0 always anyway so nothing changes (except for esoteric qvm random propagation which for our little experiment we'll ignore)
	{
		return LS_A_FLIP_STAB_GENERAL;
	}
	else
	{
		return LS_A_FLIP_SLASH_GENERAL;
	}
}

#define FLIPHACK_DISTANCE 200

qboolean  PMove::PM_SomeoneInFront(trace_t *tr)
{ //Also a very simplified version of the sp counterpart
	vec3_t flatAng;
	vec3_t fwd, back;
	vec3_t trmins = {-15, -15, -8};
	vec3_t trmaxs = {15, 15, 8};

	VectorCopy(pm->ps->viewangles, flatAng);
	flatAng[PITCH] = 0;

	AngleVectors(flatAng, fwd, 0, 0);

	back[0] = pm->ps->origin[0] + fwd[0]*FLIPHACK_DISTANCE;
	back[1] = pm->ps->origin[1] + fwd[1]*FLIPHACK_DISTANCE;
	back[2] = pm->ps->origin[2] + fwd[2]*FLIPHACK_DISTANCE;

	PM_Trace(tr, pm->ps->origin, trmins, trmaxs, back, pm->ps->clientNum, MASK_PLAYERSOLID);

	if (tr->fraction != 1.0 && tr->entityNum >= 0 && tr->entityNum < MAX_CLIENTS)
	{
		return qtrue;
	}

	return qfalse;
}

saberMoveName_t  PMove::PM_SaberLungeAttackMove( void )
{
	vec3_t fwdAngles, jumpFwd;

	VectorCopy( pm->ps->viewangles, fwdAngles );
	fwdAngles[PITCH] = fwdAngles[ROLL] = 0;
	//do the lunge
	AngleVectors( fwdAngles, jumpFwd, NULL, NULL );
	VectorScale( jumpFwd, 150, pm->ps->velocity );
	PM_AddEvent( EV_JUMP_GENERAL );

	return LS_A_LUNGE_GENERAL;
}

saberMoveName_t  PMove::PM_SaberJumpAttackMove( void )
{
	vec3_t fwdAngles, jumpFwd;

	VectorCopy( pm->ps->viewangles, fwdAngles );
	fwdAngles[PITCH] = fwdAngles[ROLL] = 0;
	AngleVectors( fwdAngles, jumpFwd, NULL, NULL );

	// TODO MAYBE jaPRO thingie

	VectorScale( jumpFwd, 300, pm->ps->velocity );
	pm->ps->velocity[2] = 280;
	PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height

	PM_AddEvent( EV_JUMP_GENERAL );
#if !CLIENTSIDEONLY
	pm->ps->fd.forceJumpSound = 1;
#endif
	pm->cmd.upmove = 0;

	return LS_A_JUMP_T__B__GENERAL;
}

float  PMove::PM_GroundDistance(void)
{
	trace_t tr;
	vec3_t down;

	VectorCopy(pm->ps->origin, down);

	down[2] -= 4096;

	PM_Trace(&tr, pm->ps->origin, pm->mins, pm->maxs, down, pm->ps->clientNum, MASK_SOLID);

	VectorSubtract(pm->ps->origin, tr.endpos, down);

	return  VectorLength(down);
}

#define SABER_ALT_ATTACK_POWER		50//75?
#define SABER_ALT_ATTACK_POWER_LR	10//30?
#define SABER_ALT_ATTACK_POWER_FB	25//30/50?


saberMoveName_t  PMove::PM_SaberAttackForMovement(saberMoveName_t curmove)
{
	saberMoveName_t newmove = LS_INVALID_GENERAL;

	if ( pm->cmd.rightmove > 0 )
	{//moving right
		if (//!noSpecials
			(pm->modParms.runFlags & RFL_CLIMBTECH)
			//&& overrideJumpRightAttackMove != LS_NONE_GENERAL
			&& pm->ps->velocity[2] > 20.0f //pm->ps->groundEntityNum != ENTITYNUM_NONE//on ground
			&& (pm->cmd.buttons & BUTTON_ATTACK)//hitting attack
			&& PM_GroundDistance() < 70.0f //not too high above ground
			&& (pm->cmd.upmove > 0 || (pm->ps->pm_flags & PMF_JUMP_HELD))//focus-holding player
			&& BG_EnoughForcePowerForMove(SABER_ALT_ATTACK_POWER_LR)//have enough power
			)
		{//cartwheel right
			//if (allowCartwheels || (pm->ps->fd.saberAnimLevel == SS_STAFF)) { //dunno why do this if they cant cart..?
				BG_ForcePowerDrain(pm->ps, FP_GRIP, SABER_ALT_ATTACK_POWER_LR);
			//}
			//if (overrideJumpRightAttackMove != LS_INVALID_GENERAL)
			//{//overridden with another move
			//	return overrideJumpRightAttackMove;
			//}
			//else 
			//if (allowCartwheels || (pm->ps->fd.saberAnimLevel == SS_STAFF))
			{
				vec3_t right, fwdAngles;

				VectorSet(fwdAngles, 0.0f, pm->ps->viewangles[YAW], 0.0f);

				AngleVectors(fwdAngles, NULL, right, NULL);
				pm->ps->velocity[0] = pm->ps->velocity[1] = 0.0f;
				VectorMA(pm->ps->velocity, 190.0f, right, pm->ps->velocity);
				//if (pm->ps->fd.saberAnimLevel == SS_STAFF)
				//{
				//	newmove = LS_BUTTERFLY_RIGHT_GENERAL;
				//	pm->ps->velocity[2] = 350.0f;
				//}
				//else 
				//if (allowCartwheels)
				{
					//PM_SetJumped( JUMP_VELOCITY, qtrue );
					PM_AddEvent(EV_JUMP_GENERAL);
					pm->ps->velocity[2] = 300.0f;

					//if ( !Q_irand( 0, 1 ) )
					//if (PM_GroundDistance() >= 25.0f)
					if (1)
					{
						newmove = LS_JUMPATTACK_ARIAL_RIGHT_GENERAL;
					}
					//else
					//{
					//	newmove = LS_JUMPATTACK_CART_RIGHT_GENERAL;
					//}
				}
			}
		} 
		else if ( pm->cmd.forwardmove > 0 )
		{//forward right = TL2BR slash
			newmove = LS_A_TL2BR_GENERAL;
		}
		else if ( pm->cmd.forwardmove < 0 )
		{//backward right = BL2TR uppercut
			newmove = LS_A_BL2TR_GENERAL;
		}
		else
		{//just right is a left slice
			newmove = LS_A_L2R_GENERAL;
		}
	}
	else if ( pm->cmd.rightmove < 0 )
	{//moving left
		if (//!noSpecials
			(pm->modParms.runFlags & RFL_CLIMBTECH)
			//&& overrideJumpLeftAttackMove != LS_NONE_GENERAL
			&& pm->ps->velocity[2] > 20.0f //pm->ps->groundEntityNum != ENTITYNUM_NONE//on ground
			&& (pm->cmd.buttons & BUTTON_ATTACK)//hitting attack
			&& PM_GroundDistance() < 70.0f //not too high above ground
			&& (pm->cmd.upmove > 0 || (pm->ps->pm_flags & PMF_JUMP_HELD))//focus-holding player
			&& BG_EnoughForcePowerForMove(SABER_ALT_ATTACK_POWER_LR)//have enough power
			)
		{//cartwheel left
			//if (allowCartwheels || (pm->ps->fd.saberAnimLevel == SS_STAFF)) { //dunno why do this if they cant cart..?
				BG_ForcePowerDrain(pm->ps, FP_GRIP, SABER_ALT_ATTACK_POWER_LR);
			//}

			//if (overrideJumpLeftAttackMove != LS_INVALID_GENERAL)
			//{//overridden with another move
			//	return overrideJumpLeftAttackMove;
			//}
			//else 
			//if (allowCartwheels || (pm->ps->fd.saberAnimLevel == SS_STAFF))
			{
				vec3_t right, fwdAngles;

				VectorSet(fwdAngles, 0.0f, pm->ps->viewangles[YAW], 0.0f);
				AngleVectors(fwdAngles, NULL, right, NULL);
				pm->ps->velocity[0] = pm->ps->velocity[1] = 0.0f;
				VectorMA(pm->ps->velocity, -190.0f, right, pm->ps->velocity);
				//if (pm->ps->fd.saberAnimLevel == SS_STAFF)
				//{
				//	newmove = LS_BUTTERFLY_LEFT_GENERAL;
				//	pm->ps->velocity[2] = 250.0f;
				//}
				//else 
				//if (allowCartwheels)
				{
					//PM_SetJumped( JUMP_VELOCITY, qtrue );
					PM_AddEvent(EV_JUMP_GENERAL);
					pm->ps->velocity[2] = 350.0f;

					//if ( !Q_irand( 0, 1 ) )
					//if (PM_GroundDistance() >= 25.0f)
					if (1)
					{
						newmove = LS_JUMPATTACK_ARIAL_LEFT_GENERAL;
					}
					//else
					//{
					//	newmove = LS_JUMPATTACK_CART_LEFT_GENERAL;
					//}
				}
			}
		}
		else if ( pm->cmd.forwardmove > 0 )
		{//forward left = TR2BL slash
			newmove = LS_A_TR2BL_GENERAL;
		}
		else if ( pm->cmd.forwardmove < 0 )
		{//backward left = BR2TL uppercut
			newmove = LS_A_BR2TL_GENERAL;
		}
		else
		{//just left is a right slice
			newmove = LS_A_R2L_GENERAL;
		}
	}
	else
	{//not moving left or right
		if ( pm->cmd.forwardmove > 0 )
		{//forward= T2B slash
			if (pm->ps->fd.saberAnimLevel == FORCE_LEVEL_2 &&
				pm->ps->velocity[2] > 100 &&
				PM_GroundDistance() < 32 &&
				!BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) &&
				(!BG_SaberInSpecialAttack(pm->ps->torsoAnim) || jk2gameplay != VERSION_1_04))
			{ //FLIP AND DOWNWARD ATTACK
				trace_t tr;

				if (PM_SomeoneInFront(&tr))
				{
					newmove = PM_SaberFlipOverAttackMove(&tr);
				}
			}
			else if (
				(pm->modParms.runFlags & RFL_CLIMBTECH) &&
				pm->ps->fd.saberAnimLevel == FORCE_LEVEL_3 &&
				pm->ps->velocity[2] > 100 &&
				PM_GroundDistance() < 32 &&
				!BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) &&
				!BG_SaberInSpecialAttack(pm->ps->torsoAnim) 
				&& BG_EnoughForcePowerForMove(SABER_ALT_ATTACK_POWER_FB)
				)
			{ //DFA (JKA)
				{
					newmove = PM_SaberJumpAttackMove();
					if (newmove != LS_A_T2B_GENERAL
						&& newmove != LS_NONE_GENERAL)
					{
						BG_ForcePowerDrain(pm->ps, FP_GRIP, SABER_ALT_ATTACK_POWER_FB);
					}
				}
			}
			else if (pm->ps->fd.saberAnimLevel == FORCE_LEVEL_1 &&
				(pm->ps->groundEntityNum != ENTITYNUM_NONE || jk2gameplay != VERSION_1_04) &&
				(pm->ps->pm_flags & PMF_DUCKED) &&
				pm->ps->weaponTime <= 0 &&
				(!BG_SaberInSpecialAttack(pm->ps->torsoAnim) || jk2gameplay != VERSION_1_04))
			{ //LUNGE (weak)
				newmove = PM_SaberLungeAttackMove();
			}
			else
			{
				newmove = LS_A_T2B_GENERAL;
			}
		}
		else if ( pm->cmd.forwardmove < 0 )
		{//backward= T2B slash//B2T uppercut?
			if (PM_CanBackstab() && (!BG_SaberInSpecialAttack(pm->ps->torsoAnim) || jk2gameplay != VERSION_1_04))
			{ //BACKSTAB (attack varies by level)
				if (pm->ps->fd.saberAnimLevel >= FORCE_LEVEL_2)
				{//medium and higher attacks
					if ( (pm->ps->pm_flags&PMF_DUCKED) || pm->cmd.upmove < 0 )
					{
						newmove = LS_A_BACK_CR_GENERAL;
					}
					else
					{
						newmove = LS_A_BACK_GENERAL;
					}
				}
				else
				{ //weak attack
					newmove = LS_A_BACKSTAB_GENERAL;
				}
			}
			else
			{
				newmove = LS_A_T2B_GENERAL;
			}
		}
		else if ( PM_SaberInBounce( curmove ) )
		{//bounces should go to their default attack if you don't specify a direction but are attacking
			if ( jk2gameplay != VERSION_1_02 ) newmove = saberMoveData_general[curmove].chain_attack;

			if ( (jk2gameplay != VERSION_1_02 && PM_SaberKataDone(curmove, newmove)) || (jk2gameplay == VERSION_1_02 && PM_SaberKataDone_1_02()) )
			{
				newmove = saberMoveData_general[curmove].chain_idle;
			}
			else
			{
				newmove = saberMoveData_general[curmove].chain_attack;
			}
		}
		else if ( curmove == LS_READY_GENERAL )
		{//Not moving at all, shouldn't have gotten here...?
			//for now, just pick a random attack
			//newmove = Q_irand( LS_A_TL2BR_GENERAL, LS_A_T2B_GENERAL );
			//rww - If we don't seed with a "common" value, the client and server will get mismatched
			//prediction values. Under laggy conditions this will cause the appearance of rapid swing
			//sequence changes.
			
			if ( jk2gameplay == VERSION_1_02 ) newmove = (saberMoveName_t)PM_irand_timesync(LS_A_TL2BR_GENERAL, LS_A_T2B_GENERAL + pml.randomAdd, pm->modParms.raceMode, LS_A_TL2BR_GENERAL);
			else							   newmove = (saberMoveName_t)LS_A_T2B_GENERAL; //decided we don't like random attacks when idle, use an overhead instead.
		}
	}

	return newmove;
}
/*
=================
PM_WeaponLightsaber

Consults a chart to choose what to do with the lightsaber.
While this is a little different than the Quake 3 code, there is no clean way of using the Q3 code for this kind of thing.
=================
*/
// Ultimate goal is to set the sabermove to the proper next location
// Note that if the resultant animation is NONE, then the animation is essentially "idle", and is set in WP_TorsoAnim_GENERAL
void  PMove::PM_WeaponLightsaber(void)
{
	int			addTime;
	qboolean	delayed_fire = qfalse;
	int			anim=-1, curmove, newmove=LS_NONE_GENERAL;

	qboolean checkOnlyWeap = qfalse;

 	if ( pm->ps->saberLockTime > pm->cmd.serverTime )
	{
		pm->ps->saberMove = LS_NONE_GENERAL;
		PM_SaberLocked();
		return;
	}
	else
	{
		if ( ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF2LOCK_GENERAL ||
				(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF1LOCK_GENERAL ||
				(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CWCIRCLELOCK_GENERAL ||
				(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CCWCIRCLELOCK_GENERAL ||
				pm->ps->saberLockFrame )
			)
		{
			if (pm->ps->saberLockEnemy < ENTITYNUM_NONE &&
				pm->ps->saberLockEnemy >= 0)
			{
				playerState_t *en;

				en = pm->bgClients[pm->ps->saberLockEnemy];

				if (en)
				{
					PM_SaberLockBreak(en, qfalse);
					return;
				}
			}

			if ( ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF2LOCK_GENERAL ||
					(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_BF1LOCK_GENERAL ||
					(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CWCIRCLELOCK_GENERAL ||
					(pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == BOTH_CCWCIRCLELOCK_GENERAL ||
					pm->ps->saberLockFrame )
				)
			{
				pm->ps->torsoTimer = 0;
				PM_SetAnim(SETANIM_TORSO,BOTH_STAND1_GENERAL,SETANIM_FLAG_OVERRIDE, 100);
				pm->ps->saberLockFrame = 0;
			}
		}
	}

	if (pm->ps->saberHolstered)
	{
		if (pm->ps->saberMove != LS_READY_GENERAL)
		{
			PM_SetSaberMove( LS_READY_GENERAL );
		}

		if ((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) != (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT))
		{
			PM_SetAnim(SETANIM_TORSO,(pm->ps->legsAnim & ~ANIM_TOGGLEBIT),SETANIM_FLAG_OVERRIDE, 100);
		}

		if (BG_InSaberStandAnim(pm->ps->torsoAnim))
		{
			PM_SetAnim(SETANIM_TORSO,BOTH_STAND1_GENERAL,SETANIM_FLAG_OVERRIDE, 100);
		}

		if (pm->ps->weaponTime < 1 && ((pm->cmd.buttons & BUTTON_ALT_ATTACK) || (pm->cmd.buttons & BUTTON_ATTACK)))
		{
			if (pm->ps->duelTime < pm->cmd.serverTime || pm->modParms.raceMode)// && !pm->modParms.raceMode) // is this the correct way to account for racemode?
			{
				pm->ps->saberHolstered = qfalse;
				PM_AddEvent(EV_SABER_UNHOLSTER_GENERAL);
			}
		}

		if ( pm->ps->weaponTime > 0 )
		{
			pm->ps->weaponTime -= pml.msec;
		}

		checkOnlyWeap = qtrue;
		goto weapChecks;
	}

	if ((pm->cmd.buttons & BUTTON_ALT_ATTACK) &&
		pm->ps->weaponTime < 1 &&
		pm->ps->saberCanThrow &&
		pm->ps->fd.forcePower >= forcePowerNeeded[NONETWORK_FORCEPOWERLEVEL(pm->ps,FP_SABERTHROW)][FP_SABERTHROW] &&
		!BG_HasYsalamiri(pm->gametype, pm->ps) &&
		BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_SABERTHROW) &&
		!pm->modParms.raceMode // don't throw sabers in racemode, its unpredictable what they will do, and they will affect our force power
		)
	{ //might as well just check for a saber throw right here
		//This will get set to false again once the saber makes it back to its owner game-side
		if (!pm->ps->saberInFlight)
		{
			pm->ps->fd.forcePower -= forcePowerNeeded[NONETWORK_FORCEPOWERLEVEL(pm->ps, FP_SABERTHROW)][FP_SABERTHROW];
		}

		pm->ps->saberInFlight = qtrue;
	}
	
	if ( pm->ps->saberInFlight )
	{//guiding saber
		PM_SetAnim(SETANIM_TORSO, BOTH_SABERPULL_GENERAL, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
		pm->ps->torsoTimer = 1;
		return;
	}

   // don't allow attack until all buttons are up
	//This is bad. It freezes the attack state and the animations if you hold the button after respawning, and it looks strange.
	if ( pm->ps->pm_flags & PMF_RESPAWNED && jk2gameplay == VERSION_1_02 ) {
		return;
	}

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	if (pm->ps->weaponstate == WEAPON_READY ||
		pm->ps->weaponstate == WEAPON_IDLE)
	{
		if (pm->ps->saberMove != LS_READY_GENERAL && pm->ps->weaponTime <= 0 && !pm->ps->saberBlocked)
		{
			PM_SetSaberMove( LS_READY_GENERAL );
		}
	}

	if( (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) == BOTH_RUN2_GENERAL ||
		(pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) == BOTH_RUN1_GENERAL )
	{
		if ((pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) != (pm->ps->legsAnim & ~ANIM_TOGGLEBIT))
		{
			PM_SetAnim(SETANIM_TORSO,(pm->ps->legsAnim & ~ANIM_TOGGLEBIT),SETANIM_FLAG_OVERRIDE, 100);
		}
	}

	// make weapon function
	if ( pm->ps->weaponTime > 0 ) {
		pm->ps->weaponTime -= pml.msec;

		if (pm->ps->saberBlocked && pm->ps->torsoAnim != saberMoveData_general[pm->ps->saberMove].animToUseGeneral)
		{ //rww - keep him in the blocking pose until he can attack again
			PM_SetAnim(SETANIM_TORSO,saberMoveData_general[pm->ps->saberMove].animToUseGeneral,saberMoveData_general[pm->ps->saberMove].animSetFlags|SETANIM_FLAG_HOLD, saberMoveData_general[pm->ps->saberMove].blendTime);
			return;
		}
	}
	else
	{
		pm->ps->weaponstate = WEAPON_READY;
	}

	// Now we react to a block action by the player's lightsaber.
	if ( pm->ps->saberBlocked )
	{
		int firstSet = 0;

		if (!pm->ps->weaponTime)
		{
			firstSet = 1;
		}

		switch ( pm->ps->saberBlocked )
		{
			case BLOCKED_BOUNCE_MOVE:
				if ( jk2gameplay != VERSION_1_02 )
				{ //act as a bounceMove and reset the saberMove instead of using a seperate value for it
					PM_SetSaberMove( pm->ps->saberMove );
					pm->ps->weaponTime = pm->ps->torsoTimer;
					pm->ps->saberBlocked = 0;
				}
				break;
			case BLOCKED_PARRY_BROKEN:
				//whatever parry we were is in now broken, play the appropriate knocked-away anim
				{
					int nextMove;

					if ( PM_SaberInBrokenParry( pm->ps->saberMove ) )
					{//already have one...?
						nextMove = pm->ps->saberMove;
					}
					else
					{
						nextMove = PM_BrokenParryForParry( pm->ps->saberMove );
					}
					if ( nextMove != LS_NONE_GENERAL )
					{
						PM_SetSaberMove( nextMove );
						pm->ps->weaponTime = pm->ps->torsoTimer;
					}
					else if ( jk2gameplay != VERSION_1_02 )
					{//Maybe in a knockaway?
						if (pm->ps->weaponTime <= 0)
						{
							pm->ps->saberBlocked = 0;
						}
					}
				}
				break;
			case BLOCKED_ATK_BOUNCE:
				// If there is absolutely no blocked move in the chart, don't even mess with the animation.
				// OR if we are already in a block or parry.
				if (pm->ps->saberMove >= LS_T1_BR__R_GENERAL)
				{//an actual bounce?  Other bounces before this are actually transitions?
					pm->ps->saberBlocked = BLOCKED_NONE;
				}
				else
				{
					int bounceMove;

					if ( pm->cmd.buttons & BUTTON_ATTACK )
					{//transition to a new attack
						int newQuad = PM_SaberMoveQuadrantForMovement( &pm->cmd );
						while ( newQuad == saberMoveData_general[pm->ps->saberMove].startQuad )
						{//player is still in same attack quad, don't repeat that attack because it looks bad, 
							//FIXME: try to pick one that might look cool?
							//newQuad = Q_irand( Q_BR, Q_BL );
							newQuad = PM_irand_timesync( Q_BR, Q_BL + pml.randomAdd, pm->modParms.raceMode, Q_BR); // idk what else to do meh
							//FIXME: sanity check, just in case?
						}//else player is switching up anyway, take the new attack dir
						bounceMove = transitionMove[saberMoveData_general[pm->ps->saberMove].startQuad][newQuad];
					}
					else
					{//return to ready
						if ( saberMoveData_general[pm->ps->saberMove].startQuad == Q_T )
						{
							bounceMove = LS_R_BL2TR_GENERAL;
						}
						else if ( saberMoveData_general[pm->ps->saberMove].startQuad < Q_T )
						{
							bounceMove = LS_R_TL2BR_GENERAL+saberMoveData_general[pm->ps->saberMove].startQuad-Q_BR;
						}
						else// if ( saberMoveData_general[pm->ps->saberMove].startQuad > Q_T )
						{
							bounceMove = LS_R_BR2TL_GENERAL+saberMoveData_general[pm->ps->saberMove].startQuad-Q_TL;
						}
					}
					PM_SetSaberMove( bounceMove );

					pm->ps->weaponTime = pm->ps->torsoTimer;//+saberMoveData_general[bounceMove].blendTime+SABER_BLOCK_DUR;

				}
				break;
			case BLOCKED_UPPER_RIGHT:
				PM_SetSaberMove( LS_PARRY_UR_GENERAL );
				break;
			case BLOCKED_UPPER_RIGHT_PROJ:
				PM_SetSaberMove( LS_REFLECT_UR_GENERAL );
				break;
			case BLOCKED_UPPER_LEFT:
				PM_SetSaberMove( LS_PARRY_UL_GENERAL );
				break;
			case BLOCKED_UPPER_LEFT_PROJ:
				PM_SetSaberMove( LS_REFLECT_UL_GENERAL );
				break;
			case BLOCKED_LOWER_RIGHT:
				PM_SetSaberMove( LS_PARRY_LR_GENERAL );
				break;
			case BLOCKED_LOWER_RIGHT_PROJ:
				PM_SetSaberMove( LS_REFLECT_LR_GENERAL );
				break;
			case BLOCKED_LOWER_LEFT:
				PM_SetSaberMove( LS_PARRY_LL_GENERAL );
				break;
			case BLOCKED_LOWER_LEFT_PROJ:
				PM_SetSaberMove( LS_REFLECT_LL_GENERAL);
				break;
			case BLOCKED_TOP:
				PM_SetSaberMove( LS_PARRY_UP_GENERAL );
				break;
			case BLOCKED_TOP_PROJ:
				PM_SetSaberMove( LS_REFLECT_UP_GENERAL );
				break;
			default:
				pm->ps->saberBlocked = BLOCKED_NONE;
				break;
		}

		if (pm->ps->saberBlocked != BLOCKED_ATK_BOUNCE && pm->ps->saberBlocked != BLOCKED_PARRY_BROKEN && pm->ps->weaponTime < 1)
		{
			pm->ps->torsoTimer = SABER_BLOCK_DUR;
			pm->ps->weaponTime = pm->ps->torsoTimer;
		}

		if (firstSet)
		{
			return;
		}

		// Charging is like a lead-up before attacking again.  This is an appropriate use, or we can create a new weaponstate for blocking
		pm->ps->weaponstate = WEAPON_READY;

		// Done with block, so stop these active weapon branches.
		return;
	}

weapChecks:
	// check for weapon change
	// can't change if weapon is firing, but can change again if lowering or raising
	if ( pm->ps->weaponTime <= 0 || pm->ps->weaponstate != WEAPON_FIRING ) {
		if ( pm->ps->weapon != pm->cmd.weapon ) {
			PM_BeginWeaponChange( pm->cmd.weapon );
		}
	}

	if ( pm->ps->weaponTime > 0 ) 
	{
		return;
	}

	// *********************************************************
	// WEAPON_DROPPING
	// *********************************************************

	// change weapon if time
	if ( pm->ps->weaponstate == WEAPON_DROPPING ) {
		PM_FinishWeaponChange();
		return;
	}

	// *********************************************************
	// WEAPON_RAISING
	// *********************************************************

	if ( pm->ps->weaponstate == WEAPON_RAISING ) 
	{//Just selected the weapon
		pm->ps->weaponstate = WEAPON_IDLE;
		if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_WALK1_GENERAL )
		{
			PM_SetAnim(SETANIM_TORSO,BOTH_WALK1_GENERAL,SETANIM_FLAG_NORMAL, 100);
		}
		else if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_RUN2_GENERAL )
		{
			PM_SetAnim(SETANIM_TORSO,BOTH_RUN2_GENERAL,SETANIM_FLAG_NORMAL, 100);
		}
		else if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_WALK2_GENERAL )
		{
			PM_SetAnim(SETANIM_TORSO,BOTH_WALK2_GENERAL,SETANIM_FLAG_NORMAL, 100);
		}
		else
		{
			PM_SetAnim(SETANIM_TORSO,PM_GetSaberStance(),SETANIM_FLAG_NORMAL, 100);
		}

		if (pm->ps->weaponstate == WEAPON_RAISING)
		{
			return;
		}

	}

	if (checkOnlyWeap)
	{
		return;
	}

	// *********************************************************
	// Check for WEAPON ATTACK
	// *********************************************************

	if(!delayed_fire)
	{
		// Start with the current move, and cross index it with the current control states.
		if ( pm->ps->saberMove > LS_NONE_GENERAL && pm->ps->saberMove < LS_MOVE_MAX_GENERAL )
		{
			curmove = pm->ps->saberMove;
		}
		else
		{
			curmove = LS_READY_GENERAL;
		}
		// check for fire
		if ( !(pm->cmd.buttons & (BUTTON_ATTACK)) )
		{
			if (pm->ps->weaponTime != 0)
			{//Still firing
				pm->ps->weaponstate = WEAPON_FIRING;
			}
			else if ( pm->ps->weaponstate != WEAPON_READY )
			{
				pm->ps->weaponstate = WEAPON_IDLE;
			}
			//Check for finishing an anim if necc.
			if ( curmove >= LS_S_TL2BR_GENERAL && curmove <= LS_S_T2B_GENERAL )
			{//started a swing, must continue from here
				newmove = LS_A_TL2BR_GENERAL + (curmove-LS_S_TL2BR_GENERAL);
			}
			else if ( curmove >= LS_A_TL2BR_GENERAL && curmove <= LS_A_T2B_GENERAL )
			{//finished an attack, must continue from here
				newmove = LS_R_TL2BR_GENERAL + (curmove-LS_A_TL2BR_GENERAL);
			}
			else if ( PM_SaberInTransition( curmove ) )
			{//in a transition, must play sequential attack
				newmove = saberMoveData_general[curmove].chain_attack;
			}
			else if ( PM_SaberInBounce( curmove ) )
			{//in a bounce
				newmove = saberMoveData_general[curmove].chain_idle;//oops, not attacking, so don't chain
			}
			else
			{//FIXME: what about returning from a parry?
				PM_SetSaberMove( LS_READY_GENERAL );
				return;
			}
		}

		// ***************************************************
		// Pressing attack, so we must look up the proper attack move.

		if ( pm->ps->weaponTime > 0 )
		{	// Last attack is not yet complete.
			pm->ps->weaponstate = WEAPON_FIRING;
			return;
		}
		else
		{
			int	both = qfalse;

			if ( curmove >= LS_PARRY_UP_GENERAL && curmove <= LS_REFLECT_LL_GENERAL )
			{//from a parry or deflection, can go directly into an attack (?)
				switch ( saberMoveData_general[curmove].endQuad )
				{
				case Q_T:
					newmove = LS_A_T2B_GENERAL;
					break;
				case Q_TR:
					newmove = LS_A_TL2BR_GENERAL;
					break;
				case Q_TL:
					newmove = LS_A_TR2BL_GENERAL;
					break;
				case Q_BR:
					newmove = LS_A_BR2TL_GENERAL;
					break;
				case Q_BL:
					newmove = LS_A_BL2TR_GENERAL;
					break;
				//shouldn't be a parry that ends at L, R or B
				}
			}

			if ( newmove != LS_NONE_GENERAL )
			{//have a valid, final LS_ move picked, so skip findingt he transition move and just get the anim
				anim = saberMoveData_general[newmove].animToUseGeneral;
			}

			//FIXME: diagonal dirs use the figure-eight attacks from ready pose?
			if ( anim == -1 )
			{
				//FIXME: take FP_SABER_OFFENSE into account here somehow?
				if ( PM_SaberInTransition( curmove ) )
				{//in a transition, must play sequential attack
					newmove = saberMoveData_general[curmove].chain_attack;
				}
				else if ( curmove >= LS_S_TL2BR_GENERAL && curmove <= LS_S_T2B_GENERAL )
				{//started a swing, must continue from here
					newmove = LS_A_TL2BR_GENERAL + (curmove-LS_S_TL2BR_GENERAL);
				}
				else if ( PM_SaberInBrokenParry( curmove ) && jk2gameplay != VERSION_1_02 )
				{//broken parries must always return to ready
					newmove = LS_READY_GENERAL;
				}
				else//if ( pm->cmd.buttons&BUTTON_ATTACK && !(pm->ps->pm_flags&PMF_ATTACK_HELD) )//only do this if just pressed attack button?
				{//get attack move from movement command
					if ( jk2gameplay != VERSION_1_02 )
					{
						saberMoveName_t checkMove = PM_SaberAttackForMovement((saberMoveName_t)curmove);
						if (checkMove != LS_INVALID_GENERAL)
						{
							newmove = checkMove;
						}

						if ( (PM_SaberInBounce( curmove )||PM_SaberInBrokenParry( curmove ))
							&& saberMoveData_general[newmove].startQuad == saberMoveData_general[curmove].endQuad )
						{//this attack would be a repeat of the last (which was blocked), so don't actually use it, use the default chain attack for this bounce
							newmove = saberMoveData_general[curmove].chain_attack;
						}

						if ( PM_SaberKataDone(curmove, newmove) )
						{//we came from a bounce and cannot chain to another attack because our kata is done
							newmove = saberMoveData_general[curmove].chain_idle;
						}
					}
					else
					{
						if (pm->modParms.runFlags & RFL_CLIMBTECH) {
							// flip the order so we can trigger the jka dfa
 							saberMoveName_t checkMove = PM_SaberAttackForMovement((saberMoveName_t)curmove);
							if (checkMove != LS_INVALID_GENERAL)
							{
								newmove = checkMove;
							}
							if (PM_SaberKataDone_1_02())
							{//we came from a bounce and cannot chain to another attack because our kata is done
								newmove = saberMoveData_general[curmove].chain_idle;
							}
						}
						else {
							if (PM_SaberKataDone_1_02())
							{//we came from a bounce and cannot chain to another attack because our kata is done
								newmove = saberMoveData_general[curmove].chain_idle;
							}
							else
							{
								saberMoveName_t checkMove = PM_SaberAttackForMovement((saberMoveName_t)curmove);
								if (checkMove != LS_INVALID_GENERAL)
								{
									newmove = checkMove;
								}
							}
						}
					}
				}

				if ( newmove != LS_NONE_GENERAL )
				{
					//Now get the proper transition move
					newmove = PM_SaberAnimTransitionAnim( curmove, newmove );

					assert(	bgGlobalAnimations[saberMoveData_general[newmove].animToUseGeneral].firstFrame != 0 || 
							bgGlobalAnimations[saberMoveData_general[newmove].animToUseGeneral].numFrames != 0);

					anim = saberMoveData_general[newmove].animToUseGeneral;
				}
			}

			if (anim == -1)
			{//not side-stepping, pick neutral anim
				// Add randomness for prototype?
				newmove = saberMoveData_general[curmove].chain_attack;

				anim= saberMoveData_general[newmove].animToUseGeneral;

				if ( !pm->cmd.forwardmove && !pm->cmd.rightmove && pm->cmd.upmove >= 0 && pm->ps->groundEntityNum != ENTITYNUM_NONE )
				{//not moving at all, so set the anim on entire body
					both = qtrue;
				}
			
			}

			if ( anim == -1)
			{
				if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_WALK1_GENERAL )
				{
					anim = BOTH_WALK1_GENERAL;
				}
				else if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_RUN2_GENERAL )
				{
					anim = BOTH_RUN2_GENERAL;
				}
				else if((pm->ps->legsAnim & ~ANIM_TOGGLEBIT) == BOTH_WALK2_GENERAL )
				{
					anim = BOTH_WALK2_GENERAL;
				}
				else
				{
					anim = PM_GetSaberStance();
				}

				if (anim == BOTH_RUN2_GENERAL && !pm->cmd.forwardmove && !pm->cmd.rightmove && jk2gameplay != VERSION_1_02)
				{ //semi-hacky (if not moving on x-y and still playing the running anim, force the player out of it)
					anim = PM_GetSaberStance();
				}
				newmove = LS_READY_GENERAL;
			}

			if ( !pm->ps->saberActive )
			{//turn on the saber if it's not on
				pm->ps->saberActive = qtrue;
			}

			PM_SetSaberMove( newmove );

			if ( both )
			{
				PM_SetAnim(SETANIM_LEGS,anim,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
			}

			//don't fire again until anim is done
			pm->ps->weaponTime = pm->ps->torsoTimer;
		}
	}

	// *********************************************************
	// WEAPON_FIRING
	// *********************************************************

	pm->ps->weaponstate = WEAPON_FIRING;

	addTime = pm->ps->weaponTime;

	pm->ps->saberAttackSequence = pm->ps->torsoAnim;
	if ( !addTime )
	{
		addTime = weaponData[pm->ps->weapon].fireTime;
	}
	pm->ps->weaponTime = addTime;
}
/*
#ifdef JK2_CGAME
#include "../cgame/cg_local.h" //ahahahahhahahaha@$!$!
#endif*/

void  PMove::PM_SetSaberMove(short newMove)
{
	unsigned int setflags = saberMoveData_general[newMove].animSetFlags;
	int	anim = saberMoveData_general[newMove].animToUseGeneral;
	int parts = SETANIM_TORSO;
	
	if ( newMove == LS_READY_GENERAL || ((newMove == LS_A_FLIP_STAB_GENERAL || newMove == LS_A_FLIP_SLASH_GENERAL) &&
		jk2gameplay != VERSION_1_02) )
	{//finished with a kata (or in a special move) reset attack counter
		pm->ps->saberAttackChainCount = 0;
	}
	else if ( BG_SaberInAttack( newMove ) )
	{//continuing with a kata, increment attack counter
		pm->ps->saberAttackChainCount++;
	}

	if (pm->ps->saberAttackChainCount > 16)
	{ //for the sake of being able to send the value over the net within a reasonable bit count
		pm->ps->saberAttackChainCount = 16;
	}

	if ( jk2gameplay != VERSION_1_02 )
	{
		if ( pm->ps->fd.saberAnimLevel > FORCE_LEVEL_1 &&
			 !BG_SaberInIdle( newMove ) && !PM_SaberInParry( newMove ) && !PM_SaberInKnockaway( newMove ) && !PM_SaberInBrokenParry( newMove ) && !PM_SaberInReflect( newMove ) && !BG_SaberInSpecial(newMove))
		{//readies, parries and reflections have only 1 level 
			//increment the anim to the next level of saber anims
			anim += (pm->ps->fd.saberAnimLevel-FORCE_LEVEL_1) * SABER_ANIM_GROUP_SIZE;
		}
	}
	
	if ( jk2gameplay == VERSION_1_02 )
	{
		if ( pm->ps->fd.saberAnimLevel > FORCE_LEVEL_1 &&
				!BG_SaberInIdle( newMove ) && !PM_SaberInParry( newMove ) && !PM_SaberInReflect( newMove ) && !BG_SaberInSpecial(newMove))
		{//readies, parries and reflections have only 1 level 
			//increment the anim to the next level of saber anims
			if ( !PM_SaberInTransition( newMove ) )
			{//FIXME: only have level 1 transitions for now
				anim += (pm->ps->fd.saberAnimLevel-FORCE_LEVEL_1) * SABER_ANIM_GROUP_SIZE;
			}
		}
	}

	// If the move does the same animation as the last one, we need to force a restart...
	if ( saberMoveData_general[pm->ps->saberMove].animToUseGeneral == anim && newMove > LS_PUTAWAY_GENERAL)
	{
		setflags |= SETANIM_FLAG_RESTART;
	}

	//saber torso anims should always be highest priority (4/12/02 - for special anims only)
	if ( newMove == LS_A_LUNGE_GENERAL 
		|| newMove == LS_A_JUMP_T__B__GENERAL 
		|| newMove == LS_A_BACKSTAB_GENERAL
		|| newMove == LS_A_BACK_GENERAL
		|| newMove == LS_A_BACK_CR_GENERAL
		|| newMove == LS_A_FLIP_STAB_GENERAL
		|| newMove == LS_A_FLIP_SLASH_GENERAL
		|| newMove == LS_JUMPATTACK_ARIAL_LEFT_GENERAL // JKA cartwheel
		|| newMove == LS_JUMPATTACK_ARIAL_RIGHT_GENERAL // JKA cartwheel
		|| jk2gameplay == VERSION_1_02 )
	{
		setflags |= SETANIM_FLAG_OVERRIDE;
	}

	if ( BG_InSaberStandAnim(anim) || anim == BOTH_STAND1_GENERAL )
	{
		anim = (pm->ps->legsAnim & ~ANIM_TOGGLEBIT);

		if ((anim >= BOTH_STAND1_GENERAL && anim <= BOTH_STAND4TOATTACK2_GENERAL) ||
			(anim >= TORSO_DROPWEAP1_GENERAL && anim <= TORSO_WEAPONIDLE12_GENERAL))
		{ //If standing then use the special saber stand anim
			anim = PM_GetSaberStance();
		}

		if (pm->ps->pm_flags & PMF_DUCKED)
		{ //Playing torso walk anims while crouched makes you look like a monkey
			anim = PM_GetSaberStance();
		}

		if (anim == BOTH_WALKBACK1_GENERAL || anim == BOTH_WALKBACK2_GENERAL)
		{ //normal stance when walking backward so saber doesn't look like it's cutting through leg
			anim = PM_GetSaberStance();
		}

		parts = SETANIM_TORSO;
	}

	if (newMove == LS_JUMPATTACK_ARIAL_RIGHT_GENERAL || // jka cartwheel
		newMove == LS_JUMPATTACK_ARIAL_LEFT_GENERAL)// jka cartwheel
	{ //force only on legs
		parts = SETANIM_LEGS;
	}
	else if ( newMove == LS_A_LUNGE_GENERAL
		|| newMove == LS_A_JUMP_T__B__GENERAL 
		|| newMove == LS_A_BACKSTAB_GENERAL
		|| newMove == LS_A_BACK_GENERAL
		|| newMove == LS_A_BACK_CR_GENERAL
		|| newMove == LS_A_FLIP_STAB_GENERAL
		|| newMove == LS_A_FLIP_SLASH_GENERAL 
		|| newMove == LS_JUMPATTACK_ARIAL_LEFT_GENERAL// jka cartwheel
		|| newMove == LS_JUMPATTACK_ARIAL_RIGHT_GENERAL)// jka cartwheel
	{
		parts = SETANIM_BOTH;
	}
	else if ( BG_SpinningSaberAnim( anim ) )
	{//spins must be played on entire body
		parts = SETANIM_BOTH;
	}
	else if ( (!pm->cmd.forwardmove&&!pm->cmd.rightmove&&!pm->cmd.upmove) && jk2gameplay != VERSION_1_02 )
	{//not trying to run, duck or jump
		if ( !BG_FlippingAnim( pm->ps->legsAnim ) && 
			!BG_InRoll( pm->ps, pm->ps->legsAnim ) && 
			!PM_InKnockDown( pm->ps ) && 
			!PM_JumpingAnim( pm->ps->legsAnim ) &&
			!BG_InSpecialJump( pm->ps->legsAnim, pm->modParms.runFlags) &&
			anim != PM_GetSaberStance() &&
			pm->ps->groundEntityNum != ENTITYNUM_NONE &&
			!(pm->ps->pm_flags & PMF_DUCKED))
		{
			parts = SETANIM_BOTH;
		}
	}

#ifdef JK2_CGAME
	PM_SetAnim(parts, anim, ((jk2gameplay == VERSION_1_02 && !cg_fixlean.integer) ? (setflags | SETANIM_FLAG_HOLD) : setflags), saberMoveData_general[newMove].blendTime);
#else
	PM_SetAnim(parts, anim, (jk2gameplay == VERSION_1_02 ? (setflags|SETANIM_FLAG_HOLD) : setflags), saberMoveData_general[newMove].blendTime);
#endif

	if (parts != SETANIM_LEGS && 
		(pm->ps->legsAnim == BOTH_ARIAL_LEFT_GENERAL ||
			pm->ps->legsAnim == BOTH_ARIAL_RIGHT_GENERAL)) // can this even ever be true?!
	{
		if (pm->ps->legsTimer > pm->ps->torsoTimer)
		{
			pm->ps->legsTimer = pm->ps->torsoTimer;
		}
	}

	if ( (pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == anim )
	{//successfully changed anims
	//special check for *starting* a saber swing
		//playing at attack
		if ( BG_SaberInAttack( newMove ) || BG_SaberInSpecialAttack( anim ) )
		{
			if ( pm->ps->saberMove != newMove )
			{//wasn't playing that attack before
				PM_AddEvent(EV_SABER_ATTACK_GENERAL);
			}
		}

		// just testing this to see if it makes cartwheel climbs easier. eh idk.
		//if ((runFlags & RFL_CLIMBTECH) && BG_SaberInSpecial(newMove) &&
		//	pm->ps->weaponTime < pm->ps->torsoTimer)
		//{ //rww 01-02-03 - I think this will solve the issue of special attacks being interruptable, hopefully without side effects
		//	pm->ps->weaponTime = pm->ps->torsoTimer;
		//}

		pm->ps->saberMove = newMove;
#if !CLIENTSIDEONLY
		pm->ps->saberBlocking = saberMoveData_general[newMove].blocking;
#endif

		pm->ps->torsoAnim = anim;

		if (pm->ps->weaponTime <= 0)
		{
			pm->ps->saberBlocked = BLOCKED_NONE;
		}
	}
}

#pragma endregion "bg_saber"








// bg_slidemove.c

#pragma region "bg_slidemove"







/*

input: origin, velocity, bounds, groundPlane, trace function

output: origin, velocity, impacts, stairup boolean

*/

vec3_t flatNormal = {0,0,1};

qboolean  PMove::PM_GroundSlideOkay(float zNormal)
{
	int legsAnim = pm->ps->legsAnim & ~ANIM_TOGGLEBIT;
	// nvm, already guarded in all calls
	//if (!(pml.mod.runFlags & RFL_CLIMBTECH)) return qtrue;

	if (zNormal > 0)
	{
		if (pm->ps->velocity[2] > 0)
		{
			if (legsAnim == BOTH_WALL_RUN_RIGHT_GENERAL
				|| legsAnim == BOTH_WALL_RUN_LEFT_GENERAL
				|| legsAnim == BOTH_WALL_RUN_RIGHT_STOP_GENERAL
				|| legsAnim == BOTH_WALL_RUN_LEFT_STOP_GENERAL
				|| legsAnim == BOTH_FORCEWALLRUNFLIP_START_GENERAL
				//|| pm->ps->legsAnim == BOTH_FORCELONGLEAP_START_GENERAL
				//|| pm->ps->legsAnim == BOTH_FORCELONGLEAP_ATTACK_GENERAL
				//|| pm->ps->legsAnim == BOTH_FORCELONGLEAP_LAND_GENERAL
				|| BG_InReboundJump(legsAnim))
			{
				return qfalse;
			}
		}
	}
	return qtrue;
}



/*
==================
PM_LimitedClipVelocity

Slide off of the impacting surface

Limit maximum velocity while keeping original direction components
==================
*/
void  PMove::PM_LimitedClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce, float maxSpeed) {
	float	backoff;
	float	change;
	int		i;
	vec3_t	normalComponent;
	vec3_t	nonNormalComponent;
	float	maxLenOut;
	float	lenOut;
	float	lenNonNormal;

	if ((pm->modParms.runFlags & RFL_CLIMBTECH) && (pm->ps->pm_flags & PMF_STUCK_TO_WALL))
	{//no sliding!
		VectorCopy(in, out);
		return;
	}

	backoff = DotProduct(in, normal);

	VectorScale(normal, backoff, nonNormalComponent); // just reusing the var to not waste memory
	VectorSubtract(in, nonNormalComponent, nonNormalComponent); // nonNormalComponent is what MUST be preserved even if we limit max velocity.
	VectorSubtract(in, nonNormalComponent, normalComponent); // non
	lenNonNormal = VectorLength(nonNormalComponent);

	if (backoff < 0) {
		backoff *= overbounce;
	}
	else {
		backoff /= overbounce;
	}

	for (i = 0; i < 3; i++) {
		change = normal[i] * backoff;
		out[i] = normalComponent[i] - change;
	}

	// length(out*f+nonNormalComponent) < 100000
	// out and nonnormal are perpendicular to each other so
	// sqrt(lenOut*lenOut + lenNonNormal*lenNonNormal) = 100000
	// lenOut*lenOut = 100000^2 - lenNonNormal*lenNonNormal 
	// lenOut = sqrt(100000^2 - lenNonNormal*lenNonNormal)
	maxLenOut = sqrtf(maxSpeed* maxSpeed - lenNonNormal* lenNonNormal);
	if ((lenOut = VectorLength(out)) > maxLenOut) {
		VectorScale(out,maxLenOut/lenOut, out);
	}
	VectorAdd(out, nonNormalComponent, out);
}

/*
==================
PM_LimitedClipVelocity

Slide off of the impacting surface

Limit maximum velocity on the normal axis while keeping original direction components
==================
*/
void  PMove::PM_LimitedClipVelocity2(vec3_t in, vec3_t normal, vec3_t out, float overbounce, float maxSpeedNormal) {
	float	backoff;
	float	change;
	int		i;
	vec3_t	normalComponent;
	vec3_t	nonNormalComponent;
	//float	maxLenOut;
	float	lenOut;
	float	lenNonNormal;

	if ((pm->modParms.runFlags & RFL_CLIMBTECH) && (pm->ps->pm_flags & PMF_STUCK_TO_WALL))
	{//no sliding!
		VectorCopy(in, out);
		return;
	}

	backoff = DotProduct(in, normal);

	VectorScale(normal, backoff, nonNormalComponent); // just reusing the var to not waste memory
	VectorSubtract(in, nonNormalComponent, nonNormalComponent); // nonNormalComponent is what MUST be preserved even if we limit max velocity.
	VectorSubtract(in, nonNormalComponent, normalComponent); // non

	if (backoff < 0) {
		backoff *= overbounce;
	}
	else {
		backoff /= overbounce;
	}

	for (i = 0; i < 3; i++) {
		change = normal[i] * backoff;
		out[i] = normalComponent[i] - change;
	}

	if ((lenOut = VectorLength(out)) > maxSpeedNormal) {
		VectorScale(out, maxSpeedNormal /lenOut, out);
	}
	VectorAdd(out, nonNormalComponent, out);
}


/*
==================
PM_StepSlideMove

Each intersection will try to step over the obstruction instead of
sliding along it.

Returns a new origin, velocity, and contact entity
Does not modify any world state?
==================
*/
#define	MIN_STEP_NORMAL	0.7		// can't step up onto very steep slopes
#define	MAX_CLIP_PLANES	5
void  PMove::PM_Q2StepSlideMove_(void)
{
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		normal,planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity;
	int			i, j;
	trace_t	trace;
	vec3_t		end;
	float		time_left;
	float		tmp;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps,&pm->cmd);

	if (overbounce == OVERCLIP) {
		overbounce = 1.01f; // if we arent overriding aanything, we use the q2 standard instead (1.01 instead of 1.001)
	}

	numbumps = 4;

	VectorCopy(pm->ps->velocity, primal_velocity);
	numplanes = 0;

	time_left = pml.frametime;

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		for (i = 0; i < 3; i++)
			end[i] = pm->ps->origin[i] + time_left * pm->ps->velocity[i];

		PM_Trace(&trace,pm->ps->origin, pm->mins, pm->maxs, end, pm->ps->clientNum, pm->tracemask);

		if (trace.allsolid)
		{	// entity is trapped in another solid
			pm->ps->velocity[2] = 0;	// don't build up falling damage
			return;
		}

		if (trace.fraction > 0)
		{	// actually covered some distance
			VectorCopy(trace.endpos, pm->ps->origin);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			break;		// moved the entire distance

	   // save entity for contact
		if (pm->numtouch < MAXTOUCH && trace.entityNum != ENTITYNUM_WORLD)
		{
			pm->touchents[pm->numtouch] = trace.entityNum;
			pm->numtouch++;
		}

		time_left -= time_left * trace.fraction;

		// slide along this plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			VectorCopy(vec3_origin, pm->ps->velocity);
			break;
		}

		VectorCopy(trace.plane.normal,normal);

		if ((pm->modParms.runFlags & RFL_CLIMBTECH) && !PM_GroundSlideOkay(normal[2]))
		{//wall-running
			//never push up off a sloped wall
			normal[2] = 0;
			VectorNormalize(normal);
		}

		//
		// if this is the same plane we hit before, nudge velocity
		// out along it, which fixes some epsilon issues with
		// non-axial planes
		// 
		// TA: Copied this over from the normal jk function and it makes the movement smoother while keeping it overall nice. Nice!
		//
		if (!(pm->modParms.runFlags & RFL_CLIMBTECH) || !(pm->ps->pm_flags & PMF_STUCK_TO_WALL))
		{//no sliding if stuck to wall!
			for (i = 0; i < numplanes; i++) {
				if (DotProduct(normal, planes[i]) > 0.99f) {
					VectorAdd(normal, pm->ps->velocity, pm->ps->velocity);
					break;
				}
			}
			if (i < numplanes) {
				continue;
			}
		}

		VectorCopy(normal, planes[numplanes]);
		numplanes++;

#if 0
		float		rub;

		//
		// modify velocity so it parallels all of the clip planes
		//
		if (numplanes == 1)
		{	// go along this plane
			VectorCopy(pm->ps->velocity, dir);
			VectorNormalize(dir);
			rub = 1.0 + 0.5 * DotProduct(dir, planes[0]);

			// slide along the plane
			PM_ClipVelocity(pm->ps->velocity, planes[0], pm->ps->velocity, 1.01);
			// rub some extra speed off on xy axis
			// not on Z, or you can scrub down walls
			pm->ps->velocity[0] *= rub;
			pm->ps->velocity[1] *= rub;
			pm->ps->velocity[2] *= rub;
		}
		else if (numplanes == 2)
		{	// go along the crease
			VectorCopy(pm->ps->velocity, dir);
			VectorNormalize(dir);
			rub = 1.0 + 0.5 * DotProduct(dir, planes[0]);

			// slide along the plane
			CrossProduct(planes[0], planes[1], dir);
			d = DotProduct(dir, pm->ps->velocity);
			VectorScale(dir, d, pm->ps->velocity);

			// rub some extra speed off
			VectorScale(pm->ps->velocity, rub, pm->ps->velocity);
		}
		else
		{
			//			Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
			VectorCopy(vec3_origin, pm->ps->velocity);
			break;
		}

#else
		//
		// modify original_velocity so it parallels all of the clip planes
		//
		for (i = 0; i < numplanes; i++)
		{
			if (pm->modParms.physics == MV_PINBALL) {
				//PM_LimitedClipVelocity(pm->ps->velocity, planes[i], pm->ps->velocity, overbounce,100000.0f);
				overbounce -= planes[i][2]*0.6f* (std::min(1600.0f,fabsf(pm->ps->velocity[2]))/1600.0f); // dont let ground and ceiling bounce as as insanely much unless we have no proper speed to begin wtih.
				PM_LimitedClipVelocity2(pm->ps->velocity, planes[i], pm->ps->velocity, overbounce,10000.0f);
			}
			else {
				PM_ClipVelocity(pm->ps->velocity, planes[i], pm->ps->velocity, overbounce);
			}
			//if (moveStyle == MV_PINBALL && (tmp=VectorLength(pm->ps->velocity)) > 100000.0f) { // this is bad, it loses non-bounce-direction almost immediately.
			//	// limit it or we eventually get stuck in walls with velocity reaching billions
			//	VectorScale(pm->ps->velocity, 100000.0f/ tmp, pm->ps->velocity);
			//}
			if (planes[i][2] >= MIN_WALK_NORMAL) {
				pml.clipped = qtrue; // uh am i putting this the right place? idk
			}
			for (j = 0; j < numplanes; j++)
				if (j != i)
				{
					if (DotProduct(pm->ps->velocity, planes[j]) < 0)
						break;	// not ok
				}
			if (j == numplanes)
				break;
		}

		if (i != numplanes)
		{	// go along this plane
		}
		else
		{	// go along the crease
			if (numplanes != 2)
			{
				//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
				VectorCopy(vec3_origin, pm->ps->velocity);
				break;
			}
			CrossProduct(planes[0], planes[1], dir);
			d = DotProduct(dir, pm->ps->velocity);
			VectorScale(dir, d, pm->ps->velocity);
		}
#endif
		//
		// if velocity is against the original velocity, stop dead
		// to avoid tiny occilations in sloping corners
		//
		if (DotProduct(pm->ps->velocity, primal_velocity) <= 0 && pm->modParms.physics != MV_PINBALL)
		{
			VectorCopy(vec3_origin, pm->ps->velocity);
			break;
		}
	}

	if (pm->ps->pm_time)
	{
		VectorCopy(primal_velocity, pm->ps->velocity);
	}
}

/*
==================
PM_StepSlideMove

==================
*/
void  PMove::PM_Q2StepSlideMove(qboolean gravity)
{
	vec3_t		start_o, start_v;
	vec3_t		down_o, down_v;
	trace_t		trace;
	float		down_dist, up_dist;
	//	vec3_t		delta;
	vec3_t		up, down;

	if (gravity) {
		//if (pm->ps->gravity > 0)
		//	pml.velocity[2] = 0;
		//else
			pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	}

	VectorCopy(pm->ps->origin, start_o);
	VectorCopy(pm->ps->velocity, start_v);

	PM_Q2StepSlideMove_();

	VectorCopy(pm->ps->origin, down_o);
	VectorCopy(pm->ps->velocity, down_v);

	VectorCopy(start_o, up);
	up[2] += STEPSIZE;

	PM_Trace(&trace,up, pm->mins, pm->maxs, up, pm->ps->clientNum, pm->tracemask);
	if (trace.allsolid)
		return;		// can't step up

	// try sliding above
	VectorCopy(up, pm->ps->origin);
	VectorCopy(start_v, pm->ps->velocity);

	PM_Q2StepSlideMove_();

	// push down the final amount
	VectorCopy(pm->ps->origin, down);
	down[2] -= STEPSIZE;
	PM_Trace(&trace,pm->ps->origin, pm->mins, pm->maxs, down, pm->ps->clientNum, pm->tracemask);
	if (!trace.allsolid)
	{
		VectorCopy(trace.endpos, pm->ps->origin);
	}

#if 0
	VectorSubtract(pm->ps->origin, up, delta);
	up_dist = DotProduct(delta, start_v);

	VectorSubtract(down_o, start_o, delta);
	down_dist = DotProduct(delta, start_v);
#else
	VectorCopy(pm->ps->origin, up);

	// decide which one went farther
	down_dist = (down_o[0] - start_o[0]) * (down_o[0] - start_o[0])
		+ (down_o[1] - start_o[1]) * (down_o[1] - start_o[1]);
	up_dist = (up[0] - start_o[0]) * (up[0] - start_o[0])
		+ (up[1] - start_o[1]) * (up[1] - start_o[1]);
#endif

	if (down_dist > up_dist || trace.plane.normal[2] < MIN_STEP_NORMAL)
	{
		VectorCopy(down_o, pm->ps->origin);
		VectorCopy(down_v, pm->ps->velocity);
		return;
	}
	//!! Special case
	// if we were walking along a plane, then we need to copy the Z over
	pm->ps->velocity[2] = down_v[2];
}


void  PMove::PM_CheckBounceJump(vec3_t normal, vec3_t velocity) {

	int JUMP_VELOCITY_NEW = JUMP_VELOCITY;
	if (pm->modParms.physics != MV_BOUNCE || pm->cmd.upmove <= 0 || (pm->ps->pm_flags & PMF_JUMP_HELD) || normal[2] < MIN_WALK_NORMAL) {
		return;
	}

	if (pm->ps->usingATST)
	{
		return;
	}

	if (pm->ps->forceHandExtend == HANDEXTEND_KNOCKDOWN)
	{
		return;
	}

	//Don't allow jump until all buttons are up
	if (pm->ps->pm_flags & PMF_RESPAWNED) {
		return;
	}

	if (PM_InKnockDown(pm->ps) || BG_InRoll(pm->ps, pm->ps->legsAnim))
	{//in knockdown
		return;
	}
	if (MovementIsQuake3Based(pm->modParms.physics)) {
		JUMP_VELOCITY_NEW = 270;
	}
	
	//if (pm->ps->groundEntityNum != ENTITYNUM_NONE || pm->ps->origin[2] < pm->ps->fd.forceJumpZStart) // do this always ? calling this function already implies there was a ground bounce anyway
	//{
		pm->ps->fd.forcePowersActive &= ~(1 << FP_LEVITATION);
	//}

  	velocity[2] += JUMP_VELOCITY_NEW;
	if (velocity[2] < JUMP_VELOCITY_NEW)
		velocity[2] = JUMP_VELOCITY_NEW;
	//if (MovementIsQuake3Based(moveStyle)) {
	//	// TODO flood protect jumps? idk
	//	pm->ps->velocity[2] += JUMP_VELOCITY_NEW;
	//	if (pm->ps->velocity[2] < 270)
	//		pm->ps->velocity[2] = 270;
	//	pm->ps->stats[STAT_LASTJUMPSPEED] = pm->ps->velocity[2];
	//}
	//else {
	//	pm->ps->velocity[2] = JUMP_VELOCITY_NEW;
	//}
	PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
	pml.bounceJumped = qtrue;
	pm->ps->pm_flags |= PMF_JUMP_HELD;
	PM_SetGroundEntityNum(ENTITYNUM_NONE);
	PM_AddEvent(EV_JUMP_GENERAL);
	
	// make sure skims work
	pm->ps->pm_flags |= PMF_TIME_LAND;
	pm->ps->pm_time = 250;

	//Set the animations
	if (pm->ps->gravity > 0 && !BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags))
	{
		PM_JumpForDir();
	}
}


/*
==================
PM_SlideMove

Returns qtrue if the velocity was clipped in some way
==================
*/
#define	MAX_CLIP_PLANES	5
qboolean	 PMove::PM_SlideMove( qboolean gravity ) {
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		normal, planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity;
	vec3_t		clipVelocity;
	int			i, j, k;
	trace_t	trace;
	vec3_t		end;
	float		time_left;
	float		into;
	vec3_t		endVelocity;
	vec3_t		endClipVelocity;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);
	
	VectorClear( endVelocity );
	VectorClear( endClipVelocity );

	numbumps = 4;

	pml.groundBounces = qfalse;

	VectorCopy (pm->ps->velocity, primal_velocity);

	if ( gravity ) {
		VectorCopy( pm->ps->velocity, endVelocity );
		endVelocity[2] -= pm->ps->gravity * pml.frametime;
		pm->ps->velocity[2] = ( pm->ps->velocity[2] + endVelocity[2] ) * 0.5;
		primal_velocity[2] = endVelocity[2];
		if ( pml.groundPlane ) {
			if(!(pm->modParms.runFlags & RFL_CLIMBTECH) || PM_GroundSlideOkay(pml.groundTrace.plane.normal[2])){
				// slide along the ground plane
				PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
					pm->ps->velocity, overbounce);
				pml.groundBounces = qtrue;

				if (pml.groundTrace.plane.normal[2] >= MIN_WALK_NORMAL) {
					pml.clippedWalkable = qtrue; // uh am i putting this the right place? idk
				}
			}
		}
	}

	time_left = pml.frametime;

	// never turn against the ground plane
	if ( pml.groundPlane ) {
		numplanes = 1;
		VectorCopy( pml.groundTrace.plane.normal, planes[0] );
		if ((pm->modParms.runFlags & RFL_CLIMBTECH) && !PM_GroundSlideOkay(planes[0][2]))
		{
			planes[0][2] = 0;
			VectorNormalize(planes[0]);
		}
	} else {
		numplanes = 0;
	}

	if (pm->modParms.physics != MV_BOUNCE && pm->modParms.physics != MV_PINBALL) {
		// never turn against original velocity
		VectorNormalize2(pm->ps->velocity, planes[numplanes]);
		numplanes++;
	}

	for ( bumpcount=0 ; bumpcount < numbumps ; bumpcount++ ) {

		// calculate position we are trying to move to
		VectorMA( pm->ps->origin, time_left, pm->ps->velocity, end );

		// see if we can make it there
		PM_Trace( &trace, pm->ps->origin, pm->mins, pm->maxs, end, pm->ps->clientNum, pm->tracemask);

		if (trace.allsolid) {
			// entity is completely trapped in another solid
			pm->ps->velocity[2] = 0;	// don't build up falling damage, but allow sideways acceleration
			return qtrue;
		}

		if (trace.fraction > 0) {
			// actually covered some distance
			VectorCopy (trace.endpos, pm->ps->origin);
		}

		if (trace.fraction == 1) {
			 break;		// moved the entire distance
		}

		// save entity for contact
		PM_AddTouchEnt( trace.entityNum );

		time_left -= time_left * trace.fraction;

		if (numplanes >= MAX_CLIP_PLANES) {
			// this shouldn't really happen
			VectorClear( pm->ps->velocity );
			return qtrue;
		}

		VectorCopy(trace.plane.normal, normal);

		if ((pm->modParms.runFlags & RFL_CLIMBTECH) && !PM_GroundSlideOkay(normal[2]))
		{//wall-running
			//never push up off a sloped wall
			normal[2] = 0;
			VectorNormalize(normal);
		}

		//
		// if this is the same plane we hit before, nudge velocity
		// out along it, which fixes some epsilon issues with
		// non-axial planes
		//
		if (!(pm->modParms.runFlags & RFL_CLIMBTECH) || !(pm->ps->pm_flags & PMF_STUCK_TO_WALL))
		{//no sliding if stuck to wall!
			for (i = 0; i < numplanes; i++) {
				if (DotProduct(normal, planes[i]) > 0.99f) {
					VectorAdd(normal, pm->ps->velocity, pm->ps->velocity);
					break;
				}
			}
			if (i < numplanes) {
				continue;
			}
		}
		VectorCopy (normal, planes[numplanes]);
		numplanes++;

		//
		// modify velocity so it parallels all of the clip planes
		//

		// find a plane that it enters
		for ( i = 0 ; i < numplanes ; i++ ) {
			into = DotProduct( pm->ps->velocity, planes[i] );
			if ( into >= 0.1 ) {
				continue;		// move doesn't interact with the plane
			}

			// see how hard we are hitting things
			if ( -into > pml.impactSpeed ) {
				pml.impactSpeed = -into;
			}

			// slide along the plane
			PM_ClipVelocity (pm->ps->velocity, planes[i], clipVelocity, overbounce);

			// slide along the plane
			PM_ClipVelocity (endVelocity, planes[i], endClipVelocity, overbounce);
			pml.groundBounces = (qboolean)(pml.groundBounces || planes[i][2] >= MIN_WALK_NORMAL);

			if (planes[i][2] >= MIN_WALK_NORMAL) {
				pml.clippedWalkable = qtrue; // uh am i putting this the right place? idk
			}

			// see if there is a second plane that the new move enters
			for ( j = 0 ; j < numplanes ; j++ ) {
				if ( j == i ) {
					continue;
				}
				if ( DotProduct( clipVelocity, planes[j] ) >= 0.1f ) {
					continue;		// move doesn't interact with the plane
				}

				// try clipping the move to the plane
				PM_ClipVelocity( clipVelocity, planes[j], clipVelocity, overbounce);
				PM_ClipVelocity( endClipVelocity, planes[j], endClipVelocity, overbounce);
				pml.groundBounces = (qboolean)(pml.groundBounces || planes[j][2] >= MIN_WALK_NORMAL); 
				if (planes[j][2] >= MIN_WALK_NORMAL) {
					pml.clippedWalkable = qtrue; // uh am i putting this the right place? idk
				}

				// TODO MAYBE jaPRO player collision physics fix?

				// see if it goes back into the first clip plane
				if ( DotProduct( clipVelocity, planes[i] ) >= 0 ) {
					continue;
				}

				// slide the original velocity along the crease
				CrossProduct (planes[i], planes[j], dir);
				VectorNormalize( dir );
				d = DotProduct( dir, pm->ps->velocity );
				VectorScale( dir, d, clipVelocity );

				CrossProduct (planes[i], planes[j], dir);
				VectorNormalize( dir );
				d = DotProduct( dir, endVelocity );
				VectorScale( dir, d, endClipVelocity );

				// see if there is a third plane the the new move enters
				for ( k = 0 ; k < numplanes ; k++ ) {
					if ( k == i || k == j ) {
						continue;
					}
					if ( DotProduct( clipVelocity, planes[k] ) >= 0.1f ) {
						continue;		// move doesn't interact with the plane
					}

					// stop dead at a tripple plane interaction
					VectorClear( pm->ps->velocity ); // TODO can we make this nicer? is this why we can get stuck in sloped walls stuff?
					return qtrue;
				}
			}

			// if we have fixed all interactions, try another move
			VectorCopy( clipVelocity, pm->ps->velocity );
			VectorCopy( endClipVelocity, endVelocity );
			break;
		}
	}

	if ( gravity ) {
		VectorCopy( endVelocity, pm->ps->velocity );
	}

	// don't change velocity if in a timer (FIXME: is this correct?)
	if ( pm->ps->pm_time ) {
		VectorCopy( primal_velocity, pm->ps->velocity );
	}

	return (qboolean)( bumpcount != 0 );
}

// A dumbed down version of PM_SlideMove
// We only check if we freely get where we need to get and then do a check for ground under us.
// The apparent cause of the bug in a nutshell:
// We can freely (no clip/objects/floor in the way) travel/fall to the place where we will logically be in pml.frameTime time.
// This means no ramp boost through PM_ClipVelocity happens.
// Let's say we are falling straight down at effective velocity[2] == -1000 and 142 fps (pml.frametime == 0.007 which is 7msec).
// That puts us traveling down at about 7 units per frame. 
// If the ground is less than 7 units away from us, we will catch it in the initial PM_StepSlideMove and it will be clipped (ramp boost).
// If the ground is 8 units away from us, we will catch it the same way in the next frame.
// HOWEVER, if the ground is more than 7 and less than 7.25 units away from us, we will first fall the 7 units, and then the following ground check (which checks 0.25 units under new position)
// will determine that we are standing on ground without giving ramp boost. Which is also the place where Loda's old ramp boost fix is applied in case the ramp boost didn't happen which he checked via
// pml.clipped
qboolean  PMove::PM_PredictDeadRamp(qboolean gravity) {
	trace_t	trace;
	vec3_t		end;
	vec3_t		testVelocity;
	vec3_t		point;
	vec3_t		newPos;
	int			i, j, k;

	if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
		return qfalse;
	}

	VectorCopy(pm->ps->velocity, testVelocity);
	if (gravity) {
		testVelocity[2] -= pm->ps->gravity * pml.frametime;
		testVelocity[2] = (pm->ps->velocity[2] + testVelocity[2]) * 0.5;
	}

	// calculate position we are trying to move to
	VectorMA(pm->ps->origin, pml.frametime, testVelocity, end);

	// see if we can make it there
	PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, end, pm->ps->clientNum, pm->tracemask);


	if (trace.fraction == 1) {
		VectorCopy(trace.endpos, newPos);

		point[0] = newPos[0];
		point[1] = newPos[1];
		point[2] = newPos[2] - 0.25;

		PM_Trace(&trace, newPos, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);

		// do something corrective if the trace starts in a solid...
		if (trace.allsolid) {
			// jitter around
			for (i = -1; i <= 1; i++) {
				for (j = -1; j <= 1; j++) {
					for (k = -1; k <= 1; k++) {
						VectorCopy(newPos, point);
						point[0] += (float)i;
						point[1] += (float)j;
						point[2] += (float)k;
						PM_Trace(&trace, point, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
						if (!trace.allsolid) {
							point[0] = newPos[0];
							point[1] = newPos[1];
							point[2] = newPos[2] - 0.25;

							PM_Trace(&trace, newPos, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
							i = j = k = 2; // Stupid way to end the loop lol.
						}
					}
				}
			}
		}

		if (trace.fraction != 1.0 && (trace.plane.normal[0] != 0.0f || trace.plane.normal[1] != 0.0f || trace.plane.normal[2] != 1.0f)) {
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
PM_StepSlideMove

==================
*/
void  PMove::PM_StepSlideMove( qboolean gravity ) {
	vec3_t		start_o, start_v;
	// vec3_t		down_o, down_v;
	trace_t		trace;
//	float		down_dist, up_dist;
//	vec3_t		delta, delta2;
	vec3_t		up, down;
	vec3_t		nvel, prevel;
	float		stepSize;
	float		totalVel;
	float		pre_z;
	int			usingspeed;
	int			i;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);
	int			NEW_STEPSIZE = STEPSIZE;

	if (MovementStyleHasQuake2Ramps(pm->modParms.physics)) {
		PM_Q2StepSlideMove(gravity);
		return;
		if (pm->ps->velocity[2] > 0 && pm->cmd.upmove > 0) { // do we really need this?
			int jumpHeight = pm->ps->origin[2] - pm->ps->fd.forceJumpZStart;

			if (jumpHeight > 48)
				jumpHeight = 48;
			else if (jumpHeight < 22)
				jumpHeight = 22;

			NEW_STEPSIZE = 48 - jumpHeight + 22;

			//trap->SendServerCommand(-1, va("print \"new stepsize: %i, expected max end height: %i\n\"", NEW_STEPSIZE, NEW_STEPSIZE + (int)(pm->ps->origin[2] - pm->ps->fd.forceJumpZStart)));

			//This means that we can always clip things up to 48 units tall, if we are moving up when we hit it and from a bhop..
			//It means we can sometimes clip things up to 70 units tall, if we hit it in right part of jump
			//Should it be higher..? some of the things in q3 are 56 units tall..

			//NEW_STEPSIZE = 46;
			//Make stepsize equal to.. our current 48 - our current jumpheight ?
		}
		else
			NEW_STEPSIZE = 22;
	}

	i = 0;

	usingspeed = 0;

	VectorCopy (pm->ps->origin, start_o);
	VectorCopy (pm->ps->velocity, start_v);

	if (pm->debugLevel) {
		if (PM_PredictDeadRamp(gravity)) {
			Com_Printf("%i:predicting dead ramp\n", c_pmove);
		}
	}
	
	if (BG_InReboundHold(pm->ps->legsAnim))
	{
		gravity = qfalse;
	}

	if ( PM_SlideMove( gravity ) == 0 ) {
		return;		// we got exactly where we wanted to go first try	
	}

	if (pm->ps->fd.forcePowersActive & (1 << FP_SPEED))
	{
		usingspeed = 1;
	}

	VectorCopy(start_o, down);
	down[2] -= NEW_STEPSIZE;
	PM_Trace(&trace, start_o, pm->mins, pm->maxs, down, pm->ps->clientNum, pm->tracemask);
	VectorSet(up, 0, 0, 1);
	// never step up when you still have up velocity
	if ( pm->ps->velocity[2] > 0 && (trace.fraction == 1.0 ||
										DotProduct(trace.plane.normal, up) < 0.7f)) {

		if (!usingspeed)
		{
			return;
		}
	}

	// VectorCopy (pm->ps->origin, down_o);
	// VectorCopy (pm->ps->velocity, down_v);

	VectorCopy (start_o, up);
	up[2] += NEW_STEPSIZE;

	// test the player position if they were a stepheight higher
	PM_Trace(&trace, start_o, pm->mins, pm->maxs, up, pm->ps->clientNum, pm->tracemask);
	if ( trace.allsolid ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:bend can't step\n", c_pmove);
		}
		if (!usingspeed)
		{
			return;		// can't step up
		}
	}

	pml.clippedPre = qtrue;

	stepSize = trace.endpos[2] - start_o[2];
	// try slidemove from this position
	VectorCopy (trace.endpos, pm->ps->origin);
	VectorCopy (start_v, pm->ps->velocity);

	VectorCopy(pm->ps->velocity, prevel);

	pre_z = prevel[2];

	PM_SlideMove( gravity );

	if (pml.clippedWalkable) { 
		// only mark as clipped if it was a walkable surface, thats the only thing that matters for dead ramps
		// otherwise we might slide down a steep slope and still get a dead ramp because its "clipped" from the slope.
		pml.clipped = qtrue;
	}

	VectorSubtract(pm->ps->velocity, prevel, prevel);
	if (prevel[0] < 0)
	{
		prevel[0] = -prevel[0];
	}
	if (prevel[1] < 0)
	{
		prevel[1] = -prevel[1];
	}

	totalVel = prevel[0]+prevel[1];

#if !CLIENTSIDEONLY
	if (pre_z > 480 && (pre_z - pm->ps->velocity[2]) >= 480 && pm->ps->fd.forceJumpZStart)
	{ //smashed head on the ceiling during a force jump
		pm->ps->fd.forceSpeedDoDamage = (pre_z - pm->ps->velocity[2])*0.04;
		if (pm->numtouch)
		{ //do damage to the other player if we hit one
			while (i < pm->numtouch)
			{
				if (pm->touchents[i] < MAX_CLIENTS && pm->touchents[i] != pm->ps->clientNum)
				{
					pm->ps->fd.forceSpeedHitIndex = pm->touchents[i];
					break;
				}

				i++;
			}
		}

		i = 0;
	}
#endif

	if (usingspeed)
	{
		if (pm->ps->fd.forceSpeedSmash > 1.3 && totalVel > 500)
		{ //if we were going fast enough and hadn't hit a while in a while then smash into it hard
		  //the difference between our velocity pre and post colide must also be greater than 600 to do damage
			//Com_Printf("SMASH %f\n", pm->ps->fd.forceSpeedSmash);
			VectorCopy(start_v, nvel); //then bounce the player back a bit in the opposite of the direction he was going
			nvel[0] += start_o[0];
			nvel[1] += start_o[1];
			nvel[2] += start_o[2];
			VectorSubtract(start_o, nvel, nvel);
			pm->ps->velocity[0] = nvel[0]*0.1;
			pm->ps->velocity[1] = nvel[1]*0.1;
			pm->ps->velocity[2] = 64; 
#if !CLIENTSIDEONLY
			pm->ps->fd.forceSpeedDoDamage = pm->ps->fd.forceSpeedSmash*10; //do somewhere in the range of 15-25 damage, depending on speed
#endif
			pm->ps->fd.forceSpeedSmash = 0;
#if !CLIENTSIDEONLY
			if (pm->numtouch)
			{
				while (i < pm->numtouch)
				{
					if (pm->touchents[i] < MAX_CLIENTS && pm->touchents[i] != pm->ps->clientNum)
					{
						pm->ps->fd.forceSpeedHitIndex = pm->touchents[i];
						break;
					}

					i++;
				}
			}
#endif
			return;
		}

		pm->ps->fd.forceSpeedSmash -= 0.1f;
		//we hit a wall so decrease speed

		if (pm->ps->fd.forceSpeedSmash < 1)
		{
			pm->ps->fd.forceSpeedSmash = 1;
		}
	}

	// push down the final amount
	VectorCopy (pm->ps->origin, down);
	down[2] -= stepSize;
	PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, down, pm->ps->clientNum, pm->tracemask);
	if ( !trace.allsolid ) {
		VectorCopy (trace.endpos, pm->ps->origin);
	}
	if ( trace.fraction < 1.0 ) {
		PM_ClipVelocity( pm->ps->velocity, trace.plane.normal, pm->ps->velocity, overbounce);
		pml.groundBounces = (qboolean)( pml.groundBounces || trace.plane.normal[2] >= MIN_WALK_NORMAL);
		if (trace.plane.normal[2] >= MIN_WALK_NORMAL) {
			pml.clippedWalkable = qtrue; // uh am i putting this the right place? idk
		}
	}

#if 0
	// if the down trace can trace back to the original position directly, don't step
	PM_Trace( &trace, pm->ps->origin, pm->mins, pm->maxs, start_o, pm->ps->clientNum, pm->tracemask);
	if ( trace.fraction == 1.0 ) {
		// use the original move
		VectorCopy (down_o, pm->ps->origin);
		VectorCopy (down_v, pm->ps->velocity);
		if ( pm->debugLevel ) {
			Com_Printf("%i:bend\n", c_pmove);
		}
	} else 
#endif
	{
		// use the step move
		float	delta;

		delta = pm->ps->origin[2] - start_o[2];
		if ( delta > 2 ) {
			if ( delta < 7 ) {
				PM_AddEvent( EV_STEP_4_GENERAL );
			} else if ( delta < 11 ) {
				PM_AddEvent( EV_STEP_8_GENERAL );
			} else if ( delta < 15 ) {
				PM_AddEvent( EV_STEP_12_GENERAL );
			} else {
				PM_AddEvent( EV_STEP_16_GENERAL );
			}
		}
		if ( pm->debugLevel ) {
			Com_Printf("%i:stepped\n", c_pmove);
		}
	}
}



#pragma endregion "bg_slidemove"




// bg_pmove.c




#pragma region "bg_pmove"








// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_pmove.c -- both games player movement code
// takes a playerstate and a usercmd as input and returns a modifed playerstate

#define MAX_WEAPON_CHARGE_TIME 5000

//extern qboolean PM_GroundSlideOkay(float zNormal);
//extern float MovementOverbounceFactor(int moveStyle, playerState_t* ps, usercmd_t* ucmd);
//extern void PM_CheckBounceJump(vec3_t normal, vec3_t velocity);
//extern vec3_t flatNormal;


weaponData_t weaponData_1_02_pm[WP_NUM_WEAPONS_GENERAL] =
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

weaponData_t weaponData_1_03_pm[WP_NUM_WEAPONS_GENERAL] =
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
		25,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
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
		25,						//	int		altEnergyPerShot;	// Amount of energy used for alt-fire
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

weaponData_t weaponData_1_04_pm[WP_NUM_WEAPONS_GENERAL] =
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

float forceSpeedLevels[4] = 
{
	1, //rank 0?
	1.25,
	1.5,
	1.75
};

int forcePowerNeeded_1_04_pm[NUM_FORCE_POWER_LEVELS][NUM_FORCE_POWERS] =
{
	{ //nothing should be usable at rank 0..
		999,//FP_HEAL,//instant
		999,//FP_LEVITATION,//hold/duration
		999,//FP_SPEED,//duration
		999,//FP_PUSH,//hold/duration
		999,//FP_PULL,//hold/duration
		999,//FP_TELEPATHY,//instant
		999,//FP_GRIP,//hold/duration
		999,//FP_LIGHTNING,//hold/duration
		999,//FP_RAGE,//duration
		999,//FP_PROTECT,//duration
		999,//FP_ABSORB,//duration
		999,//FP_TEAM_HEAL,//instant
		999,//FP_TEAM_FORCE,//instant
		999,//FP_DRAIN,//hold/duration
		999,//FP_SEE,//duration
		999,//FP_SABERATTACK,
		999,//FP_SABERDEFEND,
		999//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		65,//FP_HEAL,//instant //was 25, but that was way too little
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		30,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		50,//FP_PROTECT,//duration
		50,//FP_ABSORB,//duration
		50,//FP_TEAM_HEAL,//instant
		50,//FP_TEAM_FORCE,//instant
		20,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		2,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		60,//FP_HEAL,//instant
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		30,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		25,//FP_PROTECT,//duration
		25,//FP_ABSORB,//duration
		33,//FP_TEAM_HEAL,//instant
		33,//FP_TEAM_FORCE,//instant
		20,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		1,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		50,//FP_HEAL,//instant //You get 5 points of health.. for 50 force points!
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		60,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		10,//FP_PROTECT,//duration
		10,//FP_ABSORB,//duration
		25,//FP_TEAM_HEAL,//instant
		25,//FP_TEAM_FORCE,//instant
		20,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		0,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	}
};

int forcePowerNeeded_1_02_pm[NUM_FORCE_POWER_LEVELS][NUM_FORCE_POWERS] =
{
	{ //nothing should be usable at rank 0..
		999,//FP_HEAL,//instant
		999,//FP_LEVITATION,//hold/duration
		999,//FP_SPEED,//duration
		999,//FP_PUSH,//hold/duration
		999,//FP_PULL,//hold/duration
		999,//FP_TELEPATHY,//instant
		999,//FP_GRIP,//hold/duration
		999,//FP_LIGHTNING,//hold/duration
		999,//FP_RAGE,//duration
		999,//FP_PROTECT,//duration
		999,//FP_ABSORB,//duration
		999,//FP_TEAM_HEAL,//instant
		999,//FP_TEAM_FORCE,//instant
		999,//FP_DRAIN,//hold/duration
		999,//FP_SEE,//duration
		999,//FP_SABERATTACK,
		999,//FP_SABERDEFEND,
		999//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		25,//FP_HEAL,//instant
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		30,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		50,//FP_PROTECT,//duration
		50,//FP_ABSORB,//duration
		50,//FP_TEAM_HEAL,//instant
		50,//FP_TEAM_FORCE,//instant
		10,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		2,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		25,//FP_HEAL,//instant
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		30,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		25,//FP_PROTECT,//duration
		25,//FP_ABSORB,//duration
		33,//FP_TEAM_HEAL,//instant
		33,//FP_TEAM_FORCE,//instant
		10,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		1,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	},
	{
		25,//FP_HEAL,//instant
		10,//FP_LEVITATION,//hold/duration
		50,//FP_SPEED,//duration
		20,//FP_PUSH,//hold/duration
		20,//FP_PULL,//hold/duration
		20,//FP_TELEPATHY,//instant
		60,//FP_GRIP,//hold/duration
		1,//FP_LIGHTNING,//hold/duration
		50,//FP_RAGE,//duration
		10,//FP_PROTECT,//duration
		10,//FP_ABSORB,//duration
		25,//FP_TEAM_HEAL,//instant
		25,//FP_TEAM_FORCE,//instant
		10,//FP_DRAIN,//hold/duration
		20,//FP_SEE,//duration
		0,//FP_SABERATTACK,
		0,//FP_SABERDEFEND,
		20//FP_SABERTHROW,
		//NUM_FORCE_POWERS
	}
};

float forceJumpHeightMax[NUM_FORCE_POWER_LEVELS] =
{
	66,//normal jump (32+stepheight(18)+crouchdiff(24) = 74)
	130,//(96+stepheight(18)+crouchdiff(24) = 138)
	226,//(192+stepheight(18)+crouchdiff(24) = 234)
	418//(384+stepheight(18)+crouchdiff(24) = 426)
};

void PMove::PM_UpdateAntiLoop() {
	DF_AntiLoop_NewAngle(&pm->antiLoop, pm->lastAntiLoopVelocity, pm->ps->velocity, pm->ps->basespeed, (qboolean)(pm->modParms.raceMode && pm->ps->duelTime));
	VectorCopy(pm->ps->velocity, pm->lastAntiLoopVelocity);
}

//rww - Get a pointer to the bgEntity by the index
bgEntity_t* PMove::PM_BGEntForNum(int num)
{
	bgEntity_t* ent;

	if (!pm)
	{
		assert(!"You cannot call PM_BGEntForNum outside of pm functions!");
		return NULL;
	}
#if CLIENTSIDEONLY
	if (!pm->getEnt)
	{
		assert(!"getEnt address not set");
		return NULL;
	}

	assert(num >= 0 && num < MAX_GENTITIES);

	ent = pm->getEnt(num,getEntReference);
#else
	if (!pm->baseEnt)
	{
		assert(!"Base entity address not set");
		return NULL;
	}

	if (!pm->entSize)
	{
		assert(!"sizeof(ent) is 0, impossible (not set?)");
		return NULL;
	}

	assert(num >= 0 && num < MAX_GENTITIES);

	ent = (bgEntity_t*)((byte*)pm->baseEnt + pm->entSize * (num));
#endif

	return ent;
}

void  PMove::PM_GrabWallForJump(int anim)
{//NOTE!!! assumes an appropriate anim is being passed in!!!
	PM_SetAnim(SETANIM_BOTH, anim, SETANIM_FLAG_RESTART | SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD, 100);
	PM_AddEvent(EV_JUMP_GENERAL);//make sound for grab
	pm->ps->pm_flags |= PMF_STUCK_TO_WALL;
}


/*
Q_INLINE int PM_GetMovePhysics(void)
{
	if (!pm || !pm->ps)
		return MV_JK2;

	if (pm->mod == SVMOD_TOMMYTERNAL && pm->ps->stats[STAT_RACEMODE]) {
		return pm->ps->stats[STAT_MOVEMENTSTYLE];
	}
	else if(pm->mod == SVMOD_JK2PRO) {
		return pm->ps->stats[STAT_MOVEMENTSTYLE];
	}

	return MV_JK2; // this can happen when we die in racemode too!
}

Q_INLINE int PM_GetRunFlags(void)
{
	if (!pm || !pm->ps)
		return 0;

	if (pm->mod == SVMOD_TOMMYTERNAL && pm->ps->stats[STAT_RACEMODE]) {
		return pm->ps->stats[STAT_RUNFLAGS];
	}

	return 0; // this can happen when we die in racemode too!
}

Q_INLINE int PM_GetMsecRestrict(void)
{
	if (!pm || !pm->ps)
		return 0;
	if (pm->mod == SVMOD_TOMMYTERNAL && pm->ps->stats[STAT_RACEMODE]) {
		return pm->ps->stats[STAT_MSECRESTRICT];
	}

	return 0; // this can happen when we die in racemode too!
}


Q_INLINE int PM_GetRaceMode(pmove_t* pmove)
{
	if (!pmove || !pmove->ps)
		return 0;
	if (pmove->mod == SVMOD_TOMMYTERNAL) {
		return pmove->ps->stats[STAT_RACEMODE];
	}
	else if (pmove->mod == SVMOD_JK2PRO) {
		return pmove->ps->stats[STAT_RACEMODE];
	}
	return 0; // this can happen when we die in racemode too!
}
*/
int  PMove::PM_GetSaberStance(void)
{
	if ( pm->ps->dualBlade )
	{
		return BOTH_STAND1_GENERAL;
	}
	if (pm->ps->fd.saberAnimLevel == FORCE_LEVEL_2)
	{ //medium
		return BOTH_STAND2_GENERAL;
	}
	if (pm->ps->fd.saberAnimLevel == FORCE_LEVEL_3)
	{ //strong
		return BOTH_SABERSLOW_STANCE_GENERAL;
	}

	//fast
	return BOTH_SABERFAST_STANCE_GENERAL;
}

qboolean  PMove::PM_DoSlowFall(void)
{
	if ( ( (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL || (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_LEFT_GENERAL ) && pm->ps->legsTimer > 500 )
	{
		return qtrue;
	}

	return qfalse;
}

/*
===============
PM_AddEvent

===============
*/
void  PMove::PM_AddEvent( int newEvent ) {
	BG_AddPredictableEventToPlayerstate( newEvent, 0, pm->ps );
}

void  PMove::PM_AddEventWithParm( int newEvent, int parm ) 
{
	BG_AddPredictableEventToPlayerstate( newEvent, parm, pm->ps );
}

/*
===============
PM_AddTouchEnt
===============
*/
void  PMove::PM_AddTouchEnt( int entityNum ) {
	int		i;

	if ( entityNum == ENTITYNUM_WORLD ) {
		return;
	}
	if ( pm->numtouch == MAXTOUCH ) {
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; i++ ) {
		if ( pm->touchents[ i ] == entityNum ) {
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
	pm->numtouch++;
}



/*
==================
PM_ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)

This is the Q2 version of it. I'm not actually using it atm, not even for the Q2 ramps. Just for reference.
==================
*/
#define	STOP_EPSILON	0.1

void  PMove::PM_ClipVelocityQ2(vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float	backoff;
	float	change;
	int		i;

	backoff = DotProduct(in, normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void  PMove::PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce ) {
	float	backoff;
	float	change;
	int		i;

	if ((pm->modParms.runFlags & RFL_CLIMBTECH)&& (pm->ps->pm_flags & PMF_STUCK_TO_WALL))
	{//no sliding!
		VectorCopy(in, out);
		return;
	}
	
	backoff = DotProduct (in, normal);
	
	if ( backoff < 0 ) {
		backoff *= overbounce;
	} else {
		backoff /= overbounce;
	}

	for ( i=0 ; i<3 ; i++ ) {
		change = normal[i]*backoff;
		out[i] = in[i] - change;
	}
}


/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
 void  PMove::PM_Friction( void ) {
	vec3_t	vec;
	float	*vel;
	float	speed, newspeed, control;
	float	drop, realfriction = pm_friction; // for sp there is pm->ps->friction. is that relevant for us?
	
	vel = pm->ps->velocity;
	
	VectorCopy( vel, vec );
	if ( pml.walking ) { 
		vec[2] = 0;	// ignore slope movement
	}

	speed = VectorLength(vec);
	if (speed < 1) {
		if ((pm->modParms.physics == MV_BOUNCE || pm->modParms.physics == MV_PINBALL) && vel[2]) {
			vec[2] = vel[2]; // otherwise we stay forever in a bouncy vel[2] state on spawn and cant savespawn
			speed = VectorLength(vec);
		}
		else {
			vel[0] = 0;
			vel[1] = 0;		// allow sinking underwater
			// FIXME: still have z friction underwater?
			return;
		}
	}

	if (MovementIsQuake3Based(pm->modParms.physics))
		realfriction = pm_vq3_friction;

	drop = 0;

	// apply ground friction
	if ( pm->waterlevel <= 1 ) {
		if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
			// if getting knocked back, no friction
			if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
				//If the use key is pressed. slow the player more quickly
				if (pm->modParms.physics == MV_JK2SP && pm->cmd.buttons & BUTTON_USE)
					realfriction *= pm_sp_frictionModifier;

				control = speed < pm_stopspeed ? pm_stopspeed : speed;
				drop += control* realfriction *pml.frametime;
			}
		}
	}

	// apply water friction even if just wading
	if ( pm->waterlevel ) {
		float waterFriction = pm_waterfriction;
		if (pm->modParms.physics == MV_SICKO) {
			waterFriction = 0.4f;
		}
		else 
		 if (MovementIsQuake3Based(pm->modParms.physics)) {
			waterFriction = 0.8f;
		}
		drop += speed* waterFriction *pm->waterlevel*pml.frametime;
	}

	if ( pm->ps->pm_type == PM_SPECTATOR || pm->ps->pm_type == PM_FLOAT )
	{
		if (pm->ps->pm_type == PM_FLOAT)
		{ //almost no friction while floating
			drop += speed*0.1*pml.frametime;
		}
		else
		{
			drop += speed*pm_spectatorfriction*pml.frametime;
		}
	}

	// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0) {
		newspeed = 0;
	}
	if (speed == 0) { // normally we wouldnt get here but since bounce has a bit of hack ... lets avoid division by 0
		newspeed = 0;
	}
	else {
		newspeed /= speed;
	}

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
}


/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
 void  PMove::PM_Accelerate( vec3_t wishdir, float wishspeed, float accel ) {
#if 1
	// q2 style
	int			i;
	float		addspeed, accelspeed, currentspeed;

	currentspeed = DotProduct (pm->ps->velocity, wishdir);
	addspeed = wishspeed - currentspeed;

	accelspeed = accel * pml.frametime * wishspeed;

	pm->accelMiss = (addspeed- accelspeed) / accelspeed;
	pm->wishSpeed = wishspeed;

	if (addspeed <= 0) {
		return;
	}
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}
	
	for (i=0 ; i<3 ; i++) {
		pm->ps->velocity[i] += accelspeed*wishdir[i];	
	}
#else
	// proper way (avoids strafe jump maxspeed bug), but feels bad
	vec3_t		wishVelocity;
	vec3_t		pushDir;
	float		pushLen;
	float		canPush;

	VectorScale( wishdir, wishspeed, wishVelocity );
	VectorSubtract( wishVelocity, pm->ps->velocity, pushDir );
	pushLen = VectorNormalize( pushDir );

	canPush = accel*pml.frametime*wishspeed;
	if (canPush > pushLen) {
		canPush = pushLen;
	}

	VectorMA( pm->ps->velocity, canPush, pushDir, pm->ps->velocity );
#endif
}


/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
 void  PMove::PM_SickoAccelerate( vec3_t wishdir, float wishspeed, float baseAccel, float maxAccel) {
	// q2 style
	int			i;
	float		addspeed, accelspeed, currentspeed;
	float		baseInc, accel;

	currentspeed = DotProduct (pm->ps->velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0) {
		return;
	}
	baseInc = pml.frametime * wishspeed;

	accel = baseInc ? (addspeed / baseInc) : 0; // avoid division by 0 just in case

	if (accel > maxAccel) {
		accel = maxAccel;
	}
	else if (accel < baseAccel) {
		accel = baseAccel;
	}

	accelspeed = accel* baseInc;
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}
	
	for (i=0 ; i<3 ; i++) {
		pm->ps->velocity[i] += accelspeed*wishdir[i];	
	}
}
/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
 void  PMove::PM_QuaJKAccelerate( vec3_t wishdir, float wishspeed, float baseAccel, float maxAccel, float maxAccelWishSpeed) {
	// q2 style
	int			i;
	float		addspeed, accelspeed, currentspeed;
	float		accel;
	float		f,finalWishSpeed;
	float		accelAddSlow, accelAddHigh;
	float		neededSpeedSlow, neededSpeedHigh;

	currentspeed = DotProduct (pm->ps->velocity, wishdir);

	if (currentspeed >= wishspeed) return;

	accelAddSlow = baseAccel * pml.frametime * wishspeed;
	accelAddHigh = maxAccel * pml.frametime * maxAccelWishSpeed;

	neededSpeedSlow = wishspeed - accelAddSlow;
	neededSpeedHigh = maxAccelWishSpeed - accelAddHigh;

	if (neededSpeedSlow == neededSpeedHigh) {
		// idk if this can happen but just to avoid division by 0
		f = 1;
	}
	else {
		f = (currentspeed - neededSpeedHigh) / (neededSpeedSlow - neededSpeedHigh);
	}

	if (f < 0) f = 0;
	else if (f > 1) f = 1;

	accel = (f * baseAccel) + ((1.0f - f) * maxAccel);
	finalWishSpeed = (f * wishspeed) + ((1.0f - f) * maxAccelWishSpeed);

	accelspeed = accel * pml.frametime * finalWishSpeed;

	addspeed = finalWishSpeed - currentspeed; 
	if (addspeed <= 0) {
		return;
	}

	/*
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0) {
		return;
	}

	baseInc = pml.frametime * wishspeed;

	accel = addspeed / baseInc;

	if (accel > maxAccel) {
		accel = maxAccel;
	}
	else if (accel < baseAccel) {
		accel = baseAccel;
	}

	f = (accel - baseAccel) / (maxAccel - baseAccel);

	finalWishSpeed = (f * maxAccelWishSpeed) + ((1.0f - f) * baseAccel);

	accelspeed = accel* pml.frametime*finalWishSpeed;*/
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}
	
	for (i=0 ; i<3 ; i++) {
		pm->ps->velocity[i] += accelspeed*wishdir[i];	
	}
}
 void  PMove::PM_DreamAccelerate( vec3_t wishdir, float wishspeed, float baseAccel, float maxAccel, float maxAccelWishSpeed) {
	// q2 style
	int			i;
	float		addspeed, accelspeed, currentspeed;
	float		accel;
	float		f,finalWishSpeed;
	float		accelAddSlow, accelAddHigh;
	float		neededSpeedSlow, neededSpeedHigh;
	float		scale;
	float		maxFront;
	float		tmp;
	float		h = 2.0;
	float		velTotal = VectorLength(pm->ps->velocity);
	float		w;
	float		idealVelRatio;
	static const float backpow = 5.0f;

	currentspeed = DotProduct (pm->ps->velocity, wishdir);

	if (currentspeed >= wishspeed) return;

	accelAddSlow = baseAccel * pml.frametime * wishspeed;
	accelAddHigh = maxAccel * pml.frametime * maxAccelWishSpeed;

	neededSpeedSlow = wishspeed - accelAddSlow;
	neededSpeedHigh = maxAccelWishSpeed - accelAddHigh;

	if (currentspeed < 0) {
		f = (-1.0f*currentspeed)/velTotal;
		f = 1.0f - powf(1.0f - f, backpow);
	}
	else {
		if (neededSpeedSlow == neededSpeedHigh) {
			f = 1.0f;
		}
		else {
			f = (currentspeed - neededSpeedHigh) / (neededSpeedSlow - neededSpeedHigh);
		}
	}

	if (f < 0) f = 0;
	else if (f > 1) f = 1;

	accel = (f * baseAccel) + ((1.0f - f) * maxAccel);
	finalWishSpeed = (f * wishspeed) + ((1.0f - f) * maxAccelWishSpeed);



	accelspeed = accel * pml.frametime * finalWishSpeed;

	addspeed = finalWishSpeed - currentspeed; 
	if (addspeed <= 0) {
		return;
	}

	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}

	w = accelAddSlow + wishspeed;
	idealVelRatio = (w * w) / (velTotal*velTotal);
	idealVelRatio *= accelAddSlow / (wishspeed + accelAddSlow);
	maxFront = idealVelRatio * velTotal;

	// don't even ask lmfao...
	tmp = 2 * wishdir[0] * pm->ps->velocity[0] + 2 * wishdir[1] * pm->ps->velocity[1] + 2.0f * wishdir[2] * pm->ps->velocity[2];
	scale = (-2.0f * wishdir[0] * pm->ps->velocity[0] - 2.0f * wishdir[1] * pm->ps->velocity[1] - 2.0f * wishdir[2] * pm->ps->velocity[2] + sqrtf(tmp*tmp + 4 * h * maxFront * (wishdir[0]*wishdir[0] + wishdir[1]*wishdir[1] + wishdir[2]*wishdir[2]) * (h * maxFront + 2.0f * sqrtf(pm->ps->velocity[0]* pm->ps->velocity[0] + pm->ps->velocity[1]*pm->ps->velocity[1] + pm->ps->velocity[2]*pm->ps->velocity[2])))) / (2.0 * h * (wishdir[0]*wishdir[0] + wishdir[1]*wishdir[1] + wishdir[2]*wishdir[2]));

	if (scale < 0 || fpclassify(scale) == FP_NAN) {
		return;
	} 
	else if (scale > accelspeed)
	{
		scale = accelspeed;
	}
	accelspeed = scale;
	

applyaccel:
	for (i=0 ; i<3 ; i++) {
		pm->ps->velocity[i] += accelspeed*wishdir[i];	
	}
}



/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
 float  PMove::PM_CmdScale( usercmd_t *cmd ) {
	int		max;
	float	total;
	float	scale;
	int		umove = 0; //cmd->upmove;
			//don't factor upmove into scaling speed

	if (pm->modParms.physics == MV_JK2SP) {
		umove = cmd->upmove;
	}

	max = abs( cmd->forwardmove );
	if ( abs( cmd->rightmove ) > max ) {
		max = abs( cmd->rightmove );
	}
	if ( abs( umove ) > max ) {
		max = abs( umove );
	}
	if ( !max ) {
		return 0;
	}

	total = sqrtf( cmd->forwardmove * cmd->forwardmove
		+ cmd->rightmove * cmd->rightmove + umove * umove );
	scale = (float)pm->ps->speed * max / ( 127.0f * total );

	return scale;
}


/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
 void  PMove::PM_SetMovementDir( void ) {
	if ( pm->cmd.forwardmove || pm->cmd.rightmove ) {
		if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 0;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 1;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove == 0 ) {
			pm->ps->movementDir = 2;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 3;
		} else if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 4;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 5;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove == 0 ) {
			pm->ps->movementDir = 6;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 7;
		}
	} else {
		// if they aren't actively going directly sideways,
		// change the animation to the diagonal so they
		// don't stop too crooked
		if ( pm->ps->movementDir == 2 ) {
			pm->ps->movementDir = 1;
		} else if ( pm->ps->movementDir == 6 ) {
			pm->ps->movementDir = 7;
		} 
	}
}

#define METROID_JUMP 1

qboolean  PMove::PM_ForceJumpingUp(void)
{
	if ( !(pm->ps->fd.forcePowersActive&(1<<FP_LEVITATION)) && pm->ps->fd.forceJumpCharge )
	{//already jumped and let go
		return qfalse;
	}

	if ( BG_InSpecialJump( pm->ps->legsAnim, pm->modParms.runFlags ) )
	{
		return qfalse;
	}

	if (BG_SaberInSpecial(pm->ps->saberMove))
	{
		return qfalse;
	}

	if (BG_SaberInSpecialAttack(pm->ps->legsAnim))
	{
		return qfalse;
	}

	if (BG_HasYsalamiri(pm->gametype, pm->ps))
	{
		return qfalse;
	}

	if (!BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION))
	{
		return qfalse;
	}

	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE && //in air
		(pm->ps->pm_flags & PMF_JUMP_HELD) && //jumped
		pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_0 && //force-jump capable
		pm->ps->velocity[2] > 0 )//going up
	{
		return qtrue;
	}
	return qfalse;
}

void  PMove::PM_JumpForDir( void )
{
	int anim = BOTH_JUMP1_GENERAL;
	if ( pm->cmd.forwardmove > 0 ) 
	{
		anim = BOTH_JUMP1_GENERAL;
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	} 
	else if ( pm->cmd.forwardmove < 0 )
	{
		anim = BOTH_JUMPBACK1_GENERAL;
		pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	}
	else if ( pm->cmd.rightmove > 0 ) 
	{
		anim = BOTH_JUMPRIGHT1_GENERAL;
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	}
	else if ( pm->cmd.rightmove < 0 ) 
	{
		anim = BOTH_JUMPLEFT1_GENERAL;
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	}
	else
	{
		anim = BOTH_JUMP1_GENERAL;
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	}
	if(!BG_InDeathAnim(pm->ps->legsAnim))
	{
		PM_SetAnim(SETANIM_LEGS,anim,SETANIM_FLAG_OVERRIDE, 100);
	}
}

void  PMove::PM_SetPMViewAngle(playerState_t *ps, vec3_t angle, usercmd_t *ucmd)
{
	int			i;

	for (i=0 ; i<3 ; i++)
	{ // set the delta angle
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ps->delta_angles[i] = cmdAngle - ucmd->angles[i];
	}
	VectorCopy (angle, ps->viewangles);
}

qboolean  PMove::PM_AdjustAngleForWallRun( playerState_t *ps, usercmd_t *ucmd, qboolean doMove )
{
	if (( (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL || (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_LEFT_GENERAL ) && ps->legsTimer > 500 )
	{//wall-running and not at end of anim
		//stick to wall, if there is one
		vec3_t	rt, traceTo, mins, maxs, fwdAngles;
		trace_t	trace;
		float	dist, yawAdjust;

		VectorSet(mins, -15, -15, 0);
		VectorSet(maxs, 15, 15, 24);
		VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

		AngleVectors( fwdAngles, NULL, rt, NULL );
		if ( (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL )
		{
			dist = 128;
			yawAdjust = -90;
		}
		else
		{
			dist = -128;
			yawAdjust = 90;
		}
		VectorMA( ps->origin, dist, rt, traceTo );
		
		PM_Trace( &trace, ps->origin, mins, maxs, traceTo, ps->clientNum, MASK_PLAYERSOLID );

		if ( trace.fraction < 1.0f )
		{//still a wall there
			if ( (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL )
			{
				ucmd->rightmove = 127;
			}
			else
			{
				ucmd->rightmove = -127;
			}
			if ( ucmd->upmove < 0 )
			{
				ucmd->upmove = 0;
			}
			//make me face perpendicular to the wall
			// NOTE: Something about these 3 lines is perhaps not quite non-deterministic (or is it?) but
			// it makes replays not work properly. Why is that?
			ps->viewangles[YAW] = vectoyaw( trace.plane.normal )+yawAdjust;

			PM_SetPMViewAngle(ps, ps->viewangles, ucmd);

			ucmd->angles[YAW] = ((int)(ANGLE2SHORT( ps->viewangles[YAW] ))) - (int)ps->delta_angles[YAW];
			ucmd->angles[YAW] &= 65535;
			if ( doMove )
			{
				//push me forward
				vec3_t	fwd;
				float	zVel = ps->velocity[2];
				if ( ps->legsTimer > 500 )
				{//not at end of anim yet
					float speed = 175;

					fwdAngles[YAW] = ps->viewangles[YAW];
					AngleVectors( fwdAngles, fwd, NULL, NULL );

					if ( ucmd->forwardmove < 0 )
					{//slower
						speed = 100;
					}
					else if ( ucmd->forwardmove > 0 )
					{
						speed = 250;//running speed
					}
					VectorScale( fwd, speed, ps->velocity );
				}
				ps->velocity[2] = zVel;//preserve z velocity
				//pull me toward the wall, too
				VectorMA( ps->velocity, dist, rt, ps->velocity );
			}
			ucmd->forwardmove = 0;
			return qtrue;
		}
		else if ( doMove )
		{//stop it
			if ( (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL )
			{
				PM_SetAnim(SETANIM_BOTH, BOTH_WALL_RUN_RIGHT_STOP_GENERAL, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 0);
			}
			else if ( (ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_LEFT_GENERAL )
			{
				PM_SetAnim(SETANIM_BOTH, BOTH_WALL_RUN_LEFT_STOP_GENERAL, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 0);
			}
		}
	}

	return qfalse;
}

#define	JUMP_OFF_WALL_SPEED	200.0f
//nice...
 float  PMove::BG_ForceWallJumpStrength(void)
{
	return (forceJumpStrength[FORCE_LEVEL_3] / 2.5f);
}
qboolean  PMove::PM_AdjustAngleForWallJump(playerState_t* ps, usercmd_t* ucmd, qboolean doMove)
{
	if (((BG_InReboundJump(ps->legsAnim) || BG_InReboundHold(ps->legsAnim))
		&& (BG_InReboundJump(ps->torsoAnim) || BG_InReboundHold(ps->torsoAnim)))
		|| (pm->ps->pm_flags & PMF_STUCK_TO_WALL))
	{//hugging wall, getting ready to jump off
		//stick to wall, if there is one
		vec3_t	checkDir, traceTo, mins, maxs, fwdAngles;
		trace_t	trace;
		float	dist = 128.0f, yawAdjust;

		VectorSet(mins, pm->mins[0], pm->mins[1], 0);
		VectorSet(maxs, pm->maxs[0], pm->maxs[1], 24);
		VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

		switch (ps->legsAnim)
		{
		case BOTH_FORCEWALLREBOUND_RIGHT_GENERAL:
		case BOTH_FORCEWALLHOLD_RIGHT_GENERAL:
			AngleVectors(fwdAngles, NULL, checkDir, NULL);
			yawAdjust = -90;
			break;
		case BOTH_FORCEWALLREBOUND_LEFT_GENERAL:
		case BOTH_FORCEWALLHOLD_LEFT_GENERAL:
			AngleVectors(fwdAngles, NULL, checkDir, NULL);
			VectorScale(checkDir, -1, checkDir);
			yawAdjust = 90;
			break;
		case BOTH_FORCEWALLREBOUND_FORWARD_GENERAL:
		case BOTH_FORCEWALLHOLD_FORWARD_GENERAL:
			AngleVectors(fwdAngles, checkDir, NULL, NULL);
			yawAdjust = 180;
			break;
		case BOTH_FORCEWALLREBOUND_BACK_GENERAL:
		case BOTH_FORCEWALLHOLD_BACK_GENERAL:
			AngleVectors(fwdAngles, checkDir, NULL, NULL);
			VectorScale(checkDir, -1, checkDir);
			yawAdjust = 0;
			break;
		default:
			//WTF???
			pm->ps->pm_flags &= ~PMF_STUCK_TO_WALL;
			return qfalse;
			break;
		}
		//if (pm->debugMelee)
		//[JAPRO - Serverside + Clientside - Physics - Change g_debugmelee 1 so that it has kungfu moves but keeps normal wallgrab.  Create g_debugmelee 2 for kung fu moves and infinite wallgrab - Start]
		if (pm->debugMelee > 1) // we go directly to the JAPLUS/jaPRO behavior in jk2. why not, we're porting what ppl are using
		{//uber-skillz
			if (ucmd->upmove > 0)
			{//hold on until you let go manually
				if (BG_InReboundHold(ps->legsAnim))
				{//keep holding
					if (ps->legsTimer < 150)
					{
						ps->legsTimer = 150;
					}
				}
				else
				{//if got to hold part of anim, play hold anim
					if (ps->legsTimer <= 300)
					{
						ps->saberHolstered = (qboolean)2;
						PM_SetAnim(SETANIM_BOTH, BOTH_FORCEWALLRELEASE_FORWARD_GENERAL + (ps->legsAnim - BOTH_FORCEWALLHOLD_FORWARD_GENERAL), SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD, 0);
						ps->legsTimer = ps->torsoTimer = 150;
					}
				}
			}
		}
		//[JAPRO - Serverside + Clientside - Physics - Change g_debugmelee 1 so that it has kungfu moves but keeps normal wallgrab.  Create g_debugmelee 2 for kung fu moves and infinite wallgrab - End]
		VectorMA(ps->origin, dist, checkDir, traceTo);
		PM_Trace(&trace, ps->origin, mins, maxs, traceTo, ps->clientNum, MASK_PLAYERSOLID);
		if ( //ucmd->upmove <= 0 && 
			ps->legsTimer > 100 &&
			trace.fraction < 1.0f &&
			fabs(trace.plane.normal[2]) <= 0.2f/*MAX_WALL_GRAB_SLOPE*/)
		{//still a vertical wall there
			//FIXME: don't pull around 90 turns
			/*
			if ( ent->s.number || !player_locked )
			{
				ucmd->forwardmove = 127;
			}
			*/
			if (ucmd->upmove < 0)
			{
				ucmd->upmove = 0;
			}
			//align me to the wall
			ps->viewangles[YAW] = vectoyaw(trace.plane.normal) + yawAdjust;
			PM_SetPMViewAngle(ps, ps->viewangles, ucmd);
			/*
			if ( ent->client->ps.viewEntity <= 0 || ent->client->ps.viewEntity >= ENTITYNUM_WORLD )
			{//don't clamp angles when looking through a viewEntity
				SetClientViewAngle( ent, ent->client->ps.viewangles );
			}
			*/
			ucmd->angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - ps->delta_angles[YAW];
			//if ( ent->s.number || !player_locked )
			if (1)
			{
				if (doMove)
				{
					//pull me toward the wall
					VectorScale(trace.plane.normal, -128.0f, ps->velocity);
				}
			}
			ucmd->upmove = 0;
			ps->pm_flags |= PMF_STUCK_TO_WALL;
			return qtrue;
		}
		else if (doMove
			&& (ps->pm_flags & PMF_STUCK_TO_WALL))
		{//jump off
			//push off of it!
			ps->pm_flags &= ~PMF_STUCK_TO_WALL;
			ps->velocity[0] = ps->velocity[1] = 0;
			VectorScale(checkDir, -JUMP_OFF_WALL_SPEED, ps->velocity);
			ps->velocity[2] = BG_ForceWallJumpStrength();
			ps->pm_flags |= PMF_JUMP_HELD;//PMF_JUMPING|PMF_JUMP_HELD;
			//G_SoundOnEnt( ent, CHAN_BODY, "sound/weapons/force/jump.wav" );
#if !CLIENTSIDEONLY
			ps->fd.forceJumpSound = 1; //this is a stupid thing, i should fix it.
#endif
			//ent->client->ps.forcePowersActive |= (1<<FP_LEVITATION);
			if (ps->origin[2] < ps->fd.forceJumpZStart)
			{
				PM_SetForceJumpZStart(ps->origin[2]);
				//ps->fd.forceJumpZStart = ps->origin[2];
			}
			//FIXME do I need this?

			BG_ForcePowerDrain(ps, FP_LEVITATION, 10);
			//no control for half a second
			ps->pm_flags |= PMF_TIME_KNOCKBACK;
			ps->pm_time = 500;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 127;

			if (BG_InReboundHold(ps->legsAnim))
			{//if was in hold pose, release now
				PM_SetAnim(SETANIM_BOTH, BOTH_FORCEWALLRELEASE_FORWARD_GENERAL + (ps->legsAnim - BOTH_FORCEWALLHOLD_FORWARD_GENERAL), SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD, 0);
			}
			else
			{
				//PM_JumpForDir();
				PM_SetAnim(SETANIM_LEGS, BOTH_FORCEJUMP1_GENERAL, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_RESTART, 0);
			}

			//return qtrue;
		}
	}
	ps->pm_flags &= ~PMF_STUCK_TO_WALL;
	return qfalse;
}




//Set the height for when a force jump was started. If it's 0, nuge it up (slight hack to prevent holding jump over slopes)
void  PMove::PM_SetForceJumpZStart(float value)
{
	pm->ps->fd.forceJumpZStart = value;
	if (!pm->ps->fd.forceJumpZStart && (jk2gameplay == VERSION_1_04 || (pm->modParms.runFlags & RFL_JUMPBUGDISABLE) || pm->modParms.physics == MV_JK2SP))
	{
		pm->ps->fd.forceJumpZStart -= 0.1f;
	}
}

void  PMove::PM_SetGroundEntityNum(int num)
{
	if (num != ENTITYNUM_NONE) {
		pm->ps->groundTime = 0;
	}
	else if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
		pm->ps->groundTime = pm->cmd.serverTime; // remember time we left ground for charge jump movement
	}
	pm->ps->groundEntityNum = num;

}

/*
=============
PM_CheckJump
=============
*/
qboolean PMove::PM_CheckJump( void ) 
{
	qboolean onlyWallGrab = qfalse; // in jk 1.02, if we are in air and not wallrunning, we skip out early. but we need to go further for wallgrab. in that case ignore all but wallgrab
	int JUMP_VELOCITY_NEW = JUMP_VELOCITY;

	if (pm->modParms.physics != MV_JK2SP) {
		if (pm->ps->usingATST)
		{
			return qfalse;
		}

		if (pm->ps->forceHandExtend == HANDEXTEND_KNOCKDOWN)
		{
			return qfalse;
		}
	}

	//Don't allow jump until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return qfalse;		
	}

	if ( PM_InKnockDown( pm->ps ) || BG_InRoll( pm->ps, pm->ps->legsAnim ) ) 
	{//in knockdown
		return qfalse;		
	}

	if (MovementIsQuake3Based(pm->modParms.physics)) {
		JUMP_VELOCITY_NEW = 270;
	}

	if (pm->ps->groundEntityNum != ENTITYNUM_NONE || pm->ps->origin[2] < pm->ps->fd.forceJumpZStart)
	{
		pm->ps->fd.forcePowersActive &= ~(1<<FP_LEVITATION);
	}

	if (pm->ps->fd.forcePowersActive & (1 << FP_LEVITATION))
	{ //Force jump is already active.. continue draining power appropriately until we land.
		if (pm->ps->fd.forcePowerDebounce_FP_LEVITATION_ < pm->cmd.serverTime)
		{
			BG_ForcePowerDrain( pm->ps, FP_LEVITATION, 5 );
			if (pm->ps->fd.forcePowerLevel_FP_LEVITATION_ >= FORCE_LEVEL_2)
			{
				pm->ps->fd.forcePowerDebounce_FP_LEVITATION_ = pm->cmd.serverTime + 300;
			}
			else
			{
				pm->ps->fd.forcePowerDebounce_FP_LEVITATION_ = pm->cmd.serverTime + 200;
			}
		}
	}

	if (pm->modParms.physics != MV_CHARGEJUMP && pm->ps->forceJumpFlip) // this is just for the charge jump. we're gonna set the anim in the charge jump place itself
	{ //Forced jump anim
		int anim = BOTH_FORCEINAIR1_GENERAL;
		int	parts = SETANIM_BOTH;

		if ( pm->cmd.forwardmove > 0 )
		{
			anim = BOTH_FLIP_F_GENERAL;
		}
		else if ( pm->cmd.forwardmove < 0 )
		{
			anim = BOTH_FLIP_B_GENERAL;
		}
		else if ( pm->cmd.rightmove > 0 )
		{
			anim = BOTH_FLIP_R_GENERAL;
		}
		else if ( pm->cmd.rightmove < 0 )
		{
			anim = BOTH_FLIP_L_GENERAL;
		}
		if ( pm->ps->weaponTime )
		{//FIXME: really only care if we're in a saber attack anim...
			parts = SETANIM_LEGS;
		}

		PM_SetAnim( parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150 );
		pm->ps->forceJumpFlip = qfalse;
		return qtrue;
	}

	if (pm->modParms.physics != MV_CHARGEJUMP) { // mirrors the old #if METROID_JUMP clause

		if ( pm->waterlevel < 2 ) 
		{
			if ( pm->ps->gravity > 0 )
			{//can't do this in zero-G
				if ( PM_ForceJumpingUp() )
				{//holding jump in air
					float curHeight = pm->ps->origin[2] - pm->ps->fd.forceJumpZStart;
					//check for max force jump level and cap off & cut z vel
					if ( ( curHeight<=forceJumpHeight[0] ||//still below minimum jump height
							(pm->ps->fd.forcePower&&pm->cmd.upmove>=10) ) &&////still have force power available and still trying to jump up 
						curHeight < forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_] &&
						(pm->ps->fd.forceJumpZStart || jk2gameplay != VERSION_1_04 && pm->modParms.physics != MV_JK2SP && !(pm->modParms.runFlags & RFL_JUMPBUGDISABLE)))//still below maximum jump height
					{//can still go up
						if ( curHeight > forceJumpHeight[0] )
						{//passed normal jump height  *2?
							if ( !(pm->ps->fd.forcePowersActive&(1<<FP_LEVITATION)) )//haven't started forcejump yet
							{
								//start force jump
								pm->ps->fd.forcePowersActive |= (1<<FP_LEVITATION);
#if !CLIENTSIDEONLY
								pm->ps->fd.forceJumpSound = 1;
#endif
								//play flip
								if ((pm->cmd.forwardmove || pm->cmd.rightmove) && //pushing in a dir
									(pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_FLIP_F_GENERAL &&//not already flipping
									(pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_FLIP_B_GENERAL &&
									(pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_FLIP_R_GENERAL &&
									(pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_FLIP_L_GENERAL )
								{ 
									int anim = BOTH_FORCEINAIR1_GENERAL;
									int	parts = SETANIM_BOTH;

									if ( pm->cmd.forwardmove > 0 )
									{
										anim = BOTH_FLIP_F_GENERAL;
									}
									else if ( pm->cmd.forwardmove < 0 )
									{
										anim = BOTH_FLIP_B_GENERAL;
									}
									else if ( pm->cmd.rightmove > 0 )
									{
										anim = BOTH_FLIP_R_GENERAL;
									}
									else if ( pm->cmd.rightmove < 0 )
									{
										anim = BOTH_FLIP_L_GENERAL;
									}
									if ( pm->ps->weaponTime )
									{
										parts = SETANIM_LEGS;
									}

									PM_SetAnim( parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150 );
								}
								else if ( pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_1 )
								{
									vec3_t facingFwd, facingRight, facingAngles;
									int	anim = -1;
									float dotR, dotF;
								
									VectorSet(facingAngles, 0, pm->ps->viewangles[YAW], 0);

									AngleVectors( facingAngles, facingFwd, facingRight, NULL );
									dotR = DotProduct( facingRight, pm->ps->velocity );
									dotF = DotProduct( facingFwd, pm->ps->velocity );

									if ( fabs(dotR) > fabs(dotF) * 1.5 )
									{
										if ( dotR > 150 )
										{
											anim = BOTH_FORCEJUMPRIGHT1_GENERAL;
										}
										else if ( dotR < -150 )
										{
											anim = BOTH_FORCEJUMPLEFT1_GENERAL;
										}
									}
									else
									{
										if ( dotF > 150 )
										{
											anim = BOTH_FORCEJUMP1_GENERAL;
										}
										else if ( dotF < -150 )
										{
											anim = BOTH_FORCEJUMPBACK1_GENERAL;
										}
									}
									if ( anim != -1 )
									{
										int parts = SETANIM_BOTH;
										if ( pm->ps->weaponTime )
										{//FIXME: really only care if we're in a saber attack anim...
											parts = SETANIM_LEGS;
										}

										PM_SetAnim( parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150 );
									}
								}
							}
							else
							{ //jump is already active (the anim has started)
								if ( pm->ps->legsTimer < 1 )
								{//not in the middle of a legsAnim
									int anim = (pm->ps->legsAnim&~ANIM_TOGGLEBIT);
									int newAnim = -1;
									switch ( anim )
									{
									case BOTH_FORCEJUMP1_GENERAL:
										newAnim = BOTH_FORCELAND1_GENERAL;//BOTH_FORCEINAIR1_GENERAL;
										break;
									case BOTH_FORCEJUMPBACK1_GENERAL:
										newAnim = BOTH_FORCELANDBACK1_GENERAL;//BOTH_FORCEINAIRBACK1_GENERAL;
										break;
									case BOTH_FORCEJUMPLEFT1_GENERAL:
										newAnim = BOTH_FORCELANDLEFT1_GENERAL;//BOTH_FORCEINAIRLEFT1_GENERAL;
										break;
									case BOTH_FORCEJUMPRIGHT1_GENERAL:
										newAnim = BOTH_FORCELANDRIGHT1_GENERAL;//BOTH_FORCEINAIRRIGHT1_GENERAL;
										break;
									}
									if ( newAnim != -1 )
									{
										int parts = SETANIM_BOTH;
										if ( pm->ps->weaponTime )
										{
											parts = SETANIM_LEGS;
										}

										PM_SetAnim( parts, newAnim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150 );
									}
								}
							}
						}

						//need to scale this down, start with height velocity (based on max force jump height) and scale down to regular jump vel
					
						if (MovementIsQuake3Based(pm->modParms.physics)) {//Forcejump rampjump
							//need to scale this down, start with height velocity (based on max force jump height) and scale down to regular jump vel
							float lastJumpSpeed = pm->ps->stats[STAT_LASTJUMPSPEED];
							float realForceJumpHeight;
							//if (lastJumpSpeed == 0) {
							//	lastJumpSpeed = JUMP_VELOCITY_NEW; // avoid infinite velocity[2] which results in NaN. why does this happen anyway?
							//}
							realForceJumpHeight = forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_] * (lastJumpSpeed / (float)JUMP_VELOCITY_NEW);

							if (!realForceJumpHeight || !lastJumpSpeed) {
								pm->ps->velocity[2] = 0; // can happen sometimes and messes everything up. this might feel weird if it happens but its a freak accident anyway. might be when pressing jump during spawn not sure.
								Com_Printf("^3realForceJumpHeight is 0, weird.\n");
							}
							else {
								pm->ps->velocity[2] = (realForceJumpHeight - curHeight) / realForceJumpHeight * forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];//JUMP_VELOCITY;
								pm->ps->velocity[2] /= 10;//need to scale this down, start with height velocity (based on max force jump height) and scale down to regular jump vel
								pm->ps->velocity[2] += pm->ps->stats[STAT_LASTJUMPSPEED];
							}
						}
						else {
							pm->ps->velocity[2] = (forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_]-curHeight)/forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_]*forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];//JUMP_VELOCITY;
							pm->ps->velocity[2] /= 10;
							pm->ps->velocity[2] += JUMP_VELOCITY_NEW;
						}
						pm->ps->pm_flags |= PMF_JUMP_HELD;
					}
					else if ( curHeight > forceJumpHeight[0] && curHeight < forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_] - forceJumpHeight[0] )
					{//still have some headroom, don't totally stop it
						if ( pm->ps->velocity[2] > JUMP_VELOCITY_NEW)
						{
							pm->ps->velocity[2] = JUMP_VELOCITY_NEW;
						}
					}
					else
					{
						if (pm->modParms.physics == MV_JK2SP) {
							pm->ps->velocity[2] = 0;
						}
						else {
							//pm->ps->velocity[2] = 0;
							//rww - changed for the sake of balance in multiplayer

							if (pm->ps->velocity[2] > JUMP_VELOCITY_NEW)
							{
								pm->ps->velocity[2] = JUMP_VELOCITY_NEW;
							}
						}
					}
					pm->cmd.upmove = 0;
					return qfalse;
				}
				else if ( jk2gameplay == VERSION_1_02 && pm->modParms.physics != MV_JK2SP && pm->ps->groundEntityNum == ENTITYNUM_NONE )
				{
					int legsAnim = (pm->ps->legsAnim&~ANIM_TOGGLEBIT);
					if ( legsAnim != BOTH_WALL_RUN_LEFT_GENERAL && legsAnim != BOTH_WALL_RUN_RIGHT_GENERAL )
					{//special case.. these let you jump off a wall
						if (pm->modParms.runFlags & RFL_CLIMBTECH) {
							// gotta allow for wallgrab
							onlyWallGrab = qtrue;
						}
						else {
							return qfalse;
						}
					}
				}

			}
		}

	}

	//Not jumping
	if ( pm->cmd.upmove < 10 && (pm->ps->groundEntityNum != ENTITYNUM_NONE || jk2gameplay == VERSION_1_02)) {
		return qfalse;
	}

	// must wait for jump to be released
	if ( pm->ps->pm_flags & PMF_JUMP_HELD ) 
	{
		// clear upmove so cmdscale doesn't lower running speed
		if(!onlyWallGrab){
			pm->cmd.upmove = 0;
		}
		return qfalse;
	}

	if ( pm->ps->gravity <= 0 )
	{//in low grav, you push in the dir you're facing as long as there is something behind you to shove off of
		vec3_t	forward, back;
		trace_t	trace;

		if(!onlyWallGrab){
			AngleVectors( pm->ps->viewangles, forward, NULL, NULL );
			VectorMA( pm->ps->origin, -8, forward, back );
			PM_Trace( &trace, pm->ps->origin, pm->mins, pm->maxs, back, pm->ps->clientNum, pm->tracemask );

			if ( trace.fraction <= 1.0f )
			{
				VectorMA( pm->ps->velocity, JUMP_VELOCITY_NEW*2, forward, pm->ps->velocity );
				PM_SetAnim(SETANIM_LEGS,BOTH_FORCEJUMP1_GENERAL,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD|SETANIM_FLAG_RESTART, 150);
			}//else no surf close enough to push off of
			pm->cmd.upmove = 0;
		}
	}
	else if ( pm->cmd.upmove > 0 && pm->waterlevel < 2 &&
		pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_0 &&
		!(pm->ps->pm_flags&PMF_JUMP_HELD) &&
		pm->ps->weapon == WP_SABER_GENERAL &&
		!BG_HasYsalamiri(pm->gametype, pm->ps) &&
		BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION) )
	{
		if ( pm->ps->groundEntityNum != ENTITYNUM_NONE )
		{//on the ground
			//check for left-wall and right-wall special jumps
			int anim = -1;
			float	vertPush = 0;
			if ( pm->cmd.rightmove > 0 && pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_1 )
			{//strafing right
				if ( pm->cmd.forwardmove > 0 )
				{//wall-run
					vertPush = forceJumpStrength[FORCE_LEVEL_2]/2.0f;
					anim = BOTH_WALL_RUN_RIGHT_GENERAL;
				}
				else if ( pm->cmd.forwardmove == 0 )
				{//wall-flip
					vertPush = forceJumpStrength[FORCE_LEVEL_2]/2.25f;
					anim = BOTH_WALL_FLIP_RIGHT_GENERAL;
				}
			}
			else if ( pm->cmd.rightmove < 0 && pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_1 )
			{//strafing left
				if ( pm->cmd.forwardmove > 0 )
				{//wall-run
					vertPush = forceJumpStrength[FORCE_LEVEL_2]/2.0f;
					anim = BOTH_WALL_RUN_LEFT_GENERAL;
				}
				else if ( pm->cmd.forwardmove == 0 )
				{//wall-flip
					vertPush = forceJumpStrength[FORCE_LEVEL_2]/2.25f;
					anim = BOTH_WALL_FLIP_LEFT_GENERAL;
				}
			}
			else if ( jk2gameplay == VERSION_1_02 && pm->cmd.forwardmove > 0 && pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_1 )
			{//run up wall, flip backwards
				vertPush = forceJumpStrength[FORCE_LEVEL_2]/2.25f;
				anim = BOTH_WALL_FLIP_BACK1_GENERAL;
			}
			else if ( pm->cmd.forwardmove < 0 && !(pm->cmd.buttons&BUTTON_ATTACK) )
			{//backflip
				vertPush = JUMP_VELOCITY_NEW;
				anim = BOTH_FLIP_BACK1_GENERAL;//PM_PickAnim( BOTH_FLIP_BACK1_GENERAL, BOTH_FLIP_BACK3_GENERAL );
			}

			vertPush += 128; //give them an extra shove

			if ( anim != -1 )
			{
				vec3_t fwd, right, traceto, mins, maxs, fwdAngles;
				vec3_t	idealNormal;
				trace_t	trace;
				qboolean doTrace = qfalse;
				int contents = MASK_PLAYERSOLID;

				VectorSet(mins, pm->mins[0],pm->mins[1],0);
				VectorSet(maxs, pm->maxs[0],pm->maxs[1],24);
				VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

				memset(&trace, 0, sizeof(trace)); //to shut the compiler up

				AngleVectors( fwdAngles, fwd, right, NULL );

				//trace-check for a wall, if necc.
				switch ( anim )
				{
				case BOTH_WALL_FLIP_LEFT_GENERAL:
					//NOTE: purposely falls through to next case!
				case BOTH_WALL_RUN_LEFT_GENERAL:
					doTrace = qtrue;
					VectorMA( pm->ps->origin, -16, right, traceto );
					break;

				case BOTH_WALL_FLIP_RIGHT_GENERAL:
					//NOTE: purposely falls through to next case!
				case BOTH_WALL_RUN_RIGHT_GENERAL:
					doTrace = qtrue;
					VectorMA( pm->ps->origin, 16, right, traceto );
					break;

				case BOTH_WALL_FLIP_BACK1_GENERAL:
					doTrace = qtrue;
					VectorMA( pm->ps->origin, 16, fwd, traceto );
					break;
				}

				if ( doTrace )
				{
					PM_Trace( &trace, pm->ps->origin, mins, maxs, traceto, pm->ps->clientNum, contents );
					VectorSubtract( pm->ps->origin, traceto, idealNormal );
					VectorNormalize( idealNormal );
				}

				if ( !doTrace || (trace.fraction < 1.0f && (trace.entityNum < MAX_CLIENTS || DotProduct(trace.plane.normal,idealNormal) > 0.7f)) )
				{//there is a wall there.. or hit a client
					int parts;
					//move me to side
					if ( anim == BOTH_WALL_FLIP_LEFT_GENERAL )
					{
						pm->ps->velocity[0] = pm->ps->velocity[1] = 0;
						VectorMA( pm->ps->velocity, 150, right, pm->ps->velocity );
					}
					else if ( anim == BOTH_WALL_FLIP_RIGHT_GENERAL )
					{
						pm->ps->velocity[0] = pm->ps->velocity[1] = 0;
						VectorMA( pm->ps->velocity, -150, right, pm->ps->velocity );
					}
					else if ( anim == BOTH_FLIP_BACK1_GENERAL 
						|| anim == BOTH_FLIP_BACK2_GENERAL 
						|| anim == BOTH_FLIP_BACK3_GENERAL 
						|| anim == BOTH_WALL_FLIP_BACK1_GENERAL )
					{
						pm->ps->velocity[0] = pm->ps->velocity[1] = 0;
						VectorMA( pm->ps->velocity, -150, fwd, pm->ps->velocity );
					}

					if ( doTrace && anim != BOTH_WALL_RUN_LEFT_GENERAL && anim != BOTH_WALL_RUN_RIGHT_GENERAL )
					{
						if (trace.entityNum < MAX_CLIENTS)
						{
#if !CLIENTSIDEONLY
							pm->ps->forceKickFlip = trace.entityNum+1; //let the server know that this person gets kicked by this client
#endif
						}
					}

					//up
					if ( vertPush )
					{
						pm->ps->velocity[2] = vertPush;
						pm->ps->fd.forcePowersActive |= (1 << FP_LEVITATION);
					}
					//animate me
					parts = SETANIM_LEGS;
					if ( anim == BOTH_BUTTERFLY_LEFT_GENERAL )
					{
						parts = SETANIM_BOTH;
						pm->cmd.buttons&=~BUTTON_ATTACK;
						pm->ps->saberMove = LS_NONE_GENERAL;
					}
					else if ( !pm->ps->weaponTime )
					{
						parts = SETANIM_BOTH;
					}
					PM_SetAnim( parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 0 );
					if ( anim == BOTH_BUTTERFLY_LEFT_GENERAL )
					{
						pm->ps->weaponTime = pm->ps->torsoTimer;
					}
					PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height
					pm->ps->pm_flags |= PMF_JUMP_HELD;
					pm->cmd.upmove = 0; 
#if !CLIENTSIDEONLY
					pm->ps->fd.forceJumpSound = 1;
#endif
				}
			}
		}
		else 
		{//in the air
			int legsAnim = (pm->ps->legsAnim&~ANIM_TOGGLEBIT);
			if ( legsAnim == BOTH_WALL_RUN_LEFT_GENERAL || legsAnim == BOTH_WALL_RUN_RIGHT_GENERAL )
			{//running on a wall
				vec3_t right, traceto, mins, maxs, fwdAngles;
				trace_t	trace;
				int		anim = -1;

				if (onlyWallGrab) return qfalse;

				VectorSet(mins, pm->mins[0], pm->mins[0], 0);
				VectorSet(maxs, pm->maxs[0], pm->maxs[0], 24);
				VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

				AngleVectors( fwdAngles, NULL, right, NULL );

				if ( legsAnim == BOTH_WALL_RUN_LEFT_GENERAL )
				{
					if ( pm->ps->legsTimer > 400 )
					{//not at the end of the anim
						float animLen = PM_AnimLength( 0, (animNumberGeneral_t)BOTH_WALL_RUN_LEFT_GENERAL );
						if ( pm->ps->legsTimer < animLen - 400 )
						{//not at start of anim
							VectorMA( pm->ps->origin, -16, right, traceto );
							anim = BOTH_WALL_RUN_LEFT_FLIP_GENERAL;
						}
					}
				}
				else if ( legsAnim == BOTH_WALL_RUN_RIGHT_GENERAL )
				{
					if ( pm->ps->legsTimer > 400 )
					{//not at the end of the anim
						float animLen = PM_AnimLength( 0, (animNumberGeneral_t)BOTH_WALL_RUN_RIGHT_GENERAL );
						if ( pm->ps->legsTimer < animLen - 400 )
						{//not at start of anim
							VectorMA( pm->ps->origin, 16, right, traceto );
							anim = BOTH_WALL_RUN_RIGHT_FLIP_GENERAL;
						}
					}
				}
				if ( anim != -1 )
				{
					PM_Trace( &trace, pm->ps->origin, mins, maxs, traceto, pm->ps->clientNum, CONTENTS_SOLID|CONTENTS_BODY );
					if ( trace.fraction < 1.0f )
					{//flip off wall
						int parts = 0;

						if ( anim == BOTH_WALL_RUN_LEFT_FLIP_GENERAL )
						{
							pm->ps->velocity[0] *= 0.5f;
							pm->ps->velocity[1] *= 0.5f;
							VectorMA( pm->ps->velocity, 150, right, pm->ps->velocity );
						}
						else if ( anim == BOTH_WALL_RUN_RIGHT_FLIP_GENERAL )
						{
							pm->ps->velocity[0] *= 0.5f;
							pm->ps->velocity[1] *= 0.5f;
							VectorMA( pm->ps->velocity, -150, right, pm->ps->velocity );
						}
						parts = SETANIM_LEGS;
						if ( !pm->ps->weaponTime )
						{
							parts = SETANIM_BOTH;
						}
						PM_SetAnim( parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 0 );
						pm->cmd.upmove = 0;
					}
				}
				if ( pm->cmd.upmove != 0 )
				{//jump failed, so don't try to do normal jump code, just return
					return qfalse;
				}
			}
			else if ( jk2gameplay != VERSION_1_02 &&
				pm->cmd.forwardmove > 0 //pushing forward
				&& pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_1
				&& pm->ps->velocity[2] > 200
				&& PM_GroundDistance() <= 80 //unfortunately we do not have a happy ground timer like SP (this would use up more bandwidth if we wanted prediction workign right), so we'll just use the actual ground distance.
				&& !BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) )
			{//run up wall, flip backwards
				vec3_t fwd, traceto, mins, maxs, fwdAngles;
				trace_t	trace;
				vec3_t	idealNormal;

				if (onlyWallGrab) return qfalse;

				VectorSet(mins, pm->mins[0],pm->mins[1],pm->mins[2]);
				VectorSet(maxs, pm->maxs[0],pm->maxs[1],pm->maxs[2]);
				VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

				AngleVectors( fwdAngles, fwd, NULL, NULL );
				VectorMA( pm->ps->origin, 32, fwd, traceto );

				PM_Trace( &trace, pm->ps->origin, mins, maxs, traceto, pm->ps->clientNum, MASK_PLAYERSOLID );//FIXME: clip brushes too?
				VectorSubtract( pm->ps->origin, traceto, idealNormal );
				VectorNormalize( idealNormal );
				
				if ( trace.fraction < 1.0f )
				{//there is a wall there
					int parts = SETANIM_LEGS;

					pm->ps->velocity[0] = pm->ps->velocity[1] = 0;
					VectorMA( pm->ps->velocity, -150, fwd, pm->ps->velocity );
					pm->ps->velocity[2] += 128;

					if ( !pm->ps->weaponTime )
					{
						parts = SETANIM_BOTH;
					}
					PM_SetAnim( parts, BOTH_WALL_FLIP_BACK1_GENERAL, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 0 );

					pm->ps->legsTimer -= 600; //I force this anim to play to the end to prevent landing on your head and suddenly flipping over.
											  //It is a bit too long at the end though, so I'll just shorten it.

					PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height
					pm->cmd.upmove = 0;
#if !CLIENTSIDEONLY
					pm->ps->fd.forceJumpSound = 1;
#endif
					BG_ForcePowerDrain( pm->ps, FP_LEVITATION, 5 );

					if (trace.entityNum < MAX_CLIENTS)
					{
#if !CLIENTSIDEONLY
						pm->ps->forceKickFlip = trace.entityNum+1; //let the server know that this person gets kicked by this client
#endif
					}
				}
			} 
			else if ( (pm->modParms.runFlags & RFL_CLIMBTECH) &&
					(!BG_InSpecialJump( legsAnim , pm->modParms.runFlags)//not in a special jump anim
						||BG_InReboundJump( legsAnim )//we're already in a rebound
						||BG_InBackFlip( legsAnim ) )//a backflip (needed so you can jump off a wall behind you)
					//&& pm->ps->velocity[2] <= 0
					&& pm->ps->velocity[2] > -1200 //not falling down very fast
					&& !(pm->ps->pm_flags&PMF_JUMP_HELD)//have to have released jump since last press
					&& (pm->cmd.forwardmove||pm->cmd.rightmove)//pushing in a direction
					//&& pm->ps->forceRageRecoveryTime < pm->cmd.serverTime	//not in a force Rage recovery period
					&& pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_2//level 3 jump or better
					//&& WP_ForcePowerAvailable_GENERAL( pm->gent, FP_LEVITATION, 10 )//have enough force power to do another one
					&& BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION)
					&& (pm->ps->origin[2]-pm->ps->fd.forceJumpZStart) < (forceJumpHeightMax[FORCE_LEVEL_3]-(BG_ForceWallJumpStrength()/2.0f)) //can fit at least one more wall jump in (yes, using "magic numbers"... for now)
					//&& (pm->ps->legsAnim == BOTH_JUMP1_GENERAL || pm->ps->legsAnim == BOTH_INAIR1_GENERAL ) )//not in a flip or spin or anything
					)
			{//see if we're pushing at a wall and jump off it if so
				//if ( allowWallGrabs )
				if ( qtrue )
				{
					//FIXME: make sure we have enough force power
					//FIXME: check  to see if we can go any higher
					//FIXME: limit to a certain number of these in a row?
					//FIXME: maybe don't require a ucmd direction, just check all 4?
					//FIXME: should stick to the wall for a second, then push off...
					vec3_t checkDir, traceto, mins, maxs, fwdAngles;
					trace_t	trace;
					vec3_t	idealNormal;
					int		anim = -1;

					VectorSet(mins, pm->mins[0], pm->mins[1], 0.0f);
					VectorSet(maxs, pm->maxs[0], pm->maxs[1], 24.0f);
					VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0.0f);

					if ( pm->cmd.rightmove )
					{
						if ( pm->cmd.rightmove > 0 )
						{
							anim = BOTH_FORCEWALLREBOUND_RIGHT_GENERAL;
							AngleVectors( fwdAngles, NULL, checkDir, NULL );
						}
						else if ( pm->cmd.rightmove < 0 )
						{
							anim = BOTH_FORCEWALLREBOUND_LEFT_GENERAL;
							AngleVectors( fwdAngles, NULL, checkDir, NULL );
							VectorScale( checkDir, -1, checkDir );
						}
					}
					else if ( pm->cmd.forwardmove > 0 )
					{
						anim = BOTH_FORCEWALLREBOUND_FORWARD_GENERAL;
						AngleVectors( fwdAngles, checkDir, NULL, NULL );
					}
					else if ( pm->cmd.forwardmove < 0 )
					{
						anim = BOTH_FORCEWALLREBOUND_BACK_GENERAL;
						AngleVectors( fwdAngles, checkDir, NULL, NULL );
						VectorScale( checkDir, -1, checkDir );
					}
					if ( anim != -1 )
					{//trace in the dir we're pushing in and see if there's a vertical wall there
						bgEntity_t *traceEnt;

						VectorMA( pm->ps->origin, 8, checkDir, traceto );
						PM_Trace( &trace, pm->ps->origin, mins, maxs, traceto, pm->ps->clientNum, CONTENTS_SOLID );//FIXME: clip brushes too?
						VectorSubtract( pm->ps->origin, traceto, idealNormal );
						VectorNormalize( idealNormal );
						traceEnt = PM_BGEntForNum(trace.entityNum);
						if ( trace.fraction < 1.0f
							&&fabs(trace.plane.normal[2]) <= 0.2f/*MAX_WALL_GRAB_SLOPE*/
							&&((trace.entityNum<ENTITYNUM_WORLD&&traceEnt&&traceEnt->s.solid!=SOLID_BMODEL)||DotProduct(trace.plane.normal,idealNormal)>0.7f) )
						{//there is a wall there
							float dot = DotProduct( pm->ps->velocity, trace.plane.normal );
							if ( dot < 1.0f )
							{//can't be heading *away* from the wall!
								//grab it!
								PM_GrabWallForJump( anim );
							}
						}
					}
				}
			}
		}
	}

	if ( !onlyWallGrab
		&& pm->cmd.upmove > 0 
		&& pm->ps->weapon == WP_SABER_GENERAL
		&& (pm->ps->weaponTime > 0||pm->cmd.buttons&BUTTON_ATTACK) )
	{//okay, we just jumped and we're in an attack
		if ( !BG_InRoll( pm->ps, pm->ps->legsAnim )
			&& !PM_InKnockDown( pm->ps )
			&& !BG_InDeathAnim(pm->ps->legsAnim)
			&& !BG_FlippingAnim( pm->ps->legsAnim )
			&& !PM_SpinningAnim( pm->ps->legsAnim )
			&& !BG_SaberInSpecialAttack( pm->ps->torsoAnim )
			&& ( BG_SaberInAttack( pm->ps->saberMove ) ) )
		{//not in an anim we shouldn't interrupt
			//see if it's not too late to start a special jump-attack
			float animLength = PM_AnimLength( 0, (animNumberGeneral_t)pm->ps->torsoAnim );
			if ( animLength - pm->ps->torsoTimer < 500 )
			{//just started the saberMove
				//check for special-case jump attacks

				if ( pm->ps->fd.saberAnimLevel == FORCE_LEVEL_2 )
				{//using medium attacks
					if (PM_GroundDistance() < 32 &&
						!BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags))
					{ //FLIP AND DOWNWARD ATTACK
						trace_t tr;

						if (PM_SomeoneInFront(&tr))
						{
							PM_SetSaberMove(PM_SaberFlipOverAttackMove(&tr));
							pml.groundPlane = qfalse;
							pml.walking = qfalse;
							pm->ps->pm_flags |= PMF_JUMP_HELD;
							PM_SetGroundEntityNum(ENTITYNUM_NONE);
							VectorClear(pml.groundTrace.plane.normal);

							pm->ps->weaponTime = pm->ps->torsoTimer;
						}
					}
				}
				else if ( pm->ps->fd.saberAnimLevel == FORCE_LEVEL_3 )
				{//using strong attacks
					if ( //!(runFlags & RFL_CLIMBTECH) && // using JKA dfa instead then?
						pm->cmd.forwardmove > 0 && //going forward
						((pm->cmd.buttons & BUTTON_ATTACK) || jk2gameplay == VERSION_1_02) && //must be holding attack still
						PM_GroundDistance() < 32 &&
						!BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags))
					{//strong attack: jump-hack
						PM_SetSaberMove( PM_SaberJumpAttackMove() );
						pml.groundPlane = qfalse;
						pml.walking = qfalse;
						pm->ps->pm_flags |= PMF_JUMP_HELD;
						PM_SetGroundEntityNum(ENTITYNUM_NONE);
						VectorClear(pml.groundTrace.plane.normal);

						pm->ps->weaponTime = pm->ps->torsoTimer;
					}
				}
			}
		}
	}
	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE )
	{
		return qfalse;
	}
	if ( pm->cmd.upmove > 0 )
	{//no special jumps
		if (MovementIsQuake3Based(pm->modParms.physics)) {
			// TODO flood protect jumps? idk
			pm->ps->velocity[2] += JUMP_VELOCITY_NEW;
			if (pm->ps->velocity[2] < 270)
				pm->ps->velocity[2] = 270;
			pm->ps->stats[STAT_LASTJUMPSPEED] = pm->ps->velocity[2];
		}
		else {
			pm->ps->velocity[2] = JUMP_VELOCITY_NEW;
		}
		PM_SetForceJumpZStart(pm->ps->origin[2]);//so we don't take damage if we land at same height
		pm->ps->pm_flags |= PMF_JUMP_HELD;
	}

	//Jumping
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
	pm->ps->pm_flags |= PMF_JUMP_HELD;
	PM_SetGroundEntityNum(ENTITYNUM_NONE);
	PM_SetForceJumpZStart(pm->ps->origin[2]);

	PM_AddEvent( EV_JUMP_GENERAL );

	//Set the animations
	if ( pm->ps->gravity > 0 && !BG_InSpecialJump( pm->ps->legsAnim, pm->modParms.runFlags) )
	{
		PM_JumpForDir();
	}

	return qtrue;
}
/*
=============
PM_CheckWaterJump
=============
*/
 qboolean	 PMove::PM_CheckWaterJump( void ) {
	vec3_t	spot;
	int		cont;
	vec3_t	flatforward;

	if (pm->ps->pm_time) {
		return qfalse;
	}

	// check for water jump
	if ( pm->waterlevel != 2 ) {
		return qfalse;
	}

	flatforward[0] = pml.forward[0];
	flatforward[1] = pml.forward[1];
	flatforward[2] = 0;
	VectorNormalize (flatforward);

	VectorMA (pm->ps->origin, 30, flatforward, spot);
	spot[2] += 4;
	cont = PM_PointContents(spot, pm->ps->clientNum );
	if ( !(cont & CONTENTS_SOLID) ) {
		return qfalse;
	}

	spot[2] += 16;
	cont = PM_PointContents(spot, pm->ps->clientNum );
	if ( cont ) {
		return qfalse;
	}

	// jump out of water
	VectorScale (pml.forward, 200, pm->ps->velocity);
	pm->ps->velocity[2] = 350;

	pm->ps->pm_flags |= PMF_TIME_WATERJUMP;
	pm->ps->pm_time = 2000;

	return qtrue;
}

//============================================================================


#if CLIENTSIDEONLY
int GetRealForcePower(playerState_t* ps, forcePowers_t forcePower) {

	int drain = 0;
	switch (forcePower) {
	default:
		return NONETWORK_FORCEPOWERLEVEL(ps,forcePower);
	case FP_SEE:
		return ps->fd.forcePowerLevel_FP_SEE_;
	case FP_LEVITATION:
		return ps->fd.forcePowerLevel_FP_LEVITATION_;
	}
}
#endif

/*
* 
* CHARGE JUMP MECHANICS
* Have some extra vars we need to network via stats entities: ps.groundTime, ps->fd->forcejumpcharge
* Using new PMF_FJDIDJUMP
* 
*/

qboolean  PMove::PM_ForcePowerAvailable(forcePowers_t forcePower)
{
#if CLIENTSIDEONLY
	int	drain = forcePowerNeeded[GetRealForcePower(pm->ps,forcePower)][forcePower];
#else
	int	drain = forcePowerNeeded[pm->ps->fd.forcePowerLevel[forcePower]][forcePower];
#endif

	if (pm->ps->fd.forcePowersActive & (1 << forcePower))
	{ //we're probably going to deactivate it..
		return qtrue;
	}

	if (forcePower == FP_LEVITATION)
	{
		return qtrue;
	}
	if (!drain)
	{
		return qtrue;
	}
	if (pm->ps->fd.forcePower < drain)
	{
		return qfalse;
	}
	return qtrue;
}

qboolean  PMove::PM_ForcePowerUsable(forcePowers_t forcePower)
{

	if (BG_HasYsalamiri(pm->gametype, pm->ps))
	{
		return qfalse;
	}

	//if (pm->ps->pm_flags & PMF_FOLLOW)
	//{ //specs can't use powers through people
	//	return qfalse;
	//}
	//if (self->client->sess.sessionTeam == TEAM_SPECTATOR)
	//{
	//	return qfalse;
	//}

	if (!BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, forcePower))
	{
		return qfalse;
	}

	if (!(pm->ps->fd.forcePowersKnown & (1 << forcePower)))
	{//don't know this power
		return qfalse;
	}

	if ((pm->ps->fd.forcePowersActive & (1 << forcePower)))
	{//already using this power
		if (forcePower != FP_LEVITATION)
		{
			return qfalse;
		}
	}

	if (forcePower == FP_LEVITATION && (pm->modParms.physics == MV_CHARGEJUMP && (pm->ps->pm_flags & PMF_FJDIDJUMP)))
	{
		return qfalse;
	}

#if CLIENTSIDEONLY
	if (!GetRealForcePower(pm->ps, forcePower))
#else
	if (!pm->ps->fd.forcePowerLevel[forcePower])
#endif
	{
		return qfalse;
	}

	return PM_ForcePowerAvailable(forcePower);
}

#define PM_FORCE_JUMP_CHARGE_TIME 1000.0f
//#define PM_FORCE_JUMP_CHARGE_TIME_SEGMENTSLEGACY (PM_FORCE_JUMP_CHARGE_TIME/100.0f)
void PMove::PM_ForceJumpCharge()
{
	float baseJumpStrength = forceJumpStrength[0];
	float jumpStrengthChargeSpeedBase = forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];
	float forceJumpChargeInterval = (jumpStrengthChargeSpeedBase- baseJumpStrength) * pml.frametime * 1000.0f / PM_FORCE_JUMP_CHARGE_TIME;

	if (pm->ps->pm_type == PM_DEAD)
	{
		return;
	}

	if (pm->ps->fd.forcePowerLevel_FP_LEVITATION_ <= 0) {
		return;
	}

	//if (!pm->ps->fd.forceJumpCharge && pm->ps->groundEntityNum == ENTITYNUM_NONE) // TA I think the original version intended for charge to be possible in air
	//{
	//	return;
	//}

	if (pm->ps->fd.forcePower < forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION])
	{
#if JK2_GAME
		G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE); 
#endif
		return;
	}

	//if (!pm->ps->fd.forceJumpCharge)
	//{
	//	pm->ps->fd.forceJumpAddTime = 0;
	//}

	//if (pm->ps->fd.forceJumpAddTime >= level.time)
	//{
	//	return;
	//}

	//need to play sound
	if (!pm->ps->fd.forceJumpCharge || !(pm->ps->stats[STAT_CHARGEJUMPDATA] & CHARGEJUMPFLAG_CHARGING)) // if we interrupt our charging, restart.
	{
#if JK2_GAME
		G_Sound(g_entities+pm->ps->clientNum, TRACK_CHANNEL_1, G_SoundIndex("sound/weapons/force/jumpbuild.wav")); 
#endif
		pm->ps->fd.forceJumpCharge = baseJumpStrength; // always keep this as the basis
	}

	pm->ps->stats[STAT_CHARGEJUMPDATA] |= CHARGEJUMPFLAG_CHARGING;

	//Increment
	//if (pm->ps->fd.forceJumpAddTime < pm->cmd.serverTime)
	{
		pm->ps->fd.forceJumpCharge += forceJumpChargeInterval;// *50;// TA: wtf no, why times 50!?
		//pm->ps->fd.forceJumpAddTime = level.time + 500;
	}

	//clamp to max strength for current level
	if (pm->ps->fd.forceJumpCharge > forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_])
	{
		pm->ps->fd.forceJumpCharge = forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];
		//G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE); // TA restore this somehow idk
	}


	//clamp to max available force power
	if (pm->ps->fd.forceJumpCharge / baseJumpStrength * forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION] > pm->ps->fd.forcePower)
	{//can't use more than you have
#if JK2_GAME
		G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
#endif
		pm->ps->fd.forceJumpCharge = baseJumpStrength * pm->ps->fd.forcePower / forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION];
	}

	//G_Printf("%f\n", self->client->ps.fd.forceJumpCharge);
}
typedef enum
{
	PM_FJ_FORWARD,
	PM_FJ_BACKWARD,
	PM_FJ_RIGHT,
	PM_FJ_LEFT,
	PM_FJ_UP
};

int  PMove::PM_GetVelocityForForceJump( vec3_t jumpVel)
{
	float pushFwd = 0, pushRt = 0;
	vec3_t	view, forward, right;
	static const float sideAmt = 70.710678118654752440084436210485f;//100.0f*sqrtf(0.5f); // TA: it does 50 in vanilla but eh, then the WA/WD behavior isnt consistent with A/D behavior. sucks.
	float maxCharge = forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];
	float baseCharge = forceJumpStrength[0];
	float chargePercent;

	if (pm->ps->fd.forcePowerLevel_FP_LEVITATION_<= 0) {
		VectorCopy(pm->ps->velocity,jumpVel);
		return PM_FJ_UP;
	}

	VectorCopy(pm->ps->viewangles, view);
	view[0] = 0;
	AngleVectors(view, forward, right, NULL);
	if (pm->cmd.forwardmove && pm->cmd.rightmove)
	{
		if (pm->cmd.forwardmove > 0)
		{
			pushFwd = sideAmt;
		}
		else
		{
			pushFwd = -sideAmt;
		}
		if (pm->cmd.rightmove > 0)
		{
			pushRt = sideAmt;
		}
		else
		{
			pushRt = -sideAmt;
		}
	}
	else if (pm->cmd.forwardmove || pm->cmd.rightmove)
	{
		if (pm->cmd.forwardmove > 0)
		{
			pushFwd = 100;
		}
		else if (pm->cmd.forwardmove < 0)
		{
			pushFwd = -100;
		}
		else if (pm->cmd.rightmove > 0)
		{
			pushRt = 100;
		}
		else if (pm->cmd.rightmove < 0)
		{
			pushRt = -100;
		}
	}

#if JK2_GAME
	G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);

	G_Sound(g_entities+pm->ps->clientNum, CHAN_AUTO, G_SoundIndex("sound/weapons/force/jump.wav"));
#endif

	// TA: We are racing. Give us detailed stepless control.
	//if (pm->ps->fd.forceJumpCharge < JUMP_VELOCITY + 40)
	//{ //give him at least a tiny boost from just a tap
	//	pm->ps->fd.forceJumpCharge = JUMP_VELOCITY + 40;
	//}

	if (pm->ps->velocity[2] < -30)
	{ //so that we can get a good boost when force jumping in a fall
		pm->ps->velocity[2] = -30;
	}

	VectorMA(pm->ps->velocity, pushFwd, forward, jumpVel);
	if (pm->modParms.physics == MV_CHARGEJUMP) {
		// i think this was the intended behavior.
		VectorMA(jumpVel, pushRt, right, jumpVel);
	}
	else {
		VectorMA(pm->ps->velocity, pushRt, right, jumpVel);
	}
	jumpVel[2] += pm->ps->fd.forceJumpCharge;//forceJumpStrength;
	chargePercent = (pm->ps->fd.forceJumpCharge - baseCharge)/(maxCharge - baseCharge);
	if (pushFwd > 0 && chargePercent > 0.5f)// && pm->ps->fd.forceJumpCharge > 200) // TA changed this so there is a bit of variability.
	{
		return PM_FJ_FORWARD;
	}
	else if (pushFwd < 0 && chargePercent > 0.5f)//  && pm->ps->fd.forceJumpCharge > 200)
	{
		return PM_FJ_BACKWARD;
	}
	else if (pushRt > 0 && chargePercent > 0.5f)//  && pm->ps->fd.forceJumpCharge > 200)
	{
		return PM_FJ_RIGHT;
	}
	else if (pushRt < 0 && chargePercent > 0.5f)//  && pm->ps->fd.forceJumpCharge > 200)
	{
		return PM_FJ_LEFT;
	}
	else
	{//FIXME: jump straight up anim
		return PM_FJ_UP;
	}
}

#if JK2_GAME
void  PMove::WP_ForcePowerStart_GENERAL(gentity_t* self, forcePowers_t forcePower, int overrideAmt);
#endif

void PMove::PM_ChargeForceJump()
{
	float jumpStrengthChargeSpeedBase;
	float forceJumpChargeInterval;
	float forceDeduction;
	int anim = BOTH_FORCEINAIR1_GENERAL;
	int	parts = SETANIM_BOTH;
	vec3_t	jumpVel; 
	float baseJumpStrength = forceJumpStrength[0];
	//	int	parts = SETANIM_BOTH;

	if (pm->ps->fd.forcePowerDuration_FP_LEVITATION_ > pm->cmd.serverTime)
	{
		return;
	}
	if (pm->ps->fd.forcePowerLevel_FP_LEVITATION_ <= 0) {
		return;
	}
	if (BG_HasYsalamiri(pm->gametype, pm->ps)) {
		return;
	}
	if (!BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION)) {
		return;
	}
	if (!PM_ForcePowerUsable(FP_LEVITATION))
	{
		return;
	}
	//if (self->s.groundEntityNum == ENTITYNUM_NONE)
	if (pm->ps->groundEntityNum == ENTITYNUM_NONE)
	{
		return;
	}
	//	if ( pm->ps->pm_flags&PMF_JUMP_HELD )
	//	{
	//		return;
	//	}
	//if (self->health <= 0) // should already be excluded at this point
	//{
	//	return;
	//}

	//G_SoundOnEnt( self, CHAN_BODY, "sound/weapons/force/jump.wav" );
	//play sound here

	//self->client->fjDidJump = qtrue;
	pm->ps->pm_flags |= PMF_FJDIDJUMP;

	//forceJumpChargeInterval = forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_] / PM_FORCE_JUMP_CHARGE_TIME_SEGMENTSLEGACY;
	jumpStrengthChargeSpeedBase = forceJumpStrength[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];
	forceJumpChargeInterval = (jumpStrengthChargeSpeedBase- baseJumpStrength) * pml.frametime * 1000.0f / PM_FORCE_JUMP_CHARGE_TIME;

	switch (PM_GetVelocityForForceJump(jumpVel))
	{
	case PM_FJ_FORWARD:
		anim = BOTH_FLIP_F_GENERAL;
				//dmEvent = DM_FLIP;
		break;
	case PM_FJ_BACKWARD:
		anim = BOTH_FLIP_B_GENERAL;
				//dmEvent = DM_FLIP;
		break;
	case PM_FJ_RIGHT:
		anim = BOTH_FLIP_R_GENERAL;
				//dmEvent = DM_FLIP;
		break;
	case PM_FJ_LEFT:
		anim = BOTH_FLIP_L_GENERAL;
				//dmEvent = DM_FLIP;
		break;
	default:
	case PM_FJ_UP:
		anim = BOTH_JUMP1_GENERAL;
				//dmEvent = DM_JUMP;
		break;
	}

	if (pm->ps->weaponTime)
	{//FIXME: really only care if we're in a saber attack anim.. maybe trail length?
		parts = SETANIM_LEGS;
	}
	
	PM_SetAnim(parts, anim, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD, 150);
	//NPC_SetAnim( self, parts, anim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD );
	//if (!self->s.number)
	//{
		//G_DynaMixEvent( dmEvent );
	//}

	//FIXME: sound effect
	PM_SetForceJumpZStart(pm->ps->origin[2]);
	//pm->ps->fd.forceJumpZStart = pm->ps->origin[2];//remember this for when we land
	VectorCopy(jumpVel, pm->ps->velocity);
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	//wasn't allowing them to attack when jumping, but that was annoying
	//pm->ps->weaponTime = pm->ps->torsoAnimTimer;

	//forceDeduction = pm->ps->fd.forceJumpCharge / forceJumpChargeInterval / PM_FORCE_JUMP_CHARGE_TIME_SEGMENTSLEGACY * forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION];
	//forceDeduction = (pm->ps->fd.forceJumpCharge - baseJumpStrength) / (jumpStrengthChargeSpeedBase - baseJumpStrength) * forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION];
	forceDeduction = pm->ps->fd.forceJumpCharge / baseJumpStrength * forcePowerNeeded[pm->ps->fd.forcePowerLevel_FP_LEVITATION_][FP_LEVITATION];

#if JK2_GAME
	WP_ForcePowerStart_GENERAL(g_entities+pm->ps->clientNum, FP_LEVITATION, forceDeduction);
#else
	pm->ps->fd.forcePowersActive |= (1 << FP_LEVITATION);
	BG_ForcePowerDrain(pm->ps, FP_LEVITATION, forceDeduction);
	pm->ps->fd.forcePowerDebounce_FP_LEVITATION_ = 0;
	pm->ps->fd.forcePowerDuration_FP_LEVITATION_ = 0;
#endif
	//pm->ps->fd.forcePowerDuration[FP_LEVITATION] = pm->cmd.serverTime + pm->ps->weaponTime;
	pm->ps->fd.forceJumpCharge = 0;
	pm->ps->forceJumpFlip = qtrue;
}

/*
===================
PM_CheckChargeJump

Was in w_force but unused and rly should be predicted...
===================
*/
void PMove::PM_CheckChargeJump( void ) {
	qboolean usingForce = qfalse;
	qboolean buttonPressed = (qboolean)((pm->cmd.buttons & BUTTON_FORCEPOWER) &&
		pm->ps->fd.forcePowerSelected == FP_LEVITATION || (pm->cmd.buttons & BUTTON_BOUNCEPOWER));

	if (pm->ps->groundEntityNum != ENTITYNUM_NONE)
	{
		pm->ps->pm_flags &= ~PMF_FJDIDJUMP;
		//self->client->fjDidJump = qfalse;
	}

	// feels bad. just clear when we land without anything pressed
	/*if (pm->ps->fd.forceJumpCharge && pm->ps->groundEntityNum == ENTITYNUM_NONE && (pm->ps->pm_flags & PMF_FJDIDJUMP))
	{
		if (pm->cmd.upmove < 10 && !(pm->cmd.buttons & BUTTON_BOUNCEPOWER) && (!(pm->cmd.buttons & BUTTON_FORCEPOWER) || pm->ps->fd.forcePowerSelected != FP_LEVITATION))
		{
#if JK2_GAME
			G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
#endif
 			pm->ps->fd.forceJumpCharge = 0;
		}
	}*/

	if (!buttonPressed) { // if no longer pressing this
		pm->ps->stats[STAT_CHARGEJUMPDATA] &= ~CHARGEJUMPFLAG_CHARGING;
	}

	if ( /*!self->client->fjDidJump &&*/ buttonPressed && !BG_HasYsalamiri(pm->gametype, pm->ps) && BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION))
	{//just charging up
		PM_ForceJumpCharge();
		usingForce = qtrue;
	}
//#ifndef METROID_JUMP
#if 0 // nvm this feels weird and needs a whole extra var to network
	else if ( /*!self->client->fjDidJump &&*/ (pm->cmd.upmove > 10) && (pm->ps->pm_flags & PMF_JUMP_HELD) && pm->ps->groundTime && (pm->cmd.serverTime - pm->ps->groundTime) > 150 && !BG_HasYsalamiri(pm->gametype, pm->ps) && BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION)/*&& !pm->ps->fd.forceJumpZStart*/)
	{//just charging up

		// meh doesnt feel great/hardly noticable/weird
		//PM_ForceJumpCharge();
		//usingForce = qtrue;
	}
#endif
	else if (pm->cmd.upmove < 10 && pm->ps->groundEntityNum == ENTITYNUM_NONE && pm->ps->fd.forceJumpCharge)
	{
		pm->ps->pm_flags &= ~(PMF_JUMP_HELD);
	}
//#endif


	if (!buttonPressed && !(pm->ps->pm_flags & PMF_JUMP_HELD) && pm->ps->fd.forceJumpCharge)
	{


		//if (!(pm->cmd.buttons & BUTTON_FORCEPOWER) ||
		//	pm->ps->fd.forcePowerSelected != FP_LEVITATION)
		{
			if (pm->ps->groundEntityNum == ENTITYNUM_NONE)
			{
				/* TA: Actually nvm this always feels bad pretty much
				if (pm->cmd.upmove < 10) { //  TA keep the charge if we are still keeping jump pressed, perhaps for the following jump
					pm->ps->fd.forceJumpCharge = 0;
#if JK2_GAME
					G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
#endif
				}*/
				//This only happens if the groundEntityNum == ENTITYNUM_NONE when the button is actually released
			}
			else
			{//still on ground, so jump
				if (pm->cmd.upmove > 10) { // allow us to do delayed jumps and cool things like that.
					PM_ChargeForceJump();
				}
			}
			//if (WP_DoSpecificPower_GENERAL(self, &pm->cmd, FP_LEVITATION))
			//{
				//usingForce = qtrue;
			//}
		}
	}
}
/*
===================
PM_WaterJumpMove

Flying out of the water
===================
*/
 void  PMove::PM_WaterJumpMove( void ) {
	// waterjump has no control, but falls

	PM_StepSlideMove( qtrue );
	PM_UpdateAntiLoop();

	pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	if (pm->ps->velocity[2] < 0) {
		// cancel as soon as we are falling down again
		pm->ps->pm_flags &= ~PMF_ALL_TIMES;
		pm->ps->pm_time = 0;
	}
}

/*
===================
PM_WaterMove

===================
*/
 void  PMove::PM_WaterMove( void ) {
	int		i;
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;
	float	vel;
	float	realWaterAccelerate = pm_wateraccelerate;
	float	realSwimScale = pm_swimScale;


	if ( PM_CheckWaterJump() ) {
		PM_WaterJumpMove();
		return;
	}
#if 0
	// jump = head for surface
	if ( pm->cmd.upmove >= 10 ) {
		if (pm->ps->velocity[2] > -300) {
			if ( pm->watertype == CONTENTS_WATER ) {
				pm->ps->velocity[2] = 100;
			} else if (pm->watertype == CONTENTS_SLIME) {
				pm->ps->velocity[2] = 80;
			} else {
				pm->ps->velocity[2] = 50;
			}
		}
	}
#endif
	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );
	//
	// user intentions
	//
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = -60;		// sink towards bottom
	} else {
		for (i=0 ; i<3 ; i++)
			wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if (MovementIsQuake3Based(pm->modParms.physics)) {
		// just feels better
		realWaterAccelerate = 10.0f;
		realSwimScale = 0.75f; 
	}

	if ( wishspeed > pm->ps->speed * realSwimScale) {
		wishspeed = pm->ps->speed * realSwimScale;
	}

	if (pm->modParms.physics == MV_SICKO) {
		PM_SickoAccelerate(wishdir, wishspeed, realWaterAccelerate, 200.0f);
	}
	else if (pm->modParms.physics == MV_QUAJK) {
		float		accel;

		if (DotProduct(pm->ps->velocity, wishdir) < 0)
			accel = pm_cpm_airstopaccelerate;
		else
			accel = realWaterAccelerate;
		PM_QuaJKAccelerate(wishdir, wishspeed, accel, pm_cpm_airstrafeaccelerate, 30.0f);
	}
	else if (pm->modParms.physics == MV_DREAM) {
		PM_DreamAccelerate(wishdir, wishspeed, realWaterAccelerate, 100, 200.0f);
	}
	else {
		PM_Accelerate(wishdir, wishspeed, realWaterAccelerate);
	}
	PM_UpdateAntiLoop();

	// make sure we can go up slopes easily under water
	if ( pml.groundPlane && DotProduct( pm->ps->velocity, pml.groundTrace.plane.normal ) < 0 ) {
		vel = VectorLength(pm->ps->velocity);
		// slide along the ground plane
		PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
			pm->ps->velocity, MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd));

		VectorNormalize(pm->ps->velocity);
		VectorScale(pm->ps->velocity, vel, pm->ps->velocity);
		PM_UpdateAntiLoop();
	}

	PM_SlideMove( qfalse );
	PM_UpdateAntiLoop();
}

/*
===================
PM_FlyMove

Only with the flight powerup
===================
*/
 void  PMove::PM_FlyMove( void ) {
	int		i;
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;

	// normal slowdown
	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );
	
	if ( pm->ps->pm_type == PM_SPECTATOR && pm->cmd.buttons & BUTTON_ALT_ATTACK) { // MVSDK: 1.03+ feature, but let's enable it on 1.02 as well.
		//turbo boost
		scale *= 10;
	}
	if (pm->cmd.buttons & BUTTON_BOUNCEPOWER) {	//turbo boost for bounce mode (comfier to use mouse2 with it)
		scale *= 10;
	}

	//
	// user intentions
	//
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = pm->ps->speed * (pm->cmd.upmove/127.0f); // MVSDK: 1.02 originally put this to 0, but let's use 1.03+ behaviour for this as well.
	} else {
		for (i=0 ; i<3 ; i++) {
			wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;
		}

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	PM_Accelerate (wishdir, wishspeed, pm_flyaccelerate);
	PM_UpdateAntiLoop();

	PM_StepSlideMove( qfalse );
	PM_UpdateAntiLoop();
}


/*
===================
PM_AirMove

===================
*/
 void  PMove::PM_AirMove( void ) {
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);

	if (pm->ps->pm_type != PM_SPECTATOR)
	{
		if (pm->modParms.physics != MV_CHARGEJUMP) { // mirrors the old #if METROID_JUMP clause

			PM_CheckJump();
		}
		else {
			if (pm->ps->fd.forceJumpZStart &&
				pm->ps->forceJumpFlip)
			{
				PM_CheckJump();
			}
		}
	}
	PM_Friction();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale( &cmd );

	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();

	// project moves down to flat plane
	pml.forward[2] = 0;
	pml.right[2] = 0;
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	if ( gPMDoSlowFall )
	{//no air-control
		VectorClear( wishvel );
	}
	else
	{
		for ( i = 0 ; i < 2 ; i++ )
		{
			wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
		}
		wishvel[2] = 0;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	if (pm->modParms.physics != MV_JK2SP) {
		wishspeed *= scale;
	}

	// not on ground, so little effect on velocity
	if (pm->modParms.physics == MV_SICKO) {
		PM_SickoAccelerate(wishdir, wishspeed, pm_airaccelerate,200.0f);
	}
	else if (pm->modParms.physics == MV_QUAJK) {
		float		accel;

		if (DotProduct(pm->ps->velocity, wishdir) < 0)
			accel = pm_cpm_airstopaccelerate;
		else
			accel = pm_airaccelerate;
		PM_QuaJKAccelerate(wishdir, wishspeed, accel,pm_cpm_airstrafeaccelerate,30.0f);
	}
	else if (pm->modParms.physics == MV_DREAM) {
		PM_DreamAccelerate(wishdir, wishspeed, pm_airaccelerate,100,200.0f);
	}
	else {
		PM_Accelerate(wishdir, wishspeed, pm->modParms.physics == MV_JK2SP ? pm_sp_airaccelerate : pm_airaccelerate);
	}
	PM_UpdateAntiLoop();

	// we may have a ground plane that is very steep, even
	// though we don't have a groundentity
	// slide along the steep plane
	if ( pml.groundPlane ) {
		if (pm->modParms.runFlags & RFL_CLIMBTECH) {
			if (!(pm->ps->pm_flags & PMF_STUCK_TO_WALL))
			{//don't slide when stuck to a wall
				if (PM_GroundSlideOkay(pml.groundTrace.plane.normal[2]))
				{
					PM_ClipVelocity(pm->ps->velocity, pml.groundTrace.plane.normal,
						pm->ps->velocity, overbounce);
					PM_UpdateAntiLoop();
				}
			}
		}
		else {
			PM_ClipVelocity(pm->ps->velocity, pml.groundTrace.plane.normal,
				pm->ps->velocity, overbounce);
			PM_UpdateAntiLoop();
		}
	}

	if (pm->modParms.physics == MV_JK2SP) {
		if (!pm->ps->clientNum
			&& pm->ps->fd.forcePowerLevel_FP_LEVITATION_ > FORCE_LEVEL_0
			&& pm->ps->fd.forceJumpZStart
			&& pm->ps->velocity[2] > 0) {//I am force jumping and I'm not holding the button anymore
			float curHeight = pm->ps->origin[2] - pm->ps->fd.forceJumpZStart + (pm->ps->velocity[2] * pml.frametime);
			float maxJumpHeight = forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_];
			if (curHeight >= maxJumpHeight) {//reached top, cut velocity
				pm->ps->velocity[2] = 0;
			}
		}
	}

	PM_StepSlideMove ( qtrue );
	PM_UpdateAntiLoop();

	if (pml.groundBounces) {
		PM_CheckBounceJump(flatNormal, pm->ps->velocity);
	}
}

/*
===================
PM_WalkMove

===================
*/
 void  PMove::PM_WalkMove( void ) {
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;
	float		accelerate;
	float		vel;
	float		totalVel;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);

	if (pm->ps->velocity[0] < 0)
	{
		totalVel = -pm->ps->velocity[0];
	}
	else
	{
		totalVel = pm->ps->velocity[0];
	}

	if (pm->ps->velocity[1] < 0)
	{
		totalVel += -pm->ps->velocity[1];
	}
	else
	{
		totalVel += pm->ps->velocity[1];
	}

	if (totalVel < 200)
	{
		pm->ps->fd.forceSpeedSmash = 1;
	}

	if ( pm->waterlevel > 2 && DotProduct( pml.forward, pml.groundTrace.plane.normal ) > 0 ) {
		// begin swimming
		PM_WaterMove();
		return;
	}


	if (pm->ps->pm_type != PM_SPECTATOR)
	{
		if ( PM_CheckJump () ) {
			// jumped away
			if ( pm->waterlevel > 1 ) {
				PM_WaterMove();
			} else {
				PM_AirMove();
			}
			return;
		}
	}

	PM_Friction ();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale( &cmd );

	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();

	// project moves down to flat plane
	pml.forward[2] = 0;
	pml.right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, overbounce);
	PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, overbounce);
	//
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	for ( i = 0 ; i < 3 ; i++ ) {
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	}
	// when going up or down slopes the wish velocity should Not be zero

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// clamp the speed lower if ducking
	if ( pm->ps->pm_flags & PMF_DUCKED && (pm->modParms.physics != MV_JK2SP || !PM_InKnockDown(pm->ps)) ) {
		if ( wishspeed > pm->ps->speed * pm_duckScale ) {
			wishspeed = pm->ps->speed * pm_duckScale;
		}
	}
	else if ( (pm->ps->pm_flags & PMF_ROLLING) && !BG_InRoll(pm->ps, pm->ps->legsAnim) &&
		!PM_InRollComplete(pm->ps, pm->ps->legsAnim) && pm->modParms.physics != MV_JK2SP)
	{
		if ( wishspeed > pm->ps->speed * pm_duckScale ) {
			wishspeed = pm->ps->speed * pm_duckScale;
		}
	}

	// clamp the speed lower if wading or walking on the bottom
	if ( pm->waterlevel ) {
		float	waterScale;

		waterScale = pm->waterlevel / 3.0;
		waterScale = 1.0 - ( 1.0 - pm_swimScale ) * waterScale;
		if ( wishspeed > pm->ps->speed * waterScale ) {
			wishspeed = pm->ps->speed * waterScale;
		}
	}

	// when a player gets hit, they temporarily lose
	// full control, which allows them to be moved a bit
	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK ) {
		accelerate = pm_airaccelerate;
		if (pm->modParms.physics == MV_JK2SP)
			accelerate = pm_sp_airaccelerate; 
		else if (MovementIsQuake3Based(pm->modParms.physics))
			accelerate = pm_cpm_accelerate;
	} else {
		accelerate = pm_accelerate;
		if (pm->modParms.physics == MV_JK2SP)
			accelerate = pm_sp_accelerate;
		else if (MovementIsQuake3Based(pm->modParms.physics))
			accelerate = pm_cpm_accelerate;
	}

	if (pm->modParms.physics == MV_JK2SP && (DotProduct(pm->ps->velocity, wishdir)) < 0.0f)
	{//Encourage deceleration away from the current velocity
		wishspeed *= pm_sp_airDecelRate;
	}

	PM_Accelerate (wishdir, wishspeed, accelerate);
	PM_UpdateAntiLoop();

	//Com_Printf("velocity = %1.1f %1.1f %1.1f\n", pm->ps->velocity[0], pm->ps->velocity[1], pm->ps->velocity[2]);
	//Com_Printf("velocity1 = %1.1f\n", VectorLength(pm->ps->velocity));

	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK )
	{
		if (pm->modParms.physics == MV_JK2SP) {
			if (pm->ps->gravity >= 0 && pm->ps->groundEntityNum != ENTITYNUM_NONE && !VectorLengthSquared(pm->ps->velocity) && pml.groundTrace.plane.normal[2] == 1.0)
			{//on ground and not moving and on level ground, no reason to do stupid fucking gravity with the clipvelocity!!!!
			}
			else
			{
				//if (!(pm->ps->eFlags & EF_FORCE_GRIPPED))
#if CLIENTSIDEONLY
				if (!(pm->ps->pm_type == PM_FLOAT)) // is this correct? not that it matters in defrag anyway prolly
#else
				if (!(pm->ps->fd.forceGripBeingGripped)) // is this correct? not that it matters in defrag anyway prolly
#endif
				{
					pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
				}
			}
		}
		else {
			pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
		}
	}

	vel = VectorLength(pm->ps->velocity);

	// slide along the ground plane
	PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
		pm->ps->velocity, overbounce);
	PM_UpdateAntiLoop();
	PM_CheckBounceJump(pml.groundTrace.plane.normal, pm->ps->velocity);// allow jump out of a bounce

	// don't decrease velocity when going up or down a slope
	VectorNormalize(pm->ps->velocity);
	VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

	// don't do anything if standing still
	if (!pm->ps->velocity[0] && !pm->ps->velocity[1]) {
		pm->ps->fd.forceSpeedSmash = 1;
		return;
	}

	PM_StepSlideMove( qfalse );
	PM_UpdateAntiLoop();

	if (pml.groundBounces) {
		PM_CheckBounceJump(flatNormal, pm->ps->velocity);
	}

	//Com_Printf("velocity2 = %1.1f\n", VectorLength(pm->ps->velocity));
}


/*
==============
PM_DeadMove
==============
*/
 void  PMove::PM_DeadMove( void ) {
	float	forward;

	if ( !pml.walking ) {
		return;
	}

	// extra friction

	forward = VectorLength (pm->ps->velocity);
	forward -= 20;
	if ( forward <= 0 ) {
		VectorClear (pm->ps->velocity);
	} else {
		VectorNormalize (pm->ps->velocity);
		VectorScale (pm->ps->velocity, forward, pm->ps->velocity);
	}
}


/*
===============
PM_NoclipMove
===============
*/
 void  PMove::PM_NoclipMove( void ) {
	float	speed, drop, friction, control, newspeed;
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;

	pm->ps->viewheight = DEFAULT_VIEWHEIGHT;

	// friction

	speed = VectorLength (pm->ps->velocity);
	if (speed < 1)
	{
		VectorCopy (vec3_origin, pm->ps->velocity);
	}
	else
	{
		drop = 0;

		friction = pm_friction*1.5;	// extra friction
		control = speed < pm_stopspeed ? pm_stopspeed : speed;
		drop += control*friction*pml.frametime;

		// scale the velocity
		newspeed = speed - drop;
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;

		VectorScale (pm->ps->velocity, newspeed, pm->ps->velocity);
	}

	// accelerate
	scale = PM_CmdScale( &pm->cmd );
	if (pm->cmd.buttons & BUTTON_ATTACK) {	//turbo boost
		scale *= 10;
	}
	if (pm->cmd.buttons & BUTTON_ALT_ATTACK) {	//turbo boost
		scale *= 10;
	}
	if (pm->cmd.buttons & BUTTON_BOUNCEPOWER) {	//turbo boost for bounce mode (comfier to use mouse2 with it)
		scale *= 10;
	}

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;
	
	for (i=0 ; i<3 ; i++)
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	wishvel[2] += pm->cmd.upmove;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate( wishdir, wishspeed, pm_accelerate );
	PM_UpdateAntiLoop();

	// move
	VectorMA (pm->ps->origin, pml.frametime, pm->ps->velocity, pm->ps->origin);
}

//============================================================================

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
 int  PMove::PM_FootstepForSurface( void )
{
	if ( pml.groundTrace.surfaceFlags & SURF_NOSTEPS ) 
	{
		return 0;
	}
	if ( pml.groundTrace.surfaceFlags & SURF_METALSTEPS ) 
	{
		return EV_FOOTSTEP_METAL_GENERAL;
	}
	return EV_FOOTSTEP_GENERAL;
}
 int  PMove::PM_TryRoll( void )
{
	float rollDist = 64;
	trace_t	trace;
	int		anim = -1;
	vec3_t fwd, right, traceto, mins, maxs, fwdAngles;

	if (pm->modParms.physics == MV_JK2SP) {
		rollDist = 192;
	}

	if ( BG_SaberInAttack( pm->ps->saberMove ) || BG_SaberInSpecialAttack( pm->ps->torsoAnim ) 
		|| BG_SpinningSaberAnim( pm->ps->legsAnim ) 
		|| (jk2gameplay != VERSION_1_02 && PM_SaberInStart( pm->ps->saberMove )) ) // MVSDK: In 1.02 everyone except client 0 could roll during SaberInStart. In 1.03 and later nobody could roll during SaberInStart. 1.02 people consider client 0 being unable to roll as client 0 bug, so let him roll, too.
	{//attacking or spinning (or, if player, starting an attack)
		return 0;
	}

	// TODO MAYBE jaPRO ysal stuff here?

	if (pm->ps->weapon != WP_SABER_GENERAL || BG_HasYsalamiri(pm->gametype, pm->ps) ||
		!BG_CanUseFPNow(pm->gametype, pm->ps, pm->cmd.serverTime, FP_LEVITATION))
	{ //Not using saber, or can't use jump
		return 0;
	}

	VectorSet(mins, pm->mins[0],pm->mins[1],pm->mins[2]+STEPSIZE);
	VectorSet(maxs, pm->maxs[0],pm->maxs[1],CROUCH_MAXS_2);

	VectorSet(fwdAngles, 0, pm->ps->viewangles[YAW], 0);

	AngleVectors( fwdAngles, fwd, right, NULL );

	if ( pm->cmd.forwardmove )
	{ //check forward/backward rolls
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) 
		{
			anim = BOTH_ROLL_B_GENERAL;
			VectorMA( pm->ps->origin, -rollDist, fwd, traceto );
		}
		else
		{
			anim = BOTH_ROLL_F_GENERAL;
			VectorMA( pm->ps->origin, rollDist, fwd, traceto );
		}
	}
	else if ( pm->cmd.rightmove > 0 )
	{ //right
		anim = BOTH_ROLL_R_GENERAL;
		VectorMA( pm->ps->origin, rollDist, right, traceto );
	}
	else if ( pm->cmd.rightmove < 0 )
	{ //left
		anim = BOTH_ROLL_L_GENERAL;
		VectorMA( pm->ps->origin, -rollDist, right, traceto );
	}

	if ( anim != -1 )
	{ //We want to roll. Perform a trace to see if we can, and if so, send us into one.
		PM_Trace( &trace, pm->ps->origin, mins, maxs, traceto, pm->ps->clientNum, CONTENTS_SOLID );
		if ( trace.fraction >= 1.0f )
		{
			pm->ps->saberMove = LS_NONE_GENERAL;
			return anim;
		}
	}
	return 0;
}

/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
 void  PMove::PM_CrashLand( void ) {
	float		delta;
	float		dist;
	float		vel, acc;
	float		t;
	float		a, b, c, den;
	qboolean	didRoll = qfalse;

	// calculate the exact velocity on landing
	dist = pm->ps->origin[2] - pml.previous_origin[2];
	vel = pml.previous_velocity[2];
	acc = -pm->ps->gravity;

	a = acc / 2;
	b = vel;
	c = -dist;

	den =  b * b - 4 * a * c;
	if ( den < 0 ) {
		pm->ps->inAirAnim = qfalse;
		return;
	}
	t = (-b - sqrtf( den ) ) / ( 2 * a );

	delta = vel + t * acc;
	delta = delta*delta * 0.0001;

	// ducking while falling doubles damage
	if ( pm->ps->pm_flags & PMF_DUCKED ) {
		delta *= 2;
	}

	// decide which landing animation to use
	if (!BG_InRoll(pm->ps, pm->ps->legsAnim) && pm->ps->inAirAnim)
	{ //only play a land animation if we transitioned into an in-air animation while off the ground
		if (!BG_SaberInSpecial(pm->ps->saberMove))
		{
			if ( pm->ps->pm_flags & PMF_BACKWARDS_JUMP ) {
				PM_ForceLegsAnim( BOTH_LANDBACK1_GENERAL );
			} else {
				PM_ForceLegsAnim( BOTH_LAND1_GENERAL );
			}
		}
	}

	if (pm->ps->weapon != WP_SABER_GENERAL)
	{ //saber handles its own anims
		//This will push us back into our weaponready stance from the land anim.
		if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1)
		{
			PM_StartTorsoAnim( TORSO_WEAPONREADY4_GENERAL);
		}
		else
		{
			if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL)
			{
				PM_StartTorsoAnim( BOTH_GUNSIT1_GENERAL );
			}
			else
			{
				PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
			}
		}
	}

	if (!BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) ||
		pm->ps->legsTimer < 1 ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_LEFT_GENERAL ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_WALL_RUN_RIGHT_GENERAL)
	{ //Only set the timer if we're in an anim that can be interrupted (this would not be, say, a flip)
		if (!BG_InRoll(pm->ps, pm->ps->legsAnim) && pm->ps->inAirAnim)
		{
			if (!BG_SaberInSpecial(pm->ps->saberMove) || pm->ps->weapon != WP_SABER_GENERAL)
			{
				pm->ps->legsTimer = TIMER_LAND;
			}
		}
	}

	pm->ps->inAirAnim = qfalse;

	// never take falling damage if completely underwater
	if ( pm->waterlevel == 3 ) {
		return;
	}

	// reduce falling damage if there is standing water
	if ( pm->waterlevel == 2 ) {
		delta *= 0.25;
	}
	if ( pm->waterlevel == 1 ) {
		delta *= 0.5;
	}

	if ( delta < 1 ) {
		return;
	}

	if ( pm->ps->pm_flags & PMF_DUCKED ) 
	{
		if( delta >= 2 && !PM_InOnGroundAnim( pm->ps->legsAnim ) && !PM_InKnockDown( pm->ps ) && !BG_InRoll(pm->ps, pm->ps->legsAnim) &&
			(pm->ps->forceHandExtend == HANDEXTEND_NONE || jk2gameplay != VERSION_1_04) )
		{//roll!
			int anim = PM_TryRoll();

			if (PM_InRollComplete(pm->ps, pm->ps->legsAnim))
			{
				anim = 0;
				pm->ps->legsTimer = 0;
				pm->ps->legsAnim = 0;
				PM_SetAnim(SETANIM_BOTH,BOTH_LAND1_GENERAL,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150);
				pm->ps->legsTimer = TIMER_LAND;
			}

			if ( anim )
			{//absorb some impact
				pm->ps->legsTimer = 0;
				delta /= 3; // /= 2 just cancels out the above delta *= 2 when landing while crouched, the roll itself should absorb a little damage
				pm->ps->legsAnim = 0;
				PM_SetAnim(SETANIM_BOTH,anim,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150);
				didRoll = qtrue;
			}
		}
	}

	// SURF_NODAMAGE is used for bounce pads where you don't ever
	// want to take damage or play a crunch sound
	if ( !(pml.groundTrace.surfaceFlags & SURF_NODAMAGE) )  {
		if (delta > 7)
		{
			int delta_send = (int)delta;

			if (delta_send > 600)
			{ //will never need to know any value above this
				delta_send = 600;
			}

			if (pm->ps->fd.forceJumpZStart)
			{
				if ((int)pm->ps->origin[2] >= (int)pm->ps->fd.forceJumpZStart)
				{ //was force jumping, landed on higher or same level as when force jump was started
					if (delta_send > 8)
					{
						delta_send = 8;
					}
				}
				else
				{
					if (delta_send > 8)
					{
						int dif = ((int)pm->ps->fd.forceJumpZStart - (int)pm->ps->origin[2]);
						int dmgLess = (forceJumpHeight[pm->ps->fd.forcePowerLevel_FP_LEVITATION_] - dif);

						if (dmgLess < 0)
						{
							dmgLess = 0;
						}

						delta_send -= (dmgLess*0.3);

						if (delta_send < 8)
						{
							delta_send = 8;
						}

						//Com_Printf("Damage sub: %i\n", (int)((dmgLess*0.1)));
					}
				}
			}

			if (didRoll)
			{ //Add the appropriate event..
				PM_AddEventWithParm( EV_ROLL_GENERAL, delta_send );
			}
			else
			{
				PM_AddEventWithParm( EV_FALL_GENERAL, delta_send );
			}
		}
		else
		{
			if (didRoll)
			{
				PM_AddEventWithParm( EV_ROLL_GENERAL, 0 );
			}
			else
			{
				PM_AddEvent( PM_FootstepForSurface() );
			}
		}
	}

	// make sure velocity resets so we don't bounce back up again in case we miss the clear elsewhere
	if (!pml.bounceJumped && pm->modParms.physics != MV_PINBALL) {
		pm->ps->velocity[2] = 0;
	}

	// nah lets not do this. this isnt a true q3 overbounce, not even close. more of a meme q3 overbounce version.
	// but q3 overbounce is also kinda lame as way too finnicky. lets do sth more fun. bouncy mode maybe
	//if ((MovementIsQuake3Based(moveStyle)) && ((int)pm->ps->fd.forceJumpZStart > pm->ps->origin[2] + 1)) {
	//	if (1 > (sqrtf(pm->ps->velocity[0] * pm->ps->velocity[0] + pm->ps->velocity[1] * pm->ps->velocity[1])))//No xyvel
	//		pm->ps->velocity[2] = -vel; //OVERBOUNCE OVER BOUNCE
	////}

	// start footstep cycle over
	pm->ps->bobCycle = 0;
}

/*
=============
PM_CorrectAllSolid
=============
*/
 int  PMove::PM_CorrectAllSolid( trace_t *trace ) {
	int			i, j, k;
	vec3_t		point;

	if ( pm->debugLevel ) {
		Com_Printf("%i:allsolid\n", c_pmove);
	}

	// jitter around
	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			for (k = -1; k <= 1; k++) {
				VectorCopy(pm->ps->origin, point);
				point[0] += (float) i;
				point[1] += (float) j;
				point[2] += (float) k;
				PM_Trace(trace, point, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
				if ( !trace->allsolid ) {
					point[0] = pm->ps->origin[0];
					point[1] = pm->ps->origin[1];
					point[2] = pm->ps->origin[2] - 0.25;

					PM_Trace(trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
					pml.groundTrace = *trace;
					return qtrue;
				}
			}
		}
	}

	PM_SetGroundEntityNum(ENTITYNUM_NONE);
	pml.groundPlane = qfalse;
	pml.walking = qfalse;

	return qfalse;
}

/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
 void  PMove::PM_GroundTraceMissed( void ) {
	trace_t		trace;
	vec3_t		point;

	//rww - don't want to do this when handextend_choke, because you can be standing on the ground
	//while still holding your throat.
	if ( pm->ps->pm_type == PM_FLOAT && jk2gameplay != VERSION_1_02 )
	{
		//we're assuming this is because you're being choked
		int parts = SETANIM_LEGS;

		//rww - also don't use SETANIM_FLAG_HOLD, it will cause the legs to float around a bit before going into
		//a proper anim even when on the ground.
		PM_SetAnim(parts, BOTH_CHOKE3_GENERAL, SETANIM_FLAG_OVERRIDE, 100);
	}
	//If the anim is choke3, act like we just went into the air because we aren't in a float
	else if ( pm->ps->groundEntityNum != ENTITYNUM_NONE || ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_CHOKE3_GENERAL && jk2gameplay != VERSION_1_02) )
	{
		// we just transitioned into freefall
		if ( pm->debugLevel ) {
			Com_Printf("%i:lift\n", c_pmove);
		}

		// if they aren't in a jumping animation and the ground is a ways away, force into it
		// if we didn't do the trace, the player would be backflipping down staircases
		VectorCopy( pm->ps->origin, point );
		point[2] -= 64;

		PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
		if ( trace.fraction == 1.0 || pm->ps->pm_type == PM_FLOAT ) {
			if ( pm->ps->velocity[2] <= 0 && !(pm->ps->pm_flags&PMF_JUMP_HELD))
			{
				PM_SetAnim(SETANIM_LEGS,BOTH_INAIR1_GENERAL,SETANIM_FLAG_OVERRIDE, 100);
				pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
			}
			else if ( pm->cmd.forwardmove >= 0 ) 
			{
				PM_SetAnim(SETANIM_LEGS,BOTH_JUMP1_GENERAL,SETANIM_FLAG_OVERRIDE, 100);
				pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
			} 
			else 
			{
				PM_SetAnim(SETANIM_LEGS,BOTH_JUMPBACK1_GENERAL,SETANIM_FLAG_OVERRIDE, 100);
				pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
			}

			pm->ps->inAirAnim = qtrue;
		}
	}
	else if (!pm->ps->inAirAnim)
	{
		// if they aren't in a jumping animation and the ground is a ways away, force into it
		// if we didn't do the trace, the player would be backflipping down staircases
		VectorCopy( pm->ps->origin, point );
		point[2] -= 64;

		PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
		if ( trace.fraction == 1.0 || pm->ps->pm_type == PM_FLOAT )
		{
			pm->ps->inAirAnim = qtrue;
		}
	}

	if (PM_InRollComplete(pm->ps, pm->ps->legsAnim))
	{ //Client won't catch an animation restart because it only checks frame against incoming frame, so if you roll when you land after rolling
	  //off of something it won't replay the roll anim unless we switch it off in the air. This fixes that.
		PM_SetAnim(SETANIM_BOTH,BOTH_INAIR1_GENERAL,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150);
		pm->ps->inAirAnim = qtrue;
	}

	PM_SetGroundEntityNum(ENTITYNUM_NONE);
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
}


/*
=============
PM_GroundTrace
=============
*/
 void  PMove::PM_GroundTrace( void ) {
	vec3_t		point;
	trace_t		trace;
	float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);

	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];
	point[2] = pm->ps->origin[2] - 0.25;

	if (MovementStyleHasQuake2Ramps(pm->modParms.physics) && pm->ps->velocity[2] > 180) {
		if (pm->debugLevel) {
			Com_Printf("%i:q2ramp\n", c_pmove);
		}
		PM_SetGroundEntityNum(ENTITYNUM_NONE);
		pml.groundPlane = qtrue;
		pml.walking = qfalse;
		return;
	}

	PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
	pml.groundTrace = trace;

	// do something corrective if the trace starts in a solid...
	if ( trace.allsolid ) {
		if ( !PM_CorrectAllSolid(&trace) )
			return;
	}

	if (pm->ps->pm_type == PM_FLOAT)
	{
		PM_GroundTraceMissed();
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// if the trace didn't hit anything, we are in free fall
	if ( trace.fraction == 1.0 ) {
		PM_GroundTraceMissed();
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// check if getting thrown off the ground
	if ( pm->ps->velocity[2] > 0 && DotProduct( pm->ps->velocity, trace.plane.normal ) > 10 ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:kickoff\n", c_pmove);
		}
		// go into jump animation
		if ( pm->cmd.forwardmove >= 0 ) {
			PM_ForceLegsAnim( BOTH_JUMP1_GENERAL );
			pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
		} else {
			PM_ForceLegsAnim( BOTH_JUMPBACK1_GENERAL );
			pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
		}

		PM_SetGroundEntityNum(ENTITYNUM_NONE);
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}
	
	// slopes that are too steep will not be considered onground
	if ( trace.plane.normal[2] < MIN_WALK_NORMAL ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:steep\n", c_pmove);
		}
		PM_SetGroundEntityNum(ENTITYNUM_NONE);
		if (pm->modParms.physics != MV_DREAM) {
			pml.groundPlane = qtrue;
		}
		pml.walking = qfalse;
		pm->roll.segmentDisqualified = qtrue; // we are sliding, giving us extra speed. disqualify the roll.
		return;
	}

	if (!pml.bounceJumped && pm->modParms.physics != MV_PINBALL) {
		pml.groundPlane = qtrue;
		pml.walking = qtrue;
	}

	// hitting solid ground will end a waterjump
	if (pm->ps->pm_flags & PMF_TIME_WATERJUMP)
	{
		pm->ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
		pm->ps->pm_time = 0;
	}

	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
		// just hit the ground
		if ( pm->debugLevel ) {
			Com_Printf("%i:Land\n", c_pmove);
		}

		// Thanks to Loda for making this fix and Daggo for pointing me to it.
		if ((trace.plane.normal[0] != 0.0f || trace.plane.normal[1] != 0.0f || trace.plane.normal[2] != 1.0f))// don't count them during special predict
		{ // It's a ramp!
			if (!pml.clipped)
			{
				// TODO should we do more checks here to make sure it behaves same as normal clip would? 
				// the trace.plane.normal[2] != 1.0f check in particular seems sus no? since the slidemove stuff
				// works more with various dot products to determine whether to clip etc. oh well. fuck it.

				if (pm->modParms.runFlags & RFL_NODEADRAMPS) {
					if (pm->modParms.physics == MV_PINBALL) {
						//PM_LimitedClipVelocity(pm->ps->velocity, planes[i], pm->ps->velocity, overbounce,100000.0f);
						overbounce -= trace.plane.normal[2] * 0.6f * (std::min(1600.0f, fabsf(pm->ps->velocity[2])) / 1600.0f); // dont let ground and ceiling bounce as as insanely much.
						PM_LimitedClipVelocity2(pm->ps->velocity, trace.plane.normal, pm->ps->velocity, overbounce, 10000.0f);
					}
					else {
						PM_ClipVelocity(pm->ps->velocity, trace.plane.normal, pm->ps->velocity, overbounce);
					}
					PM_UpdateAntiLoop();
					PM_CheckBounceJump(trace.plane.normal, pm->ps->velocity); // do we need this here? not sure.

					if (pm->debugLevel) {
						Com_Printf("%i:Dead ramp fixed\n", c_pmove);
					}
				}
				else {
					if (pm->debugLevel) {
						Com_Printf("%i:Dead ramp\n", c_pmove);
					}
				}
#if JK2_CGAME
				if (pm->ps->commandTime > cg_rampCountLastCmdTime && !pm->isSpecialPredict) {
					cg_deadRampsCounted++;
				}
#endif
			}
			else {
				if (pm->debugLevel) {
					Com_Printf("%i:Good ramp\n", c_pmove);
				}
#if JK2_CGAME
				if (pm->ps->commandTime > cg_rampCountLastCmdTime && !pm->isSpecialPredict) {
					cg_goodRampsCounted++;
				}
#endif
			}
#if JK2_CGAME
			if(!pm->isSpecialPredict) cg_rampCountLastCmdTime = pm->ps->commandTime;
#endif
		}
		
		/*if (moveStyle == MV_CHARGEJUMP && pm->cmd.upmove < 10 && !(pm->ps->pm_flags & PMF_JUMP_HELD) && !(pm->cmd.buttons & BUTTON_BOUNCEPOWER)) { //  TA instead of canceling the charging in air when nothing pressed, cancel when we land if we arent pressing jump and not pressing charge.
			if (!(pm->cmd.buttons & BUTTON_FORCEPOWER) ||
				pm->ps->fd.forcePowerSelected != FP_LEVITATION) {
				pm->ps->fd.forceJumpCharge = 0;
#if JK2_GAME
				G_MuteSound(pm->ps->fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
#endif
			}
		}*/

		PM_CrashLand();

		// don't do landing time if we were just going down a slope
		if ( pml.previous_velocity[2] < -200 ) {
			// don't allow another jump for a little while
			pm->ps->pm_flags |= PMF_TIME_LAND;
			pm->ps->pm_time = 250;
		}
	}

	if (!pml.bounceJumped && pm->modParms.physics != MV_PINBALL) {
		PM_SetGroundEntityNum(trace.entityNum);
	}
#if !CLIENTSIDEONLY
	pm->ps->lastOnGround = pm->cmd.serverTime;
#endif

	PM_AddTouchEnt( trace.entityNum );
}


/*
=============
PM_SetWaterLevel
=============
*/
 void  PMove::PM_SetWaterLevel( void ) {
	vec3_t		point;
	int			cont;
	int			sample1;
	int			sample2;

	//
	// get waterlevel, accounting for ducking
	//
	pm->waterlevel = 0;
	pm->watertype = 0;

	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];
	point[2] = pm->ps->origin[2] + MINS_Z + 1;	
	cont = PM_PointContents( point, pm->ps->clientNum );

	if ( cont & MASK_WATER ) {
		sample2 = pm->ps->viewheight - MINS_Z;
		sample1 = sample2 / 2;

		pm->watertype = cont;
		pm->waterlevel = 1;
		point[2] = pm->ps->origin[2] + MINS_Z + sample1;
		cont = PM_PointContents(point, pm->ps->clientNum );
		if ( cont & MASK_WATER ) {
			pm->waterlevel = 2;
			point[2] = pm->ps->origin[2] + MINS_Z + sample2;
			cont = PM_PointContents(point, pm->ps->clientNum );
			if ( cont & MASK_WATER ){
				pm->waterlevel = 3;
			}
		}
	}

}

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/
 void  PMove::PM_CheckDuck (void)
{
	trace_t	trace;

	pm->mins[0] = -15;
	pm->mins[1] = -15;

	pm->maxs[0] = 15;
	pm->maxs[1] = 15;

	pm->mins[2] = MINS_Z;

	if (pm->ps->pm_type == PM_DEAD)
	{
		pm->maxs[2] = -8;
		pm->ps->viewheight = DEAD_VIEWHEIGHT;
		return;
	}

	if (pm->ps->usingATST)
	{
		if (pm->cmd.upmove < 0)
		{
			pm->cmd.upmove = 0;
		}
	}

	if (BG_InRoll(pm->ps, pm->ps->legsAnim))
	{
		pm->maxs[2] = CROUCH_MAXS_2;
		pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
		pm->ps->pm_flags &= ~PMF_DUCKED;
		pm->ps->pm_flags |= PMF_ROLLING;
		return;
	}
	else if (pm->ps->pm_flags & PMF_ROLLING)
	{
		// try to stand up
		pm->maxs[2] = DEFAULT_MAXS_2;
		PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, pm->ps->origin, pm->ps->clientNum, pm->tracemask );
		if (!trace.allsolid)
			pm->ps->pm_flags &= ~PMF_ROLLING;
	}
	else if (pm->cmd.upmove < 0 ||
		pm->ps->forceHandExtend == HANDEXTEND_KNOCKDOWN)
	{	// duck
		pm->ps->pm_flags |= PMF_DUCKED;
	}
	else
	{	// stand up if possible 
		if (pm->ps->pm_flags & PMF_DUCKED)
		{
			// try to stand up
			pm->maxs[2] = DEFAULT_MAXS_2;
			PM_Trace(&trace, pm->ps->origin, pm->mins, pm->maxs, pm->ps->origin, pm->ps->clientNum, pm->tracemask );
			if (!trace.allsolid)
				pm->ps->pm_flags &= ~PMF_DUCKED;
		}
	}

	if (pm->ps->pm_flags & PMF_DUCKED)
	{
		pm->maxs[2] = CROUCH_MAXS_2;
		pm->ps->viewheight = CROUCH_VIEWHEIGHT;
	}
	else if (pm->ps->pm_flags & PMF_ROLLING)
	{
		pm->maxs[2] = CROUCH_MAXS_2;
		pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
	}
	else
	{
		pm->maxs[2] = DEFAULT_MAXS_2;
		pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
	}

	if (pm->ps->usingATST)
	{
		pm->mins[0] = ATST_MINS0;
		pm->mins[1] = ATST_MINS1;
		pm->mins[2] = ATST_MINS2;

		pm->maxs[0] = ATST_MAXS0;
		pm->maxs[1] = ATST_MAXS1;
		pm->maxs[2] = ATST_MAXS2;
	}
}



//===================================================================



/*
==============
PM_Use

Generates a use event
==============
*/
#define USE_DELAY 2000

void  PMove::PM_Use( void ) 
{
#if CLIENTSIDEONLY // who cares, we can't do anything with it anyway, plus, useTime isn't even communicated via network
	pm->useEvent = 0;
#else
	if ( pm->ps->useTime > 0 )
		pm->ps->useTime -= 100;//pm->cmd.msec;

	if ( pm->ps->useTime > 0 ) {
		return;
	}

	if ( ! (pm->cmd.buttons & BUTTON_USE ) )
	{
		pm->useEvent = 0;
		pm->ps->useTime = 0;
		return;
	}

	pm->useEvent = EV_USE_GENERAL;
	pm->ps->useTime = USE_DELAY;
#endif
}

qboolean  PMove::PM_RunningAnim( int anim )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_RUN1_GENERAL:			
	case BOTH_RUN2_GENERAL:			
	case BOTH_RUNBACK1_GENERAL:			
	case BOTH_RUNBACK2_GENERAL:			
	case BOTH_RUNAWAY1_GENERAL:			
		return qtrue;
		break;
	}
	return qfalse;
}

/*
===============
PM_Footsteps
===============
*/
 void  PMove::PM_Footsteps( void ) {
	float		bobmove;
	int			old;
	// qboolean	footstep;
	int			setAnimFlags = 0;

	if ( ((PM_InSaberAnim( (pm->ps->legsAnim&~ANIM_TOGGLEBIT) ) && !BG_SpinningSaberAnim( (pm->ps->legsAnim&~ANIM_TOGGLEBIT) )) 
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_STAND1_GENERAL 
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_STAND1TO2_GENERAL 
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_STAND2TO1_GENERAL 
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_STAND2_GENERAL 
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_SABERFAST_STANCE_GENERAL
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_SABERSLOW_STANCE_GENERAL
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_BUTTON_HOLD_GENERAL
		|| (pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_BUTTON_RELEASE_GENERAL
		|| PM_LandingAnim( (pm->ps->legsAnim&~ANIM_TOGGLEBIT) ) 
		|| PM_PainAnim( (pm->ps->legsAnim&~ANIM_TOGGLEBIT) ))
		&& jk2gameplay != VERSION_1_02 )
	{//legs are in a saber anim, and not spinning, be sure to override it
		setAnimFlags |= SETANIM_FLAG_OVERRIDE;
	}

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	pm->xyspeed = sqrtf( pm->ps->velocity[0] * pm->ps->velocity[0]
		+  pm->ps->velocity[1] * pm->ps->velocity[1] );

	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {

		// airborne leaves position in cycle intact, but doesn't advance
		if ( pm->waterlevel > 1 )
		{ // MVSDK: Swimming is broken in 1.02, but let's NOT port the brokeness back in here for 1.02-gameplay. Most 1.02 mods apply the 1.04 behaviour anyway.
			if (pm->xyspeed > 60)
			{
				PM_ContinueLegsAnim( BOTH_SWIMFORWARD_GENERAL );
			}
			else
			{
				PM_ContinueLegsAnim( BOTH_SWIM_IDLE1_GENERAL );
			}
		}
		return;
	}

	// if not trying to move
	if ( !pm->cmd.forwardmove && !pm->cmd.rightmove ) {
		if (  pm->xyspeed < 5 ) {
			pm->ps->bobCycle = 0;	// start at beginning of cycle again
			if ( (pm->ps->pm_flags & PMF_DUCKED) || (pm->ps->pm_flags & PMF_ROLLING) ) {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_CROUCH1IDLE_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_CROUCH1IDLE_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_CROUCH1IDLE_GENERAL );
				}
			} else {
				if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1)
				{
					PM_ContinueLegsAnim( TORSO_WEAPONREADY4_GENERAL);
				}
				else
				{
					if (pm->ps->weapon == WP_SABER_GENERAL && pm->ps->saberHolstered)
					{
						PM_ContinueLegsAnim( BOTH_STAND1_GENERAL );
					}
					else if ( pm->ps->weapon == WP_SABER_GENERAL && pm->ps->dualBlade )
					{
						PM_ContinueLegsAnim( BOTH_STAND1_GENERAL );
					}
					else
					{
						PM_ContinueLegsAnim( WeaponReadyAnim[pm->ps->weapon] );
					}
				}
			}
		}
		return;
	}
	

	// footstep = qfalse;

	if ( pm->ps->pm_flags & PMF_DUCKED )
	{
		int rolled = 0;

		bobmove = 0.5;	// ducked characters bob much faster

		if ( PM_RunningAnim( pm->ps->legsAnim ) && !BG_InRoll(pm->ps, pm->ps->legsAnim) )
		{//roll!
			rolled = PM_TryRoll();
		}
		if ( !rolled )
		{ //if the roll failed or didn't attempt, do standard crouching anim stuff.
			if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_CROUCH1WALKBACK_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_CROUCH1WALKBACK_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_CROUCH1WALKBACK_GENERAL );
				}
			}
			else {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_CROUCH1WALK_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_CROUCH1WALK_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_CROUCH1WALK_GENERAL );
				}
			}
		}
		else
		{ //otherwise send us into the roll
			pm->ps->legsTimer = 0;
			pm->ps->legsAnim = 0;
			PM_SetAnim(SETANIM_BOTH,rolled,SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 150);
			PM_AddEventWithParm( EV_ROLL_GENERAL, 0 );
			pm->maxs[2] = CROUCH_MAXS_2;
			pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
			pm->ps->pm_flags &= ~PMF_DUCKED;
			pm->ps->pm_flags |= PMF_ROLLING;
		}
	}
	else if ((pm->ps->pm_flags & PMF_ROLLING) && !BG_InRoll(pm->ps, pm->ps->legsAnim) &&
		!PM_InRollComplete(pm->ps, pm->ps->legsAnim))
	{
		bobmove = 0.5;	// ducked characters bob much faster

		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN )
		{
			if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_CROUCH1WALKBACK_GENERAL && jk2gameplay != VERSION_1_02)
			{
				PM_SetAnim(SETANIM_LEGS, BOTH_CROUCH1WALKBACK_GENERAL, setAnimFlags, 100);
			}
			else
			{
				PM_ContinueLegsAnim( BOTH_CROUCH1WALKBACK_GENERAL );
			}
		}
		else
		{
			if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_CROUCH1WALK_GENERAL && jk2gameplay != VERSION_1_02)
			{
				PM_SetAnim(SETANIM_LEGS, BOTH_CROUCH1WALK_GENERAL, setAnimFlags, 100);
			}
			else
			{
				PM_ContinueLegsAnim( BOTH_CROUCH1WALK_GENERAL );
			}
		}
	}
	else
	{
		if ( !( pm->cmd.buttons & BUTTON_WALKING ) ) {
			bobmove = 0.4f;	// faster speeds bob faster
			if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_RUNBACK1_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_RUNBACK1_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_RUNBACK1_GENERAL );
				}
			}
			else {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_RUN1_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_RUN1_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_RUN1_GENERAL );
				}
			}
			// footstep = qtrue;
		} else {
			bobmove = 0.2f;	// walking bobs slow
			if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_WALKBACK1_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_WALKBACK1_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_WALKBACK1_GENERAL );
				}
			}
			else {
				if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) != BOTH_WALK1_GENERAL && jk2gameplay != VERSION_1_02)
				{
					PM_SetAnim(SETANIM_LEGS, BOTH_WALK1_GENERAL, setAnimFlags, 100);
				}
				else
				{
					PM_ContinueLegsAnim( BOTH_WALK1_GENERAL );
				}
			}
		}
	}

	// check for footstep / splash sounds
	old = pm->ps->bobCycle;
	pm->ps->bobCycle = (int)( old + bobmove * pml.msec ) & 255;

	// if we just crossed a cycle boundary, play an apropriate footstep event
	if ( ( ( old + 64 ) ^ ( pm->ps->bobCycle + 64 ) ) & 128 )
	{
#if !CLIENTSIDEONLY
		pm->ps->footstepTime = pm->cmd.serverTime + 300;
#endif
		if ( pm->waterlevel == 1 ) {
			// splashing
			PM_AddEvent( EV_FOOTSPLASH_GENERAL );
		} else if ( pm->waterlevel == 2 ) {
			// wading / swimming at surface
			PM_AddEvent( EV_SWIM_GENERAL );
		} else if ( pm->waterlevel == 3 ) {
			// no sound when completely underwater
		}
	}
}

/*
==============
PM_WaterEvents

Generate sound events for entering and leaving water
==============
*/
 void  PMove::PM_WaterEvents( void ) {		// FIXME?
	//
	// if just entered a water volume, play a sound
	//
	if (!pml.previous_waterlevel && pm->waterlevel) {
		PM_AddEvent( EV_WATER_TOUCH_GENERAL );
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (pml.previous_waterlevel && !pm->waterlevel) {
		PM_AddEvent( EV_WATER_LEAVE_GENERAL );
	}

	//
	// check for head just going under water
	//
	if (pml.previous_waterlevel != 3 && pm->waterlevel == 3) {
		PM_AddEvent( EV_WATER_UNDER_GENERAL );
	}

	//
	// check for head just coming out of water
	//
	if (pml.previous_waterlevel == 3 && pm->waterlevel != 3) {
		PM_AddEvent( EV_WATER_CLEAR_GENERAL );
	}
}


/*
===============
PM_BeginWeaponChange
===============
*/
void  PMove::PM_BeginWeaponChange( int weapon ) {
	if ( weapon <= WP_NONE_GENERAL || weapon >= WP_NUM_WEAPONS_GENERAL ) {
		return;
	}

	if ( !( pm->ps->stats[STAT_WEAPONS] & ( 1 << weapon ) ) ) {
		return;
	}
	
	if ( pm->ps->weaponstate == WEAPON_DROPPING ) {
		return;
	}

	// turn of any kind of zooming when weapon switching.
	if (pm->ps->zoomMode)
	{
		pm->ps->zoomMode = 0;
		pm->ps->zoomTime = pm->ps->commandTime;
	}

	// If the player still got the rocket launcher locked on a target remove the lock.
	if ( pm->ps->rocketLockIndex != MAX_CLIENTS )
	{
		pm->ps->rocketLockIndex = MAX_CLIENTS;
		pm->ps->rocketLockTime = 0;
		pm->ps->rocketTargetTime = 0;
	}

	PM_AddEvent( EV_CHANGE_WEAPON_GENERAL );
	pm->ps->weaponstate = WEAPON_DROPPING;
	pm->ps->weaponTime += 200;
	PM_StartTorsoAnim( TORSO_DROPWEAP1_GENERAL );
}


/*
===============
PM_FinishWeaponChange
===============
*/
void  PMove::PM_FinishWeaponChange( void ) {
	int		weapon;

	weapon = pm->cmd.weapon;
	if ( weapon < WP_NONE_GENERAL || weapon >= WP_NUM_WEAPONS_GENERAL ) {
		weapon = WP_NONE_GENERAL;
	}

	if ( !( pm->ps->stats[STAT_WEAPONS] & ( 1 << weapon ) ) ) {
		weapon = WP_NONE_GENERAL;
	}

	if (weapon == WP_SABER_GENERAL)
	{
		PM_SetSaberMove(LS_DRAW_GENERAL);
	}
	else
	{
		PM_StartTorsoAnim( TORSO_RAISEWEAP1_GENERAL);
	}
	pm->ps->weapon = weapon;
	pm->ps->weaponstate = WEAPON_RAISING;
	pm->ps->weaponTime += 250;
}



//---------------------------------------
 qboolean  PMove::PM_DoChargedWeapons( void )
//---------------------------------------
{
	vec3_t		ang;
	trace_t		tr;
	qboolean	charging = qfalse,
				altFire = qfalse;


	if (pm->modParms.raceMode)
		return qfalse;

	// If you want your weapon to be a charging weapon, just set this bit up
	switch( pm->ps->weapon )
	{
	//------------------
	case WP_BRYAR_PISTOL_GENERAL:

		// alt-fire charges the weapon
		if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
		{
			charging = qtrue;
			altFire = qtrue;
		}
		break;
	
	//------------------
	case WP_BOWCASTER_GENERAL:

		// primary fire charges the weapon
		if ( pm->cmd.buttons & BUTTON_ATTACK )
		{
			charging = qtrue;
		}
		break;
	
	//------------------
	case WP_ROCKET_LAUNCHER_GENERAL:

		// Not really a charge weapon, but we still want to delay fire until the button comes up so that we can
		//	implement our alt-fire locking stuff
		if ( (pm->cmd.buttons & BUTTON_ALT_ATTACK) && pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] >= weaponData[pm->ps->weapon].altEnergyPerShot )
		{
			vec3_t muzzleOffPoint, muzzlePoint, forward, right, up;

			AngleVectors( pm->ps->viewangles, forward, right, up );

			charging = qtrue;
			altFire = qtrue;

			AngleVectors(pm->ps->viewangles, ang, NULL, NULL);

			VectorCopy( pm->ps->origin, muzzlePoint );
#if CLIENTSIDEONLY
			VectorSet(muzzleOffPoint,12, 8, 0);
#else
			VectorCopy(WP_MuzzlePoint[WP_ROCKET_LAUNCHER_GENERAL], muzzleOffPoint);
#endif

			VectorMA(muzzlePoint, muzzleOffPoint[0], forward, muzzlePoint);
			VectorMA(muzzlePoint, muzzleOffPoint[1], right, muzzlePoint);
			muzzlePoint[2] += pm->ps->viewheight + muzzleOffPoint[2];

			ang[0] = muzzlePoint[0] + ang[0]*2048;
			ang[1] = muzzlePoint[1] + ang[1]*2048;
			ang[2] = muzzlePoint[2] + ang[2]*2048;

			PM_Trace(&tr, muzzlePoint, NULL, NULL, ang, pm->ps->clientNum, MASK_PLAYERSOLID);

			if (tr.fraction != 1 && tr.entityNum < MAX_CLIENTS && tr.entityNum != pm->ps->clientNum)
			{
				if (pm->ps->rocketLockIndex == MAX_CLIENTS)
				{
					pm->ps->rocketLockIndex = tr.entityNum;
					pm->ps->rocketLockTime = pm->cmd.serverTime;
				}
				else if (pm->ps->rocketLockIndex != tr.entityNum && pm->ps->rocketTargetTime < pm->cmd.serverTime)
				{
					pm->ps->rocketLockIndex = tr.entityNum;
					pm->ps->rocketLockTime = pm->cmd.serverTime;
				}
				else if (pm->ps->rocketLockIndex == tr.entityNum)
				{
					if (pm->ps->rocketLockTime == -1)
					{
#if !CLIENTSIDEONLY
						pm->ps->rocketLockTime = pm->ps->rocketLastValidTime;
#endif
					}
				}

				if (pm->ps->rocketLockIndex == tr.entityNum)
				{
					pm->ps->rocketTargetTime = pm->cmd.serverTime + 500;
				}
			}
			else if (pm->ps->rocketTargetTime < pm->cmd.serverTime)
			{
				pm->ps->rocketLockIndex = MAX_CLIENTS;
				pm->ps->rocketLockTime = 0;
			}
			else
			{
				if (pm->ps->rocketLockTime != -1)
				{
#if !CLIENTSIDEONLY // meh. unlikely we will need it. if we do, we can try to fix it. rocketlastvalidtime isnt networked
					pm->ps->rocketLastValidTime = pm->ps->rocketLockTime;
#endif
				}
				pm->ps->rocketLockTime = -1;
			}
		}
		break;

	//------------------
	case WP_THERMAL_GENERAL:

		if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
		{
			altFire = qtrue; // override default of not being an alt-fire
			charging = qtrue;
		}
		else if ( pm->cmd.buttons & BUTTON_ATTACK )
		{
			charging = qtrue;
		}
		break;

	case WP_DEMP2_GENERAL:
		if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
		{
			altFire = qtrue; // override default of not being an alt-fire
			charging = qtrue;
		}
		break;

	case WP_DISRUPTOR_GENERAL:
		if ((pm->cmd.buttons & BUTTON_ATTACK) &&
			pm->ps->zoomMode == 1 &&
			pm->ps->zoomLocked)
		{
			charging = qtrue;
			altFire = qtrue;
		}

		if (pm->ps->zoomMode != 1 &&
			pm->ps->weaponstate == WEAPON_CHARGING_ALT)
		{
			pm->ps->weaponstate = WEAPON_READY;
			charging = qfalse;
			altFire = qfalse;
		}

	} // end switch


	// set up the appropriate weapon state based on the button that's down.  
	//	Note that we ALWAYS return if charging is set ( meaning the buttons are still down )
	if ( charging )
	{
		if ( altFire )
		{
			if ( pm->ps->weaponstate != WEAPON_CHARGING_ALT )
			{
				// charge isn't started, so do it now
				pm->ps->weaponstate = WEAPON_CHARGING_ALT;
				pm->ps->weaponChargeTime = pm->cmd.serverTime;
				pm->ps->weaponChargeSubtractTime = pm->cmd.serverTime + weaponData[pm->ps->weapon].altChargeSubTime;

#ifdef _DEBUG
				Com_Printf("Starting charge\n");
#endif
				assert(pm->ps->weapon > WP_NONE_GENERAL);
				BG_AddPredictableEventToPlayerstate(EV_WEAPON_CHARGE_ALT_GENERAL, pm->ps->weapon, pm->ps);
			}

			if (pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] < (weaponData[pm->ps->weapon].altChargeSub+weaponData[pm->ps->weapon].altEnergyPerShot))
			{
				pm->ps->weaponstate = WEAPON_CHARGING_ALT;

				goto rest;
			}
			else if ((pm->cmd.serverTime - pm->ps->weaponChargeTime) < weaponData[pm->ps->weapon].altMaxCharge)
			{
				if (pm->ps->weaponChargeSubtractTime < pm->cmd.serverTime)
				{
					pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] -= weaponData[pm->ps->weapon].altChargeSub;
					pm->ps->weaponChargeSubtractTime = pm->cmd.serverTime + weaponData[pm->ps->weapon].altChargeSubTime;
				}
			}
		}
		else
		{
			if ( pm->ps->weaponstate != WEAPON_CHARGING )
			{
				// charge isn't started, so do it now
				pm->ps->weaponstate = WEAPON_CHARGING;
				pm->ps->weaponChargeTime = pm->cmd.serverTime;
				pm->ps->weaponChargeSubtractTime = pm->cmd.serverTime + weaponData[pm->ps->weapon].chargeSubTime;

#ifdef _DEBUG
				Com_Printf("Starting charge\n");
#endif
				BG_AddPredictableEventToPlayerstate(EV_WEAPON_CHARGE_GENERAL, pm->ps->weapon, pm->ps);
			}

			if (pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] < (weaponData[pm->ps->weapon].chargeSub+weaponData[pm->ps->weapon].energyPerShot))
			{
				pm->ps->weaponstate = WEAPON_CHARGING;

				goto rest;
			}
			else if ((pm->cmd.serverTime - pm->ps->weaponChargeTime) < weaponData[pm->ps->weapon].maxCharge)
			{
				if (pm->ps->weaponChargeSubtractTime < pm->cmd.serverTime)
				{
					pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] -= weaponData[pm->ps->weapon].chargeSub;
					pm->ps->weaponChargeSubtractTime = pm->cmd.serverTime + weaponData[pm->ps->weapon].chargeSubTime;
				}
			}
		}

		return qtrue; // short-circuit rest of weapon code
	}
rest:
	// Only charging weapons should be able to set these states...so....
	//	let's see which fire mode we need to set up now that the buttons are up
	if ( pm->ps->weaponstate == WEAPON_CHARGING )
	{
		// weapon has a charge, so let us do an attack
#ifdef _DEBUG
		Com_Printf("Firing.  Charge time=%d\n", pm->cmd.serverTime - pm->ps->weaponChargeTime);
#endif

		// dumb, but since we shoot a charged weapon on button-up, we need to repress this button for now
		pm->cmd.buttons |= BUTTON_ATTACK;
		pm->ps->eFlags |= EF_FIRING;
	}
	else if ( pm->ps->weaponstate == WEAPON_CHARGING_ALT )
	{
		// weapon has a charge, so let us do an alt-attack
#ifdef _DEBUG
		Com_Printf("Firing.  Charge time=%d\n", pm->cmd.serverTime - pm->ps->weaponChargeTime);
#endif

		// dumb, but since we shoot a charged weapon on button-up, we need to repress this button for now
		pm->cmd.buttons |= BUTTON_ALT_ATTACK;
		pm->ps->eFlags |= (EF_FIRING|EF_ALT_FIRING);
	}

	return qfalse; // continue with the rest of the weapon code
}


#define BOWCASTER_CHARGE_UNIT	200.0f	// bowcaster charging gives us one more unit every 200ms--if you change this, you'll have to do the same in g_weapon
#define BRYAR_CHARGE_UNIT		200.0f	// bryar charging gives us one more unit every 200ms--if you change this, you'll have to do the same in g_weapon

#if !CLIENTSIDEONLY
int  PMove::PM_ItemUsable(playerState_t *ps, int forcedUse)
{
	vec3_t fwd, fwdorg, dest, pos;
	vec3_t yawonly;
	vec3_t mins, maxs;
	vec3_t trtest;
	trace_t tr;

	if (ps->usingATST)
	{
		return 0;
	}

	if (ps->pm_flags & PMF_USE_ITEM_HELD)
	{ //force to let go first
		return 0;
	}

	if (ps->duelInProgress && jk2gameplay == VERSION_1_04)
	{ //not allowed to use holdables while in a private duel.
		return 0;
	}

	if (!forcedUse)
	{
		forcedUse = bg_itemlist[ps->stats[STAT_HOLDABLE_ITEM]].giTag;
	}

	switch (forcedUse)
	{
	case HI_MEDPAC:
		if (ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH])
		{
			return 0;
		}
		if (ps->stats[STAT_HEALTH] <= 0 ||
			(ps->eFlags & EF_DEAD))
		{
			return 0;
		}

		return 1;
	case HI_SEEKER:
		if (ps->eFlags & EF_SEEKERDRONE)
		{
			PM_AddEventWithParm(EV_ITEMUSEFAIL_GENERAL, SEEKER_ALREADYDEPLOYED);
			return 0;
		}

		return 1;
	case HI_SENTRY_GUN:
		if (ps->fd.sentryDeployed)
		{
			PM_AddEventWithParm(EV_ITEMUSEFAIL_GENERAL, SENTRY_ALREADYPLACED);
			return 0;
		}

		yawonly[ROLL] = 0;
		yawonly[PITCH] = 0;
		yawonly[YAW] = ps->viewangles[YAW];

		VectorSet( mins, -8, -8, 0 );
		VectorSet( maxs, 8, 8, 24 );

		AngleVectors(yawonly, fwd, NULL, NULL);

		fwdorg[0] = ps->origin[0] + fwd[0]*64;
		fwdorg[1] = ps->origin[1] + fwd[1]*64;
		fwdorg[2] = ps->origin[2] + fwd[2]*64;

		trtest[0] = fwdorg[0] + fwd[0]*16;
		trtest[1] = fwdorg[1] + fwd[1]*16;
		trtest[2] = fwdorg[2] + fwd[2]*16;

		PM_Trace(&tr, ps->origin, mins, maxs, trtest, ps->clientNum, MASK_PLAYERSOLID);

		if ((tr.fraction != 1 && tr.entityNum != ps->clientNum) || tr.startsolid || tr.allsolid)
		{
			PM_AddEventWithParm(EV_ITEMUSEFAIL_GENERAL, SENTRY_NOROOM);
			return 0;
		}

		return 1;
	case HI_SHIELD:
		mins[0] = -8;
		mins[1] = -8;
		mins[2] = 0;

		maxs[0] = 8;
		maxs[1] = 8;
		maxs[2] = 8;

		AngleVectors (ps->viewangles, fwd, NULL, NULL);
		fwd[2] = 0;
		VectorMA(ps->origin, 64, fwd, dest);
		PM_Trace(&tr, ps->origin, mins, maxs, dest, ps->clientNum, MASK_SHOT );
		if (tr.fraction > 0.9 && !tr.startsolid && !tr.allsolid)
		{
			VectorCopy(tr.endpos, pos);
			VectorSet( dest, pos[0], pos[1], pos[2] - 4096 );
			PM_Trace( &tr, pos, mins, maxs, dest, ps->clientNum, MASK_SOLID );
			if ( !tr.startsolid && !tr.allsolid )
			{
				return 1;
			}
		}
		PM_AddEventWithParm(EV_ITEMUSEFAIL_GENERAL, SHIELD_NOROOM);
		return 0;
	default:
		return 1;
	}
}
#endif

/*
==============
PM_Weapon

Generates weapon events and modifes the weapon counter
==============
*/
 void  PMove::PM_Weapon( void )
{
	int		addTime;
	int amount;
	int		killAfterItem = 0;

	if (pm->ps->usingATST)
	{
		if ( pm->ps->weaponTime > 0 )
		{
			pm->ps->weaponTime -= pml.msec;
		}

		if (pm->ps->weaponTime < 1 && (pm->cmd.buttons & (BUTTON_ATTACK|BUTTON_ALT_ATTACK)))
		{
			pm->ps->weaponTime += 500;

			if (pm->ps->atstAltFire)
			{
				PM_AddEvent( EV_ALT_FIRE_GENERAL );
				pm->ps->atstAltFire = qfalse;
			}
			else
			{
				PM_AddEvent( EV_FIRE_WEAPON_GENERAL );
				pm->ps->atstAltFire = qtrue;
			}
		}

		return;
	}

	if (pm->ps->weapon != WP_DISRUPTOR_GENERAL && pm->ps->weapon != WP_ROCKET_LAUNCHER_GENERAL)
	{ //check for exceeding max charge time if not using disruptor or rocket launcher
		if ( pm->ps->weaponstate == WEAPON_CHARGING_ALT )
		{
			int timeDif = (pm->cmd.serverTime - pm->ps->weaponChargeTime);

			if (timeDif > MAX_WEAPON_CHARGE_TIME)
			{
				pm->cmd.buttons &= ~BUTTON_ALT_ATTACK;
			}
		}

		if ( pm->ps->weaponstate == WEAPON_CHARGING )
		{
			int timeDif = (pm->cmd.serverTime - pm->ps->weaponChargeTime);

			if (timeDif > MAX_WEAPON_CHARGE_TIME)
			{
				pm->cmd.buttons &= ~BUTTON_ATTACK;
			}
		}
	}

	if (pm->ps->forceHandExtend == HANDEXTEND_WEAPONREADY)
	{ //reset into weapon stance
		if (pm->ps->weapon != WP_SABER_GENERAL)
		{ //saber handles its own anims
			if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1)
			{
				//PM_StartTorsoAnim( TORSO_WEAPONREADY4_GENERAL );
				PM_StartTorsoAnim( TORSO_RAISEWEAP1_GENERAL);
			}
			else
			{
				if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL)
				{
					PM_StartTorsoAnim( BOTH_GUNSIT1_GENERAL );
				}
				else
				{
					//PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
					PM_StartTorsoAnim( TORSO_RAISEWEAP1_GENERAL);
				}
			}
		}

		//we now go into a weapon raise anim after every force hand extend.
		//this is so that my holster-view-weapon-when-hand-extend stuff works.
		pm->ps->weaponstate = WEAPON_RAISING;
		pm->ps->weaponTime += 250;

		pm->ps->forceHandExtend = HANDEXTEND_NONE;
	}
	else if (pm->ps->forceHandExtend != HANDEXTEND_NONE)
	{ //nothing else should be allowed to happen during this time, including weapon fire
		int desiredAnim = 0;
		qboolean seperateOnTorso = qfalse;
		int desiredOnTorso = 0;

		switch(pm->ps->forceHandExtend)
		{
		case HANDEXTEND_FORCEPUSH:
			desiredAnim = BOTH_FORCEPUSH_GENERAL;
			break;
		case HANDEXTEND_FORCEPULL:
			desiredAnim = BOTH_FORCEPULL_GENERAL;
			break;
		case HANDEXTEND_FORCEGRIP:
			desiredAnim = BOTH_FORCEGRIP_HOLD_GENERAL;
			break;
		case HANDEXTEND_SABERPULL:
			desiredAnim = BOTH_SABERPULL_GENERAL;
			break;
		case HANDEXTEND_CHOKE:
			desiredAnim = BOTH_CHOKE3_GENERAL; //left-handed choke
			break;
		case HANDEXTEND_DODGE:
			desiredAnim = pm->ps->forceDodgeAnim;
			break;
		case HANDEXTEND_KNOCKDOWN:
			if (pm->ps->forceDodgeAnim)
			{
				if (pm->ps->forceDodgeAnim > 4 && jk2gameplay == VERSION_1_04) // MVSDK: This should be enough to have the "seperateOnTorso" behaviour only with 1.04 gameplay
				{ //this means that we want to play a sepereate anim on the torso
					int originalDAnim = pm->ps->forceDodgeAnim-8; //-8 is the original legs anim
					if (originalDAnim == 2)
					{
						desiredAnim = BOTH_FORCE_GETUP_B1_GENERAL;
					}
					else if (originalDAnim == 3)
					{
						desiredAnim = BOTH_FORCE_GETUP_B3_GENERAL;
					}
					else
					{
						desiredAnim = BOTH_GETUP1_GENERAL;
					}

					//now specify the torso anim
					seperateOnTorso = qtrue;
					desiredOnTorso = BOTH_FORCEPUSH_GENERAL;
				}
				else if (pm->ps->forceDodgeAnim == 2)
				{
					desiredAnim = BOTH_FORCE_GETUP_B1_GENERAL;
				}
				else if (pm->ps->forceDodgeAnim == 3)
				{
					desiredAnim = BOTH_FORCE_GETUP_B3_GENERAL;
				}
				else
				{
					desiredAnim = BOTH_GETUP1_GENERAL;
				}
			}
			else
			{
				desiredAnim = BOTH_KNOCKDOWN1_GENERAL;
			}
			break;
		case HANDEXTEND_DUELCHALLENGE:
			desiredAnim = BOTH_ENGAGETAUNT_GENERAL;
			break;
		case HANDEXTEND_TAUNT:
			desiredAnim = pm->ps->forceDodgeAnim;
			break;
			//Hmm... maybe use these, too?
			//BOTH_FORCEHEAL_QUICK_GENERAL //quick heal (SP level 2 & 3)
			//BOTH_MINDTRICK1_GENERAL // wave (maybe for mind trick 2 & 3 - whole area, and for force seeing)
			//BOTH_MINDTRICK2_GENERAL // tap (maybe for mind trick 1 - one person)
			//BOTH_FORCEGRIP_START_GENERAL //start grip
			//BOTH_FORCEGRIP_HOLD_GENERAL //hold grip
			//BOTH_FORCEGRIP_RELEASE_GENERAL //release grip
			//BOTH_FORCELIGHTNING_GENERAL //quick lightning burst (level 1)
			//BOTH_FORCELIGHTNING_START_GENERAL //start lightning
			//BOTH_FORCELIGHTNING_HOLD_GENERAL //hold lightning
			//BOTH_FORCELIGHTNING_RELEASE_GENERAL //release lightning
		default:
			desiredAnim = BOTH_FORCEPUSH_GENERAL;
			break;
		}

		if (!seperateOnTorso)
		{ //of seperateOnTorso, handle it after setting the legs
			PM_SetAnim(SETANIM_TORSO, desiredAnim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
			pm->ps->torsoTimer = 1;
		}

		if (pm->ps->forceHandExtend == HANDEXTEND_DODGE || pm->ps->forceHandExtend == HANDEXTEND_KNOCKDOWN ||
			((pm->ps->forceHandExtend == HANDEXTEND_CHOKE && pm->ps->groundEntityNum == ENTITYNUM_NONE) && jk2gameplay != VERSION_1_02) )
		{ //special case, play dodge anim on whole body, choke anim too if off ground
			if (seperateOnTorso)
			{
				PM_SetAnim(SETANIM_LEGS, desiredAnim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
				pm->ps->legsTimer = 1;

				PM_SetAnim(SETANIM_TORSO, desiredOnTorso, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
				pm->ps->torsoTimer = 1;
			}
			else
			{
				PM_SetAnim(SETANIM_LEGS, desiredAnim, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD, 100);
				pm->ps->legsTimer = 1;
			}
		}

		return;
	}

	if (BG_InSpecialJump(pm->ps->legsAnim, pm->modParms.runFlags) ||
		BG_InRoll(pm->ps, pm->ps->legsAnim) ||
		PM_InRollComplete(pm->ps, pm->ps->legsAnim))
	{
		pm->cmd.weapon = WP_SABER_GENERAL;
		pm->ps->weapon = WP_SABER_GENERAL;
	}

	if (pm->ps->duelInProgress)
	{
		pm->cmd.weapon = WP_SABER_GENERAL;
		pm->ps->weapon = WP_SABER_GENERAL;

		if (pm->ps->duelTime >= pm->cmd.serverTime)
		{
			pm->cmd.upmove = 0;
			pm->cmd.forwardmove = 0;
			pm->cmd.rightmove = 0;
		}
	}

	if (pm->ps->weapon == WP_SABER_GENERAL && pm->ps->saberMove != LS_READY_GENERAL && pm->ps->saberMove != LS_NONE_GENERAL)
	{
		pm->cmd.weapon = WP_SABER_GENERAL; //don't allow switching out mid-attack
	}

	if (pm->ps->weapon == WP_SABER_GENERAL)
	{
		//rww - we still need the item stuff, so we won't return immediately
		PM_WeaponLightsaber();
		killAfterItem = 1;
	}
	else
	{
		pm->ps->saberHolstered = qfalse;
	}

	if (pm->ps->weapon == WP_THERMAL_GENERAL ||
		pm->ps->weapon == WP_TRIP_MINE_GENERAL ||
		pm->ps->weapon == WP_DET_PACK_GENERAL)
	{
		if (pm->ps->weapon == WP_THERMAL_GENERAL)
		{
			if ((pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == WeaponAttackAnim[pm->ps->weapon] &&
				(pm->ps->weaponTime-200) <= 0)
			{
				PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
			}
		}
		else
		{
			if ((pm->ps->torsoAnim&~ANIM_TOGGLEBIT) == WeaponAttackAnim[pm->ps->weapon] &&
				(pm->ps->weaponTime-700) <= 0)
			{
				PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
			}
		}
	}

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;
	}

	// ignore if spectator
	if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		pm->ps->weapon = WP_NONE_GENERAL;
		return;
	}

	// check for item using
	if ( pm->cmd.buttons & BUTTON_USE_HOLDABLE ) {
		if ( ! ( pm->ps->pm_flags & PMF_USE_ITEM_HELD ) ) {

			if (!pm->ps->stats[STAT_HOLDABLE_ITEM])
			{
				return;
			}

#if !CLIENTSIDEONLY
			if (!PM_ItemUsable(pm->ps, 0))
#else 
			if(1)
#endif
			{
				pm->ps->pm_flags |= PMF_USE_ITEM_HELD;
				return;
			}
			else
			{
#if !CLIENTSIDEONLY
				if (pm->ps->stats[STAT_HOLDABLE_ITEMS] & (1 << bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag))
				{
					if (bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag != HI_BINOCULARS)
					{ //never use up the binoculars
						pm->ps->stats[STAT_HOLDABLE_ITEMS] -= (1 << bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag);
					}
				}
				else
				{
					return; //this should not happen...
				}

				pm->ps->pm_flags |= PMF_USE_ITEM_HELD;
				PM_AddEvent( EV_USE_ITEM0_GENERAL + bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag );

				if (bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag != HI_BINOCULARS)
				{
					pm->ps->stats[STAT_HOLDABLE_ITEM] = 0;
					BG_CycleInven(pm->ps, 1);
				}
#endif
			}
			return;
		}
	} else {
		pm->ps->pm_flags &= ~PMF_USE_ITEM_HELD;
	}

	if (pm->ps->weapon == WP_SABER_GENERAL)
	{ //we can't toggle zoom while using saber (for obvious reasons) so make sure it's always off
		pm->ps->zoomMode = 0;
		pm->ps->zoomFov = 0;
		pm->ps->zoomLocked = qfalse;
#if !CLIENTSIDEONLY
		pm->ps->zoomLockTime = 0;
#endif
	}

	if (killAfterItem)
	{
		return;
	}

	// make weapon function
	if ( pm->ps->weaponTime > 0 ) {
		pm->ps->weaponTime -= pml.msec;
	}

	if (pm->ps->isJediMaster && pm->ps->emplacedIndex)
	{
		pm->ps->emplacedIndex = 0;
	}

	if (pm->ps->duelInProgress && pm->ps->emplacedIndex)
	{
		pm->ps->emplacedIndex = 0;
	}

	if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL && pm->ps->emplacedIndex)
	{
		pm->cmd.weapon = WP_EMPLACED_GUN_GENERAL; //No switch for you!
		PM_StartTorsoAnim( BOTH_GUNSIT1_GENERAL );
	}

	if (pm->ps->isJediMaster || pm->ps->duelInProgress || pm->ps->trueJedi)
	{
		pm->cmd.weapon = WP_SABER_GENERAL;
		pm->ps->weapon = WP_SABER_GENERAL;

		if (pm->ps->isJediMaster || pm->ps->trueJedi)
		{
			pm->ps->stats[STAT_WEAPONS] = (1 << WP_SABER_GENERAL);
		}
	}

	amount = weaponData[pm->ps->weapon].energyPerShot;

	// take an ammo away if not infinite
	if ( pm->ps->weapon != WP_NONE_GENERAL &&
		pm->ps->weapon == pm->cmd.weapon &&
		(pm->ps->weaponTime <= 0 || pm->ps->weaponstate != WEAPON_FIRING) )
	{
		if ( pm->ps->ammo[ weaponData[pm->ps->weapon].ammoIndex ] != -1 )
		{
			// enough energy to fire this weapon?
			if (pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] < weaponData[pm->ps->weapon].energyPerShot &&
				pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] < weaponData[pm->ps->weapon].altEnergyPerShot) 
			{ //the weapon is out of ammo essentially because it cannot fire primary or secondary, so do the switch
			  //regardless of if the player is attacking or not
				PM_AddEventWithParm( EV_NOAMMO_GENERAL, WP_NUM_WEAPONS_GENERAL+pm->ps->weapon );

				if (pm->ps->weaponTime < 500)
				{
					pm->ps->weaponTime += 500;
				}
				return;
			}

			if (pm->ps->weapon == WP_DET_PACK_GENERAL && !pm->ps->hasDetPackPlanted && pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] < 1)
			{
				PM_AddEventWithParm( EV_NOAMMO_GENERAL, WP_NUM_WEAPONS_GENERAL+pm->ps->weapon );

				if (pm->ps->weaponTime < 500)
				{
					pm->ps->weaponTime += 500;
				}
				return;
			}
		}
	}

	// check for weapon change
	// can't change if weapon is firing, but can change
	// again if lowering or raising
	if ( pm->ps->weaponTime <= 0 || pm->ps->weaponstate != WEAPON_FIRING ) {
		if ( pm->ps->weapon != pm->cmd.weapon ) {
			PM_BeginWeaponChange( pm->cmd.weapon );
		}
	}

	if ( pm->ps->weaponTime > 0 ) {
		return;
	}

	// change weapon if time
	if ( pm->ps->weaponstate == WEAPON_DROPPING ) {
		PM_FinishWeaponChange();
		return;
	}

	if ( pm->ps->weaponstate == WEAPON_RAISING ) {
		pm->ps->weaponstate = WEAPON_READY;
		if ( pm->ps->weapon == WP_SABER_GENERAL ) {
			PM_StartTorsoAnim( PM_GetSaberStance() );
		} else {
			if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1)
			{
				PM_StartTorsoAnim( TORSO_WEAPONREADY4_GENERAL );
			}
			else
			{
				if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL)
				{
					PM_StartTorsoAnim( BOTH_GUNSIT1_GENERAL );
				}
				else
				{
					PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
				}
			}
		}
		return;
	}

	if (((pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) == TORSO_WEAPONREADY4_GENERAL ||
		(pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) == BOTH_ATTACK4_GENERAL) &&
		(pm->ps->weapon != WP_DISRUPTOR_GENERAL || pm->ps->zoomMode != 1))
	{
		if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL)
		{
			PM_StartTorsoAnim( BOTH_GUNSIT1_GENERAL );
		}
		else
		{
			PM_StartTorsoAnim( WeaponReadyAnim[pm->ps->weapon] );
		}
	}
	else if (((pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_WEAPONREADY4_GENERAL &&
		(pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) != BOTH_ATTACK4_GENERAL) &&
		(pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1))
	{
		PM_StartTorsoAnim( TORSO_WEAPONREADY4_GENERAL );
	}


	if (pm->ps->weapon != WP_ROCKET_LAUNCHER_GENERAL)
	{
		pm->ps->rocketLockIndex = MAX_CLIENTS;
		pm->ps->rocketLockTime = 0;
		pm->ps->rocketTargetTime = 0;
	}

	if ( PM_DoChargedWeapons())
	{
		// In some cases the charged weapon code may want us to short circuit the rest of the firing code
		return;
	}

	// check for fire
	if ( ! (pm->cmd.buttons & (BUTTON_ATTACK|BUTTON_ALT_ATTACK))) 
	{
		pm->ps->weaponTime = 0;
		pm->ps->weaponstate = WEAPON_READY;
		return;
	}

	if (pm->ps->weapon == WP_EMPLACED_GUN_GENERAL)
	{
		addTime = weaponData[pm->ps->weapon].fireTime;
		pm->ps->weaponTime += addTime;
		PM_AddEvent( EV_FIRE_WEAPON_GENERAL );
		return;
	}

	if (pm->ps->weapon == WP_DISRUPTOR_GENERAL &&
		(pm->cmd.buttons & BUTTON_ALT_ATTACK) &&
		!pm->ps->zoomLocked)
	{
		return;
	}

	if (pm->ps->weapon == WP_DISRUPTOR_GENERAL &&
		(pm->cmd.buttons & BUTTON_ALT_ATTACK) &&
		pm->ps->zoomMode == 2)
	{ //can't use disruptor secondary while zoomed binoculars
		return;
	}

	if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode == 1)
	{
		PM_StartTorsoAnim( BOTH_ATTACK4_GENERAL );
	}
	/*else if ((runFlags & RFL_CLIMBTECH) && pm->ps->weapon == WP_MELEE_GENERAL)// uh is this even actually climbtech?
	{ //special anims for standard melee attacks
		//Alternate between punches and use the anim length as weapon time.
		if (!pm->ps->m_iVehicleNum)
		{ //if riding a vehicle don't do this stuff at all
			if (pm->debugMelee &&
				(pm->cmd.buttons & BUTTON_ATTACK) &&
				(pm->cmd.buttons & BUTTON_ALT_ATTACK))
			{ //ok, grapple time
#if 0 //eh, I want to try turning the saber off, but can't do that reliably for prediction..
				qboolean icandoit = qtrue;
				if (pm->ps->weaponTime > 0)
				{ //weapon busy
					icandoit = qfalse;
				}
				if (pm->ps->forceHandExtend != HANDEXTEND_NONE)
				{ //force power or knockdown or something
					icandoit = qfalse;
				}
				if (pm->ps->weapon != WP_SABER_GENERAL && pm->ps->weapon != WP_MELEE_GENERAL)
				{
					icandoit = qfalse;
				}

				if (icandoit)
				{
					//G_SetAnim(ent, &ent->client->pers.cmd, SETANIM_BOTH, BOTH_KYLE_GRAB_GENERAL, SETANIM_FLAG_OVERRIDE|SETANIM_FLAG_HOLD);
					PM_SetAnim(SETANIM_BOTH, BOTH_KYLE_GRAB_GENERAL, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD);
					if (pm->ps->torsoAnim == BOTH_KYLE_GRAB_GENERAL)
					{ //providing the anim set succeeded..
						pm->ps->torsoTimer += 500; //make the hand stick out a little longer than it normally would
						if (pm->ps->legsAnim == pm->ps->torsoAnim)
						{
							pm->ps->legsTimer = pm->ps->torsoTimer;
						}
						pm->ps->weaponTime = pm->ps->torsoTimer;
						return;
					}
				}
#else
#ifdef _GAME
				if (pm_entSelf)
				{
					if (TryGrapple((gentity_t*)pm_entSelf))
					{
						return;
					}
				}
#else
				return;
#endif
#endif
			}
			else if (pm->debugMelee && 
				(pm->cmd.buttons & BUTTON_ALT_ATTACK))
			{ //kicks
				if (!BG_KickingAnim(pm->ps->torsoAnim) &&
					!BG_KickingAnim(pm->ps->legsAnim))
				{
					int kickMove = PM_KickMoveForConditions();
					if (kickMove == LS_HILT_BASH_GENERAL)
					{ //yeah.. no hilt to bash with!
						kickMove = LS_KICK_F_GENERAL;
					}

					if (kickMove != -1)
					{
						if (pm->ps->groundEntityNum == ENTITYNUM_NONE)
						{//if in air, convert kick to an in-air kick
							float gDist = PM_GroundDistance();
							//let's only allow air kicks if a certain distance from the ground
							//it's silly to be able to do them right as you land.
							//also looks wrong to transition from a non-complete flip anim...
							if ((!BG_FlippingAnim(pm->ps->legsAnim) || pm->ps->legsTimer <= 0) &&
								gDist > 64.0f && //strict minimum
								gDist > (-pm->ps->velocity[2]) - 64.0f //make sure we are high to ground relative to downward velocity as well
								)
							{
								switch (kickMove)
								{
								case LS_KICK_F_GENERAL:
									kickMove = LS_KICK_F_AIR_GENERAL;
									break;
								case LS_KICK_B_GENERAL:
									kickMove = LS_KICK_B_AIR_GENERAL;
									break;
								case LS_KICK_R_GENERAL:
									kickMove = LS_KICK_R_AIR_GENERAL;
									break;
								case LS_KICK_L_GENERAL:
									kickMove = LS_KICK_L_AIR_GENERAL;
									break;
								default: //oh well, can't do any other kick move while in-air
									kickMove = -1;
									break;
								}
							}
							else
							{ //off ground, but too close to ground
								kickMove = -1;
							}
						}
					}

					if (kickMove != -1)
					{
						int kickAnim = saberMoveData_general[kickMove].animToUseGeneral;

						if (kickAnim != -1)
						{
							PM_SetAnim(SETANIM_BOTH, kickAnim, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD);
							if (pm->ps->legsAnim == kickAnim)
							{
								pm->ps->weaponTime = pm->ps->legsTimer;
								return;
							}
						}
					}
				}

				//if got here then no move to do so put torso into leg idle or whatever
				if (pm->ps->torsoAnim != pm->ps->legsAnim)
				{
					PM_SetAnim(SETANIM_BOTH, pm->ps->legsAnim, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD);
				}
				pm->ps->weaponTime = 0;
				return;
			}
			else
			{ //just punch
				int desTAnim = BOTH_MELEE1_GENERAL;
				if (pm->ps->torsoAnim == BOTH_MELEE1_GENERAL)
				{
					desTAnim = BOTH_MELEE2_GENERAL;
				}
				PM_StartTorsoAnim(desTAnim);

				if (pm->ps->torsoAnim == desTAnim)
				{
					pm->ps->weaponTime = pm->ps->torsoTimer;
				}
			}
		}
	}*/
	else
	{
		PM_StartTorsoAnim( WeaponAttackAnim[pm->ps->weapon] );
	}

	if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
	{
		amount = weaponData[pm->ps->weapon].altEnergyPerShot;
	}
	else
	{
		amount = weaponData[pm->ps->weapon].energyPerShot;
	}

	pm->ps->weaponstate = WEAPON_FIRING;

	// take an ammo away if not infinite
	if ( pm->ps->ammo[ weaponData[pm->ps->weapon].ammoIndex ] != -1 )
	{
		// enough energy to fire this weapon?
		if ((pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] - amount) >= 0) 
		{
			pm->ps->ammo[weaponData[pm->ps->weapon].ammoIndex] -= amount;
		}
		else	// Not enough energy
		{
			// Switch weapons
			if (pm->ps->weapon != WP_DET_PACK_GENERAL || !pm->ps->hasDetPackPlanted)
			{
				PM_AddEventWithParm( EV_NOAMMO_GENERAL, WP_NUM_WEAPONS_GENERAL+pm->ps->weapon );
				if (pm->ps->weaponTime < 500)
				{
					pm->ps->weaponTime += 500;
				}
			}
			return;
		}
	}

	if ( pm->cmd.buttons & BUTTON_ALT_ATTACK ) 	{
		if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->zoomMode != 1)
		{
			PM_AddEvent( EV_FIRE_WEAPON_GENERAL );
			addTime = weaponData[pm->ps->weapon].fireTime;
		}
		else
		{
			PM_AddEvent( EV_ALT_FIRE_GENERAL );
			addTime = weaponData[pm->ps->weapon].altFireTime;
		}
	}
	else {
		PM_AddEvent( EV_FIRE_WEAPON_GENERAL );
		addTime = weaponData[pm->ps->weapon].fireTime;
	}

	if ( pm->ps->powerups[PW_HASTE] ) {
		addTime /= 1.3;
	}

	if (pm->ps->fd.forcePowersActive & (1 << FP_RAGE))
	{
		addTime *= 0.75;
	}
	else if (pm->ps->fd.forceRageRecoveryTime > pm->cmd.serverTime)
	{
		addTime *= 1.5;
	}

	pm->ps->weaponTime += addTime;
}

/*
================
PM_Animate
================
*/
 void  PMove::PM_Animate( void ) {
	if ( pm->cmd.buttons & BUTTON_GESTURE ) {
		if ( pm->ps->torsoTimer < 1 && pm->ps->forceHandExtend == HANDEXTEND_NONE &&
			pm->ps->legsTimer < 1 && pm->ps->weaponTime < 1 && (pm->ps->saberLockTime < pm->cmd.serverTime || jk2gameplay == VERSION_1_02)) {

			pm->ps->forceHandExtend = HANDEXTEND_TAUNT;

			//FIXME: random taunt anims?
			pm->ps->forceDodgeAnim = BOTH_ENGAGETAUNT_GENERAL;

			pm->ps->forceHandExtendTime = pm->cmd.serverTime + 1000;
			
			pm->ps->weaponTime = 100;

			PM_AddEvent( EV_TAUNT_GENERAL );
		}
#if 0
// Here's an interesting bit.  The bots in TA used buttons to do additional gestures.
// I ripped them out because I didn't want too many buttons given the fact that I was already adding some for JK2.
// We can always add some back in if we want though.
	} else if ( pm->cmd.buttons & BUTTON_GETFLAG ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_GETFLAG_GENERAL );
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
	} else if ( pm->cmd.buttons & BUTTON_GUARDBASE ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_GUARDBASE_GENERAL );
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
	} else if ( pm->cmd.buttons & BUTTON_PATROL ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_PATROL_GENERAL );
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
	} else if ( pm->cmd.buttons & BUTTON_FOLLOWME ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_FOLLOWME_GENERAL );
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
	} else if ( pm->cmd.buttons & BUTTON_AFFIRMATIVE ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_AFFIRMATIVE_GENERAL);
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
	} else if ( pm->cmd.buttons & BUTTON_NEGATIVE ) {
		if ( pm->ps->torsoTimer == 0 ) {
			PM_StartTorsoAnim( TORSO_NEGATIVE_GENERAL );
			pm->ps->torsoTimer = 600;	//TIMER_GESTURE;
		}
#endif //
	}
}


/*
================
PM_DropTimers
================
*/
 void  PMove::PM_DropTimers( void ) {

	// drop misc timing counter
	if ( pm->ps->pm_time ) {
		if ( pml.msec >= pm->ps->pm_time ) {
			pm->ps->pm_flags &= ~PMF_ALL_TIMES;
			pm->ps->pm_time = 0;
		} else {
			pm->ps->pm_time -= pml.msec;
		}
	}

	// drop animation counter
	if ( pm->ps->legsTimer > 0 ) {
		pm->ps->legsTimer -= pml.msec;
		if ( pm->ps->legsTimer < 0 ) {
			pm->ps->legsTimer = 0;
		}
	}

	if ( pm->ps->torsoTimer > 0 ) {
		pm->ps->torsoTimer -= pml.msec;
		if ( pm->ps->torsoTimer < 0 ) {
			pm->ps->torsoTimer = 0;
		}
	}

	// handle bounce power
	if (pm->modParms.physics == MV_BOUNCE) {
		int bouncePower = pm->ps->stats[STAT_BOUNCEPOWER] & BOUNCEPOWER_POWERMASK;
		int bounceRegenTimer = (pm->ps->stats[STAT_BOUNCEPOWER] & BOUNCEPOWER_REGENMASK) >> 9;
		if (pm->cmd.buttons & BUTTON_BOUNCEPOWER) {
			// using bounce power. decrease it.
			bouncePower -= pml.msec;
			bounceRegenTimer = BOUNCEPOWER_REGEN_MAX;
		}
		else {
			bounceRegenTimer -= pml.msec;
			if (bounceRegenTimer <= 0) {
				bouncePower += 10;
				bounceRegenTimer = BOUNCEPOWER_REGEN_MAX;
			}
		}
		bouncePower = std::max(0,std::min(BOUNCEPOWER_MAX,bouncePower));
		bounceRegenTimer = std::max(0, std::min(BOUNCEPOWER_REGEN_MAX, bounceRegenTimer));
		pm->ps->stats[STAT_BOUNCEPOWER] = (bouncePower & BOUNCEPOWER_POWERMASK) | ((bounceRegenTimer << 9) & BOUNCEPOWER_REGENMASK);
	}
}

/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move
================
*/
void  PMove::PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd ) {
	short		temp;
	int		i;

	if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPINTERMISSION) {
		return;		// no view changes at all
	}

	if ( ps->pm_type != PM_SPECTATOR && ps->stats[STAT_HEALTH] <= 0 ) {
		return;		// no view changes at all
	}

	// circularly clamp the angles with deltas
	for (i=0 ; i<3 ; i++) {
		temp = cmd->angles[i] + ps->delta_angles[i];
		if ( i == PITCH ) {
			// don't let the player look up or down more than 90 degrees
			if ( temp > 16000 ) {
				ps->delta_angles[i] = 16000 - cmd->angles[i];
				temp = 16000;
			} else if ( temp < -16000 ) {
				ps->delta_angles[i] = -16000 - cmd->angles[i];
				temp = -16000;
			}
		}
		ps->viewangles[i] = SHORT2ANGLE(temp);
	}

}

//-------------------------------------------
void  PMove::PM_AdjustAttackStates( pmove_t *pm )
//-------------------------------------------
{
	int amount;

	// get ammo usage
	if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
	{
		amount = pm->ps->ammo[weaponData[ pm->ps->weapon ].ammoIndex] - weaponData[pm->ps->weapon].altEnergyPerShot;
	}
	else
	{
		amount = pm->ps->ammo[weaponData[ pm->ps->weapon ].ammoIndex] - weaponData[pm->ps->weapon].energyPerShot;
	}

	// disruptor alt-fire should toggle the zoom mode, but only bother doing this for the player?
	if ( pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->weaponstate == WEAPON_READY )
	{
#if !CLIENTSIDEONLY // meh
		if ( !(pm->ps->eFlags & EF_ALT_FIRING) && (pm->cmd.buttons & BUTTON_ALT_ATTACK) /*&&
			pm->cmd.upmove <= 0 && !pm->cmd.forwardmove && !pm->cmd.rightmove*/)
		{
			// We just pressed the alt-fire key
			if ( !pm->ps->zoomMode && !pm->modParms.raceMode)
			{
				// not already zooming, so do it now
				pm->ps->zoomMode = 1;
				pm->ps->zoomLocked = qfalse;
				pm->ps->zoomFov = 80.0f;//cg_fov.value;
				pm->ps->zoomLockTime = pm->cmd.serverTime + 50;
				PM_AddEvent(EV_DISRUPTOR_ZOOMSOUND_GENERAL);
			}
			else if (pm->ps->zoomMode == 1 && pm->ps->zoomLockTime < pm->cmd.serverTime)
			{ //check for == 1 so we can't turn binoculars off with disruptor alt fire
				// already zooming, so must be wanting to turn it off
				pm->ps->zoomMode = 0;
				pm->ps->zoomTime = pm->ps->commandTime;
				pm->ps->zoomLocked = qfalse;
				PM_AddEvent(EV_DISRUPTOR_ZOOMSOUND_GENERAL);
			}
		}
		else if ( !(pm->cmd.buttons & BUTTON_ALT_ATTACK ) && pm->ps->zoomLockTime < pm->cmd.serverTime)
		{
			// Not pressing zoom any more
			if ( pm->ps->zoomMode )
			{
				if (pm->ps->zoomMode == 1 && !pm->ps->zoomLocked)
				{ //approximate what level the client should be zoomed at based on how long zoom was held
					pm->ps->zoomFov = ((pm->cmd.serverTime+50) - pm->ps->zoomLockTime) * 0.035f;
					if (pm->ps->zoomFov > 50)
					{
						pm->ps->zoomFov = 50;
					}
					if (pm->ps->zoomFov < 1)
					{
						pm->ps->zoomFov = 1;
					}
				}
				// were zooming in, so now lock the zoom
				pm->ps->zoomLocked = qtrue;
			}
		}
		//This seemed like a good idea, but apparently it confuses people. So disabled for now.
		/*
		else if (!(pm->ps->eFlags & EF_ALT_FIRING) && (pm->cmd.buttons & BUTTON_ALT_ATTACK) &&
			(pm->cmd.upmove > 0 || pm->cmd.forwardmove || pm->cmd.rightmove))
		{ //if you try to zoom while moving, just convert it into a primary attack
			pm->cmd.buttons &= ~BUTTON_ALT_ATTACK;
			pm->cmd.buttons |= BUTTON_ATTACK;
		}
		*/

		if (pm->cmd.upmove > 0 || pm->cmd.forwardmove || pm->cmd.rightmove)
		{
			if (pm->ps->zoomMode == 1 && pm->ps->zoomLockTime < pm->cmd.serverTime)
			{ //check for == 1 so we can't turn binoculars off with disruptor alt fire
				pm->ps->zoomMode = 0;
				pm->ps->zoomTime = pm->ps->commandTime;
				pm->ps->zoomLocked = qfalse;
				PM_AddEvent(EV_DISRUPTOR_ZOOMSOUND_GENERAL);
			}
		}

#endif
		if ( pm->cmd.buttons & BUTTON_ATTACK )
		{
			// If we are zoomed, we should switch the ammo usage to the alt-fire, otherwise, we'll
			//	just use whatever ammo was selected from above
			if ( pm->ps->zoomMode )
			{
				amount = pm->ps->ammo[weaponData[ pm->ps->weapon ].ammoIndex] - 
							weaponData[pm->ps->weapon].altEnergyPerShot;
			}
		}
		else
		{
			// alt-fire button pressing doesn't use any ammo
			amount = 0;
		}
	}
	else if (pm->ps->weapon == WP_DISRUPTOR_GENERAL) //still perform certain checks, even if the weapon is not ready
	{
		if (pm->cmd.upmove > 0 || pm->cmd.forwardmove || pm->cmd.rightmove)
		{
#if !CLIENTSIDEONLY
			if (pm->ps->zoomMode == 1 && pm->ps->zoomLockTime < pm->cmd.serverTime)
			{ //check for == 1 so we can't turn binoculars off with disruptor alt fire
				pm->ps->zoomMode = 0;
				pm->ps->zoomTime = pm->ps->commandTime;
				pm->ps->zoomLocked = qfalse;
				PM_AddEvent(EV_DISRUPTOR_ZOOMSOUND_GENERAL);
			}
#endif
		}
	}

	// set the firing flag for continuous beam weapons, saber will fire even if out of ammo
	if ( !(pm->ps->pm_flags & PMF_RESPAWNED) && 
			pm->ps->pm_type != PM_INTERMISSION && 
			( pm->cmd.buttons & (BUTTON_ATTACK|BUTTON_ALT_ATTACK)) && 
			( amount >= 0 || pm->ps->weapon == WP_SABER_GENERAL ))
	{
		if ( pm->cmd.buttons & BUTTON_ALT_ATTACK )
		{
			pm->ps->eFlags |= EF_ALT_FIRING;
		}
		else
		{
			pm->ps->eFlags &= ~EF_ALT_FIRING;
		}

		// This flag should always get set, even when alt-firing
		pm->ps->eFlags |= EF_FIRING;
	} 
	else 
	{
		// Clear 'em out
		pm->ps->eFlags &= ~(EF_FIRING|EF_ALT_FIRING);
	}

	// disruptor should convert a main fire to an alt-fire if the gun is currently zoomed
	if ( pm->ps->weapon == WP_DISRUPTOR_GENERAL)
	{
		if ( pm->cmd.buttons & BUTTON_ATTACK && pm->ps->zoomMode == 1 && pm->ps->zoomLocked)
		{
			// converting the main fire to an alt-fire
			pm->cmd.buttons |= BUTTON_ALT_ATTACK;
			pm->ps->eFlags |= EF_ALT_FIRING;
		}
		else if ( pm->cmd.buttons & BUTTON_ALT_ATTACK && pm->ps->zoomMode == 1 && pm->ps->zoomLocked)
		{
			pm->cmd.buttons &= ~BUTTON_ALT_ATTACK;
			pm->ps->eFlags &= ~EF_ALT_FIRING;
		}
	}
}

void  PMove::BG_CmdForRoll( int anim, usercmd_t *pCmd )
{
	switch ( (anim&~ANIM_TOGGLEBIT) )
	{
	case BOTH_ROLL_F_GENERAL:
		pCmd->forwardmove = 127;
		pCmd->rightmove = 0;
		break;
	case BOTH_ROLL_B_GENERAL:
		pCmd->forwardmove = -127;
		pCmd->rightmove = 0;
		break;
	case BOTH_ROLL_R_GENERAL:
		pCmd->forwardmove = 0;
		pCmd->rightmove = 127;
		break;
	case BOTH_ROLL_L_GENERAL:
		pCmd->forwardmove = 0;
		pCmd->rightmove = -127;
		break;
	}
	pCmd->upmove = 0;
}


void  PMove::BG_AdjustClientSpeed(playerState_t *ps, usercmd_t *cmd, int svTime)
{
	//For prediction, always reset speed back to the last known server base speed
	//If we didn't do this, under lag we'd eventually dwindle speed down to 0 even though
	//that would not be the correct predicted value.
	ps->speed = ps->basespeed;

	if (ps->forceHandExtend == HANDEXTEND_DODGE)
	{
		ps->speed = 0;
	}

	if (ps->forceHandExtend == HANDEXTEND_KNOCKDOWN)
	{
		ps->speed = 0;
	}

#if !CLIENTSIDEONLY // holdmovetime isnt networked... ATST isnt that important. skip it.
	if (ps->usingATST && (cmd->rightmove ||
		cmd->forwardmove))
	{
		if (!ps->holdMoveTime)
		{
			ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
				| BOTH_RUN1START_GENERAL;
			ps->holdMoveTime = svTime;
		}
	}
	else
	{
		ps->holdMoveTime = 0;

		if (ps->usingATST)
		{
			ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
				| BOTH_STAND1_GENERAL;
		}
	}

	if (ps->usingATST &&
		((svTime - ps->holdMoveTime) < 500 ||
		!ps->holdMoveTime))
	{
		ps->speed = 0;
	}
	else if (ps->usingATST)
	{
		if ((svTime - ps->holdMoveTime) < 600)
		{
			ps->speed *= 0.4;
		}
		else if ((svTime - ps->holdMoveTime) < 1000)
		{
			ps->speed *= 0.5;
		}
		else if ((svTime - ps->holdMoveTime) < 1400)
		{
			ps->speed *= 0.6;
		}
		else if ((svTime - ps->holdMoveTime) < 1700)
		{
			ps->speed *= 0.7;
		}
		else if ((svTime - ps->holdMoveTime) < 1900)
		{
			ps->speed *= 0.8;
		}

		if (cmd->forwardmove < 0)
		{
			ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
				| BOTH_WALKBACK1_GENERAL;
			ps->speed *= 0.6;
		}
		else
		{
			ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
				| BOTH_RUN1_GENERAL;
		}
	}
	else 
#endif
	if ( cmd->forwardmove < 0 && !(cmd->buttons&BUTTON_WALKING) && pm->ps->groundEntityNum != ENTITYNUM_NONE && (jk2gameplay == VERSION_1_04 || pm->modParms.physics == MV_JK2SP) )
	{//running backwards is slower than running forwards (like SP)// TA: Actually... is this even correct?! this is in a way different place in sp. and this whole func doesnt exist there
		ps->speed *= 0.75;
	}

	if (ps->fd.forcePowersActive & (1 << FP_GRIP) && jk2gameplay != VERSION_1_02)
	{
		ps->speed *= 0.4;
	}

	if (ps->fd.forcePowersActive & (1 << FP_SPEED))
	{
		

#if CLIENTSIDEONLY
		if (pm->haveForceSpeedSmash) { // got it tunneled. can actually predict nicely then :)
			if (ps->fd.forceSpeedSmash < 1.2)
			{
				ps->fd.forceSpeedSmash = 1.2f;
			}
			if (ps->fd.forceSpeedSmash > forceSpeedLevels[3]) //2.8
			{
				ps->fd.forceSpeedSmash = forceSpeedLevels[3];
			}
		}
		else {
			//ps->fd.forceSpeedSmash = 2.0f; // not networked. just force setting to the force level 3 level. UNLESS it is tunneled somehow TODO
			//if (ps->fd.forceSpeedSmash > forceSpeedLevels[3]) //2.8
			{
				ps->fd.forceSpeedSmash = forceSpeedLevels[3]; // it's not networked so we stutter because we predict ourselves without the actual speedgain. assume level 3 i guess, should be correct in 99% of cases of actual gameplay. 
			}
		}
#else
		if (ps->fd.forceSpeedSmash < 1.2)
		{
			ps->fd.forceSpeedSmash = 1.2f;
		}
		if (ps->fd.forceSpeedSmash > forceSpeedLevels[ps->fd.forcePowerLevel[FP_SPEED]]) //2.8
		{
			ps->fd.forceSpeedSmash = forceSpeedLevels[ps->fd.forcePowerLevel[FP_SPEED]];
		}
#endif
		ps->speed *= ps->fd.forceSpeedSmash;
		ps->fd.forceSpeedSmash += 0.005f;
	}

	if (ps->fd.forcePowersActive & (1 << FP_RAGE))
	{
		ps->speed *= 1.3;
	}
	else if (ps->fd.forceRageRecoveryTime > svTime)
	{
		ps->speed *= 0.75;
	}

	if (ps->fd.forceGripCripple)
	{
		if (ps->fd.forcePowersActive & (1 << FP_RAGE))
		{
			ps->speed *= 0.9;
		}
		else if (ps->fd.forcePowersActive & (1 << FP_SPEED))
		{ //force speed will help us escape
			ps->speed *= 0.8;
		}
		else
		{
			ps->speed *= 0.2;
		}
	}

	if ( BG_SaberInAttack( ps->saberMove ) && cmd->forwardmove < 0 )
	{//if running backwards while attacking, don't run as fast.
		switch( ps->fd.saberAnimLevel )
		{
		case FORCE_LEVEL_1:
			ps->speed *= 0.75f;
			break;
		case FORCE_LEVEL_2:
			ps->speed *= 0.60f;
			break;
		case FORCE_LEVEL_3:
			ps->speed *= 0.45f;
			break;
		default:
			break;
		}
	}
	else if ( BG_SpinningSaberAnim( ps->legsAnim ) )
	{
		if (ps->fd.saberAnimLevel == FORCE_LEVEL_3 && jk2gameplay != VERSION_1_02)
		{
			ps->speed *= 0.3f;
		}
		else
		{
			ps->speed *= 0.5f;
		}
	}
	else if ( ps->weapon == WP_SABER_GENERAL && BG_SaberInAttack( ps->saberMove ) )
	{//if attacking with saber while running, drop your speed
		switch( ps->fd.saberAnimLevel )
		{
		case FORCE_LEVEL_2:
			ps->speed *= 0.85f;
			break;
		case FORCE_LEVEL_3:
			ps->speed *= (jk2gameplay == VERSION_1_02 ? 0.70f : 0.55f);
			break;
		default:
			break;
		}
	}
	else if (ps->weapon == WP_SABER_GENERAL && ps->fd.saberAnimLevel == FORCE_LEVEL_3 &&
		PM_SaberInTransition(ps->saberMove) && jk2gameplay != VERSION_1_02)
	{ //Now, we want to even slow down in transitions for level 3 (since it has chains and stuff now)
		if (cmd->forwardmove < 0)
		{
			ps->speed *= 0.4f;
		}
		else
		{
			ps->speed *= 0.6f;
		}
	}


	if ( BG_InRoll( ps, ps->legsAnim ) && ps->speed > 200 )
	{ //can't roll unless you're able to move normally
		BG_CmdForRoll( ps->legsAnim, cmd );
		if ((ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_ROLL_B_GENERAL)
		{ //backwards roll is pretty fast, should also be slower
			ps->speed = ps->legsTimer/2.5;
		}
		else
		{
			ps->speed = ps->legsTimer/1.5;//450;
		}
		if (pm->modParms.physics == MV_DREAM) {
			ps->speed *= 1.28f;
		}
		else {
			if (ps->speed > 600)
			{
				ps->speed = 600;
			}
		}
		//Automatically slow down as the roll ends.
	}
}

void PMove::PM_CheckRollEnd() {
	qboolean inRoll = BG_InRoll(pm->ps, pm->ps->legsAnim);
	int airDuration = 0;

	switch (pm->roll.status) {
		case ROLL_NONE:
			if (inRoll) {
				pm->roll.status = ROLL_STARTED;
				pm->roll.rollDisqualified = (qboolean)(pm->roll.lastSpeed > 325.0f); // seems that with normal (not ultra low) fps and vsnap we can reach a maximum groundpeed of absolutely maximally 325. so, allow rollympics participation for rolls out of standing basically.
				pm->roll.segmentDisqualified = qfalse;
				pm->roll.rollAirTime = -1;
				pm->roll.rollType = (rollType_t)((pm->ps->legsAnim & ~ANIM_TOGGLEBIT )- BOTH_ROLL_F_GENERAL);
				pm->roll.rollSpeed = 0;
				pm->roll.rollStartedInAir = (qboolean)(pm->ps->groundEntityNum == ENTITYNUM_NONE); // shouldnt really happen but lets be safe
				if (pm->debugLevel > 1) {
					Com_Printf("%i:ROLL_NONE->ROLL_STARTED\n", c_pmove);
				}
			}
			break;
		case ROLL_STARTED:
			if (!inRoll) {
				pm->roll.status = ROLL_NONE;
				if (pm->debugLevel > 1) {
					Com_Printf("%i:ROLL_STARTED->ROLL_NONE\n", c_pmove);
				}
			}
			else if (pm->roll.lastFrameWasRoll && inRoll && pm->ps->groundEntityNum == ENTITYNUM_NONE) {
				pm->roll.status = ROLL_AIR;
				pm->roll.rollAirStarted = pm->ps->commandTime;
				pm->roll.airClientSpeed = pm->roll.rollStartedInAir ? 0 : pm->roll.lastClientSpeed;
				if (pm->debugLevel > 1) {
					Com_Printf("%i:ROLL_STARTED->ROLL_AIR\n", c_pmove);
				}
			}
			break;
		case ROLL_AIR:
			airDuration = std::max(0,pm->ps->commandTime - pm->roll.rollAirStarted);
			if (!inRoll) {
				pm->roll.status = ROLL_ENDED;
				//if (pm->roll.lastSpeed > pm->roll.rollSpeed) {
				if (airDuration > pm->roll.rollAirTime) { // longest air segment counts
					pm->roll.rollSpeed = pm->roll.lastSpeed;
					pm->roll.finalAirClientSpeed = pm->roll.airClientSpeed;
					pm->roll.rollAirTime = airDuration;
					if (pm->roll.segmentDisqualified) {
						pm->roll.rollDisqualified = qtrue;
					}
					if (pm->debugLevel > 1) {
						Com_Printf("%i:ROLL_AIR->ROLL_ENDED %.2f %d (usespeed)\n", c_pmove, pm->roll.lastSpeed, pm->roll.airClientSpeed);
					}
				}
				else {
					if (pm->debugLevel > 1) {
						Com_Printf("%i:ROLL_AIR->ROLL_ENDED %.2f %d\n", c_pmove, pm->roll.lastSpeed, pm->roll.airClientSpeed);
					}
				}
				pm->roll.lastRollEndedTime = pm->roll.lastClientTime;
			}
			else if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
				pm->roll.status = ROLL_TOUCH;
				//if (pm->roll.lastSpeed > pm->roll.rollSpeed) {
				if (airDuration > pm->roll.rollAirTime) {
					pm->roll.rollSpeed = pm->roll.lastSpeed;
					pm->roll.finalAirClientSpeed = pm->roll.airClientSpeed;
					pm->roll.rollAirTime = airDuration;
					if (pm->roll.segmentDisqualified) {
						pm->roll.rollDisqualified = qtrue;
					}
					if (pm->debugLevel > 1) {
						Com_Printf("%i:ROLL_AIR->ROLL_TOUCH %.2f %d (usespeed)\n", c_pmove, pm->roll.lastSpeed, pm->roll.airClientSpeed);
					}
				}
				else {
					if (pm->debugLevel > 1) {
						Com_Printf("%i:ROLL_AIR->ROLL_TOUCH %.2f %d\n", c_pmove, pm->roll.lastSpeed, pm->roll.airClientSpeed);
					}
				}
			}
			break;
		case ROLL_TOUCH:
			if (!inRoll) {
				pm->roll.status = ROLL_ENDED;
				pm->roll.lastRollEndedTime = pm->roll.lastClientTime;
				if (pm->debugLevel > 1) {
					Com_Printf("%i:ROLL_TOUCH->ROLL_ENDED\n", c_pmove);
				}
			}
			else if (pm->ps->groundEntityNum == ENTITYNUM_NONE) {
				pm->roll.status = ROLL_AIR;
				pm->roll.rollAirStarted = pm->ps->commandTime;
				pm->roll.segmentDisqualified = qtrue;
				pm->roll.airClientSpeed = pm->roll.rollStartedInAir ? 0 : pm->roll.lastClientSpeed;
				if (pm->debugLevel > 1) {
					Com_Printf("%i:ROLL_TOUCH->ROLL_AIR\n", c_pmove);
				}
			}
			break;
		case ROLL_ENDED:
			break;
	}
}
 void  PMove::PM_SetAnimAfterQ2(vec3_t oldVel) { // idk if this is right lol

	qboolean	duck, run;
	float xyspeed = sqrtf(pm->ps->velocity[0] * pm->ps->velocity[0] + pm->ps->velocity[1] * pm->ps->velocity[1]);
	int currentAnim = (pm->ps->legsAnim & ~ANIM_TOGGLEBIT);
	int newAnim = 0;
	float delta;
	qboolean	transitionFromJump;

	if (pm->ps->pm_flags & PMF_DUCKED)
		duck = qtrue;
	else
		duck = qfalse;
	if (xyspeed)
		run = qtrue;
	else
		run = qfalse;

	if ((oldVel[2] < 0) && (pm->ps->velocity[2] > oldVel[2]) && (pm->ps->groundEntityNum == ENTITYNUM_NONE))
	{
		delta = oldVel[2];
	}
	else
	{
		if (pm->ps->groundEntityNum == ENTITYNUM_NONE) {
			delta = 0;
		}
		else {
			delta = pm->ps->velocity[2] - oldVel[2];
		}
	}
	delta = delta * delta * 0.0001;

	//// check for stand/duck and stop/go transitions
	//if (duck != client->anim_duck && client->anim_priority < ANIM_DEATH)
	//	goto newanim;
	//if (run != client->anim_run && client->anim_priority == ANIM_BASIC)
	//	goto newanim;
	//if (!ent->groundentity && client->anim_priority <= ANIM_WAVE)
	//	goto newanim;

	//if (client->anim_priority == ANIM_REVERSE)
	//{
	//	if (ent->s.frame > client->anim_end)
	//	{
	//		ent->s.frame--;
	//		return;
	//	}
	//}
	//else if (ent->s.frame < client->anim_end)
	//{	// continue an animation
	//	ent->s.frame++;
	//	return;
	//}

	//if (client->anim_priority == ANIM_DEATH)
	if (pm->ps->pm_flags & PM_DEAD)
		return;		// stay there
	//if (client->anim_priority == ANIM_JUMP)
	//{
	//	if (!ent->groundentity)
	//		return;		// stay there
	//	ent->client->anim_priority = ANIM_WAVE;
	//	ent->s.frame = FRAME_jump3;
	//	ent->client->anim_end = FRAME_jump6;
	//	return;
	//}

newanim:
	// return to either a running or standing frame
	newAnim = BOTH_WALK1_GENERAL;

	if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
		if (pm->ps->fd.forceSide) {
			transitionFromJump = qtrue;
			pm->ps->fd.forceSide = 0;
		}
		else {
			transitionFromJump = qfalse;
			pm->ps->fd.forceSide = 1;
		}
	}
	else {
		pm->ps->fd.forceSide = 0;
	}

	if (pm->ps->groundEntityNum == ENTITYNUM_NONE)
	{
		newAnim = BOTH_JUMP1_GENERAL;
	}
	else if (run)
	{	// running
		if (duck)
		{
			newAnim = BOTH_CROUCH1WALK_GENERAL;
		}
		else
		{
			newAnim = BOTH_RUN1_GENERAL;
		}
	}
	else
	{	// standing
		if (duck)
		{
			newAnim = BOTH_CROUCH1_GENERAL;
		}
		else
		{
			newAnim = BOTH_STAND1_GENERAL;
		}
	}

	if (newAnim != currentAnim && (currentAnim != BOTH_JUMP1_GENERAL || transitionFromJump)) {
		if (newAnim != BOTH_JUMP1_GENERAL && currentAnim == BOTH_JUMP1_GENERAL) {
			PM_AddEventWithParm(EV_FALL_GENERAL, delta);
		}
		PM_SetAnim(SETANIM_BOTH,newAnim,SETANIM_FLAG_NORMAL,100);
	}
	
	if (pm->cmd.buttons & BUTTON_TALK) {
		pm->ps->eFlags |= EF_TALK;
	}
	else {
		pm->ps->eFlags &= ~EF_TALK;
	}
}

void  PMove::PM_SetModData(pmove_t* pmove) {
	memset(&pmove->modParms, 0, sizeof(pmove->modParms));
	if (pmove->mod == SVMOD_TOMMYTERNAL && pmove->ps->stats[STAT_RACEMODE]) {
		pmove->modParms.raceMode = qtrue;
		pmove->modParms.physics = pmove->ps->stats[STAT_MOVEMENTSTYLE];
		pmove->modParms.runFlags = pmove->ps->stats[STAT_RUNFLAGS];
		pmove->modParms.msecRestrict = pmove->ps->stats[STAT_MSECRESTRICT];
	}
	else if (pmove->mod == SVMOD_JK2PRO) {
		pmove->modParms.raceMode = qtrue;
		pmove->modParms.physics = pmove->ps->stats[STAT_MOVEMENTSTYLE];
	}
}

/*
================
PmoveSingle

================
*/
//void trap_SnapVector( float *v );
//void PmoveQ2(pmoveq2_t* pmove);
//void PmoveCSS(pmovecss_t* pmove);
void  PMove::PmoveSingle (pmove_t *pmove) {
	int oldCmdRoll;
	pm = pmove;

	if (pm->ps->pm_type != PM_SPECTATOR && pm->ps->pm_type != PM_NOCLIP && pm->ps->pm_type != PM_INTERMISSION) {
		//if (pm->modParms.physics == MV_Q2) {
		//	pmoveq2_t pmq2;
		//	vec3_t oldVel;
		//	int oldGEN = pm->ps->groundEntityNum;
		//	memset(&pmq2, 0, sizeof(pmq2));
		//	pmq2.ps = pm->ps;
		//	pmq2.cmd = pm->cmd;
		//	pmq2.tracemask = pm->tracemask;
		//	pmq2.trace = pm->q2trace ? pm->q2trace : pm->trace;
		//	pmq2.cornerSkims = pm->q2Skims;
		//	pmq2.haveQ2StyleTrace = pm->q2TraceStyle == 2;
		//	//pmq2.trace = pm->trace;
		//	pmq2.snapinitial = pm->positionChangedOutsidePmove;
		//	pmq2.pointcontents = pm->pointcontents;
		//	pmq2.debugLevel = pm->debugLevel;
		//	pmq2.antiLoop = &pm->antiLoop;
		//	VectorCopy(pm->mins, pmq2.mins);
		//	VectorCopy(pm->maxs, pmq2.maxs);
		//	VectorCopy(pm->ps->velocity, oldVel);
		//	c_pmove++;
		//	PmoveQ2(&pmq2);
		//	PM_SetMovementDir();
		//	if (oldGEN != ENTITYNUM_NONE && pm->ps->groundEntityNum == ENTITYNUM_NONE && pm->cmd.upmove > 0) {
		//		PM_AddEvent(EV_JUMP_GENERAL);
		//	}
		//	PM_SetAnimAfterQ2(oldVel);
		//	pm->ps->commandTime = pm->cmd.serverTime;
		//	VectorCopy(pmq2.mins, pm->mins);
		//	VectorCopy(pmq2.maxs, pm->maxs);
		//	return;
		//}
		//else if (pm->modParms.physics == MV_CSS) {
		//	pmovecss_t pmcss;
		//	vec3_t oldVel;
		//	memset(&pmcss, 0, sizeof(pmcss));
		//	pmcss.ps = pm->ps;
		//	pmcss.cmd = pm->cmd;
		//	pmcss.tracemask = pm->tracemask;
		//	pmcss.trace = pm->trace;
		//	pmcss.snapinitial = pm->positionChangedOutsidePmove;
		//	pmcss.pointcontents = pm->pointcontents;
		//	pmcss.antiLoop = &pm->antiLoop;
		//	VectorCopy(pm->mins, pmcss.mins);
		//	VectorCopy(pm->maxs, pmcss.maxs);
		//	pmcss.oldbuttons = pm->oldButtons;
		//	VectorCopy(pm->ps->velocity, oldVel);
		//	c_pmove++;
		//	PmoveCSS(&pmcss);
		//	PM_SetMovementDir();
		//	PM_SetAnimAfterQ2(oldVel);
		//	pm->ps->commandTime = pm->cmd.serverTime;
		//	VectorCopy(pmcss.mins, pm->mins);
		//	VectorCopy(pmcss.maxs, pm->maxs);
		//	return;
		//}
	}

	gPMDoSlowFall = PM_DoSlowFall();

	oldCmdRoll = pm->cmd.angles[ROLL];
	if (pm->modParms.runFlags & RFL_BOT) {
		pm->cmd.angles[ROLL] = 0;
	}

	// this counter lets us debug movement problems with a journal
	// by setting a conditional breakpoint fot the previous frame
	c_pmove++;

	// clear results
	pm->numtouch = 0;
	pm->watertype = 0;
	pm->waterlevel = 0;

	if (pm->ps->pm_type == PM_FLOAT)
	{ //You get no control over where you go in grip movement
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if (pm->ps->eFlags & EF_DISINTEGRATION)
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if ( pm->ps->saberMove == LS_A_LUNGE_GENERAL )
	{//can't move during lunge
		pm->cmd.rightmove = pm->cmd.upmove = 0;
		if ( pm->ps->legsTimer > 500 )
		{
			pm->cmd.forwardmove = 127;
		}
		else
		{
			pm->cmd.forwardmove = 0;
		}
	}

	if ( pm->ps->saberMove == LS_A_JUMP_T__B__GENERAL )
	{//can't move during leap
		if ( pm->ps->groundEntityNum != ENTITYNUM_NONE )
		{//hit the ground
			pm->cmd.forwardmove = 0;
		}
		pm->cmd.rightmove = pm->cmd.upmove = 0;
	}

	if ( pm->ps->saberMove == LS_A_BACK_GENERAL || pm->ps->saberMove == LS_A_BACK_CR_GENERAL 
		|| pm->ps->saberMove == LS_A_BACKSTAB_GENERAL || pm->ps->saberMove == LS_A_FLIP_STAB_GENERAL ||
		pm->ps->saberMove == LS_A_FLIP_SLASH_GENERAL || pm->ps->saberMove == LS_A_JUMP_T__B__GENERAL )
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if ((pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_A2_STABBACK1_GENERAL) ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_ATTACK_BACK_GENERAL) ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_CROUCHATTACKBACK1_GENERAL) ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_FORCELEAP2_T__B__GENERAL) ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_JUMPFLIPSTABDOWN_GENERAL) ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == (BOTH_JUMPFLIPSLASHDOWN1_GENERAL))
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if (((pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_KISSER1LOOP_GENERAL ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_KISSEE1LOOP_GENERAL) && jk2gameplay == VERSION_1_04)
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if (pm->ps->emplacedIndex)
	{
		if (pm->cmd.forwardmove < 0)
		{
			pm->ps->emplacedIndex = 0;
		}
		else
		{
			pm->cmd.forwardmove = 0;
			pm->cmd.rightmove = 0;
			pm->cmd.upmove = 0;
		}
	}

	if (pm->ps->weapon == WP_DISRUPTOR_GENERAL && pm->ps->weaponstate == WEAPON_CHARGING_ALT)
	{ //not allowed to move while charging the disruptor
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		if (pm->cmd.upmove > 0)
		{
			pm->cmd.upmove = 0;
		}
	}

	pm->roll.lastSpeed = XYSPEED(pm->ps->velocity);
	pm->roll.lastClientSpeed = pm->ps->speed;
	pm->roll.lastFrameWasRoll = BG_InRoll(pm->ps, pm->ps->legsAnim);
	pm->roll.lastClientTime = pm->ps->commandTime;

	BG_AdjustClientSpeed(pm->ps, &pm->cmd, pm->cmd.serverTime);

	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		pm->tracemask &= ~CONTENTS_BODY;	// corpses can fly through bodies
	}

	// make sure walking button is clear if they are running, to avoid
	// proxy no-footsteps cheats
	if ( abs( pm->cmd.forwardmove ) > 64 || abs( pm->cmd.rightmove ) > 64 ) {
		pm->cmd.buttons &= ~BUTTON_WALKING;
	}

	// set the talk balloon flag
	if ( pm->cmd.buttons & BUTTON_TALK ) {
		pm->ps->eFlags |= EF_TALK;
	} else {
		pm->ps->eFlags &= ~EF_TALK;
	}

	// In certain situations, we may want to control which attack buttons are pressed and what kind of functionality
	//	is attached to them
	PM_AdjustAttackStates( pm );

	// clear the respawned flag if attack and use are cleared
	if ( pm->ps->stats[STAT_HEALTH] > 0 && 
		!( pm->cmd.buttons & (BUTTON_ATTACK | BUTTON_USE_HOLDABLE) ) ) {
		pm->ps->pm_flags &= ~PMF_RESPAWNED;
	}

	// if talk button is down, dissallow all other input
	// this is to prevent any possible intercept proxy from
	// adding fake talk balloons
#if CLIENTSIDEONLY
	if ( pmove->cmd.buttons & BUTTON_TALK && jk2gameplay != VERSION_1_02 ) { // MVSDK: 1.02 people are used to walk around with open console, 1.03 and 1.04 can't do that. Let's make this depending on the actual version we're running, not the gameplay...
#else
	if ( pmove->cmd.buttons & BUTTON_TALK && jk2startversion != VERSION_1_02 ) { // MVSDK: 1.02 people are used to walk around with open console, 1.03 and 1.04 can't do that. Let's make this depending on the actual version we're running, not the gameplay...
#endif
		// keep the talk button set tho for when the cmd.serverTime > 66 msec
		// and the same cmd is used multiple times in Pmove
		pmove->cmd.buttons = BUTTON_TALK;
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
		pmove->cmd.upmove = 0;
	}

	// clear all pmove local vars
	memset (&pml, 0, sizeof(pml));

	pml.randomAdd = pmove->unlockRandom ? 1 : 0;

	// determine the time
	pml.seed = pmove->cmd.serverTime;
	pml.msec = pmove->cmd.serverTime - pm->ps->commandTime;
	if ( pml.msec < 1 ) {
		pml.msec = 1;
	} else if ( pml.msec > 200 && (pml.msec > pm->modParms.msecRestrict && pm->modParms.msecRestrict != -1)) { // racemode can allow higher for shits and giggles
		pml.msec = 200;
	}
	pm->ps->commandTime = pmove->cmd.serverTime;


	// save old org in case we get stuck
	VectorCopy (pm->ps->origin, pml.previous_origin);

	// save old velocity for crashlanding
	VectorCopy (pm->ps->velocity, pml.previous_velocity);

	pml.frametime = pml.msec * 0.001;

	if (pm->modParms.runFlags & RFL_CLIMBTECH) {
		PM_AdjustAngleForWallJump(pm->ps, &pm->cmd, qtrue);
	}
	PM_AdjustAngleForWallRun(pm->ps, &pm->cmd, qtrue);

	if ((pm->ps->saberMove == LS_A_JUMP_T__B__GENERAL || pm->ps->saberMove == LS_A_LUNGE_GENERAL ||
		((pm->ps->saberMove == LS_A_BACK_CR_GENERAL || pm->ps->saberMove == LS_A_BACK_GENERAL ||
		pm->ps->saberMove == LS_A_BACKSTAB_GENERAL) && jk2gameplay == VERSION_1_04)) && jk2gameplay != VERSION_1_02) // MVSDK: One of the place where 1.02, 1.03 and 1.04 are all different!
	{
		PM_SetPMViewAngle(pm->ps, pm->ps->viewangles, &pm->cmd);
	}

	if (((pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_KISSER1LOOP_GENERAL ||
		(pm->ps->legsAnim&~ANIM_TOGGLEBIT) == BOTH_KISSEE1LOOP_GENERAL) && jk2gameplay == VERSION_1_04)
	{
		pm->ps->viewangles[PITCH] = 0;
		PM_SetPMViewAngle(pm->ps, pm->ps->viewangles, &pm->cmd);
	}

	// update the viewangles
	PM_UpdateViewAngles( pm->ps, &pm->cmd );

	if (pm->modParms.runFlags & RFL_BOT) {
		// in strafebot mode, we tunnel strafebot factor through ROLL value
		float oldRoll = pm->ps->viewangles[ROLL];
		pm->ps->viewangles[ROLL] = 0;
		AngleVectors(pm->ps->viewangles, pml.forward, pml.right, pml.up);
		pm->ps->viewangles[ROLL] = oldRoll;
	}
	else {

#ifdef JK2_GAME
		if ( g_mv_blockspeedhack.integer )
		{
			float oldRoll = pm->ps->viewangles[ROLL];
			pm->ps->viewangles[ROLL] = 0;
			AngleVectors (pm->ps->viewangles, pml.forward, pml.right, pml.up);
			pm->ps->viewangles[ROLL] = oldRoll;
		}
		else
		{
#endif
		AngleVectors (pm->ps->viewangles, pml.forward, pml.right, pml.up);
#ifdef JK2_GAME
		}
#endif
	}

	if ( pm->cmd.upmove < 10 && (!(pm->modParms.runFlags & RFL_CLIMBTECH) || !(pm->ps->pm_flags & PMF_STUCK_TO_WALL))) {
		// not holding jump
		pm->ps->pm_flags &= ~PMF_JUMP_HELD;
	}

	// decide if backpedaling animations should be used
	if ( pm->cmd.forwardmove < 0 ) {
		pm->ps->pm_flags |= PMF_BACKWARDS_RUN;
	} else if ( pm->cmd.forwardmove > 0 || ( pm->cmd.forwardmove == 0 && pm->cmd.rightmove ) ) {
		pm->ps->pm_flags &= ~PMF_BACKWARDS_RUN;
	}

	if ( pm->ps->pm_type >= PM_DEAD ) {
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if (pm->ps->saberLockTime >= pm->cmd.serverTime)
	{
		pm->cmd.upmove = 0;
		pm->cmd.forwardmove = 50;
		pm->cmd.rightmove = 0;//*= 0.1;
	}

	if ( pm->ps->pm_type == PM_SPECTATOR ) {
		PM_CheckDuck ();
		PM_FlyMove ();
		PM_DropTimers ();
		if (pm->modParms.runFlags & RFL_BOT) {
			pm->cmd.angles[ROLL] = oldCmdRoll;
		}
		return;
	}

	if ( pm->ps->pm_type == PM_NOCLIP ) {
		PM_NoclipMove ();
		PM_DropTimers (); 
		if (pm->modParms.runFlags & RFL_BOT) {
			pm->cmd.angles[ROLL] = oldCmdRoll;
		}
		return;
	}

	if (pm->ps->pm_type == PM_FREEZE) {
		if (pm->modParms.runFlags & RFL_BOT) {
			pm->cmd.angles[ROLL] = oldCmdRoll;
		}
		return;		// no movement at all
	}

	if ( pm->ps->pm_type == PM_INTERMISSION || pm->ps->pm_type == PM_SPINTERMISSION) {
		if (pm->modParms.runFlags & RFL_BOT) {
			pm->cmd.angles[ROLL] = oldCmdRoll;
		}
		return;		// no movement at all
	}

	if (gPMDoSlowFall)
	{
		pm->ps->gravity *= 0.5;
	}

	// set watertype, and waterlevel
	PM_SetWaterLevel();
	pml.previous_waterlevel = pmove->waterlevel;

	// set mins, maxs, and viewheight
	PM_CheckDuck ();

	// set groundentity
	PM_GroundTrace();

	if ( pm->ps->groundEntityNum != ENTITYNUM_NONE )
	{//on ground
		pm->ps->fd.forceJumpZStart = 0;
	}

	if ( pm->ps->pm_type == PM_DEAD ) {
		PM_DeadMove ();
	}

	PM_DropTimers();


	// TODO MAYBE jaPRO fix strafebot up.
	if (pm->modParms.raceMode && pm->ps->pm_type == PM_NORMAL && pm->cmd.buttons & BUTTON_STRAFEBOT) {
		if (pm->ps->clientNum >= 0 && pm->ps->clientNum < MAX_CLIENTS && (pm->modParms.runFlags & RFL_BOT))
		{
			float realCurrentSpeed = sqrtf((pm->ps->velocity[0] * pm->ps->velocity[0]) + (pm->ps->velocity[1] * pm->ps->velocity[1]));
			if (realCurrentSpeed > 0) {
				vec3_t vel = { 0 }, velangle;
				float optimalAngle1 = 0; // option A
				float optimalAngle2 = 0; // option B
				float optimalDeltaAngle = 0;
				qboolean CJ = qtrue;
				float realFriction = MovementIsQuake3Based(pm->modParms.physics) ? pm_vq3_friction : pm_friction;
				float realAccel = MovementIsQuake3Based(pm->modParms.physics) ? pm_cpm_accelerate : (pm->modParms.physics == MV_JK2SP ? pm_sp_accelerate : pm_accelerate);
				float strafeFactor = fp16_ieee_to_fp32_value(USHORT2SHORT(oldCmdRoll))+1.0f;  // USHORT2SHORT to normalize to short range since fp16 conversion relies on it
				if (pm->ps->groundEntityNum != ENTITYNUM_WORLD || (pm->cmd.upmove > 0 && !(pm->ps->pm_flags & PMF_JUMP_HELD))) {
					realAccel = pm->modParms.physics == MV_JK2SP ? pm_sp_airaccelerate : pm_airaccelerate;
					CJ = qfalse;
				}
				//else if (moveStyle == MV_SLICK)
				//	CJ = qfalse;
				else if (pml.walking && pml.groundTrace.surfaceFlags & SURF_SLICK) { //Lmao fuck this bullshit. no way to tell if we are on slick i guess.
					if (!MovementIsQuake3Based(pm->modParms.physics)) {
						realAccel = pm->modParms.physics == MV_JK2SP ? pm_sp_airaccelerate : pm_airaccelerate;
					}
					realFriction = 0;
				}
				//else if (realCurrentSpeed > pm->ps->basespeed * 1.5f) //idk this is retarded, but lets us groundframe (TA: WHAT?)
				//	CJ = qfalse;

				if (realCurrentSpeed > pm->ps->basespeed || (CJ && (realCurrentSpeed > (pm->ps->basespeed * 0.5f)))) {
					float middleOffset = 0; //Idk
					qboolean calculationFailed = qfalse;
					qboolean wSuggestsRightWard;

					vel[0] = pm->ps->velocity[0];
					vel[1] = pm->ps->velocity[1];
					vectoangles(vel, velangle); 
					wSuggestsRightWard = (qboolean)(AngleSubtract(velangle[YAW], pm->ps->viewangles[YAW]) > 0);

					if (CJ) {//CJ)
						qboolean doSlopes = pm->handleStrafebotSlopes; // so i can change it in debugger. HEHE
						qboolean isSlope = (qboolean)(pml.groundTrace.plane.normal[2] != 1.0f);
						//if (moveStyle == MV_CPM || moveStyle == MV_RJCPM || moveStyle == MV_BOTCPM)
						//	optimalDeltaAngle = -1; //CJ //Take into account ground accel/friction.. only cpm styles turn faster?
						//else
							//optimalDeltaAngle = -6;

						if (isSlope && pm->cmd.forwardmove > 0) {
							pm->cmd.rightmove = 0; // slopes make us slower if we dont go W only!!! (allow A/D tho)
						}
						if (isSlope && doSlopes) {
							// sloped ground behaves differently. simulate the projection onto the slope from walkmove
							// maybe todo: duckscale
							int i;
							vec3_t velNorm,forwardTmp,rightTmp, forwardFlat, accelVec;
							float		angle,angle2,angleTmp,angleTmp2; 
							float		inverseAngleScaleFactor;
							float		overbounce = MovementOverbounceFactor(pm->modParms.physics, pm->ps, &pm->cmd);
							float		forward, right;
							float deprojectFactor;

							realCurrentSpeed = VectorLength(pm->ps->velocity); //sloped ground movement needs 3d speed
							angleTmp = acos((double)((pm->ps->speed - (realAccel * pm->ps->speed * pml.frametime * strafeFactor)) / (realCurrentSpeed * (1 - realFriction * (pml.frametime)))));

							if (fpclassify(angleTmp) == FP_NAN) {
								calculationFailed = qtrue;
								optimalDeltaAngle = 0;
							}
							else {

								VectorCopy(pm->ps->velocity, velNorm);
								VectorNormalize(velNorm);
								VectorCopy(velNorm, forwardTmp);
								//AngleVectors(velNorm,forwardTmp,0,0);

								// project moves down to flat plane
								//forwardTmp[2] = 0;

								// project the forward and right directions onto the ground plane
								PM_ClipVelocity(forwardTmp, pml.groundTrace.plane.normal, forwardTmp, overbounce);
								//
								VectorNormalize(forwardTmp);
								// when going up or down slopes the wish velocity should Not be zero

								angle = DotProduct(forwardTmp, velNorm);
								angle = std::max(-1.0f, std::min(1.0f, angle)); // floating point imprecision can lead to 1.00000002f (not exactly that, just the idea) and then acos is NaN
								angle = acos(angle);

								if (angle >= angleTmp) {
									angleTmp2 = 0;
								}
								else {
									angleTmp2 = sqrtf(angleTmp * angleTmp - angle* angle);
								}

								// this is the optimal delta angle ON the slope plane
								// now we need to translate it to our horizontal angle

								CrossProduct(forwardTmp, pml.groundTrace.plane.normal, rightTmp);

								forward = cos(angleTmp2);
								right = sin(angleTmp2);
								right *= ((wSuggestsRightWard || pm->unalteredCmd.rightmove > 0) && pm->unalteredCmd.rightmove >= 0) ? 1.0f : -1.0f;

								VectorScale(forwardTmp, forward, accelVec);
								VectorMA(accelVec, right, rightTmp, accelVec);

								//accelVec[2] = 0;
								deprojectFactor = accelVec[2] / pml.groundTrace.plane.normal[2];
								VectorMA(accelVec, -deprojectFactor, pml.groundTrace.plane.normal, accelVec);

								VectorNormalize(accelVec);

								VectorCopy(velNorm, forwardTmp);
								forwardTmp[2] = 0;
								VectorNormalize(forwardTmp);


								angle2 = DotProduct(accelVec, forwardTmp);
								angle2 = std::max(-1.0f, std::min(1.0f, angle2));

								optimalDeltaAngle = acos(angle2);

								if (fpclassify(optimalDeltaAngle) == FP_NAN) {
									calculationFailed = qtrue;
									optimalDeltaAngle = 0;
								}
							}
						}
						else {
							optimalDeltaAngle = acos((double)((pm->ps->speed - (realAccel * pm->ps->speed * pml.frametime * strafeFactor)) / (realCurrentSpeed * (1 - realFriction * (pml.frametime)))));
						}
						optimalDeltaAngle = optimalDeltaAngle * (180.0f / M_PI) - 45.0f;
					}
					else {
						//if (moveStyle == MV_SP)
						//	realAccel = pm_sp_airaccelerate;
						//else if (moveStyle == MV_SLICK)
						//	realAccel = pm_slick_accelerate;
						//jetpack. 1.4f ?
						optimalDeltaAngle = (acos((double)((pm->ps->speed - (realAccel * pm->ps->speed * pml.frametime * strafeFactor)) / realCurrentSpeed)) * (180.0f / M_PI) - 45.0f);
					}
					if (/*optimalDeltaAngle < 0 || optimalDeltaAngle > 360 || */fpclassify(optimalDeltaAngle) == FP_NAN) {
						calculationFailed = qtrue;
						optimalDeltaAngle = 0;

						if (pmove->debugLevel > 20) {

							Com_Printf("strafebot: optimalDeltaAngle is NAN", pmove->accelMiss);
						}

					}

					if (!calculationFailed && (!pm->isSpecialPredict || strafeFactor >= 1.0f && (strafeFactor <= 1.1f || strafeFactor <= 2.0f && !CJ))) { // strafe factors create stuttering with special predict unless very low. might have to adjust this when cpm etc. in air it can tolerate a bit more
						optimalDeltaAngle = AngleNormalize180(optimalDeltaAngle);

						if (pm->cmd.forwardmove > 0 && pm->cmd.rightmove > 0) {//WD
							optimalDeltaAngle = 0 - optimalDeltaAngle;
						}
						else if (pm->cmd.forwardmove > 0 && pm->cmd.rightmove < 0) {//WA
							optimalDeltaAngle = 0 + optimalDeltaAngle;
						}
						else if (!pm->cmd.forwardmove && pm->cmd.rightmove > 0) {//D
							//if (moveStyle == MV_QW || moveStyle == MV_CPM || moveStyle == MV_PJK || moveStyle == MV_WSW || moveStyle == MV_RJCPM || moveStyle == MV_BOTCPM)
							//	optimalDeltaAngle = 0 - middleOffset; //Take into account speed.
							//else
							//optimalDeltaAngle = 45 - optimalDeltaAngle;
							//if ((AngleSubtract(velangle[YAW], pm->ps->viewangles[YAW] + 90.0f) < 0 || pm->unalteredCmd.forwardmove > 0) && pm->unalteredCmd.forwardmove <= 0) {
							if (pm->unalteredCmd.forwardmove <= 0) { // only do backwards strafe if explicitly W is pressed during roll
								optimalDeltaAngle = 45 - optimalDeltaAngle;
							}
							else {
								optimalDeltaAngle = 135.0f + optimalDeltaAngle;
							}
						}
						else if (!pm->cmd.forwardmove && pm->cmd.rightmove < 0) {//A
							//if (moveStyle == MV_QW || moveStyle == MV_CPM || moveStyle == MV_PJK || moveStyle == MV_WSW || moveStyle == MV_RJCPM || moveStyle == MV_BOTCPM)
							//	optimalDeltaAngle = 0 + middleOffset;
							//else
							//optimalDeltaAngle = -45 + optimalDeltaAngle;
							//if ((AngleSubtract(velangle[YAW], pm->ps->viewangles[YAW]-90.0f) < 0 || pm->unalteredCmd.forwardmove > 0) && pm->unalteredCmd.forwardmove <= 0) {
							if (pm->unalteredCmd.forwardmove <= 0) { // only do backwards strafe if explicitly W is pressed during roll
								optimalDeltaAngle = -45 + optimalDeltaAngle;
							}
							else { 
								optimalDeltaAngle = 225.0f - optimalDeltaAngle; 
							}
						}
						else if (pm->cmd.forwardmove > 0 && !pm->cmd.rightmove) {//W
							//optimalAngle1 = abs(AngleSubtract(velangle[YAW] - 45 - optimalDeltaAngle, pm->ps->viewangles[YAW]));
							//optimalAngle2 = abs(AngleSubtract(velangle[YAW] + 45 + optimalDeltaAngle, pm->ps->viewangles[YAW]));
							if ((wSuggestsRightWard || pm->unalteredCmd.rightmove > 0) && pm->unalteredCmd.rightmove >= 0) { //Decide which W we want.  (Whatever is closest). this is broken for CJ (just walking basically). not sure i can fix it, but its not a huge deal i guess
							//if ((optimalAngle1 < optimalAngle2 || pm->unalteredCmd.rightmove > 0) && pm->unalteredCmd.rightmove >= 0) { //Decide which W we want.  (Whatever is closest)
								//if (moveStyle == MV_QW || moveStyle == MV_CPM || moveStyle == MV_PJK || moveStyle == MV_WSW || moveStyle == MV_RJCPM || moveStyle == MV_BOTCPM) //Why the f does it switch
								//	optimalDeltaAngle = -45; //Needs good offset
								//else
								optimalDeltaAngle = -45 - optimalDeltaAngle; //rightwards
							}
							else { //Right side
								//if (moveStyle == MV_QW || moveStyle == MV_CPM || moveStyle == MV_PJK || moveStyle == MV_WSW || moveStyle == MV_RJCPM || moveStyle == MV_BOTCPM)
								//	optimalDeltaAngle = 45; //Needs good offset
								//else
								optimalDeltaAngle = 45 + optimalDeltaAngle; //leftwards
							}
						}
						else if (pm->cmd.forwardmove < 0 && !pm->cmd.rightmove) {//S // TODO
							//if ((AngleSubtract(velangle[YAW], pm->ps->viewangles[YAW]-180.0f) > 0 || pm->unalteredCmd.rightmove > 0) && pm->unalteredCmd.rightmove >= 0) { 
							//	optimalDeltaAngle = -45 - optimalDeltaAngle; //rightwards
							//}
							//else { 
							//	optimalDeltaAngle = 45 + optimalDeltaAngle; //leftwards
							//}
						}

						velangle[YAW] += optimalDeltaAngle;
						velangle[PITCH] = pm->ps->viewangles[PITCH];

						//assert(!_isnanf(velangle[PITCH]));
						//assert(!_isnanf(velangle[YAW]));
						//assert(!_isnanf(velangle[ROLL]));

						PM_SetPMViewAngle(pm->ps, velangle, &pm->cmd);
						AngleVectors(pm->ps->viewangles, pml.forward, pml.right, pml.up); //Have to re set this here
					}
				}
			}
		}
	}

	if (pm->ps->pm_type == PM_FLOAT)
	{
		PM_FlyMove ();
	}
	else if((!pm->requiredCmdMsec || pml.msec == pm->requiredCmdMsec) && (pm->isSpecialPredict || pm->modParms.msecRestrict <= 0 || pm->modParms.msecRestrict == pml.msec))
	{
		if (pm->modParms.physics == MV_CHARGEJUMP) {
			PM_CheckChargeJump();
		}
		if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) {
			PM_WaterJumpMove();
		} else if ( pm->waterlevel > 1 ) {
			// swimming
			PM_WaterMove();
		} else if ( pml.walking ) {
			// walking on ground
			PM_WalkMove();
		} else {
			// airborne
			PM_AirMove();
		}
	}

	PM_Animate();

	// set groundentity, watertype, and waterlevel
	PM_GroundTrace();
	PM_SetWaterLevel();

	PM_CheckRollEnd();


	if (pm->cmd.forcesel != (byte)-1 && (pm->ps->fd.forcePowersKnown & (1 << pm->cmd.forcesel)))
	{
		pm->ps->fd.forcePowerSelected = pm->cmd.forcesel;
	}
#if !CLIENTSIDEONLY // meh
	if (pm->cmd.invensel != (byte)-1 && (pm->ps->stats[STAT_HOLDABLE_ITEMS] & (1 << pm->cmd.invensel)))
	{
		pm->ps->stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(pm->cmd.invensel, IT_HOLDABLE);
	}
#endif

	// weapons
	PM_Weapon();

	PM_Use();

	// footstep events / legs animations
	PM_Footsteps();

	// entering / leaving water splashes
	PM_WaterEvents();

	//Walbug fix start, if getting stuck w/o noclip is even possible.  This should maybe be after round float? im not sure..
	// TODO MAYBE jaPRO this actually kills strafing on yavin. find better solution.
	//if ((pm->ps->persistant[PERS_TEAM] != TEAM_SPECTATOR) && pm->ps->stats[STAT_RACEMODE] && VectorCompare(pm->ps->origin, pml.previous_origin) /*&& (VectorLengthSquared(pm->ps->velocity) > VectorLengthSquared(pml.previous_velocity))*/)
	//	VectorClear(pm->ps->velocity); //Their velocity is increasing while their origin is not moving (wallbug), so prevent this..
		//VectorCopy(pml.previous_velocity, pm->ps->velocity);
	//To fix rocket wallbug, since that gets applied elsewhere, just always reset vel if origins dont match?
	//Wallbug fix end

	// snap some parts of playerstate to save network bandwidth
	if (pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		SnapVector( pm->ps->velocity );
	}
	else {
		if (/*pm->modParms.raceMode || */pm->pmove_float > 2  && !pm->modParms.raceMode || pm->modParms.msecRestrict == -2) {
		}
		else if (pm->highFpsFix
			&& !pm->modParms.raceMode
			&& (pml.msec <= 4 || pml.msec > 25)
			|| pm->requiredCmdMsec && pm->requiredCmdMsec != pml.msec
			|| !pm->isSpecialPredict && pm->modParms.msecRestrict > 0 && pm->modParms.msecRestrict != pml.msec
			) { //do nothing above 250FPS or below 40FPS, or if a certain msec timing is demanded by the game via requiredCmdMsec (used for toggle limiting via g_fpsToggleDelay)
		}
		else if (pm->pmove_float == 2 && !pm->modParms.raceMode) { //pmove_float 2: snaps vertical velocity only, so 125/142fps jumps are still the same height?
			// TODO allow this option in racemode somehow too?
			vec3_t oldVelocity = { 0 };
			VectorCopy( pm->ps->velocity, oldVelocity );
			SnapVector( pm->ps->velocity );
			pm->ps->velocity[2] = oldVelocity[2];
		}
		else if (!pm->pmove_float || pm->modParms.raceMode && pm->modParms.msecRestrict > -2) {
			SnapVector( pm->ps->velocity );
		}
	}

	if (gPMDoSlowFall)
	{
		pm->ps->gravity *= 2;
	}

	if (pm->modParms.runFlags & RFL_BOT) {
		pm->cmd.angles[ROLL] = oldCmdRoll;
	}
}

//extern Q_INLINE int PM_GetRaceMode(pmove_t* pmove);

/*
================
Pmove

Can be called by either the server or the client
================
*/
void  PMove::Pmove (pmove_t *pmove) {
	int			finalTime;
	int			zeroahaha = 0.0f;

	PM_SetModData(pmove);

	finalTime = pmove->cmd.serverTime;

	VectorCopy(pmove->ps->velocity,pmove->lastAntiLoopVelocity);

#ifdef Q3_VM
	pmove->accelMiss =0.0f/0.0f; // putting NaN in there.
#else
	pmove->accelMiss = 0.0f / zeroahaha; // putting NaN in there.
#endif

	if (pmove->debugLevel> 30) {

		Com_Printf("accelmiss nonvalue is %f", pmove->accelMiss);
	}

	if ( finalTime < pmove->ps->commandTime ) {
		return;	// should not happen
	}

	if ( finalTime > pmove->ps->commandTime + 1000 ) {
		pmove->ps->commandTime = finalTime - 1000;
	}

	if (pmove->ps->fallingToDeath)
	{
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
		pmove->cmd.upmove = 0;
		pmove->cmd.buttons = 0;
	}

	pmove->ps->pmove_framecount = (pmove->ps->pmove_framecount+1) & ((1<<PS_PMOVEFRAMECOUNTBITS)-1);

	pmove->unalteredCmd = pmove->cmd; // so we can decide which direction to roll with strafebot (as roll overwrites keys that arent part of its direction)

	if (pmove->roll.status == ROLL_ENDED) {
		pmove->roll.status = ROLL_NONE;
	}

	// chop the move up if it is too long, to prevent framerate
	// dependent behavior
	while ( pmove->ps->commandTime != finalTime ) {
		int		msec;

		msec = finalTime - pmove->ps->commandTime;

		if ( pmove->pmove_fixed ) {
			if ( msec > pmove->pmove_msec ) {
				msec = pmove->pmove_msec;
			}
		}
		else {
			qboolean isRaceMode = pmove->modParms.raceMode;
			if ( msec > 66 && !isRaceMode) { // if racemode, let other places handle this. we are not really concerned about someone gaining an undue advantage then. (is that even what its for? what even is the point since ppl can just send a bunch of shorter cmds at once)
				msec = 66;
			}
		}
		pmove->cmd.serverTime = pmove->ps->commandTime + msec;
		PmoveSingle( pmove );

		if ( pmove->ps->pm_flags & PMF_JUMP_HELD ) {
			pmove->cmd.upmove = 20;
		}
	}
}

#pragma endregion "bg_pmove"







// bg_misc.c



qboolean PMove::BG_HasYsalamiri(int gametype, playerState_t* ps)
{
	if (gametype == GT_CTY && // TODO make it work for other than jk2?
		(ps->powerups[PW_REDFLAG] || ps->powerups[PW_BLUEFLAG]))
	{
		return qtrue;
	}

	if (ps->powerups[PW_YSALAMIRI])
	{
		return qtrue;
	}

	return qfalse;
}



void PMove::BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t* ps) {

	ps->events[ps->eventSequence & (MAX_PS_EVENTS - 1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS - 1)] = eventParm;
	ps->eventSequence++;
}



qboolean PMove::BG_CanUseFPNow(int gametype, playerState_t *ps, int time, forcePowers_t power)
{
	if (BG_HasYsalamiri(gametype, ps))
	{
		return qfalse;
	}

	if ( ps->forceRestricted || ps->trueNonJedi )
	{
		return qfalse;
	}

	if (ps->duelInProgress)
	{
		if (power != FP_SABERATTACK && power != FP_SABERDEFEND && (jk2gameplay == VERSION_1_04 || power != FP_SABERTHROW) &&
			power != FP_LEVITATION)
		{
			if (!ps->saberLockFrame || power != FP_PUSH)
			{
				return qfalse;
			}
		}
	}

	if (ps->saberLockFrame || ps->saberLockTime > time)
	{
		if (power != FP_PUSH)
		{
			return qfalse;
		}
	}

	if (ps->fallingToDeath)
	{
		return qfalse;
	}

	if ( power == FP_SABERTHROW && (ps->weaponstate == WEAPON_RAISING || ps->weaponstate == WEAPON_DROPPING) )
	{ // WP_SaberPositionUpdate doesn't handle saberthrow while in the above weaponstates. Instead the saber just flies away and never returns.
	  // Changing WP_SaberPositionUpdate to handle saberthrow in those states would make it possible to throw the saber in situations it wasn't possible before, so instead we just disable throw in those weaponstates to prevent sabers from getting lost.
		return qfalse;
	}

	return qtrue;
}



//};