#include "IdMapping.h"
#include "Util/ini.h"
#include "Block/BlockUtil.h"

namespace BLOCKMAN
{
	using namespace LORD;

	map<int, String>::type IdMapping::blockIdToName;
	map<String, int>::type IdMapping::blockNameToId;

	int IdMapping::readConfig(const String & filePath)
	{
		return ini_parse(filePath.c_str(), &IdMapping::parseIniHandler, 0);;
	}

	int IdMapping::parseIniHandler(void * user, const char * section, const char * name, const char * value)
	{
		if (strcmp(section, "Block") == 0)
		{
			int id = StringUtil::ParseInt(name, -1);
			if (!BlockUtil::isCustomBlock(id))
			{
				LordLogWarning("Invalid ID in id_mappings.ini: %d is not valid for custom block", id);
				return 0;
			}
			blockIdToName[id] = value;
			blockNameToId[value] = id;
			return 1;
		}
		return 0;
	}
}
