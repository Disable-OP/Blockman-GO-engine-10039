#include "gui_HideAndSeekHallResult.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "UI/GUISystem.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_hideAndSeekHallResult::gui_hideAndSeekHallResult()
		: gui_layout("HAS2HallResult.json")
	{
	}

	gui_hideAndSeekHallResult::~gui_hideAndSeekHallResult()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_hideAndSeekHallResult::onLoad()
	{
		m_subscriptionGuard.add(HideAndSeekHallResult::subscribe(std::bind(&gui_hideAndSeekHallResult::showItemInfo, this, std::placeholders::_1)));

		getWindow("HAS2HallResult-Btn-Sure")->SetText(getString(LanguageKey::GUI_HIDEANDSEEKHALLRESULT_SURE));
		getWindow("HAS2HallResult-AgainText")->SetText(getString(LanguageKey::GUI_HIDEANDSEEKHALLRESULT_AGAIN));

		getWindow<GUIButton>("HAS2HallResult-Btn-Again")->subscribeEvent(EventButtonClick, std::bind(&gui_hideAndSeekHallResult::onBtnAgain, this, std::placeholders::_1));
		getWindow<GUIButton>("HAS2HallResult-Btn-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_hideAndSeekHallResult::onBtnSure, this, std::placeholders::_1));

		for (int i = 0; i < MAX_ITEM_NUM; ++i)
		{
			m_itemBg[i] = getWindow<GUIStaticImage>(StringUtil::Format("HAS2HallResult-Item%d", i + 1).c_str());
			m_itemImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("HAS2HallResult-ItemImg%d", i + 1).c_str());
			m_moneyImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("ResultMoney-Image%d", i + 1).c_str());
			m_newImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("HallResultNew-img%d", i + 1).c_str());
			m_frameImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("HallResultframe-img%d", i + 1).c_str());
			m_itemBg[i]->SetVisible(false);
			m_itemImg[i]->SetVisible(false);
			m_moneyImg[i]->SetVisible(false);
			m_newImg[i]->SetVisible(false);
			m_frameImg[i]->SetVisible(false);
		}

		m_againIcon = getWindow<GUIStaticImage>("HAS2HallResult-AgainIcon");
		m_againMoney = getWindow<GUIStaticText>("HAS2HallResult-AgainMoney");
		m_rusultMoneyText = getWindow<GUIStaticText>("HallResult-ResultAllMoney");
	}

	void gui_hideAndSeekHallResult::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{

		}
	}

	void gui_hideAndSeekHallResult::onShow()
	{

	}

	bool gui_hideAndSeekHallResult::onBtnSure(const EventArgs)
	{
		getParent()->showMainControl();
		return false;
	}

	bool gui_hideAndSeekHallResult::onBtnAgain(const EventArgs)
	{
		getParent()->showMainControl();
		Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		if (wallet)
		{
			i64 money = wallet->getMoneyByCoinId(m_coinId);
			if (money < m_need_money)
				getParent()->showLackOfMoney(m_need_money, (CurrencyType)m_coinId);
			else
				ClientNetwork::Instance()->getSender()->sendHallLotteryAgain(m_luck_times, m_luck_entityId);
		}
		return false;
	}

	bool gui_hideAndSeekHallResult::getItemInfo(const String & info, std::vector<ItemInfo>& itemInfoList, String & moneyImg, int & money, int & times, int & entityId, int & coinId,int & rusultMoney)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_hideAndSeekHallResult::getItemInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("itemList") || !doc->HasMember("money") || !doc->HasMember("moneyImg")  || !doc->HasMember("times") || !doc->HasMember("entityId") || !doc->HasMember("coinId") || !doc->HasMember("rusultMoney"))
		{
			LordLogError("gui_hideAndSeekHallResult::getItemInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("itemList")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("itemId") || !arr[i].HasMember("itemImg") || !arr[i].HasMember("itembgImg") || !arr[i].HasMember("isHave"))
			{
				LordLogError("gui_hideAndSeekHallResult::getItemInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			String itemImg = arr[i].FindMember("itemImg")->value.GetString();
			String itembgImg = arr[i].FindMember("itembgImg")->value.GetString();
			int itemId = arr[i].FindMember("itemId")->value.GetInt();
			bool isHave = arr[i].FindMember("isHave")->value.GetBool();
			ItemInfo itemInfo;
			itemInfo.itemId = itemId;
			itemInfo.itemImg = itemImg;
			itemInfo.itembgImg = itembgImg;
			itemInfo.isHave = isHave;

			itemInfoList.push_back(itemInfo);
		}

		moneyImg = doc->FindMember("moneyImg")->value.GetString();
		money = doc->FindMember("money")->value.GetInt();
		times = doc->FindMember("times")->value.GetInt();
		entityId = doc->FindMember("entityId")->value.GetInt();
		coinId = doc->FindMember("coinId")->value.GetInt();
		rusultMoney = doc->FindMember("rusultMoney")->value.GetInt();
		
		LordDelete(doc);
		return true;
	}

	bool gui_hideAndSeekHallResult::showItemInfo(const String & info)
	{
		std::vector<ItemInfo> itemInfo;
		String moneyImg = "";
		int money = 0;
		int luck_times = -1;
		int entityId = -1;
		int coinId = 0;
		int rusultMoney = 0;
		if (getItemInfo(info, itemInfo, moneyImg, money, luck_times, entityId, coinId, rusultMoney))
		{
			m_need_money = money;
			m_luck_times = luck_times;
			m_luck_entityId = entityId;
			m_coinId = coinId;
			m_rusultMoney = rusultMoney;
			m_rusultMoneyText->SetText(StringUtil::ToString(m_rusultMoney).c_str());
			if (m_luck_times == 1)
				m_itemBg[0]->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 80.f), UDim(0, 80.f));
			else
				m_itemBg[0]->SetArea(UDim(-0.2f, 0), UDim(-0.2f, 0), UDim(0, 80.f), UDim(0, 80.f));


			for (int i = 0; i < MAX_ITEM_NUM; ++i)
			{
				m_itemBg[i]->SetVisible(false);
			}

			for (int i = 0; i < (int)itemInfo.size() && i < MAX_ITEM_NUM; ++i)
			{

				m_frameImg[i]->SetVisible(!itemInfo[i].isHave);
				m_newImg[i]->SetVisible(!itemInfo[i].isHave);
				m_moneyImg[i]->SetVisible(itemInfo[i].isHave);

				
				m_itemBg[i]->SetVisible(true);
				m_itemBg[i]->SetImage(itemInfo[i].itembgImg.c_str());
				m_itemImg[i]->SetVisible(true);
				m_itemImg[i]->SetImage(itemInfo[i].itemImg.c_str());
				m_againIcon->SetImage(moneyImg.c_str());
				m_againMoney->SetText(StringUtil::ToString(money).c_str());
			}
		}
		return true;
	}
}
