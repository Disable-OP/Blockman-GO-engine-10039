
#ifndef __STATIC_MESH_L_NOFOG_OBJECT_H__
#define __STATIC_MESH_L_NOFOG_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class ModelEntity;
	class Material;

	class LORD_CORE_API StaticMeshObject_Normal_noFog: public GameObject
	{
		friend class NormalNoFogMeshSubset;
	public:
		StaticMeshObject_Normal_noFog();
		virtual ~StaticMeshObject_Normal_noFog();

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

		// 渲染
		void frameMove(i32 deltaTime);
		void _render();

		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

		// 生成导航网格用
		Mesh*		GetMesh() { return m_pMesh; }
		virtual ui32 editor_getSubMeshNum() const;
		
		virtual const Matrix4& getWorldMatrix() const;

		virtual void setUVScale(float u, float v);

		virtual void changeDark(float radio);
		virtual void revertLight();

		void EnableGrayRender(bool gray);
		bool IsEnableGrayRender() const { return m_bGray; }
	protected:
		bool			m_bAlphaTest;
		bool			m_bGray;
		ModelEntity*	m_modelEntity;
		Mesh*			m_pMesh;
		Vector2			m_UVScale;
		Matrix4 m_InversMat;
	};

	class NormalNoFogMeshSubset : public ObjectSubset
	{
	public:
		NormalNoFogMeshSubset(){ m_Type = GOT_SMESH_L_NOFOG; }
		virtual ~NormalNoFogMeshSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<StaticMeshObject_Normal_noFog*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal_noFog* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((StaticMeshObject_Normal_noFog*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((StaticMeshObject_Normal_noFog*)object);
		}
		virtual void clearObjects()
		{
			set<StaticMeshObject_Normal_noFog*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal_noFog* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear();
		}

		virtual GameObject* findObjectByNote(const String& note)
		{
			set<StaticMeshObject_Normal_noFog*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject_Normal_noFog* pObject = *it;
				if (pObject->m_Note == note)
				{
					return pObject;
				}
			}
			return NULL;
		}
	protected:
		set<StaticMeshObject_Normal_noFog*>::type mObjects;
	};
}

#endif

