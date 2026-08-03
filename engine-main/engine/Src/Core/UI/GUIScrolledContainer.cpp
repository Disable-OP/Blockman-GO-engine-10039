#include "GUIScrolledContainer.h"

namespace LORD
{
	GUIScrolledContainer::GUIScrolledContainer( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)	
	{
	}

	GUIScrolledContainer::~GUIScrolledContainer()
	{
	}

	bool GUIScrolledContainer::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_BTN_TEXT_COLOR)
		//{
		//	SetTextColor(StringUtil::ParseColor(strValue.c_str()));			
		//	return true;
		//}

		return false;
	}

	bool GUIScrolledContainer::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_BTN_TEXT_COLOR)
		//{
		//	strValue = StringUtil::ToString(m_RenderText.GetTextColor()).c_str();
		//	return true;
		//}

		return false;
	}

	void GUIScrolledContainer::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		//properlist.push_back(GWP_BTN_TEXT_COLOR);
	
	}

	bool GUIScrolledContainer::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		//if (strName == GWP_BTN_TEXT_COLOR)
		//{
		//	type = GPT_COLOR;
		//	return true;
		//}

		return false;
	}

	void GUIScrolledContainer::OnChildAdd()
	{
		OnResizeContainer();
	}

	void GUIScrolledContainer::OnChildRemove()
	{
		OnResizeContainer();
	}

	void GUIScrolledContainer::OnResizeContainer()
	{
		ERect extents(0, 0, 0, 0);

		const size_t childCount = GetChildCount();

		for (size_t i = 0; i < childCount; ++i)
		{
			const GUIWindow* const wnd = GetChildByIndex(i);
			ERect area(wnd->GetArea().asAbsolute(m_PixelSize));

			if (area.left < extents.left)
				extents.left = area.left;

			if (area.top < extents.top)
				extents.top = area.top;

			if (area.right > extents.right)
				extents.right = area.right;

			if (area.bottom > extents.bottom)
				extents.bottom = area.bottom;
		}
			
		SetArea(URect(UDim(0, extents.left), UDim(0, extents.top), UDim(0, extents.right), UDim(0, extents.bottom)));
	}

}
