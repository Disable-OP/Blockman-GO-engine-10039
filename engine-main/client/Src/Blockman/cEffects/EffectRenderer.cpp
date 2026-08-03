#include "EffectRenderer.h"
#include "EntityDiggingFx.h"
#include "Util/Random.h"
#include "World/World.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Render/ActiveRenderInfo.h"

#include "Tessolator/FxRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN 
{

EffectRenderer::EffectRenderer(World* pWorld)
	: m_pWorld(pWorld)
{
	m_mainTexture = TextureManager::Instance()->createTexture("particles.png");
	m_explodeTexture = TextureManager::Instance()->createTexture("explosion.png");
	m_mainTexture->load();
	m_explodeTexture->load();
	m_rand = LordNew Random();
	m_gunFlameRenderable = TessManager::Instance()->createFxRenderable(m_mainTexture, FRP_DEFAULT);
	m_gunFlameRenderable->setViewSpace(true);
}

EffectRenderer::~EffectRenderer()
{
	clearEffects(m_pWorld);
	LordSafeDeleteContainer(m_effectRenderables);
	LordSafeDeleteContainer(m_fullEffectRenderables);
	LordSafeDeleteContainer(m_freeEffectRenderables);
	LordSafeDelete(m_rand);
	LordSafeDelete(m_gunFlameRenderable);
	TextureManager::Instance()->releaseResource(m_mainTexture);
	TextureManager::Instance()->releaseResource(m_explodeTexture);
}


FxRenderable* EffectRenderer::getEffectRenderable(Texture* tex, FX_RENDER_PASS rq, int quadNum)
{
	FxRenderable* pTessRenderable = NULL;

	// fetch from the inuseing queue.
	for (auto it = m_effectRenderables.begin(); it != m_effectRenderables.end(); )
	{
		pTessRenderable = *it;
		int freeQuadNum = pTessRenderable->getFreeQuadNum();
		if (freeQuadNum < quadNum)
		{
			m_fullEffectRenderables.push_back(pTessRenderable);
			auto jt = it++;
			m_effectRenderables.erase(jt);
			continue;
		}

		if (pTessRenderable->getRenderGroup() == rq &&
			pTessRenderable->getTexture() == tex)
		{
			return pTessRenderable;
		}
		++it;
	}

	if (!m_freeEffectRenderables.empty())
	{
		pTessRenderable = m_freeEffectRenderables.front();
		m_freeEffectRenderables.pop_front();

		pTessRenderable->setMaterialParam(tex, rq);
	}
	else
	{
		pTessRenderable = TessManager::Instance()->createFxRenderable(tex, rq);
	}

	m_effectRenderables.push_back(pTessRenderable);
	pTessRenderable->beginPolygon();

	return pTessRenderable;
}

void EffectRenderer::givebackEffectRenderable(FxRenderable* renderable)
{
	auto it = find(m_effectRenderables.begin(), m_effectRenderables.end(), renderable);
	if (it == m_effectRenderables.end())
		return;
	m_effectRenderables.erase(it);
	m_freeEffectRenderables.push_back(renderable);
}

void EffectRenderer::giveheadEffectRenderable(FxRenderable* renderable)
{
	auto it = find(m_effectRenderables.begin(), m_effectRenderables.end(), renderable);
	if (it == m_effectRenderables.end())
		return;
	m_effectRenderables.erase(it);
	m_freeEffectRenderables.push_back(renderable);
}

void EffectRenderer::addEffect(EntityFx* pEffect)
{
	int layer = pEffect->getFXLayer();

	if (m_fxLayers[layer].size() >= 4000)
	{
		m_fxLayers[layer].pop_front();
	}

	m_fxLayers[layer].push_back(pEffect);
}

void EffectRenderer::updateEffects()
{
	for (int i = 0; i < MAX_FX_LAYER; ++i)
	{
		for (auto it = m_fxLayers[i].begin(); it!=m_fxLayers[i].end(); )
		{
			EntityFx* particle = *it;
			particle->onUpdate();

			if (particle->isDead)
			{
				LordSafeDelete(particle);
				it = m_fxLayers[i].erase(it);
			}
			else
				++it;
		}
	}
}

void EffectRenderer::beginRender()
{
	m_freeEffectRenderables.insert(m_freeEffectRenderables.end(), m_effectRenderables.begin(), m_effectRenderables.end());
	m_freeEffectRenderables.insert(m_freeEffectRenderables.end(), m_fullEffectRenderables.begin(), m_fullEffectRenderables.end());
	m_effectRenderables.clear();
	m_fullEffectRenderables.clear();
	m_gunFlameRenderable->beginPolygon();
}

void EffectRenderer::renderParticles(Entity* pEntity, float rdt)
{
	float rx = ActiveRenderInfo::rotationX;
	float rz = ActiveRenderInfo::rotationZ;
	float ryz = ActiveRenderInfo::rotationYZ;
	float rxy = ActiveRenderInfo::rotationXY;
	float rxz = ActiveRenderInfo::rotationXZ;
	// EntityFx::interpPos = pEntity->lastTickPos + (pEntity->position - pEntity->lastTickPos) * rdt;
	Texture* tex = NULL;

	for (int i = 0; i < 3; ++i)
	{
		if (!m_fxLayers[i].empty())
		{
			switch (i)
			{
			case 0: tex = m_mainTexture; break;
			case 1: tex = EntityRenderManager::Instance()->m_BlockTex; break;
			case 2: tex = EntityRenderManager::Instance()->m_ItemTex; break;
			default:  break;
			}

			FxVertexFmt* inject = NULL;
			FxRenderable* renderable = getEffectRenderable(tex, FRP_DEFAULT, 1);
			int freeQuadNum = renderable->getFreeQuadNum();
			renderable->getLockPtr(1, inject);

			// todo.
			for (auto it:m_fxLayers[i])
			{
				EntityFx* particle = it;
				if (particle && !particle->m_isFirstPerspective)
				{
					int brightness = particle->getBrightnessForRender(rdt);
					particle->renderParticle(inject, brightness, rdt, rx, rxz, rz, ryz, rxy);
					inject = inject + 4;
					freeQuadNum--;
					if (freeQuadNum == 0)
					{
						renderable->setFreeQuadNum(freeQuadNum);
						renderable = getEffectRenderable(tex, FRP_DEFAULT, 1);
						freeQuadNum = renderable->getFreeQuadNum();
						renderable->getLockPtr(1, inject);
					}
				}
			}
			renderable->setFreeQuadNum(freeQuadNum);
			if (renderable->empty()) {
				givebackEffectRenderable(renderable);
				giveheadEffectRenderable(renderable);
			}
		}
	}

	if (!m_fxLayers[0].empty())// handle gunFlameFx specially in the first perspective
	{
		FxVertexFmt* inject = NULL;
		int freeQuadNum = m_gunFlameRenderable->getFreeQuadNum();
		m_gunFlameRenderable->getLockPtr(1, inject);

		for (auto it : m_fxLayers[0])
		{
			EntityFx* particle = it;
			if (particle && particle->m_isFirstPerspective)
			{
				int brightness = particle->getBrightnessForRender(rdt);
				particle->renderParticle(inject, brightness, rdt, rx, rxz, rz, ryz, rxy);
				inject = inject + 4;
				freeQuadNum--;

				if (freeQuadNum == 0)
				{
					LordLogInfo("FxRenderable gunFlameFx has some errors!");
					break;
				}
			}
		}
		m_gunFlameRenderable->setFreeQuadNum(freeQuadNum);
	}
}

void EffectRenderer::renderLitParticles(Entity* pEntity, float rdt)
{
	float rx = Math::Cos(pEntity->rotationYaw * Math::DEG2RAD);
	float rz = Math::Sin(pEntity->rotationYaw * Math::DEG2RAD);
	float ryz = -rz * Math::Sin(pEntity->rotationPitch * Math::DEG2RAD);
	float rxy = rx * Math::Sin(pEntity->rotationPitch * Math::DEG2RAD);
	float rxz = Math::Cos(pEntity->rotationPitch * Math::DEG2RAD);

	FxVertexFmt* inject = NULL;
	FxRenderable* renderable = getEffectRenderable(m_explodeTexture, FRP_DEFAULT, 1);
	int freeQuadNum = renderable->getFreeQuadNum();
	renderable->getLockPtr(1, inject);

	for (auto it : m_fxLayers[3])
	{
		EntityFx* particle = it;
		int brightness = particle->getBrightnessForRender(rdt);
		particle->renderParticle(inject, brightness, rdt, rx, rxz, rz, ryz, rxy);
		inject = inject + 4;
		freeQuadNum--;
		if (freeQuadNum == 0)
		{
			renderable->setFreeQuadNum(freeQuadNum);
			renderable = getEffectRenderable(m_explodeTexture, FRP_DEFAULT, 1);
			freeQuadNum = renderable->getFreeQuadNum();
			renderable->getLockPtr(1, inject);
		}
	}
	renderable->setFreeQuadNum(freeQuadNum);
	if (renderable->empty()) {
		givebackEffectRenderable(renderable);
		giveheadEffectRenderable(renderable);
	}
}

void EffectRenderer::endRender()
{
	FxRenderable* pFxRenderable = NULL;
	for (auto it : m_effectRenderables)
	{
		pFxRenderable = it;
		if (!pFxRenderable)
			continue;
		if (pFxRenderable->empty())
			continue;
		pFxRenderable->endPolygon();
		TessManager::Instance()->addFxRenderable(pFxRenderable);
	}
	for (auto it : m_fullEffectRenderables)
	{
		pFxRenderable = it;
		if (!pFxRenderable)
			continue;
		pFxRenderable->endPolygon();
		TessManager::Instance()->addFxRenderable(pFxRenderable);
	}

	if (m_gunFlameRenderable && !m_gunFlameRenderable->empty())
	{
		m_gunFlameRenderable->endPolygon();
		TessManager::Instance()->addFxRenderable(m_gunFlameRenderable);
	}
}

void EffectRenderer::clearEffects(World* pWorld)
{
	m_pWorld = pWorld;

	for (int i = 0; i < 4; ++i)
	{
		LordSafeDeleteContainer(m_fxLayers[i]);
	}
}

void EffectRenderer::addBlockDestroyEffects(const BlockPos& pos, int blockID, int side)
{
	if (blockID <= 0)
		return;
	
	Block* pBlock = BlockManager::sBlocks[blockID];
	int emitSize = 4;

	for (int x = 0; x < emitSize; ++x)
	{
		for (int y = 0; y < emitSize; ++y)
		{
			for (int z = 0; z < emitSize; ++z)
			{
				Vector3 emitPos = Vector3(pos) + (Vector3(float(x), float(y), float(z)) + Vector3::HALF) / float(emitSize);
				EntityDiggingFX* pfx = LordNew EntityDiggingFX(m_pWorld, emitPos, emitPos - Vector3(pos) - Vector3::HALF, pBlock, side);
				pfx->applyColourMultiplier(pos);
				addEffect(pfx);
			}
		}
	}
}

void EffectRenderer::addBlockHitEffects(const BlockPos& pos, int side)
{
	int blockID = m_pWorld->getBlockId(pos);

	if (blockID <= 0)
		return;

	Block* pBlock = BlockManager::sBlocks[blockID];
	float bound = 0.1F;
	Vector3 emitPos = (pBlock->getBlockBoundsMax() - pBlock->getBlockBoundsMin() - (bound * 2.f)) + bound * pBlock->getBlockBoundsMin();
	emitPos.x = pos.x + m_rand->nextFloat() * emitPos.x;
	emitPos.y = pos.y + m_rand->nextFloat() * emitPos.y;
	emitPos.z = pos.z + m_rand->nextFloat() * emitPos.z;

	switch (side)
	{
	case 0:	emitPos.y = pos.y + pBlock->getBlockBoundsMinY() - bound; break;
	case 1:	emitPos.y = pos.y + pBlock->getBlockBoundsMaxY() + bound; break;
	case 2: emitPos.z = pos.z + pBlock->getBlockBoundsMinZ() - bound; break;
	case 3: emitPos.z = pos.z + pBlock->getBlockBoundsMaxZ() + bound; break;
	case 4: emitPos.x = pos.x + pBlock->getBlockBoundsMinX() - bound; break;
	case 5: emitPos.x = pos.x + pBlock->getBlockBoundsMaxX() + bound; break;
	default: break;
	}
	
	EntityDiggingFX* pfx = LordNew EntityDiggingFX(m_pWorld, emitPos, Vector3::ZERO, pBlock, m_pWorld->getBlockMeta(pos));
	pfx->applyColourMultiplier(pos);
	pfx->multiplyVelocity(0.2f);
	pfx->multipleParticleScaleBy(0.6f);
	addEffect(pfx);
}

int EffectRenderer::getFxCount()
{
	int ret = 0;
	for (int i = 0; i < MAX_FX_LAYER; ++i)
		ret += m_fxLayers[i].size();
	return ret;
}

}