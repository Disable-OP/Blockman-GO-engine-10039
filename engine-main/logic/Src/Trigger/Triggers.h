#ifndef __TRIGGERS_HEADER__
#define __TRIGGERS_HEADER__

#include "Trigger/TriggerHelper.h"
#include "Util/RapidJsonObject.h"

namespace BLOCKMAN
{
	class EntityPlayer;
	class World;

	BIND_TRIGGER_TYPE(CLICK, TriggerClick);
	class TriggerClick : public Trigger
	{
	private:
		static constexpr int NO_ITEM = 0;
		bool m_noRestriction = false;
		LORD::set<int>::type m_itemIds;

	public:
		static TriggerPtr parse(const RapidJsonObject& json)
		{
			auto ret = LORD::make_shared<TriggerClick>();
			if (!json.IsArray("itemIds"))
			{
				ret->m_noRestriction = true;
				return ret;
			}
			for (const auto& value : json.GetArray("itemIds"))
			{
				ret->m_itemIds.insert(value.GetInt());
			}
			return ret;
		}

		virtual TriggerType getType() const override
		{
			return TriggerType::CLICK;
		}

		TriggerActivatorList checkCondition(EntityPlayer* player);
	};

	BIND_TRIGGER_TYPE(IN_RANGE, TriggerInRange);
	class TriggerInRange : public Trigger
	{
	public:
		struct Data
		{};

		static TriggerPtr parse(const RapidJsonObject& json)
		{
			return LORD::make_shared<TriggerInRange>();
		}

		virtual TriggerType getType() const override
		{
			return TriggerType::IN_RANGE;
		}

		TriggerActivatorList checkCondition(Data& data, World* world, const BlockPos& pos);
	};

	BIND_TRIGGER_TYPE(TIME_INTERVAL, TriggerTimeInterval);
	class TriggerTimeInterval : public Trigger
	{
	public:
		struct Data
		{};

		static TriggerPtr parse(const RapidJsonObject& json)
		{
			return LORD::make_shared<TriggerTimeInterval>();
		}

		virtual TriggerType getType() const override
		{
			return TriggerType::TIME_INTERVAL;
		}

		TriggerActivatorList checkCondition(Data& data);
	};

	BIND_TRIGGER_TYPE(BLOCK_ADJACENT, TriggerBlockAdjacent);
	class TriggerBlockAdjacent : public Trigger
	{
	public:
		static TriggerPtr parse(const RapidJsonObject& json)
		{
			return LORD::make_shared<TriggerBlockAdjacent>();
		}

		virtual TriggerType getType() const override
		{
			return TriggerType::BLOCK_ADJACENT;
		}

		TriggerActivatorList checkCondition(World*, const BlockPos& pos);
	};
}

#endif // !__TRIGGER_CONDITIONS_HEADER__
