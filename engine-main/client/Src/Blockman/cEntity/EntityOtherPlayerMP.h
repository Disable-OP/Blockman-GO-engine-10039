/********************************************************************
filename: 	EntityOtherPlayerMP.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-07-13
*********************************************************************/
#ifndef __ENTITY_OTHER_PLAYER_MP_HEADER__
#define __ENTITY_OTHER_PLAYER_MP_HEADER__

#include "Entity/EntityPlayer.h"
#include "Inventory/Crafting.h"

namespace BLOCKMAN
{

class Container;
class ItemStack;
class ChatMessageComponent;

class EntityOtherPlayerMP : public EntityPlayer
{
	RTTI_DECLARE(EntityOtherPlayerMP);
public:
	EntityOtherPlayerMP(World* pWorld, const String& name);
	~EntityOtherPlayerMP();

	/** implement override functions from EntityPlayer, EntityLivingBase */
	virtual bool attackEntityFrom(DamageSource* pSource, float amount) { return true; }
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	virtual void onUpdate();
	virtual float getShadowSize() { return 0.0F; }
	virtual void onLivingUpdate();
	virtual void setCurrentItemOrArmor(int armorType, ItemStackPtr pArmor);
	virtual float getEyeHeight() { return 1.82F; }
	virtual void playStepSound(const BlockPos & pos, int blockID) override;
	virtual bool isZombie();
	
	void sendChatToPlayer(ChatMessageComponent* msg);

	/** Returns true if the command sender is allowed to use the given command. */
	bool canCommandSenderUseCommand(int par1, const String& msg) { return false; }
	/** Return the position for this command sender. */
	BlockPos getPlayerCoordinates();

	void updateEntityActionState() override;
	float getNextYaw() { return otherPlayerMPYaw; }
	float getNextPitch() { return otherPlayerMPPitch; }
	bool isPlayerMoved();
	void recordOldPos();
	void updateCustomEffects();

protected:
	/** implement override functions from EntityPlayer, EntityLivingBase */
	virtual void resetHeight() { yOffset = 0.0F; }
	
protected:
	Vector3 oldPos;
	float oldMinY = 0.f;
	int m_moveTick = 0;

	bool isItemInUse = false;
	int otherPlayerMPPosRotationIncrements = 0;
	Vector3 otherPlayerPos;
	float otherPlayerMPYaw = 0.f;
	float otherPlayerMPPitch = 0.f;
};

}

#endif