#pragma once
#ifndef __GUI_RANCH_RANK_TAB_HEADER__
#define __GUI_RANCH_RANK_TAB_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchRankTab : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvName= nullptr;
		GUIStaticText* m_tvRank = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIString m_iconImage = "";

	private:
		void removeComponents();

	public:
		GuiRanchRankTab(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchRankTab() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setName(const GUIString& strName);
		void setRank(int rank);
		void setIcon(const GUIString& strIconImage);
		void setIconArea(const URect& area);
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
