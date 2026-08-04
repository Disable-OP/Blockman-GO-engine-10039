#include "BlockRailLogic.h"
#include "BlockManager.h"
#include "Block.h"
#include "Blocks.h"
#include "BM_TypeDef.h"
#include "World/World.h"

namespace BLOCKMAN
{

BlockRailLogic::BlockRailLogic(BlockRailBase* pRail, World* pWorld, const BlockPos& pos)
{
	theRail = pRail;
	logicWorld = pWorld;
	railPos = pos;
	int blockID = pWorld->getBlockId(pos);
	int meta = pWorld->getBlockMeta(pos);

	if (theRail->m_isPowered)
	{
		isStraightRail = true;
		meta &= -9;
	}
	else
	{
		isStraightRail = false;
	}

	setBasicRail(meta);
}

BlockRailLogic::~BlockRailLogic()
{
	for (size_t i = 0; i < deleteArr.size(); ++i)
	{
		LordDelete deleteArr[i];
	}
}

void BlockRailLogic::setBasicRail(int meta)
{
	railChunkPosition.clear();

	if (meta == 0)
	{
		railChunkPosition.push_back(railPos.getNegZ());
		railChunkPosition.push_back(railPos.getPosZ());
	}
	else if (meta == 1)
	{
		railChunkPosition.push_back(railPos.getNegX());
		railChunkPosition.push_back(railPos.getPosX());
	}
	else if (meta == 2)
	{
		railChunkPosition.push_back(railPos.getNegX());
		railChunkPosition.push_back(railPos.getUE());
	}
	else if (meta == 3)
	{
		railChunkPosition.push_back(railPos.getUW());
		railChunkPosition.push_back(railPos.getPosX());
	}
	else if (meta == 4)
	{
		railChunkPosition.push_back(railPos.getUN());
		railChunkPosition.push_back(railPos.getPosZ());
	}
	else if (meta == 5)
	{
		railChunkPosition.push_back(railPos.getNegZ());
		railChunkPosition.push_back(railPos.getUS());
	}
	else if (meta == 6)
	{
		railChunkPosition.push_back(railPos.getPosX());
		railChunkPosition.push_back(railPos.getPosZ());
	}
	else if (meta == 7)
	{
		railChunkPosition.push_back(railPos.getNegX());
		railChunkPosition.push_back(railPos.getPosZ());
	}
	else if (meta == 8)
	{
		railChunkPosition.push_back(railPos.getNegX());
		railChunkPosition.push_back(railPos.getNegZ());
	}
	else if (meta == 9)
	{
		railChunkPosition.push_back(railPos.getPosX());
		railChunkPosition.push_back(railPos.getNegZ());
	}
}

void BlockRailLogic::refreshConnectedTracks()
{
	size_t railsize = railChunkPosition.size();
	for (size_t i = 0; i < railsize; ++i)
	{
		BlockRailLogic* pRailLogic = getRailLogic(railChunkPosition[i]);

		if (pRailLogic != NULL && pRailLogic->isRailChunkPositionCorrect(this))
		{
			railChunkPosition[i] = pRailLogic->railPos;
		}
		else
		{
			//需要删除railChunkPosition[i];
			railChunkPosition[i] = railChunkPosition[railsize - 1]; // 把要删除的和最后一个交换
			railChunkPosition.pop_back(); // 最后一个删掉。
			railsize--; // railsize = railChunkPosition.size();
			--i; // 下次for循环还是i，但是已经是最后一个交换过来的。保证遍历所有！
			//railChunkPosition.remove(i--);
		}
	}
}

bool BlockRailLogic::isMinecartTrack(const BlockPos& pos)
{
	return BlockRailBase::isRailBlockAt(logicWorld, pos) ? true
		: (BlockRailBase::isRailBlockAt(logicWorld, pos.getPosY()) ? true
			: BlockRailBase::isRailBlockAt(logicWorld, pos.getNegY()));
}

BlockRailLogic* BlockRailLogic::getRailLogic(const BlockPos& pos)
{
	BlockRailLogic* pResult = NULL;
	if (BlockRailBase::isRailBlockAt(logicWorld, pos))
		pResult = LordNew BlockRailLogic(theRail, logicWorld, pos);
	else if (BlockRailBase::isRailBlockAt(logicWorld, pos.getPosY()))
		pResult = LordNew BlockRailLogic(theRail, logicWorld, pos.getPosY());
	else if (BlockRailBase::isRailBlockAt(logicWorld, pos.getNegY()))
		pResult = LordNew BlockRailLogic(theRail, logicWorld, pos.getNegY());

	if (pResult)
		deleteArr.push_back(pResult); // need delete!
	return pResult;
}

/** Checks if the rail is at the chunk position it is expected to be. */
bool BlockRailLogic::isRailChunkPositionCorrect(BlockRailLogic* pRailLogic)
{
	for (size_t i = 0; i < railChunkPosition.size(); ++i)
	{
		const BlockPos& pos = railChunkPosition[i];

		if (pos.x == pRailLogic->railPos.x && pos.z == pRailLogic->railPos.z)
		{
			return true;
		}
	}

	return false;
}

bool BlockRailLogic::isPartOfTrack(const BlockPos& pos)
{
	for (size_t i = 0; i < railChunkPosition.size(); ++i)
	{
		const BlockPos& rpos = railChunkPosition[i];

		if (rpos.x == pos.x && rpos.z == pos.z)
		{
			return true;
		}
	}

	return false;
}

int BlockRailLogic::getNumberOfAdjacentTracks()
{
	int result = 0;

	if (isMinecartTrack(railPos.getNegZ()))
		++result;
	if (isMinecartTrack(railPos.getPosZ()))
		++result;
	if (isMinecartTrack(railPos.getNegX()))
		++result;
	if (isMinecartTrack(railPos.getPosX()))
		++result;

	return result;
}

bool BlockRailLogic::canConnectTo(BlockRailLogic* pRailLoigc)
{
	return isRailChunkPositionCorrect(pRailLoigc) ? true : (railChunkPosition.size() == 2 ? false : (railChunkPosition.empty() ? true : true));
}

void BlockRailLogic::connectToNeighbor(BlockRailLogic* pRailLogic)
{
	railChunkPosition.push_back(pRailLogic->railPos);
	bool conNorth = isPartOfTrack(railPos.getNegZ());
	bool conSouth = isPartOfTrack(railPos.getPosZ());
	bool conWest = isPartOfTrack(railPos.getNegX());
	bool conEast = isPartOfTrack(railPos.getPosX());
	int direction = -1;

	if (conNorth || conSouth)
		direction = 0;
	if (conWest || conEast)
		direction = 1;

	if (!isStraightRail)
	{
		if (conSouth && conEast && !conNorth && !conWest)
			direction = 6;
		if (conSouth && conWest && !conNorth && !conEast)
			direction = 7;
		if (conNorth && conWest && !conSouth && !conEast)
			direction = 8;
		if (conNorth && conEast && !conSouth && !conWest)
			direction = 9;
	}

	if (direction == 0)
	{
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUN()))
			direction = 4;
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUS()))
			direction = 5;
	}

	if (direction == 1)
	{
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUE()))
			direction = 2;
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUW()))
			direction = 3;
	}

	if (direction < 0)
		direction = 0;

	int meta = direction;

	if (isStraightRail)
		meta = logicWorld->getBlockMeta(railPos) & 8 | direction;

	logicWorld->setBlockMetadataWithNotify(railPos, meta, 3);
}

bool BlockRailLogic::canConnectFrom(const BlockPos& pos)
{
	BlockRailLogic* pRailLogic = getRailLogic(pos);

	if (pRailLogic == NULL)
	{
		return false;
	}
	else
	{
		pRailLogic->refreshConnectedTracks();
		return pRailLogic->canConnectTo(this);
	}
}

void BlockRailLogic::checkConnectionToNeighbor(bool powered, bool flag)
{
	bool conNorth = canConnectFrom(railPos.getNegZ());
	bool conSouth = canConnectFrom(railPos.getPosZ());
	bool conWest = canConnectFrom(railPos.getNegX());
	bool conEast = canConnectFrom(railPos.getPosX());
	int direction = -1;

	if ((conNorth || conSouth) && !conWest && !conEast)
		direction = 0;
	if ((conWest || conEast) && !conNorth && !conSouth)
		direction = 1;

	if (!isStraightRail)
	{
		if (conSouth && conEast && !conNorth && !conWest)
			direction = 6;
		if (conSouth && conWest && !conNorth && !conEast)
			direction = 7;
		if (conNorth && conWest && !conSouth && !conEast)
			direction = 8;
		if (conNorth && conEast && !conSouth && !conWest)
			direction = 9;
	}

	if (direction == -1)
	{
		if (conNorth || conSouth)
			direction = 0;
		if (conWest || conEast)
			direction = 1;

		if (!isStraightRail)
		{
			if (powered)
			{
				if (conSouth && conEast)
					direction = 6;
				if (conWest && conSouth)
					direction = 7;
				if (conEast && conNorth)
					direction = 9;
				if (conNorth && conWest)
					direction = 8;
			}
			else
			{
				if (conNorth && conWest)
					direction = 8;
				if (conEast && conNorth)
					direction = 9;
				if (conWest && conSouth)
					direction = 7;
				if (conSouth && conEast)
					direction = 6;
			}
		}
	}

	if (direction == 0)
	{
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUN()))
			direction = 4;
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUS()))
			direction = 5;
	}

	if (direction == 1)
	{
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUE()))
			direction = 2;
		if (BlockRailBase::isRailBlockAt(logicWorld, railPos.getUW()))
			direction = 3;
	}

	if (direction < 0)
		direction = 0;

	setBasicRail(direction);
	int meta = direction;

	if (isStraightRail)
	{
		meta = logicWorld->getBlockMeta(railPos) & 8 | direction;
	}

	if (flag || logicWorld->getBlockMeta(railPos) != meta)
	{
		logicWorld->setBlockMetadataWithNotify(railPos, meta, 3);

		for (size_t i = 0; i < railChunkPosition.size(); ++i)
		{
			BlockRailLogic* pLogic = getRailLogic(railChunkPosition[i]);

			if (pLogic == NULL)
				continue;
			pLogic->refreshConnectedTracks();

			if (pLogic->canConnectTo(this))
			{
				pLogic->connectToNeighbor(this);
			}
		}
	}
}

}