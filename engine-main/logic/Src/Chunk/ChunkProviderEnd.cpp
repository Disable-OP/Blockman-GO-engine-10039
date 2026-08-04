#include "ChunkProviderEnd.h"
#include "WorldGenerator/NoiseGeneratorOctaves.h"
#include "BM_TypeDef.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "WorldGenerator/BiomeGen.h"
#include "World/WorldChunkManager.h"

namespace BLOCKMAN
{

	void ChunkProviderEnd::initialize()
	{
		m_pWorld = NULL;

		m_pNoiseGen1 = NULL;
		m_pNoiseGen2 = NULL;
		m_pNoiseGen3 = NULL;
		m_pNoiseGen4 = NULL;
		m_pNoiseGen5 = NULL;

		m_pNoiseData1 = NULL;
		m_pNoiseData2 = NULL;
		m_pNoiseData3 = NULL;
		m_pNoiseData4 = NULL;
		m_pNoiseData5 = NULL;
		m_pNoiseDensities = NULL;

		m_pBiomeGens = NULL;
		m_biomeGensLen = 0;
	}

	ChunkProviderEnd::ChunkProviderEnd(World* pWorld, i64 seed)
		: m_rand(seed)
	{
		initialize();

		m_pWorld = pWorld;
		m_pNoiseGen1 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoiseGen2 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoiseGen3 = LordNew NoiseGeneratorOctaves(&m_rand, 8);
		m_pNoiseGen4 = LordNew NoiseGeneratorOctaves(&m_rand, 10);
		m_pNoiseGen5 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pBiomeGens = NULL;
		m_biomeGensLen = 0;
	}

	ChunkProviderEnd::~ChunkProviderEnd()
	{
		LordSafeDelete(m_pNoiseGen1);
		LordSafeDelete(m_pNoiseGen2);
		LordSafeDelete(m_pNoiseGen3);
		LordSafeDelete(m_pNoiseGen4);
		LordSafeDelete(m_pNoiseGen5);
		LordSafeFree(m_pBiomeGens);
	}

	void ChunkProviderEnd::generateTerrain(int x, int z, i8* arr, BiomeGenBase** biomes)
	{
		int i = 2;
		int j = i + 1;
		int k = 33;
		int l = i + 1;
		m_pNoiseDensities = initializeNoiseField(m_pNoiseDensities, x * i, 0, z * i, j, k, l);

		for (int i1 = 0; i1 < i; ++i1)
		{
			for (int j1 = 0; j1 < i; ++j1)
			{
				for (int k1 = 0; k1 < 32; ++k1)
				{
					double d0 = 0.25;
					double d1 = m_pNoiseDensities[((i1 + 0) * l + j1 + 0) * k + k1 + 0];
					double d2 = m_pNoiseDensities[((i1 + 0) * l + j1 + 1) * k + k1 + 0];
					double d3 = m_pNoiseDensities[((i1 + 1) * l + j1 + 0) * k + k1 + 0];
					double d4 = m_pNoiseDensities[((i1 + 1) * l + j1 + 1) * k + k1 + 0];
					double d5 = (m_pNoiseDensities[((i1 + 0) * l + j1 + 0) * k + k1 + 1] - d1) * d0;
					double d6 = (m_pNoiseDensities[((i1 + 0) * l + j1 + 1) * k + k1 + 1] - d2) * d0;
					double d7 = (m_pNoiseDensities[((i1 + 1) * l + j1 + 0) * k + k1 + 1] - d3) * d0;
					double d8 = (m_pNoiseDensities[((i1 + 1) * l + j1 + 1) * k + k1 + 1] - d4) * d0;

					for (int l1 = 0; l1 < 4; ++l1)
					{
						double d9 = 0.125;
						double d10 = d1;
						double d11 = d2;
						double d12 = (d3 - d1) * d9;
						double d13 = (d4 - d2) * d9;

						for (int i2 = 0; i2 < 8; ++i2)
						{
							int i3 = (i2 + i1 * 8) << 11 | (0 + j1 * 8) << 7 | k1 * 4 + l1;
							short var43 = 128;
							double d14 = 0.125;
							double d15 = d10;
							double d16 = (d11 - d10) * d14;

							for (int j2 = 0; j2 < 8; ++j2)
							{
								int blockID = 0;

								if (d15 > 0.0)
								{
									blockID = BLOCK_ID_WHITE_STONE;
								}

								arr[i3] = (i8)blockID;
								i3 += var43;
								d15 += d16;
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

	void ChunkProviderEnd::replaceBlocksForBiome(int x, int z, i8* arr, BiomeGenBase** biomes)
	{
		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
				i8 k = 1;
				int l = -1;
				i8 blockID = (i8)BLOCK_ID_WHITE_STONE;
				i8 blockID2 = (i8)BLOCK_ID_WHITE_STONE;

				for (int k = 127; k >= 0; --k)
				{
					int k1 = (j * 16 + i) * 128 + k;
					i8 blockID3 = arr[k1];

					if (blockID3 == 0)
					{
						l = -1;
					}
					else if (blockID3 == BLOCK_ID_STONE)
					{
						if (l == -1)
						{
							if (k <= 0)
							{
								blockID = 0;
								blockID2 = (i8)BLOCK_ID_WHITE_STONE;
							}

							l = k;

							if (k >= 0)
							{
								arr[k1] = blockID;
							}
							else
							{
								arr[k1] = blockID2;
							}
						}
						else if (l > 0)
						{
							--l;
							arr[k1] = blockID2;
						}
					}
				}
			}
		}
	}

	ChunkPtr ChunkProviderEnd::CreateChunk(int x, int z)
	{
		return LORD::make_shared<Chunk>(m_pWorld, x, z);
	}

	ChunkPtr ChunkProviderEnd::provideChunk(int x, int z)
	{
		m_rand.setSeed((i64)x * 0x4F9939F508LL + (i64)z * 0x1EF1565BD5LL);
		i8* chunkData = (i8*)LordMalloc(sizeof(i8) * 32768);
		memset(chunkData, 0, sizeof(i8) * 32768);
		m_pBiomeGens = m_pWorld->getWorldChunkManager()->loadBlockGeneratorData(m_pBiomeGens, m_biomeGensLen, x * 16, z * 16, 16, 16);
		m_biomeGensLen = 16 * 16;

		generateTerrain(x, z, chunkData, m_pBiomeGens);
		replaceBlocksForBiome(x, z, chunkData, m_pBiomeGens);
		ChunkPtr pChunk = CreateChunk(x, z);
		pChunk->setData(chunkData, 32768);

		i8* pBiomeArray = pChunk->getBiomeArray();

		for (int i = 0; i < 256; ++i) // modify by zhouyou
		{
			pBiomeArray[i] = (i8)m_pBiomeGens[i]->m_ID;
		}

		pChunk->generateSkylightMap();
		LordSafeFree(chunkData);
		return pChunk;
	}

	double* ChunkProviderEnd::initializeNoiseField(double* arr, int xOffset, int yOffset, int zOffset, int xSize, int ySize, int zSize)
	{
		if (arr == NULL)
		{
			arr = (double*)LordMalloc(sizeof(double) * xSize * ySize * zSize);
			memset(arr, 0, sizeof(double)*xSize*ySize*zSize);
		}

		double d0 = 684.412;
		double d1 = 684.412;
		m_pNoiseData4 = m_pNoiseGen4->generateNoiseOctaves(m_pNoiseData4, xOffset, zOffset, xSize, zSize, 1.121, 1.121);
		m_pNoiseData5 = m_pNoiseGen5->generateNoiseOctaves(m_pNoiseData5, xOffset, zOffset, xSize, zSize, 200.0, 200.0);
		d0 *= 2.0;
		m_pNoiseData1 = m_pNoiseGen3->generateNoiseOctaves(m_pNoiseData1, xOffset, yOffset, zOffset, xSize, ySize, zSize, d0 / 80.0, d1 / 160.0, d0 / 80.0);
		m_pNoiseData2 = m_pNoiseGen1->generateNoiseOctaves(m_pNoiseData2, xOffset, yOffset, zOffset, xSize, ySize, zSize, d0, d1, d0);
		m_pNoiseData3 = m_pNoiseGen2->generateNoiseOctaves(m_pNoiseData3, xOffset, yOffset, zOffset, xSize, ySize, zSize, d0, d1, d0);
		int l = 0;
		int m = 0;

		for (int i = 0; i < xSize; ++i)
		{
			for (int j = 0; j < zSize; ++j)
			{
				double d2 = (m_pNoiseData4[m] + 256.0) / 512.0;
				if (d2 > 1.0)
					d2 = 1.0;

				double d3 = m_pNoiseData5[m] / 8000.0;
				if (d3 < 0.0)
					d3 = -d3 * 0.3;

				d3 = d3 * 3.0 - 2.0;
				float f0 = (float)(i + xOffset - 0) / 1.0F;
				float f1 = (float)(j + zOffset - 0) / 1.0F;
				float f2 = 100.0F - Math::Sqrt(f0 * f0 + f1 * f1) * 8.0F;

				if (f2 > 80.0F)
					f2 = 80.0F;

				if (f2 < -100.0F)
					f2 = -100.0F;

				if (d3 > 1.0)
					d3 = 1.0;

				d3 /= 8.0;
				d3 = 0.0;

				if (d2 < 0.0)
					d2 = 0.0;

				d2 += 0.5;
				d3 = d3 * (double)ySize / 16.0;
				++m;
				double d4 = (double)ySize / 2.0;

				for (int k = 0; k < ySize; ++k)
				{
					double d5 = 0.0;
					double d6 = ((double)k - d4) * 8.0 / d2;

					if (d6 < 0.0)
						d6 *= -1.0;

					double d7 = m_pNoiseData2[l] / 512.0;
					double d8 = m_pNoiseData3[l] / 512.0;
					double d9 = (m_pNoiseData1[l] / 10.0 + 1.0) / 2.0;

					if (d9 < 0.0)
						d5 = d7;
					else if (d9 > 1.0)
						d5 = d8;
					else
						d5 = d7 + (d8 - d7) * d9;

					d5 -= 8.0;
					d5 += (double)f2;
					i8 n = 2;
					double d10;

					if (k > ySize / 2 - n)
					{
						d10 = (double)((float)(k - (ySize / 2 - n)) / 64.0F);

						if (d10 < 0.0)
							d10 = 0.0;

						if (d10 > 1.0)
							d10 = 1.0;

						d5 = d5 * (1.0 - d10) + -3000.0 * d10;
					}

					n = 8;

					if (k < n)
					{
						d10 = (double)((float)(n - k) / ((float)n - 1.0F));
						d5 = d5 * (1.0 - d10) + -30.0 * d10;
					}

					arr[l] = d5;
					++l;
				}
			}
		}

		return arr;
	}
}