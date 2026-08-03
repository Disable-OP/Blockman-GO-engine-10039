#include "Facing.h"
#include "Random.h"

namespace BLOCKMAN
{

Facing* Facing::UP = NULL;
Facing* Facing::DOWN = NULL;
Facing* Facing::NORTH = NULL;
Facing* Facing::SOUTH = NULL;
Facing* Facing::EAST = NULL;
Facing* Facing::WEST = NULL;
Facing* Facing::VALUES[6];
Facing* Facing::HORIZONTALS[4];

Facing::Facing(int idx, int opposite, int horizontalIdx, AXIS_DIR axisDir, FACING_AXIS axis, FACING_ORDINAL ordinal, Vector3i dirVec)
	: m_index(idx)
	, m_horizontalIndex(horizontalIdx)
	, m_opposite(opposite)
	, m_axis(axis)
	, m_axisDirection(axisDir)
	, m_ordinal(ordinal)
	, m_directionVec(dirVec)
{
}

Facing* Facing::rotateAround(FACING_AXIS axis)
{
	switch (axis)
	{
	case FACING_AXIS_X:
		if (this != WEST && this != EAST)
			return rotateX();
		return this;

	case FACING_AXIS_Y:
		if (this != UP && this != DOWN)
			return rotateY();
		return this;

	case FACING_AXIS_Z:
		if (this != NORTH && this != SOUTH)
			return rotateZ();
		return this;

	default:
		LordException("Unable to get CW facing for axis %d", int(axis));
	}
	return NULL;
}

Facing* Facing::rotateY()
{
	switch (m_ordinal)
	{
	case FACING_ORIDINAL_NORTH:
		return EAST;

	case FACING_ORIDINAL_EAST:
		return SOUTH;

	case FACING_ORIDINAL_SOUTH:
		return WEST;

	case FACING_ORIDINAL_WEST:
		return NORTH;

	default:
		LordException("Unable to get Y-rotated facing of %d" + int(m_ordinal));
	}
	return NULL;
}

Facing* Facing::rotateX()
{
	switch (m_ordinal)
	{
	case FACING_ORIDINAL_NORTH:
		return DOWN;

	case FACING_ORIDINAL_SOUTH:
		return UP;

	case FACING_ORIDINAL_UP:
		return NORTH;

	case FACING_ORIDINAL_DOWN:
		return SOUTH;

	default:
		LordException("Unable to get X-rotated facing of %d" + int(m_ordinal));
	}
	return NULL;
}

Facing* Facing::rotateZ()
{
	switch (m_ordinal)
	{
	case FACING_ORIDINAL_EAST:
		return DOWN;

	case FACING_ORIDINAL_WEST:
		return UP;

	case FACING_ORIDINAL_UP:
		return EAST;

	case FACING_ORIDINAL_DOWN:
		return WEST;

	default:
		LordException("Unable to get Z-rotated facing of %d" + int(m_ordinal));
	}
	return NULL;
}

/** Rotate this Facing around the Y axis counter-clockwise (NORTH => WEST => SOUTH => EAST => NORTH) */
Facing* Facing::rotateYCCW()
{
	switch (m_ordinal)
	{
	case FACING_ORIDINAL_NORTH:
		return WEST;

	case FACING_ORIDINAL_EAST:
		return NORTH;

	case FACING_ORIDINAL_SOUTH:
		return EAST;

	case FACING_ORIDINAL_WEST:
		return SOUTH;

	default:
		LordException("Unable to get Y-rotated CCW facing of %d" + int(m_ordinal));
	}
	return NULL;
}

Facing* Facing::getByName(const String& name)
{
	String nameLowercase = name;
	StringUtil::LowerCase(nameLowercase);
	if (nameLowercase == "up")
		return UP;
	else if (nameLowercase == "down")
		return DOWN;
	else if (nameLowercase == "north")
		return NORTH;
	else if (nameLowercase == "east")
		return EAST;
	else if (nameLowercase == "south")
		return SOUTH;
	else if (nameLowercase == "west")
		return WEST;
	return NULL;
}

void Facing::initialize()
{
	LordAssert(!UP);
	DOWN = LordNew Facing(0, 1, -1, AXIS_DIR_NEGATIVE, FACING_AXIS_Y, FACING_ORIDINAL_DOWN, Vector3i::NEG_UNIT_Y);
	UP = LordNew Facing(1, 0, -1, AXIS_DIR_POSITIVE, FACING_AXIS_Y, FACING_ORIDINAL_UP, Vector3i::UNIT_Y);
	NORTH = LordNew Facing(2, 3, 2, AXIS_DIR_NEGATIVE, FACING_AXIS_Z, FACING_ORIDINAL_NORTH, Vector3i::NEG_UNIT_Z);
	SOUTH = LordNew Facing(3, 2, 0, AXIS_DIR_POSITIVE, FACING_AXIS_Z, FACING_ORIDINAL_SOUTH, Vector3i::UNIT_Z);
	WEST = LordNew Facing(4, 5, 1, AXIS_DIR_NEGATIVE, FACING_AXIS_X, FACING_ORIDINAL_WEST, Vector3i::NEG_UNIT_X);
	EAST = LordNew Facing(5, 4, 3, AXIS_DIR_POSITIVE, FACING_AXIS_X, FACING_ORIDINAL_EAST, Vector3i::UNIT_X);

	VALUES[0] = DOWN;
	VALUES[1] = UP;
	VALUES[2] = NORTH;
	VALUES[3] = SOUTH;
	VALUES[4] = WEST;
	VALUES[5] = EAST;

	HORIZONTALS[0] = NORTH;
	HORIZONTALS[1] = SOUTH;
	HORIZONTALS[2] = WEST;
	HORIZONTALS[3] = EAST;
}

void Facing::unInitialize()
{
	for (int i=0; i<6; ++i)
		LordSafeDelete(VALUES[i]);
	DOWN = NULL;
	UP = NULL;
	NORTH = NULL;
	SOUTH = NULL;
	WEST = NULL;
	EAST = NULL;
	memset(VALUES, 0, sizeof(Facing*) * 6);
	memset(HORIZONTALS, 0, sizeof(Facing*) * 4);
}

Facing* Facing::getFront(int index)
{
	return VALUES[Math::Abs(index % 6)];
}

Facing* Facing::getHorizontal(int horizontalIdx)
{
	return HORIZONTALS[Math::Abs(horizontalIdx % 4)];
}

Facing* Facing::fromAngle(double degree)
{
	return getHorizontal(int(Math::Floor(degree / 90.0 + 0.5)) & 3);
}

Facing* Facing::getRandom(Random* rand)
{
	return VALUES[rand->nextInt(6)];
}

Facing* Facing::getFacingFromVector(const Vector3& v)
{
	Facing* pResult = NORTH;
	float maxGradient = Math::MIN_FLOAT;

	for (int i = 0; i < 6; ++i)
	{
		Facing* facing = VALUES[i];
		float currentGradient =
			v.x * (float)facing->m_directionVec.x +
			v.y * (float)facing->m_directionVec.y +
			v.z * (float)facing->m_directionVec.z;

		if (currentGradient > maxGradient)
		{
			maxGradient = currentGradient;
			pResult = facing;
		}
	}

	return pResult;
}

}
