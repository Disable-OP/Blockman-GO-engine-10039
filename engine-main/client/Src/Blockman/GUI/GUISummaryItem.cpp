#include "GUISummaryItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "UI/GUISystem.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;
namespace BLOCKMAN
{
	vector<GUIWindow*>::type GuiSummaryItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiSummaryItem::GuiSummaryItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_resultEntry = nullptr;
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("SummaryItem.json");
		renameComponents(m_window);
		m_rankBgImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_rankImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_name->subscribeEvent(EventWindowClick, std::bind(&GuiSummaryItem::onNameClick, this, std::placeholders::_1));
		m_rewardIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_reward = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_doubleRank = m_window->GetChildByIndex(6);
		m_doubleRankTen = dynamic_cast<GUIStaticImage*>(m_doubleRank->GetChildByIndex(0));
		m_doubleRankOne = dynamic_cast<GUIStaticImage*>(m_doubleRank->GetChildByIndex(1));
		m_result = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(7));
		m_vipIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(8));
		m_awardMaxIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(9));
		m_score = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(10));
		m_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(11));
		m_btn->subscribeEvent(EventButtonClick, std::bind(&GuiSummaryItem::onClick, this, std::placeholders::_1));
		m_btn->SetVisible(false);
		if (CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town)
		{
			m_btn->SetVisible(true);
			m_btn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_MESSAGE_BTN_CALL_ON).c_str());
		}
		else
		{
			m_btn->SetText("");
			m_btn->SetVisible(false);
		}

		m_visitBtn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(12));
		m_visitBtn->subscribeEvent(EventButtonClick, std::bind(&GuiSummaryItem::onClickVisit, this, std::placeholders::_1));
		m_visitBtn->SetVisible(false);
		if (CGame::Instance()->GetGameType() == ClientGameType::Build_War)
		{
			m_visitBtn->SetVisible(true);
			m_visitBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BUILD_WAR_GUESS_ITEM_VISIT_BUTTON).c_str());
		}

		m_buildWarScore = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(13));
		m_buildWarScore->SetVisible(CGame::Instance()->GetGameType() == ClientGameType::Build_War);

		AddWindowToDrawList(*m_window);
	}

	void GuiSummaryItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiSummaryItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiSummaryItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_rankBgImage = nullptr;
		m_rankImage = nullptr;
		m_name = nullptr;
		m_reward = nullptr;	
		m_doubleRank = nullptr;
		m_doubleRankOne = nullptr;
		m_doubleRankTen = nullptr;
		m_result = nullptr;
		m_vipIcon = nullptr;
		m_awardMaxIcon = nullptr;
	}

	void GuiSummaryItem::RefreshItem(RankData* rankData)
	{
		String	bgImage;
		String	image;
		String  tenImage;
		Color	textColor;
		Color	borderColor;

		GetImageByRank(rankData->rank, bgImage, image, tenImage, textColor, borderColor);
		m_userId = rankData->userId;
		m_rankBgImage->SetImage(bgImage.c_str());
		m_name->SetText(rankData->name.c_str());
		m_name->SetTextColor(textColor);
		m_name->SetTextBoader(borderColor);

		m_score->SetVisible(true);
		m_score->SetText(StringUtil::ToThousandthString(rankData->score).c_str());
		m_score->SetTextColor(textColor);
		m_score->SetTextBoader(borderColor);

		m_visitBtn->SetVisible(false);

		if (rankData->rank > 9)
		{
			m_doubleRank->SetVisible(true);
			m_rankImage->SetVisible(false);
			m_doubleRankTen->SetImage(tenImage.c_str());
			m_doubleRankOne->SetImage(image.c_str());
		}
		else
		{
			m_doubleRank->SetVisible(false);
			m_rankImage->SetVisible(true);
			m_rankImage->SetImage(image.c_str());
		}

		m_result->SetVisible(false);

		String vipIconRes = "";
		switch (rankData->vip)
		{
		case  1:
			vipIconRes = "set:summary.json image:VIP";
			break;
		case  2:
			vipIconRes = "set:summary.json image:VIPPlus";
			break;
		case  3:
			vipIconRes = "set:summary.json image:MVP";
			break;
		default:
			vipIconRes = "";
			break;
		}

		m_vipIcon->SetImage(vipIconRes.c_str());
		m_awardMaxIcon->SetVisible(false);
		m_reward->SetVisible(false);
		m_rewardIcon->SetVisible(false);
	}

	void GuiSummaryItem::setButtonVisible(bool isVisible)
	{
		if (m_btn)
		{
			m_btn->SetVisible(isVisible);
		}
	}

	void GuiSummaryItem::setButtonText(GUIString str)
	{
		if (m_btn)
		{
			m_btn->SetText(str);
		}
	}

	bool GuiSummaryItem::onClick(const EventArgs &)
	{
		switch (CGame::Instance()->GetGameType())
		{
		case ClientGameType::Tiny_Town:
			RootGuiLayout::Instance()->showMainControl();
			if (m_userId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendCallOnManor(m_userId);
			}
			break;
		}
		return true;
	}

	bool GuiSummaryItem::onClickVisit(const EventArgs &)
	{
		RootGuiLayout::Instance()->showMainControl();
		SoundSystem::Instance()->playEffectByType(ST_Click);
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuildWarOpenGuessResult();
		if (m_userId > 0)
		{
			ClientNetwork::Instance()->getSender()->sendBuildWarGuessVisit(m_userId);
		}
		
		return true;
	}

	void GuiSummaryItem::RefreshItem(ResultEntry::ResultEntryPtr result)
	{
		String	bgImage;
		String	image;
		String  tenImage;
		Color	textColor;
		Color	borderColor;

		m_resultEntry = result;
		GetImageByRank(m_resultEntry->rank, bgImage, image, tenImage, textColor, borderColor);

		m_rankBgImage->SetImage(bgImage.c_str());
		m_name->SetText(m_resultEntry->name.c_str());
		m_name->SetTextColor(textColor);
		m_name->SetTextBoader(borderColor);

		if (CGame::Instance()->GetGameType() == ClientGameType::Build_War)
		{
			m_visitBtn->SetVisible(true);
		}

		if (m_resultEntry->rank > 9)
		{
			m_doubleRank->SetVisible(true);
			m_rankImage->SetVisible(false);
			m_doubleRankTen->SetImage(tenImage.c_str());
			m_doubleRankOne->SetImage(image.c_str());
		}
		else
		{
			m_doubleRank->SetVisible(false);
			m_rankImage->SetVisible(true);
			m_rankImage->SetImage(image.c_str());
		}

		if (m_resultEntry->isWin == 1)
		{
			m_result->SetImage("set:summary.json image:SmallWin");
		}
		else if (m_resultEntry->isWin == 0)
		{
			m_result->SetImage("set:summary.json image:SmallLose");
		}
		else
		{
			m_result->SetImage("set:summary.json image:SmallDogfall");
		}

		String vipIconRes = "";
		switch (m_resultEntry->vip)
		{
		case  1:
			vipIconRes = "set:summary.json image:VIP";
			break;
		case  2:
			vipIconRes = "set:summary.json image:VIPPlus";
			break;
		case  3:
			vipIconRes = "set:summary.json image:MVP";
			break;
		default:
			vipIconRes = "";
			break;
		}

		m_userId = m_resultEntry->user_id;

		m_vipIcon->SetImage(vipIconRes.c_str());
		m_awardMaxIcon->SetVisible(m_resultEntry->todayGetReward >= m_resultEntry->canGetReward);
		m_reward->SetText(StringUtil::ToString(result->reward).c_str());
		m_buildWarScore->SetText(StringUtil::ToString(result->build_war_score).c_str());
	}

	bool GuiSummaryItem::GetImageByRank(int rank, String & bgImage, String & image, String & tenImage, Color & textColor, Color & borderColor)
	{
		borderColor = Color(67.0f/255.0f, 50.0f/255.0f, 39.0f/255.0f); //433227 
		switch (rank)
		{
		case 1:
			bgImage = "set:new_gui_material.json image:red_icon";
			textColor = Color(212.0f/255.0f, 86.0f/255.0f, 86.0f/255.0f); //D45656
			break;
		case 2:
			bgImage = "set:new_gui_material.json image:yellow_icon";
			textColor = Color(237.0f/255.0f, 208.0f/255.0f, 84.0f/255.0f); //EDD054
			break;
		case 3:
			bgImage = "set:new_gui_material.json image:blue_icon";
			textColor = Color(148.0f/255.0f, 156.0f/255.0f, 177.0f/255.0f); //949CB3
			break;
		default:
			bgImage = "set:new_gui_material.json image:brown_icon";
			textColor = Color(1, 1, 1);									//D45656
			break;
		}

		int _rank = rank % 100; // 最大支持两位数排名
		if (_rank > 9)
		{
			GetNumberImage(_rank / 10, tenImage);
			GetNumberImage(_rank % 10, image);
		}
		else
		{
			GetNumberImage(_rank, image);
		}

		return true;
	}

	bool GuiSummaryItem::GetNumberImage(int num, String& image)
	{
		switch (num)
		{
		case 1:
			image = "set:new_gui_material.json image:number_one";
			break;
		case 2:
			image = "set:new_gui_material.json image:number_two";
			break;
		case 3:
			image = "set:new_gui_material.json image:number_three";
			break;
		case 4:
			image = "set:new_gui_material.json image:number_four";
			break;
		case 5:
			image = "set:new_gui_material.json image:number_five";
			break;
		case 6:
			image = "set:new_gui_material.json image:number_six";
			break;
		case 7:
			image = "set:new_gui_material.json image:number_seven";
			break;
		case 8:
			image = "set:new_gui_material.json image:number_eight";
			break;
		case 9:
			image = "set:new_gui_material.json image:number_nine";
			break;
		default:
			image = "set:new_gui_material.json image:number_zero";
			break;
		}

		return true;
	}

	bool GuiSummaryItem::onNameClick(const EventArgs & events)
	{
		if (!m_resultEntry)
			return false;

		auto rect = m_name->GetUnclippedOuterRect();
		LORD::UVector2 pos = UVector2(UDim(0.0f, rect.left + m_name->GetPixelSize().x),
			UDim(0.0f, rect.top - m_name->GetPixelSize().y / 2));

		RootGuiLayout::Instance()->showSummarPoint(pos, m_resultEntry->pointMap);
		return true;
	}
}
