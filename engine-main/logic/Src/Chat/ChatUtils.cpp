#include "ChatUtils.h"
#include "ChatComponent.h"

ChatComponentString * ChatUtils::getChatString(int sendId, String msg)
{
	return LordNew ChatComponentString(sendId, msg);
}

template<typename ... T>
ChatComponentTranslation * ChatUtils::getChatTranslation(String key, T ... args)
{
	ChatComponentTranslation* info = LordNew ChatComponentTranslation;
	info->key = key;
	info->formatArgs = { args ... };
	return info;
}
