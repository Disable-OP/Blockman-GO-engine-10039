#include "SynBlockStrategy.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "TileEntity/TileEntitys.h"

namespace BLOCKMAN
{

void SyncBlockStrategyVector::init(const Vector3& playPosition, std::vector<BlockPos>& blocks)
{
	auto world = Server::Instance()->getWorld();

	if (world == nullptr) return;

	m_updateBlocks.reserve(blocks.size());

	for (auto block : blocks)
	{
		Vector3 block_pos((float)block.x, (float)block.y, (float)block.z);
		float distance = getDistance(block_pos, playPosition);
		m_updateBlocks.emplace_back(block, world->getBlockId(block), world->getBlockMeta(block), distance);
	}

	std::sort(m_updateBlocks.begin(), m_updateBlocks.end());

	m_moveNextUpdateDistanceTick = 0;
}

void SyncBlockStrategyVector::add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta)
{
	float distance = getDistance(pos, playPosition);
	m_updateBlocks.emplace_back(pos, blockId, meta, distance);

	std::sort(m_updateBlocks.begin(), m_updateBlocks.end());
}

void SyncBlockStrategyVector::move(const Vector3& playPosition)
{
	if (m_moveNextUpdateDistanceTick < moveUpdateDistanceCd)
		m_moveNextUpdateDistanceTick++;

	if (m_moveNextUpdateDistanceTick >= moveUpdateDistanceCd)
	{
		if ((int)m_updateBlocks.size() <= 0) return;

		bool distance_change = false;

		for (std::vector<UpdateBlockItem>::iterator iter = m_updateBlocks.begin(); iter != m_updateBlocks.end(); iter++)
		{
			Vector3 block_pos((float)iter->pos.x, (float)iter->pos.y, (float)iter->pos.z);
			float distance = getDistance(block_pos, playPosition);

			if (Math::Abs(distance - iter->distance) > needChangeDistance)
			{
				distance_change = true;
				iter->distance = distance;
			}
		}

		if (distance_change)
		{
			std::sort(m_updateBlocks.begin(), m_updateBlocks.end());
		}

		m_moveNextUpdateDistanceTick = 0;
	}
}

void SyncBlockStrategyVector::sync(ui64 raknetID)
{
	if ((int)m_updateBlocks.size() <= 0) return;

	auto world = Server::Instance()->getWorld();

	if (world == nullptr) return;

	std::vector<BlockInfo> updatesBlock;
	std::vector<TileEntitySign*> titleEntitySignVec;

	// once update to client some blocks
	for (std::vector<UpdateBlockItem>::iterator iter = m_updateBlocks.begin(); iter != m_updateBlocks.end(); )
	{
		if ((int)updatesBlock.size() > onceSyncBlockNum)
		{
			break;
		}

		if (iter->distance < needUpdateDistance)
		{
			Vector3i position(iter->pos.x, iter->pos.y, iter->pos.z);
			BlockInfo block_info(position, world->getBlockId(position), world->getBlockMeta(position));
			updatesBlock.push_back(block_info);

			auto tileEntity = dynamic_cast<TileEntitySign*>(Server::Instance()->getWorld()->getBlockTileEntity(position));
			if (tileEntity)
			{
				titleEntitySignVec.push_back(tileEntity);
			}

			iter = m_updateBlocks.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	if (updatesBlock.size() <= 0) return;

	auto sender = Server::Instance()->getNetwork()->getSender();

	if (!sender) return;

	sender->sendUpdateBlocksWithDistance(raknetID, updatesBlock);

	for (auto tileEntity : titleEntitySignVec)
	{
		sender->sendSetSignTexts(tileEntity, raknetID);
	}
}

void SyncBlockStrategySection::init(const Vector3& playPosition, std::vector<BlockPos>& blocks)
{
	auto world = Server::Instance()->getWorld();

	if (world == nullptr) return;

	for (auto block : blocks)
	{
		add(playPosition, block, world->getBlockId(block), world->getBlockMeta(block));
	}
}

void SyncBlockStrategySection::add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta)
{
	SectionInfo sectionInfo;
	sectionInfo.absoluteX = i16(aPosToRPos(pos.x));
	sectionInfo.absoluteY = i16(aPosToRPos(pos.y));
	sectionInfo.absoluteZ = i16(aPosToRPos(pos.z));
	
	i16 playerSectionX = i16(aPosToRPos((int)playPosition.x));
	i16 playerSectionY = i16(aPosToRPos((int)playPosition.y));
	i16 playerSectionZ = i16(aPosToRPos((int)playPosition.z));

	BlockPosInSection blockPosInSection;
	blockPosInSection.relativeX = i8(pos.x % 16);
	blockPosInSection.relativeY = i8(pos.y % 16);
	blockPosInSection.relativeZ = i8(pos.z % 16);

	BlockInfoInSection blockInfoSection;
	blockInfoSection.pos = blockPosInSection;
	blockInfoSection.id = (ui16)blockId;
	blockInfoSection.meta = (ui8)meta;

	bool needSync = false;
	// FIX [SYMPTOM-4]: proximity must hold on ALL THREE axes (cube around the
	// player). With ||, any single axis within range — e.g. every section in
	// the player's Y band regardless of horizontal distance — was flagged,
	// syncing essentially the whole loaded world's edits.
	if (Math::Abs(sectionInfo.absoluteX - playerSectionX) <= needSyncSectionDistance
		&& Math::Abs(sectionInfo.absoluteY - playerSectionY) <= needSyncSectionDistance
		&& Math::Abs(sectionInfo.absoluteZ - playerSectionZ) <= needSyncSectionDistance)
	{
		needSync = true;
		isNeedSync = true;
	}
	
	auto sectionIter = mNearBySection.find(sectionInfo);

	if (sectionIter != mNearBySection.end())
	{
		auto blockIter = sectionIter->second.blockInfoInSectionMap.find(blockPosInSection);

		if (blockIter != sectionIter->second.blockInfoInSectionMap.end())
		{
			blockIter->second = blockInfoSection;
		}
		else
		{
			sectionIter->second.blockInfoInSectionMap[blockPosInSection] = blockInfoSection;
		}

		sectionIter->second.isNeedSync = needSync;
	}
	else
	{
		BlockInfoInSectionAll blockInfoInSectionAll;
		blockInfoInSectionAll.isNeedSync = needSync;

		blockInfoInSectionAll.blockInfoInSectionMap[blockPosInSection] = blockInfoSection;
		mNearBySection[sectionInfo] = blockInfoInSectionAll;
	}
}

void SyncBlockStrategySection::move(const Vector3& playPosition)
{
	// still in the same section
	if (aPosToRPos((int)playPosition.x) == aPosToRPos((int)mLastSection.absoluteX)
		&& aPosToRPos((int)playPosition.y) == aPosToRPos((int)mLastSection.absoluteY)
		&& aPosToRPos((int)playPosition.z) == aPosToRPos((int)mLastSection.absoluteZ))
	{
		return;
	}

	mLastSection.absoluteX = (i16)playPosition.x;
	mLastSection.absoluteY = (i16)playPosition.y;
	mLastSection.absoluteZ = (i16)playPosition.z;

	// section has changed
	bool needSync = false;
	for (SectionInfoMap::iterator sectionIter = mNearBySection.begin(); sectionIter != mNearBySection.end(); sectionIter++)
	{
		// FIX [SYMPTOM-4]: the old expression applied aPosToRPos to a DIFFERENCE
		// that was already in mixed units (absoluteX is already a section index,
		// aPosToRPos(playPosition.x) is a section index too, but their difference
		// was divided by 16 once more) — distances shrank 16x, so nearly every
		// section was flagged on every section change. Compare section indices
		// directly, on all three axes (cube).
		if (Math::Abs(((int)sectionIter->first.absoluteX) - aPosToRPos((int)playPosition.x)) <= needSyncSectionDistance
			&& Math::Abs(((int)sectionIter->first.absoluteY) - aPosToRPos((int)playPosition.y)) <= needSyncSectionDistance
			&& Math::Abs(((int)sectionIter->first.absoluteZ) - aPosToRPos((int)playPosition.z)) <= needSyncSectionDistance)
		{
			sectionIter->second.isNeedSync = true;
			needSync = true;
		}
		else
		{
			sectionIter->second.isNeedSync = false;
		}
	}

	if (needSync)
		isNeedSync = true;
	else
		isNeedSync = false;
}

void SyncBlockStrategySection::sync(ui64 raknetID)
{
	if (!isNeedSync) return;

	if (mNearBySection.empty()) return;

	std::vector<BlockInfo> updatesBlock;
	std::vector<TileEntitySign*> titleEntitySignVec;

	for (SectionInfoMap::iterator sectionIter = mNearBySection.begin(); sectionIter != mNearBySection.end(); )
	{
		if (!sectionIter->second.isNeedSync)
		{
			sectionIter++;
			continue;
		}

		for (BlockInfoInSectionMap::iterator blockIter = sectionIter->second.blockInfoInSectionMap.begin(); blockIter != sectionIter->second.blockInfoInSectionMap.end(); )
		{
			if ((int)updatesBlock.size() > onceSyncBlockNum)
			{
				break;
			}

			int absoluteX = (int)blockIter->second.pos.relativeX + rPosToaPos((int)sectionIter->first.absoluteX);
			int absoluteY = (int)blockIter->second.pos.relativeY + rPosToaPos((int)sectionIter->first.absoluteY);
			int absoluteZ = (int)blockIter->second.pos.relativeZ + rPosToaPos((int)sectionIter->first.absoluteZ);

			Vector3i position(absoluteX, absoluteY, absoluteZ);
			BlockInfo block_info(position, (int)blockIter->second.id, (int)blockIter->second.meta);
			updatesBlock.push_back(block_info);

			auto tileEntity = dynamic_cast<TileEntitySign*>(Server::Instance()->getWorld()->getBlockTileEntity(position));
			if (tileEntity)
			{
				titleEntitySignVec.push_back(tileEntity);
			}

			blockIter = sectionIter->second.blockInfoInSectionMap.erase(blockIter);
		}

		if ((int)updatesBlock.size() > onceSyncBlockNum)
		{
			break;
		}

		if (sectionIter->second.blockInfoInSectionMap.empty())
		{
			sectionIter = mNearBySection.erase(sectionIter);
		}
		else
		{
			sectionIter++;
		}
	}


	if (updatesBlock.size() <= 0) return;

	auto sender = Server::Instance()->getNetwork()->getSender();

	if (!sender) return;

	sender->sendUpdateBlocksWithDistance(raknetID, updatesBlock);

	for (auto tileEntity : titleEntitySignVec)
	{
		sender->sendSetSignTexts(tileEntity, raknetID);
	}

	if (mNearBySection.empty()) isNeedSync = false;
}

int SyncBlockStrategySection::aPosToRPos(int a)
{
	return (int)(a * 1.0 / 16);
}

int SyncBlockStrategySection::rPosToaPos(int r)
{
	return r * 16;
}


}
