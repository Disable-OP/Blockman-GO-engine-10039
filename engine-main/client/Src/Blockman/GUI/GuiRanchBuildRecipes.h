#pragma once
#ifndef __GUI_RANCH_BUILD_RECIPES_HEADER__
#define __GUI_RANCH_BUILD_RECIPES_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Entity/EntityBuildNpc.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildRecipes : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIButton* m_ivIcon = nullptr;
		GUIStaticImage* m_ivlock = nullptr;
		GUIStaticText* m_tvLevel = nullptr;
		ProductRecipe m_productRecipe;
		

	private:
		void removeComponents();

	public:
		GuiRanchBuildRecipes(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildRecipes() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setProductRecipe(ProductRecipe productRecipe);

	private:
		bool onClick(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
