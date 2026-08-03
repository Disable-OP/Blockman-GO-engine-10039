#ifndef __TRIGGER_MANAGERS_HEADER__
#define __TRIGGER_MANAGERS_HEADER__

#include "Core.h"
#include "Trigger/TriggerManager.h"
#include "Trigger/Triggers.h"

namespace BLOCKMAN
{
	class World;
	using BlockPos = LORD::Vector3i;

	namespace TRIGGER_MANAGER
	{
		class TriggerClickManager 
			: public LORD::Singleton<TriggerClickManager>
			, public TriggerManager
		{
		public:
			void checkTriggers(World* world, const BlockPos& pos, EntityPlayer* player)
			{
				checkTriggersImpl(world, pos, [=](TriggerPtr trigger) {
					return std::static_pointer_cast<TriggerClick>(trigger)->checkCondition(player);
				});
			}
		};

		class TriggerInRangeManager 
			: public LORD::Singleton<TriggerInRangeManager>
			, public TriggerManager
			, public TriggerDataManager<TriggerInRangeManager, TriggerInRange::Data>
		{
		public:
			void checkTriggers(World* world, const BlockPos& pos)
			{
				checkTriggersImpl(world, pos, [=](TriggerPtr trigger) {
					return std::static_pointer_cast<TriggerInRange>(trigger)->checkCondition(m_blockPosToTriggerData.at(pos), world, pos);
				});
			}

			void checkAllTriggers(World* world)
			{
				for (auto& pair : m_blockPosToTriggerData)
				{
					checkTriggers(world, pair.first);
				}
			}
		};

		class TriggerTimeIntervalManager 
			: public LORD::Singleton<TriggerTimeIntervalManager>
			, public TriggerManager
			, public TriggerDataManager<TriggerTimeIntervalManager, TriggerTimeInterval::Data>
		{
		public:
			void checkTriggers(World* world, const BlockPos& pos)
			{
				checkTriggersImpl(world, pos, [=](TriggerPtr trigger) {
					return std::static_pointer_cast<TriggerTimeInterval>(trigger)->checkCondition(m_blockPosToTriggerData.at(pos));
				});
			}

			void checkAllTriggers(World* world)
			{
				for (auto& pair : m_blockPosToTriggerData)
				{
					checkTriggers(world, pair.first);
				}
			}
		};

		class TriggerBlockAdjacentManager 
			: public LORD::Singleton<TriggerBlockAdjacentManager>
			, public TriggerManager
		{
		public:
			void checkTriggers(World* world, const BlockPos& pos)
			{
				checkTriggersImpl(world, pos, [=](TriggerPtr trigger) {
					return std::static_pointer_cast<TriggerBlockAdjacent>(trigger)->checkCondition(world, pos);
				});
			}
		};

		void createTriggerManagers();
		void destroyTriggerManagers();
		TriggerManager* getTriggerManager(TriggerType type);
		void notifyBlockChange(const BlockPos& pos, int oldId, int newId);
		void registerTriggers(int blockId, const TriggerList& triggers);
		void unregisterTriggers(int blockId);
	}

	using TRIGGER_MANAGER::TriggerClickManager;
	using TRIGGER_MANAGER::TriggerInRangeManager;
	using TRIGGER_MANAGER::TriggerTimeIntervalManager;
	using TRIGGER_MANAGER::TriggerBlockAdjacentManager;
}

#endif // !__TRIGGER_MANAGER_HEADER__
