#include "TriggerModuleServer.h"
#include "Trigger/TriggerConfigParser.h"
#include "Blockman/Trigger/TriggerNetworkServer.h"
#include "Trigger/TriggerManagers.h"
#include "Trigger/Triggers.h"
#include "Blockman/Trigger/TriggerActionsServer.h"
#include "Std/utility.h"
#include "Std/type_traits.h"

using namespace LORD;

namespace BLOCKMAN
{
	template<typename T, T ... numbers>
	static void registerTriggerActions(std::integer_sequence<T, numbers ...>)
	{
		vector<TriggerActionConfigParser::Parser>::type parsers
			= { &BindedTriggerActionType<static_cast<TriggerActionType>(numbers)>::parse ... };
		for (size_t i = 0; i < parsers.size(); ++i)
		{
			TriggerActionConfigParser::Instance()->addParser(triggerActionTypeNames[i], parsers[i]);
		}
	}

	template<typename T, T ... numbers>
	static void registerTriggers(std::integer_sequence<T, numbers ...>)
	{
		vector<TriggerConfigParser::Parser>::type parsers
			= { &BindedTriggerType<static_cast<TriggerType>(numbers)>::parse ... };
		for (size_t i = 0; i < parsers.size(); ++i)
		{
			TriggerConfigParser::Instance()->addParser(triggerTypeNames[i], parsers[i]);
		}
	}

	void TriggerModuleServer::initialize() const
	{
		LordNew TriggerNetworkServer;
		LordNew TriggerConfigParser;
		TRIGGER_MANAGER::createTriggerManagers();

		using TriggerActionTypeUnderlying = std::underlying_type_t<TriggerActionType>;
		registerTriggerActions(std::make_integer_sequence<TriggerActionTypeUnderlying,
			static_cast<TriggerActionTypeUnderlying>(TriggerActionType::TYPE_COUNT)>{});

		using TriggerTypeUnderlying = std::underlying_type_t<TriggerType>;
		registerTriggers(std::make_integer_sequence<TriggerTypeUnderlying,
			static_cast<TriggerTypeUnderlying>(TriggerType::TYPE_COUNT)>{});
	}

	void TriggerModuleServer::uninitialize() const
	{
		TRIGGER_MANAGER::destroyTriggerManagers();
		LordDelete(TriggerConfigParser::Instance());
		LordDelete(TriggerNetwork::Instance());
	}
}