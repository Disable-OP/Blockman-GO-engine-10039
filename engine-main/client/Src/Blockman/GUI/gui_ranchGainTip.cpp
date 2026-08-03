#include "gui_ranchGainTip.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiRanchItem.h"
#include "GUI/GuiDef.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchGainTip::gui_ranchGainTip()
		: gui_layout("RanchGainTip.json")
	{
	}

	gui_ranchGainTip::~gui_ranchGainTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchGainTip::onLoad()
	{
		setText("RanchGainTip-Title", "gui_ranch_gain_title");
		m_listView = getWindow<GUIListHorizontalBox>("RanchGainTip-Items");
		m_subscriptionGuard.add(ShowRanchGainTipEvent::subscribe(std::bind(&gui_ranchGainTip::showTip, this, std::placeholders::_1)));

	}

	void gui_ranchGainTip::onUpdate(ui32 nTimeElapse)
	{
		if (m_leftTime > 0 && isShown())
		{
			m_leftTime = m_leftTime - nTimeElapse > 0 ? m_leftTime - nTimeElapse : 0.f;
			float h = (m_leftTime / m_showTime) - 1.f;
			m_rootWindow->SetArea(UDim(0, 0), UDim(h, 0 ), UDim(0.7703f, 0), UDim(0.3041f, 0));
			if (m_leftTime <= 0)
			{
				RootGuiLayout::Instance()->setRanchGainTipVisible(false);
			}
		}
	}

	bool gui_ranchGainTip::showTip(const vector<RanchCommon>::type & items)
	{
		initItem(items);
		m_leftTime = 2000.f;
		m_rootWindow->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.7703f, 0), UDim(0.3041f, 0));
		RootGuiLayout::Instance()->setRanchGainTipVisible(true);
		return true;
	}

	void gui_ranchGainTip::initItem(const vector<RanchCommon>::type & items)
	{
		m_listView->ClearAllItem();
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchGainTip-Items-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			auto ranchItem = RanchSetting::getRanchItem(item.itemId);
			GUILayout* ranchItemLayout= (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, itemLayout.c_str());
			ranchItemLayout->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 152.f), UDim(0.0f, 108.f));
			ranchItemLayout->SetTouchable(false);

			String strIcon = StringUtil::Format("%s-Icon", itemLayout.c_str());
			GUIStaticImage* ranchItemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strIcon.c_str());
			ranchItemIcon->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 102.f), UDim(0.0f, 108.f));
			ranchItemIcon->SetTouchable(false);
			ranchItemIcon->SetImage(ranchItem ? ranchItem->icon.c_str() : "");
			ranchItemLayout->AddChildWindow(ranchItemIcon);

			String strNum = StringUtil::Format("%s-Num", itemLayout.c_str());
			GUIStaticText* ranchItemNum = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strNum.c_str());
			ranchItemNum->SetArea(UDim(0, 102.f), UDim(0, 0.0f), UDim(0.0f, 50.f), UDim(0.0f, 108.f));
			ranchItemNum->SetText(StringUtil::Format("x %d", item.num).c_str());
			ranchItemNum->SetTouchable(false);
			ranchItemNum->SetTextHorzAlign(HA_CENTRE);
			ranchItemNum->SetTextVertAlign(VA_CENTRE);
			ranchItemNum->SetProperty(GWP_TEXT_SHADOW, "true");
			ranchItemLayout->AddChildWindow(ranchItemNum);

			m_listView->AddItem(ranchItemLayout, false);
		}
		m_listView->setContainerAlignment();
	}

}
