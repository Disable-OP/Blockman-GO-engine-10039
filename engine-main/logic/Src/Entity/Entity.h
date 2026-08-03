/********************************************************************
filename: 	Entity.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-7
*********************************************************************/
#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#include "RTTI.h"
#include "BM_TypeDef.h"
#include "BM_Container_def.h"
#include "Util/Facing.h"
#include "Util/BitSet.h"
#include "Util/UUID.h"
#include "Nbt/NBT.h"
#include "Util/Listenable.h"
#include "Core.h"
#include "Audio/SoundDef.h"
using namespace LORD;

namespace BLOCKMAN
{

class World;
class Random;
class EntityItem;
class ItemStack;
class DamageSource;
class EntityLivingBase;
class EntityLightningBolt;
class EntityPlayer;
class EntityManager;
class BM_Material;
class Block;
class Explosion;

enum SelectNodeType
{
	NODE_NONE = 0,
	NODE_BLUE,
	NODE_RED
};

class Entity : public ObjectAlloc
{
	RTTI_DECLARE(Entity);

protected:
	static int	nextEntityID;

	bool		isInWeb = false;
	/** The distance that has to be exceeded in order to triger a new step sound and an onEntityWalking event on a block */
	int			nextStepDistance = 0;
	Random*		rand = nullptr;
	int			fire = 0;
	/** Whether this entity is currently inside of water (if it handles water movement that is) */
	bool		inWater = false;
	bool		firstUpdate = false;
	bool		m_isImmuneToFire = false;
	float		entityRiderPitchDelta = 0.f;
	float		entityRiderYawDelta = 0.f;

	bool		invulnerable = false;
	UUID		entityUniqueID;

	int			entityType = 0; //for debug in Visual Studio.
	
public:
	int			entityId = 0;
	float		renderDistanceWeight = 0.f;
	/** Blocks entities from spawning when they do their AABB check to make sure the spot is clear of entities that can prevent spawning. */
	bool		preventEntitySpawning = false;
	/** The entity that is riding this entity */
	Entity*	riddenByEntity = nullptr;
	/** The entity we are currently riding */
	Entity*	ridingEntity = nullptr;
	bool		forceSpawn = false;
	/** Reference to the World object. */
	World*		world = nullptr;
	Vector3		prevPos;
	Vector3		position;
	Vector3		motion;
	/** Entity rotation Yaw */
	float		rotationYaw = 0.f;
	/** Entity rotation Pitch */
	float		rotationPitch = 0.f;
	float		prevRotationYaw = 0.f;
	float		prevRotationPitch = 0.;
	/** Axis aligned bounding box. */
	Box			boundingBox;
	bool		onGround = false;
	/** True if after a move this entity has collided with something on X- or Z-axis */
	bool		isCollidedHorizontally = false;
	/** True if after a move this entity has collided with something on Y-axis */
	bool		isCollidedVertically = false;
	/** True if after a move this entity has collided with something either vertically or horizontally */
	bool		isCollided = false;
	bool		beenAttacked = false;
	//bool		field_70135_K;
	/** Gets set by setDead, so this must be the flag whether an Entity is dead (inactive may be better term) */
	bool		isDead = false;
	float		yOffset = 0.f;
	/** How wide this entity is considered to be */
	float		width = 0.f;
	/** How leng this entity is considered to be */
	float		length = 0.f;
	/** How high this entity is considered to be */
	float		height = 0.f;
	/** The previous ticks distance walked multiplied by 0.6 */
	float		prevDistanceWalkedModified = 0.f;
	/** The distance walked multiplied by 0.6 */
	float		distanceWalkedModified = 0.f;
	float		distanceWalkedOnStepModified = 0.f;
	float		fallDistance = 0.f;
	/** The entity's position at the previous tick, used to calculate position during rendering routines*/
	Vector3		lastTickPos;
	/* When player sneaks, the height of the eyes will be lower. Position of the player is determined by the height of the eyes, 
	 * which means that when player sneaks, the position.y of the player need to be decreased. When player starts to sneak, this 
	 * value is set to decrease the position.y
	 */
	float		yPositionDecrement = 0.f;
	/** How high this entity can step up when running into a block to try to get over it (currently make note the entity will always step up this amount and not just the amount needed) */
	float		stepHeight = 0.f;
	/** Whether this entity won't clip with collision or not (make note it won't disable gravity) */
	bool		noClip = false;
	/** Reduces the velocity applied by entity collisions by the specified percent. */
	float		entityCollisionReduction = 0.f;
	/** How many ticks has this entity had ran since being alive */
	int			ticksExisted = 0;
	/** The amount of ticks you have to stand inside of fire before be set on fire */
	int			fireResistance = 0;
	/** Remaining time an entity will be "immune" to further damage after being hurt. */
	int			hurtResistantTime = 0;
	/** Has this entity been added to the chunk its within */
	bool		addedToChunk = false;
	Vector3i	chunkCoord;
	Vector3i	serverPos;
	/** Render entity even if it is outside the camera frustum. Only true in EntityFish for now. Used in RenderGlobal:
	* render if ignoreFrustumCheck or in frustum. */
	bool		ignoreFrustumCheck = false;
	bool		isAirBorne = false;
	int			timeUntilPortal = 0;
	/** Which dimension the player is in (-1 = the Nether, 0 = normal world) */
	int			dimension = 0;
	int			myEntitySize = 0;

	BitSet<16>	m_flags;

	bool		m_isPulled = false;
	bool		m_flyingByPulled = false;
	int			m_remainingFlyTicksByPulled = 0;

	bool		onSelectedChange = false;
	SelectNodeType selectNodeType = SelectNodeType::NODE_NONE;

protected:
	Listenable<int>			m_air;

protected:
	void init();

	/** all virtual functions */
	virtual void entityInit() {}
	/** Takes in the distance the entity has fallen this tick and whether its on the ground to update the fall distance
	and deal fall damage if landing on the ground.  Args: distanceFallenThisTick, onGround */
	virtual void updateFallState(float y, bool onGroundIn);
	/** Called when the mob is falling. Calculates and applies fall damage. */
	virtual void fall(float y);
	/** sets the dead flag. Used when you fall off the bottom of the world. */
	virtual void kill() { setDead(); }
	/** Sets that this entity has been attacked. */
	virtual void setBeenAttacked() { beenAttacked = true; }
	
	virtual void preparePlayerToSpawn();
	/** Sets the rotation of the entity */
	void setRotation(float yaw, float pitch);
	/** Called whenever the entity is walking inside of lava. */
	void setOnFireFromLava();
	/** Plays step sound at given x, y, z for the entity */
	virtual void playStepSound(const BlockPos& pos, int blockID) {};
	/** returns if this entity triggers Block.onEntityWalking on the blocks they walk on.
	used for spiders and wolves to prevent them from trampling crops */
	virtual bool canTriggerWalking() { return true; }
	/** Will deal the specified amount of damage to the entity if the entity isn't immune to fire damage. Args: amountDamage */
	virtual void dealFireDamage(int amount);
	NBTTagList* newFloatNBTList(const Vector3& vec);
	NBTTagList* newFloatNBTList(const Vector2& vec);
	bool shouldSetPosAfterLoading() { return true; }
	/** Returns the string that identifies this Entity's class */
	String getEntityString();
	/*** Returns true if the flag is active for the entity. */
	bool getFlag(int flag) { return m_flags[flag]; }
	/** Enable or disable a entity flag, see getEntityFlag to read the know flags. */
	void setFlag(int flag, bool val);

public:
	Entity(World* pWorld);
	virtual ~Entity();

	void doConstruct();
	void doDestruct();

	static int getSuperClassID(int classID);
	/** all virtual functions */
	/** Gets called every tick from main Entity class */
	virtual void onEntityUpdate();
	/** (abstract) Protected helper method to read subclass entity data from NBT. */
	virtual void readEntityFromNBT(NBTTagCompound* pNBT) = 0;
	/** (abstract) Protected helper method to write subclass entity data to NBT. */
	virtual void writeEntityToNBT(NBTTagCompound* pNBT) = 0;
	/** Called when the entity is attacked. */
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	/** Returns true if other Entities should be prevented from moving through this Entity. */
	virtual bool canBeCollidedWith() { return false; }
	/** Returns true if this entity should push and be pushed by other entities when colliding. */
	virtual void setCanBeCollidedWith(bool isCanBeCollided){}
	virtual bool canBePushed() { return false; }
	/** Checks whether target entity is alive. */
	virtual bool isEntityAlive() { return !isDead; }
	/** Setups the entity to do the hurt animation. Only used by packets in multiplayer. */
	virtual void performHurtAnimation() {}
	/** Sets the held item, or an armor slot. Slot 0 is held item. Slot 1-4 is armor. Params: Item, slot */
	virtual void setCurrentItemOrArmor(int item, ItemStackPtr pslot) {}
	/** Set sprinting switch for Entity. */
	virtual void setSprinting(bool flag) { setFlag(ENTITY_STATUS_SPRINTING, flag); }
	virtual ItemStackArr getLastActiveItems() { return ItemStackArr(); }
	/** Called to update the entity's position/logic. */
	virtual void onUpdate() { onEntityUpdate(); }
	/** Handles updating while being ridden by an entity */
	virtual void updateRidden();
	/** Sets the position and rotation. Only difference from the other one is no bounding on the rotation. Args: posX, posY, posZ, yaw, pitch */
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	/** returns a (normalized) vector of where this entity is looking */
	virtual Vector3 getLookVec() { return Vector3::INVALID; }
	virtual float getEyeHeight() { return 0.0F; }
	virtual float getRotationYawHead() { return 0.0F; }
	/** Sets the head's yaw rotation of the entity. */
	virtual void setRotationYawHead(float yawHead) {}
	virtual bool isFlyEntity() { return false; }

	Vector3 getBottomPos() const { return Vector3(position.x, boundingBox.vMin.y, position.z); }
	Vector3 getCentorPos() const { return Vector3(position.x, (boundingBox.vMax.y + boundingBox.vMin.y) / 2, position.z); }
	Vector3 getWidelyPos() const { return Vector3(position.x, boundingBox.vMin.y + 0.5f, position.z); }
	Vector3 getTopperPos() const { return Vector3(position.x, boundingBox.vMax.y, position.z); }

	/** Sets the width and height of the entity. Args: width, height */
	void setSize(float w, float h, Vector3 scale = Vector3::ONE, bool force = false);
	/** Sets the size of the entity. Args: width, length, height */
	void setSize(float w, float l, float h, Vector3 scale = Vector3::ONE, bool force = false);
	/* Get width of entity */
	float getWidth() { return width; }
	/* Get length of entity */
	float getLength() { return length; }
	/* Get height of entity */
	float getHeight() { return height; }
	// DataWatcher getDataWatcher() { return dataWatcher; }
	int hashCode() { return entityId; }
	/** Will get destroyed next tick.*/
	virtual void setDead() { isDead = true; }
	/** Sets the x,y,z of the entity from the given parameters. Also seems to set up a bounding box. */
	void setPosition(const Vector3& pos);
	/** Adds par1*0.15 to the entity's yaw, and *subtracts* par2*0.15 from the pitch. Clamps pitch from -90 to 90. Both arguments in degrees. */
	virtual void setAngles(float yaw, float pitch);
	/** Sets entity to burn for x amount of seconds, cannot lower amount of existing fire. */
	virtual void setFire(int second);
	/** Removes fire from entity. */
	void extinguish() { fire = 0; }
	/** Checks if the offset position from the entity's current position is inside of liquid. Args: x, y, z */
	bool isOffsetPositionInLiquid(const Vector3& pos);
	/** Tries to moves the entity by the passed in displacement. Args: x, y, z */
	virtual void moveEntity(const Vector3& vel);
	/** Checks for block collisions, and calls the associated onBlockCollided method for the collided block. */
	void doBlockCollisions();
	virtual void playSoundByType(SoundType soundType);
	/** returns the bounding box for this entity */
	const Box& getBoundingBox() { return boundingBox; }
	bool isImmuneToFire() { return m_isImmuneToFire; }
	/** Checks if this entity is either in water or on an open air block in rain (used in wolves). */
	bool isWet();
	/** Checks if this entity is inside water (if inWater field is true as a result of handleWaterMovement() returning true) */
	bool isInWater() { return inWater; }
	/** Returns if this entity is in water and will end up adding the waters velocity to the entity */
	virtual bool handleWaterMovement();
	/** Checks if the current block the entity is within of the specified material type */
	bool isInsideOfMaterial(const BM_Material& mat);
	/** Whether or not the current entity is in lava */
	bool handleLavaMovement();
	/** Used in both water and by flying objects */
	virtual void moveFlying(float strafe, float forward, float speed);
	virtual int getBrightnessForRender(float rdt);
	/** Gets how bright this entity is. */
	virtual float getBrightness(float rdt);
	/** * Sets the reference to the World object.	*/
	void setWorld(World* pWorld) { world = pWorld; }
	/** Sets the entity's position and rotation. Args: posX, posY, posZ, yaw, pitch */
	void setPositionAndRotation(const Vector3& pos, float yaw, float pitch);
	/** Sets the location and Yaw/Pitch of an entity in the world */
	void setLocationAndAngles(const Vector3& pos, float yaw, float pitch);
	/** Returns the distance to the entity. Args: entity */
	float getDistanceToEntity(Entity* pEntity);
	/** Returns the squared distance to the entity. Args: entity */
	float getDistanceSqToEntity(Entity* pEntity);
	/** Gets the squared distance to the position. Args: x, y, z */
	float getDistanceSq(const Vector3& pos);
	/** Gets the distance to the position. Args: x, y, z */
	float getDistance(const Vector3& pos);
	/** Called by a player entity when they collide with an entity */
	virtual void onCollideWithPlayer(EntityPlayer* pPlayer) {}
	/** Applies a velocity to each of the entities pushing them away from each other. Args: entity */
	void applyEntityCollision(Entity* pEntity);
	/** Adds to the current velocity of the entity. Args: x, y, z */
	void addVelocity(const Vector3& vel);
	/*** Adds a value to the player score. Currently not actually used and the entity passed in does nothing */
	virtual void addToPlayerScore(Entity* pEntity, int sourceToAdd) {}
	/*** Checks using a Vec3d to determine if this entity is within range of that vector to be rendered. */
	bool isInRangeToRenderVec3D(const Vector3& vec) const;
	/** Checks if the entity is in range to render by using the past in distance and comparing it to its average edge
	* length * 64 * renderDistanceWeight Args: distance */
	virtual bool isInRangeToRenderDist(float distanceSqr) const;
	bool isInRangeToRenderDist(const Vector3& campos) const;
	bool addNotRiddenEntityID(NBTTagCompound* pNBT);
	/** adds the ID of this entity to the NBT given */
	bool addEntityID(NBTTagCompound* pNBT);
	/** Save the entity to NBT (calls an abstract helper method to write extra data) */
	void writeToNBT(NBTTagCompound* pNBT);
	/** Reads the entity from NBT (calls an abstract helper method to read specialized data) */
	void readFromNBT(NBTTagCompound* pNBT);
	void func_110123_P() {}
	float getShadowSize() { return height / 2.0F; }
	/** Drops an item stack at the entity's position. Args: itemID, count */
	EntityItem* dropItem(int itemID, int count) { return dropItemWithOffset(itemID, count, 0.0F); }
	/** Drops an item stack with a specified y offset. Args: itemID, count, yOffset */
	EntityItem* dropItemWithOffset(int itemID, int count, float yOffset);
	/** Drops an item at the position of the entity. */
	EntityItem* entityDropItem(ItemStackPtr pStack, float yOffset);
	/** Checks if this entity is inside of an opaque block */
	virtual bool isEntityInsideOpaqueBlock();
	virtual bool processInitialInteract(EntityPlayer* pPlayer) { return false; }
	/** Returns a boundingBox used to collide the entity with other entities and blocks. This enables the entity to be
	* pushable on contact, like boats or minecarts.	*/
	Box getCollisionBox(Entity* pEntity) { return Box(); }
	void updateRiderPosition();
	/** Returns the Y Offset of this entity. */
	virtual float getYOffset() { return yOffset; }
	/** Returns the Y offset from the entity's position for any entity riding this one. */
	float getMountedYOffset() { return height * 0.75f; }
	/** Called when a player mounts an entity. e.g. mounts a pig, mounts a boat. */
	virtual void mountEntity(Entity* pEntity);
	float getCollisionBorderSize() { return 0.1F; }
	/** Called by portal blocks when an entity is within it. */
	void setInPortal() { }
	/** Return the amount of cooldown before this entity can use a portal again. */
	int getPortalCooldown() { return 900; }
	/** Sets the velocity to the args. Args: x, y, z */
	virtual void setVelocity(const Vector3& vel) { motion = vel; }
	/** Returns true if the entity is on fire. Used by render to add the fire effect on rendering. */
	bool isBurning() { return !m_isImmuneToFire && (fire > 0 || getFlag(ENTITY_STATUS_BURNING)); }
	/** Returns true if the entity is riding another entity, used by render to rotate the legs to be in 'sit' position for players.	*/
	bool isRiding()	{ return ridingEntity != NULL; }
	/** Returns if this entity is sneaking. */
	virtual bool isSneaking() {	return getFlag(ENTITY_STATUS_SNEAKING); }
	/** Sets the sneaking flag. */
	void setSneaking(bool flag) { setFlag(ENTITY_STATUS_SNEAKING, flag); }
	/** Get if the Entity is sprinting. */
	bool isSprinting() { return getFlag(ENTITY_STATUS_SPRINTING); }
	bool isInvisible() { return getFlag(ENTITY_STATUS_SOMETHING); }
	virtual bool isInvisibleToPlayer(EntityPlayer* pPlayer) { return isInvisible(); }
	void setInvisible(bool flag) { setFlag(ENTITY_STATUS_SOMETHING, flag); }
	bool isEating() { return getFlag(ENTITY_STATUS_EATING); }
	void setEating(bool flag) {	setFlag(ENTITY_STATUS_EATING, flag); }
	int getAir() { return m_air; }
	void setAir(int air) { m_air = air; }
	/** Called when a lightning bolt hits the entity. */
	void onStruckByLightning(EntityLightningBolt* pLight);
	/** This method gets called when the entity kills another one. */
	virtual void onKillEntity(EntityLivingBase* pLiving) {}
	/** Adds velocity to push the entity out of blocks at the specified x, y, z position Args: x, y, z */
	bool pushOutOfBlocks(const Vector3& pos);
	/** Sets the Entity inside a web block. */
	virtual void setInWeb() { isInWeb = true; fallDistance = 0.0F; }
	/** Gets the username of the entity. */
	virtual String getEntityName();
	/** return the Entity parts count */
	int getPartsCount() { return 0; }
	/** Return the Entity parts making up this Entity (currently only for dragons) */
	Entity** getParts() { return NULL; }
	/** Returns true if Entity argument is equal to this Entity */
	bool isEntityEqual(Entity* pEntity) { return this == pEntity; }
	/** If returns false, the item will not inflict any damage against entities. */
	virtual bool canAttackWithItem() { return true; }
	bool hitByEntity(Entity* pEntity) { return false; }
	virtual String toString();
	/** Return whether this entity is invulnerable to damage. */
	bool isEntityInvulnerable() { return invulnerable; }
	void setEntityInvulnerable(bool entityInvulnerable) { invulnerable = entityInvulnerable; }
	/** Sets this entity's location and angles to the location and angles of the passed in entity. */
	void copyLocationAndAnglesFrom(Entity* pEntity);
	float getExplosionResistance(Explosion* pExplosion, World* pWorld, const BlockPos& pos, Block* pBlock);
	void copyDataFrom(Entity* pEntity, bool par2);
	bool verifyExplosion(Explosion* pExplosion, World* pWorld, const BlockPos& pos, int par6, float par7) { return true; }
	virtual int getMaxFallHeight() { return 3; }
	/** Return whether this entity should NOT trigger a pressure plate or a tripwire.*/
	virtual bool doesEntityNotTriggerPressurePlate() { return false; }
	/** Return whether this entity should be rendered as on fire. */
	bool canRenderOnFire() { return !m_isImmuneToFire && getFlag(ENTITY_STATUS_BURNING); }
	const UUID& getUniqueID() { return entityUniqueID; }
	virtual bool isPushedByWater() { return true; }
	virtual bool isImmuneToExplosions() { return false; }
	/** Returns the translated name of the entity. */
	String getTranslatedEntityName() { return getEntityName(); }
	virtual void whenRemoved() {}

	/** interpolated position vector */
	Vector3 getPosition(float rdt = 1.0f, bool bForce = false);
	Vector3 getLastTickPosition(float rdt = 1.0F);
	
	static int multiplyBy32AndRound(int s, float v);
	bool canFall();

	virtual bool canLongHit() { return false; }
	virtual int getLongHitTimes() { return -1; }

	void setOnSelected(int type);
	bool isOnSelected() { return selectNodeType != SelectNodeType::NODE_NONE; }
};

class IProjectile
{
public:
	/** Similar to setArrowHeading, it's point the throwable entity to a x, y, z direction. */
	virtual void setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy) = 0;
};

}

#endif