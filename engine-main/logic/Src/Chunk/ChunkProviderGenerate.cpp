#include "ChunkProviderGenerate.h"
#include "WorldGenerator/NoiseGeneratorOctaves.h"
#include "BM_TypeDef.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "WorldGenerator/BiomeGen.h"
#include "World/WorldChunkManager.h"
#include "WorldGenerator/MapGenerate.h"
#include "WorldGenerator/WorldGenerator.h"

namespace BLOCKMAN
{
	void ChunkProviderGenerate::initialize()
	{
		m_pWorld = NULL;

		m_pNoise_minLimit = NULL;
		m_pNoise_maxLimit = NULL;
		m_pNoise_main = NULL;
		m_pNoise_surface = NULL;
		m_pNoise_scale = NULL;
		m_pNoise_depth = NULL;
		m_pNoise_forest = NULL;

		m_bMapFeaturesEnabled = false;
		m_pMapGen_cave = LordNew MapGenCaves();
		m_pMapGen_mineshaft = LordNew MapGenMineshaft();
		m_pMapGen_ravine = LordNew MapGenRavine();

		m_pNoiseArr = NULL;
		m_pStoneNoise = NULL;
		m_pBiomes = NULL;
		m_biomesLen = 0;
		m_mainRegion = NULL;
		m_minLimitRegion = NULL;
		m_maxLimitRegion = NULL;
		m_scaleRegion = NULL;
		m_depthRegion = NULL;
		m_parabolicField = NULL;
	}

	ChunkProviderGenerate::ChunkProviderGenerate(World* pWorld, i64 seed, bool features)
		: m_rand(seed)
	{
		initialize();
		m_pWorld = pWorld;
		m_bMapFeaturesEnabled = features;
		m_pNoise_minLimit = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoise_maxLimit = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoise_main = LordNew NoiseGeneratorOctaves(&m_rand, 8);
		m_pNoise_surface = LordNew NoiseGeneratorOctaves(&m_rand, 4);
		m_pNoise_scale = LordNew NoiseGeneratorOctaves(&m_rand, 10);
		m_pNoise_depth = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoise_forest = LordNew NoiseGeneratorOctaves(&m_rand, 8);

		m_pGen_waterstill = LordNew WorldGenLakes(BLOCK_ID_WATERSTILL);
		m_pGen_lavastill = LordNew WorldGenLakes(BLOCK_ID_LAVASTILL);
		m_pGen_dungeons = LordNew WorldGenDungeons();
	}

	ChunkProviderGenerate::~ChunkProviderGenerate()
	{
		LordSafeDelete(m_pGen_lavastill);
		LordSafeDelete(m_pGen_waterstill);
		LordSafeDelete(m_pGen_dungeons);

		LordSafeDelete(m_pNoise_minLimit);
		LordSafeDelete(m_pNoise_maxLimit);
		LordSafeDelete(m_pNoise_main);
		LordSafeDelete(m_pNoise_surface);
		LordSafeDelete(m_pNoise_scale);
		LordSafeDelete(m_pNoise_depth);
		LordSafeDelete(m_pNoise_forest);
		LordSafeDelete(m_pMapGen_cave);
		LordSafeDelete(m_pMapGen_mineshaft);
		LordSafeDelete(m_pMapGen_ravine);

		LordSafeFree(m_pNoiseArr);
		LordSafeFree(m_pStoneNoise);
		LordSafeFree(m_pBiomes);
		LordSafeFree(m_minLimitRegion);
		LordSafeFree(m_maxLimitRegion);
		LordSafeFree(m_mainRegion);
		LordSafeFree(m_scaleRegion);
		LordSafeFree(m_depthRegion);
		LordSafeFree(m_parabolicField);
	}

	/*
	https://zhuanlan.zhihu.com/p/24125792
	*/
	void ChunkProviderGenerate::generateTerrain(int x, int z, i8* arr)
	{
		i8 offset = 4;
		i8 size = 16;
		i8 sealevel = 63;
		int sizex = offset + 1;
		int sizey = size + 1;
		int sizez = offset + 1;
		m_pBiomes = m_pWorld->getWorldChunkManager()->getBiomesForGeneration(m_pBiomes, m_biomesLen, x * 4 - 2, z * 4 - 2, sizex + 5, sizez + 5);
		m_biomesLen = (sizex + 5) * (sizez + 5);
		m_pNoiseArr = initializeNoiseField(m_pNoiseArr, x * offset, 0, z * offset, sizex, sizey, sizez);

		//4*16*4
		for (int i = 0; i < offset; ++i)
		{
			for (int j = 0; j < offset; ++j)
			{
				for (int k = 0; k < size; ++k)
				{
					//magnify 16*128*16
					double d0 = 0.125;
					double d1 = m_pNoiseArr[((i + 0) * sizez + j + 0) * sizey + k + 0];
					double d2 = m_pNoiseArr[((i + 0) * sizez + j + 1) * sizey + k + 0];
					double d3 = m_pNoiseArr[((i + 1) * sizez + j + 0) * sizey + k + 0];
					double d4 = m_pNoiseArr[((i + 1) * sizez + j + 1) * sizey + k + 0];
					double d5 = (m_pNoiseArr[((i + 0) * sizez + j + 0) * sizey + k + 1] - d1) * d0;
					double d6 = (m_pNoiseArr[((i + 0) * sizez + j + 1) * sizey + k + 1] - d2) * d0;
					double d7 = (m_pNoiseArr[((i + 1) * sizez + j + 0) * sizey + k + 1] - d3) * d0;
					double d8 = (m_pNoiseArr[((i + 1) * sizez + j + 1) * sizey + k + 1] - d4) * d0;

					for (int l = 0; l < 8; ++l)
					{
						double d9 = 0.25;
						double d10 = d1;
						double d11 = d2;
						double d12 = (d3 - d1) * d9;
						double d13 = (d4 - d2) * d9;

						for (int m = 0; m < 4; ++m)
						{
							int idx = (m + i * 4) << 11 | (0 + j * 4) << 7 | k * 8 + l;
							short yBatch = 128;
							idx -= yBatch;
							double d14 = 0.25;
							double d15 = (d11 - d10) * d14;
							double d16 = d10 - d15;

							for (int o = 0; o < 4; ++o)
							{
								if ((d16 += d15) > 0.0)
								{
									arr[idx += yBatch] = (i8)BLOCK_ID_STONE;
								}
								else if (k * 8 + l < sealevel)
								{
									arr[idx += yBatch] = (i8)BLOCK_ID_WATERSTILL;
								}
								else
								{
									arr[idx += yBatch] = 0;
								}
							}

							d10 += d12;
							d11 += d13;
						}

						d1 += d5;
						d2 += d6;
						d3 += d7;
						d4 += d8;
					}
				}
			}
		}
	}

	void ChunkProviderGenerate::replaceBlocksForBiome(int x, int z, i8* arr, BiomeGenBase** biomes)
	{
		i8 sealevel = 63;
		double scale = 0.03125;
		m_pStoneNoise = m_pNoise_surface->generateNoiseOctaves(m_pStoneNoise, x * 16, z * 16, 0, 16, 16, 1, scale * 2.0, scale * 2.0, scale * 2.0);

		//x,z
		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
				BiomeGenBase* pBiome = biomes[j + i * 16];
				float temperature = pBiome->getFloatTemperature();
				int t = (int)(m_pStoneNoise[i + j * 16] / 3.0 + 3.0 + m_rand.nextDouble() * 0.25);
				int num = -1;
				i8 topBlockID = pBiome->m_topBlock;
				i8 fillerBlockID = pBiome->m_fillerBlock;

				for (int height = 127; height >= 0; --height)
				{
					int index = (j * 16 + i) * 128 + height;

					if (height <= 0 + m_rand.nextInt(5))
					{
						arr[index] = BLOCK_ID_BEDROCK;
					}
					else
					{
						i8 blockID = arr[index];

						if (blockID == 0)
						{
							num = -1;
						}
						else if (blockID == BLOCK_ID_STONE)
						{
							if (num == -1)
							{
								if (t <= 0)
								{
									topBlockID = 0;
									fillerBlockID = (i8)BLOCK_ID_STONE;
								}
								else if (height >= sealevel - 4 && height <= sealevel + 1)
								{
									topBlockID = pBiome->m_topBlock;
									fillerBlockID = pBiome->m_fillerBlock;
								}

								if (height < sealevel && topBlockID == 0)
								{
									if (temperature < 0.15F)
									{
										topBlockID = (i8)BLOCK_ID_ICE;
									}
									else
									{
										topBlockID = (i8)BLOCK_ID_WATERSTILL;
									}
								}

								num = t;

								if (height >= sealevel - 1)
								{
									arr[index] = topBlockID;
								}
								else
								{
									arr[index] = fillerBlockID;
								}
							}
							else if (num > 0)
							{
								--num;
								arr[index] = fillerBlockID;

								if (num == 0 && fillerBlockID == BLOCK_ID_SAND)
								{

									num = m_rand.nextInt(4);
									fillerBlockID = (i8)BLOCK_ID_SAND_STONE;
								}
							}
						}
					}
				}
			}
		}
	}

	ChunkPtr ChunkProviderGenerate::CreateChunk(int x, int z)
	{
		return LORD::make_shared<Chunk>(m_pWorld, x, z);
	}

	ChunkPtr ChunkProviderGenerate::provideChunk(int x, int z)
	{
		m_rand.setSeed((i64)x * 0x4F9939F508LL + (i64)z * 0x1EF1565BD5LL);
		i8* pTerrainData = (i8*)LordMalloc(sizeof(i8) * 32768);
		memset(pTerrainData, 0, sizeof(i8) * 32768);
		generateTerrain(x, z, pTerrainData);
		m_pBiomes = m_pWorld->getWorldChunkManager()->
			loadBlockGeneratorData(m_pBiomes, m_biomesLen, x * 16, z * 16, 16, 16);
		m_biomesLen = 16 * 16;
		replaceBlocksForBiome(x, z, pTerrainData, m_pBiomes);
		m_pMapGen_cave->generate(m_pWorld, x, z, pTerrainData);
		m_pMapGen_ravine->generate(m_pWorld, x, z, pTerrainData);

		if (m_bMapFeaturesEnabled)
		{
			m_pMapGen_mineshaft->generate(m_pWorld, x, z, pTerrainData);
			// villageGenerator.generate(this, worldObj, x, z, var3);
			// strongholdGenerator.generate(this, worldObj, x, z, var3);
			// scatteredFeatureGenerator.generate(this, worldObj, x, z, var3);
		}

		ChunkPtr pChunk = CreateChunk(x, z);
		LordAssert(pChunk);
		pChunk->setData(pTerrainData, 32768);
		i8* biomeArray = pChunk->getBiomeArray();

		for (int i = 0; i < 256; ++i)
		{
			biomeArray[i] = (i8)m_pBiomes[i]->m_ID;
		}

		pChunk->generateSkylightMap();

		LordSafeFree(pTerrainData);
		return pChunk;
	}

	double* ChunkProviderGenerate::initializeNoiseField(double* arr, int offsetx, int offsety, int offsetz, int sizex, int sizey, int sizez)
	{
		if (arr == NULL)
		{
			arr = (double*)LordMalloc(sizeof(double) * sizex * sizey * sizez);
			memset(arr, 0, sizeof(double)*sizex*sizey*sizez);
		}

		if (m_parabolicField == NULL)
		{
			m_parabolicField = (float*)LordMalloc(sizeof(float) * 25);

			for (int var8 = -2; var8 <= 2; ++var8)
			{
				for (int var9 = -2; var9 <= 2; ++var9)
				{
					float var10 = 10.0F / Math::Sqrt((float)(var8 * var8 + var9 * var9) + 0.2F);
					m_parabolicField[var8 + 2 + (var9 + 2) * 5] = var10;
				}
			}
		}

		double scalexz = 684.412;
		double scaley = 684.412;
		m_scaleRegion = m_pNoise_scale->generateNoiseOctaves(m_scaleRegion, offsetx, offsetz, sizex, sizez, 1.121, 1.121);
		m_depthRegion = m_pNoise_depth->generateNoiseOctaves(m_depthRegion, offsetx, offsetz, sizex, sizez, 200.0, 200.0);
		m_mainRegion = m_pNoise_main->generateNoiseOctaves(m_mainRegion, offsetx, offsety, offsetz, sizex, sizey, sizez, scalexz / 80.0, scaley / 160.0, scalexz / 80.0);
		m_minLimitRegion = m_pNoise_minLimit->generateNoiseOctaves(m_minLimitRegion, offsetx, offsety, offsetz, sizex, sizey, sizez, scalexz, scaley, scalexz);
		m_maxLimitRegion = m_pNoise_maxLimit->generateNoiseOctaves(m_maxLimitRegion, offsetx, offsety, offsetz, sizex, sizey, sizez, scalexz, scaley, scalexz);

		int idx0 = 0;
		int idx1 = 0;

		for (int i = 0; i < sizex; ++i)
		{
			for (int j = 0; j < sizez; ++j)
			{
				float f0 = 0.0F;
				float f1 = 0.0F;
				float f2 = 0.0F;
				i8 k = 2;
				BiomeGenBase* var20 = m_pBiomes[i + 2 + (j + 2) * (sizex + 5)];

				for (int i1 = -k; i1 <= k; ++i1)
				{
					for (int j1 = -k; j1 <= k; ++j1)
					{
						BiomeGenBase* pBiomeGen = m_pBiomes[i + i1 + 2 + (j + j1 + 2) * (sizex + 5)];
						float parabolic = m_parabolicField[i1 + 2 + (j1 + 2) * 5] / (pBiomeGen->m_minHeight + 2.0F);

						if (pBiomeGen->m_minHeight > var20->m_minHeight)
							parabolic /= 2.0F;

						f0 += pBiomeGen->m_maxHeight * parabolic;
						f1 += pBiomeGen->m_minHeight * parabolic;
						f2 += parabolic;
					}
				}

				f0 /= f2;
				f1 /= f2;
				f0 = f0 * 0.9F + 0.1F;
				f1 = (f1 * 4.0F - 1.0F) / 8.0F;
				double depth = m_depthRegion[idx1] / 8000.0;

				if (depth < 0.0)
					depth = -depth * 0.3;

				depth = depth * 3.0 - 2.0;

				if (depth < 0.0)
				{
					depth /= 2.0;

					if (depth < -1.0)
						depth = -1.0;

					depth /= 1.4;
					depth /= 2.0;
				}
				else
				{
					if (depth > 1.0)
						depth = 1.0;

					depth /= 8.0;
				}

				++idx1;

				for (int i1 = 0; i1 < sizey; ++i1)
				{
					double var48 = (double)f1;
					double var26 = (double)f0;
					var48 += depth * 0.2;
					var48 = var48 * (double)sizey / 16.0;
					double var28 = (double)sizey / 2.0 + var48 * 4.0;
					double var30 = 0.0;
					double var32 = ((double)i1 - var28) * 12.0 * 128.0 / 128.0 / var26;

					if (var32 < 0.0)
						var32 *= 4.0;

					double var34 = m_minLimitRegion[idx0] / 512.0;
					double var36 = m_maxLimitRegion[idx0] / 512.0;
					double var38 = (m_mainRegion[idx0] / 10.0 + 1.0) / 2.0;

					if (var38 < 0.0)
						var30 = var34;
					else if (var38 > 1.0)
						var30 = var36;
					else
						var30 = var34 + (var36 - var34) * var38;

					var30 -= var32;

					if (i1 > sizey - 4)
					{
						double var40 = (double)((float)(i1 - (sizey - 4)) / 3.0F);
						var30 = var30 * (1.0 - var40) + -10.0 * var40;
					}

					arr[idx0] = var30;
					++idx0;
				}
			}
		}

		return arr;
	}
}