#include "gui_pixelgunrevive.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiDef.h"
#include "Setting/ActionStateSetting.h"
#include "Entity/EntityPlayer.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_pixelgunrevive::gui_pixelgunrevive()
		: gui_layout("PixelGunRevive.json")
	{
	}

	gui_pixelgunrevive::~gui_pixelgunrevive()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelgunrevive::onLoad()
	{
		m_killer_use_gun_img = getWindow<GUIStaticImage>("PixelGunRevive-PlayerUseGunImg");
		m_killer_use_gun_name = getWindow<GUIStaticText>("PixelGunRevive-PlayerUserdGunName");
		m_battle_btn = getWindow<GUIButton>("PixelGunRevive-BattleBtn");
		m_battle_time_txt = getWindow<GUIStaticText>("PixelGunRevive-BattleBtnTxt");
		m_killer_mode = getWindow<GuiActorWindow>("PixelGunRevive-ActorWin");
		m_health_value = getWindow<GUIProgressBar>("PixelGunRevive-Health-Value");
		m_defense_value = getWindow<GUIProgressBar>("PixelGunRevive-Defense-Value");
		m_health_number = getWindow<GUIStaticText>("PixelGunRevive-Health-Number");
		m_defense_number = getWindow<GUIStaticText>("PixelGunRevive-Defense-Number");
		m_killer_name = getWindow<GUIStaticText>("PixelGunRevive-KillerName");
		m_killer_block_img = getWindow<GUIStaticImage>("PixelGunRevive-PlayerBlockImg");
		m_killer_block_name = getWindow<GUIStaticText>("PixelGunRevive-PlayerBlockName");
		m_white_meng = getWindow<GUILayout>("PixelGunRevive-White");

		for (int i = 0; i < MAX_ARMS_NUM; ++i)
		{
			String armsImgName = StringUtil::Format("PixelGunRevive-PlayerUseArmsImg%d", i + 1);
			m_killer_arms_img[i] = getWindow<GUIStaticImage>(armsImgName.c_str());

			String armsTxtName = StringUtil::Format("PixelGunRevive-PlayerUserdArmsName%d", i + 1);
			m_killer_arms_name[i] = getWindow<GUIStaticText>(armsTxtName.c_str());
		}

		for (int i = 0; i < MAX_GAD_GETS_NUM; ++i)
		{
			String getsgadImgName = StringUtil::Format("PixelGunRevive-PlayerGadgetsImg%d", i + 1);
			m_killer_gadgets_img[i] = getWindow<GUIStaticImage>(getsgadImgName.c_str());

			String getsgadTxtName = StringUtil::Format("PixelGunRevive-PlayerGadgetsName%d", i + 1);
			m_killer_gadgets_name[i] = getWindow<GUIStaticText>(getsgadTxtName.c_str());
		}

		m_battle_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgunrevive::onBattleClick, this, std::placeholders::_1));

		getWindow<GUIStaticText>("PixelGunRevive-PlayerUserTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_PLAYER_USED_TXT));
		getWindow<GUIStaticText>("PixelGunRevive-PlayerArmysTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_PLAYER_ARMS_TXT));
		getWindow<GUIStaticText>("PixelGunRevive-PlayerGadgetsTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_PLAYER_GADGETS_TXT));
		getWindow<GUIStaticText>("PixelGunRevive-BattleBtnTxt")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_BTN_BATTLE_TXT));

		m_subscriptionGuard.add(ShowPixelGunReviveEvent::subscribe(std::bind(&gui_pixelgunrevive::refreshAll, this, std::placeholders::_1)));
	}

	void gui_pixelgunrevive::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_pixel_gun_revive_data.battle_time > 0)
			{
				m_pixel_gun_revive_data.battle_time = m_pixel_gun_revive_data.battle_time - nTimeElapse;

				String msg = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUN_BTN_BATTLE_TIME_TXT).c_str(), (int)(m_pixel_gun_revive_data.battle_time / 1000 + 1));
				m_battle_time_txt->SetText(msg.c_str());
				m_battle_btn->SetEnabled(false);
				m_battle_btn->SetTouchable(false);
			}
			else
			{
				String msg = getString(LanguageKey::GUI_PIXEL_GUN_BTN_BATTLE_TXT).c_str();
				m_battle_time_txt->SetText(msg.c_str());
				m_battle_btn->SetEnabled(true);
				m_battle_btn->SetTouchable(true);
			}

			if (m_white_time > 0)
			{
				m_white_time--;
				m_white_meng->SetBackgroundColor(Color(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.0f / WHITE_TIME * m_white_time));
				if (m_white_time <= 0)
				{
					m_white_meng->SetVisible(false);
				}
			}
		}
	}

	void gui_pixelgunrevive::onShow()
	{
		m_white_time = WHITE_TIME;
		m_white_meng->SetVisible(true);
	}

	bool gui_pixelgunrevive::onBattleClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		ClientNetwork::Instance()->getSender()->sendPixelGunRevive();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_pixelgunrevive::refreshAll(const String & data)
	{
		if (getAllInfo(data))
		{
			if (m_pixel_gun_revive_data.killer_use_gun_img.length() > 0)
			{
				m_killer_use_gun_img->SetVisible(true);
				m_killer_use_gun_img->SetImage(m_pixel_gun_revive_data.killer_use_gun_img.c_str());
				m_killer_use_gun_name->SetText(getString(m_pixel_gun_revive_data.killer_use_gun_name.c_str()).c_str());
			}
			else
			{
				m_killer_use_gun_img->SetVisible(false);
			}
			

			String name = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUN_REVIVE_KILLNAME).c_str(), m_pixel_gun_revive_data.killer_name.c_str());
			m_killer_name->SetText(name.c_str());

			if (m_pixel_gun_revive_data.killer_block_img.length() > 0)
			{
				m_killer_block_img->SetImage(m_pixel_gun_revive_data.killer_block_img.c_str());
				m_killer_block_img->SetVisible(true);
				m_killer_block_name->SetText(getString(m_pixel_gun_revive_data.killer_block_name.c_str()).c_str());
			}
			else 
			{
				m_killer_block_img->SetVisible(false);
			}
			

			for (int i = 0; i < MAX_ARMS_NUM; ++i)
			{
				if (m_pixel_gun_revive_data.killer_arms_img[i].length() > 0)
				{
					m_killer_arms_img[i]->SetImage(m_pixel_gun_revive_data.killer_arms_img[i].c_str());
					m_killer_arms_img[i]->SetVisible(true);
					m_killer_arms_name[i]->SetText(getString(m_pixel_gun_revive_data.killer_arms_name[i].c_str()).c_str());
				}
				else 
				{
					m_killer_arms_img[i]->SetVisible(false);
				}
			}

			for (int i = 0; i < MAX_GAD_GETS_NUM; ++i)
			{
				if (m_pixel_gun_revive_data.killer_gadgets_img[i].length() > 0)
				{
					m_killer_gadgets_img[i]->SetImage(m_pixel_gun_revive_data.killer_gadgets_img[i].c_str());
					m_killer_gadgets_img[i]->SetVisible(true);
					m_killer_gadgets_name[i]->SetText(getString(m_pixel_gun_revive_data.killer_gadgets_name[i].c_str()).c_str());
				}
				else
				{
					m_killer_gadgets_img[i]->SetVisible(false);
				}
			}

			if (m_pixel_gun_revive_data.max_hp > 0)
			{
				m_health_value->SetProgress(m_pixel_gun_revive_data.cur_hp * 1.0f / m_pixel_gun_revive_data.max_hp);
				m_health_number->SetText(StringUtil::Format("%d/%d", m_pixel_gun_revive_data.cur_hp, m_pixel_gun_revive_data.max_hp).c_str());
			}

			if (m_pixel_gun_revive_data.max_defence > 0)
			{
				m_defense_value->SetProgress(m_pixel_gun_revive_data.cur_defence * 1.0f / m_pixel_gun_revive_data.max_defence);
				m_defense_number->SetText(StringUtil::Format("%d/%d", m_pixel_gun_revive_data.cur_defence, m_pixel_gun_revive_data.max_defence).c_str());
			}

			if (m_pixel_gun_revive_data.killer_entityId > 0)
			{
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				Entity* pEntity = dataCache->getEntityByServerId(m_pixel_gun_revive_data.killer_entityId);
				if (pEntity)
				{
					auto pPlayer = dynamic_cast<EntityPlayer*>(pEntity);
					if (pPlayer)
					{
						m_killer_mode->SetActor(pPlayer->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", 45.0f);
						m_killer_mode->SetCustomColor(pPlayer->m_skinColor);
						m_killer_mode->reflreshArmor(pPlayer);
						m_killer_mode->SetActorScale(0.5f);

						if (m_pixel_gun_revive_data.killer_itemId > 0)
						{
							m_killer_mode->SetHoldItem(pPlayer, m_pixel_gun_revive_data.killer_itemId);
							String SkillName = ActionStateSetting::getSkillName((int)AS_GUN_FIRE, m_pixel_gun_revive_data.killer_itemId, 0);
							if (!SkillName.empty())
							{
								m_killer_mode->PlaySkill(SkillName);
							}
						}
					}
				}
			}
		}

		return true;
	}

	bool gui_pixelgunrevive::getAllInfo(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgunrevive::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("killer_use_gun_img")
			|| !doc->HasMember("killer_use_gun_name")
			|| !doc->HasMember("killer_name")
			|| !doc->HasMember("killer_block_img")
			|| !doc->HasMember("killer_block_name")
			|| !doc->HasMember("battle_time")
			|| !doc->HasMember("killer_entityId")
			|| !doc->HasMember("killer_itemId")
			|| !doc->HasMember("arms")
			|| !doc->HasMember("gadgets")
			|| !doc->HasMember("cur_hp")
			|| !doc->HasMember("max_hp")
			|| !doc->HasMember("cur_defence")
			|| !doc->HasMember("max_defence"))
		{
			LordLogError("gui_pixelgunrevive::getAllInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_pixel_gun_revive_data.battle_time = doc->FindMember("battle_time")->value.GetInt();
		int time = doc->FindMember("battle_time")->value.GetInt() * 1000;
		if (time > 0)
		{
			m_pixel_gun_revive_data.battle_time = doc->FindMember("battle_time")->value.GetInt() * 1000;
		}
		else
		{
			m_pixel_gun_revive_data.battle_time = 0;
		}

		m_pixel_gun_revive_data.killer_use_gun_img = doc->FindMember("killer_use_gun_img")->value.GetString();
		m_pixel_gun_revive_data.killer_use_gun_name = doc->FindMember("killer_use_gun_name")->value.GetString();
		m_pixel_gun_revive_data.killer_name = doc->FindMember("killer_name")->value.GetString();
		m_pixel_gun_revive_data.killer_block_img = doc->FindMember("killer_block_img")->value.GetString();
		m_pixel_gun_revive_data.killer_block_name = doc->FindMember("killer_block_name")->value.GetString();
		m_pixel_gun_revive_data.killer_entityId = doc->FindMember("killer_entityId")->value.GetInt();
		m_pixel_gun_revive_data.killer_itemId = doc->FindMember("killer_itemId")->value.GetInt();
		m_pixel_gun_revive_data.cur_hp = doc->FindMember("cur_hp")->value.GetInt();
		m_pixel_gun_revive_data.max_hp = doc->FindMember("max_hp")->value.GetInt();
		m_pixel_gun_revive_data.cur_defence = doc->FindMember("cur_defence")->value.GetInt();
		m_pixel_gun_revive_data.max_defence = doc->FindMember("max_defence")->value.GetInt();

		rapidjson::Value arr_arms = doc->FindMember("arms")->value.GetArray();
		for (size_t i = 0; i < arr_arms.Size() && i < MAX_ARMS_NUM; ++i)
		{
			if (!arr_arms[i].HasMember("img") || !arr_arms[i].HasMember("name"))
			{
				LordLogError("gui_pixelgunrevive::getAllInfo The game result content effect_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			String img = arr_arms[i].FindMember("img")->value.GetString();
			String name = arr_arms[i].FindMember("name")->value.GetString();

			m_pixel_gun_revive_data.killer_arms_img[i] = img;
			m_pixel_gun_revive_data.killer_arms_name[i] = name;
		}

		rapidjson::Value arr_gadgets = doc->FindMember("gadgets")->value.GetArray();
		for (size_t i = 0; i < arr_gadgets.Size() && i < MAX_GAD_GETS_NUM; ++i)
		{
			if (!arr_gadgets[i].HasMember("img") || !arr_gadgets[i].HasMember("name"))
			{
				LordLogError("gui_pixelgunrevive::getAllInfo The game result content effect_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			String img = arr_gadgets[i].FindMember("img")->value.GetString();
			String name = arr_gadgets[i].FindMember("name")->value.GetString();

			m_pixel_gun_revive_data.killer_gadgets_img[i] = img;
			m_pixel_gun_revive_data.killer_gadgets_name[i] = name;
		}

		return true;
	}
}
