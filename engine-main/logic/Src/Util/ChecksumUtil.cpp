#include "ChecksumUtil.h"
#include <fstream>
#include "Util/md5.h"

namespace BLOCKMAN
{
	template<size_t hashSize>
	bool ChecksumUtil::readLine(std::FILE * fp, Hash<hashSize>& hashOutput, char * fileNameOutput)
	{
		static_assert(hashSize <= 128, "readLine does not support hash size larger than 128 bytes");
		char buffer[257];
		if (fscanf(fp, "%256s%*2c%255s", buffer, fileNameOutput) != 2)
		{
			if (!feof(fp))
			{
				LordLogWarning("failed to read a entry in the checksum file: format of the line is incorrect");
			}
			return false;
		}
		if (strlen(buffer) != hashSize * 2)
		{
			LordLogWarning("failed to read a entry in the checksum file: size of hash is incorrect");
			return false;
		}
		if (strlen(fileNameOutput) >= 255)
		{
			LordLogWarning("failed to read a entry in the checksum file: size of file name is too large");
			return false;
		}
		for (size_t i = 0; i < hashSize; ++i)
		{
			if (sscanf(buffer + 2 * i, "%2hhx", hashOutput.data() + i) != 1)
			{
				LordLogWarning("failed to read a entry in the checksum file: hash is not a valid hex string");
				return false;
			}
		}
		return true;
	}

	template<size_t hashSize>
	ChecksumUtil::File2HashMap<hashSize> ChecksumUtil::parseChecksumFile(const String& checksumFilePath)
	{
		File2HashMap<hashSize> map;
		auto fp = fopen(checksumFilePath.c_str(), "r");
		if (fp != NULL)
		{
			FileGuard guard(fp);
			char file[256];
			Hash<hashSize> hash;
			for (int counter = 0; true; ++counter)
			{
				if (!readLine(fp, hash, file))
				{
					if (feof(fp))
					{
						break;
					}
					else
					{
						LordLogError("failed to parse entry %d, skip this entry", counter);
						fscanf(fp, "%*[^\n]\n");
						continue;
					}
				}
				auto iter = map.find(file);
				if (iter != map.end())
				{
					LordLogWarning("multiple entries are written in checksums for the same file: %s", file);
				}
				map[file] = hash;
			}
		}
		return map;
	}

	size_t ChecksumUtil::getEntryCount(const String & checksumFilePath)
	{
		if (StringUtil::EndWith(checksumFilePath, ".md5"))
		{
			return parseChecksumFile<16>(checksumFilePath).size();
		}
		else
		{
			LordLogError("cannot identify the hash algorithm of %s", checksumFilePath.c_str());
			return 0;
		}
	}

	bool ChecksumUtil::verify(const String & checksumFilePath)
	{
		if (StringUtil::EndWith(checksumFilePath, ".md5"))
		{
			return verifyMd5(checksumFilePath);
		}
		else
		{
			LordLogError("cannot identify the hash algorithm of %s", checksumFilePath.c_str());
			return false;
		}
	}

	bool ChecksumUtil::verifyMd5(const String & checksumFilePath)
	{
		auto file2HashMap = parseChecksumFile<16>(checksumFilePath);
		for (const auto& pair : file2HashMap)
		{
			String path = StringUtil::Replace(pair.first, "\\", "/");
			auto dir = PathUtil::GetFileDirPath(checksumFilePath);
			auto filePath = PathUtil::ConcatPath(dir, path.c_str());
			std::ifstream is(filePath.c_str(), std::ios_base::binary);
			if (!is)
			{
				LordLogError("checksum failed: file does not exist: %s", pair.first.c_str());
				return false;
			}
			MD5 md5(is);
			auto md5Bytes = md5.digest();
			if (memcmp(md5Bytes, pair.second.data(), 16) != 0)
			{
				LordLogError("checksum failed: hash of file %s does not match", pair.first.c_str());
				return false;
			}
		}
		return true;
	}
}
