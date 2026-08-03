
#ifndef __TERRAIN_BLOCK_H__
#define __TERRAIN_BLOCK_H__

#include "Core.h"
#include "Render/Renderable.h"
#include "Geom/Box.h"

namespace LORD
{
	class Scene;
	class RenderInput;
	class Texture;
	class Material;

	struct TerrainVertexType
	{
		Vector3			position;
		Dword           color;
		Vector2			texCoord;
	};

	struct TerrainSphereVertexType
	{
		Vector3			position;
		Vector2			texCoord;
	};

	class LORD_CORE_API TerrainBlock: public Renderable
	{
	public:
		struct TerrainBrushInfo
		{
			float radius;
			float strength;
			bool down;
			bool fill;
		};

		typedef vector<float>::type HeightArray;

		TerrainBlock(Scene* scene, ui16 xUnitOffset, ui16 zUnitOffset, bool bSphere);
		virtual ~TerrainBlock();

		void			exportData(DataStream* stream);
		void			importData(DataStream* stream);

		void			prepare();

		void			unprepare();

		void			updateBox();

		void			updateRenderQueue();

		Box&			getTerrainBlockAABB() { return m_box; }

		void			setTransparent(bool setting) { m_isTransparent = setting; }

		bool			isTransparent() { return m_isTransparent; }

		void			setNeedToBeDraw(bool setting) { m_isNeedToBeDraw = setting; }

		bool			isNeedToBeDraw() { return m_isNeedToBeDraw;}

		void			createRenderable();

		void			createSphereRenderble();

		void			reBuildGeometry();

		std::pair<bool, Real> editor_rayIntersect(const Ray& ray);

		void			editor_modifyTerrainHeight(const Vector3& pos, const TerrainBrushInfo& info, float escapeTime);
		
		const float*    editor_getVertexData() const { return (float*)m_Vertices; }
		const Word*		editor_getIndexData() const { return (Word*)m_Indices; }
		ui16			editor_getIndexNum() const { return m_IndexNum; }
		ui32			editor_getVertexStride() const { return m_bSphere ? sizeof(TerrainSphereVertexType) : sizeof(TerrainVertexType); };
		void			editor_createEditMesh();
		void			editor_refleshMesh();
		void			editor_setMeshVisible(bool bVisible);

		const String&	editor_getTextureName();
	protected:
		bool			m_isTransparent;
		bool			m_bPrepareed;
		bool			m_isNeedToBeDraw;
		bool			m_bSphere;
		ui16			m_IndexNum;
		ui16			m_xUnitOffset;
		ui16			m_zUnitOffset;
		ui32			m_VertexNum;

		Scene*			m_ownerScene;
		Texture*		m_terrainTexture;
		GPUBuffer*		m_vetexBuffer;
		GPUBuffer*		m_indexBuffer;
		TerrainSphereVertexType* m_Vertices;
		Word*			m_Indices;
		QueryObject*    m_MeshObject;
		RenderQueue*	m_RenderQueue;

		Box				m_box;
		HeightArray		m_HeightArray;
		RenderInput::VertexElementList m_VertexElement;
	};
}

#endif

