#ifndef __TRIGGER_ACTION_HEADER__
#define __TRIGGER_ACTION_HEADER__

#include "Core.h"
#include "Trigger/TriggerActivator.h"

namespace BLOCKMAN
{
	using BlockPos = LORD::Vector3i;

	class TriggerAction : public LORD::ObjectAlloc
	{
	public:
		virtual ~TriggerAction() = default;
		virtual bool isRemoteAction() const
		{
			return false;
		}
		virtual void doAction(const BlockPos& position, const TriggerActivatorList& activators) const = 0;
	};

	using TriggerActionPtr = std::shared_ptr<TriggerAction>;
	using TriggerActionList = LORD::vector<TriggerActionPtr>::type;
}

#endif
