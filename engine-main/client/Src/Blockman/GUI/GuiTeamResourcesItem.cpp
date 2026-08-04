#include "GuiTeamResourcesItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "GuiItemStack.h"

namespace BLOCKMAN
{
	
	void GuiTeamResourcesItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiTeamResourcesItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiTeamResourcesItem::GuiTeamResourcesItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("TeamResourcesItem.json"));
		renameComponents(m_window);
		m_teamIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_progressBar = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(1));
		m_progressValue = dynamic_cast<GUIStaticText*>(m_progressBar->GetChildByIndex(0));
		refreshUI();
		AddWindowToDrawList(*m_window);
	}

	void GuiTeamResourcesItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiTeamResourcesItem::setTeamResourcesItem(TeamResourcesItem item)
	{
		m_teamResources = item;
		refreshUI();
	}

	void GuiTeamResourcesItem::refreshUI()
	{
		if (m_teamIcon)
		{
			m_teamIcon->SetImage(m_teamResources.m_teamIcon.c_str());
		}

		if (m_progressValue)
		{
			m_progressValue->SetText(m_teamResources.m_maxValue > 0 ? StringUtil::Format("%d/%d", m_teamResources.m_value, m_teamResources.m_maxValue).c_str() : "");
		}

		if (m_progressBar && m_teamResources.m_maxValue > 0)
		{
			m_progressBar->SetVisible(true);
			float progress = (float)m_teamResources.m_value / (float)m_teamResources.m_maxValue;
			m_progressBar->SetProgress(progress);
			m_progressBar->SetBackImage(m_teamResources.m_progressNor.c_str());
			m_progressBar->SetProgressImage(m_teamResources.m_progressPre.c_str());
		}
		else if (m_progressBar && m_teamResources.m_maxValue <= 0)
		{
			m_progressBar->SetVisible(false);
		}
	}

	void GuiTeamResourcesItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiTeamResourcesItem::setBackImage(const GUIString& strBackImage)
	{
		m_backImage = strBackImage.c_str();
		if (m_window)
		{
			m_window->SetBackImage(m_backImage);
		}
	}


	bool GuiTeamResourcesItem::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LAYOUT_IMAGE_BACK)
		{
			setBackImage(strValue);
			return true;
		}

		return false;
	}

	bool GuiTeamResourcesItem::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LAYOUT_IMAGE_BACK)
		{
			strValue = m_backImage.c_str();
			return true;
		}

		return false;
	}

	void GuiTeamResourcesItem::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_LAYOUT_IMAGE_BACK);
	}

	bool GuiTeamResourcesItem::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
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


}