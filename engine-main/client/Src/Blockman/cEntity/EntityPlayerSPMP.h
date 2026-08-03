/********************************************************************
filename: 	EntityPlayerSPMP.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-07-10
*********************************************************************/
#ifndef __ENTITY_PLAYER_SP_MP_HEADER__
#define __ENTITY_PLAYER_SP_MP_HEADER__

#include "cEntity/EntityPlayerSP.h"
#include "Util/SubscriptionGuard.h"
#include "World/WorldEffectManager.h"

class ClientNetwork;

namespace BLOCKMAN
{


class EntityPlayerSPMP : public EntityPlayerSP
{
	RTTI_DECLARE(EntityPlayerSPMP);
protected:
	Vector3 oldPos;
	float oldMinY = 0.f;
	float oldYaw = 0.f;
	float oldPitch = 0.f;

	/** Check if was on ground last update */
	bool wasOnGround = false;

	/** should the player stop sneaking? */
	bool shouldStopSneaking = false;
	bool wasSneaking = false;
	int m_moveTick = 0;

	/** has the client player's health been set? */
	bool hasSetHealth = false;
	String m_serverBrand;

	bool m_inventoryInited = false;
	bool m_itemInHandChanged = false;
	int m_itemInHandIndex = -999;
	SubscriptionGuard m_subscriptionGuard;

	bool m_baseActionChanged = false;
	bool m_upperActionChanged = false;
	int  m_arrowTipMarkEntityId = 0;
	int  m_arrowTipMarkDestoryTimes = 0;

public:
	bool m_bIsInAimSight = false;
	bool m_isShooting = false;// for screen shaking
	
	map<int, Vector3>::type m_guideArrowsPos;
	map<int, SimpleEffect*>::type m_guideArrowsEffect;

public:
	EntityPlayerSPMP(Blockman* bm, World* pWorld, PlayerInteractionManager* interactionMgr);

	~EntityPlayerSPMP();

	bool isInventoryInited() {
		return m_inventoryInited;
	}
	void setInventoryInited(bool inited) {
		m_inventoryInited = inited;
	}
	
	void setClientNetWork(ClientNetwork* net);

	/** implement override functions from EntityLivingBase EntityPlayerSP */
	virtual bool attackEntityFrom(DamageSource* pSource, float amount) { return false; }
	virtual void heal(float amount) {}
	virtual void onUpdate();
	virtual void onLivingUpdate();
	virtual EntityItem* dropOneItem(bool all);
	virtual void swingItem();
	virtual void respawnPlayer();
	virtual void closeScreen();
	virtual void setHealth(float hp);
	virtual void addStat(StatBase* pStat, int amount);
	virtual void sendPlayerAbilities();
	bool setCameraLocked(bool bLocked);
	bool getCurrCameraStatus();

	/** Send updated motion and position information to the server */
	void sendMotionUpdates();

	void sendChatMessage(const String& msg);

	void closeScreenAndDropStack();

	/** Used by NetClientHandler.handleStatistic */
	void incrementStat(StatBase* pStat, int amount);
	
	void setServerBrand(const String& brand) { m_serverBrand = brand; }

	const String& getServerBrand() { return m_serverBrand; }

	void openHorseInventory();

	void setArrowTipMarkEntityId(int EntityId) { m_arrowTipMarkEntityId = EntityId; }
	int getArrowTipMarkEntityId() { return m_arrowTipMarkEntityId; }
	void setArrowTipMarkDestoryTimes(int times) { m_arrowTipMarkDestoryTimes = times * 1000; }
	int getArrowTipMarkDestoryTimes() { return m_arrowTipMarkDestoryTimes; }
	bool gunFireIsNeedBullet();
	int getCrossHairByGunType(int Type);
	bool canMovement() { return true; }
	bool isPlayerMoved();
	void recordOldPosSPMP();

	void addGuideArrow(Vector3 arrowPos);
	void deleteGuideArrow(Vector3 arrowPos);
	void deleteAllGuideArrow();

protected:
	void monitorItemInHandChange();
	void monitorActionChange();
	bool onBaseActionModified(ActionState oldAction, ActionState newAction);
	bool onUpperActionModified(ActionState oldAction, ActionState newAction);

	/** implement override functions from EntityPlayer & EntityLivingBase */
	virtual void joinEntityItemWithWorld(EntityItem* pItem) {}
	virtual void damageEntity(DamageSource* pSource, float amount);
	virtual void sendHorseJump();

private:
	void updateSneakAndSprint();
	void sendMovementInputUpdate();
	void sendPlayerMovement();
	void sendAnimateUpdate();
	void syncClientPositionToServer();
	void syncVehiclePositionToServer();

	float m_lastSyncMoveForward = 0.0f;
	float m_lastSyncMoveStrafe = 0.0f;
	bool m_lastSyncJump = false;
	bool m_lastSyncDown = false;
};


}

#endif