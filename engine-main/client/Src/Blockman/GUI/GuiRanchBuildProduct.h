#pragma once
#ifndef __GUI_RANCH_BUILD_PRODUCT_HEADER__
#define __GUI_RANCH_BUILD_PRODUCT_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "Setting/RanchSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildProduct : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		RanchCommon m_product;

		i32 m_index = 0;
		i32 m_entityId = 0;

	private:
		void removeComponents();

	public:
		GuiRanchBuildProduct(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildProduct() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setProduct(RanchCommon product);
		void setIndex(i32 index) { m_index = index; }
		void setEntityId(i32 entityId) { m_entityId = entityId; }

	private:
		bool onClick(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
