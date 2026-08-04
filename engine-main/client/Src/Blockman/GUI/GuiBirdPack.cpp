#include "GuiBirdPack.h"
#include "Util/ClientEvents.h"
#include "UI/GUIWindowManager.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "Util/LanguageManager.h"
#include "GuiDef.h"
#include "GuiBirdDressItem.h"
#include "GuiBirdFeedItem.h"
#include "Render/RenderEntity.h"
#include "Actor/ActorObject.h"
#include "Render/TextureAtlas.h"

namespace BLOCKMAN
{

	void GuiBirdPack::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiBirdPack::getPrivateChildren()
	{
		return { m_window };
	}

	void GuiBirdPack::initBirdInfo()
	{
		m_actor = dynamic_cast<GuiActorWindow*>(m_window->GetChildByIndex(4));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_station = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));

		m_level = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));
		m_exp = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(2)->GetChildByIndex(1));
		m_atk = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(2));
		m_speed = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(3));
		m_gather = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(4));
		m_convert = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(5));
		m_skill = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(6));
		m_talent = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(7));

		m_del = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6)->GetChildByIndex(0));
		m_equip = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6)->GetChildByIndex(1));
		m_feed = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6)->GetChildByIndex(2));


		m_del->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_DEL));
		m_equip->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_EQUIP));
		m_feed->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_FEED));
		if (LanguageManager::Instance())
		{
			m_del->SetText(LanguageManager::Instance()->getString("gui_bird_pack_btn_delete").c_str());
			m_feed->SetText(LanguageManager::Instance()->getString("gui_bird_pack_btn_feed").c_str());
			m_equip->SetText(LanguageManager::Instance()->getString("gui_bird_pack_btn_equip").c_str());
		}

	}

	void GuiBirdPack::initDress()
	{
		GUILayout* dress = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_glasses = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(0)->GetChildByIndex(0));
		m_hat = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(1)->GetChildByIndex(0));
		m_beak = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(2)->GetChildByIndex(0));
		m_wing = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(3)->GetChildByIndex(0));
		m_tail = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(4)->GetChildByIndex(0));
		m_effect = dynamic_cast<GUIStaticImage*>(dress->GetChildByIndex(5)->GetChildByIndex(0));

		m_dressPanel = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(7));
		m_dressDirectLeft = dynamic_cast<GUIStaticImage*>(m_dressPanel->GetChildByIndex(0));
		m_dressDirectRight = dynamic_cast<GUIStaticImage*>(m_dressPanel->GetChildByIndex(1));
		m_dressGridView = dynamic_cast<GUIGridView*>(m_dressPanel->GetChildByIndex(2));
		m_dressGridView->InitConfig(10.f, 10.f, 3);
		m_dressDirectRight->SetVisible(false);
		m_dressDirectLeft->SetVisible(false);
		m_dressPanel->SetVisible(false);

		m_glasses->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_GLASSES));
		m_hat->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_HAT));
		m_beak->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_BEAK));
		m_wing->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_WING));
		m_tail->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_TAIL));
		m_effect->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::IMG_EFFECT));
	}

	void GuiBirdPack::initFeed()
	{
		m_feedLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_feedList = dynamic_cast<GUIListBox*>(m_feedLayout->GetChildByIndex(1));
		m_feedOne = dynamic_cast<GUIButton*>(m_feedLayout->GetChildByIndex(2));
		m_feedTen = dynamic_cast<GUIButton*>(m_feedLayout->GetChildByIndex(3));
		m_feedFifty = dynamic_cast<GUIButton*>(m_feedLayout->GetChildByIndex(4));
		m_feedHunderd = dynamic_cast<GUIButton*>(m_feedLayout->GetChildByIndex(5));

		m_feedList->SetInterval(13.f);
		m_feedOne->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_FEED_ONE));
		m_feedTen->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_FEED_TEN));
		m_feedFifty->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_FEED_FIFTY));
		m_feedHunderd->subscribeEvent(EventButtonClick, std::bind(&GuiBirdPack::onClick, this, std::placeholders::_1, ViewId::BTN_FEED_HUNDERD));

		m_feedLayout->SetVisible(false);

		if (LanguageManager::Instance())
		{
			m_feedOne->SetText(LanguageManager::Instance()->getString("gui_bird_pack_feed_use_1").c_str());
			m_feedTen->SetText(LanguageManager::Instance()->getString("gui_bird_pack_feed_use_10").c_str());
			m_feedFifty->SetText(LanguageManager::Instance()->getString("gui_bird_pack_feed_use_50").c_str());
			m_feedHunderd->SetText(LanguageManager::Instance()->getString("gui_bird_pack_feed_use_100").c_str());
		}

	}

	void GuiBirdPack::changeDress(const BirdInfo & info)
	{
		if (!m_actor->GetActor())
		{
			m_actor->SetActor("g1041_bird.actor", "idle");
		}

		m_actor->UseBodyPart("glasses", info.glassesId);
		m_actor->UseBodyPart("hat", info.hatId);
		m_actor->UseBodyPart("beak", info.beakId);
		m_actor->UseBodyPart("wing", info.wingId);
		m_actor->UseBodyPart("tail", info.tailId);
		m_actor->UseBodyPart("tail_effect", info.effectId);
		m_actor->UseBodyPart("body", info.bodyId);

		auto  glassesIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("glasses_%s", info.glassesId.c_str()));
		auto  hatIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("hat_%s", info.hatId.c_str()));
		auto beakIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("beak_%s", info.beakId.c_str()));
		auto wingIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("wing_%s", info.wingId.c_str()));
		auto tailIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("tail_%s", info.tailId.c_str()));
		auto effectIcon = TextureAtlasRegister::Instance()->getAtlasSprite("bird_icon.json", StringUtil::Format("tail_effect_%s", info.effectId.c_str()));

		m_glasses->SetImage(glassesIcon ? StringUtil::Format("set:bird_icon.json image:glasses_%s", info.glassesId.c_str()).c_str() : "set:bird_pack.json image:glasses");
		m_hat->SetImage(hatIcon ? StringUtil::Format("set:bird_icon.json image:hat_%s", info.hatId.c_str()).c_str() : "set:bird_pack.json image:cap");
		m_beak->SetImage(beakIcon ? StringUtil::Format("set:bird_icon.json image:beak_%s", info.beakId.c_str()).c_str() : "set:bird_pack.json image:beak");
		m_wing->SetImage(wingIcon ? StringUtil::Format("set:bird_icon.json image:wing_%s", info.wingId.c_str()).c_str() : "set:bird_pack.json image:wing");
		m_tail->SetImage(tailIcon ? StringUtil::Format("set:bird_icon.json image:tail_%s", info.tailId.c_str()).c_str() : "set:bird_pack.json image:tail");
		m_effect->SetImage(effectIcon ? StringUtil::Format("set:bird_icon.json image:tail_effect_%s", info.effectId.c_str()).c_str() : "set:bird_pack.json image:effect");

		m_actor->GetActor()->SetBrightness(Color(1.f, 1.f, 1.f));
	}

	void GuiBirdPack::changeBirdFood()
	{
		if (m_birdInfo.id == 0)
			return;

		const auto& foods = Blockman::Instance()->m_pPlayer->m_birdSimulator->getFoods();
		m_feedLayout->SetVisible(true);
		m_dressPanel->SetVisible(false);
		m_feedList->ClearAllItem();
		if (foods.size() > 0)
		{
			BirdFood birdFood = BirdFood();
			for (auto& food : foods)
			{
				if (food.id == m_selectFoodId)
				{
					birdFood = food;
					break;
				}
			}

			if (birdFood.id > 0)
			{
				m_selectFoodId = birdFood.id;
				onFoodItemClick(birdFood.id, birdFood.num);
			}
			else
			{
				m_selectFoodId = foods[0].id;
				onFoodItemClick(foods[0].id, foods[0].num);
			}
		}
		else
		{
			m_selectFoodId = -1;
			onFoodItemClick(0, 0);
		}
		static int foodCount = 0;
		for (auto item : foods)
		{
			foodCount++;
			String viewName = String(m_feedList->GetName().c_str()) + "_Item_" + StringUtil::ToString(foodCount);
			GuiBirdFeedItem* itemView = (GuiBirdFeedItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_FEED_ITEM, viewName.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0), UDim(0, 70.0f));
			itemView->setFood(item);
			itemView->setSelectFoodId(m_selectFoodId);
			m_feedList->AddItem(itemView, false);
		}
	}

	void GuiBirdPack::changeDressPanel(i32 type)
	{
		if (!UICommon::checkBirdParam())
		{
			return;
		}

		auto birdDress = Blockman::Instance()->m_pPlayer->m_birdSimulator->findDressByType(type, m_birdInfo.id);


		if (birdDress.size() > 0)
		{
			sort(birdDress.begin(), birdDress.end(), BirdDressSort());
		}

		m_birdDressType = type;
		m_dressPanel->SetVisible(true);
		m_feedLayout->SetVisible(false);
		m_dressGridView->RemoveAllItems();
		static int count = 0;
		for (auto item : birdDress)
		{
			count++;
			String viewName = String(m_dressGridView->GetName().c_str()) + "_Item_" + StringUtil::ToString(count);
			String imageName = StringUtil::Format("set:bird_icon.json image:%s_%s", item.bodyName.c_str(), item.bodyId.c_str());
			GuiBirdDressItem* itemView = (GuiBirdDressItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_DRESS_ITEM, viewName.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 70.0f), UDim(0, 74.0f));
			itemView->setUse(item.isUse > 0);
			itemView->setDressImage(imageName);
			itemView->setBirdId(m_birdInfo.id);
			itemView->setDress(item);
			m_dressGridView->AddItem(itemView);
		}
		m_dressDirectLeft->SetVisible(type <= 3);
		m_dressDirectRight->SetVisible(type >= 4);
		switch (type)
		{
		case 1:
			m_dressDirectLeft->SetPosition(UVector2(UDim(0, -22.f), UDim(0, 20.f)));
			break;
		case 2:
			m_dressDirectLeft->SetPosition(UVector2(UDim(0, -22.f), UDim(0.5f, -22.5f)));
			break;
		case 3:
			m_dressDirectLeft->SetPosition(UVector2(UDim(0, -22.f), UDim(1.f, -67.5f)));
			break;
		case 4:
			m_dressDirectRight->SetPosition(UVector2(UDim(1, -15.f), UDim(0, 20.f)));
			break;
		case 5:
			m_dressDirectRight->SetPosition(UVector2(UDim(1, -15.f), UDim(0.5f, -22.5f)));
			break;
		case 6:
			m_dressDirectRight->SetPosition(UVector2(UDim(1, -15.f), UDim(1.f, -67.5f)));
			break;
		}

	}

	GuiBirdPack::GuiBirdPack(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		TextureAtlasRegister::Instance()->addTextureAtlas("bird_icon.json");
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdPack.json"));
		renameComponents(m_window);
		initBirdInfo();
		initDress();
		initFeed();

		AddWindowToDrawList(*m_window);
		m_subscriptionGuard.add(BirdFoodItemClickEvent::subscribe(std::bind(&GuiBirdPack::onFoodItemClick, this, std::placeholders::_1, std::placeholders::_2)));

	}

	void GuiBirdPack::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiBirdPack::setBird(BirdInfo info)
	{
		m_birdInfo = info;
		m_name->SetText(LanguageManager::Instance()->getString(info.name).c_str());
		setNameColor(info.quality);
		m_level->SetText(StringUtil::Format("Lv:%d", info.level).c_str());
		const GUIString& text = info.level < info.maxLevel ? StringUtil::Format("%d/%d", info.exp, info.nextExp).c_str() : StringUtil::Format("%d/max", info.exp).c_str();
		m_exp->GetChildByIndex(0)->SetText(text);
		m_exp->SetProgress((info.exp * 1.0f / info.nextExp * 1.0f));
		m_atk->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_attack").c_str(), info.attack).c_str());
		m_speed->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_speed").c_str(), info.speed + 200).c_str());
		m_gather->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_gather1").c_str(), info.gather, info.gatherTime).c_str());
		m_convert->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_convert1").c_str(), info.conver, info.convertTime).c_str());
		m_skill->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_skill").c_str(), LanguageManager::Instance()->getString(info.skill).c_str()).c_str());
		m_talent->SetText(StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_pack_info_talent").c_str(), LanguageManager::Instance()->getString(info.talent).c_str()).c_str());
		m_equip->SetText(LanguageManager::Instance()->getString(info.isCarry ? "gui_bird_pack_btn_unequip" : "gui_bird_pack_btn_equip").c_str());
		changeDress(info);
		switch (info.quality)
		{
		case 0:
		case 1:
			m_station->SetImage("set:bird_pack.json image:brownness");
			break;
		case 2:
			m_station->SetImage("set:bird_pack.json image:white");
			break;
		case 3:
			m_station->SetImage("set:bird_pack.json image:yellow");
			break;
		case 4:
			m_station->SetImage("set:bird_pack.json image:blue");
			break;
		}
	}

	void GuiBirdPack::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiBirdPack::onClick(const EventArgs & args, ViewId viewId)
	{

		if (m_birdInfo.id == 0)
		{
			return false;
		}

		switch (viewId)
		{
		case ViewId::BTN_DEL:
			hideView();

			if (UICommon::checkBirdBagParam() && m_birdInfo.id > 0)
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->birds.size() > 1)
				{
					ShowBirdDeleteTipEvent::emit(m_birdInfo.id, m_birdInfo.name);
				}
				else
				{
					ShowBirdCommonTipEvent::emit("gui_bird_tip_delete_bird_fail");
				}
			}

			break;
		case ViewId::BTN_EQUIP:
			hideView();
			if (UICommon::checkBirdBagParam()  &&  m_birdInfo.id > 0 )
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (m_birdInfo.isCarry)
				{
					ClientNetwork::Instance()->getSender()->sendBirdBagOperation(m_birdInfo.id, 2);
				}
				else
				{
					if (bag->curCarry >= bag->maxCarry)
					{
						ShowBirdCarryFullTipEvent::emit(bag->expandCarryPrice, bag->expandCurrencyType);
					}
					else
					{
						ClientNetwork::Instance()->getSender()->sendBirdBagOperation(m_birdInfo.id, 1);
					}
					
				}

			}
			break;
		case ViewId::BTN_FEED:
			changeBirdFood();
			break;
		case ViewId::BTN_FEED_ONE:
			if (m_birdInfo.id > 0 && m_selectFoodId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdFeed(m_birdInfo.id, m_selectFoodId, 1);
			}
			break;
		case ViewId::BTN_FEED_TEN:
			if (m_birdInfo.id > 0 && m_selectFoodId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdFeed(m_birdInfo.id, m_selectFoodId, 10);
			}
			break;
		case ViewId::BTN_FEED_FIFTY:
			if (m_birdInfo.id > 0 && m_selectFoodId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdFeed(m_birdInfo.id, m_selectFoodId, 50);
			}
			break;
		case ViewId::BTN_FEED_HUNDERD:
			if (m_birdInfo.id > 0 && m_selectFoodId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdFeed(m_birdInfo.id, m_selectFoodId, 100);
			}
			break;
		case ViewId::IMG_GLASSES:
			changeDressPanel(1);
			break;
		case ViewId::IMG_HAT:
			changeDressPanel(2);
			break;
		case ViewId::IMG_BEAK:
			changeDressPanel(3);
			break;
		case ViewId::IMG_WING:
			changeDressPanel(4);
			break;
		case ViewId::IMG_TAIL:
			changeDressPanel(5);
			break;
		case ViewId::IMG_EFFECT:
			changeDressPanel(6);
			break;
		}
		return true;
	}

	void GuiBirdPack::setNameColor(i32 quality)
	{
		switch (quality) {
		case 1:
			m_name->SetTextColor(Color(232.f / 255.f, 119.f / 255.f, 119.f / 255.f));
			m_name->SetTextBoader(Color(141.f / 255.f, 48.f / 255.f, 27.f / 255.f));
			break;
		case 2:
			m_name->SetTextColor(Color(209.f / 255.f, 252.f / 255.f, 242.f / 255.f));
			m_name->SetTextBoader(Color(59.f / 255.f, 59.f / 255.f, 59.f / 255.f));
			break;
		case 3:
			m_name->SetTextColor(Color(255.f / 255.f, 255.f / 255.f, 0.f / 255.f));
			m_name->SetTextBoader(Color(61.f / 255.f, 27.f / 255.f, 7.f / 255.f));
			break;
		case 4:
			m_name->SetTextColor(Color(0.f / 255.f, 138.f / 255.f, 255.f / 255.f));
			m_name->SetTextBoader(Color(14.f / 255.f, 17.f / 255.f, 57.f / 255.f));
			break;
		}

	}

	bool GuiBirdPack::onFoodItemClick(i32 foodId, i32 num)
	{
		if (IsVisible())
		{
			m_selectFoodId = foodId;
			m_feedOne->SetEnabled(num > 0);
			m_feedTen->SetEnabled(num >= 10);
			m_feedFifty->SetEnabled(num >= 50);
			m_feedHunderd->SetEnabled(num >= 100);
			if (m_feedList->getContainerWindow())
			{
				for (size_t i = 0; i < m_feedList->getContainerWindow()->GetChildCount(); i++)
				{
					dynamic_cast<GuiBirdFeedItem*>(m_feedList->getContainerWindow()->GetChildByIndex(i)->GetChildByIndex(0))->setSelectFoodId(foodId);
				}
			}
		}
		return true;
	}

	void GuiBirdPack::changeView()
	{
		if (m_feedLayout->IsVisible())
		{
			changeBirdFood();
		}

		if (m_dressPanel->IsVisible())
		{
			changeDressPanel(m_birdDressType);
		}
	}

	void GuiBirdPack::hideView()
	{
		m_feedLayout->SetVisible(false);
		m_dressPanel->SetVisible(false);
	}

}