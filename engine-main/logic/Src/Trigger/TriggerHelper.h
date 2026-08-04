#ifndef __TRIGGER_HELPER_HEADER__
#define __TRIGGER_HELPER_HEADER__

#include "Trigger/Trigger.h"

namespace BLOCKMAN
{
	template<TriggerType triggerType>
	struct BindedTrigger
	{
	};

	template<TriggerType triggerType>
	using BindedTriggerType = typename BindedTrigger<triggerType>::type;
}

#define BIND_TRIGGER_TYPE(_type, _class)                   \
class _class;                                              \
template<>                                                 \
struct BindedTrigger<TriggerType::_type>                   \
{                                                          \
    using type = _class;                                   \
};

#endif // !__TRIGGER_HELPER_HEADER__
