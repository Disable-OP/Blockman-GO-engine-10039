#include "EntityBubbleFx.h"

#include "Block/BM_Material.h"
#include "World/World.h"

namespace BLOCKMAN
{

EntityBubbleFX::EntityBubbleFX(World* pWorld, const Vector3& pos, const Vector3& pos2)
	: EntityFx(pWorld, pos, pos2)
{
	m_color = Color::WHITE;
	setParticleTextureIndex(32);
	setSize(0.02F, 0.02F);
	particleScale *= rand->nextFloat() * 0.6F + 0.2F;
	Vector3 randvec;
	randvec.x = Math::UnitRandom();
	randvec.y = Math::UnitRandom();
	randvec.z = Math::UnitRandom();
	motion = pos2 * 0.2f + (randvec * 2.f - 1.f) * 0.02f;
	particleMaxAge = (int)(8.0f / (Math::UnitRandom() * 0.8f + 0.2f));
}

void EntityBubbleFX::onUpdate()
{
	prevPos = position;
	motion.y += 0.002f;
	moveEntity(motion);
	motion *= 0.85f;

	if (world->getBlockMaterial(position.getFloor()) != BM_Material::BM_MAT_water)
	{
		setDead();
	}

	if (particleMaxAge-- <= 0)
	{
		setDead();
	}
}

}
