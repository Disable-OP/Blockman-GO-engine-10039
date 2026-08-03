/********************************************************************
filename: 	GUIPageView.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_PAGE_VIEW_H__
#define __GUI_PAGE_VIEW_H__

#include "GUIWindow.h"
#include "GUIScroll.h"

namespace LORD
{	
	class LORD_CORE_API GUIPageView: public GUIWindow
	{
	public:		

		GUIPageView(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIPageView();

		void            SetConfigInfo(bool horiScroll=true ,bool vertScroll=false, bool loopSwitch = false);

		void            AddPages(vector<GUIWindow*>::type & vect_pages);		
		void            AddNewPage(GUIWindow* pPage);
		void            JumpToPage(size_t index);	
		size_t          GetPagesCount(){ return m_vectPages.size();}
		GUIWindow *     GetPageByIndex(size_t index);
		void            DeleteAllPages();
		void			PreviousPage( );
		void			NextPage( );

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

 		virtual bool	TouchDown(const Vector2& position);
 		virtual bool	TouchMove(const Vector2& position);
 		virtual bool	TouchUp(const Vector2& position);
 		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);	

		virtual void    Destroy();

	protected:
		void            AdaptPagePosition(const Vector2 & curPos);
		size_t          GetNextPageIndex(size_t offset);

		enum CurStatus
		{
			IDLE,
			PAGE_SWITCH,
			VERT_SCROLL
		};
		enum PageStatus
		{
			NORMAL,
			LEFT_SWITCH,//向左翻一页
			RIGHT_SWITCH,//向右翻一页
			LEFT_BACK_OUT,//越界导致向左回退
			LEFT_BACK_NORMAL,//向左回退
			RIGHT_BACK_OUT,//越界导致向右回退
			RIGHT_BACK_NORMAL//向右回退
		};

	protected:			
		vector<GUIWindow*>::type m_vectPages;		

		GUIWindow*		m_pContainer;
		GUIWindow*		m_pCurPage;
		i32             m_curPageIndex;//0,1
		//滚动参数		
		bool            m_bLoopSwitch;//循环翻页
		bool            m_bHoriValid;//允许翻页
		bool            m_bVertValid;//允许页内滑动	
		bool			m_bIsDrag;//是否是拖动

		GUIScroll		m_horiScroll;		
		GUIScroll		m_vertScroll;	

		Vector2         m_downPos;//鼠标按下的位置
		ui32            m_downTime;//按下后未弹起的时间
		float           m_pageSwitchSpeed;//滑动速度大于该值则翻页

		CurStatus       m_curStatus;//PageView的当前状态
		PageStatus      m_pageStatus;//Page的当前状态		

		float           m_fSlideLen;//鼠标弹起后还需滑动的长度
	};
}

#endif

