
#ifndef __SCENE_DIRLIGHT_OBJECT_H__
#define __SCENE_DIRLIGHT_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class LORD_CORE_API SceneLightObject_dir : public GameObject
	{
	public:
		SceneLightObject_dir(void);
		virtual ~SceneLightObject_dir(void);

		virtual void _prepare_io();
		virtual void _prepare_res();
		virtual void _prepare_obj();
		virtual void _prepare_editor();
		virtual void _unprepare();
		virtual const Box& getWorldAABB() const;
		virtual void updateWorldAABB();
		virtual void setResourceName(const LORD::String& resName);

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);

		void frameMove(i32 deltaTime);
		void _render();	// 渲染

		const Color& getColor() const;
		float getBrightness() const;
		float getIndirectScale() const;
		float getIndirectSaturation() const;
		float getShadowExponent() const;
		ui32  getFlags() const;
		
		void createDirLine();
		void adjustSize();
	protected:
		Color	m_Color;			//光源颜色
		float	m_Brightness;		//亮度
		float   m_IndirectLightingScale;//间接光照强度
		float   m_IndirectLightingSaturation;//间接光照饱和度
		float   m_ShadowExponent;//阴影指数
		ui32	m_flags;
		Box		m_LocalAABB;
	};

	class DirLightSubset : public ObjectSubset
	{
	public:
		DirLightSubset(){ m_Type = GOT_DIRLIGHT; }
		virtual ~DirLightSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<SceneLightObject_dir*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject_dir* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SceneLightObject_dir*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SceneLightObject_dir*)object);
		}
		virtual void clearObjects()
		{
			set<SceneLightObject_dir*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject_dir* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SceneLightObject_dir*>::type mObjects;
	};
}

#endif

