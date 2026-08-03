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
	struct BlockCropsInfo
	{
		BlockCropsInfo() {}
		~BlockCropsInfo() {}

		bool updateInfo();
		bool canPick(ui64 platformId);
		bool isComplete();
		int  getProductQuantity(ui64 platformId);

		ui64 ownerPlatformId = 0;
		std::time_t lastUpdateTime = 0;
		int curStage = 1; // 1~4 growth stage; 5 mature 6 incomplete
		int curStageTime = 0;
		int residueHarvestNum = 1;
		int blockId = 0;
		int stealCount = 0;
		Vector3i blockPos;
	};

	class BlockCropsManager
	{
	public:
		BlockCropsManager(ServerWorld * pServerWorld);
		~BlockCropsManager();

		void updateCrops();
		bool addBlockCrops(BlockPos& pos, BlockCropsInfo* pCropsInfo);
		bool deleteBlockCrops(BlockPos& pos);
		bool pickCrops(ui64 platformId, BlockPos& pos);
		bool canPick(ui64 platformId, BlockPos& pos);
		bool isCropsComplete(BlockPos& pos);
		BlockCropsInfo* findCrop(BlockPos& pos);
		bool speedUpCrop(ui64 platformId, BlockPos& pos);

	private:
		void updateCropsMeta(BlockCropsInfo* pCropsInfo);

		int m_cropsTick = 0;
		ServerWorld * m_serverWorld = nullptr;
		Vector3iMap<BlockCropsInfo*> m_BlockCropsInfoMap;
		SubscriptionGuard	m_subscriptionGuard;
	};
}