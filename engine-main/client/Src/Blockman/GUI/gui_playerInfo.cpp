#include "gui_playerInfo.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Block/BM_Material.h"
#include "Util/StringUtil.h"
#include "Util/ClientEvents.h"
#include "Item/Potion.h"
#include "Setting/UIDisplaySetting.h"
#include <list>
#include <iostream>
#include <string.h>
#include <math.h>
#include "game.h"
#include "Util/UICommon.h"
#include "World/BirdSimulator.h"
#include "UI/GUIFontManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_playerInfo::gui_playerInfo() :
		gui_layout("PlayerInfo.json"),
		m_oldAir(10),
		m_airFps(0)
	{

	}

	gui_playerInfo::~gui_playerInfo()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerInfo::onLoad()
	{
		m_rootWindow->SetTouchable(false);
		
		m_airLayout = getWindow<GUILayout>("PlayerInfo-Air");
		m_armorLayout = getWindow<GUILayout>("PlayerInfo-Armor");
		m_buffLayout = getWindow<GUILayout>("PlayerInfo-Effect-Layout");
		m_heartBar = getWindow<GUILayout>("PlayerInfo-HeartBar");
		m_defenseLayout = getWindow<GUILayout>("PlayerInfo-Defense");
		m_attackLayout = getWindow<GUILayout>("PlayerInfo-Attack");
		m_birdBuffList = getWindow<GUIListHorizontalBox>("PlayerInfo-BuffList");
		
		m_airLayout->SetVisible(false);
		m_defenseLayout->SetVisible(false);
		m_attackLayout->SetVisible(false);

		m_health_value = getWindow<GUIProgressBar>("PlayerInfo-Health-Value");
		m_foodstat_value = getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value");
		m_defense_value = getWindow<GUIProgressBar>("PlayerInfo-Defense-Value");
		m_attack_value = getWindow<GUIProgressBar>("PlayerInfo-Attack-Value");

		m_health_number = getWindow<GUIStaticText>("PlayerInfo-Health-Number");
		m_foodstat_number = getWindow<GUIStaticText>("PlayerInfo-FoodSaturation-Number");
		m_defense_number = getWindow<GUIStaticText>("PlayerInfo-Defense-Number");
		m_attack_number = getWindow<GUIStaticText>("PlayerInfo-Attack-Number");

		//EffectToxication EffectMoveSpeed EffectNightVision EffectJump EffectInvisibility EffectBackground
		m_listBuff.push_back(initBuffItem("PlayerInfo-Effect-Jump", "EffectJump"));
		m_listBuff.push_back(initBuffItem("PlayerInfo-Effect-Invisibility", "EffectInvisibility"));
		m_listBuff.push_back(initBuffItem("PlayerInfo-Effect-Night-Vision", "EffectNightVision"));
		m_listBuff.push_back(initBuffItem("PlayerInfo-Effect-Move-Speed", "EffectMoveSpeed"));
		m_listBuff.push_back(initBuffItem("PlayerInfo-Effect-Toxication", "EffectToxication"));

		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			m_buffLayout->AddChildWindow(item);
			item->SetVisible(false);
			item->subscribeEvent(EventWindowTouchUp, std::bind(&gui_playerInfo::openEffectInfo, this, std::placeholders::_1));
		}

		for (int i = 1; i < 11; i++)
		{
			GUIStaticImage* airValue = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Air-Value-%d", i).c_str());
			airValue->SetTouchable(false);
			airValue->SetArea(UDim(1, -(24.0f + 26.0f * (i - 1))), UDim(0, 0), UDim(0, 24.0f), UDim(0, 24.0f));
			airValue->SetImage("set:player_info_icon.json image:playerInfoAirFull");
			m_listAir.push_back(airValue);
			m_airLayout->AddChildWindow(airValue);
		}

		for (int i = 1; i < 11; i++)
		{
			GUIStaticImage* armorValue = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Armor-Value-%d", i).c_str());
			armorValue->SetTouchable(false);
			armorValue->SetArea(UDim(0, (i - 1) * 26.0f), UDim(0, 0), UDim(0, 24.0f), UDim(0, 28.0f));
			armorValue->SetImage("set:player_info_icon.json image:playerInfoNullArmor");
			m_listArmor.push_back(armorValue);
			m_armorLayout->AddChildWindow(armorValue);
		}

		m_subscriptionGuard.add(ChangeHeartEvent::subscribe(std::bind(&gui_playerInfo::updateHeartBar, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeDefenseEvent::subscribe(std::bind(&gui_playerInfo::updateDefenseUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeAttackEvent::subscribe(std::bind(&gui_playerInfo::updateAttackUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BirdBuffChangeEvent::subscribe(std::bind(&gui_playerInfo::onBirdBuffChange, this)));

	}

	bool gui_playerInfo::updateHeartBar(int hp, int maxHp)
	{
		if (!m_heartBar)
			return false;
		m_heartBar->CleanupChildren();
		if (maxHp <= 0)
			return false;
		float itemWidth = 30;
		float itemSpace = 10;
		float width = itemWidth * maxHp + (maxHp - 1) * itemSpace;
		float startX = (m_heartBar->GetWidth().d_offset - width) / 2;
		for (int i = 0; i < maxHp; i++)
		{
			GUIStaticImage* itemHeart = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Heart-Item-%d", i).c_str());
			itemHeart->SetTouchable(false);
			itemHeart->SetArea(UDim(0, startX + i * (itemWidth + itemSpace)), UDim(0, 0), UDim(0, itemWidth), UDim(0, itemWidth));
			if (i < hp)
				itemHeart->SetImage("set:player_info_icon.json image:playerInfoFullHP");
			else
				itemHeart->SetImage("set:player_info_icon.json image:playerInfoNullHP");
			m_heartBar->AddChildWindow(itemHeart);
		}
		return true;
	}

	bool gui_playerInfo::updateDefenseUI(int defense, int maxDefense)
	{
		if (maxDefense <= 0)
		{
			m_defenseLayout->SetVisible(false);
			return false;
		}
		m_defenseLayout->SetVisible(true);
		m_defense_value->SetProgress((float)defense / (float)maxDefense);
		m_defense_number->SetText(StringUtil::Format("%d/%d", defense, maxDefense).c_str());
		return true;
	}

	bool gui_playerInfo::updateAttackUI(int attack, int maxAttack)
	{
		if (maxAttack <= 0)
		{
			m_attackLayout->SetVisible(false);
			return false;
		}
		m_attackLayout->SetVisible(true);
		m_attack_value->SetProgress((float)attack / (float)maxAttack);
		m_attack_number->SetText(StringUtil::Format("%d/%d", attack, maxAttack).c_str());
		return true;
	}

	bool gui_playerInfo::onBirdBuffChange()
	{
		if (!UICommon::checkBirdParam())
		{
			m_birdBuffList->SetVisible(false);
			return false;
		}

		const auto buffs = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBuffs();
		m_birdBuffList->SetVisible(buffs.size() > 0);
		initBirdBuff(buffs);
		return true;
	}

	void gui_playerInfo::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			return;
		}

		updateEffectUI();
		updateHealthUI();
		updateFoodStatUI();
		updateArmorUI();
		updateAirUI();

		m_rootWindow->SetVisible(GameClient::CGame::Instance()->GetGameType() != ClientGameType::Rancher);
	}


	bool gui_playerInfo::openEffectInfo(const EventArgs & events)
	{
		return false;
	}

	void gui_playerInfo::updateEffectUI()
	{
		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			item->SetVisible(false);
		}

		if (!UIDisplaySetting::getUIDisplay()->isShowPotionHint)
			return;

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		map<int, PotionEffect*>::type::iterator iter;

		int itemShowCount = 0;
		for (iter = player->getActivePotionEffects().begin(); iter != player->getActivePotionEffects().end(); iter++)
		{
			int id = iter->second->getPotionID();
			switch (id)
			{
			case 1:
				showBuffItem("PlayerInfo-Effect-Move-Speed-bg", itemShowCount);
				break;
			case 8:
				showBuffItem("PlayerInfo-Effect-Jump-bg", itemShowCount);
				break;
			case 14:
				showBuffItem("PlayerInfo-Effect-Invisibility-bg", itemShowCount);
				break;
			case 16:
				//showBuffItem("PlayerInfo-Effect-Night-Vision-bg", itemShowCount);
				break;
			case 19:
				showBuffItem("PlayerInfo-Effect-Toxication-bg", itemShowCount);
				break;
			}
		}
	}

	void gui_playerInfo::updateHealthUI()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		float curHp = player->getHealth();
		float maxHp = player->getMaxHealth();
		m_health_value->SetProgress(curHp / maxHp);
		m_health_number->SetText(StringUtil::Format("%d/%d", (int)Math::Floor(curHp + 0.5), (int)Math::Floor(maxHp + 0.5)).c_str());
	}

	void gui_playerInfo::updateFoodStatUI()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore || GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			getWindow<GUILayout>("PlayerInfo-FoodSaturation")->SetVisible(false);
			return;
		}

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		int foodLevel = player->getFoodStats()->getFoodLevel();
		int prevFoodLevel = player->getFoodStats()->getPrevFoodLevel();
		m_foodstat_value->SetProgress((float)foodLevel / (float)prevFoodLevel);
		m_foodstat_number->SetText(StringUtil::Format("%d/%d", foodLevel, prevFoodLevel).c_str());
	}

	void gui_playerInfo::updateArmorUI()
	{
		if (m_defenseLayout->IsVisible())
		{
			m_armorLayout->SetVisible(false);
			return;
		}
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		std::string imageName = "";
		list<GUIStaticImage*>::iterator guiIter;
		GUIStaticImage* guiStaticImage = nullptr;
		float armorValue = player->getTotalArmorValue() / 2.0f;
		m_armorLayout->SetVisible(armorValue != 0);
		if (armorValue != 0)
		{
			int i = 1;
			for (guiIter = m_listArmor.begin(); guiIter != m_listArmor.end(); guiIter++)
			{
				guiStaticImage = *guiIter;
				if (armorValue >= i)
				{
					imageName = "set:player_info_icon.json image:playerInfoFullArmor";
				}
				else if (armorValue > i - 1)
				{
					imageName = "set:player_info_icon.json image:playerInfoHalfArmor";
				}
				else
				{
					imageName = "set:player_info_icon.json image:playerInfoNullArmor";
				}
				guiStaticImage->SetImage(imageName);
				i++;
			}
		}
	}

	void gui_playerInfo::updateAirUI()
	{
		if (m_attackLayout->IsVisible())
		{
			m_airLayout->SetVisible(false);
			return;
		}
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		m_airLayout->SetVisible(player->isInsideOfMaterial(BM_Material::BM_MAT_water));
		list<GUIStaticImage*>::iterator guiIter;
		GUIStaticImage* guiStaticImage = nullptr;
		int i = 1;
		if (player->isInsideOfMaterial(BM_Material::BM_MAT_water))
		{
			i = 1;
			float air = ((float)player->getAir()) / 30.f;
			air = (air > 0.0f) ? floor(air + 0.5f) : ceil(air - 0.5f);

			if ((int)air != m_oldAir && m_airFps < 3)
			{
				for (guiIter = m_listAir.begin(); guiIter != m_listAir.end(); guiIter++) {
					guiStaticImage = *guiIter;
					guiStaticImage->SetVisible(i <= m_oldAir);
					guiStaticImage->SetImage(i == m_oldAir ? "set:player_info_icon.json image:playerInfoAirRupture" : "set:player_info_icon.json image:playerInfoAirFull");
					i++;
				}
				m_airFps++;
			}
			else
			{
				m_airFps = 0;
				m_oldAir = (int)air;
				for (guiIter = m_listAir.begin(); guiIter != m_listAir.end(); guiIter++) {
					guiStaticImage = *guiIter;
					guiStaticImage->SetImage("set:player_info_icon.json image:playerInfoAirFull");
					guiStaticImage->SetVisible(i <= air);
					i++;
				}
			}
		}
	}

	void gui_playerInfo::initBirdBuff(const vector<BirdBuff>::type& birdBuffs)
	{
		m_birdBuffList->ClearAllItem();
		static int count = 0;
		for (auto buff : birdBuffs)
		{
			String  viewName = StringUtil::Format("PlayerInfo-BirfBuff-Item-%d", count++);
			String  levelViewName = StringUtil::Format("PlayerInfo-BirfBuff-Level-Item-%d", count);
			GUIStaticText* buffLevelView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, levelViewName.c_str());
			buffLevelView->SetSize(UVector2(UDim(1.f, 0.f), UDim(1.f, 0.f)));
			buffLevelView->SetTextVertAlign(VA_BOTTOM);
			buffLevelView->SetTextHorzAlign(HA_RIGHT);
			buffLevelView->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
			buffLevelView->SetText(StringUtil::Format("x%d", buff.level).c_str());
			buffLevelView->SetTouchable(false);
			GUIStaticImage* buffView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, viewName.c_str());
			buffView->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			buffView->SetSize(UVector2(UDim(0, 50.f), UDim(0, 50.f)));
			buffView->SetTouchable(false);
			buffView->SetImage(buff.icon.c_str());
			buffView->AddChildWindow(buffLevelView);
			m_birdBuffList->AddItem(buffView, false);
		}
	}

	void gui_playerInfo::showBuffItem(GUIString itemName, int& showCount)
	{
		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			if (item->GetName() == itemName)
			{
				item->SetVisible(true);
				item->SetArea(UDim(0, 0), UDim(0, 51.0f*showCount), UDim(0, 50.0f), UDim(0, 50.0f));
				showCount++;
				break;
			}
		}
	}

	GUIStaticImage* gui_playerInfo::initBuffItem(String itemName, String itemRes)
	{
		GUIStaticImage* itemBg = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, (itemName + "-bg").c_str());
		itemBg->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 50.0f), UDim(0, 50.0f));
		itemBg->SetImage("set:inventory.json image:EffectBackground");
		itemBg->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		itemBg->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "4 4 4 4");
		GUIStaticImage* itemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, (itemName + "-Icon").c_str());
		itemIcon->SetImage(("set:inventory.json image:" + itemRes).c_str());
		itemBg->AddChildWindow(itemIcon);
		itemIcon->SetArea(UDim(0, 2.5f), UDim(0, 2.5f), UDim(1, -5.0f), UDim(1, -5.0f));
		return itemBg;
	}
}
