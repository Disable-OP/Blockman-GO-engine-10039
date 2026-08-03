#ifndef __TRIGGER_ACTION_HELPER_HEADER__
#define __TRIGGER_ACTION_HELPER_HEADER__

#include "Trigger/TriggerAction.h"
#include "Trigger/TriggerTypeDef.h"

namespace BLOCKMAN
{
	class RapidJsonObject;

	class RemoteTriggerAction : public TriggerAction
	{
	public:
		static TriggerActionPtr parse(const RapidJsonObject&)
		{
			return LORD::make_shared<RemoteTriggerAction>();
		}

		virtual bool isRemoteAction() const override
		{
			return true;
		}

		virtual void doAction(const BlockPos& position, const TriggerActivatorList& activators) const override {}
	};

	template<TriggerActionType triggerActionType>
	struct BindedTriggerAction
	{
		using type = RemoteTriggerAction;
	};

	template<TriggerActionType triggerActionType>
	using BindedTriggerActionType = typename BindedTriggerAction<triggerActionType>::type;

}

#define BIND_TRIGGER_ACTION_TYPE(_type, _class)            \
class _class;                                              \
template<>                                                 \
struct BindedTriggerAction<TriggerActionType::_type>       \
{                                                          \
    using type = _class;                                   \
};

#endif // !__TRIGGER_ACTION_HELPER_HEADER__
