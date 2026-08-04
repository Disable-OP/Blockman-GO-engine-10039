/********************************************************************
filename: 	Chunk.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __CHUNK_HEADER__
#define __CHUNK_HEADER__

#include "BM_Container_def.h"
#include "World/Section.h"
#include "Util/ChunkCoordIntPair.h"
#include "BM_TypeDef.h"
#include "Nbt/NbtBindingMacros.h"
#include "TileEntity/TileEntitys.h"

namespace BLOCKMAN
{

class TileEntity;
class World;
class Random;
class BiomeGenBase;
class WorldChunkManager;

class Chunk : public ObjectAlloc, public std::enable_shared_from_this<Chunk>
{
	NBTB_DECLARE_FRIEND(Chunk);
	typedef map<ChunkPosition, TileEntity*>::type ChunkTileEntityMap;

public:
	static const ChunkPtr BOUNDARY_CHUNK;
	static const int INVALID_PRECIPITATION = -999;
	/** A map, similar to heightMap, that tracks how far down precipitation can fall. */
	int* m_precipitationHeightMap = nullptr;
	
	/** Whether or not this Chunk is currently loaded into the World */
	bool m_isChunkLoaded = false;
	
	/** Reference to the World object. */
	World* m_pWorld = nullptr;
	
	int m_heightMap[256] = {0};
	
	/** The x z coordinate of the chunk. */
	int m_posX = 0;
	int m_posZ = 0;
	
	/** A Map of ChunkPositions to TileEntities in this chunk */
	ChunkTileEntityMap m_tileEntityMap;
	
	/** Array of Lists containing the entities in this Chunk. Each List represents a 16 block subchunk.	*/
	EntityArrays m_entities;
	
	/** Boolean value indicating if the terrain is populated. */
	bool m_isTerrainPopulated = false;
	
	/** Set to true if the chunk has been modified and needs to be updated internally. */
	bool m_isModified = false;
	
	/** Set to true if the chunk has been prepared for render */
	bool m_isPrepared = false;

	/** Whether this Chunk has any Entities and thus requires saving on every tick */
	bool m_hasEntities = false;
	
	/** The time according to World.worldTime when this chunk was last saved */
	i64 m_lastSaveTime = 0;
	
	/** Updates to this chunk will not be sent to clients if this is false. This field is set to true the first time the
	* chunk is sent to a client, and never set to false. */
	bool m_sendUpdates = false;
	
	/** the cumulative number of ticks players have been in this chunk */
	i64 m_inhabitedTime = 0;
	
	/** Used to store block IDs, block MSBs, Sky-light maps, Block-light maps, and metadata. Each entry corresponds to a
	* logical segment of 16x16x16 blocks, stacked vertically. */
	Section* m_pSections[16] = {nullptr};
	
	/** Contains a 16x16 mapping on the X/Z plane of the biome ID to which each colum belongs.*/
	i8 m_biomeArray[256] = {0};

protected:
	virtual Section* CreateSection(int x, int z, int yBase);
	/** the constructor function */
	void initialize();

public:
	Chunk() : Chunk(NULL, 0, 0) {}
	Chunk(World* pWorld, int x, int z);
	virtual ~Chunk();

	/** set the chunk with data(id, meta) */
	void setData(i8* pdata, int len);

	/** Checks whether the chunk is at the X/Z location specified */
	bool isAtLocation(int x, int z) const {	return x == m_posX && z == m_posZ; }
	
	/** Returns the value in the height map at this x, z coordinate in the chunk */
	virtual int getHeightValue(int x, int z) const { return m_heightMap[z << 4 | x]; }
	void resetPrecipitation(int x, int z) { m_precipitationHeightMap[z << 4 | x] = INVALID_PRECIPITATION; }
	
	/** Returns the topmost ExtendedBlockStorage instance for this Chunk that actually contains a block. */
	int getTopFilledSegment();
	
	/** for debug 15-bit visibility. added by zhouyou. */
	void setSectionXZ();

	/** Returns the ExtendedBlockStorage array for this Chunk.*/
	Section** getSectionArray() { return m_pSections; }
	
	//add by maxicheng
	int updateHeightMap(const BlockPos& pos);

	/** Generates the height map for a chunk from scratch */
	virtual void generateHeightMap();

	/** Return the light opacity of the block. */
	virtual int getBlockLightOpacity(const BlockPos& pos);

	/** Return the ID of a block in the chunk. */
	virtual int getBlockID(const BlockPos& pos);

	/** Return the metadata corresponding to the given coordinates inside a chunk. */
	virtual int getBlockMeta(const BlockPos& pos);

	/** Return the ID and meta of a block in the chunk. id is high 16 bits. and meta is low 16 bits. */
	virtual int getBlockIDAndMeta(const BlockPos& pos);

	/** Sets a blockID of a position within a chunk with metadata. Args: x, y, z, blockID, metadata */
	virtual bool setBlockIDAndMeta(const BlockPos& pos, int blockID, int meta, bool immediate = true);

	/** Sets a large size of blocks */
	virtual bool setBlocks(const BlockModifys& modifys);

	/** Set the metadata of a block in the chunk */
	virtual bool setBlockMeta(const BlockPos& pos, int meta);

	virtual bool setBlockIDAndMetaWithoutSync(const BlockPos& pos, int blockID, int meta) { return setBlockIDAndMeta(pos, blockID, meta); };

	/** Set the metadata of a block in the chunk */
	virtual bool setBlockMetaWithoutSync(const BlockPos& pos, int meta) { return setBlockMeta(pos, meta); };
	
	/** Adds an entity to the chunk. Args: entity */
	void addEntity(Entity* pEntity);
	
	/** removes entity using its y chunk coordinate as its index */
	void removeEntity(Entity* pEntity);
	
	/** Removes entity at the specified index from the entity array. */
	void removeEntityAtIndex(Entity* pEntity, int index);
	
	/** Fills the given list of all entities that intersect within the given bounding box that aren't the passed entity */
	virtual void getEntitiesWithinAABBForEntity(Entity* pEntity, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer = false);
	virtual void getEntitiesWithinAABB(ui32 entityType, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer = false);

	/** Returns whether is not a block above this one blocking sight to the sky (done via checking against the heightmap) */
	virtual bool canBlockSeeTheSky(const BlockPos& pos)	{ return pos.y >= m_heightMap[pos.z << 4 | pos.x]; }

	/** Gets the TileEntity for a given block in this chunk */
	virtual TileEntity* getChunkBlockTileEntity(const BlockPos& pos);

	/** Adds a TileEntity to a chunk */
	virtual void addTileEntity(TileEntity* pTileEntity);

	/** Sets the TileEntity for a given block in this chunk	*/
	virtual void setChunkBlockTileEntity(const BlockPos& pos, TileEntity* pTileEntity);

	/** Removes the TileEntity for a given block in this chunk */
	virtual void removeChunkBlockTileEntity(const BlockPos& pos);

	/** Called when this Chunk is loaded by the ChunkProvider */
	virtual void onChunkLoad();

	/** Called when this Chunk is unloaded by the ChunkProvider	*/
	virtual void onChunkUnload();

	/** Sets the isModified flag for this Chunk */
	virtual void setChunkModified()	{ m_isModified = true;	}
	
	/** Returns true if this Chunk needs to be saved */
	virtual bool needsSaving(bool par1);

	virtual Random* getRandomWithSeed(i64 par1);

	virtual bool isNonexistent() const { return false; }

	/** Gets the height to which rain/snow will fall. Calculates it if not already stored. */
	int getPrecipitationHeight(int x, int z);
	
	/** Gets a ChunkCoordIntPair representing the Chunk's position. */
	ChunkCoordIntPair getChunkCoordIntPair() { return ChunkCoordIntPair(m_posX, m_posZ); }

	/** Returns whether the sections containing levels (in blocks) from arg 1 to arg 2 are fully empty (true) or not (false). */
	virtual bool getAreLevelsEmpty(int x, int z);

	/** Initialise this chunk with new binary data */
	void fillChunk(i8* data, int par2, int par3, bool par4);
	
	/** This method retrieves the biome at a set of coordinates */
	BiomeGenBase* getBiomeGenForWorldCoords(int x, int z, WorldChunkManager* pWorldChunkManager);

	/** Returns an array containing a 16x16 mapping on the X/Z of block positions in this Chunk to biome IDs. */
	i8* getBiomeArray() { return m_biomeArray; }

	BlockPos toAbsoluteBlockPos(const BlockPos& position) const
	{
		return { position.x + (m_posX << 4), position.y, position.z + (m_posZ << 4) };
	}

	/** all the interface functions to implment by derived class. */
	/** Generates the initial skylight map for the chunk upon generation or load. */
	virtual void generateSkylightMap() { generateHeightMap(); }
	/** Generates the initial skylight and blocklight in pos. */
	virtual void relightBlockLight(const BlockPos& pos, int blockID, int height) {}
	/** Return if the gap-lighting is updated. */
	virtual bool gapLightingUpdated() { return false; }
	/** Return if the chunk need rebuild all. */
	virtual bool needRebuildAll() { return false; }
	/** Checks whether skylight needs updated; if it does, calls updateSkylight_do */
	virtual void updateSkylight() {}
	virtual void updateBlocklight() {}
	/** reBuild the visual graph*/
	virtual void reBuildVisibility(int idx) {}
	virtual void resetRelightChecks() {}
	/** return the minimum height for this chunk. */
	virtual int  getMinimumHeight() { return 0; }
	/** Gets the amount of light saved in this block (doesn't adjust for daylight) */
	virtual int  getSavedLightValue(int light, const BlockPos& pos) { return 0; }
	/** Gets the amount of light on a block taking into account sunlight */
	virtual int  getBlockLightValue(const BlockPos& pos, int l) { return 0; }
	/** Sets the light value at the coordinate. If lightEnum is set to sky it sets it in the skylightmap and if its a block then into the blocklightmap.*/
	virtual void setLightValue(int lightValue, const BlockPos& pos, int l) {}

private:
	void parseTileEntitiesTag(const NbtTagEvent &event, NbtParser &parser);
	void generateTileEntitiesTag(const String &tagName, NbtGenerator &generator) const;
};

NBTB_BEGIN(Chunk, void)
NBTB_ALIAS(m_posX, "xPos", NBTB_SIMPLE(TAG_INT))
NBTB_ALIAS(m_posZ, "zPos", NBTB_SIMPLE(TAG_INT))
NBTB_ALIAS(m_isTerrainPopulated, "TerrainPopulated", NBTB_SIMPLE(TAG_BYTE))
NBTB_ALIAS(m_inhabitedTime, "InhabitedTime", NBTB_SIMPLE(TAG_LONG))
NBTB_ALIAS(m_biomeArray, "Biomes", NBTB_SIMPLE(TAG_BYTE_ARRAY))
NBTB_ALIAS(m_heightMap, "HeightMap", NBTB_SIMPLE(TAG_INT_ARRAY))
NBTB_ALIAS(m_pSections, "Sections", NBTB_LIST(NBTB_COMPOUND(Section)))
NBTB_CUSTOM_HANDLERS("Entities",
	[](Chunk *chunk, const NbtTagEvent &_event, NbtParser &parser)
	{
		parser.jumpToEndEvent();
	},
	[](const Chunk *chunk, const String &tagName, NbtGenerator &generator)
	{
	}
)
NBTB_CUSTOM_HANDLERS("TileEntities", &Chunk::parseTileEntitiesTag, &Chunk::generateTileEntitiesTag)
NBTB_END

/*
 * Represent that chunk does not exist at certain position. This is different from nullptr,
 * nullptr means that the chunk is not loaded, we don't know chunk exists or not.
 */
class NonexistentChunk : public Chunk
{
protected:
	virtual Section* CreateSection(int x, int z, int yBase) override { throw std::logic_error("not implemented"); }

public:
	NonexistentChunk(int x, int z) : Chunk(nullptr, x, z)
	{
		m_isChunkLoaded = true;
		m_isTerrainPopulated = true;
		m_isPrepared = true;
		m_precipitationHeightMap = nullptr;
	}
	virtual int getHeightValue(int x, int z) const override { return 0; }
	virtual void generateHeightMap() override {}
	virtual int getBlockLightOpacity(const BlockPos& pos) override { return 255; }
	virtual int getBlockID(const BlockPos& pos) override { return 0; }
	virtual int getBlockMeta(const BlockPos& pos) override { return 0; }
	virtual int getBlockIDAndMeta(const BlockPos& pos) override { return 0; }
	virtual bool setBlockIDAndMeta(const BlockPos& pos, int blockID, int meta, bool immediate = true) override { return true; }
	virtual bool setBlockMeta(const BlockPos& pos, int meta) override { return false; }
	virtual void getEntitiesWithinAABBForEntity(Entity* pEntity, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer) override {}
	virtual void getEntitiesWithinAABB(ui32 entityType, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer) override {}
	virtual bool canBlockSeeTheSky(const BlockPos& pos) override { return false; }
	virtual TileEntity* getChunkBlockTileEntity(const BlockPos& pos) override { throw std::logic_error("not implemented"); }
	virtual void addTileEntity(TileEntity* pTileEntity) override { throw std::logic_error("not implemented"); }
	virtual void setChunkBlockTileEntity(const BlockPos& pos, TileEntity* pTileEntity) override { throw std::logic_error("not implemented"); }
	virtual void removeChunkBlockTileEntity(const BlockPos& pos) override { throw std::logic_error("not implemented"); }
	virtual void onChunkLoad() override {}
	virtual void onChunkUnload() override {}
	virtual void setChunkModified() override { throw std::logic_error("not implemented"); }
	virtual bool needsSaving(bool par1) override { return false; }
	virtual Random* getRandomWithSeed(i64 par1) override { throw std::logic_error("not implemented"); }
	virtual bool getAreLevelsEmpty(int x, int z) override { return true; }
	virtual void generateSkylightMap() override {}
	virtual void relightBlockLight(const BlockPos& pos, int blockID, int height) override {}
	virtual bool gapLightingUpdated() override { return false; }
	virtual void updateSkylight() override {}
	virtual void reBuildVisibility(int idx) override {}
	virtual void resetRelightChecks() override {}
	virtual int  getMinimumHeight() override { return 0; }
	virtual int  getSavedLightValue(int light, const BlockPos& pos) override { return 0; }
	virtual int  getBlockLightValue(const BlockPos& pos, int l) override { return 0; }
	virtual void setLightValue(int lightValue, const BlockPos& pos, int l) override {}
	virtual bool isNonexistent() const override 
	{
		return true;
	}
};

}

#endif
