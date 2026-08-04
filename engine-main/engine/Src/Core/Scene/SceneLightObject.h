
#ifndef __SCENE_LIGHT_OBJECT_H__
#define __SCENE_LIGHT_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class LORD_CORE_API SceneLightObject : public GameObject
	{
	public:
		SceneLightObject(void);
		virtual ~SceneLightObject(void);

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
		void _render();

		const Color& getColor() const;
		float getRadius() const;
		float getBrightness() const;
		float getFalloff() const;
		float getIndirectScale() const;
		float getIndirectSaturation() const;
		float getShadowExponent() const;

		void adjustSize();
	protected:
		Color	m_Color;			//光源颜色
		float	m_Radius;			//内径
		float	m_Brightness;		//亮度
		float	m_Falloff;			//衰减
		float   m_IndirectLightingScale;//间接光照强度
		float   m_IndirectLightingSaturation;//间接光照饱和度
		float   m_ShadowExponent;//阴影指数
		QueryObject*	m_pLightQueryObject;
		Box				m_LocalAABB;
	};

	class PointLightSubset : public ObjectSubset
	{
	public:
		PointLightSubset(){ m_Type = GOT_POINTLIGHT; }
		virtual ~PointLightSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<SceneLightObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SceneLightObject*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SceneLightObject*)object);
		}
		virtual void clearObjects()
		{
			set<SceneLightObject*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SceneLightObject*>::type mObjects;
	};
}

#endif

