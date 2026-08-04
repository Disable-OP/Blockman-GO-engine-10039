#pragma once
#ifndef __GUI_PLAYER_INFO_HEADER__
#define __GUI_PLAYER_INFO_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListBox.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIListHorizontalBox.h"
#include <list>
#include <map>
#include <atomic>


using namespace LORD;

namespace BLOCKMAN
{
	class BirdBuff;
	class gui_playerInfo : public  gui_layout
	{
	public:
		gui_playerInfo();
		virtual ~gui_playerInfo();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void resetMaxHealth(float health) {}
		bool updateHeartBar(int hp, int maxHp);
		bool updateDefenseUI(int defense, int maxDefense);
		bool updateAttackUI(int attack, int maxAttack);
		bool onBirdBuffChange();

	private:
		bool openEffectInfo(const EventArgs & events);
		void updateEffectUI();
		void updateHealthUI();
		void updateFoodStatUI();
		void updateArmorUI();
		void updateAirUI();
		void initBirdBuff(const vector<BirdBuff>::type& birdBuffs);
		void showBuffItem(GUIString itemName, int& showCount);
		GUIStaticImage* initBuffItem(String itemName,String itemRes);

	private:
		
		SubscriptionGuard m_subscriptionGuard;

		int m_oldAir = 10;
		int m_airFps = 0;

		GUILayout* m_airLayout = nullptr;
		GUILayout* m_armorLayout = nullptr;
		GUILayout* m_buffLayout = nullptr;
		GUILayout* m_heartBar = nullptr;
		GUILayout* m_defenseLayout = nullptr;
		GUILayout* m_attackLayout = nullptr;

		GUIProgressBar* m_health_value = nullptr;
		GUIProgressBar* m_foodstat_value = nullptr;
		GUIProgressBar* m_defense_value = nullptr;
		GUIProgressBar* m_attack_value = nullptr;

		GUIStaticText* m_health_number = nullptr;
		GUIStaticText* m_foodstat_number = nullptr;
		GUIStaticText* m_defense_number = nullptr;
		GUIStaticText* m_attack_number = nullptr;
		GUIListHorizontalBox* m_birdBuffList = nullptr;

		list<GUIStaticImage*>::type m_listAir;
		list<GUIStaticImage*>::type m_listArmor;
		list<GUIStaticImage*>::type m_listBuff;
	};


}
#endif