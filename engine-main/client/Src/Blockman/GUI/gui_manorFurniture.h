#pragma once
#ifndef __GUI_MANOR_FURNITURE_HEADER__
#define __GUI_MANOR_FURNITURE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIGridView.h"
#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manorFurniture : public  gui_layout
	{

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_manorFurniture();
		virtual ~gui_manorFurniture();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }
		bool onCheck(const EventArgs & events);
		bool manorOnUpdate();
		bool initItem();
		bool itemSizeChange();
		
	private:
		float m_gridViewPixelSizeX = 0.f;
		GUICheckBox* m_cbEditFurniture = nullptr;
		GUIGridView* m_itemGridView = nullptr;
		list<ManorFurniture*>::type m_manorFurnitures;
	};
}

#endif