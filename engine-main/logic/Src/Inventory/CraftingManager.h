/********************************************************************
filename: 	CraftingManager.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-02-28
*********************************************************************/
#ifndef __CRAFTING_MANAGER_HEADER__
#define __CRAFTING_MANAGER_HEADER__

#include "BM_Container_def.h"
#include "Core.h"
#include "rapidjson/document.h"

using namespace LORD;

namespace BLOCKMAN
{

class IRecipe;
class ShapedRecipes;
class Item;
class ItemStack;
class Block;
class World;
class InventoryCrafting;

class CraftingManager : public Singleton<CraftingManager>, public ObjectAlloc
{
public:
	typedef vector<IRecipe*>::type RecipeList;

protected:
	/** A list of all the recipes added */
	RecipeList recipes;
	ShapedRecipes* addShapedRecipe(ItemStackPtr pStack, const StringArray& recipeStrs, map<char, IngredientPtr>::type& charToIngredient, const String& name, const String& group = "");
	void addShapelessRecipe(ItemStackPtr pStack, const IngredientPtrArr& ingredients, const String& name, const String& group = "");
	ItemStackPtr createItemStackFromJson(const rapidjson::Value& itemStackJson);
	IngredientPtr createIngredientFromJson(const rapidjson::Value& ingredientJson);

public:
	CraftingManager();
	virtual ~CraftingManager();
	ItemStackPtr findMatchingRecipe(InventoryCrafting* pInvCrafting, World* pWorld);
	/** returns the recipes */
	RecipeList& getRecipeList() { return recipes; }
	IRecipe* getRecipe(int recipeId)
	{
		return recipes[recipeId];
	}
	vector<String>::type getRecipeNameList();
	bool loadRecipe(const String& recipeName);
	bool parseRecipeJson(const String& recipeName, const rapidjson::Document& recipeJson);
};


}

#endif