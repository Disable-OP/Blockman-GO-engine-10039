#include "GuiRanchRankTab.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	
	void GuiRanchRankTab::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchRankTab::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchRankTab::GuiRanchRankTab(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchRankTab.json"));
		renameComponents(m_window);
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_tvRank = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		AddWindowToDrawList(*m_window);
		setRank(30000);
	}

	void GuiRanchRankTab::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiRanchRankTab::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchRankTab::setName(const GUIString & strName)
	{
		if (m_tvName)
		{
			m_tvName->SetText(strName);
		}
	}

	void GuiRanchRankTab::setRank(int rank)
	{
		if (!m_tvRank || !LanguageManager::Instance())
		{
			return;
		}
		if (rank == 0 || rank > 3000)
		{
			m_tvRank->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_RANK_MY_RANK_NOT_RANK).c_str());
		}
		else
		{
			GUIString str = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_RANCH_RANK_MY_RANK).c_str(), rank).c_str();
			m_tvRank->SetText(str);
		}
	}

	void GuiRanchRankTab::setIcon(const GUIString& strIconImage)
	{
		m_iconImage = strIconImage.c_str();
		if (m_icon)
		{
			m_icon->SetImage(m_iconImage);
		}
	}

	void GuiRanchRankTab::setIconArea(const URect & area)
	{
		if (m_icon)
		{
			m_icon->SetArea(area);
		}
	}


	bool GuiRanchRankTab::SetProperty(const GUIString& strName, const GUIString& strValue)
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

	bool GuiRanchRankTab::GetProperty(const GUIString& strName, GUIString& strValue)
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

	void GuiRanchRankTab::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
	}

	bool GuiRanchRankTab::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
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