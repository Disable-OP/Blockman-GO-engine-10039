#include "BlockChangeRecorderClient.h"
#include "Chunk/Chunk.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	size_t BlockChangeRecorderClient::chunkPositionToIndex(int x, int z)
	{
		int offset = 0;
		if (x >= 0 && z >= 0)
		{
			offset = 0;
		}
		else if (x >= 0 && z < 0)
		{
			offset = 1;
			z = -z - 1;
		}
		else if (x < 0 && z < 0)
		{
			offset = 2;
			x = -x - 1;
			z = -z - 1;
		}
		else if (x < 0 && z >= 0)
		{
			offset = 3;
			x = -x - 1;
		}
		int distance = (std::max)(x, z);
		int index = distance * distance + x;
		if (x == distance)
		{
			index += z;
		}
		return index * 4 + offset;
	}

	BlockChangeRecorderClient::BlockChangeRecorderClient()
	{
		m_chunkChanges.resize(1024);
	}

	void BlockChangeRecorderClient::record(const BlockPos & blockPos, uint16_t blockMetaAndId)
	{
		int chunkX = blockPos.x >> 4;
		int chunkZ = blockPos.z >> 4;
		auto index = chunkPositionToIndex(chunkX, chunkZ);
		if (m_chunkChanges.size() <= index)
		{
			m_chunkChanges.resize(index + 1);
		}
		auto& chunkChange = m_chunkChanges[index];
		if (chunkChange.size() >= MAX_CHANGES_IN_ONE_CHUNK)
		{
			ClientBlockChangeRecordClearEvent::emit(chunkX, chunkZ);
			chunkChange.clear();
		}
		else
		{
			chunkChange[blockPos] = { blockMetaAndId };
		}
	}

	void BlockChangeRecorderClient::recordBlocks(int chunkX, int chunkZ, const BlockModifys & modifys)
	{
		if ((int)modifys.size() >= MAX_CHANGES_IN_ONE_CHUNK)
		{
			ClientBlockChangeRecordClearEvent::emit(chunkX, chunkZ);
		}
		else
		{
			for (const auto& block : modifys)
			{
				record(block.pos, (uint16_t)block.id, (uint8_t)block.meta);
			}
		}
	}

	void BlockChangeRecorderClient::applyChanges(const ChunkPtr& chunk) const
	{
		LordAssert(chunk);
		if (chunk->isNonexistent())
		{
			return;
		}
		auto index = chunkPositionToIndex(chunk->m_posX, chunk->m_posZ);
		if (m_chunkChanges.size() <= index)
		{
			return;
		}
		const auto& chunkChange = m_chunkChanges[index];
		for (const auto& pair : chunkChange)
		{
			const auto& absPos = pair.first;
			BlockPos relativePos(absPos.x & 15, absPos.y, absPos.z & 15);
			chunk->setBlockIDAndMeta(relativePos, pair.second.getBlockId(), pair.second.getBlockMeta());
		}
	}
}
