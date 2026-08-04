#include "GuiManorMessageItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "GuiItemStack.h"

namespace BLOCKMAN
{
	void GuiManorMessageItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiManorMessageItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiManorMessageItem::GuiManorMessageItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ManorMessageItem.json"));
		renameComponents(m_window);
		GUILayout* content = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_messageTip = dynamic_cast<GUIStaticText*>(content->GetChildByIndex(0));
		m_btnGive = dynamic_cast<GUIButton*>(content->GetChildByIndex(1));
		m_vegetable = dynamic_cast<GUILayout*>(content->GetChildByIndex(2));
		m_vegetableIcon = dynamic_cast<GUIStaticImage*>(m_vegetable->GetChildByIndex(0));
		m_vegetableValue = dynamic_cast<GUIStaticText*>(m_vegetable->GetChildByIndex(1));
		m_btnCallOn = dynamic_cast<GUIButton*>(content->GetChildByIndex(3));
		m_btnDelete = dynamic_cast<GUIButton*>(content->GetChildByIndex(4));

		m_btnGive->subscribeEvent(EventButtonClick, std::bind(&GuiManorMessageItem::onClick, this, std::placeholders::_1, ViewId::BTN_GIVE));
		m_btnDelete->subscribeEvent(EventButtonClick, std::bind(&GuiManorMessageItem::onClick, this, std::placeholders::_1, ViewId::BTN_DELETE));
		m_btnCallOn->subscribeEvent(EventButtonClick, std::bind(&GuiManorMessageItem::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));

		m_btnGive->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_BTN_GIVE).c_str());
		m_btnCallOn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_BTN_CALL_ON).c_str());

		AddWindowToDrawList(*m_window);
	}

	void GuiManorMessageItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (!m_isDelete && m_manorMessage)
		{
			String tip = "";
			switch (m_manorMessage->messageType)
			{
			case 1:
				tip = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_GIVE_ROSE_TIP).c_str(), m_manorMessage->userName.c_str());
				m_window->SetBackImage("set:manor_atlas.json image:manor_message_item_nor");
				m_btnGive->SetVisible(true);
				m_vegetable->SetVisible(false);
				break;
			case 2:
				tip = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_VEGETEAL_TIP).c_str(), m_manorMessage->userName.c_str());
				m_window->SetBackImage("set:manor_atlas.json image:manor_message_item_pre");
				m_vegetableValue->SetText(StringUtil::ToString(m_manorMessage->num).c_str());
				updateIcon(m_manorMessage->productId, 0);
				m_btnGive->SetVisible(false);
				m_vegetable->SetVisible(true);
				break;
			case 0:
			default:
				tip = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_CALL_ON_TIP).c_str(), m_manorMessage->userName.c_str());
				m_window->SetBackImage("set:manor_atlas.json image:manor_message_item_nor");
				m_btnGive->SetVisible(true);
				m_vegetable->SetVisible(false);
				break;
			}
			m_messageTip->SetText(tip.c_str());
		}
	}

	void GuiManorMessageItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiManorMessageItem::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_GIVE:
			ClientNetwork::Instance()->getSender()->sendGiveRose(m_manorMessage->userId);
			break;
		case ViewId::BTN_CALL_ON:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_manorMessage->userId);
			break;
		case ViewId::BTN_DELETE:
			m_isDelete = true;
			ClientNetwork::Instance()->getSender()->sendDeleteManorMessage(m_manorMessage);
			ManorMessageRemoveEvent::emit(m_manorMessage->id.c_str(), m_strName.c_str());
			break;
		}
		return true;
	}


	void GuiManorMessageItem::updateIcon(int itemId, int meta)
	{
		if (m_isChange)
		{
			LordLogInfo("GuiManorMessageItem set vegetable Icon : itemId %d", itemId);
			ItemStack* itemStack = LordNew ItemStack(itemId, 1, meta);
			ItemStackPtr m_itemStack = itemStack->copy();
			GuiItemStack::showItemIcon(m_vegetableIcon, m_itemStack, itemId, meta);
			LordDelete itemStack;
			m_isChange = false;
		}
	}

}