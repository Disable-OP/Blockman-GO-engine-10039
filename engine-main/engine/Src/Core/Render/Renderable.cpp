#include "Core.h"
#include "Renderable.h"
#include "ShaderProgram.h"
#include "Renderer.h"

namespace LORD
{
	Renderable::Renderable()
		: m_Material(NULL)
		, m_RenderInput(NULL)
	{
	}

	Renderable::~Renderable()
	{
		m_Material = NULL;
		m_RenderInput = NULL;
	}
	
	void Renderable::render(Renderer* pRenderer)
	{
		m_Material->active();
		pRenderer->render(m_RenderInput);
	}
}