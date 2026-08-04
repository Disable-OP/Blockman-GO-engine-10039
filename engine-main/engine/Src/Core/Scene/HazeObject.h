
#ifndef __HAZE_OBJECT_H__
#define __HAZE_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class ModelEntity;
	class Material;

	class LORD_CORE_API HazeObejct: public GameObject
	{
		friend class HazeSubset;
	public:
		HazeObejct();
		virtual ~HazeObejct();

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
		void frameMove(i32 deltaTime){}
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
		void setAlpha(float a);
	protected:
		ModelEntity*	m_modelEntity;
		Mesh*			m_pMesh;
		float			m_Alpha;
		Matrix4 m_InversMat;
	};

	class HazeSubset : public ObjectSubset
	{
	public:
		HazeSubset(){ m_Type = GOT_HAZE; }
		virtual ~HazeSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<HazeObejct*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				HazeObejct* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((HazeObejct*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((HazeObejct*)object);
		}
		virtual void clearObjects()
		{
			set<HazeObejct*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				HazeObejct* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear();
		}

		virtual GameObject* findObjectByNote(const String& note)
		{
			set<HazeObejct*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				HazeObejct* pObject = *it;
				if (pObject->m_Note == note)
				{
					return pObject;
				}
			}
			return NULL;
		}
	protected:
		set<HazeObejct*>::type mObjects;
	};
}

#endif

