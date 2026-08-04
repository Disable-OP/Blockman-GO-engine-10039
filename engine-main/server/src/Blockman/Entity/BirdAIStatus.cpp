#include "Object/Root.h"
#include "Blockman/Entity/BirdAIStatus.h"
#include "Blockman/Entity/EntityBirdAI.h"
#include "Entity/PathFinder.h"
#include "Entity/EntityPlayer.h"
#include "Util/CommonEvents.h"
#include "Common.h"
#include "Network/CommonPacketSender.h"
using namespace LORD;
namespace BLOCKMAN
{
	bool BirdAIStatusFollow::needEnter()
	{
		return super::needEnter();
	}

	bool BirdAIStatusFollow::needLeave()
	{
		return super::needLeave();
	}

	void BirdAIStatusFollow::enter(BirdAIType preType)
	{
		super::enter(preType);
		m_masterYaw[0] = m_birdAi->findMaster()->rotationYaw;
		m_masterPos[0] = m_birdAi->findMaster()->position;
		m_lastForceFollowTo = Vector3::INVALID;
		m_lastEnterIdleTime = 0;
		m_state = emFllowIdl;

		if (m_birdAi->hasLeaveMaster())
		{
			m_state = emFllowPosWhileLeaveMaster;
			m_lastForceFollowTo = m_birdAi->findPotralPosNearMaster();
			m_birdAi->forceMoveTo(m_lastForceFollowTo);
		}
	}

	void BirdAIStatusFollow::updateMasterMoveInfo()
	{
		m_masterYaw[1] = m_birdAi->findMaster()->rotationYaw;
		m_masterPos[1] = m_birdAi->findMaster()->position;
		m_lastForceFollowTo = Vector3::INVALID;
	}

	bool BirdAIStatusFollow::isMasterPosChange()
	{
		return m_masterPos[0] != m_masterPos[1];
	}
	
	bool BirdAIStatusFollow::isMasterYawChange()
	{
		return m_masterYaw[0] != m_masterYaw[1];
	}

	bool BirdAIStatusFollow::tryFollowPos()
	{
		// todo optimization

		if (isMasterPosChange())
		{
			if (m_state != emFllowPos)
			{
				m_birdAi->forceMoveTo(Vector3::INVALID);
				m_lastForceFollowTo = Vector3::INVALID;
			}

			Vector3 moveDis = m_masterPos[1] - m_masterPos[0];
			m_lastForceFollowTo = m_masterPos[1];
			auto ralitivePos = m_birdAi->getRalitivePathToMaster();
			m_lastForceFollowTo.x += ralitivePos.x;
			m_lastForceFollowTo.z += ralitivePos.z;
			m_lastForceFollowTo.y += m_birdAi->getFlyHigh();

			m_birdAi->forceMoveTo(m_lastForceFollowTo);
			m_masterPos[0] = m_masterPos[1];

			m_birdAi->forceRotation(FLT_MAX);
			m_masterYaw[0] = m_masterYaw[1];
			
			m_state = emFllowPos;
			return true;
		}
		else
		{
			if (m_state == emFllowPos)
			{
				if (!m_birdAi->getForceMoveTo().isInvalid())
				{
					return true;
				}
				else
				{
					m_lastForceFollowTo = Vector3::INVALID;
				}
			}
		}
		
		return false;
		
	}

	bool BirdAIStatusFollow::tryFollowYaw()
	{
		if (isMasterYawChange())
		{
			float yawDis = m_masterYaw[1] - m_masterYaw[0];
			m_birdAi->forceRotation(m_birdAi->rotationYaw + yawDis);

			m_masterYaw[0] = m_masterYaw[1];
			m_state = emFllowYaw;
			return true;
		}
		else
		{
			if (m_state == emFllowYaw)
			{
				if (m_birdAi->getForceYaw() != FLT_MAX)
					return true;
			}
		}

		return false;
	}

	bool BirdAIStatusFollow::tryFollowIdle()
	{
		if (m_birdAi->getForceMoveTo().isInvalid())
		{
			if (m_state != emFllowIdl)
			{
				m_lastEnterIdleTime = 0;
				m_state = emFllowIdl;
			}

			if (m_lastEnterIdleTime == 0)
			{
				m_birdAi->changeBirdAction(BIRD_AS_IDLE);
				m_lastEnterIdleTime = Root::Instance()->getCurrentTime();
			}
			else if (Root::Instance()->getCurrentTime() - m_lastEnterIdleTime > 3000)
			{
				m_birdAi->patrolReport();

				Vector3 toPos = m_birdAi->findPotralPosNearMaster();
				m_birdAi->forceMoveTo(toPos);
				m_lastEnterIdleTime = 0;
				m_state = emFllowIdl;
			}
		}

		return true;
	}

	bool BirdAIStatusFollow::tryFollowPosWhileLeaveMaster()
	{
		if (m_state != emFllowPosWhileLeaveMaster)
		{
			return false;
		}
		else if (!m_birdAi->getForceMoveTo().isInvalid())
		{
			if (m_masterPos[1].distanceTo(m_masterPos[0]) > 1.0)
			{
				m_masterPos[0] = m_masterPos[1];
				m_lastForceFollowTo = m_birdAi->findPotralPosNearMaster();
				m_birdAi->forceMoveTo(m_lastForceFollowTo);
			}
		}
		else
		{
			m_masterPos[0] = m_masterPos[1];
			m_lastForceFollowTo = Vector3::INVALID;
			m_state = emFllowIdl;
			return false;
		}

		return true;
	}

	void BirdAIStatusFollow::update()
	{
		updateMasterMoveInfo();
		if (!tryFollowPosWhileLeaveMaster())
		{
			if (!tryFollowPos())
			{
				tryFollowIdle();
			}
		}
	}


	bool BirdAIStatusGather::needEnter()
	{
		if (Root::Instance()->getServerFrameCount() % m_birdAi->getLongTimeUpdateGroups() != m_birdAi->getLongTimeUpdateIndex())
			return false;
		else
			return m_birdAi->isCanCollect();
	}

	bool BirdAIStatusGather::needLeave()
	{
		return m_bhasOver;
	}

	void BirdAIStatusGather::enter(BirdAIType preType)
	{
		super::enter(preType);
		m_birdAi->setPathToEntity(nullptr);
		m_birdAi->forceMoveTo(Vector3::INVALID);

		m_lastEnterGatherTime = 0;
		m_bInMoveToNewPos = false;
		m_bhasOver = false;
		m_lastGotherPosition = Vector3i::INVALID;

		if (!tryFindAndMoveToFruitPos())
			m_bhasOver = true;
	}

	void BirdAIStatusGather::leave()
	{
		m_birdAi->freeGatherPos(m_lastGotherPosition.getFloor());
		m_birdAi->setPathToEntity(nullptr);
		m_birdAi->forceMoveTo(Vector3::INVALID);
		m_lastEnterGatherTime = 0;
		m_bInMoveToNewPos = false;
		m_bhasOver = false;
	}

	bool BirdAIStatusGather::tryFindAndMoveToFruitPos()
	{
		Vector3 toPos = m_birdAi->findValidGatherPosition(m_lastGotherPosition);
		if (!toPos.isInvalid())
		{
			m_lastGotherPosition = toPos;
			toPos.y += 0.8;
			toPos.x += 0.5;
			toPos.z += 0.5;
			m_birdAi->forceMoveTo(toPos);
			m_lastEnterGatherTime = 0;
			return true;
		}

		return false;
	}

	void BirdAIStatusGather::update()
	{
		if (m_bhasOver)
			return;

		if (Root::Instance()->getServerFrameCount() % m_birdAi->getLongTimeUpdateGroups() == m_birdAi->getLongTimeUpdateIndex())
		{
			if (!m_birdAi->isCanCollect())
			{
				m_bhasOver = true;
				return;
			}
		}

		if (!m_birdAi->isMoveOver())
			return;


		if (m_lastEnterGatherTime == 0)
		{
			m_lastEnterGatherTime = Root::Instance()->getCurrentTime();
			m_birdAi->changeBirdAction(BIRD_AS_GATHER);
		}

		if (Root::Instance()->getCurrentTime() - m_lastEnterGatherTime > m_birdAi->getCollectCD())
		{
			auto gatherPos = m_birdAi->getPosition().getFloor();
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->onBirdSimulatorGather(m_birdAi->m_userId,
				m_birdAi->m_birdId, gatherPos);
			if (!m_birdAi->isCanCollect())
				m_bhasOver = true;
			else if (!tryFindAndMoveToFruitPos())
				m_bhasOver = true;
		}

	}


	bool BirdAIStatusTranslate::needEnter()
	{
		if (Root::Instance()->getServerFrameCount() % m_birdAi->getLongTimeUpdateGroups() != m_birdAi->getLongTimeUpdateIndex())
			return false;
		else
			return m_birdAi->hasReciveTranslateCMD();
	}

	bool BirdAIStatusTranslate::needLeave()
	{
		return m_state == emOver;
	}

	void BirdAIStatusTranslate::enter(BirdAIType preType)
	{
		Vector3 toMasterPos = m_birdAi->findTranslateNearMasterPos();
		LordAssert(!toMasterPos.isInvalid());

		m_birdAi->forceMoveTo(toMasterPos);
		m_birdAi->setNeedRegMovePath(true);
		m_birdAi->cleanRegMovePath();
		m_state = emMoveToMaster;
		m_enterLastStateTime = Root::Instance()->getCurrentTime();

	}

	void BirdAIStatusTranslate::leave()
	{
		m_birdAi->resetReciveTranslateCMDFlag();
	}

	void BirdAIStatusTranslate::update()
	{
		if (m_state == emMoveToMaster)
		{
			if (m_birdAi->isMoveOver())
			{
				m_state = emMoveToHome;
				m_enterLastStateTime = Root::Instance()->getCurrentTime();
				m_birdAi->forceMoveTo(m_birdAi->getBirdHousePosition());
			}
		}
		else if (m_state == emMoveToHome)
		{
			if (m_birdAi->isMoveOver())
			{
				m_state = emInTranslate;
				m_birdAi->beginTranslate();
				m_birdAi->changeBirdAction(BIRD_AS_CONVERT);
				m_enterLastStateTime = Root::Instance()->getCurrentTime();
			}
		}
		else if (m_state == emInTranslate)
		{
			if (Root::Instance()->getCurrentTime() - m_enterLastStateTime > m_birdAi->getConvertCD())
			{
				m_state = emOver;
				m_birdAi->finishTranslate();
			}
			else if (m_birdAi->hasReciveInteruptTranslateCMD())
			{
				m_state = emOver;
				m_birdAi->interruptTranslate();
			}
			else if (!m_birdAi->hasReciveTranslateCMD())
			{
				m_state = emOver;
			}
		}
		
	}

	bool BirdAIStatusAttack::needEnter()
	{
		if (Root::Instance()->getServerFrameCount() % m_birdAi->getLongTimeUpdateGroups() != m_birdAi->getLongTimeUpdateIndex())
			return false;
		else
			return m_birdAi->findMonster(m_currentEntityId) != 0;
	}

	bool BirdAIStatusAttack::needLeave()
	{
		return m_battackOver;
	}

	void BirdAIStatusAttack::enter(BirdAIType preType)
	{
		m_battackOver = false;
		m_currentEntityId = 0;

		m_currentEntityId = m_birdAi->findMonster(m_currentEntityId);
	}

	void BirdAIStatusAttack::leave()
	{
		m_birdAi->forceMoveTo(Vector3::INVALID);
	}

	void BirdAIStatusAttack::update()
	{
		if (Root::Instance()->getServerFrameCount() % m_birdAi->getLongTimeUpdateGroups() == m_birdAi->getLongTimeUpdateIndex())
				m_currentEntityId = m_birdAi->findMonster(m_currentEntityId);

		if (m_currentEntityId == 0)
		{
			m_battackOver = true;
			return;
		}

		if (!moveToAttacker())
		{
			m_battackOver = true;
			return;
		}

		if (m_birdAi->isMoveOver())
		{
			if (m_lastAttackTime == 0
				|| Root::Instance()->getCurrentTime() - m_lastAttackTime > (int)(m_birdAi->getAttackCD()))
			{
				m_birdAi->attackPerformance(m_currentEntityId);
				m_lastAttackTime = Root::Instance()->getCurrentTime();
			}
		}
	}

	bool BirdAIStatusAttack::moveToAttacker()
	{
		Vector3 pos = m_birdAi->getMonsterPosition(m_currentEntityId);
		if (pos.isInvalid())
		{
			return false;
		}

		if (pos.distanceTo(m_birdAi->getPosition()) < 2)
			return true;

		pos.y += 1;
		pos.x += 0.5;
		pos.z += 0.5;
		m_birdAi->forceMoveTo(pos);
		return true;
	}
}
