#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Block/Blocks.h"
#include "Block/BM_Material.h"
#include "World/RayTracyResult.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;


// Register Class Block
_BEGIN_REGISTER_CLASS(Block)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, Block::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(breakBlock, Block::breakBlock)
_CLASSREGISTER_AddMember(canBlockStay, Block::canBlockStay)
_CLASSREGISTER_AddMember(canCollideCheck, Block::canCollideCheck)
_CLASSREGISTER_AddMember(canDropFromExplosion, Block::canDropFromExplosion)
_CLASSREGISTER_AddMember(canPlaceBlockAt, Block::canPlaceBlockAt)
_CLASSREGISTER_AddMember_Override(canPlaceBlockOnSide, Block::canPlaceBlockOnSide,bool ,World * ,const BlockPos & ,int ,ItemStackPtr )
_CLASSREGISTER_AddMember_Override(canPlaceBlockOnSide1, Block::canPlaceBlockOnSide,bool ,World * ,const BlockPos & ,int )
_CLASSREGISTER_AddMember(canProvidePower, Block::canProvidePower)
_CLASSREGISTER_AddMember(canSilkHarvest, Block::canSilkHarvest)
_CLASSREGISTER_AddMember(checkType, Block::checkType)
_CLASSREGISTER_AddMember(collisionRayTrace, Block::collisionRayTrace)
_CLASSREGISTER_AddMember(createStackedBlock, Block::createStackedBlock)
_CLASSREGISTER_AddMember(damageDropped, Block::damageDropped)
_CLASSREGISTER_AddMember(disableStats, Block::disableStats)
_CLASSREGISTER_AddMember(dropBlockAsItem, Block::dropBlockAsItem)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, Block::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(dropBlockAsItem_do, Block::dropBlockAsItem_do)
_CLASSREGISTER_AddMember(dropXpOnBlockBreak, Block::dropXpOnBlockBreak)
_CLASSREGISTER_AddMember(fillWithRain, Block::fillWithRain)
_CLASSREGISTER_AddMember(getBlockBoundsMax, Block::getBlockBoundsMax)
_CLASSREGISTER_AddMember(getBlockBoundsMaxX, Block::getBlockBoundsMaxX)
_CLASSREGISTER_AddMember(getBlockBoundsMaxY, Block::getBlockBoundsMaxY)
_CLASSREGISTER_AddMember(getBlockBoundsMaxZ, Block::getBlockBoundsMaxZ)
_CLASSREGISTER_AddMember(getBlockBoundsMin, Block::getBlockBoundsMin)
_CLASSREGISTER_AddMember(getBlockBoundsMinX, Block::getBlockBoundsMinX)
_CLASSREGISTER_AddMember(getBlockBoundsMinY, Block::getBlockBoundsMinY)
_CLASSREGISTER_AddMember(getBlockBoundsMinZ, Block::getBlockBoundsMinZ)
_CLASSREGISTER_AddMember(getBlockHardness, Block::getBlockHardness)
_CLASSREGISTER_AddMember(getBlockID, Block::getBlockID)
_CLASSREGISTER_AddMember(
	sMovement, Block::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, Block::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(getComparatorInputOverride, Block::getComparatorInputOverride)
_CLASSREGISTER_AddMember(getDamageValue, Block::getDamageValue)
_CLASSREGISTER_AddMember(getEnableStats, Block::getEnableStats)
_CLASSREGISTER_AddMember(getExplosionResistance, Block::getExplosionResistance)
_CLASSREGISTER_AddMember(getHardness, Block::getHardness)
_CLASSREGISTER_AddMember(getLocalizedName, Block::getLocalizedName)
_CLASSREGISTER_AddMember(getMaterial, Block::getMaterial)
_CLASSREGISTER_AddMember(getMobilityFlag, Block::getMobilityFlag)
_CLASSREGISTER_AddMember(getParticleGravity, Block::getParticleGravity)
_CLASSREGISTER_AddMember(getPlayerRelativeBlockHardness, Block::getPlayerRelativeBlockHardness)
_CLASSREGISTER_AddMember(getResistance, Block::getResistance)
_CLASSREGISTER_AddMember(getSelectedBoundingBox, Block::getSelectedBoundingBox)
_CLASSREGISTER_AddMember(getTickRandomly, Block::getTickRandomly)
_CLASSREGISTER_AddMember(getUnlocalizedName, Block::getUnlocalizedName)
_CLASSREGISTER_AddMember(harvestBlock, Block::harvestBlock)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, Block::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(hasTileEntity, Block::hasTileEntity)
_CLASSREGISTER_AddMember(idDropped, Block::idDropped)
_CLASSREGISTER_AddMember(idPicked, Block::idPicked)
_CLASSREGISTER_AddMember(initializeBlock, Block::initializeBlock)
//_CLASSREGISTER_AddMember(isAssociatedBlockID, Block::isAssociatedBlockID)
_CLASSREGISTER_AddStaticMember_Override(isAssociatedBlockID, Block::isAssociatedBlockID)
_CLASSREGISTER_AddMember(isBlockSolid, Block::isBlockSolid)
_CLASSREGISTER_AddMember(isCollidable, Block::isCollidable)
_CLASSREGISTER_AddMember(isFlowerPot, Block::isFlowerPot)
_CLASSREGISTER_AddMember(isOpaqueCube, Block::isOpaqueCube)
_CLASSREGISTER_AddMember(isProvidingStrongPower, Block::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, Block::isProvidingWeakPower)
_CLASSREGISTER_AddMember(isUpdateScheduled, Block::isUpdateScheduled)
_CLASSREGISTER_AddMember(isVecInsideXYBounds, Block::isVecInsideXYBounds)
_CLASSREGISTER_AddMember(isVecInsideXZBounds, Block::isVecInsideXZBounds)
_CLASSREGISTER_AddMember(isVecInsideYZBounds, Block::isVecInsideYZBounds)
_CLASSREGISTER_AddMember(onBlockActivated, Block::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, Block::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockClicked, Block::onBlockClicked)
_CLASSREGISTER_AddMember(onBlockDestroyedByExplosion, Block::onBlockDestroyedByExplosion)
_CLASSREGISTER_AddMember(onBlockDestroyedByPlayer, Block::onBlockDestroyedByPlayer)
_CLASSREGISTER_AddMember(onBlockEventReceived, Block::onBlockEventReceived)
_CLASSREGISTER_AddMember(onBlockHarvested, Block::onBlockHarvested)
_CLASSREGISTER_AddMember(onBlockPlaced, Block::onBlockPlaced)
_CLASSREGISTER_AddMember(onBlockPlacedBy, Block::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, Block::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onEntityWalking, Block::onEntityWalking)
_CLASSREGISTER_AddMember(onFallenUpon, Block::onFallenUpon)
_CLASSREGISTER_AddMember(onNeighborBlockChange, Block::onNeighborBlockChange)
_CLASSREGISTER_AddMember(onPostBlockPlaced, Block::onPostBlockPlaced)
_CLASSREGISTER_AddMember(onSetBlockIDWithMetaData, Block::onSetBlockIDWithMetaData)
_CLASSREGISTER_AddMember(quantityDropped, Block::quantityDropped)
_CLASSREGISTER_AddMember(quantityDroppedWithBonus, Block::quantityDroppedWithBonus)
_CLASSREGISTER_AddMember(renderAsNormalBlock, Block::renderAsNormalBlock)
_CLASSREGISTER_AddMember_Override(setBlockBounds, Block::setBlockBounds,void ,float ,float ,float ,float ,float ,float )
_CLASSREGISTER_AddMember_Override(setBlockBounds1, Block::setBlockBounds,void ,const Vector3 & ,const Vector3 & )
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, Block::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, Block::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(setBlockUnbreakable, Block::setBlockUnbreakable)
_CLASSREGISTER_AddMember(setHardness, Block::setHardness)
_CLASSREGISTER_AddMember(setLightOpacity, Block::setLightOpacity)
_CLASSREGISTER_AddMember(setLightValue, Block::setLightValue)
_CLASSREGISTER_AddMember(setResistance, Block::setResistance)
_CLASSREGISTER_AddMember(setTickRandomly, Block::setTickRandomly)
_CLASSREGISTER_AddMember(shouldSideBeRendered, Block::shouldSideBeRendered)
_CLASSREGISTER_AddMember(slipperiness, Block::slipperiness)
_CLASSREGISTER_AddMember(tickRate, Block::tickRate)
_CLASSREGISTER_AddMember(updateTick, Block::updateTick)
_CLASSREGISTER_AddMember(velocityToAddToEntity, Block::velocityToAddToEntity)
_END_REGISTER_CLASS()




// Register Class BlockManager
_BEGIN_REGISTER_CLASS(BlockManager)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(initialize, BlockManager::initialize)
_CLASSREGISTER_AddStaticMember(isAssociatedBlockID, BlockManager::isAssociatedBlockID)
_CLASSREGISTER_AddStaticMember(isNormalCube, BlockManager::isNormalCube)
_CLASSREGISTER_AddStaticMember(onBlockCreated, BlockManager::onBlockCreated)
_CLASSREGISTER_AddStaticMember(registBlockItems, BlockManager::registBlockItems)
_CLASSREGISTER_AddStaticMember(setLightOpacity, BlockManager::setLightOpacity)
_CLASSREGISTER_AddStaticMember(setLightValue, BlockManager::setLightValue)
_CLASSREGISTER_AddStaticMember(unInitialize, BlockManager::unInitialize)
_END_REGISTER_CLASS()




// Register Class BlockAnvil
_BEGIN_REGISTER_CLASS(BlockAnvil)
_CLASSREGISTER_AddBaseClass(BlockSand)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockAnvil::damageDropped)
_CLASSREGISTER_AddMember(onBlockActivated, BlockAnvil::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockAnvil::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onFinishFalling, BlockAnvil::onFinishFalling)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockAnvil::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockAnvil::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockBasePressurePlate
_BEGIN_REGISTER_CLASS(BlockBasePressurePlate)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddMember(breakBlock, BlockBasePressurePlate::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockBasePressurePlate::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canProvidePower, BlockBasePressurePlate::canProvidePower)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockBasePressurePlate::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockBasePressurePlate::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(getMobilityFlag, BlockBasePressurePlate::getMobilityFlag)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockBasePressurePlate::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockBasePressurePlate::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockBasePressurePlate::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockBasePressurePlate::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockBasePressurePlate::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockBasePressurePlate::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockBasePressurePlate::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(tickRate, BlockBasePressurePlate::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockBasePressurePlate::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockBeacon
_BEGIN_REGISTER_CLASS(BlockBeacon)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockBeacon::createNewTileEntity)
_CLASSREGISTER_AddMember(onBlockActivated, BlockBeacon::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockBeacon::onBlockPlacedBy)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockBeacon::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockBed
_BEGIN_REGISTER_CLASS(BlockBed)
_CLASSREGISTER_AddBaseClass(BlockDirectional)
_CLASSREGISTER_AddCtor(int , const String & )
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockBed::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(getMobilityFlag, BlockBed::getMobilityFlag)
_CLASSREGISTER_AddStaticMember(getNearestEmptyChunkCoordinates, BlockBed::getNearestEmptyChunkCoordinates)
_CLASSREGISTER_AddMember(idDropped, BlockBed::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockBed::idPicked)
_CLASSREGISTER_AddStaticMember(isBedOccupied, BlockBed::isBedOccupied)
_CLASSREGISTER_AddStaticMember(isBlockHeadOfBed, BlockBed::isBlockHeadOfBed)
_CLASSREGISTER_AddMember(onBlockActivated, BlockBed::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockHarvested, BlockBed::onBlockHarvested)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockBed::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockBed::renderAsNormalBlock)
_CLASSREGISTER_AddStaticMember(setBedOccupied, BlockBed::setBedOccupied)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockBed::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockBookshelf
_BEGIN_REGISTER_CLASS(BlockBookshelf)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockBookshelf::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockBookshelf::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockBreakable
_BEGIN_REGISTER_CLASS(BlockBreakable)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_END_REGISTER_CLASS()




// Register Class BlockBrewingStand
_BEGIN_REGISTER_CLASS(BlockBrewingStand)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockBrewingStand::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(breakBlock, BlockBrewingStand::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockBrewingStand::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockBrewingStand::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockBrewingStand::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(idDropped, BlockBrewingStand::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockBrewingStand::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockBrewingStand::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockBrewingStand::onBlockPlacedBy)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockBrewingStand::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockBrewingStand::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockButton
_BEGIN_REGISTER_CLASS(BlockButton)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddMember(breakBlock, BlockButton::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockButton::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockButton::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(canProvidePower, BlockButton::canProvidePower)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockButton::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockButton::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockButton::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockActivated, BlockButton::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockButton::onBlockPlaced)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockButton::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockButton::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockButton::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockButton::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockButton::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(tickRate, BlockButton::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockButton::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockButtonStone
_BEGIN_REGISTER_CLASS(BlockButtonStone)
_CLASSREGISTER_AddBaseClass(BlockButton)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_END_REGISTER_CLASS()




// Register Class BlockButtonWood
_BEGIN_REGISTER_CLASS(BlockButtonWood)
_CLASSREGISTER_AddBaseClass(BlockButton)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_END_REGISTER_CLASS()




// Register Class BlockCactus
_BEGIN_REGISTER_CLASS(BlockCactus)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockCactus::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockCactus::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockCactus::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockCactus::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockCactus::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockCactus::renderAsNormalBlock)
_CLASSREGISTER_AddMember(updateTick, BlockCactus::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockCake
_BEGIN_REGISTER_CLASS(BlockCake)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockCake::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockCake::canPlaceBlockAt)
_CLASSREGISTER_AddMember(eatCakeSlice, BlockCake::eatCakeSlice)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockCake::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockCake::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockCake::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockCake::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockClicked, BlockCake::onBlockClicked)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockCake::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockCake::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockCake::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockCake::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockCake::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockCarpet
_BEGIN_REGISTER_CLASS(BlockCarpet)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockCarpet::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockCarpet::canPlaceBlockAt)
_CLASSREGISTER_AddMember(damageDropped, BlockCarpet::damageDropped)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockCarpet::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockCarpet::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockCarpet::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockCarpet::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockCarpet::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockCarrot
_BEGIN_REGISTER_CLASS(BlockCarrot)
_CLASSREGISTER_AddBaseClass(BlockCrops)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getCropItem, BlockCarrot::getCropItem)
_CLASSREGISTER_AddMember(getSeedItem, BlockCarrot::getSeedItem)
_END_REGISTER_CLASS()




// Register Class BlockCauldron
_BEGIN_REGISTER_CLASS(BlockCauldron)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockCauldron::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(fillWithRain, BlockCauldron::fillWithRain)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockCauldron::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockCauldron::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(idDropped, BlockCauldron::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockCauldron::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockCauldron::onBlockActivated)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockCauldron::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockCauldron::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockChest
_BEGIN_REGISTER_CLASS(BlockChest)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , int )
_CLASSREGISTER_AddMember(breakBlock, BlockChest::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockChest::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canProvidePower, BlockChest::canProvidePower)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockChest::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockChest::getComparatorInputOverride)
_CLASSREGISTER_AddMember(getInventory, BlockChest::getInventory)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockChest::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockChest::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockChest::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockActivated, BlockChest::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockChest::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockChest::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockChest::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockChest::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockChest::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(unifyAdjacentChests, BlockChest::unifyAdjacentChests)
_END_REGISTER_CLASS()




// Register Class BlockClay
_BEGIN_REGISTER_CLASS(BlockClay)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockClay::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockClay::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockCocoa
_BEGIN_REGISTER_CLASS(BlockCocoa)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockCocoa::canBlockStay)
_CLASSREGISTER_AddStaticMember(getAgeFromMeta, BlockCocoa::getAgeFromMeta)
_CLASSREGISTER_AddStaticMember(getDirection, BlockCocoa::getDirection)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockCocoa::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockCocoa::onNeighborBlockChange)
_CLASSREGISTER_AddMember(updateTick, BlockCocoa::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockColored
_BEGIN_REGISTER_CLASS(BlockColored)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String )
_CLASSREGISTER_AddMember(damageDropped, BlockColored::damageDropped)
_CLASSREGISTER_AddStaticMember(getBlockFromDye, BlockColored::getBlockFromDye)
_CLASSREGISTER_AddStaticMember(getDyeFromBlock, BlockColored::getDyeFromBlock)
_END_REGISTER_CLASS()




// Register Class BlockCommandBlock
_BEGIN_REGISTER_CLASS(BlockCommandBlock)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockCommandBlock::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockCommandBlock::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockCommandBlock::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(onBlockActivated, BlockCommandBlock::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockCommandBlock::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockCommandBlock::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockCommandBlock::quantityDropped)
_CLASSREGISTER_AddMember(tickRate, BlockCommandBlock::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockCommandBlock::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockComparator
_BEGIN_REGISTER_CLASS(BlockComparator)
_CLASSREGISTER_AddBaseClass(BlockRedstoneLogic)
_CLASSREGISTER_AddBaseClass(ITileEntityProvider)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(breakBlock, BlockComparator::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockComparator::createNewTileEntity)
_CLASSREGISTER_AddMember(getTileEntityComparator, BlockComparator::getTileEntityComparator)
_CLASSREGISTER_AddMember(idDropped, BlockComparator::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockComparator::idPicked)
_CLASSREGISTER_AddMember(isComparatorActived, BlockComparator::isComparatorActived)
_CLASSREGISTER_AddMember(onBlockActivated, BlockComparator::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockComparator::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockEventReceived, BlockComparator::onBlockEventReceived)
_CLASSREGISTER_AddMember(updateTick, BlockComparator::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockContainer
_BEGIN_REGISTER_CLASS(BlockContainer)
_CLASSREGISTER_AddBaseClass(Block )
_CLASSREGISTER_AddBaseClass(ITileEntityProvider)
_END_REGISTER_CLASS()




// Register Class BlockCrops
_BEGIN_REGISTER_CLASS(BlockCrops)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockCrops::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(fertilize, BlockCrops::fertilize)
_CLASSREGISTER_AddMember(getCropItem, BlockCrops::getCropItem)
_CLASSREGISTER_AddMember(getSeedItem, BlockCrops::getSeedItem)
_CLASSREGISTER_AddMember(idDropped, BlockCrops::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockCrops::isPowered)
_CLASSREGISTER_AddMember(quantityDropped, BlockCrops::quantityDropped)
_CLASSREGISTER_AddMember(updateTick, BlockCrops::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockDaylightDetector
_BEGIN_REGISTER_CLASS(BlockDaylightDetector)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canProvidePower, BlockDaylightDetector::canProvidePower)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockDaylightDetector::createNewTileEntity)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockDaylightDetector::isProvidingWeakPower)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockDaylightDetector::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockDaylightDetector::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockDeadBush
_BEGIN_REGISTER_CLASS(BlockDeadBush)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(harvestBlock, BlockDeadBush::harvestBlock)
_CLASSREGISTER_AddMember(idDropped, BlockDeadBush::idDropped)
_END_REGISTER_CLASS()




// Register Class BlockDetectorRail
_BEGIN_REGISTER_CLASS(BlockDetectorRail)
_CLASSREGISTER_AddBaseClass(BlockRailBase)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canProvidePower, BlockDetectorRail::canProvidePower)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockDetectorRail::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockDetectorRail::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockDetectorRail::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockDetectorRail::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockAdded, BlockDetectorRail::onBlockAdded)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockDetectorRail::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(tickRate, BlockDetectorRail::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockDetectorRail::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockDirectional
_BEGIN_REGISTER_CLASS(BlockDirectional)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddStaticMember(getDirection, BlockDirectional::getDirection)
_END_REGISTER_CLASS()




// Register Class BlockDispenser
_BEGIN_REGISTER_CLASS(BlockDispenser)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockDispenser::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockDispenser::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockDispenser::getComparatorInputOverride)
_CLASSREGISTER_AddStaticMember(getFacing, BlockDispenser::getFacing)
_CLASSREGISTER_AddStaticMember(getIPositionFromBlockSource, BlockDispenser::getIPositionFromBlockSource)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockDispenser::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(onBlockActivated, BlockDispenser::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockDispenser::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockDispenser::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockDispenser::onNeighborBlockChange)
_CLASSREGISTER_AddMember(tickRate, BlockDispenser::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockDispenser::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockDoor
_BEGIN_REGISTER_CLASS(BlockDoor)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockDoor::canPlaceBlockAt)
_CLASSREGISTER_AddMember(collisionRayTrace, BlockDoor::collisionRayTrace)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockDoor::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockDoor::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(getDoorOrientation, BlockDoor::getDoorOrientation)
_CLASSREGISTER_AddMember(getMobilityFlag, BlockDoor::getMobilityFlag)
_CLASSREGISTER_AddMember(idDropped, BlockDoor::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockDoor::idPicked)
_CLASSREGISTER_AddMember(isDoorOpen, BlockDoor::isDoorOpen)
_CLASSREGISTER_AddMember(onBlockActivated, BlockDoor::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockHarvested, BlockDoor::onBlockHarvested)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockDoor::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockDoor::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockDoor::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockDragonEgg
_BEGIN_REGISTER_CLASS(BlockDragonEgg)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(isPowered, BlockDragonEgg::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockDragonEgg::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockDragonEgg::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockClicked, BlockDragonEgg::onBlockClicked)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockDragonEgg::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockDragonEgg::renderAsNormalBlock)
_CLASSREGISTER_AddMember(tickRate, BlockDragonEgg::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockDragonEgg::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockDropper
_BEGIN_REGISTER_CLASS(BlockDropper)
_CLASSREGISTER_AddBaseClass(BlockDispenser)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockDropper::createNewTileEntity)
_END_REGISTER_CLASS()




// Register Class BlockEnchantmentTable
_BEGIN_REGISTER_CLASS(BlockEnchantmentTable)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockEnchantmentTable::createNewTileEntity)
_CLASSREGISTER_AddMember(onBlockActivated, BlockEnchantmentTable::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockEnchantmentTable::onBlockPlacedBy)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockEnchantmentTable::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockEndPortal
_BEGIN_REGISTER_CLASS(BlockEndPortal)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockEndPortal::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockEndPortal::createNewTileEntity)
_CLASSREGISTER_AddMember(idPicked, BlockEndPortal::idPicked)
_CLASSREGISTER_AddMember(onBlockAdded, BlockEndPortal::onBlockAdded)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockEndPortal::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(quantityDropped, BlockEndPortal::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockEndPortal::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockEndPortal::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockEndPortalFrame
_BEGIN_REGISTER_CLASS(BlockEndPortalFrame)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String &)
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockEndPortalFrame::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockEndPortalFrame::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockEndPortalFrame::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(idDropped, BlockEndPortalFrame::idDropped)
_CLASSREGISTER_AddStaticMember(isEnderEyeInserted, BlockEndPortalFrame::isEnderEyeInserted)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockEndPortalFrame::onBlockPlacedBy)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockEndPortalFrame::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockEnderChest
_BEGIN_REGISTER_CLASS(BlockEnderChest)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , int)
_CLASSREGISTER_AddMember(canSilkHarvest, BlockEnderChest::canSilkHarvest)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockEnderChest::createNewTileEntity)
_CLASSREGISTER_AddMember(idDropped, BlockEnderChest::idDropped)
_CLASSREGISTER_AddMember(onBlockActivated, BlockEnderChest::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockEnderChest::onBlockPlacedBy)
_CLASSREGISTER_AddMember(quantityDropped, BlockEnderChest::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockEnderChest::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockFarmland
_BEGIN_REGISTER_CLASS(BlockFarmland)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockFarmland::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockFarmland::isPowered)
_CLASSREGISTER_AddMember(onFallenUpon, BlockFarmland::onFallenUpon)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFarmland::onNeighborBlockChange)
_CLASSREGISTER_AddMember(updateTick, BlockFarmland::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockFence
_BEGIN_REGISTER_CLASS(BlockFence)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockFence::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(canConnectFenceTo, BlockFence::canConnectFenceTo)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockFence::getBlocksMovement)
_CLASSREGISTER_AddStaticMember(isIdAFence, BlockFence::isIdAFence)
_CLASSREGISTER_AddMember(onBlockActivated, BlockFence::onBlockActivated)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFence::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockFence::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockFenceGate
_BEGIN_REGISTER_CLASS(BlockFenceGate)
_CLASSREGISTER_AddBaseClass(BlockDirectional)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockFenceGate::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockFenceGate::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockFenceGate::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(isFenceGateOpen, BlockFenceGate::isFenceGateOpen)
_CLASSREGISTER_AddMember(onBlockActivated, BlockFenceGate::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockFenceGate::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFenceGate::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFenceGate::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockFenceGate::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockFire
_BEGIN_REGISTER_CLASS(BlockFire)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockCatchFire, BlockFire::canBlockCatchFire)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockFire::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getChanceToEncourageFire, BlockFire::getChanceToEncourageFire)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockFire::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(initializeBlock, BlockFire::initializeBlock)
_CLASSREGISTER_AddMember(isCollidable, BlockFire::isCollidable)
_CLASSREGISTER_AddMember(isUpdateScheduled, BlockFire::isUpdateScheduled)
_CLASSREGISTER_AddMember(onBlockAdded, BlockFire::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFire::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockFire::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFire::renderAsNormalBlock)
_CLASSREGISTER_AddMember(tickRate, BlockFire::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockFire::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockFlower
_BEGIN_REGISTER_CLASS(BlockFlower)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(canBlockStay, BlockFlower::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockFlower::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockFlower::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockFlower::idDropped)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFlower::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFlower::renderAsNormalBlock)
_CLASSREGISTER_AddMember(updateTick, BlockFlower::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockFlowerPot
_BEGIN_REGISTER_CLASS(BlockFlowerPot)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockFlowerPot::canPlaceBlockAt)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockFlowerPot::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(getDamageValue, BlockFlowerPot::getDamageValue)
_CLASSREGISTER_AddStaticMember(getMetaForPlant, BlockFlowerPot::getMetaForPlant)
_CLASSREGISTER_AddStaticMember(getPlantForMeta, BlockFlowerPot::getPlantForMeta)
_CLASSREGISTER_AddMember(idDropped, BlockFlowerPot::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockFlowerPot::idPicked)
_CLASSREGISTER_AddMember(isFlowerPot, BlockFlowerPot::isFlowerPot)
_CLASSREGISTER_AddMember(onBlockActivated, BlockFlowerPot::onBlockActivated)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFlowerPot::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFlowerPot::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockFlowerPot::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockFlowing
_BEGIN_REGISTER_CLASS(BlockFlowing)
_CLASSREGISTER_AddBaseClass(BlockFluid)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getBlocksMovement, BlockFlowing::getBlocksMovement)
_CLASSREGISTER_AddMember(isUpdateScheduled, BlockFlowing::isUpdateScheduled)
_CLASSREGISTER_AddMember(onBlockAdded, BlockFlowing::onBlockAdded)
_CLASSREGISTER_AddMember(updateTick, BlockFlowing::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockFluid
_BEGIN_REGISTER_CLASS(BlockFluid)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canCollideCheck, BlockFluid::canCollideCheck)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockFluid::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockFluid::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(getFlowDirection, BlockFluid::getFlowDirection)
_CLASSREGISTER_AddStaticMember(getFluidHeightPercent, BlockFluid::getFluidHeightPercent)
_CLASSREGISTER_AddMember(idDropped, BlockFluid::idDropped)
_CLASSREGISTER_AddMember(isBlockSolid, BlockFluid::isBlockSolid)
_CLASSREGISTER_AddMember(isUpdateScheduled, BlockFluid::isUpdateScheduled)
_CLASSREGISTER_AddMember(onBlockAdded, BlockFluid::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockFluid::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockFluid::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockFluid::renderAsNormalBlock)
_CLASSREGISTER_AddMember(tickRate, BlockFluid::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockFluid::updateTick)
_CLASSREGISTER_AddMember(velocityToAddToEntity, BlockFluid::velocityToAddToEntity)
_END_REGISTER_CLASS()




// Register Class BlockFurnace
_BEGIN_REGISTER_CLASS(BlockFurnace)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(breakBlock, BlockFurnace::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockFurnace::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockFurnace::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockFurnace::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(idDropped, BlockFurnace::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockFurnace::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockFurnace::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockFurnace::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockFurnace::onBlockPlacedBy)
_CLASSREGISTER_AddStaticMember(updateFurnaceBlockState, BlockFurnace::updateFurnaceBlockState)
_END_REGISTER_CLASS()




// Register Class BlockGlass
_BEGIN_REGISTER_CLASS(BlockGlass)
_CLASSREGISTER_AddBaseClass(BlockBreakable)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canSilkHarvest, BlockGlass::canSilkHarvest)
_CLASSREGISTER_AddMember(quantityDropped, BlockGlass::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockGlass::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockGlowStone
_BEGIN_REGISTER_CLASS(BlockGlowStone)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockGlowStone::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockGlowStone::quantityDropped)
_CLASSREGISTER_AddMember(quantityDroppedWithBonus, BlockGlowStone::quantityDroppedWithBonus)
_END_REGISTER_CLASS()




// Register Class BlockGrass
_BEGIN_REGISTER_CLASS(BlockGrass)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockGrass::idDropped)
_CLASSREGISTER_AddMember(updateTick, BlockGrass::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockHalfSlab
_BEGIN_REGISTER_CLASS(BlockHalfSlab)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockHalfSlab::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(damageDropped, BlockHalfSlab::damageDropped)
_CLASSREGISTER_AddMember(getDamageValue, BlockHalfSlab::getDamageValue)
_CLASSREGISTER_AddMember(idPicked, BlockHalfSlab::idPicked)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockHalfSlab::onBlockPlaced)
_CLASSREGISTER_AddMember(quantityDropped, BlockHalfSlab::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockHalfSlab::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockHalfSlab::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockHalfSlab::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockHay
_BEGIN_REGISTER_CLASS(BlockHay)
_CLASSREGISTER_AddBaseClass(BlockRotatedPillar)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_END_REGISTER_CLASS()




// Register Class BlockHopper
_BEGIN_REGISTER_CLASS(BlockHopper)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockHopper::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(breakBlock, BlockHopper::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockHopper::createNewTileEntity)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockHopper::getComparatorInputOverride)
_CLASSREGISTER_AddStaticMember(getDirectionFromMetadata, BlockHopper::getDirectionFromMetadata)
_CLASSREGISTER_AddStaticMember(getIsBlockNotPoweredFromMetadata, BlockHopper::getIsBlockNotPoweredFromMetadata)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockHopper::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(onBlockActivated, BlockHopper::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockHopper::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockHopper::onBlockPlaced)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockHopper::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockHopper::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockHopper::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockHopper::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockIce
_BEGIN_REGISTER_CLASS(BlockIce)
_CLASSREGISTER_AddBaseClass(BlockBreakable)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getMobilityFlag, BlockIce::getMobilityFlag)
_CLASSREGISTER_AddMember(harvestBlock, BlockIce::harvestBlock)
_CLASSREGISTER_AddMember(quantityDropped, BlockIce::quantityDropped)
_CLASSREGISTER_AddMember(updateTick, BlockIce::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockJukeBox
_BEGIN_REGISTER_CLASS(BlockJukeBox)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockJukeBox::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockJukeBox::createNewTileEntity)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockJukeBox::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(ejectRecord, BlockJukeBox::ejectRecord)
_CLASSREGISTER_AddMember(getComparatorInputOverride, BlockJukeBox::getComparatorInputOverride)
_CLASSREGISTER_AddMember(hasComparatorInputOverride, BlockJukeBox::hasComparatorInputOverride)
_CLASSREGISTER_AddMember(insertRecord, BlockJukeBox::insertRecord)
_CLASSREGISTER_AddMember(onBlockActivated, BlockJukeBox::onBlockActivated)
_END_REGISTER_CLASS()




// Register Class BlockLadder
_BEGIN_REGISTER_CLASS(BlockLadder)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockLadder::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockLadder::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockLadder::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockLadder::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockLadder::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockLadder::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockLadder::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(updateLadderBounds, BlockLadder::updateLadderBounds)
_END_REGISTER_CLASS()




// Register Class BlockLeaves
_BEGIN_REGISTER_CLASS(BlockLeaves)
_CLASSREGISTER_AddBaseClass(BlockLeavesBase)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockLeaves::breakBlock)
_CLASSREGISTER_AddMember(damageDropped, BlockLeaves::damageDropped)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockLeaves::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(harvestBlock, BlockLeaves::harvestBlock)
_CLASSREGISTER_AddMember(idDropped, BlockLeaves::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockLeaves::quantityDropped)
_CLASSREGISTER_AddMember(updateTick, BlockLeaves::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockLeavesBase
_BEGIN_REGISTER_CLASS(BlockLeavesBase)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_END_REGISTER_CLASS()




// Register Class BlockLever
_BEGIN_REGISTER_CLASS(BlockLever)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockLever::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockLever::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockLever::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(canProvidePower, BlockLever::canProvidePower)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockLever::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(invertMetadata, BlockLever::invertMetadata)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockLever::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockLever::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockActivated, BlockLever::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockLever::onBlockPlaced)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockLever::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockLever::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockLever::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockLever::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockLilyPad
_BEGIN_REGISTER_CLASS(BlockLilyPad)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockLilyPad::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(canBlockStay, BlockLilyPad::canBlockStay)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockLilyPad::getCollisionBoundingBox)
_END_REGISTER_CLASS()




// Register Class BlockLogBase
_BEGIN_REGISTER_CLASS(BlockLogBase)
_CLASSREGISTER_AddBaseClass(BlockRotatedPillar)
_CLASSREGISTER_AddMember(breakBlock, BlockLogBase::breakBlock)
_CLASSREGISTER_AddMember(idDropped, BlockLogBase::idDropped)
_CLASSREGISTER_AddStaticMember(limitToValidMetadata, BlockLogBase::limitToValidMetadata)
_CLASSREGISTER_AddMember(quantityDropped, BlockLogBase::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockLog1
_BEGIN_REGISTER_CLASS(BlockLog1)
_CLASSREGISTER_AddBaseClass(BlockLogBase)
_CLASSREGISTER_AddCtor(int, const BM_Material &, const String &)
_CLASSREGISTER_AddMember(idDropped, BlockLog1::idDropped)
_END_REGISTER_CLASS()




// Register Class BlockLog2
_BEGIN_REGISTER_CLASS(BlockLog2)
_CLASSREGISTER_AddBaseClass(BlockLogBase)
_CLASSREGISTER_AddCtor(int, const BM_Material &, const String &)
_CLASSREGISTER_AddMember(idDropped, BlockLog2::idDropped)
_END_REGISTER_CLASS()




// Register Class BlockMelon
_BEGIN_REGISTER_CLASS(BlockMelon)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockMelon::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockMelon::quantityDropped)
_CLASSREGISTER_AddMember(quantityDroppedWithBonus, BlockMelon::quantityDroppedWithBonus)
_END_REGISTER_CLASS()




// Register Class BlockMobSpawner
_BEGIN_REGISTER_CLASS(BlockMobSpawner)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockMobSpawner::createNewTileEntity)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockMobSpawner::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(idDropped, BlockMobSpawner::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockMobSpawner::isPowered)
_CLASSREGISTER_AddMember(quantityDropped, BlockMobSpawner::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockMushroom
_BEGIN_REGISTER_CLASS(BlockMushroom)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockMushroom::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockMushroom::canPlaceBlockAt)
_CLASSREGISTER_AddMember(fertilizeMushroom, BlockMushroom::fertilizeMushroom)
_CLASSREGISTER_AddMember(updateTick, BlockMushroom::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockMushroomCap
_BEGIN_REGISTER_CLASS(BlockMushroomCap)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , int )
_CLASSREGISTER_AddMember(idDropped, BlockMushroomCap::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockMushroomCap::isPowered)
_CLASSREGISTER_AddMember(quantityDropped, BlockMushroomCap::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockMycelium
_BEGIN_REGISTER_CLASS(BlockMycelium)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockMycelium::idDropped)
_CLASSREGISTER_AddMember(updateTick, BlockMycelium::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockNetherStalk
_BEGIN_REGISTER_CLASS(BlockNetherStalk)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockNetherStalk::canBlockStay)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockNetherStalk::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(idDropped, BlockNetherStalk::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockNetherStalk::isPowered)
_CLASSREGISTER_AddMember(quantityDropped, BlockNetherStalk::quantityDropped)
_CLASSREGISTER_AddMember(updateTick, BlockNetherStalk::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockNote
_BEGIN_REGISTER_CLASS(BlockNote)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockNote::createNewTileEntity)
_CLASSREGISTER_AddMember(onBlockActivated, BlockNote::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockClicked, BlockNote::onBlockClicked)
_CLASSREGISTER_AddMember(onBlockEventReceived, BlockNote::onBlockEventReceived)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockNote::onNeighborBlockChange)
_END_REGISTER_CLASS()




// Register Class BlockOre
_BEGIN_REGISTER_CLASS(BlockOre)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockOre::damageDropped)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockOre::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(idDropped, BlockOre::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockOre::quantityDropped)
_CLASSREGISTER_AddMember(quantityDroppedWithBonus, BlockOre::quantityDroppedWithBonus)
_END_REGISTER_CLASS()




// Register Class BlockOreStorage
_BEGIN_REGISTER_CLASS(BlockOreStorage)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_END_REGISTER_CLASS()




// Register Class BlockPane
_BEGIN_REGISTER_CLASS(BlockPane)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockPane::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(canSilkHarvest, BlockPane::canSilkHarvest)
_CLASSREGISTER_AddMember(canThisPaneConnectToThisBlockID, BlockPane::canThisPaneConnectToThisBlockID)
_CLASSREGISTER_AddMember(createStackedBlock, BlockPane::createStackedBlock)
_CLASSREGISTER_AddMember(idDropped, BlockPane::idDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockPane::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockPane::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockPane::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockPistonBase
_BEGIN_REGISTER_CLASS(BlockPistonBase)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockPistonBase::addCollisionBoxesToList)
_CLASSREGISTER_AddStaticMember(canExtend, BlockPistonBase::canExtend)
_CLASSREGISTER_AddStaticMember(canPushBlock, BlockPistonBase::canPushBlock)
_CLASSREGISTER_AddStaticMember(determineOrientation, BlockPistonBase::determineOrientation)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockPistonBase::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(getOrientation, BlockPistonBase::getOrientation)
_CLASSREGISTER_AddStaticMember(isExtended, BlockPistonBase::isExtended)
_CLASSREGISTER_AddMember(onBlockActivated, BlockPistonBase::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockPistonBase::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockEventReceived, BlockPistonBase::onBlockEventReceived)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockPistonBase::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockPistonBase::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockPistonBase::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockPistonBase::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockPistonBase::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockPistonExtension
_BEGIN_REGISTER_CLASS(BlockPistonExtension)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockPistonExtension::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(breakBlock, BlockPistonExtension::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockPistonExtension::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockPistonExtension::canPlaceBlockOnSide)
_CLASSREGISTER_AddStaticMember(getDirectionMeta, BlockPistonExtension::getDirectionMeta)
_CLASSREGISTER_AddMember(idPicked, BlockPistonExtension::idPicked)
_CLASSREGISTER_AddMember(onBlockHarvested, BlockPistonExtension::onBlockHarvested)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockPistonExtension::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockPistonExtension::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockPistonExtension::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockPistonExtension::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockPistonMoving
_BEGIN_REGISTER_CLASS(BlockPistonMoving)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockPistonMoving::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockPistonMoving::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockPistonMoving::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockPistonMoving::createNewTileEntity)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockPistonMoving::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(getAxisAlignedBB, BlockPistonMoving::getAxisAlignedBB)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockPistonMoving::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(getTileEntity, BlockPistonMoving::getTileEntity)
_CLASSREGISTER_AddMember(idDropped, BlockPistonMoving::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockPistonMoving::isPowered)
_CLASSREGISTER_AddMember(onBlockActivated, BlockPistonMoving::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockPistonMoving::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockPistonMoving::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockPistonMoving::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockPistonMoving::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockPortal
_BEGIN_REGISTER_CLASS(BlockPortal)
_CLASSREGISTER_AddBaseClass(BlockBreakable)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockPortal::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(isPowered, BlockPortal::isPowered)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockPortal::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockPortal::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockPortal::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockPortal::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockPortal::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(tryToCreatePortal, BlockPortal::tryToCreatePortal)
_CLASSREGISTER_AddMember(updateTick, BlockPortal::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockPotato
_BEGIN_REGISTER_CLASS(BlockPotato)
_CLASSREGISTER_AddBaseClass(BlockCrops)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockPotato::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(getCropItem, BlockPotato::getCropItem)
_CLASSREGISTER_AddMember(getSeedItem, BlockPotato::getSeedItem)
_END_REGISTER_CLASS()




// Register Class BlockPoweredOre
_BEGIN_REGISTER_CLASS(BlockPoweredOre)
_CLASSREGISTER_AddBaseClass(BlockOreStorage)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canProvidePower, BlockPoweredOre::canProvidePower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockPoweredOre::isProvidingWeakPower)
_END_REGISTER_CLASS()




// Register Class BlockPressurePlate
_BEGIN_REGISTER_CLASS(BlockPressurePlate)
_CLASSREGISTER_AddBaseClass(BlockBasePressurePlate)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , MOB_TYPE )
_CLASSREGISTER_AddMember(getMetaFromWeight, BlockPressurePlate::getMetaFromWeight)
_CLASSREGISTER_AddMember(getPlateState, BlockPressurePlate::getPlateState)
_CLASSREGISTER_AddMember(getPowerSupply, BlockPressurePlate::getPowerSupply)
_END_REGISTER_CLASS()




// Register Class BlockPressurePlateWeighted
_BEGIN_REGISTER_CLASS(BlockPressurePlateWeighted)
_CLASSREGISTER_AddBaseClass(BlockBasePressurePlate)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , int , MOB_TYPE)
_CLASSREGISTER_AddMember(getMetaFromWeight, BlockPressurePlateWeighted::getMetaFromWeight)
_CLASSREGISTER_AddMember(getPlateState, BlockPressurePlateWeighted::getPlateState)
_CLASSREGISTER_AddMember(getPowerSupply, BlockPressurePlateWeighted::getPowerSupply)
_CLASSREGISTER_AddMember(tickRate, BlockPressurePlateWeighted::tickRate)
_END_REGISTER_CLASS()




// Register Class BlockPumpkin
_BEGIN_REGISTER_CLASS(BlockPumpkin)
_CLASSREGISTER_AddBaseClass(BlockDirectional)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockPumpkin::canPlaceBlockAt)
_CLASSREGISTER_AddMember(onBlockAdded, BlockPumpkin::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockPumpkin::onBlockPlacedBy)
_END_REGISTER_CLASS()




// Register Class BlockQuartz
_BEGIN_REGISTER_CLASS(BlockQuartz)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createStackedBlock, BlockQuartz::createStackedBlock)
_CLASSREGISTER_AddMember(damageDropped, BlockQuartz::damageDropped)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockQuartz::onBlockPlaced)
_END_REGISTER_CLASS()




// Register Class BlockRail
_BEGIN_REGISTER_CLASS(BlockRail)
_CLASSREGISTER_AddBaseClass(BlockRailBase)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(updateRailState, BlockRail::updateRailState)
_END_REGISTER_CLASS()




// Register Class BlockRailBase
_BEGIN_REGISTER_CLASS(BlockRailBase)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddMember(breakBlock, BlockRailBase::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockRailBase::canPlaceBlockAt)
_CLASSREGISTER_AddMember(collisionRayTrace, BlockRailBase::collisionRayTrace)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockRailBase::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(getMobilityFlag, BlockRailBase::getMobilityFlag)
_CLASSREGISTER_AddMember(isPowered, BlockRailBase::isPowered)
_CLASSREGISTER_AddStaticMember(isRailBlock, BlockRailBase::isRailBlock)
_CLASSREGISTER_AddStaticMember(isRailBlockAt, BlockRailBase::isRailBlockAt)
_CLASSREGISTER_AddMember(onBlockAdded, BlockRailBase::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockRailBase::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockRailBase::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockRailBase::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockRailBase::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(updateRailState, BlockRailBase::updateRailState)
_END_REGISTER_CLASS()




// Register Class BlockRailPowered
_BEGIN_REGISTER_CLASS(BlockRailPowered)
_CLASSREGISTER_AddBaseClass(BlockRailBase)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(findPoweredRailSignal, BlockRailPowered::findPoweredRailSignal)
_CLASSREGISTER_AddMember(isSameRailWithPower, BlockRailPowered::isSameRailWithPower)
_CLASSREGISTER_AddMember(updateRailState, BlockRailPowered::updateRailState)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneLight
_BEGIN_REGISTER_CLASS(BlockRedstoneLight)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(idDropped, BlockRedstoneLight::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockRedstoneLight::isPowered)
_CLASSREGISTER_AddMember(onBlockAdded, BlockRedstoneLight::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockRedstoneLight::onNeighborBlockChange)
_CLASSREGISTER_AddMember(updateTick, BlockRedstoneLight::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneLogic
_BEGIN_REGISTER_CLASS(BlockRedstoneLogic)
_CLASSREGISTER_AddBaseClass(BlockDirectional)
_CLASSREGISTER_AddMember(canBlockStay, BlockRedstoneLogic::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockRedstoneLogic::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canProvidePower, BlockRedstoneLogic::canProvidePower)
_CLASSREGISTER_AddMember(isAssociatedBlockID, BlockRedstoneLogic::isAssociatedBlockID)
_CLASSREGISTER_AddMember(isFacingTowardsRepeater, BlockRedstoneLogic::isFacingTowardsRepeater)
_CLASSREGISTER_AddMember(isLocked, BlockRedstoneLogic::isLocked)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockRedstoneLogic::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockRedstoneLogic::isProvidingWeakPower)
_CLASSREGISTER_AddStaticMember(isRedstoneRepeaterBlockID, BlockRedstoneLogic::isRedstoneRepeaterBlockID)
_CLASSREGISTER_AddMember(isSameDiode, BlockRedstoneLogic::isSameDiode)
_CLASSREGISTER_AddMember(onBlockAdded, BlockRedstoneLogic::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockDestroyedByPlayer, BlockRedstoneLogic::onBlockDestroyedByPlayer)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockRedstoneLogic::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockRedstoneLogic::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockRedstoneLogic::renderAsNormalBlock)
_CLASSREGISTER_AddMember(updateTick, BlockRedstoneLogic::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneOre
_BEGIN_REGISTER_CLASS(BlockRedstoneOre)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(createStackedBlock, BlockRedstoneOre::createStackedBlock)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockRedstoneOre::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(idDropped, BlockRedstoneOre::idDropped)
_CLASSREGISTER_AddMember(onBlockActivated, BlockRedstoneOre::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockClicked, BlockRedstoneOre::onBlockClicked)
_CLASSREGISTER_AddMember(onEntityWalking, BlockRedstoneOre::onEntityWalking)
_CLASSREGISTER_AddMember(quantityDropped, BlockRedstoneOre::quantityDropped)
_CLASSREGISTER_AddMember(tickRate, BlockRedstoneOre::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockRedstoneOre::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneRepeater
_BEGIN_REGISTER_CLASS(BlockRedstoneRepeater)
_CLASSREGISTER_AddBaseClass(BlockRedstoneLogic)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(breakBlock, BlockRedstoneRepeater::breakBlock)
_CLASSREGISTER_AddMember(idDropped, BlockRedstoneRepeater::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockRedstoneRepeater::idPicked)
_CLASSREGISTER_AddMember(isLocked, BlockRedstoneRepeater::isLocked)
_CLASSREGISTER_AddMember(onBlockActivated, BlockRedstoneRepeater::onBlockActivated)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneTorch
_BEGIN_REGISTER_CLASS(BlockRedstoneTorch)
_CLASSREGISTER_AddBaseClass(BlockTorch)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(breakBlock, BlockRedstoneTorch::breakBlock)
_CLASSREGISTER_AddMember(canProvidePower, BlockRedstoneTorch::canProvidePower)
_CLASSREGISTER_AddMember(idDropped, BlockRedstoneTorch::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockRedstoneTorch::idPicked)
_CLASSREGISTER_AddMember(isAssociatedBlockID, BlockRedstoneTorch::isAssociatedBlockID)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockRedstoneTorch::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockRedstoneTorch::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockAdded, BlockRedstoneTorch::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockRedstoneTorch::onNeighborBlockChange)
_CLASSREGISTER_AddMember(tickRate, BlockRedstoneTorch::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockRedstoneTorch::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockRedstoneWire
_BEGIN_REGISTER_CLASS(BlockRedstoneWire)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockRedstoneWire::breakBlock)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockRedstoneWire::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canProvidePower, BlockRedstoneWire::canProvidePower)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockRedstoneWire::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockRedstoneWire::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockRedstoneWire::idPicked)
_CLASSREGISTER_AddStaticMember(isPowerProviderOrWire, BlockRedstoneWire::isPowerProviderOrWire)
_CLASSREGISTER_AddStaticMember(isPoweredOrRepeater, BlockRedstoneWire::isPoweredOrRepeater)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockRedstoneWire::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockRedstoneWire::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockAdded, BlockRedstoneWire::onBlockAdded)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockRedstoneWire::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockRedstoneWire::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockReed
_BEGIN_REGISTER_CLASS(BlockReed)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canBlockStay, BlockReed::canBlockStay)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockReed::canPlaceBlockAt)
_CLASSREGISTER_AddMember(idDropped, BlockReed::idDropped)
_CLASSREGISTER_AddMember(isPowered, BlockReed::isPowered)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockReed::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockReed::renderAsNormalBlock)
_CLASSREGISTER_AddMember(updateTick, BlockReed::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockRotatedPillar
_BEGIN_REGISTER_CLASS(BlockRotatedPillar)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createStackedBlock, BlockRotatedPillar::createStackedBlock)
_CLASSREGISTER_AddMember(damageDropped, BlockRotatedPillar::damageDropped)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockRotatedPillar::onBlockPlaced)
_END_REGISTER_CLASS()




// Register Class BlockSand
_BEGIN_REGISTER_CLASS(BlockSand)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddStaticMember(canFallBelow, BlockSand::canFallBelow)
_CLASSREGISTER_AddMember(onBlockAdded, BlockSand::onBlockAdded)
_CLASSREGISTER_AddMember(onFinishFalling, BlockSand::onFinishFalling)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockSand::onNeighborBlockChange)
_CLASSREGISTER_AddMember(tickRate, BlockSand::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockSand::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockSandStone
_BEGIN_REGISTER_CLASS(BlockSandStone)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockSandStone::damageDropped)
_END_REGISTER_CLASS()




// Register Class BlockSapling
_BEGIN_REGISTER_CLASS(BlockSapling)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockSapling::damageDropped)
_CLASSREGISTER_AddMember(growTree, BlockSapling::growTree)
_CLASSREGISTER_AddMember(isSameSapling, BlockSapling::isSameSapling)
_CLASSREGISTER_AddMember(markOrGrowMarked, BlockSapling::markOrGrowMarked)
_CLASSREGISTER_AddMember(updateTick, BlockSapling::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockSign
_BEGIN_REGISTER_CLASS(BlockSign)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , int , bool )
_CLASSREGISTER_AddMember(createNewTileEntity, BlockSign::createNewTileEntity)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockSign::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockSign::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockSign::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockSign::idPicked)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockSign::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockSign::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockSign::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockSilverfish
_BEGIN_REGISTER_CLASS(BlockSilverfish)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(createStackedBlock, BlockSilverfish::createStackedBlock)
_CLASSREGISTER_AddMember(getDamageValue, BlockSilverfish::getDamageValue)
_CLASSREGISTER_AddMember(onBlockDestroyedByPlayer, BlockSilverfish::onBlockDestroyedByPlayer)
_CLASSREGISTER_AddMember(quantityDropped, BlockSilverfish::quantityDropped)
_END_REGISTER_CLASS()




// Register Class BlockSkull
_BEGIN_REGISTER_CLASS(BlockSkull)
_CLASSREGISTER_AddBaseClass(BlockContainer)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockSkull::breakBlock)
_CLASSREGISTER_AddMember(createNewTileEntity, BlockSkull::createNewTileEntity)
_CLASSREGISTER_AddMember(damageDropped, BlockSkull::damageDropped)
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockSkull::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockSkull::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(getDamageValue, BlockSkull::getDamageValue)
_CLASSREGISTER_AddMember(idDropped, BlockSkull::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockSkull::idPicked)
_CLASSREGISTER_AddMember(makeWither, BlockSkull::makeWither)
_CLASSREGISTER_AddMember(onBlockHarvested, BlockSkull::onBlockHarvested)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockSkull::onBlockPlacedBy)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockSkull::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockSkull::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockSnow
_BEGIN_REGISTER_CLASS(BlockSnow)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockSnow::canPlaceBlockAt)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockSnow::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(harvestBlock, BlockSnow::harvestBlock)
_CLASSREGISTER_AddMember(idDropped, BlockSnow::idDropped)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockSnow::onNeighborBlockChange)
_CLASSREGISTER_AddMember(quantityDropped, BlockSnow::quantityDropped)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockSnow::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockSnow::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockSnow::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(updateTick, BlockSnow::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockSnowBlock
_BEGIN_REGISTER_CLASS(BlockSnowBlock)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(idDropped, BlockSnowBlock::idDropped)
_CLASSREGISTER_AddMember(quantityDropped, BlockSnowBlock::quantityDropped)
_CLASSREGISTER_AddMember(updateTick, BlockSnowBlock::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockSoulSand
_BEGIN_REGISTER_CLASS(BlockSoulSand)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockSoulSand::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockSoulSand::onEntityCollidedWithBlock)
_END_REGISTER_CLASS()




// Register Class BlockStairs
_BEGIN_REGISTER_CLASS(BlockStairs)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , Block * , int )
_CLASSREGISTER_AddMember(addCollisionBoxesToList, BlockStairs::addCollisionBoxesToList)
_CLASSREGISTER_AddMember(breakBlock, BlockStairs::breakBlock)
_CLASSREGISTER_AddMember(canCollideCheck, BlockStairs::canCollideCheck)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockStairs::canPlaceBlockAt)
_CLASSREGISTER_AddMember(collisionRayTrace, BlockStairs::collisionRayTrace)
_CLASSREGISTER_AddMember(getCollEighthBouding, BlockStairs::getCollEighthBouding)
_CLASSREGISTER_AddMember(getCollQuarterBouding, BlockStairs::getCollQuarterBouding)
_CLASSREGISTER_AddMember(getCollSlabBouding, BlockStairs::getCollSlabBouding)
_CLASSREGISTER_AddMember(getExplosionResistance, BlockStairs::getExplosionResistance)
_CLASSREGISTER_AddStaticMember(isBlockStairsID, BlockStairs::isBlockStairsID)
_CLASSREGISTER_AddMember(isCollidable, BlockStairs::isCollidable)
_CLASSREGISTER_AddMember(onBlockActivated, BlockStairs::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockStairs::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockClicked, BlockStairs::onBlockClicked)
_CLASSREGISTER_AddMember(onBlockDestroyedByExplosion, BlockStairs::onBlockDestroyedByExplosion)
_CLASSREGISTER_AddMember(onBlockDestroyedByPlayer, BlockStairs::onBlockDestroyedByPlayer)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockStairs::onBlockPlaced)
_CLASSREGISTER_AddMember(onBlockPlacedBy, BlockStairs::onBlockPlacedBy)
_CLASSREGISTER_AddMember(onEntityWalking, BlockStairs::onEntityWalking)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockStairs::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockStairs::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(tickRate, BlockStairs::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockStairs::updateTick)
_CLASSREGISTER_AddMember(velocityToAddToEntity, BlockStairs::velocityToAddToEntity)
_END_REGISTER_CLASS()




// Register Class BlockStationary
_BEGIN_REGISTER_CLASS(BlockStationary)
_CLASSREGISTER_AddBaseClass(BlockFluid)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getBlocksMovement, BlockStationary::getBlocksMovement)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockStationary::onNeighborBlockChange)
_CLASSREGISTER_AddMember(updateTick, BlockStationary::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockStem
_BEGIN_REGISTER_CLASS(BlockStem)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , Block * )
_CLASSREGISTER_AddMember(dropBlockAsItemWithChance, BlockStem::dropBlockAsItemWithChance)
_CLASSREGISTER_AddMember(fertilizeStem, BlockStem::fertilizeStem)
_CLASSREGISTER_AddMember(getState, BlockStem::getState)
_CLASSREGISTER_AddMember(idDropped, BlockStem::idDropped)
_CLASSREGISTER_AddMember(idPicked, BlockStem::idPicked)
_CLASSREGISTER_AddMember(quantityDropped, BlockStem::quantityDropped)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockStem::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockStem::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(updateTick, BlockStem::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockStep
_BEGIN_REGISTER_CLASS(BlockStep)
_CLASSREGISTER_AddBaseClass(BlockHalfSlab)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(createStackedBlock, BlockStep::createStackedBlock)
_CLASSREGISTER_AddMember(idDropped, BlockStep::idDropped)
_END_REGISTER_CLASS()




// Register Class BlockStoneBrick
_BEGIN_REGISTER_CLASS(BlockStoneBrick)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockStoneBrick::damageDropped)
_END_REGISTER_CLASS()




// Register Class BlockTNT
_BEGIN_REGISTER_CLASS(BlockTNT)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canDropFromExplosion, BlockTNT::canDropFromExplosion)
_CLASSREGISTER_AddMember(explode, BlockTNT::explode)
_CLASSREGISTER_AddMember(onBlockActivated, BlockTNT::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockAdded, BlockTNT::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockDestroyedByExplosion, BlockTNT::onBlockDestroyedByExplosion)
_CLASSREGISTER_AddMember(onBlockDestroyedByPlayer, BlockTNT::onBlockDestroyedByPlayer)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockTNT::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockTNT::onNeighborBlockChange)
_END_REGISTER_CLASS()




// Register Class BlockTallGrass
_BEGIN_REGISTER_CLASS(BlockTallGrass)
_CLASSREGISTER_AddBaseClass(BlockFlower)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(getDamageValue, BlockTallGrass::getDamageValue)
_CLASSREGISTER_AddMember(harvestBlock, BlockTallGrass::harvestBlock)
_CLASSREGISTER_AddMember(idDropped, BlockTallGrass::idDropped)
_CLASSREGISTER_AddMember(quantityDroppedWithBonus, BlockTallGrass::quantityDroppedWithBonus)
_END_REGISTER_CLASS()




// Register Class BlockTorch
_BEGIN_REGISTER_CLASS(BlockTorch)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockTorch::canPlaceBlockAt)
_CLASSREGISTER_AddMember(collisionRayTrace, BlockTorch::collisionRayTrace)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockTorch::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(onBlockAdded, BlockTorch::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockTorch::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockTorch::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockTorch::renderAsNormalBlock)
_CLASSREGISTER_AddMember(updateTick, BlockTorch::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockTrapDoor
_BEGIN_REGISTER_CLASS(BlockTrapDoor)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockTrapDoor::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(collisionRayTrace, BlockTrapDoor::collisionRayTrace)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockTrapDoor::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockTrapDoor::getCollisionBoundingBox)
_CLASSREGISTER_AddStaticMember(isTrapdoorOpen, BlockTrapDoor::isTrapdoorOpen)
_CLASSREGISTER_AddMember(onBlockActivated, BlockTrapDoor::onBlockActivated)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockTrapDoor::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockTrapDoor::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockTrapDoor::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockTrapDoor::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockTrapDoor::setBlockBoundsForItemRender)
_END_REGISTER_CLASS()




// Register Class BlockTripWire
_BEGIN_REGISTER_CLASS(BlockTripWire)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockTripWire::breakBlock)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockTripWire::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockTripWire::idDropped)
_CLASSREGISTER_AddStaticMember(isConnectedTo, BlockTripWire::isConnectedTo)
_CLASSREGISTER_AddMember(isPowered, BlockTripWire::isPowered)
_CLASSREGISTER_AddMember(notifyHook, BlockTripWire::notifyHook)
_CLASSREGISTER_AddMember(onBlockAdded, BlockTripWire::onBlockAdded)
_CLASSREGISTER_AddMember(onBlockHarvested, BlockTripWire::onBlockHarvested)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockTripWire::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockTripWire::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockTripWire::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockTripWire::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(tickRate, BlockTripWire::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockTripWire::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockTripWireSource
_BEGIN_REGISTER_CLASS(BlockTripWireSource)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(breakBlock, BlockTripWireSource::breakBlock)
//_CLASSREGISTER_AddMember(calculateState, BlockTripWireSource::calculateState)
_CLASSREGISTER_AddMember(canPlaceBlockAt, BlockTripWireSource::canPlaceBlockAt)
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockTripWireSource::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(canProvidePower, BlockTripWireSource::canProvidePower)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockTripWireSource::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(isProvidingStrongPower, BlockTripWireSource::isProvidingStrongPower)
_CLASSREGISTER_AddMember(isProvidingWeakPower, BlockTripWireSource::isProvidingWeakPower)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockTripWireSource::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockTripWireSource::onNeighborBlockChange)
_CLASSREGISTER_AddMember(onPostBlockPlaced, BlockTripWireSource::onPostBlockPlaced)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockTripWireSource::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockTripWireSource::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(tickRate, BlockTripWireSource::tickRate)
_CLASSREGISTER_AddMember(updateTick, BlockTripWireSource::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockVine
_BEGIN_REGISTER_CLASS(BlockVine)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canPlaceBlockOnSide, BlockVine::canPlaceBlockOnSide)
_CLASSREGISTER_AddMember(onBlockPlaced, BlockVine::onBlockPlaced)
_CLASSREGISTER_AddMember(onNeighborBlockChange, BlockVine::onNeighborBlockChange)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockVine::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockVine::setBlockBoundsBasedOnState)
_CLASSREGISTER_AddMember(setBlockBoundsForItemRender, BlockVine::setBlockBoundsForItemRender)
_CLASSREGISTER_AddMember(updateTick, BlockVine::updateTick)
_END_REGISTER_CLASS()




// Register Class BlockWall
_BEGIN_REGISTER_CLASS(BlockWall)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canConnectWallTo, BlockWall::canConnectWallTo)
_CLASSREGISTER_AddMember(damageDropped, BlockWall::damageDropped)
_CLASSREGISTER_AddMember(getBlocksMovement, BlockWall::getBlocksMovement)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockWall::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockWall::renderAsNormalBlock)
_CLASSREGISTER_AddMember(setBlockBoundsBasedOnState, BlockWall::setBlockBoundsBasedOnState)
_END_REGISTER_CLASS()




// Register Class BlockWeb
_BEGIN_REGISTER_CLASS(BlockWeb)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(canSilkHarvest, BlockWeb::canSilkHarvest)
_CLASSREGISTER_AddMember(getCollisionBoundingBox, BlockWeb::getCollisionBoundingBox)
_CLASSREGISTER_AddMember(idDropped, BlockWeb::idDropped)
_CLASSREGISTER_AddMember(onEntityCollidedWithBlock, BlockWeb::onEntityCollidedWithBlock)
_CLASSREGISTER_AddMember(renderAsNormalBlock, BlockWeb::renderAsNormalBlock)
_END_REGISTER_CLASS()




// Register Class BlockWood
_BEGIN_REGISTER_CLASS(BlockWood)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_CLASSREGISTER_AddMember(damageDropped, BlockWood::damageDropped)
_END_REGISTER_CLASS()




// Register Class BlockWoodSlab
_BEGIN_REGISTER_CLASS(BlockWoodSlab)
_CLASSREGISTER_AddBaseClass(BlockHalfSlab)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & , bool )
_CLASSREGISTER_AddMember(createStackedBlock, BlockWoodSlab::createStackedBlock)
_CLASSREGISTER_AddMember(idDropped, BlockWoodSlab::idDropped)
_END_REGISTER_CLASS()




// Register Class BlockWorkbench
_BEGIN_REGISTER_CLASS(BlockWorkbench)
_CLASSREGISTER_AddBaseClass(Block)
_CLASSREGISTER_AddCtor(int , const BM_Material & , const String & )
_END_REGISTER_CLASS()


