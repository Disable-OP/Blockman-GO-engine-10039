
#ifndef __SCENE_ACTOR_OBJECT_H__
#define __SCENE_ACTOR_OBJECT_H__

#include "Scene/GameObject.h"

namespace LORD
{
	class ActorObject;
	class LORD_CORE_API SceneActorObject: public GameObject
	{
	public:
		struct PropertyValue
		{
			LORD::String name;
			LORD::String valueType;
			LORD::String velue;

			PropertyValue()
			{
				name = "holle";
				valueType = "String";
				velue     = "world";
			}
		};

		typedef LORD::vector<PropertyValue>::type TypePropertyVec;
		typedef LORD::vector<LORD::ActorObject*>::type TypeChileActorVec;

	public:
		SceneActorObject();
		virtual ~SceneActorObject();

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
		void _render();// 渲染
		void frameMove(i32 deltaTime);

		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

		void setActorPropertys(TypePropertyVec* ActorProVec){m_pActorProVec = ActorProVec;};

		TypePropertyVec* getActorPropertys(){ return m_pActorProVec;}

		ActorObject* getActor(){return m_pActor;}

		virtual bool getPropertyValue(const String & name, String &value);

		virtual bool setPropertyValue(const String & name,const String & value);

		void creatChileActor(int num);

		void setLogicType(const String& type){m_Type = type;}

		String& getLogicType(){return m_Type;}

	protected:

		String m_Type;
		int m_CurentChileNum;
		ActorObject* m_pActor;
		TypePropertyVec* m_pActorProVec;
		TypeChileActorVec m_ChileActorVec;
	};

	class SceneActorSubset : public ObjectSubset
	{
	public:
		SceneActorSubset(){ m_Type = GOT_LOGIC; }
		virtual ~SceneActorSubset(){}
		
		virtual void frameMove(i32 deltaTime)
		{
			set<SceneActorObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneActorObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SceneActorObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SceneActorObject*)object);
		}
		virtual void clearObjects()
		{
			set<SceneActorObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneActorObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SceneActorObject*>::type mObjects;
	};
}

#endif

