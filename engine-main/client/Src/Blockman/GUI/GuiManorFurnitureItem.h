#pragma once
#ifndef __GUI_MANOR_FURNITURE_ITEM_HEADER__
#define __GUI_MANOR_FURNITURE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "World/Manor.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiManorFurnitureItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_contentIcon = nullptr;
		GUILayout* m_priceInfo = nullptr;
		GUIStaticImage* m_furnitureIcon = nullptr;
		GUIButton* m_btnOperation = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		GUIStaticText* m_priceValue = nullptr;
		ManorFurniture* m_manorFurniture = nullptr;

	private:
		void removeComponents();

	public:
		GuiManorFurnitureItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiManorFurnitureItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs & events);
		void setManorFurniture(ManorFurniture* furniture) { this->m_manorFurniture = furniture; }

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
