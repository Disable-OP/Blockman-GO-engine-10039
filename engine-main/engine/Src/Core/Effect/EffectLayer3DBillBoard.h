/********************************************************************
filename: 	EffectLayer3DBillBoard.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_3DBILLBOARD_H__
#define __LORD_EFFECT_LAYER_3DBILLBOARD_H__

#include "EffectLayer.h"

namespace LORD
{
	class LORD_CORE_API EffectLayer3DBillBoard : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList & list);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	public:
		EffectLayer3DBillBoard();
		virtual ~EffectLayer3DBillBoard();

		virtual void prepare();
		virtual void _updateRenderQueue();


		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);


	protected:
		float mWidth;
		float mHeight;
		float mCenterOffestW;
		float mCenterOffestH;
		Vector3  mPosCached[4];
	};
}

#endif