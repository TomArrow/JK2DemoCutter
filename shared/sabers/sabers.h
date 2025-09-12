
#ifndef SABERS_H
#define SABERS_H

#include "demoCut.h"

typedef enum SaberAnimationVersion_s
{
	JK2_102,
	JK2_104,
	JKA
} SaberAnimationVersion_t;

typedef struct surfaceBoltFrameInfo_s
{
	int frameNum;
	float matrix[16];
	//float matrix[4][4];
} surfaceBoltFrameInfo_t;

typedef struct animCfgEntry_s
{
	qboolean animExists;
	int targetFrame;
	int frameCount;
	int loopFrame;
	int frameSpeed;
} animCfgEntry_t;

typedef struct SaberAnimState_s
{
	vec3_t relativePosBase;
	vec3_t relativePosTip;
	float playerFarDistance;
	vec3_t playerFarDistAngles;
} SaberAnimState_t;


extern const byte jk2_102_anim_abin[21780];
extern const byte jk2_104_anim_abin[24200];
extern const byte jka_anim_abin[30860];
extern const byte jk2_rhandFrames_bin[1174904];
extern const byte jka_rhandFrames_bin[1453568];


const int jk2_102_anim_abin_count = sizeof(jk2_102_anim_abin) / sizeof(animCfgEntry_t);
const int jk2_104_anim_abin_count = sizeof(jk2_104_anim_abin) / sizeof(animCfgEntry_t);
const int jka_anim_abin_count = sizeof(jka_anim_abin) / sizeof(animCfgEntry_t);
const int jk2_rhandFrames_bin_count = sizeof(jk2_rhandFrames_bin) / sizeof(surfaceBoltFrameInfo_t);
const int jka_rhandFrames_bin_count = sizeof(jka_rhandFrames_bin) / sizeof(surfaceBoltFrameInfo_t);

class SaberAnimationStuff
{
	static SaberAnimState_t saberAnimFramesJk2[jk2_rhandFrames_bin_count];
	static SaberAnimState_t saberAnimFramesJk3[jka_rhandFrames_bin_count];

	//static animCfgEntry_t animStatesJk2_102[jk2_102_anim_abin_count];
	//static animCfgEntry_t animStatesJk2_104[jk2_104_anim_abin_count];
	//static animCfgEntry_t animStatesJka[jka_anim_abin_count];

	static bool initSuccessful;

public:
	static void	init();
	static void InitSaberStates(const byte* input, const int elementCount, SaberAnimState_t* saberFrameInfos);

	static SaberAnimState_t* GetSaberAnimState(int torsoAnim, int64_t animationTime, demoType_t demoType);
	static void TransformRelativePositionByAngle(vec3_t pos, vec3_t angles, vec3_t out);
	static qboolean GetSaberBaseAndTip(int torsoAnim, int64_t animationTime, vec3_t playerOrigin, vec3_t playerAngles, demoType_t demoType, vec3_t base, vec3_t tip);
	static qboolean GetSaberSpritePos(int torsoAnim, int64_t animationTime, vec3_t playerOrigin, vec3_t playerAngles, float width, demoType_t demoType, vec3_t viewOrg, vec3_t points[6]);
};




#endif

