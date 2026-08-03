/********************************************************************
filename: 	EffectParticle.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __EFFECT_PARTICLE_H__
#define __EFFECT_PARTICLE_H__

#include "Core.h"

namespace LORD
{
	class EffectParticle
	{
	public:
		EffectParticle(void);
		virtual ~EffectParticle(void);

		virtual void copyAttributesTo(EffectParticle* particle);

		void reParticleState();
		void forwardParticleState();
		void forwardParticleState2();

		EffectParticle* parentParticle;

		//// copy attribute begin
		Vector3		position;
		Quaternion		oritation;
		float		selfRotation;
		Color		color;
		float		scale;
		Vector3		direction;

		Vector3		originalPosition;
		Quaternion		originalOritation;
		float		originalSelfRotation;
		Color		originalColor;
		float		originalScale;
		float       originalSpeed;
		Vector3		originalDirection;

		Vector3		movementDirection;
		i32			mTime;
		i32         mLeaveTime;
		//// copy attribute end

		Vector3		scenePosition;
		Quaternion		sceneOritation;

		Vector3		mlastPosition;
		int			mReDirection;
		bool		mInController;

		static const size_t sCopyAttributeSize;
		static const size_t sReStateSize;
	};
}

#endif
