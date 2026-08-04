#include "GuiRanchFunctionItem.h"
#include "UI/GUIWindowManager.h"

namespace BLOCKMAN
{
	
	void GuiRanchFunctionItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchFunctionItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchFunctionItem::GuiRanchFunctionItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchFunctionItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_redPoint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_redPoint->SetEffectName("reddot.effect");
		m_redPoint->SetImage("");
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchFunctionItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiRanchFunctionItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchFunctionItem::setRedPointVisible(bool isVisible)
	{
		if (m_redPoint)
		{
			m_redPoint->SetVisible(isVisible);
		}
	}

	void GuiRanchFunctionItem::setName(const GUIString & strName)
	{
		if (m_name)
		{
			m_name->SetText(strName);
		}
	}

	void GuiRanchFunctionItem::setIcon(const GUIString& strIconImage)
	{
		m_iconImage = strIconImage.c_str();
		if (m_icon)
		{
			m_icon->SetImage(m_iconImage);
		}
	}

	void GuiRanchFunctionItem::setIconArea(const URect & area)
	{
		if (m_icon)
		{
			m_icon->SetArea(area);
		}
	}


	bool GuiRanchFunctionItem::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (strName == GWP_WINDOW_TEXT)
		{
			setName(strValue);
		}

		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			setIcon(strValue);
			return true;
		}

		return false;
	}

	bool GuiRanchFunctionItem::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			strValue = m_iconImage.c_str();
			return true;
		}
		return false;
	}

	void GuiRanchFunctionItem::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
	}

	bool GuiRanchFunctionItem::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_SI_IMAGE_NAME)
		{
			type = GPT_IMAGE;
			return true;
		}

		return false;
	}


}