
#ifndef __SCENE_EFFECT_OBJECT_H__
#define __SCENE_EFFECT_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class EffectSystem;
	class LORD_CORE_API SceneEffectObject: public GameObject
	{
	public:
		SceneEffectObject();
		virtual ~SceneEffectObject();

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
		void _render();// 渲染

		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

	protected:

		EffectSystem*		m_pEffectSystem;
	};

	class SceneEffectSubset : public ObjectSubset
	{
	public:
		SceneEffectSubset(){ m_Type = GOT_EFFECT; }
		virtual ~SceneEffectSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<SceneEffectObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneEffectObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SceneEffectObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SceneEffectObject*)object);
		}
		virtual void clearObjects()
		{
			set<SceneEffectObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneEffectObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SceneEffectObject*>::type mObjects;
	};
}

#endif

