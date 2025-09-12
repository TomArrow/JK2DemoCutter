
#include "../sabers/sabers.h"




SaberAnimState_t SaberAnimationStuff::saberAnimFramesJk2[jk2_rhandFrames_bin_count];
SaberAnimState_t SaberAnimationStuff::saberAnimFramesJk3[jka_rhandFrames_bin_count];

bool SaberAnimationStuff::initSuccessful;


void SaberAnimationStuff::init() {

	InitSaberStates(jk2_rhandFrames_bin, jk2_rhandFrames_bin_count, saberAnimFramesJk2);
	InitSaberStates(jka_rhandFrames_bin, jka_rhandFrames_bin_count, saberAnimFramesJk3);
	//animStatesJk2_102 = InitAnimStates(jk2_102anims);
	//animStatesJk2_104 = InitAnimStates(jk2_104anims);
	//animStatesJka = InitAnimStates(jkaanims);
	initSuccessful = true;
}

void SaberAnimationStuff::InitSaberStates(const byte* input, const int elementCount, SaberAnimState_t* saberFrameInfos) {
	surfaceBoltFrameInfo_t* frameInfos = (surfaceBoltFrameInfo_t*)input;
	vec4_t saberForward = {0,0,40,1};
	for (int i = 0; i < elementCount; i++, frameInfos++, saberFrameInfos++) {
		VectorSet(saberFrameInfos->relativePosBase, frameInfos->matrix[12], frameInfos->matrix[13], frameInfos->matrix[14]);
		Matrix4x4TransformOut3(saberForward,frameInfos->matrix,saberFrameInfos->relativePosTip);
		vec_t* farPos = VectorLength(saberFrameInfos->relativePosBase) > VectorLength(saberFrameInfos->relativePosTip) ? saberFrameInfos->relativePosBase : saberFrameInfos->relativePosTip;
		saberFrameInfos->playerFarDistance = VectorLength(farPos);
		vectoangles(farPos, saberFrameInfos->playerFarDistAngles);
	}
}

SaberAnimState_t* SaberAnimationStuff::GetSaberAnimState(int torsoAnim, int64_t animationTime, demoType_t demoType) {
	if (!initSuccessful) return NULL;
	animCfgEntry_t* entries = NULL;
	SaberAnimState_t* frames = NULL;
	int entriesCount = 0;
	int frameCount = 0;
	switch (demoType) {
		case DM_15:
		case DM_15_1_03:
			entries = (animCfgEntry_t*)jk2_102_anim_abin;
			entriesCount = jk2_102_anim_abin_count;
			frames = saberAnimFramesJk2;
			frameCount = jk2_rhandFrames_bin_count;
			break;
		case DM_14:
		case DM_16:
			entries = (animCfgEntry_t*)jk2_104_anim_abin;
			entriesCount = jk2_104_anim_abin_count;
			frames = saberAnimFramesJk2;
			frameCount = jk2_rhandFrames_bin_count;
			break;
		case DM_25:
		case DM_26:
		case DM_26_XBOX:
			entries = (animCfgEntry_t*)jka_anim_abin;
			entriesCount = jka_anim_abin_count;
			frames = saberAnimFramesJk3;
			frameCount = jka_rhandFrames_bin_count;
			break;
	}
	if (!entries || !frames || torsoAnim < 0 || torsoAnim >= entriesCount) return NULL;
	animCfgEntry_t* animation = entries + torsoAnim;

	int startFrame = animation->targetFrame;
	int sign = 1;
	if (animation->frameSpeed < 0)
	{
		startFrame = animation->targetFrame + animation->frameCount - 1;
		sign = -1;
	}
	int animationFrameOffset = animation->frameSpeed * animationTime / 1000;
	int absOffset = abs(animationFrameOffset);
	if (absOffset >= animation->frameCount)
	{
		// TODO maybe implement loopframes. fuck it for now.
		animationFrameOffset = sign * std::min(animation->frameCount - 1, absOffset);

	}
	int desiredFrame = startFrame + animationFrameOffset;

	if (desiredFrame < 0 || desiredFrame >= frameCount)
	{
		return NULL;
	}

	return frames + desiredFrame;
}

void SaberAnimationStuff::TransformRelativePositionByAngle(vec3_t pos, vec3_t angles, vec3_t out) {
	vec3_t forward, right, up;
	AngleVectors(angles, forward, right, up);
	VectorClear(out);
	VectorMA(out, pos[0], forward, out);
	VectorMA(out, pos[1], right, out);
	VectorMA(out, pos[2], up, out);
}

qboolean SaberAnimationStuff::GetSaberBaseAndTip(int torsoAnim, int64_t animationTime, vec3_t playerOrigin, vec3_t playerAngles, demoType_t demoType, vec3_t base, vec3_t tip) {
	SaberAnimState_t* saberFrame = GetSaberAnimState(torsoAnim, animationTime, demoType);
	if (!saberFrame) return qfalse;

	vec3_t saberBase, saberTip;// , tmp;

	TransformRelativePositionByAngle(saberFrame->relativePosBase, playerAngles, base);
	VectorAdd(playerOrigin, base, base);
	TransformRelativePositionByAngle(saberFrame->relativePosTip, playerAngles, tip);
	VectorAdd(playerOrigin, tip, tip);

	return qtrue;
}

qboolean SaberAnimationStuff::GetSaberSpritePos(int torsoAnim, int64_t animationTime, vec3_t playerOrigin, vec3_t playerAngles, float width, demoType_t demoType, vec3_t viewOrg, vec3_t points[6]) {
	vec3_t start, end;
	if (!GetSaberBaseAndTip(torsoAnim, animationTime, playerOrigin, playerAngles, demoType, start, end)) {
		return qfalse;
	}
	vec3_t v1,v2,right;
	VectorSubtract(start, viewOrg, v1);
	VectorSubtract(end, viewOrg, v2);
	CrossProduct(v1, v2, right);
	VectorNormalize(right);
	
	width *= 0.5f;

#define up right

	VectorMA(start, width, up, points[0]);
	VectorMA(start, -width, up, points[1]);
	VectorMA(end, width, up, points[2]);
	VectorMA(end, width, up, points[3]);
	VectorMA(start, -width, up, points[4]);
	VectorMA(end, -width, up, points[5]);

#undef right

	return qtrue;
}


