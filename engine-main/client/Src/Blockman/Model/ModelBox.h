/********************************************************************
filename: 	ModelBox.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __MODEL_BOX_HEADER__
#define __MODEL_BOX_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class ModelRenderer;

class ModelVertex
{
public:
	Vector3 m_pos;
	Vector2 m_baseUV;
	int		m_boneIdx = 0;

public:

	ModelVertex();
	ModelVertex(const Vector3& vec, const Vector2& texpos, int boneIdx);
	ModelVertex(const ModelVertex& textureVertex, const Vector2& texPos, int boneIdx);

	void setVertex(const Vector3& vec) { m_pos = vec; }
	void setBaseUV(const Vector2& uv) { m_baseUV = uv; }
	void setBaseUV(float u, float v) { m_baseUV.x = u; m_baseUV.y = v; }
	void setAll(const Vector3& vec, const Vector2& uv, int boneIdx);
	void setAll(float x, float y, float z, float u, float v, int boneIdx);
};


class TexturedQuad
{
protected:
	bool m_bInvertNormal = false;

public:
	ModelVertex* m_pVertices = nullptr;

public:
	TexturedQuad();
	TexturedQuad(ModelVertex* vertices);
	TexturedQuad(ModelVertex* vertices, int u1, int v1, int u2, int v2);

	void setAll(ModelVertex* vertices, int u1, int v1, int u2, int v2);
	void flipFace();
	/** Draw this primitve. This is typically called only once as the generated drawing instructions are saved by the renderer and reused later. */
	void draw(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size, ui32 color = 0xFFFFFFFF);
	void draw(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright, ui32 color = 0xFFFFFFFF);
};

class ModelBox : public ObjectAlloc
{
protected:
	/** The (x,y,z) vertex positions and (u,v) texture coordinates for each of the 8 points on a cube */
	ModelVertex* vertexPositions = nullptr;
	/** An array of 6 TexturedQuads, one for each face of a cube */
	TexturedQuad* quadList = nullptr;
	/** is mirror? */
	bool m_mirror = false;
	bool m_invertUV = false;
	float m_delta = 0.f;
	Vector2i m_textureBegin;
	Vector3i m_textureExtend;
	Vector2i m_textureOffset;
	Vector2i m_textureSize;
	int m_boneIndex = 0;

public:
	/** vertex of lower box corner */
	Vector3 pos1;
	/** vertex of upper box corner */
	Vector3 pos2;
	/** the bone position.*/
	Vector3 m_axis;
	String boxName;

	/** all the Blockman Render entity has no translate movement */
	float m_pitch = 0.f;
	float m_yaw = 0.f;
	float m_roll = 0.f;

public:
	ModelBox(ModelRenderer* renderer, int boneIdx, bool mirror = false);
	ModelBox(const Vector2i& texOff, const Vector2i& texSize, int boneIdx, bool mirror = false);
	~ModelBox();

	void setMirror(bool mirror) { m_mirror = mirror; }
	void setInvertUV(bool invertUV) { m_invertUV = invertUV; }
	void setPosition(const Vector3& pos, const Vector3& extend, float delta = 0.f);
	void setTexCoord(const Vector2i& uv, const Vector3i& extend);
	void finish();

	void render(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size, ui32 color = 0xFFFFFFFF);
	void render(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright, ui32 color = 0xFFFFFFFF);
	void setBoxName(const String& name) { boxName = name; }
	void setRotateAxis(const Vector3& axis) { m_axis = axis; }
	void caculateBoneMatrix(Vector4* pMat3x4);
	Matrix4 getBoneMatrix();
};

}

#endif