/********************************************************************
filename: 	EntityPlayerMP.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-07-10
*********************************************************************/
#ifndef __ENTITY_PLAYER_MP_HEADER__
#define __ENTITY_PLAYER_MP_HEADER__

#include "Entity/EntityPlayer.h"
#include "Inventory/Crafting.h"
#include "Item/Potion.h"
#include "Util/SubscriptionGuard.h"
#include "Util/AboutShopping.h"
#include "TileEntity/TileEntityInventory.h"
#include <vector>
#include "SynBlockStrategy.h"

namespace BLOCKMAN
{

class Container;
class ItemStack;
class ChatMessageComponent;
class TileEntityChest;

struct ServerMovementInput 
{
	float moveForward = 0.0f;
	float moveStrafe = 0.0f;
};

class EntityPlayerMP : public EntityPlayer ,public ICrafting
{
	RTTI_DECLARE(EntityPlayerMP);
private:
	SubscriptionGuard m_subscriptionGuard;
	TileEntityChest* m_openChest = nullptr;
public:
	/** player X position as seen by PlayerManager */
	float managedPosX = 0.0f;

	/** player Z position as seen by PlayerManager */
	float managedPosZ = 0.0f;

	/** LinkedList that holds the loaded chunks. */
	// List loadedChunks = new LinkedList();

	/** entities added to this list will  be packet29'd to the player */
	// List destroyedItemsNetCache = new LinkedList();
	
	/** poor mans concurency flag, lets hope the jvm doesn't re-order the setting of this flag wrt the inventory change on the next line */
	bool playerInventoryBeingManipulated = false;
	int ping = 0;

	/** Set when a player beats the ender dragon, used to respawn the player at the spawn point while retaining inventory and XP */
	bool playerConqueredTheEnd = false;

protected:
	Vector3 oldPos;
	float oldMinY = 0.f;

	/** de-increments onUpdate, attackEntityFrom is ignored if this >0 */
	int initialInvulnerability = 60;	// = 60;

	/** must be between 3>x>15 (strictly between) */
	int chatVisibility = 0;
	bool chatColours = true; // = true;

	/** The currently in use window ID. Incremented every time a window is opened. */
	int currentWindowId = 0;

	/** the ranksid */
	ui64 m_raknetID = 0;

	/** identity whether the player inventory changed */
	bool m_inventoryChanged = true;

	/** identity whether the player property changed */
	bool m_propertyChanged = true;
	bool m_itemInHandChanged = true;

	bool m_lastTickIsFlying = false;

	AttributeModifierMap m_attackModifiers;

	SyncBlockContext *mSynBlockContext = nullptr;

	int m_movePlayerUpdateTick = 0;

	bool m_bNeedSyncPosition = false;

	bool m_isForceTeleport = false;

public:
	EntityPlayerMP(World* pWorld, const String& worldName, const Vector3i& spawnPos);
	virtual ~EntityPlayerMP();
	void doConstruct(const Vector3i& spawnPos);
	void doDestruct();

	void reconstruct(const Vector3i& spawnPos);

	void addMoveSpeedPotionEffect(int seconds);
	void addJumpPotionEffect(int seconds);
	void addNightVisionPotionEffect(int seconds);
	void addPoisonPotionEffect(int seconds);
	void addEffect(int id, int seconds, int amplifier);
	void removeEffect(int id);
	void clearEffects();
	virtual void setDead();
	void leaveVehicle();
	void specialJump(float yFactor = 0.0f, float xFactor = 1.0f, float zFactor = 1.0f);

	/** implement override functions from EntityLivingBase */
	virtual void updatePotionMetadata();
	virtual void readEntityFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeEntityToNBT(NBTTagCompound* pNbtCompound);
	virtual float getEyeHeight() { return 1.62F; }
	virtual void onUpdate();
	virtual void triggerEntityUpdate();
	virtual void onDeath(DamageSource* pSource);
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	virtual bool canAttackPlayer(EntityPlayer* pPlayer);
	virtual void onItemPickup(Entity* pEntity, int size);
	virtual void mountEntity(Entity* pEntity);
	virtual void closeScreen();
	virtual void addStat(StatBase* stat, int amount);
	virtual void addChatMessage(const String& msg);
	virtual void setPositionAndUpdate(float x, float y, float z);
	virtual bool setToLastPosition(float x, float y, float z);
	virtual void onCriticalHit(Entity* pEntity);
	virtual void jump() override;
	virtual void changeCurrentItem(int slot);
	virtual void moveEntity(const Vector3& vel) override;
	virtual void updateEntityActionState() override;
	
	/** implement override functions from EntityPlayer */
	virtual void addExperienceLevel(int exp);
	virtual SLEEP_RESULT sleepInBedAt(const BlockPos& pos);
	virtual void wakeUpPlayer(bool immediately, bool update, bool setSpawn);
	virtual void setItemInUse(ItemStackPtr pStack, int duration);
	virtual void clonePlayer(EntityPlayer* pPlayer, bool all);
	virtual void onEnchantmentCritical(Entity* pEntity);
	virtual void sendPlayerAbilities();
	virtual void setGameType(GAME_TYPE type);
	virtual void notifyClientReloadBullet();
	virtual void sendGetBulletSound();
	virtual bool takeOnAircraft(EntityAircraft* pAircraft);
	virtual bool takeOffAircraft();
	virtual bool takeOffVehicle() override;

	virtual void setOpenChest(TileEntityChest* chest);
	virtual void closeOpenChest();

	virtual void knockBack(Entity* pEntity, float strength, float xRatio, float zRatio);

	/** implement override functions from ICrafting */
	virtual void sendSlotContents(Container* pContainer, int par2, ItemStackPtr pStack);
	virtual void sendContainerAndContentsToPlayer(Container* pContainer, ItemStackArr& stacks);
	virtual void sendProgressBarUpdate(Container* pContainer, int toUpdate, int val);

	void addSelfToInternalCraftingInventory();

	/** Teleports the entity to another dimension. Params: Dimension number to teleport to */
	void travelToDimension(int dimension);

	/** likeUpdateFallState, but called from updateFlyingState, rather than moveEntity */
	void updateFlyingState(float y, bool onGroundIn) { EntityPlayer::updateFallState(y, onGroundIn); }

	void sendContainerToPlayer(Container* pContainer);
	
	/*** updates item held by mouse */
	void updateHeldItem();

	/** Closes the container the player currently has open. */
	void closeContainer();

	void func_110430_a(float par1, float par2, bool par3, bool par4);
	
	void mountEntityAndWakeUp();

	void sendChatToPlayer(ChatMessageComponent* chatComponent);
	
	/** Returns true if the command sender is allowed to use the given command. */
	bool canCommandSenderUseCommand(int par1, const String& par2Str);
	
	/** Gets the player's IP address. Used in /banip. */
	String getPlayerIP();

	// todo.
	// void updateClientInfo(Packet204ClientInfo par1Packet204ClientInfo)
	
	int getChatVisibility() { return chatVisibility; }

	/** on recieving this message the client (if permission is given) will download the requested textures */
	void requestTexturePackLoad(const String& par1Str, int par2);
	
	/** Return the position for this command sender. */
	BlockPos getPlayerCoordinates();

	void setRaknetID(ui64 id) { m_raknetID = id; }
	ui64 getRaknetID() const { return m_raknetID; }

	bool useItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos, int param0);
	
	void addItem(int id, int num, int damage, bool isReverse = false, int maxDamage = 0);
	void replaceItem(int id, int num, int damage, int stackIndex, int clipBullet = 0);
	void addGunItem(int id, int num, int damage, int clipBullet, bool isReverse = false);

	void addItemToEnderChest(int slot, int id, int num, int damage);
	void addGunItemToEnderChest(int slot, int id, int num, int damage, int clipBullet);

	void addEchantmentItem(int id, int num, int damage, std::vector<std::vector<int>> enchantments);
	void addItemPotion(int index, int num);
	void buy(const Commodity& commodity, bool isConsumeCoin, bool isAddGoods);

	void setAllowFlying(bool allowFlying) override;
	void setWatchMode(bool isWatchMode) override;
	void changeMaxHealth(float health);
	void equipArmor(int id, int damage);
	bool setCameraLocked(bool bLocked);
	void changeInvisible(ui64 rakssId, bool isVisible);
	void changeNamePerspective(ui64 rakssId, bool isPerspective);
	
	void changeHeart(int hp, int maxHp);
	void showBossStripWithIcon(String name, int curProgress, int maxProgress, String iconPath);
	void changeDefense(int defense, int maxDefense);
	void changeAttack(int attack, int maxAttack);

	void setPersonalShopArea(Vector3 startPos, Vector3 endPos);
	void changeSwitchableProps(String data);
	void changeUpgradeProps(String data);
	void changeEnchantmentProps(String data);
	void changeSuperProps(String data);
	void changeCustomProps(String data);
	void updateCustomProps(String data);

	void initUpdateBlocks();
	void sendUpdateBlocks();
	void forceBrocastPosition();
	void addUpdateBlocks(std::vector<BlockPos>& blocks);
	void addSingleUpdateBlock(BlockPos pos, int block_id, int meta);

	InventoryPlayer* getInventory() { return inventory; }
	void markInventoryDirty()
	{
		m_inventoryChanged = true;
	}
	bool canPickupItem(Entity* pEntity, int size);
	void setCurrency(i64 currency);
	void addCurrency(i32 currency);
	void subCurrency(i32 currency);
	i64 getCurrency();
	void resetClothes(ui64 userId);
	bool tryBuyVehicle(int vehicleId);
	virtual void syncOwnVehicle();

	void addBackpackCapacity(int capacity);
	void subBackpackCapacity(int capacity);
	void resetBackpack(int capacity, int maxCapacity);
	void setChangePlayerActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount);

	/*set player knock back Coefficient when gun shoot */
	void SetKnockBackCoefficient(float knockBackCoefficient);
	
	/** actor npc */
	virtual void attackActorNpc(Entity* entity, PLAYER_ATTACK_TYPE damageType);
	void setSpeedAdditionLevel(int level);
	void updateExp(int level, float exp, int maxExp);

	/** attack CreatureEntity */
	virtual void attackCreatureEntity(EntityCreature* entityCreature, int attackType);

	void upgradeResource(int resourceId);
	void playSkillEffect(String name, float duration, int range, Vector3 color);
	void sendSkillEffect(int effectId);

	void setOccupation(int occupation) override;
	int getSlotByItemId(int ItemId);
	void showMask(int MilliSecond);
	void setArmItem(int itemId);
	void setUnlockedCommodities(std::vector<std::vector<int>> unlockedCommodities);

	void addGunBulletNum(int gunId, int bulletNum);
	void subGunRecoil(int gunId, float recoil);
	void subGunFireCd(int gunId, float fireCd);

	void setOnFrozen(float frozenTime);
	void setOnHurt(float hurtTime);
	void setOnColorful(float colorfulTime);
	void setDisableMove(float disableMoveTime);
	void setOnFire(int seconds);

	virtual void attackEntity(Entity* entity) override;
	bool isForceTeleport() { return m_isForceTeleport; }
	void forceTeleportStart() { m_isForceTeleport = true; }
	void forceTeleportEnd() { m_isForceTeleport = false; }
	void enableMovement();
	void checkForceMovement();
	bool isPlayerMoved();
	void recordOldPos();
	Ranch* getRanch() { return m_ranch; }
	BirdSimulator* getBirdSimulator() { return m_birdSimulator; }
	void setBagInfo(int curCapacity, int maxCapacity);
	void setBirdConvert(bool isConvert);

	i64 getDiamond();
	i64 getGold();

	void addCustomEffect(String name, String effectName, float duration);

protected:
	void initPlayerInventory();
	void monitorPropertyChanged();
	bool onPlayerHealthChanged(float old_health, float new_health);
	bool onPlayerAirChanged(int old_air, int new_air);
	bool onPlayerFoodLevelChanged(int old_foodLevel, int new_foodLevel);
	bool onPlayerFoodSaturationLevelChanged(float old_saturationLevel, float new_saturationLevel);

	void monitorInventoryChanged();

	/** implement override functions from EntityPlayer EntityLivingBase */
	virtual void resetHeight() { yOffset = 0.0F; }
	virtual void onItemUseFinish() override;
	virtual void onNewPotionEffect(PotionEffect* pEff) override;
	virtual void onChangedPotionEffect(PotionEffect* pEff, bool apply) override;
	virtual void onFinishedPotionEffect(PotionEffect* pEff) override;
	// void displayGUIEditSign(TileEntity par1TileEntity)
	// void incrementWindowID()
	// void displayGUIWorkbench(int par1, int par2, int par3)
	// void displayGUIEnchantment(int par1, int par2, int par3, String par4Str)
	// void displayGUIAnvil(int par1, int par2, int par3)
	// void displayGUIChest(IInventory par1IInventory)
	// void displayGUIHopper(TileEntityHopper par1TileEntityHopper)
	// void displayGUIHopperMinecart(EntityMinecartHopper par1EntityMinecartHopper)
	// void displayGUIFurnace(TileEntityFurnace par1TileEntityFurnace)
	// void displayGUIDispenser(TileEntityDispenser par1TileEntityDispenser)
	// void displayGUIBrewingStand(TileEntityBrewingStand par1TileEntityBrewingStand)
	// void displayGUIBeacon(TileEntityBeacon par1TileEntityBeacon)
	// void displayGUIMerchant(IMerchant par1IMerchant, String par2Str)
	// void func_110298_a(EntityHorse par1EntityHorse, IInventory par2IInventory)

	/** called from onUpdate for all tileEntity in specific chunks */
	void sendTileEntityToPlayer(TileEntity* pTileEntity);
	int onEnchantmentModifier(Entity * target, int  enchantmentModifier) override;

	void updateCustomEffects();

private:
	Vector3 lastMoveEventInvokePos;
	void updateMoveEventInvoke();
};

}

#endif
