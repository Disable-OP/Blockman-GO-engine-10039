#include "EntityPlayerSPMP.h"
#include "game.h"
#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "Stats/Stats.h"
#include "cItem/cItem.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityAircraft.h"

#include "Network/ClientNetwork.h"
#include "Util/CommonEvents.h"
#include "Item/Item.h"
#include "Object/Root.h"
#include "World/GameSettings.h"
#include "cEntity/PlayerControlller.h"
#include "Setting/LogicSetting.h"

using namespace GameClient;

namespace BLOCKMAN
{

EntityPlayerSPMP::EntityPlayerSPMP(Blockman* bm, World* pWorld, PlayerInteractionManager* interactionMgr)
	: EntityPlayerSP(bm, pWorld, 0, interactionMgr)
{
	oldPos = Vector3::ZERO;
	oldMinY = 0.f;
	oldYaw = 0.f;
	oldPitch = 0.f;
	wasOnGround = false;
	shouldStopSneaking = false;
	wasSneaking = false;
	m_moveTick = 0;
	hasSetHealth = false;
	m_bIsInAimSight = false;
	m_setEnableMovement = true;

	monitorItemInHandChange();
	monitorActionChange();
	deleteAllGuideArrow();
}

EntityPlayerSPMP::~EntityPlayerSPMP()
{
	//AimingStateChangeEvent::emit(false,NONE_TYPE);
	deleteAllGuideArrow();
}

bool EntityPlayerSPMP::onBaseActionModified(ActionState oldAction, ActionState newAction)
{
	if (oldAction != newAction) {
		m_baseActionChanged = true;
	}
	
	return true;
}

bool EntityPlayerSPMP::onUpperActionModified(ActionState oldAction, ActionState newAction)
{
	if (oldAction != newAction) {
		m_upperActionChanged = true;
	}

	return true;
}

void EntityPlayerSPMP::monitorActionChange()
{
	using namespace std::placeholders;
	m_subscriptionGuard.add(m_baseAction.subscribe(std::bind(&EntityPlayerSPMP::onBaseActionModified, this, _1, _2)));
	m_subscriptionGuard.add(m_upperAction.subscribe(std::bind(&EntityPlayerSPMP::onUpperActionModified, this, _1, _2)));
}

void EntityPlayerSPMP::monitorItemInHandChange()
{
	m_subscriptionGuard.add(inventory->onItemInHandChanged([this] {
		int index = -1;
		ItemStackPtr ptr = inventory->getCurrentItem();
		if (ptr)
		{
			index = inventory->findItemStack(ptr);
			// check the holding item is gun?
			if(ptr->isGunItem())
				isHoldGun = true;
			else
				isHoldGun = false;
		}
		else
		{
			isHoldGun = false;
		}

		m_itemInHandChanged = true;
		m_itemInHandIndex = index;
	}));
}

void EntityPlayerSPMP::onUpdate()
{
	if (m_itemInHandChanged) {
		m_itemInHandChanged = false;

		m_needUpdateUpperAction = true;
		ClientNetwork::Instance()->getSender()->sendChangeItemInHand(m_itemInHandIndex);
		// if the item in had is mesh (gun), the actor will reAttach all the slave.
		ItemStackPtr stack = inventory->getCurrentItem();
		if (stack)
		{
			cItem* citem = dynamic_cast<cItem*>(stack->getItem());
			if (citem && citem->getRenderType() == ItemRenderType::MESH && citem->getMeshName() != StringUtil::BLANK)
			{
				if (m_lastHandMesh != citem->getMeshName())
				{
					m_lastHandMesh = citem->getMeshName();
					m_handMeshChanged = true;
				}
			}
			else if (m_lastHandMesh != StringUtil::BLANK)
			{
				m_lastHandMesh = StringUtil::BLANK;
				m_handMeshChanged = true;
			}
		}
		else if (m_lastHandMesh != StringUtil::BLANK)
		{
			m_lastHandMesh = StringUtil::BLANK;
			m_handMeshChanged = true;
		}
	}

	if (m_hasShotLaserGun)
	{
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (!stack || !stack->getGunSetting() || (stack->getGunSetting()->gunType != LASER_EFFECT_GUN && stack->getGunSetting()->gunType != ELECTRICITY_EFFECT_GUN))
		{
			m_hasShotLaserGun = false;
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}
	}

	if (world->blockExists(BlockPos(int(Math::Floor(position.x)), 0, int(Math::Floor(position.z)))))
	{
		if (m_isCameraLocked)
		{
			position = m_cameraLockPos;
			m_prevCameraLockPos = m_cameraLockPos;
		}
			
		EntityPlayerSP::onUpdate();

		if (m_setEnableMovement)
		{
			Blockman::Instance()->m_playerControl->setDisableMovement(false);
			m_setEnableMovement = false;
		}

		if (isRiding())
		{
			// todo.
			// this.sendQueue.addToSendQueue(new Packet12PlayerLook(this.rotationYaw, this.rotationPitch, this.onGround));
			// this.sendQueue.addToSendQueue(new Packet27PlayerInput(this.moveStrafing, this.moveForward, this.movementInput.jump, this.movementInput.sneak));
		}
		else
		{
			sendMotionUpdates();
		}
	}

	updateSkillAnimationTime();
	updatePlayerAnimState();

	if (m_arrowTipMarkDestoryTimes > 0)
	{
		m_arrowTipMarkDestoryTimes = m_arrowTipMarkDestoryTimes - 50;
	}
	else {
		if (m_arrowTipMarkEntityId > 0) 
		{
			if (Blockman::Instance()->getWorld()->getEntity(m_arrowTipMarkEntityId))
			{
				Blockman::Instance()->getWorld()->removeEntity(m_arrowTipMarkEntityId);
				m_arrowTipMarkEntityId = 0;
			}
		}
	}

	if (m_guideArrowsPos.size() > 0)
	{
		for (auto iter = m_guideArrowsPos.begin(); iter != m_guideArrowsPos.end(); iter++)
		{
			Vector3 posTemp = this->position;
			posTemp.y -= 1.0f;
			float distance = iter->second.distanceTo(posTemp);
			if (distance <= 100.0f)
			{
				if (!m_guideArrowsEffect[iter->first])
				{
					SimpleEffect* arrowEffect = WorldEffectManager::Instance()->addSimpleEffect("guide_arrow.effect", posTemp, 0.0f, -1);
					if (!arrowEffect)
					{
						LordLogError("can't create guide arrow!");
						continue;
					}
					m_guideArrowsEffect[iter->first] = arrowEffect;
				}
				m_guideArrowsEffect[iter->first]->setExtenalParam(posTemp, iter->second);
			}
			else
			{
				if (m_guideArrowsEffect[iter->first])
				{
					m_guideArrowsEffect[iter->first]->mDuration = 0;
					m_guideArrowsEffect[iter->first] = nullptr;
				}
			}
		}
	}
}

void EntityPlayerSPMP::onLivingUpdate()
{
	EntityPlayerSP::onLivingUpdate();
	if (onGround && capabilities.isFlying)
	{
		capabilities.isFlying = false;
		ClientNetwork::Instance()->getSender()->sendSetFlying(false);
		sendPlayerAbilities();
	}
}

void EntityPlayerSPMP::sendMotionUpdates()
{
	updateSneakAndSprint();
	
	sendMovementInputUpdate();

	sendPlayerMovement();

	sendAnimateUpdate();
}

EntityItem* EntityPlayerSPMP::dropOneItem(bool all)
{
	int var2 = all ? 3 : 4;
	// todo.
	// sendQueue.addToSendQueue(new Packet14BlockDig(var2, 0, 0, 0, 0));
	return NULL;
}

void EntityPlayerSPMP::sendChatMessage(const String& msg)
{
	// todo.
	// sendQueue.addToSendQueue(new Packet3Chat(msg));
}

void EntityPlayerSPMP::swingItem()
{
	EntityLivingBase::swingItem();
	if (m_defendProgress > 0.f)
	{
		playDefendProgress(0.f);
	}
	// todo.
	// sendQueue.addToSendQueue(new Packet18Animation(this, 1));
}

void EntityPlayerSPMP::respawnPlayer()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet205ClientCommand(1));
}

void EntityPlayerSPMP::damageEntity(DamageSource* pSource, float amount)
{
	if (!isEntityInvulnerable())
	{
		setEntityHealth(getHealth() - amount);
	}
}

void EntityPlayerSPMP::closeScreen()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet101CloseWindow(openContainer.windowId));
	closeScreenAndDropStack();
}

void EntityPlayerSPMP::closeScreenAndDropStack()
{
	inventory->setItemStack(NULL);
	EntityPlayerSP::closeScreen();
}

void EntityPlayerSPMP::setHealth(float hp)
{
	if (hasSetHealth)
	{
		EntityPlayerSP::setHealth(hp);
	}
	else
	{
		setEntityHealth(hp);
		hasSetHealth = true;
	}
}

void EntityPlayerSPMP::addStat(StatBase* pStat, int amount)
{
	if (pStat)
	{
		if (pStat->isIndependent())
		{
			EntityPlayerSP::addStat(pStat, amount);
		}
	}
}

void EntityPlayerSPMP::incrementStat(StatBase* pStat, int amount)
{
	if (pStat)
	{
		if (!pStat->isIndependent())
		{
			EntityPlayerSP::addStat(pStat, amount);
		}
	}
}

void EntityPlayerSPMP::sendPlayerAbilities()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet202PlayerAbilities(capabilities));
}

void EntityPlayerSPMP::sendHorseJump()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet19EntityAction(this, 6, (int)(getHorseJumpPower() * 100.0F)));
}

void EntityPlayerSPMP::updateSneakAndSprint()
{
	bool isSprintInput = isSprinting();

	if (isSprintInput != wasSneaking)
	{
		wasSneaking = isSprintInput;
	}

	bool sneak = isSneaking();

	if (sneak != shouldStopSneaking)
	{
		shouldStopSneaking = sneak;
	}
}

void EntityPlayerSPMP::sendMovementInputUpdate()
{
	if (isOnAircraft() || (isOnVehicle() && !isDriver()))
		return;

	if (m_isCameraLocked || isWatchMode())
	{
		return;
	}

	auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
	bool moveForwardChanged = pClientMovementInput->moveForward != m_lastSyncMoveForward;
	bool moveStrafeChanged = pClientMovementInput->moveStrafe != m_lastSyncMoveStrafe;
	bool jumpChanged = pClientMovementInput->jump != m_lastSyncJump;
	bool downChanged = pClientMovementInput->down != m_lastSyncDown;

	m_lastSyncMoveForward = pClientMovementInput->moveForward;
	m_lastSyncMoveStrafe = pClientMovementInput->moveStrafe;
	m_lastSyncJump = pClientMovementInput->jump;
	m_lastSyncDown = pClientMovementInput->down;

	auto sender = ClientNetwork::Instance()->getSender();
	
	if (moveForwardChanged)
	{
		syncVehiclePositionToServer();
	}

	if (moveForwardChanged || moveStrafeChanged)
	{
		sender->sendMovementInputChanged(m_lastSyncMoveForward, m_lastSyncMoveStrafe);
	}

	if (jumpChanged)
	{
		sender->sendJumpChanged(m_lastSyncJump);
	}

	if (downChanged)
	{
		sender->sendDownChanged(m_lastSyncDown);
	}
}

void EntityPlayerSPMP::sendPlayerMovement()
{
	bool moved = isPlayerMoved();

	float dyaw = rotationYaw - oldYaw;
	float dPitch = rotationPitch - oldPitch;
	bool rotated = Math::Abs(dyaw) > 2.0f || Math::Abs(dPitch) > 2.0f;

	bool onGroundChanged = onGround != wasOnGround;

	if (ridingEntity || isOnVehicle() || isOnAircraft())
	{
		moved = false;
	}

	if (m_isCameraLocked)
	{
		m_cameraLockPos = position;
		position = m_originCameraLockPos;
		prevPos = position;
		if (moved)
			ClientNetwork::Instance()->getSender()->sendCameraLockMovement(m_cameraLockPos);

		return;
	}

	if (moved || onGroundChanged || rotated)
	{
		syncClientPositionToServer();
	}

	if (rotated && !isOnAircraft())
	{
		syncVehiclePositionToServer();
		ClientNetwork::Instance()->getSender()->sendPlayerRotationChanged(rotationYaw, rotationPitch);
	}

	if (onGroundChanged) {
		wasOnGround = onGround;
	}

	++m_moveTick;

	if (moved)
	{
		recordOldPosSPMP();
	}

	if (rotated)
	{
		oldYaw = rotationYaw;
		oldPitch = rotationPitch;
	}
}

bool EntityPlayerSPMP::isPlayerMoved()
{
	float dx = position.x - oldPos.x;
	float dy = boundingBox.vMin.y - oldMinY;
	float dz = position.z - oldPos.z;
	return dx * dx + dy * dy + dz * dz > 0.25 || m_moveTick >= 100;
}

void EntityPlayerSPMP::recordOldPosSPMP()
{
	oldPos = position;
	oldMinY = boundingBox.vMin.y;
	m_moveTick = 0;
}

void EntityPlayerSPMP::addGuideArrow(Vector3 arrowPos)
{
	bool alreadyHave = false;
	for (auto iter = m_guideArrowsPos.begin(); iter != m_guideArrowsPos.end(); iter++)
	{
		float distance = iter->second.distanceTo(arrowPos);
		if (distance <= 1.0f)
		{
			alreadyHave = true;
		}
	}
	if (!alreadyHave)
	{
		m_guideArrowsEffect.insert(std::make_pair(m_guideArrowsPos.size(), nullptr));
		m_guideArrowsPos.insert(std::make_pair(m_guideArrowsPos.size(), arrowPos));
	}
}

void EntityPlayerSPMP::deleteGuideArrow(Vector3 arrowPos)
{
	for (auto iter = m_guideArrowsPos.begin(); iter != m_guideArrowsPos.end();)
	{
		float distance = iter->second.distanceTo(arrowPos);
		if (distance <= 1.0f)
		{
			if (m_guideArrowsEffect[iter->first])
			{
				m_guideArrowsEffect[iter->first]->mDuration = 0;
				m_guideArrowsEffect[iter->first] = nullptr;
			}
			m_guideArrowsEffect.erase(iter->first);
			iter = m_guideArrowsPos.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void EntityPlayerSPMP::deleteAllGuideArrow()
{
	for (auto iter : m_guideArrowsEffect)
	{
		if (iter.second)
		{
			iter.second->mDuration = 0;
			iter.second = nullptr;
		}
	}
	m_guideArrowsPos.clear();
	m_guideArrowsEffect.clear();
}

void EntityPlayerSPMP::sendAnimateUpdate()
{
	if (m_baseActionChanged) {
		ClientNetwork::Instance()->getSender()->sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION, (int32_t)m_baseAction);
		m_baseActionChanged = false;
	}

	if (m_upperActionChanged) {
		ClientNetwork::Instance()->getSender()->sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION, (int32_t)m_upperAction);
		m_upperActionChanged = false;
	}
}

void EntityPlayerSPMP::syncClientPositionToServer()
{
	static uint32_t cntInWatchMode = 0;
	if(!isOnAircraft() && !isOnVehicle())
	{
		if (isWatchMode()) {
			cntInWatchMode++;
			if (cntInWatchMode % 30 != 0) {
				return;
			}
		} else {
			cntInWatchMode = 0;
		}
		
		auto sender = ClientNetwork::Instance()->getSender();
		sender->sendPlayerMovement(onGround, position.x, boundingBox.vMin.y, position.z, rotationYaw, rotationPitch);
	}		
}

void EntityPlayerSPMP::syncVehiclePositionToServer()
{
	if (isOnVehicle() && isDriver())
	{
		EntityVehicle* pVehicle = getVehicle();
		if (pVehicle != NULL)
		{
			auto sender = ClientNetwork::Instance()->getSender();
			sender->sendVehicleMovement(pVehicle->entityId, pVehicle->onGround, true, true, pVehicle->position.x, pVehicle->boundingBox.vMin.y, pVehicle->position.z, pVehicle->rotationYaw);
		}
	}
}

void EntityPlayerSPMP::openHorseInventory()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet19EntityAction(this, 7));
}

bool EntityPlayerSPMP::getCurrCameraStatus() 
{
	return m_isCameraLocked;
}

bool EntityPlayerSPMP::setCameraLocked(bool bLocked)
{
	EntityPlayer::setCameraLocked(bLocked);
	Blockman::Instance()->refreshActorHide();
	ClientNetwork::Instance()->getSender()->sendCameraLock(bLocked);
	return true;
}

bool EntityPlayerSPMP::gunFireIsNeedBullet()
{
	return LogicSetting::Instance()->getGunIsNeedBulletStatus();
}

int EntityPlayerSPMP::getCrossHairByGunType(int Type)
{
	int nCrossHairType = COMMON_CROSS_HAIR;
	switch (Type)
	{
	case PISTOL_GUN:
	case RIFLE_GUN:
	case RIFLE_EFFECT_GUN:
	case MORTAR_EFFECT_GUN:
	case LASER_EFFECT_GUN:
	case ELECTRICITY_EFFECT_GUN:
	case MOMENTARY_LASER_EFFECT_GUN:
	case MOMENTARY_PIERCING_LASER_EFFECT_GUN:
		nCrossHairType = RIFLE_CROSS_HAIR;
		break;
	case SNIPER_GUN:
	case SNIPER_EFFECT_GUN:
		nCrossHairType = NONE_TYPE;
		break;
	case SHOTGUN_GUN:
	case SHOTGUN_EFFECT_GUN:
		nCrossHairType = SHOTGUN_CROSS_HAIR;
		break;
	case SUBMACHINE_GUN:
		nCrossHairType = MACHINE_CROSS_HAIR;
		break;
	}
	return nCrossHairType;
}

}