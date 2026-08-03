/********************************************************************
filename: 	ChunkCollection.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-2
*********************************************************************/
#ifndef __CHUNK_COLLECTION_HEADER__
#define __CHUNK_COLLECTION_HEADER__

#include "IBlockAccess.h"
#include "World.h"

namespace BLOCKMAN
{

class Chunk;
class World;

class ChunkCollection : public IBlockAccess, public ObjectAlloc
{
public:
	ChunkCollection(World* pWorld, const BlockPos& min, const BlockPos& max, int extend);

	/** implement virtual function from IBlockAccess. */
	virtual int getBlockId(const BlockPos& pos);
	virtual int getBlockMeta(const BlockPos& pos);
	virtual int getBlockIDAndMeta(const BlockPos& pos);
	virtual TileEntity* getBlockTileEntity(const BlockPos& pos);
	virtual int getLightBrightnessForSkyBlocks(const BlockPos& pos, int minBrightness);
	virtual float getBrightness(const BlockPos& pos, int minBrightness);
	virtual float getLightBrightness(const BlockPos& pos);
	virtual const BM_Material& getBlockMaterial(const BlockPos& pos);
	virtual bool isBlockOpaqueCube(const BlockPos& pos);
	virtual bool isBlockNormalCube(const BlockPos& pos);
	virtual bool isAirBlock(const BlockPos& pos);
	virtual BiomeGenBase* getBiomeGenForCoords(int x, int z);
	virtual int getHeight() { return 256; }
	virtual bool extendedLevelsInChunkCollection() { return isEmpty;	}
	virtual bool doesBlockHaveSolidTopSurface(const BlockPos& pos);
	virtual int isBlockProvidingPowerTo(const BlockPos& pos, int side);
	
	int getLightValue(const BlockPos& pos) { return getLightValueExt(pos, true); }
	int getLightValueExt(const BlockPos& pos, bool flag);
	int getSkyBlockTypeBrightness(int lightValue, BlockPos pos);
	int getSpecialBlockBrightness(int lightValue, BlockPos pos);
	bool isInAsync() { return m_isInAsync; }

protected:
	bool checkBlockPos(const BlockPos& pos);

protected:
	int chunkX = 0;
	int chunkZ = 0;
	ChunkArr chunkArray;
	int arrayColumn = 0;
	int arrayRow = 0;

	/** True if the chunk cache is empty. */
	bool isEmpty = false;

	/** True if the chunk is in async provide */
	bool m_isInAsync = false;

	/** Reference to the World object. */
	World* worldObj = nullptr;
};

}

#endif