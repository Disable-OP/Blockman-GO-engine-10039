#include "Rasterizer.h"
#include "RenderGlobal.h"
#include "SectionRenderer.h"

#include "Tessolator/TessHeader.h"

namespace BLOCKMAN
{

int visibleLodFlags[8] =
{
	emFaceX_Plus | emFaceY_Plus | emFaceZ_Plus,
	emFaceX_Minus | emFaceY_Plus | emFaceZ_Plus,
	emFaceX_Minus | emFaceY_Plus | emFaceZ_Minus,
	emFaceX_Plus | emFaceY_Plus | emFaceZ_Minus,
	emFaceX_Plus | emFaceY_Minus | emFaceZ_Plus,
	emFaceX_Minus | emFaceY_Minus | emFaceZ_Plus,
	emFaceX_Minus | emFaceY_Minus | emFaceZ_Minus,
	emFaceX_Plus | emFaceY_Minus | emFaceZ_Minus
};

RasterizerMesh::RasterizerMesh()
{
}

RasterizerMesh::~RasterizerMesh()
{
	clean();
}

void RasterizerMesh::clean()
{
	m_rasterVertex.clear();
	for (int i = 0; i < 9; ++i)
	{
		m_rasterIndex[i].clear();
	}
	m_aabb.reset();
}

void RasterizerMesh::AddMeshQuad(const Vector3& leftdown, const Vector3& leftup, const Vector3& rightup, const Vector3& rightdown, int indicebuffers[], int count)
{
	int baseidx = m_rasterVertex.size();

	m_rasterVertex.push_back(LORD::TessRasterize(leftdown));
	m_rasterVertex.push_back(LORD::TessRasterize(leftup));
	m_rasterVertex.push_back(LORD::TessRasterize(rightup));
	m_rasterVertex.push_back(LORD::TessRasterize(rightdown));
	Vector3 offset = Vector3(m_sectionId) * 16.f;
	m_aabb.addPoint(offset + leftdown);
	m_aabb.addPoint(offset + leftup);
	m_aabb.addPoint(offset + rightup);
	m_aabb.addPoint(offset + rightdown);

	for (int i = 0; i < count; ++i)
	{
		if (indicebuffers[i] == -1)
		{
			continue;
		}

		auto& triangles = m_rasterIndex[indicebuffers[i]];
		triangles.push_back(baseidx);
		triangles.push_back(baseidx + 1);
		triangles.push_back(baseidx + 2);
		triangles.push_back(baseidx);
		triangles.push_back(baseidx + 2);
		triangles.push_back(baseidx + 3);
	}

	auto& alltriangles = m_rasterIndex[8];
	alltriangles.push_back(baseidx);
	alltriangles.push_back(baseidx + 1);
	alltriangles.push_back(baseidx + 2);
	alltriangles.push_back(baseidx);
	alltriangles.push_back(baseidx + 2);
	alltriangles.push_back(baseidx + 3);
}

int RasterizerMesh::findSuitableIndicesBuffer(const Vector3& campos, const Vector3& camDir, bool softOccCulling)
{
	float dotX = camDir.dot(Vector3::UNIT_X);
	float dotY = camDir.dot(Vector3::UNIT_Y);
	float dotZ = camDir.dot(Vector3::UNIT_Z);
	float absDotX = Math::Abs(dotX);
	float absDotZ = Math::Abs(dotZ);

	int result = 0;
	if (campos.x > m_aabb.vMax.x)
		result |= emFaceX_Plus;
	else if (campos.x < m_aabb.vMin.x)
		result |= emFaceX_Minus;
	else
	{
		if (softOccCulling)
		{
			if (absDotZ < 0.5f)
			{
				if (dotX < 0)
					result |= emFaceX_Plus;
				else
					result |= emFaceX_Minus;
			}
			else
				result = emFace_All;
		}
		else
			result = emFace_All;
	}

	if (campos.y > m_aabb.vMax.y)
		result |= emFaceY_Plus;
	else if (campos.y < m_aabb.vMin.y)
		result |= emFaceY_Minus;
	else
	{
		if (softOccCulling)
		{
			if (dotY < 0)
				result |= emFaceY_Plus;
			else
				result |= emFaceY_Minus;
		}
		else
			result = emFace_All;
	}

	if (campos.z > m_aabb.vMax.z)
		result |= emFaceZ_Plus;
	else if (campos.z < m_aabb.vMin.z)
		result |= emFaceZ_Minus;
	else
	{
		if (softOccCulling)
		{
			if (absDotX < 0.5f)
			{
				if (dotZ < 0)
					result |= emFaceZ_Plus;
				else
					result |= emFaceZ_Minus;
			}
			else
				result = emFace_All;
		}
		else
			result = emFace_All;
	}

	for (unsigned int i = 0; i < 8; ++i)
	{
		if (visibleLodFlags[i] == result)
			return i;
	}
	return 8;
}

Rasterizer::Rasterizer(RenderGlobal* render, int w, int h)
{
	Width = w;
	Height = h;
	m_render = render;
	m_depthBuffer = (ui8*)LordMalloc(Width * Height);
}

Rasterizer::~Rasterizer()
{
	LordSafeFree(m_depthBuffer);
}

bool Rasterizer::WorldToScreen(const Matrix4& m, const Vector3& v, Vector3& dst) const
{
	//if (!TransformVector(viewProj, position, clipPos))
	//	return false;
	float w = m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33;
	if (w <= 0.0f)
	{
		dst.x = 0.0f;
		dst.y = 0.0f;
		dst.z = 0.0f;
		return false;
	}

	Vector4 wpos = Vector4(v.x, v.y, v.z, 1.0f);
	Vector4 ret = wpos * m;

	float invW = 1.0f / w;
	dst.x = ret.x * invW;
	dst.y = ret.y * invW;
	dst.z = ret.z * invW;

	ViewPortToScreen(dst);
	return true;
}

void Rasterizer::PerformRaterization(const Vector3i& sectionId)
{
	SectionRenderer* pSection = m_render->getSectionRender(sectionId * 16);
	if (nullptr == pSection)
		return;
	RasterizerMesh* mesh = pSection->m_rasterizerMesh;
	if (nullptr == mesh)
		return;
	if (mesh->m_sectionId != sectionId)
		return;
	if (mesh->isEmpty())
		return;

	Vector3 campos = m_render->m_mainCamera->getPosition();
	Vector3 camDir = m_render->m_mainCamera->getDirection();

	Vector3 positionOffset = Vector3(mesh->m_sectionId) * Vector3(16.f, 16.f, 16.f);
	
	int part = mesh->findSuitableIndicesBuffer(campos, camDir, true);
	int numIndices = mesh->m_rasterIndex[part].size();
	if (numIndices == 0)
		return;
	ui16* meshIndices = (ui16*)(&(mesh->m_rasterIndex[part][0]));
	const int numVertices = mesh->m_rasterVertex.size();
	LORD::TessRasterize* meshVertexs = (LORD::TessRasterize*)(&(mesh->m_rasterVertex[0]));
	
	RasterMesh(positionOffset, numVertices, numIndices, meshVertexs, meshIndices);
	
	//for debug, make a 14*14*14 cube.
	/*
	TessRasterize meshVertexs[8];
	meshVertexs[0].SetPosition(6.00f, 10.0f, 6.00f);
	meshVertexs[1].SetPosition(10.0f, 10.0f, 6.00f);
	meshVertexs[2].SetPosition(10.0f, 10.0f, 10.0f);
	meshVertexs[3].SetPosition(6.00f, 10.0f, 10.0f);
	meshVertexs[4].SetPosition(6.00f, 6.00f, 6.00f);
	meshVertexs[5].SetPosition(10.0f, 6.00f, 6.00f);
	meshVertexs[6].SetPosition(10.0f, 6.00f, 10.0f);
	meshVertexs[7].SetPosition(6.00f, 6.00f, 10.0f);
	ui16 meshIndices[36] =
	{
		0,1,4,4,1,5,
		1,2,5,5,2,6,
		2,3,6,6,3,7,
		3,0,7,7,0,4,
		3,2,0,0,2,1,
		5,4,6,6,4,7
	};

	RasterMesh(positionOffset, 8, 36, meshVertexs, meshIndices);*/
}


bool Rasterizer::TestAABBVisible(const Vector3i& sectionId)
{
	Vector3 _min(static_cast<float>(sectionId.x * 16), static_cast<float>(sectionId.y * 16), static_cast<float>(sectionId.z * 16));
	Vector3 _max(static_cast<float>((sectionId.x + 1) * 16), static_cast<float>((sectionId.y + 1) * 16), static_cast<float>((sectionId.z + 1) * 16));
	Box bbox(_min, _max);
	bool visible = TestAABBVisible(bbox);
	/*auto& record = mapTileCullingInfo[sectionId];
	if (visible)
	{
		record.nFrame = 0;
		record.isVisible = true;
	}
	else
	{
		if (record.nFrame >= 5)
		{
			record.isVisible = false;
		}
		else
		{
			visible = record.isVisible;
			++record.nFrame;
		}
	}
	if (mapTileCullingInfo.size() > 1024)
		mapTileCullingInfo.clear();*/
	return visible;
}

bool Rasterizer::TestAABBVisible(const Box& bbox)
{
	Vector3 bpoints[8];
	bbox.getPoints(bpoints);
	Vector3 wpos;
	Vector3 screenPos;
	Vector3 screenPosMin(99999.9f, 99999.9f, 99999.9f);
	Vector3 screenPosMax(-99999.9f, -99999.9f, -99999.9f);
	float minZ = 255.0f, maxZ = 0.0f;
	const Matrix4& m = projMatrixPlusviewMatrix;
	for (int i = 0; i < 8; ++i)
	{
		wpos.x = bpoints[i].x;
		wpos.y = bpoints[i].y;
		wpos.z = bpoints[i].z;
		if (!WorldToScreen(m, wpos, screenPos))
			return true;

		screenPosMin.x = Math::Min<float>(screenPos.x, screenPosMin.x);
		screenPosMin.y = Math::Min<float>(screenPos.y, screenPosMin.y);
		minZ = Math::Min<float>(screenPos.z, minZ);

		screenPosMax.x = Math::Max<float>(screenPos.x, screenPosMax.x);
		screenPosMax.y = Math::Max<float>(screenPos.y, screenPosMax.y);
		maxZ = Math::Max<float>(screenPos.z, maxZ);
	}

	if (maxZ < 0.0f)
		return false;

	int minIntZ = static_cast<int>(minZ);

	screenPosMin.x = Math::Clamp<float>(screenPosMin.x - 1, 0.f, static_cast<float>(Width - 1));
	screenPosMin.y = Math::Clamp<float>(screenPosMin.y - 1, 0.f, static_cast<float>(Height - 1));
	screenPosMax.x = Math::Clamp<float>(screenPosMax.x + 1, 0.f, static_cast<float>(Width - 1));
	screenPosMax.y = Math::Clamp<float>(screenPosMax.y + 1, 0.f, static_cast<float>(Height - 1));
	if (GetDepth(static_cast<int>(screenPosMin.x), static_cast<int>(screenPosMin.y)) >= minIntZ ||
		GetDepth(static_cast<int>(screenPosMin.x), static_cast<int>(screenPosMax.y)) >= minIntZ ||
		GetDepth(static_cast<int>(screenPosMax.x), static_cast<int>(screenPosMin.y)) >= minIntZ ||
		GetDepth(static_cast<int>(screenPosMax.x), static_cast<int>(screenPosMax.y)) >= minIntZ)
		return true;

	int startX = static_cast<int>(screenPosMin.x);
	int endX = static_cast<int>(screenPosMax.x);
	int startY = static_cast<int>(screenPosMin.y);
	int endY = static_cast<int>(screenPosMax.y);

	bool ret = false;
	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (GetDepth(x, y) >= minIntZ)
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}

void Rasterizer::RasterMesh(const Vector3& positionOffset, int numVertices, int numIndices, LORD::TessRasterize* meshVertexs, ui16* meshIndices)
{
	Vector3 screenPos[3];
	const Matrix4& m = projMatrixPlusviewMatrix;

	for (int j = 0; j < numIndices; j += 3)
	{
		if (!WorldToScreen(m, meshVertexs[meshIndices[j + 0]].GetPosition() + positionOffset, screenPos[0]))
			continue;
		if (!WorldToScreen(m, meshVertexs[meshIndices[j + 1]].GetPosition() + positionOffset, screenPos[1]))
			continue;
		if (!WorldToScreen(m, meshVertexs[meshIndices[j + 2]].GetPosition() + positionOffset, screenPos[2]))
			continue;
		// Rasterize triangles from transformed vertices
		ProcessTriangle(screenPos[0], screenPos[1], screenPos[2]);
	}
}

void Rasterizer::ProcessTriangle(Vector3& p1, Vector3& p2, Vector3& p3)
{
	// Sort the points in order to always have them ordered like following: p1, p2, p3
	if (p1.y > p2.y)
	{
		Vector3 temp = p2;
		p2 = p1;
		p1 = temp;
	}
	if (p2.y > p3.y)
	{
		Vector3 temp = p2;
		p2 = p3;
		p3 = temp;
	}
	if (p1.y > p2.y)
	{
		Vector3 temp = p2;
		p2 = p1;
		p1 = temp;
	}

	// Helper values
	float y21 = p2.y - p1.y;
	float y31 = p3.y - p1.y;
	float y32 = p3.y - p2.y;
	float x21 = p2.x - p1.x;
	float x31 = p3.x - p1.x;

	float y21Inv = Math::IsEqual(y21, 0.0f) ? 1.0f : 1.0f / y21;
	float y31Inv = Math::IsEqual(y31, 0.0f) ? 1.0f : 1.0f / y31;
	float y32Inv = Math::IsEqual(y32, 0.0f) ? 1.0f : 1.0f / y32;

	// Compute slopes
	float dP1P2 = (y21 > 0.0f) ? x21 * y21Inv : 0.0f;
	float dP1P3 = (y31 > 0.0f) ? x31 * y31Inv : 0.0f;

	// Compute min/max for Y axis
	int minY = static_cast<int>(Math::Ceil(p1.y));
	int maxY = static_cast<int>(Math::Floor(p3.y));
	int p2Y = static_cast<int>(Math::Floor(p2.y));

	minY = Math::Max<int>(0, minY);
	maxY = Math::Min<int>(maxY, Height - 1);
	p2Y = Math::Max<int>(0, p2Y);
	p2Y = Math::Min<int>(p2Y, Height - 1);

	int y;

	// P1
	// -
	// -- 
	// - -
	// -  -
	// -   - P2
	// -  -
	// - -
	// -
	// P3
	if (dP1P2 > dP1P3)
	{
		for (y = minY; y < p2Y; y++)
			ProcessScanLine(y, p1, p3, p1, p2, y31Inv, y21Inv);
		for (y = p2Y; y <= maxY; y++)
			ProcessScanLine(y, p1, p3, p2, p3, y31Inv, y32Inv);
	}

	//       P1
	//        -
	//       -- 
	//      - -
	//     -  -
	// P2 -   - 
	//     -  -
	//      - -
	//        -
	//       P3
	else
	{
		for (y = minY; y < p2Y; y++)
			ProcessScanLine(y, p1, p2, p1, p3, y21Inv, y31Inv);
		for (y = p2Y; y <= maxY; y++)
			ProcessScanLine(y, p2, p3, p1, p3, y32Inv, y31Inv);
	}
}

// drawing line between 2 points from left to right
// papb -> pcpd
void Rasterizer::ProcessScanLine(int y, const Vector3& pa, const Vector3& pb, const Vector3& pc, const Vector3& pd, float yBa, float yDc)
{
	// Starting Z and ending Z
	float z1 = Math::Min<float>(pa.z, pb.z);
	float z2 = Math::Min<float>(pc.z, pd.z);

	if (z1 < 0.0f && z2 < 0.0f)
		return;

	float gradient1 = Math::Clamp<float>((pa.y != pb.y ? (y - pa.y) * yBa : 1.0f), 0.0f, 1.0f);
	float gradient2 = Math::Clamp<float>((pc.y != pd.y ? (y - pc.y) * yDc : 1.0f), 0.0f, 1.0f);

	// Starting X and ending X
	int sx = (int)Math::Ceil(Interpolate(pa.x, pb.x, gradient1));
	int ex = (int)Math::Floor(Interpolate(pc.x, pd.x, gradient2));
	sx = Math::Max<int>(0, sx);
	ex = Math::Min<int>(ex, Width - 1);
	ui8 zocc = Math::Max<ui8>(0, static_cast<ui8>(Math::Floor(Math::Min<float>(z1, z2))));

	for (int x = sx; x <= ex; ++x)
	{
		int index = x + y * Width;
		if (m_depthBuffer[index] > zocc)
			m_depthBuffer[index] = zocc;
	}
}

}
