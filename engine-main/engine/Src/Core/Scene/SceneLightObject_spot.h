
#ifndef __SCENE_SPOTLIGHT_OBJECT_H__
#define __SCENE_SPOTLIGHT_OBJECT_H__

#include "GameObject.h"

namespace LORD
{
	class LORD_CORE_API SceneLightObject_spot : public GameObject
	{
	public:
		SceneLightObject_spot(void);
		virtual ~SceneLightObject_spot(void);

		virtual void _prepare_io();
		virtual void _prepare_res();
		virtual void _prepare_obj();
		virtual void _prepare_editor();
		virtual void _unprepare();
		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);	// 渲染
		virtual const Box& getWorldAABB() const;
		virtual void updateWorldAABB();
		virtual void setResourceName(const LORD::String& resName);

		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

		void frameMove(i32 deltaTime);
		void _render();

		const Color& getColor() const;
		float getRadius() const;
		float getBrightness() const;
		float getFalloff() const;
		float getIndirectScale() const;
		float getIndirectSaturation() const;
		float getShadowExponent() const;
		float getInnerAngle() const;
		float getGradientAngle() const;
		ui32  getFlags() const;
		
		void createSpot();
		void adjustSize();
	protected:
		Color	m_Color;			//光源颜色
		float	m_Radius;			//半径
		float	m_Brightness;		//亮度
		float	m_Falloff;			//衰减
		float   m_IndirectLightingScale;//间接光照强度
		float   m_IndirectLightingSaturation;//间接光照饱和度
		float   m_ShadowExponent;//阴影指数
		float   m_InnerAngle;	//内径
		float   m_GradientAngle; //过渡半径
		ui32	m_flags;
		Box		m_LocalAABB;
	};

	class SpotLightSubset : public ObjectSubset
	{
	public:
		SpotLightSubset(){ m_Type = GOT_SPOTLIGHT; }
		virtual ~SpotLightSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			set<SceneLightObject_spot*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject_spot* pObject = *it;
				pObject->frameMove(deltaTime);
				pObject->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((SceneLightObject_spot*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((SceneLightObject_spot*)object);
		}
		virtual void clearObjects()
		{
			set<SceneLightObject_spot*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				SceneLightObject_spot* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<SceneLightObject_spot*>::type mObjects;
	};
}

#endif

