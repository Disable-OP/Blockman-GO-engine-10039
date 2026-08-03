#include "ServerWorldProvider.h"
#include "ServerWorld.h"

/** header files from common. */
#include "World/World.h"
#include "World/WorldChunkManager.h"

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

IChunkProvider* ServerWorldProviderHell::createChunkGenerator()
{
	throw std::logic_error("method not implemented");
}

IChunkProvider* ServerWorldProviderSurface::createChunkGenerator()
{
	throw std::logic_error("method not implemented");
}

IChunkProvider* ServerWorldProviderEnd::createChunkGenerator()
{
	throw std::logic_error("method not implemented");
}

}

