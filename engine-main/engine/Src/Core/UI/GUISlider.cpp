#include "GUISlider.h"
#include "GUISystem.h"

namespace LORD
{
	GUISlider::GUISlider( GUIWindowType nType, const GUIString& strName )
		: GUIProgressBar(nType, strName)
		, m_bIsDragging(false)
		, m_DragPosition(Vector2::ZERO)
		, m_bQuickSlide(false)
	{
	}

	GUISlider::~GUISlider()
	{
	}

	bool GUISlider::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIProgressBar::SetProperty(strName, strValue))
		{
			return true;
		}		
		if (strName == GWP_QUICK_SLIDE)
		{
			m_bQuickSlide = StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		return false;
	}

	bool GUISlider::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIProgressBar::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_QUICK_SLIDE)
		{
			strValue = StringUtil::ToString(m_bQuickSlide).c_str();
			return true;
		}
		
		return false;
	}

	void GUISlider::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIProgressBar::GetPropertyList(properlist);
		properlist.push_back(GWP_QUICK_SLIDE);		
	}

	bool GUISlider::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIProgressBar::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_QUICK_SLIDE)
		{
			type = GPT_BOOL;
			return true;
		}
		
		return false;
	}

	bool GUISlider::TouchDown( const Vector2& position )
	{
// 		if (IsInDragArea(position))
// 		{
// 			m_bIsDragging = true;
// 			m_DragPosition = position;
// 		}
		const ERect & rt = GetUnclippedOuterRect();
		
		if (m_bQuickSlide && rt.isPointInRect(position))
		{
			if (m_isVertical)
			{
				float fDelta = position.y - rt.top;
				fDelta = fDelta / rt.getHeight();
				SetProgress(fDelta);
				m_DragPosition = position;
				m_bIsDragging = true;
			}
			else
			{
				float fDelta = position.x - rt.left;
				fDelta = fDelta / rt.getWidth();
				SetProgress(fDelta);
				m_DragPosition = position;
				m_bIsDragging = true;
			}
			
			//GUISystem::Instance()->GetEventCallback()(GET_SLIDER_MOVE, GetOwnerDialogName(), GetName(), m_fProgress * 100, 0);
		}
		else
		{
			if (IsInDragArea(position))
			{
				m_bIsDragging = true;
				m_DragPosition = position;
			}
		}

		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchDown, args);

		return true;
	}

	bool GUISlider::TouchMove( const Vector2& position )
	{
		if (m_bIsDragging)
		{

			if (m_isVertical)
			{
				float fDelta = position.y - m_DragPosition.y;
				fDelta = fDelta / GetUnclippedOuterRect().getHeight();
				SetProgress(GetProgress() + fDelta);
				m_DragPosition = position;
			}
			else
			{
				float fDelta = position.x - m_DragPosition.x;
				fDelta = fDelta / GetUnclippedOuterRect().getWidth();
				SetProgress(GetProgress() + fDelta);
				m_DragPosition = position;
			}
			//GUISystem::Instance()->GetEventCallback()(GET_SLIDER_MOVE, GetOwnerDialogName(), GetName(), m_fProgress * 100, 0);
		}
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchMove, args);
		return true;
	}

	bool GUISlider::TouchUp( const Vector2& position )
	{
		m_bIsDragging = false;
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchUp, args);
		return true;
	}

	bool GUISlider::MotionRelease()
	{
		m_bIsDragging = false;
		WindowEventArgs args(this);
		fireEvent(EventMotionRelease, args);
		return true;
	}

	bool GUISlider::IsInDragArea(const Vector2& position)
	{
		ERect rtUnclippedRect = GetUnclippedOuterRect();
		float fXProgressPosition = 0.f;
		float fYProgressPosition = 0.f;
		if (m_isVertical)
		{
			fXProgressPosition = rtUnclippedRect.left + rtUnclippedRect.getWidth() * 0.5f;
			fYProgressPosition = rtUnclippedRect.top + m_fProgress * rtUnclippedRect.getHeight();
			
		}
		else
		{
			fXProgressPosition = rtUnclippedRect.left + m_fProgress * rtUnclippedRect.getWidth();
			fYProgressPosition = rtUnclippedRect.top + rtUnclippedRect.getHeight() * 0.5f;
		}

		ERect rtHeader;
		Vector2 v2HalfSize = m_HeaderImage.GetImageSize() * 0.5f;
		rtHeader.left = fXProgressPosition - v2HalfSize.x;
		rtHeader.right = fXProgressPosition + v2HalfSize.x;
		rtHeader.top = fYProgressPosition - v2HalfSize.y;
		rtHeader.bottom = fYProgressPosition + v2HalfSize.y;
		return rtHeader.isPointInRect(position);
	}

}
