#ifndef __TERRIANMESH_L_OBJECT_H__
#define __TERRIANMESH_L_OBJECT_H__

#include "GameObject.h"
#include "Util/LightMapRasterizer.h"

namespace LORD
{
	class TerrainEntity;
	class Material;

	class LORD_CORE_API TerrainMeshObject: public GameObject
	{
		friend class Scene;
		friend class DecalProjector;
	public:
		struct LayerBrushInfo
		{
			float radius;
			float strength;
			i8 layerIndex;
			bool eraser;
		};

		typedef vector<TerrainEntity*>::type TerrainEntityVector;
		typedef vector<Mesh*>::type MeshVector;
	public:
		TerrainMeshObject();
		virtual ~TerrainMeshObject();

		//virtual void addMesh(const String& meshName);
		//virtual void importMesh(const String& meshName);
		/// 多线程加载 模型，贴图资源等
		virtual void _prepare_io();
		/// 发起io请求之前，创建资源空对象
		virtual void _prepare_res();
		/// io线程完成后的工作,创建d3d对象,挂接到场景节点的过程
		virtual void _prepare_obj();
		/// 编辑器需要创建额外的资源 
		virtual void _prepare_editor();
		/// 从场景节点中卸载并且销毁模型，贴图资源
		virtual void _unprepare(); 

		///// 导出数据 用于游戏
		virtual bool exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc);
		///// 导入数据 用于游戏
		virtual bool importData(rapidxml::xml_node<>* parentNode);

		virtual void _importData(DataStream& datastream);
		virtual void _exportData(DataStream& datastream);
		
		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

		virtual Vector3 rayTerrain(const Ray& rayconst, const LayerBrushInfo& radius, bool bBrush );

		virtual Vector3 editor_modifyTerrainLayer(const Vector2& pos, const LayerBrushInfo& brushInfo, bool bBrush);
		
		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);

		void _render();		// 渲染

		virtual const Box& getWorldAABB() const;
		virtual const Matrix4& getWorldMatrix() const;

		virtual void updateWorldAABB();

		virtual void ChangeBaseTexture(const LORD::String& name);
		virtual void setLayerTexture(uint id, const String& name);
		virtual Texture* getLayerTexture(uint id) const;

		virtual void deleteLayer(uint id);

		virtual void setLMTexture(uint subid, Texture* pTex);

		virtual ui32 getEntityNum() const;

		virtual void getVertexList(size_t subId, vector<LightmapRasterizer::VertexInput>::type& lst);
		virtual void getIndexList(size_t subId, vector<Word>::type& lst);
		virtual void setLightMapParam(size_t subId, const Vector4& param, const Vector3& scale);
		virtual void unloadLMTexture();
		virtual void refleshLMTexture();

		virtual TerrainEntity* getEntity(ui32 entid) const;

		inline void getLightMapSize(Vector2& lmsize);

		virtual void changeDark(float radio);
		virtual void revertLight();
	protected:
		Mesh* m_pMesh;
		Texture** m_pLayerTexture;
		Vector2 m_lightmapSize;
		Matrix4 m_InversMat;
		Box m_LocalBox;
		TerrainEntityVector mEntityArray;
	};

	class TerrainMeshSubset : public ObjectSubset
	{
	public:
		TerrainMeshSubset(){ m_Type = GOT_TERRAIN; }
		virtual ~TerrainMeshSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<TerrainMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				TerrainMeshObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((TerrainMeshObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((TerrainMeshObject*)object);
		}
		virtual void clearObjects()
		{
			set<TerrainMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				TerrainMeshObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<TerrainMeshObject*>::type mObjects;
	};
}

#endif

