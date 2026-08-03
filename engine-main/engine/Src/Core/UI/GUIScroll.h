/********************************************************************
filename: 	GUIScroll.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SCROLL_H__
#define __GUI_SCROLL_H__

#include "Core.h"

namespace LORD
{
	class GUIRenderImage;
	class LORD_CORE_API GUIScroll: public ObjectAlloc
	{
	public:
		GUIScroll();
		~GUIScroll();

		enum ScrollType
		{
			VERTICAL_SCROLL,
			HORIZON_SCROLL
		};

		enum ThumbState
		{
			TS_NONE,
			TS_SHOW,
			TS_FADE_OUT,
			TS_HIDE,
		};

		float			GetOffset() { return m_fOffset; }
		void            SetOffset(float offset) { m_fOffset = offset;}

		void            SetScrollType(ScrollType stype) { m_ScrollType = stype; }

		void			TouchDown(const Vector2& position, float fDocSize, float fWindowSize);

		float			TouchMove(const Vector2& position, float fStartSlowMinOffset);

		void			TouchUp(const Vector2& position, float fStartSlowMinOffset);

		void			MotionRelease(float fStartSlowMinOffset);

		float			Update(ui32 nTimeElapse, float fStartSlowMinOffset);

		void			DrawThumb(const ERect& parentRect);

	private:			
		void			UpdateThumbPosition();
	private:

		bool			m_bIsDragging;
		Vector2			m_DragPosition;
		//float			m_fYOffset;
		float			m_fOffset;
		bool			m_bIsDragGoBack;
		float			m_fWholeGoBackDistance;

		ScrollType      m_ScrollType;

		// 滑动加速移动过程
		ui32			m_nStartFlyTime;
		Vector2			m_StartFlyPosition;
		float			m_fFlyInitSpeed;
		bool			m_bIsFlying;
		ui32			m_nFlyTime;
		float			m_fMoveDis;
        bool            m_bHalfSpeedFlag;

		// 增加滚动条
		GUIRenderImage*	m_pScrollThumb;
		float			m_ThumbSize;
		ThumbState		m_ThumbState;
		float			m_fDocSize;
		float			m_fWindowSize;
		float			m_ThumbPosition;
		ui32			m_ThumbFadeOutTime;
		float			m_ThumbSizeChange;
					

	};
}

#endif
