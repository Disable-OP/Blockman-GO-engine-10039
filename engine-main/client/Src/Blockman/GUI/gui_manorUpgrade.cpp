#include "gui_manorUpgrade.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Manor.h"
#include "game.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_manorUpgrade::gui_manorUpgrade()
		: gui_layout("ManorUpgrade.json")
	{

	}

	gui_manorUpgrade::~gui_manorUpgrade()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_manorUpgrade::onLoad()
	{
		m_title = getWindow<GUIStaticText>("ManorUpgrade-Title-Name");
		m_price = getWindow<GUIStaticText>("ManorUpgrade-Message-Price");
		m_groundValue = getWindow<GUIStaticText>("ManorUpgrade-Content-Ground-Value");
		m_roseValue = getWindow<GUIStaticText>("ManorUpgrade-Content-Rose-Value");
		m_seedValue = getWindow<GUIStaticText>("ManorUpgrade-Content-Seed-Value");
		m_charmValue = getWindow<GUIStaticText>("ManorUpgrade-Content-Charm-Value");
		m_btnSure = getWindow<GUIButton>("ManorUpgrade-Btn-Sure");
		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&gui_manorUpgrade::onClick, this, std::placeholders::_1, ViewId::BTN_SURE));
		getWindow<GUIButton>("ManorUpgrade-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_manorUpgrade::onClick, this, std::placeholders::_1, ViewId::BTN_CANCEL));
		getWindow("ManorUpgrade-Content-Ground-Name")->SetText(getString(LanguageKey::GUI_MANOR_UPGRADE_INFO_GROUND));
		getWindow("ManorUpgrade-Content-Rose-Name")->SetText(getString(LanguageKey::GUI_MANOR_UPGRADE_INFO_ROSE));
		getWindow("ManorUpgrade-Content-Seed-Name")->SetText(getString(LanguageKey::GUI_MANOR_UPGRADE_INFO_SEED));
		getWindow("ManorUpgrade-Content-Charm-Name")->SetText(getString(LanguageKey::GUI_MANOR_UPGRADE_INFO_CHARM));
		getWindow("ManorUpgrade-Message-Expenditure")->SetText(getString(LanguageKey::GUI_MANOR_UPGRADE_EXPENDITURE));
		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
	}

	void gui_manorUpgrade::onUpdate(ui32 nTimeElapse)
	{
		if (isShown() && Blockman::Instance()->m_pPlayer->m_manor->getNextInfo())
		{
			ManorInfo* nextInfo = Blockman::Instance()->m_pPlayer->m_manor->getNextInfo();
			getWindow<GUIStaticImage>("ManorUpgrade-Message-Currency-Icon")->SetImage(UICommon::getCurrencyIconByType(nextInfo->currencyType).c_str());
			m_title->SetText(StringUtil::Format(getString(LanguageKey::GUI_MANOR_UPGRADE_TITLE).c_str(), nextInfo->level).c_str());
			m_groundValue->SetText(StringUtil::Format("+ %s%%", nextInfo->ground.c_str()).c_str());
			m_roseValue->SetText(StringUtil::Format("+ %d", nextInfo->rose).c_str());
			m_charmValue->SetText(StringUtil::Format("+ %d", nextInfo->charm).c_str());
			m_seedValue->SetText(StringUtil::Format("+ %s", nextInfo->seed.c_str()).c_str());
			m_price->SetText(StringUtil::ToString(nextInfo->price).c_str());
		}
	}

	bool gui_manorUpgrade::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_SURE:
			{
				ManorInfo* nextInfo = Blockman::Instance()->m_pPlayer->m_manor->getNextInfo();
				if (Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)nextInfo->currencyType) - nextInfo->price >= 0)
				{
					ClientNetwork::Instance()->getSender()->sendUpgradeManor();
					RootGuiLayout::Instance()->showManor();
				}
				else
				{
					i64 difference = nextInfo->price - Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)nextInfo->currencyType);
					ShowLackOfMoneyTipDialogEvent::emit((i32)difference, nextInfo->currencyType);
				}
			}
			break;
		case ViewId::BTN_CANCEL:
			RootGuiLayout::Instance()->showManor();
			break;
		}
		return true;
	}

}
