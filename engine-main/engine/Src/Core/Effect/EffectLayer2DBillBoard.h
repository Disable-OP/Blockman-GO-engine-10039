/********************************************************************
filename: 	EfefctLayerBillBoard.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_2DBILLBOARD_H__
#define __LORD_EFFECT_LAYER_2DBILLBOARD_H__

#include "EffectLayer.h"

namespace LORD
{
	class LORD_CORE_API EffectLayer2DBillBoard : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList & list);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	public:
		EffectLayer2DBillBoard();
		virtual ~EffectLayer2DBillBoard();

		virtual void _updateRenderQueue();

		virtual void _notifyParabolicParam(const Vector3& p1, const Vector3& p2, const Vector3& p3);

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);


	protected:
		float mWidth;
		float mHeight;
		float mCenterOffestW;
		float mCenterOffestH;
		float mOffsetViewport;
		bool  mbPerpendicular;
	};
}

#endif