#pragma once
#ifndef __GUI_BIRD_ATLAS_ITEM_HEADER__
#define __GUI_BIRD_ATLAS_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{

	class BirdEgg;
	class BirdAtlas;

	class GuiBirdAtlasItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_ivTableIcon = nullptr;
		GUIListHorizontalBox* m_itemList = nullptr;
		float m_listWidth = 0.f;

	private:
		void removeComponents();

	public:
		GuiBirdAtlasItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdAtlasItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setAtlas(const BirdAtlas & table);
	private:
		void setItems(const std::vector<BirdEgg>& items);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
