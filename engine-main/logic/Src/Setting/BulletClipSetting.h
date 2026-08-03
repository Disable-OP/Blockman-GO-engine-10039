#pragma once
#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{

	class BulletClipPluginSetting
	{
	public:
		int	bulletId;
		int maxStack;
		DEFINE_NETWORK_SERIALIZER(bulletId, maxStack);
	};

	class BulletClipSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static BulletClipSetting* getBulletClipSetting(int bulletId);
		static void updateBulletClipSetting(BulletClipPluginSetting setting);

		int				bulletId;
		int             maxStack;

		String			bulletName;
		String			bulletClipModel;
		String			bulletIcon;
		String			icon;
	private:
		static std::map<int, BulletClipSetting*> m_bulletClipSettingMap;
	};
}
