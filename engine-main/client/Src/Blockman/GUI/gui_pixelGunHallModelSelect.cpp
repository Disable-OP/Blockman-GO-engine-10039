#include "gui_pixelGunHallModelSelect.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "GUIPixelGunHallModeSelectMapItem.h"
#include "GUI/GuiDef.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_pixelGunHallModelSelect::gui_pixelGunHallModelSelect()
		: gui_layout("PixelGunHallModeSelect.json")
	{
	}

	gui_pixelGunHallModelSelect::~gui_pixelGunHallModelSelect()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelGunHallModelSelect::onLoad()
	{
		m_mapInfoLayout = getWindow<GUILayout>("PixelGunHallModeSelect-MapInfoLayout");
		m_mode_des_title = getWindow<GUIStaticText>("PixelGunHallModeSelect-DesTitle");
		m_mode_des = getWindow<GUIStaticText>("PixelGunHallModeSelect-DesTxt");
		m_closeBtn = getWindow<GUIButton>("PixelGunHallModeSelect-Btn-Close");
		m_storageBtn = getWindow<GUIButton>("PixelGunHallModeSelect-BtnStorage");
		m_fightBtn = getWindow<GUIButton>("PixelGunHallModeSelect-BtnFight");
		m_left_bg = getWindow<GUILayout>("PixelGunHallModeSelect-Left");
		m_right_bg = getWindow<GUILayout>("PixelGunHallModeSelect-Right");
		m_fragLayout = getWindow<GUILayout>("PixelGunHallModeSelect-FragmentLayout");
		m_rewardLayout = getWindow<GUILayout>("PixelGunHallModeSelect-RewardLayout");
		m_rightFragmentImg = getWindow<GUIStaticImage>("PixelGunHallModeSelect-FragRightImg");
		m_rightFragmentNumTxt = getWindow<GUIStaticText>("PixelGunHallModeSelect-FragRightNum");
		m_fragLayout->SetVisible(false);
		m_rewardLayout->SetVisible(false);

		m_mapInfoGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "PixelGunHallModeSelect-MapInfoGridView");
		m_mapInfoGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_mapInfoGridView->InitConfig(5.f, 5.f, 4);
		m_mapInfoLayout->AddChildWindow(m_mapInfoGridView);

		for (int i = 0; i < MAX_PIXEL_GUNHALL_MODE_NUM; ++i)
		{
			String modeSelectName = StringUtil::Format("PixelGunHallModeSelect-ModeSelect%d", i + 1);
			m_mode_select[i] = getWindow<GUIStaticImage>(modeSelectName.c_str());

			String modeBgName = StringUtil::Format("PixelGunHallModeSelect-ModeBg%d", i + 1);
			m_mode_bg[i] = getWindow<GUIStaticImage>(modeBgName.c_str());
			m_mode_bg[i]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_pixelGunHallModelSelect::onModeImgClick, this, i));

			String modeImgName = StringUtil::Format("PixelGunHallModeSelect-ModeImg%d", i + 1);
			m_mode_img[i] = getWindow<GUIStaticImage>(modeImgName.c_str());

			String mengImgName = StringUtil::Format("PixelGunHallModeSelect-Meng%d", i + 1);
			m_mode_meng[i] = getWindow<GUIStaticImage>(mengImgName.c_str());

			String modeLockImgName = StringUtil::Format("PixelGunHallModeSelect-ModeLock%d", i + 1);
			m_mode_lock_img[i] = getWindow<GUIStaticImage>(modeLockImgName.c_str());

			String modeLvName = StringUtil::Format("PixelGunHallModeSelect-ModeLv%d", i + 1);
			m_mode_lv[i] = getWindow<GUIStaticText>(modeLvName.c_str());

			String modeNameName = StringUtil::Format("PixelGunHallModeSelect-ModeName%d", i + 1);
			m_mode_name[i] = getWindow<GUIStaticText>(modeNameName.c_str());

			for (int j = 0; j < MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM; ++j)
			{
				String modeSmallInfoImgName = StringUtil::Format("PixelGunHallModeSelect-ModeSmallInfo%d-%d", i + 1, j + 1);
				m_mode_small_info[i][j] = getWindow<GUIStaticImage>(modeSmallInfoImgName.c_str());

				String modeSmallInfoNumName = StringUtil::Format("PixelGunHallModeSelect-ModeSmallInfoNum%d-%d", i + 1, j + 1);
				m_mode_small_num[i][j] = getWindow<GUIStaticText>(modeSmallInfoNumName.c_str());
			}
		}

		for (int i = 0; i < MAX_PIXEL_GUNHALL_REWARD_NUM; ++i)
		{
			String rewardImgName = StringUtil::Format("PixelGunHallModeSelect-RandomReward%d", i + 1);
			m_reward_img[i] = getWindow<GUIStaticImage>(rewardImgName.c_str());

			String rewardNumName = StringUtil::Format("PixelGunHallModeSelect-RandomRewardTxt%d", i + 1);
			m_reward_num[i] = getWindow<GUIStaticText>(rewardNumName.c_str());
		}

		for (int i = 0; i < MAX_PIXEL_GUNHALL_FRAGMENT_NUM; ++i)
		{
			String fragMentJinduName = StringUtil::Format("PixelGunHallModeSelect-FragJindu%d", i + 1);
			m_fragment_jindu_img[i] = getWindow<GUIStaticImage>(fragMentJinduName.c_str());
			m_fragment_jindu_img[i]->SetVisible(false);
		}
		getWindow<GUIStaticText>("PixelGunHallModeSelect-TipBottom")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_BOTTOM_TIP).c_str());
		getWindow<GUIStaticText>("PixelGunHallModeSelect-SpecialRewardTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_BOTTOM_RANDOM_TIP).c_str());
		getWindow<GUIStaticText>("PixelGunHallModeSelect-BtnStorageTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_BTN_STORAGE_TXT).c_str());
		getWindow<GUIStaticText>("PixelGunHallModeSelect-BtnFightTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_BTN_FIGHT_TXT).c_str());
		getWindow<GUIStaticText>("PixelGunHallModeSelect-Title")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_TITLE_TXT).c_str());
		getWindow<GUIStaticText>("PixelGunHallModeSelect-FragMissionTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_FRAG_MISSION_TXT).c_str());
		
		m_closeBtn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunHallModelSelect::onCloseClick, this, std::placeholders::_1));
		m_storageBtn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunHallModelSelect::onStorageClick, this, std::placeholders::_1));
		m_fightBtn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunHallModelSelect::onFightClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(ShowPixelGunHallModeSelectEvent::subscribe(std::bind(&gui_pixelGunHallModelSelect::refreshAll, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPixelGunHallModeSelectReWardInfoEvent::subscribe(std::bind(&gui_pixelGunHallModelSelect::refreshBottomReward, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPixelGunHallMapSelectChangeEvent::subscribe(std::bind(&gui_pixelGunHallModelSelect::refreshMapSelect, this, std::placeholders::_1)));
		m_current_mode_index = 0;
	}

	void gui_pixelGunHallModelSelect::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_btn_fight_cd > 0)
			{
				m_fightBtn->SetEnabled(false);
				m_btn_fight_cd -= nTimeElapse;
				if (m_btn_fight_cd <= 0)
				{
					m_fightBtn->SetEnabled(true);
				}
			}

			if (m_mode_click_cd < MODE_CLICK_CD)
			{
				m_mode_click_cd += nTimeElapse;
			}
		}
	}

	void gui_pixelGunHallModelSelect::onShow()
	{
		m_current_mode_index = 0;
		m_mode_click_cd = 0;
	}

	bool gui_pixelGunHallModelSelect::onCloseClick(const EventArgs & events)
	{
		m_current_mode_index = 0;
		getParent()->showMainControl();
		playSoundByType(ST_ClosePanel);
		return true;
	}

	bool gui_pixelGunHallModelSelect::onStorageClick(const EventArgs & events)
	{
		OpenGunStoreEvent::emit();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_pixelGunHallModelSelect::onFightClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_current_mode_index < 0 || m_current_mode_index >= MAX_PIXEL_GUNHALL_MODE_NUM || m_current_mode_index >= (int)m_mode_info.size()) return true;

		String gameType = m_mode_info[m_current_mode_index].gameType;

		if (m_current_map_index < 0 || m_current_map_index >= (int)m_mode_info[m_current_mode_index].map_info.size()) return true;

		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			if (wallet)
			{
				i64 money = wallet->getMoneyByCoinId(CurrencyType::PLATFORM_MONEY);
				if (money < m_mode_info[m_current_mode_index].min_money)
				{
					i32 diff = (i32)(m_mode_info[m_current_mode_index].min_money - money);
					ShowLackOfMoneyTipDialogEvent::emit(diff, CurrencyType::PLATFORM_MONEY);
					return true;
				}
			}
			else
			{
				return true;
			}
		}

		String mapName = m_mode_info[m_current_mode_index].map_info[m_current_map_index].map_name;

		ClientNetwork::Instance()->getSender()->sendPixelGunFight(gameType, mapName);

		m_btn_fight_cd = FIGHT_BTN_CD;
		return true;
	}

	bool gui_pixelGunHallModelSelect::refreshAll(const String & data)
	{
		std::vector<ModeInfo> mode_info;

		if (getAllInfo(data, mode_info))
		{
			for (int i = 0; i < MAX_PIXEL_GUNHALL_MODE_NUM; ++i)
			{
				m_mode_bg[i]->SetVisible(false);
				m_mode_img[i]->SetVisible(false);
 				m_mode_lock_img[i]->SetVisible(true);
 				m_mode_meng[i]->SetVisible(true);
 				m_mode_select[i]->SetVisible(false);
 				for (int j = 0; j < MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM; ++j)
 				{
 					m_mode_small_info[i][j]->SetVisible(false);
 				}
			}
			m_mode_info.clear();
			for (int i = 0; i < (int)mode_info.size() && i < MAX_PIXEL_GUNHALL_MODE_NUM; ++i)
			{
				ModeInfo _mode_info;
				_mode_info.mode_img = mode_info[i].mode_img;
				_mode_info.isOpen = mode_info[i].isOpen;
				_mode_info.gameType = mode_info[i].gameType;
				_mode_info.mode_name = mode_info[i].mode_name;
				_mode_info.mode_info1 = mode_info[i].mode_info1;
				_mode_info.mode_info2 = mode_info[i].mode_info2;
				_mode_info.mode_info_num1 = mode_info[i].mode_info_num1;
				_mode_info.mode_info_num2 = mode_info[i].mode_info_num2;
				_mode_info.mode_info_show1 = mode_info[i].mode_info_show1;
				_mode_info.mode_info_show2 = mode_info[i].mode_info_show2;
				_mode_info.mode_tip = mode_info[i].mode_tip;
				_mode_info.mode_title = mode_info[i].mode_title;
				_mode_info.openLv = mode_info[i].openLv;
				_mode_info.index = mode_info[i].index;
				_mode_info.unlock = mode_info[i].unlock;
				_mode_info.min_money = mode_info[i].min_money;

				m_mode_bg[i]->SetVisible(true);
				m_mode_img[i]->SetVisible(true);

				if (mode_info[i].isOpen == 1)
				{
					m_mode_img[i]->SetImage(mode_info[i].mode_img.c_str());

					String modeLvTxt = StringUtil::Format("%d", mode_info[i].openLv);
					m_mode_lv[i]->SetText(modeLvTxt.c_str());

					if (mode_info[i].unlock)
					{
						m_mode_lock_img[i]->SetVisible(false);
						m_mode_meng[i]->SetVisible(false);
					}
					else
					{
						m_mode_lock_img[i]->SetVisible(true);
						m_mode_meng[i]->SetVisible(true);
					}

					if (mode_info[i].mode_info1.length() > 0)
					{
						m_mode_small_info[i][0]->SetImage(mode_info[i].mode_info1.c_str());
						m_mode_small_info[i][0]->SetVisible(mode_info[i].mode_info_show1);
						m_mode_small_num[i][0]->SetText(mode_info[i].mode_info_num1.c_str());
						m_mode_small_num[i][0]->SetVisible(mode_info[i].mode_info_show1);
					}

					if (mode_info[i].mode_info2.length() > 0)
					{
						m_mode_small_info[i][1]->SetImage(mode_info[i].mode_info2.c_str());
						m_mode_small_info[i][1]->SetVisible(mode_info[i].mode_info_show2);
						m_mode_small_num[i][1]->SetText(mode_info[i].mode_info_num2.c_str());
						m_mode_small_num[i][1]->SetVisible(mode_info[i].mode_info_show2);
					}

					if (i == m_current_mode_index)
					{
						m_mode_select[i]->SetVisible(true);
						m_mode_des_title->SetText(getString(mode_info[i].mode_title.c_str()).c_str());
						m_mode_des->SetText(getString(mode_info[i].mode_tip.c_str()).c_str());
					}
				}
				else
				{
					m_mode_meng[i]->SetVisible(false);
					for (int j = 0; j < MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM; ++j)
					{
						m_mode_small_info[i][j]->SetVisible(false);
					}
					m_mode_lock_img[i]->SetVisible(false);
					m_mode_img[i]->SetImage("set:pixelgunhallmodeselect.json image:mode_wen");
				}

				m_mode_name[i]->SetText(getString(mode_info[i].mode_name.c_str()).c_str());

				for (int j = 0; j < (int)mode_info[i].map_info.size(); ++j)
				{
					MapInfo _map_info;
					_map_info.map_id = mode_info[i].map_info[j].map_id;
					_map_info.map_img = mode_info[i].map_info[j].map_img;
					_map_info.map_name = mode_info[i].map_info[j].map_name;
					_map_info.map_show_name = mode_info[i].map_info[j].map_show_name;
					_map_info.small_info_img = mode_info[i].map_info[j].small_info_img;
					_map_info.openMoneyType = mode_info[i].map_info[j].openMoneyType;
					_map_info.openMoneyNum = mode_info[i].map_info[j].openMoneyNum;
					_map_info.unlock = mode_info[i].map_info[j].unlock;
					_map_info.map_size = mode_info[i].map_info[j].map_size;
					_map_info.map_type = mode_info[i].map_info[j].map_type;
					_map_info.fragment_gun_img = mode_info[i].map_info[j].fragment_gun_img;
					_map_info.fragment_gun_num_txt = mode_info[i].map_info[j].fragment_gun_num_txt;
					_map_info.fragment_jindu = mode_info[i].map_info[j].fragment_jindu;
					_map_info.random_reward_img1 = mode_info[i].map_info[j].random_reward_img1;
					_map_info.random_reward_img2 = mode_info[i].map_info[j].random_reward_img2;
					_map_info.random_reward_num1 = mode_info[i].map_info[j].random_reward_num1;
					_map_info.random_reward_num2 = mode_info[i].map_info[j].random_reward_num2;

					_mode_info.map_info.push_back(_map_info);
				}

				m_mode_info.push_back(_mode_info);
			}

			refreshAllList();
			refreshBottomReward(0);
		}

		return true;
	}

	bool gui_pixelGunHallModelSelect::getAllInfo(const String & data, std::vector<ModeInfo>& mode_info)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelGunHallModelSelect::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("mode"))
		{
			LordLogError("gui_pixelGunHallModelSelect::getAllInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr_mode = doc->FindMember("mode")->value.GetArray();
		for (size_t i = 0; i < arr_mode.Size(); ++i)
		{
			if (!arr_mode[i].HasMember("modeimg")
				|| !arr_mode[i].HasMember("is_open")
				|| !arr_mode[i].HasMember("name") 
				|| !arr_mode[i].HasMember("info1")
				|| !arr_mode[i].HasMember("info2")
				|| !arr_mode[i].HasMember("info_num1")
				|| !arr_mode[i].HasMember("info_num2")
				|| !arr_mode[i].HasMember("info_show1")
				|| !arr_mode[i].HasMember("info_show2")
				|| !arr_mode[i].HasMember("openLv")
				|| !arr_mode[i].HasMember("gameType")
				|| !arr_mode[i].HasMember("index")
				|| !arr_mode[i].HasMember("unlock")
				|| !arr_mode[i].HasMember("map")
				|| !arr_mode[i].HasMember("tip")
				|| !arr_mode[i].HasMember("tip_title")
				|| !arr_mode[i].HasMember("min_money"))
			{
				LordLogError("gui_pixelGunHallModelSelect::getAllInfo The game result content effect_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			
			String mode_img = arr_mode[i].FindMember("modeimg")->value.GetString();
			String name = arr_mode[i].FindMember("name")->value.GetString();
			String info1 = arr_mode[i].FindMember("info1")->value.GetString();
			String info2 = arr_mode[i].FindMember("info2")->value.GetString();
			String info_num1 = arr_mode[i].FindMember("info_num1")->value.GetString();
			String info_num2 = arr_mode[i].FindMember("info_num2")->value.GetString();
			bool info_show1 = arr_mode[i].FindMember("info_show1")->value.GetBool();
			bool info_show2 = arr_mode[i].FindMember("info_show2")->value.GetBool();
			int openLv = arr_mode[i].FindMember("openLv")->value.GetInt();
			int isOpen = arr_mode[i].FindMember("is_open")->value.GetInt();
			String gameType = arr_mode[i].FindMember("gameType")->value.GetString();
			int index = arr_mode[i].FindMember("index")->value.GetInt();
			bool unlock = arr_mode[i].FindMember("unlock")->value.GetBool();
			String tip = arr_mode[i].FindMember("tip")->value.GetString();
			String tip_title = arr_mode[i].FindMember("tip_title")->value.GetString();
			int min_money = arr_mode[i].FindMember("min_money")->value.GetInt();

			ModeInfo modeInfo;
			modeInfo.mode_img = mode_img;
			modeInfo.isOpen = isOpen;
			modeInfo.mode_name = name;
			modeInfo.mode_info1 = info1;
			modeInfo.mode_info2 = info2;
			modeInfo.mode_info_num1 = info_num1;
			modeInfo.mode_info_num2 = info_num2;
			modeInfo.mode_info_show1 = info_show1;
			modeInfo.mode_info_show2 = info_show2;
			modeInfo.openLv = openLv;
			modeInfo.gameType = gameType;
			modeInfo.index = index;
			modeInfo.unlock = unlock;
			modeInfo.mode_tip = tip;
			modeInfo.mode_title = tip_title;
			modeInfo.min_money = min_money;

			rapidjson::Value arr_map = arr_mode[i].FindMember("map")->value.GetArray();
			for (size_t j = 0; j < arr_map.Size(); ++j)
			{
				if (!arr_map[j].HasMember("map_id")
					|| !arr_map[j].HasMember("map_img")
					|| !arr_map[j].HasMember("map_name")
					|| !arr_map[j].HasMember("map_show_name")
					|| !arr_map[j].HasMember("small_info_img")
					|| !arr_map[j].HasMember("openMoneyType")
					|| !arr_map[j].HasMember("openMoneyNum")
					|| !arr_map[j].HasMember("unlock")
					|| !arr_map[j].HasMember("map_size")
					|| !arr_map[j].HasMember("map_type")
					|| !arr_map[j].HasMember("fragment_gun_img")
					|| !arr_map[j].HasMember("fragment_gun_num_txt")
					|| !arr_map[j].HasMember("fragment_jindu")
					|| !arr_map[j].HasMember("random_reward_img1")
					|| !arr_map[j].HasMember("random_reward_img2")
					|| !arr_map[j].HasMember("random_reward_num1")
					|| !arr_map[j].HasMember("random_reward_num2"))
				{
					LordLogError("gui_pixelGunHallModelSelect::getAllInfo The game result content effect_info missed some field. %d", i);
					LordDelete(doc);
					return false;
				}

				int map_id = arr_map[j].FindMember("map_id")->value.GetInt();
				String map_img = arr_map[j].FindMember("map_img")->value.GetString();
				String map_name = arr_map[j].FindMember("map_name")->value.GetString();
				String map_show_name = arr_map[j].FindMember("map_show_name")->value.GetString();
				String small_info_img = arr_map[j].FindMember("small_info_img")->value.GetString();
				int openMoneyType = arr_map[j].FindMember("openMoneyType")->value.GetInt();
				int openMoneyNum = arr_map[j].FindMember("openMoneyNum")->value.GetInt();
				bool unlock = arr_map[j].FindMember("unlock")->value.GetBool();
				String map_size = arr_map[j].FindMember("map_size")->value.GetString();
				int map_type = arr_map[j].FindMember("map_type")->value.GetInt();
				String fragment_gun_img = arr_map[j].FindMember("fragment_gun_img")->value.GetString();
				String fragment_gun_num_txt = arr_map[j].FindMember("fragment_gun_num_txt")->value.GetString();
				int fragment_jindu = arr_map[j].FindMember("fragment_jindu")->value.GetInt();
				String random_reward_img1 = arr_map[j].FindMember("random_reward_img1")->value.GetString();
				String random_reward_img2 = arr_map[j].FindMember("random_reward_img2")->value.GetString();
				int random_reward_num1 = arr_map[j].FindMember("random_reward_num1")->value.GetInt();
				int random_reward_num2 = arr_map[j].FindMember("random_reward_num2")->value.GetInt();

				MapInfo mapInfo;
				mapInfo.map_id = map_id;
				mapInfo.map_name = map_name;
				mapInfo.map_show_name = map_show_name;
				mapInfo.map_img = map_img;
				mapInfo.small_info_img = small_info_img;
				mapInfo.openMoneyType = openMoneyType;
				mapInfo.openMoneyNum = openMoneyNum;
				mapInfo.unlock = unlock;
				mapInfo.map_size = map_size;
				mapInfo.map_type = map_type;
				mapInfo.fragment_gun_img = fragment_gun_img;
				mapInfo.fragment_gun_num_txt = fragment_gun_num_txt;
				mapInfo.fragment_jindu = fragment_jindu;
				mapInfo.random_reward_img1 = random_reward_img1;
				mapInfo.random_reward_img2 = random_reward_img2;
				mapInfo.random_reward_num1 = random_reward_num1;
				mapInfo.random_reward_num2 = random_reward_num2;

				modeInfo.map_info.push_back(mapInfo);
			}

			mode_info.push_back(modeInfo);
		}
		return true;
	}

	bool gui_pixelGunHallModelSelect::onModeImgClick(int index)
	{
		if (m_mode_click_cd < MODE_CLICK_CD)
		{
			return true;
		}

		playSoundByType(ST_Click);
		if (index < 0 || index >= MAX_PIXEL_GUNHALL_MODE_NUM || index >= (int)m_mode_info.size()) return true;

		if (m_mode_info[index].isOpen == 0)
		{
			ShowPixelGunHallNotOpenEvent::emit(0);
			return true;
		}

		if (m_mode_info[index].unlock == false)
		{
			ShowPixelGunHallLvNotEnoughEvent::emit(m_mode_info[index].openLv, 0);
			return true;
		}
		
		m_current_mode_index = index;
		refreshAllSelf();
		refreshAllList();
		refreshBottomReward(0);

		return true;
	}

	void gui_pixelGunHallModelSelect::refreshAllSelf()
	{
		if (m_current_mode_index < 0 || m_current_mode_index >= MAX_PIXEL_GUNHALL_MODE_NUM || m_current_mode_index >= (int)m_mode_info.size()) return;

		for (int i = 0; i < MAX_PIXEL_GUNHALL_MODE_NUM; ++i)
		{
			m_mode_select[i]->SetVisible(false);
		}

		m_mode_select[m_current_mode_index]->SetVisible(true);
		m_mode_des_title->SetText(getString(m_mode_info[m_current_mode_index].mode_title.c_str()).c_str());
		m_mode_des->SetText(getString(m_mode_info[m_current_mode_index].mode_tip.c_str()).c_str());
	}

	void gui_pixelGunHallModelSelect::refreshAllList()
	{

		if (m_current_mode_index < 0 || m_current_mode_index >= (int)m_mode_info.size() || m_current_mode_index >= MAX_PIXEL_GUNHALL_MODE_NUM)
		{
			return;
		}

		m_mapInfoGridView->RemoveAllItems();

		for (int i = 0; i < (int)m_mode_info[m_current_mode_index].map_info.size(); ++i)
		{
			GUIPixelGunHallModeSelectMapItem::MapItemInfo map_info;
			map_info.map_id = m_mode_info[m_current_mode_index].map_info[i].map_id;
			map_info.map_img = m_mode_info[m_current_mode_index].map_info[i].map_img;
			map_info.small_info_img = m_mode_info[m_current_mode_index].map_info[i].small_info_img;
			map_info.map_name = m_mode_info[m_current_mode_index].map_info[i].map_name;
			map_info.map_show_name = m_mode_info[m_current_mode_index].map_info[i].map_show_name;
			map_info.unlock = m_mode_info[m_current_mode_index].map_info[i].unlock;
			map_info.map_size = m_mode_info[m_current_mode_index].map_info[i].map_size;
			map_info.openMoneyType = m_mode_info[m_current_mode_index].map_info[i].openMoneyType;
			map_info.openMoneyNum = m_mode_info[m_current_mode_index].map_info[i].openMoneyNum;
			map_info.map_type = m_mode_info[m_current_mode_index].map_info[i].map_type;

			GUIString ChildsName = StringUtil::Format("PixelGunHallModeSelectMapItem-%d", i + 1).c_str();

			if (GUIWindowManager::Instance()->GetGUIWindow(ChildsName.c_str()))
			{
				break;
			}

			float ContentSizeX = m_mapInfoGridView->GetPixelSize().x;
			float ContentSizeY = m_mapInfoGridView->GetPixelSize().y;
			float ChildWidth = (float)((ContentSizeX - 5.0 * 3) / 4);
			float ChildHeight = (float)((ContentSizeY - 10.0) / 2);
			GUIPixelGunHallModeSelectMapItem* pPixelGunHallModeSelectMapItem = dynamic_cast<GUIPixelGunHallModeSelectMapItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_PIXEL_GUNHALL_MODE_SELECT_MAP_ITEM, ChildsName.c_str()));
			pPixelGunHallModeSelectMapItem->RefreshItem(map_info, i);
			pPixelGunHallModeSelectMapItem->SetWidth({ 0.f, ChildWidth });
			pPixelGunHallModeSelectMapItem->SetHeight({ 0.f, ChildHeight });
			pPixelGunHallModeSelectMapItem->SetTouchable(false);
			pPixelGunHallModeSelectMapItem->SetClipChild(true);
			m_mapInfoGridView->AddItem(pPixelGunHallModeSelectMapItem);

			if (i == 0)
			{
				pPixelGunHallModeSelectMapItem->updateSelect(true);
			}
		}
	}

	bool gui_pixelGunHallModelSelect::refreshBottomReward(int map_index)
	{
		if (m_current_mode_index < 0 || m_current_mode_index >= (int)m_mode_info.size() || m_current_mode_index >= MAX_PIXEL_GUNHALL_MODE_NUM)
		{
			return true;
		}

		if (map_index < 0 || map_index >= (int)m_mode_info[m_current_mode_index].map_info.size())
		{
			return true;
		}

		m_current_map_index = map_index;

		if (m_mode_info[m_current_mode_index].map_info[map_index].map_type == GUIPixelGunHallModeSelectMapItem::MT_RANDOM)
		{
			m_fragLayout->SetVisible(false);
			m_rewardLayout->SetVisible(true);

			if (m_mode_info[m_current_mode_index].map_info[map_index].random_reward_img1.length() > 0)
			{
				m_reward_img[0]->SetImage(m_mode_info[m_current_mode_index].map_info[map_index].random_reward_img1.c_str());
				m_reward_img[0]->SetVisible(true);
			}
			else
			{
				m_reward_img[0]->SetVisible(false);
			}

			if (m_mode_info[m_current_mode_index].map_info[map_index].random_reward_img2.length() > 0)
			{
				m_reward_img[1]->SetImage(m_mode_info[m_current_mode_index].map_info[map_index].random_reward_img2.c_str());
				m_reward_img[1]->SetVisible(true);
			}
			else
			{
				m_reward_img[1]->SetVisible(false);
			}

			String rewardNumName1 = StringUtil::Format("+%d", m_mode_info[m_current_mode_index].map_info[map_index].random_reward_num1);
			m_reward_num[0]->SetText(rewardNumName1.c_str());

			String rewardNumName2 = StringUtil::Format("+%d", m_mode_info[m_current_mode_index].map_info[map_index].random_reward_num2);
			m_reward_num[1]->SetText(rewardNumName2.c_str());
		}
		else if (m_mode_info[m_current_mode_index].map_info[map_index].map_type == GUIPixelGunHallModeSelectMapItem::MT_FRAGMENT)
		{
			m_fragLayout->SetVisible(true);
			m_rewardLayout->SetVisible(false);

			if (m_mode_info[m_current_mode_index].map_info[map_index].fragment_gun_img.length() > 0)
			{
				m_rightFragmentImg->SetImage(m_mode_info[m_current_mode_index].map_info[map_index].fragment_gun_img.c_str());
				m_rightFragmentNumTxt->SetText(m_mode_info[m_current_mode_index].map_info[map_index].fragment_gun_num_txt.c_str());
				m_rightFragmentImg->SetVisible(true);
				m_rightFragmentNumTxt->SetVisible(true);
			}
			else
			{
				m_rightFragmentImg->SetVisible(false);
				m_rightFragmentNumTxt->SetVisible(false);
			}

			for (int i = 0; i < MAX_PIXEL_GUNHALL_FRAGMENT_NUM; ++i)
			{
				m_fragment_jindu_img[i]->SetVisible(false);
			}

			for (int i = 0; i < m_mode_info[m_current_mode_index].map_info[map_index].fragment_jindu && i < MAX_PIXEL_GUNHALL_FRAGMENT_NUM; ++i)
			{
				m_fragment_jindu_img[i]->SetVisible(true);
			}
			
		}
		else
		{
			m_fragLayout->SetVisible(false);
			m_rewardLayout->SetVisible(false);
		}
		return true;
	}

	bool gui_pixelGunHallModelSelect::refreshMapSelect(int map_index)
	{
		if (!m_mapInfoGridView) return true;

		int count = m_mapInfoGridView->GetItemCount();

		for (int i = 0; i < count; ++i)
		{
			if (map_index != i)
			{
				if (dynamic_cast<GUIPixelGunHallModeSelectMapItem*>(m_mapInfoGridView->getChildWindow(i)))
				{
					dynamic_cast<GUIPixelGunHallModeSelectMapItem*>(m_mapInfoGridView->getChildWindow(i))->updateSelect(false);
				}
			}
		}
		return true;
	}

}
