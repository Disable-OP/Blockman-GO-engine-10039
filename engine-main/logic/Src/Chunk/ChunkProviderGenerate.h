#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"
#include "Util/Random.h"

namespace BLOCKMAN
{
	class World;
	class NoiseGeneratorOctaves;
	class WorldGenerator;
	class MapGenBase;
	class MapGenMineshaft;
	class BiomeGenBase;

	class ChunkProviderGenerate : public IChunkProvider, public ObjectAlloc
	{
	public:
		ChunkProviderGenerate(World* pWorld, i64 seed, bool par4);
		virtual ~ChunkProviderGenerate();

		virtual ChunkPtr provideChunk(int x, int z) override;

	protected:
		World * m_pWorld = nullptr;

	private:
		Random m_rand;

		/** A NoiseGeneratorOctaves used in generating terrain */
		NoiseGeneratorOctaves* m_pNoise_minLimit = nullptr;
		NoiseGeneratorOctaves* m_pNoise_maxLimit = nullptr;
		NoiseGeneratorOctaves* m_pNoise_main = nullptr;
		NoiseGeneratorOctaves* m_pNoise_surface = nullptr;
		NoiseGeneratorOctaves* m_pNoise_scale = nullptr;
		NoiseGeneratorOctaves* m_pNoise_depth = nullptr;
		NoiseGeneratorOctaves* m_pNoise_forest = nullptr;

		double* m_mainRegion = nullptr;
		double* m_minLimitRegion = nullptr;
		double* m_maxLimitRegion = nullptr;
		double* m_scaleRegion = nullptr;
		double* m_depthRegion = nullptr;

		bool m_bMapFeaturesEnabled = false;

		WorldGenerator* m_pGen_waterstill = nullptr;
		WorldGenerator* m_pGen_lavastill = nullptr;
		WorldGenerator* m_pGen_dungeons = nullptr;

		/** Holds the overall noise array used in chunk generation */
		double* m_pNoiseArr = nullptr;
		double* m_pStoneNoise = nullptr;
		MapGenBase*			m_pMapGen_cave = nullptr;
		// MapGenStronghold strongholdGenerator = new MapGenStronghold();
		// MapGenVillage	villageGenerator = new MapGenVillage();
		MapGenMineshaft*	m_pMapGen_mineshaft = nullptr;
		MapGenBase*			m_pMapGen_ravine = nullptr;

		/** The biomes that are used to generate the chunk */
		BiomeGenBase** m_pBiomes = nullptr;
		int m_biomesLen = 0;

		/** Used to store the 5x5 parabolic field that is used during terrain generation. */
		float* m_parabolicField = nullptr;

		void initialize();
		virtual ChunkPtr CreateChunk(int x, int z);

		void generateTerrain(int x, int z, i8* arr);
		void replaceBlocksForBiome(int x, int z, i8* arr, BiomeGenBase** biomes);
		double* initializeNoiseField(double* arr, int bgeinx, int beginy, int beginz, int endx, int endy, int endz);
	};
}