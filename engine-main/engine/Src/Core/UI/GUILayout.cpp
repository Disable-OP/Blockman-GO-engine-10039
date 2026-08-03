#include "GUILayout.h"
#include "GUIWindowManager.h"
#include "Object/Root.h"


namespace LORD
{
	GUILayout::GUILayout( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)	
	{
	}

	GUILayout::~GUILayout()
	{
	}	

	bool GUILayout::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LAYOUT_IMAGE_BACK)
		{
			
			SetBackImage(strValue);
			//Invalidate();

			return true;
		}		

		return false;
	}

	bool GUILayout::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LAYOUT_IMAGE_BACK)
		{
			strValue = m_BackImage.GetWholeImageName();
			return true;
		}		

		return false;
	}

	void GUILayout::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_LAYOUT_IMAGE_BACK);		
	}

	bool GUILayout::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_LAYOUT_IMAGE_BACK)
		{
			type = GPT_IMAGE;
			return true;
		}
		
		return false;
	}

	void GUILayout::SetBackImage( const GUIString& strBackImage )
	{			
		m_BackImage.SetWholeImageName(strBackImage);		
	}

	void GUILayout::DrawSelf()
	{		
		ERect rtUnclippedRect = GetRenderArea();
		m_BackImage.DrawStretchImage(this, rtUnclippedRect, GetDrawColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
	}

	void GUILayout::CalcuWndPixelSize(Vector2 & wndPixelSize)
	{
		float nHeight = 0.0f;
		float nWidth =0.0f;  

		size_t nCount = this->GetChildCount();
		
		for (size_t i = 0; i < nCount; ++i)
		{
			GUIWindow* pItem = this->GetChildByIndex(i);
			if (pItem->IsVisible())
			{
				nHeight=nHeight + pItem->GetPixelSize().y;				
				nWidth=nWidth>pItem->GetPixelSize().x ? nWidth : pItem->GetPixelSize().x;
			}			
		}
		wndPixelSize.x=nWidth>this->GetPixelSize().x ? nWidth : this->GetPixelSize().x;
		wndPixelSize.y=nHeight;
	}	

	void GUILayout::LayoutChild()
	{
		Vector2 curPixelSize;
		this->CalcuWndPixelSize(curPixelSize);
		if (m_PixelSize==curPixelSize)
			return;
		
		float nHeight = 0.0f;
		size_t nCount = this->GetChildCount();
		
		for (size_t i = 0; i < nCount; ++i)
		{
			GUIWindow* pItem = this->GetChildByIndex(i);
			if (pItem->IsVisible())
			{				
				//pItem->SetXPosition(UDim(0, 0));//left point
				pItem->SetYPosition(UDim(0, nHeight));//top point
				nHeight += pItem->GetPixelSize().y;
			}			
		}
		this->SetHeight(UDim(0, nHeight));	
		this->SetWidth(UDim(0, curPixelSize.x));		
	}	
}
