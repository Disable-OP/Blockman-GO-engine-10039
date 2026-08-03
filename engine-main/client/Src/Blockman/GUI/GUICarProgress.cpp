#include "GUICarProgress.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"
#include "Setting/TeamInfoSetting.h"
#include "game.h"

namespace BLOCKMAN
{
	GUICarProgress::GUICarProgress(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("CarProgress.json"));
		renameComponents(m_window);
		m_thumb = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUICarProgress::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUICarProgress::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUICarProgress::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUICarProgress::InitTeamIcon()
	{
		ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
		auto team1Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		const TeamInfo& team1Info = TeamInfoSetting::getTeamInfo((i32)gameType, 1);
		team1Icon->SetImage(team1Info.TeamIcon.c_str());
		auto team2Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		const TeamInfo& team2Info = TeamInfoSetting::getTeamInfo((i32)gameType, 2);
		team2Icon->SetImage(team2Info.TeamIcon.c_str());
	}

	void GUICarProgress::SetStyle(CarProgressStyle style)
	{
		InitTeamIcon();
		m_style = style;
		auto progress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(2));
		auto thumb = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		progress->SetBackImage("set:car_progress.json image:car_progress_bg");
		String direction = m_direction == Left ? "left" : "right";
		switch (style)
		{
			case Yellow:
				progress->SetProgressImage("set:car_progress.json image:car_progress_yellow");
				thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_yellow_car", direction.c_str()).c_str());
				break;
			case Green:
				progress->SetProgressImage("set:car_progress.json image:car_progress_green");
				thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_green_car", direction.c_str()).c_str());
				break;
			case Red:
				progress->SetProgressImage("set:car_progress.json image:car_progress_red");
				thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_red_car", direction.c_str()).c_str());
				break;
			default:
				break;
		}
	}

	void GUICarProgress::SetDirection(CarProgressDirection direction)
	{
		m_direction = direction;
		String s_direction = m_direction == Left ? "left" : "right";
		switch (m_style)
		{
		case Yellow:
			m_thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_yellow_car", s_direction.c_str()).c_str());
			break;
		case Green:
			m_thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_green_car", s_direction.c_str()).c_str());
			break;
		case Red:
			m_thumb->SetImage(StringUtil::Format("set:car_progress.json image:%s_red_car", s_direction.c_str()).c_str());
			break;
		default:
			break;
		}
	}

	void GUICarProgress::SetProgress(float progress)
	{
		float startX = 33.0f;
		m_thumb->SetXPosition({ 0, startX + progress });
	}

}
