
#ifndef __SKELETON_MESH_OBJECT_H__
#define __SKELETON_MESH_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class SkinedEntity;
	class Skeleton;
	class SkinnedMesh;
	class Animation;
	class AnimBlender;
	class AnimState;
	class Material;

	class LORD_CORE_API SkeletonMeshObject: public GameObject
	{
	public:
		SkeletonMeshObject();
		virtual ~SkeletonMeshObject();

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

		void frameMove(i32 deltaTime);

		// 渲染
		void _render();

		virtual const Box& getWorldAABB() const;

		virtual void updateWorldAABB();

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);
	protected:
		void		updateSkeleton();
		void		updateAnimation();

		bool			m_isPlayAnim;
		float			m_InnerAnimationTime;
		SkinedEntity*	m_SkinEntity;
		Skeleton*		m_Skeleton;
		SkinnedMesh*	m_SkinMesh;
		Animation*		m_Animation;
		AnimState*		m_AnimationState;
		AnimBlender*    m_AnimationBlender;

		String			m_SkeletonName;
		String			m_SkinName;
		String			m_AnimationName;
	};

	class SkeletonMeshSubset : public ObjectSubset
	{
	public:
		SkeletonMeshSubset(){ m_Type = GOT_DMESH; }
		virtual ~SkeletonMeshSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<SkeletonMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SkeletonMeshObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SkeletonMeshObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SkeletonMeshObject*)object);
		}
		virtual void clearObjects()
		{
			set<SkeletonMeshObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SkeletonMeshObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SkeletonMeshObject*>::type mObjects;
	};
}

#endif

