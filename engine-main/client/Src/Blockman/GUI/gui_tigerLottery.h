#pragma once
#ifndef __GUI_TIGER_LOTTERY_HEADER__
#define __GUI_TIGER_LOTTERY_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class LotteryItem 
	{
	public:
		String m_uniqueId = "";
		String m_image = "";
		String m_name = "";
	public:
		LotteryItem() = default;
		LotteryItem(String uniqueId, String name,String image)
			: m_uniqueId(uniqueId)
			, m_name(name)
			, m_image(image)
		{
		};
	};

	class gui_tigerLottery : public gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;
		map<String, LotteryItem>::type m_items;
		
		list<String>::type m_firstIds;
		list<String>::type m_secondIds;
		list<String>::type m_thirdIds;

		String m_firstId = "";
		String m_secondId = "";
		String m_thirdId = "";

		float m_firstTopPosY = 0;
		float m_secondTopPosY = 0;
		float m_thirdTopPosY = 0;

		bool m_firstScroll = false;
		bool m_secondScroll = false;
		bool m_thirdScroll = false;

		int m_firstThroughTimes = 0;
		int m_secondThroughTimes = 0;
		int m_thirdThroughTimes = 0;

		float m_firstMoveSpeed = 0;
		float m_secondMoveSpeed = 0;
		float m_thirdMoveSpeed = 0;

		float m_subMoveSpeed = 2.0f;

		int m_countdownTime = 0;

	private:
		GUILayout* m_item1_scroll = nullptr;
		GUIStaticText* m_item1_name = nullptr;
		GUIStaticImage* m_item1_mask = nullptr;
		GUIStaticImage* m_item1_lock = nullptr;
		GUIButton* m_item1_choose = nullptr;
		map<String, GUIStaticImage*>::type m_item1_childs;

		GUILayout* m_item2_scroll = nullptr;
		GUIStaticText* m_item2_name = nullptr;
		GUIStaticImage* m_item2_mask = nullptr;
		GUIStaticImage* m_item2_lock = nullptr;
		GUIButton* m_item2_choose = nullptr;
		map<String, GUIStaticImage*>::type m_item2_childs;

		GUILayout* m_item3_scroll = nullptr;
		GUIStaticText* m_item3_name = nullptr;
		GUIStaticImage* m_item3_mask = nullptr;
		GUIStaticImage* m_item3_lock = nullptr;	
		GUIButton* m_item3_choose = nullptr;
		map<String, GUIStaticImage*>::type m_item3_childs;

		GUIButton* m_lottery_again = nullptr;
		GUIStaticText* m_countdown = nullptr;

	public:
		gui_tigerLottery();
		~gui_tigerLottery();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onUpdateChild(String selectId, int& throughTimes, float& moveSpeed, map<String, GUIStaticImage*>::type childs, float topPosY);

		bool onLotteryData(const String& data);
		void initAllChilds();
		float initItemChilds(GUILayout* parent, map<String, GUIStaticImage*>::type& childs, list<String>::type ids);
		bool onLotteryResult(const String& firstId, const String& secondId, const String& thirdId);
		void setLotteryResultName(String uniqueId, list<String>::type ids, GUIStaticText* item);

		void onSelectRandom();
 		bool onSelectFirst(const EventArgs & events);
		bool onSelectSecond(const EventArgs & events);
		bool onSelectThird(const EventArgs & events);
		bool onLotteryAgain(const EventArgs & events);

		void selectItem(String uniqueId);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif