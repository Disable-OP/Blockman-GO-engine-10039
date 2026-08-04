#include "gui_ranchBuildFarm.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiRanchItem.h"
#include "GUI/GuiRanchBuildRecipes.h"
#include "GUI/GuiRanchBuildProduct.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiDef.h"
#include "GUI/GuiRanchBuildFarmQueue.h"
#include "Network/ClientNetwork.h"
#include "GUI/RootGuiLayout.h"
#include "game.h"
#include "ShellInterface.h"
#include "Object/Root.h"
#include "Setting/GuideSetting.h"
#include "Inventory/Wallet.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchBuildFarm::gui_ranchBuildFarm()
		: gui_layout("RanchBuildFarm.json")
	{
	}

	gui_ranchBuildFarm::~gui_ranchBuildFarm()
	{
	}

	void gui_ranchBuildFarm::onLoad()
	{
		m_title = getWindow<GUIStaticText>("RanchBuildFarm-Title");
		m_recipeItems = getWindow<GUIListHorizontalBox>("RanchBuildFarm-Recipe-Items");
		m_productItems = getWindow<GUIListHorizontalBox>("RanchBuildFarm-Product-Items");
		m_queueItems = getWindow<GUIListHorizontalBox>("RanchBuildFarm-Queue-Items");

		m_recipeLayout = getWindow<GUILayout>("RanchBuildFarm-Recipe");
		m_recipeInfoLayout = getWindow<GUILayout>("RanchBuildFarm-RecipeInfo");
		m_buildOperation = getWindow<GuiRanchBuildOperation>("RanchBuildFarm-Operation");


		m_btnBuyQueue = getWindow<GUIButton>("RanchBuildFarm-Btn-Buy-Queue");

		m_recipeInfoLayout->SetVisible(false);
		m_queueItems->SetInterval(24.f);
		m_recipeItems->SetInterval(5.f);
		m_productItems->SetInterval(0.f);
		m_recipeItems->SetTouchable(true);

		m_btnBuyQueue->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFarm::onClick, this, std::placeholders::_1, ViewId::UNLOCK));
		m_recipeInfoLayout->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchBuildFarm::onClick, this, std::placeholders::_1, ViewId::MAIN));
		getWindow("RanchBuildFarm")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchBuildFarm::onClick, this, std::placeholders::_1, ViewId::MAIN));
		getWindow("RanchBuildFarm-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFarm::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("RanchBuildFarm-Btn-Remove")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFarm::onClick, this, std::placeholders::_1, ViewId::BTN_REMOVE));


		m_subscriptionGuard.add(ShowRanchBuildFarmEvent::subscribe(std::bind(&gui_ranchBuildFarm::showGui, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RanchBuildFarmUpdateEvent::subscribe(std::bind(&gui_ranchBuildFarm::onDataUpdate, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RanchRecipeItemClickEvent::subscribe(std::bind(&gui_ranchBuildFarm::onRecipeItemClick, this, std::placeholders::_1)));

	}

	void gui_ranchBuildFarm::onShow()
	{
		m_recipeInfoLayout->SetVisible(false);
		m_isSecondClose = false;
		GameVideoChangeEvent::emit(2);
		playGameVideo();
	}

	void gui_ranchBuildFarm::onHide()
	{
		m_recipeInfoLayout->SetVisible(false);
		m_isSecondClose = false;
		GameVideoChangeEvent::emit(0);
	}

	void gui_ranchBuildFarm::onUpdate(ui32 nTimeElapse)
	{

		if (!Blockman::Instance() || !Blockman::Instance()->m_pWorld || !Blockman::Instance()->m_pPlayer->m_ranch)
		{
			return;
		}
		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
		if (!buildEntity)
		{
			return;
		}
	}

	bool gui_ranchBuildFarm::showGui(i32 entityId)
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pWorld)
		{
			return false;
		}
		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(entityId));
		if (!buildEntity)
		{
			return false;
		}
		m_entityId = entityId;
		setText(m_title, buildEntity->getNameLang().c_str());
		RootGuiLayout::Instance()->showRanchBuildFarm();
		initProductItem(buildEntity->m_products);
		initRecipeItem(buildEntity->m_recipes);
		initQueueItem(buildEntity->isCanWorking(), buildEntity->m_actorId, buildEntity->m_unlockQueues);
		dynamic_cast<GUIStaticImage*>(m_btnBuyQueue->GetChildByIndex(0))->SetImage(UICommon::getCurrencyIconByType((CurrencyType)buildEntity->m_queueUnlockCurrencyType).c_str());
		return false;
	}


	bool gui_ranchBuildFarm::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::MAIN:
			m_recipeInfoLayout->SetVisible(false);
			break;
		case ViewId::CLOSE:
		{
			auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
			if (!buildEntity)
			{
				RootGuiLayout::Instance()->showMainControl();
				return false;
			}

			if (buildEntity->isAwait() && !m_isSecondClose)
			{
				m_isSecondClose = true;
				ShowRanchCommonTipEvent::emit("gui_ranch_build_product_capacity_full");
			}
			else
			{
				RootGuiLayout::Instance()->showMainControl();
			}
			break;
		}
		case ViewId::UNLOCK:
		{
			auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
			if (!buildEntity)
			{
				return false;
			}
			ShowRanchQueueUnlockEvent::emit(m_entityId, 0, buildEntity->m_queueUnlockPrice, buildEntity->m_queueUnlockCurrencyType, "gui_ranch_build_queue_unlock_msg");
			break;
		}
		case ViewId::BTN_REMOVE:
		{
			auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
			if (!buildEntity)
			{
				return false;
			}
			ClientNetwork::Instance()->getSender()->sendRanchBuildRemove(buildEntity->m_actorId);
			onClick(EventArgs(), ViewId::CLOSE);
			break;
		}
		}
		return true;
	}

	bool gui_ranchBuildFarm::onDataUpdate(i32 entityId)
	{
		if (isShown() && m_entityId == entityId)
		{
			if (!Blockman::Instance() || !Blockman::Instance()->m_pWorld)
			{
				return false;
			}
			auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
			if (!buildEntity)
			{
				return false;
			}
			dynamic_cast<GUIStaticImage*>(m_btnBuyQueue->GetChildByIndex(0))->SetImage(UICommon::getCurrencyIconByType((CurrencyType)buildEntity->m_queueUnlockCurrencyType).c_str());
			setText(m_title, buildEntity->getNameLang().c_str());
			initProductItem(buildEntity->m_products);
			initRecipeItem(buildEntity->m_recipes);
			initQueueItem(buildEntity->isCanWorking(), buildEntity->m_actorId, buildEntity->m_unlockQueues);
		}
		return true;
	}

	bool gui_ranchBuildFarm::onRecipeItemClick(const ProductRecipe & recipe)
	{
		if (!isShown() || recipe.productId == 0)
		{
			m_recipeInfoLayout->SetVisible(false);
			return false;
		}
		m_recipeInfoLayout->SetVisible(true);
		m_buildOperation->setRecipe(recipe);
		m_buildOperation->setEntityId(m_entityId);
		return false;
	}

	void gui_ranchBuildFarm::initProductItem(const vector<RanchCommon>::type& items)
	{
		m_productItems->ClearAllItem();
		int i = 0;
		for (auto item : items)
		{
			static unsigned productCount = 0;
			productCount++;
			i++;
			String itemLayout = StringUtil::Format("RanchBuildFarm-Product-Items-Item-%d", productCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				continue;
			}

			float w = m_productItems->GetPixelSize().x / 6;
			GuiRanchBuildProduct* itemView = (GuiRanchBuildProduct*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_PRODUCT, itemLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w));
			itemView->setProduct(item);
			itemView->setIndex(i);
			itemView->setEntityId(m_entityId);
			m_productItems->AddItem(itemView, false);
		}

	}

	void gui_ranchBuildFarm::initRecipeItem(const vector<ProductRecipe>::type& items)
	{
		m_recipeItems->ClearAllItem();
		for (auto item : items)
		{
			static unsigned recipeCount = 0;
			recipeCount++;
			String itemLayout = StringUtil::Format("RanchBuildFarm-Recipe-Items-Item-%d", recipeCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				continue;
			}

			GuiRanchBuildRecipes* ranchItemView = (GuiRanchBuildRecipes*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_RECIPES, itemLayout.c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 100.f), UDim(0.0f, 100.f));
			ranchItemView->setProductRecipe(item);

			m_recipeItems->AddItem(ranchItemView, false);
		}

		if (items.size() <= 3)
		{
			m_recipeLayout->SetArea(UDim(0, 0), UDim(0, 170.0f), UDim(0, 330.f), UDim(0.0f, 114.f));
		}
		else
		{
			float width = m_recipeItems->getContainerWindow()->GetPixelSize().x + 20.f;
			m_recipeLayout->SetArea(UDim(0, 0), UDim(0, 170.0f), UDim(0, width), UDim(0.0f, 114.f));
		}
		m_recipeItems->setContainerAlignment();
	}

	void gui_ranchBuildFarm::initQueueItem(bool isCanWorking, i32 actorId, const vector<ProductQueue>::type& items)
	{
		m_queueItems->ClearAllItem();
		i32 unlockQueueNum = 0;
		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
		for (auto item : items)
		{
			static unsigned queueCount = 0;
			queueCount++;
			String itemLayout = StringUtil::Format("RanchBuildFarm-Queue-Items-Item-%d", queueCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()) || item.state == QueueState::QUEUE_LOCK)
			{
				continue;
			}
			unlockQueueNum++;
			GuiRanchBuildFarmQueue* ranchItemView = (GuiRanchBuildFarmQueue*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_FARM_QUEUE, itemLayout.c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 140.f), UDim(0.0f, 246.f));
			ranchItemView->setProductQueue(item);
			ranchItemView->SetTouchable(false);
			ranchItemView->setEntityId(m_entityId);
			m_queueItems->AddItem(ranchItemView, false);
			m_queueItems->getContainerWindow()->SetTouchable(false);
		}
		m_btnBuyQueue->SetVisible(unlockQueueNum < buildEntity->m_maxQueueNum);
		m_queueItems->SetArea(UDim(0, 18.f), UDim(0, 71.0f), UDim(1.f, (unlockQueueNum < buildEntity->m_maxQueueNum ? -172.f : -36.f)), UDim(0.0f, 246.f));
		unlockQueueNum += 2;
		unlockQueueNum = (unlockQueueNum < buildEntity->m_maxQueueNum ? unlockQueueNum : buildEntity->m_maxQueueNum);
		float width = (140.f + 24.f) * unlockQueueNum + 36.f - 24.f;
		getWindow("RanchBuildFarm-Panel")->SetWidth(UDim(0, width));
		m_queueItems->RestChildVisible();
	}

	void gui_ranchBuildFarm::playGameVideo()
	{
		String cowKey = StringUtil::Format("isFirstOpenCow_%lld", GameClient::CGame::Instance()->getPlatformUserId());
		if (GameClient::CGame::Instance()->getShellInterface()->getBoolForKey(cowKey, true))
		{
			auto guideSetting = GuideSetting::getGuideVideoItem(2);
			if (guideSetting)
			{
				GameClient::CGame::Instance()->getShellInterface()->playVideo(guideSetting->type, GuideSetting::getGuideVideoPath(2));
				GameClient::CGame::Instance()->getShellInterface()->putBoolForKey(cowKey, false);
			}
		}
	}

}
