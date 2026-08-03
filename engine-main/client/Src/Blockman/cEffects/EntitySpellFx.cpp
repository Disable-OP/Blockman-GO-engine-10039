#include "EntitySpellFx.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/TextureAtlas.h"
#include "EntityRenders/EntityRenderManager.h"

#include "World/World.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntitySpellFX::EntitySpellFX(World* pWorld, const Vector3& pos1, const Vector3& pos2)
	: EntityFx(pWorld, pos1, pos2)
{
	motion.y *= 0.2f;

	if (pos2.x == 0.f && pos2.z == 0.f)
	{
		motion.x *= 0.1f;
		motion.z *= 0.1f;
	}

	particleScale *= 0.75F;
	particleMaxAge = (int)(8.0f / (Math::UnitRandom() * 0.8f + 0.2f));
	noClip = false;
}

void EntitySpellFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) 
{
	EntityFx::renderParticle(vertices, brightness, rdt, rx, rxz, rz, ryz, rxy);
}

void EntitySpellFX::onUpdate()
{
	prevPos = position;

	if (particleAge++ >= particleMaxAge)
	{
		setDead();
	}

	setParticleTextureIndex(baseSpellTextureIndex + (7 - particleAge * 8 / particleMaxAge));
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
