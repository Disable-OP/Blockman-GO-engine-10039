/********************************************************************
filename: 	WorldProviderClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-06-13
*********************************************************************/
#ifndef __WORLD_PROVIDER_CLIENT_HEADER__
#define __WORLD_PROVIDER_CLIENT_HEADER__

/** header files from common. */
#include "World/WorldProvider.h"

namespace BLOCKMAN
{

class WorldProviderClient : public WorldProvider
{
public:
	static WorldProvider* getProviderForDimension(int dimmesion);
};

class WorldProviderHellClient : public WorldProviderHell
{
public:
	/** Implement virtual funtions from WorldProviderHell */
	virtual IChunkProvider* createChunkGenerator();
};

class WorldProviderSurfaceClient : public WorldProviderSurface
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual IChunkProvider* createChunkGenerator();
};

class WorldProviderEndClient : public WorldProviderEnd
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual IChunkProvider* createChunkGenerator();
};


}

#endif