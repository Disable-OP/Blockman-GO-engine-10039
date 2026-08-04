#include "gui_chestLottery.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "Setting/ChestLotterySetting.h"

namespace BLOCKMAN
{
	gui_chestLottery::gui_chestLottery() :
		gui_layout("ChestLottery.json")
	{
		m_reward_result = m_reward_result ? m_reward_result : LordNew GUIChestRewardResult;
	}

	gui_chestLottery::~gui_chestLottery()
	{
		LordSafeDelete(m_reward_result);
		m_subscriptionGuard.unsubscribeAll();
		ChestLotterySetting::unloadSetting();
	}

	void gui_chestLottery::onLoad()
	{
		ChestLotterySetting::loadSetting();

		GUILayout* ChestLottery = getWindow<GUILayout>("ChestLottery");
		m_reward_result->attachTo(this, ChestLottery);
		m_reward_result->hide();

		auto Title = getWindow<GUIStaticText>("ChestLottery-Title-Text");
		Title->SetText(getString(LanguageKey::GUI_CHEST_LOTTERY_TITLE));
		auto Hint = getWindow<GUIStaticText>("ChestLottery-Open-Hint-Text");
		Hint->SetText(getString(LanguageKey::GUI_CHEST_LOTTERY_HINT));

		auto Close = getWindow<GUIButton>("ChestLottery-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_chestLottery::onCloseClick, this, std::placeholders::_1));
		auto UltimateChest = getWindow<GUIButton>("ChestLottery-Integral-Chest");
		UltimateChest->subscribeEvent(EventButtonClick, std::bind(&gui_chestLottery::onUltimateChestClick, this, std::placeholders::_1));
		auto UltimateChestQuestion = getWindow<GUIButton>("ChestLottery-Integral-Chest-Question");
		UltimateChestQuestion->subscribeEvent(EventButtonClick, std::bind(&gui_chestLottery::onUltimateChestQuestionClick, this, std::placeholders::_1));

		m_integral = getWindow<GUILayout>("ChestLottery-Integral");
		m_integral_progress = getWindow<GUIProgressBar>("ChestLottery-Integral-Progress");
		m_integral_progress_text = getWindow<GUIStaticText>("ChestLottery-Integral-Progress-Text");
		LotteryChest* ultimate_chest = ChestLotterySetting::findLotteryChestByType(ULTIMATE_CHEST);
		if (ultimate_chest != nullptr)
			setIntegralProgress(0, ultimate_chest->NeedIntegral);
		else
			setIntegralProgress(0, 0);

		m_chest_lv1 = getWindow<GUIChestLotteryItem>("ChestLottery-Chest-Lv1");
		m_chest_lv2 = getWindow<GUIChestLotteryItem>("ChestLottery-Chest-Lv2");
		m_chest_lv3 = getWindow<GUIChestLotteryItem>("ChestLottery-Chest-Lv3");
		m_chest_lv1->SetLotteryChest(ChestLotterySetting::findLotteryChestByType(BRONZE_CHEST));
		m_chest_lv2->SetLotteryChest(ChestLotterySetting::findLotteryChestByType(SILVER_CHEST));
		m_chest_lv3->SetLotteryChest(ChestLotterySetting::findLotteryChestByType(GOLD_CHEST));

		m_subscriptionGuard.add(UpdateChestLotteryIntegralEvent::subscribe(std::bind(&gui_chestLottery::setIntegralProgress, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateLotteryChestDataEvent::subscribe(std::bind(&gui_chestLottery::onUpdateLotteryChestData, this, std::placeholders::_1)));
		
	}

	void gui_chestLottery::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_chestLottery::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_chestLottery::onUltimateChestClick(const EventArgs & events)
	{
		LotteryChest* ultimate_chest = ChestLotterySetting::findLotteryChestByType(ULTIMATE_CHEST);
		if (ultimate_chest != nullptr)
		{
			if (ultimate_chest->HasNum == 0 && m_integral_progress->GetProgress() < 1.0f)
			{
				return false;
			}
			SoundSystem::Instance()->playEffectByType(ST_Click);
			OpenLotteryChestEvent::emit(ultimate_chest->Id);
		}
		return true;
	}

	bool gui_chestLottery::onUltimateChestQuestionClick(const EventArgs & events)
	{	
		playSoundByType(ST_Click);
		ShowLotteryChestDetailEvent::emit((int)ULTIMATE_CHEST); 
		return true;
	}

	bool gui_chestLottery::onUpdateLotteryChestData(const String & data)
	{
		ChestLotterySetting::parseUpdateLotteryChestData(data);
		return true;
	}

	bool gui_chestLottery::setIntegralProgress(int curIntegral, int needIntegral)
	{
		if (needIntegral == 0)
		{
			m_integral->SetVisible(false);
			return false;
		}
		m_integral->SetVisible(true);
		m_integral_progress->SetProgress((float)curIntegral / (float)needIntegral);
		LotteryChest* ultimate_chest = ChestLotterySetting::findLotteryChestByType(ULTIMATE_CHEST);
		if (ultimate_chest != nullptr)
		{
			if (ultimate_chest->HasNum > 0)
			{
				m_integral_progress_text->SetText(StringUtil::Format("%d/%d(%d)", curIntegral, needIntegral, ultimate_chest->HasNum).c_str());
			}
			else
			{
				m_integral_progress_text->SetText(StringUtil::Format("%d/%d", curIntegral, needIntegral).c_str());
			}
		}
		else
		{
			m_integral_progress_text->SetText(StringUtil::Format("%d/%d", curIntegral, needIntegral).c_str());
		}
		return true;
	}

}