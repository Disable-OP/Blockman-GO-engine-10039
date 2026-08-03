#include "ItemModelMgr.h"
#include "Common.h"

#include "cBlock/cBlock.h"

#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/RenderBlocks.h"
#include "Entity/EntityPlayer.h"

#include "Tessolator/ItemRenderable.h"
#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/EnchantRenderQueue.h"

namespace BLOCKMAN
{

ItemBlockMesh::ItemBlockMesh(int blockID, int meta)
	: m_blockID(blockID)
	, m_meta(meta)
	, m_vertices(nullptr)
	, m_colorCached(nullptr)
	, m_quadNum(0)
	, m_texture(nullptr)
{}

ItemBlockMesh::~ItemBlockMesh()
{
	LordSafeFree(m_vertices);
	LordSafeFree(m_colorCached);
	m_quadNum = 0;
}

bool ItemBlockMesh::buildMesh()
{
	LordAssert(m_blockID > 0 && m_blockID < BlockManager::MAX_BLOCK_COUNT);
	if (isValid())
		return false;
	// render the block to template tessRenderable.
	Block* pBlock = BlockManager::sBlocks[m_blockID];
	if (!pBlock)
		return false;
	Texture* blockTex = TextureAtlasRegister::Instance()->getTextureAtlas(cBlockManager::scBlocks[m_blockID]->getAtlasName())->getTexture();
	LordAssert(blockTex);

	TessRenderable* tessRenderable = TessManager::Instance()->createTessRenderable(blockTex, TRP_SOLID);
	RenderBlocks* pRenderBlocks = LordNew RenderBlocks();
	pRenderBlocks->setSpecialRenderable(tessRenderable);
	tessRenderable->beginPolygon();
	pRenderBlocks->renderBlockAsItem(pBlock, cBlockManager::scBlocks[pBlock->getBlockID()], m_meta);
	m_texture = tessRenderable->getTexture(); // tessRenderable maybe changed for chest.
	m_quadNum = tessRenderable->getQuadNum();
	bool result = false;
	do 
	{
		ERROR_BREAK((m_quadNum > 0));
		// copy the vertex data from tessRenderable's vertex.
		
		m_vertices = (SkinedVertexFmt*)LordMalloc(m_quadNum * 4 * sizeof(SkinedVertexFmt));
		m_colorCached = (int*)LordMalloc(m_quadNum * 4 * sizeof(int));
		TessVertexFmt* tessVertices = (TessVertexFmt*)tessRenderable->getVertexMenory();
		Vector3 vecTrans(-0.5f, 0.f, -0.5f);
		m_aabb.reset();
		for (int i = 0; i < m_quadNum; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				int idx = i * 4 + j;
				memcpy(m_vertices + idx, tessVertices + idx, sizeof(TessVertexFmt));
				Vector3 newPos = tessVertices[idx].GetPosition() + vecTrans;
				m_aabb.addPoint(newPos);
				m_vertices[idx].setPositionEx(newPos.x, newPos.y, newPos.z);
				m_colorCached[idx] = tessVertices[idx].GetLightColor();
				m_vertices[idx].setIndex(0);
			}
		}
		result = true;
	} while (0);

	LordSafeDelete(pRenderBlocks);
	TessManager::Instance()->destroyTessRenderable(tessRenderable);
	return result;
}

bool ItemBlockMesh::renderToRenderable(SkinedRenderable* pRenderable, int brightness, int boneIdx)
{
	if (!isValid())
		return false;
	
	SkinedVertexFmt* outputVertices = NULL;
	if (!pRenderable->getLockPtr(m_quadNum, outputVertices))
		return false;
	
	for (int i = 0; i < m_quadNum * 4; ++i)
	{
		m_vertices[i].SetLightColor((Color(brightness) * Color(m_colorCached[i])).getARGB());
		//m_vertices[i].setSkyLight(sky, block);
		m_vertices[i].setIndex(boneIdx);
	}

	memcpy(outputVertices, m_vertices, btyesInMemory());

	return true;
}

int ItemBlockMesh::getKey() const
{
	return (m_blockID << 8) | m_meta;
}

bool ItemBlockMesh::isValid() const
{
	return m_vertices != NULL;
}

int ItemBlockMesh::btyesInMemory() const
{
	return m_quadNum * 4 * sizeof(SkinedVertexFmt);
}


PackMassRenderable::PackMassRenderable(Texture* tex, SKINED_RENDER_PASS rp)
	: m_worldMat(Matrix4::IDENTITY)
	, m_lastBlockKey(0)
	, m_blockCount(0)
{
	m_pBoneMats = (Vector4*)LordMalloc(SkinedRenderable::s_maxBone * sizeof(Vector4) * 3);
	m_pRenderable = TessManager::Instance()->createSkinedRenderable(tex, rp);
	m_pRenderable->setWorldMatrix(&m_worldMat);
	m_pRenderable->setBoneCount(0);
	m_pRenderable->setBoneMatrices(m_pBoneMats);
	m_pRenderable->setExtraColor(Color::WHITE);
}

PackMassRenderable::~PackMassRenderable()
{
	LordSafeFree(m_pBoneMats);
	TessManager::Instance()->destroySkinedRenderable(m_pRenderable);
}

int PackMassRenderable::getFreeQuadNum() const
{
	return m_pRenderable->getFreeQuadNum();
}

int PackMassRenderable::getFreeBoneCount() const
{
	return SkinedRenderable::s_maxBone - m_blockCount;
}

SKINED_RENDER_PASS PackMassRenderable::getRenderGroup() const
{
	LordAssert(m_pRenderable);
	return m_pRenderable->getRenderGroup();
}

Texture* PackMassRenderable::getTexture() const
{
	LordAssert(m_pRenderable);
	return m_pRenderable->getTexture();
}

void PackMassRenderable::setMaterialParam(Texture* tex, SKINED_RENDER_PASS rq)
{
	m_pRenderable->setMaterialParam(tex, rq);
}

bool PackMassRenderable::addBoneMatrix(const Matrix4& worldMat)
{
	LordAssert(m_blockCount <= SkinedRenderable::s_maxBone);
	if (getFreeBoneCount() <= 0)
		return false;
	m_pBoneMats[3 * m_blockCount + 0].x = worldMat.m00;
	m_pBoneMats[3 * m_blockCount + 0].y = worldMat.m10;
	m_pBoneMats[3 * m_blockCount + 0].z = worldMat.m20;
	m_pBoneMats[3 * m_blockCount + 0].w = worldMat.m30;

	m_pBoneMats[3 * m_blockCount + 1].x = worldMat.m01;
	m_pBoneMats[3 * m_blockCount + 1].y = worldMat.m11;
	m_pBoneMats[3 * m_blockCount + 1].z = worldMat.m21;
	m_pBoneMats[3 * m_blockCount + 1].w = worldMat.m31;

	m_pBoneMats[3 * m_blockCount + 2].x = worldMat.m02;
	m_pBoneMats[3 * m_blockCount + 2].y = worldMat.m12;
	m_pBoneMats[3 * m_blockCount + 2].z = worldMat.m22;
	m_pBoneMats[3 * m_blockCount + 2].w = worldMat.m32;

	m_blockCount++;
	return true;
}

bool PackMassRenderable::addItemBlock(ItemBlockMesh* blockMesh, int bright, const Matrix4& worldMat)
{
	int quadNum = blockMesh->getQuadNum();
	if (getFreeQuadNum() < quadNum)
		return false;
	if (getFreeBoneCount() <= 0)
		return false;

	blockMesh->renderToRenderable(m_pRenderable, bright, m_blockCount);
	addBoneMatrix(worldMat);

	m_lastBlockKey = blockMesh->getKey();
	return true;
}

bool PackMassRenderable::addPackMass(SkinedVertexFmt* pVertices, int quadCount, int bright, const Matrix4& worldMat)
{
	if (getFreeQuadNum() < quadCount)
		return false;
	if (getFreeBoneCount() <= 0)
		return false;

	// fill the vertices data with the bone index.
	SkinedVertexFmt* outputVertices = NULL;
	Color b(bright);
	if (!m_pRenderable->getLockPtr(quadCount, outputVertices))
		return false;

	for (int i = 0; i < quadCount * 4; ++i)
	{
		pVertices[i].SetLightColor(bright);
		pVertices[i].setIndex(m_blockCount);
	}
	memcpy(outputVertices, pVertices, sizeof(SkinedVertexFmt) * 4 * quadCount);

	addBoneMatrix(worldMat);

	m_lastBlockKey = (size_t)pVertices;
	return true;
}

void PackMassRenderable::setViewSpace(bool viewspace)
{
	m_pRenderable->setViewSpace(viewspace);
}

void PackMassRenderable::setBoneMatrix(int boneIdx, const Matrix4& worldMat)
{
	LordAssert(boneIdx >= 0);
	LordAssert(boneIdx <= SkinedRenderable::s_maxBone);
	m_pBoneMats[3 * boneIdx + 0].x = worldMat.m00;
	m_pBoneMats[3 * boneIdx + 0].y = worldMat.m10;
	m_pBoneMats[3 * boneIdx + 0].z = worldMat.m20;
	m_pBoneMats[3 * boneIdx + 0].w = worldMat.m30;

	m_pBoneMats[3 * boneIdx + 1].x = worldMat.m01;
	m_pBoneMats[3 * boneIdx + 1].y = worldMat.m11;
	m_pBoneMats[3 * boneIdx + 1].z = worldMat.m21;
	m_pBoneMats[3 * boneIdx + 1].w = worldMat.m31;

	m_pBoneMats[3 * boneIdx + 2].x = worldMat.m02;
	m_pBoneMats[3 * boneIdx + 2].y = worldMat.m12;
	m_pBoneMats[3 * boneIdx + 2].z = worldMat.m22;
	m_pBoneMats[3 * boneIdx + 2].w = worldMat.m32;
}

void PackMassRenderable::setTextureAtlas(const Vector4& atlas)
{
	if (!m_pRenderable)
		return;

	if (!(m_pRenderable->getRenderGroup() == SRP_ENCHANT || m_pRenderable->getRenderGroup() == SRP_ENCHANT_NODEPTH))
		return;

	EnchantSkinedRenderable* pEnchant = (EnchantSkinedRenderable*)m_pRenderable;
	pEnchant->setTextureAtlas(atlas);
}

void PackMassRenderable::beginPolygon()
{
	m_pRenderable->beginPolygon();
	m_blockCount = 0;
}

void PackMassRenderable::endPolygon()
{
	m_pRenderable->endPolygon();
	m_pRenderable->setBoneCount(m_blockCount);
	// TessManager::Instance()->addSkinedRenderable(m_pRenderable);
}

void PackMassRenderable::updateRenderQueue()
{
	if(m_pRenderable->getRenderGroup() == SRP_ENCHANT || m_pRenderable->getRenderGroup() == SRP_ENCHANT_NODEPTH)
		TessManager::Instance()->addEnchantRenderable(m_pRenderable);
	else
		TessManager::Instance()->addSkinedRenderable(m_pRenderable);
}

ItemModelManager::ItemModelManager()
{
	for (int i = 0; i < ITEM_TESS_COUNT; ++i)
	{
		m_colorBuffer[i] = NULL;
		m_imageWidth[i] = 0;
		m_imageHeight[i] = 0;
	}
	m_ItemMeshs = (ItemRenderMesh**)LordMalloc(sizeof(ItemRenderMesh*) * Item::MAX_ITEM_COUNT);
	memset(m_ItemMeshs, 0, sizeof(ItemRenderMesh*) * Item::MAX_ITEM_COUNT);
}

ItemModelManager::~ItemModelManager()
{
	for (int i = 0; i < ITEM_TESS_COUNT; ++i)
	{
		// block image is from MemTextureAtlas, will release in MemTextureAtlas::~MemTextureAtlas;
		if (i == static_cast<int>(ItemRenderType::BLOCK))
			continue;
		LordSafeFree(m_colorBuffer[i]);
	}

	for (int i = 0; i < Item::MAX_ITEM_COUNT; ++i)
	{
		if (m_ItemMeshs[i] == NULL)
			continue;

		TessManager::Instance()->destroyItemRenderMesh(m_ItemMeshs[i]);
		m_ItemMeshs[i] = NULL;
	}
	LordSafeFree(m_ItemMeshs);

	for (ItemBlockMeshMap::iterator it = m_BlockMeshs.begin(); it != m_BlockMeshs.end(); ++it)
	{
		ItemBlockMesh* pBlockMesh = it->second;
		LordDelete(pBlockMesh);
	}
	m_BlockMeshs.clear();

	for (ItemRenderMeshMap::iterator it = m_mapItemMeshs.begin(); it != m_mapItemMeshs.end(); ++it) {
		ItemRenderMesh *pItemMesh = it->second;
		TessManager::Instance()->destroyItemRenderMesh(pItemMesh);
	}
	m_mapItemMeshs.clear();
}

//void ItemModelManager::setAtlasFor32PixBlock(const String & atlasname)
//{
//	m_pAtlasBlock = LordNew TextureAtlas();
//	LordAssert(m_pAtlasBlock->loadFromJson(atlasname));
//}

void ItemModelManager::setImage(int tessID, const String& imageName)
{
	LordAssert(!m_colorBuffer[tessID]);
	DataStream* pData = ResourceGroupManager::Instance()->openResource(imageName);
	Image* pImage = Image::loadFromDataStream(pData, imageName);
	m_imageWidth[tessID] = pImage->getWidth();
	m_imageHeight[tessID] = pImage->getHeight();
	m_colorBuffer[tessID] = (ui32*)LordMalloc(sizeof(ui32)* m_imageWidth[tessID] * m_imageHeight[tessID]);
	for (int i = 0; i < m_imageHeight[tessID]; ++i)
	{
		for (int j = 0; j < m_imageWidth[tessID]; ++j)
		{
			m_colorBuffer[tessID][i * m_imageWidth[tessID] + j] = pImage->getColor(j, i, 0).getABGR();
		}
	}
	LordSafeDelete(pImage);
	LordSafeDelete(pData);
}

void ItemModelManager::initBlockImage(MemTextureAtlas* atlas)
{
	int tessID = static_cast<int>(ItemRenderType::BLOCK);
	LordAssert(!m_colorBuffer[tessID]);
	LordAssert(atlas);
	LordAssert(m_pBlockAtlas == NULL);

	m_pBlockAtlas = atlas;
	m_imageWidth[tessID] = atlas->getMipWidth(m_blockMip);
	m_imageHeight[tessID] = atlas->getMipHeight(m_blockMip);
	m_colorBuffer[tessID] = atlas->getPixelMemMip(m_blockMip);
}
//
//// block's image is from 64bit.
//void ItemModelManager::setImage(int tessID, ui32* imagePtr, int width, int height)
//{
//	LordAssert(!m_colorBuffer[tessID]);
//	m_imageWidth[tessID] = width;
//	m_imageHeight[tessID] = height;
//	m_colorBuffer[tessID] = (ui32*)LordMalloc(sizeof(ui32)* width * height);
//	memcpy(m_colorBuffer[tessID], imagePtr, sizeof(ui32)* width * height);
//}

ItemBlockMesh* ItemModelManager::getItemBlockMesh(int blockID, int meta)
{
	int key = (blockID << 8) | meta;
	ItemBlockMeshMap::iterator it = m_BlockMeshs.find(key);
	if (it != m_BlockMeshs.end())
		return it->second;

	ItemBlockMesh* pBlockMesh = LordNew ItemBlockMesh(blockID, meta);
	pBlockMesh->buildMesh();
	m_BlockMeshs.insert(std::make_pair(key, pBlockMesh));
	return pBlockMesh;
}

ItemRenderMesh* ItemModelManager::getItemTesselation(ItemStackPtr pStack, EntityPlayer* pPlayer)
{
	int nSeg = -1;
	int itemID = pStack->itemID;
	if (itemID == Item::bow->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow;
		nSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
	}
	if (itemID == Item::bow_red->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow_red;
		nSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
	}
	if (itemID == Item::bow_black->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow_black;
		nSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
	}
	cItem* pItem = ItemClient::citemsList[itemID];
	LordAssert(pItem);

	int nItemMeta = pStack->getItemDamage();
	if (nSeg != -1) {
		//		assert(nItemMeta == 0);
		nItemMeta = nSeg;
	}
	if (pItem->IsModelWithMeta()) {
		LordAssert(itemID < 0xFFFF && nItemMeta < 0xFFFF);
		uint32_t id = itemID << 16 | nItemMeta;
		ItemRenderMeshMap::iterator it = m_mapItemMeshs.find(id);
		if (it != m_mapItemMeshs.end())
			return it->second;
	}
	else {
		if (m_ItemMeshs[itemID])
			return m_ItemMeshs[itemID];
	}

	int tessID = static_cast<int>(ItemClient::getRenderType(pStack));

	AtlasSprite* pSprite = pItem->getIconFromDamageForRenderPass(pStack->getItemDamage(), 0);
	Color color1 = Color(pItem->getColorFromItemStack(pStack, 0));
	color1.a = 1.f;
	if (pStack->itemID == Item::bow->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow;
		int durSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
		if (durSeg > 0)
			pSprite = pBow->getItemIconForUseDuration(durSeg - 1);
	}
	if (pStack->itemID == Item::bow_red->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow_red;
		int durSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
		if (durSeg > 0)
			pSprite = pBow->getItemIconForUseDuration(durSeg - 1);
	}
	if (pStack->itemID == Item::bow_black->itemID && pPlayer && pPlayer->itemInUse)
	{
		cItemBow* pBow = (cItemBow*)Item::bow_black;
		int durSeg = pBow->getDurationSeg(pPlayer->getItemInUseDuration());
		if (durSeg > 0)
			pSprite = pBow->getItemIconForUseDuration(durSeg - 1);
	}
	LordAssert(pSprite);
	Vector2i pos = pSprite->getFramePos();
	Vector2i size = pSprite->getFrameSize();

	// 补丁代码，pSprite是64位高精度的贴图，如果是Block，需要转成低精度。
	if (tessID == 0)
	{
		const String& atlasName = pSprite->getSpriteName();
		AtlasSprite* atlas32 = m_pBlockAtlas->getMipAtlasScript(m_blockMip, atlasName);
		if (atlas32)
		{
			pos = atlas32->getFramePos();
			size = atlas32->getFrameSize();
		}
		else
		{
			pos = Vector2i(0, 0);
			size = Vector2i(32, 32);
		}
	}

	Vector2i overlay_pos = Vector2i::ZERO;
	Vector2i overlay_size = Vector2i::ZERO;
	Color color2 = Color::WHITE;
	if (pItem->requiresMultipleRenderPasses())
	{
		AtlasSprite* pSprite_overlay = pItem->getIconFromDamageForRenderPass(pStack->getItemDamage(), 1);
		LordAssert(pSprite_overlay);
		color2 = Color(pItem->getColorFromItemStack(pStack, 1));
		std::swap(color1.r, color1.b);
		color2.a = 1.f;
		overlay_pos = pSprite_overlay->getFramePos();
		overlay_size = pSprite_overlay->getFrameSize();
	}

	ItemRenderMesh* pMesh = TessManager::Instance()->createItemRenderMesh(IRP_SOLID);

	if (!tesselateModel(tessID, pMesh, pos, size, color1, overlay_pos, overlay_size, color2))
	{
		TessManager::Instance()->destroyItemRenderMesh(pMesh);
		pMesh = NULL;
		return NULL;
	}

	if (pItem->IsModelWithMeta()) {
		uint32_t id = itemID << 16 | nItemMeta;
		LordAssert(m_mapItemMeshs.find(id) == m_mapItemMeshs.end())
		m_mapItemMeshs[id] = pMesh;
	}
	else {
		m_ItemMeshs[itemID] = pMesh;
	}

	return pMesh;
}


struct ItemStripe
{
	Vector2i bias2D;
	ui8 length;
	ui32 color;

	ItemStripe()
	{
		bias2D.x = 999;
		bias2D.y = 999;
		length = 0;
		color = 0;
	}

	void Reset(const Vector2i& i_bias2D, ui32 c)
	{
		bias2D = i_bias2D;
		length = 1;
		color = c;
	}

	void Reset()
	{
		bias2D = Vector2i(999, 999);
		length = 0;
		color = 0;
	}

	static bool ColorEqual(ui32 c1, ui32 c2)
	{
		i16 b = (i16)(c1 >> 0);
		i16 g = (i16)(c1 >> 8);
		i16 r = (i16)(c1 >> 16);

		i16 ab = (i16)(c2 >> 0);
		i16 ag = (i16)(c2 >> 8);
		i16 ar = (i16)(c2 >> 16);

		int v = 32;
		return  Math::Abs(r - ar) < v && Math::Abs(g - ag) < v && Math::Abs(b - ab) < v;
	}
	
	bool CanMerge(const ItemStripe& other)
	{
		return length == other.length && ColorEqual(color, other.color);
	}
};

bool ItemModelManager::tesselateModel(int tessID, ItemRenderMesh* pOut, const Vector2i pos1, const Vector2i size1, const Color& c1,
	const Vector2i& pos2, const Vector2i& size2, const Color& c2)
{
	if (!pOut)
		return false;

	// check paramters.
	if (size2 != Vector2i::ZERO)
	{
		LordAssert(size2 == size1);
	}

	int imageWidth = m_imageWidth[tessID];
	int imageHeight = m_imageHeight[tessID];
	ui32* colorBuffer = m_colorBuffer[tessID];

	if (pos1.x + size1.x < 0)
		return false;
	if (pos1.x + size1.x > imageWidth)
		return false;
	if (pos1.y + size1.y < 0)
		return false;
	if (pos1.y + size1.y > imageHeight)
		return false;

	// make a image segment for tesselate.
	ui32* imgSegment = (ui32*)LordMalloc(sizeof(ui32) * size1.x * size1.y);
	memset(imgSegment, 0, sizeof(ui32) * size1.x * size1.y);
	for (int h = 0; h < size1.y; ++h)
	{
		for (int w = 0; w < size1.x; ++w)
		{
			ui32 color1 = colorBuffer[(h + pos1.y)*imageWidth + (w + pos1.x)];
			color1 = (Color(color1)* c1).getARGB();
			ui32 color2 = 0x00000000;
			if (size2 != Vector2i::ZERO)
			{
				color2 = colorBuffer[(h + pos2.y)*imageWidth + (w + pos2.x)];
				color2 = (Color(color2) * c2).getARGB();
			}
			if (((color1 & 0xFF000000) >> 24) > 16)
				imgSegment[h * size1.x + w] = color1;
			else
				imgSegment[h * size1.x + w] = color2;
		}
	}

	typedef vector<std::pair<Vector4i, ui32> >::type EdgeArr;
	typedef vector<std::pair<Vector4i, ui32> >::type QuadArr;
	EdgeArr edges;
	QuadArr quads;

	map<Vector2i, ItemStripe>::type stripeMap;//pos,length
	ItemStripe stripe;
	for (int h = 0; h < size1.y; ++h)
	{
		for (int w = 0; w < size1.x; ++w)
		{
			int index = h * size1.x + w;

			bool shouldGen = ((imgSegment[index] & 0xFF000000) >> 24) > 16;
			ui32 ncolor = imgSegment[index];
			if ((!ItemStripe::ColorEqual(stripe.color, ncolor)) || shouldGen) // 4 as 0.1f
			{
				if (stripe.length != 0 && stripe.color != 0)
					stripeMap.insert(std::make_pair(stripe.bias2D, stripe));
				if (shouldGen)
				{
					Vector2i pos(w, h);
					stripe.Reset(pos, ncolor);
					if (w == size1.x - 1)
					{
						stripeMap.insert(std::make_pair(stripe.bias2D, stripe));
						stripe.Reset();
					}
				}
				else
					stripe.Reset();
			}
			else
			{
				stripe.length++;
				if (w == size1.x - 1 && stripe.color != 0)
				{
					stripeMap.insert(std::make_pair(stripe.bias2D, stripe));
					stripe.Reset();
				}
			}
		}
	}

	auto it = stripeMap.begin();
	for (; it != stripeMap.end(); ++it)
	{
		ItemStripe& curStripe = it->second;
		Vector2i curBias = it->first;//for curbx curby ==> z , y
		int curLen = curStripe.length;
		int curheight = 1;

		if (curLen == 0)
			continue;

		for (int y = curBias.y + 1; y < size1.y; ++y)
		{
			Vector2i searchBias(curBias.x, y);
			auto it_search = stripeMap.find(searchBias);
			if (it_search != stripeMap.end())
			{
				if (it_search->second.CanMerge(curStripe))
				{
					stripeMap[searchBias].length = 0;
					curheight++;
				}
				else
					break;
			}
			else
				break;
		}
		quads.push_back({ {curBias.x, curBias.y, curLen, curheight}, curStripe.color });
	}

	// add the egde from picture.
	ui8* greedDown =  (ui8*)LordMalloc(size1.x * size1.y * sizeof(ui8));
	ui8* greedUp =    (ui8*)LordMalloc(size1.x * size1.y * sizeof(ui8));
	ui8* greedRight = (ui8*)LordMalloc(size1.x * size1.y * sizeof(ui8));
	ui8* greedLeft =  (ui8*)LordMalloc(size1.x * size1.y * sizeof(ui8));
	memset(greedDown,  0, size1.x * size1.y * sizeof(ui8));  // vert
	memset(greedUp,    0, size1.x * size1.y * sizeof(ui8));  // horz
	memset(greedRight, 0, size1.x * size1.y * sizeof(ui8));  // vert
	memset(greedLeft,  0, size1.x * size1.y * sizeof(ui8));  // horz

	for (int h = 0; h < size1.y; ++h)
	{
		for (int w = 0; w < size1.x; ++w)
		{
			int index = h * size1.x + w;

			bool border = false;
			if (h == 0 || h == size1.y - 1 || w == 0 || w == size1.x - 1)
				border = true;

			bool currentGen = ((imgSegment[index] & 0xFF000000) >> 24) > 16;
			bool rightGen = currentGen;
			bool bottomGen = currentGen;
			ui32 currentColor = imgSegment[index];
			ui32 rightColor = currentColor;
			ui32 bottomColor = currentColor;
			if (h != size1.y - 1)
			{
				int indexBottom = index + size1.x;
				bottomGen = ((imgSegment[indexBottom] & 0xFF000000) >> 24) > 16;
				bottomColor = imgSegment[indexBottom];
			}
			if (w != size1.x - 1)
			{
				int indexRight = index + 1;
				rightGen = ((imgSegment[indexRight] & 0xFF000000) >> 24) > 16;
				rightColor = imgSegment[indexRight];
			}
			
			if (currentGen != rightGen)
			{
				if (currentGen)
				{
					edges.push_back(std::make_pair(Vector4i(w + 1, h, w + 1, h + 1), imgSegment[index]));
				}
				else
					edges.push_back(std::make_pair(Vector4i(w + 1, h + 1, w + 1, h), imgSegment[index + 1]));

			}
			if (currentGen != bottomGen)
			{
				if (currentGen)
					edges.push_back(std::make_pair(Vector4i(w + 1, h + 1, w, h + 1), imgSegment[index]));
				else
					edges.push_back(std::make_pair(Vector4i(w, h + 1, w + 1, h + 1), imgSegment[index + size1.x]));
			}


			if (currentGen && border)
			{
				if (w == 0)
					edges.push_back(std::make_pair(Vector4i(w, h + 1, w, h), imgSegment[index]));
				if (w == size1.x - 1)
					edges.push_back(std::make_pair(Vector4i(w + 1, h, w + 1, h + 1), imgSegment[index]));
				if (h == 0)
					edges.push_back(std::make_pair(Vector4i(w, h, w + 1, h), imgSegment[index]));
				if (h == size1.y - 1)
					edges.push_back(std::make_pair(Vector4i(w + 1, h + 1, w, h + 1), imgSegment[index]));
			}
		}
	}

	LordSafeFree(greedDown);
	LordSafeFree(greedUp);
	LordSafeFree(greedRight);
	LordSafeFree(greedLeft);
	LordSafeFree(imgSegment);

	// all the quad and edges to make ItemRenderable.
	pOut->beginPolygon();
	ItemVertexFmt* pVerts;
	// x:[-0.5f, 0.5f],  y:[0.f, 1.f], z:[ItemVertexFmt*
	float mz = 1.f / float(size1.x) * 0.5f;
	float mx = 1.f / float(size1.x);
	float my = 1.f / float(size1.y);

	// for all the quads.
	for (QuadArr::iterator it = quads.begin(); it != quads.end(); ++it)
	{
		const Vector4i& pb = it->first;
		ui32 color = it->second;
		Color c(color);
		c = c * 0.7f;
		color = c.getARGB();

		float x1 = pb.x * mx - 0.5f;
		float x2 = (pb.x + pb.z) * mx - 0.5f;
		float y1 = 1.f - pb.y * my;
		float y2 = 1.f - (pb.y + pb.w) * my;

		// add two quad face.
		if (!pOut->getLockPtr(2, pVerts))
			return false;

		pVerts[0].setEx3(Vector3(x1, y2, mz), color, float(pb.x) / size1.x + 0.f, float(pb.y) / size1.y + my);
		pVerts[1].setEx3(Vector3(x2, y2, mz), color, float(pb.x) / size1.x + mx, float(pb.y) / size1.y + my);
		pVerts[2].setEx3(Vector3(x2, y1, mz), color, float(pb.x) / size1.x + mx, float(pb.y) / size1.y + 0.f);
		pVerts[3].setEx3(Vector3(x1, y1, mz), color, float(pb.x) / size1.x + 0.f, float(pb.y) / size1.y + 0.f);

		pVerts[4].setEx3(Vector3(x1, y2, -mz), color, float(pb.x) / size1.x + 0.f, float(pb.y) / size1.y + my);
		pVerts[5].setEx3(Vector3(x1, y1, -mz), color, float(pb.x) / size1.x + 0.f, float(pb.y) / size1.y + 0.f);
		pVerts[6].setEx3(Vector3(x2, y1, -mz), color, float(pb.x) / size1.x + mx, float(pb.y) / size1.y + 0.f);
		pVerts[7].setEx3(Vector3(x2, y2, -mz), color, float(pb.x) / size1.x + mx, float(pb.y) / size1.y + my);

	}

	// for all the edges
	for (EdgeArr::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		const Vector4i& pb = it->first;
		LordAssert(pb.x == pb.z || pb.y == pb.w);

		ui32 color = it->second;
		Vector3 normal;
		if (pb.x == pb.z) // left or right
		{
			if (pb.y > pb.w)
				normal = Vector3::NEG_UNIT_X;
			else
				normal = Vector3::UNIT_X;
		}
		if (pb.y == pb.w) // up or down
		{
			if (pb.x > pb.z)
				normal = Vector3::NEG_UNIT_Y;
			else
				normal = Vector3::UNIT_Y;
		}

		float x1 = pb.x * mx - 0.5f;
		float x2 = pb.z * mx - 0.5f;
		float y1 = 1.0f - pb.y * mx;
		float y2 = 1.0f - pb.w * mx;
		// add one quad face.
		if (!pOut->getLockPtr(1, pVerts))
			return false;

		pVerts[0].setEx3(Vector3(x2, y2, mz), color, float(pb.z) / size1.x, float(pb.w) / size1.y);
		pVerts[1].setEx3(Vector3(x2, y2, -mz), color, float(pb.z) / size1.x, float(pb.w) / size1.y);
		pVerts[2].setEx3(Vector3(x1, y1, -mz), color, float(pb.x) / size1.x, float(pb.y) / size1.y);
		pVerts[3].setEx3(Vector3(x1, y1, mz), color, float(pb.x) / size1.x, float(pb.y) / size1.y);
	}

	pOut->endPolygon();
	return true;
}

}