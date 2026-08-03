#ifndef __ENTITY_RANK_NPC_HEADER__
#define __ENTITY_RANK_NPC_HEADER__

#include "Entity.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class EntityRankNpc : public Entity
	{
		RTTI_DECLARE(EntityRankNpc);

	private:
		String m_name;
		String m_nameLang = "default.rank.name";
		String m_rankInfo = "";

	public:
		EntityRankNpc(World* pWorld) : Entity(pWorld)
		{
			preventEntitySpawning = true;
		}
		EntityRankNpc(World* pWorld, const Vector3& pos) : EntityRankNpc(pWorld)
		{
			setPosition(pos);
		}

		~EntityRankNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		bool canBeCollidedWith() { return true; }
		void setNameLang(const String& nameLang)
		{
			m_nameLang = nameLang;
		}

		String getNameLang() const
		{
			return m_nameLang;
		}

		String getName() const
		{
			return m_name;
		}

		void setName(const String& name)
		{
			m_name = name;
		}

		String getRankInfo()
		{
			return m_rankInfo;
		}

		void setRankInfo(const String& info)
		{
			m_rankInfo = info;
		}

		void onUpdate() override;

		bool isImmuneToExplosions() { return true; }
	};
}
#endif
