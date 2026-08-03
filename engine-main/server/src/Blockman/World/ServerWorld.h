/********************************************************************
filename: 	ServerWorld
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\World\ServerWorld.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __SERVERWORLD_H__
#define __SERVERWORLD_H__

#include "Global.h"
#include "World/World.h"
#include "World/WorldSettings.h"
#include "World/WorldProvider.h"
#include "World/GameSettings.h"
#include "Util/SubscriptionGuard.h"
#include "Entity/EntityBuildNpc.h"
#include "LuaRegister/Template/TableVisitor.h"
namespace BLOCKMAN
{

class EntityTracker;

struct BlockEventData
{
	BlockPos m_pos;
	int		 m_blockID;
	int		 m_eventID;
	int		 m_eventParam;

	BlockEventData(const BlockPos& pos, int blockid, int eventid, int eventparam)
		: m_pos(pos)
		, m_blockID(blockid)
		, m_eventID(eventid)
		, m_eventParam(eventparam)
	{}

	bool operator == (const BlockEventData& rhs) const
	{
		return m_pos == rhs.m_pos && m_blockID == rhs.m_blockID && m_eventID == rhs.m_eventID && m_eventParam == rhs.m_eventParam;
	}
};

typedef list<BlockEventData>::type BlockEventList;

struct TickEntry
{
	static i64 nextTickEntryID;
	BlockPos m_pos;
	int blockID;
	int priority;
	i64 hashcode;
	i64 scheduledTime;
	i64 tickEntryID;

	TickEntry()
	{}

	TickEntry(const BlockPos pos, int id)
		: m_pos(pos)
	{
		tickEntryID = nextTickEntryID++;
		blockID = id;
		hashcode = i64(m_pos.x * 1024 * 1024 + m_pos.z * 1024 + m_pos.y) * 256 + blockID;
		priority = 0;
		scheduledTime = 0;
	}

	bool operator == (const TickEntry& rhs) const;
	bool operator > (const TickEntry& rhs) const;
	bool operator < (const TickEntry& rhs) const;

	void setScheduledTime(i64 t) { scheduledTime = t; }
	void setPriority(int p) { priority = p; }
};

typedef list<TickEntry>::type TickEntryLst;
typedef set<TickEntry>::type TickEntrySet;

class ServerWorld : public World
{
private:
	SubscriptionGuard m_subscriptionGuard;

public:
	ServerWorld(const String& name, WorldProvider* _provider, const WorldSettings& settings, int loadRange);
	virtual ~ServerWorld(void);

	//static std::shared_ptr<ServerWorld> createWorld(const String & path);
	static ServerWorld * createWorld(const String & name);
	void destroy();

	ChunkService* createChunkService(int loadRange) override;

	/** implement override functions from World. */
	virtual void tick();
	virtual bool tickUpdates(bool par1) override;
	virtual void addBlockEvent(const BlockPos& pos, int blockID, int id, int param);
	virtual bool isBlockTickScheduledThisTick(const BlockPos& pos, int blockID) override;
	virtual void scheduleBlockUpdate(const BlockPos& pos, int blockID, int tickrate) override;
	virtual void scheduleBlockUpdateWithPriority(const BlockPos& pos, int blockID, int tickrate, int priority) override;
	virtual void scheduleBlockUpdateFromLoad(const BlockPos& pos, int blockID, int tickrate, int priority) override;
	virtual float getFrameDeltaTime();
	void createShop();
	void addBlockDoor(const BlockPos& pos, int maxTick);
	void removeBlockDoor(const BlockPos& pos);
	void showBuyRespawnToPlayer(ui64 rakssId, int times);
	void setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed);
	int tryAllPlayerTakeAircraft(Vector3 startPos, Vector3 endPos, float speed);
	void generate();
	void fireTNT(Vector3i tntPos);
	void addAirDrop(float x, float y, float z);

	int addCreature(Vector3 pos, int monsterId, float yaw, String actorName);
	int addCreatureWithName(Vector3 pos, int monsterId, float yaw, String actorName, String nameLang);
	int addCreatureWithPlayer(Vector3 pos, ui64 userId, int monsterId, float yaw, String actorName, String nameLang);
	void setCreatureTeam(int entityId, int teamId);
	void killCreature(int entityId);
	void changeCreatureAttackTarget(int entityId, int targetId);
	bool setCropsBlock(ui64 platformId, const BlockPos& pos, int blockID, int metadata, int curStageTime, int stealCount, std::time_t lastUpdateTime, int residueHarvestNum = 1);
	bool setFruitsBlock(const BlockPos& pos, int blockID);

	// create or destroy house from a schematic file
	void createOrDestroyHouseFromSchematic(String fileName, const BlockPos& start_pos, bool xImage = false, bool zImage = false, bool createOrDestroy = true) override;
	// fill blocks from start_pos to end_pos.
	void fillAreaByBlockIdAndMate(const BlockPos& start_pos, const BlockPos& end_pos, int block_id, int mate = 0) override;
	void updateManorOwner(ui64 rakssid, std::vector<ManorOwner> owners) override;
	// block is in record
	bool isInChangeRecord(BlockPos blockPos, int srcId, int srcMeta) override;

	// destroy AgentPlayer in EntityCreatureAI
	void destroyAgentPlayer();
	void addSingleUpdateBlock(BlockPos pos, int block_id, int meta);

	void addSkillEffect(int skillEffectId, Vector3 position);
	void updateActorNpcContent(int entityId, String content);
	void updateBuildNpc(i32 entityId, i32 maxQueueNum, i32 productCapacity, i32 queueUnlockPrice, i32 queueUnlockCurrencyType, std::vector<RanchCommon> products,  std::vector<ProductQueue> unlockQueues, std::vector<ProductRecipe> recipes);
	int addLandNpc(Vector3 pos, float yaw, ui64 userId, i32 landCode, String nameLang, String actorName, String actorBody, String actorBodyId) override;
	void updateLandNpc(i32 entityId, i32 status, i32 price, i32 totalTime, i32 timeLeft, std::vector<RanchCommon> recipe, std::vector<RanchCommon> reward) override;

	void addSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId = 0, float scale = 1.0f);
	void setRailSpeedInfo(int entityId, float maxSpeed, float curSpeed, float addSpeed);
	void setRailCarMaxSpeed(int entityId, float maxSpeed);
	void setRailCarCurSpeed(int entityId, float curSpeed);
	void setRailCarAddSpeed(int entityId, float addSpeed);
	void setRailCarRecordRoute(int entityId, bool isRecord);
	void setRailCarActor(int entityId, String actorName);
	void setRailCarName(int entityId, String nameLang);
	void startRailCarRun(int entityId, float angle);
	void changeRailCarDirection(int entityId);
	void setRailCarFrozen(int entityId, float frozenTime);
	bool rangeCheckBlockByEntityId(i32 entityId, i32 blockId) override;
	void playCloseup(const Vector3& position, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch);
	void setEntitySelected(ui64 rakssId, int entityId, int type);
	int addEntityBulletin(Vector3 pos, float yaw, i32 bulletinId) override;
	void updateEntityBulletin(ui64 rakssid, i32 entityId, i32 bulletinId) override;
	int addEntityBird(Vector3 pos, TableVisitor birdMgr, Vector3i homePos, ui64 userId, i64 birdId, String actorName, String actorBody, String actorBodyId);
	void setBirdDress(i32 entityId, String dressGlasses, String dressHat, String dressBeak, String dressWing, String dressTail, String dressEffect);
	void updateBirdDress(i32 entityId, String  masterName, String  slaveName);
	void updateSessionNpc(i32 entityId, ui64 rakssid, String nameLang, String actorName, String actorBody, String actorBodyId, String sessionContent, String actorAction, i32 timeLeft = 0, bool isCanCollided = true);
	void setEntityHealth(i32 entityId, float health, float maxHealth);
	void setCreatureHome(i32 entityId, Vector3i pos,  int dis = 0);

protected:
	void sendAndApplyBlockEvents();

	/** Called to apply a pending BlockEvent to apply to the current world. */
	bool onBlockEventReceived(const BlockEventData& data);

public:
	int m_blockEventIndex;

	/** pending tick list entries this tick. */
	TickEntryLst m_tickEntryList;
	TickEntrySet m_tickEntryTreeSet;
	TickEntrySet m_tickEntryHashSet;

	/** Double buffer of ServerBlockEventList[] for holding pending BlockEventData's */
	BlockEventList m_blockEventCache[2];

	GameSettings * m_gamesSetting = nullptr;
};

}
#endif
