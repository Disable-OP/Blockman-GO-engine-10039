#include "Core.h"
#include "RenderQueue.h"
#include "Renderer.h"
#include "Renderable.h"
#include "ShaderProgram.h"
#include "Material.h"

namespace LORD
{
	inline int compare(const void* p1, const void* p2)
	{
		ui32 t1 = ((Renderable*)p1)->getMaterial()->getMaterialKey();
		ui32 t2 = ((Renderable*)p2)->getMaterial()->getMaterialKey();
		return t1 < t2;
	}


	RenderQueue::RenderQueue(const String& strName)
		: m_bEnableRender(true)
		, m_strName(strName)
		, m_ManualBegin(NULL)
		, m_ManualEnd(NULL)
	{
		m_Renderables.reserve(100);
	}

	RenderQueue::~RenderQueue()
	{
	}

	void RenderQueue::beginRender()
	{
		m_Renderables.clear();
	}

	void RenderQueue::addRenderable(Renderable* pRenderable)
	{
		m_Renderables.push_back(pRenderable);
	}
	
	void RenderQueue::renderQueue(bool uiRender)
	{
		if( !m_bEnableRender ) return ;

		if(m_ManualBegin)
			m_ManualBegin->render(uiRender);

		if(!m_Renderables.empty())
		{
			// sort the render queue by the texture.
			std::sort(m_Renderables.begin(), m_Renderables.end(), compare);

			// then set the shader,render states.
			Renderer* pRenderer = Renderer::Instance();

			ui32 render_size = (ui32)m_Renderables.size();
			for (ui32 i = 0; i < render_size; ++i)
			{
				Renderable* pRenderable = m_Renderables[i];
				pRenderable->getMaterial()->active();
				pRenderable->render(pRenderer);
			}
		}

		if(m_ManualEnd)
			m_ManualEnd->render(uiRender);
	}
}
