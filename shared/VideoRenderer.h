#pragma once

#ifndef VIDEO_RENDERER_H
#define VIDEO_RENDERER_H

#ifdef _WIN64
#define DOFASTPIX 1
#else
#define DOFASTPIX 0
#endif 

#include "demoCut.h"
#include "CModel.h"
#include <deque>
#include "../shared/libgmavi/libgmavi.h"
#if DOFASTPIX
//#include "../shared/fastpix3d/FastPix3D/FastPix3D.h"
#include "../shared/fastpix3d/FastPix3D/Mesh/Mesh.h"
#include "../shared/fastpix3d/FastPix3D/Mesh/Texture.h"
#include "../shared/fastpix3d/FastPix3D/RenderUnit.h"
#include "../shared/fastpix3d/FastPix3D/Interop/ThreadPool.h"
#endif

static inline class S3LRenderer* lastRenderer = NULL; // disgusting

typedef struct drawProperties3dModel_s {
	byte	color[3];
	bool	transparent;
	bool	isCube;
	bool	isWorld;
} drawProperties3dModel_t;

#define VIDEOWIDTH 400
#define VIDEOHEIGHT 304

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



typedef struct color3ub_s {
	byte b, g, r;
} color3ub_t;
//bool videoAdjustTextColor(color3ub_t* Value, int* i, const char* txt, int strLen);

extern bool nwhHexColors;
static inline bool videoAdjustTextColor(color3ub_t* Value, int* i, const char* txt, int strLen) {
	if (*i+1 < strLen) {
		if (Q_IsColorStringHex(txt,nwhHexColors)) {
			vec4_t color;
			int skipCount;
			if (Q_parseColorHex(txt+1, color, &skipCount, nwhHexColors)) {
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

static inline color3ub_t whitefont = { 255,255,255 };
#define blit_pixel color3ub_t
#define blit16_ADJUST_COLOR_FUNC videoAdjustTextColor
#define blit32_ADJUST_COLOR_FUNC videoAdjustTextColor
#define blit16_MACRO_INLINE 1
#define blit32_MACRO_INLINE 1
#include "../shared/blit-fonts/blit16.h"
#include "../shared/blit-fonts/blit32.h"
typedef std::tuple<int64_t, std::string> consoleText;


typedef struct videoFrame_s {
	int64_t	demoTime;
	byte	image[VIDEOWIDTH * VIDEOHEIGHT * 3];
} videoFrame_t;



class VideoRenderer
{
public:

	bool haveMapModel = false;
protected:
	bool renderingMap = false;
	vec3_t camerapos;
	byte drawBuffer[VIDEOWIDTH * VIDEOHEIGHT * 3];

	std::vector<drawProperties3dModel_t>	scene3dmodelProperties;

	std::vector<std::unique_ptr<videoFrame_t>> videoFrames;

	std::vector<lightmap_t>		mapLightmaps;

	std::deque<consoleText> videoConsole; // deque cuz i need to iterate over it for drawing, but rly just want a fifo.
	std::deque<consoleText> screenCenterText; // deque cuz i need to iterate over it for drawing, but rly just want a fifo.
	inline void videoDrawText(int64_t demoCurrentTime, bool nwhHexColors) {
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
			Q_StripColorAll(cleanStr,nwhHexColors);
			int x = VIDEOWIDTH / 2 - strlen(cleanStr)*blit32_ADVANCE/2;
			delete[] cleanStr;
			y += blit32_ROW_ADVANCE*blit32_TextExplicit((color3ub_t*)drawBuffer, whitefont, 1, VIDEOWIDTH, VIDEOHEIGHT, 1, x, y, std::get<1>(*it).c_str());
		}
	}

	public:
		inline void addCenterText(int64_t demoCurrentTime, const char* cpTextC) {
			screenCenterText.push_back({ demoCurrentTime,cpTextC });
		}
		inline void addConsoleText(int64_t demoCurrentTime, const char* cpTextC) {
			videoConsole.push_back({ demoCurrentTime,cpTextC });
		}
		inline void addConsoleText(int64_t demoCurrentTime, std::string& text) {
			videoConsole.push_back({ demoCurrentTime,text });
		}
		inline virtual void startNewFrame() {
			renderingMap = false;
			scene3dmodelProperties.clear();
		};
		inline virtual  drawProperties3dModel_t* startDrawingMap(CModel* cm) {

			renderingMap = true;
			drawProperties3dModel_t* mapModelProps = &scene3dmodelProperties.emplace_back();
			mapModelProps->isWorld = qtrue;
			return mapModelProps;
		};
		inline virtual void initMap(CModel* cm) {

			mapLightmaps.clear();
			auto lightmaps = cm->GetLightmaps();
			for (auto it = lightmaps.begin(); it != lightmaps.end(); it++) {
				mapLightmaps.push_back(*it);
			}
			haveMapModel = true;
		};
		inline virtual void resetMap() {

			mapLightmaps.clear();
			haveMapModel = false;
		};
		inline virtual drawProperties3dModel_t* startDrawingCube(const vec3_t position, const vec3_t scale, const vec3_t rotation) {
			return &scene3dmodelProperties.emplace_back();
		}
		inline virtual drawProperties3dModel_t* startDrawingObject(int* indices, int indexCount, vec3_t* points, int pointCount, const vec3_t position, const vec3_t scale, const vec3_t rotation, bool backFaceCulling) {
			return &scene3dmodelProperties.emplace_back();
		}
		inline virtual void initScene(vec3_t viewangles) {

		}
		inline virtual void drawFrame(int64_t demoCurrentTime, bool nwhHexColors) {

			videoDrawText(demoCurrentTime, nwhHexColors);
			;
			videoFrames.push_back(std::make_unique<videoFrame_t>());
			videoFrames.back()->demoTime = demoCurrentTime;
			memcpy(videoFrames.back()->image, drawBuffer, sizeof(drawBuffer));
		}
		inline void saveVideo(const char* videoPath) {
			if (!videoFrames.size()) {
				return;
			}
			void* gmav = gmav_open(videoPath, VIDEOWIDTH, VIDEOHEIGHT, 1000);
			int64_t lastTime = videoFrames.size() ? videoFrames.front()->demoTime : 0;
			for (auto it = videoFrames.begin(); it != videoFrames.end(); it++) {
				while (it->get()->demoTime - lastTime > 1) {
					if (it->get()->demoTime - lastTime > 100) {
						lastTime = it->get()->demoTime - 100;
					}
					gmav_add(gmav, NULL);
					lastTime++;
				}
				gmav_add(gmav, it->get()->image);
				lastTime = it->get()->demoTime;
			}
			gmav_finish(gmav);
		}
		inline void setCameraPos(vec3_t cameraPosA) {
			VectorCopy(cameraPosA, camerapos);
		}

};

class S3LRenderer : public VideoRenderer {
	
	S3L_Unit cubeVertices[24] = { S3L_CUBE_VERTICES(S3L_F) };
	S3L_Index cubeTriangles[36] = { S3L_CUBE_TRIANGLES };
	S3L_Model3D mapModel; 
	S3L_Model3D cubeModel; // 3D model, has a geometry, position, rotation etc.
	S3L_Scene scene;       // scene we'll be rendring (can have multiple models)
	std::vector<S3L_Unit>		mapVertices;
	std::vector<S3L_Unit>		mapUVs;
	std::vector<S3L_Index>		mapTriangles;
	std::vector<S3L_Index>		mapTrianglesVisFiltered;
	std::vector<S3L_Model3D>	scene3dmodels;
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
	uint32_t previousTriangle = -1;
	int drawLightmapNum = 0;
	S3L_Vec4 uv0, uv1, uv2;


public:
	inline void drawPixel(S3L_PixelInfo* p)
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

		S3LRenderer() {

			S3L_model3DInit(
				cubeVertices,
				S3L_CUBE_VERTEX_COUNT,
				cubeTriangles,
				S3L_CUBE_TRIANGLE_COUNT,
				&cubeModel);
		}

		inline void startNewFrame() override {

			VideoRenderer::startNewFrame();
			scene3dmodels.clear();
			scene3dmodelVertices.clear();
			scene3dmodelTriangles.clear();
		};
		inline void initMap(CModel* cm) override {

			VideoRenderer::initMap(cm);
			auto faceVerts = cm->GetFaceVerts();
			auto faceVertIndices = cm->GetFaceVertIndices();
			mapVertices.clear();
			mapUVs.clear();
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
		}
		inline virtual void resetMap() {
			VideoRenderer::resetMap();
		};
		inline drawProperties3dModel_t* startDrawingMap(CModel* cm) override {

			drawProperties3dModel_t* props =  VideoRenderer::startDrawingMap(cm);

			mapTrianglesVisFiltered.clear();
			S3L_Model3D visFilteredWorld = mapModel;

			auto faceVertIndices = cm->GetVisFilteredFaceVertIndices(camerapos);
			for (auto it = faceVertIndices.begin(); it != faceVertIndices.end(); it++) {
				mapTrianglesVisFiltered.push_back(*it);
			}
			S3L_model3DInit(mapVertices.data(), mapVertices.size() / 3, mapTrianglesVisFiltered.data(), mapTrianglesVisFiltered.size() / 3, &visFilteredWorld);
			scene3dmodels.push_back(visFilteredWorld);
			//scene3dmodels.push_back(mapModel);
			return props;
		};

		inline drawProperties3dModel_t* startDrawingCube(const vec3_t position, const vec3_t scale, const vec3_t rotation) override {

			drawProperties3dModel_t* props = VideoRenderer::startDrawingCube(position, scale, rotation);

			S3L_Model3D model = cubeModel;

			if (position) {
				model.transform.translation.x = S3L_POSX(position[1]);
				model.transform.translation.y = S3L_POSY(position[2]);
				model.transform.translation.z = S3L_POSZ(position[0]);
			}
			if (scale) {
				model.transform.scale.x = (scale[1]) * S3L_POSMULT;
				model.transform.scale.y = (scale[2]) * S3L_POSMULT;
				model.transform.scale.z = (scale[0]) * S3L_POSMULT;
			}
			if (rotation) {
				model.transform.rotation.y = S3L_ROTY(rotation[1]);
			}

			scene3dmodels.push_back(model);

			return props;
		}

		inline drawProperties3dModel_t* startDrawingObject(int* indices, int indexCount, vec3_t* points, int pointCount, const vec3_t position, const vec3_t scale, const vec3_t rotation, bool backFaceCulling) override {
			drawProperties3dModel_t* props = VideoRenderer::startDrawingObject(indices, indexCount, points, pointCount, position, scale, rotation, backFaceCulling);


			std::vector<S3L_Unit> vertices;
			vertices.reserve(6 * 3);
			std::vector<S3L_Index> indicess3l;
			indicess3l.reserve(indexCount);
			for (int i = 0; i < indexCount; i++) {
				indicess3l.push_back(indices[i]);
			}
			for (int i = 0; i < pointCount; i++) {
				vertices.push_back(S3L_POSX(points[i][1]));
				vertices.push_back(S3L_POSY(points[i][2]));
				vertices.push_back(S3L_POSZ(points[i][0]));
			}
			scene3dmodelVertices.push_back(std::move(vertices));
			scene3dmodelTriangles.push_back(std::move(indicess3l));


			S3L_Model3D model;
			S3L_model3DInit(scene3dmodelVertices.back().data(), 6, scene3dmodelTriangles.back().data(), indexCount/3, &model);

			if (position) {
				model.transform.translation.x = S3L_POSX(position[1]);
				model.transform.translation.y = S3L_POSY(position[2]);
				model.transform.translation.z = S3L_POSZ(position[0]);
			}
			if (scale) {
				model.transform.scale.x = (scale[1]) * S3L_POSMULT;
				model.transform.scale.y = (scale[2]) * S3L_POSMULT;
				model.transform.scale.z = (scale[0]) * S3L_POSMULT;
			}
			if (rotation) {
				model.transform.rotation.y = S3L_ROTY(rotation[1]);
			}
			if (!backFaceCulling) {
				model.config.backfaceCulling = 0;
			}

			scene3dmodels.push_back(model);


			return props;
		}

		inline void initScene(vec3_t viewangles) override {
			VideoRenderer::initScene(viewangles);

			S3L_Model3D* models = scene3dmodels.data();

			S3L_sceneInit( // Initialize the scene we'll be rendering.
				models,  // This is like an array with only one model in it.
				scene3dmodels.size(),
				&scene);

			scene.camera.transform.translation.x = S3L_POSX(camerapos[1]);
			scene.camera.transform.translation.y = S3L_POSY(camerapos[2]);
			scene.camera.transform.translation.z = S3L_POSZ(camerapos[0]);

			scene.camera.transform.rotation.x = S3L_ROTX(viewangles[0]);
			scene.camera.transform.rotation.y = S3L_ROTY(viewangles[1]);
			scene.camera.transform.rotation.z = S3L_ROTZ(viewangles[2]);

			scene.camera.focalLength = 300;
		}

		inline void drawFrame(int64_t demoCurrentTime, bool nwhHexColors) override {

			memset(drawBuffer, 0, sizeof(drawBuffer));
			previousTriangle = -1;

			S3L_newFrame();        // has to be called before each frame
			S3L_drawScene(scene);  /* This starts the scene rendering. The drawPixel
										function will be called to draw it. */

			lastRenderer = this; // because it has a static function callback D:

			VideoRenderer::drawFrame(demoCurrentTime, nwhHexColors);
		}
};


static inline void drawPixel(S3L_PixelInfo* p) {
	if (lastRenderer) {
		lastRenderer->drawPixel(p);
	}
}

#if DOFASTPIX

#define TOFASTPIXCOORDS(x,y,z) (y),(z),-(x)

class FastPix3DRenderer : public VideoRenderer {
	

	const float cubeVertices[24] = { 
							0.5f, -0.5f, -0.5f,
							-0.5f, -0.5f, -0.5f,
							0.5f, 0.5f, -0.5f,
							-0.5f, 0.5f, -0.5f,
							0.5f, -0.5f, 0.5f,
							-0.5f, -0.5f, 0.5f,
							0.5f, 0.5f, 0.5f,
							-0.5f, 0.5f, 0.5f };

	const int cubeVertexCount = sizeof(cubeVertices) / sizeof(cubeVertices[0]);
	const int cubeTriangleIndexes[36] = { 3, 0, 2,
							  1, 0, 3,
							  0, 4, 2,
							  2, 4, 6,
							  4, 5, 6,
							  7, 6, 5,
							  3, 7, 1,
							  1, 7, 5,
							  6, 3, 2,
							  7, 3, 6,
							  1, 4, 0,
							  5, 4, 1 };
	const int cubeIndexCount = sizeof(cubeTriangleIndexes) / sizeof(cubeTriangleIndexes[0]);

	std::vector<std::unique_ptr<Texture>>		lightmapTextures;
	std::vector<vertXYZ_t>						faceVerts;

	std::vector<std::unique_ptr<Mesh>>			renderMeshes;

	RenderStates state;
	RenderUnit ru;
public:

	FastPix3DRenderer() {
	}

	inline void startNewFrame() override {

		VideoRenderer::startNewFrame();
		renderMeshes.clear();
	};
	inline void initMap(CModel* cm) override {


		VideoRenderer::initMap(cm);

		// Load lightmaps
		for (lightmap_t& lm : mapLightmaps) {
			Bitmap* bm = new Bitmap(LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT);
			for (int i = 0; i < LIGHTMAP_WIDTH * LIGHTMAP_HEIGHT; i++) {
				bm->Pixels[i].R = lm.data[i * 3 + 0];
				bm->Pixels[i].G = lm.data[i * 3 + 1];
				bm->Pixels[i].B = lm.data[i * 3 + 2];
			}
			Texture* tex = Texture::FromBitmap(bm);
			delete bm;
			lightmapTextures.push_back(std::unique_ptr<Texture>(tex));
		}

		faceVerts = cm->GetFaceVerts();
	}
	inline virtual void resetMap() {
		VideoRenderer::resetMap();
	};
	inline drawProperties3dModel_t* startDrawingMap(CModel* cm) override {

		drawProperties3dModel_t* props =  VideoRenderer::startDrawingMap(cm);

		auto faceVertIndices = cm->GetVisFilteredFaceVertIndices(camerapos);

		Mesh* world = new Mesh();

		int count = faceVertIndices.size();
		for (int i = 0, j = 0; i < count; i++) {
			int from = faceVertIndices[i];
			vertXYZ_t* vert = &faceVerts[from];
			for (j = i + 1; j < count; j++) {
				int index2 = faceVertIndices[j];
				vertXYZ_t* vert2 = &faceVerts[index2];
				if (vert2->lightmapNum != vert->lightmapNum) {
					j--;
					break;
				}
			}
			if (vert->lightmapNum == -1) {
				// skip non lightmapped for now (Skies and some others)
				i = j;
				continue;
			}
			int to = faceVertIndices[j-1];
			int tris = (j + 1 - i) / 3;
			Surface* surf = world->AddSurface(tris*3,tris);
			surf->Texture = lightmapTextures[std::clamp((int)vert->lightmapNum,0, (int)lightmapTextures.size()-1)].get();
			surf->CullMode = CullMode::Back;

			for (int k = 0; k < tris; k++) {
				for (int b = 0; b < 3; b++) {

					int curVert = faceVertIndices[i + k * 3 + b];
					vertXYZ_t* vert3 = &faceVerts[curVert];
					surf->SetVertex(k*3+b, vfloat3(TOFASTPIXCOORDS(vert3->xyz[0], vert3->xyz[1], vert3->xyz[2])), vfloat3(0,0,0), vfloat2(vert3->lightmapSt[0], vert3->lightmapSt[1]));
				}
				surf->SetTriangle(k, k * 3, k * 3 + 1, k * 3 + 2);
			}
			i = j;
		}

		renderMeshes.push_back(std::unique_ptr<Mesh>(world));

		return props;
	};

	inline drawProperties3dModel_t* startDrawingCube(const vec3_t position, const vec3_t scale, const vec3_t rotation) override {

		drawProperties3dModel_t* props = VideoRenderer::startDrawingCube(position, scale, rotation);

		Mesh* cube = new Mesh();

		Surface* surf = cube->AddSurface(cubeVertexCount/3,cubeIndexCount/3);

		surf->set_Alpha(0.5f);
		for (int i = 0; i < cubeVertexCount / 3; i++) {
			surf->SetVertex(i,vfloat3(cubeVertices[i*3], cubeVertices[i * 3 + 1], -cubeVertices[i * 3 + 2]));
		}
		for (int i = 0; i < cubeIndexCount / 3; i++) {
			surf->SetTriangle(i,cubeTriangleIndexes[i*3], cubeTriangleIndexes[i * 3 + 1], cubeTriangleIndexes[i * 3 + 2]);
		}
		surf->CullMode = CullMode::Back;

		Matrix4 transform = Matrix4::Identity();
		if (scale) {
			transform *= Matrix4::Scale(TOFASTPIXCOORDS(scale[0], scale[1], scale[2]));
		}
		if (rotation) {
			if (rotation[PITCH]) {
				transform *= Matrix4::RotateX(rotation[PITCH]);
			}
			if (rotation[YAW]) {
				transform *= Matrix4::RotateY(-(rotation[YAW] + 180.0f));
			}
		}
		if (position) {
			transform *= Matrix4::Translate(TOFASTPIXCOORDS(position[0], position[1], position[2]));
		}
		cube->TransformVertices(transform);

		cube->SetBlendMode(BlendMode::Alpha);

		renderMeshes.push_back(std::unique_ptr<Mesh>(cube));

		return props;
	}

	inline drawProperties3dModel_t* startDrawingObject(int* indices, int indexCount, vec3_t* points, int pointCount, const vec3_t position, const vec3_t scale, const vec3_t rotation, bool backFaceCulling) override {
		drawProperties3dModel_t* props = VideoRenderer::startDrawingObject(indices, indexCount, points, pointCount, position, scale, rotation, backFaceCulling);

		Mesh* object = new Mesh();

		Surface* surf = object->AddSurface(pointCount, indexCount / 3);

		surf->set_Alpha(0.5f);
		for (int i = 0; i < pointCount; i++) {
			surf->SetVertex(i, vfloat3(TOFASTPIXCOORDS(points[i][0],points[i][1],points[i][2])));
		}
		for (int i = 0; i < indexCount / 3; i++) {
			surf->SetTriangle(i, indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);
		}
		surf->CullMode = backFaceCulling ? CullMode::Back : CullMode::None;

		Matrix4 transform = Matrix4::Identity();
		if (scale) {
			transform *= Matrix4::Scale(TOFASTPIXCOORDS(scale[0], scale[1], scale[2]));
		}
		if (rotation) {
			if (rotation[PITCH]) {
				transform *= Matrix4::RotateX(rotation[PITCH]);
			}
			if (rotation[YAW]) {
				transform *= Matrix4::RotateY(-(rotation[YAW] + 180.0f));
			}
		}
		if (position) {
			transform *= Matrix4::Translate(TOFASTPIXCOORDS(position[0], position[1], position[2]));
		}
		object->TransformVertices(transform);

		object->SetBlendMode(BlendMode::Alpha);

		renderMeshes.push_back(std::unique_ptr<Mesh>(object));

		return props;
	}

	inline void initScene(vec3_t viewangles) override {
		VideoRenderer::initScene(viewangles);


		state.ClipFar = 32768.0f;
		state.ClipNear = 1.0f;

		state.Zoom = 0.5f;

		state.ViewMatrix = Matrix4::Translate(vfloat3(TOFASTPIXCOORDS(-camerapos[0], -camerapos[1], -camerapos[2]))) *Matrix4::RotateY(viewangles[YAW]+180.0f )* Matrix4::RotateX(-viewangles[PITCH]);

		state.TextureEnable = true;
		state.TextureFilteringEnable = false;
		state.DepthMode = DepthMode::ReadWrite;
	}

	inline void drawFrame(int64_t demoCurrentTime, bool nwhHexColors) override {

		int dimAlign = 8; 
		// don't ask me why, it draws fine(?) with any dimensions, but if the buffer dimensions aren't multiples of 8,
		// it poops itself and causes memory access errors/corruption
		// maybe it does 8 rows in one go? idk
		int ceilWidth = dimAlign * ((VIDEOWIDTH + (dimAlign-1)) / dimAlign);
		int ceilHeight = dimAlign * ((VIDEOHEIGHT + (dimAlign-1)) / dimAlign);
		void* pixelBuf = _aligned_malloc(ceilWidth * ceilHeight * 4, 32);
		void* depth = _aligned_malloc(ceilWidth * ceilHeight * sizeof(float), 32);

		if (!pixelBuf || !depth) {
			if (pixelBuf) {
				_aligned_free(pixelBuf);
			}
			if (depth) {
				_aligned_free(depth);
			}
			return;
		}

		state.FrameBuffer = RenderTarget(VIDEOWIDTH, VIDEOHEIGHT, pixelBuf);
		state.DepthBuffer = RenderTarget(VIDEOWIDTH, VIDEOHEIGHT, depth);

		ru.ClearFrameBuffer(state, 0, 0, 0);
		ru.ClearDepthBuffer(state);

		int index = 0;
		for (auto& thing : renderMeshes) {
			if (index < scene3dmodelProperties.size()) {
				drawProperties3dModel_t& props = scene3dmodelProperties[index];
				if (!props.isWorld) {
					thing->SetVertexColors(props.color[0], props.color[1], props.color[2]);
				}
			}
			index++;
		}

		ThreadPool::Run(8, [this](WorkPartition workPartition)
		{
			RenderStates stateCopy = state;

			for (auto& thing : renderMeshes) {
				ru.DrawMesh(stateCopy, workPartition, *thing.get(), Matrix4::Identity());
			}
		});

		byte* asbyte = (byte*)pixelBuf;
		for (int y = 0; y < VIDEOHEIGHT; y++) {
			for (int x = 0; x < VIDEOWIDTH; x++) {
				drawBuffer[y * VIDEOWIDTH * 3 + x * 3] = asbyte[y * VIDEOWIDTH * 4 + x * 4];
				drawBuffer[y * VIDEOWIDTH * 3 + x * 3 + 1] = asbyte[y * VIDEOWIDTH * 4 + x * 4 + 1];
				drawBuffer[y * VIDEOWIDTH * 3 + x * 3 + 2] = asbyte[y * VIDEOWIDTH * 4 + x * 4 + 2];
			}
		}

		_aligned_free(pixelBuf);
		_aligned_free(depth);

		VideoRenderer::drawFrame(demoCurrentTime, nwhHexColors);
	}
};

#endif 


#endif