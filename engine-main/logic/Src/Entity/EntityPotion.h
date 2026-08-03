#ifndef ENTITY_POTION_H
#define ENTITY_POTION_H

#include "EntityThrowable.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{
	class World;
	class EntityLivingBase;
	
	class EntityPotion : public EntityThrowable
	{
		RTTI_DECLARE(EntityPotion);
	public:
		EntityPotion(World* pWorld);
		EntityPotion(World* pWorld, EntityLivingBase* throwerIn, ItemStackPtr usinItem);

		ItemStackPtr getPotionItem();
		int getPotionDamage();

	protected:
		virtual void onImpact(const RayTraceResult& result) override;
		virtual float getGravityVelocity() override { return 0.05F; }


	private:
		void applySplash(const RayTraceResult& result, const PotionEffectArr& effectArr);
	private:
		ItemStackPtr m_potionItem = nullptr;
	};

}
#endif
