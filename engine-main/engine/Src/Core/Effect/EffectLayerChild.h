/********************************************************************
filename: 	EffectLayerChild.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_CHILD_H__
#define __LORD_EFFECT_LAYER_CHILD_H__

#include "EffectLayer.h"

namespace LORD
{
	class EffectSystem;
	class SceneNode;
	class LORD_CORE_API EffectLayerChild : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList & list);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	public:
		EffectLayerChild();
		virtual ~EffectLayerChild();

		virtual int	_update(i32 time);
		virtual void _updateRenderQueue();

		virtual void prepare_res();
		virtual void prepare_io();
		virtual void prepare();
		virtual void unprepare();

		virtual void _notifyStart(void);
		virtual void _notifyReStart(void);
		virtual void _notifyStop();
		virtual void _notifyPause();
		virtual void _notifyResume();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);
		virtual void _notifyParabolicParam(const Vector3& p1, const Vector3& p2, const Vector3& p3);


	protected:
		String				mChildEffectName;	//
		bool				mChildEffectLoop;	//
		float				mChildEffectSpeed;  //
		EffectSystem*		mChildEffect;
		SceneNode*			mChildEffectNode;
	};
}

#endif