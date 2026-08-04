#pragma once
#ifndef __GUI_RANCH_FACTORY_HEADER__
#define __GUI_RANCH_FACTORY_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIListHorizontalBox.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchFactory : public  gui_layout
	{
	private:
		enum class ViewId {

		};

	public:
		gui_ranchFactory();
		virtual ~gui_ranchFactory();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onRanchFactoryChange();
	private:
		void initItem();

	private:
		bool m_dataChange = false;
		SubscriptionGuard m_subscriptionGuard;
		GUIListHorizontalBox * m_lvList = nullptr;
	};
}

#endif