#include "gui_occupationUnlock.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Util/UICommon.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Inventory/Wallet.h"
#include "Network/ClientNetwork.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_occupationUnlock::gui_occupationUnlock() :
		gui_layout("OccupationUnlock.json")
	{

	}


	gui_occupationUnlock::~gui_occupationUnlock()
	{
	}

	void gui_occupationUnlock::onHide()
	{
		m_occupationId = 0;
		m_singlePrice = 0;
		m_perpetualPrice = 0;
		m_singleCurrencyType = 1;
		m_perpetualCurrencyType = 1;
	}

	void gui_occupationUnlock::onLoad()
	{
		m_titleText = getWindow<GUIStaticText>("OccupationUnlock-Title-Name");
		m_contentWindow = getWindow("OccupationUnlock-Content");
		m_btnSingle = getWindow<GUIButton>("OccupationUnlock-Btn-Single");
		m_btnPerpetual = getWindow<GUIButton>("OccupationUnlock-Btn-Perpetual");

		m_actorWindow = getWindow<GuiActorWindow>("OccupationUnlock-Content-Actor");

		m_tvSinglePrice = getWindow<GUIStaticText>("OccupationUnlock-SingleCurrency-Value");
		m_tvPerpetualPrice = getWindow<GUIStaticText>("OccupationUnlock-PerpetualCurrency-Value");

		m_ivSingleCurrency = getWindow<GUIStaticImage>("OccupationUnlock-SingleCurrency-Iocn");
		m_ivPerpetualCurrency = getWindow<GUIStaticImage>("OccupationUnlock-PerpetualCurrency-Icon");
		m_lvOrientation = getWindow<GUIListBox>("OccupationUnlock-Content-HeroInfo-OrientationInfo");

		setText(m_titleText, "gui_occupation_unlock_title");
		setText(m_btnSingle, "gui_occupation_unlock_btn_single");
		setText(m_btnPerpetual, "gui_occupation_unlock_btn_perpetual");
		setText("OccupationUnlock-Content-HeroInfo-Orientation", "gui_occupation_unlock_orientation");
		setText("OccupationUnlock-Content-HeroRadar-Info-Standard", "gui_occupation_unlock_standard_value");

		m_btnSingle->subscribeEvent(EventButtonClick, std::bind(&gui_occupationUnlock::onClick, this, std::placeholders::_1, VIewId::BTN_SINGLE));
		m_btnPerpetual->subscribeEvent(EventButtonClick, std::bind(&gui_occupationUnlock::onClick, this, std::placeholders::_1, VIewId::BTN_PERPETUAL));
		getWindow("OccupationUnlock-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_occupationUnlock::onClick, this, std::placeholders::_1, VIewId::BTN_CLOSE));
		m_subscriptionGuard.add(ShowOccupationUnlockTip::subscribe(std::bind(&gui_occupationUnlock::open, this, std::placeholders::_1)));
	}

	void gui_occupationUnlock::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_occupationUnlock::onClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::BTN_PERPETUAL:
			if (UICommon::checkMoney(m_perpetualPrice, (CurrencyType)m_perpetualCurrencyType))
			{
				RootGuiLayout::Instance()->setOccupationUnlockVisible(false);
				String extra = "Perpetual=" + StringUtil::ToString(m_occupationEntityId);
				ClientNetwork::Instance()->getSender()->sendCustomTipResult(extra, true);
			}
			break;
		case VIewId::BTN_SINGLE:
			if (UICommon::checkMoney(m_singlePrice, (CurrencyType)m_singleCurrencyType))
			{
				RootGuiLayout::Instance()->setOccupationUnlockVisible(false);
				String extra = "Single=" + StringUtil::ToString(m_occupationEntityId);
				ClientNetwork::Instance()->getSender()->sendCustomTipResult(extra, true);
			}
			break; 
		case VIewId::BTN_CLOSE:
			RootGuiLayout::Instance()->setOccupationUnlockVisible(false);
			break;
		}
		
		return true;
	}

	bool gui_occupationUnlock::open(const String & info)
	{
		dataParse(info);
		RootGuiLayout::Instance()->setOccupationUnlockVisible(true);
		return false;
	}

	void gui_occupationUnlock::dataParse(const String & info)
	{
		Document doc = Document();
		doc.Parse(info.c_str());
		if (doc.HasParseError())
		{
			return;
		}

		if (doc.HasMember("id"))
		{
			m_occupationId = doc.FindMember("id")->value.GetInt();
		}

		if (doc.HasMember("entityId"))
		{
			m_occupationEntityId = doc.FindMember("entityId")->value.GetInt();
		}

		if (doc.HasMember("perpetualCurrencyType"))
		{
			m_perpetualCurrencyType = doc.FindMember("perpetualCurrencyType")->value.GetInt();
		}

		if (doc.HasMember("singleCurrencyType"))
		{
			m_singleCurrencyType = doc.FindMember("singleCurrencyType")->value.GetInt();
		}

		if (doc.HasMember("singlePrice"))
		{
			m_singlePrice = doc.FindMember("singlePrice")->value.GetInt();
		}

		if (doc.HasMember("perpetualPrice"))
		{
			m_perpetualPrice = doc.FindMember("perpetualPrice")->value.GetInt();
		}

		if (doc.HasMember("actorName"))
		{
			String actorName = doc.FindMember("actorName")->value.GetString();
			m_actorWindow->SetActor(actorName.c_str());
		}

		if (doc.HasMember("name"))
		{
			String name = doc.FindMember("name")->value.GetString();
			setText("OccupationUnlock-Content-HeroName", name);
			setText("OccupationUnlock-Content-HeroRadar-Info-Current", name);
		}

		if (doc.HasMember("radarIcon"))
		{
			String icon = doc.FindMember("radarIcon")->value.GetString();
			getWindow<GUIStaticImage>("OccupationUnlock-Content-HeroRadar-RadarInfo")->SetImage(icon.c_str());
		}

		vector<String> ::type orientations = vector<String> ::type();
		if (doc.HasMember("orientations"))
		{
			auto content = doc.FindMember("orientations")->value.GetArray();
			for (const auto& item : content)
			{
				String str =  item.GetString();
				orientations.push_back(str);
			}
		}

		static int count = 0;
		m_lvOrientation->ClearAllItem();
		for (String str : orientations)
		{
			count++;
			String itemLayout = StringUtil::Format("OccupationUnlock-Content-HeroInfo-OrientationInfo-Item-%d", count).c_str();
			GUIStaticText * orientationView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, itemLayout.c_str());
			orientationView->SetWidth(UDim(1.f, 0));
			orientationView->SetHeight(UDim(0, 64.f));
			orientationView->SetTextHorzAlign(HA_CENTRE);
			orientationView->SetTextVertAlign(VA_CENTRE);
			orientationView->SetTouchable(false);
			orientationView->SetTextColor(Color(1.f, 0.91f , 0.051f));
			orientationView->SetText(getString(str));
			orientationView->SetWordWrap(true);
			m_lvOrientation->AddItem(orientationView, false);
		}

		m_tvSinglePrice->SetText(StringUtil::ToString(m_singlePrice).c_str());
		m_tvPerpetualPrice->SetText(StringUtil::ToString(m_perpetualPrice).c_str());
		m_ivSingleCurrency->SetImage(UICommon::getCurrencyIconByType((CurrencyType)m_singleCurrencyType).c_str());
		m_ivPerpetualCurrency->SetImage(UICommon::getCurrencyIconByType((CurrencyType)m_perpetualCurrencyType).c_str());
	}
	
}
