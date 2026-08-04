/********************************************************************
filename: 	StructureNetherBridgePieceWeight.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-10
*********************************************************************/
#ifndef __STRUCTURE_NETHER_BRIDGE_PIECE_WEIGHT_HEADER__
#define __STRUCTURE_NETHER_BRIDGE_PIECE_WEIGHT_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class StructureNetherBridgePieceWeight : public ObjectAlloc
{
	/** The class of the StructureComponent to which this weight corresponds. */
public:
	int m_type = 0;
	int field_78826_b = 0;
	int field_78827_c = 0;
	int field_78824_d = 0;
	bool field_78825_e = false;

	StructureNetherBridgePieceWeight(int type, int par2, int par3, bool par4)
		: m_type(type)
		, field_78826_b(par2)
		, field_78824_d(par3)
		, field_78825_e(par4)
	{
	}

	StructureNetherBridgePieceWeight(int type, int par2, int par3)
		: m_type(type)
		, field_78826_b(par2)
		, field_78824_d(par3)
		, field_78825_e(false)
	{
	}

	bool func_78822_a(int par1)
	{
		return field_78824_d == 0 || field_78827_c < field_78824_d;
	}

	bool func_78823_a()
	{
		return field_78824_d == 0 || field_78827_c < field_78824_d;
	}
};


}

#endif

