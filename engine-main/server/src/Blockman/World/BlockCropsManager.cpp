#include "BlockCropsManager.h"
#include "Setting/CropsSetting.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Util/CommonEvents.h"
#include "ServerWorld.h"
#include "Script/GameServerEvents.h"
#include "Network/ServerNetwork.h"

namespace BLOCKMAN 
{
	BlockCropsManager::BlockCropsManager(ServerWorld * pServerWorld)
		: m_cropsTick(0)
		, m_serverWorld(pServerWorld)
	{
		m_subscriptionGuard.add(OnUseItemSeedsEvent::subscribe([this](EntityPlayer* pPlayer, BlockPos pos, int blockId) -> bool
		{
			BlockNewCrops* crops = dynamic_cast<BlockNewCrops*>(BlockManager::getBlockById(blockId));
			if (crops == NULL)
				return false;

			EntityPlayerMP* mp = dynamic_cast<EntityPlayerMP*>(pPlayer);
			if (mp == NULL)
				return false;
			CropsSetting* pSetting = CropsSetting::getCropsSetting(blockId);

			BlockCropsInfo* pInfo = new BlockCropsInfo();
			pInfo->ownerPlatformId = mp->getPlatformUserId();
			pInfo->lastUpdateTime = std::time(0);
			pInfo->curStage = 1;
			pInfo->curStageTime = 0;
			pInfo->residueHarvestNum = pSetting->canHarvestNum;
			pInfo->blockId = blockId;
			pInfo->blockPos = pos;
			addBlockCrops(pos, pInfo);
			SCRIPT_EVENT::BlockCropsPlaceEvent::invoke(mp->getPlatformUserId(), blockId, pos, pSetting->canHarvestNum);
			return true;
		}));

		m_subscriptionGuard.add(OnNewCropsBlockDestroyEvent::subscribe([this](BlockPos pos) -> bool
		{
			deleteBlockCrops(pos);
			return true;
		}));
	}

	BlockCropsManager::~BlockCropsManager()
	{
		for (auto iter = m_BlockCropsInfoMap.begin(); iter != m_BlockCropsInfoMap.end(); iter++)
		{
			delete iter->second;
		}

		m_BlockCropsInfoMap.clear();
		m_subscriptionGuard.unsubscribeAll();
		m_serverWorld = nullptr;
	}

	void BlockCropsManager::updateCrops()
	{
		m_cropsTick++;
		if (m_cropsTick % 20 != 0)
			return;

		bool bCropsChanged = false;
		for (auto iter = m_BlockCropsInfoMap.begin(); iter != m_BlockCropsInfoMap.end(); iter++)
		{
			BlockCropsInfo* pInfo = iter->second;
			bCropsChanged = pInfo->updateInfo();
			if (bCropsChanged)
			{
				updateCropsMeta(pInfo);
				//todo: lua event, storage....
				SCRIPT_EVENT::BlockCropsUpdateEvent::invoke(pInfo->ownerPlatformId, pInfo->blockId, pInfo->curStage, pInfo->curStageTime, pInfo->stealCount, pInfo->blockPos, pInfo->residueHarvestNum);
			}
		}
	}

	bool BlockCropsManager::addBlockCrops(BlockPos & pos, BlockCropsInfo* pCropsInfo)
	{
		m_BlockCropsInfoMap[pos] = pCropsInfo;

		return true;
	}

	bool BlockCropsManager::deleteBlockCrops(BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter != m_BlockCropsInfoMap.end())
		{
			m_serverWorld->setBlockToAir(pos);
			delete iter->second;
			m_BlockCropsInfoMap.erase(iter);

			//todo: event
			return true;
		}

		return true;
	}

	bool BlockCropsManager::pickCrops(ui64 platformId, BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter == m_BlockCropsInfoMap.end())
			return false;

		BlockCropsInfo* pInfo = iter->second;
		if (!pInfo->canPick(platformId)) {
			if (platformId == pInfo->ownerPlatformId)
			{
				ServerNetwork::Instance()->getSender()->sendCropInfo(platformId, pInfo->blockPos, pInfo->blockId, pInfo->curStage, pInfo->curStageTime, pInfo->residueHarvestNum);
			}
			return false;
		}

		CropsSetting* pSetting = CropsSetting::getCropsSetting(pInfo->blockId);
		if (platformId == pInfo->ownerPlatformId)
		{
			int quantity = pInfo->getProductQuantity(platformId);
			LordLogInfo("======Owner pickCrops %d, get ItemId %d , num %d", pInfo->blockId, pSetting->productionId, quantity);
			pInfo->residueHarvestNum -= 1;
			i32 oldStage = pInfo->curStage;
			pInfo->curStage = CROPS_TOTAL_STAGE - 1;
			if (!SCRIPT_EVENT::BlockCropsPickEvent::invoke(platformId, pInfo->blockId, pSetting->productionId, quantity, pos, pInfo->curStage, pInfo->residueHarvestNum))
			{
				pInfo->residueHarvestNum += 1;
				pInfo->curStage = oldStage;
				return false;
			}
			
			pInfo->curStageTime = 0;
			pInfo->lastUpdateTime = (int)std::time(0);
			
			if (pInfo->residueHarvestNum <= 0)
			{
				deleteBlockCrops(pos);
			}
			else
			{
				updateCropsMeta(pInfo);
			}
		}
		else
		{
			int quantity = pInfo->getProductQuantity(platformId);
			pInfo->stealCount = quantity;
			pInfo->curStage = CROPS_TOTAL_STAGE + 2;
			pInfo->curStageTime = 0;
			updateCropsMeta(pInfo);
			LordLogInfo("======others pickCrops %d, get ItemId %d , num %d", pInfo->blockId, pSetting->productionId, quantity);
			
			SCRIPT_EVENT::BlockCropsStealEvent::invoke(platformId, pInfo->blockId, pSetting->productionId, quantity, pos, pInfo->residueHarvestNum);
		}

		return true;
	}

	bool BlockCropsManager::canPick(ui64 platformId, BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter == m_BlockCropsInfoMap.end())
			return false;

		return iter->second->canPick(platformId);
	}

	bool BlockCropsManager::isCropsComplete(BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter == m_BlockCropsInfoMap.end())
			return false;

		return iter->second->isComplete();
	}

	BlockCropsInfo* BlockCropsManager::findCrop(BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter != m_BlockCropsInfoMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	bool BlockCropsManager::speedUpCrop(ui64 platformId, BlockPos & pos)
	{
		auto iter = m_BlockCropsInfoMap.find(pos);
		if (iter == m_BlockCropsInfoMap.end())
			return false;

		BlockCropsInfo* pInfo = iter->second;
		pInfo->curStage = CROPS_TOTAL_STAGE + 1;
		pInfo->curStageTime = 0;
		pickCrops(platformId, pos);
		return false;
	}

	void BlockCropsManager::updateCropsMeta(BlockCropsInfo * pCropsInfo)
	{
		m_serverWorld->setBlock(pCropsInfo->blockPos, pCropsInfo->blockId, pCropsInfo->curStage - 1, 3);
	}

	bool BlockCropsInfo::updateInfo()
	{
		bool bret = false;
		if (curStage == CROPS_TOTAL_STAGE + 1)
			return bret;
		
		std::time_t now = std::time(0);
		curStageTime += (int)(now - lastUpdateTime);
		lastUpdateTime = now;
		CropsSetting* pSetting = CropsSetting::getCropsSetting(blockId);
		if (pSetting == NULL)
		{
			LordLogError("There is no CropSetting of Crops Id %d", blockId);
			return bret;
		}

		int stageTime = 0;
		if (curStage <= CROPS_TOTAL_STAGE)
		{
			stageTime = pSetting->getStateTime(curStage);
			if (curStageTime >= stageTime)
			{
				curStageTime -= stageTime;
				curStage++;
				bret = true;
				//todo : event
			}
		}
		else
		{
			stageTime = pSetting->recoverTime;
			if (curStageTime >= stageTime)
			{
				curStage = CROPS_TOTAL_STAGE + 1;
				curStageTime = 0;
				bret = true;
				stealCount = 0;
				//todo : event
			}
		}

		return bret;
	}

	bool BlockCropsInfo::canPick(ui64 platformId)
	{
		if(curStage <= CROPS_TOTAL_STAGE)
			return false;

		if (platformId != ownerPlatformId && curStage != CROPS_TOTAL_STAGE + 1)
			return false;

		return true;
	}

	bool BlockCropsInfo::isComplete()
	{
		return curStage == CROPS_TOTAL_STAGE + 1;
	}

	int BlockCropsInfo::getProductQuantity(ui64 platformId)
	{
		int num = 0;
		if(!canPick(platformId))
			return 0;

		CropsSetting* pSetting = CropsSetting::getCropsSetting(blockId);
		if (pSetting == NULL)
		{
			LordLogError("There is no CropSetting of Crops Id %d", blockId);
			return 0;
		}

		Random random;
		if (platformId != ownerPlatformId)
		{
			int randomNum = pSetting->maxStealQuantity - pSetting->minStealQuantity;
			if (randomNum > 0)
			{
				randomNum = random.nextInt(randomNum);
			}
			return pSetting->minStealQuantity + randomNum;
		}
		else
		{
			int randomNum = pSetting->maxQuantity - pSetting->minQuantity;
			if (randomNum > 0)
			{
				randomNum = random.nextInt(randomNum);
			}
			return pSetting->minQuantity + randomNum - stealCount;
		}
	}
}


