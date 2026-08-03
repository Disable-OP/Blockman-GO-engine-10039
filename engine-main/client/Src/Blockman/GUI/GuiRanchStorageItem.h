#pragma once
#ifndef __GUI_RANCH_STORAGE_ITEM_HEADER__
#define __GUI_RANCH_STORAGE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Setting/RanchSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchStorageItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIString m_iconImage = "";
		RanchCommon m_tanchCommon;


	private:
		void removeComponents();

	public:
		GuiRanchStorageItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchStorageItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args);
		void setRanchItem(const RanchCommon& item);
		void setNum(const GUIString& strNum);
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
