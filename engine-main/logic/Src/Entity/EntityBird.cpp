#include "EntityBird.h"

#include "AI/AIMoveToRestriction.h"
#include "World/World.h"
#include "AI/AITask.h"
#include "Entity/EntityPlayer.h"
#include "Setting/LogicSetting.h"
#include "Setting/MonsterSetting.h"
#include "Setting/SkillSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Script/Event/LogicScriptEvents.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "Object/Root.h"
#include "World/WorldEffectManager.h"

namespace BLOCKMAN
{

	EntityBird::EntityBird(World* pWorld)
		: EntityLiving(pWorld)
		, pathToEntity(NULL)
		, entityToAttack(NULL)
		, hasAttacked(false)
		, fleeingTick(0)
		, homePosition(BlockPos::ZERO)
		, maximumHomeDistance(-1.f)
		, m_hasTasks(false)
	{
	}

	EntityBird::EntityBird(World* pWorld, i64 birdId, Vector3& pos, float yaw, String actorName)
		: EntityLiving(pWorld)
		, pathToEntity(NULL)
		, entityToAttack(NULL)
		, hasAttacked(false)
		, fleeingTick(0)
		, homePosition(BlockPos::ZERO)
		, maximumHomeDistance(-1.f)
		, m_hasTasks(false)
		, m_birdId(birdId)
	{
		setPosition(pos);
		setActorName(actorName);
		rotationYaw = yaw;
		rotationPitch = 0.0f;
		m_prevState = BIRD_AS_MOVE;
		m_curState = BIRD_AS_IDLE;
		m_prevBaseAction = BIRD_AS_MOVE;
		m_baseAction = BIRD_AS_IDLE;
		m_prevUpperAction = BIRD_AS_MOVE;
		m_upperAction = BIRD_AS_IDLE;
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
		setFreeSkillCd();
	}

	EntityBird::~EntityBird()
	{
		LordSafeDelete(m_aiMoveTo);
		setPathToEntity(nullptr);
		destroyConvertEffects();
	}


	void EntityBird::forceMoveTo(Vector3 toPos)
	{

		m_forcMovePos =  toPos;

		motion.x = 0.0;
		motion.y = 0.0;
		motion.z = 0.0;
	}

	void EntityBird::forceRotation(float yaw)
	{
		m_forceYaw = yaw;

		motion.x = 0.0;
		motion.y = 0.0;
		motion.z = 0.0;
	}


	void EntityBird::moveFlying(float strafe, float forward, float speed)
	{
		float updown = moveUpDown;
		float distance = strafe * strafe + forward * forward + updown * updown;
		float dt = world->getFrameDeltaTime();

		if (distance >= 1e-4f)
		{
			float flySpeed = getFlySpeed();
			distance = Math::Sqrt(distance);

			if (distance < 1.0F)
			{
				distance = 1.0F;
			}

			flySpeed = flySpeed / distance;

			strafe *= flySpeed;
			forward *= flySpeed;
			updown *= flySpeed;

			float f1 = Math::Sin(rotationYaw * Math::PI / 180.0F);
			float f2 = Math::Cos(rotationYaw * Math::PI / 180.0F);
			float dx = strafe * f2 - forward * f1;
			float dz = forward * f2 + strafe * f1;

			motion.x = dx;
			motion.z = dz;
			motion.y = updown;
		}
	}

	void EntityBird::updateEntityActionState()
	{
		int baseY = int(Math::Floor(boundingBox.vMin.y + 0.5f));

		moveStrafing = 0;
		moveUpDown = 0;
		moveForward = 0;

		if (m_forcMovePos != Vector3::INVALID)
		{
			Vector3 dirVec = m_forcMovePos - position;
			if (dirVec.x != 0 || dirVec.z != 0)
			{
				float yaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
				rotationYaw = limitAngle(rotationYaw, yaw, 10.0F);
			}

			if (Math::Abs(dirVec.y) < getFlySpeed()*1.5)
			{
				auto pos = position;
				pos.y = m_forcMovePos.y;
				setPosition(pos);
				motion.y = 0.0;
				moveUpDown = 0.0f;
			}
			else
			{
				moveUpDown = (dirVec.y > 0 ? 1.0f : -1.0f);
			}
			
			auto dirxzVec = dirVec;
			dirxzVec.y = 0.0f;
			if (dirVec.len() < getFlySpeed()*1.5)
			{
				auto pos = m_forcMovePos;
				pos.y = position.y;
				setPosition(pos);
				motion.x = 0.0;
				motion.z = 0.0;
				moveForward = 0.0;
			}
			else
			{
				moveForward = 1.0f;
			}

			if (m_forcMovePos == position)
			{
				motion.x = 0;
				motion.y = 0;
				motion.z = 0;
				m_forcMovePos = Vector3::INVALID;
			}

		}
		else if(m_forceYaw != FLT_MAX )
		{
			if (rotationYaw == m_forceYaw)
			{
				m_forceYaw = FLT_MAX;
			}
			else
			{
				rotationYaw = limitAngle(rotationYaw, m_forceYaw, 10.0F);
			}
		}
		else if (pathToEntity)
		{
			Vector3 toward = pathToEntity->getPosition(this);

			if (m_bNeedRegMovePath)
			{
				if (m_movePath.empty() || m_movePath.at(m_movePath.size() - 1) != toward.getFloor())
					m_movePath.push_back(toward.getFloor());
			}

			float range = (width / 2.0F);

			while (toward != Vector3::INVALID
				&& toward.squareDistanceTo(Vector3(position.x, baseY, position.z)) < range * range)
			{
				pathToEntity->incrementPathIndex();

				if (pathToEntity->isFinished())
				{
					toward = Vector3::INVALID;
					setPathToEntity(nullptr);
				}
				else
				{
					toward = pathToEntity->getPosition(this);

					motion.x = 0.0;
					motion.y = 0.0;
					motion.z = 0.0;
					break;
				}
			}

			if (toward != Vector3::INVALID)
			{
				Vector3 dirVec = toward - Vector3(position.x, float(baseY), position.z);

				float yaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
				rotationYaw = limitAngle(rotationYaw, yaw, 10.0F);

				float xzSqrt = Math::Sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z);
				float pitch = (float)(-(Math::ATan2(dirVec.y, xzSqrt) * Math::RAD2DEG));
				rotationPitch = limitAngle(rotationPitch, pitch, 10.0F);

				moveForward = 1.0f;
				moveUpDown = dirVec.y != 0.0f ? (dirVec.y > 0 ? 1.0f : -1.0f) : 0.0f;
			}

			if (entityToAttack)
			{
				faceEntity(entityToAttack, 30.0F, 30.0F);
			}
		}
	}
	void EntityBird::getPathToEntityPlayer()
	{
		if (pathToEntity != NULL)
		{
			return;
		}

		auto player = world->getFirstPlayerEntity();
		if (player)
		{
			pathToEntity = world->getPathEntityToEntity(this, player, 100.0F, true, false, false, true);
		}
	}

	void EntityBird::getPathToEntity(Entity* endEntity)
	{
		if (pathToEntity != NULL)
			return;

		if (endEntity)
		{
			pathToEntity = world->getPathEntityToEntity(this, endEntity, 256.0F, true, false, false, true);
		}
	}

	void EntityBird::setPathToEntity(PathEntity* path)
	{
		if (pathToEntity != nullptr)
		{
			LordSafeDelete(pathToEntity);
			pathToEntity = NULL;
		}

		pathToEntity = path;
	}

	bool EntityBird::getCanSpawnHere()
	{
		BlockPos pos;
		pos.x = int(Math::Floor(position.x));
		pos.y = int(Math::Floor(boundingBox.vMin.y));
		pos.z = int(Math::Floor(position.z));
		return EntityLiving::getCanSpawnHere() && getBlockPathWeight(pos) >= 0.0F;
	}

	void EntityBird::onUpdate()
	{
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		auto master = findMaster();
		if (master)
		{
			m_masterPosition = master->position;
		}

		EntityLiving::onUpdate();
		refreshAttackCdTime();
		refreshFreeSkillCdTime();
		refreshUpdateTargetCdTime();
		refreshUpdatePathCdTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
	}

	void EntityBird::onLivingUpdate()
	{
		EntityLiving::onLivingUpdate();
	}

	bool EntityBird::canBeCollidedWith()
	{
		return false;
	}

	bool EntityBird::isWithinHomeDistanceFromPosition(const BlockPos& pos)
	{
		if (maximumHomeDistance == -1.f)
			return true;

		Vector3i disVec = homePosition - pos;
		return disVec.lenSqr() < maximumHomeDistance * maximumHomeDistance;
	}

	void EntityBird::changeDress(const String & masterName, const String & slaveName)
	{
		if (masterName == "glasses")
		{
			m_dressGlasses = slaveName;
		}
		else if (masterName == "hat")
		{
			m_dressHat = slaveName;
		}
		else if (masterName == "beak")
		{
			m_dressBeak = slaveName;
		}
		else if (masterName == "wing")
		{
			m_dressWing = slaveName;
		}
		else if (masterName == "tail")
		{
			m_dressTail = slaveName;
		}
		else if (masterName == "effect")
		{
			m_dressEffect = slaveName;
		}
	}

	void EntityBird::setActorName(const String& actorName)
	{
		setSize(1, 1, 1);

		if (m_actorName != actorName)
		{
			m_isActorChange = true;
			m_actorName = actorName;
		}

	}

	void EntityBird::setActorBody(const String & actorBody)
	{
		if (m_actorBody != actorBody)
		{
			m_isActorChange = true;
			m_actorBody = actorBody;
		}
	}

	void EntityBird::setActorBodyId(const String & actorBodyId)
	{
		if (m_actorBodyId != actorBodyId)
		{
			m_isActorChange = true;
			m_actorBodyId = actorBodyId;
		}
	}

	void EntityBird::setDressGlasses(const String & dressGlasses)
	{
		if (m_dressGlasses != dressGlasses)
		{
			m_dressGlasses = dressGlasses;
			m_isActorChange = true;
		}
	}

	void EntityBird::setDressHat(const String & dressHat)
	{
		if (m_dressHat != dressHat)
		{
			m_dressHat = dressHat;
			m_isActorChange = true;
		}
	}

	void EntityBird::setDressBeak(const String & dressBeak)
	{
		if (m_dressBeak != dressBeak)
		{
			m_dressBeak = dressBeak;
			m_isActorChange = true;
		}
	}

	void EntityBird::setDressWing(const String & dressWing)
	{
		if (m_dressWing != dressWing)
		{
			m_dressWing = dressWing;
			m_isActorChange = true;
		}
	}

	void EntityBird::setDressTail(const String & dressTail)
	{
		if (m_dressTail != dressTail)
		{
			m_dressTail = dressTail;
			m_isActorChange = true;
		}
	}

	void EntityBird::setDressEffect(const String & dressEffect)
	{
		if (m_dressEffect != dressEffect)
		{
			m_dressEffect = dressEffect;
			m_isActorChange = true;
		}
	}

	float EntityBird::getAttackDistance()
	{
		return 5.f;
	}

	void EntityBird::setDead()
	{
		Entity::setDead();
	}

	bool EntityBird::changeBirdAction(BirdActionState nActionState)
	{
		if (getBirdAction() != nActionState)
		{
			setActionState(nActionState);
			return true;
		}
		else
			return false;
	}

	void EntityBird::setCurrentTargetEntityId(int EntityId, bool revengingTarget)
	{
		if (EntityId >= 0) {
			m_targetEntityId = EntityId;
			m_revengingTarget = revengingTarget;
		}
	}

	bool EntityBird::entityInAttackRange(Entity* entity)
	{
		bool bResult = false;
		if (entity)
		{
			float distance = this->getAttackDistance();
			float min_dis_sqr = distance * distance;
			float dis_sqr = this->getDistanceSq(entity->position);

			if (dis_sqr <= min_dis_sqr)
			{
				bResult = true;
			}
		}
		return bResult;
	}

	void EntityBird::setSpeedAdditionLevel(int level)
	{
		if (level > 1000)
			level = 1000;
		float speedAddition = 0.2f * level / 1000;
		setSpeedAddition(speedAddition);
	}

	void EntityBird::setSmallAttackCd()
	{
		m_attackCdTime = 1000 * m_minPlayerDistance / 8;
	}

	void EntityBird::setAttackCd()
	{
		m_attackCdTime = 10000.f;
	}

	float EntityBird::getAttackCd()
	{
		return m_attackCdTime;
	}

	void EntityBird::refreshAttackCdTime()
	{
		if (m_attackCdTime > 0)
		{
			m_attackCdTime = m_attackCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_attackCdTime = 0;
		}
	}

	void EntityBird::setFreeSkillCd()
	{
		m_freeSkillCdTime = 10 * 1000.f;
	}

	float EntityBird::getFreeSkillCd()
	{
		return m_freeSkillCdTime;
	}

	void EntityBird::refreshFreeSkillCdTime()
	{
		if (m_freeSkillCdTime > 0)
		{
			m_freeSkillCdTime = m_freeSkillCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_freeSkillCdTime = 0;
		}
	}

	void EntityBird::setUpdateTargetCd()
	{
		m_updateTargetCdTime = 1000; // milliseconds
	}

	float EntityBird::getUpdateTargetCd()
	{
		return m_updateTargetCdTime;
	}

	void EntityBird::refreshUpdateTargetCdTime()
	{
		if (m_updateTargetCdTime > 0)
		{
			m_updateTargetCdTime = m_updateTargetCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_updateTargetCdTime = 0;
		}
	}

	void EntityBird::setUpdatePathCd(int pathTargetId)
	{
		m_updatePathCdTime = 1000; // milliseconds
		m_pathTargetId = pathTargetId;
	}

	float EntityBird::getUpdatePathCd(int pathTargetId)
	{
		if (m_pathTargetId != pathTargetId) {
			return 0;
		}

		return m_updatePathCdTime;
	}

	void EntityBird::refreshUpdatePathCdTime()
	{
		if (m_updatePathCdTime > 0)
		{
			m_updatePathCdTime = m_updatePathCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_updatePathCdTime = 0;
		}
	}

	int EntityBird::getAttackCount()
	{
		return 1;
	}

	void EntityBird::attackPerformance(EntityLivingBase* targeter)
	{

	}

	int EntityBird::getBirdAttackType()
	{

		return 0;
	}

	EntityPlayer * EntityBird::findMaster()
	{
		return world->getPlayerByPlatformUserId(m_userId);
	}

	void EntityBird::destroyConvertEffects()
	{
		for (auto item : m_convertEffects)
		{
			item->mDuration = 0;
		}
		m_convertEffects.clear();
	}

	float EntityBird::limitAngle(float angle1, float angle2, float angleMax)
	{
		/*
		float angle = Math::WrapDegree(angle2 - angle1);

		if (angle > angleMax)
		{
			angle = angleMax;
		}

		if (angle < -angleMax)
		{
			angle = -angleMax;
		}
		*/

		angle1 = angle2;

		if (angle1 < 0.0F)
		{
			angle1 += 360.0F;
		}
		else if (angle1 > 360.0F)
		{
			angle1 -= 360.0F;
		}

		return angle1;
	}
}
