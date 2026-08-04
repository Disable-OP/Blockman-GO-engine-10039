/********************************************************************
filename: 	IBlockAccess.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __INTERFACE_BLOCK_ACCESS_HEADER__
#define __INTERFACE_BLOCK_ACCESS_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class BM_Material;
class BiomeGenBase;
class TileEntity;

/** the interface of block access. */
class IBlockAccess
{
public:
	/** Returns the block ID at coords pos. */
	virtual int getBlockId(const BlockPos& pos) = 0;

	/** Returns the block metadata at coords pos. */
	virtual int getBlockMeta(const BlockPos& pos) = 0;

	/** return the Block ID and  meta at corrds pos. id is high 16 bits, meta is low 16 bits. */
	virtual int getBlockIDAndMeta(const BlockPos& pos) = 0;

	/** Returns the TileEntity associated with a given block in pos coordinates, or null if no TileEntity exists. */
	virtual TileEntity* getBlockTileEntity(const BlockPos& pos) = 0;

	/** Any Light rendered on a 1.8 Block goes through here. */
	virtual int getLightBrightnessForSkyBlocks(const BlockPos& pos, int minBlockLight) = 0;

	/** Return the brightness in pos. return value >= minLight's brightness. */
	virtual float getBrightness(const BlockPos& pos, int minLight) = 0;

	/** Returns how bright the block is shown as which is the block's light value looked up in a lookup table (light
	  * values aren't linear for brightness). */
	virtual float getLightBrightness(const BlockPos& pos) = 0;
	
	/** Returns the block's material. */
	virtual const BM_Material& getBlockMaterial(const BlockPos& pos) = 0;

	/** Returns true if the block at the specified coordinates is an opaque cube. Args: pos. */
	virtual bool isBlockOpaqueCube(const BlockPos& pos) = 0;

	/** Indicate if a material is a normal solid opaque cube. */
	virtual bool isBlockNormalCube(const BlockPos& pos) = 0;

	/** Returns true if the block at the specified coordinates is empty. */
	virtual bool isAirBlock(const BlockPos& pos) = 0;

	/** Gets the biome for a given set of x/z coordinates. */
	virtual BiomeGenBase* getBiomeGenForCoords(int x, int z) = 0;

	/** Returns current world height. */
	virtual int getHeight() { return 256;  };

	/** set by !chunk.getAreLevelsEmpty. */
	virtual bool extendedLevelsInChunkCollection() = 0;

	/** Returns true if the block at the given coordinate has a solid (buildable) top surface. */
	virtual bool doesBlockHaveSolidTopSurface(const BlockPos& pos) = 0;

	/** Is this block powering in the specified direction Args: pos, dir. */
	virtual int isBlockProvidingPowerTo(const BlockPos& pos, int dir) = 0;
};

}

#endif
