#pragma once
#ifndef __GUI_RANCH_BUILD_OPERATION_HEADER__
#define __GUI_RANCH_BUILD_OPERATION_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"
#include "UI/GUIGridView.h"
#include "Entity/EntityBuildNpc.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildOperation : public CustomGuiWindow
	{
	public:
		enum class VIewId
		{
			BTN_SURE,
			BTN_CLOSE,
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvName = nullptr;
		GUIStaticText* m_tvTime = nullptr;
		GUIStaticText* m_itemInventory = nullptr;
		GUIGridView* m_gridView = nullptr;
		GUIButton* m_btnSure = nullptr;
		GUIButton* m_btnClose = nullptr;

	private:
		void removeComponents();

	private:
		i32 m_entityId = 0;
		ProductRecipe m_recipe;

	private:
		void initItem();
		String getNum(i32 itemId, i32 itemNum);
		String timeFormat(i32 needTime);

	public:
		GuiRanchBuildOperation(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildOperation() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, VIewId viewId);
		void setEntityId(i32 entityId);
		void setRecipe(const ProductRecipe& recipe);
	

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
