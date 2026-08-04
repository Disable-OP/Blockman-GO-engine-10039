/********************************************************************
filename: 	WorldClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-4
*********************************************************************/
#ifndef __WORLD_CLIENT_HEADER__
#define __WORLD_CLIENT_HEADER__

/** header files from common. */
#include "World/World.h"

namespace BLOCKMAN
{

class WorldClient : public World
{
	friend class ChunkClient;
public:
	typedef map<ui32, int>::type FrameChunkTaskMap;

protected:
	int m_cloudColour = 0; //  = 0xFFFFFF

	float m_prevRainingStrength = 0.f;
	float m_rainingStrength = 0.f;
	float m_prevThunderingStrength = 0.f;
	float m_thunderingStrength = 0.f;

	/**
	* is a temporary list of blocks and light values used when updating light levels. Holds up to 32x32x32 blocks (the
	* maximum influence of a light source.) Every element is a packed bit value: 0000000000LLLLzzzzzzyyyyyyxxxxxx. The
	* 4-bit L is a light level used when darkening blocks. 6-bit numbers x, y and z represent the block's offset from
	* the original block, plus 32 (i.e. value of 31 would mean a -1 offset
	*/
	int* m_lightUpdateBlockList = nullptr;
	FrameChunkTaskMap m_reBuildEvent;
	// int* m_lightUpdateBlockList_thread;

public:
	void init2();
	WorldClient(const String& name, WorldProvider* _provider, const WorldSettings& settings, int loadRange);
	virtual ~WorldClient();

	virtual ChunkService* createChunkService(int loadRange) override;

	/** Returns the render type of the block at the given coordinate. */
	int blockGetRenderType(const BlockPos& pos);

	float getWeightedThunderStrength(float rdt) { return (m_prevThunderingStrength + (m_thunderingStrength - m_prevThunderingStrength) * rdt) * getRainStrength(rdt); }
	float getRainStrength(float rdt) { return m_prevRainingStrength + (m_rainingStrength - m_prevRainingStrength) * rdt; }
	void setRainStrength(float rdt) { m_prevRainingStrength = rdt; m_rainingStrength = rdt; }
	
	/**
	* Gets the light value of a block location. This is the actual function that gets the value and has a bool flag
	* that indicates if its a half step block to get the maximum light value of a direct neighboring block (left,
	* right, forward, back, and up)
	*/
	int getBlockLightValue_impl(const BlockPos& pos, bool flag);
			
	/** Sets the light value either into the sky map or block map depending on if enumSkyBlock is set to sky or block. */
	void setLightValue(int lightValue, const BlockPos& pos, int newLightValue);
				
	/** Plays a sound at the entity's position. Args: entity, sound, volume, and pitch. */
	void playSoundAtEntityByType(Entity* pEntity, SoundType soundTyp);
	
	/** Returns the sun brightness - checks time of day, rain and thunder */
	float getSunBrightness(float rdt);

	/** tick the player's around 25 chunks. to re-caculate the blockLight. */
	void tickAroundAmbiance();
	void tickRebuildChunks();

	/** Calculates the color for the skybox */
	Color getSkyColor(Entity* pEntity, float rdt);
	Color getSkyColor(float rdt) { return Color(0.4f, 0.6f, 0.9f); }

	int getMoonPhase();

	Color getCloudColour(float rdt);

	/** Returns vector(ish) with R/G/B for fog	*/
	Color getFogColor(float rdt);
	
	/** How bright are stars in the sky */
	float getStarBrightness(float rdt);

	void calculateInitialWeather();

	/** Runs a single tick for the world */
	virtual void tick();

	/*** Updates all weather states. */
	void updateWeather();
	
	int computeLightValue(const BlockPos& pos, int lightValue);

	float getMoonPhaseFactor();

	void doVoidFogParticles(BlockPos pos);
	ui64 findManorByPlayerPosition(EntityPlayer* player);

	struct ChunkInfo
	{
		ChunkInfo(int x, int z) :chunkX(x), chunkZ(z) {}
		int chunkX = 0;
		int chunkZ = 0;

		bool operator< (const ChunkInfo& c) const
		{
			if (chunkX != c.chunkX)
			{
				return chunkX < c.chunkX;
			}
			else if (chunkZ != c.chunkZ)
			{
				return chunkZ < c.chunkZ;
			}
			return chunkZ < c.chunkZ;
		}
	};

	static const int MAX_SPECIAL_SOUND_TYPE = 4;
	const unsigned int MAX_SPECIAL_SOUND_TICK = 100;
	const unsigned int MAX_SPECIAL_SOUND_TICK_LIMIT = 1000;
	int m_special_sound_tick[MAX_SPECIAL_SOUND_TYPE];

	enum SpecialSoundType
	{
		SST_WATER = 0,
		SST_FIRE = 1,
		SST_SWIM = 2,
		SST_LIQUID_SPLASH = 3,
		SST_COUNT,
	};

	bool canPlaySpecialSound(int sst, int max_tick = 0);
	void updateSpecialSound();

	/** implement virtual function from World*/
	virtual bool updateLightByType(int light, const BlockPos& pos) override;
	virtual void markBlocksDirtyVertical(int x, int z, int y1, int y2) override;
	virtual void updateAllLightTypes(const BlockPos& pos) override;
	virtual int  getFullBlockLightValue(const BlockPos& pos) override;
	virtual int  getSavedLightValue(int light, const BlockPos& pos) override;
	virtual void spawnParticle(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr) override;
	virtual int  getBlockLightValue(const BlockPos& pos) override { return getBlockLightValue_impl(pos, true); }
	virtual int  getSkyBlockTypeBrightness(int light, const BlockPos& pos) override;
	virtual int  getLightBrightnessForSkyBlocks(const BlockPos& pos, int minBrightness) override;
	virtual float getBrightness(const BlockPos& pos, int minBrightness) override;
	virtual float getLightBrightness(const BlockPos& pos) override;
	virtual bool setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate = true) override;
	virtual bool setBlocks(int x, int z, const BlockModifys& modifys) override;
	virtual void playSoundOfPlaceBlock(const BlockPos& pos, int blockId) override;
	virtual void playSoundOfStepOnBlock(const BlockPos& pos, int blockId) override;
	virtual void playSoundByType(const Vector3& pos, SoundType soundType) override;
	virtual void setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed) override;
	virtual float getFrameDeltaTime() override;

	// create or destroy house from a schematic file
	void createOrDestroyHouseFromSchematic(String path, const BlockPos& start_pos, bool xImage = false, bool zImage = false, bool createOrDestroy = true) override;
	// fill blocks from start_pos to end_pos.
	void fillAreaByBlockIdAndMate(const BlockPos& start_pos, const BlockPos& end_pos, int block_id, int mate = 0) override;
	// handle large blocks
	void handleLargeBlocks(const BlockModifys& blocks_info) override;
	
	bool checkRangeBlockIsAir(const Vector3& beginPos, const Vector3& endPos);
};

}

#endif