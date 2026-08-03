/********************************************************************
filename: 	EffectLayerParaboloidal.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_PARABOLOIDAL_H__
#define __LORD_EFFECT_LAYER_PARABOLOIDAL_H__

#include "EffectLayer.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerParaboloidal : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		EffectLayerParaboloidal();
		virtual ~EffectLayerParaboloidal();

		// 
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);

	protected:
		static const i32	sParaboloidalLatitude;
		static const i32    sParaboloidalLongitude;
		float				mParam;
		float				mHeight;
		EffectVertexFmt2*	mVertices;
	};
}

#endif