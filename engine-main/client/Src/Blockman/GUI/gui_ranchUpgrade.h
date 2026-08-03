#pragma once
#ifndef __GUI_RANCH_UPGRADE_HEADER__
#define __GUI_RANCH_UPGRADE_HEADER__

#include "gui_layout.h"
#include "Setting/RanchSetting.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIButton.h"
#include "GUI/GuiRanchItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchUpgrade : public  gui_layout
	{
	private:
		enum class TipType {
			EXPAND,
			STORAGE_UPGRADE
		};

		enum class ViewId {
			MAIN,
			CLOSE,
			SURE
		};
	public:
		gui_ranchUpgrade();
		virtual ~gui_ranchUpgrade();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showExpand(i32 landCode, i32 price, const vector<RanchCommon>::type& recipe, const vector<RanchCommon>::type& reward);
		bool showStorageUpgrade();
		bool onItemClick(i32 itemId, const String& pos);

	private:
		void sureClick();
		void initRecipeItem(const vector<RanchCommon>::type& items);
		void initRewardItem(const vector<RanchCommon>::type& items);

	private:
		vector<RanchCommon>::type m_recipe = vector<RanchCommon>::type();

		TipType m_tipType = TipType::EXPAND;

		GUIButton * m_btnSure = nullptr;
		GUILayout * m_rewardLayout = nullptr;
		GUILayout * m_currencyLayout = nullptr;
		GUIStaticText * m_tvTitle = nullptr;
		GUIStaticText* m_tvMsg = nullptr;
		GUIListHorizontalBox* m_recipeListView = nullptr;
		GUIListHorizontalBox* m_rewardListView = nullptr;
		GuiRanchItem* m_ranchItemInfo = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		i32 m_leftTime = 0;

		i32 m_landCode = 0;
		i32 m_price = 0;
	};
}

#endif