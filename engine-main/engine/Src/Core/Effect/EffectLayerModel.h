/********************************************************************
filename: 	EffectLayerModel.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_MODEL_H__
#define __LORD_EFFECT_LAYER_MODEL_H__

#include "EffectLayer.h"
#include "Scene/ModelEntity.h"

namespace LORD
{

	class LORD_CORE_API EffectLayerModel : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		EffectLayerModel();
		virtual ~EffectLayerModel();

		// 
		virtual int _update(i32 time);
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);


	protected:
		bool			mbIsTransparent;
		String			mModelName;
		SceneNode*		mModelNode;
		ModelEntity*	mModelEntity;
		Mesh*			mMesh;
	};
}

#endif