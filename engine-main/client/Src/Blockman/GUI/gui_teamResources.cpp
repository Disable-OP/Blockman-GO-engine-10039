#include "gui_teamResources.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "ShellInterface.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Setting/TeamInfoSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/UIDisplaySetting.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_teamResources::gui_teamResources() :
		gui_layout("TeamResources.json")
	{

	}

	gui_teamResources::~gui_teamResources()
	{	
		m_subscriptionGuard.unsubscribeAll();
		m_teamResourcesView.clear();
		m_teamResources.clear();
	}

	void gui_teamResources::onLoad()
	{
		m_maxResourceTeam = getWindow<GUILayout>("TeamResources-MaxResourcesTeam");
		m_resouceContainer = getWindow<GUILayout>("TeamResources-Container");
		m_teamResourceAll = getWindow<GUILayout>("TeamResources-TeamResources");
		m_maxResourcesTeamText = getWindow<GUIStaticText>("TeamResources-MaxResourcesTeam-Name");
		m_otherTeamResourceText = getWindow<GUIStaticText>("TeamResources-OtherTeamResourceTitle");
		m_myTeamResourceText = getWindow<GUIStaticText>("TeamResources-MyTeamResourceTitle");
		m_notificationText = getWindow<GUIStaticText>("TeamResources-Notification");

		m_otherTeamResourceText->SetText(getString(LanguageKey::GUI_TEAM_RESOURCES_OTHER_TEAM_POOL));
		m_myTeamResourceText->SetText(getString(LanguageKey::GUI_TEAM_RESOURCES_MY_TEAM_POOL));
		m_maxResourcesTeamText->SetText(getString(LanguageKey::GUI_TEAM_RESOURCES_MY_TEAM_POOL));

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SnowmanBattle)
		{
			m_otherTeamResourceText->SetText(getString(LanguageKey::GUI_TEAM_SNOWMANHP_OTHER_TEAM_POOL));
			m_myTeamResourceText->SetText(getString(LanguageKey::GUI_TEAM_SNOWMANHP_MY_TEAM_POOL));
			m_maxResourcesTeamText->SetText(getString(LanguageKey::GUI_TEAM_SNOWMANHP_MY_TEAM_POOL));
		}

		m_maxResourcesTeamInfo = getWindow<GuiTeamResourcesItem>("TeamResources-MaxResourcesTeam-Info");
		m_myTeamResourcesInfo = getWindow<GuiTeamResourcesItem>("TeamResources-MyTeamResources");

		m_teamResourcesView.push_back(getWindow<GuiTeamResourcesItem>("TeamResources-OtherTeamResources_1"));
		m_teamResourcesView.push_back(getWindow<GuiTeamResourcesItem>("TeamResources-OtherTeamResources_2"));
		m_teamResourcesView.push_back(getWindow<GuiTeamResourcesItem>("TeamResources-OtherTeamResources_3"));

		m_maxResourceTeam->SetVisible(false);
		m_teamResourceAll->SetVisible(false);
		m_notificationText->SetText("");

		m_subscriptionGuard.add(TeamResourcesUpdateEvent::subscribe(std::bind(&gui_teamResources::onTeamResourcesUpdate, this, std::placeholders::_1)));
		m_subscriptionGuard.add(TeamResourcesShowNotificationEvent::subscribe(std::bind(&gui_teamResources::showNotification, this, std::placeholders::_1, std::placeholders::_2)));

	}

	void gui_teamResources::onUpdate(ui32 nTimeElapse)
	{
		m_keepShowNotificationTime = m_keepShowNotificationTime > 0 ? m_keepShowNotificationTime - nTimeElapse : 0;
		m_notificationText->SetVisible(m_keepShowNotificationTime > 0);
		float positionY = RootGuiLayout::Instance()->canPersonalShop() ? 118.0f : 55.0f;
		if (m_recordPosY != positionY || m_isResourceModify)
		{
			m_recordPosY = positionY;
			m_isResourceModify = false;
			if (m_teamResources.empty())
			{
				m_resouceContainer->SetArea(UDim(0.f, -2.0f), UDim(0.f, m_recordPosY), UDim(0.f, 184.0f), UDim(0.f, 72.0f));
			}
			else
			{
				float height = 100.0f + 45 * (m_teamResources.size() - 1);
				m_resouceContainer->SetArea(UDim(0.f, -2.0f), UDim(0.f, m_recordPosY), UDim(0.f, 184.0f), UDim(0.f, height));
			}
		}
	}

	bool gui_teamResources::onTeamResourcesUpdate(const String & data)
	{
		m_isResourceModify = true;
		m_maxResourceTeam->SetVisible(UIDisplaySetting::getUIDisplay()->isShowCenterTeamResource);
		m_teamResourceAll->SetVisible(true);
		size_t oldSize = m_teamResources.size();
		m_teamResources.clear();
		Document doc = Document();
		doc.Parse(data.c_str());
		if (doc.HasParseError())
		{
			LordLogError("data parse error : %s", data.c_str());
			return false;
		}
		auto content = doc.GetArray();
		for (const auto& item : content)
		{
			if (!item.HasMember("teamId") || !item.HasMember("value") || !item.HasMember("maxValue"))
			{
				LordLogError("data parse error : %s", data.c_str());
				continue;
			}

			i32 teamId = item.FindMember("teamId")->value.GetInt();
			i32 value = item.FindMember("value")->value.GetInt();
			i32 maxValue = item.FindMember("maxValue")->value.GetInt();

			GuiTeamResourcesItem::TeamResourcesItem resourcesItem = initTeamResources(teamId, value, maxValue);
			m_teamResources.push_back(resourcesItem);

			if (Blockman::Instance()->m_pPlayer->getTeamId() == resourcesItem.m_teamId)
			{
				m_myTeamResourcesInfo->setTeamResourcesItem(resourcesItem);
				m_maxResourcesTeamInfo->setTeamResourcesItem(resourcesItem);
			}
			else
			{
				setTeamResourcesItem(resourcesItem);
			}
		}

		return true;
	}

	GuiTeamResourcesItem::TeamResourcesItem gui_teamResources::initTeamResources(i32 teamId, i32 value, i32 maxValue)
	{
		ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
		const TeamInfo& teamInfo = TeamInfoSetting::getTeamInfo((i32)gameType, teamId);
		GuiTeamResourcesItem::TeamResourcesItem item = GuiTeamResourcesItem::TeamResourcesItem();
		item.m_teamId = teamId;
		item.m_teamIcon = teamInfo.TeamIcon.c_str();
		item.m_maxValue = maxValue;
		item.m_teamName = getString(teamInfo.TeamName.c_str()).c_str();
		item.m_value = value;
		switch (teamId)
		{
		case 1:
			item.m_progressNor = "set:jewel_knight.json image:progress_bar_red_nor";
			item.m_progressPre = "set:jewel_knight.json image:progress_bar_red_pre";
			break;
		case 2:
			item.m_progressNor = "set:jewel_knight.json image:progress_bar_blue_nor";
			item.m_progressPre = "set:jewel_knight.json image:progress_bar_blue_pre";
			break;
		case 3:
			item.m_progressNor = "set:jewel_knight.json image:progress_bar_green_nor";
			item.m_progressPre= "set:jewel_knight.json image:progress_bar_green_pre";
			break;
		case 4:
			item.m_progressNor = "set:jewel_knight.json image:progress_bar_yellow_nor";
			item.m_progressPre = "set:jewel_knight.json image:progress_bar_yellow_pre";
			break;
		}

		return item;
	}

	void gui_teamResources::setMaxResourcesTeamInfo()
	{
		GuiTeamResourcesItem::TeamResourcesItem maxItem;
		for (auto item : m_teamResources)
		{
			if (item.m_value > maxItem.m_value)
			{
				maxItem = item;
			}
		}

		if (maxItem.m_teamId > 0)
		{
			m_teamResourceAll->SetVisible(true);
			m_maxResourcesTeamInfo->setTeamResourcesItem(maxItem);
			m_maxResourcesTeamText->SetText(getString(LanguageKey::GUI_TEAM_RESOURCES_MY_TEAM_POOL));
		}
		else
		{
			m_teamResourceAll->SetVisible(false);
			m_maxResourcesTeamText->SetText("");
		}
	}

	void gui_teamResources::setTeamResourcesItem(GuiTeamResourcesItem::TeamResourcesItem item)
	{
		for (auto view : m_teamResourcesView)
		{
			if (view->getTeamId() == 0 || view->getTeamId() == item.m_teamId)
			{
				view->setTeamResourcesItem(item);
				break;
			}
		}
	}
	bool gui_teamResources::showNotification(const String & message, i32 keepTime)
	{
		m_notificationText->SetText(message.c_str());
		m_notificationText->SetVisible(true);
		m_notificationText->SetTouchable(false);
		m_notificationText->SetTextScale(1.2f);
		m_keepShowNotificationTime = keepTime * 1000;
		return true;
	}
}
