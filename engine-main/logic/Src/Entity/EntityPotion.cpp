#include "EntityPotion.h"
#include "World/World.h"
#include "EntityLivingBase.h"
#include "Item/Items.h"
#include "Item/Potion.h"
#include "EntityPlayer.h"

using namespace BLOCKMAN;

EntityPotion::EntityPotion(World* pWorld) 
	: EntityThrowable(pWorld)
{

}

EntityPotion::EntityPotion(World* pWorld, EntityLivingBase* throwerIn, ItemStackPtr usinItem)
	: EntityThrowable(pWorld, throwerIn)
{
	m_potionItem = usinItem;
}

ItemStackPtr EntityPotion::getPotionItem()
{
	return m_potionItem;
}

int EntityPotion::getPotionDamage()
{
	if (m_potionItem)
		return m_potionItem->getItemDamage();
	return 0xFFFFFFFF;
}

void EntityPotion::onImpact(const RayTraceResult& result)
{
	ItemPotion* item = nullptr;
	if (!world->m_isClient)
	{
		if (m_potionItem && dynamic_cast<ItemPotion*>(m_potionItem->getItem())) {
			item = dynamic_cast<ItemPotion*>(m_potionItem->getItem());
		}

		if (item && item->isSplash()) {
			PotionEffectArr effectArr;
			item->getEffects(effectArr);
			if (!effectArr.empty()) {
				applySplash(result, effectArr);
			}
		}
	}

	int x = int(Math::Round(position.x));
	int y = int(Math::Round(position.y));
	int z = int(Math::Round(position.z));
	world->playAuxSFX(2002, BlockPos(x, y, z), getPotionDamage());
	setDead();
}

void BLOCKMAN::EntityPotion::applySplash(const RayTraceResult& result, const PotionEffectArr & effectArr)
{
	Box aabb = getBoundingBox().expland(4.0f, 2.0f, 4.0f);
	EntityArr entities = world->getEntitiesWithinAABB(ENTITY_CLASS_LIVING_BASE, aabb);
	for (auto e : entities) {
		EntityLivingBase* entity = dynamic_cast<EntityLivingBase*>(e);
		if (!entity) {
			continue;
		}

		float distSqr = getDistanceSqToEntity(entity);
		if (distSqr >= 16.0) {
			continue;
		}

		double distImpact = 1.0 - Math::Sqrt(distSqr) / 4.0;
		if (entity == result.entityHit) {
			distImpact = 1.0;
		}

		for (size_t i = 0; i < effectArr.size(); ++i) {
			if (Potion::potionTypes[effectArr[i]->getPotionID()]->isInstant()) {
				Potion::potionTypes[effectArr[i]->getPotionID()]->affectEntity(this, getThrower(), entity, effectArr[i]->getAmplifier(), distImpact);
			}
			else {
				int duration = (int)(distImpact * (double)effectArr[i]->getDuration() + 0.5);
				if (duration > 20) {
					PotionEffect* effect = LordNew PotionEffect(effectArr[i]->getPotionID(), duration,
						effectArr[i]->getAmplifier(), effectArr[i]->getIsAmbient());

					entity->addPotionEffect(effect);
				}
			}
		}
	}
}

