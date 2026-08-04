#include "Recipes.h"
#include "ItemStack.h"
#include "Item.h"
#include "Items.h"

#include "Block/BlockManager.h"
#include "Inventory/InventoryCrafting.h"
#include "Inventory/IInventory.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{
	
unsigned IRecipe::count = 0;

FurnaceRecipes* FurnaceRecipes::smeltingBase = NULL;

FurnaceRecipes::FurnaceRecipes()
{
	addSmelting(BLOCK_ID_ORE_IRON, LORD::make_shared<ItemStack>(Item::ingotIron), 0.7F);
	addSmelting(BLOCK_ID_ORE_GOLD, LORD::make_shared<ItemStack>(Item::ingotGold), 1.0F);
	addSmelting(BLOCK_ID_ORE_DIAMOND, LORD::make_shared<ItemStack>(Item::diamond), 1.0F);
	addSmelting(BLOCK_ID_SAND, LORD::make_shared<ItemStack>(BlockManager::grass), 0.1F);
	addSmelting(Item::porkRaw->itemID, LORD::make_shared<ItemStack>(Item::porkCooked), 0.35F);
	addSmelting(Item::beefRaw->itemID, LORD::make_shared<ItemStack>(Item::beefCooked), 0.35F);
	addSmelting(Item::chickenRaw->itemID, LORD::make_shared<ItemStack>(Item::chickenCooked), 0.35F);
	addSmelting(Item::fishRaw->itemID, LORD::make_shared<ItemStack>(Item::fishCooked), 0.35F);
	addSmelting(BLOCK_ID_COBBLE_STONE, LORD::make_shared<ItemStack>(BlockManager::stone), 0.1F);
	addSmelting(Item::clay->itemID, LORD::make_shared<ItemStack>(Item::brick), 0.3F);
	addSmelting(BLOCK_ID_BLOCKCLAY, LORD::make_shared<ItemStack>(BlockManager::clayHardened), 0.35F);
	addSmelting(BLOCK_ID_CACTUS, LORD::make_shared<ItemStack>(Item::dyePowder, 1, 2), 0.2F);
	addSmelting(BLOCK_ID_WOOD, LORD::make_shared<ItemStack>(Item::coal, 1, 1), 0.15F);
	addSmelting(BLOCK_ID_WOOD2, LORD::make_shared<ItemStack>(Item::coal, 1, 1), 0.15F);
	addSmelting(BLOCK_ID_ORE_EMERALD, LORD::make_shared<ItemStack>(Item::emerald), 1.0F);
	addSmelting(Item::potato->itemID, LORD::make_shared<ItemStack>(Item::bakedPotato), 0.35F);
	addSmelting(BLOCK_ID_NETHERRACK, LORD::make_shared<ItemStack>(Item::netherrackBrick), 0.1F);
	addSmelting(BLOCK_ID_ORE_COAL, LORD::make_shared<ItemStack>(Item::coal), 0.1F);
	// addSmelting(BLOCK_ID_ORE_REDSTONE, LORD::make_shared<ItemStack>(Item::redstone), 0.7F);
	addSmelting(BLOCK_ID_ORE_LAPIS, LORD::make_shared<ItemStack>(Item::dyePowder, 1, 4), 0.2F);
	addSmelting(BLOCK_ID_ORE_NETHER_QUARTZ, LORD::make_shared<ItemStack>(Item::netherQuartz), 0.2F);
}

FurnaceRecipes::~FurnaceRecipes()
{
	smeltingList.clear();
	smeltingBase = NULL;
}

FurnaceRecipes* FurnaceRecipes::smelting()
{
	if (!smeltingBase)
		smeltingBase = LordNew FurnaceRecipes();
	return smeltingBase;
}

void FurnaceRecipes::addSmelting(int blockID, ItemStackPtr pStack, float exp)
{
	smeltingList.insert(std::make_pair(blockID, pStack));
	experienceList.insert(std::make_pair(pStack->itemID, exp));
}

ItemStackPtr FurnaceRecipes::getSmeltingResult(int blockID)
{
	BlockItemStackMap::iterator it = smeltingList.find(blockID);
	if (it == smeltingList.end())
		return NULL;
	return it->second;
}

float FurnaceRecipes::getExperience(int itemID)
{
	StackExperienceMap::iterator it = experienceList.find(itemID);
	if (it == experienceList.end())
		return 0.f;
	return it->second;
}

bool RecipeFireworks::matches(InventoryCrafting* pInvCrafting, World* pWorld)
{
	resultItem = NULL;
	int var3 = 0;
	int var4 = 0;
	int var5 = 0;
	int var6 = 0;
	int var7 = 0;
	int var8 = 0;

	for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pStack = pInvCrafting->getStackInSlot(i);

		if (pStack != NULL)
		{
			if (pStack->itemID == Item::gunpowder->itemID)
			{
				++var4;
			}
			else if (pStack->itemID == Item::fireworkCharge->itemID)
			{
				++var6;
			}
			else if (pStack->itemID == Item::dyePowder->itemID)
			{
				++var5;
			}
			else if (pStack->itemID == Item::paper->itemID)
			{
				++var3;
			}
			else if (pStack->itemID == Item::glowstone->itemID)
			{
				++var7;
			}
			else if (pStack->itemID == Item::diamond->itemID)
			{
				++var7;
			}
			else if (pStack->itemID == Item::fireballCharge->itemID)
			{
				++var8;
			}
			else if (pStack->itemID == Item::feather->itemID)
			{
				++var8;
			}
			else if (pStack->itemID == Item::goldNugget->itemID)
			{
				++var8;
			}
			else
			{
				if (pStack->itemID != Item::skull->itemID)
				{
					return false;
				}

				++var8;
			}
		}
	}

	var7 += var5 + var8;

	if (var4 <= 3 && var3 <= 1)
	{
		NBTTagCompound* pNbtItem;
		NBTTagCompound* pNbtFirework;

		if (var4 >= 1 && var3 == 1 && var7 == 0)
		{
			resultItem = LORD::make_shared<ItemStack>(Item::firework);

			if (var6 > 0)
			{
				pNbtItem = LordNew NBTTagCompound();
				pNbtFirework = LordNew NBTTagCompound("Fireworks");
				NBTTagList* pNbtExplosions = LordNew NBTTagList("Explosions");

				for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
				{
					ItemStackPtr pSlotItem = pInvCrafting->getStackInSlot(i);

					if (pSlotItem && pSlotItem->itemID == Item::fireworkCharge->itemID && 
						pSlotItem->hasTagCompound() && pSlotItem->getTagCompound()->hasKey("Explosion"))
					{
						pNbtExplosions->appendTag(pSlotItem->getTagCompound()->getCompoundTag("Explosion"));
					}
				}

				pNbtFirework->setTag("Explosions", pNbtExplosions);
				pNbtFirework->setByte("Flight", (i8)var4);
				pNbtItem->setTag("Fireworks", pNbtFirework);
				resultItem->setTagCompound(pNbtItem);
			}

			return true;
		}
		else if (var4 == 1 && var3 == 0 && var6 == 0 && var5 > 0 && var8 <= 1)
		{
			resultItem = LORD::make_shared<ItemStack>(Item::fireworkCharge);
			pNbtItem = LordNew NBTTagCompound();
			pNbtFirework = LordNew NBTTagCompound("Explosion");
			i8 type = 0;
			vector<int>::type colors;

			for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
			{
				ItemStackPtr pSlotItem = pInvCrafting->getStackInSlot(i);

				if (!pSlotItem)
					continue;

				if (pSlotItem->itemID == Item::dyePowder->itemID)
				{
					colors.push_back(ItemDye::dyeColors[pSlotItem->getItemDamage()]);
				}
				else if (pSlotItem->itemID == Item::glowstone->itemID)
				{
					pNbtFirework->setBool("Flicker", true);
				}
				else if (pSlotItem->itemID == Item::diamond->itemID)
				{
					pNbtFirework->setBool("Trail", true);
				}
				else if (pSlotItem->itemID == Item::fireballCharge->itemID)
				{
					type = 1;
				}
				else if (pSlotItem->itemID == Item::feather->itemID)
				{
					type = 4;
				}
				else if (pSlotItem->itemID == Item::goldNugget->itemID)
				{
					type = 2;
				}
				else if (pSlotItem->itemID == Item::skull->itemID)
				{
					type = 3;
				}
			}

			pNbtFirework->setIntArray("Colors", (int*)&(colors[0]), colors.size());
			pNbtFirework->setByte("Type", type);
			pNbtItem->setTag("Explosion", pNbtFirework);
			resultItem->setTagCompound(pNbtItem);
			return true;
		}
		else if (var4 == 0 && var3 == 0 && var6 == 1 && var5 > 0 && var5 == var7)
		{
			vector<int>::type colors;

			for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
			{
				ItemStackPtr pSlotItem = pInvCrafting->getStackInSlot(i);

				if (!pSlotItem)
					continue;
				
				if (pSlotItem->itemID == Item::dyePowder->itemID)
				{
					colors.push_back(ItemDye::dyeColors[pSlotItem->getItemDamage()]);
				}
				else if (pSlotItem->itemID == Item::fireworkCharge->itemID)
				{
					resultItem = pSlotItem->copy();
					resultItem->stackSize = 1;
				}
			}

			if (resultItem && resultItem->hasTagCompound())
			{
				NBTTagCompound* pNbtExplosion = resultItem->getTagCompound()->getCompoundTag("Explosion");

				if (!pNbtExplosion)
				{
					return false;
				}
				else
				{
					pNbtExplosion->setIntArray("FadeColors", (int*)&colors[0], colors.size());
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

ItemStackPtr RecipeFireworks::getCraftingResult(InventoryCrafting* pInvCrafting)
{
	return resultItem->copy();
}

bool RecipesArmorDyes::matches(InventoryCrafting* pInvCrafting, World* pWorld)
{
	ItemStackPtr pStack = NULL;
	ItemStackArr stacks;

	for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pSlotItem = pInvCrafting->getStackInSlot(i);

		if (pSlotItem)
		{
			ItemArmor* pArmor = dynamic_cast<ItemArmor*>(pSlotItem->getItem());
			if (pArmor)
			{
				if (pArmor->getArmorMaterial() != ArmorMaterial::CLOTH || pStack )
				{
					return false;
				}

				pStack = pSlotItem;
			}
			else
			{
				if (pSlotItem->itemID != Item::dyePowder->itemID)
				{
					return false;
				}

				stacks.push_back(pSlotItem);
			}
		}
	}

	return pStack && !stacks.empty();
}

ItemStackPtr RecipesArmorDyes::getCraftingResult(InventoryCrafting* pInvCrafting)
{
	ItemStackPtr pStack = NULL;
	int colors[3];
	int brightAmount = 0;
	int armorCount = 0;
	ItemArmor* pArmor = NULL;
	int i;
	int color;
	
	for (i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pSlotStack = pInvCrafting->getStackInSlot(i);
		pArmor = dynamic_cast<ItemArmor*>(pSlotStack->getItem());

		if (pSlotStack)
		{
			if (pArmor)
			{
				if (pArmor->getArmorMaterial() != ArmorMaterial::CLOTH || pStack != NULL)
				{
					return NULL;
				}

				pStack = pSlotStack->copy();
				pStack->stackSize = 1;

				if (pArmor->hasColor(pSlotStack))
				{
					color = pArmor->getColor(pStack);
					float fr = (float)(color >> 16 & 255) / 255.0F;
					float fg = (float)(color >> 8 & 255) / 255.0F;
					float fb = (float)(color & 255) / 255.0F;
					brightAmount = (int)((float)brightAmount + Math::Max(fr, Math::Max(fg, fb)) * 255.0F);
					colors[0] = (int)((float)colors[0] + fr * 255.0F);
					colors[1] = (int)((float)colors[1] + fg * 255.0F);
					colors[2] = (int)((float)colors[2] + fb * 255.0F);
					++armorCount;
				}
			}
			else
			{
				if (pSlotStack->itemID != Item::dyePowder->itemID)
				{
					return NULL;
				}

				// todo.
				/*
				float[] var14 = EntitySheep.fleeceColorTable[BlockColored.getBlockFromDye(pSlotStack.getItemDamage())];
				int var15 = (int)(var14[0] * 255.0F);
				int var16 = (int)(var14[1] * 255.0F);
				var17 = (int)(var14[2] * 255.0F);
				brightAmount += Math.max(var15, Math.max(var16, var17));
				colors[0] += var15;
				colors[1] += var16;
				colors[2] += var17;
				*/
				++armorCount;
			}
		}
	}

	if (pArmor == NULL)
	{
		return NULL;
	}
	else
	{
		int averageR = colors[0] / armorCount;
		int averageG = colors[1] / armorCount;
		int averageB = colors[2] / armorCount;
		float averageBright = (float)brightAmount / (float)armorCount;
		float maxBright = (float)Math::Max(averageR, Math::Max(averageG, averageB));
		averageR = (int)((float)averageR * averageBright / maxBright);
		averageG = (int)((float)averageG * averageBright / maxBright);
		averageB = (int)((float)averageB * averageBright / maxBright);
		int c = (averageR << 8) + averageG;
		c = (c << 8) + averageB;
		pArmor->setColor(pStack, c);
		return pStack;
	}
}

bool RecipesMapCloning::matches(InventoryCrafting* pInvCrafting, World* pWorld)
{
	int var3 = 0;
	ItemStackPtr pStack = NULL;

	for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pSlotStack = pInvCrafting->getStackInSlot(i);

		if (!pSlotStack)
			continue;

		if (pSlotStack->itemID == Item::imap->itemID)
		{
			if (pStack)
				return false;

			pStack = pSlotStack;
		}
		else
		{
			if (pSlotStack->itemID != Item::emptyMap->itemID)
				return false;

			++var3;
		}
	}

	return pStack != NULL && var3 > 0;
}

ItemStackPtr RecipesMapCloning::getCraftingResult(InventoryCrafting* pInvCrafting)
{
	int var2 = 0;
	ItemStackPtr pStack = NULL;

	for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
	{
		ItemStackPtr pSlotStack = pInvCrafting->getStackInSlot(i);

		if (pSlotStack)
		{
			if (pSlotStack->itemID == Item::imap->itemID)
			{
				if (pStack)
					return NULL;

				pStack = pSlotStack;
			}
			else
			{
				if (pSlotStack->itemID != Item::emptyMap->itemID)
				{
					return NULL;
				}

				++var2;
			}
		}
	}

	if (pStack != NULL && var2 >= 1)
	{
		ItemStackPtr pResult = LORD::make_shared<ItemStack>(Item::imap, var2 + 1, pStack->getItemDamage());

		if (pStack->hasDisplayName())
		{
			pResult->setItemName(pStack->getDisplayName());
		}

		return pResult;
	}
	
	return NULL;
}

ShapedRecipes::ShapedRecipes(int width, int height, const IngredientPtrArr& stacks, ItemStackPtr pStack, const String& name, const String& group)
	: IRecipe(name, group)
{
	recipeOutputItemID = pStack->itemID;
	recipeWidth = width;
	recipeHeight = height;
	recipeItems = stacks;
	recipeOutput = pStack;
}

bool ShapedRecipes::matches(InventoryCrafting* pInvCrafting, World* pWorld)
{
	for (int i = 0; i <= 3 - recipeWidth; ++i)
	{
		for (int j = 0; j <= 3 - recipeHeight; ++j)
		{
			if (checkMatch(pInvCrafting, i, j, true))
			{
				return true;
			}

			if (checkMatch(pInvCrafting, i, j, false))
			{
				return true;
			}
		}
	}

	return false;
}

bool ShapedRecipes::checkMatch(InventoryCrafting* pInvCrafting, int xPos, int yPos, bool mirror)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int xOffset = i - xPos;
			int jOffset = j - yPos;
			IngredientPtr ingredient;

			if (xOffset >= 0 && jOffset >= 0 && xOffset < recipeWidth && jOffset < recipeHeight)
			{
				if (mirror)
				{
					ingredient = recipeItems[recipeWidth - xOffset - 1 + jOffset * recipeWidth];
				}
				else
				{
					ingredient = recipeItems[xOffset + jOffset * recipeWidth];
				}
			}

			ItemStackPtr pStack = pInvCrafting->getStackInRowAndColumn(i, j);
			if (!ingredient->matches(pStack))
			{
				return false;
			}
		}
	}

	return true;
}

ItemStackPtr ShapedRecipes::getCraftingResult(InventoryCrafting* pInvCrafting)
{
	ItemStackPtr pResult = getRecipeOutput()->copy();

	if (copyIngredientNBT)
	{
		for (int i = 0; i < pInvCrafting->getSizeInventory(); ++i)
		{
			ItemStackPtr pStack = pInvCrafting->getStackInSlot(i);

			if (pStack && pStack->hasTagCompound())
			{
				pResult->setTagCompound((NBTTagCompound*)pStack->stackTagCompound->copy());
			}
		}
	}

	return pResult;
}

bool ShapedRecipes::needsItem(ItemStackPtr pStack)
{
	for (auto ingredient : recipeItems)
	{
		if (ingredient && ingredient->matches(pStack))
		{
			return true;
		}
	}
	return false;
}

ShapelessRecipes::ShapelessRecipes(ItemStackPtr pStack, const IngredientPtrArr& stacks, const String& name, const String& group)
	: IRecipe(name, group)
{
	recipeOutput = pStack;
	recipeItems = stacks;
}


bool ShapelessRecipes::matches(InventoryCrafting* pInvCrafting, World* pWorld)
{
	throw std::logic_error("not implemented");
	//typedef set<ItemStackPtr>::type ItemStackSet;
	//ItemStackSet stacks(recipeItems.begin(), recipeItems.end());

	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		ItemStackPtr pStack = pInvCrafting->getStackInRowAndColumn(j, i);

	//		if (!pStack)
	//			continue;

	//		bool flag = false;
	//		for (ItemStackSet::iterator it = stacks.begin(); it != stacks.end(); ++it)
	//		{
	//			ItemStackPtr pRecipeStack = *it;
	//			if (pStack->itemID == pRecipeStack->itemID && 
	//				(pRecipeStack->getItemDamage() == 32767 || pStack->getItemDamage() == pRecipeStack->getItemDamage()))
	//			{
	//				flag = true;
	//				stacks.erase(it);
	//				break;
	//			}
	//		}
	//		
	//		if (!flag)
	//		{
	//			return false;
	//		}
	//	}
	//}

	//return stacks.empty();
}

ItemStackPtr ShapelessRecipes::getCraftingResult(InventoryCrafting* pInvCrafting)
{
	return recipeOutput->copy();
}

void CraftingRecipe::makeUnavailable()
{
	m_craftingLimit = 0;
	size_t size = m_materialsAvailable.size();
	m_materialsAvailable.clear();
	m_materialsAvailable.resize(size);
}

}