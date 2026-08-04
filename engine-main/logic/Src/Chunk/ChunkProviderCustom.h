#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"
#include "Util/Random.h"

namespace BLOCKMAN
{
        class World;
        class NoiseGeneratorOctaves;
        class BiomeGenBase;

        // =============================================================================
        // ChunkProviderCustom — the "Sky Islands" world type.
        //
        // Generates a distinctive floating-islands world: terrain is a 3D noise
        // field thresholded so that stone forms floating blobs separated by air,
        // with grass on top and a layer of dirt underneath. The bottom of each
        // island is coated in stone. No oceans, no bedrock floor — pure sky.
        //
        // This is the default world generator for the LOCAL_MODE in-process
        // server. The server uses it when RoomGameConfig::worldType ==
        // TERRAIN_TYPE_CUSTOM (value 100). The seed comes from the World
        // (set in ServerWorld::createWorld via WorldSettings).
        //
        // Design goals:
        //   - Visually distinct from the default overworld so the user can
        //     immediately tell "this is the new custom world".
        //   - Self-contained — no dependency on BiomeGen/MapGen structures
        //     (so it works even if the biome pipeline is not yet wired).
        //   - Cheap to compute — runs on the phone in the in-process server
        //     thread without blocking the client render thread.
        // =============================================================================
        class ChunkProviderCustom : public IChunkProvider, public ObjectAlloc
        {
        public:
                ChunkProviderCustom(World* pWorld, i64 seed);
                virtual ~ChunkProviderCustom();

                virtual ChunkPtr provideChunk(int x, int z) override;

        protected:
                World* m_pWorld = nullptr;

        private:
                Random m_rand;

                // Three independent 3D noise octaves — combined to produce the
                // density field that decides "stone vs air" at each block.
                NoiseGeneratorOctaves* m_pNoise_density = nullptr;
                NoiseGeneratorOctaves* m_pNoise_height   = nullptr;  // shifts island Y up/down
                NoiseGeneratorOctaves* m_pNoise_scale    = nullptr;  // varies island size

                double* m_pNoiseArr = nullptr;   // 5*17*5 working buffer (same shape as vanilla)

                void initialize();
                virtual ChunkPtr CreateChunk(int x, int z);

                // Generate the 32768-byte (16*128*16) terrain data array.
                // Block IDs only — meta is left zero. Biome array is filled
                // with a single hardcoded biome ID (plains = 1) so the client
                // renderer does not crash on biome lookup.
                void generateTerrain(int x, int z, i8* arr);

                // Sample the 3D density field at chunk-relative coords.
                // Returns true → stone, false → air.
                double sampleDensity(int chunkX, int chunkZ, int blockX, int blockY, int blockZ);
        };
}
