#include "EntityDiggingFx.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/TextureAtlas.h"
#include "EntityRenders/EntityRenderManager.h"

#include "Block/Block.h"
#include "World/World.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntityDiggingFX::EntityDiggingFX(World* pWorld, const Vector3& pos1, const Vector3& pos2, Block* pBlock, int side)
	: EntityFx(pWorld, pos1, pos2)
{
	blockInstance = cBlockManager::scBlocks[pBlock->getBlockID()];
	setSprite(blockInstance->getIcon(0, side));
	particleGravity = pBlock->getParticleGravity();
	m_color.r = m_color.g = m_color.b = 0.8f;  // mc is 0.6f.
	particleScale /= 2.0F;
}

void EntityDiggingFX::applyColourMultiplier(const BlockPos& pos)
{
	if (blockInstance == cBlockManager::scBlocks[BLOCK_ID_GRASS])
		return;

	ui32 c = blockInstance->colorMultiplier(world, pos);
	m_color.r *= (float)(c >> 16 & 255) / 255.0F;
	m_color.g *= (float)(c >> 8 & 255) / 255.0F;
	m_color.b *= (float)(c & 255) / 255.0F;
}

void EntityDiggingFX::applyRenderColor(int meta)
{
	if (blockInstance == cBlockManager::scBlocks[BLOCK_ID_GRASS])
		return;
	
	int c = blockInstance->getRenderColor(meta);
	m_color.r *= (float)(c >> 16 & 255) / 255.0F;
	m_color.g *= (float)(c >> 8 & 255) / 255.0F;
	m_color.b *= (float)(c & 255) / 255.0F;
}

void EntityDiggingFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	float u1 = ((float)particleTextureIndexX + particleTextureJitterX / 4.0F) / 16.0F;
	float u2 = u1 + 0.015609375F;
	float v1 = ((float)particleTextureIndexY + particleTextureJitterY / 4.0F) / 16.0F;
	float v2 = v1 + 0.015609375F;
	float s = 0.1F * particleScale;

	if (particleIcon != nullptr)
	{
		u1 = particleIcon->getInterpolatedU(particleTextureJitterX / 4.0F);
		u2 = particleIcon->getInterpolatedU((particleTextureJitterX + 1.0F) / 4.0F);
		v1 = particleIcon->getInterpolatedV(particleTextureJitterY / 4.0F);
		v2 = particleIcon->getInterpolatedV((particleTextureJitterY + 1.0F) / 4.0F);
	}

	Vector3 v = prevPos + (position - prevPos) * rdt - interpPos;
	ui32 color = m_color.getARGB();
	vertices[0].set(v.x - (rx + ryz) * s, v.y - rxz * s, v.z - (rz + rxy) * s, color, brightness, u2, v2);
	vertices[1].set(v.x - (rx - ryz) * s, v.y + rxz * s, v.z - (rz - rxy) * s, color, brightness, u2, v1);
	vertices[2].set(v.x + (rx + ryz) * s, v.y + rxz * s, v.z + (rz + rxy) * s, color, brightness, u1, v1);
	vertices[3].set(v.x + (rx - ryz) * s, v.y - rxz * s, v.z + (rz - rxy) * s, color, brightness, u1, v2);
}

}