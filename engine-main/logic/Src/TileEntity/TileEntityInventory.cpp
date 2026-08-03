#include "TileEntityInventory.h"

#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Item/Recipes.h"
#include "Item/Potion.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "Inventory/Container.h"
#include "Inventory/InventoryLargeChest.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityItem.h"
#include "World/World.h"
#include "Entity/Enchantment.h"
#include "Setting/LogicSetting.h"

namespace BLOCKMAN
{

int TileEntityFurnace::slots_top[1] = { 0 };
int TileEntityFurnace::slots_bottom[2] = { 2, 1 };
int TileEntityFurnace::slots_sides[1] = { 1 };

TileEntityFurnace::TileEntityFurnace()
	: TileEntity(TILE_ENTITY_FURNACE)
	, furnaceBurnTime(0)
	, currentItemBurnTime(0)
	, furnaceCookTime(0)
{
	memset(furnaceItemStacks, 0, sizeof(ItemStackPtr) * FURNACE_INVSIZE);
}

TileEntityFurnace::~TileEntityFurnace()
{}

ItemStackPtr TileEntityFurnace::decrStackSize(int slot, int size)
{
	LordAssert(slot > 0 && slot < FURNACE_INVSIZE);
	if (!furnaceItemStacks[slot])
		return NULL;

	ItemStackPtr pStack = NULL;

	if (furnaceItemStacks[slot]->stackSize <= size)
	{
		pStack = furnaceItemStacks[slot];
		furnaceItemStacks[slot] = NULL;
		return pStack;
	}

	pStack = furnaceItemStacks[slot]->splitStack(size);

	if (furnaceItemStacks[slot]->stackSize == 0)
		furnaceItemStacks[slot] = NULL;

	return pStack;
}

ItemStackPtr TileEntityFurnace::getStackInSlotOnClosing(int slot)
{
	LordAssert(slot > 0 && slot < FURNACE_INVSIZE);
	if (!furnaceItemStacks[slot])
		return NULL;

	ItemStackPtr pStack = furnaceItemStacks[slot];
	furnaceItemStacks[slot] = NULL;
	return pStack;
}

void TileEntityFurnace::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	LordAssert(slot > 0 && slot < FURNACE_INVSIZE);
	furnaceItemStacks[slot] = pStack;

	if (pStack != NULL && pStack->stackSize > getInventoryStackLimit())
	{
		pStack->stackSize = getInventoryStackLimit();
	}
}

void TileEntityFurnace::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	NBTTagList* pNbtItems = pNbtCompound->getTagList("Items");

	for (int i = 0; i < pNbtItems->tagCount(); ++i)
	{
		NBTTagCompound* pNbtItem = (NBTTagCompound*)pNbtItems->tagAt(i);
		i8 slot = pNbtItem->getByte("Slot");

		if (slot >= 0 && slot < FURNACE_INVSIZE)
		{
			furnaceItemStacks[slot] = ItemStack::loadItemStackFromNBT(pNbtItem);
		}
	}

	furnaceBurnTime = pNbtCompound->getShort("BurnTime");
	furnaceCookTime = pNbtCompound->getShort("CookTime");
	currentItemBurnTime = getItemBurnTime(furnaceItemStacks[1]);

	if (pNbtCompound->hasKey("CustomName"))
	{
		customTitleName = pNbtCompound->getString("CustomName");
	}
}

void TileEntityFurnace::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	pNbtCompound->setShort("BurnTime", (i16)furnaceBurnTime);
	pNbtCompound->setShort("CookTime", (i16)furnaceCookTime);
	NBTTagList* pNbtLst = LordNew NBTTagList();

	for (int i = 0; i < FURNACE_INVSIZE; ++i)
	{
		if (!furnaceItemStacks[i])
			continue;

		NBTTagCompound* pNbtItem = LordNew NBTTagCompound();
		pNbtItem->setByte("Slot", (i8)i);
		furnaceItemStacks[i]->writeToNBT(pNbtItem);
		pNbtLst->appendTag(pNbtItem);
	}

	pNbtCompound->setTag("Items", pNbtLst);

	if (isInvNameLocalized())
	{
		pNbtCompound->setString("CustomName", customTitleName);
	}
}

int TileEntityFurnace::getBurnTimeRemainingScaled(int cooktime)
{
	if (currentItemBurnTime == 0)
	{
		currentItemBurnTime = 200;
	}

	return furnaceBurnTime * cooktime / currentItemBurnTime;
}

void TileEntityFurnace::updateEntity()
{
	bool burning = furnaceBurnTime > 0;
	bool var2 = false;

	if (furnaceBurnTime > 0)
	{
		--furnaceBurnTime;
	}

	if (!worldObj->m_isClient)
	{
		if (furnaceBurnTime == 0 && canSmelt())
		{
			currentItemBurnTime = furnaceBurnTime = getItemBurnTime(furnaceItemStacks[1]);

			if (furnaceBurnTime > 0)
			{
				var2 = true;

				if (furnaceItemStacks[1])
				{
					--furnaceItemStacks[1]->stackSize;

					if (furnaceItemStacks[1]->stackSize == 0)
					{
						Item* pItem = furnaceItemStacks[1]->getItem()->getContainerItem();
						furnaceItemStacks[1] = pItem != NULL ? LORD::make_shared<ItemStack>(pItem) : NULL;
					}
				}
			}
		}

		if (isBurning() && canSmelt())
		{
			++furnaceCookTime;

			if (furnaceCookTime == 200)
			{
				furnaceCookTime = 0;
				smeltItem();
				var2 = true;
			}
		}
		else
		{
			furnaceCookTime = 0;
		}

		if (burning != furnaceBurnTime > 0)
		{
			var2 = true;
			// BlockFurnace.updateFurnaceBlockState(furnaceBurnTime > 0, worldObj, xCoord, yCoord, zCoord);
		}
	}

	if (var2)
	{
		TileEntity::onInventoryChanged();
	}
}

bool TileEntityFurnace::canSmelt()
{
	if (furnaceItemStacks[0] == NULL)
	{
		return false;
	}

	ItemStackPtr pStack = FurnaceRecipes::smelting()->getSmeltingResult(furnaceItemStacks[0]->getItem()->itemID);

	if (!pStack)
		return false;
	if (!furnaceItemStacks[2])
		return true;
	if (!furnaceItemStacks[2]->isItemEqual(pStack))
		return false;
	if (furnaceItemStacks[2]->stackSize < getInventoryStackLimit() &&
		furnaceItemStacks[2]->stackSize < furnaceItemStacks[2]->getMaxStackSize())
		return true;
	return furnaceItemStacks[2]->stackSize < pStack->getMaxStackSize();
}

void TileEntityFurnace::smeltItem()
{
	if (canSmelt())
	{
		ItemStackPtr pStack = FurnaceRecipes::smelting()->getSmeltingResult(furnaceItemStacks[0]->getItem()->itemID);

		if (furnaceItemStacks[2] == NULL)
		{
			furnaceItemStacks[2] = pStack->copy();
		}
		else if (furnaceItemStacks[2]->itemID == pStack->itemID)
		{
			++furnaceItemStacks[2]->stackSize;
		}

		--furnaceItemStacks[0]->stackSize;

		if (furnaceItemStacks[0]->stackSize <= 0)
		{
			furnaceItemStacks[0] = NULL;
		}
	}
}

int TileEntityFurnace::getItemBurnTime(ItemStackPtr pStack)
{
	if (pStack == NULL)
		return 0;

	int itemid = pStack->getItem()->itemID;
	Item* pItem = pStack->getItem();

	if (Item::isBlockItem(itemid) && BlockManager::sBlocks[itemid] != NULL)
	{
		Block* pBlock = BlockManager::sBlocks[itemid];

		if (itemid == BLOCK_ID_WOOD_SINGLE_SLAB)
		{
			return 150;
		}

		if (pBlock->getMaterial() == BM_Material::BM_MAT_wood)
		{
			return 300;
		}

		if (itemid == BLOCK_ID_BLOCK_COAL)
		{
			return 16000;
		}
	}

	ItemTool* pTool = dynamic_cast<ItemTool*>(pItem);
	if (pTool && pTool->getToolMaterialName() == "WOOD")
		return 200;
	ItemSword* pSword = dynamic_cast<ItemSword*>(pItem);
	if (pSword && pSword->getToolMaterialName() == "WOOD")
		return 200;
	ItemHoe* pHoe = dynamic_cast<ItemHoe*>(pItem);
	if (pHoe && pHoe->getMaterialName() == "WOOD")
		return 200;
	if (itemid == Item::stick->itemID)
		return 100;
	if (itemid == Item::coal->itemID)
		return 1600;
	if (itemid == Item::bucketLava->itemID)
		return 20000;
	if (itemid == BLOCK_ID_SAPLING)
		return 100;
	if (itemid == Item::blazeRod->itemID)
		return 2400;
	return 0;
}

bool TileEntityFurnace::isUseableByPlayer(EntityPlayer* pPlayer)
{
	if (worldObj->getBlockTileEntity(m_pos) != this)
		return false;

	return pPlayer->getDistanceSq(Vector3(m_pos) + 0.5f) <= 64.f;
}

void TileEntityFurnace::onInventoryChanged()
{
	return TileEntity::onInventoryChanged();
}

bool TileEntityFurnace::isItemValidForSlot(int slot, ItemStackPtr pStack)
{
	return slot == 2 ? false : (slot == 1 ? isItemFuel(pStack) : true);
}

int* TileEntityFurnace::getAccessibleSlotsFromSide(int side, int& outsize)
{
	if (side == 0)
	{
		outsize = 2;
		return slots_bottom;
	}
	else if (side == 1)
	{
		outsize = 1;
		return slots_top;
	}
	else
	{
		outsize = 1;
		return slots_sides;
	}
}

bool TileEntityFurnace::canExtractItem(int slot, ItemStackPtr pStack, int side)
{
	return side != 0 || slot != 1 || pStack->itemID == Item::bucketEmpty->itemID;
}

TileEntityChest::TileEntityChest(int chestType)
	: TileEntity(TILE_ENTITY_CHEST)
	, ticksSinceSync(0)
	, cachedChestType(chestType)
	, adjacentChestChecked(false)
	, adjacentChestZNeg(NULL)
	, adjacentChestXPos(NULL)
	, adjacentChestXNeg(NULL)
	, adjacentChestZPos(NULL)
	, lidAngle(0.f)
	, prevLidAngle(0.f)
	, numUsingPlayers(0)
{
	memset(chestContents, 0, sizeof(ItemStackPtr)* CHEST_CAPACITY);
}

TileEntityChest::~TileEntityChest()
{
	for (int i = 0; i < CHEST_CAPACITY; ++i)
	{
		chestContents[i] = nullptr;
	}
}

ItemStackPtr TileEntityChest::decrStackSize(int slot, int size)
{
	if (!(slot >= 0 && slot < CHEST_INVSIZE))
		return NULL;
	if (!chestContents[slot])
		return NULL;

	ItemStackPtr pStack = NULL;

	if (chestContents[slot]->stackSize <= size)
	{
		pStack = chestContents[slot];
		chestContents[slot] = NULL;
		TileEntity::onInventoryChanged();
		return pStack;
	}

	pStack = chestContents[slot]->splitStack(size);

	if (chestContents[slot]->stackSize == 0)
		chestContents[slot] = NULL;

	TileEntity::onInventoryChanged();
	return pStack;
}

ItemStackPtr TileEntityChest::getStackInSlotOnClosing(int slot)
{
	if (!(slot >= 0 && slot < CHEST_INVSIZE))
		return NULL;
	if (!chestContents[slot])
		return NULL;

	ItemStackPtr pStack = chestContents[slot];
	chestContents[slot] = NULL;
	return pStack;
}

void TileEntityChest::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (!(slot >= 0 && slot < CHEST_INVSIZE))
		return;

	chestContents[slot] = pStack;

	if (pStack != NULL && pStack->stackSize > getInventoryStackLimit())
	{
		pStack->stackSize = getInventoryStackLimit();
	}

	TileEntity::onInventoryChanged();
}

void TileEntityChest::setPrevInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (!(slot >= 0 && slot < CHEST_INVSIZE))
		return;

	prevChestContents[slot] = pStack;
}

void TileEntityChest::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	NBTTagList* pNbtItems = pNbtCompound->getTagList("Items");

	if (pNbtCompound->hasKey("CustomName"))
	{
		customTitleName = pNbtCompound->getString("CustomName");
	}

	for (int i = 0; i < pNbtItems->tagCount(); ++i)
	{
		NBTTagCompound* pNbtItem = (NBTTagCompound*)pNbtItems->tagAt(i);
		int slot = pNbtItem->getByte("Slot") & 255;

		if (slot >= 0 && slot < CHEST_INVSIZE)
		{
			chestContents[slot] = ItemStack::loadItemStackFromNBT(pNbtItem);
		}
	}
}

void TileEntityChest::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	NBTTagList* pNbtItems = LordNew NBTTagList();

	for (int i = 0; i < CHEST_INVSIZE; ++i)
	{
		if (!chestContents[i])
			continue;

		NBTTagCompound* pNbtItem = LordNew NBTTagCompound();
		pNbtItem->setByte("Slot", (i8)i);
		chestContents[i]->writeToNBT(pNbtItem);
		pNbtItems->appendTag(pNbtItem);
	}

	pNbtCompound->setTag("Items", pNbtItems);

	if (isInvNameLocalized())
	{
		pNbtCompound->setString("CustomName", customTitleName);
	}
}

bool TileEntityChest::isUseableByPlayer(EntityPlayer* pPlayer)
{
	if (worldObj->getBlockTileEntity(m_pos) != this)
		return false;
	return pPlayer->getDistanceSq(Vector3(m_pos) + 0.5f) <= 64.f;
}

void TileEntityChest::updateContainingBlockInfo()
{
	TileEntity::updateContainingBlockInfo();
	adjacentChestChecked = false;
}

void TileEntityChest::setNeighbor(TileEntityChest* pChest, int side)
{
	if (pChest->isInvalid())
	{
		adjacentChestChecked = false;
		return;
	}

	if (!adjacentChestChecked)
		return;

	switch (side)
	{
	case 0:
		if (adjacentChestZPos != pChest)
			adjacentChestChecked = false;
		break;

	case 1:
		if (adjacentChestXNeg != pChest)
			adjacentChestChecked = false;
		break;

	case 2:
		if (adjacentChestZNeg != pChest)
			adjacentChestChecked = false;
		break;

	case 3:
		if (adjacentChestXPos != pChest)
			adjacentChestChecked = false;
		break;
	}
}

void TileEntityChest::checkForAdjacentChests()
{
	return;

	if (!adjacentChestChecked)
	{
		adjacentChestChecked = true;
		adjacentChestZNeg = NULL;
		adjacentChestXPos = NULL;
		adjacentChestXNeg = NULL;
		adjacentChestZPos = NULL;

		BlockPos west = m_pos; --west.x;
		if (isChestAt(west))
			adjacentChestXNeg = dynamic_cast<TileEntityChest*>(worldObj->getBlockTileEntity(west));

		BlockPos east = m_pos; ++east.x;
		if (isChestAt(east))
			adjacentChestXPos = dynamic_cast<TileEntityChest*>(worldObj->getBlockTileEntity(east));

		BlockPos sounth = m_pos; --sounth.z;
		if (isChestAt(sounth))
			adjacentChestZNeg = dynamic_cast<TileEntityChest*>(worldObj->getBlockTileEntity(sounth));

		BlockPos north = m_pos; ++north.z;
		if (isChestAt(north))
			adjacentChestZPos = dynamic_cast<TileEntityChest*>(worldObj->getBlockTileEntity(north));

		if (adjacentChestZNeg)
			adjacentChestZNeg->setNeighbor(this, 0);

		if (adjacentChestZPos)
			adjacentChestZPos->setNeighbor(this, 2);

		if (adjacentChestXPos)
			adjacentChestXPos->setNeighbor(this, 1);

		if (adjacentChestXNeg)
			adjacentChestXNeg->setNeighbor(this, 3);
	}
}

bool TileEntityChest::isChestAt(const BlockPos& pos)
{
	int blockID = worldObj->getBlockId(pos);
	Block* pBlock = BlockManager::sBlocks[blockID];
	if (!pBlock)
		return false;

	BlockChest* pblockchest = dynamic_cast<BlockChest*>(pBlock);
	if (!pblockchest)
		return false;
	//return pblockchest->isTrapped == getChestType();
	return pblockchest->getChestType() == getChestType();
}

void TileEntityChest::updateEntity()
{
	TileEntity::updateEntity();

	checkForAdjacentChests();
	++ticksSinceSync;

	if (!worldObj->m_isClient && numUsingPlayers != 0 && (ticksSinceSync + m_pos.x + m_pos.y + m_pos.z) % 200 == 0)
	{
		numUsingPlayers = 0;
		Vector3 extend(0.5f, 0.5f, 0.5f);
		EntityArr entities = worldObj->getEntitiesWithinAABB(ENTITY_CLASS_PLAYER, Box(Vector3(m_pos) - extend, Vector3(m_pos) + extend + Vector3::ONE));

		for (EntityArr::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(*it);
			if (!pPlayer)
				continue;

			ContainerChest* pContainer = dynamic_cast<ContainerChest*>(pPlayer->openContainer);
			if (pContainer)
			{
				IInventory* pInvenotry = pContainer->getLowerChestInventory();
				InventoryLargeChest* pInventoryLargeChest = dynamic_cast<InventoryLargeChest*>(pInvenotry);

				if (pInvenotry == this || pInventoryLargeChest && pInventoryLargeChest->isPartOfLargeChest(this))
				{
					++numUsingPlayers;
				}
			}
		}
	}

	prevLidAngle = lidAngle;


	if (numUsingPlayers > 0 && lidAngle == 0.0F && !adjacentChestZNeg && !adjacentChestXNeg)
	{
		float fx = m_pos.x + 0.5f;
		float fz = m_pos.z + 0.5f;

		if (adjacentChestZPos)
			fz += 0.5f;

		if (adjacentChestXPos)
			fx += 0.5f;

		//worldObj->playSoundEffect({ fx, m_pos.y + 0.5f, fz }, "random.chestopen", 0.5F, worldObj->m_Rand.nextFloat() * 0.1F + 0.9F);
		worldObj->playSoundEffectByType({ fx, m_pos.y + 0.5f, fz }, ST_OpenChest);
	}

	if (numUsingPlayers == 0 && lidAngle > 0.0F ||
		numUsingPlayers > 0 && lidAngle < 1.0F)
	{
		float preAngle = lidAngle;

		if (numUsingPlayers > 0)
		{
			lidAngle += 0.1f;
		}
		else
		{
			lidAngle -= 0.1f;
		}

		if (lidAngle > 1.0F)
		{
			lidAngle = 1.0F;
		}

		float var10 = 0.5F;

		if (lidAngle < var10 && preAngle >= var10 && !adjacentChestZNeg && !adjacentChestXNeg)
		{
			float fx = m_pos.x + 0.5f;
			float fz = m_pos.z + 0.5f;

			if (adjacentChestZPos)
			{
				fz += 0.5f;
			}

			if (adjacentChestXPos)
			{
				fx += 0.5f;
			}

			//worldObj->playSoundEffect({ fx, m_pos.y + 0.5f, fz }, "random.chestclosed", 0.5F, worldObj->m_Rand.nextFloat() * 0.1F + 0.9F);
			worldObj->playSoundEffectByType({ fx, m_pos.y + 0.5f, fz }, ST_CloseChest);
		}

		if (lidAngle < 0.0F)
		{
			lidAngle = 0.0F;
		}
	}
}


bool TileEntityChest::receiveClientEvent(int id, int type)
{
	if (id == 1)
	{
		numUsingPlayers = type;
		return true;
	}
	else
	{
		return TileEntity::receiveClientEvent(id, type);
	}
}

void TileEntityChest::openChest()
{
	if (numUsingPlayers < 0)
	{
		numUsingPlayers = 0;
	}

	++numUsingPlayers;
	worldObj->addBlockEvent(m_pos, getBlock()->getBlockID(), 1, numUsingPlayers);
	worldObj->notifyBlocksOfNeighborChange(m_pos, getBlock()->getBlockID());
	BlockPos downPos = m_pos; --downPos.y;
	worldObj->notifyBlocksOfNeighborChange(downPos, getBlock()->getBlockID());
	changeChestState(true);
}

void TileEntityChest::closeChest()
{
	BlockChest* pbkCheck = dynamic_cast<BlockChest*>(getBlock());
	if (pbkCheck)
	{
		--numUsingPlayers;
		worldObj->addBlockEvent(m_pos, getBlock()->getBlockID(), 1, numUsingPlayers);
		worldObj->notifyBlocksOfNeighborChange(m_pos, getBlock()->getBlockID());
		BlockPos downPos = m_pos; --downPos.y;
		worldObj->notifyBlocksOfNeighborChange(downPos, getBlock()->getBlockID());
		if (numUsingPlayers == 0)
		{
			changeChestState(false);
		}
	}
}

bool TileEntityChest::isChestOpen()
{
	int meta = TileEntity::getBlockMetadata();
	if (meta == -1)
		return false;
	return meta >> 3 == 1;
}

void TileEntityChest::onInventoryChanged()
{
	TileEntity::onInventoryChanged();
}

void TileEntityChest::invalidate()
{
	TileEntity::invalidate();
	updateContainingBlockInfo();
	checkForAdjacentChests();
}

void TileEntityChest::clearChest()
{
	for (int i = 0; i < CHEST_CAPACITY; ++i)
	{
		chestContents[i] = nullptr;
	}
}

int TileEntityChest::getChestType()
{
	if (!worldObj)
		return 0;

	if (cachedChestType == -1)
	{
		BlockChest* pbkChest = dynamic_cast<BlockChest*>(getBlock());
		if (!pbkChest)
			return 0;

		//cachedChestType = pbkChest->isTrapped;
		cachedChestType = pbkChest->getChestType();
	}

	return cachedChestType;
}

int TileEntityChest::getBlockMetadata()
{
	int meta = TileEntity::getBlockMetadata();
	return meta & 7;
}

void TileEntityChest::changeChestState(bool isOpen)
{
	int meta = TileEntity::getBlockMetadata();
	if (!worldObj || meta == -1 || !isOpen && !LogicSetting::Instance()->isCanCloseChest())
		return;

	bool isChange = false;
	if (isOpen)
	{
		meta = meta | 8;
		if (blockMetadata != meta)
		{
			isChange = true;
		}
	}
	else
	{
		meta = meta & 7;
		if (blockMetadata != meta)
		{
			isChange = true;
		}
	}

	if (isChange)
	{
		worldObj->setBlockMetadataWithNotify(m_pos, meta, 3);
	}
}

void TileEntityChest::initByItem(int slot, int itemId, int itemNum, int itemMeta)
{
	if (itemNum < 1) return;

	ItemStackPtr stack = make_shared<ItemStack>(itemId, itemNum, itemMeta);
	if (stack != NULL && stack->getGunSetting())
	{
		stack->m_totalBulletNum = stack->getGunSetting()->maxBulletNum;
		if(stack->isGunItem())
		{
			stack->m_currentClipBulletNum = itemNum;
		}
	}
	this->setInventorySlotContents(slot, stack);
}

void TileEntityChest::initByGunItem(int slot, int itemId, int itemNum, int itemMeta, int bullets)
{
	if (itemNum < 1) return;

	ItemStackPtr stack = make_shared<ItemStack>(itemId, itemNum, itemMeta);
	if (stack->isGunItem())
	{
		stack->m_currentClipBulletNum = bullets;
	}
	if (stack->getGunSetting())
	{
		stack->m_totalBulletNum = stack->getGunSetting()->maxBulletNum;
	}
	this->setInventorySlotContents(slot, stack);
}

void TileEntityChest::addEchantmentItem(int slot, int itemId, int itemNum, int itemMeta, std::vector<std::vector<int>> enchantments)
{
	if (itemNum < 1) return;

	ItemStackPtr stack = make_shared<ItemStack>(itemId, itemNum, itemMeta);
	
	if (stack != NULL && stack->getGunSetting())
	{
		stack->m_totalBulletNum = stack->getGunSetting()->maxBulletNum;
		if (stack->isGunItem())
		{
			stack->m_currentClipBulletNum = itemNum;
		}
	}

	if (stack)
	{
		for (auto enchantment : enchantments)
		{
			stack->addEnchantmentbyId(enchantment[0], enchantment[1]);
		}
	}

	this->setInventorySlotContents(slot, stack);
}

TileEntityDispenser::TileEntityDispenser(int id)
	: TileEntity(id)
{}

TileEntityDispenser::~TileEntityDispenser()
{}

ItemStackPtr TileEntityDispenser::decrStackSize(int slot, int size)
{
	if (dispenserContents == NULL || dispenserContents[slot] == nullptr)
		return NULL;

	ItemStackPtr pResult;

	if (dispenserContents[slot]->stackSize <= size)
	{
		pResult = dispenserContents[slot];
		dispenserContents[slot] = NULL;
		TileEntity::onInventoryChanged();
		return pResult;
	}
	else
	{
		pResult = dispenserContents[slot]->splitStack(size);

		if (dispenserContents[slot]->stackSize == 0)
		{
			dispenserContents[slot] = NULL;
		}

		TileEntity::onInventoryChanged();
		return pResult;
	}
}

ItemStackPtr TileEntityDispenser::getStackInSlotOnClosing(int slot)
{
	if (dispenserContents == NULL || dispenserContents[slot] == NULL)
		return NULL;

	ItemStackPtr pResult = dispenserContents[slot];
	dispenserContents[slot] = NULL;
	return pResult;
}

int TileEntityDispenser::getRandomStackFromInventory()
{
	int nResult = -1;
	int var2 = 1;

	for (int i = 0; i < getSizeInventory(); ++i)
	{
		if (dispenserContents[i] != NULL &&
			dispenserRandom.nextInt(var2++) == 0)
		{
			nResult = i;
		}
	}

	return nResult;
}

void TileEntityDispenser::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	dispenserContents[slot] = pStack;

	if (pStack != NULL && pStack->stackSize > getInventoryStackLimit())
	{
		pStack->stackSize = getInventoryStackLimit();
	}

	TileEntity::onInventoryChanged();
}

bool TileEntityDispenser::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return worldObj->getBlockTileEntity(m_pos) != this ? false : pPlayer->getDistanceSq(Vector3(m_pos) + Vector3::HALF) <= 64.0f;
}

int TileEntityDispenser::addItem(ItemStackPtr pStack)
{
	for (int i = 0; i < getSizeInventory(); ++i)
	{
		if (dispenserContents[i] == NULL || dispenserContents[i]->itemID == 0)
		{
			setInventorySlotContents(i, pStack);
			return i;
		}
	}

	return -1;
}

void TileEntityDispenser::readFromNBT(NBTTagCompound* pNBT)
{
	LordAssert(dispenserContents == NULL);

	TileEntity::readFromNBT(pNBT);
	NBTTagList* pNBTList = pNBT->getTagList("Items");
	dispenserContents = (ItemStackPtr*)LordMalloc(sizeof(ItemStackPtr) * getSizeInventory());
	memset(dispenserContents, 0, sizeof(ItemStackPtr) * getSizeInventory());

	for (int i = 0; i < pNBTList->tagCount(); ++i)
	{
		NBTTagCompound* pNBTCompound = (NBTTagCompound*)pNBTList->tagAt(i);
		int idx = pNBTCompound->getByte("Slot") & 255;

		if (idx >= 0 && idx < getSizeInventory())
		{
			dispenserContents[idx] = ItemStack::loadItemStackFromNBT(pNBTCompound);
		}
	}

	if (pNBT->hasKey("CustomName"))
	{
		customName = pNBT->getString("CustomName");
	}
}

void TileEntityDispenser::writeToNBT(NBTTagCompound* pNBT)
{
	TileEntity::writeToNBT(pNBT);
	NBTTagList* pNBTList = LordNew NBTTagList();

	for (int i = 0; i < getSizeInventory(); ++i)
	{
		if (dispenserContents[i] != NULL)
		{
			NBTTagCompound* pNBTCompound = LordNew NBTTagCompound();
			pNBTCompound->setByte("Slot", (i8)i);
			dispenserContents[i]->writeToNBT(pNBTCompound);
			pNBTList->appendTag(pNBTCompound);
		}
	}

	pNBT->setTag("Items", pNBTList);

	if (isInvNameLocalized())
	{
		pNBT->setString("CustomName", customName);
	}
}

TileEntityDropper::TileEntityDropper()
	: TileEntityDispenser(TILE_ENTITY_DROPPER)
{}

int TileEntityBrewingStand::SLOTS_FOR_UP[3] = { 0, 0, 0 };
int TileEntityBrewingStand::SLOTS_FOR_DOWN[3] = { 0, 1, 2 };

TileEntityBrewingStand::TileEntityBrewingStand()
	: TileEntity(TILE_ENTITY_CAULDRON)
	, brewTime(0)
	, filledSlots(0)
	, ingredientID(0)
{
	for (int i = 0; i < BREW_COUNT; ++i)
		brewingItemStacks[i] = NULL;
}

TileEntityBrewingStand::~TileEntityBrewingStand()
{
	for (int i = 0; i < BREW_COUNT; ++i)
		brewingItemStacks[i] = nullptr;
}

void TileEntityBrewingStand::updateEntity()
{
	if (brewTime > 0)
	{
		--brewTime;

		if (brewTime == 0)
		{
			brewPotions();
			TileEntity::onInventoryChanged();
		}
		else if (!canBrew())
		{
			brewTime = 0;
			TileEntity::onInventoryChanged();
		}
		else if (ingredientID != brewingItemStacks[3]->itemID)
		{
			brewTime = 0;
			TileEntity::onInventoryChanged();
		}
	}
	else if (canBrew())
	{
		brewTime = 400;
		ingredientID = brewingItemStacks[3]->itemID;
	}

	int var1 = getFilledSlots();

	if (var1 != filledSlots)
	{
		filledSlots = var1;
		worldObj->setBlockMetadataWithNotify(m_pos, var1, 2);
	}

	TileEntity::updateEntity();
}

void TileEntityBrewingStand::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);
	NBTTagList* pNbtItems = pNbt->getTagList("Items");

	for (int i = 0; i < pNbtItems->tagCount(); ++i)
	{
		NBTTagCompound* pNbtItem = (NBTTagCompound*)pNbtItems->tagAt(i);
		i8 slot = pNbtItem->getByte("Slot");

		if (slot >= 0 && slot < BREW_COUNT)
		{
			brewingItemStacks[slot] = ItemStack::loadItemStackFromNBT(pNbtItem);
		}
	}

	brewTime = pNbt->getShort("BrewTime");

	if (pNbt->hasKey("CustomName"))
	{
		customName = pNbt->getString("CustomName");
	}
}

void TileEntityBrewingStand::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);
	pNbt->setShort("BrewTime", (i16)brewTime);
	NBTTagList* pNbtItems = LordNew NBTTagList();

	for (int i = 0; i < BREW_COUNT; ++i)
	{
		if (brewingItemStacks[i])
		{
			NBTTagCompound* pNbtItem = LordNew NBTTagCompound();
			pNbtItem->setByte("Slot", (i8)i);
			brewingItemStacks[i]->writeToNBT(pNbtItem);
			pNbtItems->appendTag(pNbtItem);
		}
	}

	pNbt->setTag("Items", pNbtItems);

	if (isInvNameLocalized())
	{
		pNbt->setString("CustomName", customName);
	}

	LordSafeDelete(pNbtItems);
}

ItemStackPtr TileEntityBrewingStand::decrStackSize(int slot, int size)
{
	if (slot >= 0 && slot < BREW_COUNT)
	{
		ItemStackPtr pResult = brewingItemStacks[slot];
		brewingItemStacks[slot] = NULL;
		return pResult;
	}
	return NULL;
}

ItemStackPtr TileEntityBrewingStand::getStackInSlotOnClosing(int slot)
{
	if (slot >= 0 && slot < BREW_COUNT)
	{
		ItemStackPtr pResult = brewingItemStacks[slot];
		brewingItemStacks[slot] = NULL;
		return pResult;
	}
	return NULL;
}

void TileEntityBrewingStand::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot >= 0 && slot < BREW_COUNT)
	{
		brewingItemStacks[slot] = pStack;
	}
}

bool TileEntityBrewingStand::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return worldObj->getBlockTileEntity(m_pos) != this ? false : pPlayer->getDistanceSq(Vector3(m_pos) + Vector3::HALF) <= 64.0f;
}

bool TileEntityBrewingStand::isItemValidForSlot(int slot, ItemStackPtr pStack)
{
	return slot == 3 ? Item::itemsList[pStack->itemID]->isPotionIngredient() : pStack->itemID == Item::potion->itemID || pStack->itemID == Item::glassBottle->itemID;
}

int* TileEntityBrewingStand::getAccessibleSlotsFromSide(int side, int& outsize)
{
	outsize = 3;
	return side == 1 ? SLOTS_FOR_UP : SLOTS_FOR_DOWN;
}

int TileEntityBrewingStand::getFilledSlots()
{
	int fills = 0;

	for (int i = 0; i < 3; ++i)
	{
		if (brewingItemStacks[i] != NULL)
		{
			fills |= 1 << i;
		}
	}

	return fills;
}

bool TileEntityBrewingStand::canBrew()
{
	if (brewingItemStacks[3] != NULL && brewingItemStacks[3]->stackSize > 0)
	{
		ItemStackPtr pStack = brewingItemStacks[3];

		if (!Item::itemsList[pStack->itemID]->isPotionIngredient())
			return false;

		bool bResult = false;

		for (int i = 0; i < 3; ++i)
		{
			if (brewingItemStacks[i] != NULL && brewingItemStacks[i]->itemID == Item::potion->itemID)
			{
				int damage1 = brewingItemStacks[i]->getItemDamage();
				int damage2 = getPotionResult(damage1, pStack);

				if (!ItemPotion::isSplash(damage1) && ItemPotion::isSplash(damage2))
				{
					bResult = true;
					break;
				}

				PotionEffectArr effects1;
				PotionEffectArr effects2;
				Item::potion->getEffects(damage1, effects1);
				Item::potion->getEffects(damage2, effects2);

				if ((damage1 <= 0 || damage1 != damage2) && (effects1.empty() || !(effects1 == effects2) && (!effects2.empty())) && damage1 != damage2)
				{
					bResult = true;
					break;
				}
			}
		}

		return bResult;
	}
	return false;
}

void TileEntityBrewingStand::brewPotions()
{
	if (!canBrew())
		return;

	ItemStackPtr pStack = brewingItemStacks[3];

	for (int i = 0; i < 3; ++i)
	{
		if (brewingItemStacks[i] && brewingItemStacks[i]->itemID == Item::potion->itemID)
		{
			int damage1 = brewingItemStacks[i]->getItemDamage();
			int damage2 = getPotionResult(damage1, pStack);
			PotionEffectArr effects1;
			PotionEffectArr effects2;
			Item::potion->getEffects(damage1, effects1);
			Item::potion->getEffects(damage2, effects2);

			if ((damage1 <= 0 || damage1 != damage1) && (effects1.empty() || !(effects1 == effects2) && (!effects2.empty())))
			{
				if (damage1 != damage2)
				{
					brewingItemStacks[i]->setItemDamage(damage2);
				}
			}
			else if (!ItemPotion::isSplash(damage1) && ItemPotion::isSplash(damage2))
			{
				brewingItemStacks[i]->setItemDamage(damage2);
			}
		}
	}

	if (Item::itemsList[pStack->itemID]->hasContainerItem())
	{
		brewingItemStacks[3] = LORD::make_shared<ItemStack>(Item::itemsList[pStack->itemID]->getContainerItem());
	}
	else
	{
		--brewingItemStacks[3]->stackSize;

		if (brewingItemStacks[3]->stackSize <= 0)
		{
			brewingItemStacks[3] = nullptr;
		}
	}
}

int TileEntityBrewingStand::getPotionResult(int damageType, ItemStackPtr pStack)
{
	if (!pStack)
		return damageType;

	if (Item::itemsList[pStack->itemID]->isPotionIngredient())
		return PotionHelper::applyIngredient(damageType, Item::itemsList[pStack->itemID]->getPotionEffect());

	return damageType;
}

Potion* TileEntityBeacon::effectsList[4][2];

TileEntityBeacon::TileEntityBeacon()
	: TileEntity(TILE_ENTITY_BEACON)
	, m_renderCount(0LL)
	, m_renderScale(0.f)
	, m_bActive(false)
	, m_level(-1)
	, m_primaryEff(0)
	, m_secondaryEff(0)
{}

TileEntityBeacon::~TileEntityBeacon()
{
	m_payment = nullptr;
}

void TileEntityBeacon::initialize()
{
	effectsList[0][0] = Potion::moveSpeed;
	effectsList[0][1] = Potion::digSpeed;
	effectsList[1][0] = Potion::resistance;
	effectsList[1][1] = Potion::jump;
	effectsList[2][0] = Potion::damageBoost;
	effectsList[2][1] = NULL;
	effectsList[3][0] = Potion::regeneration;
	effectsList[3][1] = NULL;
}

void TileEntityBeacon::updateEntity()
{
	if (worldObj->getTotalWorldTime() % 80L == 0L)
	{
		updateState();
		addEffectsToPlayers();
	}
}

void TileEntityBeacon::addEffectsToPlayers()
{
	if (!(m_bActive && m_level > 0 && !worldObj->m_isClient && m_primaryEff > 0))
		return;

	float range = m_level * 10.f + 10;
	i8 amplifier = 0;

	if (m_level >= 4 && m_primaryEff == m_secondaryEff)
		amplifier = 1;

	Box box = Box(Vector3(m_pos), Vector3(m_pos) + Vector3::ONE).expand(range);
	box.vMax.y = float(worldObj->getHeight());
	EntityArr players = worldObj->getEntitiesWithinAABB(ENTITY_CLASS_PLAYER, box);
	EntityPlayer* pPlayer = NULL;
	for (auto it : players)
	{
		pPlayer = (EntityPlayer*)it;
		pPlayer->addPotionEffect(LordNew PotionEffect(m_primaryEff, 180, amplifier, true));
	}

	if (m_level >= 4 && m_primaryEff != m_secondaryEff && m_secondaryEff > 0)
	{
		for (auto it : players)
		{
			pPlayer = (EntityPlayer*)it;
			pPlayer->addPotionEffect(new PotionEffect(m_secondaryEff, 180, 0, true));
		}
	}
}

void TileEntityBeacon::updateState()
{
	if (!worldObj->canBlockSeeTheSky(m_pos.getPosY()))
	{
		m_bActive = false;
		m_level = 0;
	}
	else
	{
		m_bActive = true;
		m_level = 0;

		for (int i = 1; i <= 4; m_level = i++)
		{
			int y = m_pos.y - i;

			if (y < 0)
				break;

			bool flag = true;

			for (int x = m_pos.x - i; x <= m_pos.x + i && flag; ++x)
			{
				for (int z = m_pos.z - i; z <= m_pos.z + i; ++z)
				{
					int blockID = worldObj->getBlockId(BlockPos(x, y, z));

					if (blockID != BLOCK_ID_BLOCK_EMERALD && blockID != BLOCK_ID_BLOCK_GOLD && blockID != BLOCK_ID_BLOCK_DIAMOND && blockID != BLOCK_ID_BLOCK_IRON)
					{
						flag = false;
						break;
					}
				}
			}

			if (!flag)
				break;
		}

		if (m_level == 0)
		{
			m_bActive = false;
		}
	}
}

float TileEntityBeacon::shouldRender()
{
	if (!m_bActive)
		return 0.0F;

	int var1 = (int)(worldObj->getTotalWorldTime() - m_renderCount);
	m_renderCount = worldObj->getTotalWorldTime();

	if (var1 > 1)
	{
		m_renderScale -= (float)var1 / 40.0F;

		if (m_renderScale < 0.0F)
		{
			m_renderScale = 0.0F;
		}
	}

	m_renderScale += 0.025F;

	if (m_renderScale > 1.0F)
	{
		m_renderScale = 1.0F;
	}

	return m_renderScale;
}

void TileEntityBeacon::setPrimaryEffect(int effectID)
{
	m_primaryEff = 0;

	for (int i = 0; i < m_level && i < 3; ++i)
	{
		Potion** potions = (Potion**)(effectsList[i]);

		for (int j = 0; j < 2; ++j)
		{
			Potion* pPotion = potions[j];

			if (pPotion && pPotion->getId() == effectID)
			{
				m_primaryEff = effectID;
				return;
			}
		}
	}
}

void TileEntityBeacon::setSecondaryEffect(int effectID)
{
	m_secondaryEff = 0;

	if (m_level >= 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			Potion** potions = (Potion**)(effectsList[i]);

			for (int j = 0; j < 2; ++j)
			{
				Potion* pPotion = potions[j];

				if (pPotion->getId() == effectID)
				{
					m_secondaryEff = effectID;
					return;
				}
			}
		}
	}
}

void TileEntityBeacon::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);
	m_primaryEff = pNbt->getInteger("Primary");
	m_secondaryEff = pNbt->getInteger("Secondary");
	m_level = pNbt->getInteger("Levels");
}

void TileEntityBeacon::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);
	pNbt->setInteger("Primary", m_primaryEff);
	pNbt->setInteger("Secondary", m_secondaryEff);
	pNbt->setInteger("Levels", m_level);
}

ItemStackPtr TileEntityBeacon::decrStackSize(int slot, int size)
{
	if (slot == 0 && m_payment != NULL)
	{
		if (size >= m_payment->stackSize)
		{
			ItemStackPtr pStack = m_payment;
			m_payment = NULL;
			return pStack;
		}
		else
		{
			m_payment->stackSize -= size;
			return LORD::make_shared<ItemStack>(m_payment->itemID, size, m_payment->getItemDamage());
		}
	}
	return NULL;
}

ItemStackPtr TileEntityBeacon::getStackInSlotOnClosing(int slot)
{
	if (slot == 0 && m_payment != NULL)
	{
		ItemStackPtr pStack = m_payment;
		m_payment = NULL;
		return pStack;
	}
	return NULL;
}

void TileEntityBeacon::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot == 0)
	{
		m_payment = nullptr;
		m_payment = pStack;
	}
}

bool TileEntityBeacon::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return worldObj->getBlockTileEntity(m_pos) != this ? false : pPlayer->getDistanceSq(Vector3(m_pos) + Vector3::HALF) <= 64.0f;
}

bool TileEntityBeacon::isItemValidForSlot(int slot, ItemStackPtr pStack)
{
	return
		pStack->itemID == Item::emerald->itemID ||
		pStack->itemID == Item::diamond->itemID ||
		pStack->itemID == Item::ingotGold->itemID ||
		pStack->itemID == Item::ingotIron->itemID;
}

TileEntityHopper::TileEntityHopper()
	: TileEntity(TILE_ENTITY_HOPPER)
	, m_transferCD(-1)
{
	for (int i = 0; i < HOPPER_ITEM_SIZE; ++i)
		m_itemStacks[i] = NULL;
}

TileEntityHopper::~TileEntityHopper()
{
	for (int i = 0; i < HOPPER_ITEM_SIZE; ++i)
		m_itemStacks[i] = nullptr;
}

void TileEntityHopper::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);
	NBTTagList* pNbtItems = pNbt->getTagList("Items");

	if (pNbt->hasKey("CustomName"))
	{
		m_customName = pNbt->getString("CustomName");
	}

	m_transferCD = pNbt->getInteger("TransferCooldown");

	for (int i = 0; i < pNbtItems->tagCount(); ++i)
	{
		NBTTagCompound* pNbtItem = (NBTTagCompound*)pNbtItems->tagAt(i);
		i8 slot = pNbtItem->getByte("Slot");

		if (slot >= 0 && slot < HOPPER_ITEM_SIZE)
		{
			m_itemStacks[slot] = nullptr;
			m_itemStacks[slot] = ItemStack::loadItemStackFromNBT(pNbtItem);
		}
	}
}

void TileEntityHopper::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);
	NBTTagList* pNbtItems = LordNew NBTTagList();

	for (int i = 0; i < HOPPER_ITEM_SIZE; ++i)
	{
		if (m_itemStacks[i])
		{
			NBTTagCompound* pNbtItem = LordNew NBTTagCompound();
			pNbtItem->setByte("Slot", (i8)i);
			m_itemStacks[i]->writeToNBT(pNbtItem);
			pNbtItems->appendTag(pNbtItem);
		}
	}

	pNbt->setTag("Items", pNbtItems);
	pNbt->setInteger("TransferCooldown", m_transferCD);

	if (isInvNameLocalized())
	{
		pNbt->setString("CustomName", m_customName);
	}
	LordSafeDelete(pNbtItems);
}

ItemStackPtr TileEntityHopper::decrStackSize(int slot, int size)
{
	if (!m_itemStacks[slot])
		return NULL;

	ItemStackPtr pResult = NULL;

	if (m_itemStacks[slot]->stackSize <= size)
	{
		pResult = m_itemStacks[slot];
		m_itemStacks[slot] = NULL;
		return pResult;
	}
	else
	{
		pResult = m_itemStacks[slot]->splitStack(size);

		if (m_itemStacks[slot]->stackSize == 0)
		{
			m_itemStacks[slot] = NULL;
		}

		return pResult;
	}
}

ItemStackPtr TileEntityHopper::getStackInSlotOnClosing(int slot)
{
	if (m_itemStacks[slot])
	{
		ItemStackPtr pResult = m_itemStacks[slot];
		m_itemStacks[slot] = NULL;
		return pResult;
	}
	return NULL;
}

void TileEntityHopper::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	m_itemStacks[slot] = pStack;

	if (pStack && pStack->stackSize > getInventoryStackLimit())
	{
		pStack->stackSize = getInventoryStackLimit();
	}
}

bool TileEntityHopper::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return worldObj->getBlockTileEntity(m_pos) != this ? false : pPlayer->getDistanceSq(Vector3(m_pos) + Vector3::HALF) <= 64.f;
}

void TileEntityHopper::updateEntity()
{
	if (worldObj && !worldObj->m_isClient)
	{
		--m_transferCD;

		if (!isCoolingDown())
		{
			setTransferCooldown(0);
			updateHopper();
		}
	}
}

bool TileEntityHopper::updateHopper()
{
	if (worldObj && !worldObj->m_isClient)
	{
		if (!isCoolingDown() && BlockHopper::getIsBlockNotPoweredFromMetadata(getBlockMetadata()))
		{
			bool flag = insertItemToInventory();
			flag = suckItemsIntoHopper(this) || flag;

			if (flag)
			{
				setTransferCooldown(8);
				onInventoryChanged();
				return true;
			}
		}
	}
	return false;
}

bool TileEntityHopper::insertItemToInventory()
{
	IInventory* pInventory = getOutputInventory();

	if (!pInventory)
	{
		return false;
	}

	for (int i = 0; i < getSizeInventory(); ++i)
	{
		if (getStackInSlot(i))
		{
			ItemStackPtr pStack1 = getStackInSlot(i)->copy();
			ItemStackPtr pStack2 = insertStack(pInventory, decrStackSize(i, 1), oppositeSide[BlockHopper::getDirectionFromMetadata(getBlockMetadata())]);

			if (!pStack2 || pStack2->stackSize == 0)
			{
				pInventory->onInventoryChanged();
				return true;
			}

			setInventorySlotContents(i, pStack1);
		}
	}

	return false;
}

bool TileEntityHopper::suckItemsIntoHopper(IHopper* pHopper)
{
	IInventory* pInventory = getInventoryAboveHopper(pHopper);

	if (pInventory)
	{
		i8 side = 0;
		ISidedInventory* pSideInventory = dynamic_cast<ISidedInventory*>(pInventory);
		if (pSideInventory && side > -1)
		{
			int slotsize;
			int* slots = pSideInventory->getAccessibleSlotsFromSide(side, slotsize);

			for (int i = 0; i < slotsize; ++i)
			{
				if (pullItemFromSlot(pHopper, pInventory, slots[i], side))
				{
					return true;
				}
			}
		}
		else
		{
			int size = pInventory->getSizeInventory();

			for (int i = 0; i < size; ++i)
			{
				if (pullItemFromSlot(pHopper, pInventory, i, side))
				{
					return true;
				}
			}
		}
	}
	else
	{
		EntityItem* pItem = getCaptureItem(pHopper->getWorldObj(), pHopper->getPos() + Vector3::UNIT_Y);

		if (pItem != NULL)
		{
			return insertItem(pHopper, pItem);
		}
	}

	return false;
}

bool TileEntityHopper::pullItemFromSlot(IHopper* pHopper, IInventory* pInventory, int slot, int side)
{
	ItemStackPtr pStack = pInventory->getStackInSlot(slot);

	if (pStack && canExtractItemFromInventory(pInventory, pStack, slot, side))
	{
		ItemStackPtr pStack1 = pStack->copy();
		ItemStackPtr pStack2 = insertStack(pHopper, pInventory->decrStackSize(slot, 1), -1);

		if (!pStack2 || pStack2->stackSize == 0)
		{
			pInventory->onInventoryChanged();
			return true;
		}

		pInventory->setInventorySlotContents(slot, pStack1);
	}

	return false;
}

bool TileEntityHopper::insertItem(IInventory* pInventory, EntityItem* pItem)
{
	bool bResult = false;

	if (pItem)
		return false;

	ItemStackPtr pStack1 = pItem->getEntityItem()->copy();
	ItemStackPtr pStack2 = insertStack(pInventory, pStack1, -1);

	if (pStack2 && pStack2->stackSize != 0)
	{
		pItem->setEntityItemStack(pStack2);
	}
	else
	{
		bResult = true;
		pItem->setDead();
	}

	return bResult;
}

ItemStackPtr TileEntityHopper::insertStack(IInventory* pInventory, ItemStackPtr pStack, int side)
{
	ISidedInventory* pSideInventory = dynamic_cast<ISidedInventory*>(pInventory);
	if (pSideInventory && side > -1)
	{
		int outside = 0;
		int* slots = pSideInventory->getAccessibleSlotsFromSide(side, outside);

		for (int i = 0; i < outside && pStack && pStack->stackSize > 0; ++i)
		{
			pStack = insertStackInSlot(pInventory, pStack, slots[i], side);
		}
	}
	else
	{
		int size = pInventory->getSizeInventory();

		for (int i = 0; i < size && pStack && pStack->stackSize > 0; ++i)
		{
			pStack = insertStackInSlot(pInventory, pStack, i, side);
		}
	}

	if (pStack && pStack->stackSize == 0)
	{
		pStack = NULL;
	}

	return pStack;
}

bool TileEntityHopper::canInsertItemToInventory(IInventory* pInventory, ItemStackPtr pStack, int slot, int side)
{
	if (!pInventory->isItemValidForSlot(slot, pStack))
		return false;

	ISidedInventory* pSideInventory = dynamic_cast<ISidedInventory*>(pInventory);
	return !pSideInventory || pSideInventory->canInsertItem(slot, pStack, side);
}

bool TileEntityHopper::canExtractItemFromInventory(IInventory* pInventory, ItemStackPtr pStack, int slot, int side)
{
	ISidedInventory* pSideInventory = dynamic_cast<ISidedInventory*>(pInventory);
	return !pSideInventory || pSideInventory->canExtractItem(slot, pStack, side);
}

ItemStackPtr TileEntityHopper::insertStackInSlot(IInventory* pInventory, ItemStackPtr pStack, int slot, int side)
{
	ItemStackPtr pSlotStack = pInventory->getStackInSlot(slot);

	if (canInsertItemToInventory(pInventory, pStack, slot, side))
	{
		bool flag = false;

		if (!pSlotStack)
		{
			pInventory->setInventorySlotContents(slot, pStack);
			pStack = NULL;
			flag = true;
		}
		else if (areItemStacksEqualItem(pSlotStack, pStack))
		{
			int s1 = pStack->getMaxStackSize() - pSlotStack->stackSize;
			int s2 = pStack->stackSize;
			int s3 = Math::Min(s2, s1);
			pStack->stackSize -= s3;
			pSlotStack->stackSize += s3;
			flag = s3 > 0;
		}

		if (flag)
		{
			TileEntityHopper* pHopper = dynamic_cast<TileEntityHopper*>(pInventory);
			if (pHopper)
			{
				pHopper->setTransferCooldown(8);
				pInventory->onInventoryChanged();
			}

			pInventory->onInventoryChanged();
		}
	}

	return pStack;
}

IInventory* TileEntityHopper::getOutputInventory()
{
	int dir = BlockHopper::getDirectionFromMetadata(getBlockMetadata());
	return getInventoryAtLocation(getWorldObj(), Vector3(m_pos + offsetForSide[dir]));
}

IInventory* TileEntityHopper::getInventoryAboveHopper(IHopper* pHopper)
{
	return getInventoryAtLocation(pHopper->getWorldObj(), pHopper->getPos() + Vector3::UNIT_Y);
}

EntityItem* TileEntityHopper::getCaptureItem(World* pWorld, const Vector3& pos)
{
	Box aabb(pos, pos + Vector3::ONE);
	EntityArr entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ITEM, aabb);
	return entities.size() > 0 ? (EntityItem*)entities[0] : NULL;
}

IInventory* TileEntityHopper::getInventoryAtLocation(World* pWorld, const Vector3& pos)
{
	IInventory* pResult = NULL;
	BlockPos pos1 = pos.getFloor();
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos1);

	if (pTileEntity)
	{
		IInventory* pInventory = dynamic_cast<IInventory*>(pTileEntity);
		if (pInventory)
			pResult = pInventory;

		TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pInventory);
		if (pChest)
		{
			int blockID = pWorld->getBlockId(pos1);
			Block* pBlock = BlockManager::sBlocks[blockID];
			BlockChest* pBlockChest = dynamic_cast<BlockChest*>(pBlock);
			if (pBlockChest)
			{
				pResult = pBlockChest->getInventory(pWorld, pos1);
			}
		}
	}

	if (pResult == NULL)
	{
		Box aabb(pos, pos + Vector3::ONE);
		EntityArr entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_CONTAINER, aabb);

		if (entities.size() > 0)
		{
			int idx = pWorld->m_Rand.nextInt(entities.size());
			pResult = dynamic_cast<IInventory*>(entities[idx]);
		}
	}

	return pResult;
}

bool TileEntityHopper::areItemStacksEqualItem(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return pStack1->itemID != pStack2->itemID ? false :
		(pStack1->getItemDamage() != pStack2->getItemDamage() ? false :
		(pStack1->stackSize > pStack1->getMaxStackSize() ? false :
			ItemStack::areItemStackTagsEqual(pStack1, pStack2)));
}

}