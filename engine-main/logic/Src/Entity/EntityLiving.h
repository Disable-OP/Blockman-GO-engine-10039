/********************************************************************
filename: 	EntityLiving.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ENTITY_LIVING_HEADER__
#define __ENTITY_LIVING_HEADER__

#include "EntityLivingBase.h"
#include "PathNavigate.h"

namespace BLOCKMAN
{

class EntityLiving;
class Item;
class AITasks;

class EntityLookHelper
{
protected:
	EntityLiving& entity;
	/** The amount of change that is made each update for an entity facing a direction. */
	float deltaLookYaw = 0.f;
	/** The amount of change that is made each update for an entity facing a direction. */
	float deltaLookPitch = 0.f;
	/** Whether or not the entity is trying to look at something. */
	bool isLooking = false;
	Vector3 position;
	
protected:
	float updateRotation(float angle1, float angle2, float angleMax);

public:
	EntityLookHelper(EntityLiving& living);
	
	/** Sets position to look at using entity */
	void setLookPositionWithEntity(Entity* pEntity, float deltaYaw, float deltaPitch);
	/** Sets position to look at */
	void setLookPosition(const Vector3& lookat, float deltaYaw, float deltaPitch);
	/** Updates look */
	void onUpdateLook();
	bool getIsLooking() { return isLooking; }
	const Vector3& getLookPos() { return position; }
};

class EntityMoveHelper
{
protected:
	/** The EntityLiving that is being moved */
	EntityLiving& entity;
	Vector3 position;
	/** The speed at which the entity should move */
	float speed = 0.f;
	bool update = false;

protected:
	/** Limits the given angle to a upper and lower limit. */
	float limitAngle(float angle1, float angle2, float angleMax);

public:
	EntityMoveHelper(EntityLiving& living);
	
	bool isUpdating() { return update; }
	float getSpeed() {	return speed; }
	/** Sets the speed and location to move to */
	void setMoveTo(const Vector3& pos, float speedIn);
	void onUpdateMoveHelper();
};

class EntityJumpHelper
{
protected:
	EntityLiving& entity;
	bool isJumping = false;

public:
	EntityJumpHelper(EntityLiving& living);

	void setJumping() { isJumping = true; }
	/** Called to actually make the entity jump if isJumping is true. */
	void doJump();
};


class EntityBodyHelper
{
protected:
	/** Instance of EntityLiving. */
	EntityLivingBase& theLiving;
	/** Used to progressively ajust the rotation of the body to the rotation of the head */
	int rotationTickCounter = 0;
	float prevRenderYawHead = 0.f;

protected:
	/*** Return the new angle2 such that the difference between angle1 and angle2 is lower than angleMax. */
	float computeAngleWithBound(float angle1, float angle2, float bound);

public:
	EntityBodyHelper(EntityLivingBase& living);

	/** Update the Head and Body rendenring angles */
	void updateRenderAngles();
};

class EntitySenses
{
public:
	typedef set<Entity*>::type EntitySet;

protected:
	EntityLiving& entityObj;

	/** Cache of entities which we can see */
	EntitySet seenEntities;
	/** Cache of entities which we cannot see */
	EntitySet unseenEntities;

public:
	EntitySenses(EntityLiving& living);
	
	/** Clears canSeeCachePositive and canSeeCacheNegative. */
	void clearSensingCache();
	
	/** Checks, whether 'our' entity can see the entity given as argument (true) or not (false), caching the result. */
	bool canSee(Entity* pEntity);
};

class EntityLivingData
{
};

class EntityLiving : public EntityLivingBase
{
	RTTI_DECLARE(EntityLiving);

protected:
	/** The experience points the Entity gives. */
	int experienceValue = 0;
	EntityLookHelper lookHelper;
	EntityMoveHelper moveHelper;

	/** Entity jumping helper */
	EntityJumpHelper jumpHelper;
	EntityBodyHelper bodyHelper;
	PathNavigate navigator;

	// EntityAITasks tasks;
	// EntityAITasks targetTasks;

	/** The active target the Task system uses for tracking */
	EntityLivingBase* attackTarget = nullptr;
	EntitySenses senses;

	/** Equipment (armor and held item) for this entity. */
	const static int EQUIP_SLOT = 5;
	ItemStackPtr equipment[EQUIP_SLOT];
	/** Chances for each equipment piece from dropping when this entity dies. */
	float equipmentDropChances[EQUIP_SLOT];
	/** Whether this entity can pick up items from the ground. */
	bool m_canPickUpLoot = false;
	/** Whether this entity should NOT despawn. */
	bool persistenceRequired = false;
	float defaultPitch = 0.f;

	/** This entity's current target. */
	int     m_currentTargetEntityId = 0;

	/** How long to keep a specific target entity */
	int numTicksToChaseTarget = 0;
	bool isLeashed = false;
	Entity* leashedToEntity = nullptr;
	NBTTagCompound* leashNBTTag = nullptr;
	
	/** data of EntityLiving*/
	i8 data_ai_flag = 0;
	String data_custom_name = "";

	AITasks* m_tasks = nullptr;
	AITasks* m_targetTasks = nullptr;

public:
	/** Number of ticks since this EntityLiving last produced its sound */
	int livingSoundTime = 0;
		
protected:
	/** Returns the sound this mob makes while it's alive. */
	SoundType getLivingSoundType(){ return ST_Invalid; } // todo
	/** Returns the item ID for the item the mob drops on death. */
	int getDropItemId() { return 0; }
	/** Determines if an entity can be despawned, used on idle far away entities */
	bool canDespawn() { return true; }
	/** Makes the entity despawn if requirements are reached */
	void despawnEntity();
	/** Arguments: current rotation, intended rotation, max increment. */
	float updateRotation(float angle1, float angle2, float angleBound);
	/** Makes entity wear random armor based on difficulty */
	void addRandomArmor();
	/** Enchants the entity's armor and held item based on difficulty */
	void enchantEquipment();
	/** Called when a player interacts with a mob. e.g. gets milk from a cow, gets into the saddle on a pig. */
	bool interact(EntityPlayer* pPlayer) { return false; }
	virtual void updateLeashedState();
	void recreateLeash();

	/** implement override functions from EntityLivingBase */
	virtual void applyEntityAttributes();
	virtual void entityInit();
	virtual int getExperiencePoints(EntityPlayer* pPlayer);
	virtual float updateDistance(float yaw, float distance);
	virtual void dropFewItems(bool wasRecentlyHit, int lootingModifier);
	virtual bool isAIEnabled() { return false; }
	virtual void updateAITasks();
	virtual void updateEntityActionState();
	virtual void dropEquipment(bool wasRecentlyHit, int lootingModifier);
	
public:
	EntityLiving(World* pWorld);
	virtual ~EntityLiving();
	
	EntityLookHelper& getLookHelper() { return lookHelper; }
	EntityMoveHelper& getMoveHelper() { return moveHelper; }
	EntityJumpHelper& getJumpHelper() { return jumpHelper; }
	PathNavigate& getNavigator() { return navigator; }
	EntitySenses& getEntitySenses() { return senses; }

	EntityLivingBase* getAttackTarget() { return attackTarget; }
	/** Sets the active target the Task system uses for tracking */
	void setAttackTarget(EntityLivingBase* pLiving) { attackTarget = pLiving; }
	/** Returns true if this entity can attack entities of the specified class.	*/
	// bool canAttackClass(Class par1Class) { return EntityCreeper.class != par1Class && EntityGhast.class != par1Class; }
	/** This function applies the benefits of growing back wool and faster growing up to the acting entity. (This function is used in the AIEatGrass) */
	void eatGrassBonus() {}
	/** Get number of ticks, at least during which the living entity will be silent. */
	int getTalkInterval() {	return 80; }
	/** Plays living's sound at its position */
	void playLivingSound();
	/** Spawns an explosion particle around the Entity's location */
	void spawnExplosionParticle();

	/** The speed it takes to move the entityliving's rotationPitch through the faceEntity method. This is only currently use in wolves. */
	int getVerticalFaceSpeed() { return 40; }
	/** Changes pitch and yaw so that the entity calling the function is facing the entity provided as an argument. */
	void faceEntity(Entity* pTarget, float yawBound, float pitchBound);
	/** Checks if the entity's current position is a valid location to spawn this entity. */
	virtual bool getCanSpawnHere();
	/** Returns render size modifier */
	float getRenderSizeModifier() { return 1.0F; }
	/** Will return how many at most can spawn in a chunk at once. */
	int getMaxSpawnedInChunk() { return 4; }
	ItemStackPtr getArmorFromSlot(int slot) { return equipment[slot + 1]; }
	/** returns true if all the conditions for steering the entity are met. */
	bool canBeSteered() { return false; }
	void enablePersistence() { persistenceRequired = true; }
	void setCustomNameTag(const String& name) { data_custom_name = name; }
	const String& getCustomNameTag() { return data_custom_name; }
	bool hasCustomNameTag() { return !data_custom_name.empty(); }
	void setNoAI(bool disable) { data_ai_flag = disable ? 1 : 0; }
	bool isAIDisable() { return data_ai_flag == 1; }
	bool getAlwaysRenderNameTagForRender() { return isAIDisable(); }
	void setEquipmentDropChance(int slot, float chance) { equipmentDropChances[slot] = chance; }
	bool canPickUpLoot() { return m_canPickUpLoot; }
	void setCanPickUpLoot(bool canPick) { m_canPickUpLoot = canPick; }
	bool isNoDespawnRequired() { return persistenceRequired; }
	void setLeashedToEntity(Entity* pEntity, bool sendNotification);
	void clearLeashed(bool sendPacket, bool dropLead);
	bool canBeLeashedTo();
	bool getLeashed() const { return isLeashed; }
	Entity* getLeashedToEntity() { return leashedToEntity; }
	EntityLivingData* onInitialSpawn(EntityLivingData* livingdata);
	
public:
	static int getArmorPosition(ItemStackPtr pStack);
	/** Params: Armor slot, Item tier */
	static Item* getArmorItemForSlot(int armorSlot, int tier);
	
public:
	/** implement override  function. */
	virtual void onEntityUpdate();
	virtual void onUpdate();
	virtual void writeEntityToNBT(NBTTagCompound* pNbtEntity);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtEntity);
	virtual void setAIMoveSpeed(float speed);
	virtual void onLivingUpdate();
	virtual int getMaxFallHeight();
	virtual ItemStackPtr getHeldItem() { return equipment[0]; }
	virtual ItemStackPtr getCurrentItemOrArmor(int slot) { return equipment[slot]; }
	virtual void setCurrentItemOrArmor(int armorSlot, ItemStackPtr pStack) { equipment[armorSlot] = pStack; }
	virtual ItemStackArr getLastActiveItems() { return ItemStackArr(equipment, equipment+ EQUIP_SLOT); }
	virtual String getEntityName();
	virtual bool processInitialInteract(EntityPlayer* pPlayer);
};

}

#endif