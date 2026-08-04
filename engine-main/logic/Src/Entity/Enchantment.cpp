#include "Enchantment.h"
#include "DamageSource.h"

#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Util/Random.h"
#include "WorldGenerator/WeightedRandomItem.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"

namespace BLOCKMAN
{

Enchantment** Enchantment::enchantmentsList = NULL;
Enchantment** Enchantment::enchantmentsBookList = NULL;

Enchantment* Enchantment::protection = NULL;
Enchantment* Enchantment::fireProtection = NULL;
Enchantment* Enchantment::featherFalling = NULL;
Enchantment* Enchantment::blastProtection = NULL;
Enchantment* Enchantment::projectileProtection = NULL;
Enchantment* Enchantment::respiration = NULL;
Enchantment* Enchantment::aquaAffinity = NULL;
Enchantment* Enchantment::thorns = NULL;
Enchantment* Enchantment::sharpness = NULL;
Enchantment* Enchantment::smite = NULL;
Enchantment* Enchantment::baneOfArthropods = NULL;
Enchantment* Enchantment::knockback = NULL;
Enchantment* Enchantment::fireAspect = NULL;
Enchantment* Enchantment::looting = NULL;
Enchantment* Enchantment::efficiency = NULL;
Enchantment* Enchantment::silkTouch = NULL;
Enchantment* Enchantment::unbreaking = NULL;
Enchantment* Enchantment::fortune = NULL;
Enchantment* Enchantment::power = NULL;
Enchantment* Enchantment::punch = NULL;
Enchantment* Enchantment::flame = NULL;
Enchantment* Enchantment::infinity = NULL;
Enchantment* Enchantment::poison = NULL;

void Enchantment::initialize()
{
	enchantmentsList = (Enchantment**)LordMalloc(sizeof(Enchantment*) * MAX_ENCH_COUNT);
	memset(enchantmentsList, 0, sizeof(Enchantment*) * 256);

	enchantmentsBookList = (Enchantment**)LordMalloc(sizeof(Enchantment*) * MAX_ENCH_COUNT);
	memset(enchantmentsBookList, 0, sizeof(Enchantment*) * 256);

	protection			= LordNew EnchantmentProtection(0, 10, 0);
	fireProtection		= LordNew EnchantmentProtection(1, 5, 1);
	featherFalling		= LordNew EnchantmentProtection(2, 5, 2);
	blastProtection		= LordNew EnchantmentProtection(3, 2, 3);
	projectileProtection= LordNew EnchantmentProtection(4, 5, 4);
	respiration			= LordNew EnchantmentOxygen(5, 2);

	aquaAffinity		= LordNew EnchantmentWaterWorker(6, 2);
	thorns				= LordNew EnchantmentThorns(7, 1);
	sharpness			= LordNew EnchantmentDamage(16, 10, 0);
	smite				= LordNew EnchantmentDamage(17, 5, 1);
	baneOfArthropods	= LordNew EnchantmentDamage(18, 5, 2);
	knockback			= LordNew EnchantmentKnockback(19, 5);
	fireAspect			= LordNew EnchantmentFireAspect(20, 2);
	looting				= LordNew EnchantmentLootBonus(21, 2, ENCHANTMENT_TYPE_WEAPON);
	efficiency			= LordNew EnchantmentDigging(32, 10);
	silkTouch			= LordNew EnchantmentUntouching(33, 1);
	unbreaking			= LordNew EnchantmentDurability(34, 5);
	fortune				= LordNew EnchantmentLootBonus(35, 2, ENCHANTMENT_TYPE_DIGGER);
	power				= LordNew EnchantmentArrowDamage(48, 10);
	punch				= LordNew EnchantmentArrowKnockback(49, 2);
	flame				= LordNew EnchantmentArrowFire(50, 2);
	infinity			= LordNew EnchantmentArrowInfinite(51, 1);
	poison				= LordNew EnchantmentPoison(52, 2);
}

void Enchantment::unInitialize()
{
	LordSafeDelete(protection);
	LordSafeDelete(fireProtection);
	LordSafeDelete(featherFalling);
	LordSafeDelete(blastProtection);
	LordSafeDelete(projectileProtection);
	LordSafeDelete(respiration);
	LordSafeDelete(aquaAffinity);
	LordSafeDelete(thorns);
	LordSafeDelete(sharpness);
	LordSafeDelete(smite);
	LordSafeDelete(baneOfArthropods);
	LordSafeDelete(knockback);
	LordSafeDelete(fireAspect);
	LordSafeDelete(looting);
	LordSafeDelete(efficiency);
	LordSafeDelete(silkTouch);
	LordSafeDelete(unbreaking);
	LordSafeDelete(fortune);
	LordSafeDelete(power);
	LordSafeDelete(punch);
	LordSafeDelete(flame);
	LordSafeDelete(infinity);
	LordSafeDelete(poison);

	LordSafeFree(enchantmentsBookList);
	LordSafeFree(enchantmentsList);
}

Enchantment::Enchantment(int id, int w, ENCHANTMENT_TYPE t)
	: effectId(id)
	, weight(w)
	, type(t)
	, classType(0)
{
	effectId = id;
	weight = w;
	type = t;

	LordAssert(enchantmentsList[id] == NULL);
	enchantmentsList[id] = this;
}

Enchantment *Enchantment::getEnchantmentById(int id)
{
	return enchantmentsList[id];
}

bool Enchantment::canApply(ItemStackPtr pStack)
{
	return false;
}

bool Enchantment::canEnchantItem(Item* pItem)
{
	if (type == ENCHANTMENT_TYPE_ALL)
		return true;
	
	ItemArmor* pArmor = dynamic_cast<ItemArmor*>(pItem);
	if (pArmor)
	{
		if (type == ENCHANTMENT_TYPE_ARMOR)
		{
			return true;
		}
		else
		{
			return pArmor->armorType == 0 ? type == ENCHANTMENT_TYPE_ARMOR_HEAD : 
				(pArmor->armorType == 2 ? type == ENCHANTMENT_TYPE_ARMOR_LEGS :
					(pArmor->armorType == 1 ? type == ENCHANTMENT_TYPE_ARMOR_TORSO :
						(pArmor->armorType == 3 ? type == ENCHANTMENT_TYPE_ARMOR_FEET : false)));
		}
	}
	else
	{
		ItemSword* pSword = dynamic_cast<ItemSword*>(pItem);
		if (pSword)
			return type == ENCHANTMENT_TYPE_WEAPON;
		ItemTool* pTool = dynamic_cast<ItemTool*>(pItem);
		if (pTool)
			return type == ENCHANTMENT_TYPE_DIGGER;
		ItemBow* pBow = dynamic_cast<ItemBow*>(pItem);
		if (pBow)
			return type == ENCHANTMENT_TYPE_BOW;
	}
	return false;
}

String EnchantmentProtection::protectionName[COUNT]			= { "all", "fire", "fall", "explosion", "projectile" };
int EnchantmentProtection::baseEnchantability[COUNT]		= {1, 10, 5, 5, 3};
int EnchantmentProtection::levelEnchantability[COUNT]		= {11, 8, 6, 8, 6};
int EnchantmentProtection::thresholdEnchantability[COUNT]	= {20, 12, 10, 12, 15};

EnchantmentProtection::EnchantmentProtection(int id, int w, int t)
	: Enchantment(id, w, ENCHANTMENT_TYPE_ARMOR)
	, protectionType(t)
{
	classType = ENCHANTMENT_CLASS_PROTECTION;
	if (t == 2)
		type = ENCHANTMENT_TYPE_ARMOR_FEET;
}

int EnchantmentProtection::calcModifierDamage(int l, DamageSource* ds)
{
	if (ds->canHarmInCreative())
	{
		return 0;
	}
	else
	{
		float fbase = (float)(6 + l * l) / 3.0F;
		int result = 0;
		switch (protectionType)
		{
		case 0:
			result = int(Math::Floor(fbase * 0.75f));
			break;
		case 1:
			if (ds->isFireDamage())
				result = int(Math::Floor(fbase * 1.25f));
			break;
		case 2:
			if (ds == DamageSource::fall)
				result = int(Math::Floor(fbase * 2.5f));
			break;
		case 3:
			if (ds->isExplosion())
				result = int(Math::Floor(fbase * 1.5f));
			break;
		case 4:
			if (ds->isProjectile())
				result = int(Math::Floor(fbase * 1.5f));
			break;
		default:
			break;
		}

		return result;
	}
}


bool EnchantmentProtection::canApplyTogether(Enchantment* pEnchantment)
{
	if (pEnchantment->isClass(ENCHANTMENT_CLASS_PROTECTION))
	{
		EnchantmentProtection* pProtection = dynamic_cast<EnchantmentProtection*>(pEnchantment);
		if (!pProtection)
			return false;
		if (pProtection->protectionType == protectionType)
			return false;
		else
			return protectionType == 2 || pProtection->protectionType == 2;
	}
	else
	{
		return Enchantment::canApplyTogether(pEnchantment);
	}
}

int EnchantmentProtection::getFireTimeForEntity(Entity* pEntity, int t)
{
	/*int var2 = EnchantmentHelper.getMaxEnchantmentLevel(Enchantment.fireProtection.effectId, pEntity.getLastActiveItems());

	if (var2 > 0)
	{
		t -= MathHelper.floor_float((float)par1 * (float)var2 * 0.15F);
	}*/

	return t;
}

float EnchantmentProtection::getBlastDamageReduction(Entity* pEntity, float r)
{
	/*int var3 = EnchantmentHelper.getMaxEnchantmentLevel(Enchantment.blastProtection.effectId, pEntity.getLastActiveItems());

	if (var3 > 0)
	{
		r -= (double)MathHelper.floor_double(r * (double)((float)var3 * 0.15F));
	}*/

	return r;
}

EnchantmentOxygen::EnchantmentOxygen(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_ARMOR_HEAD)
{
	classType = ENCHANTMENT_CLASS_OXYGEN;
	setName("oxygen");
}

EnchantmentWaterWorker::EnchantmentWaterWorker(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_ARMOR_HEAD)
{
	classType = ENCHANTMENT_CLASS_WATERWORKER;
	setName("waterWorker");
}

EnchantmentThorns::EnchantmentThorns(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_ARMOR_TORSO)
{
	classType = ENCHANTMENT_CLASS_THORNS;
	setName("thorns");
}

bool EnchantmentThorns::canApply(ItemStackPtr pStack)
{
	// return pStack.getItem() instanceof ItemArmor ? true : super.canApply(pStack);
	return false;
}

bool EnchantmentThorns::shouldHit(int l, Random* pRand)
{
	return l <= 0 ? false : pRand->nextFloat() < 0.15F * (float)l;
}

int EnchantmentThorns::getDamage(int l, Random* pRand)
{
	return l > 10 ? l - 10 : 1 + pRand->nextInt(4);
}

void EnchantmentThorns::damageArmor(Entity* pEntity, EntityLivingBase* pLiving, Random* pRand)
{
	int var3 = EnchantmentHelper::getThornsModifier(pLiving);
	ItemStackPtr var4 = EnchantmentHelper::getEnchantedItem(Enchantment::thorns, pLiving);

	if (shouldHit(var3, pRand))
	{
		pEntity->attackEntityFrom(DamageSource::causeThornsDamage(pLiving), (float)getDamage(var3, pRand));
		//pEntity->playSound("damage.thorns", 0.5F, 1.0F);
		pEntity->playSoundByType(ST_ThornsHurt);

		if (var4)
		{
			var4->damageItem(3, pLiving);
		}
	}
	else if (var4)
	{
		var4->damageItem(1, pLiving);
	}
}

String EnchantmentDamage::protectionName[COUNT]			= { "all", "undead", "arthropods" };
int EnchantmentDamage::baseEnchantability[COUNT]		= {1, 5, 5};
int EnchantmentDamage::levelEnchantability[COUNT]		= {11, 8, 8};
int EnchantmentDamage::thresholdEnchantability[COUNT]	= {20, 20, 20};

EnchantmentDamage::EnchantmentDamage(int id, int w, int t)
	: Enchantment(id, w, ENCHANTMENT_TYPE_WEAPON)
	, damageType(t)
{
	classType = ENCHANTMENT_CLASS_DAMAGE;
}

float EnchantmentDamage::calcModifierLiving(int level, EntityLivingBase* pLiving)
{
	if (damageType == 0)
	{
		return 1.0F + (float)Math::Max(0, level - 1) * 0.5F;
	}
	else if (damageType == 1 && pLiving->getCreatureAttribute() == CREATURE_ATTRIBUTE_UNDEAD)
	{
		return (float)level * 2.5F;
	}
	else 
	{
		return damageType == 2 && pLiving->getCreatureAttribute() == CREATURE_ATTRIBUTE_ARTHROPOD ? (float)level * 2.5F : 0.0F;
	}
}

bool EnchantmentDamage::canApply(ItemStackPtr pStack)
{
	// return pStack.getItem() instanceof ItemAxe ? true : super.canApply(pStack);
	return false;
}

EnchantmentKnockback::EnchantmentKnockback(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_WEAPON)
{
	classType = ENCHANTMENT_CLASS_KNOCKBACK;
	setName("knockback");
}

EnchantmentFireAspect::EnchantmentFireAspect(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_WEAPON)
{
	classType = ENCHANTMENT_CLASS_FIREASPECT;
	setName("fire");
}

EnchantmentLootBonus::EnchantmentLootBonus(int id, int w, ENCHANTMENT_TYPE t)
	: Enchantment(id, w, t)
{
	classType = ENCHANTMENT_CLASS_LOOTBONUS;
	if (t == ENCHANTMENT_TYPE_DIGGER)
		setName("lootBonusDigger");
	else
		setName("lootBonus");
}

bool EnchantmentLootBonus::canApplyTogether(Enchantment* pEnchantment)
{
	return Enchantment::canApplyTogether(pEnchantment) && pEnchantment->effectId != silkTouch->effectId;
}

EnchantmentDigging::EnchantmentDigging(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_DIGGER)
{
	classType = ENCHANTMENT_CLASS_DIGGING;
	setName("digging");
}

bool EnchantmentDigging::canApply(ItemStackPtr pStack)
{
	// return pStack.getItem().itemID == Item.shears.itemID ? true : super.canApply(pStack);
	return false;
}

EnchantmentUntouching::EnchantmentUntouching(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_DIGGER)
{
	classType = ENCHANTMENT_CLASS_UNTOUCHING;
	setName("untouching");
}

bool EnchantmentUntouching::canApplyTogether(Enchantment* pEnchantment)
{
	return Enchantment::canApplyTogether(pEnchantment) && pEnchantment->effectId != fortune->effectId;
}

bool EnchantmentUntouching::canApply(ItemStackPtr pStack)
{
	// return pStack.getItem().itemID == Item.shears.itemID ? true : super.canApply(pStack);
	return false;
}

EnchantmentDurability::EnchantmentDurability(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_DIGGER)
{
	classType = ENCHANTMENT_CLASS_DURABILITY;
	setName("durability");
}

bool EnchantmentDurability::canApply(ItemStackPtr pStack)
{
	// return pStack.isItemStackDamageable() ? true : super.canApply(pStack);
	return false;
}

bool EnchantmentDurability::negateDamage(ItemStackPtr pStack, int l, Random* pRandom)
{
	// return pStack.getItem() instanceof ItemArmor && par2Random.nextFloat() < 0.6F ? false : par2Random.nextInt(par1 + 1) > 0;
	return false;
}

EnchantmentArrowDamage::EnchantmentArrowDamage(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_BOW)
{
	classType = ENCHANTMENT_CLASS_ARROWDAMAGE;
	setName("arrowDamage");
}

EnchantmentArrowKnockback::EnchantmentArrowKnockback(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_BOW)
{
	classType = ENCHANTMENT_CLASS_ARROWKBACK;
	setName("arrowKnockback");
}

EnchantmentArrowFire::EnchantmentArrowFire(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_BOW)
{
	classType = ENCHANTMENT_CLASS_ARROWFIRE;
	setName("arrowFire");
}

EnchantmentArrowInfinite::EnchantmentArrowInfinite(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_BOW)
{
	classType = ENCHANTMENT_CLASS_ARROWINFINITE;
	setName("arrowInfinite");
}

void EnchantmentModifierDamage::calculateModifier(Enchantment* pEnchantment, int level)
{
	damageModifier += pEnchantment->calcModifierDamage(level, source);
}

void EnchantmentModifierLiving::calculateModifier(Enchantment* pEnchantment, int level)
{
	livingModifier += pEnchantment->calcModifierLiving(level, entityLiving);
}

Random* EnchantmentHelper::enchantmentRand = NULL;
EnchantmentModifierDamage* EnchantmentHelper::enchantmentModifierDamage = NULL;
EnchantmentModifierLiving* EnchantmentHelper::enchantmentModifierLiving = NULL;

void EnchantmentHelper::initialize()
{
	enchantmentRand = LordNew Random();
	enchantmentModifierDamage = LordNew EnchantmentModifierDamage();
	enchantmentModifierLiving = LordNew EnchantmentModifierLiving();
}

void EnchantmentHelper::unInitalize()
{
	LordSafeDelete(enchantmentRand);
	LordSafeDelete(enchantmentModifierDamage);
	LordSafeDelete(enchantmentModifierLiving);
}

int EnchantmentHelper::getEnchantmentLevel(int enchID, ItemStackPtr pStack)
{
	if (!pStack)
		return 0;

	NBTTagList* pNbtLst = pStack->getEnchantmentTagList();
	if (pNbtLst == NULL)
		return 0;

	for (int i = 0; i < pNbtLst->tagCount(); ++i)
	{
		short id = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("id");
		short lvl = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("lvl");

		if (id == enchID)
		{
			return lvl;
		}
	}

	return 0;
}

void EnchantmentHelper::getEnchantments(ItemStackPtr pStack, EnchantMentMap& outmap)
{
	NBTTagList* pNbtLst = pStack->itemID == Item::enchantedBook->itemID ? Item::enchantedBook->getStoredEnchantments(pStack) : pStack->getEnchantmentTagList();

	if (pNbtLst != NULL)
	{
		for (int i = 0; i < pNbtLst->tagCount(); ++i)
		{
			i16 id = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("id");
			i16 lvl = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("lvl");
			outmap.insert(std::make_pair(int(id), int(lvl)));
		}
	}
}

void EnchantmentHelper::setEnchantments(const EnchantMentMap& inmap, ItemStackPtr pStack)
{
	NBTTagList* pNbtLst = LordNew NBTTagList();

	for (EnchantMentMap::const_iterator it = inmap.begin(); it != inmap.end(); ++it)
	{
		int id = it->first;
		int lvl = it->second;
		NBTTagCompound* pCompound = LordNew NBTTagCompound();
		pCompound->setShort("id", id);
		pCompound->setShort("lvl", lvl);
		pNbtLst->appendTag(pCompound);

		if (pStack->itemID == Item::enchantedBook->itemID)
		{
			Item::enchantedBook->getEnchantedItemStack_do(pStack, LordNew EnchantmentData(id, lvl));
		}
	}

	if (pNbtLst->tagCount() > 0)
	{
		if (pStack->itemID != Item::enchantedBook->itemID)
		{
			pStack->setTagInfo("ench", pNbtLst);
		}
	}
	else if (pStack->hasTagCompound())
	{
		pStack->getTagCompound()->removeTag("ench");
	}
}

int EnchantmentHelper::getMaxEnchantmentLevel(int enchID, const ItemStackArr& stacks)
{
	if (stacks.empty())
		return 0;

	int maxLevel = 0;
	int length = int(stacks.size());

	for (int i = 0; i < length; ++i)
	{
		ItemStackPtr pStack = stacks[i];
		int level = getEnchantmentLevel(enchID, pStack);

		if (level > maxLevel)
		{
			maxLevel = level;
		}
	}

	return maxLevel;
}

void EnchantmentHelper::applyEnchantmentModifier(IEnchantmentModifier* pModifier, ItemStackPtr pStack)
{
	if (!pStack)
		return;
	
	NBTTagList* pNbtLst = pStack->getEnchantmentTagList();
	if (!pNbtLst)
		return;
	
	for (int i = 0; i < pNbtLst->tagCount(); ++i)
	{
		i16 id = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("id");
		i16 lvl = ((NBTTagCompound*)pNbtLst->tagAt(i))->getShort("lvl");

		if (Enchantment::enchantmentsList[id] != NULL)
		{
			pModifier->calculateModifier(Enchantment::enchantmentsList[id], lvl);
		}
	}
}

void EnchantmentHelper::applyEnchantmentModifierArray(IEnchantmentModifier* pModifier, const ItemStackArr& stacks)
{
	int length = int(stacks.size());

	for (int i = 0; i < length; ++i)
	{
		ItemStackPtr pStack = stacks[i];
		applyEnchantmentModifier(pModifier, pStack);
	}
}

int EnchantmentHelper::getEnchantmentModifierDamage(const ItemStackArr& stacks, DamageSource* pSource)
{
	enchantmentModifierDamage->damageModifier = 0;
	enchantmentModifierDamage->source = pSource;
	applyEnchantmentModifierArray(enchantmentModifierDamage, stacks);

	if (enchantmentModifierDamage->damageModifier > 25)
	{
		enchantmentModifierDamage->damageModifier = 25;
	}

	return ((enchantmentModifierDamage->damageModifier + 1) >> 1) + enchantmentRand->nextInt((enchantmentModifierDamage->damageModifier >> 1) + 1);
}

float EnchantmentHelper::getEnchantmentModifierLiving(EntityLivingBase* pLiving1, EntityLivingBase* pLiving2)
{
	enchantmentModifierLiving->livingModifier = 0.0F;
	enchantmentModifierLiving->entityLiving = pLiving2;
	applyEnchantmentModifier(enchantmentModifierLiving, pLiving1->getHeldItem());
	return enchantmentModifierLiving->livingModifier;
}

int EnchantmentHelper::getKnockbackModifier(EntityLivingBase* pLiving1, EntityLivingBase* pLiving2)
{
	return getEnchantmentLevel(Enchantment::knockback->effectId, pLiving1->getHeldItem());
}

int EnchantmentHelper::getFireAspectModifier(EntityLivingBase* pLiving)
{
	return getEnchantmentLevel(Enchantment::fireAspect->effectId, pLiving->getHeldItem());
}

int EnchantmentHelper::getPoisonModifier(EntityLivingBase* pLiving)
{
	return getEnchantmentLevel(Enchantment::poison->effectId, pLiving->getHeldItem());
}

int EnchantmentHelper::getRespiration(EntityLivingBase* pLiving)
{
	return getMaxEnchantmentLevel(Enchantment::respiration->effectId, pLiving->getLastActiveItems());
}

int EnchantmentHelper::getEfficiencyModifier(EntityLivingBase* pLiving)
{
	return getEnchantmentLevel(Enchantment::efficiency->effectId, pLiving->getHeldItem());
}

bool EnchantmentHelper::getSilkTouchModifier(EntityLivingBase* pLiving)
{
	return getEnchantmentLevel(Enchantment::silkTouch->effectId, pLiving->getHeldItem()) > 0;
}

int EnchantmentHelper::getFortuneModifier(EntityLivingBase* pLving)
{
	return getEnchantmentLevel(Enchantment::fortune->effectId, pLving->getHeldItem());
}

int EnchantmentHelper::getLootingModifier(EntityLivingBase* pLiving)
{
	return getEnchantmentLevel(Enchantment::looting->effectId, pLiving->getHeldItem());
}

bool EnchantmentHelper::getAquaAffinityModifier(EntityLivingBase* pLiving)
{
	return getMaxEnchantmentLevel(Enchantment::aquaAffinity->effectId, pLiving->getLastActiveItems()) > 0;
}

int EnchantmentHelper::getThornsModifier(EntityLivingBase* pLiving)
{
	return getMaxEnchantmentLevel(Enchantment::thorns->effectId, pLiving->getLastActiveItems());
}

ItemStackPtr EnchantmentHelper::getEnchantedItem(Enchantment* pEnch, EntityLivingBase* pLiving)
{
	ItemStackArr lastActiveItems = pLiving->getLastActiveItems();
	int length = int(lastActiveItems.size());

	for (int i = 0; i < length; ++i)
	{
		ItemStackPtr pStack = lastActiveItems[i];

		if (pStack != NULL && getEnchantmentLevel(pEnch->effectId, pStack) > 0)
		{
			return pStack;
		}
	}

	return NULL;
}

int EnchantmentHelper::calcItemStackEnchantability(Random* pRand, int enchNum, int power, ItemStackPtr pStack)
{
	Item* pItem = pStack->getItem();
	int enchAbility = pItem->getItemEnchantability();

	if (enchAbility <= 0)
	{
		return 0;
	}
	else
	{
		if (power > 15)
		{
			power = 15;
		}

		int i = pRand->nextInt(8) + 1 + (power >> 1) + pRand->nextInt(power + 1);
		return enchNum == 0 ? Math::Max(i / 3, 1) : (enchNum == 1 ? i * 2 / 3 + 1 : Math::Max(i, power * 2));
	}
}

ItemStackPtr EnchantmentHelper::addRandomEnchantment(Random* pRand, ItemStackPtr pStack, int allowTreasure)
{
	EnchantmentDataArr dataarr;
	buildEnchantmentList(pRand, pStack, allowTreasure, dataarr);
	bool isbook = pStack->itemID == Item::book->itemID;

	if (isbook)
	{
		pStack->itemID = Item::enchantedBook->itemID;
	}

	for (int i = 0; i < int(dataarr.size()); ++i)
	{
		EnchantmentData* pEnch = (EnchantmentData*)dataarr[i];

		if (isbook)
		{
			Item::enchantedBook->getEnchantedItemStack_do(pStack, pEnch);
		}
		else
		{
			pStack->addEnchantment(pEnch->enchantmentobj, pEnch->enchantmentLevel);
		}
	}

	return pStack;
}

void EnchantmentHelper::buildEnchantmentList(Random* pRand, ItemStackPtr pStack, int allowTreasure, EnchantmentDataArr& outArr)
{
	EnchantmentDataArr resultArr;

	Item* pItem = pStack->getItem();
	int enchAbility = pItem->getItemEnchantability();

	if (enchAbility <= 0)
		return;
	
	enchAbility /= 2;
	enchAbility = 1 + pRand->nextInt((enchAbility >> 1) + 1) + pRand->nextInt((enchAbility >> 1) + 1);
	int totalEnchAbility = enchAbility + allowTreasure;
	float var6 = (pRand->nextFloat() + pRand->nextFloat() - 1.0F) * 0.15F;
	int finalEnchAbility = (int)((float)totalEnchAbility * (1.0F + var6) + 0.5F);

	if (finalEnchAbility < 1)
		finalEnchAbility = 1;

	WRItemList dataarr;
	mapEnchantmentData(finalEnchAbility, pStack, dataarr);

	if (dataarr.empty())
		return;

	EnchantmentData* enchData = (EnchantmentData*)WeightedRandom::getRandomItem(*pRand, dataarr);

	if (enchData != NULL)
	{
		outArr.push_back(enchData);

		for (int i = finalEnchAbility; pRand->nextInt(50) <= i; i >>= 1)
		{
			for (WRItemList::iterator it = dataarr.begin(); it!=dataarr.end(); )
			{
				bool needRemove = false;
				EnchantmentData* pEnch1 = static_cast<EnchantmentData*>(*it);
				if (!pEnch1)
					continue;

				for (int k = 0; k < int(outArr.size()); ++k)
				{
					EnchantmentData* pEnch2 = outArr[k];

					if (pEnch2->enchantmentobj->canApplyTogether(pEnch1->enchantmentobj))
						continue;

					needRemove = true;
					break;
				}
				if (needRemove)
				{
					it = dataarr.erase(it);
				}
				else
					++it;
			}

			if (!dataarr.empty())
			{
				EnchantmentData* enchData2 = (EnchantmentData*)WeightedRandom::getRandomItem(*pRand, dataarr);
				outArr.push_back(enchData2);
			}
		}
	}
}

void EnchantmentHelper::mapEnchantmentData(int enchAbility, ItemStackPtr pStack, WRItemList& outarr)
{
	Item* pItem = pStack->getItem();
	bool isBook = pStack->itemID == Item::book->itemID;
	Enchantment** enchantments = Enchantment::enchantmentsList;
	
	for (int i = 0; i < 256; ++i)
	{
		Enchantment* pEnch = enchantments[i];

		if (!pEnch)
			continue;
		if (pEnch->canEnchantItem(pItem) || isBook)
		{
			for (int i = pEnch->getMinLevel(); i <= pEnch->getMaxLevel(); ++i)
			{
				if (enchAbility >= pEnch->getMinEnchantability(i) && enchAbility <= pEnch->getMaxEnchantability(i))
				{
					outarr.push_back(LordNew EnchantmentData(pEnch, i));
				}
			}
		}
	}
}

EnchantmentPoison::EnchantmentPoison(int id, int w)
	: Enchantment(id, w, ENCHANTMENT_TYPE_WEAPON)
{
	classType = ENCHANTMENT_CLASS_FIREASPECT;
	setName("poison");
}

}