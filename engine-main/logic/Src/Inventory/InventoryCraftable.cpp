#include "InventoryCraftable.h"
#include <climits>
#include <algorithm>
#include "Inventory/CraftingManager.h"
#include "Item/ItemStack.h"
#include <Item/Recipes.h>

namespace BLOCKMAN
{
	set<ShapedRecipes*>::type InventoryCraftable::calculateAvailableShapedRecipes()
	{
		set<ItemStackPtr>::type availableMaterials;
		set<ShapedRecipes*>::type availableRecipes;
		for (int i = 0; i < m_inventory->getSizeInventory(); ++i)
		{
			availableMaterials.insert(m_inventory->getStackInSlot(i));
		}
		while (!availableMaterials.empty())
		{
			ItemStackPtr material = *availableMaterials.begin();
			availableMaterials.erase(availableMaterials.begin());
			if (!material)
			{
				continue;
			}
			for (auto recipe : CraftingManager::Instance()->getRecipeList())
			{
				auto shapedRecipe = dynamic_cast<ShapedRecipes*>(recipe);
				if (!shapedRecipe || !shapedRecipe->needsItem(material))
				{
					continue;
				}
				if (availableRecipes.find(shapedRecipe) == availableRecipes.end())
				{
					availableMaterials.insert(shapedRecipe->getRecipeOutput());
					availableRecipes.insert(shapedRecipe);
				}
			}
		}
		return availableRecipes;
	}

	void InventoryCraftable::updateAvailableRecipes()
	{
		auto availableShapedRecipes = calculateAvailableShapedRecipes();
		for (auto craftingRecipe : m_availableRecipes)
		{
			if (availableShapedRecipes.find(craftingRecipe->m_recipes) == availableShapedRecipes.end())
			{
				craftingRecipe->makeUnavailable();
			}
		}
		vector<std::shared_ptr<CraftingRecipe>>::type new_m_availablerecipes;
		new_m_availablerecipes.reserve(m_availableRecipes.size());
		for (auto shapedRecipe : availableShapedRecipes)
		{
			bool recipeFound = false;
			for (auto craftingRecipe : m_availableRecipes)
			{
				if (craftingRecipe->m_recipes == shapedRecipe)
				{
					new_m_availablerecipes.push_back(craftingRecipe);
					recipeFound = true;
					break;
				}
			}
			if (!recipeFound)
			{
				new_m_availablerecipes.push_back(LORD::make_shared<CraftingRecipe>(shapedRecipe));
			}
		}
		m_availableRecipes = std::move(new_m_availablerecipes);
	}

	void InventoryCraftable::updateCraftableItemStacks()
	{
		m_craftableItems.clear();
		for (auto recipe : m_availableRecipes)
		{
			std::shared_ptr<ItemStack> craftable(recipe->getRecipeOutput()->copy());
			int craftingLimit = INT_MAX;
			map<IngredientPtr, int>::type requiredItemNumbers;
			for (auto ingredient : recipe->getRecipeItems())
			{
				if (ingredient)
				{
					++requiredItemNumbers[ingredient];
				}
			}
			int counter = 0;
			for (auto pair : requiredItemNumbers)
			{
				int availableMaterialNum = 0;
				auto size = m_inventory->getSizeInventory();
				for (int i = 0; i < size; ++i)
				{
					auto itemInInventory = m_inventory->getStackInSlot(i);
					if (pair.first->matches(itemInInventory))
					{
						availableMaterialNum += itemInInventory->stackSize;
					}
				}
				using std::min;
				craftingLimit = min(craftingLimit, availableMaterialNum / pair.second);

				auto requiredIngredients = recipe->getRecipeItems();
				for (size_t i = 0; i < requiredIngredients.size(); ++i)
				{
					if (requiredIngredients[i] && requiredIngredients[i] == pair.first)
					{
						recipe->m_materialsAvailable[i] = availableMaterialNum-- > 0;
					}
				}
			}
			craftable->stackSize = craftingLimit;
			recipe->m_craftingLimit = craftingLimit;
			m_craftableItems.push_back(craftable);
		}
	}

	InventoryCraftable::InventoryCraftable(IInventory * inventory)
		: InventoryProxy(inventory)
	{
		onUpdate();
	}

	void InventoryCraftable::onUpdate()
	{
		updateAvailableRecipes();
		updateCraftableItemStacks();
	}
}
