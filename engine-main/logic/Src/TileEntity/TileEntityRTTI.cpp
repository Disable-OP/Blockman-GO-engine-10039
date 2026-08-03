#include "RTTI.h"
#include "TileEntity/TileEntity.h"
#include "TileEntity/TileEntitys.h"
#include "TileEntity/TileEntityInventory.h"

namespace BLOCKMAN
{

RTTI_IMPLEMENT_BASE(TileEntity,				TILE_ENTITY_BASE)
RTTI_IMPLEMENT(TileEntityChest,				TILE_ENTITY_CHEST,			TileEntity)
RTTI_IMPLEMENT(TileEntityFurnace,			TILE_ENTITY_FURNACE,		TileEntity)
RTTI_IMPLEMENT(TileEntityDispenser,			TILE_ENTITY_TRAP,			TileEntity)
RTTI_IMPLEMENT(TileEntityDropper,			TILE_ENTITY_DROPPER,		TileEntityDispenser)
RTTI_IMPLEMENT(TileEntityBrewingStand,		TILE_ENTITY_CAULDRON,		TileEntity)
RTTI_IMPLEMENT(TileEntityBeacon,			TILE_ENTITY_BEACON,			TileEntity)
RTTI_IMPLEMENT(TileEntityHopper,			TILE_ENTITY_HOPPER,			TileEntity)
RTTI_IMPLEMENT(TileEntityEnderChest,		TILE_ENTITY_ENDERCHEST,		TileEntity)
RTTI_IMPLEMENT(TileEntityPiston,			TILE_ENTITY_PISTON,			TileEntity)
RTTI_IMPLEMENT(TileEntityComparator,		TILE_ENTITY_COMPARATOR,		TileEntity)
RTTI_IMPLEMENT(TileEntitySkull,				TILE_ENTITY_SKULL,			TileEntity)
RTTI_IMPLEMENT(TileEntityRecordPlayer,		TILE_ENTITY_RECORDPLAYER,	TileEntity)
RTTI_IMPLEMENT(TileEntitySign,				TILE_ENTITY_SIGN,			TileEntity)
RTTI_IMPLEMENT(TileEntityMobSpawner,		TILE_ENTITY_MOBSWPAWER,		TileEntity)
RTTI_IMPLEMENT(TileEntityNote,				TILE_ENTITY_MUSIC,			TileEntity)
RTTI_IMPLEMENT(TileEntityEnchantmentTable,	TILE_ENTITY_ENCHANTTABLE,	TileEntity)
RTTI_IMPLEMENT(TileEntityEndPortal,			TILE_ENTITY_AIRPORTAL,		TileEntity)
RTTI_IMPLEMENT(TileEntityCommandBlock,		TILE_ENTITY_CONTROL,		TileEntity)
RTTI_IMPLEMENT(TileEntityDaylightDetector,	TILE_ENTITY_DLDETECTOR,		TileEntity)

}