/********************************************************************
filename: 	Rasterizer.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-30
*********************************************************************/
#ifndef __RENDER_RASTERIZER__
#define __RENDER_RASTERIZER__

#include "BM_TypeDef.h"
struct TessRasterize;

namespace BLOCKMAN
{

class RenderGlobal;


enum FaceDirBit
{
	emFaceX_Minus = 1 << 0,
	emFaceX_Plus = 1 << 1,
	emFaceY_Minus = 1 << 2,
	emFaceY_Plus = 1 << 3,
	emFaceZ_Minus = 1 << 4,
	emFaceZ_Plus = 1 << 5,
	emFace_All = emFaceX_Minus | emFaceX_Plus | emFaceY_Minus | emFaceY_Plus | emFaceZ_Minus | emFaceZ_Plus,
};

class RasterizerMesh : public ObjectAlloc
{
public:
	RasterizerMesh();
	~RasterizerMesh();
	
	bool isEmpty() { return m_rasterVertex.empty(); }
	void clean();
	int findSuitableIndicesBuffer(const Vector3& campos, const Vector3& camDir, bool softOccCulling = false);
	void AddMeshQuad(const Vector3& leftdown, const Vector3& leftup, const Vector3& rightup, const Vector3& rightdown, int indicebuffers[], int count);
public:
	
	vector<LORD::TessRasterize>::type m_rasterVertex;
	vector<ui16>::type m_rasterIndex[9];

	Box m_aabb;
	Vector3i m_sectionId;
};

struct TileOccludedInfo
{
	int nFrame;
	bool isVisible;
};

class Rasterizer : public ObjectAlloc
{
public:
	Rasterizer(RenderGlobal* render, int w, int h);
	virtual ~Rasterizer();
	inline void SetViewProject(const Matrix4& mat) { projMatrixPlusviewMatrix =mat; }
	inline void ClearBuffer() { memset(m_depthBuffer, 255, Width * Height); }
	void ResetIfNeed(float aspect)
	{
		int h = static_cast<int>(ceil(static_cast<float>(Width) / aspect));
		if (Height != h)
		{
			Height = h;
			LordSafeFree(m_depthBuffer);
			m_depthBuffer = (ui8*)LordMalloc(Width*Height);
		}
	}
	void PerformRaterization(const Vector3i& sectionId);
	bool TestAABBVisible(const Vector3i& tileId);
	void RasterMesh(const Vector3& positionOffset, int numVertices, int numIndices, LORD::TessRasterize* meshVertexs, ui16* meshIndices);
	bool TestAABBVisible(const Box& bbox);

	void SetNearFar(float _near, float _far) { zFar = _far; zNear = _near; }
	ui8* m_depthBuffer;

private:
	void ProcessTriangle(Vector3& p1, Vector3& p2, Vector3& p3);
	void ProcessScanLine(int y, const Vector3& pa, const Vector3& pb, const Vector3& pc, const Vector3& pd, float yBa, float yDc);
	bool WorldToScreen(const Matrix4& viewProj, const Vector3& position, Vector3& transformedPos) const;

	inline void ViewPortToScreen(Vector3& transformedPos) const
	{
		transformedPos.x = (transformedPos.x + 1.0f) * 0.5f * Width;
		transformedPos.y = (transformedPos.y + 1.0f) * 0.5f * Height;
		float _zc1 = zFar / zNear;
		float _zc0 = 1.0f - _zc1;
		transformedPos.z = (1.0f / (transformedPos.z - 1.0f) - _zc1 / zFar) * zFar / _zc0;
	}

	inline ui8 GetDepth(int x, int y)
	{
		int index = x + y * Width;
		return m_depthBuffer[index];
	}

	inline float Interpolate(float x0, float x1, float alpha)
	{
		return x0 + (x1 - x0)*alpha;
	}

	int Width;
	int Height;
	RenderGlobal* m_render;
	Matrix4 projMatrixPlusviewMatrix;
	float zFar;
	float zNear;

	//map<Vector3i, TileOccludedInfo>::type mapTileCullingInfo;
};
}

#endif