#include "EntityLavaFx.h"

#include "World/World.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntityLavaFX::EntityLavaFX(World* pWorld, const Vector3& pos)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	motion *= 0.8f;
	motion.y = rand->nextFloat() * 0.4F + 0.05F;
	m_color = Color::WHITE;
	particleScale *= rand->nextFloat() * 2.0F + 0.2F;
	lavaParticleScale = particleScale;
	particleMaxAge = (int)(16.f / (Math::UnitRandom() * 0.8f + 0.2f));
	noClip = false;
	setParticleTextureIndex(49);
}

int EntityLavaFX::getBrightnessForRender(float rdt)
{
	float age = ((float)particleAge + rdt) / (float)particleMaxAge;

	age = Math::Clamp(age, 0.f, 1.f);

	int var3 = EntityFx::getBrightnessForRender(rdt);
	short var4 = 240;
	int var5 = var3 >> 16 & 255;
	return var4 | var5 << 16;
}

void EntityLavaFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	float age = ((float)particleAge + rdt) / (float)particleMaxAge;
	particleScale = lavaParticleScale * (1.0F - age * age);
	EntityFx::renderParticle(vertices, brightness, rdt, rx, rxz, rz, ryz, rxy);
}

void EntityLavaFX::onUpdate()
{
	prevPos = position;

	if (particleAge++ >= particleMaxAge)
	{
		setDead();
	}

	float age = (float)particleAge / (float)particleMaxAge;

	if (rand->nextFloat() > age)
	{
		world->spawnParticle("smoke", position, motion);
	}

	motion.y -= 0.03f;
	moveEntity(motion);
	motion *= 0.999f;

	if (onGround)
	{
		motion.x *= 0.7f;
		motion.z *= 0.7f;
	}
}

}
