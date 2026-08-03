#ifndef __ENTITY_ACTOR_NPC_HEADER__
#define __ENTITY_ACTOR_NPC_HEADER__
#include "Entity.h"
#include "World/World.h"
namespace BLOCKMAN
{
	class EntityActorNpc : public Entity 
	{
		RTTI_DECLARE(EntityActorNpc);
	private:
		String m_actorName = "";
		String m_headName = "";
		String m_skillName = "";
		String m_content = "";
		String m_HaloEffectName = "";
		bool canObstruct = true;
		bool m_canCollided = true;

	public:
		EntityActorNpc(World* pWorld) : Entity(pWorld)
		{
			preventEntitySpawning = true;
		}
		EntityActorNpc(World* pWorld, const Vector3& pos) : EntityActorNpc(pWorld)
		{
			setPosition(pos);
		}
		~EntityActorNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool canBeCollidedWith() { return m_canCollided; }
		bool canBePushed() { return true; }

		void setActorName(const String& actorName);

		String getActorName() const
		{
			return m_actorName;
		}

		void setHeadName(const String& headName)
		{
			m_headName = headName;
		}

		String getHeadName() const
		{
			return m_headName;
		}

		void setSkillName(const String& skillName)
		{
			m_skillName = skillName;
		}

		String getSkillName()
		{
			return m_skillName;
		}

		void setHaloEffectName(const String& haloEffectName)
		{
			m_HaloEffectName = haloEffectName;
		}

		String getHaloEffectName()
		{
			return m_HaloEffectName;
		}

		bool isShowContent()
		{
			return m_content.length() > size_t(0);
		}

		void setContent(const String& content)
		{
			m_content = content;
		}

		String getContent()
		{
			return m_content;
		}

		bool isImmuneToExplosions() { return true; }

		bool isCanObstruct() { return canObstruct; }

		void setCanObstruct(bool canObstruct) { this->canObstruct = canObstruct; }

		void setCanCollided(bool canCollided) { m_canCollided = canCollided; }
	};
}
#endif
