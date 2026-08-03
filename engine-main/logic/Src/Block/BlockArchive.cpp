#include "BlockArchive.h"

using namespace LORD;

namespace BLOCKMAN
{
	bool BlockArchive::isCaseSensitive(void) const
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return false;
#else
		return true;
#endif
	}

	void BlockArchive::load()
	{
	}

	void BlockArchive::unload()
	{
	}

	DataStream * BlockArchive::open(const String & strFilename)
	{
		return FileSystemArchive::open(removePrefix(strFilename));
	}

	StringArray BlockArchive::list(bool bRecursive, bool bDirs)
	{
		static constexpr char *files[] = {
			"texture.json",
			"texture.png"
		};
		StringArray ret;
		for (const auto& filename : files)
		{
			if (FileSystemArchive::exists(filename))
			{
				ret.push_back(addPrefix(filename));
			}
		}
		return ret;
	}

	String BlockArchive::location(const String & filename)
	{
		return FileSystemArchive::location(removePrefix(filename));
	}

	bool BlockArchive::exists(const String & strFilename)
	{
		return FileSystemArchive::exists(removePrefix(strFilename));
	}
}