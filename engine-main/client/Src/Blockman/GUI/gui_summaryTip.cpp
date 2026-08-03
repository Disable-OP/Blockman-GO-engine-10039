#include "gui_summaryTip.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"

using namespace LORD;
namespace BLOCKMAN
{

	gui_summaryTip::gui_summaryTip()
		: gui_layout("SummaryRewardTip.json")
	{
	}

	gui_summaryTip::~gui_summaryTip()
	{
	}

	void gui_summaryTip::onLoad()
	{
	}

	void gui_summaryTip::onShow()
	{

	}

	void gui_summaryTip::onUpdate(ui32 nTimeElapse)
	{

	}

	void gui_summaryTip::showContent(const LORD::UVector2& pos, int total)
	{
		m_rootWindow->SetPosition(pos);
		GUIStaticText* pTip1 = getWindow<GUIStaticText>("SummaryRewardTip-Tip1");
		String tip = StringUtil::Format(getString(LanguageKey::GUI_SUMMARY_REWARD_TIP_FIRST).c_str(), total);
		pTip1->SetText(tip.c_str());
		GUIStaticText* pTip2 = getWindow<GUIStaticText>("SummaryRewardTip-Tip2");
		pTip2->SetText(getString(LanguageKey::GUI_SUMMARY_REWARD_TIP_SECOND).c_str());
	}
}
