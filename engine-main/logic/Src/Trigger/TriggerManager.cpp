#include "TriggerManager.h"
#include "Trigger/Trigger.h"
#include "World/World.h"
#include "Trigger/TriggerNetwork.h"

namespace BLOCKMAN
{
	namespace TRIGGER_MANAGER
	{
		void TriggerManager::checkTriggersImpl(World * world, const BlockPos & pos, std::function<TriggerActivatorList(TriggerPtr)> checkCondition)
		{
			int blockId = world->getBlockId(pos);
			auto iter = m_blockIdToTriggerList.find(blockId);
			if (iter == m_blockIdToTriggerList.end())
			{
				return;
			}
			const TriggerList& triggers = iter->second;
			for (size_t i = 0; i < triggers.size(); ++i)
			{
				TriggerPtr trigger = triggers[i];
				if (world->m_isClient && trigger->hasRemoteAction()
					|| !world->m_isClient && !trigger->hasLocalAction())
				{
					continue;
				}
				TriggerActivatorList activators = checkCondition(trigger);
				if (activators.empty())
				{
					continue;
				}
				if (trigger->hasRemoteAction())
				{
					TriggerNetwork::Instance()->sendActivateTrigger(pos, blockId, trigger->getType(), i, activators);
				}
				for (auto action : trigger->getActions())
				{
					action->doAction(pos, activators);
				}
			}
		}
	}
}