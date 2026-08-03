#pragma once
#ifndef __GUI_RANCH_ORDER_ITEM_HEADER__
#define __GUI_RANCH_ORDER_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"
#include "GUI/GuiUrlImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchOrderItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_stateIcon = nullptr;
		GUIStaticImage* m_hotIcon = nullptr;
		GUIStaticImage* m_helpIcon = nullptr;
		GuiUrlImage* m_helperIcon = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIStaticImage* m_doneIcon = nullptr;
		GUIStaticText* m_tvValue = nullptr;

		RanchOrderInfo	m_orderInfo;
		i32 m_orderId = 0;

	private:
		void removeComponents();

	public:
		GuiRanchOrderItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchOrderItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setOrderInfo(const RanchOrderInfo& info);
		void setOrderId(i32 orderId) { m_orderId = orderId; }

	private:
		bool onClick(const EventArgs& args);
		String getStateImage();
		String getHelperImage();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
