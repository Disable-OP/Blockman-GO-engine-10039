#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"
#include "Util/Random.h"

namespace BLOCKMAN
{
	class World;
	class NoiseGeneratorOctaves;
	class MapGenNetherBridge;
	class MapGenBase;

	class ChunkProviderHell : public IChunkProvider, public ObjectAlloc
	{
	public:
		ChunkProviderHell(World* pWorld, i64 seed);
		virtual ~ChunkProviderHell();

		virtual ChunkPtr provideChunk(int x, int z) override;

	protected:
		World * m_pWorld = nullptr;

	private:
		Random m_rand;

		/** A NoiseGeneratorOctaves used in generating nether terrain */
		NoiseGeneratorOctaves* m_pNoiseGen1 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen2 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen3 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen4 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen5 = nullptr;

		double* noiseData1 = nullptr;
		double* noiseData2 = nullptr;
		double* noiseData3 = nullptr;
		double* noiseData4 = nullptr;
		double* noiseData5 = nullptr;
		double* m_pNoiseDensities = nullptr;
		double* m_pNoiseSlowSand = nullptr;
		double* m_pNoiseGravel = nullptr;
		double* m_pNoiseNetherrack = nullptr;

		/** Is the world that the nether is getting generated. */
		MapGenNetherBridge* m_pGen_bridge = nullptr;
		MapGenBase* m_pGen_netherCave = nullptr;

		NoiseGeneratorOctaves * m_pNoiseGen6 = nullptr;
		NoiseGeneratorOctaves* m_pNoiseGen7 = nullptr;

		void initialize();
		virtual ChunkPtr CreateChunk(int x, int z);
		void generateNetherTerrain(int x, int z, i8* arr);
		void replaceBlocksForBiome(int x, int z, i8* arr);
		double* initializeNoiseField(double* arr, int offsetx, int offsety, int offsetz, int sizex, int sizey, int sizez);
		bool unloadQueuedChunks() { return false; }
	};
}