#include "EntityCreature.h"

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
#include "Memory/LordMemory.h"

namespace BLOCKMAN
{

	AttributeModifier* EntityCreature::s_FieldSpeedModifier = NULL;// = LordNew AttributeModifier(field_110179_h, "Fleeing speed bonus", 2.0D, 2)).func_111168_a(false);

	EntityCreature::EntityCreature(World* pWorld)
		: EntityLiving(pWorld)
		, pathToEntity(NULL)
		, entityToAttack(NULL)
		, hasAttacked(false)
		, fleeingTick(0)
		, homePosition(BlockPos::ZERO)
		, maximumHomeDistance(-1.f)
		, m_hasTasks(false)
	{
		m_aiMoveTo = LordNew AIMoveToRestriction(this, 1.f);
		m_prevState = CREATURE_AS_MOVE;
		m_curState = CREATURE_AS_IDLE;
		m_prevBaseAction = CREATURE_AS_MOVE;
		m_baseAction = CREATURE_AS_IDLE;
		m_prevUpperAction = CREATURE_AS_MOVE;
		m_upperAction = CREATURE_AS_IDLE;
	}

	EntityCreature::EntityCreature(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName)
		: EntityLiving(pWorld)
		, pathToEntity(NULL)
		, entityToAttack(NULL)
		, hasAttacked(false)
		, fleeingTick(0)
		, homePosition(BlockPos::ZERO)
		, maximumHomeDistance(-1.f)
		, m_hasTasks(false)
		, m_MonsterId(monsterId)
		, m_teamId(0)
	{
		setPosition(pos);
		setActorName(actorName);
		rotationYaw = yaw;
		rotationPitch = 0.0f;
		m_prevState = CREATURE_AS_MOVE;
		m_curState = CREATURE_AS_IDLE;
		m_prevBaseAction = CREATURE_AS_MOVE;
		m_baseAction = CREATURE_AS_IDLE;
		m_prevUpperAction = CREATURE_AS_MOVE;
		m_upperAction = CREATURE_AS_IDLE;
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
		m_isDynamicCreature = checkDynamicCreature();
		setFreeSkillCd();
	}

	EntityCreature::~EntityCreature()
	{
		LordSafeDelete(m_aiMoveTo);
		if (pathToEntity)
		{
			LordSafeDelete(pathToEntity);
			pathToEntity = NULL;
		}
	}

	void EntityCreature::updateEntityActionState()
	{
		if (!isDynamicCreature())
			return;
		int baseY = int(Math::Floor(boundingBox.vMin.y + 0.5f));
		bool inWater = isInWater();
		bool inLava = handleLavaMovement();
		rotationPitch = 0.0F;

		if (pathToEntity && rand->nextInt(100) != 0)
		{
			// worldObj.theProfiler.startSection("followpath");
			Vector3 toward = pathToEntity->getPosition(this);
			float range = (width * 2.0F);

			while (toward != Vector3::INVALID && toward.squareDistanceTo(Vector3(position.x, toward.y, position.z)) < range * range)
			{
				pathToEntity->incrementPathIndex();

				if (pathToEntity->isFinished())
				{
					toward = Vector3::INVALID;
					LordSafeDelete(pathToEntity);
					pathToEntity = NULL;
				}
				else
				{
					toward = pathToEntity->getPosition(this);
					break;
				}
			}

			isJumping = false;

			if (toward != Vector3::INVALID)
			{
				Vector3 dirVec = toward - Vector3(position.x, float(baseY), position.z);
				float yaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
				yaw = Math::WrapDegree(yaw - rotationYaw);
				//moveForward = getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getAttributeValue();
				moveForward = 1.0f;

				if (yaw > 30.0F)
					yaw = 30.0F;
				if (yaw < -30.0F)
					yaw = -30.0F;

				rotationYaw += yaw;

				if (hasAttacked && entityToAttack)
				{
					float dx = entityToAttack->position.x - position.x;
					float dz = entityToAttack->position.z - position.z;
					float tempYaw = rotationYaw;
					rotationYaw = Math::ATan2(dz, dx)  * Math::RAD2DEG - 90.0F;
					yaw = (tempYaw - rotationYaw + 90.0F) * Math::DEG2RAD;
					moveStrafing = -Math::Sin(yaw) * moveForward * 1.0F;
					moveForward = Math::Cos(yaw) * moveForward * 1.0F;
				}

				if (dirVec.y > 0.0f)
				{
					isJumping = true;
				}
			}

			if (entityToAttack)
			{
				faceEntity(entityToAttack, 30.0F, 30.0F);
			}

			if (isCollidedHorizontally && !hasPath())
			{
				//isJumping = true;
			}

			if (rand->nextFloat() < 0.8F && (inWater || inLava))
			{
				//isJumping = true;
			}
		}
		else
		{
			EntityLiving::updateEntityActionState();
			LordSafeDelete(pathToEntity);
			pathToEntity = NULL;
		}
	}

	EntityPlayer * EntityCreature::findTargetPlayer()
	{
		return world->getPlayerByPlatformUserId(m_userId);
	}

	void EntityCreature::updateWanderPath()
	{
		if (!isDynamicCreature())
			return;
		bool bChosen = false;
		BlockPos chosen(-1, -1, -1);
		float maxWeight = -99999.0F;

		for (int var6 = 0; var6 < 10; ++var6)
		{
			BlockPos pos;
			pos.x = int(Math::Floor(position.x + rand->nextInt(13) - 6.0f));
			//pos.y = int(Math::Floor(position.y + rand->nextInt(7) - 3.0f));
			pos.y = int(Math::Floor(position.y));
			pos.z = int(Math::Floor(position.z + rand->nextInt(13) - 6.0f));
			float pathWeidht = getBlockPathWeight(pos);

			if (pathWeidht > maxWeight)
			{
				maxWeight = pathWeidht;
				chosen = pos;
				bChosen = true;
			}
		}

		if (bChosen)
		{
			pathToEntity = world->getEntityPathToXYZ(this, chosen, 10.0F, true, false, false, true);
		}
	}

	void EntityCreature::getPathToEntityPlayer()
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

	void EntityCreature::getPathToHome()
	{
		if (pathToEntity != nullptr)
		{
			delete pathToEntity;
			pathToEntity = nullptr;
		}

		pathToEntity = world->getEntityPathToXYZ(this, homePosition, 256, true, false, false, true);
	}

	void EntityCreature::getPathToEntity(Entity* endEntity)
	{
		if (pathToEntity != NULL)
			return;

		if (endEntity)
		{
			pathToEntity = world->getPathEntityToEntity(this, endEntity, 256.0F, true, false, false, true);
		}
	}

	void EntityCreature::getPathToEntityAndDelOldPath(Entity * endEntity)
	{
		if (pathToEntity != nullptr)
		{
			delete pathToEntity;
			pathToEntity = nullptr;
		}

		if (endEntity)
		{
			pathToEntity = world->getPathEntityToEntity(this, endEntity, 256.0F, true, false, false, true);
		}
	}

	bool EntityCreature::getCanSpawnHere()
	{
		BlockPos pos;
		pos.x = int(Math::Floor(position.x));
		pos.y = int(Math::Floor(boundingBox.vMin.y));
		pos.z = int(Math::Floor(position.z));
		return EntityLiving::getCanSpawnHere() && getBlockPathWeight(pos) >= 0.0F;
	}

	void EntityCreature::onUpdate()
	{
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		EntityLiving::onUpdate();
		refreshAttackCdTime();
		refreshFreeSkillCdTime();
		refreshUpdateTargetCdTime();
		refreshUpdatePathCdTime();
		decrHurtTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
	}

	void EntityCreature::onLivingUpdate()
	{
		if (!isDynamicCreature())
			return;
		EntityLiving::onLivingUpdate();
	}

	bool EntityCreature::canBeCollidedWith()
	{
		auto pGameRule = GameRuleSetting::getGameRule();
		return pGameRule->isCreatureCollision;
	}

	void EntityCreature::updateLeashedState()
	{
		if (!isDynamicCreature())
			return;
		EntityLiving::updateLeashedState();
		if (getLeashed() && getLeashedToEntity() && getLeashedToEntity()->world == world)
		{
			Entity* pLeashed = getLeashedToEntity();
			setHomePosAndDistance(BlockPos(int(pLeashed->position.x), int(pLeashed->position.y), int(pLeashed->position.z)), 5);
			float dis = getDistanceToEntity(pLeashed);

			if (!m_hasTasks)
			{
				m_tasks->addTask(2, m_aiMoveTo);
				getNavigator().setAvoidsWater(false);
				m_hasTasks = true;
			}

			onLeashDistance(dis);

			if (dis > 4.0F)
			{
				getNavigator().tryMoveToEntityLiving(pLeashed, 1.0f);
			}

			if (dis > 6.0F)
			{
				Vector3 dir = (pLeashed->position - position) / dis;
				Vector3 absDir;
				Vector3::Abs(absDir, dir);
				motion += dir * absDir * 0.4f;
			}

			if (dis > 10.0F)
			{
				clearLeashed(true, true);
			}
		}
		else if (!getLeashed() && m_hasTasks)
		{
			m_hasTasks = false;
			m_tasks->removeTask(m_aiMoveTo);
			getNavigator().setAvoidsWater(true);
			detachHome();
		}
	}

	bool EntityCreature::isWithinHomeDistanceFromPosition(const BlockPos& pos)
	{
		if (maximumHomeDistance == -1.f)
			return true;

		Vector3i disVec = homePosition - pos;
		return disVec.lenSqr() < maximumHomeDistance * maximumHomeDistance;
	}

	CRETURE_AI_TYPE  EntityCreature::getMonsterType()
	{
		CRETURE_AI_TYPE MonsterType = CRETURE_AI_TYPE_NONE;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			MonsterType = (CRETURE_AI_TYPE)pSetting->monsterType;
		}
		return MonsterType;
	}

	void EntityCreature::setMonsterType(CRETURE_AI_TYPE type)
	{
		m_creatureEntityType = type;
		m_isDynamicCreature = checkDynamicCreature();
	}

	void EntityCreature::setActorName(const String& actorName)
	{
		m_actorName = actorName;
		ActorSize* pSize = ActorSizeSetting::getActorSize(m_actorName);
		setSize(pSize->width, pSize->length, pSize->height);
		setPosition(position);
	}

	String EntityCreature::getMonsterBtTree()
	{
		String MonsterLoadBtTree = "";
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			MonsterLoadBtTree = pSetting->monsterBtTree;
		}
		return MonsterLoadBtTree;
	}

	bool EntityCreature::isBase()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_BASE;
	}

	bool EntityCreature::isBoss()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_MONSTER_BOSS;
	}

	bool EntityCreature::isMonster()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_MONSTER;
	}

	float EntityCreature::getAttackDistance()
	{
		float nAttackDistance = 16.0f;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			nAttackDistance = pSetting->attackDistance;
		}
		return nAttackDistance;
	}

	float EntityCreature::getSkillDistance()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return 0.0f;
		const SkillEffect* pSkill = SkillSetting::getSkillEffect(pSetting->skillId);
		if (!pSkill)
			return 0.0f;
		return Math::Sqrt(float(pSkill->width * pSkill->height));
	}

	void EntityCreature::setDead()
	{
		Entity::setDead();
		//TODO
	}

	bool EntityCreature::changeCreatureAction(CreatureActionState nActionState)
	{
		if (getCreatureAction() == CREATURE_AS_DEATH)
			return false;

		if (nActionState == CREATURE_AS_ATTACK)
			m_curState = CREATURE_AS_IDLE;

		if (m_curState != nActionState ||
			nActionState == CREATURE_AS_BE_PLAYER_ATTACK ||
			nActionState == CREATURE_AS_BE_OTHER_ATTACK ||
			nActionState == CREATURE_AS_DEATH)
		{
			setActionState(nActionState);
			if (getCreatureAction() == CREATURE_AS_DEATH)
			{
				setDead();
				if (world->m_isClient)
					setDelayTicksToFree(50);
				else
					setDelayTicksToFree(2);
			}
			return true;
		}
		return false;
	}

	void EntityCreature::setCurrentTargetEntityId(int EntityId, bool revengingTarget)
	{
		if (EntityId >= 0) {
			m_targetEntityId = EntityId;
			m_revengingTarget = revengingTarget;
		}
	}

	bool EntityCreature::entityInAttackRange(Entity* entity)
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

	bool EntityCreature::isCanAttackEntityBySkill(Entity * entity)
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return false;
		const SkillEffect* pSkill = SkillSetting::getSkillEffect(pSetting->skillId);
		if (!pSkill)
			return false;
		float minX = position.x - pSkill->width;
		float maxX = position.x + pSkill->width;
		float minZ = position.z - pSkill->height;
		float maxZ = position.z + pSkill->height;
		return entity->position.x >= minX && entity->position.x <= maxX && entity->position.z >= minZ && entity->position.z <= maxZ;
	}

	void EntityCreature::setSpeedAdditionLevel(int level)
	{
		if (level > 1000)
			level = 1000;
		float speedAddition = 0.2f * level / 1000;
		setSpeedAddition(speedAddition);
	}

	void EntityCreature::setSmallAttackCd()
	{
		m_attackCdTime = 1000 * m_minPlayerDistance / 8;
	}

	void EntityCreature::setAttackCd()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			m_attackCdTime = pSetting->attackCd * 1000;
		}
	}

	float EntityCreature::getAttackCd()
	{
		return m_attackCdTime;
	}

	void EntityCreature::refreshAttackCdTime()
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

	void EntityCreature::setFreeSkillCd()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			m_freeSkillCdTime = pSetting->skillCd * 1000;
		}
	}

	float EntityCreature::getFreeSkillCd()
	{
		return m_freeSkillCdTime;
	}

	void EntityCreature::refreshFreeSkillCdTime()
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

	void EntityCreature::setUpdateTargetCd()
	{
		m_updateTargetCdTime = 1000; // milliseconds
	}

	float EntityCreature::getUpdateTargetCd()
	{
		return m_updateTargetCdTime;
	}

	void EntityCreature::refreshUpdateTargetCdTime()
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

	void EntityCreature::setUpdatePathCd(int pathTargetId)
	{
		m_updatePathCdTime = 1000; // milliseconds
		m_pathTargetId = pathTargetId;
	}

	float EntityCreature::getUpdatePathCd(int pathTargetId)
	{
		if (m_pathTargetId != pathTargetId) {
			return 0;
		}

		return m_updatePathCdTime;
	}

	void EntityCreature::refreshUpdatePathCdTime()
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

	int EntityCreature::getAttackCount()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			return pSetting->attackCount;
		}
		return 1;
	}

	void EntityCreature::attackPerformance(EntityLivingBase* targeter)
	{

	}

	CRETURE_AI_ATTACK_TYPE EntityCreature::getCreatureAttackType()
	{
		CRETURE_AI_ATTACK_TYPE attackType = CRETURE_AI_ATTACK_TYPE_MELEE;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			attackType = (CRETURE_AI_ATTACK_TYPE)pSetting->attackType;
		}
		return attackType;
	}

	bool EntityCreature::findCloestPlayerTargetInPatrolDist()
	{
		if (!isDynamicCreature())
			return false;

		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting || !pSetting->isAutoAttack)
			return false;

		EntityPlayer* pPlayer = world->getClosestPlayerToEntity(this, pSetting->patrolDistance);
		if (!pPlayer)
			return false;

		if (pPlayer->isDead || pPlayer->isLogout())
		{
			if (pPlayer->entityId == getCurrentTargetEntityId())
			{
				setCurrentTargetEntityId(0, false);
				changeCreatureAction(CreatureActionState::CREATURE_AS_MOVE);
				return false;
			}
		}
		else
		{
			setCurrentTargetEntityId(pPlayer->entityId, false);
			return true;
		}
		return false;
	}

	bool EntityCreature::checkDynamicCreature()
	{
		CRETURE_AI_TYPE MonsterType = CRETURE_AI_TYPE_NONE;
		if (!world->m_isClient)
		{
			MonsterType = getMonsterType();
			m_creatureEntityType = MonsterType;
		}
		else
		{
			MonsterType = m_creatureEntityType;
		}
		return MonsterType != CRETURE_AI_TYPE_BASE && MonsterType != CRETURE_AI_TYPE_TOWER && MonsterType != CRETURE_AI_TYPE_NONE;
	}

	EntityArr  EntityCreature::getClosestPlayers()
	{
		float attackDistance = 8.0f;
		int attackCount = 1;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			attackDistance = pSetting->attackDistance;
			attackCount = pSetting->attackCount;
		}

		Vector3 & pos = position;

		float min_dis_sqr = attackDistance * attackDistance;
		float min_sqr = 65536.0f;
		EntityArr entityes;

		EntityPlayers& players = world->getPlayers();

		for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
		{
			EntityPlayer* pPlayer = it->second;
			if (!pPlayer)
				continue;

			if (pPlayer->getTeamId() == getTeamId())
				continue;

			if (!pPlayer->capabilities.disableDamage && !pPlayer->capabilities.isWatchMode && pPlayer->isEntityAlive())
			{
				float dis_sqr = pPlayer->getDistanceSq(pos);
				if (dis_sqr < min_dis_sqr)
				{
					entityes.push_back(pPlayer);
				}
				if (dis_sqr < min_sqr)
				{
					min_sqr = dis_sqr;
				}
			}
		}

		m_minPlayerDistance = Math::Sqrt(min_sqr);

		while (entityes.size() > size_t(attackCount))
		{
			float max_sqr = 0.0f;
			auto max_sqr_iterator = entityes.begin();
			bool hasIterator = false;
			for (auto it = entityes.begin(); it != entityes.end(); ++it)
			{
				float dis_sqr = (*it)->getDistanceSq(pos);
				if (dis_sqr > max_sqr)
				{
					max_sqr = dis_sqr;
					max_sqr_iterator = it;
					hasIterator = true;
				}
			}
			if (hasIterator)
				entityes.erase(max_sqr_iterator);
		}

		return entityes;
	}

} 
