/********************************************************************
filename: 	RasterizerHelpRneder.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-04-06
*********************************************************************/

#ifndef __LORD_RASTERIZER_HELP_RENDER_H__
#define __LORD_RASTERIZER_HELP_RENDER_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API RasterizerRenderable : public ObjectAlloc
{
public:
	RasterizerRenderable();
	~RasterizerRenderable();

	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	float* getVertexMenory() { return m_VertexSysMemory; }

	inline bool getLockPtr(ui16 quadNum, LineVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (LineVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(LineVertexFmt) / sizeof(float));
			m_quadNum += quadNum;
		}
		else
		{
			return false;
		}

		return true;
	}
	
	void endPolygon();
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }

protected:
	ui16			m_quadPoolSize;
	ui16			m_quadNum;

	float*			m_VertexSysMemory;
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;
};

typedef list<RasterizerRenderable*>::type RasterizerRenderList;

class RasterizerRenderQueue : public ObjectAlloc
{
public:
	RasterizerRenderQueue();
	~RasterizerRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);

	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	void endRender(bool isLine, const Matrix4* viewproj = NULL);

	void addRenderable(RasterizerRenderable* renderable);

protected:
	void _render(Renderer* pRender, RasterizerRenderable* pRenderable, bool isLine);

protected:
	RenderInput *		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;
	int					m_sPMatMVPIndex;

	RasterizerRenderList m_renderables;
};

}
#endif