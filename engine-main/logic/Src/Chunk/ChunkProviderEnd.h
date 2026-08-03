#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"
#include "Util/Random.h"

namespace BLOCKMAN
{
	class World;
	class NoiseGeneratorOctaves;
	class BiomeGenBase;

	class ChunkProviderEnd : public IChunkProvider, public ObjectAlloc
	{
	public:
		ChunkProviderEnd(World* pWorld, i64 seed);
		~ChunkProviderEnd();

		virtual ChunkPtr provideChunk(int x, int z) override;

	protected:
		World * m_pWorld = nullptr;

	private:
		Random m_rand;

		NoiseGeneratorOctaves* m_pNoiseGen1 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen2 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen3 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen4 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen5 = nullptr;

		double* m_pNoiseData1 = nullptr;
		double* m_pNoiseData2 = nullptr;
		double* m_pNoiseData3 = nullptr;
		double* m_pNoiseData4 = nullptr;
		double* m_pNoiseData5 = nullptr;
		double* m_pNoiseDensities = nullptr;

		/** The biomes that are used to generate the chunk */
		BiomeGenBase** m_pBiomeGens = nullptr;
		int m_biomeGensLen = 0;

		void initialize();
		virtual ChunkPtr CreateChunk(int x, int z);
		void generateTerrain(int x, int z, i8* arr, BiomeGenBase** biomes);
		void replaceBlocksForBiome(int x, int z, i8* arr, BiomeGenBase** biomes);
		double* initializeNoiseField(double* arr, int xOffset, int yOffset, int zOffset, int width, int length, int depth);
	};
}