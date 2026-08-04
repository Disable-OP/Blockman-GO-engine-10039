#pragma once

#ifndef __RED_DOT_MANAGER_HEADER__
#define __RED_DOT_MANAGER_HEADER__
#include "Core.h"
#include "Object/Root.h"

using namespace LORD;

namespace BLOCKMAN {

	class RedDotManager : public ObjectAlloc
	{
	public:
		static void init();
		static void release();
		
	public:
		static bool checkRanchMain();
		static bool checkOrderMain();

	public:
		static bool checkOrder(i32 orderId);
		static bool checkOrderItem(i32 orderId, i32 orderIndex);


	public:
		static bool checkHouse();

	public:
		static bool checkStorage();

	public:
		static bool checkFactory();
		static bool checkFactoryById(i32 buildId);

	public:
		static bool checkFarming();
		static bool checkRaise();
		static bool checkRaiseById(i32 buildId);
		static bool checkPlant();
		static bool checkPlantById(i32 buildId);

	public:
		//Community
		static bool checkCommunity();
		static bool checkFriend();
		static bool checkClan();
		static bool checkMail();

	public:
		static bool checkAchievement();
		static bool checkAchievementById(i64 id);

	public:
		static bool checkBirdTask();

	};

}
#endif