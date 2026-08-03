#include "gui_lotteryChestDetail.h"
#include "UI/GUIButton.h"
#include "GUI/GUIChestRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	gui_lotteryChestDetail::gui_lotteryChestDetail() :
		gui_layout("LotteryChestDetail.json")
	{
		m_chest_reward_detail = m_chest_reward_detail ? m_chest_reward_detail : LordNew GUIChestRewardDetail;
	}

	gui_lotteryChestDetail::~gui_lotteryChestDetail()
	{
		LordSafeDelete(m_chest_reward_detail);
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_lotteryChestDetail::onLoad()
	{
		GUILayout* ChestDetail = getWindow<GUILayout>("LotteryChestDetail");
		m_chest_reward_detail->attachTo(this, ChestDetail);
		m_chest_reward_detail->hide();

		m_title = getWindow<GUIStaticText>("LotteryChestDetail-Title");
		m_image = getWindow<GUIStaticImage>("LotteryChestDetail-Image");
		m_integral_icon = getWindow<GUIStaticImage>("LotteryChestDetail-Integral-Icon");
		m_integral_text = getWindow<GUIStaticText>("LotteryChestDetail-Integral-Text");

		auto Close = getWindow<GUIButton>("LotteryChestDetail-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_lotteryChestDetail::onCloseClick, this, std::placeholders::_1));

		auto RewardItems = getWindow("LotteryChestDetail-Reward-Items");
		if (RewardItems == NULL)
			return;
		map<int, LotteryChest*>::type chests = ChestLotterySetting::getLotteryChests();
		if (chests.empty())
			return;
		static unsigned index = 0;
		for (auto chest : chests)
		{
			if (m_rewards_map.find(chest.second->Type) != m_rewards_map.end())
				continue;
			index++;
			GUIString ChildsName = StringUtil::Format("LotteryChestDetail-Childs-%d", index).c_str();
			GUIGridView* pChildGV = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
			pChildGV->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
			pChildGV->InitConfig(6.f, 6.f, 8);
			RewardItems->AddChildWindow(pChildGV);
			m_rewards_map[chest.second->Type] = pChildGV;
		}

		m_subscriptionGuard.add(ShowChestRewardDetailEvent::subscribe(std::bind(&gui_lotteryChestDetail::onShowRewardDetail, this, std::placeholders::_1)));
	}

	void gui_lotteryChestDetail::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_lotteryChestDetail::ShowChestDetail(LotteryChestType type)
	{
		for (auto rewards : m_rewards_map)
		{
			GUIGridView* childs = rewards.second;
			if (rewards.first == type)
			{
				show();
				UpdateUIByType(type);
				childs->SetVisible(true);
				size_t count = childs->GetItemCount();
				if (count == 0)
				{
					auto RewardItems = getWindow("LotteryChestDetail-Reward-Items");
					float ContentSize = RewardItems->GetPixelSize().x;
					list<LotteryChestReward*>::type reward_list = ChestLotterySetting::findLotteryChestRewardByType(type);
					static unsigned RewardChildCount = 0;
					float ChildWidth = (ContentSize - 6.0f * 7) / 8;
					for (auto reward : reward_list)
					{
						RewardChildCount++;
						GUIString ChildName = StringUtil::Format("LotteryChestDetail-Reward-Child-%d", RewardChildCount).c_str();
						GUIChestRewardItem* RewardItem = (GUIChestRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_CHEST_REWARD_ITEM, ChildName);
						RewardItem->SetWidth({ 0, ChildWidth });
						RewardItem->SetHeight({ 0, ChildWidth });
						RewardItem->SetLotteryChestReward(reward);
						childs->AddItem(RewardItem);
					}
				}
			}
			else
			{
				childs->SetVisible(false);
			}
		}
	}

	void gui_lotteryChestDetail::UpdateUIByType(LotteryChestType type)
	{
		LotteryChest* data = ChestLotterySetting::findLotteryChestByType(type);
		if (data != nullptr)
		{
			m_title->SetText(getString(data->Name));
			m_image->SetImage(data->Image.c_str());
			m_integral_icon->SetVisible(data->AddIntegral > 0);
			m_integral_text->SetText(StringUtil::ToString(data->AddIntegral).c_str());
		}
	}

	bool gui_lotteryChestDetail::onCloseClick(const EventArgs & events)
	{
		getParent()->hideLotteryChestDetail();
		return true;
	}

	bool gui_lotteryChestDetail::onShowRewardDetail(int id)
	{
		LotteryChestReward* reward = ChestLotterySetting::findLotteryChestRewardById(id);
		if (reward == nullptr)
			return false;
		playSoundByType(ST_Click);
		m_chest_reward_detail->ShowLotteryChestReward(reward);		
		return true;
	}

}
