#include "GUIChestRewardResult.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/ChestLotterySetting.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{
	GUIChestRewardResult::GUIChestRewardResult() :
		gui_layout("ChestRewardResult.json")
	{
	}

	GUIChestRewardResult::~GUIChestRewardResult()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIChestRewardResult::onLoad()
	{
		GUILayout* ChestRewardResult = getWindow<GUILayout>("ChestRewardResult");
		ChestRewardResult->subscribeEvent(EventWindowTouchUp, std::bind(&GUIChestRewardResult::onContentClick, this, std::placeholders::_1));

		m_chest = getWindow<GuiActorWindow>("ChestRewardResult-Chest");
		m_content = getWindow<GUILayout>("ChestRewardResult-Content");

		auto Title = getWindow<GUIStaticText>("ChestRewardResult-Title");
		Title->SetText(getString(LanguageKey::GUI_CHEST_REWARD_RESULT_TITLE));

		auto Continue = getWindow<GUIButton>("ChestRewardResult-Continue");
		Continue->SetText(getString(LanguageKey::GUI_BTN_CLICK_CONTINUE));
		Continue->subscribeEvent(EventButtonClick, std::bind(&GUIChestRewardResult::onContinueClick, this, std::placeholders::_1));

		m_image = getWindow<GUIStaticImage>("ChestRewardResult-Image");
		m_name = getWindow<GUIStaticText>("ChestRewardResult-Name");

		m_subscriptionGuard.add(ShowChestRewardResultEvent::subscribe(std::bind(&GUIChestRewardResult::onShowChestRewardResult, this, std::placeholders::_1)));
		m_subscriptionGuard.add(OpenLotteryChestEvent::subscribe(std::bind(&GUIChestRewardResult::onOpenLotteryChest, this, std::placeholders::_1)));
	}

	void GUIChestRewardResult::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;

		if (m_wait_open_time > 0)
		{
			m_wait_open_time -= nTimeElapse;
			if (m_wait_open_time <= 0)
			{
				m_wait_open_time = 0;
				m_content->SetVisible(true);
				m_chest->SetVisible(false);
			}
		}
	}

	bool GUIChestRewardResult::onOpenLotteryChest(int chestId)
	{
		LotteryChest* chest = ChestLotterySetting::findLotteryChestById(chestId);
		if (!chest)
			return false;
		ClientNetwork::Instance()->getSender()->sendOpenLotteryChest(chestId);
		if (chest->Actor.length() <= 7)
			return false;
		m_content->SetVisible(false);
		m_chest->SetVisible(true);
		m_chest->SetActor(chest->Actor.c_str(), "idle");
		return true;
	}

	bool GUIChestRewardResult::onShowChestRewardResult(int id)
	{
		LotteryChestReward* reward = ChestLotterySetting::findLotteryChestRewardById(id);
		if (reward == nullptr) 
		{
			hide();
			return false;
		}

		if (m_chest->IsVisible())
		{
			m_wait_open_time = 2280;
			m_chest->PlaySkill("open");
		}	
		else
		{
			m_content->SetVisible(true);
		}	

		m_image->SetImage(reward->Image.c_str());
		m_name->SetText(getString(reward->Name));
		show();
		return true;
	}

	bool GUIChestRewardResult::onContentClick(const EventArgs & events)
	{
		if (m_wait_open_time > 0)
			return false;
		hide();
		return true;
	}

	bool GUIChestRewardResult::onContinueClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		hide();
		return true;
	}

}
