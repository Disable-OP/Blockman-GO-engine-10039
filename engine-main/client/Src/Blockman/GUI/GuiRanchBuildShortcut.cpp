#include "GuiRanchBuildShortcut.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "World/Ranch.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{

	void GuiRanchBuildShortcut::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildShortcut::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildShortcut::GuiRanchBuildShortcut(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildShortcut.json"));
		renameComponents(m_window);
		m_tvTimeValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_tvPrice = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_window->GetChildByIndex(1)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildShortcut::onClick, this, std::placeholders::_1, VIewId::SURE));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildShortcut::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (!LanguageManager::Instance() || !UICommon::checkRanchParam() || !IsVisible())
		{
			return;
		}
		m_productQueue.timeLeft -= nTimeElapse;
		m_price = Blockman::Instance()->m_pPlayer->m_ranch->getItemTimePrice(m_productQueue.timeLeft / 1000, m_productQueue.productId);
		m_tvPrice->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_ranch_build_queue_shortcut_done").c_str(), m_price).c_str());
		m_tvTimeValue->SetText(timeFormat().c_str());
		if (m_productQueue.timeLeft == 0)
		{
			//ShowRanchBuildMainEvent::emit();
		}
	}


	void GuiRanchBuildShortcut::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}


	bool GuiRanchBuildShortcut::onClick(const EventArgs & args, VIewId viewId)
	{

		//1 production 2 speed up
		ClientNetwork::Instance()->getSender()->sendRanchBuildQueueOperation(2, m_entityId, m_productQueue.queueId, 0);
		//ShowRanchBuildMainEvent::emit();
		return true;
	}

	String GuiRanchBuildShortcut::timeFormat()
	{
		if (m_price == 0) {
			return LanguageManager::Instance()->getString("gui_ranch_build_queue_shortcut_free_done");
		}
		int h = m_productQueue.timeLeft / 1000 / 60 / 60;
		int m = m_productQueue.timeLeft / 1000 / 60 % 60;
		int s = m_productQueue.timeLeft / 1000 % 60;
		return StringUtil::Format("%d:%d:%d", h, m, s);
	}

}