#include "gui_shop.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GUIShopGoods.h"
#include "GuiDef.h"
#include "UI/GUIButton.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"
#include "cWorld/Blockman.h"
#include "Render/TextureAtlas.h"
#include "cItem/cItem.h"
#include "Inventory/CoinManager.h"
#include "Inventory/Wallet.h"
#include "UI/GUIStaticImage.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiItemStack.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_shop::gui_shop()
		: gui_layout("Shop.json")
	{

	}

	gui_shop::~gui_shop()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_shop::onLoad()
	{
		m_tabLayout = getWindow<GUILayout>("Shop-Content-Tabs");
		m_slotTable = getWindow<GuiSlotTable>("Shop-VisibleBar-SlotTable");
		m_currencyLayout = getWindow<GUILayout>("Shop-Currency");
		m_textTip = getWindow<GUIStaticText>("Shop-Content-Tip");
		getWindow<GUIStaticText>("Shop-Title-Name")->SetText(getString(LanguageKey::WORDART_SHOP_TILE_NAME));
		getWindow<GUIButton>("Shop-Btn-Close")->subscribeEvent(EventButtonClick,std::bind(&gui_shop::onClick, this, std::placeholders::_1));
		m_goodsGridView = getWindow<GuiShopGridView>("Shop-Content-Goods-List");
		m_goodsGridView->SetTouchable(false);
		m_goodsGridView->subscribeEvent("LackOfMoney", std::bind(&gui_shop::onLackOfMoney, this, std::placeholders::_1));
		m_subscriptionGuard.add(BuyCommodityResultEvent::subscribe(std::bind(&gui_shop::onBuyCommodityResult, this, std::placeholders::_1)));
		m_textTip->SetText("");

		m_shopItemTip = getWindow<GUILayout>("Shop-ItemTip");
		m_closeTipBtn = getWindow<GUIButton>("Shop-Item-Tip-button");
		m_goodsIcon = getWindow<GUIStaticImage>("Shop-Item-Icon");
		m_shopItemName = getWindow<GUIStaticText>("Shop-Item-Tip-Name");
		m_shopItemBuyLayout = getWindow<GUILayout>("Shop-Item-buy-btn");
		m_shopItemBuyIcon = getWindow<GUIStaticImage>("Shop-Item-buy-img");
		m_shopItemBuyCount = getWindow<GUIStaticText>("Shop-Item-buy-desc");
		m_shopItemTipDesc = getWindow<GUILayout>("Shop-Item-Tips-Desc");
		m_shopItemTip->SetVisible(false);
		m_closeTipBtn->SetEnabled(false);

		m_subscriptionGuard.add(ShowShopItemTipEvent::subscribe(std::bind(&gui_shop::onShowShopItemTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 , std::placeholders::_4 , std::placeholders::_5 , std::placeholders::_6 , std::placeholders::_7 )));


		m_closeTipBtn->subscribeEvent(EventButtonClick, std::bind(&gui_shop::onClickCloseShopItemTipBtn, this, std::placeholders::_1));
	}

	void gui_shop::onUpdate(ui32 nTimeElapse)
	{
		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		LordAssert(player);
		InventoryPlayer* inventory = player->inventory;
		LordAssert(inventory);

		if (isShown() && m_showMerchantPosition.distanceTo(player->position) > 7.f && GameClient::CGame::Instance()->GetGameType() != ClientGameType::Build_War)
		{
			getParent()->showMainControl();
			return;
		}

		for (int i = 0; i < 9; ++i)
		{
			m_slotTable->setItemStack(0, i, inventory->hotbar[i]);
		}
		m_slotTable->selectItemSlot(inventory->currentItemIndex);
		if (m_currencyLayout)
		{
			int i = 0;
			for (auto item = CoinManager::Instance()->m_coinMapping.begin(); item != CoinManager::Instance()->m_coinMapping.end(); item++)
			{
				findCurrencViewByCoinId(item->first, i);
				i++;
			}
		}

		if (m_showTipTime >0 && m_showTipTime <3000)
		{
			m_showTipTime = nTimeElapse + m_showTipTime;
		} 
		else
		{
			m_showTipTime = 0;
			m_tipMessage = "";
		}
		m_textTip->SetText(m_tipMessage.c_str());
	}

	bool gui_shop::onShopOpen(EntityMerchant * merchant)
	{
		m_currentSelectionTab = nullptr;
		m_merchantId = merchant->entityId;
		m_tabLayout->CleanupChildren();
		vector<CommodityGroup>::type groups = merchant->getCommodities();
		for (size_t i = 0; i < groups.size(); i++)
		{
			addTabView(groups[i], i);
		}
		if (groups.size() == size_t(0))
		{
			clearGridView();
		}
		m_showMerchantPosition = merchant->position;
		m_shopItemTip->SetVisible(false);
		m_closeTipBtn->SetEnabled(false);
		return true;
	}

	bool gui_shop::onLackOfMoney(const EventArgs & events)
	{
		m_showTipTime = 1;
		switch (events.handled)
		{
		case  (int)BuyResult::HasNoGun:
			m_tipMessage = getString(LanguageKey::HAS_NO_GUN).c_str();
			break;
		case (int)BuyResult::LackOfMoneyGoAppShop:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			ShowGoAppShopTipDialogEvent::emit();
			break;
		case (int)BuyResult::FullInventory:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str();
			break;
		case (int)BuyResult::LackOfItem:
			m_tipMessage = getString("gui_shop_lack_of_item").c_str();
			break;
		default:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			break;
		}
		m_textTip->SetText(m_tipMessage.c_str());
		return true;
	}

	bool gui_shop::onBuyCommodityResult(const String & msg)
	{
		m_showTipTime = 1;
		m_tipMessage = msg;
		m_textTip->SetText(msg.c_str());
		return true;
	}

	void gui_shop::onUpdateUnlockedState()
	{
		m_goodsGridView->updateUnlockedState();
	}

	void gui_shop::addTabView(CommodityGroup group, int index)
	{
		static unsigned count = 0;
		count++;
		String strTabName = StringUtil::Format("Shop-Content-Tabs-Item-%d", count).c_str();
		String iconName = StringUtil::Format("Shop-Content-Tabs-Item-Icon-%d", count).c_str();
		if (GUIWindowManager::Instance()->GetGUIWindow(strTabName.c_str()))
		{
			return;
		}

		GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
		radioItem->SetArea(UDim(0, 0), UDim(0, index * 70.0f), UDim(1, 0), UDim(0, 64));
		radioItem->SetNormalImage("set:gui_shop.json image:shop_tab_nor");
		radioItem->SetPushedImage("set:gui_shop.json image:shop_tab_pre");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "15 15 15 15");

		GUIStaticImage* iconItem = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
		iconItem->SetHorizontalAlignment(HA_LEFT);
		iconItem->SetVerticalAlignment(VA_CENTRE);
		iconItem->SetArea(UDim(0, 30.0f), UDim(0, 0.0f), UDim(0, 30.f), UDim(0, 30.f));
		GUIString tabName = "";
		String imageRes = "";
		switch (group.getCategory())
		{
		case CommodityCategory::TOOL:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_TOOL);
			imageRes = "set:shop_tab_icon.json image:Tool";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::ARMOR:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_ARMOR);
			imageRes = "set:shop_tab_icon.json image:Armor";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::ARMS:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_ARMS);
			imageRes = "set:shop_tab_icon.json image:Weapon";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::BLOCK:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_BLOCK);
			imageRes = "set:shop_tab_icon.json image:Block";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::FOOD:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_FOOD);
			imageRes = "set:shop_tab_icon.json image:Food";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::UPGRADE:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_UPGRADE);
			imageRes = "set:shop_tab_icon.json image:Upgrade";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::SKILLBOOK:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_SKILL_BOOK);
			imageRes = "set:other_icon.json image:skill_book_icon";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::SKILLBOOKFRAGMENTATION:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_SKILL_BOOK_FRAGMENTATION);
			imageRes = "set:other_icon.json image:skill_book_fragmentation_icon";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::SELL:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_SELL);
			imageRes = "set:shop_tab_icon.json image:Other";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::COLOR:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_COLOR);
			imageRes = "set:shop_tab_icon.json image:Block";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::WOOD:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_WOOD);
			imageRes = "set:shop_tab_icon.json image:Block";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::STONE:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_STONE);
			imageRes = "set:shop_tab_icon.json image:Block";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::METAL:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_METAL);
			imageRes = "set:shop_tab_icon.json image:Block";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::SUIT:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_SUIT);
			imageRes = "set:shop_tab_icon.json image:Armor";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::POTION:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_POTION);
			imageRes = "set:other_icon.json image:icon_potion";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		case CommodityCategory::OTHERS:
		default:
			tabName = getString(LanguageKey::GUI_SHOP_TAB_OTHERS);
			imageRes = "set:shop_tab_icon.json image:Other";
			radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "20 0");
			break;
		}

		//radioItem->SetTextColor(Color(0.921569f, 0.807843f, 0.643137f));
		radioItem->SetProperty(GWP_TEXT_BORDER, "true");
		radioItem->SetText(tabName);
		radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_shop::onRadioChange, this, std::placeholders::_1, group, radioItem));
		iconItem->SetImage(imageRes.c_str());
		radioItem->AddChildWindow(iconItem);
		m_tabLayout->AddChildWindow(radioItem);

		if (index == 0)
		{
			radioItem->SetSelected(true);
		}
	}

	bool gui_shop::onRadioChange(const EventArgs & events, const CommodityGroup group, GUIRadioButton* radio)
	{
		if (m_currentSelectionTab != radio)
		{
			updateGridView(group);
		}
		
		if (m_currentSelectionTab && m_currentSelectionTab->GetChildCount() > 0)
		{
			m_currentSelectionTab->GetChildByIndex(0)->SetArea(UDim(0, 30.0f), UDim(0, 0.0f), UDim(0, 30.f), UDim(0, 30.f));
		}
		m_currentSelectionTab = radio;
		float width = radio->GetPixelSize().x;
		float left = width - 150.0f > 30.0f ? (width - 150.0f > 45.0f ? 45.0f : width - 150.0f) : 30.0f;
		radio->GetChildByIndex(0)->SetArea(UDim(0, left), UDim(0, 0.0f), UDim(0, 30.f), UDim(0, 30.f));
		return true;
	}

	bool gui_shop::onClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		LordLogInfo("====gui_shop onClick Close Btn================");
		return true;
	}

	void gui_shop::updateGridView(CommodityGroup group)
	{
		m_goodsGridView->updateGridView(group, m_merchantId);
	}

	void gui_shop::clearGridView()
	{
		m_goodsGridView->clearGridView();
	}

	GUILayout * gui_shop::findCurrencViewByCoinId(int coinId, int index)
	{
		if (m_currencyMap[coinId] == nullptr)
		{
			GUIString strName = StringUtil::Format("Shop-Currency-Item-%d", coinId).c_str();
			GUIString strIconName = StringUtil::Format("Shop-Currency-Item-Icon-%d", coinId).c_str();
			GUIString strValueName = StringUtil::Format("Shop-Currency-Item-Value-%d", coinId).c_str();
			GUILayout* currencyItem = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, strName);
			currencyItem->SetArea(UDim(0, index * 120.f + 10.f), UDim(0, 0), UDim(0, 120.f), UDim(1.f, 0));

			GUIStaticImage* currencyIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strIconName);
			currencyIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 20), UDim(0, 20));
			currencyIcon->SetVerticalAlignment(VA_CENTRE);
			int coinItemId = CoinManager::Instance()->coinIdToItemId(coinId);
			ItemStackPtr coinItemStack = LORD::make_shared<ItemStack>(coinItemId, 1, 1);
			GuiItemStack::showItemIcon(currencyIcon, coinItemStack, coinItemId, 1);
			currencyItem->AddChildWindow(currencyIcon);

			GUIStaticText* currencyValue = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strValueName);
			currencyValue->SetArea(UDim(0, 30.f), UDim(0, 0), UDim(0, 90.f), UDim(1.0f, 0));
			currencyValue->SetVerticalAlignment(VA_CENTRE);
			currencyValue->SetTextVertAlign(VA_CENTRE);
			EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
			if (player && player->m_wallet)
			{
				int num =  player->m_wallet->getCoinNum(coinId);
				currencyValue->SetText(StringUtil::ToString(num).c_str());
			}
			else
			{
				currencyValue->SetText("0");
			}
			currencyItem->AddChildWindow(currencyValue);
			m_currencyMap[coinId] = currencyItem;
			m_currencyLayout->AddChildWindow(currencyItem);
		}

		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		if (player && player->m_wallet)
		{
			int itemId = CoinManager::Instance()->m_coinMapping[coinId];
			i64 num = 0;
			if (itemId >= 10000)
				num = player->m_wallet->getCurrency();
			else
				num = player->m_wallet->getCoinNum(coinId);
			m_currencyMap[coinId]->GetChildByIndex(1)->SetText(StringUtil::ToString(num).c_str());
		}
		else
		{
			m_currencyMap[coinId]->GetChildByIndex(1)->SetText("0");
		}
		return m_currencyMap[coinId];
	}

	bool gui_shop::onShowShopItemTip(int itemId, int damage, const String& image, int coinItemId, const String& tipDesc, const String& name, int price)
	{
		ItemStackPtr coinItemStack = LORD::make_shared<ItemStack>(coinItemId, 1, 1);
		GuiItemStack::showItemIcon(m_shopItemBuyIcon, coinItemStack, coinItemId, 1);

		ItemStackPtr itemStack = LORD::make_shared<ItemStack>(itemId, 1, damage);
		GuiItemStack::showItemIcon(m_goodsIcon, itemStack, itemId, damage, image);

		m_shopItemName->SetText(name.c_str());
		m_shopItemBuyCount->SetText(StringUtil::ToString(price).c_str());

		float width = m_shopItemBuyLayout->GetPixelSize().x;
		int digits = 0;
		while (price > 10)
		{
			digits++;
			price /= 10;
		}
		float left = (width - (75.0f + 12 * digits)) / 2;
		m_shopItemBuyIcon->SetXPosition(UDim(0, left));
		m_shopItemBuyCount->SetXPosition(UDim(0, 45.0f + left));

		m_shopItemBuyIcon->SetVisible(price > 0);
		m_shopItemBuyCount->SetVisible(price > 0);

		m_shopItemTipDesc->CleanupChildren();
		StringArray attrs = StringUtil::Split(tipDesc, "&");
		int index = 0;
		for (String attr : attrs)
		{
			index += 1;
			GUIStaticText* stAttr = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, StringUtil::Format("ShopItem-Tip-Desc-%d", index).c_str());
			stAttr->SetWidth({ 1, 0 });
			stAttr->SetHeight({ (1.0f / attrs.size()), 0 });
			stAttr->SetXPosition(UDim(0, 0));
			stAttr->SetYPosition(UDim((index - 1) * (1.0f /attrs.size()),0));
			stAttr->SetTextBoader(LORD::Color::BLACK);
			stAttr->SetWordWrap(true);
			if (StringUtil::Find(attr, "="))
			{
				StringArray kvPair = StringUtil::Split(attr, "=");
				stAttr->SetText(StringUtil::Format(getString(kvPair[0]).c_str(), kvPair[1].c_str()).c_str());
			}
			else
			{
				stAttr->SetText(getString(attr).c_str());
			}

			m_shopItemTipDesc->AddChildWindow(stAttr);
		}

		m_shopItemTip->SetVisible(true);
		m_closeTipBtn->SetEnabled(true);
		m_closeTipBtn->SetTouchable(true);
		return true;
	}

	bool gui_shop::onClickCloseShopItemTipBtn(const EventArgs &)
	{
		m_shopItemTip->SetVisible(false);
		m_closeTipBtn->SetEnabled(false);
		m_closeTipBtn->SetTouchable(false);
		return true;
	}
}
