#ifndef __ENTITY_BULLETIN_HEADER__
#define __ENTITY_BULLETIN_HEADER__

#include "EntitySessionNpc.h"
#include "World/World.h"

namespace BLOCKMAN {


	class EntityBulletin : public EntitySessionNpc
	{
		RTTI_DECLARE(EntityBulletin);
	protected:

		String m_awaitAnimate = "idle";
		i32 m_bulletinId = 0;

	private:
		void setNameLang(const String& nameLang) { m_nameLang = nameLang; }
		void setName(const String& name) { m_name = name; }
		

	public:
		EntityBulletin(World* pWorld);
		EntityBulletin(World* pWorld, const Vector3& pos);
		~EntityBulletin();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;

		void setBulletinId(i32 bulletinId);
		i32 getBulletinId() { return m_bulletinId; }
		
		bool isImmuneToExplosions() override{ return true; }
		bool canBeCollidedWith() { return true; }
	
	};
}
#endif
