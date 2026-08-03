#include "GuiRanchBuildOperation.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiRanchItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{

	void GuiRanchBuildOperation::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildOperation::getPrivateChildren()
	{
		return { m_window };
	}


	GuiRanchBuildOperation::GuiRanchBuildOperation(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildOperation.json"));
		renameComponents(m_window);

		m_btnClose = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_gridView = dynamic_cast<GUIGridView*>(m_window->GetChildByIndex(2));

		GUILayout * info = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_tvTime = dynamic_cast<GUIStaticText*>(info->GetChildByIndex(1));
		m_itemInventory = dynamic_cast<GUIStaticText*>(info->GetChildByIndex(3));

		m_btnSure = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));

		m_gridView->InitConfig(0.f, 0.f, 2);

		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildOperation::onClick, this, std::placeholders::_1, VIewId::BTN_SURE));
		m_btnClose->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildOperation::onClick, this, std::placeholders::_1, VIewId::BTN_CLOSE));
		if (LanguageManager::Instance())
		{
			m_btnSure->SetText(LanguageManager::Instance()->getString("gui_ranch_build_production").c_str());
		}

		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildOperation::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage() || !LanguageManager::Instance() || !IsVisible())
		{
			return;
		}
		m_itemInventory->SetText(StringUtil::ToString(Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->getItemNum(m_recipe.productId)).c_str());
	}


	void GuiRanchBuildOperation::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildOperation::setEntityId(i32 entityId)
	{

		auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(entityId));
		if (!buildEntity)
		{
			m_entityId = -1;
			return;
		}
		m_entityId = entityId;
	}

	void GuiRanchBuildOperation::setRecipe(const ProductRecipe & recipe)
	{
		m_recipe = recipe;
		m_tvName->SetText(LanguageManager::Instance()->getString(RanchSetting::getRanchItemName(m_recipe.productId)).c_str());
		m_tvTime->SetText(timeFormat(m_recipe.needTime).c_str());
		initItem();
	}

	bool GuiRanchBuildOperation::onClick(const EventArgs & args, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::BTN_CLOSE:
			RanchRecipeItemClickEvent::emit(ProductRecipe());
			break;

		case VIewId::BTN_SURE:

			RanchRecipeItemClickEvent::emit(ProductRecipe());
			if (m_entityId < 1 || m_recipe.productId < 1)
			{
				return false;
			}

			if (!UICommon::checkRanchLevel(m_recipe.needLevel))
			{
				return false;
			}

			auto buildEntity = dynamic_cast<EntityBuildNpc*>(Blockman::Instance()->m_pWorld->getEntity(m_entityId));
			if (!buildEntity)
			{
				return false;
			}

			if (!buildEntity->canMake())
			{
				if (buildEntity->canBuy())
				{
					ShowRanchQueueUnlockEvent::emit(m_entityId, 0, buildEntity->m_queueUnlockPrice, buildEntity->m_queueUnlockCurrencyType, "gui_ranch_build_queue_full_unlock_msg");
					return false;
				}
				ShowRanchCommonTipEvent::emit("gui_ranch_build_queue_full");
				return false;
			}
			auto newItems = UICommon::getLackRanchItems(m_recipe.items);
			if (newItems.size() > 0)
			{
				ShowRanchLockItemsByBuildOperationEvent::emit(newItems, m_entityId, m_recipe.productId);
				return false;
			}
			ClientNetwork::Instance()->getSender()->sendRanchBuildQueueOperation(1, m_entityId, 0, m_recipe.productId);
			break;
		}
		return true;
	}

	void GuiRanchBuildOperation::initItem()
	{
		m_gridView->RemoveAllItems();
		if (m_recipe.items.size() > 0 && m_recipe.items.size() < 3)
		{
			m_gridView->InitConfig(0.f, 0.f, 1);
			m_gridView->SetArea(UDim(0, 0), UDim(0, m_recipe.items.size() == 1 ? 110.f : 70.f), UDim(0, 180.f), UDim(0, 80.f * m_recipe.items.size()));
		}
		else 
		{
			m_gridView->InitConfig(40.f, 0.f, 2);
			m_gridView->SetArea(UDim(0, 0), UDim(0, 70.f), UDim(0, 400.f), UDim(0, 160.f));
		}
		for (auto item : m_recipe.items)
		{
			static unsigned recipeCount = 0;
			recipeCount++;
			String itemLayout = StringUtil::Format("RanchBuildOperation-Items-Item-%d", recipeCount).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchItem* ranchItemView = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, itemLayout.c_str());
			ranchItemView->resetWindow("RanchRecipeItem.json");
			ranchItemView->setNum(getNum(item.itemId, item.num).c_str());
			auto ranchItem = RanchSetting::getRanchItem(item.itemId);
			ranchItemView->setIcon(ranchItem ? ranchItem->icon.c_str() : "");
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 180.f), UDim(0.0f, 80.f));
			m_gridView->AddItem(ranchItemView);
		}

	}

	String GuiRanchBuildOperation::getNum(i32 itemId, i32 needNum)
	{
		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return  StringUtil::Format("▢FFFF0020%d▢FF69523b/%d", 0, needNum);
		}
		i32 num = Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->getItemNum(itemId);
		if (num >= needNum)
		{
			return StringUtil::Format("▢FF66ff00%d▢FF69523b/%d", num, needNum);
		}

		return StringUtil::Format("▢FFFF0020%d▢FF69523b/%d", num, needNum);
	}

	String GuiRanchBuildOperation::timeFormat(i32 needTime)
	{
		String h = StringUtil::ToString((needTime / 1000 / 60 / 60));
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString((needTime / 1000 / 60 % 60));
		m = (m.length() == 1 ? ("0" + m) : m);

		String s = StringUtil::ToString((needTime / 1000 % 60));
		s = (s.length() == 1 ? ("0" + s) : s);

		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}

}