#include "gui_pixelGunHallArmorUpgrade.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "cItem/cItem.h"
#include "UI/GUISystem.h"
#include "External/cjson.h"
#include "Render/TextureAtlas.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_pixelGunHallArmorUpgrade::gui_pixelGunHallArmorUpgrade()
		: gui_layout("PixelGunHallArmorUpgrade.json")
	{
	}

	gui_pixelGunHallArmorUpgrade::~gui_pixelGunHallArmorUpgrade()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelGunHallArmorUpgrade::onLoad()
	{
		m_time_text = getWindow<GUIStaticText>("PixelGunHallArmorUpgrade-Time");
		m_up_value = getWindow<GUIStaticText>("PixelGunHallArmorUpgrade-UpValue");
		m_price_num = getWindow<GUIStaticText>("PixelGunHallArmorUpgrade-PriceNum");
		m_closeBtn = getWindow<GUIButton>("PixelGunHallArmorUpgrade-CloseBtn");
		m_armorImg = getWindow<GUIStaticImage>("PixelGunHallArmorUpgrade-ArmorImg");
		m_priceImg = getWindow<GUIStaticImage>("PixelGunHallArmorUpgrade-PriceImg");
		m_text_layout = getWindow<GUILayout>("PixelGunHallArmorUpgrade-Upgrade-Action");
		m_time_layout = getWindow<GUILayout>("PixelGunHallArmorUpgrade-Countdown");
		m_time_layout->SetVisible(false);
		
		m_armor_updage_btn = getWindow<GUIButton>("PixelGunHallArmorUpgrade-UpBtn");
		m_armor_updage_btn->SetText(getString(LanguageKey::GUI_UPGRADE));

		getWindow("PixelGunHallArmorUpgrade-ArmorDescribe")->SetText(getString(LanguageKey::GUI_ARMOR_DESCRIBE));
		getWindow("PixelGunHallArmorUpgrade-Name")->SetText(getString(LanguageKey::GUI_HALL_ARMOR_TITLE));
		getWindow("PixelGunHallArmorUpgrade-BaseName")->SetText(getString(LanguageKey::GUI_ARMOR_BAR_TEXT));

		m_armorUp_bar = getWindow<GUIProgressBar>("PixelGunHallArmorUpgrade-ArmorUpValueBar");
		m_armorUp_bar->SetProgress(0);


		m_armor_level = getWindow<GUIStaticText>("PixelGunHallArmorUpgrade-Level");
		m_upgrade_armor_value = getWindow<GUIStaticText>("PixelGunHallArmorUpgrade-UpBarValue");
		m_max_armor_value = getWindow<GUIButton>("PixelGunHallArmorUpgrade-MaxValue");

		m_closeBtn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunHallArmorUpgrade::onCloseClick, this, std::placeholders::_1));
		m_armor_updage_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunHallArmorUpgrade::onBtnArmorUp, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowPixelGunHallArmorUpgradeEvent::subscribe(std::bind(&gui_pixelGunHallArmorUpgrade::refreshAll, this, std::placeholders::_1)));
	}

	void gui_pixelGunHallArmorUpgrade::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;

		if (m_armorTime > 0)
		{
			m_armorTime -= nTimeElapse;
			int seconds = m_armorTime / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			m_time_text->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
		}
	}

	void gui_pixelGunHallArmorUpgrade::onShow()
	{

	}

	bool gui_pixelGunHallArmorUpgrade::refreshAll(const String & data)
	{

		String armorImg = "";
		String priceImg = "";
		int price = 0;
		int priceId = 0;
		int time = 0;
		int level = 0;
		int armorUp = 0;
		int baseArmor = 10;
		int maxArmor = 50;
		int cruArmor = 10;
		int maxHealth = 20;
		int maxLevel = 44;

		if (getAllInfo(data, armorImg, priceImg, price, priceId, time, level, armorUp, baseArmor, maxArmor, cruArmor, maxHealth, maxLevel))
		{
			m_need_money = price;
			m_printId = priceId;
			m_armorTime = time * 1000;
			m_up_value->SetText(StringUtil::Format("+%d", armorUp).c_str());
			m_price_num->SetText(StringUtil::ToString(price).c_str());		

			m_priceImg->SetImage(priceImg.c_str());
			m_armorImg->SetImage(armorImg.c_str());
	
			m_armor_level->SetText(StringUtil::Format(getString(LanguageKey::GUI_ARMOR_TEXT).c_str(), level).c_str());
			m_max_armor_value->SetText(StringUtil::ToString(maxArmor).c_str());
			m_upgrade_armor_value->SetText(StringUtil::ToString(cruArmor).c_str());

			if ((maxArmor - baseArmor) > 0)
			{
				m_armorUp_bar->SetProgress( (cruArmor - baseArmor) * 1.0f / (maxArmor - baseArmor) );
				m_armorUp_bar->SetVisible(true);
			}
			else
			{
				m_armorUp_bar->SetVisible(false);
			}

			if (level == maxLevel)
			{
				m_armor_updage_btn->SetText(getString(LanguageKey::GUI_ARMOR_MAX));
				m_armor_updage_btn->SetEnabled(false);
				m_text_layout->SetVisible(false);
			}
			else
			{
				if (maxHealth < cruArmor + armorUp)
					m_armor_updage_btn->SetEnabled(false);
				else
					m_armor_updage_btn->SetEnabled(true);

				if (m_armorTime <= 0)
				{
					m_armor_updage_btn->SetText(getString(LanguageKey::GUI_UPGRADE));
					m_time_layout->SetVisible(false);
					m_armor_request = ArmorOperateRequest::ARMOR_UP;
				}
				else
				{
					m_time_layout->SetVisible(true);
					m_armor_request = ArmorOperateRequest::ARMOR_SPEEDUP;
					m_armor_updage_btn->SetText(getString(LanguageKey::GUI_SPEEDUP));
				}
			}
		}

		return true;
	}

	bool gui_pixelGunHallArmorUpgrade::getAllInfo(const String & data, String& armorImg, String& priceImg, int& price, int& priceId, int& time, int& level, int& armorUp,
													int& baseArmor, int& maxArmor, int& curArmor, int& maxHealth, int& maxLevel)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelGunHallArmorUpgrade::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("level") || !doc->HasMember("price") || !doc->HasMember("priceImg") || !doc->HasMember("armorImg") || !doc->HasMember("armorUp") || !doc->HasMember("time")
			|| !doc->HasMember("priceId") || !doc->HasMember("baseArmor") || !doc->HasMember("maxArmor") || !doc->HasMember("curArmor") || !doc->HasMember("maxHealth") || !doc->HasMember("maxLevel"))
		{

			LordLogError("gui_pixelGunHallArmorUpgrade::getAllInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		armorImg = doc->FindMember("armorImg")->value.GetString();
		priceImg = doc->FindMember("priceImg")->value.GetString();
		price = doc->FindMember("price")->value.GetInt();
		priceId = doc->FindMember("priceId")->value.GetInt();
		time = doc->FindMember("time")->value.GetInt();
		level = doc->FindMember("level")->value.GetInt();
		armorUp = doc->FindMember("armorUp")->value.GetInt();
		baseArmor = doc->FindMember("baseArmor")->value.GetInt();
		maxArmor = doc->FindMember("maxArmor")->value.GetInt();
		curArmor = doc->FindMember("curArmor")->value.GetInt();
		maxHealth = doc->FindMember("maxHealth")->value.GetInt();
		maxLevel = doc->FindMember("maxLevel")->value.GetInt();

		LordDelete(doc);
		return true;
	}

	bool gui_pixelGunHallArmorUpgrade::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}


	bool gui_pixelGunHallArmorUpgrade::onBtnArmorUp(const EventArgs)
	{
		playSoundByType(ST_Click);
		Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		if (wallet)
		{
			i64 money = wallet->getMoneyByCoinId(m_printId);
			if (money < m_need_money)
				ShowLackOfMoneyTipDialogEvent::emit(m_need_money, (CurrencyType)m_printId);
			else
			{
				ClientNetwork::Instance()->getSender()->sendUpgradeArmor(isShown(), m_armor_request);
			}
		}
		return false;
	}
}