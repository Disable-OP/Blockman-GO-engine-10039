#include "ActorObject.h"
#include "SkillAttack.h"
#include "SkillSystem.h"
#include "ActorManager.h"
#include "ActorMeshTexModify.h"
#include "ActorTemplateData.h"
#include "Effect/EffectSystem.h"
#include "Effect/EffectSystemManager.h"
#include "Object/Exception.h"
#include "Object/Root.h"
#include "Anim/SkeletonManager.h"
#include "Anim/AnimManager.h"
#include "Anim/Animation.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"
#include "Anim/AnimBlender.h"
#include "Model/MeshManager.h"
#include "Model/SkinnedMesh.h"
#include "Scene/CameraMain.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Audio/SoundSystem.h"
#include "Scene/SkinedEntity.h"
#include "UI/GUISystem.h"
#include "Render/Material.h"
#include "Util/QueryObject.h"
#include "MergeMeshManager.h"
#include "Scene/TextureSet.h"

#pragma warning(disable:4355)

namespace LORD
{
	//-----------------------------------------------------------------------
	const float ACTOR_FADEOUT_TIME = 1800;

	const Vector3 UI_ACTOR_POSITION = Vector3(0.0f, 10000.0f, 0.0f);

	inline Quaternion CaculateOritation(float yaw, float pitch, float roll)
	{
		float fCosHRoll = Math::Cos(roll * Math::DEG2RAD * 0.5f);
		float fSinHRoll = Math::Sin(roll * Math::DEG2RAD * 0.5f);
		float fCosHPitch = Math::Cos(pitch * Math::DEG2RAD * 0.5f);
		float fSinHPitch = Math::Sin(pitch * Math::DEG2RAD * 0.5f);
		float fCosHYaw = Math::Cos(yaw * Math::DEG2RAD * 0.5f);
		float fSinHYaw = Math::Sin(yaw * Math::DEG2RAD * 0.5f);

		return Quaternion(fCosHRoll*fCosHPitch*fCosHYaw + fSinHRoll*fSinHPitch*fSinHYaw,
			fCosHRoll*fSinHPitch*fCosHYaw + fSinHRoll*fCosHPitch*fSinHYaw,
			fCosHRoll*fCosHPitch*fSinHYaw - fSinHRoll*fSinHPitch*fCosHYaw,
			fSinHRoll*fCosHPitch*fCosHYaw - fCosHRoll*fSinHPitch*fSinHYaw);
	}
	//---------------------------------------------------------------------
	String ActorObject::GetNextActorName()
	{
		static int actorCount = 0;

		return "Actor" + StringUtil::ToString(actorCount++);
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetNextMeshName(void)
	{
		static int meshCount = 0;

		return "ActorMesh" + StringUtil::Format("%d", meshCount++);
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetNextSocketName(void)
	{
		static int socketCount = 0;

		return "ActorSocket" + StringUtil::Format("%d", socketCount++);
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetNextChildActorAlias(void)
	{
		static int AliasCount = 0;

		return "ActorChild" + StringUtil::Format("%d", AliasCount++);
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetNextEffectName(void)
	{
		static int EffectCount = 0;

		return "ActorEffect" + StringUtil::Format("%d", EffectCount++);
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetNextEntityName(void)
	{
		static int EntityCount = 0;

		return "ActorEntity" + StringUtil::Format("%d", EntityCount++);
	}
	//-----------------------------------------------------------------------
	LORD::String ActorObject::GetNextSkillAttackName(void)
	{
		static int SkillAttackCount = 0;

		return "SkillAttack" + StringUtil::Format("%d", SkillAttackCount++);
	}
	//-----------------------------------------------------------------------
	static SceneNode* GetActorRootSceneNode(void)
	{
		return ActorManager::Instance()->GetActorRootSceneNode();
	}
	//-----------------------------------------------------------------------
	ActorObject::ActorObject(bool isUIActor, bool bSingleThread, int AnUserData, bool isHide) :
		m_pTemplateData(NULL),
		mSkillSystem(),
		mBaseScale(1.0f),
		mTimeScale(1.0f),
		mScale(1.0f, 1.0f, 1.0f),
		mPosition(UI_ACTOR_POSITION),
		mOrientation(1.0f, 0.0f, 0.0f, 0.0f),
		mParentActor(NULL),
		mSceneNodeMaster(NULL),
		mSkeletonMaster(NULL),
		mBuffEffectSceneNode(NULL),
		mAOEEffectSceneNode(NULL),
		mTopBoxSceneNode(NULL),
		mCenterBoxSceneNode(NULL),
		mIsActorSizeValid(false),
		mHasInited(false),
		mbSingleThread(bSingleThread),
		mbUpdateManually(false),
		mPositionDirty(false),
		mOrientationDirty(false),
		mScaleDirty(false),
		mNaturalColor(Color::WHITE),
		//mCustomColor(Color::WHITE),
		//mCustomColor(Color(255.f / 255.f, 222.f / 255.f, 183.f / 255.f, 0.28f)),//白种人
		//mCustomColor(Color(226.f / 255.f, 135.f / 255.f, 52.f / 255.f, 0.28f)),//黄种人
		//mCustomColor(Color(52.f / 255.f, 20.f / 255.f, 3.f / 255.f, 0.45f)),//黑人
		//mCustomColor(Color(0.f / 255.f, 163.f / 255.f, 232.f / 255.f, 0.55f)),//蓝
		//mCustomColor(Color(10.f / 255.f, 184.f / 255.f, 70.f / 255.f, 0.55f)),//绿
		//mCustomColor(Color(193.f / 255.f, 0.f / 255.f, 0.f / 255.f, 0.55f)),//红
		//mCustomColor(Color(156.f / 255.f, 169.f / 255.f, 1.f / 255.f, 0.55f)),//黄绿
		//mCustomColor(Color(123.f / 255.f, 0.f / 255.f, 170.f / 255.f, 0.55f)),//紫色
		//mCustomColor(Color(4.f / 255.f, 0.f / 255.f, 170.f / 255.f, 0.55f)),//蓝紫
		//mCustomColor(Color(189.f / 255.f, 156.f / 255.f, 0.f / 255.f, 0.35f)),//深黄
		//mCustomColor(Color(30.f / 255.f, 12.f / 255.f, 0.f / 255.f, 0.55f)),//焦黑
		//mCustomColor(Color(144.f / 255.f, 69.f / 255.f, 3.f / 255.f, 0.65f)),//黑黄
		mCustomColor(Color(0.f, 0.f, 0.f, 0.f)),
		mAlphaColor(Color::WHITE),
		mCurentRimColor(Color::WHITE),
		mCurentAmbientColor(Color::WHITE),
		mCurentColorParams(Vector4::ONE),
		m_pShadowEffect(NULL),
		m_pShadowSceneNode(NULL),
		mHorse(NULL),
		mBoneNodeVector(NULL),
		isCameraShakeEnd(true),
		mCameraShakeDelta(0.f),
		mCameraPushDelta(0.f),
		m_isUIActor(isUIActor),
		m_nUserData(AnUserData),
		mActorNameID(""),
		m_pAnimBlender(NULL),
		mpSelectEffect(NULL),
		m_isPause(false),
		m_isFrozen(false),
		m_isHide(isHide),
		m_isBackup(false),
		m_isSkillHideActor(false),
		m_curDelta(0),
		m_isMainActor(false),
		m_isActorFadeOut(false),
		m_FadeOutScale(1.f),
		m_isMirror(false),
		m_bTranslate(false),
		m_pDefaultTexture(NULL),
		m_fAlpha(1.0f),
		m_DarkRadio(1.0f),
		m_CameraAnimHide(false),
		m_MarkDestroy(false),
		m_isFrustumVisible(false),
		m_UpperRootBoneName(StringUtil::BLANK),
		mUnloadAllBodyParts(false),
		mMergeRenderable(true)
#ifdef RENDERACTORAABB
		, m_BoxObject(NULL)
#endif

	{
		mSceneNodeMaster = GetActorRootSceneNode()->createChild();
	}
	//-----------------------------------------------------------------------
	ActorObject::~ActorObject(void)
	{
		Free();
		if (mSceneNodeMaster)
		{
			SceneNode* parentNode = mSceneNodeMaster->getParent();
			if (parentNode)
				parentNode->destroyChild(mSceneNodeMaster);

			mSceneNodeMaster = NULL;
		}
		mpSelectEffect = NULL;
#ifdef RENDERACTORAABB
		if (m_BoxObject)
		{
			SceneNode* pNode = m_BoxObject->getParent();
			pNode->getParent()->destroyChild(pNode);
			QueryObjectManager::Instance()->destroyQueryObject(m_BoxObject);
		}
#endif // RENDERACTORAABB
	}
	//------------------------------------------------------------------------
	void ActorObject::Init(ActorTemplateData* pTemplateData, bool isActorMaster)
	{
		LordAssert(pTemplateData && mSkeletonMaster == NULL);
		m_pTemplateData = pTemplateData;

		if (m_pTemplateData->SkeletonName.empty())
		{
			return;
		}

		m_isMainActor = isActorMaster;
		mSkillSystem.SetCameraShakeEnable(isActorMaster);
		mActorNameID = GetNextActorName();

		mSkeletonMaster = SkeletonManager::Instance()->createSkeleton(pTemplateData->SkeletonName);

		vector<LORD::String>::type::iterator it = pTemplateData->AnimsList.begin();
		for (; it != pTemplateData->AnimsList.end(); ++it)
		{
			mAnimtions.insert(AnimManager::Instance()->createAnim(*it));
		}

		if (m_pTemplateData->mbShowShadow && !m_isUIActor)
		{
			m_pShadowEffect = EffectSystemManager::Instance()->createEffectSystem(GetNextEffectName(), "shadow.effect");
			if (m_pShadowEffect)
			{
				m_pShadowSceneNode = GetActorRootSceneNode()->createChild();
				m_pShadowSceneNode->setWorldPosition(mSceneNodeMaster->getWorldPosition());
				m_pShadowSceneNode->update();
				m_pShadowEffect->setParentSceneNode(m_pShadowSceneNode);
				m_pShadowEffect->setSpecialRender(true);
			}
		}

		//保证角色的缩放先更新一次
		mScaleDirty = true;

		bool bEditMode = Root::Instance()->isEditorMode();
		if (mbSingleThread || bEditMode)
		{
			int curstep = 0;
			prepare_io();
			InitAfterIOprepared(curstep);
			map<String, ActorTemplateData::BodyPart*>::type::const_iterator itBodyPart;
			for (itBodyPart = m_pTemplateData->BodyPartsMap.begin(); itBodyPart != m_pTemplateData->BodyPartsMap.end(); ++itBodyPart)
			{
				if (true == itBodyPart->second->bDefaultUse)
				{
					Body_Part_Info* pBodyPart = LORD_NEW_T(Body_Part_Info);
					pBodyPart->pTemplateData = itBodyPart->second;
					LoadBodyPart(pBodyPart);
					mBodyParts.insert(std::pair<String, Body_Part_Info*>(itBodyPart->second->MasterName, pBodyPart));
				}
			}
			refleshActorAlpha();
			mHasInited = true;
		}
		else
		{
			StreamThread* pThread = StreamThread::Instance();
			if (pThread && pThread->IsRunning())
			{
				ActorPrepareEvent* pEvent = LordNew ActorPrepareEvent(this);
				pThread->AddRequest(pEvent);
			}

			map<String, ActorTemplateData::BodyPart*>::type::const_iterator itBodyPart;
			for (itBodyPart = m_pTemplateData->BodyPartsMap.begin(); itBodyPart != m_pTemplateData->BodyPartsMap.end(); ++itBodyPart)
			{
				if (true == itBodyPart->second->bDefaultUse)
				{
					SlaveAttachProcess* pSlaveProcess = LordNew SlaveAttachProcess(itBodyPart->second->MasterName, itBodyPart->second->SlaveName);
					mDelayProcessVector.push_back(pSlaveProcess);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::Free(void)
	{
		LordSafeDelete(m_meshTexModify);

		vector<ActorDelayedProcess*>::type::iterator it = mDelayProcessVector.begin();
		for (; it != mDelayProcessVector.end(); ++it)
		{
			LordDelete(*it);
		}
		mDelayProcessVector.clear();

		mSkillSystem.Free();

		if (mDummyTexture)
			TextureManager::Instance()->releaseResource(mDummyTexture);

		EnableAllBodyParts(false);

		map<String, Child_Mesh*>::type::iterator itChild = mChildMeshs.begin();
		for (; itChild != mChildMeshs.end(); ++itChild)
		{
			MeshManager::Instance()->releaseResource(itChild->second->pMesh);
			LordDelete itChild->second->pModelEntity;
			itChild->second->pModelEntity = NULL;
			itChild->second->pParentNode->destroyChild(itChild->second->pModelNode);
			delete itChild->second;
			itChild->second = NULL;
		}
		mChildMeshs.clear();

		map<String, Actor_Effect>::type::iterator itEffect;
		for (itEffect = mPluginEffects.begin(); itEffect != mPluginEffects.end(); ++itEffect)
		{
			EffectSystemManager::Instance()->destroyEffectSystem(itEffect->second.pEffect);
			itEffect->second.pEffect = NULL;
			itEffect->second.pParentNode->destroyChild(itEffect->second.pEffectNode);
		}
		mPluginEffects.clear();

		if (mParentActor)
		{
			mParentActor->DetachChildActor(this);
		}

		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end();)
		{
			DetachChildActor(*itActor);
			itActor = mChildActors.begin();
		}

		list<SkillAttack*>::type::iterator itSkillAttack;
		for (itSkillAttack = mSkillAttackList.begin(); itSkillAttack != mSkillAttackList.end(); ++itSkillAttack)
		{
			SkillAttackManager::Instance()->destroyInstance(*itSkillAttack);
		}
		mSkillAttackList.clear();

		if (mBuffEffectSceneNode)
		{
			mSceneNodeMaster->destroyChild(mBuffEffectSceneNode);
			mBuffEffectSceneNode = NULL;
		}

		if (mAOEEffectSceneNode)
		{
			ActorManager::Instance()->GetActorRootSceneNode()->destroyChild(mAOEEffectSceneNode);
			mAOEEffectSceneNode = NULL;
		}

		if (mTopBoxSceneNode)
		{
			mSceneNodeMaster->destroyChild(mTopBoxSceneNode);
			mTopBoxSceneNode = NULL;
		}

		if (mCenterBoxSceneNode)
		{
			mSceneNodeMaster->destroyChild(mCenterBoxSceneNode);
			mCenterBoxSceneNode = NULL;
		}

		if (m_pShadowEffect)
		{
			EffectSystemManager::Instance()->destroyEffectSystem(m_pShadowEffect);
			GetActorRootSceneNode()->destroyChild(m_pShadowSceneNode);
			m_pShadowSceneNode = NULL;
			m_pShadowEffect = NULL;
		}

		/*if(mEntityMaster)
		{
		LordDelete mEntityMaster;
		mEntityMaster = NULL;
		}*/
		for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
		{
			LordDelete(*it);
		}
#ifdef RENDERACTORAABB
		if (m_BoxObject)
		{
			SceneNode* pNode = m_BoxObject->getParent();
			pNode->getParent()->destroyChild(pNode);
			QueryObjectManager::Instance()->destroyQueryObject(m_BoxObject);
			m_BoxObject = NULL;
		}
#endif
#ifdef RENDERDUMMYNODE
		QueryObjectManager* pQueryMgr = QueryObjectManager::Instance();
		for (size_t i = 0; i < m_DummyObjects.size(); ++i)
		{
			pQueryMgr->destroyQueryObject(m_DummyObjects[i]);
		}
		m_DummyObjects.clear();
#endif

		if (mSkeletonMaster && mBoneNodeVector)
		{
			for (int i = mSkeletonMaster->getTotalCount() - 1; i >= 0; --i)
			{
				if (mBoneNodeVector[i])
				{
					mBoneNodeVector[i]->getParent()->destroyChild(mBoneNodeVector[i]);
				}
			}
			LordFree(mBoneNodeVector);
		}

		for (set<Animation*>::type::iterator it = mAnimtions.begin(); it != mAnimtions.end(); ++it)
		{
			AnimManager::Instance()->releaseResource(*it);
		}
		mAnimtions.clear();

		if (m_pAnimBlender)
		{
			LordSafeDelete(m_pAnimBlender);
		}

		if (mSkeletonMaster)
		{
			SkeletonManager::Instance()->releaseResource(mSkeletonMaster);
			mSkeletonMaster = NULL;
		}

		mHasInited = false;
	}
	//-----------------------------------------------------------------------
	String ActorObject::GetActorFileName() const
	{
		return m_pTemplateData->TemplateName;
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::getSocketSceneNode(const String &SocketName)
	{
		if (SocketName == "BuSuiFu")
		{
			return mSceneNodeMaster;
		}
		else if (SocketName == "Top_Box")
		{
			return getTopBoxSceneNode();
		}
		else if (SocketName == "Center_Box")
		{
			return getCenterBoxSceneNode();
		}

		if (!mSkeletonMaster)
		{
			return mSceneNodeMaster;
		}

		Bone *pBone = mSkeletonMaster->getBone(SocketName);
		if (!pBone)
		{
			return mSceneNodeMaster;
		}

		SceneNode *pSocketSceneNode = getBoneNode(pBone);
		if (!pSocketSceneNode)
		{
			return mSceneNodeMaster;
		}

		return pSocketSceneNode;
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::getBuffEffectSceneNode(void)
	{
		if (!mBuffEffectSceneNode)
		{
			// mBuffEffectSceneNode
			mBuffEffectSceneNode = mSceneNodeMaster->createChild();

			Vector3 scale;
			scale.x = 1.0f / m_pTemplateData->OriginalScale.x;
			scale.y = 1.0f / m_pTemplateData->OriginalScale.y;
			scale.z = 1.0f / m_pTemplateData->OriginalScale.z;
			mBuffEffectSceneNode->setLocalScaling(scale);
			mBuffEffectSceneNode->setLocalPosition(m_pTemplateData->mMainRootBonePosition);

			mBuffEffectSceneNode->update();
		}

		return mBuffEffectSceneNode;
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::getAOEEffectSceneNode(void)
	{
		if (!mAOEEffectSceneNode)
		{
			// mAOEEffectSceneNode
			mAOEEffectSceneNode = ActorManager::Instance()->GetActorRootSceneNode()->createChild();

			Vector3 scale;
			scale.x = 1.0f / m_pTemplateData->OriginalScale.x;
			scale.y = 1.0f / m_pTemplateData->OriginalScale.y;
			scale.z = 1.0f / m_pTemplateData->OriginalScale.z;
			mAOEEffectSceneNode->setLocalScaling(scale);
			mAOEEffectSceneNode->setLocalPosition(m_pTemplateData->mMainRootBonePosition);

			mAOEEffectSceneNode->update();
		}

		return mAOEEffectSceneNode;
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::getTopBoxSceneNode(void)
	{
		Box actorBox = getLocalAABB();
		Vector3 boxcenter = actorBox.getCenter();
		boxcenter.y += actorBox.getDY() * 0.5f;
		if (!mTopBoxSceneNode)
		{
			mTopBoxSceneNode = mSceneNodeMaster->createChild();
			Vector3 scale;
			scale.x = 1.0f / m_pTemplateData->OriginalScale.x;
			scale.y = 1.0f / m_pTemplateData->OriginalScale.y;
			scale.z = 1.0f / m_pTemplateData->OriginalScale.z;
			mTopBoxSceneNode->setLocalScaling(scale);
			mTopBoxSceneNode->setLocalPosition(Vector3(0.0f, boxcenter.y, 0.0f));
		}

		return mTopBoxSceneNode;
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::getCenterBoxSceneNode(void)
	{
		Box actorBox = getLocalAABB();
		Vector3 boxcenter = actorBox.getCenter();
		if (!mCenterBoxSceneNode)
		{
			mCenterBoxSceneNode = mSceneNodeMaster->createChild();
			Vector3 scale;
			scale.x = 1.0f / m_pTemplateData->OriginalScale.x;
			scale.y = 1.0f / m_pTemplateData->OriginalScale.x;
			scale.z = 1.0f / m_pTemplateData->OriginalScale.x;
			mCenterBoxSceneNode->setLocalScaling(scale);
			mCenterBoxSceneNode->setLocalPosition(Vector3(0.0f, boxcenter.y, 0.0f));
		}

		return mCenterBoxSceneNode;
	}
	//-----------------------------------------------------------------------
	Vector3 ActorObject::GetSocketPosition(const String &socketName)
	{
		SceneNode *pSceneNode = getSocketSceneNode(socketName);
		return pSceneNode->getWorldPosition();
	}
	//-----------------------------------------------------------------------
	Vector3 ActorObject::GetTextWorldPosition(void)
	{
		// unsigned char textMoveMode = mSkillSystem.GetTextMoveMode();
		Vector3 TextPos;
		TextPos = GetPosition();
		TextPos.y += m_pTemplateData->mTextHeightOriginal * mScale.y + m_pTemplateData->mTextHeight * mScale.y;

		return TextPos;
	}
	//-----------------------------------------------------------------------
	float ActorObject::GetTextHeightOffset() const
	{
		return m_pTemplateData->mTextHeight;
	}
	//-----------------------------------------------------------------------
	void ActorObject::getChildActorBySocket(const String &SocketName, vector<ActorObject*>::type& ChildActors) const
	{
		ChildActors.clear();
		set<ActorObject*>::type::const_iterator it;
		for (it = mChildActors.begin(); it != mChildActors.end(); ++it)
		{
			if ((*it)->mParentSocketName == SocketName)
			{
				ChildActors.push_back(*it);
			}
		}
	}
	//-----------------------------------------------------------------------
	SceneNode* ActorObject::GetSceneNodeMaster(void) const
	{
		return mSceneNodeMaster;
	}
	//-----------------------------------------------------------------------
	Skeleton* ActorObject::GetSkeletonMaster(void) const
	{
		return mSkeletonMaster;
	}
	//-----------------------------------------------------------------------
	/*SkinedEntity* ActorObject::GetEntityMaster(void) const
	{
	return mEntityMaster;
	}*/
	//-----------------------------------------------------------------------
	int ActorObject::GetSkillTimeLength(const String& name)
	{
		int animLength = 0;

		assert(m_pTemplateData);
		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(name);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			if (it->second->UserDefineTime > -1.5f)
			{
				return int(it->second->UserDefineTime * 1000.0f);
			}

			map<String, ActorTemplateData::AnimationInfo*>::type::iterator itAnim;
			for (itAnim = it->second->Animations.begin(); itAnim != it->second->Animations.end(); ++itAnim)
			{
				ActorTemplateData::AnimationInfo* pAnim = itAnim->second;
				if (pAnim->times <= 0)
				{
					int DurationTime = pAnim->beginTime + ui32((float)pAnim->Length*pAnim->times*-1 / pAnim->TimeScale);
					if (animLength < DurationTime)
					{
						animLength = DurationTime;
					}
				}
				else if (animLength > -1)
				{
					int DurationTime = pAnim->beginTime + ui32((float)pAnim->Length*pAnim->times / pAnim->TimeScale);
					if (animLength < DurationTime)
					{
						animLength = DurationTime;
					}
				}
			}
		}

		return animLength;
	}
	//-----------------------------------------------------------------------
	void ActorObject::GetSkillAnimTimeLength(const String& name, SkillState& __state)
	{
		assert(m_pTemplateData);
		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(name);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			map<String, ActorTemplateData::AnimationInfo*>::type::iterator itAnim;
			for (itAnim = it->second->Animations.begin(); itAnim != it->second->Animations.end(); ++itAnim)
			{
				ActorTemplateData::AnimationInfo* pAnim = itAnim->second;
				__state.skillAnimTime = pAnim->beginTime + ui32((float)pAnim->Length*pAnim->times / pAnim->TimeScale);
			}
			__state.AttCount = (ui32)it->second->AttackEvents.size();
		}
		else
		{
			__state.skillAnimTime = 0;
			__state.AttCount = 0;
		}
	}
	//-----------------------------------------------------------------------
	int ActorObject::PlaySkill(const String& name, bool bInsert, bool bCascade, float TimeScale, int userdata, int userdata2, const String& actorNameArray, bool playsound)
	{
		if (m_isHide || m_isBackup)
			return -3;

		if (!mHasInited)
		{
			SkillDelayedPlayProcess* pSkllEvent = LordNew SkillDelayedPlayProcess(name, bInsert, bCascade, TimeScale, userdata, userdata2, actorNameArray);
			mDelayProcessVector.push_back(pSkllEvent);
			return GetSkillTimeLength(name);
		}

		if (bCascade)
		{
			for (set<ActorObject*>::type::const_iterator itActor = mChildActors.begin();
				itActor != mChildActors.end(); ++itActor)
			{
				LordAssert(*itActor);

				const map<String, ActorTemplateData::Skill*>::type& skillsMap = (*itActor)->GetTemplateData()->SkillsMap;
				map<String, ActorTemplateData::Skill*>::type::const_iterator it = skillsMap.find(name);
				if (it != skillsMap.end())
				{
					(*itActor)->PlaySkill(name, bInsert, bCascade, TimeScale, userdata, userdata2);
				}
				/*else
				{
				LordException("ActorObject::PlaySkill : can't find the skill %s", name.c_str());
				}*/
			}
		}

		int len = mSkillSystem.PlaySkill(name, bInsert, TimeScale, userdata, userdata2, actorNameArray, playsound);
		return len;
	}
	//-----------------------------------------------------------------------
	void ActorObject::PlayUpperAnim(const String& name, bool bLooped, float timeScale)
	{
		mSkillSystem.PlayUpperAnim(name, bLooped, timeScale);
	}
	//-----------------------------------------------------------------------
	void ActorObject::StopSkill(bool bCascade)
	{
		if (!mHasInited)
		{
			SkillDelayedStopProcess* pEvent = LordNew SkillDelayedStopProcess(bCascade);
			mDelayProcessVector.push_back(pEvent);
			return;
		}

		mSkillSystem.StopSkill();

		mTargetActors.clear();

		if (bCascade)
		{
			for (set<ActorObject*>::type::const_iterator itActor = mChildActors.begin();
				itActor != mChildActors.end(); ++itActor)
			{
				LordAssert(*itActor);

				(*itActor)->StopSkill(bCascade);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::PlayWoundAction(SkillAttack* skillAtt)
	{
		//if(mSkillSystem.IsWoundActionEnable() == true)
		{
			//PlaySkill("hit",false)

			ActorManager::Wound_CallBack woundCallBackFunc = ActorManager::Instance()->getWoundCallBack();
			if (woundCallBackFunc)
			{
				if (skillAtt->getIsCallBlack())
				{
					(woundCallBackFunc)(this, skillAtt->getSkillName(), skillAtt->getTargetActor(), skillAtt->getAttNameID(),
						skillAtt->GetCallBackIndex(), skillAtt->GetCallBackNum(), skillAtt->GetWoundSkill(), skillAtt->GetUserData(), skillAtt->GetUserData2());
					LordAssert(skillAtt->GetCallBackIndex() > 0);
					LordAssert(skillAtt->GetCallBackIndex() <= skillAtt->GetCallBackNum());

					LordLogDebug("------------------------------- attack events: index[%d] num[%d] -------------------------------", skillAtt->GetCallBackIndex(), skillAtt->GetCallBackNum());
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::PlayWoundSound()
	{
		ActorTemplateData::WoundSound tWoundSound = m_pTemplateData->GetWoundSound();

		if (tWoundSound.soundFileNum < 1)
			return;

		int randInt = (int)Math::IntervalRandom(1.0f, (float)tWoundSound.soundFileNum);

		String soundNamePlay;
		switch (randInt)
		{
		case 1:
			soundNamePlay = tWoundSound.soundName1; break;
		case 2:
			soundNamePlay = tWoundSound.soundName2; break;
		default:
			break;
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetAttackTargetActors(Attack_Event_Info* pAttackEventInfo, unsigned int NumActors)
	{
		mTargetActors.clear();
		for (unsigned int i = 0; i<NumActors; ++i)
			mTargetActors.push_back(pAttackEventInfo[i]);
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddAttackTargetActors(ActorObject* pTarget, bool bMiss)
	{
		Attack_Event_Info info;
		info.pTarget = pTarget;
		info.bMiss = bMiss;
		mTargetActors.push_back(info);
	}
	//-----------------------------------------------------------------------
	void ActorObject::ClearAttackTargetActors()
	{
		mTargetActors.clear();
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSkillAttackToTargetActors(const ActorTemplateData::AttackEvent* pData, const String& skillName, int userData, int userData2, int callnum, int callindex)
	{
		vector<Attack_Event_Info>::type::iterator it;
		for (it = mTargetActors.begin(); it != mTargetActors.end();)
		{
			ActorObject* pActor = it->pTarget;
			if (!pActor)
			{
				SkillAttack* pSkillAttack = SkillAttackManager::Instance()->createInstance(pData->AttackFileName);
				if (pSkillAttack)
				{
					pSkillAttack->setIsDirectivity(false);
					UpdateLogicState();
					pSkillAttack->SetUserData(userData);
					pSkillAttack->SetUserData2(userData2);
					pSkillAttack->setAttNameID(pData->AttackFileName);
					pSkillAttack->setSkillName(skillName);
					pSkillAttack->setIsCallBlack(pData->isCallback);
					if (pData->isCallback)
					{
						pSkillAttack->SetCallBackIndex(callindex);
						pSkillAttack->SetCallBackNum(callnum);
					}
					pSkillAttack->prepare(this);
					pSkillAttack->play();

					this->AddSkillAttack(pSkillAttack);
				}
				++it;
			}
			else
			{
				if (!ActorManager::Instance()->ValidActor(pActor))
				{
					it = mTargetActors.erase(it++);
				}
				else
				{
					if (pActor->getHasInited())
					{
						SkillAttack* pSkillAttack = SkillAttackManager::Instance()->createInstance(pData->AttackFileName);
						if (pSkillAttack)
						{
							AttReplaceMap* _attMap = ActorManager::Instance()->GetAttReplaceMap();
							if (_attMap && (_attMap->size() > (unsigned int)pSkillAttack->getReplaceATTIndex()) && pSkillAttack->getNeedReplaceATT())
							{
								String _replaceAttName = _attMap->at(pSkillAttack->getReplaceATTIndex());
								SkillAttackManager::Instance()->destroyInstance(pSkillAttack);
								pSkillAttack = SkillAttackManager::Instance()->createInstance(_replaceAttName);
							}
						}
						if (pSkillAttack)
						{
							pSkillAttack->setTargetActor(pActor);
							//pSkillAttack->setFlyingTime(it->FlyingTime);

							if (true == it->bMiss)
							{
								pSkillAttack->setSkillMiss(true);
							}

							UpdateLogicState();
							pActor->UpdateLogicState();
							pSkillAttack->SetUserData(userData);
							pSkillAttack->SetUserData2(userData2);
							pSkillAttack->setAttNameID(pData->AttackFileName);
							pSkillAttack->setSkillName(skillName);
							pSkillAttack->setIsCallBlack(pData->isCallback);
							if (pData->isCallback)
							{
								pSkillAttack->SetCallBackIndex(callindex);
								pSkillAttack->SetCallBackNum(callnum);
							}
							pSkillAttack->prepare(this);
							pSkillAttack->play();

							pActor->AddSkillAttack(pSkillAttack);
						}
					}
					++it;
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::GetAttackTargetActors(vector<ActorObject*>::type& list)
	{
		vector<Attack_Event_Info>::type::iterator it;
		for (it = mTargetActors.begin(); it != mTargetActors.end(); ++it)
		{
			list.push_back(it->pTarget);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::AttachToParentNode(SceneNode* pParentSceneNode)
	{
		DetachFromParentNode();

		SceneNode *pParentNode = NULL;

		if (pParentSceneNode)
		{
			pParentNode = pParentSceneNode;
		}
		else
		{
			pParentNode = GetActorRootSceneNode();
		}

		assert(pParentNode);
		mSceneNodeMaster->setParent(pParentNode);

		SetScale(mScale);
		SetPosition(mPosition);
		SetOrientation(mOrientation);
	}
	//-----------------------------------------------------------------------
	void ActorObject::DetachFromParentNode()
	{
		if (mSceneNodeMaster)
		{
			if (SceneNode *pParentNode = mSceneNodeMaster->getParent())
			{
				mSceneNodeMaster->setParent(NULL);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetScale(const Vector3 &scale)
	{
		mScale = scale;
		mScaleDirty = true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetPosition(const Vector3 &position)
	{
		mPosition = position;
		mPositionDirty = true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetOrientation(const Quaternion &orientation)
	{
		mOrientation = orientation;
		mOrientationDirty = true;
	}
	//-----------------------------------------------------------------------
	const Vector3& ActorObject::GetScale(void) const
	{
		return mScale;
	}
	//-----------------------------------------------------------------------
	const Vector3& ActorObject::GetOriginalScale(void) const
	{
		return m_pTemplateData->OriginalScale;
	}
	//-----------------------------------------------------------------------
	float ActorObject::GetAttackScale(void) const
	{
		return m_pTemplateData->mAttackScale;
	}
	//-----------------------------------------------------------------------
	float ActorObject::GetSelectedRingSize(void) const
	{
		return m_pTemplateData->mSelectedRingSize * mScale.x;
	}
	//-----------------------------------------------------------------------
	const Vector3& ActorObject::GetPosition(void) const
	{
		return mPosition;
	}
	//-----------------------------------------------------------------------
	const Quaternion& ActorObject::GetOrientation(void) const
	{
		return mOrientation;
	}
	//-----------------------------------------------------------------------
	ActorObject* ActorObject::GetRootParent(void) const
	{
		if (mParentActor)
		{
			return mParentActor->GetRootParent();
		}

		return (ActorObject*)this;
	}
	//-----------------------------------------------------------------------
	float ActorObject::AddPluginEffect(const String &Name,
		const String &SocketName,
		const String &EffectName,
		float TimeScale,
		float Alpha,
		int times,
		const Vector3 &Position,
		const Quaternion &Orientation,
		const Vector3 &Scale,
		bool isOrientFollowParentNode)
	{
		if (!mHasInited)
		{
			EffectDelayedAddProcess* pEffectEven = LordNew EffectDelayedAddProcess;
			pEffectEven->m_Name = Name;
			pEffectEven->m_SocketName = SocketName;
			pEffectEven->m_EffectName = EffectName;
			pEffectEven->m_TimeScale = TimeScale;
			pEffectEven->m_Alpha = Alpha;
			pEffectEven->m_Times = times;
			pEffectEven->m_Position = Position;
			pEffectEven->m_Orientation = Orientation;
			pEffectEven->m_Scale = Scale;
			pEffectEven->m_isOrientFollowParentNode = isOrientFollowParentNode;
			mDelayProcessVector.push_back(pEffectEven);

			return 0.0f;
		}

		if (!mSceneNodeMaster)
		{
			return 0.0f;
		}

		if (mPluginEffects.find(Name) != mPluginEffects.end())
		{
			return 0.0f;
		}
		Actor_Effect effect;
		//static int skill_effectCount = 0;
		if (!EffectName.empty())
		{
			effect.pEffect = EffectSystemManager::Instance()->createEffectSystem(GetNextEffectName(), EffectName, m_isUIActor);
		}
		else
		{
			//LordException("ActorObject::AddPluginEffect: effect name error");
			LordLogError("ActorObject::AddPluginEffect: effect name error");
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			MessageBox(0, "ActorObject::AddPluginEffect: effect name error", "Error", 0);
#endif
			return 0.0f;
		}

		effect.bFadeOut = false;
		effect.Weight = 1.0f;
		effect.TimeScale = TimeScale;
		effect.times = times;
		effect.Alpha = Alpha;
		effect.isOrientFollowParentNode = isOrientFollowParentNode;

		SceneNode* pParentSceneNode = NULL;
		if (effect.isOrientFollowParentNode)
		{
			if (SocketName == "buff_effect")
			{
				pParentSceneNode = getBuffEffectSceneNode();
			}
			else if (SocketName == "aoe_effect")
			{
				pParentSceneNode = getAOEEffectSceneNode();
				pParentSceneNode->setLocalPosition(mSceneNodeMaster->getWorldPosition());
			}
			else
			{
				pParentSceneNode = getSocketSceneNode(SocketName);
			}
			if (!pParentSceneNode)
			{
				pParentSceneNode = mSceneNodeMaster;
			}
		}
		else
		{
			pParentSceneNode = GetActorRootSceneNode();
		}

		SceneNode* pEffectSceneNode = pParentSceneNode->createChild();
		effect.pEffect->setParentSceneNode(pEffectSceneNode);
		effect.pEffectNode = pEffectSceneNode;
		effect.pParentNode = pParentSceneNode;

		if (isOrientFollowParentNode)
		{
			effect.pEffectNode->setLocalPosition(Position);
			effect.pEffectNode->setLocalOrientation(Orientation);
		}
		else
		{
			effect.pEffectNode->setWorldPosition(pParentSceneNode->getWorldPosition());
			effect.pEffectNode->setWorldOrientation(Orientation);
		}

		effect.pEffectNode->setLocalScaling(Scale);
		effect.pEffectNode->update();
		if (m_isUIActor)
		{
			effect.pEffect->prepare_sys(true);
			effect.pEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
		}
		else
		{
			effect.pEffect->prepare_sys(true);
			effect.pEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
		}
		effect.pEffect->start();

		mPluginEffects.insert(std::pair<String, Actor_Effect>(Name, effect));

		if (effect.times <= 0)
		{
			return -1.0f;
		}
		else
		{
			float TimeLength = effect.pEffect->getEffectTimeLength();
			if (TimeLength < 0.0f)
			{
				return -1.0f;
			}
			else
			{
				return TimeLength * effect.times;
			}
		}

	}
	//-----------------------------------------------------------------------
	void ActorObject::DelPluginEffect(const String &Name, bool Smooth)
	{
		map<String, Actor_Effect>::type::iterator itEffect = mPluginEffects.find(Name);

		if (itEffect != mPluginEffects.end())
		{
			if (Smooth)
			{
				itEffect->second.bFadeOut = true;
			}
			else
			{
				itEffect->second.pEffect->unprepare_sys();
				EffectSystemManager::Instance()->destroyEffectSystem(itEffect->second.pEffect);
				itEffect->second.pEffect = NULL;
				itEffect->second.pEffectNode->getParent()->destroyChild(itEffect->second.pEffectNode);
				mPluginEffects.erase(itEffect);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetPluginEffectQuaternion(const String &Name, const Quaternion &Orientation)
	{
		map<String, Actor_Effect>::type::iterator itEffect = mPluginEffects.find(Name);

		if (itEffect != mPluginEffects.end())
		{
			itEffect->second.pEffectNode->setWorldOrientation(Orientation);
			itEffect->second.pEffectNode->update();
		}
	}
	//-----------------------------------------------------------------------
	ActorObject::Child_Mesh* ActorObject::AddChildMesh(const String &Name,
		const String &SocketName,
		const String &MeshName,
		const Vector3 &Position,
		const Quaternion &Orientation,
		const Vector3 &Scale)
	{
		if (!mSceneNodeMaster)
		{
			return NULL;
		}

		if (mChildMeshs.find(Name) != mChildMeshs.end())
		{
			return NULL;
		}

		SceneNode* pParentSceneNode = getSocketSceneNode(SocketName);

		if (!pParentSceneNode)
		{
			pParentSceneNode = mSceneNodeMaster;
		}
		LORD::Mesh* pMesh = NULL;
		Child_Mesh* childmesh = new Child_Mesh;
		childmesh->Name = Name;
		childmesh->SocketName = SocketName;
		if (!MeshName.empty())
		{
			pMesh = MeshManager::Instance()->createMesh(MeshName, true, true);
		}
		else
		{
			LordLogError("ActorObject::AddChildMesh: mesh name error");
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			MessageBox(0, "ActorObject::AddChildMesh: mesh name error", "Error", 0);
#endif
			delete childmesh;
			return NULL;
			//LordException("ActorObject::AddChildMesh: mesh name error");
		}

		pMesh->load();
		SceneNode* pMeshSceneNode = pParentSceneNode->createChild();
		ModelEntity* pModelEntity = LordNew ModelEntity;
		pModelEntity->setMesh(pMesh);
		pModelEntity->attachTo(pMeshSceneNode);
		pModelEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial());
		pModelEntity->createRenderable(m_isUIActor);
		pModelEntity->setNaturalColor(mNaturalColor);
		pModelEntity->setCustomColor(mCustomColor);
		pModelEntity->setAlphaColor(mAlphaColor);

		for (int i = 0; i < pMesh->getSubMeshNum(); ++i)
		{
			SubMesh* pSubMesh = pMesh->getSubMesh(i);
			String name = pMesh->getTextureName(i, 0);
			Texture* pTex = TextureManager::Instance()->createTexture(name);
			if (pTex->load())
			{
				pModelEntity->replaceTexture(pTex, i, 0);
			}
			else
			{
				LordLogError("ActorObject::Child_Mesh: Texture isn't exits %s.", name.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "ActorObject::Child_Mesh: Texture isn't exits %s.", name.c_str());
				MessageBox(0, buf, 0, 0);
#endif
			}
		}


		childmesh->pMesh = pMesh;
		childmesh->pModelEntity = pModelEntity;
		childmesh->pModelNode = pMeshSceneNode;
		childmesh->pParentNode = pParentSceneNode;

		childmesh->pModelNode->setLocalPosition(Position);
		childmesh->pModelNode->setLocalOrientation(Orientation);
		childmesh->pModelNode->setLocalScaling(Scale);

		mChildMeshs.insert(std::pair<String, Child_Mesh*>(Name, childmesh));

		return childmesh;
	}
	//-----------------------------------------------------------------------
	void ActorObject::DeleteChildMesh(const String &Name)
	{
		map<String, Child_Mesh*>::type::iterator itMesh = mChildMeshs.find(Name);

		if (itMesh != mChildMeshs.end())
		{
			MeshManager::Instance()->releaseResource(itMesh->second->pMesh);
			itMesh->second->pMesh = NULL;
			LordDelete itMesh->second->pModelEntity;
			itMesh->second->pModelEntity = NULL;
			itMesh->second->pParentNode->destroyChild(itMesh->second->pModelNode);
			delete itMesh->second;
			itMesh->second = NULL;
			mChildMeshs.erase(itMesh);
		}
	}
	//-----------------------------------------------------------------------
	bool ActorObject::AddMeshTexModify(const String& master, const String& slave, const Vector2i& pos, const Vector2i& size)
	{
		// ActorObject only support ONE modifier
		if (m_meshTexModify)
			return false;

		m_meshTexModify = LordNew ActorMeshTexModify(master, slave, pos, size);
		return true;
	}
	//-----------------------------------------------------------------------
	bool ActorObject::DelMeshTexModify()
	{
		if (!m_meshTexModify)
			return false;
		LordSafeDelete(m_meshTexModify);
		return true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetTimeScale(float timeScale, bool bCascade)
	{
		mBaseScale = timeScale;
		mTimeScale = mBaseScale;
		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetTimeScale(timeScale, bCascade);
			}
		}
		/*mTimeScale = timeScale * mBaseScale;

		if (bCascade)
		{
		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
		{
		(*itActor)->SetTimeScale(timeScale, bCascade);
		}
		}*/
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetCurrentTimeScale(float timeScale, bool bCascade /*= false*/)
	{
		mTimeScale = timeScale * mBaseScale;

		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetCurrentTimeScale(timeScale, bCascade);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetDefaultColor(bool bSmooth, bool bCascade)
	{
		if (m_isActorFadeOut)
			return;

		if (true == bSmooth)
		{
			mColorChangeState.color = Color::WHITE;
			mColorChangeState.smoothTime = COLOR_CHANGE_TIME;
		}
		else
		{
			mColorChangeState.smoothTime = -1000;

			mNaturalColor = Color::WHITE;
			mAlphaColor = Color::WHITE;
			mAlphaColor.a = m_fAlpha * m_pTemplateData->mfAlpha;
			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				SkinedEntity* pEntity = *it;
				if (pEntity)
				{
					pEntity->setNaturalColor(mNaturalColor);
					pEntity->setAlphaColor(mAlphaColor);
				}
			}
		}

		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetDefaultColor(bSmooth, bCascade);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetNaturalColor(const Color &color, bool bSmooth, ui32 FadeTime, bool bCascade)
	{
		if (m_isActorFadeOut)
			return;

		if (true == bSmooth)
		{
			mColorChangeState.color = color;
			mColorChangeState.smoothTime = FadeTime;
			mColorChangeState.resumeTime = COLOR_CHANGE_TIME;
		}
		else
		{
			mColorChangeState.smoothTime = -1000;
			mColorChangeState.resumeTime = COLOR_CHANGE_TIME;
			mNaturalColor = color;

			multimap<String, Body_Part_Info*>::type::iterator it;
			vector<Skin_Mesh>::type::iterator itMesh;
			for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
			{
				for (itMesh = it->second->SkinMeshes.begin(); itMesh != it->second->SkinMeshes.end(); ++itMesh)
				{
					if (itMesh->pEntity)
						itMesh->pEntity->setNaturalColor(mNaturalColor);
					if (itMesh->pEntity_WriteZ)
						itMesh->pEntity_WriteZ->setNaturalColor(mNaturalColor);
				}
			}
		}

		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetNaturalColor(color, bSmooth, FadeTime, bCascade);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetNaturalColor(const ActorTemplateData::ColorChange* pData)
	{
		if (m_isActorFadeOut)
			return;

		mColorChangeState.color = pData->color;
		mColorChangeState.smoothTime = int(pData->smoothTime * 1000);
		mColorChangeState.durationTime = int(pData->durationTime * 1000);
		mColorChangeState.resumeTime = int(pData->resumeTime * 1000);

		if (pData->bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetNaturalColor(pData);
			}
		}
	}
	void ActorObject::SetNaturalColor(const Color &color, bool bCascade)
	{
		if (m_isActorFadeOut)
			return;

		mNaturalColor = color;
		multimap<String, Body_Part_Info*>::type::iterator it;
		vector<Skin_Mesh>::type::iterator itMesh;
		for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
		{
			for (itMesh = it->second->SkinMeshes.begin(); itMesh != it->second->SkinMeshes.end(); ++itMesh)
			{
				if (itMesh->pEntity)
					itMesh->pEntity->setNaturalColor(mNaturalColor);
				if (itMesh->pEntity_WriteZ)
					itMesh->pEntity_WriteZ->setNaturalColor(mNaturalColor);
			}
		}
		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetNaturalColor(color, bCascade);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetCustomColor(const Color& color)
	{
		mCustomColor = color;
		multimap<String, Body_Part_Info*>::type::iterator it;
		vector<Skin_Mesh>::type::iterator itMesh;
		for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
		{
			for (itMesh = it->second->SkinMeshes.begin(); itMesh != it->second->SkinMeshes.end(); ++itMesh)
			{
				if(itMesh->pEntity)
					itMesh->pEntity->setCustomColor(mCustomColor);
				if (itMesh->pEntity_WriteZ)
					itMesh->pEntity_WriteZ->setCustomColor(mCustomColor);
			}
		}
		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
		{
			(*itActor)->SetCustomColor(color);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetCameraShakeEnable(bool enable, bool bCascade)
	{
		mSkillSystem.SetCameraShakeEnable(enable);

		if (bCascade)
		{
			set<ActorObject*>::type::iterator itActor;
			for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
			{
				(*itActor)->SetCameraShakeEnable(enable, bCascade);
			}
		}
	}
	//---------------------------------------------------------------------
	void ActorObject::AddCameraShake(float beginTime, float scale, float durationTime, int shakeTimes)
	{
		if (!ActorManager::Instance()->GetActorsSkillCameraEnable())
			return;

		if (m_CameraPush.durationTime > 0.0f)
			return;

		SceneManager::Instance()->addCameraShake(beginTime, scale, durationTime, shakeTimes);
	}
	//---------------------------------------------------------------------
	void ActorObject::DeleteCameraShake(unsigned int id)
	{

	}
	//---------------------------------------------------------------------
	void ActorObject::ClearAllCameraShake(void)
	{

	}
	//-----------------------------------------------------------------------
	void ActorObject::SetMasterSkeleton(Skeleton* pSkeleton)
	{
		if (pSkeleton)
		{
			mSkeletonMaster = pSkeleton;
			m_pAnimBlender = LordNew AnimBlender(mSkeletonMaster);
			initBoneNodes();

			mSkillSystem.Free();
			mSkillSystem.Init(m_pTemplateData, this);

			mHasInited = true;
		}
	}

	void ActorObject::AttachSlave(const String& masterName, const String &slaveName)
	{
		if (mbSingleThread)
		{
			UseBodyPart(masterName, slaveName);
		}
		else
		{
			if (!mHasInited)
			{
				SlaveAttachProcess* pSlaveProcess = LordNew SlaveAttachProcess(masterName, slaveName);
				mDelayProcessVector.push_back(pSlaveProcess);
				return;
			}

			bool loaded = false;

			if (!mUnloadAllBodyParts)
			{
				// bool ret = true;
				vector<LORD::SkinnedMesh*>::type pMeshes;
				std::pair<multimap<String, Body_Part_Info*>::type::iterator, multimap<String, Body_Part_Info*>::type::iterator> itBodyParts;
				itBodyParts = mBodyParts.equal_range(masterName);

				multimap<String, Body_Part_Info*>::type::iterator it, tempIt;
				for (it = itBodyParts.first; it != itBodyParts.second;)
				{
					if (it->second->pTemplateData->SlaveName == slaveName)
					{
						loaded = true;
						++it;
					}
					else
					{
						tempIt = it++;
						UnloadBodyPart(tempIt->second);
						LORD_DELETE_T(tempIt->second, Body_Part_Info);
						tempIt->second = NULL;
						mBodyParts.erase(tempIt);
					}
				}
			}

			if (!loaded)
			{
				StringArray meshNames;
				m_pTemplateData->GetMeshNames(masterName, slaveName, meshNames);
				ui32 numSkins = (ui32)meshNames.size();
				vector<SkinnedMesh*>::type pMeshes;
				pMeshes.resize(numSkins);
				for (ui32 i = 0; i<numSkins; ++i)
				{
					pMeshes[i] = MeshManager::Instance()->createSkinnedMesh(meshNames[i], true, true);
					if (!pMeshes[i])
					{
						LordLogError("ActorObject::AttachSlave: SkinedMesh create error! line:%d", __LINE__);
					}
				}

				StreamThread* pThread = StreamThread::Instance();
				if (pThread && pThread->IsRunning())
				{
					ActorAttachMeshEvent* pEvent = LordNew ActorAttachMeshEvent(this, pMeshes, masterName, slaveName);
					pThread->AddRequest(pEvent);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	bool ActorObject::UseBodyPart(const String& masterName, const String &slaveName)
	{
		bool ret = true;
		vector<LORD::SkinnedMesh*>::type pMeshes;
		std::pair<multimap<String, Body_Part_Info*>::type::iterator, multimap<String, Body_Part_Info*>::type::iterator> itBodyParts;
		itBodyParts = mBodyParts.equal_range(masterName);

		bool loaded = false;
		multimap<String, Body_Part_Info*>::type::iterator it, tempIt;
		for (it = itBodyParts.first; it != itBodyParts.second;)
		{
			if (it->second->pTemplateData->SlaveName == slaveName)
			{
				loaded = true;
				++it;
			}
			else
			{
				tempIt = it++;
				UnloadBodyPart(tempIt->second);
				LORD_DELETE_T(tempIt->second, Body_Part_Info);
				tempIt->second = NULL;
				mBodyParts.erase(tempIt);
			}
		}

		if (false == loaded)
		{
			map<String, ActorTemplateData::BodyPart*>::type::const_iterator it;
			it = m_pTemplateData->BodyPartsMap.find(masterName + "." + slaveName);
			if (it == m_pTemplateData->BodyPartsMap.end())
				return false;

			Body_Part_Info* pBodyPart = LORD_NEW_T(Body_Part_Info);
			LordAssert(pBodyPart);
			pBodyPart->pTemplateData = it->second;
			CreateBodyPartMeshes(pBodyPart, pMeshes);
			LoadBodyPart(pBodyPart, pMeshes); //此函数在createBodyPartMeshes()里有调

			assert(masterName == it->second->MasterName && slaveName == it->second->SlaveName);

			mBodyParts.insert(std::pair<String, Body_Part_Info*>(masterName, pBodyPart));

			refleshActorAlpha();
		}

		return ret;
	}
	void ActorObject::_unloadAllBodyParts()
	{
		multimap<String, Body_Part_Info*>::type::iterator it;
		for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
		{
			UnloadBodyPart(it->second);
			LORD_DELETE_T(it->second, Body_Part_Info);
			it->second = NULL;
		}
		mBodyParts.clear();
		mUnloadAllBodyParts = false;
	}
	//-----------------------------------------------------------------------
	void ActorObject::EnableAllBodyParts(bool bEnabled)
	{
		if (true == bEnabled)
		{
			set<String>::type loadedBodyParts;
			multimap<String, Body_Part_Info*>::type::iterator it;
			for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
			{
				loadedBodyParts.insert(it->second->pTemplateData->SlaveName + "." + it->second->pTemplateData->SlaveName);
			}

			map<String, ActorTemplateData::BodyPart*>::type::const_iterator itBodyParts;
			for (itBodyParts = m_pTemplateData->BodyPartsMap.begin(); itBodyParts != m_pTemplateData->BodyPartsMap.end(); ++itBodyParts)
			{
				if (true == itBodyParts->second->bDefaultUse
					&& loadedBodyParts.find(itBodyParts->second->SlaveName + "." + itBodyParts->second->SlaveName) == loadedBodyParts.end())
				{
					Body_Part_Info* pBodyPart = LORD_NEW_T(Body_Part_Info);
					LordAssert(pBodyPart);
					pBodyPart->pTemplateData = itBodyParts->second;
					LoadBodyPart(pBodyPart);
					mBodyParts.insert(std::pair<String, Body_Part_Info*>(itBodyParts->second->MasterName, pBodyPart));
				}
			}
		}
		else
		{
			if (mbSingleThread)
			{
				_unloadAllBodyParts();
			}
			else
			{
				mUnloadAllBodyParts = true;
				StreamThread* pThread = StreamThread::Instance();
				if (pThread && pThread->IsRunning())
				{
					//LordLogInfo("=========[ActorDeAttachAllEvent::Call] actor(%x)", this);
					//LogBodyPartMeshs("Call DeattachAll");
					ActorDeAttachAllEvent* pEvent = LordNew ActorDeAttachAllEvent(this);
					pThread->AddRequest(pEvent);
				}
			}
		}
	}
	//---------------------------------------------------------------------
	SkinnedMesh* ActorObject::LoadSkin(Skin_Mesh &mesh, ActorTemplateData::Skin* pSkin, LORD::SkinnedMesh* pMesh)
	{
		if (pMesh && pMesh->load())
		{
			SceneNode* pNode = mSceneNodeMaster->createChild();
			SkinedEntity* pEntity = LordNew SkinedEntity;
			pEntity->attachTo(pNode);
			pEntity->setSkeleton(mSkeletonMaster);
			pEntity->setMesh(pMesh);
			pEntity->setMaterial(ActorManager::Instance()->GetSkinMaterial());
			pEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
			pEntity->setAnimBlender(m_pAnimBlender);
			pEntity->createRenderable(m_isUIActor);
			pEntity->setColorParams(mCurentColorParams);
			pEntity->setRimColor(mCurentRimColor);
			pEntity->setAmbientColor(mCurentAmbientColor);
			pEntity->setDarkRadio(m_DarkRadio);
			pEntity->setNaturalColor(mNaturalColor);
			pEntity->setCustomColor(mCustomColor);
			pEntity->setAlphaColor(mAlphaColor);
			mEntitySet.insert(pEntity);

			mesh.pTemplateSkin = pSkin;
			mesh.pMesh = pMesh;
			mesh.pEntity = pEntity;
			mesh.pEntity_WriteZ = NULL;

			//load mesh diffuse and mask texture
			mesh.pEntity->prepareTexture(mbSingleThread, __FUNCTION__);
		}
		else
		{
			LordAssert(pMesh);
			LordLogError("ActorObject::LoadSkin:: SkinedMesh file isn't exits. [%s]", pMesh->getName().c_str());

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "ActorObject::LoadSkin::SkinedMesh file isn't exist. [%s]", pMesh->getName().c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return NULL;
		}

		return pMesh;
	}

	void ActorObject::LoadSkin(Skin_Mesh &mesh, LORD::SkinnedMesh *pMesh)
	{
		if (pMesh && pMesh->load())
		{
			SceneNode* pNode = mSceneNodeMaster->createChild();
			SkinedEntity* pEntity = LordNew SkinedEntity;
			pEntity->attachTo(pNode);
			pEntity->setSkeleton(mSkeletonMaster);
			pEntity->setMesh(pMesh);
			pEntity->setMaterial(ActorManager::Instance()->GetSkinMaterial());
			pEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
			pEntity->setAnimBlender(m_pAnimBlender);
			pEntity->createRenderable(m_isUIActor);
			pEntity->setColorParams(mCurentColorParams);
			pEntity->setRimColor(mCurentRimColor);
			pEntity->setAmbientColor(mCurentAmbientColor);
			pEntity->setDarkRadio(m_DarkRadio);
			pEntity->setNaturalColor(mNaturalColor);
			pEntity->setCustomColor(mCustomColor);
			pEntity->setAlphaColor(mAlphaColor);
			mEntitySet.insert(pEntity);

			mesh.pTemplateSkin = NULL;
			mesh.pMesh = pMesh;
			mesh.pEntity = pEntity;
			mesh.pEntity_WriteZ = NULL;

			//load mesh diffuse and mask texture
			mesh.pEntity->prepareTexture(mbSingleThread, __FUNCTION__);
		}
		else
		{
			LordAssert(pMesh);
			LordLogError("ActorObject::LoadSkin:: SkinedMesh Load failed. [%s]", pMesh->getName().c_str());

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "ActorObject::LoadSkin::SkinedMesh Load failed.. [%s]", pMesh->getName().c_str());
			MessageBox(0, buf, "Error", 0);
#endif
		}
	}

	void ActorObject::LoadMergedSkin(Skin_Mesh &mesh, LORD::SkinnedMesh *pMesh)
	{
		if (pMesh && pMesh->load())
		{
			SceneNode* pNode = mSceneNodeMaster->createChild();
			SkinedEntity* pEntity = LordNew SkinedEntity;
			pEntity->attachTo(pNode);
			pEntity->setSkeleton(mSkeletonMaster);
			pEntity->setMesh(pMesh);
			pEntity->setMaterial(ActorManager::Instance()->GetSkinMaterial());
			pEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
			pEntity->setAnimBlender(m_pAnimBlender);
			pEntity->createRenderable(m_isUIActor);
			pEntity->setColorParams(mCurentColorParams);
			pEntity->setRimColor(mCurentRimColor);
			pEntity->setAmbientColor(mCurentAmbientColor);
			pEntity->setDarkRadio(m_DarkRadio);
			pEntity->setNaturalColor(mNaturalColor);
			pEntity->setCustomColor(mCustomColor);
			pEntity->setAlphaColor(mAlphaColor);
			mEntitySet.insert(pEntity);

			mesh.pTemplateSkin = NULL;
			mesh.pMesh = pMesh;
			mesh.pEntity = pEntity;
			mesh.pEntity_WriteZ = NULL;

			//load mesh diffuse and mask texture
			mesh.pEntity->prepareTexture(true, __FUNCTION__);
		}
		else
		{
			LordAssert(pMesh);
			LordLogError("ActorObject::LoadSkin:: SkinedMesh Load failed. [%s]", pMesh->getName().c_str());

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "ActorObject::LoadSkin::SkinedMesh Load failed.. [%s]", pMesh->getName().c_str());
			MessageBox(0, buf, "Error", 0);
#endif
		}
	}

	//---------------------------------------------------------------------
	void ActorObject::UnloadSkin(Skin_Mesh &mesh)
	{
		if (mesh.pMesh)
		{
			set<SkinedEntity*>::type::iterator it;
			if (mesh.pEntity_WriteZ)
			{
				it = mEntitySet.find(mesh.pEntity_WriteZ);
				mSceneNodeMaster->destroyChild(mesh.pEntity_WriteZ->getSceneNode());
				LordSafeDelete(mesh.pEntity_WriteZ);
				mEntitySet.erase(it);
			}
			it = mEntitySet.find(mesh.pEntity);
			mEntitySet.erase(it);
			mSceneNodeMaster->destroyChild(mesh.pEntity->getSceneNode());
			LordSafeDelete(mesh.pEntity);
			MeshManager::Instance()->releaseResource(mesh.pMesh);
			mesh.pMesh = NULL;
		}
	}
	//---------------------------------------------------------------------
	void ActorObject::UpdateSkinChangeColor(Skin_Mesh &mesh)
	{
		const ActorTemplateData::Skin* pSkin = mesh.pTemplateSkin;
		mesh.pEntity->setNaturalColor(pSkin->ChangeColor);
	}
	//---------------------------------------------------------------------
	ModelEntity* ActorObject::LoadStaticMesh(Static_Mesh &mesh, ActorTemplateData::StaticMesh* pStaticMesh)
	{
		mesh.pMesh = MeshManager::Instance()->createMesh(pStaticMesh->MeshName, true, true);
		if (mesh.pMesh->load())
		{
			String name = GetNextMeshName();
			mesh.pTemplateMesh = pStaticMesh;
			mesh.pEntity = LordNew ModelEntity;
			mesh.pEntity->setMesh(mesh.pMesh);
			mesh.pParentNode = getSocketSceneNode(pStaticMesh->SocketName);
			mesh.pMeshNode = mesh.pParentNode->createChild();
			mesh.pEntity->attachTo(mesh.pMeshNode);
			mesh.pEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial());
			mesh.pEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
			mesh.pEntity->setColorParams(mCurentColorParams);
			mesh.pEntity->setRimColor(mCurentRimColor);
			mesh.pEntity->setAmbientColor(mCurentAmbientColor);
			mesh.pEntity->setDarkRadio(m_DarkRadio);
			mesh.pEntity->setNaturalColor(mNaturalColor);
			mesh.pEntity->setCustomColor(mCustomColor);
			mesh.pEntity->setAlphaColor(mAlphaColor);
			mesh.pEntity->createRenderable(m_isUIActor);

			Quaternion Orientation = CaculateOritation(pStaticMesh->yaw, pStaticMesh->pitch, pStaticMesh->roll);
			mesh.pMeshNode->setLocalPosition(pStaticMesh->position);
			mesh.pMeshNode->setLocalOrientation(Orientation);
			mesh.pMeshNode->setLocalScaling(Vector3(pStaticMesh->scale, pStaticMesh->scale, pStaticMesh->scale));

			//load mesh diffuse and mask texture
			mesh.pEntity->prepareTexture(mbSingleThread, __FUNCTION__);
		}
		else
		{
			LordLogError("ActorObject::LoadStaticMesh: Mesh file isn't exits. [%s]", pStaticMesh->MeshName.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "ActorObject::LoadStaticMesh: Mesh file isn't exist. [%s]", pStaticMesh->MeshName.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			MeshManager::Instance()->releaseResource(mesh.pMesh);
			return NULL;
		}
		return mesh.pEntity;
	}
	//---------------------------------------------------------------------
	void ActorObject::UnloadStaticMesh(Static_Mesh &mesh)
	{
		LordDelete mesh.pEntity;
		mesh.pEntity = NULL;
		LORD::MeshManager::Instance()->releaseResource(mesh.pMesh);
		mesh.pMesh = NULL;
		if (mesh.pParentNode && mesh.pMeshNode)
		{
			mesh.pParentNode->destroyChild(mesh.pMeshNode);
		}
	}
	//---------------------------------------------------------------------
	EffectSystem* ActorObject::LoadEquipEffect(Equip_Effect& effect, ActorTemplateData::ActorEffect* pEffect)
	{
		//static int equip_effectCount = 0;
		if (pEffect->EffectFileName.empty() == true)
		{
			//LordException("ActorObject::LoadEquipEffect:effect name empty");
			LordLogError("ActorObject::LoadEquipEffect:effect name empty");
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			MessageBox(0, "ActorObject::LoadEquipEffect:effect name empty", "Error", 0);
#endif
			return NULL;
		}

		effect.pEffect = EffectSystemManager::Instance()->createEffectSystem(GetNextEffectName(), pEffect->EffectFileName, m_isUIActor);
		if (effect.pEffect)
		{
			effect.pTemplateEffect = pEffect;
			SceneNode* pParentSceneNode = getSocketSceneNode(pEffect->SocketName);
			if (!pParentSceneNode)
			{
				pParentSceneNode = mSceneNodeMaster;
			}

			effect.TimeScale = pEffect->TimeScale;
			effect.Alpha = pEffect->Alpha;

			SceneNode* pEffectSceneNode = pParentSceneNode->createChild();
			effect.pEffect->setParentSceneNode(pEffectSceneNode);
			effect.pEffectNode = pEffectSceneNode;
			effect.pParentNode = pParentSceneNode;

			Quaternion Orientation = CaculateOritation(pEffect->yaw, pEffect->pitch, pEffect->roll);

			effect.pEffectNode->setLocalPosition(pEffect->position);
			effect.pEffectNode->setLocalOrientation(Orientation);
			effect.pEffectNode->setLocalScaling(Vector3(pEffect->scale, pEffect->scale, pEffect->scale));

			if (m_isUIActor)
			{
				effect.pEffect->prepare_sys(true);
				effect.pEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
			}
			else
			{
				effect.pEffect->prepare_sys(true);
				effect.pEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
			}
			effect.pEffect->start();

#ifdef RENDERDUMMYNODE
			QueryObject* pObject = QueryObjectManager::Instance()->createQueryObject(mActorNameID + "_dummyobject_" + pEffect->SocketName + "_" + effect.pEffect->getName());
			pObject->createSphere(0.02f, 10, 10, Color::WHITE);
			pObject->setQueryFlag(0);
			pObject->setTestDepth(false);
			pObject->setWriteDepth(false);
			pObject->set2DMode(true);
			pObject->attachTo(effect.pEffectNode);
			m_DummyObjects.push_back(pObject);
#endif
		}
		else
		{
			LordLogError("EquipEffect: Effect file isn't exits. [%s]", pEffect->EffectFileName.c_str());

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "Effect file isn't exist. [%s]", pEffect->EffectFileName.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return NULL;
		}

		return effect.pEffect;
	}
	//---------------------------------------------------------------------
	void ActorObject::UnloadEquipEffect(Equip_Effect& effect)
	{
		effect.pEffect->unprepare_sys();
		EffectSystemManager::Instance()->destroyEffectSystem(effect.pEffect);
		effect.pEffect = NULL;
		effect.pParentNode->destroyChild(effect.pEffectNode);
	}
	//-----------------------------------------------------------------------
	void ActorObject::CreateBodyPartMeshes(const Body_Part_Info* pBodyPart, vector<LORD::SkinnedMesh*>::type& pMeshes)
	{
		pMeshes.clear();
		const vector<ActorTemplateData::Skin*>::type& Skins = pBodyPart->pTemplateData->Skins;
		size_t NumSkins = Skins.size();
		pMeshes.resize(NumSkins);

		bool isGPUSkin = Root::Instance()->isSupportGPUSkin();
		for (size_t i = 0; i < NumSkins; i++)
		{
			pMeshes[i] = MeshManager::Instance()->createSkinnedMesh(Skins[i]->MeshName, true, true);
		}
	}
	//---------------------------------------------------------------------
	bool ActorObject::LoadBodyPart(Body_Part_Info* BodyPart)
	{
		vector<LORD::SkinnedMesh*>::type pMeshes;
		CreateBodyPartMeshes(BodyPart, pMeshes);

		return LoadBodyPart(BodyPart, pMeshes);
	}
	//---------------------------------------------------------------------
	bool ActorObject::LoadBodyPart(Body_Part_Info* BodyPart, const vector<LORD::SkinnedMesh*>::type& pMeshes)
	{
		const vector<ActorTemplateData::Skin*>::type& Skins = BodyPart->pTemplateData->Skins;
		size_t NumSkins = Skins.size();
		BodyPart->SkinMeshes.resize(NumSkins);

		mCurentRimColor = BodyPart->pTemplateData->m_edgeColor;
		mCurentColorParams = BodyPart->pTemplateData->m_colorParams;

		assert(pMeshes.size() == NumSkins);

		bool ret = true;
		for (size_t i = 0; i < NumSkins; i++)
		{
			BodyPart->SkinMeshes[i].pMesh = NULL;
			if (LoadSkin(BodyPart->SkinMeshes[i], Skins[i], pMeshes[i]) == NULL)
			{
				MeshManager::Instance()->releaseResource(pMeshes[i]);
				ret = false;
			}
		}

		const vector<ActorTemplateData::ActorEffect*>::type& Effects = BodyPart->pTemplateData->ActorEffects;
		size_t NumEffects = Effects.size();
		BodyPart->EquipEffects.resize(NumEffects);
		for (size_t i = 0; i < NumEffects; i++)
		{
			LoadEquipEffect(BodyPart->EquipEffects[i], Effects[i]);
		}

		const vector<ActorTemplateData::StaticMesh*>::type& StaticMeshs = BodyPart->pTemplateData->StaticMeshs;
		size_t NumMeshs = StaticMeshs.size();
		BodyPart->StaticMeshes.resize(NumMeshs);
		for (size_t i = 0; i < NumMeshs; i++)
		{
			LoadStaticMesh(BodyPart->StaticMeshes[i], StaticMeshs[i]);
		}

		return ret;
	}
	//-----------------------------------------------------------------------
	void ActorObject::UnloadBodyPart(Body_Part_Info* BodyPart)
	{
		size_t NumMeshes = BodyPart->SkinMeshes.size();

		for (size_t i = 0; i < NumMeshes; ++i)
		{
			UnloadSkin(BodyPart->SkinMeshes[i]);
		}
		BodyPart->SkinMeshes.clear();

		size_t NumEffects = BodyPart->EquipEffects.size();

		for (size_t i = 0; i < NumEffects; ++i)
		{
			UnloadEquipEffect(BodyPart->EquipEffects[i]);
		}
		BodyPart->EquipEffects.clear();


		size_t NumStaticMeshs = BodyPart->StaticMeshes.size();

		for (size_t i = 0; i < NumStaticMeshs; ++i)
		{
			UnloadStaticMesh(BodyPart->StaticMeshes[i]);
		}
		BodyPart->StaticMeshes.clear();
	}
	//-----------------------------------------------------------------------
	void ActorObject::UnloadBodyPart(const String& master)
	{
		for (auto it = mBodyParts.begin(); it != mBodyParts.end();)
		{
			Body_Part_Info* body = it->second;
			if (body->pTemplateData->MasterName != master)
			{
				++it;
				continue;
			}
			UnloadBodyPart(body);
			LORD_DELETE_T(body, Body_Part_Info);
			it->second = NULL;
			it = mBodyParts.erase(it);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UnloadBodyPart(const String& master, const String& slave)
	{
		for (auto it = mBodyParts.begin(); it != mBodyParts.end();)
		{
			Body_Part_Info* body = it->second;
			if (body->pTemplateData->MasterName != master || body->pTemplateData->SlaveName != slave)
			{
				++it;
				continue;
			}
			UnloadBodyPart(body);
			LORD_DELETE_T(body, Body_Part_Info);
			it->second = NULL;
			it = mBodyParts.erase(it);
		}
	}
	//-----------------------------------------------------------------------
	bool ActorObject::AttachChildActor(const String &BoneName, ActorObject *pActor)
	{
		if (!pActor || !mSceneNodeMaster)
		{
			return false;
		}

		if (pActor->GetParentActor())
		{
			return false;
		}

		pActor->mParentSocketName = BoneName;

		pActor->mParentActor = this;
		mChildActors.insert(pActor);

		if (!mHasInited)
		{
			ChildAttachProcess* pChildProcess = LordNew ChildAttachProcess(pActor, BoneName);
			mDelayProcessVector.push_back(pChildProcess);
			return true;
		}

		CompleteAttachChildActor(BoneName, pActor);
		return true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::CompleteAttachChildActor(const String &BoneName, ActorObject *pActor)
	{
		SceneNode *pSocketSceneNode = getSocketSceneNode(BoneName);

		pActor->DetachFromParentNode();

		pActor->mSceneNodeMaster->setParent(pSocketSceneNode);

		pActor->mSceneNodeMaster->setLocalPosition(Vector3(0.0f, 0.0f, 0.0f));

		if (pActor->m_pShadowEffect)
		{
			/*EffectSystemManager::Instance()->destroyEffectSystem(pActor->m_pShadowEffect);
			GetActorRootSceneNode()->destroyChild(pActor->m_pShadowSceneNode);
			pActor->m_pShadowSceneNode = NULL;
			pActor->m_pShadowEffect = NULL;*/
			pActor->m_pShadowEffect->setVisible(false);
		}

		if (!pActor->mHasInited)
		{
			ChildAddDelayedProcess* pdyProcess = LordNew ChildAddDelayedProcess(BoneName);
			pActor->mDelayProcessVector.push_back(pdyProcess);
		}
		else
		{
			if (Bone* pBone = pActor->GetSkeletonMaster()->getBone(BoneName))
			{
				static Quaternion quat1(Vector3(1.0f, 0.0f, 0.0f), -Math::PI / 2);
				assert(pBone);
				pActor->mSceneNodeMaster->setLocalOrientation(quat1);
			}
			else
			{
				pActor->mSceneNodeMaster->setLocalOrientation(Quaternion());
			}
		}

		if (m_isUIActor)//如果是挂在UIActor上，则子角色也设置为UIActor
		{
			pActor->setUIActor(true);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::DetachChildActor(ActorObject *pActor)
	{
		if (!pActor || this != pActor->mParentActor)
		{
			return;
		}

		set<ActorObject*>::type::iterator itActor = mChildActors.find(pActor);

		if (itActor == mChildActors.end())
		{
			return;
		}

		mChildActors.erase(itActor);
		pActor->mParentActor = NULL;

		if (!mHasInited)
		{
			ChildDetachProcess* pdtProcess = LordNew ChildDetachProcess(pActor);
			mDelayProcessVector.push_back(pdtProcess);
			return;
		}

		CompleteDetachChildActor(pActor);
	}
	//-----------------------------------------------------------------------
	void ActorObject::CompleteDetachChildActor(ActorObject *pActor)
	{
		Vector3 oldpos = pActor->GetPosition();

		if (false == pActor->mHasInited)
		{
			pActor->DetachFromParentNode();
			SceneNode *pParentNode = GetActorRootSceneNode();
			pActor->GetSceneNodeMaster()->setParent(pParentNode);

			ChildDelDelayedProcess* pdelProcess = LordNew ChildDelDelayedProcess;
			pActor->mDelayProcessVector.push_back(pdelProcess);
		}
		else
		{
			pActor->AttachToParentNode(GetActorRootSceneNode());
		}

		if (pActor->m_pTemplateData->mbShowShadow)
		{
			/*pActor->m_pShadowEffect = EffectSystemManager::Instance()->createEffectSystem(GetNextEffectName(), "shadow.effect", pActor->m_isUIActor);
			pActor->m_pShadowSceneNode = GetActorRootSceneNode()->createChild();
			pActor->m_pShadowSceneNode->setWorldPosition(pActor->mSceneNodeMaster->getWorldPosition());
			pActor->m_pShadowSceneNode->update();
			pActor->m_pShadowEffect->setParentSceneNode(pActor->m_pShadowSceneNode);

			if (pActor->m_isUIActor)
			{
			pActor->m_pShadowEffect->prepare_sys(true);
			pActor->m_pShadowEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
			}
			else
			{
			pActor->m_pShadowEffect->prepare_sys(true);
			pActor->m_pShadowEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
			}
			pActor->m_pShadowEffect->start();*/
			pActor->m_pShadowEffect->setVisible(true);
		}

		pActor->SetPosition(oldpos);
	}
	//-----------------------------------------------------------------------
	ActorObject* ActorObject::GetParentActor() const
	{
		return mParentActor;
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSkillAttack(SkillAttack* pSkillAttack)
	{
		mSkillAttackList.push_back(pSkillAttack);
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSkillAttack(const String& name, ActorObject* pOriginActor, bool isCallBlack, int userdata, int userdata2)
	{
		if (!mHasInited)
			return;

		SkillAttack* pSkillAttack = SkillAttackManager::Instance()->createInstance(name);

		if (pSkillAttack)
		{
			pSkillAttack->setTargetActor(this);
			pSkillAttack->SetUserData(userdata);
			pSkillAttack->SetUserData2(userdata2);

			/*if(true == it->bMiss)
			{
			pSkillAttack->setSkillMiss(true);
			}*/
			if (pOriginActor)
			{
				pSkillAttack->prepare(pOriginActor);
			}
			else
			{
				pSkillAttack->prepare(this);
			}

			pSkillAttack->setAttNameID(name);
			pSkillAttack->setIsCallBlack(isCallBlack);
			if (isCallBlack)
			{
				pSkillAttack->SetCallBackIndex(1);
				pSkillAttack->SetCallBackNum(1);
			}
			pSkillAttack->play();
			//Actor
			this->AddSkillAttack(pSkillAttack);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::RemoveSkillAttack(const String& name)
	{
		list<SkillAttack*>::type::iterator itSkillAttack;
		for (itSkillAttack = mSkillAttackList.begin(); itSkillAttack != mSkillAttackList.end();)
		{
			if ((*itSkillAttack)->getAttNameID() == name)
			{
				SkillAttackManager::Instance()->destroyInstance(*itSkillAttack);
				mSkillAttackList.erase(itSkillAttack++);
			}
			else
			{
				++itSkillAttack;
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::FreeAllSkillAttack()
	{
		list<SkillAttack*>::type::iterator itSkillAttack;
		for (itSkillAttack = mSkillAttackList.begin(); itSkillAttack != mSkillAttackList.end(); ++itSkillAttack)
		{
			SkillAttackManager::Instance()->destroyInstance(*itSkillAttack);
		}
		mSkillAttackList.clear();
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateLogicState(void)
	{
		if (true == mPositionDirty)
		{
			if (!mParentActor)
			{
				mSceneNodeMaster->setLocalPosition(mPosition);
				if (m_pShadowSceneNode)
				{
					mSceneNodeMaster->update();
					Vector3 masterNodePos = mSceneNodeMaster->getWorldPosition();
					masterNodePos.y = 0.0f;
					m_pShadowSceneNode->setWorldPosition(masterNodePos);

					m_pShadowSceneNode->update();
				}
				if (mpSelectEffect)
				{
					mSceneNodeMaster->update();
					mpSelectEffect->getEffectSceneNode()->setWorldPosition(mSceneNodeMaster->getWorldPosition());
					mpSelectEffect->getEffectSceneNode()->update();
				}
			}

			mPositionDirty = false;
		}
		if (true == mOrientationDirty)
		{
			if (!mParentActor)
			{
				mSceneNodeMaster->setLocalOrientation(mOrientation);
			}

			mOrientationDirty = false;
		}
		if (true == mScaleDirty)
		{
			Vector3 scale = mScale;
			if (m_isMirror)
			{
				scale.x = -Math::Abs(scale.x);
			}
			mSceneNodeMaster->setLocalScaling(scale * m_pTemplateData->OriginalScale);
			if (m_pShadowSceneNode)
			{
				Vector3 shadowScale = scale * m_pTemplateData->OriginalScale;
				shadowScale.abs();
				m_pShadowSceneNode->setLocalScaling(shadowScale);
				m_pShadowSceneNode->update();
			}
			if (mpSelectEffect)
			{
				mpSelectEffect->getEffectSceneNode()->setLocalScaling(scale * m_pTemplateData->OriginalScale);
				mpSelectEffect->getEffectSceneNode()->update();
			}
			mScaleDirty = false;
		}
		mSceneNodeMaster->update(true);
	}
	//-----------------------------------------------------------------------
	void ActorObject::setUpper(const String& upperRootName)
	{
		if (mSkeletonMaster && mSkeletonMaster->isLoaded())
			mSkeletonMaster->setUpper(upperRootName);
		m_UpperRootBoneName = upperRootName;
	}
	//-----------------------------------------------------------------------
	bool ActorObject::prepared()
	{
		return (mDelayProcessVector.empty() && mHasInited) || mbSingleThread;
	}
	//-----------------------------------------------------------------------
	void ActorObject::Update(ui32 delta)
	{
		ui32 time = delta;
		if (m_isPause) time = 0;
		if (mHasInited)
		{
			_UpdateObjectState(time);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateForUI(ui32 delta, const Matrix4& viewproj)
	{
		ui32 time = delta;

		time = (int)(time / ActorManager::Instance()->GetSpeedUp());

		if (m_isPause) time = 0;

		UpdateFromMatrix(time, viewproj);
		// draw effect and actor
		ui8 nStartIndex = SceneManager::Instance()->getRenderQueueIndex("Scene");
		ui8 nEndIndex = SceneManager::Instance()->getRenderQueueIndex("SkinnedMeshSpecial");

		for (ui32 i = nStartIndex; i <= nEndIndex; ++i)
		{
			RenderQueue* renderQueue = SceneManager::Instance()->getRenderQueueByIndex(i);
			LordAssert(renderQueue);
			renderQueue->renderQueue();
			renderQueue->beginRender();
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateFromMatrix(ui32 delta, const Matrix4& viewproj)
	{
		if (mHasInited)
			_UpdateObjectStateFromMat(delta, viewproj);
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateChildActors(ui32 delta)
	{
		if (m_isSkillHideActor)
		{
			return;
		}

		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
		{
			(*itActor)->Update(delta);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateChildMeshs(ui32 delta, const Matrix4* viewproj)
	{
		if (m_isSkillHideActor)
		{
			return;
		}

		map<String, Child_Mesh*>::type::iterator itMesh = mChildMeshs.begin();
		for (; itMesh != mChildMeshs.end(); ++itMesh)
		{
			itMesh->second->pModelEntity->update(viewproj);
			if (!m_isHide)
				itMesh->second->pModelEntity->updateRenderQueue(m_bTranslate ? ActorManager::m_TranslateRenderQueue : ActorManager::m_ActorRenderQueue);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateStaticMeshs(ui32 delta, const Matrix4* viewproj)
	{
		if (m_isSkillHideActor)
		{
			return;
		}

		vector<Static_Mesh>::type::iterator itMesh;
		multimap<String, Body_Part_Info*>::type::iterator it = mBodyParts.begin();
		for (; it != mBodyParts.end(); ++it)
		{
			Body_Part_Info* bp = it->second;
			bool hasMeshTexModifer = m_meshTexModify != nullptr && 
				bp->StaticMeshes.size() == 1 &&
				bp->pTemplateData->MasterName == m_meshTexModify->getMasterName() &&
				bp->pTemplateData->SlaveName == m_meshTexModify->getSlaveName();

			for (itMesh = bp->StaticMeshes.begin(); itMesh != bp->StaticMeshes.end(); ++itMesh)
			{
				ModelEntity* pme = itMesh->pEntity;
				if (pme)
				{
					pme->setNaturalColor(mNaturalColor);
					pme->setCustomColor(mCustomColor);
					pme->setAlphaColor(mAlphaColor);
					pme->update(viewproj);
					if (!m_isHide)
					{
						if (hasMeshTexModifer )
						{
							Renderable* prd = pme->getRenderables(0); // ->setTextureModify(m_meshTexModify->getModifyPtr());
							if (prd)
								prd->getMaterial()->setTextureModify(m_meshTexModify->getModifyPtr());
						}
						pme->updateRenderQueue(m_bTranslate ? ActorManager::m_TranslateRenderQueue : ActorManager::m_ActorRenderQueue);
					}
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateEffects(ui32 delta)
	{
		if (mAlphaColor.a >= 0.0f)
			UpdateEquipEffects(delta);

		UpdatePluginEffects(delta);
		if (m_pShadowEffect && !m_isSkillHideActor/* && !m_isActorFadeOut*/)
		{
			m_pShadowEffect->_update_sys(delta);
			if (!m_isHide)
				m_pShadowEffect->_updateRenderQueue();
		}
		if (mpSelectEffect && mAlphaColor.a >= 0.0f)
		{
			mpSelectEffect->_update_sys(delta);
			if (!m_isHide)
				mpSelectEffect->_updateRenderQueue();
		}
		RenderSkillEffect();
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateEquipEffects(ui32 delta)
	{
		if (m_isHide || m_isActorFadeOut || m_isSkillHideActor || m_isBackup)
			return;

		multimap<String, Body_Part_Info*>::type::iterator it;
		vector<Equip_Effect>::type::iterator itEffect;
		for (it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
		{
			for (itEffect = it->second->EquipEffects.begin(); itEffect != it->second->EquipEffects.end(); ++itEffect)
			{
				EffectSystem *pEffect = itEffect->pEffect;

				if (!pEffect->isPlaying())
				{
					pEffect->stop();
					pEffect->unprepare_sys();
					if (m_isUIActor)
					{
						pEffect->prepare_sys(true);
						pEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
					}
					else
					{
						pEffect->prepare_sys(true);
						pEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
					}

					pEffect->start();
				}

				pEffect->_update_sys((ui32)(itEffect->TimeScale * delta));
				if (!m_isHide)
					pEffect->_updateRenderQueue();
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdatePluginEffects(ui32 delta)
	{
		map<String, Actor_Effect>::type::iterator it = mPluginEffects.begin();
		for (; it != mPluginEffects.end();)
		{
			if (!it->second.isOrientFollowParentNode)
			{
				it->second.pEffectNode->setWorldPosition(it->second.pParentNode->getWorldPosition());
				it->second.pEffectNode->update();
			}

			if (!it->second.pEffect->isPlaying())
			{
				if (it->second.times < 0)
				{
					it->second.pEffect->stop();
					it->second.pEffect->start();
				}
				else
				{
					--it->second.times;
					if (0 == it->second.times)
					{
						it->second.pEffect->unprepare_sys();
						EffectSystemManager::Instance()->destroyEffectSystem(it->second.pEffect);
						//LordDelete (it->second.pEffect);
						it->second.pEffect = NULL;
					}
					else
					{
						it->second.pEffect->stop();
						it->second.pEffect->start();
					}
				}
			}

			if (it->second.pEffect)
			{
				if (it->second.bFadeOut)
				{
					it->second.Weight -= (float)delta*0.001f / EFFECT_FADE_TIME;
					if (it->second.Weight < 0.f)
					{
						it->second.pEffect->unprepare_sys();
						EffectSystemManager::Instance()->destroyEffectSystem(it->second.pEffect);
						//LordDelete (it->second.pEffect);
						it->second.pEffect = NULL;
					}
				}
			}

			if (it->second.pEffect)
			{
				it->second.pEffect->_update_sys(ui32(it->second.TimeScale * delta));
				if (!m_isHide)
					it->second.pEffect->_updateRenderQueue();
				++it;
			}
			else
			{
				it->second.pEffectNode->getParent()->destroyChild(it->second.pEffectNode);
				mPluginEffects.erase(it++);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateSkillAttacks(ui32 delta)
	{
		if (mAlphaColor.a <= 0.0f)
			return;

		list<SkillAttack*>::type::iterator itSkillAttack;
		for (itSkillAttack = mSkillAttackList.begin(); itSkillAttack != mSkillAttackList.end();)
		{
			if ((*itSkillAttack)->IsStoped() == true)
			{
				SkillAttackManager::Instance()->destroyInstance(*itSkillAttack);
				mSkillAttackList.erase(itSkillAttack++);
				//LordLogDebug("UpdateSkillAttacks ");
			}
			else
			{
				//LordLogDebug("UpdateSkillAttacks %d", delta);
				(*itSkillAttack)->update(delta);
				++itSkillAttack;
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateColor(ui32 delta)
	{
		if (mColorChangeState.smoothTime <= -1000 && mColorChangeState.resumeTime <= -1000)
			return;

		if (mColorChangeState.smoothTime > 0)
		{
			float rate = (float)delta / (float)mColorChangeState.smoothTime;
			if (rate <= 1.0f && rate >= 0)
				mNaturalColor = mColorChangeState.color * rate + mNaturalColor * (1 - rate);
			mColorChangeState.smoothTime -= delta;
		}
		else if (mColorChangeState.durationTime > 0)
		{
			mColorChangeState.durationTime -= delta;
		}
		else if (mColorChangeState.resumeTime > 0)
		{
			float rate = (float)delta / (float)mColorChangeState.resumeTime;
			if (rate <= 1.0f && rate >= 0)
				mNaturalColor = Color::WHITE * rate + mNaturalColor * (1 - rate);
			mColorChangeState.resumeTime -= delta;
		}
		else
		{
			mNaturalColor = Color::WHITE;
			mColorChangeState.smoothTime = -1000;
			mColorChangeState.durationTime = -1000;
			mColorChangeState.resumeTime = -1000;
		}

		mAlphaColor.a = m_fAlpha * m_pTemplateData->mfAlpha;

		for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
		{
			(*it)->setNaturalColor(mNaturalColor);
			(*it)->setAlphaColor(mAlphaColor);
		}
	}
	//-----------------------------------------------------------------------
	bool ActorObject::_UpdateObjectState(ui32 delta)
	{
		if (m_isBlockMan && (!m_isBlockManVisible))
			return false;
		
		// add by ajohn. query Actor is visible.
		m_isFrustumVisible = false;
		if (!m_isUIActor)
		{
			UpdateLogicState();

			if (!m_isBackup)
			{
				if (m_CameraAnimHide)
					return false;
				m_curDelta += delta;
				ui32 tempDelta = (ui32)(m_curDelta * mTimeScale);
				ui32 baseScaleDelta = (ui32)(m_curDelta * mBaseScale);

				if (tempDelta)
				{
					mSkillSystem.Update(tempDelta);
					UpdateSkillAttacks(tempDelta);
					m_curDelta = 0;
				}
				else
				{
					mSkillSystem.Update(0);
					UpdateSkillAttacks(0);
				}

				UpdateSkillAbout(baseScaleDelta);
				if (m_isActorFadeOut) UpdateActorFadeOut(baseScaleDelta);

				CalcThread* pThread = Root::Instance()->getCalcThread();
				if ((SceneManager::Instance()->getMainCamera()->getFrustum().intersect(getActorAABB()) || m_needUpdate) && mAlphaColor.a >= 0.0f)
				{
					m_isFrustumVisible = true;
					if (!m_isSkillHideActor)
					{
						RenderQueue* pRenderQueue = m_bTranslate ? ActorManager::m_TranslateRenderQueue : ActorManager::m_ActorRenderQueue;
						if (pThread->IsRunning())
						{
							if (m_pAnimBlender)
								m_pAnimBlender->frameMoveForCalcThread(baseScaleDelta);

							for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
							{
								SkinedEntity* pEntity = *it;
								pEntity->updateForCalcThread();
								if (!m_isHide)
									pEntity->updateRenderQueue(pRenderQueue);
							}
							UpdateBoneNodes();
						}
						else
						{
							if (m_pAnimBlender)
								m_pAnimBlender->frameMove(baseScaleDelta);
							UpdateBoneNodes();

							for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
							{
								SkinedEntity* pEntity = *it;
								pEntity->update();
								if (!m_isHide)
									pEntity->updateRenderQueue(pRenderQueue);
							}//for
						}//Thread
					}//if frustum

					 // 由actormanager管理update了所以不需要自己搞了
					 //UpdateChildActors(tempDelta);

					UpdateChildMeshs(baseScaleDelta);
					UpdateStaticMeshs(baseScaleDelta);
					UpdateEffects(baseScaleDelta);
					UpdateColor(baseScaleDelta);
				}
				else
				{
					if (pThread->IsRunning())
					{
						if (m_pAnimBlender)
							m_pAnimBlender->excludeCalcThread();

						for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
						{
							(*it)->excludeCalcThread();
						}
					}
				}// else frustum
			}//if (!m_isHide)
		}//if (!m_isUIActor)

		m_isBlockManVisible = false;
		return m_isFrustumVisible;
	}
	//-----------------------------------------------------------------------
	bool ActorObject::_UpdateObjectStateFromMat(ui32 delta, const Matrix4& viewproj)
	{
		UpdateLogicState();

		if (!m_isHide && !m_isBackup)
		{
			if (m_CameraAnimHide)
				return false;

			mSkillSystem.Update(delta);

			if (!m_isSkillHideActor)
			{
				if (m_pAnimBlender)	m_pAnimBlender->frameMove(delta);
				UpdateBoneNodes();

				RenderQueue* pRenderQueue = m_bTranslate ? ActorManager::m_TranslateRenderQueue : ActorManager::m_ActorRenderQueue;
				for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
				{
					SkinedEntity* pEntity = *it;
					pEntity->update(&viewproj);
					if (!m_isHide)
						pEntity->updateRenderQueue(pRenderQueue);
				}
				set<ActorObject*>::type::iterator itActor;
				for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
				{
					(*itActor)->UpdateForUI(delta, viewproj);
				}
				UpdateChildMeshs(delta, &viewproj);
				UpdateStaticMeshs(delta, &viewproj);
				UpdateEffects(delta);
				UpdateColor(delta);
			}
		}
		return true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::RenderPluginEffect()
	{
		if (m_isHide)
			return;
		for (LORD::map<LORD::String, Actor_Effect>::type::iterator it = mPluginEffects.begin(); it != mPluginEffects.end(); ++it)
		{
			if (it->second.pEffect) it->second.pEffect->_updateRenderQueue();
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::RenderSocketEffect()
	{
		if (m_isHide)
			return;
		for (LORD::multimap<LORD::String, Body_Part_Info*>::type::iterator it = mBodyParts.begin(); it != mBodyParts.end(); ++it)
		{
			Body_Part_Info* bodyPart = it->second;
			for (LORD::vector<Equip_Effect>::type::iterator iter = bodyPart->EquipEffects.begin(); iter != bodyPart->EquipEffects.end(); ++iter)
			{
				if (iter->pEffect)
					iter->pEffect->_updateRenderQueue();
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::RenderSkillEffect()
	{
		mSkillSystem.RenderSkillEffect();
	}
	//-----------------------------------------------------------------------
	bool ActorObject::prepare_io()
	{
		mSkeletonMaster->prepare();

		for (set<Animation*>::type::iterator it = mAnimtions.begin(); it != mAnimtions.end(); ++it)
		{
			Animation* pAnim = *it;
			pAnim->prepare();
		}

		return true;
	}
	//-----------------------------------------------------------------------
	bool ActorObject::InitAfterIOprepared(int& CurStep)
	{
		if (CurStep > 0 && !ActorManager::Instance()->ValidActor(this))
		{
			vector<ActorDelayedProcess*>::type::iterator it = mDelayProcessVector.begin();
			for (; it != mDelayProcessVector.end(); ++it)
			{
				LordDelete(*it);
			}
			mDelayProcessVector.clear();

			if (m_pShadowEffect)
			{
				EffectSystemManager::Instance()->destroyEffectSystem(m_pShadowEffect);
				GetActorRootSceneNode()->destroyChild(m_pShadowSceneNode);
				m_pShadowSceneNode = NULL;
				m_pShadowEffect = NULL;
			}

			return true;
		}

		switch (CurStep)
		{
		case 0:
		{
			if (!mSkeletonMaster->load())
			{
				LordLogError("Skeleton: Skeleton file isn't exist. [%s]", m_pTemplateData->SkeletonName.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "Skeleton file isn't exist. [%s]", m_pTemplateData->SkeletonName.c_str());
				MessageBox(0, buf, "Error", 0);
#endif
				SkeletonManager::Instance()->releaseResource(mSkeletonMaster);
				mSkeletonMaster = NULL;
				return true;
			}
			ActorTemplateData *pTemplate = const_cast<ActorTemplateData*>(m_pTemplateData);
			pTemplate->InitBySkeleton(mSkeletonMaster);
			m_pAnimBlender = LordNew AnimBlender(mSkeletonMaster);
			initBoneNodes();

			for (set<Animation*>::type::iterator it = mAnimtions.begin(); it != mAnimtions.end();)
			{
				Animation* pAnim = *it;
				if (pAnim->load())
				{
					++it;
				}
				else
				{
					LordLogError("Animation: Animation file isn't exist. [%s]", pAnim->getName().c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "Animation file isn't exist. [%s]", pAnim->getName().c_str());
					MessageBox(0, buf, "Error", 0);
#endif
					mAnimtions.erase(it++);
					AnimManager::Instance()->releaseResource(pAnim);
				}
			}

			if (m_pShadowEffect)
			{
				if (m_isUIActor)
				{
					m_pShadowEffect->prepare_sys(true);
					m_pShadowEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
				}
				else
				{
					m_pShadowEffect->prepare_sys(true);
					m_pShadowEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
				}
				m_pShadowEffect->start();
			}


			mSkillSystem.Free();
			mSkillSystem.Init(m_pTemplateData, this);

			mHasInited = true;
		}
		break;
		case 1:
		{
			if (!mSkeletonMaster->load())
			{
				LordLogError("Skeleton: Skeleton file isn't exist. [%s]", m_pTemplateData->SkeletonName.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "Skeleton file isn't exist. [%s]", m_pTemplateData->SkeletonName.c_str());
				MessageBox(0, buf, "Error", 0);
#endif
				SkeletonManager::Instance()->releaseResource(mSkeletonMaster);
				mSkeletonMaster = NULL;
				return true;
			}
			ActorTemplateData *pTemplate = const_cast<ActorTemplateData*>(m_pTemplateData);
			pTemplate->InitBySkeleton(mSkeletonMaster);
			m_pAnimBlender = LordNew AnimBlender(mSkeletonMaster);
			LordAssert(m_pAnimBlender);
			initBoneNodes();

			if (m_UpperRootBoneName != StringUtil::BLANK)
				mSkeletonMaster->setUpper(m_UpperRootBoneName);

			CurStep = 2;
		}
		return false;
		case 2:
		{
			for (set<Animation*>::type::iterator it = mAnimtions.begin(); it != mAnimtions.end(); )
			{
				Animation* pAnim = *it;
				if (pAnim->load())
				{
					++it;
				}
				else
				{
					LordLogError("Animation: Animation file isn't exist. [%s]", pAnim->getName().c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "Animation file isn't exist. [%s]", pAnim->getName().c_str());
					MessageBox(0, buf, "Error", 0);
#endif
					mAnimtions.erase(it++);
					AnimManager::Instance()->releaseResource(pAnim);
				}
			}
			CurStep = 3;
		}
		return false;
		case 3:
		{
			if (m_pShadowEffect)
			{
				if (m_isUIActor)
				{
					m_pShadowEffect->prepare_sys(true);
					m_pShadowEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
				}
				else
				{
					m_pShadowEffect->prepare_sys(true);
					m_pShadowEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
				}
				m_pShadowEffect->start();
			}
			CurStep = 4;
		}
		return false;
		case 4:
		{
			mSkillSystem.Free();
			mSkillSystem.Init(m_pTemplateData, this);
			mHasInited = true;

			vector<ActorDelayedProcess*>::type::iterator it = mDelayProcessVector.begin();
			for (; it != mDelayProcessVector.end(); ++it)
			{
				(*it)->Execute(this);
				LordDelete(*it);
			}
			mDelayProcessVector.clear();

			CurStep = 5;
		}
		return true;
		}

		return true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateScaleShow(void)
	{
		mScaleDirty = true;
	}
	//-----------------------------------------------------------------------
	void ActorObject::SetBrightness(const Color& color)
	{
		for (auto& part : mBodyParts)
		{
			Body_Part_Info* body = part.second;
			if (!body)
				continue;
			for (auto& model : body->StaticMeshes)
			{
				ModelEntity* pentity = model.pEntity;
				if (pentity)
					pentity->setBrightness(color);
			}
			for (auto& skin : body->SkinMeshes)
			{
				SkinedEntity* pentity = skin.pEntity;
				if (pentity)
					pentity->setBrightness(color);
			}
		}
	}
	//-----------------------------------------------------------------------
	bool ActorObject::UpHorse(ActorObject *pActor)
	{
		if (!mHorse)
		{
			if (this->AttachChildActor("p_horse", pActor))
			{
				mHorse = pActor;
				return true;
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------
	void ActorObject::DownHorse(void)
	{
		if (mHorse)
		{
			this->DetachChildActor(mHorse);
			return;
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateSkillAbout(ui32 delta)
	{
		UpdateHitFly(delta);
		UpdateSlowDown(delta);
		UpdateCameraPush((float)(delta * 0.001f));
		UpdateSkillActorScale((float)(delta * 0.001f));
	}
	//---------------------------------------------------------------------
	SceneNode* ActorObject::getBoneNode(Bone* pBone)
	{
		if (!pBone)
			return NULL;

		bool bAnimBlender = m_pAnimBlender && m_pAnimBlender->getAnimState();
		SceneNode* boneNode = mBoneNodeVector[pBone->getId()];
		if (boneNode == NULL)
		{
			if (pBone->isDummy())
			{
				int parentBoneId = pBone->getParentId();
				SceneNode* parentNode = mBoneNodeVector[parentBoneId]; // 获取挂点父骨骼节点
				if (parentNode == NULL)
				{
					Bone* pParent = pBone->getParent();
					parentNode = mSceneNodeMaster->createChild();   // 创建骨骼节点
					mBoneNodeVector[parentBoneId] = parentNode;
					if (bAnimBlender)							  // 骨骼点需要更新当前动画位置
					{
						parentNode->setLocalOrientation(m_pAnimBlender->getAnimOrientation(parentBoneId) * pParent->getRotation());
						parentNode->setLocalPosition(m_pAnimBlender->getAnimOrientation(parentBoneId) * pParent->getPosition() + m_pAnimBlender->getAnimPosition(parentBoneId));
					}
					else
					{
						parentNode->setLocalPosition(pParent->getPosition());
						parentNode->setLocalOrientation(pParent->getRotation());
					}
					parentNode->update();
				}
				boneNode = parentNode->createChild();				  // 创建挂点节点
				boneNode->setLocalPosition(pBone->getPosition());
				boneNode->setLocalOrientation(pBone->getRotation());
				boneNode->update();
			}
			else
			{
				boneNode = mSceneNodeMaster->createChild();		  // 如果是骨骼点，则创建骨骼节点
				if (bAnimBlender)								  // 骨骼点需要更新当前动画位置
				{
					uint boneId = pBone->getId();
					boneNode->setLocalOrientation(m_pAnimBlender->getAnimOrientation(boneId) * pBone->getRotation());
					boneNode->setLocalPosition(m_pAnimBlender->getAnimOrientation(boneId) * pBone->getPosition() + m_pAnimBlender->getAnimPosition(boneId));
				}
				boneNode->update();
			}

			mBoneNodeVector[pBone->getId()] = boneNode;
		}
		return boneNode;
	}
	//-----------------------------------------------------------------------
	void ActorObject::initBoneNodes()
	{
		if (mSkeletonMaster)
		{
			size_t size = sizeof(SceneNode)*mSkeletonMaster->getTotalCount();
			mBoneNodeVector = static_cast<SceneNode**>(LordMalloc(size));
			memset(mBoneNodeVector, 0, size);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateBoneNodes()
	{
		// update bone nodes
		if (!mSkeletonMaster)
			return;

		Matrix4 matWorld;
		Quaternion orientation;
		Vector3 position;

		const Skeleton::BoneList& bones = mSkeletonMaster->getBones();
		if (m_pAnimBlender && m_pAnimBlender->getAnimState())
		{
			for (size_t i = 0; i<mSkeletonMaster->getBoneCount(); ++i)
			{
				SceneNode* pBoneNode = mBoneNodeVector[i];
				if (!pBoneNode)
					continue;

				Bone* pBone = bones[i];
				assert(!pBone->isDummy());
				pBoneNode->setLocalOrientation(m_pAnimBlender->getAnimOrientation(pBone->getId()) * pBone->getRotation());
				pBoneNode->setLocalPosition(m_pAnimBlender->getAnimOrientation(pBone->getId()) * pBone->getPosition() + m_pAnimBlender->getAnimPosition(pBone->getId()));
				pBoneNode->update(true);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::UpdateWeaponActor(ui32 delta)
	{
		UpdateStaticMeshs(delta);
		UpdateEquipEffects(delta);
		RenderSocketEffect();
	}
	//-----------------------------------------------------------------------
	LORD::Box ActorObject::getActorAABB()
	{
#ifdef RENDERACTORAABB
		if (!m_BoxObject)
		{
			QueryObjectManager* pManager = QueryObjectManager::Instance();
			if (!pManager)
			{
				pManager = LordNew QueryObjectManager();
			}
			m_BoxObject = pManager->createQueryObject(mActorNameID + "query");
			m_BoxObject->setTestDepth(true);
			m_BoxObject->setWriteDepth(true);
			m_BoxObject->setQueryFlag(0);
			SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
			m_BoxObject->attachTo(pNode);
		}
		m_BoxObject->clear();
#endif

		Box actorAABB;
		bool hadMesh = false;
		if (!mEntitySet.empty())   //skin包围盒
		{
			hadMesh = true;
			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				actorAABB.unionBox((*it)->getWorldAABB());
			}
		}
		else                     //mesh包围盒
		{
			vector<Static_Mesh>::type::iterator itMesh;
			multimap<String, Body_Part_Info*>::type::iterator it = mBodyParts.begin();
			for (; it != mBodyParts.end(); ++it)
			{
				for (itMesh = it->second->StaticMeshes.begin(); itMesh != it->second->StaticMeshes.end(); ++itMesh)
				{
					if (itMesh->pEntity)
					{
						actorAABB.unionBox(itMesh->pEntity->getWorldAABB());
						hadMesh = true;
					}
				}
			}
		}

		if (!hadMesh)    //默认
		{
			actorAABB = Box(Vector3(-0.3f, 0.0f, -0.3f), Vector3(0.3f, 1.5f, 0.3f));
		}
		actorAABB = actorAABB.transform(mSceneNodeMaster->getWorldMatrix());

#ifdef RENDERACTORAABB
		m_BoxObject->createAABBFrame(actorAABB, Color::WHITE);
#endif
		return actorAABB;
	}
	//-----------------------------------------------------------------------
	LORD::Box ActorObject::getLocalAABB()
	{
		Box actorAABB;
		bool hadMesh = false;
		if (!mEntitySet.empty())   //skin包围盒
		{
			hadMesh = true;
			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				actorAABB.unionBox((*it)->getLocalAABB());
			}
		}
		else                     //mesh包围盒
		{
			vector<Static_Mesh>::type::iterator itMesh;
			multimap<String, Body_Part_Info*>::type::iterator it = mBodyParts.begin();
			for (; it != mBodyParts.end(); ++it)
			{
				for (itMesh = it->second->StaticMeshes.begin(); itMesh != it->second->StaticMeshes.end(); ++itMesh)
				{
					if (itMesh->pEntity)
					{
						actorAABB.unionBox(itMesh->pEntity->getLocalAABB());
						hadMesh = true;
					}
				}
			}
		}

		if (!hadMesh)    //默认
		{
			actorAABB = Box(Vector3(-0.3f, 0.0f, -0.3f), Vector3(0.3f, 1.5f, 0.3f));
		}

		return actorAABB;
	}
	//-----------------------------------------------------------------------
	list<SkillAttack*>::type ActorObject::GetAttackList(void) const
	{
		return mSkillAttackList;
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSlowDown(const ActorTemplateData::SlowDown* pData)
	{
		if (m_slowDown.LastTime == 0.0 && m_slowDown.EqualizeTime == 0.)
		{
			//SetTimeScale(pData->timeScale);
			m_slowDown.scale = pData->timeScale;
			m_slowDown.beginTime = 0.f;
			m_slowDown.LastTime = pData->durationTime;
			m_slowDown.EqualizeTime = pData->equalizeTime;
			m_slowDown.EqualizeTimeScale = (pData->equalizeTime + pData->durationTime * (1 - pData->timeScale)) / pData->equalizeTime;
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSlowDown(float beginTime, float scale, float durationTime, float equalizeTime)
	{
		if (m_slowDown.LastTime == 0.0 && m_slowDown.EqualizeTime == 0.)
		{
			//SetTimeScale(scale);
			m_slowDown.beginTime = beginTime;
			m_slowDown.scale = scale;
			m_slowDown.LastTime = durationTime;
			m_slowDown.EqualizeTime = equalizeTime;
			m_slowDown.EqualizeTimeScale = (equalizeTime + durationTime * (1 - scale)) / equalizeTime;
		}
	}

	//-----------------------------------------------------------------------
	void ActorObject::UpdateSlowDown(ui32 delta)
	{
		//更新顿帧
		if (m_slowDown.scale != 1.0f)
		{
			float result = 1.0f;
			float sl_delta = delta * 0.001f;
			m_slowDown.beginTime -= sl_delta;
			if (m_slowDown.beginTime <= 0.f)
			{
				//SetTimeScale(m_slowDown.scale);
				result = m_slowDown.scale;
				m_slowDown.LastTime -= sl_delta;
				if (m_slowDown.LastTime <= 0.f)
				{
					m_slowDown.LastTime = 0.f;
					//SetTimeScale(m_slowDown.EqualizeTimeScale);
					result = m_slowDown.EqualizeTimeScale;
					m_slowDown.EqualizeTime -= sl_delta;
					if (m_slowDown.EqualizeTime <= 0.f)
					{
						m_slowDown.scale = 1.0f;
						m_slowDown.beginTime = -1.0f;
						m_slowDown.EqualizeTime = 0.f;
						//SetTimeScale(1.0);
						result = 1.0f;
					}
				}
			}
			SetCurrentTimeScale(result);
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddSlowDownToTargetActors(const ActorTemplateData::SlowDown* pData)
	{
		vector<Attack_Event_Info>::type::iterator it;
		for (it = mTargetActors.begin(); it != mTargetActors.end(); ++it)
		{
			ActorObject* pActor = it->pTarget;
			if (pActor)
				pActor->AddSlowDown(pData);
		}
	}
	//-----------------------------------------------------------------------
	AnimBlender* ActorObject::getAnimationBlender() const
	{
		return m_pAnimBlender;
	}
	//-----------------------------------------------------------------------
	void ActorObject::AddHitFly(const ActorTemplateData::HitFly* pData)
	{
		if (m_HitFly.duration > 0.f)
			return;

		m_HitFly.duration = pData->durationTime * 0.5f;
		m_HitFly.high = pData->high;
		m_HitFly.curentTime = 0.f;
		m_HitFly.speed = Math::Sqrt(pData->high * 2.f * 9.8f);
	}

	void ActorObject::UpdateHitFly(ui32 delta)
	{
		if (m_HitFly.duration < 0.f)
			return;

		m_HitFly.curentTime += (float)(delta * 0.001f);
		Vector3 pos = mSceneNodeMaster->getWorldPosition();
		pos.y = m_HitFly.speed * m_HitFly.curentTime - m_HitFly.curentTime * m_HitFly.curentTime * 9.8f * 0.5f;

		if (pos.y < 0)
		{
			pos.y = 0.0f;
			m_HitFly.duration = -1.0f;
		}
		mSceneNodeMaster->setWorldPosition(pos);
		mSceneNodeMaster->update();
		if (m_pShadowSceneNode)
		{
			m_pShadowSceneNode->setWorldPosition(Vector3(pos.x, 0.0f, pos.z));
			m_pShadowSceneNode->update();
		}
	}

	void ActorObject::AddCameraPush(const ActorTemplateData::CameraPush* pData)
	{
		if (!ActorManager::Instance()->GetActorsSkillCameraEnable())
			return;

		if (m_CameraPush.durationTime > 0.f)
			return;

		if (pData->isInversion)
		{
			m_CameraPush.durationTime = pData->durationTime;
			m_CameraPush.closeTime = pData->farawayTime;
			m_CameraPush.farawayTime = pData->closeTime;
			m_CameraPush.scale = pData->scale;
			m_CameraPush.stopTime = pData->durationTime - pData->farawayTime - pData->closeTime;
			m_CameraPush.closeSpeed = -pData->scale / pData->farawayTime * 0.033f / (SceneManager::Instance()->getMainCamera()->getFov() * 10);
			m_CameraPush.farawaySpeed = -pData->scale / pData->closeTime * 0.033f / (SceneManager::Instance()->getMainCamera()->getFov() * 10);
		}
		else
		{
			m_CameraPush.durationTime = pData->durationTime;
			m_CameraPush.closeTime = pData->closeTime;
			m_CameraPush.farawayTime = pData->farawayTime;
			m_CameraPush.scale = pData->scale;
			m_CameraPush.stopTime = pData->durationTime - pData->farawayTime - pData->closeTime;
			m_CameraPush.closeSpeed = pData->scale / pData->closeTime * 0.033f / (SceneManager::Instance()->getMainCamera()->getFov() * 10);
			m_CameraPush.farawaySpeed = pData->scale / pData->farawayTime * 0.033f / (SceneManager::Instance()->getMainCamera()->getFov() * 10);
		}
	}

	void ActorObject::UpdateCameraPush(float delta)
	{
		if (m_CameraPush.durationTime <= 0.0f)
			return;

		mCameraPushDelta += delta;
		if (mCameraPushDelta < 0.033f)
		{
			return;
		}
		else
		{
			mCameraPushDelta -= 0.033f;
		}

		//获取实例中摄像机位子
		CameraMain* pCamera = static_cast<CameraMain*>(SceneManager::Instance()->getMainCamera());

		// 超时清除
		if (m_CameraPush.farawayTime <= 0.0f)
		{
			m_CameraPush.durationTime = 0.0f;
			m_CameraPush.curentScale = 0.0f;
			mCameraPushDelta = 0.f;
		}
		else
		{
			// 计算偏移
			Vector3 deltapos;
			Vector3 oldcamerapos = pCamera->getPosition();
			if (m_CameraPush.closeTime > 0.0f)
			{
				deltapos = pCamera->getDirection()* m_CameraPush.closeSpeed;
				pCamera->setPosition_forLord(oldcamerapos + deltapos);
				m_CameraPush.curentScale += m_CameraPush.closeSpeed;
				m_CameraPush.closeTime -= 0.033f;
			}
			else if (m_CameraPush.stopTime > 0.0f)
			{
				m_CameraPush.stopTime -= 0.033f;
			}
			else if (m_CameraPush.farawayTime > 0.0f)
			{
				m_CameraPush.farawayTime -= 0.033f;
				if (m_CameraPush.farawayTime > 0.0f)
				{
					deltapos = pCamera->getDirection()* m_CameraPush.farawaySpeed;
					pCamera->setPosition_forLord(oldcamerapos - deltapos);
					m_CameraPush.curentScale -= m_CameraPush.farawaySpeed;
				}
				else
				{
					deltapos = pCamera->getDirection()* m_CameraPush.curentScale;
					pCamera->setPosition_forLord(oldcamerapos - deltapos);
				}
			}
		}
	}

	const map<String, ActorTemplateData::Skill*>::type& ActorObject::GatSkillMap() const
	{
		return m_pTemplateData->SkillsMap;
	}

	bool ActorObject::IsSkillExist(const String& name)
	{
		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(name);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			return true;
		}

		return false;
	}

	const set<SkinedEntity*>::type ActorObject::GetSkinEntitySet(void)
	{
		return mEntitySet;
	}

	void ActorObject::AttachSelectEffect(const String& effectName/*const String &SocketName*/)
	{
		//如果已经有选中特效，直接返回
		if (mpSelectEffect)
			return;

		mpSelectEffect = ActorManager::Instance()->GetSelectEffect(GetNextEffectName(), effectName);

		if (!mpSelectEffect->isPlaying())
		{
			Vector3 scale = mScale;
			if (m_isMirror)
			{
				scale.x = -Math::Abs(scale.x);
			}
			mpSelectEffect->getEffectSceneNode()->setWorldPosition(mSceneNodeMaster->getWorldPosition());
			mpSelectEffect->getEffectSceneNode()->setLocalScaling(scale * m_pTemplateData->OriginalScale);
			mpSelectEffect->getEffectSceneNode()->update();
			if (m_isUIActor)
			{
				mpSelectEffect->prepare_sys(true);
				mpSelectEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());
			}
			else
			{
				mpSelectEffect->prepare_sys(true);
				mpSelectEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
			}
			mpSelectEffect->start();
		}
	}

	void ActorObject::DetachSelectEffect()
	{
		//如果没挂直接返回
		if (!mpSelectEffect)
			return;

		mpSelectEffect->unprepare_sys();
		mpSelectEffect = NULL;
	}

	void ActorObject::AddSkillActorScale(const ActorTemplateData::ActorScale* pData)
	{
		if (m_SkillActorScale.scalling)
			return;

		m_SkillActorScale.durationTime = pData->durationTime;
		m_SkillActorScale.beginScale = pData->beginScale;
		m_SkillActorScale.endScale = pData->endScale;
		m_SkillActorScale.scaleSpeed = (pData->endScale - pData->beginScale) / pData->durationTime;
		m_SkillActorScale.scalling = true;
	}

	void ActorObject::UpdateSkillActorScale(float delta)
	{
		if (m_SkillActorScale.scalling)
		{
			m_SkillActorScale.curentTime += delta;
			if (m_SkillActorScale.curentTime < m_SkillActorScale.durationTime)
			{
				float cs = m_SkillActorScale.beginScale + m_SkillActorScale.scaleSpeed * m_SkillActorScale.curentTime;
				SetScale(LORD::Vector3(cs, cs, cs));
			}
			else
			{
				m_SkillActorScale.curentTime = 0.0f;
				SetScale(LORD::Vector3(m_SkillActorScale.endScale, m_SkillActorScale.endScale, m_SkillActorScale.endScale));
				m_SkillActorScale.beginScale = m_SkillActorScale.endScale;
				m_SkillActorScale.scalling = false;
			}
		}
	}

	//-----------------------------------------------------------------------
	void ActorObject::UpdateActorFadeOut(ui32 delta)
	{
		if (mAlphaColor.a < 0.0f)
		{
			m_isActorFadeOut = false;
		}
		else
		{
			mAlphaColor.a -= (float)delta*m_FadeOutScale / ACTOR_FADEOUT_TIME;

			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				SkinedEntity* pEntity = *it;
				if (pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
				{
					pEntity->setAlphaColor(mAlphaColor);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ActorObject::StartActorFadeOut(float TimeScale)
	{
		m_isActorFadeOut = true;
		m_FadeOutScale = TimeScale;
		mAlphaColor.a = 1.0f;

		CreateActorFadeEntity();

		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
		{
			(*itActor)->StartActorFadeOut(TimeScale);
		}
	}


	void ActorObject::refleshActorAlpha()
	{
		if (m_pTemplateData->mfAlpha < 1.0f)
		{
			ReserveActorEntity();
			SetActorTranslateAlpha(m_fAlpha);
		}
	}

	void ActorObject::SetActorTranslateAlpha(float alpha)
	{
		assert(alpha >= 0);
		m_fAlpha = alpha;
		alpha = m_fAlpha * m_pTemplateData->mfAlpha;
		mAlphaColor.a = alpha;
		if (mHasInited)
		{
			if (alpha != 1.0f)
			{
				CreateActorFadeEntity();
			}
			else
			{
				ReserveActorEntity();
			}

			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				SkinedEntity* pEntity = *it;
				if (pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
				{
					pEntity->setAlphaColor(mAlphaColor);
				}
			}

			multimap<String, Body_Part_Info*>::type::iterator itBody;
			vector<Static_Mesh>::type::iterator itStMesh;
			for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
			{
				for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
				{
					ModelEntity* pMEntity = itStMesh->pEntity;
					if (pMEntity)
					{
						if (pMEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
						{
							pMEntity->setAlphaColor(mAlphaColor);
						}
					}
				}
			}
		}

		set<ActorObject*>::type::iterator itActor;
		for (itActor = mChildActors.begin(); itActor != mChildActors.end(); ++itActor)
		{
			(*itActor)->SetActorTranslateAlpha(alpha);
		}
	}

	void ActorObject::CreateActorFadeEntity()
	{
		if (m_bTranslate)
			return;

		m_bTranslate = true;

		set<SkinedEntity*>::type tempEmtitySet;
		multimap<String, Body_Part_Info*>::type::iterator itBody;
		vector<Skin_Mesh>::type::iterator itMesh;
		bool bfind;

		SkinedEntity* pEntity;

		Material* pMaterial_depth = ActorManager::Instance()->GetSkinMaterial_depth();
		Material* pMaterial_transparent = ActorManager::Instance()->GetSkinMaterial_transparent();

		for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end();)
		{
			pEntity = *it;
			LordAssert(pEntity);
			if (pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR)
			{
				Mesh* pMesh = pEntity->getMesh();
				pMesh->load();
				SkinedEntity* tempEntity_Z = LordNew SkinedEntity;
				tempEntity_Z->attachTo(pEntity->getSceneNode());
				tempEntity_Z->setSkeleton(mSkeletonMaster);
				tempEntity_Z->setMesh(pMesh);
				tempEntity_Z->setMaterial(pMaterial_depth);
				tempEntity_Z->setRenderType(ModelEntity::RT_FOR_ACTOR_WRITEDEPTH);
				tempEntity_Z->setNaturalColor(mNaturalColor);
				tempEntity_Z->setCustomColor(mCustomColor);
				tempEntity_Z->setAlphaColor(mAlphaColor);
				tempEntity_Z->setAnimBlender(m_pAnimBlender);
				tempEntity_Z->setAmbientColor(mCurentAmbientColor);
				tempEntity_Z->createRenderable(m_isUIActor);
				tempEmtitySet.insert(tempEntity_Z);

				SkinedEntity* tempEntity = LordNew SkinedEntity;
				tempEntity->attachTo(pEntity->getSceneNode());
				tempEntity->setSkeleton(mSkeletonMaster);
				tempEntity->setMesh(pMesh);
				tempEntity->setMaterial(pMaterial_transparent);
				tempEntity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
				tempEntity->setNaturalColor(mNaturalColor);
				tempEntity->setCustomColor(mCustomColor);
				tempEntity->setAlphaColor(mAlphaColor);
				tempEntity->setAnimBlender(m_pAnimBlender);
				tempEntity->setAmbientColor(mCurentAmbientColor);
				tempEntity->createRenderable(m_isUIActor);
				tempEmtitySet.insert(tempEntity);

				bfind = false;
				for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
				{
					for (itMesh = itBody->second->SkinMeshes.begin(); itMesh != itBody->second->SkinMeshes.end(); ++itMesh)
					{
						if (itMesh->pEntity == pEntity)
						{
							itMesh->pEntity = tempEntity;
							itMesh->pEntity_WriteZ = tempEntity_Z;
							bfind = true;
							break;
						}
					}
					if (bfind)
						break;
				}

				tempEntity->prepareTexture(mbSingleThread, __FUNCTION__);

				LordDelete pEntity;
				mEntitySet.erase(it++);
			}
		}

		for (set<SkinedEntity*>::type::iterator it = tempEmtitySet.begin(); it != tempEmtitySet.end(); ++it)
		{
			mEntitySet.insert(*it);
		}
		tempEmtitySet.clear();

		Material* smesh_transparent = ActorManager::Instance()->GetStaticMaterial_transparent();
		vector<Static_Mesh>::type::iterator itStMesh;
		for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
		{
			for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
			{
				ModelEntity* pMEntity = itStMesh->pEntity;
				if (pMEntity)
				{
					pMEntity->getMesh()->load();
					//LordAssert(pMEntity);
					if (pMEntity->getRenderType() != ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
					{
						ModelEntity* tempModelEntity = LordNew ModelEntity;
						tempModelEntity->setMesh(pMEntity->getMesh());
						tempModelEntity->attachTo(pMEntity->getSceneNode());
						tempModelEntity->setMaterial(smesh_transparent);
						tempModelEntity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
						tempModelEntity->createRenderable(m_isUIActor);
						tempModelEntity->setNaturalColor(mNaturalColor);
						tempModelEntity->setCustomColor(mCustomColor);
						tempModelEntity->setAlphaColor(mAlphaColor);
						itStMesh->pEntity = tempModelEntity;

						tempModelEntity->prepareTexture(mbSingleThread, __FUNCTION__);

						LordDelete pMEntity;
					}
				}
			}
		}

	}
	
	void ActorObject::ReserveActorEntity()
	{
		if (!m_bTranslate)
			return;

		m_isActorFadeOut = false;
		m_bTranslate = false;

		set<SkinedEntity*>::type tempEmtitySet;
		multimap<String, Body_Part_Info*>::type::iterator itBody;
		vector<Skin_Mesh>::type::iterator itMesh;
		bool bfind;

		Material* pMaterial = ActorManager::Instance()->GetSkinMaterial();
		for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end();)
		{
			SkinedEntity* pEntity = *it;
			LordAssert(pEntity);
			if (pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
			{
				SkinedEntity* tempEntity = LordNew SkinedEntity;
				tempEntity->attachTo(pEntity->getSceneNode());
				tempEntity->setSkeleton(mSkeletonMaster);
				tempEntity->setMesh(pEntity->getMesh());
				tempEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
				tempEntity->setMaterial(pMaterial);
				tempEntity->setAnimBlender(m_pAnimBlender);
				tempEntity->createRenderable(m_isUIActor);
				tempEntity->setColorParams(mCurentColorParams);
				tempEntity->setRimColor(mCurentRimColor);
				tempEntity->setAmbientColor(mCurentAmbientColor);
				tempEntity->setNaturalColor(mNaturalColor);
				tempEntity->setAlphaColor(mAlphaColor);
				tempEntity->setCustomColor(mCustomColor);
				tempEmtitySet.insert(tempEntity);
				mEntitySet.erase(it++);

				bfind = false;
				for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
				{
					for (itMesh = itBody->second->SkinMeshes.begin(); itMesh != itBody->second->SkinMeshes.end(); ++itMesh)
					{
						if (itMesh->pEntity == pEntity)
						{
							itMesh->pEntity = tempEntity;
							itMesh->pEntity_WriteZ = NULL;
							bfind = true;
							break;
						}
					}
					if (bfind)
						break;
				}
				tempEntity->prepareTexture(mbSingleThread, __FUNCTION__);
				LordDelete pEntity;
			}
			else if (pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_WRITEDEPTH)
			{
				LordDelete pEntity;
				mEntitySet.erase(it++);
			}
		}

		for (set<SkinedEntity*>::type::iterator it = tempEmtitySet.begin(); it != tempEmtitySet.end(); ++it)
		{
			mEntitySet.insert(*it);
		}
		tempEmtitySet.clear();

		Material* smesh_Material = ActorManager::Instance()->GetStaticMaterial();
		vector<Static_Mesh>::type::iterator itStMesh;
		for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
		{
			for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
			{
				ModelEntity* pMEntity = itStMesh->pEntity;
				if (pMEntity)
				{
					//LordAssert(pMEntity);
					if ((pMEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT))
					{
						ModelEntity* tempModelEntity = LordNew ModelEntity;
						tempModelEntity->setMesh(pMEntity->getMesh());
						tempModelEntity->attachTo(pMEntity->getSceneNode());
						tempModelEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
						tempModelEntity->setMaterial(smesh_Material);
						tempModelEntity->setColorParams(mCurentColorParams);
						tempModelEntity->setRimColor(mCurentRimColor);
						tempModelEntity->setAmbientColor(mCurentAmbientColor);
						tempModelEntity->setNaturalColor(mNaturalColor);
						tempModelEntity->setAlphaColor(mAlphaColor);
						tempModelEntity->setCustomColor(mCustomColor);
						tempModelEntity->createRenderable(m_isUIActor);
						itStMesh->pEntity = tempModelEntity;
						LordDelete pMEntity;
					}
				}
			}
		}
	}


	void ActorObject::SetShadowVisible(bool visible)
	{
		if (m_pShadowEffect)
		{
			m_pShadowEffect->setVisible(visible);
		}
	}

	bool ActorObject::GetShadowVisible()
	{
		if (m_pShadowEffect)
		{
			return m_pShadowEffect->getVisible();
		}

		return false;
	}

	void ActorObject::SetEffectTexture(int effect)
	{
		// todo.
	}

	Texture* ActorObject::GetEffectTexture(int effect)
	{
		Texture* pTexture = NULL;
		switch (effect)
		{
		case 0:
			if (!ActorManager::s_StoneTexture)
			{
				ActorManager::s_StoneTexture = TextureManager::Instance()->createTexture("yanshi.tga");
				ActorManager::s_StoneTexture->load();
			}
			pTexture = ActorManager::s_StoneTexture;
			break;
		case 1:
			if (!ActorManager::s_FrozenTexture)
			{
				ActorManager::s_FrozenTexture = TextureManager::Instance()->createTexture("frozen.tga");
				ActorManager::s_FrozenTexture->load();
			}
			pTexture = ActorManager::s_FrozenTexture;
			break;
		case 2:
			if (!ActorManager::s_PoisonTexture)
			{
				ActorManager::s_PoisonTexture = TextureManager::Instance()->createTexture("poison.tga");
				ActorManager::s_PoisonTexture->load();
			}
			pTexture = ActorManager::s_PoisonTexture;
			break;
		}
		return pTexture;
	}

	void ActorObject::RevertEffectTexture()
	{
		if (mHasInited)
		{
			Material* pMaterial = ActorManager::Instance()->GetSkinMaterial();
			Material* pMaterial_translate = ActorManager::Instance()->GetSkinMaterial_transparent();
			Material* pMaterial_depth = ActorManager::Instance()->GetSkinMaterial_depth();

			Material* pMaterial_static = ActorManager::Instance()->GetStaticMaterial();
			Material* pMaterial_static_translate = ActorManager::Instance()->GetStaticMaterial_transparent();
			for (set<SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				if ((*it)->getRenderType() == ModelEntity::RT_FOR_ACTOR_WRITEDEPTH)
					continue;

				if ((*it)->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
				{
					(*it)->setMaterial(pMaterial_translate);
				}
				else
				{
					(*it)->setMaterial(pMaterial);
				}
				(*it)->revertEffectTexture();
			}

			map<String, Child_Mesh*>::type::iterator itChild = mChildMeshs.begin();
			for (; itChild != mChildMeshs.end(); ++itChild)
			{
				itChild->second->pModelEntity->setMaterial(pMaterial_static);
				itChild->second->pModelEntity->revertEffectTexture();
			}

			vector<Static_Mesh>::type::iterator itStMesh;
			multimap<String, Body_Part_Info*>::type::iterator itBody;
			for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
			{
				for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
				{
					if (itStMesh->pEntity)
					{
						if (itStMesh->pEntity->getRenderType() == ModelEntity::RT_FOR_ACTOR_TRANSPARENT)
						{
							itStMesh->pEntity->setMaterial(pMaterial_static_translate);
						}
						else
						{
							itStMesh->pEntity->setMaterial(pMaterial_static);
						}
						itStMesh->pEntity->revertEffectTexture();
					}
				}
			}

			setActorFrozen(false);
		}
	}

	void ActorObject::SetAmbientColor(const Color& color)
	{
		mCurentAmbientColor = color;
		if (mHasInited)
		{
			for (LORD::set<LORD::SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				LORD::SkinedEntity* pEntity = *it;
				if (pEntity)
				{
					pEntity->setAmbientColor(color);
				}
			}
			LORD::map<LORD::String, Child_Mesh*>::type::iterator itMesh = mChildMeshs.begin();
			for (; itMesh != mChildMeshs.end(); ++itMesh)
			{
				itMesh->second->pModelEntity->setAmbientColor(color);
			}
			vector<Static_Mesh>::type::iterator itSMesh;
			multimap<String, Body_Part_Info*>::type::iterator itSm = mBodyParts.begin();
			for (; itSm != mBodyParts.end(); ++itSm)
			{
				for (itSMesh = itSm->second->StaticMeshes.begin(); itSMesh != itSm->second->StaticMeshes.end(); ++itSMesh)
				{
					if (itSMesh->pEntity)
						itSMesh->pEntity->setAmbientColor(color);
				}
			}
		}
	}

	void ActorObject::SetMirror(bool bMirror)
	{
		m_isMirror = bMirror;
		mScaleDirty = true;
	}

	LORD::Vector2 ActorObject::GetTextScreenPosition()
	{
		Vector3 worldpos = GetPosition();
		Vector3 screenpos(Vector3::ZERO);

		Renderer::Instance()->project(screenpos, worldpos, SceneManager::Instance()->getMainCamera()->getViewProjMatrix());

		Vector2 logicpos(Vector2::ZERO);
		GUISystem::Instance()->AdaptPosition(Vector2(screenpos.x, screenpos.y), logicpos);
		return logicpos;
	}

	void ActorObject::ChangeDefaultTexture(const String& name)
	{
		if (mHasInited)
		{
			if (m_pDefaultTexture)
			{
				TextureManager::Instance()->releaseResource(m_pDefaultTexture);
				m_pDefaultTexture = NULL;
			}

			m_pDefaultTexture = TextureManager::Instance()->createTexture(name);
			if (m_pDefaultTexture)
			{
				if (m_pDefaultTexture->load())
				{
					for (LORD::set<LORD::SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
					{
						LORD::SkinedEntity* pEntity = *it;
						if (pEntity)
						{
							pEntity->changeDefaultTexture(m_pDefaultTexture);
						}
					}
				}
				else
				{
					LordLogError("%s", ("ActorObject::ChangeDefaulTexture:load texture error : " + name).c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "ActorObject::ChangeDefaulTexture:load texture error : [%s].", name.c_str());
					MessageBox(0, buf, "Error", 0);
#endif
				}
			}
			else
			{
				LordLogError("%s", ("ActorObject::ChangeDefaulTexture:create texture error : " + name).c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "ActorObject::ChangeDefaulTexture:create texture error : [%s].", name.c_str());
				MessageBox(0, buf, "Error", 0);
#endif
			}
		}
		else
		{
			DefaultTexDelayProcess* process = LordNew DefaultTexDelayProcess(name);
			mDelayProcessVector.push_back(process);
		}
	}

	void ActorObject::RevertDefaultTexture()
	{
		if (mHasInited)
		{
			if (m_pDefaultTexture)
			{
				TextureManager::Instance()->releaseResource(m_pDefaultTexture);
				m_pDefaultTexture = NULL;
			}
			for (LORD::set<LORD::SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				LORD::SkinedEntity* pEntity = *it;
				if (pEntity)
				{
					pEntity->revertDefaultTexture();
				}
			}
		}
		else
		{
			RevertTexDelayProcess* process = LordNew RevertTexDelayProcess;
			mDelayProcessVector.push_back(process);
		}
	}

	void ActorObject::ChangeDark(float radio)
	{
		if (m_DarkRadio != radio)
		{
			m_DarkRadio = radio;

			for (LORD::set<LORD::SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
			{
				LORD::SkinedEntity* pEntity = *it;
				if (pEntity)
				{
					pEntity->setDarkRadio(radio);
				}
			}

			multimap<String, Body_Part_Info*>::type::iterator itBody;
			vector<Static_Mesh>::type::iterator itStMesh;
			for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
			{
				for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
				{
					ModelEntity* pMEntity = itStMesh->pEntity;
					if (pMEntity)
						pMEntity->setDarkRadio(m_DarkRadio);
				}
			}
		}
	}

	void ActorObject::RevertLight()
	{
		m_DarkRadio = 1.0f;

		for (LORD::set<LORD::SkinedEntity*>::type::iterator it = mEntitySet.begin(); it != mEntitySet.end(); ++it)
		{
			LORD::SkinedEntity* pEntity = *it;
			if (pEntity)
			{
				pEntity->setDarkRadio(m_DarkRadio);
			}
		}

		multimap<String, Body_Part_Info*>::type::iterator itBody;
		vector<Static_Mesh>::type::iterator itStMesh;
		for (itBody = mBodyParts.begin(); itBody != mBodyParts.end(); ++itBody)
		{
			for (itStMesh = itBody->second->StaticMeshes.begin(); itStMesh != itBody->second->StaticMeshes.end(); ++itStMesh)
			{
				ModelEntity* pMEntity = itStMesh->pEntity;
				if (pMEntity)
					pMEntity->setDarkRadio(m_DarkRadio);
			}
		}
	}

	void ActorObject::stopCameraAnim()
	{
		mSkillSystem.StopCameraAnim();
	}

	void ActorObject::mergeMeshes()
	{

	}

	void ActorObject::releaseMergeResource()
	{
		if (mMergeTexture)
		{
			LordSafeDelete(mMergeTexture);
		}
		if (mMergeSkin)
		{
			// LordSafeDelete(mMergeSkin);
		}
	}

	void ActorObject::LogBodyPartMeshs(const String& prompt)
	{
		/*
		String msg = StringUtil::Format("%s~~~~~~~~~[%x] ", prompt.c_str(), (ui32)this);
		
		if (mBodyParts.size() > 0)
		{
			for (auto it : mBodyParts)
			{
				msg += "{";
				if (it.second->SkinMeshes.size() > 0)
				{
					msg += "skins:";
					for (auto itskin = it.second->SkinMeshes.begin(); itskin != it.second->SkinMeshes.end(); ++itskin)
					{
						if (itskin->pMesh)
							msg += itskin->pMesh->getName();
						else
							msg += "0000,";
					}
				}
				if (it.second->StaticMeshes.size() > 0)
				{
					msg += "static:";
					for (auto itmesh = it.second->StaticMeshes.begin(); itmesh != it.second->StaticMeshes.end(); ++itmesh)
					{
						if (itmesh->pMesh)
							msg += itmesh->pMesh->getName();
						else
							msg += "0000,";
					}
				}
				msg += "} ";
			}
		}
		LordLogInfo(msg.c_str());
		*/
	}

	void ActorObject::requestMergeMeshesEvent()
	{
		if (mMergeRenderable)
		{
			if (mbSingleThread)
			{
				MergeTexturesEvent* pEvent = LordNew MergeTexturesEvent(this);
				pEvent->ProcessEvent();
				pEvent->RespondEvent();
				LordDelete pEvent;
			}
			else
			{
				StreamThread* pThread = StreamThread::Instance();
				if (pThread && pThread->IsRunning())
				{
					MergeTexPrepareEvent* pEvent = LordNew MergeTexPrepareEvent(this);
					pThread->AddRequest(pEvent);
					mbMergeFinished = false;
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	ActorPrepareEvent::ActorPrepareEvent(ActorObject* pActor)
		: m_pActor(pActor)
		, m_RespondStep(1)
	{

	}

	ActorPrepareEvent::~ActorPrepareEvent()
	{
		m_pActor = NULL;
	}

	bool ActorPrepareEvent::ProcessEvent()
	{
		//LordLogInfo("=========[ActorPrepareEvent::ProcessEvent] actor(%x)", m_pActor);
		if (m_pActor)
		{
			m_pActor->prepare_io();
			return true;
		}
		else
		{
			LordException("ActorPrepareEvent::ProcessEvent: the request Actor Point is NULL!");
		}
		return false;
	}

	bool ActorPrepareEvent::RespondEvent()
	{
		//LordLogInfo("=========[ActorPrepareEvent::RespondEvent] actor(%x)", m_pActor);
		if (m_pActor)
		{
			return m_pActor->InitAfterIOprepared(m_RespondStep);
		}
		else
		{
			LordException("ActorPrepareEvent::RespondEvent: the request Actor Point is NULL!");
		}
		return false;
	}

	//-----------------------------------------------------------------------
	ActorDestoryEvent::ActorDestoryEvent(ActorObject* pActor)
		: m_pActor(pActor)
	{
	}

	ActorDestoryEvent::~ActorDestoryEvent()
	{
		if (m_pActor)
		{
			LordDelete m_pActor;
		}
		m_pActor = NULL;
	}

	//-----------------------------------------------------------------------
	ActorPreDestoryEvent::ActorPreDestoryEvent(ActorObject* pActor)
		: m_pActor(pActor)
	{
	}

	ActorPreDestoryEvent::~ActorPreDestoryEvent()
	{
	}

	bool ActorPreDestoryEvent::ProcessEvent()
	{
		//LordLogInfo("=========[ActorPreDestoryEvent::ProcessEvent] actor(%x)", m_pActor);
		return true;
	}

	bool ActorPreDestoryEvent::RespondEvent()
	{
		//LordLogInfo("=========[ActorPreDestoryEvent::RespondEvent] actor(%x)", m_pActor);
		if (m_pActor)
		{
			StreamThread* pThread = StreamThread::Instance();
			if (pThread && pThread->IsRunning())
			{
				ActorDestoryEvent* pEvent = LordNew ActorDestoryEvent(m_pActor);
				pThread->AddRequest(pEvent);
			}
			return true;
		}
		else
		{
			LordException("ActorPreDestoryEvent::RespondEvent: the request Actor point is NULL!");
		}
		return false;
	}

	bool ActorDestoryEvent::ProcessEvent()
	{
		//LordLogInfo("=========[ActorDestoryEvent::ProcessEvent] actor(%x)", m_pActor);
		return true;
	}

	bool ActorDestoryEvent::RespondEvent()
	{
		//LordLogInfo("=========[ActorDestoryEvent::RespondEvent] actor(%x)", m_pActor);
		if (m_pActor)
		{
			m_pActor->EnableAllBodyParts(false);
			m_pActor->bodyPartString = "Delete by multyThread Call";
			LordDelete m_pActor;
			m_pActor = NULL;
			return true;
		}
		else
		{
			LordException("ActorDestoryEvent::RespondEvent: the request Actor point is NULL!");
		}
		return false;
	}

	ActorDeAttachAllEvent::ActorDeAttachAllEvent(ActorObject* pActor)
		: m_pActor(pActor)
	{}

	bool ActorDeAttachAllEvent::RespondEvent()
	{
		//m_pActor->LogBodyPartMeshs("Before DeattachAll");
		if (ActorManager::Instance() && ActorManager::Instance()->ValidActor(m_pActor)) {
			m_pActor->_unloadAllBodyParts();
		}

		//m_pActor->LogBodyPartMeshs("After DeattachAll");
		return true;
	}

	//-----------------------------------------------------------------------
	ActorAttachMeshEvent::ActorAttachMeshEvent(ActorObject* pActor, vector<SkinnedMesh*>::type& pMeshes, const String& masterName, const String& slaveName)
		: m_pActor(pActor)
	{
		m_MeshesVec.swap(pMeshes);
		m_SlaveName = slaveName;
		m_MasterName = masterName;
		mCurrentIndex = 0;
		//LordLogInfo("=========[ActorAttachMeshEvent::ActorAttachMeshEvent] m_MasterName[%s], m_SlaveName[%s], actor(%x)", m_MasterName.c_str(), m_SlaveName.c_str(),m_pActor);
	}

	ActorAttachMeshEvent::~ActorAttachMeshEvent()
	{
		m_pActor = NULL;
		m_MeshesVec.clear();
	}

	bool ActorAttachMeshEvent::ProcessEvent()
	{
		//LordLogInfo("=========[ActorAttachMeshEvent::ProcessEvent] main(%s) slave(%s) actor(%x)", m_MasterName.c_str(), m_SlaveName.c_str(), m_pActor);
		if (!ActorManager::Instance()->ValidActor(m_pActor))
			return true;

		try
		{
			for (ui32 i = 0; i<m_MeshesVec.size(); ++i)
			{
				m_MeshesVec[i]->prepare();
			}
			return true;
		}
		catch (...)
		{
			LordException("ActorAttachMeshEvent::ProcessEvent: error prepare actor in attchMesh event.");
		}
		return false;
	}

	bool ActorAttachMeshEvent::RespondEvent()
	{
		//LordLogInfo("=========[ActorAttachMeshEvent::RespondEvent] main(%s) slave(%s) actor(%x)", m_MasterName.c_str(), m_SlaveName.c_str(), m_pActor);
		
		if (mCurrentIndex < m_MeshesVec.size())
		{
			int index = mCurrentIndex++;
			bool ret = m_MeshesVec[index]->load();
			if (!ret)
			{
				LordLogError("%s", ("load mesh error : " + m_MeshesVec[index]->getName()).c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "load mesh error : [%s].", m_MeshesVec[index]->getName().c_str());
				MessageBox(0, buf, "Error", 0);
#endif
				MeshManager::Instance()->releaseResource(m_MeshesVec[index]);
				m_MeshesVec[index] = NULL;
			}
			return false;
		}

		if (ActorManager::Instance()->ValidActor(m_pActor))
		{
			if (!m_pActor->mHasInited)
				return false;

			vector<LORD::SkinnedMesh*>::type pMeshes;
			std::pair<multimap<String, ActorObject::Body_Part_Info*>::type::iterator, multimap<String, ActorObject::Body_Part_Info*>::type::iterator> itBodyParts;
			itBodyParts = m_pActor->mBodyParts.equal_range(m_MasterName);

			bool loaded = false;
			multimap<String, ActorObject::Body_Part_Info*>::type::iterator itt, tempIt;
			for (itt = itBodyParts.first; itt != itBodyParts.second;)
			{
				if (itt->second->pTemplateData->SlaveName == m_SlaveName)
				{
					loaded = true;
					++itt;
				}
				else
				{
					tempIt = itt++;
					m_pActor->UnloadBodyPart(tempIt->second);
					LORD_DELETE_T(tempIt->second, Body_Part_Info);
					tempIt->second = NULL;
					m_pActor->mBodyParts.erase(tempIt);
				}
			}

			if (!loaded)
			{
				map<String, ActorTemplateData::BodyPart*>::type::const_iterator it = m_pActor->m_pTemplateData->BodyPartsMap.find(m_MasterName + "." + m_SlaveName);
				if (it == m_pActor->m_pTemplateData->BodyPartsMap.end())
				{
					LordAssert(("there is resource lost" + m_MasterName + "." + m_SlaveName).c_str());
					return true;
				}

				ActorObject::Body_Part_Info* pBodyPart = LORD_NEW_T(ActorObject::Body_Part_Info);
				LordAssert(pBodyPart);
				pBodyPart->pTemplateData = it->second;
				m_pActor->LoadBodyPart(pBodyPart, m_MeshesVec);

				assert(m_SlaveName == it->second->SlaveName && m_MasterName == it->second->MasterName);

				m_pActor->mBodyParts.insert(std::pair<String, ActorObject::Body_Part_Info*>(m_MasterName, pBodyPart));

				m_pActor->refleshActorAlpha();
			}
		}

		//String prompt = StringUtil::Format("After AttachMeshEvent %s:%s", m_MasterName.c_str(), m_SlaveName.c_str());
		//m_pActor->LogBodyPartMeshs(prompt);
		return true;
	}
	MergeTexPrepareEvent::MergeTexPrepareEvent(ActorObject* pActor)
		: m_pActor(pActor)
	{}
	MergeTexPrepareEvent::~MergeTexPrepareEvent()
	{
		m_pActor = NULL;
	}

	bool MergeTexPrepareEvent::ProcessEvent()
	{
		//LordLogInfo("=========[MergeTexPrepareEvent::ProcessEvent] actor(%x)", m_pActor);
		return true;
	}

	bool MergeTexPrepareEvent::RespondEvent()
	{
		//LordLogInfo("=========[MergeTexPrepareEvent::RespondEvent] actor(%x)", m_pActor);

		if (!ActorManager::Instance()->ValidActor(m_pActor))
		{
			m_pActor->mbMergeFinished = true;
			return true;
		}

		StreamThread* pThread = StreamThread::Instance();
		if (pThread && pThread->IsRunning())
		{
			MergeTexturesEvent* pEvent = LordNew MergeTexturesEvent(m_pActor);
			pThread->AddRequest(pEvent);
		}
	
		return true;
	}

	//-----------------------------------------------------------------------
	MergeTexturesEvent::MergeTexturesEvent(ActorObject* pActor)
		: m_pActor(pActor)
		, m_NeedMerge(false)
	{
		//pActor->LogBodyPartMeshs("~~~~~~~~~~~~~~MergeTextureEvent");
		vector<Mesh*>::type meshes;
		for (auto it : m_pActor->mBodyParts)
		{
			for (auto itskin = it.second->SkinMeshes.begin(); itskin != it.second->SkinMeshes.end(); ++itskin)
			{
				meshes.push_back(itskin->pMesh);
			}
			for (auto itmesh = it.second->StaticMeshes.begin(); itmesh != it.second->StaticMeshes.end(); ++itmesh)
			{
				meshes.push_back(itmesh->pMesh);
			}
		}
		 
		String setname;
		for (Mesh* it : meshes)
		{
			Mesh* pMesh = it;
			if (!pMesh) {
				LordLogError("=========[MergeTexturesEvent::MergeTexturesEvent] mesh is null , actor(%x)", pActor);
				continue;
			}
			if (!pMesh->isLoaded()) {
				LordLogError("=========[MergeTexturesEvent::MergeTexturesEvent] mesh not loaded, actor(%x)", pActor);
				continue;
			}
			for (int sub = 0; sub < pMesh->getSubMeshNum(); ++sub)
			{
				String texname = pMesh->getTextureName(sub, 0);
				Texture* pTex = (Texture*)TextureManager::Instance()->getByName(texname);

				bool hasexit = false;
				for (auto itor : m_Textures)
				{
					if (itor.second == texname)
					{
						hasexit = true;
						break;
					}
				}
				if (!hasexit)
					m_Textures.insert(std::make_pair(pTex->getWidth()*pTex->getHeight(), texname));
				//setname += pMesh->getName();//模型组成一致，贴图使用一定一致，可根据此来进行复用
			}
		}
		static int s_Texid = 0;
		setname = "MergeTexture_" + StringUtil::ToString(s_Texid++);
		//暂不复用
		//m_TextureSet = MergeMeshManager::Instance()->GetTextureSet(setname);
		//if (!m_TextureSet)
		{
			//创建贴图需要在主线程
			m_TextureSet = MergeMeshManager::Instance()->AllocTextureSet(setname, 1024, 512);
			m_NeedMerge = true;
		}

		//m_TextureSet->addTextureRefrence(); //加引用计数
	}
	MergeTexturesEvent::~MergeTexturesEvent()
	{
		m_pActor = NULL;
		m_Textures.clear();
		m_TextureIds.clear();
	}

	bool MergeTexturesEvent::ProcessEvent()
	{
		//LordLogInfo("=========[MergeTexturesEvent::ProcessEvent] actor(%x), ", m_pActor);
		if (!m_NeedMerge)
			return true;

		if (!ActorManager::Instance()->ValidActor(m_pActor))
		{
			m_pActor->mbMergeFinished = true;
			return true;
		}

		//LordLogInfo("=========[MergeTexturesEvent::ProcessEvent] actor(%x)", m_pActor);

		for (auto it = m_Textures.rbegin(); it != m_Textures.rend(); ++it)
		{
			String texname = it->second;
			if (m_TextureIds.find(texname) == m_TextureIds.end())
			{
				DataStream* pStream = ResourceGroupManager::Instance()->openResource(texname);
				if (!pStream)
				{
					LordLogError("MergeTexturesEvent::ProcessEvent Texture [%s] is not exist", texname.c_str());
					m_TextureIds.insert(std::make_pair(texname, 0));
					continue;
				}
				Byte* pData = (Byte*)(LordMalloc(pStream->size()));
				int nSize = pStream->size();
				pStream->read(pData, nSize);

				Buffer imageBuffer(nSize, pData);
				Image* img = Image::CreateFromMemory(imageBuffer, Image::GetImageFormat(texname));

				int texWidth = img->getWidth();
				int texHeight = img->getHeight();
				Byte* sourceData = img->getData();
				
				int texid = m_TextureSet->Insert_MultiThread(sourceData, img->getPixelFormat(), texWidth, texHeight);
				m_TextureIds.insert(std::make_pair(texname, texid));

				LordDelete img;
				LordFree(pData);
				LordDelete pStream;
			}
		}

		return true;
	}

	bool MergeTexturesEvent::RespondEvent()
	{
		if (!ActorManager::Instance()->ValidActor(m_pActor))
		{
			m_pActor->mbMergeFinished = true;
			return true;
		}
		
		//LordLogInfo("=========[MergeTexturesEvent::RespondEvent] actor(%x)", m_pActor);

		if (m_NeedMerge)
		{
			m_TextureSet->uploadBuffer();
		}
		if (m_pActor->mbSingleThread)
		{
			MergeMeshesEvent* pEvent = LordNew MergeMeshesEvent(m_pActor, m_TextureSet, m_TextureIds);
			pEvent->ProcessEvent();
			pEvent->RespondEvent();
			LordDelete pEvent;
		}
		else
		{
			StreamThread * pThread = StreamThread::Instance();
			if (pThread && pThread->IsRunning())
			{
				//贴图装箱完毕开始模型buffer合并
				MergeMeshesEvent* pEvent = LordNew MergeMeshesEvent(m_pActor, m_TextureSet, m_TextureIds);
				pThread->AddRequest(pEvent);
			}
		}
		return true;
	}
	//-----------------------------------------------------------------------
	MergeMeshesEvent::MergeMeshesEvent(ActorObject* pActor, TextureSet* pTexSet, map<String, int>::type& textureIds)
		: m_pActor(pActor)
		, m_pTexSet(pTexSet)
		, m_NeedMerge(false)
	{

		m_TextureIds.insert(textureIds.begin(), textureIds.end());
		for (auto it : m_pActor->mBodyParts)
		{
			for (auto itskin : it.second->SkinMeshes)
			{
				m_Meshes.push_back(itskin.pMesh);
			}
			for (auto itstatic : it.second->StaticMeshes)
			{
				LordAssert(m_pActor->GetSkeletonMaster());
				Bone* pFatherBone = m_pActor->GetSkeletonMaster()->getBone(itstatic.pTemplateMesh->SocketName);
				m_staticMeshes.push_back(std::make_pair(itstatic.pMesh, pFatherBone->getId()));
			}
		}
		//贴图不复用，模型也一样不复用，保持统一
		//m_pSkinnedMesh = MergeMeshManager::Instance()->GetSkinnedMesh(pTexSet);
		//if (!m_pSkinnedMesh)
		{
			m_NeedMerge = true;
			m_pSkinnedMesh = MergeMeshManager::Instance()->AllocSkinnedMesh(pTexSet);
		}
		//m_pSkinnedMesh->addRefrenceCount(); //加引用计数
	}
	MergeMeshesEvent::~MergeMeshesEvent()
	{
		m_pActor = NULL;
		m_Meshes.clear();
	}

	bool MergeMeshesEvent::ProcessEvent()
	{
		if (!ActorManager::Instance()->ValidActor(m_pActor))
		{
			m_pActor->mbMergeFinished = true;
			return true;
		}

		if (m_NeedMerge)
		{
			//LordLogInfo("=========[MergeMeshesEvent::ProcessEvent] actor(%x)", m_pActor);
			map<SubMesh*, Vector4>::type meshwithUVBias;
			map<SubMesh*, std::pair<Vector4, int>>::type staticmeshwithUVBias;
			for (Mesh* it : m_Meshes)
			{
				Mesh* pMesh = it;
				for (int i = 0; i < pMesh->getSubMeshNum(); ++i)
				{
					auto itid = m_TextureIds.find(pMesh->getTextureName(i, 0));
					LordAssert(itid != m_TextureIds.end());
					Vector4 uvbias = m_pTexSet->GetViewport(itid->second);
					SubMesh* pSubmesh = pMesh->getSubMesh(i);
					meshwithUVBias.insert(std::make_pair(pSubmesh, uvbias));
				}
			}
			for (auto it : m_staticMeshes)
			{
				Mesh* pMesh = it.first;
				for (int i = 0; i < pMesh->getSubMeshNum(); ++i)
				{
					auto itid = m_TextureIds.find(pMesh->getTextureName(i, 0));
					LordAssert(itid != m_TextureIds.end());
					Vector4 uvbias = m_pTexSet->GetViewport(itid->second);
					SubMesh* pSubmesh = pMesh->getSubMesh(i);
					staticmeshwithUVBias.insert(std::make_pair(pSubmesh, std::make_pair(uvbias, it.second)));
				}
			}
			m_pSkinnedMesh->MergeSubMesh(meshwithUVBias, staticmeshwithUVBias);
		}
		return true;
	}

	bool MergeMeshesEvent::RespondEvent()
	{
		//LordLogInfo("=========[MergeMeshesEvent::RespondEvent] actor(%x)", m_pActor);
		if (!ActorManager::Instance()->ValidActor(m_pActor))
		{
			m_pActor->mbMergeFinished = true;
			return true;
		}

		if (m_NeedMerge)
		{
			m_pSkinnedMesh->setTextureName(0, m_pTexSet->GetTexture()->getName());
			m_pSkinnedMesh->load();
		}

		//卸载当前身上的Skin和Staticmesh
		ActorObject::Body_Part_Info* pBodyPart = LORD_NEW_T(ActorObject::Body_Part_Info);
		LordAssert(pBodyPart);
		pBodyPart->SkinMeshes.resize(1);
		for (auto it = m_pActor->mBodyParts.begin(); it != m_pActor->mBodyParts.end();)
		{
			pBodyPart->pTemplateData = it->second->pTemplateData;//取个指针指向
			for (auto itskin = it->second->SkinMeshes.begin(); itskin != it->second->SkinMeshes.end(); ++itskin)
			{
				pBodyPart->SkinMeshes[0].pTemplateSkin = itskin->pTemplateSkin;
				m_pActor->UnloadSkin(*itskin);
			}
			for (auto itmesh = it->second->StaticMeshes.begin(); itmesh != it->second->StaticMeshes.end(); ++itmesh)
			{
				m_pActor->UnloadStaticMesh(*itmesh);
			}
			if (it->first == "Merge")//删除之前合并的模型
			{
				m_pActor->mBodyParts.erase(it++);
			}
			else
			{
				++it;
			}
		}
		
		m_pActor->releaseMergeResource();

		//装载合并后的skin
		m_pActor->LoadSkin(pBodyPart->SkinMeshes[0], m_pSkinnedMesh);
		//m_pActor->LoadMergedSkin(pBodyPart->SkinMeshes[0], m_pSkinnedMesh);
		m_pActor->mBodyParts.insert(std::pair<String, ActorObject::Body_Part_Info*>("Merge", pBodyPart));
		pBodyPart->SkinMeshes[0].pTemplateSkin = new ActorTemplateData::Skin;
		m_pActor->mbMergeFinished = true;
		m_pActor->mMergeTexture = m_pTexSet;
		m_pActor->mMergeSkin = m_pSkinnedMesh;
		//m_pActor->LogBodyPartMeshs("After Merge Meshs");
		//LordLogInfo("----------[MergeMesh Finished] actor(%x) ", m_pActor);
		return true;
	}
	//-----------------------------------------------------------------------
	SkillDelayedPlayProcess::SkillDelayedPlayProcess(const String& name, bool bInsert, bool casecade, float timescale, int userdata, int userdata2, const String& actorNameArray)
		: m_Name(name)
		, m_bInsert(bInsert)
		, m_bCasecade(casecade)
		, m_Timescale(timescale)
		, m_Userdata(userdata)
		, m_Userdata2(userdata2)
	{
		m_ActorNameArray.assign(actorNameArray.begin(), actorNameArray.end());
	}

	SkillDelayedPlayProcess::~SkillDelayedPlayProcess()
	{
		m_ActorNameArray.clear();
	}

	void SkillDelayedPlayProcess::Execute(ActorObject* pActor)
	{
		pActor->PlaySkill(m_Name, m_bInsert, m_bCasecade, m_Timescale, m_Userdata, m_Userdata2, m_ActorNameArray);
	}

	//-----------------------------------------------------------------------
	SkillDelayedStopProcess::SkillDelayedStopProcess(bool casecade)
		: m_bCasecade(casecade)
	{

	}

	SkillDelayedStopProcess::~SkillDelayedStopProcess()
	{
	}

	void SkillDelayedStopProcess::Execute(ActorObject* pActor)
	{
		pActor->StopSkill(m_bCasecade);
	}

	//-----------------------------------------------------------------------
	EffectDelayedAddProcess::EffectDelayedAddProcess()
		: m_isOrientFollowParentNode(false)
		, m_Times(0)
		, m_TimeScale(1.f)
		, m_Alpha(1.f)
	{

	}

	EffectDelayedAddProcess::~EffectDelayedAddProcess()
	{

	}

	void EffectDelayedAddProcess::Execute(ActorObject* pActor)
	{
		pActor->AddPluginEffect(m_Name, m_SocketName, m_EffectName, m_TimeScale,
			m_Alpha, m_Times, m_Position, m_Orientation, m_Scale, m_isOrientFollowParentNode);
	}

	//-----------------------------------------------------------------------
	EffectDelayedDelProcess::EffectDelayedDelProcess(const String& name, bool smooth)
		: m_Name(name)
		, m_Smooth(smooth)
	{

	}

	EffectDelayedDelProcess::~EffectDelayedDelProcess()
	{

	}

	void EffectDelayedDelProcess::Execute(ActorObject* pActor)
	{
		pActor->DelPluginEffect(m_Name, m_Smooth);
	}

	//-----------------------------------------------------------------------
	SlaveAttachProcess::SlaveAttachProcess(const String& masterName, const String& slaveName)
		: m_MasterName(masterName)
		, m_SlaveName(slaveName)
	{

	}

	SlaveAttachProcess::~SlaveAttachProcess()
	{

	}

	void SlaveAttachProcess::Execute(ActorObject* pActor)
	{
		pActor->AttachSlave(m_MasterName, m_SlaveName);
	}


	//-----------------------------------------------------------------------
	ChildAttachProcess::ChildAttachProcess(ActorObject* pChild, const String& SocketName)
		: m_ChildActor(pChild)
		, m_SocketName(SocketName)
	{

	}

	ChildAttachProcess::~ChildAttachProcess()
	{
		m_ChildActor = NULL;
	}

	void ChildAttachProcess::Execute(ActorObject* pActor)
	{
		if (ActorManager::Instance()->ValidActor(m_ChildActor))
		{
			pActor->CompleteAttachChildActor(m_SocketName, m_ChildActor);
		}
	}

	//-----------------------------------------------------------------------
	ChildDetachProcess::ChildDetachProcess(ActorObject* child)
		: m_ChildActor(child)
	{

	}

	void ChildDetachProcess::Execute(ActorObject* pActor)
	{
		if (ActorManager::Instance()->ValidActor(m_ChildActor))
		{
			pActor->CompleteDetachChildActor(m_ChildActor);
		}
	}

	//-----------------------------------------------------------------------
	ChildAddDelayedProcess::ChildAddDelayedProcess(const String& boneName)
		: m_BoneName(boneName)
	{

	}

	void ChildAddDelayedProcess::Execute(ActorObject* pActor)
	{
		if (Bone* pBone = pActor->GetSkeletonMaster()->getBone(m_BoneName))
		{
			static Quaternion quat1(Vector3(1.0f, 0.0f, 0.0f), -Math::PI / 2);
			assert(pBone);
			pActor->GetSceneNodeMaster()->setLocalOrientation(quat1);
		}
		else
		{
			pActor->GetSceneNodeMaster()->setLocalOrientation(Quaternion());
		}
	}

	//-----------------------------------------------------------------------
	void ChildDelDelayedProcess::Execute(ActorObject* pActor)
	{
		//reflesh local pos ori scale

		if (pActor->isUIActor())
		{
			pActor->SetPosition(UI_ACTOR_POSITION);
		}
		else
		{
			pActor->SetPosition(pActor->GetPosition());
		}
		pActor->SetOrientation(pActor->GetOrientation());
		pActor->SetScale(pActor->GetScale());
	}

	void DefaultTexDelayProcess::Execute(ActorObject* pActor)
	{
		pActor->ChangeDefaultTexture(m_TextureName);
	}


	void RevertTexDelayProcess::Execute(ActorObject* pActor)
	{
		pActor->RevertDefaultTexture();
	}

}