#include "cPotionManager.h"
#include "Item/PotionManager.h"
#include "Item/Item.h"
#include "cItem/cItem.h"

using namespace BLOCKMAN;

cItemPotion* cPotionManager::itemInvisibility = nullptr;
cItemPotion* cPotionManager::itemLongInvisibility = nullptr;

cItemPotion* cPotionManager::itemJump = nullptr;
cItemPotion* cPotionManager::itemLongJump = nullptr;
cItemPotion* cPotionManager::itemStrongJump = nullptr;

cItemPotion* cPotionManager::itemHeal = nullptr;
cItemPotion* cPotionManager::itemStrongHeal = nullptr;

cItemPotion* cPotionManager::itemMoveSpeed = nullptr;
cItemPotion* cPotionManager::itemLongMoveSpeed = nullptr;
cItemPotion* cPotionManager::itemStrongMoveSpeed = nullptr;

cItemPotion* cPotionManager::itemPoison = nullptr;
cItemPotion* cPotionManager::itemLongPoison = nullptr;
cItemPotion* cPotionManager::itemStrongPoison = nullptr;

cItemPotion* cPotionManager::itemNightVision = nullptr;
cItemPotion* cPotionManager::itemLongNightVision = nullptr;

cItemPotion* cPotionManager::itemSplashHeal = nullptr;
cItemPotion* cPotionManager::itemSplashStrongHeal = nullptr;

cItemPotion* cPotionManager::itemSplashPoison = nullptr;
cItemPotion* cPotionManager::itemSplashLongPoison = nullptr;
cItemPotion* cPotionManager::itemSplashStrongPoison = nullptr;

void cPotionManager::cRegisterPotionItem()
{
	int index = Item::POTION_ITEM_ID_BEGIN - 256;

	itemInvisibility = LordNew cItemPotion(index + 0, "potion_invisibility", "potion");
	itemInvisibility->addEffect(PotionManager::effectInvisibility);

	itemLongInvisibility = LordNew cItemPotion(index + 1, "potion_long_invisibility", "potion");
	itemLongInvisibility->addEffect(PotionManager::effectLongInvisibility);

	itemJump = LordNew cItemPotion(index + 2, "potion_jump", "potion");
	itemJump->addEffect(PotionManager::effectJump);

	itemLongJump = LordNew cItemPotion(index + 3, "potion_long_jump", "potion");
	itemLongJump->addEffect(PotionManager::effectLongJump);

	itemStrongJump = LordNew cItemPotion(index + 4, "potion_strong_jump", "potion");
	itemStrongJump->addEffect(PotionManager::effectStrongJump);

	itemHeal = LordNew cItemPotion(index + 5, "potion_heal", "potion");
	itemHeal->addEffect(PotionManager::effectHeal);

	itemStrongHeal = LordNew cItemPotion(index + 6, "potion_strong_heal", "potion");
	itemStrongHeal->addEffect(PotionManager::effectStrongHeal);

	itemMoveSpeed = LordNew cItemPotion(index + 7, "potion_move_speed", "potion");
	itemMoveSpeed->addEffect(PotionManager::effectMoveSpeed);

	itemLongMoveSpeed = LordNew cItemPotion(index + 8, "potion_long_move_speed", "potion");
	itemLongMoveSpeed->addEffect(PotionManager::effectLongMoveSpeed);

	itemStrongMoveSpeed = LordNew cItemPotion(index + 9, "potion_strong_move_speed", "potion");
	itemStrongMoveSpeed->addEffect(PotionManager::effectStrongMoveSpeed);

	itemPoison = LordNew cItemPotion(index + 10, "potion_poison", "potion");
	itemPoison->addEffect(PotionManager::effectPoison);

	itemLongPoison = LordNew cItemPotion(index + 11, "potion_long_poison", "potion");
	itemLongPoison->addEffect(PotionManager::effectLongPoison);

	itemStrongPoison = LordNew cItemPotion(index + 12, "potion_strong_poison", "potion");
	itemStrongPoison->addEffect(PotionManager::effectStrongPoison);

	itemNightVision = LordNew cItemPotion(index + 13, "potion_night_vision", "potion");
	itemNightVision->addEffect(PotionManager::effectNightVision);

	itemLongNightVision = LordNew cItemPotion(index + 14, "potion_long_night_vision", "potion");
	itemLongNightVision->addEffect(PotionManager::effectLongNightVision);

	itemSplashHeal = LordNew cItemPotion(index + 15, "potion_splash_heal", "potion", true);
	itemSplashHeal->addEffect(PotionManager::effectHeal);

	itemSplashStrongHeal = LordNew cItemPotion(index + 16, "potion_splash_strong_heal", "potion", true);
	itemSplashStrongHeal->addEffect(PotionManager::effectStrongHeal);

	itemSplashPoison = LordNew cItemPotion(index + 17, "potion_splash_poison", "potion", true);
	itemSplashPoison->addEffect(PotionManager::effectPoison);

	itemSplashLongPoison = LordNew cItemPotion(index + 18, "potion_splash_long_poison", "potion", true);
	itemSplashLongPoison->addEffect(PotionManager::effectLongPoison);

	itemSplashStrongPoison = LordNew cItemPotion(index + 19, "potion_splash_strong_poison", "potion", true);
	itemSplashStrongPoison->addEffect(PotionManager::effectStrongPoison);
}
