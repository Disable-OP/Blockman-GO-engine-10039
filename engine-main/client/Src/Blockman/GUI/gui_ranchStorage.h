#pragma once
#ifndef __GUI_RANCH_STORAGE_HEADER__
#define __GUI_RANCH_STORAGE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIGridView.h"
#include "Setting/RanchSetting.h"
#include "GUI/GuiRanchSellItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchStorage : public  gui_layout
	{
	private:
		enum class ViewId {
			ALL,
			FARMING,
			MANUFACTURE,
			TOOL,
			MACHINE,
			BUILDING,
			PARTS
		};

		enum Click {
			BTN_SURE,
			BTN_MAIN,
			BTN_REMOVE,
		};

	public:
		gui_ranchStorage();
		virtual ~gui_ranchStorage();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, Click id);
		bool showSellItem(const UVector2& pos, const RanchCommon & item);
		bool onRadioChange(const EventArgs & events,ViewId viewId);
		bool onRanchStorageChange();

	private:
		void initItem(i32 type);
		void itemSizeChange();
		std::vector<RanchCommon> getItemsByType(i32 type);

	private:
		ViewId m_selecdView = ViewId::ALL;
		float m_gridViewPixelSizeX = 0.f;

		bool m_dataChange = false;

		SubscriptionGuard m_subscriptionGuard;

		GUIGridView* m_itemGridView = nullptr;
		GUIStaticText * m_tvCapacityValue = nullptr;
		GuiRanchSellItem* m_viewSellItem = nullptr;
		map<ViewId, GUIString>::type m_langMap;
		map<ViewId, GUIRadioButton*>::type m_radioButtonMap;

	};
}

#endif