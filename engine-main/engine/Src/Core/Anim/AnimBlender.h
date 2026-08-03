/********************************************************************
filename: 	AnimBlender.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIMBLENDER_H__
#define __LORD_ANIMBLENDER_H__

#include "Core.h"
#include "Resource/LordThread.h"

#define LERPCALCANIM

namespace LORD
{
	class Skeleton;
	class AnimState;
	class BlenderEvent;

	class LORD_CORE_API AnimBlender : public ObjectAlloc
	{
		friend class BlenderEvent;
	public:
		AnimBlender(Skeleton* pSkel);
		virtual ~AnimBlender();

	public:

		enum BlendMode
		{
			BM_REPLACE,		// stop src anim and start dest anim
			BM_CROSSFADE,	// blend src animation out while blending dest animation in
		};

	public:
		virtual bool					blend(AnimState* pAnimState, ui32 blendTime);
		virtual bool					blend_upper(AnimState* pAnimState, bool bLoop, float timeScale);

		virtual void					setBlendMode(BlendMode mode);

		virtual Skeleton*				getSkeleton() const;
		virtual bool					isBlending() const;
		virtual AnimState*				getAnimState() const;
		virtual const Matrix4&			getAnimMatrix(uint idx) const;
		const Matrix4*					getAnimMatrixPtr() const;
		virtual const Quaternion&		getAnimOrientation(uint idx) const;
		const Quaternion*				getAnimOrientationPtr() const;
		virtual const Vector3&			getAnimPosition(uint idx) const;
		const Vector3*					getAnimPositionPtr() const;
		void							setExtRotation(int idx, const Quaternion& q);

		virtual void					frameMove(ui32 deltaTime);
		virtual void					frameMoveForCalcThread(ui32 deltaTime);
		virtual void					excludeCalcThread();
		virtual void					setIsLoopSkill(bool isLoopSkill);

	protected:
		Skeleton*		m_pSkel;
		BlendMode		m_mode;
		AnimState*		m_pAnimState;
		AnimState*		m_pTargetAnimState;
		AnimState*		m_pUpperState;
		ui32			m_blendTime;
		int				m_leftBlendTime;
		Matrix4*		m_AnimMatrix;
		Quaternion*		m_KeyOrientation;
		Vector3*		m_KeyPosition;
		BlenderEvent*	m_pBlenderEvent;
		bool			mbNeedMerge;
		bool            mbIsLoopSkill;
		bool			m_hasExtRotation;    // just only for b_Head bone's yaw,pitch.
		int				m_extIdx;			 // just only for b_Head bone's yaw,pitch.
		Quaternion		m_extRotation;		 // just only for b_Head bone's yaw,pitch.
	};

	class BlenderEvent : public ThreadEvent
	{
	public:
		BlenderEvent(AnimBlender* blender);
		~BlenderEvent();
		virtual bool IsAutoDestroy() { return true; }
		virtual bool GetRepeatRespondIfFail() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent() { return true; }

		virtual void SetInCalcThread(bool inThread);

		virtual bool GetInCalcThread();

		virtual void RestoreEvent();

	protected:
		AnimBlender* mBlender;
		Quaternion* mBlenderOrientation;
		Vector3*	mBlenderPos;
		bool		mInCalcThread;
	};
}

#endif