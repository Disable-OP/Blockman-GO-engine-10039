#include "ServerWorldProvider.h"
#include "ServerWorld.h"

/** header files from common. */
#include "World/World.h"
#include "World/WorldChunkManager.h"
#include "Chunk/ChunkProviderGenerate.h"
#include "Chunk/ChunkProviderHell.h"
#include "Chunk/ChunkProviderEnd.h"
#include "Chunk/ChunkProviderFlat.h"
#include "Chunk/ChunkProviderCustom.h"

namespace BLOCKMAN
{

WorldProvider* ServerWorldProvider::getProviderForDimension(int dimension)
{
	if (dimension == -1)
	{
		return LordNew ServerWorldProviderHell();
	}
	else if (dimension == 0)
	{
		return LordNew ServerWorldProviderSurface();
	}
	else if (dimension == 1)
	{
		return LordNew ServerWorldProviderEnd();
	}
	return NULL;
}

// Server-authoritative world generation: the server now actually runs the
// shared WorldGenerator pipeline (Perlin/Octaves noise + BiomeGen +
// BiomeDecorator + MapGenerate + StructureStart) instead of throwing.
// See docs/WORLDGEN.md for the design contract.
//
// Each provider delegates to the matching ChunkProvider implementation in
// logic/Src/Chunk/. The seed comes from the World (set in
// ServerWorld::createWorld via WorldSettings).

IChunkProvider* ServerWorldProviderHell::createChunkGenerator()
{
	return LordNew ChunkProviderHell(worldObj, worldObj->getSeed());
}

IChunkProvider* ServerWorldProviderSurface::createChunkGenerator()
{
	// Custom world type (sky islands) — check the marker set by
	// ServerWorld::createWorld() when worldType == TERRAIN_TYPE_CUSTOM.
	if (generateOptions == "custom")
	{
		return LordNew ChunkProviderCustom(worldObj, worldObj->getSeed());
	}

	// Otherwise: delegate to the base class — it picks ChunkProviderFlat
	// vs ChunkProviderGenerate based on terrainType.
	return WorldProviderSurface::createChunkGenerator();
}

IChunkProvider* ServerWorldProviderEnd::createChunkGenerator()
{
	return LordNew ChunkProviderEnd(worldObj, worldObj->getSeed());
}

}

