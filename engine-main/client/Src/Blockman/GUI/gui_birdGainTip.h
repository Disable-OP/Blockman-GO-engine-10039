#pragma once
#ifndef __GUI_BIRD_GAIN_TIP_HEADER__
#define __GUI_BIRD_GAIN_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListHorizontalBox.h"
#include "GUI/GuiActorWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class BirdGain;
	class BirdInfo;

	class gui_birdGainTip : public  gui_layout
	{

	public:
		gui_birdGainTip();
		virtual ~gui_birdGainTip();
		void onHide() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool showTip(const vector<BirdGain>::type& items);
		bool showLotteryTip(const String& eggActorName, const String& eggActorBodyId, const BirdInfo& birdInfo);
		void initItem(const vector<BirdGain>::type& items);

	private:
		i32 m_openAnimationTime = 0;
		i32 m_shakeAnimationTime = 0;
		GUILayout* m_commonTip = nullptr;
		GUILayout* m_lotteryTip = nullptr;
		GuiActorWindow* m_actor = nullptr;
		GuiActorWindow* m_birdActor = nullptr;
		GUIListHorizontalBox * m_listView = nullptr;
		const float m_showTime = 2000.f;
		float m_leftTime = 0;
	};
}

#endif