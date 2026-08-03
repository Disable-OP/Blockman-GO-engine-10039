#ifndef __TRIGGER_ACTIONS_SERVER__
#define __TRIGGER_ACTIONS_SERVER__

#include "Trigger/TriggerActionHelper.h"
#include "Util/RapidJsonObject.h"

namespace BLOCKMAN
{
	BIND_TRIGGER_ACTION_TYPE(EXPLODE, TriggerActionExplode);
	class TriggerActionExplode : public TriggerAction
	{
	private:
		float m_power;

	public:
		static TriggerActionPtr parse(const RapidJsonObject& json)
		{
			return LORD::make_shared<TriggerActionExplode>(json.GetFloat("power", 4.0f));
		}

		TriggerActionExplode(float power)
			: m_power(power)
		{}
		virtual void doAction(const BlockPos& position, const TriggerActivatorList& activators) const override;
	};
}

#endif // !__TRIGGER_ACTIONS_SERVER__
