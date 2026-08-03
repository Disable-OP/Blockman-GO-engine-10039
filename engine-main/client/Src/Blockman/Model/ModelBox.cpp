#include "ModelBox.h"
#include "ModelRenderer.h"

#include "Tessolator/TessManager.h"
#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

ModelVertex::ModelVertex()
	: m_pos(Vector3::ZERO)
	, m_baseUV(Vector2::ZERO)
	, m_boneIdx(-1)
{}

ModelVertex::ModelVertex(const Vector3& vec, const Vector2& texpos, int boneIdx)
	: m_pos(vec)
	, m_baseUV(texpos)
	, m_boneIdx(boneIdx)
{
}

ModelVertex::ModelVertex(const ModelVertex& textureVertex, const Vector2& texPos, int boneIdx)
	: m_pos(textureVertex.m_pos)
	, m_baseUV(texPos)
	, m_boneIdx(boneIdx)
{
}

void ModelVertex::setAll(const Vector3& vec, const Vector2& uv, int boneIdx)
{
	m_pos = vec;
	m_baseUV = uv;
	m_boneIdx = boneIdx;
}

void ModelVertex::setAll(float x, float y, float z, float u, float v, int boneIdx)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	m_baseUV.x = u;
	m_baseUV.y = v;
	m_boneIdx = boneIdx;
}

TexturedQuad::TexturedQuad()
	: m_bInvertNormal(false)
{}

TexturedQuad::TexturedQuad(ModelVertex* vertices)
	: m_bInvertNormal(false)
{
	m_pVertices = vertices;
}

TexturedQuad::TexturedQuad(ModelVertex* vertices, int u1, int v1, int u2, int v2)
	: m_bInvertNormal(false)
{
	m_pVertices = vertices;
	
	m_pVertices[0].setBaseUV(float(u2), float(v1));
	m_pVertices[1].setBaseUV(float(u1), float(v1));
	m_pVertices[2].setBaseUV(float(u1), float(v2));
	m_pVertices[3].setBaseUV(float(u2), float(v2));
}

void TexturedQuad::setAll(ModelVertex* vertices, int u1, int v1, int u2, int v2)
{
	m_pVertices = vertices;

	m_pVertices[0].setBaseUV(float(u2), float(v1));
	m_pVertices[1].setBaseUV(float(u1), float(v1));
	m_pVertices[2].setBaseUV(float(u1), float(v2));
	m_pVertices[3].setBaseUV(float(u2), float(v2));
}

void TexturedQuad::flipFace()
{
	// invertNormal = true;
	for (int i = 0; i < 2; ++i)
	{
		ModelVertex temp = m_pVertices[i];
		m_pVertices[i] = m_pVertices[4 - i - 1];
		m_pVertices[4 - i - 1] = temp;
	}
}

void TexturedQuad::draw(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size, ui32 color)
{
	Vector3 tanget = m_pVertices[1].m_pos - m_pVertices[0].m_pos;
	Vector3 binormal = m_pVertices[1].m_pos - m_pVertices[2].m_pos;
	Vector3 normal = binormal.cross(tanget).normalizedCopy();
	
	/*
	if (invertNormal)
		normal = -normal;
	*/

	SkinedVertexFmt* pVetLockPtr = NULL;
	if (!renderer->getLockPtr(1, pVetLockPtr))
		return;
	
	for (int i = 0; i < 4; ++i)
	{
		SkinedVertexFmt& v = pVetLockPtr[i];
		v.setPositionEx(m_pVertices[i].m_pos.x, m_pVertices[i].m_pos.y, m_pVertices[i].m_pos.z);
		v.SetLightColor(color);
		v.SetTexcoord((m_pVertices[i].m_baseUV.x + begin.x) / size.x, (m_pVertices[i].m_baseUV.y + begin.y) / size.y);
		v.setIndex(m_pVertices[i].m_boneIdx);
	}
}

void TexturedQuad::draw(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright, ui32 color /* = 0xFFFFFFFF */)
{
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderer->getLockPtr(1, pVetLockPtr))
		return;

	for (int i = 0; i < 4; ++i)
	{
		TessVertexFmt& v = pVetLockPtr[i];
		Vector3 ipos = m_pVertices[i].m_pos * mat;
		float tu = (m_pVertices[i].m_baseUV.x + begin.x) / size.x;
		float tv = (m_pVertices[i].m_baseUV.y + begin.y) / size.y;
		v.setEx(Vector3::ZERO, ipos.x, ipos.y, ipos.z, color, bright, tu, tv);
	}

}

ModelBox::ModelBox(ModelRenderer* renderer, int boneIdx, bool mirror)
	: m_axis(Vector3::ZERO)
	, m_pitch(0.f)
	, m_roll(0.f)
	, m_yaw(0.f)
	, m_mirror(false)
	, m_invertUV(false)
	, m_delta(0.f)
	, m_textureBegin(Vector2i::ZERO)
	, m_textureExtend(Vector3i::ZERO)
	, m_boneIndex(boneIdx)
{
	vertexPositions = LordNewArrayT(ModelVertex, 24);
	quadList = LordNewArrayT(TexturedQuad, 6);
}

ModelBox::ModelBox(const Vector2i& texOff, const Vector2i& texSize, int boneIdx, bool mirror)
	: m_axis(Vector3::ZERO)
	, m_pitch(0.f)
	, m_roll(0.f)
	, m_yaw(0.f)
	, m_mirror(false)
	, m_invertUV(false)
	, m_delta(0.f)
	, m_textureBegin(Vector2i::ZERO)
	, m_textureExtend(Vector3i::ZERO)
	, m_boneIndex(boneIdx)
	, m_textureOffset(texSize)
	, m_textureSize(texSize)
{
	vertexPositions = LordNewArrayT(ModelVertex, 24);
	quadList = LordNewArrayT(TexturedQuad, 6);
}

void ModelBox::setPosition(const Vector3& pos, const Vector3& extend, float delta)
{
	pos1 = pos;
	pos2 = pos + extend;
	m_delta = delta;
}

void ModelBox::setTexCoord(const Vector2i& uv, const Vector3i& extend)
{
	m_textureBegin = uv;
	m_textureExtend = extend;
}

void ModelBox::finish()
{
	Vector3 mind = pos1 - Vector3(m_delta, m_delta, m_delta);
	Vector3 maxd = pos2 + Vector3(m_delta, m_delta, m_delta);

	if (m_mirror)
	{
		float temp = mind.x;
		mind.x = maxd.x;
		maxd.x = temp;
	}

	vertexPositions[ 0].setAll(maxd.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 1].setAll(maxd.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 2].setAll(maxd.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 3].setAll(maxd.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);

	vertexPositions[ 4].setAll(mind.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 5].setAll(mind.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 6].setAll(mind.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 7].setAll(mind.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);

	vertexPositions[ 8].setAll(maxd.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[ 9].setAll(mind.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[10].setAll(mind.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[11].setAll(maxd.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);

	vertexPositions[12].setAll(maxd.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[13].setAll(mind.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[14].setAll(mind.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[15].setAll(maxd.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);

	vertexPositions[16].setAll(maxd.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[17].setAll(mind.x, mind.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[18].setAll(mind.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[19].setAll(maxd.x, maxd.y, mind.z, 0.0F, 0.0F, m_boneIndex);

	vertexPositions[20].setAll(mind.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[21].setAll(maxd.x, mind.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[22].setAll(maxd.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);
	vertexPositions[23].setAll(mind.x, maxd.y, maxd.z, 0.0F, 0.0F, m_boneIndex);

	float x = float(m_textureOffset.x);
	float y = float(m_textureOffset.y);
	float w = float(m_textureSize.x);
	float h = float(m_textureSize.y);
	const Vector2i& uv = m_textureBegin;
	const Vector3i& ex = m_textureExtend;

	if (m_invertUV)
	{
		quadList[0].setAll(vertexPositions + 0, uv.x + ex.z + ex.x, uv.y + ex.z, uv.x + ex.z + ex.x + ex.z, uv.y + ex.z + ex.y);
		quadList[1].setAll(vertexPositions + 4, uv.x, uv.y + ex.z, uv.x + ex.z, uv.y + ex.z + ex.y);
		quadList[2].setAll(vertexPositions + 8, uv.x + ex.z, uv.y, uv.x + ex.z + ex.x, uv.y + ex.z);
		quadList[3].setAll(vertexPositions + 12, uv.x + ex.z + ex.x, uv.y + ex.z, uv.x + ex.z + ex.x + ex.x, uv.y);
		quadList[4].setAll(vertexPositions + 16, uv.x + ex.z, uv.y + ex.z, uv.x + ex.z + ex.x, uv.y + ex.z + ex.y);
		quadList[5].setAll(vertexPositions + 20, uv.x + ex.z + ex.x + ex.z, uv.y + ex.z, uv.x + ex.z + ex.x + ex.z + ex.x, uv.y + ex.z + ex.y);
	}
	else
	{
		quadList[0].setAll(vertexPositions + 0, uv.x + ex.z + ex.x, uv.y + ex.z + ex.y, uv.x + ex.z + ex.x + ex.z, uv.y + ex.z);
		quadList[1].setAll(vertexPositions + 4, uv.x, uv.y + ex.z + ex.y, uv.x + ex.z, uv.y + ex.z);
		quadList[2].setAll(vertexPositions + 8, uv.x + ex.z + ex.x, uv.y, uv.x + ex.z + ex.x + ex.x, uv.y + ex.z);
		quadList[3].setAll(vertexPositions + 12, uv.x + ex.z, uv.y + ex.z, uv.x + ex.z + ex.x, uv.y);
		quadList[4].setAll(vertexPositions + 16, uv.x + ex.z, uv.y + ex.z + ex.y, uv.x + ex.z + ex.x, uv.y + ex.z);
		quadList[5].setAll(vertexPositions + 20, uv.x + ex.z + ex.x + ex.z, uv.y + ex.z + ex.y, uv.x + ex.z + ex.x + ex.z + ex.x, uv.y + ex.z);
	}

	if (m_mirror)
	{
		for (int i = 0; i < 6; ++i)
		{
			quadList[i].flipFace();
		}
	}
}

ModelBox::~ModelBox()
{
	LordDeleteArrayT(vertexPositions, ModelVertex, 24);
	LordDeleteArrayT(quadList, TexturedQuad, 6);
}

void ModelBox::render(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size, ui32 bright)
{
	for (int i = 0; i < 6; ++i)
	{
		quadList[i].draw(renderer, begin, size, bright);
	}
}

void ModelBox::render(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright, ui32 color /* = 0xFFFFFFFF */)
{
	for (int i = 0; i < 6; ++i)
	{
		quadList[i].draw(renderer, mat, begin, size, bright, color);
	}
}

Matrix4 ModelBox::getBoneMatrix()
{
	Matrix4 mat;
	// Matrix4::RotateYawPitchRoll(mat, m_yaw , m_pitch , m_roll);
	mat.identity();
	mat.rotateY(m_yaw);
	mat.rotateX(m_pitch);
	mat.rotateZ(m_roll);
	mat.translate(m_axis);
	return mat;
}

void ModelBox::caculateBoneMatrix(Vector4* pMat3x4)
{
	Matrix4 mat;
	// Matrix4::RotateYawPitchRoll(mat, m_yaw , m_pitch , m_roll);
	mat.identity();
	mat.rotateY(m_yaw);
	mat.rotateX(m_pitch);
	mat.rotateZ(m_roll);
	mat.translate(m_axis);

	pMat3x4[0].x = mat.m00; 
	pMat3x4[0].y = mat.m10;
	pMat3x4[0].z = mat.m20;
	pMat3x4[0].w = mat.m30;

	pMat3x4[1].x = mat.m01;
	pMat3x4[1].y = mat.m11;
	pMat3x4[1].z = mat.m21;
	pMat3x4[1].w = mat.m31;

	pMat3x4[2].x = mat.m02;
	pMat3x4[2].y = mat.m12;
	pMat3x4[2].z = mat.m22;
	pMat3x4[2].w = mat.m32;
}

}