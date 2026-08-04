#include "gui_ranchBuildFactory.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiRanchItem.h"
#include "GUI/GuiRanchBuildRecipes.h"
#include "GUI/GuiRanchBuildProduct.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiDef.h"
#include "GUI/GuiRanchBuildFactoryQueue.h"
#include "Network/ClientNetwork.h"
#include "GUI/RootGuiLayout.h"
#include "Inventory/Wallet.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchBuildFactory::gui_ranchBuildFactory()
		: gui_layout("RanchBuildFactory.json")
	{
	}

	gui_ranchBuildFactory::~gui_ranchBuildFactory()
	{
	}

	void gui_ranchBuildFactory::onLoad()
	{
		m_title = getWindow<GUIStaticText>("RanchBuildFactory-Title");
		m_recipeItems = getWindow<GUIListHorizontalBox>("RanchBuildFactory-Recipe-Items");
		m_productItems = getWindow<GUIListHorizontalBox>("RanchBuildFactory-Product-Items");
		m_queueItems = getWindow<GUIListHorizontalBox>("RanchBuildFactory-Queue-Items");

		m_recipeLayout = getWindow<GUILayout>("RanchBuildFactory-Recipe");
		m_recipeInfoLayout = getWindow<GUILayout>("RanchBuildFactory-RecipeInfo");
		m_buildOperation = getWindow<GuiRanchBuildOperation>("RanchBuildFactory-Operation");


		m_leftTimeLayout = getWindow<GUILayout>("RanchBuildFactory-LeftTime");
		m_buyTimeLayout = getWindow<GUILayout>("RanchBuildFactory-BuyTime");
		m_pbTimeBar = getWindow<GUIProgressBar>("RanchBuildFactory-TimeBar");
		m_tvPriceValue = getWindow<GUIStaticText>("RanchBuildFactory-PriceValue");

		m_btnBuyQueue = getWindow<GUIButton>("RanchBuildFactory-Btn-Buy-Queue");

		m_recipeInfoLayout->SetVisible(false);
		m_queueItems->SetInterval(24.f);
		m_recipeItems->SetInterval(5.f);
		m_productItems->SetInterval(0.f);
		m_recipeItems->SetTouchable(true);

		m_btnBuyQueue->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::UNLOCK));
		m_recipeInfoLayout->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::MAIN));
		m_buyTimeLayout->GetChildByIndex(0)->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::BUY_TIME));
		getWindow("RanchBuildFactory")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::MAIN));
		getWindow("RanchBuildFactory-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("RanchBuildFactory-Btn-Remove")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchBuildFactory::onClick, this, std::placeholders::_1, ViewId::BTN_REMOVE));

		m_subscriptionGuard.add(ShowRanchBuildFactoryEvent::subscribe(std::bind(&gui_ranchBuildFactory::showGui, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RanchBuildFactoryUpdateEvent::subscribe(std::bind(&gui_ranchBuildFactory::onDataUpdate, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RanchRecipeItemClickEvent::subscribe(std::bind(&gui_ranchBuildFactory::onRecipeItemClick, this, std::placeholders::_1)));

	}

	void gui_ranchBuildFactory::onShow()
	{
		m_isSecondClose = false;
		m_recipeInfoLayout->SetVisible(false);
	}

	void gui_ranchBuildFactory::onHide()
	{
		m_isSecondClose = false;
		m_recipeInfoLayout->SetVisible(false);
	}

	void gui_ranchBuildFactory::onUpdate(ui32 nTimeElapse)
	{

		if (!isShown() || !Blockman::Instance() || !Blockman::Instance()->m_pWorld || !Blockman::Instance()->m_pPlayer->m_ranch)
		{
			return;
		}
		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
		if (!buildEntity)
		{
			return;
		}

		if (m_workingQueue.productId != 0)
		{
			switch (m_workingQueue.state)
			{
			case QueueState::QUEUE_WORKING: {
				m_pbTimeBar->SetVisible(true);
				m_buyTimeLayout->GetChildByIndex(0)->SetVisible(true);
				m_workingQueue.timeLeft -= nTimeElapse;
				m_workingQueue.timeLeft = (m_workingQueue.timeLeft < 0 ? 0 : m_workingQueue.timeLeft);
				m_pbTimeBar->GetChildByIndex(0)->SetText(timeFormat().c_str());
				float pb = m_workingQueue.timeLeft * 1.0f / m_workingQueue.needTime * 1.0f;
				m_pbTimeBar->SetProgress(1.f - pb);
				m_tvPriceValue->SetText(StringUtil::ToString(Blockman::Instance()->m_pPlayer->m_ranch->getItemTimePrice(m_workingQueue.timeLeft / 1000, m_workingQueue.productId)).c_str());
			}
				break;
			case QueueState::QUEUE_AWAIT: {
				m_pbTimeBar->SetVisible(true);
				m_buyTimeLayout->GetChildByIndex(0)->SetVisible(false);
				m_pbTimeBar->GetChildByIndex(0)->SetText(timeFormat().c_str());
				float pb = m_workingQueue.timeLeft * 1.0f / m_workingQueue.needTime * 1.0f;
				m_pbTimeBar->SetProgress(1.f - pb);
			}
				break;
			default:
				m_pbTimeBar->SetVisible(false);
				m_buyTimeLayout->GetChildByIndex(0)->SetVisible(false);
				break;
			}
		}
		else
		{
			m_pbTimeBar->SetVisible(false);
			m_buyTimeLayout->GetChildByIndex(0)->SetVisible(false);
		}
	}

	bool gui_ranchBuildFactory::showGui(i32 entityId)
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
		RootGuiLayout::Instance()->showRanchBuildFactory();
		initProductItem(buildEntity->m_products);
		initRecipeItem(buildEntity->m_recipes);
		initQueueItem(buildEntity->isCanWorking(), buildEntity->m_actorId, buildEntity->m_unlockQueues);
		dynamic_cast<GUIStaticImage*>(m_btnBuyQueue->GetChildByIndex(0))->SetImage(UICommon::getCurrencyIconByType((CurrencyType)buildEntity->m_queueUnlockCurrencyType).c_str());
		return false;
	}


	bool gui_ranchBuildFactory::onClick(const EventArgs & events, ViewId viewId)
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
		case ViewId::BUY_TIME:
			if (m_entityId > 0 && m_workingQueue.productId > 0 && m_workingQueue.timeLeft > 0)
			{
				ShowSpeedUpQueueEvent::emit(m_entityId, m_workingQueue.queueId, m_workingQueue.productId, m_workingQueue.needTime, m_workingQueue.timeLeft);
			}
			break;
		}
		return true;
	}

	bool gui_ranchBuildFactory::onDataUpdate(i32 entityId)
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

			setText(m_title, buildEntity->getNameLang().c_str());
			initProductItem(buildEntity->m_products);
			initRecipeItem(buildEntity->m_recipes);
			initQueueItem(buildEntity->isCanWorking(), buildEntity->m_actorId, buildEntity->m_unlockQueues);
			dynamic_cast<GUIStaticImage*>(m_btnBuyQueue->GetChildByIndex(0))->SetImage(UICommon::getCurrencyIconByType((CurrencyType)buildEntity->m_queueUnlockCurrencyType).c_str());
		}
		return true;
	}

	bool gui_ranchBuildFactory::onRecipeItemClick(const ProductRecipe & recipe)
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

	void gui_ranchBuildFactory::initProductItem(const vector<RanchCommon>::type& items)
	{
		m_productItems->ClearAllItem();
		int i = 0;
		for (auto item : items)
		{
			static unsigned productCount = 0;
			productCount++;
			i++;
			String itemLayout = StringUtil::Format("RanchBuildFactory-Product-Items-Item-%d", productCount).c_str();
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

	void gui_ranchBuildFactory::initRecipeItem(const vector<ProductRecipe>::type& items)
	{
		m_recipeItems->ClearAllItem();
		for (auto item : items)
		{
			static unsigned recipeCount = 0;
			recipeCount++;
			String itemLayout = StringUtil::Format("RanchBuildFactory-Recipe-Items-Item-%d", recipeCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchBuildRecipes* ranchItemView = (GuiRanchBuildRecipes*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_RECIPES, itemLayout.c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 100.f), UDim(0.0f, 100.f));
			ranchItemView->setProductRecipe(item);
			m_recipeItems->AddItem(ranchItemView, false);
		}

		if (items.size() <= 3)
		{
			m_recipeLayout->SetArea(UDim(0, 0), UDim(0, 170.0f), UDim(1.f, -160.f), UDim(0.0f, 114.f));
		}
		else
		{
			float width = m_recipeItems->getContainerWindow()->GetPixelSize().x + 20.f;
			m_recipeLayout->SetArea(UDim(0, 0), UDim(0, 170.0f), UDim(0, width), UDim(0.0f, 114.f));
		}
		m_recipeItems->setContainerAlignment();
	}

	void gui_ranchBuildFactory::initQueueItem(bool isCanWorking, i32 actorId, const vector<ProductQueue>::type& items)
	{
		static unsigned queueCount = 0;
		m_queueItems->ClearAllItem();

		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
		if (buildEntity->isWoring())
		{
			m_workingQueue = items[0];
		}
		else
		{
			queueCount++;
			m_workingQueue = ProductQueue();
			String itemLayout = StringUtil::Format("RanchBuildFactory-Queue-Items-Item-%d", queueCount).c_str();
			GUILayout* layout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, itemLayout.c_str());
			layout->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 140.f), UDim(0.0f, 174.f));
			layout->SetTouchable(false);
			m_queueItems->AddItem(layout, false);
		}
		i32 unlockQueueNum = 0;
		for (auto item : items)
		{
			queueCount++;
			String itemLayout = StringUtil::Format("RanchBuildFactory-Queue-Items-Item-%d", queueCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()) || item.state == QueueState::QUEUE_LOCK)
			{
				continue;
			}
			unlockQueueNum++;
			GuiRanchBuildFactoryQueue* ranchItemView = (GuiRanchBuildFactoryQueue*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_FACTORY_QUEUE, itemLayout.c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 140.f), UDim(0.0f, 174.f));
			ranchItemView->setProductQueue(item);
			ranchItemView->SetTouchable(false);
			m_queueItems->AddItem(ranchItemView, false);
			m_queueItems->getContainerWindow()->SetTouchable(false);
		}
		m_btnBuyQueue->SetVisible(unlockQueueNum < buildEntity->m_maxQueueNum);
		unlockQueueNum += 2;
		m_queueItems->SetArea(UDim(0, 18.f), UDim(0, 89.0f), UDim(1.f, (unlockQueueNum < buildEntity->m_maxQueueNum ? -172.f : -36.f)), UDim(0.0f, 174.f));
		unlockQueueNum = (unlockQueueNum >( buildEntity->m_maxQueueNum + 1) ? (buildEntity->m_maxQueueNum + 1) : unlockQueueNum);
		float width = (140.f + 24.f) * unlockQueueNum + 36.f - 24.f;
		getWindow("RanchBuildFactory-Panel")->SetWidth(UDim(0, width));
		m_queueItems->RestChildVisible();

	}

	String gui_ranchBuildFactory::timeFormat()
	{
		String h = StringUtil::ToString(m_workingQueue.timeLeft / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(m_workingQueue.timeLeft / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(m_workingQueue.timeLeft / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}

}
