/********************************************************************
filename: 	ChunkCorrdIntPair.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-7
*********************************************************************/
#ifndef __CHUNK_COORDINT_PAIR_HEADER__
#define __CHUNK_COORDINT_PAIR_HEADER__

#include "ChunkPosition.h"

namespace BLOCKMAN
{

class ChunkCoordIntPair
{
public:
	/** The X position of this Chunk Coordinate Pair */
	int m_chunkXPos;

	/** The Z position of this Chunk Coordinate Pair */
	int m_chunkZPos;

	ChunkCoordIntPair()
		: m_chunkXPos(Math::MIN_INT)
		, m_chunkZPos(Math::MIN_INT)
	{}

	ChunkCoordIntPair(int x, int z)
		: m_chunkXPos(x)
		, m_chunkZPos(z)
	{ }

	ChunkCoordIntPair(const ChunkCoordIntPair& rhs)
		: m_chunkXPos(rhs.m_chunkXPos)
		, m_chunkZPos(rhs.m_chunkZPos)
	{}

	void setPos(int x, int z)
	{
		m_chunkXPos = x;
		m_chunkZPos = z;
	}

	bool isValid() const
	{
		return m_chunkXPos != Math::MIN_INT && m_chunkZPos != Math::MIN_INT;
	}

	/**
	* converts a chunk coordinate pair to an integer (suitable for hashing)
	*/
	static i64 chunkXZ2Int(int x, int z)
	{
		return (i64)x & (i64)4294967295LL | ((i64)z & (i64)4294967295LL) << 32;
	}

	static void Int2chunkXZ(i64 i64Value, int& x, int& z)
	{
		x = int(i64Value & 0xFFFFFFFFLL);
		z = int(ui64(i64Value) >> 32);
	}

	int hashCode()
	{
		i64 var1 = chunkXZ2Int(m_chunkXPos, m_chunkZPos);
		int var3 = (int)var1;
		int var4 = (int)(var1 >> 32);
		return var3 ^ var4;
	}

	bool operator==(const ChunkCoordIntPair& rhs) const
	{
		return rhs.m_chunkXPos == m_chunkXPos && rhs.m_chunkZPos == m_chunkZPos;
	}
	
	bool operator<(const ChunkCoordIntPair& rhs) const
	{
		return m_chunkXPos != rhs.m_chunkXPos ? m_chunkXPos < rhs.m_chunkXPos : m_chunkZPos < rhs.m_chunkZPos;
	}

	int getCenterXPos() const { return (m_chunkXPos << 4) + 8; }
	int getCenterZPos() const { return (m_chunkZPos << 4) + 8; }
	
	ChunkPosition getChunkPosition(int y)
	{
		return ChunkPosition(getCenterXPos(), y, getCenterZPos());
	}

	String toString()
	{
		char buff[64];
		snprintf(buff, 64, "[%d, %d]", m_chunkXPos, m_chunkZPos);
		return buff;
	}

};

typedef set<ChunkCoordIntPair>::type ChunkCoordSet;

}

#endif
