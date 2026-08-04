#include "EntityRankNpc.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"

namespace BLOCKMAN
{
	EntityRankNpc::~EntityRankNpc()
	{
	}

	void EntityRankNpc::onUpdate()
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
	}
}