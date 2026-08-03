/********************************************************************
filename: 	EntityLivingBase.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-14
*********************************************************************/
#ifndef __LIVING_BASE_HEADER__
#define __LIVING_BASE_HEADER__

#include "Entity.h"
#include "Attributes.h"
#include "Util/UUID.h"
#include "Util/Listenable.h"

namespace BLOCKMAN
{

class ItemStack;
class CombatTracker;
class EntityPlayer;
class PotionEffect;
class Potion;
class RayTraceResult;

class EntityLivingBase : public Entity
{
	RTTI_DECLARE(EntityLivingBase);

public:
	typedef map<int, PotionEffect*>::type ActivePotionsMap;
protected:
	static UUID SPRINTING_SPEED_BOOST_ID;							// field_110156_b = UUID.fromString("662A6B8D-DA3E-4C1C-8813-96EA6097278D");
	static AttributeModifier* SPRINTING_SPEED_BOOST;	// = (new AttributeModifier(field_110156_b, "Sprinting speed boost", 0.30000001192092896D, 2)).func_111168_a(false);
	
	/** data */
	i8		data_Hand_State = 0;
	int		data_Potion_Effects = 0;				// 7
	bool	data_Hide_Particles = 0;				// 8
	int		data_Arrow_Count = 0;					// 9
	Listenable<float>	data_Health{ 1.0f };			// 6

	/** attribute */
	IAttributeMap* attributeMap = nullptr;
	
	CombatTracker* combatTracker = nullptr;
	ActivePotionsMap activePotionsMap;
	/** The equipment this mob was previously wearing, used for syncing. */
	ItemStackPtr previousEquipment[5];
	/** The most recent player that has attacked this entity */
	int m_attackingPlayerEntityId = 0;
	/** Set to 60 when hit by the player or the player's wolf, then decrements. Used to determine whether the entity
	* should drop items on death. */
	int recentlyHit = 0;
	/** Holds the living entity age, used to control the despawn. */
	int entityAge = 0;
	float prevOnGroundSpeedFactor = 0.f;
	float onGroundSpeedFactor = 0.f;
	float movedDistance = 0.f;
	float prevMovedDistance = 0.f;
	float unused180 = 0.f;
	/** The score value of the Mob, the amount of points the mob is worth. */
	int scoreValue = 0;
	float lastDamage = 0.f;
	/** used to check whether entity is jumping. */
	bool isJumping = false;
	float randomYawVelocity = 0.f;
	/** The number of updates over which the new position and rotation are to be applied to the entity. */
	int newPosRotationIncrements = 0;
	/** The new position to be applied to the entity. */
	Vector3 newPos;
	/** The new yaw pitch rotation to be applied to the entity. */
	float newRotationYaw = 0.f;
	float newRotationPitch = 0.f;
	/** Whether the DataWatcher needs to be updated with the active potions */
	bool potionsNeedUpdate = true; // = true;
	EntityLivingBase* lastAttacker = nullptr;
	int lastAttackerTime = 0;
	/** A factor used to determine how far this entity will move each tick if it is walking on land. 
	Adjusted by speed, and slipperiness of the current block. */
	float landMovementFactor = 0.f;
	/** Number of ticks since last jump */
	int jumpTicks = 0;
	float absorptionAmount = 0.f;
	// speed addition
	float m_speedAddition = 0.f;
	// delay ticks to free
	int m_delayTicksToFree = 1;
public:
	/** is the entity Arm is swing or gunfire in fist-person view. */
	bool isHoldGun = false;
	/** Whether an arm swing is currently in progress. */
	bool isSwingInProgress = false;
	int swingProgressInt = 0;
	float swingProgress = 0.f;
	float prevSwingProgress = 0.f;
	/** Whether an arm hold a gun and fire. */
	bool isHoldGunFire = false;
	int gunFireInt = 0;
	float gunFireProgress = 0.f;
	float prevGunFireProgress = 0.f;
	/** whether the gun is reloading? */
	bool m_isReloadBullet = false;
	int m_reloadBulletInt = 0;
	float m_reloadPregress = 0.f;
	float m_prevReloadPregress = 0.f;
	/** whether in defend progress */
	float m_defendProgress = 0.0f;
	
	int arrowHitTimer = 0;
	float prevHealth = 0.f;
	/** The amount of time remaining this entity should act 'hurt'. (Visual appearance of red tint)*/
	int hurtTime = 0;
	/** What the hurt time was max set to last. */
	int maxHurtTime = 0;
	/** The yaw at which this entity was last attacked from. */
	float attackedAtYaw = 0.f;
	/** The amount of time remaining this entity should act 'dead', i.e. have a corpse in the world.*/
	int deathTime = 0;
	int attackTime = 0;
	float prevLimbYaw = 0.f;
	float limbYaw = 0.f;
	/** Only relevant when limbYaw is not 0(the entity is moving). Influences where in its swing legs and arms currently are. */
	float limbSwing = 0.f;
	int maxHurtResistantTime = 10; // = 10;
	float prevCameraPitch = 0.f;
	float cameraPitch = 0.f;
	float randomUnused2 = 0.f;
	float randomUnused1 = 0.f;
	float renderYawOffset = 0.f;
	float prevRenderYawOffset = 0.f;
	/** Entity head rotation yaw */
	float rotationYawHead = 0.f;
	/** Entity head rotation yaw at previous tick */
	float prevRotationYawHead = 0.f;
	/** A factor used to determine how far this entity will move each tick if it is jumping or falling.*/
	float jumpMovementFactor = 0.02f; // = 0.02F;
	float moveStrafing = 0.f;
	float moveForward = 0.f;
	bool m_moving = false;

	/* Set the knock Back Coefficient When a player shoot by gun */
	float m_knockBackCoefficient = 1.0f;

protected:
	void init_LivingBase();
	
	/** implement override functions */
	virtual void entityInit();
	virtual void updateFallState(float y, bool onGroundIn);
	virtual void fall(float distance);
	virtual void kill();
	virtual void setBeenAttacked();

	virtual void applyEntityAttributes();
	/** handles entity death timer, experience orb and particle creation */
	void onDeathUpdate();
	/** Decrements the entity's air supply when underwater */
	int decreaseAirSupply(int air);
	/** Get the experience points the entity currently has. */
	virtual int getExperiencePoints(EntityPlayer* pPlayer) { return 0; }
	/** Only use is to identify if class is an instance of player for experience dropping */
	virtual bool isPlayer() { return false; }
	void updatePotionEffects();
	virtual void updatePotionMetadata();
	virtual void onNewPotionEffect(PotionEffect* pEffect);
	virtual void onChangedPotionEffect(PotionEffect* pEffect, bool apply);
	virtual void onFinishedPotionEffect(PotionEffect* pEffect);
	/** Drop the equipment for this entity. */
	virtual void dropEquipment(bool wasRecentlyHit, int lootingModifier) {}
	/** Returns the sound this mob makes when it is hurt. */
	virtual SoundType getHurtSoundType() { return ST_MalePlayerBeHit;}
	/** Returns the sound this mob makes on death. */
	virtual SoundType getDeathSoundType() { return ST_MalePlayerBeHit;}

	void dropRareDrop(int par1) {}
	/** Drop 0-2 items of this living's type. @param par1 - Whether this entity has recently been hit by a player. @param
	* par2 - Level of Looting used to kill this mob.*/
	virtual void dropFewItems(bool wasRecentlyHit, int lootingModifier) {}
	virtual void damageArmor(float amount) {}
	/** Reduces damage, depending on armor */
	float applyArmorCalculations(DamageSource* pSource, float damage);
	/** Reduces damage, depending on potions */
	float applyPotionDamageCalculations(DamageSource* pSource, float damage);
	/** Deals damage to the entity. If its a EntityPlayer then will take damage from the armor first and then health
	* second with the reduced value. Args: damageAmount */
	virtual void damageEntity(DamageSource* pSource, float damage);
	/*** Returns an integer indicating the end point of the swing animation, used by {@link #swingProgress} to provide a
	* progress indicator. Takes dig speed enchantments into account. */
	int getArmSwingAnimationEnd();
	int getArmHoldingGunAnimEnd();
	/** Updates the arm swing progress counters and animation progress */
	void updateArmSwingProgress();
	/** Returns the volume for the sounds this mob makes.*/
	float getSoundVolume() { return 1.0F; }
	/** Gets the pitch of living sounds in living entities. */
	float getSoundPitch();
	/** Dead and sleeping entities cannot move */
	virtual bool isMovementBlocked() { return getHealth() <= 0.0F; }
	/** Causes this entity to do an upwards motion (jumping). */
	virtual void jump();
	/** Returns true if the newer Entity AI code should be run */
	virtual bool isAIEnabled() { return false; }
	void updateAITasks() {}
	void collideWithNearbyEntities();
	void collideWithEntity(Entity* pEntity) { pEntity->applyEntityCollision(this); }
	/** main AI tick function, replaces updateEntityActionState */
	virtual void updateAITick() {}
	virtual void updateEntityActionState() { ++entityAge; }
	float updateDistance(float yaw, float distance);
	
public:
	static void initialize();
	static void unInitialize();

	EntityLivingBase(World* pWorld);
	virtual ~EntityLivingBase();
	
	void doDestruct();
	void doConstruct();

	/** implement override function. */
	virtual void onEntityUpdate();
	virtual void writeEntityToNBT(NBTTagCompound* pNBT);
	virtual void readEntityFromNBT(NBTTagCompound* pNBT);
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	virtual bool isEntityAlive() { return !isDead && getHealth() > 0.0F; }
	virtual void performHurtAnimation() { hurtTime = maxHurtTime = 10; attackedAtYaw = 0.0f; }
	virtual void setCurrentItemOrArmor(int armorType, ItemStackPtr item) = 0;
	virtual void setSprinting(bool sprinting);
	virtual ItemStackArr getLastActiveItems() { return ItemStackArr(); }
	virtual int getLastActiveItemsCount() { return 0; }
	virtual void onUpdate();
	virtual void updateRidden();
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	virtual Vector3 getLookVec() { return getLook(1.0F); }
	virtual bool canBeCollidedWith() { return !isDead; }
	virtual bool canBePushed() { return !isDead; }
	virtual float getEyeHeight() { return height * 0.85F; }
	virtual float getRotationYawHead() { return rotationYawHead; }
	virtual void setRotationYawHead(float yaw) { rotationYawHead = yaw; }

	void setMoveForward(float speed) { moveForward = speed; }
	void setMoveStrafe(float strafe) { moveStrafing = strafe; }

	bool canBreatheUnderwater() { return false; }
	/** If Animal, checks if the age timer is negative */
	bool isChild() { return false; }
	Random* getRNG() { return rand; }
	EntityLivingBase* getLastAttacker() { return lastAttacker; }
	int getLastAttackerTime() { return lastAttackerTime; }
	void setLastAttacker(Entity* pEntity);
	int getAge() { return entityAge; }
	void clearActivePotions();
	ActivePotionsMap& getActivePotionEffects() { return activePotionsMap; }
	bool isPotionActive(int potionID) { return activePotionsMap.find(potionID) != activePotionsMap.end(); }
	bool isPotionActive(Potion* potion);
	/** returns the PotionEffect for the supplied Potion if it is active, null otherwise. */
	PotionEffect* getActivePotionEffect(Potion* potion);
	/** adds a PotionEffect to the entity */
	void addPotionEffect(PotionEffect* pEffect);
	bool isPotionApplicable(PotionEffect* pEffect);
	/** Returns true if this entity is undead. */
	bool isEntityUndead() { return getCreatureAttribute() == CREATURE_ATTRIBUTE_UNDEAD; }
	/** Remove the speified potion effect from this entity. */
	void removePotionEffectClient(int potionID) { activePotionsMap.erase(potionID); }
	/** Remove the specified potion effect from this entity. */
	void removePotionEffect(int potionID);
	/** Heal living entity (param: amount of half-hearts) */
	void heal(float amount);
	float getHealth() { return data_Health; }
	void setEntityHealth(float amount);
	void setEntityMaxHealth(float maxHealth);
	/** Renders broken item particles using the given ItemStackPtr */
	void renderBrokenItemStack(ItemStackPtr pStack);
	/** Called when the mob's health reaches 0. */
	virtual void onDeath(DamageSource* pSource);
	/** knocks back this entity */
	virtual void knockBack(Entity* pEntity, float strength, float xRatio, float zRatio);
	/** returns true if this entity is by a ladder, false otherwise */
	bool isOnLadder();
	/** Returns the current armor value as determined by a call to InventoryPlayer.getTotalArmorValue */
	virtual int getTotalArmorValue();
	CombatTracker* getCombatTracker() { return combatTracker; }
	EntityLivingBase* getAttackingEntity();
	float getMaxHealth() { return (float)getEntityAttribute(SharedMonsterAttributes::EAMAX_HEALTH)->getAttributeValue(); }
	float getBaseHealth() { return (float)getEntityAttribute(SharedMonsterAttributes::EAMAX_HEALTH)->getBaseValue(); }
	/** counts the amount of arrows stuck in the entity. getting hit by arrows increases this, used in rendering */
	int getArrowCountInEntity() { return data_Arrow_Count; }
	/** sets the amount of arrows stuck in the entity. used for rendering those */
	void setArrowCountInEntity(int arrowCount) { data_Arrow_Count = arrowCount; }
	/** Swings the item the player is holding. */
	virtual void swingItem();
	IAttributeInstance* getEntityAttribute(IAttribute* attr) { return attributeMap->getAttributeInstance(attr); }
	IAttributeMap* getAttributeMap() { 	return attributeMap; }
	/** Get this Entity's EnumCreatureAttribute */
	CREATURE_ATTRIBUTE getCreatureAttribute() { return CREATURE_ATTRIBUTE_UNDEFINED; }
	/** Returns the item that this EntityLiving is holding, if any. */
	virtual ItemStackPtr getHeldItem() = 0;
	/** 0 = item, 1-n is armor */
	virtual ItemStackPtr getCurrentItemOrArmor(int armorType) = 0;
	/** Move the entity to the coordinates informed, but keep yaw/pitch values. */
	void setPositionAndUpdate(const Vector3& pos) { setLocationAndAngles(pos, rotationYaw, rotationPitch); }
	/** Moves the entity to a position out of the way of its mount. */
	void dismountEntity(Entity* pEntity);
	virtual bool getAlwaysRenderNameTagForRender() { return false; }
	/** Moves the entity based on the specified heading.  Args: strafe, forward */
	virtual void moveEntityWithHeading(float strafe, float forward);
	virtual void moveEntity(const Vector3& vel) { Entity::moveEntity(vel); }
	/** the movespeed used for the new AI system */
	virtual float getAIMoveSpeed() { return isAIEnabled() ? landMovementFactor : 0.1F; }
	/** set the movespeed used for the new AI system */
	void setAIMoveSpeed(float speed) { landMovementFactor = speed; }
	virtual bool attackEntityAsMob(Entity* pEntity);
	/** Returns whether player is sleeping or not */
	virtual bool isPlayerSleeping() { return false; }
	/** Called frequently so the entity can update its state every tick as required. For example, zombies and skeletons
	* use this to react to sunlight and start to burn. */
	virtual void onLivingUpdate();
	void setJumping(bool isjump) { isJumping = isjump; }
	/** Called whenever an item is picked up from walking over it. Args: pickedUpEntity, stackSize */
	virtual void onItemPickup(Entity* pickedEntity, int stackSize);
	/** returns true if the entity provided in the argument can be seen. (Raytrace) */
	bool canEntityBeSeen(Entity* pEntity);
	/** interpolated look vector */
	virtual Vector3 getLook(float ticktime);
	/** Returns where in the swing animation the living entity is (from 0 to 1).  Args: partialTickTime */
	float getSwingProgress(float ticktime);
	/** Performs a ray trace for the distance specified and using the partial tick time. Args: distance, partialTickTime */
	RayTraceResult rayTrace(float distance, float ticktime);
	/** Returns whether the entity is in a local (client) world */
	virtual bool isClientWorld();
	virtual float getAbsorptionAmount() { return absorptionAmount; }
	virtual void setAbsorptionAmount(float amount);
	/** Team getTeam() { return null; } */

	virtual void setSpeedAddition(float speedAddition) { m_speedAddition = speedAddition; };
	virtual float getSpeedAddition() { return m_speedAddition; }
	virtual bool isReadyToFree() { return m_delayTicksToFree <= 0; }
	virtual void decrTicksToFree() { --m_delayTicksToFree; }
	void setDelayTicksToFree(int value) { m_delayTicksToFree = value; }
	
private:
	void moveEntityInWater(float strafe, float forward, float dt);
	void moveEntityInLava(float strafe, float forward, float dt);
	void moveEntityWhenParachute(float strafe, float forward, float dt);
	void moveEntityCommon(float strafe, float forward, float dt);
	void moveEntityFly(float strafe, float forward, float dt);
	float getSlipperiness(float& speed_attenuation);
	void handleOnLadderBeforeMove(float dt);
	void handleOnLadderAfterMove();
	void handleEntityDrop(float dt);
};

}
#endif
