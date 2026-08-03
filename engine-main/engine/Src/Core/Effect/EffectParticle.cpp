#include "EffectParticle.h"

namespace LORD
{
	const size_t EffectParticle::sCopyAttributeSize = (sizeof(Vector3)+sizeof(Quaternion)+sizeof(Vector3)+sizeof(float)+sizeof(Color)+sizeof(float))*2+sizeof(Vector3)+sizeof(float);
	const size_t EffectParticle::sReStateSize = sizeof(Vector3)+sizeof(Quaternion)+sizeof(float)+sizeof(Color)+sizeof(float);

	void EffectParticle::copyAttributesTo(EffectParticle* particle)
	{
		memcpy(&(particle->position), &(position), sCopyAttributeSize);
	}

	EffectParticle::EffectParticle()
		: parentParticle(NULL)
		, position(Vector3::ZERO)
		, oritation(Quaternion::IDENTITY)
		, selfRotation(0.f)
		, color(Color::WHITE)
		, scale(1.f)
		, direction(Vector3::UNIT_Z)
		, originalPosition(Vector3::ZERO)
		, originalOritation(Quaternion::IDENTITY)
		, originalSelfRotation(0.f)
		, originalColor(Color::WHITE)
		, originalScale(1.f)
		, originalSpeed(0.0f)
		, originalDirection(Vector3::UNIT_Z)
		, movementDirection(Vector3::ZERO)
		, mReDirection(2)
		, mInController(false)
	{
	}

	EffectParticle::~EffectParticle()
	{
	}

	void EffectParticle::reParticleState()
	{
		memcpy(&position, &originalPosition, sReStateSize);
	}

	void EffectParticle::forwardParticleState()
	{	
		memcpy(&originalPosition, &position, sReStateSize);
	}

	void EffectParticle::forwardParticleState2()
	{
		originalScale = scale;
		originalColor = color;
	}
}