/********************************************************************
filename: 	StructureBB.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-7
*********************************************************************/
#ifndef __STRUCTURE_BB_HEADER__
#define __STRUCTURE_BB_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{

class StructureBB : public ObjectAlloc
{
public:
	/** The first x y z coordinate of a bounding box. */
	int m_minX;
	int m_minY;
	int m_minZ;
	int m_maxX;
	int m_maxY;
	int m_maxZ;

	StructureBB()
		: m_minX(Math::MAX_INT)
		, m_minY(Math::MAX_INT)
		, m_minZ(Math::MAX_INT)
		, m_maxX(Math::MIN_INT)
		, m_maxY(Math::MIN_INT)
		, m_maxZ(Math::MIN_INT)
	{}

	void invalid()
	{
		m_minX = Math::MAX_INT;
		m_minY = Math::MAX_INT;
		m_minZ = Math::MAX_INT;
		m_maxX = Math::MIN_INT;
		m_maxY = Math::MIN_INT;
		m_maxZ = Math::MIN_INT;
	}

	bool isInvalid() const
	{
		return m_minX > m_maxX || m_minY > m_maxY || m_minZ > m_maxZ;
	}

	/** used to project a possible new component Bounding Box - to check if it would cut anything already spawned */
	static StructureBB getComponentToAddBoundingBox(int par0, int par1, int par2, int par3, int par4, int par5, int par6, int par7, int par8, int par9)
	{
		switch (par9)
		{
		case 0:
			return StructureBB(par0 + par3, par1 + par4, par2 + par5, par0 + par6 - 1 + par3, par1 + par7 - 1 + par4, par2 + par8 - 1 + par5);

		case 1:
			return StructureBB(par0 - par8 + 1 + par5, par1 + par4, par2 + par3, par0 + par5, par1 + par7 - 1 + par4, par2 + par6 - 1 + par3);

		case 2:
			return StructureBB(par0 + par3, par1 + par4, par2 - par8 + 1 + par5, par0 + par6 - 1 + par3, par1 + par7 - 1 + par4, par2 + par5);

		case 3:
			return StructureBB(par0 + par5, par1 + par4, par2 + par3, par0 + par8 - 1 + par5, par1 + par7 - 1 + par4, par2 + par6 - 1 + par3);

		default:
			return StructureBB(par0 + par3, par1 + par4, par2 + par5, par0 + par6 - 1 + par3, par1 + par7 - 1 + par4, par2 + par8 - 1 + par5);
		}
	}

	StructureBB(const StructureBB& rhs)
	{
		m_minX = rhs.m_minX;
		m_minY = rhs.m_minY;
		m_minZ = rhs.m_minZ;
		m_maxX = rhs.m_maxX;
		m_maxY = rhs.m_maxY;
		m_maxZ = rhs.m_maxZ;
	}

	StructureBB(int par1, int par2, int par3, int par4, int par5, int par6)
	{
		m_minX = par1;
		m_minY = par2;
		m_minZ = par3;
		m_maxX = par4;
		m_maxY = par5;
		m_maxZ = par6;
	}

	StructureBB(int par1, int par2, int par3, int par4)
	{
		m_minX = par1;
		m_minZ = par2;
		m_maxX = par3;
		m_maxZ = par4;
		m_minY = 1;
		m_maxY = 512;
	}

	void set(int par1, int par2, int par3, int par4, int par5, int par6)
	{
		m_minX = par1;
		m_minY = par2;
		m_minZ = par3;
		m_maxX = par4;
		m_maxY = par5;
		m_maxZ = par6;
	}

	/**
	* Returns whether the given bounding box intersects with this one. Args: structureboundingbox
	*/
	bool intersectsWith(const StructureBB& rhs) const
	{
		return m_maxX >= rhs.m_minX && m_minX <= rhs.m_maxX && m_maxZ >= rhs.m_minZ && m_minZ <= rhs.m_maxZ && m_maxY >= rhs.m_minY && m_minY <= rhs.m_maxY;
	}

	/**
	* Discover if a coordinate is inside the bounding box area.
	*/
	bool intersectsWith(int minx, int minz, int maxx, int maxz) const
	{
		return m_maxX >= minx && m_minX <= maxx && m_maxZ >= minz && m_minZ <= maxz;
	}

	/**
	* Expands a bounding box's dimensions to include the supplied bounding box.
	*/
	void expandTo(const StructureBB& rhs)
	{
		m_minX = Math::Min(m_minX, rhs.m_minX);
		m_minY = Math::Min(m_minY, rhs.m_minY);
		m_minZ = Math::Min(m_minZ, rhs.m_minZ);
		m_maxX = Math::Max(m_maxX, rhs.m_maxX);
		m_maxY = Math::Max(m_maxY, rhs.m_maxY);
		m_maxZ = Math::Max(m_maxZ, rhs.m_maxZ);
	}

	/**
	* Offsets the current bounding box by the specified coordinates. Args: x, y, z
	*/
	void offset(int par1, int par2, int par3)
	{
		m_minX += par1;
		m_minY += par2;
		m_minZ += par3;
		m_maxX += par1;
		m_maxY += par2;
		m_maxZ += par3;
	}

	/**
	* Returns true if block is inside bounding box
	*/
	bool isVecInside(int par1, int par2, int par3) const
	{
		return par1 >= m_minX && par1 <= m_maxX && par3 >= m_minZ && par3 <= m_maxZ && par2 >= m_minY && par2 <= m_maxY;
	}

	bool isVecInside(const BlockPos& pos) const
	{
		return pos.x >= m_minX && pos.x <= m_maxX && pos.z >= m_minZ && pos.z <= m_maxZ && pos.y >= m_minY && pos.y <= m_maxY;
	}

	/**
	* Returns width of a bounding box
	*/
	int getXSize() const
	{
		return m_maxX - m_minX + 1;
	}

	/**
	* Returns height of a bounding box
	*/
	int getYSize() const
	{
		return m_maxY - m_minY + 1;
	}

	/**
	* Returns length of a bounding box
	*/
	int getZSize() const
	{
		return m_maxZ - m_minZ + 1;
	}

	int getCenterX() const
	{
		return m_minX + (m_maxX - m_minX + 1) / 2;
	}

	int getCenterY() const
	{
		return m_minY + (m_maxY - m_minY + 1) / 2;
	}

	int getCenterZ() const
	{
		return m_minZ + (m_maxZ - m_minZ + 1) / 2;
	}

	String toString() const
	{
		char buff[64];
		sprintf(buff, "(%d, %d, %d; %d, %d, %d)", m_minX, m_minY, m_minZ, m_maxX, m_maxY, m_maxZ);
		return buff;
	}

};

}

#endif
