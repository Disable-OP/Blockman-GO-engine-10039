#pragma once
#ifndef __GUI_RANCH_SELL_ITEM_HEADER__
#define __GUI_RANCH_SELL_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchSellItem : public CustomGuiWindow
	{
	public:
		enum class VIewId
		{
			BTN_SUB,
			BTN_ADD,
			BTN_SURE
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_desc = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIButton* m_btnSub = nullptr;
		GUIButton* m_btnAdd = nullptr;

	private:
		void removeComponents();

	private:
		i32 m_num = 0;
		RanchCommon m_commonItem;

	public:
		GuiRanchSellItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchSellItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setItem(const RanchCommon & item);
		bool onClick(const EventArgs& args, VIewId viewId);
		void setBack(bool isLeft, bool isTop);
		
	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
