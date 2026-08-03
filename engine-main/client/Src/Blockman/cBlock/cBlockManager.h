/********************************************************************
filename: 	BlockManagerClient.h
file path:	dev\client\Src\Block

version:	1
author:		ajohn
company:	supernano
date:		2017-06-21
*********************************************************************/
#ifndef __BLOCK_MANAGER_CLIENT_HEADER__
#define __BLOCK_MANAGER_CLIENT_HEADER__

#include "Block/BlockManager.h"

namespace BLOCKMAN
{

class cBlock;

class cBlockManager : public BlockManager
{
public:
	static void cInitialize();
	static void cUnInitialize();
	static void cRegistBlockItems();
	static void registSprite(TextureAtlasRegister* regist);
	
	static cBlock** scBlocks;
};

}

#endif