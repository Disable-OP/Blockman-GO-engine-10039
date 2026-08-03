#pragma once

#include "Block/BlockManager.h"

namespace BLOCKMAN
{
	class BlockUtil
	{
	public:
		static bool isValidBlockId(int id)
		{
			return id >= 0 && id < BlockManager::MAX_BLOCK_COUNT;
		}
		static bool isBasicBlock(int id)
		{
			return id >= BlockManager::BASIC_BLOCK_ID_BEGIN && id < BlockManager::BASIC_BLOCK_ID_END;
		}
		static bool isExtendedBlock(int id)
		{
			return id >= BlockManager::EXTENDED_BLOCK_ID_BEGIN && id < BlockManager::EXTENDED_BLOCK_ID_END;
		}
		static bool isCustomBlock(int id)
		{
			return id >= BlockManager::CUSTOM_BLOCK_ID_BEGIN && id < BlockManager::CUSTOM_BLOCK_ID_END;
		}
		static bool isECBlock(int id)
		{
			return isExtendedBlock(id) || isCustomBlock(id);
		}
	};
}