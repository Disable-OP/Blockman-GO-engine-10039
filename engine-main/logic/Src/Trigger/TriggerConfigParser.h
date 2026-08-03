#ifndef __TRIGGER_CONFIG_PARSER_HEADER__
#define	__TRIGGER_CONFIG_PARSER_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class Trigger;
	class TriggerAction;
	class RapidJsonObject;

	using TriggerPtr = std::shared_ptr<Trigger>;
	using TriggerActionPtr = std::shared_ptr<TriggerAction>;

	class TriggerActionConfigParser : public LORD::ObjectAlloc, public LORD::Singleton<TriggerActionConfigParser>
	{
	public:
		using Parser = TriggerActionPtr(*)(const RapidJsonObject&);

	private:
		LORD::map<LORD::String, Parser>::type m_parsers;

	public:
		TriggerActionPtr parse(const RapidJsonObject& json);

		void addParser(const LORD::String& type, Parser parser)
		{
			m_parsers[type] = parser;
		}
	};

	class TriggerConfigParser : public LORD::ObjectAlloc, public LORD::Singleton<TriggerConfigParser>
	{
	public:
		using Parser = TriggerPtr(*)(const RapidJsonObject&);

	private:
		LORD::map<LORD::String, Parser>::type m_parsers;

	public:
		TriggerConfigParser()
		{
			LordNew TriggerActionConfigParser;
		}
		~TriggerConfigParser()
		{
			LordDelete(TriggerActionConfigParser::Instance());
		}
		TriggerPtr parse(const RapidJsonObject& json);

		void addParser(const LORD::String& type, Parser parser)
		{
			m_parsers[type] = parser;
		}
	};
}

#endif // !__TRIGGER_FACTORY_HEADER__
