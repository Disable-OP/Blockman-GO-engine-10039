#ifndef __ID_MAPPING_HEADER__
#define __ID_MAPPING_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class IdMapping
	{
	public:
		static int readConfig(const LORD::String& filePath);
		static int getBlockId(const LORD::String& blockName)
		{
			auto it = blockNameToId.find(blockName);
			return it == blockNameToId.end() ? -1 : it->second;
		}
		static LORD::String getBlockName(int blockId)
		{
			auto it = blockIdToName.find(blockId);
			return it == blockIdToName.end() ? "" : it->second;
		}

	private:
		static LORD::map<int, LORD::String>::type blockIdToName;
		static LORD::map<LORD::String, int>::type blockNameToId;

		static int parseIniHandler(void* user, const char* section, const char* name, const char* value);
	};
}

#endif // !__ID_MAPPING_HEADER__
