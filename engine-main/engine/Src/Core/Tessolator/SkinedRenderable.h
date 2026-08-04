/********************************************************************
filename: 	EntityRenderable.h
file path:	dev\engine\Src\Core\Tessellator\

version:	1
author:		ajohn
company:	supernano
datatime:   2017-03-09
*********************************************************************/

#ifndef __LORD_ENTITY_RENDERABLE_H__
#define __LORD_ENTITY_RENDERABLE_H__

#include "Core.h"

namespace LORD
{

class TessManager;

class LORD_CORE_API SkinedRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	SkinedRenderable();
	~SkinedRenderable();

	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	//void setExtraSkyBlockLight(const Color& b) { m_extraSkyBlockLight = b; }
	void setExtraColor(const Color& c) { m_extraVertexColor = c; }
	void setBoneMatrices(Vector4* boneMatrices) { m_boneMatrices = boneMatrices; }
	void setWorldMatrix(Matrix4* worldMat) { m_worldMat = worldMat; }
	void setBoneCount(int count) { LordAssert(count <= s_maxBone); m_boneCount = count; }
	void setViewSpace(bool viewer) { m_viewSpace = viewer; }
	Matrix4* getWorldMatrix() const { return m_worldMat; }
	Vector4* getBoneMatrices() const { return m_boneMatrices; }
	int getBoneCount() const { return m_boneCount; }
	//const Color& getExtraSkyBlockLight() const { return m_extraSkyBlockLight; }
	const Color& getExtraColor() const { return m_extraVertexColor; }
	bool isViewSpace() const { return m_viewSpace; }

	void setMaterialParam(Texture* texture, SKINED_RENDER_PASS group);
	inline Texture* getTexture() const { return m_Texture; }
	inline SKINED_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }
	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();

	inline bool getLockPtr(ui16 quadNum, SkinedVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (SkinedVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(SkinedVertexFmt) / sizeof(float));
			m_quadNum += quadNum;
		}
		else
		{
			return false;
		}

		return true;
	}

	inline void reversePtr(ui16 vertexNum)
	{
		m_quadNum -= vertexNum;
	}

	void endPolygon(bool needBatch = false);
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }

	bool needBatch() { return m_VertexSysMemory != nullptr; }
	void batch(BatchVertexFmt* pDst);

public:
	static const int s_maxBone = 60;

protected:
	SKINED_RENDER_PASS m_RenderGroup;

	ui16			m_quadPoolSize;
	ui16			m_quadNum;

	//Color			m_extraSkyBlockLight;
	Color			m_extraVertexColor;
	
	float*			m_VertexSysMemory;
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;

	Texture*		m_Texture;
	Vector4*		m_boneMatrices;
	int				m_boneCount;
	Matrix4*		m_worldMat;
	bool			m_viewSpace;
};

}

#endif