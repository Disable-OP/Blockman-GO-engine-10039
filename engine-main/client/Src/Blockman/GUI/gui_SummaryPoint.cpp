#include "gui_summaryPoint.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Setting/ScorePointSetting.h"

using namespace LORD;
namespace BLOCKMAN
{

	gui_summaryPoint::gui_summaryPoint()
		: gui_layout("SummaryPoint.json")
	{
	}

	gui_summaryPoint::~gui_summaryPoint()
	{
	}

	void gui_summaryPoint::onLoad()
	{
		m_pointList = getWindow<GUIListBox>("SummaryPoint-Content-List");
	}

	void gui_summaryPoint::onShow()
	{
	
	}

	void gui_summaryPoint::onUpdate(ui32 nTimeElapse)
	{
	
	}

	void gui_summaryPoint::showContent(const LORD::UVector2& pos, const std::map<int, int>& pointMap)
	{
		if (!m_pointList)
			return;

		String pointName;
		int    point;
		m_rootWindow->SetPosition(pos);
		m_pointList->ClearAllItem();
		m_pointList->SetItemHeight(40.0f);
		for (int i = 1; i <= (int)ScorePointSetting::getScorePointCount(); i++)
		{
			auto iter = pointMap.find(i);
			if (iter == pointMap.end())
				continue;

			pointName = ScorePointSetting::getScorePointName(i);
			point = iter->second;

			String strTextName = StringUtil::Format("SummaryPoint-Content-List-item-%d", i).c_str();
			String strTextPointName = StringUtil::Format("SummaryPoint-Content-List-item-%d-Name", i).c_str();
			String strTextPointNum = StringUtil::Format("SummaryPoint-Content-List-item-%d-Num", i).c_str();
			GUIWindow* pLayout = GUIWindowManager::Instance()->CreateGUIWindow(GWT_DEFAULT_WINDOW, strTextName.c_str());
			pLayout->SetWidth(UDim(1, 0));
			pLayout->SetHeight(UDim(0, 30));
			pLayout->SetPosition(UVector2(UDim(0,0),UDim(0,10)));
			pLayout->SetTouchable(false);

			GUIStaticText* pPointName = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextPointName.c_str());
			pPointName->SetText(pointName.c_str());
			pPointName->SetWidth(UDim(1, 0));
			pPointName->SetHeight(UDim(1, 0));
			pPointName->SetTouchable(false);
			pPointName->SetTextVertAlign(VA_CENTRE);
			pPointName->SetTextHorzAlign(HA_LEFT);
			pPointName->SetPosition(UVector2(UDim(0, 10), UDim(0, 0)));
			pPointName->SetTextScale(1.2f);
			pLayout->AddChildWindow(pPointName);

			GUIStaticText* pPointNum = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextPointNum.c_str());
			pPointNum->SetText(StringUtil::ToString(point).c_str());
			pPointNum->SetWidth(UDim(1, 0));
			pPointNum->SetHeight(UDim(1, 0));
			pPointNum->SetTouchable(false);
			pPointNum->SetTextVertAlign(VA_CENTRE);
			pPointNum->SetTextHorzAlign(HA_RIGHT);
			pPointNum->SetPosition(UVector2(UDim(0, -10), UDim(0, 0)));
			pPointNum->SetTextScale(1.2f);
			pLayout->AddChildWindow(pPointNum);
			m_pointList->AddItem(pLayout, true);
		}
	}
}
