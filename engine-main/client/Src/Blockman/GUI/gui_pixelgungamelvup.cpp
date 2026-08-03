#include "gui_pixelgungamelvup.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	gui_pixelgungamelvup::gui_pixelgungamelvup() :
		gui_layout("PixelGunGameLvUp.json")
	{
	}

	gui_pixelgungamelvup::~gui_pixelgungamelvup()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelgungamelvup::onLoad()
	{
		GUILayout* PixelGunGameLvUp = getWindow<GUILayout>("PixelGunGameLvUp");
		PixelGunGameLvUp->subscribeEvent(EventWindowTouchUp, std::bind(&gui_pixelgungamelvup::onContentClick, this, std::placeholders::_1));

		m_title = getWindow<GUIStaticText>("PixelGunGameLvUp-Title");
		getWindow<GUIButton>("PixelGunGameLvUp-Sure")->SetText(getString(LanguageKey::GUI_PIXEL_GUN_LVUP_SURE_BTN).c_str());
		getWindow<GUIButton>("PixelGunGameLvUp-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgungamelvup::onSureClick, this, std::placeholders::_1));

		for (int i = 0; i < MAX_REWARD_NUM; ++i)
		{
			String ImgName = StringUtil::Format("PixelGunGameLvUp-Image%d", i + 1);
			m_image[i] = getWindow<GUIStaticImage>(ImgName.c_str());

			String NumName = StringUtil::Format("PixelGunGameLvUp-Name%d", i + 1);
			m_num[i] = getWindow<GUIStaticText>(NumName.c_str());
		}

		m_subscriptionGuard.add(ShowPixelGunLvupEvent::subscribe(std::bind(&gui_pixelgungamelvup::onShowPixelGunGameLvUp, this, std::placeholders::_1)));
	}

	void gui_pixelgungamelvup::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_pixelgungamelvup::onShowPixelGunGameLvUp(const String& data)
	{
		if (getAllInfo(data))
		{
			for (int i = 0; i < MAX_REWARD_NUM; ++i)
			{
				if (m_lvup_reward[i].img.length() > 0)
				{
					m_image[i]->SetVisible(true);
					m_image[i]->SetImage(m_lvup_reward[i].img.c_str());

					String rewardNumTxtName = StringUtil::Format("+ %d", m_lvup_reward[i].num);
					m_num[i]->SetText(rewardNumTxtName.c_str());
				}
				else
				{
					m_image[i]->SetVisible(false);
				}
			}

			if (m_level > 0)
			{
				String titleName = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUN_LVUP_TITLE).c_str(), m_level);
				m_title->SetText(titleName.c_str());
				m_title->SetVisible(true);
			}
			else
			{
				m_title->SetVisible(false);
			}
		}

		return true;
	}

	bool gui_pixelgungamelvup::getAllInfo(const String & data)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_pixelgungamelvup::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("rewards")
			|| !doc->HasMember("level"))
		{
			LordLogError("gui_pixelgungamelvup::getAllInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("rewards")->value.GetArray();
		for (size_t i = 0; i < arr.Size() && i < MAX_REWARD_NUM; ++i)
		{
			if (!arr[i].HasMember("img") || !arr[i].HasMember("num"))
			{
				LordLogError("gui_pixelgungameresult::getAllInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			m_lvup_reward[i].img = arr[i].FindMember("img")->value.GetString();
			m_lvup_reward[i].num = arr[i].FindMember("num")->value.GetInt();
		}

		m_level = doc->FindMember("level")->value.GetInt();

		return true;
	}

	bool gui_pixelgungamelvup::onContentClick(const EventArgs & events)
	{
		return true;
	}

	bool gui_pixelgungamelvup::onSureClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->showPixelGunGameLvUp(false);
		return true;
	}

}
