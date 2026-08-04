#include "GUIScroll.h"
#include "Object/Root.h"
#include "GUISystem.h"
#include "GUIRenderImage.h"

namespace LORD
{
	const float MAX_SCROLL_OUT_RANGE = 100.0f;
	// 阻尼系数 0 ～ 1
	const float DAMP_PARAM = 0.8f;
	const float GO_BACK_STEP = 20.0f;//20.0f
	const ui32	MAX_FLY_TIME = 500;

	const float MIN_THUMB_SIZE = 50.0f;
	const float THUMB_FADE_OUT_TIME = 500;

	GUIScroll::GUIScroll()
		: m_bIsDragging(false)
		, m_DragPosition(Vector2::ZERO)
		, m_fOffset(0.0f)
		, m_bIsDragGoBack(false)
		, m_fWholeGoBackDistance(0.0f)
		, m_nStartFlyTime(0)
		, m_StartFlyPosition(Vector2::ZERO)
		, m_fFlyInitSpeed(0.0f)
		, m_bIsFlying(false)
		, m_nFlyTime(0)
		, m_ScrollType(VERTICAL_SCROLL)
		, m_fMoveDis(0.0f)
		, m_bHalfSpeedFlag(false)
		, m_ThumbSize(0.0f)
		, m_ThumbState(TS_NONE)
		, m_fDocSize(0.0f)
		, m_fWindowSize(0.0f)
		, m_ThumbPosition(0.0f)
		, m_ThumbFadeOutTime(0)
		, m_ThumbSizeChange(0.0f)
	{
		m_pScrollThumb = LordNew GUIRenderImage;
		m_pScrollThumb->SetWholeImageName(GUISystem::Instance()->GetScrollThumbImage());
	}

	GUIScroll::~GUIScroll()
	{
		LordSafeDelete(m_pScrollThumb);
	}

	void GUIScroll::TouchDown(const Vector2& position, float fDocSize, float fWindowSize)
	{
		m_bIsDragging = true;
		m_DragPosition = position;

		m_StartFlyPosition = position;
		m_nStartFlyTime = Root::Instance()->getCurrentTime();

		m_fDocSize = fDocSize;
		m_fWindowSize = fWindowSize;

		if (fDocSize < fWindowSize)
		{
			m_ThumbSize = 0.0f;
		}
		else
		{
			m_ThumbSize = fWindowSize - (fDocSize - fWindowSize);
			if (m_ThumbSize < MIN_THUMB_SIZE)
			{
				m_ThumbSize = MIN_THUMB_SIZE;
			}
		}
	}

	float GUIScroll::TouchMove(const Vector2& position, float fStartSlowMinOffset)
	{
		if (m_bIsDragging)
		{
			float pos_v, d_pos_v;
			if (m_ScrollType == VERTICAL_SCROLL)
			{
				pos_v = position.y;
				d_pos_v = m_DragPosition.y;
			}
			else
			{
				pos_v = position.x;
				d_pos_v = m_DragPosition.x;
			}

			float fMove = Math::Abs(pos_v - d_pos_v);
			if (fMove < 10.0f) {
				return m_fOffset;
			}

			m_fOffset += pos_v - d_pos_v;

			fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

			float fStartSlowMaxOffset = 0.0f;

			float fMinOffset = fStartSlowMinOffset - MAX_SCROLL_OUT_RANGE;

			float fMaxOffset = MAX_SCROLL_OUT_RANGE;

			if (m_fOffset > fStartSlowMaxOffset)
			{
				// 向下滑动变慢
				m_fOffset -= DAMP_PARAM * (pos_v - d_pos_v);
			}
			else if (m_fOffset < fStartSlowMinOffset)
			{
				// 向上滑动变慢
				m_fOffset -= DAMP_PARAM * (pos_v - d_pos_v);
			}

			m_fOffset = m_fOffset < fMinOffset ? fMinOffset : m_fOffset;
			m_fOffset = m_fOffset > fMaxOffset ? fMaxOffset : m_fOffset;

			m_DragPosition = position;


			// 计算thumb的位置
			m_ThumbState = TS_SHOW;

			UpdateThumbPosition();
		}

		return m_fOffset;
	}

	void GUIScroll::TouchUp(const Vector2& position, float fStartSlowMinOffset)
	{
		m_bIsDragging = false;

		float fFlyMoveDis = 0.0f;
		if (m_ScrollType == VERTICAL_SCROLL)
			fFlyMoveDis = position.y - m_StartFlyPosition.y;
		else
			fFlyMoveDis = position.x - m_StartFlyPosition.x;

		float fMove = Math::Abs(fFlyMoveDis);
		if (fMove < 30.0f) {
			return;
		}


		ui32 nElapsTime = Root::Instance()->getCurrentTime() - m_nStartFlyTime;

		if (nElapsTime < 300)
		{
			//m_fFlyInitSpeed = fFlyMoveDis / nElapsTime;
			float fFactor = fFlyMoveDis > 0.0f ? 1.0f : -1.0f;
			m_fFlyInitSpeed = 0.8f * fFactor;
			if (Math::Abs(m_fFlyInitSpeed) > 0)
			{
				m_bIsFlying = true;
				m_bHalfSpeedFlag = false;
				m_nFlyTime = 0;
			}
		}

		fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

		float fStartSlowMaxOffset = 0.0f;
		if (m_fOffset > fStartSlowMaxOffset)
		{
			// 向下滑动变慢
			m_bIsDragGoBack = true;
			m_fWholeGoBackDistance = m_fOffset - fStartSlowMaxOffset;
		}
		else if (m_fOffset < fStartSlowMinOffset)
		{
			// 向上滑动变慢
			m_bIsDragGoBack = true;
			m_fWholeGoBackDistance = fStartSlowMinOffset - m_fOffset;
		}

		if (!m_bIsDragGoBack && !m_bIsFlying)
		{
			m_ThumbState = TS_FADE_OUT;
			m_ThumbFadeOutTime = 0;
		}

		// 处理thumb
		UpdateThumbPosition();
	}

	void GUIScroll::MotionRelease(float fStartSlowMinOffset)
	{
		m_bIsDragging = false;

		fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

		float fStartSlowMaxOffset = 0.0f;
		if (m_fOffset > fStartSlowMaxOffset)
		{
			// 向下滑动变慢
			m_bIsDragGoBack = true;
			m_fWholeGoBackDistance = m_fOffset - fStartSlowMaxOffset;
		}
		else if (m_fOffset < fStartSlowMinOffset)
		{
			// 向上滑动变慢
			m_bIsDragGoBack = true;
			m_fWholeGoBackDistance = fStartSlowMinOffset - m_fOffset;
		}
	}

	float GUIScroll::Update(ui32 nTimeElapse, float fStartSlowMinOffset)
	{
		fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

		float fStartSlowMaxOffset = 0.0f;

		if (m_bIsDragGoBack)
		{
			float fParam = 1.0f;
			if (m_fOffset > fStartSlowMaxOffset)
			{
				// 向下滑动变慢
				fParam = -1.0f;
			}
			else if (m_fOffset < fStartSlowMinOffset)
			{
				fParam = 1.0f;
				// 向上滑动变慢
			}

			if (fParam > 0)
			{
				m_fOffset += GO_BACK_STEP;
			}
			else
			{
				m_fOffset -= GO_BACK_STEP;
			}

			m_fMoveDis += GO_BACK_STEP;

			if (m_fMoveDis >= m_fWholeGoBackDistance)
			{
				m_bIsDragGoBack = false;

				if (fParam > 0)
				{
					m_fOffset -= (m_fMoveDis - m_fWholeGoBackDistance);
				}
				else
				{
					m_fOffset += (m_fMoveDis - m_fWholeGoBackDistance);
				}

				m_fMoveDis = 0.0f;

				m_bIsFlying = false;

				m_ThumbState = TS_FADE_OUT;
				m_ThumbFadeOutTime = 0;
			}

			UpdateThumbPosition();
		}
		else if (m_bIsFlying)
		{
			m_fOffset += m_fFlyInitSpeed * nTimeElapse;

			m_nFlyTime += nTimeElapse;

			if ((m_fOffset + 100.0f) < fStartSlowMinOffset)
			{
				//m_fOffset = fStartSlowMinOffset;
				m_bIsFlying = false;
				m_bIsDragGoBack = true;
				m_fWholeGoBackDistance = fStartSlowMinOffset - m_fOffset;
			}
			else if ((m_fOffset - 100.0f) > fStartSlowMaxOffset)
			{
				//m_fOffset = fStartSlowMaxOffset;
				m_bIsFlying = false;
				m_bIsDragGoBack = true;
				m_fWholeGoBackDistance = m_fOffset - fStartSlowMaxOffset;
			}

			if (!m_bHalfSpeedFlag && m_nFlyTime > MAX_FLY_TIME*0.5f)
			{
				m_fFlyInitSpeed *= 0.5f;
				m_bHalfSpeedFlag = true;
			}
			else if (m_nFlyTime > MAX_FLY_TIME)
			{
				m_nFlyTime = 0;
				m_bIsFlying = false;
				m_bHalfSpeedFlag = false;
				if (m_fOffset < fStartSlowMinOffset)
				{
					//m_fOffset = fStartSlowMinOffset;
					m_bIsFlying = false;
					m_bIsDragGoBack = true;
					m_fWholeGoBackDistance = fStartSlowMinOffset - m_fOffset;
				}
				else if (m_fOffset > fStartSlowMaxOffset)
				{
					//m_fOffset = fStartSlowMaxOffset;
					m_bIsFlying = false;
					m_bIsDragGoBack = true;
					m_fWholeGoBackDistance = m_fOffset - fStartSlowMaxOffset;
				}
				else
				{
					m_ThumbState = TS_FADE_OUT;
					m_ThumbFadeOutTime = 0;
				}
			}

			UpdateThumbPosition();
		}

		switch (m_ThumbState)
		{
		case TS_FADE_OUT:
		{
			if (m_ThumbFadeOutTime < THUMB_FADE_OUT_TIME)
			{
				m_ThumbFadeOutTime += nTimeElapse;
			}
			else
			{
				m_ThumbFadeOutTime = 0;
				m_ThumbState = TS_HIDE;
			}
		}
		break;
		case TS_NONE:
		case TS_SHOW:
		case TS_HIDE:
		{
			// 暂时什么都不做
		}
		break;
		default:
			break;
		}

		return m_fOffset;
	}

	void GUIScroll::UpdateThumbPosition()
	{
		float fFullMoveThumb = m_fWindowSize - m_ThumbSize;
		float fFullMoveDoc = m_fDocSize - m_fWindowSize;

		if (fFullMoveDoc > 0.0f)
		{
			float fPosition = fabs(m_fOffset) / fFullMoveDoc;
			m_ThumbPosition = fPosition * fFullMoveThumb;

			if (m_fOffset > 0)
			{
				m_ThumbSizeChange = m_fOffset;
			}
			else if (fabs(m_fOffset) > fFullMoveDoc)
			{
				m_ThumbSizeChange = fabs(m_fOffset) - fFullMoveDoc;
			}
			else
			{
				m_ThumbSizeChange = 0.0f;
			}
		}
		else
		{
			m_ThumbPosition = 0.0f;
		}
	}

	void GUIScroll::DrawThumb(const ERect& parentRect)
	{
		if (!m_pScrollThumb)
		{
			return;
		}

		if (m_ThumbState == TS_NONE
			|| m_ThumbState == TS_HIDE)
		{
			return;
		}

		float fSize = m_ThumbSize - m_ThumbSizeChange;
		fSize = fSize < 5.0f ? 5.0f : fSize;

		float fPosition = m_ThumbPosition;
		if (m_ThumbSizeChange > 0)
		{
			if (m_fOffset > 0.0f)
			{
				// 向下拉
				fPosition = 0.0f;
			}
			else
			{
				// 向上滚
				fPosition = fPosition + m_ThumbSizeChange;
			}
		}

		ERect destRect(parentRect);
		if (m_ScrollType == VERTICAL_SCROLL)
		{
			destRect.left = parentRect.right - 10.0f;

			if (m_ThumbSizeChange > 0)
			{
				if (m_fOffset >= 0.0f)
				{
					// 向下拉
					destRect.top = parentRect.top;
				}
				else
				{
					// 向上滚
					destRect.top = parentRect.bottom - fSize;
				}
			}
			else
			{
				destRect.top = parentRect.top + m_ThumbPosition;
			}

			destRect.right = parentRect.right;
			destRect.bottom = destRect.top + fSize;
		}
		else
		{
			if (m_ThumbSizeChange > 0)
			{
				if (m_fOffset >= 0.0f)
				{
					// 向下拉
					destRect.left = parentRect.left;
				}
				else
				{
					// 向上滚
					destRect.left = parentRect.right - fSize;
				}
			}
			else
			{
				destRect.left = parentRect.left + fPosition;
			}

			destRect.top = parentRect.bottom - 10.0f;
			destRect.right = destRect.left + fSize;
			destRect.bottom = destRect.bottom;
		}

		Color thumbColor = Color::WHITE;
		if (m_ThumbState == TS_SHOW)
		{
			thumbColor.a = 1.0f;
		}
		else if (m_ThumbState == TS_FADE_OUT)
		{
			thumbColor.a = 1.0f - m_ThumbFadeOutTime / THUMB_FADE_OUT_TIME;
		}
		m_pScrollThumb->DrawImage(NULL, destRect, thumbColor.getABGR(), 0.0f);
	}

}
