#ifndef __POTIONMANAGER_H__
#define __POTIONMANAGER_H__

#include "Items.h"
#include "Item.h"

class PotionManager {
public:
	static void initialize();
	static void unInitialize();
	static void registerPotionItem();

public:
	static BLOCKMAN::PotionEffect* effectInvisibility;
	static BLOCKMAN::PotionEffect* effectLongInvisibility;
	static BLOCKMAN::PotionEffect* effectJump;
	static BLOCKMAN::PotionEffect* effectLongJump;
	static BLOCKMAN::PotionEffect* effectStrongJump;
	static BLOCKMAN::PotionEffect* effectHeal;
	static BLOCKMAN::PotionEffect* effectStrongHeal;
	static BLOCKMAN::PotionEffect* effectMoveSpeed;
	static BLOCKMAN::PotionEffect* effectLongMoveSpeed;
	static BLOCKMAN::PotionEffect* effectStrongMoveSpeed;
	static BLOCKMAN::PotionEffect* effectPoison;
	static BLOCKMAN::PotionEffect* effectLongPoison;
	static BLOCKMAN::PotionEffect* effectStrongPoison;
	static BLOCKMAN::PotionEffect* effectNightVision;
	static BLOCKMAN::PotionEffect* effectLongNightVision;

public:
	static BLOCKMAN::ItemPotion* itemInvisibility;
	static BLOCKMAN::ItemPotion* itemLongInvisibility;
	static BLOCKMAN::ItemPotion* itemJump;
	static BLOCKMAN::ItemPotion* itemLongJump;
	static BLOCKMAN::ItemPotion* itemStrongJump;
	static BLOCKMAN::ItemPotion* itemHeal;
	static BLOCKMAN::ItemPotion* itemStrongHeal;
	static BLOCKMAN::ItemPotion* itemMoveSpeed;
	static BLOCKMAN::ItemPotion* itemLongMoveSpeed;
	static BLOCKMAN::ItemPotion* itemStrongMoveSpeed;
	static BLOCKMAN::ItemPotion* itemPoison;
	static BLOCKMAN::ItemPotion* itemLongPoison;
	static BLOCKMAN::ItemPotion* itemStrongPoison;
	static BLOCKMAN::ItemPotion* itemNightVision;
	static BLOCKMAN::ItemPotion* itemLongNightVision;

	static BLOCKMAN::ItemPotion* itemSplashHeal;
	static BLOCKMAN::ItemPotion* itemSplashStrongHeal;
	static BLOCKMAN::ItemPotion* itemSplashPoison;
	static BLOCKMAN::ItemPotion* itemSplashLongPoison;
	static BLOCKMAN::ItemPotion* itemSplashStrongPoison;
	static BLOCKMAN::ItemPotion* getItemPotionByIndex(int index);
};

#endif
