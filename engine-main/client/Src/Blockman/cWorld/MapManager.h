#ifndef __MAP_MANAGER_HEADER__
#define __MAP_MANAGER_HEADER__

#include <atomic>
#include "Core.h"
#include "Object/Root.h"
#include "Util/PathUtil.h"
#include "Util/HttpPromise.h"

using namespace LORD;

namespace GameClient
{
	class MapManager : public Singleton<MapManager>, public ObjectAlloc
	{
	private:
		
		String m_mapRootPath;
		static const char* DOWNLOAD_ADDRESS;
		using HttpPromise = BLOCKMAN::HttpPromise;
		bool downloadMapImpl(const String& mapName, const String& mapUrl, const std::string& mapNameReal, HttpPromise promise);

	public:
		MapManager()
		{
			m_mapRootPath = PathUtil::ConcatPath(Root::Instance()->getMapPath());
		}

		String getMapRootPath() const
		{
			return m_mapRootPath;
		}

		String getMapPath(const String& mapName) const
		{
			return PathUtil::ConcatPath(getMapRootPath(), mapName);
		}

		bool mapExistsAndValid(const String& mapName, const std::string& mapNameReal);
		HttpPromise downloadMap(const String& mapName, const String& mapUrl, const std::string& mapNameReal);
		bool checkMd5(const String& mapPath);
		void delelteMap(const String& mapName, const String& mapUrl);
	};
}

#endif // !__MAP_MANAGER_HEADER__
