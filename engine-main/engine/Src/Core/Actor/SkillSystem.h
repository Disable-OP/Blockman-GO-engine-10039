/********************************************************************
filename: 	SkillSystem.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _SKILLSYSTEM_H_
#define _SKILLSYSTEM_H_

#include "Core.h"
#include "ActorTemplateData.h"
#include "SkillAnimSystem.h"

namespace LORD
{

	// 特效被强制停止后的渐出时间
	const float EFFECT_FADE_TIME = 0.25f;

	class ActorObject;
	class EffectSystem;
	class CameraAnimation;
	class LORD_CORE_API SkillSystem : public ObjectAlloc
	{
	private:
		enum eElementType
		{
			eAnimation,
			eEffect,
			eSound,
			eCameraShake,
			eAttackEvent,
			eColorChange,
			eSlowDown,
			eHitFly,
			eCameraPush,
			eSkillBreak,
			eActorScale,
			eCameraAnim,
			eChangeDark
		};

		struct ElementInfo
		{
			eElementType type;
			int WaitingTime;
			void* pData;
		};

		struct CurentSkillState
		{
			//是否回调
			bool isUpdateSkillBreakCallBlack;
			bool isPlaySkillBreakCallBlack;
			//是否切技能
			bool isSkillBreakChangeSkill;
			//是否暂停
			bool isPauseActor;
			bool isOnlyCameraAnim;
			//是否隐藏
			bool isHideActor;
			//是否调整了角色缩放
			bool isChangeActorScale;
			//是否是无限循环；
			int isLoop;
			//当前技能的缩放比例
			float mCurSkillTimeScale;
			float lastActorScale;

			CurentSkillState()
			{
				mCurSkillTimeScale = 1.0f;
				isUpdateSkillBreakCallBlack = false;
				isPlaySkillBreakCallBlack = false;
				isSkillBreakChangeSkill = false;
				isPauseActor = false;
				isOnlyCameraAnim = false;
				isHideActor = false;
				isLoop = -1000;
				isChangeActorScale = false;
				lastActorScale = 1.0f;
			}
		};

		// 特效基本
		struct EffectBase
		{
			// 渐出状态
			bool bFadeOut;
			// 播放次数(小于等于0为无限次)
			int times;
			// 权重
			float Weight;
			// 每次播放开始后经过的时间
			float CurrentTime;
			//播放速度（客户端用）
			float _TimeScale;
			// 特效指针
			EffectSystem *pEffect;
			// 特效场景节点
			SceneNode *pEffectNode;
			// 挂接的父节点
			SceneNode *pParentNode;
		};

		// 一般技能特效
		struct ActorEffect : public EffectBase
		{
			// 模板数据
			const ActorTemplateData::ActorEffect* pData;
		};

		// 攻击事件
		struct AttEvent
		{
			// 剩余播放次数(小于等于0为无限次)
			int timesLast;
			// 距下次播放的时间
			float NextTime;
			// 模板数据
			const ActorTemplateData::AttackEvent* pData;
		};

		struct ChangeDark
		{
			bool bChangeDark;
			float time;
			const ActorTemplateData::ChangeDark* pData;

			ChangeDark()
				: bChangeDark()
				, pData(NULL)
			{
			}
		};

		struct SoundEvent
		{
			ui32 id;
			float time;
			const ActorTemplateData::Sound* pData;
		};

	public:
		SkillSystem(void);
		~SkillSystem(void);

		static String GetNextSkillName();
	public:
		void Init(const ActorTemplateData* pTemplateData, ActorObject* pActorObject);
		void Free();

		// 播放一个技能，bInsert用于控制原来除动画外的效果是否停止
		// 返回值：0.0 未找到该技能或技能中没有动画；负数 动画为无限循环动作；正数 动画的播放时长
		int PlaySkill(const String& name, bool bInsert, float TimeScale = 1.0f, int UserData = 0, int UserData2 = -1, const String& actorNameArray = StringUtil::BLANK, bool playsound = true);

		void PlayUpperAnim(const String& name, bool bLoop = false, float TimeScla = 1.f);

		// 停止一个技能，动画不再更新，其他效果一律停止
		void StopSkill();

		// 播放一个基本动作
		void PlayAnimation(const String &name, int nTime);

		// 判断当前是否可以受击
		bool IsWoundActionEnable(void);

		// 获得当前头顶字移动模式
		unsigned char GetTextMoveMode(void);

		// 更新技能
		void Update(ui32 delta);

		// 设置相机抖动启用
		void SetCameraShakeEnable(bool enable) { mCameraShakeEnable = enable; }
		bool GetCameraShakeEnable() { return mCameraShakeEnable; }

		const SkillAnimSystem::AnimStateList& getAnimStateList() const;

		void RenderSkillEffect();

		int GetCurCallBackNum() { return mCallBackNum; }

		void SetOnlyCameraAnim(bool onlyanim) { mCurSkillState.isOnlyCameraAnim = onlyanim; }

		void StopCameraAnim();
	private:
		void _stopSkillElements(bool playallevent = false);
		void _addAnimation(const ActorTemplateData::AnimationInfo* pData);
		void _addEffect(const ActorTemplateData::ActorEffect* pData);
		void _addCameraShake(const ActorTemplateData::CameraShake* pData);
		void _addSound(const ActorTemplateData::Sound* pData);
		void _addAttackEvent(const ActorTemplateData::AttackEvent* pData);
		void _addColorChange(const ActorTemplateData::ColorChange* pData);
		void _addSlowDown(const ActorTemplateData::SlowDown* pData);
		void _addHitFly(const ActorTemplateData::HitFly* pData);
		void _addCameraPush(const ActorTemplateData::CameraPush* pData);
		void _addSkillBreak(const ActorTemplateData::SkillBreak* pData);
		void _addActorScale(const ActorTemplateData::ActorScale* pData);
		void _addCameraAnim(const ActorTemplateData::CameraAnim* pData);
		void _addChangeDark(const ActorTemplateData::ChangeDark* pData);
	private:
		// 当前头顶字移动状态
		unsigned char mTextMoveMode;

		bool mTextMoveModeDirty;
		bool mCameraShakeEnable;// 相机抖动是否启用

								// 还有多长时间恢复上个技能名称（小于0永不恢复）
		int m_TimeLastSkillName;
		int	mCallBackNum;
		int mCurCallIndex;
		int mUserData;
		int mUserData2;
		int mLastAnimLength;

		const ActorTemplateData* m_pTemplateData;
		ActorObject* m_pActorObject;

		CameraAnimation* mCurrentAnimation;
		CameraAnimation* mNextAnimation;

		ChangeDark m_ChangeDark; //同一时刻只存在唯一一个场景暗化事件
								 //当前技能状态
		CurentSkillState mCurSkillState;
		// 动画系统
		SkillAnimSystem mAnimSystem;

		// 待处理的列表
		list<ElementInfo>::type m_ElementsInfo;
		list<ElementInfo>::type m_TerrainElementsInfo;
		// 处理中的列表
		// 特效、音效、颜色改变、飞行击中、顿帧
		list<ActorEffect>::type m_effects;
		list<SoundEvent>::type m_sounds;
		list<AttEvent>::type m_Atts;
		// 当前播放的技能名称
		String mCurSkillName;
		// 当前技能播完后恢复的技能名称（上半身播完后恢复回上个技能的名字）
		String mLastSkillName;
		// 当前各个挂点要持续播的子模动画
		map<String, String>::type mChildSkillMap;
		map<String, CameraAnimation*>::type mCameraAnimations;
	};
}

#endif