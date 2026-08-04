#ifndef __TRIGGER_TYPE_DEF_HEADER__
#define __TRIGGER_TYPE_DEF_HEADER__

namespace BLOCKMAN
{
	enum class TriggerType
	{
		INVALID = -1,
		CLICK,
		IN_RANGE,
		TIME_INTERVAL,
		BLOCK_ADJACENT,
		TYPE_COUNT
	};

	constexpr const char* triggerTypeNames[] = {
		"CLICK",
		"IN_RANGE",
		"TIME_INTERVAL",
		"BLOCK_ADJACENT"
	};

	static_assert(sizeof(triggerTypeNames) / sizeof(const char*) == static_cast<int>(TriggerType::TYPE_COUNT), 
		"number of TriggerType does not match the size of triggerTypeNames");

	enum class TriggerActionType
	{
		INVALID = -2,
		REMOTE,
		EXPLODE,
		SHOW_MESSAGE,
		TYPE_COUNT
	};

	constexpr const char* triggerActionTypeNames[] = {
		"EXPLODE",
		"SHOW_MESSAGE"
	};

	static_assert(sizeof(triggerActionTypeNames) / sizeof(const char*) == static_cast<int>(TriggerActionType::TYPE_COUNT),
		"number of TriggerActionType does not match the size of triggerActionTypeNames");
}

#endif // !__TRIGGER_TYPE_DEF_HEADER__
