#include "EntityCreatureAI.h"
#include "Setting/MonsterSetting.h"
#include "Setting/SkillSetting.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Entity/EntityCreatureBullet.h"
#include "Script/GameServerEvents.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	EntityCreatureAI::EntityCreatureAI(World * pWorld, int monsterId, Vector3 & pos, float yaw, String actorName)
		: EntityCreature(pWorld, monsterId, pos, yaw, actorName)
	{
	}

	EntityCreatureAI::~EntityCreatureAI()
	{
		if (m_pAgentPlayer != NULL)
			m_pAgentPlayer->setEntity(nullptr);
	}

	void EntityCreatureAI::onUpdate()
	{
		EntityCreature::onUpdate();
		updateAgent();
	}

	void EntityCreatureAI::onLivingUpdate()
	{
		EntityCreature::onLivingUpdate();
	}

	bool EntityCreatureAI::attackEntityFrom(DamageSource * pSource, float amount)
	{
		if (pSource && pSource->getEntity())
		{
			EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(pSource->getEntity());
			player->attackCreatureEntity(this, (int)PLAYER_ATTACK_CREATURE_DAMAGE);
		}
		return false;
	}

	void EntityCreatureAI::selectBtTreeByMonsterId(int monsterId)
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(monsterId);
		if (pSetting)
		{
			if (pSetting->monsterBtTree.size() > 0)
				setCurrentAI((pSetting->monsterBtTree).c_str());

			if (pSetting->moveSpeed > 0)
			{
				setSpeedAdditionLevel(pSetting->moveSpeed);
			}

			m_monsterType = pSetting->monsterType;
		}
	}

	void EntityCreatureAI::setCurrentAI(const char* pBtTree)
	{
		m_sLoadBTTree = pBtTree;
		m_loadAgentMutex.lock();
		loadAgent();
		m_loadAgentMutex.unlock();
		setAIActive(true);
	}

	void EntityCreatureAI::setAIActive(bool bActive)
	{
		m_bIsAIActive = bActive;
		m_pAgentPlayer->SetActive(m_bIsAIActive);
	}

	void EntityCreatureAI::unloadAgent()
	{
		if (m_pAgentPlayer != NULL)
		{
			m_pAgentPlayer->setEntity(nullptr);
			behaviac::Agent::Destroy(m_pAgentPlayer);
		}
		m_pAgentPlayer = NULL;
	}

	bool EntityCreatureAI::loadAgent()
	{
		m_pAgentPlayer = behaviac::Agent::Create<BaseAgent>();
		m_pAgentPlayer->setEntity(this);
		bool bRet = m_pAgentPlayer->btload(m_sLoadBTTree.c_str());
		m_pAgentPlayer->btsetcurrent(m_sLoadBTTree.c_str());
		return bRet;
	}

	void EntityCreatureAI::updateAgent()
	{
		if (m_pAgentPlayer == NULL || !m_bIsAIActive || !m_pAgentPlayer->IsActive())
			return;

		if (m_btExecCnt % 3 == 0) {
			m_pAgentPlayer->btexec();
			
			if (m_btExecCnt == 300) {
				m_btExecCnt = 0;
			}
		}
		m_btExecCnt++;
		
	}

	bool EntityCreatureAI::changeCreatureAction(CreatureActionState nActionStatus)
	{
		bool isChange = EntityCreature::changeCreatureAction(nActionStatus);

		if (isChange && !isBase())
		{
			ServerNetwork::Instance()->getSender()->sendCreatureEntityAction(entityId, nActionStatus,(int)getCreatureAction(),(int)getPreCreatureAction());
		}

		return isChange;
	}

	void EntityCreatureAI::onAttackPlayer(EntityPlayer * player)
	{
		SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(player->hashCode(), hashCode());
	}

	void EntityCreatureAI::attackPerformance(EntityLivingBase* targeter)
	{
		if (getCreatureAction() == CREATURE_AS_DEATH)
			return;
		CRETURE_AI_ATTACK_TYPE attackType = EntityCreature::getCreatureAttackType();
		EntityCreatureBullet* creatureBullet = nullptr;
		switch (attackType)
		{
		case CRETURE_AI_ATTACK_TYPE_MELEE:
			switch (targeter->getClassID())
			{
			case ENTITY_CLASS_PLAYERMP:
				SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(targeter->hashCode(), this->hashCode());
				break;
			case ENTITY_CLASS_CREATUREAI:
				SCRIPT_EVENT::CreatureAttackCreatureEvent::invoke(targeter->hashCode(), this->hashCode());
				break;
			default:
				break;
			}
			break;
		case CRETURE_AI_ATTACK_TYPE_REMOTE:
			creatureBullet = LordNew EntityCreatureBullet(world, this, targeter);
			break;
		case CRETURE_AI_ATTACK_TYPE_TOWER:
			creatureBullet = LordNew EntityCreatureBullet(world, this, targeter, CreatureBulletType::TowerBomb);
			break;
		case CRETURE_AI_ATTACK_TYPE_GROUP:
			SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(targeter->hashCode(), this->hashCode());
			break;
		default:
			break;
		}
		if (creatureBullet)
			world->spawnEntityInWorld(creatureBullet);
	}

	void EntityCreatureAI::groupAttackPerformance()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return;
		sendSkillEffect(pSetting->skillId);
	}

	void EntityCreatureAI::sendSkillEffect(int effectId)
	{
		SkillEffect* pSkill = SkillSetting::getSkillEffect(effectId);
		if (!pSkill)
			return;
		ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, pSkill->name, pSkill->duration,
			pSkill->width, pSkill->height, pSkill->color, pSkill->density);
	}
}