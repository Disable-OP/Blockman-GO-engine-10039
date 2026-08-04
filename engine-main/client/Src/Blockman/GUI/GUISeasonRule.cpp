#include "GUISeasonRule.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SeasonSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Network/ClientHttpRequest.h"

namespace BLOCKMAN
{
	GUISeasonRule::GUISeasonRule() :
		gui_layout("SeasonRule.json")
	{
	}

	GUISeasonRule::~GUISeasonRule()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUISeasonRule::onLoad()
	{
		auto SeasonRule = getWindow("SeasonRule");
		SeasonRule->subscribeEvent(EventWindowTouchUp, std::bind(&GUISeasonRule::onContentClick, this, std::placeholders::_1));

		getWindow("SeasonRule-Title")->SetText(getString(LanguageKey::GUI_SEASON_RULE_TITLE));
		getWindow("SeasonRule-Bottom-Hint")->SetText(getString(LanguageKey::GUI_SEASON_RULE_BOTTOM_HINT));
		getWindow("SeasonRule-Rule-Text")->SetText(getString(LanguageKey::GUI_SEASON_RULE_CONTENT));

		Season* bronze = SeasonSetting::findSeasonByHonorId(SeasonHonorId::BRONZE);
		if (bronze != NULL)
		{
			auto HonorIconLv1 = getWindow<GUIStaticImage>("SeasonRule-Honor-Lv1-Icon");
			auto HonorNameLv1 = getWindow("SeasonRule-Honor-Lv1-Name");
			auto HonorValueLv1 = getWindow("SeasonRule-Honor-Lv1-Value-Text");
			HonorIconLv1->SetImage(bronze->HonorIcon.c_str());
			HonorNameLv1->SetText(getString(bronze->HonorName));
			HonorValueLv1->SetText(StringUtil::ToString(bronze->Score).c_str());
		}

		Season* silver = SeasonSetting::findSeasonByHonorId(SeasonHonorId::SILVER);
		if (silver != NULL)
		{
			auto HonorIconLv2 = getWindow<GUIStaticImage>("SeasonRule-Honor-Lv2-Icon");
			auto HonorNameLv2 = getWindow("SeasonRule-Honor-Lv2-Name");
			auto HonorValueLv2 = getWindow("SeasonRule-Honor-Lv2-Value-Text");
			HonorIconLv2->SetImage(silver->HonorIcon.c_str());
			HonorNameLv2->SetText(getString(silver->HonorName));
			HonorValueLv2->SetText(StringUtil::ToString(silver->Score).c_str());
		}

		Season* gold = SeasonSetting::findSeasonByHonorId(SeasonHonorId::GOLD);
		if (gold != NULL)
		{
			auto HonorIconLv3 = getWindow<GUIStaticImage>("SeasonRule-Honor-Lv3-Icon");
			auto HonorNameLv3 = getWindow("SeasonRule-Honor-Lv3-Name");
			auto HonorValueLv3 = getWindow("SeasonRule-Honor-Lv3-Value-Text");
			HonorIconLv3->SetImage(gold->HonorIcon.c_str());
			HonorNameLv3->SetText(getString(gold->HonorName));
			HonorValueLv3->SetText(StringUtil::ToString(gold->Score).c_str());
		}

		Season* jewel = SeasonSetting::findSeasonByHonorId(SeasonHonorId::JEWEL);
		if (jewel != NULL)
		{
			auto HonorIconLv4 = getWindow<GUIStaticImage>("SeasonRule-Honor-Lv4-Icon");
			auto HonorNameLv4 = getWindow("SeasonRule-Honor-Lv4-Name");
			auto HonorValueLv4 = getWindow("SeasonRule-Honor-Lv4-Value-Text");
			HonorIconLv4->SetImage(jewel->HonorIcon.c_str());
			HonorNameLv4->SetText(getString(jewel->HonorName));
			HonorValueLv4->SetText(StringUtil::ToString(jewel->Score).c_str());
		}

		Season* king = SeasonSetting::findSeasonByHonorId(SeasonHonorId::KING);
		if (king != NULL)
		{
			auto HonorIconLv5 = getWindow<GUIStaticImage>("SeasonRule-Honor-Lv5-Icon");
			auto HonorNameLv5 = getWindow("SeasonRule-Honor-Lv5-Name");
			auto HonorValueLv5 = getWindow("SeasonRule-Honor-Lv5-Value-Text");
			HonorIconLv5->SetImage(king->HonorIcon.c_str());
			HonorNameLv5->SetText(getString(king->HonorName));
			HonorValueLv5->SetText(StringUtil::ToString(king->Score).c_str());
		}

		if (GameRuleSetting::getGameRule()->isHasSeason)
		{
			ClientHttpRequest::loadSeasonRule();
		}
		m_subscriptionGuard.add(UpdateSeasonRuleEvent::subscribe(std::bind(&GUISeasonRule::onUpdateSeasonRule, this, std::placeholders::_1)));
	}

	void GUISeasonRule::onUpdate(ui32 nTimeElapse)
	{

	}

	bool GUISeasonRule::onContentClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUISeasonRule::onUpdateSeasonRule(const String & data)
	{
		SeasonSetting::parseSeasonRuleData(data);
		Season* bronze = SeasonSetting::findSeasonByHonorId(SeasonHonorId::BRONZE);
		if (bronze != NULL)
		{
			auto HonorValueLv1 = getWindow("SeasonRule-Honor-Lv1-Value-Text");
			HonorValueLv1->SetText(StringUtil::ToString(bronze->Score).c_str());
		}
		Season* silver = SeasonSetting::findSeasonByHonorId(SeasonHonorId::SILVER);
		if (silver != NULL)
		{
			auto HonorValueLv2 = getWindow("SeasonRule-Honor-Lv2-Value-Text");
			HonorValueLv2->SetText(StringUtil::ToString(silver->Score).c_str());
		}
		Season* gold = SeasonSetting::findSeasonByHonorId(SeasonHonorId::GOLD);
		if (gold != NULL)
		{
			auto HonorValueLv3 = getWindow("SeasonRule-Honor-Lv3-Value-Text");
			HonorValueLv3->SetText(StringUtil::ToString(gold->Score).c_str());
		}
		Season* jewel = SeasonSetting::findSeasonByHonorId(SeasonHonorId::JEWEL);
		if (jewel != NULL)
		{
			auto HonorValueLv4 = getWindow("SeasonRule-Honor-Lv4-Value-Text");
			HonorValueLv4->SetText(StringUtil::ToString(jewel->Score).c_str());
		}
		Season* king = SeasonSetting::findSeasonByHonorId(SeasonHonorId::KING);
		if (king != NULL)
		{
			auto HonorValueLv5 = getWindow("SeasonRule-Honor-Lv5-Value-Text");
			HonorValueLv5->SetText(StringUtil::ToString(king->Score).c_str());
		}
		return true;
	}

}
