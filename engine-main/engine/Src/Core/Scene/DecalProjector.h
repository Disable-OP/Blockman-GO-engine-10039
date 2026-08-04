/*
**功能描述：贴花，投影纹理 渲染单元
**文件说明：
**作	者：韦建伟
*/
#ifndef _DECALPROJECTOR_H
#define _DECALPROJECTOR_H

#include "Render/Renderable.h"
#include "GameObject.h"

#define DEFAULT_DECAL_INTENSITY 0.125f	

namespace LORD
{	
	class DecalEntity;
	class TerrainMeshObject;
	class TerrainBlock;
	class OceanRender;
	class LORD_CORE_API DecalProjector : public ObjectAlloc
	{
		friend class DecalEntity;
	public:
		typedef list<DecalEntity*>::type ProjectorRenderableList;

	public:
		DecalProjector();
		~DecalProjector();

		void _unprepare(); 
		void setTerrainObject(TerrainMeshObject* pObj) { m_pTerrainObject = pObj; }
		void setOceanRender(OceanRender* pOcean) { m_pOceanRender = pOcean; }
		void setPosition(const Vector3& pos);
		void setTexture(const String& name);
		void setColor(const Color& color);
		bool UpdateDecalData();
		void setRadius(float radius);
	public:
		void _updateRenderQueue();

	protected:
		bool PopulateForMesh(void);
		void ClearRenderableList();

		void AddSMeshRenderable();

	protected:
		TerrainMeshObject* m_pTerrainObject;
		OceanRender* m_pOceanRender;
		Texture* m_pTexture;
		Color m_Color;
		float m_Radius;
		Box		m_worldAABB;
		Vector3	m_position;
		//ProjectorRenderableList mSMeshRenderableList;
		GPUBuffer* m_VertexGpuBuf;
		GPUBuffer* m_IndexGpuBuf;
		DecalEntity* m_pDecalEnt;
	};

	class DecalEntity : public Renderable
	{
	public:
		DecalEntity();
		virtual ~DecalEntity();
		void unprepare();
		void createRenderable(GPUBuffer* vertexBuf, ui32 vertexCount, GPUBuffer* indexBuf, ui32 indexCount);	
	public:
		Matrix4	m_WVPMat;
		RenderInput::VertexElementList m_VertexElement;
	};
}
#endif