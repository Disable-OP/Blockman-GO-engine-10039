#ifndef __BLOCK_CHANGE_RECORDER_SERVER_HEADER__
#define __BLOCK_CHANGE_RECORDER_SERVER_HEADER__

#include "Core.h"
#include "Server.h"
#include "Blockman/World/ServerWorld.h"
#include "Network/ServerNetwork.h"
#include "Blockman/Chunk/ChunkServer.h"
using namespace LORD;

namespace BLOCKMAN
{
    class BlockChangeRecorderServer : public Singleton<BlockChangeRecorderServer>, public ObjectAlloc
    {
    private:
        using BlockPos = Vector3i;
		struct RecordItem
		{
			BlockPos pos;
			int  id;
			int  meta;
		};

        struct BlockPosCompare
        {
            bool operator()(const RecordItem& lhs, const RecordItem& rhs) const
            {
                if (lhs.pos.x < rhs.pos.x)
                {
                    return true;
                }
                if (lhs.pos.x > rhs.pos.x)
                {
                    return false;
                }
                if (lhs.pos.y < rhs.pos.y)
                {
                    return true;
                }
                if (lhs.pos.y > rhs.pos.y)
                {
                    return false;
                }
                if (lhs.pos.z < rhs.pos.z)
                {
                    return true;
                }
                return false;
            }
        };
        using Records = set<RecordItem, BlockPosCompare>::type;
        Records m_changedPositions;
		std::vector<BlockPos>   m_blockSyncList;
    public:
        void record(int x, int y, int z, int srcId, int srcMeta)
        {
            record(BlockPos(x, y, z), srcId, srcMeta);
        }

        void record(const BlockPos& blockPos, int srcId, int srcMeta)
        {
            //LordLogDebug("block update: x:%d, y:%d, z:%d, id:%d, meta:%d", 
            //    blockPos.x, blockPos.y, blockPos.z, 
             //   Server::Instance()->getWorld()->getBlockId(blockPos), 
             //   Server::Instance()->getWorld()->getBlockMeta(blockPos));
			
			RecordItem item;
			item.pos = blockPos;
			item.id = srcId;
			item.meta = srcMeta;
            m_changedPositions.insert(item);
        }
        const Records getChangedPositions()
        {
            return m_changedPositions;
        }

		std::vector<BlockPos>& getBlockSyncList()
		{
			return m_blockSyncList;
		}

		void addToSyncList(BlockPos& pos)
		{
			m_blockSyncList.push_back(pos);
		}

		void reset()
		{
			for (auto &item : m_changedPositions)
			{
				Server::Instance()->getWorld()->getChunkFromBlockCoords(item.pos.x, item.pos.z)->setBlockIDAndMetaWithoutSync(item.pos.getChunkPos(), item.id, item.meta);
			}

			Server::Instance()->getNetwork()->getSender()->resetUpdateBlocks();
			this->m_changedPositions.clear();
		}

		bool isInChangeSet(BlockPos blockPos, int srcId, int srcMeta)
		{
			RecordItem item;
			item.pos = blockPos;
			item.id = srcId;
			item.meta = srcMeta;
			auto iter = m_changedPositions.find(item);

			return iter != m_changedPositions.end();
		}
    };
}

#endif // !__BLOCK_CHANGE_RECORDER_HEADER__
