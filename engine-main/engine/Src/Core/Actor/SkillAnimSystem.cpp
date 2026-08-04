#include "Core.h"
#include "Anim/AnimManager.h"
#include "Anim/Animation.h"
#include "Anim/Skeleton.h"
#include "Anim/AnimBlender.h"
#include "Anim/AnimState.h"
#include "SkillAnimSystem.h"
#include "Actor/ActorManager.h"

namespace LORD
{
	SkillAnimSystem::SkillAnimSystem(void)
		: m_pSkeleton(NULL)
		, m_pBlender(NULL)
		, m_NumBones(0)
		, m_ActorAnimation()
	{
	}
	//-----------------------------------------------------------------------
	SkillAnimSystem::~SkillAnimSystem(void)
	{
		Free();
	}
	//-----------------------------------------------------------------------
	void SkillAnimSystem::Init(Skeleton* pSkeleton, AnimBlender* pblender)
	{
		m_pSkeleton = pSkeleton;
		m_pBlender = pblender;

		// 初始化骨骼分组信息
		if (!pSkeleton)
			return;

		// 设置骨骼总数
		m_NumBones = pSkeleton->getBoneCount();
	}
	//-----------------------------------------------------------------------
	void SkillAnimSystem::Free()
	{
		ClearAllAnimations();

		m_pBlender = NULL;

		for (size_t i = 0; i < m_animStates.size(); ++i)
		{
			LordSafeDelete(m_animStates[i]);
		}
		m_animStates.clear();

		m_pSkeleton = NULL;
		m_NumBones = 0;
	}
	//-----------------------------------------------------------------------
	void SkillAnimSystem::ClearAllAnimations(bool smooth)
	{
		m_ActorAnimation.bEnd = true;
		m_ActorAnimation.bFadeOut = true;
		m_ActorAnimation.CurTime = 0;
		m_ActorAnimation.Length = 0;
		m_ActorAnimation.pAnimState = NULL;
		m_ActorAnimation.TimeLast = 0;
		m_ActorAnimation.Name.clear();
		m_ActorAnimation.FadeTime = 0;
		m_ActorAnimation.TimeScale = 0;
		m_ActorAnimation.scaleSpeed = 0;
		m_ActorAnimation.m_Actor = NULL;
		m_ActorAnimation.pSetScale = NULL;
	}
	//-----------------------------------------------------------------------
	ui32 SkillAnimSystem::GetAnimationLength(const String& name)
	{
		Animation* pAnim = static_cast<Animation*>(AnimManager::Instance()->getByName(name));
		LordAssert(pAnim);
		return pAnim->getLength();
	}
	//-----------------------------------------------------------------------
	void SkillAnimSystem::AddUpperAnimation(const String& name, bool bLoop, float timeScale)
	{
		if (!m_pSkeleton)
			return;

		Animation* pAnim = static_cast<Animation*>(AnimManager::Instance()->getByName(name));
		if (!pAnim)
		{
			LordLogError("unloaded Animation file %s!", name.c_str());
			return;
		}

		AnimState* pAnimState = getAnimState(name);
		if (!pAnimState)
			pAnimState = createAnimState(pAnim);

		m_pBlender->blend_upper(pAnimState, bLoop, timeScale);
	}
	//-----------------------------------------------------------------------
	void SkillAnimSystem::AddAnimation(const String &name,
		int nTime,
		ui32 FadeTime,
		float TimeScale,
		const String& skillName,
		ActorObject* pActor,
		Vector2* pScale)
	{
		// 添加动画，这里没有做重复添加相同动画的判断
		if (!m_pSkeleton)
			return;

		Animation* pAnim = static_cast<Animation*>(AnimManager::Instance()->getByName(name));
		if (!pAnim)
		{
			//LordException("未载入动画文件 %s !", name.c_str());
			//LordLogError("未载入动画文件 %s !", name.c_str());

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			char buf[MAX_PATH];
			sprintf_s(buf, "未载入动画文件 %s !", name.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
		}

		AnimState* pAnimState = getAnimState(name);
		if (!pAnimState)
		{
			pAnimState = createAnimState(pAnim);
		}
		m_ActorAnimation.bEnd = false;
		m_ActorAnimation.bFadeOut = false;
		m_ActorAnimation.CurTime = 0;
		m_ActorAnimation.Length = pAnimState->getLength();
		m_ActorAnimation.pAnimState = pAnimState;
		m_ActorAnimation.TimeLast = nTime;
		m_ActorAnimation.Name = name;
		m_ActorAnimation.FadeTime = FadeTime;
		m_ActorAnimation.TimeScale = TimeScale;
		m_ActorAnimation.SkillName = skillName;
		m_ActorAnimation.m_Actor = pActor;
		m_ActorAnimation.pSetScale = pScale;
		if (pScale && pActor)
		{
			m_ActorAnimation.scaleSpeed = (pScale->y - pScale->x) / m_ActorAnimation.Length;
			pActor->SetScale(Vector3(pScale->x, pScale->x, pScale->x));
		}

		if (nTime <= 0)
			m_ActorAnimation.TimeLast = -1;	// 设置无限循环

		m_pBlender->blend(pAnimState, m_ActorAnimation.FadeTime);

		if (nTime <= 0)
		{
			m_pBlender->setIsLoopSkill(true);
		}
		else
		{
			m_pBlender->setIsLoopSkill(false);
		}


		// 初始时间设置
		pAnimState->setTimePosition(m_ActorAnimation.CurTime);
	}
	//-----------------------------------------------------------------------	
	void SkillAnimSystem::ActorAnimation::UpdateTime(ui32 delta)
	{
		if (bEnd) // 已播完，自动停在最后一帧
			return;

		delta = ui32(delta * TimeScale);

		CurTime += delta;
		if (CurTime > Length)
		{
			if (TimeLast < 0)
			{
				// 无限循环
				//CurTime -= Length
				float rate = (float)CurTime / (float)Length;
				rate -= (float)(int)rate;
				CurTime = ui32(rate * Length);
			}
			else
			{
				// 有限次
				--TimeLast;
				if (0 == TimeLast)
				{
					// 已播完最后一次，设置停在最后一帧
					bEnd = true;
					CurTime = Length - ANIM_FLOAT_DISTIME;

					if (pSetScale && m_Actor)
					{
						m_Actor->SetScale(Vector3(pSetScale->y, pSetScale->y, pSetScale->y));
					}
				}
				else
				{
					float rate = (float)CurTime / (float)Length;
					rate -= (float)(int)rate;
					CurTime = ui32(rate * Length);
				}
				ActorManager::SkillName_CallBack skillNameCBFuncp = ActorManager::Instance()->getSkillNameCallBack();
				if (skillNameCBFuncp)
				{
					(skillNameCBFuncp)(SkillName, m_Actor);
				}
			}
		}

		if (pSetScale && m_Actor)
		{
			float cs = pSetScale->x + scaleSpeed * CurTime;
			m_Actor->SetScale(Vector3(cs, cs, cs));
		}

		pAnimState->setTimePosition(CurTime);
	}
	//-----------------------------------------------------------------------	
	void SkillAnimSystem::Update(ui32 delta)
	{
		//1. 更新时间
		m_ActorAnimation.UpdateTime(delta);
	}

	AnimState* SkillAnimSystem::getAnimState(const String& name)
	{
		AnimStateList::const_iterator it = m_animStates.begin();
		for (; it != m_animStates.end(); ++it)
		{
			if ((*it)->getAnim()->getName() == name)
			{
				return *it;
			}
		}
		return NULL;
	}

	AnimState* SkillAnimSystem::createAnimState(Animation* pAnim)
	{
		AnimState* pAnimState = LordNew AnimState(pAnim);
		m_animStates.push_back(pAnimState);
		return pAnimState;
	}

}