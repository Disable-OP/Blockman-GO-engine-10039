/********************************************************************
filename: 	FxRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-08-01
*********************************************************************/

#ifndef __LORD_FX_RENDERABLE_H__
#define __LORD_FX_RENDERABLE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API FxRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	FxRenderable();
	~FxRenderable();

	FX_RENDER_PASS getpass() const { return m_RenderGroup; }
	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	void setMaterialParam(Texture* texture, FX_RENDER_PASS group = FRP_DEFAULT);
	inline Texture* getTexture() const { return m_Texture; }
	inline FX_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }
	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	float* getVertexMenory() { return m_VertexSysMemory; }


	inline bool getLockPtr(ui16 quadNum, FxVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (FxVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(FxVertexFmt) / sizeof(float));
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

	void endPolygon();
	bool empty() { return m_quadNum == 0; }
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }
	void setFreeQuadNum(ui16 freeNum) { m_quadNum = m_quadPoolSize - freeNum; }
	void setViewSpace(bool viewer) { m_viewSpace = viewer; }
	bool isViewSpace() const { return m_viewSpace; }

protected:
	FX_RENDER_PASS m_RenderGroup;

	ui16			m_quadPoolSize;
	ui16			m_quadNum;

	float*			m_VertexSysMemory;  // no need to release in running
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;

	Texture*		m_Texture;

	bool			m_created;
	bool			m_viewSpace;
};

}

#endif
