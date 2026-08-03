#ifndef __ENTITY_SKILL_THROWABLE_HEADER__
#define __ENTITY_SKILL_THROWABLE_HEADER__

#include "Entity.h"
#include "EntityThrowable.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"

namespace BLOCKMAN
{
	class EntitySkillThrowable : public EntityThrowable
	{
		RTTI_DECLARE(EntitySkillThrowable);
	private:
		Vector3 m_startPos;
		float m_releaseDistance = 0.0f;
		bool m_parabola = false;
		int m_throwerId = 0;
		int m_skillId = 0;
		int m_skillItemId = 0;
		float m_moveSpeed = 0.0f;
		float m_dropSpeed = 0.0f;
		float m_gravity = 0.03f;
		String m_atlasName = "";
		String m_itemIconName = "";
		String m_effectName = "";

		SimpleEffect* m_bombEffect = NULL;

	public:
		EntitySkillThrowable(World* pWorld);
		EntitySkillThrowable(World* pWorld, const Vector3& pos);
		EntitySkillThrowable(World* pWorld, EntityLivingBase* pShooting);

		~EntitySkillThrowable();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {};
		void writeEntityToNBT(NBTTagCompound* pNBT) override {};
		void onUpdate() override;
		float getReleaseDistance() { return m_releaseDistance; }
		Vector3 getStartPos() { return m_startPos; }
		int getThrowerId() { return m_throwerId; }

		void setItemSkillId(int skillItemId) { m_skillItemId = skillItemId; }
		int getItemSkillId() {return m_skillItemId;}
		void setReleaseDistance(float releaseDistance) { m_releaseDistance = releaseDistance; }

		void setMoveSpeed(float moveSpeed);
		float getMoveSpeed() { return m_moveSpeed; }

		void setDropSpeed(float dropSpeed);
		float getDropSpeed() { return m_dropSpeed; }

		void setAtlasName(String atlasName) { m_atlasName = atlasName; }
		String getAtlasName() { return m_atlasName; }

		void setItemIconName(String itemIconName) { m_itemIconName = itemIconName; }
		String getItemIconName() { return m_itemIconName; }

		void setIsParabola(bool parabola) { m_parabola = parabola; }
		bool isParabola() { return m_parabola; }

		void setSkillId(int skillId) { m_skillId = skillId; }
		int getSkillId() { return m_skillId; }

		void setEffectName(String effectName);
		String getEffectName() { return m_effectName; }

	protected:
		void onImpact(const RayTraceResult& result) override;
		float getGravityVelocity() { return m_gravity; }

	private:
		void onEffectUpdate();
	};
}
#endif
