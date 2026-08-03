#include "EntityPlayer.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "EntityItem.h"
#include "EntityArrow.h"
#include "EntitySnowball.h"
#include "EntityMerchant.h"
#include "EntityVehicle.h"
#include "CombatTracker.h"
#include "Block/BM_Material.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Inventory/InventoryEnderChest.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/Container.h"
#include "Inventory/CoinManager.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "World/World.h"
#include "Stats/StatList.h"
#include "Item/Potion.h"
#include "World/WorldProvider.h"
#include "Chunk/ChunkCache.h"
#include "Common.h"
#include "Setting/LogicSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityAircraft.h"
#include "Chunk/ChunkService.h"
#include "World/Manor.h"
#include "World/Ranch.h"
#include "World/BirdSimulator.h"

namespace BLOCKMAN
{

	PlayerCapabilities::PlayerCapabilities()
		: disableDamage(false)
		, isFlying(false)
		, allowFlying(false)
		, isCreativeMode(false)
		, allowEdit(true)
		, isWatchMode(false)
		, isParachute(false)
		, flySpeed(0.05f)
		, walkSpeed(0.1f)
	{}

	void PlayerCapabilities::writeCapabilitiesToNBT(NBTTagCompound* pNbtCompound)
	{
		NBTTagCompound* pNbt = LordNew NBTTagCompound();
		pNbt->setBool("invulnerable", disableDamage);
		pNbt->setBool("flying", isFlying);
		pNbt->setBool("mayfly", allowFlying);
		pNbt->setBool("instabuild", isCreativeMode);
		pNbt->setBool("mayBuild", allowEdit);
		pNbt->setFloat("flySpeed", flySpeed);
		pNbt->setFloat("walkSpeed", walkSpeed);
		pNbtCompound->setTag("abilities", pNbt);
	}

	void PlayerCapabilities::readCapabilitiesFromNBT(NBTTagCompound* pNbtCompound)
	{
		if (!pNbtCompound->hasKey("abilities"))
			return;

		NBTTagCompound* pNbt = pNbtCompound->getCompoundTag("abilities");
		disableDamage = pNbt->getBool("invulnerable");
		isFlying = pNbt->getBool("flying");
		allowFlying = pNbt->getBool("mayfly");
		isCreativeMode = pNbt->getBool("instabuild");

		if (pNbt->hasKey("flySpeed"))
		{
			flySpeed = pNbt->getFloat("flySpeed");
			walkSpeed = pNbt->getFloat("walkSpeed");
		}

		if (pNbt->hasKey("mayBuild"))
		{
			allowEdit = pNbt->getBool("mayBuild");
		}
	}

	FoodStats::FoodStats()
		: foodLevel(20)
		, foodSaturationLevel(5.f)
		, foodExhaustionLevel(0.f)
		, foodTimer(0)
		, prevFoodLevel(20)
	{}

	void FoodStats::addStats(int foodlvl, float foodSatModifier)
	{
		foodLevel = Math::Min(foodlvl + foodLevel, 20);
		foodSaturationLevel = Math::Min(foodSaturationLevel + (float)foodlvl * foodSatModifier * 2.0F, (float)foodLevel);
	}

	void FoodStats::addStats(ItemFood* pFood)
	{
		addStats(pFood->getHealAmount(), pFood->getSaturationModifier());
	}

	void FoodStats::onUpdate(EntityPlayer* pPlayer)
	{
		//if (!LogicSetting::Instance()->isNeedFoodStats()) return;
		int diff = pPlayer->world->m_difficultySetting;
		bool canReducefoodSaturation = !pPlayer->isOnAircraft() && !pPlayer->isOnVehicle();
		prevFoodLevel = foodLevel;

		if (foodExhaustionLevel > 4.0F && LogicSetting::Instance()->isNeedFoodStats())
		{
			foodExhaustionLevel -= 4.0F;
			if (canReducefoodSaturation)
			{
				if (foodSaturationLevel > 0.0F)
				{
					foodSaturationLevel = Math::Max(foodSaturationLevel - 1.0F, 0.0F);
				}
				else if (diff > 0)
				{
					foodLevel = Math::Max(foodLevel - 1, 0);
				}
				else
				{
					foodLevel = Math::Max(foodLevel - 1, 0);
				}
			}
		}

		// todo.
		if (/*pPlayer->world->getGameRules().getGameRuleBooleanValue("naturalRegeneration") &&*/true && foodLevel >= 18 && pPlayer->shouldHeal())
		{
			++foodTimer;

			if (foodTimer >= 80 && LogicSetting::Instance()->isFoodHeal())
			{
				pPlayer->heal(1.0F);
				addExhaustion(3.0F);
				foodTimer = 0;
			}
		}
		else if (foodLevel <= 0)
		{
			++foodTimer;

			if (foodTimer >= 80)
			{
				if (pPlayer->getHealth() > 10.0F || diff >= 3 || pPlayer->getHealth() > 1.0F && diff >= 2)
				{
					pPlayer->attackEntityFrom(DamageSource::starve, 1.0F);
				}

				foodTimer = 0;
			}
		}
		else
		{
			foodTimer = 0;
		}
	}

	void FoodStats::readNBT(NBTTagCompound* pNbtCompound)
	{
		if (!pNbtCompound->hasKey("foodLevel"))
			return;

		foodLevel = pNbtCompound->getInteger("foodLevel");
		foodTimer = pNbtCompound->getInteger("foodTickTimer");
		foodSaturationLevel = pNbtCompound->getFloat("foodSaturationLevel");
		foodExhaustionLevel = pNbtCompound->getFloat("foodExhaustionLevel");
	}

	void FoodStats::writeNBT(NBTTagCompound* pNbtCompound)
	{
		pNbtCompound->setInteger("foodLevel", foodLevel);
		pNbtCompound->setInteger("foodTickTimer", foodTimer);
		pNbtCompound->setFloat("foodSaturationLevel", foodSaturationLevel);
		pNbtCompound->setFloat("foodExhaustionLevel", foodExhaustionLevel);
	}

	EntityPlayer::EntityPlayer(World* pWorld, const String& name)
		: EntityLivingBase(pWorld)
		, username(name)
		, showName(name)
	{
		doConstruct();
	}

	EntityPlayer::~EntityPlayer()
	{
		doDestruct();
	}

	void EntityPlayer::doDestruct()
	{
		LordSafeDelete(m_wallet);
		LordSafeDelete(m_backpack);
		LordSafeDelete(m_manor);
		LordSafeDelete(m_ranch);
		LordSafeDelete(m_birdSimulator);
		LordSafeDelete(inventoryContainer);
		LordSafeDelete(inventory);
		LordSafeDelete(foodStats); 
		LordSafeDelete(theInventoryEnderChest);
		clearCustomEffects();
	}

	void EntityPlayer::doConstruct()
	{
		yOffset = 1.62F;
		Vector3i spawnPos = world->getSpawnPoint();
		setLocationAndAngles(Vector3(spawnPos) + Vector3(0.5f, 1.f, 0.5f), 0.0F, 0.0F);
		unused180 = 180.0F;
		fireResistance = 20;

		theInventoryEnderChest = LordNew InventoryEnderChest();
		foodStats = LordNew FoodStats();
		
		flyToggleTimer = 0;
		sleeping = false;
		sleepTimer = 0;
		spawnChunk = Vector3i::INVALID;
		spawnForced = false;
		startMinecartRidingCoordinate = Vector3i::INVALID;
		itemInUseCount = 0;
		speedOnGround = 0.1F;
		speedInAir = 0.02F;
		lastXPSound = 0;

		inventory = LordNew InventoryPlayer(this);
		m_wallet = LordNew Wallet(inventory);
		m_backpack = LordNew Backpack();
		m_manor = LordNew Manor();
		m_ranch = LordNew Ranch();
		m_birdSimulator = LordNew BirdSimulator();
		inventoryContainer = LordNew ContainerPlayer(inventory, !world->m_isClient, this);
		openContainer = inventoryContainer;
		prevCameraYaw = 0.f;
		cameraYaw = 0.f;
		xpCooldown = 0;
		prevChasingPosX = 0.f;
		prevChasingPosY = 0.f;
		prevChasingPosZ = 0.f;
		chasingPosX = 0.f;
		chasingPosY = 0.f;
		chasingPosZ = 0.f;
		bedLocation = Vector3i::INVALID;
		renderBedOffset = Vector3::ZERO;
		experienceLevel = 0;
		experienceTotal = 0;
		experience = 0.f;
		itemInUse = NULL;
		m_prevBaseAction = AS_IDLE;
		m_prevUpperAction = AS_IDLE;
		m_baseAction = AS_IDLE;
		m_upperAction = AS_IDLE;
		m_aircraftEntityId = 0;
		m_vehicleEntityId = 0;
		m_isDriver = false;

		//capabilities init
		capabilities = PlayerCapabilities();

		m_outLooksChanged = true;
		m_faceID = 1;				// custom user define the face model
		m_hairID = 1;				// custom user define the hair model
		m_topsID = 1;				// user clothes tops id. 0 is not exist
		m_pantsID = 1;				// user clothes pants id. 0 is not exist
		m_shoesID = 1;				// user clothes shoes id. 0 is not exist
		m_glassesId = 0;			// user glasses id. 0 is not exist
		m_scarfId = 0;
		m_wingId = 0;
		m_sex = 1;
		m_hatId = 0;
		m_decorate_hatId = 0;
		m_armId = 0;
		m_extra_wingId = 0;
		m_footHaloId = 0;
		m_showClothes = false;		// custom user define show the clothes or show the armor(if armor parts is exist)
		m_skinColor = Color(1.f, 0.9f, 0.9f, 0.f);
		m_team = 0;
		m_bagId = 0;
		m_logout = false;
		m_delayTicksToFree = 60;
		autoJumpTime = 0;
		enableMovement();
	}

	void EntityPlayer::addGunDamage(int gunId, float damage)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		m_gunExtraAttrMap->addGunDamage(gunId, damage);
	}

	void EntityPlayer::addGunBulletNum(int gunId, int bulletNum)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		m_gunExtraAttrMap->addGunBulletNum(gunId, bulletNum);
	}

	void EntityPlayer::subGunRecoil(int gunId, float recoil)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		m_gunExtraAttrMap->subGunRecoil(gunId, recoil);
	}

	void EntityPlayer::subGunFireCd(int gunId, float fireCd)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		m_gunExtraAttrMap->subGunFireCd(gunId, fireCd);
	}

	float EntityPlayer::getGunDamage(const GunSetting* setting)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		return setting->damage + m_gunExtraAttrMap->getGunExtraAttr(setting->gunId)->add_damage;
	}

	int EntityPlayer::getGunBulletNum(const GunSetting* setting)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		return setting->maxBulletNum + m_gunExtraAttrMap->getGunExtraAttr(setting->gunId)->add_bullet_num;
	}

	float EntityPlayer::getGunRecoil(const GunSetting* setting)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		float recoil = setting->recoil - m_gunExtraAttrMap->getGunExtraAttr(setting->gunId)->sub_recoil;
		return recoil < 0.01f ? 0.01f : recoil;
	}

	float EntityPlayer::getGunFireCd(const GunSetting * setting)
	{
		if (m_gunExtraAttrMap == nullptr)
		{
			m_gunExtraAttrMap = GunExtraAttrSetting::getGunExtraAttrMap(platformUserId);
		}
		float fireCd = setting->cdTime - m_gunExtraAttrMap->getGunExtraAttr(setting->gunId)->sub_fire_cd;
		return fireCd < 0.01f ? 0.01f : fireCd;
	}

	bool EntityPlayer::canAddBulletItem(int itemId)
	{
		return true;
	}

	bool EntityPlayer::canBuy(const Commodity & commodity, BuyResult& buyResult)
	{
		if (commodity.price <= 0)
		{
			buyResult = BuyResult::BuySuccess;
			return true;
		}

		bool canAddItem = true;
		buyResult = BuyResult::BuySuccess;
		if (!canAddBulletItem(commodity.itemId))
		{
			buyResult = BuyResult::HasNoGun;
			return false;
		}

		Item* pItem = Item::getItemById(commodity.itemId);
		if (commodity.itemId >= 10000)
		{
			canAddItem = true;
		}
		else if (pItem && dynamic_cast<ItemAppIcon*>(pItem))
		{
			canAddItem = true;
		}
		else
		{
			if (commodity.itemId < 5096)
			{
				auto item = Item::itemsList[commodity.itemId];
				if (item)
				{
					auto newItemStack = LORD::make_shared<ItemStack>(commodity.itemId, commodity.itemNum, commodity.itemMeta);
					canAddItem = inventory->canItemStackAddedToInventory(newItemStack);
				}
				else
				{
					canAddItem = true;
				}
			}
			else
			{
				canAddItem = true;
			}
		}

		int moneyId = CoinManager::Instance()->coinIdToItemId(commodity.coinId);
		bool hasEnoughMoney = true;
		if (!m_wallet)
		{
			return false;
		}
		if (moneyId >= 10000)
		{
			hasEnoughMoney = m_wallet->getCurrency() >= commodity.price;
			buyResult = BuyResult::LackOfMoney;
		}
		else
		{
			hasEnoughMoney = m_wallet->getCoinNum(commodity.coinId) >= commodity.price;
			buyResult = commodity.itemId >= 10000 ? BuyResult::LackOfItem : BuyResult::LackOfMoney;
		}
		return canAddItem && hasEnoughMoney;
	}

	bool EntityPlayer::canGet(const Commodity & commodity, BuyResult& buyResult)
	{
		bool canAddItem = true;
		buyResult = BuyResult::BuySuccess;
		if (commodity.itemId < 5096)
		{
			auto item = Item::itemsList[commodity.itemId];
			if (item)
			{
				if (inventory->isContainItem(commodity.itemId, commodity.itemMeta))
				{
					canAddItem = false;
					buyResult = BuyResult::HaveGotGood;
				}
				else
				{
					auto newItemStack = LORD::make_shared<ItemStack>(commodity.itemId, commodity.itemNum, commodity.itemMeta);
					canAddItem = inventory->canItemStackAddedToInventory(newItemStack);
					buyResult = canAddItem ? buyResult : BuyResult::FullInventory;
				}
			}
		}
		return canAddItem;
	}

	bool EntityPlayer::canUnlock(const Commodity & commodity, BuyResult& buyResult)
	{
		buyResult = BuyResult::BuySuccess;
		int moneyId = CoinManager::Instance()->coinIdToItemId(commodity.coinId);
		bool hasEnoughMoney = true;
		if (!m_wallet)
		{
			buyResult = BuyResult::LackOfMoney;
			return false;
		}
		if (moneyId >= 10000)
		{
			hasEnoughMoney = m_wallet->getCurrency() >= commodity.price;
		}
		else
		{
			hasEnoughMoney = m_wallet->getCoinNum(commodity.coinId) >= commodity.price;
		}
		buyResult = hasEnoughMoney ? buyResult : BuyResult::LackOfMoney;

		bool isLocking = !this->isUnlocked(commodity.itemId, commodity.itemMeta);

		return hasEnoughMoney && isLocking;
	}

	void EntityPlayer::setFire(int second)
	{
		if (capabilities.isWatchMode)
		{
			return;
		}
		EntityLivingBase::setFire(second);
	}

	void EntityPlayer::applyEntityAttributes()
	{
		EntityLivingBase::applyEntityAttributes();
		attributeMap->registerAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->setBaseValue(1.0f);
	}

	void EntityPlayer::entityInit()
	{
		EntityLivingBase::entityInit();
		data_hide_caps = 0;
		data_absorption = 0.f;
		data_score = 0;
	}

	int EntityPlayer::getItemInUseDuration()
	{
		return isUsingItem() ? itemInUse->getMaxItemUseDuration() - itemInUseCount : 0;
	}

	void EntityPlayer::stopUsingItem()
	{
		if (itemInUse)
		{
			itemInUse->onPlayerStoppedUsing(world, this, itemInUseCount);
		}
		clearItemInUse();
	}

	void EntityPlayer::clearItemInUse()
	{
		itemInUse = nullptr;
		itemInUseCount = 0;

		if (!world->m_isClient)
		{
			setEating(false);
		}
	}
		
	bool EntityPlayer::isBlocking()
	{
		return isUsingItem() && Item::itemsList[itemInUse->itemID]->getItemUseAction(itemInUse) == ACTION_TYPE_BLOCK;
	}

	bool EntityPlayer::consumeBullet()
	{
		ItemStackPtr stack = getHeldItem();
		if (!stack || stack->stackSize < 1)
			return false;

		if (!stack->consumeBullet())
			return false;

		return true;
	}

	bool EntityPlayer::shootTargetByGun(EntityPlayer * pTarget, const C2SPACKET_DETAIL::Vector3& hitpos,
		bool isHeadshot, const GunSetting* pGunSetting)
	{
		if (pTarget->capabilities.disableDamage)
			return false;

		SCRIPT_EVENT::EntityHitEvent::invoke(pGunSetting->adaptedBulletId, hitpos, entityId);
		float amount = getGunDamage(pGunSetting);
		if (isHeadshot)
		{
			amount += pGunSetting->headshotDamage;
		}
		DamageSource* pSource = DamageSource::causePlayerGunDamage(this, isHeadshot);
		pSource->knockBackDis = pGunSetting->knockbackDistance;
		
		if (pTarget->attackEntityFrom(pSource, amount))
		{
			EnchantmentThorns::damageArmor(this, pTarget, rand);
		}

		LordSafeDelete(pSource);
		return true;
	}

	void EntityPlayer::reloadBullet()
	{
		ItemStackPtr stack = getHeldItem();
		if (!stack || stack->stackSize < 1)
			return;

		const GunSetting* pGunSetting = stack->getGunSetting();
		if (!pGunSetting)
			return ;

		int bulletId = pGunSetting->adaptedBulletId;
		int nCountBulletInInventory = inventory->getItemNum(bulletId);

		int bulletNum = stack->getCurrentClipBulletNum();
		int totalBulletNum = stack->getTotalBulletNum();

		if (totalBulletNum == 0) {
			int MaxBulletNum = getGunBulletNum(pGunSetting);
			if (MaxBulletNum > 0) {
				totalBulletNum = MaxBulletNum;
				stack->setTotalBulletNum(MaxBulletNum);
				if (!LogicSetting::Instance()->getGunIsNeedBulletStatus())
				{
					stack->setCurrentClipBulletNum(MaxBulletNum);
				}
			}
		}

		int canAddBulletNum = totalBulletNum - bulletNum;
		if (LogicSetting::Instance()->getGunIsNeedBulletStatus())
		{
			canAddBulletNum = Math::Min(nCountBulletInInventory, canAddBulletNum);
			if (stack->reloadBulletClip(canAddBulletNum)) {
				inventory->removeItem(bulletId, canAddBulletNum);
			}
		}
		else
		{
			nCountBulletInInventory = getGunBulletNum(pGunSetting);
			canAddBulletNum = Math::Min(nCountBulletInInventory, canAddBulletNum);
			stack->reloadBulletClip(canAddBulletNum);
		}
	}

	int EntityPlayer::getHeldItemId()
	{
		ItemStackPtr stack = getHeldItem();
		if (!stack || stack->stackSize < 1)
			return 0;
		return stack->itemID;
	}

	void EntityPlayer::syncPlayerMovement(const Vector3& pos, float yaw, float pitch, float swing, float yawhead, float lbyaw)
	{
		prevPos = position;
		position = pos;
		prevRotationYaw = rotationYaw;
		rotationYaw = yaw;
		prevRotationPitch = rotationPitch;
		rotationPitch = pitch;
		prevRotationYawHead = rotationYawHead;
		rotationYawHead = yawhead;
		prevLimbYaw = limbSwing;
		limbSwing = lbyaw;
		if (isHoldGun)
		{
			prevGunFireProgress = gunFireProgress;
			gunFireProgress = swing;
		}
		else
		{
			prevSwingProgress = swingProgress;
			swingProgress = swing;
		}

		yPositionDecrement = 0.0F;
		float dyaw = prevRotationYaw - yaw;

		if (dyaw < -180.0f)
			prevRotationYaw += 360.0F;

		if (dyaw >= 180.0f)
			prevRotationYaw -= 360.0F;

		setPosition(position);
		setRotation(yaw, pitch);
	}

	void EntityPlayer::updateSkillAnimationTime()
	{
		if (!m_isPlayingSkill || m_PlayingSkillTime <= 0)
			return;

		int dt = (int)(world->getFrameDeltaTime() * 1000);
		m_PlayingSkillTime -= dt;
		if (m_PlayingSkillTime <= 0)
		{
			stopSkillAnimation();
		}
	}

	void EntityPlayer::setSkillAnimationTime(int duration)
	{
		if (m_isPlayingSkill) 
		{ 
			m_PlayingSkillTime = duration;
		}
	}

	bool EntityPlayer::setSkillAnimation(ActionState actionState)
	{
		if (actionState >= AS_STATE_TOTAL || actionState < AS_ATTACK)
			return false;

		m_PlayingSkillTime = 0;
		m_isPlayingSkill = true;
		m_prevUpperAction = m_upperAction;
		m_upperAction = actionState;
		return true;
	}

	void EntityPlayer::updatePlayerAnimState()
	{
		if (m_isCameraLocked)
			return;
		//m_prevBaseAction = m_baseAction;
		// update base action.
		if (!onGround && !isInWater() && !isParachute() && !isOnLadder())
			m_baseAction = AS_JUMP_BEGIN;
		else if (onGround && m_prevBaseAction == AS_JUMP_BEGIN)
			m_baseAction = AS_JUMP_END;
		else if (isOnLadder())
		{
			float deltax = position.x - prevPos.x;
			float deltaz = position.z - prevPos.z;
			float deltay = position.y - prevPos.y;
			bool moving = (deltax * deltax + deltaz * deltaz + deltay * deltay) > 0.0004f;

			if (moving)
			{
				m_baseAction = deltay > 0 ? AS_ON_LADDER_UP : AS_ON_LADDER_DOWN;
			}
			else
			{
				m_baseAction = AS_ON_LADDER_STATIC;
			}
			
		}
		else if (isSneaking() || isBlocking())
			m_baseAction = m_moving ? AS_WALK : AS_SNEAK;
		else if (isSprinting())
			m_baseAction = m_moving ? AS_SPRINT : AS_IDLE;
		else if (isInWater())
			m_baseAction = m_moving ? AS_SWIM : AS_FLOAT;
		else
			m_baseAction = m_moving ? AS_RUN : AS_IDLE;

		// update upper action.
		if (m_isPlayingSkill)
			return;

		//m_prevUpperAction = m_upperAction;
		ItemStackPtr pStack = inventory->getCurrentItem();
		if (pStack && getItemInUseCount() > 0)
		{
			ACTION_TYPE action = pStack->getItemUseAction();
			switch (action)
			{
			case BLOCKMAN::ACTION_TYPE_EAT:		m_upperAction = AS_EAT_BEGIN; break;
			case BLOCKMAN::ACTION_TYPE_DRINK:	m_upperAction = AS_DRINK_BEGIN; break;
			case BLOCKMAN::ACTION_TYPE_RELOAD:	m_upperAction = AS_GUN_RELOAD; break;
				//case BLOCKMAN::ACTION_TYPE_BLOCK:	m_upperAction = AS_BLOCK; break;
			case BLOCKMAN::ACTION_TYPE_BOW:		m_upperAction = AS_AIM_BEGIN; break;
			default: break;
			}
		}
		else if (swingProgress > 0.f)
			m_upperAction = AS_ATTACK;
		else if (gunFireProgress > 0.f)
			m_upperAction = AS_GUN_FIRE;
		else if (m_reloadPregress > 0.f)
			m_upperAction = AS_GUN_RELOAD;
		else if (m_defendProgress > 0.0f)
			m_upperAction = AS_BLOCK;
		else
		{
			if (m_prevUpperAction == AS_EAT_BEGIN)
				m_upperAction = AS_EAT_END;
			else if (m_prevUpperAction == AS_DRINK_BEGIN)
				m_upperAction = AS_DRINK_END;
			else if (m_prevUpperAction == AS_AIM_BEGIN)
				m_upperAction = AS_AIM_END;
			else
				m_upperAction = m_baseAction;
		}
	}

	void EntityPlayer::onUpdate()
	{
		onCapabilitieUpdate();
		updateCustomEffects();
		if (inventory)
		{
			inventory->onUpdate();
		}

		if (m_wallet)
		{
			m_wallet->onUpdate();
		}

		if (m_ranch)
		{
			m_ranch->onUpdate(this);
		}

		if (m_birdSimulator)
		{
			m_birdSimulator->onUpdate(this);
		}

		if (itemInUse)
		{
			ItemStackPtr pStack = inventory->getCurrentItem();

			if (pStack == itemInUse)
			{
				if (itemInUseCount <= 25 && itemInUseCount % 4 == 0)
				{
					updateItemUse(pStack, 5);
				}

				if (--itemInUseCount <= 0 && world->m_isClient)
				{
					onItemUseFinish();
					m_isUsingItemByBtn = false;
				}
			}
			else
			{
				clearItemInUse();
				m_isUsingItemByBtn = false;
			}
		}

		if (xpCooldown > 0)
		{
			--xpCooldown;
		}

		if (isPlayerSleeping())
		{
			++sleepTimer;

			if (sleepTimer > 100)
			{
				sleepTimer = 100;
			}

			if (!world->m_isClient)
			{
				if (!isInBed())
				{
					wakeUpPlayer(true, true, false);
				}
				else if (world->isDaytime())
				{
					wakeUpPlayer(false, true, true);
				}
			}
		}
		else if (sleepTimer > 0)
		{
			++sleepTimer;

			if (sleepTimer >= 110)
			{
				sleepTimer = 0;
			}
		}

		EntityLivingBase::onUpdate();

		if (!world->m_isClient && openContainer != NULL && !openContainer->canInteractWith(this))
		{
			closeScreen();
			openContainer = inventoryContainer;
		}

		if (isBurning() && (capabilities.disableDamage || capabilities.isWatchMode))
		{
			extinguish();
		}

		prevChasingPosX = chasingPosX;
		prevChasingPosY = chasingPosY;
		prevChasingPosZ = chasingPosZ;
		float var9 = position.x - chasingPosX;
		float var3 = position.y - chasingPosY;
		float var5 = position.z - chasingPosZ;
		float var7 = 10.0f;

		if (var9 > var7)
			prevChasingPosX = chasingPosX = position.x;

		if (var5 > var7)
			prevChasingPosZ = chasingPosZ = position.y;

		if (var3 > var7)
			prevChasingPosY = chasingPosY = position.z;

		if (var9 < -var7)
			prevChasingPosX = chasingPosX = position.x;

		if (var5 < -var7)
			prevChasingPosZ = chasingPosZ = position.z;

		if (var3 < -var7)
			prevChasingPosY = chasingPosY = position.y;

		chasingPosX += var9 * 0.25f;
		chasingPosZ += var5 * 0.25f;
		chasingPosY += var3 * 0.25f;
		addStat(StatList::minutesPlayedStat, 1);

		if (ridingEntity == NULL)
		{
			startMinecartRidingCoordinate = Vector3i::INVALID;
		}

		if (!world->m_isClient)
		{
			foodStats->onUpdate(this);
		}
	}

	void EntityPlayer::playSound(SoundType soundType)
	{
		world->playSoundToNearExceptByType(this, soundType);
	}

	void EntityPlayer::updateItemUse(ItemStackPtr pStack, int amount)
	{
		if (pStack->getItemUseAction() == ACTION_TYPE_DRINK)
		{
			playSound(ST_Drink);
		}

		if (pStack->getItemUseAction() == ACTION_TYPE_RELOAD)
		{
			playSound(ST_Eat);
		}

		if (pStack->getItemUseAction() == ACTION_TYPE_EAT)
		{
			for (int i = 0; i < amount; ++i)
			{
				String paritcleName = StringUtil::Format("iconcrack_%d", pStack->getItem()->itemID);
				for (int i = 0; i < amount; ++i)
				{
					Vector3 var4((rand->nextFloat() - 0.5f) * 0.1f, Math::UnitRandom() * 0.1f + 0.1f, 0.0f);
					var4.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var4.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var4.y += 0.05f;
					Vector3 var5((rand->nextFloat() - 0.5f) * 0.3f, (-rand->nextFloat()) * 0.6f - 0.3f, 0.6f);
					var5.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var5.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var5 = var5 + position;
					var5.y += getEyeHeight();
					world->spawnParticle(paritcleName, var5, var4);
				}
			}

			playSound(ST_Eat);
		}
	}

	void EntityPlayer::onItemUseFinish()
	{
		if (itemInUse)
		{
			updateItemUse(itemInUse, 16);
			int size = itemInUse->stackSize;

			auto action = itemInUse->getItemUseAction();
			if (world->m_isClient) {
				NETWORK_DEFINE::C2SPacketUseItemUseType type = action == ACTION_TYPE_USE_COMMON_ITEM ?
					NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_COMMON : NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_EASTABLE;

				BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendUseItem(type);
			}
			clearItemInUse();
		}
	}

	bool EntityPlayer::isMovementBlocked()
	{
		// do not block the movement when move in watch mode, add by oujie 
		if (capabilities.isWatchMode)
			return false;

		return getHealth() <= 0.0F || isPlayerSleeping();
	}

	void EntityPlayer::mountEntity(Entity* pEntity)
	{
		if (ridingEntity && pEntity)
		{
			if (!world->m_isClient)
			{
				dismountEntity(ridingEntity);
			}

			if (ridingEntity)
			{
				ridingEntity->riddenByEntity = NULL;
			}

			ridingEntity = NULL;
		}
		else
		{
			Entity::mountEntity(pEntity);
		}
	}

	void EntityPlayer::updateRidden()
	{
		if (!world->m_isClient && isSneaking())
		{
			mountEntity(NULL);
			setSneaking(false);
		}
		else
		{
			Vector3 pos = position;
			float yaw = rotationYaw;
			float pitch = rotationPitch;
			EntityLivingBase::updateRidden();
			prevCameraYaw = cameraYaw;
			cameraYaw = 0.0F;
			pos = position - pos;
			addMountedMovementStat(pos);

			// todo.
			/*if (ridingEntity instanceof EntityPig)
			{
				rotationPitch = pitch;
				rotationYaw = yaw;
				renderYawOffset = ((EntityPig)ridingEntity).renderYawOffset;
			}*/
		}
	}

	void EntityPlayer::preparePlayerToSpawn()
	{
		yOffset = 1.62F;
		setSize(0.6F, 1.8F);
		Entity::preparePlayerToSpawn();
		setEntityHealth(getMaxHealth());
		deathTime = 0;
	}

	void EntityPlayer::updateEntityActionState()
	{
		EntityLivingBase::updateEntityActionState();
		updateArmSwingProgress();
	}

	void EntityPlayer::onLivingUpdate()
	{
		if (flyToggleTimer > 0)
		{
			--flyToggleTimer;
		}

		if (world->m_difficultySetting == 0 &&
			getHealth() < getMaxHealth() &&
			/* world->getGameRules().getGameRuleBooleanValue("naturalRegeneration") && */ false && // todo.
			ticksExisted % 20 * 12 == 0)
		{
			heal(1.0F);
		}

		inventory->decrementAnimations();
		prevCameraYaw = cameraYaw;
		auto beforeMovePosition = position;
		EntityLivingBase::onLivingUpdate();
		IAttributeInstance* att_movespeed = attributeMap->getAttributeInstance(SharedMonsterAttributes::MOVEMENT_SPEED);

		if (!world->m_isClient)
		{
			att_movespeed->setBaseValue(capabilities.getWalkSpeed());
		}

		jumpMovementFactor = speedInAir;

		if (isSprinting())
		{
			jumpMovementFactor = jumpMovementFactor + speedInAir * 0.3f;
		}

		setAIMoveSpeed(att_movespeed->getAttributeValue());
		float fMoveX = position.x - beforeMovePosition.x;
		float fMoveZ = position.z - beforeMovePosition.z;
		float fMoveY = position.y - beforeMovePosition.y;
		float montionsqr_xz = Math::Sqrt(fMoveX * fMoveX + fMoveZ * fMoveZ);
		float montion_y = Math::ATan(-fMoveY * 0.2f) * 15.0F;

		if (montionsqr_xz > 0.1F)
		{
			montionsqr_xz = 0.1F;
		}

		if (!onGround || getHealth() <= 0.0F)
		{
			montionsqr_xz = 0.0F;
		}

		if (onGround || getHealth() <= 0.0F)
		{
			montion_y = 0.0F;
		}

		cameraYaw += (montionsqr_xz - cameraYaw) * 0.4F;
		cameraPitch += (montion_y - cameraPitch) * 0.8F;

		if (getHealth() > 0.0F && !capabilities.isWatchMode)
		{
			Box aabb;

			if (ridingEntity && !ridingEntity->isDead)
			{
				aabb = boundingBox.unionCopy(ridingEntity->boundingBox).expland(1.2f, 0.0f, 1.2f);
			}
			else
			{
				aabb = boundingBox.expland(1.2f, 0.7f, 1.2f);
			}

			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, aabb);

			for (int i = 0; i < int(entities.size()); ++i)
			{
				Entity* pentity = entities[i];

				if (!pentity->isDead)
				{
					collideWithPlayer(pentity);
				}
			}
		}

		if ((onGround && capabilities.isParachute)|| (isInWater() && capabilities.isParachute) || (this->position.y <= 0 && capabilities.isParachute)) {
			parachuteEnd();
		}

	}

	EntityVehicle * EntityPlayer::getVehicle()
	{
		EntityVehicle* pVehicle = dynamic_cast<EntityVehicle*>(world->getEntity(m_vehicleEntityId));
		return pVehicle;
	}

	bool EntityPlayer::takeOnVehicle(EntityVehicle* vehicle, bool bForce, bool bIsDriver)
	{
		if (isOnVehicle())
			return false;

		if (!vehicle)
			return false;

		if (!bForce)
		{
			bool hasDriver = vehicle->m_hasDriver;
			if (hasDriver && vehicle->getFreeSeatNum() <= 0)
				return false;

			if (hasDriver && vehicle->m_bisLocked)
				return false;

			if (!hasDriver && !vehicle->canDrivedBy(this))
				return false;
		}

		if (!vehicle->carryPassenger(this->entityId, bIsDriver))
			return false;

		m_vehicleEntityId = vehicle->entityId;
		m_isDriver = vehicle->isDriver(this->entityId);
		onGround = true;
		fallDistance = 0;
		return true;
	}

	bool EntityPlayer::takeOffVehicle()
	{
		EntityVehicle* pVehicle = getVehicle();
		if (pVehicle == NULL || !pVehicle->getOffPassenger(this->entityId))
			return false;

		auto pos = getLeaveVehiclePos();
		setPosition(pos);
		m_vehicleEntityId = 0;
		m_isDriver = false;
		onGround = true;
		fallDistance = 0;
		return true;
	}

	Vector3 EntityPlayer::getLeaveVehiclePos()
	{
		Vector3i pos;
		int blockId = 0;
		bool bSucc = false;
		for (int i = 1; i <= 5; i++)
		{
			pos = Vector3i{ (int)(position.x + i), (int)position.y, (int)position.z };
			blockId = world->getBlockId(pos);
			if (blockId != 0)
			{
				bSucc = true;
				break;
			}

			pos = Vector3i{ (int)(position.x - i), (int)position.y, (int)position.z };
			blockId = world->getBlockId(pos);
			if (blockId != 0)
			{
				bSucc = true;
				break;
			}

			pos = Vector3i{ (int)(position.x), (int)(position.y + i), (int)position.z };
			blockId = world->getBlockId(pos);
			if (blockId != 0)
			{
				bSucc = true;
				break;
			}

			pos = Vector3i{ (int)(position.x), (int)(position.y), (int)(position.z + i) };
			blockId = world->getBlockId(pos);
			if (blockId != 0)
			{
				bSucc = true;
				break;
			}

			pos = Vector3i{ (int)(position.x), (int)(position.y), (int)(position.z - i) };
			blockId = world->getBlockId(pos);
			if (blockId != 0)
			{
				bSucc = true;
				break;
			}
		}

		if (bSucc)
			return Vector3((Real)pos.x, (Real)pos.y, (Real)pos.z);
		else
			return position + Vector3(0, 0.5f, 0);
	}

	void EntityPlayer::addOwnVehicle(int vehicleId)
	{
		m_ownVehicle.insert(vehicleId);
	}

	bool EntityPlayer::hasOwnVehicle(int vehicleId)
	{
		auto iter = m_ownVehicle.find(vehicleId);
		return iter != m_ownVehicle.end();
	}

	void EntityPlayer::resetOwnvehicle(const vector<int>::type& vehicles)
	{
		m_ownVehicle.clear();
		for (auto iter = vehicles.cbegin(); iter != vehicles.cend(); iter++)
		{
			m_ownVehicle.insert(*iter);
		}
	}

	EntityAircraft * EntityPlayer::getAircraft()
	{
		EntityAircraft* pAircraft = dynamic_cast<EntityAircraft*>(world->getEntity(m_aircraftEntityId));
		return pAircraft;
	}

	bool EntityPlayer::takeOnAircraft(EntityAircraft * pAircraft)
	{
		if(isOnAircraft() || pAircraft == NULL)
			return false;

		if (!pAircraft->addPassenger(this))
			return false;

		m_aircraftEntityId = pAircraft->entityId;
		onGround = false;
		return true;
	}

	bool EntityPlayer::takeOffAircraft()
	{
		if (!isOnAircraft())
			return false;

		EntityAircraft* m_aircraft = getAircraft();
		if (m_aircraft == NULL || !m_aircraft->getOffPassenger(this->entityId))
			return false;

		auto pos = m_aircraft->position + m_aircraft->m_passengerEye;
		setPosition(pos);
		m_aircraftEntityId = 0;
		startParachute();
		return true;
	}

	bool EntityPlayer::startParachute()
	{
		capabilities.isParachute = true;
		return true;
	}

	bool EntityPlayer::parachuteEnd()
	{
		capabilities.isParachute = false;
		return true;
	}

	void EntityPlayer::addBackpackCapacity(int capacity)
	{
		if (m_backpack)
			m_backpack->addCapacity(capacity);
	}

	void EntityPlayer::subBackpackCapacity(int capacity)
	{
		if (m_backpack)
			m_backpack->subCapacity(capacity);
	}

	void EntityPlayer::resetBackpack(int capacity, int maxCapacity)
	{
		if (m_backpack)
		{
			m_backpack->setMaxCapacity(maxCapacity);
			m_backpack->setCapacity(capacity);
		}
	}

	bool EntityPlayer::tryLockVehicle(bool state)
	{
		if (!isDriver())
			return false;

		EntityVehicle* pVehicle = getVehicle();
		if (pVehicle == NULL || !pVehicle->lockVehicle(this, state))
			return false;

		return true;
	}

	void EntityPlayer::onCapabilitieUpdate()
	{
		float time = world->getFrameDeltaTime() * 1000;
		if (isFrozen())
		{
			capabilities.onFrozenUpdate(time);
			if (!isFrozen())
			{
				m_outLooksChanged = true;
				m_setEnableMovement = true;
			}
		}
		capabilities.onHurtUpdate(time);
		if (isColorful())
		{
			capabilities.onColorfulUpdate(time);
			if (!isColorful())
			{
				m_outLooksChanged = true;
				m_setEnableMovement = true;
			}
		}
		if (isDisableMove())
		{
			capabilities.disableMoveUpdate(time);
			if (!isDisableMove())
			{
				m_setEnableMovement = true;
			}
		}
	}

	void EntityPlayer::collideWithPlayer(Entity* pEntity)
	{
		pEntity->onCollideWithPlayer(this);
	}

	void EntityPlayer::onDeath(DamageSource* pSource)
	{
		EntityLivingBase::onDeath(pSource);
		setSize(0.2F, 0.2F);
		setPosition(position);
		motion.y = 0.1f;

		if (username == "Notch")
		{
			dropPlayerItemWithRandomChoice(LORD::make_shared<ItemStack>(Item::appleRed, 1), true);
		}

		// todo.
		/*if (!world->getGameRules().getGameRuleBooleanValue("keepInventory"))
		{
			inventory->dropAllItems();
		}*/
		String damageType = "";
		if (pSource)
		{
			damageType = pSource->getDamageType();
		}
		if (SCRIPT_EVENT::EntityDeathDropItemEvent::invoke(hashCode(), damageType))
		{
			inventory->dropAllItems();
		}
		
		if (pSource != NULL)
		{
			motion.x = (-Math::Cos((attackedAtYaw + rotationYaw) * Math::PI / 180.0F) * 0.1F);
			motion.z = (-Math::Sin((attackedAtYaw + rotationYaw) * Math::PI / 180.0F) * 0.1F);
		}
		else
		{
			motion.x = motion.z = 0.0f;
		}

		// yOffset = 0.1F;
		addStat(StatList::deathsStat, 1);
	}

	void EntityPlayer::addToPlayerScore(Entity* pEntity, int scoreToAdd)
	{
		addScore(scoreToAdd);
		EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(pEntity);
		if (pPlayer)
		{
			addStat(StatList::playerKillsStat, 1);
		}
		else
		{
			addStat(StatList::mobKillsStat, 1);
		}
		/*Collection var3 = getWorldScoreboard().func_96520_a(ScoreObjectiveCriteria.field_96640_e);

		if (pEntity instanceof EntityPlayer)
		{
			addStat(StatList.playerKillsStat, 1);
			var3.addAll(getWorldScoreboard().func_96520_a(ScoreObjectiveCriteria.field_96639_d));
		}
		else
		{
			addStat(StatList.mobKillsStat, 1);
		}

		Iterator var4 = var3.iterator();

		while (var4.hasNext())
		{
			ScoreObjective var5 = (ScoreObjective)var4.next();
			Score var6 = getWorldScoreboard().func_96529_a(getEntityName(), var5);
			var6.func_96648_a();
		}*/
	}

	EntityItem* EntityPlayer::dropOneItem(bool par1)
	{
		return dropPlayerItemWithRandomChoice(inventory->decrStackSize(inventory->getInventoryIndexOfCurrentItem(), par1 && inventory->getCurrentItem() ? inventory->getCurrentItem()->stackSize : 1), false);
	}

	EntityItem* EntityPlayer::dropPlayerItemWithRandomChoice(ItemStackPtr pStack, bool fromPlayer)
	{
		if (!pStack)
		{
			return NULL;
		}
		else if (pStack->stackSize == 0)
		{
			return NULL;
		}
		else if (world->m_isClient)
		{
			return NULL;
		}
		else if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pStack->itemID, pStack->itemDamage, "player.drop"))
		{
			return NULL;
		}
		float f2 = 0.3F;
		EntityItem* pResult = LordNew EntityItem(world, position, pStack);
		pResult->position.y = pResult->position.y - 0.3f;
		pResult->delayBeforeCanPickup = 40;
		
		if (fromPlayer)
		{
			float f = world->m_Rand.nextFloat() * 0.5f;
			float f1 = world->m_Rand.nextFloat() * ((float)Math::PI * 2.0f);
			pResult->motion.x = static_cast<Real>(-sin(f1) * f);
			pResult->motion.z = static_cast<Real>(cos(f1) * f);
			pResult->motion.y = 0.20000000298023224;
		}
		else
		{
			pResult->motion.x = -sin(rotationYaw * 0.017453292F) * cos(rotationPitch * 0.017453292F) * f2;
			pResult->motion.z = cos(rotationYaw * 0.017453292F) *cos(rotationPitch * 0.017453292F) * f2;
			pResult->motion.y = -sin(rotationPitch * 0.017453292F) * f2 + 0.1F;

			float f3 = world->m_Rand.nextFloat() * ((float)Math::PI * 2.0f);
			f2 = 0.02F * world->m_Rand.nextFloat();
			pResult->motion.x += cos(f3) * f2;
			pResult->motion.y += (world->m_Rand.nextFloat() - world->m_Rand.nextFloat()) * 0.1F;
			pResult->motion.z += sin(f3) * f2;
		}
		world->spawnEntityInWorld(pResult);
		return NULL;

		// todo.
		/*EntityItem var3 = new EntityItem(worldObj, posX, posY - 0.30000001192092896D + (double)getEyeHeight(), posZ, pStack);
		var3.delayBeforeCanPickup = 40;
		float var4 = 0.1F;
		float var5;

		if (fromPlayer)
		{
			var5 = rand.nextFloat() * 0.5F;
			float var6 = rand.nextFloat() * (float)Math.PI * 2.0F;
			var3.motionX = (double)(-MathHelper.sin(var6) * var5);
			var3.motionZ = (double)(MathHelper.cos(var6) * var5);
			var3.motionY = 0.20000000298023224D;
		}
		else
		{
			var4 = 0.3F;
			var3.motionX = (double)(-MathHelper.sin(rotationYaw / 180.0F * (float)Math.PI) * MathHelper.cos(rotationPitch / 180.0F * (float)Math.PI) * var4);
			var3.motionZ = (double)(MathHelper.cos(rotationYaw / 180.0F * (float)Math.PI) * MathHelper.cos(rotationPitch / 180.0F * (float)Math.PI) * var4);
			var3.motionY = (double)(-MathHelper.sin(rotationPitch / 180.0F * (float)Math.PI) * var4 + 0.1F);
			var4 = 0.02F;
			var5 = rand.nextFloat() * (float)Math.PI * 2.0F;
			var4 *= rand.nextFloat();
			var3.motionX += Math.cos((double)var5) * (double)var4;
			var3.motionY += (double)((rand.nextFloat() - rand.nextFloat()) * 0.1F);
			var3.motionZ += Math.sin((double)var5) * (double)var4;
		}

		joinEntityItemWithWorld(var3);
		addStat(StatList.dropStat, 1);
		return var3;*/
	}

	void EntityPlayer::joinEntityItemWithWorld(EntityItem* pItem)
	{
		world->spawnEntityInWorld(pItem);
	}

	float EntityPlayer::getCurrentPlayerStrVsBlock(Block* pBlock, bool par2)
	{
		float fResult = inventory->getStrVsBlock(pBlock);

		if (fResult > 1.0F)
		{
			int lvl = EnchantmentHelper::getEfficiencyModifier(this);
			ItemStackPtr pStack = inventory->getCurrentItem();

			if (lvl > 0 && pStack)
			{
				float var6 = (float)(lvl * lvl + 1);

				if (!pStack->canHarvestBlock(pBlock) && fResult <= 1.0F)
				{
					fResult += var6 * 0.08F;
				}
				else
				{
					fResult += var6;
				}
			}
		}

		if (isPotionActive(Potion::digSpeed))
		{
			fResult *= 1.0F + (float)(getActivePotionEffect(Potion::digSpeed)->getAmplifier() + 1) * 0.2F;
		}

		if (isPotionActive(Potion::digSlowdown))
		{
			fResult *= 1.0F - (float)(getActivePotionEffect(Potion::digSlowdown)->getAmplifier() + 1) * 0.2F;
		}

		if (isInsideOfMaterial(BM_Material::BM_MAT_water) && !EnchantmentHelper::getAquaAffinityModifier(this))
		{
			fResult /= 5.0F;
		}

		return fResult;
	}

	bool EntityPlayer::canHarvestBlock(Block* pBlock)
	{
		return inventory->canHarvestBlock(pBlock);
	}

	void EntityPlayer::readEntityFromNBT(NBTTagCompound* pNbtCompound)
	{
		EntityLivingBase::readEntityFromNBT(pNbtCompound);
		NBTTagList* pNbtInventory = pNbtCompound->getTagList("Inventory");
		inventory->readFromNBT(pNbtInventory);
		// TODO: meaning of currentItemIndex is changed
		//inventory->currentItemIndex = pNbtCompound->getInteger("SelectedItemSlot");
		sleeping = pNbtCompound->getBool("Sleeping");
		sleepTimer = pNbtCompound->getShort("SleepTimer");
		experience = pNbtCompound->getFloat("XpP");
		experienceLevel = pNbtCompound->getInteger("XpLevel");
		experienceTotal = pNbtCompound->getInteger("XpTotal");
		setScore(pNbtCompound->getInteger("Score"));

		if (sleeping)
		{
			bedLocation.x = int(Math::Floor(position.x));
			bedLocation.y = int(Math::Floor(position.y));
			bedLocation.z = int(Math::Floor(position.z));
			wakeUpPlayer(true, true, false);
		}

		if (pNbtCompound->hasKey("SpawnX") && pNbtCompound->hasKey("SpawnY") && pNbtCompound->hasKey("SpawnZ"))
		{
			spawnChunk.x = pNbtCompound->getInteger("SpawnX");
			spawnChunk.y = pNbtCompound->getInteger("SpawnY");
			spawnChunk.z = pNbtCompound->getInteger("SpawnZ");
			spawnForced = pNbtCompound->getBool("SpawnForced");
		}

		foodStats->readNBT(pNbtCompound);
		capabilities.readCapabilitiesFromNBT(pNbtCompound);

		if (pNbtCompound->hasKey("EnderItems"))
		{
			NBTTagList* pNbtEnderItems = pNbtCompound->getTagList("EnderItems");
			theInventoryEnderChest->loadInventoryFromNBT(pNbtEnderItems);
		}
	}

	void EntityPlayer::writeEntityToNBT(NBTTagCompound* pNbtCompound)
	{
		EntityLivingBase::writeEntityToNBT(pNbtCompound);
		pNbtCompound->setTag("Inventory", inventory->writeToNBT(new NBTTagList()));
		//pNbtCompound->setInteger("SelectedItemSlot", inventory->currentItemIndex);
		pNbtCompound->setBool("Sleeping", sleeping);
		pNbtCompound->setShort("SleepTimer", (i16)sleepTimer);
		pNbtCompound->setFloat("XpP", experience);
		pNbtCompound->setInteger("XpLevel", experienceLevel);
		pNbtCompound->setInteger("XpTotal", experienceTotal);
		pNbtCompound->setInteger("Score", getScore());

		if (spawnChunk != Vector3i::INVALID)
		{
			pNbtCompound->setInteger("SpawnX", spawnChunk.x);
			pNbtCompound->setInteger("SpawnY", spawnChunk.y);
			pNbtCompound->setInteger("SpawnZ", spawnChunk.z);
			pNbtCompound->setBool("SpawnForced", spawnForced);
		}

		foodStats->writeNBT(pNbtCompound);
		capabilities.writeCapabilitiesToNBT(pNbtCompound);
		pNbtCompound->setTag("EnderItems", theInventoryEnderChest->saveInventoryToNBT());
	}

	bool EntityPlayer::attckedByPlayer(EntityPlayer* attackerPlayer, float amount)
	{
		if (getHealth() <= 0.0F)
		{
			LordLogInfo("attacked by fail, health <= 0");
			return false;
		}

		if (amount <= 0.0F)
		{
			return false;
		}

		float hp = getHealth();
		setEntityHealth(hp - amount);
		if (amount >= hp && amount >= 15)
		{
			LordLogInfo("====setEntityHealth====EntityPlayer::attckedByPlayer current hp %f, amount %f", hp, amount);
		}
		LordLogInfo("attack succ, damage=%f, origin health=%f, now=%f", amount, hp, getHealth());
		return true;
	}


	bool EntityPlayer::attackEntityFrom(DamageSource* pSource, float amount)
	{
		if (isParachute())
		{
			return false;
		}
		if (isEntityInvulnerable())
		{
			return false;
		}
		else if (capabilities.disableDamage && !pSource->canHarmInCreative())
		{
			return false;
		}
		else if (capabilities.isWatchMode)
		{
			return false;
		}
		else
		{
			entityAge = 0;

			if (getHealth() <= 0.0F)
			{
				return false;
			}
			else
			{
				if (isPlayerSleeping() && !world->m_isClient)
				{
					wakeUpPlayer(true, true, false);
				}

				if (pSource->isDifficultyScaled())
				{
					if (world->m_difficultySetting == 0)
					{
						amount = 0.0F;
					}

					if (world->m_difficultySetting == 1)
					{
						amount = amount / 2.0F + 1.0F;
					}

					if (world->m_difficultySetting == 3)
					{
						amount = amount * 3.0F / 2.0F;
					}
				}

				if (amount == 0.0F && !world->knockbackNoHarm())
				{
					return false;
				}
				
				Entity* pEntity = pSource->getEntity();
				EntityArrow* pArrow = dynamic_cast<EntityArrow*>(pEntity);
				if (pArrow && pArrow->getShootingEntity())
				{
					pEntity = pArrow->getShootingEntity();
				}

				addStat(StatList::damageTakenStat, int(Math::Round(amount * 10.0F)));
				return EntityLivingBase::attackEntityFrom(pSource, amount);
			}
		}
	}

	void EntityPlayer::damageArmor(float amount)
	{
		inventory->damageArmor(amount);
	}

	int EntityPlayer::getTotalArmorValue()
	{
		return inventory->getTotalArmorValue();
	}

	void EntityPlayer::damageEntity(DamageSource* pSource, float amount)
	{
		if (isEntityInvulnerable())
			return;

		if (!pSource->isUnblockable() && isBlocking() && amount > 0.0F)
		{
			amount = (1.0F + amount) * 0.5F;
		}

		amount = applyArmorCalculations(pSource, amount);
		amount = applyPotionDamageCalculations(pSource, amount);
		amount = Math::Clamp(amount, 0.f, getHealth());

		if (amount != 0.0F)
		{
			float lastHealth = getHealth();
			setEntityHealth(lastHealth - amount);
			if (amount >= lastHealth && amount >= 15)
			{
				LordLogInfo("====setEntityHealth====EntityPlayer::damageEntity current hp %f, amount %f DamageSource %s", 
				lastHealth, amount, pSource->damageType.c_str());
			}
			/*
			comment by wanglei
			reason: not used now and cause server crash because entity in damageSource destroyed but still referece
			combatTracker->trackDamage(pSource, lastHealth, amount);
			*/
			
			addExhaustion(pSource->getHungerDamage());
		}
	}

	bool EntityPlayer::interactWith(Entity* pEntity)
	{
		ItemStackPtr pEquippedItem = getCurrentEquippedItem();
		ItemStackPtr pEquippedItem2 = pEquippedItem ? pEquippedItem->copy() : NULL;

		if (!pEntity->processInitialInteract(this))
		{
			EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pEntity);
			if (pEquippedItem && pLiving)
			{
				if (capabilities.isCreativeMode)
				{
					pEquippedItem = pEquippedItem2;
				}

				if (pEquippedItem->interactWithEntity(this, pLiving))
				{
					if (pEquippedItem->stackSize <= 0 && !capabilities.isCreativeMode)
					{
						destroyCurrentEquippedItem();
					}

					return true;
				}
			}

			return false;
		}
		else
		{
			if (pEquippedItem && pEquippedItem == getCurrentEquippedItem())
			{
				if (pEquippedItem->stackSize <= 0 && !capabilities.isCreativeMode)
				{
					destroyCurrentEquippedItem();
				}
				else if (pEquippedItem->stackSize < pEquippedItem2->stackSize && capabilities.isCreativeMode)
				{
					pEquippedItem->stackSize = pEquippedItem2->stackSize;
				}
			}

			return true;
		}
	}

	ItemStackPtr EntityPlayer::getCurrentEquippedItem()
	{
		return inventory->getCurrentItem();
	}

	void EntityPlayer::destroyCurrentEquippedItem()
	{
		inventory->setInventorySlotContents(inventory->getInventoryIndexOfCurrentItem(), NULL);
	}

	bool EntityPlayer::simpleAttackPlayerWithCurrentItem(EntityPlayer* pTargetPlayer) {
		if (!pTargetPlayer->canAttackWithItem())
			return false;
		if (pTargetPlayer->hitByEntity(this))
			return false;

		float damage = attributeMap->getAttributeInstance(SharedMonsterAttributes::ATTACK_DAMAGE)->getAttributeValue();
		float magicExtra = 0.0F;

		EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pTargetPlayer);
		if (pLiving)
		{
			magicExtra = EnchantmentHelper::getEnchantmentModifierLiving(this, pLiving);
			EnchantmentThorns::damageArmor(this, pLiving, rand);
		}

		if (damage > 0.0F || magicExtra > 0.0F)
		{
			bool flag = fallDistance > 0.0F && !onGround && !isOnLadder() && !isInWater() && !isPotionActive(Potion::blindness) && !ridingEntity && pLiving;

			if (flag && damage > 0.0F)
			{
				damage *= 1.5F;
			}

			damage += magicExtra;

			return pTargetPlayer->attckedByPlayer(this, damage);
		}

		return false;
	}

	bool EntityPlayer::attackTargetEntityWithCurrentItem(Entity* pTarget)
	{
		if (!pTarget->canAttackWithItem())
			return false;
		if (pTarget->hitByEntity(this))
			return false;

		float damage = attributeMap->getAttributeInstance(SharedMonsterAttributes::ATTACK_DAMAGE)->getAttributeValue();
		int knockback = 0;
		float magicExtra = 0.0F;

		EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pTarget);
		if (pLiving)
		{
			magicExtra = EnchantmentHelper::getEnchantmentModifierLiving(this, pLiving);
			knockback += EnchantmentHelper::getKnockbackModifier(this, pLiving);
			EnchantmentThorns::damageArmor(this, pLiving, rand);
		}

		if (isSprinting())
		{
			++knockback;
		}

		if (damage > 0.0F || magicExtra > 0.0F)
		{
			bool flag = fallDistance > 0.0F && !onGround && !isOnLadder() && !isInWater() && !isPotionActive(Potion::blindness) && !ridingEntity && pLiving;

			if (flag && damage > 0.0F)
			{
				damage *= 1.5F;
			}

			damage += magicExtra;
			bool var6 = false;
			int var7 = 0;
			int poison = 0;
			
			bool attackSuc = pTarget->attackEntityFrom(DamageSource::causePlayerDamage(this), damage);

			if (attackSuc)
			{
				var7 = EnchantmentHelper::getFireAspectModifier(this);
				poison = EnchantmentHelper::getPoisonModifier(this);

				if (knockback > 0)
				{
					EntityLivingBase *liveEntity = dynamic_cast<EntityLivingBase*>(pTarget);
					if (nullptr != liveEntity)
					{
						liveEntity->knockBack(this, (float)knockback * 0.5F,
							Math::Sin(rotationYaw * Math::DEG2RAD),
							-Math::Cos(rotationYaw * Math::DEG2RAD));
					}
					else
					{
						Vector3 vec;
						vec.x = (-Math::Sin(rotationYaw * Math::DEG2RAD) * (float)knockback * 0.5F);
						vec.y = 0.1;
						vec.z = (Math::Cos(rotationYaw * Math::DEG2RAD) * (float)knockback * 0.5F);
						pTarget->addVelocity(vec);
					}

					//motionX *= 0.6D;  why?
					//motionZ *= 0.6D;
					setSprinting(false);
				}

				if (flag)
				{
					onCriticalHit(pTarget);
				}

				if (magicExtra > 0.0F)
				{
					onEnchantmentCritical(pTarget);
				}

			}

			if (pLiving && var7 > 0 && !pTarget->isBurning())
			{
				var6 = true;
				pTarget->setFire(1);
			}

			ItemStackPtr pWeapon = getCurrentEquippedItem();
			EntityLivingBase *pLiveEntity = dynamic_cast<EntityLivingBase*>(pTarget);

			if (nullptr != pLiveEntity)
			{
				if (pWeapon != NULL)
				{
					if (SCRIPT_EVENT::HitEntityWithCurrentItem::invoke(pWeapon->itemID, pWeapon->itemDamage, pLiveEntity->hashCode(), hashCode()))
					{
						pWeapon->hitEntity(pLiveEntity, this);

						if (pWeapon->stackSize <= 0)
						{
							destroyCurrentEquippedItem();
						}
					}
				}
	

				if (var7 > 0)
				{
					pTarget->setFire(var7 * 4);
				}
				else if (var6)
				{
					pTarget->extinguish();
				}

				if (poison > 0)
				{
					auto effect = LordNew PotionEffect(19, poison * 90);
					pLiveEntity->addPotionEffect(effect); 
				}
			}

			addExhaustion(0.3F);
		}

		return true;
	}

	void EntityPlayer::setDead()
	{
		Entity::setDead();
		inventoryContainer->onContainerClosed(this);

		if (openContainer)
		{
			openContainer->onContainerClosed(this);
		}
	}

	bool EntityPlayer::isEntityInsideOpaqueBlock()
	{
		return !sleeping && Entity::isEntityInsideOpaqueBlock();
	}

	SLEEP_RESULT EntityPlayer::sleepInBedAt(const BlockPos& pos)
	{
		if (!world->m_isClient)
		{
			if (isPlayerSleeping() || !isEntityAlive())
			{
				return SLEEP_RESULT_OTHER_PROBLEM;
			}

			if (!world->m_provider->isSurfaceWorld())
			{
				return SLEEP_RESULT_NOT_POSSIBLE_HERE;
			}

			if (world->isDaytime())
			{
				return SLEEP_RESULT_NOT_POSSIBLE_NOW;
			}

			if (Math::Abs(position.x - pos.x) > 3.0f || Math::Abs(position.y - pos.y) > 2.0f || Math::Abs(position.z - pos.z) > 3.0f)
			{
				return SLEEP_RESULT_TOO_FAR_AWAY;
			}

			Vector3 bound(8.f, 5.f, 8.f);
			EntityArr mobs = world->getEntitiesWithinAABB(ENTITY_CLASS_MOB, Box(Vector3(pos) - bound, Vector3(pos) + bound));

			if (!mobs.empty())
			{
				return SLEEP_RESULT_NOT_SAFE;
			}
		}

		if (isRiding())
		{
			mountEntity(NULL);
		}

		setSize(0.2F, 0.2F);
		yOffset = 0.2F;

		Vector3 off(0.5f, 0.9375F, 0.5f);
		if (world->blockExists(pos))
		{
			int metadata = world->getBlockMeta(pos);
			int dir = BlockBed::getDirection(metadata);

			switch (dir)
			{
			case 0: off.z = 0.9F; break;
			case 1: off.x = 0.1F; break;
			case 2: off.z = 0.1F; break;
			case 3:	off.x = 0.9F; break;
			}
			setRenderOffsetForSleep(dir);
		}
		setPosition(Vector3(pos) + off);

		sleeping = true;
		sleepTimer = 0;
		bedLocation = pos;
		motion = Vector3::ZERO;

		if (!world->m_isClient)
		{
			// world->updateAllPlayersSleepingFlag();
		}

		return SLEEP_RESULT_OK;
	}

	void EntityPlayer::setRenderOffsetForSleep(int dir)
	{
		renderBedOffset.x = 0.f;
		renderBedOffset.z = 0.f;

		switch (dir)
		{
		case 0:
			renderBedOffset.z = -1.8F;
			break;

		case 1:
			renderBedOffset.x = 1.8F;
			break;

		case 2:
			renderBedOffset.z = 1.8F;
			break;

		case 3:
			renderBedOffset.x = -1.8F;
		}
	}

	void EntityPlayer::wakeUpPlayer(bool immediately, bool updateWorld, bool setSpawn)
	{
		setSize(0.6F, 1.8F);
		resetHeight();
		Vector3i bed1 = bedLocation;
		Vector3i bed2 = bedLocation;

		if (bed1 != Vector3i::INVALID && world->getBlockId(bed1) == BLOCK_ID_BED)
		{
			BlockBed::setBedOccupied(world, bed1, false);
			bed2 = BlockBed::getNearestEmptyChunkCoordinates(world, bed1, 0);

			if (bed2 == Vector3i::INVALID)
			{
				bed2 = Vector3i(bed1.x, bed1.y + 1, bed1.z);
			}

			setPosition(Vector3(bed2) + Vector3(0.5f, yOffset + 0.1f, 0.5f));
		}

		sleeping = false;

		if (!world->m_isClient && updateWorld)
		{
			// todo.
			// world->updateAllPlayersSleepingFlag();
		}

		if (immediately)
		{
			sleepTimer = 0;
		}
		else
		{
			sleepTimer = 100;
		}

		if (setSpawn)
		{
			setSpawnChunk(bedLocation, false);
		}
	}

	bool EntityPlayer::isInBed()
	{
		return world->getBlockId(bedLocation) == BLOCK_ID_BED;
	}

	Vector3i EntityPlayer::verifyRespawnCoordinates(World* pWorld, const Vector3i& respawnPos, bool par2)
	{
		auto pChunkService = pWorld->getChunkService();
		pChunkService->getChunk((respawnPos.x - 3) >> 4, (respawnPos.z - 3) >> 4);
		pChunkService->getChunk((respawnPos.x + 3) >> 4, (respawnPos.z - 3) >> 4);
		pChunkService->getChunk((respawnPos.x - 3) >> 4, (respawnPos.z + 3) >> 4);
		pChunkService->getChunk((respawnPos.x + 3) >> 4, (respawnPos.z + 3) >> 4);

		if (pWorld->getBlockId(respawnPos) == BLOCK_ID_BED)
		{
			Vector3i result = BlockBed::getNearestEmptyChunkCoordinates(pWorld, respawnPos, 0);
			return result;
		}
		else
		{
			const BM_Material& matlower = pWorld->getBlockMaterial(respawnPos);
			const BM_Material& matupper = pWorld->getBlockMaterial(respawnPos);
			bool galleryful_lower = !matlower.isSolid() && !matlower.isLiquid();
			bool galleryful_upper = !matupper.isSolid() && !matupper.isLiquid();
			return par2 && galleryful_lower && galleryful_upper ? respawnPos : Vector3i::INVALID;
		}
	}

	float EntityPlayer::getBedOrientationInDegrees()
	{
		if (bedLocation != Vector3i::INVALID)
		{
			int metadata = world->getBlockMeta(bedLocation);
			int dir = BlockBed::getDirection(metadata);

			switch (dir)
			{
			case 0:
				return 90.0F;

			case 1:
				return 0.0F;

			case 2:
				return 270.0F;

			case 3:
				return 180.0F;
			}
		}

		return 0.0F;
	}

	void EntityPlayer::setHideCape(int flag, bool cape)
	{
		if (cape)
			data_hide_caps = data_hide_caps | (1 << flag);
		else
			data_hide_caps = data_hide_caps & ~(1 << flag);
	}

	void EntityPlayer::setSpawnChunk(const Vector3i& chunkCoord, bool spawn)
	{
		if (chunkCoord != Vector3i::INVALID)
		{
			spawnChunk = chunkCoord;
			spawnForced = spawn;
		}
		else
		{
			spawnChunk = Vector3i::INVALID;
			spawnForced = false;
		}
	}

	void EntityPlayer::jump()
	{
		EntityLivingBase::jump();
		addStat(StatList::jumpStat, 1);

		if (isSprinting())
		{
			addExhaustion(0.8F);
		}
		else
		{
			addExhaustion(0.2F);
		}
	}

	void EntityPlayer::moveEntity(const Vector3 & vel)
	{
		float prevX = position.x;
		float prevZ = position.z;
		float prevY = position.y;

		// add by zhouyou. for debug movment and collision.
		//do_Entity_moveEntity_with_debgu_info(vel);

		if (isOnVehicle() || isOnAircraft())
			return;

		Entity::moveEntity(vel);
		updateAutoJump(position.x - prevX, position.z - prevZ);
	}

	void EntityPlayer::moveEntityWithHeading(float strafe, float forward)
	{
		if (isOnAircraft() || isOnVehicle())
		{
			fallDistance = 0.0f;
			motion = Vector3::ZERO;
			return;
		}

		Vector3 pos = position;

		if (capabilities.isFlying && !ridingEntity)
		{
			float motiony = motion.y;
			float jumpFactor = jumpMovementFactor;
			jumpMovementFactor = capabilities.getFlySpeed();
			EntityLivingBase::moveEntityWithHeading(strafe, forward);
			motion.y = motiony * 0.6f;
			jumpMovementFactor = jumpFactor;
			motion.y = 0.0f;
		}
		else
		{
			EntityLivingBase::moveEntityWithHeading(strafe, forward);
		}

		pos = position - pos;
		addMovementStat(pos);
	}

	float EntityPlayer::getAIMoveSpeed()
	{
		return attributeMap->getAttributeInstance(SharedMonsterAttributes::MOVEMENT_SPEED)->getAttributeValue();
	}

	void EntityPlayer::addMovementStat(const Vector3& pos)
	{
		if (ridingEntity)
			return;

		int amount;

		if (isInsideOfMaterial(BM_Material::BM_MAT_water))
		{
			amount = int(Math::Round(pos.len() * 100.0F));

			if (amount > 0)
			{
				addStat(StatList::distanceDoveStat, amount);
				addExhaustion(0.015F * (float)amount * 0.01F);
			}
		}
		else if (isInWater())
		{
			amount = int(Math::Round(Math::Sqrt(pos.x * pos.x + pos.z * pos.z) * 100.0F));

			if (amount > 0)
			{
				addStat(StatList::distanceSwumStat, amount);
				addExhaustion(0.015F * (float)amount * 0.01F);
			}
		}
		else if (isOnLadder())
		{
			if (pos.y > 0.0f)
			{
				addStat(StatList::distanceClimbedStat, (int)Math::Round(pos.y * 100.0f));
			}
		}
		else if (onGround)
		{
			amount = int(Math::Round(Math::Sqrt(pos.x * pos.x + pos.z * pos.z) * 100.0F));

			if (amount > 0)
			{
				addStat(StatList::distanceWalkedStat, amount);

				if (isSprinting())
				{
					addExhaustion(0.099999994F * (float)amount * 0.01F);
				}
				else
				{
					addExhaustion(0.01F * (float)amount * 0.01F);
				}
			}
		}
		else
		{
			amount = int(Math::Round(Math::Sqrt(pos.x * pos.x + pos.z * pos.z) * 100.0F));

			if (amount > 25)
			{
				addStat(StatList::distanceFlownStat, amount);
			}
		}
	}

	void EntityPlayer::addMountedMovementStat(const Vector3& pos)
	{
		if (!ridingEntity)
			return;


		int amount = int(Math::Round(pos.len() * 100.0F));

		if (amount > 0)
		{
			/*if (ridingEntity instanceof EntityMinecart)
			{
				addStat(StatList.distanceByMinecartStat, amount);

				if (startMinecartRidingCoordinate == null)
				{
					startMinecartRidingCoordinate = new ChunkCoordinates(MathHelper.floor_double(posX), MathHelper.floor_double(posY), MathHelper.floor_double(posZ));
				}
				else if ((double)startMinecartRidingCoordinate.getDistanceSquared(MathHelper.floor_double(posX), MathHelper.floor_double(posY), MathHelper.floor_double(posZ)) >= 1000000.0D)
				{
					addStat(AchievementList.onARail, 1);
				}
			}
			else if (ridingEntity instanceof EntityBoat)
			{
				addStat(StatList.distanceByBoatStat, amount);
			}
			else if (ridingEntity instanceof EntityPig)
			{
				addStat(StatList.distanceByPigStat, amount);
			}*/
		}
	}

	void EntityPlayer::updateFallState(float y, bool onGroundIn)
	{
		EntityLivingBase::updateFallState(y, onGroundIn);
		if (capabilities.isFlying)
		{
			fallDistance = 0;
		}
	}

	void EntityPlayer::fall(float distance)
	{
		if (!capabilities.allowFlying && !capabilities.isWatchMode)
		{
			if (distance >= 2.0F)
			{
				addStat(StatList::distanceFallenStat, (int)Math::Round(distance * 100.0f));
			}

			EntityLivingBase::fall(distance);
		}
	}

	void EntityPlayer::onKillEntity(EntityLivingBase* pLiving)
	{
		// todo.
		//if (pLiving->isClass(ENTITY_CLASS_MOB))
		//{
		//	// triggerAchievement(AchievementList::killEnemy);
		//}
	}

	void EntityPlayer::setInWeb()
	{
		if (!capabilities.isFlying)
		{
			Entity::setInWeb();
		}
	}

	ItemStackPtr EntityPlayer::getCurrentArmor(int slot)
	{
		return inventory->armorItemInSlot(slot);
	}

	void EntityPlayer::addExperience(int exp)
	{
		addScore(exp);
		int safeBound = Math::MAX_INT - experienceTotal;

		if (exp > safeBound)
		{
			exp = safeBound;
		}

		experience += (float)exp / (float)xpBarCap();

		for (experienceTotal += exp; experience >= 1.0F; experience /= (float)xpBarCap())
		{
			experience = (experience - 1.0F) * (float)xpBarCap();
			addExperienceLevel(1);
		}
	}

	void EntityPlayer::addExperienceLevel(int lvl)
	{
		experienceLevel += lvl;

		if (experienceLevel < 0)
		{
			experienceLevel = 0;
			experience = 0.0F;
			experienceTotal = 0;
		}

		if (lvl > 0 && experienceLevel % 5 == 0 && (float)lastXPSound < (float)ticksExisted - 100.0F)
		{
			float var2 = experienceLevel > 30 ? 1.0F : (float)experienceLevel / 30.0F;
			//world->playSoundAtEntity(this, "random.levelup", var2 * 0.75F, 1.0F);
			world->playSoundAtEntityByType(this, ST_LevelUp);
			lastXPSound = ticksExisted;
		}
	}

	int EntityPlayer::xpBarCap()
	{
		return experienceLevel >= 30 ? 62 + (experienceLevel - 30) * 7 : (experienceLevel >= 15 ? 17 + (experienceLevel - 15) * 3 : 17);
	}

	void EntityPlayer::addExhaustion(float amount)
	{
		if (!capabilities.disableDamage && !capabilities.isWatchMode)
		{
			if (!world->m_isClient)
			{
				foodStats->addExhaustion(amount);
			}
		}
	}

	bool EntityPlayer::canEat(bool checkFood)
	{
		return (checkFood || foodStats->needFood()) && !capabilities.disableDamage && !capabilities.isWatchMode;
	}

	void EntityPlayer::setItemInUse(ItemStackPtr pStack, int duration)
	{
		if (pStack != itemInUse)
		{
			itemInUse = pStack;
			itemInUseCount = duration;

			if (!world->m_isClient && pStack->getItemUseAction() == ACTION_TYPE_EAT)
			{
				setEating(true);
			}
		}
	}

	bool EntityPlayer::isCurrentToolAdventureModeExempt(const BlockPos& pos)
	{
		if (capabilities.allowEdit)
			return true;

		int blockid = world->getBlockId(pos);

		if (blockid > 0)
		{
			Block* pBlock = BlockManager::sBlocks[blockid];

			if (pBlock->getMaterial().isAdventureModeExempt())
			{
				return true;
			}

			if (getCurrentEquippedItem())
			{
				ItemStackPtr pStack = getCurrentEquippedItem();

				if (pStack->canHarvestBlock(pBlock) || pStack->getStrVsBlock(pBlock) > 1.0F)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool EntityPlayer::canPlayerEdit(const BlockPos& pos, int face, ItemStackPtr pStack)
	{
		if (capabilities.isWatchMode)
		{
			return false;
		}
		if (capabilities.allowEdit)
		{
			return true;
		}
		if (!pStack)
		{
			return false;
		}
		return pStack->canEditBlocks();
	}

	int EntityPlayer::getExperiencePoints(EntityPlayer* pPlayer)
	{
		// todo.
		if (/*world->getGameRules().getGameRuleBooleanValue("keepInventory")*/ false)
		{
			return 0;
		}
		else
		{
			int var2 = experienceLevel * 7;
			return var2 > 100 ? 100 : var2;
		}
	}

	void EntityPlayer::clonePlayer(EntityPlayer* pPlayer, bool all)
	{
		if (all)
		{
			inventory->copyInventory(*pPlayer->inventory);
			setEntityHealth(pPlayer->getHealth());
			foodStats = pPlayer->foodStats;
			experienceLevel = pPlayer->experienceLevel;
			experienceTotal = pPlayer->experienceTotal;
			experience = pPlayer->experience;
			setScore(pPlayer->getScore());
			// teleportDirection = pPlayer.teleportDirection;
		}
		// todo.
		/*
		else if (world->getGameRules().getGameRuleBooleanValue("keepInventory"))
		{
			inventory->copyInventory(*pPlayer->inventory);
			experienceLevel = pPlayer->experienceLevel;
			experienceTotal = pPlayer->experienceTotal;
			experience = pPlayer->experience;
			setScore(pPlayer->getScore());
		}
		*/
		theInventoryEnderChest = pPlayer->theInventoryEnderChest;
	}

	ItemStackPtr EntityPlayer::getCurrentItemOrArmor(int armorType)
	{
		return armorType == 0 ? inventory->getCurrentItem() : inventory->armorInventory[armorType - 1];
	}

	ItemStackPtr EntityPlayer::getHeldItem()
	{
		return inventory->getCurrentItem();
	}

	void EntityPlayer::setCurrentItemOrArmor(int armorType, ItemStackPtr pArmor)
	{
		inventory->armorInventory[armorType] = pArmor;
	}

	ItemStackArr EntityPlayer::getLastActiveItems()
	{
		return ItemStackArr(inventory->armorInventory, inventory->armorInventory + InventoryPlayer::ARMOR_INVENTORY_COUNT);
	}

	bool EntityPlayer::canAttackPlayer(EntityPlayer* pPlayer)
	{
		return true;
	}

	float EntityPlayer::getArmorVisibility()
	{
		int count = 0;

		for (int i = 0; i < InventoryPlayer::ARMOR_INVENTORY_COUNT; ++i)
		{
			ItemStackPtr pStack = inventory->armorInventory[i];

			if (pStack)
			{
				++count;
			}
		}

		return (float)count / (float)InventoryPlayer::ARMOR_INVENTORY_COUNT;
	}

	bool EntityPlayer::isInvisibleToPlayer(EntityPlayer* pPlayer)
	{
		if (!isInvisible())
		{
			return false;
		}
		else
		{
			// todo.
			/*Team var2 = getTeam();
			return var2 == null || pPlayer == null || pPlayer.getTeam() != var2 || !var2.func_98297_h();*/
			return true;
		}
	}

	void EntityPlayer::setAbsorptionAmount(float amount)
	{
		if (amount < 0.0F)
		{
			amount = 0.0F;
		}

		data_absorption = amount;
	}

	void EntityPlayer::changeMaxHealth(float health)
	{
		this->getEntityAttribute(SharedMonsterAttributes::EAMAX_HEALTH)->setBaseValue(health);
		float maxHealth = getMaxHealth();
		this->setEntityHealth(maxHealth);
		this->data_Health = maxHealth;
	}

	void EntityPlayer::setAllowFlying(bool allow)
	{
		capabilities.allowFlying = allow;
		if (!capabilities.canBeFlying())
		{
			capabilities.isFlying = false;
		}
	}

	void EntityPlayer::setWatchMode(bool isWatchMode)
	{
		if (!capabilities.isWatchMode && isWatchMode)
		{
			capabilities.isFlying = true;
			if (onGround)
			{
				motion.y = 0.1f;
			}
		}
		capabilities.isWatchMode = isWatchMode;
		if (!capabilities.canBeFlying())
		{
			capabilities.isFlying = false;
		}
	}

	bool EntityPlayer::setCameraLocked(bool bLocked)
	{
		m_isCameraLocked = bLocked;
		m_isNeedChangePostion = true;
		if (bLocked)
		{
			m_originCameraLockPos = position;
			m_cameraLockPos = position;
			m_prevCameraLockPos = position;
			m_cameraLockYaw = rotationYaw;
			m_cameraLockPitch = rotationPitch;
		}
		else
		{
			position = m_originCameraLockPos;
			lastTickPos = m_originCameraLockPos;
			prevPos = m_originCameraLockPos;
			rotationYaw = m_cameraLockYaw;
			rotationPitch = m_cameraLockPitch;
		}
		return true;
	}

	SoundType EntityPlayer::getHurtSoundType()
	{
		if (!isZombie())
		{
			return m_sex == 1 ? ST_MalePlayerBeHit : ST_FemalePlayerBeHit;
		}
		else
		{
			return ST_MonsterWound;
		}
	}

	SoundType EntityPlayer::getDeathSoundType()
	{
		if (m_isPeopleActor)
		{
			return m_sex == 1 ? ST_MalePlayerBeHit : ST_FemalePlayerBeHit;
		}
		else
		{
			return ST_MonsterDead;
		}
	}

	Vector3 EntityPlayer::getForward()
	{
		float f = Math::Cos(-rotationPitch * Math::DEG2RAD - Math::PI);
		float f1 = Math::Sin(-rotationPitch * Math::DEG2RAD - Math::PI);
		float f2 = -Math::Cos(-rotationYaw * Math::DEG2RAD);
		float f3 = Math::Sin(-rotationYaw * Math::DEG2RAD);
		return Vector3(f1 * f2, f3, f * f2);
	}

	void EntityPlayer::clearCustomEffects()
	{
		m_custom_effects.clear();
	}

	void EntityPlayer::addCustomEffect(String name, String effectName, float duration)
	{
		for (auto& custom_effect : m_custom_effects)
		{
			if (custom_effect.m_name == name)
			{
				custom_effect.m_duration = duration;
				return;
			}
		}
		CustomEffect effect = CustomEffect();
		effect.m_name = name;
		effect.m_effect_name = effectName;
		effect.m_duration = duration;
		m_custom_effects.push_back(effect);
	}

	void EntityPlayer::updateCustomEffects()
	{
		float time = world->getFrameDeltaTime() * 1000;
		for (auto iter = m_custom_effects.begin(); iter != m_custom_effects.end(); )
		{
			auto effect = *iter;
			if (effect.m_status == CustomEffectStatus::Delete)
			{
				iter = m_custom_effects.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		for (auto& custom_effect : m_custom_effects)
		{
			if (custom_effect.m_status == CustomEffectStatus::Running)
			{
				custom_effect.m_duration -= time;
				if (custom_effect.m_duration < 0.0f)
				{
					custom_effect.m_duration = 0.0f;
					custom_effect.m_status = CustomEffectStatus::Unload;
				}
			}
		}
	}
	
	void EntityPlayer::changeClothes(String partName, int actorId)
	{
		if (partName == "custom_hair") {
			this->m_hairID = actorId;
		}
		else if (partName == "custom_face") {
			this->m_faceID = actorId;
		}
		else if (partName == "clothes_tops") {
			this->m_topsID = actorId;
		}
		else if (partName == "clothes_pants") {
			this->m_pantsID = actorId;
		}
		else if (partName == "custom_shoes") {
			this->m_shoesID = actorId;
		}
		else if (partName == "custom_glasses") {
			this->m_glassesId = actorId;
		}
		else if (partName == "custom_scarf") {
			this->m_scarfId = actorId;
		}
		else if (partName == "custom_wing") {
			this->m_wingId = actorId;
		}
		else if (partName == "custom_hat") {
			this->m_hatId = actorId;
		}
		else if (partName == "custom_decorate_hat") {
			this->m_decorate_hatId = actorId;
		}
		else if (partName == "custom_hand") {
			this->m_armId = actorId;
		}
		else if (partName == "custom_tail")
		{
			this->m_tailId = actorId;
		}
		else if (partName == "custom_wing_flag") {
			this->m_extra_wingId = actorId;
		}
		else if (partName == "custom_foot_halo") {
			this->m_footHaloId = actorId;
		}
		else if (partName == "custom_back_effect")
		{
			this->m_backEffectId = actorId;
		}
		else if (partName == "custom_crown")
		{
			this->m_crownId = actorId;
		}
		else if (partName == "custom_head_effect")
		{
			this->m_headEffectId = actorId;
		}
		else if (partName == "custom_bag")
		{
			this->m_bagId = actorId;
		}
		this->m_isClothesChange = true;
	}

	void EntityPlayer::setOnFrozen(float mills)
	{
		bool frozen = isFrozen();
		capabilities.frozenTime = mills;
		if (isFrozen())
			m_outLooksChanged = m_outLooksChanged || !frozen;
		else
			m_outLooksChanged = m_outLooksChanged || frozen;
	}

	void EntityPlayer::setOnColorful(float mills)
	{
		bool colorful = isColorful();
		capabilities.colorfulTime = mills;
		if (isColorful())
			m_outLooksChanged = m_outLooksChanged || !colorful;
		else
			m_outLooksChanged = m_outLooksChanged || colorful;
	}

	void EntityPlayer::setDisableMove(float mills)
	{
		bool disableMove = isDisableMove();
		capabilities.disableMoveTime = mills;
	}

	void EntityPlayer::whenRemoved()
	{
		if (isOnAircraft())
		{
			takeOffAircraft();
		}

		if (isOnVehicle())
		{
			takeOffVehicle();
		}
	}  

	void EntityPlayer::applyVehicleMovementInput()
	{
		moveStrafing = 0.0f;
		moveForward = 0.0f;

		if (movementInput == nullptr)
			return;

		if (isDriver())
		{
			EntityVehicle* m_vehicle = getVehicle();
			if (m_vehicle != NULL)
			{
				m_vehicle->m_moveForward = movementInput->moveForward;
				if (m_vehicle->m_moveForward > 0.0f)
					m_vehicle->m_moveForward = 1.0f;
				else if (m_vehicle->m_moveForward < 0.0f)
					m_vehicle->m_moveForward = -1.0f;

				float deltaYaw = Math::Mod(rotationYaw - m_vehicle->rotationYaw, 360.f);
				if (deltaYaw < 0.f)
					deltaYaw += 360.f;
				float criticalYaw = 2.f;
				if (deltaYaw > criticalYaw && deltaYaw < 180.f)
					m_vehicle->m_moveStrafing = -1.f;
				else if (deltaYaw > 180.f && deltaYaw - 360.f < -criticalYaw)
					m_vehicle->m_moveStrafing = 1.f;
				else
					m_vehicle->m_moveStrafing = 0.f;
			}
		}
	}

	void EntityPlayer::applyMovementInput()
	{
		if (movementInput == nullptr)
			return;

		if (isOnVehicle())
		{
			applyVehicleMovementInput();
		}
		else if (!isOnAircraft())
		{
			moveStrafing = 0.0f;
			moveForward = 0.0f;
			if (canMovement())
			{
				moveStrafing = movementInput->moveStrafe;
				moveForward = movementInput->moveForward;
				if (isPlayerMoved())
				{
					movementTimes();
					recordOldPos();
				}
			}
		}

		bool bIsAutoJumping = false;
		isJumping = movementInput->jump;
		if (autoJumpTime > 0)
		{
			--autoJumpTime;
			isJumping = true;
			bIsAutoJumping = true;
		}

		if (capabilities.isFlying)
		{
			if (movementInput->down)
			{
				motion.y = -0.15f;
			}

			if (movementInput->jump && !bIsAutoJumping)
			{
				motion.y = 0.15f;
			}
		}
	}

	void EntityPlayer::setMovementInput(float forward, float strafe)
	{
		if (movementInput)
		{
			movementInput->moveForward = forward;
			movementInput->moveStrafe = strafe;
		}
	}

	void EntityPlayer::updateAutoJump(float deltaX, float deltaZ)
	{
		if (true/*isAutoJumpEnabled() && !capabilities.isParachute*/)
		{
			float strafe = movementInput->moveStrafe;
			float forward = movementInput->moveForward;

			if (autoJumpTime <= 0 && isInWater() && !isSneaking() && !isRiding())
			{
				if (strafe != 0.0F || forward != 0.0F)
					autoJumpTime = 1;
			}

			// if can cross the 1.0 height block.
			if (autoJumpTime <= 0 && onGround && !isSneaking() && !isRiding())
			{
				if (strafe != 0.0F || forward != 0.0F)
				{
					Vector3 vec3d = getBottomPos();
					float d0 = position.x + deltaX;
					float d1 = position.z + deltaZ;
					Vector3 vec3d1(d0, boundingBox.vMin.y, d1);
					Vector3 vec3d2(deltaX, 0.0f, deltaZ);
					float f = getAIMoveSpeed();
					float f1 = vec3d2.lenSqr();

					if (f1 <= 0.001F)
					{
						float f2 = f * strafe;
						float f3 = f * forward;
						float f4 = Math::Sin(rotationYaw * 0.017453292F);
						float f5 = Math::Cos(rotationYaw * 0.017453292F);
						vec3d2 = Vector3(f2 * f5 - f3 * f4, vec3d2.y, f3 * f5 + f2 * f4);
						f1 = vec3d2.lenSqr();

						if (f1 <= 0.001F)
						{
							return;
						}
					}

					float f12 = (float)Math::FastInvSqrt((double)f1);
					Vector3 vec3d12 = vec3d2 * f12;
					Vector3 vec3d13 = getForward();
					float f13 = vec3d13.x * vec3d12.x + vec3d13.z * vec3d12.z;

					if (f13 >= -0.15F)
					{
						BlockPos pos = Vector3(position.x, boundingBox.vMax.y, position.z).getFloor();
						int blockID = world->getBlockId(pos);
						Block* pblock = blockID > 0 ? BlockManager::sBlocks[blockID] : NULL;

						if (!pblock || pblock->getCollisionBoundingBox(world, pos).isNull())
						{
							pos.y++;
							int blockupID = world->getBlockId(pos);
							Block* pblockup = blockupID > 0 ? BlockManager::sBlocks[blockupID] : NULL;

							if (!pblockup || pblockup->getCollisionBoundingBox(world, pos).isNull())
							{
								float f6 = 7.0F;
								float f7 = 1.2F;

								if (isPotionActive(Potion::jump))
								{
									f7 += (float)(getActivePotionEffect(Potion::jump)->getAmplifier() + 1) * 0.75F;
								}

								float f8 = Math::Max(f * 7.0F, 1.0F / f12);
								Vector3 vec3d4 = vec3d1 + vec3d12 * f8;
								float f9 = width;
								float f10 = height;
								Box axisalignedbb = Box(vec3d, vec3d4 + Vector3(0.0f, f10, 0.0f)).expland(f9, 0.0f, f9);
								Vector3 lvt_19_1_ = vec3d + Vector3(0.0f, 0.51f, 0.0f);
								vec3d4 = vec3d4 + Vector3(0.0f, 0.51f, 0.0f);
								Vector3 vec3d5 = vec3d12.cross(Vector3::UNIT_Y);
								Vector3 vec3d6 = vec3d5 * (f9 * 0.5F);
								Vector3 vec3d7 = lvt_19_1_ - vec3d6;
								Vector3 vec3d8 = vec3d4 - vec3d6;
								Vector3 vec3d9 = lvt_19_1_ + vec3d6;
								Vector3 vec3d10 = vec3d4 + vec3d6;
								AABBList& aabbs = world->getCollidingBoundingBoxes(this, axisalignedbb);

								float f11 = Math::MIN_FLOAT;
								bool breakcodeFallow = false;

								for (AABBList::iterator it = aabbs.begin(); it != aabbs.end(); ++it)
								{
									Box& axisalignedbb2 = *it;

									if (axisalignedbb2.intersects(vec3d7, vec3d8) || axisalignedbb2.intersects(vec3d9, vec3d10))
									{
										f11 = axisalignedbb2.vMax.y;
										Vector3 vec3d11 = axisalignedbb2.getCenter();
										BlockPos pos2 = vec3d11.getFloor();
										int i = 1;

										while (true)
										{
											if ((float)i >= f7)
											{
												breakcodeFallow = true;
												break;
											}

											Vector3i pos3 = pos2;
											pos3.y += i;
											int blockID2 = world->getBlockId(pos3);
											Block* pBlock2 = blockID2 > 0 ? BlockManager::sBlocks[blockID2] : NULL;

											Box axisalignedbb1;
											if (pBlock2)
												axisalignedbb1 = pBlock2->getCollisionBoundingBox(world, pos3);

											if (!axisalignedbb1.isNull())
											{
												f11 = axisalignedbb1.vMax.y + pos3.y;

												if (f11 - boundingBox.vMin.y > f7)
												{
													return;
												}
											}

											if (i > 1)
											{
												pos.y++;
												int blockID3 = world->getBlockId(pos);
												Block* pBlock3 = blockID3 > 0 ? BlockManager::sBlocks[blockID3] : NULL;
												if (pBlock3 && !pBlock3->getCollisionBoundingBox(world, pos).isNull())
												{
													return;
												}
											}

											++i;
										}

										if (breakcodeFallow)
											break;
									}
								}

								if (f11 != Math::MIN_FLOAT)
								{
									float f14 = f11 - boundingBox.vMin.y;

									if (f14 > 0.5F && f14 <= f7)
									{
										autoJumpTime = 1;
									}
								}
							}
						}
					}
				}
			}
		}

	}

	void EntityPlayer::setIsJump(bool isJump)
	{
		if (movementInput)
		{
			movementInput->jump = isJump;
		}
	}

	void EntityPlayer::setIsDown(bool isDown)
	{
		if (movementInput)
		{
			movementInput->down = isDown;
		}
	}

	bool EntityPlayer::isPlayerMoved()
	{
		return true;
	}

	void EntityPlayer::setBagInfo(int curCapacity, int maxCapacity)
	{
		if (m_bagCapacity != curCapacity || m_bagMaxCapacity != maxCapacity)
		{
			m_isBagChange = true;
			m_bagCapacity = curCapacity;
			m_bagMaxCapacity = maxCapacity;
		}
	}

	void EntityPlayer::setSpeedAdditionLevel(int level)
	{
		if (level > 1000)
			level = 1000;
		float speedAddition = 0.2f * level / 1000;
		setSpeedAddition(speedAddition);
	}

	void EntityPlayer::addUnlockedCommodity(int unlockedId, int meta)
	{
		std::vector<int> temp;
		temp.clear();
		temp.push_back(unlockedId);
		temp.push_back(meta);
		if (find(m_unlockedCommodities.begin(), m_unlockedCommodities.end(), temp) == m_unlockedCommodities.end())
		{
			m_unlockedCommodities.push_back(temp);
		}
	}

	bool EntityPlayer::isUnlocked(int itemId, int meta)
	{
		for (auto iter : m_unlockedCommodities)
		{
			if (iter.size() == 2 && iter[0] == itemId && iter[1] == meta)
			{
				return true;
			}
		}
		return false;
	}

	bool EntityPlayer::canPlayAction()
	{
		if (m_isPeopleActor)
			return true;
		auto pGameRule = GameRuleSetting::getGameRule();
		return pGameRule->isActorAction;
	}

}
