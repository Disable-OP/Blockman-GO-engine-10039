#pragma once
#ifndef __GUI_RANCH_COMMON_ITEM_HEADER__
#define __GUI_RANCH_COMMON_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchCommonItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticImage* m_done = nullptr;
		GUIButton* m_btnBuy = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvNotReach = nullptr;
		GUIString m_iconImage = "";
		i32 m_itemId = 0;
		RanchCommon m_itemRecipe;

	private:
		void init(const String& jsonName);
		void removeComponents();

	public:
		GuiRanchCommonItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchCommonItem() = default;
		void resetWindow(const String& jsonName);
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		
		i32 getItemId();
		void setNum(i32 num, i32 needNum);
		void setPrice(i32 price);
		void setIcon(const GUIString& strIconImage);
		void setIconArea(const URect& area);
		void setItemRecipe(RanchCommon item);
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;
		bool onClick(const EventArgs & events);
		bool iconClick(const EventArgs & events);

	private:
		String getNumString(i32 num, i32 needNum);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
