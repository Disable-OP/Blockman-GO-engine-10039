#include "GuiRanchFriendOperate.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{

	void GuiRanchFriendOperate::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiRanchFriendOperate::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchFriendOperate::GuiRanchFriendOperate(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchFriendOperate.json"));
		renameComponents(m_window);

		m_window->GetChildByIndex(0)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchFriendOperate::onClick, this, std::placeholders::_1, ViewId:: BTN_HELP));
		m_window->GetChildByIndex(1)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchFriendOperate::onClick, this, std::placeholders::_1, ViewId::BTN_VISIT));

		if (LanguageManager::Instance())
		{
			m_window->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_operate").c_str());
			m_window->GetChildByIndex(1)->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_visit").c_str());
		}
		
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchFriendOperate::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		m_window->GetChildByIndex(0)->SetEnabled(m_needHelp);
	}


	void GuiRanchFriendOperate::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiRanchFriendOperate::onClick(const EventArgs & args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_HELP:
			ShowRanchFriendHelpListEvent::emit(m_userId);
			break;
		case ViewId::BTN_VISIT:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_userId);
			break;
		}
		return true;
	}
}