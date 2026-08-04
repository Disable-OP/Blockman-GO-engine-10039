#include "GunStoreSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{
	list<StoreTab*>::type		GunStoreSetting::m_store_tab_list = {};
	map<int, StoreProperty*>::type      GunStoreSetting::m_store_property_map = {};
	map<int, StoreAttribute*>::type      GunStoreSetting::m_store_attribute_map = {};

	map<int, StoreGun*>::type      GunStoreSetting::m_store_gun_map = {};
	map<int, StoreGunLevel*>::type      GunStoreSetting::m_store_gun_level_map = {};

	map<int, StoreProp*>::type      GunStoreSetting::m_store_prop_map = {};
	map<int, StorePropLevel*>::type      GunStoreSetting::m_store_prop_level_map = {};

	void GunStoreSetting::loadSetting()
	{
		loadStoreTabSetting();
		loadPropertySetting();
		loadAttributeSetting();
			
		loadGunSetting();
		loadGunLevelSetting();

		loadPropSetting();
		loadPropLevelSetting();
	}

	void GunStoreSetting::unloadSetting()
	{
		for (auto iter : m_store_tab_list)
		{
			delete iter;
		}
		m_store_tab_list.clear();

		for (auto iter : m_store_property_map)
		{
			delete iter.second;
		}
		m_store_property_map.clear();

		for (auto iter : m_store_attribute_map)
		{
			delete iter.second;
		}
		m_store_attribute_map.clear();

		for (auto iter : m_store_gun_map)
		{
			delete iter.second;
		}
		m_store_gun_map.clear();

		for (auto iter : m_store_gun_level_map)
		{
			delete iter.second;
		}
		m_store_gun_level_map.clear();

		for (auto iter : m_store_prop_map)
		{
			delete iter.second;
		}
		m_store_prop_map.clear();

		for (auto iter : m_store_prop_level_map)
		{
			delete iter.second;
		}
		m_store_prop_level_map.clear();
	}

	void GunStoreSetting::onUpdate(ui32 nTimeElapse)
	{
		for (auto iter : m_store_gun_map)
		{
			if (iter.second->UpgradeTime > 0)
			{
				iter.second->UpgradeTime -= nTimeElapse;
			}
			else
			{
				iter.second->UpgradeTime = 0;
			}
		}
		for (auto iter : m_store_prop_map)
		{
			if (iter.second->UpgradeTime > 0)
			{
				iter.second->UpgradeTime -= nTimeElapse;
			}
			else
			{
				iter.second->UpgradeTime = 0;
			}
		}
	}

	void GunStoreSetting::loadStoreTabSetting()
	{
		StoreTab* pStoreTab = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "StoreTabSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		while (pReader->Read())
		{
			pStoreTab = new StoreTab();
			pReader->GetInt("Id", pStoreTab->Id);
			pReader->GetString("Name", pStoreTab->Name);
			pReader->GetString("Icon", pStoreTab->Icon);
			pReader->GetInt("Type", pStoreTab->Type);
			int TabType = 0;
			pReader->GetInt("TabType", TabType);
			pStoreTab->TabType = (StoreTabType)TabType;
			m_store_tab_list.push_back(pStoreTab);
		}
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadPropertySetting()
	{
		StoreProperty* pStoreProperty = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PropertySetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreProperty = new StoreProperty();
			pReader->GetInt("Id", pStoreProperty->Id);
			pReader->GetString("Name", pStoreProperty->Name);
			pReader->GetString("Icon", pStoreProperty->Icon);
			m_store_property_map[pStoreProperty->Id] = pStoreProperty;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadAttributeSetting()
	{
		StoreAttribute* pStoreAttribute = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "AttributeSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreAttribute = new StoreAttribute();
			pReader->GetInt("Id", pStoreAttribute->Id);
			pReader->GetString("Name", pStoreAttribute->Name);
			pReader->GetString("Icon", pStoreAttribute->Icon);
			pReader->GetInt("Value", pStoreAttribute->Value);
			pReader->GetBool("IsPlus", pStoreAttribute->IsPlus);
			m_store_attribute_map[pStoreAttribute->Id] = pStoreAttribute;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadGunSetting()
	{
		StoreGun* pStoreGun = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "GunSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreGun = new StoreGun();
			pReader->GetInt("Id", pStoreGun->Id);
			pReader->GetInt("ItemId", pStoreGun->ItemId);
			pReader->GetInt("TabType", pStoreGun->TabType);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pStoreGun->Type = (StoreGunType)Type;
			pReader->GetString("Name", pStoreGun->Name);
			pReader->GetString("Tag", pStoreGun->Tag);
			pReader->GetString("Image", pStoreGun->Image);
			pReader->GetString("LockImage", pStoreGun->LockImage);
			pReader->GetInt("MaxLevel", pStoreGun->MaxLevel);
			pReader->GetString("Actor", pStoreGun->Actor);
			pReader->GetFloat("Damage", pStoreGun->Damage);
			pReader->GetFloat("MaxDamage", pStoreGun->MaxDamage);
			pReader->GetInt("ShootSpeed", pStoreGun->ShootSpeed);
			pReader->GetInt("Bullet", pStoreGun->Bullet);
			pReader->GetInt("SpareBullet", pStoreGun->SpareBullet);
			pReader->GetString("PropertyIds", pStoreGun->PropertyIds);
			int PriceType = 0;
			pReader->GetInt("PriceType", PriceType);
			pStoreGun->PriceType = (CurrencyType)PriceType;
			pReader->GetInt("Price", pStoreGun->Price);
			pReader->GetInt("UnlockLevel", pStoreGun->UnlockLevel);
			pReader->GetString("LockTip", pStoreGun->LockTip);
			pReader->GetInt("SortWeight", pStoreGun->SortWeight);
			bool HasGet = false;
			pReader->GetBool("HasGet", HasGet);
			pStoreGun->Status = HasGet ? STATUS_HAS_GET : STATUS_LOCK;
			m_store_gun_map[pStoreGun->Id] = pStoreGun;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadGunLevelSetting()
	{
		StoreGunLevel* pStoreGunLevel = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "GunLevelSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreGunLevel = new StoreGunLevel();
			pReader->GetInt("Id", pStoreGunLevel->Id);
			pReader->GetInt("GunId", pStoreGunLevel->GunId);
			pReader->GetInt("Level", pStoreGunLevel->Level);
			pReader->GetFloat("CurDamage", pStoreGunLevel->CurDamage);
			pReader->GetFloat("AddDamage", pStoreGunLevel->AddDamage);
			pReader->GetFloat("MaxDamage", pStoreGunLevel->MaxDamage);
			int UpgradePriceType = 0;
			pReader->GetInt("UpgradePriceType", UpgradePriceType);
			pStoreGunLevel->UpgradePriceType = (CurrencyType)UpgradePriceType;
			pReader->GetInt("UpgradePrice", pStoreGunLevel->UpgradePrice);
			pReader->GetInt("UpgradeTime", pStoreGunLevel->UpgradeTime);
			int QuickUpgradePriceType = 0;
			pReader->GetInt("QuickUpgradePriceType", QuickUpgradePriceType);
			pStoreGunLevel->QuickUpgradePriceType = (CurrencyType)QuickUpgradePriceType;
			pReader->GetInt("QuickUpgradePrice", pStoreGunLevel->QuickUpgradePrice);
			m_store_gun_level_map[pStoreGunLevel->Id] = pStoreGunLevel;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadPropSetting()
	{
		StoreProp* pStoreProp = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PropSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreProp = new StoreProp();
			pReader->GetInt("Id", pStoreProp->Id);
			pReader->GetInt("ItemId", pStoreProp->ItemId);
			pReader->GetInt("TabType", pStoreProp->TabType);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pStoreProp->Type = (StorePropType)Type;
			pReader->GetString("Name", pStoreProp->Name);
			pReader->GetString("Tag", pStoreProp->Tag);
			pReader->GetString("Image", pStoreProp->Image);
			pReader->GetString("LockImage", pStoreProp->LockImage);
			pReader->GetString("Desc", pStoreProp->Desc);
			pReader->GetInt("MaxLevel", pStoreProp->MaxLevel);
			pReader->GetString("AttributeIds", pStoreProp->AttributeIds);
			pReader->GetString("PropertyIds", pStoreProp->PropertyIds);
			int PriceType = 0;
			pReader->GetInt("PriceType", PriceType);
			pStoreProp->PriceType = (CurrencyType)PriceType;
			pReader->GetInt("Price", pStoreProp->Price);
			pReader->GetInt("UnlockLevel", pStoreProp->UnlockLevel);
			pReader->GetString("LockTip", pStoreProp->LockTip);
			pReader->GetInt("SortWeight", pStoreProp->SortWeight);
			bool HasGet = false;
			pReader->GetBool("HasGet", HasGet);
			pStoreProp->Status = HasGet ? STATUS_HAS_GET : STATUS_LOCK;
			m_store_prop_map[pStoreProp->Id] = pStoreProp;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	void GunStoreSetting::loadPropLevelSetting()
	{
		StorePropLevel* pStorePropLevel = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PropLevelSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStorePropLevel = new StorePropLevel();
			pReader->GetInt("Id", pStorePropLevel->Id);
			pReader->GetInt("PropId", pStorePropLevel->PropId);
			pReader->GetInt("Level", pStorePropLevel->Level);
			pReader->GetString("CurValues", pStorePropLevel->CurValues);
			pReader->GetString("AddValues", pStorePropLevel->AddValues);
			int UpgradePriceType = 0;
			pReader->GetInt("UpgradePriceType", UpgradePriceType);
			pStorePropLevel->UpgradePriceType = (CurrencyType)UpgradePriceType;
			pReader->GetInt("UpgradePrice", pStorePropLevel->UpgradePrice);
			pReader->GetInt("UpgradeTime", pStorePropLevel->UpgradeTime);
			int QuickUpgradePriceType = 0;
			pReader->GetInt("QuickUpgradePriceType", QuickUpgradePriceType);
			pStorePropLevel->QuickUpgradePriceType = (CurrencyType)QuickUpgradePriceType;
			pReader->GetInt("QuickUpgradePrice", pStorePropLevel->QuickUpgradePrice);

			StringArray curValues = StringUtil::Split(pStorePropLevel->CurValues, "#");
			for (String curValue : curValues)
			{
				StringArray value = StringUtil::Split(curValue, "=");
				if (value.size() == 2)
				{
					pStorePropLevel->cur_value_map.insert({ StringUtil::ParseI32(value[0]), StringUtil::ParseI32(value[1]) });
				}
			}
			StringArray addValues = StringUtil::Split(pStorePropLevel->AddValues, "#");
			for (String addValue : addValues)
			{
				StringArray value = StringUtil::Split(addValue, "=");
				if (value.size() == 2)
				{
					pStorePropLevel->add_value_map.insert({ StringUtil::ParseI32(value[0]), StringUtil::ParseI32(value[1]) });
				}
			}

			m_store_prop_level_map[pStorePropLevel->Id] = pStorePropLevel;
		}
		pReader->Close();
		LordSafeDelete(pReader);
	}

	list<StoreProperty*>::type GunStoreSetting::findStorePropertyByIds(String propertyIds)
	{
		list<StoreProperty*>::type result;
		StringArray ids = StringUtil::Split(propertyIds, "#");
		for (String id : ids)
		{
			StoreProperty* property = findStorePropertyById(StringUtil::ParseI32(id));
			if (property)
			{
				result.push_back(property);
			}
		}
		return result;
	}

	StoreProperty * GunStoreSetting::findStorePropertyById(int id)
	{
		for (auto iter : m_store_property_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<StoreAttribute*>::type GunStoreSetting::findStoreAttributeByIds(String attributeIds)
	{
		list<StoreAttribute*>::type result;
		StringArray ids = StringUtil::Split(attributeIds, "#");
		for (String id : ids)
		{
			StoreAttribute* attribute = findStoreAttributeById(StringUtil::ParseI32(id));
			if (attribute)
			{
				result.push_back(attribute);
			}
		}
		return result;
	}

	StoreAttribute * GunStoreSetting::findStoreAttributeById(int id)
	{
		for (auto iter : m_store_attribute_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	bool GunStoreSetting::sortGunFunc(StoreGun * gun1, StoreGun * gun2)
	{
		if (gun1->Status != gun2->Status)
		{
			if (gun1->Status == STATUS_NEW && gun2->Status == STATUS_NOT_GET)
			{
				return gun1->SortWeight > gun2->SortWeight;
			}
			if (gun1->Status == STATUS_NOT_GET && gun2->Status == STATUS_NEW)
			{
				return gun1->SortWeight > gun2->SortWeight;
			}
			return (int)gun1->Status > (int)gun2->Status;
		}
		return gun1->SortWeight > gun2->SortWeight;
	}

	list<StoreGun*>::type GunStoreSetting::sortStoreGuns(list<StoreGun*>::type guns)
	{
		guns.sort(sortGunFunc);
		return guns;
	}

	StoreGun * GunStoreSetting::findStoreGunById(int id)
	{
		for (auto iter : m_store_gun_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	StoreGun* GunStoreSetting::findSelectedGunByTabType(int type)
	{
		for (auto iter : m_store_gun_map)
		{
			if (iter.second->TabType == type && iter.second->IsSelected)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<StoreGun*>::type GunStoreSetting::findStoreGunByTabType(int type)
	{
		list<StoreGun*>::type result;
		for (auto iter : m_store_gun_map)
		{
			if (iter.second->TabType == type)
			{
				result.push_back(iter.second);
			}
		}
		result = sortStoreGuns(result);
		return result;
	}

	StoreGunLevel * GunStoreSetting::findStoreGunLevelByGunIdAndLevel(int gunId, int level)
	{
		for (auto iter : m_store_gun_level_map)
		{
			if (iter.second->GunId == gunId && iter.second->Level == level)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<StoreGun*>::type GunStoreSetting::parseUpdateStoreGunData(const String & data, map<int, bool>::type& needSortTypes)
	{
		list<StoreGun*>::type result;
		if (data.length() == 0)
			return result;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return result;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return result;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			StoreGun* gun = findStoreGunById(Id);
			if (!gun)
				continue;	
			int Level = obj.HasMember("Level") ? obj.FindMember("Level")->value.GetInt() : gun->Level;
			int Chip = obj.HasMember("Chip") ? obj.FindMember("Chip")->value.GetInt() : gun->Chip;
			StoreStatus Status = obj.HasMember("Status") ? (StoreStatus)obj.FindMember("Status")->value.GetInt() : gun->Status;
			gun->Level = Level;
			gun->Chip = Chip;
			gun->UpgradeTime = obj.HasMember("UpgradeTime") ? obj.FindMember("UpgradeTime")->value.GetInt() * 1000 : gun->UpgradeTime;
			if (gun->Status != Status && gun->Status != STATUS_NEW)
			{
				if (needSortTypes.find(gun->TabType) == needSortTypes.end())
				{
					needSortTypes.insert({ gun->TabType, true });
				}
			}
			gun->Status = Status;
			gun->HasChange = true;
			result.push_back(gun);
		}
		LordDelete(doc);
		return result;
	}

	bool GunStoreSetting::sortPropFunc(StoreProp * prop1, StoreProp * prop2)
	{
		if (prop1->Status != prop2->Status)
		{
			if (prop1->Status == STATUS_NEW && prop2->Status == STATUS_NOT_GET)
			{
				return prop1->SortWeight > prop2->SortWeight;
			}
			if (prop1->Status == STATUS_NOT_GET && prop2->Status == STATUS_NEW)
			{
				return prop1->SortWeight > prop2->SortWeight;
			}
			return (int)prop1->Status > (int)prop2->Status;
		}
		return prop1->SortWeight > prop2->SortWeight;
	}

	list<StoreProp*>::type GunStoreSetting::sortStoreProps(list<StoreProp*>::type props)
	{
		props.sort(sortPropFunc);
		return props;
	}

	StoreProp * GunStoreSetting::findStorePropById(int id)
	{
		for (auto iter : m_store_prop_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	StoreProp * GunStoreSetting::findSelectedPropByTabType(int type)
	{
		for (auto iter : m_store_prop_map)
		{
			if (iter.second->TabType == type && iter.second->IsSelected)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<StoreProp*>::type GunStoreSetting::findStorePropByTabType(int type)
	{
		list<StoreProp*>::type result;
		for (auto iter : m_store_prop_map)
		{
			if (iter.second->TabType == type)
			{
				result.push_back(iter.second);
			}
		}
		result = sortStoreProps(result);
		return result;
	}

	StorePropLevel * GunStoreSetting::findStorePropLevelByPropIdAndLevel(int propId, int level)
	{
		for (auto iter : m_store_prop_level_map)
		{
			if (iter.second->PropId == propId && iter.second->Level == level)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<StoreProp*>::type GunStoreSetting::parseUpdateStorePropData(const String & data, map<int, bool>::type& needSortTypes)
	{
		list<StoreProp*>::type result;
		if (data.length() == 0)
			return result;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return result;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return result;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			StoreProp* prop = findStorePropById(Id);
			if (!prop)
				continue;
			int Level = obj.HasMember("Level") ? obj.FindMember("Level")->value.GetInt() : prop->Level;
			int Chip = obj.HasMember("Chip") ? obj.FindMember("Chip")->value.GetInt() : prop->Chip;
			StoreStatus Status = obj.HasMember("Status") ? (StoreStatus)obj.FindMember("Status")->value.GetInt() : prop->Status;
			prop->Level = Level;
			prop->Chip = Chip;
			prop->UpgradeTime = obj.HasMember("UpgradeTime") ? obj.FindMember("UpgradeTime")->value.GetInt() * 1000 : prop->UpgradeTime;
			if (prop->Status != Status && prop->Status != STATUS_NEW)
			{
				if (needSortTypes.find(prop->TabType) == needSortTypes.end())
				{
					needSortTypes.insert({ prop->TabType, true });
				}
			}
			prop->Status = Status;
			prop->HasChange = true;
			result.push_back(prop);
		}
		LordDelete(doc);
		return result;
	}

	String GunStoreSetting::getPriceIcon(CurrencyType priceType)
	{
		return UICommon::getCurrencyIconByType(priceType);
	}

	String GunStoreSetting::getPriceText(CurrencyType priceType, int chip, int price, StoreStatus status)
	{
		if (priceType == OTHER_MONEY)
		{
			if (status == STATUS_HAS_GET || status == STATUS_HAS_USE)
			{
				return StringUtil::Format("▢FF9758EE%d", chip);
			}
			else
			{
				return StringUtil::Format("▢FF9758EE%d/%d", chip, price);
			}
		}
		else
		{
			return StringUtil::Format("▢FFF8DA00%d", price);
		}
	}
	 
	bool GunStoreSetting::checkHasMoney(CurrencyType priceType, int chip, int price)
	{
		if (priceType == UNKNOWN)
			return false;
		bool hasEnoughtMoney = false;
		if (priceType == OTHER_MONEY)
		{
			hasEnoughtMoney = chip >= price;
		}
		else
		{
			i64 money = Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)priceType);
			hasEnoughtMoney = money >= price;
		}
		return hasEnoughtMoney;
	}

	void GunStoreSetting::useGunStoreFunc(StoreAction acton, int id)
	{
		switch (acton)
		{
		case FIRST_SHOW_GUN:
			ClientNetwork::Instance()->getSender()->sendFirstShowStoreGun(id);
			break;
		case USE_GUN:
			ClientNetwork::Instance()->getSender()->sendUseStoreGun(id);
			break;
		case BUY_GUN:
			ClientNetwork::Instance()->getSender()->sendBuyStoreGun(id);
			break;
		case UPGRADE_GUN:
			ClientNetwork::Instance()->getSender()->sendUpgradeStoreGun(id);
			break;
		case QUICK_UPGRADE_GUN:
			ClientNetwork::Instance()->getSender()->sendQuickUpgradeStoreGun(id);
			break;
		case FIRST_SHOW_PROP:
			ClientNetwork::Instance()->getSender()->sendFirstShowStoreProp(id);
			break;
		case USE_PROP:
			ClientNetwork::Instance()->getSender()->sendUseStoreProp(id);
			break;
		case UNLOAD_PROP:
			ClientNetwork::Instance()->getSender()->sendUnloadStoreProp(id);
			break;
		case BUY_PROP:
			ClientNetwork::Instance()->getSender()->sendBuyStoreProp(id);
			break;
		case UPGRADE_PROP:
			ClientNetwork::Instance()->getSender()->sendUpgradeStoreProp(id);
			break;
		case QUICK_UPGRADE_PROP:
			ClientNetwork::Instance()->getSender()->sendQuickUpgradeStoreProp(id);
			break;
		default:
			break;
		}
	}

}
