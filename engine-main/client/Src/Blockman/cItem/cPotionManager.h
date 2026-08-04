#ifndef __CPOTIONMANAGER_H__
#define __CPOTIONMANAGER_H__

#include "cItem.h"

class cPotionManager
{
public:
	static void cRegisterPotionItem();

public:
	static BLOCKMAN::cItemPotion* itemInvisibility;
	static BLOCKMAN::cItemPotion* itemLongInvisibility;
	static BLOCKMAN::cItemPotion* itemJump;
	static BLOCKMAN::cItemPotion* itemLongJump;
	static BLOCKMAN::cItemPotion* itemStrongJump;
	static BLOCKMAN::cItemPotion* itemHeal;
	static BLOCKMAN::cItemPotion* itemStrongHeal;
	static BLOCKMAN::cItemPotion* itemMoveSpeed;
	static BLOCKMAN::cItemPotion* itemLongMoveSpeed;
	static BLOCKMAN::cItemPotion* itemStrongMoveSpeed;
	static BLOCKMAN::cItemPotion* itemPoison;
	static BLOCKMAN::cItemPotion* itemLongPoison;
	static BLOCKMAN::cItemPotion* itemStrongPoison;
	static BLOCKMAN::cItemPotion* itemNightVision;
	static BLOCKMAN::cItemPotion* itemLongNightVision;

	static BLOCKMAN::cItemPotion* itemSplashHeal;
	static BLOCKMAN::cItemPotion* itemSplashStrongHeal;
	static BLOCKMAN::cItemPotion* itemSplashPoison;
	static BLOCKMAN::cItemPotion* itemSplashLongPoison;
	static BLOCKMAN::cItemPotion* itemSplashStrongPoison;
};

#endif // __CPOTIONMANAGER_H__
