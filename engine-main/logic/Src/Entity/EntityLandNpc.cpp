#include "EntityLandNpc.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{

	EntityLandNpc::EntityLandNpc(World* pWorld) : Entity(pWorld) {
		preventEntitySpawning = true;
	}

	EntityLandNpc::EntityLandNpc(World* pWorld, const Vector3& pos) : EntityLandNpc(pWorld) 
	{ 
		setPosition(pos); 
	}

	EntityLandNpc::~EntityLandNpc()
	{
	}

	void EntityLandNpc::onUpdate()
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

		if (timeLeft > 0 && status ==2 )
		{
			i32 dt = (i32)(world->getFrameDeltaTime() * 1000);
			timeLeft = (timeLeft - dt <= 0) ? 0 : (timeLeft - dt);
		}
	}

	void EntityLandNpc::setRecipe(const vector<RanchCommon>::type& items)
	{
		recipe.clear();
		for (auto item : items)
		{
			recipe.push_back(item);
		}
	}
	void EntityLandNpc::setReward(const vector<RanchCommon>::type& items)
	{
		reward.clear();
		for (auto item : items)
		{
			reward.push_back(item);
		}
	}

	void EntityLandNpc::setActorName(const String & actorName)
	{
		if (m_actorName != actorName)
		{
			m_actorName = actorName;
			m_isActorChange = true;
		}

	}

	void EntityLandNpc::setActorBody(String body)
	{
		if (m_actorBody != body)
		{
			m_actorBody = body;
			m_isActorChange = true;
		}
	}

	void EntityLandNpc::setActorBodyId(String bodyId)
	{
		if (m_actorBodyId != bodyId)
		{
			m_actorBodyId = bodyId;
			m_isActorChange = true;
		}
	}
}