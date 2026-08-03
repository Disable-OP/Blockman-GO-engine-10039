#include "GuiRanchOrderItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/RanchSetting.h"
#include "Util/FriendManager.h"
#include "Util/RedDotManager.h"

namespace BLOCKMAN
{

	void GuiRanchOrderItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiRanchOrderItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchOrderItem::GuiRanchOrderItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchOrderItem.json"));
		renameComponents(m_window);

		m_stateIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_hotIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_helpIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_helperIcon = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(3));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4));
		m_tvValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5));
		m_doneIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(6));
		m_window->GetChildByIndex(7)->SetEffectName("reddot.effect");
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchOrderItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchOrderItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);


		m_hotIcon->SetVisible(m_orderInfo.isHot);
		m_helpIcon->SetVisible(m_orderInfo.isHelp && !m_orderInfo.isDone);
		m_helperIcon->SetVisible(m_orderInfo.isHelp && m_orderInfo.isDone);
		m_doneIcon->SetVisible(!m_orderInfo.isHelp && m_orderInfo.isDone);
		m_tvValue->SetVisible(!m_orderInfo.isDone);
		m_itemIcon->SetVisible(!m_orderInfo.isDone);
		m_stateIcon->SetImage(getStateImage().c_str());
		m_itemIcon->SetImage(RanchSetting::getRanchItemIcon(m_orderInfo.itemId).c_str());
		m_tvValue->SetText(StringUtil::Format("x%d", m_orderInfo.needNum).c_str());
		m_window->GetChildByIndex(7)->SetVisible(RedDotManager::checkOrderItem(m_orderId, m_orderInfo.index));
		m_window->SetTouchable(!m_orderInfo.isDone);
	}


	void GuiRanchOrderItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchOrderItem::setOrderInfo(const RanchOrderInfo & info)
	{
		m_orderInfo = info;
		m_helperIcon->setDefaultImage(getHelperImage().c_str());
		if (m_orderInfo.isHelp)
		{
			auto pal = FriendManager::findFriendById(m_orderInfo.helperId);
			m_helperIcon->setUrl(pal ? pal->picUrl : "");
		}
	}

	bool GuiRanchOrderItem::onClick(const EventArgs & args)
	{
		RanchOrderItemClickEvent::emit(GetPosition(), m_orderInfo);
		return true;
	}

	String GuiRanchOrderItem::getStateImage()
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

	String GuiRanchOrderItem::getHelperImage()
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