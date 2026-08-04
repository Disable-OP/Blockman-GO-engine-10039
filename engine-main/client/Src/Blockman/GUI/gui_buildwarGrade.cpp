#include "gui_buildwarGrade.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "UI/GUISystem.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_buildwarGrade::gui_buildwarGrade()
		: gui_layout("BuildWarGrade.json")
	{
	}

	gui_buildwarGrade::~gui_buildwarGrade()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_buildwarGrade::onLoad()
	{
		for (int i = SBI1; i < SBI_END; ++i)
		{
			auto iter = m_button_star_map.find(i);
			if (iter == m_button_star_map.end())
			{
				String button_name = StringUtil::Format("BuildWarScoce%d", i);
				m_button_star_map[i] = getWindow<GUIStaticImage>(button_name.c_str());
				if (m_button_star_map[i])
				{
					m_button_star_map[i]->subscribeEvent("WindowTouchDown", std::bind(&gui_buildwarGrade::onClick, this, std::placeholders::_1, (StarButtonId)i));
				}
			}
		}
		m_subscriptionGuard.add(ShowBuildWarGrade::subscribe(std::bind(&gui_buildwarGrade::showBuildWarGrade, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_buildwarGrade::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{

		}
	}

	void gui_buildwarGrade::onShow()
	{
		for (auto btn : m_button_star_map)
		{
			String image = StringUtil::Format("set:buildwar.json image:star%dnormal", btn.first);
			btn.second->SetImage(image.c_str());
		}

		getWindow<GUIStaticText>("BuildWarGradeTxt")->SetText("");
	}

	bool gui_buildwarGrade::onClick(const EventArgs & events, StarButtonId starButtonId)
	{
		// SoundSystem::Instance()->playEffectByType(SoundType((int)ST_BuildWarGrade1 + starButtonId - 1));
		for (auto btn : m_button_star_map)
		{
			String image = StringUtil::Format("set:buildwar.json image:star%dnormal", btn.first);
			btn.second->SetImage(image.c_str());
		}

		auto iter = m_button_star_map.find(starButtonId);
		if (iter != m_button_star_map.end())
		{
			String image = StringUtil::Format("set:buildwar.json image:star%dpress", starButtonId);
			iter->second->SetImage(image.c_str());
		}

		ClientNetwork::Instance()->getSender()->sendGradeScore((int)starButtonId);

		return true;
	}

	bool gui_buildwarGrade::showBuildWarGrade(const String & msg, ui8 left_time)
	{
		if (isShown())
		{
			String show_msg = StringUtil::Format(getString(LanguageKey::TOOL_BAR_BUILDWAR_GRADE).c_str(), msg.c_str(), left_time);
			getWindow<GUIStaticText>("BuildWarGradeTxt")->SetText(show_msg.c_str());
		}
		return true;
	}

}
