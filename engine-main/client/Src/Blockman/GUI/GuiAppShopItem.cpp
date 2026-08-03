#include "GuiAppShopItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"
#include "cItem/cItem.h"
#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Inventory/InventoryPlayer.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiAppShopItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GuiAppShopItem::GuiAppShopItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("AppShopItem.json"));
		renameComponents(m_window);
		m_itemName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_quota = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_quotaValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_btnBuy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_currencyIcon = dynamic_cast<GUIStaticImage*>(m_btnBuy->GetChildByIndex(0));
		m_itemNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&GuiAppShopItem::onButtonClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiAppShopItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		String name = LanguageManager::Instance()->getString(m_goods.desc);
		m_itemName->SetText(name.c_str());
		m_itemNum->SetText(StringUtil::ToString(m_goods.itemNum).c_str());
		m_btnBuy->SetText(StringUtil::ToString(GameClient::CGame::Instance()->isMultiplayer() ? m_goods.blockmanPrice : m_goods.blockymodsPrice).c_str());
		switch (m_goods.coinId)
		{
		case 1:
			m_currencyIcon->SetImage(GameClient::CGame::Instance()->isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods");
			break;
		case 2:
			m_currencyIcon->SetImage("set:app_shop.json image:app_shop_gold");
			break;
		case 3:
			m_currencyIcon->SetImage(UICommon::getCurrencyIcon().c_str());
			break;
		}
		
		if (m_goods.limit == -1)
		{
			m_quota->SetVisible(false);
		}
		else
		{
			m_quota->SetVisible(true);
			String num = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_STR_APP_SHOP_LIMIT).c_str(), m_goods.limit);
			m_quotaValue->SetText(num.c_str());
		}
		updateIcon(m_goods.itemId, m_goods.itemMeta, m_goods.icon);
	}

	void GuiAppShopItem::updateIcon(int itemId, int damage, String icon)
	{
		if (itemId == m_oldItemId && damage == m_oldItemDamage)
		{
			return;
		}
		m_oldItemId = itemId;
		m_oldItemDamage = damage;

		ItemStack* itemStack = LordNew ItemStack(itemId, 1, damage);
		ItemStackPtr m_itemStack = itemStack->copy();
		if(m_itemIcon)
			GuiItemStack::showItemIcon(m_itemIcon, m_itemStack, itemId, damage, icon);
		LordDelete itemStack;
	}

	void GuiAppShopItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GuiAppShopItem::getPrivateChildren()
	{
		return { m_window };
	}

	bool GuiAppShopItem::onButtonClick(const EventArgs & events)
	{
		EventArgs args;
		if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_goods.itemId, m_goods.itemMeta, m_goods.itemNum))
		{
			args.handled = (int)BuyResult::FullInventory;
			fireEvent("LackOfMoney", args);
			return false;
		}

		if (!m_goods.canBuy())
		{
			args.handled = (int)BuyResult::LackingGood;
			fireEvent("LackOfMoney", args);
			return false;
		}

		if (!Blockman::Instance()->m_pPlayer->canAddBulletItem(m_goods.itemId))
		{
			args.handled = (int)BuyResult::HasNoGun;
			fireEvent("LackOfMoney", args);
			return false;
		}

		Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		if (wallet)
		{
			ui64 money = wallet->getMoneyByCoinId(m_goods.coinId);
			i32 price = GameClient::CGame::Instance()->isMultiplayer() ? m_goods.blockmanPrice : m_goods.blockymodsPrice;
			if (money < price)
			{
				args.handled = (int)BuyResult::LackOfMoney;
				fireEvent("LackOfMoney", args);
				ShowLackOfMoneyTipDialogEvent::emit((i32)(price - money), (CurrencyType)m_goods.coinId);
				return false;
			}
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyGoods(m_groupIndex, m_goods.index, m_goods.uniqueId);
		}
		return true;
	}
}