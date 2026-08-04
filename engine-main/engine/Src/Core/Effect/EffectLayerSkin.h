/********************************************************************
filename: 	EffectLayerSkin.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_SKIN_H__
#define __LORD_EFFECT_LAYER_SKIN_H__

#include "Core.h"
#include "EffectLayer.h"
#include "Scene/SkinedEntity.h"
#include "Scene/ModelEntity.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerSkin : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		EffectLayerSkin();
		virtual ~EffectLayerSkin();

		virtual void _notifyStart();
		virtual void _notifyReStart();
		// 
		virtual int _update(i32 time);
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);

		const String& getSkeletonName() const { return mSkeltonName; }
		const String& getAttachBoneName() const { return mAttachBoneName; }

	protected:
		static bool		s_GpuSkin;
		bool			mbIsTransparent;
		String			mSkeltonName;
		String			mSkinName;
		String			mAnimationName;
		String			mModelName;
		String			mAttachBoneName;

		SceneNode*		mSkinNode;
		SkinedEntity*	mSkinEntity;
		Skeleton*		mSkeleton;
		SkinnedMesh*	mSkinMesh;
		Animation*		mAnimation;
		AnimState*		mAnimationState;
		AnimBlender*	mAnimationBlender;

		i32				mInnerAnimationTime; 

		SceneNode*		mAttachBoneNode;
		ModelEntity*	mAttachEntity;
		Mesh*			mAttachMesh;
	};
}

#endif