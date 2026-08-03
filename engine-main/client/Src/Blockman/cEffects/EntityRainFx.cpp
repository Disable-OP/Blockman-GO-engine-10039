#include "EntityRainFx.h"

#include "World/World.h"
#include "Block/Blocks.h"
#include "Block/BM_Material.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntityRainFX::EntityRainFX(World* pWorld, const Vector3& pos)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	motion.x *= 0.3f;
	motion.y = Math::UnitRandom() * 0.2f + 0.1f;
	motion.z *= 0.3f;
	m_color = Color::WHITE;
	setParticleTextureIndex(19 + rand->nextInt(4));
	setSize(0.01F, 0.01F);
	particleGravity = 0.06F;
	particleMaxAge = (int)(8.0f / (Math::UnitRandom() * 0.8f + 0.2f));
}

void EntityRainFX::onUpdate()
{
	prevPos = position;
	motion.y -= particleGravity;
	moveEntity(motion);
	motion *= 0.98f;

	if (particleMaxAge-- <= 0)
	{
		setDead();
	}

	if (onGround)
	{
		if (Math::UnitRandom() < 0.5f)
		{
			setDead();
		}

		motion.x *= 0.7f;
		motion.z *= 0.7f;
	}

	BlockPos ipos = position.getFloor();
	const BM_Material& mat = world->getBlockMaterial(ipos);

	if (mat.isLiquid() || mat.isSolid())
	{
		float miny = (Math::Floor(position.y) + 1.f) - BlockFluid::getFluidHeightPercent(world->getBlockMeta(ipos));

		if (position.y < miny)
		{
			setDead();
		}
	}
}

}
