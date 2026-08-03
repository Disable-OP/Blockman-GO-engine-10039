#include "TileEntityRenderMgr.h"
#include "TileEntityRenders.h"
#include "TileEntityRenderable.h"

#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"

#include "BM_TypeDef.h"
#include "TileEntity/TileEntity.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"

#include "Tessolator/TessRenderable.h"
#include "Tessolator/FontRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{

TileEntityRenderMgr::TileEntityRenderMgr()
{
	tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_SIGN), LordNew TESignRenderer()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_MOBSWPAWER), LordNew TEMobSpawnerRender()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_PISTON), LordNew TEPistonRender()));
	tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_CHEST), LordNew TEChestRender()));
	tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_ENDERCHEST), LordNew TEEnderChestRenderer()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_ENCHANTTABLE), LordNew TEEnchantmentTableRender()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_AIRPORTAL), LordNew TEEndPortalRender()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_BEACON), LordNew TEBeaconRender()));
	//tileEntityRenderMap.insert(std::make_pair(int(TILE_ENTITY_SKULL), LordNew TileEntitySkullRenderer()));

	for (auto it : tileEntityRenderMap)
	{
		TileEntityRender* pRender = it.second;
		pRender->setTileEntityRenderer(this);
	}
	tileEntityRenderSelector.insert(tileEntityRenderMap.begin(), tileEntityRenderMap.end());
}

TileEntityRenderMgr::~TileEntityRenderMgr()
{
	for (auto it : tileEntityRenderMap)
	{
		LordSafeDelete(it.second);
	}
	tileEntityRenderMap.clear();

	for (auto it : tileEntityRenderableMap)
	{
		LordSafeDelete(it.second);
	}
	tileEntityRenderableMap.clear();
}

TileEntityRender* TileEntityRenderMgr::getSpecialRendererForClass(int type)
{
	TileEntityRender* pResult = nullptr;
	auto it = tileEntityRenderSelector.find(type);
	if (it != tileEntityRenderSelector.end())
		pResult = it->second;

	if (pResult == nullptr && type != TILE_ENTITY_BASE)
	{
		pResult = getSpecialRendererForClass(TileEntity::getSuperClassID(type));
		tileEntityRenderSelector.insert(std::make_pair(type, pResult));
	}

	return pResult;
}

TileEntityRenderable* TileEntityRenderMgr::getTileEntityRenderable(TileEntity* pEntity)
{
	TileEntityRenderable* pResult = NULL;
	
	auto it = tileEntityRenderableMap.find(pEntity);
	if (it == tileEntityRenderableMap.end())
	{
		pResult = LordNew TileEntityRenderable(pEntity);
		tileEntityRenderableMap.insert(std::make_pair(pEntity, pResult));
	}
	else
		pResult = it->second;
	return pResult;
}

bool TileEntityRenderMgr::hasSpecialRenderer(TileEntity* pTileEntity)
{
	return getSpecialRendererForEntity(pTileEntity) != nullptr;
}

TileEntityRender* TileEntityRenderMgr::getSpecialRendererForEntity(TileEntity* pTileEntity)
{
	return pTileEntity == nullptr ? nullptr: getSpecialRendererForClass(pTileEntity->getClassID());
}

void TileEntityRenderMgr::cacheActiveRenderInfo(World* pWorld, EntityLivingBase* pLiving, float rdt)
{
	if (worldObj != pWorld)
		setWorld(pWorld);

	// renderEngine = par2TextureManager;
	entityLivingPlayer = pLiving;
	// fontRenderer = par3FontRenderer;
	playerYaw = pLiving->prevRotationYaw + (pLiving->rotationYaw - pLiving->prevRotationYaw) * rdt;
	playerPitch = pLiving->prevRotationPitch + (pLiving->rotationPitch - pLiving->prevRotationPitch) * rdt;
	playerPos = pLiving->lastTickPos + (pLiving->position - pLiving->lastTickPos) * rdt;
}

void TileEntityRenderMgr::renderTileEntity(TileEntity* pTileEntity, float rdt)
{
	if (pTileEntity->getDistanceFrom(playerPos) < pTileEntity->getMaxRenderDistanceSquared())
	{
		int brightUV = worldObj->getLightBrightnessForSkyBlocks(pTileEntity->m_pos, 0);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		renderTileEntityAt(pTileEntity, pTileEntity->m_pos, brightness, rdt);
	}
}

void TileEntityRenderMgr::renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt)
{
	TileEntityRender* pRender = getSpecialRendererForEntity(pTileEntity);

	if (pRender)
		pRender->renderTileEntityAt(pTileEntity, pos, bright, rdt);
}

void TileEntityRenderMgr::renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pRenderable)
{
	TileEntityRender* pRender = getSpecialRendererForEntity(pTileEntity);

	if (pRender)
		pRender->renderTileEntityToTessRender(pTileEntity, pos, bright, pRenderable);
}

void TileEntityRenderMgr::setWorld(World* pWorld)
{
	worldObj = pWorld;
	for(auto it : tileEntityRenderMap)
	{
		TileEntityRender* pRender = it.second;
		if (pRender != nullptr)
		{
			pRender->onWorldChange(pWorld);
		}
	}
}

TessRenderable* TileEntityRenderMgr::getTessRenderable(Texture* texture, TESS_RENDER_PASS pass, int quadNum)
{
	TessRenderable* pResult = nullptr;

	// fetch from the inuseing queue.
	for (auto it = m_blockRender.begin(); it != m_blockRender.end();)
	{
		pResult = *it;
		int freeQuadNum = pResult->getFreeQuadNum();
		if (freeQuadNum < quadNum)
		{
			m_fullBlockRender.push_back(pResult);
			auto jt = it++;
			m_blockRender.erase(jt);
			continue;
		}

		if (pResult->getRenderGroup() == pass &&
			pResult->getTexture() == texture)
		{
			return pResult;
		}
		++it;
	}

	if (!m_freeBlockRender.empty())
	{
		pResult = m_freeBlockRender.front();
		m_freeBlockRender.pop_front();

		pResult->setMaterialParam(texture, pass);
	}
	else
	{
		pResult = LordNew TessRenderable();
	}

	m_blockRender.push_back(pResult);
	pResult->beginPolygon();

	return pResult;
}

FontRenderable* TileEntityRenderMgr::getFontRenderable(Texture* fontTex, bool zTestEnabled)
{
	FontRenderable* pResult = nullptr;

	// fetch from the inuseing queue.
	for (auto it = m_fontRender.begin(); it != m_fontRender.end();)
	{
		pResult = *it;
		int freeCharNum = pResult->getFreeCharNum();
		if (freeCharNum < 1)
		{
			m_fullFontRender.push_back(pResult);
			auto jt = it++;
			m_fontRender.erase(jt);
			continue;
		}

		if (pResult->getTexture() == fontTex && pResult->isZTestEnabled() == zTestEnabled)
		{
			return pResult;
		}
		++it;
	}

	if (!m_freeFontRender.empty())
	{
		pResult = m_freeFontRender.front();
		m_freeFontRender.pop_front();

		pResult->setMaterialParam(fontTex);
	}
	else
	{
		pResult = TessManager::Instance()->createFontRenderable();
	}

	m_fontRender.push_back(pResult);
	pResult->beginPolygon();
	pResult->setMaterialParam(fontTex);
	pResult->setZTestEnabled(zTestEnabled);

	return pResult;
}

void TileEntityRenderMgr::beginRender()
{
	m_freeBlockRender.insert(m_freeBlockRender.end(), m_blockRender.begin(), m_blockRender.end());
	m_freeBlockRender.insert(m_freeBlockRender.end(), m_fullBlockRender.begin(), m_fullBlockRender.end());
	m_blockRender.clear();
	m_fullBlockRender.clear();

	m_freeFontRender.insert(m_freeFontRender.end(), m_fontRender.begin(), m_fontRender.end());
	m_freeFontRender.insert(m_freeFontRender.end(), m_fullFontRender.begin(), m_fullFontRender.end());
	m_fontRender.clear();
	m_fullFontRender.clear();
}

void TileEntityRenderMgr::endRender()
{
	for (auto& it : m_blockRender)
	{
		if (!it)
			continue;
		it->endPolygon();
		TessManager::Instance()->addTessRenderable(it);
	}
	for (auto it : m_fullBlockRender)
	{
		if (!it)
			continue;
		it->endPolygon();
		TessManager::Instance()->addTessRenderable(it);
	}

	for (auto& it : m_fontRender)
	{
		if (!it)
			continue;
		it->endPolygon();
		TessManager::Instance()->addFontRenderable(it);
	}

	for (auto& it : m_fullFontRender)
	{
		if (!it)
			continue;
		it->endPolygon();
		TessManager::Instance()->addFontRenderable(it);
	}
}

}