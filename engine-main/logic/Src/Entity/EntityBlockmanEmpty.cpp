#include "EntityBlockmanEmpty.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "World/WorldProvider.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Setting/GameRuleSetting.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Object/Root.h"
#include "Common.h"

namespace BLOCKMAN
{
	EntityBlockmanEmpty::EntityBlockmanEmpty(World * pWorld) : EntityBlockman(pWorld) 
	{
		blockmanType = EntityBlockmanType::BLOCKMAN_EMPTY;
		isCanCollision = GameRuleSetting::getGameRule()->isBlockmanCollision;
		isCanLongHit = GameRuleSetting::getGameRule()->isBlockmanLongHit;
		longHitTimes = GameRuleSetting::getGameRule()->blockmanLongHitTimes;
		pushX = 0;
		pushZ = 0;
	}

	EntityBlockmanEmpty::EntityBlockmanEmpty(World * pWorld, const Vector3 & pos, float yaw, bool isRun)
		: EntityBlockman(pWorld, pos)
	{
		rotationYaw = yaw;
		blockmanType = EntityBlockmanType::BLOCKMAN_EMPTY;
		isCanCollision = GameRuleSetting::getGameRule()->isBlockmanCollision;
		isCanLongHit = GameRuleSetting::getGameRule()->isBlockmanLongHit;
		longHitTimes = GameRuleSetting::getGameRule()->blockmanLongHitTimes;
		setSpeedAddtion(0.05);
		if (isRun)
		{
			BlockPos bPos = BlockPos((int)pos.x, (int)pos.y, (int)pos.z);
			if (BlockRailBase::isRailBlock(pWorld->getBlockId(bPos.getPosX())))
			{
				pushX = 1;
				pushZ = 0;
			}
			if (BlockRailBase::isRailBlock(pWorld->getBlockId(bPos.getNegX())))
			{
				pushX = -1;
				pushZ = 0;
			}
			if (BlockRailBase::isRailBlock(pWorld->getBlockId(bPos.getNegZ())))
			{
				pushX = 0;
				pushZ = -1;
			}
			if (BlockRailBase::isRailBlock(pWorld->getBlockId(bPos.getPosZ())))
			{
				pushX = 0;
				pushZ = 1;
			}
			if (pushX != 0 || pushZ != 0)
			{
				changeDirection(yaw);
			}
		}
		else 
		{
			pushX = 0;
			pushZ = 0;
		}
	}

	EntityBlockmanEmpty::~EntityBlockmanEmpty()
	{
		xRecordRoute.clear();
		zRecordRoute.clear();
	}

	void EntityBlockmanEmpty::onUpdate()
	{
		onUpdateStart();
		onFrozenUpdate();
		onSyncBlockmanInfo();
		onRecordAllRoute();
		EntityBlockman::onUpdate();
		onUpdateEnd();
	}

	bool EntityBlockmanEmpty::processInitialInteract(EntityPlayer * par1EntityPlayer)
	{
		/*fuel += 3600;
		pushX = position.x - par1EntityPlayer->position.x;
		pushZ = position.z - par1EntityPlayer->position.z;*/
		return false;
	}

	void EntityBlockmanEmpty::onUpdateStart()
	{
		currUpdateTime = Root::Instance()->getCurrentTime();
		if (world->m_isClient)
		{
			return;
		}
		if (pushX != 0 && pushZ != 0)
		{
			return;
		}
		if (pushX == 0 && lastPushX != 0)
		{
			lastPushX = 0;
		}
		if (pushZ == 0 && lastPushZ != 0)
		{
			lastPushZ = 0;
		}
	}

	void EntityBlockmanEmpty::onUpdateEnd()
	{
		lastUpdateTime = currUpdateTime;
		if (world->m_isClient)
		{
			return;
		}
		if (position.distanceTo(lastScriptPos) >= 0.5f)
		{
			lastScriptPos = position;
			SCRIPT_EVENT::BlockmanEmptyMoveEvent::invoke(this->entityId, position);
		}
		if (lastPushX != 0 && lastPushZ != 0)
		{
			return;
		}
		bool isNeedSync = false;
		if (lastPushX == 0 && pushX != 0)
		{
			lastPushX = 1;
			isNeedSync = true;
		}
		if (lastPushZ == 0 && pushZ != 0)
		{
			lastPushZ = 1;
			isNeedSync = true;
		}
		if (isNeedSync)
		{
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->syncBlockmanMoveAttr(0, this);
		}
	}

	void EntityBlockmanEmpty::onFrozenUpdate()
	{
		float time = world->getFrameDeltaTime() * 1000;
		if (isFrozen())
		{
			frozenTime -= time;
			if (!isFrozen())
			{
				outLooksChanged = true;
			}
		}
	}

	void EntityBlockmanEmpty::onSyncBlockmanInfo()
	{
		if (world->m_isClient)
		{
			return;
		}
		if (isNeedSyncInfo)
		{
			isNeedSyncInfo = false;
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->syncEntityBlockman(0, this);
		}
		if (isNeedSyncMove)
		{
			isNeedSyncMove = false;
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->syncBlockmanMoveAttr(0, this);
		}
	}

	void EntityBlockmanEmpty::onRecordAllRoute()
	{
		if (!needRecordRoute)
		{
			return;
		}
		if (pushX == 0 && pushZ == 0)
		{
			return; // don't record idle
		}
		if (pushX != 0 && pushZ != 0)
		{
			return; // don't record turns
		}
		if (pushX == 0) // run on the z axis
		{
			onRecordRoute(0);
		}
		if (pushZ == 0) // run on the x axis
		{
			onRecordRoute(1);
		}
	}

	void EntityBlockmanEmpty::onRecordRoute(int type)
	{
		int key = (int)Math::Floor(type == 1 ? position.z : position.x);
		auto pair = type == 1 ? xRecordRoute.equal_range(key) : zRecordRoute.equal_range(key);
		float min = type == 1 ? position.x : position.z;
		float max = type == 1 ? position.x : position.z;
		float RecordMin = 0.0f;
		float RecordMax = 0.0f;
		for (auto iter = pair.first; iter != pair.second; ++iter)
		{
			float r_min = (*iter).second.x;
			float r_max = (*iter).second.y;
			if (RecordMin == 0.0f)
			{
				RecordMin = r_min;
			}
			else
			{
				if (r_min < RecordMin)
				{
					RecordMin = r_min;
				}
			}
			if (RecordMax == 0.0f)
			{
				RecordMax = r_max;
			}
			else
			{
				if (r_max > RecordMax)
				{
					RecordMax = r_max;
				}
			}
			if (r_min < min)
			{
				min = r_min;
			}
			if (r_max > max)
			{
				max = r_max;
			}
		}

		bool isNeedNewRoute = false;
		for (auto iter = pair.first; iter != pair.second; ++iter)
		{
			float r_min = (*iter).second.x;
			float r_max = (*iter).second.y;
			if (r_min == RecordMin)
			{
				if (min - RecordMin > -2.0f)
				{
					(*iter).second.x = min;
					float p_max = type == 1 ? position.x : position.z;
					if (r_max < p_max)
					{
						(*iter).second.y = p_max;
					}
				}
				else
				{
					isNeedNewRoute = true;
				}
			}
			if (r_max == RecordMax)
			{
				if (max - RecordMax < 2.0f)
				{
					(*iter).second.y = max;
					float p_min = type == 1 ? position.x : position.z;
					if (r_min > p_min)
					{
						(*iter).second.x = p_min;
					}
				}
				else
				{
					isNeedNewRoute = true;
				}
			}
		}

		if (RecordMin == 0.0f || RecordMax == 0.0f || isNeedNewRoute)
		{
			if (type == 1)
			{
				xRecordRoute.insert({ key, { position.x, position.x } });
			}
			else
			{
				zRecordRoute.insert({ key, { position.z, position.z } });
			}
		}
	}

	bool EntityBlockmanEmpty::ifWalkOnRecordRoute()
	{
		if (!needRecordRoute || speedAddition == 0)
		{
			return false;
		}
		if (pushX == 0 && pushZ == 0)
		{
			return false;
		}
		if (pushX != 0 && pushZ != 0)
		{
			return false;
		}
		int type = 0;
		if (pushZ == 0)
		{
			type = 1;
		}
		int key = (int)Math::Floor(type == 1 ? position.z : position.x);
		auto pair = type == 1 ? xRecordRoute.equal_range(key) : zRecordRoute.equal_range(key);
		bool isOnRecord = false;
		for (auto iter = pair.first; iter != pair.second; ++iter)
		{
			float r_min = (*iter).second.x;
			float r_max = (*iter).second.y;
			float p_pos = type == 1 ? position.x : position.z;
			if (p_pos >= r_min && p_pos <= r_max)
			{
				isOnRecord = true;
				break;
			}
		}
		return isOnRecord;
	}

	bool EntityBlockmanEmpty::isMoving()
	{
		if (pushX == 0 && pushZ == 0)
			return false;
		return getMoveSpeed() > 0;
	}

	void EntityBlockmanEmpty::updateOnTrack(int posX, int posY, int posZ, int blockId, int meta)
	{
		EntityBlockman::updateOnTrack(posX, posY, posZ, blockId, meta);

		float var10 = pushX * pushX + pushZ * pushZ;
		if (var10 > 0.00005 && motion.x * motion.x + motion.z * motion.z > 0.00005)
		{
			var10 = std::sqrt(var10);
			pushX /= var10;
			pushZ /= var10;

			if (pushX * motion.x + pushZ * motion.z < 0.0f)
			{
				pushX = 0.0f;
				pushZ = 0.0f;
			}
			else
			{
				pushX = motion.x;
				pushZ = motion.z;
			}
		}
	}
	void EntityBlockmanEmpty::applyDrag()
	{
		float var1 = pushX * pushX + pushZ * pushZ;

		if (var1 > 0.00005)
		{
			var1 = std::sqrt(var1);
			pushX /= var1;
			pushZ /= var1;
			float var3 = getMoveSpeed() * ((currUpdateTime - lastUpdateTime) / 20.0f);
			motion.x *= 0.800000011920929f;
			motion.y *= 0.0f;
			motion.z *= 0.800000011920929f;
			motion.x += pushX * var3;
			motion.z += pushZ * var3;
		}
		else
		{
			motion.x *= 0.9800000190734863f;
			motion.y *= 0.0f;
			motion.z *= 0.9800000190734863f;
		}
		EntityBlockman::applyDrag();
	}

	bool EntityBlockmanEmpty::checkBlcok(int blockId)
	{
		if (!world->m_isClient)
		{
			if (blockId == BlockManager::railReceive->getBlockID())
			{
				//TODO
				setDead();
				return true;
			}
		}
		return false;
	}

	void EntityBlockmanEmpty::reverseDirection()
	{
		pushX = -pushX;
		pushZ = -pushZ;
		motion = -motion;
		rotationYaw += 180;
		isNeedSyncMove = true;
	}

	void EntityBlockmanEmpty::changeDirection(float angle)
	{
		isNeedSyncMove = true;
		rotationYaw = angle;
		while (angle < 0)
		{
			angle += 360;
		}
		while (angle > 360)
		{
			angle -= 360;
		}
		if (angle > 45 && angle <= 135)
		{
			pushX = -1;
			pushZ = 0;
			return;
		}
		if (angle > 135 && angle <= 225)
		{
			pushX = 0;
			pushZ = -1;
			return;
		}
		if (angle > 225 && angle <= 315)
		{
			pushX = 1;
			pushZ = 0;
			return;
		}
		pushX = 0;
		pushZ = 1;
	}

	void EntityBlockmanEmpty::setNeedRecordRoute(bool needRecord)
	{
		needRecordRoute = needRecord;
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setCurSpeed(float speed)
	{
		curSpeed = speed;
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setSpeedAddtion(float addtion)
	{
		speedAddition = addtion;
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setNameLang(String nameLang)
	{
		EntityBlockman::setNameLang(nameLang);
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setActorName(String actorName)
	{
		EntityBlockman::setActorName(actorName);
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setActorBody(String body)
	{
		EntityBlockman::setActorBody(body);
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setActorBodyId(String bodyId)
	{
		EntityBlockman::setActorBodyId(bodyId);
		isNeedSyncInfo = true;
	}

	void EntityBlockmanEmpty::setCurrentMaxSpeed(float maxSpeed)
	{
		EntityBlockman::setCurrentMaxSpeed(maxSpeed);
		isNeedSyncInfo = true;
	}

	float EntityBlockmanEmpty::getMoveSpeed()
	{
		if (isFrozen())
			return 0.0f;
		bool isOnRecord = ifWalkOnRecordRoute();
		if (isOnRecord)
			return curSpeed + speedAddition;
		else
			return curSpeed;
	}

	void EntityBlockmanEmpty::setOnFrozen(float mills)
	{
		bool frozen = isFrozen();
		frozenTime = mills;
		if (isFrozen())
			outLooksChanged = outLooksChanged || !frozen;
		else
			outLooksChanged = outLooksChanged || frozen;
	}

}