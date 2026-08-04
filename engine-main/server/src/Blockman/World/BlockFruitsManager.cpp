#include "BlockFruitsManager.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Util/CommonEvents.h"
#include "ServerWorld.h"
#include "Script/GameServerEvents.h"
#include "Network/ServerNetwork.h"
#include "Setting/FruitsSetting.h"
#include "Script/Event/LogicScriptEvents.h"


namespace BLOCKMAN 
{
	BlockFruitsManager::BlockFruitsManager(ServerWorld * pServerWorld)
		: m_cropsTick(0)
		, m_serverWorld(pServerWorld)
	{
		//m_subscriptionGuard.add(BirdSimulatorGatherEvent::subscribe(std::bind(&BlockFruitsManager::onGather, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3)));
	}

	BlockFruitsManager::~BlockFruitsManager()
	{
		for (auto iter = m_blockFruitsInfoMap.begin(); iter != m_blockFruitsInfoMap.end(); iter++)
		{
			delete iter->second;
		}

		m_blockFruitsInfoMap.clear();
		m_subscriptionGuard.unsubscribeAll();
		m_serverWorld = nullptr;
	}

	void BlockFruitsManager::updateFruits()
	{
		m_cropsTick++;
		if (m_cropsTick % 20 != 0)
			return;

		for (auto item : m_blockFruitsInfoMap)
		{
			if (item.second)
			{
				if (item.second->updateInfo())
				{
					updateFruitsMeta(item.second);
				}
			}
		}
	}

	bool BlockFruitsManager::addBlockFruits(const BlockPos & pos, i32 blockId)
	{
		const auto  fruit = FruitsSetting::findFruitById(blockId);
		if (!fruit)
		{
			return false;
		}
		BlockFruitsInfo* pFruitsInfo = new BlockFruitsInfo();
		pFruitsInfo->blockId = blockId;
		pFruitsInfo->blockPos = pos;
		pFruitsInfo->score = fruit->score;
		pFruitsInfo->maxStage = fruit->maxStage;
		pFruitsInfo->resetTime = fruit->resetTime;
		pFruitsInfo->recoveryTime = fruit->recoveryTime;
		pFruitsInfo->lastGatherTime = 0;
		pFruitsInfo->firstGatherTime = 0;
		pFruitsInfo->curStage = 0;

		m_blockFruitsInfoMap[pos] = pFruitsInfo;
		return true;
	}

	bool BlockFruitsManager::deleteBlockFruits(const BlockPos & pos)
	{
		auto iter = m_blockFruitsInfoMap.find(pos);
		if (iter != m_blockFruitsInfoMap.end())
		{
			m_serverWorld->setBlockToAir(pos);
			delete iter->second;
			m_blockFruitsInfoMap.erase(iter);
			//todo: event
			return true;
		}

		return true;
	}

	bool BlockFruitsManager::onGather(ui64 platformId, i64 birdId, const BlockPos& pos)
	{
		BlockFruitsInfo* pInfo = findFruits(pos);
		if (!pInfo || !pInfo->canPick()) {
			return false;
		}

		IntProxy num;
		num.value = pInfo->curStage;
		
		if (!SCRIPT_EVENT::BirdSimulatorAddGatherScoreEvent::invoke(platformId, birdId, pInfo->blockId, pos, pInfo->score, pInfo->maxStage, &num))
		{
			return false;
		}

		if (pInfo->curStage == 0)
		{
			pInfo->firstGatherTime = std::time(0);
		}

		pInfo->lastGatherTime = std::time(0);
		pInfo->curStage = num.value;
		updateFruitsMeta(pInfo);
		return true;
	}

	BlockFruitsInfo* BlockFruitsManager::findFruits(const BlockPos & pos)
	{
		if (!m_serverWorld)
		{
			return nullptr;
		}
		auto iter = m_blockFruitsInfoMap.find(pos);
		if (iter == m_blockFruitsInfoMap.end())
		{
			i32 blockId = m_serverWorld->getBlockId(pos);
			if (!dynamic_cast<BlockFruits*>(BlockManager::sBlocks[blockId]))
			{
				return nullptr;
			}

			if (addBlockFruits(pos, blockId))
			{
				iter = m_blockFruitsInfoMap.find(pos);
				if (iter == m_blockFruitsInfoMap.end())
				{
					return nullptr;
				}
			}
		}

		return iter->second;
	}

	void BlockFruitsManager::updateFruitsMeta(BlockFruitsInfo * pFruitsInfo)
	{
		if (m_serverWorld)
		{
			m_serverWorld->setBlockMetadataWithNotify(pFruitsInfo->blockPos, pFruitsInfo->curStage, 2);
		}
	}

	bool BlockFruitsInfo::updateInfo()
	{
		if (curStage > 0)
		{
			std::time_t curTime = std::time(0);
			bool isChange = false;
			if (curTime - lastGatherTime >= recoveryTime)
			{
				curStage -= 1;
				isChange = true;
				lastGatherTime = 0;
			}

			if (curTime - firstGatherTime >= resetTime)
			{
				curStage = 0;
				isChange = true;
				firstGatherTime = 0;
			}
			return isChange;
		}
		return false;
	}

	bool BlockFruitsInfo::canPick()
	{
		return curStage < maxStage;
	}

	
}


