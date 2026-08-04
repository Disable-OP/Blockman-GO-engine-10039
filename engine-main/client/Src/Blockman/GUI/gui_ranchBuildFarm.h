#pragma once
#ifndef __GUI_RANCH_BUILD_FARM_HEADER__
#define __GUI_RANCH_BUILD_FARM_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIStaticText.h"
#include "GUI/GuiRanchBuildOperation.h"
#include "Entity/EntityBuildNpc.h"
#include "Setting/RanchSetting.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchBuildFarm : public  gui_layout
	{
	private:
		enum class ViewId {
			MAIN,
			CLOSE,
			BTN_REMOVE,
			UNLOCK,
			BUY_TIME,
		};

	private:
		i32 m_entityId = 0;
		bool m_isSecondClose = false;

	public:
		gui_ranchBuildFarm();
		virtual ~gui_ranchBuildFarm();
		void onLoad() override;
		void onShow() override;
		void onHide() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool showGui(i32 entityId);
		bool onClick(const EventArgs & events, ViewId viewId);

	private:
		bool onDataUpdate(i32 entityId);
		bool onRecipeItemClick(const ProductRecipe& recipe);
		void initProductItem(const vector<RanchCommon>::type& items);
		void initRecipeItem(const vector<ProductRecipe>::type& items);
		void initQueueItem(bool isCanWorking,i32 actorId, const vector<ProductQueue>::type& items);
		void playGameVideo();
	private:

		GUIStaticText * m_title = nullptr;
		GUILayout* m_recipeInfoLayout = nullptr;
		GUILayout* m_recipeLayout = nullptr;
		GuiRanchBuildOperation* m_buildOperation = nullptr;
		GUIListHorizontalBox * m_productItems = nullptr;
		GUIListHorizontalBox * m_recipeItems = nullptr;
		GUIListHorizontalBox* m_queueItems = nullptr;
		GUIButton* m_btnBuyQueue = nullptr;

	};
}

#endif