/********************************************************************
filename: 	Renderable.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERABLE_H__
#define __LORD_RENDERABLE_H__

#include "Core.h"
#include "Material.h"

namespace LORD
{
	class LORD_CORE_API Renderable: public ObjectAlloc
	{
		friend class RenderQueue;

	public:
		Renderable();
		virtual ~Renderable();

	public:
		void render(Renderer* pRenderer);

		inline RenderInput* getRenderInput() const { return m_RenderInput; }
		inline void setRenderInput(RenderInput* input) { m_RenderInput = input;	}
		inline void setMaterial(Material* material) { m_Material = material; }
		inline Material* getMaterial() { return m_Material; }
	protected:
		/// Material
		Material*	m_Material;

		RenderInput*	m_RenderInput;	/// VB, IB, etc.
	};
}

#endif
