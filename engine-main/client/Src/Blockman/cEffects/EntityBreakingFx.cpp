#include "EntityBreakingFx.h"

#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"

#include "World/World.h"
#include "Block/Block.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"


namespace BLOCKMAN
{

EntityBreakingFX::EntityBreakingFX(World* pWorld, const Vector3& pos, cItem* pItem, int subtype)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	setSprite(pItem->getIconFromDamage(subtype));
	m_color = Color::WHITE;
	particleGravity = BlockManager::blockSnow->getParticleGravity();
	particleScale /= 2.f;
}

EntityBreakingFX::EntityBreakingFX(World* pWorld, const Vector3& pos, const Vector3& pos2, cItem* pItem, int subtype)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	setSprite(pItem->getIconFromDamage(subtype));
	m_color = Color::WHITE;
	particleGravity = BlockManager::blockSnow->getParticleGravity();
	particleScale /= 2.f;
	motion *= 0.1f;
	motion += pos2;
}

void EntityBreakingFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	float u1 = ((float)particleTextureIndexX + particleTextureJitterX / 4.0F) / 16.0F;
	float u2 = u1 + 0.015609375F;
	float v1 = ((float)particleTextureIndexY + particleTextureJitterY / 4.0F) / 16.0F;
	float v2 = v1 + 0.015609375F;
	float s = 0.1F * particleScale;

	if (particleIcon != NULL)
	{
		u1 = particleIcon->getInterpolatedU(particleTextureJitterX / 4.0F);
		u2 = particleIcon->getInterpolatedU((particleTextureJitterX + 1.0F) / 4.0F);
		v1 = particleIcon->getInterpolatedV(particleTextureJitterY / 4.0F);
		v2 = particleIcon->getInterpolatedV((particleTextureJitterY + 1.0F) / 4.0F);
	}

	Vector3 v = prevPos + (position - prevPos) * rdt - interpPos;
	ui32 color = m_color.getARGB();
	vertices[0].set(v.x - (rx + ryz) * s, v.y - rxz * s, v.z - (rz + rxy) * s, color, brightness, u1, v2);
	vertices[1].set(v.x - (rx - ryz) * s, v.y + rxz * s, v.z - (rz - rxy) * s, color, brightness, u1, v1);
	vertices[2].set(v.x + (rx + ryz) * s, v.y + rxz * s, v.z + (rz + rxy) * s, color, brightness, u2, v1);
	vertices[3].set(v.x + (rx - ryz) * s, v.y - rxz * s, v.z + (rz - rxy) * s, color, brightness, u2, v2);
}

}
