#include "AnvilManager.h"
#include <algorithm>
#include <cstdio>
#include "Util/PathUtil.h"

namespace BLOCKMAN
{
        void AnvilManager::loadAnvil(const String& fileName)
        {
                int x, z;
                if (sscanf(fileName.c_str(), "r.%d.%d.mca", &x, &z) == 2)
                {
                        loadAnvil(x, z);
                }
                else
                {
                        LordLogWarning("load anvil file failed, invalid file name: %s", fileName.c_str());
                }
        }

        std::shared_ptr<Anvil> AnvilManager::getAnvilFromCache(int anvilX, int anvilZ)
        {
                shared_lock lock(m_mutex);
                auto iter = m_cache.find({ anvilX, anvilZ });
                if (iter == m_cache.end())
                {
                        return nullptr;
                }
                return iter->second;
        }

        std::shared_ptr<Anvil> AnvilManager::getAnvil(int x, int z)
        {
                auto anvil = getAnvilFromCache(x, z);
                if (!anvil)
                {
                        loadAnvil(x, z);
                }
                return getAnvilFromCache(x, z);
        }

        vector<std::shared_ptr<Anvil>>::type AnvilManager::getAllAnvilsInCache()
        {
                vector<std::shared_ptr<Anvil>>::type ret;
                {
                        shared_lock lock(m_mutex);
                        for (const auto& pair : m_cache)
                        {
                                ret.push_back(pair.second);
                        }
                }
                return ret;
        }

        void AnvilManager::saveAnvil(std::shared_ptr<Anvil> pAnvil)
        {
                auto filePath = getAnvilFilePath(pAnvil->getX(), pAnvil->getZ());
                std::ofstream stream(filePath.c_str(), std::ios_base::out | std::ios_base::binary);
                if (!stream) {
                        // TODO: log error
                        return;
                }
                pAnvil->serialize(stream << std::noskipws);
        }

        void AnvilManager::saveAllAnvilsInCache()
        {
                shared_lock lock(m_mutex);
                for (auto pair : m_cache)
                {
                        saveAnvil(pair.second);
                }
        }

        void AnvilManager::loadAnvil(int anvilX, int anvilZ)
        {
                static int regAnvilError = 0;
                auto filePath = getAnvilFilePath(anvilX, anvilZ);
                if (PathUtil::IsFileExist(filePath))
                {
                        LordLogInfo("begin to load anvil file %s, size %d", filePath.c_str(), (int)PathUtil::GetFileSize(filePath));

                        auto pAnvil = createAnvil(anvilX, anvilZ);
                        auto filePath = getAnvilFilePath(anvilX, anvilZ);
                        std::ifstream stream(filePath.c_str(), std::ios_base::in | std::ios_base::binary);
                        if (!stream.good()) {
                                LordLogError("Failed to open anvil file: %s, reason: %s", filePath.c_str(), strerror(errno));
                                return;
                        }

                        try
                        {
                                if (!pAnvil->deserialize(stream >> std::noskipws))
                                {
                                        LordLogError("Failed to read anvil file: %s", filePath.c_str());
                                        return;
                                }
                        }
                        catch (std::exception& e)
                        {
                                // A region file that fails to deserialize is unusable. The old
                                // code did `throw 0;` here — an unhandled exception escaping the
                                // server's EntryThread kills the whole process (std::terminate),
                                // i.e. one corrupt .mca file (e.g. after a crash mid-write)
                                // permanently crashed the app on every launch. Instead: quarantine
                                // the corrupt file (rename, keep for inspection) and continue —
                                // chunks regenerate deterministically from the seed.
                                LordLogError("anvil file failed to deserialize: %s exception: %s strerror: %s — quarantining file",
                                        filePath.c_str(), e.what(), strerror(errno));
                                String quarantine = filePath + ".corrupt";
                                std::remove(quarantine.c_str());
                                if (std::rename(filePath.c_str(), quarantine.c_str()) != 0)
                                {
                                        LordLogError("failed to quarantine corrupt anvil file: %s (errno: %s)",
                                                quarantine.c_str(), strerror(errno));
                                }
                                return;
                        }
                        
                        {
                                unique_lock lock(m_mutex);
                                m_cache.insert({ { anvilX, anvilZ }, pAnvil });
                        }
                }
                else if(regAnvilError == 0)
                {
                        LordLogError("anvil file does not exists: %s", filePath.c_str());
                        regAnvilError = 100;
                }
                else
                        regAnvilError--;
        }

        void AnvilManager::loadAllAnvilsInDirectory()
        {
                if (!PathUtil::IsDirExist(m_regionDir))
                {
                        LordLogWarning("region directory is not found: %s", m_regionDir.c_str());
                        return;
                }
                StringArray files;
                PathUtil::EnumFilesInDir(files, m_regionDir);
                for (const auto& file : files)
                {
                        loadAnvil(file);
                }
        }

        bool AnvilManager::anvilFileExists(int anvilX, int anvilZ)
        {
                return PathUtil::IsFileExist(getAnvilFilePath(anvilX, anvilZ));
        }

        std::shared_ptr<Anvil> AnvilManager::createNewAnvilFile(int anvilX, int anvilZ)
        {
                if (anvilFileExists(anvilX, anvilZ))
                {
                        LordLogWarning("tried to create an anvil at %d, %d, but anvil already exists", anvilX, anvilZ);
                        return getAnvil(anvilX, anvilZ);
                }

                auto newAnvil = createAnvil(anvilX, anvilZ);
                saveAnvil(newAnvil);
                {
                        unique_lock lock(m_mutex);
                        m_cache.insert({ { anvilX, anvilZ }, newAnvil });
                }
                return newAnvil;
        }
}