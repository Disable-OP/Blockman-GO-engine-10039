/********************************************************************
filename: 	LineRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-04-06
*********************************************************************/

#ifndef __LORD_LINE_RENDERABLE_H__
#define __LORD_LINE_RENDERABLE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API LineRenderable : public ObjectAlloc
{
public:
	LineRenderable(LINE_RENDER_PASS pass);
	~LineRenderable();

	void setPointPoolSize(ui16 pointSize);
	void destroyVertexPool();

	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();

	bool getLockPtr_line(ui16 pointNum, LineVertexFmt*& vertexAddr);
	bool getLockPtr_seg(ui16 pointNum, LineVertexFmt*& vertexAddr);
	
	void endPolygon();
	ui16 getPointNum() { return m_pointNum; }
	ui16 getLineSegNum() { return m_lineSegNum; }
	ui16 getFreePointNum() { return m_pointPoolSize - m_pointNum; }

	inline LINE_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }

protected:
	LINE_RENDER_PASS	m_RenderGroup;
	ui16				m_pointPoolSize;
	ui16				m_pointNum;
	ui16				m_lineSegNum;
	
	float*				m_VertexSysMemory;
	i16*				m_IndexSysMemory;
	GPUBuffer*			m_VertexBuffer;
	GPUBuffer*			m_IndexBuffer;
	bool				m_bInitialize;
};

}
#endif