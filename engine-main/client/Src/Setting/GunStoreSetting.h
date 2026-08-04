#pragma once
#ifndef __GUN_STORE_SETTING_HEADER__
#define __GUN_STORE_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum StoreAction 
	{
		FIRST_SHOW_GUN = 1,
		USE_GUN,
		BUY_GUN,
		UPGRADE_GUN,
		QUICK_UPGRADE_GUN,
		FIRST_SHOW_PROP,
		USE_PROP,
		UNLOAD_PROP,
		BUY_PROP,
		UPGRADE_PROP,
		QUICK_UPGRADE_PROP
	};

	enum StoreTabType
	{
		STORE_TAB_NONE = 0,
		STORE_TAB_GUN,
		STORE_TAB_PROP
	};

	enum StoreStatus
	{
		STATUS_LOCK = 0,
		STATUS_NOT_GET,
		STATUS_NEW,
		STATUS_HAS_GET,
		STATUS_HAS_USE,
	};

	enum StoreGunType
	{
		GUN_TYPE_GUN = 1,
		GUN_TYPE_ARMS
	};

	enum StorePropType
	{
		PROP_TYPE_PROP = 1,
		PROP_TYPE_BLOCK
	};

	class StoreTab
	{
	public:
		int Id = 0;
		String Name = "";
		String Icon = "";
		int Type = 0;
		StoreTabType TabType = STORE_TAB_NONE;
	};

	class StoreProperty
	{
	public:
		int Id = 0;
		String Name = "";
		String Icon = "";
	};

	class StoreAttribute
	{
	public:
		int Id = 0;
		String Name = "";
		String Icon = "";
		int Value = 0;
		bool IsPlus = true;
	};

	class StoreGun
	{
	public:
		int Id = 0;
		int ItemId = 0;
		int TabType = 0;
		StoreGunType Type = GUN_TYPE_GUN;
		String Name = "";
		String Tag = "";
		String Image = "";
		String LockImage = "";
		String Actor = "";
		int MaxLevel = 0;
		float Damage = 0.0f;
		float MaxDamage = 0.0f;
		int ShootSpeed = 0;
		int Bullet = 0;
		int SpareBullet = 0;
		String PropertyIds = "";
		CurrencyType PriceType = UNKNOWN;
		int Price = 0;
		int UnlockLevel = 0;
		String LockTip = "";
		int SortWeight = 0;

		bool HasChange = false;
		bool IsSelected = false;
		int Level = 1;
		int Chip = 0;
		int UpgradeTime = 0;
		StoreStatus Status = STATUS_LOCK;

	};

	class StoreGunLevel
	{
	public:
		int Id = 0;
		int GunId = 0;
		int Level = 0;
		float CurDamage = 0.0f;
		float AddDamage = 0.0f;
		float MaxDamage = 0.0f;
		CurrencyType UpgradePriceType = UNKNOWN;
		int UpgradePrice = 0;
		int UpgradeTime = 0;
		CurrencyType QuickUpgradePriceType = UNKNOWN;
		int QuickUpgradePrice = 0;
	};

	class StoreProp
	{
	public:
		int Id = 0;
		int ItemId = 0;
		int TabType = 0;
		StorePropType Type = PROP_TYPE_PROP;
		String Name = "";
		String Tag = "";
		String Image = "";
		String LockImage = "";
		String Desc = "";
		int MaxLevel = 0;
		String AttributeIds = "";
		String PropertyIds = "";
		CurrencyType PriceType = UNKNOWN;
		int Price = 0;
		int UnlockLevel = 0;
		String LockTip = "";
		int SortWeight = 0;

		bool HasChange = false;
		bool IsSelected = false;
		int Level = 1;
		int Chip = 0;
		int UpgradeTime = 0;
		StoreStatus Status = STATUS_LOCK;

	};

	class StorePropLevel
	{
	public:
		int Id = 0;
		int PropId = 0;
		int Level = 0;
		String CurValues = "";
		String AddValues = "";
		CurrencyType UpgradePriceType = UNKNOWN;
		int UpgradePrice = 0;
		int UpgradeTime = 0;
		CurrencyType QuickUpgradePriceType = UNKNOWN;
		int QuickUpgradePrice = 0;

		map<int, int>::type cur_value_map;
		map<int, int>::type add_value_map;

	public:
		int getCurValueById(int id) 
		{
			if (cur_value_map.find(id) != cur_value_map.end())
			{
				return cur_value_map[id];
			}
			return 0;
		}
		int getAddVauleById(int id)
		{
			if (add_value_map.find(id) != add_value_map.end())
			{
				return add_value_map[id];
			}
			return 0;
		}
	};

	class GunStoreSetting
	{
	private:
		static list<StoreTab*>::type m_store_tab_list;
		static map<int, StoreProperty*>::type m_store_property_map;
		static map<int, StoreAttribute*>::type m_store_attribute_map;

		static map<int, StoreGun*>::type m_store_gun_map;
		static map<int, StoreGunLevel*>::type m_store_gun_level_map;

		static map<int, StoreProp*>::type m_store_prop_map;
		static map<int, StorePropLevel*>::type m_store_prop_level_map;

	public:
		static void	loadSetting();
		static void	unloadSetting();
		static void onUpdate(ui32 nTimeElapse);

		static list<StoreTab*>::type getStoreTabs() { return m_store_tab_list; }
		static list<StoreProperty*>::type findStorePropertyByIds(String propertyIds);
		static StoreProperty* findStorePropertyById(int id);
		static list<StoreAttribute*>::type findStoreAttributeByIds(String attributeIds);
		static StoreAttribute* findStoreAttributeById(int id);

		static bool sortGunFunc(StoreGun* gun1, StoreGun* gun2);
		static list<StoreGun*>::type sortStoreGuns(list<StoreGun*>::type guns);
		static StoreGun* findStoreGunById(int id);
		static StoreGun* findSelectedGunByTabType(int type);
		static list<StoreGun*>::type findStoreGunByTabType(int type);
		static StoreGunLevel* findStoreGunLevelByGunIdAndLevel(int gunId, int level);
		static list<StoreGun*>::type parseUpdateStoreGunData(const String& data, map<int, bool>::type& needSortTypes);
	
		static bool sortPropFunc(StoreProp* prop1, StoreProp* prop2);
		static list<StoreProp*>::type sortStoreProps(list<StoreProp*>::type props);
		static StoreProp* findStorePropById(int id);
		static StoreProp* findSelectedPropByTabType(int type);
		static list<StoreProp*>::type findStorePropByTabType(int type);
		static StorePropLevel* findStorePropLevelByPropIdAndLevel(int propId, int level);
		static list<StoreProp*>::type parseUpdateStorePropData(const String& data, map<int, bool>::type& needSortTypes);

		static String getPriceIcon(CurrencyType priceType);
		static String getPriceText(CurrencyType priceType, int chip, int price, StoreStatus status = STATUS_LOCK);
		static bool checkHasMoney(CurrencyType priceType, int chip, int price);	
		static void useGunStoreFunc(StoreAction acton, int id);

	private:
		static void loadStoreTabSetting();
		static void loadPropertySetting();
		static void loadAttributeSetting();

		static void loadGunSetting();
		static void loadGunLevelSetting();

		static void loadPropSetting();
		static void loadPropLevelSetting();
	
	};
}

#endif 