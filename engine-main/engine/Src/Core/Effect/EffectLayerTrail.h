/********************************************************************
filename: 	EffectLayerParticlesCurve.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_TRAIL_H__
#define __LORD_EFFECT_LAYER_TRAIL_H__

#include "EffectLayer.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerTrail : public EffectLayer
	{
		struct TrailElement
		{
			TrailElement(const Vector3& pos1, const Vector3& pos2, i32 t)
				: position1(pos1)
				, position2(pos2)
				, mcurrentTime(t)
			{
			}

			TrailElement()
			{
			}

			Vector3 position1;
			Vector3 position2;
			i32  mcurrentTime;
		};

	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		EffectLayerTrail();
		virtual ~EffectLayerTrail();

		virtual void unprepare();

		virtual int _update(i32 time);
		virtual void _updateRenderQueue();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);


	protected:
		Vector3 mOriginalPosition1;					
		Vector3 mOriginalPosition2;					
		int	 mDurationTime;						
		bool mFallowParent;						

		static const float minDistance;         
		typedef list<TrailElement>::type TrailElementList;
		typedef list<TrailElement>::type::iterator TrailElementIter;

		TrailElementList mTrailElementList;			
		ui16 mElementNum;						
		Vector3   mLastPosition;													
	};
}

#endif