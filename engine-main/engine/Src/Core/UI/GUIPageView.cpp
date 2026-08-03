#include "GUIPageView.h"
#include "GUIWindowManager.h"
#include "Object/Root.h"
#include "GUISystem.h"

namespace LORD
{
	const LORD::i32 gStepWidth = 40;//暂时设定为匀速翻页。。此值为步长
	const float fEdge = 5.0f;

	GUIPageView::GUIPageView( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)	
		, m_bLoopSwitch(true)
		, m_bHoriValid(true)
		, m_bVertValid(false)		
		, m_bIsDrag( false ) 
		, m_pContainer(NULL)
		, m_pCurPage(NULL)
		, m_curPageIndex(-1)
		, m_pageSwitchSpeed(20.0f)
		, m_curStatus(IDLE)		
		, m_fSlideLen(0.0f)
		, m_pageStatus( NORMAL )
	{
		SetClipChild(true);		
		m_horiScroll.SetScrollType(GUIScroll::HORIZON_SCROLL);
		m_vertScroll.SetScrollType(GUIScroll::VERTICAL_SCROLL);
	}

	GUIPageView::~GUIPageView()
	{
		
	}

	void GUIPageView::Destroy()
	{
		GUIWindow::Destroy();

		DeleteAllPages();
	}

	bool GUIPageView::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_PAGE_TURN)
		{
			m_bHoriValid=StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		else if (strName == GWP_PAGE_INNER_SLIDE)
		{
			m_bVertValid=StringUtil::ParseBool(strValue.c_str());
			return true;
		}	
		else if (strName == GWP_PAGE_LOOP_SWITCH)
		{
			m_bLoopSwitch=StringUtil::ParseBool(strValue.c_str());
			return true;
		}	

		return false;
	}

	bool GUIPageView::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_PAGE_TURN)
		{
			strValue = StringUtil::ToString(m_bHoriValid).c_str();
			return true;
		}
		else if (strName == GWP_PAGE_INNER_SLIDE)
		{
			strValue = StringUtil::ToString(m_bVertValid).c_str();
			return true;
		}
		else if (strName == GWP_PAGE_LOOP_SWITCH)
		{
			strValue = StringUtil::ToString(m_bLoopSwitch).c_str();
			return true;
		}
		
		return false;
	}

	void GUIPageView::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_PAGE_TURN);
		properlist.push_back(GWP_PAGE_INNER_SLIDE);		
		properlist.push_back(GWP_PAGE_LOOP_SWITCH);		
	}

	bool GUIPageView::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_PAGE_TURN)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_PAGE_INNER_SLIDE)
		{
			type = GPT_BOOL;
			return true;
		}	
		else if (strName == GWP_PAGE_LOOP_SWITCH)
		{
			type = GPT_BOOL;
			return true;
		}	
		return false;
	}

	void GUIPageView::SetConfigInfo(bool horiScroll ,bool vertScroll, bool loopSwitch)
	{
		m_bHoriValid = horiScroll ;
		m_bVertValid = vertScroll ;
		m_bLoopSwitch =loopSwitch ;
	}

	GUIWindow * GUIPageView::GetPageByIndex(size_t index)
	{
		if (index <0 || index >= m_vectPages.size())
			return NULL;
		return m_vectPages[index];
	}
	
	void GUIPageView::DeleteAllPages()
	{
		if (m_pContainer)
		{
			while(m_pContainer->GetChildCount() != 0)
			{
				GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
				m_pContainer->RemoveChildWindow(wnd);
			}						
		}
		size_t count = m_vectPages.size();
		for (size_t i=0; i<count; i++)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_vectPages[i]);											
		}	
		m_vectPages.clear();
	}

	void GUIPageView::AddNewPage(GUIWindow* pPage)
	{
		if (NULL == pPage)
			return;

		if (!m_pContainer)
		{
			m_pContainer = GUIWindowManager::Instance()->CreateGUIWindow(GWT_DEFAULT_WINDOW, GetName()+"_Container");
			AddChildWindow(m_pContainer);
			m_pContainer->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			m_pContainer->SetSize(UVector2(UDim(1.0f, 0), UDim(1.0f, 0)));
			m_pContainer->SetTouchable(false);
		}
		while(m_pContainer->GetChildCount() != 0)
		{
			GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
			m_pContainer->RemoveChildWindow(wnd);
		}				
		m_vectPages.push_back(pPage);
		pPage->SetVisible(false);			

		Vector2 pixelSize = GetPixelSize();	
		size_t num = m_vectPages.size();	
		float x = 0.0f;		
		size_t i;
		for (i=0; i<2 && i<num; ++i)
		{
			m_pContainer->AddChildWindow(m_vectPages[i]);	
			m_vectPages[i]->SetPosition(UVector2(UDim(0,x),UDim(0,0)));
			m_vectPages[i]->SetWidth(UDim(0,pixelSize.x));
			m_vectPages[i]->SetVisible(true);
			x = x + pixelSize.x;			
		}		
		m_pContainer->SetWidth(UDim(0,i * pixelSize.x));		
	}

	void GUIPageView::AddPages(vector<GUIWindow*>::type & vect_pages)
	{
		if (vect_pages.size()<1)
			return;
		
		if (!m_pContainer)
		{
			m_pContainer = GUIWindowManager::Instance()->CreateGUIWindow(GWT_DEFAULT_WINDOW, GetName()+"_Container");
			AddChildWindow(m_pContainer);
			m_pContainer->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			m_pContainer->SetSize(UVector2(UDim(1.0f, 0), UDim(1.0f, 0)));
			m_pContainer->SetTouchable(false);
		}
		while(m_pContainer->GetChildCount() != 0)
		{
			GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
			m_pContainer->RemoveChildWindow(wnd);
		}
		
		Vector2 pixelSize = GetPixelSize();
		size_t num = vect_pages.size();		
		for (size_t i=0; i<num; ++i)	
		{
			m_vectPages.push_back(vect_pages[i]);
			vect_pages[i]->SetVisible(false);
		}

		float x = 0.0f;		
		size_t i;
		for (i=0; i<2 && i<num; ++i)
		{
			m_pContainer->AddChildWindow(vect_pages[i]);	
			vect_pages[i]->SetPosition(UVector2(UDim(0,x),UDim(0,0)));
			vect_pages[i]->SetWidth(UDim(0,pixelSize.x));
			vect_pages[i]->SetVisible(true);
			x = x + pixelSize.x;
		}		
		m_pContainer->SetWidth(UDim(0,i * pixelSize.x));		
	}

	void GUIPageView::JumpToPage(size_t index)
	{
		size_t num =  m_vectPages.size();
		if (index<0 || index>=num || NULL == m_pContainer)
			return;

		while(m_pContainer->GetChildCount() != 0)
		{
			GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
			wnd->SetVisible(false);
			m_pContainer->RemoveChildWindow(wnd);
		}

		m_pCurPage = m_vectPages[index];
		m_curPageIndex = 0;

		float x = 0.0f;		
		size_t i;
		for (i=0; i<2 && i<num; ++i)
		{
			size_t t = index % num;
			m_pContainer->AddChildWindow(m_vectPages[t]);	
			m_vectPages[t]->SetPosition(UVector2(UDim(0,x),UDim(0,0)));			
			m_vectPages[t]->SetVisible(true);
			x = x + GetPixelSize().x;
			index++ ;
		}		

		m_pContainer->SetWidth(UDim(0,i * GetPixelSize().x));		
		m_pContainer->SetXPosition(UDim(0,0));
	}

	void GUIPageView::PreviousPage()
	{
		m_fSlideLen = GetPixelSize().x;
		size_t liCount = m_vectPages.size();
		size_t liIndex = 0;
		size_t liPreIndex = 0;

		float liCurOffset = m_pContainer->GetXPosition().d_offset;
		i32 liCurIndex = liCurOffset < 0 ? 1 : 0;
		GUIWindow* pWnd = m_pContainer->GetChildByIndex( liCurIndex );
		for ( size_t i = 0 ; i < liCount; ++i )
		{
			if ( pWnd == m_vectPages[i] )
			{
				liIndex = i;
				break;
			}
		}

		liPreIndex = liIndex == 0 ? liCount-1 : liIndex - 1;

		while(m_pContainer->GetChildCount() != 0)
		{
			GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
			m_pContainer->RemoveChildWindow(wnd);
			wnd->SetVisible( false );
		}

		m_pContainer->AddChildWindow( m_vectPages[liPreIndex] );
		m_pContainer->AddChildWindow( m_vectPages[liIndex] );				
		m_vectPages[liPreIndex]->SetVisible( true );
		m_vectPages[liIndex]->SetVisible( true );
		m_vectPages[liPreIndex]->SetXPosition( UDim(0,0) );	
		m_vectPages[liIndex]->SetXPosition( UDim(0,GetPixelSize().x) );	

		m_pContainer->SetXPosition( UDim( 0, -m_fSlideLen ) );
		m_pageStatus = RIGHT_SWITCH;
		m_curStatus = PAGE_SWITCH;

		//GUISystem::Instance()->GetEventCallback()( GET_PAGE_START_MOVE, GetOwnerDialogName(), GetName(), 0, 0 );
	}

	void GUIPageView::NextPage()
	{
		m_fSlideLen = GetPixelSize().x;
		size_t liCount = m_vectPages.size();
		size_t liIndex = 0;
		size_t liNextIndex = 0;

		float liCurOffset = m_pContainer->GetXPosition().d_offset;
		i32 liCurIndex = liCurOffset < 0 ? 1 : 0;
		GUIWindow* pWnd = m_pContainer->GetChildByIndex( liCurIndex );
		for ( size_t i = 0 ; i < liCount; ++i )
		{
			if ( pWnd == m_vectPages[i] )
			{
				liIndex = i;
				break;
			}
		}

		liNextIndex = liIndex == liCount-1 ? 0 : liIndex + 1;

		while(m_pContainer->GetChildCount() != 0)
		{
			GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
			m_pContainer->RemoveChildWindow(wnd);
			wnd->SetVisible( false );
		}

		m_pContainer->AddChildWindow( m_vectPages[liIndex] );
		m_pContainer->AddChildWindow( m_vectPages[liNextIndex] );				
		m_vectPages[liIndex]->SetVisible( true );
		m_vectPages[liNextIndex]->SetVisible( true );
		m_vectPages[liIndex]->SetXPosition( UDim(0,0) );	
		m_vectPages[liNextIndex]->SetXPosition( UDim(0,GetPixelSize().x) );	

		m_pContainer->SetXPosition( UDim( 0, 0 ) );

		m_pageStatus = LEFT_SWITCH;
		m_curStatus = PAGE_SWITCH;

		//GUISystem::Instance()->GetEventCallback()( GET_PAGE_START_MOVE, GetOwnerDialogName(), GetName(), 0, 0 );
	}

	size_t GUIPageView::GetNextPageIndex(size_t offset)
	{
		size_t index =0;
		size_t count = m_vectPages.size();
		for (size_t i =0; i<count; ++i)
		{
			if (m_vectPages[i] == m_pCurPage)
			{
				index = i;
				break;
			}
		}

		if (1 == offset)//加一页		
			return (index + 1) % count;		
		else//减一页	
			return (index - 1 + count) % count;	
	}

	void GUIPageView::AdaptPagePosition(const Vector2 & curPos)
	{
		size_t count = m_vectPages.size();
		if (count<2)//只有一页的情况
			return;
		if (!m_bLoopSwitch)//不循环翻页
		{
			if (m_vectPages[count -1] == m_pCurPage)//当前是最后一页
				return;
			if (m_vectPages[0] == m_pCurPage)//当前是首页
				return;
			size_t index =0;
			for (index ; index < count; ++ index)
			{
				if (m_vectPages[index] == m_pCurPage)
					break;
			}
			if (curPos.x < m_downPos.x)//向左滑动
			{
				if (m_curPageIndex == 0)
					return;
				else
				{
					m_vectPages[index - 1]->SetVisible(false);
					m_vectPages[index]->SetVisible(false);
					
					while(m_pContainer->GetChildCount() != 0)
					{
						GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
						m_pContainer->RemoveChildWindow(wnd);
					}

					m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset + GetPixelSize().x));
					m_horiScroll.SetOffset(m_pContainer->GetXPosition().d_offset);

					m_pContainer->AddChildWindow(m_vectPages[index]);	
					m_vectPages[index]->SetXPosition(UDim(0,0));					
					m_vectPages[index]->SetVisible(true);
					m_pContainer->AddChildWindow(m_vectPages[index + 1]);	
					m_vectPages[index + 1]->SetXPosition(UDim(0,GetPixelSize().x));					
					m_vectPages[index + 1]->SetVisible(true);	
					m_curPageIndex = 0;
					return;
				}
			}
			else if(curPos.x > m_downPos.x)//向右滑动
			{
				if (m_curPageIndex == 1)
					return;
				else
				{
					m_vectPages[index + 1]->SetVisible(false);
					m_vectPages[index]->SetVisible(false);
					
					while(m_pContainer->GetChildCount() != 0)
					{
						GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
						m_pContainer->RemoveChildWindow(wnd);
					}

					m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset - GetPixelSize().x));
					m_horiScroll.SetOffset(m_pContainer->GetXPosition().d_offset);

					m_pContainer->AddChildWindow(m_vectPages[index - 1]);	
					m_vectPages[index - 1]->SetXPosition(UDim(0,0));					
					m_vectPages[index - 1]->SetVisible(true);
					m_pContainer->AddChildWindow(m_vectPages[index]);	
					m_vectPages[index]->SetXPosition(UDim(0,GetPixelSize().x));					
					m_vectPages[index]->SetVisible(true);	
					m_curPageIndex = 1;
					return;
				}
			}
		}
		else//循环翻页
		{
			size_t index =0;
			for (index ; index < count; ++ index)
			{
				if (m_vectPages[index] == m_pCurPage)
					break;
			}
			if (curPos.x < m_downPos.x)//向左滑动
			{
				if (m_curPageIndex == 0)
					return;
				else
				{
					m_pContainer->GetChildByIndex(0)->SetVisible(false);
					m_pContainer->GetChildByIndex(1)->SetVisible(false);
					
					while(m_pContainer->GetChildCount() != 0)
					{
						GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
						m_pContainer->RemoveChildWindow(wnd);
					}

					m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset + GetPixelSize().x));
					m_horiScroll.SetOffset(m_pContainer->GetXPosition().d_offset);

					m_pContainer->AddChildWindow(m_vectPages[index]);	
					m_vectPages[index]->SetXPosition(UDim(0,0));					
					m_vectPages[index]->SetVisible(true);
					size_t t_index = (index + 1) % count;
					
					m_pContainer->AddChildWindow(m_vectPages[t_index]);	
					m_vectPages[t_index]->SetXPosition(UDim(0,GetPixelSize().x));					
					m_vectPages[t_index]->SetVisible(true);	
					m_curPageIndex = 0;
					return;
				}				
			}
			else if(curPos.x > m_downPos.x)//向右滑动
			{
				if (m_curPageIndex == 1)
					return;
				else
				{
					m_pContainer->GetChildByIndex(0)->SetVisible(false);
					m_pContainer->GetChildByIndex(1)->SetVisible(false);
					
					while(m_pContainer->GetChildCount() != 0)
					{
						GUIWindow* wnd = m_pContainer->GetChildByIndex(0);
						m_pContainer->RemoveChildWindow(wnd);
					}

					m_pContainer->SetXPosition(UDim(0, m_pContainer->GetXPosition().d_offset - GetPixelSize().x));
					m_horiScroll.SetOffset(m_pContainer->GetXPosition().d_offset);

					size_t t_index = 0;
					t_index = (index - 1 + count) % count;
					m_pContainer->AddChildWindow(m_vectPages[t_index]);	
					m_vectPages[t_index]->SetXPosition(UDim(0,0));					
					m_vectPages[t_index]->SetVisible(true);	
					m_pContainer->AddChildWindow(m_vectPages[index]);	
					m_vectPages[index]->SetXPosition(UDim(0,GetPixelSize().x));					
					m_vectPages[index]->SetVisible(true);
					
					m_curPageIndex = 1;
					return;
				}				
			}
		}
	}

	bool GUIPageView::TouchDown( const Vector2& position )
	{	
		if (m_pContainer)
		{
			if ( m_pageStatus != NORMAL )
			{
				return false;
			}
			if ( PAGE_SWITCH == m_curStatus )
			{
				return true;
			}
			m_curStatus = IDLE;
			m_downPos   = position;
			m_downTime  = Root::Instance()->getCurrentTime();			
			m_pageStatus = NORMAL;
			i32 i = 0;
			size_t count = m_pContainer->GetChildCount();
			for (; i < (int)count; ++i)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				if (pItem->IsHit(position))
				{
					m_pCurPage = pItem;
					m_curPageIndex = i;					
					break;
				}
			}
			if (m_bHoriValid)	
			{
				m_horiScroll.TouchDown(position, 0.0f, 0.0f);
				m_horiScroll.SetOffset(-1 * m_curPageIndex * GetPixelSize().x );
				m_pContainer->SetXPosition(UDim(0,m_horiScroll.GetOffset()));		
				//m_pContainer->SetXPosition(UDim(0, -1 * m_curPageIndex * GetPixelSize().x));
			}
			if (m_bVertValid)			
				m_vertScroll.TouchDown(position, 0.0f, 0.0f);				
		}
		return true;
	}

	bool GUIPageView::TouchMove( const Vector2& position )
	{
		if (m_pContainer)
		{
			if ( m_pageStatus != NORMAL )
			{
				return false;
			}
			GUIWindow* pWnd = m_pContainer->GetChildByIndex( m_curPageIndex );
			if ( pWnd != NULL && !pWnd->IsHit( position ) )
			{
				TouchUp( position );
				return false;
			}
			switch(m_curStatus)
			{
			case IDLE:
				{
					float x = LORD::Math::Abs(position.x-m_downPos.x);
					float y = LORD::Math::Abs(position.y-m_downPos.y);
					if (x>y && m_bHoriValid)					
						m_curStatus = PAGE_SWITCH;
					else if(m_bVertValid)
						m_curStatus = VERT_SCROLL;
				}
				break;
			case PAGE_SWITCH:
				{
					float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;
					float xOffset = m_horiScroll.TouchMove(position, fStartSlowMinOffset);
					m_pContainer->SetXPosition(UDim(0, xOffset));
					AdaptPagePosition(position);
					if ( !m_bIsDrag && m_bHoriValid )
					{
						m_bIsDrag = true;
						//GUISystem::Instance()->GetEventCallback()( GET_PAGE_START_MOVE, GetOwnerDialogName(), GetName(), 0, 0 );
					}
				}
				break;
			case VERT_SCROLL:
				{
					if (!m_pCurPage)
						break;
					float fStartSlowMinOffset = GetPixelSize().y - m_pCurPage->GetPixelSize().y;
					float yOffset = m_vertScroll.TouchMove(position, fStartSlowMinOffset);
					m_pCurPage->SetYPosition(UDim(0, yOffset));
					if ( !m_bIsDrag && m_bVertValid )
					{
						m_bIsDrag = true;
						//GUISystem::Instance()->GetEventCallback()( GET_PAGE_START_MOVE, GetOwnerDialogName(), GetName(), 0, 0 );
					}
				}
				break;
			}			
		}
		return true;
	}

	bool GUIPageView::TouchUp( const Vector2& position )
	{		
		if (m_pContainer)
		{
			if ( m_pageStatus != NORMAL )
			{
				m_bIsDrag = false;
				return false;
			}
			switch(m_curStatus)
			{			
			case PAGE_SWITCH:
				{
					int slidInc = 1;//向左
					if (position.x - m_downPos.x > 0)
						slidInc = -1;//向右
					float slidLen = LORD::Math::Abs(position.x - m_downPos.x);					
					ui32 slidTime = Root::Instance()->getCurrentTime() - m_downTime;
					
					//翻页..拖动过一半时自动翻页。。如果down和up的时间间隔短并且移动距离有一定的值也进行翻页
					//slidTime间隔时间。。之前为120。。但是在手机上手感不好。。慢慢调吧
					if (slidLen > (GetPixelSize().x/2) || ( slidLen > 20 && slidTime < 300 ) )//翻页
					{
						if (slidInc == 1)//向左翻页
						{	
							if ((m_curPageIndex+1) == m_pContainer->GetChildCount())
							{ 								
								m_fSlideLen = LORD::Math::Abs(m_curPageIndex * GetPixelSize().x + m_pContainer->GetXPosition().d_offset);
								m_pageStatus = RIGHT_BACK_OUT;
								break;
							}								
							m_fSlideLen =  m_pContainer->GetXPosition().d_offset + (m_curPageIndex + 1) * GetPixelSize().x;							
							m_pageStatus = LEFT_SWITCH;
							//GUISystem::Instance()->GetEventCallback()(GET_PAGE_CHANGED, GetOwnerDialogName(), GetName(), GetNextPageIndex(1), 0);
						}
						else//向右翻页
						{							
							if (m_curPageIndex == 0)
							{
								m_fSlideLen = LORD::Math::Abs(m_pContainer->GetXPosition().d_offset);						
								m_pageStatus = LEFT_BACK_OUT;
								break;
							}											
							m_fSlideLen = GetPixelSize().x - (m_curPageIndex * GetPixelSize().x + m_pContainer->GetXPosition().d_offset);						
							m_pageStatus = RIGHT_SWITCH;
							//GUISystem::Instance()->GetEventCallback()(GET_PAGE_CHANGED, GetOwnerDialogName(), GetName(), GetNextPageIndex(-1), 0);
						}						
						
					}
					else//回退
					{			
						// float fStartSlowMinOffset = 0.0f;
						if (-1 == slidInc)//向右滑再回退
						{							
							if (m_curPageIndex == 0)
							{
								m_fSlideLen = LORD::Math::Abs(m_pContainer->GetXPosition().d_offset);
								m_pageStatus = LEFT_BACK_OUT;
							}
							else	
							{
								m_fSlideLen =  m_pContainer->GetXPosition().d_offset + m_curPageIndex * GetPixelSize().x;	
								m_pageStatus = LEFT_BACK_NORMAL;
							}
						}
						else//向左滑再回退
						{
							if (m_curPageIndex + 1 == m_pContainer->GetChildCount())
							{								
								m_fSlideLen = LORD::Math::Abs(m_curPageIndex * GetPixelSize().x + m_pContainer->GetXPosition().d_offset);
								m_pageStatus = RIGHT_BACK_OUT;
							}
							else
							{ 								
								m_fSlideLen = LORD::Math::Abs(m_curPageIndex * GetPixelSize().x + m_pContainer->GetXPosition().d_offset);
								m_pageStatus = RIGHT_BACK_NORMAL;
							}							
						}						
					}					
				 }
				break;
			case VERT_SCROLL:
				{
					if (!m_pCurPage)
						break;
					float fStartSlowMinOffset = GetPixelSize().y - m_pCurPage->GetPixelSize().y;
					m_vertScroll.TouchUp(position, fStartSlowMinOffset);					
				}
				break;
			}
		}

		m_bIsDrag = false;
		return true;
	}

	bool GUIPageView::MotionRelease()
	{		
		if (m_pContainer)
		{
			if (PAGE_SWITCH == m_curStatus)
			{
				float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;
				m_horiScroll.MotionRelease(fStartSlowMinOffset);
			}
			else if (VERT_SCROLL == m_curStatus)
			{
				if (!m_pCurPage)
					return false;
				float fStartSlowMinOffset = GetPixelSize().y - m_pCurPage->GetPixelSize().y;
				m_vertScroll.MotionRelease(fStartSlowMinOffset);
			}			
		}

		return true;
	}

	void GUIPageView::UpdateSelf( ui32 nTimeElapse )
	{
		if (m_pContainer)
		{			
			if (PAGE_SWITCH == m_curStatus)
			{				
				switch(m_pageStatus)
				{
				case NORMAL:					
					break;
				case LEFT_SWITCH:
				case LEFT_BACK_NORMAL:
				case LEFT_BACK_OUT:
					{						
						if (m_fSlideLen > gStepWidth)
						{
							m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset - gStepWidth));
							m_fSlideLen -= gStepWidth;
						}
						else
						{
							m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset - m_fSlideLen));
							m_fSlideLen = 0;
							m_pageStatus = NORMAL;
							m_curStatus = IDLE;
							//GUISystem::Instance()->GetEventCallback()( GET_PAGE_MOVE_FINISHED, GetOwnerDialogName(), GetName(), 0, 0 );
						}
					}
					break;
				case RIGHT_SWITCH:
				case RIGHT_BACK_NORMAL:
				case RIGHT_BACK_OUT:
					{
						if (m_fSlideLen > gStepWidth)
						{
							m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset + gStepWidth));
							m_fSlideLen -= gStepWidth;
						}
						else
						{
							m_pContainer->SetXPosition(UDim(0,m_pContainer->GetXPosition().d_offset + m_fSlideLen));
							m_fSlideLen = 0;
							m_pageStatus = NORMAL;
							m_curStatus = IDLE;
							//GUISystem::Instance()->GetEventCallback()( GET_PAGE_MOVE_FINISHED , GetOwnerDialogName(), GetName(), 0, 0 );
						}
					}
					break;
				}
			}
			else if (VERT_SCROLL == m_curStatus)
			{
				if (!m_pCurPage)
					return;
				float fStartSlowMinOffset = GetPixelSize().y - m_pCurPage->GetPixelSize().y;
				float fYOffset = m_vertScroll.Update(nTimeElapse, fStartSlowMinOffset);
				m_pCurPage->SetYPosition(UDim(0, fYOffset));
			}
		}
	}
}