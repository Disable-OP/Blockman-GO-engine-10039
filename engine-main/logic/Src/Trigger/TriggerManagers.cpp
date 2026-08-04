#include "TriggerManagers.h"

using namespace LORD;

namespace BLOCKMAN
{
	namespace TRIGGER_MANAGER
	{

		void createTriggerManagers()
		{
			LordNew TriggerClickManager;
			LordNew TriggerInRangeManager;
			LordNew TriggerTimeIntervalManager;
			LordNew TriggerBlockAdjacentManager;
		}

		void destroyTriggerManagers()
		{
			LordDelete(TriggerClickManager::Instance());
			LordDelete(TriggerInRangeManager::Instance());
			LordDelete(TriggerTimeIntervalManager::Instance());
			LordDelete(TriggerBlockAdjacentManager::Instance());
		}

		TriggerManager* getTriggerManager(TriggerType type)
		{
			switch (type)
			{
			case TriggerType::CLICK:
				return TriggerClickManager::Instance();
			case TriggerType::IN_RANGE:
				return TriggerInRangeManager::Instance();
			case TriggerType::TIME_INTERVAL:
				return TriggerTimeIntervalManager::Instance();
			case TriggerType::BLOCK_ADJACENT:
				return TriggerBlockAdjacentManager::Instance();
			default:
				return nullptr;
			}
		}

		void notifyBlockChange(const BlockPos & pos, int oldId, int newId)
		{
			TriggerInRangeManager::Instance()->onBlockChange(pos, oldId, newId);
			TriggerTimeIntervalManager::Instance()->onBlockChange(pos, oldId, newId);
		}

		void registerTriggers(int blockId, const TriggerList & triggers)
		{
			for (TriggerPtr trigger : triggers)
			{
				switch (trigger->getType())
				{
				case TriggerType::CLICK:
					TriggerClickManager::Instance()->registerTrigger(blockId, trigger);
					break;
				case TriggerType::IN_RANGE:
					TriggerInRangeManager::Instance()->registerTrigger(blockId, trigger);
					break;
				case TriggerType::TIME_INTERVAL:
					TriggerTimeIntervalManager::Instance()->registerTrigger(blockId, trigger);
					break;
				case TriggerType::BLOCK_ADJACENT:
					TriggerBlockAdjacentManager::Instance()->registerTrigger(blockId, trigger);
					break;
				default:
					break;
				}
			}
		}

		void unregisterTriggers(int blockId)
		{
			TriggerClickManager::Instance()->unregisterTriggers(blockId);
			TriggerInRangeManager::Instance()->unregisterTriggers(blockId);
			TriggerTimeIntervalManager::Instance()->unregisterTriggers(blockId);
			TriggerBlockAdjacentManager::Instance()->unregisterTriggers(blockId);
		}
	}
}