
#include "demoCut.h"
#include "otherGameStuff2.h"
#include "../sabers/sabers.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <queue>
#include <filesystem>
#include "sqlite3.h"
#include <set>
#include "../shared/boostTupleHash.hpp"
#include <unordered_map>

#include <iso646.h>
#include "picosha3.h"

#include "popl.hpp"
#include "SQLDelayer.h"
#include "CModel.h"

#include <mutex>
#include "named_mutex.hpp"
#include "tsl/htrie_map.h"
#include "../shared/libgmavi/libgmavi.h"

#include "include/rapidjson/document.h"

#include "../shared/base58/base58.h"

#define DEBUGSTATSDB

#define BUFFERBAT

#define KILLVECARRAY 1


#define VIDEOWIDTH 400
#define VIDEOHEIGHT 300
#define S3L_POSMULT 8
// we need to define screen resolution before including the library:
#define S3L_RESOLUTION_X VIDEOWIDTH
#define S3L_RESOLUTION_Y VIDEOHEIGHT
// and a name of the function we'll be using to draw individual pixels:
#define S3L_PIXEL_FUNCTION drawPixel
#define S3L_NEAR_CROSS_STRATEGY 3 // 3
#define S3L_USE_WIDER_TYPES 0
#define S3L_PERSPECTIVE_CORRECTION 2
//#define S3L_STENCIL_BUFFER 1
//#define S3L_SORT 1
#define S3L_MAX_TRIANGES_DRAWN 100000
//#define S3L_FAR S3L_POSMULT*2048
#define S3L_Z_BUFFER 1
#define S3L_USE_WIDER_TYPES 1
#include "../shared/small3dlib/small3dlib.h" // now include the library


static byte drawBuffer[VIDEOWIDTH * VIDEOHEIGHT * 3];
S3L_Unit cubeVertices[] = { S3L_CUBE_VERTICES(S3L_F) };
S3L_Index cubeTriangles[] = { S3L_CUBE_TRIANGLES };
S3L_Model3D mapModel; 
bool haveMapModel = false;
S3L_Model3D cubeModel; // 3D model, has a geometry, position, rotation etc.
S3L_Scene scene;       // scene we'll be rendring (can have multiple models)
typedef struct drawProperties3dModel_s {
	byte	color[3];
	bool	transparent;
	bool	isCube;
} drawProperties3dModel_t;
std::vector<S3L_Unit>		mapVertices;
std::vector<S3L_Unit>		mapUVs;
std::vector<S3L_Index>		mapTriangles;
std::vector<S3L_Index>		mapTrianglesVisFiltered;
std::vector<lightmap_t>		mapLightmaps;
std::vector<S3L_Model3D>	scene3dmodels;
std::vector<drawProperties3dModel_t>	scene3dmodelProperties;
std::vector<std::vector<S3L_Unit>>		scene3dmodelVertices;
std::vector<std::vector<S3L_Index>>		scene3dmodelTriangles;
#define S3L_POSX(y) ((y)*S3L_POSMULT)
#define S3L_POSY(z) ((z)*S3L_POSMULT)
#define S3L_POSZ(x) ((x)*S3L_POSMULT)
#define S3L_ROTX(x) ((-x)*S3L_F/360)
#define S3L_ROTY(y) ((-y)*S3L_F/360)
#define S3L_ROTZ(z) ((-z)*S3L_F/360)
//#define S3L_ROTX(x) ((-x)*S3L_F/360)
//#define S3L_ROTY(y) ((-y)*S3L_F/360)
//#define S3L_ROTZ(z) ((-z)*S3L_F/360)
bool renderingMap = false;
uint32_t previousTriangle = -1;
int drawLightmapNum = 0;
S3L_Vec4 uv0, uv1, uv2;
void drawPixel(S3L_PixelInfo* p)
{
	uint8_t c[3];  // ASCII pixel we'll write to the screen

	/* We'll draw different triangles with different ASCII symbols to give the
	illusion of lighting. */
	bool transparency = false;

	if (renderingMap && p->modelIndex == 0) { // rendering world
		if (p->triangleID != previousTriangle)
		{

			S3L_getIndexedTriangleValues(p->triangleIndex, mapTrianglesVisFiltered.data(), mapUVs.data(), 3, &uv0, &uv1, &uv2);
			drawLightmapNum = uv0.z;
			previousTriangle = p->triangleID;
		}

		// kind of a fog sort of thing.
		if (drawLightmapNum >= mapLightmaps.size()) {
			c[0] = c[1] = c[2] = std::min(p->depth / S3L_POSMULT / 8, (S3L_Unit)128);
			//c[0] = (p->triangleID) & 255;
			//c[1] = (p->triangleID >> 8) & 255;
			//c[2] = (p->triangleID >> 16) & 255;
		}
		else {
			lightmap_t* lm = &mapLightmaps[drawLightmapNum];
			S3L_Unit uv[2];
			uv[0] = std::clamp(S3L_interpolateBarycentric(uv0.x, uv1.x, uv2.x, p->barycentric), (S3L_Unit)0, (S3L_Unit)LIGHTMAP_SIZE-1);
			uv[1] = std::clamp(S3L_interpolateBarycentric(uv0.y, uv1.y, uv2.y, p->barycentric), (S3L_Unit)0, (S3L_Unit)LIGHTMAP_SIZE-1);

			c[2] = lm->data[uv[1] * 3 * LIGHTMAP_SIZE + uv[0] * 3];
			c[1] = lm->data[uv[1] * 3 * LIGHTMAP_SIZE + uv[0] * 3 + 1];
			c[0] = lm->data[uv[1] * 3 * LIGHTMAP_SIZE + uv[0] * 3 + 2];
		}
	}
	else {

		if (scene3dmodelProperties[p->modelIndex].isCube) {
			if (p->triangleIndex == 0 || p->triangleIndex == 1 ||
				p->triangleIndex == 4 || p->triangleIndex == 5)
				c[0] = c[1] = c[2] = '#';
			else if (p->triangleIndex == 2 || p->triangleIndex == 3 ||
				p->triangleIndex == 6 || p->triangleIndex == 7)
				c[0] = c[1] = c[2] = 'x';
			else
				c[0] = c[1] = c[2] = '.';

			c[0] = std::min(((int)c[0] * (int)scene3dmodelProperties[p->modelIndex].color[2]) >> 8, 255);
			c[1] = std::min(((int)c[1] * (int)scene3dmodelProperties[p->modelIndex].color[1]) >> 8, 255);
			c[2] = std::min(((int)c[2] * (int)scene3dmodelProperties[p->modelIndex].color[0]) >> 8, 255);
		}
		else {
			c[0] = scene3dmodelProperties[p->modelIndex].color[2];
			c[1] = scene3dmodelProperties[p->modelIndex].color[1];
			c[2] = scene3dmodelProperties[p->modelIndex].color[0];
		}

		transparency = scene3dmodelProperties[p->modelIndex].transparent;
	}

	// draw to ASCII screen
	int y = p->y;// S3L_RESOLUTION_Y - 1 - p->y;
	int x = S3L_RESOLUTION_X - 1 - p->x;
	//drawBuffer[(S3L_RESOLUTION_Y - 1 - p->y) * S3L_RESOLUTION_X * 3 + p->x*3] = c;
	//drawBuffer[(S3L_RESOLUTION_Y - 1 - p->y) * S3L_RESOLUTION_X * 3 + p->x*3+1] = c;
	//drawBuffer[(S3L_RESOLUTION_Y - 1 - p->y) * S3L_RESOLUTION_X * 3 + p->x*3+2] = c;
	if (transparency) {
		c[0] = std::min(((int)c[0] + (int)drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3]) / 2, 255);
		c[1] = std::min(((int)c[1] + (int)drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3 + 1]) / 2, 255);
		c[2] = std::min(((int)c[2] + (int)drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3 + 2]) / 2, 255);
		S3L_zBufferWrite(p->x,p->y,p->previousZ);
	}

	drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3] = c[0];
	drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3 + 1] = c[1];
	drawBuffer[y * S3L_RESOLUTION_X * 3 + x * 3 + 2] = c[2];
}
template<unsigned int max_clients>
void setModelColor(drawProperties3dModel_t* modelProps, int number) {

	int team = number < 0 ? (-number) - 1 : (number < max_clients ? playerTeams[number] : -1);
	if (team > -1) {
		switch (team) {
		case TEAM_FREE:
			modelProps->color[0] = 0;
			modelProps->color[1] = 255;
			modelProps->color[2] = 0;
			break;
		case TEAM_RED:
			modelProps->color[0] = 255;
			modelProps->color[1] = 0;
			modelProps->color[2] = 0;
			break;
		case TEAM_BLUE:
			modelProps->color[0] = 0;
			modelProps->color[1] = 0;
			modelProps->color[2] = 255;
			break;
		case TEAM_SPECTATOR:
			modelProps->color[0] = 255;
			modelProps->color[1] = 255;
			modelProps->color[2] = 0;
			break;
		default:
			modelProps->color[0] = 255;
			modelProps->color[1] = 255;
			modelProps->color[2] = 255;
			break;
		}
	}
	else {
		modelProps->color[0] = 255;
		modelProps->color[1] = 255;
		modelProps->color[2] = 255;
	}
}
int64_t lastTorsoAnim[MAX_GENTITIES];
int64_t lastTorsoAnimChange[MAX_GENTITIES];
template<unsigned int max_clients>
void sceneAdd3DSaberForPlayer(vec3_t playerOrigin, vec3_t playerAngles, int torsoAnim, vec3_t camerapos, demoType_t demoType, int entityNum, int64_t demoCurrentTime) {


	if (lastTorsoAnim[entityNum] != torsoAnim) {
		lastTorsoAnimChange[entityNum] = demoCurrentTime;
		lastTorsoAnim[entityNum] = torsoAnim;
	}
	int64_t animationTime = demoCurrentTime - lastTorsoAnimChange[entityNum];

	vec3_t points[6];
	//if (SaberAnimationStuff::GetSaberSpritePos(torsoAnim & ~getANIM_TOGGLEBIT(demoType), animationTime, playerOrigin, playerAngles, 3, demoType, camerapos, points)) {
	if (SaberAnimationStuff::GetSaberSpritePos(torsoAnim & ~getANIM_TOGGLEBIT(demoType), animationTime, vec3_origin, playerAngles, 3, demoType, camerapos, points)) {
		int startIndex = scene3dmodelVertices.size();
		std::vector<S3L_Unit> vertices;
		vertices.reserve(6 * 3);
		std::vector<S3L_Index> indices;
		indices.reserve(6);
		for (int i = 0; i < 6; i++) {
			vertices.push_back(S3L_POSX(points[i][1]));
			vertices.push_back(S3L_POSY(points[i][2]));
			vertices.push_back(S3L_POSZ(points[i][0]));
			indices.push_back(i);
		}
		scene3dmodelVertices.push_back(std::move(vertices));
		scene3dmodelTriangles.push_back(std::move(indices));

		S3L_Model3D saberModel;
		S3L_model3DInit(scene3dmodelVertices.back().data(), 6, scene3dmodelTriangles.back().data(), 2, &saberModel);
		saberModel.transform.translation.x = S3L_POSX(playerOrigin[1]);
		saberModel.transform.translation.y = S3L_POSY(playerOrigin[2]);
		saberModel.transform.translation.z = S3L_POSZ(playerOrigin[0]);
		saberModel.config.backfaceCulling = 0;
		scene3dmodels.push_back(saberModel);
		drawProperties3dModel_t* modelProps2 = &scene3dmodelProperties.emplace_back();
		setModelColor<max_clients>(modelProps2, entityNum);
		modelProps2->transparent = false;
		modelProps2->isCube = false;
	}
}

typedef struct color3ub_s {
	byte b, g, r;
} color3ub_t;
color3ub_t whitefont = { 255,255,255 };
#define blit_pixel color3ub_t
#define blit16_ADJUST_COLOR_FUNC videoAdjustTextColor
#define blit32_ADJUST_COLOR_FUNC videoAdjustTextColor
#include "../shared/blit-fonts/blit16.h"
#include "../shared/blit-fonts/blit32.h"
typedef std::tuple<int64_t, std::string> consoleText;
std::deque<consoleText> videoConsole; // deque cuz i need to iterate over it for drawing, but rly just want a fifo.
std::deque<consoleText> screenCenterText; // deque cuz i need to iterate over it for drawing, but rly just want a fifo.
void videoDrawText(int64_t demoCurrentTime) {
	while (videoConsole.size() && demoCurrentTime - std::get<0>(videoConsole.front()) > 3000LL) {
		videoConsole.pop_front();
	}
	int y = 1;
	for (auto it = videoConsole.begin(); it != videoConsole.end(); it++) {
		y += blit16_ROW_ADVANCE*blit16_TextExplicit((color3ub_t*)drawBuffer, whitefont, 1, VIDEOWIDTH, VIDEOHEIGHT, 1, 1, y, std::get<1>(*it).c_str());
	}
	while (screenCenterText.size() && demoCurrentTime - std::get<0>(screenCenterText.front()) > 3000LL) {
		screenCenterText.pop_front();
	}
	y = VIDEOHEIGHT/2-VIDEOHEIGHT/8;
	for (auto it = screenCenterText.begin(); it != screenCenterText.end(); it++) {
		char* cleanStr = new char[std::get<1>(*it).size() + 1];
		Q_strncpyz(cleanStr, std::get<1>(*it).size() + 1, std::get<1>(*it).c_str(), std::get<1>(*it).size()+1);
		Q_StripColorAll(cleanStr);
		int x = VIDEOWIDTH / 2 - strlen(cleanStr)*blit32_ADVANCE/2;
		delete[] cleanStr;
		y += blit32_ROW_ADVANCE*blit32_TextExplicit((color3ub_t*)drawBuffer, whitefont, 1, VIDEOWIDTH, VIDEOHEIGHT, 1, x, y, std::get<1>(*it).c_str());
	}
}

bool videoAdjustTextColor(color3ub_t* Value, int* i, const char* txt, int strLen) {
	if (*i+1 < strLen) {
		if (Q_IsColorStringHex(txt+1)) {
			vec4_t color;
			int skipCount;
			if (Q_parseColorHex(txt+1, color, &skipCount)) {
				*i += skipCount;
				Value->r = (byte)std::min(color[0] * 255.0f, 255.0f);
				Value->g = (byte)std::min(color[1] * 255.0f, 255.0f);
				Value->b = (byte)std::min(color[2] * 255.0f, 255.0f);
				return true;
			}
		}
		else {
			*i += 1;
			int colorIndex = ColorIndex(txt[1]);
			Value->r = (byte)std::min(g_color_table[colorIndex][0] * 255.0f, 255.0f);
			Value->g = (byte)std::min(g_color_table[colorIndex][1] * 255.0f, 255.0f);
			Value->b = (byte)std::min(g_color_table[colorIndex][2] * 255.0f, 255.0f);
			return true;
		}
	}
	return false;
}

CModel* cm = NULL;


std::queue<entityState_t*> parsedEventEntities;




// flags for sql fields so we dont need to do dumb string comparisons
#define QF_PLAYERNAME0				(1<<0)
#define QF_PLAYERNAME0STRIPPED		(1<<1)
#define QF_PLAYERNAME1				(1<<2)
#define QF_PLAYERNAME1STRIPPED		(1<<3)


typedef class SQLDelayedQueryWrapper_t {
public:
	SQLDelayedQuery query;
	std::string batchSuffix = ""; // Text that should appear after the cutting command (like metadata that finishes collecting after a kill)
	std::string batchPrefix = ""; // Text that should appear before the cutting command (like insert Id)
	std::string batchString1 = ""; // Main text to appear in batch (part1) In parts so we can add something to the target filename.
	std::string batchString2 = ""; // Main text to appear in batch (part2)
	std::stringstream batchMiddlePart; // Something to insert into the middle of the target cut filename. Not the most intuitive variable name but that's what this comment is for
	int	databaseIndex = 0; // so we can split finds based on maps or other things
};

#define MAX_KILLDBS 32

struct ioHandlesKillDb_t {

	sqlite3* killDb;
	sqlite3_stmt* insertPlayerNameStatement;
	sqlite3_stmt* insertEntryMetaStatement;
	sqlite3_stmt* insertSpecialStatement;
	sqlite3_stmt* insertLaughsStatement;
	sqlite3_stmt* insertDefragRunStatement;
	sqlite3_stmt* insertCaptureStatement;
	sqlite3_stmt* insertFlagGrabStatement;
	sqlite3_stmt* insertSpreeStatement;
	sqlite3_stmt* insertStatement;
	sqlite3_stmt* insertAngleStatement;
	sqlite3_stmt* insertDemoDatabaseProperty;
	sqlite3_stmt* insertDemoMeta;
	sqlite3_stmt* selectLastInsertRowIdStatement;
};

typedef std::vector<SQLDelayedQueryWrapper_t*> queryCollection;
struct ioHandles_t {
	// Killdb
	ioHandlesKillDb_t killDb[MAX_KILLDBS];

	// statsDb
	sqlite3* statsDb;
	sqlite3_stmt* insertPlayerModelStatement;
	sqlite3_stmt* updatePlayerModelCountStatement;
	sqlite3_stmt* selectStatsLastInsertRowIdStatement;
	sqlite3_stmt* insertPlayerDemoStatsStatement;

	// Debugstatsdb
	sqlite3* debugStatsDb;
	sqlite3_stmt* insertAnimStanceStatement;
	sqlite3_stmt* updateAnimStanceCountStatement;
	sqlite3_stmt* insertFrameInfoViewModelAnimStatement;
	sqlite3_stmt* updateFrameInfoViewModelAnimCountStatement;
	sqlite3_stmt* insertFrameInfoViewModelAnimStatementSimple;
	sqlite3_stmt* updateFrameInfoViewModelAnimCountStatementSimple;

	// Demostats (optionaal)
	sqlite3* demoStatsDb;
	sqlite3_stmt* insertPacketStatsStatement;

	// Chats (optional)
	sqlite3* chatsUniqueDb;
	sqlite3_stmt* insertChatUniqueStatement;
	sqlite3_stmt* updateChatUniqueCountStatement;

	// Entity to DB (optional)
	sqlite3* entityDataDb;
	sqlite3_stmt* insertEntityDataStatement;
	sqlite3_stmt* insertPlayerStateDataStatement;
	sqlite3_stmt* insertGameStateDataStatement;
	sqlite3_stmt* insertConfigStringDataStatement;

	// Delayer query stuff
	queryCollection* markQueries;
	queryCollection* specialQueries;
	queryCollection* laughQueries;
	queryCollection* defragQueries;
	queryCollection* captureQueries;
	queryCollection* flagGrabQueries;
	queryCollection* spreeQueries;
	queryCollection* killQueries;
	queryCollection* killAngleQueries;
	queryCollection* playerModelQueries;
	queryCollection* playerDemoStatsQueries;
	queryCollection* animStanceQueries;
	queryCollection* frameInfoVieWModelQueries;
	queryCollection* frameInfoVieWModelSimpleQueries;
	queryCollection* packetStatsQueries;
	queryCollection* chatsUniqueQueries;

	// Output bat files
	std::ostream* outputBatHandle;
	std::ostream* outputBatHandleKillSprees;
	std::ostream* outputBatHandleDefrag;
	std::ostream* outputBatHandleCaptures;
	std::ostream* outputBatHandleFlagGrabs;
	std::ostream* outputBatHandleLaughs;
	std::ostream* outputBatHandleSpecial;
	std::ostream* outputBatHandleMarks;
};
struct sharedVariables_t {
	std::string oldPath;
	std::string oldBasename;
	time_t oldDemoDateModified;
	int64_t demoFilesize;
};


struct teleportInfo_t {
	int64_t demoTime;
	vec3_t position;
};

std::vector<teleportInfo_t> teleports; // teleport analysis. if we want to fast forward repeated attempts of stuff. (-A option)

class ResultFilter {
public:
	typedef enum Type {
		FILTER_GAMETYPE,
		FILTER_MAP,
		FILTER_ALLGOES
	};
	Type type = FILTER_ALLGOES;

	// gametype
	int64_t matchValue = 0; // e.g. the gametype.
	
	// mapname or other string filters
	std::string matchString;
	bool matchStart = true; // match the start (user didnt use *)
	bool matchEnd = true; // match the end (user didnt use *)

};

class ExtraSearchOptions {
public:
	bool quickSkipNonSaberExclusive = false; // Not implemented (yet). Skip demoss that aren't saber only
	bool onlyLogSaberKills = false;
	bool entityDataToDb = false;
	int onlyLogKillSpreesWithSaberKills = 0;
	int writeDemoPacketStats = 0; // 0 = don't write stats. -1  = write immediately on every packet. other number = write  as soon as time interval in demotime has passed
	bool onlyLogCapturesWithSaberKills = false;
	bool findSuperSlowKillStreaks = false;
	bool noFindOutput = false;
	bool killSpreesOnlyVisible = false;
	bool testOnly = false;
	bool reframeIfNeeded = false;
	bool dumpStufftext = false;
	bool printDebug = false;
	int jumpMetaEventsLimit = 0;
	int strafeCSVSyncPoint = 0; // Where do we sync different strafes in the csv? I think speed 300 makes sense? This also serves as a general trigger for the strafe CSV functionality. If 0 -> disabled
	int strafeCSVResetPoint = 0; // Where do we reset to a new strafe in the csv? When falls below this. I think 200 makes sense.
	int strafeCSVMinRunDuration = 0; // Discard runs for strafe csv that are shorter than N milliseconds.
	bool strafeCSVInterpolate = false;
	bool playerCSVDump = false;
	bool playerCSVDumpCommandTimeDupeSkip = false;
	int	entityCSVDump = 0; // -1 = all ents; 0 = none; > 0 specific ent
	bool doChatSearch = false;
	bool doPrintSearch = false;
	bool doStringSearch = false;
	bool skipKills = false;
	std::string chatSearch = "";
	std::string printSearch = "";
	std::string stringSearch = "";
	int netAnalysisMode = 0;
	int teleportAnalysis = 0; // 1 analyze teleports that are to the same place. 2 analyze being at same place multiple times (more expensive computation)
	int64_t teleportAnalysisEndDemoTime = 0;
	int teleportAnalysisBufferTimeFuture = 5000;
	int teleportAnalysisBufferTimePast = 2000;
	int teleportAnalysisMinTimeFastForward = 1000;
	float teleportAnalysisMaxDistanceHorizontal = 100; 
	float teleportAnalysisMaxDistanceVertical = 40; 
	bool writeChatsUnique = false;
	bool writeChatsCategorized = false; // Not currently implemented
	bool doStatsDb = false;
	bool doStrafeDeviation = false;
	bool findjumpbugs = false;
	bool makeVideo = false;
	int videoMinMsec = 0;
	std::string videoPath;
	std::vector<std::string> bspDirectories;
	int throughWallChecks = 0; // 1 = all kills, 2 = only saber

	std::vector<ResultFilter> filters;
	int	killDbsCount = 1; // pretty much sorta just filters.size() but minimum 1 always gg
};

typedef struct videoFrame_s {
	int64_t	demoTime;
	byte	image[VIDEOWIDTH * VIDEOHEIGHT * 3];
} videoFrame_t;

std::vector<videoFrame_t> videoFrames;

void saveVideo(const ExtraSearchOptions& opts) {
	void* gmav = gmav_open(opts.videoPath.c_str(), VIDEOWIDTH, VIDEOHEIGHT, 1000);
	int64_t lastTime = videoFrames.size() ? videoFrames.front().demoTime : 0;
	for (auto it = videoFrames.begin(); it != videoFrames.end(); it++) {
		while (it->demoTime - lastTime > 1) {
			if (it->demoTime - lastTime > 100) {
				lastTime = it->demoTime - 100;
			}
			gmav_add(gmav, NULL);
			lastTime++;
		}
		gmav_add(gmav, it->image);
		lastTime = it->demoTime;
	}
	gmav_finish(gmav);
}

typedef struct strafeCSVPoint_t {
	int64_t timeOffset;
	float distanceTraveledFromLast;
	float velocityXY;
};

typedef std::vector<strafeCSVPoint_t> strafeCSVRun_t;
strafeCSVRun_t strafeCSVCurrentRun[MAX_CLIENTS_MAX];
std::vector< strafeCSVRun_t> strafeCSVRuns[MAX_CLIENTS_MAX];

inline static void strafeCSVResetPlayer(int clientNum, const ExtraSearchOptions& opts) {
	if (strafeCSVCurrentRun[clientNum].size() > 0) {
		int64_t timeDuration = strafeCSVCurrentRun[clientNum].back().timeOffset;
		if (!opts.strafeCSVMinRunDuration || timeDuration >= opts.strafeCSVMinRunDuration) {
			strafeCSVRuns[clientNum].push_back(strafeCSVCurrentRun[clientNum]);
		}
		strafeCSVCurrentRun[clientNum].clear();
	}
}


typedef struct playerDumpCSVPoint_t {
	int64_t demoTime;
	int commandTime;
	vec3_t position;
	vec3_t velocity;
	vec3_t viewangles;
};
std::vector<playerDumpCSVPoint_t> playerDumpCSVDataPoints[MAX_CLIENTS_MAX];

typedef struct entityDumpCSVPoint_t {
	int serverTime;
	int eType;
	const char* extraInfo;
	trajectory_t pos;
	trajectory_t apos;
};
std::vector<entityDumpCSVPoint_t> entityDumpCSVDataPoints[MAX_GENTITIES];


#define PLAYERSTATEOTHERKILLERBOOSTDETECTION
#undef PLAYERSTATEOTHERKILLERBOOSTDETECTION // I disabled this because while it theoretically could work, it won't work because the otherKiller value of the playerstate is not actually transmitted. So useless.


typedef jpcre2::select<char> jp;
//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
// OC Defrag
jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\]( which is his personal best time)?.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");


jp::Regex defragDarkFinishRegex(R"raw(\^7Info\^5: \^7(?<playername>[^\n]+?) \^7finished \^5'\^7(?<coursename>[^\n]+?)\^5' \[\^7\^\d(?<style>[^\n]+?)\^5\] \^7in \^5'\^3(?<time>(?<hours>\d\d):(?<minutes>\d\d):(?<seconds>\d\d):(?<milliseconds>\d\d\d))\^5' \(Diff to best\^5: \^\d(?<timediff>(?<plusminusdiff>[+-])(?<hoursdiff>\d\d):(?<minutesdiff>\d\d):(?<secondsdiff>\d\d):(?<millisecondsdiff>\d\d\d))\^5\).)raw", "mSi");
jp::Regex defragDarkPersonalBestRegex(R"raw(\^4\[PERSONAL\] \^7(?<playername>[^\n]+?) \^5\[\^7(?<username>[^\n]+?)\^5\] \^7improved their personal best time for \^5'\^7(?<coursename>[^\n]+?)\^5' \[\^7\^\d(?<style>[^\n]+?)\^5\] -> '\^7(?<time>(?<hours>\d\d):(?<minutes>\d\d):(?<seconds>\d\d):(?<milliseconds>\d\d\d))\^5'.)raw", "mSi");
jp::Regex defragDarkRecordRegex(R"raw(\^1\[ GLOBAL \] \^7(?<playername>[^\n]+?) \^5\[\^7(?<username>[^\n]+?)\^5\] \^7set a new record for \^5'\^7(?<coursename>[^\n]+?)\^5' \[\^7\^\d(?<style>[^\n]+?)\^5\] -> '\^7(?<time>(?<hours>\d\d):(?<minutes>\d\d):(?<seconds>\d\d):(?<milliseconds>\d\d\d))\^5'.)raw", "mSi");

// Razor Defrag (different server)
jp::Regex defragRazorFinishRegex(R"raw(\^\d:\[\s*\^7(.*?)\s\^7(finished in|(beat the WORLD RECORD and )?(is now ranked) \^\d#(\d) \^7with)\s\^3(\d+):(\d+))raw", "mSi");
jp::Regex defragRazorPersonalBestRegex(R"raw(\^\d:\[\s*\^7New personal record on this map!\s*\^\d\]:)raw", "mSi");

jp::Regex defragJaProFinishRegex(R"raw(\^(?:\d)(?<mapname>[^\s]+)?(?:(?:\s*\^\d\s*)?(?<c>c))ompleted(?:(?<prorun> \(PRO\))|(?<tpscps> with (?<tps>\d+) TPs & (?<cps>\d+) CPs))? in\s*\^3(?:(?:(?<hours>\d+):)?(?<minutes>\d+):)?(?<seconds>\d+).(?<msec>\d+)\s*\^(?<color>\d)\s*max:\^3(?<maxSpeed>\d+)\s*\^\d\s*avg:\^3(?<avgSpeed>\d+)\s*\^\d\s*style:\^3(?<style>[^\s]+)\s*\^\d\s*by \^(?<nameColor>\d)(?<name>[^\s]+)\s*(?:\^5\((?<recordType>[^\)]+)\))?\s*(?:\((?:(?<oldRank>\d+)->)?(?<newRank>\d+)\s*\+(?<addedScore>[^\)]+)\))?)raw", "mSi");



class defragRunInfo_t {
public:
	int milliseconds = 0;
	std::string playerName;
	std::string defragPlayerName;
	qboolean isNumber1 = qfalse;
	//qboolean isTop10 = qfalse;
	qboolean isLogged = qfalse;
	qboolean isPersonalBest = qfalse;

	// JAPRO
	int knownClientNum = -1;
	std::string courseName;
	int exactRank;
	std::string style;
	qboolean isValid = qtrue;
	qboolean isProRun = qfalse;
	int teleports = 0;
	int checkpoints = 0;
	
	// q3
	bool checksumError = false;

};

class defragRunInfoFinal_t {
public:
	defragRunInfo_t	info;
	defragRunMeta_t meta;	// final info for saving
};


#define DEFRAG_STRAFEDEVIATION_SAMPLE_START_TIME_MAX_OFFSET 500 // When we start to measure strafe precision of a defrag run, the start time of our measurement must fall within 500ms of the start of the run to be considered valid. The reason we need to accept some offset is (I guess) because of discrepance between command times, snaps, servertimes etc. Idk.


//std::map<int,int> playerFirstVisible;
//std::map<int,int> playerFirstFollowed;
//std::map<int,int> playerFirstFollowedOrVisible;
//std::map<int,int> lastEvent;
#define MAX_PAST_SABERMOVE_SAVE 3
#define PARRY_DFA_DETECT_SLOW_THRESHOLD 350
#define PARRY_YDFA_DETECT_SLOW_THRESHOLD 600
struct saberMoveInfo_t {
	int saberMoveGeneral;
	int64_t saberMoveChange;
	float speed;
};
struct LastSaberMoveInfo {
	saberMoveInfo_t lastSaberMove[MAX_PAST_SABERMOVE_SAVE]; // 0 is last. 1+ are previous ones and always move up
	// the past ones are to track chained attacks like kills from a parry
};

struct TeamInfo {
	int playerCount;
	int score;
	//int flagHoldOrigin; // Did the current flag holder pick up the flag from base or from a dropped place? This will simply be the old flag status.
};
struct CapperKillsInfo {
	int lastKillDemoTime; // Will reset if the last kill time is before the flag hold.
	int kills;
	int rets;
	int saberkills;
};

struct EnemyNearbyInfo {
	int lastUpdateTime;
	int enemyNearbyTimes[MAX_CLIENTS_MAX + 1]; // There can be 31 nearby enemies (one oneself cant be an enemy) max. Also 0. 32 means: unknown. Index 32 (+1) is for unknown, if flag carrier wasnt visible.
	int enemyVeryCloseTimes[MAX_CLIENTS_MAX + 1];
};
struct VariousCappingInfo {
	int lastUpdateTime;
	float maxSpeedThisRun;
	double sumSpeeds;
	double divisorSpeeds;
};
struct strafeDeviationInfo_t {
	averageHelper_t averageHelper;
	int64_t lastReset;
};

#define AIR_TO_AIR_DETECTION_HEIGHT_THRESHOLD forceJumpHeight[FORCE_LEVEL_1] // Higher than a normal lowest tier force jump could reach.
#define ALMOST_CAPTURE_DISTANCE 200

bool gameIsSaberOnlyIsh = false; // Saber only-ish. The basic question this answers is: Does this gamemode allow normal weapons? Or only saber (and maybe mines)? Basically I'm trying to question whether this demo contains mostly saber combat. Aka if blasters and such are banned. So I can early cancel analyzing some demos.
bool gameAllowsDoomingForcePowers = false; // Stuff like push/pull is allowed in this game. Hence if we are detecting saber kills that lead to dooms/suic, we need to try to exclude a few kills that might just be push/pull/grip etc
int sentryModelIndex = -1; // Model index of sentry, so we don't have to do string comparison all the time


class q3DefragInfo_t {
	int32_t crs(int32_t a, int32_t b) {
		return (int32_t)(((uint32_t)a & 0xffffffff) >> b);
	}
	int32_t cls(int32_t a, int32_t b) {
		return (int)(((uint32_t)a << b) & 0xffffffff);
	}
public:
	bool promode = false;
	int version = 0;
	int mapnameChecksum = 0;
	int gameType = 0;
	bool online = false;
	bool cheats = false;
	void SetMapName(const char* mapname) {
		mapnameChecksum = 0;
		if (!mapname || !*mapname) {
			return;
		}
		while (*mapname) {
			mapnameChecksum += *mapname;
			mapname++;
		}
		mapnameChecksum &= 0xFF;
	}

	int calculateCorrectTime(playerState_t* ps, int serverTime, bool* errored) {
		if (errored) {
			*errored = false;
		}
		int normalTime = (int)((uint16_t)(int16_t)ps->stats[7]) * 65536 + ((uint16_t)(int16_t)ps->stats[8]);
		if (online && version != 190) {
			return normalTime;
		}
		if (version >= 19112 && cheats) {
			return normalTime;
		}
		int time = normalTime;
		time ^= abs((int)floorf(ps->origin[0])) & 0xFFFF;
		time ^= (abs((int)floorf(ps->velocity[0])) & 0xFFFF)* 65536;
		time ^= ps->stats[STAT_HEALTH] > 0 ? ps->stats[STAT_HEALTH] & 255 : 150;
		time ^= (ps->movementDir & 15) << 28;

		for (int i = 24; i > 0; i -= 8)
		{
			int32_t temp = (crs(time, i) ^ crs(time, i - 8)) & 0xff;
			time = (time & ~cls(0xff, i)) | cls(temp, i);
		}

		int32_t randomThing = cls(serverTime, 2);
		randomThing += cls(version + mapnameChecksum, 8);
		randomThing ^= cls(serverTime, 0x18);
		time ^= randomThing;
		randomThing = crs(time, 0x1c);
		randomThing |= cls(~randomThing, 4) & 0xff;
		randomThing |= cls(randomThing, 8);
		randomThing |= cls(randomThing, 0x10);
		time ^= randomThing;
		randomThing = crs(time, 0x16) & 0x3f;
		time &= 0x3fffff;

		int32_t randomThing2 = 0;
		for (int l = 0; l < 3; l++)
		{
			randomThing2 += crs(time, 6 * l) & 0x3f;
		}

		randomThing2 += crs(time, 0x12) & 0xf;

		if (randomThing != (randomThing2 & 0x3f))
		{
			if (errored) {
				*errored = true;
			}
			std::cout << "Checksum error in Q3 defrag run\n";
		}
		return time;
	}
};
q3DefragInfo_t q3DefragInfo;
bool gameIsQ3Defrag = false;


typedef struct flagPosition_t {
	qboolean known;
	vec3_t pos;
};
flagPosition_t	baseFlagPositions[MAX_TEAMS]{};

#define MAX_PAST_SPIRAL_LOCATIONS 512 // i think we dont need more do we?
typedef struct playerSpiralLocationInfo_t {
	spiralLocation_t	locs[MAX_PAST_SPIRAL_LOCATIONS];
	int64_t				nextLoc;
};
playerSpiralLocationInfo_t	playerSpiralLocations[MAX_CLIENTS_MAX]; // coulda gone for std::vector or sth but i think 512 past locations is good enough. theres practically no circumstance where we'd want to save more into the past. that would imply, guesstimating, traveling 256,000 units while holding the flag. i guess it might happen in theory on very long holds but keeping that much info isnt practical anyway...
std::string getPlayerSpiralLocationsString(int clientNum, int64_t sinceTime, int maxItems) {
	if (playerSpiralLocations[clientNum].nextLoc == 0) {
		return "";
	}
	int64_t firstIndex = playerSpiralLocations[clientNum].nextLoc;
	int64_t lowestPossibleIndex = std::max(0LL, playerSpiralLocations[clientNum].nextLoc-MAX_PAST_SPIRAL_LOCATIONS);
	spiralLocation_t* locInfo = NULL;
	int maxLocation = 0;
	do {
		firstIndex--;
		locInfo = &playerSpiralLocations[clientNum].locs[firstIndex % MAX_PAST_SPIRAL_LOCATIONS];
		if (locInfo->encodedLocation > maxLocation) {
			maxLocation = locInfo->encodedLocation;
		}
	} while (locInfo->demoTime > sinceTime && firstIndex > lowestPossibleIndex);
	// figure out the amount of base91 chars we need to encode the largest number.
	int digitsneeded = 0;
	int maxLocationLimit = maxLocation;
	while (maxLocation) {
		maxLocation /= 91;
		digitsneeded++;
	}

	if (digitsneeded > 4) { // thats already insane. doubt we should ever go over 2.
		digitsneeded = 4;
		maxLocationLimit = 91 * 91 * 91 * 91 - 1;
	}

	if (!digitsneeded) {
		return "0"; // its literally just a location at 0,0, shrug.
	}

	std::stringstream str;
	str << (char)('0' + digitsneeded);  // we always start with a control character to signal the version of encoding we use, in case we ever want to improve/change it. for now, 0-9 is reserved as a 500-distance spiral encoding with 0-9 base91 chars to encode the individual location.

	int itemCount = playerSpiralLocations[clientNum].nextLoc - firstIndex;
	float skip = itemCount > maxItems ? ((float)(itemCount - 1) / (float)(maxItems - 1)) : 1.0f; // if needed, we "thin out" a bit.

	int roundedIndex = 0;
	for (float index = firstIndex; (roundedIndex = roundf(index) + 0.25f) < playerSpiralLocations[clientNum].nextLoc; index += skip) {
		spiralLocation_t* locInfo = &playerSpiralLocations[clientNum].locs[roundedIndex % MAX_PAST_SPIRAL_LOCATIONS];
		// decompose the encoded location into base91 chars
		int loc = locInfo->encodedLocation;
		if (loc > maxLocationLimit) continue;
		for (int digit = 0; digit < digitsneeded; digit++) {
			int number = loc % 91;
			str << (char)base91BasicAlphabet_[number];
			loc /= 91;
		}
	}

	return str.str();
}


int mohaaPlayerWeaponModelIndexThisFrame[MAX_CLIENTS_MAX];
int mohaaPlayerWeaponModelIndex[MAX_CLIENTS_MAX];
int mohaaPlayerWeapon[MAX_CLIENTS_MAX];
//int mohaaPlayerWeaponTagNum[MAX_CLIENTS_MAX];
LastSaberMoveInfo playerLastSaberMove[MAX_CLIENTS_MAX];
int playerFirstVisible[MAX_CLIENTS_MAX];
int playerFirstFollowed[MAX_CLIENTS_MAX];
int playerFirstFollowedOrVisible[MAX_CLIENTS_MAX];
int playerLastSeen[MAX_CLIENTS_MAX];
int recentFlagHoldTimes[MAX_CLIENTS_MAX];
int playerVisibleFrames[MAX_CLIENTS_MAX];
int playerVisibleClientFrames[MAX_CLIENTS_MAX];
CapperKillsInfo recentKillsDuringFlagHold[MAX_CLIENTS_MAX];
EnemyNearbyInfo recentFlagHoldEnemyNearbyTimes[MAX_CLIENTS_MAX]; // For each player: How much time during cap was spent with X amount of enemies nearby? To judge dangerousness of cap.
VariousCappingInfo recentFlagHoldVariousInfo[MAX_CLIENTS_MAX]; // For each player: How much time during cap was spent with X amount of enemies nearby? To judge dangerousness of cap.
int playerTeams[MAX_CLIENTS_MAX];
TeamInfo teamInfo[MAX_TEAMS];
int lastEvent[MAX_GENTITIES];
int lastEventTime[MAX_GENTITIES];
float lastGroundHeight[MAX_CLIENTS_MAX]; // Last Z coordinate (height in Q3 system) when groundEntityNum was ENTITYNUM_WORLD
//std::map<int,std::string> lastPlayerModel;
std::string lastPlayerModel[MAX_CLIENTS_MAX];
int lastKnownFlagCarrier[TEAM_NUM_TEAMS] = {-1,-1,-1,-1};
strafeDeviationInfo_t strafeDeviationsDefrag[MAX_CLIENTS_MAX];
int saberComboCounter[MAX_CLIENTS_MAX];

#define GROUND_CROUCH_SNEAK_THRESHOLD 2000 // If you are crouching and walking on ground for this long, we're gonna assume you're SNEAKING
int groundCrouchDurations[MAX_CLIENTS_MAX];
int64_t lastSneak[MAX_CLIENTS_MAX];
int lastSneakDuration[MAX_CLIENTS_MAX];

// From including, to excluding
#define STRAFE_ANALYSIS_BUCKET_COUNT 18
struct strafeAnalysisBucket {
	float fromIncluding;
	float toExcluding;
};
strafeAnalysisBucket strafeAnalysisBuckets[STRAFE_ANALYSIS_BUCKET_COUNT] = { {-9999999,250},{250,350},{350,450},{450,550},{550,650},{750,850},{850,950},{950,1050},{1050,1150},{1150,1250},{1250,1350},{1350,1450},{1450,1550},{1550,1650},{1650,1750},{1750,1850},{1850,1950},{1950,99999999999} };

struct playerDemoStats_t {
	qboolean everUsed;
	averageHelper_t strafeDeviation;
	averageHelper_t strafeDeviationBuckets[STRAFE_ANALYSIS_BUCKET_COUNT];

	// Extra stats dedicated for when saber is not in use - more meaningful?
	qboolean everUsedNoSaberMove;
	averageHelper_t strafeDeviationNoSaberMove;
	averageHelper_t strafeDeviationNoSaberMoveBuckets[STRAFE_ANALYSIS_BUCKET_COUNT];

	averageHelper_t	groundFrameQuality;

	int64_t hitBySaberCount;
	int64_t hitBySaberBlockableCount;
	int64_t parryCount;
	int64_t attackFromParryCount;
};
// Since players could connect/disconnect/rename, we keep a map with all players that existed during a game.
// To keep good performance, we have an array of pointers towards the current playerDemoStats_t struct in the map for each player for quick access
// This array is updated whenever a "cs" command is received (since that could mean a map or playername change or a new player connecting.
typedef std::tuple<std::string, std::string, int> playerDemoStatsMapKey_t;
std::unordered_map<playerDemoStatsMapKey_t, playerDemoStats_t,tupleHash::tuple_hash> playerDemoStatsMap; // Keys are: Mapname, playername, clientnum
//ankerl::unordered_dense::map<playerDemoStatsMapKey_t, playerDemoStats_t, ankerl::unordered_dense::hash<playerDemoStatsMapKey_t>> playerDemoStatsMap; // Keys are: Mapname, playername, clientnum
// NOPE DONT USE ANKERL HERE because the pointers arent stable so if we insert new values, old iterators get invalidated (shudder). playerDemoStatsPointers keeps the pointers. leads to absolutely horrible and near-impossible to track down issues. ended up figuring it out only with git bisect. we might be able to work around it but honestly it seems barely worth it. cana consider doing it someday, idk.

playerDemoStats_t* playerDemoStatsPointers[MAX_CLIENTS_MAX];

struct hitDetectionData_t {
	qboolean confirmedHit;
	qboolean painDetected;
	qboolean nearbySaberHitDetected;
	qboolean newParryDetected;
};

hitDetectionData_t hitDetectionData[MAX_CLIENTS_MAX];
qboolean jumpDetected[MAX_CLIENTS_MAX];

// Tries to find all sorts of laughter in chat, but tries to exclude non-exuberant types (like a simple lol), and focus on big letter LOL, big letter XD, rofl, wtf etc and some misspelled variants.
jp::Regex regexLaugh(R"raw(\x19:\s*(r+[oi]+[tf]+[kl]+|[op]+[mn]+[ghf]+|[lk]+[mn]+[fg]*a+[okli]+|a?ha[ha]{2,}|w+[rt]+[gf]+|(?-i)X+D+|L+O{1,100}L+(?i)))raw", "mSi");
#define MAX_LAUGH_DELAY 7000 // From first laugh to last laugh, max delay.
#define LAUGHS_CUT_PRE_TIME 10000 // Upon first laugh, cut last 10 seconds so we see context.
int64_t firstLaugh = -1;
int64_t lastLaugh = -1;
int laughCount = 0;
std::stringstream laughs;
std::stringstream laughsChatlog;

void resetLaughs() {
	firstLaugh = -1;
	lastLaugh = -1;
	laughCount = 0;
	laughs.str(std::string());
	laughsChatlog.str(std::string());
}


//struct cgs{// 0 is at base, 1 is taken, 2 is dropped
//	int redflag, blueflag, yellowflag;
//	int redFlagLastChange, blueFlagLastChange, yellowflagLastChange;
//	int redFlagLastPickupOrigin, blueFlagLastPickupOrigin, yellowflagLastPickupOrigin;
//	int redFlagLastChangeToTaken, blueFlagLastChangeToTaken, yellowflagLastChangeToTaken;
//} cgs;

struct cgs{// 0 is at base, 1 is taken, 2 is dropped
	int flag[TEAM_NUM_TEAMS];
	int64_t flagLastChange[TEAM_NUM_TEAMS];
	int flagLastPickupOrigin[TEAM_NUM_TEAMS];
	int64_t flagLastChangeToTaken[TEAM_NUM_TEAMS];
	int64_t flagLastFlagHoldTimeOver0[TEAM_NUM_TEAMS];
} cgs;


#ifdef DEBUGSTATSDB
typedef std::tuple<int, int, int, int, int,int> animStanceKey; // demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance
//std::unordered_map< animStanceKey, int, tupleHash::tuple_hash> animStanceCounts;
ankerl::unordered_dense::map< animStanceKey, int, ankerl::unordered_dense::hash<animStanceKey>> animStanceCounts;
typedef enum frameInfoType_t {
	PLAYER,
	PLAYER_HIGHWEIGHT,
	WEAPON,
	WEAPON_HIGHWEIGHT
};
char* frameInfoTypeNames[] = {
	"player",
	"player_highweight",
	"weapon",
	"weapon_highweight",
};
typedef std::tuple<int, frameInfoType_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> frameInfoViewModelAnimKey; // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
ankerl::unordered_dense::map< frameInfoViewModelAnimKey, int, ankerl::unordered_dense::hash<frameInfoViewModelAnimKey>> frameInfoViewModelAnimCounts;
typedef std::tuple<int, frameInfoType_t, int, int> frameInfoViewModelAnimSimpleKey; // demoVersion,type,viewModelAnim,frameInfoIndex
ankerl::unordered_dense::map< frameInfoViewModelAnimSimpleKey, int, ankerl::unordered_dense::hash<frameInfoViewModelAnimSimpleKey>> frameInfoViewModelAnimSimpleCounts;
#endif


//#define SQLBIND_DELAYED(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
//#define SQLBIND_DELAYED_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
//#define SQLBIND_DELAYED_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)
// 
#define SQLBIND_NONDELAYED(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
#define SQLBIND_NONDELAYED_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
#define SQLBIND_NONDELAYED_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)

#define SQLBIND_DELAYED(delayedQuery,type,name,value) delayedQuery->add(name,value)
#define SQLBIND_DELAYED_FLAGS(delayedQuery,type,name,value,flags) delayedQuery->add(name,value,flags)
#define SQLBIND_DELAYED_NULL(delayedQuery,name) delayedQuery->add(name,SQLDelayedValue_NULL)
#define SQLBIND_DELAYED_NULL_FLAGS(delayedQuery,name,flags) delayedQuery->add(name,SQLDelayedValue_NULL,flags)
#define SQLBIND_DELAYED_TEXT(delayedQuery,name,value) delayedQuery->add(name,value)
#define SQLBIND_DELAYED_TEXT_FLAGS(delayedQuery,name,value,flags) delayedQuery->add(name,value,flags)

#define NEARBY_PLAYER_MAX_DISTANCE 1000.0f
#define VERYCLOSE_PLAYER_MAX_DISTANCE 300.0f
struct NearbyPlayer {
	int clientNum;
	float distance;
};

class Kill {
public:
	int time;
	int targetClientNum;
	meansOfDeath_t mod;
	bool isSuicide;
	bool isRet;
	bool isTeamKill;
	bool isDoom;
	bool isExplosion;
	bool isVisible;
	bool isFollowed;
	bool isSaberKill;
	int timeSinceSaberMoveChange;
	int timeSinceBackflip;
	float speedatSaberMoveChange;
	float victimMaxSpeedPastSecond;
	std::string hashSourceString;
	std::string hash;
	std::string attackerName;
	std::string victimName;
	std::string modInfoString;
	std::vector< NearbyPlayer> nearbyPlayers;
};

struct SpreeInfo {
	int totalTime;
	int64_t lastKillTime;
	int countKills = 0;
	int countRets = 0;
	int countTeamKills = 0;
	int countDooms = 0;
	int countExplosions = 0;
	int countThirdPersons = 0; // Not followed ones.
	int countInvisibles = 0; // Not visible ones.
	float maxVictimSpeed;
};



// For detecting killstreaks
// Killer is the key, kill info is the value
//std::map<int, int> timeCheckedForKillStreaks;
#if KILLVECARRAY
std::vector<Kill> kills[MAX_CLIENTS_MAX + 1]; // +1 is for world
#define KILLERARRAYINDEX(k) (((k) < 0 || (k)>=max_clients) ? max_clients : (k)) 
#define KILLERARRAYINDEX_REVERSE(k) (((k) < 0 || (k)>=max_clients) ? ENTITYNUM_WORLD : (k)) 
EzBitmask<MAX_GENTITIES> clientHasKills;
#else
ankerl::unordered_dense::map<uint16_t, std::vector<Kill>, ankerl::unordered_dense::hash<uint16_t>> kills; // unordered_map should be faster. i could do array but then ... we might have to iterate over many emptyt ones. meh.
#define KILLERARRAYINDEX(k) (k)
#define KILLERARRAYINDEX_REVERSE(k) (k)
#endif
//
//std::map<int, std::vector<Kill>> kills; // unordered_map should be faster. i could do array but then ... we might have to iterate over many emptyt ones. meh.
//std::vector<Kill> kills[MAX_GENTITIES];
int timeCheckedForKillStreaks[MAX_GENTITIES] = { 0 };

#define KILLSTREAK_MIN_KILLS 3
#define KILLSTREAK_SUPERFAST_MAX_INTERVAL 1000
#define KILLSTREAK_MAX_INTERVAL 3000
#define SLOW_KILLSTREAK_MAX_INTERVAL 5000
#define VERYSLOW_KILLSTREAK_MAX_INTERVAL 7000
#define VERYVERYSLOW_KILLSTREAK_MAX_INTERVAL 9000
#define GIGASUPERVERYVERYSLOW_OPTIONAL_KILLSTREAK_MAX_INTERVAL 18000
#define OLDER_SPEEDS_STORE_LIMIT 2000 // Any speeds older than 2000ms are removed.
#define MAX_ASSUMED_SERVER_FPS 200
#define MAX_NEEDED_PAST_SPEED_SAMPLES (OLDER_SPEEDS_STORE_LIMIT*MAX_ASSUMED_SERVER_FPS/1000)



// Meta events (for logging surrounding jumps, kills etc for each kill for music sync)




class MetaEvent {

	MetaEvent* previous = NULL;
	bool destroyPrevious = true;
	int timeDelta = 0;

	int64_t demoTime; // Relative to the kill
	metaEventType_t type;
	friend class MetaEventTracker;
public:
	~MetaEvent() {
		if (destroyPrevious) { // Avoid recursion. This loop is only called on the latest kill that's being deleted.

			MetaEvent* previous = this->previous;
			MetaEvent* prevTmp;
			while (previous) {
				prevTmp = previous->previous;
				previous->destroyPrevious = false; // Avoid recursion.
				delete previous;
				previous = prevTmp;
			}
		}
	}
	MetaEvent(int64_t demoTimeA, metaEventType_t typeA, MetaEvent* previousA, int timeDeltaA) {
		demoTime = demoTimeA;
		type = typeA;
		previous = previousA;
		timeDelta = timeDeltaA;
	}
};

// Kill object for metaevent tracking
// These are chained together in a slightly nonintuitive way.
// For each player there is a pointer saying NULL. 
// If a kill is "logged" this way, a new Kill_ME is created, the old pointer is saved in "previous"
// and the new kill becomes the pointer.
// That way we can always go back in time to an appropriate amount of time to find past kills.
// Once a kill is detected as past a certain age, we delete it
// It then goes back through the chain of "previous" and deletess them all.
// The destructor also dumps all the meta info into the actual query.
// This means we HAVE to destroy all the kills eventually for it to even work.
// Idk how efficient this really is but seems like a fun thing to try.
#define METAEVENT_RESULTING_CAPTURE_TRACKING_MAX_DELAY 5000
#define METAEVENT_RESULTING_LAUGH_TRACKING_MAX_DELAY 7000
class MetaEventTracker { // kill for meta events
	MetaEventTracker* previous = NULL;
	SQLDelayedQueryWrapper_t* queryWrapper = NULL;
	bool destroyPrevious = true;
	std::vector<MetaEventItem> metaEvents;
	int64_t demoTime;
	int64_t demoTimeStart;
	int bufferTimeReal;
	//bool trackResultingCaptures = false; 
	int trackResultingMetaEventsBitMask = 0; 
	
	
	inline void addEvent(MetaEvent* metaEventAbs) {
		int delta = (int)(metaEventAbs->demoTime - demoTime);
		MetaEventItem metaEvent{ delta ,metaEventAbs->type };
		metaEvents.push_back(metaEvent);
	}
public:
	bool needsReframe = false;
	int reframeClientNum = -1; // If reframing is active, the target client number is here. When evaluating later it is checked whether this player has been followed throughout. If not, reframe.

	MetaEventTracker(int64_t demoTimeA,SQLDelayedQueryWrapper_t* queryWrapperA,MetaEventTracker* previousA, int bufferTime, int duration,int trackResultingMetaEventsBitMaskA) {
		demoTime = demoTimeA;
		queryWrapper = queryWrapperA;
		previous = previousA;
		demoTimeStart = demoTime - (int64_t)duration;
		bufferTimeReal = demoTime - std::max((int64_t)0, demoTime - (int64_t)bufferTime - (int64_t)duration);
		trackResultingMetaEventsBitMask = trackResultingMetaEventsBitMaskA;
	}
	~MetaEventTracker() {
		// Dump metaEvents into query
		if (queryWrapper) { // Let's be sure

			std::sort(metaEvents.begin(),metaEvents.end(), [](MetaEventItem a, MetaEventItem b)
				{
					return a.relativeTime < b.relativeTime;
				}
			);

			int ledToCaptures = 0;
			int ledToCapturesAfter = 0;
			int ledToTeamCaptures = 0;
			int ledToTeamCapturesAfter = 0;
			int ledToEnemyTeamCaptures = 0;
			int ledToEnemyTeamCapturesAfter = 0;
			
			int ledToLaughs = 0;
			int ledToLaughsAfter = 0;

			std::stringstream ss;

			if (metaEvents.size()) {
				// string for SQL
				ss << "{\"me\":\"";
				for (auto it = metaEvents.begin(); it != metaEvents.end(); it++) {
					ss << (it == metaEvents.begin() ? "" : ",") << metaEventKeyNames[it->type] << it->relativeTime;
					if (it->type == METAEVENT_CAPTURE && (trackResultingMetaEventsBitMask & (1 << METAEVENT_CAPTURE)) && (demoTime + it->relativeTime) > demoTimeStart && it->relativeTime < METAEVENT_RESULTING_CAPTURE_TRACKING_MAX_DELAY) { // Track self (by killer) captures resulting from thing.
						ledToCaptures++;
						if (it->relativeTime >= 0) {
							ledToCapturesAfter++;
						}
					}
					if (it->type == METAEVENT_TEAMCAPTURE && (trackResultingMetaEventsBitMask & (1 << METAEVENT_TEAMCAPTURE)) && (demoTime + it->relativeTime) > demoTimeStart && it->relativeTime < METAEVENT_RESULTING_CAPTURE_TRACKING_MAX_DELAY) { // Track team captures resulting from thing.
						ledToTeamCaptures++;
						if (it->relativeTime >= 0) {
							ledToTeamCapturesAfter++;
						}
					}
					if (it->type == METAEVENT_ENEMYTEAMCAPTURE && (trackResultingMetaEventsBitMask & (1 << METAEVENT_ENEMYTEAMCAPTURE)) && (demoTime + it->relativeTime) > demoTimeStart && it->relativeTime < METAEVENT_RESULTING_CAPTURE_TRACKING_MAX_DELAY) { // Track enemy team captures resulting from thing.
						ledToEnemyTeamCaptures++;
						if (it->relativeTime >= 0) {
							ledToEnemyTeamCapturesAfter++;
						}
					}
					if (it->type == METAEVENT_LAUGH && (trackResultingMetaEventsBitMask & (1 << METAEVENT_LAUGH)) && (demoTime + it->relativeTime) > demoTimeStart && it->relativeTime < METAEVENT_RESULTING_LAUGH_TRACKING_MAX_DELAY) { // Track team captures resulting from thing.
						ledToLaughs++;
						if (it->relativeTime >= 0) {
							ledToLaughsAfter++;
						}
					}
				}
				ss << "\"}";
				queryWrapper->query.add("@metaEvents", ss.str());
			}
			else {

				queryWrapper->query.add("@metaEvents", SQLDelayedValue_NULL);
			}

			// String for .bat
			if (queryWrapper->batchString1.size() || queryWrapper->batchString2.size()) {

				ss.str("");
				ss << " --meta \"{\\\"hl\\\":"<< bufferTimeReal <<",\\\"me\\\":\\\"";
				for (auto it = metaEvents.begin(); it != metaEvents.end(); it++) {
					ss << (it == metaEvents.begin() ? "" : ",") << metaEventKeyNames[it->type] << (bufferTimeReal+it->relativeTime); // TODO Compensate for demo not starting at exact correct millisecond.
				}
				ss << "\\\"}\"";

				if (reframeClientNum != -1 && needsReframe) {
					ss << " --reframe " << reframeClientNum;
				}

				queryWrapper->batchSuffix = ss.str();
			}

			if (ledToCaptures >= 1) {
				queryWrapper->batchMiddlePart << "_LTSC";
				if (ledToCapturesAfter == ledToCaptures) {
					queryWrapper->batchMiddlePart << "A";
					if (ledToCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToCaptures;
					}
				}
				else {
					if (ledToCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToCaptures;
					}
					if (ledToCapturesAfter > 0) {
						queryWrapper->batchMiddlePart << "_LTSCA";
						if (ledToCapturesAfter > 1) {
							queryWrapper->batchMiddlePart << ledToCapturesAfter;
						}
					}
				}
				queryWrapper->query.add("@resultingSelfCaptures", ledToCaptures);
				queryWrapper->query.add("@resultingSelfCapturesAfter", ledToCapturesAfter);
			}
			else {
				queryWrapper->query.add("@resultingSelfCaptures", SQLDelayedValue_NULL);
				queryWrapper->query.add("@resultingSelfCapturesAfter", SQLDelayedValue_NULL);
			}
			if (ledToTeamCaptures >= 1) {
				queryWrapper->batchMiddlePart << "_LTC";
				if (ledToTeamCaptures == ledToTeamCapturesAfter) {
					queryWrapper->batchMiddlePart << "A";
					if (ledToTeamCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToTeamCaptures;
					}
				}
				else {
					if (ledToTeamCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToTeamCaptures;
					}
					if (ledToTeamCapturesAfter > 0) {
						queryWrapper->batchMiddlePart << "_LTCA";
						if (ledToTeamCapturesAfter > 1) {
							queryWrapper->batchMiddlePart << ledToTeamCapturesAfter;
						}
					}
				}
				queryWrapper->query.add("@resultingCaptures", ledToTeamCaptures);
				queryWrapper->query.add("@resultingCapturesAfter", ledToTeamCapturesAfter);
			}
			else {
				queryWrapper->query.add("@resultingCaptures", SQLDelayedValue_NULL);
				queryWrapper->query.add("@resultingCapturesAfter", SQLDelayedValue_NULL);
			}
			if (ledToEnemyTeamCaptures >= 1) {
				queryWrapper->batchMiddlePart << "_LTEC";
				if (ledToEnemyTeamCaptures == ledToEnemyTeamCapturesAfter) {
					queryWrapper->batchMiddlePart << "A";
					if (ledToEnemyTeamCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToEnemyTeamCaptures;
					}
				}
				else {
					if (ledToEnemyTeamCaptures > 1) {
						queryWrapper->batchMiddlePart << ledToEnemyTeamCaptures;
					}
					if (ledToEnemyTeamCapturesAfter > 0) {
						queryWrapper->batchMiddlePart << "_LTECA";
						if (ledToEnemyTeamCapturesAfter > 1) {
							queryWrapper->batchMiddlePart << ledToEnemyTeamCapturesAfter;
						}
					}
				}
				queryWrapper->query.add("@resultingEnemyCaptures", ledToEnemyTeamCaptures);
				queryWrapper->query.add("@resultingEnemyCapturesAfter", ledToEnemyTeamCapturesAfter);
			}
			else {
				queryWrapper->query.add("@resultingEnemyCaptures", SQLDelayedValue_NULL);
				queryWrapper->query.add("@resultingEnemyCapturesAfter", SQLDelayedValue_NULL);
			}
			if (ledToLaughs >= 1) {
				queryWrapper->batchMiddlePart << "_LGH";
				if (ledToLaughs == ledToLaughsAfter) {
					queryWrapper->batchMiddlePart << "A";
					if (ledToLaughs > 1) {
						queryWrapper->batchMiddlePart << ledToLaughs;
					}
				}
				else {
					if (ledToLaughs > 1) {
						queryWrapper->batchMiddlePart << ledToLaughs;
					}
					if (ledToLaughsAfter > 0) {
						queryWrapper->batchMiddlePart << "_LGHA";
						if (ledToLaughsAfter > 1) {
							queryWrapper->batchMiddlePart << ledToLaughsAfter;
						}
					}
				}
				queryWrapper->query.add("@resultingLaughs", ledToLaughs);
				queryWrapper->query.add("@resultingLaughsAfter", ledToLaughsAfter);
			}
			else {
				queryWrapper->query.add("@resultingLaughs", SQLDelayedValue_NULL);
				queryWrapper->query.add("@resultingLaughsAfter", SQLDelayedValue_NULL);
			}
		}

		if (destroyPrevious) { // Avoid recursion. This loop is only called on the latest kill that's being deleted.

			MetaEventTracker* previous = this->previous;
			MetaEventTracker* prevTmp;
			while (previous) {
				prevTmp = previous->previous;
				previous->destroyPrevious = false; // Avoid recursion.
				delete previous;
				previous = prevTmp;
			}
		}
	}
	inline void addPastEvents(MetaEvent* lastMetaEvent, int maxTimeDelta, bool purgeOlderEvents = true) {
		MetaEvent* current = lastMetaEvent;
		MetaEvent* old = NULL;
		while (current && current->demoTime >= this->demoTime - maxTimeDelta) { // Not using demotimeStart here because maxTimeDelta already takes into account how far into the past we must save
			if (current->demoTime >= this->demoTimeStart - current->timeDelta) {// Using demoTimeStart here because we're adding past events to a newer tracker. They can be up to "timeDelta" before the start of this tracker timespan.
				this->addEvent(current);
			}
			old = current;
			current = current->previous;
		}
		if (purgeOlderEvents && old && current) { // this is only true if we arrive at one that's too old. Otherwise current will be NULL. It will also never be true if current is the original because then old would be NULL.
			delete current; // No need to delete the ones before it as the destructor handles that automatically
			old->previous = NULL;
		}
	}
	inline void addEventRecursive(MetaEvent* metaEventAbs, int maxTimeDelta, bool purgeOlder = true) {
		MetaEventTracker* current = this;
		MetaEventTracker* old = NULL;
		while (current && current->demoTime >= metaEventAbs->demoTime - maxTimeDelta) { // Using demoTime here because we're adding new events to a past tracker. They can be up to "timeDelta" after the end of a tracker timespan.
			if (current->demoTime >= metaEventAbs->demoTime - metaEventAbs->timeDelta) {
				current->addEvent(metaEventAbs);
			}
			old = current;
			current = current->previous;
		}
		if (purgeOlder && old && current) { // this is only true if we arrive at one that's too old. Otherwise current will be NULL. It will also never be true if current == this (don't want to delete ourselves) because then old would be NULL.
			delete current; // No need to delete the ones before it as the destructor handles that automatically
			old->previous = NULL;
		}
	}
	// Mark all the democuts into the past maxTimeDelta (democut buffer time typically) as requiring a reframe (because player not followed or not visible)
	inline void setReframeRequired(int64_t demoTime, int maxTimeDelta, bool purgeOlder = true) {
		MetaEventTracker* current = this;
		MetaEventTracker* old = NULL;
		while (current && current->demoTime >= demoTime - maxTimeDelta) { // Using demoTime here because we're adding new events to a past tracker. They can be up to "timeDelta" after the end of a tracker timespan.
			current->needsReframe = true;
			old = current;
			current = current->previous;
		}
		if (purgeOlder && old && current) { // this is only true if we arrive at one that's too old. Otherwise current will be NULL. It will also never be true if current == this (don't want to delete ourselves) because then old would be NULL.
			delete current; // No need to delete the ones before it as the destructor handles that automatically
			old->previous = NULL;
		}
	}
};

typedef enum MetaEventTrackerType { // We're not gonna do it for defrag becaue we can't tell when a defrag run is starting in all cases.
	METRACKER_KILLS,
	METRACKER_KILLSPREES,
	METRACKER_CAPTURES,
	METRACKER_FLAGGRABS,
	//METRACKER_LAUGHS, // No laugh cuts with metaevents for now as they're a bit different. They aren't specific to individual players. Might wanna include at least kills at some point but we won't for now as it would complicate stuff.
	METRACKER_TOTAL_COUNT
};

int resultingMetaEventTracking[METRACKER_TOTAL_COUNT] = {
	(1 << METAEVENT_CAPTURE) | (1 << METAEVENT_TEAMCAPTURE) | (1 << METAEVENT_LAUGH),
	(1 << METAEVENT_CAPTURE) | (1 << METAEVENT_TEAMCAPTURE) | (1 << METAEVENT_LAUGH),
	(1 << METAEVENT_LAUGH),
	(1 << METAEVENT_CAPTURE) | (1 << METAEVENT_TEAMCAPTURE) | (1 << METAEVENT_ENEMYTEAMCAPTURE) | (1 << METAEVENT_LAUGH), // todo track deaths/rets as well for flaggrabs? idk
};

int64_t requiredMetaEventAges[METRACKER_TOTAL_COUNT][MAX_CLIENTS_MAX]; // demotimes of how far back we need to keep metaevents (to have enough info for full demos) for each type of metaevent tracker. the oldest is always the one that is applied so make sure to keep this up to date.
MetaEventTracker* metaEventTrackers[METRACKER_TOTAL_COUNT][MAX_CLIENTS_MAX];
MetaEvent* playerPastMetaEvents[MAX_CLIENTS_MAX];

inline int getMinimumMetaEventBufferTime(int clientNum, int bufferTime, int demoCurrentTime) {
	int64_t oldestTimeRequired = demoCurrentTime-bufferTime;
	for (int i = 0; i < METRACKER_TOTAL_COUNT; i++) {
		if (requiredMetaEventAges[i][clientNum] < oldestTimeRequired) {
			oldestTimeRequired = requiredMetaEventAges[i][clientNum];
		}
	}
	return demoCurrentTime-oldestTimeRequired;
}

inline void addMetaEvent(metaEventType_t type, int64_t demoTime, int clientNum, int timeDelta, const ExtraSearchOptions& opts, int bufferTime) {
	MetaEvent* theEvent = new MetaEvent(demoTime, type, playerPastMetaEvents[clientNum],timeDelta);
	playerPastMetaEvents[clientNum] = theEvent;
	for (int i = 0; i < METRACKER_TOTAL_COUNT; i++) {
		if (metaEventTrackers[i][clientNum]) {
			metaEventTrackers[i][clientNum]->addEventRecursive(theEvent, bufferTime, true);
		}
	}
}

inline void setReframeRequired(int64_t demoTime, int clientNum, const ExtraSearchOptions& opts, int bufferTime) {
	
	if (!opts.reframeIfNeeded) return;
	for (int i = 0; i < METRACKER_TOTAL_COUNT; i++) {
		if (metaEventTrackers[i][clientNum]) {
			metaEventTrackers[i][clientNum]->setReframeRequired(demoTime, bufferTime, true);
		}
	}
}

template <int max_clients>
inline void addMetaEventNearby(vec3_t origin, float maxDistance, metaEventType_t type, int64_t demoTime, int timeDelta, const ExtraSearchOptions& opts, int bufferTime) {
	for (int playerNum = 0; playerNum < max_clients; playerNum++) {
		if (thisFrameInfo.entityExists[playerNum] && VectorDistance(thisFrameInfo.playerPositions[playerNum], origin) <= maxDistance) {
			addMetaEvent(type, demoTime, playerNum, timeDelta, opts, bufferTime);
		}
	}
}



enum trackedEntityType_t {
	TET_NONE,
	TET_TRIPMINE,
	TET_SENTRY,
	TET_FORCEFIELD
};

#define TETFLAG_EXPLODED 1
#define TETFLAG_AIRBORNE 2

struct entityOwnerInfo_t {
	int64_t firstSeen; // Demo time of time we started tracking this item
	trackedEntityType_t type;
	int owner;
	int flags;
}; // For items like mines, we wanna track the owner. Reason: Detect stuff like boosted mine kills. No use to detect a boost for a mine kill if the mine that did the kill was fired before the boost.


struct samplePoint_t {
	float value;
	double time;
};

#define FIF_SCRIPTCHECK (1<<0) // set if no more than 10ms commandtime passed since last frame and mvement dir changed to backwards (4) but wasnt before.
struct frameInfo_t {
	int64_t demoTime;
	int serverTime;
	qboolean isAlive[MAX_CLIENTS_MAX];
	qboolean canBlockSimplified[MAX_CLIENTS_MAX];
	qboolean entityExists[MAX_GENTITIES];
	entityOwnerInfo_t entityOwnerInfo[MAX_GENTITIES];
	vec3_t playerPositions[MAX_CLIENTS_MAX];
	vec3_t playerVelocities[MAX_CLIENTS_MAX];
	vec3_t playerAngles[MAX_CLIENTS_MAX];
	samplePoint_t playerAngularVelocities[MAX_CLIENTS_MAX];
	samplePoint_t playerAngularAccelerations[MAX_CLIENTS_MAX];
	samplePoint_t playerAngularJerks[MAX_CLIENTS_MAX];
	samplePoint_t playerAngularSnaps[MAX_CLIENTS_MAX];
	//float playerGSpeeds[max_clients]; // "speed" value that basically contains g_speed
	//float playerMaxWalkSpeed[max_clients]; // Kind of naive guess of how fast this client could theoretically walk: sqrt(g_speed*g_speed+g_speed*g_speed)
#ifdef PLAYERSTATEOTHERKILLERBOOSTDETECTION
	int otherKillerValue[max_clients];
	int otherKillerTime[max_clients];
#endif
	//qboolean pmFlagKnockback[MAX_CLIENTS_MAX];
	int pmFlags[MAX_CLIENTS_MAX];
	qboolean psTeleportBit[MAX_CLIENTS_MAX];
	int pmFlagTime[MAX_CLIENTS_MAX];
	float jumpzstart[MAX_CLIENTS_MAX];
	int duelTime[MAX_CLIENTS_MAX];
	int commandTime[MAX_CLIENTS_MAX];
	int legsAnimGeneral[MAX_CLIENTS_MAX];
	int torsoAnimGeneral[MAX_CLIENTS_MAX];
	int groundEntityNum[MAX_CLIENTS_MAX];
	int psStats12[MAX_CLIENTS_MAX];
	int saberMoveGeneral[MAX_CLIENTS_MAX];
	int movementDir[MAX_CLIENTS_MAX];
	int frameInfoFlags[MAX_CLIENTS_MAX];
}; 

frameInfo_t lastFrameInfo;

frameInfo_t thisFrameInfo;


inline void TET_LastSeenUpdate(int entityNum, int demoCurrentTime) {
	if (lastFrameInfo.entityExists[entityNum] &&
		thisFrameInfo.entityOwnerInfo[entityNum].type == lastFrameInfo.entityOwnerInfo[entityNum].type &&
		thisFrameInfo.entityOwnerInfo[entityNum].owner == lastFrameInfo.entityOwnerInfo[entityNum].owner) {

		thisFrameInfo.entityOwnerInfo[entityNum].firstSeen = lastFrameInfo.entityOwnerInfo[entityNum].firstSeen;
	}
	else {
		thisFrameInfo.entityOwnerInfo[entityNum].firstSeen = demoCurrentTime;
	}
}



// Demo stats
typedef struct demoPeriodPacketStats_t {
	int totalPacketsSize;
	int totalSnapshotSize;
	int totalServerCommandSize;
	int droppedPackets;
	int totalPacketCount;
	int periodTotalTime;
	int maxPacketSize;
	int minPacketSize;
	int nonDeltaSnapshotCount;
	int angleChanges;
	int entitiesReceivedTotal;
};
demoPeriodPacketStats_t currentPacketPeriodStats;
int64_t lastPacketStatsWritten = 0;




static inline void resetCurrentPacketPeriodStats() {
	Com_Memset(&currentPacketPeriodStats, 0, sizeof(currentPacketPeriodStats));
	currentPacketPeriodStats.minPacketSize = INT_MAX;
}

enum BoostDetectionType {
	BOOST_NONE,
	BOOST_PLAYERSTATE,
	BOOST_PLAYERSTATE_KNOCKBACK_FLAG,
	BOOST_ENTITYSTATE_GUESS_VERTICAL,
	BOOST_ENTITYSTATE_GUESS_HORIZONTAL,
	BOOST_ENTITYSTATE_GUESS_HORIZONTAL_AND_VERTICAL,
};



#define BOOST_DETECT_MAX_AGE 5000
#define BOOST_DETECT_MIN_TRAVEL_DISTANCE 100.0f // If the boost didn't send us flying at least 100 units... just ignore it. We were probably either standing on the ground and just slightly pushed aside, or pushed against a wall or some other random irrelevant thing. Otherwise we just get way too many misdetects.
#define BOOST_DETECT_MIN_TRAVEL_DISTANCE_LOW 50.0f // This is the distance we will check was travel if BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY is reached.
#define BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY 600.0f // If the velocity after the boost exceeds this value, we ill check only against BOOST_DETECT_MIN_TRAVEL_DISTANCE_LOW
#define BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY_ABSOLUTE 800.0f // If the velocity after the boost exceeds this value, we will automatically confirm the boost and not need to check travel distance. Reason is: Might super fast boost but get ignored because of a quick ground touch for strafe. This is a decent compromise I think.
#define BOOST_DETECT_MAX_AGE_WALKING 1000 // If we are just walking (our velocity doesn't exceeed maximum walk speed), what age of boosts do we allow for consideration?
#define BOOST_PS_VERTICAL_SPEED_DELTA_DETECT_THRESHOLD ((float)(JUMP_VELOCITY/2)) // Vertical speed delta that must be detected for a boost (detected in playerstate) to be considered relevant (this isn't required if horizontal speed is above walking speed). Just set it to the minimum possible jump velocity/2. Just wanna avoid detecting micro-boosts that have no real impact on anything. 100-ish still won't be very meaningful usually but in some edge situations it might?
#define BLUEUPPERCUT_MAX_VELOCITY_DELTA_HORZ 150.0f // There are honestly so many different things that can give you a horizontal boost it's almost impossible to account for them all. But from my experience playing around, it seems very few if any non-boost situations speed you up by more than 150 in a single frame.
#define BOOST_ENTITY_HORIZONTAL_DELTA_DETECT_THRESHOLD BLUEUPPERCUT_MAX_VELOCITY_DELTA_HORZ // There are honestly so many different things that can give you a horizontal boost it's almost impossible to account for them all. But from my experience playing around, it seems very few if any non-boost situations speed you up by more than 150 in a single frame. That also happens to be the maximum horizontal boost you get from blue uppercut. Additionally though, we need to check for DFA, since it can give you a 300 forward boost
#define BACKFLIP_MAX_VELOCITY_DELTA_VERT ((float)(JUMP_VELOCITY+128))
#define YELLOWDFA_MAX_VELOCITY_DELTA_VERT 400.0f
#define DFA_MAX_VELOCITY_DELTA_VERT 280.0f
#define DFA_MAX_VELOCITY_DELTA_HORZ 300.0f


class boost_t {
public:
	int64_t demoTime = -1;
	vec3_t startPosition{ 0,0,0 };
	vec3_t velocityDeltaVector{ 0,0,0 }; // To make sure that the minimum travel distance was traveled in the direction of the boost?  Worth a try
	qboolean confirmed = qfalse; // This is set to true the first time we touch ground after the boost. That is when we test whether we have actually travelled a meaningful distance. 
	// Ok but what if we touch ground very briefly after the boost for strafe? Hmm idk. Do we go more permissive direction? Maybe check whether the distance travelled is in reasonable relationship to velocity? Hmm that doesnt really work either

	float minimumTravelDistance = BOOST_DETECT_MIN_TRAVEL_DISTANCE; // This is the default to check against.

	int estimatedStrength = -1; // -1 if unknown
	int boostedClientNum = -1;
	int boosterClientNum = -1; // -1 if unknown
	BoostDetectionType detectType = BOOST_NONE;
	int isEnemyBoost = -1; // -1 if unknown

	qboolean facingTowards[MAX_CLIENTS_MAX] = {}; // For each 

	// The reason we put a higher requirement on the slow speed boost is simple: High speed boosts are simply more interesting and we don't wanna lose them by accident.
	inline void autoSetMinimumTravelDistance(const float boostSpeed) {
		if (boostSpeed > BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY_ABSOLUTE) {
			confirmed = qtrue; // Nothing more to do.
		} else if (boostSpeed > BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY) {
			minimumTravelDistance = BOOST_DETECT_MIN_TRAVEL_DISTANCE_LOW;
		}
		else {
			minimumTravelDistance = BOOST_DETECT_MIN_TRAVEL_DISTANCE;
		}
	}

	// 
	template<unsigned int max_clients>
	inline void setFacingTowards(const frameInfo_t* referenceFrame, const vec3_t boostVelocityDelta) {
		if (boostedClientNum == -1 || detectType == BOOST_NONE) {
			throw std::logic_error("Internal coding error: boostedClientNum and detectType must be set before calling setFacingTowards()");
		}

		if (detectType >= BOOST_PLAYERSTATE && detectType <= BOOST_PLAYERSTATE_KNOCKBACK_FLAG) return; // We only do this check for entity boosts. For playerstate boosts we know who boosted us so we can avoid useless boosts (like results from fighting against each other)
		
		if (!referenceFrame->entityExists[boostedClientNum]) {
			throw std::logic_error("Internal coding error: boostedClientNum entity doesn't exist in referenceFrame in setFacingTowards()");
		}

		for (int i = 0; i < max_clients; i++) {
			if (referenceFrame->entityExists[i]) { // If it doesn't exist, we will just assume we're not facing towards it. Better avoid misdetects than to avoid missing out. Or we end up cluttered with nonsense invalid boosts
				static vec3_t vectorTowardsOtherClient;
				VectorSubtract(referenceFrame->playerPositions[i], referenceFrame->playerPositions[boostedClientNum], vectorTowardsOtherClient);

				// Dot product tells us, roughly speaking, whether two vectors are pointing in the same direction
				// Or rather, it gives us the degree to which a vector extends along the length of another?
				// Either way this should do the trick. In some other circumstances we might normalize the vectors first,
				// but we don't care about absolute values, only whether > 0 so it doesn't really matter.
				float dot = DotProduct(vectorTowardsOtherClient, boostVelocityDelta); 
				if (dot > 0) {
					facingTowards[i] = qtrue;
				}
			}
		}
		facingTowards[boostedClientNum] = qtrue; // Erm... I guess if it was a suicide? Always allow then? Idk. Deal with this some other time.
	}

	inline qboolean checkFacingTowards(const int clientNum) {
		if (boostedClientNum == -1 || detectType == BOOST_NONE) {
			throw std::logic_error("Internal coding error: boostedClientNum and detectType must be set before calling checkFacingTowards()");
		}
		if (detectType >= BOOST_PLAYERSTATE && detectType <= BOOST_PLAYERSTATE_KNOCKBACK_FLAG) {
			return qtrue; // We only do this check for entity boosts. For playerstate boosts we know who boosted us so we can avoid useless boosts (like results from fighting against each other)
		}
		else if(detectType >= BOOST_ENTITYSTATE_GUESS_VERTICAL && detectType <= BOOST_ENTITYSTATE_GUESS_HORIZONTAL_AND_VERTICAL) {

			// But what if we just didn't know at the time?
			// Too bad. Better avoid misdetects.
			return facingTowards[clientNum]; 
		}
		throw std::logic_error("Internal coding error: checkFacingTowards() didn't cover detect type.");
	}
	
	// Instead of just checking if the total traveled distance is above minimum travel distance to be considered a real boost,
	// we check if that traveled distance happened *in the direction of the boost*.
	// Hence the dot product of the travel vector against the normalized bosot speed delta vector
	inline qboolean checkDistanceTraveled(vec3_t currentPosition) {
		vec3_t positionDeltaVector,deltaVectorNormalized;
		VectorCopy(velocityDeltaVector, deltaVectorNormalized);
		VectorNormalize(deltaVectorNormalized);
		VectorSubtract(currentPosition,startPosition,positionDeltaVector);
		float dot = DotProduct(deltaVectorNormalized, positionDeltaVector);
		return (qboolean)(dot > minimumTravelDistance);
	}
};

std::vector<boost_t> boosts;




int64_t lastBackflip[MAX_CLIENTS_MAX];
int64_t lastSelfSentryJump[MAX_CLIENTS_MAX];

int headJumpCount[MAX_CLIENTS_MAX]; // Jumps over player heads
int specialJumpCount[MAX_CLIENTS_MAX]; // Jumps over items in certain situations (over top of forcefield or midair sentry for example). Not yet implemented.

// groundframe tracking
#define TRACK_GROUNDFRAME 1
#define TRACK_GROUNDFRAME_DEBUG 1
#if TRACK_GROUNDFRAME
#define GROUNDFRAME_MINDURATION 10 // arbitrary?
#define GROUNDFRAME_MAXDURATION 250 // arbitrary? (1-t*6)^(1/t) shows that remaining speed of groundframe at 0.3s is like 0.165 or so. like 0.3 at 0.2s. lets go in the middle. slower than that and we may as well be walking
#define GROUNDFRAME_MAXPASTCOUNT 1000 // arbitrary?
int64_t lastGroundFrame[MAX_CLIENTS_MAX]; 
int64_t lastPreGroundFrameAir[MAX_CLIENTS_MAX]; // commandtime
vec3_t lastPreGroundFrameVelocity[MAX_CLIENTS_MAX];
vec3_t lastPreGroundFramePosition[MAX_CLIENTS_MAX];
float lastGroundFrameQuality[MAX_CLIENTS_MAX];
float lastGroundFrameAngleChange[MAX_CLIENTS_MAX];
#if TRACK_GROUNDFRAME_DEBUG
typedef struct groundframeDebug_t {
	float	oldSpeed;
	float	oldSpeedInNewDir;
	float	newSpeed;
	vec3_t	oldVel;
	vec3_t	newVel;
	vec3_t	lastGroundVel;
	float	groundFrameDurationEstimate;
};
groundframeDebug_t lastGroundFrameDebug[MAX_CLIENTS_MAX];
#endif
#endif

//int64_t walkDetectedTime[max_clients];
std::vector<int64_t> walkDetectedTimes[MAX_CLIENTS_MAX];
size_t walkDetectedTimesObsoleteIndex[MAX_CLIENTS_MAX] = { 0 };

inline bool demoTimeSmallerPredicate(const int64_t& curTime, const float& demoTime) {
	return curTime < demoTime;
};


struct {
	int maxForceJumpLevel;
	float maxForceJumpVelocityDelta;
} forcePowersInfo;


// For calculating top/average speed of past second.
// Top level key is clientNum
// Lower level key is time
// Value is speed.
//std::map<int, std::map<int, float>> speeds;
//std::map<int, float> speeds[MAX_CLIENTS];
struct Speed {
	int64_t time;
	float speed;
	float angularSpeed;
	float angularAcceleration;
	float angularJerk;
	float angularSnap;
};

std::vector<Speed> speeds[MAX_CLIENTS_MAX];
// TODO For future? Have angle saved too. See at what angle the kill comes. Big angle between capper and chaser at high speed is more impressive?


enum highlightSearchMode_t {
	SEARCH_ALL,
	SEARCH_INTERESTING,
	SEARCH_MY_CTF_RETURNS,
	SEARCH_CTF_RETURNS,
	SEARCH_ALL_MY_KILLS,
	SEARCH_ALL_KILLS,
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
	SEARCH_ALL_DEFRAG, // All defrags, even if not top 10
};


// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;


tsl::htrie_map<char, int> playerNamesToClientNums;
qboolean clientNameIsDuplicate[MAX_CLIENTS_MAX];
std::set<std::string>	recorderPlayerNames;

#define DERR_SEH				(1<<0)
#define DERR_OPENFAIL			(1<<1)
#define DERR_FAILREADSEQNUM		(1<<2)
#define DERR_DUPEMSGNUM			(1<<3)
#define DERR_MSGSIZEOVERMAX		(1<<4)
#define DERR_FAILREADMSG		(1<<5)
#define DERR_READPASTEND		(1<<6)
#define DERR_ILLEGIBLESRVMSG	(1<<7)
#define DERR_FAILPARSECMDSTR	(1<<8)
#define DERR_FAILPARSEGAMESTATE	(1<<9)
#define DERR_FAILPARSESNAP		(1<<10)
#define DERR_SNAPMALFORMED		(1<<11)
#define DERR_ERRORCNFSTRMOD		(1<<12)
#define DERR_WEIRDMSGSIZE		(1<<13)
#define DERR_MSGSIZEENDBUTLEFT	(1<<14)
#define DERR_BADJSONMETA		(1<<15)
#define DERR_GAMELOGICFLAW		(1<<16)
//#define DERR_THRGHWALLBOXSOLID	(1<<15) // kill-based
int	demoErrorFlags = 0;
std::stringstream	demoErrors;

#define DERIV_FAKEDEMO		(1<<0) // dentified by fake demo server name. could be reframe merge etc
#define DERIV_SNAPSMANIP	(1<<1) // snaps manipulation by demo optimizer
int	demoDerivativeFlags = 0;

// This also updates the playerNamesToClientNums trie
template<unsigned int max_clients>
void updatePlayerDemoStatsArrayPointers(demoType_t demoType, const ExtraSearchOptions& opts) {

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if (isMOHAADemo) {
		playerNamesToClientNums.clear();
		Com_Memset(clientNameIsDuplicate, 0, sizeof(clientNameIsDuplicate));
	}

	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	for (int i = 0; i < max_clients; i++) {
		stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + i];
		const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");

		if (i == demo.cut.Clc.clientNum) {
			recorderPlayerNames.insert(playerName);
		}

		if (isMOHAADemo && *playerInfo) { // Don't insert non existing players.
			auto existingName = playerNamesToClientNums.find(playerName.c_str());
			if (existingName != playerNamesToClientNums.end()) {
				int otherClientNum = existingName.value();
				clientNameIsDuplicate[otherClientNum] = qtrue;
				clientNameIsDuplicate[i] = qtrue;
				if (playerLastSeen[i] > playerLastSeen[otherClientNum]) {
					// Prioritize this player because he was seen more recently (duplicates could be lagged out connections). SHITTY solution, especially since it's only triggered during CS change.
					// TODO FIX THIS omg
					playerNamesToClientNums[playerName.c_str()] = i;
				}
			}
			else {
				playerNamesToClientNums[playerName.c_str()] = i; // The lowest client number with that name will be accepted... kinda dumb but idk.
			}
		}

		if (opts.doStatsDb) {
			playerDemoStatsMapKey_t mapPointer(mapname, playerName, i); // We can think of this as something of a soft (not really hard enforced in database) primary key to distinguish players.
			playerDemoStatsPointers[i] = &playerDemoStatsMap[mapPointer];
		}
	}
}

void freedomDefragFillInfo(defragRunInfo_t* info, finishedRunInfo_t* runInfo) {
	info->milliseconds = runInfo->milliseconds;
	if (runInfo->userId != -1) {
		info->defragPlayerName = runInfo->username;
	}
	if (runInfo->pbStatus != -1 && runInfo->pbStatus & PB_LB) {
		info->isNumber1 = (qboolean)(runInfo->rankLB == 1);
		info->isLogged = (qboolean)(runInfo->userId != -1);
		info->isPersonalBest = qtrue;
		info->exactRank = runInfo->rankLB;
	}
	info->knownClientNum = runInfo->clientNum;
	info->courseName = runInfo->subcoursename;
	info->style = runInfo->stylename;
	info->playerName = runInfo->playerName;
}

ankerl::unordered_dense::map< int, finishedRunInfo_t, ankerl::unordered_dense::hash<int>> freedomDefrag_unrankedRuns;
template<unsigned int max_clients>
qboolean findFreedomDefragRun(std::string printText, defragRunInfoFinal_t* info, qboolean flush, int64_t demoCurrentTime, int64_t lastGameStateChangeInDemoTime, demoType_t demoType, int CS_PLAYERS_here) {
	if (flush) {
		if (freedomDefrag_unrankedRuns.size() > 0) {
			auto first = freedomDefrag_unrankedRuns.begin();
			std::cerr << "Flushing run with id " << first->second.runId << ". No ranked variant exists.";
			freedomDefragFillInfo(&info->info, &first->second);
			info->meta = first->second.meta;
			freedomDefrag_unrankedRuns.erase(first);
			return qtrue;
		}
		return qfalse;
	}
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	const char* extraArg;
	if (Cmd_Argc() < 3) {
		return qfalse;
	}
	extraArg = (const char*)Cmd_Argv(2);
	bool isRanked = !_stricmp(extraArg, "dffinish_ranked");
	if (!_stricmp(extraArg, "dffinish") || isRanked) {
		finishedRunInfo_t runInfo;
		runInfo.runId = atoi(Cmd_Argv(3));
		runInfo.clientNum = atoi(Cmd_Argv(4));
		runInfo.userId = atoi(Cmd_Argv(5));
		runInfo.milliseconds = atoi(Cmd_Argv(6));
		runInfo.levelTimeStart = atoi(Cmd_Argv(7));
		runInfo.levelTimeEnd = atoi(Cmd_Argv(8));
		runInfo.endCommandTime = atoi(Cmd_Argv(9));
		runInfo.startLessTime = atoi(Cmd_Argv(10));
		runInfo.endLessTime = atoi(Cmd_Argv(11));
		runInfo.warningFlags = atoi(Cmd_Argv(12));
		runInfo.topspeed = atof(Cmd_Argv(13));
		runInfo.average = atof(Cmd_Argv(14));
		runInfo.distance = atof(Cmd_Argv(15));
		runInfo.distanceXY = atof(Cmd_Argv(16));
		runInfo.raceStyle.movementStyle = atoi(Cmd_Argv(17));
		runInfo.raceStyle.msec = atoi(Cmd_Argv(18));
		runInfo.raceStyle.jumpLevel = atoi(Cmd_Argv(19));
		runInfo.raceStyle.variant = atoi(Cmd_Argv(20));
		runInfo.raceStyle.runFlags = atoi(Cmd_Argv(21));
		runInfo.savePosCount = atoi(Cmd_Argv(22));
		runInfo.resposCount = atoi(Cmd_Argv(23));
		runInfo.lostMsecCount = atoi(Cmd_Argv(24));
		runInfo.lostPacketCount = atoi(Cmd_Argv(25));
		runInfo.placeHolder1 = atoi(Cmd_Argv(26));
		runInfo.placeHolder2 = atoi(Cmd_Argv(27));
		runInfo.placeHolder3 = atoi(Cmd_Argv(28));
		runInfo.placeHolder4 = atoi(Cmd_Argv(29));
		runInfo.millisecondsSegmentedTotal = atoi(Cmd_Argv(30));
		runInfo.rollSpeed = atof(Cmd_Argv(31));
		runInfo.rollTakeoffClientSpeed = atoi(Cmd_Argv(32));
		runInfo.startTriggerSpeed = atof(Cmd_Argv(33));
		runInfo.pbStatus = atoi(Cmd_Argv(34));
		runInfo.rankLB = atoi(Cmd_Argv(35));
		strcpy_s(runInfo.coursename,sizeof(runInfo.coursename), Cmd_Argv(36));
		strcpy_s(runInfo.username,sizeof(runInfo.username), Cmd_Argv(37));
		runInfo.unixTimeStampShifted = atoi(Cmd_Argv(38));
		runInfo.unixTimeStampShiftedBillionCount = atoi(Cmd_Argv(39));
		runInfo.lbType = (mainLeaderboardType_t)atoi(Cmd_Argv(40));
		strcpy_s(runInfo.subcoursename, sizeof(runInfo.subcoursename), Cmd_Argv(41)); 
		
		int stringOffsetB = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + runInfo.clientNum];
		const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffsetB;
		const char* playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffsetB, isMOHAADemo ? "name" : "n");
		runInfo.playerName = std::string(playerName);

		int offsetstylestart = 0;
		int offsetstyleend = 0;
		const char* thetext = printText.c_str();
		while (*thetext && *thetext != '[') {
			thetext++;
			offsetstylestart++;
		}
		if (*thetext) {
			thetext++;
			offsetstylestart++;
		}
		offsetstyleend = offsetstylestart;
		while (*thetext && *thetext != ']') {
			thetext++;
			offsetstyleend++;
		}
		if (offsetstylestart > 1 && offsetstyleend > 1 && offsetstylestart != offsetstyleend) {
			runInfo.stylename = printText.substr(offsetstylestart, offsetstyleend- offsetstylestart);
		}

		auto existingRun = freedomDefrag_unrankedRuns.find(runInfo.runId);
		if (isRanked) {

			freedomDefragFillInfo(&info->info, &runInfo);
			if (existingRun == freedomDefrag_unrankedRuns.end()) {
				std::cerr << "WTF ranked freedom defrag run with id " << runInfo.runId << " is not known!";
			}
			else {
				if (existingRun->second.clientNum != runInfo.clientNum || existingRun->second.levelTimeStart != runInfo.levelTimeStart || existingRun->second.userId != runInfo.userId || existingRun->second.levelTimeEnd != runInfo.levelTimeEnd || existingRun->second.milliseconds != runInfo.milliseconds) {
					// just a few santiy checks
					std::cerr << "WTF ranked freedom defrag run with id " << runInfo.runId << " mismatch with unranked version!";
				}
				else if(existingRun->second.meta.isSet) {
					// get it from the original since the ranked one is delayed (async)
					info->meta = existingRun->second.meta;
					//runInfo.stylename = existingRun->second.stylename;
					info->info.style = existingRun->second.stylename; // cuz the ranked can be blank sometimes with no text.
				}
				else {
					std::cerr << "WTF ranked freedom defrag run with id " << runInfo.runId << " has an unranked version without meta!";
				}
				freedomDefrag_unrankedRuns.erase(existingRun);
			}
			if (!info->meta.isSet) { // oh well, can't help it, for some reason we only have the current info. so its all guesswork from here on out and could go horribly wrong
				int64_t guessedDemoTimeOffset = std::min((int64_t)runInfo.levelTimeEnd - (int64_t)demo.cut.Cl.snap.serverTime, (int64_t)0);
				int64_t guessedDemoTime= demoCurrentTime + guessedDemoTimeOffset;
				int64_t guessedLastGameStateChangeInDemoTime = lastGameStateChangeInDemoTime < guessedDemoTime ? lastGameStateChangeInDemoTime : 0; // UGLY
				GetRunMeta<max_clients>(info->info, info->meta, demoCurrentTime + guessedDemoTimeOffset, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here);
			}
			return qtrue;
		}
		else {
			if (existingRun != freedomDefrag_unrankedRuns.end()) {
				std::cerr << "WTF freedom defrag run with id " << runInfo.runId << " is already logged!";
			}
			freedomDefragFillInfo(&info->info, &runInfo); // just so we can properly get the runmeta
			GetRunMeta<max_clients>(info->info, runInfo.meta, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here);
			freedomDefrag_unrankedRuns[runInfo.runId] = runInfo;
			return qfalse;
		}

	}
	return qfalse;
}

qboolean findOCDefragRun(std::string printText, defragRunInfo_t* info) {
	jp::VecNum vec_num;
	jp::RegexMatch rm;


	size_t count = rm.setRegexObject(&defragRecordFinishRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
		info->playerName = vec_num[matchNum][1];
		int minutes = atoi(vec_num[matchNum][2].c_str());
		//std::string secondString = vec_num[matchNum][3];
		float seconds = atof(vec_num[matchNum][3].c_str());
		int milliSeconds = (1000.0f * seconds) + 0.25f;
		int pureMilliseconds = milliSeconds % 1000;
		int pureSeconds = milliSeconds / 1000;

		info->milliseconds = milliSeconds + minutes * 60 * 1000;
		info->isLogged = (qboolean)(vec_num[matchNum][5].length() > 0);
		info->isNumber1 = (qboolean)(vec_num[matchNum][7].length() > 0);
		info->isPersonalBest = (qboolean)(vec_num[matchNum][4].length() > 0);

		return qtrue;
	}
	return qfalse;
}


static constexpr auto jaPRONameColorClientNumMap{ []() constexpr {
	std::array<int64_t, 8> finalMap{};
	for (int clientNum = 0; clientNum < 64; clientNum++) {
		// JAPRO time report name coloring code
		int nameColor = 7 - (clientNum % 8);//sad hack
		if (nameColor < 2)
			nameColor = 2;
		else if (nameColor > 7 || nameColor == 5)
			nameColor = 7;
		finalMap[nameColor] |= (1L << clientNum);
	}
	
	return finalMap;
}() };

template<int max_clients>
qboolean findJAProDefragRun(std::string printText, defragRunInfo_t* info, demoType_t demoType) {
	jp::VecNum vec_num;
	jp::VecNas vec_nas;
	jp::RegexMatch rm;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	size_t count = rm.setRegexObject(&defragJaProFinishRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
		.setNamedSubstringVector(&vec_nas)         //pass pointer to VecNum vector
		.match();

	auto test = jaPRONameColorClientNumMap.begin();
	
	for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever

		static int CS_PLAYERS_here = getCS_PLAYERS(demoType);

		info->playerName = vec_nas[matchNum]["name"];

		int hours = atoi(vec_nas[matchNum]["hours"].c_str());
		int minutes = atoi(vec_nas[matchNum]["minutes"].c_str());
		int seconds = atoi(vec_nas[matchNum]["seconds"].c_str());
		int milliSeconds = atoi(vec_nas[matchNum]["msec"].c_str());

		int color = atoi(vec_nas[matchNum]["color"].c_str());
		int nameColor = atoi(vec_nas[matchNum]["nameColor"].c_str());

		// Runs with run teles
		info->isProRun = (qboolean)( strlen(vec_nas[matchNum]["prorun"].c_str()) > 0);
		info->teleports = atoi(vec_nas[matchNum]["tps"].c_str());
		info->checkpoints = atoi(vec_nas[matchNum]["cps"].c_str());

		//if (info->isProRun || info->teleports || info->checkpoints) {
		//	std::cout << "Run with run teles found\n";
		//}

		const char* recordType = vec_nas[matchNum]["recordType"].c_str();

		info->milliseconds = milliSeconds + seconds*1000 + minutes * 60 * 1000 + hours * 60 * 60 * 1000;
		info->isLogged = (qboolean)(color == 5);
		info->isValid = (qboolean)(color == 5 || color == 2);
		info->isNumber1 = (qboolean)(!_stricmp(recordType, "WR"));
		info->isPersonalBest = (qboolean)(!_stricmp(recordType,"WR") || !_stricmp(recordType,"SR+PB") || !_stricmp(recordType,"PB"));
		info->exactRank = info->isPersonalBest ? atoi(vec_nas[matchNum]["newRank"].c_str()) : -1;
		info->style = vec_nas[matchNum]["style"];
		info->courseName = vec_nas[matchNum]["c"][0] == 'C' ? "" : vec_nas[matchNum]["mapname"];
		int64_t clientNumBitmask = jaPRONameColorClientNumMap[nameColor];


		bool isPossiblyRealName = color != 5;

		int playerNumber = -1;
		std::vector<int> possibleClientNums;
		for (int clientNum = 0; clientNum < max_clients; clientNum++) {

			if (!(clientNumBitmask & (1L << clientNum))) { // Color coding indicates this definitely wasn't the right player.
				continue;
			}

			int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNum];
			const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;

			if (!*playerInfo) continue;

			char* team = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "team" : "t");

			if (!*team || atoi(team)==3) continue;

			possibleClientNums.push_back(clientNum);
			if (isPossiblyRealName) {
				std::string playerNameCompare = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");
				std::string playerNameCompareStripped = Q_StripColorAll(playerNameCompare);
				if (playerNameCompare == info->playerName) {
					playerNumber = clientNum;
				}
			}
		}
		if (playerNumber == -1 && possibleClientNums.size() > 0) {
			// For logged in users, we can only reliably determine whose run it was if only one active player matched the color coding
			// or if we are following the player and can verify that his run ended on this frame (although that's kind of dirty as theoretically it could be different players still but oh well)
			if (possibleClientNums.size() == 1) {
				// Only one player matches the color coding. Good. Ez.
				playerNumber = possibleClientNums[0];
			}
			else {

				if (lastFrameInfo.duelTime[demo.cut.Cl.snap.ps.clientNum] && !demo.cut.Cl.snap.ps.duelTime && demo.cut.Cl.snap.ps.stats[11]) { // Was running last frame but not anymore. stats[11] means racemode in japro

					for (int i = 0; i < possibleClientNums.size(); i++) {

						if (possibleClientNums[i] == demo.cut.Cl.snap.ps.clientNum) {

							playerNumber = demo.cut.Cl.snap.ps.clientNum;
							break;
						}
					}
				}
			}
		}

		if (playerNumber != -1) {
			info->knownClientNum = playerNumber;
		}

		return qtrue;
	}
	return qfalse;
}

qboolean razorDefragLastCheckedPrintWasPersonalBest = qfalse;

qboolean findRazorDefragRun(std::string printText, defragRunInfo_t* info) {
	jp::VecNum vec_num, vec_num2;
	jp::RegexMatch rm, rm2;

	qboolean lastWasPersonalBestMessage = razorDefragLastCheckedPrintWasPersonalBest;
	razorDefragLastCheckedPrintWasPersonalBest = qfalse;

	size_t count = rm.setRegexObject(&defragRazorFinishRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
		info->playerName = vec_num[matchNum][1];
		int seconds = atoi(vec_num[matchNum][6].c_str());
		int pureMilliseconds = atoi(vec_num[matchNum][7].c_str());

		info->milliseconds = pureMilliseconds + seconds * 1000;
		info->isLogged = (qboolean)(vec_num[matchNum][4].length() > 0);
		info->isNumber1 = (qboolean)(vec_num[matchNum][3].length() > 0);
		info->isPersonalBest = lastWasPersonalBestMessage;

		return qtrue;
	}

	size_t count2 = rm2.setRegexObject(&defragRazorPersonalBestRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num2)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num2.size(); matchNum++) { // really its just going to be 1 but whatever
		razorDefragLastCheckedPrintWasPersonalBest = qtrue; // Razor defrag sends 2 print messages. First says "personal best", second says time and rank. So we gotta remember the last message saying "personal best".
	}
	return qfalse;
}

std::vector<defragRunInfoFinal_t> DARKDefragRuns;

qboolean findDARKDefragRun(std::string printText) {
	jp::VecNum vec_num, vec_num2, vec_num3;
	jp::RegexMatch rm, rm2, rm3;

	size_t count = rm.setRegexObject(&defragDarkFinishRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
		
		defragRunInfoFinal_t* info = &DARKDefragRuns.emplace_back();

		info->info.playerName = vec_num[matchNum][1];

		if (vec_num[matchNum][2] != "main") {
			info->info.courseName = vec_num[matchNum][2];
		}
		info->info.style = vec_num[matchNum][3];

		int hours = atoi(vec_num[matchNum][5].c_str());
		int minutes = atoi(vec_num[matchNum][6].c_str());
		int seconds = atoi(vec_num[matchNum][7].c_str());
		int milliseconds = atoi(vec_num[matchNum][8].c_str());
		info->info.milliseconds = milliseconds + seconds*1000 + minutes*60*1000 + hours*60*60*1000;
		/*info->playerName = vec_num[matchNum][1];
		int seconds = atoi(vec_num[matchNum][6].c_str());
		int pureMilliseconds = atoi(vec_num[matchNum][7].c_str());

		info->milliseconds = pureMilliseconds + seconds * 1000;
		info->isLogged = (qboolean)(vec_num[matchNum][4].length() > 0);
		info->isNumber1 = (qboolean)(vec_num[matchNum][3].length() > 0);
		info->isPersonalBest = lastWasPersonalBestMessage;*/

		return qtrue;
	}

	size_t count2 = rm2.setRegexObject(&defragDarkPersonalBestRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num2)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num2.size(); matchNum++) { // really its just going to be 1 but whatever
		// It was pb. Modify existing run.
		if (DARKDefragRuns.size() && DARKDefragRuns.back().info.playerName == vec_num2[matchNum][1] ) {
			DARKDefragRuns.back().info.defragPlayerName = vec_num2[matchNum][2]; // Replace player name with username.
			DARKDefragRuns.back().info.isPersonalBest = qtrue;
			DARKDefragRuns.back().info.isLogged = qtrue;
		}
		return qtrue;
	}

	size_t count3 = rm3.setRegexObject(&defragDarkRecordRegex)                          //set associated Regex object
		.setSubject(&printText)                         //set subject string
		.setNumberedSubstringVector(&vec_num3)         //pass pointer to VecNum vector
		.match();

	for (int matchNum = 0; matchNum < vec_num3.size(); matchNum++) { // really its just going to be 1 but whatever
		// It was pb. Modify existing run.
		if (DARKDefragRuns.size() && DARKDefragRuns.back().info.defragPlayerName == vec_num3[matchNum][2] ) {
			DARKDefragRuns.back().info.isNumber1 = qtrue;
		}
		return qtrue;
	}
	return qfalse;
}


void updateForcePowersInfo(clientActive_t* clCut) { // TODO: make this adapt to JKA
	int stringOffset = clCut->gameState.stringOffsets[CS_SERVERINFO];
	const char* playerInfo = clCut->gameState.stringData + stringOffset;

	int g_forcePowerDisable = atoi(Info_ValueForKey(playerInfo, sizeof(clCut->gameState.stringData) - stringOffset, "g_forcePowerDisable"));
	int g_maxForceRank = atoi(Info_ValueForKey(playerInfo, sizeof(clCut->gameState.stringData) - stringOffset, "g_maxForceRank"));

	if (g_forcePowerDisable & (1 << FP_LEVITATION))
	{
		forcePowersInfo.maxForceJumpLevel = 1; // Only Ysalamiri would be 0 I believe.
	}
	else {
		// Just see what the theoretical maximum jump level is that any player on this server could have.
		int maxPoints = forceMasteryPoints[std::clamp(g_maxForceRank,0,NUM_FORCE_MASTERY_LEVELS-1)];
		int maxReachedJumpLevel = 0;
		int availablePoints = maxPoints;
		for (int i = 0; i < NUM_FORCE_POWER_LEVELS; i++) {
			if (availablePoints >= bgForcePowerCost[FP_LEVITATION][i]) {
				maxReachedJumpLevel = i;
				availablePoints -= bgForcePowerCost[FP_LEVITATION][i];
			}
			else {
				break;
			}
		}
		forcePowersInfo.maxForceJumpLevel = maxReachedJumpLevel;
	}

	// Now calculate maximum vertical speed delta
	int curHeight = 0;
	forcePowersInfo.maxForceJumpVelocityDelta = (forceJumpHeight[forcePowersInfo.maxForceJumpLevel] - curHeight) / forceJumpHeight[forcePowersInfo.maxForceJumpLevel] * forceJumpStrength[forcePowersInfo.maxForceJumpLevel];
}

int g_gametype = 0;
int g_gametype_general = 0;
int activeKillDatabase = 0; // right now we dont have kill-specific filters, only global ones based on gametype or mapname and such. so we can optimize this and save it globally. 
std::string currentMapName = "";
void updateGameInfo(clientActive_t* clCut, demoType_t demoType, const ExtraSearchOptions& opts) { // TODO: make this adapt to JKA
	int stringOffset = clCut->gameState.stringOffsets[CS_SERVERINFO];
	const char* serverInfo = clCut->gameState.stringData + stringOffset;

	int g_weaponDisable = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "g_weaponDisable"));
	g_gametype = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "g_gametype"));
	g_gametype_general = generalizeGameValue<GMAP_GAMETYPE,SAFE>(g_gametype,demoType);
	gameIsQ3Defrag = false;
	if (demoType == DM_68) {
		const char* gamename = Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "gamename");
		if (!_stricmp(gamename, "defrag")) {
			gameIsQ3Defrag = true;
			q3DefragInfo.cheats = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "sv_cheats"));
			q3DefragInfo.promode = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "df_promode"));
			q3DefragInfo.version = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "defrag_vers"));
			q3DefragInfo.SetMapName(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "mapname"));
			q3DefragInfo.gameType = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "defrag_gametype"));
			q3DefragInfo.online = q3DefragInfo.gameType > 4;

		}
	}

	const char* theNewMapname = Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "mapname");
	if (stricmp(theNewMapname,currentMapName.c_str())) {
		// map changed. null the base flag positions.
		Com_Memset(&baseFlagPositions, 0, sizeof(baseFlagPositions));
		currentMapName = theNewMapname;
	}

	const char* hostname = Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "sv_hostname");
	if (!stricmp(hostname, "^1^7^1FAKE ^4^7^4DEMO")) {
		demoDerivativeFlags |= DERIV_FAKEDEMO;
	}

	activeKillDatabase = 0;
	if (opts.filters.size()) {
		activeKillDatabase = -1; // kinda ugly since i have to check it but meh
		const char* curMapname = Info_ValueForKey(serverInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
		bool foundMatch = false;
		for (int i = 0; i < opts.filters.size(); i++) {
			const ResultFilter* fitler = &opts.filters[i];
			switch (fitler->type) {
				case ResultFilter::Type::FILTER_ALLGOES:
					foundMatch = true;
					break;
				case ResultFilter::Type::FILTER_GAMETYPE:
					if (fitler->matchValue & (1 << g_gametype_general)) {
						foundMatch = true;
					}
					break;
				case ResultFilter::Type::FILTER_MAP:
					{
						const char* search = fitler->matchString.c_str();
						const int maplen = strlen(curMapname);
						const int searchlen = strlen(search);
						if (fitler->matchStart && fitler->matchEnd) {
							foundMatch = !_stricmp(curMapname, search);
						}
						else if (fitler->matchStart) {
							foundMatch = !_strnicmp(curMapname, search, searchlen);
						}
						else if (fitler->matchEnd) {
							if (maplen >= searchlen) {
								const char* maptest = curMapname + (maplen- searchlen);
								foundMatch = !_stricmp(maptest, search);
							}
						}
						else {
							const char* maptest = curMapname;
							while (*maptest && !foundMatch) {
								foundMatch = !_strnicmp(maptest, search, searchlen);
								maptest++;
							}
						}
					}
					break;
			}
			if (foundMatch) {
				activeKillDatabase = i;
				break;
			}
		}
	}

	// JKA:
	// Online forum says 65523 is saber only
	// CTF Haven uses 57339
	// I think difference is: The former allows saber and disallows tripmines, while the latter has those flipped. 
	// And you actually get a saber even if saber is disabled, not sure why.
	// But, neither of these two disallow WP_BRYAR_OLD or WP_EMPLACED_GUN (16 and 17). Not sure why.
	// Logically speaking maybe those should be disabled too, but my task is to analyze demos, not to complain
	// to server owners since they can't retroactively change it either, so the demos are what they are.
	// 
	// Helpful btw: http://www.bradgoodman.com/bittool/
	// Some other mentions: 524279 (all but saber is blocked), 65531 (all but saber and above 15 blocked)

	int i = 0;
	int numWeapons = specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_NUM_WEAPONS_GENERAL,demoType);
	int allowedSaberIshWeapons[] = { 
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_NONE_GENERAL, demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_SABER_GENERAL,demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_MELEE_GENERAL,demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_THERMAL_GENERAL,demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_TRIP_MINE_GENERAL,demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_DET_PACK_GENERAL,demoType),
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_BRYAR_OLD_GENERAL,demoType), // Not sure about this one but it also often ends up enabled even though it's saber-only
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_EMPLACED_GUN_GENERAL,demoType), // I think this is just a mounted gun and is typically not disallowed in the weapondisable
		specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_TURRET_GENERAL,demoType)
	};
	int allowedSaberIshWeaponsCount = sizeof(allowedSaberIshWeapons) / sizeof(allowedSaberIshWeapons[0]);
	gameIsSaberOnlyIsh = true;
	while (i < numWeapons)
	{
		if (!(g_weaponDisable & (1 << i))) // Is this particular weapon allowed?
		{
			// Check if it is one of the allowed weapons
			bool thisWeaponIsOk = false;
			for (int b = 0; b < allowedSaberIshWeaponsCount; b++) {
				if (i == allowedSaberIshWeapons[b]) {
					thisWeaponIsOk = true;
				}
			}
			if (!thisWeaponIsOk) {
				gameIsSaberOnlyIsh = false;
				break;
			}
		}

		i++;
	}

	if (sentryModelIndex == -1) {
		sentryModelIndex = G_ModelIndex_NoAdd("models/items/psgun.glm", clCut, NULL, demoType);
		if (!sentryModelIndex) sentryModelIndex = -1;
	}


	// Check if any "moving" force powers (that could result in a doom/suic) are present
	// IF they are, we must use some guesses to see if a doom/suic kill is actually a saber kill, because it might 
	// just be someone force pulling someone or such
	// Force powers luckily are same for JK2 and JKA so no specialization needed atm (but might change if we ever support some mods?)

	int g_forcePowerDisable = atoi(Info_ValueForKey(serverInfo, sizeof(clCut->gameState.stringData) - stringOffset, "g_forcePowerDisable"));

	gameAllowsDoomingForcePowers = false;
	// These are the powers that I think POTENTIALLY can result in dooms?
	if (
		!(g_forcePowerDisable & (1 << FP_PUSH)) ||
		!(g_forcePowerDisable & (1 << FP_PULL)) ||
		!(g_forcePowerDisable & (1 << FP_GRIP)) ||
		!(g_forcePowerDisable & (1 << FP_LIGHTNING)) || // I'm not sure about this one. Edit: Yeah we checked. It can doom u.
		(!(g_forcePowerDisable & (1 << FP_DRAIN)) && demoType >= DM_25 && demoType <= DM_26_XBOX)|| // In JKA drain can eat at health too i think? But not in jk2, drain cant doom u in jk2 thats for sure.
		!(g_forcePowerDisable & (1 << FP_SABERTHROW))
		) {
		gameAllowsDoomingForcePowers = true;
	}


}











#ifdef RELDEBUG
//#pragma optimize("", off)
#endif

struct eventAndParm_t {
	int event;
	int eventParm;
};
struct psEventData_t {
	eventAndParm_t externalEvent;
	eventAndParm_t predictableEvents[MAX_PS_EVENTS];
};

// Does this work at all? Idk
psEventData_t demoCutGetEvent(playerState_t* ps, playerState_t* ops, int demoCurrentTime) {
	int			i;
	int			event;
	psEventData_t psEventData{};

	//if (lastEventTime[ps->clientNum] < demoCurrentTime - EVENT_VALID_MSEC) { // Don't do this for playerstate external because its never cleared serverside
	//	lastEvent[ps->clientNum] = 0;
	//}

	//if (ps->externalEvent && ps->externalEvent != lastEvent[ps->clientNum]) {
	if (ps->externalEvent && ps->externalEvent != ops->externalEvent) {
		int eventNumberRaw = ps->externalEvent;
		int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

		lastEventTime[ps->clientNum] = demoCurrentTime;
			
		//lastEvent[ps->clientNum] = eventNumberRaw;
		psEventData.externalEvent.event = eventNumber;
		psEventData.externalEvent.eventParm = ps->externalEventParm;
	}

	int predictableIndex = 0;
	// go through the predictable events buffer
	for (i = ps->eventSequence - MAX_PS_EVENTS; i < ps->eventSequence; i++) {
		// if we have a new predictable event
		if (i >= ops->eventSequence
			// or the server told us to play another event instead of a predicted event we already issued
			// or something the server told us changed our prediction causing a different event
			|| (i > ops->eventSequence - MAX_PS_EVENTS && ps->events[i & (MAX_PS_EVENTS - 1)] != ops->events[i & (MAX_PS_EVENTS - 1)])) {

			psEventData.predictableEvents[predictableIndex].event = ps->events[i & (MAX_PS_EVENTS - 1)];
			psEventData.predictableEvents[predictableIndex++].eventParm = ps->eventParms[i & (MAX_PS_EVENTS - 1)];

		}
		if (predictableIndex == 2) break; // Just to be safe :shrug:
	}

	return psEventData;
}

// readOnly parameter: Do not write anythng or change any state. We are pre-parsing events.
int demoCutGetEvent(entityState_t* es,int demoCurrentTime, demoType_t demoType,qboolean readOnly) {
	//if (lastEvent.find(es->number) == lastEvent.end()) {
	//	lastEvent[es->number] = 0;
	//} // Not really necessary is it? That's what it will be by default?

	// check for event-only entities
	/*if (es->eType > ET_EVENTS) {
		if (cent->previousEvent) {
			return;	// already fired
		}
		// if this is a player event set the entity number of the client entity number
		if (es->eFlags & EF_PLAYER_EVENT) {
			es->number = es->otherEntityNum;
		}

		cent->previousEvent = 1;

		es->event = es->eType - ET_EVENTS;
	}
	else {
		// check for events riding with another entity
		if (es->event == cent->previousEvent) {
			return;
		}
		cent->previousEvent = es->event;
		if ((es->event & ~EV_EVENT_BITS) == 0) {
			return;
		}
	}*/

	// if ( cent->snapShotTime < cg.time - EVENT_VALID_MSEC ) {
	if (lastEventTime[es->number] < demoCurrentTime - EVENT_VALID_MSEC && !readOnly) {
		lastEvent[es->number] = 0;
	}

	int eventNumberRaw = es->eType > getET_EVENTS(demoType) ? es->eType - getET_EVENTS(demoType) : es->event;
	int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

	if (!readOnly) {
		lastEventTime[es->number] = demoCurrentTime;
	}
	if (eventNumberRaw == lastEvent[es->number]) {
		return 0;
	}

	if (!readOnly) {
		lastEvent[es->number] = eventNumberRaw;
	}
	return eventNumber;
	
}

entityState_t* findEntity(int number) {
	for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

		if (demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)].number == number) {
			return &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
		}
	}
	return NULL;
}

class NearbyPlayerInfo {
public:
	std::string nearbyPlayersString;
	std::string nearbyEnemiesString;
	int nearbyPlayersCount = 0;
	int nearbyEnemiescount = 0;
	int veryClosePlayersCount = 0;
	int verycloseEnemiescount = 0;
};

template <unsigned int max_clients>
inline NearbyPlayerInfo* getNearbyPlayersInfo(vec3_t position, int excludeNum) {
	NearbyPlayerInfo* retVal = new NearbyPlayerInfo();

	// Find nearby players.
	std::stringstream nearbyPlayersSS;
	std::vector<int> nearbyPlayers;
	std::vector<int> nearbyPlayersDistances;
	for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
		entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
		if (thisEntitySub->number >= 0 && thisEntitySub->number < max_clients && thisEntitySub->number != excludeNum) {
			float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, position);
			if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
				nearbyPlayersSS << (retVal->nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " (" << (int)nearbyPlayerDistance << ")";
				nearbyPlayers.push_back(thisEntitySub->number);
				nearbyPlayersDistances.push_back(nearbyPlayerDistance);
			}
		}
	}
	if (demo.cut.Cl.snap.ps.clientNum != excludeNum) {
		float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, position);
		if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
			nearbyPlayersSS << (retVal->nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
			nearbyPlayers.push_back(demo.cut.Cl.snap.ps.clientNum);
			nearbyPlayersDistances.push_back(nearbyPlayerDistance);
		}
	}

	retVal->nearbyPlayersString = nearbyPlayersSS.str();

	// Find nearby enemies
	std::stringstream nearbyEnemiesSS;
	//int nearbyEnemiescount = 0;
	//int veryClosePlayersCount = 0;
	//int verycloseEnemiescount = 0;
	for (int nearV = 0; nearV < nearbyPlayers.size(); nearV++) {
		int nearbyPlayerHere = nearbyPlayers[nearV];
		if (playerTeams[nearbyPlayerHere] != playerTeams[excludeNum]) {
			nearbyEnemiesSS << (retVal->nearbyEnemiescount++ == 0 ? "" : ",") << nearbyPlayerHere << " (" << (int)nearbyPlayersDistances[nearV] << ")";
			if (nearbyPlayersDistances[nearV] <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
				retVal->verycloseEnemiescount++;
			}
		}
		if (nearbyPlayersDistances[nearV] <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
			retVal->veryClosePlayersCount++;
		}
	}
	retVal->nearbyEnemiesString = nearbyEnemiesSS.str();

	return retVal;
}



template<float Speed::*thingie>
inline float getMaxSpeedThingForClientinTimeFrame(int clientNum, int fromTime, int toTime) {
	float maxThingie = -1.0f;
	//for (auto it = speeds[clientNum].begin(), end = speeds[clientNum].end(); it != end; it++) {
	//	if (it->time >= fromTime && it->time <= toTime && (*it).*thingie > maxThingie) {
	//		maxThingie = (*it).*thingie;
	//	}
	//}

	// i know its disgusting but i want this to run fast in debug mode
	Speed* data = speeds[clientNum].data();
	data += speeds[clientNum].size() -1;
	for (int64_t i = speeds[clientNum].size() - 1; i >= 0; i--, data--) {
		if (data->time < fromTime) break;
		if (data->time <= toTime && (*data).*thingie > maxThingie) {
			maxThingie = (*data).*thingie;
		}
	}
	//for (auto it = speeds[clientNum].rbegin(), end = speeds[clientNum].rend(); it != end; it++) {
	//	if (it->time >= fromTime && it->time <= toTime && (*it).*thingie > maxThingie) {
	//		maxThingie = (*it).*thingie;
	//	}
	//}
	return maxThingie;
}

float getMaxSpeedForClientinTimeFrame(int clientNum, int fromTime, int toTime) {
	//float maxSpeed = -1.0f;
	/*for (auto it = speeds[clientNum].begin(); it != speeds[clientNum].end(); it++) {
		if (it->first >= fromTime && it->first <= toTime && it->second > maxSpeed) {
			maxSpeed = it->second;
		}
	}*/
	//for (auto it = speeds[clientNum].begin(), end = speeds[clientNum].end(); it != end; it++) {
	//	if (it->time >= fromTime && it->time <= toTime && it->speed > maxSpeed) {
	//		maxSpeed = it->speed;
	//	}
	//}
	//return maxSpeed;
	return getMaxSpeedThingForClientinTimeFrame<&Speed::speed>(clientNum, fromTime, toTime);
}


float getMaxAngularSpeedForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {

	return getMaxSpeedThingForClientinTimeFrame<&Speed::angularSpeed>(clientNum, fromTime, toTime);
	//float maxSpeed = -1.0f;
	//for (int i = 0;i< speeds[clientNum].size(); i++) {
	//	if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularSpeed) > maxSpeed) {
	//		maxSpeed = abs(speeds[clientNum][i].angularSpeed);
	//	}
	//}
	//return maxSpeed;
}


float getMaxAngularAccelerationForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {

	return getMaxSpeedThingForClientinTimeFrame<&Speed::angularAcceleration>(clientNum, fromTime, toTime);
	//float maxAccel = -1.0f;
	//for (int i = 0;i< speeds[clientNum].size(); i++) {
	//	if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularAcceleration) > maxAccel) {
	//		maxAccel = abs(speeds[clientNum][i].angularAcceleration);
	//	}
	//}
	//return maxAccel;
}


float getMaxAngularJerkForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	return getMaxSpeedThingForClientinTimeFrame<&Speed::angularJerk>(clientNum, fromTime, toTime);
	//float maxJerk = -1.0f;
	//for (int i = 0;i< speeds[clientNum].size(); i++) {
	//	if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularJerk) > maxJerk) {
	//		maxJerk = abs(speeds[clientNum][i].angularJerk);
	//	}
	//}
	//return maxJerk;
}


float getMaxAngularSnapForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	return getMaxSpeedThingForClientinTimeFrame<&Speed::angularSnap>(clientNum, fromTime, toTime);
	//float maxSnap = -1.0f;
	//for (int i = 0;i< speeds[clientNum].size(); i++) {
	//	if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularSnap) > maxSnap) {
	//		maxSnap = abs(speeds[clientNum][i].angularSnap);
	//	}
	//}
	//return maxSnap;
}

const char* getPlayerName(int clientNum, int CS_PLAYERS_here, bool isMOHAADemo) {
	int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNum];
	const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
	return Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
}

template <unsigned int max_clients>
void setPlayerAndTeamData(clientActive_t* clCut, demoType_t demoType) {
	int stringOffset;
	memset(teamInfo,0,sizeof(teamInfo));
	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	for (int i = 0; i < max_clients; i++) {

		stringOffset = clCut->gameState.stringOffsets[CS_PLAYERS_here + i];
		const char* playerInfo = clCut->gameState.stringData + stringOffset;
		const char* playerTeam = Info_ValueForKey(playerInfo, sizeof(clCut->gameState.stringData) - stringOffset, "t");
		if (strlen(playerTeam)) {
			int team = atoi(playerTeam);
			playerTeams[i] = team;
			teamInfo[team].playerCount++;
		}
	}
	stringOffset = clCut->gameState.stringOffsets[CS_SCORES1];
	teamInfo[TEAM_RED].score = atoi(clCut->gameState.stringData + stringOffset);
	stringOffset = clCut->gameState.stringOffsets[CS_SCORES2];
	teamInfo[TEAM_BLUE].score = atoi(clCut->gameState.stringData + stringOffset);
}

template<unsigned int max_clients>
void CheckForNameChanges(clientActive_t* clCut,const ioHandles_t &io, demoType_t demoType,bool& wasDoingSQLiteExecution, const ExtraSearchOptions& opts) {


	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");

	static std::vector<std::string> modelsToAdd;
	modelsToAdd.clear();

	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	for (int i = 0; i < max_clients; i++) {

		stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + i];
		const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string modelName = Info_ValueForKey(victimInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "model");
		if (modelName != lastPlayerModel[i]) {
			lastPlayerModel[i] = modelName;
			if (opts.doStatsDb) {
				modelsToAdd.push_back(modelName);
			}
		}
	}

	for (int i = 0; i < modelsToAdd.size(); i++) {
		std::string* thisModel = &modelsToAdd[i];
		std::transform(thisModel->begin(), thisModel->end(), thisModel->begin(), tolowerSignSafe);
		size_t firstSlash = thisModel->find_first_of("/");

		bool variantExists = false;
		std::string baseModel = "";
		std::string variant = "";
		if (firstSlash == std::string::npos) { 
			// No variant. Just the model name
			baseModel = *thisModel;
			//baseModelCString = thisModel->c_str();
		}
		else {
			baseModel = thisModel->substr(0,firstSlash);
			//baseModelCString = thisModel->c_str();
			if (firstSlash < (thisModel->size() - 1)) {
				variantExists = true;
				variant = thisModel->substr(firstSlash + 1, thisModel->size()-firstSlash-1);
				//variantCString = variant.c_str();
			}
		}

		SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
		SQLDelayedQuery* query = &queryWrapper->query;

		SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
		//SQLBIND_DELAYED_TEXT(io.updatePlayerModelCountStatement, "@map", mapname.c_str());
		SQLBIND_DELAYED_TEXT(query, "@baseModel", baseModel.c_str());
		//SQLBIND_DELAYED_TEXT(io.updatePlayerModelCountStatement, "@baseModel", baseModel.c_str());
		if (variantExists) {
			SQLBIND_DELAYED_TEXT(query, "@variant", variant.c_str());
			//SQLBIND_DELAYED_TEXT(io.updatePlayerModelCountStatement, "@variant", variant.c_str());
		}
		else {
			SQLBIND_DELAYED_TEXT(query, "@variant", ""); // I wanted this to be NULL, I really did. But turns out, SQLite lets you have multiple NULL values without triggering a unique constraint. Cringe. So we just go with empty space.
			//SQLBIND_DELAYED_NULL(query, "@variant");
			//SQLBIND_DELAYED_NULL(io.updatePlayerModelCountStatement, "@variant");
		}

		io.playerModelQueries->push_back(queryWrapper);

	}

}

#define HASH_BITS 224
inline std::string makeStringHash(std::string input) {
	constexpr int bytes = picosha3::bits_to_bytes(HASH_BITS);
	auto sha3_512 = picosha3::get_sha3_generator<HASH_BITS>();
	//return sha3_512.get_hex_string(input);
	std::array<uint8_t, bytes> hash{};
	sha3_512.process(input.cbegin(),input.cend());
	sha3_512.finish();
	sha3_512.get_hash_bytes(hash.begin(), hash.end());
	std::string output;
	ankerl::base58::encode(hash.data(),bytes, output);
	return output.substr(std::min((size_t)22,output.size())); // 22 bytes of base58 should be around 128 bits?
}


template<unsigned int max_clients>
void CheckSaveKillstreak(int maxDelay,SpreeInfo* spreeInfo,int clientNumAttacker, std::vector<Kill>* killsOfThisSpree,std::vector<int>* victims,std::vector<std::string>* killHashes,std::string allKillsHashString, int64_t demoCurrentTime, const ioHandles_t& io, int bufferTime,int64_t lastGameStateChangeInDemoTime, const char* sourceDemoFile,std::string oldBasename,std::string oldPath,time_t oldDemoDateModified, demoType_t demoType, const ExtraSearchOptions& opts, const highlightSearchMode_t searchMode,bool& wasDoingSQLiteExecution) {
	
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if ((searchMode == SEARCH_MY_CTF_RETURNS || searchMode == SEARCH_ALL_MY_KILLS) && spreeInfo->countThirdPersons == spreeInfo->countKills) return;

	if (spreeInfo->countKills >= KILLSTREAK_MIN_KILLS || spreeInfo->countRets >= 2) { // gotta be at least 3 kills or 2 rets to count as killstreak

		int uniqueTargetCount = 0;

		int countSaberKills = 0;
		int maxDelayActual = 0;
		int CS_PLAYERS_here = getCS_PLAYERS(demoType);
		int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
		const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
		std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");
		std::string playername = "WEIRDATTACKER";

		if (clientNumAttacker >= 0 && clientNumAttacker < max_clients) {

			stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNumAttacker];
			const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo? "name": "n");
		}
		//playerInfo = demo.cut.Cl.gameState.stringData + demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + target];
		//std::string victimname = Info_ValueForKey(playerInfo, "n");

		std::stringstream victimsSS;
		std::stringstream victimsStrippedSS;
		std::stringstream victimsNumsSS;
		/*for (int i = 0; i < victims->size(); i++) {
			stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + (*victims)[i]];
			const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			victimsSS << (*victims)[i] << ": " << Info_ValueForKey(victimInfo, sizeof(demo.cut.Cl.gameState.stringData)-stringOffset, "n") << " ("<< (*killsOfThisSpree)[i].modInfoString<<")" << "\n";
			victimsNumsSS << (i==0? "" :",") << (*victims)[i];
		}*/
		int lastKillTime = 0;
		std::set<std::string> killTypes;
		std::set<int> nearbyPlayers;
		std::set<int> uniqueTargets;
		for (int i = 0; i < killsOfThisSpree->size(); i++) {
			if ((*killsOfThisSpree)[i].isSaberKill) {
				countSaberKills++;
			}
			//stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + (*victims)[i]];
			//const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			int timeSinceLastKill = (*killsOfThisSpree)[i].time - lastKillTime;
			victimsSS << (*killsOfThisSpree)[i].targetClientNum/*<< (*victims)[i]*/ << ": " << (*killsOfThisSpree)[i].victimName << " ("<< (*killsOfThisSpree)[i].modInfoString<<", +"<< (i>0? timeSinceLastKill :0)<<")" << "\n";
			victimsStrippedSS << (*killsOfThisSpree)[i].targetClientNum/*<< (*victims)[i]*/ << ": " << Q_StripColorAll((*killsOfThisSpree)[i].victimName) << " ("<< (*killsOfThisSpree)[i].modInfoString<<", +"<< (i>0? timeSinceLastKill :0)<<")" << "\n";

			uniqueTargets.insert((*killsOfThisSpree)[i].targetClientNum);

			killTypes.insert((*killsOfThisSpree)[i].modInfoString);
			//killTypesSS << (*killsOfThisSpree)[i].modInfoString << "\n";
			if (i > 0) {
				maxDelayActual = std::max(maxDelayActual, timeSinceLastKill);
			}
			lastKillTime = (*killsOfThisSpree)[i].time;
			victimsNumsSS << (i==0? "" :",") << (*victims)[i];
			for (int n = 0; n < (*killsOfThisSpree)[i].nearbyPlayers.size(); n++) {
				nearbyPlayers.insert((*killsOfThisSpree)[i].nearbyPlayers[n].clientNum);
			}
		}

		uniqueTargetCount = uniqueTargets.size();

		if (countSaberKills < opts.onlyLogKillSpreesWithSaberKills) {
			return;
		}

		std::string victimsString = victimsSS.str();
		std::string victimsStringStripped = victimsStrippedSS.str();
		std::string victimsNumsString = victimsNumsSS.str();

		// Remove any nearby players that are victims or that are the attacker. (they dont count as nearby players)
		for (int i = 0; i < killsOfThisSpree->size(); i++) {
			nearbyPlayers.erase((*killsOfThisSpree)[i].targetClientNum);
		}
		nearbyPlayers.erase(clientNumAttacker);


		std::stringstream nearbyPlayersSS;
		int nearbyPlayerscount = 0;
		for (auto it = nearbyPlayers.begin(); it != nearbyPlayers.end(); it++) {
			nearbyPlayersSS << (nearbyPlayerscount++ == 0 ? "" : ",") << *it;
		}
		std::string nearbyPlayersString = nearbyPlayersSS.str();


		std::stringstream killTypesSS;
		int killTypeCount = 0;
		for (auto it = killTypes.begin(); it != killTypes.end(); it++) {
			killTypesSS << (killTypeCount++ == 0 ? "" : ",") << *it;
		}
		std::string killTypesString = killTypesSS.str();


		std::stringstream killHashesSS;
		for (int i = 0; i < killHashes->size(); i++) {
			killHashesSS << (i == 0 ? "" : "\n") << (*killHashes)[i];
		}
		std::string killHashesString = killHashesSS.str();

		//hashss << playername << "_" << victimname << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
		//auto sha3_512 = picosha3::get_sha3_generator<224>();
		//sha3_512.get_hash_bytes()
		//std::array<uint8_t, picosha3::bits_to_bytes(256)> hash{};
		//std::string hash_hex_string = sha3_512.get_hex_string(allKillsHashString);
		std::string hash_hex_string = makeStringHash(allKillsHashString);
		std::string shorthash = hash_hex_string.substr(0, std::min((size_t)3,hash_hex_string.size()));


		int maxSpeedAttacker = getMaxSpeedForClientinTimeFrame(clientNumAttacker, spreeInfo->lastKillTime - spreeInfo->totalTime - 1000, spreeInfo->lastKillTime);
		int maxSpeedVictims = spreeInfo->maxVictimSpeed;

		// Log the killspree.
		SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
		SQLDelayedQuery* query = &queryWrapper->query;

		SQLBIND_DELAYED_TEXT(query, "@hash", hash_hex_string.c_str());
		SQLBIND_DELAYED_TEXT(query, "@shorthash", shorthash.c_str());
		SQLBIND_DELAYED(query, int, "@maxDelay", maxDelay);
		SQLBIND_DELAYED(query, int, "@maxDelayActual", maxDelayActual);
		SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
		SQLBIND_DELAYED_TEXT(query, "@serverName", serverName.c_str());
		std::string serverNameStripped = Q_StripColorAll(serverName);
		SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@killerName", playername.c_str(), QF_PLAYERNAME0);
		std::string playernameStripped = Q_StripColorAll(playername);
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@killerNameStripped", playernameStripped.c_str(), QF_PLAYERNAME0STRIPPED);
		SQLBIND_DELAYED_TEXT(query, "@victimNames", victimsString.c_str());
		SQLBIND_DELAYED_TEXT(query, "@victimNamesStripped", victimsStringStripped.c_str());
		SQLBIND_DELAYED_TEXT(query, "@killTypes", killTypesString.c_str());
		SQLBIND_DELAYED(query, int, "@killTypesCount", killTypeCount);
		SQLBIND_DELAYED_TEXT(query, "@killHashes", killHashesString.c_str());
		SQLBIND_DELAYED(query, int, "@killerClientNum", clientNumAttacker);
		SQLBIND_DELAYED_TEXT(query, "@victimClientNums", victimsNumsString.c_str());
		SQLBIND_DELAYED(query, int, "@countKills", spreeInfo->countKills);
		SQLBIND_DELAYED(query, int, "@countRets", spreeInfo->countRets);
		SQLBIND_DELAYED(query, int, "@countTeamKills", spreeInfo->countTeamKills);
		SQLBIND_DELAYED(query, int, "@countUniqueTargets", uniqueTargetCount);
		SQLBIND_DELAYED(query, int, "@countDooms", spreeInfo->countDooms);
		SQLBIND_DELAYED(query, int, "@countExplosions", spreeInfo->countExplosions);
		SQLBIND_DELAYED(query, int, "@countThirdPersons", spreeInfo->countThirdPersons);
		SQLBIND_DELAYED(query, int, "@countInvisibles", spreeInfo->countInvisibles);

		SQLBIND_DELAYED_TEXT(query, "@nearbyPlayers", nearbyPlayers.size() > 0 ? nearbyPlayersString.c_str() : NULL);
		SQLBIND_DELAYED(query, int, "@nearbyPlayerCount", nearbyPlayers.size());

		SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
		SQLBIND_DELAYED(query, int, "@maxSpeedAttacker", maxSpeedAttacker);
		SQLBIND_DELAYED(query, int, "@maxSpeedTargets", spreeInfo->maxVictimSpeed);
		SQLBIND_DELAYED_TEXT(query, "@demoName", oldBasename.c_str());
		SQLBIND_DELAYED_TEXT(query, "@demoPath", oldPath.c_str());
		SQLBIND_DELAYED(query, int, "@demoTime", spreeInfo->lastKillTime/* - spreeInfo->totalTime*/); // Keep this consistent with the metaevents which are always in relation to the end
		SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime); 
		SQLBIND_DELAYED(query, int, "@duration", spreeInfo->totalTime);
		SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);
		SQLBIND_DELAYED(query, int, "@demoDateTime", oldDemoDateModified);
		SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);

		if (activeKillDatabase != -1) {
			queryWrapper->databaseIndex = activeKillDatabase;
			io.spreeQueries->push_back(queryWrapper);
		}
		else {
			delete queryWrapper;
			return;
		}

		if (clientNumAttacker >= 0 && clientNumAttacker < max_clients) {
			MetaEventTracker* killSpreeME = new MetaEventTracker(spreeInfo->lastKillTime, queryWrapper, metaEventTrackers[METRACKER_KILLSPREES][clientNumAttacker], bufferTime, spreeInfo->totalTime, resultingMetaEventTracking[METRACKER_KILLSPREES]);
			bool wasFollowedThroughBufferTime = playerFirstFollowed[clientNumAttacker] != -1 && playerFirstFollowed[clientNumAttacker] < (demo.cut.Cl.snap.serverTime - spreeInfo->totalTime - bufferTime);
			killSpreeME->reframeClientNum = clientNumAttacker;
			killSpreeME->needsReframe = opts.reframeIfNeeded && !wasFollowedThroughBufferTime;
			killSpreeME->addPastEvents(playerPastMetaEvents[clientNumAttacker], getMinimumMetaEventBufferTime(clientNumAttacker, bufferTime, demoCurrentTime));
			metaEventTrackers[METRACKER_KILLSPREES][clientNumAttacker] = killSpreeME;
		}


		int64_t startTime = spreeInfo->lastKillTime-spreeInfo->totalTime - bufferTime;
		int64_t endTime = spreeInfo->lastKillTime + bufferTime;
		int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
		bool isTruncated = false;
		int truncationOffset = 0;
		if (earliestPossibleStart > startTime) {
			truncationOffset = earliestPossibleStart - startTime;
			startTime = earliestPossibleStart;
			isTruncated = true;
		}



		std::stringstream ss; // First half of target filename
		std::stringstream ss2; // Second half of target filename (so we can insert more modifiers that we don't know about yet)
		ss << mapname << std::setfill('0') << "___KILLSPREE" << maxDelay << "_" << spreeInfo->countKills << (spreeInfo->countRets ? va("R%d", spreeInfo->countRets) : "") << (spreeInfo->countDooms ? va("D%d", spreeInfo->countDooms) : "") << (spreeInfo->countExplosions ? va("E%d", spreeInfo->countExplosions) : "") << (spreeInfo->countTeamKills ? va("T%d", spreeInfo->countTeamKills) : "") << "_U" << uniqueTargetCount;
		ss2 << "___" << playername << "__";
		for (int i = 0; i < victims->size(); i++) {
			ss2 << "_" << (*victims)[i];
		}
		ss2 << "___" << maxSpeedAttacker <<"_"<< maxSpeedVictims << "ups" << (spreeInfo->countThirdPersons ? va("___thirdperson%d", spreeInfo->countThirdPersons) : "") << (spreeInfo->countInvisibles ? va("___invisibles%d", spreeInfo->countInvisibles) : "") << "___" << clientNumAttacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "") << "_" << shorthash;

		std::string targetFilename = ss.str();
		std::string targetFilename2 = ss2.str();
		char* targetFilenameFiltered = new char[targetFilename.length() + 1];
		char* targetFilename2Filtered = new char[targetFilename2.length() + 1];
		sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);
		sanitizeFilename(targetFilename2.c_str(), targetFilename2Filtered);

		std::stringstream batSS;
		std::stringstream batSS2;
		batSS << "\nrem hash: " << hash_hex_string;
		batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
		batSS << "\n" << (((spreeInfo->countKills-std::max(spreeInfo->countTeamKills,spreeInfo->countInvisibles)) >= KILLSTREAK_MIN_KILLS) ? "" : "rem ") << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered;
		batSS2 << targetFilename2Filtered << "\" " << startTime << " " << endTime;
		batSS2 << (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");

		queryWrapper->batchString1 = batSS.str();
		queryWrapper->batchString2 = batSS2.str();

		delete[] targetFilenameFiltered;
		delete[] targetFilename2Filtered;
		//std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "\n";
		if(!opts.noFindOutput) std::cout << ss.str() << ss2.str() << "\n";

		//timeCheckedForKillStreaks[clientNumAttacker] = spreeInfo->lastKillTime;
	}

}

template<unsigned int max_clients>
void GetRunMeta(defragRunInfo_t& runInfo, defragRunMeta_t& meta, int64_t demoCurrentTime, int64_t lastGameStateChangeInDemoTime, demoType_t demoType, int CS_PLAYERS_here) {

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	meta.mapName = std::string(Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname"));
	if (runInfo.courseName != "") {
		std::stringstream ssMapName;
		ssMapName << meta.mapName << "(" << runInfo.courseName << ")";
		meta.mapName = ssMapName.str();
	}
	
	meta.serverName = std::string(Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname"));


	meta.playerName = runInfo.playerName;
	if (runInfo.defragPlayerName.size()) {
		meta.playerName = runInfo.defragPlayerName;
	}

	// Find player
	meta.playerNum = -1;
	if (runInfo.knownClientNum != -1) {
		meta.playerNum = runInfo.knownClientNum;
	}
	else {
		for (int clientNum = 0; clientNum < max_clients; clientNum++) {

			int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNum];
			const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			std::string playerNameCompare = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");
			if (playerNameCompare == meta.playerName) {
				meta.playerNum = clientNum;
			}
		}
	}

	if (meta.playerNum != -1) {
		if (playerFirstFollowed[meta.playerNum] != -1 && playerFirstFollowed[meta.playerNum] < (demo.cut.Cl.snap.serverTime - runInfo.milliseconds)) {
			meta.wasFollowed = true; // determine outside (because delayed PB/WR info)
		}
		if (playerFirstVisible[meta.playerNum] != -1 && playerFirstVisible[meta.playerNum] < (demo.cut.Cl.snap.serverTime - runInfo.milliseconds)) {
			meta.wasVisible = true; // determine outside (because delayed PB/WR info)
		}
		if (playerFirstFollowedOrVisible[meta.playerNum] != -1 && playerFirstFollowedOrVisible[meta.playerNum] < (demo.cut.Cl.snap.serverTime - runInfo.milliseconds)) {
			meta.wasVisibleOrFollowed = true; // determine outside (because delayed PB/WR info)
		}
	}


	meta.demoTime = demoCurrentTime;
	meta.serverTime = demo.cut.Cl.snap.serverTime;
	meta.lastGameStateChangeInDemoTime = lastGameStateChangeInDemoTime;


	// Do we have strafe deviation info?
	int64_t runStart = meta.demoTime - runInfo.milliseconds;  // determine outside (because delayed PB/WR info)
	int64_t measurementStartTimeOffset = (meta.playerNum != -1) ? abs(strafeDeviationsDefrag[meta.playerNum].lastReset - runStart) : 0;
	meta.averageStrafeDeviationSet = false;
	if (meta.wasVisibleOrFollowed && meta.playerNum != -1 && measurementStartTimeOffset < DEFRAG_STRAFEDEVIATION_SAMPLE_START_TIME_MAX_OFFSET) {
		meta.averageStrafeDeviation = strafeDeviationsDefrag[meta.playerNum].averageHelper.sum / strafeDeviationsDefrag[meta.playerNum].averageHelper.divisor;
		meta.averageStrafeDeviationSet = true;
	}

	meta.demoRecorderClientNum = demo.cut.Clc.clientNum;

	meta.isSet = true;
}

template<unsigned int max_clients>
qboolean SaveDefragRun(const defragRunInfoFinal_t& runInfoFinal,const sharedVariables_t& sharedVars, const char* sourceDemoFile, const ioHandles_t& io, int bufferTime,  const ExtraSearchOptions& opts, const highlightSearchMode_t searchMode,bool& wasDoingSQLiteExecution) {
	
	if (!runInfoFinal.meta.isSet) {
		throw std::exception("SaveDefragRun called with defragRunInfoFinal_t that has uninited run meta.");
	}

	const defragRunMeta_t* meta = &runInfoFinal.meta;
	const defragRunInfo_t* runInfo = &runInfoFinal.info;


	bool isLogged = runInfo->isLogged;
	bool isNumberOne = runInfo->isNumber1;
	bool isPersonalBest = runInfo->isPersonalBest;


	int pureMilliseconds = runInfo->milliseconds % 1000;
	int tmpSeconds = runInfo->milliseconds / 1000;
	int pureSeconds = tmpSeconds % 60;
	int minutes = tmpSeconds / 60;

	int64_t runStart = meta->demoTime - runInfo->milliseconds;  // determine outside (because delayed PB/WR info)

	std::stringstream formattedTime;
	formattedTime << std::setfill('0') << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds;
	std::string formattedTimeString = formattedTime.str();

	SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
	SQLDelayedQuery* query = &queryWrapper->query;

	SQLBIND_DELAYED_TEXT(query, "@map", meta->mapName.c_str());
	SQLBIND_DELAYED_TEXT(query, "@serverName", meta->serverName.c_str());
	std::string serverNameStripped = Q_StripColorAll(meta->serverName);
	SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
	SQLBIND_DELAYED_TEXT(query, "@readableTime", formattedTimeString.c_str());
	SQLBIND_DELAYED(query, int, "@totalMilliseconds", runInfo->milliseconds);
	SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerName", meta->playerName.c_str(), QF_PLAYERNAME0);
	if (runInfo->style != "") {
		SQLBIND_DELAYED_TEXT(query, "@style", runInfo->style.c_str());
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@style");
	}
	std::string playernameStripped = Q_StripColorAll(meta->playerName);
	SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerNameStripped", playernameStripped.c_str(), QF_PLAYERNAME0STRIPPED);
	SQLBIND_DELAYED(query, int, "@isTop10", runInfo->isLogged);
	SQLBIND_DELAYED(query, int, "@isNumber1", runInfo->isNumber1);
	SQLBIND_DELAYED(query, int, "@isPersonalBest", runInfo->isPersonalBest);

	if (runInfo->teleports || runInfo->checkpoints || runInfo->isProRun) {
		SQLBIND_DELAYED(query, int, "@runTeleProRun", runInfo->isProRun);
		SQLBIND_DELAYED(query, int, "@runTeleTeleports", runInfo->teleports);
		SQLBIND_DELAYED(query, int, "@runTeleCheckpoints", runInfo->checkpoints);
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@runTeleProRun");
		SQLBIND_DELAYED_NULL(query, "@runTeleTeleports");
		SQLBIND_DELAYED_NULL(query, "@runTeleCheckpoints");
	}
	SQLBIND_DELAYED_TEXT(query, "@demoName", sharedVars.oldBasename.c_str());
	SQLBIND_DELAYED_TEXT(query, "@demoPath", sharedVars.oldPath.c_str());
	SQLBIND_DELAYED(query, int, "@demoTime", meta->demoTime);  // determine outside (because delayed PB/WR info)
	SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", meta->lastGameStateChangeInDemoTime); // determine outside (because delayed PB/WR info)
	SQLBIND_DELAYED(query, int, "@serverTime", meta->serverTime);  // determine outside (because delayed PB/WR info)
	SQLBIND_DELAYED(query, int, "@demoDateTime", sharedVars.oldDemoDateModified);
	SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);
	SQLBIND_DELAYED(query, int, "@wasVisible", meta->wasVisible); // determine outside (because delayed PB/WR info)
	SQLBIND_DELAYED(query, int, "@wasFollowed", meta->wasFollowed); // determine outside (because delayed PB/WR info)
	SQLBIND_DELAYED(query, int, "@wasFollowedOrVisible", meta->wasVisibleOrFollowed); // determine outside (because delayed PB/WR info)

	// Do we have strafe deviation info?
	if (meta->averageStrafeDeviationSet) {
		SQLBIND_DELAYED(query, double, "@averageStrafeDeviation", meta->averageStrafeDeviation);
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@averageStrafeDeviation");
	}

	SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", meta->demoRecorderClientNum);
	SQLBIND_DELAYED(query, int, "@runnerClientNum", meta->playerNum);

	io.defragQueries->push_back(queryWrapper);

	//if (searchMode != SEARCH_INTERESTING && searchMode != SEARCH_ALL && searchMode != SEARCH_TOP10_DEFRAG) continue;
	//if (!isNumberOne && (searchMode != SEARCH_TOP10_DEFRAG || !isLogged)) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.
	if (!runInfo->isNumber1 && (/*searchMode != SEARCH_TOP10_DEFRAG || */!runInfo->isLogged) && searchMode != SEARCH_ALL_DEFRAG) return qfalse; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.


	int64_t startTime = runStart - bufferTime;
	int64_t endTime = meta->demoTime + bufferTime;
	int64_t earliestPossibleStart = meta->lastGameStateChangeInDemoTime + 1;
	bool isTruncated = false;
	int truncationOffset = 0;
	if (earliestPossibleStart > startTime) {
		truncationOffset = earliestPossibleStart - startTime;
		startTime = earliestPossibleStart;
		isTruncated = true;
	}
	//startTime = std::max(lastGameStateChangeInDemoTime+1, startTime); // We can't start before 0 or before the last gamestate change. +1 to be safe, not sure if necessary.


	std::stringstream ss;
	ss << meta->mapName << (runInfo->style != "" ? va("___%s", runInfo->style.c_str()) : "") << std::setfill('0') << "___" << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << (runInfo->isProRun ? "_RTPRO" : "") << ((runInfo->teleports || runInfo->checkpoints) ? va("_RT%dT%dC", runInfo->teleports, runInfo->checkpoints) : "") << "___" << meta->playerName << (!isNumberOne && isLogged ? "___top10" : "") << (isLogged ? "" : (isNumberOne ? "___unloggedWR" : "___unlogged")) << (meta->wasFollowed ? "" : (meta->wasVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" << meta->playerNum << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

	std::string targetFilename = ss.str();
	char* targetFilenameFiltered = new char[targetFilename.length() + 1];
	sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

	std::stringstream batSS;
	batSS << "\nrem demoCurrentTime: " << meta->demoTime;
	batSS << "\n" << (meta->wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
	queryWrapper->batchString1 = batSS.str();
	queryWrapper->batchString2 = (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");

	delete[] targetFilenameFiltered;
	//std::cout << mapname << " " << playerNumber << " " << playername << " " << minutes << ":" << secondString << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << wasFollowed << " visible:" << wasVisible << " visibleOrFollowed:" << wasVisibleOrFollowed << "\n";
	if (!opts.noFindOutput)  std::cout << meta->mapName << " " << meta->playerNum << " " << meta->playerName << " " << minutes << ":" << std::setfill('0') << std::setw(2) << pureSeconds << ":" << std::setw(3) << pureMilliseconds << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << meta->wasFollowed << " visible:" << meta->wasVisible << " visibleOrFollowed:" << meta->wasVisibleOrFollowed << (runInfo->isProRun ? " RUNTELE PRO" : "") << ((runInfo->teleports || runInfo->checkpoints) ? va(" RUNTELE %dT%dC", runInfo->teleports, runInfo->checkpoints) : "") << "\n";

	return qtrue;
}

void checkSaveLaughs(int64_t demoCurrentTime, int bufferTime, int64_t lastGameStateChangeInDemoTime, const ioHandles_t& io, std::string* oldBasename, std::string* oldPath,int oldDemoDateModified, const char* sourceDemoFile,  qboolean force,bool& wasDoingSQLiteExecution, const ExtraSearchOptions& opts) {
	
	if (firstLaugh != -1  && (demoCurrentTime - firstLaugh > MAX_LAUGH_DELAY || force) && laughCount > 0) {
		
		if (laughCount > 1) { // Let's not bloat the database with single laughs. Could miss some stuff but oh well.

			int duration = lastLaugh - firstLaugh;

			SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
			SQLDelayedQuery* query = &queryWrapper->query;

			// Aye, let's log it.
			int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
			const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
			std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
			std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");
			SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
			SQLBIND_DELAYED_TEXT(query, "@serverName", serverName.c_str());
			std::string serverNameStripped = Q_StripColorAll(serverName);
			SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
			std::string laughsString = laughs.str();
			std::string laughsChatlogString = laughsChatlog.str();
			SQLBIND_DELAYED_TEXT(query, "@laughs", laughsString.c_str());
			SQLBIND_DELAYED_TEXT(query, "@chatlog", laughsChatlogString.c_str());
			std::string laughsChatlogStringStripped = Q_StripColorAll(laughsChatlogString);
			SQLBIND_DELAYED_TEXT(query, "@chatlogStripped", laughsChatlogStringStripped.c_str());
			SQLBIND_DELAYED(query, int, "@laughCount", laughCount);
			SQLBIND_DELAYED_TEXT(query, "@demoName", (*oldBasename).c_str());
			SQLBIND_DELAYED_TEXT(query, "@demoPath", (*oldPath).c_str());
			SQLBIND_DELAYED(query, int, "@duration", duration);
			SQLBIND_DELAYED(query, int, "@demoTime", lastLaugh /*firstLaugh*/); // Make this consistent with the rest. Always log the end time here.
			SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime);
			SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);
			SQLBIND_DELAYED(query, int, "@demoDateTime", oldDemoDateModified);
			SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);
			SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);

			if (activeKillDatabase != -1) {
				queryWrapper->databaseIndex = activeKillDatabase;
				io.laughQueries->push_back(queryWrapper);
			}
			else {
				delete queryWrapper;
				return;
			}

			int64_t startTime = firstLaugh - LAUGHS_CUT_PRE_TIME - bufferTime;
			int64_t endTime = lastLaugh + bufferTime;
			int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
			bool isTruncated = false;
			int truncationOffset = 0;
			if (earliestPossibleStart > startTime) {
				truncationOffset = earliestPossibleStart - startTime;
				startTime = earliestPossibleStart;
				isTruncated = true;
			}
			

			std::stringstream ss;
			ss << mapname << std::setfill('0') << "___LAUGHS" << laughCount << "_" << duration << "_" << laughsString.substr(0,70) << (laughsString.size() > 70 ? "--" : "") << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

			std::string targetFilename = ss.str();
			char* targetFilenameFiltered = new char[targetFilename.length() + 1];
			sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

			std::stringstream batSS;
			batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
			batSS << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
			queryWrapper->batchString1 = batSS.str();
			queryWrapper->batchString2 = (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");

			delete[] targetFilenameFiltered;
			if (!opts.noFindOutput)  std::cout << targetFilename << "\n";
		}

		resetLaughs(); // Reset again so we can track future laugh sequences.
	}
}

// TODO Optimize this a bit more with the random usage of std::string/const char etc?
void logSpecialThing(const char* specialType, const std::string details, const std::string comment, const std::string playerNameIfExists, const int reframeClientNum, const int altClientNum, const int64_t demoCurrentTime, const int duration, const int bufferTime, const int64_t lastGameStateChangeInDemoTime, const ioHandles_t& io, std::string* oldBasename, std::string* oldPath, const int oldDemoDateModified, const char* sourceDemoFile, const qboolean force,bool& wasDoingSQLiteExecution, const ExtraSearchOptions& opts, const highlightSearchMode_t searchMode,const demoType_t demoType) {
	
	SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
	SQLDelayedQuery* query = &queryWrapper->query;

	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
	std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");

	 // TODO actually log it
	// Aye, let's log it.
	SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
	SQLBIND_DELAYED_TEXT(query, "@serverName", serverName.c_str());
	std::string serverNameStripped = Q_StripColorAll(serverName);
	SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
	SQLBIND_DELAYED_TEXT(query, "@type", specialType);
	if (reframeClientNum != -1) {
		SQLBIND_DELAYED_TEXT(query, "@clientNum", reframeClientNum);
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@clientNum");
	}
	if (altClientNum != -1 && altClientNum != reframeClientNum) {
		SQLBIND_DELAYED_TEXT(query, "@clientNumAlt", altClientNum);
		const char* playerNameAlt = getPlayerName(altClientNum,getCS_PLAYERS(demoType),demoTypeIsMOHAA(demoType));
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerNameAlt", playerNameAlt, QF_PLAYERNAME1);
		std::string playerNameAltStripped = Q_StripColorAll(playerNameAlt);
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerNameAltStripped", playerNameAltStripped.c_str(), QF_PLAYERNAME1STRIPPED);
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@clientNumAlt");
		SQLBIND_DELAYED_NULL_FLAGS(query, "@playerNameAlt",QF_PLAYERNAME1);
		SQLBIND_DELAYED_NULL_FLAGS(query, "@playerNameAltStripped", QF_PLAYERNAME1STRIPPED);
	}
	if (playerNameIfExists.size()) {
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerName", playerNameIfExists.c_str(),QF_PLAYERNAME0);
		std::string playerNameIfExistsStripped = Q_StripColorAll(playerNameIfExists);
		SQLBIND_DELAYED_TEXT_FLAGS(query, "@playerNameStripped", playerNameIfExistsStripped.c_str(), QF_PLAYERNAME0STRIPPED);
	}
	else {
		SQLBIND_DELAYED_NULL_FLAGS(query, "@playerName", QF_PLAYERNAME0);
		SQLBIND_DELAYED_NULL_FLAGS(query, "@playerNameStripped", QF_PLAYERNAME0STRIPPED);
	}
	if (details.size()) {
		SQLBIND_DELAYED_TEXT(query, "@details", details.c_str());
		std::string detailsStripped = Q_StripColorAll(details);
		SQLBIND_DELAYED_TEXT(query, "@detailsStripped", detailsStripped.c_str());
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@details");
		SQLBIND_DELAYED_NULL(query, "@detailsStripped");
	}
	if (comment.size()) {
		SQLBIND_DELAYED_TEXT(query, "@comment", comment.c_str());
		std::string commentStripped = Q_StripColorAll(comment);
		SQLBIND_DELAYED_TEXT(query, "@commentStripped", commentStripped.c_str());
	}
	else {
		SQLBIND_DELAYED_NULL(query, "@comment");
		SQLBIND_DELAYED_NULL(query, "@commentStripped");
	}
	SQLBIND_DELAYED_TEXT(query, "@demoName", (*oldBasename).c_str());
	SQLBIND_DELAYED_TEXT(query, "@demoPath", (*oldPath).c_str());
	SQLBIND_DELAYED(query, int, "@duration", duration);
	SQLBIND_DELAYED(query, int, "@demoTime", demoCurrentTime); // Make this consistent with the rest. Always log the end time here.
	SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime);
	SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);
	SQLBIND_DELAYED(query, int, "@demoDateTime", oldDemoDateModified);
	SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);
	SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum); 

	if (activeKillDatabase != -1) {
		queryWrapper->databaseIndex = activeKillDatabase;
		io.specialQueries->push_back(queryWrapper);
	}
	else {
		delete queryWrapper;
		return;
	}

	if (!demo.cut.Cl.snap.ps.clientNum != reframeClientNum && (searchMode == SEARCH_MY_CTF_RETURNS || searchMode == SEARCH_ALL_MY_KILLS)) return; // We are searching for our own kills.

	int64_t eventStart = demoCurrentTime - duration;
	int64_t startTime = eventStart - bufferTime;
	int64_t endTime = demoCurrentTime + bufferTime;
	int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
	bool isTruncated = false;
	int truncationOffset = 0;
	if (earliestPossibleStart > startTime) {
		truncationOffset = earliestPossibleStart - startTime;
		startTime = earliestPossibleStart;
		isTruncated = true;
	}
	
	// todo log preceding kills/rets/caps/whatever via special eventtracker?

	std::stringstream ss;
	std::string detailsString = details;
	ss << mapname << std::setfill('0') << "___SPECIAL___" << specialType << "___" << playerNameIfExists << (playerNameIfExists.size() ? "___" : "") << detailsString.substr(0, 70) << (detailsString.size() > 70 ? "--" : "")<< (reframeClientNum != -1 ? va("_%d", reframeClientNum) : "") << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

	std::string targetFilename = ss.str();
	char* targetFilenameFiltered = new char[targetFilename.length() + 1];
	sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

	std::stringstream batSS;
	batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
	if (comment.size()) {
		batSS << "\nrem comment: " << comment;
	}
	batSS << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
	if (reframeClientNum != -1 && opts.reframeIfNeeded) {
		batSS << " --reframe " << reframeClientNum; // dont differently than with kills aand killsprees cuz we dont do meta event tracking here. idk, whatever. TODO? 
	}
	queryWrapper->batchString1 = batSS.str();
	queryWrapper->batchString2 = (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");

	delete[] targetFilenameFiltered;
	if (!opts.noFindOutput)  std::cout << targetFilename << "\n";
}


void openAndSetupDb(ioHandles_t& io, const ExtraSearchOptions& opts) {

	char* preparedStatementText;
	int sqlResult = 0;
	int readonlyResult = 0;
	for (int i = 0; i < opts.killDbsCount; i++) {
		const char* killDbName = opts.filters.size() ? va("killDatabase%d.db",i) : "killDatabase.db"; // todo dont create databases we end up not needing
		while ((sqlResult = sqlite3_open(killDbName, &io.killDb[i].killDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.killDb[i].killDb, "main"))) {
			std::cerr << DPrintFLocation << ":" << "error opening "<< killDbName << " for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.killDb[i].killDb) << ". Trying again in 1000ms." << "\n";
			sqlite3_close(io.killDb[i].killDb);
			io.killDb[i].killDb = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE kills ("
			"hash	TEXT,"
			"shorthash	TEXT,"
			"map	TEXT NOT NULL,"
			"killerId	INTEGER NOT NULL," // player table
			"victimId	INTEGER NOT NULL," // player table
			//"killerName	TEXT NOT NULL,"
			//"killerNameStripped	TEXT NOT NULL,"
			//"victimName	TEXT NOT NULL,"
			//"victimNameStripped	TEXT NOT NULL,"
			"killerTeam	INTEGER,"
			"victimTeam	INTEGER NOT NULL,"
			"redScore INTEGER,"
			"blueScore INTEGER,"
			"otherFlagStatus INTEGER,"
			"redPlayerCount INTEGER,"
			"bluePlayerCount INTEGER,"
			"sumPlayerCount INTEGER,"
			"killerClientNum	INTEGER NOT NULL,"
			"victimClientNum	INTEGER NOT NULL,"
			"isDoomKill	BOOLEAN NOT NULL,"
			"isExplosion	BOOLEAN NOT NULL,"
			"isSuicide	BOOLEAN NOT NULL,"
			"isModSuicide	BOOLEAN NOT NULL,"
			"meansOfDeath	INTEGER NOT NULL,"
			"positionX	REAL,"
			"positionY	REAL,"
			"positionZ	REAL,"
			"PRIMARY KEY(hash)"
			"); ",
			NULL, NULL, NULL);

		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE killAngles ("// old comment about serverName: Because of our dumb decision to mark processed demos in the serverName, we need to save this here to be able to distinguish them...
			"hash	TEXT,"
			"shorthash	TEXT,"
			"killerIsFlagCarrier	BOOLEAN NOT NULL,"
			"isReturn	BOOLEAN NOT NULL,"
			"isTeamKill	BOOLEAN NOT NULL,"
			"victimCapperKills INTEGER,"
			"victimCapperRets INTEGER,"
			"victimCapperWasFollowedOrVisible	BOOLEAN,"

			"victimCapperMaxNearbyEnemyCount	REAL,"
			"victimCapperMoreThanOneNearbyEnemyTimePercent	REAL," // Percentage
			"victimCapperAverageNearbyEnemyCount	REAL,"
			"victimCapperMaxVeryCloseEnemyCount	REAL,"
			"victimCapperAnyVeryCloseEnemyTimePercent	REAL," // Percentage
			"victimCapperMoreThanOneVeryCloseEnemyTimePercent	REAL," // Percentage
			"victimCapperAverageVeryCloseEnemyCount	REAL,"

			"victimFlagPickupSource	INTEGER,"
			"victimFlagHoldTime	INTEGER,"
			"targetIsVisible	BOOLEAN NOT NULL,"
			"targetIsFollowed	BOOLEAN NOT NULL,"
			"targetIsFollowedOrVisible	BOOLEAN NOT NULL,"
			"isSuicide	BOOLEAN NOT NULL,"
			"isModSuicide	BOOLEAN NOT NULL,"
			"attackerIsVisible	BOOLEAN NOT NULL,"
			"attackerIsFollowed	BOOLEAN NOT NULL,"
			"attackerIsFollowedOrVisible	BOOLEAN NOT NULL,"
			"demoRecorderClientnum	INTEGER NOT NULL,"

			"boosts	TEXT,"
			"boostCountTotal	INTEGER NOT NULL,"
			"boostCountAttacker	INTEGER NOT NULL,"
			"boostCountVictim	INTEGER NOT NULL,"

			"projectileWasAirborne	BOOLEAN,"

			"sameFrameRet	BOOLEAN,"

			"baseFlagDistance	REAL,"
			"headJumps	INTEGER,"
			"specialJumps	INTEGER,"
			"timeSinceLastSelfSentryJump	INTEGER,"
#if TRACK_GROUNDFRAME
			"groundFrameQuality	REAL,"
			"groundFrameAngleChange	REAL,"
#endif
			"throughWallNormal	REAL,"
			"throughWallOcclusion	INTEGER,"
			"saberComboCount	INTEGER,"

			"lastSneak	INTEGER,"
			"lastSneakDuration	INTEGER,"

			"resultingCaptures INTEGER,"
			"resultingSelfCaptures INTEGER,"
			"resultingLaughs INTEGER,"
			"metaEvents	TEXT,"

			"maxAngularSpeedAttacker	REAL,"
			"maxAngularAccelerationAttacker	REAL,"
			"maxAngularJerkAttacker	REAL,"
			"maxAngularSnapAttacker	REAL,"

			"maxSpeedAttacker	REAL,"
			"maxSpeedTarget	REAL,"
			"currentSpeedAttacker	REAL,"
			"currentSpeedTarget	REAL,"
			"lastSaberMoveChangeSpeed	REAL,"
			"timeSinceLastSaberMoveChange INTEGER,"
			"timeSinceLastBackflip INTEGER,"
			"meansOfDeathString	TEXT NOT NULL,"
			"nearbyPlayers	TEXT,"
			"nearbyPlayerCount	INTEGER NOT NULL,"
			"probableKillingWeapon	INTEGER NOT NULL,"

			"attackerJumpHeight	REAL,"
			"victimJumpHeight	REAL,"

			"directionX	REAL,"
			"directionY	REAL,"
			"directionZ	REAL,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"errorFlags INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE captures ("
			"id	INTEGER PRIMARY KEY,"
			"flagHoldTime	INTEGER NOT NULL,"
			"flagPickupSource	INTEGER NOT NULL,"
			"capperId INTEGER NOT NULL,"
			//"capperName	TEXT NOT NULL,"
			//"capperNameStripped	TEXT NOT NULL,"
			"capperClientNum INTEGER NOT NULL,"
			"capperIsVisible	BOOLEAN NOT NULL,"
			"capperIsFollowed	BOOLEAN NOT NULL,"
			"capperIsFollowedOrVisible	BOOLEAN NOT NULL,"
			"capperWasVisible	BOOLEAN NOT NULL,"
			"capperWasFollowed	BOOLEAN NOT NULL,"
			"capperWasFollowedOrVisible	BOOLEAN NOT NULL,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"flagTeam	INTEGER NOT NULL,"
			"capperKills INTEGER NOT NULL,"
			"capperRets INTEGER NOT NULL,"
			"sameFrameCap	BOOLEAN NOT NULL,"
			"redScore INTEGER,"
			"blueScore INTEGER,"
			"redPlayerCount INTEGER,"
			"bluePlayerCount INTEGER,"
			"sumPlayerCount INTEGER,"
			"boosts	TEXT,"
			"boostCount	INTEGER NOT NULL,"
			"maxSpeedCapperLastSecond	REAL,"
			"maxSpeedCapper	REAL,"
			"averageSpeedCapper	REAL,"
			"metaEvents	TEXT,"
			"nearbyPlayers	TEXT,"
			"nearbyPlayerCount	INTEGER NOT NULL,"
			"nearbyEnemies	TEXT,"
			"nearbyEnemyCount	INTEGER NOT NULL,"
			"maxNearbyEnemyCount	REAL NOT NULL,"
			"moreThanOneNearbyEnemyTimePercent	REAL NOT NULL," // Percentage
			"averageNearbyEnemyCount	REAL NOT NULL,"
			"maxVeryCloseEnemyCount	REAL NOT NULL,"
			"anyVeryCloseEnemyTimePercent	REAL NOT NULL," // Percentage
			"moreThanOneVeryCloseEnemyTimePercent	REAL NOT NULL," // Percentage
			"averageVeryCloseEnemyCount	REAL NOT NULL,"
			"directionX	REAL,"
			"directionY	REAL,"
			"directionZ	REAL,"
			"positionX	REAL,"
			"positionY	REAL,"
			"positionZ	REAL,"
			"resultingLaughs INTEGER,"
			"resultingLaughsAfter INTEGER,"
			"pastLocations	TEXT," // special rough tracking of locations during the run.
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE flaggrabs ("
			"id	INTEGER PRIMARY KEY,"
			"enemyFlagHoldTime	INTEGER,"
			"flagPickupSource	INTEGER NOT NULL,"
			"grabberId INTEGER NOT NULL,"
			"capperId INTEGER,"
			//"grabberName	TEXT NOT NULL,"
			//"grabberNameStripped	TEXT NOT NULL,"
			//"capperName	TEXT,"
			//"capperNameStripped	TEXT,"
			"grabberClientNum INTEGER NOT NULL,"
			"capperClientNum INTEGER,"
			"grabberIsVisible	BOOLEAN NOT NULL,"
			"grabberIsFollowed	BOOLEAN NOT NULL,"
			"grabberIsFollowedOrVisible	BOOLEAN NOT NULL,"
			"capperIsVisible	BOOLEAN,"
			"capperIsFollowed	BOOLEAN,"
			"capperIsFollowedOrVisible	BOOLEAN,"
			"capperWasVisible	BOOLEAN,"
			"capperWasFollowed	BOOLEAN,"
			"capperWasFollowedOrVisible	BOOLEAN,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"flagTeam	INTEGER NOT NULL,"
			"capperKills INTEGER,"
			"capperRets INTEGER,"
			"redScore INTEGER,"
			"blueScore INTEGER,"
			"redPlayerCount INTEGER,"
			"bluePlayerCount INTEGER,"
			"sumPlayerCount INTEGER,"
			"boosts	TEXT,"
			"boostCount	INTEGER NOT NULL,"
			"maxSpeedGrabberLastSecond	REAL,"
			"maxSpeedCapperLastSecond	REAL,"
			"maxSpeedCapper	REAL,"
			"averageSpeedCapper	REAL,"
			"metaEvents	TEXT,"
			"nearbyPlayers	TEXT,"
			"nearbyPlayerCount	INTEGER NOT NULL,"
			"nearbyEnemies	TEXT,"
			"nearbyEnemyCount	INTEGER NOT NULL,"
			"veryCloseEnemyCount	INTEGER NOT NULL,"
			"veryClosePlayersCount	INTEGER NOT NULL,"
			"padNearbyPlayers	TEXT," // for swoops
			"padNearbyPlayerCount	INTEGER NOT NULL," // for swoops
			"padNearbyEnemies	TEXT," // for swoops
			"padNearbyEnemyCount	INTEGER NOT NULL," // for swoops
			"padVeryCloseEnemyCount	INTEGER NOT NULL," // for swoops
			"padVeryClosePlayersCount	INTEGER NOT NULL," // for swoops
			"maxNearbyEnemyCount	REAL,"
			"moreThanOneNearbyEnemyTimePercent	REAL," // Percentage
			"averageNearbyEnemyCount	REAL,"
			"maxVeryCloseEnemyCount	REAL,"
			"anyVeryCloseEnemyTimePercent	REAL," // Percentage
			"moreThanOneVeryCloseEnemyTimePercent	REAL," // Percentage
			"averageVeryCloseEnemyCount	REAL,"
			"capperPadDistance	REAL,"
			"capperTimeToCap	INTEGER,"
			"directionX	REAL,"
			"directionY	REAL,"
			"directionZ	REAL,"
			"positionX	REAL,"
			"positionY	REAL,"
			"positionZ	REAL,"
			"capperDirectionX	REAL,"
			"capperDirectionY	REAL,"
			"capperDirectionZ	REAL,"
			"capperPositionX	REAL,"
			"capperPositionY	REAL,"
			"capperPositionZ	REAL,"
			"resultingCaptures INTEGER,"
			"resultingSelfCaptures INTEGER,"
			"resultingEnemyCaptures INTEGER,"
			"resultingLaughs INTEGER,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE defragRuns ("
			"readableTime TEXT NOT NULL,"
			"totalMilliseconds	INTEGER,"
			"style	TEXT,"
			"playerId	INTEGER NOT NULL,"
			//"playerName	TEXT NOT NULL,"
			//"playerNameStripped	TEXT NOT NULL,"
			"isTop10	BOOLEAN NOT NULL,"
			"isNumber1	BOOLEAN NOT NULL,"
			"isPersonalBest	BOOLEAN NOT NULL,"
			"runTeleProRun	BOOLEAN,"
			"runTeleTeleports	INTEGER,"
			"runTeleCheckpoints	INTEGER,"
			"wasVisible	BOOLEAN NOT NULL,"
			"wasFollowed	BOOLEAN NOT NULL,"
			"wasFollowedOrVisible	BOOLEAN NOT NULL,"
			"averageStrafeDeviation REAL,"
			"runnerClientNum	INTEGER NOT NULL,"
			"resultingLaughs	INTEGER,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE laughs ("
			"id	INTEGER PRIMARY KEY,"
			"laughs	TEXT NOT NULL,"
			"chatlog	TEXT NOT NULL,"
			"chatlogStripped	TEXT NOT NULL,"
			"laughCount	INTEGER NOT NULL,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"duration INTEGER NOT NULL,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE special ("
			"id	INTEGER PRIMARY KEY,"
			"type	TEXT NOT NULL,"
			"clientNum	INTEGER,"
			"clientNumAlt	INTEGER,"
			"playerId	INTEGER,"
			"playerIdAlt	INTEGER,"
			//"playerName	TEXT,"
			//"playerNameStripped	TEXT,"
			//"playerNameAlt	TEXT,"
			//"playerNameAltStripped	TEXT,"
			"details	TEXT,"
			"detailsStripped	TEXT,"
			"comment	TEXT,"
			"commentStripped	TEXT,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"duration INTEGER NOT NULL,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE killSprees ("
			"hash	TEXT,"
			"shorthash	TEXT,"
			"maxDelay	INTEGER NOT NULL,"
			"maxDelayActual	INTEGER NOT NULL,"
			"killerId	INTEGER NOT NULL,"
			//"killerName	TEXT NOT NULL,"
			//"killerNameStripped	TEXT NOT NULL,"
			"victimNames	TEXT NOT NULL,"
			"victimNamesStripped	TEXT NOT NULL,"
			"killTypes	TEXT NOT NULL,"
			"killTypesCount	INTEGER NOT NULL,"
			"killHashes	TEXT NOT NULL,"
			"killerClientNum	INTEGER NOT NULL,"
			"victimClientNums	TEXT NOT NULL,"
			"countKills	INTEGER NOT NULL,"
			"countRets	INTEGER NOT NULL,"
			"countTeamKills	INTEGER NOT NULL,"
			"countUniqueTargets	INTEGER NOT NULL,"
			"countDooms	INTEGER NOT NULL,"
			"countExplosions	INTEGER NOT NULL,"
			"countThirdPersons	INTEGER NOT NULL,"
			"countInvisibles	INTEGER NOT NULL,"
			"nearbyPlayers	TEXT,"
			"nearbyPlayerCount	INTEGER NOT NULL,"
			"demoRecorderClientnum	INTEGER NOT NULL,"
			"maxSpeedAttacker	REAL,"
			"maxSpeedTargets	REAL,"
			"resultingCaptures INTEGER,"
			"resultingSelfCaptures INTEGER,"
			"resultingCapturesAfter INTEGER,"
			"resultingSelfCapturesAfter INTEGER,"
			"resultingLaughs INTEGER,"
			"resultingLaughsAfter INTEGER,"
			"metaEvents	TEXT,"
			"demoTime INTEGER NOT NULL,"
			"lastGamestateDemoTime INTEGER NOT NULL,"
			"duration INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"entryMeta INTEGER NOT NULL"
			//"PRIMARY KEY(hash)"
			"); ",
			NULL, NULL, NULL);



		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE demoDatabaseProperties ("
			"propertyName	TEXT NOT NULL,"
			"value	TEXT NOT NULL,"
			"PRIMARY KEY(propertyName)"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE demoMeta ("
			"demoName	TEXT NOT NULL,"
			"demoPath	TEXT NOT NULL,"
			"fileSize INTEGER NOT NULL,"
			"demoTimeDuration INTEGER NOT NULL,"
			"demoDateTime TIMESTAMP NOT NULL,"
			"demoRecorderNames	TEXT NOT NULL,"
			"demoRecorderNamesStripped	TEXT NOT NULL,"
			"demoErrorFlags INTEGER NOT NULL,"
			"demoErrors	TEXT,"
			"demoDerivativeFlags INTEGER NOT NULL,"
			"PRIMARY KEY(demoPath)"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE entryMeta ("
			"id	INTEGER PRIMARY KEY,"
			"map	TEXT NOT NULL,"
			"serverName	TEXT NOT NULL,"
			"serverNameStripped	TEXT NOT NULL,"
			"demoName TEXT NOT NULL,"
			"demoPath TEXT NOT NULL,"
			"demoDateTime TIMESTAMP NOT NULL,"
			"demoDerivativeFlags INTEGER NOT NULL," // mark reframes/optiized demos
			"count INTEGER NOT NULL,"
			"UNIQUE(map,serverName,serverNameStripped,demoName,demoPath,demoDateTime,demoDerivativeFlags)"
			"); ",
			NULL, NULL, NULL);


		sqlite3_exec(io.killDb[i].killDb, "CREATE TABLE playerNames ("
			"id	INTEGER PRIMARY KEY,"
			"playerName	TEXT NOT NULL,"
			"playerNameStripped	TEXT NOT NULL,"
			"count INTEGER NOT NULL,"
			"UNIQUE(playerName,playerNameStripped)"
			"); ",
			NULL, NULL, NULL);

// TODO: Would be more efficient to do the inserts manually somewhere and then inject the id. but lets keep it like this for now so we can quickly adapt the code without major restructuring

		/*char* preparedStatementText = "INSERT INTO kills"
			"(hash, shorthash, map, killerName, victimName, killerClientNum, victimClientNum, isReturn, isDoomKill, isExplosion, isSuicide, targetIsVisible,attackerIsVisible,"
			"isFollowed, meansOfDeath, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTarget, meansOfDeathString, probableKillingWeapon, positionX, "
			"positionY, positionZ,demoName,demoTime, serverTime, demoDateTime)"
			"VALUES "
			"(@hash, @shorthash, @map, @killerName, @victimName, @killerClientNum, @victimClientNum, @isReturn, @isDoomKill, @isExplosion, @isSuicide, @targetIsVisible,@attackerIsVisible,"
			"@isFollowed, @meansOfDeath, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTarget, @meansOfDeathString, @probableKillingWeapon, @positionX,"
			"@positionY, @positionZ,@demoName,@demoTime, @serverTime, @demoDateTime);";*/
		preparedStatementText = "INSERT INTO kills"
			"(hash,shorthash,map,killerId,victimId,killerTeam,victimTeam,redScore,blueScore,otherFlagStatus,redPlayerCount,bluePlayerCount,sumPlayerCount,killerClientNum,victimClientNum,isDoomKill,isExplosion,isSuicide,isModSuicide,meansOfDeath,positionX,positionY,positionZ)"
			"VALUES "
			"(@hash,@shorthash,@map,@killerId,@victimId,@killerTeam,@victimTeam,@redScore,@blueScore,@otherFlagStatus,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@killerClientNum,@victimClientNum,@isDoomKill,@isExplosion,@isSuicide,@isModSuicide,@meansOfDeath,@positionX,@positionY,@positionZ);";
		;
		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertStatement, NULL);
		preparedStatementText = "INSERT INTO killAngles"
			"(hash,shorthash,killerIsFlagCarrier,isReturn,isTeamKill,victimCapperKills,victimCapperRets,victimCapperWasFollowedOrVisible,victimCapperMaxNearbyEnemyCount,victimCapperMoreThanOneNearbyEnemyTimePercent,victimCapperAverageNearbyEnemyCount,victimCapperMaxVeryCloseEnemyCount,victimCapperAnyVeryCloseEnemyTimePercent,victimCapperMoreThanOneVeryCloseEnemyTimePercent,victimCapperAverageVeryCloseEnemyCount,victimFlagPickupSource,victimFlagHoldTime,targetIsVisible,targetIsFollowed,targetIsFollowedOrVisible,attackerIsVisible,attackerIsFollowed,demoRecorderClientnum,boosts,boostCountTotal,boostCountAttacker,boostCountVictim,projectileWasAirborne,sameFrameRet,baseFlagDistance,headJumps,specialJumps,timeSinceLastSelfSentryJump"
#if TRACK_GROUNDFRAME
",groundFrameQuality,groundFrameAngleChange"
#endif
			",throughWallNormal,throughWallOcclusion,saberComboCount,lastSneak,lastSneakDuration,resultingCaptures,resultingSelfCaptures,resultingLaughs,metaEvents,maxAngularSpeedAttacker,maxAngularAccelerationAttacker,maxAngularJerkAttacker,maxAngularSnapAttacker,maxSpeedAttacker,maxSpeedTarget,currentSpeedAttacker,currentSpeedTarget,meansOfDeathString,probableKillingWeapon,demoTime,lastGamestateDemoTime,serverTime,lastSaberMoveChangeSpeed,timeSinceLastSaberMoveChange,timeSinceLastBackflip,nearbyPlayers,nearbyPlayerCount,attackerJumpHeight, victimJumpHeight,directionX,directionY,directionZ,isSuicide,isModSuicide,attackerIsFollowedOrVisible,errorFlags,entryMeta)"
			"VALUES "
			"(@hash,@shorthash,@killerIsFlagCarrier,@isReturn,@isTeamKill,@victimCapperKills,@victimCapperRets,@victimCapperWasFollowedOrVisible,@victimCapperMaxNearbyEnemyCount,@victimCapperMoreThanOneNearbyEnemyTimePercent,@victimCapperAverageNearbyEnemyCount,@victimCapperMaxVeryCloseEnemyCount,@victimCapperAnyVeryCloseEnemyTimePercent,@victimCapperMoreThanOneVeryCloseEnemyTimePercent,@victimCapperAverageVeryCloseEnemyCount,@victimFlagPickupSource,@victimFlagHoldTime,@targetIsVisible,@targetIsFollowed,@targetIsFollowedOrVisible,@attackerIsVisible,@attackerIsFollowed,@demoRecorderClientnum,@boosts,@boostCountTotal,@boostCountAttacker,@boostCountVictim,@projectileWasAirborne,@sameFrameRet,@baseFlagDistance,@headJumps,@specialJumps,@timeSinceLastSelfSentryJump"
#if TRACK_GROUNDFRAME
",@groundFrameQuality,@groundFrameAngleChange"
#endif	
",@throughWallNormal,@throughWallOcclusion,@saberComboCount,@lastSneak,@lastSneakDuration,@resultingCaptures,@resultingSelfCaptures,@resultingLaughs,@metaEvents,@maxAngularSpeedAttacker,@maxAngularAccelerationAttacker,@maxAngularJerkAttacker,@maxAngularSnapAttacker,@maxSpeedAttacker,@maxSpeedTarget,@currentSpeedAttacker,@currentSpeedTarget,@meansOfDeathString,@probableKillingWeapon,@demoTime, @lastGamestateDemoTime,@serverTime,@lastSaberMoveChangeSpeed,@timeSinceLastSaberMoveChange,@timeSinceLastBackflip,@nearbyPlayers,@nearbyPlayerCount,@attackerJumpHeight, @victimJumpHeight,@directionX,@directionY,@directionZ,@isSuicide,@isModSuicide,@attackerIsFollowedOrVisible,@errorFlags,@entryMeta);";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertAngleStatement, NULL);
		preparedStatementText = "INSERT INTO captures"
			"(flagHoldTime,flagPickupSource,capperId,capperClientNum,capperIsVisible,capperIsFollowed,capperIsFollowedOrVisible,capperWasVisible,capperWasFollowed,capperWasFollowedOrVisible,demoRecorderClientnum,flagTeam,capperKills,capperRets,sameFrameCap,redScore,blueScore,redPlayerCount,bluePlayerCount,sumPlayerCount,boosts,boostCount,maxSpeedCapperLastSecond,maxSpeedCapper,averageSpeedCapper,metaEvents,nearbyPlayers,nearbyPlayerCount,nearbyEnemies,nearbyEnemyCount,maxNearbyEnemyCount,moreThanOneNearbyEnemyTimePercent,averageNearbyEnemyCount,maxVeryCloseEnemyCount,anyVeryCloseEnemyTimePercent,moreThanOneVeryCloseEnemyTimePercent,averageVeryCloseEnemyCount,directionX,directionY,directionZ,positionX,positionY,positionZ,resultingLaughs,resultingLaughsAfter,pastLocations,demoTime,lastGamestateDemoTime,serverTime,entryMeta)"
			"VALUES "
			"(@flagHoldTime,@flagPickupSource,@capperId,@capperClientNum,@capperIsVisible,@capperIsFollowed,@capperIsFollowedOrVisible,@capperWasVisible,@capperWasFollowed,@capperWasFollowedOrVisible,@demoRecorderClientnum,@flagTeam,@capperKills,@capperRets,@sameFrameCap,@redScore,@blueScore,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@boosts,@boostCount,@maxSpeedCapperLastSecond,@maxSpeedCapper,@averageSpeedCapper,@metaEvents,@nearbyPlayers,@nearbyPlayerCount,@nearbyEnemies,@nearbyEnemyCount,@maxNearbyEnemyCount,@moreThanOneNearbyEnemyTimePercent,@averageNearbyEnemyCount,@maxVeryCloseEnemyCount,@anyVeryCloseEnemyTimePercent,@moreThanOneVeryCloseEnemyTimePercent,@averageVeryCloseEnemyCount,@directionX,@directionY,@directionZ,@positionX,@positionY,@positionZ,@resultingLaughs,@resultingLaughsAfter,@pastLocations,@demoTime, @lastGamestateDemoTime,@serverTime,@entryMeta);";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertCaptureStatement, NULL);

		preparedStatementText = "INSERT INTO flaggrabs"
			"(enemyFlagHoldTime,flagPickupSource,grabberId,capperId,grabberClientNum,capperClientNum,grabberIsVisible,grabberIsFollowed,grabberIsFollowedOrVisible,capperIsVisible,capperIsFollowed,capperIsFollowedOrVisible,capperWasVisible,capperWasFollowed,capperWasFollowedOrVisible,demoRecorderClientnum,flagTeam,capperKills,capperRets,redScore,blueScore,redPlayerCount,bluePlayerCount,sumPlayerCount,boosts,boostCount,maxSpeedGrabberLastSecond,maxSpeedCapperLastSecond,maxSpeedCapper,averageSpeedCapper,metaEvents,nearbyPlayers,nearbyPlayerCount,nearbyEnemies,nearbyEnemyCount,veryCloseEnemyCount,veryClosePlayersCount,padNearbyPlayers,padNearbyPlayerCount,padNearbyEnemies,padNearbyEnemyCount,padVeryCloseEnemyCount,padVeryClosePlayersCount,maxNearbyEnemyCount,moreThanOneNearbyEnemyTimePercent,averageNearbyEnemyCount,maxVeryCloseEnemyCount,anyVeryCloseEnemyTimePercent,moreThanOneVeryCloseEnemyTimePercent,averageVeryCloseEnemyCount,capperPadDistance,capperTimeToCap,directionX,directionY,directionZ,positionX,positionY,positionZ,capperDirectionX,capperDirectionY,capperDirectionZ,capperPositionX,capperPositionY,capperPositionZ,resultingCaptures,resultingSelfCaptures,resultingEnemyCaptures,resultingLaughs,demoTime,lastGamestateDemoTime,serverTime,entryMeta)"
			"VALUES "
			"(@enemyFlagHoldTime,@flagPickupSource,@grabberId,@capperId,@grabberClientNum,@capperClientNum,@grabberIsVisible,@grabberIsFollowed,@grabberIsFollowedOrVisible,@capperIsVisible,@capperIsFollowed,@capperIsFollowedOrVisible,@capperWasVisible,@capperWasFollowed,@capperWasFollowedOrVisible,@demoRecorderClientnum,@flagTeam,@capperKills,@capperRets,@redScore,@blueScore,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@boosts,@boostCount,@maxSpeedGrabberLastSecond,@maxSpeedCapperLastSecond,@maxSpeedCapper,@averageSpeedCapper,@metaEvents,@nearbyPlayers,@nearbyPlayerCount,@nearbyEnemies,@nearbyEnemyCount,@veryCloseEnemyCount,@veryClosePlayersCount,@padNearbyPlayers,@padNearbyPlayerCount,@padNearbyEnemies,@padNearbyEnemyCount,@padVeryCloseEnemyCount,@padVeryClosePlayersCount,@maxNearbyEnemyCount,@moreThanOneNearbyEnemyTimePercent,@averageNearbyEnemyCount,@maxVeryCloseEnemyCount,@anyVeryCloseEnemyTimePercent,@moreThanOneVeryCloseEnemyTimePercent,@averageVeryCloseEnemyCount,@capperPadDistance,@capperTimeToCap,@directionX,@directionY,@directionZ,@positionX,@positionY,@positionZ,@capperDirectionX,@capperDirectionY,@capperDirectionZ,@capperPositionX,@capperPositionY,@capperPositionZ,@resultingCaptures,@resultingSelfCaptures,@resultingEnemyCaptures,@resultingLaughs,@demoTime,@lastGamestateDemoTime,@serverTime,@entryMeta);";


		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertFlagGrabStatement, NULL);

		preparedStatementText = "INSERT INTO defragRuns"
			"(readableTime,totalMilliseconds,style,playerId,demoRecorderClientnum,runnerClientNum,isTop10,isNumber1,isPersonalBest,runTeleProRun,runTeleTeleports,runTeleCheckpoints,wasVisible,wasFollowed,wasFollowedOrVisible,averageStrafeDeviation,resultingLaughs,demoTime,lastGamestateDemoTime,serverTime,entryMeta)"
			"VALUES "
			"(@readableTime,@totalMilliseconds,@style,@playerId,@demoRecorderClientnum,@runnerClientNum,@isTop10,@isNumber1,@isPersonalBest,@runTeleProRun,@runTeleTeleports,@runTeleCheckpoints,@wasVisible,@wasFollowed,@wasFollowedOrVisible,@averageStrafeDeviation,@resultingLaughs,@demoTime, @lastGamestateDemoTime,@serverTime,@entryMeta);";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertDefragRunStatement, NULL);

		preparedStatementText = "INSERT INTO laughs"
			"(laughs,chatlog,chatlogStripped,laughCount,demoRecorderClientnum,duration,demoTime,lastGamestateDemoTime,serverTime,entryMeta)"
			"VALUES "
			"(@laughs,@chatlog,@chatlogStripped,@laughCount,@demoRecorderClientnum,@duration,@demoTime, @lastGamestateDemoTime,@serverTime,@entryMeta);";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertLaughsStatement, NULL);

//#define ENTRYMETA_PRESTATEMENT "INSERT INTO entryMeta (map,serverName,serverNameStripped,demoName,demoPath,demoDateTime) VALUES (@map,@serverName,@serverNameStripped,@demoName,@demoPath,@demoDateTime) ON CONFLICT (map,serverName,serverNameStripped,demoName,demoPath,demoDateTime) DO NOTHING; " // RETURNING id; the returning doesnt do anything rn but oh well
//#define ENTRYMETA_IDSELECTOR "(SELECT id FROM entryMeta WHERE map=@map AND serverName=@serverName AND serverNameStripped=@serverNameStripped AND demoName=@demoName AND demoPath=@demoPath AND demoDateTime=@demoDateTime )"
		preparedStatementText = "INSERT INTO entryMeta (map,serverName,serverNameStripped,demoName,demoPath,demoDateTime,demoDerivativeFlags,count) VALUES (@map,@serverName,@serverNameStripped,@demoName,@demoPath,@demoDateTime,@demoDerivativeFlags,1) ON CONFLICT (map,serverName,serverNameStripped,demoName,demoPath,demoDateTime,demoDerivativeFlags) DO UPDATE SET count=count+1 RETURNING id;";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertEntryMetaStatement, NULL);

		preparedStatementText = "INSERT INTO playerNames (playerName,playerNameStripped,count) VALUES (@playerName,@playerNameStripped,1) ON CONFLICT (playerName,playerNameStripped) DO UPDATE SET count=count+1 RETURNING id;";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertPlayerNameStatement, NULL);

		preparedStatementText = "INSERT INTO special"
			"(type,clientNum,clientNumAlt,playerId,playerIdAlt,details,detailsStripped,comment,commentStripped,demoRecorderClientnum,duration,demoTime,lastGamestateDemoTime,serverTime,entryMeta)"
			"VALUES "
			"(@type,@clientNum,@clientNumAlt,@playerId,@playerIdAlt,@details,@detailsStripped,@comment,@commentStripped,@demoRecorderClientnum,@duration,@demoTime,@lastGamestateDemoTime,@serverTime,@entryMeta);";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertSpecialStatement, NULL);

		preparedStatementText = "INSERT INTO killSprees "
			"( hash, shorthash,maxDelay,maxDelayActual,killerId, victimNames, victimNamesStripped ,killTypes, killTypesCount,killHashes, killerClientNum, victimClientNums, countKills, countRets, countTeamKills,countUniqueTargets, countDooms, countExplosions,"
			" countThirdPersons, countInvisibles, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTargets,resultingCaptures,resultingSelfCaptures,resultingCapturesAfter,resultingSelfCapturesAfter,resultingLaughs,resultingLaughsAfter,metaEvents,demoTime,lastGamestateDemoTime,duration,serverTime,nearbyPlayers,nearbyPlayerCount,entryMeta)"
			" VALUES "
			"( @hash, @shorthash, @maxDelay, @maxDelayActual, @killerId, @victimNames ,@victimNamesStripped, @killTypes,@killTypesCount ,@killHashes, @killerClientNum, @victimClientNums, @countKills, @countRets,@countTeamKills,@countUniqueTargets, @countDooms, @countExplosions,"
			" @countThirdPersons, @countInvisibles, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTargets,@resultingCaptures,@resultingSelfCaptures,@resultingCapturesAfter,@resultingSelfCapturesAfter,@resultingLaughs,@resultingLaughsAfter,@metaEvents,@demoTime, @lastGamestateDemoTime,@duration,@serverTime,@nearbyPlayers,@nearbyPlayerCount,@entryMeta)";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertSpreeStatement, NULL);
		preparedStatementText = "INSERT INTO demoDatabaseProperties "
			"( propertyName, value )"
			" VALUES "
			"( @propertyName, @value )";
		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertDemoDatabaseProperty, NULL);

		preparedStatementText = "INSERT INTO demoMeta "
			"( demoName, demoPath, fileSize, demoTimeDuration, demoDateTime, demoRecorderNames, demoRecorderNamesStripped, demoErrorFlags, demoErrors,demoDerivativeFlags  )"
			" VALUES "
			"( @demoName, @demoPath, @fileSize, @demoTimeDuration, @demoDateTime, @demoRecorderNames, @demoRecorderNamesStripped, @demoErrorFlags, @demoErrors,@demoDerivativeFlags)";
		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].insertDemoMeta, NULL);

		preparedStatementText = "SELECT last_insert_rowid();";

		sqlite3_prepare_v2(io.killDb[i].killDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.killDb[i].selectLastInsertRowIdStatement, NULL);

		sqlite3_exec(io.killDb[i].killDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "INSERT OR IGNORE INTO demoDatabaseProperties (`propertyName`,`value`) VALUES ('serverNameInKillAngles','1');", NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "INSERT OR IGNORE INTO demoDatabaseProperties (`propertyName`,`value`) VALUES ('serverNameInKillSpree','1');", NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "INSERT OR IGNORE INTO demoDatabaseProperties (`propertyName`,`value`) VALUES ('entryMetaTable','2');", NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "INSERT OR IGNORE INTO demoDatabaseProperties (`propertyName`,`value`) VALUES ('playerNamesTable','1');", NULL, NULL, NULL);
		sqlite3_exec(io.killDb[i].killDb, "INSERT OR IGNORE INTO demoDatabaseProperties (`propertyName`,`value`) VALUES ('hashType','base58');", NULL, NULL, NULL);


	}

	if (opts.doStatsDb) {
		while ((sqlResult = sqlite3_open("statistics.db", &io.statsDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.statsDb, "main"))) {
			std::cerr << DPrintFLocation << ":" << "error opening stats.db for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.statsDb) << ". Trying again in 1000ms." << "\n";
			sqlite3_close(io.statsDb);
			io.statsDb = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		sqlite3_exec(io.statsDb, "CREATE TABLE playerModels ("
			"map	TEXT NOT NULL,"
			"baseModel	TEXT NOT NULL,"
			"variant	TEXT,"
			"countFound INTEGER NOT NULL,"
			"PRIMARY KEY(map,baseModel,variant)"
			"); ",
			NULL, NULL, NULL);
		sqlite3_exec(io.statsDb, "CREATE TABLE playerDemoStats ("
			"map	TEXT NOT NULL,"
			"playerName TEXT NOT NULL,"
			"playerNameStripped TEXT NOT NULL,"
			"clientNum INT NOT NULL,"
			"averageStrafeDeviation REAL,"
			"averageStrafeDeviationBucketsJSON TEXT,"
			"strafeSampleCount INTEGER NOT NULL,"
			"averageStrafeDeviationNoSaberMove REAL,"
			"averageStrafeDeviationNoSaberMoveBucketsJSON TEXT,"
			"strafeNoSaberMoveSampleCount INTEGER NOT NULL,"
#if TRACK_GROUNDFRAME
			"averageGroundFrameQuality REAL,"
			"groundFrameSampleCount INTEGER NOT NULL,"
#endif
			"hitBySaberCount INTEGER NOT NULL,"
			"hitBySaberBlockableCount INTEGER NOT NULL,"
			"parryCount INTEGER NOT NULL,"
			"attackFromParryCount INTEGER NOT NULL,"
			"demoName TEXT NOT NULL,"
			"demoPath TEXT NOT NULL,"
			"demoDateTime TIMESTAMP NOT NULL"//,"
			//"PRIMARY KEY(playerName,clientNum,map,demoPath)"
			"); ",
			NULL, NULL, NULL);

		preparedStatementText = "SELECT last_insert_rowid();";

		sqlite3_prepare_v2(io.statsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.selectStatsLastInsertRowIdStatement, NULL);

		preparedStatementText = "INSERT OR IGNORE INTO playerModels (map,baseModel,variant,countFound) VALUES (@map,@baseModel,@variant, 0);";

		sqlite3_prepare_v2(io.statsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertPlayerModelStatement, NULL);
		preparedStatementText = "UPDATE playerModels SET countFound = countFound + 1 WHERE map=@map AND baseModel=@baseModel AND variant=@variant;";

		sqlite3_prepare_v2(io.statsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.updatePlayerModelCountStatement, NULL);

		preparedStatementText = "INSERT INTO playerDemoStats "
			"(map,playerName,playerNameStripped,clientNum,averageStrafeDeviation,averageStrafeDeviationBucketsJSON,averageStrafeDeviationNoSaberMove,averageStrafeDeviationNoSaberMoveBucketsJSON"
#if TRACK_GROUNDFRAME
			",averageGroundFrameQuality,groundFrameSampleCount"
#endif
			",strafeSampleCount,strafeNoSaberMoveSampleCount,hitBySaberCount,hitBySaberBlockableCount,parryCount,attackFromParryCount,demoName,demoPath,demoDateTime)"
			" VALUES "
			"( @map,@playerName,@playerNameStripped,@clientNum,@averageStrafeDeviation,@averageStrafeDeviationBucketsJSON,@averageStrafeDeviationNoSaberMove,@averageStrafeDeviationNoSaberMoveBucketsJSON"
#if TRACK_GROUNDFRAME
			",@averageGroundFrameQuality,@groundFrameSampleCount"
#endif
			",@strafeSampleCount,@strafeNoSaberMoveSampleCount,@hitBySaberCount,@hitBySaberBlockableCount,@parryCount,@attackFromParryCount,@demoName,@demoPath,@demoDateTime)";

		sqlite3_prepare_v2(io.statsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertPlayerDemoStatsStatement, NULL);

		sqlite3_exec(io.statsDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	}
	else {
		io.statsDb = NULL;
		io.selectStatsLastInsertRowIdStatement = NULL;
		io.insertPlayerModelStatement = NULL;
		io.updatePlayerModelCountStatement = NULL;
		io.insertPlayerDemoStatsStatement = NULL;
	}


	if (opts.writeDemoPacketStats) {

		while ((sqlResult = sqlite3_open("demoStats.db", &io.demoStatsDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.demoStatsDb, "main"))) {
			std::cerr << DPrintFLocation << ":" << "error opening demoStats.db for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.demoStatsDb) << ". Trying again in 1000ms." << "\n";
			sqlite3_close(io.demoStatsDb);
			io.demoStatsDb = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		sqlite3_exec(io.demoStatsDb, "CREATE TABLE demoPacketStats ("
			"demoTime INTEGER NOT NULL,"
			"serverTime INTEGER NOT NULL,"
			"timeSinceLast INTEGER NOT NULL," // demotime since last point
			"skippedPacketsSinceLast INTEGER NOT NULL,"
			"bytesSinceLast INTEGER NOT NULL,"
			"bytesPerSecond INTEGER NOT NULL,"
			"snapshotBytesSinceLast INTEGER NOT NULL,"
			"snapshotBytesPerSecond INTEGER NOT NULL,"
			"serverCommandBytesSinceLast INTEGER NOT NULL,"
			"serverCommandBytesPerSecond INTEGER NOT NULL,"
			"packetsSinceLast INTEGER NOT NULL,"
			"packetsPerSecond REAL NOT NULL,"
			"bytesPerPacket INTEGER NOT NULL,"
			"periodMaxPacketSize INTEGER NOT NULL,"
			"periodMinPacketSize INTEGER NOT NULL,"
			"nonDeltaSnapsSinceLast INTEGER NOT NULL,"
			"nonDeltaSnapsPerSecond INTEGER NOT NULL,"
			"angleChangesSinceLast INTEGER NOT NULL,"
			"angleChangesPerSecond INTEGER NOT NULL,"
			"entitiesSinceLast INTEGER NOT NULL,"
			"entitiesPerSecond INTEGER NOT NULL,"
			"entitiesPerPacket INTEGER NOT NULL,"
			"demoName TEXT NOT NULL,"
			"demoPath TEXT NOT NULL,"
			"demoDateTime TIMESTAMP NOT NULL"
			"); ",
			NULL, NULL, NULL);
		preparedStatementText = "INSERT INTO demoPacketStats "
			"(demoTime,serverTime,timeSinceLast,skippedPacketsSinceLast,bytesSinceLast,bytesPerSecond,snapshotBytesSinceLast,snapshotBytesPerSecond,serverCommandBytesSinceLast,serverCommandBytesPerSecond,packetsSinceLast,packetsPerSecond,bytesPerPacket,periodMaxPacketSize,periodMinPacketSize,nonDeltaSnapsSinceLast,nonDeltaSnapsPerSecond,angleChangesSinceLast,angleChangesPerSecond,entitiesSinceLast,entitiesPerSecond,entitiesPerPacket,demoName,demoPath,demoDateTime)"
			" VALUES "
			"(@demoTime,@serverTime,@timeSinceLast,@skippedPacketsSinceLast,@bytesSinceLast,@bytesPerSecond,@snapshotBytesSinceLast,@snapshotBytesPerSecond,@serverCommandBytesSinceLast,@serverCommandBytesPerSecond,@packetsSinceLast,@packetsPerSecond,@bytesPerPacket,@periodMaxPacketSize,@periodMinPacketSize,@nonDeltaSnapsSinceLast,@nonDeltaSnapsPerSecond,@angleChangesSinceLast,@angleChangesPerSecond,@entitiesSinceLast,@entitiesPerSecond,@entitiesPerPacket,@demoName,@demoPath,@demoDateTime)";

		sqlite3_prepare_v2(io.demoStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertPacketStatsStatement, NULL);
		sqlite3_exec(io.demoStatsDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	}
	else {
		io.demoStatsDb = NULL;
		io.insertPacketStatsStatement = NULL;
	}

	if (opts.writeChatsUnique) {

		while ((sqlResult = sqlite3_open("chatsUnique.db", &io.chatsUniqueDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.chatsUniqueDb, "main"))) {
			std::cerr << DPrintFLocation << ":" << "error opening chatsUnique.db for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.chatsUniqueDb) << ". Trying again in 1000ms." << "\n";
			sqlite3_close(io.chatsUniqueDb);
			io.chatsUniqueDb = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		sqlite3_exec(io.chatsUniqueDb, "CREATE TABLE chatsUnique ("
			"chat TEXT NOT NULL COLLATE BINARY,"
			"demoType INTEGER NOT NULL,"
			"countFound INTEGER NOT NULL,"
			"PRIMARY KEY (demoType,chat COLLATE BINARY)"
			"); ",
			NULL, NULL, NULL);

		preparedStatementText = "INSERT OR IGNORE INTO chatsUnique (chat, demoType, countFound) VALUES (@chat, @demoType,0);";
		sqlite3_prepare_v2(io.chatsUniqueDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertChatUniqueStatement, NULL);

		preparedStatementText = "UPDATE chatsUnique SET countFound = countFound + 1 WHERE chat=@chat AND demoType=@demoType;"; // TODO make this dynamic? not a million queries for dupes?
		sqlite3_prepare_v2(io.chatsUniqueDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.updateChatUniqueCountStatement, NULL);

		sqlite3_exec(io.chatsUniqueDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	}
	else {
		io.chatsUniqueDb = NULL;
		io.insertChatUniqueStatement = NULL;
		io.updateChatUniqueCountStatement = NULL;
	}


	if (opts.entityDataToDb) {
		while ((sqlResult = sqlite3_open("entityDataDb.db", &io.entityDataDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.entityDataDb, "main"))) {
			std::cerr << DPrintFLocation << ":" << "error opening entityDataDb.db for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.entityDataDb) << ". Trying again in 1000ms." << "\n";
			sqlite3_close(io.entityDataDb);
			io.entityDataDb = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}


#ifdef DEBUGSTATSDB
	while ((sqlResult = sqlite3_open("debugStatsDb.db", &io.debugStatsDb)) != SQLITE_OK || (readonlyResult = sqlite3_db_readonly(io.debugStatsDb, "main"))) {
		std::cerr << DPrintFLocation << ":" << "error opening debugStatsDb.db for read/write (" << sqlResult << "," << readonlyResult << "): " << sqlite3_errmsg(io.debugStatsDb) << ". Trying again in 1000ms." << "\n";
		sqlite3_close(io.debugStatsDb);
		io.debugStatsDb = NULL;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	sqlite3_exec(io.debugStatsDb, "CREATE TABLE animStances ("
		"demoVersion INTEGER NOT NULL,"
		"saberHolstered BOOLEAN NOT NULL,"
		"torsoAnim INTEGER NOT NULL,"
		"legsAnim INTEGER NOT NULL,"
		"saberMove INTEGER NOT NULL,"
		"stance INTEGER NOT NULL,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance)"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(io.debugStatsDb, "CREATE TABLE frameInfoViewModelAnims ("
		"demoVersion INTEGER NOT NULL,"
		"type TEXT NOT NULL," // player or weapon
		"viewModelAnim INTEGER NOT NULL,"
		"frameInfo0Index INTEGER NOT NULL,"
		"frameInfo1Index INTEGER NOT NULL,"
		"frameInfo2Index INTEGER NOT NULL,"
		"frameInfo3Index INTEGER NOT NULL,"
		"frameInfo4Index INTEGER NOT NULL,"
		"frameInfo5Index INTEGER NOT NULL,"
		"frameInfo6Index INTEGER NOT NULL,"
		"frameInfo7Index INTEGER NOT NULL,"
		"frameInfo8Index INTEGER NOT NULL,"
		"frameInfo9Index INTEGER NOT NULL,"
		"frameInfo10Index INTEGER NOT NULL,"
		"frameInfo11Index INTEGER NOT NULL,"
		"frameInfo12Index INTEGER NOT NULL,"
		"frameInfo13Index INTEGER NOT NULL,"
		"frameInfo14Index INTEGER NOT NULL,"
		"frameInfo15Index INTEGER NOT NULL,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index)"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(io.debugStatsDb, "CREATE TABLE frameInfoViewModelAnimsSimple ("
		"demoVersion INTEGER NOT NULL,"
		"type TEXT NOT NULL," // player or weapon
		"viewModelAnim INTEGER NOT NULL,"
		"frameInfoIndex INTEGER NOT NULL,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(demoVersion,type,viewModelAnim,frameInfoIndex)"
		"); ",
		NULL, NULL, NULL);
	preparedStatementText = "INSERT OR IGNORE INTO animStances (demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance,countFound) VALUES (@demoVersion,@saberHolstered,@torsoAnim,@legsAnim,@saberMove,@stance, 0);";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertAnimStanceStatement, NULL);

	preparedStatementText = "UPDATE animStances SET countFound = countFound + @countFound WHERE demoVersion=@demoVersion AND saberHolstered=@saberHolstered AND torsoAnim=@torsoAnim AND @legsAnim=@legsAnim AND saberMove=@saberMove AND stance=@stance;";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.updateAnimStanceCountStatement, NULL);

	preparedStatementText = "INSERT OR IGNORE INTO frameInfoViewModelAnims (demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index,countFound) VALUES (@demoVersion,@type,@viewModelAnim,@frameInfo0Index,@frameInfo1Index,@frameInfo2Index,@frameInfo3Index,@frameInfo4Index,@frameInfo5Index,@frameInfo6Index,@frameInfo7Index,@frameInfo8Index,@frameInfo9Index,@frameInfo10Index,@frameInfo11Index,@frameInfo12Index,@frameInfo13Index,@frameInfo14Index,@frameInfo15Index,0);";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertFrameInfoViewModelAnimStatement, NULL);
	
	preparedStatementText = "UPDATE frameInfoViewModelAnims SET countFound = countFound + @countFound WHERE demoVersion=@demoVersion AND type=@type AND viewModelAnim=@viewModelAnim AND frameInfo0Index=@frameInfo0Index AND frameInfo1Index=@frameInfo1Index AND frameInfo2Index=@frameInfo2Index AND frameInfo3Index=@frameInfo3Index AND frameInfo4Index=@frameInfo4Index AND frameInfo5Index=@frameInfo5Index AND frameInfo6Index=@frameInfo6Index AND frameInfo7Index=@frameInfo7Index AND frameInfo8Index=@frameInfo8Index AND frameInfo9Index=@frameInfo9Index AND frameInfo10Index=@frameInfo10Index AND frameInfo11Index=@frameInfo11Index AND frameInfo12Index=@frameInfo12Index AND frameInfo13Index=@frameInfo13Index AND frameInfo14Index=@frameInfo14Index AND frameInfo15Index=@frameInfo15Index;";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.updateFrameInfoViewModelAnimCountStatement, NULL);

	preparedStatementText = "INSERT OR IGNORE INTO frameInfoViewModelAnimsSimple (demoVersion,type,viewModelAnim,frameInfoIndex,countFound) VALUES (@demoVersion,@type,@viewModelAnim,@frameInfoIndex,0);";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.insertFrameInfoViewModelAnimStatementSimple, NULL);
	
	preparedStatementText = "UPDATE frameInfoViewModelAnimsSimple SET countFound = countFound + @countFound WHERE demoVersion=@demoVersion AND type=@type AND viewModelAnim=@viewModelAnim AND frameInfoIndex=@frameInfoIndex;";
	sqlite3_prepare_v2(io.debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &io.updateFrameInfoViewModelAnimCountStatementSimple, NULL);

	sqlite3_exec(io.debugStatsDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
#endif
}

void handleKillDbPreQueries(SQLDelayedQuery* mainQuery, ioHandlesKillDb_t* killDbHandle, sqlite3_stmt* mainStatement) {
	// entrymeta
	mainQuery->bind(killDbHandle->insertEntryMetaStatement);
	int queryResult = sqlite3_step(killDbHandle->insertEntryMetaStatement);
	if (queryResult != SQLITE_ROW) {
		std::cerr << "Error getting entry meta id from database: " << queryResult << ":" << sqlite3_errmsg(killDbHandle->killDb) << "(" << DPrintFLocation << ")" << "\n";
	}
	else {
		int64_t id = sqlite3_column_int64(killDbHandle->insertEntryMetaStatement, 0);
		SQLDelayedValue val("@entryMeta", id);
		val.bind(mainStatement);
	}
	queryResult = sqlite3_step(killDbHandle->insertEntryMetaStatement);
	if (queryResult != SQLITE_DONE) {
		std::cerr << "Error inserting entry meta into database: " << queryResult << ":" << sqlite3_errmsg(killDbHandle->killDb) << "(" << DPrintFLocation << ")" << "\n";
	}
	sqlite3_reset(killDbHandle->insertEntryMetaStatement);
}
void handleKillDbPlayerPreQuery(const char* nameField,int fieldFlag, int fieldFlagStripped,SQLDelayedQuery* mainQuery, ioHandlesKillDb_t* killDbHandle, sqlite3_stmt* mainStatement) {

	SQLDelayedValue* valRaw = mainQuery->getValueByFlag(fieldFlag);
	SQLDelayedValue* valStrip = mainQuery->getValueByFlag(fieldFlagStripped);

	if (valRaw->getType() == SQLVALUE_TYPE_NULL || valStrip->getType() == SQLVALUE_TYPE_NULL) {

		SQLDelayedValue val(nameField, SQLDelayedValue_NULL);
		val.bind(mainStatement);
	}
	else {
		valRaw->bindOverrideName(killDbHandle->insertPlayerNameStatement, "@playerName");
		valStrip->bindOverrideName(killDbHandle->insertPlayerNameStatement, "@playerNameStripped");

		int queryResult = sqlite3_step(killDbHandle->insertPlayerNameStatement);
		if (queryResult != SQLITE_ROW) {
			std::cerr << "Error getting player id from database: " << queryResult << ":" << sqlite3_errmsg(killDbHandle->killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			int64_t id = sqlite3_column_int64(killDbHandle->insertPlayerNameStatement, 0);
			SQLDelayedValue val(nameField, id);
			val.bind(mainStatement);
		}
		queryResult = sqlite3_step(killDbHandle->insertPlayerNameStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting player name into database: " << queryResult << ":" << sqlite3_errmsg(killDbHandle->killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(killDbHandle->insertPlayerNameStatement);
	}
	
}

void executeAllQueries(ioHandles_t& io, const ExtraSearchOptions& opts) {

	// Animstances
#ifdef DEBUGSTATSDB
	// anim stance stuff
	for (auto it = io.animStanceQueries->begin(); it != io.animStanceQueries->end(); it++) {
		//wasDoingSQLiteExecution = true;
		(*it)->query.bind(io.insertAnimStanceStatement);
		int queryResult = sqlite3_step(io.insertAnimStanceStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting anim stance into database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.insertAnimStanceStatement);

		(*it)->query.bind(io.updateAnimStanceCountStatement);
		queryResult = sqlite3_step(io.updateAnimStanceCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error updating anim stance count in database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.updateAnimStanceCountStatement);
		//wasDoingSQLiteExecution = false;
	}
	// frame info viewmodelindex info
	for (auto it = io.frameInfoVieWModelQueries->begin(); it != io.frameInfoVieWModelQueries->end(); it++) {
		//wasDoingSQLiteExecution = true;
		(*it)->query.bind(io.insertFrameInfoViewModelAnimStatement);
		int queryResult = sqlite3_step(io.insertFrameInfoViewModelAnimStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting frameInfo-viewmodelanim info into database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.insertFrameInfoViewModelAnimStatement);

		(*it)->query.bind(io.updateFrameInfoViewModelAnimCountStatement);
		queryResult = sqlite3_step(io.updateFrameInfoViewModelAnimCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error updating frameInfo-viewmodelanim info count in database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.updateFrameInfoViewModelAnimCountStatement);
		//wasDoingSQLiteExecution = false;
	}
	// frame info viewmodelindex info
	for (auto it = io.frameInfoVieWModelSimpleQueries->begin(); it != io.frameInfoVieWModelSimpleQueries->end(); it++) {
		//wasDoingSQLiteExecution = true;
		(*it)->query.bind(io.insertFrameInfoViewModelAnimStatementSimple);
		int queryResult = sqlite3_step(io.insertFrameInfoViewModelAnimStatementSimple);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting frameInfo-viewmodelanim (simple) info into database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.insertFrameInfoViewModelAnimStatementSimple);

		(*it)->query.bind(io.updateFrameInfoViewModelAnimCountStatementSimple);
		queryResult = sqlite3_step(io.updateFrameInfoViewModelAnimCountStatementSimple);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error updating frameInfo-viewmodelanim info (simple) count in database: " << queryResult << ":" << sqlite3_errmsg(io.debugStatsDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.updateFrameInfoViewModelAnimCountStatementSimple);
		//wasDoingSQLiteExecution = false;
	}
#endif

	// Kills
	for (auto it = io.killQueries->begin(); it != io.killQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertStatement);
		handleKillDbPlayerPreQuery("@killerId",QF_PLAYERNAME0,QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertStatement);
		handleKillDbPlayerPreQuery("@victimId",QF_PLAYERNAME1,QF_PLAYERNAME1STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertStatement);
		if (queryResult != SQLITE_DONE) {
#ifndef DEBUG
			if (queryResult != SQLITE_CONSTRAINT)
#endif
				std::cerr << "Error inserting kill into database: " << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << " (" << queryResult << ")" << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertStatement);
		//wasDoingSQLiteExecution = false;
	}

	// Kill angles
	for (auto it = io.killAngleQueries->begin(); it != io.killAngleQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertAngleStatement);
		handleKillDbPreQueries(&(*it)->query,&io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertAngleStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertAngleStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting kill angle into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertAngleStatement);
		//wasDoingSQLiteExecution = false;
		if ((*it)->batchString1.size() || (*it)->batchString2.size())  (*io.outputBatHandle) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Sprees
	for (auto it = io.spreeQueries->begin(); it != io.spreeQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertSpreeStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertSpreeStatement);
		handleKillDbPlayerPreQuery("@killerId", QF_PLAYERNAME0, QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertSpreeStatement);

		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertSpreeStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting killing spree into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertSpreeStatement);
		//wasDoingSQLiteExecution = false;
		if ((*it)->batchString1.size() || (*it)->batchString2.size())  (*io.outputBatHandleKillSprees) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Captures
	for (auto it = io.captureQueries->begin(); it != io.captureQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertCaptureStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertCaptureStatement);
		handleKillDbPlayerPreQuery("@capperId", QF_PLAYERNAME0, QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertCaptureStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertCaptureStatement);
		uint64_t insertedId = -1;
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting capture into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
			if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
				std::cerr << "Error retrieving inserted capture id from database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			else {
				insertedId = sqlite3_column_int64(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement, 0);
			}
			sqlite3_reset(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertCaptureStatement);
		//wasDoingSQLiteExecution = false;

		if((*it)->batchString1.size() || (*it)->batchString1.size()) (*io.outputBatHandleCaptures) << "\nrem insertid" << insertedId;
		if ((*it)->batchString1.size() || (*it)->batchString2.size())  (*io.outputBatHandleCaptures) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// FlagGrabs
	for (auto it = io.flagGrabQueries->begin(); it != io.flagGrabQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		handleKillDbPlayerPreQuery("@grabberId", QF_PLAYERNAME0, QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		handleKillDbPlayerPreQuery("@capperId", QF_PLAYERNAME1, QF_PLAYERNAME1STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		uint64_t insertedId = -1;
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting flag grab into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
			if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
				std::cerr << "Error retrieving inserted flaggrab id from database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			else {
				insertedId = sqlite3_column_int64(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement, 0);
			}
			sqlite3_reset(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertFlagGrabStatement);
		//wasDoingSQLiteExecution = false;

		if((*it)->batchString1.size() || (*it)->batchString1.size()) (*io.outputBatHandleFlagGrabs) << "\nrem insertid" << insertedId;
		if ((*it)->batchString1.size() || (*it)->batchString2.size())  (*io.outputBatHandleFlagGrabs) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Defrag
	for (auto it = io.defragQueries->begin(); it != io.defragQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertDefragRunStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertDefragRunStatement);
		handleKillDbPlayerPreQuery("@playerId", QF_PLAYERNAME0, QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertDefragRunStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertDefragRunStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting defrag run into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertDefragRunStatement);
		//wasDoingSQLiteExecution = false;
		if((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleDefrag) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Laughs
	for (auto it = io.laughQueries->begin(); it != io.laughQueries->end(); it++) {
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertLaughsStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertLaughsStatement);

		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertLaughsStatement);
		uint64_t insertedId = -1;
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting laugh spree into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
			if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
				std::cerr << "Error retrieving inserted laughs id from database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			else {
				insertedId = sqlite3_column_int64(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement, 0);
			}
			sqlite3_reset(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertLaughsStatement);
		//wasDoingSQLiteExecution = false;
				
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleLaughs) << "\nrem insertid" << insertedId;
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleLaughs) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Special
	for (auto it = io.specialQueries->begin(); it != io.specialQueries->end(); it++) {
		// TODO put in database
		(*it)->query.bind(io.killDb[(*it)->databaseIndex].insertSpecialStatement);
		handleKillDbPreQueries(&(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertSpecialStatement);
		handleKillDbPlayerPreQuery("@playerId", QF_PLAYERNAME0, QF_PLAYERNAME0STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertSpecialStatement);
		handleKillDbPlayerPreQuery("@playerIdAlt", QF_PLAYERNAME1, QF_PLAYERNAME1STRIPPED, &(*it)->query, &io.killDb[(*it)->databaseIndex], io.killDb[(*it)->databaseIndex].insertSpecialStatement);

		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].insertSpecialStatement);
		uint64_t insertedId = -1;
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting special into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			queryResult = sqlite3_step(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
			if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
				std::cerr << "Error retrieving inserted specials id from database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			else {
				insertedId = sqlite3_column_int64(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement, 0);
			}
			sqlite3_reset(io.killDb[(*it)->databaseIndex].selectLastInsertRowIdStatement);
		}
		sqlite3_reset(io.killDb[(*it)->databaseIndex].insertSpecialStatement);
		//wasDoingSQLiteExecution = false;
				
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleLaughs) << "\nrem insertid" << insertedId;
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleSpecial) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}
	// Marks
	for (auto it = io.markQueries->begin(); it != io.markQueries->end(); it++) {
		/* // TODO put in database
		(*it)->query.bind(io.insertLaughsStatement);

		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.insertLaughsStatement);
		uint64_t insertedId = -1;
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting laugh spree into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		else {
			queryResult = sqlite3_step(io.selectLastInsertRowIdStatement);
			if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
				std::cerr << "Error retrieving inserted laughs id from database: " << queryResult << ":" << sqlite3_errmsg(io.killDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			else {
				insertedId = sqlite3_column_int64(io.selectLastInsertRowIdStatement, 0);
			}
			sqlite3_reset(io.selectLastInsertRowIdStatement);
		}
		sqlite3_reset(io.insertLaughsStatement);
		//wasDoingSQLiteExecution = false;
				
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleLaughs) << "\nrem insertid" << insertedId;*/
		if ((*it)->batchString1.size() || (*it)->batchString2.size()) (*io.outputBatHandleMarks) << (*it)->batchPrefix << (*it)->batchString1 << (*it)->batchMiddlePart.str() << (*it)->batchString2 << (*it)->batchSuffix << "\n";
	}

	// Player demo stats
	if (opts.doStatsDb) {
		for (auto it = io.playerDemoStatsQueries->begin(); it != io.playerDemoStatsQueries->end(); it++) {
			(*it)->query.bind(io.insertPlayerDemoStatsStatement);

			//wasDoingSQLiteExecution = true;
			int queryResult = sqlite3_step(io.insertPlayerDemoStatsStatement);
			if (queryResult != SQLITE_DONE) {
				std::cerr << "Error inserting player demo stats into database: " << queryResult << ":" << sqlite3_errmsg(io.statsDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			sqlite3_reset(io.insertPlayerDemoStatsStatement);
			//wasDoingSQLiteExecution = false;
		}
	}

	// Packet stats
	if (opts.writeDemoPacketStats) {
		for (auto it = io.packetStatsQueries->begin(); it != io.packetStatsQueries->end(); it++) {
			(*it)->query.bind(io.insertPacketStatsStatement);
			//wasDoingSQLiteExecution = true;
			int queryResult = sqlite3_step(io.insertPacketStatsStatement);
			if (queryResult != SQLITE_DONE) {
				std::cerr << "Error inserting packet stats into database: " << queryResult << ":" << sqlite3_errmsg(io.demoStatsDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			sqlite3_reset(io.insertPacketStatsStatement);
			//wasDoingSQLiteExecution = false;
		}
	}

	// Chats
	if (opts.writeChatsUnique) {
		for (auto it = io.chatsUniqueQueries->begin(); it != io.chatsUniqueQueries->end(); it++) {
			(*it)->query.bind(io.insertChatUniqueStatement);
			(*it)->query.bind(io.updateChatUniqueCountStatement);
			//wasDoingSQLiteExecution = true;
			int queryResult = sqlite3_step(io.insertChatUniqueStatement);
			if (queryResult != SQLITE_DONE) {
				std::cerr << "Error inserting unique chat into database: " << queryResult << ":" << sqlite3_errmsg(io.chatsUniqueDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			sqlite3_reset(io.insertChatUniqueStatement);
			queryResult = sqlite3_step(io.updateChatUniqueCountStatement);
			if (queryResult != SQLITE_DONE) {
				std::cerr << "Error updating unique chat count in database: " << queryResult << ":" << sqlite3_errmsg(io.chatsUniqueDb) << "(" << DPrintFLocation << ")" << "\n";
			}
			sqlite3_reset(io.updateChatUniqueCountStatement);
			//wasDoingSQLiteExecution = false;
		}
	}

	// Player model
	for (auto it = io.playerModelQueries->begin(); it != io.playerModelQueries->end(); it++) {
		(*it)->query.bind(io.insertPlayerModelStatement);
		(*it)->query.bind(io.updatePlayerModelCountStatement);
		//wasDoingSQLiteExecution = true;
		int queryResult = sqlite3_step(io.insertPlayerModelStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting player model into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.insertPlayerModelStatement);
		queryResult = sqlite3_step(io.updatePlayerModelCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error updating player model count in database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[(*it)->databaseIndex].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.updatePlayerModelCountStatement);
		//wasDoingSQLiteExecution = false;
	}



	if (opts.entityDataToDb) {
		// ? Not yet implemented anyway.
	}




	// IMPORTANT NOTE:
	// I work a lot with pointers here.
	// It would be appropriate to clean them up again under normal circumstances.
	// However, this is pretty much the end of the program already.
	// Once the program exits, all allocated memory will be automatically freed anyway.
	// Why waste cycles cleaning up "leaked" memory when it will be cleaned by the operating system in a few moments anyway?
	// However if this were ever "library-ized" to make it possible to run repeatedly from the same executable,
	// all this delayed query stuff needs to be cleaned up.
	// Concretely, the vectors holding the SQLDelayedQueryWrapper_t items (aka queryCollection) need to have delete called on them.
	// This will call the destructor of the vectors which will also call the destructor of the SQLDelayedQuery items which in turn calls the destructors
	// of the SQLDelayedValue items in it, and so on. That much is already prepared.
	// But like said above, why waste cycles calling destructors etc. All is going to be cleared up after program exit anyway. This tool should be FAST!
	// (yeah I realize it's probably not a huge performance impact but there is literally zero benefit outside of elegance/cleanliness)
}

static void inline writeStrafeCSV(int i, const ExtraSearchOptions& opts);
static void inline writePlayerDumpCSV(int i, const ExtraSearchOptions& opts);
static void inline writeTeleportRelatedStuff(const ExtraSearchOptions& opts);


inline size_t streamsize(std::ostream* stream) {
	size_t oldPointer = stream->tellp();
	stream->seekp(0, std::ios::end);
	size_t end = stream->tellp();
	stream->seekp(oldPointer, std::ios::beg);
	return end;
}

template<unsigned int max_clients>
qboolean demoHighlightFindExceptWrapper(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, const char* outputBatFileKillSprees, const char* outputBatFileDefrag, const char* outputBatFileCaptures, const char* outputBatFileFlagGrabs, const char* outputBatFileLaughs, const char* outputBatFileSpecial, const char* outputBatFileMarks, const highlightSearchMode_t searchMode, const ExtraSearchOptions& opts) {

	ioHandles_t io;

	



	io.markQueries = new queryCollection();
	io.specialQueries = new queryCollection();
	io.laughQueries = new queryCollection();
	io.defragQueries = new queryCollection();
	io.captureQueries = new queryCollection();
	io.flagGrabQueries = new queryCollection();
	io.spreeQueries = new queryCollection();
	io.killQueries = new queryCollection();
	io.killAngleQueries = new queryCollection();
	io.playerModelQueries = new queryCollection();
	io.playerDemoStatsQueries = new queryCollection();
	io.animStanceQueries = new queryCollection();
	io.frameInfoVieWModelQueries = new queryCollection();
	io.frameInfoVieWModelSimpleQueries = new queryCollection();
	io.packetStatsQueries = new queryCollection();
	io.chatsUniqueQueries = new queryCollection();



	bool SEHExceptionCaught = false;
	bool wasDoingSQLiteExecution = false;
	bool doFinalizeSQLite = true;

	// turn oldpath absolute
	//std::filesystem::path tmpFSPath = sourceDemoFile;
	//std::string oldPathFullOutside = std::filesystem::absolute(tmpFSPath).string();

	sharedVariables_t sharedVars;

	int64_t			demoCurrentTime = 0;

	qboolean success = demoHighlightFindExceptWrapper2<max_clients>(sourceDemoFile, bufferTime, searchMode, opts, SEHExceptionCaught, demoCurrentTime, wasDoingSQLiteExecution, io, sharedVars);

	for (int i = 0; i < max_clients; i++) {
		for (int t = 0; t < METRACKER_TOTAL_COUNT; t++) {
			if (metaEventTrackers[t][i]) {
				delete metaEventTrackers[t][i]; // This flushes meta info to database query and .bat
			}
		}
	}

	if (opts.makeVideo && videoFrames.size()) {
		saveVideo(opts);
	}

	if (opts.testOnly) return qtrue;

	// Acquire the mutex for saving stuff here
	nes::named_mutex mutex{ "DemoHighlightFinder_postAnalysis_dataSaving" };
	std::unique_lock lock{ mutex };

	openAndSetupDb(io,opts);

	qboolean successStatisticsSave = qfalse;
	if (!SEHExceptionCaught || !wasDoingSQLiteExecution) { // this... kinda old with how it was previously. can prolly simplify. todo.
		if (opts.doStatsDb) {
			successStatisticsSave = saveStatisticsToDb(io, wasDoingSQLiteExecution, sharedVars, SEHExceptionCaught); // This will only return false if there's an exception.
		}
		else {
			successStatisticsSave = qtrue;
		}
	}



#ifdef BUFFERBAT
	std::stringstream outputBatSS;
	std::stringstream outputBatSSKillSprees;
	std::stringstream outputBatSSDefrag;
	std::stringstream outputBatSSCaptures;
	std::stringstream outputBatSSFlagGrabs;
	std::stringstream outputBatSSLaughs;
	std::stringstream outputBatSSSpecial;
	std::stringstream outputBatSSMarks;

	io.outputBatHandle = &outputBatSS;
	io.outputBatHandleKillSprees = &outputBatSSKillSprees;
	io.outputBatHandleDefrag = &outputBatSSDefrag;
	io.outputBatHandleCaptures = &outputBatSSCaptures;
	io.outputBatHandleFlagGrabs = &outputBatSSFlagGrabs;
	io.outputBatHandleLaughs = &outputBatSSLaughs;
	io.outputBatHandleSpecial = &outputBatSSSpecial;
	io.outputBatHandleMarks = &outputBatSSMarks;
#else
	std::ofstream outputBatHandle;
	std::ofstream outputBatHandleKillSprees;
	std::ofstream outputBatHandleDefrag;
	std::ofstream outputBatHandleCaptures;
	std::ofstream outputBatHandleLaughs;
	std::ofstream outputBatHandleSpecial;
	std::ofstream outputBatHandleMarks;

	io.outputBatHandle = &outputBatHandle;
	io.outputBatHandleKillSprees = &outputBatHandleKillSprees;
	io.outputBatHandleDefrag = &outputBatHandleDefrag;
	io.outputBatHandleCaptures = &outputBatHandleCaptures;
	io.outputBatHandleLaughs = &outputBatHandleLaughs;
	io.outputBatHandleSpecial = &outputBatHandleSpecial;
	io.outputBatHandleMarks = &outputBatHandleMarks;

	io.outputBatHandle->open(outputBatFile, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleKillSprees->open(outputBatFileKillSprees, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleDefrag->open(outputBatFileDefrag, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleCaptures->open(outputBatFileCaptures, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleLaughs->open(outputBatFileLaughs, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleSpecial->open(outputBatFileSpecial, std::ios_base::app); // append instead of overwrite
	io.outputBatHandleMarks->open(outputBatFileMarks, std::ios_base::app); // append instead of overwrite
#endif


	if (SEHExceptionCaught || !successStatisticsSave) { // this was after executeAllQueries(io, opts); originally. dunno why? but i want it to add to demoErrors in demoMeta
		// This demo errored. Remember it.
		// We are catching access violations and such.
		//
		SEHExceptionCaught = true; // Actually it can be normal exception too but whatever.
		if (wasDoingSQLiteExecution) { // If there was access violation while doing sqlite writess, maybe its not safe to wanna preserve the data.  Just ditch it.
			doFinalizeSQLite = false;
		}

		std::ofstream exceptionOutputHandle;

		exceptionOutputHandle.open("highlightExtractionExceptions.log", std::ios_base::app); // append instead of overwrite
		demoErrors << "SEH: " << demoCurrentTime << ";" << wasDoingSQLiteExecution << ";" << errorInfo << "\n";
		demoErrorFlags |= DERR_SEH;
		exceptionOutputHandle << sharedVars.oldPath << ";" << demoCurrentTime << ";" << wasDoingSQLiteExecution << ";" << errorInfo << "\n";
		std::cout << "\n\n\n\n\nHORRIBLE EXCEPTION! Demo caused something really bad to happen. (I kid, you'll live but maybe send me the demo to investigate). Details: \n\n" << sharedVars.oldPath << ";" << demoCurrentTime << ";" << wasDoingSQLiteExecution << "\n\n" << errorInfo << "\n\n\n\n\n";
		exceptionOutputHandle.close();
	}

	// Save demo stats
	for (int i = 0; i < opts.killDbsCount; i++) {
		const char* demoNameCStr = sharedVars.oldBasename.c_str();
		SQLBIND_NONDELAYED_TEXT(io.killDb[i].insertDemoMeta, "@demoName", demoNameCStr);
		const char* demoPathCStr = sharedVars.oldPath.c_str();
		SQLBIND_NONDELAYED_TEXT(io.killDb[i].insertDemoMeta, "@demoPath", demoPathCStr);
		SQLBIND_NONDELAYED(io.killDb[i].insertDemoMeta, int64, "@fileSize", sharedVars.demoFilesize);
		SQLBIND_NONDELAYED(io.killDb[i].insertDemoMeta, int64, "@demoTimeDuration", demoCurrentTime);
		SQLBIND_NONDELAYED(io.killDb[i].insertDemoMeta, int64, "@demoDateTime", sharedVars.oldDemoDateModified);
		SQLBIND_NONDELAYED(io.killDb[i].insertDemoMeta, int, "@demoErrorFlags", demoErrorFlags);
		std::string demoErrorsString = demoErrors.str();
		SQLBIND_NONDELAYED_TEXT(io.killDb[i].insertDemoMeta, "@demoErrors", demoErrorsString.c_str());
		SQLBIND_NONDELAYED(io.killDb[i].insertDemoMeta, int, "@demoDerivativeFlags", demoDerivativeFlags);

		std::stringstream recorderNames;
		std::stringstream recorderNamesStripped;
		int j = 0;
		for (auto it = recorderPlayerNames.begin(); it != recorderPlayerNames.end(); it++, j++) {
			if (j > 0) {
				recorderNames << ", ";
				recorderNamesStripped << ", ";
			}
			recorderNames << *it;
			recorderNamesStripped << Q_StripColorAll(*it);
		}
		std::string recorderNamesString = recorderNames.str();
		std::string recorderNamesStrippedString = recorderNamesStripped.str();
		const char* recorderNamesStringCStr = recorderNamesString.c_str();
		const char* recorderNamesStrippedStringCStr = recorderNamesStrippedString.c_str();
		SQLBIND_NONDELAYED_TEXT(io.killDb[i].insertDemoMeta, "@demoRecorderNames", recorderNamesStringCStr);
		SQLBIND_NONDELAYED_TEXT(io.killDb[i].insertDemoMeta, "@demoRecorderNamesStripped", recorderNamesStrippedStringCStr);

		int queryResult = sqlite3_step(io.killDb[i].insertDemoMeta);
		if (queryResult != SQLITE_DONE) {
			std::cerr << "Error inserting demo meta into database: " << queryResult << ":" << sqlite3_errmsg(io.killDb[i].killDb) << "(" << DPrintFLocation << ")" << "\n";
		}
		sqlite3_reset(io.killDb[i].insertDemoMeta);
	}

	executeAllQueries(io, opts);

	if (doFinalizeSQLite) {
#ifdef DEBUGSTATSDB
		sqlite3_exec(io.debugStatsDb, "COMMIT;", NULL, NULL, NULL);

		sqlite3_finalize(io.insertAnimStanceStatement);
		sqlite3_finalize(io.updateAnimStanceCountStatement);
		sqlite3_finalize(io.insertFrameInfoViewModelAnimStatement);
		sqlite3_finalize(io.updateFrameInfoViewModelAnimCountStatement);
		sqlite3_finalize(io.insertFrameInfoViewModelAnimStatementSimple);
		sqlite3_finalize(io.updateFrameInfoViewModelAnimCountStatementSimple);
		sqlite3_close(io.debugStatsDb);
#endif

		for (int i = 0; i < opts.killDbsCount; i++) {
			sqlite3_exec(io.killDb[i].killDb, "COMMIT;", NULL, NULL, NULL);
			sqlite3_finalize(io.killDb[i].insertLaughsStatement);
			sqlite3_finalize(io.killDb[i].insertDefragRunStatement);
			sqlite3_finalize(io.killDb[i].insertCaptureStatement);
			sqlite3_finalize(io.killDb[i].insertSpreeStatement);
			sqlite3_finalize(io.killDb[i].insertStatement);
			sqlite3_finalize(io.killDb[i].insertAngleStatement);
			sqlite3_finalize(io.killDb[i].insertDemoDatabaseProperty);
			sqlite3_finalize(io.killDb[i].insertDemoMeta);
			sqlite3_finalize(io.killDb[i].selectLastInsertRowIdStatement);
			sqlite3_finalize(io.killDb[i].insertEntryMetaStatement);
			sqlite3_finalize(io.killDb[i].insertPlayerNameStatement);
			sqlite3_close(io.killDb[i].killDb);
		}

		if (opts.doStatsDb) {
			sqlite3_exec(io.statsDb, "COMMIT;", NULL, NULL, NULL);
			sqlite3_finalize(io.insertPlayerModelStatement);
			sqlite3_finalize(io.updatePlayerModelCountStatement);
			sqlite3_finalize(io.selectStatsLastInsertRowIdStatement);
			sqlite3_finalize(io.insertPlayerDemoStatsStatement);
			sqlite3_close(io.statsDb);
		}

		if (opts.writeDemoPacketStats) {
			sqlite3_exec(io.demoStatsDb, "COMMIT;", NULL, NULL, NULL);
			sqlite3_finalize(io.insertPacketStatsStatement);
			sqlite3_close(io.demoStatsDb);
		}
		if (opts.writeChatsUnique) {
			sqlite3_exec(io.chatsUniqueDb, "COMMIT;", NULL, NULL, NULL);
			sqlite3_finalize(io.insertChatUniqueStatement);
			sqlite3_finalize(io.updateChatUniqueCountStatement);
			sqlite3_close(io.chatsUniqueDb);
		}
		if (opts.entityDataToDb) {
			sqlite3_exec(io.entityDataDb, "COMMIT;", NULL, NULL, NULL);

			if(io.insertEntityDataStatement) sqlite3_finalize(io.insertEntityDataStatement); // TODO Remove the ifs here once all is implemented
			if(io.insertPlayerStateDataStatement) sqlite3_finalize(io.insertPlayerStateDataStatement);
			if(io.insertGameStateDataStatement) sqlite3_finalize(io.insertGameStateDataStatement);
			if(io.insertConfigStringDataStatement) sqlite3_finalize(io.insertConfigStringDataStatement);
			sqlite3_close(io.entityDataDb);
		}
	}

	// Dump strafeCSV if desired
	if (opts.strafeCSVSyncPoint) {
		for (int i = 0; i < max_clients; i++) {
			strafeCSVResetPlayer(i, opts);

			writeStrafeCSV(i, opts);
		}
	}

	if (opts.teleportAnalysis) {
		writeTeleportRelatedStuff(opts);
	}
	
	// Dump player CSV if desired
	if (opts.playerCSVDump) {
		for (int i = 0; i < max_clients; i++) {
			writePlayerDumpCSV(i, opts);
		}
	}
	if (opts.entityCSVDump) {
		for (int i = max_clients; i < MAX_GENTITIES; i++) {
			writeEntityDumpCSV(i, opts);
		}
	}
#ifdef BUFFERBAT

	if (streamsize(io.outputBatHandle)) {
		std::ofstream outputBatHandle;
		outputBatHandle.open(outputBatFile, std::ios_base::app); // append instead of overwrite
		outputBatHandle << io.outputBatHandle->rdbuf();
		outputBatHandle.close();
	}
	if (streamsize(io.outputBatHandleKillSprees)) {
		std::ofstream outputBatHandleKillSprees;
		outputBatHandleKillSprees.open(outputBatFileKillSprees, std::ios_base::app); // append instead of overwrite
		outputBatHandleKillSprees << io.outputBatHandleKillSprees->rdbuf();
		outputBatHandleKillSprees.close();
	}
	if (streamsize(io.outputBatHandleDefrag)) {
		std::ofstream outputBatHandleDefrag;
		outputBatHandleDefrag.open(outputBatFileDefrag, std::ios_base::app); // append instead of overwrite
		outputBatHandleDefrag << io.outputBatHandleDefrag->rdbuf();
		outputBatHandleDefrag.close();
	}
	if (streamsize(io.outputBatHandleCaptures)) {
		std::ofstream outputBatHandleCaptures;
		outputBatHandleCaptures.open(outputBatFileCaptures, std::ios_base::app); // append instead of overwrite
		outputBatHandleCaptures << io.outputBatHandleCaptures->rdbuf();
		outputBatHandleCaptures.close();
	}
	if (streamsize(io.outputBatHandleFlagGrabs)) {
		std::ofstream outputBatHandleFlagGrabs;
		outputBatHandleFlagGrabs.open(outputBatFileFlagGrabs, std::ios_base::app); // append instead of overwrite
		outputBatHandleFlagGrabs << io.outputBatHandleFlagGrabs->rdbuf();
		outputBatHandleFlagGrabs.close();
	}
	if (streamsize(io.outputBatHandleLaughs)) {
		std::ofstream outputBatHandleLaughs;
		outputBatHandleLaughs.open(outputBatFileLaughs, std::ios_base::app); // append instead of overwrite
		outputBatHandleLaughs << io.outputBatHandleLaughs->rdbuf();
		outputBatHandleLaughs.close();
	}
	if (streamsize(io.outputBatHandleSpecial)) {
		std::ofstream outputBatHandleSpecial;
		outputBatHandleSpecial.open(outputBatFileSpecial, std::ios_base::app); // append instead of overwrite
		outputBatHandleSpecial << io.outputBatHandleSpecial->rdbuf();
		outputBatHandleSpecial.close();
	}
	if (streamsize(io.outputBatHandleMarks)) {
		std::ofstream outputBatHandleMarks;
		outputBatHandleMarks.open(outputBatFileMarks, std::ios_base::app); // append instead of overwrite
		outputBatHandleMarks << io.outputBatHandleMarks->rdbuf();
		outputBatHandleMarks.close();
	}

#else
	io.outputBatHandle->close();
	io.outputBatHandleKillSprees->close();
	io.outputBatHandleDefrag->close();
	io.outputBatHandleCaptures->close();
	io.outputBatHandleLaughs->close();
	io.outputBatHandleSpecial->close();
#endif

	lock.unlock();
	
	return success;
}

static void inline writePlayerDumpCSV(int clientNum, const ExtraSearchOptions& opts) {
	if (playerDumpCSVDataPoints[clientNum].size() > 0) {

		std::ofstream strafeCSVPlayerOutputHandle;
		strafeCSVPlayerOutputHandle.open(va("playerDumpCSV_client%d.csv", clientNum), std::ios_base::app); // append instead of overwrite

		strafeCSVPlayerOutputHandle << "demoTime,commandTime,posX,posY,posZ,velX,velY,velZ,pitch,yaw,roll\n";

		int lastCommandTime = -9999999;
		int64_t countSkipped = 0;
		for (auto it = playerDumpCSVDataPoints[clientNum].begin(); it != playerDumpCSVDataPoints[clientNum].end(); it++) {
			if (opts.playerCSVDumpCommandTimeDupeSkip && lastCommandTime == it->commandTime) {
				countSkipped++;
				continue;
			}
			strafeCSVPlayerOutputHandle << it->demoTime << ","<< it->commandTime << ","<< it->position[0] << ","<< it->position[1] << ","<< it->position[2] << ","<< it->velocity[0] << ","<< it->velocity[1] << ","<< it->velocity[2] << ","<< it->viewangles[PITCH] << ","<< it->viewangles[YAW] << ","<< it->viewangles[ROLL] << "\n";
			lastCommandTime = it->commandTime;
		}
		if (countSkipped) {
			std::cout << "writePlayerDumpCSV: Skipped " << countSkipped << " frames with duplicate commandTime for player " << clientNum << ".\n";
		}

		strafeCSVPlayerOutputHandle.close();
	}
}

static void inline writeEntityDumpCSV(int entityNum, const ExtraSearchOptions& opts) {
	if (entityDumpCSVDataPoints[entityNum].size() > 0) {

		int oldEType = INT_MIN;
		bool streamOpen = false;
		int fileIndex = 0;
		const char* oldExtraInfo = NULL;
		const char* etypename;

		std::ofstream strafeCSVentityOutputHandle;
		std::filesystem::create_directory("entityCSVDumps");

		for (auto it = entityDumpCSVDataPoints[entityNum].begin(); it != entityDumpCSVDataPoints[entityNum].end(); it++) {

			if (!streamOpen || oldEType != it->eType || oldExtraInfo != it->extraInfo) {
				if (streamOpen) {
					strafeCSVentityOutputHandle.close();
				}
				etypename = it->eType >= 0 && it->eType < ET_EVENTS_GENERAL ? entityTypesGeneralNames[it->eType].name : (it->eType >= ET_EVENTS_GENERAL ? "ET_EVENTS_GENERAL" : "ET_WEIRD");
				strafeCSVentityOutputHandle.open(va("entityCSVDumps/entityDumpCSV_entity%d_%d_st%d_%s_%s.csv", entityNum, fileIndex++,it->serverTime, etypename,it->extraInfo ? it->extraInfo : ""), std::ios_base::app); // append instead of overwrite

				strafeCSVentityOutputHandle << "serverTime,pos.trType,pos.trTime,pos.trDuration,pos.trBase[0],pos.trBase[1],pos.trBase[2],pos.trDelta[0],pos.trDelta[1],pos.trDelta[2],apos.trType,apos.trTime,apos.trDuration,apos.trBase[0],apos.trBase[1],apos.trBase[2],apos.trDelta[0],apos.trDelta[1],apos.trDelta[2]\n";
				streamOpen = true;
				oldEType = it->eType;
				oldExtraInfo = it->extraInfo;
			}

			strafeCSVentityOutputHandle << it->serverTime << ","<< it->pos.trType << ","<< it->pos.trTime << ","<< it->pos.trDuration << ","<< it->pos.trBase[0] << ","<< it->pos.trBase[1] << ","<< it->pos.trBase[2] << ","<< it->pos.trDelta[0] << ","<< it->pos.trDelta[1] << ","<< it->pos.trDelta[2] << ","<< it->apos.trType << ","<< it->apos.trTime << ","<< it->apos.trDuration << ","<< it->apos.trBase[0] << ","<< it->apos.trBase[1] << ","<< it->apos.trBase[2] << ","<< it->apos.trDelta[0] << ","<< it->apos.trDelta[1] << ","<< it->apos.trDelta[2] << "\n";
		}

		if (streamOpen) {
			strafeCSVentityOutputHandle.close();
		}
	}
}

static void inline writeTeleportRelatedStuff(const ExtraSearchOptions& opts) {

	std::vector<std::pair<int64_t, int64_t>> fastForwardSegments;
	int64_t totalSkippedTime = 0;
	
	if (opts.teleportAnalysis == 1) {
		for (int i = 0; i < teleports.size(); i++) {
			for (int c = teleports.size() - 1; c > i; c--) {
				if ((teleports[c].demoTime - teleports[i].demoTime) < opts.teleportAnalysisMinTimeFastForward) break; // We're not gonna fast forward such short segments
				if (VectorDistance(teleports[i].position, teleports[c].position) < 10.0f) {
					fastForwardSegments.push_back({ teleports[i].demoTime,teleports[c].demoTime });
					totalSkippedTime += teleports[c].demoTime - teleports[i].demoTime;
					i = c + 1;
				}
			}
		}
	}
	else if (opts.teleportAnalysis == 2) {

		vec3_t distance;
		const int quickSkipDuration = 1000;
		int averageFrameDuration = (teleports.back().demoTime - teleports.front().demoTime) / teleports.size();
		int framesQuickSkip = quickSkipDuration / averageFrameDuration;
		for (int64_t i = 0; i < teleports.size(); i++) {
		continueteleportcomparesearch:
			for (int64_t c = teleports.size() - 1; c > i; c-= framesQuickSkip) { // we need to step over bigger parts as a rough search because otherwise its too damn slow

				if (VectorDistance(teleports[i].position, teleports[c].position) < 1000.0f) {
					// Might be nearby, do closer search
					for (int64_t d = std::min(c+framesQuickSkip,(int64_t)teleports.size() - 1); d >= c; d --) {

						if ((teleports[d].demoTime - teleports[i].demoTime) < opts.teleportAnalysisMinTimeFastForward) break; // We're not gonna fast forward such short segments
						// TODO Check for overlap instead of checking distance of 64
						VectorSubtract(teleports[i].position, teleports[d].position,distance);
						if (VectorLength2(distance) < opts.teleportAnalysisMaxDistanceHorizontal && abs(distance[2]) < opts.teleportAnalysisMaxDistanceVertical) {
							int64_t ffStart = teleports[i].demoTime+opts.teleportAnalysisBufferTimePast;
							int64_t ffEnd = teleports[d].demoTime- opts.teleportAnalysisBufferTimeFuture;
							if (fastForwardSegments.size()) {
								// If we are using a negative teleportAnalysisBufferTimePast, make sure we do not conflict with the ending of the last fast forward segment.
								ffStart = std::max(ffStart,fastForwardSegments.back().second);
							}
							if ((ffEnd-ffStart)<1000) {
								break;
							}
							fastForwardSegments.push_back({ ffStart,ffEnd });
							totalSkippedTime += ffEnd - ffStart;
							i = d + 1;
							//while (i > 0 && teleports[i-1].demoTime >= ffEnd) {
							//	i--; // Allow us to fast forward after the end of this segment. // Edit: actually don't, we end up skipping important parts that way for reasons.
							//}
							goto continueteleportcomparesearch;
						}
					}
				}
				/*if ((teleports[c].demoTime - teleports[i].demoTime) < 5000) break; // We're not gonna fast forward such short segments
				// TODO Check for overlap instead of checking distance of 64
				if (VectorDistance(teleports[i].position, teleports[c].position) < 64.0f) {
					fastForwardSegments.push_back({ teleports[i].demoTime,teleports[c].demoTime });
					totalSkippedTime += teleports[c].demoTime - teleports[i].demoTime;
					i = c + 1;
				}*/
			}
			if ((i % 100) == 0) {
				std::cout << "Teleport analysis mode 2 progress: " << (100.0f*(float)i/(float)teleports.size()) << "%\r";
			}
		}
	}
	std::cout << "\n";
	std::cout << fastForwardSegments.size() << " fast-forward segments found with total fast-forwarded time of " << totalSkippedTime << " milliseconds.\n";

	std::stringstream ss;
	// Capture
	ss << ("<capture>\n");
	ss << ("\t<start>0</start>\n");
	ss << ("\t<end>0</end>\n");
	ss << ("\t<speed>1.00</speed>\n");
	ss << ("\t<view>chase</view>\n");
	ss << ("\t<view>chase</view>\n");
	ss << ("</capture>\n");

	// Camera
	ss << ("<camera>\n");
	ss << ("\t<smoothPos>2</smoothPos>\n");
	ss << ("\t<smoothAngles>1</smoothAngles>\n");
	ss << ("\t<locked>0</locked>\n");
	ss << ("\t<target>-1</target>\n");
	ss << ("\t<flags>3</flags>\n");
	ss << ("</camera>\n");

	// Chase
	ss << ("<chase>\n");
	ss << ("\t<locked>0</locked>\n");
	ss << ("</chase>\n");

	// Line (timeline)
	ss << ("<line>\n");
	ss << ("\t<offset>0</offset>\n");
	ss << ("\t<speed>1.00</speed>\n");
	ss << ("\t<locked>1</locked>\n");

	ss << "\t<point>\n";
	ss << "\t\t<time>0</time>\n";
	ss << "\t\t<demotime>0</demotime>\n";
	ss << "\t</point>\n";

	int64_t currentDemoTime = 0;
	int64_t currentProjectTime = 0;
	int64_t lastProjectTime = 0;
	int64_t lastDemoTime = 0;
	for (int i = 0; i < fastForwardSegments.size(); i++) {

		currentProjectTime += fastForwardSegments[i].first - lastDemoTime;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].first << "</demotime>\n";
		ss << "\t</point>\n";

		currentProjectTime += 100;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].first+100 << "</demotime>\n";
		ss << "\t</point>\n";

		float speedupRatio = (fastForwardSegments[i].second - fastForwardSegments[i].first - 200.0f) / 800.0f; // the entire duration (minus 200 ms) must be passed in 800 ms.

		currentProjectTime += 100;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].first + 100 + (int64_t)(speedupRatio * 100.0f) << "</demotime>\n";
		ss << "\t</point>\n";


		currentProjectTime += 600;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].second-100-(int64_t)(100.0f*speedupRatio) << "</demotime>\n";
		ss << "\t</point>\n";

		currentProjectTime += 100;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].second-100 << "</demotime>\n";
		ss << "\t</point>\n";

		currentProjectTime += 100;
		ss << "\t<point>\n";
		ss << "\t\t<time>" << currentProjectTime << "</time>\n";
		ss << "\t\t<demotime>" << fastForwardSegments[i].second << "</demotime>\n";
		ss << "\t</point>\n";


		lastProjectTime = currentProjectTime;
		lastDemoTime = fastForwardSegments[i].second;
	}

	ss << ("</line>\n");

	// DOF
	ss << ("<dof>\n");
	ss << ("\t<locked>0</locked>\n");
	ss << ("\t<target>-1</target>\n");
	ss << ("</dof>\n");

	// Weather
	ss << ("<weather>\n");
	ss << ("\t<sun>\n");
	ss << ("\t\t<active>0</active>\n");
	ss << ("\t\t<size>1.0000</size>\n");
	ss << ("\t\t<precision>10.0000</precision>\n");
	ss << ("\t\t<yaw>45.0000</yaw>\n");
	ss << ("\t\t<pitch>45.0000</pitch>\n");
	ss << ("\t</sun>\n");
	ss << ("\t<rain>\n");
	ss << ("\t\t<active>0</active>\n");
	ss << ("\t\t<number>100</number>\n");
	ss << ("\t\t<range>1000.0000</range>\n");
	ss << ("\t\t<back>0</back>\n");
	ss << ("\t</rain>\n");
	ss << ("</weather>\n");

	std::ofstream mmeFastForwarderConfig;
	mmeFastForwarderConfig.open("fastforward.cfg", std::ios_base::app); // append instead of overwrite

	mmeFastForwarderConfig << ss.str();


	mmeFastForwarderConfig.close();
}

static void inline writeStrafeCSV(int clientNum, const ExtraSearchOptions& opts) {
	if (strafeCSVRuns[clientNum].size() > 0) {

		std::ofstream strafeCSVPlayerOutputHandle;
		strafeCSVPlayerOutputHandle.open(va("strafeCSV_client%d.csv", clientNum), std::ios_base::app); // append instead of overwrite


		// Find sync points for each run
		int64_t* syncPoints = new int64_t[strafeCSVRuns[clientNum].size()]{};
		float* syncPointsDistance = new float[strafeCSVRuns[clientNum].size()]{};

		int64_t earliestTime = 0;
		for (int runId = 0; runId < strafeCSVRuns[clientNum].size(); runId++) {
			syncPoints[runId] = 0;
			syncPointsDistance[runId] = 0;
			bool syncPointFound = false;
			for (int frame = 0; frame < strafeCSVRuns[clientNum][runId].size(); frame++) {
				syncPoints[runId] = strafeCSVRuns[clientNum][runId][frame].timeOffset;
				syncPointsDistance[runId] += strafeCSVRuns[clientNum][runId][frame].distanceTraveledFromLast;
				if (strafeCSVRuns[clientNum][runId][frame].timeOffset >= opts.strafeCSVSyncPoint) {
					syncPointFound = true;
					break;
				}
			}
			if (!syncPointFound) {
				syncPoints[runId] = -1;
			}
			else if (earliestTime > -syncPoints[runId]) {
				earliestTime = -syncPoints[runId];
			}
		}

		// Write header
		strafeCSVPlayerOutputHandle << "time";
		for (int r = 0; r < strafeCSVRuns[clientNum].size(); r++) {
			if (syncPoints[r] == -1) continue;
			strafeCSVPlayerOutputHandle << ",run" << r << "Dist";
		}
		for (int r = 0; r < strafeCSVRuns[clientNum].size(); r++) {
			if (syncPoints[r] == -1) continue;
			strafeCSVPlayerOutputHandle << ",run" << r << "Speed";
		}
		strafeCSVPlayerOutputHandle << "\n";


		// Write data
		int64_t currentTime = earliestTime;
		int* runFrameIndizi = new int[strafeCSVRuns[clientNum].size()]{};
		float* distancesTraveled = new float[strafeCSVRuns[clientNum].size()]{};
		while (true) {

			strafeCSVPlayerOutputHandle << currentTime;


			// Write distance travelled
			for (int runId = 0; runId < strafeCSVRuns[clientNum].size(); runId++) {
				if (syncPoints[runId] == -1) continue;

				bool entryWritten = false;
				while (runFrameIndizi[runId] < strafeCSVRuns[clientNum][runId].size()) {
					strafeCSVPoint_t* pointHere = &strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]];
					int64_t realTimehere = pointHere->timeOffset - syncPoints[runId];
					if (realTimehere < currentTime) {
						runFrameIndizi[runId]++;
						if (runFrameIndizi[runId] < strafeCSVRuns[clientNum][runId].size()) {
							distancesTraveled[runId] += strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]].distanceTraveledFromLast;
						}
					}
					else if (realTimehere == currentTime) {
						strafeCSVPlayerOutputHandle << "," << (distancesTraveled[runId] - syncPointsDistance[runId]);
						entryWritten = true;
						break;
					}
					else { // Point lies in the future
						if (opts.strafeCSVInterpolate && runFrameIndizi[runId] > 0) {
							strafeCSVPoint_t* lastPoint = &strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]-1];
							int64_t timeDiff = pointHere->timeOffset - lastPoint->timeOffset;
							int64_t timeInterpol = realTimehere - currentTime;
							float distanceToSubtract = pointHere->distanceTraveledFromLast * ((float)timeInterpol / (float)timeDiff); // TODO Worry about division by zero? Or is it safe? Prolly safe in this specific case.
							strafeCSVPlayerOutputHandle << "," << (distancesTraveled[runId] - syncPointsDistance[runId] - distanceToSubtract);
							entryWritten = true;
						}
						break;
					}
				}
				if (!entryWritten) {
					strafeCSVPlayerOutputHandle << ",";
				}
			}

			// Write current speed. 
			for (int runId = 0; runId < strafeCSVRuns[clientNum].size(); runId++) {
				if (syncPoints[runId] == -1) continue;

				bool entryWritten = false;
				if (runFrameIndizi[runId] < strafeCSVRuns[clientNum][runId].size()) {
					strafeCSVPoint_t* pointHere = &strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]];
					int64_t realTimehere = pointHere->timeOffset - syncPoints[runId];
					if (realTimehere == currentTime) {
						strafeCSVPlayerOutputHandle << "," << pointHere->velocityXY;
						entryWritten = true;
					}
					else if(realTimehere > currentTime) { // A simple else should do really, but let's be safe I guess
						if (opts.strafeCSVInterpolate && runFrameIndizi[runId] > 0) {
							strafeCSVPoint_t* lastPoint = &strafeCSVRuns[clientNum][runId][runFrameIndizi[runId] - 1];
							float speedDiff = pointHere->velocityXY - lastPoint->velocityXY;
							int64_t timeDiff = pointHere->timeOffset - lastPoint->timeOffset;
							int64_t timeInterpol = realTimehere - currentTime;
							float speedHere = pointHere->velocityXY - speedDiff * ((float)timeInterpol / (float)timeDiff); // TODO Worry about division by zero? Or is it safe? Prolly safe in this specific case.
							strafeCSVPlayerOutputHandle << "," << speedHere;
							entryWritten = true;
						}
					}
				}
				if (!entryWritten) {
					strafeCSVPlayerOutputHandle << ",";
				}
			}
			strafeCSVPlayerOutputHandle << "\n";

			bool nextBiggerTimeSet = false;
			int64_t nextBiggerTime = 0;
			// Find next time point.
			for (int runId = 0; runId < strafeCSVRuns[clientNum].size(); runId++) {
				if (syncPoints[runId] == -1) continue;

				while (runFrameIndizi[runId] < strafeCSVRuns[clientNum][runId].size()) {
					strafeCSVPoint_t* pointHere = &strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]];
					int64_t realTimehere = pointHere->timeOffset - syncPoints[runId];
					if (realTimehere <= currentTime) {
						runFrameIndizi[runId]++;
						if (runFrameIndizi[runId] < strafeCSVRuns[clientNum][runId].size()) {
							distancesTraveled[runId] += strafeCSVRuns[clientNum][runId][runFrameIndizi[runId]].distanceTraveledFromLast;
						}
					}
					else { // Point lies in the future
						if (realTimehere < nextBiggerTime || !nextBiggerTimeSet) {
							nextBiggerTime = realTimehere;
						}
						nextBiggerTimeSet = true;
						break;
					}
				}
			}

			if (nextBiggerTimeSet) {
				currentTime = nextBiggerTime;
			}
			else {
				break; // No data left to write
			}
			
		}


		delete[] syncPoints;
		delete[] syncPointsDistance;
		delete[] runFrameIndizi;
		delete[] distancesTraveled;

		strafeCSVPlayerOutputHandle.close();
	}
}

static void inline saveStatisticsToDbReal(const ioHandles_t& io,bool& wasDoingSQLiteExecution, const sharedVariables_t& sharedVars) {
	// Save player stats
	for (auto it = playerDemoStatsMap.begin(); it != playerDemoStatsMap.end(); it++) {

		if (it->second.everUsed) { // Some clients may have never been visible or not even existed at all (we have entries for all client nums by default but they're only set to "everUsed" if they were actually seen)

			SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
			SQLDelayedQuery* query = &queryWrapper->query;

			std::string mapname = std::get<0>(it->first);
			std::string playerName = std::get<1>(it->first);
			int clientNum = std::get<2>(it->first);
			double strafeDeviation = it->second.strafeDeviation.sum / it->second.strafeDeviation.divisor;
			double strafeDeviationNoSaberMove = it->second.strafeDeviationNoSaberMove.sum / it->second.strafeDeviationNoSaberMove.divisor;
			int64_t strafeSampleCount = it->second.strafeDeviation.divisor + 0.5;
			int64_t strafeNoSaberMoveSampleCount = it->second.strafeDeviationNoSaberMove.divisor + 0.5;
			double groundFrameQuality = it->second.groundFrameQuality.sum / it->second.groundFrameQuality.divisor;
			int64_t groundFrameSampleCount = it->second.groundFrameQuality.divisor + 0.5;
			SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
			SQLBIND_DELAYED_TEXT(query, "@playerName", playerName.c_str());
			std::string playernameStripped = Q_StripColorAll(playerName);
			SQLBIND_DELAYED_TEXT(query, "@playerNameStripped", playernameStripped.c_str());
			SQLBIND_DELAYED(query, int, "@clientNum", clientNum);
			SQLBIND_DELAYED(query, double, "@averageStrafeDeviation", strafeDeviation);
			SQLBIND_DELAYED(query, double, "@averageStrafeDeviationNoSaberMove", strafeDeviationNoSaberMove);

			std::stringstream ssStrafeJson;
			std::stringstream ssStrafeNoSaberMoveJson;
			ssStrafeJson << "[\n";
			ssStrafeNoSaberMoveJson << "[\n";
			for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
				double valueHere = it->second.strafeDeviationBuckets[b].sum / it->second.strafeDeviationBuckets[b].divisor;
				int64_t sampleCount = it->second.strafeDeviationBuckets[b].divisor + 0.5;
				if (b != 0) {
					ssStrafeJson << ",\n";
				}
				ssStrafeJson << "{\n";
				ssStrafeJson << "\"average\":";
				if (sampleCount) {
					ssStrafeJson << valueHere;
				}
				else {
					ssStrafeJson << "null";
				}
				ssStrafeJson << ",\n";
				ssStrafeJson << "\"sampleCount\":" << sampleCount << ",\n";
				ssStrafeJson << "\"bucketFromIncluding\":" << strafeAnalysisBuckets[b].fromIncluding << ",\n";
				ssStrafeJson << "\"bucketToExcluding\":" << strafeAnalysisBuckets[b].toExcluding << "\n";
				ssStrafeJson << "}\n";

				// No saber move data
				valueHere = it->second.strafeDeviationNoSaberMoveBuckets[b].sum / it->second.strafeDeviationNoSaberMoveBuckets[b].divisor;
				sampleCount = it->second.strafeDeviationNoSaberMoveBuckets[b].divisor + 0.5;
				if (b != 0) {
					ssStrafeNoSaberMoveJson << ",\n";
				}
				ssStrafeNoSaberMoveJson << "{\n";
				ssStrafeNoSaberMoveJson << "\"average\":";
				if (sampleCount) {
					ssStrafeNoSaberMoveJson << valueHere;
				}
				else {
					ssStrafeNoSaberMoveJson << "null";
				}
				ssStrafeNoSaberMoveJson << ",\n";
				ssStrafeNoSaberMoveJson << "\"sampleCount\":" << sampleCount << ",\n";
				ssStrafeNoSaberMoveJson << "\"bucketFromIncluding\":" << strafeAnalysisBuckets[b].fromIncluding << ",\n";
				ssStrafeNoSaberMoveJson << "\"bucketToExcluding\":" << strafeAnalysisBuckets[b].toExcluding << "\n";
				ssStrafeNoSaberMoveJson << "}\n";
			}
			ssStrafeJson << "]\n";
			ssStrafeNoSaberMoveJson << "]\n";
			std::string ssStrafeJsonString = ssStrafeJson.str();
			std::string ssStrafeNoSaberMoveJsonString = ssStrafeNoSaberMoveJson.str();
			SQLBIND_DELAYED_TEXT(query, "@averageStrafeDeviationBucketsJSON", ssStrafeJsonString.c_str());
			SQLBIND_DELAYED_TEXT(query, "@averageStrafeDeviationNoSaberMoveBucketsJSON", ssStrafeNoSaberMoveJsonString.c_str());


			SQLBIND_DELAYED(query, int, "@strafeSampleCount", strafeSampleCount);
			SQLBIND_DELAYED(query, int, "@strafeNoSaberMoveSampleCount", strafeNoSaberMoveSampleCount);

			SQLBIND_DELAYED(query, double, "@averageGroundFrameQuality", groundFrameQuality);
			SQLBIND_DELAYED(query, int, "@groundFrameSampleCount", groundFrameSampleCount);

			SQLBIND_DELAYED(query, int, "@hitBySaberCount", it->second.hitBySaberCount);
			SQLBIND_DELAYED(query, int, "@hitBySaberBlockableCount", it->second.hitBySaberBlockableCount);
			SQLBIND_DELAYED(query, int, "@parryCount", it->second.parryCount);
			SQLBIND_DELAYED(query, int, "@attackFromParryCount", it->second.attackFromParryCount);

			SQLBIND_DELAYED_TEXT(query, "@demoName", sharedVars.oldBasename.c_str());
			SQLBIND_DELAYED_TEXT(query, "@demoPath", sharedVars.oldPath.c_str());
			SQLBIND_DELAYED(query, int, "@demoDateTime", sharedVars.oldDemoDateModified);

			io.playerDemoStatsQueries->push_back(queryWrapper);
		}

	}

#ifdef DEBUGSTATSDB
	for (auto it = animStanceCounts.begin(); it != animStanceCounts.end(); it++) {

		SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
		SQLDelayedQuery* query = &queryWrapper->query;

		/*SQLBIND(io.insertAnimStanceStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(io.insertAnimStanceStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(io.insertAnimStanceStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(io.insertAnimStanceStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(io.insertAnimStanceStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);
		SQLBIND(io.updateAnimStanceCountStatement, int, "@countFound", );
		SQLBIND(io.updateAnimStanceCountStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(io.updateAnimStanceCountStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(io.updateAnimStanceCountStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(io.updateAnimStanceCountStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(io.updateAnimStanceCountStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);*/
		SQLBIND_DELAYED(query, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND_DELAYED(query, int, "@saberHolstered", std::get<1>(it->first));
		SQLBIND_DELAYED(query, int, "@torsoAnim", std::get<2>(it->first));
		SQLBIND_DELAYED(query, int, "@legsAnim", std::get<3>(it->first));
		SQLBIND_DELAYED(query, int, "@saberMove", std::get<4>(it->first));
		SQLBIND_DELAYED(query, int, "@stance", std::get<5>(it->first));
		SQLBIND_DELAYED(query, int, "@countFound", it->second);
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@demoVersion", std::get<0>(it->first));
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@saberHolstered", std::get<1>(it->first));
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@torsoAnim", std::get<2>(it->first));
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@legsAnim", std::get<3>(it->first));
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@saberMove", std::get<4>(it->first));
		//SQLBIND_DELAYED(io.updateAnimStanceCountStatement, int, "@stance", std::get<5>(it->first));

		io.animStanceQueries->push_back(queryWrapper);
	}
	
	for (auto it = frameInfoViewModelAnimCounts.begin(); it != frameInfoViewModelAnimCounts.end(); it++) {

		SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
		SQLDelayedQuery* query = &queryWrapper->query;

		SQLBIND_DELAYED(query, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND_DELAYED_TEXT(query, "@type", frameInfoTypeNames[std::get<1>(it->first)]);
		SQLBIND_DELAYED(query, int, "@viewModelAnim", std::get<2>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo0Index", std::get<3>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo1Index", std::get<4>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo2Index", std::get<5>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo3Index", std::get<6>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo4Index", std::get<7>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo5Index", std::get<8>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo6Index", std::get<9>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo7Index", std::get<10>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo8Index", std::get<11>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo9Index", std::get<12>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo10Index", std::get<13>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo11Index", std::get<14>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo12Index", std::get<15>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo13Index", std::get<16>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo14Index", std::get<17>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfo15Index", std::get<18>(it->first));
		SQLBIND_DELAYED(query, int, "@countFound", it->second);

		io.frameInfoVieWModelQueries->push_back(queryWrapper);
	}
	for (auto it = frameInfoViewModelAnimSimpleCounts.begin(); it != frameInfoViewModelAnimSimpleCounts.end(); it++) {

		SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
		SQLDelayedQuery* query = &queryWrapper->query;

		SQLBIND_DELAYED(query, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND_DELAYED_TEXT(query, "@type", frameInfoTypeNames[std::get<1>(it->first)]);
		SQLBIND_DELAYED(query, int, "@viewModelAnim", std::get<2>(it->first));
		SQLBIND_DELAYED(query, int, "@frameInfoIndex", std::get<3>(it->first));
		SQLBIND_DELAYED(query, int, "@countFound", it->second);

		io.frameInfoVieWModelSimpleQueries->push_back(queryWrapper);
	}
#endif
}


qboolean inline saveStatisticsToDb(ioHandles_t& io, bool& wasDoingSQLiteExecution, const sharedVariables_t& sharedVars, bool& SEHExceptionCaught) {
	__TRY{
		saveStatisticsToDbReal(io, wasDoingSQLiteExecution, sharedVars);
		return qtrue;
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
}

template<unsigned int max_clients>
qboolean inline demoHighlightFindExceptWrapper2(const char* sourceDemoFile, int bufferTime,  const highlightSearchMode_t searchMode, const ExtraSearchOptions& opts,bool& SEHExceptionCaught,int64_t& demoCurrentTime,bool& wasDoingSQLiteExecution,const ioHandles_t& io,sharedVariables_t& sharedVars) {

	__TRY{
		return demoHighlightFindReal<max_clients>(sourceDemoFile, bufferTime, searchMode, opts, demoCurrentTime,wasDoingSQLiteExecution,io,sharedVars,SEHExceptionCaught);
	}
	__EXCEPT{
		SEHExceptionCaught = true;
		return qfalse;
	}
}



template<unsigned int max_clients>
qboolean inline demoHighlightFindReal(const char* sourceDemoFile, int bufferTime, const highlightSearchMode_t searchMode, const ExtraSearchOptions& opts, int64_t& demoCurrentTime, bool& wasDoingSQLiteExecution, const ioHandles_t& io, sharedVariables_t& sharedVars, bool& SEHExceptionCaught) {
	fileHandle_t	oldHandle = 0;
	//fileHandle_t	newHandle = 0;
	msg_t			oldMsg;
	byte			oldData[MAX_MSGLEN];
	std::vector<byte>	oldDataRaw;
	int				oldSize;
	char			oldName[MAX_OSPATH];
	//char			newName[MAX_OSPATH];
	int				buf;
	int				readGamestate = 0;
	//demoPlay_t* play = demo.play.handle;
	qboolean		ret = qfalse;
	int				framesSaved = 0;
	char			ext[7]{};
	//char			originalExt[7]{};
	demoType_t		demoType;
	int				demoStartTime = 0;
	int64_t			demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int64_t			demoOldTime = 0;
	int				deltaTimeFromLastSnapshot = 0;
	int				lastGameStateChange = 0;
	int64_t			lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;
	int				lastKnownInOrderTime = 0;
	qboolean		isCompressedFile = qfalse;
	int				psGeneralSaberMove = 0;
	int				psGeneralPMType = 0;
	int				psGeneralTorsoAnim = 0;
	int				psGeneralLegsAnim = 0;
	const int		speedTypesSkip = opts.findSuperSlowKillStreaks ? 0 : 1; // The different max delays between kills for killstreaks are in an array. Normally we skip the last one (veeery long 18 seconds), but some ppl might wanna activate those too.

	

	Com_Memset(saberComboCounter,0,sizeof(saberComboCounter));
	Com_Memset(playerSpiralLocations,0,sizeof(playerSpiralLocations));
	Com_Memset(playerDemoStatsPointers,0,sizeof(playerDemoStatsPointers));
	Com_Memset(playerVisibleFrames,0,sizeof(playerVisibleFrames));
	Com_Memset(playerVisibleClientFrames,0,sizeof(playerVisibleClientFrames));
	Com_Memset(mohaaPlayerWeapon,0,sizeof(mohaaPlayerWeapon));
	Com_Memset(mohaaPlayerWeaponModelIndex,0,sizeof(mohaaPlayerWeaponModelIndex));
	for (int i = 0; i < max_clients; i++) {
		mohaaPlayerWeaponModelIndex[i] = -1;
	}
	Com_Memset(mohaaPlayerWeaponModelIndexThisFrame,0,sizeof(mohaaPlayerWeaponModelIndexThisFrame));
	//Com_Memset(mohaaPlayerWeaponTagNum,0,sizeof(mohaaPlayerWeaponTagNum));
	Com_Memset(playerFirstVisible,0,sizeof(playerFirstVisible));
	Com_Memset(playerFirstFollowed,0,sizeof(playerFirstFollowed));
	Com_Memset(playerFirstFollowedOrVisible,0,sizeof(playerFirstFollowedOrVisible));
	Com_Memset(lastEvent,0,sizeof(lastEvent));
	Com_Memset(lastGroundHeight,0,sizeof(lastGroundHeight));
	Com_Memset(lastEventTime,0,sizeof(lastEventTime));
	Com_Memset(playerLastSaberMove,0,sizeof(playerLastSaberMove));
	Com_Memset(recentFlagHoldTimes,0,sizeof(recentFlagHoldTimes));
	Com_Memset(recentKillsDuringFlagHold,0,sizeof(recentKillsDuringFlagHold));
	Com_Memset(recentFlagHoldEnemyNearbyTimes,0,sizeof(recentFlagHoldEnemyNearbyTimes));
	Com_Memset(recentFlagHoldVariousInfo,0,sizeof(recentFlagHoldVariousInfo));
	Com_Memset(playerTeams,0,sizeof(playerTeams));
	Com_Memset(teamInfo,0,sizeof(teamInfo));
	Com_Memset(&thisFrameInfo, 0, sizeof(thisFrameInfo));
	Com_Memset(&lastFrameInfo, 0, sizeof(lastFrameInfo));
	Com_Memset(&forcePowersInfo, 0, sizeof(forcePowersInfo));
	Com_Memset(&strafeDeviationsDefrag, 0, sizeof(strafeDeviationsDefrag));
	Com_Memset(&hitDetectionData, 0, sizeof(hitDetectionData));
	Com_Memset(&jumpDetected, 0, sizeof(jumpDetected));
	Com_Memset(&requiredMetaEventAges, 0, sizeof(requiredMetaEventAges));
	Com_Memset(&metaEventTrackers, 0, sizeof(metaEventTrackers));
	Com_Memset(&playerPastMetaEvents, 0, sizeof(playerPastMetaEvents));
	Com_Memset(&groundCrouchDurations, 0, sizeof(groundCrouchDurations));
	Com_Memset(&lastSneak, 0, sizeof(lastSneak));
	Com_Memset(&lastSneakDuration, 0, sizeof(lastSneakDuration));
	resetCurrentPacketPeriodStats();

	lastKnownFlagCarrier[TEAM_RED] = lastKnownFlagCarrier[TEAM_BLUE] = lastKnownFlagCarrier[TEAM_FREE] = -1;

	//Com_Memset(lastBackflip, 0, sizeof(lastBackflip));
	for (int i = 0; i < max_clients; i++) {
		lastBackflip[i] = -1;
		lastSelfSentryJump[i] = -1;
#if TRACK_GROUNDFRAME
		lastGroundFrame[i] = -1;
		lastPreGroundFrameAir[i] = -1;
#endif
	}
	Com_Memset(&specialJumpCount, 0, sizeof(specialJumpCount));
	Com_Memset(&headJumpCount, 0, sizeof(headJumpCount));
#if TRACK_GROUNDFRAME
	Com_Memset(&lastPreGroundFrameVelocity, 0, sizeof(lastPreGroundFrameVelocity));
	Com_Memset(&lastGroundFrameQuality, 0, sizeof(lastGroundFrameQuality));
	Com_Memset(&lastGroundFrameAngleChange, 0, sizeof(lastGroundFrameAngleChange));
#if TRACK_GROUNDFRAME_DEBUG
	Com_Memset(&lastGroundFrameDebug, 0, sizeof(lastGroundFrameDebug));
#endif
#endif
	//for (int i = 0; i < max_clients; i++) {
	//	walkDetectedTime[i] = -1;
	//}
	Com_Memset(&cgs,0,sizeof(cgs));
	for (int t = 0; t < TEAM_NUM_TEAMS; t++) {
		cgs.flag[t] = -1;
		cgs.flagLastChange[t] = -1;
		cgs.flagLastPickupOrigin[t] = -1;
		cgs.flagLastChangeToTaken[t] = -1;
	}

	resetLaughs();

	



	//mvprotocol_t	protocol;

	// Since not in MME:
	/*if (!play) {
		Com_Printf("Demo cutting is allowed in mme mode only.\n");
		return qfalse;
	}*/
	//startTime += play->startTime;
	//endTime += play->startTime;
	

	//protocol = MV_GetCurrentProtocol();
	//if (protocol == PROTOCOL_UNDEF)
	//	ext = ".dm_16";
	//else
	//	ext = va(".dm_%i", protocol);
	//ext = Cvar_FindVar("mme_demoExt")->string;
	//strncpy_s(oldName, sizeof(oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	//strncpy_s(ext, sizeof(ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalExt, sizeof(originalExt), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	/*char specialTypeChar = ext[3];
	ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));
	demoCutGetDemoType(sourceDemoFile,ext, oldName,&demoType,&isCompressedFile,&demo.cut.Clc);

	int CS_PLAYERS_here = getCS_PLAYERS(demoType);

	//createCompressedOutput = (qboolean)!isCompressedFile;

	/**
	demo.cut.Clc.demoCheckFor103 = qfalse;
	if (!*ext) {
		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}
	else if (!_stricmp(ext, ".dm_15")) {

		demoType = DM_15;
		strncpy_s(ext, sizeof(ext), ".dm_15", 6); 
		demo.cut.Clc.demoCheckFor103 = qtrue;
	}
	else if (!_stricmp(ext, ".dm_16")) {

		demoType = DM_16;
		strncpy_s(ext, sizeof(ext), ".dm_16", 6);
	}*/
	sharedVars.demoFilesize = oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue, isCompressedFile);
	if (!oldHandle) {
		demoErrorFlags |= DERR_OPENFAIL;
		Com_DPrintf("Failed to open %s for reading.\n", oldName);
		demoErrors << "Failed to open for reading.\n";
		return qfalse;
	}

	int oldSizeOriginal = oldSize;


	sharedVars.oldPath = va("%s%s", oldName, ext);
	sharedVars.oldBasename = sharedVars.oldPath.substr(sharedVars.oldPath.find_last_of("/\\") + 1);





	// turn oldpath absolute
	std::filesystem::path tmpFSPath = sharedVars.oldPath;
	sharedVars.oldPath = std::filesystem::absolute(tmpFSPath).string();

	std::filesystem::file_time_type filetime = std::filesystem::last_write_time(va("%s%s", oldName, ext));
	//time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));
	sharedVars.oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));



	if (opts.makeVideo) {
		SaberAnimationStuff::init();
		S3L_model3DInit(
			cubeVertices,
			S3L_CUBE_VERTEX_COUNT,
			cubeTriangles,
			S3L_CUBE_TRIANGLE_COUNT,
			&cubeModel);
	}


	int messageOffset = 0;


	// Maybe we will make these user options someday...
	const qboolean derivativeRetiming = qtrue;
	const qboolean derivativeMagnitudeCompensate = qtrue;

	float accelerationMultiplier = derivativeMagnitudeCompensate ?  1.0f / 100.0f : 1.0f;
	float jerkMultiplier = derivativeMagnitudeCompensate ? 1.0f / 10000.0f : 1.0f;
	float snapMultiplier = derivativeMagnitudeCompensate ? 1.0f / 1000000.0f : 1.0f;

	// If dropped or base flag entities exist, we track them so we can detect "almost captures"
	// Array is sized for teams, with the number corresponding to clientinfo string TEAM_FREE, TEAM_RED, TEAM_BLUE, TEAM_SPECTATOR enum values.
	// I was using NUM_TEAMS_MAX first but I'm not sure if some mods might not assign too high team values for random reasons. So this is safer.
	entityState_t*	droppedFlagEntities[MAX_TEAMS]{};
	entityState_t*	baseFlagEntities[MAX_TEAMS]{};

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	bool firstSnapAfterGamestate = false;
	bool sequenceNumOrderChanged = false;

	int oldSequenceNum=-9999;
	int maxSequenceNum=-9999;
	//	Com_SetLoadingMsg("Cutting the demo...");
	while (oldSize > 0) {

		Com_Memset(&thisFrameInfo, 0, sizeof(thisFrameInfo));
		Com_Memset(&hitDetectionData, 0, sizeof(hitDetectionData));
		Com_Memset(&jumpDetected, 0, sizeof(jumpDetected));

		thisFrameInfo.demoTime = demoCurrentTime; // temporary, until snapshot assigns. bad? but i think we always use this inside snapshot anyway.
		thisFrameInfo.serverTime = demo.cut.Cl.snap.serverTime; // temporary, until snapshot assigns. bad? but i think we always use this inside snapshot anyway.

		qboolean strafeApplicablePlayerStateThisFrame = qfalse;
		float playerStateStrafeDeviationThisFrame = 0;

		int redFlagStatusResetByConfigstring = 0;
		int blueFlagStatusResetByConfigstring = 0;
		int64_t	redFlagNewCarrierByEventBitMask = 0;
		int64_t	blueFlagNewCarrierByEventBitMask = 0;
		int64_t	redFlagCapturedPlayerBitMask = 0;
		int64_t	blueFlagCapturedPlayerBitMask = 0;
		int64_t	playersKilledThisFrameBitMask = 0;

	cutcontinue:

		int oldOldSize = oldSize;

		if (isCompressedFile) {
			oldDataRaw.clear();
			MSG_InitRaw(&oldMsg, &oldDataRaw); // Input message
		}
		else {
			MSG_Init(&oldMsg, oldData, sizeof(oldData)); // Input message
		}
		oldSequenceNum = demo.cut.Clc.serverMessageSequence;

		/* Read the sequence number */
		if (FS_Read(&demo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4) {
			demoErrorFlags |= DERR_FAILREADSEQNUM;
			demoErrors << "Failed to read sequence number.\n";
			goto cuterror;
		}

		demo.cut.Clc.serverMessageSequence = LittleLong(demo.cut.Clc.serverMessageSequence);

		maxSequenceNum = std::max(maxSequenceNum, demo.cut.Clc.serverMessageSequence);

		sequenceNumOrderChanged = false;
		if (demo.cut.Clc.serverMessageSequence == oldSequenceNum) {
			demoErrorFlags |= DERR_DUPEMSGNUM;
			std::cerr << "WARNING: Duplicated message number "<< oldSequenceNum << " at demotime " << demoCurrentTime << " (" << DPrintFLocation << ")\n";
			demoErrors << "WARNING: Duplicated message number "<< oldSequenceNum << " at demotime " << demoCurrentTime << ")\n";
		}
		else if (demo.cut.Clc.serverMessageSequence < oldSequenceNum) {
			sequenceNumOrderChanged = true;
			if (demo.cut.Clc.serverMessageSequence == -1) {
#ifdef DEBUG
				std::cout << "Message number changed to -1. Probably end of demo after message " << oldSequenceNum << " at demotime " << demoCurrentTime << " with "<< oldSize << " bytes left." << "\n";
#endif
			}
			else {
				std::cerr << "WARNING: Message number order changed: " << oldSequenceNum << "->" << demo.cut.Clc.serverMessageSequence << " at demotime " << demoCurrentTime << " (" << DPrintFLocation << ")\n";
			}
		}
		else {
			currentPacketPeriodStats.droppedPackets += demo.cut.Clc.serverMessageSequence - oldSequenceNum - 1; // Stats
		}

		oldSize -= 4;
		/* Read the message size */
		if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4) {
			goto cuterror;
		}
		oldMsg.cursize = LittleLong(oldMsg.cursize);
		oldSize -= 4;
		/* Negative size signals end of demo */
		if (oldMsg.cursize < 0) {
			if (oldMsg.cursize == -1) {
				if (oldSize > 0
#ifdef DEBUG
	|| true // wait. why?
#endif
					) {
					if (oldSize > 0) {
						demoErrorFlags |= DERR_MSGSIZEENDBUTLEFT;
						demoErrors << "Message size changed to -1. Probably end of demo after message " << oldSequenceNum << " at demotime " << demoCurrentTime << " with " << oldSize << " bytes left." << "\n";
					}
					std::cout << "Message size changed to -1. Probably end of demo after message " << oldSequenceNum << " at demotime " << demoCurrentTime << " with " << oldSize << " bytes left." << "\n";
				}
			}
			else {
				demoErrorFlags |= DERR_WEIRDMSGSIZE;
				demoErrors << "Message size changed to weird value: "<< oldMsg.cursize << ". Treating as end of demo after message " << oldSequenceNum << " at demotime " << demoCurrentTime << " with " << oldSize << " bytes left." << "\n";
				std::cout << "Message size changed to weird value: "<< oldMsg.cursize << ". Treating as end of demo after message " << oldSequenceNum << " at demotime " << demoCurrentTime << " with " << oldSize << " bytes left." << "\n";
			}
			break;
		}
		if (oldMsg.cursize > oldMsg.maxsize) {
			demoErrorFlags |= DERR_MSGSIZEOVERMAX;
			demoErrors << "Message size above maxsize.\n";
			goto cuterror;
		}
		/* Read the actual message */
		if (oldMsg.raw) {
			oldMsg.dataRaw->resize(oldMsg.cursize);
			if (FS_Read(oldMsg.dataRaw->data(), oldMsg.cursize, oldHandle) != oldMsg.cursize) {
				demoErrorFlags |= DERR_FAILREADMSG;
				demoErrors << "Failed to read entire message (raw).\n";
				goto cuterror;
			}
		}
		else {
			if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize) {
				demoErrorFlags |= DERR_FAILREADMSG;
				demoErrors << "Failed to read entire message.\n";
				goto cuterror;
			}
		}
		oldSize -= oldMsg.cursize;

		//Stats
		int thisMessageTotalSize = oldOldSize - oldSize;
		currentPacketPeriodStats.totalPacketCount++;
		currentPacketPeriodStats.totalPacketsSize += thisMessageTotalSize;
		currentPacketPeriodStats.maxPacketSize = std::max(currentPacketPeriodStats.maxPacketSize, thisMessageTotalSize);
		currentPacketPeriodStats.minPacketSize = std::min(currentPacketPeriodStats.minPacketSize, thisMessageTotalSize);


		// init the bitstream
		MSG_BeginReading(&oldMsg);
		// Skip the reliable sequence acknowledge number
		MSG_ReadLong(&oldMsg);
		//
		// parse the message
		//
		byte previouscmd = 255;
		byte previousOcmd = 255;
		while (1) {
			bool malformedMessageCaught = false;
			int oldMsgOffset = oldMsg.readcount;

			byte cmd, ocmd;
			if (oldMsg.readcount > oldMsg.cursize) {
				demoErrorFlags |= DERR_READPASTEND;
				Com_DPrintf("Demo cutter, read past end of server message.\n");
				demoErrors << "Read past end of server message\n";
				goto cuterror;
			}
			ocmd = MSG_ReadByte(&oldMsg);
			cmd = generalizeGameSVCOp(ocmd, demoType);
			if (cmd == svc_EOF_general) {
				demoCutReadPossibleHiddenUserCMDs(&oldMsg,demoType,SEHExceptionCaught);

				const char* maybeMeta = demoCutReadPossibleMetadata(&oldMsg, demoType);
				if (maybeMeta) {

					rapidjson::Document* jsonPreviousMetaDocument = new rapidjson::Document();
					if (jsonPreviousMetaDocument->Parse(maybeMeta).HasParseError() || !jsonPreviousMetaDocument->IsObject()) {
						// We won't quit demo cutting over this. It's whatever. We don't wanna make a demo unusable just because it contains bad
						// metadata. Kinda goes against the spirit. This is a different approach from above with the main metadata, where an error in that
						// will quit the process. Because the user can after all just adjust and fix the commandline.
						demoErrorFlags |= DERR_BADJSONMETA;
						demoErrors << "Demo appears to contain metadata, but wasn't able to parse it. Discarding.\n";
						std::cout << "Demo appears to contain metadata, but wasn't able to parse it. Discarding.\n";
						break;
					}

					const char* snapsmanipKey = jsonGetRealMetadataKeyName(jsonPreviousMetaDocument,"opt_snapsmanip");
					if (snapsmanipKey) {
						int opt_snapsmanip = (*jsonPreviousMetaDocument)[snapsmanipKey].GetInt();
						if (opt_snapsmanip) {
							demoDerivativeFlags |= DERIV_SNAPSMANIP;
						}
					}

				}
				break;
			}
			// skip all the gamestates until we reach needed
			if (readGamestate < demo.currentNum) {
				//if (readGamestate < (demo.nextNum-1)) { // not sure if this is correct tbh... but I dont wanna rewrite entire cl_demos
				if (cmd == svc_gamestate_general) {
					readGamestate++;
				}
				goto cutcontinue;
			}


			// other commands
			switch (cmd) {
			case svc_centerprint_general:
			case svc_locprint_general:
			case svc_cgameMessage_general:
				if (isMOHAADemo) {
					demoCutParseMOHAASVC(&oldMsg, demoType, cmd, SEHExceptionCaught);
					break;
				}
			default:
				demoErrorFlags |= DERR_ILLEGIBLESRVMSG;
				Com_DPrintf("ERROR: CL_ParseServerMessage: Illegible server message %d (%d); previous: %d (%d) \n",cmd,ocmd,previouscmd,previousOcmd);
				demoErrors << "ERROR: CL_ParseServerMessage: Illegible server message "<< cmd<<" ("<< ocmd<<"); previous: "<< previouscmd<<" ("<< previousOcmd <<") \n";
				if (isMOHAADemo && readGamestate == 0) {
					goto cutcontinue; // Uh I made some silly oopsie
				}
				goto cuterror;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				if (!demoCutParseCommandString(&oldMsg, &demo.cut.Clc, demoType,SEHExceptionCaught)) {
					demoErrorFlags |= DERR_FAILPARSECMDSTR;
					demoErrors << "Failed to parse command string\n";
					goto cuterror;
				}
				currentPacketPeriodStats.totalServerCommandSize += oldMsg.readcount - oldMsgOffset;

				// Pre-execution of commands for mohaa (since events are sent as prints)
				// Change: Also do it in any demo for flag status
				if(true || isMOHAADemo){
					for (; demo.cut.Clc.lastPreExecutedServerCommand <= demo.cut.Clc.serverCommandSequence; demo.cut.Clc.lastPreExecutedServerCommand++) {
						char* command = demo.cut.Clc.serverCommands[demo.cut.Clc.lastPreExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
						Cmd_TokenizeString(command);

						char* cmd = Cmd_Argv(0);

						//if (isMOHAADemo) {

							if (!strcmp(cmd, "cs")) {

								int index = atoi(Cmd_Argv(1));
								if (index == CS_FLAGSTATUS) {
									char* str = Cmd_Argv(2);

									int flagTmp[TEAM_NUM_TEAMS];
									//int redflagTmp, blueflagTmp, yellowflagTmp;
									// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
									if (strlen(str) >= 2) {
										if (str[0] != '1') {
											redFlagStatusResetByConfigstring++;
										}
										if (str[1] != '1') {
											blueFlagStatusResetByConfigstring++;
										}
										flagTmp[TEAM_RED] = str[0] - '0';
										flagTmp[TEAM_BLUE] = str[1] - '0';
									}
									else { // This is some weird bug/imperfection in the code. Sometimes it just sends cs 23 0 for whatever reason. Seems to happen at end of games.
										redFlagStatusResetByConfigstring++;
										blueFlagStatusResetByConfigstring++;
										flagTmp[TEAM_RED] = 0;
										flagTmp[TEAM_BLUE] = 0;
									}
									if (strlen(str) >= 3) { // Too lazy to do other way lol.
										flagTmp[TEAM_FREE] = str[2] - '0';
									}
									else {
										flagTmp[TEAM_FREE] = 0;
									}

									// We already set cgs.flagLastChangeToTaken[TEAM_RED] up here because farther down (after capture/kill event evaluation) it is used to update flag hold times.
									// Without this, the flag hold time will be wrong on the first and following frame, potentially leading to wrong flag hold times for captures that happened in 1 frame or 2 frames.
									// But we don't yet update other flag related status stuff, we do that at the bottom. (idk it's messy maybe it doesnt even make sense. TODO)
									for (int t = TEAM_FREE; t < TEAM_SPECTATOR; t++) {
										if (cgs.flag[t] != flagTmp[t]) {
											//cgs.flagLastChange[TEAM_RED] = demoCurrentTime;
											if (flagTmp[t] == 1) {
												cgs.flagLastChangeToTaken[t] = demoCurrentTime;
												//cgs.flagLastPickupOrigin[TEAM_RED] = cgs.flag[TEAM_RED];
											}
											else if (cgs.flag[t] == 1 && cgs.flagLastChangeToTaken[t] != demoCurrentTime) {
												cgs.flagLastFlagHoldTimeOver0[t] = demoCurrentTime - cgs.flagLastChangeToTaken[t]; // to track caps where capper was unknown up to and including the cap. we will not have accurate flaghold times otherwise
											}
										}
									}
									/*if (cgs.flag[TEAM_RED] != flagTmp[TEAM_RED]) {
										//cgs.flagLastChange[TEAM_RED] = demoCurrentTime;
										if (redflagTmp == 1) {
											cgs.flagLastChangeToTaken[TEAM_RED] = demoCurrentTime;
											//cgs.flagLastPickupOrigin[TEAM_RED] = cgs.flag[TEAM_RED];
										}
										else if (cgs.flag[TEAM_RED] == 1 && cgs.flagLastChangeToTaken[TEAM_RED] != demoCurrentTime) {
											cgs.flagLastFlagHoldTimeOver0[TEAM_RED] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_RED]; // to track caps where capper was unknown up to and including the cap. we will not have accurate flaghold times otherwise
										}
									}
									if (cgs.flag[TEAM_BLUE] != blueflagTmp) {
										//cgs.flagLastChange[TEAM_BLUE] = demoCurrentTime;
										if (blueflagTmp == 1) {
											if (cgs.flagLastChangeToTaken[TEAM_BLUE] != demoCurrentTime) {
												cgs.flagLastFlagHoldTimeOver0[TEAM_BLUE] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_BLUE]; // to track caps where capper was unknown up to and including the cap. we will not have accurate flaghold times otherwise
											}
											cgs.flagLastChangeToTaken[TEAM_BLUE] = demoCurrentTime;
											//cgs.flagLastPickupOrigin[TEAM_BLUE] = cgs.flag[TEAM_BLUE];
										}
									}
									if (cgs.flag[TEAM_FREE] != yellowflagTmp) {
										//cgs.flagLastChange[TEAM_FREE] = demoCurrentTime;
										if (yellowflagTmp == 1) {
											if (cgs.flagLastChangeToTaken[TEAM_FREE] != demoCurrentTime) {
												cgs.flagLastFlagHoldTimeOver0[TEAM_FREE] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_FREE]; // to track caps where capper was unknown up to and including the cap. we will not have accurate flaghold times otherwise
											}
											cgs.flagLastChangeToTaken[TEAM_FREE] = demoCurrentTime;
											//cgs.flagLastPickupOrigin[TEAM_FREE] = cgs.flag[TEAM_FREE]; // Not sure if this is correct?
										}
									}*/

								}
							}
							else if (isMOHAADemo) {
								if (!strcmp(cmd, "print")) {
									entityState_t* deathEvent = parseMOHAADeathMessage(&playerNamesToClientNums, Cmd_Argv(1));
									if (deathEvent) {
										parsedEventEntities.push(deathEvent);
									}
								}
								else if (!strcmp(cmd, "printdeathmsg")) {
									entityState_t* deathEvent = parseMOHAAPrintDeathMsgFromTokenized(&playerNamesToClientNums);
									if (deathEvent) {
										parsedEventEntities.push(deathEvent);
									}
								}
							}
						//}
					}
				}
				break;
			case svc_gamestate_general:
				lastGameStateChange = demo.cut.Cl.snap.serverTime;
				lastGameStateChangeInDemoTime = demoCurrentTime;
				//if (readGamestate > demo.currentNum) {
				//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
				//	goto cutcomplete;
				//}
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl,&demoType, (qboolean)(readGamestate == 0),SEHExceptionCaught)) { // Pass demoType by reference in case we need 1.03 detection
					demoErrorFlags |= DERR_FAILPARSEGAMESTATE;
					demoErrors << "Failed to parse gamestate\n";
					goto cuterror;
				}
				if (opts.makeVideo || opts.throughWallChecks) {

					char mapname[MAX_STRING_CHARS_MAX];
					if (cm) {
						delete cm;
						cm = NULL;
						haveMapModel = false;
					}
					int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
					const char* info = demo.cut.Cl.gameState.stringData + offset;
					Q_strncpyz(mapname,sizeof(mapname),Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "mapname"),sizeof(mapname));
					if (mapname[0]) {
						int mapnameLen = strlen(mapname);
						if (mapnameLen + 4 < sizeof(mapname)) {
							mapname[mapnameLen] = '.';
							mapname[mapnameLen + 1] = 'b';
							mapname[mapnameLen + 2] = 's';
							mapname[mapnameLen + 3] = 'p';
							mapname[mapnameLen + 4] = '\0';

							std::string mappath = CModel::GetMapPath(mapname, &opts.bspDirectories);
							if (mappath.size() > 0) {
								cm = new CModel(mappath.c_str(), opts.makeVideo);
								if (opts.makeVideo) {

									auto faceVerts = cm->GetFaceVerts();
									auto faceVertIndices = cm->GetFaceVertIndices();
									auto lightmaps = cm->GetLightmaps();
									mapVertices.clear();
									mapUVs.clear();
									mapLightmaps.clear();
									for (auto it = lightmaps.begin(); it != lightmaps.end(); it++) {
										mapLightmaps.push_back(*it);
									}
									//if (!mapLightmaps.size()) {
									//	mapLightmaps.emplace_back();
									//}
									for (auto it = faceVerts.begin(); it != faceVerts.end(); it++) {
										mapVertices.push_back(S3L_POSX(it->xyz[1]));
										mapVertices.push_back(S3L_POSY(it->xyz[2]));
										mapVertices.push_back(S3L_POSZ(it->xyz[0]));
										mapUVs.push_back(it->lightmapSt[0] * LIGHTMAP_SIZE);
										mapUVs.push_back(it->lightmapSt[1] * LIGHTMAP_SIZE);
										mapUVs.push_back(it->lightmapNum >= mapLightmaps.size() ? mapLightmaps.size() - 1 : it->lightmapNum);
									}
									mapTriangles.clear();
									for (auto it = faceVertIndices.begin(); it != faceVertIndices.end(); it++) {
										mapTriangles.push_back(*it);
									}
									S3L_model3DInit(mapVertices.data(), mapVertices.size() / 3, mapTriangles.data(), mapTriangles.size() / 3, &mapModel);
									//mapModel.config.backfaceCulling = 1;
									haveMapModel = true;
								}
							}
						}
					}
				}

				firstSnapAfterGamestate = true;

				lastKnownFlagCarrier[TEAM_RED] = lastKnownFlagCarrier[TEAM_BLUE] = lastKnownFlagCarrier[TEAM_FREE] = -1; // New gamestate. Fair to assume that nobody has the flag now. or its a demo starting in the middle of a game, but then we wouldn't know anyway.

				sentryModelIndex = -1; // Reset this here because a new gamestate could mean that modelIndizi changed

				CheckForNameChanges<max_clients>(&demo.cut.Cl,io,demoType,wasDoingSQLiteExecution,opts);
				setPlayerAndTeamData<max_clients>(&demo.cut.Cl, demoType);
				updateForcePowersInfo(&demo.cut.Cl);
				updateGameInfo(&demo.cut.Cl, demoType,opts);
				if (opts.quickSkipNonSaberExclusive && !gameIsSaberOnlyIsh) {
					std::cout << "Quick skipping demo of game that allows other weapons than saber/melee/explosives.";
					goto cutcomplete;
				}
				updatePlayerDemoStatsArrayPointers<max_clients>(demoType, opts);
				//Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
				//newHandle = FS_FOpenFileWrite(newName);
				//if (!newHandle) {
				//	Com_Printf("Failed to open %s for target cutting.\n", newName);
				//	return qfalse;
				//}
				readGamestate++;
				break;
			case svc_snapshot_general:
				malformedMessageCaught = false;
				GlobalDebugDemoTime = demoCurrentTime;
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType,SEHExceptionCaught, malformedMessageCaught,qtrue)) {
					demoErrorFlags |= DERR_FAILPARSESNAP;
					demoErrors << "Failed to parse snapshot\n";
					if (malformedMessageCaught) {
						demoErrorFlags |= DERR_SNAPMALFORMED;
						demoErrors << "Trying to continue after malformed message...\n";
						std::cout << "Trying to continue after malformed message...\n";
						goto cutcontinue;
					}
					goto cuterror;
				}
				currentPacketPeriodStats.angleChanges += (int)(demo.cut.Cl.oldSnap.ps.clientNum != demo.cut.Cl.snap.ps.clientNum); // I'm 14 and this is optimized.
				currentPacketPeriodStats.nonDeltaSnapshotCount += (int)(demo.cut.Cl.snap.deltaNum == -1); // I'm 14 and this is optimized.
				currentPacketPeriodStats.entitiesReceivedTotal += demo.cut.Cl.snap.numEntities;
				currentPacketPeriodStats.totalSnapshotSize += oldMsg.readcount - oldMsgOffset;

				// Time related stuff
				if (messageOffset++ == 0) {
					// first message in demo. Get servertime offset from here to cut correctly.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}

				if (demo.cut.Clc.serverMessageSequence == maxSequenceNum) { // Only do any form of time base handling if packets are arriving in linear order.
					if (demo.cut.Cl.snap.serverTime < lastKnownInOrderTime /* && demo.cut.Cl.snap.serverTime < 10000*/) { // Assume a servertime reset (new serverTime is under 10 secs). 
						// Ok so: The old method was to assume that it's a proper serverTime reset if serverTime is under 10000 (we are freshly after a restart).
						// But some demos can be so damn laggy that we go slightly past that.
						// We can't entirely remove the check because some demos can have serverTime going slightly backwards due to packets arriving in the wrong order (blame on me I guess)
						// However, whenever the serverTime goes backwards due to lag, the order of demo.cut.Clc.serverMessageSequence is jumbled up as well!
						// So we can use that as a much more reliable check for whether it is a true serverTime reset or not.
						//
						//if (demo.cut.Cl.snap.serverTime < 120000 || (lastKnownTime - demo.cut.Cl.snap.serverTime) > 120000) {

						if (demo.cut.Cl.snap.serverTime > 10000) {
							// This is a non-critical warning, mostly for debugging. It used to be more dangerous.
							std::cerr << "demo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Clc.serverMessageSequence == maxSequenceNum but demo.cut.Cl.snap.serverTime > 10000;  delta " << (lastKnownTime - demo.cut.Cl.snap.serverTime) << ", sequenceNumOrderChanged " << (sequenceNumOrderChanged ? "true" : "false") << ", firstSnapAfterGameState " << (firstSnapAfterGamestate ? "true" : "false") << ", demoOldTime " << demoOldTime << ", lastGameStateChangeInDemoTime " << lastGameStateChangeInDemoTime << ", lastGameStateChange " << lastGameStateChange << ", demoCurrentTime " << demoCurrentTime << ", demoBaseTime " << demoBaseTime << ", demoStartTime " << demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", lastKnownTime " << lastKnownTime << " (" << DPrintFLocation << ")\n";
							//std::cerr << "demo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Clc.serverMessageSequence == maxSequenceNum but demo.cut.Cl.snap.serverTime > 10000; delta " << (lastKnownTime - demo.cut.Cl.snap.serverTime) << ", demoOldTime " << demoOldTime << ", demoCurrentTime " << demoCurrentTime << ", demoBaseTime " << demoBaseTime << ", demoStartTime " << demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", lastKnownTime " << lastKnownTime << " (" << DPrintFLocation << ")\n";
						}
						
						demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
						demoStartTime = demo.cut.Cl.snap.serverTime;
					}
					lastKnownInOrderTime = demo.cut.Cl.snap.serverTime;
				}
				demoCurrentTime = demoBaseTime + demo.cut.Cl.snap.serverTime - demoStartTime;
				thisFrameInfo.demoTime = demoCurrentTime;
				thisFrameInfo.serverTime = demo.cut.Cl.snap.serverTime;
				if (demoCurrentTime < 0) {
					std::cerr << "demoCurrentTime negative wtf?! demoOldTime "<< demoOldTime << ", demoCurrentTime " << demoCurrentTime  << ", demoBaseTime " << demoBaseTime << ", demoStartTime " << demoStartTime << ", serverTime " << demo.cut.Cl.snap.serverTime << ", lastKnownTime " << lastKnownTime << " (" << DPrintFLocation << ")\n";
				}
				deltaTimeFromLastSnapshot = demoCurrentTime - demoOldTime;
				lastKnownTime = demo.cut.Cl.snap.serverTime;
				currentPacketPeriodStats.periodTotalTime += demoCurrentTime - demoOldTime;
				demoOldTime = demoCurrentTime;
				firstSnapAfterGamestate = false;

				for (int cl = 0; cl < max_clients; cl++) {
					requiredMetaEventAges[METRACKER_KILLS][cl] = demoCurrentTime - bufferTime; // Kills are a point in time. Whenever a kill happens, we need to have the past [bufferTime] milliseconds available. So just always require the last [bufferTime] milliseconds minimum for kills.
				}

				psGeneralPMType = generalizeGameValue<GMAP_PLAYERMOVETYPE, SAFE>(demo.cut.Cl.snap.ps.pm_type,demoType);
				psGeneralSaberMove = generalizeGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(demo.cut.Cl.snap.ps.saberMove,demoType);
				psGeneralLegsAnim = generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(demo.cut.Cl.snap.ps.legsAnim,demoType);
				psGeneralTorsoAnim = generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(demo.cut.Cl.snap.ps.torsoAnim,demoType);

				Com_Memset(droppedFlagEntities, 0, sizeof(droppedFlagEntities));
				Com_Memset(baseFlagEntities, 0, sizeof(baseFlagEntities));
				//Com_Memset(mohaaPlayerWeaponTagNum, 0, sizeof(mohaaPlayerWeaponTagNum));
				for (int i = 0; i < max_clients; i++) {
					mohaaPlayerWeaponModelIndexThisFrame[i] = -1;
				}

				if (isMOHAADemo) {
					playerTeams[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.stats[STAT_TEAM_MOH];
				}

				if (opts.makeVideo) {
					static int64_t lastDemoRenderFrameTime = -1000;
					//const int minmsecpassed = 90; // 42;
					if (demoCurrentTime - lastDemoRenderFrameTime >= opts.videoMinMsec) {
						lastDemoRenderFrameTime = demoCurrentTime;
						scene3dmodels.clear();
						scene3dmodelProperties.clear();
						scene3dmodelVertices.clear();
						scene3dmodelTriangles.clear();

						// set up camera pos
						vec3_t camerapos;
						vec3_t forward;
						AngleVectors(demo.cut.Cl.snap.ps.viewangles, forward, NULL, NULL);
						VectorMA(demo.cut.Cl.snap.ps.origin, -80.0f, forward, camerapos);
						camerapos[2] += demo.cut.Cl.snap.ps.viewheight;

						// world/models
						if (haveMapModel) {
							renderingMap = true;

							S3L_Model3D visFilteredWorld = mapModel;
							auto faceVertIndices = cm->GetVisFilteredFaceVertIndices(camerapos);
							mapTrianglesVisFiltered.clear();
							for (auto it = faceVertIndices.begin(); it != faceVertIndices.end(); it++) {
								mapTrianglesVisFiltered.push_back(*it);
							}
							S3L_model3DInit(mapVertices.data(), mapVertices.size() / 3, mapTrianglesVisFiltered.data(), mapTrianglesVisFiltered.size() / 3, &visFilteredWorld);
							scene3dmodels.push_back(visFilteredWorld);
							//scene3dmodels.push_back(mapModel);
							scene3dmodelProperties.emplace_back();
						}
						else {
							renderingMap = false;
						}
						vec3_t mins, maxs;

						int saberWeap = specializeGameValue<GMAP_WEAPONS, UNSAFE>(WP_SABER_GENERAL,demoType);

						for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

							entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
							//if (thisEs->number >= 32) break;
							S3L_Model3D model = cubeModel;
							int thisEntityType = generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(thisEs->eType, demoType);
							bool isPlayerEnt = (thisEntityType == ET_PLAYER_GENERAL || thisEntityType == ET_GRAPPLE_GENERAL);
							bool isDeadPlayer = false;

							if ((!thisEs->solid && !isPlayerEnt && thisEntityType >= ET_EVENTS_GENERAL) || thisEs->solid == SOLID_BMODEL) continue;

							bool isForceField = thisEntityType == ET_SPECIAL_GENERAL && thisEs->modelindex == 2 && thisEs->time2;

							if (isPlayerEnt && !thisEs->solid && (thisEs->eFlags & EF_DEAD)) {
								VectorSet(mins, -15, -15, DEFAULT_MINS_2);
								VectorSet(maxs, 15, 15, -8);
								isDeadPlayer = true;
							} 
							else if (!thisEs->solid) {
								VectorSet(mins, -10, -10, -10);
								VectorSet(maxs, 10, 10, 10);
							}
							else {
								IntegerToBoundingBox(thisEs->solid, isForceField ? thisEs->time2 : 0, mins, maxs, demoType); // thisEs->modelindex == 2  means HI_SHIELD
							}
							float zCenter = thisEs->pos.trBase[2];
							zCenter += (mins[2] + maxs[2]) * 0.5f;

							model.transform.translation.x = S3L_POSX(thisEs->pos.trBase[1]);
							model.transform.translation.y = S3L_POSY(zCenter);
							model.transform.translation.z = S3L_POSZ(thisEs->pos.trBase[0]);
							model.transform.scale.x = (maxs[1] - mins[1]) * S3L_POSMULT;
							model.transform.scale.y = (maxs[2] - mins[2]) * S3L_POSMULT;
							model.transform.scale.z = (maxs[0] - mins[0]) * S3L_POSMULT;

							scene3dmodels.push_back(model);
							drawProperties3dModel_t* modelProps = &scene3dmodelProperties.emplace_back();
							setModelColor<max_clients>(modelProps, isForceField ? -(thisEs->otherEntityNum2+1) : thisEs->number);
							if (isDeadPlayer) {
								modelProps->color[0] >>= 2;
								modelProps->color[1] >>= 2;
								modelProps->color[2] >>= 2;
							}
							else if (!thisEs->solid) {
								modelProps->color[0] >>= 4;
								modelProps->color[1] >>= 4;
								modelProps->color[2] >>= 4;
							}
							modelProps->transparent = isPlayerEnt || isForceField;
							modelProps->isCube = true;

							if (isPlayerEnt && thisEs->powerups & ((1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) | (1 << PW_NEUTRALFLAG))) {
								// draw "flag"
								int flagTeam = (thisEs->powerups & (1 << PW_REDFLAG)) ? TEAM_RED : ((thisEs->powerups & (1 << PW_BLUEFLAG)) ? TEAM_BLUE : TEAM_FREE);
								zCenter += 35;
								model.transform.translation.y = S3L_POSY(zCenter);
								model.transform.scale.x = 10 * S3L_POSMULT;
								model.transform.scale.y = 15 * S3L_POSMULT;
								model.transform.scale.z = 2 * S3L_POSMULT;
								model.transform.rotation.y = S3L_ROTY(thisEs->apos.trBase[1]);
								scene3dmodels.push_back(model);
								drawProperties3dModel_t* modelProps2 = &scene3dmodelProperties.emplace_back();
								setModelColor<max_clients>(modelProps2, -(flagTeam+1));
								modelProps2->transparent = false;
								modelProps2->isCube = true;
							}

							if (isPlayerEnt && thisEs->weapon == saberWeap) {
								// draw saber
								sceneAdd3DSaberForPlayer<max_clients>(thisEs->pos.trBase,thisEs->apos.trBase,thisEs->torsoAnim,camerapos,demoType,thisEs->number,demoCurrentTime);
							}
						}

						S3L_Model3D model = cubeModel;
						float zCenter = demo.cut.Cl.snap.ps.origin[2];
						if (demo.cut.Cl.snap.ps.viewheight >= DEFAULT_VIEWHEIGHT) {
							maxs[2] = DEFAULT_MAXS_2;
						} else if (demo.cut.Cl.snap.ps.viewheight >= CROUCH_VIEWHEIGHT) {
							maxs[2] = CROUCH_MAXS_2;
						} else {
							maxs[2] = -8;
						} 
						zCenter += (MINS_Z + maxs[2]) * 0.5f;
						model.transform.translation.x = S3L_POSX(demo.cut.Cl.snap.ps.origin[1]);
						model.transform.translation.y = S3L_POSY(zCenter);
						model.transform.translation.z = S3L_POSZ(demo.cut.Cl.snap.ps.origin[0]);
						model.transform.scale.x = 30 * S3L_POSMULT;
						model.transform.scale.y = (maxs[2]-MINS_Z) * S3L_POSMULT;
						model.transform.scale.z = 30 * S3L_POSMULT;

						scene3dmodels.push_back(model);
						drawProperties3dModel_t* modelProps = &scene3dmodelProperties.emplace_back();
						setModelColor<max_clients>(modelProps, demo.cut.Cl.snap.ps.clientNum);
						modelProps->transparent = true;
						modelProps->isCube = true;

						if (demo.cut.Cl.snap.ps.powerups[PW_REDFLAG] || demo.cut.Cl.snap.ps.powerups[PW_BLUEFLAG] || demo.cut.Cl.snap.ps.powerups[PW_NEUTRALFLAG]) {
							// draw "flag"
							int flagTeam = demo.cut.Cl.snap.ps.powerups[PW_REDFLAG] ? TEAM_RED : (demo.cut.Cl.snap.ps.powerups[PW_BLUEFLAG] ? TEAM_BLUE : TEAM_FREE);
							zCenter += 35;
							model.transform.translation.y = S3L_POSY(zCenter);
							model.transform.scale.x = 10 * S3L_POSMULT;
							model.transform.scale.y = 15 * S3L_POSMULT;
							model.transform.scale.z = 2 * S3L_POSMULT;
							model.transform.rotation.y = S3L_ROTY(demo.cut.Cl.snap.ps.viewangles[1]);
							scene3dmodels.push_back(model);
							drawProperties3dModel_t* modelProps2 = &scene3dmodelProperties.emplace_back();
							setModelColor<max_clients>(modelProps2, -(flagTeam + 1));
							modelProps2->transparent = false;
							modelProps2->isCube = true;
						}

						if (demo.cut.Cl.snap.ps.weapon == saberWeap) {
							// draw saber
							sceneAdd3DSaberForPlayer<max_clients>(demo.cut.Cl.snap.ps.origin, demo.cut.Cl.snap.ps.viewangles, demo.cut.Cl.snap.ps.torsoAnim, camerapos, demoType, demo.cut.Cl.snap.ps.clientNum,demoCurrentTime);
						}

						S3L_Model3D* models = scene3dmodels.data();

						S3L_sceneInit( // Initialize the scene we'll be rendering.
							models,  // This is like an array with only one model in it.
							scene3dmodels.size(),
							&scene);

						scene.camera.transform.translation.x = S3L_POSX(camerapos[1]);
						scene.camera.transform.translation.y = S3L_POSY(camerapos[2]);
						scene.camera.transform.translation.z = S3L_POSZ(camerapos[0]);

						scene.camera.transform.rotation.x = S3L_ROTX(demo.cut.Cl.snap.ps.viewangles[0]);
						scene.camera.transform.rotation.y = S3L_ROTY(demo.cut.Cl.snap.ps.viewangles[1]);
						scene.camera.transform.rotation.z = S3L_ROTZ(demo.cut.Cl.snap.ps.viewangles[2]);

						scene.camera.focalLength = 300;

						// shift the camera a little bit backwards so that it's not inside the cube:

						//scene.camera.transform.translation.z = -2 * S3L_F;

						memset(drawBuffer, 0, sizeof(drawBuffer));

						previousTriangle = -1;
						S3L_newFrame();        // has to be called before each frame
						S3L_drawScene(scene);  /* This starts the scene rendering. The drawPixel
													function will be called to draw it. */
						videoDrawText(demoCurrentTime);

						videoFrames.push_back({ demoCurrentTime,{0} });
						memcpy(videoFrames.back().image, drawBuffer, sizeof(drawBuffer));
					}

				}

				// Record speeds, check sabermove changes and other entity related tracking
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

					thisFrameInfo.entityExists[thisEs->number] = qtrue;

					int tmpItemListGeneralValue = 0; // Not needed for all circumstances, just for 1 place atm (maybe more in future, might forget to update comment)

					int generalizedEntityType = generalizeGameValue<GMAP_ENTITYTYPE, UNSAFE>(thisEs->eType, demoType);


					if (thisEs->number > max_clients && (opts.entityCSVDump == -1  || opts.entityCSVDump == thisEs->number)) {
						entityDumpCSVPoint_t newPoint;
						newPoint.serverTime = demo.cut.Cl.snap.serverTime;
						newPoint.pos = thisEs->pos;
						newPoint.apos = thisEs->apos;
						newPoint.eType = generalizedEntityType;
						newPoint.extraInfo = NULL;
						if (thisEs->solid == SOLID_BMODEL) {
							newPoint.extraInfo = internedStrings.emplace(va("_bmodel%d", thisEs->modelindex)).first->c_str();
						}
						entityDumpCSVDataPoints[thisEs->number].push_back(newPoint);
					}

					// Player related tracking
					if (thisEs->number >= 0 && thisEs->number < max_clients) {

						if (isMOHAADemo) {
							playerTeams[thisEs->number] = getMOHTeam(thisEs);

#ifdef DEBUGSTATSDB
							if (thisEs->number == demo.cut.Cl.snap.ps.clientNum) {

								frameInfoViewModelAnimKey keyHere = { demoType,PLAYER,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[0].index, thisEs->frameInfo[1].index, thisEs->frameInfo[2].index, thisEs->frameInfo[3].index, thisEs->frameInfo[4].index, thisEs->frameInfo[5].index, thisEs->frameInfo[6].index, thisEs->frameInfo[7].index, thisEs->frameInfo[8].index, thisEs->frameInfo[9].index, thisEs->frameInfo[10].index, thisEs->frameInfo[11].index, thisEs->frameInfo[12].index, thisEs->frameInfo[13].index, thisEs->frameInfo[14].index, thisEs->frameInfo[15].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
								frameInfoViewModelAnimCounts[keyHere]++;
								float strongestWeight = -9999.9f;
								int strongestIndex = -1;
								for (int i = 0; i < 16; i++) {
									frameInfoViewModelAnimSimpleKey keySimpleHere = { demoType,PLAYER,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[i].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
									frameInfoViewModelAnimSimpleCounts[keySimpleHere]++;
									if (thisEs->frameInfo[i].weight > strongestWeight) {
										strongestIndex = i;
										strongestWeight = thisEs->frameInfo[i].weight;
									}
								}
								if (strongestIndex != -1) {
									frameInfoViewModelAnimSimpleKey keySimpleHere = { demoType,PLAYER_HIGHWEIGHT,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[strongestIndex].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
									frameInfoViewModelAnimSimpleCounts[keySimpleHere]++;
								}

							}
#endif
						}

						if (opts.strafeCSVSyncPoint && (thisEs->eFlags & getEF_TELEPORTBIT(demoType))) {
							strafeCSVResetPlayer(thisEs->number, opts);
						}

						saberMoveName_t thisEsGeneralSaberMove = (saberMoveName_t)generalizeGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(thisEs->saberMove,demoType);
						animNumberGeneral_t thisEsGeneralLegsAnim = (animNumberGeneral_t)generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(thisEs->legsAnim,demoType);
						animNumberGeneral_t thisEsGeneralTorsoAnim = (animNumberGeneral_t)generalizeGameValue<GMAP_ANIMATIONS, UNSAFE>(thisEs->torsoAnim,demoType);

						// Crouch/Sneak detection
						if (isGeneralizedCrouchAnim(thisEsGeneralLegsAnim, demoType) && thisEs->groundEntityNum == ENTITYNUM_WORLD) {
							groundCrouchDurations[thisEs->number] += deltaTimeFromLastSnapshot;
							if (groundCrouchDurations[thisEs->number] > GROUND_CROUCH_SNEAK_THRESHOLD) {
								lastSneak[thisEs->number] = demoCurrentTime;
								lastSneakDuration[thisEs->number] = groundCrouchDurations[thisEs->number];
							}
						}
						else {
							groundCrouchDurations[thisEs->number] = 0;
						}

						thisFrameInfo.canBlockSimplified[thisEs->number] = WP_SaberCanBlock_Simple(thisEs,demoType);

						float speed = VectorLength(thisEs->pos.trDelta); // Used for strafe analysis buckets and saving recent speeds

						// Strafe precision
						qboolean strafeApplicable = qfalse;
						float strafeDeviation = opts.doStrafeDeviation ? calculateStrafeDeviation(thisEs, &strafeApplicable) : std::numeric_limits<float>::infinity();
						if (strafeApplicable) {
							//strafeDeviations[thisEs->number].averageHelper.sum += strafeDeviation; // Not really needed because we have no way to reliably reset it (since no "Timer started" info). But maybe we can use it in the futuer some time
							//strafeDeviations[thisEs->number].averageHelper.divisor++;

							if (playerDemoStatsPointers[thisEs->number]) {
								playerDemoStatsPointers[thisEs->number]->everUsed = qtrue;
								playerDemoStatsPointers[thisEs->number]->strafeDeviation.sum += strafeDeviation;
								playerDemoStatsPointers[thisEs->number]->strafeDeviation.divisor++;
								for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
									if (speed >= strafeAnalysisBuckets[b].fromIncluding && speed < strafeAnalysisBuckets[b].toExcluding) {
										playerDemoStatsPointers[thisEs->number]->strafeDeviationBuckets[b].sum += strafeDeviation;
										playerDemoStatsPointers[thisEs->number]->strafeDeviationBuckets[b].divisor++;
									}
								}

								// Special strafe value that is only measured when saber is not in any active attack
								if (thisEsGeneralSaberMove >= LS_NONE_GENERAL && thisEsGeneralSaberMove <= LS_PUTAWAY_GENERAL) {

									playerDemoStatsPointers[thisEs->number]->everUsedNoSaberMove = qtrue;
									playerDemoStatsPointers[thisEs->number]->strafeDeviationNoSaberMove.sum += strafeDeviation;
									playerDemoStatsPointers[thisEs->number]->strafeDeviationNoSaberMove.divisor++;
									for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
										if (speed >= strafeAnalysisBuckets[b].fromIncluding && speed < strafeAnalysisBuckets[b].toExcluding) {
											playerDemoStatsPointers[thisEs->number]->strafeDeviationNoSaberMoveBuckets[b].sum += strafeDeviation;
											playerDemoStatsPointers[thisEs->number]->strafeDeviationNoSaberMoveBuckets[b].divisor++;
										}
									}
								}
							}
						}

						thisFrameInfo.commandTime[thisEs->number] = thisEs->pos.trType == TR_LINEAR_STOP ? thisEs->pos.trTime : -1;
						thisFrameInfo.legsAnimGeneral[thisEs->number] = thisEsGeneralLegsAnim;
						thisFrameInfo.torsoAnimGeneral[thisEs->number] = thisEsGeneralTorsoAnim;

						// Backflip detection
						if (
							lastFrameInfo.entityExists[thisEs->number] && (
								(thisEsGeneralLegsAnim != lastFrameInfo.legsAnimGeneral[thisEs->number] && isBackflip<qtrue>(thisEsGeneralLegsAnim, demoType))
							|| (thisEsGeneralTorsoAnim != lastFrameInfo.torsoAnimGeneral[thisEs->number] && isBackflip<qtrue>(thisEsGeneralTorsoAnim, demoType))
							)) {
							lastBackflip[thisEs->number] = demoCurrentTime;
						}
						// Backflip interrupt (touching ground) (don't interrupt if we still happen to be on ground on first frame of backflip?)
						else if (thisEs->groundEntityNum != ENTITYNUM_NONE) {
							lastBackflip[thisEs->number] = -1;
						}

						thisFrameInfo.groundEntityNum[thisEs->number] = thisEs->groundEntityNum;
						VectorCopy(thisEs->pos.trBase, thisFrameInfo.playerPositions[thisEs->number]);
						VectorCopy(thisEs->pos.trDelta, thisFrameInfo.playerVelocities[thisEs->number]);
						VectorCopy(thisEs->apos.trBase, thisFrameInfo.playerAngles[thisEs->number]);
						//thisFrameInfo.playerGSpeeds[thisEs->number] = thisEs->speed;
						//thisFrameInfo.playerMaxWalkSpeed[thisEs->number] = sqrtf(thisEs->speed* thisEs->speed*2);

						if (!(thisEs->eFlags & EF_DEAD)) { // Don't count speeds of dead bodies. They get boosts from dying.
							thisFrameInfo.isAlive[thisEs->number] = qtrue;
							//speeds[thisEs->number][demoCurrentTime] = VectorLength(thisEs->pos.trDelta);
							
							// Speeds and angular speeds/accelerations/jerks
							samplePoint_t angularSpeed{}, angularAcceleration{}, angularJerk{}, angularSnap{};
							if (thisFrameInfo.commandTime[thisEs->number] != -1) { // -1 means probably: g_smoothclients was disabled, therefore we have no info on the command time. Therefore we can't do this analysis for this player.

								float timeSpan = ((double)thisFrameInfo.commandTime[thisEs->number] - (double)lastFrameInfo.commandTime[thisEs->number]) / 1000.0f; // double because float can't hold full precision highest value 32 bit integer i think
								qboolean clientFrameHasAdvanced = (qboolean)(thisFrameInfo.commandTime[thisEs->number] != lastFrameInfo.commandTime[thisEs->number]);
								if (playerVisibleClientFrames[thisEs->number] >= 1) { // We can look back 1 past frame at least
									if (!clientFrameHasAdvanced) {
										// Same command time as last frame. Copy over
										angularSpeed = lastFrameInfo.playerAngularVelocities[thisEs->number];
									}
									else {
										angularSpeed.value = AngleSubtract(lastFrameInfo.playerAngles[thisEs->number][YAW], thisEs->apos.trBase[YAW]) / timeSpan;
										angularSpeed.time = (double)thisFrameInfo.commandTime[thisEs->number] - (double)timeSpan / 2.0;
									}
								}
								if (playerVisibleClientFrames[thisEs->number] >= 2) { // We can look back 2 frames at least
									if (!clientFrameHasAdvanced) {
										// Same command time as last frame. Copy over
										angularAcceleration = lastFrameInfo.playerAngularAccelerations[thisEs->number];
									}
									else {
										if (derivativeRetiming) {
											angularAcceleration.value = (angularSpeed.value - lastFrameInfo.playerAngularVelocities[thisEs->number].value) * 1000.0f / (angularSpeed.time - lastFrameInfo.playerAngularVelocities[thisEs->number].time); // 1000.0f because milliseconds
										}
										else {
											angularAcceleration.value = (angularSpeed.value - lastFrameInfo.playerAngularVelocities[thisEs->number].value) / timeSpan;
										}
										angularAcceleration.time = (angularSpeed.time + lastFrameInfo.playerAngularVelocities[thisEs->number].time) / 2.0;
									}
								}
								if (playerVisibleClientFrames[thisEs->number] >= 3) { // We can look back 3 frames at least. TODO: Does this need to be 4 or more? Not sure hmm. Brain meltink.
									if (!clientFrameHasAdvanced) {
										// Same command time as last frame. Copy over
										angularJerk = lastFrameInfo.playerAngularJerks[thisEs->number];
									}
									else {
										if (derivativeRetiming) {
											angularJerk.value = (angularAcceleration.value - lastFrameInfo.playerAngularAccelerations[thisEs->number].value) * 1000.0f / (angularAcceleration.time - lastFrameInfo.playerAngularAccelerations[thisEs->number].time); // 1000.0f because milliseconds
										}
										else {
											angularJerk.value = (angularAcceleration.value - lastFrameInfo.playerAngularAccelerations[thisEs->number].value) / timeSpan;
										}
										angularJerk.time = (angularAcceleration.time + lastFrameInfo.playerAngularAccelerations[thisEs->number].time) / 2.0;
									}
								}
								if (playerVisibleClientFrames[demo.cut.Cl.snap.ps.clientNum] >= 4) { // We can look back 4 frames at least. TODO: Does this need to be more? Not sure hmm. Brain meltink.
									if (!clientFrameHasAdvanced) {
										// Same command time as last frame. Copy over
										angularSnap = lastFrameInfo.playerAngularSnaps[thisEs->number];
									}
									else {
										if (derivativeRetiming) {
											angularSnap.value = (angularJerk.value - lastFrameInfo.playerAngularJerks[thisEs->number].value) * 1000.0f / (angularJerk.time - lastFrameInfo.playerAngularJerks[thisEs->number].time); // 1000.0f because milliseconds
										}
										else {
											angularSnap.value = (angularJerk.value - lastFrameInfo.playerAngularJerks[thisEs->number].value) / timeSpan;
										}
										angularSnap.time = (angularJerk.time + lastFrameInfo.playerAngularJerks[thisEs->number].time) / 2.0;
									}
								}
							}
							thisFrameInfo.playerAngularVelocities[thisEs->number] = angularSpeed;
							thisFrameInfo.playerAngularAccelerations[thisEs->number] = angularAcceleration;
							thisFrameInfo.playerAngularJerks[thisEs->number] = angularJerk;
							thisFrameInfo.playerAngularSnaps[thisEs->number] = angularSnap;
							speeds[thisEs->number].push_back({ demoCurrentTime,speed, angularSpeed.value,angularAcceleration.value,angularJerk.value,angularSnap.value });

							// Is this client walking?
							if (thisEs->groundEntityNum != ENTITYNUM_NONE && VectorLength(thisEs->pos.trDelta) < sqrtf(thisEs->speed * thisEs->speed * 2)) {
								// TODO: better logic here. Im naively assuming that maximum theoretical walking speed equals sqrt(g_speed*g_speed+g_speed*g_speed). This is probably not entirely correct although the ballpark seems alright.
								walkDetectedTimes[thisEs->number].push_back(demoCurrentTime);
							}

							thisFrameInfo.movementDir[thisEs->number] = thisEs->angles2[YAW];

							// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
							thisFrameInfo.saberMoveGeneral[thisEs->number] = thisEsGeneralSaberMove;
							if (playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMoveGeneral != thisEsGeneralSaberMove) {
								if (thisEsGeneralSaberMove >= LS_PARRY_UP_GENERAL && thisEsGeneralSaberMove <= LS_PARRY_LL_GENERAL) {
									hitDetectionData[thisEs->number].newParryDetected = qtrue;
								}
								for (int smI = MAX_PAST_SABERMOVE_SAVE -1; smI > 0; smI--) {
									playerLastSaberMove[thisEs->number].lastSaberMove[smI] = playerLastSaberMove[thisEs->number].lastSaberMove[smI-1];
								}
								playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMoveChange = demoCurrentTime;
								playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMoveGeneral= thisEsGeneralSaberMove;
								playerLastSaberMove[thisEs->number].lastSaberMove[0].speed= speed;
							}
						}
					}
					// Track trip mine owners and the first time they were seen.
					// Careful: The current way it is handled is blocked if the mine is temporarily not visible. The detection will think it appeared later.
					else if (generalizedEntityType == ET_GENERAL_GENERAL && generalizeGameValue<GMAP_WEAPONS,SAFE>( thisEs->weapon,demoType) == WP_TRIP_MINE_GENERAL && (thisEs->eFlags & getEF_MISSILE_STICK(demoType))) { // tripmine
						thisFrameInfo.entityOwnerInfo[thisEs->number].owner = thisEs->genericenemyindex - 1024;
						thisFrameInfo.entityOwnerInfo[thisEs->number].type = TET_TRIPMINE;
						if ((generalizeGameValue<GMAP_EVENTS, UNSAFE>(thisEs->event,demoType) & ~EV_EVENT_BITS) == EV_MISSILE_MISS_GENERAL) {
							// This mine is exploding right now
							thisFrameInfo.entityOwnerInfo[thisEs->number].flags |= TETFLAG_EXPLODED;
						}
						if (thisEs->pos.trType == TR_GRAVITY) {
							// This mine is airborne
							thisFrameInfo.entityOwnerInfo[thisEs->number].flags |= TETFLAG_AIRBORNE;
						}
						/*if (lastFrameInfo.entityExists[thisEs->number] &&
							thisFrameInfo.entityOwnerInfo[thisEs->number].type == lastFrameInfo.entityOwnerInfo[thisEs->number].type && 
							thisFrameInfo.entityOwnerInfo[thisEs->number].owner == lastFrameInfo.entityOwnerInfo[thisEs->number].owner) {

							thisFrameInfo.entityOwnerInfo[thisEs->number].firstSeen = lastFrameInfo.entityOwnerInfo[thisEs->number].firstSeen;
						}
						else {
							thisFrameInfo.entityOwnerInfo[thisEs->number].firstSeen = demoCurrentTime;
						}*/
						TET_LastSeenUpdate(thisEs->number, demoCurrentTime);
					}
					// Track sentries
					else if (generalizedEntityType == ET_GENERAL_GENERAL && thisEs->modelindex == sentryModelIndex && sentryModelIndex != -1) {
						thisFrameInfo.entityOwnerInfo[thisEs->number].owner = thisEs->owner;
						thisFrameInfo.entityOwnerInfo[thisEs->number].type = TET_SENTRY;
						if (thisEs->pos.trType == TR_GRAVITY) {
							// This sentry is airborne (I hope!)
							thisFrameInfo.entityOwnerInfo[thisEs->number].flags |= TETFLAG_AIRBORNE;
						}
						TET_LastSeenUpdate(thisEs->number,demoCurrentTime);
					}
					// Track force fields
					else if (generalizedEntityType == ET_SPECIAL_GENERAL && thisEs->modelindex == HI_SHIELD) {
						thisFrameInfo.entityOwnerInfo[thisEs->number].owner = thisEs->owner;
						thisFrameInfo.entityOwnerInfo[thisEs->number].type = TET_FORCEFIELD;
						TET_LastSeenUpdate(thisEs->number,demoCurrentTime);
					}
					// Track dropped flags and flags on post
					else if ((demoType < DM_25 || demoType > DM_26_XBOX) && // We can't do this rn in JKA because JKA doesn't set EF_BOUNCE_HALF for dropped flags, so we can't distinguish dropped and normal flags. so would get misdetections
						generalizedEntityType == ET_ITEM_GENERAL /* && thisEs->modelindex*/ &&
						(tmpItemListGeneralValue = generalizeGameValue<GMAP_ITEMLIST, SAFE>(thisEs->modelindex, demoType)) &&
						tmpItemListGeneralValue >= ITEMLIST_TEAM_CTF_REDFLAG_GENERAL &&
						tmpItemListGeneralValue <= ITEMLIST_TEAM_CTF_NEUTRALFLAG_GENERAL) {
						
						bool isDroppedFlag = thisEs->eFlags & EF_BOUNCE_HALF; // This does NOT work in JKA. EF_BOUNCE_HALF doesn't even exist there. It does work in Q3. (or ought to)
						entityState_t** relevantArray = isDroppedFlag ? droppedFlagEntities : baseFlagEntities;
						team_t flagTeam;
						switch (tmpItemListGeneralValue) {
							case ITEMLIST_TEAM_CTF_REDFLAG_GENERAL:
								flagTeam = TEAM_RED;
								break;
							case ITEMLIST_TEAM_CTF_BLUEFLAG_GENERAL:
								flagTeam = TEAM_BLUE;
								break;
							case ITEMLIST_TEAM_CTF_NEUTRALFLAG_GENERAL:
								flagTeam = TEAM_FREE;
								break;
						}
						relevantArray[(int)flagTeam] = thisEs;
						if (!isDroppedFlag) {
							baseFlagPositions[flagTeam].known = qtrue;
							VectorCopy(thisEs->pos.trBase, baseFlagPositions[flagTeam].pos);
						}
					}
					else if (isMOHAADemo && thisEs->parent > 0 && thisEs->parent < max_clients && generalizedEntityType == ET_ITEM_GENERAL && (thisEs->eFlags & EF_SENTIENT_MOH)/* && && (thisEs->eFlags & EF_WEAPON_MOH)*/) { // Don't ask me why weapons have EF_SENTIENT and not EF_WEAPON...
						
						//static int tagNumUsage[1000]{};
						//tagNumUsage[thisEs->tag_num]++;
						// Magic tag numbers. Seem to be the ones for main held weapon. 22 and 55. Used by far the most.
						// There is others that happen to be used sometimes like 14 for snipers. I'm guessing that means they're strapped onto someone's back or sth because the snipers can also have 22/55
						// General observed usage (with sample count as example): 
						// 
						// 3 (47), 5(400), 6(191), 14 (11555), 22 (347807), 23 (11), 29 (6024), 40 (1243), 52 (90908)
						if(thisEs->tag_num == 52 || thisEs->tag_num == 22){

#ifdef DEBUGSTATSDB
							if (thisEs->parent == demo.cut.Cl.snap.ps.clientNum) {
								frameInfoViewModelAnimKey keyHere = { demoType,WEAPON,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[0].index, thisEs->frameInfo[1].index, thisEs->frameInfo[2].index, thisEs->frameInfo[3].index, thisEs->frameInfo[4].index, thisEs->frameInfo[5].index, thisEs->frameInfo[6].index, thisEs->frameInfo[7].index, thisEs->frameInfo[8].index, thisEs->frameInfo[9].index, thisEs->frameInfo[10].index, thisEs->frameInfo[11].index, thisEs->frameInfo[12].index, thisEs->frameInfo[13].index, thisEs->frameInfo[14].index, thisEs->frameInfo[15].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
								frameInfoViewModelAnimCounts[keyHere]++;
								float strongestWeight = -9999.9f;
								int strongestIndex = -1;
								for (int i = 0; i < 16; i++) {
									frameInfoViewModelAnimSimpleKey keySimpleHere = { demoType,WEAPON,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[i].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
									frameInfoViewModelAnimSimpleCounts[keySimpleHere]++;
									if (thisEs->frameInfo[i].weight > strongestWeight) {
										strongestIndex = i;
										strongestWeight = thisEs->frameInfo[i].weight;
									}
								}
								if (strongestIndex != -1) {
									frameInfoViewModelAnimSimpleKey keySimpleHere = { demoType,WEAPON_HIGHWEIGHT,demo.cut.Cl.snap.ps.iNetViewModelAnim,thisEs->frameInfo[strongestIndex].index };  // demoVersion,type,viewModelAnim,frameInfo0Index,frameInfo1Index,frameInfo2Index,frameInfo3Index,frameInfo4Index,frameInfo5Index,frameInfo6Index,frameInfo7Index,frameInfo8Index,frameInfo9Index,frameInfo10Index,frameInfo11Index,frameInfo12Index,frameInfo13Index,frameInfo14Index,frameInfo15Index
									frameInfoViewModelAnimSimpleCounts[keySimpleHere]++;
								}
							}
#endif

							int configStringBaseIndex = getCS_MODELS(demoType); //(demoType == DM_26 || demoType == DM_25) ? CS_MODELS_JKA : CS_MODELS;
							if (mohaaPlayerWeaponModelIndexThisFrame[thisEs->parent] != -1) {
								int offset = demo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + mohaaPlayerWeaponModelIndexThisFrame[thisEs->parent]];
								char* modelName2 = demo.cut.Cl.gameState.stringData + offset;
								offset = demo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + thisEs->modelindex];
								char* modelName = demo.cut.Cl.gameState.stringData + offset;
								std::cerr << "Weird. MOHAA weapon for player found twice. Duplicate? " << modelName << " vs. " << modelName2 << " (" << DPrintFLocation << ")\n";
							}
							if (thisEs->modelindex != mohaaPlayerWeaponModelIndex[thisEs->parent]) {
								// Model changed. Update weapons array.

								// Detecting weapons players are carrying
								int offset = demo.cut.Cl.gameState.stringOffsets[configStringBaseIndex + thisEs->modelindex];
								//if (maxLength) *maxLength = sizeof(thisDemo.cut.Cl.gameState.stringData) - offset;
								std::string weaponModel = demo.cut.Cl.gameState.stringData + offset;
								auto foundModel = mohaaWeaponModelMap.find(weaponModel);
								if (foundModel != mohaaWeaponModelMap.end()) {
									mohaaPlayerWeapon[thisEs->parent] = generalizeGameValue<GMAP_WEAPONS, UNSAFE>(foundModel->second, demoType);
								}
								else {
									mohaaPlayerWeapon[thisEs->parent] = WP_NONE_GENERAL;
								}
								
							}
							mohaaPlayerWeaponModelIndexThisFrame[thisEs->parent] = thisEs->modelindex;
							mohaaPlayerWeaponModelIndex[thisEs->parent] = thisEs->modelindex;
						}

					}
				
				}

				// 
				// Playerstate tracking
				{
					// Crouch/Sneak detection
					if (isGeneralizedCrouchAnim((animNumberGeneral_t)psGeneralLegsAnim, demoType) && demo.cut.Cl.snap.ps.groundEntityNum == ENTITYNUM_WORLD) {
						groundCrouchDurations[demo.cut.Cl.snap.ps.clientNum] += deltaTimeFromLastSnapshot;
						if (groundCrouchDurations[demo.cut.Cl.snap.ps.clientNum] > GROUND_CROUCH_SNEAK_THRESHOLD) {
							lastSneak[demo.cut.Cl.snap.ps.clientNum] = demoCurrentTime;
							lastSneakDuration[demo.cut.Cl.snap.ps.clientNum] = groundCrouchDurations[demo.cut.Cl.snap.ps.clientNum];
						}
					}
					else {
						groundCrouchDurations[demo.cut.Cl.snap.ps.clientNum] = 0;
					}

					if (opts.strafeCSVSyncPoint && (demo.cut.Cl.snap.ps.eFlags & getEF_TELEPORTBIT(demoType))) {
						strafeCSVResetPlayer(demo.cut.Cl.snap.ps.clientNum, opts);
					}

					float speed = VectorLength(demo.cut.Cl.snap.ps.velocity);

					playerStateStrafeDeviationThisFrame = opts.doStrafeDeviation ? calculateStrafeDeviation(&demo.cut.Cl.snap.ps, &strafeApplicablePlayerStateThisFrame) : std::numeric_limits<float>::infinity();
					if (strafeApplicablePlayerStateThisFrame) {
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.divisor++;
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.sum += playerStateStrafeDeviationThisFrame;
						if (playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]) {
							playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->everUsed = qtrue;
							playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviation.sum += playerStateStrafeDeviationThisFrame;
							playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviation.divisor++;
							for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
								if (speed >= strafeAnalysisBuckets[b].fromIncluding && speed < strafeAnalysisBuckets[b].toExcluding) {
									playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationBuckets[b].sum += playerStateStrafeDeviationThisFrame;
									playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationBuckets[b].divisor++;
								}
							}

							// Special strafe value that is only measured when saber is not in any active attack
							if (psGeneralSaberMove >= LS_NONE_GENERAL && psGeneralSaberMove <= LS_PUTAWAY_GENERAL) {

								playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->everUsedNoSaberMove = qtrue;
								playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationNoSaberMove.sum += playerStateStrafeDeviationThisFrame;
								playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationNoSaberMove.divisor++;
								for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
									if (speed >= strafeAnalysisBuckets[b].fromIncluding && speed < strafeAnalysisBuckets[b].toExcluding) {
										playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationNoSaberMoveBuckets[b].sum += playerStateStrafeDeviationThisFrame;
										playerDemoStatsPointers[demo.cut.Cl.snap.ps.clientNum]->strafeDeviationNoSaberMoveBuckets[b].divisor++;
									}
								}
							}
						}
					}

					thisFrameInfo.duelTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.duelTime;
					thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.commandTime;
					thisFrameInfo.pmFlags[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.pm_flags;
					thisFrameInfo.jumpzstart[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.fd.forceJumpZStart;
					thisFrameInfo.legsAnimGeneral[demo.cut.Cl.snap.ps.clientNum] = psGeneralLegsAnim;
					thisFrameInfo.torsoAnimGeneral[demo.cut.Cl.snap.ps.clientNum] = psGeneralTorsoAnim;

					// jumpbug detection
					if (opts.findjumpbugs && (lastFrameInfo.pmFlags[demo.cut.Cl.snap.ps.clientNum] & PMF_JUMP_HELD) && (thisFrameInfo.pmFlags[demo.cut.Cl.snap.ps.clientNum] & PMF_JUMP_HELD) && lastFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum][2] < 0 && demo.cut.Cl.snap.ps.velocity[2] > 0 && demo.cut.Cl.snap.ps.groundEntityNum == ENTITYNUM_NONE && lastFrameInfo.groundEntityNum[demo.cut.Cl.snap.ps.clientNum] == ENTITYNUM_NONE && lastGroundHeight[demo.cut.Cl.snap.ps.clientNum] > demo.cut.Cl.snap.ps.origin[2]+500.0f && lastFrameInfo.jumpzstart[demo.cut.Cl.snap.ps.clientNum] == demo.cut.Cl.snap.ps.fd.forceJumpZStart) { // checking for demo.cut.Cl.snap.ps.fd.forceJumpZStart not being 0 is a bit cringe but idk what else to do
						//std::cout << "hehe found\n";
						int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + demo.cut.Cl.snap.ps.clientNum];
						const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
						const char* playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
						logSpecialThing("JUMPBUGSEARCH", "", "", playerName, demo.cut.Cl.snap.ps.clientNum, -1, demoCurrentTime, 0, bufferTime, lastGameStateChangeInDemoTime, io, &sharedVars.oldBasename, &sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
					}

					// Backflip detection
					if (
						lastFrameInfo.entityExists[demo.cut.Cl.snap.ps.clientNum] && (
							(psGeneralLegsAnim != lastFrameInfo.legsAnimGeneral[demo.cut.Cl.snap.ps.clientNum] && isBackflip<qtrue>(psGeneralLegsAnim, demoType))
							|| (psGeneralTorsoAnim != lastFrameInfo.torsoAnimGeneral[demo.cut.Cl.snap.ps.clientNum] && isBackflip<qtrue>(psGeneralTorsoAnim, demoType))
							)) {
						lastBackflip[demo.cut.Cl.snap.ps.clientNum] = demoCurrentTime;
					}
					// Backflip interrupt (touching ground) (don't interrupt if we still happen to be on ground on first frame of backflip?)
					else if (demo.cut.Cl.snap.ps.groundEntityNum != ENTITYNUM_NONE) {
						lastBackflip[demo.cut.Cl.snap.ps.clientNum] = -1;
					}

					thisFrameInfo.groundEntityNum[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.groundEntityNum;
					thisFrameInfo.psStats12[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.stats[12]; // q3 defrag run tracking
					VectorCopy(demo.cut.Cl.snap.ps.origin, thisFrameInfo.playerPositions[demo.cut.Cl.snap.ps.clientNum]);
					VectorCopy(demo.cut.Cl.snap.ps.velocity, thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]);
					VectorCopy(demo.cut.Cl.snap.ps.viewangles, thisFrameInfo.playerAngles[demo.cut.Cl.snap.ps.clientNum]);
					//thisFrameInfo.playerGSpeeds[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.speed;
					//thisFrameInfo.playerMaxWalkSpeed[demo.cut.Cl.snap.ps.clientNum] = sqrtf(demo.cut.Cl.snap.ps.speed * demo.cut.Cl.snap.ps.speed * 2);
					thisFrameInfo.entityExists[demo.cut.Cl.snap.ps.clientNum] = qtrue;
					thisFrameInfo.canBlockSimplified[demo.cut.Cl.snap.ps.clientNum] = WP_SaberCanBlock_Simple(&demo.cut.Cl.snap.ps, demoType);

					if (psGeneralPMType != PM_DEAD_GENERAL && demo.cut.Cl.snap.ps.stats[STAT_HEALTH] > 0) {
						thisFrameInfo.isAlive[demo.cut.Cl.snap.ps.clientNum] = qtrue;
						//speeds[demo.cut.Cl.snap.ps.clientNum][demoCurrentTime] = VectorLength(demo.cut.Cl.snap.ps.velocity);
						
						samplePoint_t angularSpeed{}, angularAcceleration{}, angularJerk{}, angularSnap{};
						float timeSpan = ((double)thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] - (double)lastFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum]) / 1000.0f; // double because commandtime could be a full integer value which 32 bit floating point cant represent accurately.
						qboolean clientFrameHasAdvanced = (qboolean)(thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] != lastFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum]);
						if (playerVisibleClientFrames[demo.cut.Cl.snap.ps.clientNum] >= 1) { // We can look back 1 past frame at least
							if (!clientFrameHasAdvanced) {
								// Same command time as last frame. Copy over
								angularSpeed = lastFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum];
							}
							else {
								angularSpeed.value = AngleSubtract(lastFrameInfo.playerAngles[demo.cut.Cl.snap.ps.clientNum][YAW], demo.cut.Cl.snap.ps.viewangles[YAW]) / timeSpan;
								angularSpeed.time = (double)thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] - (double)timeSpan / 2.0;
							}
						}
						if (playerVisibleClientFrames[demo.cut.Cl.snap.ps.clientNum] >= 2) { // We can look back 2 frames at least
							if (!clientFrameHasAdvanced) {
								// Same command time as last frame. Copy over
								angularAcceleration = lastFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum];
							}
							else {
								if (derivativeRetiming) {
									angularAcceleration.value = (angularSpeed.value - lastFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum].value) * 1000.0f / (angularSpeed.time - lastFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum].time); // 1000.0f because milliseconds
								}
								else {
									angularAcceleration.value = (angularSpeed.value - lastFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum].value) / timeSpan;
								}
								angularAcceleration.time = (angularSpeed.time + lastFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum].time) / 2.0;
							}
						}
						if (playerVisibleClientFrames[demo.cut.Cl.snap.ps.clientNum] >= 3) { // We can look back 3 frames at least. TODO: Does this need to be 4 or more? Not sure hmm. Brain meltink.
							if (!clientFrameHasAdvanced) {
								// Same command time as last frame. Copy over
								angularJerk = lastFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum];
							}
							else {
								if (derivativeRetiming) {
									angularJerk.value = (angularAcceleration.value - lastFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum].value) * 1000.0f / (angularAcceleration.time - lastFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum].time); // 1000.0f because milliseconds
								}
								else {
									angularJerk.value = (angularAcceleration.value - lastFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum].value) / timeSpan;
								}
								angularJerk.time = (angularAcceleration.time + lastFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum].time) / 2.0;
							}
						}
						if (playerVisibleClientFrames[demo.cut.Cl.snap.ps.clientNum] >= 4) { // We can look back 4 frames at least. TODO: Does this need to be more? Not sure hmm. Brain meltink.
							if (!clientFrameHasAdvanced) {
								// Same command time as last frame. Copy over
								angularSnap = lastFrameInfo.playerAngularSnaps[demo.cut.Cl.snap.ps.clientNum];
							}
							else {
								if (derivativeRetiming) {
									angularSnap.value = (angularJerk.value - lastFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum].value) * 1000.0f / (angularJerk.time - lastFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum].time); // 1000.0f because milliseconds
								}
								else {
									angularSnap.value = (angularJerk.value - lastFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum].value) / timeSpan;
								}
								angularSnap.time = (angularJerk.time + lastFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum].time) / 2.0;
							}
						}
						thisFrameInfo.playerAngularVelocities[demo.cut.Cl.snap.ps.clientNum] = angularSpeed;
						thisFrameInfo.playerAngularAccelerations[demo.cut.Cl.snap.ps.clientNum] = angularAcceleration;
						thisFrameInfo.playerAngularJerks[demo.cut.Cl.snap.ps.clientNum] = angularJerk;
						thisFrameInfo.playerAngularSnaps[demo.cut.Cl.snap.ps.clientNum] = angularSnap;
						speeds[demo.cut.Cl.snap.ps.clientNum].push_back({ demoCurrentTime,speed, angularSpeed.value,angularAcceleration.value,angularJerk.value,angularSnap.value });


						// Is this client walking?
						if (demo.cut.Cl.snap.ps.groundEntityNum != ENTITYNUM_NONE && VectorLength(demo.cut.Cl.snap.ps.velocity) < sqrtf(demo.cut.Cl.snap.ps.speed * demo.cut.Cl.snap.ps.speed * 2)) {
							// TODO: better logic here. Im naively assuming that maximum theoretical walking speed equals sqrt(g_speed*g_speed+g_speed*g_speed). This is probably not entirely correct although the ballpark seems alright.
							walkDetectedTimes[demo.cut.Cl.snap.ps.clientNum].push_back(demoCurrentTime);
						}

#ifdef PLAYERSTATEOTHERKILLERBOOSTDETECTION
						thisFrameInfo.otherKillerTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.otherKillerTime;
						thisFrameInfo.otherKillerValue[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.otherKiller;
#endif
						thisFrameInfo.movementDir[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.movementDir;

						// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
						thisFrameInfo.saberMoveGeneral[demo.cut.Cl.snap.ps.clientNum] = psGeneralSaberMove;
						if (playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMoveGeneral != psGeneralSaberMove) {
							if (psGeneralSaberMove >= LS_PARRY_UP_GENERAL && psGeneralSaberMove <= LS_PARRY_LL_GENERAL) {
								hitDetectionData[demo.cut.Cl.snap.ps.clientNum].newParryDetected = qtrue;
							}
							for (int smI = MAX_PAST_SABERMOVE_SAVE - 1; smI > 0; smI--) {
								playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[smI] = playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[smI - 1];
							}
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMoveChange = demoCurrentTime;
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMoveGeneral = psGeneralSaberMove;
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].speed = speed;
						}
					}

					if (opts.teleportAnalysis == 1 && (opts.teleportAnalysisEndDemoTime > demoCurrentTime || !opts.teleportAnalysisEndDemoTime) ) {
						static int oldTeleportBit = -1;
						int currentTeleportBit = (demo.cut.Cl.snap.ps.eFlags & getEF_TELEPORTBIT(demoType));
						if (oldTeleportBit == -1) {
							oldTeleportBit = currentTeleportBit;
						}
						if (currentTeleportBit != oldTeleportBit) {
							teleportInfo_t teleport;
							teleport.demoTime = demoCurrentTime;
							VectorCopy(demo.cut.Cl.snap.ps.origin, teleport.position);
							teleports.push_back(teleport);
							oldTeleportBit = currentTeleportBit;
						}
					}
					else if (opts.teleportAnalysis == 2) { // just save location on every frame.
						teleportInfo_t teleport;
						teleport.demoTime = demoCurrentTime;
						VectorCopy(demo.cut.Cl.snap.ps.origin, teleport.position);
						teleports.push_back(teleport);
					}
				}

				

				// Playerstate boost detection
				// If otherKiller and otherKillerTime in PS has changed from last frame, this player is boosted. Add the boost to his list.
				{
					qboolean playerStateBoostDetected = qfalse;
					//thisFrameInfo.pmFlagKnockback[demo.cut.Cl.snap.ps.clientNum] = (qboolean)!!(demo.cut.Cl.snap.ps.pm_flags & PMF_TIME_KNOCKBACK);
					thisFrameInfo.pmFlagTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.pm_time;
					thisFrameInfo.psTeleportBit[demo.cut.Cl.snap.ps.clientNum] = (qboolean)!!(demo.cut.Cl.snap.ps.eFlags & getEF_TELEPORTBIT(demoType));

#ifdef PLAYERSTATEOTHERKILLERBOOSTDETECTION
					if (thisFrameInfo.otherKillerTime[demo.cut.Cl.snap.ps.clientNum] != lastFrameInfo.otherKillerTime[demo.cut.Cl.snap.ps.clientNum]
						|| thisFrameInfo.otherKillerValue[demo.cut.Cl.snap.ps.clientNum] != lastFrameInfo.otherKillerValue[demo.cut.Cl.snap.ps.clientNum]
						) {
						
						playerStateBoostDetected = qtrue;
						
						boost_t newBoost;
						VectorCopy(demo.cut.Cl.snap.ps.origin, newBoost.startPosition);
						VectorSubtract(demo.cut.Cl.snap.ps.velocity, lastFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum], newBoost.velocityDeltaVector);
						newBoost.boosterClientNum = (demo.cut.Cl.snap.ps.otherKiller >= 0 && demo.cut.Cl.snap.ps.otherKiller < max_clients) ?demo.cut.Cl.snap.ps.otherKiller:-1;
						newBoost.boostedClientNum = demo.cut.Cl.snap.ps.clientNum;
						newBoost.demoTime = demoCurrentTime;
						newBoost.detectType = BOOST_PLAYERSTATE;
						//newBoost.confirmed = (qboolean)(VectorLength(thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]) > BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY);
						newBoost.autoSetMinimumTravelDistance(VectorLength(thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]));


						// Determine teams of both
						if (newBoost.boosterClientNum != -1) {

							int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + newBoost.boosterClientNum];
							const char * playerInfo = demo.cut.Cl.gameState.stringData + offset;
							int boosterTeam = atoi(Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "t"));
							offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + newBoost.boostedClientNum];
							playerInfo = demo.cut.Cl.gameState.stringData + offset;
							int boostedTeam = atoi(Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "t"));
							newBoost.isEnemyBoost = boosterTeam == boostedTeam ? 1 : 0;
						}
						else {
							newBoost.isEnemyBoost = -1;
						}
						boosts.push_back(newBoost);
					}
#else
					int psCommandTimeDelta = thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] - lastFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum];
					if (thisFrameInfo.psTeleportBit[demo.cut.Cl.snap.ps.clientNum] == lastFrameInfo.psTeleportBit[demo.cut.Cl.snap.ps.clientNum] &&   // Respawn/teleport also creates knockback. But we're not looking for that.
						//thisFrameInfo.pmFlagKnockback[demo.cut.Cl.snap.ps.clientNum]
						(thisFrameInfo.pmFlags[demo.cut.Cl.snap.ps.clientNum] & PMF_TIME_KNOCKBACK)
						&& thisFrameInfo.pmFlagTime[demo.cut.Cl.snap.ps.clientNum] != (lastFrameInfo.pmFlagTime[demo.cut.Cl.snap.ps.clientNum]-(psCommandTimeDelta))
						) {
						
						qboolean horizontalSpeedAboveWalking = (qboolean)(VectorLength2(demo.cut.Cl.snap.ps.velocity) > sqrtf(demo.cut.Cl.snap.ps.speed * demo.cut.Cl.snap.ps.speed * 2));
						float verticalSpeedDelta = demo.cut.Cl.snap.ps.velocity[2] - lastFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum][2];

						float maximumGravityCausedDelta = -demo.cut.Cl.snap.ps.gravity * (float)psCommandTimeDelta * 0.001f -10.0f; // I subtract 10 additionally to account for fps-based weirdness of gravity affecting velocity due to rounding. Hopefully 10 is enough. I guess rounding could cause maximum 0.9999 difference per frame. So 10 should account for up to 10 frames?


						if (horizontalSpeedAboveWalking // If the boost didn't at least raise us above walking speed, just ignore it. Or we will be tracking completely useless micro boosts like getting hit by a turret in some corner.
							|| verticalSpeedDelta > BOOST_PS_VERTICAL_SPEED_DELTA_DETECT_THRESHOLD // Something like this could also be caused by a jump so it can't necessarily be generalized but we do know knockback has occurred so it's "somewhat" safe or rather: low-ish but not perfect probability of misdetect.
							|| verticalSpeedDelta < (maximumGravityCausedDelta- BOOST_PS_VERTICAL_SPEED_DELTA_DETECT_THRESHOLD)
							// TODO: Separate handling for vertical and horizontal
							// && VectorLength(demo.cut.Cl.snap.ps.velocity) > VectorLength(lastFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]) // If the boost didn't actually increase our effective speed, don't consider it a boost (it's more of a brake I suppose).
							) { 

							playerStateBoostDetected = qtrue;

							boost_t newBoost;
							VectorCopy(demo.cut.Cl.snap.ps.origin,newBoost.startPosition);
							VectorSubtract(demo.cut.Cl.snap.ps.velocity, lastFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum],newBoost.velocityDeltaVector);
							newBoost.boosterClientNum = (demo.cut.Cl.snap.ps.persistant[PERS_ATTACKER] >= 0 && demo.cut.Cl.snap.ps.persistant[PERS_ATTACKER] < max_clients) ? demo.cut.Cl.snap.ps.persistant[PERS_ATTACKER] : -1;
							newBoost.boostedClientNum = demo.cut.Cl.snap.ps.clientNum;
							newBoost.demoTime = demoCurrentTime;
							newBoost.detectType = BOOST_PLAYERSTATE_KNOCKBACK_FLAG;
							//newBoost.confirmed = (qboolean)(VectorLength(thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]) > BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY);
							newBoost.autoSetMinimumTravelDistance(VectorLength(thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]));

							// 
							// Estimated strength calculation:
							// From JK2 game code: VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
							// pm_time is actually knockback * 2, mass is hardcoded to 200 and g_knockback default is 1000 so simplified:
							// 1000 * knockback / 200 = 5*knockback = 5*(knockbacktime/2) = 2.5f*knockbacktime.
							// 
							// Note: Any pm_time value over 200 is limited to 200 and any value under 50 is limited to 50. So it is not possible to detect a boost strength over 2.5*200 = 500 or under 2.5*50 = 125.
							//
							newBoost.estimatedStrength = 2.5f * (float)thisFrameInfo.pmFlagTime[demo.cut.Cl.snap.ps.clientNum]; // The knockback time is damage * 2, but capped to 200, so a maximum damage of 100 can be guessed. TODO: If it's enemy damage, just get it from health/armor info. However it could also be self-inflicted and have a multiplier, complicating things.

							if (newBoost.boosterClientNum != -1) {

								int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + newBoost.boosterClientNum];
								const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
								int boosterTeam = atoi(Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "t"));
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + newBoost.boostedClientNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								int boostedTeam = atoi(Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "t"));
								newBoost.isEnemyBoost = boosterTeam != boostedTeam ? 1 : 0;
							}
							else {
								newBoost.isEnemyBoost = -1;
							}

							boosts.push_back(newBoost);
						}
					}
#endif

					// THE BIG MESS: Entity boost detection
					//  
					// VERTICAL:
					// We wanna avoid misdetecting simple jumps as boosts. So we need to know the maximum possible velocity delta that can be caused by jumps
					// From bg_misc.c: bgForcePowerCost[NUM_FORCE_POWERS][NUM_FORCE_POWER_LEVELS] -	{	0,	0,	2,	6	},	// Jump			//FP_LEVITATION,//hold/duration
					// So cost for maximum jump force is: 8 points. Depending on maxForceRank players get: 0:0, 1:5, 2:10. So a maxForceRank of 2 is enough to buy full jump force.
					// In general, we need:
					// g_maxForceRank (see the maximum allowed jump height on server if force jump is enabled)
					// g_forcePowerDisable (see if force jump is enabled at all)
					//
					for (int i = 0; i < max_clients; i++) {
						if (i == demo.cut.Cl.snap.ps.clientNum && playerStateBoostDetected) continue; // Playerstate boosts are processed separately and we want to avoid dupes. However sometimes the playerstate detection doesn't properly work so if nothing was detected in playerstate, we will use this as a backup.

						// TODO: Make it work with boosts that happen on the exact frame spectator switches angle?
						if (thisFrameInfo.entityExists[i] && lastFrameInfo.entityExists[i] && thisFrameInfo.isAlive[i] && lastFrameInfo.isAlive[i]) { // If we are dead it doesn't count as boost. That's just our corpse getting a speed bump

							boost_t newBoost;

							qboolean isBoost = qfalse;
							float horizontalDelta = VectorLength2(thisFrameInfo.playerVelocities[i]) - VectorLength2(lastFrameInfo.playerVelocities[i]);
							float maximumWalkSpeed = sqrtf(demo.cut.Cl.snap.ps.speed * demo.cut.Cl.snap.ps.speed * 2);
							float horizontalSpeed = VectorLength2(thisFrameInfo.playerVelocities[i]);
							//qboolean horizontalSpeedAboveWalking = (qboolean)(horizontalSpeed > maximumWalkSpeed); // TODO: use entity speed here
							float verticalDelta = thisFrameInfo.playerVelocities[i][2] - lastFrameInfo.playerVelocities[i][2];
							float verticalDeltaNormalized = verticalDelta <= 0 ? verticalDelta : std::min(thisFrameInfo.playerVelocities[i][2], verticalDelta); // If positive, we wanna avoid overly high numbers resulting from bunny hopping (because the negative falling velocity transitioning to jump delta is much higher than the jump delta itself)
							VectorCopy(thisFrameInfo.playerPositions[i], newBoost.startPosition);
							VectorSubtract(thisFrameInfo.playerVelocities[i], lastFrameInfo.playerVelocities[i], newBoost.velocityDeltaVector);
							
							qboolean downMoreThanGravityAllows = qfalse;
							// If we kno command time, we can check if the downward acceleration is more than gravity would allow
							if (thisFrameInfo.commandTime[i] != -1 && lastFrameInfo.commandTime[i] != -1) {
								int entityCommandTimeDelta = thisFrameInfo.commandTime[i] - lastFrameInfo.commandTime[i];
								float maximumGravityCausedDelta = -demo.cut.Cl.snap.ps.gravity * (float)entityCommandTimeDelta * 0.001f - 10.0f; // TODO: Make it use gravity of this ent?
								if (verticalDelta < (maximumGravityCausedDelta - BOOST_PS_VERTICAL_SPEED_DELTA_DETECT_THRESHOLD)
									&& thisFrameInfo.playerVelocities[i][2] < maximumGravityCausedDelta // If we hit a wall above us, that would reduce our speed to 0 immediately. So make sure our vertical speed is more negative than that would indicate
									) {
									// TODO: This doesn't take into account any possible bounce when hitting a wall on top? Does it bounce us back down? I'm not sure...
									downMoreThanGravityAllows = qtrue;
								}
							}

							qboolean playerIsDFAing = (qboolean)((thisFrameInfo.legsAnimGeneral[i] != lastFrameInfo.legsAnimGeneral[i] && isDFA<qtrue>(thisFrameInfo.legsAnimGeneral[i], demoType))
								|| (thisFrameInfo.torsoAnimGeneral[i] != lastFrameInfo.torsoAnimGeneral[i] && isDFA<qtrue>(thisFrameInfo.torsoAnimGeneral[i], demoType)));

							// Vertical boost detected
							// TODO: Deal with jumpbug somehow? Not necessary for most maps tho, it will be lower than ordinary jump speed I think.
							// TODO: Don't check against jump velocity if we are not in a jump?
							// TODO: Detect horizontal boosts too
							if (verticalDeltaNormalized > forcePowersInfo.maxForceJumpVelocityDelta) {

								// If we are in a dfa, our vertical velocity delta could exceed a usual jump velocity so make sure our speed upwards is higher than the speed of a yellow dfa
								// if (playerLastSaberMove[i].lastSaberMove == LS_A_JUMP_T__B_){
								if (playerIsDFAing) {
									//if (thisFrameInfo.playerVelocities[i][2] > DFA_MAX_VELOCITY_DELTA_VERT) {
									if (verticalDeltaNormalized > DFA_MAX_VELOCITY_DELTA_VERT) {
										isBoost = qtrue;
									}
								}
								// If we are in a backflip, our vertical velocity delta could exceed a usual jump velocity so make sure our speed upwards is higher than the speed of a backflip
								//if (isBackflip(thisFrameInfo.legsAnim[i],thisFrameInfo.torsoAnim[i], demoType)){
								else if ((thisFrameInfo.legsAnimGeneral[i] != lastFrameInfo.legsAnimGeneral[i] && isBackflip<qtrue>(thisFrameInfo.legsAnimGeneral[i],demoType))
									|| (thisFrameInfo.torsoAnimGeneral[i] != lastFrameInfo.torsoAnimGeneral[i] && isBackflip<qtrue>(thisFrameInfo.torsoAnimGeneral[i], demoType))
									){
									//if (thisFrameInfo.playerVelocities[i][2] > BACKFLIP_MAX_VELOCITY_DELTA_VERT) {
									if (verticalDeltaNormalized > BACKFLIP_MAX_VELOCITY_DELTA_VERT) {
										isBoost = qtrue;
									}
								} 
								// If we are in a yellow dfa, our vertical velocity delta could exceed a usual jump velocity so make sure our speed upwards is higher than the speed of a yellow dfa
								//else if (playerLastSaberMove[i].lastSaberMove == LS_A_FLIP_STAB || playerLastSaberMove[i].lastSaberMove == LS_A_FLIP_SLASH){
								else if ((thisFrameInfo.legsAnimGeneral[i] != lastFrameInfo.legsAnimGeneral[i] && isYellowDFA<qtrue>(thisFrameInfo.legsAnimGeneral[i], demoType))
									|| (thisFrameInfo.torsoAnimGeneral[i] != lastFrameInfo.torsoAnimGeneral[i] && isYellowDFA<qtrue>(thisFrameInfo.torsoAnimGeneral[i], demoType))
									){
									//if (thisFrameInfo.playerVelocities[i][2] > YELLOWDFA_MAX_VELOCITY_DELTA_VERT) {
									if (verticalDeltaNormalized > YELLOWDFA_MAX_VELOCITY_DELTA_VERT) {
										isBoost = qtrue;
									}
								}
								else {
									// No special cases detected, consider this a boost.
									isBoost = qtrue;
								}
									
								newBoost.detectType = BOOST_ENTITYSTATE_GUESS_VERTICAL;
							}
							if (downMoreThanGravityAllows) {

								// TODO: Ok but what if we hit a wall above us?
								isBoost = qtrue;
								newBoost.detectType = BOOST_ENTITYSTATE_GUESS_VERTICAL;
							}

							float horizontalSpeedToExceed = maximumWalkSpeed;
							if (playerIsDFAing) {
								horizontalSpeedToExceed = std::max(maximumWalkSpeed,DFA_MAX_VELOCITY_DELTA_HORZ+5.0f); // Adding 5 here because apparently in some rare situations we can get a horizontal speed of like 300.5 and such for a DFA. Not sure why. Just adding a little safety buffer there.
							}
							if (horizontalDelta > BOOST_ENTITY_HORIZONTAL_DELTA_DETECT_THRESHOLD
								&& horizontalSpeed > horizontalSpeedToExceed
								) { // Make sure we both got a boost and our resulting speed is more than walking speed. Otherwise a bit lame and uninteresting?
								
								isBoost = qtrue;
								newBoost.detectType = newBoost.detectType == BOOST_ENTITYSTATE_GUESS_VERTICAL ? BOOST_ENTITYSTATE_GUESS_HORIZONTAL_AND_VERTICAL : BOOST_ENTITYSTATE_GUESS_HORIZONTAL;
							}
							
							if (isBoost) {

								newBoost.boosterClientNum = -1; // Kinda impossible to know
								newBoost.boostedClientNum = i;
								newBoost.demoTime = demoCurrentTime;
								newBoost.isEnemyBoost = -1;
								//newBoost.confirmed = (qboolean)(VectorLength(thisFrameInfo.playerVelocities[i]) > BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY); 
								newBoost.autoSetMinimumTravelDistance(VectorLength(thisFrameInfo.playerVelocities[i]));

								// See which players we are facing.
								// Since we don't have info about who boosted us for entities, we have to do a more dirty approach and only accept boosts that go in the direction of the killer/victim, 
								// but we can only determine this at a later stage when the kill actually happens (since we don't yet know who will do a kill), so we predetermine who the boosted person
								// is facing right now.
								static vec3_t velocityDelta3d;
								VectorSubtract(thisFrameInfo.playerVelocities[i], lastFrameInfo.playerVelocities[i],velocityDelta3d);
								newBoost.setFacingTowards<max_clients>(&thisFrameInfo, velocityDelta3d);

								vec3_t lastVelocityTmp;
								vec3_t velocityChange;
								VectorCopy(lastFrameInfo.playerVelocities[i],lastVelocityTmp);
								lastVelocityTmp[2] = std::max(lastVelocityTmp[2], 0.0f); // Again, avoid too big reads resulting from a big delta when falling and immediately jumping up again
								VectorSubtract(thisFrameInfo.playerVelocities[i], lastVelocityTmp, velocityChange);
								newBoost.estimatedStrength = VectorLength(velocityChange);

								boosts.push_back(newBoost);
							}
						}
					}


					// Remove detected boosts that are too old to matter
					int lastIndexToRemove = -1;
					for (int i = 0; i < boosts.size(); i++) {
						if ((demoCurrentTime-boosts[i].demoTime)> BOOST_DETECT_MAX_AGE) {
							lastIndexToRemove = i;
						}
						else {
							break;
						}
					}
					if (lastIndexToRemove != -1 && lastIndexToRemove>= boosts.size()/2) { // too much left in: longer loops. but if we delete less than half,
						boosts.erase(boosts.begin(), boosts.begin()+lastIndexToRemove+1);
					}
					
					// Remove boosts of players who are currently only walking if the age of the boost is higher than BOOST_DETECT_MAX_AGE_WALKING
					// Example: A 5 second old boost that was interrupted by walking isn't really that interesting.
					// NO: This logic is actually flawed. This would somehow have to be evaluated at the time of processing the kill itself. What matters is the time of the walking before the kill, not the time of the walking after the boost.
					/*for (int i = boosts.size() - 1; i >= 0; i--) {
						if (
							thisFrameInfo.playerHadVelocity[boosts[i].boostedClientNum]
							// TODO: Figure this out better. Currently I kinda naively assume that maximum theoretically possible walking speed equals sqrt(g_speed*g_speed+g_speed*g_speed), but that's probably not entirely correct.
							&& VectorLength(thisFrameInfo.playerVelocities[boosts[i].boostedClientNum]) < thisFrameInfo.playerMaxWalkSpeed[boosts[i].boostedClientNum]
							&& (demoCurrentTime - boosts[i].demoTime) > BOOST_DETECT_MAX_AGE_WALKING
							) {

							boosts.erase(boosts.begin()+i);
						}
					}*/

					// Do some preparation for removing boosts that happened before some walks that happened a while ago. If somebody was walking over a second ago then any boosts before that are irrelevant tbh.
					// But if someone was walking half a second ago, then maybe he only landed, prepared an attack and executed, in which case the boost is still relevant.
					// TLDR: If we can find a detected walk that is older than BOOST_DETECT_MAX_AGE_WALKING but newer than a boost, we remove that boost.
					
					// Some preparation. Find the relevant walk times that are older than BOOST_DETECT_MAX_AGE_WALKING, but out of those, find the newest one so we can use that info to remove any boosts that happened before that.
					static int64_t newestRemoveWorthyWalks[max_clients];
					Com_Memset(newestRemoveWorthyWalks, 0, sizeof(newestRemoveWorthyWalks));
					int64_t oldestInterestingWalkingTime = demoCurrentTime - BOOST_DETECT_MAX_AGE_WALKING;
					for (int i = 0; i < max_clients; i++) {
						size_t maxIndexToRemove = -1;
						//for (int w = 0; w < walkDetectedTimes[i].size(); w++) {
						int w = walkDetectedTimesObsoleteIndex[i]; // kinda cringe performance micro-optimization, so we don't have to erase stuff from the vector on every frame (slow) but also don't have to iterate over it all.
						for (int64_t* it = walkDetectedTimes[i].data() + walkDetectedTimesObsoleteIndex[i], *endIt = walkDetectedTimes[i].data() + walkDetectedTimes[i].size(); it!=endIt; it++,w++) {
							bool isRemoveWorthy = (demoCurrentTime - *it) > BOOST_DETECT_MAX_AGE_WALKING;
							if (isRemoveWorthy) {
								walkDetectedTimesObsoleteIndex[i] = maxIndexToRemove = w;
								newestRemoveWorthyWalks[i] = *it;
							}
							else {
								break;
							}
						}
						/*auto firstToKeepIt = std::lower_bound(walkDetectedTimes[i].begin(), walkDetectedTimes[i].end(), oldestInterestingWalkingTime);
						if (firstToKeepIt != walkDetectedTimes[i].end()) {
							maxIndexToRemove = (firstToKeepIt - walkDetectedTimes[i].begin())-1;
							newestRemoveWorthyWalks[i] = *firstToKeepIt-1;
						}
						else {
							maxIndexToRemove = walkDetectedTimes[i].size() - 1;
							if (maxIndexToRemove != -1) {
								newestRemoveWorthyWalks[i] = walkDetectedTimes[i].back();
							}
						}*/

						if (maxIndexToRemove != -1 && maxIndexToRemove > walkDetectedTimes[i].size()/2) { 
							// don't do erase operations on every frame.
							// let's say the vec has 30 items and on each frame we erase a single one. 
							// that means on each frame we reduce the loopsize by 1 item, basically keeping it high anyway
							// but in return, we have to shift the entire array of 30 items by 1 on each frame. bad tradeoff.
							walkDetectedTimes[i].erase(walkDetectedTimes[i].begin(), walkDetectedTimes[i].begin() + maxIndexToRemove + 1);
							walkDetectedTimesObsoleteIndex[i] = 0;
						}
						else {
							//newestRemoveWorthyWalks[i] = 0;
						}
					}
					// Do the removal.
					for (int i = boosts.size() - 1; i >= 0; i--) {
						if (newestRemoveWorthyWalks[boosts[i].boostedClientNum] >= boosts[i].demoTime
							) {

							boosts.erase(boosts.begin()+i);
						}
					}

					// Remove unconfirmed boosts that didn't result in meaningful movement
					for (int i = boosts.size() - 1; i >= 0; i--) {
						if (!boosts[i].confirmed) {
							int boostedClient = boosts[i].boostedClientNum;

							// If the boost isn't already confirmed, we check whether we traveled a meaningful distance. If not, we ditch the boost. Just to get rid of tiny irrelevant crap.
							// A boost can be confirmed in 2 ways:
							// 1. By having traveled enough distance from boost time to hitting ground again
							// 2. Initial speed after boost was fast enough (above BOOST_DETECT_MIN_TRAVEL_DISTANCE_OVERRIDE_VELOCITY atm)

							// Entity exists in this and in the last frame
							// In the last frame we were airborne (or at least not touching the level ground) and in this frame we are touching the level ground.
							if (thisFrameInfo.entityExists[boostedClient] && lastFrameInfo.entityExists[boostedClient]
								&& thisFrameInfo.groundEntityNum[boostedClient] == ENTITYNUM_WORLD && lastFrameInfo.groundEntityNum[boostedClient] != ENTITYNUM_WORLD
								) {
								
								if(!boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boostedClient])){
								/*float distanceTraveled = VectorDistance(thisFrameInfo.playerPositions[boostedClient], boosts[i].startPosition);

								if (distanceTraveled < boosts[i].minimumTravelDistance) {
									*/
									boosts.erase(boosts.begin() + i);
								}
								else {
									boosts[i].confirmed = qtrue;
								}
							}
						}
					}
				}


#ifdef DEBUGSTATSDB
				if(!isMOHAADemo && generalizeGameValue<GMAP_WEAPONS, UNSAFE>( demo.cut.Cl.snap.ps.weapon,demoType)==WP_SABER_GENERAL){ // TODO Maybe add saber on/off here too? Because saber off might have same anim always?
					
					animStanceKey keyHere = { demoType,demo.cut.Cl.snap.ps.saberHolstered,demo.cut.Cl.snap.ps.torsoAnim & ~getANIM_TOGGLEBIT(demoType),demo.cut.Cl.snap.ps.legsAnim & ~getANIM_TOGGLEBIT(demoType),demo.cut.Cl.snap.ps.saberMove,demo.cut.Cl.snap.ps.fd.saberAnimLevel };  // torsoAnim,legsAnim,saberMove,stance
					animStanceCounts[keyHere]++;
					
				}
#endif


				// Update player visible frames
				// For some info like angular velocity, acceleration and jerk we need to know that the past X frames had info about the player, else the value becomes invalid.
				// 
				// While we're at it, register z coordinates of players touching the ground.
				//
				// Also while we're at it, manage special jumps (over player heads for now)
				// 
				// Also while at it, do groundframe tracking
				// 
				// While at it, if requested, log stuff for strafe CSV output.
				for (int i = 0; i < max_clients;i++) {
					if (thisFrameInfo.entityExists[i]) {
						playerVisibleFrames[i]++;
						if (thisFrameInfo.commandTime[i] != lastFrameInfo.commandTime[i]) {
							playerVisibleClientFrames[i]++;
						}

						// rough location tracking (restrict to cappers?)
						if (encodeNewLocation(thisFrameInfo.playerPositions[i],playerSpiralLocations[i].nextLoc ? &playerSpiralLocations[i].locs[(playerSpiralLocations[i].nextLoc-1) % MAX_PAST_SPIRAL_LOCATIONS] : NULL, &playerSpiralLocations[i].locs[playerSpiralLocations[i].nextLoc % MAX_PAST_SPIRAL_LOCATIONS])) {
							playerSpiralLocations[i].locs[playerSpiralLocations[i].nextLoc % MAX_PAST_SPIRAL_LOCATIONS].demoTime = demoCurrentTime;
							playerSpiralLocations[i].nextLoc++;
						}

						// special jumps/headjumps tracking
						if (thisFrameInfo.groundEntityNum[i] == ENTITYNUM_WORLD) {
							lastGroundHeight[i] = thisFrameInfo.playerPositions[i][2];
							specialJumpCount[i] = 0; 
							headJumpCount[i] = 0;
						}
						else if (lastFrameInfo.entityExists[i] && lastFrameInfo.groundEntityNum[i] == ENTITYNUM_NONE) {
							if (thisFrameInfo.groundEntityNum[i] >= 0 && thisFrameInfo.groundEntityNum[i] < max_clients) {
								headJumpCount[i]++;
							}
							// TODO:
							else if (thisFrameInfo.entityOwnerInfo[thisFrameInfo.groundEntityNum[i]].type == TET_FORCEFIELD || 
								(thisFrameInfo.entityOwnerInfo[thisFrameInfo.groundEntityNum[i]].type == TET_SENTRY && thisFrameInfo.entityOwnerInfo[thisFrameInfo.groundEntityNum[i]].flags & TETFLAG_AIRBORNE)) {
								specialJumpCount[i]++;
								if (thisFrameInfo.entityOwnerInfo[thisFrameInfo.groundEntityNum[i]].type == TET_SENTRY && thisFrameInfo.entityOwnerInfo[thisFrameInfo.groundEntityNum[i]].owner == i) {
									// Self-sentry jump. Giga rare. Could be accident or giga thing
									lastSelfSentryJump[i] = demoCurrentTime;
								}
							}
						}

						if (lastFrameInfo.entityExists[i]) {

							// script check part #2
							if ((lastFrameInfo.frameInfoFlags[i] & FIF_SCRIPTCHECK) && thisFrameInfo.movementDir[i] != 4 && (thisFrameInfo.commandTime[i] - lastFrameInfo.commandTime[i]) < 10) {
								logSpecialThing("SCRIPTDETECT","OneFrameBackKeyBSTrigger","3 consecutive client frames no more than 10ms apart; middle frame has dbs/bs/blubs trigger and only frame with back key.",getPlayerName(i,CS_PLAYERS_here,isMOHAADemo),i,-1,demoCurrentTime,0,bufferTime,lastGameStateChangeInDemoTime,io,&sharedVars.oldBasename,&sharedVars.oldPath,sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
							}

							if (lastFrameInfo.saberMoveGeneral[i] != thisFrameInfo.saberMoveGeneral[i]) {

								// script check part #1
								if (thisFrameInfo.movementDir[i] == 4 && lastFrameInfo.movementDir[i] != 4 && (thisFrameInfo.commandTime[i] - lastFrameInfo.commandTime[i]) < 10 && (thisFrameInfo.saberMoveGeneral[i] == LS_A_BACK_CR_GENERAL || thisFrameInfo.saberMoveGeneral[i] == LS_A_BACK_GENERAL || thisFrameInfo.saberMoveGeneral[i] == LS_A_BACKSTAB_GENERAL)) {
									thisFrameInfo.frameInfoFlags[i] |= FIF_SCRIPTCHECK;
								}


								// saber combo tracking
								//saberMoveType_t oldType = classifySaberMove(lastFrameInfo.saberMoveGeneral[i]);
								saberMoveType_t newType = classifySaberMove((saberMoveName_t)thisFrameInfo.saberMoveGeneral[i]);
								if (newType == LST_ATTACK) {
									saberComboCounter[i]++;
								}
								else if (newType == LST_IDLE) {
									saberComboCounter[i] = 0;
								} // else: some kind of transition. dont care, doesnt affect us.
							}

#if TRACK_GROUNDFRAME
							// groundframe tracking
							if (lastFrameInfo.groundEntityNum[i] == ENTITYNUM_NONE && thisFrameInfo.groundEntityNum[i] != ENTITYNUM_NONE) {
								lastPreGroundFrameAir[i] = lastFrameInfo.commandTime[i] == -1 ? lastFrameInfo.serverTime : lastFrameInfo.commandTime[i];//lastFrameInfo.demoTime;
								VectorCopy(lastFrameInfo.playerVelocities[i], lastPreGroundFrameVelocity[i]);
								VectorCopy(lastFrameInfo.playerPositions[i], lastPreGroundFramePosition[i]);
							}
							else if (lastFrameInfo.groundEntityNum[i] != ENTITYNUM_NONE && thisFrameInfo.groundEntityNum[i] == ENTITYNUM_NONE && lastPreGroundFrameAir[i] != -1) {
								//JUMP_VELOCITY
								// do a naive calc to determine the exaact length of the groundframe based on time it would take to land, and time it would take to reach current height.
								int64_t curTime = thisFrameInfo.commandTime[i] == -1 ? thisFrameInfo.serverTime : thisFrameInfo.commandTime[i];
								float baseDiff = (curTime - lastPreGroundFrameAir[i]);
								float groundHeight = lastFrameInfo.playerPositions[i][2];
								//float preDelta = std::max(0,lastPreGroundFramePosition[i][2] - groundHeight);
								//float postDelta = std::max(0, thisFrameInfo.playerPositions[i][2] - groundHeight);
								float preHeight = std::max(groundHeight,lastPreGroundFramePosition[i][2]);
								float postHeight = std::max(groundHeight,thisFrameInfo.playerPositions[i][2]);
								// startpos - 0.5*time*time*800 = endpos
								// startpos-endpos = 0.5*time*time*800
								// time = sqrt(2*(startpos-endpos)/800);
								// moar
								// startpos - vel*time - 0.5*time*time*800 = endpos
								// startpos - endps = 0.5*time*time*800 + vel*time 
								// s - f = 0.5*t*t*g + v*t
								float fallspeed = -lastPreGroundFrameVelocity[i][2];
								float fallspeed2 = -thisFrameInfo.playerVelocities[i][2];
								float gravity = DEFAULT_GRAVITY;
								//float preTime = -(sqrtf(-2.0f* groundHeight* gravity + 2.0f* gravity*preHeight+fallspeed*fallspeed) + fallspeed) / gravity; (this is the other result of the curve in the past)
								float preTime = (sqrtf(-2.0f* groundHeight* gravity + 2.0f* gravity*preHeight+fallspeed*fallspeed) - fallspeed) / gravity;
								//float postTime = (sqrtf(-2.0f* groundHeight* gravity + 2.0f* gravity* postHeight + fallspeed2*fallspeed2) + fallspeed2) / gravity;
								// force jump maintains at least JUMP_VELOCITY while jump is held so this all kinda falls apart a bit.
								// lets do a naive alternate calculation where we assume just constant JUMP_VELOCITY
								// startpos + jumpvel*time = endpos
								// (startpos-endpos)/jumpvel = time
								float postTimeForceJump = (postHeight-groundHeight)/(float)JUMP_VELOCITY;
								

								baseDiff -= preTime * 1000.0f + postTimeForceJump * 1000.0f;


								if(baseDiff < GROUNDFRAME_MAXDURATION && baseDiff > GROUNDFRAME_MINDURATION){
									float oldspeed = VectorLength2(lastPreGroundFrameVelocity[i]);
									float newspeed = VectorLength2(thisFrameInfo.playerVelocities[i]);
									if (oldspeed < 325.0f || newspeed < 325.0f) {
										lastPreGroundFrameAir[i] = -1; // This is about walking speed before or after. or we somehow gained speed. ignore.
									}
									else {
										vec3_t olddir,newdir;
										// TODO how to prevent detecting groundframes on slopes that boost?
										VectorCopy(thisFrameInfo.playerVelocities[i], newdir);
										VectorCopy(lastPreGroundFrameVelocity[i], olddir);
										newdir[2] = 0;
										Vector2Normalize(newdir);
										olddir[2] = 0;
										Vector2Normalize(olddir);
										vec3_t oldVel;
										VectorCopy(lastPreGroundFrameVelocity[i], oldVel);
										oldVel[2] = 0;
										float oldSpeedInNewDir = DotProduct(oldVel,newdir);
										lastGroundFrame[i] = demoCurrentTime;
										lastGroundFrameQuality[i] = newspeed / oldSpeedInNewDir;
										lastGroundFrameAngleChange[i] = acosf(DotProduct(olddir,newdir))*180.0f/M_PI;
										if (playerDemoStatsPointers[i] && newspeed > 430.0f && oldspeed > 430.0f && lastGroundFrameQuality[i] <= 1.0f && lastGroundFrameQuality[i] >= 0.0 && fpclassify(lastGroundFrameQuality[i]) != FP_NAN) { // just to keep this relevant for stats
											playerDemoStatsPointers[i]->everUsed = qtrue;
											playerDemoStatsPointers[i]->groundFrameQuality.sum += lastGroundFrameQuality[i];
											playerDemoStatsPointers[i]->groundFrameQuality.divisor += 1.0;
										}
#if TRACK_GROUNDFRAME_DEBUG
										lastGroundFrameDebug[i].newSpeed = newspeed;
										lastGroundFrameDebug[i].oldSpeed = oldspeed;
										lastGroundFrameDebug[i].oldSpeedInNewDir = oldSpeedInNewDir;
										lastGroundFrameDebug[i].groundFrameDurationEstimate = baseDiff;
										VectorCopy(lastPreGroundFrameVelocity[i], lastGroundFrameDebug[i].oldVel);
										VectorCopy(thisFrameInfo.playerVelocities[i], lastGroundFrameDebug[i].newVel);
										VectorCopy(lastFrameInfo.playerVelocities[i], lastGroundFrameDebug[i].lastGroundVel);
#endif
									}

								}
							}
#endif
						}

						if (opts.playerCSVDump) {
							playerDumpCSVPoint_t newPoint;
							newPoint.demoTime = demoCurrentTime;
							newPoint.commandTime = thisFrameInfo.commandTime[i];
							VectorCopy(thisFrameInfo.playerPositions[i], newPoint.position);
							VectorCopy(thisFrameInfo.playerVelocities[i], newPoint.velocity);
							VectorCopy(thisFrameInfo.playerAngles[i], newPoint.viewangles);
							playerDumpCSVDataPoints[i].push_back(newPoint);
						}

						if (lastFrameInfo.entityExists[i]) {

							if (opts.strafeCSVSyncPoint) { // Strafe CSV logging is enabled. Player visible this and last frame. Let's do some magic. Why did I write that. What's magiclal about it?
								float currentSpeed = VectorLength2(thisFrameInfo.playerVelocities[i]);
								float lastSpeed = VectorLength2(lastFrameInfo.playerVelocities[i]);
								if ((lastSpeed >= opts.strafeCSVResetPoint && currentSpeed < opts.strafeCSVResetPoint) || fabsf(currentSpeed) < 1.0f) {
									strafeCSVResetPlayer(i, opts);
								}
								int64_t timeOffset = strafeCSVCurrentRun[i].size() > 0 ? strafeCSVCurrentRun[i].back().timeOffset + deltaTimeFromLastSnapshot: 0;
								float distanceTraveled = Vector2Distance(thisFrameInfo.playerPositions[i], lastFrameInfo.playerPositions[i]);
								strafeCSVPoint_t newPoint{};
								newPoint.distanceTraveledFromLast = distanceTraveled;
								newPoint.timeOffset = timeOffset;
								newPoint.velocityXY = currentSpeed;
								strafeCSVCurrentRun[i].push_back(newPoint);
							}

							if (thisFrameInfo.groundEntityNum[i] == ENTITYNUM_NONE && lastFrameInfo.groundEntityNum[i] != ENTITYNUM_NONE) {
								// Sadly this doesn't always work. It often does, but also often doesn't.
								// My guess: It's only 1 frame with quick jumps and the server didn't send that particular frame due to snap limit.
								// Or: It's not even a single frame? idk.
								jumpDetected[i] = qtrue;
							}

							// Q3 defrag run tracking
							if (gameIsQ3Defrag && i == demo.cut.Cl.snap.ps.clientNum && thisFrameInfo.psStats12[i] && lastFrameInfo.psStats12[i]) {
								int thisFrameDefragInfo = thisFrameInfo.psStats12[i];
								int lastFrameDefragInfo = lastFrameInfo.psStats12[i];
								if ((thisFrameDefragInfo & 8) && !(thisFrameDefragInfo & 2) && (lastFrameDefragInfo & 2)) {
									// IDK dont ask me rofl
									defragRunInfoFinal_t runInfo{};
									runInfo.info.milliseconds = q3DefragInfo.calculateCorrectTime(&demo.cut.Cl.snap.ps, demo.cut.Cl.snap.serverTime,&runInfo.info.checksumError);
									//runInfo.milliseconds = demo.cut.Cl.snap.ps.stats[7] * 65536 + (uint16_t)(int16_t)demo.cut.Cl.snap.ps.stats[8];
									runInfo.info.knownClientNum = i;
									runInfo.info.style = q3DefragInfo.promode ? "cpm" : "vq3";

									int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + i];
									const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
									runInfo.info.playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "dfn");
									if (!runInfo.info.playerName.size()) {
										runInfo.info.playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "n");
									}
									GetRunMeta<max_clients>(runInfo.info, runInfo.meta, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here);
									qboolean result = SaveDefragRun<max_clients>(runInfo, sharedVars,sourceDemoFile, io, bufferTime, opts, searchMode, wasDoingSQLiteExecution);
								}
							}
						}

					}
					else {
						saberComboCounter[i] = 0; // can't rly see whats happening
						playerVisibleFrames[i] = 0;
						playerVisibleClientFrames[i] = 0;
						lastGroundHeight[i] = 99999999; // What's the point of remembering the ground height if the player may suddenly appear already in air and we have an old value and wrongly detect Air 2 Air kills?
						specialJumpCount[i] = 0; // If we can't see the player we can't tell if he's jumping over other stuff so don't track
						headJumpCount[i] = 0; // If we can't see the player we can't tell if he's jumping over other stuff so don't track
#if TRACK_GROUNDFRAME
						lastPreGroundFrameAir[i] = -1; // If we can't see the player we can't tell if he's on the ground or not
#endif

						if (opts.strafeCSVSyncPoint) { // Strafe CSV logging is enabled. Player not visible. Reset this run.
							strafeCSVResetPlayer(i,opts);
						}
					}
				}



				if(!isMOHAADemo){ // Playerstate events... uhm experimental?
					psEventData_t psEventData = demoCutGetEvent(&demo.cut.Cl.snap.ps, &demo.cut.Cl.oldSnap.ps,demoCurrentTime);

					// Ok this confirms he took damage.
					int generalEvent = generalizeGameValue<GMAP_EVENTS, UNSAFE>(psEventData.externalEvent.event, demoType);
					int generalEventPred0 = generalizeGameValue<GMAP_EVENTS, UNSAFE>(psEventData.predictableEvents[0].event, demoType);
					int generalEventPred1 = generalizeGameValue<GMAP_EVENTS, UNSAFE>(psEventData.predictableEvents[1].event, demoType);
					if (generalEvent >= EV_PAIN_GENERAL && generalEvent <= EV_DEATH3_GENERAL
						|| generalEventPred0 >= EV_PAIN_GENERAL && generalEventPred0 <= EV_DEATH3_GENERAL
						|| generalEventPred1 >= EV_PAIN_GENERAL && generalEventPred1 <= EV_DEATH3_GENERAL
						) {
						hitDetectionData[demo.cut.Cl.snap.ps.clientNum].painDetected = qtrue;
					}
					if (generalEvent == EV_JUMP_GENERAL
						|| generalEventPred0 == EV_JUMP_GENERAL
						|| generalEventPred1 == EV_JUMP_GENERAL
						) {
						jumpDetected[demo.cut.Cl.snap.ps.clientNum] = qtrue;
					}
				}

				// Preparse flag pickup events (lame but it CAN happen that people pick up the flag and die on the same frame, and we won't be able to register it as a ret if the kill event comes before the flag pickup event otherwise)
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) { // +1 is so we can process the fake event entities created from parsing prints in MOHAA

					entityState_t* thisEs = NULL;
					int eventNumber = 0;
					thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					eventNumber = demoCutGetEvent(thisEs, demoCurrentTime, demoType,qtrue);
					eventNumber = generalizeGameValue<GMAP_EVENTS, UNSAFE>(eventNumber, demoType);

					if (eventNumber) {
						if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_PLAYER_GOT_FLAG) {
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;

							// So, since many things can in theory happen during a single server frame (like someone both getting the flag and dying)
							// we track all the players that got the flag on this frame.
							// And if they are killed, we clear this flag again. (Or not?)
							// This will be checked against in the EV_OBITUARY handling to detect returns that might otherwise have been lost.
							if (flagTeam == TEAM_RED) {
								redFlagNewCarrierByEventBitMask |= (1L << playerNum);
							}
							else if (flagTeam == TEAM_BLUE) {
								blueFlagNewCarrierByEventBitMask |= (1L << playerNum);
							}
						}
						else if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_PLAYER_CAPTURED_FLAG) {
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2; 

							// Similarly, here, we track whoever captured the flag on this frame.
							// Because let's say we remember the flag carrier from last frame and he actually successfully captured,
							// but he died on the same frame after capturing, we don't wanna count that as a return.
							// So this will be checked against in the EV_OBITUARY handling.
							if (flagTeam == TEAM_RED) {
								redFlagCapturedPlayerBitMask |= (1L << playerNum);
							}
							else if (flagTeam == TEAM_BLUE) {
								blueFlagCapturedPlayerBitMask |= (1L << playerNum);
							}
						}
					}
				}

				// Fire events
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities + 1; pe++) { // +1 is so we can process the fake event entities created from parsing prints in MOHAA

					bool thisIsFakeEventEntity = false;
					entityState_t* thisEs = NULL;
					int eventNumber = 0;
					if (pe == demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities) {
						if (!isMOHAADemo || parsedEventEntities.empty()) {
							break;
						}
						else {
							thisEs = parsedEventEntities.front();
							parsedEventEntities.pop();
							if (!parsedEventEntities.empty()) {
								pe--; // Keep running this number until none are left here.
							}
							eventNumber = thisEs->event;
							thisIsFakeEventEntity = true;
						}
					}
					else {
						thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						eventNumber = demoCutGetEvent(thisEs, demoCurrentTime, demoType,qfalse);
						eventNumber = generalizeGameValue<GMAP_EVENTS, UNSAFE>(eventNumber, demoType);
					}

					if (eventNumber) {
						

						// Handle kills
						if (eventNumber == EV_OBITUARY_GENERAL && !opts.skipKills) {
							int				target = thisEs->otherEntityNum;
							int				attacker = thisEs->otherEntityNum2;
							int				mod = thisEs->eventParm;
							int				originalMod = mod;
							bool			victimIsFlagCarrier = false;
							bool			attackerIsFlagCarrier = false;
							bool			isSuicide;
							bool			isDoomKill;
							bool			isWorldKill = false;
							bool			targetIsVisible = false;
							bool			attackerIsVisibleOrFollowed = false;
							bool			targetIsVisibleOrFollowed = false;
							bool			attackerIsVisible = false;
							bool			isSaberKill = false; // In case we want to avoid non-saber kills to save space with huge demo collections
#define KAEF_OCCLUSION_SOLID_AFTER_BOXTEST	(1<<0)
							int				errorFlags = 0;
							qboolean		sameFrameRet = qfalse;
							if (target < 0 || target >= max_clients) {
								std::cout << "CG_Obituary: target out of range. This should never happen really.";
							}

							playersKilledThisFrameBitMask |= (1L << target);

							if (attacker < 0 || attacker >= max_clients) {
								attacker = ENTITYNUM_WORLD;
								isWorldKill = true;
							}
							
							mod = generalizeGameValue<GMAP_MEANSOFDEATH,SAFE>(mod,demoType);

							entityState_t* targetEntity = findEntity(target);
							if (targetEntity) {
								targetIsVisible = true;
								/*if (targetEntity->powerups & (1 << PW_REDFLAG) || targetEntity->powerups & (1 << PW_BLUEFLAG)) {
									// If the victim isn't visible, his entity won't be available, thus this won't be set
									// But we're trying to find interesting moments, so stuff that's not even visible is not that interesting to us
									victimIsFlagCarrier = true;
								}*/
							}
							entityState_t* attackerEntity = findEntity(attacker);
							if (attackerEntity) {
								attackerIsVisible = true;
							}
							victimIsFlagCarrier = target == lastKnownFlagCarrier[TEAM_BLUE] || target == lastKnownFlagCarrier[TEAM_RED];
							attackerIsFlagCarrier = attacker == lastKnownFlagCarrier[TEAM_BLUE] || attacker == lastKnownFlagCarrier[TEAM_RED];

							int victimFlagTeam = victimIsFlagCarrier ? (target == lastKnownFlagCarrier[TEAM_BLUE] ? TEAM_BLUE : TEAM_RED) : -1;
							int attackerFlagTeam = attackerIsFlagCarrier ? (attacker == lastKnownFlagCarrier[TEAM_BLUE] ? TEAM_BLUE : TEAM_RED) : -1;

							int victimCarrierLastPickupOrigin = victimIsFlagCarrier ? (target == lastKnownFlagCarrier[TEAM_BLUE] ? cgs.flagLastPickupOrigin[TEAM_BLUE] : cgs.flagLastPickupOrigin[TEAM_RED]) : -1;

							if (!victimIsFlagCarrier) {
								// Victim acquired the flag on this frame/snap
								// We technically don't know the order - did he get the flag first? Or did he die first?
								// But logically it should be the former. How would you die and get the flag after you died?

								// TODO what about victimCarrierLastPickupOrigin?
								if (redFlagNewCarrierByEventBitMask & (1L << target)) {
									victimIsFlagCarrier = true;
									victimFlagTeam = TEAM_BLUE;
									victimCarrierLastPickupOrigin = -1;
									sameFrameRet = qtrue;
								}
								else if (blueFlagNewCarrierByEventBitMask & (1L << target)) {
									victimIsFlagCarrier = true;
									victimFlagTeam = TEAM_RED;
									victimCarrierLastPickupOrigin = -1;
									sameFrameRet = qtrue;
								}
							}
							if (!attackerIsFlagCarrier && attacker >= 0 && attacker < max_clients) {
								// Attacker acquired the flag on this frame/snap
								// Same as above for victim

								// TODO what about victimCarrierLastPickupOrigin?
								if (redFlagNewCarrierByEventBitMask & (1L << attacker)) {
									attackerIsFlagCarrier = true;
									attackerFlagTeam = TEAM_BLUE;
								}
								else if (blueFlagNewCarrierByEventBitMask & (1L << attacker)) {
									attackerIsFlagCarrier = true;
									attackerFlagTeam = TEAM_RED;
								}
							}
							if (victimIsFlagCarrier) {
								// Victim with flag captured the flag on the same frame.
								// So what this tells us is that we may be wrong about the victim being a flag carrier
								// We get both the EV_OBITUARY of the victim as well as the event telling us that the victim captured a flag.
								// We can't know the order for sure but logically it makes sense that someone captured the flag and died on the next frame
								// versus someone dying and THEN capturing the flag. lol.
								// TLDR: We assume that if that player captured a flag on this frame, this isn't a return.
								if (redFlagCapturedPlayerBitMask & (1L << target)) {
									victimIsFlagCarrier = false;
									victimFlagTeam = -1;
									victimCarrierLastPickupOrigin = -1;
								}
								else if (blueFlagCapturedPlayerBitMask & (1L << target)) {
									victimIsFlagCarrier = false;
									victimFlagTeam = -1;
									victimCarrierLastPickupOrigin = -1;
								}
							}

							// Attacker with flag captured the flag on the same frame.
							// There is really no way to tell whether he captured the flag first or whether
							// he killed the other person first. It's the same frame, either is possible.
							// Let's give benefit of doubt and assume he did the kill with the flag on his back.
							/*if (attackerIsFlagCarrier) {
								if (redFlagCapturedPlayerBitMask & (1L << attacker)) {
									attackerIsFlagCarrier = false;
									attackerFlagTeam = -1;
								}
								else if (blueFlagCapturedPlayerBitMask & (1L << attacker)) {
									attackerIsFlagCarrier = false;
									attackerFlagTeam = -1;
								}
							}*/



							// Track how many people a flag carrier kills during his hold.
							if (!isWorldKill && attackerIsFlagCarrier) {
								if (recentKillsDuringFlagHold[attacker].lastKillDemoTime < demoCurrentTime-recentFlagHoldTimes[attacker]) {
									// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
									Com_Memset(&recentKillsDuringFlagHold[attacker],0,sizeof(CapperKillsInfo));
								}
								if (victimIsFlagCarrier) {
									recentKillsDuringFlagHold[attacker].rets++;
								}
								recentKillsDuringFlagHold[attacker].kills++;
								recentKillsDuringFlagHold[attacker].lastKillDemoTime = demoCurrentTime;
							}
							int victimFlagCarrierKillCount = -1;
							int victimFlagCarrierRetCount = -1;
							if (victimIsFlagCarrier) {
								// See if he had any kills/rets as carrier. (did he fight valiantly before he died?)
								if (recentKillsDuringFlagHold[target].lastKillDemoTime < demoCurrentTime - recentFlagHoldTimes[target]) {
									// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
									Com_Memset(&recentKillsDuringFlagHold[target], 0, sizeof(CapperKillsInfo));
								}
								victimFlagCarrierKillCount = recentKillsDuringFlagHold[target].kills;
								victimFlagCarrierRetCount = recentKillsDuringFlagHold[target].rets;
							}
							// TODO What if lastKnownFlagCarrier[TEAM_BLUE] or lastKnownFlagCarrier[TEAM_RED] is remembered and another kill is counted wrongly as ret even though he is no longer holding a flag?


							// If victim was capper, see if his entire run from start to unfortunate end was captured.
							bool capperWasFollowed = false;
							bool capperWasVisible = false;
							bool capperWasVisibleOrFollowed = false;
							if (victimIsFlagCarrier) {

								int victimFlagHoldTime = recentFlagHoldTimes[target];
								if (playerFirstFollowed[target] != -1 && playerFirstFollowed[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasFollowed = true;
								}
								if (playerFirstVisible[target] != -1 && playerFirstVisible[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasVisible = true;
								}
								if (playerFirstFollowedOrVisible[target] != -1 && playerFirstFollowedOrVisible[target] < (demo.cut.Cl.snap.serverTime - victimFlagHoldTime)) {
									capperWasVisibleOrFollowed = true;
								}
							}


							isSuicide = target == attacker;
							isDoomKill = mod == MOD_FALLING_GENERAL;
							bool targetIsFollowed = demo.cut.Cl.snap.ps.clientNum == target;
							bool attackerIsFollowed = demo.cut.Cl.snap.ps.clientNum == attacker;
							attackerIsVisibleOrFollowed = attackerIsFollowed || attackerIsVisible;
							targetIsVisibleOrFollowed = targetIsFollowed || targetIsVisible;

							float attackerJumpHeight = 0;
							float victimJumpHeight = 0;

							if (attackerIsVisibleOrFollowed) {
								attackerJumpHeight = attackerIsFollowed ? demo.cut.Cl.snap.ps.origin[2] - lastGroundHeight[demo.cut.Cl.snap.ps.clientNum] : attackerEntity->pos.trBase[2] - lastGroundHeight[attackerEntity->number];
							} 
							if (targetIsVisibleOrFollowed) {
								victimJumpHeight = targetIsFollowed ? demo.cut.Cl.snap.ps.origin[2] - lastGroundHeight[demo.cut.Cl.snap.ps.clientNum] : targetEntity->pos.trBase[2] - lastGroundHeight[targetEntity->number];
							} 
							if(attackerJumpHeight < 0.0f) attackerJumpHeight = 0.0f;
							if(victimJumpHeight < 0.0f) victimJumpHeight = 0.0f;


							// See if flag carrier's flag was at base and we know its position & distance
							bool baseFlagDistanceKnownAndApplicable = false;
							float baseFlagDistance = 0.0f;
							entityState_t* baseFlagEntity = baseFlagEntities[playerTeams[target]];
							if (victimIsFlagCarrier && targetIsVisibleOrFollowed && baseFlagEntity) {
								baseFlagDistance = VectorDistance(baseFlagEntity->pos.trBase, targetIsFollowed ? demo.cut.Cl.snap.ps.origin : targetEntity->pos.trBase);
								baseFlagDistanceKnownAndApplicable = true;
							}


							bool isTeamKill = false;
							if (isMOHAADemo) {
								isTeamKill = g_gametype > GT_FFA_MOH && attacker != target && attacker >= 0 && attacker < max_clients&& playerTeams[attacker] == playerTeams[target] && playerTeams[target] != TEAM_FREEFORALL_MOH && playerTeams[target] != TEAM_NONE_MOH;
							}
							else {
								isTeamKill = attacker != target && attacker >= 0 && attacker < max_clients&& playerTeams[attacker] == playerTeams[target] && playerTeams[target] != TEAM_FREE;
							}

							//targetIsVisible = targetIsVisible && attackerIsVisibleOrFollowed; // Make sure both attacker and victim are visible. Some servers send info

							float maxSpeedTargetFloat = getMaxSpeedForClientinTimeFrame(target, demoCurrentTime - 1000, demoCurrentTime);
							int maxSpeedTarget = maxSpeedTargetFloat;
							Kill thisKill;
							thisKill.isRet = victimIsFlagCarrier;
							thisKill.isTeamKill = isTeamKill;
							thisKill.isSuicide = isSuicide;
							thisKill.isDoom = isDoomKill;
							thisKill.isExplosion = (mod == MOD_FLECHETTE_GENERAL ||
								mod == MOD_FLECHETTE_ALT_SPLASH_GENERAL ||
								mod == MOD_ROCKET_GENERAL ||
								mod == MOD_ROCKET_SPLASH_GENERAL ||
								mod == MOD_ROCKET_HOMING_GENERAL ||
								mod == MOD_ROCKET_HOMING_SPLASH_GENERAL ||
								mod == MOD_THERMAL_GENERAL ||
								mod == MOD_THERMAL_SPLASH_GENERAL ||
								mod == MOD_TRIP_MINE_SPLASH_GENERAL ||
								mod == MOD_TIMED_MINE_SPLASH_GENERAL ||
								mod == MOD_DET_PACK_SPLASH_GENERAL

								// JK 3
								|| mod == MOD_COLLISION_GENERAL // Not sure about this one... in one demo i have it was just 2 ppl running into each other and falling to their deaths. but that demo was weird in general (flechette detected as mine etc)
								|| mod == MOD_VEH_EXPLOSION_GENERAL
								//|| mod == MOD_CONC_ALT_GENERAL
								// 
								// Q3
								|| mod == MOD_GRENADELAUNCHER_GENERAL
								|| mod == MOD_GRENADE_SPLASH_GENERAL
								|| mod == MOD_KAMIKAZE_GENERAL
								//|| mod == MOD_PLASMA_GENERAL, // Idk if i should count plasma as explosive... not really?
								//|| mod == MOD_PLASMA_SPLASH_GENERAL,
								//|| mod == MOD_BFG_GENERAL
								//|| mod == MOD_BFG_SPLASH_GENERAL // Bfg is kinda explosive for gibbing purposes, but not really in terms im interested in here?
								|| mod == MOD_PROXIMITY_MINE_GENERAL

								// MOHAA
								|| mod == MOD_GRENADE_GENERAL
								|| mod == MOD_EXPLOSION_GENERAL
								|| mod == MOD_EXPLODEWALL_GENERAL
								|| mod == MOD_GRENADE_GENERAL
								|| mod == MOD_GRENADE_GENERAL
								);
							thisKill.mod = (meansOfDeath_t)mod; // Hmm. Use generalized? Or normal. Tough question. investigate

							thisKill.targetClientNum = target;
							thisKill.time = demoCurrentTime;
							//thisKill.isVisible = targetIsVisible;
							thisKill.isVisible = targetIsVisibleOrFollowed;
							thisKill.isFollowed = attackerIsFollowed;
							thisKill.victimMaxSpeedPastSecond = maxSpeedTargetFloat;
							thisKill.timeSinceSaberMoveChange = isWorldKill ? -1 : (demoCurrentTime-playerLastSaberMove[attacker].lastSaberMove[0].saberMoveChange);
							thisKill.timeSinceBackflip = isWorldKill ? -1 : (lastBackflip[attacker] >= 0?(demoCurrentTime-lastBackflip[attacker]):-1);
							thisKill.speedatSaberMoveChange = isWorldKill ? -1 : (playerLastSaberMove[attacker].lastSaberMove[0].speed);

							int64_t timeSinceLastSelfSentryJump = isWorldKill ? -1 : (lastSelfSentryJump[attacker] >= 0 ? (demoCurrentTime - lastSelfSentryJump[attacker]) : -1);
							
							int64_t timeSinceLastSneak = isWorldKill ? -1 : (demoCurrentTime - lastSneak[attacker]);

							float groundFrameQuality = (isWorldKill || (demoCurrentTime- lastGroundFrame[attacker]) > GROUNDFRAME_MAXPASTCOUNT) ? -1 : (lastGroundFrameQuality[attacker]);
							float groundFrameAnglechange = groundFrameQuality != -1 ? lastGroundFrameAngleChange[attacker] : -1;

							// This is the place that had all the continues originally.
							
							//entityState_t* attackerEntity = findEntity(attacker);

							// Find extra info for means of death.
							std::stringstream modInfo;
							weapon_t probableKillingWeapon = (weapon_t)weaponFromMOD_GENERAL[mod];
							
							qboolean needMoreInfo = qtrue;
							if (probableKillingWeapon == WP_NONE_GENERAL) { // means something like doom kill
								switch (mod) {
									case MOD_UNKNOWN_GENERAL:
										modInfo << "_WEIRD";
										break;
									case MOD_FORCE_DARK_GENERAL:
										modInfo << "_FORCE";
										needMoreInfo = qfalse;
										break;
									case MOD_SENTRY_GENERAL:
										modInfo << "_SENTRY";
										needMoreInfo = qfalse;
										break;
									case MOD_WATER_GENERAL:
										modInfo << "_DROWN";
										break;
									case MOD_SLIME_GENERAL:
										modInfo << "_SLIME";
										break;
									case MOD_LAVA_GENERAL:
										modInfo << "_LAVA";
										break;
									case MOD_CRUSH_GENERAL:
										modInfo << "_CRUSH";
										break;
									case MOD_TELEFRAG_GENERAL:
										modInfo << "_TELE";
										break;
									case MOD_FALLING_GENERAL:
										modInfo << "_DOOM";
										break;
									case MOD_MELEE_GENERAL: // Kicked
										modInfo << "_KICK";
										break;
									case MOD_SUICIDE_GENERAL:
										modInfo << "_SUIC";
										break;
									case MOD_TARGET_LASER_GENERAL:
										modInfo << "_LASER";
										break;
									case MOD_TRIGGER_HURT_GENERAL:
										modInfo << "_TRIG";
										break;

									//JK3
									case MOD_TURBLAST_GENERAL:
										modInfo << "_TURBLAST";
										break;
									case MOD_VEHICLE_GENERAL:
										// Do special handling
										probableKillingWeapon = (weapon_t)generalizeGameValue<GMAP_WEAPONS, UNSAFE>(thisEs->generic1,demoType);
										switch (probableKillingWeapon)
										{
											case WP_BLASTER_GENERAL://primary blasters
												modInfo << "_VEHBLASTERS";
												needMoreInfo = qfalse;
												break;
											case WP_ROCKET_LAUNCHER_GENERAL://missile
												modInfo << "_VEHMISSILE";
												needMoreInfo = qfalse;
												break;
											case WP_THERMAL_GENERAL://bomb
												modInfo << "_VEHBOMB";
												needMoreInfo = qfalse;
												break;
											case WP_DEMP2_GENERAL://ion cannon
												modInfo << "_VEHION";
												needMoreInfo = qfalse;
												break;
											case WP_TURRET_GENERAL://turret
												modInfo << "_VEHTURRET";
												needMoreInfo = qfalse;
												break;
											default:
												modInfo << "_VEHICLE";
												break;
										}
										break;
									case MOD_COLLISION_GENERAL:
										modInfo << "_COLLIDE";
										break;
									case MOD_VEH_EXPLOSION_GENERAL:
										modInfo << "_VEHEXP";
										break;
									case MOD_TEAM_CHANGE_GENERAL:
										modInfo << "_TEAMCHANGE";
										break;

									// Q3
									case MOD_SHOTGUN_GENERAL:
										modInfo << "_SHOTGUN";
										needMoreInfo = qfalse;
										break;
									case MOD_GAUNTLET_GENERAL:
										modInfo << "_GAUNTLET";
										needMoreInfo = qfalse;
										break;
									case MOD_MACHINEGUN_GENERAL:
										modInfo << "_MG";
										needMoreInfo = qfalse;
										break;
									case MOD_GRENADE_GENERAL:
										modInfo << "_GREN";
										needMoreInfo = qfalse;
										break;
									case MOD_GRENADE_SPLASH_GENERAL:
										modInfo << "_GRENSPL";
										needMoreInfo = qfalse;
										break;
									case MOD_PLASMA_GENERAL:
										modInfo << "_PLAS";
										needMoreInfo = qfalse;
										break;
									case MOD_PLASMA_SPLASH_GENERAL:
										modInfo << "_PLASSPL";
										needMoreInfo = qfalse;
										break;
									case MOD_RAILGUN_GENERAL:
										modInfo << "_RAIL";
										needMoreInfo = qfalse;
										break;
									case MOD_LIGHTNING_GENERAL:
										modInfo << "_LIGHTNG";
										needMoreInfo = qfalse;
										break;
									case MOD_BFG_GENERAL:
										modInfo << "_BFG";
										needMoreInfo = qfalse;
										break;
									case MOD_BFG_SPLASH_GENERAL:
										modInfo << "_BFGSPL";
										needMoreInfo = qfalse;
										break;
									case MOD_NAIL_GENERAL:
										modInfo << "_NAIL";
										needMoreInfo = qfalse;
										break;
									case MOD_CHAINGUN_GENERAL:
										modInfo << "_CHAINGUN";
										needMoreInfo = qfalse;
										break;
									case MOD_PROXIMITY_MINE_GENERAL:
										modInfo << "_PROXMINE";
										needMoreInfo = qfalse;
										break;
									case MOD_KAMIKAZE_GENERAL:
										modInfo << "_KAMIKAZE";
										break;
									case MOD_JUICED_GENERAL:
										modInfo << "_JUICED";
										break;
									case MOD_GRAPPLE_GENERAL:
										modInfo << "_GRAPPLE";
										break;
									case MOD_SWITCH_TEAMS_GENERAL:
										modInfo << "_SWITCHTEAM";
										break;
									case MOD_THAW_GENERAL:
										modInfo << "_THAW";
										break;
									case MOD_LIGHTNING_DISCHARGE_GENERAL:
										modInfo << "_LIGHTNGDISCHRG";
										break;
									case MOD_HMG_GENERAL:
										modInfo << "_HMG";
										needMoreInfo = qfalse;
										break;
									case MOD_RAILGUN_HEADSHOT_GENERAL:
										modInfo << "_RAILHEADSHOT";
										needMoreInfo = qfalse;
										break;

									// MOHAA
									case MOD_CRUSH_EVERY_FRAME_GENERAL:
										modInfo << "_CRUSH";
										break;
									case MOD_LAST_SELF_INFLICTED_GENERAL:
										modInfo << "_SELFINFLICT";
										break;
									case MOD_EXPLOSION_GENERAL:
										modInfo << "_EXPL";
										break;
									case MOD_EXPLODEWALL_GENERAL:
										modInfo << "_EXPLWALL";
										break;
									case MOD_ELECTRIC_GENERAL:
										modInfo << "_ELECTR";
										break;
									case MOD_ELECTRICWATER_GENERAL:
										modInfo << "_ELECTRWAT";
										break;
									case MOD_THROWNOBJECT_GENERAL:
										modInfo << "_THROWOBJ";
										break;
									case MOD_BEAM_GENERAL:
										modInfo << "_BEAM";
										break;
									case MOD_BULLET_GENERAL:
										switch (thisEs->weapon) {
											case MOH_WEAPON_CLASS_PISTOL:
												modInfo << "_PISTOL";
												break;
											case MOH_WEAPON_CLASS_RIFLE:
												modInfo << "_RIFLE";
												break;
											case MOH_WEAPON_CLASS_SMG:
												modInfo << "_SMG";
												break;
											default:
												modInfo << "_BULLET";
												break;
										}
										needMoreInfo = qfalse;
										break;
									case MOD_FAST_BULLET_GENERAL:
										switch (thisEs->weapon) {
										case MOH_WEAPON_CLASS_PISTOL:
											modInfo << "_PISTOL";
											break;
										case MOH_WEAPON_CLASS_RIFLE:
											modInfo << "_RIFLE";
											break;
										case MOH_WEAPON_CLASS_SMG:
											modInfo << "_SMG";
											break;
										case MOH_WEAPON_CLASS_MG:
											modInfo << "_MG";
											break;
										default:
											modInfo << "_FASTBULLET";
											break;
										}
										needMoreInfo = qfalse;
										break;
									case MOD_FIRE_GENERAL:
										modInfo << "_FIRE";
										break;
									case MOD_FLASHBANG_GENERAL:
										modInfo << "_FLASH";
										needMoreInfo = qfalse;
										break;
									case MOD_ON_FIRE_GENERAL:
										modInfo << "_ONFIRE";
										break;
									case MOD_GIB_GENERAL:
										modInfo << "_GIB";
										break;
									case MOD_IMPALE_GENERAL:
										modInfo << "_IMPALE";
										break;
									case MOD_BASH_GENERAL:
										modInfo << "_BASH";
										break;
									case MOD_LANDMINE_GENERAL:
										modInfo << "_LANDMINE";
										break;
								}
								if(needMoreInfo)
									modInfo << "_~";
								
								if (!isMOHAADemo) {
									if (attackerIsFollowed) {
										probableKillingWeapon = (weapon_t)generalizeGameValue<GMAP_WEAPONS, UNSAFE>(demo.cut.Cl.snap.ps.weapon, demoType);
									}
									else if (attackerEntity) {
										probableKillingWeapon = (weapon_t)generalizeGameValue<GMAP_WEAPONS, SAFE>(attackerEntity->weapon, demoType); // Safe just in case theres some weird killer entity with weird values. Not a big deal inside this kill analysis part, wont be huge perfrmance impact
									}
								}
								else {
									// TODO does this work for main player too?
									probableKillingWeapon = attacker >= 0 && attacker <=max_clients ? (weapon_t)mohaaPlayerWeapon[attacker] : WP_NONE_GENERAL;
								}
							}

							char* mohHitLocationString = NULL;
							if (isMOHAADemo) {
								if (thisEs->time == 1) {
									modInfo << "_SNIPE";
								}
								if (thisEs->time != -1) {
									switch (thisEs->time)
									{
										case 0:
											mohHitLocationString = "head";
											break;
										case 1:
											mohHitLocationString = "helmet";
											break;
										case 2:
											mohHitLocationString = "neck";
											break;
										case 3:
											mohHitLocationString = "upper torso";
											break;
										case 4:
											mohHitLocationString = "middle torso";
											break;
										case 5:
											mohHitLocationString = "lower torso";
											break;
										case 6:
											mohHitLocationString = "pelvis";
											break;
										case 7:
											mohHitLocationString = "upper right arm";
											break;
										case 8:
											mohHitLocationString = "upper left arm";
											break;
										case 9:
											mohHitLocationString = "upper right leg";
											break;
										case 10:
											mohHitLocationString = "upper left leg";
											break;
										case 11:
											mohHitLocationString = "lower right arm";
											break;
										case 12:
											mohHitLocationString = "lower left arm";
											break;
										case 13:
											mohHitLocationString = "lower right leg";
											break;
										case 14:
											mohHitLocationString = "lower left leg";
											break;
										case 15:
											mohHitLocationString = "right hand";
											break;
										case 16:
											mohHitLocationString = "left hand";
											break;
										case 17:
											mohHitLocationString = "right foot";
											break;
										case 18:
											mohHitLocationString = "left foot";
											break;
										default:
											break;
									}
									switch (thisEs->time) // We only need to detect headshot for the highlight finding
									{
										case 0: // Head
											modInfo << "_HS"; // Headshot
											break;
										case 1: // Helmet
											modInfo << "_HSH";
											break;
										case 2: // Neck
											modInfo << "_HSN";
											break;
										default:
											break;
									}
								}
							}

							if (needMoreInfo) {


								switch (probableKillingWeapon) {
									case WP_SABER_GENERAL:
										isSaberKill = true;
										if (attackerIsFollowed) {

											modInfo << saberMoveNames_general[psGeneralSaberMove];
											if (!modInfo.str().size()) {
												//modInfo << saberStyleNames[demo.cut.Cl.snap.ps.fd.saberDrawAnimLevel];
												modInfo << saberStyleNames[demo.cut.Cl.snap.ps.fd.saberAnimLevel]; // animlevel is more correct. reflects the actual saber stance rather than the displayed one (which may not be locked in yet)
											}
											else {
												// It's a special attack.
												// Save speed at which it was executed.
												modInfo << "_" << (int)thisKill.speedatSaberMoveChange << "u";
											}

										}
										else {

											if (attackerEntity) {
												modInfo << saberMoveNames_general[generalizeGameValue<GMAP_LIGHTSABERMOVE, UNSAFE>(attackerEntity->saberMove,demoType)];
												if (!modInfo.str().size()) {
													// THIS IS INVALID!
													int saberStance = attackerEntity->fireflag;
													if (saberStance == 0) {
														// Server removed this info to prevent cheating I guess...
														// Let's make a guess.
														byte stanceProbability = 0; // 0-100
														int likelyStance = getLikelyStanceFromTorsoAnim(attackerEntity->torsoAnim, demoType, &stanceProbability);
														if (stanceProbability == 100) {
															modInfo << saberStyleNames[likelyStance] << "_G";
														}
														else {
															modInfo << saberStyleNames[likelyStance] << "_G" << (int)stanceProbability;
														}
													}
													else {

														modInfo << saberStyleNames[saberStance];
													}
													
												}
												else {
													// It's a special attack.
													// Save speed at which it was executed.
													modInfo <<"_" << (int)thisKill.speedatSaberMoveChange<<"u";
												}
											}
											else {
												modInfo << "_SABER";
											}
										}
										// Is the current saber move the follow up move of a bounce, deflect, parry, deflect, knockaway etc?
										//if ((playerLastSaberMove[attacker].lastLastSaberMove >= LS_K1_T_ && playerLastSaberMove[attacker].lastLastSaberMove <= LS_REFLECT_LL)
										//	|| (playerLastSaberMove[attacker].lastLastSaberMove >= LS_B1_BR && playerLastSaberMove[attacker].lastLastSaberMove <= LS_D1_B_)
										//	) {
										if (playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral >= LS_B1_BR_GENERAL && playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral <= LS_REFLECT_LL_GENERAL) {
											if (playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral == saberMoveData_general[playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral].chain_attack || playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral == saberMoveData_general[playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral].chain_idle) {
												// yep this is a chain from a parry or such.
												// not sure if we should include the stuff like broken parry which is followed by idle saber,
												// but i guess a broken parry followed by a burn kill could be fun to know too
												modInfo << "_FR";
												modInfo << saberMoveNames_general[playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral];
											}
										}
										// DFA from parry? If our attack is a DFA or other pre-swing attack resulting from a parry (well basically there's only dfa and yellow dfa I think that qualify)
										// BUT: This doesnt find them all for some reason. Idk why. Maybe sometimes they arent true parry dfas. Who knows. Often theres nothing in the sabermove history with a saber move above 74, but rather transitions and whatnot
										else if (playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral >= LS_A_JUMP_T__B__GENERAL && playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral <= LS_A_FLIP_SLASH_GENERAL) {
											if (playerLastSaberMove[attacker].lastSaberMove[2].saberMoveGeneral >= LS_B1_BR_GENERAL && playerLastSaberMove[attacker].lastSaberMove[2].saberMoveGeneral <= LS_REFLECT_LL_GENERAL) {
												if (playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral == saberMoveData_general[playerLastSaberMove[attacker].lastSaberMove[2].saberMoveGeneral].chain_attack || playerLastSaberMove[attacker].lastSaberMove[1].saberMoveGeneral == saberMoveData_general[playerLastSaberMove[attacker].lastSaberMove[2].saberMoveGeneral].chain_idle) {
													
													qboolean isSlowDfa = (qboolean)(playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral == LS_A_JUMP_T__B__GENERAL && (demoCurrentTime - playerLastSaberMove[attacker].lastSaberMove[1].saberMoveChange) > PARRY_DFA_DETECT_SLOW_THRESHOLD);
													qboolean isSlowYDfa = (qboolean)(playerLastSaberMove[attacker].lastSaberMove[0].saberMoveGeneral != LS_A_JUMP_T__B__GENERAL && (demoCurrentTime - playerLastSaberMove[attacker].lastSaberMove[1].saberMoveChange) > PARRY_YDFA_DETECT_SLOW_THRESHOLD);
													if (isSlowDfa || isSlowYDfa) {
														// We are interested mostly in those very fast parry DFAs
														// If its a dfa from a parry that takes a while to connect.. meh
														// Because it could just be a DFA as a result of fighting someone else and then it hits the other person after a second. 
														// Or its a dfa that the other person runs into again after a second. Not really that fascinating.
														// So we make it clear its a slow one by marking it _FRSL
														modInfo << "_FRSL";
													}
													else {
														modInfo << "_FR";
													}
													modInfo << saberMoveNames_general[playerLastSaberMove[attacker].lastSaberMove[2].saberMoveGeneral];

												}
											}
										}
										if (thisKill.timeSinceBackflip != -1 && thisKill.timeSinceBackflip < 1000) {
											modInfo << "_BF";
										}
										break;
									case WP_STUN_BATON_GENERAL:
										modInfo << "_STUN";
										break;
									case WP_BRYAR_PISTOL_GENERAL:
										modInfo << "_BRYAR";
										break;
									case WP_BLASTER_GENERAL:
										modInfo << "_BLASTER";
										break;
									case WP_DISRUPTOR_GENERAL:
										modInfo << "_DISRUPTOR";
										break;
									case WP_BOWCASTER_GENERAL:
										modInfo << "_BOWCAST";
										break;
									case WP_REPEATER_GENERAL:
										modInfo << "_REPEATER";
										break;
									case WP_DEMP2_GENERAL:
										modInfo << "_DEMP2";
										break;
									case WP_FLECHETTE_GENERAL:
										modInfo << "_FLECH";
										break;
									case WP_ROCKET_LAUNCHER_GENERAL:
										modInfo << "_ROCKET";
										break;
									case WP_THERMAL_GENERAL:
										modInfo << "_THERMAL";
										break;
									case WP_TRIP_MINE_GENERAL:
										modInfo << "_MINE";
										break;
									case WP_DET_PACK_GENERAL:
										modInfo << "_DTPCK";
										break;
									case WP_EMPLACED_GUN_GENERAL:
										modInfo << "_EMPLACED";
										break;
									case WP_TURRET_GENERAL:
										modInfo << "_TURRET";
										break;
									
									//JK 3
									case WP_MELEE_GENERAL:
										modInfo << "_MELEE";
										break;
									case WP_CONCUSSION_GENERAL:
										modInfo << "_CONC";
										break;
									case WP_BRYAR_OLD_GENERAL:
										modInfo << "_BRYAROLD";
										break;
									
									//q3
									case WP_GAUNTLET_GENERAL:
										modInfo << "_GAUNTLET";
										break;
									case WP_MACHINEGUN_GENERAL:
										modInfo << "_MACHINEGUN";
										break;
									case WP_SHOTGUN_GENERAL:
										modInfo << "_SHOTGUN";
										break;
									case WP_GRENADE_LAUNCHER_GENERAL:
										modInfo << "_GRENLAUNCH";
										break;
									case WP_LIGHTNING_GENERAL:
										modInfo << "_LIGHTNING";
										break;
									case WP_RAILGUN_GENERAL:
										modInfo << "_RAILGUN";
										break;
									case WP_PLASMAGUN_GENERAL:
										modInfo << "_PLASMA";
										break;
									case WP_BFG_GENERAL:
										modInfo << "_BFG";
										break;
									case WP_GRAPPLING_HOOK_GENERAL:
										modInfo << "_GRAPPLE";
										break;
									case WP_NAILGUN_GENERAL:
										modInfo << "_NAILGUN";
										break;
									case WP_PROX_LAUNCHER_GENERAL:
										modInfo << "_PROXMINE";
										break;
									case WP_CHAINGUN_GENERAL:
										modInfo << "_CHAINGUN";
										break;
									case WP_HEAVY_MACHINEGUN_GENERAL:
										modInfo << "_HMG";
										break;

									// MOHAA
									case WP_NONE_GENERAL:
										modInfo << "_WEIRD";
										break;
									case WP_PAPERS_GENERAL:
										modInfo << "_PAPERS";
										break;
									case WP_COLT45_GENERAL:
										modInfo << "_COLT45";
										break;
									case WP_P38_GENERAL:
										modInfo << "_P38";
										break;
									case WP_HISTANDARD_GENERAL:
										modInfo << "_HISTANDARD";
										break;
									case WP_GARAND_GENERAL:
										modInfo << "_GARAND";
										break;
									case WP_KAR98_GENERAL:
										modInfo << "_KAR98";
										break;
									case WP_KAR98SNIPER_GENERAL:
										modInfo << "_KAR98SNIPER";
										break;
									case WP_SPRINGFIELD_GENERAL:
										modInfo << "_SPRINGFIELD";
										break;
									case WP_THOMPSON_GENERAL:
										modInfo << "_THOMPSON";
										break;
									case WP_MP40_GENERAL:
										modInfo << "_MP40";
										break;
									case WP_BAR_GENERAL:
										modInfo << "_BAR";
										break;
									case WP_MP44_GENERAL:
										modInfo << "_MP44";
										break;
									case WP_FRAGGRENADE_GENERAL:
										modInfo << "_FRAGGRENADE";
										break;
									case WP_STIELHANDGRANATE_GENERAL:
										modInfo << "_STIELHANDGRANATE";
										break;
									case WP_BAZOOKA_GENERAL:
										modInfo << "_BAZOOKA";
										break;
									case WP_PANZERSCHRECK_GENERAL:
										modInfo << "_PANZERSCHRECK";
										break;
									case WP_UNARMED_GENERAL:
										modInfo << "_UNARMED";
										break;

									// MOH
									// Team Assault and Team Tactics weapons
									case WP_MG42_PORTABLE_GENERAL:
										modInfo << "_MG42_PORTABLE";
										break;
									case WP_WEBLEY_GENERAL:
										modInfo << "_WEBLEY";
										break;
									case WP_NAGANTREV_GENERAL:
										modInfo << "_NAGANTREV";
										break;
									case WP_BERETTA_GENERAL:
										modInfo << "_BERETTA";
										break;
									case WP_ENFIELD_GENERAL:
										modInfo << "_ENFIELD";
										break;
									case WP_SVT_GENERAL:
										modInfo << "_SVT";
										break;
									case WP_MOSIN_GENERAL:
										modInfo << "_MOSIN";
										break;
									case WP_G43_GENERAL:
										modInfo << "_G43";
										break;
									case WP_ENFIELDL42A_GENERAL:
										modInfo << "_ENFIELDL42A";
										break;
									case WP_CARCANO_GENERAL:
										modInfo << "_CARCANO";
										break;
									case WP_DELISLE_GENERAL:
										modInfo << "_DELISLE";
										break;
									case WP_STEN_GENERAL:
										modInfo << "_STEN";
										break;
									case WP_PPSH_GENERAL:
										modInfo << "_PPSH";
										break;
									case WP_MOSCHETTO_GENERAL:
										modInfo << "_MOSCHETTO";
										break;
									case WP_FG42_GENERAL:
										modInfo << "_FG42";
										break;
									case WP_VICKERS_GENERAL:
										modInfo << "_VICKERS";
										break;
									case WP_BREDA_GENERAL:
										modInfo << "_BREDA";
										break;
									case WP_F1_GRENADE_GENERAL:
										modInfo << "_F1_GRENADE";
										break;
									case WP_MILLS_GRENADE_GENERAL:
										modInfo << "_MILLS_GRENADE";
										break;
									case WP_NEBELHANDGRANATE_GENERAL:
										modInfo << "_NEBELHANDGRANATE";
										break;
									case WP_M18_SMOKE_GRENADE_GENERAL:
										modInfo << "_M18_SMOKE_GRENADE";
										break;
									case WP_RDG1_SMOKE_GRENADE_GENERAL:
										modInfo << "_RDG1_SMOKE_GRENADE";
										break;
									case WP_BOMBA_GENERAL:
										modInfo << "_BOMBA";
										break;
									case WP_BOMBA_BREDA_GENERAL:
										modInfo << "_BOMBA_BREDA";
										break;
									case WP_MINE_GENERAL:
										modInfo << "_MINE";
										break;
									case WP_MINE_DETECTOR_GENERAL:
										modInfo << "_MINE_DETECTOR";
										break;
									case WP_MINE_DETECTOR_AXIS_GENERAL:
										modInfo << "_MINE_DETECTOR_AXIS";
										break;
									case WP_DETONATOR_GENERAL:
										modInfo << "_DETONATOR";
										break;
									case WP_KAR98_MORTAR_GENERAL:
										modInfo << "_KAR98_MORTAR";
										break;
									case WP_PIAT_GENERAL:
										modInfo << "_PIAT";
										break;


									default:
										break;
								}
								
							}

							if (mod == MOD_SUICIDE_GENERAL && attacker == target) { 
								// attacker == target is a special condition I added because of JKA. JKA can attribute suicides to other players.
								// TODO: It's not perfect tho. Might misattribute force pulls/pushes where the attacker had saber equipped.
								isSaberKill = false;
							}

							thisKill.isSaberKill = isSaberKill;
							if (isSaberKill /* && !isWorldKill*/ && attackerIsFlagCarrier) {
								recentKillsDuringFlagHold[attacker].saberkills++; // I wanted to do this above with the rest of the recentKillsDuringFlagHold stuff, but up there I don't know if it was a saber kill yet
							}


							if (attackerJumpHeight > AIR_TO_AIR_DETECTION_HEIGHT_THRESHOLD && victimJumpHeight > AIR_TO_AIR_DETECTION_HEIGHT_THRESHOLD) {
								modInfo << "_A2A";
							}

							if (baseFlagDistanceKnownAndApplicable && baseFlagDistance < ALMOST_CAPTURE_DISTANCE) {
								modInfo << "_AC" << (int)baseFlagDistance;
							}

							bool willDoWallchecks = !isWorldKill && attackerIsVisibleOrFollowed && (opts.throughWallChecks == 2 && isSaberKill || opts.throughWallChecks == 1) && cm;
							
							if (isSaberKill && attackerIsFollowed && !targetIsVisibleOrFollowed && !willDoWallchecks) {
								modInfo << "_TW"; // Through wall. Meh, not very universal. But decent?
							}

							if (sameFrameRet) {
								modInfo << "_SFR"; 
							}

							bool throughWall = false;
							float throughWallNormal = 0;
							int throughWallOcclusion = 0;
							if (willDoWallchecks) {
								trace_t trace;
								vec3_t startpos;
								vec3_t endpos;
								static const vec3_t playerMins = { -15, -15, DEFAULT_MINS_2 + 1 }; // do +1 in case there's any issues with being in solid or whatever.
								static const vec3_t playerMaxs = { 15, 15, DEFAULT_MAXS_2 -1  }; // do -1 in case there's any issues with being in solid or whatever.
								static const vec3_t playerMaxsCrouch = { 15, 15, CROUCH_MAXS_2 -1  }; // do -1 in case there's any issues with being in solid or whatever.
								// do a trace through the center. if its blocked. do a trace from "hitbox edges" do determine just how occluded it really is.
								// why hitbox edges? because we know they must fit into the level. or at least are reasonably likely to.
								// might cause issues with crouched situations tho? do we just go for crouched size as a compromise?
								// or do we just do a boxtrace on a fixed location to figure out if they can stand?
								VectorCopy(thisFrameInfo.playerPositions[attacker], startpos);
								VectorCopy(thisEs->pos.trBase, endpos);
								// first check if we can stand
								float attackerMaxZ = DEFAULT_MAXS_2;
								float victimMaxZ = DEFAULT_MAXS_2;
								memset(&trace, 0, sizeof(trace));
								cm->CM_BoxTrace(&trace, startpos, startpos, playerMins, playerMaxs,0, MASK_PLAYERSOLID,qfalse);
								if (trace.allsolid || trace.startsolid) {
									attackerMaxZ = CROUCH_MAXS_2;
								}
								memset(&trace,0,sizeof(trace));
								cm->CM_BoxTrace(&trace, endpos, endpos, playerMins, playerMaxs, 0, MASK_PLAYERSOLID, qfalse);
								if (trace.allsolid || trace.startsolid) {
									victimMaxZ = CROUCH_MAXS_2;
								}

								// i'd say do the first trace as high as possible to avoid obstacles on the floor, for starters.
								startpos[2] += attackerMaxZ;
								endpos[2] += victimMaxZ;

								memset(&trace, 0, sizeof(trace));
								cm->CM_BoxTrace(&trace, startpos, endpos,NULL,NULL,0, MASK_PLAYERSOLID,qfalse);
								if (trace.fraction < 1.0f || trace.allsolid || trace.startsolid) {
									// ok there is some obstacle. lets calculate an "occlusion" amount for it by doing a few traces and checking the percentage of them that get blocked.
									int occludedTests = 0;
									int occluding = 0;
									throughWall = true;
									throughWallNormal = trace.plane.normal[2];
									// for each dimension we try the low and high val
									for (int x = 0; x < 2; x++) {
										startpos[0] = thisFrameInfo.playerPositions[attacker][0] - 15.0f + x * (15.0f + 15.0f);
										endpos[0] = thisEs->pos.trBase[0] - 15.0f + x * (15.0f + 15.0f);
										for (int y = 0; y < 2; y++) {
											startpos[1] = thisFrameInfo.playerPositions[attacker][1] - 15.0f + y * (15.0f + 15.0f);
											endpos[1] = thisEs->pos.trBase[1] - 15.0f + y * (15.0f + 15.0f);
											for (int z = 0; z < 2; z++) {
												startpos[2] = thisFrameInfo.playerPositions[attacker][2] + (float)DEFAULT_MINS_2 + z * (-DEFAULT_MINS_2 + attackerMaxZ);
												endpos[2] = thisEs->pos.trBase[2] + (float)DEFAULT_MINS_2 + z * (-DEFAULT_MINS_2 + victimMaxZ);
												memset(&trace, 0, sizeof(trace));
												cm->CM_BoxTrace(&trace, startpos, endpos, NULL, NULL, 0, MASK_PLAYERSOLID, qfalse);
												if (trace.allsolid || trace.startsolid) {
													errorFlags |= KAEF_OCCLUSION_SOLID_AFTER_BOXTEST;
													std::cout << "Through-Wall occlusion testing: Trace returned solid but shouldn't since we tested the box.\n";
												}
												else {
													occludedTests++;
													occluding += (trace.fraction < 1.0f) ? 1 : 0;
												}
											}
										}
									}
									throughWallOcclusion = 100 * occluding / occludedTests; // very rough percentage, doesnt need to be super precise since it's just 8 tests anyway
									if (throughWallOcclusion == 100) {
										modInfo << "_TW" << (int)((throughWallNormal) * 100.0f); // Through wall. Meh, not very universal. But decent?
									}
									else {
										modInfo << "_PTW" << throughWallOcclusion << "N" << (int)((throughWallNormal) * 100.0f); // Through wall. Meh, not very universal. But decent?
									}
								}
							}


							int killerProjectile = -1;
							qboolean isProjectileBased = qfalse;
							qboolean canBeAirborne = qfalse;
							qboolean projectileWasAirborne = qfalse;
							// TODO: Do this for other weapons too, not just mines
							if (mod == MOD_TRIP_MINE_SPLASH_GENERAL) { // If it's a trip mine kill, we wanna make sure that the mine that killed the victim was fired after a boost, else we ignore the boost.

								isProjectileBased = qtrue;
								canBeAirborne = qtrue;
								// Find the mine that killed him
								for (int i = 0; i < MAX_GENTITIES; i++) {
									if (lastFrameInfo.entityOwnerInfo[i].type == TET_TRIPMINE && lastFrameInfo.entityOwnerInfo[i].owner == attacker && lastFrameInfo.entityExists[i] && (!thisFrameInfo.entityExists[i] || (thisFrameInfo.entityOwnerInfo[i].flags & TETFLAG_EXPLODED))) { // Due to snaps dropping server frames, the mine could be either gone or on the exact frame of the explosion. Not a perfect solution, but good enough.
										// this is likely the tripmine that did the kill
										killerProjectile = i;
										if (thisFrameInfo.entityOwnerInfo[i].flags & TETFLAG_AIRBORNE || (!thisFrameInfo.entityExists[i] && lastFrameInfo.entityOwnerInfo[i].flags & TETFLAG_AIRBORNE)) {
											projectileWasAirborne = qtrue;
										}
										break;
									}
								}
							}
							if (projectileWasAirborne) {
								modInfo << "_AIR";
							}


							int headJumpCountAttacker = 0;
							int specialJumpCountAttacker = 0;
							if (attacker >= 0 && attacker < max_clients) {
								headJumpCountAttacker = headJumpCount[attacker];
								specialJumpCountAttacker = specialJumpCount[attacker];
							}
							if (headJumpCountAttacker > 0) {
								modInfo << "_HJ"<< headJumpCountAttacker;
							}
							if (specialJumpCountAttacker > 0) {
								modInfo << "_SJ"<< specialJumpCountAttacker;
							}
							if (timeSinceLastSelfSentryJump != -1 && timeSinceLastSelfSentryJump < 5000) {
								modInfo << "_SELFSENTRYJUMP"<< (int)timeSinceLastSelfSentryJump; // this is so rare that i'd rather just find all and decide if its worth keeping later
							}

							if (timeSinceLastSneak < 2000 && timeSinceLastSneak != -1) {
								modInfo << "_SNEAK";
							}

							
							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "mapname");
							std::string serverName = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "sv_hostname");
							std::string playername = "WEIRDATTACKER";
							const char* playerInfo;
							if (attacker >= 0 && attacker < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + attacker];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset,isMOHAADemo ? "name": "n");
							}
							offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + target];
							playerInfo = demo.cut.Cl.gameState.stringData + offset;
							std::string victimname = Info_ValueForKey(playerInfo,sizeof(demo.cut.Cl.gameState.stringData)- offset,isMOHAADemo?"name": "n");



							int64_t startTime = demoCurrentTime - bufferTime;
							int64_t endTime = demoCurrentTime + bufferTime;
							int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
							bool isTruncated = false;
							int truncationOffset = 0;
							if (earliestPossibleStart > startTime) {
								truncationOffset = earliestPossibleStart - startTime;
								startTime = earliestPossibleStart;
								isTruncated = true;
							}

							float maxSpeedAttackerFloat = getMaxSpeedForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);
							int maxSpeedAttacker = maxSpeedAttackerFloat;

							float maxAngularSpeedAttackerFloat = getMaxAngularSpeedForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);
							float maxAngularAccelerationAttackerFloat = getMaxAngularAccelerationForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);
							float maxAngularJerkAttackerFloat = getMaxAngularJerkForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);
							float maxAngularSnapAttackerFloat = getMaxAngularSnapForClientinTimeFrame(attacker, demoCurrentTime - 1000, demoCurrentTime);


							// Create hash.
							std::stringstream hashss;
							hashss << playername << "_" << victimname << "_" << serverName << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
							if (isMOHAADemo) { 
								// Using serverTime is very bad because different angles will have different serverTimes due to snaps etc.
								// But sadly this is the best we can do in MOHAA, as we don't get a real EV_OBITUARY
								hashss << demo.cut.Cl.snap.serverTime/1000; // Let's at least divide it by 1000. Will still obviously get different hashes for same kill, but more rarely.
								if (thisIsFakeEventEntity) {
									hashss << thisEs->time;
									hashss << thisEs->weapon;
								}
							}
							//auto sha3_512 = picosha3::get_sha3_generator<224>();
							//std::string hash_hex_string = sha3_512.get_hex_string(hashss.str());
							std::string hash_hex_string = makeStringHash(hashss.str());
							std::string shorthash = hash_hex_string.substr(0, std::min((size_t)3, hash_hex_string.size()));

							// Ok now with knowing the hash stuff, save into kills list
							thisKill.hashSourceString = hashss.str();
							thisKill.hash = hash_hex_string;
							thisKill.modInfoString = modInfo.str();
							thisKill.attackerName = playername;
							thisKill.victimName = victimname;


							// Find nearby players.
							std::stringstream nearbyPlayersSS;
							int nearbyPlayersCount = 0;
							for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
								entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
								if (thisEntitySub->number >= 0 && thisEntitySub->number < max_clients && thisEntitySub->number != attacker && thisEntitySub->number != target) {
									float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, thisEs->pos.trBase);
									if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
										nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " ("<<(int)nearbyPlayerDistance<<")";
										thisKill.nearbyPlayers.push_back({ thisEntitySub->number,nearbyPlayerDistance });
									}
								}
							}
							if (demo.cut.Cl.snap.ps.clientNum != target && demo.cut.Cl.snap.ps.clientNum != attacker) {
								float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, thisEs->pos.trBase);
								if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
									nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
									thisKill.nearbyPlayers.push_back({ demo.cut.Cl.snap.ps.clientNum,nearbyPlayerDistance });
								}
							}
							std::string nearbyPlayersString = nearbyPlayersSS.str();

#if KILLVECARRAY
							kills[KILLERARRAYINDEX(attacker)].push_back(thisKill);
							clientHasKills.setbit(KILLERARRAYINDEX(attacker));
#else
							kills[attacker].push_back(thisKill);
#endif



							if (opts.onlyLogSaberKills && !isSaberKill) {
								continue;
							}



							// Stats about nearby enemy count throughout run if victim was capper
							float maxNearbyEnemyCount = 0, moreThanOneNearbyEnemyTimePercent = 0, averageNearbyEnemyCount = -1, maxVeryCloseEnemyCount = 0, anyVeryCloseEnemyTimePercent = 0, moreThanOneVeryCloseEnemyTimePercent = 0, averageVeryCloseEnemyCount = -1;
							if (victimIsFlagCarrier) {
								// Resets if necessary.
								if (recentFlagHoldEnemyNearbyTimes[target].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[target]) {
									// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
									Com_Memset(&recentFlagHoldEnemyNearbyTimes[target], 0, sizeof(EnemyNearbyInfo));
									recentFlagHoldEnemyNearbyTimes[target].lastUpdateTime = demoCurrentTime;
								}
								averageHelper_t nearbyHelper, veryCloseHelper;
								Com_Memset(&nearbyHelper, 0, sizeof(nearbyHelper));
								Com_Memset(&veryCloseHelper, 0, sizeof(veryCloseHelper));
								for (int nearbyCount = 0; nearbyCount < max_clients; nearbyCount++) { // You'd think here it should be <= MAX_CLIENTS because 32 is a valid number, BUT with max of 32 players you can only have a max of 31 enemies. 32 is actually a valid index though, for "unknown", but we aren't currently tracking that. We just track for the time that the flag carrier WAS visible.
									int nearbyTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[target].enemyNearbyTimes[nearbyCount];
									int veryCloseTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[target].enemyVeryCloseTimes[nearbyCount];

									// If this count happened even for the shortest amount of time, that was the max amount of near/very close enemies.
									if (nearbyTimeOfThisCount && nearbyCount > maxNearbyEnemyCount) maxNearbyEnemyCount = nearbyCount;
									if (veryCloseTimeOfThisCount && nearbyCount > maxVeryCloseEnemyCount) maxVeryCloseEnemyCount = nearbyCount;

									nearbyHelper.sum += nearbyCount * nearbyTimeOfThisCount;
									nearbyHelper.divisor += nearbyTimeOfThisCount;
									veryCloseHelper.sum += nearbyCount * veryCloseTimeOfThisCount;
									veryCloseHelper.divisor += veryCloseTimeOfThisCount;

									if (nearbyCount > 0) {
										anyVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
										if (nearbyCount > 1) {
											moreThanOneNearbyEnemyTimePercent += nearbyTimeOfThisCount;
											moreThanOneVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
										}
									}
								}
								averageNearbyEnemyCount = nearbyHelper.divisor == 0 ? 0 : nearbyHelper.sum / nearbyHelper.divisor;
								averageVeryCloseEnemyCount = veryCloseHelper.divisor == 0 ? 0 : veryCloseHelper.sum / veryCloseHelper.divisor;
								moreThanOneNearbyEnemyTimePercent *= nearbyHelper.divisor == 0 ? 0 : 100.0f / nearbyHelper.divisor;
								moreThanOneVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
								anyVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
							}
							// END Stats about nearby enemy count throughout run


							// Boosts that led to this kill
							std::stringstream boostsStringStream;
							int boostCountAttacker = 0;
							int boostCountVictim = 0;
							if (/*mod != MOD_SUICIDE*/ true) { // If it's a literal "/kill" console command, let's just ignore boosts. Why bother.

								int64_t excludeAttackerBoostsAfter = INT64_MAX;

								/*if (mod == MOD_TRIP_MINE_SPLASH) { // If it's a trip mine kill, we wanna make sure that the mine that killed the victim was fired after a boost, else we ignore the boost.
									// Find the mine that killed him
									for (int i = 0; i < MAX_GENTITIES; i++) {
										if (lastFrameInfo.entityOwnerInfo[i].type == TET_TRIPMINE && lastFrameInfo.entityOwnerInfo[i].owner == attacker && lastFrameInfo.entityExists[i] && (!thisFrameInfo.entityExists[i] || (thisFrameInfo.entityOwnerInfo[i].flags & TETFLAG_EXPLODED))) { // Due to snaps dropping server frames, the mine could be either gone or on the exact frame of the explosion. Not a perfect solution, but good enough.
											// this is likely the tripmine that did the kill
											excludeBoostsAfter = lastFrameInfo.entityOwnerInfo[i].firstSeen;
											break;
										}
									}
								}*/
								if (isProjectileBased && killerProjectile != -1) {
									excludeAttackerBoostsAfter = lastFrameInfo.entityOwnerInfo[killerProjectile].firstSeen;
								}

								for (int i = 0; i < boosts.size(); i++) {
									if ((demoCurrentTime - boosts[i].demoTime) > BOOST_DETECT_MAX_AGE) continue; // meh
									qboolean doThis = qfalse;
									// find out if we should even bother
									if (boosts[i].boostedClientNum == attacker && boosts[i].boosterClientNum != target) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts
									
										if (boosts[i].demoTime > excludeAttackerBoostsAfter) continue;
										
										// Ignore mini boosts?
										//if (!boosts[i].confirmed && VectorDistance(thisFrameInfo.playerPositions[boosts[i].boostedClientNum], boosts[i].startPosition) < boosts[i].minimumTravelDistance) continue;
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;

										if (!boosts[i].checkFacingTowards(target)) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

										boostCountAttacker++;
										//if (groundFrameQuality != -1 && lastGroundFrame[attacker] ) {
										// TODO somehow make boosts not be counted as giga groundframes?
										//}
										doThis = qtrue;
										boostsStringStream << "[KILLER by";
									}
									else if (boosts[i].boostedClientNum == target && boosts[i].boosterClientNum != attacker) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts

										// Ignore mini boosts?
										//if (!boosts[i].confirmed && VectorDistance(thisFrameInfo.playerPositions[boosts[i].boostedClientNum], boosts[i].startPosition) < boosts[i].minimumTravelDistance) continue;
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;
										
										if (!((attacker < 0 || attacker >= max_clients) || boosts[i].checkFacingTowards(attacker))) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

										boostCountVictim++;
										doThis = qtrue;
										boostsStringStream << "[VICTIM by";
									}
									if (!doThis) continue;

									// ok let's bother
									std::string boosterName = "";
									if (boosts[i].boosterClientNum != -1) {
										int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + boosts[i].boosterClientNum];
										const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
										boosterName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset,isMOHAADemo ? "name": "n");
									}
									if (boosts[i].isEnemyBoost != -1) {
										boostsStringStream << (boosts[i].isEnemyBoost ? (target == boosts[i].boosterClientNum ? " VICTIM]":" ENEMY]") :(boosts[i].boosterClientNum == attacker ? " SELF]" : " TEAM]"));
									}
									else {
										boostsStringStream << " UNKNOWN]";
									}
									if (boosterName.size() > 0) {
										boostsStringStream << " " << boosterName;
									}
									boostsStringStream << " (" << (boosts[i].demoTime- demoCurrentTime) << ", ~"<< boosts[i].estimatedStrength <<")";
									boostsStringStream << "\n";
								}
							}
							std::string boostsString = boostsStringStream.str();





							std::stringstream logModStringSS;
							logModStringSS << (victimIsFlagCarrier ? "RET" : "MOD") << modInfo.str();
							std::string logModString = logModStringSS.str();
							const char* modString = logModString.c_str();

							if (opts.makeVideo) {
								if (isSuicide) {
									videoConsole.push_back({ demoCurrentTime,va("^7%s ^7%s",playername.c_str(),modString) });
								}
								else {
									videoConsole.push_back({ demoCurrentTime,va("^7%s ^7%s ^7%s",playername.c_str(),modString,victimname.c_str()) });
								}
								if (attackerIsFollowed && !isSuicide) {
									screenCenterText.push_back({ demoCurrentTime,va("^7%s ^7%s",modString,victimname.c_str()) });
								}
							}

							SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
							SQLDelayedQuery* query = &queryWrapper->query;

							// Log the kill.
							SQLBIND_DELAYED_TEXT(query, "@hash", hash_hex_string.c_str());
							SQLBIND_DELAYED_TEXT(query, "@shorthash", shorthash.c_str());
							SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@killerName", playername.c_str(), QF_PLAYERNAME0);
							std::string playernameStripped = Q_StripColorAll(playername);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@killerNameStripped", playernameStripped.c_str(), QF_PLAYERNAME0STRIPPED);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@victimName", victimname.c_str(), QF_PLAYERNAME1);
							std::string victimnameStripped = Q_StripColorAll(victimname);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@victimNameStripped", victimnameStripped.c_str(), QF_PLAYERNAME1STRIPPED);
							if (isWorldKill) {
								SQLBIND_DELAYED_NULL(query, "@killerTeam");
							}
							else {
								SQLBIND_DELAYED(query, int, "@killerTeam", playerTeams[attacker]);
							}
							SQLBIND_DELAYED(query, int, "@victimTeam", playerTeams[target]);
							SQLBIND_DELAYED(query, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND_DELAYED(query, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							if (victimIsFlagCarrier) {
								SQLBIND_DELAYED(query, int, "@otherFlagStatus",playerTeams[target] == TEAM_BLUE ? cgs.flag[TEAM_BLUE]: cgs.flag[TEAM_RED]);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@otherFlagStatus");
							}
							SQLBIND_DELAYED(query, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND_DELAYED(query, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND_DELAYED(query, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount +  teamInfo[TEAM_BLUE].playerCount + teamInfo[TEAM_RED].playerCount);
							SQLBIND_DELAYED(query, int, "@killerClientNum", attacker);
							SQLBIND_DELAYED(query, int, "@victimClientNum", target);
							SQLBIND_DELAYED(query, int, "@isDoomKill", isDoomKill);
							SQLBIND_DELAYED(query, int, "@isExplosion", thisKill.isExplosion);
							SQLBIND_DELAYED(query, int, "@isSuicide", isSuicide);
							SQLBIND_DELAYED(query, int, "@isModSuicide", mod==MOD_SUICIDE_GENERAL);
							SQLBIND_DELAYED(query, int, "@meansOfDeath", mod); // Original or generalized? hmm
							SQLBIND_DELAYED(query, double, "@positionX", thisEs->pos.trBase[0]);
							SQLBIND_DELAYED(query, double, "@positionY", thisEs->pos.trBase[1]);
							SQLBIND_DELAYED(query, double, "@positionZ", thisEs->pos.trBase[2]);

							if (activeKillDatabase != -1) {
								queryWrapper->databaseIndex = activeKillDatabase;
								io.killQueries->push_back(queryWrapper);
							}
							else {
								delete queryWrapper;
								continue;
							}

							SQLDelayedQueryWrapper_t* angleQueryWrapper = new SQLDelayedQueryWrapper_t();
							SQLDelayedQuery* angleQuery = &angleQueryWrapper->query;

							SQLBIND_DELAYED_TEXT(angleQuery, "@hash", hash_hex_string.c_str());
							SQLBIND_DELAYED_TEXT(angleQuery, "@shorthash", shorthash.c_str());
							SQLBIND_DELAYED_TEXT(angleQuery, "@map", mapname.c_str());
							SQLBIND_DELAYED_TEXT(angleQuery, "@serverName", serverName.c_str());
							std::string serverNameStripped = Q_StripColorAll(serverName);
							SQLBIND_DELAYED_TEXT(angleQuery, "@serverNameStripped", serverNameStripped.c_str());
							SQLBIND_DELAYED(angleQuery, int, "@isReturn", victimIsFlagCarrier);
							SQLBIND_DELAYED(angleQuery, int, "@isTeamKill", isTeamKill);
							SQLBIND_DELAYED(angleQuery, int, "@killerIsFlagCarrier", attackerIsFlagCarrier);
							if (victimIsFlagCarrier) {
								SQLBIND_DELAYED(angleQuery, int, "@victimFlagHoldTime", recentFlagHoldTimes[target]);
								//SQLBIND(io.insertAngleStatement, int, "@flagPickupSource", teamInfo[victimFlagTeam].flagHoldOrigin);
								SQLBIND_DELAYED(angleQuery, int, "@victimFlagPickupSource", victimCarrierLastPickupOrigin);
								SQLBIND_DELAYED(angleQuery, int, "@victimCapperKills", victimFlagCarrierKillCount);
								SQLBIND_DELAYED(angleQuery, int, "@victimCapperRets", victimFlagCarrierRetCount);
								SQLBIND_DELAYED(angleQuery, int, "@victimCapperWasFollowedOrVisible", capperWasVisibleOrFollowed);

								SQLBIND_DELAYED(angleQuery, double, "@victimCapperMaxNearbyEnemyCount", maxNearbyEnemyCount);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperMoreThanOneNearbyEnemyTimePercent", moreThanOneNearbyEnemyTimePercent);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperAverageNearbyEnemyCount", averageNearbyEnemyCount);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperMaxVeryCloseEnemyCount", maxVeryCloseEnemyCount);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperAnyVeryCloseEnemyTimePercent", anyVeryCloseEnemyTimePercent);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperMoreThanOneVeryCloseEnemyTimePercent", moreThanOneVeryCloseEnemyTimePercent);
								SQLBIND_DELAYED(angleQuery, double, "@victimCapperAverageVeryCloseEnemyCount", averageVeryCloseEnemyCount);

								SQLBIND_DELAYED(angleQuery, int, "@sameFrameRet", sameFrameRet);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@victimFlagHoldTime");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimFlagPickupSource");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperKills");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperRets");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperWasFollowedOrVisible");

								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperMaxNearbyEnemyCount");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperMoreThanOneNearbyEnemyTimePercent");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperAverageNearbyEnemyCount");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperMaxVeryCloseEnemyCount");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperAnyVeryCloseEnemyTimePercent");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperMoreThanOneVeryCloseEnemyTimePercent");
								SQLBIND_DELAYED_NULL(angleQuery, "@victimCapperAverageVeryCloseEnemyCount");

								SQLBIND_DELAYED_NULL(angleQuery, "@sameFrameRet");
							}
							SQLBIND_DELAYED(angleQuery, int, "@targetIsVisible", targetIsVisible);
							SQLBIND_DELAYED(angleQuery, int, "@targetIsFollowed", targetIsFollowed);
							SQLBIND_DELAYED(angleQuery, int, "@targetIsFollowedOrVisible", targetIsVisibleOrFollowed);
							SQLBIND_DELAYED(angleQuery, int, "@isSuicide", isSuicide);
							SQLBIND_DELAYED(angleQuery, int, "@isModSuicide", mod == MOD_SUICIDE_GENERAL);
							SQLBIND_DELAYED(angleQuery, int, "@attackerIsVisible", attackerIsVisible);
							SQLBIND_DELAYED(angleQuery, int, "@attackerIsFollowed", attackerIsFollowed);
							SQLBIND_DELAYED(angleQuery, int, "@attackerIsFollowedOrVisible", attackerIsVisibleOrFollowed);
							SQLBIND_DELAYED(angleQuery, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);

							SQLBIND_DELAYED_TEXT(angleQuery, "@boosts", (boostCountAttacker + boostCountVictim) > 0 ? boostsString.c_str() : NULL);
							SQLBIND_DELAYED(angleQuery, int, "@boostCountTotal", boostCountAttacker + boostCountVictim);
							SQLBIND_DELAYED(angleQuery, int, "@boostCountAttacker", boostCountAttacker);
							SQLBIND_DELAYED(angleQuery, int, "@boostCountVictim", boostCountVictim);


							if (canBeAirborne) {
								if (killerProjectile == -1) {
									SQLBIND_DELAYED_NULL(angleQuery, "@projectileWasAirborne");
								}
								else {
									SQLBIND_DELAYED(angleQuery, int, "@projectileWasAirborne", projectileWasAirborne);
								}
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@projectileWasAirborne");
							}

							if (attacker >= 0 && attacker < max_clients) {
								SQLBIND_DELAYED(angleQuery, int, "@headJumps", headJumpCountAttacker);
								SQLBIND_DELAYED(angleQuery, int, "@specialJumps", specialJumpCountAttacker);
								SQLBIND_DELAYED(angleQuery, int, "@timeSinceLastSelfSentryJump", timeSinceLastSelfSentryJump);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@headJumps");
								SQLBIND_DELAYED_NULL(angleQuery, "@specialJumps");
								SQLBIND_DELAYED_NULL(angleQuery, "@timeSinceLastSelfSentryJump");
							}

#if TRACK_GROUNDFRAME
							if (!isWorldKill && groundFrameQuality != -1) {
								SQLBIND_DELAYED(angleQuery, double, "@groundFrameQuality", groundFrameQuality);
								SQLBIND_DELAYED(angleQuery, double, "@groundFrameAngleChange", groundFrameAnglechange);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@groundFrameQuality");
								SQLBIND_DELAYED_NULL(angleQuery, "@groundFrameAngleChange");
							}
#endif

							if (isSaberKill && !isWorldKill && attackerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(angleQuery, int, "@saberComboCount", saberComboCounter[attacker]);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@saberComboCount");
							}

							if (throughWall) {
								SQLBIND_DELAYED(angleQuery, double, "@throughWallNormal", throughWallNormal);
								SQLBIND_DELAYED(angleQuery, int, "@throughWallOcclusion", throughWallOcclusion);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@throughWallNormal");
								SQLBIND_DELAYED_NULL(angleQuery, "@throughWallOcclusion");
							}


							if (!isWorldKill && timeSinceLastSneak < 10000 && timeSinceLastSneak != -1) {
								SQLBIND_DELAYED(angleQuery, int, "@lastSneak", timeSinceLastSneak);
								SQLBIND_DELAYED(angleQuery, int, "@lastSneakDuration", lastSneakDuration[attacker]);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@lastSneak");
								SQLBIND_DELAYED_NULL(angleQuery, "@lastSneakDuration");
							}

							if (baseFlagDistanceKnownAndApplicable) {
								SQLBIND_DELAYED(angleQuery, double, "@baseFlagDistance", baseFlagDistance);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@baseFlagDistance");
							}


							SQLBIND_DELAYED(angleQuery, double, "@maxAngularSpeedAttacker", maxAngularSpeedAttackerFloat >= 0 ? maxAngularSpeedAttackerFloat : NULL);
							SQLBIND_DELAYED(angleQuery, double, "@maxAngularAccelerationAttacker", maxAngularAccelerationAttackerFloat >= 0 ? maxAngularAccelerationAttackerFloat * accelerationMultiplier : NULL);
							SQLBIND_DELAYED(angleQuery, double, "@maxAngularJerkAttacker", maxAngularJerkAttackerFloat >= 0 ? maxAngularJerkAttackerFloat * jerkMultiplier : NULL);
							SQLBIND_DELAYED(angleQuery, double, "@maxAngularSnapAttacker", maxAngularSnapAttackerFloat >= 0 ? maxAngularSnapAttackerFloat *snapMultiplier : NULL);


							SQLBIND_DELAYED(angleQuery, double, "@maxSpeedAttacker", maxSpeedAttackerFloat >= 0 ? maxSpeedAttackerFloat : NULL);
							SQLBIND_DELAYED(angleQuery, double, "@maxSpeedTarget", maxSpeedTargetFloat >= 0 ? maxSpeedTargetFloat : NULL);
							SQLBIND_DELAYED(angleQuery, double, "@lastSaberMoveChangeSpeed", thisKill.speedatSaberMoveChange >= 0 ? thisKill.speedatSaberMoveChange : NULL);
							SQLBIND_DELAYED(angleQuery, int, "@timeSinceLastSaberMoveChange", thisKill.timeSinceSaberMoveChange >= 0 ? thisKill.timeSinceSaberMoveChange : NULL);
							if (thisKill.timeSinceBackflip >= 0) {
								SQLBIND_DELAYED(angleQuery, int, "@timeSinceLastBackflip",thisKill.timeSinceBackflip);
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@timeSinceLastBackflip");
							}
							SQLBIND_DELAYED_TEXT(angleQuery, "@meansOfDeathString", modString);
							SQLBIND_DELAYED_TEXT(angleQuery, "@nearbyPlayers", thisKill.nearbyPlayers.size() > 0? nearbyPlayersString.c_str():NULL);
							SQLBIND_DELAYED(angleQuery, int, "@nearbyPlayerCount", thisKill.nearbyPlayers.size());
							SQLBIND_DELAYED(angleQuery, int, "@probableKillingWeapon", probableKillingWeapon);

							SQLBIND_DELAYED(angleQuery, double, "@attackerJumpHeight", attackerJumpHeight);
							SQLBIND_DELAYED(angleQuery, double, "@victimJumpHeight", victimJumpHeight);

							if (attackerIsFollowed) {
								SQLBIND_DELAYED(angleQuery, double, "@directionX", demo.cut.Cl.snap.ps.velocity[0]);
								SQLBIND_DELAYED(angleQuery, double, "@directionY", demo.cut.Cl.snap.ps.velocity[1]);
								SQLBIND_DELAYED(angleQuery, double, "@directionZ", demo.cut.Cl.snap.ps.velocity[2]);
								SQLBIND_DELAYED(angleQuery, double, "@currentSpeedAttacker", VectorLength(demo.cut.Cl.snap.ps.velocity));
							} else if(attackerEntity){
								SQLBIND_DELAYED(angleQuery, double, "@directionX", attackerEntity->pos.trDelta[0]);
								SQLBIND_DELAYED(angleQuery, double, "@directionY", attackerEntity->pos.trDelta[1]);
								SQLBIND_DELAYED(angleQuery, double, "@directionZ", attackerEntity->pos.trDelta[2]);
								SQLBIND_DELAYED(angleQuery, double, "@currentSpeedAttacker", VectorLength(attackerEntity->pos.trDelta));
							} else {
								SQLBIND_DELAYED_NULL(angleQuery,  "@directionX");
								SQLBIND_DELAYED_NULL(angleQuery,  "@directionY");
								SQLBIND_DELAYED_NULL(angleQuery,  "@directionZ");
								SQLBIND_DELAYED_NULL(angleQuery,  "@currentSpeedAttacker");
							}
							if (targetIsFollowed) {
								SQLBIND_DELAYED(angleQuery, double, "@currentSpeedTarget", VectorLength(demo.cut.Cl.snap.ps.velocity));
							}
							else if (targetEntity) {
								SQLBIND_DELAYED(angleQuery, double, "@currentSpeedTarget", VectorLength(targetEntity->pos.trDelta));
							}
							else {
								SQLBIND_DELAYED_NULL(angleQuery, "@currentSpeedTarget");
							}
							SQLBIND_DELAYED_TEXT(angleQuery, "@demoName", sharedVars.oldBasename.c_str());
							SQLBIND_DELAYED_TEXT(angleQuery, "@demoPath", sharedVars.oldPath.c_str());
							SQLBIND_DELAYED(angleQuery, int, "@demoTime", demoCurrentTime);
							SQLBIND_DELAYED(angleQuery, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime);
							SQLBIND_DELAYED(angleQuery, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND_DELAYED(angleQuery, int, "@demoDateTime", sharedVars.oldDemoDateModified);
							SQLBIND_DELAYED(angleQuery, int, "@demoDerivativeFlags", demoDerivativeFlags);
							SQLBIND_DELAYED(angleQuery, int, "@errorFlags", errorFlags);

							if (activeKillDatabase != -1) {
								angleQueryWrapper->databaseIndex = activeKillDatabase;
								io.killAngleQueries->push_back(angleQueryWrapper);
							}
							else {
								delete angleQueryWrapper;
								continue;
							}


							addMetaEvent(METAEVENT_DEATH, demoCurrentTime, target, bufferTime, opts,bufferTime);
							if(attacker >= 0 && attacker < max_clients && target != attacker){
								MetaEventTracker* killME = new MetaEventTracker(demoCurrentTime, angleQueryWrapper, metaEventTrackers[METRACKER_KILLS][attacker],bufferTime,0, resultingMetaEventTracking[METRACKER_KILLS]);
								bool wasFollowedThroughBufferTime = playerFirstFollowed[attacker] != -1 && playerFirstFollowed[attacker] < (demo.cut.Cl.snap.serverTime - bufferTime);
								killME->reframeClientNum = attacker;
								killME->needsReframe = opts.reframeIfNeeded && !wasFollowedThroughBufferTime;
								killME->addPastEvents(playerPastMetaEvents[attacker], getMinimumMetaEventBufferTime(attacker,bufferTime,demoCurrentTime));
								addMetaEvent(victimIsFlagCarrier ? METAEVENT_RETURN : METAEVENT_KILL, demoCurrentTime, attacker, bufferTime, opts, bufferTime);
								metaEventTrackers[METRACKER_KILLS][attacker] = killME;
							}


							//if (isSuicide || !victimIsFlagCarrier || isWorldKill || !targetIsVisible) continue; // Not that interesting.
							if (isSuicide || (!victimIsFlagCarrier && searchMode != SEARCH_ALL_KILLS && searchMode != SEARCH_ALL_MY_KILLS) || isWorldKill || (!targetIsVisibleOrFollowed && !attackerIsVisibleOrFollowed)) continue; // Not that interesting.
							// If it's not a doom kill, it's not that interesting unless we specifically are searching for our own returns or searching for everything
							if (!isDoomKill && searchMode != SEARCH_ALL_KILLS && searchMode != SEARCH_ALL_MY_KILLS && searchMode != SEARCH_ALL && searchMode != SEARCH_MY_CTF_RETURNS && searchMode != SEARCH_CTF_RETURNS) continue;
							if (!attackerIsFollowed && (searchMode == SEARCH_MY_CTF_RETURNS|| searchMode == SEARCH_ALL_MY_KILLS)) continue; // We are searching for our own kills.
							if (!attackerIsVisibleOrFollowed) continue; // Attacker isn't visible at all. Meh. Leave it.


							std::stringstream ss;
							std::stringstream ss2;
							std::string boostString = ((boostCountAttacker + boostCountVictim) > 0 ?( va("_BST%s%s", boostCountAttacker > 0 ? va("%dA", boostCountAttacker) : "", boostCountVictim > 0 ? va("%dV", boostCountVictim) : "")) : "");
							ss << mapname << std::setfill('0') << "___" << logModString << boostString;
							ss2 << "___" << playername << "___" << victimname << "___" << maxSpeedAttacker << "_" << maxSpeedTarget << "ups" << (attackerIsFollowed ? "" : "___thirdperson") << (attackerIsVisibleOrFollowed ? "" : "_attackerInvis") << (targetIsVisibleOrFollowed ? "" : "_targetInvis") << "_" << attacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "") << "_" << shorthash;

							std::string targetFilename = ss.str();
							std::string targetFilename2 = ss2.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							char* targetFilename2Filtered = new char[targetFilename2.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);
							sanitizeFilename(targetFilename2.c_str(), targetFilename2Filtered);

							std::stringstream batSS;
							std::stringstream batSS2;
							batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
							batSS << "\nrem hash: " << hash_hex_string;
							batSS << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered; 
							batSS2 << targetFilename2Filtered << "\" " << startTime << " " << endTime;
							batSS2 << (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");

							angleQueryWrapper->batchString1 = batSS.str();
							angleQueryWrapper->batchString2 = batSS2.str();
							
							delete[] targetFilenameFiltered;
							delete[] targetFilename2Filtered;
							if (!opts.noFindOutput)  std::cout << mapname << " " << logModString << boostString << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "___" << maxSpeedAttacker << "_" << maxSpeedTarget << "ups" << "\n";

						}

						
						else if (eventNumber >= EV_PAIN_GENERAL && eventNumber <= EV_DEATH3_GENERAL) {
							// Player took some damage. Could indicate a saber hit if an EV_SABER_HIT is nearby
							int playerNum = thisEs->number;
							if (thisEs->eFlags & EF_PLAYER_EVENT) {
								playerNum = thisEs->otherEntityNum;
							}
							if (playerNum >= 0 && playerNum < max_clients) {
								hitDetectionData[playerNum].painDetected = qtrue;
							}
						}

						else if (eventNumber == EV_JUMP_GENERAL) {

							int clientNumJump = thisEs->number < max_clients ? thisEs->number : thisEs->otherEntityNum;
							if(clientNumJump >=0 && clientNumJump < max_clients){ // idk why it wouldnt but be safe. someone could alwayss mess with a demo or make weird mods.
								jumpDetected[clientNumJump] = qtrue;
							}
						}
					
						else if (eventNumber == EV_SABER_HIT_GENERAL && thisEs->eventParm == 1) { // Saber hit against client
							
							// Mark nearby players for potential saber hit detection
							for (int playerNum = 0; playerNum < max_clients; playerNum++) {
								constexpr float maxDistance = 100.0f + 2.0f * (float)SABER_LENGTH_MAX;
								if (thisFrameInfo.entityExists[playerNum] && VectorDistance(thisFrameInfo.playerPositions[playerNum], thisEs->pos.trBase) <= maxDistance) {
									hitDetectionData[playerNum].nearbySaberHitDetected = qtrue;
									addMetaEvent(METAEVENT_SABERHIT, demoCurrentTime, playerNum, bufferTime, opts, bufferTime);
								}
							}
						}
						else if (eventNumber == EV_SHIELD_HIT_GENERAL) { // How to do this as meta event? idk. should I?
							int playerNum = thisEs->otherEntityNum;
							if (playerNum >= 0 && playerNum < max_clients) {
								hitDetectionData[playerNum].confirmedHit = qtrue;
							}
						}
						else if (eventNumber == EV_SABER_BLOCK_GENERAL || eventNumber == EV_SABER_CLASHFLARE_GENERAL) {
							// We don't get anything useful out of this per se, but we can use it to sync with music for example for effect and cool.
							addMetaEventNearby<max_clients>(thisEs->pos.trBase,200, METAEVENT_SABERBLOCK, demoCurrentTime, bufferTime, opts, bufferTime);
						}
						else if (eventNumber == EV_PLAY_EFFECT_GENERAL || eventNumber == EV_PLAY_EFFECT_ID_GENERAL) {
							// We don't get anything useful out of this per se, but we can use it to sync with music for example for effect and cool.
							// tripmine has splash radius 256.0f. lets add 50 to make it safe (since stuff can move).
							// There is no weapon (at least in JK2) with higher splash radius I believe.
							addMetaEventNearby<max_clients>(thisEs->pos.trBase, 256.0f + 50.0f, METAEVENT_EFFECT, demoCurrentTime, bufferTime, opts, bufferTime); 
						}
						else if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_PLAYER_GOT_FLAG) {
							if (opts.makeVideo) {
								int playerNum = thisEs->trickedentindex;
								int flagTeam = thisEs->trickedentindex2;
								std::string playername = "WEIRDONAME";
								const char* playerInfo;
								if (playerNum >= 0 && playerNum < max_clients) {
									int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
									playerInfo = demo.cut.Cl.gameState.stringData + offset;
									playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
								}

								if (opts.makeVideo) {
									videoConsole.push_back({ demoCurrentTime,va("^7%s ^7got the %s flag",playername.c_str(),flagTeam == TEAM_RED ? "RED" : (flagTeam == TEAM_BLUE ? "BLUE" : "YELLOW")) });
								}
							}
							//Flaggrab.
							int grabberPlayerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;

							int playerNum = -1;// we track info about both the grabber and the enemy team capper
							// TODO i might need to overthink this some more at some point re. whether i have the correct capper.
							// but let's call it a day for now since this is mainly about the flag grab.
							switch (flagTeam) {
							case TEAM_FREE: // Is this correct?
								//victimCarrierLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_FREE];
								break;
							case TEAM_RED:
								//victimCarrierLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_RED];
								playerNum = lastKnownFlagCarrier[TEAM_BLUE]; // if we picked red flag, we wanna know who (if anyone) carries blue flag
								break;
							case TEAM_BLUE:
								//victimCarrierLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_BLUE];
								playerNum = lastKnownFlagCarrier[TEAM_RED]; // if we picked blue flag, we wanna know who (if anyone) carries red flag
								break;
							}

							//bool sameFrameCap = false;

							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "mapname");
							std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "sv_hostname");
							std::string grabberplayername = "WEIRDONAME";
							std::string playername = "WEIRDONAME";
							const char* playerInfo;
							if (grabberPlayerNum >= 0 && grabberPlayerNum < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + grabberPlayerNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								grabberplayername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
							}
							if (playerNum >= 0 && playerNum < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
							}

							float capperPadDistance = -1; // if other capper, how far is he from pad?
							int capperTimeToCap = -1; // if other capper, based on his current speed, how long until he caps?


							bool grabberPlayerIsVisible = false;
							entityState_t* grabberPlayerEntity = findEntity(grabberPlayerNum);
							if (grabberPlayerEntity) {
								grabberPlayerIsVisible = true;
							}
							bool grabberPlayerIsFollowed = demo.cut.Cl.snap.ps.clientNum == grabberPlayerNum;
							bool grabberPlayerIsVisibleOrFollowed = grabberPlayerIsFollowed || grabberPlayerIsVisible;


							bool playerIsVisible = false;
							entityState_t* playerEntity = playerNum != -1 ? findEntity(playerNum) : NULL;
							if (playerEntity) {
								playerIsVisible = true;
							}
							bool playerIsFollowed = demo.cut.Cl.snap.ps.clientNum == playerNum;
							bool playerIsVisibleOrFollowed = playerIsFollowed || playerIsVisible;

							int flagHoldTime = playerNum != -1 ? recentFlagHoldTimes[playerNum] : 0;

							if (playerNum != -1) {
								if (redFlagNewCarrierByEventBitMask & (1L << playerNum)) {
									// Player got the flag on this exact frame. Flag hold time is going to be wrong. Set to 0 manually.
									flagHoldTime = 0;
									//sameFrameCap = true;
								}
								else if (blueFlagNewCarrierByEventBitMask & (1L << playerNum)) {
									flagHoldTime = 0;
									//sameFrameCap = true;
								}
							}

							bool wasFollowed = false;
							bool wasVisible = false;
							bool wasVisibleOrFollowed = false;
							if (playerNum != -1) {

								if (playerFirstFollowed[playerNum] != -1 && playerFirstFollowed[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasFollowed = true;
								}
								if (playerFirstVisible[playerNum] != -1 && playerFirstVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisible = true;
								}
								if (playerFirstFollowedOrVisible[playerNum] != -1 && playerFirstFollowedOrVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisibleOrFollowed = true;
								}
							}

							float maxSpeedGrabberLastSecond = getMaxSpeedForClientinTimeFrame(grabberPlayerNum, demoCurrentTime - 1000, demoCurrentTime);
							float maxSpeedCapperLastSecond = playerNum != -1 ? getMaxSpeedForClientinTimeFrame(playerNum, demoCurrentTime - 1000, demoCurrentTime) : -1;

							// See if he had any kills/rets as carrier. (did he fight valiantly before he died?)
							if (playerNum != -1 && recentKillsDuringFlagHold[playerNum].lastKillDemoTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentKillsDuringFlagHold[playerNum], 0, sizeof(CapperKillsInfo));
							}
							int flagCarrierKillCount = playerNum != -1 ? recentKillsDuringFlagHold[playerNum].kills : 0;
							int flagCarrierRetCount = playerNum != -1 ? recentKillsDuringFlagHold[playerNum].rets : 0;
							int flagCarrierSaberKillCount = playerNum != -1 ? recentKillsDuringFlagHold[playerNum].saberkills : 0;


							vec3_t currentPos;
							vec3_t currentDir;
							vec3_t currentPosCapper;
							vec3_t currentDirCapper;

							NearbyPlayerInfo* nearbyInfo = NULL;
							if (grabberPlayerIsVisibleOrFollowed) {
								if (grabberPlayerEntity) {
									VectorCopy(grabberPlayerEntity->pos.trBase, currentPos); // This is also useful in general.
									VectorCopy(grabberPlayerEntity->pos.trDelta, currentDir); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPos);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDir);
								}
								nearbyInfo = getNearbyPlayersInfo<max_clients>(currentPos, grabberPlayerNum);
							}


							// Pickup origin
							int pickupOrigin = -1;// victimIsFlagCarrier ? (attacker == lastKnownFlagCarrier[TEAM_BLUE] ? cgs.flagLastPickupOrigin[TEAM_BLUE] : cgs.flagLastPickupOrigin[TEAM_RED]) : -1;
							switch (flagTeam) {
							case TEAM_FREE: // Is this correct?
								pickupOrigin = cgs.flagLastPickupOrigin[TEAM_FREE];
								break;
							case TEAM_RED:
								pickupOrigin = cgs.flagLastPickupOrigin[TEAM_RED];
								break;
							case TEAM_BLUE:
								pickupOrigin = cgs.flagLastPickupOrigin[TEAM_BLUE];
								break;
							}

							NearbyPlayerInfo* nearbyInfoPad = NULL;
							if (pickupOrigin == 2 && baseFlagPositions[flagTeam].known) {
								nearbyInfoPad = getNearbyPlayersInfo<max_clients>(baseFlagPositions[flagTeam].pos, grabberPlayerNum);
							}

							// Boosts that led to this cap
							std::stringstream boostsStringStream;
							int boostCountGrabber = 0;
							if (true) { // If it's a literal "/kill" console command, let's just ignore boosts. Why bother.

								//int64_t excludeAttackerBoostsAfter = INT64_MAX; // todo exclude anything that happened on this frame? so we ignore boosts that happened after or right during cap? meh
								// oh btw... could use pm_time for boost detection. check if it was set since last frame.

								for (int i = 0; i < boosts.size(); i++) {
									if ((demoCurrentTime - boosts[i].demoTime) > BOOST_DETECT_MAX_AGE) continue; // meh
									qboolean doThis = qfalse;
									// find out if we should even bother
									if (boosts[i].boostedClientNum == grabberPlayerNum) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts

										//if (boosts[i].demoTime > excludeAttackerBoostsAfter) continue;

										// Ignore mini boosts?
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;

										//if (!boosts[i].checkFacingTowards(target)) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

										boostCountGrabber++;
										doThis = qtrue;
										boostsStringStream << "[GRABBER by";
									}
									if (!doThis) continue;

									// ok let's bother
									std::string boosterName = "";
									if (boosts[i].boosterClientNum != -1) {
										int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + boosts[i].boosterClientNum];
										const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
										boosterName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
									}
									if (boosts[i].isEnemyBoost != -1) {
										boostsStringStream << (boosts[i].isEnemyBoost ?  " ENEMY]" : (boosts[i].boosterClientNum == grabberPlayerNum ? " SELF]" : " TEAM]"));
										//boostsStringStream << (boosts[i].isEnemyBoost ? (target == boosts[i].boosterClientNum ? " VICTIM]" : " ENEMY]") : (boosts[i].boosterClientNum == attacker ? " SELF]" : " TEAM]"));
									}
									else {
										boostsStringStream << " UNKNOWN]";
									}
									if (boosterName.size() > 0) {
										boostsStringStream << " " << boosterName;
									}
									boostsStringStream << " (" << (boosts[i].demoTime - demoCurrentTime) << ", ~" << boosts[i].estimatedStrength << ")";
									boostsStringStream << "\n";
								}
							}
							std::string boostsString = boostsStringStream.str();

							// Find nearby players.
							/*std::stringstream nearbyPlayersSS;
							std::vector<int> nearbyPlayers;
							std::vector<int> nearbyPlayersDistances;
							int nearbyPlayersCount = 0;
							if (grabberPlayerIsVisibleOrFollowed) {
								if (grabberPlayerEntity) {
									VectorCopy(grabberPlayerEntity->pos.trBase, currentPos); // This is also useful in general.
									VectorCopy(grabberPlayerEntity->pos.trDelta, currentDir); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPos);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDir);
								}
								for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
									entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
									if (thisEntitySub->number >= 0 && thisEntitySub->number < max_clients && thisEntitySub->number != grabberPlayerNum) {
										float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, currentPos);
										if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
											nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " (" << (int)nearbyPlayerDistance << ")";
											nearbyPlayers.push_back(thisEntitySub->number);
											nearbyPlayersDistances.push_back(nearbyPlayerDistance);
										}
									}
								}
								if (demo.cut.Cl.snap.ps.clientNum != grabberPlayerNum) {
									float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, currentPos);
									if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
										nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
										nearbyPlayers.push_back(demo.cut.Cl.snap.ps.clientNum);
										nearbyPlayersDistances.push_back(nearbyPlayerDistance);
									}
								}
							}
							
							std::string nearbyPlayersString = nearbyPlayersSS.str();

							// Find nearby enemies
							std::stringstream nearbyEnemiesSS;
							int nearbyEnemiescount = 0;
							int veryClosePlayersCount = 0;
							int verycloseEnemiescount = 0;
							for (int nearV = 0; nearV < nearbyPlayers.size(); nearV++) {
								int nearbyPlayerHere = nearbyPlayers[nearV];
								if (playerTeams[nearbyPlayerHere] != playerTeams[grabberPlayerNum]) {
									nearbyEnemiesSS << (nearbyEnemiescount++ == 0 ? "" : ",") << nearbyPlayerHere << " (" << (int)nearbyPlayersDistances[nearV] << ")";
									if (nearbyPlayersDistances[nearV] <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
										verycloseEnemiescount++;
									}
								}
								if (nearbyPlayersDistances[nearV] <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
									veryClosePlayersCount++;
								}
							}
							std::string nearbyEnemiesString = nearbyEnemiesSS.str();
							
							
							*/


							if (playerIsVisibleOrFollowed) {
								if (playerEntity) {
									VectorCopy(playerEntity->pos.trBase, currentPosCapper); // This is also useful in general.
									VectorCopy(playerEntity->pos.trDelta, currentDirCapper); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPosCapper);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDirCapper);
								}

								if (baseFlagPositions[flagTeam].known) {
									capperPadDistance = VectorDistance(baseFlagPositions[flagTeam].pos, currentPosCapper);
									vec3_t vectorToFlag;
									VectorSubtract(baseFlagPositions[flagTeam].pos, currentPosCapper, vectorToFlag);
									VectorNormalize(vectorToFlag);
									float velToFlag = DotProduct(vectorToFlag, currentDirCapper);
									capperTimeToCap = 0;
									if (velToFlag < 250) {
										// we're not even moving enough towards the flag, but let's say we could turn towards the flag within 1 second and then move at minimum 250 ups
										capperTimeToCap = (250.0f- velToFlag)/250.0f*500.0f;
										velToFlag = 250;
									}
									capperTimeToCap += 1000.0f*(capperPadDistance / velToFlag);
								}
							}


							// Stats about speed
							if (playerNum != -1 && recentFlagHoldVariousInfo[playerNum].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentFlagHoldVariousInfo[playerNum], 0, sizeof(VariousCappingInfo));
								recentFlagHoldVariousInfo[playerNum].lastUpdateTime = demoCurrentTime;
							}
							float maxSpeedCapper = playerNum != -1? recentFlagHoldVariousInfo[playerNum].maxSpeedThisRun : 0;
							float averageSpeedCapper = playerNum != -1? (recentFlagHoldVariousInfo[playerNum].divisorSpeeds == 0 ? 0 : recentFlagHoldVariousInfo[playerNum].sumSpeeds / recentFlagHoldVariousInfo[playerNum].divisorSpeeds) : 0;




							// Stats about nearby enemy count throughout run
							float maxNearbyEnemyCount = 0, moreThanOneNearbyEnemyTimePercent = 0, averageNearbyEnemyCount = -1, maxVeryCloseEnemyCount = 0, anyVeryCloseEnemyTimePercent = 0, moreThanOneVeryCloseEnemyTimePercent = 0, averageVeryCloseEnemyCount = -1;
							// Resets if necessary.
							if (playerNum != -1 && recentFlagHoldEnemyNearbyTimes[playerNum].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentFlagHoldEnemyNearbyTimes[playerNum], 0, sizeof(EnemyNearbyInfo));
								recentFlagHoldEnemyNearbyTimes[playerNum].lastUpdateTime = demoCurrentTime;
							}
							averageHelper_t nearbyHelper, veryCloseHelper;
							Com_Memset(&nearbyHelper, 0, sizeof(nearbyHelper));
							Com_Memset(&veryCloseHelper, 0, sizeof(veryCloseHelper));
							if(playerNum != -1){
								for (int nearbyCount = 0; nearbyCount < max_clients; nearbyCount++) { // You'd think here it should be <= MAX_CLIENTS because 32 is a valid number, BUT with max of 32 players you can only have a max of 31 enemies. 32 is actually a valid index though, for "unknown", but we aren't currently tracking that. We just track for the time that the flag carrier WAS visible.
									int nearbyTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[playerNum].enemyNearbyTimes[nearbyCount];
									int veryCloseTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[playerNum].enemyVeryCloseTimes[nearbyCount];

									// If this count happened even for the shortest amount of time, that was the max amount of near/very close enemies.
									if (nearbyTimeOfThisCount && nearbyCount > maxNearbyEnemyCount) maxNearbyEnemyCount = nearbyCount;
									if (veryCloseTimeOfThisCount && nearbyCount > maxVeryCloseEnemyCount) maxVeryCloseEnemyCount = nearbyCount;

									nearbyHelper.sum += nearbyCount * nearbyTimeOfThisCount;
									nearbyHelper.divisor += nearbyTimeOfThisCount;
									veryCloseHelper.sum += nearbyCount * veryCloseTimeOfThisCount;
									veryCloseHelper.divisor += veryCloseTimeOfThisCount;

									if (nearbyCount > 0) {
										anyVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
										if (nearbyCount > 1) {
											moreThanOneNearbyEnemyTimePercent += nearbyTimeOfThisCount;
											moreThanOneVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
										}
									}
								}
							}
							averageNearbyEnemyCount = nearbyHelper.divisor == 0 ? 0 : nearbyHelper.sum / nearbyHelper.divisor;
							averageVeryCloseEnemyCount = veryCloseHelper.divisor == 0 ? 0 : veryCloseHelper.sum / veryCloseHelper.divisor;
							moreThanOneNearbyEnemyTimePercent *= nearbyHelper.divisor == 0 ? 0 : 100.0f / nearbyHelper.divisor;
							moreThanOneVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
							anyVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
							// END Stats about nearby enemy count throughout run



							// TODO handle this:
							// someone swoops flag, dies on same frame, you pick up the flag on pad.
							// it would rn show u as having swooped it, but in reality you got it on the pad
							// very niche scenario but maybe worth handling someday


							//if (opts.onlyLogCapturesWithSaberKills && flagCarrierSaberKillCount == 0) continue;


							SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
							SQLDelayedQuery* query = &queryWrapper->query;

							SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
							SQLBIND_DELAYED_TEXT(query, "@serverName", serverName.c_str());
							std::string serverNameStripped = Q_StripColorAll(serverName);
							SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
							//SQLBIND(io.insertCaptureStatement, int, "@flagPickupSource", teamInfo[flagTeam].flagHoldOrigin);
							SQLBIND_DELAYED(query, int, "@flagPickupSource", pickupOrigin);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@grabberName", grabberplayername.c_str(), QF_PLAYERNAME0);
							std::string grabbernameStripped = Q_StripColorAll(grabberplayername);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@grabberNameStripped", grabbernameStripped.c_str(), QF_PLAYERNAME0STRIPPED);
							SQLBIND_DELAYED(query, int, "@grabberClientNum", grabberPlayerNum);
							if (playerNum != -1) {
								SQLBIND_DELAYED(query, int, "@enemyFlagHoldTime", flagHoldTime);
								SQLBIND_DELAYED(query, int, "@capperClientNum", playerNum);
								SQLBIND_DELAYED_TEXT_FLAGS(query, "@capperName", playername.c_str(), QF_PLAYERNAME1);
								std::string playernameStripped = Q_StripColorAll(playername);
								SQLBIND_DELAYED_TEXT_FLAGS(query, "@capperNameStripped", playernameStripped.c_str(), QF_PLAYERNAME1STRIPPED);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@enemyFlagHoldTime");
								SQLBIND_DELAYED_NULL(query, "@capperClientNum");
								SQLBIND_DELAYED_NULL_FLAGS(query, "@capperName",QF_PLAYERNAME1);
								SQLBIND_DELAYED_NULL_FLAGS(query, "@capperNameStripped", QF_PLAYERNAME1STRIPPED);
							}
							SQLBIND_DELAYED(query, int, "@grabberIsVisible", grabberPlayerIsVisible);
							SQLBIND_DELAYED(query, int, "@grabberIsFollowed", grabberPlayerIsFollowed);
							SQLBIND_DELAYED(query, int, "@grabberIsFollowedOrVisible", grabberPlayerIsVisibleOrFollowed);
							if (playerNum != -1) {
								SQLBIND_DELAYED(query, int, "@capperIsVisible", playerIsVisible);
								SQLBIND_DELAYED(query, int, "@capperIsFollowed", playerIsFollowed);
								SQLBIND_DELAYED(query, int, "@capperIsFollowedOrVisible", playerIsVisibleOrFollowed);
								SQLBIND_DELAYED(query, int, "@capperWasVisible", wasVisible);
								SQLBIND_DELAYED(query, int, "@capperWasFollowed", wasFollowed);
								SQLBIND_DELAYED(query, int, "@capperWasFollowedOrVisible", wasVisibleOrFollowed);
								SQLBIND_DELAYED(query, int, "@capperKills", flagCarrierKillCount);
								SQLBIND_DELAYED(query, int, "@capperRets", flagCarrierRetCount);
								SQLBIND_DELAYED(query, double, "@maxSpeedCapper", maxSpeedCapper);
								SQLBIND_DELAYED(query, double, "@averageSpeedCapper", averageSpeedCapper);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@capperIsVisible");
								SQLBIND_DELAYED_NULL(query, "@capperIsFollowed");
								SQLBIND_DELAYED_NULL(query, "@capperIsFollowedOrVisible");
								SQLBIND_DELAYED_NULL(query, "@capperWasVisible");
								SQLBIND_DELAYED_NULL(query, "@capperWasFollowed");
								SQLBIND_DELAYED_NULL(query, "@capperWasFollowedOrVisible");
								SQLBIND_DELAYED_NULL(query, "@capperKills");
								SQLBIND_DELAYED_NULL(query, "@capperRets");
								SQLBIND_DELAYED_NULL(query, "@maxSpeedCapper");
								SQLBIND_DELAYED_NULL(query, "@averageSpeedCapper");
							}
							SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
							SQLBIND_DELAYED(query, int, "@flagTeam", flagTeam);
							//SQLBIND_DELAYED(query, int, "@sameFrameCap", sameFrameCap);
							SQLBIND_DELAYED(query, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND_DELAYED(query, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							SQLBIND_DELAYED(query, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND_DELAYED(query, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND_DELAYED(query, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount + teamInfo[TEAM_BLUE].playerCount + teamInfo[TEAM_RED].playerCount);
							SQLBIND_DELAYED_TEXT(query, "@boosts", (boostCountGrabber > 0 ? boostsString.c_str() : NULL));
							SQLBIND_DELAYED(query, int, "@boostCount", boostCountGrabber);
							if (grabberPlayerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@maxSpeedGrabberLastSecond", maxSpeedGrabberLastSecond);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@maxSpeedGrabberLastSecond");
							}
							if (playerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@maxSpeedCapperLastSecond", maxSpeedCapperLastSecond);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@maxSpeedCapperLastSecond");
							}

							if (nearbyInfo) {
								SQLBIND_DELAYED_TEXT(query, "@nearbyPlayers", nearbyInfo->nearbyPlayersString.c_str());
								SQLBIND_DELAYED(query, int, "@nearbyPlayerCount", nearbyInfo->nearbyPlayersCount);
								SQLBIND_DELAYED_TEXT(query, "@nearbyEnemies", nearbyInfo->nearbyEnemiesString.c_str());
								SQLBIND_DELAYED(query, int, "@nearbyEnemyCount", nearbyInfo->nearbyEnemiescount);
								SQLBIND_DELAYED(query, int, "@veryCloseEnemyCount", nearbyInfo->verycloseEnemiescount);
								SQLBIND_DELAYED(query, int, "@veryClosePlayersCount", nearbyInfo->veryClosePlayersCount);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@nearbyPlayers");
								SQLBIND_DELAYED(query, int, "@nearbyPlayerCount", 0);
								SQLBIND_DELAYED_NULL(query, "@nearbyEnemies");
								SQLBIND_DELAYED(query, int, "@nearbyEnemyCount", 0);
								SQLBIND_DELAYED(query, int, "@veryCloseEnemyCount", 0);
								SQLBIND_DELAYED(query, int, "@veryClosePlayersCount", 0);
							}
							if (nearbyInfoPad) {
								SQLBIND_DELAYED_TEXT(query, "@padNearbyPlayers", nearbyInfoPad->nearbyPlayersString.c_str());
								SQLBIND_DELAYED(query, int, "@padNearbyPlayerCount", nearbyInfoPad->nearbyPlayersCount);
								SQLBIND_DELAYED_TEXT(query, "@padNearbyEnemies", nearbyInfoPad->nearbyEnemiesString.c_str());
								SQLBIND_DELAYED(query, int, "@padNearbyEnemyCount", nearbyInfoPad->nearbyEnemiescount);
								SQLBIND_DELAYED(query, int, "@padVeryCloseEnemyCount", nearbyInfoPad->verycloseEnemiescount);
								SQLBIND_DELAYED(query, int, "@padVeryClosePlayersCount", nearbyInfoPad->veryClosePlayersCount);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@padNearbyPlayers");
								SQLBIND_DELAYED(query, int, "@padNearbyPlayerCount", 0);
								SQLBIND_DELAYED_NULL(query, "@padNearbyEnemies");
								SQLBIND_DELAYED(query, int, "@padNearbyEnemyCount", 0);
								SQLBIND_DELAYED(query, int, "@padVeryCloseEnemyCount", 0);
								SQLBIND_DELAYED(query, int, "@padVeryClosePlayersCount", 0);
							}

							if (playerNum != -1) {
								SQLBIND_DELAYED(query, double, "@maxNearbyEnemyCount", maxNearbyEnemyCount);
								SQLBIND_DELAYED(query, double, "@moreThanOneNearbyEnemyTimePercent", moreThanOneNearbyEnemyTimePercent);
								SQLBIND_DELAYED(query, double, "@averageNearbyEnemyCount", averageNearbyEnemyCount);
								SQLBIND_DELAYED(query, double, "@maxVeryCloseEnemyCount", maxVeryCloseEnemyCount);
								SQLBIND_DELAYED(query, double, "@anyVeryCloseEnemyTimePercent", anyVeryCloseEnemyTimePercent);
								SQLBIND_DELAYED(query, double, "@moreThanOneVeryCloseEnemyTimePercent", moreThanOneVeryCloseEnemyTimePercent);
								SQLBIND_DELAYED(query, double, "@averageVeryCloseEnemyCount", averageVeryCloseEnemyCount);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@maxNearbyEnemyCount");
								SQLBIND_DELAYED_NULL(query, "@moreThanOneNearbyEnemyTimePercent");
								SQLBIND_DELAYED_NULL(query, "@averageNearbyEnemyCount");
								SQLBIND_DELAYED_NULL(query, "@maxVeryCloseEnemyCount");
								SQLBIND_DELAYED_NULL(query, "@anyVeryCloseEnemyTimePercent");
								SQLBIND_DELAYED_NULL(query, "@moreThanOneVeryCloseEnemyTimePercent");
								SQLBIND_DELAYED_NULL(query, "@averageVeryCloseEnemyCount");
							}

							if (playerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@capperPadDistance", capperPadDistance);
								SQLBIND_DELAYED(query, int, "@capperTimeToCap", capperTimeToCap);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@capperPadDistance");
								SQLBIND_DELAYED_NULL(query, "@capperTimeToCap");
							}

							if (grabberPlayerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@positionX", currentPos[0]);
								SQLBIND_DELAYED(query, double, "@positionY", currentPos[1]);
								SQLBIND_DELAYED(query, double, "@positionZ", currentPos[2]);
								SQLBIND_DELAYED(query, double, "@directionX", currentDir[0]);
								SQLBIND_DELAYED(query, double, "@directionY", currentDir[1]);
								SQLBIND_DELAYED(query, double, "@directionZ", currentDir[2]);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@positionX");
								SQLBIND_DELAYED_NULL(query, "@positionY");
								SQLBIND_DELAYED_NULL(query, "@positionZ");
								SQLBIND_DELAYED_NULL(query, "@directionX");
								SQLBIND_DELAYED_NULL(query, "@directionY");
								SQLBIND_DELAYED_NULL(query, "@directionZ");
							}
							if (playerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@capperPositionX", currentPosCapper[0]);
								SQLBIND_DELAYED(query, double, "@capperPositionY", currentPosCapper[1]);
								SQLBIND_DELAYED(query, double, "@capperPositionZ", currentPosCapper[2]);
								SQLBIND_DELAYED(query, double, "@capperDirectionX", currentDirCapper[0]);
								SQLBIND_DELAYED(query, double, "@capperDirectionY", currentDirCapper[1]);
								SQLBIND_DELAYED(query, double, "@capperDirectionZ", currentDirCapper[2]);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@capperPositionX");
								SQLBIND_DELAYED_NULL(query, "@capperPositionY");
								SQLBIND_DELAYED_NULL(query, "@capperPositionZ");
								SQLBIND_DELAYED_NULL(query, "@capperDirectionX");
								SQLBIND_DELAYED_NULL(query, "@capperDirectionY");
								SQLBIND_DELAYED_NULL(query, "@capperDirectionZ");
							}
							SQLBIND_DELAYED_TEXT(query, "@demoName", sharedVars.oldBasename.c_str());
							SQLBIND_DELAYED_TEXT(query, "@demoPath", sharedVars.oldPath.c_str());
							SQLBIND_DELAYED(query, int, "@demoTime", demoCurrentTime);
							SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime);
							SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND_DELAYED(query, int, "@demoDateTime", sharedVars.oldDemoDateModified);
							SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);

							if (nearbyInfo) {
								delete nearbyInfo;
							}
							if (nearbyInfoPad) {
								delete nearbyInfoPad;
							}

							if (activeKillDatabase != -1) {
								queryWrapper->databaseIndex = activeKillDatabase;
								io.flagGrabQueries->push_back(queryWrapper);
							}
							else {
								delete queryWrapper;
								continue;
							}


							/*
							int enemyTeam = flagTeam;
							int playerTeam = flagTeam == TEAM_RED ? TEAM_BLUE : TEAM_RED;
							for (int client = 0; client < max_clients; client++) {
								if (playerTeams[client] == playerTeam && client != playerNum) {
									addMetaEvent(METAEVENT_TEAMCAPTURE, demoCurrentTime, client, bufferTime, opts, bufferTime);
								}
								else if (playerTeams[client] == enemyTeam) {
									addMetaEvent(METAEVENT_ENEMYTEAMCAPTURE, demoCurrentTime, client, bufferTime, opts, bufferTime);
								}
							}*/
							// Meta event tracker for flaggrab
							MetaEventTracker* grabME = new MetaEventTracker(demoCurrentTime, queryWrapper, metaEventTrackers[METRACKER_FLAGGRABS][grabberPlayerNum], bufferTime, 0, resultingMetaEventTracking[METRACKER_FLAGGRABS]);
							bool wasFollowedThroughBufferTime = playerFirstFollowed[grabberPlayerNum] != -1 && playerFirstFollowed[grabberPlayerNum] < (demo.cut.Cl.snap.serverTime - bufferTime);
							grabME->needsReframe = opts.reframeIfNeeded && !wasFollowedThroughBufferTime;
							grabME->reframeClientNum = grabberPlayerNum;
							grabME->addPastEvents(playerPastMetaEvents[grabberPlayerNum], getMinimumMetaEventBufferTime(grabberPlayerNum, bufferTime, demoCurrentTime));
							addMetaEvent(METAEVENT_FLAGGRAB, demoCurrentTime, playerNum, bufferTime, opts, bufferTime);
							metaEventTrackers[METRACKER_FLAGGRABS][grabberPlayerNum] = grabME;


							if (!grabberPlayerIsVisibleOrFollowed /* && !wasVisibleOrFollowed*/) continue; // No need to cut out those who were not visible at all in any way.
							if (searchMode == SEARCH_MY_CTF_RETURNS && grabberPlayerNum != demo.cut.Cl.snap.ps.clientNum) continue; // Only cut your own for SEARCH_MY_CTF_RETURNS

							//int64_t runStart = demoCurrentTime - flagHoldTime;
							int64_t startTime = demoCurrentTime - bufferTime;
							int64_t endTime = demoCurrentTime + bufferTime;
							int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
							bool isTruncated = false;
							int truncationOffset = 0;
							if (earliestPossibleStart > startTime) {
								truncationOffset = earliestPossibleStart - startTime;
								startTime = earliestPossibleStart;
								isTruncated = true;
							}
							//startTime = std::max(lastGameStateChangeInDemoTime+1, startTime); // We can't start before 0 or before the last gamestate change. +1 to be safe, not sure if necessary.

							//int milliSeconds = flagHoldTime;
							//int pureMilliseconds = milliSeconds % 1000;
							//int seconds = milliSeconds / 1000;
							//int pureSeconds = seconds % 60;
							//int minutes = seconds / 60;

							// TODO track kills during flaggrab?

							std::stringstream ss;
							std::stringstream ss2;
							// what we need: danger of grabbing, closeness of enemy capper to scoring (always compare to base flag pos cuz swoops)
							// << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << "___"
							// << "_" << (int)maxSpeedCapperLastSecond
							std::string boostString = boostCountGrabber > 0 ? va("_BST%d", boostCountGrabber) : "";
							ss << mapname << std::setfill('0') << "___FLAGGRAB" << (flagCarrierKillCount > 0 ? va("%dK", flagCarrierKillCount) : "") << (flagCarrierRetCount > 0 ? va("%dR", flagCarrierRetCount) : "") << boostString;
							ss2 << std::setfill('0') << "___" << /*"" << "___"<<*/  grabberplayername << "___P" << pickupOrigin << "T" << flagTeam << "___" << (nearbyInfo ? nearbyInfo->verycloseEnemiescount : 0)  << "DANGER" << (nearbyInfo ? nearbyInfo->nearbyEnemiescount : 0) << "___" << (int)maxSpeedGrabberLastSecond  << "ups" << (grabberPlayerIsFollowed ? "" : (grabberPlayerIsVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" << grabberPlayerNum << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

							std::string targetFilename = ss.str();
							std::string targetFilename2 = ss2.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							char* targetFilenameFiltered2 = new char[targetFilename2.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);
							sanitizeFilename(targetFilename2.c_str(), targetFilenameFiltered2);

							std::stringstream batSS;
							std::stringstream batSS2;
							batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
							batSS << "\n" << (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered;
							batSS2 << targetFilenameFiltered2 << "\" " << startTime << " " << endTime;
							batSS2 << (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");
							queryWrapper->batchString1 = batSS.str();
							queryWrapper->batchString2 = batSS2.str();

							delete[] targetFilenameFiltered;
							if (!opts.noFindOutput)  std::cout << targetFilename << targetFilename2 << "\n";


							/*int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;
							// A bit pointless tbh because we reset it to -1 anyway before checking entities. 
							// Let me rethink this some day TODO
							if (flagTeam == TEAM_RED) {
								redFlagNewCarrierByEvent = playerNum;
								//lastKnownFlagCarrier[TEAM_RED] = playerNum; // Don't set it here so it doesn't interfere with potential kill events timing-wise, in case the flag return and pickup of new flag happens on same frame (unlikely but possible, especially with snaps?)
							}else if (flagTeam == TEAM_BLUE) {
								blueFlagNewCarrierByEvent = playerNum;
								//lastKnownFlagCarrier[TEAM_BLUE] = playerNum;
							}*/
						}
						else if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_FRAGGED_FLAG_CARRIER) {
							if (opts.makeVideo) {
								int playerNum = thisEs->trickedentindex;
								int flagTeam = thisEs->trickedentindex2;
								std::string playername = "WEIRDONAME";
								const char* playerInfo;
								if (playerNum >= 0 && playerNum < max_clients) {
									int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
									playerInfo = demo.cut.Cl.gameState.stringData + offset;
									playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
								}

								if (opts.makeVideo) {
									videoConsole.push_back({ demoCurrentTime,va("^7%s ^7killed %s's flag carrier",playername.c_str(),flagTeam == TEAM_RED ? "RED" : (flagTeam == TEAM_BLUE ? "BLUE" : "YELLOW")) });
								}
							}
						}
						else if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_PLAYER_RETURNED_FLAG) {
							if (opts.makeVideo) {
								int playerNum = thisEs->trickedentindex;
								int flagTeam = thisEs->trickedentindex2;
								std::string playername = "WEIRDONAME";
								const char* playerInfo;
								if (playerNum >= 0 && playerNum < max_clients) {
									int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
									playerInfo = demo.cut.Cl.gameState.stringData + offset;
									playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
								}

								if (opts.makeVideo) {
									videoConsole.push_back({ demoCurrentTime,va("^7%s ^7returned the %s flag",playername.c_str(),flagTeam == TEAM_RED ? "RED" : (flagTeam == TEAM_BLUE ? "BLUE" : "YELLOW")) });
								}
							}
						}
						else if (eventNumber == EV_CTFMESSAGE_GENERAL && thisEs->eventParm == CTFMESSAGE_PLAYER_CAPTURED_FLAG) {
							//Capture.
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;

							bool sameFrameCap = false;

							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "mapname");
							std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "sv_hostname");
							std::string playername = "WEIRDONAME";
							const char* playerInfo;
							if (playerNum >= 0 && playerNum < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset,isMOHAADemo ?"name": "n");
							}

							bool playerIsVisible = false;
							entityState_t* playerEntity = findEntity(playerNum);
							if (playerEntity) {
								playerIsVisible = true;
							}
							bool playerIsFollowed = demo.cut.Cl.snap.ps.clientNum == playerNum;
							bool playerIsVisibleOrFollowed = playerIsFollowed || playerIsVisible;

							int flagHoldTime = recentFlagHoldTimes[playerNum];

							if (lastKnownFlagCarrier[flagTeam] == -1) { 
								// we didn't actually know who this player was, so use the generic team-based last flag hold over 0.
								// TODO maybe we should have a per-frame based one instead so we can apply it to 
								// flag grabs as well even tho its not that important there
								flagHoldTime = cgs.flagLastFlagHoldTimeOver0[flagTeam];
							}
							else if (lastKnownFlagCarrier[flagTeam] != playerNum) {
								demoErrorFlags |= DERR_GAMELOGICFLAW;
								demoErrors << "DEMO LOGIC FLAW: Capture: Last known flag carrier is known but NOT the person who got the cap?!?\n";
								std::cerr << "Capture: Last known flag carrier is known but NOT the person who got the cap?!?" << "(" << DPrintFLocation << ")\n";
								// i'm not sure what to do here because it makes no sense.
							}

							if (redFlagNewCarrierByEventBitMask & (1L << playerNum)) {
								// Player got the flag on this exact frame. Flag hold time is going to be wrong. Set to 0 manually.
								flagHoldTime = 0;
								sameFrameCap = true;
							}
							else if (blueFlagNewCarrierByEventBitMask & (1L << playerNum)) {
								flagHoldTime = 0;
								sameFrameCap = true;
							}

							bool wasFollowed = false;
							bool wasVisible = false;
							bool wasVisibleOrFollowed = false;
							if (playerNum != -1) {

								if (playerFirstFollowed[playerNum] != -1 && playerFirstFollowed[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasFollowed = true;
								}
								if (playerFirstVisible[playerNum] != -1 && playerFirstVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisible = true;
								}
								if (playerFirstFollowedOrVisible[playerNum] != -1 && playerFirstFollowedOrVisible[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime)) {
									wasVisibleOrFollowed = true;
								}
							}

							float maxSpeedCapperLastSecond = getMaxSpeedForClientinTimeFrame(playerNum, demoCurrentTime - 1000, demoCurrentTime);

							// See if he had any kills/rets as carrier. (did he fight valiantly before he died?)
							if (recentKillsDuringFlagHold[playerNum].lastKillDemoTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last capping related kill of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentKillsDuringFlagHold[playerNum], 0, sizeof(CapperKillsInfo));
							}
							int flagCarrierKillCount = recentKillsDuringFlagHold[playerNum].kills;
							int flagCarrierRetCount = recentKillsDuringFlagHold[playerNum].rets;
							int flagCarrierSaberKillCount = recentKillsDuringFlagHold[playerNum].saberkills;


							vec3_t currentPos;
							vec3_t currentDir;

							NearbyPlayerInfo* nearbyInfo = NULL;
							if (playerIsVisibleOrFollowed) {
								if (playerEntity) {
									VectorCopy(playerEntity->pos.trBase, currentPos); // This is also useful in general.
									VectorCopy(playerEntity->pos.trDelta, currentDir); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPos);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDir);
								}
								nearbyInfo = getNearbyPlayersInfo<max_clients>(currentPos, playerNum);
							}


							// Boosts that led to this cap
							std::stringstream boostsStringStream;
							int boostCountCapper = 0;
							if ( true) { // If it's a literal "/kill" console command, let's just ignore boosts. Why bother.

								//int64_t excludeAttackerBoostsAfter = INT64_MAX; // todo exclude anything that happened on this frame? so we ignore boosts that happened after or right during cap? meh
								// oh btw... could use pm_time for boost detection. check if it was set since last frame.

								for (int i = 0; i < boosts.size(); i++) {
									if ((demoCurrentTime - boosts[i].demoTime) > BOOST_DETECT_MAX_AGE) continue; // meh
									qboolean doThis = qfalse;
									// find out if we should even bother
									if (boosts[i].boostedClientNum == playerNum) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts

										//if (boosts[i].demoTime > excludeAttackerBoostsAfter) continue;

										// Ignore mini boosts?
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;

										//if (!boosts[i].checkFacingTowards(target)) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

										boostCountCapper++;
										doThis = qtrue;
										boostsStringStream << "[CAPPER by";
									}
									if (!doThis) continue;

									// ok let's bother
									std::string boosterName = "";
									if (boosts[i].boosterClientNum != -1) {
										int offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + boosts[i].boosterClientNum];
										const char* playerInfo = demo.cut.Cl.gameState.stringData + offset;
										boosterName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, isMOHAADemo ? "name" : "n");
									}
									if (boosts[i].isEnemyBoost != -1) {
										boostsStringStream << (boosts[i].isEnemyBoost ? " ENEMY]" : (boosts[i].boosterClientNum == playerNum ? " SELF]" : " TEAM]"));
										//boostsStringStream << (boosts[i].isEnemyBoost ? (target == boosts[i].boosterClientNum ? " VICTIM]" : " ENEMY]") : (boosts[i].boosterClientNum == attacker ? " SELF]" : " TEAM]"));
									}
									else {
										boostsStringStream << " UNKNOWN]";
									}
									if (boosterName.size() > 0) {
										boostsStringStream << " " << boosterName;
									}
									boostsStringStream << " (" << (boosts[i].demoTime - demoCurrentTime) << ", ~" << boosts[i].estimatedStrength << ")";
									boostsStringStream << "\n";
								}
							}
							std::string boostsString = boostsStringStream.str();

							// Find nearby players.
							/*std::stringstream nearbyPlayersSS;
							std::vector<int> nearbyPlayers;
							std::vector<int> nearbyPlayersDistances;
							int nearbyPlayersCount = 0;
							if (playerIsVisibleOrFollowed) {
								if (playerEntity) {
									VectorCopy(playerEntity->pos.trBase,currentPos); // This is also useful in general.
									VectorCopy(playerEntity->pos.trDelta, currentDir); // This is also useful in general.
								}
								else {
									VectorCopy(demo.cut.Cl.snap.ps.origin, currentPos);
									VectorCopy(demo.cut.Cl.snap.ps.velocity, currentDir);
								}
								for (int subPe = demo.cut.Cl.snap.parseEntitiesNum; subPe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; subPe++) {
									entityState_t* thisEntitySub = &demo.cut.Cl.parseEntities[subPe & (MAX_PARSE_ENTITIES - 1)];
									if (thisEntitySub->number >= 0 && thisEntitySub->number < max_clients && thisEntitySub->number != playerNum) {
										float nearbyPlayerDistance = VectorDistance(thisEntitySub->pos.trBase, currentPos);
										if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
											nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << thisEntitySub->number << " (" << (int)nearbyPlayerDistance << ")";
											nearbyPlayers.push_back(thisEntitySub->number);
											nearbyPlayersDistances.push_back(nearbyPlayerDistance);
										}
									}
								}
								if (demo.cut.Cl.snap.ps.clientNum != playerNum) {
									float nearbyPlayerDistance = VectorDistance(demo.cut.Cl.snap.ps.origin, currentPos);
									if (nearbyPlayerDistance <= NEARBY_PLAYER_MAX_DISTANCE) {
										nearbyPlayersSS << (nearbyPlayersCount++ == 0 ? "" : ",") << demo.cut.Cl.snap.ps.clientNum << " (" << (int)nearbyPlayerDistance << ")";
										nearbyPlayers.push_back(demo.cut.Cl.snap.ps.clientNum);
										nearbyPlayersDistances.push_back(nearbyPlayerDistance);
									}
								}
							}
							std::string nearbyPlayersString = nearbyPlayersSS.str();
							
							// Find nearby enemies
							std::stringstream nearbyEnemiesSS;
							int nearbyEnemiescount = 0;
							for (int nearV = 0; nearV < nearbyPlayers.size(); nearV++) {
								int nearbyPlayerHere = nearbyPlayers[nearV];
								if (playerTeams[nearbyPlayerHere] != playerTeams[playerNum]) {
									nearbyEnemiesSS << (nearbyEnemiescount++ == 0 ? "" : ",") << nearbyPlayerHere << " (" << (int)nearbyPlayersDistances[nearV] << ")";
								}
							}
							std::string nearbyEnemiesString = nearbyEnemiesSS.str();
							*/

							// Stats about speed
							if (recentFlagHoldVariousInfo[playerNum].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentFlagHoldVariousInfo[playerNum], 0, sizeof(VariousCappingInfo));
								recentFlagHoldVariousInfo[playerNum].lastUpdateTime = demoCurrentTime;
							}
							float maxSpeedCapper = recentFlagHoldVariousInfo[playerNum].maxSpeedThisRun;
							float averageSpeedCapper = recentFlagHoldVariousInfo[playerNum].divisorSpeeds == 0 ? 0 : recentFlagHoldVariousInfo[playerNum].sumSpeeds/recentFlagHoldVariousInfo[playerNum].divisorSpeeds;


							// Stats about nearby enemy count throughout run
							float maxNearbyEnemyCount=0, moreThanOneNearbyEnemyTimePercent=0, averageNearbyEnemyCount=-1, maxVeryCloseEnemyCount=0, anyVeryCloseEnemyTimePercent=0, moreThanOneVeryCloseEnemyTimePercent=0, averageVeryCloseEnemyCount=-1;
							// Resets if necessary.
							if (recentFlagHoldEnemyNearbyTimes[playerNum].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentFlagHoldEnemyNearbyTimes[playerNum], 0, sizeof(EnemyNearbyInfo));
								recentFlagHoldEnemyNearbyTimes[playerNum].lastUpdateTime = demoCurrentTime;
							}
							averageHelper_t nearbyHelper,veryCloseHelper;
							Com_Memset(&nearbyHelper, 0, sizeof(nearbyHelper));
							Com_Memset(&veryCloseHelper, 0, sizeof(veryCloseHelper));
							for (int nearbyCount = 0; nearbyCount < max_clients; nearbyCount++) { // You'd think here it should be <= MAX_CLIENTS because 32 is a valid number, BUT with max of 32 players you can only have a max of 31 enemies. 32 is actually a valid index though, for "unknown", but we aren't currently tracking that. We just track for the time that the flag carrier WAS visible.
								int nearbyTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[playerNum].enemyNearbyTimes[nearbyCount];
								int veryCloseTimeOfThisCount = recentFlagHoldEnemyNearbyTimes[playerNum].enemyVeryCloseTimes[nearbyCount];

								// If this count happened even for the shortest amount of time, that was the max amount of near/very close enemies.
								if (nearbyTimeOfThisCount && nearbyCount > maxNearbyEnemyCount) maxNearbyEnemyCount = nearbyCount;
								if (veryCloseTimeOfThisCount && nearbyCount > maxVeryCloseEnemyCount) maxVeryCloseEnemyCount = nearbyCount;

								nearbyHelper.sum += nearbyCount * nearbyTimeOfThisCount;
								nearbyHelper.divisor += nearbyTimeOfThisCount;
								veryCloseHelper.sum += nearbyCount * veryCloseTimeOfThisCount;
								veryCloseHelper.divisor += veryCloseTimeOfThisCount;

								if (nearbyCount > 0) {
									anyVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
									if (nearbyCount > 1) {
										moreThanOneNearbyEnemyTimePercent += nearbyTimeOfThisCount;
										moreThanOneVeryCloseEnemyTimePercent += veryCloseTimeOfThisCount;
									}
								}
							}
							averageNearbyEnemyCount = nearbyHelper.divisor == 0 ? 0 : nearbyHelper.sum / nearbyHelper.divisor;
							averageVeryCloseEnemyCount = veryCloseHelper.divisor == 0 ? 0 : veryCloseHelper.sum / veryCloseHelper.divisor;
							moreThanOneNearbyEnemyTimePercent *= nearbyHelper.divisor == 0 ? 0 : 100.0f / nearbyHelper.divisor;
							moreThanOneVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
							anyVeryCloseEnemyTimePercent *= veryCloseHelper.divisor == 0 ? 0 : 100.0f / veryCloseHelper.divisor;
							// END Stats about nearby enemy count throughout run


							// Pickup origin
							int capperLastPickupOrigin = -1;// victimIsFlagCarrier ? (attacker == lastKnownFlagCarrier[TEAM_BLUE] ? cgs.flagLastPickupOrigin[TEAM_BLUE] : cgs.flagLastPickupOrigin[TEAM_RED]) : -1;
							switch (flagTeam) {
								case TEAM_FREE: // Is this correct?
									capperLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_FREE];
									break;
								case TEAM_RED:
									capperLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_RED];
									break;
								case TEAM_BLUE:
									capperLastPickupOrigin = cgs.flagLastPickupOrigin[TEAM_BLUE];
									break;
							}

							if (sameFrameCap) {
								capperLastPickupOrigin = 2; // pretty much guaranteed and I think it's gonna be very hard to tell it apart from a hypothetical where someone picks it up from its base and captures it on the same frame :/
							}

							if (opts.onlyLogCapturesWithSaberKills && flagCarrierSaberKillCount == 0) continue;

							if (opts.makeVideo) {
								videoConsole.push_back({ demoCurrentTime,va("^7%s ^7captured the %s flag",playername.c_str(),flagTeam == TEAM_RED ? "RED" : (flagTeam == TEAM_BLUE ? "BLUE" : "YELLOW"))});
							}

							SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
							SQLDelayedQuery* query = &queryWrapper->query;

							SQLBIND_DELAYED_TEXT(query, "@map", mapname.c_str());
							SQLBIND_DELAYED_TEXT(query, "@serverName", serverName.c_str());
							std::string serverNameStripped = Q_StripColorAll(serverName);
							SQLBIND_DELAYED_TEXT(query, "@serverNameStripped", serverNameStripped.c_str());
							SQLBIND_DELAYED(query, int, "@flagHoldTime", flagHoldTime);
							//SQLBIND(io.insertCaptureStatement, int, "@flagPickupSource", teamInfo[flagTeam].flagHoldOrigin);
							SQLBIND_DELAYED(query, int, "@flagPickupSource", capperLastPickupOrigin);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@capperName", playername.c_str(), QF_PLAYERNAME0);
							std::string playernameStripped = Q_StripColorAll(playername);
							SQLBIND_DELAYED_TEXT_FLAGS(query, "@capperNameStripped", playernameStripped.c_str(), QF_PLAYERNAME0STRIPPED);
							SQLBIND_DELAYED(query, int, "@capperClientNum", playerNum);
							SQLBIND_DELAYED(query, int, "@capperIsVisible", playerIsVisible);
							SQLBIND_DELAYED(query, int, "@capperIsFollowed", playerIsFollowed);
							SQLBIND_DELAYED(query, int, "@capperIsFollowedOrVisible", playerIsVisibleOrFollowed);
							SQLBIND_DELAYED(query, int, "@capperWasVisible", wasVisible);
							SQLBIND_DELAYED(query, int, "@capperWasFollowed", wasFollowed);
							SQLBIND_DELAYED(query, int, "@capperWasFollowedOrVisible", wasVisibleOrFollowed);
							SQLBIND_DELAYED(query, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
							SQLBIND_DELAYED(query, int, "@flagTeam", flagTeam);
							SQLBIND_DELAYED(query, int, "@capperKills", flagCarrierKillCount);
							SQLBIND_DELAYED(query, int, "@capperRets", flagCarrierRetCount);
							SQLBIND_DELAYED(query, int, "@sameFrameCap", sameFrameCap);
							SQLBIND_DELAYED(query, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND_DELAYED(query, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							SQLBIND_DELAYED(query, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND_DELAYED(query, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND_DELAYED(query, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount + teamInfo[TEAM_BLUE].playerCount+teamInfo[TEAM_RED].playerCount);

							SQLBIND_DELAYED_TEXT(query, "@boosts", (boostCountCapper > 0 ? boostsString.c_str() : NULL));
							SQLBIND_DELAYED(query, int, "@boostCount", boostCountCapper);

							SQLBIND_DELAYED(query, double, "@maxSpeedCapperLastSecond", maxSpeedCapperLastSecond);
							SQLBIND_DELAYED(query, double, "@maxSpeedCapper", maxSpeedCapper);
							SQLBIND_DELAYED(query, double, "@averageSpeedCapper", averageSpeedCapper);
							if (nearbyInfo) {
								SQLBIND_DELAYED_TEXT(query, "@nearbyPlayers", nearbyInfo->nearbyPlayersString.c_str());
								SQLBIND_DELAYED(query, int, "@nearbyPlayerCount", nearbyInfo->nearbyPlayersCount);
								SQLBIND_DELAYED_TEXT(query, "@nearbyEnemies", nearbyInfo->nearbyEnemiesString.c_str());
								SQLBIND_DELAYED(query, int, "@nearbyEnemyCount", nearbyInfo->nearbyEnemiescount);
							}
							else {
								SQLBIND_DELAYED_NULL(query, "@nearbyPlayers");
								SQLBIND_DELAYED(query, int, "@nearbyPlayerCount", 0);
								SQLBIND_DELAYED_NULL(query, "@nearbyEnemies");
								SQLBIND_DELAYED(query, int, "@nearbyEnemyCount", 0);
							}

							SQLBIND_DELAYED(query, double, "@maxNearbyEnemyCount", maxNearbyEnemyCount);
							SQLBIND_DELAYED(query, double, "@moreThanOneNearbyEnemyTimePercent", moreThanOneNearbyEnemyTimePercent);
							SQLBIND_DELAYED(query, double, "@averageNearbyEnemyCount", averageNearbyEnemyCount);
							SQLBIND_DELAYED(query, double, "@maxVeryCloseEnemyCount", maxVeryCloseEnemyCount);
							SQLBIND_DELAYED(query, double, "@anyVeryCloseEnemyTimePercent", anyVeryCloseEnemyTimePercent);
							SQLBIND_DELAYED(query, double, "@moreThanOneVeryCloseEnemyTimePercent", moreThanOneVeryCloseEnemyTimePercent);
							SQLBIND_DELAYED(query, double, "@averageVeryCloseEnemyCount", averageVeryCloseEnemyCount);

							if (playerIsVisibleOrFollowed) {
								SQLBIND_DELAYED(query, double, "@positionX", currentPos[0]);
								SQLBIND_DELAYED(query, double, "@positionY", currentPos[1]);
								SQLBIND_DELAYED(query, double, "@positionZ", currentPos[2]);
								SQLBIND_DELAYED(query, double, "@directionX", currentDir[0]);
								SQLBIND_DELAYED(query, double, "@directionY", currentDir[1]);
								SQLBIND_DELAYED(query, double, "@directionZ", currentDir[2]);
							}
							else {
								SQLBIND_DELAYED_NULL(query,  "@positionX");
								SQLBIND_DELAYED_NULL(query,  "@positionY");
								SQLBIND_DELAYED_NULL(query,  "@positionZ");
								SQLBIND_DELAYED_NULL(query,  "@directionX");
								SQLBIND_DELAYED_NULL(query,  "@directionY");
								SQLBIND_DELAYED_NULL(query,  "@directionZ");
							}
							SQLBIND_DELAYED_TEXT(query, "@demoName", sharedVars.oldBasename.c_str());
							SQLBIND_DELAYED_TEXT(query, "@demoPath", sharedVars.oldPath.c_str());
							SQLBIND_DELAYED(query, int, "@demoTime", demoCurrentTime);
							SQLBIND_DELAYED(query, int, "@lastGamestateDemoTime", lastGameStateChangeInDemoTime);
							SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND_DELAYED(query, int, "@demoDerivativeFlags", demoDerivativeFlags);
							SQLBIND_DELAYED(query, int, "@demoDateTime", sharedVars.oldDemoDateModified);
							std::string pastLocationsString = getPlayerSpiralLocationsString(playerNum,demoCurrentTime-flagHoldTime,15);
							SQLBIND_DELAYED_TEXT(query, "@pastLocations", pastLocationsString.c_str());

							if (nearbyInfo) {
								delete nearbyInfo;
							}

							if (activeKillDatabase != -1) {
								queryWrapper->databaseIndex = activeKillDatabase;
								io.captureQueries->push_back(queryWrapper);
							}
							else {
								delete queryWrapper;
								continue;
							}


							int enemyTeam = flagTeam;
							int playerTeam = flagTeam == TEAM_RED ? TEAM_BLUE : TEAM_RED;
							for (int client = 0; client < max_clients; client++) {
								if (playerTeams[client] == playerTeam && client != playerNum) {
									addMetaEvent(METAEVENT_TEAMCAPTURE, demoCurrentTime, client, bufferTime, opts, bufferTime);
								}
								else if (playerTeams[client] == enemyTeam) {
									addMetaEvent(METAEVENT_ENEMYTEAMCAPTURE, demoCurrentTime, client, bufferTime, opts, bufferTime);
								}
							}
							// Meta event tracker for capture
							MetaEventTracker* capME = new MetaEventTracker(demoCurrentTime, queryWrapper, metaEventTrackers[METRACKER_CAPTURES][playerNum], bufferTime, flagHoldTime, resultingMetaEventTracking[METRACKER_CAPTURES]);
							bool wasFollowedThroughBufferTime = playerFirstFollowed[playerNum] != -1 && playerFirstFollowed[playerNum] < (demo.cut.Cl.snap.serverTime - flagHoldTime - bufferTime);
							capME->needsReframe = opts.reframeIfNeeded && !wasFollowedThroughBufferTime;
							capME->reframeClientNum = playerNum;
							capME->addPastEvents(playerPastMetaEvents[playerNum], getMinimumMetaEventBufferTime(playerNum, bufferTime, demoCurrentTime));
							addMetaEvent(METAEVENT_CAPTURE, demoCurrentTime, playerNum, bufferTime, opts, bufferTime);
							metaEventTrackers[METRACKER_CAPTURES][playerNum] = capME;


							if (!playerIsVisibleOrFollowed && !wasVisibleOrFollowed) continue; // No need to cut out those who were not visible at all in any way.
							if (searchMode == SEARCH_MY_CTF_RETURNS && playerNum != demo.cut.Cl.snap.ps.clientNum) continue; // Only cut your own for SEARCH_MY_CTF_RETURNS

							int64_t runStart = demoCurrentTime - flagHoldTime;
							int64_t startTime = runStart - bufferTime;
							int64_t endTime = demoCurrentTime + bufferTime;
							int64_t earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
							bool isTruncated = false;
							int truncationOffset = 0;
							if (earliestPossibleStart > startTime) {
								truncationOffset = earliestPossibleStart - startTime;
								startTime = earliestPossibleStart;
								isTruncated = true;
							}
							//startTime = std::max(lastGameStateChangeInDemoTime+1, startTime); // We can't start before 0 or before the last gamestate change. +1 to be safe, not sure if necessary.

							int milliSeconds = flagHoldTime;
							int pureMilliseconds = milliSeconds % 1000;
							int seconds = milliSeconds / 1000;
							int pureSeconds = seconds % 60;
							int minutes = seconds / 60;

							std::stringstream ss;
							std::stringstream ss2;
							std::string boostString = boostCountCapper > 0 ? va("_BST%d", boostCountCapper) : "";
							ss << mapname << std::setfill('0') << "___CAPTURE"<<(flagCarrierKillCount>0 ? va("%dK", flagCarrierKillCount):"")<<(flagCarrierRetCount>0 ? va("%dR", flagCarrierRetCount):"") << boostString;
							ss2 << std::setfill('0') << "___" << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << "___" << playername << "___P"<< capperLastPickupOrigin <<"T"<<flagTeam<< "___"<< (int)moreThanOneVeryCloseEnemyTimePercent<<"DANGER"<<(int)(averageVeryCloseEnemyCount*100)<<"___"<<(int) maxSpeedCapper<<"_"<<(int)averageSpeedCapper<<"ups" << (wasFollowed ? "" : (wasVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" << playerNum << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

							std::string targetFilename = ss.str();
							std::string targetFilename2 = ss2.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							char* targetFilenameFiltered2 = new char[targetFilename2.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);
							sanitizeFilename(targetFilename2.c_str(), targetFilenameFiltered2);

							std::stringstream batSS;
							std::stringstream batSS2;
							batSS << "\nrem demoCurrentTime: " << demoCurrentTime;
							batSS << "\n" << (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered;
							batSS2 << targetFilenameFiltered2 << "\" " << startTime << " " << endTime;
							batSS2 << (isTruncated ? va(" --meta \"{\\\"trim\\\":%d}\"", truncationOffset) : "");
							queryWrapper->batchString1 = batSS.str();
							queryWrapper->batchString2 = batSS2.str();
							
							delete[] targetFilenameFiltered;
							if (!opts.noFindOutput)  std::cout << targetFilename << targetFilename2 << "\n";


						}
					}
				
					if (thisIsFakeEventEntity) {
						delete thisEs;
						thisEs = NULL;
					}
				}


				// Check jumps (because it relies on events too... otherwise I'd do it up there.
				for (int i = 0; i < max_clients; i++) {
					if (opts.jumpMetaEventsLimit > 0 && jumpDetected[i]) {
						addMetaEvent(METAEVENT_JUMP, demoCurrentTime, i, opts.jumpMetaEventsLimit, opts,bufferTime);
					}
				}


				// Check for killstreaks.
				// Anything happening within KILLSTREAK_MAX_INTERVAL milliseconds of last kill counts towards it.
				// Baseline here is 3 seconds, since that is the limit for kill messages to be displayed as one in stacked kill messages.
				// TODO Also count stuff that maybe isn't within 3 secs but pretty darn close?
				// KILLSTREAK_MIN_KILLS 3
				// KILLSTREAK_MAX_INTERVAL 3000
				// TODO Let it count chains of killstreaks. If there's already a valid killstreak, let there be a single longer gap there.

				for (int cln = 0; cln < max_clients; cln++) {
					requiredMetaEventAges[METRACKER_KILLSPREES][cln] = demoCurrentTime - bufferTime; // We will set the proper value below but kills is a map that might not contain a key for some players so we just go safe here and set the appropriate value below.
				}
#if KILLVECARRAY
				for (int attackerNum = 0; attackerNum <= max_clients; attackerNum++) { // <= because max_clients itself is used for world kills
					if (!clientHasKills[attackerNum]) continue;
					std::vector<Kill>* killsVec = &kills[attackerNum];
					int clientNumAttacker = KILLERARRAYINDEX_REVERSE(attackerNum);
#else
				for (auto clientIt = kills.begin(),endIt = kills.end(); clientIt != endIt; clientIt++) {
					std::vector<Kill>* killsVec = &clientIt->second;
					int clientNumAttacker = clientIt->first;
#endif

					static const int killStreakSpeedTypes[] = { KILLSTREAK_SUPERFAST_MAX_INTERVAL,KILLSTREAK_MAX_INTERVAL,SLOW_KILLSTREAK_MAX_INTERVAL,VERYSLOW_KILLSTREAK_MAX_INTERVAL,VERYVERYSLOW_KILLSTREAK_MAX_INTERVAL,GIGASUPERVERYVERYSLOW_OPTIONAL_KILLSTREAK_MAX_INTERVAL };
					constexpr int spCount = sizeof(killStreakSpeedTypes) / sizeof(int);

					//const int speedTypesSkip = opts.findSuperSlowKillStreaks ? 0 : 1; // Moved to start of function.


					// Update required meta event tracking times
					if(clientNumAttacker < max_clients && clientNumAttacker >=0 ){
						//if (killsVec->size() == 0) {
						//	requiredMetaEventAges[METRACKER_KILLSPREES][clientNumAttacker] = demoCurrentTime - bufferTime; // No kills in backlog. We only have to keep [bufferTime] milliseconds into past.
						//}
						//else 
						if(killsVec->size() > 0)
						{
							requiredMetaEventAges[METRACKER_KILLSPREES][clientNumAttacker] = (*killsVec)[0].time - bufferTime; // Kills in backlog that might become part of killsprees. Take oldest one and go [bufferTime] milliseconds back in time and there's our answer.
						}
					}

					if (killsVec->size() == 0 || (killsVec->back().time + killStreakSpeedTypes[spCount-1-speedTypesSkip]) >= demoCurrentTime) continue; // Might still have an unfinished one here!

					// TODO ... Wait... what happens if there's two shorter duration killsprees behind each other here? Will that get caught? Oh yeah it might.
					for (int sp = 0; sp < (spCount- speedTypesSkip); sp++) { // We do this twice, for every killstreak duration type.

						int maxTimeHere = killStreakSpeedTypes[sp];
						int maxTimeLast = sp == 0 ? -1 : killStreakSpeedTypes[sp-1];
						int countFittingInLastBracket = 0;

						static std::vector<Kill> killsOfThisSpree;
						static std::vector<int> victims;
						static std::vector<std::string> hashes;
						victims.reserve(10);
						hashes.reserve(10);
						killsOfThisSpree.reserve(10);
						victims.clear();
						hashes.clear();
						killsOfThisSpree.clear();

						SpreeInfo spreeInfo;
						Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
						std::stringstream allKillsHashSS;

						int lastKillTime = 0;

						std::string* lastKillHash = NULL;

						for (int i = 0; i < killsVec->size(); i++) {
							Kill* thisKill = &(*killsVec)[i];

							// Whether it was or wasn't a killstreak, there are no killstreaks up to this point, so earlier than current kill time can be safely cleaned up.
							timeCheckedForKillStreaks[clientNumAttacker] = thisKill->time - 1;

							if (thisKill->time <= timeCheckedForKillStreaks[clientNumAttacker]) continue; // This one's already been registered.
							if (thisKill->isSuicide || (!thisKill->isVisible && opts.killSpreesOnlyVisible)) continue; // Uninteresting.


							// Starting or continuing kill spree?
							if (spreeInfo.countKills == 0 || thisKill->time <= (spreeInfo.lastKillTime + maxTimeHere)) {
							
								if (lastKillHash && *lastKillHash == thisKill->hashSourceString) continue; // Weird duplicated kill from lost packets likely, identical hash to last one.
								if (sp > 0  && (spreeInfo.countKills ==0 || thisKill->time <= (spreeInfo.lastKillTime + maxTimeLast))) countFittingInLastBracket++; // Measure how many of the kills would have fit in a faster bracket. If ALL of them would have, we discard this and just count it the faster bracket variant.
								victims.push_back(thisKill->targetClientNum);
								hashes.push_back(thisKill->hash);
								killsOfThisSpree.push_back(*thisKill);
								if (thisKill->isDoom) spreeInfo.countDooms++;
								if (thisKill->isRet) spreeInfo.countRets++;
								if (thisKill->isTeamKill) spreeInfo.countTeamKills++;
								if (thisKill->isExplosion) spreeInfo.countExplosions++;
								if (!thisKill->isFollowed) spreeInfo.countThirdPersons++;
								if (!thisKill->isVisible) spreeInfo.countInvisibles++;
								spreeInfo.totalTime += spreeInfo.countKills == 0? 0: (thisKill->time - spreeInfo.lastKillTime);
								spreeInfo.countKills++;
								spreeInfo.lastKillTime = thisKill->time;
								spreeInfo.maxVictimSpeed = std::max(spreeInfo.maxVictimSpeed,thisKill->victimMaxSpeedPastSecond);
								allKillsHashSS << thisKill->hashSourceString;
								lastKillHash = &thisKill->hashSourceString;
							}
							else {
								// This kill is not part of a killspree. Reset.
								// But first, check if this concludes an existing killspree that we can now save.
								if (countFittingInLastBracket < spreeInfo.countKills) { // If all of the kills would fit in a faster bracket (like for example delay 3000 instead of delay 5000) we don't count this one and only count the faster one. To avoid pointless dupes.
									CheckSaveKillstreak<max_clients>(maxTimeHere,&spreeInfo, clientNumAttacker, &killsOfThisSpree, &victims, &hashes, allKillsHashSS.str(), demoCurrentTime, io, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile, sharedVars.oldBasename, sharedVars.oldPath, sharedVars.oldDemoDateModified, demoType,opts,searchMode,wasDoingSQLiteExecution);
								}

								// Reset.
								Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
								victims.clear();
								hashes.clear();
								killsOfThisSpree.clear();
								allKillsHashSS.str(std::string());
								countFittingInLastBracket = 0;
								i--; // New spree might be coming after and start with the kill we just skipped.
							}
						}
						if ( countFittingInLastBracket < spreeInfo.countKills) { // If all of the kills would fit in a faster bracket (like for example delay 3000 instead of delay 5000) we don't count this one and only count the faster one. To avoid pointless dupes.
							CheckSaveKillstreak<max_clients>(maxTimeHere, &spreeInfo, clientNumAttacker, &killsOfThisSpree, &victims, &hashes, allKillsHashSS.str(), demoCurrentTime, io, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile, sharedVars.oldBasename, sharedVars.oldPath, sharedVars.oldDemoDateModified, demoType,opts,searchMode,wasDoingSQLiteExecution);
						}

					}
					// Clean up old kills that no longer have to be stored
					// We can clear the entire thing since earlier we made sure that we aren't in the middle of an ongoing killstreak
					// So anything that's in there now was already checked for being part of a killstreak.
					// So we can get rid of it all.
					killsVec->clear();
#if KILLVECARRAY
					clientHasKills.clearbit(attackerNum);
#endif

					// Can reset this now as no pending killsprees are here anymore.
					if (clientNumAttacker < max_clients && clientNumAttacker >= 0) {
						requiredMetaEventAges[METRACKER_KILLSPREES][clientNumAttacker] = demoCurrentTime - bufferTime;
					}
				
					// Clean up old speeds
					// We do need to keep speeds of at least OLDER_SPEEDS_STORE_LIMIT because we might have to check past speeds for future kills that aren't logged yet.
					// So let's find the last speed we're erasing
					/*std::map<int, float>::iterator lastToRemove;
					int countToRemove = 0;
					for (auto speedsIt = speeds[clientNumAttacker].begin(); speedsIt != speeds[clientNumAttacker].end(); speedsIt++) {
						if ((demoCurrentTime - speedsIt->first) > OLDER_SPEEDS_STORE_LIMIT) {
							lastToRemove = speedsIt;
							countToRemove++;
						}
					}
					if (countToRemove) {
						speeds[clientNumAttacker].erase(speeds[clientNumAttacker].begin(), lastToRemove);
					}*/
					if(clientNumAttacker >= 0 && clientNumAttacker < max_clients){
						int countToRemove = 0;
						for (auto it= speeds[clientNumAttacker].begin(),end= speeds[clientNumAttacker].end(); it!=end;it++) {
							if ((demoCurrentTime - it->time) > OLDER_SPEEDS_STORE_LIMIT) { // BUT WAIT. What about killsprees with maxspeedattacker etc? oh wait. nvm
								countToRemove++;
							}
						}
						if (countToRemove && countToRemove > 100) { // don't do remove operations on every frame.
							speeds[clientNumAttacker].erase(speeds[clientNumAttacker].begin(), speeds[clientNumAttacker].begin()+ countToRemove);
						}
					}
				}


				// Find out which players are visible / followed
				// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
				//lastKnownFlagCarrier[TEAM_BLUE] = lastKnownFlagCarrier[TEAM_RED] = -1;
				if (redFlagStatusResetByConfigstring) { // First reset flag carrier if the flag status has been reset via configstring.
					lastKnownFlagCarrier[TEAM_RED] = -1;
				}
				if (blueFlagStatusResetByConfigstring) {
					lastKnownFlagCarrier[TEAM_BLUE] = -1;
				}
				if (lastKnownFlagCarrier[TEAM_RED] != -1) {
					recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_RED]; 
				}
				if (lastKnownFlagCarrier[TEAM_BLUE] != -1) {
					recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_BLUE]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_BLUE];
				}
				if (redFlagNewCarrierByEventBitMask) { // Now set the flag carrier if we got any flag pickup events. And afterwards we check for entities with the flag additionally.
					int64_t possiblePlayers = redFlagNewCarrierByEventBitMask; // These are all players who picked up a red flag this frame (silly I know!)
					possiblePlayers &= ~playersKilledThisFrameBitMask; // These are players who were killed this frame, so they can no longer have the flag.
					possiblePlayers &= ~redFlagCapturedPlayerBitMask; // These are players who captured the flag this frame, so they too can no longer have it.
					
					// Check if possiblePlayers is a power of 2
					if (possiblePlayers && !(possiblePlayers & (possiblePlayers - 1))) {
						// It's one possible player.
						lastKnownFlagCarrier[TEAM_RED] = (int)(log2(possiblePlayers)+0.5); // + 0.5 to avoid double to int conversion issues issues.
					}
					else {
						// Either no player left or multiple ones. In short, we don't know.
						lastKnownFlagCarrier[TEAM_RED] = -1;
					}
				}
				if (blueFlagNewCarrierByEventBitMask) {
					// Same logic as with red flag above.
					int64_t possiblePlayers = blueFlagNewCarrierByEventBitMask; 
					possiblePlayers &= ~playersKilledThisFrameBitMask; 
					possiblePlayers &= ~blueFlagCapturedPlayerBitMask; 

					if (possiblePlayers && !(possiblePlayers & (possiblePlayers - 1))) {
						lastKnownFlagCarrier[TEAM_BLUE] = (int)(log2(possiblePlayers) + 0.5);
					}
					else {
						lastKnownFlagCarrier[TEAM_BLUE] = -1;
					}
				}
				vec3_t lastKnownFlagCarrierPosition[TEAM_NUM_TEAMS];
				vec3_t lastKnownFlagCarrierVelocity[TEAM_NUM_TEAMS];
				for (int p = 0; p < max_clients; p++) {
					// Go through parseenttities of last snap to see if client is in it
					bool clientIsInSnapshot = false;
					bool clientVisibleOrFollowed = false;
					// TODO This is kind of messy uuuh... fix it?!
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum+demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						if (thisEntity->number == p) {
							clientIsInSnapshot = true;

							// TODO check playerteam bc ghost powerups? - I think that's not needed as we are checking entities and not scoreboard data.
							if (thisEntity->powerups & (1 << PW_REDFLAG)) {
								lastKnownFlagCarrier[TEAM_RED] = thisEntity->number;
								VectorCopy(thisEntity->pos.trBase,lastKnownFlagCarrierPosition[TEAM_RED]);
								VectorCopy(thisEntity->pos.trDelta,lastKnownFlagCarrierVelocity[TEAM_RED]);
								recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_RED]; // Hmm but this creates a wrong value on the first frame of holding the flag. If it's a one-frame capture we will end up with a wrong time duration held ...
							}
							else if (thisEntity->powerups & (1 << PW_BLUEFLAG)) {
								lastKnownFlagCarrier[TEAM_BLUE] = thisEntity->number;
								VectorCopy(thisEntity->pos.trBase, lastKnownFlagCarrierPosition[TEAM_BLUE]);
								VectorCopy(thisEntity->pos.trDelta, lastKnownFlagCarrierVelocity[TEAM_BLUE]);
								recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_BLUE]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_BLUE];
							}
							else { 
								// TODO also reset recent flag hold time here?

								// A little safety backup. If we see this player without a flag and he is remembered as the last flag carrier, reset it.
								// We'll just keep this for a while maybe, worst case just as debug. If it turns out that it's not needed we can remove it.
								if (lastKnownFlagCarrier[TEAM_RED] == p) {
									demoErrorFlags |= DERR_GAMELOGICFLAW;
									demoErrors << "Player entity ("<< p << (thisEntity->eFlags & EF_DEAD ? ", dead": "") << ") is not carrying red flag, but remembered as lastKnownFlagCarrier[TEAM_RED] WTF, resetting\n";
									std::cerr << "Player entity ("<< p << (thisEntity->eFlags & EF_DEAD ? ", dead": "") << ") is not carrying red flag, but remembered as lastKnownFlagCarrier[TEAM_RED] WTF, resetting" << "(" << DPrintFLocation << ")\n";
									lastKnownFlagCarrier[TEAM_RED] = -1;
								}
								if (lastKnownFlagCarrier[TEAM_BLUE] == p) {
									demoErrorFlags |= DERR_GAMELOGICFLAW;
									demoErrors << "Player entity (" << p << (thisEntity->eFlags & EF_DEAD ? ", dead" : "") << ") is not carrying blue flag, but remembered as lastKnownFlagCarrier[TEAM_BLUE] WTF, resetting\n";
									std::cerr << "Player entity (" << p << (thisEntity->eFlags & EF_DEAD ? ", dead" : "") << ") is not carrying blue flag, but remembered as lastKnownFlagCarrier[TEAM_BLUE] WTF, resetting" << "(" << DPrintFLocation << ")\n";
									lastKnownFlagCarrier[TEAM_BLUE] = -1;
								}

								// Reset the required meta event age for capture cuts if the player is seen without a flag.
								// This means the player is not currently holding a flag, therefore he can't be in the process of capturing a flag,
								// hence we can stop tracking meta events for him into the past in case we want to do a cut of him with meta events.
								
								// Doing this purely based on player entities visible without flag could lead to the value not being updated for some time when a player wasn't visible,
								// theoretically leading to accumulated amounts of meta events.
								// However ... if a player isn't visible, the most cluttery meta events won't even come into existence (jumps, saberhits etc) because that player 
								// has to be visible for those events to be tracked. So it's ok I think.

								// On the flipside... if the player is seen WITH the flag, we don't update this time, therefore we have the nice value going *at least* far enough into the past 
								// to the point where he didn't have the flag.
								requiredMetaEventAges[METRACKER_CAPTURES][p] = demoCurrentTime - bufferTime;
							}
						}
					}
					if (demo.cut.Cl.snap.ps.powerups[PW_REDFLAG] && demo.cut.Cl.snap.ps.persistant[PERS_TEAM] < TEAM_SPECTATOR && playerTeams[demo.cut.Cl.snap.ps.clientNum] < TEAM_SPECTATOR) {

						lastKnownFlagCarrier[TEAM_RED] = demo.cut.Cl.snap.ps.clientNum;
						VectorCopy(demo.cut.Cl.snap.ps.origin, lastKnownFlagCarrierPosition[TEAM_RED]);
						VectorCopy(demo.cut.Cl.snap.ps.velocity, lastKnownFlagCarrierVelocity[TEAM_RED]);
						recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_RED];
					}else if (demo.cut.Cl.snap.ps.powerups[PW_BLUEFLAG] && demo.cut.Cl.snap.ps.persistant[PERS_TEAM] < TEAM_SPECTATOR && playerTeams[demo.cut.Cl.snap.ps.clientNum] < TEAM_SPECTATOR) {

						lastKnownFlagCarrier[TEAM_BLUE] = demo.cut.Cl.snap.ps.clientNum;
						VectorCopy(demo.cut.Cl.snap.ps.origin, lastKnownFlagCarrierPosition[TEAM_BLUE]);
						VectorCopy(demo.cut.Cl.snap.ps.velocity, lastKnownFlagCarrierVelocity[TEAM_BLUE]);
						recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_BLUE]] = demoCurrentTime - cgs.flagLastChangeToTaken[TEAM_BLUE];
					}
					else {
						// A little safety backup. If we see this player without a flag and he is remembered as the last flag carrier, reset it.
						// We'll just keep this for a while maybe, worst case just as debug. If it turns out that it's not needed we can remove it.
						if (lastKnownFlagCarrier[TEAM_RED] == demo.cut.Cl.snap.ps.clientNum) {
							if (psGeneralPMType != PM_INTERMISSION_GENERAL) {
								demoErrorFlags |= DERR_GAMELOGICFLAW;
								std::cerr << "Playerstate (" << demo.cut.Cl.snap.ps.clientNum << (demo.cut.Cl.snap.ps.stats[STAT_HEALTH] <= 0 ? ", dead" : "") << ") is not carrying red flag, but remembered as lastKnownFlagCarrier[TEAM_RED] WTF, resetting\n";
								demoErrors << "Playerstate (" << demo.cut.Cl.snap.ps.clientNum << (demo.cut.Cl.snap.ps.stats[STAT_HEALTH] <= 0 ? ", dead" : "") << ") is not carrying red flag, but remembered as lastKnownFlagCarrier[TEAM_RED] WTF, resetting" << "(" << DPrintFLocation << ")\n";
							}
							lastKnownFlagCarrier[TEAM_RED] = -1;
						}
						if (lastKnownFlagCarrier[TEAM_BLUE] == demo.cut.Cl.snap.ps.clientNum) {
							if (psGeneralPMType != PM_INTERMISSION_GENERAL) {
								demoErrorFlags |= DERR_GAMELOGICFLAW;
								demoErrors << "Playerstate (" << demo.cut.Cl.snap.ps.clientNum << (demo.cut.Cl.snap.ps.stats[STAT_HEALTH] <= 0 ? ", dead" : "") << ") is not carrying blue flag, but remembered as lastKnownFlagCarrier[TEAM_BLUE] WTF, resetting\n";
								std::cerr << "Playerstate (" << demo.cut.Cl.snap.ps.clientNum << (demo.cut.Cl.snap.ps.stats[STAT_HEALTH] <= 0 ? ", dead" : "") << ") is not carrying blue flag, but remembered as lastKnownFlagCarrier[TEAM_BLUE] WTF, resetting" << "(" << DPrintFLocation << ")\n";
							}
							lastKnownFlagCarrier[TEAM_BLUE] = -1;
						}

						// Same logic as with entities above, see comment above. But ofc playerstate is always visible but view angle can change
						// So I think overall similar logic applies here too and besides, let's keep it consistent.
						requiredMetaEventAges[METRACKER_CAPTURES][demo.cut.Cl.snap.ps.clientNum] = demoCurrentTime - bufferTime;
					}

					if (clientIsInSnapshot) {
						clientVisibleOrFollowed = true;
						if (playerFirstVisible[p] == -1) {
							playerFirstVisible[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						playerFirstVisible[p] = -1;
					}
					if (demo.cut.Cl.snap.ps.clientNum == p) {
						clientVisibleOrFollowed = true;
						if (playerFirstFollowed[p] == -1) {
							playerFirstFollowed[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						// Set reframe required.
						if (playerFirstFollowed[p] != -1) {
							// Little optimization. Went from followed to not followed. 
							// Don't wanna set this every single frame we're not followed.
							setReframeRequired(demoCurrentTime, p, opts, bufferTime);
						}

						// Set not followed
						playerFirstFollowed[p] = -1;
					}
					if (clientVisibleOrFollowed) {
						playerLastSeen[p] = demoCurrentTime;
						if (playerFirstFollowedOrVisible[p] == -1) {
							playerFirstFollowedOrVisible[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						playerFirstFollowedOrVisible[p] = -1;
					}
				}

				// save maxspeed of flag hold
				if (lastKnownFlagCarrier[TEAM_RED] != -1) {
					if (recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]]) {
						// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
						Com_Memset(&recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]], 0, sizeof(VariousCappingInfo));
					}
					float speedHere = VectorLength(lastKnownFlagCarrierVelocity[TEAM_RED]);
					if (recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].maxSpeedThisRun < speedHere) {
						recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].maxSpeedThisRun = speedHere;
					}
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].sumSpeeds += speedHere* deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].divisorSpeeds += deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_RED]].lastUpdateTime = demoCurrentTime;
				}
				if (lastKnownFlagCarrier[TEAM_BLUE] != -1) {
					if (recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_BLUE]]) {
						// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
						Com_Memset(&recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]], 0, sizeof(VariousCappingInfo));
					}
					float speedHere = VectorLength(lastKnownFlagCarrierVelocity[TEAM_BLUE]);
					if (recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].maxSpeedThisRun < speedHere) {
						recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].maxSpeedThisRun = speedHere;
					}
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].sumSpeeds += speedHere * deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].divisorSpeeds += deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownFlagCarrier[TEAM_BLUE]].lastUpdateTime = demoCurrentTime;
				}


				// Check amount of enemies nearby cappers and log it in case we need it later.
				if (lastKnownFlagCarrier[TEAM_BLUE]!=-1 || lastKnownFlagCarrier[TEAM_RED]!=-1) { // Don't bother if no flag carrier is visible at all
					int enemiesNearRedCarrier =0, enemiesNearBlueCarrier = 0;
					int enemiesVeryCloseRedCarrier =0, enemiesVeryCloseBlueCarrier = 0;

					
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						if (thisEntity->number < 0 || thisEntity->number >= max_clients || playerTeams[thisEntity->number]==TEAM_SPECTATOR) continue;
						if (lastKnownFlagCarrier[TEAM_BLUE] != -1 && playerTeams[thisEntity->number] == TEAM_BLUE) {
							float distance = VectorDistance(lastKnownFlagCarrierPosition[TEAM_BLUE],thisEntity->pos.trBase);
							if (distance <= NEARBY_PLAYER_MAX_DISTANCE) {
								enemiesNearBlueCarrier++;
							}
							if (distance <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
								enemiesVeryCloseBlueCarrier++;
							}
						}
						if (lastKnownFlagCarrier[TEAM_RED] != -1 && playerTeams[thisEntity->number] == TEAM_RED){
							float distance = VectorDistance(lastKnownFlagCarrierPosition[TEAM_RED], thisEntity->pos.trBase);
							if (distance <= NEARBY_PLAYER_MAX_DISTANCE) {
								enemiesNearRedCarrier++;
							}
							if (distance <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
								enemiesVeryCloseRedCarrier++;
							}
						}
					}

					// Resets if necessary.
					if (lastKnownFlagCarrier[TEAM_BLUE] != -1) {
						if (recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_BLUE]].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_BLUE]]) {
							// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
							Com_Memset(&recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_BLUE]], 0, sizeof(EnemyNearbyInfo));
						}
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_BLUE]].enemyNearbyTimes[enemiesNearBlueCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_BLUE]].enemyVeryCloseTimes[enemiesVeryCloseBlueCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_BLUE]].lastUpdateTime = demoCurrentTime;
					}
					if (lastKnownFlagCarrier[TEAM_RED] != -1) {
						if (recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_RED]].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]]) {
							// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
							Com_Memset(&recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_RED]], 0, sizeof(EnemyNearbyInfo));
						}
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_RED]].enemyNearbyTimes[enemiesNearRedCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_RED]].enemyVeryCloseTimes[enemiesVeryCloseRedCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownFlagCarrier[TEAM_RED]].lastUpdateTime = demoCurrentTime;
					}

				}
				// End logging nearby enemies

				// Save parries and hits for playerstats
				// The saber hit detection isn't really all that reliable but it's good enough for a sort of global stats I guess
				for (int playerNum = 0; playerNum < max_clients; playerNum++) {
					if (hitDetectionData[playerNum].confirmedHit || hitDetectionData[playerNum].nearbySaberHitDetected && hitDetectionData[playerNum].painDetected) {
						if (playerDemoStatsPointers[playerNum]) {
							playerDemoStatsPointers[playerNum]->everUsed = qtrue;
							playerDemoStatsPointers[playerNum]->hitBySaberCount++;
							if (lastFrameInfo.canBlockSimplified[playerNum]) {
								playerDemoStatsPointers[playerNum]->hitBySaberBlockableCount++;
							}
						}
					}
					if (hitDetectionData[playerNum].newParryDetected) {
						if (playerDemoStatsPointers[playerNum]) {
							playerDemoStatsPointers[playerNum]->everUsed = qtrue;
							playerDemoStatsPointers[playerNum]->parryCount++;
						}
					}
					// New attack from parry?
					if (playerLastSaberMove[playerNum].lastSaberMove[0].saberMoveChange == demoCurrentTime // Happened on this frame, avoid duplicates
						&& playerLastSaberMove[playerNum].lastSaberMove[1].saberMoveGeneral >= LS_PARRY_UP_GENERAL && playerLastSaberMove[playerNum].lastSaberMove[1].saberMoveGeneral <= LS_PARRY_LL_GENERAL // last one was parry
						&& playerLastSaberMove[playerNum].lastSaberMove[0].saberMoveGeneral == saberMoveData_general[playerLastSaberMove[playerNum].lastSaberMove[1].saberMoveGeneral].chain_attack // this one is the result if you click atack
						) {
						if (playerDemoStatsPointers[playerNum]) {
							playerDemoStatsPointers[playerNum]->everUsed = qtrue;
							playerDemoStatsPointers[playerNum]->attackFromParryCount++;
						}
					}
				}
				
				break;
			case svc_download_general:
				// read block number
				buf = MSG_ReadShort(&oldMsg);
				if (!buf)	//0 block, read file size
					MSG_ReadLong(&oldMsg);
				// read block size
				buf = MSG_ReadShort(&oldMsg);
				// read the data block
				for (; buf > 0; buf--)
					MSG_ReadByte(&oldMsg);
				break;
			case svc_setgame_general:
				{
					static char	newGameDir[MAX_QPATH];
					int i = 0;
					while (i < MAX_QPATH) {
						int next = MSG_ReadByte(&oldMsg);
						if (next) {
							newGameDir[i] = next;
						}
						else {
							break;
						}
						i++;
					}
					newGameDir[i] = 0;
					// But here we stop, and don't do more. If this goes horribly wrong sometime, you might have to go and actually do something with this.
				}
				break;
			case svc_mapchange_general:
				// nothing to parse.
				break;
			}
		
			previouscmd = cmd;
			previousOcmd = ocmd;
		}

		// Check for finished laugh sequences
		checkSaveLaughs(demoCurrentTime,bufferTime, lastGameStateChangeInDemoTime,io,&sharedVars.oldBasename,&sharedVars.oldPath, sharedVars.oldDemoDateModified,sourceDemoFile,qfalse,wasDoingSQLiteExecution,opts);


		int firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;

		bool hadConfigStringCommands = false;
		// process any new server commands
		for (; demo.cut.Clc.lastExecutedServerCommand <= demo.cut.Clc.serverCommandSequence; demo.cut.Clc.lastExecutedServerCommand++) {
			char* command = demo.cut.Clc.serverCommands[demo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
			Cmd_TokenizeString(command);
			char* cmd = Cmd_Argv(0);
			//if (cmd[0] && !firstServerCommand) {
			//	firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
			//}


			if (opts.doStringSearch) {
				std::string rawcommand = command;
				std::string strippedCommand = Q_StripColorAll(rawcommand);
				if (strstr(rawcommand.c_str(), opts.stringSearch.c_str()) || strstr(strippedCommand.c_str(), opts.stringSearch.c_str())) {
					parsedChatMessage_t msgInfo;
					if (!strcmp(cmd, "chat") || !strcmp(cmd, "tchat")) {
						msgInfo = ParseChatMessage<max_clients>(&demo.cut.Cl, demoType, Cmd_Argv(1), Cmd_Argc() >= 3 ? Cmd_Argv(2) : NULL);
					}
					logSpecialThing("STRINGSEARCH", msgInfo.isValid? msgInfo.message : opts.stringSearch, rawcommand, msgInfo.isValid ? msgInfo.playerName : "", msgInfo.isValid ? msgInfo.playerNum : -1, -1, demoCurrentTime, 0, bufferTime, lastGameStateChangeInDemoTime, io, & sharedVars.oldBasename, & sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
				}
			}

			if (!strcmp(cmd, "chat") || !strcmp(cmd, "tchat")) {
				std::string rawChatCommand = command;
				std::string chatCommand = Q_StripColorAll(rawChatCommand);

				if (opts.makeVideo) {
					videoConsole.push_back({ demoCurrentTime,Cmd_Argv(1)});
				}

				if (Q_stristr(rawChatCommand.c_str(), "!mark") || Q_stristr(chatCommand.c_str(), "[DEMOMOMENT]")) {
					parsedChatMessage_t msgInfo = ParseChatMessage<max_clients>(&demo.cut.Cl, demoType, Cmd_Argv(1), Cmd_Argc() >= 3 ? Cmd_Argv(2) : NULL);
					if (msgInfo.isValid) {
						std::vector<std::string> splitMsg = splitString(msgInfo.message, " ");
						if (splitMsg.size()) {
							std::string cleaned = Q_StripColorAll(splitMsg[0]);
							bool isMark = false;
							int reframeClientNum = -1;
							int duration = 60000;
							std::string playerName = msgInfo.playerName;
							// TODO should i keep the future 1 minute too like the original ones? eh i can do it in the gui i guess
							if (!_stricmp(cleaned.c_str(), "[DEMOMOMENT]")) {
								isMark = true;
								reframeClientNum = msgInfo.playerNum;
							}
							else if (!_stricmp(cleaned.c_str(), "!markme")) {
								isMark = true;
								reframeClientNum = msgInfo.playerNum;
								if (splitMsg.size() > 1) {
									int number = atoi(splitMsg[1].c_str());
									if (number > 0 && number <= 20) {
										duration = 60000 * number;
									}
								}
							}
							else if (!_stricmp(cleaned.c_str(), "!mark")) {
								isMark = true;
								playerName = "";
								if (splitMsg.size() > 1) {
									int number = atoi(splitMsg[1].c_str());
									if (number > 0 && number <= 20) {
										duration = 60000 * number;
									}
								}
							}
							else if (!_stricmp(cleaned.c_str(), "!markas")) {
								isMark = true;
								bool haveClientNum = false;
								if (splitMsg.size() > 1) {
									char startChar = splitMsg[1][0];
									if (startChar >= '0' && startChar <= '9') {
										int number = atoi(splitMsg[1].c_str());
										if (number >= 0 && number < max_clients) {
											reframeClientNum = number;
											//int CS_PLAYERS_here = getCS_PLAYERS(demoType);
											int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + reframeClientNum];
											const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
											playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, isMOHAADemo ? "name" : "n");
											haveClientNum = true;
										}
									}
								}
								if (splitMsg.size() > 2) {
									int number = atoi(splitMsg[2].c_str());
									if (number > 0 && number <= 20) {
										duration = 60000 * number;
									}
								}
							}


							if (isMark) {
								logSpecialThing("MARK", msgInfo.message, rawChatCommand, playerName, reframeClientNum, msgInfo.playerNum, demoCurrentTime, duration, bufferTime, lastGameStateChangeInDemoTime, io, &sharedVars.oldBasename, &sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
							}

						}
					}
					else {
					}
				}

				// Detect a special chat search
				if (opts.doChatSearch) {
					if (strstr(rawChatCommand.c_str(),opts.chatSearch.c_str()) || strstr(chatCommand.c_str(), opts.chatSearch.c_str())) {
						parsedChatMessage_t msgInfo = ParseChatMessage<max_clients>(&demo.cut.Cl, demoType, Cmd_Argv(1), Cmd_Argc() >= 3 ? Cmd_Argv(2) : NULL);
						logSpecialThing("CHATSEARCH", msgInfo.isValid ? msgInfo.message : opts.chatSearch, rawChatCommand, msgInfo.isValid ? msgInfo.playerName : "", msgInfo.isValid ? msgInfo.playerNum : -1, -1, demoCurrentTime, 0, bufferTime, lastGameStateChangeInDemoTime, io, &sharedVars.oldBasename, &sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
					}
				}
				if (opts.writeChatsUnique) {
					parsedChatMessage_t msgInfo = ParseChatMessage<max_clients>(&demo.cut.Cl, demoType, Cmd_Argv(1), Cmd_Argc() >= 3 ? Cmd_Argv(2) : NULL);
					
					if (msgInfo.message.length() > 0) {

						SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
						SQLDelayedQuery* query = &queryWrapper->query;

						SQLBIND_DELAYED_TEXT(query, "@chat", msgInfo.message.c_str());
						SQLBIND_DELAYED(query, int, "@demoType", demoType);

						io.chatsUniqueQueries->push_back(queryWrapper);
					}
				}

				// Detect a laugh
				jp::VecNum vec_num, vec_num2;
				jp::RegexMatch rm, rm2;

				size_t count = rm.setRegexObject(&regexLaugh)                          //set associated Regex object
					.setSubject(&chatCommand)                         //set subject string
					.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
					.match();

				for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
					std::string matchedText= vec_num[matchNum][1];
					if (firstLaugh == -1) {
						firstLaugh = demoCurrentTime; // This is the first laugh of this laugh spree.
					}
					if (demoCurrentTime - firstLaugh <= MAX_LAUGH_DELAY) {
						laughs << (laughCount++ > 0 ? "," : "") << matchedText;

					}
					lastLaugh = demoCurrentTime;
					for (int client = 0; client < max_clients; client++) {
						// TODO Check WHO laughed and maybe add extra type for that or check if ppl were visible etc? Whatever.
						addMetaEvent(METAEVENT_LAUGH, demoCurrentTime, client, bufferTime, opts, bufferTime);
					}
				}

				// If we are in the middle of a laugh sequence, we temporarily log the entire chat no matter what.
				if (firstLaugh != -1 && demoCurrentTime - firstLaugh <= MAX_LAUGH_DELAY) {
					laughsChatlog << command << "\n";
				}
			}
			else if (!strcmp(cmd, "cs")) {
				if (!demoCutConfigstringModified(&demo.cut.Cl,demoType)) {
					demoErrorFlags |= DERR_ERRORCNFSTRMOD;
					demoErrors << "Error calling configstringModified\n";
					goto cuterror;
				}

				int index = atoi(Cmd_Argv(1));
				if (index == CS_FLAGSTATUS) {
					char* str = Cmd_Argv(2);

					int redflagTmp, blueflagTmp, yellowflagTmp;
					// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
					if (strlen(str) >= 2) {
						redflagTmp = str[0] - '0';
						blueflagTmp = str[1] - '0';
					}
					else { // This is some weird bug/imperfection in the code. Sometimes it just sends cs 23 0 for whatever reason. Seems to happen at end of games.
						redflagTmp = 0;
						blueflagTmp = 0;
					}
					if (strlen(str) >= 3) { // Too lazy to do other way lol.
						yellowflagTmp = str[2] - '0';
					}
					else {
						yellowflagTmp = 0;
					}

					if (cgs.flag[TEAM_RED] != redflagTmp) {
						/* // Actually, we can't do this here bc lastKnownFlagCarrier[TEAM_RED] might not be actually known.
						if (redflagTmp == 1) { // Was just picked up. Set flag hold time to 0.
							recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]] = 0;
						}
						if (cgs.flag[TEAM_RED] == 1) { // Was taken before. So just count the flag hold time.
							recentFlagHoldTimes[lastKnownFlagCarrier[TEAM_RED]] = demoCurrentTime-cgs.flagLastChange[TEAM_RED];
						}*/
						cgs.flagLastChange[TEAM_RED] = demoCurrentTime;
						if (redflagTmp == 1) {
							cgs.flagLastChangeToTaken[TEAM_RED] = demoCurrentTime;
							//teamInfo[TEAM_RED].flagHoldOrigin = cgs.flag[TEAM_RED];
							cgs.flagLastPickupOrigin[TEAM_RED] = cgs.flag[TEAM_RED];
						}
					}
					if (cgs.flag[TEAM_BLUE] != blueflagTmp) {
						cgs.flagLastChange[TEAM_BLUE] = demoCurrentTime;
						if (blueflagTmp == 1) {
							cgs.flagLastChangeToTaken[TEAM_BLUE] = demoCurrentTime;
							//teamInfo[TEAM_BLUE].flagHoldOrigin = cgs.flag[TEAM_BLUE];
							cgs.flagLastPickupOrigin[TEAM_BLUE] = cgs.flag[TEAM_BLUE];
						}
					}
					if (cgs.flag[TEAM_FREE] != yellowflagTmp) {
						cgs.flagLastChange[TEAM_FREE] = demoCurrentTime;
						if (yellowflagTmp == 1) {
							cgs.flagLastChangeToTaken[TEAM_FREE] = demoCurrentTime;
							//teamInfo[TEAM_FREE].flagHoldOrigin = cgs.flag[TEAM_FREE]; // Not sure if this is correct?
							cgs.flagLastPickupOrigin[TEAM_FREE] = cgs.flag[TEAM_FREE]; // Not sure if this is correct?
						}
					}
					cgs.flag[TEAM_RED] = redflagTmp;
					cgs.flag[TEAM_BLUE] = blueflagTmp;
					cgs.flag[TEAM_FREE] = yellowflagTmp;
					/*if (cgs.isCTFMod && cgs.CTF3ModeActive)
						demo.cgs.flag[TEAM_FREE] = str[2] - '0';
					else
						demo.cgs.flag[TEAM_FREE] = 0;*/
				}

				hadConfigStringCommands = true;
			}
			else if (opts.dumpStufftext && !strcmp(cmd, "stufftext")) {
				
				std::cerr << "STUFFTEXT DUMP: " << Cmd_Argv(1) << "\n";

			}
			else if (opts.makeVideo && !strcmp(cmd, "cp"))
			{
				if (opts.makeVideo) {
					screenCenterText.push_back({ demoCurrentTime,Cmd_Argv(1) });
				}
			}
			else if (!strcmp(cmd, "print")) {
				//Looking for 
				//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."

				// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.
				
				//if (isMOHAADemo) { // We do this in another place now.
				//	parseMOHAADeathMessage(&playerNamesToClientNums, Cmd_Argv(1));
				//}

				//jp::VecNum vec_num;
				//jp::RegexMatch rm;
				qboolean runFound = qfalse;
				defragRunInfoFinal_t runInfo;

				std::string printText = Cmd_Argv(1);

				if (opts.makeVideo) {
					videoConsole.push_back({ demoCurrentTime,printText });
				}

				if (opts.doPrintSearch) {
					std::string strippedPrint = Q_StripColorAll(printText);
					if (strstr(printText.c_str(), opts.printSearch.c_str()) || strstr(strippedPrint.c_str(), opts.printSearch.c_str())) {
						logSpecialThing("PRINTSEARCH", opts.printSearch, printText,"", -1, -1, demoCurrentTime, 0, bufferTime, lastGameStateChangeInDemoTime, io, &sharedVars.oldBasename, &sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution, opts, searchMode, demoType);
					}
				}

#if DEBUG
				//std::cout << printText << "\n";
#endif

				/*size_t count = rm.setRegexObject(&defragRecordFinishRegex)                          //set associated Regex object
					.setSubject(&printText)                         //set subject string
					.setNumberedSubstringVector(&vec_num)         //pass pointer to VecNum vector
					.match();*/
				
				if (printText.find("Timer started") != std::string::npos) {
					strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].lastReset = demoCurrentTime;
					if (strafeApplicablePlayerStateThisFrame) {
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.divisor = 1;
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.sum = playerStateStrafeDeviationThisFrame;
					}
					else {
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.divisor = 0;
						strafeDeviationsDefrag[demo.cut.Cl.snap.ps.clientNum].averageHelper.sum = 0;
					}
				}

				if (findFreedomDefragRun<max_clients>(printText,&runInfo, qfalse, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here)) {
					runFound = qtrue;
				} else if (findOCDefragRun(printText, &runInfo.info)) {
					runFound = qtrue;
				} else if (findRazorDefragRun(printText, &runInfo.info)) {
					runFound = qtrue;
				} else if (findJAProDefragRun<max_clients>(printText, &runInfo.info, demoType)) {
					runFound = qtrue;
				}
				else {
					findDARKDefragRun(printText);
				}


				//for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
				if (runFound) { // really its just going to be 1 but whatever

					if (!runInfo.meta.isSet) {
						GetRunMeta<max_clients>(runInfo.info, runInfo.meta, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here);
					}
					qboolean result = SaveDefragRun<max_clients>(runInfo, sharedVars, sourceDemoFile, io, bufferTime, opts, searchMode, wasDoingSQLiteExecution);
					if (!result) continue;
				}

				
				
			}
		}

		if (DARKDefragRuns.size()) {
			// Dark sends multiple messages depending on whether something was a personal best, record, etc...
			// Luckily it sends them on the same frame. Unluckily, it sends them in such an order that the info about whether something was a PB or WR comes after the actual time etc.
			// So we first get the info about the time, then info about PB, then info about record. 
			// So we store all found runs in an array and just update it from later messages. Then we store it all.
			for (int i = 0; i < DARKDefragRuns.size();i++) {
				GetRunMeta<max_clients>(DARKDefragRuns[i].info, DARKDefragRuns[i].meta, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here);
				SaveDefragRun<max_clients>(DARKDefragRuns[i], sharedVars, sourceDemoFile, io, bufferTime, opts, searchMode, wasDoingSQLiteExecution);
			}
			DARKDefragRuns.clear();
		}

		if (hadConfigStringCommands) {
			CheckForNameChanges<max_clients>(&demo.cut.Cl, io, demoType, wasDoingSQLiteExecution,opts);
			setPlayerAndTeamData<max_clients>(&demo.cut.Cl, demoType);
			updateForcePowersInfo(&demo.cut.Cl);
			updateGameInfo(&demo.cut.Cl,demoType,opts);
			if (opts.quickSkipNonSaberExclusive && !gameIsSaberOnlyIsh) {
				std::cout << "Quick skipping demo of game that allows other weapons than saber/melee/explosives.";
				goto cutcomplete;
			}
			updatePlayerDemoStatsArrayPointers<max_clients>(demoType, opts);
		}



		if ( opts.writeDemoPacketStats && (oldSize == 0 || (currentPacketPeriodStats.periodTotalTime > 0 && (opts.writeDemoPacketStats < 0 || (demoCurrentTime - lastPacketStatsWritten) >= opts.writeDemoPacketStats)))) {

			SQLDelayedQueryWrapper_t* queryWrapper = new SQLDelayedQueryWrapper_t();
			SQLDelayedQuery* query = &queryWrapper->query;
			
			int non0periodTotalTime = std::max(currentPacketPeriodStats.periodTotalTime, 1); // Avoid division by 0
			int non0totalPacketCount = std::max(currentPacketPeriodStats.totalPacketCount, 1); // Avoid division by 0
			SQLBIND_DELAYED(query, int, "@timeSinceLast", currentPacketPeriodStats.periodTotalTime);
			SQLBIND_DELAYED(query, int, "@skippedPacketsSinceLast", currentPacketPeriodStats.droppedPackets);
			SQLBIND_DELAYED(query, int, "@bytesSinceLast", currentPacketPeriodStats.totalPacketsSize);
			SQLBIND_DELAYED(query, int, "@bytesPerSecond", 1000*currentPacketPeriodStats.totalPacketsSize/ non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@snapshotBytesSinceLast", currentPacketPeriodStats.totalSnapshotSize);
			SQLBIND_DELAYED(query, int, "@snapshotBytesPerSecond", 1000 * currentPacketPeriodStats.totalSnapshotSize / non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@serverCommandBytesSinceLast", currentPacketPeriodStats.totalServerCommandSize);
			SQLBIND_DELAYED(query, int, "@serverCommandBytesPerSecond", 1000 * currentPacketPeriodStats.totalServerCommandSize / non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@packetsSinceLast", currentPacketPeriodStats.totalPacketCount);
			SQLBIND_DELAYED(query, double, "@packetsPerSecond", 1000.0* (double)currentPacketPeriodStats.totalPacketCount/ (double)non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@bytesPerPacket", currentPacketPeriodStats.totalPacketsSize / non0totalPacketCount);
			SQLBIND_DELAYED(query, int, "@periodMaxPacketSize", currentPacketPeriodStats.maxPacketSize);
			SQLBIND_DELAYED(query, int, "@periodMinPacketSize", currentPacketPeriodStats.minPacketSize);

			SQLBIND_DELAYED(query, int, "@nonDeltaSnapsSinceLast", currentPacketPeriodStats.nonDeltaSnapshotCount);
			SQLBIND_DELAYED(query, int, "@nonDeltaSnapsPerSecond", 1000  * currentPacketPeriodStats.nonDeltaSnapshotCount / non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@angleChangesSinceLast", currentPacketPeriodStats.angleChanges);
			SQLBIND_DELAYED(query, int, "@angleChangesPerSecond", 1000 * currentPacketPeriodStats.angleChanges / non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@entitiesSinceLast", currentPacketPeriodStats.entitiesReceivedTotal);
			SQLBIND_DELAYED(query, int, "@entitiesPerSecond", 1000 * currentPacketPeriodStats.entitiesReceivedTotal / non0periodTotalTime);
			SQLBIND_DELAYED(query, int, "@entitiesPerPacket", currentPacketPeriodStats.entitiesReceivedTotal / non0totalPacketCount);

			SQLBIND_DELAYED(query, int, "@demoTime", demoCurrentTime);
			SQLBIND_DELAYED(query, int, "@serverTime", demo.cut.Cl.snap.serverTime);

			SQLBIND_DELAYED_TEXT(query, "@demoName", sharedVars.oldBasename.c_str());
			SQLBIND_DELAYED_TEXT(query, "@demoPath", sharedVars.oldPath.c_str());
			SQLBIND_DELAYED(query, int, "@demoDateTime", sharedVars.oldDemoDateModified);

			io.packetStatsQueries->push_back(queryWrapper);

			lastPacketStatsWritten = demoCurrentTime;
			resetCurrentPacketPeriodStats();
		}

#if DEBUG
		if (oldSize == 0) {
			goto cutcomplete;
		}
#endif
		/*if (demo.cut.Cl.snap.serverTime > endTime) {
			goto cutcomplete;
		}
		else if (framesSaved > 0) {
			// this msg is in range, write it 
			if (framesSaved > Q_max(10, demo.cut.Cl.snap.messageNum - demo.cut.Cl.snap.deltaNum)) {
				demoCutWriteDemoMessage(&oldMsg, newHandle, &demo.cut.Clc);
			}
			else {
				demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qfalse, &demo.cut.Clc, &demo.cut.Cl,demoType);
			}
			framesSaved++;
		}
		else if (demo.cut.Cl.snap.serverTime >= startTime) {
			demoCutWriteDemoHeader(newHandle, &demo.cut.Clc, &demo.cut.Cl,demoType);
			demoCutWriteDeltaSnapshot(firstServerCommand, newHandle, qtrue, &demo.cut.Clc, &demo.cut.Cl,demoType);
			// copy rest
			framesSaved = 1;
		}*/

		Com_Memcpy(&lastFrameInfo, &thisFrameInfo, sizeof(lastFrameInfo));




	}
cutcomplete:
	ret = qtrue;
	/*if (newHandle) {
		buf = -1;
		FS_Write(&buf, 4, newHandle);
		FS_Write(&buf, 4, newHandle);
		ret = qtrue;
	}*/
cuterror:
	//remove previosly converted demo from the same cut
	/*if (newHandle) {
		memset(newName, 0, sizeof(newName));
		if (demo.currentNum > 0) {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s.%d_cut.mme", oldName, demo.currentNum);
		}
		else {
			Com_sprintf(newName, sizeof(newName), "mmedemos/%s_cut.mme", oldName);
		}
		if (FS_FileExists(newName))
			FS_FileErase(newName);
	}*/

	// One last check for unsaved laughs near end of demo. TODO Do we have to do this with some other stuff too? Not sure.
	checkSaveLaughs(demoCurrentTime,bufferTime, lastGameStateChangeInDemoTime, io, &sharedVars.oldBasename, &sharedVars.oldPath, sharedVars.oldDemoDateModified, sourceDemoFile, qtrue, wasDoingSQLiteExecution,opts);


	defragRunInfoFinal_t runInfo;
	while (findFreedomDefragRun<max_clients>("", &runInfo, qtrue, demoCurrentTime, lastGameStateChangeInDemoTime, demoType, CS_PLAYERS_here)) {
		// we might have a few runs that didnt end up getting a ranked result in time before the demo ended. save them as such then.
		SaveDefragRun<max_clients>(runInfo, sharedVars, sourceDemoFile, io, bufferTime, opts, searchMode, wasDoingSQLiteExecution);
	}





	FS_FCloseFile(oldHandle);
	//FS_FCloseFile(newHandle);


	std::cout << "\ndone at demotime "<< demoCurrentTime << " and messsage number "<<demo.cut.Clc.serverMessageSequence << " (old: " << oldSequenceNum << ")." << "\n\n";


 	return ret;
}



qboolean demoHighlightFind(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, const char* outputBatFileKillSprees, const char* outputBatFileDefrag, const char* outputBatFileCaptures, const char* outputBatFileFlagGrabs, const char* outputBatFileLaughs, const char* outputBatFileSpecial, const char* outputBatFileMarks, highlightSearchMode_t searchMode, const ExtraSearchOptions& opts) {
	char			ext[7]{};
	demoType_t		demoType;
	qboolean		isCompressedFile = qfalse;
	demoCutGetDemoType(sourceDemoFile, ext, NULL, &demoType, &isCompressedFile);
	int maxClientsHere = getMAX_CLIENTS(demoType);
	switch (maxClientsHere) {
	case 1: // JK 2 SP
		return demoHighlightFindExceptWrapper<1>(sourceDemoFile, bufferTime, outputBatFile, outputBatFileKillSprees, outputBatFileDefrag, outputBatFileCaptures, outputBatFileFlagGrabs, outputBatFileLaughs, outputBatFileSpecial, outputBatFileMarks, searchMode, opts);
		break;
	case 32:
		return demoHighlightFindExceptWrapper<32>(sourceDemoFile, bufferTime, outputBatFile, outputBatFileKillSprees, outputBatFileDefrag, outputBatFileCaptures, outputBatFileFlagGrabs, outputBatFileLaughs, outputBatFileSpecial, outputBatFileMarks, searchMode, opts);
		break;
	case 64:
		return demoHighlightFindExceptWrapper<64>(sourceDemoFile, bufferTime, outputBatFile, outputBatFileKillSprees, outputBatFileDefrag, outputBatFileCaptures, outputBatFileFlagGrabs, outputBatFileLaughs, outputBatFileSpecial, outputBatFileMarks, searchMode, opts);
		break;
	default:
		throw std::exception("unsupported MAX_CLIENTS count.");
		break;
	}
}
/*void CL_DemoCut_f(void) {
	double startTime, endTime;
	char demoName[MAX_OSPATH];
	if (Cmd_Argc() < 3) {
		Com_Printf("Usage: demoCut start end (in seconds)\n");
		return;
	}
	startTime = atof(Cmd_Argv(1));
	endTime = atof(Cmd_Argv(2));
	if (endTime <= startTime) {
		Com_Printf("invalid range: %.3f >= %.3f\n", startTime, endTime);
		return;
	}
	if (endTime - startTime < 0.05) {
		Com_Printf("invalid range: less than 50 milliseconds is not allowed\n");
		return;
	}
	// convert to msec 
	startTime *= 1000;
	endTime *= 1000;
	Com_sprintf(demoName, MAX_OSPATH, mme_demoFileName->string);
	if (demoCut(demoName, startTime, endTime)) {
		Com_Printf("Demo %s got successfully cut\n", mme_demoFileName->string);
	}
	else {
		Com_Printf("Demo %s has failed to get cut or cut with errors\n", mme_demoFileName->string);
	}
}*/





int main(int argcO, char** argvO) {


	popl::OptionParser op("Allowed options");
	auto h = op.add<popl::Switch>("h", "help", "Show help");
	auto s = op.add<popl::Switch>("s", "only-log-saber-kills", "Only log saber kills (.db as well as .bat)");
	auto S = op.add<popl::Implicit<int>>("S", "only-log-killsprees-with-saberkills", "Only log killsprees that contain at least one saber kill (.db as well as .bat). Optional: Provide number of needed saber kills in killspree.",1);
	auto d = op.add<popl::Implicit<int>>("d", "write-demo-packet-stats", "Write stats about packet size and rate over time. If specified without a value, each packet gets an entry. Otherwise, a number can be provided as the interval in milliseconds to write stats.",0);
	//auto j = op.add<popl::Implicit<int>>("j", "jump-meta-events", "Track jumps as meta events. Optionally, number to limit time around kill that they are tracked in milliseconds.",0);
	auto c = op.add<popl::Switch>("c", "only-log-captures-with-saber-kills", "Only log captures that had at least one saber kill by the flag carrier");
	auto q = op.add<popl::Switch>("q", "quickskip-non-saber-exclusive", "If demo is of a game that allows other weapons than saber/melee/explosives, immediately skip it");
	auto l = op.add<popl::Switch>("l", "long-killstreaks", "Finds very long killstreaks with up to 18 seconds between kills (default is 9 seconds)");
	auto e = op.add<popl::Switch>("e", "entity-to-database", "Writes playerState, entityState and configstring to an sqlite database. (in progress, needs a lot of space)");
	auto n = op.add<popl::Switch>("n", "no-finds-output", "Don't output found highlights in the terminal. Useful for seeing error messages.");
	auto t = op.add<popl::Switch>("t", "test-only", "Don't write anything, only run through the demo for testing.");
	auto A = op.add<popl::Implicit<int>>("A", "teleport-analysis", "Find teleports in demo and make script to speed up time of repeated attempts with teleports putting the player back. Optionally, set to 2 to not only analyze teleports but all locations.",1);
	auto x = op.add<popl::Value<std::string>>("x", "teleport-analysis-endtime", "Provide parameters for teleport analysis as numbers, separated by empty space: demotime of detection end, buffer around fast forward segments post, same thing pre (used so u dont cut off gameplay). Fourth and fifth value is distance to consider 'same place' in mode 2 (horizontal and vertical). Fifth value: minimum amount of time to consider fast-forwarding (excluding buffertimes). Defaults: 0 5000 2000 100 40 10000");
	auto k = op.add<popl::Switch>("k", "kill-sprees-only-visible-kills", "Only find killsprees made up out of visible kills.");
	auto r = op.add<popl::Switch>("r", "reframe-if-needed", "Reframe demos if needed via --reframe parameter to DemoCutter command.");
	auto D = op.add<popl::Switch>("D", "dump-stufftext", "Prints out stufftext commands in the demo as error output for convenient redirecting.");
	auto p = op.add<popl::Switch>("p", "print-debug", "Prints out various debug things, like all configstrings.");
	auto a = op.add<popl::Implicit<int>>("a", "net-analysis", "Debugs some net stuff to understand how mods work. A bit like shownet but without showing all the normal stuff like angles/origin etc, and with support for pers/stats/ammo etc. Pass number for more (atm only mode 1)",1);
	auto y = op.add<popl::Implicit<int>>("y", "player-frames-csv", "Writes CSV files containing position, velocity and viewangle of players. Pass 1 as a value to skip duplicate commandTime values",0);
	auto E = op.add<popl::Implicit<int>>("E", "entity-frames-csv", "Writes CSV files containing trajectory of non-player entities. Pass a number to only do a specific entity number.",-1);
	auto z = op.add<popl::Value<std::string>>("z", "strafe-csv", "Writes CSV files containing different strafes. Pass 2 numbers separated by comma. Sync point (ups speed) and reset point ups (speed). Optionally, a third number for a minimum run length in milliseconds.");
	auto Z = op.add<popl::Switch>("Z", "strafe-csv-interpolate", "Does the strafe CSV with interpolated values instead of leaving them empty when not available at a certain time interval.");
	auto C = op.add<popl::Value<std::string>>("C", "chat-search", "Searches for a string in chats.");
	auto P = op.add<popl::Value<std::string>>("P", "print-search", "Searches for a string in prints.");
	auto T = op.add<popl::Value<std::string>>("T", "text-search", "Searches for a string in general.");
	auto K = op.add<popl::Switch>("K", "skip-kills", "Skips searching for kills (e.g. if you want to only search for chats or defrag runs)");
	auto L = op.add<popl::Implicit<int>>("L", "log-chats", "Writes database with chats. 1 = unique chats database (every unique chat only once). 2 = categorized chats (not currently implemented)", 1);
	auto Q = op.add<popl::Implicit<int>>("Q", "skip-stats", "1 = Avoids creating the statistics db, 2 = avoids strafe deviation calculation. Bitmask. Can combine.",1);
	auto j = op.add<popl::Implicit<int>>("j", "find-jumpbugs", "Finds instances of jumpbugs in demos.",1);
	auto v = op.add<popl::Value<std::string>>("v", "make-video", "Make a little preview video AVI.");
	auto V = op.add<popl::Implicit<int>>("V", "video-maxfps", "Max FPS for video generation, default 1000", 1000);
	auto b = op.add<popl::Value<std::string>>("b", "bsp-directory", "Directory containing bsp files");
	auto f = op.add<popl::Value<std::string>>("f", "filter", "Filter kills/captures/sprees/laughs. Each time this option is specified, a new database is used for the results. Add one with 'rest' to save all the rest. Filters are checked in order. If it fits, it goes in. If not, other filters are checked.\n\tgametype:[gametype]:[gametype2]\n\tmap:[*mapnamepart*]\n\trest (matches anything)");
	auto o = op.add<popl::Implicit<int>>("o", "through-wall", "Properly check whether a kill happened through a wall by tracing from the attacker to the fragged location. Pass 1 as a value to do this only for saber kills.",0);

	
	op.parse(argcO, argvO);
	auto args = op.non_option_args();


	//if (argc < 3) {
	if (args.size() < 2) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds\n";
		std::cout << "third optional argument: myctfreturns, ctfreturns, top10defrag, alldefrag, allkills or allmykills\n";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		std::cin.get();
		return 1;
	}
	else if (h->is_set()) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds\n";
		std::cout << "third optional argument: myctfreturns, ctfreturns, top10defrag, alldefrag, allkills or allmykills\n";
		std::cout << "Extra options:\n";
		std::cout << op << "\n";
		return 0;
	}
	initializeGameInfos();

	//char* demoName = argv[1];
	const char* demoName = args[0].c_str();
	//float bufferTime = atof(argv[2]);
	float bufferTime = atof(args[1].c_str());

	ExtraSearchOptions opts;
	opts.entityDataToDb = e->is_set();
	opts.onlyLogSaberKills = s->is_set();
	opts.onlyLogKillSpreesWithSaberKills = S->is_set() ? S->value() : 0;
	opts.netAnalysisMode = a->is_set() ? a->value() : 0;
	opts.jumpMetaEventsLimit = bufferTime;//j->is_set() ? (j->value() == 0 ? bufferTime : j->value()) : 0;
	opts.writeDemoPacketStats = d->is_set() ? (d->value() <= 0 ? -1 : d->value()) : 0;
	opts.onlyLogCapturesWithSaberKills = c->is_set();
	opts.quickSkipNonSaberExclusive =  q->value();
	opts.findSuperSlowKillStreaks = l->value();
	opts.noFindOutput = n->value();
	opts.testOnly = t->value();
	opts.killSpreesOnlyVisible = k->value();
	opts.reframeIfNeeded = r->value();
	opts.dumpStufftext = D->value();
	opts.printDebug = p->value();
	opts.strafeCSVInterpolate = Z->value();
	opts.playerCSVDump = y->is_set();
	opts.findjumpbugs = j->is_set();
	opts.entityCSVDump = !E->is_set() ? 0 : E->value();
	opts.skipKills = K->is_set();
	opts.teleportAnalysis = A->is_set() ? A->value() : 0;
	opts.writeChatsUnique = L->is_set() ? (L->value() & 1) : false;
	opts.writeChatsCategorized = L->is_set() ? (L->value() & 2) : false;
	opts.doStatsDb = Q->is_set() ? !(Q->value() & 1) : true;
	opts.doStrafeDeviation = Q->is_set() ? !(Q->value() & 2) : true;
	if (x->is_set()) {
		int v[6];
		int countfound = sscanf(x->value().c_str(),"%d %d %d %d %d %d",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5]);
		if (countfound > 0) {

			opts.teleportAnalysisEndDemoTime = v[0];
		}
		if (countfound > 1) {

			opts.teleportAnalysisBufferTimeFuture = v[1];
		}
		if (countfound > 2) {

			opts.teleportAnalysisBufferTimePast = v[2];
		}
		if (countfound > 3) {

			opts.teleportAnalysisMaxDistanceHorizontal = v[3];
		}
		if (countfound > 4) {

			opts.teleportAnalysisMaxDistanceVertical = v[4];
		}
		if (countfound > 5) {

			opts.teleportAnalysisMinTimeFastForward = v[5];
		}
	}
	opts.playerCSVDumpCommandTimeDupeSkip = y->is_set() ? y->value() == 1 : false;

	opts.throughWallChecks = o->is_set() ? (o->value() == 0 ? 1 : 2)  : 0;

	if (C->is_set()) {
		opts.doChatSearch = true;
		opts.chatSearch = C->value();
	}
	if (P->is_set()) {
		opts.doPrintSearch = true;
		opts.printSearch = P->value();
	}
	if (T->is_set()) {
		opts.doStringSearch = true;
		opts.stringSearch = T->value();
	}
	if (v->is_set()) {
		opts.makeVideo = true;
		opts.videoPath = v->value();
		opts.videoMinMsec = 1000 / V->value();
	}
	if (b->is_set()) {
		for (int i = 0; i < b->count(); i++) {
			std::string dir = b->value(i);
			dir = dir.erase(dir.find_last_not_of("/\\\"") + 1); // " also because uh ... someone might do -b "path\" .. xd
			opts.bspDirectories.push_back(dir);
		}
	}
	if (f->is_set()) {
		for (int i = 0; i < f->count(); i++) {
			std::string filterstring = f->value(i);
			std::vector<std::string> filterparts = splitString(filterstring, ":", true, false);
			ResultFilter newFilter;
			bool isvalid = false;
			if (filterparts.size() == 1) {
				const char* part1 = filterparts[0].c_str();
				if (!_stricmp(part1, "all") || !_stricmp(part1, "rest") || !_stricmp(part1, "*")) {
					newFilter.type = ResultFilter::Type::FILTER_ALLGOES;
					isvalid = true;
				}
			}
			else if(filterparts.size() > 1) {
				const char* part1 = filterparts[0].c_str();
				if (!_stricmp(part1,"gametype")) {
					newFilter.type = ResultFilter::Type::FILTER_GAMETYPE;
					int64_t gameTypeBitMask = 0;
					for (int j = 1; j < filterparts.size(); j++) {
						const char* gametype = filterparts[j].c_str();
						if (!_stricmp(gametype, "ffa"))				gameTypeBitMask |= (1<<GT_FFA_GENERAL);
						else if (!_stricmp(gametype, "holocron"))	gameTypeBitMask |= (1 << GT_HOLOCRON_GENERAL);
						else if (!_stricmp(gametype, "jedimaster"))	gameTypeBitMask |= (1 << GT_JEDIMASTER_GENERAL);
						else if (!_stricmp(gametype, "duel"))		gameTypeBitMask |= (1 << GT_TOURNAMENT_GENERAL);
						else if (!_stricmp(gametype, "powerduel"))	gameTypeBitMask |= (1 << GT_POWERDUEL_GENERAL);
						//else if (_stricmp(gametype, "sp"))			gameTypeBitMask |= GT_CTF_GENERAL;
						else if (!_stricmp(gametype, "tffa"))		gameTypeBitMask |= (1 << GT_TEAM_GENERAL);
						else if (!_stricmp(gametype, "siege"))		gameTypeBitMask |= (1 << GT_SAGA_SIEGE_GENERAL);
						else if (!_stricmp(gametype, "cty"))		gameTypeBitMask |= (1 << GT_CTY_GENERAL);
						else if (!_stricmp(gametype, "ctf"))		gameTypeBitMask |= (1 << GT_CTF_GENERAL);
						else if (!_stricmp(gametype, "1flagctf"))	gameTypeBitMask |= (1 << GT_1FCTF_GENERAL);
						else if (!_stricmp(gametype, "obelisk"))	gameTypeBitMask |= (1 << GT_OBELISK_GENERAL);
						else if (!_stricmp(gametype, "harvester"))	gameTypeBitMask |= (1 << GT_HARVESTER_GENERAL);
						else if (!_stricmp(gametype, "teamrounds"))	gameTypeBitMask |= (1 << GT_TEAM_ROUNDS_GENERAL);
						else if (!_stricmp(gametype, "objective"))	gameTypeBitMask |= (1 << GT_OBJECTIVE_GENERAL);
						else if (!_stricmp(gametype, "tow"))		gameTypeBitMask |= (1 << GT_TOW_GENERAL);
						else if (!_stricmp(gametype, "liberation"))	gameTypeBitMask |= (1 << GT_LIBERATION_GENERAL);
						else {
							std::cerr << "Error: gametype filter "<< gametype<<" not recognized\n";
#ifdef DEBUG
							std::cin.get();
#endif
							return 1;
						}
					}
					if (gameTypeBitMask) {
						isvalid = true;
						newFilter.matchValue = gameTypeBitMask;
					}
				}
				else if (!_stricmp(part1,"map") || !_stricmp(part1, "mapname")) {
					newFilter.type = ResultFilter::Type::FILTER_MAP;
					const char* mapnameFilter = filterparts[1].c_str();
					// TODO regex someday?
					int startindex = 0, count = filterparts[1].size();
					if (*mapnameFilter == '*') {
						newFilter.matchStart = false;
						startindex++;
						count--;
						mapnameFilter++;
					}
					if (mapnameFilter[count-1] == '*') {
						newFilter.matchEnd = false;
						count--;
					}
					newFilter.matchString = filterparts[1].substr(startindex,count);

					if (newFilter.matchString.size()) {
						isvalid = true;
					}
				}
			}
			if (!isvalid) {
				std::cerr << "Error: invalid filters\n";
#ifdef DEBUG
				std::cin.get();
#endif
				return 1;
			}
			opts.filters.push_back(newFilter);
		}
		if (opts.filters.size() > 1) {
			opts.killDbsCount = opts.filters.size();
		}
	}

	if (z->is_set()) {
		std::string values = z->value();
		size_t firstCommaPos = values.find(",");
		if (firstCommaPos != std::string::npos) {
			std::string number1String = values.substr(0,firstCommaPos);
			// "12,32" -> firstCommaPos is 2. so desired offset is 3. size() is 5, 5-2 is 3. -1 is 2.
			std::string number2String = values.substr(firstCommaPos+1,values.size()-firstCommaPos-1);
			size_t secondCommaPos = number2String.find(",");
			std::string number3String = "";
			if (secondCommaPos != std::string::npos) {
				number3String = number2String.substr(secondCommaPos + 1, number2String.size() - secondCommaPos - 1);
				number2String = number2String.substr(0,secondCommaPos);
			}
			int num1 = atoi(number1String.c_str());
			int num2 = atoi(number2String.c_str());
			int num3 = atoi(number3String.c_str());
			opts.strafeCSVSyncPoint = num1;
			opts.strafeCSVResetPoint = num2;
			opts.strafeCSVMinRunDuration = num3;
			std::cout << "Doing strafe CSV output with sync point " << num1 << ", reset point " << num2 << " and min run duration " << num3 << "\n";
		}
		else {
			std::cerr << "ERROR: Value supplied for -z/--strafe-csv option must be at least 2 ups speed numbers delimited by a comma. One for sync point, other for reset point. Optionally, a third number for a minimum run length in milliseconds. (" << DPrintFLocation << ")\n";
#ifdef DEBUG
			std::cin.get();
#endif
			return 1;
		}
	}

	GlobalDebugOutputFlags = 0;
	if (opts.printDebug) GlobalDebugOutputFlags |= (1 << DEBUG_COMMANDS) | (1 << DEBUG_CONFIGSTRING) | (1 << DEBUG_HIDDENUSERCMD); // TODO Make this more flexible? Able to specify types of debug output? Merge the stufftext stuff into it too.
	if (opts.netAnalysisMode == 1) GlobalDebugOutputFlags |= (1 << DEBUG_NETANALYSIS1);

	highlightSearchMode_t searchMode = SEARCH_INTERESTING;
	//if (argc > 3) {
	if (args.size() > 2) {
		// Searchmode specified
		//char* searchModeText = argv[3];
		const char* searchModeText = args[2].c_str();
		if (!_stricmp(searchModeText, "myctfreturns")) {
			searchMode = SEARCH_MY_CTF_RETURNS;
		}else if (!_stricmp(searchModeText, "ctfreturns")) {
			searchMode = SEARCH_CTF_RETURNS;
		}else if (!_stricmp(searchModeText, "top10defrag")) {
			searchMode = SEARCH_TOP10_DEFRAG;
		}else if (!_stricmp(searchModeText, "alldefrag")) {
			searchMode = SEARCH_ALL_DEFRAG;
		}else if (!_stricmp(searchModeText, "allkills")) {
			searchMode = SEARCH_ALL_KILLS;
		}else if (!_stricmp(searchModeText, "allmykills")) {
			searchMode = SEARCH_ALL_MY_KILLS;
		}
	}

	Com_Printf("Looking at %s.\n", demoName); 
	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	DPrintFLocation = demoName;
	if (demoHighlightFind(demoName, bufferTime,"highlightExtractionScript.bat","highlightExtractionScriptKillSprees.bat","highlightExtractionScriptDefrag.bat","highlightExtractionScriptCaptures.bat","highlightExtractionScriptFlagGrabs.bat","highlightExtractionScriptLaughs.bat","highlightExtractionScriptSpecial.bat","highlightExtractionScriptMarks.bat", searchMode, opts)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Highlights successfully found in %.5f seconds.\n",seconds);
	}
	else {
		Com_DPrintf("Finding highlights in demo %s has resulted in errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif