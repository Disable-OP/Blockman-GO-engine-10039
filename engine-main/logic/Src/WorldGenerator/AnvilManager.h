#ifndef __ANVIL_MANAGER_HEADER__
#define __ANVIL_MANAGER_HEADER__

#include <memory>
#include <utility>
#include <stdexcept>
#include "Core.h"
#include "WorldGenerator/Anvil.h"
#include "Std/shared_mutex.h"

namespace BLOCKMAN
{
	class Anvil;

	// This class is thread safe
	class AnvilManager
	{
	private:
		using unique_lock = std::unique_lock<std::shared_mutex>;
		using shared_lock = std::shared_lock<std::shared_mutex>;
		map<std::pair<int, int>, std::shared_ptr<Anvil>>::type m_cache;
		const String m_regionDir;
		std::shared_mutex m_mutex;

		void loadAnvil(const String& fileName);
		virtual std::shared_ptr<Anvil> createAnvil(int anvilX, int anvilZ) const = 0;
		std::shared_ptr<Anvil> getAnvilFromCache(int anvilX, int anvilZ);

	public:
		AnvilManager() = default;
		AnvilManager(const String& regionDir) : m_regionDir(regionDir) {}
		std::shared_ptr<Anvil> getAnvil(int anvilX, int anvilZ);
		vector<std::shared_ptr<Anvil>>::type getAllAnvilsInCache();
		void saveAnvil(std::shared_ptr<Anvil> pAnvil);
		void saveAllAnvilsInCache();
		void loadAnvil(int anvilX, int anvilZ);
		void loadAllAnvilsInDirectory();
		bool anvilFileExists(int anvilX, int anvilZ);
		std::shared_ptr<Anvil> createNewAnvilFile(int anvilX, int anvilZ);

		String getRegionDir() const noexcept
		{
			return m_regionDir;
		}

		String getAnvilFilePath(int anvilX, int anvilZ) const
		{
			return StringUtil::concatToString(m_regionDir, "r.", anvilX, '.', anvilZ, ".mca");
		}
	};

	class file_not_found : public std::runtime_error
	{
	public:
		template<typename CharT, typename Traits, typename Alloc>
		file_not_found(const std::basic_string<CharT, Traits, Alloc>& msg) : std::runtime_error(msg.c_str()) {}
	};
}

#endif