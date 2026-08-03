#ifndef __TRIGGER_NETWORK_SERVER__
#define __TRIGGER_NETWORK_SERVER__

#include "Trigger/TriggerNetwork.h"
#include "Trigger/TriggerActivator.h"
#include "Network/ServerNetwork.h"
#include "Blockman/Entity/EntityPlayerMP.h"

namespace BLOCKMAN
{
	class TriggerNetworkServer : public TriggerNetwork
	{
	public:
		virtual void sendActivateTrigger(const BlockPos& blockPos, int blockId,
			TriggerType type, int triggerIndex, const TriggerActivatorList& activators) const
		{
			for (auto& activator : activators)
			{
				if (!activator.isEntity())
				{
					continue;
				}
				if (auto player = dynamic_cast<EntityPlayerMP*>(activator.toEntity()))
				{
					ServerNetwork::Instance()->getSender()->sendActivateTrigger(
						player->getRaknetID(), blockPos, blockId, triggerIndex);
				}
			}
		}
	};
}
#endif // !__TRIGGER_NETWORK_SERVER__
