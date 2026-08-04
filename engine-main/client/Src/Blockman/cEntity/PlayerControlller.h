/********************************************************************
filename: 	PlayerController.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-7
*********************************************************************/
#ifndef __PLAYER_CONTROLLER_HEADER__
#define __PLAYER_CONTROLLER_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"
#include "Item/ItemStack.h"
#include "Util/ClientEvents.h"
using namespace LORD;

namespace BLOCKMAN
{

class Blockman;
class ItemStack;
class EntityPlayerSPMP;
class Entity;
class EntityPlayer;
class World;
class PlayerInteractionManager;

class PlayerController : public ObjectAlloc
{
protected:
	/** The Blockman instance. */
	Blockman* bm = nullptr;
	// NetClientHandler netClientHandler;
	/** PosX of the current block being destroyed */
	BlockPos currentBlockPos;
	ItemStackPtr currentItemHittingBlock = nullptr;
	/** Current block damage (MP) */
	float curBlockDamageMP = 0.f;
	/** Tick counter, when it hits 4 it resets back to 0 and plays the step sound */
	float stepSoundTickCounter = 0.f;
	/** Delays the first damage on the block after the first click on the block */
	int blockHitDelay = 0;
	/** Tells if the player is hitting a block */
	bool isHittingBlock = false;
	/** Current game type for the player */
	GAME_TYPE currentGameType = GAME_TYPE_INVALID;
	/** Index of the current item held by the player in the inventory hotbar */
	int currentPlayerItem = 0;
	bool m_disableMovement = false;

	/** Current long hit entity times */
	int curLongHitEntityTimes = 0;
	/** Current long hit entity */
	Entity* curLongHitEntity = nullptr;

protected:
	
	bool sameToolAndBlock(const BlockPos& pos);
	/** Syncs the current player item with the server */
	void syncCurrentPlayItem();
	void setCurBlockDamageMP(float mpValue, RAYTRACE_TYPE type = RAYTRACE_TYPE_BLOCK) {
		this->curBlockDamageMP = mpValue;
		BreakBlockProgressEvent::emit(curBlockDamageMP, type);
	}

public:
	PlayerController(Blockman* pmc/*, NetClientHandler par2NetClientHandler*/);
	
	/** Block dig operation in creative mode (instantly digs the block). */
	static void clickBlockCreative(Blockman* pmc, PlayerController* ppc, const BlockPos& pos, int face);
	/** Sets player capabilities depending on current gametype. params: player */
	void setPlayerCapabilities(EntityPlayer* pPlayer);
	/** If modified to return true, the player spins around slowly around (0, 68.5, 0). The GUI is disabled, the view is
	* set to first person, and both chat and menu are disabled. Unless the server is modified to ignore illegal
	* stances, attempting to enter a world at all will result in an immediate kick due to an illegal stance. Appears to
	* be left-over debug, or demo code. */
	bool enableEverythingIsScrewedUpMode() { return false; }
	/** Sets the game type for the player. */
	void setGameType(GAME_TYPE type);
	/** Flips the player around. Args: player */
	void flipPlayer(EntityPlayer* pPlayer);
	bool shouldDrawHUD();
	/** Called when a player completes the destruction of a block */
	bool onPlayerDestroyBlock(const BlockPos& pos, int face);
	/** Called by Blockman class when the player is hitting a block with an item. Args: x, y, z, side */
	void clickBlock(const BlockPos& pos, int face);
	/** Resets current block damage and isHittingBlock */
	void resetBlockRemoving();
	/** Called when a player damages a block and updates damage counters */
	void onPlayerDamageBlock(const BlockPos& pos, int face);
	/** player reach distance = 4F */
	float getBlockReachDistance() { return currentGameType == GAME_TYPE_CREATIVE ? 6.5F : 6.5F; }
	void updateController();
	/** Handles a players right click. Args: player, world, x, y, z, side, hitVec */
	bool onPlayerClickToBlock(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack, const BlockPos& pos, int face, const Vector3& ray);
	/** Notifies the server of things like consuming food, etc... */
	bool useItem(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack);
	EntityPlayerSPMP* createClientPlayer(World* pWorld, PlayerInteractionManager* pInteractionMgr);
	/** Attacks an entity */
	void attackEntity(EntityPlayer* pPlayer, Entity* pEntity);
	bool interactWithEntity(EntityPlayer* pPlayer, Entity* pEntity);
	ItemStackPtr windowClick(int windowID, int slotId, int mouseButton, int clickType, EntityPlayer* pPlayer);
	/** GuiEnchantment uses this during multiplayer to tell PlayerControllerMP to send a packet indicating the enchantment action the player has taken. */
	void sendEnchantPacket(int windowID, int button);
	/** Used in PlayerControllerMP to update the server with an ItemStack in a slot. */
	void sendSlotPacket(ItemStackPtr pStack, int slot);
	void sendPacketDropItem(ItemStackPtr pStack);
	void onStoppedUsingItem(EntityPlayer* pPlayer);
	bool gameIsSurvivalOrAdventure() { return currentGameType == GAME_TYPE_SURVIVAL || currentGameType == GAME_TYPE_ADVENTURE; }
	/** Checks if the player is not creative, used for checking if it should break a block instantly */
	bool isNotCreative() { return currentGameType != GAME_TYPE_CREATIVE; }
	/** returns true if player is in creative mode */
	bool isInCreativeMode() { return currentGameType == GAME_TYPE_CREATIVE; }
	/** true for hitting entities far away. */
	bool extendedReach() { return currentGameType == GAME_TYPE_CREATIVE; }
	bool isRidingHorse();
	bool getDisableMovement() { return m_disableMovement; }
	void setDisableMovement(bool disableMovement) { m_disableMovement = disableMovement; }

	/** Called by Blockman class when the player is long hit a entity*/
	void onPlayerLongHitEntity(Entity* pEntity);
	void onPlayerCancelLongHitEntity();
	void onPlayerFinishLongHitEntity();
};

}

#endif