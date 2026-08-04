/********************************************************************
filename: 	FlatGeneratorInfo.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-11
*********************************************************************/
#ifndef __FLAT_GENERATOR_INFO_HEADER__
#define __FLAT_GENERATOR_INFO_HEADER__

#include "FlatLayerInfo.h"
#include "BM_Container_def.h"

namespace BLOCKMAN
{

typedef list<FlatLayerInfo>::type FlatLayerInfoLst;


class FlatGeneratorInfo : public ObjectAlloc
{
public:
	/** Return the biome used on this preset.*/
	int getBiome() const { return m_biomeToUse; }

	/** Set the biome used on this preset. */
	void setBiome(int biome) { m_biomeToUse = biome; }

	/** Return the list of world features enabled on this preset. */
	StringMapMap& getWorldFeatures() { return m_worldFeatures; }

	/** Return the list of layers on this preset. */
	FlatLayerInfoLst& getFlatLayers() { return m_flatLayers; }

	void updateLayers();

	String toString();

	static FlatGeneratorInfo* createFlatGeneratorFromString(const String& str);

	static FlatGeneratorInfo* getDefaultFlatGenerator();

protected:
	static FlatLayerInfo getLayersFromString(const String& str, int miny);

	static FlatLayerInfoLst getDefaultFlatGenerator(const String& str);

protected:
	/** List of layers on this preset. */
	FlatLayerInfoLst m_flatLayers;

	/** List of world features enabled on this preset. */
	StringMapMap m_worldFeatures;
	int m_biomeToUse = 0;

};

}
#endif