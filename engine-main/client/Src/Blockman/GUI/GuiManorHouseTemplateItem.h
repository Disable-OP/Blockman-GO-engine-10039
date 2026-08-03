#pragma once
#ifndef  __GUI_MANOR_HOUSE_TEMPLATE_ITEM_HEADER__
#define __GUI_MANOR_HOUSE_TEMPLATE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "World/Manor.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiManorHouseTemplateItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_templateIcon = nullptr;
		GUIStaticImage* m_templateLockIcon = nullptr;
		GUIStaticImage* m_templateCanUnlockIcon = nullptr;
		ManorHouse* m_manorHouse = nullptr;

	private:
		void removeComponents();

	public:
		GuiManorHouseTemplateItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiManorHouseTemplateItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs & events);
		void setManorHouse(ManorHouse* house) { this->m_manorHouse = house; }

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
