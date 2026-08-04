#include "Core.h"
#include "ActorObject.h"
#include "SkillSystem.h"
#include "SkillAttack.h"
#include "Effect/EffectSystem.h"
#include "Effect/EffectSystemManager.h"
#include "Object/Exception.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Audio/SoundSystem.h"
#include "Actor/ActorManager.h"
#include "Anim/AnimBlender.h"
#include "Scene/CameraAnimation.h"
#include "Scene/Scene.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

namespace LORD
{

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
	//-----------------------------------------------------------------------
	SkillSystem::SkillSystem()
		: m_pTemplateData(NULL)
		, m_pActorObject(NULL)
		, m_TimeLastSkillName(-1000)
		, mTextMoveMode(0)
		, mTextMoveModeDirty(false)
		, mCameraShakeEnable(false)
		, mCallBackNum(0)
		, mCurCallIndex(1)
		, mUserData(0)
		, mUserData2(0)
		, mLastAnimLength(-1)
		, mCurrentAnimation(NULL)
		, mNextAnimation(NULL)
	{

	}
	//-----------------------------------------------------------------------
	SkillSystem::~SkillSystem(void)
	{
		Free();
	}
	//-----------------------------------------------------------------------
	String SkillSystem::GetNextSkillName()
	{
		static int skill = 0;
		return StringUtil::Format("Skill_%d", ++skill);
	}
	//-----------------------------------------------------------------------
	void SkillSystem::Init(const ActorTemplateData* pTemplateData, ActorObject* pActorObject)
	{
		assert(pTemplateData && pActorObject);
		m_pActorObject = pActorObject;
		m_pTemplateData = pTemplateData;
		mAnimSystem.Init(pActorObject->GetSkeletonMaster(), pActorObject->getAnimationBlender());
	}
	//-----------------------------------------------------------------------
	void SkillSystem::Free()
	{
		StopSkill();
		mAnimSystem.Free();

		// 清空特效
		for (list<ActorEffect>::type::iterator it = m_effects.begin(); it != m_effects.end(); ++it)
		{
			it->pEffect->unprepare_sys();
			EffectSystemManager::Instance()->destroyEffectSystem(it->pEffect);
			it->pEffect = NULL;
			it->pEffectNode->getParent()->destroyChild(it->pEffectNode);
		}
		m_effects.clear();

		// 清空飞行击中信息
		m_Atts.clear();
	}
	//-----------------------------------------------------------------------
	void SkillSystem::PlayAnimation(const String &name, int nTime)
	{
		mAnimSystem.AddAnimation(name, nTime, ANIM_FADE_TIME, 1.0f);
	}
	//-----------------------------------------------------------------------
	bool SkillSystem::IsWoundActionEnable(void)
	{
		if (mCurSkillName.empty() == true)
		{
			return false;
		}

		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(mCurSkillName);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			if (it->second->bWoundEnable)
			{
				return true;
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------
	unsigned char SkillSystem::GetTextMoveMode(void)
	{
		// 不需更新时直接返回
		if (false == mTextMoveModeDirty)
		{
			return mTextMoveMode;
		}

		// 需要更新时查找后更新并返回，找不到则不更新
		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(mCurSkillName);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			mTextMoveMode = it->second->TextMoveMode;
		}

		mTextMoveModeDirty = false;
		return mTextMoveMode;
	}
	//-----------------------------------------------------------------------
	void SkillSystem::PlayUpperAnim(const String& name, bool bLoop /* = false */, float TimeScla /* = 1.f */)
	{
		auto it = m_pTemplateData->SkillsMap.find(name);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			for (auto itAnim : it->second->Animations)
			{
				ActorTemplateData::AnimationInfo* pAnim = itAnim.second;
				if (!pAnim)
					continue;
				mAnimSystem.AddUpperAnimation(pAnim->AnimName, bLoop, TimeScla);
				return;
			}
		}
	}
	//-----------------------------------------------------------------------
	int SkillSystem::PlaySkill(const String& name, bool bInsert, float TimeScale, int UserData, int UserData2, const String& actorNameArray, bool playsound)
	{
		mUserData = UserData;
		mUserData2 = UserData2;
		if (mCurSkillName == name && mCurSkillState.isLoop != -1000)
		{
			return mLastAnimLength;
		}
		// 判断技能特效是否需要停止
		for (list<ActorEffect>::type::iterator it = m_effects.begin(); it != m_effects.end(); ++it)
		{
			if (false == it->pData->Persist)
			{
				it->bFadeOut = true;
			}
		}

		// 进行播放技能的处理
		int animLength = 0;
		mCurSkillState.mCurSkillTimeScale = TimeScale;
		//m_pActorObject->m_isFrozen = false;
		if (mCurSkillState.isChangeActorScale)
		{
			float cs = mCurSkillState.lastActorScale;
			m_pActorObject->SetScale(Vector3(cs, cs, cs));
			mCurSkillState.isChangeActorScale = false;
		}

		if (mCurSkillState.isPauseActor)
		{
			mCurSkillState.isPauseActor = false;
		}

		if (mCurSkillState.isHideActor)
		{
			mCurSkillState.isHideActor = false;
			m_pActorObject->m_isSkillHideActor = false;
		}

		if (mCurSkillState.isPlaySkillBreakCallBlack || mCurSkillState.isUpdateSkillBreakCallBlack)
		{
			mCurSkillState.isSkillBreakChangeSkill = true;
			StopSkill();
		}

		if (!bInsert)
		{
			_stopSkillElements(true);
		}

		// 添加到待处理列表
		assert(m_pTemplateData);
		map<String, ActorTemplateData::Skill*>::type::const_iterator it;
		it = m_pTemplateData->SkillsMap.find(name);
		if (it != m_pTemplateData->SkillsMap.end())
		{
			ui32 len;
			ElementInfo element;

			// 动画
			element.type = eAnimation;
			map<String, ActorTemplateData::AnimationInfo*>::type::iterator itAnim;
			for (itAnim = it->second->Animations.begin(); itAnim != it->second->Animations.end(); ++itAnim)
			{
				ActorTemplateData::AnimationInfo* pAnim = itAnim->second;
				pAnim->skillName = name;
				pAnim->pSetScale = it->second->pSetScale;
				element.WaitingTime = (int)(pAnim->beginTime / mCurSkillState.mCurSkillTimeScale);
				element.pData = pAnim;
				m_ElementsInfo.push_back(element);

				if (pAnim->times <= 0)
				{
					// 如果有循环动画，则动画长度返回-1
					//m_pActorObject->m_pAnimBlender->setIsLoopSkill(true);
					mCurSkillState.isLoop = int(pAnim->Length / pAnim->TimeScale / mCurSkillState.mCurSkillTimeScale);

					int DurationTime = pAnim->beginTime + ui32((float)pAnim->Length*pAnim->times*-1 / pAnim->TimeScale / mCurSkillState.mCurSkillTimeScale);

					if (animLength < DurationTime)
					{
						animLength = DurationTime;
					}
				}
				else if (animLength > -1)
				{
					//m_pActorObject->m_pAnimBlender->setIsLoopSkill(false);
					// 否则计算每个动画的结束时间
					int DurationTime = pAnim->beginTime + ui32((float)pAnim->Length*pAnim->times / pAnim->TimeScale / mCurSkillState.mCurSkillTimeScale);

					if (animLength < DurationTime)
					{
						animLength = DurationTime;
					}
				}
			}

			// 判断是否由用户定义的技能时间替换
			if (it->second->UserDefineTime > -1.5f)
			{
				animLength = int(it->second->UserDefineTime*1000.0f);
			}

			//// 记录上个技能及名称恢复时间
			//if (true == bSkillNameNeedRecover)
			//{
			//	if (mLastSkillName.empty() == true)
			//	{
			//		mLastSkillName = mCurSkillName;
			//	}

			//	if (animLength < 0)
			//	{
			//		m_TimeLastSkillName = -1000;
			//	}
			//	else
			//	{
			//		m_TimeLastSkillName = animLength;
			//	}
			//}
			//else
			//{
			//	// 清空上个技能
			//	mLastSkillName.clear();
			//	m_TimeLastSkillName = -1000;
			//}

			// 设置当前的技能名字
			mCurSkillName = name;

			// 提示头顶字运动模式要更新
			mTextMoveModeDirty = true;

			// 特效
			element.type = eEffect;
			len = (ui32)it->second->ActorEffects.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->ActorEffects[i]->beginTime * 1000 / mCurSkillState.mCurSkillTimeScale);
				element.pData = it->second->ActorEffects[i];
				m_ElementsInfo.push_back(element);
			}

			// 声音
			if (playsound)
			{
				element.type = eSound;
				len = (ui32)it->second->Sounds.size();
				for (ui32 i = 0; i < len; ++i)
				{
					element.WaitingTime = (ui32)(it->second->Sounds[i]->beginTime * 1000);
					element.pData = it->second->Sounds[i];
					m_ElementsInfo.push_back(element);
				}
			}

			// 颜色改变
			element.type = eColorChange;
			len = (ui32)it->second->ColorChanges.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->ColorChanges[i]->beginTime * 1000);
				element.pData = it->second->ColorChanges[i];
				m_ElementsInfo.push_back(element);
			}
			// 顿帧
			element.type = eSlowDown;
			len = (ui32)it->second->SlowDowns.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->SlowDowns[i]->beginTime * 1000 / mCurSkillState.mCurSkillTimeScale);
				element.pData = it->second->SlowDowns[i];
				m_ElementsInfo.push_back(element);
			}

			// 击飞腾空
			element.type = eHitFly;
			len = (ui32)it->second->HitFlys.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->HitFlys[i]->beginTime * 1000);
				element.pData = it->second->HitFlys[i];
				m_ElementsInfo.push_back(element);
			}

			// 技能断点
			element.type = eSkillBreak;
			len = (ui32)it->second->SkillBreaks.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->SkillBreaks[i]->beginTime * 1000);
				element.pData = it->second->SkillBreaks[i];
				m_ElementsInfo.push_back(element);
			}

			// 摄像机拉近拉远
			element.type = eCameraPush;
			len = (ui32)it->second->CameraPushs.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->CameraPushs[i]->beginTime * 1000);
				element.pData = it->second->CameraPushs[i];
				m_ElementsInfo.push_back(element);
			}

			// 角色缩放
			element.type = eActorScale;
			len = (ui32)it->second->ActorScales.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->ActorScales[i]->beginTime * 1000);
				element.pData = it->second->ActorScales[i];
				m_ElementsInfo.push_back(element);
			}

			// 相机振动
			element.type = eCameraShake;
			len = (ui32)it->second->CameraShakes.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->CameraShakes[i]->beginTime * 1000);
				element.pData = it->second->CameraShakes[i];
				m_ElementsInfo.push_back(element);
			}

			// 攻击事件
			mCallBackNum = 0;
			mCurCallIndex = 1;
			element.type = eAttackEvent;
			len = (ui32)it->second->AttackEvents.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->AttackEvents[i]->beginTime * 1000 / mCurSkillState.mCurSkillTimeScale);
				// 击中的时间不能小于0.001秒
				if (element.WaitingTime < 0)
				{
					element.WaitingTime = 1;
				}
				element.pData = it->second->AttackEvents[i];
				if (it->second->AttackEvents[i]->isCallback)
					mCallBackNum += it->second->AttackEvents[i]->times;
				m_ElementsInfo.push_back(element);
			}

			// 摄像机动画
			if (actorNameArray != StringUtil::BLANK)
			{
				mCurrentAnimation = mNextAnimation = NULL;
				element.type = eCameraAnim;
				len = (ui32)it->second->CameraAnims.size();

				for (ui32 i = 0; i < len; ++i)
				{
					String filename = it->second->CameraAnims[i]->animName;
					ui32 index = (ui32)filename.find_last_of(".");
					String name = filename.substr(0, index);
					String ext = filename.substr(index + 1, filename.length());
					CameraAnimation* pCameraAnimation = LordNew CameraAnimation(name);
					if (pCameraAnimation)
					{
						DataStream* stream = ResourceGroupManager::Instance()->openResource(filename);
						if (stream)
						{
							element.WaitingTime = (ui32)(it->second->CameraAnims[i]->beginTime * 1000 / mCurSkillState.mCurSkillTimeScale);
							element.pData = &(it->second->CameraAnims[i]->animName);
							m_ElementsInfo.push_back(element);

							pCameraAnimation->setTargetActor(m_pActorObject);
							pCameraAnimation->SetVisibleActor(actorNameArray);

							if (ext == "camAnim")
							{
								pCameraAnimation->importData(*stream);
							}
							else
							{
								size_t fileSize = stream->size();
								char* xmlData = (char*)LordMalloc(fileSize + 1);
								stream->read(xmlData, fileSize);
								xmlData[fileSize] = 0;

								rapidxml::xml_document<>* doc = LordNewT(rapidxml::xml_document<>);        // character type defaults to cha
								doc->parse<0>(xmlData);
								pCameraAnimation->importData(doc->first_node());
								LordDeleteT(doc, xml_document<>);
								LordFree(xmlData);
							}
							pCameraAnimation->_prepare_obj();
							pCameraAnimation->_prepare_editor();
							pCameraAnimation->refleshPath();
							mCameraAnimations.insert(std::make_pair(filename, pCameraAnimation));
							LordDelete stream;

							int camlength = (int)(pCameraAnimation->getTotalTime() * 1000);
							int escapelength = camlength + element.WaitingTime - animLength;
							if (escapelength > 0)
							{
								animLength += escapelength;
							}
							//animLength += (int)(pCameraAnimation->getPauseLength() * 1000);
						}
						else
						{
							LordLogError("Actor: cameraAnimtion file isn't exits: %s", filename.c_str());

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
							char buf[MAX_PATH];
							sprintf_s(buf, "Actor: cameraAnimtion file isn't exits: %s", filename.c_str());
							MessageBox(0, buf, "Error", 0);
#endif
							LordDelete pCameraAnimation;
						}
					}
				}
			}

			// 场景暗化
			element.type = eChangeDark;
			len = (ui32)it->second->ChangeDarks.size();
			for (ui32 i = 0; i < len; ++i)
			{
				element.WaitingTime = (ui32)(it->second->ChangeDarks[i]->beginTime * 1000 / mCurSkillState.mCurSkillTimeScale);
				// 击中的时间不能小于0.001秒
				if (element.WaitingTime < 0)
				{
					element.WaitingTime = 1;
				}
				element.pData = it->second->ChangeDarks[i];
				m_ElementsInfo.push_back(element);
			}


			// 其他新的种类++++
		}
		else
		{
			LordLogError("Actor: %s does not has skill: %s", m_pActorObject->GetActorFileName().c_str(), name.c_str());

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			char buf[MAX_PATH];
			sprintf_s(buf, "Actor: %s does not has skill: %s", m_pActorObject->GetActorFileName().c_str(), name.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return mLastAnimLength = -1;
		}

		// 直接处理等待时间为0的技能元素
		list<ElementInfo>::type::iterator itElement;
		for (itElement = m_ElementsInfo.begin(); itElement != m_ElementsInfo.end();)
		{
			if (itElement->WaitingTime < 1)
			{
				switch (itElement->type)
				{
				case eAnimation: _addAnimation(static_cast<ActorTemplateData::AnimationInfo*>(itElement->pData)); break;
				case eEffect: _addEffect(static_cast<ActorTemplateData::ActorEffect*>(itElement->pData)); break;
				case eCameraShake: _addCameraShake(static_cast<ActorTemplateData::CameraShake*>(itElement->pData)); break;
				case eSound: _addSound(static_cast<ActorTemplateData::Sound*>(itElement->pData)); break;
				case eAttackEvent: _addAttackEvent(static_cast<ActorTemplateData::AttackEvent*>(itElement->pData)); break;
				case eColorChange: _addColorChange(static_cast<ActorTemplateData::ColorChange*>(itElement->pData)); break;
				case eSlowDown: _addSlowDown(static_cast<ActorTemplateData::SlowDown*>(itElement->pData)); break;
				case eHitFly: _addHitFly(static_cast<ActorTemplateData::HitFly*>(itElement->pData)); break;
				case eActorScale: _addActorScale(static_cast<ActorTemplateData::ActorScale*>(itElement->pData)); break;
				case eCameraPush: _addCameraPush(static_cast<ActorTemplateData::CameraPush*>(itElement->pData)); break;
				case eCameraAnim: _addCameraAnim(static_cast<ActorTemplateData::CameraAnim*>(itElement->pData)); break;
				case eChangeDark: _addChangeDark(static_cast<ActorTemplateData::ChangeDark*>(itElement->pData)); break;
				case eSkillBreak:
				{
					ActorTemplateData::SkillBreak* pData = static_cast<ActorTemplateData::SkillBreak*>(itElement->pData);
					mCurSkillState.isPlaySkillBreakCallBlack = pData->isCallBlack;
					_addSkillBreak(pData);
					if (!mCurSkillState.isSkillBreakChangeSkill)
						mCurSkillState.isPlaySkillBreakCallBlack = false;
				}
				break;
				// 其他种类++++
				default: break;
				}

				if (mCurSkillState.isSkillBreakChangeSkill && mCurSkillState.isPlaySkillBreakCallBlack)
				{
					mCurSkillState.isSkillBreakChangeSkill = false;
					mCurSkillState.isPlaySkillBreakCallBlack = false;
					break;
				}

				m_ElementsInfo.erase(itElement++);
			}
			else
			{
				++itElement;
			}
		}

		return mLastAnimLength = animLength;
	}
	//-----------------------------------------------------------------------
	void SkillSystem::StopSkill()
	{
		// 停止特效等其他技能
		mCurSkillState.mCurSkillTimeScale = 1.0f;
		_stopSkillElements();

		// 相机抖动的关闭由客户端逻辑控制，编辑器关相机抖动也挪到CActor处理，这里不再关相机抖动
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_stopSkillElements(bool playallevent)
	{
		// 清空所有待播放和正在播放的，但是不影响正在播的动画
		mCurSkillState.isLoop = -1000;

		//处理完未完成的事件
		if (playallevent)
		{
			for (list<ElementInfo>::type::iterator itevt = m_ElementsInfo.begin(); itevt != m_ElementsInfo.end(); ++itevt)
			{
				switch (itevt->type)
				{
					//case eAnimation: _addAnimation(static_cast<ActorTemplateData::AnimationInfo*>(itevt->pData)); break;
					//case eEffect: _addEffect(static_cast<ActorTemplateData::ActorEffect*>(itevt->pData)); break;
					//case eCameraShake: _addCameraShake(static_cast<ActorTemplateData::CameraShake*>(itevt->pData)); break;
				case eSound: _addSound(static_cast<ActorTemplateData::Sound*>(itevt->pData)); break;
				case eAttackEvent: _addAttackEvent(static_cast<ActorTemplateData::AttackEvent*>(itevt->pData)); break;
				case eColorChange: _addColorChange(static_cast<ActorTemplateData::ColorChange*>(itevt->pData)); break;
				case eSlowDown: _addSlowDown(static_cast<ActorTemplateData::SlowDown*>(itevt->pData)); break;
				case eHitFly: _addHitFly(static_cast<ActorTemplateData::HitFly*>(itevt->pData)); break;
				case eActorScale: _addActorScale(static_cast<ActorTemplateData::ActorScale*>(itevt->pData)); break;
					//case eCameraPush: _addCameraPush(static_cast<ActorTemplateData::CameraPush*>(itevt->pData)); break;
					//case eCameraAnim: _addCameraAnim(static_cast<ActorTemplateData::CameraAnim*>(itevt->pData)); break;
					//case eChangeDark: _addChangeDark(static_cast<ActorTemplateData::ChangeDark*>(itevt->pData)); break;
					// 其他种类++++
				default: break;
				}
			}
		}
		m_ElementsInfo.clear();

		//5. 播放次数多于1次的飞行击中
		for (list<AttEvent>::type::iterator itAttEvent = m_Atts.begin(); itAttEvent != m_Atts.end();)
		{
			// 播一次			
			m_pActorObject->AddSkillAttackToTargetActors(itAttEvent->pData, mCurSkillName, mUserData, mUserData2, mCallBackNum, itAttEvent->pData->isCallback ? mCurCallIndex++ : mCurCallIndex);

			// 重置时间
			itAttEvent->NextTime = itAttEvent->pData->CycleTime;

			if (itAttEvent->timesLast > 0)
			{
				--itAttEvent->timesLast;
			}

			if (0 == itAttEvent->timesLast)
			{
				itAttEvent = m_Atts.erase(itAttEvent);
			}
		}

		// 音效
		for (list<SoundEvent>::iterator it = m_sounds.begin(); it != m_sounds.end(); ++it)
		{
			SoundSystem::Instance()->stopEffect(it->id);
		}
		m_sounds.clear();

		// 子模动画
		mChildSkillMap.clear();

		//清空摄像机动画
		for (map<String, CameraAnimation*>::type::iterator it = mCameraAnimations.begin(); it != mCameraAnimations.end(); ++it)
		{
			LordDelete it->second;
		}
		mCameraAnimations.clear();

		mCurrentAnimation = mNextAnimation = NULL;

		if (m_ChangeDark.bChangeDark)
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			if (pScene) pScene->ChangeDark(1.0f);
		}
		// 其他++++
	}
	//-----------------------------------------------------------------------
	void SkillSystem::Update(ui32 delta)
	{
		if (mCurrentAnimation)
		{
			mCurrentAnimation->update(delta);
			if (mCurrentAnimation->isStop() && mNextAnimation)
			{
				mCurrentAnimation = mNextAnimation; mNextAnimation = NULL;
				mCurrentAnimation->play();
			}
		}
		else
		{
			if (mNextAnimation)
			{
				mCurrentAnimation = mNextAnimation; mNextAnimation = NULL;
				mCurrentAnimation->play();
			}
		}

		/*list<SoundEvent>::type::iterator itSound = m_sounds.begin();
		for (; itSound != m_sounds.end();)
		{
		itSound->time += (float)delta * 0.001f;
		if (itSound->pData->length>0.0f && itSound->time >= itSound->pData->length)
		{
		SoundSystem::Instance()->stopEffect(itSound->id);
		m_sounds.erase(itSound++);
		}
		else
		{
		++itSound;
		}
		}*/

		if (!mCurSkillState.isOnlyCameraAnim)
		{
			// 更新已有的
			//1. 动画
			if (m_pActorObject->m_isFrozen)
			{
				StopSkill();
			}
			else if (mCurSkillState.isPauseActor)
			{
			}
			else
			{
				mAnimSystem.Update(delta);
			}
			//2. 特效
			float milTime = (float)delta * 0.001f;
			for (list<ActorEffect>::type::iterator it = m_effects.begin(); it != m_effects.end();)
			{
				// 更新时间
				float EffectTimeDelta = milTime * it->_TimeScale;
				if (!it->pData->isSlowdownValuable && m_pActorObject->mTimeScale != 1.0f)
				{
					EffectTimeDelta /= m_pActorObject->mTimeScale;
				}
				it->CurrentTime += EffectTimeDelta;

				bool bNeedReStart = false;
				if (it->pData->CycleTime > 0.0f && it->CurrentTime > it->pData->CycleTime)
				{
					bNeedReStart = true;
					it->CurrentTime -= it->pData->CycleTime;
					EffectTimeDelta = it->CurrentTime;
				}
				else if (it->pData->CycleTime <= 0.0f && !it->pEffect->isPlaying())
				{
					bNeedReStart = true;
					it->CurrentTime = EffectTimeDelta;
				}

				if (true == bNeedReStart)
				{
					if (it->times < 0)
					{
						// 无限次
						it->pEffect->stop();
						it->pEffect->start();
					}
					else
					{
						// 有限次
						--it->times;
						if (0 == it->times)
						{
							it->pEffect->unprepare_sys();
							EffectSystemManager::Instance()->destroyEffectSystem(it->pEffect);
							it->pEffect = NULL;
						}
						else
						{
							it->pEffect->stop();
							it->pEffect->start();
						}
					}
				}

				// 更新权重
				if (it->pEffect)
				{
					if (true == it->bFadeOut)
					{
						it->Weight -= EffectTimeDelta / EFFECT_FADE_TIME;
						if (it->Weight < 0.f)
						{
							it->pEffect->unprepare_sys();
							EffectSystemManager::Instance()->destroyEffectSystem(it->pEffect);
							it->pEffect = NULL;
						}
					}
				}

				// 更新状态
				if (it->pEffect)
				{
					float __tempDelta = (float)delta;
					if (!it->pData->isSlowdownValuable && m_pActorObject->mTimeScale != 1.0f)
					{
						__tempDelta /= m_pActorObject->mTimeScale;
					}
					it->pEffect->_update_sys(ui32(it->_TimeScale * it->pData->TimeScale * __tempDelta));
					++it;
				}
				else
				{
					it->pParentNode->destroyChild(it->pEffectNode);
					it->pEffectNode = NULL;
					it = m_effects.erase(it);
				}
			}

			//5. 播放次数多于1次的飞行击中
			for (list<AttEvent>::type::iterator itAttEvent = m_Atts.begin(); itAttEvent != m_Atts.end();)
			{
				// 更新时间
				itAttEvent->NextTime -= milTime;

				bool bDelete = false;
				if (itAttEvent->NextTime <= 0.0f)
				{
					// 播一次
					m_pActorObject->AddSkillAttackToTargetActors(itAttEvent->pData, mCurSkillName, mUserData, mUserData2, mCallBackNum, itAttEvent->pData->isCallback ? mCurCallIndex++ : mCurCallIndex);

					// 重置时间
					itAttEvent->NextTime = itAttEvent->pData->CycleTime;

					if (itAttEvent->timesLast > 0)
					{
						--itAttEvent->timesLast;
					}

					if (0 == itAttEvent->timesLast)
					{
						bDelete = true;
					}
				}

				// 是否继续播
				if (bDelete)
				{
					itAttEvent = m_Atts.erase(itAttEvent);
				}
				else
				{
					++itAttEvent;
				}
			}

			//6. 场景暗化
			if (m_ChangeDark.bChangeDark)
			{
				Scene* pScene = SceneManager::Instance()->getCurrentScene();
				if (pScene)
				{
					m_ChangeDark.time += milTime;
					const ActorTemplateData::ChangeDark* pChangeDark = reinterpret_cast<const ActorTemplateData::ChangeDark*>(m_ChangeDark.pData);
					if (m_ChangeDark.time <= pChangeDark->duration)
					{
						if (m_ChangeDark.time <= pChangeDark->darkTime)
						{
							//dark
							pScene->ChangeDark(1.f - pChangeDark->darkness*Math::Min(m_ChangeDark.time / (pChangeDark->darkTime + Math::MIN_FLOAT), 1.0f));
						}
						else if (m_ChangeDark.time >= pChangeDark->duration - pChangeDark->lightTime)
						{
							//light
							pScene->ChangeDark(1.f - pChangeDark->lightness*Math::Min((pChangeDark->duration - m_ChangeDark.time) / (pChangeDark->lightTime + Math::MIN_FLOAT), 1.0f));
						}
					}
					else
					{
						m_ChangeDark.bChangeDark = false;
					}
				}
				else
				{
					m_ChangeDark.bChangeDark = false;
				}
			}
			// 其他++++

			// 添加新的并设置初始状态
			for (list<ElementInfo>::type::iterator it = m_ElementsInfo.begin(); it != m_ElementsInfo.end();)
			{
				it->WaitingTime -= delta;
				if (it->WaitingTime <= 0.f)
				{
					switch (it->type)
					{
					case eAnimation: _addAnimation(static_cast<ActorTemplateData::AnimationInfo*>(it->pData)); break;
					case eEffect: _addEffect(static_cast<ActorTemplateData::ActorEffect*>(it->pData)); break;
					case eCameraShake: _addCameraShake(static_cast<ActorTemplateData::CameraShake*>(it->pData)); break;
					case eSound: _addSound(static_cast<ActorTemplateData::Sound*>(it->pData)); break;
					case eAttackEvent: _addAttackEvent(static_cast<ActorTemplateData::AttackEvent*>(it->pData)); break;
					case eColorChange: _addColorChange(static_cast<ActorTemplateData::ColorChange*>(it->pData)); break;
					case eSlowDown: _addSlowDown(static_cast<ActorTemplateData::SlowDown*>(it->pData)); break;
					case eHitFly: _addHitFly(static_cast<ActorTemplateData::HitFly*>(it->pData)); break;
					case eActorScale: _addActorScale(static_cast<ActorTemplateData::ActorScale*>(it->pData)); break;
					case eCameraPush: _addCameraPush(static_cast<ActorTemplateData::CameraPush*>(it->pData)); break;
					case eCameraAnim: _addCameraAnim(static_cast<ActorTemplateData::CameraAnim*>(it->pData)); break;
					case eChangeDark: _addChangeDark(static_cast<ActorTemplateData::ChangeDark*>(it->pData)); break;
					case eSkillBreak:
					{
						ActorTemplateData::SkillBreak* pData = static_cast<ActorTemplateData::SkillBreak*>(it->pData);
						mCurSkillState.isUpdateSkillBreakCallBlack = pData->isCallBlack;
						_addSkillBreak(pData);
						if (!mCurSkillState.isSkillBreakChangeSkill)
							mCurSkillState.isUpdateSkillBreakCallBlack = false;
						break;
					}
					// 其他种类++++
					default: break;
					}

					if (mCurSkillState.isSkillBreakChangeSkill && mCurSkillState.isUpdateSkillBreakCallBlack)
					{
						mCurSkillState.isSkillBreakChangeSkill = false;
						mCurSkillState.isUpdateSkillBreakCallBlack = false;
						break;
					}

					m_ElementsInfo.erase(it++);
				}
				else
				{
					++it;
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addAnimation(const ActorTemplateData::AnimationInfo* pData)
	{
		int nTime = pData->times;
		if (nTime <= 0)
			nTime = -1;		// 设置无限循环

		mAnimSystem.AddAnimation(pData->AnimName,
			nTime,
			pData->FadeTime,
			pData->TimeScale * mCurSkillState.mCurSkillTimeScale,
			pData->skillName,
			m_pActorObject,
			pData->pSetScale);
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addEffect(const ActorTemplateData::ActorEffect* pData)
	{
		ActorEffect effect;
		String strEffectName = m_pActorObject->GetNextEffectName();
		//static int skill_effectCount = 0;
		if (!pData->EffectFileName.empty())
		{
			effect.pEffect = EffectSystemManager::Instance()->createEffectSystem(strEffectName, pData->EffectFileName);
		}
		else
		{
			//LordException("SkillSystem::_addEffect: effect name error");
			LordLogError("SkillSystem::_addEffect: effect name error");
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			MessageBox(0, "SkillSystem::_addEffect: effect name error", 0, 0);
#endif
			return;
		}

		int nTime = pData->times;
		if (nTime <= 0)
			nTime = -1;		// 设置无限循环

		effect.bFadeOut = false;
		effect.Weight = 1.0f;
		effect.times = nTime;
		effect.pData = pData;
		effect.CurrentTime = 0.0f;
		effect._TimeScale = mCurSkillState.mCurSkillTimeScale; //客户端动态调整播放速度。

															   // 创建特效
		SceneNode* pParentSceneNode = m_pActorObject->getSocketSceneNode(pData->SocketName);
		if (!pParentSceneNode)
		{
			pParentSceneNode = m_pActorObject->GetSceneNodeMaster();
		}

		// 挂接到挂点上
		SceneNode* pEffectSceneNode = pParentSceneNode->createChild();
		effect.pEffect->setParentSceneNode(pEffectSceneNode);

		effect.pEffectNode = pEffectSceneNode;
		effect.pParentNode = pParentSceneNode;

		// 偏移 
		Quaternion Orientation = CaculateOritation(pData->yaw, pData->pitch, pData->roll);
		effect.pEffectNode->setLocalPosition(pData->position);
		effect.pEffectNode->setLocalOrientation(Orientation);
		effect.pEffectNode->setLocalScaling(Vector3(pData->scale, pData->scale, pData->scale));
		effect.pEffectNode->update();

		// 如果不跟随父节点，则挂到Actor的根节点下，但世界坐标下位置、姿态、缩放不变
		if (false == pData->FollowActor)
		{
			// 获得世界坐标下的位置姿态

			// 重设父节点
			SceneNode* pActorRoot = SceneManager::Instance()->getRootNode()->createChild();
			effect.pEffectNode->setParent(pActorRoot);
			if (m_pActorObject->IsMirror())
			{
				pActorRoot->setWorldPosition(effect.pParentNode->getWorldPosition());
				pActorRoot->setWorldOrientation(effect.pParentNode->getWorldOrientation());
				Vector3 pScale = effect.pParentNode->getWorldScaling();
				pScale.x = Math::Abs(pScale.x);
				pActorRoot->setLocalScaling(pScale);
			}
			else
			{
				Vector3 pos = effect.pEffectNode->getWorldPosition();
				Vector3 scale = effect.pEffectNode->getWorldScaling();
				Quaternion orient = effect.pEffectNode->getWorldOrientation();
				// 重设位置姿态
				effect.pEffectNode->setWorldPosition(pos);
				effect.pEffectNode->setWorldOrientation(orient);
				effect.pEffectNode->setLocalScaling(scale);
			}
			effect.pParentNode = pActorRoot;

		}

		// 初始化	
		bool ui = m_pActorObject->isUIActor();
		if (ui)
			effect.pEffect->setCurrentCamera(LORD::SceneManager::Instance()->getGUICamera());
		else
			effect.pEffect->setCurrentCamera(LORD::SceneManager::Instance()->getMainCamera());
		effect.pEffect->prepare_sys(ui);
		effect.pEffect->start();
		m_effects.push_back(effect);
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addCameraShake(const ActorTemplateData::CameraShake* pData)
	{
		if (false == pData->isShow)
			return;

		if (mCameraShakeEnable && pData->durationTime != 0)
		{
			// 相机振动
			//添加到摄像机抖动MAP中
			m_pActorObject->AddCameraShake(0, pData->shakescale, pData->durationTime, pData->shakeTimes);
		}
		else if (pData->isAddToAllActors)
		{
			ActorManager::Instance()->AddCrameraShakeToAllActors(pData->shakescale, pData->durationTime, pData->shakeTimes);
		}
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addSound(const ActorTemplateData::Sound* pData)
	{
		float rate = Math::IntervalRandom(0.0f, 1.0f);
		if (pData->showRate < rate)
			return;

		// 随机选个声音文件播
		int randInt = (int)Math::IntervalRandom(1.0f, (float)pData->soundFileNum);

		int soundNamePlay;
		switch (randInt)
		{
		case 1:
			soundNamePlay = pData->soundName; break;
		case 2:
			soundNamePlay = pData->soundName2; break;
		default:
			break;
		}

		if (mCameraShakeEnable)
		{
			SoundEvent sevnt;
			sevnt.id = SoundSystem::Instance()->playEffectByType((SoundType) soundNamePlay); 
			//SoundSystem::Instance()->setEffectsVolume(pData->volume);
			sevnt.pData = pData;
			sevnt.time = 0.0f;
			if (pData->bloop)
			{
				m_sounds.push_back(sevnt);
			}
		}
		else if (pData->isAddToAllActors)
		{
			SoundEvent sevnt;
			sevnt.id = SoundSystem::Instance()->playEffectByType((SoundType)soundNamePlay);
			//SoundSystem::Instance()->setEffectsVolume(pData->addToAllActorVolume);
			sevnt.pData = pData;
			sevnt.time = 0.0f;
			if (pData->bloop)
			{
				m_sounds.push_back(sevnt);
			}
		}

	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addAttackEvent(const ActorTemplateData::AttackEvent* pData)
	{
		if (false == pData->isShow)
			return;

		// 先播放1次
		m_pActorObject->AddSkillAttackToTargetActors(pData, mCurSkillName, mUserData, mUserData2, mCallBackNum, pData->isCallback ? mCurCallIndex++ : mCurCallIndex);

		// 播放次数不是1，记录下来以便循环播
		if (pData->times != 1)
		{
			AttEvent attInfo;
			attInfo.pData = pData;
			attInfo.NextTime = pData->CycleTime;
			if (pData->times > 1)
			{
				// 大于1，剩余次数为播放次数减1
				attInfo.timesLast = pData->times - 1;
			}
			else
			{
				// 0和负数，无限播放，直到播别的技能或Actor释放
				attInfo.timesLast = -1;
			}

			m_Atts.push_back(attInfo);
		}
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addColorChange(const ActorTemplateData::ColorChange* pData)
	{
		//m_pActorObject->SetCustomColor(pData->color, true, COLOR_CHANGE_TIME, pData->bCascade);
		m_pActorObject->SetNaturalColor(pData);
	}
	//-----------------------------------------------------------------------
	void SkillSystem::RenderSkillEffect()
	{
		for (list<ActorEffect>::type::iterator it = m_effects.begin(); it != m_effects.end(); ++it)
		{
			if (it->pEffect)
				it->pEffect->_updateRenderQueue();
		}
	}
	//-----------------------------------------------------------------------
	void SkillSystem::_addSlowDown(const ActorTemplateData::SlowDown* pData)
	{
		if (false == pData->isShow)
			return;

		float rate = Math::IntervalRandom(0.0f, 1.0f);
		if (pData->showRate < rate)
			return;



		if (pData->durationTime > 0.f && pData->equalizeTime > 0.f)
		{
			if (mCurSkillState.mCurSkillTimeScale != 1.0f)  //客户端调整播放速度
			{
				ActorTemplateData::SlowDown* pTempSlow = LordNewT(ActorTemplateData::SlowDown); LordAssert(pTempSlow);
				pTempSlow->durationTime = pData->durationTime / mCurSkillState.mCurSkillTimeScale;
				pTempSlow->equalizeTime = pData->equalizeTime / mCurSkillState.mCurSkillTimeScale;
				pTempSlow->timeScale = pData->timeScale / mCurSkillState.mCurSkillTimeScale;
				m_pActorObject->AddSlowDown(pData);
				m_pActorObject->AddSlowDownToTargetActors(pData);
				LordDeleteT(pTempSlow, SlowDown);
			}
			else
			{
				m_pActorObject->AddSlowDown(pData);
				m_pActorObject->AddSlowDownToTargetActors(pData);
			}
		}
	}

	const SkillAnimSystem::AnimStateList& SkillSystem::getAnimStateList() const
	{
		return mAnimSystem.getAnimStateList();
	}

	void SkillSystem::_addHitFly(const ActorTemplateData::HitFly* pData)
	{
		if (false == pData->isShow)
			return;

		if (mCameraShakeEnable)
			m_pActorObject->AddHitFly(pData);
	}

	void SkillSystem::_addCameraPush(const ActorTemplateData::CameraPush* pData)
	{
		if (false == pData->isShow)
			return;
		if (mCameraShakeEnable)
		{
			m_pActorObject->AddCameraPush(pData);
		}
		else if (pData->isAddToAllActors)
		{

		}
	}

	void SkillSystem::_addSkillBreak(const ActorTemplateData::SkillBreak* pData)
	{
		mCurSkillState.isPauseActor = pData->isFrozen;
		mCurSkillState.isHideActor = pData->isHide;

		/*if(mCurSkillState.isHideActor == true)
		{
		m_pActorObject->m_isSkillHideActor = true;
		}*/
		m_pActorObject->m_isSkillHideActor = mCurSkillState.isHideActor;

		if (pData->isCallBlack == true)
		{
			ActorManager::SkillBreak_CallBack skillBreakCallBackFunc = ActorManager::Instance()->getSkillBreakCallBack();
			if (skillBreakCallBackFunc)
			{
				(skillBreakCallBackFunc)(mCurSkillName, m_pActorObject);
			}
		}
	}

	void SkillSystem::_addActorScale(const ActorTemplateData::ActorScale* pData)
	{
		if (!mCurSkillState.isChangeActorScale)
		{
			mCurSkillState.lastActorScale = m_pActorObject->GetScale().x;
			mCurSkillState.isChangeActorScale = true;
		}
		m_pActorObject->AddSkillActorScale(pData);
	}

	void SkillSystem::_addCameraAnim(const ActorTemplateData::CameraAnim* pData)
	{
		String* pStr = (String*)pData;
		CameraAnimation* pCameraAnimation = mCameraAnimations[(*pStr)];
		if (pCameraAnimation)
		{
			//pCameraAnimation->play();
			mNextAnimation = pCameraAnimation;
		}
	}

	void SkillSystem::_addChangeDark(const ActorTemplateData::ChangeDark* pData)
	{
		if (!m_ChangeDark.bChangeDark)
		{
			m_ChangeDark.pData = pData;
			m_ChangeDark.bChangeDark = true;
			m_ChangeDark.time = 0;
		}
	}

	void SkillSystem::StopCameraAnim()
	{
		//清空摄像机动画
		for (map<String, CameraAnimation*>::type::iterator it = mCameraAnimations.begin(); it != mCameraAnimations.end(); ++it)
		{
			if (!it->second->isStop())
			{
				it->second->stop();
			}
		}
	}

}