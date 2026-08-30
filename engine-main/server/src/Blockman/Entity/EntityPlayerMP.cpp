#include "EntityPlayerMP.h"
#include "Entity/DamageSource.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "EntityTracker.h"

#include "World/World.h"
#include "World/WorldProvider.h"
#include "Inventory/Container.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryEnderChest.h"
#include "Stats/StatList.h"
#include "Stats/Stats.h"
#include "Inventory/Slot.h"
#include "Inventory/CoinManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "Item/PotionManager.h"
#include "Item/Items.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Blockman/World/ServerWorld.h"
#include "Util/LanguageKey.h"
#include "Script/GameServerEvents.h"
#include "Network/ClientPeer.h"
#include "Block/BlockManager.h"
#include "Entity/Entity.h"
#include "Setting/CarSetting.h"
#include "Setting/SkillSetting.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityItem.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Network/ManorRequest.h"

namespace BLOCKMAN
{
	
EntityPlayerMP::EntityPlayerMP(World* pWorld, const String& worldName, const Vector3i& spawnPos)
	: EntityPlayer(pWorld, worldName)
{
	doConstruct(spawnPos);
}

EntityPlayerMP::~EntityPlayerMP() 
{
	doDestruct();
}

void EntityPlayerMP::doConstruct(const Vector3i& spawnPos)
{
	oldPos = Vector3::ZERO;
	oldMinY = 0.f;
	managedPosX = 0.f;
	managedPosZ = 0.f;
	playerInventoryBeingManipulated = false;
	ping = 0;
	playerConqueredTheEnd = false;
	initialInvulnerability = 60;
	chatVisibility = false;
	chatColours = true;
	currentWindowId = 0;
	m_raknetID = 0;

	//stepHeight = 0.0F;
	yOffset = 0.0F;
	setLocationAndAngles(Vector3(spawnPos) + Vector3(0.5f, 0.f, 0.5f), 0.0F, 0.0F);
	while (!world->getCollidingBoundingBoxes(this, boundingBox).empty())
	{
		setPosition(position + Vector3::UNIT_Y);
	}

	monitorInventoryChanged();
	monitorPropertyChanged();

	// FIX [SYMPTOM-5]: initPlayerInventory() is a DEBUG care-package (~20 item
	// stacks) and was granted to every new player whenever no gameplay script
	// was loaded — which is always the case for the script-less in-process
	// server. Players now start with an EMPTY inventory (vanilla survival).
	// if (!Server::Instance()->isScriptLoaded()) {
	//      initPlayerInventory();
	// }

	// for debug, added by wanglei
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	//foodStats->setFoodLevel(10);
	data_Health = 20.0f;
#else
	data_Health = 20.0f;
#endif

	setGameType(Server::Instance()->getWorld()->getWorldInfo().getGameType());

	m_inventoryChanged = true;
	m_propertyChanged = true;
	m_itemInHandChanged = true;
	isDead = false;
	movementInput = LordNew MovementInput();
	mSynBlockContext = LordNew SyncBlockContext(LordNew SyncBlockStrategySection());
	m_bNeedSyncPosition = false;
	lastMoveEventInvokePos = position;
	m_movePlayerUpdateTick = 0;
}

void EntityPlayerMP::doDestruct()
{
	m_subscriptionGuard.unsubscribeAll();
	LordSafeDelete(movementInput);
	LordSafeDelete(mSynBlockContext);
}

void EntityPlayerMP::reconstruct(const Vector3i& spawnPos)
{
	EntityPlayerMP::doDestruct();
	EntityPlayer::doDestruct();
	EntityLivingBase::doDestruct();
	Entity::doDestruct();

	Entity::doConstruct();
	EntityLivingBase::doConstruct();
	EntityPlayer::doConstruct();
	EntityPlayerMP::doConstruct(spawnPos);
}

void EntityPlayerMP::addMoveSpeedPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::moveSpeed->getId(), seconds * 20));
}

void EntityPlayerMP::addJumpPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::jump->getId(), seconds * 20));
}

void EntityPlayerMP::addNightVisionPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::nightVision->getId(), seconds * 20));
}

void EntityPlayerMP::addPoisonPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::poison->getId(), seconds * 20));
}

void EntityPlayerMP::addEffect(int id, int seconds, int amplifier)
{
	if (id <= 0 || id > 23)
	{
		LordLogError("unsupported potion id=%d", id);
		return;
	}

	auto effect = LordNew PotionEffect(id, seconds * 20, amplifier);
	addPotionEffect(effect);
}

void EntityPlayerMP::removeEffect(int id)
{
	if (id < 0 || id > 23)
	{
		return;
	}
	removePotionEffect(id);
}

void EntityPlayerMP::clearEffects()
{
	clearActivePotions();
}

void EntityPlayerMP::setDead()
{
	EntityPlayer::setDead();
	leaveVehicle();
	takeOffAircraft();
}

void EntityPlayerMP::leaveVehicle()
{
	if (isOnVehicle())
	{
		int vehicleId = m_vehicleEntityId;
		EntityPlayer::takeOffVehicle();
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastSyncTakeVehicle(entityId, false, false, vehicleId);
	}
}

void EntityPlayerMP::specialJump(float yFactor, float xFactor, float zFactor)
{
	if (!isWatchMode())
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerSpecialJump(entityId, yFactor, xFactor, zFactor);
	}
}

void EntityPlayerMP::buy(const Commodity & commodity, bool isConsumeCoin, bool isAddGoods)
{
	if (isConsumeCoin && m_wallet)
	{
		int moneyId = CoinManager::Instance()->coinIdToItemId(commodity.coinId);
		if (moneyId >= 10000)
		{
			subCurrency(commodity.price);
			ManorRequest::trade(getPlatformUserId() , -commodity.price);
		}
		else
		{
			m_wallet->consumeCoin(commodity.coinId, commodity.price);
		}
	}

	if (isAddGoods)
	{
		addItem(commodity.itemId, commodity.itemNum, commodity.itemMeta);
		ServerNetwork::Instance()->getSender()->notifyGetItem(this->getRaknetID(), commodity.itemId, commodity.itemMeta, commodity.itemNum);
	}
}

void EntityPlayerMP::addItem(int id, int num, int damage, bool isReverse, int maxDamage)
{
	if (this->isDead)
		return;
	if (id >= 10000)
	{
		addCurrency(num);
		ManorRequest::trade(getPlatformUserId(), num);
		return;
	}
	Item* pItem = Item::getItemById(id);
	if (!pItem) 
	{
		auto pBlock = BlockManager::getBlockById(id);
		if (pBlock)
		{
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pBlock, num, damage);
			if (itemStack)
			{
				this->getInventory()->addItemStackToInventory(itemStack, isReverse);
			}
		}
		return;
	}
		

	if (maxDamage > 0)
	{
		pItem->setMaxDamage(maxDamage);
	}

	if (dynamic_cast<ItemAppIcon*>(pItem))
	{
		return;
	}
	
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		this->getInventory()->addItemStackToInventory(itemStack, isReverse);
	}
}

void EntityPlayerMP::replaceItem(int id, int num, int damage, int stackIndex, int clipBullet)
{
	if (this->isDead)
		return;

	if (id >= 10000)
	{
		addCurrency(num);
		ManorRequest::trade(getPlatformUserId(), num);
		return;
	}

	if (stackIndex < 0 || stackIndex >= InventoryPlayer::HOTBAR_COUNT)
		return;

	if (inventory == nullptr)
		return;

	ItemStackPtr oldStack = inventory->getItemIdBySlot(stackIndex);
	if (oldStack)
	{
		inventory->mainInventory[stackIndex] = nullptr;
		inventory->hotbar[stackIndex] = nullptr;
	}

	Item* pItem = Item::getItemById(id);
	if (!pItem)
		return;

	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		itemStack->m_needAdjustStackIndex = false;
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
		}

		inventory->mainInventory[stackIndex] = itemStack;
		inventory->hotbar[stackIndex] = itemStack;
	}
}

void EntityPlayerMP::addGunItem(int id, int num, int damage, int clipBullet, bool isReverse)
{
	if (this->isDead)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
	if (itemStack)
	{
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
			this->getInventory()->addItemStackToInventory(itemStack, isReverse);
		}
	}
}

void EntityPlayerMP::addItemToEnderChest(int slot, int id, int num, int damage)
{
	if (this->isDead)
		return;
	Item* pItem = Item::getItemById(id);
	if (!pItem)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		this->getInventoryEnderChest()->setInventorySlotContents(slot, itemStack);
	}
}

void EntityPlayerMP::addGunItemToEnderChest(int slot, int id, int num, int damage, int clipBullet)
{
	if (this->isDead)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
	if (itemStack)
	{
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
			this->getInventoryEnderChest()->setInventorySlotContents(slot, itemStack);
		}
	}
}

void EntityPlayerMP::addItemPotion(int index, int num)
{
	if (!this->isDead)
	{
		auto itemStack = LORD::make_shared<ItemStack>(PotionManager::getItemPotionByIndex(index), num, 0);
		this->inventory->addItemStackToInventory(itemStack);
	}
}

void EntityPlayerMP::addEchantmentItem(int id, int num, int damage, std::vector<std::vector<int>> enchantments)
{
	if (!this->isDead)
	{
		auto itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
		for (auto enchantment : enchantments)
		{
			itemStack->addEnchantmentbyId(enchantment[0], enchantment[1]);
		}
		this->inventory->addItemStackToInventory(itemStack);
	}
}

void EntityPlayerMP::initPlayerInventory()
{
	int idx = 0;
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::nest_01, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_LADDER, 64, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::fishingRod, 1, 0);
	/*inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::digShovelIron, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelRake, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelMarching, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelBigScooper, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelJackHammer, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelDrill, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelEngineerGold, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_DIRT, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_LADDER, 64, 0);*/

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::building_02, 50, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::house_01, 50, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::blockmanEmpty, 50, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(66, 50, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(28, 50, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_DIRT, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(171, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(185 + 256, 16, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::snowball, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::grenade, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::tpScroll_01, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::createBridge_01, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelRenovation, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelToy, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::rabbitCooked, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bow, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::fishCooked, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::sign, 16, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemSplashHeal, 1, 16384);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemSplashHeal, 1, 16384);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_TORCH_WOOD, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::snowball, 16, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::flintAndSteel, 1, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::tnt, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::cloth, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::arrowPoison, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::skull, 1, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::arrowLongPoison, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::arrowStrongPoison, 64, 0);

	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::arrow, 64, 0);

	/*inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_CHEST, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::sign, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_PISTON_BASE, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_PISTON_STICKY_BASE, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_TORCH_REDSTONE_ACTIVE, 64, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_TORCH_REDSTONE_ACTIVE, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_REDSTONE_LAMP_ACTIVE, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::doorWood, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_DIRT, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::flintAndSteel, 1, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_WATERSTILL, 64, 0);
	//inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_WATERMOVING, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bucketEmpty, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bucketWater, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemNightVision, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemLongNightVision, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemInvisibility, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemLongInvisibility, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemJump, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemLongJump, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemStrongJump, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemHeal, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemStrongHeal, 1, 0);*/
	/*inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemInvisibility, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemMoveSpeed, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemLongMoveSpeed, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemStrongMoveSpeed, 1, 0);*/
	/*inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::stick, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_PLANKS, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_COBBLE_STONE, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::leather, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::diamond, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::ingotIron, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::ingotGold, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::swordWood, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::porkCooked, 5, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::helmetDiamond, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::plateDiamond, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::legsDiamond, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bootsDiamond, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::helmetDiamond, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::plateChain, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::legsChain, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bootsChain, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::helmetChain, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::plateGold, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::legsGold, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bootsGold, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::helmetGold, 1, 0);*/

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::plateIron, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::legsIron, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bootsIron, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::helmetIron, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::plateLeather, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::legsLeather, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bootsLeather, 1, 0);


	if (idx > 36)
	{
		throw std::out_of_range("initializing mainInventory caused out of range error");
	}

	for (size_t i = 0; i < InventoryPlayer::HOTBAR_COUNT; ++i)
	{
		inventory->hotbar[i] = inventory->mainInventory[i];
	}
}

void EntityPlayerMP::monitorPropertyChanged()
{
	using namespace std::placeholders;
	m_subscriptionGuard.add(data_Health.subscribe(std::bind(&EntityPlayerMP::onPlayerHealthChanged, this, _1, _2)));
	m_subscriptionGuard.add(m_air.subscribe(std::bind(&EntityPlayerMP::onPlayerAirChanged, this, _1, _2)));
	m_subscriptionGuard.add(foodStats->foodLevelListenable().subscribe(std::bind(&EntityPlayerMP::onPlayerFoodLevelChanged, this, _1, _2)));
	m_subscriptionGuard.add(foodStats->saturationLevelListenable().subscribe(std::bind(&EntityPlayerMP::onPlayerFoodSaturationLevelChanged, this, _1, _2)));
}

bool EntityPlayerMP::onPlayerFoodSaturationLevelChanged(float old_saturationLevel, float new_saturationLevel)
{
	if (!Math::IsNearlyZero(old_saturationLevel - new_saturationLevel)) {
		m_propertyChanged = true;
	}

	return true;
}

bool EntityPlayerMP::onPlayerFoodLevelChanged(int old_foodLevel, int new_foodLevel)
{
	if (old_foodLevel != new_foodLevel) {
		m_propertyChanged = true;
	}
	return true;
}

bool EntityPlayerMP::onPlayerAirChanged(int old_air, int new_air)
{
	if (Math::Abs(old_air - new_air) >= 30) {
		m_propertyChanged = true;
	}
	return true;
}

bool EntityPlayerMP::onPlayerHealthChanged(float old_health, float new_health)
{
	if (!Math::IsNearlyZero(old_health - new_health)) {
		m_propertyChanged = true;
	}
	return true;
}

void EntityPlayerMP::monitorInventoryChanged()
{
	m_subscriptionGuard.add(inventory->onInventoryContentChanged([this] {
		m_inventoryChanged = true;
	}));
	
	m_subscriptionGuard.add(inventory->onItemInHandChanged([this] {
		m_itemInHandChanged = true;
		// attack logic read this
		this->attributeMap->removeAttributeModifiers(m_attackModifiers);
		m_attackModifiers.clear();
		if (inventory->getCurrentItem())
		{
			inventory->getCurrentItem()->getAttributeModifiers(m_attackModifiers);
			this->attributeMap->applyAttributeModifiers(m_attackModifiers);
		}
	}));
}

void EntityPlayerMP::knockBack(Entity* pEntity, float strength, float xRatio, float zRatio)
{
	if (rand->nextFloat() < getEntityAttribute(SharedMonsterAttributes::KNOCKBACK_RESISTANCE)->getAttributeValue())
		return;

	isAirBorne = true;
	float lensqr = Math::Sqrt(xRatio * xRatio + zRatio * zRatio);

	motion /= 2.0f;
	motion.x -= xRatio / lensqr * strength * 2;
	motion.y = 0;
	motion.z -= zRatio / lensqr * strength * 2;

	if (this->onGround && strength != 0.0f)
	{
		motion.y = 0.4f;
	}

	if (strength != 0.0f)
	{
		m_bNeedSyncPosition = true;
	}
}

void EntityPlayerMP::readEntityFromNBT(NBTTagCompound* pNbtCompound)
{
	EntityPlayer::readEntityFromNBT(pNbtCompound);

	if (pNbtCompound->hasKey("playerGameType"))
	{

	}
}

void EntityPlayerMP::writeEntityToNBT(NBTTagCompound* pNbtCompound)
{
	EntityPlayer::writeEntityToNBT(pNbtCompound);
	// pNbtCompound.setInteger("playerGameType", this.theItemInWorldManager.getGameType().getID());
}

void EntityPlayerMP::addExperienceLevel(int exp)
{
	EntityPlayer::addExperienceLevel(exp);
}

void EntityPlayerMP::addSelfToInternalCraftingInventory()
{
	openContainer->addCraftingToCrafters(this);
}

void EntityPlayerMP::onUpdate()
{
	try
	{
		EntityPlayer::onUpdate();
	}
	catch (const Exception &e)
	{
		LordLogError(e.getMessage().c_str());
	}

	auto sender = Server::Instance()->getNetwork()->getSender();
	// sync player inventory if changed
	if (m_inventoryChanged)
	{
		sender->sendPlayerInventory(m_raknetID, inventory);
		m_inventoryChanged = false;
		//LordLogInfo("sendPlayerInventory: player name: %s", getCommandSenderName().c_str());
	}
	// EntityPlayerMP will not call EntityPlayer::onUpdate() periodically, so update the wallet here to ensure the wallet is updated in every tick
	if (m_wallet)
	{
		m_wallet->onUpdate();
	}

	// sync player property if changed
	if (m_propertyChanged)
	{
		sender->sendEntityAttribute(this, this);
		sender->sendEntityAttributeToTrackingPlayers(this);
		m_propertyChanged = false;
	}

	if (m_itemInHandChanged)
	{
		if (inventory->getCurrentItem())
		{
			SCRIPT_EVENT::PlayerChangeItemInHandEvent::invoke(this->getRaknetID(), inventory->getCurrentItem()->itemID, inventory->getCurrentItem()->itemDamage);
		}
		else
		{
			SCRIPT_EVENT::PlayerChangeItemInHandEvent::invoke(this->getRaknetID(), 0, 0);
		}

		sender->sendChangeItemInHandToTrackingPlayers(entityId, inventory->getCurrentItem());
		m_itemInHandChanged = false;
	}

	if (m_lastTickIsFlying != capabilities.isFlying)
	{
		sender->sendSetFlyingToTrackingPlayers(this);
		m_lastTickIsFlying = capabilities.isFlying;
	}

	// theItemInWorldManager.updateBlockRemoving();
	--initialInvulnerability;
	openContainer->detectAndSendChanges();

	if (!world->m_isClient && !openContainer->canInteractWith(this))
	{
		closeScreen();
		openContainer = inventoryContainer;
	}

	if (m_isClothesChange)
	{
		sender->sendUserAttrFromEntityToTrackingPlayers(this);
		m_isClothesChange = false;
		m_isClothesChanged = true;
	}

	if (m_isPulled)
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerMovementWithMotion(entityId, true);
		m_isPulled = false;
	}

	if (m_remainingFlyTicksByPulled > 0)
	{
		m_remainingFlyTicksByPulled--;
		m_remainingFlyTicksByPulled = m_remainingFlyTicksByPulled > 1 ? m_remainingFlyTicksByPulled : 1;
		if (m_remainingFlyTicksByPulled == 1 && onGround)
		{
			m_remainingFlyTicksByPulled = 0;
			m_flyingByPulled = false;
		}
	}

	updateMoveEventInvoke();
	sendUpdateBlocks();

	checkForceMovement();
	forceBrocastPosition();
	/*while (!destroyedItemsNetCache.isEmpty())
	{
		int var1 = Math.min(destroyedItemsNetCache.size(), 127);
		int[] var2 = new int[var1];
		Iterator var3 = destroyedItemsNetCache.iterator();
		int var4 = 0;

		while (var3.hasNext() && var4 < var1)
		{
			var2[var4++] = ((Integer)var3.next()).intValue();
			var3.remove();
		}

		playerNetServerHandler.sendPacketToPlayer(new Packet29DestroyEntity(var2));
	}

	if (!loadedChunks.isEmpty())
	{
		ArrayList var6 = new ArrayList();
		Iterator var7 = loadedChunks.iterator();
		ArrayList var8 = new ArrayList();

		while (var7.hasNext() && var6.size() < 5)
		{
			ChunkCoordIntPair var9 = (ChunkCoordIntPair)var7.next();
			var7.remove();

			if (var9 != null && worldObj.blockExists(var9.chunkXPos << 4, 0, var9.chunkZPos << 4))
			{
				var6.add(worldObj.getChunkFromChunkCoords(var9.chunkXPos, var9.chunkZPos));
				var8.addAll(((WorldServer)worldObj).getAllTileEntityInBox(var9.chunkXPos * 16, 0, var9.chunkZPos * 16, var9.chunkXPos * 16 + 16, 256, var9.chunkZPos * 16 + 16));
			}
		}

		if (!var6.isEmpty())
		{
			playerNetServerHandler.sendPacketToPlayer(new Packet56MapChunks(var6));
			Iterator var10 = var8.iterator();

			while (var10.hasNext())
			{
				TileEntity var5 = (TileEntity)var10.next();
				sendTileEntityToPlayer(var5);
			}

			var10 = var6.iterator();

			while (var10.hasNext())
			{
				Chunk var11 = (Chunk)var10.next();
				getServerForPlayer().getEntityTracker().func_85172_a(this, var11);
			}
		}
	}*/
}

void EntityPlayerMP::triggerEntityUpdate()
{
	try
	{
		EntityPlayer::onUpdate();
	}
	catch (const Exception &e)
	{
		LordLogError(e.getMessage().c_str());
	}
}

void EntityPlayerMP::jump()
{
	EntityPlayer::jump();
}

void EntityPlayerMP::changeCurrentItem(int slot)
{
	if (slot >= 0 && slot < InventoryPlayer::HOTBAR_COUNT)
	{
		inventory->currentItemIndex = slot;
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->sendPlayerChangeItem(m_raknetID, slot);
	}
}

void EntityPlayerMP::moveEntity(const Vector3 & vel)
{
	EntityPlayer::moveEntity(vel);
}

void EntityPlayerMP::updateEntityActionState()
{
	if (isOnVehicle())
	{
		applyVehicleMovementInput();
	}

	if (!isOnAircraft() && !isOnVehicle())
	{
		EntityPlayer::updateEntityActionState();
	}
}

void EntityPlayerMP::onDeath(DamageSource* pSource)
{
	
	// mcServer.getConfigurationManager().sendChatMsg(func_110142_aN().func_94546_b());

	// todo.
	// if (!world->getGameRules().getGameRuleBooleanValue("keepInventory"))
	//{
	//	inventory->dropAllItems();
	//}

	/*Collection var2 = worldObj.getScoreboard().func_96520_a(ScoreObjectiveCriteria.field_96642_c);
	Iterator var3 = var2.iterator();	

	while (var3.hasNext())
	{
		ScoreObjective var4 = (ScoreObjective)var3.next();
		Score var5 = getWorldScoreboard().func_96529_a(getEntityName(), var4);
		var5.func_96648_a();
	}*/

	leaveVehicle();
	EntityLivingBase* pLiving = getAttackingEntity();

	if (pLiving)
	{
		if (SCRIPT_EVENT::PlayerDieEvent::invoke(this, true, dynamic_cast<EntityPlayerMP*>(pLiving)))
		{
			EntityPlayer::onDeath(pSource);
			pLiving->addToPlayerScore(this, scoreValue);
			this->isDead = true;
		}
	}
	else
	{
		if (SCRIPT_EVENT::PlayerDieEvent::invoke(this, false, reinterpret_cast<EntityPlayerMP*>(NULL)))
		{
			EntityPlayer::onDeath(pSource);
			this->isDead = true;
		}
	}

	if (this->isDead)
	{
		addStat(StatList::deathsStat, 1);
		Server::Instance()->getNetwork()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_DEATH, getEntityName(), 0);
		Server::Instance()->getNetwork()->getSender()->sendEntityAttribute(this, this);
		Server::Instance()->getNetwork()->getSender()->sendEntityAttributeToTrackingPlayers(this);

		setWatchMode(true);
	}
}

bool EntityPlayerMP::attackEntityFrom(DamageSource* pSource, float amount)
{
	//if (isOnVehicle() || isEntityInvulnerable())
	if (pSource == NULL || isEntityInvulnerable())
		return false;

	bool allowPVP = true;//mcServer.isDedicatedServer() && mcServer.isPVPEnabled() && "fall".equals(pSource.damageType);

	if (!allowPVP && initialInvulnerability > 0 && pSource != DamageSource::outOfWorld)
		return false;

	EntityDamageSource* pEntityDS = dynamic_cast<EntityDamageSource*>(pSource);
	EntityPlayerMP *attackFrom = nullptr;

	if (pEntityDS)
	{
		Entity* pEntity = pSource->getEntity();
		if (pEntity == NULL)
			return false;

		if (pEntity->isClass(ENTITY_CLASS_PLAYER))
		{
			attackFrom = dynamic_cast<EntityPlayerMP*>(pEntity);
			if (!canAttackPlayer((EntityPlayer*)pEntity))
			{
				return false;
			}
		}

		if (pEntity->isClass(ENTITY_CLASS_ARROW))
		{
			EntityArrow* pArrow = (EntityArrow*)pEntity;
			Entity* pShootingEntity = pArrow->getShootingEntity();
			if (pShootingEntity == NULL || (pShootingEntity->isClass(ENTITY_CLASS_PLAYER) && !canAttackPlayer((EntityPlayer*)pShootingEntity)))
			{
				return false;
			}
		}

		if (pEntity->isClass(ENTITY_CLASS_BULLET))
		{
			EntityBullet* pBullet = (EntityBullet*)pEntity;
			Entity* pShootingEntity = pBullet->getShootingEntity();
			if (pShootingEntity == NULL || (pShootingEntity->isClass(ENTITY_CLASS_PLAYER) && !canAttackPlayer((EntityPlayer*)pShootingEntity)))
			{
				return false;
			}
		}
	}
	FloatProxy floatProxy;
	floatProxy.value = amount;

	if (!SCRIPT_EVENT::PlayerAttackedEvent::invoke(this, attackFrom, pSource->getDamageType().c_str(), &floatProxy))
		return false;
	
	return EntityPlayer::attackEntityFrom(pSource, floatProxy.value);
}

bool EntityPlayerMP::canAttackPlayer(EntityPlayer* pPlayer)
{
	return /*!mcServer.isPVPEnabled() ? false :*/ EntityPlayer::canAttackPlayer(pPlayer);
}

void EntityPlayerMP::travelToDimension(int dimension)
{
	// todo.
}

void EntityPlayerMP::sendTileEntityToPlayer(TileEntity* pTileEntity)
{
	if (pTileEntity)
	{
		// todo.
		/*Packet var2 = pTileEntity->getDescriptionPacket();

		if (var2 != null)
		{
			playerNetServerHandler.sendPacketToPlayer(var2);
		}*/
	}
}

int EntityPlayerMP::onEnchantmentModifier(Entity* target, int enchantmentModifier)
{

	FloatProxy floatProxy;
	floatProxy.value = 0;
	auto targetPlayer = dynamic_cast<EntityPlayerMP*>(target);
	if (!targetPlayer)
	{
		return enchantmentModifier;
	}

	if (!SCRIPT_EVENT::PlayerAttackedEvent::invoke(targetPlayer, this, "inFire", &floatProxy))
	{
		return 0;
	}
	return enchantmentModifier;
}

void EntityPlayerMP::addCustomEffect(String name, String effectName, float duration)
{
	EntityPlayer::addCustomEffect(name, effectName, duration);
	ServerNetwork::Instance()->getSender()->sendAddPlayerCustomEffect(this->entityId, name, effectName, duration);
}

void EntityPlayerMP::updateCustomEffects()
{
	for (auto& custom_effect : m_custom_effects)
	{
		if (custom_effect.m_status == CustomEffectStatus::Create)
		{
			custom_effect.m_status = CustomEffectStatus::Running;
		}
	}
	EntityPlayer::updateCustomEffects();
	for (auto& custom_effect : m_custom_effects)
	{
		if (custom_effect.m_status == CustomEffectStatus::Unload)
		{
			custom_effect.m_status = CustomEffectStatus::Delete;
		}
	}
}

bool EntityPlayerMP::canPickupItem(Entity* pEntity, int size)
{
	EntityPlayer::canPickupItem(pEntity, size);
	auto item = dynamic_cast<EntityItem*>(pEntity);
	if (!SCRIPT_EVENT::PlayerPickupItemEvent::invoke(this->getRaknetID(), item->getEntityItem()->itemID, size, pEntity->entityId))
		return false;
	else
		return true;
}

void EntityPlayerMP::setCurrency(i64 currency)
{
	if (m_wallet && currency != -1)
	{
		m_wallet->setCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
	}
}

void EntityPlayerMP::addCurrency(i32 currency)
{
	if (m_wallet && currency != 0)
	{
		m_wallet->addCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
		ServerNetwork::Instance()->getSender()->notifyGetMoney(this->getRaknetID(), currency);
	}
}

void EntityPlayerMP::subCurrency(i32 currency)
{
	if (m_wallet && currency != 0)
	{
		m_wallet->subCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
	}
}

i64 EntityPlayerMP::getCurrency()
{
	if (m_wallet)
	{
		return m_wallet->getCurrency();
	}
	return 0;
}

void EntityPlayerMP::setChangePlayerActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount)
{
	if (this->isDead)
		return;

	if (changeActorCount == 0 || needMoneyCount == 0 || isCanChangeActor)
		SCRIPT_EVENT::PlayerBuyActorResultEvent::invoke(this->getRaknetID(), true);
	else 
		Server::Instance()->getNetwork()->getSender()->syncChangePlayerActorInfo(m_raknetID, changeActorCount, needMoneyCount);

}

void EntityPlayerMP::attackActorNpc(Entity * entity, PLAYER_ATTACK_TYPE damageType)
{
	if (entity)
	{
		if (damageType == PLAYER_ATTACK_DAMAGE)
		{
			SCRIPT_EVENT::PlayerAttackActorNpcEvent::invoke(this->getRaknetID(), entity->entityId);
			return;
		}
		if (damageType == PLAYER_ATTACK_GUN)
		{
			SCRIPT_EVENT::PlayerShotActorNpcEvent::invoke(this->getRaknetID(), entity->entityId);
			return;
		}
	}
}

void EntityPlayerMP::setSpeedAdditionLevel(int level)
{
	float addition = getSpeedAddition();
	EntityPlayer::setSpeedAdditionLevel(level);
	if (addition != getSpeedAddition())
		ServerNetwork::Instance()->getSender()->sendPlayerSpeedAdditionLevel(getRaknetID(), level);
}

void EntityPlayerMP::updateExp(int level, float exp, int maxExp)
{
	ServerNetwork::Instance()->getSender()->sendPlayerExpInfo(getRaknetID(), level, exp, maxExp);
}
	
void EntityPlayerMP::upgradeResource(int resourceId)
{
	SCRIPT_EVENT::PlayerUpgradeResourceEvent::invoke(this->getRaknetID(), resourceId);
}


void EntityPlayerMP::setUnlockedCommodities(std::vector<std::vector<int>> unlockedCommodities)
{
	m_unlockedCommodities.clear();
	m_unlockedCommodities = unlockedCommodities;
	for (auto iter : m_unlockedCommodities)
	{
		ServerNetwork::Instance()->getSender()->sendInitUnlockedCommodity(getRaknetID(), iter);
	}
}

void EntityPlayerMP::playSkillEffect(String name, float duration, int range, Vector3 color)
{
	ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, name, duration, range, range, color, 1);
}

void EntityPlayerMP::sendSkillEffect(int effectId)
{
	SkillEffect* pSkill = SkillSetting::getSkillEffect(effectId);
	if (!pSkill)
		return;
	ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, pSkill->name, pSkill->duration,
		pSkill->width, pSkill->height, pSkill->color, pSkill->density);
}

void EntityPlayerMP::setOccupation(int occupation)
{
	m_occupation = occupation;
	ServerNetwork::Instance()->getSender()->sendEntityPlayerOccupation(0, entityId, occupation);
}

void EntityPlayerMP::resetClothes(ui64 userId)
{
	UserAttrInfo attrInfo;
	if (Server::Instance()->getEnableRoom())
	{
		bool succ = Server::Instance()->getRoomManager()->getUserAttrInfo(userId, attrInfo);
		if (succ)
		{
			this->m_faceID = attrInfo.faceId;
			this->m_hairID = attrInfo.hairId;
			this->m_topsID = attrInfo.topsId;
			this->m_pantsID = attrInfo.pantsId;
			this->m_shoesID = attrInfo.shoesId;
			this->m_glassesId = attrInfo.glassesId;
			this->m_scarfId = attrInfo.scarfId;
			this->m_wingId = attrInfo.wingId;
			this->m_hatId = attrInfo.hatId;
			this->m_decorate_hatId = attrInfo.decoratehatId;
			this->m_armId = attrInfo.armId;
			this->m_tailId = attrInfo.tailId;
			this->m_crownId = attrInfo.crownId;
			this->m_bagId = attrInfo.bagId;
			this->m_extra_wingId = 0;
			this->m_decorate_hatId = 0;
			this->m_armId = 0;

			ServerNetwork::Instance()->getSender()->sendUserAttrToTrackingPlayers(entityId, attrInfo);
			m_isClothesChanged = false;
		}
		else
		{
			LordLogError("reset clothes get user attr info failure");
		}

	}
}

bool EntityPlayerMP::tryBuyVehicle(int vehicleId)
{
	if (CarSetting::m_sbIsCarFree)
		return false;

	if (hasOwnVehicle(vehicleId))
		return false;

	const CarSetting* pSetting = CarSetting::getCarSetting(vehicleId);
	if (!pSetting || pSetting->price <= 0)
		return false;

	i64 money = getCurrency();
	if (money < pSetting->price)
		return false;

	subCurrency(pSetting->price);
	addOwnVehicle(vehicleId);
	syncOwnVehicle();

	SCRIPT_EVENT::PlayerBuyVehicleSuccessEvent::invoke(getRaknetID(), vehicleId);

	return true;
}

void EntityPlayerMP::syncOwnVehicle()
{
	ServerNetwork::Instance()->getSender()->sendPlayerOwnVehicle(getRaknetID(), m_ownVehicle);
}

void EntityPlayerMP::addBackpackCapacity(int capacity)
{
	if (capacity == 0)
		return;
	EntityPlayer::addBackpackCapacity(capacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::subBackpackCapacity(int capacity)
{
	if (capacity == 0)
		return;
	EntityPlayer::subBackpackCapacity(capacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::resetBackpack(int capacity, int maxCapacity)
{
	EntityPlayer::resetBackpack(capacity, maxCapacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::SetKnockBackCoefficient(float knockBackCoefficient)
{
	this->m_knockBackCoefficient = knockBackCoefficient;
}

void EntityPlayerMP::onItemPickup(Entity* pEntity, int size)
{
	EntityPlayer::onItemPickup(pEntity, size);
	openContainer->detectAndSendChanges();
	auto item = dynamic_cast<EntityItem*>(pEntity);
	if (item)
	{
		SCRIPT_EVENT::PlayerPickupItemEndEvent::invoke(this->getRaknetID(), item->getEntityItem()->itemID, size);
	}
}

SLEEP_RESULT EntityPlayerMP::sleepInBedAt(const BlockPos& pos)
{
	SLEEP_RESULT ret = EntityPlayer::sleepInBedAt(pos);

	if (ret == SLEEP_RESULT_OK)
	{
		/*Packet17Sleep var5 = new Packet17Sleep(this, 0, par1, par2, par3);
		getServerForPlayer().getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, var5);
		playerNetServerHandler.setPlayerLocation(posX, posY, posZ, rotationYaw, rotationPitch);
		playerNetServerHandler.sendPacketToPlayer(var5);*/
	}

	return ret;
}

void EntityPlayerMP::wakeUpPlayer(bool immediately, bool update, bool setSpawn)
{
	if (isPlayerSleeping())
	{
		// todo.
		// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(this, 3));
	}

	EntityPlayer::wakeUpPlayer(immediately, update, setSpawn);

	// todo.
	/*if (playerNetServerHandler != null)
	{
		playerNetServerHandler.setPlayerLocation(posX, posY, posZ, rotationYaw, rotationPitch);
	}*/
}

void EntityPlayerMP::mountEntity(Entity* pEntity)
{
	EntityPlayer::mountEntity(pEntity);
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet39AttachEntity(0, this, ridingEntity));
	// playerNetServerHandler.setPlayerLocation(posX, posY, posZ, rotationYaw, rotationPitch);
}

void EntityPlayerMP::sendSlotContents(Container* pContainer, int slot, ItemStackPtr pStack)
{

	if (!(dynamic_cast<SlotCrafting*>(pContainer->getSlot(slot))))
	{
		if (!playerInventoryBeingManipulated)
		{
			// todo.
			// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(pContainer.windowId, slot, pStack));
		}
	}
}

void EntityPlayerMP::sendContainerAndContentsToPlayer(Container* pContainer, ItemStackArr& stacks)
{
	// playerNetServerHandler.sendPacketToPlayer(new Packet104WindowItems(pContainer.windowId, stacks));
	// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(-1, -1, inventory.getItemStack()));
}

void EntityPlayerMP::sendProgressBarUpdate(Container* pContainer, int toUpdate, int val)
{
	// playerNetServerHandler.sendPacketToPlayer(new Packet105UpdateProgressbar(pContainer.windowId, toUpdate, val));
}

void EntityPlayerMP::sendContainerToPlayer(Container* pContainer)
{
	ItemStackArr stacks;
	pContainer->getInventory(stacks);
	sendContainerAndContentsToPlayer(pContainer, stacks);
}

void EntityPlayerMP::closeScreen()
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet101CloseWindow(openContainer.windowId));
	closeContainer();
}

void EntityPlayerMP::updateHeldItem()
{
	if (!playerInventoryBeingManipulated)
	{
		// todo.
		// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(-1, -1, inventory.getItemStack()));
	}
}

void EntityPlayerMP::closeContainer()
{
	// no need to operator container in server side... just tell all the clients that the chest is closed by some body.
	// openContainer->onContainerClosed(this);
	// openContainer = inventoryContainer;
}

void EntityPlayerMP::addStat(StatBase* stat, int amount)
{
	if (stat)
	{
		if (!stat->isIndependent())
		{
			// todo.
			// playerNetServerHandler.sendPacketToPlayer(new Packet200Statistic(stat.statId, amount));
		}
	}
}

void EntityPlayerMP::mountEntityAndWakeUp()
{
	if (riddenByEntity)
	{
		riddenByEntity->mountEntity(this);
	}

	if (sleeping)
	{
		wakeUpPlayer(true, false, false);
	}
}


void EntityPlayerMP::setAllowFlying(bool allowFlying)
{
	EntityPlayer::setAllowFlying(allowFlying);
	Server::Instance()->getNetwork()->getSender()->sendSetAllowFlying(getRaknetID(), allowFlying);
}

void EntityPlayerMP::setWatchMode(bool isWatchMode)
{
	EntityPlayer::setWatchMode(isWatchMode);
	Server::Instance()->getNetwork()->getSender()->sendSetWatchMode(getRaknetID(), entityId, isWatchMode);
	Server::Instance()->getNetwork()->getSender()->sendSetWatchModeToTrackingPlayers(entityId, isWatchMode);
}

void EntityPlayerMP::addChatMessage(const String& msg)
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet3Chat(ChatMessageComponent.func_111077_e(msg)));
}

void EntityPlayerMP::onItemUseFinish()
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet38EntityStatus(entityId, (byte)9));
	EntityPlayer::onItemUseFinish();
}

bool EntityPlayerMP::useItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos, int param0)
{
	bool bResult = false;
	ItemStackPtr currentItem = inventory->getCurrentItem();
	if (!currentItem) {
		LordLogError("EntityPlayerMP::useItem but current item in hand is nullptr");
		return bResult;
	}

	int size = currentItem->stackSize;
	ItemStackPtr pStackAfterUsed;
	ItemBucket* bucket = nullptr;

	switch (useType)
	{
	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_EASTABLE:
		pStackAfterUsed = currentItem->onFoodEaten(world, this);
		break;

	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_BUCKET:
		bucket = dynamic_cast<ItemBucket*>(currentItem->getItem());
		if (!bucket) {
			return bResult;
		}
		pStackAfterUsed = bucket->doIt(currentItem, world, this, pos, param0);
		break;
	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_COMMON:
		pStackAfterUsed = currentItem->useItemRightClick(world, this);
		break;

	default:
		return bResult;
	}
	
	if (pStackAfterUsed != itemInUse || (pStackAfterUsed && pStackAfterUsed->stackSize != size))
	{
		inventory->setInventorySlotContents(inventory->getInventoryIndexOfCurrentItem(), pStackAfterUsed);

		if (pStackAfterUsed->stackSize == 0)
		{
			inventory->setInventorySlotContents(inventory->getInventoryIndexOfCurrentItem(), nullptr);
		}

		bResult = true;
	}

	if (currentItem->itemID == ITEM_ID_FIRE_BALL)
	{
		bResult = true;
	}

	if (bResult)
	{
		SCRIPT_EVENT::PlayerUseCommonItemEvent::invoke(this->getRaknetID(), currentItem->itemID);
	}

	return bResult;
}

void EntityPlayerMP::updatePotionMetadata()
{
	EntityLivingBase::updatePotionMetadata();
}

void EntityPlayerMP::setItemInUse(ItemStackPtr pStack, int duration)
{
	EntityPlayer::setItemInUse(pStack, duration);

	if (pStack && pStack->getItem() && pStack->getItem()->getItemUseAction(pStack) == ACTION_TYPE_EAT)
	{
		// todo.
		// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(this, 5));
	}
}

void EntityPlayerMP::clonePlayer(EntityPlayer* pPlayer, bool all)
{
	EntityPlayer::clonePlayer(pPlayer, all);
	// todo.
	// destroyedItemsNetCache.addAll(((EntityPlayerMP)pPlayer).destroyedItemsNetCache);
}

void EntityPlayerMP::onNewPotionEffect(PotionEffect* pEff)
{
	EntityPlayer::onNewPotionEffect(pEff);
	Server::Instance()->getNetwork()->getSender()->sendAddEntityEffectToTrackingPlayers(entityId, pEff);
}

void EntityPlayerMP::onChangedPotionEffect(PotionEffect* pEff, bool apply)
{
	EntityPlayer::onChangedPotionEffect(pEff, apply);
	Server::Instance()->getNetwork()->getSender()->sendAddEntityEffectToTrackingPlayers(entityId, pEff);
}

void EntityPlayerMP::onFinishedPotionEffect(PotionEffect* pEff)
{
	EntityPlayer::onFinishedPotionEffect(pEff);
	Server::Instance()->getNetwork()->getSender()->sendRemoveEntityEffectToTrackingPlayers(entityId, pEff->getPotionID());
	SCRIPT_EVENT::PlayerPotionEffectFinishedEvent::invoke(this->getRaknetID(), pEff->getPotionID());
}

bool EntityPlayerMP::setToLastPosition(float x, float y, float z)
{
	if (x != position.x || z != position.z)
	{
		float yaw = rotationYaw;
		float pitch = rotationPitch;
		auto posX = static_cast<int>(Math::Floor(position.x * 32.F));
		auto posY = static_cast<int>(Math::Floor(position.y * 32.F));
		auto posZ = static_cast<int>(Math::Floor(position.z * 32.F));
		Server::Instance()->getNetwork()->getSender()->sendEntityTeleport(getRaknetID(), entityId, posX, posY, posZ, yaw, pitch);
		return true;
	}

	return false;
}

void EntityPlayerMP::changeMaxHealth(float health)
{
	float lastBaseHealth = getBaseHealth();
	EntityPlayer::changeMaxHealth(health);
	float curBaseHealth = getBaseHealth();
	if (lastBaseHealth != curBaseHealth)
	{
		Server::Instance()->getNetwork()->getSender()->broadCastChangePlayerMaxHealth(entityId, curBaseHealth);
	}
}

void EntityPlayerMP::equipArmor(int id, int damage)
{
	if (this->isDead)
		return;
	int slot = inventory->getFirstEmptyStackInInventory();
	if (slot < 0)
		return;

	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), 1, damage);
	inventory->setInventorySlotContents(slot, itemStack);
	if (inventory->equipArmor(itemStack))
	{
		Server::Instance()->getNetwork()->getSender()->sendEquipArmors(this, entityId, { itemStack });
		Server::Instance()->getNetwork()->getSender()->sendEquipArmorsToTrackingPlayers(entityId, { itemStack });
	}
}

void EntityPlayerMP::setPositionAndUpdate(float x, float y, float z)
{
	auto peer = Server::Instance()->getNetwork()->findPlayerByRakssid(getRaknetID());
	if (!peer)
		return;

	peer->setPlayerLocation({x, y, z}, rotationYaw, rotationPitch);
	auto sender = Server::Instance()->getNetwork()->getSender();
	if (world->getPlayerEntity(entityId) == NULL) // is loging in!!!!!
	{
		float yaw = rotationYaw;
		float pitch = rotationPitch;
		auto posX = static_cast<int>(Math::Floor(x * 32.F));
		auto posY = static_cast<int>(Math::Floor(y * 32.F));
		auto posZ = static_cast<int>(Math::Floor(z * 32.F));
		sender->sendEntityTeleport(getRaknetID(), entityId, posX, posY, posZ, yaw, pitch);
	}
	else
	{
		sender->broadCastPlayerMovement(entityId, true);
		sender->broadCastPlayerRotation(entityId, true);
	}
	
}

void EntityPlayerMP::onCriticalHit(Entity* pEntity)
{
	// todo.
	// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(pEntity, 6));
}

void EntityPlayerMP::onEnchantmentCritical(Entity* pEntity)
{
	// todo.
	// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(pEntity, 7));
}

void EntityPlayerMP::sendPlayerAbilities()
{
	// todo.
	/*if (playerNetServerHandler != null)
	{
		playerNetServerHandler.sendPacketToPlayer(new Packet202PlayerAbilities(capabilities));
	}*/
}

void EntityPlayerMP::setGameType(GAME_TYPE type)
{
	// todo.
	// theItemInWorldManager.setGameType(type);
	// playerNetServerHandler.sendPacketToPlayer(new Packet70GameEvent(3, type.getID()));
	if (type == GAME_TYPE_CREATIVE)
	{
		capabilities.allowFlying = true;
		capabilities.isCreativeMode = true;
		capabilities.disableDamage = true;
		capabilities.isWatchMode = false;
	}
	else
	{
		capabilities.allowFlying = false;
		capabilities.isCreativeMode = false;
		capabilities.disableDamage = false;
		capabilities.isFlying = false;
		capabilities.isWatchMode = false;
	}

	capabilities.allowEdit = type != GAME_TYPE_ADVENTURE;
}

void EntityPlayerMP::notifyClientReloadBullet()
{
	Server::Instance()->getNetwork()->getSender()->sendReloadBullet(m_raknetID);
}

void EntityPlayerMP::sendGetBulletSound()
{
	Server::Instance()->getNetwork()->getSender()->sendPlaySoundByType(m_raknetID, (int)ST_FemalePlayerBeHit);
}

bool EntityPlayerMP::takeOnAircraft(EntityAircraft * pAircraft)
{
	bool bRet = EntityPlayer::takeOnAircraft(pAircraft);
	if (bRet)
	{
		setPositionAndUpdate(position.x, position.y, position.z);
		Server::Instance()->getNetwork()->getSender()->broadcastTakeAircraft(entityId , true, pAircraft->entityId);
		//LordLogInfo("+++++EntityPlayerMP id %d takeOnAircraft pos(%f, %f, %f)", entityId, position.x, position.y, position.z);
	}
	return bRet;
}

bool EntityPlayerMP::takeOffAircraft()
{
	int aircraftId = m_aircraftEntityId;
	bool bRet = EntityPlayer::takeOffAircraft();
	if (bRet)
	{
		setPositionAndUpdate(position.x, position.y, position.z);
		Server::Instance()->getNetwork()->getSender()->broadcastTakeAircraft(entityId, false, aircraftId);
		//LordLogInfo("-----EntityPlayerMP id %d takeOffAircraft pos(%f, %f, %f)", entityId, position.x, position.y, position.z);
	}
	return bRet;
}

bool BLOCKMAN::EntityPlayerMP::takeOffVehicle()
{
	leaveVehicle();
	return true;
}

void BLOCKMAN::EntityPlayerMP::setOpenChest(TileEntityChest * chest)
{
	m_openChest = chest;
	m_openChest->openChest();
}

void EntityPlayerMP::closeOpenChest()
{
	if (m_openChest)
	{
		auto iter = find(m_openChest->openChestPlayers.begin(), m_openChest->openChestPlayers.end(), this->entityId);
		if (iter != m_openChest->openChestPlayers.end())
		{
			m_openChest->openChestPlayers.erase(iter);
		}
		m_openChest->closeChest();
	}
}

void EntityPlayerMP::sendChatToPlayer(ChatMessageComponent* chatComponent)
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet3Chat(chatComponent));
}

bool EntityPlayerMP::canCommandSenderUseCommand(int par1, const String& par2Str)
{
	// todo.
	// return "seed".equals(par2Str) && !mcServer.isDedicatedServer() ? true : (!"tell".equals(par2Str) && !"help".equals(par2Str) && !"me".equals(par2Str) ? (mcServer.getConfigurationManager().areCommandsAllowed(username) ? mcServer.func_110455_j() >= par1 : false) : true);
	return true;

}

String EntityPlayerMP::getPlayerIP()
{
	// todo.
	return String("127.0.0.1");
}

void EntityPlayerMP::requestTexturePackLoad(const String& par1Str, int par2)
{
	// todo.
	/*
	String var3 = par1Str + "\u0000" + par2;
	playerNetServerHandler.sendPacketToPlayer(new Packet250CustomPayload("MC|TPack", var3.getBytes()));
	*/
}

BlockPos EntityPlayerMP::getPlayerCoordinates()
{
	BlockPos pos;
	pos.x = int(Math::Floor(position.x));
	pos.z = int(Math::Floor(position.z));
	pos.y = int(Math::Floor(position.y + 0.5f));
	return pos;
}

void EntityPlayerMP::func_110430_a(float par1, float par2, bool par3, bool par4)
{
	if (ridingEntity)
	{
		if (par1 >= -1.0F && par1 <= 1.0F)
		{
			moveStrafing = par1;
		}

		if (par2 >= -1.0F && par2 <= 1.0F)
		{
			moveForward = par2;
		}

		isJumping = par3;
		setSneaking(par4);
	}
}

bool EntityPlayerMP::setCameraLocked(bool bLocked)
{
	if (m_isCameraLocked != bLocked)
	{
		EntityPlayer::setCameraLocked(bLocked);
		ServerNetwork::Instance()->getSender()->broadCastCameraLockToClient(bLocked, entityId);
		return true;
	}
	return false;
}

void BLOCKMAN::EntityPlayerMP::changeInvisible(ui64 rakssId, bool isInvisible)
{
	if (rakssId == 0)
		ServerNetwork::Instance()->getSender()->broadCastPlayerInvisible(entityId, isInvisible);
	else
		ServerNetwork::Instance()->getSender()->sendPlayerInvisible(rakssId, entityId, isInvisible);
}

void BLOCKMAN::EntityPlayerMP::changeHeart(int hp, int maxHp)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeHeart(getRaknetID(), hp, maxHp);
}

void BLOCKMAN::EntityPlayerMP::showBossStripWithIcon(String name, int curProgress, int maxProgress, String iconPath)
{
	ServerNetwork::Instance()->getSender()->sendPlayerBossStripWithIcon(getRaknetID(), name, curProgress, maxProgress, iconPath);
}

void BLOCKMAN::EntityPlayerMP::changeDefense(int defense, int maxDefense)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeDefense(getRaknetID(), defense, maxDefense);
}

void BLOCKMAN::EntityPlayerMP::changeAttack(int attack, int maxAttack)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeAttack(getRaknetID(), attack, maxAttack);
}

void BLOCKMAN::EntityPlayerMP::setPersonalShopArea(Vector3 startPos, Vector3 endPos)
{
	ServerNetwork::Instance()->getSender()->sendSetPersonalShopArea(getRaknetID(), startPos, endPos);
}

void BLOCKMAN::EntityPlayerMP::changeSwitchableProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendSwitchablePropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeUpgradeProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendUpgradePropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeEnchantmentProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendEnchantmentPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeSuperProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendSuperPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeCustomProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendCustomPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::updateCustomProps(String data)
{
	ServerNetwork::Instance()->getSender()->updateCustomProps(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::attackCreatureEntity(EntityCreature* entityCreature, int attackType)
{
	if (entityCreature)
	{
		if (!SCRIPT_EVENT::PlayerAttackCreatureEvent::invoke(this->getRaknetID(), entityCreature->entityId, attackType))
			return;
		entityCreature->changeCreatureAction(CreatureActionState::CREATURE_AS_BE_PLAYER_ATTACK);
	}
}

void BLOCKMAN::EntityPlayerMP::initUpdateBlocks()
{
	const auto& positions = BlockChangeRecorderServer::Instance()->getChangedPositions();
	auto world = Server::Instance()->getWorld();

	if (world == nullptr) return;

	std::vector<BlockPos> blocks;

	for (const auto& position : positions)
	{
		blocks.emplace_back(position.pos);
	}

	if (mSynBlockContext)
		mSynBlockContext->init(position, blocks);
}

void BLOCKMAN::EntityPlayerMP::sendUpdateBlocks()
{
	if (mSynBlockContext)
		mSynBlockContext->sync(m_raknetID);
}

void BLOCKMAN::EntityPlayerMP::addUpdateBlocks(std::vector<BlockPos>& blocks)
{
	for (const auto& position : blocks)
	{
		Vector3 block_pos((float)position.x, (float)position.y, (float)position.z);
		float distance = getDistance(block_pos);
		addSingleUpdateBlock(position, world->getBlockId(position), world->getBlockMeta(position));
	}
}

void BLOCKMAN::EntityPlayerMP::addSingleUpdateBlock(BlockPos pos, int block_id, int meta)
{
	if (mSynBlockContext)
		mSynBlockContext->add(position, pos, block_id, meta);
}

void BLOCKMAN::EntityPlayerMP::forceBrocastPosition()
{
	if (m_bNeedSyncPosition)
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerMovement(entityId, true);
		m_bNeedSyncPosition = false;
	}
}

void EntityPlayerMP::updateMoveEventInvoke()
{
	if (mSynBlockContext)
		mSynBlockContext->move(position);
}

int EntityPlayerMP::getSlotByItemId(int ItemId)
{
	int slot = inventory->getItemSlotByHotbat(ItemId);
	return slot;
}

void BLOCKMAN::EntityPlayerMP::showMask(int MilliSecond)
{
	auto sender = Server::Instance()->getNetwork()->getSender();

	if (!sender)
		return;

	sender->syncShowMaskTime(this->getRaknetID(), MilliSecond);
}

void BLOCKMAN::EntityPlayerMP::setArmItem(int itemId)
{
	Server::Instance()->getNetwork()->getSender()->sendSetArmItem(getRaknetID(), itemId);
}

void BLOCKMAN::EntityPlayerMP::addGunBulletNum(int gunId, int bulletNum)
{
	EntityPlayer::addGunBulletNum(gunId, bulletNum);
	Server::Instance()->getNetwork()->getSender()->sendAddGunBulletNum(getRaknetID(), gunId, bulletNum);
}

void BLOCKMAN::EntityPlayerMP::subGunRecoil(int gunId, float recoil)
{
	EntityPlayer::subGunRecoil(gunId, recoil);
	Server::Instance()->getNetwork()->getSender()->sendSubGunRecoil(getRaknetID(), gunId, recoil);
}

void BLOCKMAN::EntityPlayerMP::subGunFireCd(int gunId, float fireCd)
{
	EntityPlayer::subGunFireCd(gunId, fireCd);
	Server::Instance()->getNetwork()->getSender()->sendSubGunFireCd(getRaknetID(), gunId, fireCd);
}

void BLOCKMAN::EntityPlayerMP::setOnFrozen(float frozenTime)
{
	EntityPlayer::setOnFrozen(frozenTime);
	ServerNetwork::Instance()->getSender()->syncEntityFrozenTime(0, entityId, frozenTime);
}

void BLOCKMAN::EntityPlayerMP::setOnHurt(float hurtTime)
{
	EntityPlayer::setOnHurt(hurtTime);
	ServerNetwork::Instance()->getSender()->syncEntityHurtTime(0, entityId, hurtTime);
}

void BLOCKMAN::EntityPlayerMP::setOnColorful(float colorfulTime)
{
	EntityPlayer::setOnColorful(colorfulTime);
	ServerNetwork::Instance()->getSender()->syncEntityColorfulTime(0, entityId, colorfulTime);
}

void BLOCKMAN::EntityPlayerMP::setDisableMove(float disableMoveTime)
{
	EntityPlayer::setDisableMove(disableMoveTime);
	ServerNetwork::Instance()->getSender()->syncEntityDisableMoveTime(0, entityId, disableMoveTime);
}

void BLOCKMAN::EntityPlayerMP::setOnFire(int seconds)
{
	fire = seconds * 20;
}

void EntityPlayerMP::attackEntity(Entity * entity)
{
	switch (entity->getClassID())
	{
	case ENTITY_CLASS_SESSION_NPC:
		SCRIPT_EVENT::PlayerAttackSessionNpcEvent::invoke(this->getRaknetID(), entity->hashCode());
		break;
	case ENTITY_CLASS_BLOCKMAN:
	case ENTITY_CLASS_BLOCKMAN_EMPTY:
		attackTargetEntityWithCurrentItem(entity);
		break;
	case ENTITY_CLASS_BUILD_NPC:
		attackTargetEntityWithCurrentItem(entity);
		break;
	}
}

void BLOCKMAN::EntityPlayerMP::enableMovement()
{
	EntityPlayer::enableMovement();
	ServerNetwork::Instance()->getSender()->sendPlayerEnableMovement(this->hashCode());
}

void BLOCKMAN::EntityPlayerMP::checkForceMovement()
{
	m_movePlayerUpdateTick++;
	if (m_movePlayerUpdateTick % 80 != 0)
		return;
	if (isPlayerMoved())
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerMovement(entityId, false);
		recordOldPos();
	}
}

bool BLOCKMAN::EntityPlayerMP::isPlayerMoved()
{
	float dx = position.x - oldPos.x;
	float dy = boundingBox.vMin.y - oldMinY;
	float dz = position.z - oldPos.z;
	return dx * dx + dy * dy + dz * dz > 0.25;
}

void EntityPlayerMP::recordOldPos()
{
	oldPos = position;
	oldMinY = boundingBox.vMin.y;
}

void EntityPlayerMP::setBagInfo(int curCapacity, int maxCapacity)
{
	m_bagCapacity = curCapacity;
	m_bagMaxCapacity = maxCapacity;
	ServerNetwork::Instance()->getSender()->syncPlayerBagInfo(0, this);
}

void EntityPlayerMP::setBirdConvert(bool isConvert)
{
	m_isBirdConvert = isConvert;
	ServerNetwork::Instance()->getSender()->syncPlayerBagInfo(0, this);
}

i64 BLOCKMAN::EntityPlayerMP::getDiamond()
{
	if (m_wallet)
		return m_wallet->getDiamonds();
	return 0;
}

i64 BLOCKMAN::EntityPlayerMP::getGold()
{
	if (m_wallet)
		return m_wallet->getGolds();
	return 0;
}

void BLOCKMAN::EntityPlayerMP::changeNamePerspective(ui64 rakssId, bool isPerspective)
{
	if (rakssId != 0)
		ServerNetwork::Instance()->getSender()->sendPlayerNamePerspective(rakssId, entityId, isPerspective);
}

}
