#include "gui_birdActivity.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "Util/LanguageKey.h"
#include "World/BirdSimulator.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiBirdActivityItem.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{

	gui_birdActivity::gui_birdActivity() :
		gui_layout("BirdActivity.json")
	{
	}

	gui_birdActivity::~gui_birdActivity()
	{
	}

	void gui_birdActivity::onShow()
	{
		if (m_isActivityUpdate)
		{
			showBirdActivity();
			m_isActivityUpdate = false;
		}
	}

	void gui_birdActivity::onLoad()
	{
		setText("BirdActivity-Title", LanguageKey::GUI_BIRD_ACTIVITY);
		setText("BirdActivity-Buy", LanguageKey::GUI_BIRD_ACTIVITY_BUTTON);
		m_time = getWindow<GUIStaticText>("BirdActivity-Time-Text");
		m_gift = getWindow<GUIListHorizontalBox>("BirdActivity-gift");
		m_currencyIcon = getWindow<GUIStaticImage>("BirdActivity-Pay-Icon");
		m_currencyNum = getWindow<GUIStaticText>("BirdActivity-Pay-Num");

		getWindow("BirdActivity-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdActivity::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BirdActivity-Buy")->subscribeEvent(EventButtonClick, std::bind(&gui_birdActivity::onClick, this, std::placeholders::_1, ViewId::BUY));
		m_subscriptionGuard.add(UpdateBirdActivityEvent::subscribe(std::bind(&gui_birdActivity::updateBirdActivity, this)));
		
	}

	void gui_birdActivity::onUpdate(ui32 nTimeElapse)
	{
		if (m_time && m_timeLeft > 0)
		{
			m_timeLeft = (m_timeLeft - nTimeElapse) > 0 ? (m_timeLeft - nTimeElapse) : 0;
			m_time->SetText(timeFormat(m_timeLeft).c_str());
			if (m_timeLeft <= 0)
			{
				getWindow("BirdActivity-Buy")->SetEnabled(false);
				HideBirdActivityEvent::emit();
			}
		}
	}

	bool gui_birdActivity::updateBirdActivity()
	{
		m_isActivityUpdate = true;
		onShow();
		return false;
	}

	void gui_birdActivity::showBirdActivity()
	{
		m_gift->ClearAllItem();
		m_gift->SetInterval(31.f);

		if (!UICommon::checkBirdParam())
		{
			return;
		}

		auto gifts = Blockman::Instance()->m_pPlayer->m_birdSimulator->getActivity();
		if (!gifts || gifts->timeLeft <= 0)
		{
			getWindow("BirdActivity-Buy")->SetEnabled(false);
			HideBirdActivityEvent::emit();
		}
		if (gifts)
		{
			m_timeLeft = gifts->timeLeft;
			if (isShown() && m_buyTime > 0)
			{
				m_timeLeft = m_buyTime;
			}
			m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(gifts->currencyType).c_str());
			m_currencyNum->SetText(StringUtil::ToString(gifts->price).c_str());
			static int count = 0;
			for (auto gift : gifts->items)
			{
				count++;
				String itemGiftView = StringUtil::Format("BirdActivity-gift-Item-%d", count).c_str();
				GuiBirdActivityItem* itemGift = (GuiBirdActivityItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_ACTIVITY_ITEM, itemGiftView.c_str());
				itemGift->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.f, 86.f), UDim(0.0f, 85.f));
				itemGift->SetTouchable(false);
				itemGift->setGiftItem(gift.icon, gift.num);
				m_gift->AddItem(itemGift, false);
			}
		}
	}

	bool gui_birdActivity::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case  ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			playSoundByType(ST_ClosePanel);
			break;
		case  ViewId::BUY:
			if (UICommon::checkBirdParam())
			{
				auto gifts = Blockman::Instance()->m_pPlayer->m_birdSimulator->getActivity();
				if (gifts && UICommon::checkMoney(gifts->price, gifts->currencyType))
				{
					ClientNetwork::Instance()->getSender()->sendBirdPersonalStoreBuyGoods(-1, gifts->id);
					m_buyTime = m_timeLeft;
				}
			}
			break;
			
		}
		return true;
	}

	String gui_birdActivity::timeFormat(i32 time)
	{
		i32 seconds = time / 1000;
		i32 day = seconds / 86400;
		String d = StringUtil::ToString(day);
		String h = StringUtil::ToString((seconds % 86400) / 3600);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString((seconds % 3600 ) / 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(seconds % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		GUIString name = getString(LanguageKey::GUI_BIRD_ACTIVITY_DAY);
		return StringUtil::Format("%s%s%s:%s:%s", d.c_str(), name.c_str(), h.c_str(), m.c_str(), s.c_str());
		
	}

}
