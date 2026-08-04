/********************************************************************
filename: 	Section.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __SECTION_HEADER__
#define __SECTION_HEADER__

#include "BM_TypeDef.h"
#include "Nbt/NbtBindingMacros.h"
#include "World/NibbleArray.h"

namespace BLOCKMAN
{

class SetVisibility;

struct BlockModifyInfo
{
	BlockPos	pos;
	ui16		id;
	ui16		meta;
};

typedef vector<BlockModifyInfo>::type BlockModifys;

class Section : public ObjectAlloc
{
	NBTB_DECLARE_FRIEND(Section);
public:
	Section(int yBase = 0);
	virtual ~Section() = default;

	// Returns the block ID for a location in a chunk.
	int getBlockID(const BlockPos& pos);

	// Sets the block ID for a location in a chunk.
	virtual void setBlockID(const BlockPos& pos, int blockID);

	// Returns the metadata associated with the block at the given coordinates in this ExtendedBlockStorage.
	int getBlockMeta(const BlockPos& pos);

	// Sets the metadata of the Block at the given coordinates in this ExtendedBlockStorage to the given metadata.
	virtual void setBlockMeta(const BlockPos& pos, int blockMetaData);

	// Returns whether or not this block storage's Chunk is fully empty, based on its internal reference count.
	bool isEmpty();

	// Returns whether or not this block storage's Chunk will require random ticking, used to avoid looping through
	// random block ticks when there are no blocks that would randomly tick.
	bool getNeedsRandomTick();

	void removeInvalidBlocks();

	i8* getBlockLSBArray();

	NibbleArray* getMetadataArray();

	// Returns the Y location of this ExtendedBlockStorage.
	int getYLocation() const { return m_yBase; }
	int blockRefCount() const { return m_blockRefCount; }
	int tickRefCount() const { return m_tickRefCount;  }

	/** this is the interface for all the section (for client side about the light value) */
	// Sets the saved Sky-light value in the extended block storage structure.
	virtual void setExtSkylightValue(const BlockPos& pos, int skyLightValue){}
	// Gets the saved Sky-light value in the extended block storage structure.
	virtual int getExtSkylightValue(const BlockPos& pos) { return 0; }
	// Sets the saved Block-light value in the extended block storage structure.
	virtual void setExtBlocklightValue(const BlockPos& pos, int blockLightValue) {}
	// Gets the saved Block-light value in the extended block storage structure.
	virtual int getExtBlocklightValue(const BlockPos& pos) { return 0; }
	// rebuild the section's visibility-set.
	virtual void reBuildVisibility() {}
	// get the section's visibility-set
	virtual SetVisibility* getVisibility() { return NULL; }

	// for debug!.
	void setPosXZ(int x, int z) { m_posX = x; m_posZ = z; }

public:
	// Block array size: 16 * 16 * 16
	static const int BLOCK_ARRAY_SIZE = 4096;

protected:
	// Contains the bottom-most Y block represented by this BlockStorage. Typically a multiple of 16.
	int m_yBase = 0;
	int m_posX = 0;
	int m_posZ = 0;
	// A total count of the number of non-air blocks in this block storage's Chunk.
	int m_blockRefCount = 0;
	// A total count of the number of custom blocks in this block storage's Chunk.
	int m_customBlockRefCount = 0;
	// Contains the number of blocks in this block storage's parent chunk that require random ticking. Used to cull the
	// Chunk from random tick updates for performance reasons.
	int m_tickRefCount = 0;
	// Contains the least significant 8 bits of each block ID belonging to this block storage's parent Chunk.
	i8 m_blockLSBArray[BLOCK_ARRAY_SIZE] = {0};
	// Stores the metadata associated with blocks in this BlockStorage.
	NibbleArray m_blockMetadataArray;
	std::unique_ptr<NibbleArray> m_blockHSBArray = nullptr;

	void parseBlockHSBArrayNbt(const NbtTagEvent &event, NbtParser &parser);
	void generateBlockHSBArrayNbt(const String &tagName, NbtGenerator &generator) const;
};

NBTB_BEGIN(Section, void)
NBTB_ALIAS(m_yBase, "Y", NBTB_SIMPLE(TAG_BYTE), NBTB_CONVERTERS([](int& mem, int val) { mem = val * 16; }, [](int mem) { return mem / 16; }))
NBTB_ALIAS(m_blockLSBArray, "Blocks", NBTB_SIMPLE(TAG_BYTE_ARRAY))
NBTB_ALIAS(m_blockMetadataArray, "Data", NBTB_SIMPLE(TAG_BYTE_ARRAY))
NBTB_CUSTOM_HANDLERS("Add", &Section::parseBlockHSBArrayNbt, &Section::generateBlockHSBArrayNbt)
NBTB_END

}

#endif
