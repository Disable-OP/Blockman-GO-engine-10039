
#ifndef __STATIC_MESH_L_OBJECT_H__
#define __STATIC_MESH_L_OBJECT_H__

#include "GameObject.h"
#include "Util/LightMapRasterizer.h"

namespace LORD
{
	class ModelEntity;
	class Material;

	class LORD_CORE_API StaticMeshObject_Normal: public GameObject
	{
		friend class NormalMeshSubset;
		friend class Zoom;
	public:
		StaticMeshObject_Normal();
		virtual ~StaticMeshObject_Normal();

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

		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);

		void frameMove(i32 deltaTime);
		void _render();	// 渲染
	
		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

		// 生成导航网格用
		Mesh*		GetMesh() { return m_pMesh; }
		virtual ui32 editor_getSubMeshNum() const;

		virtual void getVertexList(ui32 subId, vector<LightmapRasterizer::VertexInput>::type& lst);
		virtual void getIndexList(ui32 subId, vector<Word>::type& lst);

		virtual const Matrix4& getWorldMatrix() const;

		virtual void setLMTexture(ui32 subId, Texture* pTexture);
		virtual void setLightMapParam(ui32 subId, const Vector4& param, const Vector3& scale);

		virtual void unloadLMTexture();
		virtual void refleshLMTexture();

		virtual void setUVScale(float u, float v);

		virtual void setSelSubMesh(i32 subId);

		virtual void setLightMapSize(int subId, const Vector2& lmSize);
		virtual void getLightMapSize(int subId, Vector2& lmSize) const;
		
		virtual void changeDark(float radio);
		virtual void revertLight();

		void EnableGrayRender(bool gray);
		bool IsEnableGrayRender() const { return m_bGray; }
	protected:
		bool			m_bAlphaTest;
		bool			m_bGray;

		int				editor_selSubId;

		float			m_Extent;
		float			m_SquaredLength;

		ModelEntity*	m_modelEntity;
		Mesh*			m_pMesh;
		
		Vector2			m_UVScale;
		Matrix4 m_InversMat;
		vector<Vector2>::type       m_lightmapSize;
	};

	class NormalMeshSubset : public ObjectSubset
	{
	public:
		NormalMeshSubset(){ m_Type = GOT_SMESH_L; }
		virtual ~NormalMeshSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<StaticMeshObject_Normal*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((StaticMeshObject_Normal*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((StaticMeshObject_Normal*)object);
		}
		virtual void clearObjects()
		{
			set<StaticMeshObject_Normal*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear();
		}

		virtual GameObject* findObjectByNote(const String& note)
		{
			set<StaticMeshObject_Normal*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal* pObject = *it;
				if (pObject->m_Note == note)
				{
					return pObject;
				}
			}
			return NULL;
		}
	protected:
		set<StaticMeshObject_Normal*>::type mObjects;
	};
}

#endif

