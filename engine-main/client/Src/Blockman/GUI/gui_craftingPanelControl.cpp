#include "gui_craftingPanelControl.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/gui_playerInventoryControl.h"
#include "game.h"

#include "Item/Recipes.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryCraftable.h"

#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Util/StringUtil.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	gui_craftingPanelControl::gui_craftingPanelControl()
		: gui_layout("CraftingPanel.json")
	{
	}

	void gui_craftingPanelControl::onLoad()
	{
		m_recipes = getWindow<GuiScrollableInventory>("CraftingPanel-Recipes");
		m_craftTable = getWindow<GuiSlotTable>("CraftingPanel-Materials");
		m_outputName = getWindow<GUIStaticText>("CraftingPanel-OutputName");
		m_craftButton = getWindow<GUIButton>("CraftingPanel-CraftButton");
		m_craftButtonText = getWindow<GUIStaticText>("CraftingPanel-CraftButton-Text");
		m_craftButtonIcon = getWindow<GuiItemStack>("CraftingPanel-CraftButton-Icon");

		getWindow<GUIStaticText>("CraftingPanel-Title")->SetText(LanguageManager::Instance()->getString(LanguageKey::WORDART_WORKBENCH).c_str());
		m_craftButton->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_PANEL_MAKE).c_str());
		m_recipes->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_craftingPanelControl::onItemStackTouchUp, this));
		m_craftButton->subscribeEvent("ButtonClick", SubscriberSlot(&gui_craftingPanelControl::onCraftButtonClick, this));
	}

	void gui_craftingPanelControl::onHide()
	{
		m_selectedRecipe = nullptr;
	}

	void gui_craftingPanelControl::onUpdate(ui32 nTimeElapse)
	{
		updateCraftTableSize(nTimeElapse);
		updateCraftTableContent();
		updateRecipes(nTimeElapse);
		updateCraftButton();
		updateOutputName();
	}

	bool gui_craftingPanelControl::onItemStackTouchUp(const EventArgs & event)
	{
		auto& touchEvent = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event);
		InventoryCraftable* inventory;
		if (m_craftTableSize == 2)
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryHandCraftable();
		}
		else
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryAllCraftable();
		}
		m_selectedRecipe = inventory->getRecipe(touchEvent.index);

		ItemStackPtr touchStack = inventory->getStackInSlot(touchEvent.index);
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		return true;
	}

	bool gui_craftingPanelControl::onCraftButtonClick(const EventArgs & event)
	{
		if (m_selectedRecipe)
		{
			if (m_craftTableSize == 2)
			{
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendCraftWithHand(m_selectedRecipe->getId());
			}
			else
			{
				auto pakcetSender = GameClient::CGame::Instance()->getNetwork()->getSender();
				auto craftingTablePosition = Blockman::Instance()->m_pPlayer->getActivatedBlock();
				pakcetSender->sendCraftWithCraftingTable(m_selectedRecipe->getId(), craftingTablePosition);
			}
			//playSonud("random.click");
			playSoundByType(ST_Click);
		}
		return true;
	}

	inline gui_playerInventoryControl * gui_craftingPanelControl::getParent()
	{
		return dynamic_cast<gui_playerInventoryControl*>(m_parentLayout);
	}

	void gui_craftingPanelControl::updateCraftTableSize(ui32 nTimeElapse)
	{
		m_craftTableSize = getParent()->isInCraftingTable() ? 3 : 2;
		m_craftTable->setTableSize(m_craftTableSize, m_craftTableSize);
		m_craftTable->Update(nTimeElapse);
	}

	void gui_craftingPanelControl::updateCraftTableContent()
	{
		if (m_selectedRecipe)
		{
			auto materials = m_selectedRecipe->getRecipeItems();
			for (size_t i = 0, counter = 0; i < m_craftTableSize; ++i)
			{
				for (size_t j = 0; j < m_craftTableSize; ++j)
				{
					if (counter < materials.size() 
						&& i < m_selectedRecipe->getRecipeHeight()
						&& j < m_selectedRecipe->getRecipeWidth())
					{
						m_craftTable->setIngredient(i, j, materials[counter]);
						m_craftTable->setGrayedOut(i, j, !m_selectedRecipe->materialsAvailable(counter));
						++counter;
					}
					else
					{
						m_craftTable->setItemStack(i, j, nullptr);
					}
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_craftTableSize; ++i)
			{
				for (size_t j = 0; j < m_craftTableSize; ++j)
				{
					m_craftTable->setItemStack(i, j, nullptr);
				}
			}
		}
	}

	void gui_craftingPanelControl::updateRecipes(ui32 nTimeElapse)
	{
		InventoryCraftable* inventory;
		if (m_craftTableSize == 2)
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryHandCraftable();
		}
		else
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryAllCraftable();
		}
		m_recipes->setInventory(inventory);
		m_recipes->Update(nTimeElapse);
		auto size = inventory->getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			m_recipes->setGrayedOut(i, !inventory->getStackInSlot(i)->stackSize);
		}
	}

	void gui_craftingPanelControl::updateCraftButton()
	{
		if (m_selectedRecipe)
		{
			auto craftingOutput = m_selectedRecipe->getRecipeOutput();
			m_craftButtonText->SetText((StringUtil::ToString(craftingOutput->stackSize) + " X").c_str());
			m_craftButtonIcon->setItemStack(craftingOutput);
			if (m_selectedRecipe->getLimit() > 0)
			{
				m_craftButton->SetEnabled(true);
				m_craftButtonIcon->setGrayedOut(false);
			}
			else
			{
				m_craftButton->SetEnabled(false);
				m_craftButtonIcon->setGrayedOut(true);
			}
		}
		else
		{
			m_craftButtonText->SetText("");
			m_craftButtonIcon->setItemStack(nullptr);
			m_craftButton->SetEnabled(false);
		}
	}

	void gui_craftingPanelControl::updateOutputName()
	{
		if (m_selectedRecipe)
		{
			auto craftingOutput = m_selectedRecipe->getRecipeOutput();
			String  itemName = LanguageManager::Instance()->getItemName(craftingOutput->getItemName()).c_str();
			m_outputName->SetText(itemName.c_str());
		}
		else
		{
			m_outputName->SetText("");
		}
	}
}
