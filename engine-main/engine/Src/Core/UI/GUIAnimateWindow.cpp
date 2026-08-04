#include "GUIAnimateWindow.h"
#include "GUIAnimatMgr.h"
#include "GUIAnimate.h"

namespace LORD
{
	GUIAnimateWindow::GUIAnimateWindow( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)	
		, m_nAnimateID(-1)
		, m_hasRenderTime(0)
		, m_index(0)
		, m_play(true)
	{
	}

	GUIAnimateWindow::~GUIAnimateWindow()
	{
		GUIAnimate* pAnimate = GUIAnimatMgr::Instance()->GetAnimatById(m_nAnimateID);
		if (pAnimate)
		{
			pAnimate->clearImage();
		}
	}

	bool GUIAnimateWindow::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ANIMATE_ID)
		{
			SetAnimateID(StringUtil::ParseI32(strValue.c_str()));			
			return true;
		}
		else if (strName == GWP_ANIMATE_PLAY)
		{
			m_play = StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		return false;
	}

	bool GUIAnimateWindow::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ANIMATE_ID)
		{
			strValue = StringUtil::ToString(GetAnimateID()).c_str();
			return true;
		}
		else if (strName == GWP_ANIMATE_PLAY)
		{
			strValue = StringUtil::ToString(m_play).c_str();
			return true;
		}

		return false;
	}

	void GUIAnimateWindow::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_ANIMATE_ID);
		properlist.push_back(GWP_ANIMATE_PLAY);
	}

	bool GUIAnimateWindow::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_ANIMATE_ID)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_ANIMATE_PLAY)
		{
			type = GPT_BOOL;
			return true;
		}

		return false;
	}

	void GUIAnimateWindow::DrawSelf()
	{
		GUIAnimate* pAnimate = GUIAnimatMgr::Instance()->GetAnimatById(m_nAnimateID);
		if (pAnimate)
		{
			pAnimate->doPlayEx(GetRenderArea(), this, m_renderImage, m_hasRenderTime, m_index, m_play);
		}
	}

	void GUIAnimateWindow::SetAnimateID( i32 nID )
	{
		if (nID != m_nAnimateID)
		{
			m_renderImage.SetWholeImageName("");
			m_nAnimateID = nID;
		}
	}

	void GUIAnimateWindow::play()
	{
		m_play = true;
		m_hasRenderTime = 0;
		m_index = 0;
	}

}
