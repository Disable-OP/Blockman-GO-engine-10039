#pragma once
#ifndef __GUI_RANCH_ORDER_OPERATE_HEADER__
#define __GUI_RANCH_ORDER_OPERATE_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchOrderOperate : public CustomGuiWindow
	{

	private:
		enum class ViewId {
			BTN_HELP,
			BTN_PUT,
			BTN_HELP_PUT
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIStaticText* m_tvValue = nullptr;
		GUIStaticText* m_tvName = nullptr;

		RanchOrderInfo	m_orderInfo;
		i32 m_orderId = 0;

	private:
		void removeComponents();

	public:
		GuiRanchOrderOperate(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchOrderOperate() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setOrderId(i32 orderId) { m_orderId = orderId; }
		void setOrderInfo(const RanchOrderInfo& info) { m_orderInfo = info; }
		void chengeUI();

	private:
		bool onClick(const EventArgs& args, ViewId viewId);
		String getNumString(i32 num, i32 needNum);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
