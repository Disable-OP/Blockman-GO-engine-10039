#ifndef __CHUNK_SERVICE_HEADER__
#define __CHUNK_SERVICE_HEADER__

#include "Core.h"
#include "Util/Concurrent/WorkerThread.h"
#include "Chunk/ChunkCache.h"

namespace BLOCKMAN
{
        class World;
        class AnvilManager;
        class IChunkProvider;
        class IChunkStorage;
        class Chunk;
        using ChunkPtr = std::shared_ptr<Chunk>;

        class ChunkService : public LORD::ObjectAlloc
        {
        public:
                ChunkService(World* world, unsigned range, bool supportAsync) 
                        : m_cache(range)
                        , m_world(world)
                        , m_supportAsync(supportAsync)
                {
                        if (m_supportAsync) {
                                m_workerThread.start();
                        }
                }
                virtual ~ChunkService()
                {
                        // NOTE: m_workerThread must be stopped before the m_anvilManager is destructed
                        if (m_supportAsync) {
                                m_workerThread.stopSync();
                        }
                }

                virtual bool chunkInCache(int x, int z)
                {
                        return m_cache.inCache(x, z);
                }
                ChunkPtr getChunk(int x, int z);
                Future<ChunkPtr> getChunkAsync(int x, int z);
                void cancelAsyncGets()
                {
                        if (m_supportAsync) {
                                m_workerThread.cancelAllTasks();
                        }
                }
                void saveChunk(int x, int z, bool is_flush);
                // TODO: implement later
                Future<void> saveChunkAsync(int x, int z) = delete;
                void saveAllChunks(bool is_flush);
                // TODO: implement later
                Future<void> saveAllChunksAsync() = delete;
                void setCenter(int x, int z)
                {
                        m_cache.setCenter(x, z);
                }

                // Server-authoritative worldgen: inject a chunk that arrived over
                // the network (S2CPacketChunkData on the client). Adds it to the
                // cache and runs prepareChunk so the renderer picks it up. Safe
                // to call from any thread — the cache uses a mutex internally.
                void injectChunk(const ChunkPtr& chunk);

                void reset()
                {
                        m_cache.setCenter(0, 0);
                        m_cache.minimize();
                        // TODO: reset other member variables
                }

                template<typename ProviderType, typename ... Args>
                void useChunkProvider(Args&& ... args)
                {
                        static_assert(std::is_base_of<IChunkProvider, ProviderType>::value, 
                                "template parameter 'ProviderType' must be a derived type of IChunkProvider");
                        m_chunkProvider = LORD::make_shared<ProviderType>(std::forward<Args>(args) ...);
                }

                template<typename StorageType, typename ... Args>
                void useChunkStorage(Args&& ... args)
                {
                        static_assert(std::is_base_of<IChunkStorage, StorageType>::value, 
                                "template parameter 'StorageType' must be a derived type of IChunkStorage");
                        m_chunkStorage = LORD::make_shared<StorageType>(std::forward<Args>(args) ...);
                }

                template<typename ReadableStorageType, typename ... Args>
                void useReadableStorage(Args&& ... args)
                {
                        static_assert(std::is_base_of<IChunkProvider, ReadableStorageType>::value
                                && std::is_base_of<IChunkStorage, ReadableStorageType>::value,
                                "template parameter 'ReadableStorageType' must be a derived type of both IChunkProvider and IChunkStorage");
                        auto readableStorage = LORD::make_shared<ReadableStorageType>(std::forward<Args>(args) ...);
                        m_chunkProvider = readableStorage;
                        m_chunkStorage = readableStorage;
                }

                // Register ONE object that implements both IChunkProvider and
                // IChunkStorage (same contract as useReadableStorage, but for a
                // pre-built instance such as the server's disk-first /
                // generate-on-miss ChunkProviderServerFile). Using a single
                // object for both roles guarantees one Anvil region-file
                // manager owns all reads and writes.
                void useProviderStorage(const std::shared_ptr<IChunkProvider>& provider, const std::shared_ptr<IChunkStorage>& storage)
                {
                        m_chunkProvider = provider;
                        m_chunkStorage = storage;
                }

                [[deprecated("refactor with getChunkAync with callback instead")]]
                bool ayncGetExists(int x, int z) const
                {
                        return m_loadingTaskFutures.find({ x, z }) != m_loadingTaskFutures.end();
                }

                unsigned getAsyncCount() const
                {
                        return m_loadingTaskFutures.size();
                }

        protected:
                virtual void prepareChunk(const ChunkPtr& chunk);

                // Server-authoritative worldgen hook: called by getChunk() when
                // the local provider returns a NonexistentChunk (i.e., the chunk
                // is not on disk). The base-class default is a no-op. The client
                // overrides this to send C2SRequestChunk to the server; the
                // server replies async with S2CPacketChunkData, which the client
                // injects via injectChunk().
                virtual void onChunkMiss(int /*x*/, int /*z*/) {}

        private:
                bool m_supportAsync = true;
                WorkerThread m_workerThread{ "ChunkServiceWorkerThread" };
                map<std::pair<int, int>, Future<ChunkPtr>>::type m_loadingTaskFutures;
                ChunkCache m_cache;
                std::shared_ptr<IChunkProvider> m_chunkProvider;
                std::shared_ptr<IChunkStorage> m_chunkStorage;
                World* m_world;

        };
}

#endif // !__CHUNK_MANAGER_HEADER__
