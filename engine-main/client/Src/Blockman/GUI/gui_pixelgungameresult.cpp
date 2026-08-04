#include "gui_pixelgungameresult.h"
#include "gui_layout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/StringUtil.h"
#include "ShellInterface.h"
#include "game.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN {
	gui_pixelgungameresult::gui_pixelgungameresult() :
		gui_layout("PixelGunGameResult.json")
	{
		memset(m_data_common.reward_num, 0, sizeof(m_data_common.reward_num));
		memset(m_data_person.person_rank, 0, sizeof(m_data_person.person_rank));
		memset(m_data_person.person_lv, 0, sizeof(m_data_person.person_lv));
		memset(m_data_person.person_kills, 0, sizeof(m_data_person.person_kills));
		memset(m_data_team.blue_rank, 0, sizeof(m_data_team.blue_rank));
		memset(m_data_team.blue_lv, 0, sizeof(m_data_team.blue_lv));
		memset(m_data_team.blue_kills, 0, sizeof(m_data_team.blue_kills));
		memset(m_data_team.red_rank, 0, sizeof(m_data_team.red_rank));
		memset(m_data_team.red_lv, 0, sizeof(m_data_team.red_lv));
		memset(m_data_team.red_kills, 0, sizeof(m_data_team.red_kills));
	}

	gui_pixelgungameresult::~gui_pixelgungameresult()
	{
	}

	void gui_pixelgungameresult::onLoad()
	{
		getWindow<GUIStaticText>("PixelGunGameResult-Title")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_BLUE_TEAM_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RED_TEAM_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTitle-Rank")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_RANK_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTitle-Duan")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_DUAN_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTitle-Lv")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_LV_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTitle-Name")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_NAME_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTitle-Kills")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_KILL_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTitle-Rank")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_RANK_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTitle-Duan")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_DUAN_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTitle-Lv")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_LV_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTitle-Name")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_NAME_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTitle-Kills")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_KILL_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-SelfRewardTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_REWARD_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-SelfGoldTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_GOLD_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-SelfBlockmanGoTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_BLOCKMAN_TITLE));
		getWindow<GUIStaticText>("PixelGunGameResult-BackTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_BTN_BACK));
		getWindow<GUIStaticText>("PixelGunGameResult-NextTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_BTN_NEXT));
		getWindow<GUIStaticText>("PixelGunGameResult-RevengeTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_RESULT_BTN_REVENGE));

		for (int i = 0; i < MAX_REWARD_NUM; ++i)
		{
			String rewardImgName = StringUtil::Format("PixelGunGameResult-SelfRewardImg%d", i + 1);
			m_pixel_reward_img[i] = getWindow<GUIStaticImage>(rewardImgName.c_str());

			String rewardNumName = StringUtil::Format("PixelGunGameResult-SelfRewardNum%d", i + 1);
			m_pixel_reward_num[i] = getWindow<GUIStaticText>(rewardNumName.c_str());
		}

		m_pixel_blue_team_score = getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamScore");
		m_pixel_red_team_score = getWindow<GUIStaticText>("PixelGunGameResult-RedTeamScore");
		m_pixel_self_gold_txt = getWindow<GUIStaticText>("PixelGunGameResult-SelfGoldNum");
		m_pixel_blue_team_title = getWindow<GUIStaticText>("PixelGunGameResult-BlueTeamTxt");
		m_pixel_red_team_title = getWindow<GUIStaticText>("PixelGunGameResult-RedTeamTxt");
		m_pixel_blue_team_title_bg = getWindow<GUIStaticImage>("PixelGunGameResult-BlueTeamTitleBg");
		m_pixel_red_team_title_bg = getWindow<GUIStaticImage>("PixelGunGameResult-RedTeamTitleBg");
		m_pixel_blue_icon = getWindow<GUIStaticImage>("PixelGunGameResult-BlueTeamIcon");
		m_pixel_red_icon = getWindow<GUIStaticImage>("PixelGunGameResult-RedTeamIcon");

		for (int i = 0; i < MAX_DATA_ITEM_NUM; ++i)
		{
			String strItemBgName = StringUtil::Format("PixelGunGameResult-DataItemBg-%d", i + 1).c_str();
			m_pixel_data_item_bg[i] = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strItemBgName.c_str());
			m_pixel_data_item_bg[i]->SetTouchable(false);
			float relaY = 1.2f + i % MAX_COLLUME_NUM;
			m_pixel_data_item_bg[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_bg[i]->SetArea(UDim(0.f, 0.f), UDim(relaY, 0.f), UDim(1.f, 0.f), UDim(0, 32.f));
			m_pixel_data_item_bg[i]->SetImage("set:pixelgungameresult.json image:black_di");

			String strItemRankName = StringUtil::Format("PixelGunGameResult-DataItemRank-%d", i + 1).c_str();
			m_pixel_data_item_rank_num[i] = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strItemRankName.c_str());
			m_pixel_data_item_rank_num[i]->SetTouchable(false);
			m_pixel_data_item_rank_num[i]->SetArea(UDim(0.07f, 0.f), UDim(0.f, 0.f), UDim(0.f, 30.f), UDim(0.f, 32.f));
			m_pixel_data_item_rank_num[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_rank_num[i]->SetTextHorzAlign(HA_CENTRE);
			m_pixel_data_item_rank_num[i]->SetTextVertAlign(VA_CENTRE);
			m_pixel_data_item_rank_num[i]->SetTextColor(Color(1.f, 1.f, 0.f));
			m_pixel_data_item_rank_num[i]->SetTextBoader(LORD::Color::BLACK);
			m_pixel_data_item_bg[i]->AddChildWindow(m_pixel_data_item_rank_num[i]);

			String strItemDuanName = StringUtil::Format("PixelGunGameResult-DataItemDuan-%d", i + 1).c_str();
			m_pixel_data_item_duan[i] = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strItemDuanName.c_str());
			m_pixel_data_item_duan[i]->SetTouchable(false);
			m_pixel_data_item_duan[i]->SetArea(UDim(0.215f, 0.f), UDim(0.f, 0.f), UDim(0.f, 35.f), UDim(0.f, 30.f));
			m_pixel_data_item_duan[i]->SetImage("set:pixelgunhallmodeselect.json image:rank1");
			m_pixel_data_item_duan[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_bg[i]->AddChildWindow(m_pixel_data_item_duan[i]);

			String strItemLvName = StringUtil::Format("PixelGunGameResult-DataItemLv-%d", i + 1).c_str();
			m_pixel_data_item_lv[i] = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strItemLvName.c_str());
			m_pixel_data_item_lv[i]->SetTouchable(false);
			m_pixel_data_item_lv[i]->SetArea(UDim(0.365f, 0.f), UDim(0.f, 0.f), UDim(0.f, 30.f), UDim(1.f, 0.f));
			m_pixel_data_item_lv[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_lv[i]->SetTextHorzAlign(HA_CENTRE);
			m_pixel_data_item_lv[i]->SetTextVertAlign(VA_CENTRE);
			m_pixel_data_item_lv[i]->SetTextBoader(LORD::Color::BLACK);
			m_pixel_data_item_bg[i]->AddChildWindow(m_pixel_data_item_lv[i]);

			String strItemNameName = StringUtil::Format("PixelGunGameResult-DataItemName-%d", i + 1).c_str();
			m_pixel_data_item_name[i] = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strItemNameName.c_str());
			m_pixel_data_item_name[i]->SetTouchable(false);
			m_pixel_data_item_name[i]->SetArea(UDim(0.5f, 0.f), UDim(0.f, 0.f), UDim(0.f, 100.f), UDim(1.f, 0.f));
			m_pixel_data_item_name[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_name[i]->SetTextHorzAlign(HA_CENTRE);
			m_pixel_data_item_name[i]->SetTextVertAlign(VA_CENTRE);
			m_pixel_data_item_name[i]->SetTextBoader(LORD::Color::BLACK);
			m_pixel_data_item_bg[i]->AddChildWindow(m_pixel_data_item_name[i]);

			String strItemKillName = StringUtil::Format("PixelGunGameResult-DataItemKill-%d", i + 1).c_str();
			m_pixel_data_item_kill[i] = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strItemKillName.c_str());
			m_pixel_data_item_kill[i]->SetTouchable(false);
			m_pixel_data_item_kill[i]->SetArea(UDim(0.8f, 0.f), UDim(0.f, 0.f), UDim(0.f, 100.f), UDim(1.f, 0.f));
			m_pixel_data_item_kill[i]->SetHorizontalAlignment(HA_LEFT);
			m_pixel_data_item_kill[i]->SetTextHorzAlign(HA_CENTRE);
			m_pixel_data_item_kill[i]->SetTextVertAlign(VA_CENTRE); 
			m_pixel_data_item_kill[i]->SetTextBoader(LORD::Color::BLACK);
			m_pixel_data_item_bg[i]->AddChildWindow(m_pixel_data_item_kill[i]);

			if (i / MAX_COLLUME_NUM == 0)
			{
				m_pixel_blue_team_title_bg->AddChildWindow(m_pixel_data_item_bg[i]);
				m_pixel_data_item_lv[i]->SetTextColor(Color(0.3803f, 0.6431f, 1.0f));
				m_pixel_data_item_name[i]->SetTextColor(Color(0.3803f, 0.6431f, 1.0f));
				m_pixel_data_item_kill[i]->SetTextColor(Color(0.3803f, 0.6431f, 1.0f));
			}
			else
			{
				m_pixel_red_team_title_bg->AddChildWindow(m_pixel_data_item_bg[i]);
				m_pixel_data_item_lv[i]->SetTextColor(Color(1.0f, 0.447f, 0.4196f));
				m_pixel_data_item_name[i]->SetTextColor(Color(1.0f, 0.447f, 0.4196f));
				m_pixel_data_item_kill[i]->SetTextColor(Color(1.0f, 0.447f, 0.4196f));
			}
		}

		m_pixel_back_btn = getWindow<GUIButton>("PixelGunGameResult-Back");
		m_pixel_next_btn = getWindow<GUIButton>("PixelGunGameResult-Next");
		m_pixel_revenge_btn = getWindow<GUIButton>("PixelGunGameResult-Revenge");

		m_pixel_back_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgungameresult::onBackClick, this, std::placeholders::_1));
		m_pixel_next_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgungameresult::onNextClick, this, std::placeholders::_1));
		m_pixel_revenge_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgungameresult::onRevbengeClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(ShowPixelGunResultEvent::subscribe(std::bind(&gui_pixelgungameresult::refreshAll, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowPixelGunResultSpecialInfoEvent::subscribe(std::bind(&gui_pixelgungameresult::refreshSpecialInfo, this, std::placeholders::_1)));
	}

	void gui_pixelgungameresult::onShow()
	{

	}

	bool gui_pixelgungameresult::refreshAll(const String & data, int result_type)
	{
		bool common = false;
		switch (result_type)
		{
		case RT_1V1:
			if (get1v1Info(data))
			{
				common = true;

				m_pixel_blue_team_title->SetVisible(false);
				m_pixel_red_team_title->SetVisible(false);
				m_pixel_blue_team_score->SetVisible(false);
				m_pixel_red_team_score->SetVisible(false);
				m_pixel_revenge_btn->SetVisible(true);
				m_pixel_next_btn->SetVisible(false);
				m_pixel_red_icon->SetVisible(true);
				m_pixel_blue_icon->SetVisible(true);

				m_pixel_revenge_btn->SetEnabled(m_data_1v1.revenge_btn_enable);

				if (m_data_1v1.win_team == TT_BLUE)
				{
					m_pixel_red_icon->SetImage("set:pixelgungameresult.json image:kulou");
					m_pixel_blue_icon->SetImage("set:pixelgungameresult.json image:jiangbei");
				}
				else
				{
					m_pixel_blue_icon->SetImage("set:pixelgungameresult.json image:kulou");
					m_pixel_red_icon->SetImage("set:pixelgungameresult.json image:jiangbei");
				}

				for (int i = 0; i < MAX_DATA_ITEM_NUM; ++i)
				{
					if (i % MAX_COLLUME_NUM == 0)
					{
						m_pixel_data_item_bg[i]->SetVisible(true);
					}
					else
					{
						m_pixel_data_item_bg[i]->SetVisible(false);
					}
				}

				if (m_data_1v1.blue_duan_img.length() > 0)
				{
					m_pixel_data_item_duan[0]->SetImage(m_data_1v1.blue_duan_img.c_str());
				}
				
				if (m_data_1v1.red_duan_img.length() > 0)
				{
					m_pixel_data_item_duan[8]->SetImage(m_data_1v1.red_duan_img.c_str());
				}

				m_pixel_data_item_name[0]->SetText(m_data_1v1.blue_name.c_str());
				m_pixel_data_item_name[8]->SetText(m_data_1v1.red_name.c_str());

				if (m_data_1v1.blue_lv >= 0)
				{
					String blueLvTxtName = StringUtil::Format("%d", m_data_1v1.blue_lv);
					m_pixel_data_item_lv[0]->SetText(blueLvTxtName.c_str());
					m_pixel_data_item_lv[0]->SetVisible(true);
				}
				else
				{
					m_pixel_data_item_lv[0]->SetVisible(false);
				}

				if (m_data_1v1.red_lv >= 0)
				{
					String redLvTxtName = StringUtil::Format("%d", m_data_1v1.red_lv);
					m_pixel_data_item_lv[8]->SetText(redLvTxtName.c_str());
					m_pixel_data_item_lv[8]->SetVisible(true);
				}
				else
				{
					m_pixel_data_item_lv[8]->SetVisible(false);
				}

				if (m_data_1v1.blue_kills >= 0)
				{
					String blueKillsTxtName = StringUtil::Format("%d", m_data_1v1.blue_kills);
					m_pixel_data_item_kill[0]->SetText(blueKillsTxtName.c_str());
					m_pixel_data_item_kill[0]->SetVisible(true);
				}
				else
				{
					m_pixel_data_item_kill[0]->SetVisible(false);
				}

				
				if (m_data_1v1.red_kills >= 0)
				{
					String redKillsTxtName = StringUtil::Format("%d", m_data_1v1.red_kills);
					m_pixel_data_item_kill[8]->SetText(redKillsTxtName.c_str());
					m_pixel_data_item_kill[8]->SetVisible(true);
				}
				else
				{
					m_pixel_data_item_kill[8]->SetVisible(false);
				}

				if (m_data_1v1.self_team == TT_BLUE)
				{
					m_pixel_data_item_bg[0]->SetImage("set:pixelgungameresult.json image:green_di");
					m_pixel_data_item_bg[8]->SetImage("set:pixelgungameresult.json image:black_di");
				}
				else
				{
					m_pixel_data_item_bg[0]->SetImage("set:pixelgungameresult.json image:black_di");
					m_pixel_data_item_bg[8]->SetImage("set:pixelgungameresult.json image:green_di");
				}
			}

			break;
		case RT_PERSON:
			if (getPersonInfo(data))
			{
				common = true;

				m_pixel_blue_team_title->SetVisible(false);
				m_pixel_red_team_title->SetVisible(false);
				m_pixel_blue_team_score->SetVisible(false);
				m_pixel_red_team_score->SetVisible(false);
				m_pixel_revenge_btn->SetVisible(false);
				m_pixel_next_btn->SetVisible(true);
				m_pixel_red_icon->SetVisible(false);
				m_pixel_blue_icon->SetVisible(false);

				for (int i = 0; i < MAX_DATA_ITEM_NUM; ++i)
				{
					if (m_data_person.person_rank[i] > 0)
					{
						if (m_data_person.person_duan_img[i].length() > 0)
						{
							m_pixel_data_item_duan[i]->SetImage(m_data_person.person_duan_img[i].c_str());
						}
						m_pixel_data_item_name[i]->SetText(m_data_person.person_name[i].c_str());

						String personLvTxtName = StringUtil::Format("%d", m_data_person.person_lv[i]);
						m_pixel_data_item_lv[i]->SetText(personLvTxtName.c_str());

						String personRankTxtName = StringUtil::Format("%d", m_data_person.person_rank[i]);
						m_pixel_data_item_rank_num[i]->SetText(personRankTxtName.c_str());

						String personKillTxtName = StringUtil::Format("%d", m_data_person.person_kills[i]);
						m_pixel_data_item_kill[i]->SetText(personKillTxtName.c_str());

						m_pixel_data_item_bg[i]->SetVisible(true);
						
						if (m_data_person.self_rank == m_data_person.person_rank[i])
						{
							m_pixel_data_item_bg[i]->SetImage("set:pixelgungameresult.json image:green_di");
						}
						else
						{
							m_pixel_data_item_bg[i]->SetImage("set:pixelgungameresult.json image:black_di");
						}
					}
					else
					{
						m_pixel_data_item_bg[i]->SetVisible(false);
					}
				}
			}

			break;
		case RT_TEAM:
			if (getTeamInfo(data))
			{
				common = true;

				m_pixel_blue_team_title->SetVisible(true);
				m_pixel_red_team_title->SetVisible(true);
				m_pixel_blue_team_score->SetVisible(true);
				m_pixel_red_team_score->SetVisible(true);
				m_pixel_revenge_btn->SetVisible(false);
				m_pixel_next_btn->SetVisible(true);

				m_pixel_red_icon->SetVisible(true);
				m_pixel_blue_icon->SetVisible(true);

				if (m_data_team.win_team == TT_BLUE)
				{
					m_pixel_red_icon->SetImage("set:pixelgungameresult.json image:kulou");
					m_pixel_blue_icon->SetImage("set:pixelgungameresult.json image:jiangbei");
				}
				else
				{
					m_pixel_blue_icon->SetImage("set:pixelgungameresult.json image:kulou");
					m_pixel_red_icon->SetImage("set:pixelgungameresult.json image:jiangbei");
				}

				String selfBlueScoreTxtName = StringUtil::Format("%d", m_data_team.blue_team_score);
				m_pixel_blue_team_score->SetText(selfBlueScoreTxtName.c_str());

				String selfRedScoreTxtName = StringUtil::Format("%d", m_data_team.red_team_score);
				m_pixel_red_team_score->SetText(selfRedScoreTxtName.c_str());

				for (int i = 0; i < MAX_COLLUME_NUM; ++i)
				{
					int index = i;

					if (m_data_team.blue_rank[i] > 0)
					{
						if (m_data_team.blue_duan_img[i].length() > 0)
						{
							m_pixel_data_item_duan[index]->SetImage(m_data_team.blue_duan_img[i].c_str());
						}
 						m_pixel_data_item_name[index]->SetText(m_data_team.blue_name[i].c_str());
 
						String blueLvTxtName = StringUtil::Format("%d", m_data_team.blue_lv[i]);
						m_pixel_data_item_lv[index]->SetText(blueLvTxtName.c_str());

						String blueRankTxtName = StringUtil::Format("%d", m_data_team.blue_rank[i]);
						m_pixel_data_item_rank_num[index]->SetText(blueRankTxtName.c_str());

						String blueKillTxtName = StringUtil::Format("%d", m_data_team.blue_kills[i]);
						m_pixel_data_item_kill[index]->SetText(blueKillTxtName.c_str());

						m_pixel_data_item_bg[index]->SetVisible(true);

						if (m_data_team.self_team == TT_BLUE)
						{
							if (m_data_team.self_rank == m_data_team.blue_rank[i])
							{
								m_pixel_data_item_bg[index]->SetImage("set:pixelgungameresult.json image:green_di");
							}
							else
							{
								m_pixel_data_item_bg[index]->SetImage("set:pixelgungameresult.json image:black_di");
							}
						}
					}
					else
					{
						m_pixel_data_item_bg[index]->SetVisible(false);
					}
				}

				for (int i = 0; i < MAX_COLLUME_NUM; ++i)
				{
					int index = i + MAX_COLLUME_NUM;

					if (m_data_team.red_rank[i] > 0)
					{
						if (m_data_team.red_duan_img[i].length() > 0)
						{
							m_pixel_data_item_duan[index]->SetImage(m_data_team.red_duan_img[i].c_str());
						}
						m_pixel_data_item_name[index]->SetText(m_data_team.red_name[i].c_str());

						String redLvTxtName = StringUtil::Format("%d", m_data_team.red_lv[i]);
						m_pixel_data_item_lv[index]->SetText(redLvTxtName.c_str());

						String redRankTxtName = StringUtil::Format("%d", m_data_team.red_rank[i]); 
						m_pixel_data_item_rank_num[index]->SetText(redRankTxtName.c_str()); 

						String redKillTxtName = StringUtil::Format("%d", m_data_team.red_kills[i]);
						m_pixel_data_item_kill[index]->SetText(redKillTxtName.c_str()); 

						m_pixel_data_item_bg[index]->SetVisible(true);

						if (m_data_team.self_team == TT_RED)
						{
							if (m_data_team.self_rank == m_data_team.red_rank[i])
							{
								m_pixel_data_item_bg[index]->SetImage("set:pixelgungameresult.json image:green_di");
							}
							else
							{
								m_pixel_data_item_bg[index]->SetImage("set:pixelgungameresult.json image:black_di");
							}
						}
					}
					else
					{
						m_pixel_data_item_bg[index]->SetVisible(false);
					}
				}
			}

			break;
		default:
			break;
		}
		
		if (common)
		{
			String selfGoldTxtName = StringUtil::Format("%d", m_data_common.reward_gold_num);
			m_pixel_self_gold_txt->SetText(selfGoldTxtName.c_str());

			for (int i = 0; i < MAX_REWARD_NUM; ++i)
			{
				if (m_data_common.reward_num[i] != 0)
				{
					if (m_data_common.reward_img[i].length() > 0)
					{
						m_pixel_reward_img[i]->SetImage(m_data_common.reward_img[i].c_str());
						m_pixel_reward_img[i]->SetVisible(true);
					}
					else
					{
						m_pixel_reward_img[i]->SetVisible(false);
					}

					String rewardNumTxtName = StringUtil::Format("%d", m_data_common.reward_num[i]);
					m_pixel_reward_num[i]->SetText(rewardNumTxtName.c_str());
				}
				else
				{
					m_pixel_reward_img[i]->SetVisible(false);
				}
			}
		}

		return true;
	}

	bool gui_pixelgungameresult::get1v1Info(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgungameresult::get1v1Info HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("reward_gold_num")
			|| !doc->HasMember("reward_list")
			|| !doc->HasMember("blue_duan_img")
			|| !doc->HasMember("blue_lv")
			|| !doc->HasMember("blue_name")
			|| !doc->HasMember("blue_kills")
			|| !doc->HasMember("red_duan_img")
			|| !doc->HasMember("red_lv")
			|| !doc->HasMember("red_name")
			|| !doc->HasMember("red_kills")
			|| !doc->HasMember("self_team")
			|| !doc->HasMember("win_team")
			|| !doc->HasMember("revenge_btn_enable")
			|| !doc->HasMember("min_money"))
		{
			LordLogError("gui_pixelgungameresult::get1v1Info The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_data_common.reward_gold_num = doc->FindMember("reward_gold_num")->value.GetInt();
		m_data_1v1.blue_duan_img = doc->FindMember("blue_duan_img")->value.GetString();
		m_data_1v1.blue_lv = doc->FindMember("blue_lv")->value.GetInt();
		m_data_1v1.blue_name = doc->FindMember("blue_name")->value.GetString();
		m_data_1v1.blue_kills = doc->FindMember("blue_kills")->value.GetInt();
		m_data_1v1.red_duan_img = doc->FindMember("red_duan_img")->value.GetString();
		m_data_1v1.red_lv = doc->FindMember("red_lv")->value.GetInt();
		m_data_1v1.red_name = doc->FindMember("red_name")->value.GetString();
		m_data_1v1.red_kills = doc->FindMember("red_kills")->value.GetInt();
		m_data_1v1.self_team = doc->FindMember("self_team")->value.GetInt();
		m_data_1v1.win_team = doc->FindMember("win_team")->value.GetInt();
		m_data_1v1.revenge_btn_enable = doc->FindMember("revenge_btn_enable")->value.GetBool();
		m_data_1v1.min_money = doc->FindMember("min_money")->value.GetInt();

		rapidjson::Value arr = doc->FindMember("reward_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size() && i < MAX_REWARD_NUM; ++i)
		{
			if (!arr[i].HasMember("img") || !arr[i].HasMember("num"))
			{
				LordLogError("gui_pixelgungameresult::get1v1Info The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_common.reward_img[i] = arr[i].FindMember("img")->value.GetString();
			m_data_common.reward_num[i] = arr[i].FindMember("num")->value.GetInt();
		}

		return true;
	}

	bool gui_pixelgungameresult::getPersonInfo(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgungameresult::getPersonInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("reward_gold_num")
			|| !doc->HasMember("reward_list")
			|| !doc->HasMember("player_list")
			|| !doc->HasMember("self_rank"))
		{
			LordLogError("gui_pixelgungameresult::getPersonInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_data_common.reward_gold_num = doc->FindMember("reward_gold_num")->value.GetInt();
		m_data_person.self_rank = doc->FindMember("self_rank")->value.GetInt();

		rapidjson::Value arr = doc->FindMember("reward_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size() && i < MAX_REWARD_NUM; ++i)
		{
			if (!arr[i].HasMember("img") || !arr[i].HasMember("num"))
			{
				LordLogError("gui_pixelgungameresult::getPersonInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_common.reward_img[i] = arr[i].FindMember("img")->value.GetString();
			m_data_common.reward_num[i] = arr[i].FindMember("num")->value.GetInt();
		}

		rapidjson::Value arr2 = doc->FindMember("player_list")->value.GetArray();
		for (size_t i = 0; i < arr2.Size() && i < MAX_DATA_ITEM_NUM; ++i)
		{
			if (!arr2[i].HasMember("rank")
				|| !arr2[i].HasMember("duam_img")
				|| !arr2[i].HasMember("lv")
				|| !arr2[i].HasMember("name")
				|| !arr2[i].HasMember("kills"))
			{
				LordLogError("gui_pixelgungameresult::getPersonInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_person.person_rank[i] = arr2[i].FindMember("rank")->value.GetInt();
			m_data_person.person_duan_img[i] = arr2[i].FindMember("duam_img")->value.GetString();
			m_data_person.person_lv[i] = arr2[i].FindMember("lv")->value.GetInt();
			m_data_person.person_name[i] = arr2[i].FindMember("name")->value.GetString();
			m_data_person.person_kills[i] = arr2[i].FindMember("kills")->value.GetInt();
		}

		return true;
	}

	bool gui_pixelgungameresult::getTeamInfo(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgungameresult::getTeamInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("reward_gold_num")
			|| !doc->HasMember("blue_team_score")
			|| !doc->HasMember("red_team_score")
			|| !doc->HasMember("reward_list")
			|| !doc->HasMember("blue_team")
			|| !doc->HasMember("red_team")
			|| !doc->HasMember("win_team"))
		{
			LordLogError("gui_pixelgungameresult::getTeamInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_data_common.reward_gold_num = doc->FindMember("reward_gold_num")->value.GetInt();
		m_data_team.blue_team_score = doc->FindMember("blue_team_score")->value.GetInt();
		m_data_team.red_team_score = doc->FindMember("red_team_score")->value.GetInt();
		m_data_team.self_team = doc->FindMember("self_team")->value.GetInt();
		m_data_team.self_rank = doc->FindMember("self_rank")->value.GetInt();
		m_data_team.win_team = doc->FindMember("win_team")->value.GetInt();

		rapidjson::Value arr = doc->FindMember("reward_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size() && i < MAX_REWARD_NUM; ++i)
		{
			if (!arr[i].HasMember("img") || !arr[i].HasMember("num"))
			{
				LordLogError("gui_pixelgungameresult::getTeamInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_common.reward_img[i] = arr[i].FindMember("img")->value.GetString();
			m_data_common.reward_num[i] = arr[i].FindMember("num")->value.GetInt();
		}

		rapidjson::Value arr2 = doc->FindMember("blue_team")->value.GetArray();
		for (size_t i = 0; i < arr2.Size() && i < MAX_COLLUME_NUM; ++i)
		{
			if (!arr2[i].HasMember("rank")
				|| !arr2[i].HasMember("duam_img")
				|| !arr2[i].HasMember("lv")
				|| !arr2[i].HasMember("name")
				|| !arr2[i].HasMember("kills"))
			{
				LordLogError("gui_pixelgungameresult::getTeamInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_team.blue_rank[i] = arr2[i].FindMember("rank")->value.GetInt();
			m_data_team.blue_duan_img[i] = arr2[i].FindMember("duam_img")->value.GetString();
			m_data_team.blue_lv[i] = arr2[i].FindMember("lv")->value.GetInt();
			m_data_team.blue_name[i] = arr2[i].FindMember("name")->value.GetString();
			m_data_team.blue_kills[i] = arr2[i].FindMember("kills")->value.GetInt();
		}

		rapidjson::Value arr3 = doc->FindMember("red_team")->value.GetArray();
		for (size_t i = 0; i < arr3.Size() && i < MAX_COLLUME_NUM; ++i)
		{
			if (!arr3[i].HasMember("rank")
				|| !arr3[i].HasMember("duam_img")
				|| !arr3[i].HasMember("lv")
				|| !arr3[i].HasMember("name")
				|| !arr3[i].HasMember("kills"))
			{
				LordLogError("gui_pixelgungameresult::getTeamInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_data_team.red_rank[i] = arr3[i].FindMember("rank")->value.GetInt();
			m_data_team.red_duan_img[i] = arr3[i].FindMember("duam_img")->value.GetString();
			m_data_team.red_lv[i] = arr3[i].FindMember("lv")->value.GetInt();
			m_data_team.red_name[i] = arr3[i].FindMember("name")->value.GetString();
			m_data_team.red_kills[i] = arr3[i].FindMember("kills")->value.GetInt();
		}

		return true;
	}

	bool gui_pixelgungameresult::refreshSpecialInfo(bool btnRevengeEnable)
	{
		m_pixel_revenge_btn->SetEnabled(btnRevengeEnable);
		return true;
	}

	void gui_pixelgungameresult::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_pixelgungameresult::onNextClick(const EventArgs & events)
	{
		// getParent()->showMainControl();
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendPixelGunResultNext();
		return true;
	}

	bool gui_pixelgungameresult::onRevbengeClick(const EventArgs & events)
	{
		// getParent()->showMainControl();

		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			if (wallet)
			{
				i64 money = wallet->getMoneyByCoinId(CurrencyType::PLATFORM_MONEY);
				if (money < m_data_1v1.min_money)
				{
					i32 diff = (i32)(m_data_1v1.min_money - money);
					ShowLackOfMoneyTipDialogEvent::emit(diff, CurrencyType::PLATFORM_MONEY);
					return true;
				}
			}
			else
			{
				return true;
			}
		}

		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendPixelGunResultRevenge();
		return true;
	}

	bool gui_pixelgungameresult::onBackClick(const EventArgs & events)
	{
		// getParent()->showMainControl();
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendPixelGunResultBack();
		return true;
	}

}
