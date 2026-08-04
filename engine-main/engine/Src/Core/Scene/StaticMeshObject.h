
#ifndef __STATIC_MESH_OBJECT_H__
#define __STATIC_MESH_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class ModelEntity;
	class Material;

	class LORD_CORE_API StaticMeshObject: public GameObject
	{
	public:
		StaticMeshObject();
		virtual ~StaticMeshObject();

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

		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);

		void frameMove(i32 deltaTime);
		// 渲染
		void _render();

		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

		virtual void setUVScale(float u, float v);

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	protected:

		ModelEntity*	m_modelEntity;
		Mesh*			m_pMesh;
		Vector2			m_UVScale;
	};

	class StaticMeshSubset : public ObjectSubset
	{
	public:
		StaticMeshSubset(){ m_Type = GOT_SMESH; }
		virtual ~StaticMeshSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<StaticMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((StaticMeshObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((StaticMeshObject*)object);
		}
		virtual void clearObjects()
		{
			set<StaticMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				StaticMeshObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<StaticMeshObject*>::type mObjects;
	};
}

#endif

