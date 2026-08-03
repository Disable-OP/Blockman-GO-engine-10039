#include "BehaviorDispenseItem.h"
#include "ItemStack.h"
#include "Items.h"

#include "Entity/EntityItem.h"
#include "World/World.h"

namespace BLOCKMAN
{
	
IBehaviorDispenseItem::IBehaviorDispenseItem()
{
	itemDispenseBehaviorProvider = LordNew BehaviorDispenseItemProvider();
}

IBehaviorDispenseItem::~IBehaviorDispenseItem()
{
	LordSafeDelete(itemDispenseBehaviorProvider);
}

Vector3 IBehaviorDispenseItem::getIPositionFromBlockSource(World* pWorld, const Vector3i& pos)
{
	Facing* facing = Facing::getFront(pWorld->getBlockMeta(pos) & 7);
	float x = pos.x + 0.5f + 0.7f * facing->getFrontOffsetX();
	float y = pos.y + 0.5f + 0.7f * facing->getFrontOffsetY();
	float z = pos.z + 0.5f + 0.7f * facing->getFrontOffsetZ();
	return Vector3(x, y, z);
}

void BehaviorDefaultDispenseItem::playDispenseSound(World* pWorld, const Vector3i& pos)
{
	// pWorld->playAuxSFX(1000, pos.x, pos.y, pos.z, 0);
}

void BehaviorDefaultDispenseItem::spawnDispenseParticles(World* pWorld, const Vector3i& pos, Facing* par2EnumFacing)
{
	// pWorld->playAuxSFX(2000, pos.x, pos.y, pos.z, getWorldEventDataFrom(par2EnumFacing));
}

int BehaviorDefaultDispenseItem::getWorldEventDataFrom(Facing* facing)
{
	return facing->getFrontOffsetX() + 1 + (facing->getFrontOffsetZ() + 1) * 3;
}

/** Dispenses the specified ItemStack from a dispenser. */
ItemStackPtr BehaviorDefaultDispenseItem::dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack)
{
	ItemStackPtr pDispense = dispenseStack(pWorld, pos, pStack);
	playDispenseSound(pWorld, pos);
	spawnDispenseParticles(pWorld, pos, Facing::getFront(pWorld->getBlockMeta(pos) & 7));
	return pDispense;
}

/** Dispense the specified stack, play the dispense sound and spawn particles. */
ItemStackPtr BehaviorDefaultDispenseItem::dispenseStack(World* pWorld, const Vector3i& pos, ItemStackPtr pStack)
{
	Facing* facing = Facing::getFront(pWorld->getBlockMeta(pos) & 7);
	Vector3 pos2 = getIPositionFromBlockSource(pWorld, pos);
	ItemStackPtr pSplit = pStack->splitStack(1);
	doDispense(pWorld, pSplit, 6, facing, pos2);
	return pStack;
}

void BehaviorDefaultDispenseItem::doDispense(World* pWorld, ItemStackPtr pStack, int par2, Facing* facing, const Vector3& pos)
{
	EntityItem* pItem = LordNew EntityItem(pWorld, pos + Vector3(0.f, -0.3f, 0.f), pStack);
	float randrange = pWorld->m_Rand.nextFloat() * 0.1f + 0.2f;
	pItem->motion.x = facing->getFrontOffsetX() * randrange;
	pItem->motion.y = 0.2f;
	pItem->motion.z = facing->getFrontOffsetZ() * randrange;
	pItem->motion.x += float(pWorld->m_Rand.nextGaussian() * 0.0075f * par2);
	pItem->motion.y += float(pWorld->m_Rand.nextGaussian() * 0.0075f * par2);
	pItem->motion.z += float(pWorld->m_Rand.nextGaussian() * 0.0075f * par2);
	pWorld->spawnEntityInWorld(pItem);
}

DispenserBehaviorPotion::DispenserBehaviorPotion()
{
	defaultDispenserItemBehavior = LordNew BehaviorDefaultDispenseItem();
}

DispenserBehaviorPotion::~DispenserBehaviorPotion()
{
	LordSafeDelete(defaultDispenserItemBehavior);
}

ItemStackPtr DispenserBehaviorPotion::dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack)
{
	if (ItemPotion::isSplash(pStack->getItemDamage()))
	{
		// todo. 
		// (LordNew DispenserBehaviorPotionProjectile(this, pStack))->dispense(pWorld, pos, pStack);
		return NULL;
	}
	else
	{
		return defaultDispenserItemBehavior->dispense(pWorld, pos, pStack);
	}

	return NULL; // silence 
}

}