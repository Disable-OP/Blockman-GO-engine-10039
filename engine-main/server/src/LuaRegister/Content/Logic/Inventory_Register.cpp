
#include "Inventory/Container.h"
#include "Inventory/Crafting.h"
#include "Inventory/CraftingManager.h"
#include "Inventory/IInventory.h"
#include "Inventory/InventoryCraftable.h"
#include "Inventory/InventoryCrafting.h"
#include "Inventory/InventoryEnderChest.h"
#include "Inventory/InventoryFiltered.h"
#include "Inventory/InventoryHandCraftable.h"
#include "Inventory/InventoryLargeChest.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryProxy.h"
#include "Inventory/InventorySubset.h"
#include "Inventory/Slot.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;


// Register Class Container
_BEGIN_REGISTER_CLASS(Container)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(addCraftingToCrafters, Container::addCraftingToCrafters)
_CLASSREGISTER_AddStaticMember(calcRedstoneFromInventory, Container::calcRedstoneFromInventory)
_CLASSREGISTER_AddStaticMember(canAddItemToSlot, Container::canAddItemToSlot)
_CLASSREGISTER_AddMember(canDragIntoSlot, Container::canDragIntoSlot)
_CLASSREGISTER_AddMember(canMergeSlot, Container::canMergeSlot)
_CLASSREGISTER_AddStaticMember(computeStackSize, Container::computeStackSize)
_CLASSREGISTER_AddMember(detectAndSendChanges, Container::detectAndSendChanges)
_CLASSREGISTER_AddMember(enchantItem, Container::enchantItem)
_CLASSREGISTER_AddStaticMember(extractDragMode, Container::extractDragMode)
_CLASSREGISTER_AddStaticMember(getDragEvent, Container::getDragEvent)
_CLASSREGISTER_AddMember(getInventory, Container::getInventory)
_CLASSREGISTER_AddMember(getNextTransactionID, Container::getNextTransactionID)
_CLASSREGISTER_AddStaticMember(getQuickcraftMask, Container::getQuickcraftMask)
_CLASSREGISTER_AddMember(getSlot, Container::getSlot)
_CLASSREGISTER_AddMember(getSlotFromInventory, Container::getSlotFromInventory)
_CLASSREGISTER_AddMember(isPlayerNotUsingContainer, Container::isPlayerNotUsingContainer)
_CLASSREGISTER_AddStaticMember(isValidDragMode, Container::isValidDragMode)
_CLASSREGISTER_AddMember(onContainerClosed, Container::onContainerClosed)
_CLASSREGISTER_AddMember(onCraftMatrixChanged, Container::onCraftMatrixChanged)
_CLASSREGISTER_AddMember(putStackInSlot, Container::putStackInSlot)
_CLASSREGISTER_AddMember(putStacksInSlots, Container::putStacksInSlots)
_CLASSREGISTER_AddMember(removeCraftingFromCrafters, Container::removeCraftingFromCrafters)
_CLASSREGISTER_AddMember(setPlayerIsPresent, Container::setPlayerIsPresent)
_CLASSREGISTER_AddMember(slotClick, Container::slotClick)
_CLASSREGISTER_AddMember(transferStackInSlot, Container::transferStackInSlot)
_CLASSREGISTER_AddMember(updateProgressBar, Container::updateProgressBar)
_END_REGISTER_CLASS()




// Register Class ContainerChest
_BEGIN_REGISTER_CLASS(ContainerChest)
_CLASSREGISTER_AddBaseClass(Container)
_CLASSREGISTER_AddCtor(IInventory * , IInventory * )
_CLASSREGISTER_AddMember(canInteractWith, ContainerChest::canInteractWith)
_CLASSREGISTER_AddMember(getLowerChestInventory, ContainerChest::getLowerChestInventory)
_CLASSREGISTER_AddMember(onContainerClosed, ContainerChest::onContainerClosed)
_CLASSREGISTER_AddMember(transferStackInSlot, ContainerChest::transferStackInSlot)
_END_REGISTER_CLASS()




// Register Class ContainerPlayer
_BEGIN_REGISTER_CLASS(ContainerPlayer)
_CLASSREGISTER_AddBaseClass(Container)
_CLASSREGISTER_AddCtor(InventoryPlayer * , bool , EntityPlayer * )
_CLASSREGISTER_AddMember(canInteractWith, ContainerPlayer::canInteractWith)
_CLASSREGISTER_AddMember(canMergeSlot, ContainerPlayer::canMergeSlot)
_CLASSREGISTER_AddMember(onContainerClosed, ContainerPlayer::onContainerClosed)
_CLASSREGISTER_AddMember(onCraftMatrixChanged, ContainerPlayer::onCraftMatrixChanged)
_CLASSREGISTER_AddMember(transferStackInSlot, ContainerPlayer::transferStackInSlot)
_END_REGISTER_CLASS()






// Register Class CraftingManager
_BEGIN_REGISTER_CLASS(CraftingManager)
_CLASSREGISTER_AddBaseClass(Singleton<CraftingManager>)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(findMatchingRecipe, CraftingManager::findMatchingRecipe)
_CLASSREGISTER_AddMember(getRecipe, CraftingManager::getRecipe)
_CLASSREGISTER_AddMember(getRecipeList, CraftingManager::getRecipeList)
_CLASSREGISTER_AddMember(getRecipeNameList, CraftingManager::getRecipeNameList)
_CLASSREGISTER_AddMember(loadRecipe, CraftingManager::loadRecipe)
_CLASSREGISTER_AddMember(parseRecipeJson, CraftingManager::parseRecipeJson)
_END_REGISTER_CLASS()






// Register Class IInventory
_BEGIN_REGISTER_CLASS(IInventory)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(findCanSaveItemStackById, IInventory::findCanSaveItemStackById)
_CLASSREGISTER_AddMember(findItemStack, IInventory::findItemStack)
_CLASSREGISTER_AddMember(onInventoryChanged, IInventory::onInventoryChanged)
_CLASSREGISTER_AddMember(swapStacksInSlots, IInventory::swapStacksInSlots)
_CLASSREGISTER_AddMember(getItemNum, IInventory::getItemNum)
_CLASSREGISTER_AddMember(removeItem, IInventory::removeItem)
_CLASSREGISTER_AddMember(clear, IInventory::clear)
_END_REGISTER_CLASS()






// Register Class InventoryCraftable
_BEGIN_REGISTER_CLASS(InventoryCraftable)
_CLASSREGISTER_AddBaseClass(InventoryProxy)
_CLASSREGISTER_AddCtor(IInventory * )
_CLASSREGISTER_AddMember(decrStackSize, InventoryCraftable::decrStackSize)
_CLASSREGISTER_AddMember(getRecipe, InventoryCraftable::getRecipe)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryCraftable::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryCraftable::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryCraftable::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryCraftable::isItemValidForSlot)
_CLASSREGISTER_AddMember(onUpdate, InventoryCraftable::onUpdate)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryCraftable::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventoryCraftResult
_BEGIN_REGISTER_CLASS(InventoryCraftResult)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(closeChest, InventoryCraftResult::closeChest)
_CLASSREGISTER_AddMember(decrStackSize, InventoryCraftResult::decrStackSize)
_CLASSREGISTER_AddMember(getInvName, InventoryCraftResult::getInvName)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryCraftResult::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryCraftResult::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryCraftResult::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryCraftResult::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryCraftResult::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryCraftResult::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryCraftResult::isUseableByPlayer)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryCraftResult::onInventoryChanged)
_CLASSREGISTER_AddMember(openChest, InventoryCraftResult::openChest)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryCraftResult::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventoryCrafting
_BEGIN_REGISTER_CLASS(InventoryCrafting)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor(Container * , int , int )
_CLASSREGISTER_AddMember(closeChest, InventoryCrafting::closeChest)
_CLASSREGISTER_AddMember(decrStackSize, InventoryCrafting::decrStackSize)
_CLASSREGISTER_AddMember(getInvName, InventoryCrafting::getInvName)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryCrafting::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryCrafting::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInRowAndColumn, InventoryCrafting::getStackInRowAndColumn)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryCrafting::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryCrafting::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryCrafting::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryCrafting::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryCrafting::isUseableByPlayer)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryCrafting::onInventoryChanged)
_CLASSREGISTER_AddMember(openChest, InventoryCrafting::openChest)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryCrafting::setInventorySlotContents)
_END_REGISTER_CLASS()






// Register Class InventoryBasic
_BEGIN_REGISTER_CLASS(InventoryBasic)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor(const String & , bool , int )
_CLASSREGISTER_AddMember(addInventoryContent, InventoryBasic::addInventoryContent)
_CLASSREGISTER_AddMember(closeChest, InventoryBasic::closeChest)
_CLASSREGISTER_AddMember(decrStackSize, InventoryBasic::decrStackSize)
_CLASSREGISTER_AddMember(getInvName, InventoryBasic::getInvName)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryBasic::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryBasic::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryBasic::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryBasic::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryBasic::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryBasic::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryBasic::isUseableByPlayer)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryBasic::onInventoryChanged)
_CLASSREGISTER_AddMember(openChest, InventoryBasic::openChest)
_CLASSREGISTER_AddMember(removeInventoryContent, InventoryBasic::removeInventoryContent)
_CLASSREGISTER_AddMember(setCustomTitle, InventoryBasic::setCustomTitle)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryBasic::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventoryEnderChest
_BEGIN_REGISTER_CLASS(InventoryEnderChest)
_CLASSREGISTER_AddBaseClass(InventoryBasic)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(closeChest, InventoryEnderChest::closeChest)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryEnderChest::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryEnderChest::isUseableByPlayer)
_CLASSREGISTER_AddMember(loadInventoryFromNBT, InventoryEnderChest::loadInventoryFromNBT)
_CLASSREGISTER_AddMember(openChest, InventoryEnderChest::openChest)
_CLASSREGISTER_AddMember(saveInventoryToNBT, InventoryEnderChest::saveInventoryToNBT)
_CLASSREGISTER_AddMember(setAssociatedChest, InventoryEnderChest::setAssociatedChest)
_CLASSREGISTER_AddMember(getItemStackInfo, InventoryEnderChest::getItemStackInfo)
_END_REGISTER_CLASS()




// Register Class InventoryFiltered
_BEGIN_REGISTER_CLASS(InventoryFiltered)
_CLASSREGISTER_AddBaseClass(InventoryProxy)
_CLASSREGISTER_AddCtor(IInventory * , std::function<bool(int)> )
_CLASSREGISTER_AddMember(decrStackSize, InventoryFiltered::decrStackSize)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryFiltered::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryFiltered::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryFiltered::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryFiltered::isItemValidForSlot)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryFiltered::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventoryHandCraftable
_BEGIN_REGISTER_CLASS(InventoryHandCraftable)
_CLASSREGISTER_AddBaseClass(InventoryCraftable)
_CLASSREGISTER_AddCtor(IInventory * )
_END_REGISTER_CLASS()




// Register Class InventoryLargeChest
_BEGIN_REGISTER_CLASS(InventoryLargeChest)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor(const String & , IInventory * , IInventory * )
_CLASSREGISTER_AddMember(closeChest, InventoryLargeChest::closeChest)
_CLASSREGISTER_AddMember(decrStackSize, InventoryLargeChest::decrStackSize)
_CLASSREGISTER_AddMember(getInvName, InventoryLargeChest::getInvName)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryLargeChest::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryLargeChest::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryLargeChest::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryLargeChest::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryLargeChest::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryLargeChest::isItemValidForSlot)
_CLASSREGISTER_AddMember(isPartOfLargeChest, InventoryLargeChest::isPartOfLargeChest)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryLargeChest::isUseableByPlayer)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryLargeChest::onInventoryChanged)
_CLASSREGISTER_AddMember(openChest, InventoryLargeChest::openChest)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryLargeChest::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventoryPlayer
_BEGIN_REGISTER_CLASS(InventoryPlayer)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor(EntityPlayer * )
_CLASSREGISTER_AddMember(addItemStackToInventory, InventoryPlayer::addItemStackToInventory)
_CLASSREGISTER_AddMember(addItemToInventory, InventoryPlayer::addItemToInventory)
_CLASSREGISTER_AddMember(armorItemInSlot, InventoryPlayer::armorItemInSlot)
_CLASSREGISTER_AddMember(canHarvestBlock, InventoryPlayer::canHarvestBlock)
_CLASSREGISTER_AddMember(changeCurrentItem, InventoryPlayer::changeCurrentItem)
_CLASSREGISTER_AddMember(changeCurrentItemToNext, InventoryPlayer::changeCurrentItemToNext)
_CLASSREGISTER_AddMember(clearInventory, InventoryPlayer::clearInventory)
_CLASSREGISTER_AddMember(closeChest, InventoryPlayer::closeChest)
_CLASSREGISTER_AddMember(consumeInventoryItem, InventoryPlayer::consumeInventoryItem)
_CLASSREGISTER_AddMember(copyInventory, InventoryPlayer::copyInventory)
_CLASSREGISTER_AddMember(damageArmor, InventoryPlayer::damageArmor)
_CLASSREGISTER_AddMember(decrStackSize, InventoryPlayer::decrStackSize)
_CLASSREGISTER_AddMember(decrementAnimations, InventoryPlayer::decrementAnimations)
_CLASSREGISTER_AddMember(dropAllItems, InventoryPlayer::dropAllItems)
_CLASSREGISTER_AddMember(equipArmor, InventoryPlayer::equipArmor)
_CLASSREGISTER_AddMember(getArmorFilteredInventory, InventoryPlayer::getArmorFilteredInventory)
_CLASSREGISTER_AddMember(getArmorInventory, InventoryPlayer::getArmorInventory)
_CLASSREGISTER_AddMember(getCurrentItem, InventoryPlayer::getCurrentItem)
_CLASSREGISTER_AddMember(getFirstEmptyStackInHotbar, InventoryPlayer::getFirstEmptyStackInHotbar)
_CLASSREGISTER_AddMember(getFirstEmptyStackInInventory, InventoryPlayer::getFirstEmptyStackInInventory)
_CLASSREGISTER_AddStaticMember(getHotbarSize, InventoryPlayer::getHotbarSize)
_CLASSREGISTER_AddMember(getInvName, InventoryPlayer::getInvName)
_CLASSREGISTER_AddMember(getInventoryAllCraftable, InventoryPlayer::getInventoryAllCraftable)
_CLASSREGISTER_AddMember(getInventoryHandCraftable, InventoryPlayer::getInventoryHandCraftable)
_CLASSREGISTER_AddMember(getInventoryIndexOfCurrentItem, InventoryPlayer::getInventoryIndexOfCurrentItem)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryPlayer::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getItemStack, InventoryPlayer::getItemStack)
_CLASSREGISTER_AddMember(getMainInventory, InventoryPlayer::getMainInventory)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryPlayer::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryPlayer::getStackInSlot)
_CLASSREGISTER_AddMember(getItemStackInfo, InventoryPlayer::getItemStackInfo)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryPlayer::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(getStrVsBlock, InventoryPlayer::getStrVsBlock)
_CLASSREGISTER_AddMember(getTotalArmorValue, InventoryPlayer::getTotalArmorValue)
_CLASSREGISTER_AddMember(hasItem, InventoryPlayer::hasItem)
_CLASSREGISTER_AddMember(hasItemInSlot, InventoryPlayer::hasItemInSlot)
_CLASSREGISTER_AddMember(hasItemStack, InventoryPlayer::hasItemStack)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryPlayer::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryPlayer::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryPlayer::isUseableByPlayer)
_CLASSREGISTER_AddMember(notifyUpdateToProxy, InventoryPlayer::notifyUpdateToProxy)
//_CLASSREGISTER_AddMember(onItemInHandChanged, InventoryPlayer::onItemInHandChanged)
//_CLASSREGISTER_AddMember(onInventoryContentChanged, InventoryPlayer::onInventoryContentChanged)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryPlayer::onInventoryChanged)
_CLASSREGISTER_AddMember(openChest, InventoryPlayer::openChest)
_CLASSREGISTER_AddMember(putItemToHotbar, InventoryPlayer::putItemToHotbar)
_CLASSREGISTER_AddMember(readFromNBT, InventoryPlayer::readFromNBT)
_CLASSREGISTER_AddMember(removeCurrentItemFromHotbar, InventoryPlayer::removeCurrentItemFromHotbar)
_CLASSREGISTER_AddMember(removeItemFromHotbar, InventoryPlayer::removeItemFromHotbar)
_CLASSREGISTER_AddMember_Override(setCurrentItem, InventoryPlayer::setCurrentItem, void, ItemStackPtr)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryPlayer::setInventorySlotContents)
_CLASSREGISTER_AddMember(setItemStack, InventoryPlayer::setItemStack)
_CLASSREGISTER_AddMember(storePartialItemStack, InventoryPlayer::storePartialItemStack)
_CLASSREGISTER_AddMember(unequipArmor, InventoryPlayer::unequipArmor)
_CLASSREGISTER_AddMember(writeToNBT, InventoryPlayer::writeToNBT)
_CLASSREGISTER_AddMember(isCanAddItem, InventoryPlayer::isCanAddItem)
_CLASSREGISTER_AddMember(isEnchantment, InventoryPlayer::isEnchantment)
_CLASSREGISTER_AddMember(getItemStackEnchantmentId, InventoryPlayer::getItemStackEnchantmentId)
_CLASSREGISTER_AddMember(getItemStackEnchantmentLv, InventoryPlayer::getItemStackEnchantmentLv)
_END_REGISTER_CLASS()




// Register Class InventoryProxy
_BEGIN_REGISTER_CLASS(InventoryProxy)
_CLASSREGISTER_AddBaseClass(IInventory)
_CLASSREGISTER_AddCtor(IInventory * )
_CLASSREGISTER_AddMember(closeChest, InventoryProxy::closeChest)
_CLASSREGISTER_AddMember(decrStackSize, InventoryProxy::decrStackSize)
_CLASSREGISTER_AddMember(getInvName, InventoryProxy::getInvName)
_CLASSREGISTER_AddMember(getInventoryStackLimit, InventoryProxy::getInventoryStackLimit)
_CLASSREGISTER_AddMember(getSizeInventory, InventoryProxy::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventoryProxy::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventoryProxy::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(isInvNameLocalized, InventoryProxy::isInvNameLocalized)
_CLASSREGISTER_AddMember(isItemValidForSlot, InventoryProxy::isItemValidForSlot)
_CLASSREGISTER_AddMember(isUseableByPlayer, InventoryProxy::isUseableByPlayer)
_CLASSREGISTER_AddMember(onInventoryChanged, InventoryProxy::onInventoryChanged)
_CLASSREGISTER_AddMember(onUpdate, InventoryProxy::onUpdate)
_CLASSREGISTER_AddMember(openChest, InventoryProxy::openChest)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventoryProxy::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class InventorySubset
_BEGIN_REGISTER_CLASS(InventorySubset)
_CLASSREGISTER_AddBaseClass(InventoryProxy)
_CLASSREGISTER_AddCtor(IInventory * , size_t , size_t )
_CLASSREGISTER_AddMember(decrStackSize, InventorySubset::decrStackSize)
_CLASSREGISTER_AddMember(getSizeInventory, InventorySubset::getSizeInventory)
_CLASSREGISTER_AddMember(getStackInSlot, InventorySubset::getStackInSlot)
_CLASSREGISTER_AddMember(getStackInSlotOnClosing, InventorySubset::getStackInSlotOnClosing)
_CLASSREGISTER_AddMember(setInventorySlotContents, InventorySubset::setInventorySlotContents)
_END_REGISTER_CLASS()




// Register Class Slot
_BEGIN_REGISTER_CLASS(Slot)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(IInventory * , int , int , int )
_CLASSREGISTER_AddMember(canBeHovered, Slot::canBeHovered)
_CLASSREGISTER_AddMember(canTakeStack, Slot::canTakeStack)
_CLASSREGISTER_AddMember(decrStackSize, Slot::decrStackSize)
_CLASSREGISTER_AddMember(getHasStack, Slot::getHasStack)
_CLASSREGISTER_AddMember(getSlotStackLimit, Slot::getSlotStackLimit)
_CLASSREGISTER_AddMember(getStack, Slot::getStack)
_CLASSREGISTER_AddMember(isItemValid, Slot::isItemValid)
_CLASSREGISTER_AddMember(isSlotInInventory, Slot::isSlotInInventory)
_CLASSREGISTER_AddMember(onPickupFromSlot, Slot::onPickupFromSlot)
_CLASSREGISTER_AddMember(onSlotChange, Slot::onSlotChange)
_CLASSREGISTER_AddMember(onSlotChanged, Slot::onSlotChanged)
_CLASSREGISTER_AddMember(putStack, Slot::putStack)
_END_REGISTER_CLASS()




// Register Class SlotArmor
_BEGIN_REGISTER_CLASS(SlotArmor)
_CLASSREGISTER_AddBaseClass(Slot)
_CLASSREGISTER_AddCtor(ContainerPlayer * , IInventory * , int , int , int , int )
_CLASSREGISTER_AddMember(getSlotStackLimit, SlotArmor::getSlotStackLimit)
_CLASSREGISTER_AddMember(isItemValid, SlotArmor::isItemValid)
_END_REGISTER_CLASS()




// Register Class SlotCrafting
_BEGIN_REGISTER_CLASS(SlotCrafting)
_CLASSREGISTER_AddBaseClass(Slot)
_CLASSREGISTER_AddCtor(EntityPlayer * , IInventory * , IInventory * , int , int , int )
_CLASSREGISTER_AddMember(decrStackSize, SlotCrafting::decrStackSize)
_CLASSREGISTER_AddMember(isItemValid, SlotCrafting::isItemValid)
_CLASSREGISTER_AddMember(onPickupFromSlot, SlotCrafting::onPickupFromSlot)
_END_REGISTER_CLASS()




