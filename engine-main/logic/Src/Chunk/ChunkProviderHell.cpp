#include "ChunkProviderHell.h"
#include "WorldGenerator/NoiseGeneratorOctaves.h"
#include "BM_TypeDef.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "WorldGenerator/BiomeGen.h"
#include "World/WorldChunkManager.h"
#include "WorldGenerator/MapGenerate.h"

namespace BLOCKMAN
{
	void ChunkProviderHell::initialize()
	{
		m_pNoiseGen1 = NULL;
		m_pNoiseGen2 = NULL;
		m_pNoiseGen3 = NULL;
		m_pNoiseGen4 = NULL;
		m_pNoiseGen5 = NULL;

		m_pWorld = NULL;
		m_pNoiseDensities = NULL;
		m_pGen_bridge = LordNew MapGenNetherBridge();
		m_pGen_netherCave = LordNew MapGenCavesHell();
		noiseData1 = NULL;
		noiseData2 = NULL;
		noiseData3 = NULL;
		noiseData4 = NULL;
		noiseData5 = NULL;
		m_pNoiseGen6 = NULL;
		m_pNoiseGen7 = NULL;
		m_pNoiseSlowSand = (double*)LordMalloc(sizeof(double) * 256);
		memset(m_pNoiseSlowSand, 0, sizeof(double) * 256);
		m_pNoiseGravel = (double*)LordMalloc(sizeof(double) * 256);
		memset(m_pNoiseGravel, 0, sizeof(double) * 256);
		m_pNoiseNetherrack = (double*)LordMalloc(sizeof(double) * 256);
		memset(m_pNoiseNetherrack, 0, sizeof(double) * 256);
	}

	ChunkProviderHell::ChunkProviderHell(World* pWorld, i64 seed)
		: m_rand(seed)
	{
		m_pWorld = pWorld;

		m_pNoiseGen1 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoiseGen2 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
		m_pNoiseGen3 = LordNew NoiseGeneratorOctaves(&m_rand, 8);
		m_pNoiseGen4 = LordNew NoiseGeneratorOctaves(&m_rand, 4);
		m_pNoiseGen5 = LordNew NoiseGeneratorOctaves(&m_rand, 4);
		m_pNoiseGen6 = LordNew NoiseGeneratorOctaves(&m_rand, 10);
		m_pNoiseGen7 = LordNew NoiseGeneratorOctaves(&m_rand, 16);
	}

	ChunkProviderHell::~ChunkProviderHell()
	{
		LordSafeDelete(m_pNoiseGen1);
		LordSafeDelete(m_pNoiseGen2);
		LordSafeDelete(m_pNoiseGen3);
		LordSafeDelete(m_pNoiseGen4);
		LordSafeDelete(m_pNoiseGen5);
		LordSafeDelete(m_pNoiseGen6);
		LordSafeDelete(m_pNoiseGen7);
		LordSafeDelete(m_pNoiseGen1);
		LordSafeDelete(m_pGen_bridge);
		LordSafeDelete(m_pGen_netherCave);

		LordSafeFree(m_pNoiseDensities);
		LordSafeFree(noiseData1);
		LordSafeFree(noiseData2);
		LordSafeFree(noiseData3);
		LordSafeFree(noiseData4);
		LordSafeFree(noiseData5);
		LordSafeFree(m_pNoiseSlowSand);
		LordSafeFree(m_pNoiseGravel);
		LordSafeFree(m_pNoiseNetherrack);
	}

	void ChunkProviderHell::generateNetherTerrain(int x, int z, i8* arr)
	{
		i8 offset = 4;
		i8 sealevel = 32;
		int sizex = offset + 1;
		i8 sizey = 17;
		int sizez = offset + 1;
		m_pNoiseDensities = initializeNoiseField(m_pNoiseDensities, x * offset, 0, z * offset, sizex, sizey, sizez);

		for (int i = 0; i < offset; ++i)
		{
			for (int j = 0; j < offset; ++j)
			{
				for (int k = 0; k < 16; ++k)
				{
					double d0 = 0.125;
					double d1 = m_pNoiseDensities[((i + 0) * sizez + j + 0) * sizey + k + 0];
					double d2 = m_pNoiseDensities[((i + 0) * sizez + j + 1) * sizey + k + 0];
					double d3 = m_pNoiseDensities[((i + 1) * sizez + j + 0) * sizey + k + 0];
					double d4 = m_pNoiseDensities[((i + 1) * sizez + j + 1) * sizey + k + 0];
					double d5 = (m_pNoiseDensities[((i + 0) * sizez + j + 0) * sizey + k + 1] - d1) * d0;
					double d6 = (m_pNoiseDensities[((i + 0) * sizez + j + 1) * sizey + k + 1] - d2) * d0;
					double d7 = (m_pNoiseDensities[((i + 1) * sizez + j + 0) * sizey + k + 1] - d3) * d0;
					double d8 = (m_pNoiseDensities[((i + 1) * sizez + j + 1) * sizey + k + 1] - d4) * d0;

					for (int i1 = 0; i1 < 8; ++i1)
					{
						double d10 = 0.25;
						double d11 = d1;
						double d12 = d2;
						double d13 = (d3 - d1) * d10;
						double d14 = (d4 - d2) * d10;

						for (int j1 = 0; j1 < 4; ++j1)
						{
							int idx = (j1 + i * 4) << 11 | (0 + j * 4) << 7 | k * 8 + i1;
							short span = 128;
							double d15 = 0.25;
							double d16 = d11;
							double d17 = (d12 - d11) * d15;

							for (int k1 = 0; k1 < 4; ++k1)
							{
								int var51 = 0;

								if (k * 8 + i1 < sealevel)
								{
									var51 = BLOCK_ID_LAVASTILL;
								}

								if (d16 > 0.0)
								{
									var51 = BLOCK_ID_NETHERRACK;
								}

								arr[idx] = (i8)var51;
								idx += span;
								d16 += d17;
							}

							d11 += d13;
							d12 += d14;
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

	void ChunkProviderHell::replaceBlocksForBiome(int x, int z, i8* arr)
	{
		i8 sealevel = 64;
		double scale = 0.03125;
		m_pNoiseSlowSand = m_pNoiseGen4->generateNoiseOctaves(m_pNoiseSlowSand, x * 16, z * 16, 0, 16, 16, 1, scale, scale, 1.0);
		m_pNoiseGravel = m_pNoiseGen4->generateNoiseOctaves(m_pNoiseGravel, x * 16, 109, z * 16, 16, 1, 16, scale, 1.0, scale);
		m_pNoiseNetherrack = m_pNoiseGen5->generateNoiseOctaves(m_pNoiseNetherrack, x * 16, z * 16, 0, 16, 16, 1, scale * 2.0, scale * 2.0, scale * 2.0);

		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
				bool flag = m_pNoiseSlowSand[i + j * 16] + m_rand.nextDouble() * 0.2 > 0.0;
				bool flag1 = m_pNoiseGravel[i + j * 16] + m_rand.nextDouble() * 0.2 > 0.0;
				int i1 = (int)(m_pNoiseNetherrack[i + j * 16] / 3.0 + 3.0 + m_rand.nextDouble() * 0.25);
				int i2 = -1;
				i8 bID0 = (i8)BLOCK_ID_NETHERRACK;
				i8 bID1 = (i8)BLOCK_ID_NETHERRACK;

				for (int k = 127; k >= 0; --k)
				{
					int i3 = (j * 16 + i) * 128 + k;

					if (k < 127 - m_rand.nextInt(5) && k > 0 + m_rand.nextInt(5))
					{
						i8 i4 = arr[i3];

						if (i4 == 0)
						{
							i2 = -1;
						}
						else if (i4 == BLOCK_ID_NETHERRACK)
						{
							if (i2 == -1)
							{
								if (i1 <= 0)
								{
									bID0 = 0;
									bID1 = (i8)BLOCK_ID_NETHERRACK;
								}
								else if (k >= sealevel - 4 && k <= sealevel + 1)
								{
									bID0 = (i8)BLOCK_ID_NETHERRACK;
									bID1 = (i8)BLOCK_ID_NETHERRACK;

									if (flag1)
										bID0 = (i8)BLOCK_ID_GRAVEL;

									if (flag1)
										bID1 = (i8)BLOCK_ID_NETHERRACK;

									if (flag)
										bID0 = (i8)BLOCK_ID_SLOW_SAND;

									if (flag)
										bID1 = (i8)BLOCK_ID_SLOW_SAND;
								}

								if (k < sealevel && bID0 == 0)
									bID0 = (i8)BLOCK_ID_LAVASTILL;

								i2 = i1;

								if (k >= sealevel - 1)
									arr[i3] = bID0;
								else
									arr[i3] = bID1;
							}
							else if (i2 > 0)
							{
								--i2;
								arr[i3] = bID1;
							}
						}
					}
					else
					{
						arr[i3] = (i8)BLOCK_ID_BEDROCK;
					}
				}
			}
		}
	}

	ChunkPtr ChunkProviderHell::CreateChunk(int x, int z)
	{
		return LORD::make_shared<Chunk>(m_pWorld, x, z);
	}

	ChunkPtr ChunkProviderHell::provideChunk(int x, int z)
	{
		m_rand.setSeed((i64)x * 341873128712LL + (i64)z * 132897987541LL);
		i8* data = (i8*)LordMalloc(sizeof(i8) * 32768);
		memset(data, 0, sizeof(i8) * 32768);
		generateNetherTerrain(x, z, data);
		replaceBlocksForBiome(x, z, data);
		m_pGen_netherCave->generate(m_pWorld, x, z, data);
		m_pGen_bridge->generate(m_pWorld, x, z, data);
		ChunkPtr pchunk = CreateChunk(x, z);
		pchunk->setData(data, 32768);
		BiomeGenBase** var5 = m_pWorld->getWorldChunkManager()->loadBlockGeneratorData(NULL, 0, x * 16, z * 16, 16, 16);
		i8* biomes = pchunk->getBiomeArray();

		for (int var7 = 0; var7 < 256; ++var7)
		{
			biomes[var7] = (i8)var5[var7]->m_ID;
		}

		pchunk->resetRelightChecks();
		LordSafeFree(data);
		return pchunk;
	}

	double* ChunkProviderHell::initializeNoiseField(double* arr, int offsetx, int offsety, int offsetz, int sizex, int sizey, int sizez)
	{
		if (arr == NULL)
		{
			arr = (double*)LordMalloc(sizeof(double)*sizex * sizey * sizez);
			memset(arr, 0, sizeof(double) * sizex*sizey*sizez);
		}

		double scalex = 684.412;
		double scalez = 684.412;
		double scaley = 2053.236;
		noiseData4 = m_pNoiseGen6->generateNoiseOctaves(noiseData4, offsetx, offsety, offsetz, sizex, 1, sizez, 1.0, 0.0, 1.0);
		noiseData5 = m_pNoiseGen7->generateNoiseOctaves(noiseData5, offsetx, offsety, offsetz, sizex, 1, sizez, 100.0, 0.0, 100.0);
		noiseData1 = m_pNoiseGen3->generateNoiseOctaves(noiseData1, offsetx, offsety, offsetz, sizex, sizey, sizez, scalex / 80.0, scaley / 60.0, scalez / 80.0);
		noiseData2 = m_pNoiseGen1->generateNoiseOctaves(noiseData2, offsetx, offsety, offsetz, sizex, sizey, sizez, scalex, scaley, scalez);
		noiseData3 = m_pNoiseGen2->generateNoiseOctaves(noiseData3, offsetx, offsety, offsetz, sizex, sizey, sizez, scalex, scaley, scalez);
		int idx0 = 0;
		int idx1 = 0;
		double* dArray = (double*)LordMalloc(sizeof(double) * sizey);
		memset(dArray, 0, sizeof(double)*sizey);
		int i;

		for (i = 0; i < sizey; ++i)
		{
			dArray[i] = Math::Cos((double)i * Math::PI * 6.0 / (double)sizey) * 2.0;
			double d0 = (double)i;

			if (i > sizey / 2)
			{
				d0 = (double)(sizey - 1 - i);
			}

			if (d0 < 4.0)
			{
				d0 = 4.0 - d0;
				dArray[i] -= d0 * d0 * d0 * 10.0;
			}
		}

		for (i = 0; i < sizex; ++i)
		{
			for (int j = 0; j < sizez; ++j)
			{
				double d1 = (noiseData4[idx1] + 256.0) / 512.0;

				if (d1 > 1.0)
					d1 = 1.0;

				double d2 = 0.0;
				double d3 = noiseData5[idx1] / 8000.0;

				if (d3 < 0.0)
					d3 = -d3;

				d3 = d3 * 3.0 - 3.0;

				if (d3 < 0.0)
				{
					d3 /= 2.0;

					if (d3 < -1.0)
						d3 = -1.0;

					d3 /= 1.4;
					d3 /= 2.0;
					d1 = 0.0;
				}
				else
				{
					if (d3 > 1.0)
						d3 = 1.0;

					d3 /= 6.0;
				}

				d1 += 0.5;
				d3 = d3 * (double)sizey / 16.0;
				++idx1;

				for (int k = 0; k < sizey; ++k)
				{
					double d4 = 0.0;
					double d5 = dArray[k];
					double d6 = noiseData2[idx0] / 512.0;
					double d7 = noiseData3[idx0] / 512.0;
					double d8 = (noiseData1[idx0] / 10.0 + 1.0) / 2.0;

					if (d8 < 0.0)
						d4 = d6;
					else if (d8 > 1.0)
						d4 = d7;
					else
						d4 = d6 + (d7 - d6) * d8;

					d4 -= d5;
					double var34;

					if (k > sizey - 4)
					{
						var34 = (double)((float)(k - (sizey - 4)) / 3.0F);
						d4 = d4 * (1.0 - var34) + -10.0 * var34;
					}

					if ((double)k < d2)
					{
						var34 = (d2 - (double)k) / 4.0;

						if (var34 < 0.0)
							var34 = 0.0;

						if (var34 > 1.0)
							var34 = 1.0;

						d4 = d4 * (1.0 - var34) + -10.0 * var34;
					}

					arr[idx0] = d4;
					++idx0;
				}
			}
		}

		LordSafeFree(dArray);
		return arr;
	}
}