#include "gui_renderItems.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIRenderManager.h"
#include "UI/GUICell.h"
#include "UI/GUICommonDef.h"

#include "Render/TextureAtlas.h"

namespace BLOCKMAN
{

gui_renderItems::gui_renderItems()
{
	TextureAtlasRegister* atlasRegsiter = TextureAtlasRegister::Instance();

	m_pImagesetBlock = GUIImagesetManager::Instance()->createImageset("Blocks.json", RESOURCE_TYPE_OUTSIDE);
	MemTextureAtlas* atlas_block = dynamic_cast<MemTextureAtlas*>(atlasRegsiter->getTextureAtlas("Blocks.json"));
	int mip = 1;
	GUIImageset::ImageSetUnitArr blockimages;
	if (atlas_block)
	{
		atlas_block->getMipAtlasSpriteArr(mip, blockimages);
		m_pImagesetBlock->loadOutSide(atlas_block->getMipTexture(mip), blockimages);
	}

	m_pImagesetItem = GUIImagesetManager::Instance()->createImageset("items.json", RESOURCE_TYPE_OUTSIDE);
	TextureAtlas* atlas_item = atlasRegsiter->getTextureAtlas("items.json");
	GUIImageset::ImageSetUnitArr itemimages;
	if (atlas_item)
	{
		atlas_item->getAtlasSpriteArr(itemimages);
		m_pImagesetItem->loadOutSide(atlas_item->getTexture(), itemimages);
	}

	m_pImagesetChest_normal = GUIImagesetManager::Instance()->createImageset("normal_ui.json");
	m_pImagesetChest_normal->load();

	m_pImagesetChest_trapped = GUIImagesetManager::Instance()->createImageset("trapped_ui.json");
	m_pImagesetChest_trapped->load();

	m_pImagesetChest_christmas = GUIImagesetManager::Instance()->createImageset("christmas_ui.json");
	m_pImagesetChest_christmas->load();

	m_pImagesetChest_ender = GUIImagesetManager::Instance()->createImageset("ender_ui.json");
	m_pImagesetChest_ender->load();
}

gui_renderItems::~gui_renderItems()
{
//    LordSafeDelete(m_pImagesetBlock);
//    LordSafeDelete(m_pImagesetItem);
//    LordSafeDelete(m_pImagesetChest_normal);
//    LordSafeDelete(m_pImagesetChest_trapped);
//    LordSafeDelete(m_pImagesetChest_christmas);
//    LordSafeDelete(m_pImagesetChest_ender);
}


void gui_renderItems::renderItemInWindow(GUIWindow *pWindow, int itemID, int damage)
{
	cItem *pcItem = ItemClient::citemsList[itemID];
	cBlock *pcBlock = NULL;
	int blockId = -1;

	if (auto itemBlock = dynamic_cast<cItemBlock*>(pcItem))
	{
		blockId = itemBlock->getBlockId();
		pcBlock = cBlockManager::scBlocks[blockId];
	}

	//render as block
	if (pcItem && pcItem->getRenderType() == ItemRenderType::BLOCK && pcBlock && RenderBlocks::renderItemIn3d(pcBlock->getRenderType()))
	{
		renderBlockAsItem(pWindow, blockId, damage);
	}
	//render as item
	else
	{
		renderItem(pWindow, itemID, damage);
	}


	GUICell *pCell = dynamic_cast<GUICell*>(pWindow);
	LordAssert(pCell);
	pCell->SetVertexData(m_actionVertex);
}


void gui_renderItems::renderItem(GUIWindow *pWindow, int itemID, int damage)
{
	m_actionVertex.clear();

	Item *pItem = Item::itemsList[itemID];
	cItem *pcItem = ItemClient::citemsList[itemID];

	if (!pItem)
	{
		return;
	}

	const ERect& rtArea = pWindow->GetRenderArea();

	//vertex
	float left = 0.0f;
	float right = rtArea.getWidth();
	float top = 0.0f;
	float bottom = rtArea.getHeight();

	Vector3 t[4];
	t[0] = Vector3(left, top, 0.0f);
	t[1] = Vector3(left, bottom, 0.0f);
	t[2] = Vector3(right, bottom, 0.0f);
	t[3] = Vector3(right, top, 0.0f);

	//color
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, 1, damage);
	Color color(pcItem->getColorFromItemStack(pStack, 0));
	color.a = 255;
	pStack = nullptr;

	//uv
	bool isBlock = pcItem->getRenderType() == ItemRenderType::BLOCK;
	AtlasSprite *atlas = pcItem->getIconFromDamage(damage);
	if (atlas == NULL)
	{
		return;
	}

	String imageName = atlas->getSpriteName();
	GUIImage *pImage = m_pImagesetItem->GetImage(imageName.c_str());
	if (isBlock)
	{
		pImage = m_pImagesetBlock->GetImage(imageName.c_str());
	}

	if (pImage == NULL)
	{
		return;
	}

	ERect uvRect = pImage->GetUV();

	if (m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[1];
		temp.rt = t[3];
		temp.uv = uvRect;
		temp.color = ColorRect(color);
		temp.imageset = m_pImagesetItem;
		if (isBlock)
		{
			temp.imageset = m_pImagesetBlock;
		}
		m_actionVertex.push_back(temp);
	}
}

void gui_renderItems::renderBlockAsItem(GUIWindow *pWindow, int blockID, int metadata)
{
	m_actionVertex.clear();
	if (blockID == BLOCK_ID_STAIRS_COBBLE_STONE ||
		blockID == BLOCK_ID_STAIRS_NETHER_BRICK ||
		blockID == BLOCK_ID_STAIRS_SAND_STONE	||
		blockID == BLOCK_ID_STAIRS_STONE_BRICK	||
		blockID == BLOCK_ID_STAIRS_WOOD_BIRCH	||
		blockID == BLOCK_ID_STAIRS_WOOD_JUNGLE	||
		blockID == BLOCK_ID_STAIRS_WOOD_OAK		||
		blockID == BLOCK_ID_STAIRS_WOOD_SPURCE	||
		blockID == BLOCK_ID_STAIRS_BRICK		|| 
		blockID == BLOCK_ID_STAIRS_NETHER_QUARTZ||
		blockID == BLOCK_ID_STAIRS_WOOD_DARK_OAK)

	{
		renderBlockStairs(pWindow, blockID, metadata);
	}
	else if (blockID == BLOCK_ID_CHEST ||
		blockID == BLOCK_ID_CHEST_TRAPPED ||
		blockID == BLOCK_ID_ENDER_CHEST ||
		blockID == BLOCK_ID_CHEST_NEW ||
		blockID == BLOCK_ID_ENDER_CHEST_NEW ||
		blockID == BLOCK_ID_CHEST_TRAPPED_NEW)
	{
		renderBlockChest(pWindow, blockID, metadata);
	}
	else
	{
		renderStandardBlock(pWindow, blockID, metadata);
	}

	GUIStaticImage *pImage = dynamic_cast<GUIStaticImage*>(pWindow);
	pImage->SetVertexData(m_actionVertex);
}

void gui_renderItems::renderBlockChest(GUIWindow* pWindow, int blockID, int metadata)
{
	GUIImageset* chestImageset = NULL;
	if (blockID == BLOCK_ID_CHEST || blockID == BLOCK_ID_CHEST_NEW)
		chestImageset = m_pImagesetChest_normal;
	else if (blockID == BLOCK_ID_CHEST_TRAPPED || blockID == BLOCK_ID_CHEST_TRAPPED_NEW)
		chestImageset = m_pImagesetChest_trapped;
	else if (blockID == BLOCK_ID_ENDER_CHEST || blockID == BLOCK_ID_ENDER_CHEST_NEW)
		chestImageset = m_pImagesetChest_ender;
	else
		return;

	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];
	pBlock->setBlockBoundsForItemRender();

	float width = pBlock->getBlockBoundsMaxX();
	float height = pBlock->getBlockBoundsMaxY();
	float depth = pBlock->getBlockBoundsMaxZ();

	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();


	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.f;
	float y2 = rtDest.top + w / 2.f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;
	float m1 = -f1 + f1 * 2.f / 3.f;
	float m2 = -f3 + (f2 + f3) / 3.f;

	float offsetWidth = 0.0f;
	float offsetHeight = (1.0f - height) * (f2);
	float offsetDepth = 0.0f;
	Vector3 t[10];
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0); //
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f);
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);
	t[7] = Vector3(f0 + x2, m1 + y2, 0.f);
	t[8] = Vector3(0.f + x2, m2 + y2, 0.f);
	t[9] = Vector3(-f0 + x2, m1 + y2, 0.f);

	/**
	1
	0       2
	9   3   7
	6   8   5
	4
	*/

	/*bright*/
	Color brightLeft(0.5f, 0.5f, 0.5f, 1.f);
	Color brightRight(0.7f, 0.7f, 0.7f, 1.f);

	Color topColor = Color::WHITE;
	Color sideColor = Color::WHITE;

	// top
	GUIImage* pTopImage = chestImageset->GetImage("lid_top");
	if (!pTopImage)	return;
	//side 1
	GUIImage* pLidSide1 = chestImageset->GetImage("lid_side");
	if (!pLidSide1) return;
	GUIImage* pLidSide2 = chestImageset->GetImage("lid_side2");
	if (!pLidSide2) return;
	//side 2
	GUIImage* pBelowSide1 = chestImageset->GetImage("below");
	if (!pBelowSide1) return;
	GUIImage* pBelowSide2 = chestImageset->GetImage("below2");
	if (!pBelowSide2) return;

	if (m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = pTopImage->GetUV();
		temp.color = ColorRect(topColor);
		temp.imageset = chestImageset;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[7];
		temp.lb = t[8];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = pLidSide1->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[0];
		temp.rb = t[8];
		temp.lb = t[9];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = pLidSide2->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[8];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[7];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = pBelowSide1->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[9];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[8];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = pBelowSide2->GetUV();
		m_actionVertex.push_back(temp);
	}
}

void gui_renderItems::renderStandardBlock(GUIWindow *pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}
		

	pBlock->setBlockBoundsForItemRender();

	float width = pBlock->getBlockBoundsMaxX();
	float height = pBlock->getBlockBoundsMaxY();
	float depth = pBlock->getBlockBoundsMaxZ();

	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getHeight();
	rtDest.bottom = rtArea.getWidth();;
	rtDest = rtDest.makeSquare();


	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.f;
	float y2 = rtDest.top + w / 2.f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;

	float offsetWidth = 0.0f;
	float offsetHeight = (1.0f - height) * (f2);
	float offsetDepth = 0.0f;
	Vector3 t[7];
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0);   // left top corner
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);   // center top corner
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);  // right top corner
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f); // center point
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);                 // center bottom corner
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);                  // right bottom corner
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);                 // left bottom corner

	/*bright*/
	Color brightLeft(0.5f, 0.5f, 0.5f, 1.f);
	Color brightRight(0.7f, 0.7f, 0.7f, 1.f);

	/*color*/
	Color topColor = Color::WHITE;
	if (blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_LEAVES)
	{
		topColor = pcBlock->getBlockColor();
		topColor.a = 1.f;
	}

	Color sideColor = Color::WHITE;
	if (blockID == BLOCK_ID_LEAVES)
	{
		sideColor = pcBlock->getBlockColor();
		topColor.a = 1.f;
	}

	// top
	String strTopImageName;
	if (blockID == BLOCK_ID_PISTON_BASE || blockID == BLOCK_ID_PISTON_STICKY_BASE)
		strTopImageName = pcBlock->getIcon(BM_FACE_UP, 6)->getSpriteName();
	else
		strTopImageName = pcBlock->getIcon(BM_FACE_UP, metadata)->getSpriteName();
	auto imageset = GUIImagesetManager::Instance()->createOrRetrieveImageset(pcBlock->getAtlasName().c_str());
	if (!imageset->isLoaded())
	{
		imageset->load();
	}
	GUIImage *pTopImage = imageset->GetImage(strTopImageName.c_str());
	if (pTopImage == NULL)
	{
		return;
	}

	ERect rtTexUV_top = pTopImage->GetUV();
	//side 1
	String strSide1ImageName = pcBlock->getIcon(BM_FACE_SOUTH, metadata)->getSpriteName();
	GUIImage *pSide1Image = imageset->GetImage(strSide1ImageName.c_str());
	ERect rtTexUV_side1 = pSide1Image->GetUV();
	rtTexUV_side1.top += (1.0f - height) * 0.03125f;
	//side 2
	String strSide2IamgeName = pcBlock->getIcon(BM_FACE_WEST, metadata)->getSpriteName();
	GUIImage *pSide2Iamge = imageset->GetImage(strSide2IamgeName.c_str());
	ERect rtTexUV_side2 = pSide2Iamge->GetUV();
	rtTexUV_side2.top += (1.0f - height) * 0.03125f;
	
	if(m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);
		temp.imageset = imageset;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);
		//const ImageBuffer& temp1 = vertex.back();

		temp.lt = t[0];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);
		//const ImageBuffer& temp2 = vertex.back();
	}
}


void gui_renderItems::renderBlockStairs(GUIWindow *pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}


	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	//rtDest.top = PixelAligned(rtArea.top);
	//rtDest.left = PixelAligned(rtArea.left);
	//rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth()); // *scale
	//rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight()); // *scale
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();

	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.0f;
	float y2 = rtDest.top + w / 2.0f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;

	float offsetHeight = 0.5f * f2;
	float offsetX = 0.5f * f0;
	float offsetY = 0.5f * f1;
	
	Vector3 t[14];
	//下半部分
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0);	//left
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);	//up
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);	//right
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f);	//down
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);

	//上半部分
	t[7] = t[0];
	t[8] = (t[1] + t[2]) / 2.0f;
	t[9] = (t[0] + t[3]) / 2.0f;
	t[10] = t[7] - Vector3(0, offsetHeight, 0);			//left
	t[11] = t[1] - Vector3(0, offsetHeight, 0);			//up
	t[12] = t[8] - Vector3(0, offsetHeight, 0);			//right
	t[13] = t[9] - Vector3(0, offsetHeight, 0);			//down

	/*bright*/
	float brightLeft = 0.5f;
	float brightRight = 0.7f;

	/*color*/
	Color topColor = Color::WHITE;	
	topColor.a = 255;
	Color sideColor = Color::WHITE;	
	sideColor.a = 255;

	/*uv*/
	// top
	String strTopImageName = pcBlock->getIcon(1, metadata)->getSpriteName();
	GUIImage *pTopImage = m_pImagesetBlock->GetImage(strTopImageName.c_str());
	ERect rtTexUV_top = pTopImage->GetUV();
	rtTexUV_top.top += 0.5f * 0.03125f;
	//side 1
	String strSide1ImageName = pcBlock->getIcon(3, metadata)->getSpriteName();
	GUIImage *pSide1Image = m_pImagesetBlock->GetImage(strSide1ImageName.c_str());
	ERect rtTexUV_side1 = pSide1Image->GetUV();	
	rtTexUV_side1.top += 0.5f * 0.03125f;
	//side 2
	String strSide2IamgeName = pcBlock->getIcon(5, metadata)->getSpriteName();
	GUIImage *pSide2Iamge = m_pImagesetBlock->GetImage(strSide2IamgeName.c_str());
	ERect rtTexUV_side2 = pSide2Iamge->GetUV();	
	rtTexUV_side2.top += 0.5f * 0.03125f;
	
	if (m_actionVertex.empty())
	{
		//stairs nether
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);
		temp.imageset = m_pImagesetBlock;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);		

		temp.lt = t[0];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);		

		//stairs upper
		
		// 0.0625	16/256		
		rtTexUV_top.right -= 0.5f * 0.0625f;		
		rtTexUV_side1.right -= 0.5f * 0.0625f;		
		rtTexUV_side2.right -= 0.5f * 0.0625f;

		temp.lt = t[10];
		temp.rb = t[12];
		temp.lb = t[13];
		temp.rt = t[11];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);		
		m_actionVertex.push_back(temp);

		temp.lt = t[13];
		temp.rb = t[8];
		temp.lb = t[9];
		temp.rt = t[12];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);

		temp.lt = t[10];
		temp.rb = t[9];
		temp.lb = t[7];
		temp.rt = t[13];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);
	}
		
}


}
