#pragma once
#ifndef __GUI_MANOR_HOUSE_HEADER__
#define __GUI_MANOR_INFO_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include "World/Manor.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manorHouse : public  gui_layout
	{
		enum class ViewId
		{
			BUY,
			BUILD
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_manorHouse();
		virtual ~gui_manorHouse();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onItemClick(ManorHouse* house);
		bool manorOnUpdate();
		bool initItem();
		bool itemSizeChange();

	private:
		bool buildHoues();
		void refreshUI(ui32 timeElapse);
		bool resetBuildResidueCoolTime() {
			m_buildResidueCoolTime = 0;
			return true;
		}

	private:
		float m_gridViewPixelSizeX = 0.f;
		list<ManorHouse*>::type m_manorHouses;
		GUIGridView* m_itemGridView = nullptr;
		GUIButton* m_btnOperation = nullptr;

		ManorHouse* m_selectHouse = nullptr;
		int m_buildResidueCoolTime = 0;
		
	};
}

#endif