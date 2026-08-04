#include "GuiBirdFuse.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	
	void GuiBirdFuse::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdFuse::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdFuse::GuiBirdFuse(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdFuse.json"));
		renameComponents(m_window);

		m_firstBird = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(0)->GetChildByIndex(0));
		m_secondBird = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(1)->GetChildByIndex(0));
		m_thirdBird = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(2)->GetChildByIndex(0));
		m_btnSure = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_btnCancel = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));


		m_firstBird->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFuse::onClick, this, std::placeholders::_1, ViewId::BTN_FIRST));
		m_secondBird->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFuse::onClick, this, std::placeholders::_1, ViewId::BTN_SECOND));
		m_thirdBird->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFuse::onClick, this, std::placeholders::_1, ViewId::BTN_THIRD));

		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&GuiBirdFuse::onClick, this, std::placeholders::_1, ViewId::BTN_SURE));
		m_btnCancel->subscribeEvent(EventButtonClick, std::bind(&GuiBirdFuse::onClick, this, std::placeholders::_1, ViewId::BTN_CANCEL));

		m_subscriptionGuard.add(BirdRemoveFuseItemEvent::subscribe(std::bind(&GuiBirdFuse::removeFuseItem, this, std::placeholders::_1)));

		if (LanguageManager::Instance())
		{
			m_btnSure->SetText(LanguageManager::Instance()->getString("gui_bird_fush_sure").c_str());
			m_btnCancel->SetText(LanguageManager::Instance()->getString("gui_bird_fush_cancel").c_str());
		}

		AddWindowToDrawList(*m_window);
	}

	void GuiBirdFuse::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		m_btnCancel->SetEnabled(m_firstBirdId > 0 || m_secondBirdId > 0 || m_thirdBirdId > 0);
		m_btnSure->SetEnabled(m_firstBirdId > 0 && m_secondBirdId > 0 && m_thirdBirdId > 0);

	}

	
	void GuiBirdFuse::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdFuse::itemRemove(i64 birdId)
	{
		if (birdId > 0)
		{
			BirdFuseRemoveItemEvent::emit(birdId);
		}
	}

	bool GuiBirdFuse::onPutBird(i64 birdId, const String & icon)
	{
		if (m_firstBirdId<= 0)
		{
			m_firstBird->SetImage(icon.c_str());
			m_firstBirdId = birdId;
			return true;
		}

		 if (m_secondBirdId <= 0)
		{
			m_secondBird->SetImage(icon.c_str());
			m_secondBirdId = birdId;
			return true;
		}

		if (m_thirdBirdId <= 0)
		{
			m_thirdBird->SetImage(icon.c_str());
			m_thirdBirdId = birdId;
			return true;
		}

		return false;
	}

	bool GuiBirdFuse::removeFuseItem(i64 birdId)
	{
		if (birdId == m_firstBirdId)
		{
			removeBirdFuse(ViewId::BTN_FIRST);
			return true;
		}

		if (birdId == m_secondBirdId)
		{
			removeBirdFuse(ViewId::BTN_SECOND);
			return true;
		}

		if (birdId == m_thirdBirdId)
		{
			removeBirdFuse(ViewId::BTN_THIRD);
			return true;
		}

		return false;
	}

	void GuiBirdFuse::removeBirdFuse(ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_FIRST:
			m_firstBird->SetImage("");
			m_firstBirdId = 0;
			break;
		case ViewId::BTN_SECOND:
			m_secondBird->SetImage("");
			m_secondBirdId = 0;
			break;
		case ViewId::BTN_THIRD:
			m_thirdBird->SetImage("");
			m_thirdBirdId = 0;
			break;
		}
	}

	void GuiBirdFuse::removeAllItems()
	{
		removeBirdFuse(ViewId::BTN_FIRST);
		removeBirdFuse(ViewId::BTN_SECOND);
		removeBirdFuse(ViewId::BTN_THIRD);
	}

	bool GuiBirdFuse::onClick(const EventArgs & args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_FIRST:
			itemRemove(m_firstBirdId);
			removeBirdFuse(ViewId::BTN_FIRST);
			break;
		case ViewId::BTN_SECOND:
			itemRemove( m_secondBirdId);
			removeBirdFuse(ViewId::BTN_SECOND);
			break;
		case ViewId::BTN_THIRD:
			itemRemove(m_thirdBirdId);
			removeBirdFuse(ViewId::BTN_THIRD);
			break;
		case ViewId::BTN_SURE:
		{
			vector<i64>::type brids = vector<i64>::type();
			brids.push_back(m_firstBirdId);
			brids.push_back(m_secondBirdId);
			brids.push_back(m_thirdBirdId);
			ClientNetwork::Instance()->getSender()->sendBirdFuse(brids);
		}
			break;
		case ViewId::BTN_CANCEL:
			itemRemove(m_firstBirdId);
			itemRemove(m_secondBirdId);
			itemRemove(m_thirdBirdId);
			removeBirdFuse(ViewId::BTN_FIRST);
			removeBirdFuse(ViewId::BTN_SECOND);
			removeBirdFuse(ViewId::BTN_THIRD);
			break;
		}
		return true;
	}



}