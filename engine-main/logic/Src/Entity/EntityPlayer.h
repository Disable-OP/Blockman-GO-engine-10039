/********************************************************************
filename: 	EntityPlayer.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-17
*********************************************************************/
#ifndef __ENTITY_PLAYER_HEADER__
#define __ENTITY_PLAYER_HEADER__

#include "EntityLivingBase.h"

#include "Core.h"
#include "Inventory/Wallet.h"
#include "Inventory/Backpack.h"
#include "Util/AboutShopping.h"
#include "Setting/GunExtraAttrSetting.h"
#include "Entity/EntityFishHook.h"
#include "Network/NetworkDefine.h"
#include "World/WorldEffectManager.h"

using namespace LORD;

namespace BLOCKMAN
{

class InventoryEnderChest;
class InventoryPlayer;
class Container;
class FoodStats;
class ItemFood;
class StatBase;
class Commodity;
class EntityVehicle;
class EntityAircraft;
class Ranch;
class Manor;
class BirdSimulator;

class MovementInput : public ObjectAlloc
{
public:
	/** The speed at which the player is strafing. Postive numbers to the left and negative to the right. */
	float moveStrafe = 0.f;
	/** The speed at which the player is moving forward. Negative numbers will move backwards. */
	float moveForward = 0.f;
	bool jump = false;
	bool sneak = false;
	bool lastSneak = false;
	bool sneakPressed = false;
	bool isSprintInput = false;
	bool down = false;
	virtual void updatePlayerMoveState() {}
};

class PlayerCapabilities
{
protected:
	float flySpeed = 0.05f;// = 0.05F;
	float walkSpeed = 0.1f;// = 0.1F;

public:
	/** Disables player damage. */
	bool disableDamage = false;
	/** Sets/indicates whether the player is flying. */
	bool isFlying = false;
	/** whether or not to allow the player to fly when they double jump. */
	bool allowFlying = false;
	/** Used to determine if creative mode is enabled, and therefore if items should be depleted on usage */
	bool isCreativeMode = false;
	/** Indicates whether the player is allowed to modify the surroundings */
	bool allowEdit = true; // = true;
	/* Watch mode */
	bool isWatchMode = false;
	/* Parachute Status */
	bool isParachute = false;
	/* Frozen time (millisecond) */
	float frozenTime = 0.0f;
	/* Hurt Time*/
	float hurtTime = 0.0f;
	/* Colorful Time*/
	float colorfulTime = 0.0f;
	/* DisableMove Time*/
	float disableMoveTime = 0.0f;


public:
	PlayerCapabilities();

	void writeCapabilitiesToNBT(NBTTagCompound* pNbtCompound);
	void readCapabilitiesFromNBT(NBTTagCompound* pNbtCompound);

	float getFlySpeed() { return flySpeed; }
	void setFlySpeed(float speed) { flySpeed = speed; }
	float getWalkSpeed() { return walkSpeed; }
	void setPlayerWalkSpeed(float speed) { walkSpeed = speed; }
	bool canBeFlying() { return allowFlying || isCreativeMode || isWatchMode; }
	bool isFrozen() { return frozenTime > 0.0f; }
	void onFrozenUpdate(float mills) { frozenTime -= mills; }
	bool isHurted() { return hurtTime > 0.0f; }
	void onHurtUpdate(float mills) { hurtTime -= mills; }
	bool isColorful() { return colorfulTime > 0.0f; }
	void onColorfulUpdate(float mills) { colorfulTime -= mills; }
	bool isDisableMove() { return disableMoveTime > 0.0f; }
	void disableMoveUpdate(float mills) { disableMoveTime -= mills; }

};

enum ActionState
{
	AS_IDLE = 0,
	AS_RUN,
	AS_JUMP_BEGIN,
	AS_JUMP_END,
	AS_SNEAK,		
	AS_WALK,		
	AS_SPRINT,		
	AS_SWIM,		
	AS_FLOAT,
	AS_ON_LADDER_UP,
	AS_ON_LADDER_DOWN,
	AS_ON_LADDER_STATIC,

	
	AS_ATTACK = 1000,
	AS_AIM_BEGIN,
	AS_AIM_END,
	AS_THROW,	
	AS_EAT_BEGIN,
	AS_EAT_END,
	AS_DRINK_BEGIN,
	AS_DRINK_END,
	AS_BLOCK,
	AS_GUN_RELOAD,
	AS_GUN_FIRE,
	AS_GREETINGS, 

	AS_SKILL_START = 2000,
	AS_SKILL_POISION = AS_SKILL_START,

	AS_STATE_TOTAL,
};

class FoodStats : public ObjectAlloc
{
protected:
	/** The player's food level. */
	Listenable<int> foodLevel; // = 20;
	/** The player's food saturation. */
	Listenable<float> foodSaturationLevel; // = 5.0F;
	/** The player's food exhaustion. */
	float foodExhaustionLevel = 0.f;
	/** The player's food timer value. */
	int foodTimer = 0;
	int prevFoodLevel = 20; // = 20;
	
public:
	FoodStats();

	/** Args: int foodLevel, float foodSaturationModifier */
	void addStats(int foodlvl, float foodSatModifier);
	/** Eat some food. */
	void addStats(ItemFood* par1ItemFood);
	/** Handles the food game logic. */
	void onUpdate(EntityPlayer* pPlayer);
	/** Reads food stats from an NBT object. */
	void readNBT(NBTTagCompound* pNbtCompound);
	/** Writes food stats to an NBT object. */
	void writeNBT(NBTTagCompound* pNbtCompound);
	int getFoodLevel() { return foodLevel; }
	Listenable<int>& foodLevelListenable() { return foodLevel; }
	int getPrevFoodLevel() { return prevFoodLevel; }
	bool needFood() { return foodLevel < 20; }
	void addExhaustion(float lvl) { foodExhaustionLevel = Math::Min(foodExhaustionLevel + lvl, 40.0F); }
	float getSaturationLevel() { return foodSaturationLevel; }
	Listenable<float>& saturationLevelListenable() { return foodSaturationLevel; }
	void setFoodLevel(int lvl) { foodLevel = lvl; }
	void setFoodSaturationLevel(float lvl) { foodSaturationLevel = lvl; }
};

enum CustomEffectStatus
{
	Create = 1,
	Running,
	Unload,
	Delete
};

class CustomEffect
{
public:
	String m_name = "";
	String m_effect_name = "";
	float m_duration = 0.0f;
	CustomEffectStatus m_status = CustomEffectStatus::Create;
	SimpleEffect* m_effect = NULL;
	DEFINE_NETWORK_SERIALIZER(m_name, m_effect_name, m_duration);
};

class EntityPlayer : public EntityLivingBase
{
	RTTI_DECLARE(EntityPlayer);

protected:
	Vector3 oldPos;
	float oldMinY = 0.f;
	int m_moveTick = 0;

	InventoryEnderChest* theInventoryEnderChest = nullptr; // = new InventoryEnderChest();
	 /** The player's food stats. (See class FoodStats) */
	FoodStats* foodStats = nullptr;	// = new FoodStats();
	/** Used to tell if the player pressed jump twice. If this is at 0 and it's pressed (And they are allowed to fly, as
	* defined in the player's movementInput) it sets this to 7. If it's pressed and it's greater than 0 enable fly.	*/
	int flyToggleTimer = 0;
	String username = "noPlayerUserName";
	String showName = "noPlayerShowName";
	/** Boolean value indicating weather a player is sleeping or not */
	bool sleeping = false;
	int sleepTimer = 0;
	/** Holds the last coordinate to spawn based on last bed that the player sleep. */
	Vector3i spawnChunk;
	/** Whether this player's spawn point is forced, preventing execution of bed checks. */
	bool spawnForced = false;
	/** Holds the coordinate of the player when enter a blockman to ride. */
	Vector3i startMinecartRidingCoordinate;

	/** This field starts off equal to getMaxItemUseDuration and is decremented on each tick */
	int itemInUseCount = 0;
	float speedOnGround = 0.1f; // = 0.1F;
	float speedInAir = 0.02f; // = 0.02F;
	int lastXPSound = 0;

	int teamId = 0;
	String teamName = "noPlayerTeamName";
	int m_occupation = 0;

	/** driver vehicle */
	int m_vehicleEntityId = 0;
	bool m_isDriver = false;
	std::set<int> m_ownVehicle;
	int m_aircraftEntityId = 0;

	/** data */
	i8		data_hide_caps = 0;
	float	data_absorption = 0.f;
	int		data_score = 0;
	Vector3i m_activatedBlock;

	ActionState m_prevBaseAction;
	ActionState m_prevUpperAction;
	Listenable<ActionState> m_baseAction;
	Listenable<ActionState> m_upperAction;

	std::vector<std::vector<int>> m_unlockedCommodities;

public:
	ui64 platformUserId = 0;

	/** Inventory of the player */
	InventoryPlayer* inventory = nullptr;		// = new InventoryPlayer(this);
	/** The Container for the player's inventory (which opens when they press E) */
	Container* inventoryContainer = nullptr;
	/** The Container the player has open. */
	Container* openContainer = nullptr;
	// wallet
	Wallet* m_wallet = nullptr;
	// backpack
	Backpack* m_backpack = nullptr;
	// GunExtraAttrMap
	GunExtraAttrMap* m_gunExtraAttrMap = nullptr;

	//Manor
	Manor* m_manor = nullptr;

	//Ranch
	Ranch* m_ranch = nullptr;

	//BirdSimulator
	BirdSimulator* m_birdSimulator = nullptr;

	/* for record player input */
	MovementInput* movementInput = nullptr;

	float prevCameraYaw = 0.f;
	float cameraYaw = 0.f;
	bool spYaw = false;
	float spYawRadian = 0.f;

	/** Used by EntityPlayer to prevent too many xp orbs from getting absorbed at once. */
	int xpCooldown = 0;
	float prevChasingPosX = 0.f;
	float prevChasingPosY = 0.f;
	float prevChasingPosZ = 0.f;
	float chasingPosX = 0.f;
	float chasingPosY = 0.f;
	float chasingPosZ = 0.f;

	/** The chunk coordinates of the bed the player is in (null if player isn't in a bed). */
	BlockPos bedLocation;
	Vector3 renderBedOffset;

	/** The player's capabilities. (See class PlayerCapabilities) */
	PlayerCapabilities capabilities; //  = new PlayerCapabilities();

	/** The current experience level the player is on. */
	int experienceLevel = 0;
	/** The total amount of experience the player has. This also includes the amount of experience within their Experience Bar. */
	int experienceTotal = 0;
	/** The current amount of experience the player has within their Experience Bar. */
	float experience = 0.f;
	/** This is the item that is in use when the player is holding down the useItemButton (e.g., bow, food, sword) */
	ItemStackPtr itemInUse = nullptr;
	/** is entity outlooks changed? */
	bool m_outLooksChanged = true;
	bool m_handMeshChanged = true;
	bool m_isPeopleActor = true;

	bool m_isClothesChange = false; // change
	bool m_isClothesChanged = false; // changed -- clothes hint
	bool m_needUpdateUpperAction = false;
	bool m_isActorInvisible = false;
	bool m_isNamePerspective = false;

	String m_actorBodyId = "";  // differs from each other when use the same m_actorName
	String m_actorName = "";
	String m_lastHandMesh = "";
	int m_faceID = 1;				// custom user define the face model
	int m_hairID = 1;				// custom user define the hair model
	int m_topsID = 1;				// user clothes tops id. 0 is not exist
	int m_pantsID = 1;				// user clothes pants id. 0 is not exist
	int m_shoesID = 1;				// user clothes shoes id. 0 is not exist
	int m_glassesId = 0;
	int m_scarfId = 0;
	int m_wingId = 0;
	int m_hatId = 0;
	int m_decorate_hatId = 0;
	int m_armId = 0;
	int m_tailId = 0;
	int m_extra_wingId = 0;
	int m_footHaloId = 0;
	int m_crownId = 0;

	int m_bagId = 0;

	int m_sex = 1;					// 1: boy, 2: girl
	bool m_showClothes = false;		// custom user define show the clothes or show the armor(if armor parts is exist)
	Color m_skinColor;				// skin color.
	int m_team = 0;                 // teamId
	int m_backEffectId = 0;
	int m_headEffectId = 0;
	/** An instance of a fishing rod's hook. If this isn't null, the icon image of the fishing rod is slightly different */
	EntityFishHook* m_fishEntity = nullptr;
	/** need to set enable movement when freeze over */
	bool m_setEnableMovement = false;
	bool m_hasShotLaserGun = false;

	Vector3 m_thirdPersonCameraOffset = Vector3::ZERO;
	bool m_isUsingItemByBtn = false;

	int m_defaultIdle = 0;

	float m_opacity = 1.0f;

	vector<CustomEffect>::type m_custom_effects;
	bool m_isBagChange = false;
	bool m_isBirdConvert = false;
	int m_bagCapacity = 0;
	int m_bagMaxCapacity = 1;

protected:
	bool m_logout = false;
	bool m_isPlayingSkill = false;
	int m_PlayingSkillTime = 0;

private:
	bool m_isThrowingSnowball = false;
	bool m_isPlacingBlock = false;
	int m_movementTimes = 0;

public:
	bool isThrowingSnowball() { return m_isThrowingSnowball; }
	void setThrowingSnowball(bool throwing) { m_isThrowingSnowball = throwing; }
	bool isPlacingBlock() { return m_isPlacingBlock; }
	void setPlacingBlock(bool placing) { m_isPlacingBlock = placing; }

public:
	bool isReadyToFree() { return m_logout && m_delayTicksToFree <= 0; }
	void decrTicksToFree() {
		if (m_logout) {
			--m_delayTicksToFree;
		}
	}

	bool isLogout() { return m_logout; }
	void setLogout(bool value) { m_logout = value; }

	int getTeamId() { return teamId; }
	void setTeamId(int teamId) { this->teamId = teamId; }

	String getTeamName() { return teamName; }
	void setTeamName(String teamName) { this->teamName = teamName; }

	ui64 getPlatformUserId() { return platformUserId; }
	void setPlatformUserId(ui64 userId) { this->platformUserId = userId; }

	bool isActorInvisible() { return m_isActorInvisible; }
	void setActorInvisible(bool invisible) { m_isActorInvisible = invisible; }
	
	bool isNamePerspective() { return m_isNamePerspective; }
	void setNamePerspective(bool perspective) { m_isNamePerspective = perspective; }

	virtual void addGunDamage(int gunId, float damage);
	virtual void addGunBulletNum(int gunId, int bulletNum);
	virtual void subGunRecoil(int gunId, float recoil);
	virtual void subGunFireCd(int gunId, float fireCd);

	float getGunDamage(const GunSetting* setting);
	int getGunBulletNum(const GunSetting* setting);
	float getGunRecoil(const GunSetting* setting);
	float getGunFireCd(const GunSetting* setting);

protected:

	/** implement override functions from EntityLivingBase */
	virtual void applyEntityAttributes();
	virtual void entityInit();
	virtual bool isMovementBlocked();
	virtual void updateEntityActionState();
	virtual void damageArmor(float amount);
	virtual void damageEntity(DamageSource* pSource, float amount);
	virtual void jump();
	virtual void moveEntity(const Vector3& vel);
	/** Takes in the distance the entity has fallen this tick and whether its on the ground to update the fall distance
	and deal fall damage if landing on the ground.  Args: distanceFallenThisTick, onGround */
	virtual void updateFallState(float y, bool onGroundIn);
	virtual void fall(float distance);
	virtual int getExperiencePoints(EntityPlayer* pPlayer);
	virtual bool isPlayer() { return true; }
	virtual bool canTriggerWalking() { return !capabilities.isFlying; }
	virtual void onCapabilitieUpdate();

	void collideWithPlayer(Entity* pEntity);
	/** Plays sounds and makes particles for item in use state */
	void updateItemUse(ItemStackPtr pStack, int amount);
	/** Used for when item use count runs out, ie: eating completed */
	virtual void onItemUseFinish();
	/** Joins the passed in entity item with the world. Args: entityItem */
	virtual void joinEntityItemWithWorld(EntityItem* pItem);
	/** sets the players height back to normal after doing things like sleeping and dieing */
	virtual void resetHeight() { yOffset = 1.62F; }
	void setRenderOffsetForSleep(int dir);
	/** Checks if the player is currently in a bed */
	bool isInBed();
	bool getHideCape(int flag) { return (data_hide_caps & (1 << flag)) != 0; }
	void setHideCape(int flag, bool cape);
	/** Adds a value to a mounted movement statistic field - by blockman, boat, or pig. */
	void addMountedMovementStat(const Vector3& pos);
	/** update the player's anim-state.*/
	void updatePlayerAnimState();
	virtual SoundType getHurtSoundType();

	Vector3 getForward();

	void clearCustomEffects();
	virtual void updateCustomEffects();

public:
	EntityPlayer(World* pWorld, const String& name);
	virtual ~EntityPlayer();

	bool canBuy(const Commodity& commodity, BuyResult& buyResult);
	bool canGet(const Commodity& commodity, BuyResult& buyResult);
	bool canUnlock(const Commodity& commodity, BuyResult& buyResult);
	void setFire(int second) override;
	void doDestruct();
	void doConstruct();
	bool canAddBulletItem(int itemId);

	virtual SoundType getDeathSoundType();

	/** returns the ItemStack containing the itemInUse */
	ItemStackPtr getItemInUse() { return itemInUse; }
	/** Returns the item in use count */
	int getItemInUseCount() { return itemInUseCount; }
	/** Checks if the entity is currently using an item (e.g., bow, food, sword) by holding down the useItemButton */
	bool isUsingItem() { return itemInUse != NULL; }
	/** gets the duration for how long the current itemInUse has been in use */
	int getItemInUseDuration();
	void stopUsingItem();
	void clearItemInUse();
	bool isBlocking();
	int getScore() { return data_score; }
	void setScore(int s) { data_score = s; }
	void addScore(int s) { data_score += s; }

	/** vehicle */
	bool isDriver() const { return m_vehicleEntityId != 0 && m_isDriver == true; }
	bool isOnVehicle() const { return m_vehicleEntityId != 0; }
	EntityVehicle* getVehicle();
	virtual void askTakeOnVehicle(EntityVehicle* vehicle) {}
	virtual void askTakeOffVehicle() {}
	bool takeOnVehicle(EntityVehicle* vehicle, bool bForce, bool bIsDriver);
	virtual bool takeOffVehicle();
	Vector3 getLeaveVehiclePos();
	bool tryLockVehicle(bool state);
	virtual void addOwnVehicle(int vehicleId);
	virtual bool hasOwnVehicle(int vehicleId);
	virtual void resetOwnvehicle(const vector<int>::type& vehicles);
	virtual void syncOwnVehicle() {};
	virtual void leaveVehicle() {};

	/* aircraft */
	EntityAircraft* getAircraft();
	bool isOnAircraft() const { return m_aircraftEntityId != 0; }
	virtual bool takeOnAircraft(EntityAircraft* pAircraft);
	virtual bool takeOffAircraft();

	/*parachute*/
	virtual bool startParachute();
	virtual bool parachuteEnd();

	/* backpack */
	Backpack* getBackpack() { return m_backpack; }
	virtual void addBackpackCapacity(int capacity);
	virtual void subBackpackCapacity(int capacity);
	virtual void resetBackpack(int capacity, int maxCapacity);

	/** actor npc */
	virtual void attackActorNpc(Entity* entity, PLAYER_ATTACK_TYPE damageType) {}

	virtual void attackEntity(Entity* entity) {};

	/** attack EntityCreature*/
	virtual void attackCreatureEntity(EntityCreature* entity) {}

	/*Camera lock*/
	bool m_isCameraLocked = false;
	bool m_isNeedChangePostion = false;
	Vector3 m_cameraLockPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_prevCameraLockPos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_originCameraLockPos;
	float   m_cameraLockYaw = 0.0f;
	float   m_cameraLockPitch = 0.0f;

	int autoJumpTime = 0;

	/*skill animation*/
	bool setSkillAnimation(ActionState actionState);
	void stopSkillAnimation() { m_isPlayingSkill = false; m_PlayingSkillTime = 0; }
	void setSkillAnimationTime(int duration);
	void updateSkillAnimationTime();

	/** Called when player presses the drop item key */
	virtual EntityItem* dropOneItem(bool par1);
	/** called when player drops an item stack that's not in his inventory (like items still placed in a workbench while the workbench'es GUI gets closed) */
	EntityItem* dropPlayerItem(ItemStackPtr pStack) { return dropPlayerItemWithRandomChoice(pStack, false); }
	EntityItem* dropPlayerItemWithRandomChoice(ItemStackPtr pStack, bool fromPlayer);
	/** Returns how strong the player is against the specified block at this moment */
	float getCurrentPlayerStrVsBlock(Block* pBlock, bool par2);
	/** Checks if the player has the ability to harvest a block (checks current inventory item for a tool if necessary) */
	bool canHarvestBlock(Block* pBlock);
	bool interactWith(Entity* pEntity);
	/** Returns the currently being used item by the player. */
	ItemStackPtr getCurrentEquippedItem();
	bool canAttackPlayer(EntityPlayer* pPlayer);
	float getArmorVisibility();
	/** Destroys the currently equipped item from the player's inventory. */
	void destroyCurrentEquippedItem();
	bool simpleAttackPlayerWithCurrentItem(EntityPlayer * pTargetPlayer);
	/** Attacks for the player the targeted entity with the currently equipped item.  The equipped item has hitEntity called on it. */
	bool attackTargetEntityWithCurrentItem(Entity* pTarget);
	/** Called when the player performs a critical hit on the Entity. Args: entity that was hit critically */
	virtual void onCriticalHit(Entity* pEntity) {}
	virtual void onEnchantmentCritical(Entity* pEntity) {}
	void respawnPlayer() {}
	/** Attempts to have the player sleep in a bed at the specified location. */
	virtual SLEEP_RESULT sleepInBedAt(const BlockPos& pos);
	/** Wake up the player if they're sleeping. */
	virtual void wakeUpPlayer(bool immediately, bool updateWorld, bool setSpawn);
	/*** Ensure that a block enabling respawning exists at the specified coordinates and find an empty space nearby to spawn. */
	static Vector3i verifyRespawnCoordinates(World* pWorld, const Vector3i& respawnPos, bool par2);
	/** Returns the orientation of the bed in degrees. */
	float getBedOrientationInDegrees();
	bool isPlayerFullyAsleep() { return sleeping && sleepTimer >= 100; }
	int getSleepTimer() { return sleepTimer; }
	/** Add a chat message to the player */
	virtual void addChatMessage(const String& par1Str) {}
	const Vector3i& getBedLocation() { return spawnChunk; }
	bool isSpawnForced() { return spawnForced; }
	void setSpawnChunk(const Vector3i& chunkCoord, bool spawn);
	/** Will trigger the specified trigger. */
	void triggerAchievement(StatBase* pStat) { addStat(pStat, 1); }
	/** Adds a value to a statistic field. */
	virtual void addStat(StatBase* pStat, int amount) {}
	/*** Adds a value to a movement statistic field - like run, walk, swin or climb. */
	void addMovementStat(const Vector3& pos);
	ItemStackPtr getCurrentArmor(int slot);
	/** This method increases the player's current amount of experience.*/
	void addExperience(int exp);
	/** Add experience levels to this player. */
	virtual void addExperienceLevel(int lvl);
	/** This method returns the cap amount of experience that the experience bar can hold. */
	int xpBarCap();
	/** increases exhaustion level by supplied amount */
	void addExhaustion(float amount);
	/** Returns the player's FoodStats object.*/
	FoodStats* getFoodStats() { return foodStats; }
	bool canEat(bool checkFood);
	/** Checks if the player's health is not full and not zero. */
	bool shouldHeal() { return getHealth() > 0.0F && getHealth() < getMaxHealth(); }
	/** sets the itemInUse when the use item button is clicked. */
	virtual void setItemInUse(ItemStackPtr pStack, int duration);
	/** Returns true if the given block can be mined with the current tool in adventure mode. */
	bool isCurrentToolAdventureModeExempt(const BlockPos& pos);
	bool canPlayerEdit(const BlockPos& pos, int face, ItemStackPtr pStack);
	/** Copies the values from the given player into this player if boolean par2 is true. Always clones Ender Chest Inventory. */
	void clonePlayer(EntityPlayer* pPlayer, bool all);
	/** Sends the player's abilities to the server (if there is one). */
	virtual void sendPlayerAbilities() {}
	/** Sets the player's game mode and sends it to them. */
	virtual void setGameType(GAME_TYPE gametype) {}
	/** Gets the name of this command sender (usually username, but possibly "Rcon") */
	String getCommandSenderName() { return username; }
	String getShowName() { return showName.length() > 0 ? showName : username; }
	void setShowName(String name) { showName = name; }
	World* getWorldPtr() { return world; }
	/** Returns the InventoryEnderChest of this player. */
	InventoryEnderChest* getInventoryEnderChest() { return theInventoryEnderChest; }
	bool getHideCape() { return getHideCape(1); }

	bool canPlayAction();
	ActionState getPrevBaseAction() { return m_prevBaseAction; }
	ActionState getPrevUpperAction() { return m_prevUpperAction; }
	ActionState getBaseAction() { return m_baseAction; }
	void setBaseAction(ActionState as) { m_prevBaseAction = m_baseAction; m_baseAction = as; }
	ActionState getUpperAction() { return m_upperAction; }
	void setUpperAction(ActionState as) { m_prevUpperAction = m_upperAction; m_upperAction = as; }
	void refreshBaseAction() { m_prevBaseAction = m_baseAction; }
	void refreshUpperAction() { m_prevUpperAction = m_upperAction; }

	bool shootTargetByGun(EntityPlayer* pTarget, const C2SPACKET_DETAIL::Vector3& hitpos,
		bool isHeadshot, const GunSetting* pGunSetting);
	bool consumeBullet();
	void reloadBullet();
	int getHeldItemId();
	virtual void notifyClientReloadBullet() {}
	virtual void markInventoryDirty() {}
	virtual void sendGetBulletSound() {}

	float getYaw() { return rotationYaw; }
	int getSex() { return m_sex; }

	// todo. Scoreboard getWorldScoreboard();

	// todo. String getTranslatedEntityName()

	void syncPlayerMovement(const Vector3& pos, float yaw, float pitch, float swing, float yawhead, float limbyaw);

	/** implement override functions from EntityLivingBase */
	virtual void onUpdate();
	virtual void playSound(SoundType soundType);
	virtual void mountEntity(Entity* pEntity);
	virtual void updateRidden();
	virtual void preparePlayerToSpawn();
	virtual void onLivingUpdate();
	virtual void onDeath(DamageSource* pSource);
	virtual void addToPlayerScore(Entity* pEntity, int scoreToAdd);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeEntityToNBT(NBTTagCompound* pNbtCompound);
	bool attckedByPlayer(EntityPlayer * pEntity, float amount);
	virtual float getEyeHeight() { return 0.12F; }
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	virtual int getTotalArmorValue();
	virtual float getYOffset() { return yOffset - 0.5f; }
	virtual void setDead();
	virtual void whenRemoved();
	virtual bool isEntityInsideOpaqueBlock();
	virtual bool isPlayerSleeping() { return sleeping; }
	virtual void moveEntityWithHeading(float strafe, float forward);
	virtual float getAIMoveSpeed();
	virtual void onKillEntity(EntityLivingBase* pLiving);
	virtual void setInWeb();
	virtual String getEntityName() { return username; }
	virtual bool getAlwaysRenderNameTagForRender() { return true; }
	virtual ItemStackPtr getCurrentItemOrArmor(int armorType);
	virtual ItemStackPtr getHeldItem();
	virtual void setCurrentItemOrArmor(int armorType, ItemStackPtr pArmor);
	virtual ItemStackArr getLastActiveItems();
	virtual bool isInvisibleToPlayer(EntityPlayer* pPlayer);
	virtual bool isPushedByWater() { return !capabilities.isFlying; }
	virtual void setAbsorptionAmount(float amount);
	virtual float getAbsorptionAmount() { return data_absorption; }
	virtual void changeMaxHealth(float health);
	Vector3i getActivatedBlock() const { return m_activatedBlock; }
	void setActivatedBlock(const Vector3i& pos) { m_activatedBlock = pos; }
	virtual void setAllowFlying(bool allow);
	virtual bool isWatchMode() { return capabilities.isWatchMode; }
	virtual void setWatchMode(bool isWatchMode);
	virtual bool setCameraLocked(bool bLocked);
	void changeClothes(String partName, int actorId);
	bool isParachute() { return capabilities.isParachute; }

	virtual bool isFrozen() { return capabilities.isFrozen(); }
	virtual void setOnFrozen(float mills);
	virtual float getFrozenTime() { return capabilities.frozenTime; }
	virtual bool isHurted() { return capabilities.isHurted(); }
	virtual void setOnHurt(float mills) { capabilities.hurtTime = mills; }
	virtual float getHurtTime() { return capabilities.hurtTime; }
	virtual bool isColorful() { return capabilities.isColorful(); }
	virtual void setOnColorful(float mills);
	virtual float getColorfulTime() { return capabilities.colorfulTime; }
	virtual bool isDisableMove() { return capabilities.isDisableMove(); }
	virtual void setDisableMove(float mills);
	virtual float getDisableMoveTime() { return capabilities.disableMoveTime; }

	void setOnFire(bool onFire) {
		setFlag(ENTITY_STATUS_BURNING, onFire);
	}
	/** Return whether this entity should NOT trigger a pressure plate or a tripwire.*/
	virtual bool doesEntityNotTriggerPressurePlate() { return capabilities.isWatchMode; }
	virtual bool canPickupItem(Entity* pEntity, int size) { return false; }
	/** GUI interface. */
	// todo.
	/** sets current screen to null (used on escape buttons of GUIs) */
	virtual void closeScreen() { openContainer = inventoryContainer; }
	virtual bool isZombie() { return false; }
	virtual void setSpeedAdditionLevel(int level);
	virtual bool canBeCollidedWith() override { return false; }
	//virtual void releaseSkillAction(ActionState  actionState, float  skillActionDuration);
	//virtual void displayGUIChest(IInventory* pInventory) {}
	//virtual void displayGUIHopper(TileEntityHopper* pHopper) {}
	//virtual void displayGUIHopperMinecart(EntityMinecartHopper* pMCHopper) {}
	//virtual void func_110298_a(EntityHorse* pHorse, IInventory* pInventory) {}
	//virtual void displayGUIEnchantment(int par1, int par2, int par3, const String& par4Str) {}
	//virtual void displayGUIAnvil(int par1, int par2, int par3) {}
	//virtual void displayGUIWorkbench(int par1, int par2, int par3) {}
	//virtual void displayGUIFurnace(TileEntityFurnace* pFurnace) {}
	//virtual void displayGUIDispenser(TileEntityDispenser* pDispenser) {}
	//virtual void displayGUIEditSign(TileEntity* pTileEntity) {}
	//virtual void displayGUIBrewingStand(TileEntityBrewingStand* pBrewingStand) {}
	//virtual void displayGUIBeacon(TileEntityBeacon* pBeacon) {}
	//virtual void displayGUIMerchant(IMerchant* pMerchant, const String& name) {}
	//virtual void displayGUIBook(ItemStackPtr pStack) {}
	int updateTick = 0;

	void addUnlockedCommodity(int unlockedId, int meta);
	bool isUnlocked(int itemId, int meta);
	virtual int onEnchantmentModifier(Entity * target, int  enchantmentModifier) { return enchantmentModifier; };
	virtual void addCustomEffect(String name, String effectName, float duration);

protected:

public:
	void applyVehicleMovementInput();
	void applyMovementInput();
	void setMovementInput(float forward, float strafe);
	void updateAutoJump(float deltaX, float deltaZ);
	void setIsJump(bool isJump);
	void setIsDown(bool isDown);

	void movementTimes() { m_movementTimes++; }
	virtual bool canMovement() { return m_movementTimes < 2; }
	virtual void enableMovement() { m_movementTimes = 0; }
	virtual bool isPlayerMoved();
	virtual void recordOldPos() {}
	virtual void setOccupation(int occupation) { m_occupation = occupation; }
	virtual int getOccupation() { return m_occupation; }
	virtual void setBagInfo(int curCapacity, int maxCapacity);
	virtual void setBirdConvert(bool isConvert) { m_isBirdConvert = isConvert; }
};
}

#endif
