#include "PotionManager.h"
#include "Potion.h"
#include "Items.h"
#include "Item.h"

using namespace BLOCKMAN;

PotionEffect* PotionManager::effectInvisibility = nullptr;
PotionEffect* PotionManager::effectLongInvisibility = nullptr;
PotionEffect* PotionManager::effectJump = nullptr;
PotionEffect* PotionManager::effectLongJump = nullptr;
PotionEffect* PotionManager::effectStrongJump = nullptr;
PotionEffect* PotionManager::effectHeal = nullptr;
PotionEffect* PotionManager::effectStrongHeal = nullptr;
PotionEffect* PotionManager::effectMoveSpeed = nullptr;
PotionEffect* PotionManager::effectLongMoveSpeed = nullptr;
PotionEffect* PotionManager::effectStrongMoveSpeed = nullptr;
PotionEffect* PotionManager::effectPoison = nullptr;
PotionEffect* PotionManager::effectLongPoison = nullptr;
PotionEffect* PotionManager::effectStrongPoison = nullptr;
PotionEffect* PotionManager::effectNightVision = nullptr;
PotionEffect* PotionManager::effectLongNightVision = nullptr;

ItemPotion* PotionManager::itemInvisibility = nullptr;
ItemPotion* PotionManager::itemLongInvisibility = nullptr;
ItemPotion* PotionManager::itemJump = nullptr;
ItemPotion* PotionManager::itemLongJump = nullptr;
ItemPotion* PotionManager::itemStrongJump = nullptr;
ItemPotion* PotionManager::itemHeal = nullptr;
ItemPotion* PotionManager::itemStrongHeal = nullptr;
ItemPotion* PotionManager::itemMoveSpeed = nullptr;
ItemPotion* PotionManager::itemLongMoveSpeed = nullptr;
ItemPotion* PotionManager::itemStrongMoveSpeed = nullptr;
ItemPotion* PotionManager::itemPoison = nullptr;
ItemPotion* PotionManager::itemLongPoison = nullptr;
ItemPotion* PotionManager::itemStrongPoison = nullptr;
ItemPotion* PotionManager::itemNightVision = nullptr;
ItemPotion* PotionManager::itemLongNightVision = nullptr;

ItemPotion* PotionManager::itemSplashHeal = nullptr;
ItemPotion* PotionManager::itemSplashStrongHeal = nullptr;
ItemPotion* PotionManager::itemSplashPoison = nullptr;
ItemPotion* PotionManager::itemSplashLongPoison = nullptr;
ItemPotion* PotionManager::itemSplashStrongPoison = nullptr;

ItemPotion* ItemPotions[20];
void PotionManager::initialize()
{
	effectInvisibility = LordNew PotionEffect(Potion::invisibility->getId(), 3600);
	effectLongInvisibility = LordNew PotionEffect(Potion::invisibility->getId(), 9600);
	effectJump = LordNew PotionEffect(Potion::jump->getId(), 3600);
	effectLongJump = LordNew PotionEffect(Potion::jump->getId(), 9600);
	effectStrongJump = LordNew PotionEffect(Potion::jump->getId(), 1800, 1);
	effectHeal = LordNew PotionEffect(Potion::heal->getId(), 1);
	effectStrongHeal = LordNew PotionEffect(Potion::heal->getId(), 1, 1);
	effectMoveSpeed = LordNew PotionEffect(Potion::moveSpeed->getId(), 3600);
	effectLongMoveSpeed = LordNew PotionEffect(Potion::moveSpeed->getId(), 9600);
	effectStrongMoveSpeed = LordNew PotionEffect(Potion::moveSpeed->getId(), 1800, 1);
	effectPoison = LordNew PotionEffect(Potion::poison->getId(), 900);
	effectLongPoison = LordNew PotionEffect(Potion::poison->getId(), 1800);
	effectStrongPoison = LordNew PotionEffect(Potion::poison->getId(), 432, 1);
	effectNightVision = LordNew PotionEffect(Potion::nightVision->getId(), 3600);
	effectLongNightVision = LordNew PotionEffect(Potion::nightVision->getId(), 9600);
}

void PotionManager::unInitialize()
{
	LordSafeDelete(effectInvisibility);
	LordSafeDelete(effectLongInvisibility);
	LordSafeDelete(effectJump);
	LordSafeDelete(effectLongJump);
	LordSafeDelete(effectStrongJump);
	LordSafeDelete(effectHeal);
	LordSafeDelete(effectStrongHeal);
	LordSafeDelete(effectMoveSpeed);
	LordSafeDelete(effectLongMoveSpeed);
	LordSafeDelete(effectStrongMoveSpeed);
	LordSafeDelete(effectPoison);
	LordSafeDelete(effectLongPoison);
	LordSafeDelete(effectStrongPoison);
	LordSafeDelete(effectNightVision);
	LordSafeDelete(effectLongNightVision);
}

void PotionManager::registerPotionItem()
{
	int index = Item::POTION_ITEM_ID_BEGIN - 256;

	itemInvisibility = LordNew ItemPotion(index + 0, "potion_invisibility");
	itemInvisibility->addEffect(effectInvisibility);
	ItemPotions[0] = itemInvisibility;

	itemLongInvisibility = LordNew ItemPotion(index + 1, "potion_long_invisibility");
	itemLongInvisibility->addEffect(effectLongInvisibility);
	ItemPotions[1] = itemInvisibility;

	itemJump = LordNew ItemPotion(index + 2, "potion_jump");
	itemJump->addEffect(effectJump);
	ItemPotions[2] = itemInvisibility;

	itemLongJump = LordNew ItemPotion(index + 3, "potion_long_jump");
	itemLongJump->addEffect(effectLongJump);
	ItemPotions[3] = itemInvisibility;

	itemStrongJump = LordNew ItemPotion(index + 4, "potion_strong_jump");
	itemStrongJump->addEffect(effectStrongJump);
	ItemPotions[4] = itemInvisibility;

	itemHeal = LordNew ItemPotion(index + 5, "potion_heal");
	itemHeal->addEffect(effectHeal);
	ItemPotions[5] = itemInvisibility;

	itemStrongHeal = LordNew ItemPotion(index + 6, "potion_strong_heal");
	itemStrongHeal->addEffect(effectStrongHeal);
	ItemPotions[6] = itemInvisibility;

	itemMoveSpeed = LordNew ItemPotion(index + 7, "potion_move_speed");
	itemMoveSpeed->addEffect(effectMoveSpeed);
	ItemPotions[7] = itemInvisibility;

	itemLongMoveSpeed = LordNew ItemPotion(index + 8, "potion_long_move_speed");
	itemLongMoveSpeed->addEffect(effectLongMoveSpeed);
	ItemPotions[8] = itemInvisibility;

	itemStrongMoveSpeed = LordNew ItemPotion(index + 9, "potion_strong_move_speed");
	itemStrongMoveSpeed->addEffect(effectStrongMoveSpeed);
	ItemPotions[9] = itemInvisibility;

	itemPoison = LordNew ItemPotion(index + 10, "potion_poison");
	itemPoison->addEffect(effectPoison);
	ItemPotions[10] = itemInvisibility;

	itemLongPoison = LordNew ItemPotion(index + 11, "potion_long_poison");
	itemLongPoison->addEffect(effectLongPoison);
	ItemPotions[11] = itemInvisibility;

	itemStrongPoison = LordNew ItemPotion(index + 12, "potion_strong_poison");
	itemStrongPoison->addEffect(effectStrongPoison);
	ItemPotions[12] = itemInvisibility;

	itemNightVision = LordNew ItemPotion(index + 13, "potion_night_vision");
	itemNightVision->addEffect(effectNightVision);
	ItemPotions[13] = itemInvisibility;

	itemLongNightVision = LordNew ItemPotion(index + 14, "potion_long_night_vision");
	itemLongNightVision->addEffect(effectLongNightVision);
	ItemPotions[14] = itemInvisibility;

	itemSplashHeal = LordNew ItemPotion(index + 15, "potion_splash_heal", true);
	itemSplashHeal->addEffect(effectHeal);
	ItemPotions[15] = itemInvisibility;

	itemSplashStrongHeal = LordNew ItemPotion(index + 16, "potion_splash_strong_heal", true);
	itemSplashStrongHeal->addEffect(effectStrongHeal);
	ItemPotions[16] = itemInvisibility;

	itemSplashPoison = LordNew ItemPotion(index + 17, "potion_splash_poison", true);
	itemSplashPoison->addEffect(effectPoison);
	ItemPotions[17] = itemInvisibility;

	itemSplashLongPoison = LordNew ItemPotion(index + 18, "potion_splash_long_poison", true);
	itemSplashLongPoison->addEffect(effectLongPoison);
	ItemPotions[18] = itemInvisibility;

	itemSplashStrongPoison = LordNew ItemPotion(index + 19, "potion_splash_strong_poison", true);
	itemSplashStrongPoison->addEffect(effectStrongPoison);
	ItemPotions[19] = itemInvisibility;
}

BLOCKMAN::ItemPotion* PotionManager::getItemPotionByIndex(int index)
{
	if (index > 19)
		return nullptr;

	return ItemPotions[index];
}