#pragma once
#ifndef __MAIL_MANAGER_HEADER__
#define __MAIL_MANAGER_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"

namespace BLOCKMAN
{
	class BroadcastMessage
	{
	public:
		String msg = "";
		i32 rollTime = 5000;
		i32 stayTime = 5000;
		BroadcastMessage() {}
		BroadcastMessage(const String& message, i32 roll, i32 stay)  : msg(message), rollTime(roll), stayTime(stay) { }
	};

	class BroadcastManager : public ObjectAlloc
	{

	public:
		static void parseBroadcastData(i32 type , const String & content);
	private:
		static void praseRanchOrderHelp(const String & content);
		static void praseRanchOrderFinish(const String & content);
		static void praseRanchEnterGame(const String & content);


	private:
		static void praseCommon(const String & content);
	};

	
};

#endif