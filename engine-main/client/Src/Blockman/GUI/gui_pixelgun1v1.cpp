#include "gui_pixelgun1v1.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "UI/GUISystem.h"
#include "External/cjson.h"
#include "Render/TextureAtlas.h"
#include "GUI/GuiDef.h"
#include "Setting/ActionStateSetting.h"
#include "Entity/EntityPlayer.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_pixelgun1v1::gui_pixelgun1v1()
		: gui_layout("PixelGunGame1v1.json")
	{
	}

	gui_pixelgun1v1::~gui_pixelgun1v1()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelgun1v1::onLoad()
	{
		m_time_txt = getWindow<GUIStaticText>("PixelGunGame1v1-TimeTxt");
		m_left_select_box_img = getWindow<GUIStaticImage>("PixelGunGame1v1-BoxLeft");
		m_right_select_box_img = getWindow<GUIStaticImage>("PixelGunGame1v1-BoxRight");
		m_left_select_box_des = getWindow<GUIStaticText>("PixelGunGame1v1-BoxLeftDes");
		m_right_select_box_des = getWindow<GUIStaticText>("PixelGunGame1v1-BoxRightDes");
		m_left_rank_img = getWindow<GUIStaticImage>("PixelGunGame1v1-ActorLeftRankImg");
		m_right_rank_img = getWindow<GUIStaticImage>("PixelGunGame1v1-ActorRightRankImg");
		m_left_name_txt = getWindow<GUIStaticText>("PixelGunGame1v1-ActorLeftNameTxt");
		m_right_name_txt = getWindow<GUIStaticText>("PixelGunGame1v1-ActorRightNameTxt");
		m_left_honor_txt = getWindow<GUIStaticText>("PixelGunGame1v1-ActorLeftHonorTxt");
		m_right_honor_txt = getWindow<GUIStaticText>("PixelGunGame1v1-ActorRightHonorTxt");
		m_rematch_btn = getWindow<GUIButton>("PixelGunGame1v1-BtnRematch");
		m_exit_btn = getWindow<GUIButton>("PixelGunGame1v1-BtnExit");
		m_select_box_title = getWindow<GUIStaticText>("PixelGunGame1v1-SelectBoxTitle");
		m_final_select_box = getWindow<GUIStaticImage>("PixelGunGame1v1-FinalBox");
		m_final_select_des = getWindow<GUIStaticText>("PixelGunGame1v1-FinalBoxDes");
		m_vs_img = getWindow<GUIStaticImage>("PixelGunGame1v1-SelectVs");
		m_leftmode = getWindow<GuiActorWindow>("PixelGunGame1v1-ActorLeftActorWin");
		m_rightmode = getWindow<GuiActorWindow>("PixelGunGame1v1-ActorRightActorWin");
		m_box_left_wenhao = getWindow<GUIStaticImage>("PixelGunGame1v1-BoxLeftWenhao");
		m_box_right_wenhao = getWindow<GUIStaticImage>("PixelGunGame1v1-BoxRightWenhao");
		m_box_final_wenhao = getWindow<GUIStaticImage>("PixelGunGame1v1-FinalBoxWenhao");

		for (int i = 0; i < MAX_BOX_NUM; ++i)
		{
			String boxImgName = StringUtil::Format("PixelGunGame1v1-AllBox%d", i + 1);
			m_box_img[i] = getWindow<GUIStaticImage>(boxImgName.c_str());

			String boxMoneyName = StringUtil::Format("PixelGunGame1v1-AllBoxMoneyTxt%d", i + 1);
			m_box_money[i] = getWindow<GUIStaticText>(boxMoneyName.c_str());

			String boxSelectName = StringUtil::Format("PixelGunGame1v1-AllBoxSelect%d", i + 1);
			m_box_select[i] = getWindow<GUIStaticImage>(boxSelectName.c_str());

			m_box_img[i]->subscribeEvent(EventWindowClick, std::bind(&gui_pixelgun1v1::onBoxImgClick, this, std::placeholders::_1, i));
		}

		m_rematch_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgun1v1::onRematchClick, this, std::placeholders::_1));
		m_exit_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgun1v1::onCloseClick, this, std::placeholders::_1));
		m_box_left_wenhao->subscribeEvent(EventWindowClick, std::bind(&gui_pixelgun1v1::onWenHaoClick, this, std::placeholders::_1, G1WH_LEFT));
		m_box_right_wenhao->subscribeEvent(EventWindowClick, std::bind(&gui_pixelgun1v1::onWenHaoClick, this, std::placeholders::_1, G1WH_RIGHT));
		m_box_final_wenhao->subscribeEvent(EventWindowClick, std::bind(&gui_pixelgun1v1::onWenHaoClick, this, std::placeholders::_1, G1WH_FiNAL));

		getWindow<GUIStaticText>("PixelGunGame1v1-Title")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_1V1_TITLE));
		getWindow<GUIStaticText>("PixelGunGame1v1-TimeTitleTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_1V1_TIME_TITLE_TXT));
		getWindow<GUIStaticText>("PixelGunGame1v1-AllBoxDes")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_1V1_ALLBOX_DES));
		getWindow<GUIStaticText>("PixelGunGame1v1-BtnRematchTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_1V1_BTN_REMATCH_TXT));
		getWindow<GUIStaticText>("PixelGunGame1v1-BtnExitTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_1V1_BTN_EXIT_TXT));

		m_subscriptionGuard.add(ShowPixelGun1v1Event::subscribe(std::bind(&gui_pixelgun1v1::refreshAll, this, std::placeholders::_1)));
	}

	void gui_pixelgun1v1::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_gun_1v1_data.time > 0)
			{
				m_gun_1v1_data.time = m_gun_1v1_data.time - nTimeElapse;

				String msg = StringUtil::Format("%d", (int)(m_gun_1v1_data.time / 1000 + 1));
				m_time_txt->SetText(msg.c_str());
				m_time_txt->SetVisible(true);
			}
			else
			{
				m_time_txt->SetVisible(false);
			}
		}
	}

	void gui_pixelgun1v1::onShow()
	{
		
	}

	bool gui_pixelgun1v1::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		ClientNetwork::Instance()->getSender()->sendPixelGunExit();
		return true;
	}

	bool gui_pixelgun1v1::onRematchClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->showMainControl();
		ClientNetwork::Instance()->getSender()->sendPixelGunRematch();
		return true;
	}

	bool gui_pixelgun1v1::onBoxImgClick(const EventArgs & events, int index)
	{
		playSoundByType(ST_Click);

		if (index < 0 || index >= MAX_BOX_NUM)
		{
			return true;
		}

		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			if (wallet)
			{
				i64 money = wallet->getMoneyByCoinId(CurrencyType::PLATFORM_MONEY);
				if (money < m_gun_1v1_data.box_money[index])
				{
					i32 diff = i32(m_gun_1v1_data.box_money[index] - money);
					ShowLackOfMoneyTipDialogEvent::emit(diff, CurrencyType::PLATFORM_MONEY);
					return true;
				}
			}
			else
			{
				return true;
			}
		}

		refreshBoxSelect(index);
	
		return true;
	}

	void gui_pixelgun1v1::refreshBoxSelect(int index)
	{
		if (index < 0 || index >= MAX_BOX_NUM)
		{
			return;
		}

		for (int i = 0; i < MAX_BOX_NUM; ++i)
		{
			if (index == i)
			{
				m_box_select[i]->SetVisible(true);
			}
			else
			{
				m_box_select[i]->SetVisible(false);
			}
		}

		ClientNetwork::Instance()->getSender()->sendPixelGunSelectBox(index + 1);
	}

	bool gui_pixelgun1v1::onWenHaoClick(const EventArgs & events, int wenhao)
	{
		if (wenhao == G1WH_LEFT)
		{
			ShowLotteryChestDetailEvent::emit(m_gun_1v1_data.left_select_index);
		}
		else if (wenhao == G1WH_RIGHT)
		{
			ShowLotteryChestDetailEvent::emit(m_gun_1v1_data.right_select_index);
		}
		else if (wenhao == G1WH_FiNAL)
		{
			ShowLotteryChestDetailEvent::emit(m_gun_1v1_data.final_select_index);
		}
		playSoundByType(ST_Click);

		return true;
	}

	bool gui_pixelgun1v1::refreshAll(const String & data)
	{
		if (getAllInfo(data))
		{
			if (m_gun_1v1_data.state == GS_PREPARE)
			{
				if (m_gun_1v1_data.left_select_index > 0 && m_gun_1v1_data.left_select_index <= MAX_BOX_NUM)
				{
					String leftSelectBoxImgName = StringUtil::Format("set:pixelgun1v1.json image:box%d", m_gun_1v1_data.left_select_index);
					m_left_select_box_img->SetImage(leftSelectBoxImgName.c_str());
					m_left_select_box_img->SetVisible(true);

					if (m_gun_1v1_data.left_select_des.length() > 0)
					{
						m_left_select_box_des->SetText(getString(m_gun_1v1_data.left_select_des.c_str()).c_str());
						m_left_select_box_des->SetVisible(true);
					}
				}
				else
				{
					m_left_select_box_img->SetVisible(false);
					m_left_select_box_des->SetVisible(false);
				}

				if (m_gun_1v1_data.right_select_index > 0 && m_gun_1v1_data.right_select_index <= MAX_BOX_NUM)
				{
					String rightSelectBoxImgName = StringUtil::Format("set:pixelgun1v1.json image:box%d", m_gun_1v1_data.right_select_index);
					m_right_select_box_img->SetImage(rightSelectBoxImgName.c_str());
					m_right_select_box_img->SetVisible(true);

					if (m_gun_1v1_data.right_select_des.length() > 0)
					{
						m_right_select_box_des->SetText(getString(m_gun_1v1_data.right_select_des.c_str()).c_str());
						m_right_select_box_des->SetVisible(true);
					}
				}
				else
				{
					m_right_select_box_img->SetVisible(false);
					m_right_select_box_des->SetVisible(false);
				}
				
				m_final_select_box->SetVisible(false);
				m_vs_img->SetVisible(true);
				m_exit_btn->SetVisible(true);
				m_rematch_btn->SetVisible(true);
			}
			else
			{
				m_left_select_box_img->SetVisible(false);
				m_right_select_box_img->SetVisible(false);
				m_vs_img->SetVisible(false);
				
				if (m_gun_1v1_data.final_select_index > 0 && m_gun_1v1_data.final_select_index <= MAX_BOX_NUM)
				{
					String finalBoxImgName = StringUtil::Format("set:pixelgun1v1.json image:box%d", m_gun_1v1_data.final_select_index);
					m_final_select_box->SetImage(finalBoxImgName.c_str());
					m_final_select_box->SetVisible(true);

					if (m_gun_1v1_data.final_select_des.length() > 0)
					{
						m_final_select_des->SetText(getString(m_gun_1v1_data.final_select_des.c_str()).c_str());
						m_final_select_des->SetVisible(true);
					}
					else
					{
						m_final_select_des->SetVisible(false);
					}
				}

				m_exit_btn->SetVisible(false);
				m_rematch_btn->SetVisible(false);
			}

			if (m_gun_1v1_data.left_honor_stage > 0 && m_gun_1v1_data.left_honor_stage <= GHS_COUNT)
			{
				String leftHonorStageImgName = StringUtil::Format("set:pixelgunhallmodeselect.json image:rank%d", m_gun_1v1_data.left_honor_stage);
				m_left_rank_img->SetImage(leftHonorStageImgName.c_str());
				m_left_rank_img->SetVisible(true);
			}
			else
			{
				m_left_rank_img->SetVisible(false);
			}

			if (m_gun_1v1_data.right_honor_stage > 0 && m_gun_1v1_data.right_honor_stage <= GHS_COUNT)
			{
				String rightHonorStageImgName = StringUtil::Format("set:pixelgunhallmodeselect.json image:rank%d", m_gun_1v1_data.right_honor_stage);
				m_right_rank_img->SetImage(rightHonorStageImgName.c_str());
				m_right_rank_img->SetVisible(true);
			}
			else
			{
				m_right_rank_img->SetVisible(false);
			}

			m_left_name_txt->SetText(m_gun_1v1_data.left_name.c_str());
			m_right_name_txt->SetText(m_gun_1v1_data.right_name.c_str());

			String leftHonorScoreTxt = StringUtil::Format("%d", m_gun_1v1_data.left_honor_score);
			m_left_honor_txt->SetText(leftHonorScoreTxt.c_str());

			String rightHonorScoreTxt = StringUtil::Format("%d", m_gun_1v1_data.right_honor_score);
			m_right_honor_txt->SetText(rightHonorScoreTxt.c_str());

			for (int i = 0; i < MAX_BOX_NUM; ++i)
			{
				if (i + 1 == m_gun_1v1_data.self_select_index)
				{
					m_box_select[i]->SetVisible(true);
				}
				else
				{
					m_box_select[i]->SetVisible(false);
				}

				String boxMoneyTxt = StringUtil::Format("%d", m_gun_1v1_data.box_money[i]);
				m_box_money[i]->SetText(boxMoneyTxt.c_str());
			}

			if (m_gun_1v1_data.left_entityId > 0)
			{
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				Entity* pEntity = dataCache->getEntityByServerId(m_gun_1v1_data.left_entityId);
				if (pEntity)
				{
					auto pPlayerLeft = dynamic_cast<EntityPlayer*>(pEntity);
					if (pPlayerLeft)
					{
						m_leftmode->SetActor(pPlayerLeft->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", 45.0f);
						m_leftmode->SetCustomColor(pPlayerLeft->m_skinColor);
						m_leftmode->reflreshArmor(pPlayerLeft);
						m_leftmode->SetActorScale(0.6f);

						if (m_gun_1v1_data.left_itemId > 0)
						{
							m_leftmode->SetHoldItem(pPlayerLeft, m_gun_1v1_data.left_itemId);
							String SkillName = ActionStateSetting::getSkillName((int)AS_GUN_FIRE, m_gun_1v1_data.left_itemId, 0);
							if (!SkillName.empty())
							{
								m_leftmode->PlaySkill(SkillName);
							}
						}
						
					}	
				}
			}
			else
			{
				m_leftmode->SetVisible(false);
			}

			if (m_gun_1v1_data.right_entityId > 0)
			{
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				Entity* pEntity = dataCache->getEntityByServerId(m_gun_1v1_data.right_entityId);
				if (pEntity)
				{
					auto pPlayerRight = dynamic_cast<EntityPlayer*>(pEntity);
					if (pPlayerRight)
					{
						m_rightmode->SetActor(pPlayerRight->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", -45.0f);
						m_rightmode->SetCustomColor(pPlayerRight->m_skinColor);
						m_rightmode->reflreshArmor(pPlayerRight);
						m_rightmode->SetActorScale(0.6f);

						if (m_gun_1v1_data.right_itemId > 0)
						{
							m_rightmode->SetHoldItem(pPlayerRight, m_gun_1v1_data.right_itemId);
							String SkillName = ActionStateSetting::getSkillName((int)AS_GUN_FIRE, m_gun_1v1_data.right_itemId, 0);
							if (!SkillName.empty())
							{
								m_rightmode->PlaySkill(SkillName);
							}
						}
					}
				}
			}
			else
			{
				m_leftmode->SetVisible(false);
			}
 		}

		return true;
	}

	bool gui_pixelgun1v1::getAllInfo(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgun1v1::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("state") 
			|| !doc->HasMember("time")
			|| !doc->HasMember("left_entityId")
			|| !doc->HasMember("right_entityId")
			|| !doc->HasMember("left_itemId")
			|| !doc->HasMember("right_itemId")
			|| !doc->HasMember("left_name")
			|| !doc->HasMember("right_name")
			|| !doc->HasMember("left_honor_stage")
			|| !doc->HasMember("left_honor_score")
			|| !doc->HasMember("right_honor_stage")
			|| !doc->HasMember("right_honor_score")
			|| !doc->HasMember("left_select_index")
			|| !doc->HasMember("left_select_des")
			|| !doc->HasMember("right_select_index")
			|| !doc->HasMember("right_select_des")
			|| !doc->HasMember("self_select_index")
			|| !doc->HasMember("box_money1")
			|| !doc->HasMember("box_money2")
			|| !doc->HasMember("box_money3")
			|| !doc->HasMember("box_money4")
			|| !doc->HasMember("final_select_index")
			|| !doc->HasMember("final_select_des"))
		{
			LordLogError("gui_pixelgun1v1::getAllInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_gun_1v1_data.state = doc->FindMember("state")->value.GetInt();
		int time = doc->FindMember("time")->value.GetInt() * 1000;
		if (time > 0)
		{
			m_gun_1v1_data.time = doc->FindMember("time")->value.GetInt() * 1000;
		}
		else
		{
			m_gun_1v1_data.time = 0;
		}
		
		m_gun_1v1_data.left_entityId = doc->FindMember("left_entityId")->value.GetInt();
		m_gun_1v1_data.right_entityId = doc->FindMember("right_entityId")->value.GetInt();
		m_gun_1v1_data.left_itemId = doc->FindMember("left_itemId")->value.GetInt();
		m_gun_1v1_data.right_itemId = doc->FindMember("right_itemId")->value.GetInt();
		m_gun_1v1_data.left_name = doc->FindMember("left_name")->value.GetString();
		m_gun_1v1_data.right_name = doc->FindMember("right_name")->value.GetString();
		m_gun_1v1_data.left_honor_stage = doc->FindMember("left_honor_stage")->value.GetInt();
		m_gun_1v1_data.left_honor_score = doc->FindMember("left_honor_score")->value.GetInt();
		m_gun_1v1_data.right_honor_stage = doc->FindMember("right_honor_stage")->value.GetInt();
		m_gun_1v1_data.right_honor_score = doc->FindMember("right_honor_score")->value.GetInt();
		m_gun_1v1_data.left_select_index = doc->FindMember("left_select_index")->value.GetInt();
		m_gun_1v1_data.left_select_des = doc->FindMember("left_select_des")->value.GetString();
		m_gun_1v1_data.right_select_index = doc->FindMember("right_select_index")->value.GetInt();
		m_gun_1v1_data.right_select_des = doc->FindMember("right_select_des")->value.GetString();
		m_gun_1v1_data.self_select_index = doc->FindMember("self_select_index")->value.GetInt();
		m_gun_1v1_data.final_select_index = doc->FindMember("final_select_index")->value.GetInt();
		m_gun_1v1_data.final_select_des = doc->FindMember("final_select_des")->value.GetString();

		for (int i = 0; i < MAX_BOX_NUM; ++i)
		{
			String boxMoneyName = StringUtil::Format("box_money%d", i + 1);
			m_gun_1v1_data.box_money[i] = doc->FindMember(boxMoneyName.c_str())->value.GetInt();
		}

 		return true;
 	}
}
