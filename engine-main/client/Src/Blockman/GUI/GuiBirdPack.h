#pragma once
#ifndef __GUI_BIRD_PACK_HEADER__
#define __GUI_BIRD_PACK_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIListBox.h"
#include "UI/GUIGridView.h"
#include "GUI/GuiActorWindow.h"
#include "Util/SubscriptionGuard.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{

	class GuiBirdPack : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			BTN_DEL,
			BTN_EQUIP,
			IMG_GLASSES,
			IMG_HAT,
			IMG_BEAK,
			IMG_WING,
			IMG_TAIL,
			IMG_EFFECT,
			BTN_FEED,
			BTN_FEED_ONE,
			BTN_FEED_TEN,
			BTN_FEED_FIFTY,
			BTN_FEED_HUNDERD
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GuiActorWindow* m_actor = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_level = nullptr;
		GUIProgressBar* m_exp = nullptr;
		GUIStaticText *m_atk = nullptr;
		GUIStaticText* m_speed = nullptr;
		GUIStaticText* m_gather = nullptr;
		GUIStaticText* m_convert = nullptr;
		GUIStaticText* m_skill = nullptr;
		GUIStaticText* m_talent = nullptr;
		GUIStaticImage* m_station = nullptr;
		
		GUIButton* m_del = nullptr;
		GUIButton* m_equip = nullptr;
		GUIButton* m_feed = nullptr;
		GUILayout* m_feedLayout = nullptr;
		GUIListBox* m_feedList = nullptr;
		GUIButton* m_feedOne = nullptr;
		GUIButton* m_feedTen = nullptr;
		GUIButton* m_feedFifty = nullptr;
		GUIButton* m_feedHunderd = nullptr;

		GUIStaticImage* m_glasses = nullptr;
		GUIStaticImage* m_hat = nullptr;
		GUIStaticImage* m_beak = nullptr;
		GUIStaticImage* m_wing = nullptr;
		GUIStaticImage* m_tail = nullptr;
		GUIStaticImage* m_effect = nullptr;

		GUILayout* m_dressPanel = nullptr;
		GUIGridView* m_dressGridView = nullptr;


		GUIStaticImage* m_dressDirectLeft = nullptr;
		GUIStaticImage* m_dressDirectRight = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	private:
		BirdInfo m_birdInfo;
		i32 m_birdDressType = 0;
		i32 m_selectFoodId = -1;
		i32 m_curryCarryNum = 0;
		i32 m_curryCapacityNum = 0;

	private:
		void initBirdInfo();
		void initDress();
		void initFeed();
		void changeDress(const BirdInfo& info);
		void changeBirdFood();
		void changeDressPanel(i32 type);

	public:
		GuiBirdPack(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdPack() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, ViewId viewId);
		void setBird(BirdInfo info);
		void setNameColor(i32 quality);
		bool onFoodItemClick(i32 foodId, i32 num);
	
		void changeView();
		void hideView();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif