/********************************************************************
filename: 	WorldProvider.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-11
*********************************************************************/
#ifndef __WORLD_PROVIDER_HEADER__
#define __WORLD_PROVIDER_HEADER__

#include "BM_TypeDef.h"

#include "Core.h"
using namespace LORD;


namespace BLOCKMAN
{

class World;
class WorldChunkManager;
class IChunkProvider;

class WorldProvider : public ObjectAlloc
{
public:
	static float MOON_PHASE_FACTORS[8];// = new float[] {1.0F, 0.75F, 0.5F, 0.25F, 0.0F, 0.25F, 0.5F, 0.75F};

	/** world object being used */
	World* worldObj = nullptr;
	TERRAIN_TYPE terrainType = TERRAIN_TYPE_INVALID;
	String generateOptions;

	/** World chunk manager being used to generate chunks */
	WorldChunkManager* worldChunkMgr = nullptr;

	/** States whether the Hell world provider is used(true) or if the normal world provider is used(false)	*/
	bool isHellWorld = false;

	/** A boolean that tells if a world does not have a sky. Used in calculating weather and skylight */
	bool hasNoSky = false;

	/** Light to brightness conversion table */
	float* lightBrightnessTable = nullptr; // = new float[16];

	/** The id for the dimension (ex. -1: Nether, 0: Overworld, 1: The End) */
	int dimensionId = 0;

public:
	WorldProvider();
	virtual ~WorldProvider();

	/** associate an existing world with a World provider, and setup its lightbrightness table */
	void registerWorld(World* par1World);

public:
	/** Array for sunrise/sunset colors (RGBA) */
	float* colorsSunriseSunset; //  = new float[4];

	/** Creates the light to brightness table */
	virtual void generateLightBrightnessTable();

	/** creates a new world chunk manager for WorldProvider */
	virtual void registerWorldChunkManager();

	/** Returns a new chunk provider which generates chunks for this world */
	virtual IChunkProvider* createChunkGenerator();

	/** Will check if the x, z position specified is alright to be set as the map spawn point */
	virtual bool canCoordinateBeSpawn(int x, int z) const;

	/** Calculates the angle of sun and moon in the sky relative to a specified time (usually worldTime) */
	virtual float calculateCelestialAngle(i64 dayticks, float rdt);

	int getMoonPhase(i64 dayticks) { return (int)(dayticks / 24000L) % 8; }

	/** Returns 'true' if in the "main surface world", but 'false' if in the Nether or End dimensions. */
	virtual bool isSurfaceWorld() const { return true; }

	/** Returns array with sunrise/sunset colors */
	virtual float* calcSunriseSunsetColors(float angle, float rdt);

	/** Return Vec3D with biome specific fog color */
	virtual Color getFogColor(float angle, float rdt);

	/** True if the player can respawn in this dimension (true = overworld, false = nether). */
	virtual bool canRespawnHere() { return true; }

	static WorldProvider* getProviderForDimension(int dimmesion);

	/** the y level at which clouds are rendered. */
	virtual float getCloudHeight() { return 128.0F; }

	virtual bool isSkyColored() { return true; }

	/** Gets the hard-coded portal location to use when entering this dimension.*/
	// ChunkCoordinates getEntrancePortalLocation()

	virtual int getAverageGroundLevel() { return terrainType == TERRAIN_TYPE_FLAT ? 4 : 64; }

	/** returns true if this dimension is supposed to display void particles and pull in the far plane based on the user's Y offset. */
	bool getWorldHasVoidParticles() { return terrainType != TERRAIN_TYPE_FLAT && !hasNoSky; }

	/** Returns a double value representing the Y value relative to the top of the map at which void fog is at its
	* maximum. The default factor of 0.03125 relative to 256, for example, means the void fog will be at its maximum at
	* (256*0.03125), or 8.*/
	double getVoidFogYFactor() { return terrainType == TERRAIN_TYPE_FLAT ? 1.0 : 0.03125; }

	/** Returns true if the given X,Z coordinate should show environmental fog. */
	virtual bool doesXZShowFog(int x, int z) { return false; }

	/** Returns the dimension's name, e.g. "The End", "Nether", or "Overworld". */
	virtual String getDimensionName() = 0;
};

class WorldProviderHell : public WorldProvider
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual void registerWorldChunkManager();
	virtual Color getFogColor(float par1, float par2) { return Color(0.20000000298023224, 0.029999999329447746, 0.029999999329447746); }
	virtual IChunkProvider* createChunkGenerator();
	virtual bool isSurfaceWorld() const { return false; }
	virtual bool canCoordinateBeSpawn(int par1, int par2) const { return false; }
	virtual float calculateCelestialAngle(i64 par1, float par3) { return 0.5F; }
	virtual bool canRespawnHere() { return false; }
	virtual bool doesXZShowFog(int x, int z) { return true; }
	virtual String getDimensionName() { return String("Nether"); }
	virtual void generateLightBrightnessTable();
};

class WorldProviderSurface : public WorldProvider
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual IChunkProvider* createChunkGenerator();
	virtual String getDimensionName() { return String("Overworld");	}
};

class WorldProviderEnd : public WorldProvider
{
public:
	/** Implement virtual funtions from WorldProvider */
	virtual void registerWorldChunkManager();
	virtual IChunkProvider* createChunkGenerator();
	virtual float calculateCelestialAngle(i64 par1, float par3) { return 0.0F; }
	virtual float* calcSunriseSunsetColors(float par1, float par2) { return NULL; }
	virtual Color getFogColor(float par1, float par2);
	virtual bool isSkyColored() { return false; }
	virtual bool canRespawnHere() { return false; }
	virtual bool isSurfaceWorld() { return false; }
	virtual float getCloudHeight() { return 8.0F; }
	virtual bool canCoordinateBeSpawn(int par1, int par2);
	virtual int getAverageGroundLevel() { return 50; }
	virtual bool doesXZShowFog(int par1, int par2) { return true; }
	virtual String getDimensionName() { return String("The End"); }
};

}

#endif