#include "GUIProgressBar.h"

namespace LORD
{
	GUIProgressBar::GUIProgressBar( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_fProgress(1.f)
		, m_isVertical(false)
	{
	}

	GUIProgressBar::~GUIProgressBar()
	{
	}


	bool GUIProgressBar::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_PRO_BACK_IMAGE)
		{
			SetBackImage(strValue);
			return true;
		}
		else if (strName == GWP_PRO_IMAGE)
		{
			SetProgressImage(strValue);
			return true;
		}
		else if (strName == GWP_PRO_HEADER_IMAGE)
		{
			SetHeaderImage(strValue);
			return true;
		}
		else if (strName == GWP_PRO_PROGRESS)
		{
			SetProgress(StringUtil::ParseFloat(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_PRO_IS_VERTICAL)
		{
			SetVertical(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}

		return false;
	}

	bool GUIProgressBar::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_PRO_BACK_IMAGE)
		{
			strValue = m_BackImage.GetWholeImageName();
			return true;
		}
		else if (strName == GWP_PRO_IMAGE)
		{
			strValue = m_ProgressImage.GetWholeImageName();
			return true;
		}
		else if (strName == GWP_PRO_HEADER_IMAGE)
		{
			strValue = m_HeaderImage.GetWholeImageName();
			return true;
		}
		else if (strName == GWP_PRO_PROGRESS)
		{
			strValue = StringUtil::ToString(m_fProgress).c_str();
			return true;
		}
		else if (strName == GWP_PRO_IS_VERTICAL)
		{
			strValue = StringUtil::ToString(m_isVertical).c_str();
			return true;
		}

		return false;
	}

	void GUIProgressBar::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_PRO_PROGRESS);
		properlist.push_back(GWP_PRO_BACK_IMAGE);
		properlist.push_back(GWP_PRO_IMAGE);
		properlist.push_back(GWP_PRO_HEADER_IMAGE);
		properlist.push_back(GWP_PRO_IS_VERTICAL);
	}

	bool GUIProgressBar::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_PRO_PROGRESS)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_PRO_BACK_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_PRO_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_PRO_HEADER_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_PRO_IS_VERTICAL)
		{
			type = GPT_BOOL;
			return true;
		}
		return false;
	}

	void GUIProgressBar::DrawSelf()
	{
		ERect rtUnclippedRect = GetRenderArea();
		m_BackImage.DrawStretchImage(this, rtUnclippedRect, getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
		

		if (m_isVertical)
		{
			float fXProgressPosition = rtUnclippedRect.left + rtUnclippedRect.getWidth() * 0.5f;
			float fYProgressPosition = rtUnclippedRect.top + m_fProgress *  rtUnclippedRect.getHeight();
			ERect rtClip = rtUnclippedRect;
			rtClip.bottom = fYProgressPosition;
			m_ProgressImage.DrawStretchImage(this, rtUnclippedRect, getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &rtClip);

			ERect rtHeader;
			Vector2 v2HalfSize = m_HeaderImage.GetImageSize() * 0.5f;
			rtHeader.left = fXProgressPosition - v2HalfSize.x;
			rtHeader.right = fXProgressPosition + v2HalfSize.x;
			rtHeader.top = fYProgressPosition - v2HalfSize.y;
			rtHeader.bottom = fYProgressPosition + v2HalfSize.y;

			m_HeaderImage.DrawImage(this, rtHeader, getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
		}
		else
		{
			float fXProgressPosition = rtUnclippedRect.left + m_fProgress * rtUnclippedRect.getWidth();
			float fYProgressPosition = rtUnclippedRect.top + rtUnclippedRect.getHeight() * 0.5f;
			//
			ERect rtClip = rtUnclippedRect;
			rtClip.right = fXProgressPosition;
			m_ProgressImage.DrawStretchImage(this, rtUnclippedRect, getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &rtClip);

			ERect rtHeader;
			Vector2 v2HalfSize = m_HeaderImage.GetImageSize() * 0.5f;
			rtHeader.left = fXProgressPosition - v2HalfSize.x;
			rtHeader.right = fXProgressPosition + v2HalfSize.x;
			rtHeader.top = fYProgressPosition - v2HalfSize.y;
			rtHeader.bottom = fYProgressPosition + v2HalfSize.y;

			m_HeaderImage.DrawImage(this, rtHeader, getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
		}
	}

	void GUIProgressBar::SetBackImage( const GUIString& strBackImage )
	{
		m_BackImage.SetWholeImageName(strBackImage);
	}

	void GUIProgressBar::SetProgressImage( const GUIString& strProgressImage )
	{
		m_ProgressImage.SetWholeImageName(strProgressImage);
	}

	void GUIProgressBar::SetHeaderImage( const GUIString& strHeaderImage )
	{
		m_HeaderImage.SetWholeImageName(strHeaderImage);
	}

	void GUIProgressBar::SetProgress( float fProgress )
	{
		if(fProgress  < 0.0f)
		{
			return;
		}

		if (fProgress != m_fProgress)
		{
			m_fProgress  = fProgress > 1.0f ? 1.0f : fProgress;
			
			Invalidate();
		}
	}

}
