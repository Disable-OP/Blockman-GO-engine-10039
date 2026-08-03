#pragma once
#include "Event.h"

namespace BLOCKMAN
{
	class ItemStack;
	class EntityPlayer;
	class TileEntitySign;

	inline namespace PLAYER
	{
		struct AimingStateChangeEvent : Event<AimingStateChangeEvent, bool, int> {};
	}

	inline namespace LOGIC
	{
		struct SignTextsChangeEvent : Event<SignTextsChangeEvent, TileEntitySign*> {};
		struct OnUseItemSeedsEvent : Event<OnUseItemSeedsEvent, EntityPlayer*, BlockPos, int> {};
		struct OnNewCropsBlockDestroyEvent : Event<OnNewCropsBlockDestroyEvent, BlockPos> {};
		struct BirdSimulatorGatherEvent : Event<BirdSimulatorGatherEvent, ui64, i64, const BlockPos&> {};
		struct ShowBirdSimulatorMiniTipEvent : Event<ShowBirdSimulatorMiniTipEvent, i32, const String&> {};
	}
}
