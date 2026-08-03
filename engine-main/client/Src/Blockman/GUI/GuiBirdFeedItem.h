#pragma once
#ifndef __GUI_BIRD_FEED_ITEM_HEADER__
#define __GUI_BIRD_FEED_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Util/SubscriptionGuard.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdFeedItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_num = nullptr;
		GUIStaticText* m_des = nullptr;
		SubscriptionGuard m_subscriptionGuard;

		BirdFood m_food;
		i32 m_selectFoodId = -1;

	private:
		void removeComponents();

	public:
		GuiBirdFeedItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdFeedItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args);
		void setSelectFoodId(i32 foodId) { m_selectFoodId = foodId; }
		void setFood(BirdFood food);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
