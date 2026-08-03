/********************************************************************
filename: 	LineRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-04-06
*********************************************************************/

#ifndef __LORD_LINE_RENDERQUEUE_H__
#define __LORD_LINE_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{
class LineRenderable;
typedef list<LineRenderable*>::type LineRenderList;

class LineRenderQueue : public ObjectAlloc
{
public:
	LineRenderQueue();
	~LineRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);

	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	void endRender(const Matrix4* viewproj = NULL);
	
	void addRenderable(LineRenderable* renderable);

protected:
	void _render(Renderer* pRender, LineRenderable* pRenderable);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;
	int					m_sPMatMVPIndex;
	
	LineRenderList		m_renderables;
};

}
#endif