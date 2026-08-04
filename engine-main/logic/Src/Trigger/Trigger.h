#ifndef __TRIGGER_HEADER__
#define __TRIGGER_HEADER__

#include "Core.h"
#include "Trigger/TriggerTypeDef.h"
#include "Trigger/TriggerAction.h"

namespace BLOCKMAN
{
    class Trigger : public LORD::ObjectAlloc
    {
	private:
		TriggerActionList m_actions;
		bool m_hasLocalAction = false;
		bool m_hasRemoteAction = false;

	public:
		virtual ~Trigger() = default;

		bool hasLocalAction() const
		{
			return m_hasLocalAction;
		}

		bool hasRemoteAction() const
		{
			return m_hasRemoteAction;
		}

		const TriggerActionList& getActions() const
		{
			return m_actions;
		}

		void addAction(const TriggerActionPtr& action)
		{
			m_actions.push_back(action);
			if (action->isRemoteAction())
			{
				m_hasRemoteAction = true;
			}
			else
			{
				m_hasLocalAction = true;
			}
		}

		virtual TriggerType getType() const = 0;
    };

	using TriggerPtr = std::shared_ptr<Trigger>;
	using TriggerList = LORD::vector<TriggerPtr>::type;
}

#endif

