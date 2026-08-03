#include "EntitySmokeFx.h"

#include "World/World.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntitySmokeFX::EntitySmokeFX(World* pWorld, const Vector3& pos, const Vector3& pos2, float scale)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	motion *= 0.1f;
	motion += pos2;
	m_color.r = m_color.g = m_color.b = Math::UnitRandom() * 0.3f;
	particleScale *= 0.75F;
	particleScale *= scale;
	smokeParticleScale = particleScale;
	particleMaxAge = int(8.0f / (Math::UnitRandom() * 0.8f + 0.2f));
	particleMaxAge = (int)(particleMaxAge * scale);
	noClip = false;
}

void EntitySmokeFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	float age = ((float)particleAge + rdt) / (float)particleMaxAge * 32.0F;

	age = Math::Clamp(age, 0.f, 1.f);

	particleScale = smokeParticleScale * age;
	EntityFx::renderParticle(vertices, brightness, rdt, rx, rxz, rz, ryz, rxy);
}

void EntitySmokeFX::onUpdate()
{
	prevPos = position;

	if (particleAge++ >= particleMaxAge)
	{
		setDead();
	}

	setParticleTextureIndex(7 - particleAge * 8 / particleMaxAge);
	motion.y += 0.004f;
	moveEntity(motion);

	if (position.y == prevPos.y)
	{
		motion.x *= 1.1f;
		motion.z *= 1.1f;
	}

	motion *= 0.96f;

	if (onGround)
	{
		motion.x *= 0.7f;
		motion.z *= 0.7f;
	}
}

}
