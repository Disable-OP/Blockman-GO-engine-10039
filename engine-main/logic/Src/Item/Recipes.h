/********************************************************************
filename: 	Recipes.h
file path:	dev\client\Src\Blockman\Item

version:	1
author:		ajohn
company:	supernano
date:		2017-2-24
*********************************************************************/
#ifndef __RECIPES_HEADER__
#define __RECIPES_HEADER__

#include "BM_Container_def.h"
#include "Core.h"
#include "Item/Ingredient.h"
using namespace LORD;

namespace BLOCKMAN
{

class ItemStack;
class InventoryCrafting;
class World;
class IInventory;
class InventoryPlayer;

typedef map<int, ItemStackPtr>::type	BlockItemStackMap;
typedef map<int, float>::type StackExperienceMap;


class IRecipe : public ObjectAlloc
{
private:
	static unsigned count;
	const int m_id;
	const String m_name;
	const String m_group;

public:
	IRecipe(const String& name) : m_id(count++), m_name(name), m_group() {}
	IRecipe(const String& name, const String& group) : m_id(count++), m_name(name), m_group(group) {}
    ~ IRecipe() {
        count = 0;
    }
    
	int getId() const
	{
		return m_id;
	}
	String getName() const
	{
		return m_name;
	}
	/** Used to check if a recipe matches current crafting inventory */
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld) = 0;
	/** Returns an Item that is the result of this recipe */
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting) = 0;
	/** Returns the size of the recipe area */
	virtual int getRecipeSize() const = 0;
	virtual ItemStackPtr getRecipeOutput() = 0;
};

class RecipeFireworks : public IRecipe
{
	using IRecipe::IRecipe;
private:
	ItemStackPtr resultItem = nullptr;

public:
	/** Implement override functions from IRecipe*/
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld);
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting);
	virtual int getRecipeSize() const { return 10; }
	virtual ItemStackPtr getRecipeOutput() { return resultItem; }
};

class RecipesArmorDyes : public IRecipe
{
	using IRecipe::IRecipe;
public:
	/** Implement override functions from IRecipe*/
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld);
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting);
	virtual int getRecipeSize() const { return 10; }
	virtual ItemStackPtr getRecipeOutput() { return NULL; }
};


class RecipesMapCloning : public IRecipe
{
	using IRecipe::IRecipe;
public:
	/** Implement override functions from IRecipe*/
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld);
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting);
	virtual int getRecipeSize() const { return 9; }
	virtual ItemStackPtr getRecipeOutput() { return NULL; }
};

class ShapedRecipes : public IRecipe
{
protected:
	/** How many horizontal slots this recipe is wide. */
	int recipeWidth = 0;
	/** How many vertical slots this recipe uses. */
	int recipeHeight = 0;
	/** Is a array of ItemStack that composes the recipe. */
	IngredientPtrArr recipeItems;
	/** Is the ItemStack that you get when craft the recipe. */
	ItemStackPtr recipeOutput = nullptr;
	bool copyIngredientNBT = false;

public:
	/** Is the itemID of the output item that you get when craft the recipe. */
	int recipeOutputItemID = 0;
	
protected:
	/** Checks if the region of a crafting inventory is match for the recipe. */
	bool checkMatch(InventoryCrafting* pInvCrafting, int xPos, int yPos, bool par4);

public:
	ShapedRecipes(int width, int height, const IngredientPtrArr& stacks, ItemStackPtr pStack, const String& name, const String& group = "");
	void setCopyIngredientNBT() { copyIngredientNBT = true; }

	/** Implement override functions from IRecipe*/
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld);
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting);
	virtual ItemStackPtr getRecipeOutput() { return recipeOutput; }
	virtual int getRecipeSize() const { return recipeWidth * recipeHeight; }
	bool needsItem(ItemStackPtr pStack);
	IngredientPtrArr getRecipeItems() { return recipeItems; }
	int getRecipeWidth() const { return recipeWidth; }
	int getRecipeHeight() const { return recipeHeight; }
};


class ShapelessRecipes : public IRecipe
{
protected:
	/** Is the ItemStack that you get when craft the recipe. */
	ItemStackPtr recipeOutput = nullptr;
	/** Is a List of ItemStack that composes the recipe. */
	IngredientPtrArr recipeItems;

public:
	ShapelessRecipes(ItemStackPtr pStack, const IngredientPtrArr& stacks, const String& name, const String& group = "");
	
	/** Implement override functions from IRecipe*/
	virtual ItemStackPtr getRecipeOutput() { return recipeOutput; }
	virtual bool matches(InventoryCrafting* pInvCrafting, World* pWorld);
	virtual ItemStackPtr getCraftingResult(InventoryCrafting* pInvCrafting);
	virtual int getRecipeSize() const { return recipeItems.size(); }
};


class FurnaceRecipes : public ObjectAlloc
{
protected:
	static FurnaceRecipes* smeltingBase;	// = new FurnaceRecipes();
	/** The list of smelting results. */
	BlockItemStackMap smeltingList;
	StackExperienceMap experienceList;

	FurnaceRecipes();
public:

	~FurnaceRecipes();
	/** Used to call methods addSmelting and getSmeltingResult. */
	static FurnaceRecipes* smelting();
	/** Adds a smelting recipe. */
	void addSmelting(int blockID, ItemStackPtr pStack, float exp);
	/** Returns the smelting result of an item. */
	ItemStackPtr getSmeltingResult(int blockID);
	BlockItemStackMap& getSmeltingList() { return smeltingList; }
	float getExperience(int itemID);
};

class CraftingRecipe
{
	friend class InventoryCraftable;
private:
	ShapedRecipes* m_recipes = nullptr;
	int m_craftingLimit = 0;
	vector<bool>::type m_materialsAvailable;

	void makeUnavailable();

public:
	explicit CraftingRecipe(ShapedRecipes* recipes)
		: m_recipes(recipes)
		, m_craftingLimit(recipes->getRecipeSize())
	{
		m_materialsAvailable.resize(m_craftingLimit);
	}
	int getId()
	{
		return m_recipes->getId();
	}
	int getRecipeSize() const
	{
		return m_recipes->getRecipeSize();
	}
	size_t getRecipeWidth() const
	{
		return m_recipes->getRecipeWidth();
	}
	size_t getRecipeHeight() const
	{
		return m_recipes->getRecipeHeight();
	}
	ItemStackPtr getRecipeOutput()
	{
		return m_recipes->getRecipeOutput();
	}

	decltype(m_recipes->getRecipeItems()) getRecipeItems()
	{
		return m_recipes->getRecipeItems();
	}

	int getLimit()
	{
		return m_craftingLimit;
	}

	bool materialsAvailable(size_t index)
	{
		return m_materialsAvailable[index];
	}
};

}

#endif
