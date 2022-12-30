#include "demoCut.h"
#define PCRE2_STATIC
#include "jpcre2.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>
#include "sqlite3.h"
#include <set>
#include "../shared/boostTupleHash.hpp"
#include <unordered_map>

#include <iso646.h>
#include "picosha3.h"

#define DEBUGSTATSDB



#define PLAYERSTATEOTHERKILLERBOOSTDETECTION
#undef PLAYERSTATEOTHERKILLERBOOSTDETECTION // I disabled this because while it theoretically could work, it won't work because the otherKiller value of the playerstate is not actually transmitted. So useless.


typedef jpcre2::select<char> jp;
//jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.)raw", "mSi");
// OC Defrag
jp::Regex defragRecordFinishRegex(R"raw(\^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+):(\d+.\d+)\^7\]( which is his personal best time)?.( \^2Top10 time!\^7)? Difference to best: \[((\^200:00.000\^7)|(\^2(\d+):(\d+.\d+)\^7))\]\.)raw", "mSi");

// Razor Defrag (different server)
jp::Regex defragRazorFinishRegex(R"raw(\^\d:\[\s*\^7(.*?)\s\^7(finished in|(beat the WORLD RECORD and )?(is now ranked) \^\d#(\d) \^7with)\s\^3(\d+):(\d+))raw", "mSi");
jp::Regex defragRazorPersonalBestRegex(R"raw(\^\d:\[\s*\^7New personal record on this map!\s*\^\d\]:)raw", "mSi");

class defragRunInfo_t {
public:
	int milliseconds = 0;
	std::string playerName;
	qboolean isNumber1 = qfalse;
	qboolean isTop10 = qfalse;
	qboolean isLogged = qfalse;
	qboolean isPersonalBest = qfalse;
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
	int saberMove;
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

LastSaberMoveInfo playerLastSaberMove[MAX_CLIENTS_MAX];
int playerFirstVisible[MAX_CLIENTS_MAX];
int playerFirstFollowed[MAX_CLIENTS_MAX];
int playerFirstFollowedOrVisible[MAX_CLIENTS_MAX];
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
std::map<int,std::string> lastPlayerModel;
int lastKnownRedFlagCarrier = -1;
int lastKnownBlueFlagCarrier = -1;
strafeDeviationInfo_t strafeDeviationsDefrag[MAX_CLIENTS_MAX];

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

playerDemoStats_t* playerDemoStatsPointers[MAX_CLIENTS_MAX];

struct hitDetectionData_t {
	qboolean confirmedHit;
	qboolean painDetected;
	qboolean nearbySaberHitDetected;
	qboolean newParryDetected;
};

hitDetectionData_t hitDetectionData[MAX_CLIENTS_MAX];

// Tries to find all sorts of laughter in chat, but tries to exclude non-exuberant types (like a simple lol), and focus on big letter LOL, big letter XD, rofl, wtf etc and some misspelled variants.
jp::Regex regexLaugh(R"raw(\x19:\s*(r+[oi]+[tf]+[kl]+|[op]+[mn]+[ghf]+|[lk]+[mn]+[fg]*a+[okli]+|a?ha[ha]{2,}|w+[rt]+[gf]+|(?-i)X+D+|L+O{1,100}L+(?i)))raw", "mSi");
#define MAX_LAUGH_DELAY 7000 // From first laugh to last laugh, max delay.
#define LAUGHS_CUT_PRE_TIME 10000 // Upon first laugh, cut last 10 seconds so we see context.
int firstLaugh = -1;
int lastLaugh = -1;
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


struct cgs{
	int redflag, blueflag, yellowflag;
	int redFlagLastChange, blueFlagLastChange, yellowflagLastChange;
	int redFlagLastPickupOrigin, blueFlagLastPickupOrigin, yellowflagLastPickupOrigin;
	int redFlagLastChangeToTaken, blueFlagLastChangeToTaken, yellowflagLastChangeToTaken;
} cgs;


#ifdef DEBUGSTATSDB
typedef std::tuple<int, int, int, int, int,int> animStanceKey; // demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance
std::map< animStanceKey, int> animStanceCounts;
#endif


#define SQLBIND(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
#define SQLBIND_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
#define SQLBIND_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)

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
	bool isDoom;
	bool isExplosion;
	bool isVisible;
	bool isFollowed;
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
	int lastKillTime;
	int countKills = 0;
	int countRets = 0;
	int countDooms = 0;
	int countExplosions = 0;
	int countThirdPersons = 0; // Not followed ones.
	float maxVictimSpeed;
};

// For detecting killstreaks
// Killer is the key, kill info is the value
std::map<int, std::vector<Kill>> kills;
std::map<int, int> timeCheckedForKillStreaks;

#define KILLSTREAK_MIN_KILLS 3
#define KILLSTREAK_MAX_INTERVAL 3000
#define SLOW_KILLSTREAK_MAX_INTERVAL 5000
#define VERYSLOW_KILLSTREAK_MAX_INTERVAL 7000
#define OLDER_SPEEDS_STORE_LIMIT 2000 // Any speeds older than 2000ms are removed.
#define MAX_ASSUMED_SERVER_FPS 200
#define MAX_NEEDED_PAST_SPEED_SAMPLES (OLDER_SPEEDS_STORE_LIMIT*MAX_ASSUMED_SERVER_FPS/1000)


enum trackedEntityType_t {
	TET_NONE,
	TET_TRIPMINE
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


struct frameInfo_t {
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
	qboolean pmFlagKnockback[MAX_CLIENTS_MAX];
	qboolean psTeleportBit[MAX_CLIENTS_MAX];
	int pmFlagTime[MAX_CLIENTS_MAX];
	int commandTime[MAX_CLIENTS_MAX];
	int legsAnim[MAX_CLIENTS_MAX];
	int torsoAnim[MAX_CLIENTS_MAX];
	int groundEntityNum[MAX_CLIENTS_MAX];
}; 

frameInfo_t lastFrameInfo;

frameInfo_t thisFrameInfo;


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

//int64_t walkDetectedTime[max_clients];
std::vector<int64_t> walkDetectedTimes[MAX_CLIENTS_MAX];


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
	SEARCH_TOP10_DEFRAG, // Top10 Defrags, even if not number 1.
	SEARCH_ALL_DEFRAG, // All defrags, even if not top 10
};


// Most of this code is from cl_demos_cut.cpp from jomma/jamme
//

demo_t			demo;



template<unsigned int max_clients>
void updatePlayerDemoStatsArrayPointers(demoType_t demoType) {
	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
	const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
	std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	for (int i = 0; i < max_clients; i++) {
		stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + i];
		const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string playerName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "n");

		playerDemoStatsMapKey_t mapPointer(mapname, playerName, i); // We can think of this as something of a soft (not really hard enforced in database) primary key to distinguish players.
		playerDemoStatsPointers[i] = &playerDemoStatsMap[mapPointer];
	}
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
		std::string secondString = vec_num[matchNum][3];
		float seconds = atof(vec_num[matchNum][3].c_str());
		int milliSeconds = (1000.0f * seconds) + 0.5f;
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


void updateForcePowersInfo(clientActive_t* clCut) {
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

int demoCutGetEvent(entityState_t* es,int demoCurrentTime) {
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
	if (lastEventTime[es->number] < demoCurrentTime - EVENT_VALID_MSEC) {
		lastEvent[es->number] = 0;
	}

	int eventNumberRaw = es->eType > ET_EVENTS ? es->eType - ET_EVENTS : es->event;
	int eventNumber = eventNumberRaw & ~EV_EVENT_BITS;

	lastEventTime[es->number] = demoCurrentTime;
	if (eventNumberRaw == lastEvent[es->number]) {
		return 0;
	}

	lastEvent[es->number] = eventNumberRaw;
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


float getMaxSpeedForClientinTimeFrame(int clientNum, int fromTime, int toTime) {
	float maxSpeed = -1.0f;
	/*for (auto it = speeds[clientNum].begin(); it != speeds[clientNum].end(); it++) {
		if (it->first >= fromTime && it->first <= toTime && it->second > maxSpeed) {
			maxSpeed = it->second;
		}
	}*/
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && speeds[clientNum][i].speed > maxSpeed) {
			maxSpeed = speeds[clientNum][i].speed;
		}
	}
	return maxSpeed;
}


float getMaxAngularSpeedForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	float maxSpeed = -1.0f;
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularSpeed) > maxSpeed) {
			maxSpeed = abs(speeds[clientNum][i].angularSpeed);
		}
	}
	return maxSpeed;
}


float getMaxAngularAccelerationForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	float maxAccel = -1.0f;
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularAcceleration) > maxAccel) {
			maxAccel = abs(speeds[clientNum][i].angularAcceleration);
		}
	}
	return maxAccel;
}


float getMaxAngularJerkForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	float maxJerk = -1.0f;
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularJerk) > maxJerk) {
			maxJerk = abs(speeds[clientNum][i].angularJerk);
		}
	}
	return maxJerk;
}


float getMaxAngularSnapForClientinTimeFrame(int clientNum, int64_t fromTime, int64_t toTime) {
	float maxSnap = -1.0f;
	for (int i = 0;i< speeds[clientNum].size(); i++) {
		if (speeds[clientNum][i].time >= fromTime && speeds[clientNum][i].time <= toTime && abs(speeds[clientNum][i].angularSnap) > maxSnap) {
			maxSnap = abs(speeds[clientNum][i].angularSnap);
		}
	}
	return maxSnap;
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
void CheckForNameChanges(clientActive_t* clCut,sqlite3* killDb,sqlite3_stmt* insertPlayerModelStatement,sqlite3_stmt* updatePlayerModelCountStatement, demoType_t demoType) {


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
			modelsToAdd.push_back(modelName);
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
		SQLBIND_TEXT(insertPlayerModelStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(updatePlayerModelCountStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(insertPlayerModelStatement, "@baseModel", baseModel.c_str());
		SQLBIND_TEXT(updatePlayerModelCountStatement, "@baseModel", baseModel.c_str());
		if (variantExists) {
			SQLBIND_TEXT(insertPlayerModelStatement, "@variant", variant.c_str());
			SQLBIND_TEXT(updatePlayerModelCountStatement, "@variant", variant.c_str());
		}

		int queryResult = sqlite3_step(insertPlayerModelStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting player model into database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(insertPlayerModelStatement);
		queryResult = sqlite3_step(updatePlayerModelCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error updating player model count in database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(updatePlayerModelCountStatement);
	}

}

template<unsigned int max_clients>
void CheckSaveKillstreak(int maxDelay,SpreeInfo* spreeInfo,int clientNumAttacker, std::vector<Kill>* killsOfThisSpree,std::vector<int>* victims,std::vector<std::string>* killHashes,std::string allKillsHashString, int demoCurrentTime, std::ofstream* outputBatHandleKillSprees, int bufferTime,int lastGameStateChangeInDemoTime, const char* sourceDemoFile,sqlite3_stmt* insertSpreeStatement,sqlite3* killDb,std::string oldBasename,std::string oldPath,time_t oldDemoDateModified, demoType_t demoType) {

	int CS_PLAYERS_here = getCS_PLAYERS(demoType);
	if (spreeInfo->countKills >= KILLSTREAK_MIN_KILLS) {
		int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
		const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
		std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
		std::string playername = "WEIRDATTACKER";

		if (clientNumAttacker >= 0 && clientNumAttacker < max_clients) {

			stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNumAttacker];
			const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "n");
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
		std::set<int> nearbyPlayers;
		for (int i = 0; i < killsOfThisSpree->size(); i++) {
			//stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + (*victims)[i]];
			//const char* victimInfo = demo.cut.Cl.gameState.stringData + stringOffset;
			victimsSS << (*killsOfThisSpree)[i].targetClientNum/*<< (*victims)[i]*/ << ": " << (*killsOfThisSpree)[i].victimName << " ("<< (*killsOfThisSpree)[i].modInfoString<<", +"<< (i>0? ((*killsOfThisSpree)[i].time-lastKillTime) :0)<<")" << "\n";
			victimsStrippedSS << (*killsOfThisSpree)[i].targetClientNum/*<< (*victims)[i]*/ << ": " << Q_StripColorAll((*killsOfThisSpree)[i].victimName) << " ("<< (*killsOfThisSpree)[i].modInfoString<<", +"<< (i>0? ((*killsOfThisSpree)[i].time-lastKillTime) :0)<<")" << "\n";
			lastKillTime = (*killsOfThisSpree)[i].time;
			victimsNumsSS << (i==0? "" :",") << (*victims)[i];
			for (int n = 0; n < (*killsOfThisSpree)[i].nearbyPlayers.size(); n++) {
				nearbyPlayers.insert((*killsOfThisSpree)[i].nearbyPlayers[n].clientNum);
			}
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


		std::stringstream killHashesSS;
		for (int i = 0; i < killHashes->size(); i++) {
			killHashesSS << (i == 0 ? "" : "\n") << (*killHashes)[i];
		}
		std::string killHashesString = killHashesSS.str();

		//hashss << playername << "_" << victimname << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
		auto sha3_512 = picosha3::get_sha3_generator<224>();
		std::string hash_hex_string = sha3_512.get_hex_string(allKillsHashString);
		std::string shorthash = hash_hex_string.substr(0, 3);


		int maxSpeedAttacker = getMaxSpeedForClientinTimeFrame(clientNumAttacker, spreeInfo->lastKillTime - spreeInfo->totalTime - 1000, spreeInfo->lastKillTime);
		int maxSpeedVictims = spreeInfo->maxVictimSpeed;

		// Log the kill.
		SQLBIND_TEXT(insertSpreeStatement, "@hash", hash_hex_string.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@shorthash", shorthash.c_str());
		SQLBIND(insertSpreeStatement, int, "@maxDelay", maxDelay);
		SQLBIND_TEXT(insertSpreeStatement, "@map", mapname.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@killerName", playername.c_str());
		std::string playernameStripped = Q_StripColorAll(playername);
		SQLBIND_TEXT(insertSpreeStatement, "@killerNameStripped", playernameStripped.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@victimNames", victimsString.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@victimNamesStripped", victimsStringStripped.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@killHashes", killHashesString.c_str());
		SQLBIND(insertSpreeStatement, int, "@killerClientNum", clientNumAttacker);
		SQLBIND_TEXT(insertSpreeStatement, "@victimClientNums", victimsNumsString.c_str());
		SQLBIND(insertSpreeStatement, int, "@countKills", spreeInfo->countKills);
		SQLBIND(insertSpreeStatement, int, "@countRets", spreeInfo->countRets);
		SQLBIND(insertSpreeStatement, int, "@countDooms", spreeInfo->countDooms);
		SQLBIND(insertSpreeStatement, int, "@countExplosions", spreeInfo->countExplosions);
		SQLBIND(insertSpreeStatement, int, "@countThirdPersons", spreeInfo->countThirdPersons);

		SQLBIND_TEXT(insertSpreeStatement, "@nearbyPlayers", nearbyPlayers.size() > 0 ? nearbyPlayersString.c_str() : NULL);
		SQLBIND(insertSpreeStatement, int, "@nearbyPlayerCount", nearbyPlayers.size());

		SQLBIND(insertSpreeStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
		SQLBIND(insertSpreeStatement, int, "@maxSpeedAttacker", maxSpeedAttacker);
		SQLBIND(insertSpreeStatement, int, "@maxSpeedTargets", spreeInfo->maxVictimSpeed);
		SQLBIND_TEXT(insertSpreeStatement, "@demoName", oldBasename.c_str());
		SQLBIND_TEXT(insertSpreeStatement, "@demoPath", oldPath.c_str());
		SQLBIND(insertSpreeStatement, int, "@demoTime", spreeInfo->lastKillTime - spreeInfo->totalTime);
		SQLBIND(insertSpreeStatement, int, "@duration", spreeInfo->totalTime);
		SQLBIND(insertSpreeStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
		SQLBIND(insertSpreeStatement, int, "@demoDateTime", oldDemoDateModified);

		int queryResult = sqlite3_step(insertSpreeStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting killing spree into database: " << sqlite3_errmsg(killDb) << "\n";
		}
		sqlite3_reset(insertSpreeStatement);


		int startTime = spreeInfo->lastKillTime-spreeInfo->totalTime - bufferTime;
		int endTime = spreeInfo->lastKillTime + bufferTime;
		int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
		bool isTruncated = false;
		int truncationOffset = 0;
		if (earliestPossibleStart > startTime) {
			truncationOffset = earliestPossibleStart - startTime;
			startTime = earliestPossibleStart;
			isTruncated = true;
		}



		std::stringstream ss;
		ss << mapname << std::setfill('0') << "___KILLSPREE" <<maxDelay<<"_"<< spreeInfo->countKills << (spreeInfo->countRets ? va("R%d", spreeInfo->countRets) : "") << (spreeInfo->countDooms ? va("D%d", spreeInfo->countDooms) : "") << (spreeInfo->countExplosions ? va("E%d", spreeInfo->countExplosions) : "") << "___" << playername << "__";
		for (int i = 0; i < victims->size(); i++) {
			ss << "_" << (*victims)[i];
		}
		ss << "___" << maxSpeedAttacker <<"_"<< maxSpeedVictims << "ups" << (spreeInfo->countThirdPersons ? va("___thirdperson%d", spreeInfo->countThirdPersons) : "") << "___" << clientNumAttacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "") << "_" << shorthash;

		std::string targetFilename = ss.str();
		char* targetFilenameFiltered = new char[targetFilename.length() + 1];
		sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


		(*outputBatHandleKillSprees) << "\nrem hash: " << hash_hex_string;
		(*outputBatHandleKillSprees) << "\nrem demoCurrentTime: " << demoCurrentTime;
		(*outputBatHandleKillSprees) << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
		delete[] targetFilenameFiltered;
		//std::cout << mapname << " " << modInfo.str() << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "\n";
		std::cout << ss.str() << "\n";

		//timeCheckedForKillStreaks[clientNumAttacker] = spreeInfo->lastKillTime;
	}

}

void checkSaveLaughs(int demoCurrentTime, int bufferTime, int lastGameStateChangeInDemoTime, std::ofstream* outputBatHandleLaughs, sqlite3* killDb, sqlite3_stmt* insertLaughsStatement, sqlite3_stmt* selectLastInsertRowIdStatement, std::string* oldBasename, std::string* oldPath,int oldDemoDateModified, const char* sourceDemoFile,  qboolean force) {
	if (firstLaugh != -1  && (demoCurrentTime - firstLaugh > MAX_LAUGH_DELAY || force) && laughCount > 0) {
		
		if (laughCount > 1) { // Let's not bloat the database with single laughs. Could miss some stuff but oh well.

			int duration = lastLaugh - firstLaugh;

			// Aye, let's log it.
			int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
			const char* info = demo.cut.Cl.gameState.stringData + stringOffset;
			std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "mapname");
			std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");
			SQLBIND_TEXT(insertLaughsStatement, "@map", mapname.c_str());
			SQLBIND_TEXT(insertLaughsStatement, "@serverName", serverName.c_str());
			std::string serverNameStripped = Q_StripColorAll(serverName);
			SQLBIND_TEXT(insertLaughsStatement, "@serverNameStripped", serverNameStripped.c_str());
			std::string laughsString = laughs.str();
			std::string laughsChatlogString = laughsChatlog.str();
			SQLBIND_TEXT(insertLaughsStatement, "@laughs", laughsString.c_str());
			SQLBIND_TEXT(insertLaughsStatement, "@chatlog", laughsChatlogString.c_str());
			std::string laughsChatlogStringStripped = Q_StripColorAll(laughsChatlogString);
			SQLBIND_TEXT(insertLaughsStatement, "@chatlogStripped", laughsChatlogStringStripped.c_str());
			SQLBIND(insertLaughsStatement, int, "@laughCount", laughCount);
			SQLBIND_TEXT(insertLaughsStatement, "@demoName", (*oldBasename).c_str());
			SQLBIND_TEXT(insertLaughsStatement, "@demoPath", (*oldPath).c_str());
			SQLBIND(insertLaughsStatement, int, "@duration", duration);
			SQLBIND(insertLaughsStatement, int, "@demoTime", firstLaugh);
			SQLBIND(insertLaughsStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
			SQLBIND(insertLaughsStatement, int, "@demoDateTime", oldDemoDateModified);
			SQLBIND(insertLaughsStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);

			int queryResult = sqlite3_step(insertLaughsStatement);
			uint64_t insertedId = -1;
			if (queryResult != SQLITE_DONE) {
				std::cout << "Error inserting laugh spree into database: " << sqlite3_errmsg(killDb) << "\n";
			}
			else {
				queryResult = sqlite3_step(selectLastInsertRowIdStatement);
				if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
					std::cout << "Error retrieving inserted laughs id from database: " << sqlite3_errmsg(killDb) << "\n";
				}
				else {
					insertedId = sqlite3_column_int64(selectLastInsertRowIdStatement, 0);
				}
				sqlite3_reset(selectLastInsertRowIdStatement);
			}
			sqlite3_reset(insertLaughsStatement);

			int startTime = firstLaugh - LAUGHS_CUT_PRE_TIME - bufferTime;
			int endTime = lastLaugh + bufferTime;
			int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
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

			(*outputBatHandleLaughs) << "\nrem demoCurrentTime: " << demoCurrentTime;
			(*outputBatHandleLaughs) << "\nrem insertid" << insertedId;
			(*outputBatHandleLaughs) << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
			delete[] targetFilenameFiltered;
			std::cout << targetFilename << "\n";
		}

		resetLaughs(); // Reset again so we can track future laugh sequences.
	}
}



template<unsigned int max_clients>
qboolean demoHighlightFindReal(const char* sourceDemoFile, int bufferTime, const char* outputBatFile,const char* outputBatFileKillSprees, const char* outputBatFileDefrag,const char* outputBatFileCaptures,const char* outputBatFileLaughs, highlightSearchMode_t searchMode) {
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
	int				demoBaseTime = 0; // Fixed offset in demo time (due to servertime resets)
	int64_t			demoCurrentTime = 0;
	int				demoOldTime = 0;
	int				deltaTimeFromLastSnapshot = 0;
	int				lastGameStateChange = 0;
	int				lastGameStateChangeInDemoTime = 0;
	int				lastKnownTime = 0;
	qboolean		isCompressedFile = qfalse;

	std::ofstream outputBatHandle;
	std::ofstream outputBatHandleKillSprees;
	std::ofstream outputBatHandleDefrag;
	std::ofstream outputBatHandleCaptures;
	std::ofstream outputBatHandleLaughs;

	outputBatHandle.open(outputBatFile, std::ios_base::app); // append instead of overwrite
	outputBatHandleKillSprees.open(outputBatFileKillSprees, std::ios_base::app); // append instead of overwrite
	outputBatHandleDefrag.open(outputBatFileDefrag, std::ios_base::app); // append instead of overwrite
	outputBatHandleCaptures.open(outputBatFileCaptures, std::ios_base::app); // append instead of overwrite
	outputBatHandleLaughs.open(outputBatFileLaughs, std::ios_base::app); // append instead of overwrite

	Com_Memset(playerDemoStatsPointers,0,sizeof(playerDemoStatsPointers));
	Com_Memset(playerVisibleFrames,0,sizeof(playerVisibleFrames));
	Com_Memset(playerVisibleClientFrames,0,sizeof(playerVisibleClientFrames));
	Com_Memset(playerFirstVisible,0,sizeof(playerFirstVisible));
	Com_Memset(playerFirstFollowed,0,sizeof(playerFirstFollowed));
	Com_Memset(playerFirstFollowedOrVisible,0,sizeof(playerFirstFollowedOrVisible));
	Com_Memset(lastEvent,0,sizeof(lastEvent));
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

	//Com_Memset(lastBackflip, 0, sizeof(lastBackflip));
	for (int i = 0; i < max_clients; i++) {
		lastBackflip[i] = -1;
	}
	//for (int i = 0; i < max_clients; i++) {
	//	walkDetectedTime[i] = -1;
	//}
	Com_Memset(&cgs,0,sizeof(cgs));
	resetLaughs();

	sqlite3* killDb;
	sqlite3_open("killDatabase.db",&killDb);
	/*sqlite3_exec(killDb, "CREATE TABLE kills ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"victimName	TEXT NOT NULL,"
		"killerClientNum	INTEGER NOT NULL,"
		"victimClientNum	INTEGER NOT NULL,"
		"isReturn	BOOLEAN NOT NULL,"
		"isDoomKill	BOOLEAN NOT NULL,"
		"isExplosion	BOOLEAN NOT NULL,"
		"isSuicide	BOOLEAN NOT NULL,"
		"targetIsVisible	BOOLEAN NOT NULL,"
		"attackerIsVisible	BOOLEAN NOT NULL,"
		"isFollowed	BOOLEAN NOT NULL,"
		"meansOfDeath	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"maxSpeedAttacker	REAL,"
		"maxSpeedTarget	REAL,"
		"meansOfDeathString	TEXT NOT NULL,"
		"probableKillingWeapon	INTEGER NOT NULL,"
		"positionX	REAL,"
		"positionY	REAL,"
		"positionZ	REAL,"
		"demoName TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL,"
		"PRIMARY KEY(hash)"
		"); ",
		NULL,NULL,NULL);*/
	sqlite3_exec(killDb, "CREATE TABLE kills ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"serverNameStripped	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"killerNameStripped	TEXT NOT NULL,"
		"victimName	TEXT NOT NULL,"
		"victimNameStripped	TEXT NOT NULL,"
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

	sqlite3_exec(killDb, "CREATE TABLE killAngles ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"map	TEXT NOT NULL,"
		"killerIsFlagCarrier	BOOLEAN NOT NULL,"
		"isReturn	BOOLEAN NOT NULL,"
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
		"directionX	REAL,"
		"directionY	REAL,"
		"directionZ	REAL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE captures ("
		"id	INTEGER PRIMARY KEY,"
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"serverNameStripped	TEXT NOT NULL,"
		"flagHoldTime	INTEGER NOT NULL,"
		"flagPickupSource	INTEGER NOT NULL,"
		"capperName	TEXT NOT NULL,"
		"capperNameStripped	TEXT NOT NULL,"
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
		"redScore INTEGER,"
		"blueScore INTEGER,"
		"redPlayerCount INTEGER,"
		"bluePlayerCount INTEGER,"
		"sumPlayerCount INTEGER,"
		"maxSpeedCapperLastSecond	REAL,"
		"maxSpeedCapper	REAL,"
		"averageSpeedCapper	REAL,"
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
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE defragRuns ("
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"serverNameStripped	TEXT NOT NULL,"
		"readableTime TEXT NOT NULL,"
		"totalMilliseconds	INTEGER,"
		"playerName	TEXT NOT NULL,"
		"playerNameStripped	TEXT NOT NULL,"
		"isTop10	BOOLEAN NOT NULL,"
		"isNumber1	BOOLEAN NOT NULL,"
		"isPersonalBest	BOOLEAN NOT NULL,"
		"wasVisible	BOOLEAN NOT NULL,"
		"wasFollowed	BOOLEAN NOT NULL,"
		"wasFollowedOrVisible	BOOLEAN NOT NULL,"
		"averageStrafeDeviation REAL,"
		"runnerClientNum	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE laughs ("
		"id	INTEGER PRIMARY KEY,"
		"map	TEXT NOT NULL,"
		"serverName	TEXT NOT NULL,"
		"serverNameStripped	TEXT NOT NULL,"
		"laughs	TEXT NOT NULL,"
		"chatlog	TEXT NOT NULL,"
		"chatlogStripped	TEXT NOT NULL,"
		"laughCount	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"duration INTEGER NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"
		"); ",
		NULL, NULL, NULL);
	sqlite3_exec(killDb, "CREATE TABLE killSprees ("
		"hash	TEXT,"
		"shorthash	TEXT,"
		"maxDelay	INTEGER NOT NULL,"
		"map	TEXT NOT NULL,"
		"killerName	TEXT NOT NULL,"
		"killerNameStripped	TEXT NOT NULL,"
		"victimNames	TEXT NOT NULL,"
		"victimNamesStripped	TEXT NOT NULL,"
		"killHashes	TEXT NOT NULL,"
		"killerClientNum	INTEGER NOT NULL,"
		"victimClientNums	TEXT NOT NULL,"
		"countKills	INTEGER NOT NULL,"
		"countRets	INTEGER NOT NULL,"
		"countDooms	INTEGER NOT NULL,"
		"countExplosions	INTEGER NOT NULL,"
		"countThirdPersons	INTEGER NOT NULL,"
		"nearbyPlayers	TEXT,"
		"nearbyPlayerCount	INTEGER NOT NULL,"
		"demoRecorderClientnum	INTEGER NOT NULL,"
		"maxSpeedAttacker	REAL,"
		"maxSpeedTargets	REAL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoTime INTEGER NOT NULL,"
		"duration INTEGER NOT NULL,"
		"serverTime INTEGER NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"//,"
		//"PRIMARY KEY(hash)"
		"); ",
		NULL,NULL,NULL);
	sqlite3_exec(killDb, "CREATE TABLE playerModels (" 
		"map	TEXT NOT NULL,"
		"baseModel	TEXT NOT NULL,"
		"variant	TEXT,"
		"countFound INTEGER NOT NULL,"
		"PRIMARY KEY(map,baseModel,variant)"
		"); ",
		NULL,NULL,NULL);
	sqlite3_exec(killDb, "CREATE TABLE playerDemoStats (" 
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
		"hitBySaberCount INTEGER NOT NULL,"
		"hitBySaberBlockableCount INTEGER NOT NULL,"
		"parryCount INTEGER NOT NULL,"
		"attackFromParryCount INTEGER NOT NULL,"
		"demoName TEXT NOT NULL,"
		"demoPath TEXT NOT NULL,"
		"demoDateTime TIMESTAMP NOT NULL"//,"
		//"PRIMARY KEY(playerName,clientNum,map,demoPath)"
		"); ",
		NULL,NULL,NULL);
	
	/*char* preparedStatementText = "INSERT INTO kills"
		"(hash, shorthash, map, killerName, victimName, killerClientNum, victimClientNum, isReturn, isDoomKill, isExplosion, isSuicide, targetIsVisible,attackerIsVisible,"
		"isFollowed, meansOfDeath, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTarget, meansOfDeathString, probableKillingWeapon, positionX, "
		"positionY, positionZ,demoName,demoTime, serverTime, demoDateTime)"
		"VALUES "
		"(@hash, @shorthash, @map, @killerName, @victimName, @killerClientNum, @victimClientNum, @isReturn, @isDoomKill, @isExplosion, @isSuicide, @targetIsVisible,@attackerIsVisible,"
		"@isFollowed, @meansOfDeath, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTarget, @meansOfDeathString, @probableKillingWeapon, @positionX,"
		"@positionY, @positionZ,@demoName,@demoTime, @serverTime, @demoDateTime);";*/
	char* preparedStatementText = "INSERT INTO kills"
		"(hash,shorthash,map,serverName,serverNameStripped,killerName,killerNameStripped,victimName,victimNameStripped,killerTeam,victimTeam,redScore,blueScore,otherFlagStatus,redPlayerCount,bluePlayerCount,sumPlayerCount,killerClientNum,victimClientNum,isDoomKill,isExplosion,isSuicide,isModSuicide,meansOfDeath,positionX,positionY,positionZ)"
		"VALUES "
		"(@hash,@shorthash,@map,@serverName,@serverNameStripped,@killerName,@killerNameStripped,@victimName,@victimNameStripped,@killerTeam,@victimTeam,@redScore,@blueScore,@otherFlagStatus,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@killerClientNum,@victimClientNum,@isDoomKill,@isExplosion,@isSuicide,@isModSuicide,@meansOfDeath,@positionX,@positionY,@positionZ);";
	sqlite3_stmt* insertStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText, strlen(preparedStatementText) + 1, &insertStatement, NULL);
	preparedStatementText = "INSERT INTO killAngles"
		"(hash,shorthash,killerIsFlagCarrier,isReturn,victimCapperKills,victimCapperRets,victimCapperWasFollowedOrVisible,victimCapperMaxNearbyEnemyCount,victimCapperMoreThanOneNearbyEnemyTimePercent,victimCapperAverageNearbyEnemyCount,victimCapperMaxVeryCloseEnemyCount,victimCapperAnyVeryCloseEnemyTimePercent,victimCapperMoreThanOneVeryCloseEnemyTimePercent,victimCapperAverageVeryCloseEnemyCount,victimFlagPickupSource,victimFlagHoldTime,targetIsVisible,targetIsFollowed,targetIsFollowedOrVisible,attackerIsVisible,attackerIsFollowed,demoRecorderClientnum,boosts,boostCountTotal,boostCountAttacker,boostCountVictim,projectileWasAirborne,maxAngularSpeedAttacker,maxAngularAccelerationAttacker,maxAngularJerkAttacker,maxAngularSnapAttacker,maxSpeedAttacker,maxSpeedTarget,currentSpeedAttacker,currentSpeedTarget,meansOfDeathString,probableKillingWeapon,demoName,demoPath,demoTime,serverTime,demoDateTime,lastSaberMoveChangeSpeed,timeSinceLastSaberMoveChange,timeSinceLastBackflip,nearbyPlayers,nearbyPlayerCount,directionX,directionY,directionZ,map,isSuicide,isModSuicide,attackerIsFollowedOrVisible)"
		"VALUES "
		"(@hash,@shorthash,@killerIsFlagCarrier,@isReturn,@victimCapperKills,@victimCapperRets,@victimCapperWasFollowedOrVisible,@victimCapperMaxNearbyEnemyCount,@victimCapperMoreThanOneNearbyEnemyTimePercent,@victimCapperAverageNearbyEnemyCount,@victimCapperMaxVeryCloseEnemyCount,@victimCapperAnyVeryCloseEnemyTimePercent,@victimCapperMoreThanOneVeryCloseEnemyTimePercent,@victimCapperAverageVeryCloseEnemyCount,@victimFlagPickupSource,@victimFlagHoldTime,@targetIsVisible,@targetIsFollowed,@targetIsFollowedOrVisible,@attackerIsVisible,@attackerIsFollowed,@demoRecorderClientnum,@boosts,@boostCountTotal,@boostCountAttacker,@boostCountVictim,@projectileWasAirborne,@maxAngularSpeedAttacker,@maxAngularAccelerationAttacker,@maxAngularJerkAttacker,@maxAngularSnapAttacker,@maxSpeedAttacker,@maxSpeedTarget,@currentSpeedAttacker,@currentSpeedTarget,@meansOfDeathString,@probableKillingWeapon,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime,@lastSaberMoveChangeSpeed,@timeSinceLastSaberMoveChange,@timeSinceLastBackflip,@nearbyPlayers,@nearbyPlayerCount,@directionX,@directionY,@directionZ,@map,@isSuicide,@isModSuicide,@attackerIsFollowedOrVisible);";
	sqlite3_stmt* insertAngleStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertAngleStatement,NULL);
	preparedStatementText = "INSERT INTO captures"
		"(map,serverName,serverNameStripped,flagHoldTime,flagPickupSource,capperName,capperNameStripped,capperClientNum,capperIsVisible,capperIsFollowed,capperIsFollowedOrVisible,capperWasVisible,capperWasFollowed,capperWasFollowedOrVisible,demoRecorderClientnum,flagTeam,capperKills,capperRets,redScore,blueScore,redPlayerCount,bluePlayerCount,sumPlayerCount,maxSpeedCapperLastSecond,maxSpeedCapper,averageSpeedCapper,nearbyPlayers,nearbyPlayerCount,nearbyEnemies,nearbyEnemyCount,maxNearbyEnemyCount,moreThanOneNearbyEnemyTimePercent,averageNearbyEnemyCount,maxVeryCloseEnemyCount,anyVeryCloseEnemyTimePercent,moreThanOneVeryCloseEnemyTimePercent,averageVeryCloseEnemyCount,directionX,directionY,directionZ,positionX,positionY,positionZ,demoName,demoPath,demoTime,serverTime,demoDateTime)"
		"VALUES "
		"(@map,@serverName,@serverNameStripped,@flagHoldTime,@flagPickupSource,@capperName,@capperNameStripped,@capperClientNum,@capperIsVisible,@capperIsFollowed,@capperIsFollowedOrVisible,@capperWasVisible,@capperWasFollowed,@capperWasFollowedOrVisible,@demoRecorderClientnum,@flagTeam,@capperKills,@capperRets,@redScore,@blueScore,@redPlayerCount,@bluePlayerCount,@sumPlayerCount,@maxSpeedCapperLastSecond,@maxSpeedCapper,@averageSpeedCapper,@nearbyPlayers,@nearbyPlayerCount,@nearbyEnemies,@nearbyEnemyCount,@maxNearbyEnemyCount,@moreThanOneNearbyEnemyTimePercent,@averageNearbyEnemyCount,@maxVeryCloseEnemyCount,@anyVeryCloseEnemyTimePercent,@moreThanOneVeryCloseEnemyTimePercent,@averageVeryCloseEnemyCount,@directionX,@directionY,@directionZ,@positionX,@positionY,@positionZ,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime);";
	sqlite3_stmt* insertCaptureStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertCaptureStatement,NULL);
	preparedStatementText = "INSERT INTO defragRuns"
		"(map,serverName,serverNameStripped,readableTime,totalMilliseconds,playerName,playerNameStripped,demoRecorderClientnum,runnerClientNum,isTop10,isNumber1,isPersonalBest,wasVisible,wasFollowed,wasFollowedOrVisible,averageStrafeDeviation,demoName,demoPath,demoTime,serverTime,demoDateTime)"
		"VALUES "
		"(@map,@serverName,@serverNameStripped,@readableTime,@totalMilliseconds,@playerName,@playerNameStripped,@demoRecorderClientnum,@runnerClientNum,@isTop10,@isNumber1,@isPersonalBest,@wasVisible,@wasFollowed,@wasFollowedOrVisible,@averageStrafeDeviation,@demoName,@demoPath,@demoTime,@serverTime,@demoDateTime);";
	sqlite3_stmt* insertDefragRunStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertDefragRunStatement,NULL);
	preparedStatementText = "INSERT INTO laughs"
		"(map,serverName,serverNameStripped,laughs,chatlog,chatlogStripped,laughCount,demoRecorderClientnum,demoName,demoPath,duration,demoTime,serverTime,demoDateTime)"
		"VALUES "
		"(@map,@serverName,@serverNameStripped,@laughs,@chatlog,@chatlogStripped,@laughCount,@demoRecorderClientnum,@demoName,@demoPath,@duration,@demoTime,@serverTime,@demoDateTime);";
	sqlite3_stmt* insertLaughsStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertLaughsStatement,NULL);
	preparedStatementText = "INSERT INTO killSprees "
		"( hash, shorthash,maxDelay, map,killerName,killerNameStripped, victimNames, victimNamesStripped ,killHashes, killerClientNum, victimClientNums, countKills, countRets, countDooms, countExplosions,"
		" countThirdPersons, demoRecorderClientnum, maxSpeedAttacker, maxSpeedTargets,demoName,demoPath,demoTime,duration,serverTime,demoDateTime,nearbyPlayers,nearbyPlayerCount)"
		" VALUES "
		"( @hash, @shorthash, @maxDelay,@map, @killerName,@killerNameStripped, @victimNames ,@victimNamesStripped ,@killHashes, @killerClientNum, @victimClientNums, @countKills, @countRets, @countDooms, @countExplosions,"
		" @countThirdPersons, @demoRecorderClientnum, @maxSpeedAttacker, @maxSpeedTargets,@demoName,@demoPath,@demoTime,@duration,@serverTime,@demoDateTime,@nearbyPlayers,@nearbyPlayerCount)";
	sqlite3_stmt* insertSpreeStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertSpreeStatement,NULL);
	preparedStatementText = "INSERT OR IGNORE INTO playerModels (map,baseModel,variant,countFound) VALUES (@map,@baseModel,@variant, 0);";
	sqlite3_stmt* insertPlayerModelStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&insertPlayerModelStatement,NULL);
	preparedStatementText = "UPDATE playerModels SET countFound = countFound + 1 WHERE map=@map AND baseModel=@baseModel AND variant=@variant;";
	sqlite3_stmt* updatePlayerModelCountStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&updatePlayerModelCountStatement,NULL);
	preparedStatementText = "SELECT last_insert_rowid();";
	sqlite3_stmt* selectLastInsertRowIdStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText,strlen(preparedStatementText)+1,&selectLastInsertRowIdStatement,NULL);

	preparedStatementText = "INSERT INTO playerDemoStats "
		"(map,playerName,playerNameStripped,clientNum,averageStrafeDeviation,averageStrafeDeviationBucketsJSON,averageStrafeDeviationNoSaberMove,averageStrafeDeviationNoSaberMoveBucketsJSON,strafeSampleCount,strafeNoSaberMoveSampleCount,hitBySaberCount,hitBySaberBlockableCount,parryCount,attackFromParryCount,demoName,demoPath,demoDateTime)"
		" VALUES "
		"( @map,@playerName,@playerNameStripped,@clientNum,@averageStrafeDeviation,@averageStrafeDeviationBucketsJSON,@averageStrafeDeviationNoSaberMove,@averageStrafeDeviationNoSaberMoveBucketsJSON,@strafeSampleCount,@strafeNoSaberMoveSampleCount,@hitBySaberCount,@hitBySaberBlockableCount,@parryCount,@attackFromParryCount,@demoName,@demoPath,@demoDateTime)";
	sqlite3_stmt* insertPlayerDemoStatsStatement;
	sqlite3_prepare_v2(killDb, preparedStatementText, strlen(preparedStatementText) + 1, &insertPlayerDemoStatsStatement, NULL);


	sqlite3_exec(killDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);


#ifdef DEBUGSTATSDB
	sqlite3* debugStatsDb;
	sqlite3_open("debugStatsDb.db", &debugStatsDb);

	sqlite3_exec(debugStatsDb, "CREATE TABLE animStances ("
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
	preparedStatementText = "INSERT OR IGNORE INTO animStances (demoVersion,saberHolstered,torsoAnim,legsAnim,saberMove,stance,countFound) VALUES (@demoVersion,@saberHolstered,@torsoAnim,@legsAnim,@saberMove,@stance, 0);";
	sqlite3_stmt* insertAnimStanceStatement;
	sqlite3_prepare_v2(debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &insertAnimStanceStatement, NULL);
	preparedStatementText = "UPDATE animStances SET countFound = countFound + @countFound WHERE demoVersion=@demoVersion AND saberHolstered=@saberHolstered AND torsoAnim=@torsoAnim AND @legsAnim=@legsAnim AND saberMove=@saberMove AND stance=@stance;";
	sqlite3_stmt* updateAnimStanceCountStatement;
	sqlite3_prepare_v2(debugStatsDb, preparedStatementText, strlen(preparedStatementText) + 1, &updateAnimStanceCountStatement, NULL);
	sqlite3_exec(debugStatsDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
#endif



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
	strncpy_s(oldName, sizeof(oldName),sourceDemoFile, strlen(sourceDemoFile) - 6);
	//strncpy_s(ext, sizeof(ext), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);
	//strncpy_s(originalExt, sizeof(originalExt), (char*)sourceDemoFile + strlen(sourceDemoFile) - 6, 6);

	/*char specialTypeChar = ext[3];
	ext[3] = '_';

	if (specialTypeChar == 'c') {
		isCompressedFile = qtrue;
	}*/

	demoCutGetDemoType(sourceDemoFile,ext,&demoType,&isCompressedFile,&demo.cut.Clc.demoCheckFor103);

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
	oldSize = FS_FOpenFileRead(va("%s%s", oldName, ext), &oldHandle, qtrue, isCompressedFile);
	if (!oldHandle) {
		Com_Printf("Failed to open %s for reading.\n", oldName);
		return qfalse;
	}



	std::string oldPath = va("%s%s", oldName, ext);
	std::string oldBasename = oldPath.substr(oldPath.find_last_of("/\\") + 1);

	// turn oldpath absolute
	std::filesystem::path tmpFSPath = oldPath;
	oldPath = std::filesystem::absolute(tmpFSPath).string();

	std::filesystem::file_time_type filetime = std::filesystem::last_write_time(va("%s%s", oldName, ext));
	time_t oldDemoDateModified = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(filetime -std::filesystem::_File_time_clock::now() + std::chrono::system_clock::now()));

	//memset(&demo.cut.Clc, 0, sizeof(demo.cut.Clc));
	memset(&demo, 0, sizeof(demo));

	int messageOffset = 0;


	// Maybe we will make these user options someday...
	const qboolean derivativeRetiming = qtrue;
	const qboolean derivativeMagnitudeCompensate = qtrue;

	float accelerationMultiplier = derivativeMagnitudeCompensate ?  1.0f / 100.0f : 1.0f;
	float jerkMultiplier = derivativeMagnitudeCompensate ? 1.0f / 10000.0f : 1.0f;
	float snapMultiplier = derivativeMagnitudeCompensate ? 1.0f / 1000000.0f : 1.0f;

	//	Com_SetLoadingMsg("Cutting the demo...");
	while (oldSize > 0) {

		Com_Memset(&thisFrameInfo, 0, sizeof(thisFrameInfo));
		Com_Memset(&hitDetectionData, 0, sizeof(hitDetectionData));

		qboolean strafeApplicablePlayerStateThisFrame = qfalse;
		float playerStateStrafeDeviationThisFrame = 0;

	cutcontinue:
		if (isCompressedFile) {
			oldDataRaw.clear();
			MSG_InitRaw(&oldMsg, &oldDataRaw); // Input message
		}
		else {
			MSG_Init(&oldMsg, oldData, sizeof(oldData)); // Input message
		}
		/* Read the sequence number */
		if (FS_Read(&demo.cut.Clc.serverMessageSequence, 4, oldHandle) != 4)
			goto cuterror;
		demo.cut.Clc.serverMessageSequence = LittleLong(demo.cut.Clc.serverMessageSequence);
		oldSize -= 4;
		/* Read the message size */
		if (FS_Read(&oldMsg.cursize, 4, oldHandle) != 4)
			goto cuterror;
		oldMsg.cursize = LittleLong(oldMsg.cursize);
		oldSize -= 4;
		/* Negative size signals end of demo */
		if (oldMsg.cursize < 0)
			break;
		if (oldMsg.cursize > oldMsg.maxsize)
			goto cuterror;
		/* Read the actual message */
		if (oldMsg.raw) {
			oldMsg.dataRaw->resize(oldMsg.cursize);
			if (FS_Read(oldMsg.dataRaw->data(), oldMsg.cursize, oldHandle) != oldMsg.cursize) {
				goto cuterror;
			}
		}
		else {
			if (FS_Read(oldMsg.data, oldMsg.cursize, oldHandle) != oldMsg.cursize)
				goto cuterror;
		}
		oldSize -= oldMsg.cursize;
		// init the bitstream
		MSG_BeginReading(&oldMsg);
		// Skip the reliable sequence acknowledge number
		MSG_ReadLong(&oldMsg);
		//
		// parse the message
		//
		while (1) {
			byte cmd;
			if (oldMsg.readcount > oldMsg.cursize) {
				Com_Printf("Demo cutter, read past end of server message.\n");
				goto cuterror;
			}
			cmd = MSG_ReadByte(&oldMsg);
			cmd = generalizeGameSVCOp(cmd, demoType);
			if (cmd == svc_EOF_general) {
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
			default:
				Com_Printf("ERROR: CL_ParseServerMessage: Illegible server message\n");
				goto cuterror;
			case svc_nop_general:
				break;
			case svc_serverCommand_general:
				demoCutParseCommandString(&oldMsg, &demo.cut.Clc);
				break;
			case svc_gamestate_general:
				lastGameStateChange = demo.cut.Cl.snap.serverTime;
				lastGameStateChangeInDemoTime = demoCurrentTime;
				//if (readGamestate > demo.currentNum) {
				//	Com_Printf("Warning: unexpected new gamestate, finishing cutting.\n");
				//	goto cutcomplete;
				//}
				if (!demoCutParseGamestate(&oldMsg, &demo.cut.Clc, &demo.cut.Cl,&demoType)) { // Pass demoType by reference in case we need 1.03 detection
					goto cuterror;
				}

				CheckForNameChanges<max_clients>(&demo.cut.Cl,killDb,insertPlayerModelStatement, updatePlayerModelCountStatement,demoType);
				setPlayerAndTeamData<max_clients>(&demo.cut.Cl, demoType);
				updateForcePowersInfo(&demo.cut.Cl);
				updatePlayerDemoStatsArrayPointers<max_clients>(demoType);
				//Com_sprintf(newName, sizeof(newName), "%s_cut%s", oldName, ext);
				//newHandle = FS_FOpenFileWrite(newName);
				//if (!newHandle) {
				//	Com_Printf("Failed to open %s for target cutting.\n", newName);
				//	return qfalse;
				//}
				readGamestate++;
				break;
			case svc_snapshot_general:
				if (!demoCutParseSnapshot(&oldMsg, &demo.cut.Clc, &demo.cut.Cl, demoType,qtrue)) {
					goto cuterror;
				}

				// Time related stuff
				if (messageOffset++ == 0) {
					// first message in demo. Get servertime offset from here to cut correctly.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}
				if (demo.cut.Cl.snap.serverTime < lastKnownTime && demo.cut.Cl.snap.serverTime < 10000) { // Assume a servertime reset (new serverTime is under 10 secs). 
					demoBaseTime = demoCurrentTime; // Remember fixed offset into demo time.
					demoStartTime = demo.cut.Cl.snap.serverTime;
				}
				demoCurrentTime = demoBaseTime + demo.cut.Cl.snap.serverTime - demoStartTime;
				deltaTimeFromLastSnapshot = demoCurrentTime - demoOldTime;
				lastKnownTime = demo.cut.Cl.snap.serverTime;
				demoOldTime = demoCurrentTime;

				// Record speeds, check sabermove changes and other entity related tracking
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];

					thisFrameInfo.entityExists[thisEs->number] = qtrue;

					// Player related tracking
					if (thisEs->number >= 0 && thisEs->number < max_clients) {

						thisFrameInfo.canBlockSimplified[thisEs->number] = WP_SaberCanBlock_Simple(thisEs,demoType);

						float speed = VectorLength(thisEs->pos.trDelta); // Used for strafe analysis buckets and saving recent speeds

						// Strafe precision
						qboolean strafeApplicable = qfalse;
						float strafeDeviation = calculateStrafeDeviation(thisEs, &strafeApplicable);
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
								if (thisEs->saberMove >= LS_NONE && thisEs->saberMove <= LS_PUTAWAY) {

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
						thisFrameInfo.legsAnim[thisEs->number] = thisEs->legsAnim;
						thisFrameInfo.torsoAnim[thisEs->number] = thisEs->torsoAnim;

						// Backflip detection
						if (
							lastFrameInfo.entityExists[thisEs->number] && (
								(thisEs->legsAnim != lastFrameInfo.legsAnim[thisEs->number] && isBackflip(thisEs->legsAnim, demoType))
							|| (thisEs->torsoAnim != lastFrameInfo.torsoAnim[thisEs->number] && isBackflip(thisEs->torsoAnim, demoType))
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

							// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
							if (playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMove != thisEs->saberMove) {
								if (thisEs->saberMove >= LS_PARRY_UP && thisEs->saberMove <= LS_PARRY_LL) {
									hitDetectionData[thisEs->number].newParryDetected = qtrue;
								}
								for (int smI = MAX_PAST_SABERMOVE_SAVE -1; smI > 0; smI--) {
									playerLastSaberMove[thisEs->number].lastSaberMove[smI] = playerLastSaberMove[thisEs->number].lastSaberMove[smI-1];
								}
								playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMoveChange = demoCurrentTime;
								playerLastSaberMove[thisEs->number].lastSaberMove[0].saberMove= thisEs->saberMove;
								playerLastSaberMove[thisEs->number].lastSaberMove[0].speed= speed;
							}
						}
					}
					// Track trip mine owners and the first time they were seen.
					// Careful: The current way it is handled is blocked if the mine is temporarily not visible. The detection will think it appeared later.
					else if (thisEs->eType == ET_GENERAL && thisEs->weapon == WP_TRIP_MINE && (thisEs->eFlags & EF_MISSILE_STICK)) { // tripmine
						thisFrameInfo.entityOwnerInfo[thisEs->number].owner = thisEs->genericenemyindex - 1024;
						thisFrameInfo.entityOwnerInfo[thisEs->number].type = TET_TRIPMINE;
						if ((thisEs->event & ~EV_EVENT_BITS) == EV_MISSILE_MISS) {
							// This mine is exploding right now
							thisFrameInfo.entityOwnerInfo[thisEs->number].flags |= TETFLAG_EXPLODED;
						}
						if (thisEs->pos.trType == TR_GRAVITY) {
							// This mine is airborne
							thisFrameInfo.entityOwnerInfo[thisEs->number].flags |= TETFLAG_AIRBORNE;
						}
						if (lastFrameInfo.entityExists[thisEs->number] && 
							thisFrameInfo.entityOwnerInfo[thisEs->number].type == lastFrameInfo.entityOwnerInfo[thisEs->number].type && 
							thisFrameInfo.entityOwnerInfo[thisEs->number].owner == lastFrameInfo.entityOwnerInfo[thisEs->number].owner) {

							thisFrameInfo.entityOwnerInfo[thisEs->number].firstSeen = lastFrameInfo.entityOwnerInfo[thisEs->number].firstSeen;
						}
						else {
							thisFrameInfo.entityOwnerInfo[thisEs->number].firstSeen = demoCurrentTime;
						}
					}
				}

				// 
				// Playerstate tracking
				{
					float speed = VectorLength(demo.cut.Cl.snap.ps.velocity);

					playerStateStrafeDeviationThisFrame = calculateStrafeDeviation(&demo.cut.Cl.snap.ps, &strafeApplicablePlayerStateThisFrame);
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
							if (demo.cut.Cl.snap.ps.saberMove >= LS_NONE && demo.cut.Cl.snap.ps.saberMove <= LS_PUTAWAY) { 

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

					thisFrameInfo.commandTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.commandTime;
					thisFrameInfo.legsAnim[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.legsAnim;
					thisFrameInfo.torsoAnim[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.torsoAnim;

					// Backflip detection
					if (
						lastFrameInfo.entityExists[demo.cut.Cl.snap.ps.clientNum] && (
							(demo.cut.Cl.snap.ps.legsAnim != lastFrameInfo.legsAnim[demo.cut.Cl.snap.ps.clientNum] && isBackflip(demo.cut.Cl.snap.ps.legsAnim, demoType))
							|| (demo.cut.Cl.snap.ps.torsoAnim != lastFrameInfo.torsoAnim[demo.cut.Cl.snap.ps.clientNum] && isBackflip(demo.cut.Cl.snap.ps.torsoAnim, demoType))
							)) {
						lastBackflip[demo.cut.Cl.snap.ps.clientNum] = demoCurrentTime;
					}
					// Backflip interrupt (touching ground) (don't interrupt if we still happen to be on ground on first frame of backflip?)
					else if (demo.cut.Cl.snap.ps.groundEntityNum != ENTITYNUM_NONE) {
						lastBackflip[demo.cut.Cl.snap.ps.clientNum] = -1;
					}

					thisFrameInfo.groundEntityNum[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.groundEntityNum;
					VectorCopy(demo.cut.Cl.snap.ps.origin, thisFrameInfo.playerPositions[demo.cut.Cl.snap.ps.clientNum]);
					VectorCopy(demo.cut.Cl.snap.ps.velocity, thisFrameInfo.playerVelocities[demo.cut.Cl.snap.ps.clientNum]);
					VectorCopy(demo.cut.Cl.snap.ps.viewangles, thisFrameInfo.playerAngles[demo.cut.Cl.snap.ps.clientNum]);
					//thisFrameInfo.playerGSpeeds[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.speed;
					//thisFrameInfo.playerMaxWalkSpeed[demo.cut.Cl.snap.ps.clientNum] = sqrtf(demo.cut.Cl.snap.ps.speed * demo.cut.Cl.snap.ps.speed * 2);
					thisFrameInfo.entityExists[demo.cut.Cl.snap.ps.clientNum] = qtrue;
					thisFrameInfo.canBlockSimplified[demo.cut.Cl.snap.ps.clientNum] = WP_SaberCanBlock_Simple(&demo.cut.Cl.snap.ps, demoType);

					if (demo.cut.Cl.snap.ps.pm_type != PM_DEAD && demo.cut.Cl.snap.ps.stats[STAT_HEALTH] > 0) {
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

						// Remember at which time and speed the last sabermove change occurred. So we can see movement speed at which dbs and such was executed.
						if (playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMove != demo.cut.Cl.snap.ps.saberMove) {
							if (demo.cut.Cl.snap.ps.saberMove >= LS_PARRY_UP && demo.cut.Cl.snap.ps.saberMove <= LS_PARRY_LL) {
								hitDetectionData[demo.cut.Cl.snap.ps.clientNum].newParryDetected = qtrue;
							}
							for (int smI = MAX_PAST_SABERMOVE_SAVE - 1; smI > 0; smI--) {
								playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[smI] = playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[smI - 1];
							}
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMoveChange = demoCurrentTime;
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].saberMove = demo.cut.Cl.snap.ps.saberMove;
							playerLastSaberMove[demo.cut.Cl.snap.ps.clientNum].lastSaberMove[0].speed = speed;
						}
					}
				}

				

				// Playerstate boost detection
				// If otherKiller and otherKillerTime in PS has changed from last frame, this player is boosted. Add the boost to his list.
				{
					qboolean playerStateBoostDetected = qfalse;
					thisFrameInfo.pmFlagKnockback[demo.cut.Cl.snap.ps.clientNum] = (qboolean)!!(demo.cut.Cl.snap.ps.pm_flags & PMF_TIME_KNOCKBACK);
					thisFrameInfo.pmFlagTime[demo.cut.Cl.snap.ps.clientNum] = demo.cut.Cl.snap.ps.pm_time;
					thisFrameInfo.psTeleportBit[demo.cut.Cl.snap.ps.clientNum] = (qboolean)!!(demo.cut.Cl.snap.ps.eFlags & EF_TELEPORT_BIT);

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
						thisFrameInfo.pmFlagKnockback[demo.cut.Cl.snap.ps.clientNum]
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

							qboolean playerIsDFAing = (qboolean)((thisFrameInfo.legsAnim[i] != lastFrameInfo.legsAnim[i] && isDFA(thisFrameInfo.legsAnim[i], demoType))
								|| (thisFrameInfo.torsoAnim[i] != lastFrameInfo.torsoAnim[i] && isDFA(thisFrameInfo.torsoAnim[i], demoType)));

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
								else if ((thisFrameInfo.legsAnim[i] != lastFrameInfo.legsAnim[i] && isBackflip(thisFrameInfo.legsAnim[i],demoType))
									|| (thisFrameInfo.torsoAnim[i] != lastFrameInfo.torsoAnim[i] && isBackflip(thisFrameInfo.torsoAnim[i], demoType))
									){
									//if (thisFrameInfo.playerVelocities[i][2] > BACKFLIP_MAX_VELOCITY_DELTA_VERT) {
									if (verticalDeltaNormalized > BACKFLIP_MAX_VELOCITY_DELTA_VERT) {
										isBoost = qtrue;
									}
								} 
								// If we are in a yellow dfa, our vertical velocity delta could exceed a usual jump velocity so make sure our speed upwards is higher than the speed of a yellow dfa
								//else if (playerLastSaberMove[i].lastSaberMove == LS_A_FLIP_STAB || playerLastSaberMove[i].lastSaberMove == LS_A_FLIP_SLASH){
								else if ((thisFrameInfo.legsAnim[i] != lastFrameInfo.legsAnim[i] && isYellowDFA(thisFrameInfo.legsAnim[i], demoType))
									|| (thisFrameInfo.torsoAnim[i] != lastFrameInfo.torsoAnim[i] && isYellowDFA(thisFrameInfo.torsoAnim[i], demoType))
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
					if (lastIndexToRemove != -1) {
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
					for (int i = 0; i < max_clients; i++) {
						size_t maxIndexToRemove = -1;
						for (int w = 0; w < walkDetectedTimes[i].size(); w++) {
							bool isRemoveWorthy = (demoCurrentTime - walkDetectedTimes[i][w]) > BOOST_DETECT_MAX_AGE_WALKING;
							if (isRemoveWorthy) {
								maxIndexToRemove = w;
								newestRemoveWorthyWalks[i] = walkDetectedTimes[i][w];
							}
							else {
								break;
							}
						}
						if (maxIndexToRemove != -1) {
							walkDetectedTimes[i].erase(walkDetectedTimes[i].begin(), walkDetectedTimes[i].begin() + maxIndexToRemove + 1);
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
				if(demo.cut.Cl.snap.ps.weapon==WP_SABER){ // TODO Maybe add saber on/off here too? Because saber off might have same anim always?
					
					animStanceKey keyHere = { demoType,demo.cut.Cl.snap.ps.saberHolstered,demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT,demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT,demo.cut.Cl.snap.ps.saberMove,demo.cut.Cl.snap.ps.fd.saberAnimLevel };  // torsoAnim,legsAnim,saberMove,stance
					animStanceCounts[keyHere]++;
					
				}
#endif


				// Update player visible frames
				// For some info like angular velocity, acceleration and jerk we need to know that the past X frames had info about the player, else the value becomes invalid.
				for (int i = 0; i < max_clients;i++) {
					if (thisFrameInfo.entityExists[i]) {
						playerVisibleFrames[i]++;
						if (thisFrameInfo.commandTime[i] != lastFrameInfo.commandTime[i]) {
							playerVisibleClientFrames[i]++;
						}
					}
					else {
						playerVisibleFrames[i] = 0;
						playerVisibleClientFrames[i] = 0;
					}
				}
				
				{ // Playerstate events... uhm experimental?
					psEventData_t psEventData = demoCutGetEvent(&demo.cut.Cl.snap.ps, &demo.cut.Cl.oldSnap.ps,demoCurrentTime);

					// Ok this confirms he took damage.
					if (psEventData.externalEvent.event >= EV_PAIN && psEventData.externalEvent.event <= EV_DEATH3
						|| psEventData.predictableEvents[0].event >= EV_PAIN && psEventData.predictableEvents[0].event <= EV_DEATH3
						|| psEventData.predictableEvents[1].event >= EV_PAIN && psEventData.predictableEvents[1].event <= EV_DEATH3
						) {
						hitDetectionData[demo.cut.Cl.snap.ps.clientNum].painDetected = qtrue;
					}
				}

				// Fire events
				for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {

					entityState_t* thisEs = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
					int eventNumber = demoCutGetEvent(thisEs,demoCurrentTime);
					if (eventNumber) {
						

						// Handle kills
						if (eventNumber == EV_OBITUARY) {
							int				target = thisEs->otherEntityNum;
							int				attacker = thisEs->otherEntityNum2;
							int				mod = thisEs->eventParm;
							bool			victimIsFlagCarrier = false;
							bool			attackerIsFlagCarrier = false;
							bool			isSuicide;
							bool			isDoomKill;
							bool			isWorldKill = false;
							bool			targetIsVisible = false;
							bool			attackerIsVisibleOrFollowed = false;
							bool			targetIsVisibleOrFollowed = false;
							bool			attackerIsVisible = false;
							if (target < 0 || target >= max_clients) {
								std::cout << "CG_Obituary: target out of range. This should never happen really.";
							}

							if (attacker < 0 || attacker >= max_clients) {
								attacker = ENTITYNUM_WORLD;
								isWorldKill = true;
							}
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
							victimIsFlagCarrier = target == lastKnownBlueFlagCarrier || target == lastKnownRedFlagCarrier;
							attackerIsFlagCarrier = attacker == lastKnownBlueFlagCarrier || attacker == lastKnownRedFlagCarrier;

							int victimFlagTeam = victimIsFlagCarrier ? (target == lastKnownBlueFlagCarrier ? TEAM_BLUE:TEAM_RED) : -1;
							int attackerFlagTeam = attackerIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? TEAM_BLUE:TEAM_RED) : -1;

							int victimCarrierLastPickupOrigin = victimIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? cgs.blueFlagLastPickupOrigin : cgs.redFlagLastPickupOrigin) : -1;

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
							// TODO What if lastKnownBlueFlagCarrier or lastKnownRedFlagCarrier is remembered and another kill is counted wrongly as ret even though he is no longer holding a flag?


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
							isDoomKill = mod == MOD_FALLING;
							bool targetIsFollowed = demo.cut.Cl.snap.ps.clientNum == target;
							bool attackerIsFollowed = demo.cut.Cl.snap.ps.clientNum == attacker;
							attackerIsVisibleOrFollowed = attackerIsFollowed || attackerIsVisible;
							targetIsVisibleOrFollowed = targetIsFollowed || targetIsVisible;

							//targetIsVisible = targetIsVisible && attackerIsVisibleOrFollowed; // Make sure both attacker and victim are visible. Some servers send info

							float maxSpeedTargetFloat = getMaxSpeedForClientinTimeFrame(target, demoCurrentTime - 1000, demoCurrentTime);
							int maxSpeedTarget = maxSpeedTargetFloat;
							Kill thisKill;
							thisKill.isRet = victimIsFlagCarrier;
							thisKill.isSuicide = isSuicide;
							thisKill.isDoom = isDoomKill;
							thisKill.isExplosion = (mod == MOD_FLECHETTE ||
								mod == MOD_FLECHETTE_ALT_SPLASH ||
								mod == MOD_ROCKET ||
								mod == MOD_ROCKET_SPLASH ||
								mod == MOD_ROCKET_HOMING ||
								mod == MOD_ROCKET_HOMING_SPLASH ||
								mod == MOD_THERMAL ||
								mod == MOD_THERMAL_SPLASH ||
								mod == MOD_TRIP_MINE_SPLASH ||
								mod == MOD_TIMED_MINE_SPLASH ||
								mod == MOD_DET_PACK_SPLASH);
							thisKill.mod = (meansOfDeath_t)mod;
							thisKill.targetClientNum = target;
							thisKill.time = demoCurrentTime;
							//thisKill.isVisible = targetIsVisible;
							thisKill.isVisible = targetIsVisibleOrFollowed;
							thisKill.isFollowed = attackerIsFollowed;
							thisKill.victimMaxSpeedPastSecond = maxSpeedTargetFloat;
							thisKill.timeSinceSaberMoveChange = isWorldKill ? -1 : (demoCurrentTime-playerLastSaberMove[attacker].lastSaberMove[0].saberMoveChange);
							thisKill.timeSinceBackflip = isWorldKill ? -1 : (lastBackflip[attacker] >= 0?(demoCurrentTime-lastBackflip[attacker]):-1);
							thisKill.speedatSaberMoveChange = isWorldKill ? -1 : (playerLastSaberMove[attacker].lastSaberMove[0].speed);


							// This is the place that had all the continues originally.
							
							//entityState_t* attackerEntity = findEntity(attacker);

							// Find extra info for means of death.
							std::stringstream modInfo;
							weapon_t probableKillingWeapon = (weapon_t)weaponFromMOD[mod];
							
							qboolean needMoreInfo = qtrue;
							if (probableKillingWeapon == WP_NONE) { // means something like doom kill
								switch (mod) {
									case MOD_UNKNOWN:
										modInfo << "_WEIRD";
										break;
									case MOD_FORCE_DARK:
										modInfo << "_FORCE";
										needMoreInfo = qfalse;
										break;
									case MOD_SENTRY:
										modInfo << "_SENTRY";
										needMoreInfo = qfalse;
										break;
									case MOD_WATER:
										modInfo << "_DROWN";
										break;
									case MOD_SLIME:
										modInfo << "_SLIME";
										break;
									case MOD_LAVA:
										modInfo << "_LAVA";
										break;
									case MOD_CRUSH:
										modInfo << "_CRUSH";
										break;
									case MOD_TELEFRAG:
										modInfo << "_TELE";
										break;
									case MOD_FALLING:
										modInfo << "_DOOM";
										break;
									case MOD_SUICIDE:
										modInfo << "_SUIC";
										break;
									case MOD_TARGET_LASER:
										modInfo << "_LASER";
										break;
									case MOD_TRIGGER_HURT:
										modInfo << "_TRIG";
										break;
								}
								if(needMoreInfo)
									modInfo << "_~";
								if (attackerIsFollowed) {
									probableKillingWeapon = (weapon_t)demo.cut.Cl.snap.ps.weapon;
								}
								else if (attackerEntity) {
									probableKillingWeapon = (weapon_t)attackerEntity->weapon;
								}
							}
							if (needMoreInfo) {
								switch (probableKillingWeapon) {
									case WP_SABER:
										if (attackerIsFollowed) {

											modInfo << saberMoveNames[demo.cut.Cl.snap.ps.saberMove];
											if (!modInfo.str().size()) {
												modInfo << saberStyleNames[demo.cut.Cl.snap.ps.fd.saberDrawAnimLevel];
											}
											else {
												// It's a special attack.
												// Save speed at which it was executed.
												modInfo << "_" << (int)thisKill.speedatSaberMoveChange << "u";
											}

										}
										else {

											if (attackerEntity) {
												modInfo << saberMoveNames[attackerEntity->saberMove];
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
										}
										// Is the current saber move the follow up move of a bounce, deflect, parry, deflect, knockaway etc?
										//if ((playerLastSaberMove[attacker].lastLastSaberMove >= LS_K1_T_ && playerLastSaberMove[attacker].lastLastSaberMove <= LS_REFLECT_LL)
										//	|| (playerLastSaberMove[attacker].lastLastSaberMove >= LS_B1_BR && playerLastSaberMove[attacker].lastLastSaberMove <= LS_D1_B_)
										//	) {
										if (playerLastSaberMove[attacker].lastSaberMove[1].saberMove >= LS_B1_BR && playerLastSaberMove[attacker].lastSaberMove[1].saberMove <= LS_REFLECT_LL) {
											if (playerLastSaberMove[attacker].lastSaberMove[0].saberMove == saberMoveData[playerLastSaberMove[attacker].lastSaberMove[1].saberMove].chain_attack || playerLastSaberMove[attacker].lastSaberMove[0].saberMove == saberMoveData[playerLastSaberMove[attacker].lastSaberMove[1].saberMove].chain_idle) {
												// yep this is a chain from a parry or such.
												// not sure if we should include the stuff like broken parry which is followed by idle saber,
												// but i guess a broken parry followed by a burn kill could be fun to know too
												modInfo << "_FR";
												modInfo << saberMoveNames[playerLastSaberMove[attacker].lastSaberMove[1].saberMove];
											}
										}
										// DFA from parry? If our attack is a DFA or other pre-swing attack resulting from a parry (well basically there's only dfa and yellow dfa I think that qualify)
										// BUT: This doesnt find them all for some reason. Idk why. Maybe sometimes they arent true parry dfas. Who knows. Often theres nothing in the sabermove history with a saber move above 74, but rather transitions and whatnot
										else if (playerLastSaberMove[attacker].lastSaberMove[0].saberMove >= LS_A_JUMP_T__B_ && playerLastSaberMove[attacker].lastSaberMove[0].saberMove <= LS_A_FLIP_SLASH) {
											if (playerLastSaberMove[attacker].lastSaberMove[2].saberMove >= LS_B1_BR && playerLastSaberMove[attacker].lastSaberMove[2].saberMove <= LS_REFLECT_LL) {
												if (playerLastSaberMove[attacker].lastSaberMove[1].saberMove == saberMoveData[playerLastSaberMove[attacker].lastSaberMove[2].saberMove].chain_attack || playerLastSaberMove[attacker].lastSaberMove[1].saberMove == saberMoveData[playerLastSaberMove[attacker].lastSaberMove[2].saberMove].chain_idle) {
													
													qboolean isSlowDfa = (qboolean)(playerLastSaberMove[attacker].lastSaberMove[0].saberMove == LS_A_JUMP_T__B_ && (demoCurrentTime - playerLastSaberMove[attacker].lastSaberMove[1].saberMoveChange) > PARRY_DFA_DETECT_SLOW_THRESHOLD);
													qboolean isSlowYDfa = (qboolean)(playerLastSaberMove[attacker].lastSaberMove[0].saberMove != LS_A_JUMP_T__B_ && (demoCurrentTime - playerLastSaberMove[attacker].lastSaberMove[1].saberMoveChange) > PARRY_YDFA_DETECT_SLOW_THRESHOLD);
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
													modInfo << saberMoveNames[playerLastSaberMove[attacker].lastSaberMove[2].saberMove];

												}
											}
										}
										if (thisKill.timeSinceBackflip != -1 && thisKill.timeSinceBackflip < 1000) {
											modInfo << "_BF";
										}
										break;
									case WP_STUN_BATON:
										modInfo << "_STUN";
										break;
									case WP_BRYAR_PISTOL:
										modInfo << "_BRYAR";
										break;
									case WP_BLASTER:
										modInfo << "_BLASTER";
										break;
									case WP_DISRUPTOR:
										modInfo << "_DISRUPTOR";
										break;
									case WP_BOWCASTER:
										modInfo << "_BOWCAST";
										break;
									case WP_REPEATER:
										modInfo << "_REPEATER";
										break;
									case WP_DEMP2:
										modInfo << "_DEMP2";
										break;
									case WP_FLECHETTE:
										modInfo << "_FLECH";
										break;
									case WP_ROCKET_LAUNCHER:
										modInfo << "_ROCKET";
										break;
									case WP_THERMAL:
										modInfo << "_THERMAL";
										break;
									case WP_TRIP_MINE:
										modInfo << "_MINE";
										break;
									case WP_DET_PACK:
										modInfo << "_DTPCK";
										break;
									case WP_EMPLACED_GUN:
										modInfo << "_EMPLACED";
										break;
									case WP_TURRET:
										modInfo << "_TURRET";
										break;

									default:
										break;
								}
							}

							int killerProjectile = -1;
							qboolean isProjectileBased = qfalse;
							qboolean canBeAirborne = qfalse;
							qboolean projectileWasAirborne = qfalse;
							// TODO: Do this for other weapons too, not just mines
							if (mod == MOD_TRIP_MINE_SPLASH) { // If it's a trip mine kill, we wanna make sure that the mine that killed the victim was fired after a boost, else we ignore the boost.

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
							
							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "mapname");
							std::string serverName = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)-offset, "sv_hostname");
							std::string playername = "WEIRDATTACKER";
							const char* playerInfo;
							if (attacker >= 0 && attacker < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + attacker];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "n");
							}
							offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + target];
							playerInfo = demo.cut.Cl.gameState.stringData + offset;
							std::string victimname = Info_ValueForKey(playerInfo,sizeof(demo.cut.Cl.gameState.stringData)- offset, "n");



							int startTime = demoCurrentTime - bufferTime;
							int endTime = demoCurrentTime + bufferTime;
							int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
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
							hashss << playername << "_" << victimname << "_" << attacker << "_" << target << "_" << isDoomKill << "_" << isSuicide << "_" << mod << "_" << mapname << "_" << thisEs->pos.trBase[0] << "_" << thisEs->pos.trBase[1] << "_" << thisEs->pos.trBase[2];
							auto sha3_512 = picosha3::get_sha3_generator<224>();
							std::string hash_hex_string = sha3_512.get_hex_string(hashss.str());
							std::string shorthash = hash_hex_string.substr(0,3);

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


							kills[attacker].push_back(thisKill);



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

									qboolean doThis = qfalse;
									// find out if we should even bother
									if (boosts[i].boostedClientNum == attacker && boosts[i].boosterClientNum != target) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts
									
										if (boosts[i].demoTime > excludeAttackerBoostsAfter) continue;
										
										// Ignore mini boosts?
										//if (!boosts[i].confirmed && VectorDistance(thisFrameInfo.playerPositions[boosts[i].boostedClientNum], boosts[i].startPosition) < boosts[i].minimumTravelDistance) continue;
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;

										if (!boosts[i].checkFacingTowards(target)) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

										boostCountAttacker++;
										doThis = qtrue;
										boostsStringStream << "[KILLER by";
									}
									else if (boosts[i].boostedClientNum == target && boosts[i].boosterClientNum != attacker) { // Avoid detecting mutual boosts between killer and victin. Could have been swordfight. TODO: Allow very strong boosts

										// Ignore mini boosts?
										//if (!boosts[i].confirmed && VectorDistance(thisFrameInfo.playerPositions[boosts[i].boostedClientNum], boosts[i].startPosition) < boosts[i].minimumTravelDistance) continue;
										if (!boosts[i].confirmed && !boosts[i].checkDistanceTraveled(thisFrameInfo.playerPositions[boosts[i].boostedClientNum])) continue;
										
										if (!boosts[i].checkFacingTowards(attacker)) continue; // This ONLY affects entity based boost detects (check code of checkFacingTowards) and is a patchwork solution to missing boosterClientNum.

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
										boosterName = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "n");
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

							// Log the kill.
							SQLBIND_TEXT(insertStatement, "@hash", hash_hex_string.c_str());
							SQLBIND_TEXT(insertStatement, "@shorthash", shorthash.c_str());
							SQLBIND_TEXT(insertStatement, "@map", mapname.c_str());
							SQLBIND_TEXT(insertStatement, "@serverName", serverName.c_str());
							std::string serverNameStripped = Q_StripColorAll(serverName);
							SQLBIND_TEXT(insertStatement, "@serverNameStripped", serverNameStripped.c_str());
							SQLBIND_TEXT(insertStatement, "@killerName", playername.c_str());
							std::string playernameStripped = Q_StripColorAll(playername);
							SQLBIND_TEXT(insertStatement, "@killerNameStripped", playernameStripped.c_str());
							SQLBIND_TEXT(insertStatement, "@victimName", victimname.c_str());
							std::string victimnameStripped = Q_StripColorAll(victimname);
							SQLBIND_TEXT(insertStatement, "@victimNameStripped", victimnameStripped.c_str());
							if (isWorldKill) {
								SQLBIND_NULL(insertStatement, "@killerTeam");
							}
							else {
								SQLBIND(insertStatement, int, "@killerTeam", playerTeams[attacker]);
							}
							SQLBIND(insertStatement, int, "@victimTeam", playerTeams[target]);
							SQLBIND(insertStatement, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND(insertStatement, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							if (victimIsFlagCarrier) {
								SQLBIND(insertStatement, int, "@otherFlagStatus",playerTeams[target] == TEAM_BLUE ? cgs.blueflag: cgs.redflag);
							}
							else {
								SQLBIND_NULL(insertStatement, "@otherFlagStatus");
							}
							SQLBIND(insertStatement, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertStatement, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND(insertStatement, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount +  teamInfo[TEAM_BLUE].playerCount + teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertStatement, int, "@killerClientNum", attacker);
							SQLBIND(insertStatement, int, "@victimClientNum", target);
							SQLBIND(insertStatement, int, "@isDoomKill", isDoomKill);
							SQLBIND(insertStatement, int, "@isExplosion", thisKill.isExplosion);
							SQLBIND(insertStatement, int, "@isSuicide", isSuicide);
							SQLBIND(insertStatement, int, "@isModSuicide", mod==MOD_SUICIDE);
							SQLBIND(insertStatement, int, "@meansOfDeath", mod);
							SQLBIND(insertStatement, double, "@positionX", thisEs->pos.trBase[0]);
							SQLBIND(insertStatement, double, "@positionY", thisEs->pos.trBase[1]);
							SQLBIND(insertStatement, double, "@positionZ", thisEs->pos.trBase[2]);

							int queryResult = sqlite3_step(insertStatement);
							if (queryResult != SQLITE_DONE) {
								std::cout << "Error inserting kill into database: " << sqlite3_errmsg(killDb) << " (" << queryResult << ")" << "\n";
							}
							sqlite3_reset(insertStatement);


							SQLBIND_TEXT(insertAngleStatement, "@hash", hash_hex_string.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@shorthash", shorthash.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@map", mapname.c_str());
							SQLBIND(insertAngleStatement, int, "@isReturn", victimIsFlagCarrier);
							SQLBIND(insertAngleStatement, int, "@killerIsFlagCarrier", attackerIsFlagCarrier);
							if (victimIsFlagCarrier) {
								SQLBIND(insertAngleStatement, int, "@victimFlagHoldTime", recentFlagHoldTimes[target]);
								//SQLBIND(insertAngleStatement, int, "@flagPickupSource", teamInfo[victimFlagTeam].flagHoldOrigin);
								SQLBIND(insertAngleStatement, int, "@victimFlagPickupSource", victimCarrierLastPickupOrigin);
								SQLBIND(insertAngleStatement, int, "@victimCapperKills", victimFlagCarrierKillCount);
								SQLBIND(insertAngleStatement, int, "@victimCapperRets", victimFlagCarrierRetCount);
								SQLBIND(insertAngleStatement, int, "@victimCapperWasFollowedOrVisible", capperWasVisibleOrFollowed);

								SQLBIND(insertAngleStatement, double, "@victimCapperMaxNearbyEnemyCount", maxNearbyEnemyCount);
								SQLBIND(insertAngleStatement, double, "@victimCapperMoreThanOneNearbyEnemyTimePercent", moreThanOneNearbyEnemyTimePercent);
								SQLBIND(insertAngleStatement, double, "@victimCapperAverageNearbyEnemyCount", averageNearbyEnemyCount);
								SQLBIND(insertAngleStatement, double, "@victimCapperMaxVeryCloseEnemyCount", maxVeryCloseEnemyCount);
								SQLBIND(insertAngleStatement, double, "@victimCapperAnyVeryCloseEnemyTimePercent", anyVeryCloseEnemyTimePercent);
								SQLBIND(insertAngleStatement, double, "@victimCapperMoreThanOneVeryCloseEnemyTimePercent", moreThanOneVeryCloseEnemyTimePercent);
								SQLBIND(insertAngleStatement, double, "@victimCapperAverageVeryCloseEnemyCount", averageVeryCloseEnemyCount);
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@victimFlagHoldTime");
								SQLBIND_NULL(insertAngleStatement, "@victimFlagPickupSource");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperKills");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperRets");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperWasFollowedOrVisible");

								SQLBIND_NULL(insertAngleStatement, "@victimCapperMaxNearbyEnemyCount");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperMoreThanOneNearbyEnemyTimePercent");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperAverageNearbyEnemyCount");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperMaxVeryCloseEnemyCount");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperAnyVeryCloseEnemyTimePercent");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperMoreThanOneVeryCloseEnemyTimePercent");
								SQLBIND_NULL(insertAngleStatement, "@victimCapperAverageVeryCloseEnemyCount");
							}
							SQLBIND(insertAngleStatement, int, "@targetIsVisible", targetIsVisible);
							SQLBIND(insertAngleStatement, int, "@targetIsFollowed", targetIsFollowed);
							SQLBIND(insertAngleStatement, int, "@targetIsFollowedOrVisible", targetIsVisibleOrFollowed);
							SQLBIND(insertAngleStatement, int, "@isSuicide", isSuicide);
							SQLBIND(insertAngleStatement, int, "@isModSuicide", mod == MOD_SUICIDE);
							SQLBIND(insertAngleStatement, int, "@attackerIsVisible", attackerIsVisible);
							SQLBIND(insertAngleStatement, int, "@attackerIsFollowed", attackerIsFollowed);
							SQLBIND(insertAngleStatement, int, "@attackerIsFollowedOrVisible", attackerIsVisibleOrFollowed);
							SQLBIND(insertAngleStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);

							SQLBIND_TEXT(insertAngleStatement, "@boosts", (boostCountAttacker + boostCountVictim) > 0 ? boostsString.c_str() : NULL);
							SQLBIND(insertAngleStatement, int, "@boostCountTotal", boostCountAttacker + boostCountVictim);
							SQLBIND(insertAngleStatement, int, "@boostCountAttacker", boostCountAttacker);
							SQLBIND(insertAngleStatement, int, "@boostCountVictim", boostCountVictim);

							if (canBeAirborne) {
								if (killerProjectile == -1) {
									SQLBIND_NULL(insertAngleStatement, "@projectileWasAirborne");
								}
								else {
									SQLBIND(insertAngleStatement, int, "@projectileWasAirborne", projectileWasAirborne);
								}
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@projectileWasAirborne");
							}


							SQLBIND(insertAngleStatement, double, "@maxAngularSpeedAttacker", maxAngularSpeedAttackerFloat >= 0 ? maxAngularSpeedAttackerFloat : NULL);
							SQLBIND(insertAngleStatement, double, "@maxAngularAccelerationAttacker", maxAngularAccelerationAttackerFloat >= 0 ? maxAngularAccelerationAttackerFloat * accelerationMultiplier : NULL);
							SQLBIND(insertAngleStatement, double, "@maxAngularJerkAttacker", maxAngularJerkAttackerFloat >= 0 ? maxAngularJerkAttackerFloat * jerkMultiplier : NULL);
							SQLBIND(insertAngleStatement, double, "@maxAngularSnapAttacker", maxAngularSnapAttackerFloat >= 0 ? maxAngularSnapAttackerFloat *snapMultiplier : NULL); 


							SQLBIND(insertAngleStatement, double, "@maxSpeedAttacker", maxSpeedAttackerFloat >= 0 ? maxSpeedAttackerFloat : NULL);
							SQLBIND(insertAngleStatement, double, "@maxSpeedTarget", maxSpeedTargetFloat >= 0 ? maxSpeedTargetFloat : NULL);
							SQLBIND(insertAngleStatement, double, "@lastSaberMoveChangeSpeed", thisKill.speedatSaberMoveChange >= 0 ? thisKill.speedatSaberMoveChange : NULL);
							SQLBIND(insertAngleStatement, int, "@timeSinceLastSaberMoveChange", thisKill.timeSinceSaberMoveChange >= 0 ? thisKill.timeSinceSaberMoveChange : NULL);
							if (thisKill.timeSinceBackflip >= 0) {
								SQLBIND(insertAngleStatement, int, "@timeSinceLastBackflip",thisKill.timeSinceBackflip);
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@timeSinceLastBackflip");
							}
							SQLBIND_TEXT(insertAngleStatement, "@meansOfDeathString", modString);
							SQLBIND_TEXT(insertAngleStatement, "@nearbyPlayers", thisKill.nearbyPlayers.size() > 0? nearbyPlayersString.c_str():NULL);
							SQLBIND(insertAngleStatement, int, "@nearbyPlayerCount", thisKill.nearbyPlayers.size());
							SQLBIND(insertAngleStatement, int, "@probableKillingWeapon", probableKillingWeapon);
							if (attackerIsFollowed) {
								SQLBIND(insertAngleStatement, double, "@directionX", demo.cut.Cl.snap.ps.velocity[0]);
								SQLBIND(insertAngleStatement, double, "@directionY", demo.cut.Cl.snap.ps.velocity[1]);
								SQLBIND(insertAngleStatement, double, "@directionZ", demo.cut.Cl.snap.ps.velocity[2]);
								SQLBIND(insertAngleStatement, double, "@currentSpeedAttacker", VectorLength(demo.cut.Cl.snap.ps.velocity));
							} else if(attackerEntity){
								SQLBIND(insertAngleStatement, double, "@directionX", attackerEntity->pos.trDelta[0]);
								SQLBIND(insertAngleStatement, double, "@directionY", attackerEntity->pos.trDelta[1]);
								SQLBIND(insertAngleStatement, double, "@directionZ", attackerEntity->pos.trDelta[2]);
								SQLBIND(insertAngleStatement, double, "@currentSpeedAttacker", VectorLength(attackerEntity->pos.trDelta));
							} else {
								SQLBIND_NULL(insertAngleStatement,  "@directionX");
								SQLBIND_NULL(insertAngleStatement,  "@directionY");
								SQLBIND_NULL(insertAngleStatement,  "@directionZ");
								SQLBIND_NULL(insertAngleStatement,  "@currentSpeedAttacker");
							}
							if (targetIsFollowed) {
								SQLBIND(insertAngleStatement, double, "@currentSpeedTarget", VectorLength(demo.cut.Cl.snap.ps.velocity));
							}
							else if (targetEntity) {
								SQLBIND(insertAngleStatement, double, "@currentSpeedTarget", VectorLength(targetEntity->pos.trDelta));
							}
							else {
								SQLBIND_NULL(insertAngleStatement, "@currentSpeedTarget");
							}
							SQLBIND_TEXT(insertAngleStatement, "@demoName", oldBasename.c_str());
							SQLBIND_TEXT(insertAngleStatement, "@demoPath", oldPath.c_str());
							SQLBIND(insertAngleStatement, int, "@demoTime", demoCurrentTime);
							SQLBIND(insertAngleStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND(insertAngleStatement, int, "@demoDateTime", oldDemoDateModified);

							queryResult = sqlite3_step(insertAngleStatement);
							if (queryResult != SQLITE_DONE) {
								std::cout<< "Error inserting kill angle into database: " << sqlite3_errmsg(killDb) <<"\n";
							}
							sqlite3_reset(insertAngleStatement);



							//if (isSuicide || !victimIsFlagCarrier || isWorldKill || !targetIsVisible) continue; // Not that interesting.
							if (isSuicide || !victimIsFlagCarrier || isWorldKill || !targetIsVisibleOrFollowed) continue; // Not that interesting.
							// If it's not a doom kill, it's not that interesting unless we specifically are searching for our own returns or searching for everything
							if (!isDoomKill && searchMode != SEARCH_ALL && searchMode != SEARCH_MY_CTF_RETURNS && searchMode != SEARCH_CTF_RETURNS) continue;
							if (!attackerIsFollowed && searchMode == SEARCH_MY_CTF_RETURNS) continue; // We are searching for our own kills.
							if (!attackerIsVisibleOrFollowed) continue; // Attacker isn't visible at all. Meh. Leave it.


							std::stringstream ss;
							std::string boostString = ((boostCountAttacker + boostCountVictim) > 0 ?( va("_BST%s%s", boostCountAttacker > 0 ? va("%dA", boostCountAttacker) : "", boostCountVictim > 0 ? va("%dV", boostCountVictim) : "")) : "");
							ss << mapname << std::setfill('0') << "___RET" << modInfo.str() << boostString << "___" << playername << "___" << victimname << "___" << maxSpeedAttacker << "_" << maxSpeedTarget << "ups" << (attackerIsFollowed ? "" : "___thirdperson") << "_" << attacker << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "") << "_" << shorthash;

							std::string targetFilename = ss.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);


							outputBatHandle << "\nrem demoCurrentTime: " << demoCurrentTime;
							outputBatHandle << "\nrem hash: " << hash_hex_string;
							outputBatHandle << "\n" << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
							delete[] targetFilenameFiltered;
							std::cout << mapname << " " << modInfo.str() << boostString << " " << attacker << " " << target << " " << playername << " " << victimname << (isDoomKill ? " DOOM" : "") << " followed:" << attackerIsFollowed << "___" << maxSpeedAttacker << "_" << maxSpeedTarget << "ups" << "\n";

						}

						
						else if (eventNumber >= EV_PAIN && eventNumber <= EV_DEATH3) {
							// Player took some damage. Could indicate a saber hit if an EV_SABER_HIT is nearby
							int playerNum = thisEs->number;
							if (thisEs->eFlags & EF_PLAYER_EVENT) {
								playerNum = thisEs->otherEntityNum;
							}
							if (playerNum >= 0 && playerNum < max_clients) {
								hitDetectionData[playerNum].painDetected = qtrue;
							}
						}
					
						else if (eventNumber == EV_SABER_HIT && thisEs->eventParm == 1) { // Saber hit against client
							
							// Mark nearby players for potential saber hit detection
							for (int playerNum = 0; playerNum < max_clients; playerNum++) {
								constexpr float maxDistance = 100.0f + 2.0f * (float)SABER_LENGTH_MAX;
								if (thisFrameInfo.entityExists[playerNum] && VectorDistance(thisFrameInfo.playerPositions[playerNum], thisEs->pos.trBase) <= maxDistance) {
									hitDetectionData[playerNum].nearbySaberHitDetected = qtrue;
								}
							}
						}
						else if (eventNumber == EV_SHIELD_HIT) {
							int playerNum = thisEs->otherEntityNum;
							if (playerNum >= 0 && playerNum < max_clients) {
								hitDetectionData[playerNum].confirmedHit = qtrue;
							}
						}
						else if (eventNumber == EV_CTFMESSAGE && thisEs->eventParm == CTFMESSAGE_PLAYER_GOT_FLAG) {
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;
							// A bit pointless tbh because we reset it to -1 anyway before checking entities. 
							// Let me rethink this some day TODO
							if (flagTeam == TEAM_RED) {
								lastKnownRedFlagCarrier = playerNum;
							}else if (flagTeam == TEAM_BLUE) {
								lastKnownBlueFlagCarrier = playerNum;
							}
						}
						else if (eventNumber == EV_CTFMESSAGE && thisEs->eventParm == CTFMESSAGE_PLAYER_CAPTURED_FLAG) {
							//Capture.
							int playerNum = thisEs->trickedentindex;
							int flagTeam = thisEs->trickedentindex2;

							int offset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
							const char* info = demo.cut.Cl.gameState.stringData + offset;
							std::string mapname = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "mapname");
							std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - offset, "sv_hostname");
							std::string playername = "WEIRDONAME";
							const char* playerInfo;
							if (playerNum >= 0 && playerNum < max_clients) {
								offset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + playerNum];
								playerInfo = demo.cut.Cl.gameState.stringData + offset;
								playername = Info_ValueForKey(playerInfo, sizeof(demo.cut.Cl.gameState.stringData) - offset, "n");
							}

							bool playerIsVisible = false;
							entityState_t* playerEntity = findEntity(playerNum);
							if (playerEntity) {
								playerIsVisible = true;
							}
							bool playerIsFollowed = demo.cut.Cl.snap.ps.clientNum == playerNum;
							bool playerIsVisibleOrFollowed = playerIsFollowed || playerIsVisible;

							int flagHoldTime = recentFlagHoldTimes[playerNum];

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

							vec3_t currentPos;
							vec3_t currentDir;

							// Find nearby players.
							std::stringstream nearbyPlayersSS;
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

							// Stats about speed
							if (recentFlagHoldVariousInfo[playerNum].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[playerNum]) {
								// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
								Com_Memset(&recentFlagHoldVariousInfo[playerNum], 0, sizeof(VariousCappingInfo));
								recentFlagHoldVariousInfo[playerNum].lastUpdateTime = demoCurrentTime;
							}
							float maxSpeedCapper = recentFlagHoldVariousInfo[playerNum].maxSpeedThisRun;
							float averageSpeedCapper = recentFlagHoldVariousInfo[playerNum].divisorSpeeds == 0 ? 0 : recentFlagHoldVariousInfo[playerNum].sumSpeeds/recentFlagHoldVariousInfo[playerNum].divisorSpeeds;


							// Find nearby enemies
							std::stringstream nearbyEnemiesSS;
							int nearbyEnemiescount = 0;
							for (int near = 0; near < nearbyPlayers.size(); near++) {
								int nearbyPlayerHere = nearbyPlayers[near];
								if (playerTeams[nearbyPlayerHere] != playerTeams[playerNum]) {
									nearbyEnemiesSS << (nearbyEnemiescount++ == 0 ? "" : ",") << nearbyPlayerHere << " (" << (int)nearbyPlayersDistances[near] << ")";
								}
							}
							std::string nearbyEnemiesString = nearbyEnemiesSS.str();


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
							int victimCarrierLastPickupOrigin = -1;// victimIsFlagCarrier ? (attacker == lastKnownBlueFlagCarrier ? cgs.blueFlagLastPickupOrigin : cgs.redFlagLastPickupOrigin) : -1;
							switch (flagTeam) {
								case TEAM_FREE: // Is this correct?
									victimCarrierLastPickupOrigin = cgs.yellowflagLastPickupOrigin;
									break;
								case TEAM_RED:
									victimCarrierLastPickupOrigin = cgs.redFlagLastPickupOrigin;
									break;
								case TEAM_BLUE:
									victimCarrierLastPickupOrigin = cgs.blueFlagLastPickupOrigin;
									break;
							}

							SQLBIND_TEXT(insertCaptureStatement, "@map", mapname.c_str());
							SQLBIND_TEXT(insertCaptureStatement, "@serverName", serverName.c_str());
							std::string serverNameStripped = Q_StripColorAll(serverName);
							SQLBIND_TEXT(insertCaptureStatement, "@serverNameStripped", serverNameStripped.c_str());
							SQLBIND(insertCaptureStatement, int, "@flagHoldTime", flagHoldTime);
							//SQLBIND(insertCaptureStatement, int, "@flagPickupSource", teamInfo[flagTeam].flagHoldOrigin);
							SQLBIND(insertCaptureStatement, int, "@flagPickupSource", victimCarrierLastPickupOrigin);
							SQLBIND_TEXT(insertCaptureStatement, "@capperName", playername.c_str());
							std::string playernameStripped = Q_StripColorAll(playername);
							SQLBIND_TEXT(insertCaptureStatement, "@capperNameStripped", playernameStripped.c_str());
							SQLBIND(insertCaptureStatement, int, "@capperClientNum", playerNum);
							SQLBIND(insertCaptureStatement, int, "@capperIsVisible", playerIsVisible);
							SQLBIND(insertCaptureStatement, int, "@capperIsFollowed", playerIsFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperIsFollowedOrVisible", playerIsVisibleOrFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperWasVisible", wasVisible);
							SQLBIND(insertCaptureStatement, int, "@capperWasFollowed", wasFollowed);
							SQLBIND(insertCaptureStatement, int, "@capperWasFollowedOrVisible", wasVisibleOrFollowed);
							SQLBIND(insertCaptureStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
							SQLBIND(insertCaptureStatement, int, "@flagTeam", flagTeam);
							SQLBIND(insertCaptureStatement, int, "@capperKills", flagCarrierKillCount);
							SQLBIND(insertCaptureStatement, int, "@capperRets", flagCarrierRetCount);
							SQLBIND(insertCaptureStatement, int, "@redScore", teamInfo[TEAM_RED].score);
							SQLBIND(insertCaptureStatement, int, "@blueScore", teamInfo[TEAM_BLUE].score);
							SQLBIND(insertCaptureStatement, int, "@redPlayerCount", teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertCaptureStatement, int, "@bluePlayerCount", teamInfo[TEAM_BLUE].playerCount);
							SQLBIND(insertCaptureStatement, int, "@sumPlayerCount", teamInfo[TEAM_FREE].playerCount + teamInfo[TEAM_BLUE].playerCount+teamInfo[TEAM_RED].playerCount);
							SQLBIND(insertCaptureStatement, double, "@maxSpeedCapperLastSecond", maxSpeedCapperLastSecond);
							SQLBIND(insertCaptureStatement, double, "@maxSpeedCapper", maxSpeedCapper);
							SQLBIND(insertCaptureStatement, double, "@averageSpeedCapper", averageSpeedCapper);
							SQLBIND_TEXT(insertCaptureStatement, "@nearbyPlayers", nearbyPlayersString.c_str());
							SQLBIND(insertCaptureStatement, int, "@nearbyPlayerCount", nearbyPlayersCount);
							SQLBIND_TEXT(insertCaptureStatement, "@nearbyEnemies", nearbyEnemiesString.c_str());
							SQLBIND(insertCaptureStatement, int, "@nearbyEnemyCount", nearbyEnemiescount);

							SQLBIND(insertCaptureStatement, double, "@maxNearbyEnemyCount", maxNearbyEnemyCount);
							SQLBIND(insertCaptureStatement, double, "@moreThanOneNearbyEnemyTimePercent", moreThanOneNearbyEnemyTimePercent);
							SQLBIND(insertCaptureStatement, double, "@averageNearbyEnemyCount", averageNearbyEnemyCount);
							SQLBIND(insertCaptureStatement, double, "@maxVeryCloseEnemyCount", maxVeryCloseEnemyCount);
							SQLBIND(insertCaptureStatement, double, "@anyVeryCloseEnemyTimePercent", anyVeryCloseEnemyTimePercent);
							SQLBIND(insertCaptureStatement, double, "@moreThanOneVeryCloseEnemyTimePercent", moreThanOneVeryCloseEnemyTimePercent);
							SQLBIND(insertCaptureStatement, double, "@averageVeryCloseEnemyCount", averageVeryCloseEnemyCount);

							if (playerIsVisibleOrFollowed) {
								SQLBIND(insertCaptureStatement, double, "@positionX", currentPos[0]);
								SQLBIND(insertCaptureStatement, double, "@positionY", currentPos[1]);
								SQLBIND(insertCaptureStatement, double, "@positionZ", currentPos[2]);
								SQLBIND(insertCaptureStatement, double, "@directionX", currentDir[0]);
								SQLBIND(insertCaptureStatement, double, "@directionY", currentDir[1]);
								SQLBIND(insertCaptureStatement, double, "@directionZ", currentDir[2]);
							}
							else {
								SQLBIND_NULL(insertCaptureStatement,  "@positionX");
								SQLBIND_NULL(insertCaptureStatement,  "@positionY");
								SQLBIND_NULL(insertCaptureStatement,  "@positionZ");
								SQLBIND_NULL(insertCaptureStatement,  "@directionX");
								SQLBIND_NULL(insertCaptureStatement,  "@directionY");
								SQLBIND_NULL(insertCaptureStatement,  "@directionZ");
							}
							SQLBIND_TEXT(insertCaptureStatement, "@demoName", oldBasename.c_str());
							SQLBIND_TEXT(insertCaptureStatement, "@demoPath", oldPath.c_str());
							SQLBIND(insertCaptureStatement, int, "@demoTime", demoCurrentTime);
							SQLBIND(insertCaptureStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
							SQLBIND(insertCaptureStatement, int, "@demoDateTime", oldDemoDateModified);

							int queryResult = sqlite3_step(insertCaptureStatement);
							uint64_t insertedId = -1;
							if (queryResult != SQLITE_DONE) {
								std::cout << "Error inserting capture into database: " << sqlite3_errmsg(killDb) << "\n";
							}
							else {
								queryResult = sqlite3_step(selectLastInsertRowIdStatement);
								if (queryResult != SQLITE_DONE && queryResult != SQLITE_ROW) {
									std::cout << "Error retrieving inserted capture id from database: " << sqlite3_errmsg(killDb) << "\n";
								}
								else {
									insertedId = sqlite3_column_int64(selectLastInsertRowIdStatement,0);
								}
								sqlite3_reset(selectLastInsertRowIdStatement);
							}
							sqlite3_reset(insertCaptureStatement);

							if (!playerIsVisibleOrFollowed && !wasVisibleOrFollowed) continue; // No need to cut out those who were not visible at all in any way.
							if (searchMode == SEARCH_MY_CTF_RETURNS && playerNum != demo.cut.Cl.snap.ps.clientNum) continue; // Only cut your own for SEARCH_MY_CTF_RETURNS

							int runStart = demoCurrentTime - flagHoldTime;
							int startTime = runStart - bufferTime;
							int endTime = demoCurrentTime + bufferTime;
							int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
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
							ss << mapname << std::setfill('0') << "___CAPTURE"<<(flagCarrierKillCount>0 ? va("%dK", flagCarrierKillCount):"")<<(flagCarrierRetCount>0 ? va("%dR", flagCarrierRetCount):"") << "___" << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << "___" << playername << "___P"<< victimCarrierLastPickupOrigin <<"T"<<flagTeam<< "___"<< (int)moreThanOneVeryCloseEnemyTimePercent<<"DANGER"<<(int)(averageVeryCloseEnemyCount*100)<<"___"<<(int) maxSpeedCapper<<"_"<<(int)averageSpeedCapper<<"ups" << (wasFollowed ? "" : (wasVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" << playerNum << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

							std::string targetFilename = ss.str();
							char* targetFilenameFiltered = new char[targetFilename.length() + 1];
							sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

							outputBatHandleCaptures << "\nrem demoCurrentTime: " << demoCurrentTime;
							outputBatHandleCaptures << "\nrem insertid" << insertedId;
							outputBatHandleCaptures << "\n" << (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \"" << sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
							delete[] targetFilenameFiltered;
							std::cout << targetFilename << "\n";


						}
					}
				}


				// Check for killstreaks.
				// Anything happening within KILLSTREAK_MAX_INTERVAL milliseconds of last kill counts towards it.
				// Baseline here is 3 seconds, since that is the limit for kill messages to be displayed as one in stacked kill messages.
				// TODO Also count stuff that maybe isn't within 3 secs but pretty darn close?
				// KILLSTREAK_MIN_KILLS 3
				// KILLSTREAK_MAX_INTERVAL 3000
				// TODO Let it count chains of killstreaks. If there's already a valid killstreak, let there be a single longer gap there.


				for (auto clientIt = kills.begin(); clientIt != kills.end(); clientIt++) {

					static const int killStreakSpeedTypes[] = { KILLSTREAK_MAX_INTERVAL,SLOW_KILLSTREAK_MAX_INTERVAL,VERYSLOW_KILLSTREAK_MAX_INTERVAL };
					constexpr int spCount = sizeof(killStreakSpeedTypes) / sizeof(int);

					int clientNumAttacker = clientIt->first;

					if (clientIt->second.size() == 0 || (clientIt->second.back().time + killStreakSpeedTypes[spCount-1]) >= demoCurrentTime) continue; // Might still have an unfinished one here!


					for (int sp = 0; sp < spCount; sp++) { // We do this twice, for every killstreak duration type.

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

						for (int i = 0; i < clientIt->second.size(); i++) {
							Kill* thisKill = &clientIt->second[i];

							// Whether it was or wasn't a killstreak, there are no killstreaks up to this point, so earlier than current kill time can be safely cleaned up.
							timeCheckedForKillStreaks[clientNumAttacker] = thisKill->time - 1;

							if (thisKill->time <= timeCheckedForKillStreaks[clientNumAttacker]) continue; // This one's already been registered.
							if (thisKill->isSuicide || !thisKill->isVisible) continue; // Uninteresting.


							// Starting or continuing kill spree?
							if (spreeInfo.countKills == 0 || thisKill->time <= (spreeInfo.lastKillTime + maxTimeHere)) {
							
								if (lastKillHash && *lastKillHash == thisKill->hashSourceString) continue; // Weird duplicated kill from lost packets likely, identical hash to last one.
								if (sp > 0  && (spreeInfo.countKills ==0 || thisKill->time <= (spreeInfo.lastKillTime + maxTimeLast))) countFittingInLastBracket++; // Measure how many of the kills would have fit in a faster bracket. If ALL of them would have, we discard this and just count it the faster bracket variant.
								victims.push_back(thisKill->targetClientNum);
								hashes.push_back(thisKill->hash);
								killsOfThisSpree.push_back(*thisKill);
								if (thisKill->isDoom) spreeInfo.countDooms++;
								if (thisKill->isRet) spreeInfo.countRets++;
								if (thisKill->isExplosion) spreeInfo.countExplosions++;
								if (!thisKill->isFollowed) spreeInfo.countThirdPersons++;
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
									CheckSaveKillstreak<max_clients>(maxTimeHere,&spreeInfo, clientNumAttacker, &killsOfThisSpree, &victims, &hashes, allKillsHashSS.str(), demoCurrentTime, &outputBatHandleKillSprees, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile, insertSpreeStatement, killDb, oldBasename, oldPath, oldDemoDateModified, demoType);
								}

								// Reset.
								Com_Memset(&spreeInfo, 0, sizeof(SpreeInfo));
								victims.clear();
								hashes.clear();
								killsOfThisSpree.clear();
								allKillsHashSS.str(std::string());
								countFittingInLastBracket = 0;
							}
						}
						if ( countFittingInLastBracket < spreeInfo.countKills) { // If all of the kills would fit in a faster bracket (like for example delay 3000 instead of delay 5000) we don't count this one and only count the faster one. To avoid pointless dupes.
							CheckSaveKillstreak<max_clients>(maxTimeHere, &spreeInfo, clientNumAttacker, &killsOfThisSpree, &victims, &hashes, allKillsHashSS.str(), demoCurrentTime, &outputBatHandleKillSprees, bufferTime, lastGameStateChangeInDemoTime, sourceDemoFile, insertSpreeStatement, killDb, oldBasename, oldPath, oldDemoDateModified, demoType);
						}

					}
					// Clean up old kills that no longer have to be stored
					// We can clear the entire thing since earlier we made sure that we aren't in the middle of an ongoing killstreak
					// So anything that's in there now was already checked for being part of a killstreak.
					// So we can get rid of it all.
					clientIt->second.clear();
				
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
					int countToRemove = 0;
					for (int s = 0; s < speeds[clientNumAttacker].size();s++) {
						if ((demoCurrentTime - speeds[clientNumAttacker][s].time) > OLDER_SPEEDS_STORE_LIMIT) {
							countToRemove++;
						}
					}
					if (countToRemove) {
						speeds[clientNumAttacker].erase(speeds[clientNumAttacker].begin(), speeds[clientNumAttacker].begin()+ countToRemove);
					}
				}


				// Find out which players are visible / followed
				// Also find out if any visible player is carrying the flag. (we do this after events so we always have the value from the last snap up there, bc dead entities no longer hold the flag)
				lastKnownBlueFlagCarrier = lastKnownRedFlagCarrier = -1;
				vec3_t lastKnownBlueFlagCarrierPosition, lastKnownRedFlagCarrierPosition;
				vec3_t lastKnownBlueFlagCarrierVelocity, lastKnownRedFlagCarrierVelocity;
				for (int p = 0; p < max_clients; p++) {
					// Go through parseenttities of last snap to see if client is in it
					bool clientIsInSnapshot = false;
					bool clientVisibleOrFollowed = false;
					// TODO This is kind of messy uuuh... fix it?!
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum+demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						if (thisEntity->number == p) {
							clientIsInSnapshot = true;

							if (thisEntity->powerups & (1 << PW_REDFLAG)) {
								lastKnownRedFlagCarrier = thisEntity->number;
								VectorCopy(thisEntity->pos.trBase,lastKnownRedFlagCarrierPosition);
								VectorCopy(thisEntity->pos.trDelta,lastKnownRedFlagCarrierVelocity);
								recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime - cgs.redFlagLastChangeToTaken;
							}
							else if (thisEntity->powerups & (1 << PW_BLUEFLAG)) {
								lastKnownBlueFlagCarrier = thisEntity->number;
								VectorCopy(thisEntity->pos.trBase, lastKnownBlueFlagCarrierPosition);
								VectorCopy(thisEntity->pos.trDelta, lastKnownBlueFlagCarrierVelocity);
								recentFlagHoldTimes[lastKnownBlueFlagCarrier] = demoCurrentTime - cgs.blueFlagLastChangeToTaken;
							}
						}
					}
					if (demo.cut.Cl.snap.ps.powerups[PW_REDFLAG]) {

						lastKnownRedFlagCarrier = demo.cut.Cl.snap.ps.clientNum;
						VectorCopy(demo.cut.Cl.snap.ps.origin, lastKnownRedFlagCarrierPosition);
						VectorCopy(demo.cut.Cl.snap.ps.velocity, lastKnownRedFlagCarrierVelocity);
						recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime - cgs.redFlagLastChangeToTaken;
					}else if (demo.cut.Cl.snap.ps.powerups[PW_BLUEFLAG]) {

						lastKnownBlueFlagCarrier = demo.cut.Cl.snap.ps.clientNum;
						VectorCopy(demo.cut.Cl.snap.ps.origin, lastKnownBlueFlagCarrierPosition);
						VectorCopy(demo.cut.Cl.snap.ps.velocity, lastKnownBlueFlagCarrierVelocity);
						recentFlagHoldTimes[lastKnownBlueFlagCarrier] = demoCurrentTime - cgs.blueFlagLastChangeToTaken;
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
						playerFirstFollowed[p] = -1;
					}
					if (clientVisibleOrFollowed) {
						if (playerFirstFollowedOrVisible[p] == -1) {
							playerFirstFollowedOrVisible[p] = demo.cut.Cl.snap.serverTime;
						}
					}
					else {
						playerFirstFollowedOrVisible[p] = -1;
					}
				}

				// save maxspeed of flag hold
				if (lastKnownRedFlagCarrier != -1) {
					if (recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownRedFlagCarrier]) {
						// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
						Com_Memset(&recentFlagHoldVariousInfo[lastKnownRedFlagCarrier], 0, sizeof(VariousCappingInfo));
					}
					float speedHere = VectorLength(lastKnownRedFlagCarrierVelocity);
					if (recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].maxSpeedThisRun < speedHere) {
						recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].maxSpeedThisRun = speedHere;
					}
					recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].sumSpeeds += speedHere* deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].divisorSpeeds += deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownRedFlagCarrier].lastUpdateTime = demoCurrentTime;
				}
				if (lastKnownBlueFlagCarrier != -1) {
					if (recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownBlueFlagCarrier]) {
						// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
						Com_Memset(&recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier], 0, sizeof(VariousCappingInfo));
					}
					float speedHere = VectorLength(lastKnownBlueFlagCarrierVelocity);
					if (recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].maxSpeedThisRun < speedHere) {
						recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].maxSpeedThisRun = speedHere;
					}
					recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].sumSpeeds += speedHere * deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].divisorSpeeds += deltaTimeFromLastSnapshot;
					recentFlagHoldVariousInfo[lastKnownBlueFlagCarrier].lastUpdateTime = demoCurrentTime;
				}


				// Check amount of enemies nearby cappers and log it in case we need it later.
				if (lastKnownBlueFlagCarrier!=-1 || lastKnownRedFlagCarrier!=-1) { // Don't bother if no flag carrier is visible at all
					int enemiesNearRedCarrier =0, enemiesNearBlueCarrier = 0;
					int enemiesVeryCloseRedCarrier =0, enemiesVeryCloseBlueCarrier = 0;

					
					for (int pe = demo.cut.Cl.snap.parseEntitiesNum; pe < demo.cut.Cl.snap.parseEntitiesNum + demo.cut.Cl.snap.numEntities; pe++) {
						entityState_t* thisEntity = &demo.cut.Cl.parseEntities[pe & (MAX_PARSE_ENTITIES - 1)];
						if (thisEntity->number < 0 || thisEntity->number >= max_clients || playerTeams[thisEntity->number]==TEAM_SPECTATOR) continue;
						if (lastKnownBlueFlagCarrier != -1 && playerTeams[thisEntity->number] == TEAM_BLUE) {
							float distance = VectorDistance(lastKnownBlueFlagCarrierPosition,thisEntity->pos.trBase);
							if (distance <= NEARBY_PLAYER_MAX_DISTANCE) {
								enemiesNearBlueCarrier++;
							}
							if (distance <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
								enemiesVeryCloseBlueCarrier++;
							}
						}
						if (lastKnownRedFlagCarrier != -1 && playerTeams[thisEntity->number] == TEAM_RED){
							float distance = VectorDistance(lastKnownRedFlagCarrierPosition, thisEntity->pos.trBase);
							if (distance <= NEARBY_PLAYER_MAX_DISTANCE) {
								enemiesNearRedCarrier++;
							}
							if (distance <= VERYCLOSE_PLAYER_MAX_DISTANCE) {
								enemiesVeryCloseRedCarrier++;
							}
						}
					}

					// Resets if necessary.
					if (lastKnownBlueFlagCarrier != -1) {
						if (recentFlagHoldEnemyNearbyTimes[lastKnownBlueFlagCarrier].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownBlueFlagCarrier]) {
							// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
							Com_Memset(&recentFlagHoldEnemyNearbyTimes[lastKnownBlueFlagCarrier], 0, sizeof(EnemyNearbyInfo));
						}
						recentFlagHoldEnemyNearbyTimes[lastKnownBlueFlagCarrier].enemyNearbyTimes[enemiesNearBlueCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownBlueFlagCarrier].enemyVeryCloseTimes[enemiesVeryCloseBlueCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownBlueFlagCarrier].lastUpdateTime = demoCurrentTime;
					}
					if (lastKnownRedFlagCarrier != -1) {
						if (recentFlagHoldEnemyNearbyTimes[lastKnownRedFlagCarrier].lastUpdateTime < demoCurrentTime - recentFlagHoldTimes[lastKnownRedFlagCarrier]) {
							// If the last nearby enemy info of this capper was before he even got the flag, reset before adding to the count
							Com_Memset(&recentFlagHoldEnemyNearbyTimes[lastKnownRedFlagCarrier], 0, sizeof(EnemyNearbyInfo));
						}
						recentFlagHoldEnemyNearbyTimes[lastKnownRedFlagCarrier].enemyNearbyTimes[enemiesNearRedCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownRedFlagCarrier].enemyVeryCloseTimes[enemiesVeryCloseRedCarrier] += deltaTimeFromLastSnapshot;
						recentFlagHoldEnemyNearbyTimes[lastKnownRedFlagCarrier].lastUpdateTime = demoCurrentTime;
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
						&& playerLastSaberMove[playerNum].lastSaberMove[1].saberMove >= LS_PARRY_UP && playerLastSaberMove[playerNum].lastSaberMove[1].saberMove <= LS_PARRY_LL // last one was parry
						&& playerLastSaberMove[playerNum].lastSaberMove[0].saberMove == saberMoveData[playerLastSaberMove[playerNum].lastSaberMove[1].saberMove].chain_attack // this one is the result if you click atack
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
		}

		// Check for finished laugh sequences
		checkSaveLaughs(demoCurrentTime,bufferTime, lastGameStateChangeInDemoTime,&outputBatHandleLaughs,killDb,insertLaughsStatement,selectLastInsertRowIdStatement,&oldBasename,&oldPath,oldDemoDateModified,sourceDemoFile,qfalse);


		int firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;

		bool hadConfigStringCommands = false;
		// process any new server commands
		for (; demo.cut.Clc.lastExecutedServerCommand <= demo.cut.Clc.serverCommandSequence; demo.cut.Clc.lastExecutedServerCommand++) {
			char* command = demo.cut.Clc.serverCommands[demo.cut.Clc.lastExecutedServerCommand & (MAX_RELIABLE_COMMANDS - 1)];
			Cmd_TokenizeString(command);
			char* cmd = Cmd_Argv(0);
			if (cmd[0]) {
				firstServerCommand = demo.cut.Clc.lastExecutedServerCommand;
			}

			if (!strcmp(cmd, "chat") || !strcmp(cmd, "tchat")) {
				std::string chatCommand = command;
				chatCommand = Q_StripColorAll(chatCommand);

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
				}

				// If we are in the middle of a laugh sequence, we temporarily log the entire chat no matter what.
				if (firstLaugh != -1 && demoCurrentTime - firstLaugh <= MAX_LAUGH_DELAY) {
					laughsChatlog << command << "\n";
				}
			}
			else if (!strcmp(cmd, "cs")) {
				if (!demoCutConfigstringModified(&demo.cut.Cl,demoType)) {
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

					if (cgs.redflag != redflagTmp) {
						/* // Actually, we can't do this here bc lastknownredflagcarrier might not be actually known.
						if (redflagTmp == 1) { // Was just picked up. Set flag hold time to 0.
							recentFlagHoldTimes[lastKnownRedFlagCarrier] = 0;
						}
						if (cgs.redflag == 1) { // Was taken before. So just count the flag hold time.
							recentFlagHoldTimes[lastKnownRedFlagCarrier] = demoCurrentTime-cgs.redFlagLastChange;
						}*/
						cgs.redFlagLastChange = demoCurrentTime;
						if (redflagTmp == 1) {
							cgs.redFlagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_RED].flagHoldOrigin = cgs.redflag;
							cgs.redFlagLastPickupOrigin = cgs.redflag;
						}
					}
					if (cgs.blueflag != blueflagTmp) {
						cgs.blueFlagLastChange = demoCurrentTime;
						if (blueflagTmp == 1) {
							cgs.blueFlagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_BLUE].flagHoldOrigin = cgs.blueflag;
							cgs.blueFlagLastPickupOrigin = cgs.blueflag;
						}
					}
					if (cgs.yellowflag != yellowflagTmp) {
						cgs.yellowflagLastChange = demoCurrentTime;
						if (yellowflagTmp == 1) {
							cgs.yellowflagLastChangeToTaken = demoCurrentTime;
							//teamInfo[TEAM_FREE].flagHoldOrigin = cgs.yellowflag; // Not sure if this is correct?
							cgs.yellowflagLastPickupOrigin = cgs.yellowflag; // Not sure if this is correct?
						}
					}
					cgs.redflag = redflagTmp;
					cgs.blueflag = blueflagTmp;
					cgs.yellowflag = yellowflagTmp;
					/*if (cgs.isCTFMod && cgs.CTF3ModeActive)
						demo.cgs.yellowflag = str[2] - '0';
					else
						demo.cgs.yellowflag = 0;*/
				}

				hadConfigStringCommands = true;
			}
			if (!strcmp(cmd, "print")) {
				//Looking for 
				//"^2[^7OC-System^2]: bizzle^7 has finished in [^200:24.860^7] which is his personal best time. ^2Top10 time!^7 Difference to best: [^200:00.000^7]."

				// regex: \^2\[\^7OC-System\^2\]: (.*?)\^7 has finished in \[\^2(\d+:\d+.\d+)\^7\] which is his personal best time.( \^2Top10 time!\^7)? Difference to best: \[\^200:00.000\^7\]\.
				

				//jp::VecNum vec_num;
				//jp::RegexMatch rm;
				qboolean runFound = qfalse;
				defragRunInfo_t runInfo;

				std::string printText = Cmd_Argv(1);
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

				if (findOCDefragRun(printText, &runInfo)) {
					runFound = qtrue;
				} else if (findRazorDefragRun(printText, &runInfo)) {
					runFound = qtrue;
				}


				//for (int matchNum = 0; matchNum < vec_num.size(); matchNum++) { // really its just going to be 1 but whatever
				if (runFound) { // really its just going to be 1 but whatever
					int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO];
					const char * info = demo.cut.Cl.gameState.stringData + stringOffset;
					std::string mapname = Info_ValueForKey(info,sizeof(demo.cut.Cl.gameState.stringData)- stringOffset, "mapname");
					std::string serverName = Info_ValueForKey(info, sizeof(demo.cut.Cl.gameState.stringData) - stringOffset, "sv_hostname");
					//std::string playername = vec_num[matchNum][1];
					std::string playername = runInfo.playerName;
					//int minutes = atoi(vec_num[matchNum][2].c_str());
					//std::string secondString = vec_num[matchNum][3];
					//float seconds = atof(vec_num[matchNum][3].c_str());
					//int milliSeconds = (1000.0f* seconds)+0.5f;
					//int pureMilliseconds = milliSeconds % 1000;
					//int pureSeconds = milliSeconds / 1000;

					//bool isLogged = vec_num[matchNum][4].length() > 0;
					//bool isNumberOne = vec_num[matchNum][6].length() > 0;
					bool isLogged = runInfo.isLogged;
					bool isNumberOne = runInfo.isNumber1;
					bool isPersonalBest = runInfo.isPersonalBest;

					//int totalSeconds = minutes * 60 + seconds;
					//int totalMilliSeconds = minutes * 60000 + milliSeconds;
					int totalMilliSeconds = runInfo.milliseconds;

					int pureMilliseconds = totalMilliSeconds % 1000;
					int tmpSeconds = totalMilliSeconds / 1000;
					int pureSeconds = tmpSeconds % 60;
					int minutes = tmpSeconds / 60;
					
					// Find player
					int playerNumber = -1;
					for (int clientNum = 0; clientNum < max_clients; clientNum++) {

						int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_PLAYERS_here + clientNum];
						const char* playerInfo = demo.cut.Cl.gameState.stringData + stringOffset;
						std::string playerNameCompare = Info_ValueForKey(playerInfo,sizeof(demo.cut.Cl.gameState.stringData)- stringOffset, "n");
						if (playerNameCompare == playername) {
							playerNumber = clientNum;
						}
					}
					
					bool wasFollowed = false;
					bool wasVisible = false;
					bool wasVisibleOrFollowed = false;
					if (playerNumber != -1) {
						if (playerFirstFollowed[playerNumber] != -1 && playerFirstFollowed[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
							wasFollowed = true;
						}
						if (playerFirstVisible[playerNumber] != -1 && playerFirstVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
							wasVisible = true;
						}
						if (playerFirstFollowedOrVisible[playerNumber] != -1 && playerFirstFollowedOrVisible[playerNumber] < (demo.cut.Cl.snap.serverTime - totalMilliSeconds)) {
							wasVisibleOrFollowed = true;
						}
					}
					int runStart = demoCurrentTime - totalMilliSeconds;

					std::stringstream formattedTime;
					formattedTime << std::setfill('0') << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds;
					std::string formattedTimeString = formattedTime.str();

					SQLBIND_TEXT(insertDefragRunStatement, "@map", mapname.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@serverName", serverName.c_str());
					std::string serverNameStripped = Q_StripColorAll(serverName);
					SQLBIND_TEXT(insertDefragRunStatement, "@serverNameStripped", serverNameStripped.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@readableTime", formattedTimeString.c_str());
					SQLBIND(insertDefragRunStatement, int, "@totalMilliseconds", totalMilliSeconds);
					SQLBIND_TEXT(insertDefragRunStatement, "@playerName", playername.c_str());
					std::string playernameStripped = Q_StripColorAll(playername);
					SQLBIND_TEXT(insertDefragRunStatement, "@playerNameStripped", playernameStripped.c_str());
					SQLBIND(insertDefragRunStatement, int, "@isTop10", isLogged);
					SQLBIND(insertDefragRunStatement, int, "@isNumber1", isNumberOne);
					SQLBIND(insertDefragRunStatement, int, "@isPersonalBest", isPersonalBest);
					SQLBIND_TEXT(insertDefragRunStatement, "@demoName", oldBasename.c_str());
					SQLBIND_TEXT(insertDefragRunStatement, "@demoPath", oldPath.c_str());
					SQLBIND(insertDefragRunStatement, int, "@demoTime", demoCurrentTime);
					SQLBIND(insertDefragRunStatement, int, "@serverTime", demo.cut.Cl.snap.serverTime);
					SQLBIND(insertDefragRunStatement, int, "@demoDateTime", oldDemoDateModified);
					SQLBIND(insertDefragRunStatement, int, "@wasVisible", wasVisible);
					SQLBIND(insertDefragRunStatement, int, "@wasFollowed", wasFollowed);
					SQLBIND(insertDefragRunStatement, int, "@wasFollowedOrVisible", wasVisibleOrFollowed);

					// Do we have strafe deviation info?
					int64_t measurementStartTimeOffset = abs(strafeDeviationsDefrag[playerNumber].lastReset - runStart);
					if (wasVisibleOrFollowed && playerNumber != -1 && measurementStartTimeOffset < DEFRAG_STRAFEDEVIATION_SAMPLE_START_TIME_MAX_OFFSET) {
						SQLBIND(insertDefragRunStatement, double, "@averageStrafeDeviation", strafeDeviationsDefrag[playerNumber].averageHelper.sum/strafeDeviationsDefrag[playerNumber].averageHelper.divisor);
					}
					else {
						SQLBIND_NULL(insertDefragRunStatement, "@averageStrafeDeviation");
					}

					SQLBIND(insertDefragRunStatement, int, "@demoRecorderClientnum", demo.cut.Clc.clientNum);
					SQLBIND(insertDefragRunStatement, int, "@runnerClientNum", playerNumber);

					int queryResult = sqlite3_step(insertDefragRunStatement);
					if (queryResult != SQLITE_DONE) {
						std::cout << "Error inserting defrag run into database: " << sqlite3_errmsg(killDb) << "\n";
					}
					sqlite3_reset(insertDefragRunStatement);

					//if (searchMode != SEARCH_INTERESTING && searchMode != SEARCH_ALL && searchMode != SEARCH_TOP10_DEFRAG) continue;
					//if (!isNumberOne && (searchMode != SEARCH_TOP10_DEFRAG || !isLogged)) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.
					if (!isNumberOne && (/*searchMode != SEARCH_TOP10_DEFRAG || */!isLogged) && searchMode != SEARCH_ALL_DEFRAG) continue; // If it's not #1 and not logged, we cannot tell if it's a top 10 time.
					

					int startTime = runStart - bufferTime;
					int endTime = demoCurrentTime + bufferTime;
					int earliestPossibleStart = lastGameStateChangeInDemoTime + 1;
					bool isTruncated = false;
					int truncationOffset = 0;
					if (earliestPossibleStart > startTime) {
						truncationOffset = earliestPossibleStart - startTime;
						startTime = earliestPossibleStart;
						isTruncated = true;
					}
					//startTime = std::max(lastGameStateChangeInDemoTime+1, startTime); // We can't start before 0 or before the last gamestate change. +1 to be safe, not sure if necessary.
					

					std::stringstream ss;
					ss << mapname << std::setfill('0') << "___" << std::setw(3) << minutes << "-" << std::setw(2) << pureSeconds << "-" << std::setw(3) << pureMilliseconds << "___" << playername << (isNumberOne ? "" : "___top10") << (isLogged ? "" : (isNumberOne ? "___unloggedWR" : "___unlogged")) << (wasFollowed ? "" : (wasVisibleOrFollowed ? "___thirdperson" : "___NOTvisible")) << "_" << playerNumber << "_" << demo.cut.Clc.clientNum << (isTruncated ? va("_tr%d", truncationOffset) : "");

					std::string targetFilename = ss.str();
					char* targetFilenameFiltered = new char[targetFilename.length()+1];
					sanitizeFilename(targetFilename.c_str(), targetFilenameFiltered);

					outputBatHandleDefrag << "\nrem demoCurrentTime: "<< demoCurrentTime;
					outputBatHandleDefrag << "\n"<< (wasVisibleOrFollowed ? "" : "rem ") << "DemoCutter \""<<sourceDemoFile << "\" \"" << targetFilenameFiltered << "\" " << startTime << " " << endTime;
					delete[] targetFilenameFiltered;
					//std::cout << mapname << " " << playerNumber << " " << playername << " " << minutes << ":" << secondString << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << wasFollowed << " visible:" << wasVisible << " visibleOrFollowed:" << wasVisibleOrFollowed << "\n";
					std::cout << mapname << " " << playerNumber << " " << playername << " " << minutes << ":" << std::setfill('0') << std::setw(2) << pureSeconds << ":" << std::setw(3) << pureMilliseconds << " number1:" << isNumberOne << " logged:" << isLogged << " followed:" << wasFollowed << " visible:" << wasVisible << " visibleOrFollowed:" << wasVisibleOrFollowed << "\n";
				}

				
				
			}
		}

		if (hadConfigStringCommands) {
			CheckForNameChanges<max_clients>(&demo.cut.Cl, killDb, insertPlayerModelStatement, updatePlayerModelCountStatement, demoType);
			setPlayerAndTeamData<max_clients>(&demo.cut.Cl, demoType);
			updateForcePowersInfo(&demo.cut.Cl);
			updatePlayerDemoStatsArrayPointers<max_clients>(demoType);
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
	checkSaveLaughs(demoCurrentTime,bufferTime, lastGameStateChangeInDemoTime, &outputBatHandleLaughs, killDb, insertLaughsStatement, selectLastInsertRowIdStatement, &oldBasename, &oldPath, oldDemoDateModified, sourceDemoFile, qtrue);



	// Save player stats
	for (auto it = playerDemoStatsMap.begin(); it != playerDemoStatsMap.end(); it++) {

		if (it->second.everUsed) { // Some clients may have never been visible or not even existed at all (we have entries for all client nums by default but they're only set to "everUsed" if they were actually seen)

			std::string mapname = std::get<0>(it->first);
			std::string playerName = std::get<1>(it->first);
			int clientNum = std::get<2>(it->first);
			double strafeDeviation = it->second.strafeDeviation.sum / it->second.strafeDeviation.divisor;
			double strafeDeviationNoSaberMove = it->second.strafeDeviationNoSaberMove.sum / it->second.strafeDeviationNoSaberMove.divisor;
			int64_t strafeSampleCount = it->second.strafeDeviation.divisor+0.5;
			int64_t strafeNoSaberMoveSampleCount = it->second.strafeDeviationNoSaberMove.divisor+0.5;
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@map", mapname.c_str());
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@playerName", playerName.c_str());
			std::string playernameStripped = Q_StripColorAll(playerName);
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@playerNameStripped", playernameStripped.c_str());
			SQLBIND(insertPlayerDemoStatsStatement, int, "@clientNum", clientNum);
			SQLBIND(insertPlayerDemoStatsStatement, double, "@averageStrafeDeviation", strafeDeviation);
			SQLBIND(insertPlayerDemoStatsStatement, double, "@averageStrafeDeviationNoSaberMove", strafeDeviationNoSaberMove);

			std::stringstream ssStrafeJson;
			std::stringstream ssStrafeNoSaberMoveJson;
			ssStrafeJson << "[\n";
			ssStrafeNoSaberMoveJson << "[\n";
			for (int b = 0; b < STRAFE_ANALYSIS_BUCKET_COUNT; b++) {
				double valueHere = it->second.strafeDeviationBuckets[b].sum / it->second.strafeDeviationBuckets[b].divisor;
				int64_t sampleCount = it->second.strafeDeviationBuckets[b].divisor+0.5;
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
				sampleCount = it->second.strafeDeviationNoSaberMoveBuckets[b].divisor+0.5;
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
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@averageStrafeDeviationBucketsJSON", ssStrafeJsonString.c_str());
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@averageStrafeDeviationNoSaberMoveBucketsJSON", ssStrafeNoSaberMoveJsonString.c_str());


			SQLBIND(insertPlayerDemoStatsStatement, int, "@strafeSampleCount", strafeSampleCount);
			SQLBIND(insertPlayerDemoStatsStatement, int, "@strafeNoSaberMoveSampleCount", strafeNoSaberMoveSampleCount);

			SQLBIND(insertPlayerDemoStatsStatement, int, "@hitBySaberCount", it->second.hitBySaberCount);
			SQLBIND(insertPlayerDemoStatsStatement, int, "@hitBySaberBlockableCount", it->second.hitBySaberBlockableCount);
			SQLBIND(insertPlayerDemoStatsStatement, int, "@parryCount", it->second.parryCount);
			SQLBIND(insertPlayerDemoStatsStatement, int, "@attackFromParryCount", it->second.attackFromParryCount);

			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@demoName", oldBasename.c_str());
			SQLBIND_TEXT(insertPlayerDemoStatsStatement, "@demoPath", oldPath.c_str());
			SQLBIND(insertPlayerDemoStatsStatement, int, "@demoDateTime", oldDemoDateModified);

			int queryResult = sqlite3_step(insertPlayerDemoStatsStatement);
			if (queryResult != SQLITE_DONE) {
				std::cout << "Error inserting player demo stats into database: " << sqlite3_errmsg(debugStatsDb) << "\n";
			}
			sqlite3_reset(insertPlayerDemoStatsStatement);
		}

	}

#ifdef DEBUGSTATSDB
	for (auto it = animStanceCounts.begin(); it != animStanceCounts.end(); it++) {

		/*SQLBIND(insertAnimStanceStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(insertAnimStanceStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(insertAnimStanceStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(insertAnimStanceStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(insertAnimStanceStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);
		SQLBIND(updateAnimStanceCountStatement, int, "@countFound", );
		SQLBIND(updateAnimStanceCountStatement, int, "@saberHolstered", demo.cut.Cl.snap.ps.saberHolstered);
		SQLBIND(updateAnimStanceCountStatement, int, "@torsoAnim", demo.cut.Cl.snap.ps.torsoAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(updateAnimStanceCountStatement, int, "@legsAnim", demo.cut.Cl.snap.ps.legsAnim & ~ANIM_TOGGLEBIT);
		SQLBIND(updateAnimStanceCountStatement, int, "@saberMove", demo.cut.Cl.snap.ps.saberMove);
		SQLBIND(updateAnimStanceCountStatement, int, "@stance", demo.cut.Cl.snap.ps.fd.saberAnimLevel);*/
		SQLBIND(insertAnimStanceStatement, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@saberHolstered", std::get<1>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@torsoAnim", std::get<2>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@legsAnim", std::get<3>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@saberMove", std::get<4>(it->first));
		SQLBIND(insertAnimStanceStatement, int, "@stance", std::get<5>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@countFound", it->second);
		SQLBIND(updateAnimStanceCountStatement, int, "@demoVersion", std::get<0>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@saberHolstered", std::get<1>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@torsoAnim", std::get<2>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@legsAnim", std::get<3>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@saberMove", std::get<4>(it->first));
		SQLBIND(updateAnimStanceCountStatement, int, "@stance", std::get<5>(it->first));

		int queryResult = sqlite3_step(insertAnimStanceStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error inserting anim stance into database: " << sqlite3_errmsg(debugStatsDb) << "\n";
		}
		sqlite3_reset(insertAnimStanceStatement);

		queryResult = sqlite3_step(updateAnimStanceCountStatement);
		if (queryResult != SQLITE_DONE) {
			std::cout << "Error updating anim stance count in database: " << sqlite3_errmsg(debugStatsDb) << "\n";
		}
		sqlite3_reset(updateAnimStanceCountStatement);
	}



	sqlite3_exec(debugStatsDb, "COMMIT;", NULL, NULL, NULL);

	sqlite3_finalize(insertAnimStanceStatement);
	sqlite3_finalize(updateAnimStanceCountStatement);
	sqlite3_close(debugStatsDb);
#endif


	sqlite3_exec(killDb, "COMMIT;", NULL, NULL, NULL);
	sqlite3_finalize(insertLaughsStatement);
	sqlite3_finalize(insertDefragRunStatement);
	sqlite3_finalize(insertCaptureStatement);
	sqlite3_finalize(insertSpreeStatement);
	sqlite3_finalize(insertStatement);
	sqlite3_finalize(insertAngleStatement);
	sqlite3_finalize(insertPlayerModelStatement);
	sqlite3_finalize(updatePlayerModelCountStatement);
	sqlite3_finalize(selectLastInsertRowIdStatement);
	sqlite3_finalize(insertPlayerDemoStatsStatement);
	sqlite3_close(killDb);

	FS_FCloseFile(oldHandle);
	//FS_FCloseFile(newHandle);

	outputBatHandle.close();
	outputBatHandleKillSprees.close();
	outputBatHandleDefrag.close();
	outputBatHandleCaptures.close();
	outputBatHandleLaughs.close();


	std::cout << "\ndone." << "\n\n";


 	return ret;
}



qboolean demoHighlightFind(const char* sourceDemoFile, int bufferTime, const char* outputBatFile, const char* outputBatFileKillSprees, const char* outputBatFileDefrag, const char* outputBatFileCaptures, const char* outputBatFileLaughs, highlightSearchMode_t searchMode) {
	char			ext[7]{};
	demoType_t		demoType;
	qboolean		isCompressedFile = qfalse;
	demoCutGetDemoType(sourceDemoFile, ext, &demoType, &isCompressedFile);
	int maxClientsHere = getMAX_CLIENTS(demoType);
	switch (maxClientsHere) {
	case 32:
		return demoHighlightFindReal<32>(sourceDemoFile, bufferTime, outputBatFile, outputBatFileKillSprees, outputBatFileDefrag, outputBatFileCaptures, outputBatFileLaughs, searchMode);
		break;
	case 64:
		return demoHighlightFindReal<64>(sourceDemoFile, bufferTime, outputBatFile, outputBatFileKillSprees, outputBatFileDefrag, outputBatFileCaptures, outputBatFileLaughs, searchMode);
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


int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "need 2 arguments at least: demoname and buffer (before and after highlight) in milliseconds";
		std::cin.get();
		return 1;
	}
	initializeGameInfos();


	char* demoName = argv[1];
	float bufferTime = atof(argv[2]);

	highlightSearchMode_t searchMode = SEARCH_INTERESTING;
	if (argc > 3) {
		// Searchmode specified
		char* searchModeText = argv[3];
		if (!_stricmp(searchModeText, "myctfreturns")) {
			searchMode = SEARCH_MY_CTF_RETURNS;
		}else if (!_stricmp(searchModeText, "ctfreturns")) {
			searchMode = SEARCH_CTF_RETURNS;
		}else if (!_stricmp(searchModeText, "top10defrag")) {
			searchMode = SEARCH_TOP10_DEFRAG;
		}else if (!_stricmp(searchModeText, "alldefrag")) {
			searchMode = SEARCH_ALL_DEFRAG;
		}
	}

	Com_Printf("Looking at %s.\n", demoName); 
	std::chrono::high_resolution_clock::time_point benchmarkStartTime = std::chrono::high_resolution_clock::now();
	if (demoHighlightFind(demoName, bufferTime,"highlightExtractionScript.bat","highlightExtractionScriptKillSprees.bat","highlightExtractionScriptDefrag.bat","highlightExtractionScriptCaptures.bat","highlightExtractionScriptLaughs.bat", searchMode)) {
		std::chrono::high_resolution_clock::time_point benchmarkEndTime = std::chrono::high_resolution_clock::now();
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(benchmarkEndTime - benchmarkStartTime).count() / 1000000.0f;
		Com_Printf("Highlights successfully found in %.5f seconds.\n",seconds);
	}
	else {
		Com_Printf("Finding highlights in demo %s has resulted in errors\n", demoName);
	}
#ifdef DEBUG
	std::cin.get();
#endif
}

#ifdef RELDEBUG
//#pragma optimize("", on)
#endif