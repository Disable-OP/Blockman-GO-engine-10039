#include "GuiRanchStorageItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	
	void GuiRanchStorageItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchStorageItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchStorageItem::GuiRanchStorageItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchStorageItem.json"));
		renameComponents(m_window);
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_icon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchStorageItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchStorageItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiRanchStorageItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiRanchStorageItem::onClick(const EventArgs & args)
	{
		ShowRanchSellItemEvent::emit(GetPosition(), m_tanchCommon);
		return true;
	}

	void GuiRanchStorageItem::setRanchItem(const RanchCommon & item)
	{
		m_tanchCommon = item;
		auto ranchItem = RanchSetting::getRanchItem(m_tanchCommon.itemId);
		setIcon(ranchItem ? ranchItem->icon.c_str() : "");
		setNum(StringUtil::ToString(m_tanchCommon.num).c_str());
	}

	void GuiRanchStorageItem::setNum(const GUIString & strNum)
	{
		if (m_tvNum)
		{
			m_tvNum->SetText(strNum);
		}
	}

	void GuiRanchStorageItem::setIcon(const GUIString& strIconImage)
	{
		m_iconImage = strIconImage.c_str();
		if (m_icon)
		{
			m_icon->SetImage(m_iconImage);
		}
	}

	void GuiRanchStorageItem::setIconArea(const URect & area)
	{
		if (m_icon)
		{
			m_icon->SetArea(area);
		}
	}


	bool GuiRanchStorageItem::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (strName == GWP_WINDOW_TEXT)
		{
			setNum(strValue);
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

	bool GuiRanchStorageItem::GetProperty(const GUIString& strName, GUIString& strValue)
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

	void GuiRanchStorageItem::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
	}

	bool GuiRanchStorageItem::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
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