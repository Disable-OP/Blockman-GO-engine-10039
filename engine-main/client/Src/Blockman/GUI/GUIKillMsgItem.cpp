#include "GUIKillMsgItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GuiItemStack.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	GUIKillMsgItem::GUIKillMsgItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("KillMsgItem.json"));
		renameComponents(m_window);
		m_message = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_armsHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_headHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_killHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIKillMsgItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIKillMsgItem::setKillMsg(KillMsg* msg)
	{
		float right = 0;
		m_killHint->SetVisible(msg->kills > 0);
		m_killHint->SetXPosition({ 0, right });
		if (msg->kills > 0)
		{
			int kills = msg->kills > 5 ? 5 : msg->kills;
			m_killHint->SetImage(StringUtil::Format("set:gun.json image:Kill_%d", kills).c_str());
		}
		right -= msg->kills > 0 ? 50 : 0;
		m_headHint->SetVisible(msg->head > 0);
		m_headHint->SetXPosition({ 0, right });
		right -= msg->head > 0 ? 50 : 0;
		m_armsHint->SetVisible(msg->armsId > 0);
		m_armsHint->SetXPosition({ 0, right });
		if (msg->armsId > 0)
		{
			ItemStackPtr pItemStack = LORD::make_shared<ItemStack>(msg->armsId, 1, 0);
			GuiItemStack::showItemIcon(m_armsHint, pItemStack, msg->armsId, 1);
		}
		right -= msg->armsId > 0 ? 50 : 0;
		right -= 5;
		m_message->SetXPosition({ 0, right });
		String content = LanguageManager::Instance()->getString(LanguageKey::GUI_KILL_MSG_ITEM_MESSAGE);
		content = StringUtil::Format(content.c_str(), msg->killer.c_str(), msg->dead.c_str());
		content = msg->color + content;
		m_message->SetText(content.c_str());
	}

	void GUIKillMsgItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIKillMsgItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

}
