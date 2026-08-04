#include "TriggerConfigParser.h"
#include "Trigger/Trigger.h"
#include "Util/RapidJsonObject.h"

namespace BLOCKMAN
{
	TriggerActionPtr TriggerActionConfigParser::parse(const RapidJsonObject& json)
	{
		const char * type = json.GetString("type");
		auto iter = m_parsers.find(type);
		if (iter == m_parsers.end())
		{
			LordLogWarning("Ignored unknown trigger action type: %s", type);
			return nullptr;
		}
		return iter->second(json);
	}

	TriggerPtr TriggerConfigParser::parse(const RapidJsonObject& json)
	{
		const char* type = json.GetString("type");
		auto iter = m_parsers.find(type);
		if (iter == m_parsers.end())
		{
			LordLogWarning("Ignored unknown trigger type: %s", type);
			return nullptr;
		}
		auto trigger = iter->second(json);
		for (const auto& actionJson : json.GetArray("actions"))
		{
			if (auto action = TriggerActionConfigParser::Instance()->parse(actionJson.GetObject()))
			{
				trigger->addAction(action);
			}
		}
		return trigger;
	}
}