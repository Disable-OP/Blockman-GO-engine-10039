/********************************************************************
filename: 	WorldProviderServer
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\Game\World\WorldProviderServer.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/28
*********************************************************************/
#ifndef __WORLDPROVIDERSERVER_H__
#define __WORLDPROVIDERSERVER_H__

#include "World/WorldProvider.h"

namespace BLOCKMAN
{

class ServerWorld;
class ChunkServer;

class ServerWorldProvider : public WorldProvider
{
public:
	static WorldProvider* getProviderForDimension(int dimmesion);
};

class ServerWorldProviderHell : public WorldProviderHell
{
public:
	/** Implement virtual funtions from WorldProviderHell */
	virtual IChunkProvider* createChunkGenerator();
};

class ServerWorldProviderSurface : public WorldProviderSurface
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual IChunkProvider* createChunkGenerator();
};

class ServerWorldProviderEnd : public WorldProviderEnd
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual IChunkProvider* createChunkGenerator();
};

}
#endif
