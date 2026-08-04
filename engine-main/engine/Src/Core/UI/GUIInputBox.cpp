#include "GUIInputBox.h"

namespace LORD
{
#define MAX_HEIGHT 50
#define SENSIBILITY 5
	GUIInputBox::GUIInputBox(GUIWindowType nType, const GUIString& strName)
	: GUIWindow(nType, strName)
	,m_fcolor(Color::GREEN)
	,m_bcolor(Color::WHITE)
	,m_Start(false)
	,Resetting(false)
	,MoveLenght(0)
	,ResetLenght(0)
	//,len(0)
	,LastLen(0)
	,m_isMoveAble(true)
	{
		m_RenderText.SetFont(GetFont());
		m_RenderText.SetWordWrap(true);
	}

	GUIInputBox::~GUIInputBox()
	{
		ClearList();
	}

	void GUIInputBox::Init()
	{
		m_Rect = GetUnclippedOuterRect();
		m_Rect.bottom-=30;
		m_Rect.top += 20;
		m_height = GetFont()->GetFontHeight();
		m_width = GetFont()->GetFontHeight();
		m_MaxWidth = m_Rect.right - m_Rect.left;
		m_MaxHeight = m_Rect.bottom - m_Rect.top;
		m_MaxNUMWidth = (LORD::ui32)((m_Rect.right - m_Rect.left)/m_width - 1);
		m_MaxNUMHeight = (LORD::ui32)((m_Rect.bottom - m_Rect.top)/m_height);
		m_RenderText.SetWidth(m_Rect.right - m_Rect.left-80);
		//BackGround.SetWholeImageName(_ImgName);
	}
	 
	bool GUIInputBox::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}
		if (strName == GWP_INPUTBOX_BACK_IMAGE)
		{
			SetNormalImage(strValue);
			return true;
		}

		return true;
	}

	bool GUIInputBox::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}
		if (strName == GWP_INPUTBOX_BACK_IMAGE)
		{
			strValue = BackGround.GetWholeImageName();
			return true;
		}
		return false;
	}

	void GUIInputBox::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_INPUTBOX_BACK_IMAGE);

	}

	bool GUIInputBox::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_INPUTBOX_BACK_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		return false;
	}

	void GUIInputBox::SetNormalImage( const GUIString& strNormalImage )
	{
		BackGround.SetWholeImageName(strNormalImage);
	}

	void GUIInputBox::ADDText(const GUIString& text)
	{
		int len1 = (int)text.length();
		if(!m_Start)
		{
			Init();
			m_Start = true;
		}
		if(len1<=0)
			return;
		else
		{
			//LastLen = len;
			Real tempW = len*m_width;////字符串实际在屏幕中的长度
			m_RenderText.SetText(text);
			int height = (int)m_RenderText.GetTextStringHigh();
			m_RenderText.SetText(text);
			len = (LORD::ui32)m_RenderText.GetTextStringHigh();
			//if(tempW<m_MaxWidth)
			//{
				InputBoxType *pstr_ = LordNew InputBoxType;
				if(!InputBoxList.empty())
				{
					list<InputBoxType*>::type::iterator forward_it;
					for(forward_it = InputBoxList.begin();forward_it!=InputBoxList.end();++forward_it)
					{
						(*forward_it)->y -= height;//*(len);
					}
					pstr_->x = InputBoxList.back()->x;
					pstr_->y = InputBoxList.back()->y+m_height;//*(LastLen);
					m_height = Real(height);
						//pstr_->y = InputBoxList.back()->y-m_height*(LastLen);
				}
				else
				{
					pstr_->x =  m_Rect.left+30;
					pstr_->y =  m_Rect.bottom - m_height;
					m_height = Real(height);
				}
				pstr_->name = text;
				InputBoxList.push_back(pstr_);
				if(InputBoxList.size()>MAX_HEIGHT)////最多只保存MAX_HEIGHT超出的话删除第一个
				{
					LordDelete InputBoxList.front();
					InputBoxList.pop_front();
				}
			//}
			/*else
			{
				ui32 tempRow = tempW/m_MaxWidth + 1;///算出多少该字符占用多少行

				//InputBoxType *pstr = LordNewArray(InputBoxType,tempRow);
				for(int i=0;i<tempRow;++i)
				{
					InputBoxType *pstr = LordNew InputBoxType;
					if(!InputBoxList.empty())
					{
						pstr->x = InputBoxList.back()->x;
						pstr->y = InputBoxList.back()->y;
						list<InputBoxType*>::type::iterator forward_it;
						for(forward_it = InputBoxList.begin();forward_it!=InputBoxList.end();++forward_it)
						{
							(*forward_it)->y -= m_height;
						}
					}
					else
					{					
						pstr->x =  m_Rect.left+30;
						pstr->y =  m_Rect.bottom;
					}
					pstr->name = text.substr(i*m_MaxNUMWidth,m_MaxNUMWidth);
					InputBoxList.push_back(pstr);
					if(InputBoxList.size()>MAX_HEIGHT)////最多只保存MAX_HEIGHT超出的话删除第一个
					{
						InputBoxType* temp = *InputBoxList.begin();
						InputBoxList.pop_front();
						LordDelete temp;
					}
				}
			}*/
			
		}
	}

	void GUIInputBox::DrawSelf()
	{
		ERect rtOuterRect(0,0,1280,720);
		LORD::Color col(0.0f,0.0f,0.0f,1.0f);
		LORD::Vector2 vec(1,1);
		BackGround.DrawImage(this, GetRenderArea(), getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
		list<InputBoxType*>::type::iterator it;
		if(InputBoxList.size() <= 0)
		{
			return;
		}
		for(it = InputBoxList.begin();it!=InputBoxList.end();++it)
		{
			if((*it)->y >= m_Rect.top && (*it)->y <= m_Rect.bottom)
			{
				//GetFont()->RenderText((*it)->name,LORD::Vector2((*it)->x,(*it)->y),GetRenderArea(),m_fcolor,false,m_bcolor,vec);
				m_RenderText.SetText((*it)->name);
				LORD::ERect rect_Temp;
				rect_Temp.left = (*it)->x;rect_Temp.right = m_Rect.right;rect_Temp.top = (*it)->y;rect_Temp.bottom = m_Rect.bottom;
				m_RenderText.DrawGUIText(rect_Temp,this);
			}
		}
		if(Resetting)
		{
			if(InputBoxList.back()->y < (m_Rect.bottom)&&InputBoxList.size()>m_MaxNUMHeight)
			{
				list<InputBoxType*>::type::reverse_iterator reverse_it;
				for(reverse_it = InputBoxList.rbegin();reverse_it!=InputBoxList.rend();++reverse_it)
				{
					(*reverse_it)->y += ResetLenght;
					if(InputBoxList.back()->y>(m_Rect.bottom))
					{	
						InputBoxList.back()->y -= ResetLenght;
						ResetLenght = m_Rect.bottom - InputBoxList.back()->y;
						InputBoxList.back()->y += ResetLenght;
					}
				}
			}
			else if(InputBoxList.front()->y>m_Rect.top&&InputBoxList.size()>m_MaxNUMHeight)
			{
				list<InputBoxType*>::type::iterator forward_it;
				for(forward_it = InputBoxList.begin();forward_it!=InputBoxList.end();++forward_it)
				{
					(*forward_it)->y -= ResetLenght;
					if(InputBoxList.front()->y<m_Rect.top)
					{	
						InputBoxList.front()->y += ResetLenght;
						ResetLenght = InputBoxList.front()->y - m_Rect.top;
						InputBoxList.front()->y -= ResetLenght;
					}
				}
			}
			else
			{
				Resetting = false;
			}
		}
	}

	bool GUIInputBox::TouchDown(const LORD::Vector2& position)
	{
		GUIWindow::TouchDown( position );

		m_Postion = position.y;
		Resetting = false;
		return true;
	}


	bool GUIInputBox::TouchMove(const LORD::Vector2& position)
	{
		if (!m_isMoveAble)
			return true;
		if(position.y >= m_Rect.top && position.y <= m_Rect.bottom&&position.x>=m_Rect.left&&position.x<=m_Rect.right)
		{
		
		}
		else
		{
			if(!InputBoxList.empty())
			{
				if(InputBoxList.back()->y < (m_Rect.bottom))
				{
					ResetLenght = (m_Rect.bottom - InputBoxList.back()->y)/15.0f;
				}
				else if(InputBoxList.front()->y>m_Rect.top)
				{
					ResetLenght = (InputBoxList.front()->y - m_Rect.top)/15.0f;
				}
				MoveLenght = 0;
				m_Postion = 0;
				Resetting = true;
			}
			return false;
		}
		if(!InputBoxList.empty()&&InputBoxList.size()>m_MaxNUMHeight)
		{
			list<InputBoxType*>::type::reverse_iterator reverse_it;
			list<InputBoxType*>::type::iterator forward_it;
			MoveLenght = m_Postion-position.y;
			if(MoveLenght<0)/////字体整体下移动
			{
				for(reverse_it = InputBoxList.rbegin();reverse_it!=InputBoxList.rend();++reverse_it)
				{
					(*reverse_it)->y -= MoveLenght;
				}
			}
			else/////字体整体上移动
			{
				for(forward_it = InputBoxList.begin();forward_it!=InputBoxList.end();++forward_it)
				{
					(*forward_it)->y -=  MoveLenght;
				}
			}
		}
		m_Postion = position.y;
		return true;
	}

	bool GUIInputBox::TouchUp(const LORD::Vector2& position)
	{
		if(!InputBoxList.empty())
		{
			if(InputBoxList.back()->y < (m_Rect.bottom))
			{
				ResetLenght = (m_Rect.bottom - InputBoxList.back()->y)/15.0f;		
			}
			else if(InputBoxList.front()->y>m_Rect.top)
			{
				ResetLenght = (InputBoxList.front()->y - m_Rect.top)/15.0f;
			}
			MoveLenght = 0;
			m_Postion = 0;
			Resetting = true;
		}
		return true;
	}

	void GUIInputBox::ClearList()
	{
		list<InputBoxType*>::type::iterator it;
		if ( InputBoxList.size() <= 0 )
		{
			return;
		}
		for(it = InputBoxList.begin();it!=InputBoxList.end();++it)
		{
			LordDelete (*it);
		}
		InputBoxList.clear();
	}
}
