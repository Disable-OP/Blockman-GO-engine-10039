#include "EntitySessionNpc.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Setting/ActorSizeSetting.h"

namespace BLOCKMAN
{

	EntitySessionNpc::EntitySessionNpc(World* pWorld) : Entity(pWorld) {
		preventEntitySpawning = true;
	}

	EntitySessionNpc::EntitySessionNpc(World* pWorld, const Vector3& pos) : EntitySessionNpc(pWorld) 
	{ 
		setPosition(pos); 
	}

	EntitySessionNpc::~EntitySessionNpc()
	{
		m_timeLeftMap.clear();
		m_nameLangMap.clear();
		m_actorNameMap.clear();
		m_actorBodyMap.clear();
		m_actorBodyIdMap.clear();
		m_effectNameMap.clear();
		m_sessionContentMap.clear();
		m_actorActionMap.clear();
		m_canCollidedMap.clear();
	}

	void EntitySessionNpc::onUpdate()
	{
		Entity::onUpdate();

		if (canFall())
		{
			moveEntity(motion);
		}
		
		float slipperiness = 0.91F;

		if (onGround)
		{
			slipperiness = 0.546F;
			int ix = int(Math::Floor(position.x));
			int iy = int(Math::Floor(boundingBox.vMin.y)) - 1;
			int iz = int(Math::Floor(position.z));
			int blockID = world->getBlockId(BlockPos(ix, iy, iz));
			if (blockID > 0)
			{
				slipperiness = BlockManager::sBlocks[blockID]->slipperiness() * 0.91F;
			}
		}

		if (position.y > -128.f)
		{
			motion.y -= 0.08f;
		}
		else
		{
			motion.y = 0.0f;
		}

		motion.y *= 0.98f;
		motion.x *= slipperiness;
		motion.z *= slipperiness;

		i32 dt = (i32)(world->getFrameDeltaTime() * 1000);
		
		if (world->m_isClient)
		{
			m_timeLeft -= dt;
		}
		else
		{
			for (auto& item : m_timeLeftMap)
			{
				i32 time = item.second - dt;
				time = time > 0 ? time : 0;
				item.second = time;
			}
			m_frame++;
			if (m_frame % (20 * 60 * 3) == 0 && m_isPerson)
			{
				removeInexistencePlayerData();
			}
		}
	}

	void EntitySessionNpc::setActorName(const String& actorName)
	{
		if (m_actorName != actorName)
		{
			m_actorName = actorName;
			m_isActorChange = true;
		}
		ActorSize* pSize = ActorSizeSetting::getActorSize(m_actorName);
		setSize(pSize->width, pSize->length, pSize->height);
		setPosition(position);
	}

	void EntitySessionNpc::setActorBody(String body)
	{
		if (m_actorBody != body)
		{
			m_actorBody = body;
			m_isActorChange = true;
		}
	}

	void EntitySessionNpc::setActorBodyId(String bodyId)
	{
		if (m_actorBodyId != bodyId)
		{
			m_actorBodyId = bodyId;
			m_isActorChange = true;
		}
	}

	void EntitySessionNpc::setActorAction(String action)
	{
		if (m_actorAction != action)
		{
			m_actorAction = action;
			m_isActorChange = true;
		}
	}

	void EntitySessionNpc::setTimeLeftByUser(ui64 userId, i32 timeLeft)
	{
		m_timeLeftMap[userId] = timeLeft;
		if (userId == 0)
		{
			m_timeLeft = timeLeft;
		}
	}

	void EntitySessionNpc::setNameLangByUser(ui64 userId, String nameLang)
	{
		m_nameLangMap[userId] = nameLang;
		if (userId == 0)
		{
			m_nameLang = nameLang;
		}
	}

	void EntitySessionNpc::setActorNameByUser(ui64 userId, String actorName)
	{
		if (userId == 0)
		{
			setActorName(actorName);
		}
		m_actorNameMap[userId] = actorName;
	}

	void EntitySessionNpc::setActorBodyByUser(ui64 userId, String actorBody)
	{
		if (userId == 0)
		{
			m_actorBody = actorBody;
		}
		m_actorBodyMap[userId] = actorBody;
	}

	void EntitySessionNpc::setActorBodyIdByUser(ui64 userId, String actorBodyId)
	{
		if (userId == 0)
		{
			m_actorBodyId = actorBodyId;
		}
		m_actorBodyIdMap[userId] = actorBodyId;
	}

	void EntitySessionNpc::setEffectNameByUser(ui64 userId, String effectName)
	{
		if (userId == 0)
		{
			m_effectName = effectName;
		}
		m_effectNameMap[userId] = effectName;
	}

	void EntitySessionNpc::setSessionContentByUser(ui64 userId, String content)
	{
		if (userId == 0)
		{
			m_sessionContent = content;
		}
		m_sessionContentMap[userId] = content;
	}

	void EntitySessionNpc::setActorActionByUser(ui64 userId, String actorAction)
	{
		if (userId == 0)
		{
			m_actorAction = actorAction;
		}
		m_actorActionMap[userId] = actorAction;
	}

	void EntitySessionNpc::setCanCollidedByUser(ui64 userId, bool isCanCollided)
	{
		if (userId == 0)
		{
			m_canCollided = isCanCollided;
		}
		m_canCollidedMap[userId] = isCanCollided;
	}

	i32 EntitySessionNpc::getTimeLeftByUser(ui64 userId)
	{
		auto item = m_timeLeftMap.find(userId);
		if (item != m_timeLeftMap.end())
		{
			return item->second;
		}
		return m_timeLeft;
	}

	String EntitySessionNpc::getNameLangByUser(ui64 userId)
	{
		auto item = m_nameLangMap.find(userId);
		if (item != m_nameLangMap.end())
		{
			return item->second;
		}
		return m_nameLang;
	}

	String EntitySessionNpc::getActorNameByUser(ui64 userId)
	{
		auto item = m_actorNameMap.find(userId);
		if (item != m_actorNameMap.end())
		{
			return item->second;
		}
		return m_actorName;
	}

	String EntitySessionNpc::getActorBodyByUser(ui64 userId)
	{
		auto item = m_actorBodyMap.find(userId);
		if (item != m_actorBodyMap.end())
		{
			return item->second;
		}
		return m_actorBody;
	}

	String EntitySessionNpc::getActorBodyIdByUser(ui64 userId)
	{
		auto item = m_actorBodyIdMap.find(userId);
		if (item != m_actorBodyIdMap.end())
		{
			return item->second;
		}
		return m_actorBodyId;
	}

	String EntitySessionNpc::getActorActionByUser(ui64 userId)
	{
		auto item = m_actorActionMap.find(userId);
		if (item != m_actorActionMap.end())
		{
			return item->second;
		}
		return m_actorAction;
	}

	String EntitySessionNpc::getEffectNameByUser(ui64 userId)
	{
		auto item = m_effectNameMap.find(userId);
		if (item != m_effectNameMap.end())
		{
			return item->second;
		}
		return m_effectName;
	}

	String EntitySessionNpc::getSessionContentByUser(ui64 userId)
	{
		auto item = m_sessionContentMap.find(userId);
		if (item != m_sessionContentMap.end())
		{
			return item->second;
		}
		return m_sessionContent;
	}

	bool EntitySessionNpc::isCanCollidedByUser(ui64 userId)
	{
		auto item = m_canCollidedMap.find(userId);
		if (item != m_canCollidedMap.end())
		{
			return item->second;
		}
		return m_canCollided;
	}

	void EntitySessionNpc::removeInexistencePlayerData()
	{
		vector<i64>::type userList = vector<i64>::type();

		for (auto& item  : m_sessionContentMap)
		{
			if (item.first != 0 && !world->getPlayerByPlatformUserId(item.first))
			{
				userList.push_back(item.first);
			}
		}

		for (i64 userId : userList)
		{
			if (m_timeLeftMap.find(userId) != m_timeLeftMap.end())
			{
				m_timeLeftMap.erase(m_timeLeftMap.find(userId));
			}

			if (m_nameLangMap.find(userId) != m_nameLangMap.end())
			{
				m_nameLangMap.erase(m_nameLangMap.find(userId));
			}

			if (m_actorNameMap.find(userId) != m_actorNameMap.end())
			{
				m_actorNameMap.erase(m_actorNameMap.find(userId));
			}

			if (m_actorBodyMap.find(userId) != m_actorBodyMap.end())
			{
				m_actorBodyMap.erase(m_actorBodyMap.find(userId));
			}

			if (m_actorBodyIdMap.find(userId) != m_actorBodyIdMap.end())
			{
				m_actorBodyIdMap.erase(m_actorBodyIdMap.find(userId));
			}

			if (m_effectNameMap.find(userId) != m_effectNameMap.end())
			{
				m_effectNameMap.erase(m_effectNameMap.find(userId));
			}

			if (m_sessionContentMap.find(userId) != m_sessionContentMap.end())
			{
				m_sessionContentMap.erase(m_sessionContentMap.find(userId));
			}

			if (m_actorActionMap.find(userId) != m_actorActionMap.end())
			{
				m_actorActionMap.erase(m_actorActionMap.find(userId));
			}

			if (m_canCollidedMap.find(userId) != m_canCollidedMap.end())
			{
				m_canCollidedMap.erase(m_canCollidedMap.find(userId));
			}
		}
	}

}