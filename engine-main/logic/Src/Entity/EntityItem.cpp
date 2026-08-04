#include "EntityItem.h"
#include "DamageSource.h"

#include "Nbt/NBT.h"
#include "Block/BM_Material.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "World/World.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Entity/EntityPlayer.h"
#include "Inventory/InventoryPlayer.h"
#include "Common.h"
#include "Item/Items.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/LogicSetting.h"

namespace BLOCKMAN
{

void EntityItem::init_entityItem()
{
	entityType = 3;
	health = 5;
	age = 0;
	delayBeforeCanPickup = 0;
	hoverStart = Math::UnitRandom() * Math::PI_2;
	setSize(0.25F, 0.25F);
	yOffset = height / 2.0F;
	rotationYaw = Math::UnitRandom() * 360.0f;
	motion.x = Math::UnitRandom() * 0.2f - 0.1f;
	motion.y = 0.2f;
	motion.z = Math::UnitRandom() * 0.2f - 0.1f;
}

EntityItem::EntityItem(World* pWorld, const Vector3& pos)
	: Entity(pWorld)
{
	init_entityItem();
	setPosition(pos);
}

EntityItem::EntityItem(World* pWorld, const Vector3& pos, ItemStackPtr pStack)
	: Entity(pWorld)
{
	init_entityItem();
	setPosition(pos);
	setEntityItemStack(pStack);
}

EntityItem::EntityItem(World* pWorld)
	: Entity(pWorld)
{
	health = 5;
	age = 0;
	delayBeforeCanPickup = 0;
	hoverStart = Math::UnitRandom() * Math::PI_2;
	setSize(0.25F, 0.25F);
	yOffset = height / 2.0F;
	entityType = 3;
}

EntityItem::~EntityItem()
{

}

void EntityItem::onUpdate()
{
	Entity::onUpdate();

	if (delayBeforeCanPickup > 0)
	{
		--delayBeforeCanPickup;
		if (delayBeforeCanPickup % 10 == 0) {
			//LordLogInfo("onUpdate delayBeforeCanPickup [%d]", delayBeforeCanPickup);
		}
	}

	prevPos = position;
	motion.y -= 0.04f;
	
	if (canFall())
	{
		noClip = pushOutOfBlocks(getCentorPos());
		moveEntity(motion);
	}
	
	bool posChanged = (int)prevPos.x != (int)position.x || (int)prevPos.y != (int)position.y || (int)prevPos.z != (int)position.z;
	BlockPos bpos = position.getFloor();

	if (posChanged || ticksExisted % 25 == 0)
	{
		if (world->getBlockMaterial(bpos) == BM_Material::BM_MAT_lava)
		{
			motion.y = 0.2f;
			motion.x = (rand->nextFloat() - rand->nextFloat()) * 0.2F;
			motion.z = (rand->nextFloat() - rand->nextFloat()) * 0.2F;
			//playSound("random.fizz", 0.4F, 2.0F + rand->nextFloat() * 0.4F);
			playSoundByType(ST_Fizz);
		}

		if (!world->m_isClient)
		{
			searchForOtherItemsNearby();
		}
	}

	float g = 0.98F;

	if (onGround)
	{
		g = 0.588f;
		int blockid = world->getBlockId(bpos.getNegY());

		if (blockid > 0)
		{
			Block* pBlock = BlockManager::sBlocks[blockid];
			if(pBlock)
				g = pBlock->slipperiness() * 0.98F;
		}
	}

	motion.x *= g;
	motion.y *= 0.98f;
	motion.z *= g;

	if (onGround)
	{
		motion.y *= -0.5f;
	}

	++age;

	if (!world->m_isClient && age >= LogicSetting::Instance()->getEntityItemLife() && !isEverLife)
	{
		setDead();
	}
}

void EntityItem::searchForOtherItemsNearby()
{
	if (!isCombine)
	{
		return;
	}
	EntityArr entities = world->getEntitiesWithinAABB(ENTITY_CLASS_ITEM, boundingBox.expland(0.5f, 0.0f, 0.5f));

	for (EntityArr::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		EntityItem* pItem = dynamic_cast<EntityItem*>(*it);
		if (pItem)
			combineItems(pItem);
	}
}

bool EntityItem::combineItems(EntityItem* other)
{
	if (other == this)
	{
		return false;
	}
	else if (other->isEntityAlive() && isEntityAlive())
	{
		ItemStackPtr myStack = getEntityItem();
		ItemStackPtr otherStack = other->getEntityItem();

		if (otherStack->getItem() != myStack->getItem())
		{
			return false;
		}
		else if (otherStack->hasTagCompound() ^ myStack->hasTagCompound())
		{
			return false;
		}
		else if (otherStack->hasTagCompound() && !otherStack->getTagCompound()->equals(myStack->getTagCompound()))
		{
			return false;
		}
		else if (otherStack->getItem()->getHasSubtypes() && otherStack->getItemDamage() != myStack->getItemDamage())
		{
			return false;
		}
		else if (otherStack->stackSize < myStack->stackSize)
		{
			return other->combineItems(this);
		}
		else if (otherStack->stackSize + myStack->stackSize > otherStack->getMaxStackSize())
		{
			return false;
		}
		else
		{
			otherStack->stackSize += myStack->stackSize;
			other->delayBeforeCanPickup = Math::Max(other->delayBeforeCanPickup, delayBeforeCanPickup);
			other->age = Math::Min(other->age, age);
			other->setEntityItemStack(otherStack);
			setDead();
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool EntityItem::handleWaterMovement()
{
	return world->handleMaterialAcceleration(boundingBox, BM_Material::BM_MAT_water, this);
}

void EntityItem::dealFireDamage(int amount)
{
	attackEntityFrom(DamageSource::inFire, (float)amount);
}

bool EntityItem::attackEntityFrom(DamageSource* source, float amount)
{
	if (isEntityInvulnerable())
	{
		return false;
	}
	// todo
	// else if (getEntityItem() && getEntityItem()->itemID == Item::netherStar->itemID && source->isExplosion())
	else if (getEntityItem() && getEntityItem()->itemID == 143 && source->isExplosion())
	{
		return false;
	}
	else
	{
		setBeenAttacked();
		health = (int)((float)health - amount);

		if (health <= 0)
		{
			setDead();
		}

		return false;
	}
}

void EntityItem::writeEntityToNBT(NBTTagCompound* pNbtCompound)
{
	pNbtCompound->setShort("Health", (i16)(health));
	pNbtCompound->setShort("Age", (i16)age);

	if (getEntityItem())
	{
		pNbtCompound->setCompoundTag("Item", getEntityItem()->writeToNBT(LordNew NBTTagCompound()));
	}
}

void EntityItem::readEntityFromNBT(NBTTagCompound* pNbtCompound)
{
	health = pNbtCompound->getShort("Health") & 255;
	age = pNbtCompound->getShort("Age");
	NBTTagCompound* pNbtStack = pNbtCompound->getCompoundTag("Item");
	setEntityItemStack(ItemStack::loadItemStackFromNBT(pNbtStack));

	if (!getEntityItem())
	{
		setDead();
	}
}

void EntityItem::onCollideWithPlayer(EntityPlayer* pPlayer)
{
	ItemStackPtr gunStack = nullptr;

	if (delayBeforeCanPickup != 0) {
		return;
	}

	ItemStackPtr pStack = getEntityItem();
	Item* pItem = pStack->getItem();
	if (!pItem)
		return;

	int size = pStack->stackSize;

	if (!pPlayer->canPickupItem(this, size))
	{
		return;
	}

	bool succ = false;
	succ = pPlayer->inventory->addItemStackToInventory(pStack);
	if (!succ) {
		return;
	}

	if (BlockLogBase::isWood(pStack->itemID))
	{
		// todo.
		// pPlayer->triggerAchievement(AchievementList.mineWood);
	}

	if (pStack->itemID == Item::leather->itemID)
	{
		// todo.
		// pPlayer.triggerAchievement(AchievementList.killCow);
	}

	if (pStack->itemID == Item::diamond->itemID)
	{
		// todo.
		// pPlayer.triggerAchievement(AchievementList.diamonds);
	}

	if (pStack->itemID == Item::blazeRod->itemID)
	{
		// todo.
		// pPlayer.triggerAchievement(AchievementList.blazeRod);
	}

	//playSound("random.pop", 0.2F, ((rand->nextFloat() - rand->nextFloat()) * 0.7F + 1.0F) * 2.0F);
	playSoundByType(ST_Pop);
	pPlayer->onItemPickup(this, size); 
	BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendPlaySound(pPlayer,"random.pop", 0.2f, ((rand->nextFloat() - rand->nextFloat()) * 0.7F + 1.0F) * 2.0F);

	if (pStack->stackSize <= 0)
	{
		setDead();
	}
}

String EntityItem::getEntityName()
{
	// todo.
	// return StatCollector.translateToLocal("item." + getEntityItem().getItemName());
	return String("item.") + getEntityItem()->getItemName();
}

ItemStackPtr EntityItem::getEntityItem()
{
	ItemStackPtr pResult = data_itemstack;

	if (!pResult)
	{
		LordLogError("Item entity %d has no item?", entityId);
		return LORD::make_shared<ItemStack>(BlockManager::stone);
	}
	return pResult;
}

}