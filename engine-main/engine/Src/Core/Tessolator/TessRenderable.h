/********************************************************************
filename: 	TessRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2016-11-29
*********************************************************************/

#ifndef __LORD_TESSOLATOR_RENDERABLE_H__
#define __LORD_TESSOLATOR_RENDERABLE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{

class LORD_CORE_API TessRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	TessRenderable();
	~TessRenderable();

	TESS_RENDER_PASS getpass() const { return m_RenderGroup; }
	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	void setMaterialParam(Texture* texture, TESS_RENDER_PASS group);
	inline Texture* getTexture() const { return m_Texture; }
	inline TESS_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }
	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	float* getVertexMenory() { return m_VertexSysMemory; }

	inline bool getLockPtr(ui16 quadNum, TessVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (TessVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(TessVertexFmt) / sizeof(float));
			m_quadNum += quadNum;
		}
		else
		{
			return false;
		}

		return true;
	}

	inline bool getLockPtr(ui16 quadNum, SkinedVertexFmt*& vertexAddr)
	{
		return false;
	}

	inline void reversePtr(ui16 vertexNum)
	{
		m_quadNum -= vertexNum;
	}

	void setVertexAlpha(float alpha);
	void endPolygon();
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }

	void setPosition(const Vector3& pos) { m_position = pos; }
	const Vector3& getPosition() const { return m_position; }
	Vector4 updateUVParam(int frameCount);

protected:
	TESS_RENDER_PASS m_RenderGroup;

	ui16			m_quadPoolSize;
	ui16			m_quadNum;

	Vector3			m_position;

	float*			m_VertexSysMemory;
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;

	Texture*		m_Texture;
};

}

#endif