#pragma once
#ifndef __CHAT_UTILS_HEADER__
#define __CHAT_UTILS_HEADER__
#include "ChatComponent.h"
class ChatUtils
{
public:
	static ChatComponentString* getChatString(int sendId, String msg);
	template<typename ... T>
	static ChatComponentTranslation* getChatTranslation(String key, T ... args);
private:

};
#endif