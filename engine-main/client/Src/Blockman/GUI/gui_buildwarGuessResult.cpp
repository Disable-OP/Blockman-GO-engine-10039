#include "gui_buildwarGuessResult.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "UI/GUISystem.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_buildwarGuessResult::gui_buildwarGuessResult()
		: gui_layout("BuildWarGuessResult.json")
	{
	}

	gui_buildwarGuessResult::~gui_buildwarGuessResult()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_buildwarGuessResult::onLoad()
	{
		getWindow<GUIStaticText>("BuildWar-GuessResultBtnText")->SetText(getString(LanguageKey::GUI_BUILD_WAR_GUESS_RESULT_BUTTON));
		m_subscriptionGuard.add(ShowBuildWarGuessResult::subscribe(std::bind(&gui_buildwarGuessResult::showBuildGuessResult, this, std::placeholders::_1, std::placeholders::_2)));
		getWindow<GUIButton>("BuildWar-ResultButton")->subscribeEvent(EventButtonClick, std::bind(&gui_buildwarGuessResult::onClick, this, std::placeholders::_1));

		getWindow<GUIStaticText>("BuildWar-MyRankTitle")->SetText(getString(LanguageKey::GUI_BUILD_WAR_MY_RANK_TITLE));
		getWindow<GUIStaticText>("BuildWar-MyGuessTitle")->SetText(getString(LanguageKey::GUI_BUILD_WAR_MY_GUESS_TITLE));

		for (int i = 1; i <= 64; ++i)
		{
			String layout_name = StringUtil::Format("BuildWarGuessResult-EffectLayout%d", i);
			GUILayout* layout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, layout_name.c_str());
			if (layout)
			{
				Random m_Rand;
				float random_x = m_Rand.nextIntRange(25, 75) * 1.0f / 100;
				float random_y = m_Rand.nextIntRange(10, 70) * 1.0f / 100;
				layout->SetPosition(UVector2(UDim(random_x, 0), UDim(random_y, 0)));
				layout->SetSize(UVector2(UDim(0, 50.0f), UDim(0, 50.0f)));
				layout->SetTouchable(false);
				layout->SetVisible(false);
				
				getWindow<GUILayout>("BuildWarGuessResult")->AddChildWindow(layout);
				m_effectLayout.push_back(layout);
			}
		}
	}

	void gui_buildwarGuessResult::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_curEffectIndex >= 0 && m_curEffectIndex < (int)m_effectLayout.size())
			{
				if (m_effectTime > 0)
				{
					m_effectTime -= nTimeElapse;
				}
				else
				{
					m_effectLayout[m_curEffectIndex]->SetVisible(true);
					switch (m_curEffectIndex % 4)
					{
					case 0:
						m_effectLayout[m_curEffectIndex]->SetEffectName("fireworks_blue01.effect");
						break;
					case 1:
						m_effectLayout[m_curEffectIndex]->SetEffectName("fireworks_green01.effect");
						break;
					case 2:
						m_effectLayout[m_curEffectIndex]->SetEffectName("fireworks_violet01.effect");
						break;
					case 3:
						m_effectLayout[m_curEffectIndex]->SetEffectName("fireworks_yellow01.effect");
						break;
					default:
						m_effectLayout[m_curEffectIndex]->SetEffectName("fireworks_blue01.effect");
						break;
					}

					m_curEffectIndex++;
					m_effectTime = effectInterval;
				}
			}
		}
		else
		{
			if (m_curEffectIndex > 0)
			{
				if (!getParent()->isChatShown() && !getParent()->isMenuShown() && !getParent()->isAppShopShown() && !getParent()->isFinalSummaryShown() && !getParent()->isLoading())
				{
					getParent()->showBuildWarGuessResult(true);
				}
			}
		}
	}

	void gui_buildwarGuessResult::onShow()
	{
		m_effectTime = effectInterval;
	}

	bool gui_buildwarGuessResult::onClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendBuildWarResult();
		return true;
	}

	bool gui_buildwarGuessResult::showBuildGuessResult(bool right, int rank)
	{
		getWindow<GUIStaticText>("BuildWarGuessResult-MyRank")->SetText(StringUtil::ToString(rank).c_str());

		if (right)
		{
			getWindow<GUIStaticImage>("BuildWarGuessResult-BuildWar-MyGuess")->SetImage("set:buildwar.json image:gou");
		}
		else
		{
			getWindow<GUIStaticImage>("BuildWarGuessResult-BuildWar-MyGuess")->SetImage("set:buildwar.json image:cha");
		}
		
		return true;
	}

}
