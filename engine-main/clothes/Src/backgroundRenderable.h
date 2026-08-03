
#ifndef __background_Renderable_H__
#define __background_Renderable_H__

#include "Core.h"
#include "Render/Renderable.h"
#include "Math/Vector3.h"
#include "Geom/Box.h"

namespace LORD
{
	class RenderInput;
	class Texture;
	class Material;

	struct BackGroundVertexType
	{
		Vector3			position;
		Dword           color;
		Vector2			texCoord;
	};
	
	class BackGroundRenderable: public Renderable
	{
	public:
		BackGroundRenderable(const String& texName);
		virtual ~BackGroundRenderable();

		void changeTexture(const String& texName);

		void update();
		void render();

	public:
		String			m_defaultBG = "";

	protected:
		RenderQueue*	m_pRenderQueue;
		GPUBuffer*		m_vetexBuffer;
		GPUBuffer*		m_indexBuffer;
		Texture*		m_pTexture;
		Matrix4			m_matWVP;
		RenderInput::VertexElementList m_VertexElement;
	};
}

#endif

