#include "ItemCrafter.h"
#include "Item/Recipes.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Util/CommonEvents.h"
#include "Entity/EntityPlayer.h"
#include "Inventory/CraftingManager.h"

namespace BLOCKMAN
{
	// FIX [SYMPTOM-3]: ingredient access generalized to any recipe type so
	// shapeless recipes can be consumed too (previously ShapedRecipes-only).
	static IngredientPtrArr getRecipeIngredients(ShapedRecipes* recipe)
	{
		return recipe->getRecipeItems();
	}

	static IngredientPtrArr getRecipeIngredients(ShapelessRecipes* recipe)
	{
		return recipe->getRecipeItems();
	}

	bool ItemCrafter::tryConsumeItemsFromInventory(IRecipe * recipe, IInventory * inventory)
	{
		auto size = InventoryPlayer::MAIN_INVENTORY_COUNT;
		vector<int>::type usage(size);

		IngredientPtrArr ingredients;
		if (auto shaped = dynamic_cast<ShapedRecipes*>(recipe))
			ingredients = getRecipeIngredients(shaped);
		else if (auto shapeless = dynamic_cast<ShapelessRecipes*>(recipe))
			ingredients = getRecipeIngredients(shapeless);
		else
			return false;

		for (auto ingredient : ingredients)
		{
			if (!ingredient)
			{
				continue;
			}
			bool found = false;
			for (int i = 0; i < size; ++i)
			{
				auto targetStack = inventory->getStackInSlot(i);
				if (targetStack && ingredient->matches(targetStack) && targetStack->stackSize > usage[i])
				{
					++usage[i];
					found = true;
					break;
				}
			}
			if (!found)
			{
				return false;
			}
		}
		for (int i = 0; i < size; ++i)
		{
			inventory->decrStackSize(i, usage[i]);
		}
		return true;
	}

	bool ItemCrafter::craftWithinInventory(IRecipe * recipe, InventoryPlayer * inventory)
	{
		if (!recipe->getRecipeOutput() || !inventory->canItemStackAddedToInventory(recipe->getRecipeOutput()))
		{
			return false;
		}
		if (!tryConsumeItemsFromInventory(recipe, inventory))
		{
			return false;
		}
		auto itemStack = recipe->getRecipeOutput()->copy();
		int old_size = itemStack->stackSize;
		LordAssert(inventory->addItemStackToInventory(itemStack));
		LordAssert(old_size != itemStack->stackSize);
		return true;
	}

	bool ItemCrafter::craftItem(EntityPlayer* player, int recipeId)
	{
		// FIX [SYMPTOM-3]: accept both shaped AND shapeless recipes (was
		// dynamic_cast<ShapedRecipes*> only — every shapeless craft failed).
		auto recipe = CraftingManager::Instance()->getRecipe(recipeId);
		if (recipe)
		{
			if (dynamic_cast<ShapedRecipes*>(recipe) || dynamic_cast<ShapelessRecipes*>(recipe))
		{
			if (player)
			{
				return craftWithinInventory(recipe, player->inventory);
			}
			else
			{
				LordLogError("r ItemCrafter::craftItem player is null,please check");
				return false;
			}
		}
		else
		{
				LordLogError("recipe with id %d is neither ShapedRecipe nor ShapelessRecipe", recipeId);
				return false;
			}
		}
		else
		{
			LordLogError("recipe with id %d not found", recipeId);
			return false;
		}
	}
}