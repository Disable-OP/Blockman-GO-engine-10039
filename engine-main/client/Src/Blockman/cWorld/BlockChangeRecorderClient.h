#ifndef __BLOCK_CHANGE_RECORDER_CLIENT_HEADER__
#define __BLOCK_CHANGE_RECORDER_CLIENT_HEADER__

#include <functional>
#include "Core.h"
#include "World/Section.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	class BlockChangeRecorderClient : public Singleton<BlockChangeRecorderClient>, public ObjectAlloc
	{
	private:
		using BlockPos = Vector3i;
		struct BlockInfo
		{
			uint16_t blockMetaAndId;

			uint16_t getBlockId() const { return blockMetaAndId & 0xFFF; }
			uint8_t getBlockMeta() const { return (blockMetaAndId >> 12) & 15; }
		};

		static constexpr size_t MAX_CHANGES_IN_ONE_CHUNK = 64;
		vector<Vector3iMap<BlockInfo>>::type m_chunkChanges;
		std::function<void(int, int)> m_recordClearListener;

		static size_t chunkPositionToIndex(int x, int z);

	public:
		BlockChangeRecorderClient();
		void record(const BlockPos& position, uint16_t blockMetaAndId);
		void record(const BlockPos& position, uint16_t blockId, uint8_t blockMeta)
		{
			record(position, blockMeta << 12 | blockId & 0xFFF);
		}
		void recordBlocks(int chunkX, int chunkZ, const BlockModifys& modifys);
		void applyChanges(const ChunkPtr& chunk) const;
	};
}

#endif