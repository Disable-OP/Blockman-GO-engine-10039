/********************************************************************
filename: 	GUIListHorizontalBox.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_LIST_HORIZONTAL_BOX_H__
#define __GUI_LIST_HORIZONTAL_BOX_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIScroll.h"

namespace LORD
{
	class LORD_CORE_API GUIListHorizontalBox: public GUIWindow
	{
	public:
		GUIListHorizontalBox(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIListHorizontalBox();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);

		void			AddItem(GUIWindow* pItem = NULL,bool defaultHigh=true);
		void			DeleteItem(size_t nIndex);
		void			ClearAllItem();

		void			SetItemNormalImage(const GUIString& strName);
		void			SetItemSelectImage(const GUIString& strName);
		void			SetItemWidth(float fWidth);
		float			GetItemWidth() { return m_fItemWidth; }

		void			SetInterval(float interval);

		void			SetSelectItem(size_t nIndex);
		void			UnSelectAll();

		void			SetBlinkSelect(bool setting) { m_bIsBlinkSelect = setting; }
		void			SetAllowScroll(bool setting) { m_allowScroll   = setting;}
		bool			IsBlinkSelect() { return m_bIsBlinkSelect; }
		void			SetMoveAble( bool setting ){ m_bCanMove = setting; };

		void			ResetScroll();
		float			GetScrollOffset();
		void			SetScrollOffset(float offSet);

		GUIWindow* getContainerWindow() {
			return m_pContainer;
		}

		void			setContainerWindow(GUIWindow* pContainer) { m_pContainer = pContainer; }
		virtual void	OnEndDraw();
		void setContainerAlignment();
		void			RestChildVisible();

	protected:
		void			LayoutChild();
		void			OnImageChanged();

	protected:
		GUIWindow*		m_pContainer;
		GUIScroll		m_scroll;
		//bool			m_bIsDragging;
		//Vector2		m_DragPosition;
		//float			m_fYOffset;
		//bool			m_bIsDragGoBack;
		//float			m_fWholeGoBackDistance;

		GUIString		m_strItemNormalImageName;
		GUIString		m_strItemSelectImageName;
		float			m_fItemWidth;
		int				m_nSelectIndex;
		bool			m_bIsBlinkSelect;
		bool			m_bCanMove;

		bool            m_allowScroll;

		float           m_fInteval;
		
		//ui32			m_nStartFlyTime;
		//Vector2			m_StartFlyPosition;
		//float			m_fFlyInitSpeed;
		//bool			m_bIsFlying;
		//ui32			m_nFlyTime;
	};
}

#endif