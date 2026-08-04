/********************************************************************
filename: 	VisualGraph.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-23
*********************************************************************/
#ifndef __VISUAL_GRAPH_HEADER__
#define __VISUAL_GRAPH_HEADER__

#include "BM_TypeDef.h"
#include "Util/BitSet.h"
#include "Util/Facing.h"

namespace BLOCKMAN
{

class SetVisibility
{
public:
	static SetVisibility* s_Empty;
	static SetVisibility* s_Full;

protected:
	static const int s_FaceNum = 6;
	BitSet<36> m_visFlag;
	// BitSet<s_FaceNum*s_FaceNum> m_visFlag;

/**
   FACE  U  D  N  S  E  W
      U  s
	  D     t
	  N        u
	  S           p  
	  E              i 
	  W                 d
*/

public:
	SetVisibility();

	static void initialize();
	static void unInitialize();

	void buildVisibility(const FacingArr& arr);
	void setVisibility(Facing* facing1, Facing* facing2, bool bVisible);
	void setAll(bool bVisible);
	bool getVisibility(Facing* facing1, Facing* facing2);
	void buildVisualHelp(const BlockPos& pos);
	void outputInfo();
};

class VisualGraph : public ObjectAlloc
{
protected:
	static const int s_Xbegin = 1;		// pow(16.0D, 0.0D);
	static const int s_Zbegin = 16;		// pow(16.0D, 1.0D);
	static const int s_Ybegin = 256;	// pow(16.0D, 2.0D);
	BitSet<4096> m_visibleFlag;
	static int* INDEX_OF_EDGES;			// = new int[1352]; 1024 + 256 + 64 + 8.
	static int INDEX_OF_EDGES_LEN;
	int m_emptyQuota = 4096;					// = 4096;

protected:
	static int getBlockIdx(const BlockPos& pos)
	{
		LordAssert(pos.x >= 0 && pos.x < 16);
		LordAssert(pos.y >= 0 && pos.y < 16);
		LordAssert(pos.z >= 0 && pos.z < 16);
		return pos.x | (pos.y << 8) | (pos.z << 4);
	}

public:
	VisualGraph();
	void setOpaque(const BlockPos& pos);
	void computeVisibility(SetVisibility* setVisibility);

	FacingArr getVisibleFacings(const BlockPos& pos)
	{
		return floodFill(getBlockIdx(pos));
	}

	FacingArr floodFill(int blockPosPak);
	void addEdges(int edges, FacingArr& facingArr);
	int getNeighborIndexAtFace(int edges, Facing* facing);

	static void initialize();
	static void unInitialize();
};
}

#endif