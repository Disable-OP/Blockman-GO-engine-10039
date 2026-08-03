#include "GuiRanchOrderOperate.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/RanchSetting.h"
#include "Util/LanguageManager.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{

	void GuiRanchOrderOperate::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiRanchOrderOperate::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchOrderOperate::GuiRanchOrderOperate(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchOrderOperate.json"));
		renameComponents(m_window);

		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_itemIcon->GetChildByIndex(0));
		m_tvValue = dynamic_cast<GUIStaticText*>(m_itemIcon->GetChildByIndex(1));
		m_window->GetChildByIndex(1)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchOrderOperate::onClick, this, std::placeholders::_1, ViewId::BTN_HELP));
		m_window->GetChildByIndex(2)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchOrderOperate::onClick, this, std::placeholders::_1, ViewId::BTN_PUT));

		if (LanguageManager::Instance())
		{
			m_window->GetChildByIndex(1)->SetText(LanguageManager::Instance()->getString("gui_ranch_order_operate_help").c_str());
			m_window->GetChildByIndex(2)->SetText(LanguageManager::Instance()->getString("gui_ranch_order_operate_put").c_str());
		}

		AddWindowToDrawList(*m_window);
	}

	void GuiRanchOrderOperate::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage() || !IsVisible() || !LanguageManager::Instance())
		{
			return;
		}
		
		m_window->GetChildByIndex(1)->SetEnabled(!m_orderInfo.isHelp);
		m_itemIcon->SetImage(RanchSetting::getRanchItemIcon(m_orderInfo.itemId).c_str());
		m_tvName->SetText(LanguageManager::Instance()->getString(RanchSetting::getRanchItemName(m_orderInfo.itemId)).c_str());
		m_tvValue->SetText(getNumString(Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->getItemNum(m_orderInfo.itemId), m_orderInfo.needNum).c_str());
	}


	void GuiRanchOrderOperate::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchOrderOperate::chengeUI()
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchOrderHelpOperate.json"));
		renameComponents(m_window);

		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_itemIcon->GetChildByIndex(0));
		m_tvValue = dynamic_cast<GUIStaticText*>(m_itemIcon->GetChildByIndex(1));
		m_window->GetChildByIndex(2)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchOrderOperate::onClick, this, std::placeholders::_1, ViewId::BTN_HELP_PUT));

		if (LanguageManager::Instance())
		{
			m_window->GetChildByIndex(1)->SetText(LanguageManager::Instance()->getString("gui_ranch_order_operate_help").c_str());
			m_window->GetChildByIndex(2)->SetText(LanguageManager::Instance()->getString("gui_ranch_order_operate_put").c_str());
		}

		AddWindowToDrawList(*m_window);
	}

	bool GuiRanchOrderOperate::onClick(const EventArgs & args, ViewId viewId)
	{

		switch (viewId)
		{
		case ViewId::BTN_HELP:
			ClientNetwork::Instance()->getSender()->sendRanchOrderOperate(1, m_orderId, m_orderInfo.index);
			break;
		case ViewId::BTN_PUT:
		{
			if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
			{
				i32 itemInventory = Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->getItemNum(m_orderInfo.itemId);
				i32 lackNum = m_orderInfo.needNum - itemInventory;
				if (lackNum > 0 )
				{
					vector<RanchCommon>::type items = vector<RanchCommon>::type();
					items.push_back(RanchCommon(m_orderInfo.itemId, lackNum, RanchSetting::getRanchItemCubePrice(m_orderInfo.itemId)));
					ShowRanchOrderOperateLockItemsEvent::emit(items, m_orderId, m_orderInfo.index);
					return false;
				}
				ClientNetwork::Instance()->getSender()->sendRanchOrderOperate(2, m_orderId, m_orderInfo.index);
			}
		}
		break;
		case ViewId::BTN_HELP_PUT:
		{
			SetVisible(false);
			if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
			{
				i32 itemInventory = Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->getItemNum(m_orderInfo.itemId);
				i32 lackNum = m_orderInfo.needNum - itemInventory;
				if (lackNum > 0)
				{
					vector<RanchCommon>::type items = vector<RanchCommon>::type();
					items.push_back(RanchCommon(m_orderInfo.itemId, lackNum, RanchSetting::getRanchItemCubePrice(m_orderInfo.itemId)));
					ShowRanchLockItemsByOrderHelpEvent::emit(items, m_orderInfo.helpId);
					return false;
				}
				ClientNetwork::Instance()->getSender()->sendRanchHelpFinish(m_orderInfo.helpId, 1);
			}
		}
		break;
		}
		RanchOrderItemHideEvent::emit();
		return true;
	}


	String GuiRanchOrderOperate::getNumString(i32 num, i32 needNum)
	{
		if (num >= needNum)
		{
			return StringUtil::Format("▢FF66ff00%d▢FF69523b/%d", num, needNum);
		}
		else
		{
			return StringUtil::Format("▢FFFF0020%d▢FF69523b/%d", num, needNum);
		}
	}


}