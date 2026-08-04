#pragma once
#ifndef __GUI_RANCH_TIME_TIP_HEADER__
#define __GUI_RANCH_TIME_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchTimeTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			SURE
		};

		enum class ShowType
		{
			EXTEND_LAND,
			CROP_INFO,
			SPEED_UP_QUEUE
		};
	public:
		gui_ranchTimeTip();
		virtual ~gui_ranchTimeTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showTip(i32 landCode, i32 totalTime, i32 timeLeft, const String & titlle);
		bool showCropInfo(Vector3i blockPos, i32 blockId, i32 stage, i32 curStageTime, i32 residueHarvestNum);
		bool showSpeedUpQueue(i32 entityId, i32 queueId, i32 itemId, i32 totalTime, i32 timeLeft);

	private:
		String timeFormat(i32 time);

	private:
		ShowType m_showType = ShowType::EXTEND_LAND;

		SubscriptionGuard m_subscriptionGuard;
		GUIStaticText * m_tvTitleName = nullptr;
		GUIStaticText * m_price = nullptr;
		GUIStaticText * m_tvDesc = nullptr;
		GUIStaticText * m_tvTimeLeftValue = nullptr;
		GUIProgressBar * m_timeProgress = nullptr;
		i32 m_landCode = 0;
		i32 m_priceNum = 0;
		float m_timeLeft = 0.f;
		float m_totalTime = 1.f;


		i32 m_blockId = 0;
		Vector3i m_blockPos = Vector3i::ZERO;

		i32 m_queueId = 0;
		i32 m_entityId = 0;
		i32 m_itemId = 0;
	};
}

#endif