#pragma once
#ifndef __GUI_BIRD_PACKANDFUSE_HEADER__
#define __GUI_BIRD_PACKANDFUSE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIRadioButton.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticImage.h"
#include "World/BirdSimulator.h"
#include "GUI/GuiBirdPack.h"
#include "GUI/GuiBirdFuse.h"
#include "UI/GUISlider.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdPackAndFuse : public  gui_layout
	{
	private:
		enum class ViewId {
			MAIN,
			PACK,
			FUSE,
			QUALITY,
			LEVEL,
			CLOSE,
			CARRAY,
			CAPACITY,
			GRID_VIEW,
			SOLL_BAR,
			HIDE_VIEW
		};

	public:
		gui_birdPackAndFuse();
		virtual ~gui_birdPackAndFuse();
		void onHide() override;
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onPackUpdate();
		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onRadioSortChange(const EventArgs & events, ViewId viewId);
		bool onClick(const EventArgs & events, ViewId viewId);
		bool openExpandCarry();
		void setShowPackOrFuse(bool isPack);
		bool isShowPack();
		bool onShowBirdInfo(i64 birdId, bool isRefresh = true);

	private:
		void sortItemSizeChange();

		void initPackItem();
		void initFuseItem();
		void changeSortBirdItem();
		bool onFuseRemoveItem(i64 birdId);
		bool onFusePutItem(i64 birdId, const String& image);
		bool onViewSoll(const EventArgs&, ViewId viewId);

	private:
		bool m_isPackUpdate = false;

		float m_gvSortXSize = 0.f;

		SubscriptionGuard m_subscriptionGuard;
		map<ViewId, GUIWindow*>::type m_radioContentMap;
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		map<ViewId, GUIRadioButton*>::type m_radioSortMap;
		GUIGridView* m_gvSort = nullptr;
		GUISlider* m_sollBar = nullptr;

		GUILayout* m_pack = nullptr;
		GUILayout* m_fuse = nullptr;
		GUILayout* m_carryTab = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GuiBirdPack* m_birdPackView = nullptr;
		GuiBirdFuse* m_birdFuseView = nullptr;

		BirdInfo m_birdInfo;
	};
}

#endif