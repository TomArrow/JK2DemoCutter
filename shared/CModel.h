
#ifndef CMODEL_H
#define CMODEL_H
#include "demoCut.h"


class CModel;

/*
==============================================================================

  .BSP file format

==============================================================================
*/


// little-endian "RBSP"
#define BSP_IDENT				(('P'<<24)+('S'<<16)+('B'<<8)+'R')

#define BSP_VERSION				1


// there shouldn't be any problem with increasing these values at the
// expense of more memory allocation in the utilities
#define	MAX_MAP_MODELS			0x400
#define	MAX_MAP_BRUSHES			0x8000
#define	MAX_MAP_ENTITIES		0x800
#define	MAX_MAP_ENTSTRING		0x40000
#define	MAX_MAP_SHADERS			0x400

#define	MAX_MAP_AREAS			0x100	// MAX_MAP_AREA_BYTES in q_shared must match!
#define	MAX_MAP_FOGS			0x100
#define	MAX_MAP_PLANES			0x20000
#define	MAX_MAP_NODES			0x20000
#define	MAX_MAP_BRUSHSIDES		0x20000
#define	MAX_MAP_LEAFS			0x20000
#define	MAX_MAP_LEAFFACES		0x20000
#define	MAX_MAP_LEAFBRUSHES		0x40000
#define	MAX_MAP_PORTALS			0x20000
#define	MAX_MAP_LIGHTING		0x800000
#define	MAX_MAP_LIGHTGRID		65535
#define	MAX_MAP_LIGHTGRID_ARRAY	0x100000
#define	MAX_MAP_VISIBILITY		0x600000

#define	MAX_MAP_DRAW_SURFS	0x20000
#define	MAX_MAP_DRAW_VERTS	0x80000
#define	MAX_MAP_DRAW_INDEXES	0x80000


// key / value pair sizes in the entities lump
#define	MAX_KEY				32
#define	MAX_VALUE			1024

// the editor uses these predefined yaw angles to orient entities up or down
#define	ANGLE_UP			-1
#define	ANGLE_DOWN			-2

#define	LIGHTMAP_WIDTH		128
#define	LIGHTMAP_HEIGHT		128

#define MAX_WORLD_COORD		( 64 * 1024 )
#define MIN_WORLD_COORD		( -64 * 1024 )
#define WORLD_SIZE			( MAX_WORLD_COORD - MIN_WORLD_COORD )

//=============================================================================

#define	MAX_FACE_POINTS		64

typedef struct {
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_SHADERS		1
#define	LUMP_PLANES			2
#define	LUMP_NODES			3
#define	LUMP_LEAFS			4
#define	LUMP_LEAFSURFACES	5
#define	LUMP_LEAFBRUSHES	6
#define	LUMP_MODELS			7
#define	LUMP_BRUSHES		8
#define	LUMP_BRUSHSIDES		9
#define	LUMP_DRAWVERTS		10
#define	LUMP_DRAWINDEXES	11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define LUMP_LIGHTARRAY		17
#define	HEADER_LUMPS		18

typedef struct {
int			ident;
int			version;

lump_t		lumps[HEADER_LUMPS];
} dheader_t;


typedef enum {
	MST_BAD,
	MST_PLANAR,
	MST_PATCH,
	MST_TRIANGLE_SOUP,
	MST_FLARE
} mapSurfaceType_t;


// Light Style Constants
#define	MAXLIGHTMAPS	4
#define LS_NORMAL		0x00
#define LS_UNUSED		0xfe
#define	LS_LSNONE		0xff //rww - changed name because it unhappily conflicts with a lightsaber state name and changing this is just easier
#define MAX_LIGHT_STYLES		64

typedef struct {
	vec3_t		xyz;
	float		st[2];
	float		lightmap[MAXLIGHTMAPS][2];
	vec3_t		normal;
	byte		color[MAXLIGHTMAPS][4];
} drawVert_t;
typedef enum {
	SF_BAD,
	SF_SKIP,				// ignore
	SF_FACE,
	SF_GRID,
	SF_TRIANGLES,
	SF_POLY,
	SF_MD3,
	SF_MD4,
	/*
	Ghoul2 Insert Start
	*/

	SF_MDX,
	/*
	Ghoul2 Insert End
	*/
	SF_FLARE,
	SF_ENTITY,				// beams, rails, lightning, etc that can be determined by entity
	SF_DISPLAY_LIST,

	SF_NUM_SURFACE_TYPES,
	SF_MAX = 0xffffffff			// ensures that sizeof( surfaceType_t ) == sizeof( int )
} surfaceType_t;

typedef struct msurface_s {
	int					viewCount;		// if == tr.viewCount, already added
	struct shader_s* shader;
	int					fogIndex;
	int					contents;		// so we can determine whether to draw a particular surface based on content flags
	int					flags;		// surfaceflags (might come in handy?)
	qboolean			trisoupMapSurf;

	surfaceType_t* data;			// any of srf*_t
} msurface_t;


typedef struct {
	int			shaderNum;
	int			fogNum;
	int			surfaceType;

	int			firstVert;
	int			numVerts;

	int			firstIndex;
	int			numIndexes;

	byte		lightmapStyles[MAXLIGHTMAPS], vertexStyles[MAXLIGHTMAPS];
	int			lightmapNum[MAXLIGHTMAPS];
	int			lightmapX[MAXLIGHTMAPS], lightmapY[MAXLIGHTMAPS];
	int			lightmapWidth, lightmapHeight;

	vec3_t		lightmapOrigin;
	vec3_t		lightmapVecs[3];	// for patches, [0] and [1] are lodbounds

	int			patchWidth;
	int			patchHeight;
} dsurface_t;


typedef struct {
	vec3_t		xyz;
	float		st[2];
	float		lightmap[MAXLIGHTMAPS][2];
	vec3_t		normal;
	byte		color[MAXLIGHTMAPS][4];
} mapVert_t;


// cm_polylib.h



typedef struct
{
	int		numpoints;
	vec3_t	p[4];		// variable sized
} winding_t;

#define	MAX_POINTS_ON_WINDING	64

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define	SIDE_CROSS	3

#define	CLIP_EPSILON	0.1f

#define MAX_MAP_BOUNDS			65535

// you can define on_epsilon in the makefile as tighter
#ifndef	ON_EPSILON
#define	ON_EPSILON	0.1f
#endif





// cm_patch.h

#define	MAX_FACETS			1024
#define	MAX_PATCH_PLANES	4096 // doubled in jk2mv

typedef struct {
	float	plane[4];
	int		signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
} patchPlane_t;

typedef struct {
	int			surfacePlane;
	int			numBorders;		// 3 or four + 6 axial bevels + 4 or 3 * 4 edge bevels
	int			borderPlanes[4+6+16];
	int			borderInward[4+6+16];
	qboolean	borderNoAdjust[4+6+16];
} facet_t;

typedef struct patchCollide_s {
	vec3_t	bounds[2];
	int		numPlanes;			// surface planes plus edge planes
	patchPlane_t	*planes;
	int		numFacets;
	facet_t	*facets;
} patchCollide_t;

#define	SUBDIVIDE_DISTANCE	16	//4	// never more than this units away from curve
#define	PLANE_TRI_EPSILON	0.1f
#define	WRAP_POINT_EPSILON	0.1f

#define	MAX_GRID_SIZE	129


typedef struct {
	int			width;
	int			height;
	qboolean	wrapWidth;
	qboolean	wrapHeight;
	vec3_t	points[MAX_GRID_SIZE][MAX_GRID_SIZE];	// [width][height]
} cGrid_t;


#define	MAX_SUBMODELS			256




#define	CONTENTS_SOLID			0x00000001u	// Default setting. An eye is never valid in a solid
#define	CONTENTS_LAVA			0x00000002u
#define	CONTENTS_WATER			0x00000004u
#define	CONTENTS_FOG			0x00000008u
#define	CONTENTS_PLAYERCLIP		0x00000010u
#define	CONTENTS_MONSTERCLIP	0x00000020u	// Physically block bots
#define CONTENTS_BOTCLIP		0x00000040u	// A hint for bots - do not enter this brush by navigation (if possible)
#define CONTENTS_SHOTCLIP		0x00000080u
#define	CONTENTS_BODY			0x00000100u	// should never be on a brush, only in game
#define	CONTENTS_CORPSE			0x00000200u	// should never be on a brush, only in game
#define	CONTENTS_TRIGGER		0x00000400u
#define	CONTENTS_NODROP			0x00000800u	// don't leave bodies or items (death fog, lava)
#define CONTENTS_TERRAIN		0x00001000u	// volume contains terrain data
#define CONTENTS_LADDER			0x00002000u
#define CONTENTS_ABSEIL			0x00004000u // (SOF2) used like ladder to define where an NPC can abseil
#define CONTENTS_OPAQUE			0x00008000u // defaults to on, when off, solid can be seen through
#define CONTENTS_OUTSIDE		0x00010000u	// volume is considered to be in the outside (i.e. not indoors)
#define CONTENTS_SLIME			0x00020000u	// CHC needs this since we use same tools
#define CONTENTS_LIGHTSABER		0x00040000u	// ""
#define CONTENTS_TELEPORTER		0x00080000u	// ""
#define CONTENTS_ITEM			0x00100000u	// ""
#define CONTENTS_NOSHOT			0x00200000u	// shots pass through me
#define	CONTENTS_DETAIL			0x08000000u	// brushes not used for the bsp
#define	CONTENTS_TRANSLUCENT	0x80000000u	// don't consume surface fragments inside

#define CONTENTS_ALL			0xFFFFFFFFu

#define	SURF_SKY				0x00002000u	// lighting from environment map
#define	SURF_SLICK				0x00004000u	// affects game physics
#define	SURF_METALSTEPS			0x00008000u	// CHC needs this since we use same tools (though this flag is temp?)
#define SURF_FORCEFIELD			0x00010000u	// CHC ""			(but not temp)
#define	SURF_NODAMAGE			0x00040000u	// never give falling damage
#define	SURF_NOIMPACT			0x00080000u	// don't make missile explosions
#define	SURF_NOMARKS			0x00100000u	// don't leave missile marks
#define	SURF_NODRAW				0x00200000u	// don't generate a drawsurface at all
#define	SURF_NOSTEPS			0x00400000u	// no footstep sounds
#define	SURF_NODLIGHT			0x00800000u	// don't dlight even if solid (solid lava, skies)
#define	SURF_NOMISCENTS			0x01000000u	// no client models allowed on this surface

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)


typedef struct {
	cplane_t	*plane;
	int			children[2];		// negative numbers are leafs
} cNode_t;

typedef struct {
	int			planeNum;
	int			children[2];	// negative numbers are -(leafs+1), not nodes
	int			mins[3];		// for frustom culling
	int			maxs[3];
} dnode_t;

typedef struct {
	int			cluster;
	int			area;

	int			firstLeafBrush;
	int			numLeafBrushes;

	int			firstLeafSurface;
	int			numLeafSurfaces;
} cLeaf_t;

typedef struct {
	int			cluster;			// -1 = opaque cluster (do I still store these?)
	int			area;

	int			mins[3];			// for frustum culling
	int			maxs[3];

	int			firstLeafSurface;
	int			numLeafSurfaces;

	int			firstLeafBrush;
	int			numLeafBrushes;
} dleaf_t;

typedef struct cmodel_s {
	vec3_t		mins, maxs;
	cLeaf_t		leaf;			// submodels don't reference the main tree
} cmodel_t;
typedef struct {
	float		mins[3], maxs[3];
	int			firstSurface, numSurfaces;
	int			firstBrush, numBrushes;
} dmodel_t;

typedef struct {
	int			planeNum;			// positive plane side faces out of the leaf
	int			shaderNum;
	int			drawSurfNum;
} dbrushside_t;


// plane types are used to speed some tests
// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2
#define	PLANE_NON_AXIAL	3

/*
=================
PlaneTypeForNormal
=================
*/

#define PlaneTypeForNormal(x) (x[0] == 1 ? PLANE_X : (x[1] == 1 ? PLANE_Y : (x[2] == 1 ? PLANE_Z : PLANE_NON_AXIAL) ) )

typedef struct {
	cplane_t	*plane;
	int			surfaceFlags;
	int			shaderNum;
	int			drawSurfNum;
	int			damage;
} cbrushside_t;

typedef struct {
	int			shaderNum;		// the shader that determined the contents
	int			contents;
	vec3_t		bounds[2];
	int			numsides;
	cbrushside_t	*sides;
	int			checkcount;		// to avoid repeated testings
} cbrush_t;



typedef struct {
	int			firstSide;
	int			numSides;
	int			shaderNum;		// the shader that determines the contents flags
} dbrush_t;

typedef struct {
	char		shader[MAX_QPATH];
	int			surfaceFlags;
	int			contentFlags;
} dshader_t;

class CCMShader
{
public:
	char		shader[MAX_QPATH];
class CCMShader	*mNext;
	int			surfaceFlags;
	int			contentFlags;
	vec3_t		sunLight;
	vec3_t		sunDirection;
	vec3_t		fogColor;
	float		depthForOpaque;
	int			damageShaderNum;
	int			damage;
	char		hitMaterial[MAX_QPATH];
	char		hitLocation[MAX_QPATH];

	const char *GetName(void) const { return(shader); }
	class CCMShader *GetNext(void) const { return(mNext); }
	void SetNext(class CCMShader *next) { mNext = next; }
	void Destroy(void) { }
};

typedef struct {
	int			checkcount;				// to avoid repeated testings
	int			surfaceFlags;
	int			contents;
	struct patchCollide_s	*pc;
} cPatch_t;


typedef struct {
	int			floodnum;
	int			floodvalid;
} cArea_t;

typedef struct {
	char		name[MAX_QPATH];

	int			numShaders;
	CCMShader	*shaders;

	int			numBrushSides;
	cbrushside_t *brushsides;

	int			numPlanes;
	cplane_t	*planes;

	int			numNodes;
	cNode_t		*nodes;

	int			numLeafs;
	cLeaf_t		*leafs;

	int			numLeafBrushes;
	int			*leafbrushes;

	int			numLeafSurfaces;
	int			*leafsurfaces;

	int			numSubModels;
	cmodel_t	*cmodels;

	int			numBrushes;
	cbrush_t	*brushes;

	int			numClusters;
	int			clusterBytes;
	byte		*visibility;
	qboolean	vised;			// if false, visibility is just a single cluster of ffs

	int			numEntityChars;
	char		*entityString;

	int			numAreas;
	cArea_t		*areas;
	int			*areaPortals;	// [ numAreas*numAreas ] reference counts

	int			numSurfaces;
	cPatch_t	**surfaces;			// non-patches will be NULL

	int			floodvalid;
	int			checkcount;					// incremented on each trace

	int			boxModelHandle;
	int			capsuleModelHandle;
} clipMap_t;


// keep 1/8 unit away to keep the position valid before network snapping
// and to avoid various numeric issues
#define	SURFACE_CLIP_EPSILON	0.125f


// cm_test.c

// Used for oriented capsule collision detection
typedef struct
{
	qboolean	use;
	float		radius;
	float		halfheight;
	vec3_t		offset;
} sphere_t;

typedef struct {
	vec3_t		start;
	vec3_t		end;
	vec3_t		size[2];	// size of the box being swept through the model
	vec3_t		offsets[8];	// [signbits][x] = either size[0][x] or size[1][x]
	float		maxOffset;	// longest corner length from origin
	vec3_t		extents;	// greatest of abs(size[0]) and abs(size[1])
	vec3_t		bounds[2];	// enclosing box of start and end surrounding by size
	vec3_t		modelOrigin;// origin of the model tracing through
	int			contents;	// ored contents of the model tracing through
	qboolean	isPoint;	// optimized case
	trace_t		trace;		// returned from trace call
	sphere_t	sphere;		// sphere for oriendted capsule collision
} traceWork_t;


typedef struct leafList_s {
	int		count;
	int		maxcount;
	qboolean	overflowed;
	int* list;
	vec3_t	bounds[2];
	int		lastLeaf;		// for overflows where each leaf can't be stored individually
	void	(CModel::* storeLeafs)(struct leafList_s* ll, int nodenum);
} leafList_t;

typedef enum {
	h_high,
	h_low,
	h_dontcare
} ha_pref;

typedef struct triangle_s {
	vec3_t	xyz[3];
} triangle_t;
typedef struct vertXYZ_s {
	vec3_t	xyz;
	vec2_t	lightmapSt;
	uint16_t	lightmapNum;
} vertXYZ_t;
#define	LIGHTMAP_SIZE	128
typedef struct lightmap_s {
	byte	data[LIGHTMAP_SIZE * LIGHTMAP_SIZE * 3];
} lightmap_t;


class CModel {

	void* gpvCachedMapDiskImage = NULL;
	char  gsCachedMapDiskImage[MAX_QPATH];
	qboolean gbUsingCachedMapDataRightNow = qfalse;	// if true, signifies that you can't delete this at the moment!! (used during z_malloc()-fail recovery attempt)

	byte* cmod_base;
	cmodel_t	box_model;
	cplane_t* box_planes;
	cbrush_t* box_brush;
	clipMap_t	cm;
	int			c_pointcontents;
	int			c_traces, c_brush_traces, c_patch_traces;


	int	c_totalPatchBlocks;
	int	c_totalPatchSurfaces;
	int	c_totalPatchEdges;
	
	// counters are only bumped when running single threaded,
// because they are an awefull coherence problem
	int	c_active_windings;
	int	c_peak_windings;
	int	c_winding_allocs;
	int	c_winding_points;


	bool	cm_noAreas = false;
	bool	cm_noCurves = false;
	bool	cm_playerCurveClip = true;
	bool	r_debugSurfaceUpdate = true;

	const patchCollide_t* debugPatchCollide;
	const facet_t* debugFacet;
	qboolean		debugBlock;
	vec3_t		debugBlockPoints[4];

	int	c_removed;


	// this "hunk" is disgusting but whatever. maybe replace it with a better system someday.
	std::vector<void*> stuffToFree;
	inline void* Hunk_Alloc(int size, ha_pref preference) {
		if (size < 1) {
			return NULL; // i read there's some weirdo compilers/oses that will have VERY strange behavior when you call calloc (or was it malloc) with size 0, that can lead to security vulnerabilities.
			//throw std::exception("Super fake Hunk_Alloc: Can't alloc less than 1 byte.");
		}
		void* ptr = calloc(1, size);
		stuffToFree.push_back(ptr);
		return ptr;
	}

	int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, struct cplane_s* p);

	int CM_BoxBrushes(const vec3_t mins, const vec3_t maxs, cbrush_t** list, int listsize);

	void CM_StoreLeafs(leafList_t* ll, int nodenum);
	void CM_StoreBrushes(leafList_t* ll, int nodenum);

	void CM_BoxLeafnums_r(leafList_t* ll, int nodenum);


	// render stuff (simplified)
	void R_LoadSurfaces(lump_t* surfs, lump_t* verts, lump_t* indexLump);
	void ParseFace(dsurface_t* ds, mapVert_t* verts,  int* indexes);
	void R_LoadLightmaps(lump_t* l);

	// cm_test.c
	int  CM_PointLeafnum_r(const vec3_t p, int num);
	int  CM_PointLeafnum(const vec3_t p);
	int	 CM_BoxLeafnums(const vec3_t mins, const vec3_t maxs, int* list, int listsize, int* lastLeaf);
	byte* CM_ClusterPVS(int cluster);
	void  CM_FloodArea_r(int areaNum, int floodnum);
	void	 CM_FloodAreaConnections(void);
	void	 CM_AdjustAreaPortalState(int area1, int area2, qboolean open);
	qboolean	 CM_AreasConnected(int area1, int area2);
	int CM_WriteAreaBits(byte* buffer, int area);

	// cm_load.c
	void  CMod_LoadShaders(lump_t* l);

	void  CMod_LoadSubmodels(lump_t* l);
	void  CMod_LoadNodes(lump_t* l);
	void  CM_BoundBrush(cbrush_t* b);
	void  CMod_LoadBrushes(lump_t* l);
	void  CMod_LoadLeafs(lump_t* l);

	void  CMod_LoadPlanes(lump_t* l);

	void  CMod_LoadLeafBrushes(lump_t* l);

	void  CMod_LoadLeafSurfaces(lump_t* l);

	void  CMod_LoadBrushSides(lump_t* l);

	void  CMod_LoadEntityString(lump_t* l, const char* name);
	void  CMod_LoadVisibility(lump_t* l);
	void  CMod_LoadPatches(lump_t* surfs, lump_t* verts);
	unsigned  CM_LumpChecksum(lump_t* lump);
	unsigned  CM_Checksum(dheader_t* header);
	qboolean  CM_DeleteCachedMap(qboolean bGuaranteedOkToDelete);

	void  CM_LoadMap_Actual(const char* name, qboolean clientload, int* checksum);
	void  CM_LoadMap(const char* name, qboolean clientload, int* checksum);

	void  CM_ClearMap(void);

	cmodel_t* CM_ClipHandleToModel(clipHandle_t handle);
	clipHandle_t	 CM_InlineModel(int index);
	int		 CM_NumClusters(void);
	int		 CM_NumInlineModels(void);
	char* CM_EntityString(void);
	int		 CM_LeafCluster(int leafnum);
	int		 CM_LeafArea(int leafnum);
	void  CM_InitBoxHull(void);


	// cm_polylib.c
	void  pw(winding_t* w);
	winding_t* AllocWinding(int points);
	void  FreeWinding(winding_t* w);
	void	 RemoveColinearPoints(winding_t* w);
	void  WindingPlane(winding_t* w, vec3_t normal, vec_t* dist);
	vec_t	 WindingArea(winding_t* w);
	void	 WindingBounds(winding_t* w, vec3_t mins, vec3_t maxs);
	void	 WindingCenter(winding_t* w, vec3_t center);
	winding_t* BaseWindingForPlane(vec3_t normal, vec_t dist);
	winding_t* CopyWinding(winding_t* w);
	winding_t* ReverseWinding(winding_t* w);
	void	 ClipWindingEpsilon(winding_t* in, vec3_t normal, vec_t dist,
		vec_t epsilon, winding_t** front, winding_t** back);
	void  ChopWindingInPlace(winding_t** inout, vec3_t normal, vec_t dist, vec_t epsilon);
	winding_t* ChopWinding(winding_t* in, vec3_t normal, vec_t dist);
	void  CheckWinding(winding_t* w);
	int		 WindingOnPlaneSide(winding_t* w, vec3_t normal, vec_t dist);
	void	 AddWindingToConvexHull(winding_t* w, winding_t** hull, vec3_t normal);

	// cm_patch.c
	int  CM_SignbitsForNormal(vec3_t normal);
	qboolean  CM_PlaneFromPoints(vec4_t plane, vec3_t a, vec3_t b, vec3_t c);
	qboolean	 CM_NeedsSubdivision(vec3_t a, vec3_t b, vec3_t c);
	void  CM_Subdivide(vec3_t a, vec3_t b, vec3_t c, vec3_t out1, vec3_t out2, vec3_t out3);
	void  CM_TransposeGrid(cGrid_t* grid);
	void  CM_SetGridWrapWidth(cGrid_t* grid);
	void  CM_SubdivideGridColumns(cGrid_t* grid);
	qboolean  CM_ComparePoints(float* a, float* b);
	void  CM_RemoveDegenerateColumns(cGrid_t* grid);
	int  CM_PlaneEqual(patchPlane_t* p, float plane[4], int* flipped);
	void  CM_SnapVector(vec3_t normal);
	int  CM_FindPlane2(float plane[4], int* flipped);
	int  CM_FindPlane(float* p1, float* p2, float* p3);
	int  CM_PointOnPlaneSide(float* p, int planeNum);
	int	 CM_GridPlane(int gridPlanes[MAX_GRID_SIZE][MAX_GRID_SIZE][2], int i, int j, int tri);
	int  CM_EdgePlaneNum(cGrid_t* grid, int gridPlanes[MAX_GRID_SIZE][MAX_GRID_SIZE][2], int i, int j, int k);
	void  CM_SetBorderInward(facet_t* facet, cGrid_t* grid, int gridPlanes[MAX_GRID_SIZE][MAX_GRID_SIZE][2],
		int i, int j, int which);
	qboolean  CM_ValidateFacet(facet_t* facet);
	void  CM_AddFacetBevels(facet_t* facet);
	void  CM_DrawDebugSurface(void (*drawPoly)(int color, int numPoints, float* points));
	void CM_TracePointThroughPatchCollide(traceWork_t* tw, const struct patchCollide_s* pc);
	int CM_CheckFacetPlane(float* plane, vec3_t start, vec3_t end, float* enterFrac, float* leaveFrac, int* hit);
	struct patchCollide_s* CM_GeneratePatchCollide(int width, int height, vec3_t* points);
	void CM_TraceThroughPatchCollide(traceWork_t* tw, const struct patchCollide_s* pc);
	qboolean CM_PositionTestInPatchCollide(traceWork_t* tw, const struct patchCollide_s* pc);
	void CM_PatchCollideFromGrid(cGrid_t* grid, patchCollide_t* pf);
	void CM_ClearLevelPatches(void);

	void CM_Trace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, const vec3_t origin, int brushmask, qboolean capsule, sphere_t* sphere);



	void CM_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs);

	void CM_ProjectPointOntoVector(vec3_t point, vec3_t vStart, vec3_t vDir, vec3_t vProj);

	float CM_DistanceFromLineSquared(vec3_t p, vec3_t lp1, vec3_t lp2, vec3_t dir);

	float CM_VectorDistanceSquared(vec3_t p1, vec3_t p2);

	void CM_TestBoxInBrush(traceWork_t* tw, cbrush_t* brush);

	void CM_TestCapsuleInCapsule(traceWork_t* tw, clipHandle_t model);

	void CM_TestBoundingBoxInCapsule(traceWork_t* tw, clipHandle_t model);

	void CM_PositionTest(traceWork_t* tw);

	void CM_TraceThroughPatch(traceWork_t* tw, cPatch_t* patch);

	void CM_TraceThroughBrush(traceWork_t* tw, cbrush_t* brush);

	void CM_TraceThroughSphere(traceWork_t* tw, vec3_t origin, float radius, vec3_t start, vec3_t end);

	void CM_TraceThroughVerticalCylinder(traceWork_t* tw, vec3_t origin, float radius, float halfheight, vec3_t start, vec3_t end);

	void CM_TraceCapsuleThroughCapsule(traceWork_t* tw, clipHandle_t model);

	void CM_TraceBoundingBoxThroughCapsule(traceWork_t* tw, clipHandle_t model);

	void CM_TraceThroughTree(traceWork_t* tw, int num, float p1f, float p2f, vec3_t p1, vec3_t p2);

	void CM_TestInLeaf(traceWork_t* tw, cLeaf_t* leaf);

	void CM_TraceThroughLeaf(traceWork_t* tw, cLeaf_t* leaf);


	bool simpleHeightMode = false;
	bool loadDrawSurfs = false;
	vec3_t groundPos = { 0,0,0 };

	std::vector<int> indices;
	//std::vector<triangle_t> faceTriangles;
	std::vector<vertXYZ_t> faceVerts;
	std::vector<lightmap_t> lightmaps;
public:

	const std::vector<vertXYZ_t>& GetFaceVerts() const {
		return faceVerts; 
	}
	const std::vector<int>& GetFaceVertIndices() const {
		return indices;
	}
	const std::vector<lightmap_t>& GetLightmaps() const {
		return lightmaps;
	}

	static std::string GetMapPath(const char* filename, const std::vector<std::string>* bspPaths = NULL) {

		const char* pathRelativeToDir;
		if (bspPaths) {
			for (auto it = bspPaths->begin(); it != bspPaths->end(); it++) {

				pathRelativeToDir = va("%s/%s", it->c_str(), filename);
				if (FS_FileExists(pathRelativeToDir)) {
					return pathRelativeToDir;
				}
			}

		}

		if (FS_FileExists(filename)) {

			return filename;
		}

		int pathLen = wai_getExecutablePath(NULL, 0, NULL);
		char* path = new char[pathLen + 1];
		int dirlen = 0;
		wai_getExecutablePath(path, pathLen, &dirlen);
		path[dirlen] = '\0';

		pathRelativeToDir = va("%s/%s", path, filename);
		if (FS_FileExists(pathRelativeToDir)) {
			delete[] path;
			return pathRelativeToDir;
		}

		pathRelativeToDir = va("%s/maps/%s", path, filename);
		if (FS_FileExists(pathRelativeToDir)) {
			delete[] path;
			return pathRelativeToDir;
		}

		delete[] path;
		return "";
	}
	CModel(const char* filename, bool loadDrawSurfsA) {
		int checksum = 0;
		
		loadDrawSurfs = loadDrawSurfsA;
		if (FS_FileExists(filename)) {

			CM_LoadMap(filename, qfalse, &checksum);
			return;
		}

		throw new std::exception("CModel constructor: Map file not found.");

	}
	
	CModel() {
		// no map available. we're just gonna use last known ground height
		simpleHeightMode = true;
	}
	void SetGroundPos(vec3_t ground, bool playerPos) {
		//if (!simpleHeightMode) {

		//	throw new std::exception("CModel constructor: Map file not found.");
		//}
		VectorCopy(ground, groundPos);
		if (playerPos) {
			groundPos[2] -= MINS_Z;
		}
	}
	~CModel() { // meh. LOL
		for (auto it = stuffToFree.begin(); it != stuffToFree.end(); it++) {
			free(*it);
		}
		stuffToFree.clear();
		CM_ClearMap();
	}


	int  CM_PointContents(const vec3_t p, clipHandle_t model);
	int	 CM_TransformedPointContents(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles);

	void CM_BoxTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, qboolean capsule);

	void CM_TransformedBoxTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles, qboolean capsule);

	clipHandle_t CM_TempBoxModel(const vec3_t mins, const vec3_t maxs, qboolean capsule);

	void CM_TraceWrap(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask) {
		if (simpleHeightMode) {
			vec3_t minsG, maxsG;
			VectorCopy(groundPos, minsG);
			VectorCopy(groundPos, maxsG);
			minsG[0] -= 10000;
			minsG[1] -= 10000;
			minsG[2] -= 1;
			maxsG[0] += 10000;
			maxsG[1] += 10000;
			clipHandle_t tmp = CM_TempBoxModel(minsG, maxsG, qfalse);
			CM_TransformedBoxTrace(results,start,end,mins,maxs,tmp,contentMask,vec3_origin,vec3_origin,qfalse);
		}
		else {
			CM_BoxTrace(results,start,end,mins,maxs,0,contentMask,qfalse);
		}
	}
	int CM_PointContentsWrap(vec3_t point, int passEntityNum) {
		if (simpleHeightMode) {
			vec3_t minsG, maxsG;
			VectorCopy(groundPos, minsG);
			VectorCopy(groundPos, maxsG);
			minsG[0] -= 10000;
			minsG[1] -= 10000;
			minsG[2] -= 1;
			maxsG[0] += 10000;
			maxsG[1] += 10000;
			clipHandle_t tmp = CM_TempBoxModel(minsG, maxsG, qfalse);
			return CM_TransformedPointContents(point,tmp,vec3_origin,vec3_origin);
		}
		else {
			return CM_PointContents(point, 0);
		}
	}

};

#endif
