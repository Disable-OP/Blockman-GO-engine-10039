#include "gui_ranchHousePanel.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Ranch.h"
#include "Inventory/Wallet.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "GUI/GuiDef.h"
#include "Util/RedDotManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchHousePanel::gui_ranchHousePanel()
		: gui_layout("RanchHousePanel.json")
	{
	}

	gui_ranchHousePanel::~gui_ranchHousePanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchHousePanel::onLoad()
	{
		m_tvNowHouseName = getWindow<GUIStaticText>("RanchHousePanel-NowHouse-Name");
		m_ivNowHouseIcon = getWindow<GUIStaticImage>("RanchHousePanel-NowHouse-Icon");

		m_tvNextHouseName = getWindow<GUIStaticText>("RanchHousePanel-NextHouse-Name");
		m_ivNextHouseIcon = getWindow<GUIStaticImage>("RanchHousePanel-NextHouse-Icon");

		m_tvPrice = getWindow<GUIStaticText>("RanchHousePanel-Content-Price");
		m_tvNeedLevel = getWindow<GUIStaticText>("RanchHousePanel-Content-NeedLevel-Value");

		m_ranchItemInfo = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, "RanchHousePanel-Recipe-Item-Info");
		m_ranchItemInfo->changeInfoUi();
		m_ranchItemInfo->SetAlwaysOnTop(true);
		m_ranchItemInfo->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 206.f), UDim(0.0f, 130.f));
		getWindow("RanchHousePanel-Content")->AddChildWindow(m_ranchItemInfo);
		m_ranchItemInfo->SetVisible(false);

		m_panelItemNeed1 = getWindow<GuiRanchCommonItem>("RanchHousePanel-Content-Need-1");
		m_panelItemNeed2 = getWindow<GuiRanchCommonItem>("RanchHousePanel-Content-Need-2");
		m_panelItemNeed3 = getWindow<GuiRanchCommonItem>("RanchHousePanel-Content-Need-3");

		m_panelItemVector.push_back(m_panelItemNeed1);
		m_panelItemVector.push_back(m_panelItemNeed2);
		m_panelItemVector.push_back(m_panelItemNeed3);

		GUIButton* btnSure = getWindow<GUIButton>("RanchHousePanel-Content-Btn-Sure");
		btnSure->SetText(getString(LanguageKey::GUI_RANCH_HOUSE_UPGRADE));
		btnSure->subscribeEvent(EventButtonClick, std::bind(&gui_ranchHousePanel::onClick, this, std::placeholders::_1, ViewId::SURE));
		m_rootWindow->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchHousePanel::onClick, this, std::placeholders::_1, ViewId::MAIN));
		m_subscriptionGuard.add(RanchHouseChangeEvent::subscribe(std::bind(&gui_ranchHousePanel::onRanchHouseChange, this)));
		m_subscriptionGuard.add(RanchItemClickEvent::subscribe(std::bind(&gui_ranchHousePanel::onItemClick, this, std::placeholders::_1, std::placeholders::_2)));

	}

	void gui_ranchHousePanel::onHide()
	{
		if (m_ranchItemInfo)
		{
			m_ranchItemInfo->SetVisible(false);
		}
	}

	void gui_ranchHousePanel::onShow()
	{
		if (m_ranchItemInfo)
		{
			m_ranchItemInfo->SetVisible(false);
		}
	}

	void gui_ranchHousePanel::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			m_leftTime -= nTimeElapse;
			m_leftTime = (m_leftTime > 0 ? m_leftTime : 0);
			getWindow("RanchHousePanel-RedDot")->SetVisible(RedDotManager::checkHouse());
			getWindow<GUIButton>("RanchHousePanel-Content-Btn-Sure")->SetEnabled(m_leftTime == 0);
		}
	}

	bool gui_ranchHousePanel::onClick(const EventArgs & events, ViewId viewId)
	{
		m_ranchItemInfo->SetVisible(false);
		switch (viewId)
		{
		case ViewId::SURE:
			if (Blockman::Instance()->m_pPlayer)
			{
				auto wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				Ranch* ranch = Blockman::Instance()->m_pPlayer->m_ranch;
				if (wallet && ranch && ranch->getHouse())
				{

					if (!UICommon::checkRanchItems(ranch->getHouse()->upgradeRecipe))
					{
						m_leftTime = 1500;
						return false;
					}

					if (!UICommon::checkRanchLevel(ranch->getHouse()->needPlayerLevel))
					{
						return false;
					}

					i32 price = ranch->getHouse()->upgradePrice;
					if (!UICommon::checkMoney(price, CurrencyType::GAME_MONEY))
					{
						return false;
					}
					ClientNetwork::Instance()->getSender()->sendRanchOperation(RanchOperationType::HOUSE_UPGRADE);
				}
			}
			break;
		}

		return true;
	}

	bool gui_ranchHousePanel::onItemClick(i32 itemId, const String & name)
	{
		if (!isShown())
		{
			return false;
		}

		m_ranchItemInfo->SetVisible(true);
		m_ranchItemInfo->setItemId(itemId);

		String indexStr = StringUtil::Replace(name.c_str(), "RanchHousePanel-Content-Need-", "");
		i32 index = atoi(indexStr.c_str()) - 1;
		m_ranchItemInfo->SetPosition(UVector2(UDim(0, -103.f + 61.f), UDim(0, -110.f + 95.f * index)));
		return true;
	}

	bool gui_ranchHousePanel::onRanchHouseChange()
	{
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getHouse())
		{
			RanchHouse* house = Blockman::Instance()->m_pPlayer->m_ranch->getHouse();

			m_tvNeedLevel->SetText(StringUtil::ToString(house->needPlayerLevel).c_str());
			setText("RanchHousePanel-Content-Btn-Sure", house->level >= house->maxLevel ? "gui_ranch_max_level" : LanguageKey::GUI_RANCH_HOUSE_UPGRADE);
			setText("RanchHousePanel-Content-MaxLevel", "gui_ranch_house_max_level");
			getWindow("RanchHousePanel-Content-NeedLevel")->SetVisible(house->level < house->maxLevel);
			getWindow("RanchHousePanel-Content-Currency")->SetVisible(house->level < house->maxLevel);
			getWindow("RanchHousePanel-Content-Btn-Sure")->SetVisible(house->level < house->maxLevel);
			getWindow("RanchHousePanel-Content-MaxLevel")->SetVisible(house->level >= house->maxLevel);

			m_tvNowHouseName->SetText(getString(house->name));
			m_ivNowHouseIcon->SetImage(house->icon.c_str());

			m_tvNextHouseName->SetText(getString(house->nextName));
			m_ivNextHouseIcon->SetImage(house->nextIcon.c_str());

			m_tvPrice->SetText(StringUtil::ToString(house->upgradePrice).c_str());

			for (auto item : house->upgradeRecipe)
			{
				for (auto view : m_panelItemVector)
				{
					if (item.itemId == view->getItemId() || view->getItemId() == 0)
					{
						view->setItemRecipe(item);
						break;
					}
				}
			}
			for (auto view : m_panelItemVector)
			{
				view->SetVisible(view->getItemId() != 0 && house->level < house->maxLevel);
			}
		}
		
		return true;
	}
}
