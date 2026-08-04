#ifndef __GROUP_OBJECT_H__
#define __GROUP_OBJECT_H__

#include "Scene/Scene.h"
#include "GameObject.h"

namespace LORD
{
	class LORD_CORE_API GroupObject : public GameObject
	{
		friend class SceneManager;
	public:
		struct ObjectInfo
		{
			Vector3 dir;
			Vector3 scale;
			Quaternion q;
		};

		typedef map<GameObject*, ObjectInfo>::type GroupObjectMap;
	public:
		GroupObject(void);
		virtual ~GroupObject(void);
		virtual void _prepare_io(){}
		virtual void _prepare_obj(){}
		virtual void _prepare_editor(){}
		virtual void _unprepare();
		virtual void updateWorldAABB();
		virtual const Box& getWorldAABB() const{ return m_WorldAABB;}
		
		void setPosition(const Vector3& pos);
		void setOritation(const Quaternion& ori);
		void setScale(const Vector3& scale);

		void addGameObject(GameObject* pObj, const Vector3& direct);
		void UntieGameObject(GameObject* pObj);
		
		void getGameObjectlist(Scene::GameObjectList& list);
		GroupObjectMap getGroupObjectList() const;
		void editor_refleshInfo();

		virtual void setVisible(bool visible);
	protected:
		GroupObjectMap mGroupObjectMap;
		Box m_WorldAABB;
	};


	class GroupSubset : public ObjectSubset 
	{
	public:
		GroupSubset(){ m_Type = GOT_GROUP; }
		virtual ~GroupSubset(){}
		virtual void frameMove(i32 deltaTime){};
		virtual void addObject(GameObject* object){};
		virtual void deleteObject(GameObject* object){};
		virtual void clearObjects(){};
	};

}

#endif
