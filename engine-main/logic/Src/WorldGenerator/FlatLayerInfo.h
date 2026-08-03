/********************************************************************
filename: 	FlatLayerInfo.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-11
*********************************************************************/
#ifndef __FLAT_LAYER_INFO_HEADER__
#define __FLAT_LAYER_INFO_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class FlatLayerInfo : public ObjectAlloc
{
public:
	FlatLayerInfo(int count, int blockID)
		: m_layerCount(count)
		, m_layerFillBlock(blockID)
	{ }

	FlatLayerInfo(int count, int blockID, int meta)
		: m_layerCount(count)
		, m_layerFillBlock(blockID)
		, m_layerFillMeta(meta)
	{ }

	/** Return the amount of layers for this set of layers. */
	int getLayerCount() const { return m_layerCount; }

	/** Return the block type used on this set of layers. */
	int getFillBlock() const { return m_layerFillBlock; }

	/** Return the block metadata used on this set of layers. */
	int getFillBlockMeta() const { return m_layerFillMeta; }

	/** Return the minimum Y coordinate for this layer, set during generation. */
	int getMinY() const { return m_layerMinimumY; }

	/** Set the minimum Y coordinate for this layer. */
	void setMinY(int minY) { m_layerMinimumY = minY; }

	String toString()
	{
		String strResult = StringUtil::ToString(m_layerFillBlock);

		if (m_layerCount > 1)
		{
			strResult = StringUtil::ToString(m_layerCount) + "x" + strResult;
		}

		if (m_layerFillMeta > 0)
		{
			strResult = strResult + ":" + StringUtil::ToString(m_layerCount);
		}

		return strResult;
	}

protected:
	/** Amount of layers for this set of layers. */
	int m_layerCount = 0;

	/** Block type used on this set of layers. */
	int m_layerFillBlock = 0;

	/** Block metadata used on this set of laeyrs. */
	int m_layerFillMeta = 0;
	int m_layerMinimumY = 0;
};


}
#endif