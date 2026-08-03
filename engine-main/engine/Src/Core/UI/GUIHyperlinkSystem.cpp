#include "GUIHyperlinkSystem.h"
#include "GUISystem.h"

namespace LORD
{
	//---------------------------------------------------------------------
	GUIHyperlinkManager* GUIHyperlinkManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIHyperlinkManager::GUIHyperlinkManager()
		: m_bIsClickedLink(false)
	{

	}

	GUIHyperlinkManager::~GUIHyperlinkManager()
	{

	}

	void GUIHyperlinkManager::AddHyperLink( const ERect& rtArea, const GUIString& strHyperLinkInfo, GUIWindow* pWindow )
	{
		GUIHyperlinkInfo stInfo;
		stInfo.rtHyperlinkArea = rtArea;
		stInfo.strHyperlinkInfo = strHyperLinkInfo;
		stInfo.pWindow = pWindow;
		m_HyperlinkInfoList.push_back(stInfo);
	}

	void GUIHyperlinkManager::ClearAllHyperLink()
	{
		m_HyperlinkInfoList.clear();
	}

	bool GUIHyperlinkManager::OnClickHyperLink( const Vector2& point, GUIWindow* pTargetWindow )
	{
		if (pTargetWindow == NULL)
		{
			return false;
		}

		for (size_t i = 0; i < m_HyperlinkInfoList.size(); ++i)
		{
			if (m_HyperlinkInfoList[i].rtHyperlinkArea.isPointInRect(point)
				&& m_HyperlinkInfoList[i].pWindow == pTargetWindow )
			{
				// fire event
				GUISystem::Instance()->GetHyperlinkCallback()(m_HyperlinkInfoList[i].strHyperlinkInfo);

				//
				SetClickLink(true);
				m_ClickArea = m_HyperlinkInfoList[i].rtHyperlinkArea;
				return true;
			}
		}

		return false;
	}

}
