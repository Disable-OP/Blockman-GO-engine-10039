/********************************************************************
filename: 	BatchRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-07-03
*********************************************************************/

#ifndef __LORD_STANDARD_RENDERABLE_H__
#define __LORD_STANDARD_RENDERABLE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API BatchRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	BatchRenderable();
	~BatchRenderable();

	BATCH_RENDER_PASS getpass() const { return m_RenderGroup; }
	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	void setMaterialParam(Texture* texture, BATCH_RENDER_PASS group);
	inline Texture* getTexture() const { return m_Texture; }
	inline BATCH_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }
	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	float* getVertexMenory() { return m_VertexSysMemory; }


	inline bool getLockPtr(ui16 quadNum, BatchVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (BatchVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(BatchVertexFmt) / sizeof(float));
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

	void endPolygon();
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }

protected:
	BATCH_RENDER_PASS m_RenderGroup;

	ui16			m_quadPoolSize;
	ui16			m_quadNum;
	
	float*			m_VertexSysMemory;
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;

	Texture*		m_Texture;
};

}

#endif