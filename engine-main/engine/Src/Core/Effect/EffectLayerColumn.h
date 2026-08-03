/********************************************************************
filename: 	EffectColumn.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_COLUMN_H__
#define __LORD_EFFECT_LAYER_COLUMN_H__

#include "EffectLayer.h"

namespace LORD
{

	class LORD_CORE_API EffectLayerColumn : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList & list);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	public:
		EffectLayerColumn();
		virtual ~EffectLayerColumn();

		// 
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);


	protected:
		i32			mSegment;						
		float		mRadiusBottom;					
		float		mRadiusTop;						
		float		mHeight;					
		bool		mNoScaleRadius;					
		bool		mNoScaleHeight;				
		bool		mKeepCenter;				
		bool		mTextureClamp;				
		EffectVertexFmt2* mVertices;
	};
}

#endif