#include "Core.h"
#include "EffectLayerSkin.h"
#include "EffectSystemManager.h"
#include "Model/MeshManager.h"
#include "Model/SkinnedMesh.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Anim/SkeletonManager.h"
#include "Anim/AnimManager.h"
#include "Anim/Animation.h"
#include "Anim/AnimState.h"
#include "Anim/AnimBlender.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"
#include "Resource/DataStream.h"
#include "Actor/ActorManager.h"

namespace LORD
{
	bool		EffectLayerSkin::s_GpuSkin = true;

	EffectLayerSkin::EffectLayerSkin()
		: mbIsTransparent(false)
		, mSkeltonName(StringUtil::BLANK)
		, mSkinName(StringUtil::BLANK)
		, mAnimationName(StringUtil::BLANK)
		, mModelName(StringUtil::BLANK)
		, mAttachBoneName(StringUtil::BLANK)
		, mSkinNode(NULL)
		, mSkinEntity(NULL)
		, mSkeleton(NULL)
		, mSkinMesh(NULL)
		, mAnimation(NULL)
		, mAnimationState(NULL)
		, mAnimationBlender(NULL)
		, mInnerAnimationTime(0)
		, mAttachBoneNode(NULL)
		, mAttachEntity(NULL)
		, mAttachMesh(NULL)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_Skin;
	}

	EffectLayerSkin::~EffectLayerSkin()
	{

	}

	void EffectLayerSkin::_notifyReStart()
	{
		EffectLayer::_notifyReStart();
		mInnerAnimationTime = 0;
	}

	void EffectLayerSkin::_notifyStart()
	{
		EffectLayer::_notifyStart();
		mInnerAnimationTime = 0;
	}

	void EffectLayerSkin::prepare()
	{
		EffectLayer::prepare();

		if((mSkinName!=StringUtil::BLANK) && (mSkeltonName!=StringUtil::BLANK) && (mAnimationName!=StringUtil::BLANK) && mParentSystem->getEffectSceneNode())
		{
			mSkeleton = SkeletonManager::Instance()->createSkeleton(mSkeltonName);
			mSkeleton->load();

			mSkinMesh = MeshManager::Instance()->createSkinnedMesh(mSkinName, false, true);
			mSkinMesh->load();

			mAnimation = AnimManager::Instance()->createAnim(mAnimationName);
			mAnimation->load();

			mAnimationState = LordNew AnimState(mAnimation);
			mAnimationState->setTimePosition(0);

			mAnimationBlender = LordNew AnimBlender(mSkeleton);
			mAnimationBlender->setBlendMode(AnimBlender::BM_REPLACE);
			mAnimationBlender->blend(mAnimationState, 0);

			mSkinNode = mParentSystem->getEffectSceneNode()->createChild();
			mSkinEntity = LordNew SkinedEntity;
			if(mbIsTransparent)
			{
				mSkinEntity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
				mSkinEntity->setMaterial(ActorManager::Instance()->GetSkinMaterial_transparent());
			}
			else
			{
				mSkinEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
				mSkinEntity->setMaterial(ActorManager::Instance()->GetSkinMaterial());
			}

			mSkinEntity->setSkeleton(mSkeleton);
			mSkinEntity->setMesh(mSkinMesh);
			mSkinEntity->setAnimBlender(mAnimationBlender);
			mSkinEntity->attachTo(mSkinNode);

			mSkinEntity->createRenderable();
			for (int i = 0; i < mSkinMesh->getSubMeshNum(); ++i)
			{
				String name = mSkinMesh->getTextureName(i, 0);
				Texture* pTex = TextureManager::Instance()->createTexture(name);
				if (pTex->load())
				{
					mSkinEntity->replaceTexture(pTex, i, 0);
				}
				else
				{
					LordLogError("EffectLayerSkin::prepare: Texture isn't exits %s.", name.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "EffectLayerSkin::prepare: Texture isn't exits %s.", name.c_str());
					MessageBox(0, buf, 0, 0);
#endif
				}
			}


			if((mAttachBoneName!=StringUtil::BLANK) && (mModelName!=StringUtil::BLANK))
			{
				mAttachMesh = MeshManager::Instance()->createMesh(mModelName, false, true);
				mAttachMesh->load();

				mAttachEntity = LordNew ModelEntity;
				mAttachEntity->setMesh(mAttachMesh);

				Bone* pBone = mSkeleton->getBone(mAttachBoneName);
				if (pBone->isDummy())
				{
					Bone* pParentBone = pBone->getParent();

					mAttachBoneNode = mSkinNode->createChild();//SceneManager::Instance()->getRootNode()->createChild();
					mAttachBoneNode->setLocalOrientation(pParentBone->getRotation());
					mAttachBoneNode->setLocalPosition(pParentBone->getPosition());
					SceneNode* node = mAttachBoneNode->createChild();
					node->setLocalOrientation(pBone->getRotation());
					node->setLocalPosition(pBone->getPosition());
					mAttachEntity->attachTo(node);
				}
				else
				{
					mAttachBoneNode = mSkinNode->createChild();//SceneManager::Instance()->getRootNode()->createChild();
					mAttachBoneNode->setLocalOrientation(pBone->getRotation());
					mAttachBoneNode->setLocalPosition(pBone->getPosition());
					mAttachEntity->attachTo(mAttachBoneNode);
				}
				if (mbIsTransparent)
				{
					mAttachEntity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
					mAttachEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial_transparent());
				}
				else
				{
					mAttachEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
					mAttachEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial());
				}
				mAttachEntity->createRenderable();
				for (int i = 0; i < mAttachMesh->getSubMeshNum(); ++i)
				{
					//SubMesh* pSubMesh = mAttachMesh->getSubMesh(i);
					String name = mAttachMesh->getTextureName(i, 0);
					Texture* pTex = TextureManager::Instance()->createTexture(name);
					if (pTex->load())
					{
						mAttachEntity->replaceTexture(pTex, i, 0);
					}
					else
					{
						LordLogError("EffectLayerSkin::prepare: Texture isn't exits %s.", name.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
						char buf[MAX_PATH];
						sprintf_s(buf, MAX_PATH, "EffectLayerSkin::prepare: Texture isn't exits %s.", name.c_str());
						MessageBox(0, buf, 0, 0);
#endif
					}
				}
			}
		}
	}

	void EffectLayerSkin::unprepare()
	{
		if(mAttachBoneNode)
		{
			mAttachBoneNode->destroyAllChildren();
			mSkinNode->destroyChild(mAttachBoneNode);
			mAttachBoneNode = NULL;
		}

		LordSafeDelete(mAttachEntity);

		if(mAttachMesh)
		{
			MeshManager::Instance()->releaseResource(mAttachMesh);
			mAttachMesh = NULL;
		}

		LordSafeDelete(mAnimationBlender);
		LordSafeDelete(mAnimationState);

		if(mAnimation)
		{
			AnimManager::Instance()->releaseResource(mAnimation);
			mAnimation = NULL;
		}

		LordSafeDelete(mSkinEntity);

		if(mSkinNode)
		{
			mParentSystem->getEffectSceneNode()->destroyChild(mSkinNode);
			mSkinNode = NULL;
		}

		if(mSkinMesh)
		{
			MeshManager::Instance()->releaseResource(mSkinMesh);
			mSkinMesh = NULL;
			mAnimationState = NULL;
		}

		if(mSkeleton)
		{
			SkeletonManager::Instance()->releaseResource(mSkeleton);
			mSkeleton = NULL;
		}
	}

	int EffectLayerSkin::_update(i32 time)
	{
		int ret = EffectLayer::_update(time);

		if(ret && mbAffected && mSkinNode)
		{
			mSkinNode->setLocalScaling(Vector3(scale, scale, scale));
			mSkinNode->setLocalPosition(position);
			mSkinNode->setLocalOrientation(oritation);
			mSkinNode->update();

			if(mAnimationBlender)
			{
				mInnerAnimationTime += time;
				mAnimationState->setTimePosition(mInnerAnimationTime);

				mAnimationBlender->frameMove(mInnerAnimationTime);

				if(mAttachEntity)
				{
					Bone* pbone = mSkeleton->getBone(mAttachBoneName);
					if(pbone->isDummy())
					{
						Bone* pParent = pbone->getParent();
						mAttachBoneNode->setLocalOrientation(mAnimationBlender->getAnimOrientation(pParent->getId()) * pParent->getRotation());
						mAttachBoneNode->setLocalPosition(mAnimationBlender->getAnimOrientation(pParent->getId()) * pParent->getPosition() + mAnimationBlender->getAnimPosition(pParent->getId()));
						
					}
					else
					{
						//matWorld = pbone->getMatrix() * mAnimationBlender->getAnimMatrix((Dword)pbone->getId()) * mSkinNode->getWorldMatrix();
						mAttachBoneNode->setLocalOrientation(mAnimationBlender->getAnimOrientation(pbone->getId()) * pbone->getRotation());
						mAttachBoneNode->setLocalPosition(mAnimationBlender->getAnimOrientation(pbone->getId()) * pbone->getPosition() + mAnimationBlender->getAnimPosition(pbone->getId()));
					}
					mAttachBoneNode->update(true);
				}
			}
		}
		return ret;
	}

	void EffectLayerSkin::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) )
			return;
		
		const Matrix4* viewproj = &mParentSystem->getCurrentCamera()->getViewProjMatrix();
		RenderQueue* queue = mParentSystem->getIsUIEffect() ? SceneManager::Instance()->getRenderQueue("SkinnedMeshSpecial") : SceneManager::Instance()->getRenderQueue("Scene");
		if(mSkinEntity && mSkinNode)
		{
			//mSkinEntity->setNaturalColor(color);
			mSkinEntity->update(viewproj);
			mSkinEntity->updateRenderQueue(queue);
		}

		if(mAttachEntity && mAttachBoneNode)
		{
			//mAttachEntity->setNaturalColor(color);
			mAttachEntity->update(viewproj);
			mAttachEntity->updateRenderQueue(queue);
		}
	}

	void EffectLayerSkin::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mbIsTransparent,sizeof(bool));
		ui32 strLength;
		pStream->read(&strLength, sizeof(strLength));
		char str[128];
		pStream->read(str, strLength);
		str[strLength] = 0;
		mSkinName = str;
		pStream->read(&strLength, sizeof(strLength));
		pStream->read(str, strLength);
		str[strLength] = 0;
		mSkeltonName = str;
		pStream->read(&strLength, sizeof(strLength));
		pStream->read(str, strLength);
		str[strLength] = 0;
		mAnimationName = str;
		pStream->read(&strLength, sizeof(strLength));
		pStream->read(str, strLength);
		str[strLength] = 0;
		mModelName = str;
		pStream->read(&strLength, sizeof(strLength));
		pStream->read(str, strLength);
		str[strLength] = 0;
		mAttachBoneName = str;
	}

	void EffectLayerSkin::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mbIsTransparent,sizeof(bool));
		ui32 strlength = (ui32)mSkinName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mSkinName.c_str(), strlength);
		strlength = (ui32)mSkeltonName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mSkeltonName.c_str(), strlength);
		strlength = (ui32)mAnimationName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mAnimationName.c_str(), strlength);
		strlength = (ui32)mModelName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mModelName.c_str(), strlength);
		strlength = (ui32)mAttachBoneName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mAttachBoneName.c_str(), strlength);
	}

	void EffectLayerSkin::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerSkin* skin = static_cast<EffectLayerSkin*>(particle);
		skin->mbIsTransparent = mbIsTransparent;
		skin->mSkinName = mSkinName;
		skin->mSkeltonName = mSkeltonName;
		skin->mAnimationName = mAnimationName;
		skin->mModelName = mModelName;
		skin->mAttachBoneName = mAttachBoneName;
	}

	void EffectLayerSkin::mergeAligenBox(Box& aabb)
	{
		if(mSkinNode && mSkinEntity)
		{
			mSkinNode->setLocalScaling(Vector3(scale, scale, scale));
			mSkinNode->setLocalPosition(position);
			mSkinNode->setLocalOrientation(oritation);
			mSkinNode->update();

			Box modelBox = mSkinMesh->getBox();
			Matrix4 worldTrans = mSkinNode->getWorldMatrix();
			modelBox.transform(worldTrans);
			aabb.unionBox(modelBox);
		}
	}

	void EffectLayerSkin::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_DMD_TRANSPARENT]);
		list.push_back(token[TOKEN_LY_DMD_SKELETON]);
		list.push_back(token[TOKEN_LY_DMD_SKIN]);
		list.push_back(token[TOKEN_LY_DMD_ANIMATION]);
		list.push_back(token[TOKEN_LY_DMD_ATTACHMODEL]);
		list.push_back(token[TOKEN_LY_DMD_ATTACHBONE]);
	}

	bool EffectLayerSkin::getPropertyType(const String& name, PropertyType& type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_DMD_TRANSPARENT])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if (name == token[TOKEN_LY_DMD_SKELETON])
		{
			type = IElement::PT_SKELETON_NAME_TYPE; return true;
		}
		else if(name == token[TOKEN_LY_DMD_SKIN])
		{
			type = IElement::PT_SKIN_NAME_TYPE; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ANIMATION])
		{
			type = IElement::PT_MODEL_ANIMATION_TYPE; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHMODEL])
		{
			type = IElement::PT_MODEL_NAME_TYPE; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHBONE])
		{
			type = IElement::PT_SKIN_BONE_TYPE; return true;
		}
		return false;
	}


	bool EffectLayerSkin::getPropertyValue(const String& name, String& value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_DMD_TRANSPARENT])
		{
			value = StringUtil::ToString(mbIsTransparent); return true;
		}
		else if (name == token[TOKEN_LY_DMD_SKELETON])
		{
			value = mSkeltonName; return true;
		}
		else if(name == token[TOKEN_LY_DMD_SKIN])
		{
			value = mSkinName; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ANIMATION])
		{
			value = mAnimationName; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHMODEL])
		{
			value = mModelName; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHBONE])
		{
			value = mAttachBoneName; return true;
		}
		return false;
	}

	bool EffectLayerSkin::setPropertyValue(const String& name,const String& value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_DMD_TRANSPARENT])
		{
			mbIsTransparent = StringUtil::ParseBool(value); return true;
		}
		else if (name == token[TOKEN_LY_DMD_SKELETON])
		{
			mSkeltonName = value; return true;
		}
		else if(name == token[TOKEN_LY_DMD_SKIN])
		{
			mSkinName = value; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ANIMATION])
		{
			mAnimationName = value; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHMODEL])
		{
			mModelName = value; return true;
		}
		else if(name == token[TOKEN_LY_DMD_ATTACHBONE])
		{
			mAttachBoneName = value; return true;
		}

		return false;
	}
}