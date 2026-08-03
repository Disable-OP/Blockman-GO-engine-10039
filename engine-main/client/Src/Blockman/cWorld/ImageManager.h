#pragma once
#include <atomic>
#include "Core.h"
#include "Object/Root.h"
#include "Util/PathUtil.h"
#include "Util/HttpPromise.h"

using namespace LORD;

namespace GameClient
{
	class ImageManager : public Singleton<ImageManager>, public ObjectAlloc
	{
	private:
		String m_urlImageRootPath;
		using HttpPromise = BLOCKMAN::HttpPromise;
		std::map<String, String> m_urlImageMap;

	public:
		ImageManager();
		~ImageManager();

		HttpPromise downloadImage(const String& imageUrl);

		String getUrlImageRootPath() const { return m_urlImageRootPath; }
		String getImageName(String url);
		void setUrlImageDownloading(String url) { m_urlImageMap[url] = "downloading"; }
		void deleteUrlImage(String url) { m_urlImageMap.erase(url); }

	private:
		bool downloadImageImpl(const String& imageUrl, HttpPromise promise);
		bool loadUrlConfig();
		bool saveUrlConfig();
		bool imageExists(const String & imageName);
	};
}