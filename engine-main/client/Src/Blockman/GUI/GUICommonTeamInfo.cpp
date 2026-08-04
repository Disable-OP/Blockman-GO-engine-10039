#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "GUICommonTeamInfo.h"

using namespace LORD;
namespace BLOCKMAN
{
	vector<GUIWindow*>::type GuiCommonTeamInfo::getPrivateChildren()
	{
		return { m_window };
	}

	GuiCommonTeamInfo::GuiCommonTeamInfo(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("CommonTeamInfo.json");
		renameComponents(m_window);
		m_teamBg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_teamIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_teamInfo = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_teamInfoNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_isRefreshIcon = true;
		AddWindowToDrawList(*m_window);
	}

	void GuiCommonTeamInfo::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiCommonTeamInfo::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiCommonTeamInfo::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_teamBg = nullptr;
		m_teamIcon = nullptr;
		m_teamInfo = nullptr;
		m_teamInfoNum = nullptr;
		m_isRefreshIcon = true;
	}

	void GuiCommonTeamInfo::ShowContent(String& icon,String& textMsg, String& textNumMsg, bool showBg)
	{
		m_teamBg->SetVisible(showBg);
		if (this->m_isRefreshIcon) {
			m_teamIcon->SetImage(icon.c_str());
		}
		m_teamInfo->SetText(textMsg.c_str());
		m_teamInfoNum->SetText(textNumMsg.c_str());
	}

	void GuiCommonTeamInfo::SetIcon(String& icon)
	{
		m_teamIcon->SetImage(icon.c_str());
	}
}
