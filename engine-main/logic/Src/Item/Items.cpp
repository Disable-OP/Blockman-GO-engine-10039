#include "Items.h"
#include "ItemStack.h"
#include "BM_TypeDef.h"
#include "Potion.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityPotion.h"
#include "Item/Potion.h"
#include "Entity/Enchantment.h"
#include "World/World.h"
#include "World/RayTracyResult.h"
#include "WorldGenerator/WeightedRandomItem.h"
#include "Inventory/InventoryPlayer.h"
#include "Entity/EntityArrow.h"
#include "TileEntity/TileEntitys.h"
#include "Common.h"
#include "Util/CommonEvents.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityFireball.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityItem.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Util/Random.h"
#include "Script/Event/LogicScriptEvents.h"

#include "Util/StringUtil.h"
#include "Entity/EntityFishHook.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityBuildNpc.h"
#include "Setting/HouseSetting.h"
#include "Setting/BuildingSetting.h"
#include "Setting/LogicSetting.h"

namespace BLOCKMAN
{

ToolMaterial* ToolMaterial::WOOD	= NULL;
ToolMaterial* ToolMaterial::STONE	= NULL;
ToolMaterial* ToolMaterial::IRON	= NULL;
ToolMaterial* ToolMaterial::EMERALD = NULL;
ToolMaterial* ToolMaterial::GOLD	= NULL;

ToolMaterial::ToolMaterial(int level, int maxuses, float effectiency, float damage, int enchantabilityIn)
	: harvestLevel(level)
	, maxUses(maxuses)
	, efficiencyOnProperMaterial(effectiency)
	, damageVsEntity(damage)
	, enchantability(enchantabilityIn)
{
}

void ToolMaterial::initialize()
{
	WOOD	= LordNew ToolMaterial(0, 59, 2.0F, 0.0F, 15);
	STONE	= LordNew ToolMaterial(1, 131, 4.0F, 1.0F, 5);
	IRON	= LordNew ToolMaterial(2, 250, 5.0F, 2.0F, 14);
	GOLD	= LordNew ToolMaterial(3, 600, 6.0F, 3.0F, 22);
	EMERALD = LordNew ToolMaterial(4, 1561, 8.0F, 4.0F, 10); 
}

void ToolMaterial::unInitialize()
{
	LordSafeDelete(WOOD);
	LordSafeDelete(STONE);
	LordSafeDelete(IRON);
	LordSafeDelete(GOLD);
	LordSafeDelete(EMERALD);
}

int ToolMaterial::getToolCraftingMaterial()
{
	if (this == WOOD)
		return BLOCK_ID_PLANKS;
	else if (this == STONE)
		return BLOCK_ID_COBBLE_STONE;
	else if (this == GOLD)
		return Item::ingotGold->itemID;
	else if (this == IRON)
		return Item::ingotIron->itemID;
	else if (this == EMERALD)
		return Item::diamond->itemID;
	return 0;
}

String ToolMaterial::toString()
{
	if (this == WOOD)
		return "WOOD";
	else if (this == STONE)
		return "STONE";
	else if (this == GOLD)
		return "GOLD";
	else if (this == IRON)
		return "IRON";
	else if (this == EMERALD)
		return "EMERALD";
	return StringUtil::BLANK;
}

ArmorMaterial* ArmorMaterial::CLOTH = NULL;	// (5, new int[] {1, 3, 2, 1}, 15),
ArmorMaterial* ArmorMaterial::CHAIN = NULL;	// (15, new int[] {2, 5, 4, 1}, 12),
ArmorMaterial* ArmorMaterial::IRON = NULL;	// (15, new int[] {2, 6, 5, 2}, 9),
ArmorMaterial* ArmorMaterial::GOLD = NULL;	// (7, new int[] {2, 5, 3, 1}, 25),
ArmorMaterial* ArmorMaterial::DIAMOND = NULL;	// (33, new int[] {3, 8, 6, 3}, 10);

void ArmorMaterial::initialize()
{
	int arr_cloth[] = { 1,3,2,1 };
	int arr_chain[] = { 2, 3, 2, 2 };
	int arr_iron[] = { 2, 4, 3, 2 };
	int arr_gold[] = { 3, 5, 4, 3 };
	int arr_diamond[] = { 4, 7, 5, 4 };
	CLOTH = LordNew ArmorMaterial(5, arr_cloth, 15);
	CHAIN = LordNew ArmorMaterial(15, arr_chain, 12);
	IRON = LordNew ArmorMaterial(15, arr_iron, 9);
	GOLD = LordNew ArmorMaterial(7, arr_gold, 25);
	DIAMOND = LordNew ArmorMaterial(33, arr_diamond, 10);
}

void ArmorMaterial::unInitialize()
{
	LordSafeDelete(CLOTH);
	LordSafeDelete(CHAIN);
	LordSafeDelete(IRON);
	LordSafeDelete(GOLD);
	LordSafeDelete(DIAMOND);
}

ArmorMaterial::ArmorMaterial(int factor, int* amountArray, int enchability)
	: maxDamageFactor(factor)
	, enchantability(enchability)
{
	memcpy(damageReductionAmountArray, amountArray, sizeof(int)*ARMOR_SIZE);
}

int ArmorMaterial::getDurability(int armorIdx)
{
	return ItemArmor::getMaxDamageArray()[armorIdx] * maxDamageFactor;
}

int ArmorMaterial::getArmorCraftingMaterial()
{
	if (this == CLOTH)
		return Item::leather->itemID;
	else if (this == CHAIN)
		return Item::ingotIron->itemID;
	else if (this == GOLD)
		return Item::ingotGold->itemID;
	else if (this == IRON)
		return Item::ingotIron->itemID;
	else if (this == DIAMOND)
		return Item::diamond->itemID;
	return 0;
}

ItemTool::ItemTool(int id, const String& name, float attackSpeed, ToolMaterial* toolMat)
	: ItemTool(id, name, attackSpeed , toolMat , 0.f)
{
}

ItemTool::ItemTool(int id, const String& name, float attackSpeed, ToolMaterial* toolMat, float effectiveDistanceIn)
	: Item(id, name)
	, efficiencyOnProperMaterial(4.f)
	, effectiveDistance(effectiveDistanceIn)
{
	toolMaterial = toolMat;
	maxStackSize = 1;
	setMaxDamage(toolMat->getMaxUses());
	efficiencyOnProperMaterial = toolMat->getEfficiencyOnProperMaterial();
	damageVsEntity = attackSpeed + toolMat->getDamageVsEntity();

	m_modifierAttr = LordNew AttributeModifier(ATTACK_DAMAGE_MODIFIER, "Tool modifier", damageVsEntity, 0);
}

float ItemTool::getStrVsBlock(ItemStackPtr pStack, Block* pBlock)
{
	for (size_t i = 0; i < blocksEffectiveAgainst.size(); ++i)
	{
		if (blocksEffectiveAgainst[i] == pBlock)
		{
			return efficiencyOnProperMaterial;
		}
	}

	return 1.0F;
}

bool ItemTool::hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2)
{
	pStack->damageItem(2, pLiving2);
	return true;
}

bool ItemTool::onBlockDestroyed(ItemStackPtr pStack, World* pWorld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving)
{
	if (BlockManager::sBlocks[blockID]->getBlockHardness(pWorld, pos) != 0.0f)
	{
		pStack->damageItem(1, pLiving);
	}

	return true;
}

bool ItemTool::getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return toolMaterial->getToolCraftingMaterial() == pStack2->itemID ? true : Item::getIsRepairable(pStack1, pStack2);
}

void ItemTool::getAttributeModifiers(AttributeModifierMap& outmap)
{
	outmap.insert(std::make_pair(SharedMonsterAttributes::ATTACK_DAMAGE->getName(), m_modifierAttr));
}

ItemSpade::ItemSpade(int id, const String& name, ToolMaterial* toolMat)
	: ItemTool(id, name, 1.0f, toolMat)
{
	BlockArr effAgaistBlock;
	Block** blockList = BlockManager::sBlocks;
	effAgaistBlock.push_back(blockList[BLOCK_ID_GRASS]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_DIRT]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_SAND]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_GRAVEL]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_SNOW]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCK_SNOW]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCKCLAY]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_TILLED_FIELD]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_SLOW_SAND]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_MYCELIUM]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_REDSAND_STONE_DOUBLESLAB]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_REDSAND_STONE_SINGLESLAB]);
	setEffectiveBlocks(effAgaistBlock);
}

bool ItemSpade::canHarvestBlock(Block* pBlock)
{
	return pBlock->getBlockID() == BLOCK_ID_SNOW ? true : pBlock->getBlockID() == BLOCK_ID_BLOCK_SNOW;
}

ItemPickaxe::ItemPickaxe(int id, const String& name, ToolMaterial* toolMat)
	: ItemTool(id, name, 2.0f, toolMat)
{
	BlockArr effAgaistBlock;
	Block** blockList = BlockManager::sBlocks;
	effAgaistBlock.push_back(blockList[BLOCK_ID_COBBLE_STONE]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_STONE_DOUBLE_SLAB]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_STONE_SINGLE_SLAB]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_STONE]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_SAND_STONE]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_COBBLE_STONE_MOSSY]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_IRON]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCK_IRON]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_COAL]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCK_GOLD]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_GOLD]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCK_DIAMOND]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_DIAMOND]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ICE]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_NETHERRACK]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_LAPIS]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BLOCK_LAPIS]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_REDSTONE]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_ORE_REDSTONE_GLOWING]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_RAIL]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_RAIL_DETECTOR]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_RAIL_POWERED]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_RAIL_ACTIVATOR]);
	setEffectiveBlocks(effAgaistBlock);
}

bool ItemPickaxe::canHarvestBlock(Block* pBlock)
{
	LordAssert(pBlock);
	int blockID = pBlock->getBlockID();

	if (SCRIPT_EVENT::ItemPickaxeCanHarvestEvent::invoke(blockID))
	{
		if (blockID == BLOCK_ID_OBSIDIAN)
			return toolMaterial->getHarvestLevel() == 3;
		if (blockID == BLOCK_ID_BLOCK_DIAMOND || blockID == BLOCK_ID_ORE_DIAMOND)
			return toolMaterial->getHarvestLevel() >= 2;
		if (blockID == BLOCK_ID_ORE_EMERALD || blockID == BLOCK_ID_BLOCK_EMERALD)
			return toolMaterial->getHarvestLevel() >= 2;
		if (blockID == BLOCK_ID_BLOCK_GOLD || blockID == BLOCK_ID_ORE_GOLD)
			return toolMaterial->getHarvestLevel() >= 2;
		if (blockID == BLOCK_ID_BLOCK_IRON || blockID == BLOCK_ID_ORE_IRON)
			return toolMaterial->getHarvestLevel() >= 1;
		if (blockID == BLOCK_ID_BLOCK_LAPIS || blockID == BLOCK_ID_ORE_LAPIS)
			return toolMaterial->getHarvestLevel() >= 1;
		if (blockID == BLOCK_ID_ORE_REDSTONE || blockID == BLOCK_ID_ORE_REDSTONE_GLOWING)
			return toolMaterial->getHarvestLevel() >= 2;
		if (pBlock->getMaterial() == BM_Material::BM_MAT_rock)
			return true;
		if (pBlock->getMaterial() == BM_Material::BM_MAT_iron)
			return true;
		return pBlock->getMaterial() == BM_Material::BM_MAT_anvil;
	}
	else
	{
		return true;
	}
	
}

float ItemPickaxe::getStrVsBlock(ItemStackPtr pStack, Block* pBlock)
{
	if (!pBlock)
		return ItemTool::getStrVsBlock(pStack, pBlock);
	const BM_Material& mat = pBlock->getMaterial();
	if (mat == BM_Material::BM_MAT_iron || mat == BM_Material::BM_MAT_anvil || mat == BM_Material::BM_MAT_rock)
		return efficiencyOnProperMaterial;
	return ItemTool::getStrVsBlock(pStack, pBlock);
}

ItemAxe::ItemAxe(int id, const String& name, ToolMaterial* toolMat)
	: ItemTool(id, name, 2.0f, toolMat)
{
	BlockArr effAgaistBlock;
	Block** blockList = BlockManager::sBlocks;
	effAgaistBlock.push_back(blockList[BLOCK_ID_PLANKS]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_BOOKSHELF]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_WOOD]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_WOOD2]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_CHEST]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_STONE_DOUBLE_SLAB]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_STONE_SINGLE_SLAB]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_PUMPKIN]);
	effAgaistBlock.push_back(blockList[BLOCK_ID_PUMPKIN_LANTERN]);
	setEffectiveBlocks(effAgaistBlock);
}

float ItemAxe::getStrVsBlock(ItemStackPtr pStack, Block* pBlock)
{
	if(!pBlock)
		return ItemTool::getStrVsBlock(pStack, pBlock);
	const BM_Material& mat = pBlock->getMaterial();
	if (mat == BM_Material::BM_MAT_wood || mat == BM_Material::BM_MAT_plants || mat == BM_Material::BM_MAT_vine)
		return efficiencyOnProperMaterial;
	return ItemTool::getStrVsBlock(pStack, pBlock);
}

ItemFlintAndSteel::ItemFlintAndSteel(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 1;
	setMaxDamage(64);
}

bool ItemFlintAndSteel::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	BlockPos pos1 = pos;
	if (face == BM_FACE_DOWN)
		--pos1.y;
	if (face == BM_FACE_UP)
		++pos1.y;
	if (face == BM_FACE_NORTH)
		--pos1.z;
	if (face == BM_FACE_SOUTH)
		++pos1.z;
	if (face == BM_FACE_WEST)
		--pos1.x;
	if (face == BM_FACE_EAST)
		++pos1.x;

	if (!pPlayer->canPlayerEdit(pos1, face, pStack))
	{
		return false;
	}
	else
	{
		int blockID = pWorld->getBlockId(pos1);

		if (blockID == 0)
		{
			//pWorld->playSoundEffect(Vector3(pos1) + 0.5f, "fire.ignite", 1.0F, itemRand->nextFloat() * 0.4F + 0.8F);
			pWorld->playSoundByType(Vector3(pos1) + 0.5f, ST_IgniteFire);
			pWorld->setBlock(pos1, BLOCK_ID_FIRE);
		}

		pStack->damageItem(1, pPlayer);
		return true;
	}
}

ItemFood::ItemFood(int id, const String& name, int amount, float modifier, bool isfavorite)
	: Item(id, name)
	, healAmount(amount)
	, saturationModifier(modifier)
	, isWolfsFavoriteMeat(isfavorite)
	, alwaysEdible(false)
	, potionId(0)
	, potionDuration(0)
	, potionAmplifier(0)
	, potionEffectProbability(0.f)
	, itemUseDuration(32)
{
}

ItemFood::ItemFood(int id, const String& name, int amount, bool isfavorite)
	: Item(id, name)
	, healAmount(amount)
	, saturationModifier(0.6f)
	, isWolfsFavoriteMeat(isfavorite)
	, alwaysEdible(false)
	, potionId(0)
	, potionDuration(0)
	, potionAmplifier(0)
	, potionEffectProbability(0.f)
	, itemUseDuration(32)
{
}

ItemStackPtr ItemFood::onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	--pStack->stackSize;
	pPlayer->getFoodStats()->addStats(this);
	//pWorld->playSoundAtEntity(pPlayer, "random.burp", 0.5F, pWorld->m_Rand.nextFloat() * 0.1F + 0.9F);
	pWorld->playSoundAtEntityByType(pPlayer, ST_Burp);
	onFoodEaten(pStack, pWorld, pPlayer);
	return pStack;
}

void ItemFood::onFoodEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pWorld->m_isClient && potionId > 0 && pWorld->m_Rand.nextFloat() < potionEffectProbability)
	{
		pPlayer->addPotionEffect(LordNew PotionEffect(potionId, potionDuration * 20, potionAmplifier));
	}
}

ItemStackPtr ItemFood::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (pPlayer->canEat(alwaysEdible))
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	}

	return pStack;
}

void ItemFood::setPotionEffect(int id, int duration, int amplifier, float probability)
{
	potionId = id;
	potionDuration = duration;
	potionAmplifier = amplifier;
	potionEffectProbability = probability;
}

ItemSoup::ItemSoup(int id, const String& name, int amount)
	: ItemFood(id, name, amount, false)
{
	setMaxStackSize(1);
}

ItemStackPtr ItemSoup::onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	ItemFood::onEaten(pStack, pWorld, pPlayer);
	return LORD::make_shared<ItemStack>(Item::bowlEmpty);
}

ItemAppleGold::ItemAppleGold(int id, const String& name, int amount, float modifier, bool isfavorite)
	: ItemFood(id, name, amount, modifier, isfavorite)
{
	setHasSubtypes(true);
}

bool ItemAppleGold::hasEffect(ItemStackPtr pStack)
{ 
	return pStack->getItemDamage() > 0; 
}

RARITY_TYPE ItemAppleGold::getRarity(ItemStackPtr pStack)
{
	return pStack->getItemDamage() == 0 ? RARITY_TYPE_RARE : RARITY_TYPE_EPIC;
}

void ItemAppleGold::onFoodEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pWorld->m_isClient)
	{
		pPlayer->addPotionEffect(LordNew PotionEffect(Potion::absorption->getId(), 2400, 0));
	}

	if (pStack->getItemDamage() > 0)
	{
		if (!pWorld->m_isClient)
		{
			pPlayer->addPotionEffect(LordNew PotionEffect(Potion::regeneration->getId(), 600, 4));
			pPlayer->addPotionEffect(LordNew PotionEffect(Potion::resistance->getId(), 6000, 0));
			pPlayer->addPotionEffect(LordNew PotionEffect(Potion::fireResistance->getId(), 6000, 0));
		}
	}
	else
	{
		ItemFood::onFoodEaten(pStack, pWorld, pPlayer);
	}
}

ItemSeedFood::ItemSeedFood(int id, const String& name, int amount, float modifier, int crop, int soil)
	: ItemFood(id, name, amount, modifier, false)
	, cropId(crop)
	, soilId(soil)
{
}

bool ItemSeedFood::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face != 1)
		return false;
	
	if (pPlayer->canPlayerEdit(pos, face, pStack) && 
		pPlayer->canPlayerEdit(pos.getPosY(), face, pStack))
	{
		int blockID = pWorld->getBlockId(pos);

		if (blockID == soilId && pWorld->isAirBlock(pos.getPosY()))
		{
			pWorld->setBlock(pos.getPosY(), cropId);
			--pStack->stackSize;
			return true;
		}
		return false;
	}

	return false;
}

ItemBow::ItemBow(int id , const String& name)
	: Item(id, name)
{
	maxStackSize = 1;
	setMaxDamage(384);
}

void ItemBow::onPlayerStoppedUsing(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, int timeleft)
{
	AimingStateChangeEvent::emit(false,NONE_TYPE);
	bool flag = pPlayer->capabilities.isCreativeMode || EnchantmentHelper::getEnchantmentLevel(Enchantment::infinity->effectId, pStack) > 0;
	BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::STOP_PULL_BOW);

	ItemStackPtr arrowItemStack = pPlayer->inventory->getArrowItemStack();
	if (flag || arrowItemStack)
	{
		int ticktime = getMaxItemUseDuration(pStack) - timeleft;
		float cdtime = (float)ticktime / 20.0F;
		cdtime = (cdtime * cdtime + cdtime * 2.0F) / 3.0F;

		if (cdtime < 0.1f)
			return;

		BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()
			->sendLaunchArrow(cdtime, pPlayer->position + Vector3(0.f, pPlayer->getEyeHeight(), 0.f),
				pPlayer->rotationYaw, pPlayer->rotationPitch, arrowItemStack ? arrowItemStack->itemID : 262);
	}
}

ItemStackPtr ItemBow::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	ItemStackPtr arrowItemStack = pPlayer->inventory->getArrowItemStack();
	if (pPlayer->capabilities.isCreativeMode || arrowItemStack)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		AimingStateChangeEvent::emit(true, COMMON_CROSS_HAIR);
		BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::START_PULL_BOW);
	}

	return pStack;
}

ItemGun::ItemGun(int id, int gunId, const String& name)
	: Item(id, name)
	, gunId(gunId)
{
	maxStackSize = 1;
}

void ItemGun::onPlayerStoppedUsing(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, int timeleft)
{
	// TODO...
}

Vector3 ItemGun::calcDirPos(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	Vector3 dir;
	const GunSetting* setting = getGunSetting();
	if (!setting)
		return Vector3(0,0,0);

	i64 seed = pWorld->m_Rand.nextLong();
	pWorld->m_Rand.setSeed(seed);
	float bulletOffset = ((float)pWorld->m_Rand.nextInt((int)(setting->bulletOffset * 100))) / 1000;
	int randInt = pWorld->m_Rand.nextInt(2);
	bulletOffset = randInt > 0 ? bulletOffset : -bulletOffset; //bullet offset
	const float range = setting->shootRange;

	Vector3 beginPos = pPlayer->position;
	beginPos.y += pPlayer->getEyeHeight();
	float yaw = pPlayer->rotationYaw;						// add offset you want.
	float pitch = pPlayer->rotationPitch + bulletOffset;

	dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
	dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
	dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));

	return dir;
}

bool ItemGun::traceWorldEntity(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer,RayTraceResult trace, float rangeRatio, float range,bool isNeedConsumeBullet)
{
	Vector3 beginPos = pPlayer->position + pPlayer->m_thirdPersonCameraOffset;
	Vector3 dir = calcDirPos(pStack, pWorld, pPlayer);
	Vector3 endPos = beginPos + dir * range;
	bool bResult = false;
	float min_dis = range;
	bool isHeadshot = false;

	Entity* tracyEntity = nullptr;
	EntityArr playerEntities = pWorld->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_LIVING_BASE, beginPos, dir, range * rangeRatio);
	for (int i = 0; i < int(playerEntities.size()); ++i)
	{
		Entity* pEntity = playerEntities[i];
		float distance = beginPos.distanceTo(pEntity->position);
		if (pEntity->canBePushed() && distance <= min_dis && pEntity != pPlayer)
		{
			tracyEntity = pEntity;
			min_dis = distance;
		}
	}

	if (tracyEntity)
	{
		if (LogicSetting::Instance()->getAllowHeadshotStatus() && tracyEntity->isClass(ENTITY_CLASS_PLAYER))
		{
			EntityPlayer* pEntityPlayer = dynamic_cast<EntityPlayer*>(tracyEntity);
			if (pEntityPlayer && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || pEntityPlayer->getTeamId() != pPlayer->getTeamId()))
			{
				isHeadshot = pWorld->isEntityHeadWithinRayTracy(Box(Vector3(tracyEntity->boundingBox.vMin.x, tracyEntity->boundingBox.vMin.y + tracyEntity->getHeight() * 5 / 7, tracyEntity->boundingBox.vMin.z), tracyEntity->boundingBox.vMax), beginPos, dir, range * rangeRatio);// head:body = 1:2.5
			}
		}

		if (tracyEntity->isClass(ENTITY_CLASS_CREATURE))
		{
			sendGunFire(tracyEntity, trace, RAYTRACE_TYPE_ENTITY, ENTITY_CREATUREAI_TRACE_TYPE, beginPos, dir, isNeedConsumeBullet, isHeadshot);
		}
		else
		{
			sendGunFire(tracyEntity, trace, RAYTRACE_TYPE_ENTITY, ENTITY_PLAYER_TRACE_TYPE, beginPos, dir, isNeedConsumeBullet, isHeadshot);
		}
		bResult = true;
	}

	return bResult;
}

bool ItemGun::traceWorldVehicle(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, RayTraceResult trace, float rangeRatio, float range, bool isNeedConsumeBullet)
{
	Vector3 beginPos = pPlayer->position + pPlayer->m_thirdPersonCameraOffset;
	Vector3 dir = calcDirPos(pStack, pWorld, pPlayer);
	Vector3 endPos = beginPos + dir * range;
	float min_dis = range;
	bool bResult = false;

	Entity* tracyEntityVehicle = nullptr;
	EntityArr VehicleEntities = pWorld->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_VEHICLE, beginPos, dir, range * rangeRatio);
	for (int i = 0; i < int(VehicleEntities.size()); ++i)
	{
		Entity* pEntity = VehicleEntities[i];
		float distance = beginPos.distanceTo(pEntity->position);
		if (pEntity->canBePushed() && distance <= min_dis)
		{
			tracyEntityVehicle = pEntity;
			min_dis = distance;
		}
	}

	if (tracyEntityVehicle)
	{
		sendGunFire(tracyEntityVehicle, trace, RAYTRACE_TYPE_ENTITY, ENTITY_VEHICLE_TRACE_TYPE, beginPos, dir, isNeedConsumeBullet, false);
		bResult = true;
	}

	return bResult;
}

bool ItemGun::traceWorldActorNpc(ItemStackPtr pStack, World * pWorld, EntityPlayer * pPlayer, RayTraceResult trace, float rangeRatio, float range, bool isNeedConsumeBullet)
{
	Vector3 beginPos = pPlayer->position + pPlayer->m_thirdPersonCameraOffset;
	Vector3 dir = calcDirPos(pStack, pWorld, pPlayer);
	Vector3 endPos = beginPos + dir * range;
	float min_dis = range;
	bool bResult = false;

	Entity* tracyActorNpc = nullptr;
	EntityArr ActorNpcEntitys = pWorld->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_ACTOR_NPC, beginPos, dir, range * rangeRatio);
	for (int i = 0; i < int(ActorNpcEntitys.size()); ++i)
	{
		Entity* pEntity = ActorNpcEntitys[i];
		float distance = beginPos.distanceTo(pEntity->position);
		if (pEntity->canBePushed() && distance <= min_dis)
		{
			tracyActorNpc = pEntity;
			min_dis = distance;
		}
	}

	if (tracyActorNpc)
	{
		sendGunFire(tracyActorNpc, trace, RAYTRACE_TYPE_ENTITY, ENTITY_ACTOR_NPC_TRACE_TYPE, beginPos, dir, isNeedConsumeBullet, false);
		bResult = true;
	}

	return bResult;
}


void ItemGun::fire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	const GunSetting* setting = getGunSetting();
	bool isNeedConsumeBullet = true;
	bool isHit = false;
	bool isNeedNoHitSend = true;
	if (!setting)
		return;

	// shot gun
	if (setting->gunType == SHOTGUN_GUN)
	{
		fireShotGun(pStack, pWorld, pPlayer);
	}
	// effect gun
	else if (setting->gunType >= RIFLE_EFFECT_GUN && setting->gunType <= MOMENTARY_PIERCING_LASER_EFFECT_GUN)
	{
		Vector3 beginPos = pPlayer->getPosition();
		beginPos.y += pPlayer->getEyeHeight();
		Vector3 endPos = Vector3::ZERO;
		if (getTarget(pStack, pWorld, pPlayer, endPos))
		{
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendEffectGunFire(isNeedConsumeBullet, beginPos, endPos);
			if (setting->gunType == LASER_EFFECT_GUN || setting->gunType == ELECTRICITY_EFFECT_GUN)
			{
				pPlayer->m_hasShotLaserGun = true;
			}
		}
	}
	else
	{
		// normal gun
		LastFire(pStack, pWorld, pPlayer, isNeedConsumeBullet, isNeedNoHitSend);
	}
}

void ItemGun::fireShotGun(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) 
{
	const GunSetting* setting = getGunSetting();
	if (!setting)
		return;
	bool isHit = false;
	bool isNeedConsumeBullet = true;
	bool isNeedNoHitSend = true;
	int i = 0;

	for (i = 0; i < setting->bulletPerShoot; i++)
	{
		if (i != 0)
		{
			isNeedConsumeBullet = false;
			isNeedNoHitSend = false;
		}
		if (!isHit)
		{
			isHit = LastFire(pStack, pWorld, pPlayer, isNeedConsumeBullet, isNeedNoHitSend);
		}
		else
		{
			LastFire(pStack, pWorld, pPlayer, isNeedConsumeBullet, isNeedNoHitSend);
		}
	}

	if (!isHit)
	{
		Entity* tracyEntity = nullptr;
		Vector3 beginPos = pPlayer->position;
		Vector3 dir = calcDirPos(pStack, pWorld, pPlayer);
		const float range = setting->shootRange;
		Vector3 endPos = beginPos + dir * range;
		RayTraceResult trace = pWorld->rayTraceBlocks(beginPos, endPos, false, true);
		int traceType = NONE_TRACE_TYPE;
		float rangeRatio = 1.0f;
		if (trace.result)
		{
			endPos = trace.hitVec;
			rangeRatio = beginPos.distanceTo(endPos) / range;
			traceType = BLOCK_TRACE_TYPE;
		}
		sendGunFire(tracyEntity, trace, RAYTRACE_TYPE_ENTITY, traceType, beginPos, dir, isNeedConsumeBullet, false);
	}
}

bool ItemGun::LastFire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, bool isNeedConsumeBullet, bool isNeedNoHitSend)
{
	const GunSetting* setting = getGunSetting();
	if (!setting)
		return false;

	const float range = setting->shootRange;
	Vector3 beginPos = pPlayer->position + pPlayer->m_thirdPersonCameraOffset;
	Vector3 dir = calcDirPos(pStack, pWorld, pPlayer);
	Vector3 endPos = beginPos + dir * range;
	float min_dis = range;
	float rangeRatio = 1.0f;

	int traceType = NONE_TRACE_TYPE;
	//trace the world's block.
	RayTraceResult trace = pWorld->rayTraceBlocks(beginPos, endPos, false, true);
	if (trace.result)
	{
		endPos = trace.hitVec;
		rangeRatio = beginPos.distanceTo(endPos) / range;
		traceType = BLOCK_TRACE_TYPE;
	}

	// trace the world's entites.
	bool bTraceEntity = traceWorldEntity(pStack, pWorld, pPlayer, trace, rangeRatio, range, isNeedConsumeBullet);
	if (bTraceEntity)
		return true;

	// trace the world's Vehicle.
	bool bTraceVehicle = traceWorldVehicle(pStack, pWorld, pPlayer, trace, rangeRatio, range, isNeedConsumeBullet);
	if (bTraceVehicle)
		return true;

	bool bTraceActorNpc = traceWorldActorNpc(pStack, pWorld, pPlayer, trace, rangeRatio, range, isNeedConsumeBullet);
	if (bTraceActorNpc)
		return true;

	if(isNeedNoHitSend)
		sendGunFire(nullptr, trace, RAYTRACE_TYPE_ENTITY, traceType, beginPos, dir, isNeedConsumeBullet, false);

	return false;
}

bool ItemGun::getTarget(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, Vector3& endPos)
{
	const GunSetting* setting = getGunSetting();
	if (!setting)
	{
		return false;
	}

	float range = setting->shootRange;

	Vector3 beginPos = pPlayer->position + pPlayer->m_thirdPersonCameraOffset;

	Vector3 dir = Vector3::ZERO;
	dir.x = -float(Math::Sin(double(pPlayer->rotationYaw) * Math::DEG2RAD) * Math::Cos(double(pPlayer->rotationPitch) * Math::DEG2RAD));
	dir.z = float(Math::Cos(double(pPlayer->rotationYaw) * Math::DEG2RAD) * Math::Cos(double(pPlayer->rotationPitch) * Math::DEG2RAD));
	dir.y = -float(Math::Sin(double(pPlayer->rotationPitch) * Math::DEG2RAD));

	endPos = beginPos + dir * range;
	float min_dis = range;
	float rangeRatio = 1.0f;

	RayTraceResult trace = pWorld->rayTraceBlocks(beginPos, endPos, false, true);
	if (trace.result)
	{
		endPos = trace.hitVec;
		min_dis = beginPos.distanceTo(endPos);
		if (min_dis <= range && range > 0.00001f)
		{
			rangeRatio = min_dis / range;
		}
		else
		{
			min_dis = range;
		}
	}

	Entity* tracyEntity = nullptr;
	EntityArr playerEntities = pWorld->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_ENTITY, beginPos, dir, range * rangeRatio);
	for (int i = 0; i < int(playerEntities.size()); ++i)
	{
		Entity* pEntity = playerEntities[i];
		float distance = beginPos.distanceTo(pEntity->position);
		if (pEntity->canBePushed() && distance <= min_dis && pEntity != pPlayer)
		{
			tracyEntity = pEntity;
			min_dis = distance;
		}
	}

	if (tracyEntity)
	{
		LORD::Ray ray(beginPos, endPos - beginPos);
		LORD::HitInfo hitinfo;
		float trim;
		const Box& aabb = tracyEntity->boundingBox;
		bool hit = ray.hitBox(aabb, trim, hitinfo);
		if (hit)
		{
			endPos = hitinfo.hitPos;
		}
	}

	return true;
}

bool ItemGun::sendGunFire(Entity* tracyEntity, RayTraceResult trace, RAYTRACE_TYPE sendTypeOfHit,int sendTraceType, Vector3 beginPos, Vector3 dir, bool isNeedConsumeBullet, bool isHeadshot) {
	int tracyEntityID = 0;
	int traceType = NONE_TRACE_TYPE;
	if (tracyEntity)
	{
		if (tracyEntity->isDead)
			return false;

		trace.result = true;
		trace.typeOfHit = sendTypeOfHit;
		trace.set(tracyEntity);

		// re-caculate the hit pos for entity.
		LORD::Ray ray(beginPos, dir);
		LORD::HitInfo hitinfo;
		float trim;
		const Box& aabb = tracyEntity->boundingBox;
		bool hit = ray.hitBox(aabb, trim, hitinfo);
		LordAssert(hit);
		trace.hitVec = hitinfo.hitPos;
		tracyEntityID = tracyEntity->entityId;
	}
	traceType = sendTraceType;
	// send message to server what we want.
	BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendGunFireResult(
		beginPos, dir, traceType, trace.blockPos, trace.hitVec, tracyEntityID, isNeedConsumeBullet, isHeadshot);
	return false;
}

ItemStackPtr ItemGun::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	ItemStackPtr bullet = pPlayer->inventory->getBulletItemStack(180);
	if (pPlayer->capabilities.isCreativeMode || bullet)
	{
		
	}

	return pStack;
}

const GunSetting* ItemGun::getGunSetting()
{
	return GunSetting::getGunSetting(itemID);
}

ItemBulletClip::ItemBulletClip(int id, int bulletClipId, const String& name)
	: Item(id, name)
	, bulletId(bulletClipId)
{
	maxStackSize = 1;
	const BulletClipSetting* setting = ItemBulletClip::getBulletClipSetting();
	if (!setting)
		return;
	maxStackSize = setting->maxStack;
}

const BulletClipSetting* ItemBulletClip::getBulletClipSetting()
{
	return BulletClipSetting::getBulletClipSetting(itemID);
}

bool ItemBulletClip::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	return false;
}

ItemStackPtr ItemBulletClip::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) {
	return nullptr;
}

ItemHandcuffs::ItemHandcuffs(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 100;
}

ItemCoal::ItemCoal(int id, const String& name)
	: Item(id, name)
{
	setHasSubtypes(true);
	setMaxDamage(0);
}

String ItemCoal::getUnlocalizedName(ItemStackPtr pStack)
{
	return pStack->getItemDamage() == 1 ? "item.charcoal" : "item.coal";
}

ItemSword::ItemSword(int id, const String& name, ToolMaterial* toolMat)
	: Item(id, name)
	, toolMaterial(toolMat)
{
	maxStackSize = 1;
	setMaxDamage(toolMat->getMaxUses());
	weaponDamage = 3.0F + toolMat->getDamageVsEntity();

	m_modifierAttr = LordNew AttributeModifier(ATTACK_DAMAGE_MODIFIER, "Weapon modifier", weaponDamage, 0);
}

float ItemSword::getStrVsBlock(ItemStackPtr pStack, Block* pBlock)
{
	if (pBlock->getBlockID() == BLOCK_ID_WEB)
	{
		return 15.0F;
	}
	
	const BM_Material& mat = pBlock->getMaterial();
	return (
		mat != BM_Material::BM_MAT_plants && 
		mat != BM_Material::BM_MAT_vine &&
		mat != BM_Material::BM_MAT_coral &&
		mat != BM_Material::BM_MAT_leaves &&
		mat != BM_Material::BM_MAT_pumpkin)
		? 1.0F : 1.5F;
}

bool ItemSword::hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2)
{
	pStack->damageItem(1, pLiving2);
	return true;
}

bool ItemSword::onBlockDestroyed(ItemStackPtr pStack, World* pWorld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving)
{
	if (BlockManager::sBlocks[blockID]->getBlockHardness(pWorld, pos) != 0.0f)
	{
		pStack->damageItem(2, pLiving);
	}
	return true;
}

ItemStackPtr ItemSword::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	return pStack;
}

bool ItemSword::canHarvestBlock(Block* pBlock)
{
	return pBlock->getBlockID() == BLOCK_ID_WEB;
}

bool ItemSword::getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return toolMaterial->getToolCraftingMaterial() == pStack2->itemID ? true : Item::getIsRepairable(pStack1, pStack2);
}

void ItemSword::getAttributeModifiers(AttributeModifierMap& outmap)
{
	outmap.insert(std::make_pair(SharedMonsterAttributes::ATTACK_DAMAGE->getName(), m_modifierAttr));
}

ItemReed::ItemReed(int id, const String& name, int reedid)
	: Item(id, name)
	, spawnID(reedid)
{
}

bool ItemReed::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	int blockID = pWorld->getBlockId(pos);

	BlockPos pos1 = pos;
	if (blockID == BLOCK_ID_SNOW && (pWorld->getBlockMeta(pos) & 7) < 1)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		if (face == BM_FACE_DOWN)
			--pos1.y;
		else if (face == BM_FACE_UP)
			++pos1.y;
		else if (face == BM_FACE_NORTH)
			--pos1.z;
		else if (face == BM_FACE_SOUTH)
			++pos1.z;
		else if (face == BM_FACE_WEST)
			--pos1.x;
		else if (face == BM_FACE_EAST)
			++pos1.x;
	}

	if (!pPlayer->canPlayerEdit(pos1, face, pStack))
	{
		return false;
	}
	else if (pStack->stackSize == 0)
	{
		return false;
	}
	else
	{
		if (pWorld->canPlaceEntityOnSide(spawnID, pos1, false, face, NULL, pStack))
		{
			Block* pBlock = BlockManager::sBlocks[spawnID];
			int meta = pBlock->onBlockPlaced(pWorld, pos1, face, hit, 0);

			if (pWorld->setBlock(pos1, spawnID, meta, 3))
			{
				if (pWorld->getBlockId(pos1) == spawnID)
				{
					BlockManager::sBlocks[spawnID]->onBlockPlacedBy(pWorld, pos1, pPlayer, pStack);
					BlockManager::sBlocks[spawnID]->onPostBlockPlaced(pWorld, pos1, meta);
				}

				pWorld->playSoundOfPlaceBlock(pos1, spawnID);
				--pStack->stackSize;
			}
		}

		return true;
	}
}

ItemHoe::ItemHoe(int id, const String& name, ToolMaterial* toolMat)
	: Item(id, name)
	, theToolMaterial(toolMat)
{
	maxStackSize = 1;
	setMaxDamage(toolMat->getMaxUses());
}

bool ItemHoe::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (!pPlayer->canPlayerEdit(pos, face, pStack))
		return false;

	int blockID = pWorld->getBlockId(pos);
	int upperID = pWorld->getBlockId(pos.getPosY());

	if (face != 0 && upperID == 0 && (blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT))
	{
		pWorld->playSoundOfStepOnBlock(pos, BLOCK_ID_TILLED_FIELD);

		if (pWorld->m_isClient)
		{
			return true;
		}
		else
		{
			pWorld->setBlock(pos, BLOCK_ID_TILLED_FIELD);
			pStack->damageItem(1, pPlayer);
			return true;
		}
	}

	return false;
}


ItemSeeds::ItemSeeds(int id, const String& name, int blocktype, int soilID)
	: Item(id, name)
	, blockType(blocktype)
	, soilBlockID(soilID)
{
}

bool ItemSeeds::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face != BM_FACE_UP)
		return false;

	if (pPlayer->canPlayerEdit(pos, face, pStack) &&
		pPlayer->canPlayerEdit(pos.getPosY(), face, pStack))
	{
		int blockID = pWorld->getBlockId(pos);

		if (blockID == soilBlockID && pWorld->isAirBlock(pos.getPosY()))
		{
			auto usePos = pos.getPosY();
			pWorld->setBlock(usePos, blockType);
			--pStack->stackSize;
			OnUseItemSeedsEvent::emit(pPlayer, usePos, blockType);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

RanchSeeds::RanchSeeds(int id, const String & name, int blocktype, int soilID)
	:ItemSeeds(id, name, blocktype, soilID)
{
}

ItemHangingEntity::ItemHangingEntity(int id, const String& name, int classID)
	: Item(id, name)
	, hangingClassID(classID)
{
}

bool ItemHangingEntity::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face == BM_FACE_DOWN ||
		face == BM_FACE_UP)
		return false;

	int dir = dir_facingToDirection[face];
	if (!pPlayer->canPlayerEdit(pos, face, pStack))
		return false;
	
	// todo.
	/*
	EntityHanging* pHanging = createHangingEntity(pWorld, x, y, z, dir);
	if (pHanging != NULL && pHanging->onValidSurface())
	{
		if (!pWorld->m_isRemote)
		{
			pWorld->spawnEntityInWorld(pHanging);
		}

		--pStack->stackSize;
	}
	*/

	return true;
}

EntityHanging* ItemHangingEntity::createHangingEntity(World* pWorld, const BlockPos& pos, int dir)
{
	// todo.
	/*if (hangingClassID == 1)
		return LordNew EntityPainting(pWorld, x, y, z, dir);
	else if (hangingClassID == 2)
		return LordNew EntityItemFrame(pWorld, x, y, z, dir);*/

	return NULL;
}

ItemSign::ItemSign(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 16;
}

bool ItemSign::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face == BM_FACE_DOWN)
		return false;

	if (!pWorld->getBlockMaterial(pos).isSolid())
		return false;

	BlockPos pos1 = pos;
	if (face == BM_FACE_UP)
		++pos1.y;
	if (face == BM_FACE_NORTH)
		--pos1.z;
	if (face == BM_FACE_SOUTH)
		++pos1.z;
	if (face == BM_FACE_WEST)
		--pos1.x;
	if (face == BM_FACE_EAST)
		++pos1.x;

	if (!pPlayer->canPlayerEdit(pos1, face, pStack))
	{
		return false;
	}
	else if (!BlockManager::signPost->canPlaceBlockAt(pWorld, pos1))
	{
		return false;
	}
	else if (pWorld->m_isClient)
	{
		return true;
	}
	else
	{
		if (face == 1)
		{
			int meta = int(Math::Floor(((pPlayer->rotationYaw + 180.0F) * 16.0F / 360.0F) + 0.5F)) & 15;
			pWorld->setBlock(pos1, BLOCK_ID_SIGN_POST, meta, 3);
		}
		else
		{
			pWorld->setBlock(pos1, BLOCK_ID_SIGN_WALL, face, 3);
		}

		--pStack->stackSize;
		// todo.
		/*
		TileEntitySign* pSign = (TileEntitySign*)pWorld->getBlockTileEntity(x, y, z);
		if (pSign != NULL)
		{
			pPlayer->displayGUIEditSign(pSign);
		}
		*/

		return true;
	}
}

ItemDoor::ItemDoor(int id, const String& name, const BM_Material& mat)
	: Item(id, name)
	, doorMaterial(mat)
{
	maxStackSize = 1;
}

bool ItemDoor::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face != BM_FACE_UP)
		return false;

	BlockPos pos1 = pos;
	++pos1.y;
	Block* pBlock;

	if (doorMaterial == BM_Material::BM_MAT_wood)
	{
		pBlock = BlockManager::doorWood;
	}
	else
	{
		pBlock = BlockManager::doorIron;
	}

	if (pPlayer->canPlayerEdit(pos1, face, pStack) && 
		pPlayer->canPlayerEdit(pos1.getPosY(), face, pStack))
	{
		if (!pBlock->canPlaceBlockAt(pWorld, pos1))
		{
			return false;
		}
		else
		{
			int meta = int(Math::Floor(((pPlayer->rotationYaw + 180.0F) * 4.0F / 360.0F) - 0.5f)) & 3;
			placeDoorBlock(pWorld, pos1, meta, pBlock);
			--pStack->stackSize;
			return true;
		}
	}
	return false;
}

void ItemDoor::placeDoorBlock(World* pWorld, const BlockPos& pos, int dir, Block* pBlock)
{
	int offsetx = 0;
	int offsetz = 0;

	if (dir == 0)
		offsetz = 1;
	if (dir == 1)
		offsetx = -1;
	if (dir == 2)
		offsetz = -1;
	if (dir == 3)
		offsetx = 1;

	int var8 =  (pWorld->isBlockNormalCube(pos + BlockPos(-offsetx, 0, -offsetz)) ? 1 : 0) +
				(pWorld->isBlockNormalCube(pos + BlockPos(-offsetx, 1, -offsetz)) ? 1 : 0);
	int var9 =  (pWorld->isBlockNormalCube(pos + BlockPos(offsetx, 0, offsetz)) ? 1 : 0) +
				(pWorld->isBlockNormalCube(pos + BlockPos(offsetx, 1, offsetz)) ? 1 : 0);
	bool var10 = pWorld->getBlockId(pos + BlockPos(-offsetx, 0, -offsetz)) == pBlock->getBlockID() ||
				 pWorld->getBlockId(pos + BlockPos(-offsetx, 1, -offsetz)) == pBlock->getBlockID();
	bool var11 = pWorld->getBlockId(pos + BlockPos(offsetx, 0, offsetz)) == pBlock->getBlockID() || 
				 pWorld->getBlockId(pos + BlockPos(offsetx, 1, offsetz)) == pBlock->getBlockID();
	bool var12 = false;

	if (var10 && !var11)
	{
		var12 = true;
	}
	else if (var9 > var8)
	{
		var12 = true;
	}

	pWorld->setBlock(pos, pBlock->getBlockID(), dir, 2);
	pWorld->setBlock(pos.getPosY(), pBlock->getBlockID(), 8 | (var12 ? 1 : 0), 2);
	pWorld->notifyBlocksOfNeighborChange(pos, pBlock->getBlockID());
	pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), pBlock->getBlockID());
}

vector<Vector3i>::type ItemDoor::getPlacePositions(World * pWorld, BlockPos& pos, int face)
{
	vector<Vector3i>::type positions = vector<Vector3i>::type();
	int blockID = pWorld->getBlockId(pos);
	if (blockID == BLOCK_ID_SNOW)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos.y; break;
		case BM_FACE_UP:	++pos.y; break;
		case BM_FACE_NORTH: --pos.z; break;
		case BM_FACE_SOUTH: ++pos.z; break;
		case BM_FACE_WEST:	--pos.x; break;
		case BM_FACE_EAST:	++pos.x; break;
		default: break;
		}
	}
	positions.push_back(pos);
	BlockPos pos2 = Vector3i(pos.x, pos.y + 1, pos.z);
	positions.push_back(pos2);
	return positions;
}

ItemEnderPearl::ItemEnderPearl(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 16;
}

ItemStackPtr ItemEnderPearl::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (pPlayer->capabilities.isCreativeMode)
		return pStack;

	--pStack->stackSize;
	//pWorld->playSoundAtEntity(pPlayer, "random.bow", 0.5F, 0.4F / (itemRand->nextFloat() * 0.4F + 0.8F));
	pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);

	// todo.
	/*if (!pWorld->m_isRemote)
	{
		pWorld->spawnEntityInWorld(LordNew EntityEnderPearl(pWorld, pPlayer));
	}*/

	return pStack;
}

ItemGlassBottle::ItemGlassBottle(int id, const String& name)
	: Item(id, name)
{
}

ItemStackPtr ItemGlassBottle::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	RayTraceResult trace = getMovingObjectPositionFromPlayer(pWorld, pPlayer, true);

	if (!trace.result)
		return pStack;

	if (trace.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		BlockPos pos = trace.getBlockPos();

		if (!pWorld->canMineBlock(pPlayer, pos))
		{
			return pStack;
		}

		if (!pPlayer->canPlayerEdit(pos, trace.sideHit->getIndex(), pStack))
		{
			return pStack;
		}

		if (pWorld->getBlockMaterial(pos) == BM_Material::BM_MAT_water)
		{
			--pStack->stackSize;

			if (pStack->stackSize <= 0)
			{
				return LORD::make_shared<ItemStack>(Item::potion);
			}

			if (!pPlayer->inventory->addItemStackToInventory(LORD::make_shared<ItemStack>(Item::potion)))
			{
				pPlayer->dropPlayerItem(LORD::make_shared<ItemStack>(Item::potion->itemID, 1, 0));
			}
		}
	}

	return pStack;
}

ItemEnderEye::ItemEnderEye(int id, const String& name)
	: Item(id, name)
{
}

bool ItemEnderEye::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	// todo.
	return false;
}

ItemStackPtr ItemEnderEye::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	RayTraceResult rayTrace = getMovingObjectPositionFromPlayer(pWorld, pPlayer, false);

	if (rayTrace.result && rayTrace.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		int blockID = pWorld->getBlockId(rayTrace.getBlockPos());

		if (blockID == BLOCK_ID_END_PROTAL_FRAME)
		{
			return pStack;
		}
	}

	if (!pWorld->m_isClient)
	{
		//ChunkPosition chunkPos = pWorld->findClosestStructure("Stronghold", pPlayer->position.getFloor());

		//EntityEnderEye var6 = new EntityEnderEye(pWorld, pPlayer.posX, pPlayer.posY + 1.62D - (double)pPlayer.yOffset, pPlayer.posZ);
		//var6.moveTowards(chunkPos);
		//pWorld->spawnEntityInWorld(var6);
		//pWorld->playSoundAtEntity(pPlayer, "random.bow", 0.5F, 0.4F / (itemRand->nextFloat() * 0.4F + 0.8F));
		pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);
		pWorld->playAuxSFXAtEntity(nullptr, 1002, pPlayer->position.getFloor(), 0);

		if (!pPlayer->capabilities.isCreativeMode)
		{
			--pStack->stackSize;
		}
	}

	return pStack;
}

ItemMonsterPlacer::ItemMonsterPlacer(int id, const String& name)
	: Item(id, name)
{
	setHasSubtypes(true);
}

String ItemMonsterPlacer::getItemDisplayName(ItemStackPtr pStack)
{
	return Item::getItemDisplayName(pStack);
	// todo.
	/*String var2 = ("" + StatCollector.translateToLocal(this.getUnlocalizedName() + ".name")).trim();
	String var3 = EntityList::getStringFromID(pStack.getItemDamage());

	if (var3 != null)
	{
		var2 = var2 + " " + StatCollector.translateToLocal("entity." + var3 + ".name");
	}

	return var2;*/
}

bool ItemMonsterPlacer::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	int blockID = pWorld->getBlockId(pos);
	BlockPos pos1 = pos + offsetForSide[face];
	float offsetY = 0.0f;

	if (face == 1 && 
		BlockManager::sBlocks[blockID] != NULL &&
		blockID == BLOCK_ID_FENCE)
	{
		offsetY = 0.5f;
	}

	Entity* pEntity = spawnCreature(pWorld, pStack->getItemDamage(), Vector3(pos1) + Vector3(0.5f, offsetY, 0.5f));

	if (pEntity != NULL)
	{
		// todo.
		//EntityLiving* pliving = dynamic_cast<EntityLivingBase*>(pEntity);
		//if (pliving && pStack->hasDisplayName())
		//{
		//	// todo.
		//	// ((EntityLiving*)pEntity)->setCustomNameTag(pStack->getDisplayName());
		//}

		if (!pPlayer->capabilities.isCreativeMode)
		{
			--pStack->stackSize;
		}
	}

	return true;
}

ItemStackPtr ItemMonsterPlacer::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (pWorld->m_isClient)
		return pStack;

	RayTraceResult rayTrace = getMovingObjectPositionFromPlayer(pWorld, pPlayer, true);

	if (!rayTrace.result)
		return pStack;

	if (rayTrace.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		BlockPos pos = rayTrace.getBlockPos();

		if (!pWorld->canMineBlock(pPlayer, pos))
		{
			return pStack;
		}

		if (!pPlayer->canPlayerEdit(pos, rayTrace.sideHit->getIndex(), pStack))
		{
			return pStack;
		}

		if (pWorld->getBlockMaterial(pos) == BM_Material::BM_MAT_water)
		{
			Entity* pEntity = spawnCreature(pWorld, pStack->getItemDamage(), Vector3(pos));

			if (pEntity != NULL)
			{
				// todo.
				//if (pEntity->isClass(ENTITY_CLASS_LIVING) && pStack->hasDisplayName())
				//{
				//	
				//	// ((EntityLiving*)pEntity).setCustomNameTag(pStack->getDisplayName());
				//}

				if (!pPlayer->capabilities.isCreativeMode)
				{
					--pStack->stackSize;
				}
			}
		}
	}

	return pStack;
}

Entity* ItemMonsterPlacer::spawnCreature(World* pWorld, int entityID, const Vector3& pos)
{
	// todo.
	return NULL;
}

int ItemArmor::maxDamageArray[MAX_ARMOR_COUNT] = {11, 16, 15, 13};
// todo.
// IBehaviorDispenseItem* ItemArmor::behaviorDispenseArmor = LordNew BehaviorDispenseArmor();

ItemArmor::ItemArmor(int id, const String& name, ArmorMaterial* armorMat, int armortype)
	: Item(id, name)
	, material(armorMat)
	, armorType(armortype)
{
	damageReduceAmount = armorMat->getDamageReductionAmount(armortype);
	setMaxDamage(armorMat->getDurability(armortype));
	maxStackSize = 1;
	// todo.
	// BlockDispenser.dispenseBehaviorRegistry.putObject(this, behaviorDispenseArmor);
}

bool ItemArmor::hasColor(ItemStackPtr pStack)
{
	if (material != ArmorMaterial::CLOTH)
		return false;
	
	if (!pStack->hasTagCompound())
		return false;

	if (!pStack->getTagCompound()->hasKey("display"))
		return false;

	return pStack->getTagCompound()->getCompoundTag("display")->hasKey("color");
}

int ItemArmor::getColor(ItemStackPtr pStack)
{
	if (material != ArmorMaterial::CLOTH)
		return -1;

	NBTTagCompound* pCompound = pStack->getTagCompound();
	if (!pCompound)
		return 0x00A06540;

	NBTTagCompound* pdisplay = pCompound->getCompoundTag("display");
	if (!pdisplay)
		return 0x00A06540;

	if (pdisplay->hasKey("color"))
		return pdisplay->getInteger("color");
	
	return 0x00A06540;
}

void ItemArmor::removeColor(ItemStackPtr pStack)
{
	if (material == ArmorMaterial::CLOTH)
	{
		NBTTagCompound* pCompound = pStack->getTagCompound();

		if (pCompound)
		{
			NBTTagCompound* pDisplay = pCompound->getCompoundTag("display");

			if (pDisplay->hasKey("color"))
			{
				pDisplay->removeTag("color");
			}
		}
	}
}

void ItemArmor::setColor(ItemStackPtr pStack, int color)
{
	LordAssert(material == ArmorMaterial::CLOTH);

	NBTTagCompound* pCompound = pStack->getTagCompound();

	if (pCompound)
	{
		pCompound = LordNew NBTTagCompound();
		pStack->setTagCompound(pCompound);
	}

	NBTTagCompound* pDisplay = pCompound->getCompoundTag("display");

	if (!pCompound->hasKey("display"))
	{
		pCompound->setCompoundTag("display", pDisplay);
	}

	pDisplay->setInteger("color", color);
}

bool ItemArmor::getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return material->getArmorCraftingMaterial() == pStack2->itemID ? true : Item::getIsRepairable(pStack1, pStack2);
}

ItemStackPtr ItemArmor::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	// todo.
	/*int armorIdx = EntityLiving::getArmorPosition(pStack) - 1;
	ItemStackPtr pArmor = pPlayer->getCurrentArmor(armorIdx);

	if (!pArmor)
	{
		pPlayer->setCurrentItemOrArmor(armorIdx, pStack->copy());
		pStack->stackSize = 0;
	}*/

	return pStack;
}

ItemPotion::EffectCacheMap ItemPotion::field_77835_b;

ItemPotion::ItemPotion(int id, const String& name, bool isSplash/* = false*/)
	: Item(id, name)
{
	setMaxStackSize(1);
	setHasSubtypes(true);
	setMaxDamage(0);
	m_isSplash = isSplash;
}

void ItemPotion::getEffects(ItemStackPtr pStack, PotionEffectArr& outArr)
{/*
	LordAssert(outArr.empty());

	if (pStack->hasTagCompound() && pStack->getTagCompound()->hasKey("CustomPotionEffects"))
	{
		NBTTagList* pPotionEffects = pStack->getTagCompound()->getTagList("CustomPotionEffects");

		for (int i = 0; i < pPotionEffects->tagCount(); ++i)
		{
			NBTTagCompound* pPotionEffect = (NBTTagCompound*)pPotionEffects->tagAt(i);
			outArr.push_back(PotionEffect::readCustomPotionEffectFromNBT(pPotionEffect));
		}
	}
	else
	{
		EffectCacheMap::iterator it = effectCache.find(pStack->getItemDamage());
		
		if (it == effectCache.end())
		{
			PotionHelper::getPotionEffects(pStack->getItemDamage(), false, outArr);
			effectCache.insert(std::make_pair(pStack->getItemDamage(), outArr));
		}
		else
		{
			outArr = it->second;
		}
	}*/
}

void ItemPotion::getEffects(int damageType, PotionEffectArr& outArr)
{
	/*
	EffectCacheMap::iterator it = effectCache.find(damageType);
	if(it == effectCache.end())
	{
		PotionHelper::getPotionEffects(damageType, false, outArr);
		effectCache.insert(std::make_pair(damageType, outArr));
	}
	else
	{
		outArr = it->second;
	}*/
}

void ItemPotion::getEffects(PotionEffectArr& outArr)
{
	for (auto i_e : m_ownEffects) {
		outArr.push_back(i_e.second);
	}
}

ItemStackPtr ItemPotion::onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pPlayer->capabilities.isCreativeMode)
	{
		--pStack->stackSize;
	}

	if (!pWorld->m_isClient)
	{
		/*
		PotionEffectArr effects;
		getEffects(pStack, effects);

		for (PotionEffectArr::iterator it = effects.begin(); it != effects.end(); ++it)
		{
			PotionEffect* effect = *it;
			pPlayer->addPotionEffect(LordNew PotionEffect(*effect));
		}
		*/
		for (const auto& it : m_ownEffects) {
			PotionEffect* effect = it.second;
			if (Potion::potionTypes[effect->getPotionID()]->isInstant()) {
				Potion::potionTypes[effect->getPotionID()]->affectEntity(pPlayer, pPlayer, pPlayer, effect->getAmplifier(), 1.0);
			}
			else {
				pPlayer->addPotionEffect(LordNew PotionEffect(*effect));
			}
		}
	}

	if (!pPlayer->capabilities.isCreativeMode)
	{

		if (!SCRIPT_EVENT::PotionConvertGlassBottleEvent::invoke(pPlayer->hashCode(), pStack->itemID))
			return pStack;

		if (pStack->stackSize <= 0)
		{
			return LORD::make_shared<ItemStack>(Item::glassBottle);
		}

		pPlayer->inventory->addItemStackToInventory(LORD::make_shared<ItemStack>(Item::glassBottle));
	}

	return pStack;
}

ItemStackPtr ItemPotion::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (isSplash())
	{
		if (pWorld->m_isClient) {
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()
				->sendSplashPotion(pPlayer->position + Vector3(0.f, pPlayer->getEyeHeight(), 0.f),
					pPlayer->rotationYaw, pPlayer->rotationPitch);
		}

		return pStack;
	}
	else
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		return pStack;
	}
}

int ItemPotion::getColorFromDamage(int damage)
{
	return PotionHelper::getColorFromDamage(damage, false);
}

bool ItemPotion::isEffectInstant(int damage)
{
	/*
	PotionEffectArr arr;
	getEffects(damage, arr);

	if (arr.empty())
		return false;

	for (int i = 0; i < int(arr.size()); ++i)
	{
		PotionEffect* effect = arr[i];
		if (Potion::potionTypes[effect->getPotionID()]->isInstant())
			return true;
	}
	return false;
	*/
	if (m_ownEffects.empty()) {
		return false;
	}

	for (const auto& it : m_ownEffects) {
		PotionEffect* effect = it.second;
		if (Potion::potionTypes[effect->getPotionID()]->isInstant()) {
			return true;
		}
	}
	return false;
}

String ItemPotion::getItemDisplayName(ItemStackPtr pStack)
{
	return Item::getItemDisplayName(pStack);
	// todo.
	/*if (pStack.getItemDamage() == 0)
	{
		return StatCollector.translateToLocal("item.emptyPotion.name").trim();
	}
	else
	{
		String var2 = "";

		if (isSplash(pStack.getItemDamage()))
		{
			var2 = StatCollector.translateToLocal("potion.prefix.grenade").trim() + " ";
		}

		List var3 = Item.potion.getEffects(pStack);
		String var4;

		if (var3 != null && !var3.isEmpty())
		{
			var4 = ((PotionEffect)var3.get(0)).getEffectName();
			var4 = var4 + ".postfix";
			return var2 + StatCollector.translateToLocal(var4).trim();
		}
		else
		{
			var4 = PotionHelper.func_77905_c(pStack.getItemDamage());
			return StatCollector.translateToLocal(var4).trim() + " " + super.getItemDisplayName(pStack);
		}
	}*/
}

bool ItemPotion::hasEffect(ItemStackPtr pStack)
{
	/*
	PotionEffectArr arr;
	getEffects(pStack, arr);
	return !arr.empty();
	*/
	if (m_ownEffects.empty()) {
		return false;
	}
	else {
		return true;
	}
}

void ItemPotion::addEffect(PotionEffect * effect)
{
	EffectMap::iterator it = m_ownEffects.find(effect->getPotionID());
	if (it != m_ownEffects.end()) {
		it->second->combine(effect);
	}
	else {
		m_ownEffects.insert(std::make_pair(effect->getPotionID(), effect));
	}
}

ItemEnchantedBook::ItemEnchantedBook(int id, const String& name)
	: Item(id, name)
{
}

RARITY_TYPE ItemEnchantedBook::getRarity(ItemStackPtr pStack)
{
	NBTTagList* pNbtLst = getStoredEnchantments(pStack);
	if (pNbtLst && pNbtLst->tagCount() > 0)
		return RARITY_TYPE_UNCOMMON;
	return Item::getRarity(pStack);
}

NBTTagList* ItemEnchantedBook::getStoredEnchantments(ItemStackPtr pStack)
{
	if (pStack->stackTagCompound != NULL && pStack->stackTagCompound->hasKey("StoredEnchantments"))
		return (NBTTagList*)pStack->stackTagCompound->getTag("StoredEnchantments");

	return LordNew NBTTagList();
}

void ItemEnchantedBook::getEnchantedItemStack_do(ItemStackPtr pStack, EnchantmentData* pEnchData)
{
	NBTTagList* pStoreEnchs = getStoredEnchantments(pStack);
	bool isNewEnch = true;

	for (int i = 0; i < pStoreEnchs->tagCount(); ++i)
	{
		NBTTagCompound* pStoreEnch = (NBTTagCompound*)pStoreEnchs->tagAt(i);

		if (pStoreEnch->getShort("id") == pEnchData->enchantmentobj->effectId)
		{
			if (pStoreEnch->getShort("lvl") < pEnchData->enchantmentLevel)
			{
				pStoreEnch->setShort("lvl", (i16)pEnchData->enchantmentLevel);
			}

			isNewEnch = false;
			break;
		}
	}

	if (isNewEnch)
	{
		NBTTagCompound* pNewEnch = LordNew NBTTagCompound();
		pNewEnch->setShort("id", (i16)pEnchData->enchantmentobj->effectId);
		pNewEnch->setShort("lvl", (i16)pEnchData->enchantmentLevel);
		pStoreEnchs->appendTag(pNewEnch);
	}

	if (!pStack->hasTagCompound())
	{
		pStack->setTagCompound(LordNew NBTTagCompound());
	}

	pStack->getTagCompound()->setTag("StoredEnchantments", pStoreEnchs);
}

ItemStackPtr ItemEnchantedBook::getEnchantedItemStack(EnchantmentData* pEnchData)
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(this);
	getEnchantedItemStack_do(pStack, pEnchData);
	return pStack;
}

void ItemEnchantedBook::func_92113_a(Enchantment* pEnch, ItemStackArr& outarr)
{
	for (int lvl = pEnch->getMinLevel(); lvl <= pEnch->getMaxLevel(); ++lvl)
	{
		outarr.push_back(getEnchantedItemStack(LordNew EnchantmentData(pEnch, lvl)));
	}
}

WeightedRandomChestContent* ItemEnchantedBook::func_92114_b(Random* par1Random)
{
	return func_92112_a(par1Random, 1, 1, 1);
}

WeightedRandomChestContent* ItemEnchantedBook::func_92112_a(Random* pRand, int par2, int par3, int par4)
{
	Enchantment* pEnch = NULL;
	while(!pEnch) 
		pEnch = Enchantment::enchantmentsBookList[pRand->nextInt(Enchantment::MAX_ENCH_COUNT)];

	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, 1, 0);

	int maxlvl = pEnch->getMaxLevel();
	int minlvl = pEnch->getMinLevel();
	int lvl = minlvl;
	if (maxlvl > minlvl)
		lvl = pRand->nextInt(maxlvl - minlvl) + minlvl;
	getEnchantedItemStack_do(pStack, LordNew EnchantmentData(pEnch, lvl));
	return LordNew WeightedRandomChestContent(pStack, par2, par3, par4);
}

String ItemDye::dyeColorNames[MAX_DYE_NUM] = { "black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightBlue", "magenta", "orange", "white" };
int ItemDye::dyeColors[MAX_DYE_NUM] = { 1973019, 11743532, 3887386, 5320730, 2437522, 8073150, 2651799, 11250603, 4408131, 14188952, 4312372, 14602026, 6719955, 12801229, 15435844, 15790320 };

ItemDye::ItemDye(int id, const String& name)
	: Item(id, name)
{
	setHasSubtypes(true);
	setMaxDamage(0);
}

String ItemDye::getUnlocalizedName(ItemStackPtr pStack)
{
	int idx = Math::Clamp(pStack->getItemDamage(), 0, 15);
	return Item::getUnlocalizedName() + "." + dyeColorNames[idx];
}

bool ItemDye::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (!pPlayer->canPlayerEdit(pos, face, pStack))
		return false;


	if (pStack->getItemDamage() == 15)
	{
		if (applyBonemeal(pStack, pWorld, pos))
		{
			if (!pWorld->m_isClient)
			{
				// pWorld->playAuxSFX(2005, x, y, z, 0);
			}

			return true;
		}
	}
	else if (pStack->getItemDamage() == 3)
	{
		int blockID = pWorld->getBlockId(pos);
		int metadata = pWorld->getBlockMeta(pos);

		if (BlockLogBase::isWood(blockID) && BlockLogBase::limitToValidMetadata(metadata) == 3)
		{
			if (face == BM_FACE_DOWN || face == BM_FACE_UP)
				return false;

			BlockPos pos1 = pos;
			if (face == BM_FACE_NORTH)
				--pos1.z;
			if (face == BM_FACE_NORTH)
				++pos1.z;
			if (face == BM_FACE_WEST)
				--pos1.x;
			if (face == BM_FACE_EAST)
				++pos1.x;

			if (pWorld->isAirBlock(pos1))
			{
				int placedID = BlockManager::cocoaPlant->onBlockPlaced(pWorld, pos1, face, hit, 0);
				pWorld->setBlock(pos1, BLOCK_ID_COCOAPLANT, placedID, 2);

				if (!pPlayer->capabilities.isCreativeMode)
				{
					--pStack->stackSize;
				}
			}

			return true;
		}
	}

	return false;
}

bool ItemDye::applyBonemeal(ItemStackPtr pStack, World* pWorld, const BlockPos& pos)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID == BLOCK_ID_SAPLING)
	{
		if (!pWorld->m_isClient)
		{
			if (pWorld->m_Rand.nextFloat() < 0.45f)
			{
				// todo.
				// ((BlockSapling)Block.sapling).markOrGrowMarked(pWorld, x, y, z, pWorld.rand);
			}

			--pStack->stackSize;
		}

		return true;
	}
	else if (blockID != BLOCK_ID_MUSHROOM_BROWN && blockID != BLOCK_ID_MUSHROOM_RED)
	{
		if (blockID != BLOCK_ID_MELON_STEM && blockID != BLOCK_ID_PUMPKIN_STEM)
		{
			// todo.
			/*
			if (blockID > 0 && Block.blocksList[blockID] instanceof BlockCrops)
			{
				if (pWorld.getBlockMetadata(x, y, z) == 7)
				{
					return false;
				}
				else
				{
					if (!pWorld.isRemote)
					{
						((BlockCrops)Block.blocksList[blockID]).fertilize(pWorld, x, y, z);
						--pStack.stackSize;
					}

					return true;
				}
			}
			else
			{
				int var6;
				int var7;
				int var8;

				if (blockID == Block.cocoaPlant.blockID)
				{
					var6 = pWorld.getBlockMetadata(x, y, z);
					var7 = BlockDirectional.getDirection(var6);
					var8 = BlockCocoa.func_72219_c(var6);

					if (var8 >= 2)
					{
						return false;
					}
					else
					{
						if (!pWorld.isRemote)
						{
							++var8;
							pWorld.setBlockMetadataWithNotify(x, y, z, var8 << 2 | var7, 2);
							--pStack.stackSize;
						}

						return true;
					}
				}
				else if (blockID != Block.grass.blockID)
				{
					return false;
				}
				else
				{
					if (!pWorld.isRemote)
					{
						--pStack.stackSize;
					label102:

						for (var6 = 0; var6 < 128; ++var6)
						{
							var7 = x;
							var8 = y + 1;
							int var9 = z;

							for (int var10 = 0; var10 < var6 / 16; ++var10)
							{
								var7 += itemRand.nextInt(3) - 1;
								var8 += (itemRand.nextInt(3) - 1) * itemRand.nextInt(3) / 2;
								var9 += itemRand.nextInt(3) - 1;

								if (pWorld.getBlockId(var7, var8 - 1, var9) != Block.grass.blockID || pWorld.isBlockNormalCube(var7, var8, var9))
								{
									continue label102;
								}
							}

							if (pWorld.getBlockId(var7, var8, var9) == 0)
							{
								if (itemRand.nextInt(10) != 0)
								{
									if (Block.tallGrass.canBlockStay(pWorld, var7, var8, var9))
									{
										pWorld.setBlock(var7, var8, var9, Block.tallGrass.blockID, 1, 3);
									}
								}
								else if (itemRand.nextInt(3) != 0)
								{
									if (Block.plantYellow.canBlockStay(pWorld, var7, var8, var9))
									{
										pWorld.setBlock(var7, var8, var9, Block.plantYellow.blockID);
									}
								}
								else if (Block.plantRed.canBlockStay(pWorld, var7, var8, var9))
								{
									pWorld.setBlock(var7, var8, var9, Block.plantRed.blockID);
								}
							}
						}
					}

					return true;
				}
			}
			*/
		}
		else if (pWorld->getBlockMeta(pos) == 7)
		{
			return false;
		}
		else
		{
			if (!pWorld->m_isClient)
			{
				// todo.
				// ((BlockStem)Block.blocksList[blockID]).fertilizeStem(pWorld, x, y, z);
				--pStack->stackSize;
			}

			return true;
		}
	}
	else
	{
		if (!pWorld->m_isClient)
		{
			if (pWorld->m_Rand.nextFloat() < 0.4f)
			{
				// ((BlockMushroom)Block.blocksList[blockID]).fertilizeMushroom(pWorld, x, y, z, pWorld.rand);
			}

			--pStack->stackSize;
		}

		return true;
	}
	return false; // silence warning.
}

void ItemDye::spawnBonemealParticles(World* pWorld, const BlockPos& pos, int amount)
{
	int blockID = pWorld->getBlockId(pos);

	if (amount == 0)
	{
		amount = 15;
	}

	Block* pBlock = BlockManager::sBlocks[blockID];

	if (pBlock)
	{
		pBlock->setBlockBoundsBasedOnState(pWorld, pos);

		for (int i = 0; i < amount; ++i)
		{
			Vector3 emitVel = itemRand->nextVector() * 0.02f;
			Vector3 emitPos;
			emitPos.x = pos.x + itemRand->nextFloat();
			emitPos.y = pos.y + itemRand->nextFloat() * pBlock->getBlockBoundsMaxY();
			emitPos.z = pos.z + itemRand->nextFloat();
			pWorld->spawnParticle("happyVillager", emitPos, emitVel);
		}
	}
}

bool ItemDye::interactWithEntity(ItemStack& pStack, EntityPlayer& pPlayer, EntityLivingBase* pLiving)
{
	// todo.
	/*if (pLiving instanceof EntitySheep)
	{
		EntitySheep var4 = (EntitySheep)pLiving;
		int var5 = BlockColored.getBlockFromDye(pStack.getItemDamage());

		if (!var4.getSheared() && var4.getFleeceColor() != var5)
		{
			var4.setFleeceColor(var5);
			--pStack.stackSize;
		}

		return true;
	}
	else
	{
		return false;
	}*/
	return false;
}

ItemMapBase::ItemMapBase(int id, const String& name)
	: Item(id, name)
{
}

ItemEmptyMap::ItemEmptyMap(int id, const String& name)
	: ItemMapBase(id, name)
{
}

ItemMap::ItemMap(int id, const String& name)
	: ItemMapBase(id, name)
{
	setHasSubtypes(true);
}

ItemBlock::ItemBlock(int id, const String& name)
	: Item(id, name)
{
	int itemId = id + 256;
	LordAssert(Item::isBlockItem(itemId));
	if (Item::isBasicBlockItem(itemId))
	{
		blockID = itemId;
	}
	else
	{
		blockID = itemId - Item::EC_BLOCK_TO_ITEM_OFFSET;
	}
}

bool ItemBlock::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	int placeID = pWorld->getBlockId(pos);

	BlockPos pos1 = pos;
	if (placeID == BLOCK_ID_SNOW && (pWorld->getBlockMeta(pos) & 7) < 1)
	{
		face = BM_FACE_UP;
	}
	else if (placeID != BLOCK_ID_VINE && placeID != BLOCK_ID_TALL_GRASS && placeID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos1.y; break;
		case BM_FACE_UP:	++pos1.y; break;
		case BM_FACE_NORTH: --pos1.z; break;
		case BM_FACE_SOUTH: ++pos1.z; break;
		case BM_FACE_WEST:	--pos1.x; break;
		case BM_FACE_EAST:	++pos1.x; break;
		default:
			break;
		}
	}

	if (pStack->stackSize == 0)
	{
		return false;
	}
	else if (!pPlayer->canPlayerEdit(pos1, face, pStack))
	{
		return false;
	}
	else if (pos1.y == 255 && BlockManager::sBlocks[blockID]->getMaterial().isSolid())
	{
		return false;
	}
	else if (pWorld->canPlaceEntityOnSide(blockID, pos1, false, face, pPlayer, pStack))
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		int meta = getMetadata(pStack->getItemDamage());
		int meta2 = pBlock->onBlockPlaced(pWorld, pos1, face, hit, meta);

		if (pWorld->setBlock(pos1, blockID, meta2, 3))
		{
			if (pWorld->getBlockId(pos1) == blockID)
			{
				pBlock->onBlockPlacedBy(pWorld, pos1, pPlayer, pStack);
				pBlock->onPostBlockPlaced(pWorld, pos1, meta2);
			}

			pWorld->playSoundOfPlaceBlock(pos1, blockID);
			--pStack->stackSize;
		}

		return true;
	}

	return false;
}


void ItemBlock::caculatePlacePos(World* pWorld,  BlockPos& pos, int face)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID == BLOCK_ID_SNOW)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos.y; break;
		case BM_FACE_UP:	++pos.y; break;
		case BM_FACE_NORTH: --pos.z; break;
		case BM_FACE_SOUTH: ++pos.z; break;
		case BM_FACE_WEST:	--pos.x; break;
		case BM_FACE_EAST:	++pos.x; break;
		default: break;
		}
	}
}

bool ItemBlock::canPlaceItemBlockOnSide(World* pWorld, const BlockPos& pos, int face, EntityPlayer* pPlayer, ItemStackPtr pStack)
{
	int blockID = pWorld->getBlockId(pos);

	BlockPos pos1 = pos;
	if (blockID == BLOCK_ID_SNOW)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos1.y; break;
		case BM_FACE_UP:	++pos1.y; break;
		case BM_FACE_NORTH: --pos1.z; break;
		case BM_FACE_SOUTH: ++pos1.z; break;
		case BM_FACE_WEST:	--pos1.x; break;
		case BM_FACE_EAST:	++pos1.x; break;
		default: break;
		}

	}

	return pWorld->canPlaceEntityOnSide(getBlockID(), pos1, false, face, NULL, pStack);
}

String ItemBlock::getUnlocalizedName(ItemStackPtr pStack)
{
	return BlockManager::sBlocks[blockID]->getUnlocalizedName();
}

String ItemBlock::getUnlocalizedName()
{
	return BlockManager::sBlocks[blockID]->getUnlocalizedName();
}

ItemShears::ItemShears(int id, const String& name)
: Item(id, name)
{
	setMaxStackSize(1);
	setMaxDamage(238);
}

bool ItemShears::onBlockDestroyed(ItemStackPtr pStack, World* porld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving)
{
	if (blockID != BLOCK_ID_LEAVES &&
		blockID != BLOCK_ID_WEB &&
		blockID != BLOCK_ID_TALL_GRASS &&
		blockID != BLOCK_ID_VINE &&
		blockID != BLOCK_ID_TRIP_WIRE)
	{
		return Item::onBlockDestroyed(pStack, porld, blockID, pos, pLiving);
	}

	pStack->damageItem(1, pLiving);
	return true;
}

bool ItemShears::canHarvestBlock(Block* pBlock)
{
	LordAssert(pBlock);
	int blockID = pBlock->getBlockID();

	return 
		blockID == BLOCK_ID_WEB || 
		blockID == BLOCK_ID_REDSTONE_WIRE ||
		blockID == BLOCK_ID_TRIP_WIRE;
}

float ItemShears::getStrVsBlock(ItemStackPtr pStack, Block* pBlock)
{
	LordAssert(pStack);
	LordAssert(pBlock);
	int blockID = pBlock->getBlockID();

	return
		blockID != BLOCK_ID_WEB &&
		blockID != BLOCK_ID_LEAVES ? (blockID == BLOCK_ID_CLOTH ? 5.0F : Item::getStrVsBlock(pStack, pBlock)) : 15.0F;
}

ItemBucket::ItemBucket(int id, const String& name, int fillID)
: Item(id, name)
, isFull(fillID)
{
	maxStackSize = 1;
}

ItemStackPtr ItemBucket::doIt(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, const Vector3i& posIn, int sideIndex)
{
	Vector3i pos = posIn;
	if (!pWorld->canMineBlock(pPlayer, pos))
	{
		return pStack;
	}

	bool isEmpty = isFull == 0;
	if (isEmpty)
	{
		if (!pPlayer->canPlayerEdit(pos, sideIndex, pStack))
		{
			return pStack;
		}

		if (pWorld->getBlockMaterial(pos) == BM_Material::BM_MAT_water &&
			pWorld->getBlockMeta(pos) == 0)
		{
			pWorld->setBlockToAir(pos);

			if (pPlayer->capabilities.isCreativeMode)
			{
				return pStack;
			}

			if (--pStack->stackSize <= 0)
			{
				return LORD::make_shared<ItemStack>(Item::bucketWater);
			}

			if (!pPlayer->inventory->addItemStackToInventory(LORD::make_shared<ItemStack>(Item::bucketWater)))
			{
				pPlayer->dropPlayerItem(LORD::make_shared<ItemStack>(Item::bucketWater->itemID, 1, 0));
			}

			return pStack;
		}

		if (pWorld->getBlockMaterial(pos) == BM_Material::BM_MAT_lava &&
			pWorld->getBlockMeta(pos) == 0)
		{
			pWorld->setBlockToAir(pos);

			if (pPlayer->capabilities.isCreativeMode)
			{
				return pStack;
			}

			if (--pStack->stackSize <= 0)
			{
				return LORD::make_shared<ItemStack>(Item::bucketLava);
			}

			if (!pPlayer->inventory->addItemStackToInventory(LORD::make_shared<ItemStack>(Item::bucketLava)))
			{
				pPlayer->dropPlayerItem(LORD::make_shared<ItemStack>(Item::bucketLava->itemID, 1, 0));
			}

			return pStack;
		}
	}
	else
	{
		if (isFull < 0)
		{
			return LORD::make_shared<ItemStack>(Item::bucketEmpty);
		}

		if (sideIndex == Facing::DOWN->getIndex())
			--pos.y;
		if (sideIndex == Facing::UP->getIndex())
			++pos.y;
		if (sideIndex == Facing::NORTH->getIndex())
			--pos.z;
		if (sideIndex == Facing::SOUTH->getIndex())
			++pos.z;
		if (sideIndex == Facing::WEST->getIndex())
			--pos.x;
		if (sideIndex == Facing::EAST->getIndex())
			++pos.x;

		if (!pPlayer->canPlayerEdit(pos, sideIndex, pStack))
		{
			return pStack;
		}

		if (tryPlaceContainedLiquid(pWorld, pos) && !pPlayer->capabilities.isCreativeMode)
		{
			return LORD::make_shared<ItemStack>(Item::bucketEmpty);
		}
	}

	return pStack;
}

ItemStackPtr ItemBucket::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pWorld->m_isClient) {
		return pStack;
	}

	bool isEmpty = isFull == 0;
	RayTraceResult rayTrace = getMovingObjectPositionFromPlayer(pWorld, pPlayer, isEmpty);

	if (!rayTrace.result) {
		return pStack;
	}
	
	if (rayTrace.typeOfHit != RAYTRACE_TYPE_BLOCK) {
		return pStack;
	}

	BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()
		->sendUseItem(NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_BUCKET, rayTrace.blockPos, rayTrace.sideHit->getIndex());

	return doIt(pStack, pWorld, pPlayer, rayTrace.blockPos, rayTrace.sideHit->getIndex());
	
}

bool ItemBucket::tryPlaceContainedLiquid(World* pWorld, const BlockPos& pos)
{
	if (isFull <= 0)
		return false;

	const BM_Material& mat = pWorld->getBlockMaterial(pos);
	bool notSolid = !mat.isSolid();
	bool replaceable = mat.isReplaceable();
	if (!pWorld->isAirBlock(pos) && !notSolid && !replaceable)
	{
		return false;
	}
	else
	{
		// todo.
		if (/*pWorld->m_provider->isHellWorld*/ false && isFull == BLOCK_ID_WATERMOVING)
		{
			//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.fizz", 0.5F, 2.6F + (pWorld->m_Rand.nextFloat() - pWorld->m_Rand.nextFloat()) * 0.8F);
			pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Fizz);
			for (int i = 0; i < 8; ++i)
			{
				pWorld->spawnParticle("largesmoke", Vector3(pos) + pWorld->m_Rand.nextVector(), Vector3::ZERO);
			}
		}
		else
		{
			if (!pWorld->m_isClient && (notSolid || replaceable) && !mat.isLiquid())
			{
				pWorld->destroyBlock(pos, true);
			}

			pWorld->setBlock(pos, isFull, 0, 3);
		}

		return true;
	}
}

ItemBucketMilk::ItemBucketMilk(int id, const String& name)
: Item(id, name)
{
	setMaxStackSize(1);
}

ItemStackPtr ItemBucketMilk::onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pPlayer->capabilities.isCreativeMode)
	{
		--pStack->stackSize;
	}

	if (!pWorld->m_isClient)
	{
		pPlayer->clearActivePotions();
	}

	return pStack->stackSize <= 0 ? LORD::make_shared<ItemStack>(Item::bucketEmpty) : pStack;
}

ItemStackPtr ItemBucketMilk::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	return pStack;
}

IBehaviorDispenseItem* ItemBlockman::dispenserMinecartBehavior = NULL;

ItemBlockman::ItemBlockman(int id, const String& name, int type)
: Item(id, name)
, minecartType(type)
{
	maxStackSize = 1;
	// todo.
	// BlockDispenser::dispenseBehaviorRegistry.putObject(this, dispenserMinecartBehavior);
}

void ItemBlockman::initialize()
{
	// todo.
	// dispenserMinecartBehavior = LordNew BehaviorDispenseMinecart();
}

void ItemBlockman::unInitialize()
{
	// todo.
	// LordSafeDelete(dispenserMinecartBehavior);
}

bool ItemBlockman::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	int blockID = pWorld->getBlockId(pos);

	if (BlockRailBase::isRailBlock(blockID))
	{
		if (!pWorld->m_isClient)
		{
			EntityBlockmanEmpty* entity = LordNew EntityBlockmanEmpty(pWorld, pos, pPlayer->rotationYaw, true);
			if (pStack->hasDisplayName())
			{
				entity->setBlocmanName(pStack->getDisplayName());
			}
			pWorld->spawnEntityInWorld(entity);
		}

		--pStack->stackSize;
		return true;
	}
	return false;
}

ItemSaddle::ItemSaddle(int id, const String& name)
: Item(id, name)
{
	maxStackSize = 1;
}

bool ItemSaddle::hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2)
{
	if (pLiving2->isClass(ENTITY_CLASS_PID))
	{
		// todo.
		/*EntityPig var4 = (EntityPig)par3EntityLivingBase;

		if (!var4.getSaddled() && !var4.isChild())
		{
			var4.setSaddled(true);
			--par1ItemStack.stackSize;
		}*/

		return true;
	}
	return true;
}

ItemRedstone::ItemRedstone(int id, const String& name)
: Item(id, name)
{}

bool ItemRedstone::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	BlockPos pos1 = pos;
	if (pWorld->getBlockId(pos) != BLOCK_ID_SNOW)
	{
		if (face == BM_FACE_DOWN)
			--pos1.y;
		if (face == BM_FACE_UP)
			++pos1.y;
		if (face == BM_FACE_NORTH)
			--pos1.z;
		if (face == BM_FACE_SOUTH)
			++pos1.z;
		if (face == BM_FACE_WEST)
			--pos1.x;
		if (face == BM_FACE_EAST)
			++pos1.x;

		if (!pWorld->isAirBlock(pos))
		{
			return false;
		}
	}

	if (!pPlayer->canPlayerEdit(pos1, face, pStack))
	{
		return false;
	}
	else
	{
		if (BlockManager::redstoneWire->canPlaceBlockAt(pWorld, pos1))
		{
			--pStack->stackSize;
			pWorld->setBlock(pos1, BLOCK_ID_REDSTONE_WIRE);
		}

		return true;
	}
}

ItemSnowball::ItemSnowball(int id, const String& name)
: Item(id, name)
{
	maxStackSize = 16;
}

ItemStackPtr ItemSnowball::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	//LordLogInfo(" ItemSnowball::onItemRightClick ==================");
	//if (!pPlayer->capabilities.isCreativeMode)
	//{
	//	--pStack->stackSize;
	//	pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	//	
	//}

	//pWorld->playSoundAtEntity(pPlayer, "random.bow", 0.5F, 0.4F / (itemRand->nextFloat() * 0.4F + 0.8F));
	pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);

	if (!pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		if (pPlayer->inventory->consumeInventoryItem(pStack->getItem()->itemID)) {
			EntitySnowball* entitysnowball = LordNew EntitySnowball(pWorld, pPlayer);
			entitysnowball->setMeta(pStack->subItemID);
			entitysnowball->position.y += pPlayer->getEyeHeight();
			entitysnowball->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0, 2.0, 0.0);
			pWorld->spawnEntityInWorld(entitysnowball);
		}
	}
	else
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->swingItem();
		if (pPlayer->inventory->consumeInventoryItem(pStack->getItem()->itemID)) {
			GameCommon::Instance()->getCommonPacketSender()->sendUseThrowableItem();
		}
		else {
			LordLogError("throw snowball fail, no snowball in inventory");
		}
		pPlayer->clearItemInUse();
	}
	return pStack;
}

ItemTnt::ItemTnt(int id, const String& name)
	: ItemBlock(id, name)
{
	maxStackSize = 64;
}

ItemStackPtr ItemTnt::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{

	if (!pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->inventory->consumeInventoryItem(pStack->getItem()->itemID);
		EntityTNTThrowable* entityTNTThrowable = LordNew EntityTNTThrowable(pWorld, pPlayer);
		entityTNTThrowable->position.y += pPlayer->getEyeHeight();
		entityTNTThrowable->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0, 2.0, 0.0);
		pWorld->spawnEntityInWorld(entityTNTThrowable);
	}
	else
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->swingItem();
		GameCommon::Instance()->getCommonPacketSender()->sendUseThrowableItem();
	}
	return pStack;
}

ItemGrenade::ItemGrenade(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 5;
}

ItemStackPtr ItemGrenade::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->inventory->consumeInventoryItem(pStack->getItem()->itemID);
		EntityGrenade* entityGrenade = LordNew EntityGrenade(pWorld, pPlayer, pStack->getItem()->itemID);
		entityGrenade->position.y += pPlayer->getEyeHeight();
		entityGrenade->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0, 2.0, 0.0);
		pWorld->spawnEntityInWorld(entityGrenade);
	}
	else
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->swingItem();
		GameCommon::Instance()->getCommonPacketSender()->sendUseThrowableItem();
	}
	return pStack;
}

ItemBoat::ItemBoat(int id, const String& name)
: Item(id, name)
{
	maxStackSize = 1;
}

ItemStackPtr ItemBoat::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	float rdt = 1.0F;
	float pitch = pPlayer->prevRotationPitch + (pPlayer->rotationPitch - pPlayer->prevRotationPitch) * rdt;
	float yaw = pPlayer->prevRotationYaw + (pPlayer->rotationYaw - pPlayer->prevRotationYaw) * rdt;
	Vector3 start = pPlayer->position + (pPlayer->position - pPlayer->prevPos) * rdt;
	start.y += 1.62f - pPlayer->yOffset;

	float fx = Math::Sin(-yaw * Math::DEG2RAD - Math::PI);
	float fz = Math::Cos(-yaw * Math::DEG2RAD - Math::PI);
	float fxz = -Math::Cos(-pitch * Math::DEG2RAD);
	float fy = Math::Sin(-pitch * Math::DEG2RAD);
	fx = fx * fxz;
	fz = fz * fxz;
	float len = 5.0f;
	Vector3 end = start + Vector3(fx * len, fy * len, fz * len);
	RayTraceResult rayTrace = pWorld->rayTraceBlocks(start, end, true);

	if (!rayTrace.result)
	{
		return pStack;
	}
	else
	{
		Vector3 lookat = pPlayer->getLook(rdt);
		bool flag = false;
		EntityArr entites = pWorld->getEntitiesWithinAABBExcludingEntity(pPlayer, pPlayer->boundingBox.addCoord(lookat * len).expand(1.f));

		for (auto it : entites)
		{
			Entity* pEntity = it;

			if (pEntity->canBePushed())
			{
				float border = pEntity->getCollisionBorderSize();
				Box aabb = pEntity->boundingBox.expand(border);

				if (aabb.isPointInside(start))
				{
					flag = true;
				}
			}
		}

		if (flag)
		{
			return pStack;
		}
		else
		{
			if (rayTrace.typeOfHit == RAYTRACE_TYPE_BLOCK)
			{
				BlockPos pos = rayTrace.blockPos;

				if (pWorld->getBlockId(pos) == BLOCK_ID_SNOW)
				{
					--pos.y;
				}

				// todo.
				/*
				EntityBoat var35 = new EntityBoat(par2World, (double)((float)var29 + 0.5F), (double)((float)var33 + 1.0F), (double)((float)var34 + 0.5F));
				var35.rotationYaw = (float)(((MathHelper.floor_double((double)(pPlayer->rotationYaw * 4.0F / 360.0F) + 0.5D) & 3) - 1) * 90);

				if (!par2World.getCollidingBoundingBoxes(var35, var35.boundingBox.expand(-0.1D, -0.1D, -0.1D)).isEmpty())
				{
				return par1ItemStack;
				}

				if (!par2World.isRemote)
				{
				par2World.spawnEntityInWorld(var35);
				}

				if (!pPlayer->capabilities.isCreativeMode)
				{
				--par1ItemStack.stackSize;
				}*/
			}

			return pStack;
		}
	}
}

ItemBook::ItemBook(int id, const String& name)
: Item(id, name)
{}

bool ItemBook::isItemTool(ItemStackPtr pStack)
{
	return pStack->stackSize == 1; 
}

ItemEgg::ItemEgg(int id, const String& name)
: Item(id, name)
{
	maxStackSize = 16;
}

ItemStackPtr ItemEgg::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pPlayer->capabilities.isCreativeMode)
	{
		--pStack->stackSize;
	}

	//pWorld->playSoundAtEntity(pPlayer, "random.bow", 0.5F, 0.4F / (itemRand->nextFloat() * 0.4F + 0.8F));
	pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);

	if (!pWorld->m_isClient)
	{
		// todo.
		// pWorld->spawnEntityInWorld(new EntityEgg(par2World, par3EntityPlayer));
	}

	return pStack;
}

ItemFishingRod::ItemFishingRod(int id, const String& name)
: Item(id, name)
{
	setMaxDamage(64);
	setMaxStackSize(1);
}

ItemStackPtr ItemFishingRod::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
 	if (pPlayer->m_fishEntity)
 	{
		if (pWorld->m_isClient)
		{
			pPlayer->swingItem();
			GameCommon::Instance()->getCommonPacketSender()->sendUseFishingRod();
		}
		else
		{
			pPlayer->m_fishEntity->catchFish();
 			pStack->damageItem(1, pPlayer);
		}

		pPlayer->m_fishEntity->setDead();
		pPlayer->m_fishEntity = nullptr;
 	}
 	else
	{
		pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);

		if (!pWorld->m_isClient)
		{
			EntityFishHook* pFishHook = LordNew EntityFishHook(pWorld, pPlayer);
			pFishHook->position.y += pPlayer->getEyeHeight();
			pFishHook->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0f, 2.0f, 0.0f);
			pPlayer->m_fishEntity = pFishHook;
			pWorld->spawnEntityInWorld(pFishHook);
		}
		else
		{
			pPlayer->swingItem();
			GameCommon::Instance()->getCommonPacketSender()->sendUseFishingRod();
		}
	}

	return pStack;
}

ItemBed::ItemBed(int id, const String& name)
: Item(id, name)
{}

bool ItemBed::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;
	if (face != 1)
		return false;

	BlockPos pos1 = pos.getPosY();
	BlockBed* pBlockBed = dynamic_cast<BlockBed*>(BlockManager::bed);
	int dir = int(Math::Floor((pPlayer->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;
	BlockPos offset = BlockPos::ZERO;

	if (dir == 0)
		offset.z = 1;
	if (dir == 1)
		offset.x = -1;
	if (dir == 2)
		offset.z = -1;
	if (dir == 3)
		offset.x = 1;

	if (pPlayer->canPlayerEdit(pos1, face, pStack) && pPlayer->canPlayerEdit(pos1 + offset, face, pStack))
	{
		if (pWorld->isAirBlock(pos1) && 
			pWorld->isAirBlock(pos1+offset) && 
			pWorld->doesBlockHaveSolidTopSurface(pos1.getNegY()) &&
			pWorld->doesBlockHaveSolidTopSurface(pos1.getNegY() + offset))
		{
			pWorld->setBlock(pos1, BLOCK_ID_BED, dir, 3);

			if (pWorld->getBlockId(pos1) == BLOCK_ID_BED)
			{
				pWorld->setBlock(pos1 + offset, BLOCK_ID_BED, dir + 8, 3);
			}

			--pStack->stackSize;
			return true;
		}
		return false;
	}
	return false;
}

ItemExpBottle::ItemExpBottle(int id, const String& name)
: Item(id, name)
{}

ItemStackPtr ItemExpBottle::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (!pPlayer->capabilities.isCreativeMode)
	{
		--pStack->stackSize;
	}

	//pWorld->playSoundAtEntity(pPlayer, "random.bow", 0.5F, 0.4F / (itemRand->nextFloat() * 0.4F + 0.8F));
	pWorld->playSoundAtEntityByType(pPlayer, ST_PullTheBow);

	if (!pWorld->m_isClient)
	{
		// todo.
		// pWorld->spawnEntityInWorld(LordNew EntityExpBottle(pWorld, pPlayer));
	}

	return pStack;
}

ItemFireball::ItemFireball(int id, const String& name)
: Item(id, name)
{}

ItemStackPtr ItemFireball::onItemRightClick(ItemStackPtr pStack, World * pWorld, EntityPlayer * pPlayer)
{
	if (!pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		EntityFireball* entityFireball = LordNew EntityFireball(pWorld, pPlayer);
		entityFireball->position.y += pPlayer->getEyeHeight();
		entityFireball->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0, 2.0, 0.0);
		pWorld->spawnEntityInWorld(entityFireball);
	}
	else
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
		pPlayer->swingItem();
		GameCommon::Instance()->getCommonPacketSender()->sendUseThrowableItem();
	}
	return pStack;
}

bool ItemFireball::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	BlockPos pos1 = pos + offsetForSide[face];

	if (!pPlayer->canPlayerEdit(pos1, face, pStack))
		return false;

	int blockID = pWorld->getBlockId(pos1);

	if (blockID == 0)
	{
		//pWorld->playSoundEffect(Vector3(pos1) + 0.5f, "fire.ignite", 1.0F, itemRand->nextFloat() * 0.4F + 0.8F);
		pWorld->playSoundEffectByType(Vector3(pos1) + 0.5f, ST_IgniteFire);
		pWorld->setBlock(pos1, BLOCK_ID_FIRE);
	}

	if (!pPlayer->capabilities.isCreativeMode)
	{
		--pStack->stackSize;
	}

	return true;
}

ItemWritableBook::ItemWritableBook(int id, const String& name)
: Item(id, name)
{
	setMaxStackSize(1);
}

ItemStackPtr ItemWritableBook::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	// todo. in client
	// pPlayer->displayGUIBook(pStack);
	return pStack;
}

bool ItemWritableBook::validBookTagPages(NBTTagCompound* pNbtCompound)
{
	if (!pNbtCompound)
	{
		return false;
	}
	if (!pNbtCompound->hasKey("pages"))
	{
		return false;
	}

	NBTTagList* pNbtPages = (NBTTagList*)pNbtCompound->getTag("pages");

	for (int i = 0; i < pNbtPages->tagCount(); ++i)
	{
		NBTTagString* pNbtStr = (NBTTagString*)pNbtPages->tagAt(i);

		if (pNbtStr->data == StringUtil::BLANK)
		{
			return false;
		}

		if (pNbtStr->data.length() > 256)
		{
			return false;
		}
	}

	return true;
}

ItemEditableBook::ItemEditableBook(int id, const String& name)
: Item(id, name)
{
	setMaxStackSize(1);
}

String ItemEditableBook::getItemDisplayName(ItemStackPtr pStack)
{
	if (pStack->hasTagCompound())
	{
		NBTTagCompound* pNbt = pStack->getTagCompound();
		NBTTagString* pNbtTitle = (NBTTagString*)pNbt->getTag("title");

		if (pNbtTitle)
		{
			return pNbtTitle->toString();
		}
	}

	return Item::getItemDisplayName(pStack);
}

ItemStackPtr ItemEditableBook::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	// par3EntityPlayer.displayGUIBook(par1ItemStack);
	return pStack;
}

bool ItemEditableBook::validBookTagContents(NBTTagCompound* pNbtCompound)
{
	if (!ItemWritableBook::validBookTagPages(pNbtCompound))
		return false;
	if (!pNbtCompound->hasKey("title"))
		return false;

	String strTitle = pNbtCompound->getString("title");
	return strTitle.length() <= 16 ? pNbtCompound->hasKey("author") : false;
}


String ItemSkull::skullTypes[SKULL_TYPE_COUNT] = {"skeleton", "wither", "zombie", "char", "creeper"};

ItemSkull::ItemSkull(int id, const String& name)
: Item(id, name)
{
	setMaxDamage(0);
	setHasSubtypes(true);
}

bool ItemSkull::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (face != BM_FACE_UP)
		return false;
	
	if (!pWorld->getBlockMaterial(pos).isSolid())
		return false;
	
	BlockPos pos1 = pos + offsetForSide[face];

	--pStack->stackSize;

	BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()
		->sendSpawnVehicle(10000, Vector3(pos1), 0);
	return true;
}

String ItemSkull::getUnlocalizedName(ItemStackPtr pStack)
{
	int meta = pStack->getItemDamage();

	if (meta < 0 || meta >= SKULL_TYPE_COUNT)
	{
		meta = 0;
	}

	return Item::getUnlocalizedName() + "." + skullTypes[meta];
}

String ItemSkull::getItemDisplayName(ItemStackPtr pStack)
{
	// todo.
	/*return pStack->getItemDamage() == 3 && pStack->hasTagCompound() && pStack->getTagCompound().hasKey("SkullOwner") ? 
		StatCollector.translateToLocalFormatted("item.skull.player.name", new Object[] {par1ItemStack.getTagCompound().getString("SkullOwner")}) : 
		Item::getItemDisplayName(pStack);*/
	return Item::getItemDisplayName(pStack);
}

ItemCarrotOnAStick::ItemCarrotOnAStick(int id, const String& name)
: Item(id, name)
{
	setMaxStackSize(1);
	setMaxDamage(25);
}

ItemStackPtr ItemCarrotOnAStick::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (pPlayer->isRiding() && pPlayer->ridingEntity->isClass(ENTITY_CLASS_PID))
	{
		// todo.
		/*EntityPig var4 = (EntityPig)par3EntityPlayer.ridingEntity;

		if (var4.getAIControlledByPlayer().isControlledByPlayer() && par1ItemStack.getMaxDamage() - par1ItemStack.getItemDamage() >= 7)
		{
			var4.getAIControlledByPlayer().boostSpeed();
			par1ItemStack.damageItem(7, par3EntityPlayer);

			if (par1ItemStack.stackSize == 0)
			{
				ItemStack var5 = LORD::make_shared<ItemStack>(Item.fishingRod);
				var5.setTagCompound(par1ItemStack.stackTagCompound);
				return var5;
			}
		}*/
	}

	return pStack;
}

ItemSimpleFoiled::ItemSimpleFoiled(int id, const String& name)
: Item(id, name)
{}

ItemFirework::ItemFirework(int id, const String& name)
: Item(id, name)
{}

bool ItemFirework::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (!pWorld->m_isClient)
	{
		// todo.
		/*EntityFireworkRocket var11 = new EntityFireworkRocket(par3World, (double)((float)par4 + par8), (double)((float)par5 + par9), (double)((float)par6 + par10), par1ItemStack);
		par3World.spawnEntityInWorld(var11);

		if (!par2EntityPlayer.capabilities.isCreativeMode)
		{
			--par1ItemStack.stackSize;
		}*/

		return true;
	}
	return false;
}

ItemFireworkCharge::ItemFireworkCharge(int id, const String& name)
: Item(id, name)
{}


ItemLeash::ItemLeash(int id, const String& name)
: Item(id, name)
{}

bool ItemLeash::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID == BLOCK_ID_FENCE)
	{
		if (pWorld->m_isClient)
		{
			return true;
		}
		else
		{
			func_135066_a(pPlayer, pWorld, pos);
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool ItemLeash::func_135066_a(EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos)
{
	// todo.
	/*EntityLeashKnot var5 = EntityLeashKnot.func_110130_b(pWorld, par2, par3, par4);
	boolean var6 = false;
	double var7 = 7.0D;
	List var9 = pWorld.getEntitiesWithinAABB(EntityLiving.class, AxisAlignedBB.getAABBPool().getAABB((double)par2 - var7, (double)par3 - var7, (double)par4 - var7, (double)par2 + var7, (double)par3 + var7, (double)par4 + var7));

	if (var9 != null)
	{
		Iterator var10 = var9.iterator();

		while (var10.hasNext())
		{
			EntityLiving var11 = (EntityLiving)var10.next();

			if (var11.func_110167_bD() && var11.func_110166_bE() == pPlayer)
			{
				if (var5 == null)
				{
					var5 = EntityLeashKnot.func_110129_a(pWorld, par2, par3, par4);
				}

				var11.func_110162_b(var5, true);
				var6 = true;
			}
		}
	}

	return var6;*/
	return false;
}

ItemNameTag::ItemNameTag(int id, const String& name)
: Item(id, name)
{}

bool ItemNameTag::interactWithEntity(ItemStackPtr pStack, EntityPlayer* pPlayer, EntityLivingBase* pLiving)
{
	if (!pStack->hasDisplayName())
		return false;
	
	if (pLiving->isClass(ENTITY_CLASS_ENTITY))
	{
		EntityLiving* pLiving2 = (EntityLiving*)pLiving;
		pLiving2->setCustomNameTag(pStack->getDisplayName());
		pLiving2->enablePersistence();
		--pStack->stackSize;
		return true;
	}

	return Item::interactWithEntity(pStack, pPlayer, pLiving);
}

ItemLeaves::ItemLeaves(int id, const String& name)
	: ItemBlock(id, name)
{
}

String ItemLeaves::getUnlocalizedName(ItemStackPtr pStack)
{
	int damage = pStack->getItemDamage();

	// todo.
	/*if (damage < 0 || damage >= BlockLeaves.LEAF_TYPES.length)
	{
		damage = 0;
	}*/

	return ItemBlock::getUnlocalizedName() + "."; //+ BlockLeaves.LEAF_TYPES[damage];
}



ItemSlab::ItemSlab(int id, const String& name, Block* pHalfSlab, Block* pDoubleSlab, bool isFullBlock)
	: ItemBlock(id, name)
	, m_pHalfSlab(pHalfSlab)
	, m_pDoubleSlab(pDoubleSlab)
	, m_isFullBlock(isFullBlock)
{
	setMaxDamage(0);
	setHasSubtypes(true);
}

String ItemSlab::getUnlocalizedName(ItemStackPtr pStack)
{
	// todo.
	//return this.theHalfSlab.getFullSlabName(par1ItemStack.getItemDamage());
	return ItemBlock::getUnlocalizedName();
}

bool ItemSlab::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (m_isFullBlock)
	{
		return ItemBlock::onItemUse(pStack, pPlayer, pWorld, pos, face, hit);
	}
	else if (pStack->stackSize == 0)
	{
		return false;
	}
	else if (!pPlayer->canPlayerEdit(pos, face, pStack))
	{
		return false;
	}
	else
	{
		int blockID = pWorld->getBlockId(pos);
		int meta = pWorld->getBlockMeta(pos);
		int damage = meta & 7;
		bool var14 = (meta & 8) != 0;

		if ((face == 1 && !var14 || face == 0 && var14) && blockID == m_pHalfSlab->getBlockID() && damage == pStack->getItemDamage())
		{
			if (pWorld->checkNoEntityCollision(m_pDoubleSlab->getCollisionBoundingBox(pWorld, pos)) && 
				pWorld->setBlock(pos, m_pDoubleSlab->getBlockID(), damage, 3))
			{
				pWorld->playSoundOfPlaceBlock(pos, m_pDoubleSlab->getBlockID());
				--pStack->stackSize;
			}

			return true;
		}
		else
		{
			return func_77888_a(pStack, pPlayer, pWorld, pos, face) ? true : ItemBlock::onItemUse(pStack, pPlayer, pWorld, pos, face, hit);
		}
	}
}

bool ItemSlab::canPlaceItemBlockOnSide(World* pWorld, const BlockPos& pos, int face, EntityPlayer* pPlayer, ItemStackPtr pStack)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	int blockID = pWorld->getBlockId(pos);
	int meta = pWorld->getBlockMeta(pos);
	int damage = meta & 7;
	bool var14 = (meta & 8) != 0;

	if ((face == 1 && !var14 || face == 0 && var14) && blockID == m_pHalfSlab->getBlockID() && damage == pStack->getItemDamage())
	{
		return true;
	}
	else
	{
		if (face == 0)
		{
			--y;
		}

		if (face == 1)
		{
			++y;
		}

		if (face == 2)
		{
			--z;
		}

		if (face == 3)
		{
			++z;
		}

		if (face == 4)
		{
			--x;
		}

		if (face == 5)
		{
			++x;
		}

		blockID = pWorld->getBlockId(BlockPos(x, y, z));
		meta = pWorld->getBlockMeta(BlockPos(x, y, z));
		damage = meta & 7;
		var14 = (meta & 8) != 0;
		return blockID == m_pHalfSlab->getBlockID() && damage == pStack->getItemDamage() ? true : ItemBlock::canPlaceItemBlockOnSide(pWorld, pos, face, pPlayer, pStack);
	}
}

vector<Vector3i>::type ItemSlab::getPlacePositions(World * pWorld, BlockPos & pos, int face)
{
	vector<Vector3i>::type positions = vector<Vector3i>::type();
	BlockPos pos2 = Vector3i(pos.x, pos.y, pos.z);
	int blockID = pWorld->getBlockId(pos);
	if (blockID == BLOCK_ID_SNOW)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos.y; break;
		case BM_FACE_UP:	++pos.y; break;
		case BM_FACE_NORTH: --pos.z; break;
		case BM_FACE_SOUTH: ++pos.z; break;
		case BM_FACE_WEST:	--pos.x; break;
		case BM_FACE_EAST:	++pos.x; break;
		default: break;
		}
	}
	positions.push_back(pos);
	positions.push_back(pos2);
	return positions;
}


bool ItemSlab::func_77888_a(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	if (face == 0)
	{
		--y;
	}

	if (face == 1)
	{
		++y;
	}

	if (face == 2)
	{
		--z;
	}

	if (face == 3)
	{
		++z;
	}

	if (face == 4)
	{
		--x;
	}

	if (face == 5)
	{
		++x;
	}

	BlockPos tempPos(x, y, z);	
	int blockID = pWorld->getBlockId(tempPos);
	int meta = pWorld->getBlockMeta(tempPos);
	int damage = meta & 7;

	if (blockID == m_pHalfSlab->getBlockID() && damage == pStack->getItemDamage())
	{
		if (pWorld->checkNoEntityCollision(m_pDoubleSlab->getCollisionBoundingBox(pWorld, tempPos)) && 
			pWorld->setBlock(tempPos, m_pDoubleSlab->getBlockID(), damage, 3))
		{
			pWorld->playSoundOfPlaceBlock(tempPos, m_pDoubleSlab->getBlockID());
			--pStack->stackSize;
		}

		return true;
	}
	else
	{
		return false;
	}
}



ItemMultiTextureTile::ItemMultiTextureTile(int id, const String& name, const StringArray& names)
	: ItemBlock(id, name)
{
	setMaxDamage(0);
	setHasSubtypes(true);
	m_blockNames = names;
}

String ItemMultiTextureTile::getUnlocalizedName(ItemStackPtr pStack)
{	
	int damage = pStack->getItemDamage();
	if (damage < 0 || damage >= int(m_blockNames.size()))
	{
		damage = 0;
	}
	return ItemBlock::getUnlocalizedName() + "." + m_blockNames[damage];	
}

ItemMultiTextureTile* ItemMultiTextureTile::setBlockNames(String names[], int len)
{	
	for (int i = 0; i < len; ++i)
	{
		m_blockNames.push_back(names[i]);
	}
	return this;
}

ItemAnvilBlock::ItemAnvilBlock(int id, const String& name)
	:ItemMultiTextureTile(id, name, BlockAnvil::statuses)
{
}

ItemPiston::ItemPiston(int id)
	:ItemBlock(id)
{
}

ItemBlockWithMetadata::ItemBlockWithMetadata(int id)
	: ItemBlock(id)
{
}

ItemSnow::ItemSnow(int id)
	: ItemBlockWithMetadata(id)
{
}

bool ItemSnow::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (pStack->stackSize == 0)
	{
		return false;
	}
	else if (!pPlayer->canPlayerEdit(pos, face, pStack))
	{
		return false;
	}
	else
	{
		int blockID = pWorld->getBlockId(pos);

		if (blockID == BLOCK_ID_SNOW)
		{
			Block* pBlock = BlockManager::sBlocks[getBlockID()];
			int meta = pWorld->getBlockMeta(pos);
			int damage = meta & 7;

			if (damage <= 6 && pWorld->checkNoEntityCollision(pBlock->getCollisionBoundingBox(pWorld, pos)) && 
				pWorld->setBlockMetadataWithNotify(pos, damage + 1 | meta & -8, 2))
			{
				pWorld->playSoundOfPlaceBlock(pos, getBlockID());
				--pStack->stackSize;
				return true;
			}
		}

		return ItemBlock::onItemUse(pStack, pPlayer, pWorld, pos, face, hit);
	}
}

ItemCloth::ItemCloth(int id, const String& name)
	:ItemBlock(id, name)
{
	setMaxDamage(0);
	setHasSubtypes(true);
}

String ItemCloth::getUnlocalizedName(ItemStackPtr pStack)
{
	return ItemBlock::getUnlocalizedName() + "." + ItemDye::dyeColorNames[BlockColored::getBlockFromDye(pStack->getItemDamage())];
}

ItemColored::ItemColored(int id, bool flag)
	:ItemBlock(id)
{
	if (flag)
	{
		setMaxDamage(0);
		setHasSubtypes(true);
	}
}

ItemColored::ItemColored(int id, bool flag, const StringArray& names)
	:ItemBlock(id)
{
	m_blockNames = names;
	if (flag)
	{
		setMaxDamage(0);
		setHasSubtypes(true);
	}
}


String ItemColored::getUnlocalizedName(ItemStackPtr pStack)
{	

	int damage = pStack->getItemDamage();

	if (m_blockNames.empty() || damage < 0 || damage >= int(m_blockNames.size()))
	{
		return ItemBlock::getUnlocalizedName(pStack);
	}
		
	return ItemBlock::getUnlocalizedName(pStack) + "." + m_blockNames[damage];	
}

ItemColored* ItemColored::setBlockNames(String names[], int len)
{
	for (int i = 0; i < len; ++i)
	{
		m_blockNames.push_back(names[i]);
	}
	return this;
}

ItemLilyPad::ItemLilyPad(int id)
	:ItemColored(id, false)
{	
}

ItemStackPtr ItemLilyPad::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	RayTraceResult var4 = getMovingObjectPositionFromPlayer(pWorld, pPlayer, true);
	
	if (!var4.result)
	{
		return pStack;
	}
			
	if (var4.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{			
		int x = var4.getBlockPos().x;
		int y = var4.getBlockPos().y;
		int z = var4.getBlockPos().z;

		if (!pWorld->canMineBlock(pPlayer, BlockPos(x, y, z)))
		{
			return pStack;
		}			
		if (!pPlayer->canPlayerEdit(BlockPos(x, y, z), var4.sideHit->getIndex(), pStack))
		{
			return pStack;
		}

		if (pWorld->getBlockMaterial(BlockPos(x, y, z)) == BM_Material::BM_MAT_water && 
			pWorld->getBlockMeta(BlockPos(x, y, z)) == 0 && 
			pWorld->isAirBlock(BlockPos(x, y + 1, z)))
		{
			pWorld->setBlock(BlockPos(x, y + 1, z), BLOCK_ID_WATERLILY);

			if (!pPlayer->capabilities.isCreativeMode)
			{
				--pStack->stackSize;
			}
		}
	}

	return pStack;
	
}

ItemRecord::RecordMap ItemRecord::records;

ItemRecord::ItemRecord(int id, const String& name, const String& record)
	: Item(id, name)
{
	recordName = record;
	maxStackSize = 1;
	records.insert(std::make_pair(record, this));
}

bool ItemRecord::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	if (pWorld->getBlockId(pos) == BLOCK_ID_JUKE_BOX && pWorld->getBlockMeta(pos) == 0)
	{
		if (pWorld->m_isClient)
			return true;
		((BlockJukeBox*)BlockManager::jukebox)->insertRecord(pWorld, pos, pStack);
		pWorld->playAuxSFXAtEntity(NULL, 1005, pos, itemID);
		--pStack->stackSize;
		return true;
	}
	return false;
}

ItemRecord* ItemRecord::getRecord(const String& name)
{
	auto it = records.find(name);
	if (it == records.end())
		return NULL;
	return it->second;
}

ItemArrowPotion::ItemArrowPotion(int id, const String& name) 
	: Item(id, name) 
{
}

ItemKey::ItemKey(int id, const String & name)
	: Item(id, name)
{
}

ItemTeleportScroll::ItemTeleportScroll(int id, const String & name)
	: Item(id, name)
{
}

ItemStackPtr ItemTeleportScroll::onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	if (pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	}
	else if(pStack->stackSize > 0)
	{
		SCRIPT_EVENT::UseItemTeleportEvent::invoke(pPlayer->hashCode(), itemID);
		--pStack->stackSize;
	}
	return pStack;
}

ItemCreateBridge::ItemCreateBridge(int id, const String & name)
	: Item(id, name)
{
}

ItemStackPtr ItemCreateBridge::onItemRightClick(ItemStackPtr pStack, World * pWorld, EntityPlayer * pPlayer)
{
	if (pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	}
	else if (pStack->stackSize > 0)
	{
		--pStack->stackSize;
	}
	return pStack;
}

ItemToolDigger::ItemToolDigger(int id, const String & name, ToolMaterial * material)
	: ItemTool(id, name, 1.0f, material)
{
}

bool ItemToolDigger::canHarvestBlock(Block * pBlock)
{
	return true;
}

ItemMedichine::ItemMedichine(int id, const String & name, float amount)
	: Item(id, name)
	, healAmount(amount)
{
}

ItemStackPtr ItemMedichine::onItemRightClick(ItemStackPtr pStack, World * pWorld, EntityPlayer * pPlayer)
{
	if (pWorld->m_isClient)
	{
		pPlayer->setItemInUse(pStack, getMaxItemUseDuration(pStack));
	}
	return pStack;
}

ItemStackPtr ItemMedichine::onEaten(ItemStackPtr pItemStack, World * pWorld, EntityPlayer * pPlayer)
{
	--pItemStack->stackSize;
	pPlayer->heal(healAmount);
	pWorld->playSoundAtEntityByType(pPlayer, ST_Burp);
	return pItemStack;
}

ItemProp::ItemProp(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 1;
}

bool ItemProp::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	return false;
}

ItemSkillProp::ItemSkillProp(int id,const String& name)
	: Item(id, name)
{
}

bool ItemSkillProp::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	return false;
}

ItemSkillBook::ItemSkillBook(int id, const String& name)
	: Item(id, name)
{
}

ItemSkillBookFragmentation::ItemSkillBookFragmentation(int id, const String& name)
	: Item(id, name)
{
}

ItemSurpriseTreasureChest::ItemSurpriseTreasureChest(int id, const String& name)
	: Item(id, name)
{

}

ItemBuild::ItemBuild(int id, const String & name)
	: Item(id, name)
{
}

bool ItemBuild::onItemUse(ItemStackPtr pStack, EntityPlayer * pPlayer, World * pWorld, const BlockPos & pos, int face, const Vector3 & hit)
{
	if (!pWorld->m_isClient)
	{
		Vector3  newPos = Vector3((float)pos.x, (float)pos.y + 1.f, (float)pos.z);
		Vector3 startPos = Vector3(newPos.x - getLength() / 2, newPos.y, newPos.z - getWidth() / 2);
		Vector3 endPos = Vector3(newPos.x + getLength() / 2, newPos.y + getHeight(), newPos.z + getWidth() / 2);
		if ((int)getLength() % 2 != 0)
		{
			startPos.x += 0.5f;
			endPos.x += 0.5f;
			newPos.x += 0.5f;
		}
		if ((int)getWidth() % 2 != 0)
		{
			startPos.z += 0.5f;
			endPos.z += 0.5f;
			newPos.z += 0.5f;
		}
		Box b = Box(startPos, endPos);
		EntityArr entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ENTITY, b);

		if (entities.size() > 0)
		{
			String names = "";
			for (auto entity : entities)
			{
				names += "EntityName[ " + entity->getEntityName() + "]";
			}
			LordLogInfo("ItemBuild::onItemUse  failure by  entities.size() = %d entityNames[%s] userName[%s]", (int)entities.size(), names.c_str(), pPlayer->getEntityName().c_str());
			return false;
		}
		int yaw =(((int)pPlayer->rotationYaw) % 360 + 360/* remve minus*/ + 180 /* build rotation degree */+ 45 /*error range*/) % 360 / 90 * 90;
		LordLogInfo("ItemBuild::onItemUse  player yaw = [%4.f], build yaw = [%d]", pPlayer->rotationYaw , yaw);
		if (!SCRIPT_EVENT::PlaceItemBuildingEvent::invoke(pPlayer->getPlatformUserId(), getActorId(), getActorType(), newPos, yaw, startPos, endPos)) {
			LordLogInfo("ItemBuild::onItemUse  failure by PlaceItemBuildingEvent userName[%s]", pPlayer->getEntityName().c_str());
			return false;
		}
		//Vector3 v = Vector3(newPos.x, newPos.y + 5.f, newPos.z);
		//pWorld->addEntityBird(v, yaw, pPlayer->getPlatformUserId(), 100, "g1041_bird.actor", "body", "301");
		//pWorld->addEntityBulletin(newPos, yaw, 1);
		//EntityBuildNpc* entity = LordNew EntityBuildNpc(pWorld, newPos);
		//entity->m_userId = pPlayer->getPlatformUserId();
		//entity->m_itemId = itemID;
		//entity->rotationYaw = (int)(pPlayer->rotationYaw / 90 - 2) * 90.0f;
		//entity->setPosition(newPos);
		//entity->setActorId(getActorId());
		//pWorld->spawnEntityInWorld(entity);
		--pStack->stackSize;
	}
	
	return true;
}

int ItemBuild::getActorId()
{
	auto building = BuildingSetting::getBuildingByItemId(itemID);
	assert(building);
	return building->actorId;
}

int ItemBuild::getActorType()
{
	auto building = BuildingSetting::getBuildingByItemId(itemID);
	assert(building);
	return building->type;
}

float ItemBuild::getLength()
{
	auto building = BuildingSetting::getBuildingByItemId(itemID);
	assert(building);
	return building->length;
}

float ItemBuild::getWidth()
{
	auto building = BuildingSetting::getBuildingByItemId(itemID);
	assert(building);
	return building->width;
}

float ItemBuild::getHeight()
{
	auto building = BuildingSetting::getBuildingByItemId(itemID);
	assert(building);
	return building->height;
}

ItemHouse::ItemHouse(int id, const String & name)
	: Item(id, name)
{
}

bool ItemHouse::onItemUse(ItemStackPtr pStack, EntityPlayer * pPlayer, World * pWorld, const BlockPos & pos, int face, const Vector3 & hit)
{
	if (!pWorld->m_isClient)
	{
		BlockPos pos2 = BlockPos(pos.x +(int) getLength(), pos.y + (int)getHeight(), pos.z + (int)getWidth());
		EntityArr entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ENTITY, Box(pos, pos2));
		for (auto iter = entities.begin(); iter != entities.end();) {
			if (dynamic_cast<EntityItem*>((Entity*)*iter))
			{
				iter = entities.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		if (entities.size() > 0)
		{
			String names = "";
			for (auto entity : entities)
			{
				names += "EntityName[ " + entity->getEntityName() + "]";
			}
			LordLogInfo(" ItemHouse::onItemUse  failure by  entities.size() = %d entityNames[%s] userName[%s]", (int)entities.size(), names.c_str(), pPlayer->getEntityName().c_str());
			return false;
		}

		if (!SCRIPT_EVENT::PlaceItemHouseEvent::invoke(pPlayer->getPlatformUserId(), getTemplateName(), pos, pos2)) {
			LordLogInfo(" ItemBuild::onItemUse  failure by PlaceItemHouseEvent userName[%s]", pPlayer->getEntityName().c_str());
			return false;
		}
		--pStack->stackSize;
		//pWorld->createOrDestroyHouseFromSchematic(getTemplateName(), pos.getNegY());
	}
	//--pStack->stackSize;
	return true;
}

float ItemHouse::getLength()
{
	HouseItem* house = HouseSetting::getHouseItem(itemID);
	assert(house);
	return house->length;
}

float ItemHouse::getWidth()
{
	HouseItem* house = HouseSetting::getHouseItem(itemID);
	assert(house);
	return house->width;
}

float ItemHouse::getHeight()
{
	HouseItem* house = HouseSetting::getHouseItem(itemID);
	assert(house);
	return house->height;
}

String ItemHouse::getTemplateName()
{
	HouseItem* house = HouseSetting::getHouseItem(itemID);
	assert(house);
	return house->templateName;
}

ItemBanner::ItemBanner(int id, const String& name)
	: ItemAnvilBlock(id, name)
{
}

String ItemBanner::getUnlocalizedName(ItemStackPtr pStack)
{
	return BlockManager::sBlocks[blockID]->getUnlocalizedName() + StringUtil::Format(".%d", pStack->subItemID);
}

ItemAppIcon::ItemAppIcon(int id, const String& name)
	: Item(id, name)
{
	maxStackSize = 1;
}

ItemTransparent::ItemTransparent(int id, const String& name)
	: Item(id, name)
{
}

ItemToolGather::ItemToolGather(int id, const String & name, ToolMaterial * material)
	: ItemTool(id, name, 1.0f, material)
{
}

bool ItemToolGather::canHarvestBlock(Block * pBlock)
{
	return true;
}

ItemFruitCurrency::ItemFruitCurrency(int id, const String & name)
	: Item(id, name)
{
}

}
