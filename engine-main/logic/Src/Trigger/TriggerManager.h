#ifndef __TRIGGER_MANAGER__
#define __TRIGGER_MANAGER__

#include "Core.h"

namespace BLOCKMAN
{
	class World;
	class Trigger;
	class TriggerActivator;

	using TriggerPtr = std::shared_ptr<Trigger>;
	using TriggerList = LORD::vector<TriggerPtr>::type;
	using TriggerActivatorList = LORD::vector<TriggerActivator>::type;
	using BlockPos = LORD::Vector3i;

	namespace TRIGGER_MANAGER
	{
		class TriggerManager : public LORD::ObjectAlloc
		{
		protected:
			LORD::map<int, TriggerList>::type m_blockIdToTriggerList;

			void checkTriggersImpl(World* world, const BlockPos& pos, std::function<TriggerActivatorList(TriggerPtr)> checkCondition);

		public:
			void registerTrigger(int blockId, TriggerPtr trigger)
			{
				m_blockIdToTriggerList[blockId].push_back(trigger);
			}

			void unregisterTriggers(int blockId)
			{
				m_blockIdToTriggerList.erase(blockId);
			}

			bool blockHasTrigger(int blockId)
			{
				return m_blockIdToTriggerList.find(blockId) != m_blockIdToTriggerList.end();
			}

			const TriggerList& getTriggerList(int blockId) const
			{
				auto iter = m_blockIdToTriggerList.find(blockId);
				if (iter == m_blockIdToTriggerList.end())
				{
					static const TriggerList emptyList;
					return emptyList;
				}
				return iter->second;
			}
		};

		template<typename ActualTriggerManager, typename DataType>
		class TriggerDataManager
		{
		protected:
			LORD::Vector3iMap<DataType> m_blockPosToTriggerData;

		public:
			void onBlockChange(const BlockPos& pos, int oldId, int newId)
			{
				m_blockPosToTriggerData.erase(pos);
				if (static_cast<ActualTriggerManager*>(this)->blockHasTrigger(newId))
				{
					m_blockPosToTriggerData[pos];
				}
			}
		};
	}
}

#endif // !__TRIGGER_MANAGER_HELPER__
