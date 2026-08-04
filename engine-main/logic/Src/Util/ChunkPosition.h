/********************************************************************
filename: 	ChunkPosition.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-4
*********************************************************************/
#ifndef __CHUNK_POSITION_HEADER__
#define __CHUNK_POSITION_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class ChunkPosition
{
public:
	ChunkPosition()
		: m_x(Math::MIN_INT)
		, m_y(Math::MIN_INT)
		, m_z(Math::MIN_INT)
	{}

	ChunkPosition(int x, int y, int z)
		: m_x(x)
		, m_y(y)
		, m_z(z)
	{ }

	ChunkPosition(const Vector3i& pos)
		: m_x(pos.x)
		, m_y(pos.y)
		, m_z(pos.z)
	{}

	ChunkPosition(const Vector3& vec)
	{
		m_x = (int)Math::Floor(vec.x);
		m_y = (int)Math::Floor(vec.y);
		m_z = (int)Math::Floor(vec.z);
	}

	bool operator == (const ChunkPosition& rhs)
	{
		return
			m_x == rhs.m_x &&
			m_y == rhs.m_y &&
			m_z == rhs.m_z;
	}

	int hashCode() const
	{
		return m_x * 8976890 + m_y * 981131 + m_z;
	}

	bool isValid() const
	{
		return m_x != Math::MIN_INT && m_y != Math::MIN_INT && m_z != Math::MIN_INT;
	}

	bool isInvalid() const
	{
		return !isValid();
	}

	bool operator> (const ChunkPosition& rhs) const { return hashCode() > rhs.hashCode(); }
	bool operator< (const ChunkPosition& rhs) const { return hashCode() < rhs.hashCode(); }
	bool operator>=(const ChunkPosition& rhs) const { return hashCode() >= rhs.hashCode(); }
	bool operator<=(const ChunkPosition& rhs) const { return hashCode() <= rhs.hashCode(); }
	bool operator==(const ChunkPosition& rhs) const { return hashCode() == rhs.hashCode(); }
	bool operator!=(const ChunkPosition& rhs) const { return hashCode() != rhs.hashCode(); }

	/** The x y zcoordinate of this ChunkPosition */
	int m_x = 0;
	int m_y = 0;
	int m_z = 0;
};

typedef vector<ChunkPosition>::type ChunkPosArr;

}

#endif