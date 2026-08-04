#pragma once
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiCommonTeamInfo : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;

		GUIStaticImage* m_teamBg = nullptr;
		GUIStaticImage* m_teamIcon = nullptr;
		GUIStaticText* m_teamInfo = nullptr;
		GUIStaticText* m_teamInfoNum = nullptr;
		bool   m_isRefreshIcon = true;

		void removeComponents();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiCommonTeamInfo(const GUIWindowType& nType, const GUIString& strName);
		~GuiCommonTeamInfo() = default;

		void SetIcon(String& icon);
		void ShowContent(String& icon, String& textMsg,  String& textNumMsg, bool showBg = false);
		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		float getWindowWidth() { return m_window->GetPixelSize().x; }
		void  SetRefreshIconStatus(bool status) { m_isRefreshIcon = status; }
	};
}