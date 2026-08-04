#ifndef __TRIGGER_NETWORK__
#define __TRIGGER_NETWORK__

#include "Core.h"
#include "Trigger/TriggerTypeDef.h"

namespace BLOCKMAN
{
	class TriggerActivator;

	using BlockPos = LORD::Vector3i;
	using TriggerActivatorList = LORD::vector<TriggerActivator>::type;

	class TriggerNetwork : public LORD::Singleton<TriggerNetwork>, public LORD::ObjectAlloc
	{
	public:
		virtual void sendActivateTrigger(const BlockPos& blockPos, int blockId, 
			TriggerType type, int triggerIndex, const TriggerActivatorList& activators) const {}
	};
}

#endif // !__TRIGGER_SYNCHRONIZER__
