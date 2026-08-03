#include "EntityBuildNpc.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "DamageSource.h"
#include "Item/Item.h"
#include "Setting/BuildingSetting.h"
#include "Common.h"
#include "Network/CommonPacketSender.h"

namespace BLOCKMAN
{

	EntityBuildNpc::EntityBuildNpc(World* pWorld) : Entity(pWorld)
	{
		preventEntitySpawning = true;

	}

	EntityBuildNpc::EntityBuildNpc(World* pWorld, const Vector3& pos) : EntityBuildNpc(pWorld)
	{
		setPosition(pos);
	}

	EntityBuildNpc::~EntityBuildNpc()
	{
	}

	void EntityBuildNpc::onUpdate()
	{
		Entity::onUpdate();
		m_frame++;

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

		if (m_damage > 0)
		{
			m_damage -= 1.f;
		}

		switch (m_buildType) {
		case BuildType::BUILD_FARM:
			syncQueue();
			break;
		case BuildType::BUILD_PLANT:
			singleQueue();
			break;
		}

		if (m_frame % 100 == 0 && world->m_isClient &&  m_userId > 0)
		{
			if (( m_buildType == BuildType::BUILD_FARM || m_buildType == BuildType::BUILD_PLANT) &&  !isCanWorking())
			{
				GameCommon::Instance()->getCommonPacketSender()->buildNotification(m_userId, m_nameLang);
			}
		}
	}

	bool EntityBuildNpc::attackEntityFrom(DamageSource * source, float amount)
	{
		if (!world->m_isClient && !isDead)
		{
			m_damage += amount * 10.f;
			if (m_damage > 40.f)
			{
				setDead();
				if (m_itemId > 0)
				{
					ItemStackPtr itemStack = std::shared_ptr<ItemStack>();
					itemStack.reset(LordNew ItemStack(m_itemId, 1, 0));
					entityDropItem(itemStack, 0.0F);
				}
			}
		}
		return true;
	}

	void EntityBuildNpc::singleQueue()
	{
		bool isAwait = false;
		bool isWorking = false;
		bool working = false;

		for (auto & queue : m_unlockQueues)
		{
			if (queue.state == QueueState::QUEUE_WORKING && isCanWorking())
			{
				i32 dt = (i32)(world->getFrameDeltaTime() * 1000);
				queue.timeLeft = ((queue.timeLeft - dt) <= 0) ? 0 : (queue.timeLeft - dt);
				queue.state = queue.timeLeft == 0 ? QueueState::QUEUE_DONE : queue.state;
				isWorking = true;
				working = true;
			}

			if (queue.state == QueueState::QUEUE_AWAIT)
			{
				isWorking = true;
				isAwait = true;
			}
		}

		m_isWorking = isWorking;
		m_isAwait = (!working && isAwait);
	}

	void EntityBuildNpc::syncQueue()
	{
		bool isWorking = false;
		m_isAwait = false;
		for (auto& queue : m_unlockQueues)
		{
			if (queue.state == QueueState::QUEUE_WORKING)
			{
				i32 dt = (i32)(world->getFrameDeltaTime() * 1000);
				queue.timeLeft = queue.timeLeft - dt <= 0 ? 0 : (queue.timeLeft - dt);
				queue.state = queue.timeLeft == 0 ? QueueState::QUEUE_DONE : queue.state;
				isWorking = true;
			}

			if (queue.state == QueueState::QUEUE_AWAIT)
			{
				isWorking = true;
				m_isAwait = true;
			}
		}
		m_isWorking = isWorking;
	}

	void EntityBuildNpc::setActorName(const String & actorName)
	{
		if (m_actorName != actorName)
		{
			m_actorName = actorName;
			m_isActorChange = true;
		}
		
	}

	void EntityBuildNpc::setActorBody(String body)
	{
		if (m_actorBody != body)
		{
			m_actorBody = body;
			m_isActorChange = true;
		}
	}

	void EntityBuildNpc::setActorBodyId(String bodyId)
	{
		if (m_actorBodyId != bodyId)
		{
			m_actorBodyId = bodyId;
			m_isActorChange = true;
		}
	}

	void EntityBuildNpc::setActorId(i32 actorId)
	{
		if (m_actorId != actorId)
		{
			m_actorId = actorId;
			auto actor = BuildingSetting::getBuildingByActorId(m_actorId);
			if (actor)
			{
				m_itemId = actor->itemId;
				m_nameLang = actor->name;
				setActorBody(actor->actorBody);
				setActorName(actor->actorName);
				setActorBodyId(actor->actorBodyId);

				m_buildType = (BuildType)actor->type;
				m_awaitAnimate = actor->awaitAnimate;
				m_finishAnimate = actor->finishAnimate;
				m_manufactureAnimate = actor->manufactureAnimate;
				setSize(actor->length, actor->width, actor->height);
				setPosition(position);
				m_startPos = BlockPos((int)(position.x - getLength() / 2), (int)position.y, (int)(position.z - getWidth() / 2));
				m_endPos = BlockPos((int)(position.x + getLength() / 2), (int)(position.y + getHeight()), (int)(position.z + getWidth() / 2));
			}
			m_isActorChange = true;
		}
	}

	void EntityBuildNpc::setRecipes(vector<ProductRecipe>::type & recipes)
	{
		m_recipes.clear();
		for (auto item : recipes)
		{
			m_recipes.push_back(item);
		}
	}

	void EntityBuildNpc::setProducts(vector<RanchCommon>::type & products)
	{
		m_products.clear();
		for (auto item : products)
		{
			m_products.push_back(item);
		}
	}

	void EntityBuildNpc::setUnlockQueues(vector<ProductQueue>::type & queues)
	{
		m_unlockQueues.clear();
		bool isWorking = false;
		for (auto item : queues)
		{
			m_unlockQueues.push_back(item);
			if (item.state == QueueState::QUEUE_WORKING || item.state == QueueState::QUEUE_AWAIT)
			{
				isWorking = true;
			}
		}
		m_isWorking = isWorking;
	}

	i32 EntityBuildNpc::getProductNum()
	{
		int num = 0;
		for (auto item : m_products)
		{
			num += item.num;
		}
		return num;
	}

	bool EntityBuildNpc::isAwait()
	{
		return m_isAwait;
	}

	bool EntityBuildNpc::isWoring()
	{
		return m_isWorking;
	}

	bool EntityBuildNpc::isCanWorking(i32 blockId)
	{
		//return world->rangeCheckBlock(blockId, m_startPos, m_endPos);
		return true;
	}

	bool EntityBuildNpc::canMake()
	{
		for (auto& queue : m_unlockQueues)
		{
			if (queue.state == QueueState::QUEUE_LEISURE)
			{
				return true;
			}
		}
		return false;
	}

	bool EntityBuildNpc::canBuy()
	{
		return   m_unlockQueues.size() < m_maxQueueNum;
	}

	String EntityBuildNpc::getAnimate()
	{
		if (getProductNum() > 0)
		{
			return m_finishAnimate;
		}

		if (m_isWorking)
		{
			return m_manufactureAnimate;
		}

		return m_awaitAnimate;
	}

}