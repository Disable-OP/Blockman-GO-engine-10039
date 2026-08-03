#ifndef __CHECKSUM_UTIL_HEADER__
#define __CHECKSUM_UTIL_HEADER__

#include <array>
#include <cstdio>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ChecksumUtil
	{
	private:
		template<size_t hashSize>
		using Hash = std::array<ui8, hashSize>;
		template<size_t hashSize>
		using File2HashMap = typename map<String, Hash<hashSize>>::type;
		class FileGuard
		{
		private:
			std::FILE* m_fp;
		public:
			FileGuard(std::FILE* fp) : m_fp(fp) {}
			FileGuard(FileGuard&) = delete;
			FileGuard(FileGuard&&) = delete;
			FileGuard& operator=(FileGuard&) = delete;
			FileGuard& operator=(FileGuard&&) = delete;
			~FileGuard() { fclose(m_fp); }
		};

		template<size_t hashSize>
		static bool readLine(std::FILE* fp, Hash<hashSize>& md5Output, char* fileNameOutput);
		template<size_t hashSize>
		static File2HashMap<hashSize> parseChecksumFile(const String& checksumFilePath);

	public:
		static size_t getEntryCount(const String& checksumFilePath);
		static bool verify(const String& checksumFilePath);
		static bool verifyMd5(const String& checksumFilePath);
	};
}

#endif // !__CHECKSUM_UTIL_HEADER__
