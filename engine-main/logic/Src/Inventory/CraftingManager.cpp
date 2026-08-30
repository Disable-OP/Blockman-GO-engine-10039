#include "CraftingManager.h"
#include "InventoryCrafting.h"
#include "Block/BlockManager.h"

#include "Item/Recipes.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Block/Block.h"
#include "Util/ContainerUtils.h"
#include "Object/Root.h"
#include "Item/RecipeJsonValidator.h"
#include "rapidjson/schema.h"
#include "rapidjson/istreamwrapper.h"

#define RETURN_ON_FAIL(expr, msg, ...) if (!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace BLOCKMAN
{

CraftingManager::CraftingManager()
{
}

CraftingManager::~CraftingManager()
{
	for (auto it : recipes) {
        LordSafeDelete(it);
	}
	recipes.clear();
}

vector<String>::type CraftingManager::getRecipeNameList()
{
	vector<String>::type ret;
	for (const auto& recipe : recipes)
	{
		ret.push_back(recipe->getName());
	}
	return ret;
}

ShapedRecipes* CraftingManager::addShapedRecipe(ItemStackPtr pStack, const StringArray& patternStrings, map<char, IngredientPtr>::type& charToIngredient, const String& name, const String& group)
{
	String recipeStr;
	size_t width = 0;
	size_t height = 0;

	for (int i = 0; i < int(patternStrings.size()); ++i)
	{
		recipeStr += patternStrings[i];
		width = (std::max)(width, patternStrings[i].length());
		height++;
	}

	IngredientPtrArr ingredients;
	for (auto ch : recipeStr)
	{
		auto it = charToIngredient.find(ch);
		if (it != charToIngredient.end())
		{
			ingredients.push_back(it->second);
		}
		else
		{
			ingredients.push_back(nullptr);
		}
	}

	ShapedRecipes* pShapedRecipes = LordNew ShapedRecipes(width, height, ingredients, pStack, name, group);
	element_at(recipes, pShapedRecipes->getId()) = pShapedRecipes;
	return pShapedRecipes;
}

void CraftingManager::addShapelessRecipe(ItemStackPtr pStack, const IngredientPtrArr& ingredients, const String& name, const String& group)
{
	auto pRecipe = LordNew ShapelessRecipes(pStack, ingredients, name, group);
	element_at(recipes, pRecipe->getId()) = pRecipe;
}

bool CraftingManager::loadRecipe(const String & recipeName)
{
	String recipePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "recipe", recipeName + ".json");
	RETURN_ON_FAIL(PathUtil::IsFileExist(recipePath), "failed to load recipe %s: json file does not exist: %s", recipeName.c_str(), recipePath.c_str());
	std::ifstream ifs(recipePath.c_str());
	rapidjson::IStreamWrapper is(ifs);
	rapidjson::Document doc;
	RETURN_ON_FAIL(!doc.ParseStream(is).HasParseError(), "failed to load recipe %s: failed to parse json", recipeName.c_str());
	return parseRecipeJson(recipeName, doc);
}

bool CraftingManager::parseRecipeJson(const String& recipeName, const rapidjson::Document & recipeJson)
{
	RETURN_ON_FAIL(validateRecipeJson(recipeJson), "failed to load recipe %s: recipe json is invalid", recipeName.c_str());
	String type = recipeJson["type"].GetString();
	if (type == "crafting_shaped")
	{
		StringArray patternStrings;
		map<char, IngredientPtr>::type charToIngredient;
		for (const auto& line : recipeJson["pattern"].GetArray())
		{
			patternStrings.push_back(line.GetString());
		}
		for (const auto& pair : recipeJson["key"].GetObject())
		{
			auto ingredient = createIngredientFromJson(pair.value);
			RETURN_ON_FAIL(ingredient, "failed to load recipe %s: failed to create Ingredients", recipeName.c_str());
			char ch = pair.name.GetString()[0];
			charToIngredient[ch] = ingredient;
		}
		String recipeGroup;
		if (recipeJson.HasMember("group"))
		{
			recipeGroup = recipeJson["group"].GetString();
		}
		auto itemStack = createItemStackFromJson(recipeJson["result"]);
		RETURN_ON_FAIL(itemStack, "failed to load recipe %s: failed to create ItemStack for result", recipeName.c_str());
		addShapedRecipe(itemStack, patternStrings, charToIngredient, recipeName, recipeGroup);
	}
	else if (type == "crafting_shapeless")
	{
		// FIX [SYMPTOM-3]: the shapeless branch was never implemented here —
		// every crafting_shapeless json fell into the "unsupported" error even
		// though addShapelessRecipe() and ShapelessRecipes existed.
		IngredientPtrArr ingredients;
		for (const auto& ingredientJson : recipeJson["ingredients"].GetArray())
		{
			auto ingredient = createIngredientFromJson(ingredientJson);
			RETURN_ON_FAIL(ingredient, "failed to load recipe %s: failed to create Ingredients", recipeName.c_str());
			ingredients.push_back(ingredient);
		}
		String recipeGroup;
		if (recipeJson.HasMember("group"))
		{
			recipeGroup = recipeJson["group"].GetString();
		}
		auto itemStack = createItemStackFromJson(recipeJson["result"]);
		RETURN_ON_FAIL(itemStack, "failed to load recipe %s: failed to create ItemStack for result", recipeName.c_str());
		addShapelessRecipe(itemStack, ingredients, recipeName, recipeGroup);
	}
	else
	{
		LordLogError("failed to load recipe %s: recipe type %s is unsupported", recipeName.c_str(), type.c_str());
		return false;
	}
	return true;
}

IngredientPtr CraftingManager::createIngredientFromJson(const rapidjson::Value & json)
{
	vector<int>::type itemDataList;
	int itemId;
	if (json.IsObject())
	{
		auto item = Item::getItemByName(json["item"].GetString());
		if (!item)
		{
			LordLogError("unknown item type: %s", json["item"].GetString());
			return nullptr;
		}
		itemId = item->itemID;
		if (json.HasMember("data"))
		{
			itemDataList.push_back(json["data"].GetInt());
		}
		else
		{
			itemDataList.push_back(0);
		}
	}
	else
	{
		auto item = Item::getItemByName(json[0]["item"].GetString());
		if (!item)
		{
			LordLogError("unknown item type: %s", json[0]["item"].GetString());
			return nullptr;
		}
		itemId = item->itemID;
		for (const auto& ingredientJson : json.GetArray())
		{
			if (ingredientJson.HasMember("data"))
			{
				itemDataList.push_back(ingredientJson["data"].GetInt());
			}
			else
			{
				itemDataList.push_back(0);
			}
		}
	}

	return LORD::make_shared<Ingredient>(itemId, itemDataList);
}

ItemStackPtr CraftingManager::createItemStackFromJson(const rapidjson::Value & json)
{
	auto item = Item::getItemByName(json["item"].GetString());
	if (!item)
	{
		LordLogError("unknown item type: %s", json["item"].GetString());
		return nullptr;
	}
	int id = item->itemID;
	int data;
	if (json.HasMember("data"))
	{
		data = json["data"].GetInt();
	}
	else
	{
		data = 0;
	}
	int count;
	if (json.HasMember("count"))
	{
		count = json["count"].GetInt();
	}
	else
	{
		count = 1;
	}
	return make_shared<ItemStack>(id, count, data);
}

ItemStackPtr CraftingManager::findMatchingRecipe(InventoryCrafting* pInvCrafting, World* pWorld)
{
	int stackCount = 0;
	ItemStackPtr pFirst = NULL;
	ItemStackPtr pSecond = NULL;

	for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pSlotStack = pInvCrafting->getStackInSlot(i);

		if (pSlotStack)
		{
			if (stackCount == 0)
				pFirst = pSlotStack;

			if (stackCount == 1)
				pSecond = pSlotStack;

			++stackCount;
		}
	}

	if (stackCount == 2 && pFirst && pFirst->itemID == pSecond->itemID && pFirst->stackSize == 1 && 
		pSecond && pSecond->stackSize == 1 && Item::itemsList[pFirst->itemID]->isDamageable())
	{
		Item* pItem = Item::itemsList[pFirst->itemID];
		int var13 = pItem->getMaxDamage() - pFirst->getItemDamageForDisplay();
		int var8 = pItem->getMaxDamage() - pSecond->getItemDamageForDisplay();
		int var9 = var13 + var8 + pItem->getMaxDamage() * 5 / 100;
		int var10 = pItem->getMaxDamage() - var9;

		if (var10 < 0)
		{
			var10 = 0;
		}

		return LORD::make_shared<ItemStack>(pFirst->itemID, 1, var10);
	}
	else
	{
		for (auto pRecipe : recipes)
		{
			if (pRecipe->matches(pInvCrafting, pWorld))
			{
				return pRecipe->getCraftingResult(pInvCrafting);
			}
		}

		return NULL;
	}
}


}
