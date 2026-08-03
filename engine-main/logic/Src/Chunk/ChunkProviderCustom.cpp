#include "ChunkProviderCustom.h"
#include "WorldGenerator/NoiseGeneratorOctaves.h"
#include "BM_TypeDef.h"
#include "World/World.h"
#include "Chunk/Chunk.h"

namespace BLOCKMAN
{
        // Custom terrain-type enum value used by RoomGameConfig / WorldSettings
        // to request the sky-islands generator. Lives in the "extended" range
        // (>TERRAIN_TYPE_COUNT) so it does not collide with vanilla values.
        static const int TERRAIN_TYPE_CUSTOM = 100;

        void ChunkProviderCustom::initialize()
        {
                m_pWorld = NULL;
                m_pNoise_density = NULL;
                m_pNoise_height   = NULL;
                m_pNoise_scale    = NULL;
                m_pNoiseArr       = NULL;
        }

        ChunkProviderCustom::ChunkProviderCustom(World* pWorld, i64 seed)
                : m_rand(seed)
        {
                initialize();
                m_pWorld = pWorld;
                // Octave counts chosen so the dominant features are ~32 blocks wide;
                // smaller octaves add high-frequency detail. Cheap (4 Perlin instances).
                m_pNoise_density = LordNew NoiseGeneratorOctaves(&m_rand, 4);
                m_pNoise_height   = LordNew NoiseGeneratorOctaves(&m_rand, 2);
                m_pNoise_scale    = LordNew NoiseGeneratorOctaves(&m_rand, 2);
        }

        ChunkProviderCustom::~ChunkProviderCustom()
        {
                LordSafeDelete(m_pNoise_density);
                LordSafeDelete(m_pNoise_height);
                LordSafeDelete(m_pNoise_scale);
                LordSafeFree(m_pNoiseArr);
        }

        ChunkPtr ChunkProviderCustom::CreateChunk(int x, int z)
        {
                return LORD::make_shared<Chunk>(m_pWorld, x, z);
        }

        void ChunkProviderCustom::generateTerrain(int x, int z, i8* arr)
        {
                // ----- 1. Generate the 3D density field (16 * 128 * 16 doubles) -----
                // xSize=16, ySize=128, zSize=16. The noise array layout (matching
                // NoiseGeneratorPerlin::populateNoiseArray) is:
                //     index = (bx * zSize + bz) * ySize + by
                // so it lines up directly with the (z*16+x)*128+y layout used by
                // the vanilla setData() path.
                const int xSize = 16, ySize = 128, zSize = 16;

                // Scale = 1/featureSize. 1/24 gives features ~24 blocks wide.
                // Y scale 1/48 means features are taller than they are wide
                // (visually distinct "islands" rather than "caves").
                m_pNoiseArr = m_pNoise_density->generateNoiseOctaves(
                        m_pNoiseArr,
                        x * 16, 0, z * 16,
                        xSize, ySize, zSize,
                        1.0 / 24.0, 1.0 / 48.0, 1.0 / 24.0);

                // Per-column offset (smoothly varies island centre height across the world)
                double* heightNoise = NULL;
                heightNoise = m_pNoise_height->generateNoiseOctaves(
                        heightNoise,
                        x * 16, z * 16, 0,
                        xSize, zSize, 1,
                        1.0 / 64.0, 1.0 / 64.0, 1.0);

                // ----- 2. Threshold the density into stone/air, with a height falloff -----
                // The falloff is a gaussian-like envelope centred on y=64 so islands
                // cluster around mid-height. density > 0 → stone, else air.
                for (int bx = 0; bx < 16; ++bx)
                {
                        for (int bz = 0; bz < 16; ++bz)
                        {
                                // Island centre height for this column (in [48, 80])
                                double colHeightOffset = heightNoise[(bx * zSize + bz)] * 16.0;
                                double islandCentreY = 64.0 + colHeightOffset;

                                int topStoneY = -1;
                                for (int by = 127; by >= 0; --by)
                                {
                                        int noiseIdx = (bx * zSize + bz) * ySize + by;
                                        int arrIdx   = (bz * 16 + bx) * 128 + by;

                                        double density = m_pNoiseArr[noiseIdx];

                                        // Distance from the column's island centre, normalised.
                                        double dy = ((double)by - islandCentreY) / 24.0;
                                        double envelope = -dy * dy + 1.0;     // 1.0 at centre, 0 at ±24

                                        // Threshold: tuned so roughly 35-45% of blocks within
                                        // the envelope are stone (islands with overhangs).
                                        double threshold = 0.0;
                                        if (density + envelope * 0.6 > threshold)
                                        {
                                                arr[arrIdx] = (i8)BLOCK_ID_STONE;
                                                if (topStoneY < 0) topStoneY = by;
                                        }
                                        else
                                        {
                                                arr[arrIdx] = 0;
                                        }
                                }

                                // ----- 3. Surface dressing: grass on top, dirt below -----
                                if (topStoneY >= 0)
                                {
                                        int idx = (bz * 16 + bx) * 128 + topStoneY;
                                        arr[idx] = (i8)BLOCK_ID_GRASS;
                                        for (int dy = 1; dy <= 3 && topStoneY - dy >= 0; ++dy)
                                        {
                                                arr[idx - dy] = (i8)BLOCK_ID_DIRT;
                                        }
                                }
                        }
                }

                LordSafeFree(heightNoise);
        }

        ChunkPtr ChunkProviderCustom::provideChunk(int x, int z)
        {
                // Per-chunk seed: deterministic, independent of provider call order.
                m_rand.setSeed((i64)x * 0x4F9939F508LL + (i64)z * 0x1EF1565BD5LL);

                i8* pTerrainData = (i8*)LordMalloc(sizeof(i8) * 32768);
                memset(pTerrainData, 0, sizeof(i8) * 32768);
                generateTerrain(x, z, pTerrainData);

                ChunkPtr pChunk = CreateChunk(x, z);
                LordAssert(pChunk);
                pChunk->setData(pTerrainData, 32768);

                // Biome array — fill with plains (biome ID 1) so the client's
                // biome lookup does not crash on the new world type. A proper
                // biome pipeline can be wired in later without changing this
                // surface generator.
                i8* biomeArray = pChunk->getBiomeArray();
                for (int i = 0; i < 256; ++i)
                {
                        biomeArray[i] = (i8)1;  // plains
                }

                pChunk->generateSkylightMap();

                LordSafeFree(pTerrainData);
                return pChunk;
        }
}
