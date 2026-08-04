
#include "Item/BehaviorDispenseItem.h"
#include "Item/Ingredient.h"
#include "Item/Item.h"
#include "Item/ItemCrafter.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "Item/RecipeJsonValidator.h"
#include "Item/Recipes.h"

#include "Block/BM_Material.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;


// Register Class BehaviorDefaultDispenseItem
_BEGIN_REGISTER_CLASS(BehaviorDefaultDispenseItem)
_CLASSREGISTER_AddBaseClass(IBehaviorDispenseItem)
_CLASSREGISTER_AddMember(dispense, BehaviorDefaultDispenseItem::dispense)
_CLASSREGISTER_AddMember(dispenseStack, BehaviorDefaultDispenseItem::dispenseStack)
_CLASSREGISTER_AddStaticMember(doDispense, BehaviorDefaultDispenseItem::doDispense)
_END_REGISTER_CLASS()




// Register Class BehaviorDispenseItemProvider
_BEGIN_REGISTER_CLASS(BehaviorDispenseItemProvider)
_CLASSREGISTER_AddBaseClass(IBehaviorDispenseItem)
_CLASSREGISTER_AddMember(dispense, BehaviorDispenseItemProvider::dispense)
_END_REGISTER_CLASS()




// Register Class DispenserBehaviorPotion
_BEGIN_REGISTER_CLASS(DispenserBehaviorPotion)
_CLASSREGISTER_AddBaseClass(IBehaviorDispenseItem)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(dispense, DispenserBehaviorPotion::dispense)
_END_REGISTER_CLASS()




// Register Class IBehaviorDispenseItem
_BEGIN_REGISTER_CLASS(IBehaviorDispenseItem)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(getIPositionFromBlockSource, IBehaviorDispenseItem::getIPositionFromBlockSource)
_END_REGISTER_CLASS()




// Register Class Ingredient
_BEGIN_REGISTER_CLASS(Ingredient)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getCandidateItemDataList, Ingredient::getCandidateItemDataList)
_CLASSREGISTER_AddMember(getItemId, Ingredient::getItemId)
_CLASSREGISTER_AddMember(matches, Ingredient::matches)
_END_REGISTER_CLASS()




// Register Class Item
_BEGIN_REGISTER_CLASS(Item)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(canHarvestBlock, Item::canHarvestBlock)
_CLASSREGISTER_AddMember(canItemEditBlocks, Item::canItemEditBlocks)
_CLASSREGISTER_AddMember(doesContainerItemLeaveCraftingGrid, Item::doesContainerItemLeaveCraftingGrid)
_CLASSREGISTER_AddMember(getAttributeModifiers, Item::getAttributeModifiers)
_CLASSREGISTER_AddMember(getContainerItem, Item::getContainerItem)
_CLASSREGISTER_AddMember(getHasSubtypes, Item::getHasSubtypes)
_CLASSREGISTER_AddMember(getIsRepairable, Item::getIsRepairable)
_CLASSREGISTER_AddStaticMember(getItemByName, Item::getItemByName)
_CLASSREGISTER_AddStaticMember(getItemById, Item::getItemById)
_CLASSREGISTER_AddMember(getItemDisplayName, Item::getItemDisplayName)
_CLASSREGISTER_AddMember(getItemEnchantability, Item::getItemEnchantability)
_CLASSREGISTER_AddMember(getItemStackDisplayName, Item::getItemStackDisplayName)
_CLASSREGISTER_AddMember(getItemStackLimit, Item::getItemStackLimit)
_CLASSREGISTER_AddMember(getItemUseAction, Item::getItemUseAction)
_CLASSREGISTER_AddMember(getLocalizedName, Item::getLocalizedName)
_CLASSREGISTER_AddMember(getMaxDamage, Item::getMaxDamage)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, Item::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(getMetadata, Item::getMetadata)
_CLASSREGISTER_AddMember(getPotionEffect, Item::getPotionEffect)
_CLASSREGISTER_AddMember(getRarity, Item::getRarity)
_CLASSREGISTER_AddMember(getShareTag, Item::getShareTag)
_CLASSREGISTER_AddMember(getStatName, Item::getStatName)
_CLASSREGISTER_AddMember(getStrVsBlock, Item::getStrVsBlock)
_CLASSREGISTER_AddMember_Override(getUnlocalizedName, Item::getUnlocalizedName,String )
_CLASSREGISTER_AddMember_Override(getUnlocalizedName1, Item::getUnlocalizedName,String ,ItemStackPtr )
_CLASSREGISTER_AddMember(hasContainerItem, Item::hasContainerItem)
_CLASSREGISTER_AddMember(hasEffect, Item::hasEffect)
_CLASSREGISTER_AddMember(hitEntity, Item::hitEntity)
_CLASSREGISTER_AddStaticMember(initialize, Item::initialize)
_CLASSREGISTER_AddStaticMember(initializeItemNameToIdMap, Item::initializeItemNameToIdMap)
_CLASSREGISTER_AddMember(interactWithEntity, Item::interactWithEntity)
_CLASSREGISTER_AddMember(isDamageable, Item::isDamageable)
_CLASSREGISTER_AddMember(isItemTool, Item::isItemTool)
_CLASSREGISTER_AddMember(isMap, Item::isMap)
_CLASSREGISTER_AddMember(isPotionIngredient, Item::isPotionIngredient)
_CLASSREGISTER_AddMember(onBlockDestroyed, Item::onBlockDestroyed)
_CLASSREGISTER_AddMember(onCreated, Item::onCreated)
_CLASSREGISTER_AddMember(onEaten, Item::onEaten)
_CLASSREGISTER_AddMember(onItemRightClick, Item::onItemRightClick)
_CLASSREGISTER_AddMember(onItemUse, Item::onItemUse)
_CLASSREGISTER_AddMember(onPlayerStoppedUsing, Item::onPlayerStoppedUsing)
_CLASSREGISTER_AddMember(onUpdate, Item::onUpdate)
_CLASSREGISTER_AddMember(setContainerItem, Item::setContainerItem)
_CLASSREGISTER_AddMember(setMaxStackSize, Item::setMaxStackSize)
_CLASSREGISTER_AddMember(setPotionEffect, Item::setPotionEffect)
_CLASSREGISTER_AddMember(setUnlocalizedName, Item::setUnlocalizedName)
_CLASSREGISTER_AddStaticMember(unInitialize, Item::unInitialize)
_END_REGISTER_CLASS()




// Register Class ItemCrafter
_BEGIN_REGISTER_CLASS(ItemCrafter)
_CLASSREGISTER_AddBaseClass(Singleton<ItemCrafter>)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(craftItem, ItemCrafter::craftItem)
_CLASSREGISTER_AddMember(craftWithinInventory, ItemCrafter::craftWithinInventory)
_END_REGISTER_CLASS()




// Register Class ArmorMaterial
_BEGIN_REGISTER_CLASS(ArmorMaterial)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(getArmorCraftingMaterial, ArmorMaterial::getArmorCraftingMaterial)
_CLASSREGISTER_AddMember(getDamageReductionAmount, ArmorMaterial::getDamageReductionAmount)
_CLASSREGISTER_AddMember(getDurability, ArmorMaterial::getDurability)
_CLASSREGISTER_AddMember(getEnchantability, ArmorMaterial::getEnchantability)
_CLASSREGISTER_AddStaticMember(initialize, ArmorMaterial::initialize)
_CLASSREGISTER_AddStaticMember(unInitialize, ArmorMaterial::unInitialize)
_END_REGISTER_CLASS()




// Register Class ItemAnvilBlock
_BEGIN_REGISTER_CLASS(ItemAnvilBlock)
_CLASSREGISTER_AddBaseClass(ItemMultiTextureTile)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getMetadata, ItemAnvilBlock::getMetadata)
_END_REGISTER_CLASS()




// Register Class ItemAppleGold
_BEGIN_REGISTER_CLASS(ItemAppleGold)
_CLASSREGISTER_AddBaseClass(ItemFood)
_CLASSREGISTER_AddCtor(int , const String & , int , float , bool )
_CLASSREGISTER_AddMember(getRarity, ItemAppleGold::getRarity)
_CLASSREGISTER_AddMember(hasEffect, ItemAppleGold::hasEffect)
_END_REGISTER_CLASS()




// Register Class ItemArmor
_BEGIN_REGISTER_CLASS(ItemArmor)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , ArmorMaterial * , int )
_CLASSREGISTER_AddMember(getArmorMaterial, ItemArmor::getArmorMaterial)
_CLASSREGISTER_AddMember(getColor, ItemArmor::getColor)
_CLASSREGISTER_AddMember(getIsRepairable, ItemArmor::getIsRepairable)
_CLASSREGISTER_AddMember(getItemEnchantability, ItemArmor::getItemEnchantability)
_CLASSREGISTER_AddStaticMember(getMaxDamageArray, ItemArmor::getMaxDamageArray)
_CLASSREGISTER_AddMember(hasColor, ItemArmor::hasColor)
_CLASSREGISTER_AddStaticMember_Override(isArmor, ItemArmor::isArmor,bool ,int )
_CLASSREGISTER_AddStaticMember_Override(isArmor1, ItemArmor::isArmor,bool ,Item * )
_CLASSREGISTER_AddMember(onItemRightClick, ItemArmor::onItemRightClick)
_CLASSREGISTER_AddMember(removeColor, ItemArmor::removeColor)
_CLASSREGISTER_AddMember(requiresMultipleRenderPasses, ItemArmor::requiresMultipleRenderPasses)
_CLASSREGISTER_AddMember(setColor, ItemArmor::setColor)
_END_REGISTER_CLASS()




// Register Class ItemAxe
_BEGIN_REGISTER_CLASS(ItemAxe)
_CLASSREGISTER_AddBaseClass(ItemTool)
_CLASSREGISTER_AddCtor(int , const String & , ToolMaterial * )
_CLASSREGISTER_AddMember(getStrVsBlock, ItemAxe::getStrVsBlock)
_END_REGISTER_CLASS()




// Register Class ItemBed
_BEGIN_REGISTER_CLASS(ItemBed)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemBed::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemBlock
_BEGIN_REGISTER_CLASS(ItemBlock)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(canPlaceItemBlockOnSide, ItemBlock::canPlaceItemBlockOnSide)
_CLASSREGISTER_AddMember(getBlockID, ItemBlock::getBlockID)
_CLASSREGISTER_AddMember_Override(getUnlocalizedName, ItemBlock::getUnlocalizedName,String ,ItemStackPtr )
_CLASSREGISTER_AddMember_Override(getUnlocalizedName1, ItemBlock::getUnlocalizedName,String )
_CLASSREGISTER_AddMember(onItemUse, ItemBlock::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemBlockWithMetadata
_BEGIN_REGISTER_CLASS(ItemBlockWithMetadata)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getMetadata, ItemBlockWithMetadata::getMetadata)
_END_REGISTER_CLASS()




// Register Class ItemBoat
_BEGIN_REGISTER_CLASS(ItemBoat)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemBoat::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemBook
_BEGIN_REGISTER_CLASS(ItemBook)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemEnchantability, ItemBook::getItemEnchantability)
_CLASSREGISTER_AddMember(isItemTool, ItemBook::isItemTool)
_END_REGISTER_CLASS()




// Register Class ItemBow
_BEGIN_REGISTER_CLASS(ItemBow)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemEnchantability, ItemBow::getItemEnchantability)
_CLASSREGISTER_AddMember(getItemUseAction, ItemBow::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemBow::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(onEaten, ItemBow::onEaten)
_CLASSREGISTER_AddMember(onItemRightClick, ItemBow::onItemRightClick)
_CLASSREGISTER_AddMember(onPlayerStoppedUsing, ItemBow::onPlayerStoppedUsing)
_END_REGISTER_CLASS()




// Register Class ItemBucket
_BEGIN_REGISTER_CLASS(ItemBucket)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , int )
_CLASSREGISTER_AddMember(doIt, ItemBucket::doIt)
_CLASSREGISTER_AddMember(onItemRightClick, ItemBucket::onItemRightClick)
_CLASSREGISTER_AddMember(tryPlaceContainedLiquid, ItemBucket::tryPlaceContainedLiquid)
_END_REGISTER_CLASS()




// Register Class ItemBucketMilk
_BEGIN_REGISTER_CLASS(ItemBucketMilk)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemUseAction, ItemBucketMilk::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemBucketMilk::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(onEaten, ItemBucketMilk::onEaten)
_CLASSREGISTER_AddMember(onItemRightClick, ItemBucketMilk::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemCarrotOnAStick
_BEGIN_REGISTER_CLASS(ItemCarrotOnAStick)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemCarrotOnAStick::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemCloth
_BEGIN_REGISTER_CLASS(ItemCloth)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getMetadata, ItemCloth::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemCloth::getUnlocalizedName)
_END_REGISTER_CLASS()




// Register Class ItemCoal
_BEGIN_REGISTER_CLASS(ItemCoal)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemCoal::getUnlocalizedName)
_END_REGISTER_CLASS()




// Register Class ItemColored
_BEGIN_REGISTER_CLASS(ItemColored)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int , bool )
_CLASSREGISTER_AddCtor(int , bool , const StringArray & )
_CLASSREGISTER_AddMember(getMetadata, ItemColored::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemColored::getUnlocalizedName)
_CLASSREGISTER_AddMember(setBlockNames, ItemColored::setBlockNames)
_END_REGISTER_CLASS()




// Register Class ItemDoor
_BEGIN_REGISTER_CLASS(ItemDoor)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , const BM_Material & )
_CLASSREGISTER_AddMember(onItemUse, ItemDoor::onItemUse)
_CLASSREGISTER_AddStaticMember(placeDoorBlock, ItemDoor::placeDoorBlock)
_END_REGISTER_CLASS()




// Register Class ItemDye
_BEGIN_REGISTER_CLASS(ItemDye)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddStaticMember(applyBonemeal, ItemDye::applyBonemeal)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemDye::getUnlocalizedName)
_CLASSREGISTER_AddMember(interactWithEntity, ItemDye::interactWithEntity)
_CLASSREGISTER_AddMember(onItemUse, ItemDye::onItemUse)
_CLASSREGISTER_AddStaticMember(spawnBonemealParticles, ItemDye::spawnBonemealParticles)
_END_REGISTER_CLASS()




// Register Class ItemEditableBook
_BEGIN_REGISTER_CLASS(ItemEditableBook)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemDisplayName, ItemEditableBook::getItemDisplayName)
_CLASSREGISTER_AddMember(getShareTag, ItemEditableBook::getShareTag)
_CLASSREGISTER_AddMember(hasEffect, ItemEditableBook::hasEffect)
_CLASSREGISTER_AddMember(onItemRightClick, ItemEditableBook::onItemRightClick)
_CLASSREGISTER_AddStaticMember(validBookTagContents, ItemEditableBook::validBookTagContents)
_END_REGISTER_CLASS()




// Register Class ItemEgg
_BEGIN_REGISTER_CLASS(ItemEgg)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemEgg::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemEmptyMap
_BEGIN_REGISTER_CLASS(ItemEmptyMap)
_CLASSREGISTER_AddBaseClass(ItemMapBase)
_CLASSREGISTER_AddCtor(int , const String & )
_END_REGISTER_CLASS()




// Register Class ItemEnchantedBook
_BEGIN_REGISTER_CLASS(ItemEnchantedBook)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(func_92112_a, ItemEnchantedBook::func_92112_a)
_CLASSREGISTER_AddMember(func_92113_a, ItemEnchantedBook::func_92113_a)
_CLASSREGISTER_AddMember(func_92114_b, ItemEnchantedBook::func_92114_b)
_CLASSREGISTER_AddMember(getEnchantedItemStack, ItemEnchantedBook::getEnchantedItemStack)
_CLASSREGISTER_AddMember(getEnchantedItemStack_do, ItemEnchantedBook::getEnchantedItemStack_do)
_CLASSREGISTER_AddMember(getRarity, ItemEnchantedBook::getRarity)
_CLASSREGISTER_AddMember(getStoredEnchantments, ItemEnchantedBook::getStoredEnchantments)
_CLASSREGISTER_AddMember(hasEffect, ItemEnchantedBook::hasEffect)
_CLASSREGISTER_AddMember(isItemTool, ItemEnchantedBook::isItemTool)
_END_REGISTER_CLASS()




// Register Class ItemEnderEye
_BEGIN_REGISTER_CLASS(ItemEnderEye)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemEnderEye::onItemRightClick)
_CLASSREGISTER_AddMember(onItemUse, ItemEnderEye::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemEnderPearl
_BEGIN_REGISTER_CLASS(ItemEnderPearl)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemEnderPearl::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemExpBottle
_BEGIN_REGISTER_CLASS(ItemExpBottle)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(hasEffect, ItemExpBottle::hasEffect)
_CLASSREGISTER_AddMember(onItemRightClick, ItemExpBottle::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemFireball
_BEGIN_REGISTER_CLASS(ItemFireball)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemFireball::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemFirework
_BEGIN_REGISTER_CLASS(ItemFirework)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemFirework::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemFireworkCharge
_BEGIN_REGISTER_CLASS(ItemFireworkCharge)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_END_REGISTER_CLASS()




// Register Class ItemFishingRod
_BEGIN_REGISTER_CLASS(ItemFishingRod)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemFishingRod::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemFlintAndSteel
_BEGIN_REGISTER_CLASS(ItemFlintAndSteel)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemFlintAndSteel::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemFood
_BEGIN_REGISTER_CLASS(ItemFood)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddMember(IsWolfsFavoriteMeat, ItemFood::IsWolfsFavoriteMeat)
_CLASSREGISTER_AddCtor(int , const String & , int , float , bool )
_CLASSREGISTER_AddCtor(int , const String & , int , bool )
_CLASSREGISTER_AddMember(getHealAmount, ItemFood::getHealAmount)
_CLASSREGISTER_AddMember(getItemUseAction, ItemFood::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemFood::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(getSaturationModifier, ItemFood::getSaturationModifier)
_CLASSREGISTER_AddMember(onEaten, ItemFood::onEaten)
_CLASSREGISTER_AddMember(onItemRightClick, ItemFood::onItemRightClick)
_CLASSREGISTER_AddMember(setAlwaysEdible, ItemFood::setAlwaysEdible)
_CLASSREGISTER_AddMember(setPotionEffect, ItemFood::setPotionEffect)
_END_REGISTER_CLASS()




// Register Class ItemGlassBottle
_BEGIN_REGISTER_CLASS(ItemGlassBottle)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemGlassBottle::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemHangingEntity
_BEGIN_REGISTER_CLASS(ItemHangingEntity)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , int )
_CLASSREGISTER_AddMember(onItemUse, ItemHangingEntity::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemHoe
_BEGIN_REGISTER_CLASS(ItemHoe)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , ToolMaterial * )
_CLASSREGISTER_AddMember(getMaterialName, ItemHoe::getMaterialName)
_CLASSREGISTER_AddMember(onItemUse, ItemHoe::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemLeash
_BEGIN_REGISTER_CLASS(ItemLeash)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddStaticMember(func_135066_a, ItemLeash::func_135066_a)
_CLASSREGISTER_AddMember(onItemUse, ItemLeash::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemLeaves
_BEGIN_REGISTER_CLASS(ItemLeaves)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getMetadata, ItemLeaves::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemLeaves::getUnlocalizedName)
_END_REGISTER_CLASS()




// Register Class ItemLilyPad
_BEGIN_REGISTER_CLASS(ItemLilyPad)
_CLASSREGISTER_AddBaseClass(ItemColored)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(onItemRightClick, ItemLilyPad::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemMap
_BEGIN_REGISTER_CLASS(ItemMap)
_CLASSREGISTER_AddBaseClass(ItemMapBase)
_CLASSREGISTER_AddCtor(int , const String & )
_END_REGISTER_CLASS()




// Register Class ItemMapBase
_BEGIN_REGISTER_CLASS(ItemMapBase)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(isMap, ItemMapBase::isMap)
_END_REGISTER_CLASS()




// Register Class ItemMinecart
_BEGIN_REGISTER_CLASS(ItemBlockman)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , int )
_CLASSREGISTER_AddStaticMember(initialize, ItemBlockman::initialize)
_CLASSREGISTER_AddMember(onItemUse, ItemBlockman::onItemUse)
_CLASSREGISTER_AddStaticMember(unInitialize, ItemBlockman::unInitialize)
_END_REGISTER_CLASS()




// Register Class ItemMonsterPlacer
_BEGIN_REGISTER_CLASS(ItemMonsterPlacer)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemDisplayName, ItemMonsterPlacer::getItemDisplayName)
_CLASSREGISTER_AddMember(onItemRightClick, ItemMonsterPlacer::onItemRightClick)
_CLASSREGISTER_AddMember(onItemUse, ItemMonsterPlacer::onItemUse)
_CLASSREGISTER_AddMember(requiresMultipleRenderPasses, ItemMonsterPlacer::requiresMultipleRenderPasses)
_CLASSREGISTER_AddStaticMember(spawnCreature, ItemMonsterPlacer::spawnCreature)
_END_REGISTER_CLASS()




// Register Class ItemMultiTextureTile
_BEGIN_REGISTER_CLASS(ItemMultiTextureTile)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int , const String & , const StringArray & )
_CLASSREGISTER_AddMember(getMetadata, ItemMultiTextureTile::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemMultiTextureTile::getUnlocalizedName)
_CLASSREGISTER_AddMember(setBlockNames, ItemMultiTextureTile::setBlockNames)
_END_REGISTER_CLASS()




// Register Class ItemNameTag
_BEGIN_REGISTER_CLASS(ItemNameTag)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(interactWithEntity, ItemNameTag::interactWithEntity)
_END_REGISTER_CLASS()




// Register Class ItemPickaxe
_BEGIN_REGISTER_CLASS(ItemPickaxe)
_CLASSREGISTER_AddBaseClass(ItemTool)
_CLASSREGISTER_AddCtor(int , const String & , ToolMaterial * )
_CLASSREGISTER_AddMember(canHarvestBlock, ItemPickaxe::canHarvestBlock)
_CLASSREGISTER_AddMember(getStrVsBlock, ItemPickaxe::getStrVsBlock)
_END_REGISTER_CLASS()




// Register Class ItemPiston
_BEGIN_REGISTER_CLASS(ItemPiston)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getMetadata, ItemPiston::getMetadata)
_END_REGISTER_CLASS()




// Register Class ItemPotion
_BEGIN_REGISTER_CLASS(ItemPotion)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(addEffect, ItemPotion::addEffect)
_CLASSREGISTER_AddMember(getColorFromDamage, ItemPotion::getColorFromDamage)
_CLASSREGISTER_AddMember_Override(getEffects, ItemPotion::getEffects,void ,ItemStackPtr ,PotionEffectArr & )
_CLASSREGISTER_AddMember_Override(getEffects1, ItemPotion::getEffects,void ,int ,PotionEffectArr & )
_CLASSREGISTER_AddMember(getItemDisplayName, ItemPotion::getItemDisplayName)
_CLASSREGISTER_AddMember(getItemUseAction, ItemPotion::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemPotion::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(hasEffect, ItemPotion::hasEffect)
_CLASSREGISTER_AddMember(isEffectInstant, ItemPotion::isEffectInstant)
//_CLASSREGISTER_AddStaticMember(isSplash, ItemPotion::isSplash)
_CLASSREGISTER_AddMember(onEaten, ItemPotion::onEaten)
_CLASSREGISTER_AddMember(onItemRightClick, ItemPotion::onItemRightClick)
_CLASSREGISTER_AddMember(onItemUse, ItemPotion::onItemUse)
_CLASSREGISTER_AddMember(requiresMultipleRenderPasses, ItemPotion::requiresMultipleRenderPasses)
_END_REGISTER_CLASS()




// Register Class ItemRecord
_BEGIN_REGISTER_CLASS(ItemRecord)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , const String & )
_CLASSREGISTER_AddStaticMember(getRecord, ItemRecord::getRecord)
_CLASSREGISTER_AddMember(getRecordTitle, ItemRecord::getRecordTitle)
_CLASSREGISTER_AddMember(onItemUse, ItemRecord::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemRedstone
_BEGIN_REGISTER_CLASS(ItemRedstone)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemRedstone::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemReed
_BEGIN_REGISTER_CLASS(ItemReed)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , int )
_CLASSREGISTER_AddMember(onItemUse, ItemReed::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSaddle
_BEGIN_REGISTER_CLASS(ItemSaddle)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(hitEntity, ItemSaddle::hitEntity)
_END_REGISTER_CLASS()




// Register Class ItemSeedFood
_BEGIN_REGISTER_CLASS(ItemSeedFood)
_CLASSREGISTER_AddBaseClass(ItemFood)
_CLASSREGISTER_AddCtor(int , const String & , int , float , int , int )
_CLASSREGISTER_AddMember(onItemUse, ItemSeedFood::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSeeds
_BEGIN_REGISTER_CLASS(ItemSeeds)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , int , int )
_CLASSREGISTER_AddMember(onItemUse, ItemSeeds::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemShears
_BEGIN_REGISTER_CLASS(ItemShears)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(canHarvestBlock, ItemShears::canHarvestBlock)
_CLASSREGISTER_AddMember(getStrVsBlock, ItemShears::getStrVsBlock)
_CLASSREGISTER_AddMember(onBlockDestroyed, ItemShears::onBlockDestroyed)
_END_REGISTER_CLASS()




// Register Class ItemSign
_BEGIN_REGISTER_CLASS(ItemSign)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemUse, ItemSign::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSimpleFoiled
_BEGIN_REGISTER_CLASS(ItemSimpleFoiled)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(hasEffect, ItemSimpleFoiled::hasEffect)
_END_REGISTER_CLASS()




// Register Class ItemSkull
_BEGIN_REGISTER_CLASS(ItemSkull)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getItemDisplayName, ItemSkull::getItemDisplayName)
_CLASSREGISTER_AddMember(getMetadata, ItemSkull::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemSkull::getUnlocalizedName)
_CLASSREGISTER_AddMember(onItemUse, ItemSkull::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSlab
_BEGIN_REGISTER_CLASS(ItemSlab)
_CLASSREGISTER_AddBaseClass(ItemBlock)
_CLASSREGISTER_AddCtor(int , const String & , Block * , Block * , bool )
_CLASSREGISTER_AddMember(canPlaceItemBlockOnSide, ItemSlab::canPlaceItemBlockOnSide)
_CLASSREGISTER_AddMember(getMetadata, ItemSlab::getMetadata)
_CLASSREGISTER_AddMember(getUnlocalizedName, ItemSlab::getUnlocalizedName)
_CLASSREGISTER_AddMember(onItemUse, ItemSlab::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSnow
_BEGIN_REGISTER_CLASS(ItemSnow)
_CLASSREGISTER_AddBaseClass(ItemBlockWithMetadata)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(onItemUse, ItemSnow::onItemUse)
_END_REGISTER_CLASS()




// Register Class ItemSnowball
_BEGIN_REGISTER_CLASS(ItemSnowball)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(onItemRightClick, ItemSnowball::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemSoup
_BEGIN_REGISTER_CLASS(ItemSoup)
_CLASSREGISTER_AddBaseClass(ItemFood)
_CLASSREGISTER_AddCtor(int , const String & , int )
_CLASSREGISTER_AddMember(onEaten, ItemSoup::onEaten)
_END_REGISTER_CLASS()




// Register Class ItemSpade
_BEGIN_REGISTER_CLASS(ItemSpade)
_CLASSREGISTER_AddBaseClass(ItemTool)
_CLASSREGISTER_AddCtor(int , const String & , ToolMaterial * )
_CLASSREGISTER_AddMember(canHarvestBlock, ItemSpade::canHarvestBlock)
_END_REGISTER_CLASS()




// Register Class ItemSword
_BEGIN_REGISTER_CLASS(ItemSword)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & , ToolMaterial * )
_CLASSREGISTER_AddMember(canHarvestBlock, ItemSword::canHarvestBlock)
_CLASSREGISTER_AddMember(getAttributeModifiers, ItemSword::getAttributeModifiers)
_CLASSREGISTER_AddMember(getDamageVsEntity, ItemSword::getDamageVsEntity)
_CLASSREGISTER_AddMember(getIsRepairable, ItemSword::getIsRepairable)
_CLASSREGISTER_AddMember(getItemEnchantability, ItemSword::getItemEnchantability)
_CLASSREGISTER_AddMember(getItemUseAction, ItemSword::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemSword::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(getStrVsBlock, ItemSword::getStrVsBlock)
_CLASSREGISTER_AddMember(getToolMaterialName, ItemSword::getToolMaterialName)
_CLASSREGISTER_AddMember(hitEntity, ItemSword::hitEntity)
_CLASSREGISTER_AddMember(onBlockDestroyed, ItemSword::onBlockDestroyed)
_CLASSREGISTER_AddMember(onItemRightClick, ItemSword::onItemRightClick)
_END_REGISTER_CLASS()




// Register Class ItemTool
_BEGIN_REGISTER_CLASS(ItemTool)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddMember(getAttributeModifiers, ItemTool::getAttributeModifiers)
_CLASSREGISTER_AddMember(getIsRepairable, ItemTool::getIsRepairable)
_CLASSREGISTER_AddMember(getItemEnchantability, ItemTool::getItemEnchantability)
_CLASSREGISTER_AddMember(getStrVsBlock, ItemTool::getStrVsBlock)
_CLASSREGISTER_AddMember(getToolMaterialName, ItemTool::getToolMaterialName)
_CLASSREGISTER_AddMember(hitEntity, ItemTool::hitEntity)
_CLASSREGISTER_AddMember(onBlockDestroyed, ItemTool::onBlockDestroyed)
_CLASSREGISTER_AddMember(setEffectiveBlocks, ItemTool::setEffectiveBlocks)
_END_REGISTER_CLASS()




// Register Class ItemWritableBook
_BEGIN_REGISTER_CLASS(ItemWritableBook)
_CLASSREGISTER_AddBaseClass(Item)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(getShareTag, ItemWritableBook::getShareTag)
_CLASSREGISTER_AddMember(onItemRightClick, ItemWritableBook::onItemRightClick)
_CLASSREGISTER_AddStaticMember(validBookTagPages, ItemWritableBook::validBookTagPages)
_END_REGISTER_CLASS()




// Register Class ToolMaterial
_BEGIN_REGISTER_CLASS(ToolMaterial)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(getDamageVsEntity, ToolMaterial::getDamageVsEntity)
_CLASSREGISTER_AddMember(getEfficiencyOnProperMaterial, ToolMaterial::getEfficiencyOnProperMaterial)
_CLASSREGISTER_AddMember(getEnchantability, ToolMaterial::getEnchantability)
_CLASSREGISTER_AddMember(getHarvestLevel, ToolMaterial::getHarvestLevel)
_CLASSREGISTER_AddMember(getMaxUses, ToolMaterial::getMaxUses)
_CLASSREGISTER_AddMember(getToolCraftingMaterial, ToolMaterial::getToolCraftingMaterial)
_CLASSREGISTER_AddStaticMember(initialize, ToolMaterial::initialize)
_CLASSREGISTER_AddMember(toString, ToolMaterial::toString)
_CLASSREGISTER_AddStaticMember(unInitialize, ToolMaterial::unInitialize)
_END_REGISTER_CLASS()




// Register Class ItemStack
_BEGIN_REGISTER_CLASS(ItemStack)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddBaseClass(std::enable_shared_from_this<ItemStack>)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(Block * , int , int )
_CLASSREGISTER_AddCtor(Item * , int , int )
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(addEnchantment, ItemStack::addEnchantment)
_CLASSREGISTER_AddStaticMember(areItemStackTagsEqual, ItemStack::areItemStackTagsEqual)
_CLASSREGISTER_AddStaticMember(areItemStacksEqual, ItemStack::areItemStacksEqual)
_CLASSREGISTER_AddMember(attemptDamageItem, ItemStack::attemptDamageItem)
_CLASSREGISTER_AddMember(canEditBlocks, ItemStack::canEditBlocks)
_CLASSREGISTER_AddMember(canHarvestBlock, ItemStack::canHarvestBlock)
_CLASSREGISTER_AddMember(clearCustomName, ItemStack::clearCustomName)
_CLASSREGISTER_AddMember(copy, ItemStack::copy)
_CLASSREGISTER_AddStaticMember(copyItemStack, ItemStack::copyItemStack)
_CLASSREGISTER_AddMember(damageItem, ItemStack::damageItem)
_CLASSREGISTER_AddMember(getAttributeModifiers, ItemStack::getAttributeModifiers)
_CLASSREGISTER_AddMember(getDisplayName, ItemStack::getDisplayName)
_CLASSREGISTER_AddMember(getEnchantmentTagList, ItemStack::getEnchantmentTagList)
_CLASSREGISTER_AddMember(getHasSubtypes, ItemStack::getHasSubtypes)
_CLASSREGISTER_AddMember(getItem, ItemStack::getItem)
_CLASSREGISTER_AddMember(getItemStackSize, ItemStack::getItemStackSize)
_CLASSREGISTER_AddMember(getItemDamage, ItemStack::getItemDamage)
_CLASSREGISTER_AddMember(getItemDamageForDisplay, ItemStack::getItemDamageForDisplay)
_CLASSREGISTER_AddMember(getItemName, ItemStack::getItemName)
_CLASSREGISTER_AddMember(getItemUseAction, ItemStack::getItemUseAction)
_CLASSREGISTER_AddMember(getMaxDamage, ItemStack::getMaxDamage)
_CLASSREGISTER_AddMember(getMaxItemUseDuration, ItemStack::getMaxItemUseDuration)
_CLASSREGISTER_AddMember(getMaxStackSize, ItemStack::getMaxStackSize)
_CLASSREGISTER_AddMember(getRarity, ItemStack::getRarity)
_CLASSREGISTER_AddMember(getRepairCost, ItemStack::getRepairCost)
_CLASSREGISTER_AddMember(getStrVsBlock, ItemStack::getStrVsBlock)
_CLASSREGISTER_AddMember(getTagCompound, ItemStack::getTagCompound)
_CLASSREGISTER_AddMember(getTooltip, ItemStack::getTooltip)
_CLASSREGISTER_AddMember(hasDisplayName, ItemStack::hasDisplayName)
_CLASSREGISTER_AddMember(hasEffect, ItemStack::hasEffect)
_CLASSREGISTER_AddMember(hasTagCompound, ItemStack::hasTagCompound)
_CLASSREGISTER_AddMember(hitEntity, ItemStack::hitEntity)
_CLASSREGISTER_AddMember(interactWithEntity, ItemStack::interactWithEntity)
_CLASSREGISTER_AddMember(isItemDamaged, ItemStack::isItemDamaged)
_CLASSREGISTER_AddMember(isItemEnchantable, ItemStack::isItemEnchantable)
_CLASSREGISTER_AddMember(isItemEnchanted, ItemStack::isItemEnchanted)
_CLASSREGISTER_AddMember(isItemEqual, ItemStack::isItemEqual)
_CLASSREGISTER_AddMember(isItemStackDamageable, ItemStack::isItemStackDamageable)
_CLASSREGISTER_AddMember(isStackable, ItemStack::isStackable)
_CLASSREGISTER_AddStaticMember(loadItemStackFromNBT, ItemStack::loadItemStackFromNBT)
_CLASSREGISTER_AddMember(onBlockDestroyed, ItemStack::onBlockDestroyed)
_CLASSREGISTER_AddMember(onCrafting, ItemStack::onCrafting)
_CLASSREGISTER_AddMember(onFoodEaten, ItemStack::onFoodEaten)
_CLASSREGISTER_AddMember(onPlayerStoppedUsing, ItemStack::onPlayerStoppedUsing)
_CLASSREGISTER_AddMember(readFromNBT, ItemStack::readFromNBT)
_CLASSREGISTER_AddMember(setItemDamage, ItemStack::setItemDamage)
_CLASSREGISTER_AddMember(setItemName, ItemStack::setItemName)
_CLASSREGISTER_AddMember(setRepairCost, ItemStack::setRepairCost)
_CLASSREGISTER_AddMember(setTagCompound, ItemStack::setTagCompound)
_CLASSREGISTER_AddMember(setTagInfo, ItemStack::setTagInfo)
_CLASSREGISTER_AddMember(splitStack, ItemStack::splitStack)
_CLASSREGISTER_AddMember(toString, ItemStack::toString)
_CLASSREGISTER_AddMember(tryPlaceItemIntoWorld, ItemStack::tryPlaceItemIntoWorld)
_CLASSREGISTER_AddMember(updateAnimation, ItemStack::updateAnimation)
_CLASSREGISTER_AddMember(useItemRightClick, ItemStack::useItemRightClick)
_CLASSREGISTER_AddMember(writeToNBT, ItemStack::writeToNBT)
_CLASSREGISTER_AddMember(addEnchantmentbyId, ItemStack::addEnchantmentbyId)
_END_REGISTER_CLASS()

// Register Class Potion
/*
_BEGIN_REGISTER_CLASS(Potion)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(affectEntity, Potion::affectEntity)
_CLASSREGISTER_AddMember(applyAttributesModifiersToEntity, Potion::applyAttributesModifiersToEntity)
_CLASSREGISTER_AddMember(getAttributeModifierAmount, Potion::getAttributeModifierAmount)
_CLASSREGISTER_AddMember(getAttributeModifierMap, Potion::getAttributeModifierMap)
_CLASSREGISTER_AddStaticMember(getDurationString, Potion::getDurationString)
_CLASSREGISTER_AddMember(getEffectiveness, Potion::getEffectiveness)
_CLASSREGISTER_AddMember(getId, Potion::getId)
_CLASSREGISTER_AddMember(getLiquidColor, Potion::getLiquidColor)
_CLASSREGISTER_AddMember(getName, Potion::getName)
_CLASSREGISTER_AddMember(getStatusIconIndex, Potion::getStatusIconIndex)
_CLASSREGISTER_AddMember(hasStatusIcon, Potion::hasStatusIcon)
_CLASSREGISTER_AddStaticMember(initialize, Potion::initialize)
_CLASSREGISTER_AddMember(isBadEffect, Potion::isBadEffect)
_CLASSREGISTER_AddMember(isInstant, Potion::isInstant)
_CLASSREGISTER_AddMember(isReady, Potion::isReady)
_CLASSREGISTER_AddMember(isUsable, Potion::isUsable)
_CLASSREGISTER_AddMember(performEffect, Potion::performEffect)
_CLASSREGISTER_AddMember(registerPotionAttributeModifier, Potion::registerPotionAttributeModifier)
_CLASSREGISTER_AddMember(removeAttributesModifiersFromEntity, Potion::removeAttributesModifiersFromEntity)
_CLASSREGISTER_AddMember(setPotionName, Potion::setPotionName)
_CLASSREGISTER_AddStaticMember(unInitialize, Potion::unInitialize)
_END_REGISTER_CLASS()
*/




// Register Class PotionAbsoption
_BEGIN_REGISTER_CLASS(PotionAbsoption)
_CLASSREGISTER_AddBaseClass(Potion)
_CLASSREGISTER_AddCtor(int , bool , int )
_CLASSREGISTER_AddMember(applyAttributesModifiersToEntity, PotionAbsoption::applyAttributesModifiersToEntity)
_CLASSREGISTER_AddMember(removeAttributesModifiersFromEntity, PotionAbsoption::removeAttributesModifiersFromEntity)
_END_REGISTER_CLASS()




// Register Class PotionAttackDamage
_BEGIN_REGISTER_CLASS(PotionAttackDamage)
_CLASSREGISTER_AddBaseClass(Potion)
_CLASSREGISTER_AddCtor(int , bool , int )
_CLASSREGISTER_AddMember(getAttributeModifierAmount, PotionAttackDamage::getAttributeModifierAmount)
_END_REGISTER_CLASS()




// Register Class PotionEffect
_BEGIN_REGISTER_CLASS(PotionEffect)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddCtor(int , int , int , bool )
_CLASSREGISTER_AddCtor(const PotionEffect & )
_CLASSREGISTER_AddMember(combine, PotionEffect::combine)
_CLASSREGISTER_AddMember(getAmplifier, PotionEffect::getAmplifier)
_CLASSREGISTER_AddMember(getDuration, PotionEffect::getDuration)
_CLASSREGISTER_AddMember(getEffectName, PotionEffect::getEffectName)
_CLASSREGISTER_AddMember(getIsAmbient, PotionEffect::getIsAmbient)
_CLASSREGISTER_AddMember(getIsPotionDurationMax, PotionEffect::getIsPotionDurationMax)
_CLASSREGISTER_AddMember(getIsSplashPotion, PotionEffect::getIsSplashPotion)
_CLASSREGISTER_AddMember(getPotionID, PotionEffect::getPotionID)
_CLASSREGISTER_AddMember(hasCode, PotionEffect::hasCode)
_CLASSREGISTER_AddMember(onUpdate, PotionEffect::onUpdate)
_CLASSREGISTER_AddMember(performEffect, PotionEffect::performEffect)
_CLASSREGISTER_AddStaticMember(readCustomPotionEffectFromNBT, PotionEffect::readCustomPotionEffectFromNBT)
_CLASSREGISTER_AddMember(setPotionDurationMax, PotionEffect::setPotionDurationMax)
_CLASSREGISTER_AddMember(setSplashPotion, PotionEffect::setSplashPotion)
_CLASSREGISTER_AddMember(toString, PotionEffect::toString)
_CLASSREGISTER_AddMember(writeCustomPotionEffectToNBT, PotionEffect::writeCustomPotionEffectToNBT)
_END_REGISTER_CLASS()




// Register Class PotionHealth
_BEGIN_REGISTER_CLASS(PotionHealth)
_CLASSREGISTER_AddBaseClass(Potion)
_CLASSREGISTER_AddCtor(int , bool , int )
_CLASSREGISTER_AddMember(isInstant, PotionHealth::isInstant)
_CLASSREGISTER_AddMember(isReady, PotionHealth::isReady)
_END_REGISTER_CLASS()




// Register Class PotionHealthBoost
_BEGIN_REGISTER_CLASS(PotionHealthBoost)
_CLASSREGISTER_AddBaseClass(Potion)
_CLASSREGISTER_AddCtor(int , bool , int )
_CLASSREGISTER_AddMember(removeAttributesModifiersFromEntity, PotionHealthBoost::removeAttributesModifiersFromEntity)
_END_REGISTER_CLASS()




// Register Class PotionHelper
_BEGIN_REGISTER_CLASS(PotionHelper)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(applyIngredient, PotionHelper::applyIngredient)
_CLASSREGISTER_AddStaticMember(calcPotionLiquidColor, PotionHelper::calcPotionLiquidColor)
_CLASSREGISTER_AddStaticMember(checkFlag, PotionHelper::checkFlag)
_CLASSREGISTER_AddStaticMember(func_77905_c, PotionHelper::func_77905_c)
_CLASSREGISTER_AddStaticMember(func_77908_a, PotionHelper::func_77908_a)
_CLASSREGISTER_AddStaticMember(func_77909_a, PotionHelper::func_77909_a)
_CLASSREGISTER_AddStaticMember(getColorFromDamage, PotionHelper::getColorFromDamage)
_CLASSREGISTER_AddStaticMember(getPotionEffects, PotionHelper::getPotionEffects)
_CLASSREGISTER_AddStaticMember(initialize, PotionHelper::initialize)
_CLASSREGISTER_AddStaticMember(isAmbient, PotionHelper::isAmbient)
_END_REGISTER_CLASS()




// Register Class PotionManager
_BEGIN_REGISTER_CLASS(PotionManager)
_CLASSREGISTER_AddStaticMember(initialize, PotionManager::initialize)
_CLASSREGISTER_AddStaticMember(registerPotionItem, PotionManager::registerPotionItem)
_CLASSREGISTER_AddStaticMember(unInitialize, PotionManager::unInitialize)
_END_REGISTER_CLASS()




_REGISTER_FUNC(validateRecipeJson)


// Register Class CraftingRecipe
_BEGIN_REGISTER_CLASS(CraftingRecipe)
_CLASSREGISTER_AddCtor(ShapedRecipes * )
_CLASSREGISTER_AddMember(getId, CraftingRecipe::getId)
_CLASSREGISTER_AddMember(getLimit, CraftingRecipe::getLimit)
_CLASSREGISTER_AddMember(getRecipeHeight, CraftingRecipe::getRecipeHeight)
_CLASSREGISTER_AddMember(getRecipeItems, CraftingRecipe::getRecipeItems)
_CLASSREGISTER_AddMember(getRecipeOutput, CraftingRecipe::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, CraftingRecipe::getRecipeSize)
_CLASSREGISTER_AddMember(getRecipeWidth, CraftingRecipe::getRecipeWidth)
_CLASSREGISTER_AddMember(materialsAvailable, CraftingRecipe::materialsAvailable)
_END_REGISTER_CLASS()




// Register Class FurnaceRecipes
_BEGIN_REGISTER_CLASS(FurnaceRecipes)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(addSmelting, FurnaceRecipes::addSmelting)
_CLASSREGISTER_AddMember(getExperience, FurnaceRecipes::getExperience)
_CLASSREGISTER_AddMember(getSmeltingList, FurnaceRecipes::getSmeltingList)
_CLASSREGISTER_AddMember(getSmeltingResult, FurnaceRecipes::getSmeltingResult)
_CLASSREGISTER_AddStaticMember(smelting, FurnaceRecipes::smelting)
_END_REGISTER_CLASS()




// Register Class IRecipe
_BEGIN_REGISTER_CLASS(IRecipe)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(getId, IRecipe::getId)
_CLASSREGISTER_AddMember(getName, IRecipe::getName)
_END_REGISTER_CLASS()




// Register Class RecipeFireworks
_BEGIN_REGISTER_CLASS(RecipeFireworks)
_CLASSREGISTER_AddBaseClass(IRecipe)
_CLASSREGISTER_AddMember(getCraftingResult, RecipeFireworks::getCraftingResult)
_CLASSREGISTER_AddMember(getRecipeOutput, RecipeFireworks::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, RecipeFireworks::getRecipeSize)
_CLASSREGISTER_AddMember(matches, RecipeFireworks::matches)
_END_REGISTER_CLASS()




// Register Class RecipesArmorDyes
_BEGIN_REGISTER_CLASS(RecipesArmorDyes)
_CLASSREGISTER_AddBaseClass(IRecipe)
_CLASSREGISTER_AddMember(getCraftingResult, RecipesArmorDyes::getCraftingResult)
_CLASSREGISTER_AddMember(getRecipeOutput, RecipesArmorDyes::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, RecipesArmorDyes::getRecipeSize)
_CLASSREGISTER_AddMember(matches, RecipesArmorDyes::matches)
_END_REGISTER_CLASS()




// Register Class RecipesMapCloning
_BEGIN_REGISTER_CLASS(RecipesMapCloning)
_CLASSREGISTER_AddBaseClass(IRecipe)
_CLASSREGISTER_AddMember(getCraftingResult, RecipesMapCloning::getCraftingResult)
_CLASSREGISTER_AddMember(getRecipeOutput, RecipesMapCloning::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, RecipesMapCloning::getRecipeSize)
_CLASSREGISTER_AddMember(matches, RecipesMapCloning::matches)
_END_REGISTER_CLASS()




// Register Class ShapedRecipes
_BEGIN_REGISTER_CLASS(ShapedRecipes)
_CLASSREGISTER_AddBaseClass(IRecipe)
_CLASSREGISTER_AddCtor(int , int , const IngredientPtrArr & , ItemStackPtr , const String & , const String & )
_CLASSREGISTER_AddMember(getCraftingResult, ShapedRecipes::getCraftingResult)
_CLASSREGISTER_AddMember(getRecipeHeight, ShapedRecipes::getRecipeHeight)
_CLASSREGISTER_AddMember(getRecipeItems, ShapedRecipes::getRecipeItems)
_CLASSREGISTER_AddMember(getRecipeOutput, ShapedRecipes::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, ShapedRecipes::getRecipeSize)
_CLASSREGISTER_AddMember(getRecipeWidth, ShapedRecipes::getRecipeWidth)
_CLASSREGISTER_AddMember(matches, ShapedRecipes::matches)
_CLASSREGISTER_AddMember(needsItem, ShapedRecipes::needsItem)
_CLASSREGISTER_AddMember(setCopyIngredientNBT, ShapedRecipes::setCopyIngredientNBT)
_END_REGISTER_CLASS()




// Register Class ShapelessRecipes
_BEGIN_REGISTER_CLASS(ShapelessRecipes)
_CLASSREGISTER_AddBaseClass(IRecipe)
_CLASSREGISTER_AddCtor(ItemStackPtr , const IngredientPtrArr & , const String & , const String & )
_CLASSREGISTER_AddMember(getCraftingResult, ShapelessRecipes::getCraftingResult)
_CLASSREGISTER_AddMember(getRecipeOutput, ShapelessRecipes::getRecipeOutput)
_CLASSREGISTER_AddMember(getRecipeSize, ShapelessRecipes::getRecipeSize)
_CLASSREGISTER_AddMember(matches, ShapelessRecipes::matches)
_END_REGISTER_CLASS()




