#include "gui_ranchUpgrade.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiRanchCommonItem.h"
#include "GUI/GuiDef.h"
#include "GUI/GuiRanchItem.h"
#include "Network/ClientNetwork.h"
#include "cWorld/Blockman.h"
#include "World/Ranch.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/UICommon.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchUpgrade::gui_ranchUpgrade()
		: gui_layout("RanchUpgrade.json")
	{
	}

	gui_ranchUpgrade::~gui_ranchUpgrade()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchUpgrade::onLoad()
	{
		m_rewardLayout = getWindow<GUILayout>("RanchUpgrade-Reward");
		m_currencyLayout = getWindow<GUILayout>("RanchUpgrade-Currency");
		m_recipeListView = getWindow<GUIListHorizontalBox>("RanchUpgrade-Recipe");
		m_rewardListView = getWindow<GUIListHorizontalBox>("RanchUpgrade-Reward-Items");

		m_recipeListView->SetInterval(2.f);
		m_rewardListView->SetInterval(2.f);

		m_btnSure = getWindow<GUIButton>("RanchUpgrade-Btn-Sure");
		m_tvTitle = getWindow<GUIStaticText>("RanchUpgrade-Title-Name");
		m_tvMsg = getWindow<GUIStaticText>("RanchUpgrade-Content-Message");

		setText(m_tvMsg, "gui_ranch_expand_reward");
		m_rootWindow->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchUpgrade::onClick, this, std::placeholders::_1,ViewId::MAIN));
		getWindow<GUIButton>("RanchUpgrade-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchUpgrade::onClick, this, std::placeholders::_1,ViewId::CLOSE));
		getWindow<GUIButton>("RanchUpgrade-Btn-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchUpgrade::onClick, this, std::placeholders::_1, ViewId::SURE));
		m_subscriptionGuard.add(ShowRanchExpandTipEvent::subscribe(std::bind(&gui_ranchUpgrade::showExpand, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchStorageUpgradeTipEvent::subscribe(std::bind(&gui_ranchUpgrade::showStorageUpgrade, this)));
		m_subscriptionGuard.add(RanchItemClickEvent::subscribe(std::bind(&gui_ranchUpgrade::onItemClick, this, std::placeholders::_1, std::placeholders::_2)));

		m_ranchItemInfo =  (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, "RanchUpgrade-Recipe-Item-Info");
		m_ranchItemInfo->changeInfoUi();
		m_ranchItemInfo->SetAlwaysOnTop(true);
		m_ranchItemInfo->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 206.f), UDim(0.0f, 130.f));
		getWindow("RanchUpgrade-Content")->AddChildWindow(m_ranchItemInfo);
		
		m_ranchItemInfo->SetVisible(false);
	}

	void gui_ranchUpgrade::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			m_ranchItemInfo->SetVisible(false);
		}
		m_leftTime = m_leftTime - nTimeElapse;
		m_leftTime = (m_leftTime > 0 ? m_leftTime : 0);
		m_btnSure->SetEnabled(m_leftTime == 0);
	}

	bool gui_ranchUpgrade::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId) {
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->hideRanchUpgrade();
			break;
		case ViewId::SURE:
			sureClick();
			break;
		}
		m_ranchItemInfo->SetVisible(false);
		return true;
	}

	void gui_ranchUpgrade::sureClick()
	{
		switch (m_tipType)
		{
		case TipType::EXPAND:

			if (!UICommon::checkRanchItems(m_recipe))
			{
				m_leftTime = 1500;
				return;
			}

			if (!UICommon::checkMoney(m_price, CurrencyType::GAME_MONEY))
			{
				return;
			}

			if (m_landCode > 0)
			{
				ClientNetwork::Instance()->getSender()->sendRanchExpand(m_landCode);
			}
			
			RootGuiLayout::Instance()->hideRanchUpgrade();
			break;
		case TipType::STORAGE_UPGRADE:

			if (!UICommon::checkMoney(m_price, CurrencyType::GAME_MONEY))
			{
				return;
			}

			auto newItems = UICommon::getLackRanchItems(m_recipe);
			if (newItems.size() > 0)
			{
				ShowRanchLockItemsByUpgradeStorageEvent::emit(newItems);
				RootGuiLayout::Instance()->hideRanchUpgrade();
				return;
			}

			ClientNetwork::Instance()->getSender()->sendRanchOperation(RanchOperationType::STORAGE_UPGRADE);
			RootGuiLayout::Instance()->hideRanchUpgrade();
			break;
		}
	}

	bool gui_ranchUpgrade::showExpand(i32 landCode, i32 price, const vector<RanchCommon>::type & recipe, const vector<RanchCommon>::type & reward)
	{
		m_recipe = recipe;
		m_price = price;
		m_tipType = TipType::EXPAND;
		RootGuiLayout::Instance()->showRanchUpgrade();
		m_landCode = landCode;
		initRecipeItem(recipe);
		initRewardItem(reward);
		m_rewardLayout->SetVisible(reward.size() > 0);
		m_currencyLayout->SetVisible(false);
		setText(m_btnSure, "gui_ranch_btn_expand");
		setText(m_tvTitle, "gui_ranch_manor_expand_title");
		setText(m_tvMsg, "gui_ranch_extend_do_not_need_material");
		m_tvMsg->SetVisible(m_recipe.size() == 0);
		return true;
	}

	bool gui_ranchUpgrade::showStorageUpgrade()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		m_recipe.clear();
		for (auto item : storage->upgradeRecipe)
		{
			m_recipe.push_back(item);
		}
		m_tipType = TipType::STORAGE_UPGRADE;
		RootGuiLayout::Instance()->showRanchUpgrade();
		initRecipeItem(m_recipe);
		m_price = storage->upgradePrice;
		m_rewardLayout->SetVisible(false);
		m_currencyLayout->SetVisible(true);
		m_btnSure->SetText(StringUtil::Format(getString("gui_ranch_augment_storage").c_str(), storage->nextCapacity).c_str());
		m_currencyLayout->GetChildByIndex(1)->SetText(StringUtil::ToString(storage->upgradePrice).c_str());
		setText(m_tvTitle, "gui_ranch_storage_expand_title");
		setText(m_tvMsg, "gui_ranch_upgrade_do_not_need_material");
		m_tvMsg->SetVisible(m_recipe.size() == 0);
		return true;
	}

	bool gui_ranchUpgrade::onItemClick(i32 itemId, const String & name)
	{
		if (!isShown() || !m_recipeListView->getContainerWindow())
		{
			return false;
		}
		i32 index = m_recipeListView->getContainerWindow()->GetChildIndex(name.c_str());
		if (index >= 0)
		{
			GUIWindow* view = m_recipeListView->getContainerWindow()->GetChildByIndex(index);
			if (view)
			{
				m_ranchItemInfo->SetVisible(true);
				m_ranchItemInfo->setItemId(itemId);
				UVector2 uv = view->GetPosition();
				UVector2 newUv = UVector2(UDim(0, uv.d_x.d_offset - 46.f -  15.f + 82.5f ), UDim(0, uv.d_y.d_offset + 92.f - 130.f + 10.f));
				m_ranchItemInfo->SetPosition(newUv);
			}
		}
		return true;
	}

	void gui_ranchUpgrade::initRecipeItem(const vector<RanchCommon>::type& items)
	{
		m_recipeListView->ClearAllItem();
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchUpgrade-Recipe-List-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchCommonItem* ranchCommonItem = (GuiRanchCommonItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_COMMON_ITEM, itemLayout.c_str());
			ranchCommonItem->resetWindow("RanchUpgradeItem.json");
			ranchCommonItem->setItemRecipe(item);
			ranchCommonItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 165.f), UDim(0.0f, 216.f));
			m_recipeListView->AddItem(ranchCommonItem, false);
		}
		float containerWidth = m_recipeListView->getContainerWindow() ? m_recipeListView->getContainerWindow()->GetPixelSize().x : 0.f;
		containerWidth = (containerWidth > 495.f ? containerWidth : 495.f) + 92.f;
		getWindow("RanchUpgrade-Content")->SetWidth(UDim(0, containerWidth));
		m_recipeListView->setContainerAlignment();
	}

	void gui_ranchUpgrade::initRewardItem(const vector<RanchCommon>::type & items)
	{
		m_rewardListView->ClearAllItem();
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchUpgrade-Reward-List-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchItem* ranchItemView = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, itemLayout.c_str());
			ranchItemView->resetWindow("RanchRewardItem.json");
			ranchItemView->setNum(StringUtil::Format("+%d", item.num).c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 108.f), UDim(0.0f, 54.f));
			ranchItemView->setItemId(item.itemId);
			m_rewardListView->AddItem(ranchItemView, false);
		}
		m_rewardListView->setContainerAlignment();
	}
}
