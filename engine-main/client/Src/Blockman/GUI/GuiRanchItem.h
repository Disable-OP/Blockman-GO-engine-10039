#pragma once
#ifndef __GUI_RANCH_ITEM_HEADER__
#define __GUI_RANCH_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "GUI/GuiUrlImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GuiUrlImage* m_helperIcon = nullptr;

		GUIStaticText* m_tvName = nullptr;
		GUIStaticText* m_tvDesc = nullptr;

		GUIString m_iconImage = "";
		String m_num = "";
		i32 m_itemId = 0;

	private:
		void removeComponents();
		void init(const String & jsonName);

	public:
		GuiRanchItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchItem() = default;
		void changeInfoUi();
		void initOrderReward();
		void resetWindow(const String & jsonName);
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setItemId(i32 itemId);
		void setNum(String strNum);
		void setIcon(const GUIString& strIconImage);

		void setName(String name);
		void setDesc(String desc);
		void setHelper(i64 userId);
		void setTextScale(float scale);
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
