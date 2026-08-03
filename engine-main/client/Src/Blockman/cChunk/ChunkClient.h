/********************************************************************
filename: 	ChunkClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-06-08
*********************************************************************/
#ifndef __CHUNK_CLIENT_HEADER__
#define __CHUNK_CLIENT_HEADER__

/** header files from common. */
#include "Chunk/Chunk.h"
#include "Functional/StaticCastOp.h"
#include "Functional/AssignOp.h"
#include "cWorld/SectionClient.h"

namespace BLOCKMAN
{
	
class SectionClient;
class SetVisibility;

class ChunkClient : public Chunk
{
public:
	/** Determines if the chunk is lit or not at a light value greater than 0.*/
	static bool isLit;

	/** Which columns need their skylightMaps updated. */
	i8* m_updateSkylightColumns = nullptr;
	
	/** Boolean value indicating if the visibility is populated. */
	bool m_isReBuildVisibility = false;
	
	/** Lowest value in the heightmap. */
	int m_heightMapMinimum = 0;

	/** Contains the current round-robin relight check index, and is implied as the relight check location as well. */
	int m_queuedLightChecks = 0;
		
	bool m_isSkyLightingUpdated = false;
	bool m_isBlockLightingUpdated = false;
	/** rebuild all the chunk's skylight, blocklight, visibility.*/
	bool m_needRebuildAll = false;

	int  m_updateCD = 0;

	void initialize2();

protected:
	virtual Section* CreateSection(int x, int z, int yBase);

public:
	ChunkClient() : ChunkClient(nullptr, 0, 0) {};
	ChunkClient(World* pWorld, int x, int z);
	virtual ~ChunkClient();

	SetVisibility* getSetVisibility(int idx);
	/** Runs delayed skylight updates. */
	void updateSkylight_do();
	/** Propagates a given sky-visible block's light value downward and upward to neighboring blocks as necessary. */
	void propagateSkylightOcclusion(int par1, int par2);
	/** Checks the height of a block next to a sky-visible block and schedules a lighting update as necessary. */
	void checkSkylightNeighborHeight(int x, int z, int maxVal);
	/** update the sky-light on the neighbor height from starty to  endy. */
	void updateSkylightNeighborHeight(int x, int z, int starty, int endy);
	/** Initiates the recalculation of both the block-light and sky-light for a given block inside a chunk. */
	void relightBlock(const BlockPos& pos);
	bool checkNeedRebuildAll();

	void reBuildVisibility();
	/** Resets the relight check index to 0 for this Chunk.	*/
	void resetRelightChecks() { m_queuedLightChecks = 0; }
	/** Called once-per-chunk-per-tick, and advances the round-robin relight check index per-storage-block by up to 8
	* blocks at a time. In a worst-case scenario, can potentially take up to 1.6 seconds, calculated via
	* (4096/(8*16))/20, to re-check all blocks in a chunk, which could explain both lagging light updates in certain
	* cases as well as Nether relight */
	void enqueueRelightChecks();

	/** implement virtual function from Chunk. */
	virtual bool setBlocks(const BlockModifys& modifys) override;
	virtual void generateSkylightMap() override;
	virtual void relightBlockLight(const BlockPos& pos, int blockID, int height) override;
	virtual bool gapLightingUpdated() override { return !m_isSkyLightingUpdated && !m_isBlockLightingUpdated; }
	virtual bool needRebuildAll() override { return m_needRebuildAll; }
	virtual void updateSkylight() override;
	virtual void updateBlocklight() override;
	virtual void reBuildVisibility(int idx) override;
	virtual int getMinimumHeight()  override { return m_heightMapMinimum; }
	virtual int getSavedLightValue(int light, const BlockPos& pos) override;
	virtual int getBlockLightValue(const BlockPos& pos, int l) override;
	virtual void setLightValue(int lightValue, const BlockPos& pos, int l) override;
	// virtual bool setBlocks(const BlockModifys& modifys) override;
};

NBTB_BEGIN(ChunkClient, Chunk)
NBTB_ALIAS(m_pSections, "Sections", NBTB_LIST(NBTB_COMPOUND(SectionClient)), NBTB_CONVERTERS(assign_op, static_cast_op<SectionClient*>()))
NBTB_END

}

#endif
