#include "ImageManager.h"
#include <fstream>
#include "cpr/cpr.h"
#include "Resource/ZipArchive.h"
#include "libzippp.h"
#include "Util/ChecksumUtil.h"
#include "Util/Concurrent/WorkerThread.h"
#include "Util/CSVReader.h"
#include "Util/CSVWriter.h"

#define RETURN_ON_FAIL(expr, msg, ...) if (!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace GameClient
{
	ImageManager::ImageManager()
	{
		m_urlImageRootPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Textures/url/");
		loadUrlConfig();
	}

	ImageManager::~ImageManager()
	{
		saveUrlConfig();
	}

	ImageManager::HttpPromise ImageManager::downloadImage(const String& imageUrl)
	{
		HttpPromise p;
		BLOCKMAN::WorkerThread::getDefault().addTask(&ImageManager::downloadImageImpl, this, imageUrl, p);
		return p;
	}

	String ImageManager::getImageName(String url)
	{
		String result = "";
		auto iter = m_urlImageMap.find(url);
		if (iter != m_urlImageMap.end())
		{
			result = iter->second;
		}
		return result;
	}

	bool ImageManager::downloadImageImpl(const String& imageUrl, HttpPromise promise)
	{
		if (getImageName(imageUrl) != "downloading")
		{
			return false;
		}

		static int imageNameCount = 0;
		String imgName = "image_" + PathUtil::GetTime() + "_" + StringUtil::ToString(imageNameCount++) + ".png";

		String imageRootPath = getUrlImageRootPath();
		if (!PathUtil::IsDirExist(imageRootPath))
		{
			LordLogWarning("url image root path does not exists, create one");
			if (!PathUtil::CreateDir(imageRootPath))
			{
				promise.reject(StringUtil::Format("failed to create url image root path"));
				deleteUrlImage(imageUrl);
				return false;
			}
		}

		cpr::Response res = cpr::Get(
			cpr::Url(imageUrl.c_str()),
			cpr::ConnectTimeout(std::chrono::seconds(10)),
			cpr::Timeout(180000),
			cpr::XferInfo([promise](const cpr::TransferProgress& progress2) mutable -> int
		{
			if (progress2.downloadTotal == 0)
			{
				promise.progress(0.0, 0.0);
			}
			else
			{
				promise.progress(static_cast<double>(progress2.downloadNow) / progress2.downloadTotal, static_cast<float>(progress2.downloadTotal));
			}
			return 0;
		}));

		if (res.status_code < 200 || res.status_code >= 300)
		{
			promise.reject(StringUtil::Format("Failed to download the image %s. HTTP status code: %d", imgName.c_str(), res.status_code));
			deleteUrlImage(imageUrl);
			return false;
		}

		String imagePath = PathUtil::ConcatPath(imageRootPath, imgName.c_str());

		std::ofstream os(imagePath.c_str(), std::ios::binary | std::ios::trunc);
		os.write(res.text.data(), res.text.size());
		os.flush();
		os.close();

		if (!os)
		{
			promise.reject(StringUtil::Format("failed to write downloaded image to file: %s", imagePath.c_str()));
			PathUtil::DelPath(imagePath);
			deleteUrlImage(imageUrl);
			return false;
		}

		if (res.error)
		{
			promise.reject(StringUtil::Format("Error occured while downloading the image %s: code: %d, msg: %s", imgName.c_str(), static_cast<int>(res.error.code), res.error.message.c_str()));
			deleteUrlImage(imageUrl);
			return false;
		}

		m_urlImageMap[imageUrl] = imgName;
		promise.fulfill();
		return true;
	}

	bool ImageManager::loadUrlConfig()
	{
		String path = PathUtil::ConcatPath(getUrlImageRootPath(), "UrlConfig.csv");
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordLogError("Can not open url config file %s", path.c_str());
			return false;
		}
		String urlTemp;
		String imageTemp;
		while (pReader->Read())
		{
			pReader->GetString("ImageUrl", urlTemp);
			pReader->GetString("ImageName", imageTemp);
			if (imageExists(imageTemp))
			{
				m_urlImageMap[urlTemp] = imageTemp;
			}
		}
		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	bool ImageManager::saveUrlConfig()
	{
		String path = PathUtil::ConcatPath(getUrlImageRootPath(), "UrlConfig.csv");
		std::vector<std::string> header =
		{
			"ImageUrl", "ImageName"
		};
		CsvWriter* pWriter = new CsvWriter(path.c_str(), header);
		pWriter->CreateNew();

		for (auto iter : m_urlImageMap)
		{
			std::map<String, String> result =
			{
				{ "ImageUrl", iter.first },
				{ "ImageName", iter.second },
			};
			pWriter->Write(result);
		}

		pWriter->Close();
		LordSafeDelete(pWriter);
		return true;
	}

	bool ImageManager::imageExists(const String & imageName)
	{
		String urlImageRootPath = getUrlImageRootPath();
		RETURN_ON_FAIL(PathUtil::IsDirExist(urlImageRootPath), "url image root dir is missing : %s", urlImageRootPath.c_str());

		String urlImagePath = PathUtil::ConcatPath(urlImageRootPath, (imageName).c_str());
		RETURN_ON_FAIL(PathUtil::IsFileExist(urlImagePath), "url image is missing : %s", urlImagePath.c_str());

		return true;
	}
}
