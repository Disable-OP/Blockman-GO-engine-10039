#include "WorldProviderClient.h"

/** header files from common. */
#include "World/World.h"
#include "World/WorldChunkManager.h"
#include "cChunk/ChunkProviderEndClient.h"
#include "cChunk/ChunkProviderFlatClient.h"
#include "cChunk/ChunkProviderGenerateClient.h"
#include "cChunk/ChunkProviderHellClient.h"


namespace BLOCKMAN
{

WorldProvider* WorldProviderClient::getProviderForDimension(int dimension)
{
	if (dimension == -1)
	{
		return LordNew WorldProviderHellClient();
	}
	else if (dimension == 0)
	{
		return LordNew WorldProviderSurfaceClient();
	}
	else if (dimension == 1)
	{
		return LordNew WorldProviderEndClient();
	}
	return NULL;
}

IChunkProvider* WorldProviderHellClient::createChunkGenerator()
{
	return LordNew ChunkProviderHellClient(worldObj, worldObj->getSeed());
}

IChunkProvider* WorldProviderSurfaceClient::createChunkGenerator()
{
	IChunkProvider* pResult = NULL;
	if (terrainType == TERRAIN_TYPE_FLAT)
	{
		pResult = LordNew ChunkProviderFlatClient(worldObj, generateOptions);
	}
	else
	{
		pResult = LordNew ChunkProviderGenerateClient(worldObj, worldObj->getSeed(), worldObj->getWorldInfo().isMapFeaturesEnabled());
	}
	return pResult;
}

IChunkProvider* WorldProviderEndClient::createChunkGenerator()
{
	return LordNew ChunkProviderEndClient(worldObj, worldObj->getSeed());
}

}
