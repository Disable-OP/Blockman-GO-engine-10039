#pragma once

#include "Core.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "BM_TypeDef.h"

using namespace LORD;
namespace BLOCKMAN
{
	struct BlockDoorOpenTick 
	{
		BlockDoorOpenTick(int maxTick)
			: curTick(0)
			, maxTick(maxTick)
		{}

		int curTick;
		int maxTick;
	};

	class BlockDoorManager
	{
	public:
		BlockDoorManager(ServerWorld *pWorld)
			: m_serverWorld(pWorld)
		{
			m_pBlockDoor = dynamic_cast<BlockDoor*>(BlockManager::sBlocks[241]);
		}	

		~BlockDoorManager()
		{
			for (auto iter = m_posToTick.begin(); iter != m_posToTick.end(); iter++)
			{
				delete iter->second;
			}

			m_posToTick.clear();
		}

		void tick()
		{
			if (!m_pBlockDoor)
				return;

			for (auto iter = m_posToTick.begin(); iter != m_posToTick.end(); )
			{
				iter->second->curTick++;
				if (iter->second->curTick >= iter->second->maxTick)
				{
					m_pBlockDoor->switchDoorState(m_serverWorld, iter->first, true);
					delete iter->second;
					m_posToTick.erase(iter++);
				}
				else
				{
					iter++;
				}
			}
		}

		void addPos(const BlockPos& pos, int maxTick)
		{
			m_posToTick[pos] =  new BlockDoorOpenTick(maxTick);
		}

		void deletePos(const BlockPos& pos)
		{
			auto iter = m_posToTick.find(pos);
			if (iter != m_posToTick.end())
			{
				delete iter->second;
				m_posToTick.erase(iter);
			}
		}

	private:
		ServerWorld * m_serverWorld = nullptr;
		BlockDoor* m_pBlockDoor = nullptr;
		Vector3iMap<BlockDoorOpenTick*> m_posToTick;
	};
}
