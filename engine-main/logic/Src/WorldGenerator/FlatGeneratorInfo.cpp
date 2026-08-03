#include "FlatGeneratorInfo.h"
#include "BM_TypeDef.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{

void FlatGeneratorInfo::updateLayers()
{
	int i = 0;

	for (FlatLayerInfoLst::iterator it = m_flatLayers.begin(); it!=m_flatLayers.end(); ++it)
	{
		it->setMinY(i);
		i += it->getLayerCount();
	}
}

String FlatGeneratorInfo::toString()
{
	String result = "2;";
	
	int i = 0;
	for (FlatLayerInfoLst::iterator it = m_flatLayers.begin(); it!=m_flatLayers.end(); ++it)
	{
		if (i++ > 0)
			result += ",";
		result += it->toString();
	}
	
	result += ";";
	result += StringUtil::ToString(m_biomeToUse);

	if (!m_worldFeatures.empty())
	{
		result += ";";
		i = 0;

		for (StringMapMap::iterator it = m_worldFeatures.begin(); it != m_worldFeatures.end(); ++it)
		{
			if (i++ > 0)
				result += ",";
			String temp = it->first;
			StringUtil::LowerCase(temp);
			result += temp;

			StringMap& stringMap = it->second;
			int var6 = 0;
			for (StringMap::iterator jt = stringMap.begin(); jt!=stringMap.end(); ++jt)
			{
				if (var6++ > 0)
				{
					result += " ";
				}
				result += jt->first;
				result += "=";
				result += jt->second;
			}
			result += ")";
		}
	}
	else
	{
		result.append(";");
	}

	return result;
}

FlatGeneratorInfo* FlatGeneratorInfo::createFlatGeneratorFromString(const String& str)
{
	if (str == StringUtil::BLANK)
	{
		return getDefaultFlatGenerator();
	}
	else
	{
		StringArray strs0 = StringUtil::Split(str, ";");
		int count = strs0.size() == 1 ? 0 : StringUtil::ParseInt(strs0[0]);

		if (count >= 0 && count <= 2)
		{
			FlatGeneratorInfo* pResult = LordNew FlatGeneratorInfo();
			int index = strs0.size() == 1 ? 0 : 1;
			FlatLayerInfoLst infoLst = getDefaultFlatGenerator(strs0[index++]);

			if (!infoLst.empty())
			{
				for (FlatLayerInfoLst::iterator it = infoLst.begin(); it != infoLst.end(); ++it)
				{
					pResult->getFlatLayers().push_back(*it);
				}
				
				pResult->updateLayers();
				int biome = BIOME_GEN_PLAINS;

				if (count > 0 && int(strs0.size()) > index)
				{
					biome = StringUtil::ParseInt(strs0[index++], biome);
				}

				pResult->setBiome(biome);

				if (count > 0 && int(strs0.size()) > index)
				{
					String lowcases = strs0[index++];
					StringUtil::LowerCase(lowcases);
					StringArray strs1 = StringUtil::Split(lowcases, ",");

					for (int i = 0; i < int(strs1.size()); ++i)
					{
						String str = strs1[i];
						StringArray strs2 = StringUtil::Split(str, "\\(", 2);
						StringMap strMap;


						if (strs2[0].length() > 0)
						{
							if (strs2.size() > 1 && strs2[1].size() > 1 && strs2[1][strs2[1].length()-1] == ')' )
							{
								String substring = strs2[1].substr(0, strs2[1].length() - 1);
								StringArray strs3 = StringUtil::Split(substring, " ");

								for (int j = 0; j < int(strs3.size()); ++j)
								{
									StringArray strs4 = StringUtil::Split(strs3[j], "=", 2);

									if (strs4.size() == 2)
									{
										strMap.insert(std::make_pair(strs4[0], strs4[1]));
									}
								}
							}
							pResult->getWorldFeatures()[strs2[0]] = strMap; // insert.
						}
					}
				}
				else
				{
					StringMap blankMap;
					pResult->getWorldFeatures()[String("village")] = blankMap;
				}

				return pResult;
			}
			else
			{
				return getDefaultFlatGenerator();
			}
		}
		else
		{
			return getDefaultFlatGenerator();
		}
	}

	return NULL; //silience warning.
}

FlatGeneratorInfo* FlatGeneratorInfo::getDefaultFlatGenerator()
{
	FlatGeneratorInfo* pResult = LordNew FlatGeneratorInfo();
	pResult->setBiome(BIOME_GEN_PLAINS);
	pResult->getFlatLayers().push_back(FlatLayerInfo(1, BLOCK_ID_BEDROCK));
	pResult->getFlatLayers().push_back(FlatLayerInfo(2, BLOCK_ID_DIRT));
	pResult->getFlatLayers().push_back(FlatLayerInfo(1, BLOCK_ID_GRASS));
	pResult->updateLayers();
	pResult->getWorldFeatures()["village"] = StringMap();
	return pResult;
}

FlatLayerInfo FlatGeneratorInfo::getLayersFromString(const String& str, int miny)
{
	StringArray strs = StringUtil::Split(str, "x", 2);
	int count = 1;
	int meta = 0;

	if (strs.size() == 2)
	{
		count = StringUtil::ParseInt(strs[0]);

		if (miny + count >= 256)
		{
			count = 256 - miny;
		}

		if (count < 0)
		{
			count = 0;
		}
	}

	String var6 = strs[strs.size() - 1];
	strs = StringUtil::Split(var6, ":", 2);
	int id = StringUtil::ParseInt(strs[0]);

	if (strs.size() > 1)
	{
		meta = StringUtil::ParseInt(strs[1]);
	}

	if (BlockManager::sBlocks[id] == NULL)
	{
		id = 0;
		meta = 0;
	}

	if (meta < 0 || meta > 15)
	{
		meta = 0;
	}

	FlatLayerInfo result(count, id, meta);
	result.setMinY(miny);
	return result;
}

FlatLayerInfoLst FlatGeneratorInfo::getDefaultFlatGenerator(const String& str)
{
	FlatLayerInfoLst lst;
	if (str.length() >= 1)
	{
		StringArray strs = StringUtil::Split(str, ",");
		int count = 0;

		for (int i = 0; i < int(strs.size()); ++i)
		{
			String str2 = strs[i];
			FlatLayerInfo var8 = getLayersFromString(str2, count);

			lst.push_back(var8);
			count += var8.getLayerCount();
		}
	}

	return lst;
}

}
