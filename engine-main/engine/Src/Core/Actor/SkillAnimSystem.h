/********************************************************************
filename: 	ActorAnimSystem.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _SKILLANIMSYSTEM_H_
#define _SKILLANIMSYSTEM_H_

#include "Actor/ActorTemplateData.h"

namespace LORD
{
	// 新的动画系统不负责维护时间，只负责根据外部要求设置动画的时间位置和影响的骨骼
	// 动画自己维护自己的局部时间，由于有若干不同的技能同时运行，所以没有统一的时间
	class AnimBlende;
	class AnimState;
	class SkillAnimSystem : public ObjectAlloc
	{
	private:
		struct ActorAnimation
		{
			bool bFadeOut;		// 是否处于淡出状态（用于更新权重和删除）
			bool bEnd;			// 是否播到最后一帧（用于更新时间判断）
			ui32 CurTime;		// 当前动画时间
			ui32 Length;		// 动画整体时长
			ui32 FadeTime;		// 动画过渡时间
			int TimeLast;		// 还要播多少次（负数为无限循环播放）
			float TimeScale;	// 播放速度
			float Weight;		// 权重 值域[0.0f, 1.0f]，淡入淡出时用
			float  scaleSpeed;

			ActorObject* m_Actor;
			AnimState* pAnimState;
			Vector2* pSetScale; //技能缩放
			String Name;		// 动画名称
			String SkillName;	//技能名

			void UpdateTime(ui32 delta);
		};


	public:
		SkillAnimSystem(void);
		~SkillAnimSystem(void);

		typedef vector<AnimState*>::type	AnimStateList;
	public:
		void Init(Skeleton* pSkeleton, AnimBlender* pblender);

		void Free();

		// 参数bUpdateSkeleton为true才计算动画数据更新骨骼位置，
		// 否则只更新动画的权重、时间和状态
		void Update(ui32 delta);

		// 添加动画
		void AddAnimation(const String &name,
			int nTime,
			ui32 FadeTime,
			float TimeScale,
			const String& skillName = "",
			ActorObject* pActor = NULL,
			Vector2* pScale = NULL);

		void AddUpperAnimation(const String& name, bool bLoop, float timeScale);

		// 清空所有动画：smooth为true，当前所有动画渐出；smooth为false，所有动画都置为无效
		void ClearAllAnimations(bool smooth = false);

		// 获得指定动画的时间长度
		ui32 GetAnimationLength(const String& name);

		AnimState* getAnimState(const String& name);

		AnimState* createAnimState(Animation* pAnim);

		const AnimStateList& getAnimStateList() const { return m_animStates; }

	private:
		unsigned short m_NumBones;
		Skeleton* m_pSkeleton;
		AnimBlender* m_pBlender;
		ActorAnimation m_ActorAnimation;
		AnimStateList	m_animStates;
	};

}

#endif