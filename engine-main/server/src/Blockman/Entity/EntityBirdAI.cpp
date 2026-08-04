#include "EntityBirdAI.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Script/GameServerEvents.h"
#include "Script/Event/LogicScriptEvents.h"
#include "LuaRegister/Template/TableVisitor.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Blockman/Entity/BirdAIStatus.h"
#include "Block/Blocks.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Script/HostApi.h"

namespace BLOCKMAN
{
	std::set<LORD::Vector3i, EntityBirdAI::BlockPosCompare> EntityBirdAI::m_usedGotherPos;
	std::map<LORD::ui64, std::vector<LORD::Vector3i>> EntityBirdAI::m_validGatherPosSetForPlayer;
	LORD::ui32 EntityBirdAI::m_longTimeOPUpdateGroup[longTimeUpdateGroups] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	EntityBirdAI::EntityBirdAI(World * pWorld, std::shared_ptr<TableVisitor> birdMgr, Vector3i homePos,
		i64 birdId, Vector3 & pos, float yaw, String actorName)
		: EntityBird(pWorld, birdId, pos, yaw, actorName)
		, m_birdManagerInScript(birdMgr)
	{
		homePosition = homePos;
		setEntityInvulnerable(true);
		initAIStatus();
		dispathLongTimeUpdateGroup();
	}

	EntityBirdAI::~EntityBirdAI()
	{
		if (m_curAIIndex != -1)
			m_allAIStatus[m_curAIIndex]->leave();

		if (getBelongSameMasterBirdNum() == 0)
		{
			auto it = m_validGatherPosSetForPlayer.find(m_userId);
			if (it != m_validGatherPosSetForPlayer.end())
			{
				m_validGatherPosSetForPlayer.erase(it);
			}
		}

		m_longTimeOPUpdateGroup[m_longTimeUpdateIndex] -= 1;
	}

	void EntityBirdAI::dispathLongTimeUpdateGroup()
	{
		m_longTimeUpdateIndex = 0;
		int minMems = m_longTimeOPUpdateGroup[0];
		for (int i = 1; i < longTimeUpdateGroups; i++)
		{
			if (m_longTimeOPUpdateGroup[i] < minMems)
			{
				m_longTimeUpdateIndex = i;
				minMems = m_longTimeOPUpdateGroup[i];
			}
		}

		m_longTimeOPUpdateGroup[m_longTimeUpdateIndex] += 1;
	}

	void EntityBirdAI::onUpdate()
	{
		EntityBird::onUpdate();
	}

	void EntityBirdAI::onLivingUpdate()
	{
		updateAIStatus();

		//EntityBird::onLivingUpdate();

		updateEntityActionState();
		rotationYawHead = rotationYaw;

		moveStrafing *= 0.98F;
		moveForward *= 0.98F;
		moveEntityWithHeading(moveStrafing, moveForward);
	}

	bool EntityBirdAI::changeBirdAction(BirdActionState nActionStatus)
	{
		bool isChange = EntityBird::changeBirdAction(nActionStatus);

		if (isChange)
		{
			ServerNetwork::Instance()->getSender()->sendEntityAction(0, entityId, getClassID(), nActionStatus, (int)getBirdAction(), (int)getPreBirdAction());
		}

		return isChange;
	}

	void EntityBirdAI::attackPerformance(int id)
	{
		changeBirdAction(BIRD_AS_ATTACK);
		m_birdManagerInScript->callFunc("attackMonster",
			*m_birdManagerInScript, this->m_birdId, id);
	}

	void EntityBirdAI::patrolReport()
	{
		m_birdManagerInScript->callFunc("patrol",
			*m_birdManagerInScript, this->m_birdId);
	}

	Vector3i EntityBirdAI::getRalitivePathToMaster()
	{
		if (m_lastPotrolRelativePos == Vector3i::INVALID)
			findPotralPosNearMaster();

		return m_lastPotrolRelativePos;
	}

	Vector3 EntityBirdAI::findPotralPosNearMaster()
	{
		Vector3 vec = Vector3::ZERO;

		LordAssert(m_birdManagerInScript);

		m_birdManagerInScript->callFunc(vec, "selectPotralPositionForBirdById",
			*m_birdManagerInScript, this->m_birdId);

		m_lastPotrolRelativePos = vec.getFloor();

		auto masterPos = findMaster()->getPosition();
		vec.x += masterPos.x;
		vec.z += masterPos.z;
		vec.y =  masterPos.y + getFlyHigh();
		return vec;
	}

	void EntityBirdAI::tryMoveTo(const Vector3 &pos)
	{
		//auto newPathToEntity = world->getEntityPathToXYZ(this, pos.getFloor(), 20.0F, true, false, false, true);
		//setPathToEntity(newPathToEntity);
	}

	bool EntityBirdAI::tryTeleportTo(const Vector3 &pos)
	{

		if (pos.distanceTo(this->getPosition()) > 100)
		{
			this->setPathToEntity(nullptr);
			this->setPosition(pos);
			EntityTracker::Instance()->forceTeleport(this->entityId);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool EntityBirdAI::hasLeaveMaster()
	{
		return (getPosition().distanceTo(findMaster()->getPosition()) > getBirdAreaLength()+1
			&& getPosition().y - findMaster()->getPosition().y < getFlyHigh());
	}

	void EntityBirdAI::initAIStatus()
	{
		m_allAIStatus.resize(4);
		m_allAIStatus[(int)BirdAIType::emFollow] = std::make_shared<BirdAIStatusFollow>(this);
		m_allAIStatus[(int)BirdAIType::emGather] = std::make_shared<BirdAIStatusGather>(this);
		m_allAIStatus[(int)BirdAIType::emTranlate] = std::make_shared<BirdAIStatusTranslate>(this);
		m_allAIStatus[(int)BirdAIType::emAttack] = std::make_shared<BirdAIStatusAttack>(this);

		m_curAIIndex = -1;

	}

	void EntityBirdAI::updateAIStatus()
	{
		if (m_curAIIndex == -1)
		{
			m_curAIIndex = (int)BirdAIType::emFollow;
			m_allAIStatus[m_curAIIndex]->enter(BirdAIType::emFollow);
		}
		else
		{

			int endAskAIIndex = m_curAIIndex;
			if (m_allAIStatus[endAskAIIndex]->needLeave())
			{
				endAskAIIndex = -1;
				m_allAIStatus[m_curAIIndex]->leave();
			}

			for (int i = m_allAIStatus.size() - 1; i > endAskAIIndex; i--)
			{
				bool needEnter = m_allAIStatus[i]->needEnter();
				if (needEnter)
				{
					if(m_curAIIndex != -1)
						m_allAIStatus[m_curAIIndex]->leave();

					m_allAIStatus[i]->enter(BirdAIType(m_curAIIndex));
					m_curAIIndex = i;
					break;
				}
			}
		}

		m_allAIStatus[m_curAIIndex]->update();
	}

	bool EntityBirdAI::isCanCollect()
	{
		bool can = false;
		m_birdManagerInScript->callFunc(can, "isCanCollect",
			*m_birdManagerInScript);
		return can;
	}


	void EntityBirdAI::freeGatherPos(const Vector3i& pos)
	{
		auto it = m_usedGotherPos.find(pos);
		if (it != m_usedGotherPos.end())
			m_usedGotherPos.erase(it);
	}

	void EntityBirdAI::regGatherPos(const Vector3i & pos)
	{
		m_usedGotherPos.insert(pos);
	}

	bool EntityBirdAI::isGatherPosUsed(const Vector3i & pos)
	{
		if (m_usedGotherPos.find(pos) != m_usedGotherPos.end())
			return true;
		else
			return false;
	}

	bool EntityBirdAI::isValidGatherPosition(const Vector3i  &pos)
	{
		if(!BlockFruits::isFruits(world->getBlockId(pos)))
			return false;

		int fruitsNum = world->getBlockMeta(pos);
		if (fruitsNum <= 2 && fruitsNum != 0)
			return false;

		return true;
	}

	std::vector<LORD::Vector3i>& EntityBirdAI::collectGatherPosition()
	{
		Vector3 vec = findMaster()->getPosition();
		auto it = m_validGatherPosSetForPlayer.find(m_userId);
		if (it != m_validGatherPosSetForPlayer.end())
		{
			if (vec.getFloor() == it->second[0])
				return it->second;
		}
		else
		{
			m_validGatherPosSetForPlayer[m_userId] = std::vector<LORD::Vector3i>();
			it = m_validGatherPosSetForPlayer.find(m_userId);
		}

		it->second.clear();
		it->second.push_back(vec.getFloor());

		int gatherArea = this->getBirdAreaLength() + 3;

		Vector3i tryVec;
		int i = 0;
		for (; i < gatherArea; i++)
		{
			for (int dx = -i; dx <= i; dx++)
			{
				if (Math::Abs(dx) + i > gatherArea)
					continue;

				tryVec = vec.getFloor();
				tryVec.x += dx;
				tryVec.z += -i;
				if(isValidGatherPosition(tryVec))
					it->second.push_back(tryVec);
				

				tryVec = vec.getFloor();
				tryVec.x += dx;
				tryVec.z += i;

				if (isValidGatherPosition(tryVec))
					it->second.push_back(tryVec);
				
			}


			for (int dz = -i + 1; dz < i; dz++)
			{
				if (Math::Abs(dz) + i > gatherArea)
					continue;

				tryVec = vec.getFloor();
				tryVec.x += -i;
				tryVec.z += dz;

				if (isValidGatherPosition(tryVec))
					it->second.push_back(tryVec);
				

				tryVec = vec.getFloor();
				tryVec.x += -i;
				tryVec.z += dz;

				if (isValidGatherPosition(tryVec))
					it->second.push_back(tryVec);
				
			}
		}

		return it->second;
	}

	Vector3 EntityBirdAI::findValidGatherPosition(const Vector3 &curGotherPos)
	{
		if (!curGotherPos.isInvalid())
			freeGatherPos(curGotherPos.getFloor());

		auto validPosSet = collectGatherPosition();

		Vector3 rtVec = Vector3::INVALID;
		
		if (validPosSet.size() <= 1)
			return Vector3::INVALID;

		
		int beginFindPos = HostApi::random("GameBird_Gather_pos", 0, validPosSet.size()-1);
		int findReg = 0;
		while (findReg <= validPosSet.size())
		{
			if (beginFindPos == validPosSet.size())
				beginFindPos = 0;

			if (beginFindPos != 0)
			{
				if (!isGatherPosUsed(validPosSet[beginFindPos]))
				{
					regGatherPos(validPosSet[beginFindPos]);
					return validPosSet[beginFindPos];
				}
			}

			beginFindPos++;
			findReg++;
		}

		return Vector3::INVALID;
	}

	void EntityBirdAI::interruptTranslate()
	{
		m_birdManagerInScript->callFunc("interruptConvert",
			*m_birdManagerInScript, this->m_birdId);
	}

	void EntityBirdAI::beginTranslate()
	{
		m_birdManagerInScript->callFunc("convertFruit",
			*m_birdManagerInScript, this->m_birdId);
	}

	void EntityBirdAI::finishTranslate()
	{
		m_birdManagerInScript->callFunc("finishConvert",
			*m_birdManagerInScript, this->m_birdId);
	}

	Vector3 EntityBirdAI::getBirdHousePosition()
	{
		return Vector3(homePosition.x + 0.5f, homePosition.y + 0.3f, homePosition.z + 0.5f);
	}

	bool EntityBirdAI::hasReciveTranslateCMD()
	{
		int state = 0;
		m_birdManagerInScript->callFunc(state, "getConvertStateById",
			*m_birdManagerInScript, this->m_birdId);
		
		return state == 1;
	}

	bool EntityBirdAI::hasReciveInteruptTranslateCMD()
	{
		int state = 0;
		m_birdManagerInScript->callFunc(state, "getConvertStateById",
			*m_birdManagerInScript, this->m_birdId);

		return state == 2;
	}

	void EntityBirdAI::resetReciveTranslateCMDFlag()
	{

	}

	Vector3 EntityBirdAI::findTranslateNearMasterPos()
	{
		Vector3 vec = findMaster()->getPosition();
		vec.x += 1.0f;
		return vec;
	}
	
	int EntityBirdAI::findMonster(int curAttacker)
	{
		auto pos = findMaster()->getPosition();
		std::vector<int> validMoster;
		auto entitys = world->getLoadedEntityMap();
		for (auto entity : *entitys)
		{
			auto entityPos = entity.second->position;
			if (Math::Abs(pos.x - entityPos.x) + Math::Abs(pos.z - entityPos.z) <  attackRange)
			{
				EntityCreature* pCreature = dynamic_cast<EntityCreature*>(entity.second);
				if (pCreature)
				{
					if (pCreature->m_userId == m_userId)
						return entity.first;
					else
						validMoster.push_back(entity.first);
				}
			}
		}

		if (validMoster.size() > 0)
			return validMoster[0];
		else
			return 0;
	}

	Vector3 EntityBirdAI::getMonsterPosition(int id)
	{
		Vector3 pos = Vector3::INVALID;
		if (world->getEntity(id) != nullptr)
		{
			pos = world->getEntity(id)->position;
		}
		return pos;
	}

	void EntityBirdAI::forceMoveTo(const Vector3 & toPos)
	{
		EntityBird::forceMoveTo(toPos);
		if (!toPos.isInvalid())
		{
			changeBirdAction(BIRD_AS_MOVE);
		}
	}

	float EntityBirdAI::getFloatAttrFromLua(const char * attr)
	{
		float rtAttr = 0.0;
		m_birdManagerInScript->callFunc(rtAttr, "getFloatAttributeById",
			*m_birdManagerInScript, this->m_birdId, attr);
		return rtAttr;
	}

	int EntityBirdAI::getBelongSameMasterBirdNum()
	{
		int rt = 1;
		m_birdManagerInScript->callFunc(rt, "getBirdNumInWorld",
			*m_birdManagerInScript);
		return rt;
	}

	float EntityBirdAI::getFlyHigh()
	{
		static float flyHigh = 0.0;
		if(flyHigh == 0.0)
		   flyHigh = getFloatAttrFromLua(emFlyHigh); 
		return flyHigh;
	}
	
	float EntityBirdAI::getFlySpeedAddition()
	{
		if (m_lastGetSpeedAddition == FLT_MAX || Root::Instance()->getServerFrameCount() % (getLongTimeUpdateGroups()*30) == (getLongTimeUpdateIndex()*30))
			m_lastGetSpeedAddition = 0.2f * ((int)getFloatAttrFromLua(emFlySpeed) / 1000);

		return m_lastGetSpeedAddition;
	}

	float EntityBirdAI::getCollectCD()
	{
		if(m_lastGetCollectCD == FLT_MAX || 
			Root::Instance()->getServerFrameCount() % (getLongTimeUpdateGroups() * 30) == (getLongTimeUpdateIndex() * 30))
			m_lastGetCollectCD = getFloatAttrFromLua(emCollectCD);

		return m_lastGetCollectCD;
	}

	float EntityBirdAI::getConvertCD()
	{
		if (m_lastGetConvertCD == FLT_MAX ||
			Root::Instance()->getServerFrameCount() % (getLongTimeUpdateGroups() * 30) == (getLongTimeUpdateIndex() * 30))
			m_lastGetConvertCD = getFloatAttrFromLua(emConvertCD);

		return m_lastGetConvertCD;
	}

	float EntityBirdAI::getAttackCD()
	{
		if (m_lastGetAttackCD == FLT_MAX ||
			Root::Instance()->getServerFrameCount() % (getLongTimeUpdateGroups() * 30) == (getLongTimeUpdateIndex() * 30))
			m_lastGetAttackCD = getFloatAttrFromLua(emAttackCD);

		return m_lastGetAttackCD;
	}


	int EntityBirdAI::getBirdAreaLength()
	{
		int birdNum = getBelongSameMasterBirdNum();
		if (birdNum == m_lastBirdNum)
			return m_currentBirdArealen;

		int i = 1;
		while (((i*i + i) * 2) < birdNum) i++;

		m_currentBirdArealen = i;
		return m_currentBirdArealen;
	}
}