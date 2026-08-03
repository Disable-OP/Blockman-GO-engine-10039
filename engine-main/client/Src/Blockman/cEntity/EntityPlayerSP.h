/********************************************************************
filename: 	EntityPlayerSP.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-17
*********************************************************************/
#ifndef __ENTITY_PLAYER_SP_HEADER__
#define __ENTITY_PLAYER_SP_HEADER__

#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

class Blockman;
class MouseFilter;
class GameSettings;
class PlayerInteractionManager;
class EntitySessionNpc;
class EntityBuildNpc;

class MovementInputFromOptions : public MovementInput
{
protected:
	GameSettings* gameSettings = nullptr;
public:
	MovementInputFromOptions(GameSettings* pGameSettings);
	virtual void updatePlayerMoveState();
};

class EntityPlayerSP : public EntityPlayer
{
	RTTI_DECLARE(EntityPlayerSP);

protected:
	Blockman* bm = nullptr;
	/** Used to tell if the player pressed forward twice. If this is at 0 and it's pressed (And they are allowed to sprint,
	aka enough food on the ground etc) it sets this to 7. If it's pressed and it's greater than 0 enable sprinting. */
	int sprintToggleTimer = 0;
	int horseJumpPowerCounter = 0;
	float horseJumpPower = 0.f;
	bool autoJumpEnabled = false;
	PlayerInteractionManager* m_iterationMgr = nullptr;
	bool isPlayReloadProgress = false;
	bool isLastFrameFastMoveForward = false;

	ItemStackPtr armStack = nullptr;

public:

	/** Ticks left before sprinting is disabled. */
	int sprintingTicksLeft = 0;
	float renderArmYaw = 0.f;
	float renderArmPitch = 0.f;
	float prevRenderArmYaw = 0.f;
	float prevRenderArmPitch = 0.f;
	/** The amount of time an entity has been in a Portal */
	float timeInPortal = 0.f;
	/** The amount of time an entity has been in a Portal the previous tick */
	float prevTimeInPortal = 0.f;
	int preStartParachuteSerpece = 0;

	bool m_isActorChange = false;

protected:
	bool canBlockPushOutEntity(const BlockPos& pos);
	/** Adds velocity to push the entity out of blocks at the specified x, y, z position */
	bool pushOutOfBlocks(const Vector3& vec);
	void do_Entity_moveEntity_with_debgu_info(const Vector3& vel);

public:
	EntityPlayerSP(Blockman* pmc, World* pWorld, int dim, PlayerInteractionManager* interactionMgr);
	static void setDefaultTexture(Texture* defaultTex);
	/** Gets the player's field of view multiplier. (ex. when flying) */
	float getFOVMultiplier();
	/** Updates health locally. */
	virtual void setHealth(float hp);
	/** Sets the current XP, total XP, and level number. */
	void setXPStats(float exp, int totalExp, int lvl);
	// void sendChatToPlayer(ChatMessageComponent par1ChatMessageComponent);
	/*** Returns true if the command sender is allowed to use the given command. */
	bool canCommandSenderUseCommand(int par1, const String& par2Str) { return par1 <= 0; }
	/** gun fire*/
	void gunFire();

	/** vehicle */
	virtual void askTakeOnVehicle(EntityVehicle* vehicle);
	virtual void askTakeOffVehicle();

	/** actor npc */
	virtual void attackActorNpc(Entity* entity);

	virtual void attackSessionNpc(EntitySessionNpc* npc);
	virtual void attackBuildNpc(EntityBuildNpc* npc);

	/*parachute*/
	bool startParachute();
	bool parachuteEnd();

	/** Return the position for this command sender. */
	Vector3i getPlayerCoordinates();
	bool isRidingInHorse() { return ridingEntity != NULL; /* todo. && ridingEntity instanceof EntityHorse */; }
	float getHorseJumpPower() { return horseJumpPower; }
	bool isAutoJumpEnabled() { return autoJumpEnabled; }
	virtual void sendHorseJump() {}

	/** Reload bullet clip. */
	bool beginReloadBullet(bool bForce = false);
	void updateReloadProgress();
	void _finishReloadBullet();
	int _getReloadAnimEnd(bool isForce = false);
	float getReloadProcess(float rdt);
	bool playReloadProgress();
	void playDefendProgress(float durationTime);
	void onUpdateDefendProgress();

	/** implement override functions from Entity/EntityLivingBase */
	virtual void updateEntityActionState();
	virtual void onUpdate();
	virtual void onLivingUpdate();
	virtual bool isSneaking() { return movementInput->sneak && !sleeping; }
	virtual void setSprinting(bool sprinting);
	virtual bool canSprinting();
	virtual ItemStackPtr getHeldItem();

	virtual void playStepSound(const BlockPos& pos, int blockID) override;
	virtual void playSound(SoundType soundType);
	virtual bool isClientWorld() { return true; }
	virtual Vector3 getLook(float ticktime);

	/** implement override functions from EntityPlayer */
	virtual void closeScreen();

	virtual void onCriticalHit(Entity* pEntity);
	virtual void onEnchantmentCritical(Entity* pEntity);
	virtual void addChatMessage(const String& str);
	virtual void addStat(StatBase* pStat, int amount);
	bool isZombie();
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	/*attack creature*/
	void attAckEntityCreature(EntityCreature* entityCreature);

	void removeArmStack();
	void setArmStack(ItemStackPtr stack);
	ItemStackPtr getArmStack();

	void updateCustomEffects();

};


}

#endif
