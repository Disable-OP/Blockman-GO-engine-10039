#include "EntityPlayerSP.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "cEntity/PlayerControlller.h"
#include "cWorld/Blockman.h"
#include "cWorld/PlayerInteractionManager.h"
#include "Render/RenderEntity.h"
#include "Render/RenderGlobal.h"

/** header files from common. */
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Stats/Stats.h"
#include "Entity/DamageSource.h"
#include "Inventory/InventoryPlayer.h"
#include "World/World.h"
#include "World/GameSettings.h"
#include "Network/ClientNetwork.h"
#include "Enums/Enums.h"
#include "Audio/SoundSystem.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"
#include "Setting/LogicSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Entity/EntityBuildNpc.h"
#include "game.h"

namespace BLOCKMAN
{

	MovementInputFromOptions::MovementInputFromOptions(GameSettings* pGameSettings)
		: gameSettings(pGameSettings)
	{
		lastSneak	= false;
		sneak		= false;
		isSprintInput		= false;
	}

	void MovementInputFromOptions::updatePlayerMoveState()
	{
		gameSettings->updateClientInput();
		sneak = gameSettings->m_clientMovementInput->sneak;
		isSprintInput = gameSettings->m_clientMovementInput->isSprintInput;

		/* apply input immediately */
		moveForward = gameSettings->m_clientMovementInput->moveForward;
		moveStrafe = gameSettings->m_clientMovementInput->moveStrafe;
		down = gameSettings->m_clientMovementInput->down;
		jump = gameSettings->m_clientMovementInput->jump;

		if (lastSneak != sneak) {
			if (sneak) {
				// send start sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK);
			}
			else {
				// send stop sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::STOP_SNEAK);
			}

			lastSneak = sneak;
		}
		/*
		if (gameSettings->usePole < 1)
		{
			moveStrafe = 0.0F;
			moveForward = 0.0F;
			if (gameSettings->keyBindForward->pressed || Blockman::Instance()->getAutoRunStatus())
			{
				++moveForward;
			}

			if (gameSettings->keyBindBack->pressed)
			{
				--moveForward;
			}

			if (gameSettings->keyBindLeft->pressed)
			{
				++moveStrafe;
			}

			if (gameSettings->keyBindRight->pressed)
			{
				--moveStrafe;
			}

			if (gameSettings->keyBindTopLeft->pressed)
			{
				moveForward = moveForward + 0.7f;
				moveStrafe = moveStrafe + 0.7f;
			}

			if (gameSettings->keyBindTopRight->pressed)
			{
				moveForward = moveForward + 0.7f;
				moveStrafe = moveStrafe - 0.7f;
			}
		}
		else
		{
			moveForward = gameSettings->m_poleForward;
			moveStrafe = gameSettings->m_poleStrafe;
		}

		down = gameSettings->keyBindSneak->pressed;
		jump = gameSettings->keyBindJump->pressed;
		sneak = sneakPressed ||  gameSettings->m_curMoveState == MoveState::SneakState;
		isSprintInput = gameSettings->m_curMoveState == MoveState::SprintState;

		if (sneak)
		{
			moveStrafe = moveStrafe * 0.3f;
			moveForward = moveForward * 0.3f;
		}

		if (lastSneak != sneak) {
			if (sneak) {
				// send start sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::C2SPacketPlayerActionType::START_SNEAK);
			}
			else {
				// send stop sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::C2SPacketPlayerActionType::STOP_SNEAK);
			}

			lastSneak = sneak;
		}
		*/
}

EntityPlayerSP::EntityPlayerSP(Blockman* pbm, World* pWorld, int dim, PlayerInteractionManager* interactionMgr)
	//: EntityPlayer(pWorld, "singlePlayer")
	: EntityPlayer(pWorld, GameClient::CGame::Instance()->getPlayerName())
{
	bm = pbm;
	dimension = dim;

	sprintToggleTimer = 0;
	horseJumpPowerCounter = 0;
	horseJumpPower = 0.f;
	sprintingTicksLeft = 0;
	renderArmYaw = 0.f;
	renderArmPitch = 0.f;
	prevRenderArmYaw = 0.f;
	prevRenderArmPitch = 0.f;
	timeInPortal = 0.f;
	prevTimeInPortal = 0.f;
	movementInput = NULL;
	autoJumpEnabled = true;
	autoJumpTime = 0;
	m_iterationMgr = NULL;
	isLastFrameFastMoveForward = false;

	if (interactionMgr)
	{
		interactionMgr->playerSP = this;
		interactionMgr->setGameType(pbm->m_worldSettings->getGameType());
		m_iterationMgr = interactionMgr;
	}
}

void EntityPlayerSP::updateEntityActionState()
{
	applyMovementInput();

	if (!isOnAircraft() && !isOnVehicle())
	{
		EntityPlayer::updateEntityActionState();
		updateReloadProgress();
	}

	prevRenderArmYaw = renderArmYaw;
	prevRenderArmPitch = renderArmPitch;
	renderArmPitch = renderArmPitch + (rotationPitch - renderArmPitch) * 0.5f;
	renderArmYaw = renderArmYaw + (rotationYaw - renderArmYaw) * 0.5f;
}

void EntityPlayerSP::onUpdate()
{
	if(m_iterationMgr)
		m_iterationMgr->updateBlockRemoving();
	EntityPlayer::onUpdate();
	onUpdateDefendProgress();
}

void EntityPlayerSP::onLivingUpdate()
{
	if (sprintingTicksLeft > 0)
	{
		--sprintingTicksLeft;

		if (sprintingTicksLeft == 0)
		{
			setSprinting(false);
		}
	}

	if (sprintToggleTimer > 0)
	{
		--sprintToggleTimer;
	}

	if (bm->m_playerControl->enableEverythingIsScrewedUpMode())
	{
		position.x = 0.0f;
		position.z = 0.0f;
		rotationYaw = ticksExisted / 12.0F;
		rotationPitch = 10.0F;
		position.y = 68.5f;
	}
	else
	{
		if (bm->m_playerControl->getDisableMovement())
		{
			KeyBinding::unPressAllKeys();
			Blockman::Instance()->m_gameSettings->setMouseMoveState(false);
			Blockman::Instance()->m_gameSettings->m_poleForward = 0.0f;
			Blockman::Instance()->m_gameSettings->m_poleStrafe = 0.0f;
		}

		prevTimeInPortal = timeInPortal;

		if (isPotionActive(Potion::confusion) && getActivePotionEffect(Potion::confusion)->getDuration() > 60)
		{
			timeInPortal += 0.006666667F;

			if (timeInPortal > 1.0F)
			{
				timeInPortal = 1.0F;
			}
		}
		else
		{
			if (timeInPortal > 0.0F)
			{
				timeInPortal -= 0.05F;
			}

			if (timeInPortal < 0.0F)
			{
				timeInPortal = 0.0F;
			}
		}

		if (timeUntilPortal > 0)
		{
			--timeUntilPortal;
		}

		bool isjump = movementInput->jump;
		float movespeed = 0.8F;
		movementInput->updatePlayerMoveState();
		auto pClientInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		bool isCurFrameFastMoveForward = movementInput->moveForward >= movespeed;
		if (isUsingItem() && !isRiding())
		{
			pClientInput->moveStrafe *= 0.2F;
			pClientInput->moveForward *= 0.2F;
			sprintToggleTimer = 0;
		}

		if (movementInput->sneak && yPositionDecrement < 0.2F)
		{
			yPositionDecrement = 0.2F;
		}

		bool flag3 = false;
		/*
		if (autoJumpTime > 0)
		{
			--autoJumpTime;
			flag3 = true;
			movementInput->jump = true;
		}
		*/

		Vector3 playerPos = getWidelyPos();
		float f = 0.35f;
		pushOutOfBlocks(playerPos + Vector3(-width*f, 0, width*f));
		pushOutOfBlocks(playerPos + Vector3(-width*f, 0, -width*f));
		pushOutOfBlocks(playerPos + Vector3(width*f, 0, -width*f));
		pushOutOfBlocks(playerPos + Vector3(width*f, 0, width*f));
		bool hasEnoughEnergy = getFoodStats()->getFoodLevel() > 6.0F || capabilities.allowFlying;

		if ((onGround && isCurFrameFastMoveForward && !isSprinting() && hasEnoughEnergy && !isUsingItem() && !isPotionActive(Potion::blindness))
			&& (movementInput->isSprintInput || !isLastFrameFastMoveForward))
		{
			if (sprintToggleTimer == 0)
			{
				sprintToggleTimer = 7;
			}
			else
			{
				setSprinting(true);
				sprintToggleTimer = 0;
			}
		}

		isLastFrameFastMoveForward = isCurFrameFastMoveForward;

		if (isSneaking())
		{
			sprintToggleTimer = 0;
		}

		if (isSprinting() && (!isCurFrameFastMoveForward || isCollidedHorizontally || !hasEnoughEnergy))
		{
			setSprinting(false);
		}

		if (isOnVehicle() && isjump)
		{
			askTakeOffVehicle();
		}

		if (isRidingInHorse())
		{
			if (horseJumpPowerCounter < 0)
			{
				++horseJumpPowerCounter;

				if (horseJumpPowerCounter == 0)
				{
					horseJumpPower = 0.0F;
				}
			}

			if (isjump && !movementInput->jump)
			{
				horseJumpPowerCounter = -10;
				sendHorseJump();
			}
			else if (!isjump && movementInput->jump)
			{
				horseJumpPowerCounter = 0;
				horseJumpPower = 0.0F;
			}
			else if (isjump)
			{
				++horseJumpPowerCounter;

				if (horseJumpPowerCounter < 10)
				{
					horseJumpPower = (float)horseJumpPowerCounter * 0.1F;
				}
				else
				{
					horseJumpPower = 0.8F + 2.0F / (float)(horseJumpPowerCounter - 9) * 0.1F;
				}
			}
		}
		else
		{
			horseJumpPower = 0.0F;
		}

		EntityPlayer::onLivingUpdate();
	}
}

float EntityPlayerSP::getFOVMultiplier()
{
	float fovScale = 1.0F;

	if (capabilities.isFlying)
	{
		fovScale *= 1.1F;
	}

	IAttributeInstance* att_movespeed = getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);
	fovScale = fovScale * ((att_movespeed->getAttributeValue() / capabilities.getWalkSpeed() + 1.0f) / 2.0f);

	if (isUsingItem() && (getItemInUse()->itemID == Item::bow->itemID || getItemInUse()->itemID == Item::bow_red->itemID || getItemInUse()->itemID == Item::bow_black->itemID))
	{
		int duration = getItemInUseDuration();
		float duration_sec = duration / 20.0F;

		if (duration_sec > 1.0F)
		{
			duration_sec = 1.0F;
		}
		else
		{
			duration_sec *= duration_sec;
		}

		fovScale *= 1.0F - duration_sec * 0.15F;
	}

	return fovScale;
}

void EntityPlayerSP::closeScreen()
{
	EntityPlayer::closeScreen();
	// bm.displayGuiScreen((GuiScreen)null);
}

void EntityPlayerSP::onCriticalHit(Entity* pEntity)
{
	// bm.effectRenderer.addEffect(new EntityCrit2FX(bm.theWorld, par1Entity));
}

void EntityPlayerSP::onEnchantmentCritical(Entity* pEntity)
{
	// EntityCrit2FX var2 = new EntityCrit2FX(bm.theWorld, par1Entity, "magicCrit");
	// bm.effectRenderer.addEffect(var2);
}

void EntityPlayerSP::setHealth(float hp)
{
	float damageAmount = getHealth() - hp;

	if (damageAmount <= 0.0F)
	{
		setEntityHealth(hp);

		if (damageAmount < 0.0F)
		{
			hurtResistantTime = maxHurtResistantTime / 2;
		}
	}
	else
	{
		lastDamage = damageAmount;
		setEntityHealth(getHealth());
		hurtResistantTime = maxHurtResistantTime;
		damageEntity(DamageSource::generic, damageAmount);
		hurtTime = maxHurtTime = 10;
	}
}

void EntityPlayerSP::addChatMessage(const String& str)
{
	// bm.ingameGUI.getChatGUI().addTranslatedMessage(par1Str, new Object[0]);
}

void EntityPlayerSP::addStat(StatBase* pStat, int amount)
{
	if (!pStat)
		return;

	if (pStat->isAchievement())
	{
		/*Achievement var3 = (Achievement)pStat;

		if (var3.parentAchievement == null || bm.statFileWriter.hasAchievementUnlocked(var3.parentAchievement))
		{
			if (!bm.statFileWriter.hasAchievementUnlocked(var3))
			{
				bm.guiAchievement.queueTakenAchievement(var3);
			}

			bm.statFileWriter.readStat(pStat, amount);
		}*/
	}
	else
	{
		// bm.statFileWriter.readStat(pStat, amount);
	}
}

bool EntityPlayerSP::canBlockPushOutEntity(const BlockPos& pos)
{
	if (!world->isBlockNormalCube(pos))
	{
		return false;
	}
	auto collided = false;
	auto block = BlockManager::sBlocks[world->getBlockId(pos)];
	if (!block)
	{
		return false;
	}
	AABBList collisionBoxList; 
	block->addCollisionBoxesToList(world, pos, boundingBox, collisionBoxList, this);
	for (const auto& box : collisionBoxList)
	{
		if (box.isIntersected(boundingBox))
		{
			return true;
		}
	}
	return false;
}

bool EntityPlayerSP::pushOutOfBlocks(const Vector3& vec)
{
	BlockPos pos = vec.getFloor();
	float x_off = vec.x - pos.x;
	float z_off = vec.z - pos.z;
	BlockPos pos1 = pos.getPosY();

	if (canBlockPushOutEntity(pos) || canBlockPushOutEntity(pos1))
	{
		bool stop_w = !canBlockPushOutEntity(pos.getNegX()) && !canBlockPushOutEntity(pos1.getNegX());
		bool stop_e = !canBlockPushOutEntity(pos.getPosX()) && !canBlockPushOutEntity(pos1.getPosX());
		bool stop_n = !canBlockPushOutEntity(pos.getNegZ()) && !canBlockPushOutEntity(pos1.getNegZ());
		bool stop_s = !canBlockPushOutEntity(pos.getPosZ()) && !canBlockPushOutEntity(pos1.getPosZ());
		i8 dir = -1;
		float min_off = 9999.0f;

		if (stop_w && x_off < min_off)
		{
			min_off = x_off;
			dir = 0;
		}

		if (stop_e && 1.0f - x_off < min_off)
		{
			min_off = 1.0f - x_off;
			dir = 1;
		}

		if (stop_n && z_off < min_off)
		{
			min_off = z_off;
			dir = 4;
		}

		if (stop_s && 1.0f - z_off < min_off)
		{
			min_off = 1.0f - z_off;
			dir = 5;
		}

		float push_speed = 0.1F;

		if (dir == 0)
		{
			motion.x = -push_speed;
		}

		if (dir == 1)
		{
			motion.x = push_speed;
		}

		if (dir == 4)
		{
			motion.z = -push_speed;
		}

		if (dir == 5)
		{
			motion.z = push_speed;
		}
	}

	return false;
}

void EntityPlayerSP::setSprinting(bool sprinting)
{
	if (!canSprinting())
		return;
	EntityLivingBase::setSprinting(sprinting);
	ClientNetwork::Instance()->getSender()->sendPlayerAction(
		sprinting ? NETWORK_DEFINE::PacketPlayerActionType::START_SPRINTING : NETWORK_DEFINE::PacketPlayerActionType::STOP_SPRINTING);
	sprintingTicksLeft = sprinting ? 600 : 0;
}

bool EntityPlayerSP::canSprinting()
{
	auto pRule = GameRuleSetting::getGameRule();
	return pRule->isCanSprint;
}

void EntityPlayerSP::setXPStats(float exp, int totalExp, int lvl)
{
	experience = exp;
	experienceTotal = totalExp;
	experienceLevel = lvl;
}

void EntityPlayerSP::gunFire()
{
	if (!isHoldGun)
		return;

	ItemStackPtr gun = inventory->getCurrentItem();
	ItemGun* pgunItem = dynamic_cast<ItemGun*>(gun->getItem());
	if (!pgunItem)
		return;
	pgunItem->fire(gun, world, this);
}

Vector3i EntityPlayerSP::getPlayerCoordinates()
{
	Vector3 pos = position + Vector3(0.5f, 0.5f, 0.5f);
	pos.floor();
	return Vector3i(int(pos.x), int(pos.y), int(pos.z));
}

void EntityPlayerSP::askTakeOnVehicle(EntityVehicle* vehicle)
{
	if (m_isActorChange)
	{
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_EDIT_BAN_GET_ON);
		return;
	}
	if (position.distanceTo(vehicle->position) > 4.f)
		return;

	if (isOnVehicle())
		return;

	bool hasDriver = vehicle->m_hasDriver;
	if (hasDriver)
	{
		if (vehicle->m_bisLocked)
		{
			String msg = LanguageManager::Instance()->getString(LanguageKey::VEHICLE_LOCKED);
			ChatMessageReceiveEvent::emit(msg);
			return;
		}

		if (vehicle->getFreeSeatNum() <= 0)
		{
			String msg = LanguageManager::Instance()->getString(LanguageKey::VEHICLE_FULL_LOADED);
			ChatMessageReceiveEvent::emit(msg);
			return;
		}
	}

	if (!hasDriver && !vehicle->canDrivedBy(this))
	{
		ShowVehicleTipDialogEvent::emit(vehicle->m_vehicleID);
		return;
	}

	ClientNetwork::Instance()->getSender()->sendTakeVehicle(true, vehicle->entityId);
}

void EntityPlayerSP::askTakeOffVehicle()
{
	if (!isOnVehicle())
		return;

	ClientNetwork::Instance()->getSender()->sendTakeVehicle(false, m_vehicleEntityId);
}

void EntityPlayerSP::attackActorNpc(Entity * entity)
{
	auto npc = dynamic_cast<EntityActorNpc*>(entity);
	if (npc)
	{
		ClientNetwork::Instance()->getSender()->sendAttackActorNpc(entity->entityId);
	}
}

void EntityPlayerSP::attackSessionNpc(EntitySessionNpc * npc)
{
	if (!npc)
	{
		return;
	}
	switch (npc->getSessionType())
	{
	case SessionType::DEFAULT:
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, npc->getSessionContent().c_str());
		break;
	case SessionType::GET_MANOR:
		ShowGetManorTipDialogEvent::emit();
		break;
	case SessionType::SELL_MANOR:
		ShowSellManorTipDialogEvent::emit();
		break;
	case SessionType::TIP_NPC:
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, npc->getSessionContent().c_str());
		break;
	case SessionType::MANOR_FURNITURE:
		if (Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			auto clientWorld = dynamic_cast<WorldClient*>(world);
			if (clientWorld->findManorByPlayerPosition(this) > 0 || m_isActorChange)
			{
				LordLogError("manor furniture edit failure: is not oneself's manor or actor change");
				return;
			}
			int id = atoi(npc->getSessionContent().c_str());
			if (id == 0)
			{
				LordLogError("manor furniture edit failure: session npc content is null");
				return;
			}
			SetPutFurnitureIdEvent::emit(id);
			ClientNetwork::Instance()->getSender()->sendPutFurniture(id, PutFurnitureStatus::EDIT_START);
		}
		break;

	case SessionType::MULTI_TIP_NPC:
		ShowMulitTipDialogEvent::emit(npc->getSessionContent().c_str());
		break;

	case SessionType::PERSONAL_SHOP:
		OpenPersonalShopEvent::emit();
		break;

	case SessionType::STATUE_NPC:
		ClientNetwork::Instance()->getSender()->sendAttackEntity(npc->entityId);
		break;
		
	case SessionType::SUPER_SHOP:
		OpenSuperShopEvent::emit();
		break;
		
	case SessionType::MULTI_TIP_INTERACTION_NPC:
		ShowMulitTipInteractionDialogEvent::emit(npc->getSessionContent().c_str());
		break;
	case SessionType::RANCHERS_EXPLORE_BEGIN:
		ShowRanchExBeginTipDialogEvent::emit();
		break;
	case SessionType::RANCHERS_TASK_FINISH:
		ShowRanchExTaskFinishTipDialogEvent::emit();
	case SessionType::ENCHANTMENT:
		ClientNetwork::Instance()->getSender()->sendOpenEnchantMent();
		break;
	case SessionType::MODE_SELECT:
		ClientNetwork::Instance()->getSender()->sendOpenModeSelect();
		break;
	case SessionType::GUN_STORE:
		OpenGunStoreEvent::emit();
		break;
	case SessionType::CHEST_LOTTERY:
		OpenChestLotteryEvent::emit();
		break;
	case SessionType::ARMOR_UPGRADE:
		ClientNetwork::Instance()->getSender()->sendOpenArmorUpgrade();
		break;
	case SessionType::SEASON_RANK:
		OpenSeasonRankEvent::emit();
		break;
	case SessionType::TREASURE_CHEST:
		if (npc->getTimeLeft() > 0)
		{
			ShowBirdOpenChestTipEvent::emit(npc->entityId, 1, npc->getTimeLeft());
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendBirdOpenTreasureChest(npc->entityId);
		}
		break;
	case SessionType::BIRD_LOTTERY:
		ShowBirdLotteryByNpcEvent::emit(npc->entityId);
		break;
	case SessionType::BIRD_STORE_ITEM:
		ShowBirdStoreByNpcEvent::emit(npc->getSessionContent());
		break;
	case SessionType::BIRD_TASK_TIP:
		ShowBirdTaskTipEvent::emit(npc->getSessionContent());
		break;
	default:
		break;
	}
}

void EntityPlayerSP::attackBuildNpc(EntityBuildNpc * npc)
{
	switch (npc->getBuildType())
	{
	case BuildType::BUILD_DEFAULT:
		break;
	case BuildType::BUILD_FARM:
		ShowRanchBuildFarmEvent::emit(npc->entityId);
		break;
	case BuildType::BUILD_PLANT:
		ShowRanchBuildFactoryEvent::emit(npc->entityId);
		break;
	case BuildType::BUILD_WAREHOUSE:
		RanchStorageOpenEvent::emit();
		break;
	}
}

bool EntityPlayerSP::startParachute() {
	capabilities.isParachute = true;
	preStartParachuteSerpece = Blockman::Instance()->getCurrPersonView();
	Blockman::Instance()->setPerspece(GameSettings::SPVT_THIRD);
	auto sender = ClientNetwork::Instance()->getSender();
	sender->sendChangeClothes(3000, "custom_wing");
	SoundSystem::Instance()->stopAllEffects();
	return true;
}

bool EntityPlayerSP::parachuteEnd() {
	capabilities.isParachute = false;
	Blockman::Instance()->setPerspece(preStartParachuteSerpece);
	auto sender = ClientNetwork::Instance()->getSender();
	sender->sendChangeClothes(0, "custom_wing");
	playSoundByType(SoundType::ST_Landing);
	return true;
}

bool EntityPlayerSP::playReloadProgress() {

	m_isReloadBullet = true;
	m_reloadBulletInt = 0;
	m_reloadPregress = m_prevReloadPregress = 0.f;
	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	const BulletClipSetting* pSetting = stack->getBulletClipSetting();
	if (!pSetting)
		return false;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return false;

	SoundSystem::Instance()->playEffectByType((SoundType)pGunSetting->switchClipSoundType);
	isPlayReloadProgress = true;
	return true;
}

void EntityPlayerSP::playDefendProgress(float durationTime)
{
	m_defendProgress = durationTime;
	if (durationTime == 0.f)
	{
		ClientNetwork::Instance()->getSender()->sendSkillType(SkillType::DEFENSE, false);
	}
}

void EntityPlayerSP::onUpdateDefendProgress()
{
	if (m_defendProgress >0.f)
	{
		m_defendProgress -= world->getFrameDeltaTime();
	}
}

bool EntityPlayerSP::beginReloadBullet(bool bForce)
{
	if (!isHoldGun)		// no gun in hand?
		return false;
	if (isHoldGunFire)	// gun is fire?
		return false;

	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	const BulletClipSetting* pSetting = stack->getBulletClipSetting();
	if (!pSetting)
		return false;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return false;

	int bulletId = pGunSetting->adaptedBulletId;
	int nCountBulletInInventory = inventory->getItemNum(bulletId);
	int bulletNum = stack->getCurrentClipBulletNum();
	int totalBulletNum = stack->getTotalBulletNum();

	if (bulletNum == totalBulletNum && totalBulletNum > 0)
		return false;

	if (!bForce)
	{
		if (totalBulletNum == 0) {
			int MaxBulletNum = getGunBulletNum(pGunSetting);
			if (MaxBulletNum > 0) {
				totalBulletNum = MaxBulletNum;
				stack->setTotalBulletNum(MaxBulletNum);
			}
		}

		if (!LogicSetting::Instance()->getGunIsNeedBulletStatus())
		{
			nCountBulletInInventory = getGunBulletNum(pGunSetting);
		}
		else
		{
			if (bulletNum >= totalBulletNum || nCountBulletInInventory <= 0) {
				return false;
			}
		}
	}

	m_isReloadBullet = true;
	m_reloadBulletInt = 0;
	m_reloadPregress = m_prevReloadPregress = 0.f;
	SoundSystem::Instance()->playEffectByType((SoundType)pGunSetting->switchClipSoundType);
	// play a reload sound please!

	return true;
}

void EntityPlayerSP::updateReloadProgress()
{
	if (isHoldGun && m_isReloadBullet)
	{
		int i = _getReloadAnimEnd(isPlayReloadProgress);
		if (i <= 0)
		{
			m_isReloadBullet = false;
			m_reloadBulletInt = 0;
			m_prevReloadPregress = m_reloadPregress = 0.f;
			isPlayReloadProgress = false;
			return;
		}

		++m_reloadBulletInt;

		if (m_reloadBulletInt >= i)
		{
			_finishReloadBullet();
		}
		m_reloadPregress = (float)m_reloadBulletInt / (float)i;
	}
	else
	{
		m_reloadBulletInt = 0;
	}
}

int EntityPlayerSP::_getReloadAnimEnd(bool isForce)
{
	if (!isHoldGun)		// no gun in hand?
		return 0;
	if (isHoldGunFire && !isForce)	// gun is fire?
		return 0;

	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	return pGun->getGunSetting()->reloadTick;
	//return 40;
}

void EntityPlayerSP::_finishReloadBullet()
{
	m_reloadBulletInt = 0;
	m_isReloadBullet = false;
	m_prevReloadPregress = m_reloadPregress = 0.f;

	ItemStackPtr stack = inventory->getCurrentItem();
	if (!stack || stack->stackSize < 1)
		return;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return ;
	
	int bulletId = pGunSetting->adaptedBulletId;
	int nCountBulletInInventory = inventory->getItemNum(bulletId);

	int bulletNum = stack->getCurrentClipBulletNum();
	int totalBulletNum = stack->getTotalBulletNum();

	if (isPlayReloadProgress && ( bulletNum > 0)) {
		isPlayReloadProgress = false;
		return;
	}

	int canAddBulletNum = totalBulletNum - bulletNum;
	if (LogicSetting::Instance()->getGunIsNeedBulletStatus())
	{
		canAddBulletNum = Math::Min(nCountBulletInInventory, canAddBulletNum);
		stack->reloadBulletClip(canAddBulletNum);
	}
	else
	{
		stack->reloadBulletClip(canAddBulletNum);
	}
	// finished reload bullet
	ClientNetwork::Instance()->getSender()->sendReloadBullet();
}

float EntityPlayerSP::getReloadProcess(float rdt)
{
	if (isHoldGun && m_isReloadBullet)
	{
		float delta = m_reloadPregress - m_prevReloadPregress;
		if (delta < 0.f)
			++delta;
		return m_prevReloadPregress + delta * rdt;
	}
	return 0.f;
}

ItemStackPtr EntityPlayerSP::getHeldItem()
{
	return inventory->getCurrentItem();
}

void EntityPlayerSP::playStepSound(const BlockPos & pos, int blockID)
{
	auto stepSound = cBlockManager::scBlocks[blockID]->getStepSound();

	if (world->getBlockId({ pos.x, pos.y + 1, pos.z }) == BlockManager::snow->getBlockID())
	{
		//playSound(stepSound.getStepSound(), stepSound.volume * 0.15F, stepSound.pitch);
		playSoundByType(stepSound.stepSound);
	}
	else if (!BlockManager::sBlocks[blockID]->getMaterial().isLiquid())
	{
		//playSound(stepSound.getStepSound(), stepSound.volume * 0.15F, stepSound.pitch);
		playSoundByType(stepSound.stepSound);
	}
}

void EntityPlayerSP::playSound(SoundType soundType)
{
	world->playSoundByType({ position.x, position.y - yOffset, position.z }, soundType);
}

void EntityPlayerSP::do_Entity_moveEntity_with_debgu_info(const Vector3& vel)
{
	Vector3 prePos = position;

	if (noClip)
	{
		boundingBox.move(vel);
		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
	}
	else
	{
		yPositionDecrement *= 0.4F;
		Vector3 pos = position;
		float x = vel.x;
		float y = vel.y;
		float z = vel.z;

		if (isInWeb)
		{
			isInWeb = false;
			x *= 0.25f;
			y *= 0.05f;
			z *= 0.25f;
			motion = Vector3::ZERO;
		}

		float tx = x;
		float ty = y;
		float tz = z;
		Box bb = boundingBox;
		EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(this);
		bool flag = onGround && isSneaking() && pplayer;

		if (flag)
		{
			float f;

			for (f = 0.05f; x != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, 0.0f)).empty(); tx = x)
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;
			}

			for (; z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(0.0f, -1.0f, z)).empty(); tz = z)
			{
				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;
			}

			while (x != 0.0f && z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, z)).empty())
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;

				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;

				tx = x;
				tz = z;
			}
		}

		Box extendBox = boundingBox.addCoord(x, y, z);
		AABBList& aabbs = world->getCollidingBoundingBoxes(this, extendBox);
		int* flags = new int[aabbs.size() + 1];
		memset(flags, 0, sizeof(int) * (aabbs.size() + 1));
		float ox = tx;
		float oy = ty;
		float oz = tz;

		for (int i = 0; i < int(aabbs.size()); ++i)
		{
			y = aabbs[i].getYOffset(boundingBox, y);
			if (y != oy)
			{
				oy = y;
				flags[i] = 1;
			}
		}

		boundingBox.move(0.0f, y, 0.0f);

		bool flag2 = onGround || ty != y && ty < 0.0f;
		int i;

		for (i = 0; i < int(aabbs.size()); ++i)
		{
			x = aabbs[i].getXOffset(boundingBox, x);
			if (x != ox)
			{
				ox = x;
				flags[i] = 1;
			}
		}

		boundingBox.move(x, 0.0f, 0.0f);

		for (i = 0; i < int(aabbs.size()); ++i)
		{
			z = aabbs[i].getZOffset(boundingBox, z);
			if (z != oz)
			{
				oz = z;
				flags[i] = 1;
			}
		}

		boundingBox.move(0.0f, 0.0f, z);

		// add all the collision box(red) and tested box to render visual helps.
		RenderGlobal* render = Blockman::Instance()->m_globalRender;
		render->visualHelps.clear();
		for (size_t i = 0; i < aabbs.size(); ++i)
		{
			ui32 color = flags[i] == 1 ? 0xFF9999FF : 0xFF99FF99;
			render->visualHelps.push_back(std::make_pair(aabbs[i], color));
		}
		render->visualHelps.push_back(std::make_pair(extendBox, 0xFFFF9999));
		render->drawVisualHelpBox();

		float dy;
		float dz;
		float dx;
		if (stepHeight > 0.0F && flag2 && (flag || yPositionDecrement < 0.05F) && (tx != x || tz != z))
		{
			dx = x;
			dy = y;
			dz = z;
			x = tx;
			y = stepHeight;
			z = tz;
			Box bb1 = boundingBox;
			boundingBox = bb;
			aabbs = world->getCollidingBoundingBoxes(this, boundingBox.addCoord(tx, y, tz));

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}

			boundingBox.move(0.0f, y, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				x = aabbs[i].getXOffset(boundingBox, x);
			}

			boundingBox.move(x, 0.0f, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				z = aabbs[i].getZOffset(boundingBox, z);
			}
			boundingBox.move(0.0f, 0.0f, z);

			y = -stepHeight;
			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}
			boundingBox.move(0.0f, y, 0.0f);

			if (dx * dx + dz * dz >= x * x + z * z)
			{
				x = dx;
				y = dy;
				z = dz;
				boundingBox = bb1;
			}
		}

		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
		isCollidedHorizontally = (Math::Abs(tx - x) > 0.001f) || Math::Abs(tz - z) > 0.001f;
		isCollidedVertically = (Math::Abs(ty - y) > 0.001f);
		onGround = isCollidedVertically && ty < 0.0f;
		isCollided = isCollidedHorizontally || isCollidedVertically;
		updateFallState(y, onGround);

		if (tx != x)
			motion.x = 0.0f;

		if (ty != y)
			motion.y = 0.0f;

		if (tz != z)
			motion.z = 0.0f;

		dx = position.x - pos.x;
		dy = position.y - pos.y;
		dz = position.z - pos.z;

		if (canTriggerWalking() && !flag && ridingEntity == NULL)
		{
			BlockPos pos = (position + Vector3(0.f, -0.2f - yOffset, 0.f)).getFloor();
			int blockID = world->getBlockId(pos);

			if (blockID == 0)
			{
				int blockID1 = world->getBlockId(pos.getNegY());

				if (blockID1 == BLOCK_ID_FENCE ||
					blockID1 == BLOCK_ID_COBBLE_STONE_WALL ||
					blockID1 == BLOCK_ID_FENCE_GATE)
				{
					blockID = blockID1;
				}
			}

			if (blockID != BLOCK_ID_LADDER)
				dy = 0.0f;

			distanceWalkedModified = distanceWalkedModified + Math::Sqrt(dx * dx + dz * dz) * 0.6f;
			distanceWalkedOnStepModified = distanceWalkedOnStepModified + Math::Sqrt(dx * dx + dy * dy + dz * dz) * 0.6f;

			if (distanceWalkedOnStepModified > nextStepDistance && blockID > 0)
			{
				nextStepDistance = (int)distanceWalkedOnStepModified + 1;

				if (isInWater())
				{
					float var42 = Math::Sqrt(motion.x * motion.x * 0.2f + motion.y * motion.y + motion.z * motion.z * 0.2f) * 0.35F;

					if (var42 > 1.0F)
						var42 = 1.0F;

					//playSound("liquid.swim", var42, 1.0F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
					if (world->canPlaySpecialSound(2))
					{
						playSoundByType(ST_Swim);
					}
				}

				BlockPos pos1(pos.x, i, pos.z);
				playStepSound(pos1, blockID);
				BlockManager::sBlocks[blockID]->onEntityWalking(world, pos1, this);
			}
		}

		doBlockCollisions();

		bool wet = isWet();

		if (world->isBoundingBoxBurning(boundingBox.contract(0.001f, 0.001f, 0.001f)))
		{
			dealFireDamage(1);

			if (!wet)
			{
				++fire;

				if (fire == 0)
				{
					setFire(8);
				}
			}
		}
		else if (fire <= 0)
		{
			fire = -fireResistance;
		}

		if (wet && fire > 0)
		{
			playSoundByType(ST_Fizz);
			//playSound("random.fizz", 0.7F, 1.6F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
			fire = -fireResistance;
		}
	}

}

Vector3 EntityPlayerSP::getLook(float ticktime)
{
	Camera* mainCamera = SceneManager::Instance()->getMainCamera();
	GameSettings* setting = Blockman::Instance()->m_gameSettings;
	Ray ray;
	mainCamera->getCameraRay(ray, setting->getMousePos());
	return ray.dir;
}

bool EntityPlayerSP::isZombie()
{
	if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::Zombie_Infecting)
	{
		return false;
	}
	return !m_isPeopleActor;
}

void EntityPlayerSP::setPositionAndRotation2(const Vector3 & pos, float yaw, float pitch, int increment)
{
	setPositionAndRotation(pos + Vector3(0.0f, yOffset, 0.0f), yaw, pitch);
	setVelocity(Vector3(0, 0, 0));
}

void  EntityPlayerSP::attAckEntityCreature(EntityCreature* entityCreature)
{
	if (entityCreature)
	{
		ClientNetwork::Instance()->getSender()->sendAttackCreatureEntity(entityCreature->entityId);
	}
}

void EntityPlayerSP::removeArmStack()
{
	if (armStack)
		armStack = nullptr;
}

void EntityPlayerSP::setArmStack(ItemStackPtr stack)
{
	removeArmStack();
	armStack = stack;
}

ItemStackPtr EntityPlayerSP::getArmStack()
{
	return armStack;
}

void EntityPlayerSP::updateCustomEffects()
{
	EntityPlayer::updateCustomEffects();
	for (auto& custom_effect : m_custom_effects)
	{
		if (custom_effect.m_status == CustomEffectStatus::Create)
		{
			custom_effect.m_status = CustomEffectStatus::Running;
			Vector3 pos = position;
			pos.y -= getHeight();
			custom_effect.m_effect = WorldEffectManager::Instance()->addSimpleEffect(custom_effect.m_effect_name, pos, rotationYaw, -1);
		}
		if (custom_effect.m_status == CustomEffectStatus::Running)
		{
			if (custom_effect.m_effect != NULL)
			{
				Vector3 pos = position;
				pos.y -= getHeight();
				custom_effect.m_effect->mPosition = pos;
				custom_effect.m_effect->mQuaternion = Quaternion(Vector3::UNIT_Y, -rotationYaw * Math::DEG2RAD);
			}
		}
		if (custom_effect.m_status == CustomEffectStatus::Unload)
		{
			custom_effect.m_status = CustomEffectStatus::Delete;
			custom_effect.m_effect->mDuration = 0;
			custom_effect.m_effect = NULL;
		}
	}
}

}
