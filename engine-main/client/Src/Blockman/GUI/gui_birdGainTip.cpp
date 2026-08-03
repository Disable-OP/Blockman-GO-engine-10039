#include "gui_birdGainTip.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIWindowManager.h"
#include "World/BirdSimulator.h"
#include "Actor/ActorObject.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_birdGainTip::gui_birdGainTip()
		: gui_layout("BirdGainTip.json")
	{
	}

	gui_birdGainTip::~gui_birdGainTip()
	{
	}

	void gui_birdGainTip::onHide()
	{
		m_leftTime = 0;
		m_shakeAnimationTime = 0;
		m_openAnimationTime = 0;
		m_actor->SetSkillName("");
	}

	void gui_birdGainTip::onLoad()
	{

		setText("BirdGainTip-Title", "gui_bird_gain_title");
		m_listView = getWindow<GUIListHorizontalBox>("BirdGainTip-Items");
		m_commonTip = getWindow<GUILayout>("BirdGainTip-Common");
		m_lotteryTip = getWindow<GUILayout>("BirdGainTip-Lottery");
		m_actor = getWindow<GuiActorWindow>("BirdGainTip-Actor");
		m_birdActor = getWindow<GuiActorWindow>("BirdGainTip-BirdActor");
		m_subscriptionGuard.add(ShowBirdGainTipEvent::subscribe(std::bind(&gui_birdGainTip::showTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBirdLotteryResultTipEvent::subscribe(std::bind(&gui_birdGainTip::showLotteryTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	}

	void gui_birdGainTip::onUpdate(ui32 nTimeElapse)
	{
		if (m_leftTime > 0 && isShown())
		{
			m_leftTime = m_leftTime - nTimeElapse > 0 ? m_leftTime - nTimeElapse : 0.f;
			if (m_commonTip->IsVisible())
			{
				float h = (m_leftTime / m_showTime) - 1.f;
				m_commonTip->SetArea(UDim(0, 0), UDim(h, 0), UDim(0.7703f, 0), UDim(0.3041f, 0));
			}
			
			if (m_leftTime <= 0)
			{
				RootGuiLayout::Instance()->setBirdGainTipVisible(false);
			}
		}

		if (m_shakeAnimationTime > 0 && isShown() && m_lotteryTip->IsVisible())
		{
			m_shakeAnimationTime = m_shakeAnimationTime - nTimeElapse > 0 ? m_shakeAnimationTime - nTimeElapse : 0;
			if (m_shakeAnimationTime <= 0)
			{
				m_actor->UseBodyPart("body", "");
				m_actor->SetSkillName("open");
				m_openAnimationTime = m_actor->GetActor()->GetSkillTimeLength("open");
			}
		}

		if (m_openAnimationTime > 0 && isShown() && m_lotteryTip->IsVisible())
		{
			m_openAnimationTime = m_openAnimationTime - nTimeElapse > 0 ? m_openAnimationTime - nTimeElapse : 0;
			if (m_openAnimationTime <= 0)
			{
				UpdateBirdLotteryEvent::emit();
				m_birdActor->SetVisible(true);
				m_leftTime = 2500.f;
			}
		}
	}

	bool gui_birdGainTip::showTip(const vector<BirdGain>::type & items)
	{
		initItem(items);
		m_leftTime = 2500.f;
		m_commonTip->SetVisible(true);
		m_lotteryTip->SetVisible(false);
		m_commonTip->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.7703f, 0), UDim(0.3041f, 0));
		RootGuiLayout::Instance()->setBirdGainTipVisible(true);
		return true;
	}


	bool gui_birdGainTip::showLotteryTip(const String & eggActorName, const String & eggActorBodyId, const BirdInfo & birdInfo)
	{
		m_commonTip->SetVisible(false);
		m_lotteryTip->SetVisible(true);
		m_birdActor->SetVisible(false);
		m_actor->SetVisible(true);
		m_actor->SetActor(eggActorName.c_str(), "shake");
		m_actor->UseBodyPart("body", eggActorBodyId);
		m_actor->SetSkillName("shake");
		m_shakeAnimationTime = m_actor->GetActor()->GetSkillTimeLength("shake");
		m_birdActor->SetActor("g1041_bird.actor");
		m_birdActor->UseBodyPart("hat", birdInfo.hatId);
		m_birdActor->UseBodyPart("tail", birdInfo.tailId);
		m_birdActor->UseBodyPart("beak", birdInfo.beakId);
		m_birdActor->UseBodyPart("wing", birdInfo.wingId);
		m_birdActor->UseBodyPart("body", birdInfo.bodyId);
		m_birdActor->UseBodyPart("tail_effect", birdInfo.effectId);
		m_birdActor->UseBodyPart("glasses", birdInfo.glassesId);
		RootGuiLayout::Instance()->setBirdGainTipVisible(true);
		return true;
	}

	void gui_birdGainTip::initItem(const vector<BirdGain>::type & items)
	{
		m_listView->ClearAllItem();
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("BirdGainTip-Items-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GUILayout* birdItemLayout= (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, itemLayout.c_str());
			birdItemLayout->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 152.f), UDim(0.0f, 108.f));
			birdItemLayout->SetTouchable(false);

			String strIcon = StringUtil::Format("%s-Icon", itemLayout.c_str());
			GUIStaticImage* birdItemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strIcon.c_str());
			birdItemIcon->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 102.f), UDim(0.0f, 108.f));
			birdItemIcon->SetTouchable(false);
			birdItemIcon->SetImage(item.icon.c_str());
			birdItemLayout->AddChildWindow(birdItemIcon);

			String strNum = StringUtil::Format("%s-Num", itemLayout.c_str());
			GUIStaticText* birdItemNum = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strNum.c_str());
			birdItemNum->SetArea(UDim(0, 102.f), UDim(0, 0.0f), UDim(0.0f, 50.f), UDim(0.0f, 108.f));
			birdItemNum->SetText(StringUtil::Format("x %d", item.num).c_str());
			birdItemNum->SetTouchable(false);
			birdItemNum->SetTextHorzAlign(HA_CENTRE);
			birdItemNum->SetTextVertAlign(VA_CENTRE);
			birdItemNum->SetProperty(GWP_TEXT_SHADOW, "true");
			birdItemLayout->AddChildWindow(birdItemNum);

			m_listView->AddItem(birdItemLayout, false);
		}
		m_listView->setContainerAlignment();
	}


}
