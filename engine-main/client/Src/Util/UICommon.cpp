#include "UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "World/BirdSimulator.h"
#include "Setting/GameTypeSetting.h"
#include "GUI/RootGuiLayout.h"
#include "Util/LanguageManager.h"
#include "Entity/EntitySessionNpc.h"
#include "cWorld/WorldClient.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN {

	bool UICommon::checkMoney(i32 price, i32 type)
	{
		return checkMoney(price, (CurrencyType)type);
	}

	bool UICommon::checkMoney(i32 price, CurrencyType type)
	{
		if (!checkWalletParam())
		{
			return false;
		}

		i64 currency = 0;
		if (type == CurrencyType::OTHER_MONEY)
		{
			currency = getOtherCurrency();
		}
		else
		{
			currency = Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)type);
		}

		if (price > currency)
		{
			ShowLackOfMoneyTipDialogEvent::emit((i32)(price - currency), type);
			return false;
		}
		return true;
	}

	bool UICommon::checkRanchItems(const std::vector<RanchCommon>& items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		return checkRanchItems(newItems);
	}

	bool UICommon::checkRanchItems(const vector<RanchCommon>::type& items)
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			if (items.size() > 0)
			{
				ShowRanchLockItemsEvent::emit(items);
				return false;
			}
			return true;
		}

		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();

		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		i32 prosperity = info->prosperity;
		i64 currency = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();

		i32 needCurrency = 0;
		i32 needProsperity = 0;

		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0 && item.itemId < 900000)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}

			if (item.itemId == 900001)
			{
				needCurrency = (int)(item.num - currency);
			}

			if (item.itemId == 900002)
			{
				needProsperity = item.num - prosperity;
			}
		}

		if (newItems.size() > 0)
		{
			ShowRanchLockItemsEvent::emit(newItems);
			return false;
		}

		if (needProsperity > 0)
		{
			ShowRanchCommonTipEvent::emit("gui_ranch_check_prosperity");
			return false;
		}

		if (needCurrency > 0)
		{
			ShowLackOfMoneyTipDialogEvent::emit(needCurrency, CurrencyType::GAME_MONEY);
			return false;
		}

		return true;
	}

	bool UICommon::checkRanchStorageCapacity(const vector<RanchCommon>::type& items)
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			ShowRanchStorageFullTipEvent::emit("gui_ranch_storage_full_operation_failure");
			return false;
		}
		i32 num = 0;
		for (auto item : items)
		{
			num += item.num;
		}

		if (!Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->canSave(num))
		{
			ShowRanchStorageFullTipEvent::emit("gui_ranch_storage_full_operation_failure");
			return false;
		}

		return true;
	}

	bool UICommon::checkRanchLevel(i32 needLevel)
	{
		if (checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= needLevel)
		{
			return true;
		}

		ShowRanchCommonTipEvent::emit("gui_ranch_check_level");
		return false;
	}

	bool UICommon::isRanchIconVisible()
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		auto ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& ranchStartPos = Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->startPos;
		const auto& ranchEndPos = Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->endPos;
		const auto& pos = Blockman::Instance()->m_pPlayer->position;
		return (pos.x >= ranchStartPos.x) && (pos.z >= ranchStartPos.z) && (pos.x <= ranchEndPos.x) && (pos.z <= ranchEndPos.z);
	}

	bool UICommon::checkOpenFlying()
	{
		//if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		//{
		//	if (checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		//	{
		//		return !Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->isCanFlying;
		//	}
		//}
		return false;
	}

	vector<RanchCommon>::type UICommon::getLackRanchItems(const std::vector<RanchCommon>& items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}
		}
		return newItems;
	}

	vector<RanchCommon>::type UICommon::getLackRanchItems(const vector<RanchCommon>::type & items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}
		}

		return newItems;
	}

	bool UICommon::checkBirdMiniTip(const BirdScope& scope)
	{
		return scope.id > 0 && !RootGuiLayout::Instance()->isBirdViewShow();
	}

	bool UICommon::checkBirdConvert()
	{
		return checkBirdParam() && Blockman::Instance()->m_pPlayer->m_birdSimulator->isConvert();
	}

	String UICommon::getBirdMiniTipText(const BirdScope& scope)
	{
		switch (scope.type)
		{
		case 1:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_store");
		case 2:
		{
			String title = LanguageManager::Instance()->getString("gui_bird_mini_tip_open");
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(scope.id));
			if (!entity)
			{
				return  StringUtil::Format(title.c_str(), "npc");
			}
			return StringUtil::Format(title.c_str(), entity->getName().c_str());
		}
		case 3:
			return LanguageManager::Instance()->getString("gui_bird_mini_tip_open_lottery");
		case 4:
			return LanguageManager::Instance()->getString(checkBirdConvert() ? "gui_bird_mini_tip_stop_convert" : "gui_bird_mini_tip_convert");
		case 5:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_chest");
		}
		return "";
	}

	void UICommon::onMiniTipClick()
	{
		if (!checkBirdParam())
		{
			return;
		}
		BirdScope  item = Blockman::Instance()->m_pPlayer->m_birdSimulator->getScopeByPosition(Blockman::Instance()->m_pPlayer->position);
		switch (item.type)
		{
		case 1:
			ShowBirdStoreEvent::emit(item.id, 0);
			break;
		case 2:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			ShowBirdTaskTipEvent::emit(entity->getSessionContent());
		}
			break;
		case 3:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			ShowBirdLotteryByNpcEvent::emit(entity->entityId);
		}
			break;
		case 4:
			ClientNetwork::Instance()->getSender()->sendBirdBagOperation(0, 6);
			break;
		case 5:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			Blockman::Instance()->m_pPlayer->attackSessionNpc(entity);
		}
		break;
		}

	}

	bool UICommon::checkPlayerParam()
	{
		return Blockman::Instance() && Blockman::Instance() && Blockman::Instance()->m_pPlayer;
	}

	bool UICommon::checkWalletParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer->m_wallet;
	}

	bool UICommon::checkRanchParam()
	{
		return checkPlayerParam() && Blockman::Instance() && Blockman::Instance()->m_pPlayer->m_ranch;
	}

	bool UICommon::checkBirdParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer->m_birdSimulator;
	}

	bool UICommon::checkBirdBagParam()
	{
		return checkBirdParam() && Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
	}

	i64 UICommon::getOtherCurrency()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BirdSimulator:
		{
			if (!checkBirdParam())
				return 0;
			return Blockman::Instance()->m_pPlayer->m_birdSimulator->getEggTicket();
		}
		default:
			return 0;
		}
	}

	String UICommon::getCurrencyIcon()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::RanchersExplore:
			return "set:ranchersexplore.json image:dianshu";
		case ClientGameType::BirdSimulator:
			return "set:bird_main.json image:fruit_currency";
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			return "set:gun_store.json image:gold";
		default:
			return "set:jail_break.json image:jail_break_currency";
		}
	}

	String UICommon::getOtherCurrencyIcon()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BirdSimulator:
			return "set:bird_main.json image:egg_ticket_mini";
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			return "set:gun_store.json image:chip";
		default:
			return "";
		}
	}

	String UICommon::getCurrencyIconByType(i32 currencyType)
	{
		return getCurrencyIconByType((CurrencyType) currencyType);
	}

	String UICommon::getCurrencyIconByType(CurrencyType currencyType)
	{
		switch (currencyType)
		{
		case CurrencyType::PLATFORM_MONEY:
			return isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods";
		case CurrencyType::PLATFORM_GOLD:
			return "set:app_shop.json image:app_shop_gold";
		case CurrencyType::GAME_MONEY:
			return getCurrencyIcon();
		case CurrencyType::OTHER_MONEY:
			return getOtherCurrencyIcon();
		default:
			return "";
		}
	}

	String UICommon::timeFormat(i32 time)
	{
		String h = StringUtil::ToString(time / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(time / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(time / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}

	bool UICommon::isMultiplayer()
	{
		return GameClient::CGame::Instance()->isMultiplayer();
	}
}
