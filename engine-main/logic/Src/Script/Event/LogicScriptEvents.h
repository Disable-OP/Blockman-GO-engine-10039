#pragma once
#include "Script/Event/ScriptEvent.h"
#include "Script/VarProxy/LogicVarProxy.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "TileEntity/TileEntityInventory.h"

namespace SCRIPT_EVENT
{
	//Entity hit event
	// EntityId
	// hitPos
	// PlayerId
	struct EntityHitEvent : public ScriptEvent<EntityHitEvent, int, Vector3, int> {};

	//hurt Entity hurt from hurtvalue
	struct EntityAttackedEvent :public ScriptEvent<EntityAttackedEvent, int, int, const char*, float> {};

	//tnt explode
	struct BlockTNTExplodeEvent : public ScriptEvent<BlockTNTExplodeEvent, int, Vector3, ExplosionAttr*> {};

	//tnt explode
	struct BlockTNTExplodeBreakBlockEvent : public ScriptEvent<BlockTNTExplodeBreakBlockEvent, Vector3i, int> {};

	//open the door
	struct PlayerOpenDoorEvent : public ScriptEvent<PlayerOpenDoorEvent, int, Vector3i> {};

	// entity item spawn
	struct EntityItemSpawnEvent : public ScriptEvent<EntityItemSpawnEvent, ui32, ui32, String> {};

	// potion convert glass bottle
	struct PotionConvertGlassBottleEvent : public ScriptEvent<PotionConvertGlassBottleEvent, ui32, ui32> {};

	// aircraft move
	struct AircraftMoveEvent : public ScriptEvent<AircraftMoveEvent, int, Vector3>{};

	// creature attack player
	struct CreatureAttackPlayerEvent : public  ScriptEvent<CreatureAttackPlayerEvent, ui32, ui32> {};

	// creature attack creature
	struct CreatureAttackCreatureEvent : public  ScriptEvent<CreatureAttackCreatureEvent, ui32, ui32> {};
	
	// use itemseeds 
	struct TryToUseItemSeedsEvent : public ScriptEvent<TryToUseItemSeedsEvent, int, Vector3> {};

	// click blocksign event
	struct OnClickBlockSignEvent : public ScriptEvent<OnClickBlockSignEvent, int, Vector3i> {};

	//skill attack
	struct ItemSkillAttackEvent : public  ScriptEvent<ItemSkillAttackEvent, int, int, Vector3> {};

	// consume block
	struct PlacingConsumeBlockEvent: public  ScriptEvent<PlacingConsumeBlockEvent, ui32, ui32>{};

	// block switch event:entityId, press down or up, blockId, pos, return true means can handle default neighbor event
	struct BlockSwitchEvent :public ScriptEvent<BlockSwitchEvent, int, bool, int, Vector3i> {};

	// block empty move
	struct BlockmanEmptyMoveEvent : public ScriptEvent<BlockmanEmptyMoveEvent, int, Vector3>{};
	
	// block pressure weight event: entityId power, blockId, pos, 
	struct BlockPressurePlateWeightedEvent :public ScriptEvent<BlockPressurePlateWeightedEvent, int, int, int, Vector3i> {}; 

	// place item building
	struct PlaceItemBuildingEvent : public ScriptEvent<PlaceItemBuildingEvent, ui64, int, int, Vector3, int, Vector3, Vector3> {};

	// place item house
	struct PlaceItemHouseEvent : public ScriptEvent<PlaceItemHouseEvent, ui64, String, Vector3, Vector3> {};
	
	// remove item building event
	struct RemoveItemBuildingEvent : public ScriptEvent<RemoveItemBuildingEvent, ui64, i32> {};

	// use item teleport scoll
	struct UseItemTeleportEvent : public ScriptEvent<UseItemTeleportEvent, ui32, int> {};

	// player's ranch order ask for help 
	struct PlayerAskForHelpRanchOrderEvent : public ScriptEvent<PlayerAskForHelpRanchOrderEvent, ui64, i32, i32> {};

	// player put into ranch order cargo event
	struct PlayerPutIntoRanchOrderCargoEvent : public ScriptEvent<PlayerPutIntoRanchOrderCargoEvent, ui64, i32, i32> {};

	// player receive ranch order cargo event
	struct PlayerReceiveRanchOrderCargoEvent : public ScriptEvent<PlayerReceiveRanchOrderCargoEvent, ui64, i32, i32> {};

	// player refresh ranch order event
	struct PlayerRefreshRanchOrderEvent : public ScriptEvent<PlayerRefreshRanchOrderEvent, ui64, i32, i32> {};

	// blockore dropped
	struct BlockOreDroppedEvent : public ScriptEvent<BlockOreDroppedEvent, int> {};

	// itemPickaxe Harvest
	struct ItemPickaxeCanHarvestEvent : public ScriptEvent<ItemPickaxeCanHarvestEvent, int> {};

	//ChestOpen Special Handle
	struct ChestOpenSpHandleEvent :public ScriptEvent<ChestOpenSpHandleEvent, Vector3i, int> {};

	// block leaves break
	struct BlockLeavesBreakEvent : public ScriptEvent<BlockLeavesBreakEvent, int> {};

	// block stationary set not stationary
	struct BlockStationaryNotStationaryEvent : public ScriptEvent<BlockStationaryNotStationaryEvent, int> {};

	//Entity hit with current item event
	struct HitEntityWithCurrentItem : public ScriptEvent<HitEntityWithCurrentItem, int, int, int, int> {};

	// entity death drop item
	struct EntityDeathDropItemEvent : public ScriptEvent<EntityDeathDropItemEvent, int, String> {};

	// get gather num
	struct BirdSimulatorGatherEvent : public ScriptEvent<BirdSimulatorGatherEvent, ui64, IntProxy*, Vector3i> {};

	// add gather score
	struct BirdSimulatorAddGatherScoreEvent : public ScriptEvent<BirdSimulatorAddGatherScoreEvent, ui64, i64, int, Vector3i, int, int, IntProxy*> {};

	struct BirdSimulatorNestOperationEvent : public ScriptEvent<BirdSimulatorNestOperationEvent, ui64, i32,  Vector3i> {};

}
