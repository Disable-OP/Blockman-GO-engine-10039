#include "GuiRanchBuildItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"

namespace BLOCKMAN
{
	
	void GuiRanchBuildItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiRanchBuildItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildItem::GuiRanchBuildItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		initUI("RanchBuildItem.json");
	}

	void GuiRanchBuildItem::initUI(String json)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON(json.c_str()));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));

		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_redPoint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_btnBuy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));

		m_priceLayout = dynamic_cast<GUILayout*>(m_btnBuy->GetChildByIndex(0));
		m_tvPrice = dynamic_cast<GUIStaticText*>(m_btnBuy->GetChildByIndex(0)->GetChildByIndex(1));

		m_banBuy = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
		m_tvNum->GetChildByIndex(0)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildItem::onClick, this, std::placeholders::_1, ViewId::BTN_SUB));
		m_tvNum->GetChildByIndex(1)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildItem::onClick, this, std::placeholders::_1, ViewId::BTN_ADD));
		m_icon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchBuildItem::onClick, this, std::placeholders::_1, ViewId::BTN_INFO));
		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildItem::onClick, this, std::placeholders::_1, ViewId::BTN_BUY));
		m_btnBuy->SetTouchable(false);
		m_icon->SetTouchable(false);
		m_redPoint->SetEffectName("reddot.effect");
		m_redPoint->SetImage("");
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo() || !LanguageManager::Instance())
		{
			return;
		}

		m_icon->SetImage(RanchSetting::getRanchItemIcon(m_build.id).c_str());
		m_name->SetText(LanguageManager::Instance()->getString(RanchSetting::getRanchItemName(m_build.id)).c_str());
		
		m_tvPrice->SetText(StringUtil::ToString(m_build.price * m_num).c_str());

		if (Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel)
		{
			m_priceLayout->SetVisible(m_build.price > 0);
			m_btnBuy->SetText(m_build.price == 0 ? LanguageManager::Instance()->getString("gui_ranch_build_price_free").c_str() : "");
			m_btnBuy->SetTouchable(true);
		}
		else
		{
			GUIString needLevelStr = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_RANCH_UNLOCK_LEVEL).c_str(), m_build.needLevel).c_str();
			m_btnBuy->SetText(needLevelStr);
			m_priceLayout->SetVisible(false);
			m_btnBuy->SetTouchable(false);

		}

		if (m_type == 1)
		{
			m_redPoint->SetVisible(RedDotManager::checkRaiseById(m_build.id));
		}
		else if (m_type == 2)
		{
			m_redPoint->SetVisible(RedDotManager::checkFactoryById(m_build.id));
		}
		else
		{
			if (m_build.id == 500000)
			{
				m_redPoint->SetVisible(RedDotManager::checkPlantById(m_build.id));
			}
			else 
			{
				m_redPoint->SetVisible(false);
			}
		}

		if (m_type == 3 && m_build.id != 500000)
		{
			m_tvNum->SetText(StringUtil::ToString(m_num).c_str());
			m_tvNum->GetChildByIndex(0)->SetEnabled(m_num > 1);
			m_tvNum->GetChildByIndex(1)->SetEnabled(m_num > 0);
			m_banBuy->SetVisible(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level < m_build.needLevel);
			//m_icon->SetTouchable(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);
			m_btnBuy->SetTouchable(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);
			m_tvNum->GetChildByIndex(0)->SetVisible(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);
			m_tvNum->GetChildByIndex(1)->SetVisible(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);

		}
		else 
		{
			//m_icon->SetTouchable(m_build.num < m_build.maxNum && Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);
			m_btnBuy->SetTouchable(m_build.num < m_build.maxNum && Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel);
			m_banBuy->SetVisible(m_build.num >= m_build.maxNum || Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level < m_build.needLevel);
			m_tvNum->SetText(StringUtil::Format("%d/%d", m_build.num, m_build.maxNum).c_str());
			m_tvNum->GetChildByIndex(0)->SetVisible(false);
			m_tvNum->GetChildByIndex(1)->SetVisible(false);
		}
		m_btnBuy->SetNormalImage(m_build.num < m_build.maxNum && Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_build.needLevel ? "set:ranch_main.json image:ranch_btn_sure" : "set:ranch_main.json image:ranch_build_item_price");
	}

	
	void GuiRanchBuildItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildItem::setName(const GUIString & strName)
	{
		if (m_name)
		{
			m_name->SetText(strName);
		}
	}

	void GuiRanchBuildItem::setIcon(const GUIString& strIconImage)
	{
		if (m_icon)
		{
			m_icon->SetImage(strIconImage);
		}
	}

	void GuiRanchBuildItem::setBuild(const RanchBuild & build)
	{
		m_build = build;
		m_num = ((m_type == 3 && m_build.id != 500000) ? 10 : 1);
	}

	void GuiRanchBuildItem::setType(int type)
	{
		m_type = type;
		
	}

	bool GuiRanchBuildItem::onClick(const EventArgs & args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_BUY:

			if (!UICommon::checkRanchLevel(m_build.needLevel))
			{
				return false;
			}

			if (UICommon::checkMoney(m_build.price * m_num, CurrencyType::GAME_MONEY))
			{
				ClientNetwork::Instance()->getSender()->sendBuyRanchBuildItem(m_build.id, m_num, m_type, (i32)CurrencyType::GAME_MONEY);
				m_num = ((m_type == 3 && m_build.id != 500000) ? 10 : 1);
			}
			break;

		case ViewId::BTN_INFO:

			break;
		case ViewId::BTN_SUB:
			m_num -= 1;
			break;
		case ViewId::BTN_ADD:
			m_num += 1;
			break;
		}
		return true;
	}

}