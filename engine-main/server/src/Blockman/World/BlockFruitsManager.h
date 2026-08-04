#pragma once

#include "Core.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "BM_TypeDef.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;
namespace BLOCKMAN
{
	class ServerWorld;
	class EntityPlayer;

	struct BlockFruitsInfo
	{
		BlockFruitsInfo() {}
		~BlockFruitsInfo() {}

		bool updateInfo();
		bool canPick();

		std::time_t firstGatherTime = 0;
		std::time_t lastGatherTime = 0;
		int curStage = 0; // 0~9 growth stage;
		int maxStage = 9;
		int blockId = 0;
		int recoveryTime = 0;
		int resetTime = 0;
		int score = 1;
		Vector3i blockPos;
	};

	class BlockFruitsManager
	{
	public:
		BlockFruitsManager(ServerWorld * pServerWorld);
		~BlockFruitsManager();

		void updateFruits();
		bool addBlockFruits(const BlockPos& pos, i32 blockId);
		bool deleteBlockFruits(const BlockPos& pos);
		bool onGather(ui64 platformId, i64 birdId, const BlockPos& pos);
		BlockFruitsInfo* findFruits(const BlockPos& pos);

	private:
		void updateFruitsMeta(BlockFruitsInfo* pFruitsInfo);

		int m_cropsTick = 0;
		ServerWorld * m_serverWorld = nullptr;
		Vector3iMap<BlockFruitsInfo*> m_blockFruitsInfoMap;
		SubscriptionGuard m_subscriptionGuard;
	};
}