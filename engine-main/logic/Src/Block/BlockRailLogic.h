/********************************************************************
filename: 	Blocks.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-22
*********************************************************************/
#ifndef __BLOCK_RAIL_LOIGC_HEADER__
#define __BLOCK_RAIL_LOIGC_HEADER__

#include "BM_TypeDef.h"
#include "BM_Container_def.h"

using namespace LORD;

namespace BLOCKMAN
{

	class World;
	class BlockRailBase;

	class BlockRailLogic : public ObjectAlloc
	{
	private:
		World * logicWorld;
		BlockPos railPos;
		bool isStraightRail;

		/** The chunk position the rail is at. */
		BlockPosArr railChunkPosition;
		BlockRailBase* theRail;
		vector<BlockRailLogic*>::type deleteArr;

	public:
		// 重新制定一个铁轨位置，用于后续操作
		BlockRailLogic(BlockRailBase* pRail, World* pWorld, const BlockPos& pos);
		~BlockRailLogic();
		void checkConnectionToNeighbor(bool powered, bool flag);

	private:
		void setBasicRail(int meta);
		void refreshConnectedTracks();
		bool isMinecartTrack(const BlockPos& pos);
		BlockRailLogic* getRailLogic(const BlockPos& pos);

		/** Checks if the rail is at the chunk position it is expected to be. */
		bool isRailChunkPositionCorrect(BlockRailLogic* pRailLogic);
		bool isPartOfTrack(const BlockPos& pos);
		int getNumberOfAdjacentTracks();
		bool canConnectTo(BlockRailLogic* pRailLoigc);
		void connectToNeighbor(BlockRailLogic* pRailLogic);
		bool canConnectFrom(const BlockPos& pos);
	};

}
#endif