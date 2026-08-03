#include "gui_birdPersonalShop.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "GUI/GuiDef.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiBirdPersonalShopItem.h"
#include "Network/ClientNetwork.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{

	gui_birdPersonalShop::gui_birdPersonalShop() :
		gui_layout("BirdPersonalShop.json")
	{
	}

	gui_birdPersonalShop::~gui_birdPersonalShop()
	{
	}

	void gui_birdPersonalShop::onShow()
	{
		if (m_isShopUpdate)
		{
			onShopUpdate();
			m_isShopUpdate = false;
		}
	}

	void gui_birdPersonalShop::onLoad()
	{
		getWindow("BirdPersonalShop-Title")->SetText(getString(LanguageKey::GUI_BIRD_PERSONAL_SHOP));
		getWindow("BirdPersonalShop-Info-Name")->SetText(getString(LanguageKey::GUI_BIRD_PERSONAL_SHOP_DETAILS_INFO));
		getWindow("BirdPersonalShop-Buy-Button")->SetText(getString(LanguageKey::GUI_BIRD_PERSONAL_SHOP_BUY));
		m_tabLayout = getWindow<GUILayout>("BirdPersonalShop-Tabs");
		m_itemListLayout = getWindow<GUILayout>("BirdPersonalShop-ItemList");
		m_image = getWindow<GUIStaticImage>("BirdPersonalShop-Image-Detail");
		m_des = getWindow<GUIStaticText>("BirdPersonalShop-Info-Detail");
		m_money = getWindow<GUIStaticText>("BirdPersonalShop-Money-Num");
		m_moneyIcon = getWindow<GUIStaticImage>("BirdPersonalShop-Money-Icon");

		getWindow<GUIButton>("BirdPersonalShop-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPersonalShop::onButtonClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("BirdPersonalShop-Buy-Button")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPersonalShop::onButtonClick, this, std::placeholders::_1, ViewId::BUY));

		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "BirdPersonalShop-ItemList-GridView");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0,0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(7.f, 6.f, 4);
		m_itemListLayout->AddChildWindow(m_itemGridView);


		m_sollBar = getWindow<GUISlider>("BirdPersonalShop-ListRollBar");
		m_sollBar->subscribeEvent(EventWindowTouchMove, std::bind(&gui_birdPersonalShop::onViewSoll, this, std::placeholders::_1, ViewId::SOLL_BAR));
		m_sollBar->subscribeEvent(EventWindowTouchDown, std::bind(&gui_birdPersonalShop::onViewSoll, this, std::placeholders::_1, ViewId::SOLL_BAR));
		m_itemGridView->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdPersonalShop::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));
		m_itemGridView->subscribeEvent(EventWindowTouchMove, std::bind(&gui_birdPersonalShop::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));
		m_itemGridView->subscribeEvent(EventWindowTouchDown, std::bind(&gui_birdPersonalShop::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));

		m_subscriptionGuard.add(UpdateBirdPersonalShopInfoEvent::subscribe(std::bind(&gui_birdPersonalShop::updateBirdPersonalShopInfo, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowBirdPersonalShopEvent::subscribe(std::bind(&gui_birdPersonalShop::showPersonalShop, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateBirdPersonalShopEvent::subscribe(std::bind(&gui_birdPersonalShop::updateBirdPersonalShop, this)));
	}

	void gui_birdPersonalShop::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_itemGridViewXSize != m_itemGridView->GetPixelSize().x)
			{
				m_itemGridViewXSize = m_itemGridView->GetPixelSize().x;
				sortItemSizeChange();
			}
		}
	}

	void gui_birdPersonalShop::onShopUpdate()
	{
		if (!UICommon::checkBirdParam())
		{
			return;
		}
		auto store = Blockman::Instance()->m_pPlayer->m_birdSimulator->getPersonalStore();
		int num = (int)m_tabLayout->GetChildCount();
		for (int i = 0; i < num; i++)
		{
			m_tabLayout->RemoveChildWindow(m_tabLayout->GetChildByIndex(0));
		}

		for (size_t i = 0; i < store.size(); i++)
		{
			addTabView(store[i], i);
		}
	}

	bool gui_birdPersonalShop::showPersonalShop(i32 tabId)
	{
		m_isShopUpdate = true;
		m_curGoodsTabId = tabId;
		RootGuiLayout::Instance()->showBirdPersonShop();
		return true;
	}


	void gui_birdPersonalShop::addTabView(BirdPersonalStoreTab group, int index)
	{
		static int count = 0;
		GUIString strTabName = StringUtil::Format("BirdPersonalShop-Tabs-Item-%d", count++).c_str();
		GUIString iconName = StringUtil::Format("BirdPersonalShop-Tabs-Item-Icon-%d", count).c_str();
		GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
		radioItem->SetArea(UDim(0, 0.f), UDim(0, index * 80.f + 16.f), UDim(0.f, 190.f), UDim(0, 72.f));
		radioItem->SetNormalImage("set:bird_personalShop.json image:radioButtonNor");
		radioItem->SetPushedImage("set:bird_personalShop.json image:radioButtonPre");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "15 15 15 15");
 
		radioItem->SetTextColor(Color(0.25098f, 0.945098f, 0.917647f));		
		radioItem->SetProperty(GWP_TEXT_BORDER, "true");
		radioItem->SetProperty(GWP_TEXT_BORDER_COLOR, "0.10961f 0.298039f 0.262745f 1.0f");
		
		radioItem->SetText(getString(group.tabName));
	    radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_birdPersonalShop::onRadioChange, this, std::placeholders::_1, group, radioItem));
 		m_tabLayout->AddChildWindow(radioItem);
		
		if (m_curGoodsTabId == -1 && index == 0 ||  group.tabId == m_curGoodsTabId)
		{
			radioItem->SetSelected(true);
		}
	}


	bool gui_birdPersonalShop::onRadioChange(const EventArgs & events, BirdPersonalStoreTab store, GUIRadioButton * radio)
	{
		if (radio->IsSelected())
		{
			float oldOffset = m_itemGridView->GetScrollOffset();
			m_itemGridView->RemoveAllItems();
			getWindow("BirdPersonalShop-Details")->SetVisible(store.items.size() > 0);
			static int goodsCount = 0;
			bool isDetailsChange = false;
			for (BirdGiftBag item : store.items)
			{
				GUIString strTabName = StringUtil::Format("BirdPersonalShop-ItemList-GridView-Item-%d", goodsCount++).c_str();
				GuiBirdPersonalShopItem* goodsItem = (GuiBirdPersonalShopItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PERSONAL_SHOP_ITEM, strTabName);
				float w = (m_itemGridView->GetPixelSize().x - 21) / 4;
				goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, w), UDim(0, 217.f));
				goodsItem->setGoodInfo(item, store.tabId);
				m_itemGridView->AddItem(goodsItem);
				if (store.tabId == m_curGoodsTabId && m_curGoodsId == item.id)
				{
					isDetailsChange = true;
					updateBirdPersonalShopInfo(item, store.tabId);
				}
			}
			m_itemGridView->ResetPos();
			m_itemGridView->SetScrollOffset(oldOffset);
			if (!isDetailsChange && store.items.size() > 0)
			{
				updateBirdPersonalShopInfo(store.items[0], store.tabId);
				m_itemGridView->SetScrollOffset(0);
			}
			onViewSoll(EventArgs(), ViewId::GRID_VIEW);
		}
		return true;
	}

	bool gui_birdPersonalShop::onButtonClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			playSoundByType(ST_ClosePanel);
			break;
		case ViewId::BUY:
			if (UICommon::checkBirdParam() && UICommon::checkMoney(m_curPrice, m_curCurrencyType))
			{
				ClientNetwork::Instance()->getSender()->sendBirdPersonalStoreBuyGoods(m_curGoodsTabId, m_curGoodsId);
			}			
			break;
		}
		
		return true;
	}

	

	bool gui_birdPersonalShop::updateBirdPersonalShopInfo(const BirdGiftBag& goods, i32 tabId)
	{
		m_curGoodsId = goods.id;
		m_curGoodsTabId = tabId;
		m_curPrice = goods.price;
		m_curCurrencyType = goods.currencyType;
		m_image->SetImage(goods.icon.c_str());
		m_des->SetText(getString(goods.desc));
		m_money->SetText(StringUtil::ToString(goods.price).c_str());
		m_moneyIcon->SetImage(UICommon::getCurrencyIconByType(goods.currencyType).c_str());
		setAlreadyBuy(goods.limit);
		return true;
	}

	bool gui_birdPersonalShop::updateBirdPersonalShop()
	{
		m_isShopUpdate = true;
		if (isShown())
		{
			onShow();
		}
		return false;
	}

	void gui_birdPersonalShop::setAlreadyBuy(i32 limit)
	{
		getWindow("BirdPersonalShop-Buy-Button")->SetEnabled(limit > 0);
		getWindow("BirdPersonalShop-Buy-Button")->SetText(getString(limit == 0 ?   LanguageKey::GUI_BIRD_PERSONAL_SHOP_ALREADY_BUY : LanguageKey::GUI_BIRD_PERSONAL_SHOP_BUY));
	}

	bool gui_birdPersonalShop::onViewSoll(const EventArgs &, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::GRID_VIEW:
		{
			float offset = m_itemGridView->GetScrollOffset();
			float height = m_itemGridView->GetContainerHeight();
			float progres = offset / (m_itemGridView->GetPixelSize().y - height);
			m_sollBar->SetProgress(progres);
		}
		break;
		case ViewId::SOLL_BAR:
		{
			float progres = m_sollBar->GetProgress();
			float height = m_itemGridView->GetContainerHeight();
			float offset = (m_itemGridView->GetPixelSize().y - height) * progres;
			m_itemGridView->SetScrollOffset(offset > 0 ? 0 : offset);
		}
		break;
		}
		return true;
	}

	void gui_birdPersonalShop::sortItemSizeChange()
	{
		for (size_t i = 0; i < m_itemGridView->GetItemCount(); i++)
		{
			float w = (m_itemGridView->GetPixelSize().x - 21) / 4;
			m_itemGridView->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, w), UDim(0, 217.f));
		}
	}

}
