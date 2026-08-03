#ifndef __LORD_TERRAINENTITY_H__
#define __LORD_TERRAINENTITY_H__

#include "Core.h"
#include "Render/Renderable.h"
#include "Math/Matrix4.h"
#include "Image/Color.h"

namespace LORD
{
	/**
    Renderalbe. 
	add this TerrainEntity to RenderQueue[xxxx_mesh_for_xxxx];
	**/
	class SubMesh;
	class LORD_CORE_API TerrainEntity: public Renderable
	{
	public:

		enum RenderType
		{
			RT_FOR_ACTOR,
			RT_FOR_SCENE_L,
			RT_FOR_SCENE,
		};

		TerrainEntity();
		virtual ~TerrainEntity();

	public:
		void			attachTo(SceneNode* pNode);
		SceneNode*		getSceneNode() const;

		void			setRenderType(RenderType type);
		RenderType		getRenderType() const;

		virtual void	importSubMesh(SubMesh* pSubMesh);
		virtual void	createRenderable();
		virtual void	update();

		Real			getViewDepth(Camera* pCamera) const;

		void			setVisible(const bool& visible);
		const bool&		getVisible() const;

		void			setTexture(uint index, Texture* pTex);

		inline const Box&		getLocalAABB() const
		{
			return m_LocalBox;
		}

		inline const Box&      getWorldAABB() const
		{
			return m_WorldBox;
		}

		inline void updateWorldAABB(const Matrix4& mat)
		{
			m_WorldBox = m_LocalBox.transform(mat);
		}
		
		inline void setLMParam( const Vector4& param, const Vector3& scale )
		{
			memcpy(m_LMParam, &param, sizeof(Vector4));
			memcpy(&m_LMParam[1], &scale, sizeof(Vector3));
		}

		void	setDarkRadio(float factor)
		{
			m_LMParam[1].w = factor;
		}

		void*			getVertices();
		void*			getIndices();
		ui16			getVertexCount() const;
		ui16			getIndexCount() const;
		ui16			getVertexStride() const;
	protected:
		bool		m_isVisible;
		ui16		m_VertexCount;
		ui16		m_IndexCount;
		ui16		m_VertexStride;
		SceneNode*  m_pSceneNode;
		Texture*	m_LMTexture;
		GPUBuffer*	m_GpuVertexBuffer;
		GPUBuffer*	m_GpuIndexBuffer;
		void*		m_VertexBuffer;
		void*		m_IndexBuffer;
		uint		m_TextureStage[5];
		Vector4		m_LMParam[2];
		Matrix4     m_matWVP;
		Box			m_LocalBox;
		Box			m_WorldBox;
		RenderInput::VertexElementList m_VertexElements; // ∂•µ„∏Ò Ω
	};
}

#endif
