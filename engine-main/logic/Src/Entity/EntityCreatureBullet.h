#ifndef __ENTITY_CREATURE_BULLET_HEADER__
#define __ENTITY_CREATURE_BULLET_HEADER__

#include "Entity.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"

namespace BLOCKMAN
{
	enum CreatureBulletType
	{
		Default = 0, 
		FireBomb,
		TowerBomb
	};

	class EntityCreatureBullet : public Entity
	{
		RTTI_DECLARE(EntityCreatureBullet);
	private:
		int throwerId = 0;
		int targeterId = 0;

		int currUpdateTime = 0;
		int lastUpdateTime = 0;

		CreatureBulletType type;

		SimpleEffect* bombEffect = NULL;

	public:
		EntityCreatureBullet(World* pWorld)
			: Entity(pWorld)
		{
			type = CreatureBulletType::Default;
		}
		EntityCreatureBullet(World* pWorld, CreatureBulletType type)
			: Entity(pWorld)
		{
			this->type = type;
		}
		EntityCreatureBullet(World* pWorld, EntityLivingBase* thrower, EntityLivingBase* targeter, CreatureBulletType type);
		EntityCreatureBullet(World* pWorld, EntityLivingBase* thrower, EntityLivingBase* targeter)
			: EntityCreatureBullet(pWorld, thrower, targeter, CreatureBulletType::Default)
		{
		}
		~EntityCreatureBullet();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}

		void onUpdate() override;
		void onEffectUpdate();

		int getThrowerId() { return throwerId; }
		int getTargeterId() { return targeterId; }
		CreatureBulletType getType() { return type; }

	private:
		String getBombEffectName();
		void initBombEffect();

	};
}
#endif