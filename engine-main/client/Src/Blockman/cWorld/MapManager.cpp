#include "MapManager.h"
#include <fstream>
#include "cpr/cpr.h"
#include "Resource/ZipArchive.h"
#include "libzippp.h"
#include "Util/ChecksumUtil.h"
#include "Util/Concurrent/WorkerThread.h"

#define RETURN_ON_FAIL(expr, msg, ...) if (!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }
#define REJECT_PROMISE_ON_FAIL(expr, promise, msg, ...) if (!(expr)) { (promise).reject(StringUtil::Format(msg, ##__VA_ARGS__)); return false; }

namespace GameClient
{

	const char* MapManager::DOWNLOAD_ADDRESS = "http://cloudatlasoft.u.qiniudn.com/";

	bool MapManager::mapExistsAndValid(const String & mapName, const std::string& mapNameReal)
	{
		// root dir is whether exist.
		String mapRootPath = getMapRootPath();
		RETURN_ON_FAIL(PathUtil::IsDirExist(mapRootPath), "mapExistsAndValid root dir is missing : %s", mapRootPath.c_str());

		// mapNameId dir is whether exist.
		String mapIdPath = getMapPath(mapName);
		RETURN_ON_FAIL(PathUtil::IsDirExist(mapIdPath), "mapExistsAndValid mapNameId dir is missing : %s", mapIdPath.c_str());

		// temp.zip is whether exist.
		String mapTempZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + "_temp.zip").c_str());
		RETURN_ON_FAIL(!PathUtil::IsFileExist(mapTempZipPath), "mapExistsAndValid temp.zip is exist, should be broken-point continuingly-transferring : %s", mapTempZipPath.c_str());

		// map.zip is whether exist.
		String mapZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + ".zip").c_str());
		RETURN_ON_FAIL(PathUtil::IsFileExist(mapZipPath), "mapExistsAndValid map is missing : %s", mapZipPath.c_str());

		// create map dir
		String mapPath = PathUtil::ConcatPath(mapIdPath, mapNameReal.c_str());
		if (!PathUtil::IsDirExist(mapPath.c_str()) && !PathUtil::CreateDir(mapPath))
		{
			LordLogError("mapExistsAndValid failed to create map path %s", mapPath.c_str());
		}

		// Unzip map.zip, then check md5
		libzippp::ZipArchive archive(mapZipPath.c_str());
		if (!archive.open())
		{
			PathUtil::DelPath(mapZipPath);
			return false;
		}
		for (const auto& entry : archive.getEntries())
		{
			auto filePath = PathUtil::ConcatPath(mapPath, entry.getName().c_str());
			if (entry.isDirectory())
			{
				if (!PathUtil::IsDirExist(filePath.c_str())) 
				{
					PathUtil::CreateDir(filePath.c_str());
				}
			}
			else
			{
				std::ofstream os(filePath.c_str(), std::ios::binary | std::ios::trunc);
				entry.readContent(os);
			}
		}
		archive.close();
		// check md5
		RETURN_ON_FAIL(checkMd5(mapPath), "checksums.md5 error : %s", mapNameReal.c_str());
		return true;
	}

	bool MapManager::checkMd5(const String & mapPath)
	{
		String checksumPath = PathUtil::ConcatPath(mapPath, "checksums.md5");
		RETURN_ON_FAIL(PathUtil::IsFileExist(checksumPath), "checksums.md5 is missing in the map %s", mapPath.c_str());
		RETURN_ON_FAIL(BLOCKMAN::ChecksumUtil::verify(checksumPath), "map %s failed the checksum verification", mapPath.c_str());
		StringArray fileList;
		PathUtil::EnumFilesInDir(fileList, mapPath, false, true);
		RETURN_ON_FAIL(fileList.size() == BLOCKMAN::ChecksumUtil::getEntryCount(checksumPath) + 1, "number of files in map %s is incorrect", mapPath.c_str());

		return true;
	}

	void MapManager::delelteMap(const String & mapName, const String & mapUrl)
	{
		// parse mapNameReal from mapUrl
		std::string mapUrlStr(mapUrl.c_str());
		std::string mapNameReal("");

		if (mapUrlStr.find_last_of("/") != mapUrlStr.npos)
		{
			mapNameReal = mapUrlStr.substr(mapUrlStr.find_last_of("/") + 1);
		}

		if (mapNameReal.find(".zip") != mapNameReal.npos)
		{
			mapNameReal = mapNameReal.substr(0, mapNameReal.find(".zip"));
		}

		String mapIdPath = getMapPath(mapName);
		String mapPath = PathUtil::ConcatPath(mapIdPath, mapNameReal.c_str());
		if (PathUtil::IsDirExist(mapPath))
		{
			PathUtil::DelPath(mapPath);
		}
	}

	MapManager::HttpPromise MapManager::downloadMap(const String & mapName, const String& mapUrl, const std::string& mapNameReal)
	{
		HttpPromise p;
		BLOCKMAN::WorkerThread::getDefault().addTask(&MapManager::downloadMapImpl, this, mapName, mapUrl, mapNameReal, p);
		return p;
	}

	bool MapManager::downloadMapImpl(const String & mapName, const String& mapUrl, const std::string& mapNameReal, HttpPromise promise)
	{
		// create root dir
		String mapRootPath = getMapRootPath();
		if (!PathUtil::IsDirExist(mapRootPath))
		{
			LordLogWarning("Map root path does not exists, create one");
			REJECT_PROMISE_ON_FAIL(PathUtil::CreateDir(mapRootPath), promise, "failed to create map root path");
		}

		// create mapNameId dir
		String mapIdPath = getMapPath(mapName);
		if (!PathUtil::IsDirExist(mapIdPath))
		{
			LordLogWarning("MapId path does not exists, create one");
			REJECT_PROMISE_ON_FAIL(PathUtil::CreateDir(mapIdPath), promise, "failed to create mapid path");
		}

		// temp.zip is whether exist.
		String mapTempZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + "_temp.zip").c_str());

		cpr::Response res;
		// broken-point continuingly-transferring.
		// Range identify the range of the file that need to request
		if (PathUtil::IsFileExist(mapTempZipPath))
		{
			std::ifstream isTemp(mapTempZipPath.c_str(), std::ios::binary | std::ios::ate);
			auto range_size = isTemp.tellg();

			char range_buf[128];
			sprintf(range_buf, "bytes=%d-", (unsigned int)range_size);
			std::string strRange(range_buf);

			res = cpr::Get(
				cpr::Url(mapUrl.c_str()),
				cpr::ConnectTimeout(std::chrono::seconds(10)),
				cpr::Timeout(180000),
				cpr::Header{ { "Range", strRange } },
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
		}
		// download all file
		else
		{
			// delete all files in mapNameId dir.
			PathUtil::DelFilesInDir(mapIdPath);

			res = cpr::Get(
				cpr::Url(mapUrl.c_str()),
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
		}

		REJECT_PROMISE_ON_FAIL(res.status_code >= 200 && res.status_code < 300, promise,
			"Failed to download the map %s. HTTP status code: %d", mapName.c_str(), res.status_code);

		std::ofstream os(mapTempZipPath.c_str(), std::ios::binary | std::ios::app);
		os.write(res.text.data(), res.text.size());
		os.flush();
		os.close();
		if (!os)
		{
			promise.reject(StringUtil::Format("failed to write downloaded map to file: %s", mapTempZipPath.c_str()));
			PathUtil::DelPath(mapTempZipPath);
			return false;
		}

		REJECT_PROMISE_ON_FAIL(!res.error, promise, "Error occured while downloading the map %s: code: %d, msg: %s",
			mapName.c_str(), static_cast<int>(res.error.code), res.error.message.c_str());

		// rename temp.zip to map.zip
		String mapZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + ".zip").c_str());
		PathUtil::RenameFile(mapTempZipPath, mapZipPath);

		// create map dir
		String mapPath = PathUtil::ConcatPath(mapIdPath, mapNameReal.c_str());
		REJECT_PROMISE_ON_FAIL(PathUtil::CreateDir(mapPath), promise, "failed to create map path %s", mapPath.c_str());

		// unzip map
		libzippp::ZipArchive archive(mapZipPath.c_str());
		if (!archive.open())
		{
			promise.reject(StringUtil::Format("failed to open zip file: %s", mapZipPath.c_str()));
			PathUtil::DelPath(mapZipPath);
			return false;
		}
		for (const auto& entry : archive.getEntries())
		{
			auto filePath = PathUtil::ConcatPath(mapPath, entry.getName().c_str());
			if (entry.isDirectory())
			{
				if (!PathUtil::IsDirExist(filePath.c_str())) {
					PathUtil::CreateDir(filePath.c_str());
				}
			}
			else
			{
				std::ofstream os(filePath.c_str(), std::ios::binary | std::ios::trunc);
				entry.readContent(os);
			}
		}
		archive.close();

		// check md5
		REJECT_PROMISE_ON_FAIL(checkMd5(mapPath), promise, "downloadMapImpl checksums.md5 error : %s", mapNameReal.c_str());
		promise.fulfill();
		return true;
	}
}
