#include "Core.h"
#include "SkillAttack.h"
#include "ActorObject.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceGroupManager.h"
#include "Object/Exception.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Effect/EffectSystemManager.h"
#include "Anim/SkeletonManager.h"
#include "Audio/SoundSystem.h"
#include "ActorManager.h"

namespace LORD
{
	int SkillAttack::skillEffectCount = 0;

	String SkillAttack::getSkillSceneNodeName()
	{
		String result = "SkillAtt";
		result += StringUtil::Format("%d", skillEffectCount++);
		return result;
	}

	SkillAttack::SkillAttack()
		: mpTargetActor(NULL)
		, mbFly(false)
		, mbAttack(false)
		, mbGround(false)
		, mbChangeTargetColor(false)
		, mbHasAttackedSound(false)
		, mbHasFlySound(false)
		, mbTargetCameraShake(false)
		, mbTargetSlowDown(false)
		, mbPlayWoundAction(true)
		, mCallbackIndex(0)
		, mCallbackNum(0)
		, mUserData(-1)
		, mUserData2(-1)
		, mOriginalActorPos(Vector3::ZERO)
		, mpOriginalActor(NULL)
		, mBeginPosition(Vector3::ZERO)
		, mBeginOffset(Vector3::ZERO)
		, mEndNode(NULL)
		, mEndOffset(Vector3::ZERO)
		, mFlyScale(1.0f)
		, mFlySpeed(20.0f)
		, mFlyTime(2.0f)
		, mFlyType(FLT_STRAIGHTLINE)
		, mFlyDirection(FD_SRCTODEST)
		, mTargetSize(1.6f)
		, mChangeSize(true)
		, mAttackNode(NULL)
		, mAttackOffset(Vector3::ZERO)
		, mAttackScale(1.0f)
		, mFallowTarget(false)
		, mGroundNode(NULL)
		, mGroundOffset(Vector3::ZERO)
		, mGroundScale(1.0f)
		, mGroundTarget(false)
		, mState(SAS_WAITTING)
		, mbMissed(false)
		, mChangeColorNum(0)
		, mBeginColor(Color::BLACK)
		, mAttackedSoundName(0)
		, mfAttackedSoundVolume(0.6f)
		, mbBroadcastAttackedSound(false)
		, mfBroadcastAttackedSoundVolume(0.2f)
		, mfAttackedSoundMinDistance(2.0f)
		, mfAttackedSoundMaxDistance(5.0f)
		, mFlySoundName(0)
		, mfFlySoundVolume(0.6f)
		, mbBroadcastFlySound(false)
		, mfBroadcastFlySoundVolume(0.2f)
		, mfFlySoundMinDistance(2.0f)
		, mfFlySoundMaxDistance(5.0f)
		, mTargetCameraShakeBeginTime(0.0f)
		, mTargetCameraShakeScale(0.0f)
		, mTargetCameraShakeDuration(0.0f)
		, mTargetSlowDownScale(0.0f)
		, mTargetSlowDownBeginTime(0.0f)
		, mTargetSlowDownDurationTime(0.0f)
		, mTargetSlowDownEqualizeTime(0.0f)
		, mpFlyEffectSystem(NULL)
		, mpAttackEffectSystem(NULL)
		, mpGroundEffectSystem(NULL)
		, mpFlySceneNode(NULL)
		, mpAttackSceneNode(NULL)
		, mpGroundSceneNode(NULL)
		, mCurrentFlyTime(0.0f)
		, mCurrentSlowDownTime(0.0f)
		, mCurrentCameraShakeTime(0.0f)
		, mCurrentFlyState(CFS_UNLAUNCH)
		, mbAttackedSoundPlayed(false)
		, mAttNameID(StringUtil::BLANK)
		, mSkillName(StringUtil::BLANK)
		, mbIsCallBack(true)
		, mbDirectivity(true)
		, mpUnDirectivityNode(NULL)
		, mbNeedReplaceATT(false)
		, mReplaceATTIndex(0)
		, mWoundSkill("hit")
		, mFlyStayTime(0.0f)
		, mFlyDeltaHeight(5.0f)
	{
		for (int i = 0; i < MAXCOLORNUM; ++i)
		{
			mTargetColor[i] = Color::BLACK;
			mTargetTime[i] = 0.0f;
			mExposureMultiple[i] = 1.0f;
		}
	}

	SkillAttack::~SkillAttack()
	{
		unprepare();
	}

	void SkillAttack::copyAttributesTo(SkillAttack* another)
	{
		another->Init();

		another->mbFly = mbFly;
		if (another->mbFly)
		{
			another->mFlyEffectName = mFlyEffectName;
			another->mBeginNodeName = mBeginNodeName;
			another->mBeginOffset = mBeginOffset;
			another->mEndNodeName = mEndNodeName;
			another->mEndOffset = mEndOffset;
			another->mFlyScale = mFlyScale;
			another->mFlySpeed = mFlySpeed;
			another->mFlyTime = mFlyTime;
			another->mFlyType = mFlyType;
			another->mFlyDirection = mFlyDirection;
			another->mFlyStayTime = mFlyStayTime;
			another->mFlyDeltaHeight = mFlyDeltaHeight;
		}

		another->mbAttack = mbAttack;
		if (another->mbAttack)
		{
			another->mAttackEffectName = mAttackEffectName;
			another->mTargetSize = mTargetSize;
			another->mChangeSize = mChangeSize;
			another->mAttackNodeName = mAttackNodeName;
			another->mAttackOffset = mAttackOffset;
			another->mAttackScale = mAttackScale;
			another->mFallowTarget = mFallowTarget;
		}

		another->mbGround = mbGround;
		if (another->mbGround)
		{
			another->mGroundEffectName = mGroundEffectName;
			another->mGroundNodeName = mGroundNodeName;
			another->mGroundOffset = mGroundOffset;
			another->mGroundScale = mGroundScale;
			another->mGroundTarget = mGroundTarget;
		}

		another->mbChangeTargetColor = mbChangeTargetColor;
		if (another->mbChangeTargetColor)
		{
			another->mChangeColorNum = mChangeColorNum;
			another->mBeginColor = mBeginColor;
			for (int i = 0; i<MAXCOLORNUM; ++i)
			{
				another->mTargetColor[i] = mTargetColor[i];
				another->mTargetTime[i] = mTargetTime[i];
				another->mExposureMultiple[i] = mExposureMultiple[i];
			}
		}

		another->mbHasAttackedSound = mbHasAttackedSound;
		if (another->mbHasAttackedSound)
		{
			another->mAttackedSoundName = mAttackedSoundName;
			another->mfAttackedSoundVolume = mfAttackedSoundVolume;
			another->mbBroadcastAttackedSound = mbBroadcastAttackedSound;
			another->mfBroadcastAttackedSoundVolume = mfBroadcastAttackedSoundVolume;
			another->mfAttackedSoundMinDistance = mfAttackedSoundMinDistance;
			another->mfAttackedSoundMaxDistance = mfAttackedSoundMaxDistance;
		}

		another->mbHasFlySound = mbHasFlySound;
		if (another->mbHasFlySound)
		{
			another->mFlySoundName = mFlySoundName;
			another->mfFlySoundVolume = mfFlySoundVolume;
			another->mbBroadcastFlySound = mbBroadcastFlySound;
			another->mfBroadcastFlySoundVolume = mfBroadcastFlySoundVolume;
			another->mfFlySoundMinDistance = mfFlySoundMinDistance;
			another->mfFlySoundMaxDistance = mfAttackedSoundMaxDistance;
		}

		another->mbTargetCameraShake = mbTargetCameraShake;
		if (another->mbTargetCameraShake)
		{
			another->mTargetCameraShakeBeginTime = mTargetCameraShakeBeginTime;
			another->mTargetCameraShakeDuration = mTargetCameraShakeDuration;
			another->mTargetCameraShakeScale = mTargetCameraShakeScale;
		}

		another->mbTargetSlowDown = mbTargetSlowDown;
		if (another->mbTargetSlowDown)
		{
			another->mTargetSlowDownBeginTime = mTargetSlowDownBeginTime;
			another->mTargetSlowDownScale = mTargetSlowDownScale;
			another->mTargetSlowDownDurationTime = mTargetSlowDownDurationTime;
			another->mTargetSlowDownEqualizeTime = mTargetSlowDownEqualizeTime;
		}

		another->mbNeedReplaceATT = mbNeedReplaceATT;
		if (another->mbNeedReplaceATT)
		{
			another->mReplaceATTIndex = mReplaceATTIndex;
		}
	}

	bool SkillAttack::loadSkillAttack(const String& fileName)
	{
		try
		{
			DataStream* stream = ResourceGroupManager::Instance()->openResource(fileName);
			if (stream == NULL)
			{
				LordLogError("ActorTemplate file not found [%s].", fileName.c_str());
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
				char buf[MAX_PATH];
				sprintf_s(buf, "ActorTemplate file not found [%s].", fileName.c_str());
				MessageBox(0, buf, "Error", 0);
#endif
				return NULL;
			}

			loadSkillAttack(*stream);

			LordDelete stream;
			return true;
		}
		catch (...)
		{
			//LordException("%s can't found.", fileName.c_str());
			LordLogError("%s can't found.", fileName.c_str());

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			char buf[MAX_PATH];
			sprintf_s(buf, "%s can't found.", fileName.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return false;
		}
	}

	bool SkillAttack::loadSkillAttack(DataStream& stream)
	{
		uint len = 0;
		ActorManager* pActorManager = ActorManager::Instance();
		float attscale = pActorManager ? pActorManager->GetAttackScale() : 1.0f;

		ui32 version;
		stream.read(&version, sizeof(ui32));

		char szBuffer[256];
		stream.read((Byte*)&mbFly, sizeof(bool));
		if (mbFly)
		{
			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mFlyEffectName = szBuffer;

			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mBeginNodeName = szBuffer;

			stream.read(&mBeginOffset, sizeof(Vector3));

			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mEndNodeName = szBuffer;

			stream.read(&mEndOffset, sizeof(Vector3));

			stream.read(&mFlyScale, sizeof(Real)); mFlyScale *= attscale;
			stream.read(&mFlySpeed, sizeof(Real)); mFlySpeed *= attscale;
			stream.read(&mFlyTime, sizeof(Real));
			stream.read(&mFlyType, sizeof(uint));
			stream.read(&mFlyDirection, sizeof(uint));
			stream.read(&mFlyFade, sizeof(bool));

			if (version > 0x00010000)
			{
				stream.read(&mFlyStayTime, sizeof(float));
			}
			if (version >= 0x00010002)
			{
				stream.read(&mFlyDeltaHeight, sizeof(float));
			}
		}

		stream.read(&mbAttack, sizeof(bool));
		if (mbAttack)
		{
			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mAttackEffectName = szBuffer;

			stream.read(&mTargetSize, sizeof(float));
			stream.read(&mChangeSize, sizeof(bool));

			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mAttackNodeName = szBuffer;

			stream.read(&mAttackOffset, sizeof(Vector3));
			stream.read(&mAttackScale, sizeof(float)); mAttackScale *= attscale;
			stream.read(&mFallowTarget, sizeof(bool));
		}

		stream.read(&mbGround, sizeof(bool));
		if (mbGround)
		{
			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mGroundEffectName = szBuffer;

			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mGroundNodeName = szBuffer;

			stream.read(&mGroundOffset, sizeof(Vector3));
			stream.read(&mGroundScale, sizeof(Real)); mGroundScale *= attscale;
			stream.read(&mGroundTarget, sizeof(bool));
		}

		stream.read(&mbChangeTargetColor, sizeof(bool));
		if (mbChangeTargetColor)
		{
			stream.read(&mChangeColorNum, sizeof(unsigned int));
			stream.read(&mBeginColor, sizeof(Color));
			for (size_t i = 0; i < mChangeColorNum; ++i)
				stream.read(mTargetColor + i, sizeof(Color));
			for (size_t i = 0; i < mChangeColorNum; ++i)
				stream.read(mTargetTime + i, sizeof(Real));
			for (size_t i = 0; i < mChangeColorNum; ++i)
				stream.read(mExposureMultiple + i, sizeof(Real));
		}

		stream.read(&mbHasAttackedSound, sizeof(bool));
		if (mbHasAttackedSound)
		{
			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mAttackedSoundName = StringUtil::ParseInt(szBuffer);

			stream.read(&mfAttackedSoundVolume, sizeof(float));
			stream.read(&mbBroadcastAttackedSound, sizeof(bool));
			stream.read(&mfBroadcastAttackedSoundVolume, sizeof(float));
			stream.read(&mfAttackedSoundMinDistance, sizeof(Real));
			stream.read(&mfAttackedSoundMaxDistance, sizeof(Real));
		}

		stream.read(&mbHasFlySound, sizeof(bool));
		if (mbHasFlySound)
		{
			stream.read((Byte*)&len, sizeof(uint));
			stream.read(&szBuffer, len);
			szBuffer[len] = 0;
			mFlySoundName = StringUtil::ParseInt(szBuffer);

			stream.read(&mfFlySoundVolume, sizeof(float));
			stream.read(&mbBroadcastFlySound, sizeof(bool));
			stream.read(&mfBroadcastFlySoundVolume, sizeof(Real));
			stream.read(&mfFlySoundMinDistance, sizeof(Real));
			stream.read(&mfFlySoundMaxDistance, sizeof(Real));
		}

		stream.read(&mbTargetCameraShake, sizeof(bool));
		if (mbTargetCameraShake)
		{
			stream.read(&mTargetCameraShakeBeginTime, sizeof(float));
			stream.read(&mTargetCameraShakeDuration, sizeof(Real));
			stream.read(&mTargetCameraShakeScale, sizeof(Real));
		}

		stream.read(&mbTargetSlowDown, sizeof(bool));
		if (mbTargetSlowDown)
		{
			stream.read(&mTargetSlowDownScale, sizeof(float));
			stream.read(&mTargetSlowDownBeginTime, sizeof(Real));
			stream.read(&mTargetSlowDownDurationTime, sizeof(Real));
			stream.read(&mTargetSlowDownEqualizeTime, sizeof(Real));
		}

		stream.read(&mbNeedReplaceATT, sizeof(bool));
		if (mbNeedReplaceATT)
		{
			stream.read(&mReplaceATTIndex, sizeof(int));
		}

		return true;
	}

	void SkillAttack::setTargetActor(ActorObject* targetActor)
	{
		mpTargetActor = targetActor;
	}

	void SkillAttack::setFlyingTime(Real flyTime)
	{
		if (mbFly)
		{
			mFlyTime = flyTime;
		}
	}

	void SkillAttack::setSkillMiss(bool miss)
	{
		mbMissed = miss;
	}

	void SkillAttack::prepare(ActorObject* pOriginalActor)
	{
		LordAssert(pOriginalActor);
		pOriginalActor->UpdateLogicState();

		if (mpTargetActor)
		{
			mpTargetActor->UpdateLogicState();
		}

		SceneNode* pSceneRoot = SceneManager::Instance()->getRootNode();
		mOriginalActorPos = pOriginalActor->GetPosition();
		mpOriginalActor = pOriginalActor;
		if (mbFly)
		{
			assert(mFlyEffectName != StringUtil::BLANK);
			assert(mBeginNodeName != StringUtil::BLANK);
			assert(mEndNodeName != StringUtil::BLANK);
			if (mbDirectivity)
				assert(mpTargetActor);


			SceneNode* pNode = pOriginalActor->getSocketSceneNode(mBeginNodeName);
			if (!pNode)
			{
				mBeginPosition = mOriginalActorPos + LORD::Vector3::UNIT_Y*1.0f;
			}
			else
			{
				mBeginPosition = pNode->getWorldPosition();
			}

			if (mbDirectivity)
			{
				mEndNode = mpTargetActor->getSocketSceneNode(mEndNodeName);
			}
			else
			{
				mEndNode = getUnDirectivityNode();
			}
			Vector3 EndPositon;
			if (!mEndNode)
			{
				EndPositon = mpTargetActor->GetSceneNodeMaster()->getWorldPosition() + LORD::Vector3::UNIT_Y*1.0f;
			}
			else
			{
				EndPositon = mEndNode->getWorldPosition();
			}
			Real dist = (mBeginPosition - EndPositon).len();
			mInnerFlyTime = dist / mFlySpeed;

			String flySkillName = getSkillSceneNodeName();
			mpFlySceneNode = pSceneRoot->createChild();
			assert(mpFlySceneNode);

			mpFlyEffectSystem = EffectSystemManager::Instance()->createEffectSystem(pOriginalActor->GetNextEffectName(), mFlyEffectName);//LordNew EffectSystem(mFlyEffectName);
			mpFlyEffectSystem->setParentSceneNode(mpFlySceneNode);
			mpFlySceneNode->setLocalScaling(Vector3(mFlyScale, mFlyScale, mFlyScale));
			mpFlySceneNode->setLocalOrientation(pOriginalActor->GetOrientation());
			mpFlySceneNode->update();
			mpFlyEffectSystem->prepare_sys();
			mpFlyEffectSystem->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
			mCurrentFlyTime = 0;


			/*if (mbHasFlySound)
			{
			SoundSystem::getSingletonPtr()->addSoundSource(mFlySoundName);
			}*/
		}

		if (mbAttack && !mbMissed)
		{
			//assert(mAttackEffectName!=StringUtil::BLANK);
			//assert(mAttackNodeName!=StringUtil::BLANK);
			if (mbDirectivity)
				assert(mpTargetActor);

			if (mAttackNodeName != StringUtil::BLANK)
			{
				if (mbDirectivity)
					mAttackNode = mpTargetActor->getSocketSceneNode(mAttackNodeName);
				else
					mAttackNode = getUnDirectivityNode();
			}

			if (mAttackNode == NULL)
			{
				mAttackNode = mpTargetActor->getSocketSceneNode("bip01");
				assert(mAttackNode);
			}
			String attackSkillName = getSkillSceneNodeName();

			if (mAttackEffectName != StringUtil::BLANK)
			{
				mpAttackSceneNode = pSceneRoot->createChild();
				assert(mpAttackSceneNode);
				mpAttackEffectSystem = EffectSystemManager::Instance()->createEffectSystem(pOriginalActor->GetNextEffectName(), mAttackEffectName);//LordNew EffectSystem(mAttackEffectName);
				mpAttackEffectSystem->setParentSceneNode(mpAttackSceneNode);
			}
			float tscale = mAttackScale;

			if (mChangeSize && mbDirectivity)
			{
				//Vec3 targetSize = mpTargetActor->GetDefaultActorSize();
				//Real maxSize = std::max(targetSize.x, targetSize.y);
				//maxSize = std::max(targetSize.z, maxSize);
				//tscale *= maxSize / mTargetSize;
				tscale *= mpTargetActor->GetAttackScale() / mTargetSize;
			}
			if (mpAttackSceneNode)
			{
				mpAttackSceneNode->setLocalScaling(Vector3(tscale, tscale, tscale));
				mpAttackSceneNode->update();
			}
			if (mpAttackEffectSystem)
			{
				mpAttackEffectSystem->prepare_sys();
				mpAttackEffectSystem->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
			}
		}

		if (mbGround && !mbMissed)
		{
			assert(mGroundEffectName != StringUtil::BLANK);
			assert(mGroundNodeName != StringUtil::BLANK);
			if (mbDirectivity)
				assert(mpTargetActor);

			if (mbDirectivity)
				mGroundNode = mpTargetActor->getSocketSceneNode(mGroundNodeName);
			else
				mGroundNode = getUnDirectivityNode();

			if (mGroundNode == NULL)
			{
				mGroundNode = mpTargetActor->getSocketSceneNode("bip01");
				assert(mGroundNode);
			}

			String groundSkillNmae = getSkillSceneNodeName();
			mpGroundSceneNode = pSceneRoot->createChild();
			assert(mpGroundSceneNode);

			mpGroundEffectSystem = EffectSystemManager::Instance()->createEffectSystem(mpOriginalActor->GetNextEffectName(), mGroundEffectName);//LordNew EffectSystem(mGroundEffectName);
			mpGroundEffectSystem->setParentSceneNode(mpGroundSceneNode);
			mpGroundSceneNode->setLocalScaling(Vector3(mGroundScale, mGroundScale, mGroundScale));
			mpGroundSceneNode->update();
			mpGroundEffectSystem->prepare_sys();
			mpGroundEffectSystem->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
		}

		/*if (!mbMissed && mbHasAttackedSound)
		{
		SoundSystem::getSingletonPtr()->addSoundSource(mAttackedSoundName);
		for (int i=0; i < MAXCOMMBO; ++i)
		{
		mbAttackedSoundPlayed[i] = false;
		}
		}*/
		if (!mbMissed && mbHasAttackedSound)
		{
			mbAttackedSoundPlayed = false;
		}
	}

	void SkillAttack::unprepare()
	{
		SceneManager* pManager = SceneManager::Instance();
		SceneNode* pSceneRoot = pManager ? pManager->getRootNode() : NULL;

		if (mpUnDirectivityNode)
		{
			pSceneRoot->destroyChild(mpUnDirectivityNode);
			mpUnDirectivityNode = NULL;
		}

		if (mbFly)
		{
			if (mpFlyEffectSystem)
			{
				mpFlyEffectSystem->stop();
				mpFlyEffectSystem->unprepare_sys();
				if (mpFlySceneNode)
				{
					LORD::EffectSystemManager::Instance()->destroyEffectSystem(mpFlyEffectSystem);
					mpFlyEffectSystem = NULL;
				}
			}

			if (mpFlySceneNode)
			{
				pSceneRoot->destroyChild(mpFlySceneNode);
				mpFlySceneNode = NULL;
			}

			mEndNode = NULL;
		}

		if (mbAttack && !mbMissed)
		{
			if (mpAttackEffectSystem)
			{
				mpAttackEffectSystem->stop();
				mpAttackEffectSystem->unprepare_sys();
				if (mpAttackSceneNode)
				{
					//mpAttackSceneNode[i]->detachObject(mpAttackEffectSystem[i]);
					LORD::EffectSystemManager::Instance()->destroyEffectSystem(mpAttackEffectSystem);
					mpAttackEffectSystem = NULL;
				}
			}

			if (mpAttackSceneNode)
			{
				pSceneRoot->destroyChild(mpAttackSceneNode);
				mpAttackSceneNode = NULL;
			}
		}

		if (mbGround && !mbMissed)
		{
			if (mpGroundEffectSystem)
			{
				mpGroundEffectSystem->stop();
				mpGroundEffectSystem->unprepare_sys();
				if (mpGroundSceneNode)
				{
					//mpGroundSceneNode[i]->detachObject(mpGroundEffectSystem[i]);
					LORD::EffectSystemManager::Instance()->destroyEffectSystem(mpGroundEffectSystem);
					mpGroundEffectSystem = NULL;
				}
			}

			if (mpGroundSceneNode)
			{
				pSceneRoot->destroyChild(mpGroundSceneNode);
				mpGroundSceneNode = NULL;
			}
		}
	}

	SkillAttack::SkillAttackState SkillAttack::getState()const
	{
		return mState;
	}

	void SkillAttack::play()
	{
		mbPause = false;
		if (mState == SAS_WAITTING || mState == SAS_STOPED)
		{
			mState = SAS_PLAYING;

			mCurrentFlyTime = 0;

			if (mbChangeTargetColor && !mbMissed)
			{
				mInnerChangeColorTime = 0.0f;
			}

			if (mbFly)
			{
				mpFlySceneNode->setLocalOrientation(mpOriginalActor->GetOrientation());
				mpFlySceneNode->update();

				mCurrentFlyState = CFS_FLYING;
				mBeginFlyingPosition = getBeginPosition();
				mOldFlyPosition = mBeginFlyingPosition;
				InitFlyParam();
				if (mbMissed)
					caculateMissPosition();
				updateFlyingPosition(0.0f);
				mpFlyEffectSystem->start();

				/*if (mbHasFlySound)
				{
				int id = EESoundSystem::getSingletonPtr()->playeSound(
				mFlySoundName
				, SOUND_TYPE_SKILL_SOUNDEFFECT
				, mOriginalActorPos
				, false, true, true);
				if (-1 != id)
				{
				EESoundSource* pFlySoundSource = (EESoundSource*)(INT_PTR)id;
				pFlySoundSource->setSoundVolume(mfFlySoundVolume);
				pFlySoundSource->setMinMaxDistance(mfFlySoundMinDistance, mfFlySoundMaxDistance);
				}
				}*/
				if (mbHasFlySound)
				{
					if (mpOriginalActor->isMainActor())
					{
						SoundSystem::Instance()->playEffectByType((SoundType)mFlySoundName);
						//SoundSystem::Instance()->setEffectsVolume(mfFlySoundVolume);
					}
					else if (mbBroadcastFlySound)
					{
						SoundSystem::Instance()->playEffectByType((SoundType)mFlySoundName);
						//SoundSystem::Instance()->setEffectsVolume(mfBroadcastFlySoundVolume);
					}

				}
			}
			else
			{
				mCurrentFlyState = CFS_ATTACK;

				if (!mbMissed && (mbAttack || mbGround) && mbPlayWoundAction && mbDirectivity)
				{
					if (mbIsCallBack)
						mpOriginalActor->PlayWoundAction(this);//mpTargetActor->PlayWoundAction();

					if (mbTargetCameraShake)
					{
						if (mbDirectivity && mpTargetActor->isMainActor())
						{
							mpTargetActor->AddCameraShake(mTargetCameraShakeBeginTime, mTargetCameraShakeScale, mTargetCameraShakeDuration, 1);
						}
						if (mpOriginalActor->isMainActor())
						{
							mpOriginalActor->AddCameraShake(mTargetCameraShakeBeginTime, mTargetCameraShakeScale, mTargetCameraShakeDuration, 1);
						}
					}

					if (mbTargetSlowDown && mbDirectivity)
					{
						mpTargetActor->AddSlowDown(mTargetSlowDownBeginTime, mTargetSlowDownScale, mTargetSlowDownDurationTime, mTargetSlowDownEqualizeTime);
					}
				}

				if (mbAttack && !mbMissed)
				{
					Vector3 attackPos = mAttackNode->getWorldPosition() + mAttackNode->getWorldOrientation() * mAttackOffset;
					if (mpAttackSceneNode)
					{
						mpAttackSceneNode->setLocalPosition(attackPos);
						mpAttackSceneNode->update();
					}
					if (mpAttackEffectSystem)
						mpAttackEffectSystem->start();
				}
				if (mbGround && !mbMissed)
				{
					Vector3 groundPos = mGroundNode->getWorldPosition() + mGroundNode->getWorldOrientation() * mGroundOffset;
					mpGroundSceneNode->setLocalPosition(groundPos);
					mpGroundSceneNode->update();
					mpGroundEffectSystem->start();
				}
			}

			Quaternion yawQuaternion = Quaternion::IDENTITY;
			if (!mbMissed && (mbAttack || mbGround))
			{
				Vector3 targetActorPos;
				if (mbDirectivity)
					targetActorPos = mpTargetActor->GetSceneNodeMaster()->getWorldPosition();
				else
					targetActorPos = getUnDirectivityNode()->getWorldPosition();

				Vector3 ori2tarVec = targetActorPos - mOriginalActorPos;
				ori2tarVec.y = 0;
				if (targetActorPos == mOriginalActorPos)
					ori2tarVec.x = 0.0001f;
				yawQuaternion = Vector3::UNIT_Z.getRotationTo(ori2tarVec, Vector3::UNIT_Y);
				mAttackGroundYaw = yawQuaternion;
			}

			if (mbAttack && !mbMissed)
			{
				if (mpAttackSceneNode)
				{
					mpAttackSceneNode->setLocalOrientation(yawQuaternion);
					mpAttackSceneNode->update();
				}
			}

			if (mbGround && !mbMissed)
			{
				mpGroundSceneNode->setLocalOrientation(yawQuaternion);
				mpGroundSceneNode->update();
			}
		}
	}

	void SkillAttack::pause()
	{
		mbPause = true;
		if (mpFlyEffectSystem) mpFlyEffectSystem->pause();
		if (mpAttackEffectSystem) mpAttackEffectSystem->pause();
		if (mpGroundEffectSystem) mpGroundEffectSystem->pause();
	}

	void SkillAttack::resume()
	{
		mbPause = false;
		if (mpFlyEffectSystem) mpFlyEffectSystem->resume();
		if (mpAttackEffectSystem) mpAttackEffectSystem->resume();
		if (mpGroundEffectSystem) mpGroundEffectSystem->resume();
	}

	void SkillAttack::stop()
	{
		mState = SAS_STOPED;
	}

	bool SkillAttack::IsPlaying() const
	{
		return mState == SAS_PLAYING;
	}

	bool SkillAttack::IsPause() const
	{
		return mbPause;
	}

	bool SkillAttack::IsStoped() const
	{
		return mState == SAS_STOPED;
	}

	Vector3 SkillAttack::getBeginPosition()
	{
		Vector3 result;
		switch (mFlyDirection)
		{
		case FD_SRCTODEST:
		case FD_ONLYSRC:
		case FD_SRCDOWN:
		case FD_SRCUP:
		case FD_SRCLINKDEST:
			result = mBeginPosition;
			result += mBeginOffset;
			break;
		case FD_ONLYDEST:
		case FD_DESTTOSRC:
		case FD_DESTDOWN:
		case FD_DESTUP:
		case FD_DESTLINKSRC:
		{
			if (mEndNode)
			{
				result = mEndNode->getWorldPosition();
			}
			else
			{
				result = mpTargetActor->GetSceneNodeMaster()->getWorldPosition() + Vector3::UNIT_Y*1.0f;
			}
			result += mEndOffset;
		}
		break;
		}

		return result;
	}

	Vector3 SkillAttack::getEndPosition()
	{
		Vector3 result;
		switch (mFlyDirection)
		{
		case FD_SRCTODEST:
		case FD_ONLYDEST:
		case FD_SRCLINKDEST:
		{
			if (mEndNode)
			{
				result = mEndNode->getWorldPosition();
			}
			else
			{
				result = mpTargetActor->GetSceneNodeMaster()->getWorldPosition() + Vector3::UNIT_Y*1.0f;
			}
			result += mEndOffset;
		}
		break;
		case FD_DESTTOSRC:
		case FD_ONLYSRC:
		case FD_DESTLINKSRC:
			result = mBeginPosition;
			result += mBeginOffset;
			break;
		case FD_DESTDOWN:
		{
			if (mEndNode)
			{
				result = mEndNode->getWorldPosition() - Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed;
			}
			else
			{
				result = mpTargetActor->GetSceneNodeMaster()->getWorldPosition() + Vector3::UNIT_Y*1.0f - Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed;
			}
		}
		break;
		case FD_DESTUP:
		{
			if (mEndNode)
			{
				result = mEndNode->getWorldPosition() + Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed;
			}
			else
			{
				result = mpTargetActor->GetSceneNodeMaster()->getWorldPosition() + Vector3::UNIT_Y*1.0f + Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed;
			}
		}
		break;
		case FD_SRCDOWN:
			result = mBeginPosition - Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed; break;
		case FD_SRCUP:
			result = mBeginPosition + Vector3::UNIT_Y*mInnerFlyTime*mFlySpeed; break;
		}
		return result;
	}

	void SkillAttack::InitFlyParam()
	{
		switch (mFlyType)
		{
		case FLT_PARABOLA:
			mFlyParam[0] = mFlyDeltaHeight;// Math::IntervalRandom(3.0f, 5.0f);
			mFlyParam[1] = -mFlyParam[0] * 2.0f / mInnerFlyTime;
			break;
		case FLT_MISSILE:
		{
			mFlyParam[0] = Math::IntervalRandom(4.0f, 6.0f);
			mFlyParam[1] = Math::IntervalRandom(0.2f, 0.4f);
			mFlyParam[2] = -mFlyParam[0] * 2.0f / (mFlyParam[1] * mInnerFlyTime);
			mFlyParam[3] = Math::IntervalRandom(3.0f, 4.5f);
			float dir = Math::UnitRandom();
			if (dir > 0.5f)
				mFlyParam[3] = -mFlyParam[3];
		}
		break;
		case FLT_SHOWER:
		{
			Vector3* pBegin = (Vector3*)mFlyParam;
			*pBegin = getEndPosition();
			Vector3 vbegin = getBeginPosition();
			*pBegin += (vbegin - *pBegin) / 3.0f;
			*pBegin += Vector3(0.1f, mFlySpeed * mInnerFlyTime, 0.1f);
		}
		break;
		case FLT_SPIRAL:
		{
			mFlyParam[0] = 0.0f;
			mFlyParam[1] = Math::IntervalRandom(0.2f, 0.5f);
			mFlyParam[2] = Math::IntervalRandom(12.0f, 20.0f);
		}
		break;
		case FLT_BANANA:
		{
			Vector3 dir = (getEndPosition() - getBeginPosition());
			Vector3 normal = dir.cross(Vector3::UNIT_Y);
			normal.normalize();
			float randomValue = Math::UnitRandom();
			if (randomValue > 0.5f)
				normal = -normal;
			Vector3* pNormal = (Vector3*)mFlyParam;
			*pNormal = normal;
			mFlyParam[3] = Math::IntervalRandom(3.0f, 5.0f);
			mFlyParam[4] = -mFlyParam[3] * 2.0f / mInnerFlyTime;
		}
		break;
		case FLT_LINK:
		{
			NameValuePairList params;
			params["BeginPosition"] = StringUtil::ToString(getBeginPosition());
			params["EndPosition"] = StringUtil::ToString(getEndPosition());
			mpFlyEffectSystem->setExtenalParam(getBeginPosition(), getEndPosition());
		}
		break;
		}
	}

	void SkillAttack::caculateMissPosition()
	{
		Vector3 mFrom = getBeginPosition();
		Vector3 mTo = getEndPosition();

		Vector3 dir = mTo - mFrom;
		dir.y = 0.0f;
		dir.normalize();
		float actorSize = mpTargetActor->GetAttackScale();
		mTo += dir*actorSize;

		//float len = Math::UnitRandom()*actorSize;
		Vector3 randomUnitY = Vector3::UNIT_Y*Math::UnitRandom()*actorSize;
		float angle(Math::IntervalRandom(0.0f, Math::PI_2));
		Quaternion oritation(dir, angle);
		mMissPosition = mTo + oritation*randomUnitY;
	}

	void SkillAttack::updateFlyingPosition(Real slerp)
	{
		Vector3 currentEndPos;
		if (mbMissed)
			currentEndPos = mMissPosition;
		else
			currentEndPos = getEndPosition();

		Vector3 flyingPosition;
		Quaternion q = Quaternion::IDENTITY;

		if (mFlyType == FLT_STRAIGHTLINE)
		{
			flyingPosition = mBeginFlyingPosition + (currentEndPos - mBeginFlyingPosition)*slerp;
			mpFlySceneNode->setLocalPosition(flyingPosition);
			q = mpOriginalActor->GetOrientation();
		}
		if (mFlyType == FLT_STRAIGHTLINE_NO_ROTAE)
		{
			flyingPosition = mBeginFlyingPosition + (currentEndPos - mBeginFlyingPosition)*slerp;
			mpFlySceneNode->setLocalPosition(flyingPosition);
			mpFlySceneNode->update();
			q = mpOriginalActor->GetOrientation();
			return;
		}
		else if (mFlyType == FLT_PARABOLA)
		{
			float currentTime = slerp * mInnerFlyTime;
			flyingPosition = mBeginFlyingPosition + (currentEndPos - mBeginFlyingPosition)*slerp;
			float deltaY = mFlyParam[0] * currentTime + 0.5f*mFlyParam[1] * currentTime*currentTime;
			flyingPosition.y += deltaY;
			mpFlySceneNode->setLocalPosition(flyingPosition);
			q = mpOriginalActor->GetOrientation();
		}
		else if (mFlyType == FLT_MISSILE)
		{
			Vector3 path = (currentEndPos - mBeginFlyingPosition).normalizedCopy();
			Vector3 normal = path.cross(Vector3::UNIT_Y);
			normal.normalize();
			float currentTime = slerp*mInnerFlyTime;
			flyingPosition = mBeginFlyingPosition;
			if (slerp < mFlyParam[1])
			{
				float zdis = mFlyParam[0] * currentTime + 0.5f*mFlyParam[2] * currentTime*currentTime;
				float xdis = mFlyParam[3] * currentTime;
				flyingPosition += normal*xdis;
				flyingPosition += path * (-zdis);
			}
			else
			{
				float xdis = mFlyParam[3] * mInnerFlyTime*mFlyParam[1];
				Vector3 vSeg2 = mBeginFlyingPosition + normal*xdis;
				path = currentEndPos - vSeg2;
				float slerp2 = (slerp - mFlyParam[1]) / (1.0f - mFlyParam[1]);
				flyingPosition = vSeg2 + path*slerp2;
			}
			mpFlySceneNode->setLocalPosition(flyingPosition);
			q = mpOriginalActor->GetOrientation();
		}
		else if (mFlyType == FLT_SHOWER)
		{
			Vector3 *vShowerBegin = (Vector3*)mFlyParam;
			flyingPosition = *vShowerBegin + (currentEndPos - *vShowerBegin)*slerp;
			mpFlySceneNode->setLocalPosition(flyingPosition);
		}
		else if (mFlyType == FLT_SPIRAL)
		{
			float radio = mFlyParam[1];
			if (slerp < 0.1f)
				radio = radio*slerp*10.0f;
			if (slerp > 0.9f)
				radio = radio*(1.0f - slerp)*10.0f;
			mFlyParam[0] = mFlyParam[2] * slerp*mInnerFlyTime;
			flyingPosition = mBeginFlyingPosition + (currentEndPos - mBeginFlyingPosition)*slerp;

			Vector3 direction = (currentEndPos - flyingPosition).normalizedCopy();
			Quaternion oritation(direction, (mFlyParam[0]));
			Vector3 offset;
			if (direction.x == 0.0f && direction.z == 0.0f)
				offset = oritation * Vector3::UNIT_X;
			else
				offset = oritation * Vector3::UNIT_Y;
			offset *= radio;
			flyingPosition += offset;
			mpFlySceneNode->setLocalPosition(flyingPosition);
			q = mpOriginalActor->GetOrientation();
		}
		else if (mFlyType == FLT_BANANA)
		{
			float currentTime = slerp * mInnerFlyTime;
			flyingPosition = mBeginFlyingPosition + (currentEndPos - mBeginFlyingPosition)*slerp;
			float deltaY = mFlyParam[3] * currentTime + 0.5f*mFlyParam[4] * currentTime*currentTime;
			Vector3* pNormal = (Vector3*)mFlyParam;
			flyingPosition += (*pNormal)*deltaY;
			mpFlySceneNode->setLocalPosition(flyingPosition);
			q = mpOriginalActor->GetOrientation();
		}
		else if (mFlyType == FLT_LINK)
		{
			NameValuePairList params;
			params["BeginPosition"] = StringUtil::ToString(getBeginPosition());
			params["EndPosition"] = StringUtil::ToString(currentEndPos);
			mpFlyEffectSystem->setExtenalParam(getBeginPosition(), currentEndPos);
			if (mbIsCallBack)
			{
				mpOriginalActor->PlayWoundAction(this);//mpTargetActor->PlayWoundAction();
				mbIsCallBack = false;
			}
		}

		if ((flyingPosition - mOldFlyPosition).lenSqr() > 0)
		{
			Vector3 direction = flyingPosition - mOldFlyPosition;
			mOldFlyPosition = flyingPosition;
			direction.normalize();
			Vector3 tang = direction.cross(-Vector3::UNIT_Y);
			float radian = Math::ACos(Vector3::UNIT_Y.dot(direction));
			Quaternion quat(tang, radian);

			mpFlySceneNode->setLocalOrientation(quat * q);
		}
		mpFlySceneNode->update();
	}

	bool SkillAttack::update(unsigned int tickTime)
	{
		if (mState == SAS_WAITTING)
			return false;

		if (mbPause)
		{
			if (mpFlyEffectSystem && !mpFlyEffectSystem->isStoped())
			{
				mpFlyEffectSystem->_updateRenderQueue();
			}
			if (mpAttackEffectSystem && !mpAttackEffectSystem->isStoped())
			{
				mpAttackEffectSystem->_updateRenderQueue();
			}
			if (mpGroundEffectSystem && !mpGroundEffectSystem->isStoped())
			{
				mpGroundEffectSystem->_updateRenderQueue();
			}
			return false;
		}

		if (mState == SAS_PLAYING)
		{
			if (mbFly)
			{
				mCurrentFlyTime += (float)tickTime*0.001f;
				if (mCurrentFlyTime > 0 && mCurrentFlyState == CFS_UNLAUNCH)
				{
					mCurrentFlyState = CFS_FLYING;
					mBeginFlyingPosition = getBeginPosition();
					InitFlyParam();
					if (mbMissed)
						caculateMissPosition();
					mpFlyEffectSystem->start();
				}
				bool notLinkStay = mFlyStayTime <= 0.0f || mFlyType != FLT_LINK;
				if (mCurrentFlyTime > mInnerFlyTime && mCurrentFlyState == CFS_FLYING)
				{
					if (notLinkStay)
						mpFlyEffectSystem->stop();

					mCurrentFlyState = CFS_ATTACK;

					if (!mbMissed && (mbAttack || mbGround) && mbPlayWoundAction && mbDirectivity)
					{
						if (mbIsCallBack)
							mpOriginalActor->PlayWoundAction(this);//mpTargetActor->PlayWoundAction();

						if (mbTargetCameraShake)
						{
							if (mbDirectivity && mpTargetActor->isMainActor())
							{
								mpTargetActor->AddCameraShake(mTargetCameraShakeBeginTime, mTargetCameraShakeScale, mTargetCameraShakeDuration, 1);
							}
							if (mpOriginalActor->isMainActor())
							{
								mpOriginalActor->AddCameraShake(mTargetCameraShakeBeginTime, mTargetCameraShakeScale, mTargetCameraShakeDuration, 1);
							}
						}
						if (mbTargetSlowDown && mbDirectivity)
						{
							mpTargetActor->AddSlowDown(mTargetSlowDownBeginTime, mTargetSlowDownScale, mTargetSlowDownDurationTime, mTargetSlowDownEqualizeTime);
						}

						Vector3 targetActorPos;
						if (mbDirectivity)
							targetActorPos = mpTargetActor->GetSceneNodeMaster()->getWorldPosition();
						else
							targetActorPos = mEndNode->getWorldPosition();

						Vector3 ori2tarVec = targetActorPos - mOriginalActorPos;
						ori2tarVec.y = 0;
						if (targetActorPos == mOriginalActorPos)
							ori2tarVec.x = 0.0001f;
						mAttackGroundYaw = Vector3::UNIT_Z.getRotationTo(ori2tarVec, Vector3::UNIT_Y);
					}

					if (mbAttack && !mbMissed)
					{
						Vector3 attackPos = mAttackNode->getWorldPosition() + mAttackNode->getWorldOrientation() * mAttackOffset;
						if (mpAttackSceneNode)
						{
							mpAttackSceneNode->setLocalPosition(attackPos);
							mpAttackSceneNode->setLocalOrientation(mAttackGroundYaw);
							mpAttackSceneNode->update();
						}
						if (mpAttackEffectSystem)
							mpAttackEffectSystem->start();
					}
					if (mbGround && !mbMissed)
					{
						Vector3 groundPos = mGroundNode->getWorldPosition() + mGroundNode->getWorldOrientation() * mGroundOffset;
						mpGroundSceneNode->setLocalPosition(groundPos);
						mpGroundSceneNode->setLocalOrientation(mAttackGroundYaw);
						mpGroundSceneNode->update();
						mpGroundEffectSystem->start();
					}
					if (mbChangeTargetColor && !mbMissed)
					{
						mInnerChangeColorTime = 0.0f;
					}
				}

				if (mCurrentFlyState == CFS_FLYING)
				{
					float flyTime = mCurrentFlyTime;
					if (flyTime < 0.0f)
						flyTime = 0.0f;
					updateFlyingPosition(flyTime / mInnerFlyTime);
				}

				if (!notLinkStay)
					if (mCurrentFlyTime > mFlyStayTime)
						mpFlyEffectSystem->stop();

				if (!mpFlyEffectSystem->isStoped())
				{
					mpFlyEffectSystem->_update_sys(tickTime);
					mpFlyEffectSystem->_updateRenderQueue();
				}
			}

			int stopedCount = 0;

			if (mCurrentFlyState == CFS_ATTACK)
			{
				bool stoped = true;

				if (mbAttack && !mbMissed && mbDirectivity)
				{
					if (mpAttackEffectSystem)
					{
						mpAttackEffectSystem->_update_sys((i32)(tickTime));
						mpAttackEffectSystem->_updateRenderQueue();

						if (mFallowTarget)
						{
							Vector3 attackPos = mAttackNode->getWorldPosition() + mAttackNode->getWorldOrientation() * mAttackOffset;
							if (mpAttackSceneNode)
							{
								mpAttackSceneNode->setLocalPosition(attackPos);
								mpAttackSceneNode->update();
							}
						}
						if (mpAttackEffectSystem->isPlaying())
							stoped = false;
					}
				}
				if (mbGround && !mbMissed && mbDirectivity)
				{
					mpGroundEffectSystem->_update_sys((i32)(tickTime));
					mpGroundEffectSystem->_updateRenderQueue();
					if (mGroundTarget)
					{
						Vector3 groundPos = mGroundNode->getWorldPosition() + mGroundNode->getWorldOrientation() * mGroundOffset;
						mpGroundSceneNode->setLocalPosition(groundPos);
						mpGroundSceneNode->update();
					}
					mpGroundSceneNode->setLocalOrientation(mAttackGroundYaw);
					mpGroundSceneNode->update();
					if (mpGroundEffectSystem->isPlaying())
						stoped = false;
				}
				if (mbChangeTargetColor && !mbMissed && mbDirectivity)
				{
					Real timetest = mInnerChangeColorTime;
					unsigned int i;
					//bool played = false;
					for (i = 0; i<mChangeColorNum; ++i)
					{
						if (timetest < mTargetTime[i])
							break;
						timetest -= mTargetTime[i];
					}
					if (i<mChangeColorNum)
					{
						stoped = false;
						Color begin, end;
						if (i == 0)
							begin = mBeginColor;
						else
							begin = mTargetColor[i - 1];
						end = mTargetColor[i];
						Real slerp = timetest / mTargetTime[i];
						Color targetColor = (end - begin)*slerp + begin;
						targetColor *= mExposureMultiple[i];
						mpTargetActor->SetNaturalColor(targetColor, true, 50, true);
					}
					mInnerChangeColorTime += tickTime;
				}
				if (!mbMissed && mbHasAttackedSound && !mbAttackedSoundPlayed && mbDirectivity)
				{
					if (mpTargetActor->isMainActor())
					{
						SoundSystem::Instance()->playEffectByType((SoundType)mAttackedSoundName);
						//SoundSystem::Instance()->setEffectsVolume(mfAttackedSoundVolume);
					}
					else if (mbBroadcastAttackedSound)
					{
						SoundSystem::Instance()->playEffectByType((SoundType)mAttackedSoundName);
						//SoundSystem::Instance()->setEffectsVolume(mfBroadcastAttackedSoundVolume);
					}

					mbAttackedSoundPlayed = true;
				}
				if (stoped)
				{
					mCurrentFlyState = CFS_STOPED;
					stopedCount++;
				}
			}
			else if (mCurrentFlyState == CFS_STOPED)
				stopedCount++;

			if (stopedCount > 0)
				mState = SAS_STOPED;
		}
		return true;
	}

	SceneNode* SkillAttack::getUnDirectivityNode()
	{
		if (!mpUnDirectivityNode)
		{
			/*mpUnDirectivityNode = SceneManager::Instance()->getRootNode()->createChild();
			Quaternion q = mpOriginalActor->GetOrientation();
			float x,y,z;
			q.toEulerAngle(x,y,z);

			y = Math::Tan(y);
			z = UNDIRECTIVITY_DISTANCE / Math::Sqr(y + 1);
			x = UNDIRECTIVITY_DISTANCE / Math::Sqr(1/y + 1);

			Vector3 pos = mpOriginalActor->GetPosition();
			pos.x += x;
			pos.z += z;

			mpUnDirectivityNode->setWorldPosition(pos);
			mpUnDirectivityNode->update();*/

			SceneNode* pNode = mpOriginalActor->GetSceneNodeMaster()->createChild();
			pNode->setLocalPosition(Vector3(0.0f, 0.0f, UNDIRECTIVITY_DISTANCE));
			pNode->update();

			mpUnDirectivityNode = SceneManager::Instance()->getRootNode()->createChild();
			mpUnDirectivityNode->setWorldPosition(pNode->getWorldPosition());
			mpUnDirectivityNode->update();
			mpOriginalActor->GetSceneNodeMaster()->destroyChild(pNode);
		}

		return mpUnDirectivityNode;
	}

	void SkillAttack::Init()
	{
		unprepare();

		mOriginalActorPos = Vector3::ZERO;
		mpTargetActor = NULL;
		mpOriginalActor = NULL;
		mbFly = false;
		mBeginPosition = Vector3::ZERO;
		mBeginOffset = Vector3::ZERO;
		mEndNode = NULL;
		mEndOffset = Vector3::ZERO;
		mFlyScale = 1.0f;
		mFlySpeed = 20.0f;
		mFlyTime = 2.0f;
		mFlyType = FLT_STRAIGHTLINE;
		mFlyDirection = FD_SRCTODEST;
		mbAttack = false;
		mTargetSize = 1.6f;
		mChangeSize = true;
		mAttackNode = NULL;
		mAttackOffset = Vector3::ZERO;
		mAttackScale = 1.0f;
		mFallowTarget = false;
		mbGround = false;
		mbChangeTargetColor = false;
		mGroundNode = NULL;
		mGroundOffset = Vector3::ZERO;
		mGroundScale = 1.0f;
		mGroundTarget = false;
		mState = SAS_WAITTING;
		mbMissed = false;
		mChangeColorNum = 0;
		mBeginColor = Color::BLACK;
		mbHasAttackedSound = false;
		mAttackedSoundName = 0;
		mfAttackedSoundVolume = 0.6f;
		mbBroadcastAttackedSound = false;
		mfBroadcastAttackedSoundVolume = 0.2f;
		mfAttackedSoundMinDistance = 2.0f;
		mfAttackedSoundMaxDistance = 5.0f;
		mbHasFlySound = false;
		mFlySoundName = 0;
		mfFlySoundVolume = 0.6f;
		mbBroadcastFlySound = false;
		mfBroadcastFlySoundVolume = 0.2f;
		mfFlySoundMinDistance = 2.0f;
		mfFlySoundMaxDistance = 5.0f;
		mTargetCameraShakeBeginTime = 0.0f;
		mTargetCameraShakeScale = 0.0f;
		mTargetCameraShakeDuration = 0.0f;
		mTargetSlowDownScale = 0.0f;
		mTargetSlowDownBeginTime = 0.0f;
		mTargetSlowDownDurationTime = 0.0f;
		mTargetSlowDownEqualizeTime = 0.0f;

		mpFlyEffectSystem = NULL;
		mpAttackEffectSystem = NULL;
		mpGroundEffectSystem = NULL;
		mpFlySceneNode = NULL;
		mpAttackSceneNode = NULL;
		mpGroundSceneNode = NULL;
		mCurrentFlyTime = 0.0f;
		mCurrentSlowDownTime = 0.0f;
		mCurrentCameraShakeTime = 0.0f;
		mCurrentFlyState = CFS_UNLAUNCH;
		mbAttackedSoundPlayed = false;
		mAttNameID = StringUtil::BLANK;
		mSkillName = StringUtil::BLANK;
		mbIsCallBack = true;
		mbDirectivity = true;
		mpUnDirectivityNode = NULL;

		mbNeedReplaceATT = false;
		mReplaceATTIndex = 0;
		mWoundSkill = "hit";

		mFlyStayTime = 0.0f;

		for (int i = 0; i < MAXCOLORNUM; ++i)
		{
			mTargetColor[i] = Color::BLACK;
			mTargetTime[i] = 0.0f;
			mExposureMultiple[i] = 1.0f;
		}
	}


	//--------------------------------------------------------------------
	SkillAttackManager* SkillAttackManager::Instance()
	{
		return ms_pSingleton;
	}

	SkillAttackManager::SkillAttackManager()
	{
		// nothing to do;
	}

	SkillAttackManager::~SkillAttackManager()
	{
		DelAllSkillAttackTemplate();
	}

	bool SkillAttackManager::AddSkillAttackTemplate(const String& attName)
	{
		SkillAttack* pAtt = LORD_NEW_T(SkillAttack);
		if (pAtt->loadSkillAttack(attName))
		{
			mSkillAttackMap[attName] = pAtt;
			return true;
		}

		LORD_DELETE_T(pAtt, SkillAttack);
		return false;
	}

	bool SkillAttackManager::DelSkillAttackTemplate(const String& attName)
	{
		SkillAttackItor it = mSkillAttackMap.find(attName);
		if (it != mSkillAttackMap.end())
		{
			LORD_DELETE_T(it->second, SkillAttack);
			mSkillAttackMap.erase(it);
			return true;
		}

		return false;
	}

	void SkillAttackManager::DelAllSkillAttackTemplate()
	{
		SkillAttackItor it = mSkillAttackMap.begin();
		for (; it != mSkillAttackMap.end(); ++it)
		{
			LORD_DELETE_T(it->second, SkillAttack);
		}

		mSkillAttackMap.clear();
	}

	SkillAttack* SkillAttackManager::createInstance(const String& attName)
	{
		SkillAttack* result = LordNew SkillAttack;

		if (attName == StringUtil::BLANK)
			return result;

		SkillAttackItor it = mSkillAttackMap.find(attName);
		if (it == mSkillAttackMap.end())
		{
			if (!AddSkillAttackTemplate(attName))
			{
				LordDelete result;
				return NULL;
			}
			else
			{
				it = mSkillAttackMap.find(attName);
			}
		}

		it->second->copyAttributesTo(result);
		return result;
	}

	void SkillAttackManager::destroyInstance(SkillAttack* att)
	{
		LORD_DELETE_T(att, SkillAttack);
	}
}