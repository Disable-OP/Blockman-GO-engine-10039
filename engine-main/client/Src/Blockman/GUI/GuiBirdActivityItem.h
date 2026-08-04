#pragma once
#ifndef __GUI_BIRD_ACTIVITY_ITEM_HEADER__
#define __GUI_BIRD_ACVITITY_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdActivityItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_num = nullptr;

	private:
		void removeComponents();

	public:
		GuiBirdActivityItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdActivityItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setGiftItem(String icon, i32 num);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
