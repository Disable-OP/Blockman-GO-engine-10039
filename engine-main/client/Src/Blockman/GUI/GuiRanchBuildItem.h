#pragma once
#ifndef __GUI_RANCH_BUILD_ITEM_HEADER__
#define __GUI_RANCH_BUILD_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildItem : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			BTN_BUY,
			BTN_SUB,
			BTN_ADD,
			BTN_INFO
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticImage* m_redPoint = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIButton* m_btnBuy = nullptr;
		GUILayout* m_priceLayout = nullptr;
		GUIStaticImage* m_banBuy = nullptr;
		RanchBuild m_build;

	private:
		void removeComponents();
		

	private:
		i32 m_type = 1;
		i32 m_num = 1;

	public:
		GuiRanchBuildItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildItem() = default;

		void initUI(String json);
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setName(const GUIString& strName);
		void setIcon(const GUIString& strIconImage);
		void setBuild(const RanchBuild& build);
		bool onClick(const EventArgs& args, ViewId viewId);
		void setType(i32 type);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
