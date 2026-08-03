#include "GUIBubbleContain.h"
#include "GUIWindowManager.h"
#include "GUIStaticImage.h"
#include "GUIStaticText.h"
#include "GUIMultiLineEdit.h"
#include "GUIRenderManager.h"
#include "Object/Root.h"
#include "GUISystem.h"

namespace LORD
{

	GUIBubbleContain::GUIBubbleContain( GUIWindowType nType, const GUIString& strName)
		:GUIWindow(nType, strName)	
		,m_strItemBackImageName("")//set:task.xml image:04
		,m_hAlign(HA_LEFT)
		,m_VAlign(VA_TOP)
		,m_wordWrap(true)	
		,m_bOverlap(true)
	{
		SetClipChild(true);
		setSpeedAndStayTime();
	}

	GUIBubbleContain::~GUIBubbleContain(void)
	{
	}

	void GUIBubbleContain::setSpeedAndStayTime(float speed, i32 stayTime)
	{
		m_goStep=speed;
		m_stayTime=stayTime;
	}

	bool GUIBubbleContain::SetProperty( const GUIString& strName, const GUIString& strValue )
	{	 
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			setItemBackImageName(strValue);
			return true;
		}	
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{			
			if (strValue == "Centre")
			{
				m_hAlign = HA_CENTRE;
			}
			else if (strValue == "Right")
			{
				m_hAlign = HA_RIGHT;
			}
			else
			{
				m_hAlign = HA_LEFT;
			}	
			if (m_hAlign!=HA_LEFT)			
				m_wordWrap=false;
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{			
			if (strValue == "Centre")
			{
				m_VAlign = VA_CENTRE;
			}
			else if (strValue == "Bottom")
			{
				m_VAlign = VA_BOTTOM;
			}
			else
			{
				m_VAlign = VA_TOP;
			}	
			if (m_VAlign!=VA_TOP)			
				m_wordWrap=false;
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			m_wordWrap=StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		return false;
	}

	bool GUIBubbleContain::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			strValue = m_strItemBackImageName;
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			switch(m_hAlign)
			{
			case HA_CENTRE:
				strValue = "Centre";
				break;

			case HA_RIGHT:
				strValue = "Right";
				break;

			default:
				strValue = "Left";
			}
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			switch(m_VAlign)
			{
			case VA_CENTRE:
				strValue = "Centre";
				break;

			case VA_BOTTOM:
				strValue = "Bottom";
				break;

			default:
				strValue = "Top";
			}
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			strValue = StringUtil::ToString(m_wordWrap).c_str();
			return true;
		}
		return false;
	}

	void GUIBubbleContain::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_BUBBLE_ITEM_BACK_IMAGE);
		properlist.push_back(GWP_TEXT_HORZ_ALIGN);
		properlist.push_back(GWP_TEXT_VERT_ALIGN);
		properlist.push_back(GWP_TEXT_WORDWRAP);
	}

	bool GUIBubbleContain::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			type = GPT_HA;
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			type = GPT_VA;
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			type = GPT_BOOL;
			return true;
		}
		return false;
	}

	void GUIBubbleContain::addItem(GUIString strValue, bool bStretched /* = false */)
	{		
		static ui32 itemIndex=0;
		String strTextName = StringUtil::Format("%s_Bubb_Text_%d", strValue.substr(0,30).c_str(),itemIndex);
		GUIStaticText* pStaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextName.c_str());
		
		String strBackImageName = StringUtil::Format("%s_Bubb_Back_%d",strValue.substr(0,30).c_str(),itemIndex);
		GUIStaticImage* pBackImage = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strBackImageName.c_str());
		
		if (pStaticText!=NULL && pBackImage!=NULL)
		{			
			pStaticText->SetWidth(UDim(0,GetPixelSize().x-20));				
			pStaticText->SetHeight(UDim(0,26));	
			
			pStaticText->SetText(strValue);			
			pStaticText->SetWordWrap(m_wordWrap);
			pStaticText->SetSelfAdaptionArea(m_wordWrap);
			pStaticText->SetTouchable( false );
			pStaticText->SetTextBoader(LORD::Color::BLACK);
			if (!m_wordWrap)
			{
				pStaticText->AllShowOneLine(true);
				pStaticText->SetWidth(UDim(0,GetPixelSize().x));//恢复宽度			
			}
			
			pStaticText->SetTextHorzAlign(m_hAlign);
			pStaticText->SetTextVertAlign(m_VAlign);
			pStaticText->SetHorizontalAlignment( HA_CENTRE );
			pStaticText->SetVerticalAlignment( VA_CENTRE );
			pStaticText->SetYPosition(UDim(0, -3));

			pBackImage->SetTouchable(false);		
			pBackImage->SetImage(m_strItemBackImageName);
			pBackImage->SetSize(UVector2(UDim(0,GetPixelSize().x),UDim(0,pStaticText->GetPixelSize().y)));
			
			if ( bStretched )
			{
				//pBackImage->SetStretched( true );
				//pBackImage->SetStretchOffset(1);
				pBackImage->SetSize(UVector2(UDim(0,GetPixelSize().x),UDim(0,pStaticText->GetPixelSize().y+10)));
			}

			pBackImage->AddChildWindow(pStaticText);	

			AddChildWindow(pBackImage);					

			ElemInfo ei(pBackImage);		
			float h=pBackImage->GetPixelSize().y;		

			ei.maxSlidLen=h;		

			m_vectElemInfo.push_back(ei);

			//update other ElemInfos			
			for (int i=(int)m_vectElemInfo.size()-2;i>=0;--i)
			{			
				m_vectElemInfo[i].maxSlidLen=m_vectElemInfo[i+1].maxSlidLen+m_vectElemInfo[i].pWnd->GetPixelSize().y;
				if (!m_bOverlap)//不重叠
				{
					m_vectElemInfo[i].slidLen=LORD::Math::Max(m_vectElemInfo[i].slidLen,m_vectElemInfo[i+1].slidLen+m_vectElemInfo[i+1].pWnd->GetPixelSize().y);
				}
			}

			h=this->GetPixelSize().y+h;

			pBackImage->SetPosition(UVector2(UDim(0,0),UDim(0,h)));		

			itemIndex++;
			if (itemIndex & 0x10000000)
				itemIndex=0;
		}	
	}

	void GUIBubbleContain::AddChild_impl( GUIWindow* wnd )
	{
		// if window is already attached, detach it first (will fire normal events)
		GUIWindow* const old_parent = wnd->GetParent();
		if (old_parent)
			old_parent->RemoveChildWindow(wnd);

		AddWindowToDrawList(*wnd,true);

		// add window to child list
		m_Children.push_back(wnd);

		// set the parent window
		wnd->SetParent(this);

		// update area rects and content for the added window
		wnd->NotifyScreenAreaChanged(true);
		wnd->Invalidate(true);

		if (!old_parent || old_parent->GetPixelSize() != GetPixelSize())
		{
			wnd->OnParentSized();
		}
	}

	void GUIBubbleContain::setItemBackImageName(GUIString strValue)
	{
		if (m_strItemBackImageName!=strValue)
		{
			m_strItemBackImageName=strValue;
			OnImageChanged();
		}
	}
	void GUIBubbleContain::OnImageChanged()
	{	
		size_t nCount =GetChildCount();
		for (size_t i = 0; i < nCount; ++i)
		{
			GUIWindow* pChild = GetChildByIndex(i);
			if (pChild->GetType() == GWT_STATIC_IMAGE)
			{
				GUIStaticImage* pImage = (GUIStaticImage*)pChild;					
					pImage->SetImage(m_strItemBackImageName);					
			}
		}		
	}	

	void GUIBubbleContain::MoveElemPosition(GUIWindow * wnd,float slidLen)
	{			
		float h = GetPixelSize().y - slidLen;
		
		h=h>0 ? h:0;

		wnd->SetPosition(UVector2(UDim(0,0),UDim(0,h)));
	}

	void GUIBubbleContain::DrawSelf()
	{		
		// float h=0.0f;
		size_t nCount = m_vectElemInfo.size();
		for (size_t i=0; i<nCount; ++i)
		{
			ElemInfo & ei = m_vectElemInfo[i];
			switch(ei.status)
			{
			case MOVE:
				MoveElemPosition(ei.pWnd,ei.slidLen);
				break;
			case STAY_MOVE:
				MoveElemPosition(ei.pWnd,ei.slidLen);
				break;
			case STAY_STOP:
				break;
			}	
		}		
	}
	void GUIBubbleContain::UpdateSelf( ui32 nTimeElapse )
	{
		static i32 timePassed = 0;
		
		timePassed += nTimeElapse;	

		if(timePassed > 50 )
		{
			for (size_t i=0; i<m_vectElemInfo.size();)
			{				
				size_t index = m_vectElemInfo.size()-1-i;				

				if (m_vectElemInfo[index].slidLen>=(GetPixelSize().y - m_vectElemInfo[index].pWnd->GetPixelSize().y))
				{
					RemoveChildWindow(m_vectElemInfo[index].pWnd);
					GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
					m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
					continue;
				}				
				if (m_vectElemInfo[index].slidLen>=m_vectElemInfo[index].maxSlidLen)
				{
					m_vectElemInfo[index].status=STAY_STOP;
					m_vectElemInfo[index].slidLen = m_vectElemInfo[index].maxSlidLen;
					if (index==0)
					{
						if (m_vectElemInfo[index].stayTime>=m_stayTime && m_stayTime != 0)
						{
							RemoveChildWindow(m_vectElemInfo[index].pWnd);
							GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
							m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
							continue;
						}						
						else if ((index+1)<m_vectElemInfo.size() && m_vectElemInfo[index].slidLen <
							m_vectElemInfo[index+1].slidLen +m_vectElemInfo[index+1].pWnd->GetPixelSize().y)
						{
							RemoveChildWindow(m_vectElemInfo[index].pWnd);
							GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
							m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
							continue;
						}
						else
							m_vectElemInfo[index].stayTime+=timePassed;
					}
					else
						m_vectElemInfo[index].stayTime+=(timePassed>>4);
					
				}
				else
				{
					if ( m_vectElemInfo[index].slidLen + m_goStep >= m_vectElemInfo[index].maxSlidLen )
					{
						m_vectElemInfo[index].slidLen = m_vectElemInfo[index].maxSlidLen;
					}
					else
					{
						m_vectElemInfo[index].slidLen+=m_goStep;
					}
					
					m_vectElemInfo[index].status=MOVE;
				}
				
				++i;
			}
			timePassed = 0;
			Invalidate();
		}
	}

		/*
		if (timePassed > 100 )//方法二效果
		{	
			float len_offset=0.0f , elemH=0.0f , gapLen=0.0f ,flag=0.0f;

			size_t index = 0 ;			

			for (size_t i=0; i<m_vectElemInfo.size();)
			{
				index = m_vectElemInfo.size()-1-i;

				len_offset += m_goStep;

				elemH=m_vectElemInfo[index].pWnd->GetPixelSize().y;

				if (index+1<m_vectElemInfo.size())//if next elem is not end
				{
					gapLen=m_vectElemInfo[index].slidLen-m_vectElemInfo[index+1].slidLen;

					if (m_vectElemInfo[index].slidLen>=m_vectElemInfo[index].maxSlidLen)// beyond maxSlidlen
					{
						if (elemH>gapLen)  // overlapped
						{
							elemH -= gapLen;
							m_vectElemInfo[index].slidLen += (len_offset>elemH?elemH:len_offset);
							m_vectElemInfo[index].status = STAY_MOVE;	
							flag=0.1f;							
						}
						else   // not overlapped
						{
							m_vectElemInfo[index].status=STAY_STOP; 
							flag=1.0f;							
						}
						if (index==0)// the top elem
						{
							if (m_vectElemInfo[index].stayTime>=m_stayTime)
							{	
								RemoveChildWindow(m_vectElemInfo[index].pWnd);
								m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
								continue;
							}
							else
								m_vectElemInfo[index].stayTime += flag*timePassed;
						}
					}
					else  // low  maxSliden
					{
						m_vectElemInfo[index].status = MOVE;									
						if (elemH>gapLen)// overlapped
						{
							elemH -= gapLen;
							m_vectElemInfo[index].slidLen += (len_offset>elemH?elemH:len_offset);							
						}
						else
							m_vectElemInfo[index].slidLen += len_offset;					
						
					}
				}
				else// the next elem is end
				{
					if (m_vectElemInfo[index].slidLen>=m_vectElemInfo[index].maxSlidLen)// beyond maxSlidlen
					{						
						m_vectElemInfo[index].status=STAY_STOP; 
						if (index==0)// the top elem
						{
							if (m_vectElemInfo[index].stayTime>=m_stayTime)
							{	
								RemoveChildWindow(m_vectElemInfo[index].pWnd);
								m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
								continue;
							}
							else
								m_vectElemInfo[index].stayTime += timePassed;
						}						
					}
					else
					{
						m_vectElemInfo[index].status = MOVE;
						m_vectElemInfo[index].slidLen += len_offset;	
					}
				}				
				++i;
			}
			timePassed = 0;
			Invalidate();
		}
	}*/
	GUIBubbleContainEx::GUIBubbleContainEx( GUIWindowType nType, const GUIString& strName)
		:GUIWindow(nType, strName)	
		,m_strItemBackImageName("")//set:task.xml image:04
		,m_hAlign(HA_LEFT)
		,m_VAlign(VA_TOP)
		,m_wordWrap(true)	
		,m_bOverlap(true)
	{
		SetClipChild(true);
		setSpeedAndStayTime();
	}

	GUIBubbleContainEx::~GUIBubbleContainEx(void)
	{
	}

	void GUIBubbleContainEx::setSpeedAndStayTime(float speed, i32 stayTime)
	{
		m_goStep=speed;
		m_stayTime=stayTime;
	}

	bool GUIBubbleContainEx::SetProperty( const GUIString& strName, const GUIString& strValue )
	{	 
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			setItemBackImageName(strValue);
			return true;
		}	
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{			
			if (strValue == "Centre")
			{
				m_hAlign = HA_CENTRE;
			}
			else if (strValue == "Right")
			{
				m_hAlign = HA_RIGHT;
			}
			else
			{
				m_hAlign = HA_LEFT;
			}	
			if (m_hAlign!=HA_LEFT)			
				m_wordWrap=false;
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{			
			if (strValue == "Centre")
			{
				m_VAlign = VA_CENTRE;
			}
			else if (strValue == "Bottom")
			{
				m_VAlign = VA_BOTTOM;
			}
			else
			{
				m_VAlign = VA_TOP;
			}	
			if (m_VAlign!=VA_TOP)			
				m_wordWrap=false;
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			m_wordWrap=StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		return false;
	}

	bool GUIBubbleContainEx::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			strValue = m_strItemBackImageName;
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			switch(m_hAlign)
			{
			case HA_CENTRE:
				strValue = "Centre";
				break;

			case HA_RIGHT:
				strValue = "Right";
				break;

			default:
				strValue = "Left";
			}
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			switch(m_VAlign)
			{
			case VA_CENTRE:
				strValue = "Centre";
				break;

			case VA_BOTTOM:
				strValue = "Bottom";
				break;

			default:
				strValue = "Top";
			}
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			strValue = StringUtil::ToString(m_wordWrap).c_str();
			return true;
		}
		return false;
	}

	void GUIBubbleContainEx::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_BUBBLE_ITEM_BACK_IMAGE);
		properlist.push_back(GWP_TEXT_HORZ_ALIGN);
		properlist.push_back(GWP_TEXT_VERT_ALIGN);
		properlist.push_back(GWP_TEXT_WORDWRAP);
	}

	bool GUIBubbleContainEx::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_BUBBLE_ITEM_BACK_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			type = GPT_HA;
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			type = GPT_VA;
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			type = GPT_BOOL;
			return true;
		}
		return false;
	}

	void GUIBubbleContainEx::addItem(GUIString strValue,GUIString& strTemplate,GUIString& strText, bool bStretched /* = false */)
	{		
		static ui32 itemIndexEx =0;

		String _strWinName = StringUtil::Format("%s_Bubb_BackEx_%d",strValue.substr(0,30).c_str(),itemIndexEx);
		GUIWindow *_pWin = GUIWindowManager::Instance()->CreateWindowFromTemplate(_strWinName.c_str(),strTemplate);

		String _strTextName = StringUtil::Format("%s_%s",_strWinName.c_str(),strText.c_str());
		GUIStaticText* _pText = (GUIStaticText*)LORD::GUIWindowManager::Instance()->GetGUIWindow(_strTextName.c_str());

		if ( _pWin && _pText )
		{	
			_pText->SetText(strValue.c_str());

			AddChildWindow(_pWin);					

			ElemInfo ei(_pWin);		
			float h= _pWin->GetPixelSize().y;		

			ei.maxSlidLen=h;		

			m_vectElemInfo.push_back(ei);

			//update other ElemInfos			
			for (int i=(int)m_vectElemInfo.size()-2;i>=0;--i)
			{			
				m_vectElemInfo[i].maxSlidLen=m_vectElemInfo[i+1].maxSlidLen+m_vectElemInfo[i].pWnd->GetPixelSize().y;
				if (!m_bOverlap)//不重叠
				{
					m_vectElemInfo[i].slidLen=LORD::Math::Max(m_vectElemInfo[i].slidLen,m_vectElemInfo[i+1].slidLen+m_vectElemInfo[i+1].pWnd->GetPixelSize().y);
				}
			}

			h=this->GetPixelSize().y+h;

			_pWin->SetPosition(UVector2(UDim(0,0),UDim(0,h)));		

			itemIndexEx++;
			if (itemIndexEx & 0x10000000)
				itemIndexEx=0;
		}	
	}

	void GUIBubbleContainEx::AddChild_impl( GUIWindow* wnd )
	{
		// if window is already attached, detach it first (will fire normal events)
		GUIWindow* const old_parent = wnd->GetParent();
		if (old_parent)
			old_parent->RemoveChildWindow(wnd);

		AddWindowToDrawList(*wnd,true);

		// add window to child list
		m_Children.push_back(wnd);

		// set the parent window
		wnd->SetParent(this);

		// update area rects and content for the added window
		wnd->NotifyScreenAreaChanged(true);
		wnd->Invalidate(true);

		if (!old_parent || old_parent->GetPixelSize() != GetPixelSize())
		{
			wnd->OnParentSized();
		}
	}

	void GUIBubbleContainEx::setItemBackImageName(GUIString strValue)
	{
		if (m_strItemBackImageName!=strValue)
		{
			m_strItemBackImageName=strValue;
			OnImageChanged();
		}
	}
	void GUIBubbleContainEx::OnImageChanged()
	{	
		size_t nCount =GetChildCount();
		for (size_t i = 0; i < nCount; ++i)
		{
			GUIWindow* pChild = GetChildByIndex(i);
			if (pChild->GetType() == GWT_STATIC_IMAGE)
			{
				GUIStaticImage* pImage = (GUIStaticImage*)pChild;					
				pImage->SetImage(m_strItemBackImageName);					
			}
		}		
	}	

	void GUIBubbleContainEx::MoveElemPosition(GUIWindow * wnd,float slidLen)
	{			
		float h = GetPixelSize().y - slidLen;

		h=h>0 ? h:0;

		wnd->SetPosition(UVector2(UDim(0,0),UDim(0,h)));
	}

	void GUIBubbleContainEx::DrawSelf()
	{		
		// float h=0.0f;
		size_t nCount = m_vectElemInfo.size();
		for (size_t i=0; i<nCount; ++i)
		{
			ElemInfo & ei = m_vectElemInfo[i];
			switch(ei.status)
			{
			case MOVE:
				MoveElemPosition(ei.pWnd,ei.slidLen);
				break;
			case STAY_MOVE:
				MoveElemPosition(ei.pWnd,ei.slidLen);
				break;
			case STAY_STOP:
				break;
			}	
		}		
	}
	void GUIBubbleContainEx::UpdateSelf( ui32 nTimeElapse )
	{
		static i32 timePassed = 0;

		timePassed += nTimeElapse;	

		if(timePassed > 50 )
		{
			for (size_t i=0; i<m_vectElemInfo.size();)
			{				
				size_t index = m_vectElemInfo.size()-1-i;				

				if (m_vectElemInfo[index].slidLen>=(GetPixelSize().y - m_vectElemInfo[index].pWnd->GetPixelSize().y))
				{
					RemoveChildWindow(m_vectElemInfo[index].pWnd);
					GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
					m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
					continue;
				}				
				if (m_vectElemInfo[index].slidLen>=m_vectElemInfo[index].maxSlidLen)
				{
					m_vectElemInfo[index].status=STAY_STOP;
					m_vectElemInfo[index].slidLen = m_vectElemInfo[index].maxSlidLen;
					if (index==0)
					{
						if (m_vectElemInfo[index].stayTime>=m_stayTime && m_stayTime != 0)
						{
							RemoveChildWindow(m_vectElemInfo[index].pWnd);
							GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
							m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
							continue;
						}						
						else if ((index+1)<m_vectElemInfo.size() && m_vectElemInfo[index].slidLen <
							m_vectElemInfo[index+1].slidLen +m_vectElemInfo[index+1].pWnd->GetPixelSize().y)
						{
							RemoveChildWindow(m_vectElemInfo[index].pWnd);
							GUIWindowManager::Instance()->DestroyGUIWindow(m_vectElemInfo[index].pWnd);
							m_vectElemInfo.erase(m_vectElemInfo.begin()+index);										
							continue;
						}
						else
							m_vectElemInfo[index].stayTime+=timePassed;
					}
					else
						m_vectElemInfo[index].stayTime+=(timePassed>>4);

				}
				else
				{
					if ( m_vectElemInfo[index].slidLen + m_goStep >= m_vectElemInfo[index].maxSlidLen )
					{
						m_vectElemInfo[index].slidLen = m_vectElemInfo[index].maxSlidLen;
					}
					else
					{
						m_vectElemInfo[index].slidLen+=m_goStep;
					}

					m_vectElemInfo[index].status=MOVE;
				}

				++i;
			}
			timePassed = 0;
			Invalidate();
		}
	}
}
