#pragma once
#include "Entity.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class EntityActorCannon : public Entity
	{
		RTTI_DECLARE(EntityActorCannon);

	public:
		Vector3 m_beginPos;
		Vector3 m_fallOnPos;
		Vector3 m_launchPoint;
		Vector3 m_motion;
		int m_remainingFlyTicks = 0;

	protected:
		String m_actorName = "";
		String m_headName = "";
		String m_skillName = "";
		String m_content = "";
		String m_HaloEffectName = "";
		bool canObstruct = true;

	public:
		EntityActorCannon(World* pWorld, const Vector3& beginPos, const Vector3& fallOnPos);
		~EntityActorCannon();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool canBeCollidedWith() { return true; }

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
	};
}