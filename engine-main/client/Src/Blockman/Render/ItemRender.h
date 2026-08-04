/********************************************************************
filename: 	ItemRender.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-3-17
*********************************************************************/
#ifndef __ITEM_RENDER_HEADER__
#define __ITEM_RENDER_HEADER__

#include "Core.h"
#include "Item/ItemStack.h"
using namespace LORD;

namespace BLOCKMAN
{

class Blockman;
class ItemStack;
class RenderBlocks;
class EntityLivingBase;
class EntityRenderManager;
class AtlasSprite;
class RightArmRenderable;
class PackMassRenderable;
class ModelEntity;

class ItemRenderer : public ObjectAlloc
{
public:
	static Texture* s_texEnchantment;	// = new ResourceLocation("textures/misc/enchanted_item_glint.png");
	//static Texture* field_110931_c;		// = new ResourceLocation("textures/map/map_background.png");
	//static Texture* field_110929_d;		// = new ResourceLocation("textures/misc/underwater.png");
	static Color	s_colorEnch_item;
	static Color	s_colorEnch_block;
	static Color	s_colorEnch_equip;

protected:
	/** A reference to the Blockman object. */
	Blockman* bm = nullptr;
	ItemStackPtr itemToRender = nullptr;
	/** How far the current item has been equipped (0 disequipped and 1 fully up) */
	float equippedProgress = 0.f;
	float prevEquippedProgress = 0.f;
	/** Instance of RenderBlocks. */
	RenderBlocks* renderBlocksInstance = nullptr;
	// public final MapItemRenderer mapItemRenderer;
	/** The index of the currently held item (0-8, or -1 if not yet updated) */
	int equippedItemSlot = 0;
	EntityRenderManager* renderManager = nullptr;

	// for right arm renders.
	RightArmRenderable*		m_rightArmRenderable = nullptr;
	PackMassRenderable*		m_rightBlockRenderable = nullptr;
	PackMassRenderable*		m_rightBlockEnchantRenderable = nullptr;
	PackMassRenderable*		m_fireEffectsRenderable = nullptr;
	ItemRenderable*			m_rightItemRenderable = nullptr;
	ItemRenderable*			m_rightItemEnchantRenderable = nullptr;
	LORD::ModelEntity*		m_rightItemMesh = nullptr;
	int						m_lastItemMeshID = 0;

	int m_color = 0;
	Vector2 m_lightUV;

protected:
	/** Renders the texture of the block the player is inside as an overlay. Args: partialTickTime, blockTextureIndex */
	void renderInsideOfBlock(float rdt, AtlasSprite* sprite);
	/** Renders a texture that warps around based on the direction the player is looking. Texture needs to be bound
	* before being called. Used for the water overlay.*/
	void renderWarpedTextureOverlay(float rdt);
	/** Renders the fire on the screen for first person mode.*/
	void renderFireInFirstPerson(float rdt);
	/** test the right block renderable for first person mode.*/
	void checkRightBlockRenderable(int blockID, int meta);
	void checkRightBlockEnchantRenderable(int blockID, int meta);
	/** test the right item renderable for first person mode.*/
	void checkRgihtItemRenderable(ItemStackPtr pStack);
	void checkRightItemEnchantRenderable(ItemStackPtr pStack);
	void checkRightItemMesh(ItemStackPtr pStack);
	void destroyRightItemMesh();

public:
	static void initialize();
	static void unInitialize();

	ItemRenderer(Blockman* par1Blockman);
	~ItemRenderer();
	
	/** Renders the item stack for being in an entity's hand Args: itemStack */
	void renderItem(EntityLivingBase* pLiving, ItemStackPtr pStack, int par3);
	/** Renders an item held in hand as a 2D texture with thickness */
	static void renderItemIn2D(float tu_min, float tu_max, float tv_min, float tv_max, int sizeU, int sizeV, float anti_gap);
	/** Renders the active item in the player's hand when in first person mode. Args: partialTickTime */
	void renderItemInFirstPerson(float rdt);
	/** Renders all the overlays that are in first person mode. Args: rdt */
	void renderOverlays(float rdt);
	void updateEquippedItem();
	/** Resets equippedProgress */
	void resetEquippedProgress() { equippedProgress = 0.0F; }
	/** Resets equippedProgress */
	void resetEquippedProgress2() { equippedProgress = 0.0F; }
};

}

#endif