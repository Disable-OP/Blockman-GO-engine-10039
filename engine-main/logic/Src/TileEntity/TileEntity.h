/********************************************************************
filename: 	TileEntity.h
file path:	dev\client\Src\Blockman\BlockEntity

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __TILE_ENTITY_HEADER__
#define __TILE_ENTITY_HEADER__

#include "RTTI.h"
#include "BM_TypeDef.h"
#include "Nbt/NbtBindingMacros.h"

namespace BLOCKMAN
{

class World;
class NBTTagCompound;
class Block;

class TileEntity : public ObjectAlloc
{
	RTTI_DECLARE(TileEntity);
	NBTB_DECLARE_FRIEND(TileEntity);
public:
	TileEntity(int id);
	virtual ~TileEntity();

	static void initialize();
	/** Creates a new entity and loads its data from the specified NBT. */
	static TileEntity* createAndLoadEntity(NBTTagCompound* par0NBTTagCompound);
	static int getSuperClassID(int type);

	World* getWorldObj() { return worldObj; }
	bool hasWorldObj() { return worldObj != NULL; }
	void setWorldObj(World* pWorld) { worldObj = pWorld; }

	/** Reads a tile entity from NBT. */
	virtual void readFromNBT(NBTTagCompound* pNBT);
	/** Writes a tile entity to NBT. */
	virtual void writeToNBT(NBTTagCompound* pNBT);
	/** Allows the entity to update its state. Overridden in most subclasses */
	virtual void updateEntity() {}
	/** Called when an the contents of an Inventory change, usually	*/
	virtual void onInventoryChanged();
	/** Causes the TileEntity to reset all it's cached values for it's container block, blockID, metaData and in the case of chests */
	virtual void updateContainingBlockInfo();

	/** Returns block data at the location of this entity (client-only). */
	int getBlockMetadata();
	/** Returns the square of the distance between this entity and the passed in coordinates. */
	float getDistanceFrom(const Vector3& vec);
	float getMaxRenderDistanceSquared() { return 4096.f; }
	/** Gets the block type at the location of this entity (client-only). */
	Block* getBlock();
	/** Overriden in a sign to provide the text.*/
	// Packet getDescriptionPacket() { return NULL; }
	/** returns true if tile entity is invalid, false otherwise */
	bool isInvalid() { return tileEntityInvalid; }
	virtual void invalidate() { tileEntityInvalid = true; }
	void validate() { tileEntityInvalid = false; }
	/** Called when a client event is received with the event number and argument, see World.sendClientEvent */
	bool receiveClientEvent(int id, int type) { return false; }

	const String& getClassName() const;
	static TileEntity* deserializeTileEntity(const String& id, NbtParser& parser, bool startWithCompoundTag = true);
	bool serializeTileEntity(NbtGenerator& generator) const;
	bool serializable() const;

public:
	/** The x coordinate of the tile entity. */
	BlockPos m_pos;

	int blockMetadata = 0;
	Block* blockType = nullptr;

protected:
	typedef map<int, String>::type IDTileEntityType;
	typedef map<String, int>::type NameTileEntityType;
	static IDTileEntityType tileEntityIds;
	static NameTileEntityType tileEntityNames;
	/** The reference to the world. */
	World* worldObj = nullptr;
	bool tileEntityInvalid = false;
	int m_id = 0;
};

NBTB_BEGIN(TileEntity, void)
NBTB_ALIAS(m_id, "id", NBTB_SIMPLE(TAG_STRING), NBTB_CONVERTERS([](int& id, const String& val) { id = TileEntity::tileEntityNames[val]; }, [](const int& id) { return TileEntity::tileEntityIds[id]; }))
NBTB_ALIAS(m_pos, "x", NBTB_SIMPLE(TAG_INT), NBTB_CONVERTERS([](BlockPos& pos, int val) { pos.x = val; }, [](const BlockPos& pos) { return pos.x; }))
NBTB_ALIAS(m_pos, "y", NBTB_SIMPLE(TAG_INT), NBTB_CONVERTERS([](BlockPos& pos, int val) { pos.y = val; }, [](const BlockPos& pos) { return pos.y; }))
NBTB_ALIAS(m_pos, "z", NBTB_SIMPLE(TAG_INT), NBTB_CONVERTERS([](BlockPos& pos, int val) { pos.z = val; }, [](const BlockPos& pos) { return pos.z; }))
NBTB_END

}

#endif
