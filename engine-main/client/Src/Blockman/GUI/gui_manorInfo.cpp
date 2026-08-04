#include "gui_manorInfo.h"
#include "Util/LanguageKey.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN 
{

	gui_manorInfo::gui_manorInfo()
		: gui_layout("ManorInfo.json")
	{
		
	}

	gui_manorInfo::~gui_manorInfo()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_manorInfo::onLoad()
	{
		m_manorPreview = getWindow<GUIStaticImage>("ManorInfo-Left-Preview");
		m_manorLevel = getWindow<GUIStaticText>("ManorInfo-Right-Level");
		m_btnTeleport = getWindow<GUIButton>("ManorInfo-Btn-Teleport");
		m_btnUpgrade = getWindow<GUIButton>("ManorInfo-Btn-Upgrade");

		m_groundValue = getWindow<GUIStaticText>("ManorInfo-Ground-Value");
		m_roseValue = getWindow<GUIStaticText>("ManorInfo-Rose-Value");
		m_seedValue = getWindow<GUIStaticText>("ManorInfo-Seed-Value");
		m_charmValue = getWindow<GUIStaticText>("ManorInfo-Charm-Value");
		m_valueValue = getWindow<GUIStaticText>("ManorInfo-Value-Value");

		getWindow("ManorInfo-Ground-Name")->SetText(getString(LanguageKey::GUI_MANOR_INFO_GROUND));
		getWindow("ManorInfo-Rose-Name")->SetText(getString(LanguageKey::GUI_MANOR_INFO_ROSE));
		getWindow("ManorInfo-Seed-Name")->SetText(getString(LanguageKey::GUI_MANOR_INFO_SEED));
		getWindow("ManorInfo-Charm-Name")->SetText(getString(LanguageKey::GUI_MANOR_INFO_CHARM));
		getWindow("ManorInfo-Value-Name")->SetText(getString(LanguageKey::GUI_MANOR_INFO_VALUE));

		m_btnUpgrade->subscribeEvent(EventButtonClick, std::bind(&gui_manorInfo::onClick, this, std::placeholders::_1, ViewId::UPGRADE));
		m_btnTeleport->subscribeEvent(EventButtonClick, std::bind(&gui_manorInfo::onClick, this, std::placeholders::_1, ViewId::TELEPORT));
		m_btnTeleport->SetText(getString(LanguageKey::GUI_MANOR_INFO_BTN_TELEPORT));
		m_btnUpgrade->SetText(StringUtil::Format(getString(LanguageKey::GUI_MANOR_INFO_BTN_UPGRADE).c_str(), 0).c_str());
		m_subscriptionGuard.add(ManorOnUpdateEvent::subscribe(std::bind(&gui_manorInfo::manorOnUpdate, this)));
		
	}

	void gui_manorInfo::onUpdate(ui32 nTimeElapse)
	{
		if (m_manorInfo && Blockman::Instance()->m_pPlayer)
		{
			getWindow<GUIStaticImage>("ManorInfo-Currency-Icon")->SetImage(UICommon::getCurrencyIconByType(m_manorInfo->currencyType).c_str());
			m_manorPreview->SetImage(m_manorInfo->icon.c_str());
			m_manorLevel->SetText(StringUtil::Format("LV %d", m_manorInfo->level).c_str());
			m_groundValue->SetText(m_manorInfo->ground.c_str());
			m_seedValue->SetText(m_manorInfo->seed.c_str());
			m_roseValue->SetText(StringUtil::ToString(m_manorInfo->rose).c_str());
			m_charmValue->SetText(StringUtil::ToString(m_manorInfo->charm).c_str());
			m_valueValue->SetText(StringUtil::ToString(m_manorInfo->value).c_str());
			m_btnCDTime = m_btnCDTime > 0 ? m_btnCDTime - nTimeElapse : 0;
			m_btnUpgrade->SetEnabled(m_manorInfo->level < m_manorInfo->maxLevel && m_manorInfo->isMaster && m_btnCDTime <=0);
			
			if (m_manorInfo->level < m_manorInfo->maxLevel)
			{
				m_btnUpgrade->SetRenderOffset(Vector2(25, 0));
				getWindow("ManorInfo-Currency-Icon")->SetVisible(true);
				m_btnUpgrade->SetText(StringUtil::Format(getString(LanguageKey::GUI_MANOR_INFO_BTN_UPGRADE).c_str(), m_nextManorInfo->price).c_str());
			}
			else
			{
				m_btnUpgrade->SetEnabled(false);
				m_btnUpgrade->SetRenderOffset(Vector2(0, 0));
				getWindow("ManorInfo-Currency-Icon")->SetVisible(false);
				m_btnUpgrade->SetText(getString(LanguageKey::GUI_MANOR_MAX_LEVEL).c_str());
			}
		}
	}

	bool gui_manorInfo::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::TELEPORT:
			if (Blockman::Instance()->m_pPlayer->isOnVehicle())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_CALL_TP_IN_VEHICLE);
				return false;
			}

			if (m_manorInfo && m_manorInfo->isMaster)
			{
				ClientNetwork::Instance()->getSender()->sendManorTeleport();
			}
			else
			{
				RootGuiLayout::Instance()->showMainControl();
				GameClient::CGame::Instance()->resetGame(Blockman::Instance()->m_pPlayer->getPlatformUserId());
			}
			break;
		case ViewId::UPGRADE:
			if (m_nextManorInfo)
			{
				if (m_manorInfo->level < m_manorInfo->maxLevel)
				{
					m_btnCDTime = 10000;
					RootGuiLayout::Instance()->showUpgradeManor();
				}
			}
			break;
		}
		return true;
	}

	bool gui_manorInfo::manorOnUpdate()
	{
		if (Blockman::Instance()->m_pPlayer->m_manor)
		{
			m_manorInfo = Blockman::Instance()->m_pPlayer->m_manor->getInfo();
			m_nextManorInfo = Blockman::Instance()->m_pPlayer->m_manor->getNextInfo();
			m_btnCDTime = 0;
		}
		return true;
	}
}
