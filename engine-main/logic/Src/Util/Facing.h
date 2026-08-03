/********************************************************************
filename: 	Facing.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-23
*********************************************************************/
#ifndef __FACING_HEADER__
#define __FACING_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Random;

class Facing : public ObjectAlloc
{
public:
	enum AXIS_DIR
	{
		AXIS_DIR_POSITIVE,
		AXIS_DIR_NEGATIVE,
	};

	enum FACING_AXIS
	{
		FACING_AXIS_X,
		FACING_AXIS_Y,
		FACING_AXIS_Z,
	};

	enum FACING_ORDINAL
	{
		FACING_ORIDINAL_NORTH,
		FACING_ORIDINAL_EAST,
		FACING_ORIDINAL_SOUTH,
		FACING_ORIDINAL_WEST,
		FACING_ORIDINAL_UP,
		FACING_ORIDINAL_DOWN,
	};

public:
	static Facing* DOWN;
	static Facing* UP;
	static Facing* NORTH;
	static Facing* SOUTH;
	static Facing* WEST;
	static Facing* EAST;
	static Facing* VALUES[6];		// All facings in D-U-N-S-W-E order
	static Facing* HORIZONTALS[4];	// All Facings with horizontal axis in order S-W-N-E 

protected:
	int m_index = 0;				// Ordering index for D-U-N-S-W-E
	int m_opposite = 0;				// Index of the opposite Facing in the VALUES array
	int m_horizontalIndex = 0;		// Ordering index for the HORIZONTALS field (S-W-N-E) 
	FACING_AXIS m_axis = FACING_AXIS_X;			// 0:x 1:y 2: z
	AXIS_DIR m_axisDirection = AXIS_DIR_POSITIVE;	// 0:positive 1:negative
	FACING_ORDINAL m_ordinal = FACING_ORIDINAL_NORTH;	// ordinal value.
	Vector3i m_directionVec;	// Normalized Vector that points in the direction of this Facing

	Facing(int idx, int opposite, int horizontalIdx, AXIS_DIR axisDir, FACING_AXIS axis, FACING_ORDINAL ordinal, Vector3i dirVec);
	
	/** left-hand. Rotate this Facing around the Y axis clockwise (NORTH => EAST => SOUTH => WEST => NORTH) */
	Facing* rotateY();
	/** left-hand. Rotate this Facing around the X axis (NORTH => DOWN => SOUTH => UP => NORTH) */
	Facing* rotateX();
	/** left-hand.  Rotate this Facing around the Z axis (EAST => DOWN => WEST => UP => EAST) */
	Facing* rotateZ();

	int getOffset() const { return m_axisDirection == AXIS_DIR_POSITIVE ? 1 : -1; }

public:
	int getIndex() const { return m_index; }
	int getHorizontalIndex() const { return m_horizontalIndex; }
	AXIS_DIR getAxisDirection() const { return m_axisDirection; }
	Facing* getOpposite() { return getFront(m_opposite); }
	int getFrontOffsetX() const { return m_axis == FACING_AXIS_X ? getOffset() : 0; }
	int getFrontOffsetY() const { return m_axis == FACING_AXIS_Y ? getOffset() : 0; }
	int getFrontOffsetZ() const { return m_axis == FACING_AXIS_Z ? getOffset() : 0; }
	Vector3i getFrontOffset() const { return Vector3i(getFrontOffsetX(), getFrontOffsetY(), getFrontOffsetZ()); }
	FACING_AXIS getAxis() const { return m_axis; }
	Vector3i getDirectionVec() const { return m_directionVec; }

	/** Rotate this Facing around the given axis clockwise. If this facing cannot be rotated around the given axis */
	Facing* rotateAround(FACING_AXIS axis);

	/** Rotate this Facing around the Y axis counter-clockwise (NORTH => WEST => SOUTH => EAST => NORTH) */
	Facing* rotateYCCW();

	/** Get the facing specified by the given name */
	static void initialize();
	static void unInitialize();
	static Facing* getByName(const String& name);
	static Facing* getFront(int index);
	static Facing* getHorizontal(int horizontalIdx);
	static Facing* fromAngle(double degree);
	static Facing* getRandom(Random* rand);
	static Facing* getFacingFromVector(const Vector3& v);
};

typedef set<Facing*>::type FacingArr;

}

#endif