#pragma once
#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{
	class RanchCommon {
	public:
		int itemId = 0;
		int num = 0;
		int price = 0;
		i64 userId = 0;
		DEFINE_NETWORK_SERIALIZER(itemId, num, price , userId);

	public:
		RanchCommon() {

		}
		RanchCommon(int itemId, int num, int price)  {
			this->itemId = itemId;
			this->num = num;
			this->price = price;
		}
			
	};

	class RanchItem {
	public:
		int id = 0;
		String name = "";
		String desc = "";
		String icon = "";
		String makeIcon = "";
		i32 cubePrice = 0;
		int type = 0;
		DEFINE_NETWORK_SERIALIZER(id, name, desc, icon, cubePrice, type);
	};

	class RanchSetting
	{
	private:
		static bool loadRanchItem(bool bIsClient);

	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
	public:
		static RanchItem* getRanchItem(int ranchItemId);
		static i32 getRanchItemType(int ranchItemId);
		static String getRanchItemIcon(int ranchItemId);
		static String getRanchItemName(int ranchItemId);
		static String getRanchItemDesc(int ranchItemId);
		static String getRanchItemMakeIcon(int ranchItemId);
		static i32 getRanchItemCubePrice(int ranchItemId);

	private:
		static std::map<int, RanchItem*> m_ranchItemMap;
	};
}
