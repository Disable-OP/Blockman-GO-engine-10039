#include "GuiRanchFriendHelpItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/RanchSetting.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{

	void GuiRanchFriendHelpItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiRanchFriendHelpItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchFriendHelpItem::GuiRanchFriendHelpItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchFriendHelpItem.json"));
		renameComponents(m_window);

		m_stateIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_hotIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_tvValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_tvStateValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(0));
		if (LanguageManager::Instance())
		{
			m_window->GetChildByIndex(4)->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_help_state").c_str());
		}
		m_itemIcon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchFriendHelpItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchFriendHelpItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);


		m_hotIcon->SetVisible(m_orderInfo.isHot);
		//m_itemIcon->SetVisible(!m_orderInfo.isDone);
		//m_tvValue->SetVisible(!m_orderInfo.isDone);
		m_stateIcon->SetImage(getStateImage().c_str());
		m_tvValue->SetText(StringUtil::Format("x%d", m_orderInfo.needNum).c_str());
		m_itemIcon->SetImage(RanchSetting::getRanchItemIcon(m_orderInfo.itemId).c_str());
		m_tvStateValue->SetText(StringUtil::Format("%d/%d", m_orderInfo.fullBoxNum, m_orderInfo.boxAmount).c_str());
	}


	void GuiRanchFriendHelpItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiRanchFriendHelpItem::onClick(const EventArgs & args)
	{
		RanchOrderItemClickEvent::emit(getAbsolutePositionFromRoot(), m_orderInfo);
		return true;
	}

	String GuiRanchFriendHelpItem::getStateImage()
	{
		if (m_orderInfo.isDone && !m_orderInfo.isHelp)
		{
			return "set:ranch_order.json image:order_full";
		}

		if (!m_orderInfo.isDone && !m_orderInfo.isHelp)
		{
			return "set:ranch_order.json image:order_empty";
		}

		if (!m_orderInfo.isDone && m_orderInfo.isHelp)
		{
			return "set:ranch_order.json image:order_help_empty";
		}

		if (m_orderInfo.isDone && m_orderInfo.isHelp)
		{
			return "set:ranch_order.json image:order_help_full";
		}

		return "set:ranch_order.json image:order_empty";
	}

	String GuiRanchFriendHelpItem::getHelperImage()
	{
		switch (m_orderInfo.helperSex)
		{
		case 1:
			return "set:ranch_boy_icon.json image:ranch_boy_icon";
		case 2:
			return "set:ranch_girl_icon.json image:ranch_girl_icon";
		default:
			return "";
		}
	}


}